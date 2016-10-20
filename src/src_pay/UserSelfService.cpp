#include "UserSelfService.h"
#include "UserSelfServiceSql.h"
#include "PaymentInfoSql.h"

#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"
#include "errorcode_pay.h"


static vector<R_PayAbilityQuerySvc *> g_vRPayAbilityQuerySvc;
static vector<R_PayAbilityQuerySvc *>::iterator g_itr1;

static vector<R_MicropaymentInfo *> g_vRMicropaymentInfo;
static vector<R_MicropaymentInfo *>::iterator g_itr2;
	
extern bool g_toPlatformFlag;

PayAbilityQuery::PayAbilityQuery()
{
		m_poSql = NULL;
		m_poUnpack = NULL;
		m_poPack = NULL;
		m_dccOperation = NULL ;
}

PayAbilityQuery::~PayAbilityQuery()
{
	if (m_poSql != NULL)
		delete m_poSql;
	if (m_dccOperation != NULL)
		delete m_dccOperation;
}

int PayAbilityQuery::init(ABMException &oExp)
{
	if ((m_poSql = new PayAbilityQuerySql) == NULL)
	{
		ADD_EXCEPT0(oExp, "PayAbilityQuery::init malloc failed!");
		return -1;
	}
	
	if ((m_dccOperation = new DccOperation) == NULL)
	{
		ADD_EXCEPT0(oExp, "DccOperation::init malloc failed!");
		return -1;
	}
	
	return 0;
}

int PayAbilityQuery::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
	__DEBUG_LOG0_(0, "=========>测试：PayAbilityQuery::deal 请求服务开始");
	
	int iRet=0;
	int iSize=0;

	g_toPlatformFlag = true;
	
	m_poUnpack = (PayAbilityQueryCCR *)pCCR;
	m_poPack = (PayAbilityQueryCCA *)pCCA;
	
	m_poPack->clear();
	g_vRPayAbilityQuerySvc.clear();
	g_vRMicropaymentInfo.clear();
	
	A_PayAbilityQuerySvc oSvc={0} ;

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
		__DEBUG_LOG0_(0, "支付能力状态查询,接收CCR包,消息头入TT!");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存接收的CCR包消息头信息失败");
			throw SAVE_RECEVIE_DCC_CCR_ERR;
		}
		
		__DEBUG_LOG0_(0, "支付能力状态查询,CCR消息包解包!");
		//消息解包 - 第一层
		if ((iSize=m_poUnpack->getR_PayAbilityQuerySvc(g_vRPayAbilityQuerySvc)) == 0 )
		{
			__DEBUG_LOG0_(0, "支付能力状态查询::deal 请求服务消息包为NULL");
			m_poPack->setRltCode(ECODE_NOMSG);
			throw DEDUCT_UNPACK_NOMSG;
		}
		else if (iSize < 0)
		{
			__DEBUG_LOG0_(0, "支付能力状态查询::deal 消息解包出错!");
			m_poPack->setRltCode(ECODE_UNPACK_FAIL);
			throw DEDUCT_UNPACK_ERR;
		}
	
		
		// 处理第一层信息
		for (g_itr1=g_vRPayAbilityQuerySvc.begin(); g_itr1!=g_vRPayAbilityQuerySvc.end(); ++g_itr1)
		{
			// 根据SVC解detail包 - 第二层
			if ((iSize=m_poUnpack->getR_MicropaymentInfo(*g_itr1,g_vRMicropaymentInfo)) == 0 )
			{
				__DEBUG_LOG0_(0, "PayAbilityQuery::deal 请求明细消息包为NULL");
				m_poPack->setRltCode(ECODE_NOMSG);
				throw DEDUCT_UNPACK_NOMSG;
			}
			else if (iSize < 0)
			{
				__DEBUG_LOG0_(0, "PayAbilityQuery::deal 消息解包出错!");
				m_poPack->setRltCode(ECODE_UNPACK_FAIL);
				throw DEDUCT_UNPACK_ERR;
			}
			 
			__DEBUG_LOG0_(0, "测试:处理第一层信息........");	

			iRet = _deal(*g_itr1,g_vRMicropaymentInfo);

			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "支付能力状态查询操作失败");
				throw iRet;
			}
		}
		//return iRet;
	}
	catch(TTStatus oSt)
	{
		iRet = DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "PayAbilityQuery::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value)
	{
		iRet=value;
		__DEBUG_LOG1_(0, "支付能力状态查询失败:: 失败信息 value=%d", value);
	}
	if(iRet != 0)
	{
		// 打返回CCA包,返回错误码
		oSvc.m_svcResultCode=iRet;
		if (!m_poPack->addResPayAbilityQuerySvc(oSvc))
		{
			__DEBUG_LOG0_(0, "AllocateBalance:: _deal m_poPack 打包失败!");
			m_poPack->clear();
			m_poPack->setRltCode(ECODE_PACK_FAIL);
		}
		__DEBUG_LOG0_(0, "支付能力状态查询失败,iRet=[%d]",oSvc.m_svcResultCode);
		
	}
	return iRet;
}


int PayAbilityQuery::_deal(R_PayAbilityQuerySvc *pSvcIn,vector<R_MicropaymentInfo *> g_vRMicropaymentInfo)
{
	
	ABMException oExp;
		
	int iRet=0;
	time_t tTime;
	char sResponseTime[32]={0};
	
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "========================支付状态查询=======================");
	__DEBUG_LOG0_(0, "=====>start,取第一层CCR服务信息:");
	
	__DEBUG_LOG1_(0, "请求流水：m_requestId:%s",pSvcIn->m_requestId);
	__DEBUG_LOG1_(0, "请求时间：m_requestTime：%s",pSvcIn->m_requestTime);


	UserSelfService pUserSelfService={0};
	
	// 划拨CCR信息准备	
	strcpy(pUserSelfService.m_requestId,pSvcIn->m_requestId);		// 请求流水号
	strcpy(pUserSelfService.m_requestTime,pSvcIn->m_requestTime);	// 请求时间

			
	A_PayAbilityQuerySvc oSvc={0} ;	// CCA 响应服务信息
	A_MicropaymentInfo oData={0} ;	// CCA 响应扣费帐目明细信息
	vector<struct A_MicropaymentInfo> vMicropaymentInfo;
		
	// 根据CCR中的用户号码和号码类型查询用户、帐户属性信息
	ServAcctInfo oServAcct={0};

	
	try
	{	

		for (g_itr2=g_vRMicropaymentInfo.begin(); g_itr2!=g_vRMicropaymentInfo.end(); ++g_itr2)
		{	
			strcpy(pUserSelfService.m_servNbr,(*g_itr2)->m_servNbr);		//用户号码
			__DEBUG_LOG1_(0, "m_servNbr:%s", (*g_itr2)->m_servNbr);
			
			pUserSelfService.m_nbrType =(*g_itr2)->m_nbrType;		//号码类型
			__DEBUG_LOG1_(0, "m_nbrType:%d",(*g_itr2)->m_nbrType);
				
			pUserSelfService.m_servAttr = (*g_itr2)->m_servAttr;		//用户属性
			__DEBUG_LOG1_(0, "m_servAttr:%d",(*g_itr2)->m_servAttr);
			
			strcpy(pUserSelfService.m_sServicePlatformID,(*g_itr2)->m_sServicePlatformID);//业务平台ID
			__DEBUG_LOG1_(0, "m_sServicePlatformID:%s",(*g_itr2)->m_sServicePlatformID);
		
			
		
			// 调用业务逻辑处理
			iRet = m_poSql->PayAbilityStatusInfo(pUserSelfService,oExp);
			
			tTime = time(NULL);
			tTime += 2209017599;
			long2date(tTime,sResponseTime);
			strcpy(oSvc.m_responseTime,sResponseTime);
		
			strcpy(oSvc.m_responseId,pUserSelfService.m_requestId);
			strcpy(oData.m_PayStatus,pUserSelfService.m_payStatus);
			
			__DEBUG_LOG1_(0, "m_responseTime:%s",oSvc.m_responseTime);
			__DEBUG_LOG1_(0, "m_responseId:%s",oSvc.m_responseId);
			__DEBUG_LOG1_(0, "m_PayStatus:%s",oData.m_PayStatus);
			
			vMicropaymentInfo.push_back(oData);
		}
		
		// 打包 - 第一层服务信息
		if (!m_poPack->addResPayAbilityQuerySvc(oSvc))
		{
			__DEBUG_LOG0_(0, "PayAbilityQuery:: _deal m_poPack 打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw DEDUCT_PACK_ERR;
		}
		// 测试，打包后，oSvc数据
		__DEBUG_LOG1_(0, "第一层服务CCA打包完成,oSvc,responseId:[%s]",oSvc.m_responseId);
		
		// 打包 - 第二层扣费明细
		for(int j=0;j<vMicropaymentInfo.size();j++)
		{
			if (!m_poPack->addResMicropaymentInfo(vMicropaymentInfo[j]))
			{
				__DEBUG_LOG0_(0, "PayAbilityQuery:: _deal m_poPack 打包失败!");
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

 
 
static vector<R_BalanceTransSvc *> g_vRBalanceTransSvc;
static vector<R_BalanceTransSvc *>::iterator g_itrBT1;

static vector<R_BTMicropaymentInfo *> g_vRBTMicropaymentInfo;
static vector<R_BTMicropaymentInfo *>::iterator g_itrBT2;


BalanceTransQuery::BalanceTransQuery()
{
		m_poSql = NULL;
		m_poUnpack1 = NULL;
		m_poPack1 = NULL;
		m_dccOperation = NULL;
}

BalanceTransQuery::~BalanceTransQuery()
{
	if (m_poSql != NULL)
		delete m_poSql;
	if (m_dccOperation != NULL)
		delete m_dccOperation;
}

int BalanceTransQuery::init(ABMException &oExp)
{
	if ((m_poSql = new BalanceTransQuerySql) == NULL)
	{
		ADD_EXCEPT0(oExp, "BalanceTransQuery::init malloc failed!");
		return -1;
	}
	if ((m_dccOperation = new DccOperation) == NULL)
	{
		ADD_EXCEPT0(oExp, "DccOperation::init malloc failed!");
		return -1;
	}
	return 0;
}

int BalanceTransQuery::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
	__DEBUG_LOG0_(0, "=========>测试：BalanceTransQuery::deal 请求服务开始");
	
	int iRet=0;
	int iSize=0;

	m_poUnpack1 = (BalanceTransCCR *)pCCR;
	m_poPack1 = (BalanceTransCCA *)pCCA;
	
	m_poPack1->clear();
	g_vRBalanceTransSvc.clear();
	g_vRBTMicropaymentInfo.clear();
	
	A_BalanceTransSvc oSvc={0} ;
	
	g_toPlatformFlag = true;
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
			__DEBUG_LOG0_(0, "[余额划拨规则]:查询SEQ失败!");
			throw GET_DCC_INFO_RECORD_SEQ_ERR;	
		}
		
		// dcc包消息头信息
		__DEBUG_LOG0_(0, "余额划拨规则查询,接收CCR包,消息头入TT!");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack1->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存接收的CCR包消息头信息失败");
			throw SAVE_RECEVIE_DCC_CCR_ERR;
		}
		
		__DEBUG_LOG0_(0, "余额划拨规则查询,CCR消息包解包!");
		//消息解包 - 第一层
		if ((iSize=m_poUnpack1->getBalanceTransSvc(g_vRBalanceTransSvc)) == 0 )
		{
			__DEBUG_LOG0_(0, "余额划拨规则查询::deal 请求服务消息包为NULL");
			m_poPack1->setRltCode(ECODE_NOMSG);
			throw DEDUCT_UNPACK_NOMSG;
		}
		else if (iSize < 0)
		{
			__DEBUG_LOG0_(0, "余额划拨规则查询::deal 消息解包出错!");
			m_poPack1->setRltCode(ECODE_UNPACK_FAIL);
			throw DEDUCT_UNPACK_ERR;
		}
	
		
		// 处理第一层信息
		for (g_itrBT1=g_vRBalanceTransSvc.begin(); g_itrBT1!=g_vRBalanceTransSvc.end(); ++g_itrBT1)
		{
			// 根据SVC解detail包 - 第二层
			if ((iSize=m_poUnpack1->getBTMicropaymentInfo(*g_itrBT1,g_vRBTMicropaymentInfo)) == 0 )
			{
				__DEBUG_LOG0_(0, "BalanceTransQuery::deal 请求明细消息包为NULL");
				m_poPack1->setRltCode(ECODE_NOMSG);
				throw DEDUCT_UNPACK_NOMSG;
			}
			else if (iSize < 0)
			{
				__DEBUG_LOG0_(0, "BalanceTransQuery::deal 消息解包出错!");
				m_poPack1->setRltCode(ECODE_UNPACK_FAIL);
				throw DEDUCT_UNPACK_ERR;
			}
			 
			__DEBUG_LOG1_(0, "测试:处理第一层信息........iSize:%d",iSize);	

			iRet = _deal(*g_itrBT1,g_vRBTMicropaymentInfo);

			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "余额划拨规则查询操作失败");
				throw iRet;
			}
		}
		//return iRet;
	}
	catch(TTStatus oSt)
	{
		iRet = DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "BalanceTransQuery::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value)
	{
		iRet=value;
		__DEBUG_LOG1_(0, "余额划拨规则查询失败:: 失败信息 value=%d", value);
	}
	if(iRet != 0)
	{
		// 打返回CCA包,返回错误码
		oSvc.m_svcResultCode=iRet;
		if (!m_poPack1->addResBalanceTransSvc(oSvc))
		{
			__DEBUG_LOG0_(0, "BalanceTransQuery:: _deal m_poPack 打包失败!");
			m_poPack1->clear();
			m_poPack1->setRltCode(ECODE_PACK_FAIL);
		}
		__DEBUG_LOG0_(0, "余额划拨规则查询失败,iRet=[%d]",oSvc.m_svcResultCode);
		
	}
	return iRet;
}


