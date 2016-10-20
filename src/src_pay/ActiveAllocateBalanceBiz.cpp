// ActiveAllocateBalanceBiz.cpp 
#include "ActiveAllocateBalanceBiz.h"
#include "AllocateBalanceSql.h"

#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"

static vector<ResBalanceSvc *> g_vResBalanceSvc;    // 主动划拨CCA 
static vector<ResBalanceSvc *>::iterator g_itr1;

static vector<A_AllocateBalanceSvc *> g_vAAllocateBalanceSvc;	// 被动划拨CCA服务请求	
static vector<A_AllocateBalanceSvc *>::iterator g_itr2;

static vector<A_ResponseDebitAcctItem *> g_vAResponseDebitAcctItem; // 被动划拨CCA帐目明细
static vector<A_ResponseDebitAcctItem *>::iterator g_itr3;

extern TimesTenCMD *m_poBalance;

static char cPreSessionId[100]={0};

static long lServId=0L;

static char sCCASessionId[100]={0};

ActiveAllocateBalanceBiz::ActiveAllocateBalanceBiz()
{
		m_poSql = NULL;
		m_poUnpack = NULL;
		m_poActiveUnpack = NULL;
		m_poPack = NULL;
		m_paymentInfo = new PaymentInfo;
		
		m_dccOperation = new DccOperation;	
		
		m_depositInfo = new DepositInfo;
}

ActiveAllocateBalanceBiz::~ActiveAllocateBalanceBiz()
{
	if (m_poSql != NULL)
		delete m_poSql;	
	if (m_paymentInfo != NULL)
		delete m_paymentInfo;
		
	if (m_dccOperation != NULL)
		delete m_dccOperation;
	if (m_depositInfo != NULL)
		delete m_depositInfo;
}

int ActiveAllocateBalanceBiz::init(ABMException &oExp)
{
	if ((m_poSql = new AllocateBalanceSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "ActiveAllocateBalanceBiz::init malloc failed!");
		return -1;
	}
	return 0;
}

