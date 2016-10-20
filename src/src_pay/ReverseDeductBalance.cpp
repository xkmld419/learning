// ReverseDeductBalance.cpp
#include "ReverseDeductBalance.h"
#include "AllocateBalanceSql.h"

#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"
#include "PublicConndb.h"
#include "errorcode_pay.h"

static vector<ReqReverseDeductSvc *> g_vReqReverseDeductSvc;
static vector<ReqReverseDeductSvc *>::iterator g_itr;

extern TimesTenCMD *m_poBalance;

ReverseDeductBalance::ReverseDeductBalance()
{
		m_poSql = NULL;
		m_poUnpack = NULL;
		m_poPack = NULL;
		m_paymentInfo = new PaymentInfo;
		m_dccOperation = new DccOperation;
}

ReverseDeductBalance::~ReverseDeductBalance()
{
	if (m_poSql != NULL)
		delete m_poSql;
	if (m_paymentInfo != NULL)
		delete m_paymentInfo;
	if (m_dccOperation != NULL)
		delete m_dccOperation;
}

int ReverseDeductBalance::init(ABMException &oExp)
{
	if ((m_poSql = new AllocateBalanceSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "ReverseDeductBalance::init malloc failed!");
		return -1;
	}
	return 0;
}

int ReverseDeductBalance::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
	__DEBUG_LOG0_(0, "=========>余额划拨冲正,请求服务开始<=============");
	
	int iRet=0;
	int iSize=0;

	m_poUnpack = (ReverseDeductBalanceCCR *)pCCR;
	m_poPack = (ReverseDeductBalanceCCA *)pCCA;
	
	m_poPack->clear();
	g_vReqReverseDeductSvc.clear();

	ResReverseDeductData oSvcData={0};

	//接收的CCR消息头入TT
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
			__DEBUG_LOG0_(0, "[划拨冲正-ReverseDeductBalanc]:查询SEQ失败!");
			throw GET_DCC_INFO_RECORD_SEQ_ERR;	
		}
		// dcc包消息头信息
		__DEBUG_LOG0_(0, "[划拨冲正-ReverseDeductBalanc]:保存接收的划拨冲正请求CCR包消息头信息，入TT!");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[划拨冲正-ReverseDeductBalanc]:保存接收的划拨冲正请求CCR包消息头信息失败!");
			throw SAVE_RECEVIE_DCC_CCR_ERR;
		}
		
		//消息解包 - 第一层
		if ((iSize=m_poUnpack->getReqReverseDeductSvc(g_vReqReverseDeductSvc)) == 0 )
		{
			__DEBUG_LOG0_(0, "[划拨冲正-ReverseDeductBalanc]:划拨冲正::deal 请求服务消息包为NULL");
			m_poPack->setRltCode(ECODE_NOMSG);
			throw UNPACK_REVERSE_DEDUCT_NOMSG;
		}
		else if (iSize < 0)
		{
			__DEBUG_LOG0_(0, "[划拨冲正-ReverseDeductBalanc]:划拨冲正::deal 消息解包出错!");
			m_poPack->setRltCode(ECODE_UNPACK_FAIL);
			throw UNPACK_REVERSE_DEDUCT_ERR;
		}
	
		__DEBUG_LOG0_(0, "[划拨冲正-ReverseDeductBalanc]:调用划拨冲正业务逻辑处理!");
		// 处理第一层信息
		for (g_itr=g_vReqReverseDeductSvc.begin(); g_itr!=g_vReqReverseDeductSvc.end(); ++g_itr)
		{			 
			__DEBUG_LOG0_(0, "[划拨冲正-ReverseDeductBalanc]:处理第一层信息!");	
			// 根据CCR请求信息执行业务逻辑处理
			iRet = _deal(*g_itr);
			if (iRet != 0)
			{
				strcpy(oSvcData.m_sResponseId,(*g_itr)->m_sRequestId);
				break;
			}
		}
	}
	catch(TTStatus oSt)
	{
		iRet = REVERSE_DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "[划拨冲正-ReverseDeductBalanc]:划拨冲正::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "[划拨冲正-ReverseDeductBalanc]:划拨冲正::deal 业务级返回结果iRet=%d",value);
	}
	
	if(iRet != 0)
	{
		oSvcData.m_iServiceRltCode=iRet;
		
		time_t tTime;
		tTime = time(NULL);
		oSvcData.m_iResponseTime = tTime;
		if (!m_poPack->addResReverseDeductData(oSvcData))
		{
			__DEBUG_LOG0_(0, "ReverseDeductBalance:: _deal CCA返回信息打包失败!");
			m_poPack->clear();
			m_poPack->setRltCode(ECODE_TT_FAIL);
		}
	}
	
	return iRet;
}

/*
 * 余额划拨冲正业务逻辑处理
 *
 */