int BalanceTransQuery::_deal(R_BalanceTransSvc *pSvcIn,vector<R_BTMicropaymentInfo *> g_vRBTMicropaymentInfo)
{
	
	ABMException oExp;
		
	int iRet=0;
	time_t tTime;
	char sResponseTime[32]={0};
	
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "========================余额划拨规则查询=======================");
	__DEBUG_LOG0_(0, "=====>start,取第一层CCR服务信息:");
	
	__DEBUG_LOG1_(0, "请求流水：m_requestId:%s",pSvcIn->m_requestId);
	__DEBUG_LOG1_(0, "请求时间：m_requestTime：%s",pSvcIn->m_requestTime);


	UserSelfService pUserSelfService={0};
	
	// 划拨CCR信息准备	
	strcpy(pUserSelfService.m_requestId,pSvcIn->m_requestId);		// 请求流水号
	strcpy(pUserSelfService.m_requestTime,pSvcIn->m_requestTime);	// 请求时间

			
	A_BalanceTransSvc oSvc={0} ;	// CCA 响应服务信息
	A_BTMicropaymentInfo oData={0} ;	// CCA 响应扣费帐目明细信息
	vector<struct A_BTMicropaymentInfo> vMicropaymentInfo;
		
	// 根据CCR中的用户号码和号码类型查询用户、帐户属性信息
	ServAcctInfo oServAcct={0};

	
	try
	{	

		for (g_itrBT2=g_vRBTMicropaymentInfo.begin(); g_itrBT2!=g_vRBTMicropaymentInfo.end(); ++g_itrBT2)
		{	
			strcpy(pUserSelfService.m_servNbr,(*g_itrBT2)->m_servNbr);		//用户号码
			__DEBUG_LOG1_(0, "m_servNbr:%s", (*g_itrBT2)->m_servNbr);
			
			pUserSelfService.m_nbrType =(*g_itrBT2)->m_nbrType;		//号码类型
			__DEBUG_LOG1_(0, "m_nbrType:%d",(*g_itrBT2)->m_nbrType);
				
			pUserSelfService.m_servAttr = (*g_itrBT2)->m_servAttr;		//用户属性
			__DEBUG_LOG1_(0, "m_servAttr:%d",(*g_itrBT2)->m_servAttr);
			
			strcpy(pUserSelfService.m_sServicePlatformID,(*g_itrBT2)->m_sServicePlatformID);//业务平台ID
			__DEBUG_LOG1_(0, "m_sServicePlatformID:%s",(*g_itrBT2)->m_sServicePlatformID);
		
			
		
			// 调用业务逻辑处理
			iRet = m_poSql->BalanceTransInfo(pUserSelfService,oExp);
			
			tTime = time(NULL);
			tTime += 2209017599;
			long2date(tTime,sResponseTime);
			strcpy(oSvc.m_responseTime,sResponseTime);
		
			strcpy(oSvc.m_responseId,pUserSelfService.m_requestId);
			
			oData.m_dateTransFreq=pUserSelfService.m_dateTransFreq;
			oData.m_transLimit=pUserSelfService.m_transLimit;
			oData.m_monthTransLimit=pUserSelfService.m_monthTransLimit;
			oData.m_autotransflag=pUserSelfService.m_autotransflag;
			oData.m_autobalancelimit=pUserSelfService.m_autobalancelimit;
			oData.m_autoAmountlimit=pUserSelfService.m_autoAmountlimit;
			
			__DEBUG_LOG1_(0, "m_responseTime:%s",oSvc.m_responseTime);
			__DEBUG_LOG1_(0, "m_responseId:%s",oSvc.m_responseId);
			
			__DEBUG_LOG1_(0, "m_dateTransFreq:%d",oData.m_dateTransFreq);
			__DEBUG_LOG1_(0, "m_transLimit:%d",oData.m_transLimit);
			__DEBUG_LOG1_(0, "m_month_TransLimit:%d",oData.m_monthTransLimit);
			__DEBUG_LOG1_(0, "m_auto_transflag:%d",oData.m_autotransflag);
			__DEBUG_LOG1_(0, "m_autobalancelimit:%d",oData.m_autobalancelimit);
			__DEBUG_LOG1_(0, "m_autoAmountlimit:%d",oData.m_autoAmountlimit);
			
			vMicropaymentInfo.push_back(oData);
		}
		
		// 打包 - 第一层服务信息
		if (!m_poPack1->addResBalanceTransSvc(oSvc))
		{
			__DEBUG_LOG0_(0, "PayAbilityQuery:: _deal m_poPack 打包失败!");
			m_poPack1->setRltCode(ECODE_PACK_FAIL);
			throw DEDUCT_PACK_ERR;
		}
		// 测试，打包后，oSvc数据
		__DEBUG_LOG1_(0, "第一层服务CCA打包完成,oSvc,responseId:[%s]",oSvc.m_responseId);
		
		// 打包 - 第二层扣费明细
		for(int j=0;j<vMicropaymentInfo.size();j++)
		{
			if (!m_poPack1->addResBTMicropaymentInfo(vMicropaymentInfo[j]))
			{
				__DEBUG_LOG0_(0, "BalanceTransQuery:: _deal m_poPack 打包失败!");
				m_poPack1->setRltCode(ECODE_PACK_FAIL);
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


static vector<R_BalanceTransResetSvc *> g_vRBalanceTransResetSvc;
static vector<R_BalanceTransResetSvc *>::iterator g_itrTR1;

static vector<R_TRMicropaymentInfo *> g_vRTRMicropaymentInfo;
static vector<R_TRMicropaymentInfo *>::iterator g_itrTR2;


BalanceTransReset::BalanceTransReset()
{
		m_poSql = NULL;
		m_poSqlPassBTR = NULL;
		m_poSqlRules = NULL;
		m_poUnpack1 = NULL;
		m_poPack1 = NULL;
		m_dccOperation = NULL;
		m_poSqlOpt = NULL;
}

BalanceTransReset::~BalanceTransReset()
{
	if (m_poSql != NULL)
		delete m_poSql;
	if (m_poSqlPassBTR != NULL)
		delete m_poSqlPassBTR;
	if (m_poSqlRules != NULL)
		delete m_poSqlRules;
	if (m_dccOperation != NULL)
		delete m_dccOperation;
	if (m_poSqlOpt != NULL)
		delete m_poSqlOpt;	
}

int BalanceTransReset::init(ABMException &oExp)
{
	if ((m_poSql = new BalanceTransResetSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "BalanceTransReset::init malloc failed!");
		return -1;
	}
	
	if ((m_poSqlPassBTR = new PasswordResetSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "PasswordResetSql::init malloc failed!");
		return -1;
	}
	if ((m_poSqlRules = new TransferRulesSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "TransferRulesSql::init malloc failed!");
		return -1;
	}
	
	if ((m_dccOperation = new DccOperation) == NULL)
	{
		ADD_EXCEPT0(oExp, "DccOperation::init malloc failed!");
		return -1;
	}
	
	if ((m_poSqlOpt = new PaymentInfoSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "PaymentInfoSql::init malloc failed!");
		return -1;
	}
	return 0;
}

int BalanceTransReset::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
	__DEBUG_LOG0_(0, "=========>测试：BalanceTransReset::deal 请求服务开始");
	
	int iRet=0;
	int iSize=0;

	m_poUnpack1 = (BalanceTransResetCCR *)pCCR;
	m_poPack1 = (BalanceTransResetCCA *)pCCA;
	
	m_poPack1->clear();
	g_vRBalanceTransResetSvc.clear();
	g_vRTRMicropaymentInfo.clear();
	
	A_BalanceTransResetSvc oSvc={0} ;
	
	g_toPlatformFlag = true;
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
			__DEBUG_LOG0_(0, "[余额划拨规则]:查询SEQ失败!");
			throw GET_DCC_INFO_RECORD_SEQ_ERR;	
		}
		
		// dcc包消息头信息
		__DEBUG_LOG0_(0, "余额划拨规则设置,接收CCR包,消息头入TT!");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack1->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存接收的CCR包消息头信息失败");
			throw SAVE_RECEVIE_DCC_CCR_ERR;
		}
		
		__DEBUG_LOG0_(0, "余额划拨规则设置,CCR消息包解包!");
		//消息解包 - 第一层
		if ((iSize=m_poUnpack1->getBalanceTransResetSvc(g_vRBalanceTransResetSvc)) == 0 )
		{
			__DEBUG_LOG0_(0, "余额划拨规则设置::deal 请求服务消息包为NULL");
			m_poPack1->setRltCode(ECODE_NOMSG);
			throw DEDUCT_UNPACK_NOMSG;
		}
		else if (iSize < 0)
		{
			__DEBUG_LOG0_(0, "余额划拨规则设置::deal 消息解包出错!");
			m_poPack1->setRltCode(ECODE_UNPACK_FAIL);
			throw DEDUCT_UNPACK_ERR;
		}
	
		
		// 处理第一层信息
		for (g_itrTR1=g_vRBalanceTransResetSvc.begin(); g_itrTR1!=g_vRBalanceTransResetSvc.end(); ++g_itrTR1)
		{
			// 根据SVC解detail包 - 第二层
			if ((iSize=m_poUnpack1->getTRMicropaymentInfo(*g_itrTR1,g_vRTRMicropaymentInfo)) == 0 )
			{
				__DEBUG_LOG0_(0, "BalanceTransReset::deal 请求明细消息包为NULL");
				m_poPack1->setRltCode(ECODE_NOMSG);
				throw DEDUCT_UNPACK_NOMSG;
			}
			else if (iSize < 0)
			{
				__DEBUG_LOG0_(0, "BalanceTransReset::deal 消息解包出错!");
				m_poPack1->setRltCode(ECODE_UNPACK_FAIL);
				throw DEDUCT_UNPACK_ERR;
			}
			 
			__DEBUG_LOG1_(0, "测试:处理第一层信息........iSize:%d",iSize);	

			iRet = _deal(*g_itrTR1,g_vRTRMicropaymentInfo);

			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "余额划拨规则设置操作失败");
				throw iRet;
			}
		}
		//return iRet;
	}
	catch(TTStatus oSt)
	{
		iRet = DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "BalanceTransReset::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value)
	{
		iRet=value;
		__DEBUG_LOG1_(0, "余额划拨规则设置失败:: 失败信息 value=%d", value);
	}
	if(iRet != 0)
	{
		// 打返回CCA包,返回错误码
		oSvc.m_svcResultCode=iRet;
		if (!m_poPack1->addBalanceTransResetSvc(oSvc))
		{
			__DEBUG_LOG0_(0, "BalanceTransReset:: _deal m_poPack 打包失败!");
			m_poPack1->clear();
			m_poPack1->setRltCode(ECODE_PACK_FAIL);
		}
		__DEBUG_LOG0_(0, "余额划拨规则设置失败,iRet=[%d]",oSvc.m_svcResultCode);
		
	}
	return iRet;
}


