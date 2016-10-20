/*
 * 支付冲正
 */
#include "ReversePay.h"

#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"
#include "errorcode_pay.h"

static vector<ReversePaySvc *> g_vReversePaySvc;
static vector<ReversePaySvc *>::iterator g_itr;

ReversePay::ReversePay()
{
	m_poUnpack = NULL;
	m_poPack = NULL;
	
	m_paymentInfo = new PaymentInfo;
	m_dccOperation = new DccOperation;
}

ReversePay::~ReversePay()
{
	if (m_paymentInfo != NULL)
		delete m_paymentInfo;
	if (m_dccOperation != NULL)
		delete m_dccOperation;
}

int ReversePay::init(ABMException &oExp)
{
	return 0;
}

int ReversePay::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
	__DEBUG_LOG0_(0, "=========>支付冲正测试,请求服务开始<=============");
	
	int iRet=0;
	int iSize=0;
	m_poUnpack = (ReversePayCCR *)pCCR;
	m_poPack = (ReversePayCCA *)pCCA;
	
	m_poPack->clear();
	g_vReversePaySvc.clear();

	//消息解包
	if ((iSize=m_poUnpack->getReversePaySvc(g_vReversePaySvc)) == 0)
	{
		__DEBUG_LOG0_(0, "ReversePay::deal 消息包为NULL");
		m_poPack->setRltCode(ECODE_NOMSG);
		return -1;
	}
	else if (iSize < 0)
	{
		__DEBUG_LOG0_(0, "ReversePay::deal 消息解包出错!");
		m_poPack->setRltCode(ECODE_UNPACK_FAIL);
		return -1;
	}

	try
	{
		__DEBUG_LOG0_(0, "调用支付冲正业务逻辑处理..............");
		for (g_itr=g_vReversePaySvc.begin(); g_itr!=g_vReversePaySvc.end(); ++g_itr) 
		{
			if (_deal(*g_itr) != 0)
			break;
		}
		return 0;
	}
	catch(TTStatus oSt)
	{
		__DEBUG_LOG1_(0, "ReversePay::deal oSt.err_msg=%s", oSt.err_msg);
	}
	m_poPack->clear();
	m_poPack->setRltCode(ECODE_TT_FAIL);
	
	return 0;
}