int ActiveAllocateBalanceBiz::deal(ABMCCR *pRecivePack, ABMCCA *pSendPack)
{
	__DEBUG_LOG0_(0, "=========>测试：ActiveAllocateBalanceBiz::deal 响应服务开始");
	__DEBUG_LOG0_(0, "=========>1：接收省ABM返回的CCA...");
	int iRet=0;
	int iBalanceTransFlag=2; // 余额划拨类型
	
	// 获取pRecivePack的公共CCA消息头
	char sSessionId[101]={0};
	struDccHead ccaHead;

	char dccType[1];	// DCC消息类型
	strcpy(dccType,"A"); 
	
	long lDccRecordSeq=0L; // 流水号，主键
	
	ReqPlatformSvc reqSvc;
	ReqPlatformPayInfo reqPayInfo;
	
	try
	{
		// 取消息包的消息头信息
		memcpy((void *)&ccaHead,(void *)(pRecivePack->m_sDccBuf),sizeof(ccaHead));
		// 收到CCA包Session-Id
		__DEBUG_LOG1_(0, "=======>收到省ABM返回的CCA的消息头，SESSION_ID:[%s]",ccaHead.m_sSessionId);
		
		// 取出CCA中的SessionId,业务处理结束后更新DCC_INFO_RECORD用
		strcpy(sCCASessionId,ccaHead.m_sSessionId);
		
		//根据CCA响应流水去找自服务平台请求CCR的会话ID
		iRet = m_dccOperation->qryPreSessionId(ccaHead.m_sSessionId,sSessionId);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "查询CCA会话ID的源Session Id失败.");
			throw QRY_PRE_SESSION_ID_ERR;
		}
		__DEBUG_LOG1_(0, "========>自服务平台发起CCR的SESSION_ID:[%s]",sSessionId);
		strcpy(cPreSessionId,sSessionId);
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "查询DCC_INFO_RECORD_SEQ失败");
			throw SAVE_RECEIVE_DCC_CCA_ERR;
		}
		
		// 将收到的CCA消息头入TT
		iRet = m_dccOperation->insertDccInfo(ccaHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存自服务平台CCA包消息头信息失败");
			throw SAVE_RECEIVE_DCC_CCA_ERR;
		}
		
		// 根据sessionId取TT对应的CCR信息
		__DEBUG_LOG0_(0, "=========>根据sessionId取TT对应的CCR[自服务平台CCR请求信息]信息:");
	
		iRet = m_dccOperation->getCCRInfo(sSessionId,reqSvc,reqPayInfo);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "获取自服务平台CCR请求业务信息失败");
			throw SAVE_RECEIVE_DCC_CCA_ERR;
		}
		__DEBUG_LOG0_(0, "getCCRInfo,iRet=[%d]",iRet);
		// 余额划拨类型
		iBalanceTransFlag = reqPayInfo.m_iBalanceTransFlag;
		
		// 根据用户号码获取用户标识
		ServAcctInfo oServAcct;
		ABMException oExp;
		__DEBUG_LOG1_(0, "用户号码:[%s]",reqPayInfo.m_sDestinationAccount);
		iRet = m_poSql->preQryServAcctInfo(reqPayInfo.m_sDestinationAccount,2,oServAcct,oExp);
		if(iRet !=0)
		{
			__DEBUG_LOG0_(0, "根据用户号码查询用户资料失败");
			throw 13847;
		}
		lServId = oServAcct.m_servId;
		__DEBUG_LOG1_(0, "用户标识Serv_ID:[%ld]",lServId);
		/*
		__DEBUG_LOG0_(0, "=========>自服平台请求CCR明细:");
		__DEBUG_LOG1_(0, "请求流水：m_requestId:[%s]",reqSvc.m_requestId);
		__DEBUG_LOG1_(0, "请求时间：m_requestTime:[%s]",reqSvc.m_requestTime);
		
		__DEBUG_LOG1_(0, "用户号码：m_sDestinationAccount:[%s]",reqPayInfo.m_sDestinationAccount);
		__DEBUG_LOG1_(0, "标识类型：m_iDestinationType:[%d]",reqPayInfo.m_iDestinationType);
		__DEBUG_LOG1_(0, "用户属性：m_iDestinationAttr:[%d]",reqPayInfo.m_iDestinationAttr);
		__DEBUG_LOG1_(0, "业务平台标识：m_sSvcPlatformId:[%s]",reqPayInfo.m_sSvcPlatformId);
		__DEBUG_LOG1_(0, "支付密码：m_sPayPassword:[%s]",reqPayInfo.m_sPayPassword);
		__DEBUG_LOG1_(0, "余额划拨类型：m_iBalanceTransFlag:[%d]",reqPayInfo.m_iBalanceTransFlag);
		__DEBUG_LOG1_(0, "余额的单位类型：m_iUnitTypeId:[%d]",reqPayInfo.m_iUnitTypeId);
		__DEBUG_LOG1_(0, "需划拨金额：m_lDeductAmount:[%ld]",reqPayInfo.m_lDeductAmount);
		*/

		// 根据查询出的余额划拨类型，解包处理
		m_poPack = (PlatformDeductBalanceCCA *)pSendPack;
		if(iBalanceTransFlag == 1)
		{
			m_poUnpack = (AllocateBalanceCCA *)pRecivePack;		  // 解包-响应CCA,省到全国
			iRet = deductBalanceIn(reqSvc,reqPayInfo);
			// 对划拨充值操作结果进行校验，赋错误码	
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "划拨充值操作失败",);
				throw DEDUCT_BIZ1_IN_ERR;
			}	
		}
		else if(iBalanceTransFlag == 2)
		{
			m_poActiveUnpack = (ActiveAllocateBalanceCCA *)pRecivePack; // 解包-响应CCA,全国到省	
			iRet = deductBalanceOut(reqSvc,reqPayInfo);
			// 对主动余额划出操作结果进行校验，赋错误码	
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "主动余额划出操作失败");
				throw DEDUCT_BIZ2_IN_ERR;
			}
		}
	}
	catch(TTStatus oSt)
	{
		iRet = ACTIVE_DEDUCT_BIZ_TT_ERR;
		__DEBUG_LOG1_(0, "ActiveAllocateBalanceBiz::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "ActiveAllocateBalanceBiz::deal value=%d",value);
	}
	
	if(iRet != 0)
	{
		// 打返回包，赋错误码
		//m_poPack->clear();
		//m_poPack->setRltCode(ECODE_TT_FAIL);
		__DEBUG_LOG0_(0, "ActiveAllocateBalanceBiz::deal 主动收发CCA失败");
	}
	return iRet;
}

// 划拨类型操作
/*
int ActiveAllocateBalanceBiz::_deal(ReqPlatformSvc *pSvcIn,vector<ReqPlatformPayInfo *> g_vReqPlatformPayInfo)
{
	
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "========================余额划拨处理=======================");
	__DEBUG_LOG0_(0, "=========>取自服务平台请求CCR第一层服务信息:");
	__DEBUG_LOG1_(0, "m_requestId:%s",pSvcIn->m_requestId);
	__DEBUG_LOG1_(0, "m_requestTime:%s",pSvcIn->m_requestTime);
	
	int iRet=0;
	int iBalanceTransFlag; // 余额划拨类型
	
	for (g_itr2=g_vReqPlatformPayInfo.begin(); g_itr2!=g_vReqPlatformPayInfo.end(); ++g_itr2)
	{
	
		__DEBUG_LOG0_(0, "=========>取自服务平台请求CCR第二层支付信息：");
		 iBalanceTransFlag = (*g_itr2)->m_iBalanceTransFlag;
		__DEBUG_LOG1_(0, "=========>余额划拨类型：[%d](1-省到全国，2-全国到省)",iBalanceTransFlag);
		 switch(iBalanceTransFlag)
		 {
		 	case 1:
		 		// 省到全国
		 		// 省ABM被动划拨
		 		// 中心ABM做划拨充值业务
		 		__DEBUG_LOG0_(0, "=========>余额划拨，省到全国业务处理。");
		 		iRet = deductBalanceIn(pSvcIn,*g_itr2);
		 		if(iRet != 0)
		 		{
		 			__DEBUG_LOG0_(0, "=========>余额划拨，省到全国业务处理失败");
		 			// 组返回CCA
		 		}
		 		break;
		 	case 2:
		 		// 全国到省
		 		// 中心ABM主动划出
		 		// 中心ABM做余额划出业务
		 		__DEBUG_LOG0_(0, "=========>余额划拨，全国到省业务处理。");
		 		 iRet = deductBalanceOut(pSvcIn,*g_itr2);
		 		 if(iRet !=0)
		 		 {
		 		 	__DEBUG_LOG0_(0, "=========>余额划拨，全国到省业务处理。");
		 			// 组返回CCA
		 		 }
		 		break;
		 }
	}

	return iRet;	
}
*/