int BalanceTransReset::_deal(R_BalanceTransResetSvc *pSvcIn,vector<R_TRMicropaymentInfo *> g_vRTRMicropaymentInfo)
{
	
	ABMException oExp;
		
	int iRet=0;
	time_t tTime;
	char sResponseTime[32]={0};
	char sTmp[32];
	bool bState=false;
	memset(sTmp,0,sizeof(sTmp));
	
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "========================余额划拨规则设置=======================");
	__DEBUG_LOG0_(0, "=====>start,取第一层CCR服务信息:");
	
	__DEBUG_LOG1_(0, "请求流水：m_requestId:%s",pSvcIn->m_requestId);
	__DEBUG_LOG1_(0, "请求时间：m_requestTime：%s",pSvcIn->m_requestTime);


	UserSelfService pUserSelfService={0};
	StructPaymentSeqInfo  sStructPaymentSeqInfo={0};
	PaymentInfoALLCCR pPaymentInfoCCR={0};
	
	// 划拨CCR信息准备	
	strcpy(pUserSelfService.m_requestId,pSvcIn->m_requestId);		// 请求流水号
	strcpy(pUserSelfService.m_requestTime,pSvcIn->m_requestTime);	// 请求时间

			
	A_BalanceTransResetSvc oSvc={0} ;	// CCA 响应服务信息
	A_TRMicropaymentInfo oData={0} ;	// CCA 响应扣费帐目明细信息
	vector<struct A_TRMicropaymentInfo> vMicropaymentInfo;
		
	// 根据CCR中的用户号码和号码类型查询用户、帐户属性信息
	ServAcctInfo oServAcct={0};

	
	try
	{	

		for (g_itrTR2=g_vRTRMicropaymentInfo.begin(); g_itrTR2!=g_vRTRMicropaymentInfo.end(); ++g_itrTR2)
		{	
			__DEBUG_LOG0_(0, "---------CCR如下：");
			strcpy(pUserSelfService.m_servNbr,(*g_itrTR2)->m_servNbr);		//用户号码
			__DEBUG_LOG1_(0, "m_servNbr:%s", (*g_itrTR2)->m_servNbr);
			
			pUserSelfService.m_nbrType =(*g_itrTR2)->m_nbrType;		//号码类型
			__DEBUG_LOG1_(0, "m_nbrType:%d",(*g_itrTR2)->m_nbrType);
				
			pUserSelfService.m_servAttr = (*g_itrTR2)->m_servAttr;		//用户属性
			__DEBUG_LOG1_(0, "m_servAttr:%d",(*g_itrTR2)->m_servAttr);
			
			strcpy(pUserSelfService.m_sServicePlatformID,(*g_itrTR2)->m_sServicePlatformID);//业务平台ID
			__DEBUG_LOG1_(0, "m_sServicePlatformID:%s",(*g_itrTR2)->m_sServicePlatformID);
			
			strcpy(sTmp,(*g_itrTR2)->m_payPassword);//支付密码
			__DEBUG_LOG1_(0, "m_payPassword:%s",(*g_itrTR2)->m_payPassword);
			
			pUserSelfService.m_dateTransFreq=(*g_itrTR2)->m_dateTransFreq;//每天划拨频度（单位次）
			__DEBUG_LOG1_(0, "m_dateTransFreq:%d",(*g_itrTR2)->m_dateTransFreq);
			
			pUserSelfService.m_transLimit=(*g_itrTR2)->m_transLimit;//每次金额限额（单位分）
			__DEBUG_LOG1_(0, "m_transLimit:%d",(*g_itrTR2)->m_transLimit);
			
			pUserSelfService.m_monthTransLimit=(*g_itrTR2)->m_monthTransLimit;//每月金额限额（单位分）
			__DEBUG_LOG1_(0, "m_monthTransLimit:%d",(*g_itrTR2)->m_monthTransLimit);
			
			pUserSelfService.m_autotransflag=(*g_itrTR2)->m_autotransflag;//自动触发标志（0-不自动划拨；1-自动）
			__DEBUG_LOG1_(0, "m_autotransflag:%d",(*g_itrTR2)->m_autotransflag);
			
			pUserSelfService.m_autobalancelimit=(*g_itrTR2)->m_autobalancelimit;//自动划拨阀值（单位分）
			__DEBUG_LOG1_(0, "m_autobalancelimit:%d",(*g_itrTR2)->m_autobalancelimit);
			
			pUserSelfService.m_autoAmountlimit=(*g_itrTR2)->m_autoAmountlimit;//自动划拨金额（单位分）
			__DEBUG_LOG1_(0, "m_autoAmountlimit:%d",(*g_itrTR2)->m_autoAmountlimit);
			
		
			tTime = time(NULL);
			tTime += 2209017599;
			long2date(tTime,sResponseTime);
			strcpy(oSvc.m_responseTime,sResponseTime);
		
			strcpy(oSvc.m_responseId,pUserSelfService.m_requestId);
			
			oData.m_dateTransFreq=pUserSelfService.m_dateTransFreq;
			oData.m_transLimit=pUserSelfService.m_transLimit;
			oData.m_monthTransLimit=pUserSelfService.m_monthTransLimit;
			oData.m_autotransflag=pUserSelfService.m_autotransflag;
			oData.m_autobalancelimit=pUserSelfService.m_autobalancelimit;
			oData.m_autoAmountlimit=pUserSelfService.m_autoAmountlimit;
			
			__DEBUG_LOG0_(0, "---------CCA如下：");
			__DEBUG_LOG1_(0, "m_responseTime:%s",oSvc.m_responseTime);
			__DEBUG_LOG1_(0, "m_responseId:%s",oSvc.m_responseId);
			
			__DEBUG_LOG1_(0, "m_dateTransFreq:%d",oData.m_dateTransFreq);
			__DEBUG_LOG1_(0, "m_transLimit:%d",oData.m_transLimit);
			__DEBUG_LOG1_(0, "m_month_TransLimit:%d",oData.m_monthTransLimit);
			__DEBUG_LOG1_(0, "m_auto_transflag:%d",oData.m_autotransflag);
			__DEBUG_LOG1_(0, "m_autobalancelimit:%d",oData.m_autobalancelimit);
			__DEBUG_LOG1_(0, "m_autoAmountlimit:%d",oData.m_autoAmountlimit);
			
			vMicropaymentInfo.push_back(oData);
			
			if( ( iRet = m_poSqlPassBTR->PasswordResetAuthen(pUserSelfService,oExp) ) <0)
			{
				return PAY_PASSWD_AUTHEN_ERR;
			}
			
			
			__DEBUG_LOG2_(0, "鉴权密码%s==%s",pUserSelfService.m_payPassword,sTmp);
			
			if(strncmp(pUserSelfService.m_payPassword,sTmp,strlen(pUserSelfService.m_payPassword))==0 &&
				strncmp(pUserSelfService.m_payPassword,sTmp,strlen(sTmp))==0)
			{
				//查用户标识给入业务记录用
				if( ( iRet = m_poSqlPassBTR->PasswordResetSelect(pUserSelfService,oExp) ) <0)
				{
					__DEBUG_LOG0_(0, "查询用户标识失败!");
					throw PAY_PASSWD_SELECT_ERR;
				}
			
				//查询业务流水
				iRet=m_poSqlOpt->QueryPaySeq(sStructPaymentSeqInfo,bState);
				if(iRet!=0)
		        {
					__DEBUG_LOG0_(0, "查询业务流水失败!");
					throw iRet;
		        }
		        
				pUserSelfService.m_lPaymentSeq=sStructPaymentSeqInfo.m_lPaymentSeq;
			
				if( (iRet = m_poSqlRules->TransferRulesQuery(pUserSelfService,oExp)) == 1 )
				{
					__DEBUG_LOG0_(0, "----查询没有划拨支付规则记录，可以新增---");
						iRet = m_poSqlRules->TransferRulesAcctId(pUserSelfService,oExp);
						
						pUserSelfService.m_datePayFreq =0;
						pUserSelfService.m_payLimit =0;
						pUserSelfService.m_monthPayLimit =0;
						pUserSelfService.m_microPayLimit =0;
						__DEBUG_LOG1_(0, "---查询出账户标识:%d",pUserSelfService.m_acctId);
						
						if( (iRet = m_poSqlRules->TransferRulesInsert(pUserSelfService,oExp)) == 0)
						{
							__DEBUG_LOG0_(0, "----新增划拨支付规则成功---");
						}
						else
						{
							__DEBUG_LOG0_(0, "----新增划拨支付规则失败---");
							oSvc.m_svcResultCode=TRANSFERRULES_INSERT_ERR;
						}
				}
				else
				{
					__DEBUG_LOG0_(0, "----查询有划拨支付规则记录，直接修改---");
					iRet = m_poSql->BalanceTransResetInfo(pUserSelfService,oExp);
				}
				
				strcpy(pPaymentInfoCCR.m_sOperateSeq,pUserSelfService.m_requestId);  //外部流水
				strcpy(pPaymentInfoCCR.m_sCapabilityCode,"0014"); // 能力编码
				pPaymentInfoCCR.m_lServID=atoi(pUserSelfService.m_userInfoId);  //用户标识
				strcpy(pPaymentInfoCCR.m_sDescription,"自服务平台划拨规则设置");
				
				//插业务记录
				iRet=m_poSqlOpt->InsertPaymentInfo(pPaymentInfoCCR,sStructPaymentSeqInfo);
				if(iRet!=0)
	        	{
					__DEBUG_LOG0_(0, "记录业务信息表失败!");
					throw iRet;
	        	}
			}
			else
			{
				oSvc.m_svcResultCode=PASSWD_AUTHEN_ERR;
			}
        	
		}
		
		// 打包 - 第一层服务信息
		if (!m_poPack1->addBalanceTransResetSvc(oSvc))
		{
			__DEBUG_LOG0_(0, "BalanceTransReset:: _deal m_poPack 打包失败!");
			m_poPack1->setRltCode(ECODE_PACK_FAIL);
			throw DEDUCT_PACK_ERR;
		}
		// 测试，打包后，oSvc数据
		__DEBUG_LOG1_(0, "第一层服务CCA打包完成,oSvc,responseId:[%s]",oSvc.m_responseId);
		
		// 打包 - 第二层扣费明细
		for(int j=0;j<vMicropaymentInfo.size();j++)
		{
			if (!m_poPack1->addResTRMicropaymentInfo(vMicropaymentInfo[j]))
			{
				__DEBUG_LOG0_(0, "BalanceTransReset:: _deal m_poPack 打包失败!");
				m_poPack1->setRltCode(ECODE_PACK_FAIL);
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


static vector<R_PasswordReset *> g_vRPasswordResetSvc;
static vector<R_PasswordReset *>::iterator g_itrPR1;

static vector<R_PRMicropaymentInfo *> g_vPRMicropaymentInfo;
static vector<R_PRMicropaymentInfo *>::iterator g_itrPR2;


PasswordReset::PasswordReset()
{
		m_poSql = NULL;
		m_poUnpack2 = NULL;
		m_poPack2 = NULL;
		m_dccOperation = NULL;
		m_poSqlOpt=NULL;
}

PasswordReset::~PasswordReset()
{
	if (m_poSql != NULL)
		delete m_poSql;
	if (m_dccOperation != NULL)
		delete m_dccOperation;
	if (m_poSqlOpt != NULL)
		delete m_poSqlOpt;	
}

int PasswordReset::init(ABMException &oExp)
{
	if ((m_poSql = new PasswordResetSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "PasswordReset::init malloc failed!");
		return -1;
	}
	
	if ((m_dccOperation = new DccOperation) == NULL)
	{
		ADD_EXCEPT0(oExp, "DccOperation::init malloc failed!");
		return -1;
	}
	
	if ((m_poSqlOpt = new PaymentInfoSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "PaymentInfoSql::init malloc failed!");
		return -1;
	}
	return 0;
}

int PasswordReset::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
	__DEBUG_LOG0_(0, "=========>PasswordReset::deal 请求服务开始");
	
	int iRet=0;
	int iSize=0;

	g_toPlatformFlag = true;
	
	m_poUnpack2 = (PasswordResetCCR *)pCCR;
	m_poPack2 = (PasswordResetCCA *)pCCA;
	
	m_poPack2->clear();
	g_vRPasswordResetSvc.clear();
	g_vPRMicropaymentInfo.clear();
	
	A_PasswordReset oSvc={0} ;

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
			__DEBUG_LOG0_(0, "[支付密码修改-PasswordReset]:查询SEQ失败!");
			throw GET_DCC_INFO_RECORD_SEQ_ERR;	
		}
		
		// dcc包消息头信息
		__DEBUG_LOG0_(0, "支付能力状态查询,接收CCR包,消息头入TT!");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack2->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存接收的CCR包消息头信息失败");
			throw SAVE_RECEVIE_DCC_CCR_ERR;
		}
		
		__DEBUG_LOG0_(0, "支付密码修改,CCR消息包解包!");
		//消息解包 - 第一层
		if ((iSize=m_poUnpack2->getR_PasswordReset(g_vRPasswordResetSvc)) == 0 )
		{
			__DEBUG_LOG0_(0, "支付密码修改::deal 请求服务消息包为NULL");
			m_poPack2->setRltCode(ECODE_NOMSG);
			throw DEDUCT_UNPACK_NOMSG;
		}
		else if (iSize < 0)
		{
			__DEBUG_LOG0_(0, "支付密码修改::deal 消息解包出错!");
			m_poPack2->setRltCode(ECODE_UNPACK_FAIL);
			throw DEDUCT_UNPACK_ERR;
		}
	
		
		// 处理第一层信息
		for (g_itrPR1=g_vRPasswordResetSvc.begin(); g_itrPR1!=g_vRPasswordResetSvc.end(); ++g_itrPR1)
		{
			// 根据SVC解detail包 - 第二层
			if ((iSize=m_poUnpack2->getR_PRMicropaymentInfo(*g_itrPR1,g_vPRMicropaymentInfo)) == 0 )
			{
				__DEBUG_LOG0_(0, "PasswordReset::deal 请求明细消息包为NULL");
				m_poPack2->setRltCode(ECODE_NOMSG);
				throw DEDUCT_UNPACK_NOMSG;
			}
			else if (iSize < 0)
			{
				__DEBUG_LOG0_(0, "PasswordReset::deal 消息解包出错!");
				m_poPack2->setRltCode(ECODE_UNPACK_FAIL);
				throw DEDUCT_UNPACK_ERR;
			}
			 
			__DEBUG_LOG0_(0, "测试:处理第一层信息........");	

			iRet = _deal(*g_itrPR1,g_vPRMicropaymentInfo);

			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "支付密码修改操作失败");
				throw iRet;
			}
		}
		//return iRet;
	}
	catch(TTStatus oSt)
	{
		iRet = DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "PasswordReset::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value)
	{
		iRet=value;
		__DEBUG_LOG1_(0, "支付密码修改失败:: 失败信息 value=%d", value);
	}
	if(iRet != 0)
	{
		// 打返回CCA包,返回错误码
		oSvc.m_svcResultCode=iRet;
		if (!m_poPack2->addA_PasswordReset(oSvc))
		{
			__DEBUG_LOG0_(0, "PasswordReset:: _deal m_poPack 打包失败!");
			m_poPack2->clear();
			m_poPack2->setRltCode(ECODE_PACK_FAIL);
		}
		__DEBUG_LOG0_(0, "支付密码修改操作失败,iRet=[%d]",oSvc.m_svcResultCode);
		
	}
	return iRet;
}


