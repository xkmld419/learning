#include "AllocateBalance.h"
#include "AllocateBalanceSql.h"

#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"
#include "errorcode_pay.h"

static vector<R_AllocateBalanceSvc *> g_vRAllocateBalanceSvc;
static vector<R_AllocateBalanceSvc *>::iterator g_itr1;

static vector<R_RequestDebitAcctItem *> g_vRRequestDebitAcctItem;
static vector<R_RequestDebitAcctItem *>::iterator g_itr2;

extern TimesTenCMD *m_poBalance;

AllocateBalance::AllocateBalance()
{
		m_poSql = NULL;
		m_poUnpack = NULL;
		m_poPack = NULL;
		m_paymentInfo = new PaymentInfo;
		m_dccOperation = new DccOperation;
}

AllocateBalance::~AllocateBalance()
{
	if (m_poSql != NULL)
		delete m_poSql;
	if (m_paymentInfo != NULL)
		delete m_paymentInfo;
	if (m_dccOperation != NULL)
		delete m_dccOperation;
}

int AllocateBalance::init(ABMException &oExp)
{
	if ((m_poSql = new AllocateBalanceSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "AllocateBalance::init malloc failed!");
		return -1;
	}
	return 0;
}

int AllocateBalance::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
	__DEBUG_LOG0_(0, "=========>测试：AllocateBalance::deal 请求服务开始");
	
	int iRet=0;
	int iSize=0;

	m_poUnpack = (AllocateBalanceCCR *)pCCR;
	m_poPack = (AllocateBalanceCCA *)pCCA;
	
	m_poPack->clear();
	g_vRAllocateBalanceSvc.clear();
	g_vRRequestDebitAcctItem.clear();
	
	A_AllocateBalanceSvc oSvc={0} ;

	// 将收到的DCC包，消息头入TT
	struct struDccHead receiveDccHead;
	char dccType[1];	// DCC消息类型
	strcpy(dccType,"R"); 
	long lDccRecordSeq=0L;
	try 
	{
		// 取DCC_INFO_RECORD_SEQ
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		if(iRet !=0)
		{
			__DEBUG_LOG0_(0, "[被动划拨-ReverseDeductBalanc]:查询SEQ失败!");
			throw GET_DCC_INFO_RECORD_SEQ_ERR;	
		}
		
		// dcc包消息头信息
		__DEBUG_LOG0_(0, "中心ABM余额被动划拨,接收CCR包,消息头入TT!");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存接收的CCR包消息头信息失败");
			throw SAVE_RECEVIE_DCC_CCR_ERR;
		}
		
		__DEBUG_LOG0_(0, "中心ABM被动余额划拨,CCR消息包解包!");
		//消息解包 - 第一层
		if ((iSize=m_poUnpack->getR_AllocateBalanceSvc(g_vRAllocateBalanceSvc)) == 0 )
		{
			__DEBUG_LOG0_(0, "中心ABM余额被动划拨::deal 请求服务消息包为NULL");
			m_poPack->setRltCode(ECODE_NOMSG);
			throw DEDUCT_UNPACK_NOMSG;
		}
		else if (iSize < 0)
		{
			__DEBUG_LOG0_(0, "中心ABM余额被动划拨::deal 消息解包出错!");
			m_poPack->setRltCode(ECODE_UNPACK_FAIL);
			throw DEDUCT_UNPACK_ERR;
		}
	
		// 查询余额划拨规则 
		__DEBUG_LOG0_(0, "查询余额划拨规则!");	
		
		// 处理第一层信息
		for (g_itr1=g_vRAllocateBalanceSvc.begin(); g_itr1!=g_vRAllocateBalanceSvc.end(); ++g_itr1)
		{
			// 根据SVC解detail包 - 第二层
			if ((iSize=m_poUnpack->getR_RequestDebitAcctItem(*g_itr1,g_vRRequestDebitAcctItem)) == 0 )
			{
				__DEBUG_LOG0_(0, "AllocateBalance::deal 请求明细消息包为NULL");
				m_poPack->setRltCode(ECODE_NOMSG);
				throw DEDUCT_UNPACK_NOMSG;
			}
			else if (iSize < 0)
			{
				__DEBUG_LOG0_(0, "AllocateBalance::deal 消息解包出错!");
				m_poPack->setRltCode(ECODE_UNPACK_FAIL);
				throw DEDUCT_UNPACK_ERR;
			}
			 
			__DEBUG_LOG0_(0, "测试:处理第一层信息........");	
			// 处理	
			/*
			if (_deal(*g_itr1,g_vRRequestDebitAcctItem) != 0)
			break;
			*/
			iRet = _deal(*g_itr1,g_vRRequestDebitAcctItem);
			// 对主动余额划出操作结果进行校验，赋错误码	
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "中心被动余额划出操作失败");
				throw iRet;
			}
		}
		//return iRet;
	}
	catch(TTStatus oSt)
	{
		iRet = DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "AllocateBalance::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value)
	{
		iRet=value;
		__DEBUG_LOG1_(0, "中心ABM被动余额划拨失败:: 失败信息 value=%d", value);
	}
	if(iRet != 0)
	{
		// 打返回CCA包,返回错误码
		oSvc.m_svcResultCode=iRet;
		if (!m_poPack->addResBalanceSvcData(oSvc))
		{
			__DEBUG_LOG0_(0, "AllocateBalance:: _deal m_poPack 打包失败!");
			m_poPack->clear();
			m_poPack->setRltCode(ECODE_PACK_FAIL);
		}
		__DEBUG_LOG0_(0, "中心ABM被动余额划拨失败,iRet=[%d]",oSvc.m_svcResultCode);
		
	}
	return iRet;
}