// 划拨充值，业务逻辑处理
int ActiveAllocateBalanceBiz::deductBalanceIn(ReqPlatformSvc &reqSvc,ReqPlatformPayInfo &reqPayInfo)
{
	int iRet=0;
	int iSize=0;
	char dccType[1];	// DCC消息类型
	strcpy(dccType,"A"); 
	
	// 中心ABM响应自服务平台的CCA返回信息
	ResPlatformSvc oCCASvc;
	ResPlatformPayInfo oCCAPayInfo;
	ResPlatformAcctBalance oCCABalanceDetail;
	
	// 
	long lSessionIdSeq=0L;	// SessionId SEQ
	struct struDccHead Head;
	char sSessionId[64];
	time_t tTime;
	
	long lDccRecordSeq=0L;
	char sResponseTime[32]={0};
	g_toPlatformFlag = true; // 发往自服务平台的消息
	try
	{
		// 解CCA包
		__DEBUG_LOG0_(0, "========>解省ABM返回的CCA包-被动划拨请求CCA");
		
		//消息解包 - 第一层
		g_vAAllocateBalanceSvc.clear();
		if ((iSize=m_poUnpack->getResBalanceSvcData(g_vAAllocateBalanceSvc)) == 0 )
		{
			__DEBUG_LOG0_(0, "ActiveAllocateBalanceBiz::deductBalanceIn 省ABM返回CCA，请求服务消息包为NULL");
			m_poPack->setRltCode(ECODE_NOMSG);
			throw  RETURN_CCA_UNPACK_NOMSG;
		}
		else if (iSize < 0)
		{
			__DEBUG_LOG0_(0, "ActiveAllocateBalanceBiz::deductBalanceIn 消息解包出错!");
			m_poPack->setRltCode(ECODE_UNPACK_FAIL);
			throw RETURN_CCA_UNPACK_ERR;
		}
		__DEBUG_LOG1_(0, "============>iSize=[%d],第一层信息解包的条数!",iSize);
		// 处理第一层信息
		for (g_itr2=g_vAAllocateBalanceSvc.begin(); g_itr2!=g_vAAllocateBalanceSvc.end(); ++g_itr2)
		{
			// 根据SVC解detail包 - 第二层
			if ((iSize=m_poUnpack->getAcctBalanceData(*g_itr2,g_vAResponseDebitAcctItem)) == 0 )
			{
				__DEBUG_LOG0_(0, "AllocateBalance::deal 请求明细消息包为NULL");
				m_poPack->setRltCode(ECODE_NOMSG);
				throw RETURN_CCA_UNPACK_NOMSG;
			}
			else if (iSize < 0)
			{
				__DEBUG_LOG0_(0, "AllocateBalance::deal 消息解包出错!");
				m_poPack->setRltCode(ECODE_UNPACK_FAIL);
				throw RETURN_CCA_UNPACK_ERR;
			}
			
			__DEBUG_LOG0_(0, "=========>处理省ABM返回的CCA信息.执行划拨充值业务处理");			
			// 处理					
			__DEBUG_LOG1_(0, "省ABM返回CCA-响应流水：[%s]",(*g_itr2)->m_responseId);
			__DEBUG_LOG1_(0, "省ABM返回CCA-业务级的结果代码：[%d]", (*g_itr2)->m_svcResultCode);
			__DEBUG_LOG1_(0, "省ABM返回CCA-扣费帐务标识：[%ld]", (*g_itr2)->m_lAcctId);
			__DEBUG_LOG1_(0, "省ABM返回CCA-余额单位：[%d]", (*g_itr2)->m_balanceUnit);
			__DEBUG_LOG1_(0, "省ABM返回CCA-成功划拨金额：[%ld]", (*g_itr2)->m_deductAmount);
			__DEBUG_LOG1_(0, "省ABM返回CCA-帐户剩余金额：[%ld]", (*g_itr2)->m_acctBalance);
			
			// 2011.7.14.更新扩展表ACTIVE_CCR_LOG表状态，根据响应流水更新状态为00A
			iRet = m_dccOperation->updateActiveCCR((*g_itr2)->m_responseId);
			if(iRet !=0 )
			{
				__DEBUG_LOG0_(0, "更新ACTIVE_CCR_LOG状态失败.");
				throw DEDUCT_OUT_ERR;
			}
			
			if((*g_itr2)->m_svcResultCode != 0)
			{
				__DEBUG_LOG0_(0, "省ABM返回CCA-业务级结果代码非0！");
				throw RETURN_BIZ_CODE_ERR;
			}
			
			oCCAPayInfo.m_lDeductAmount=(*g_itr2)->m_deductAmount; // 成功划拨的金额
			
			for (g_itr3=g_vAResponseDebitAcctItem.begin(); g_itr3!=g_vAResponseDebitAcctItem.end(); ++g_itr3)
			{	
				__DEBUG_LOG1_(0, "省ABM返回CCA-帐务周期YYYYMM：[%d]",(*g_itr3)->m_billingCycle);
				__DEBUG_LOG1_(0, "省ABM返回CCA-帐目类型标识：[%s]",(*g_itr3)->m_acctItemTypeId);
				__DEBUG_LOG1_(0, "省ABM返回CCA-帐扣费账本标识：[%ld]",(*g_itr3)->m_acctBalanceId);
				__DEBUG_LOG1_(0, "省ABM返回CCA-帐本划拨金额：[%ld]",(*g_itr3)->m_deductAmount);
				__DEBUG_LOG1_(0, "省ABM返回CCA-帐本剩余金额：[%ld]",(*g_itr3)->m_acctBalanceAmount);
			
				// TODO:根据CCA业务级的结果代码判断是否做划拨充值业务
				
				// CCA响应成功，做划拨充值业务
				__DEBUG_LOG0_(0, "========>CCA响应成功，做划拨充值业务");
				DepositServCondOne *pInOne = new DepositServCondOne;
				vector<DepositServCondTwo *> vInTwo;
				
				strcpy(pInOne->m_sReqSerial,(*g_itr2)->m_responseId);	// 充值请求流水-CCA响应流水
				//strcpy(pInOne->m_sBalanceType,"10");			// 余额类型
				pInOne->m_iBalanceType=10;
				pInOne->m_iDestinationBalanceUnit = (*g_itr2)->m_balanceUnit;// 余额单位     
				pInOne->m_iVoucherValue=(*g_itr3)->m_deductAmount;	// 划拨金额
				pInOne->m_iProlongDays = 30;				// 有效期-暂写死
				strcpy(pInOne->m_sDestinationType,"5BC");		// 号码类型
				strcpy(pInOne->m_sDestinationAccount,reqPayInfo.m_sDestinationAccount);	// 用户号码
				//strcpy(pInOne->m_sDestinationAccount,"18951765356");	// 用户号码-测试暂写死
				pInOne->m_iDestinationAttr=reqPayInfo.m_iDestinationAttr;// 用户属性
				pInOne->m_iDestinationAttr=2;				 // 测试，暂写死
				pInOne->m_lDestinationAttrDetail=0;
				strcpy(pInOne->m_sAreaCode,"010");			// 区号，暂写死
				
				iRet = m_depositInfo->_dealIn(pInOne,vInTwo);
				if(iRet !=0 )
				{
					__DEBUG_LOG0_(0, "划拨充值业务处理失败.");
					throw DEDUCT_IN_ERR;
				}
				
				// 更新DCC消息头信息-DCC_INFO_RECORD ,SessionId
				iRet = m_dccOperation->updateDccInfo(sCCASessionId);
				if(iRet !=0 )
				{
					__DEBUG_LOG0_(0, "更新DCC_INFO_RECORD状态失败.");
					throw DEDUCT_OUT_ERR;
				}
				
				oCCAPayInfo.m_iUnitTypeId = pInOne->m_iDestinationBalanceUnit; // 余额单位类型
				
				__DEBUG_LOG0_(0, "CCA响应成功，划拨充值业务结束.");
				// CCA响应成功，划拨充值业务结束
			}
			
			/*
			// 2011.7.14.更新扩展表ACTIVE_CCR_LOG表状态，根据响应流水更新状态为00A
			iRet = m_dccOperation->updateActiveCCR((*g_itr2)->m_responseId);
			if(iRet !=0 )
			{
				__DEBUG_LOG0_(0, "更新ACTIVE_CCR_LOG状态失败.");
				throw DEDUCT_OUT_ERR;
			}
			*/
		}
		
		/*
		for(int j=0;j<g_vAResponseDebitAcctItem.size();j++)
		{
			delete g_vAResponseDebitAcctItem[j];
			g_vAResponseDebitAcctItem[j]= NULL;
		}
		g_vAResponseDebitAcctItem.clear();
		
		for(int j=0;j<g_vAAllocateBalanceSvc.size();j++)
		{
			delete g_vAAllocateBalanceSvc[j];
			g_vAAllocateBalanceSvc[j]= NULL;
		}
		
		g_vAAllocateBalanceSvc.clear();
		*/
		
		oCCASvc.m_svcResultCode = 0;
		strcpy(oCCASvc.m_sParaFieldResult,"0");	// 错误信息
		strcpy(oCCASvc.m_responseId,reqSvc.m_requestId); // 返回CCA的响应流水，自服务平台的请求流水
		//strcpy(oCCASvc.m_responseId,"response-id-20110529");// 测试时写死
		
		tTime = time(NULL);
		tTime += 2209017599;
		long2date(tTime,sResponseTime);
		strcpy(oCCASvc.m_responseTime,sResponseTime);
		
		//strcpy(oCCASvc.m_responseTime,"20110529"); // TODO：待转换
		
		// 测试时暂写死xxxxxxxxxxxxxxxxxxxx
		oCCAPayInfo.m_iUnitTypeId = 2;
		oCCAPayInfo.m_lDeductAmount=10;
		oCCABalanceDetail.m_iBalanceSource = 1;
		oCCABalanceDetail.m_lBalanceAmount = 100;
		
		// Modify 2011.6.15,取消
		// 设置CCA消息头
		// 生成Session-id
		iRet = m_dccOperation->QueryPaySeq(lSessionIdSeq);
		if(iRet !=0 )
		{
			__DEBUG_LOG0_(0, "取Session-id序列失败.");
			throw GET_SESSION_ID_SEQ_ERR;
		}
		
		//tTime = time(NULL);
		// 2011.8.1
		snprintf(Head.m_sSessionId,sizeof(Head.m_sSessionId),"%s%010d%s%04d\0","ABM@001.ChinaTelecom.com;",tTime,";",lSessionIdSeq);
		
		strcpy(Head.m_sSessionId,cPreSessionId);
			
		__DEBUG_LOG1_(0, "返回CCA的SESSION_ID:[%s]",Head.m_sSessionId);
		
		strcpy(Head.m_sSrvTextId,"DeductFromABM.Micropayment@001.ChinaTelecom.com");
		strcpy(Head.m_sOutPlatform,"");
		strcpy(Head.m_sOrignHost,"ABM@001.ChinaTelecom.com"); // 2011.8.1
		strcpy(Head.m_sOrignRealm,"001.ChinaTelecom.com");
		
		// 获取请求划拨号码的归属地
		char sOrgId[5]={0};
		
		char sAccoutNbr[32]={0};
		char sLocalAreaId[5]={0};
		strcpy(sAccoutNbr,reqPayInfo.m_sDestinationAccount);
		//iRet = m_dccOperation->qryAccountOrgId(reqPayInfo.m_iDestinationAttr,sOrgId); // 测试后放开
		//iRet = m_dccOperation->qryAccountOrgId(sAccoutNbr,sOrgId); // 测试时写死
		iRet = m_dccOperation->qryAccountOrgId(sAccoutNbr,sOrgId,sLocalAreaId);
		if(iRet !=0 )
		{
			__DEBUG_LOG0_(0, "取号码所属机构代码失败.");
			throw GET_ACCT_NBR_ORGID_ERR;
		}
		
		snprintf(Head.m_sDestRealm,sizeof(Head.m_sDestRealm),"%s.ChinaTelecom.com",sOrgId);

		__DEBUG_LOG1_(0, "消息头，目标设备标识-Head.m_sDestRealm:[%s]",Head.m_sDestRealm);
		
		Head.m_iAuthAppId = 4;
		Head.m_iReqType = 4;
		Head.m_iReqNumber  = 0;
		Head.m_iResultCode = 0;
	
		memcpy((void *)(m_poPack->m_sDccBuf),(void *)&Head,sizeof(Head));
		
		//m_poPack->m_iCmdID = ABMCMD_ALLOCATE_BALANCE;
		//m_poPack->m_iCmdID = ABMCMD_PLATFORM_QUEUE;
		
		
		// 打CCA包，返回自服务平台oCCAPayInfo
		__DEBUG_LOG0_(0, "========>打CCA包，返回自服务平台");
		// CCA 第一层服务信息
		if (!m_poPack->addResPlatformSvc(oCCASvc))
		{
			__DEBUG_LOG0_(0, "ActiveAllocateBalanceBiz::deductBalanceIn 第一层CCA打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw DEDUCT_IN_PACK_ERR;
		}
		__DEBUG_LOG0_(0, "第一层服务CCA打包完成");
		

		// CCA 第三层明细信息
		if (!m_poPack->addResPlatformAcctBalance(oCCABalanceDetail))
		{
			__DEBUG_LOG0_(0, "ActiveAllocateBalanceBiz::deductBalanceIn 第三层CCA打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw DEDUCT_IN_PACK_ERR;
		}
		
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		if(iRet !=0 )
		{
			__DEBUG_LOG0_(0, "取DCC_INFO_RECORD_SEQ失败.");
			throw GET_DCC_INFO_RECORD_SEQ_ERR;
		}
		
		// 保存发出的CCR信息
		__DEBUG_LOG0_(0, "保存发出的DCC包信息入TT");
		iRet = m_dccOperation->insertDccInfo(Head,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存DCC业务信息失败");
			throw SAVE_SEND_DCC_CCA_ERR;
		}
		m_poPack->m_iCmdID = ABMCMD_PLATFORM_DEDUCT_BALANCE;
		
	}
	catch(TTStatus oSt)
	{
		iRet = ACTIVE_DEDUCT_BIZ_TT_ERR;
		__DEBUG_LOG1_(0, "AllocateBalance::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "AllocateBalance::deal value=%d",value);
	}	
	
	for(int j=0;j<g_vAResponseDebitAcctItem.size();j++)
	{
		delete g_vAResponseDebitAcctItem[j];
		g_vAResponseDebitAcctItem[j]= NULL;
	}
	g_vAResponseDebitAcctItem.clear();
	
	for(int j=0;j<g_vAAllocateBalanceSvc.size();j++)
	{
		delete g_vAAllocateBalanceSvc[j];
		g_vAAllocateBalanceSvc[j]= NULL;
	}
	
	g_vAAllocateBalanceSvc.clear();
	
	return iRet;
}

// 余额划出，业务逻辑处理
int ActiveAllocateBalanceBiz::deductBalanceOut(ReqPlatformSvc &reqSvc,ReqPlatformPayInfo &reqPayInfo)
{
	int iRet=0;
	int iSize=0;
	char dccType[1];	// DCC消息类型
	strcpy(dccType,"A"); 
	
	// CCA返回信息
	ResPlatformSvc oCCASvc;
	ResPlatformPayInfo oCCAPayInfo;
	ResPlatformAcctBalance oCCABalanceDetail;
	
	long lSessionIdSeq;
	struct struDccHead Head;
	char sSessionId[64];
	time_t tTime;
	
	long lDccRecordSeq=0L;
	
	char sResponseTime[32]={0};
	g_toPlatformFlag = true; // 发往自服务平台的消息
	try
	{
		
		// 解CCA包
		__DEBUG_LOG0_(0, "解省ABM返回的CCA包-主动划拨请求CCA");
		
		// CCA响应成功，做余额划出业务
		__DEBUG_LOG0_(0, "========>CCA响应成功，做余额划出业务");
		
		//消息解包 - 第一层
		if ((iSize=m_poActiveUnpack->getResBalanceSvc(g_vResBalanceSvc)) == 0 )
		{
			__DEBUG_LOG0_(0, "ActiveAllocateBalanceBiz::deductBalanceIn 中心ABM主动请求划出-省ABM返回CCA，请求服务消息包为NULL");
			m_poPack->setRltCode(ECODE_NOMSG);
			throw RETURN_CCA_UNPACK_NOMSG;
		}
		else if (iSize < 0)
		{
			__DEBUG_LOG0_(0, "ActiveAllocateBalanceBiz::deductBalanceOut 消息解包出错!");
			m_poPack->setRltCode(ECODE_UNPACK_FAIL);
			throw RETURN_CCA_UNPACK_ERR;
		}
		
		// 处理第一层信息
		__DEBUG_LOG1_(0, "业务结构体大小：size:[%d].",g_vResBalanceSvc.size());
		for (g_itr1=g_vResBalanceSvc.begin(); g_itr1!=g_vResBalanceSvc.end(); ++g_itr1)
		{ 
			__DEBUG_LOG0_(0, "=========>_deal.处理省ABM返回的CCA信息.执行主动划出业务处理");			
			// 处理							
			__DEBUG_LOG1_(0, "省ABM返回CCA-响应流水：[%s]",(*g_itr1)->m_responseId);
			__DEBUG_LOG1_(0, "省ABM返回CCA-业务级的结果代码：[%d]", (*g_itr1)->m_svcResultCode);
			__DEBUG_LOG1_(0, "省ABM返回CCA-错误信息：[%s]", (*g_itr1)->m_sParaFieldResult);
			
			__DEBUG_LOG1_(0, "省ABM返回CCA-响应时间：[%d]", (*g_itr1)->m_responseTime);
			__DEBUG_LOG1_(0, "省ABM返回CCA-余额单位：[%d]", (*g_itr1)->m_balanceUnit);
			__DEBUG_LOG1_(0, "省ABM返回CCA-划拨后省ABM帐户余额：[%ld]", (*g_itr1)->m_balanceAmount);
			__DEBUG_LOG1_(0, "省ABM返回CCA-余额类型：[%ld]", (*g_itr1)->m_blanceType);
			
			__DEBUG_LOG0_(0, "========>CCA响应成功，做余额划出业务");
			// 余额划拨规则校验
			
			// 2011.7.14.更新扩展表ACTIVE_CCR_LOG表状态，根据响应流水更新状态为00A
			iRet = m_dccOperation->updateActiveCCR((*g_itr1)->m_responseId);
			if(iRet !=0 )
			{
				__DEBUG_LOG0_(0, "更新ACTIVE_CCR_LOG状态失败.");
				throw DEDUCT_OUT_ERR;
			}
			
			if((*g_itr1)->m_svcResultCode != 0)
			{
				__DEBUG_LOG0_(0, "省ABM返回CCA-业务级结果代码非0！");
				throw RETURN_BIZ_CODE_ERR;
			}
			
			
			// 余额划拨操作
			PaymentInfoALLCCR pPaymentInfoCCR={0};
			PaymentInfoALLCCA pPaymentInfoCCA={0};
			
			// 划拨CCR信息准备	
			strcpy(pPaymentInfoCCR.m_sOperateSeq,reqSvc.m_requestId);			// 请求流水号
			strcpy(pPaymentInfoCCR.m_sDestinationAccount,reqPayInfo.m_sDestinationAccount);	// 用户号码
			pPaymentInfoCCR.m_iDestinationIDtype = 2;					// 号码类型
			pPaymentInfoCCR.m_iDestinationAttr = reqPayInfo.m_iDestinationAttr;		// 用户属性
			strcpy(pPaymentInfoCCR.m_sAreaCode,"010");					// 电话区号
			pPaymentInfoCCR.m_iStaffID = 66848;						// 操作员ID
			strcpy(pPaymentInfoCCR.m_sDeductDate,(*g_itr1)->m_responseTime);		// 扣费时间
			strcpy(pPaymentInfoCCR.m_sMicropayType,"0");
			strcpy(pPaymentInfoCCR.m_sCapabilityCode,"1004");
			strcpy(pPaymentInfoCCR.m_sDescription,"中心ABM主动划出");
			pPaymentInfoCCR.m_lPayAmount = 0-reqPayInfo.m_lDeductAmount;			// 需划拨金额
			//pPaymentInfoCCR.m_lServID = 161; // 暂写死
			pPaymentInfoCCR.m_lServID = lServId;
			
			// 调用业务逻辑处理	
			__DEBUG_LOG1_(0, "余额划出前，paymentInfoCCR需划拨金额：%ld.",pPaymentInfoCCR.m_lPayAmount);
			iRet = m_paymentInfo->DeductBalanceInfo(pPaymentInfoCCR,pPaymentInfoCCA);
			if(iRet !=0 )
			{
				__DEBUG_LOG0_(0, "主动划出业务处理失败.");
				throw DEDUCT_OUT_ERR;
			}
			
			// 更新DCC消息头信息-DCC_INFO_RECORD ,SessionId
			iRet = m_dccOperation->updateDccInfo(sCCASessionId);
			if(iRet !=0 )
			{
				__DEBUG_LOG0_(0, "更新DCC_INFO_RECORD状态失败.");
				throw DEDUCT_OUT_ERR;
			}
			
			/*
			// 2011.7.14.更新扩展表ACTIVE_CCR_LOG表状态，根据响应流水更新状态为00A
			iRet = m_dccOperation->updateActiveCCR((*g_itr1)->m_responseId);
			if(iRet !=0 )
			{
				__DEBUG_LOG0_(0, "更新ACTIVE_CCR_LOG状态失败.");
				throw DEDUCT_OUT_ERR;
			}
			*/
			
			
			// 返回CCA信息-合并到第一层结构
			//oCCAPayInfo.m_lDeductAmount=0-pPaymentInfoCCR.m_lPayAmount; // 成功划拨的金额
			//oCCAPayInfo.m_iUnitTypeId = (*g_itr1)->m_balanceUnit; // 余额单位类型
			
			oCCASvc.m_lDeductAmount=0-pPaymentInfoCCR.m_lPayAmount; // 成功划拨的金额
			oCCASvc.m_iUnitTypeId = (*g_itr1)->m_balanceUnit; // 余额单位类型
			
			__DEBUG_LOG0_(0, "========>返回自服务平台的CCA的余额单位类型:[%d]",oCCAPayInfo.m_iUnitTypeId);	
			__DEBUG_LOG0_(0, "========>主动余额划出业务处理结束");	
			
			oCCABalanceDetail.m_lBalanceAmount = pPaymentInfoCCA.m_lAcctBalanceAmount; // 余额帐本剩余金额
		}
		
		__DEBUG_LOG1_(0, "g_vResBalanceSvc.size()=[%d]!",g_vResBalanceSvc.size());
		
		
		
		oCCASvc.m_svcResultCode = 0;
		strcpy(oCCASvc.m_sParaFieldResult,"");	// 出错Avp Code
		
		strcpy(oCCASvc.m_responseId,reqSvc.m_requestId); // 返回CCA的响应流水，自服务平台的请求流水
		//strcpy(oCCASvc.m_responseId,"response-id-20110529");// 测试时写死
		tTime = time(NULL);
		tTime += 2209017599;
		long2date(tTime,sResponseTime);
		strcpy(oCCASvc.m_responseTime,sResponseTime);
		
		// 测试时暂写死。。。。。。。。
		/* 2011.7.11 去除
		oCCAPayInfo.m_iUnitTypeId = 2;
		oCCAPayInfo.m_lDeductAmount=10;
		oCCABalanceDetail.m_iBalanceSource = 1;  // 余额帐本来源
		oCCABalanceDetail.m_lBalanceAmount = 300;  // 余额帐本当前余额
		*/

		
		// 设置CCA消息头
		// Session-id - 返回自服务平台的Session需要根据请求流水号从DCC业务信息记录表中获取
		
		iRet = m_dccOperation->QueryPaySeq(lSessionIdSeq);
		
		// 2011.8.1
		snprintf(Head.m_sSessionId,sizeof(Head.m_sSessionId),"%s%010d%s%04d\0","ABM@001.ChinaTelecom.com;",tTime,";",lSessionIdSeq);
			
		__DEBUG_LOG1_(0, "SESSION_ID:[%s]",Head.m_sSessionId);
		
		strcpy(Head.m_sSrvTextId,"DeductFromABM.Micropayment@001.ChinaTelecom.com");
		strcpy(Head.m_sOutPlatform,""); // 外部平台，SGW用
		strcpy(Head.m_sOrignHost,"ABM@001.ChinaTelecom.com"); // 2011.8.1
		strcpy(Head.m_sOrignRealm,"001.ChinaTelecom.com");
		
		// 获取请求划拨号码的归属地
		char sOrgId[5]={0};
		
		char sAccoutNbr[32]={0};
		char sLocalAreaId[5]={0};
		//strcpy(sAccoutNbr,"18951765356");
		strcpy(sAccoutNbr,reqPayInfo.m_sDestinationAccount);
		//iRet = m_dccOperation->qryAccountOrgId(sAccoutNbr,sOrgId); // 测试时写死，此处返回自服务平台；
		//iRet = m_dccOperation->qryAccountOrgId(reqPayInfo.m_iDestinationAttr,sOrgId); // 测试后放开
		iRet = m_dccOperation->qryAccountOrgId(sAccoutNbr,sOrgId,sLocalAreaId);
		if(iRet !=0 )
		{
			__DEBUG_LOG0_(0, "取号码所属机构代码失败.");
			throw GET_ACCT_NBR_ORGID_ERR;
		}	
	
		snprintf(Head.m_sDestRealm,sizeof(Head.m_sDestRealm),"%s.ChinaTelecom.com",sOrgId);

		__DEBUG_LOG1_(0, "消息头，目标设备标识-Head.m_sDestRealm:[%s]",Head.m_sDestRealm);
		
		Head.m_iAuthAppId = 4;
		Head.m_iReqType = 4;
		Head.m_iReqNumber  = 0;
		Head.m_iResultCode = 0;
	
		memcpy((void *)(m_poPack->m_sDccBuf),(void *)&Head,sizeof(Head));
		
		// 打CCA包，返回自服务平台
		__DEBUG_LOG0_(0, "========>打CCA包，返回自服务平台");
			__DEBUG_LOG0_(0, "========>打CCA包，返回自服务平台");
		// CCA 第一层服务信息
		if (!m_poPack->addResPlatformSvc(oCCASvc))
		{
			__DEBUG_LOG0_(0, "ActiveAllocateBalanceBiz::deductBalanceIn 第一层CCA打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw DEDUCT_IN_PACK_ERR;
		}
		__DEBUG_LOG0_(0, "第一层CCA打包完成");
		
		// CCA 第二层打包
		if (!m_poPack->addResPlatformAcctBalance(oCCABalanceDetail))
		{
			__DEBUG_LOG0_(0, "ActiveAllocateBalanceBiz::deductBalanceIn 第二层CCA打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw DEDUCT_IN_PACK_ERR;
		}
		__DEBUG_LOG0_(0, "第二层CCA打包完成");
		
		//m_poPack->m_iCmdID = ABMCMD_PLATFORM_QUEUE;//ABMCMD_ALLOCATE_BALANCE;
		
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		if(iRet !=0 )
		{
			__DEBUG_LOG0_(0, "取DCC_INFO_RECORD_SEQ失败.");
			throw GET_DCC_INFO_RECORD_SEQ_ERR;
		}
		
		
		// 保存发出的CCA信息
		__DEBUG_LOG0_(0, "保存发出的DCC-CCA包信息入TT");
		iRet = m_dccOperation->insertDccInfo(Head,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存DCC-CCA信息失败");
			throw SAVE_SEND_DCC_CCA_ERR;
		}
		m_poPack->m_iCmdID = ABMCMD_PLATFORM_DEDUCT_BALANCE;
	}
	catch(TTStatus oSt)
	{
		iRet = ACTIVE_DEDUCT_BIZ_TT_ERR;
		__DEBUG_LOG1_(0, "ActiveAllocateBalanceBiz::deductBalanceOut oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "ActiveAllocateBalanceBiz::deductBalanceOut value=%d",value);
	}	
	for(int j=0;j<g_vResBalanceSvc.size();j++)
	{
		delete g_vResBalanceSvc[j];
		g_vResBalanceSvc[j]= NULL;
	}
	g_vResBalanceSvc.clear();
	
	
	return iRet;
}