int PasswordReset::_deal(R_PasswordReset *pSvcIn,vector<R_PRMicropaymentInfo *> g_vPRMicropaymentInfo)
{
	
	ABMException oExp;
		
	int iRet=0;
	time_t tTime;
	char sResponseTime[32]={0};
	bool bState=false;
	
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "========================支付密码变更=======================");
	__DEBUG_LOG0_(0, "=====>start,取第一层CCR服务信息:");
	
	__DEBUG_LOG1_(0, "请求流水：m_requestId:%s",pSvcIn->m_requestId);
	__DEBUG_LOG1_(0, "请求时间：m_requestTime：%s",pSvcIn->m_requestTime);


	UserSelfService pUserSelfService={0};
	StructPaymentSeqInfo  sStructPaymentSeqInfo={0};
	PaymentInfoALLCCR pPaymentInfoCCR={0};
	
	// 划拨CCR信息准备	
	strcpy(pUserSelfService.m_requestId,pSvcIn->m_requestId);		// 请求流水号
	strcpy(pUserSelfService.m_requestTime,pSvcIn->m_requestTime);	// 请求时间

			
	A_PasswordReset oSvc={0} ;	// CCA 响应服务信息

	
	try
	{	

		for (g_itrPR2=g_vPRMicropaymentInfo.begin(); g_itrPR2!=g_vPRMicropaymentInfo.end(); ++g_itrPR2)
		{	
			
			strcpy(pUserSelfService.m_servNbr,(*g_itrPR2)->m_servNbr);		//用户号码
			__DEBUG_LOG1_(0, "m_servNbr:%s", (*g_itrPR2)->m_servNbr);
			
			pUserSelfService.m_nbrType =(*g_itrPR2)->m_nbrType;		//号码类型
			__DEBUG_LOG1_(0, "m_nbrType:%d",(*g_itrPR2)->m_nbrType);
				
			pUserSelfService.m_servAttr = (*g_itrPR2)->m_servAttr;		//用户属性
			__DEBUG_LOG1_(0, "m_servAttr:%d",(*g_itrPR2)->m_servAttr);
			
			strcpy(pUserSelfService.m_sServicePlatformID,(*g_itrPR2)->m_sServicePlatformID);//业务平台ID
			__DEBUG_LOG1_(0, "m_sServicePlatformID:%s",(*g_itrPR2)->m_sServicePlatformID);
		
			strcpy(pUserSelfService.m_oldPassword,(*g_itrPR2)->m_oldPassword);//旧密码
			__DEBUG_LOG1_(0, "m_oldPassword:%s",pUserSelfService.m_oldPassword);
			
			strcpy(pUserSelfService.m_newPassword,(*g_itrPR2)->m_newPassword);//信密码
			__DEBUG_LOG1_(0, "m_newPassword:%s",(*g_itrPR2)->m_newPassword);
			
			strcpy(pUserSelfService.m_actionId,(*g_itrPR2)->m_actionId);//操作类型
			__DEBUG_LOG1_(0, "m_actionId:%s",(*g_itrPR2)->m_actionId);
			
			
			
			tTime = time(NULL);
			tTime += 2209017599;
			long2date(tTime,sResponseTime);
			strcpy(oSvc.m_responseTime,sResponseTime);
		
			strcpy(oSvc.m_responseId,pUserSelfService.m_requestId);
			
			__DEBUG_LOG1_(0, "m_responseTime:%s",oSvc.m_responseTime);
			__DEBUG_LOG1_(0, "m_responseId:%s",oSvc.m_responseId);
			
			
			if( ( iRet = m_poSql->PasswordResetAuthen(pUserSelfService,oExp) ) <0)
			{
				return PAY_PASSWD_AUTHEN_ERR;
			}

			__DEBUG_LOG2_(0, "鉴权密码%s==%s",pUserSelfService.m_payPassword,pUserSelfService.m_oldPassword);
			
			if(strncmp(pUserSelfService.m_payPassword,pUserSelfService.m_oldPassword,strlen(pUserSelfService.m_payPassword))==0 &&
				strncmp(pUserSelfService.m_payPassword,pUserSelfService.m_oldPassword,strlen(pUserSelfService.m_oldPassword))==0)
			{
				if( ( iRet = m_poSql->PasswordResetSelect(pUserSelfService,oExp) ) <0)
				{
					__DEBUG_LOG0_(0, "查询用户标识失败!");
					throw PAY_PASSWD_SELECT_ERR;
				}
				
				//查询业务流水
				iRet=m_poSqlOpt->QueryPaySeq(sStructPaymentSeqInfo,bState);
				if(iRet!=0)
		        {
					__DEBUG_LOG0_(0, "查询业务流水失败!");
					throw iRet;
		        }
		        
				pUserSelfService.m_lPaymentSeq=sStructPaymentSeqInfo.m_lPaymentSeq;
			
				if(strncmp(pUserSelfService.m_actionId,"01",2)==0)
				{
					if( ( iRet = m_poSql->PasswordResetUpdate(pUserSelfService,oExp) ) <0)
					{
						return	PAY_PASSWD_UPDATE_ERR;
					}
				}
				else if(strncmp(pUserSelfService.m_actionId,"02",2)==0)
				{
					if( (iRet = m_poSql->PasswordResetdelete(pUserSelfService,oExp) ) <0 )
					{
						return	PAY_PASSWD_DELETE_ERR;
					}
					
					if( ( iRet = m_poSql->PasswordResetSelect(pUserSelfService,oExp) )<0 )
					{
						return	PAY_PASSWD_SELECT_ERR;
					}
					
					
					if( (iRet = m_poSql->PasswordResetinsert(pUserSelfService,oExp) )<0)
					{
						return	PAY_PASSWD_INSERT_ERR;
					}
				}
				else
				{
					return PASSWD_FLAG_TYPE_ERR;
				}
				
				strcpy(pPaymentInfoCCR.m_sOperateSeq,pUserSelfService.m_requestId);  //外部流水
				strcpy(pPaymentInfoCCR.m_sCapabilityCode,"0010"); // 能力编码
				pPaymentInfoCCR.m_lServID=atoi(pUserSelfService.m_userInfoId);  //用户标识
				strcpy(pPaymentInfoCCR.m_sDescription,"自服务平台支付密码修改");
				
				//插业务记录
				iRet=m_poSqlOpt->InsertPaymentInfo(pPaymentInfoCCR,sStructPaymentSeqInfo);
				if(iRet!=0)
	        	{
					__DEBUG_LOG0_(0, "记录业务信息表失败!");
					throw iRet;
	        	}
			}
			else
			{
				oSvc.m_svcResultCode=PASSWD_AUTHEN_ERR;
			}
		}
		
		// 打包 - 第一层服务信息
		if (!m_poPack2->addA_PasswordReset(oSvc))
		{
			__DEBUG_LOG0_(0, "PasswordReset:: _deal m_poPack 打包失败!");
			m_poPack2->setRltCode(ECODE_PACK_FAIL);
			throw DEDUCT_PACK_ERR;
		}
		// 测试，打包后，oSvc数据
		__DEBUG_LOG1_(0, "第一层服务CCA打包完成,oSvc,responseId:[%s]",oSvc.m_responseId);
	
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



static vector<R_PayStatusReset *> g_vRPayStatusResetSvc;
static vector<R_PayStatusReset *>::iterator g_itrPSR1;

static vector<R_PSRMicropaymentInfo *> g_vPSRMicropaymentInfo;
static vector<R_PSRMicropaymentInfo *>::iterator g_itrPSR2;

PayStatusReset::PayStatusReset()
{
		m_poSql = NULL;
		m_poSqlPasswd = NULL;
		m_poUnpack = NULL;
		m_poPack = NULL;
		m_dccOperation = NULL;
		m_poSqlOpt=NULL;
}

PayStatusReset::~PayStatusReset()
{
	if (m_poSql != NULL)
		delete m_poSql;
	if (m_poSqlPasswd != NULL)
		delete m_poSqlPasswd;
	if (m_dccOperation != NULL)
		delete m_dccOperation;
	if (m_poSqlOpt != NULL)
		delete m_poSqlOpt;	
}

int PayStatusReset::init(ABMException &oExp)
{
	if ((m_poSql = new PayStatusResetSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "PayStatusReset::init malloc failed!");
		return -1;
	}
	
	if ((m_poSqlPasswd = new PasswordResetSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "PasswordResetSql::init malloc failed!");
		return -1;
	}
	
	if ((m_dccOperation = new DccOperation) == NULL)
	{
		ADD_EXCEPT0(oExp, "DccOperation::init malloc failed!");
		return -1;
	}
	if ((m_poSqlOpt = new PaymentInfoSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "PaymentInfoSql::init malloc failed!");
		return -1;
	}
	
	return 0;
}

int PayStatusReset::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
	__DEBUG_LOG0_(0, "=========>测试：PayStatusReset::deal 请求服务开始");
	
	int iRet=0;
	int iSize=0;

	g_toPlatformFlag = true;
	
	m_poUnpack = (PayStatusResetCCR *)pCCR;
	m_poPack = (PayStatusResetCCA *)pCCA;
	
	m_poPack->clear();
	g_vRPayStatusResetSvc.clear();
	g_vPSRMicropaymentInfo.clear();
	
	A_PayStatusReset oSvc={0} ;

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
			__DEBUG_LOG0_(0, "[支付状态变更-PayStatusReset]:查询SEQ失败!");
			throw GET_DCC_INFO_RECORD_SEQ_ERR;	
		}
		
		// dcc包消息头信息
		__DEBUG_LOG0_(0, "支付状态变更,接收CCR包,消息头入TT!");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存接收的CCR包消息头信息失败");
			throw SAVE_RECEVIE_DCC_CCR_ERR;
		}
		
		__DEBUG_LOG0_(0, "支付状态变更,CCR消息包解包!");
		//消息解包 - 第一层
		if ((iSize=m_poUnpack->getR_PayStatusReset(g_vRPayStatusResetSvc)) == 0 )
		{
			__DEBUG_LOG0_(0, "支付状态变更::deal 请求服务消息包为NULL");
			m_poPack->setRltCode(ECODE_NOMSG);
			throw DEDUCT_UNPACK_NOMSG;
		}
		else if (iSize < 0)
		{
			__DEBUG_LOG0_(0, "支付状态变更::deal 消息解包出错!");
			m_poPack->setRltCode(ECODE_UNPACK_FAIL);
			throw DEDUCT_UNPACK_ERR;
		}
	
		
		// 处理第一层信息
		for (g_itrPSR1=g_vRPayStatusResetSvc.begin(); g_itrPSR1!=g_vRPayStatusResetSvc.end(); ++g_itrPSR1)
		{
			// 根据SVC解detail包 - 第二层
			if ((iSize=m_poUnpack->getR_PSRMicropaymentInfo(*g_itrPSR1,g_vPSRMicropaymentInfo)) == 0 )
			{
				__DEBUG_LOG0_(0, "PayStatusReset::deal 请求明细消息包为NULL");
				m_poPack->setRltCode(ECODE_NOMSG);
				throw DEDUCT_UNPACK_NOMSG;
			}
			else if (iSize < 0)
			{
				__DEBUG_LOG0_(0, "PayStatusReset::deal 消息解包出错!");
				m_poPack->setRltCode(ECODE_UNPACK_FAIL);
				throw DEDUCT_UNPACK_ERR;
			}
			 
			__DEBUG_LOG0_(0, "测试:处理第一层信息........");	

			iRet = _deal(*g_itrPSR1,g_vPSRMicropaymentInfo);

			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "支付状态变更操作失败");
				throw iRet;
			}
		}
		//return iRet;
	}
	catch(TTStatus oSt)
	{
		iRet = DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "PayStatusReset::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value)
	{
		iRet=value;
		__DEBUG_LOG1_(0, "支付状态变更失败:: 失败信息 value=%d", value);
	}
	if(iRet != 0)
	{
		// 打返回CCA包,返回错误码
		oSvc.m_svcResultCode=iRet;
		if (!m_poPack->addA_PayStatusReset(oSvc))
		{
			__DEBUG_LOG0_(0, "PayStatusReset:: _deal m_poPack 打包失败!");
			m_poPack->clear();
			m_poPack->setRltCode(ECODE_PACK_FAIL);
		}
		__DEBUG_LOG0_(0, "支付能力状态查询失败,iRet=[%d]",oSvc.m_svcResultCode);
		
	}
	return iRet;
}