int ReversePay::_deal(ReversePaySvc *pSvcIn)
{
	int iRet=0;
	// 测试--取第一层CCR信息
	__DEBUG_LOG0_(0, "========================支付冲正处理=======================");
	__DEBUG_LOG1_(0,"===>oSvcCCR.lOperationAction=[%ld]",pSvcIn->lOperationAction);
	__DEBUG_LOG1_(0,"===>oSvcCCR.sReqSerial=[%s]",pSvcIn->sReqSerial);
	__DEBUG_LOG1_(0,"===>oSvcCCR.sSpId=[%s]",pSvcIn->sSpId);
	__DEBUG_LOG1_(0,"===>oSvcCCR.sSpName=[%s]",pSvcIn->sSpName);
	__DEBUG_LOG1_(0,"===>oSvcCCR.sServicePlatformId=[%s]",pSvcIn->sServicePlatformId);
	__DEBUG_LOG1_(0,"===>oSvcCCR.sStaffId=[%d]",pSvcIn->iStaffId);
	
	
	// 获取CCR数据信息
	ReversePaySvc reverseBizInfo={0};
	
	reverseBizInfo.lOperationAction = pSvcIn->lOperationAction;	// 
	strcpy(reverseBizInfo.sOperationSeq,pSvcIn->sOperationSeq);	// 操作流水
	strcpy(reverseBizInfo.sReqSerial,pSvcIn->sReqSerial);	// 需要取消扣费的原支付操作流水号
	strcpy(reverseBizInfo.sSpId,pSvcIn->sSpId);			// 商家ID
	strcpy(reverseBizInfo.sSpName,pSvcIn->sSpName);			// 商家名称
	strcpy(reverseBizInfo.sServicePlatformId,pSvcIn->sServicePlatformId);// 平台ID
	reverseBizInfo.iStaffId = pSvcIn->iStaffId;
	
	// 支付冲正业务逻辑处理
	iRet = reversePay(reverseBizInfo);
	__DEBUG_LOG0_(0, "iRet = reversePay(reverseBizInfo),iRet = [%d]",iRet);
	if(iRet !=0)
	{
		__DEBUG_LOG0_(0, "========================支付冲正处理失败=======================");
		return -1;
	}
	
	ReversePayRes1 oSvc;			// CCA1
	vector<ReversePayRes2> vReversePayRes2; // CCA2
	
	// 将业务处理结果赋值给CCA
	strcpy(oSvc.sParaFieldResult,"avp-err-ccr-code");
	
	ReversePayRes2 reversePayRes2;
	strcpy(reversePayRes2.sOperateSeq,reverseBizInfo.sOperationSeq); // 冲正请求流水
	strcpy(reversePayRes2.sReqSerial,reverseBizInfo.sReqSerial);  // 被冲正流水号
	strcpy(reversePayRes2.sParaFieldResult,"avp-err-cca-code");
	
	vReversePayRes2.push_back(reversePayRes2);
	
	// CCA信息打包
	// 打包 - 第一层服务信息
	if (!m_poPack->addReversePayCCA1(oSvc))
	{
		__DEBUG_LOG0_(0, "ReversePay:: _deal m_poPack 打包失败!");
		m_poPack->setRltCode(ECODE_PACK_FAIL);
		return -1;
	}
	
	// 测试，打包后，oSvc数据
	__DEBUG_LOG1_(0, "第一层服务CCA打包完成，oSvc：%d",oSvc.lServiceRltCode);
	
	
	// 打包 - 第二层信息
	for(int j=0;j<vReversePayRes2.size();j++)
	{
		if (!m_poPack->addReversePayCCA2(vReversePayRes2[j]))
		{
			__DEBUG_LOG0_(0, "ReversePay:: _deal m_poPack 打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			return -1;
		}	
	}
	
	__DEBUG_LOG0_(0, "第二层服务CCA打包完成.");
	
	
	return 0;
}


int ReversePay::reversePay(ReversePaySvc &oRvsInfo)
{
	bool bFlag=false;
	int iRet=0;
	long lDeductAmount=0L;
	long m_lBalance=0L;
	char sOldRequestId[64]={0};

	// 被冲正的业务流水号：oRvsInfo.m_sOldRequestId
	strcpy(sOldRequestId,oRvsInfo.sReqSerial);
	__DEBUG_LOG1_(0, "被冲正流水号：ReversePaySvc::sReqSerial=[%s]",oRvsInfo.sReqSerial);
	//__DEBUG_LOG1_(0, "ReversePaySvc::sReqSerial=[%s]",sOldRequestId.c_str());
	vector<BalancePayoutInfo*>  vBalancePayOut;
	try
	{
		//划拨冲正业务处理逻辑
		// 1.根据被冲正的划拨流水号，查找业务记录表，获取冲正总金额
		//m_paymentInfo->qryOptInfo();
		
		// 2.根据业务记录流水号找余额支出记录
		//long lOldRequestId = atol(sOldRequestId.c_str()); // 类型不匹配，暂时写死
		
		// 余额支出冲正
		iRet = m_paymentInfo->ReversePayoutInfo(sOldRequestId,oRvsInfo,vBalancePayOut);
		
		if(iRet !=0)
		{
			__DEBUG_LOG0_(0, "ReverseDeductBalance::reversePay 取支出信息错误");
		}
		__DEBUG_LOG1_(0, "===>支出信息,业务流水号OPT_ID:%d",vBalancePayOut[0]->m_lOptId);
		__DEBUG_LOG1_(0, "===>支出信息,支出金额Amount:%d",vBalancePayOut[0]->m_lAmount);
		__DEBUG_LOG1_(0, "===>支出信息:%d",vBalancePayOut[0]->m_lBalance);
		
	}
	catch(TTStatus oSt) 
	{
		iRet=-1;
		__DEBUG_LOG1_(0, "ReverseDeductBalance::reversePay oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "ReverseDeductBalance::reversePay value=%d",value);
	}	
	
	return iRet;
}