int ReverseDeductBalance::_deal(ReqReverseDeductSvc *pSvcIn)
{
	int iRet=0;
	
	if(pSvcIn->m_iRequestTime > 0)
		long2date(pSvcIn->m_iRequestTime,pSvcIn->m_sRequestTime);
	else
		GetTime(pSvcIn->m_sRequestTime);
	
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "========================余额划拨冲正处理=======================");
	__DEBUG_LOG1_(0,"===>划拨冲正，请求流水：oSvcCCR.m_sRequestId=[%s]",pSvcIn->m_sRequestId);
	__DEBUG_LOG1_(0,"===>划拨冲正，被冲正流水：oSvcCCR.m_sOldRequestId=[%s]",pSvcIn->m_sOldRequestId);
	__DEBUG_LOG1_(0,"===>划拨冲正，请求时间[秒数]：oSvcCCR.m_iRequestTime=[%u]",pSvcIn->m_iRequestTime);
	__DEBUG_LOG1_(0,"===>划拨冲正，请求时间[日期]：oSvcCCR.m_sRequestTime=[%s]",pSvcIn->m_sRequestTime);
	__DEBUG_LOG1_(0,"===>划拨冲正，发起ABM平台标识：oSvcCCR.m_sABMId=[%s]",pSvcIn->m_sABMId);
	__DEBUG_LOG1_(0,"===>划拨冲正，冲正操作类型：oSvcCCR.m_iActionType=[%d]",pSvcIn->m_iActionType);
	
	ReqReverseDeductSvc oRvsInfo={0};
	
	strcpy(oRvsInfo.m_sRequestId,pSvcIn->m_sRequestId);
	strcpy(oRvsInfo.m_sOldRequestId,pSvcIn->m_sOldRequestId);
	oRvsInfo.m_iRequestTime = pSvcIn->m_iRequestTime;
	strcpy(oRvsInfo.m_sRequestTime,pSvcIn->m_sRequestTime);
	strcpy(oRvsInfo.m_sABMId,pSvcIn->m_sABMId);
	oRvsInfo.m_iActionType = pSvcIn->m_iActionType;
	
	ResReverseDeductData oSvcData={0};
	try 
	{
		iRet = reverseDeduct(oRvsInfo);
		
		if(iRet !=0)
		{
			__DEBUG_LOG0_(0, "[划拨冲正-ReverseDeductBalanc]:业务处理失败!");
			throw REVERSE_DEDUCT_BIZ_ERR;
		}
		
		__DEBUG_LOG0_(0, "[划拨冲正-ReverseDeductBalanc]:CCA返回信息打包!");
		
		// 测试数据
		//strcpy(oSvcData.m_sResponseId,"6684804");
		//oSvcData.m_iServiceRltCode=0;
		//strcpy(oSvcData.m_sResponseTime,"2011-5-18");
		
		// 返回CCA赋值
		strcpy(oSvcData.m_sResponseId,pSvcIn->m_sRequestId);
		oSvcData.m_iServiceRltCode=iRet;
		time_t tTime;
		
		tTime = time(NULL);
		oSvcData.m_iResponseTime = tTime+2209017599;
		
		__DEBUG_LOG1_(0, "[划拨冲正-响应时间:[%u]",oSvcData.m_iResponseTime);
		// 通过转换函数取当前时间秒数
		// date2long(oSvcData.m_iResponseTime);
		
		// 打包 - 第一层CCA响应服务信息
		if (!m_poPack->addResReverseDeductData(oSvcData))
		{
			__DEBUG_LOG0_(0, "ReverseDeductBalance:: _deal CCA返回信息打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw REVERSE_DEDUCT_PACK_ERR;
		}
		
		__DEBUG_LOG1_(0, "[划拨冲正-ReverseDeductBalanc]:返回CCA打包完成,oSvc:%s",oSvcData.m_sResponseId);
	}
	catch(TTStatus oSt)
	{
		iRet = REVERSE_DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "[划拨冲正-ReverseDeductBalanc]:ReverseDeductBalance::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "[划拨冲正-ReverseDeductBalanc]:ReverseDeductBalance::deal value=%d",value);
	}	

	return iRet;	
}


/*
 * 划拨冲正动作
 */
int ReverseDeductBalance::reverseDeduct(ReqReverseDeductSvc &oRvsInfo)
{
	bool bFlag=false;
	int iRet=0;
	long lDeductAmount=0L;
	long m_lBalance=0L;
	char sOldRequestId[UTF8_MAX]={0};

	// 被冲正的业务流水号：oRvsInfo.m_sOldRequestId
	//string sOldRequestId = oRvsInfo.m_sOldRequestId;
	strcpy(sOldRequestId,oRvsInfo.m_sOldRequestId);
	__DEBUG_LOG1_(0, "ReverseDeductBalance::sOldRequestId=[%s]",oRvsInfo.m_sOldRequestId);
	//__DEBUG_LOG1_(0, "ReverseDeductBalance::sOldRequestId=[%s]",sOldRequestId.c_str());
	vector<BalancePayoutInfo*>  vBalancePayOut;
	try
	{
		//划拨冲正业务处理逻辑
		// 1.根据被冲正的划拨流水号，查找业务记录表，获取冲正总金额
		//m_paymentInfo->qryOptInfo();
		
		// 2.根据业务记录流水号找余额支出记录	
	
		// 余额划拨冲正
		iRet = m_paymentInfo->ReverseDeductBalance(sOldRequestId,oRvsInfo);
		if(iRet !=0)
		{
			throw REVERSE_DEDUCT_BIZ_ERR;
			__DEBUG_LOG0_(0, "划拨冲正业务处理失败");
		}	
	}
	catch(TTStatus oSt) 
	{
		iRet=REVERSE_DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "ReverseDeductBalance::reversePay oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "ReverseDeductBalance::reversePay value=%d",value);
	}	
	
	return iRet;
}