int PayStatusReset::_deal(R_PayStatusReset *pSvcIn,vector<R_PSRMicropaymentInfo *> g_vPSRMicropaymentInfo)
{
	
	ABMException oExp;
		
	int iRet=0;
	time_t tTime;
	char sResponseTime[32]={0};
	char tPasswd[32];
	bool bState=false;
	
	memset(tPasswd,0,sizeof(tPasswd));
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "========================支付状态变更=======================");
	__DEBUG_LOG0_(0, "=====>start,取第一层CCR服务信息:");
	
	__DEBUG_LOG1_(0, "请求流水：m_requestId:%s",pSvcIn->m_requestId);
	__DEBUG_LOG1_(0, "请求时间：m_requestTime：%s",pSvcIn->m_requestTime);


	UserSelfService pUserSelfService={0};
	StructPaymentSeqInfo  sStructPaymentSeqInfo={0};
	PaymentInfoALLCCR pPaymentInfoCCR={0};
	

	strcpy(pUserSelfService.m_requestId,pSvcIn->m_requestId);		// 请求流水号
	strcpy(pUserSelfService.m_requestTime,pSvcIn->m_requestTime);	// 请求时间

			
	A_PayStatusReset oSvc={0} ;	// CCA 响应服务信息

	try
	{	

		for (g_itrPSR2=g_vPSRMicropaymentInfo.begin(); g_itrPSR2!=g_vPSRMicropaymentInfo.end(); ++g_itrPSR2)
		{	
			////CCR
			strcpy(pUserSelfService.m_servNbr,(*g_itrPSR2)->m_servNbr);		//用户号码
			__DEBUG_LOG1_(0, "m_servNbr:%s", (*g_itrPSR2)->m_servNbr);
			
			pUserSelfService.m_nbrType =(*g_itrPSR2)->m_nbrType;		//号码类型
			__DEBUG_LOG1_(0, "m_nbrType:%d",(*g_itrPSR2)->m_nbrType);
				
			pUserSelfService.m_servAttr = (*g_itrPSR2)->m_servAttr;		//用户属性
			__DEBUG_LOG1_(0, "m_servAttr:%d",(*g_itrPSR2)->m_servAttr);
			
			strcpy(pUserSelfService.m_sServicePlatformID,(*g_itrPSR2)->m_sServicePlatformID);//业务平台ID
			__DEBUG_LOG1_(0, "m_sServicePlatformID:%s",(*g_itrPSR2)->m_sServicePlatformID);
		
			strcpy(tPasswd,(*g_itrPSR2)->m_payPassword);//支付密码
			__DEBUG_LOG1_(0, "m_payPassword:%s",(*g_itrPSR2)->m_payPassword);
		
			strcpy(pUserSelfService.m_payStatus,(*g_itrPSR2)->m_payStatus);//支付状态
			__DEBUG_LOG1_(0, "m_payStatus:%s",(*g_itrPSR2)->m_payStatus);
			
			tTime = time(NULL);
			tTime += 2209017599;
			long2date(tTime,sResponseTime);
			strcpy(oSvc.m_responseTime,sResponseTime);
		
			strcpy(oSvc.m_responseId,pUserSelfService.m_requestId);
			strcpy(oSvc.m_payStatus,pUserSelfService.m_payStatus);
			
			__DEBUG_LOG1_(0, "m_responseTime:%s",oSvc.m_responseTime);
			__DEBUG_LOG1_(0, "m_responseId:%s",oSvc.m_responseId);
			__DEBUG_LOG1_(0, "m_payStatus:%s",oSvc.m_payStatus);
		
			//密码鉴权
			if( ( iRet = m_poSqlPasswd->PasswordResetAuthen(pUserSelfService,oExp) ) <0)
			{
				return PAY_PASSWD_AUTHEN_ERR;
			}
			
			__DEBUG_LOG2_(0, "鉴权密码%s==%s",pUserSelfService.m_payPassword,tPasswd);
			

			// 调用业务逻辑处理
			if(strncmp(pUserSelfService.m_payPassword,tPasswd,strlen(pUserSelfService.m_payPassword))==0 && \
				strncmp(pUserSelfService.m_payPassword,tPasswd,strlen(tPasswd))==0)
			{
				//查询用户标识入业务记录表
				if( ( iRet = m_poSqlPasswd->PasswordResetSelect(pUserSelfService,oExp) ) <0)
				{
					__DEBUG_LOG0_(0, "查询业务流水失败!");
					throw PAY_PASSWD_SELECT_ERR;
				}
			
				//查询业务流水
				iRet=m_poSqlOpt->QueryPaySeq(sStructPaymentSeqInfo,bState);
				if(iRet!=0)
		        {
					__DEBUG_LOG0_(0, "查询业务流水失败!");
					throw iRet;
		        }
		        
				pUserSelfService.m_lPaymentSeq=sStructPaymentSeqInfo.m_lPaymentSeq;
			
				iRet = m_poSql->PayStatusResetUpdate(pUserSelfService,oExp);
				
				
				strcpy(pPaymentInfoCCR.m_sOperateSeq,pUserSelfService.m_requestId);  //外部流水
				strcpy(pPaymentInfoCCR.m_sCapabilityCode,"0015"); //业务编码
				pPaymentInfoCCR.m_lServID=atoi(pUserSelfService.m_userInfoId);  //用户表示
				strcpy(pPaymentInfoCCR.m_sDescription,"自服务平台支付状态变更");
				
				//插业务记录
				iRet=m_poSqlOpt->InsertPaymentInfo(pPaymentInfoCCR,sStructPaymentSeqInfo);
				if(iRet!=0)
	        	{
					__DEBUG_LOG0_(0, "记录业务信息表失败!");
					throw iRet;
	        	}
        	
			}
			else
			{
				oSvc.m_svcResultCode=PASSWD_AUTHEN_ERR;
			}
			
		}
		
		// 打包 - 第一层服务信息
		if (!m_poPack->addA_PayStatusReset(oSvc))
		{
			__DEBUG_LOG0_(0, "PayStatusReset:: _deal m_poPack 打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw DEDUCT_PACK_ERR;
		}
		// 测试，打包后，oSvc数据
		__DEBUG_LOG1_(0, "第一层服务CCA打包完成,oSvc,responseId:[%s]",oSvc.m_responseId);
		

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


static vector<R_PayRulesQuerySvc *> g_vPayRulesQuerySvc;
static vector<R_PayRulesQuerySvc *>::iterator g_itrPRQ1;

static vector<R_PRQMicropaymentInfo *> g_vPRQMicropaymentInfo;
static vector<R_PRQMicropaymentInfo *>::iterator g_itrPRQ2;


PayRulesQuery::PayRulesQuery()
{
		m_poSql = NULL;
		m_poUnpack = NULL;
		m_poPack = NULL;
		m_dccOperation = NULL;
}

PayRulesQuery::~PayRulesQuery()
{
	if (m_poSql != NULL)
		delete m_poSql;
	if (m_dccOperation != NULL)
		delete m_dccOperation;
}

int PayRulesQuery::init(ABMException &oExp)
{
	if ((m_poSql = new PayRulesQuerySql) == NULL)
	{
		ADD_EXCEPT0(oExp, "PayRulesQuery::init malloc failed!");
		return -1;
	}
	
	if ((m_dccOperation = new DccOperation) == NULL)
	{
		ADD_EXCEPT0(oExp, "DccOperation::init malloc failed!");
		return -1;
	}
	
	return 0;
}

int PayRulesQuery::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
	__DEBUG_LOG0_(0, "=========>测试：PayRulesQuery::deal 请求服务开始");
	
	int iRet=0;
	int iSize=0;

	g_toPlatformFlag = true;
	
	m_poUnpack = (PayRulesQueryCCR *)pCCR;
	m_poPack = (PayRulesQueryCCA *)pCCA;
	
	m_poPack->clear();
	g_vPayRulesQuerySvc.clear();
	g_vPRQMicropaymentInfo.clear();
	
	A_PayRulesQuerySvc oSvc={0} ;

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
			__DEBUG_LOG0_(0, "[支付规则查询-PayRulesQuery]:查询SEQ失败!");
			throw GET_DCC_INFO_RECORD_SEQ_ERR;	
		}
		
		// dcc包消息头信息
		__DEBUG_LOG0_(0, "支付规则查询,接收CCR包,消息头入TT!");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存接收的CCR包消息头信息失败");
			throw SAVE_RECEVIE_DCC_CCR_ERR;
		}
		
		__DEBUG_LOG0_(0, "支付规则查询,CCR消息包解包!");
		//消息解包 - 第一层
		if ((iSize=m_poUnpack->getR_PayRulesQuerySvc(g_vPayRulesQuerySvc)) == 0 )
		{
			__DEBUG_LOG0_(0, "支付规则查询::deal 请求服务消息包为NULL");
			m_poPack->setRltCode(ECODE_NOMSG);
			throw DEDUCT_UNPACK_NOMSG;
		}
		else if (iSize < 0)
		{
			__DEBUG_LOG0_(0, "支付规则查询::deal 消息解包出错!");
			m_poPack->setRltCode(ECODE_UNPACK_FAIL);
			throw DEDUCT_UNPACK_ERR;
		}
	
		
		// 处理第一层信息
		for (g_itrPRQ1=g_vPayRulesQuerySvc.begin(); g_itrPRQ1!=g_vPayRulesQuerySvc.end(); ++g_itrPRQ1)
		{
			// 根据SVC解detail包 - 第二层
			if ((iSize=m_poUnpack->getR_PRQMicropaymentInfo(*g_itrPRQ1,g_vPRQMicropaymentInfo)) == 0 )
			{
				__DEBUG_LOG0_(0, "PayRulesQuery::deal 请求明细消息包为NULL");
				m_poPack->setRltCode(ECODE_NOMSG);
				throw DEDUCT_UNPACK_NOMSG;
			}
			else if (iSize < 0)
			{
				__DEBUG_LOG0_(0, "PayRulesQuery::deal 消息解包出错!");
				m_poPack->setRltCode(ECODE_UNPACK_FAIL);
				throw DEDUCT_UNPACK_ERR;
			}
			 
			__DEBUG_LOG0_(0, "测试:处理第一层信息........");	

			iRet = _deal(*g_itrPRQ1,g_vPRQMicropaymentInfo);

			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "支付规则查询操作失败");
				throw iRet;
			}
		}
		//return iRet;
	}
	catch(TTStatus oSt)
	{
		iRet = DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "PayRulesQuery::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value)
	{
		iRet=value;
		__DEBUG_LOG1_(0, "支付规则查询失败:: 失败信息 value=%d", value);
	}
	if(iRet != 0)
	{
		// 打返回CCA包,返回错误码
		oSvc.m_svcResultCode=iRet;
		if (!m_poPack->addA_PayRulesQuerySvc(oSvc))
		{
			__DEBUG_LOG0_(0, "PayRulesQuery:: _deal m_poPack 打包失败!");
			m_poPack->clear();
			m_poPack->setRltCode(ECODE_PACK_FAIL);
		}
		__DEBUG_LOG0_(0, "支付规则查询失败,iRet=[%d]",oSvc.m_svcResultCode);
		
	}
	return iRet;
}