/*
 * 余额划拨处理操作
 * ---查询余额划拨规则及校验
 * ---余额划拨相当于帐务中的提取操作
 * ---
 * ---做业务流水记录、更新余额来源记录、更新余额帐本记录
 */
int AllocateBalance::_deal(R_AllocateBalanceSvc *pSvcIn,vector<R_RequestDebitAcctItem *> g_vRRequestDebitAcctItem)
{
	
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "========================余额划拨处理=======================");
	__DEBUG_LOG0_(0, "=====>start,取第一层CCR服务信息:");
	
	__DEBUG_LOG1_(0, "请求流水：m_requestId:%s",pSvcIn->m_requestId);
	__DEBUG_LOG1_(0, "用户号码：m_servNbr：%s",pSvcIn->m_servNbr);
	__DEBUG_LOG1_(0, "号码类型：m_nbrType：%d",pSvcIn->m_nbrType);
	__DEBUG_LOG1_(0, "用户属性：m_servAttr：%d",pSvcIn->m_servAttr);
	
	__DEBUG_LOG1_(0, "电话区号：m_areaCode：%s",pSvcIn->m_areaCode);
	
	if(pSvcIn->m_nbrType==2)
	{
		if(!(pSvcIn->m_servAttr==2 || pSvcIn->m_servAttr==99))
			snprintf(pSvcIn->m_servNbr,sizeof(pSvcIn->m_servNbr),"%s%s\0",pSvcIn->m_areaCode,pSvcIn->m_servNbr);
		__DEBUG_LOG1_(0, "带区号的用户号码：m_servNbr：%s",pSvcIn->m_servNbr);
	}
	
	ABMException oExp;
		
	int iRet=0;
	char sBuffer[256];
	
	PaymentInfoALLCCR pPaymentInfoCCR={0};
	PaymentInfoALLCCA pPaymentInfoCCA={0};
	
	// 划拨CCR信息准备	
	strcpy(pPaymentInfoCCR.m_sOperateSeq,pSvcIn->m_requestId);		// 请求流水号
	strcpy(pPaymentInfoCCR.m_sDestinationAccount,pSvcIn->m_servNbr);	// 用户号码
	pPaymentInfoCCR.m_iDestinationIDtype = pSvcIn->m_nbrType;		// 号码类型
	//strcpy(pPaymentInfoCCR.m_sDestinationIDtype,pSvcIn->m_nbrType);	// 号码类型
	pPaymentInfoCCR.m_iDestinationAttr = pSvcIn->m_servAttr;		// 用户属性
	strcpy(pPaymentInfoCCR.m_sAreaCode,pSvcIn->m_areaCode);			// 电话区号
	pPaymentInfoCCR.m_iStaffID = pSvcIn->m_staffId;				// 操作员ID
	strcpy(pPaymentInfoCCR.m_sDeductDate,pSvcIn->m_deductDate);		// 扣费时间
	//pPaymentInfoCCR.m_iBalance_unit = pSvcIn->m_balanceUnit;		// 余额单位-分
	strcpy(pPaymentInfoCCR.m_sMicropayType,"0");
	strcpy(pPaymentInfoCCR.m_sCapabilityCode,"0004");
	
			
	// 发起扣费ABM标识
	A_AllocateBalanceSvc oSvc={0} ;	// CCA 响应服务信息
	A_ResponseDebitAcctItem oData={0} ;	// CCA 响应扣费帐目明细信息
	vector<struct A_ResponseDebitAcctItem> vResAcctItem;
		
	// 根据CCR中的用户号码和号码类型查询用户、帐户属性信息
	ServAcctInfo oServAcct={0};
	
	bool bHaveRules = true;
	try
	{	
		__DEBUG_LOG1_(0, "=====>查询用户信息：m_servNbr=%s.",pSvcIn->m_servNbr);
		__DEBUG_LOG1_(0, "=====>查询用户信息：m_nbrType=%d.",pSvcIn->m_nbrType);
		iRet = m_poSql->preQryServAcctInfo(pSvcIn->m_servNbr,pSvcIn->m_nbrType,oServAcct,oExp);
		if(iRet != 0 )
		{
			if(iRet == 1)
			{
				__DEBUG_LOG0_(0, "=====>用户信息不存在");
				throw DEDUCT_SERV_NOT_EXIST;
			}
			__DEBUG_LOG0_(0, "=====>用户信息查询失败");
			throw DEDUCT_SERV_QRY_ERR;
		}
		// 用户信息查询结果
		__DEBUG_LOG1_(0, "=====>用户基本信息,帐户标识：ACCT_ID=%d.",oServAcct.m_acctId);
		__DEBUG_LOG1_(0, "=====>用户基本信息,用户标识：SERV_ID=%d.",oServAcct.m_servId);
		__DEBUG_LOG1_(0, "=====>用户基本信息,用户号码：ACCT_NBR=%s.",oServAcct.m_servNbr);
		
		pPaymentInfoCCR.m_lServID = oServAcct.m_servId;
		
		int iCnt = 0;
		TransRule oTransRule={0};	// 规则配置
		TransRule oTransRuleDay={0};	// 日发生
		TransRule oTransRuleMonth={0};	// 月发生
		for (g_itr2=g_vRRequestDebitAcctItem.begin(); g_itr2!=g_vRRequestDebitAcctItem.end(); ++g_itr2)
		{	
			__DEBUG_LOG0_(0, "=====>start,余额划拨业务逻辑处理：");
			
			// 余额划拨规则查询，校验	
			iRet = m_poSql->preQryDeductRule(oServAcct.m_acctId,oTransRule,oExp);
			if(iRet != 0 )
			{
				if(iRet == 1)
				{
					__DEBUG_LOG0_(0, "=====>用户余额划拨规则不存在。");
					bHaveRules =false;
					// throw TRANSFER_RULUES_NO_EXIT;
				}
				else
				{
					__DEBUG_LOG0_(0, "=====>用户余额划拨规则查询失败");
					throw TRANSFER_RULUES_QRY_ERR;
				}
			}
			__DEBUG_LOG1_(0, "=====>余额划拨规则信息,帐户标识：ACCT_ID=%d.",oTransRule.lAcctId);
			__DEBUG_LOG1_(0, "=====>余额划拨规则信息,日划拨频度：lDayTransFreq=%d.",oTransRule.lDayTransFreq);
			__DEBUG_LOG1_(0, "=====>余额划拨规则信息,日划拨限额：lTransLimit=%d.",oTransRule.lTransLimit);
			__DEBUG_LOG1_(0, "=====>余额划拨规则信息,月划拨频度：lMonthTransLimit=%d.",oTransRule.lMonthTransLimit);
				
			// 统计当前、当月发生划拨次数及划拨额度
			iRet = m_poSql->preQryServDayDeductInfo(oServAcct.m_servId,oTransRuleDay,oTransRuleMonth,oExp);
			__DEBUG_LOG1_(0, "=====>用户已划拨信息，当天次数：lDayTransFreq=%d.",oTransRuleDay.lDayTransFreq);
			__DEBUG_LOG1_(0, "=====>用户已划拨信息，当天金额：lTransLimit=%d.",oTransRuleDay.lTransLimit);	
			__DEBUG_LOG1_(0, "=====>用户已划拨信息，当月金额：lMonthTransLimit=%d.",oTransRuleMonth.lMonthTransLimit);
			
			if(bHaveRules)
			{// 存在规则,进行校验,无规则，不校验	
				// 余额划拨规则校验
				__DEBUG_LOG0_(0, "余额划拨规则校验");
				if(oTransRuleDay.lDayTransFreq >= oTransRule.lDayTransFreq)
				{	// 当天已发生次数 > 每天划拨频度
					__DEBUG_LOG0_(0, "当天已发生次数 > 每天划拨频度");
					throw OVER_DAY_TRANS_FREQ;
				}
				else if(oTransRuleDay.lTransLimit >= oTransRule.lTransLimit)
				{	// 当天天已发生金额 > 每天划拨限额
					__DEBUG_LOG0_(0, "当天天已发生金额 > 每天划拨限额");
					throw OVER_DAY_TRANS_LIMIT;
				}
				else if(oTransRuleMonth.lMonthTransLimit >= oTransRule.lMonthTransLimit)
				{	// 当月已发生金额 > 每月划拨限额
					
					__DEBUG_LOG0_(0, "当月已发生金额 > 每月划拨限额");
					throw OVER_MONTH_TRANS_LIMIT;
				}
			}	
			__DEBUG_LOG0_(0, "=====>end,余额划拨业务逻辑处理.");
			
			// 测试代码：准备返回CCA数据：第一层服务信息
			strcpy(oSvc.m_responseId, "6684801");	// 响应流水
			oSvc.m_svcResultCode = 1;		// 业务级的结果代码
			oSvc.m_balanceUnit = 0;			// 余额单位-分
			oSvc.m_deductAmount = 10;		// 成功划拨金额
			oSvc.m_acctBalance = 0;			// 帐户剩余金额
			
			// 测试代码：准备返回CCA数据：第二层扣费帐目类型明细信息	
			oData.m_billingCycle=201105;		// 帐务周期YYYYMMDD	
			strcpy(oData.m_acctItemTypeId, "2");	// 帐目类型标识	
			oData.m_acctBalanceId = 888;		// 扣费帐本标识
			oData.m_deductAmount = 10;		// 帐本划拨金额
			oData.m_acctBalanceAmount = 10;		// 帐本剩余金额	
				
				
			
			//strcpy(pPaymentInfoCCR.m_sOperateSeq,"66848001");	// 请求流水号
			pPaymentInfoCCR.m_iStaffID = 66848;
			strcpy(pPaymentInfoCCR.m_sDestinationAccount,oServAcct.m_servNbr);
			strcpy(pPaymentInfoCCR.m_sDestinationtype,"2");
			pPaymentInfoCCR.m_iDestinationAttr = 99;
	
			pPaymentInfoCCR.m_lCharge_type=0;
			pPaymentInfoCCR.m_iBalance_unit=0;
			strcpy(pPaymentInfoCCR.m_sAreaCode,"025");
			strcpy(pPaymentInfoCCR.m_sDescription,"余额划拨");
			
			pPaymentInfoCCR.m_lPayAmount = 0 - (*g_itr2)->m_deductAmount;		// 划拨金额，付款记录记录负金额
			strcpy(pPaymentInfoCCR.m_sAcctItemTypeID,(*g_itr2)->m_acctItemTypeId);	// 账目类型
			pPaymentInfoCCR.m_lCharge_type = (*g_itr2)->m_chargeType;		// 必须一次扣完
			
			// 划拨业务处理
			__DEBUG_LOG0_(0, "=====>start,m_paymentInfo->RecodePaymentInfo.");
			
			if((*g_itr2)->m_deductAmount <= 0)
			{
				__DEBUG_LOG0_(0, "划拨金额小于等于0,无效.");
				throw DEDUCT_AMOUNT_ERR;
			}
			
			// 调用业务逻辑处理
			strcpy(pPaymentInfoCCR.m_sOrderState,"C0C");	
			iRet = m_paymentInfo->DeductBalanceInfo(pPaymentInfoCCR,pPaymentInfoCCA);
			
			if(iRet!=0)
			{	// 余额帐本金额不足
				__DEBUG_LOG0_(0, "=====>start,m_paymentInfo->RecodePaymentInfo.");
				throw BALANCE_NOT_DEDUCT;
			}
			__DEBUG_LOG0_(0, "=====>end,m_paymentInfo->RecodePaymentInfo.");
			if(iCnt==0)
			{	// CCA返回信息赋值-响应服务,只记录一次
				strcpy(oSvc.m_responseId,pPaymentInfoCCR.m_sOperateSeq);	// 响应流水
				oSvc.m_svcResultCode = pPaymentInfoCCA.m_iSvcResultCode;	// 业务级的结果代码
				oSvc.m_balanceUnit = pPaymentInfoCCR.m_iBalance_unit;		// 余额单位-分
				oSvc.m_deductAmount = pPaymentInfoCCA.m_lDeductAmountAll;	// 成功划拨金额
				oSvc.m_acctBalance = pPaymentInfoCCA.m_lAcctBalance;		// 帐户剩余金额
				
				__DEBUG_LOG1_(0, "CCA返回信息,响应流水：[%s]",oSvc.m_responseId);
				__DEBUG_LOG1_(0, "CCA返回信息,业务级的结果代码：[%d]",oSvc.m_svcResultCode);
				__DEBUG_LOG1_(0, "CCA返回信息,余额单位：[%d]",oSvc.m_balanceUnit);
				__DEBUG_LOG1_(0, "CCA返回信息,成功划拨金额：[%ld]",oSvc.m_deductAmount);
				__DEBUG_LOG1_(0, "CCA返回信息,帐户剩余金额：[%ld]",oSvc.m_acctBalance);
				__DEBUG_LOG1_(0, "CCA返回信息,AVP code 错误码：[%s]",oSvc.m_sParaFieldResult);
			}
			iCnt++;
			
			// CCA返回信息赋值-扣费明细
			oData.m_billingCycle=(*g_itr2)->m_billingCycle;			// 帐务周期YYYYMM	
			strcpy(oData.m_acctItemTypeId,(*g_itr2)->m_acctItemTypeId);	// 帐目类型标识	
			oData.m_acctBalanceId = pPaymentInfoCCA.m_lAcctBalanceId;	// 扣费帐本标识
			oData.m_deductAmount = pPaymentInfoCCA.m_lDeductAmountAll;	// 帐本划拨金额
			oData.m_acctBalanceAmount = pPaymentInfoCCA.m_lAcctBalanceAmount;	// 帐本剩余金额	
			//strcpy(oData.m_sParaFieldResult,"0");	// AVP code - Error	// 测试默认写死
			
			vResAcctItem.push_back(oData);
			__DEBUG_LOG1_(0, "CCA返回信息,帐务周期YYYYMM：[%d]",oData.m_billingCycle);
			__DEBUG_LOG1_(0, "CCA返回信息,帐目类型标识：[%s]",oData.m_acctItemTypeId);
			__DEBUG_LOG1_(0, "CCA返回信息,扣费帐本标识：[%d]",oData.m_acctBalanceId);
			__DEBUG_LOG1_(0, "CCA返回信息,帐本划拨金额：[%d]",oData.m_deductAmount);
			__DEBUG_LOG1_(0, "CCA返回信息,帐本剩余金额：[%d]",oData.m_acctBalanceAmount);
		}
		
		// 打包 - 第一层服务信息
		if (!m_poPack->addResBalanceSvcData(oSvc))
		{
			__DEBUG_LOG0_(0, "AllocateBalance:: _deal m_poPack 打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw DEDUCT_PACK_ERR;
		}
		// 测试，打包后，oSvc数据
		__DEBUG_LOG1_(0, "第一层服务CCA打包完成,oSvc,responseId:[%s]",oSvc.m_responseId);
		
		// 打包 - 第二层扣费明细
		for(int j=0;j<vResAcctItem.size();j++)
		{
			if (!m_poPack->addResDebitAcctData(vResAcctItem[j]))
			{
				__DEBUG_LOG0_(0, "AllocateBalance:: _deal m_poPack 打包失败!");
				m_poPack->setRltCode(ECODE_PACK_FAIL);
				throw DEDUCT_PACK_ERR;
			}	
		}
		__DEBUG_LOG0_(0, "第二层服务CCA打包完成.");
	}
	catch(ABMException &oExp)
	{
		iRet = DEDUCT_TT_ERR;
		__DEBUG_LOG0_(0, "ABMException....");
	}
	catch(int &value ) 
	{

		iRet=value;
		oSvc.m_svcResultCode=iRet;
		__DEBUG_LOG1_(0, "CCA,业务级的结果代码iRet:[%d]",iRet);
	}

	return iRet;	
}