int PayRulesQuery::_deal(R_PayRulesQuerySvc *pSvcIn,vector<R_PRQMicropaymentInfo *> g_vPRQMicropaymentInfo)
{
	
	ABMException oExp;
		
	int iRet=0;
	time_t tTime;
	char sResponseTime[32]={0};
	
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "========================支付规则查询=======================");
	__DEBUG_LOG0_(0, "=====>start,取第一层CCR服务信息:");
	
	__DEBUG_LOG1_(0, "请求流水：m_requestId:%s",pSvcIn->m_requestId);
	__DEBUG_LOG1_(0, "请求时间：m_requestTime：%s",pSvcIn->m_requestTime);


	UserSelfService pUserSelfService={0};
	
	// 划拨CCR信息准备	
	strcpy(pUserSelfService.m_requestId,pSvcIn->m_requestId);		// 请求流水号
	strcpy(pUserSelfService.m_requestTime,pSvcIn->m_requestTime);	// 请求时间

			
	A_PayRulesQuerySvc oSvc={0} ;	// CCA 响应服务信息
	A_PRQMicropaymentInfo oData={0} ;	// CCA 响应扣费帐目明细信息
	vector<struct A_PRQMicropaymentInfo> vPQRMicropaymentInfo;

	try
	{	

		for (g_itrPRQ2=g_vPRQMicropaymentInfo.begin(); g_itrPRQ2!=g_vPRQMicropaymentInfo.end(); ++g_itrPRQ2)
		{	
			strcpy(pUserSelfService.m_servNbr,(*g_itrPRQ2)->m_servNbr);		//用户号码
			__DEBUG_LOG1_(0, "m_servNbr:%s", (*g_itrPRQ2)->m_servNbr);
			
			pUserSelfService.m_nbrType =(*g_itrPRQ2)->m_nbrType;		//号码类型
			__DEBUG_LOG1_(0, "m_nbrType:%d",(*g_itrPRQ2)->m_nbrType);
				
			pUserSelfService.m_servAttr = (*g_itrPRQ2)->m_servAttr;		//用户属性
			__DEBUG_LOG1_(0, "m_servAttr:%d",(*g_itrPRQ2)->m_servAttr);
			
			strcpy(pUserSelfService.m_sServicePlatformID,(*g_itrPRQ2)->m_sServicePlatformID);//业务平台ID
			__DEBUG_LOG1_(0, "m_sServicePlatformID:%s",(*g_itrPRQ2)->m_sServicePlatformID);
			
		
			// 调用业务逻辑处理
			iRet = m_poSql->PayRulesQueryInfo(pUserSelfService,oExp) ;
			
			
			tTime = time(NULL);
			tTime += 2209017599;
			long2date(tTime,sResponseTime);
			strcpy(oSvc.m_responseTime,sResponseTime);
			strcpy(oSvc.m_responseId,pUserSelfService.m_requestId);
			
			oData.m_datePayFreq=pUserSelfService.m_datePayFreq;
			oData.m_payLimit=pUserSelfService.m_payLimit;
			oData.m_monthPayLimit=pUserSelfService.m_monthPayLimit;
			oData.m_microPayLimit=pUserSelfService.m_microPayLimit;
			
			__DEBUG_LOG0_(0, "----------CCA---------");
			__DEBUG_LOG1_(0, "m_responseTime:%s",oSvc.m_responseTime);
			__DEBUG_LOG1_(0, "m_responseId:%s",oSvc.m_responseId);
			
			__DEBUG_LOG1_(0, "m_datePayFreq:%d",oData.m_datePayFreq);
			__DEBUG_LOG1_(0, "m_payLimit:%d",oData.m_payLimit);
			__DEBUG_LOG1_(0, "m_monthPayLimit:%d",oData.m_monthPayLimit);
			__DEBUG_LOG1_(0, "m_microPayLimit:%d",oData.m_microPayLimit);
			
			vPQRMicropaymentInfo.push_back(oData);
		}
		
		// 打包 - 第一层服务信息
		if (!m_poPack->addA_PayRulesQuerySvc(oSvc))
		{
			__DEBUG_LOG0_(0, "PayRulesQuery:: _deal m_poPack 打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw DEDUCT_PACK_ERR;
		}
		// 测试，打包后，oSvc数据
		__DEBUG_LOG1_(0, "第一层服务CCA打包完成,oSvc,responseId:[%s]",oSvc.m_responseId);
		
		// 打包 - 第二层扣费明细
		for(int j=0;j<vPQRMicropaymentInfo.size();j++)
		{
			if (!m_poPack->addA_PRQMicropaymentInfo(vPQRMicropaymentInfo[j]))
			{
				__DEBUG_LOG0_(0, "PayRulesQuery:: _deal m_poPack 打包失败!");
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




static vector<R_PayRulesResetSvc *> g_vPayRulesResetSvc;
static vector<R_PayRulesResetSvc *>::iterator g_itrPSE1;

static vector<R_PRRMicropaymentInfo *> g_vPRRMicropaymentInfo;
static vector<R_PRRMicropaymentInfo *>::iterator g_itrPSE2;


PayRulesReset::PayRulesReset()
{
		m_poSql = NULL;
		m_poUnpack = NULL;
		m_poSqlPass=NULL;
		m_poSqlPayRules=NULL;
		m_poSqlOpt=NULL;
		m_poPack = NULL;
		m_dccOperation = NULL;
}

PayRulesReset::~PayRulesReset()
{
	if (m_poSql != NULL)
		delete m_poSql;
	if (m_poSqlPass != NULL)
		delete m_poSqlPass;
	if (m_poSqlPayRules != NULL)
		delete m_poSqlPayRules;
	if (m_dccOperation != NULL)
		delete m_dccOperation;
	if (m_poSqlOpt != NULL)
		delete m_poSqlOpt;	
}

int PayRulesReset::init(ABMException &oExp)
{
	if ((m_poSql = new PayRulesResetSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "PayRulesReset::init malloc failed!");
		return -1;
	}
	
	if ((m_poSqlPass = new PasswordResetSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "PasswordResetSql::init malloc failed!");
		return -1;
	}
	if ((m_poSqlPayRules = new TransferRulesSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "TransferRulesSql::init malloc failed!");
		return -1;
	}
	
	if ((m_dccOperation = new DccOperation) == NULL)
	{
		ADD_EXCEPT0(oExp, "DccOperation::init malloc failed!");
		return -1;
	}
	
	if ((m_poSqlOpt = new PaymentInfoSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "PaymentInfoSql::init malloc failed!");
		return -1;
	}
	return 0;
}

int PayRulesReset::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
	__DEBUG_LOG0_(0, "=========>PayRulesReset::deal 请求服务开始");
	
	int iRet=0;
	int iSize=0;

	g_toPlatformFlag = true;
	
	m_poUnpack = (PayRulesResetCCR *)pCCR;
	m_poPack = (PayRulesResetCCA *)pCCA;
	
	m_poPack->clear();
	g_vPayRulesResetSvc.clear();
	g_vPRRMicropaymentInfo.clear();
	
	A_PayRulesResetSvc oSvc={0} ;

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
			__DEBUG_LOG0_(0, "[支付规则设置-PayRulesReset]:查询SEQ失败!");
			throw GET_DCC_INFO_RECORD_SEQ_ERR;	
		}
		
		// dcc包消息头信息
		__DEBUG_LOG0_(0, "支付规则设置,接收CCR包,消息头入TT!");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存接收的CCR包消息头信息失败");
			throw SAVE_RECEVIE_DCC_CCR_ERR;
		}
		
		__DEBUG_LOG0_(0, "支付规则设置,CCR消息包解包!");
		//消息解包 - 第一层
		if ((iSize=m_poUnpack->getR_PayRulesResetSvc(g_vPayRulesResetSvc)) == 0 )
		{
			__DEBUG_LOG0_(0, "支付规则设置::deal 请求服务消息包为NULL");
			m_poPack->setRltCode(ECODE_NOMSG);
			throw DEDUCT_UNPACK_NOMSG;
		}
		else if (iSize < 0)
		{
			__DEBUG_LOG0_(0, "支付规则设置::deal 消息解包出错!");
			m_poPack->setRltCode(ECODE_UNPACK_FAIL);
			throw DEDUCT_UNPACK_ERR;
		}
	
		
		// 处理第一层信息
		for (g_itrPSE1=g_vPayRulesResetSvc.begin(); g_itrPSE1!=g_vPayRulesResetSvc.end(); ++g_itrPSE1)
		{
			// 根据SVC解detail包 - 第二层
			if ((iSize=m_poUnpack->getR_PRRMicropaymentInfo(*g_itrPSE1,g_vPRRMicropaymentInfo)) == 0 )
			{
				__DEBUG_LOG0_(0, "PayRulesReset::deal 请求明细消息包为NULL");
				m_poPack->setRltCode(ECODE_NOMSG);
				throw DEDUCT_UNPACK_NOMSG;
			}
			else if (iSize < 0)
			{
				__DEBUG_LOG0_(0, "PayRulesReset::deal 消息解包出错!");
				m_poPack->setRltCode(ECODE_UNPACK_FAIL);
				throw DEDUCT_UNPACK_ERR;
			}
			 
			__DEBUG_LOG0_(0, "测试:处理第一层信息........");	

			iRet = _deal(*g_itrPSE1,g_vPRRMicropaymentInfo);

			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "支付规则设置操作失败");
				throw iRet;
			}
		}
		//return iRet;
	}
	catch(TTStatus oSt)
	{
		iRet = DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "PayRulesReset::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value)
	{
		iRet=value;
		__DEBUG_LOG1_(0, "支付规则设置失败:: 失败信息 value=%d", value);
	}
	if(iRet != 0)
	{
		// 打返回CCA包,返回错误码
		oSvc.m_svcResultCode=iRet;
		if (!m_poPack->addA_PayRulesResetSvc(oSvc))
		{
			__DEBUG_LOG0_(0, "PayRulesReset:: _deal m_poPack 打包失败!");
			m_poPack->clear();
			m_poPack->setRltCode(ECODE_PACK_FAIL);
		}
		__DEBUG_LOG0_(0, "支付规则设置失败,iRet=[%d]",oSvc.m_svcResultCode);
		
	}
	return iRet;
}


int PayRulesReset::_deal(R_PayRulesResetSvc *pSvcIn,vector<R_PRRMicropaymentInfo *> g_vPRRMicropaymentInfo)
{
	
	ABMException oExp;
		
	int iRet=0;
	time_t tTime;
	char sResponseTime[32]={0};
	char tPasswd[32];
	bool bState=false;
	
	memset(tPasswd,0,sizeof(tPasswd));
	
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "========================支付规则设置=======================");
	__DEBUG_LOG0_(0, "=====>start,取第一层CCR服务信息:");
	
	__DEBUG_LOG1_(0, "请求流水：m_requestId:%s",pSvcIn->m_requestId);
	__DEBUG_LOG1_(0, "请求时间：m_requestTime：%s",pSvcIn->m_requestTime);


	UserSelfService pUserSelfService={0};
	StructPaymentSeqInfo  sStructPaymentSeqInfo={0};
	PaymentInfoALLCCR pPaymentInfoCCR={0};
	
	// 划拨CCR信息准备	
	strcpy(pUserSelfService.m_requestId,pSvcIn->m_requestId);		// 请求流水号
	strcpy(pUserSelfService.m_requestTime,pSvcIn->m_requestTime);	// 请求时间

			
	A_PayRulesResetSvc oSvc={0} ;	// CCA 响应服务信息
	A_PRRMicropaymentInfo oData={0} ;	// CCA 响应扣费帐目明细信息
	vector<struct A_PRRMicropaymentInfo> vPRRMicropaymentInfo;

	try
	{	

		for (g_itrPSE2=g_vPRRMicropaymentInfo.begin(); g_itrPSE2!=g_vPRRMicropaymentInfo.end(); ++g_itrPSE2)
		{	
			strcpy(pUserSelfService.m_servNbr,(*g_itrPSE2)->m_servNbr);		//用户号码
			__DEBUG_LOG1_(0, "m_servNbr:%s", (*g_itrPSE2)->m_servNbr);
			
			pUserSelfService.m_nbrType =(*g_itrPSE2)->m_nbrType;		//号码类型
			__DEBUG_LOG1_(0, "m_nbrType:%d",(*g_itrPSE2)->m_nbrType);
				
			pUserSelfService.m_servAttr = (*g_itrPSE2)->m_servAttr;		//用户属性
			__DEBUG_LOG1_(0, "m_servAttr:%d",(*g_itrPSE2)->m_servAttr);
			
			strcpy(pUserSelfService.m_sServicePlatformID,(*g_itrPSE2)->m_sServicePlatformID);//业务平台ID
			__DEBUG_LOG1_(0, "m_sServicePlatformID:%s",(*g_itrPSE2)->m_sServicePlatformID);
			
			strcpy(tPasswd,(*g_itrPSE2)->m_payPassword);//支付密码
			__DEBUG_LOG1_(0, "m_payPassword:%s",(*g_itrPSE2)->m_payPassword);
			
			pUserSelfService.m_datePayFreq =(*g_itrPSE2)->m_datePayFreq;		////每天支付频度（单位次）
			__DEBUG_LOG1_(0, "m_datePayFreq:%d",(*g_itrPSE2)->m_datePayFreq);
			
			pUserSelfService.m_payLimit =(*g_itrPSE2)->m_payLimit;		//每次支付限额（单位分）
			__DEBUG_LOG1_(0, "m_payLimit:%d",(*g_itrPSE2)->m_payLimit);
			
			pUserSelfService.m_monthPayLimit =(*g_itrPSE2)->m_monthPayLimit;		//每月支付限额（单位分）
			__DEBUG_LOG1_(0, "m_monthPayLimit:%d",(*g_itrPSE2)->m_monthPayLimit);
			
			pUserSelfService.m_microPayLimit =(*g_itrPSE2)->m_microPayLimit;		//小额支付阀值（单位分）位分）
			__DEBUG_LOG1_(0, "m_microPayLimit:%d",(*g_itrPSE2)->m_microPayLimit);
			
			tTime = time(NULL);
			tTime += 2209017599;
			long2date(tTime,sResponseTime);
			strcpy(oSvc.m_responseTime,sResponseTime);
			strcpy(oSvc.m_responseId,pUserSelfService.m_requestId);
			
			oData.m_datePayFreq=pUserSelfService.m_datePayFreq;
			oData.m_payLimit=pUserSelfService.m_payLimit;
			oData.m_monthPayLimit=pUserSelfService.m_monthPayLimit;
			oData.m_microPayLimit=pUserSelfService.m_microPayLimit;
			
			__DEBUG_LOG0_(0, "----------CCA---------");
			__DEBUG_LOG1_(0, "m_responseTime:%s",oSvc.m_responseTime);
			__DEBUG_LOG1_(0, "m_responseId:%s",oSvc.m_responseId);
			
			__DEBUG_LOG1_(0, "m_datePayFreq:%d",oData.m_datePayFreq);
			__DEBUG_LOG1_(0, "m_payLimit:%d",oData.m_payLimit);
			__DEBUG_LOG1_(0, "m_monthPayLimit:%d",oData.m_monthPayLimit);
			__DEBUG_LOG1_(0, "m_microPayLimit:%d",oData.m_microPayLimit);
			
			vPRRMicropaymentInfo.push_back(oData);
		
			if( ( iRet = m_poSqlPass->PasswordResetAuthen(pUserSelfService,oExp) ) <0)
			{
				oSvc.m_svcResultCode= PAY_PASSWD_AUTHEN_ERR;
			}
			
			__DEBUG_LOG2_(0, "鉴权密码%s==%s",pUserSelfService.m_payPassword,tPasswd);
			

			// 调用业务逻辑处理
			if(strncmp(pUserSelfService.m_payPassword,tPasswd,strlen(pUserSelfService.m_payPassword))==0 &&\
				strncmp(pUserSelfService.m_payPassword,tPasswd,strlen(tPasswd))==0 )
			{
				//查询用户标识给入业务记录用
				if( ( iRet = m_poSqlPass->PasswordResetSelect(pUserSelfService,oExp) ) <0)
				{
					__DEBUG_LOG0_(0, "查询业务流水失败!");
					throw PAY_PASSWD_SELECT_ERR;
		        
				}
				
				//查询业务流水
				iRet=m_poSqlOpt->QueryPaySeq(sStructPaymentSeqInfo,bState);
				if(iRet!=0)
		        {
					__DEBUG_LOG0_(0, "查询业务流水失败!");
					throw iRet;
		        }
		        
				pUserSelfService.m_lPaymentSeq=sStructPaymentSeqInfo.m_lPaymentSeq;
			
				if( (iRet = m_poSqlPayRules->TransferRulesQuery(pUserSelfService,oExp)) == 1 )
				{
					__DEBUG_LOG0_(0, "----查询没有划拨支付规则记录，可以新增---");
					iRet = m_poSqlPayRules->TransferRulesAcctId(pUserSelfService,oExp);
						
					pUserSelfService.m_dateTransFreq =0;
					pUserSelfService.m_transLimit =0;
					pUserSelfService.m_monthTransLimit =0;
					pUserSelfService.m_autotransflag =0;
					pUserSelfService.m_autobalancelimit =0;
					pUserSelfService.m_autoAmountlimit =0;
					__DEBUG_LOG1_(0, "---查询出账户标识:%d",pUserSelfService.m_acctId);
				
					if( (iRet = m_poSqlPayRules->TransferRulesInsert(pUserSelfService,oExp)) == 0)
					{
						__DEBUG_LOG0_(0, "----新增划拨支付规则成功---");
					}
					else
					{
							__DEBUG_LOG0_(0, "----新增划拨支付规则失败---");
							oSvc.m_svcResultCode=TRANSFERRULES_INSERT_ERR;
					}
				}
				else
				{
					__DEBUG_LOG0_(0, "----查询有划拨支付规则记录，直接修改---");
					iRet = m_poSql->PayRulesUpdate(pUserSelfService,oExp) ;
				}
				
				strcpy(pPaymentInfoCCR.m_sOperateSeq,pUserSelfService.m_requestId);  //外部流水
				strcpy(pPaymentInfoCCR.m_sCapabilityCode,"0013"); // 能力编码
				pPaymentInfoCCR.m_lServID=atoi(pUserSelfService.m_userInfoId);  //用户表示
				strcpy(pPaymentInfoCCR.m_sDescription,"自服务平台支付规则设置");
				
				//插业务记录
				iRet=m_poSqlOpt->InsertPaymentInfo(pPaymentInfoCCR,sStructPaymentSeqInfo);
				if(iRet!=0)
	        	{
					__DEBUG_LOG0_(0, "记录业务信息表失败!");
					throw iRet;
	        	}
			}
			else
			{
				
				oSvc.m_svcResultCode=PASSWD_AUTHEN_ERR;
				
			}
			
		}
		
		// 打包 - 第一层服务信息
		if (!m_poPack->addA_PayRulesResetSvc(oSvc))
		{
			__DEBUG_LOG0_(0, "PayRulesReset:: _deal m_poPack 打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw DEDUCT_PACK_ERR;
		}
		// 测试，打包后，oSvc数据
		__DEBUG_LOG1_(0, "第一层服务CCA打包完成,oSvc,responseId:[%s]",oSvc.m_responseId);
		
		// 打包 - 第二层扣费明细
		for(int j=0;j<vPRRMicropaymentInfo.size();j++)
		{
			if (!m_poPack->addA_PRRMicropaymentInfo(vPRRMicropaymentInfo[j]))
			{
				__DEBUG_LOG0_(0, "PayRulesReset:: _deal m_poPack 打包失败!");
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


static vector<R_BindingAccNbrQuery *> g_vRBindingAccNbrQuery;
static vector<R_BindingAccNbrQuery *>::iterator g_itrBDA1;

static vector<R_BDAMicropaymentInfo *> g_vRBDAMicropaymentInfo;
static vector<R_BDAMicropaymentInfo *>::iterator g_itrBDA2;


BindingAccNbrQuery::BindingAccNbrQuery()
{
		m_poSql = NULL;
		m_poUnpack = NULL;
		m_poPack = NULL;
		m_dccOperation = NULL;
}

BindingAccNbrQuery::~BindingAccNbrQuery()
{
	if (m_poSql != NULL)
		delete m_poSql;
	if (m_dccOperation != NULL)
		delete m_dccOperation;
}

int BindingAccNbrQuery::init(ABMException &oExp)
{
	if ((m_poSql = new BindingAccNbrQuerySql) == NULL)
	{
		ADD_EXCEPT0(oExp, "BindingAccNbrQuery::init malloc failed!");
		return -1;
	}
	if ((m_dccOperation = new DccOperation) == NULL)
	{
		ADD_EXCEPT0(oExp, "DccOperation::init malloc failed!");
		return -1;
	}
	return 0;
}

int BindingAccNbrQuery::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
	__DEBUG_LOG0_(0, "=========>测试：BindingAccNbrQuery::deal 请求服务开始");
	
	int iRet=0;
	int iSize=0;

	g_toPlatformFlag = true;
	
	m_poUnpack = (BindingAccNbrQueryCCR *)pCCR;
	m_poPack = (BindingAccNbrQueryCCA *)pCCA;
	
	m_poPack->clear();
	g_vRBindingAccNbrQuery.clear();
	g_vRBDAMicropaymentInfo.clear();
	
	A_BindingAccNbrQuery oSvc={0} ;

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
			__DEBUG_LOG0_(0, "[绑定号码查询-BindingAccNbrQuery]:查询SEQ失败!");
			throw GET_DCC_INFO_RECORD_SEQ_ERR;	
		}
		
		// dcc包消息头信息
		__DEBUG_LOG0_(0, "绑定号码查询,接收CCR包,消息头入TT!");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存接收的CCR包消息头信息失败");
			throw SAVE_RECEVIE_DCC_CCR_ERR;
		}
		
		__DEBUG_LOG0_(0, "绑定号码查询,CCR消息包解包!");
		//消息解包 - 第一层
		if ((iSize=m_poUnpack->getR_BindingAccNbrQuery(g_vRBindingAccNbrQuery)) == 0 )
		{
			__DEBUG_LOG0_(0, "绑定号码查询::deal 请求服务消息包为NULL");
			m_poPack->setRltCode(ECODE_NOMSG);
			throw DEDUCT_UNPACK_NOMSG;
		}
		else if (iSize < 0)
		{
			__DEBUG_LOG0_(0, "绑定号码查询::deal 消息解包出错!");
			m_poPack->setRltCode(ECODE_UNPACK_FAIL);
			throw DEDUCT_UNPACK_ERR;
		}
	
		
		// 处理第一层信息
		for (g_itrBDA1=g_vRBindingAccNbrQuery.begin(); g_itrBDA1!=g_vRBindingAccNbrQuery.end(); ++g_itrBDA1)
		{
			// 根据SVC解detail包 - 第二层
			if ((iSize=m_poUnpack->getR_BDAMicropaymentInfo(*g_itrBDA1,g_vRBDAMicropaymentInfo)) == 0 )
			{
				__DEBUG_LOG0_(0, "BindingAccNbrQuery::deal 请求明细消息包为NULL");
				m_poPack->setRltCode(ECODE_NOMSG);
				throw DEDUCT_UNPACK_NOMSG;
			}
			else if (iSize < 0)
			{
				__DEBUG_LOG0_(0, "BindingAccNbrQuery::deal 消息解包出错!");
				m_poPack->setRltCode(ECODE_UNPACK_FAIL);
				throw DEDUCT_UNPACK_ERR;
			}
			 
			__DEBUG_LOG0_(0, "测试:处理第一层信息........");	

			iRet = _deal(*g_itrBDA1,g_vRBDAMicropaymentInfo);

			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "绑定号码查询操作失败");
				throw iRet;
			}
		}
		//return iRet;
	}
	catch(TTStatus oSt)
	{
		iRet = DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "BindingAccNbrQuery::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value)
	{
		iRet=value;
		__DEBUG_LOG1_(0, "绑定号码查询失败:: 失败信息 value=%d", value);
	}
	if(iRet != 0)
	{
		// 打返回CCA包,返回错误码
		oSvc.m_svcResultCode=iRet;
		if (!m_poPack->addA_BindingAccNbrQuery(oSvc))
		{
			__DEBUG_LOG0_(0, "BindingAccNbrQuery:: _deal m_poPack 打包失败!");
			m_poPack->clear();
			m_poPack->setRltCode(ECODE_PACK_FAIL);
		}
		__DEBUG_LOG0_(0, "绑定号码查询失败,iRet=[%d]",oSvc.m_svcResultCode);
		
	}
	return iRet;
}


int BindingAccNbrQuery::_deal(R_BindingAccNbrQuery *pSvcIn,vector<R_BDAMicropaymentInfo *> g_vRBDAMicropaymentInfo)
{
	
	ABMException oExp;
		
	int iRet=0;
	time_t tTime;
	char sResponseTime[32]={0};
	
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "========================绑定号码查询=======================");
	__DEBUG_LOG0_(0, "=====>start,取第一层CCR服务信息:");
	
	__DEBUG_LOG1_(0, "请求流水：m_requestId:%s",pSvcIn->m_requestId);
	__DEBUG_LOG1_(0, "请求时间：m_requestTime：%s",pSvcIn->m_requestTime);


	UserSelfService pUserSelfService={0};
	
	// 划拨CCR信息准备	
	strcpy(pUserSelfService.m_requestId,pSvcIn->m_requestId);		// 请求流水号
	strcpy(pUserSelfService.m_requestTime,pSvcIn->m_requestTime);	// 请求时间

	A_BindingAccNbrQuery oSvc={0} ;	// CCA 响应服务信息
	
	try
	{	

		for (g_itrBDA2=g_vRBDAMicropaymentInfo.begin(); g_itrBDA2!=g_vRBDAMicropaymentInfo.end(); ++g_itrBDA2)
		{	
			strcpy(pUserSelfService.m_servNbr,(*g_itrBDA2)->m_servNbr);		//用户号码
			__DEBUG_LOG1_(0, "m_servNbr:%s", (*g_itrBDA2)->m_servNbr);
			
			pUserSelfService.m_nbrType =(*g_itrBDA2)->m_nbrType;		//号码类型
			__DEBUG_LOG1_(0, "m_nbrType:%d",(*g_itrBDA2)->m_nbrType);
				
			pUserSelfService.m_servAttr = (*g_itrBDA2)->m_servAttr;		//用户属性
			__DEBUG_LOG1_(0, "m_servAttr:%d",(*g_itrBDA2)->m_servAttr);
			
			strcpy(pUserSelfService.m_sServicePlatformID,(*g_itrBDA2)->m_sServicePlatformID);//业务平台ID
			__DEBUG_LOG1_(0, "m_sServicePlatformID:%s",(*g_itrBDA2)->m_sServicePlatformID);
		
			
		
			// 调用业务逻辑处理
			iRet = m_poSql->BindingAccNbrSelect(pUserSelfService,oExp);
			
			tTime = time(NULL);
			tTime += 2209017599;
			long2date(tTime,sResponseTime);
			strcpy(oSvc.m_responseTime,sResponseTime);
		
			strcpy(oSvc.m_responseId,pUserSelfService.m_requestId);
			strcpy(oSvc.m_randomAccNbr,pUserSelfService.m_randomAccNbr);
			
			__DEBUG_LOG1_(0, "m_responseTime:%s",oSvc.m_responseTime);
			__DEBUG_LOG1_(0, "m_responseId:%s",oSvc.m_responseId);
			__DEBUG_LOG1_(0, "m_randomAccNbr:%s",oSvc.m_randomAccNbr);

		}
		
		// 打包 - 第一层服务信息
		if (!m_poPack->addA_BindingAccNbrQuery(oSvc))
		{
			__DEBUG_LOG0_(0, "BindingAccNbrQuery:: _deal m_poPack 打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw DEDUCT_PACK_ERR;
		}
		// 测试，打包后，oSvc数据
		__DEBUG_LOG1_(0, "第一层服务CCA打包完成,oSvc,responseId:[%s]",oSvc.m_responseId);
		
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

