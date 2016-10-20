#include "ActiveAllocateBalance.h"
#include "AllocateBalanceSql.h"

#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"
#include "PublicConndb.h"
#include "errorcode_pay.h"

static vector<ReqPlatformSvc *> g_vReqPlatformSvc;
static vector<ReqPlatformSvc *>::iterator g_itr1;
	
static vector<ReqPlatformPayInfo *> g_vReqPlatformPayInfo;
static vector<ReqPlatformPayInfo *>::iterator g_itr2;

extern TimesTenCMD *m_poBalance;

ActiveAllocateBalance::ActiveAllocateBalance()
{
		m_poSql = NULL;
		m_poUnpack = NULL;
		m_poPack = NULL;
		m_poActivePack = NULL;
		m_paymentInfo = new PaymentInfo;
		
		m_dccOperation = new DccOperation;
		
		
}

ActiveAllocateBalance::~ActiveAllocateBalance()
{
	if (m_poSql != NULL)
		delete m_poSql;	
	if (m_paymentInfo != NULL)
		delete m_paymentInfo;
			
	if (m_dccOperation != NULL)
		delete m_dccOperation;
}

int ActiveAllocateBalance::init(ABMException &oExp)
{
	if ((m_poSql = new AllocateBalanceSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "AllocateBalance::init malloc failed!");
		return -1;
	}
	
	return 0;
}

int ActiveAllocateBalance::deal(ABMCCR *pCCR, ABMCCA *pSendPack)
{
	__DEBUG_LOG0_(0, "=========ActiveAllocateBalance::deal 请求服务开始============");
	__DEBUG_LOG0_(0, "[主动划拨-收发CCR,ActiveAllocateBalance]：接收来自服务的请求CCR!");
	
	int iRet=0;
	int iSize=0;
	
	m_poUnpack = (PlatformDeductBalanceCCR *)pCCR; // 解包请求CCR
	m_poPack = (AllocateBalanceCCR *)pSendPack;    // 打包请求CCR
	
	m_poPack->clear();
	g_vReqPlatformSvc.clear();
	g_vReqPlatformPayInfo.clear();

	//消息解包 - 第一层
	if ((iSize=m_poUnpack->getReqPlatformSvc(g_vReqPlatformSvc)) == 0 )
	{
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,ActiveAllocateBalance]:deal 自服务平台请求划拨，请求服务消息包为NULL!");
		m_poPack->setRltCode(ECODE_NOMSG);
		return UNPACK_PLATFORM_CCR_NOMSG;
	}
	else if (iSize < 0)
	{
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,ActiveAllocateBalance]:deal 消息解包出错!");
		m_poPack->setRltCode(ECODE_UNPACK_FAIL);
		return UNPACK_PLATFORM_CCR_ERR;
	}
	
	try
	{		
		// 处理第一层信息
		for (g_itr1=g_vReqPlatformSvc.begin(); g_itr1!=g_vReqPlatformSvc.end(); ++g_itr1)
		{
			// 根据SVC解detail包 - 第二层
			if ((iSize=m_poUnpack->getReqPlatformPayInfo(*g_itr1,g_vReqPlatformPayInfo)) == 0 )
			{
				__DEBUG_LOG0_(0, "[主动划拨-收发CCR,ActiveAllocateBalance]:deal 请求明细消息包为NULL!");
				m_poPack->setRltCode(ECODE_NOMSG);
				throw UNPACK_PLATFORM_CCR_NOMSG;
			}
			else if (iSize < 0)
			{
				__DEBUG_LOG0_(0, "[主动划拨-收发CCR,ActiveAllocateBalance]:deal 消息解包出错!");
				m_poPack->setRltCode(ECODE_UNPACK_FAIL);
				throw UNPACK_PLATFORM_CCR_ERR;
			}
			 
			__DEBUG_LOG0_(0, "=========>2：_deal.处理自服务平台所请求的CCR信息.");			
			// 处理	
			iRet = 	_deal(*g_itr1,g_vReqPlatformPayInfo);	
			if (iRet != 0)
				break;
		}
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,ActiveAllocateBalance]:deal iRet=%d",iRet);
	}
	catch(TTStatus oSt)
	{
		iRet = ACTIVE_DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,ActiveAllocateBalance]:deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,ActiveAllocateBalance],业务处理结果码: iRet=%d",value);
	}
	
	// 业务处理失败，打返回包，TODO：
	if(iRet!=0)
	{
		m_poPack->clear();
		m_poPack->setRltCode(ECODE_TT_FAIL);
	}
	__DEBUG_LOG1_(0, "ActiveAllocateBalance::deal iRet=%d",iRet);
	return iRet;
}

// 划拨类型操作
int ActiveAllocateBalance::_deal(ReqPlatformSvc *pSvcIn,vector<ReqPlatformPayInfo *> g_vReqPlatformPayInfo)
{
	
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "========================余额划拨处理=======================");
	__DEBUG_LOG0_(0, "=========>取自服务平台请求CCR第一层服务信息:");
	__DEBUG_LOG1_(0, "[主动划拨-收发CCR,ActiveAllocateBalance],请求流水：m_requestId:%s",pSvcIn->m_requestId);
	//__DEBUG_LOG1_(0, "[主动划拨-收发CCR,ActiveAllocateBalance],请求时间：m_iRequestTime:%d",pSvcIn->m_iRequestTime);
	__DEBUG_LOG1_(0, "[主动划拨-收发CCR,ActiveAllocateBalance],请求时间：m_requestTime:%s",pSvcIn->m_requestTime);
	
	/* Modify 2011.7.7. 去除时间转换
	if(pSvcIn->m_iRequestTime > 0)
		long2date(pSvcIn->m_iRequestTime,pSvcIn->m_requestTime);
	else
		GetTime(pSvcIn->m_requestTime);
	*/
	int iRet=0;
	int iBalanceTransFlag; // 余额划拨类型
	
	try
	{
		for (g_itr2=g_vReqPlatformPayInfo.begin(); g_itr2!=g_vReqPlatformPayInfo.end(); ++g_itr2)
		{
		
			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,ActiveAllocateBalance]:取自服务平台请求CCR第二层支付信息：");
			 iBalanceTransFlag = (*g_itr2)->m_iBalanceTransFlag;
			__DEBUG_LOG1_(0, "[主动划拨-收发CCR,ActiveAllocateBalance],余额划拨类型：[%d](1-省到全国，2-全国到省)",iBalanceTransFlag);
			 switch(iBalanceTransFlag)
			 {
			 	case 1:
			 		// 省到全国
			 		// 省ABM被动划拨
			 		// 中心ABM做划拨充值业务
			 		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,ActiveAllocateBalance]:余额划拨，省到全国业务处理。");
			 		iRet = deductBalanceIn(pSvcIn,*g_itr2);
			 		if(iRet!=0)
			 		{
			 			throw CENTER_ABM_DEDUCT_IN_ERR;
			 			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,ActiveAllocateBalance]:余额划拨，省到全国业务处理失败");
			 		}
			 		break;
			 	case 2:
			 		// 全国到省
			 		// 中心ABM主动划出
			 		// 中心ABM做余额划出业务
			 		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,ActiveAllocateBalance]:余额划拨，全国到省业务处理。");
			 		 iRet = deductBalanceOut(pSvcIn,*g_itr2);
			 		if(iRet!=0)
			 		{
			 			throw CENTER_ABM_DEDUCT_OUT_ERR;
			 			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,ActiveAllocateBalance]:余额划拨，全国到省业务处理失败");
			 		}
			 		break;
			 }
		}
		
		for(int j=0;j<g_vReqPlatformPayInfo.size();j++)
		{
			delete g_vReqPlatformPayInfo[j];
			g_vReqPlatformPayInfo[j]= NULL;
		}
		g_vReqPlatformPayInfo.clear();
	}
	catch(TTStatus oSt) 
	{
		iRet=REVERSE_PAYMENT_TT_ERR;
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,ActiveAllocateBalance] oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,ActiveAllocateBalance] iRet=%d",value);
	}
	return iRet;	
}

// 划拨充值 
int ActiveAllocateBalance::deductBalanceIn(ReqPlatformSvc *pSvcIn,ReqPlatformPayInfo *g_itr2)
{
	int iRet=0;
	
	// 请求CCR信息
	R_AllocateBalanceSvc reqAllocateBalanceSvc;
	vector<R_RequestDebitAcctItem> vResAcctItem;

	// 请求明细
	R_RequestDebitAcctItem reqAllocateBalanceItem={0};
	// 存CCR信息
	__DEBUG_LOG0_(0, "========================请求省ABM充值CCR入TT=======================");
	__DEBUG_LOG0_(0, "[主动划拨-收发CCR,划拨充值]-自服平台请求CCR明细:");
	__DEBUG_LOG1_(0, "请求流水：m_requestId:[%s]",pSvcIn->m_requestId);
	__DEBUG_LOG1_(0, "请求时间：m_requestTime:[%s]",pSvcIn->m_requestTime);
	
	__DEBUG_LOG1_(0, "用户号码：m_sDestinationAccount:[%s]",g_itr2->m_sDestinationAccount);
	__DEBUG_LOG1_(0, "标识类型：m_iDestinationType:[%d]",g_itr2->m_iDestinationType);
	__DEBUG_LOG1_(0, "用户属性：m_iDestinationAttr:[%d]",g_itr2->m_iDestinationAttr);
	__DEBUG_LOG1_(0, "业务平台标识：m_sSvcPlatformId:[%s]",g_itr2->m_sSvcPlatformId);
	__DEBUG_LOG1_(0, "支付密码：m_sPayPassword:[%s]",g_itr2->m_sPayPassword);
	__DEBUG_LOG1_(0, "余额划拨类型：m_iBalanceTransFlag:[%d]",g_itr2->m_iBalanceTransFlag);
	__DEBUG_LOG1_(0, "余额的单位类型：m_iUnitTypeId:[%d]",g_itr2->m_iUnitTypeId);
	__DEBUG_LOG1_(0, "需划拨金额：m_lDeductAmount:[%ld]",g_itr2->m_lDeductAmount);
	
	struct struDccHead receiveDccHead;
	char dccType[1];	// DCC消息类型
	strcpy(dccType,"R"); 
	long lDccRecordSeq=0L;
	try
	{	
		// 取DCC_INFO_RECORD_SEQ
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		
		// dcc包消息头信息
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,划拨充值]:保存CCR包消息头信息,入TT");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,划拨充值]:保存CCR包消息头信息失败");
			throw SAVE_PLATFOR_CCR_ERR;
		}
		
		// dcc包业务数据信息
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,划拨充值]:保存CCR包业务信息，入TT");
		iRet = m_dccOperation->savePlatformCCRBiz(receiveDccHead.m_sSessionId,pSvcIn,g_itr2);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,划拨充值]:保存CCR业务信息失败");
			throw SAVE_PLATFOR_CCR_BIZ_ERR;
		}
	
		// 组CCR请求信息
		__DEBUG_LOG0_(0, "========================组CCR请求信息=======================");
		
		// 被动余额划拨CCR
		// 第一层CCR赋值
		strcpy(reqAllocateBalanceSvc.m_requestId,pSvcIn->m_requestId); // 请求流水
		strcpy(reqAllocateBalanceSvc.m_servNbr,g_itr2->m_sDestinationAccount);// 用户号码
		reqAllocateBalanceSvc.m_nbrType = 2;// 号码类型
		reqAllocateBalanceSvc.m_servAttr=g_itr2->m_iDestinationAttr; // 用户属性
		//strcpy(reqAllocateBalanceSvc.m_areaCode, "010");// 区号
		reqAllocateBalanceSvc.m_staffId=0; // 工号，写死，每省一个
		//strcpy(reqAllocateBalanceSvc.m_deductDate,pSvcIn->m_requestTime); // 请求时间
		
		long lRequestTime=0L;
		//date2long(lRequestTime);
		//reqAllocateBalanceSvc.m_iDeductDate=lRequestTime; // 请求时间为当前时间
		
		time_t tTime;
		
		tTime = time(NULL);
		reqAllocateBalanceSvc.m_iDeductDate = tTime+2209017599;
		
		//reqAllocateBalanceSvc.m_iDeductDate = pSvcIn->m_iRequestTime;
		
		strcpy(reqAllocateBalanceSvc.m_abmId,g_itr2->m_sSvcPlatformId);  // 业务平台标识
		
		__DEBUG_LOG1_(0, "请求流水：reqAllocateBalanceSvc.m_requestId:[%s]",reqAllocateBalanceSvc.m_requestId);
		__DEBUG_LOG1_(0, "用户号码：reqAllocateBalanceSvc.m_servNbr:[%s]",reqAllocateBalanceSvc.m_servNbr);
		__DEBUG_LOG1_(0, "号码类型：reqAllocateBalanceSvc.m_nbrType:[%d]",reqAllocateBalanceSvc.m_nbrType);
		__DEBUG_LOG1_(0, "用户属性：reqAllocateBalanceSvc.m_servAttr:[%d]",reqAllocateBalanceSvc.m_servAttr);
		//__DEBUG_LOG1_(0, "区号：reqAllocateBalanceSvc.m_areaCode:[%s]",reqAllocateBalanceSvc.m_areaCode);
		__DEBUG_LOG1_(0, "工号：reqAllocateBalanceSvc.m_staffId:[%d]",reqAllocateBalanceSvc.m_staffId);
		__DEBUG_LOG1_(0, "请求时间：reqAllocateBalanceSvc.m_iDeductDate:[%d]",reqAllocateBalanceSvc.m_iDeductDate);
		__DEBUG_LOG1_(0, "业务平台标识：reqAllocateBalanceSvc.m_abmId:[%s]",reqAllocateBalanceSvc.m_abmId);
		
		// 第二层CCR赋值
		// 取当前系统时间所属账期
		time_t timep;
		struct tm *p;
		time(&timep);
		p=gmtime(&timep);
		char sBillCycle[9]={0};
		snprintf(sBillCycle,sizeof(sBillCycle),"%d%d%s\0",(1900+p->tm_year),(1+p->tm_mon),"01");
		
		reqAllocateBalanceItem.m_billingCycle = atol(sBillCycle);
		strcpy(reqAllocateBalanceItem.m_acctItemTypeId,"1005");
		reqAllocateBalanceItem.m_balanceUnit = g_itr2->m_iUnitTypeId;
		reqAllocateBalanceItem.m_deductAmount = g_itr2->m_lDeductAmount; // 需划拨的金额
		reqAllocateBalanceItem.m_chargeType = 2;
		
		__DEBUG_LOG1_(0, "帐务周期：reqAllocateBalanceItem.m_abmId:[%d]",reqAllocateBalanceItem.m_billingCycle);
		__DEBUG_LOG1_(0, "帐目类型标识：reqAllocateBalanceItem.m_acctItemTypeId:[%s]",reqAllocateBalanceItem.m_acctItemTypeId);
		__DEBUG_LOG1_(0, "金额单位：reqAllocateBalanceItem.m_balanceUnit:[%d]",reqAllocateBalanceItem.m_balanceUnit);
		__DEBUG_LOG1_(0, "需划拨金额：reqAllocateBalanceItem.m_deductAmount:[%ld]",reqAllocateBalanceItem.m_deductAmount);
		__DEBUG_LOG1_(0, "扣费类型：reqAllocateBalanceItem.m_chargeType:[%d]",reqAllocateBalanceItem.m_chargeType);
		
		vResAcctItem.push_back(reqAllocateBalanceItem);
		
		// 设置CCR消息头
		// DeductFromABM. Micropayment@<DomainName>.ChinaTelecom.com
		// 生成Session-id
		long lSessionIdSeq;
		iRet = m_dccOperation->QueryPaySeq(lSessionIdSeq);
		if(iRet !=0)
		{
			__DEBUG_LOG0_(0, "查询DCC会话标识Session-Id失败");
			throw QRY_SESSION_ID_ERR;
		}
		struct struDccHead Head;
		char sSessionId[64];
		
		tTime = time(NULL);
		// 2011.8.1
		snprintf(Head.m_sSessionId,sizeof(Head.m_sSessionId),"%s%010d%s%04d\0","ABM@001.ChinaTelecom.com;",tTime,";",lSessionIdSeq);
		
		//snprintf(Head.m_sSessionId,sizeof(Head.m_sSessionId),"%s;%s;%06d\0","ABM@001.ChinaTelecom.com",reqAllocateBalanceSvc.m_deductDate,lSessionIdSeq);
		
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,划拨充值]:SESSION_ID:[%s]",Head.m_sSessionId);
	
		//strcpy(Head.m_sSrvTextId,"DeductFromABM.Micropayment@025.ChinaTelecom.com");
		strcpy(Head.m_sOutPlatform,""); // 外部平台，SGW使用
		strcpy(Head.m_sOrignHost,"ABM@001.ChinaTelecom.com"); // 2011.8.1
		strcpy(Head.m_sOrignRealm,"001.ChinaTelecom.com");
		
		// 获取请求划拨号码的归属地
		char sOrgId[4];
		char sLocalAreaId[4];
		iRet = m_dccOperation->qryAccountOrgId(reqAllocateBalanceSvc.m_servNbr,sOrgId,sLocalAreaId);
		if(iRet !=0)
		{
			__DEBUG_LOG0_(0, "查询用户号码所属机构失败");
			throw QRY_SERV_ORGID_ERR;
		}
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,主动划出]:号码所属区域:[%s]",sLocalAreaId);
		strcpy(reqAllocateBalanceSvc.m_areaCode, sLocalAreaId);// 区号
		__DEBUG_LOG1_(0, "区号：reqAllocateBalanceSvc.m_areaCode:[%s]",reqAllocateBalanceSvc.m_areaCode);
		snprintf(Head.m_sDestRealm,sizeof(Head.m_sDestRealm),"%s.ChinaTelecom.com",sOrgId);
		//strcpy(Head.m_sDestRealm,"025.ChinaTelecom.com");
		
		snprintf(Head.m_sSrvTextId,sizeof(Head.m_sSrvTextId),"DeductFromABM.Micropayment@%s.ChinaTelecom.com",sOrgId);
		__DEBUG_LOG1_(0, "消息头，目标设备标识-Head.m_sDestRealm:[%s]",Head.m_sDestRealm);
		
		Head.m_iAuthAppId = 4;
		Head.m_iReqType = 4;
		Head.m_iReqNumber  = 0;
		Head.m_iResultCode = 0;
		strcpy(Head.m_sSrvFlowId,""); // 按规则，取源m_sSrvFlowId后追加
		snprintf(Head.m_sSrvFlowId,sizeof(Head.m_sSrvFlowId),"%s%s",receiveDccHead.m_sSrvFlowId,"test---");
		// 自己组service-flow-id
		// 201107281124300000000100-00100-000000003001-1-010009-RechargeFromABM000.Micropayment.001
		char sSnDate[15]={0};
		long2date(reqAllocateBalanceSvc.m_iDeductDate,sSnDate);
		
		snprintf(Head.m_sSrvFlowId,sizeof(Head.m_sSrvFlowId),"%s08%d00-00100-000000003001-1-010010-RechargeFromABM000.Micropayment.001",sSnDate,lDccRecordSeq);
		
		memcpy((void *)(m_poPack->m_sDccBuf),(void *)&Head,sizeof(Head));
		
		m_poPack->m_iCmdID = ABMCMD_ALLOCATE_BALANCE;
	
		// 打包 - 第一层服务信息 CCR	
		if (!m_poPack->addAllocateBalanceSvc(reqAllocateBalanceSvc))
		{
			__DEBUG_LOG0_(0, "AcctiveAllocateBalance::deductBalanceIn 第一层CCR打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			return ACTIVE_PACK_CCR_ERR;
		}
		__DEBUG_LOG0_(0, "m_poPack->m_iCmdID=%d",m_poPack->m_iCmdID);
		
		__DEBUG_LOG0_(0, "第一层服务CCA打包完成");
		
		// 打包 - 第二层扣费明细
		for(int j=0;j<vResAcctItem.size();j++)
		{
			if (!m_poPack->addRequestDebitAcctItem(vResAcctItem[j]))
			{
				__DEBUG_LOG0_(0, "AllocateBalance:: _deal m_poPack 打包失败!");
				m_poPack->setRltCode(ECODE_PACK_FAIL);
				throw ACTIVE_PACK_CCR_ERR;
			}	
		}
		
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,划拨充值]:第二层付费明细CCR打包完成.");
	
		// 取DCC_INFO_RECORD_SEQ
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		
		// 保存发出的CCR信息
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,划拨充值]:保存发出的DCC包信息入TT");
		// modify.增加保存源DCC会话Session-id
		iRet = m_dccOperation->insertDccInfo(Head,dccType,lDccRecordSeq,receiveDccHead.m_sSessionId);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,划拨充值]:保存DCC业务信息失败");
			throw SAVE_PLATFOR_CCR_BIZ_ERR;
		}
		
		// 扩展一张表用来保存主动发出去的CCR和请求流水的关系，用于省里未返回CCA时的
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:保存发出的CCR,SESSION_ID与业务请求流水信息入TT");
		__DEBUG_LOG1_(0, "请求流水：reqAllocateBalanceSvc.m_requestId:[%s]",reqAllocateBalanceSvc.m_requestId);
		__DEBUG_LOG1_(0, "SESSION_ID:[%s]",Head.m_sSessionId);
		char sActionType[4]={0};
		strncpy(sActionType,"002",sizeof(sActionType));
		iRet = m_dccOperation->insertActiveCCR(Head.m_sSessionId,reqAllocateBalanceSvc.m_requestId,sActionType,sOrgId);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:保存CCR信息失败");
			throw SAVE_PLATFOR_CCR_BIZ_ERR;
		}
	}
	catch(TTStatus oSt)
	{
		iRet = ACTIVE_DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,划拨充值]: oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,划拨充值] iRet=%d",value);
	}
	
	return iRet;
}

// 余额划出
int ActiveAllocateBalance::deductBalanceOut(ReqPlatformSvc *pSvcIn,ReqPlatformPayInfo *g_itr2)
{
	int iRet=0;
	
	m_poActivePack = (ActiveAllocateBalanceCCR *)m_poPack;    // 打包请求CCR
	
	// 存CCR信息
	__DEBUG_LOG0_(0, "========================请求中心ABM余额划出CCR入TT=======================");
	__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]自服平台请求CCR明细:");
	__DEBUG_LOG1_(0, "请求流水：m_requestId:[%s]",pSvcIn->m_requestId);
	//__DEBUG_LOG1_(0, "请求时间：m_iRequestTime:[%d]",pSvcIn->m_iRequestTime);
	__DEBUG_LOG1_(0, "请求时间：m_requestTime:[%s]",pSvcIn->m_requestTime);
	__DEBUG_LOG1_(0, "用户号码：m_sDestinationAccount:[%s]",g_itr2->m_sDestinationAccount);
	__DEBUG_LOG1_(0, "标识类型：m_iDestinationType:[%d]",g_itr2->m_iDestinationType);
	__DEBUG_LOG1_(0, "用户属性：m_destinationAttr:[%d]",g_itr2->m_iDestinationAttr);
	__DEBUG_LOG1_(0, "业务平台标识：m_sSvcPlatformId:[%s]",g_itr2->m_sSvcPlatformId);
	__DEBUG_LOG1_(0, "支付密码：m_sPayPassword:[%s]",g_itr2->m_sPayPassword);
	__DEBUG_LOG1_(0, "余额划拨类型：m_iBalanceTransFlag:[%d]",g_itr2->m_iBalanceTransFlag);
	__DEBUG_LOG1_(0, "余额的单位类型：m_iUnitTypeId:[%d]",g_itr2->m_iUnitTypeId);
	__DEBUG_LOG1_(0, "需划拨金额：m_sDestinationAccount:[%ld]",g_itr2->m_lDeductAmount);
	
	/* Modify 2011.7.7 去除时间格式转换，与自服务平台交互直接使用字符串格式
	if(pSvcIn->m_iRequestTime > 0)
		long2date(pSvcIn->m_iRequestTime,pSvcIn->m_requestTime);
	else
		GetTime(pSvcIn->m_requestTime);
	*/
	
	struct struDccHead receiveDccHead;
	char dccType[1];	// DCC消息类型
	strcpy(dccType,"R"); 
	long lDccRecordSeq=0L;
	char sReceiveSessionId[100]={0};
	try
	{
		// 取DCC_INFO_RECORD_SEQ
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		
		// dcc包消息头信息
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:保存CCR包消息头信息，入TT");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:保存CCR包消息头信息失败");
			throw SAVE_PLATFOR_CCR_ERR;
		}
		
		// 获取接收DCC消息头的m_sSrvFlowId
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:接收CCR的m_sSrvFlowId:[%s]",receiveDccHead.m_sSrvFlowId);
		
		// dcc包业务数据信息
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:保存CCR包业务信息，入TT");
		iRet = m_dccOperation->savePlatformCCRBiz(receiveDccHead.m_sSessionId,pSvcIn,g_itr2);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:保存CCR业务信息失败");
			throw SAVE_PLATFOR_CCR_BIZ_ERR;
		}
		
		// 组CCR请求信息
		__DEBUG_LOG0_(0, "========================组CCR请求信息=======================");
		
		// 主动划拨CCR
		ReqBalanceSvc oReqBalanceSvc;
		
		// 数据准备
		strcpy(oReqBalanceSvc.m_requestId,pSvcIn->m_requestId); // 请求流水				
		strcpy(oReqBalanceSvc.m_destinationId,g_itr2->m_sDestinationAccount);//被充值用户号码
		oReqBalanceSvc.m_destinationIdType = 2;	// 默认，暂写死
		oReqBalanceSvc.m_destinationAttr = g_itr2->m_iDestinationAttr; // 用户属性
		
		long lRequestTime=0L;
		//strcpy(oReqBalanceSvc.m_requestTime,sRequestTime); // 请求时间-系统当前时间
		//date2long(lRequestTime);
		//oReqBalanceSvc.m_iRequestTime=lRequestTime;	
		
		time_t tTime;
		
		tTime = time(NULL);
		oReqBalanceSvc.m_iRequestTime = tTime+2209017599;
		
		
		//oReqBalanceSvc.m_iRequestTime=pSvcIn->m_iRequestTime;		
		oReqBalanceSvc.m_balanceUnit = g_itr2->m_iUnitTypeId;	// 余额单位类型
		oReqBalanceSvc.m_rechargeAmount	= g_itr2->m_lDeductAmount; // 充值金额	
		//strcpy(oReqBalanceSvc.m_areaCode,"001"); // 暂写死
		oReqBalanceSvc.m_blanceType = 0; // 余额类型,无法获得，暂写死
			
		__DEBUG_LOG1_(0, "请求流水：oReqBalanceSvc.m_requestId:[%s]",oReqBalanceSvc.m_requestId);
		__DEBUG_LOG1_(0, "被充值用户号码：oReqBalanceSvc.m_destinationId:[%s]",oReqBalanceSvc.m_destinationId);
		__DEBUG_LOG1_(0, "号码类型：oReqBalanceSvc.m_destinationIdType:[%d]",oReqBalanceSvc.m_destinationIdType);
		__DEBUG_LOG1_(0, "用户属性：oReqBalanceSvc.m_destinationAttr:[%d]",oReqBalanceSvc.m_destinationAttr);
		__DEBUG_LOG1_(0, "请求时间：oReqBalanceSvc.m_iRequestTime:[%d]",oReqBalanceSvc.m_iRequestTime);
		__DEBUG_LOG1_(0, "余额单位类型：oReqBalanceSvc.m_balanceUnit:[%d]",oReqBalanceSvc.m_balanceUnit);
		__DEBUG_LOG1_(0, "充值金额：oReqBalanceSvc.m_rechargeAmount:[%ld]",oReqBalanceSvc.m_rechargeAmount);
		//__DEBUG_LOG1_(0, "业务平台标识：oReqBalanceSvc.m_areaCode:[%s]",oReqBalanceSvc.m_areaCode);	
		__DEBUG_LOG1_(0, "余额类型：oReqBalanceSvc.m_blanceType:[%d]",oReqBalanceSvc.m_blanceType);	
				
		// 设置消息头
		// 生成Session-id
		long lSessionIdSeq;
		iRet = m_dccOperation->QueryPaySeq(lSessionIdSeq);
		if(iRet !=0)
		{
			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:查询DCC会话标识Session-Id失败");
			throw QRY_SESSION_ID_ERR;
		}
		
		struct struDccHead Head;
		char sSessionId[64];

		tTime = time(NULL);
		// 2011.8.1
		snprintf(Head.m_sSessionId,sizeof(Head.m_sSessionId),"%s%010d%s%04d\0","ABM@001.ChinaTelecom.com;",tTime,";",lSessionIdSeq);
		
		__DEBUG_LOG1_(0, "SESSION_ID:[%s]",Head.m_sSessionId);
		
		//strcpy(Head.m_sSrvTextId,"RechargeFromABM000.Micropayment@025.ChinaTelecom.com");
		
		strcpy(Head.m_sOutPlatform,""); // 外部平台,SGW使用
		strcpy(Head.m_sOrignHost,"ABM@001.ChinaTelecom.com"); // 2011.8.1
		strcpy(Head.m_sOrignRealm,"001.ChinaTelecom.com");
		
		// 获取请求划拨号码的归属地
		char sOrgId[5]={'\0'};
		char sLocalAreaId[5]={0};
		iRet = m_dccOperation->qryAccountOrgId(oReqBalanceSvc.m_destinationId,sOrgId,sLocalAreaId);
		
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,主动划出]:号码所属机构:[%s]",sOrgId);
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,主动划出]:号码所属区域:[%s]",sLocalAreaId);
		
		if(iRet !=0)
		{
			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:查询用户号码所属机构失败");
			throw QRY_SERV_ORGID_ERR;
		}
		
		strcpy(oReqBalanceSvc.m_areaCode,sLocalAreaId);
		__DEBUG_LOG1_(0, "区号：oReqBalanceSvc.m_areaCode:[%s]",oReqBalanceSvc.m_areaCode);
		
		snprintf(Head.m_sDestRealm,sizeof(Head.m_sDestRealm),"%s.ChinaTelecom.com",sOrgId);
		//strcpy(Head.m_sDestRealm,"025.ChinaTelecom.com");
		snprintf(Head.m_sSrvTextId,sizeof(Head.m_sSrvTextId),"RechargeFromABM000.Micropayment@%s.ChinaTelecom.com",sOrgId);
		
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,主动划出]:消息头，目标设备标识-Head.m_sDestRealm:[%s]",Head.m_sDestRealm);
			
		Head.m_iAuthAppId = 4;
		Head.m_iReqType = 4;
		Head.m_iReqNumber  = 0;
		Head.m_iResultCode = 0;
		strcpy(Head.m_sSrvFlowId,""); // 按规则，取源m_sSrvFlowId后追加
		// 自己组service-flow-id
		// 201107281124300000000100-00100-000000003001-1-010009-RechargeFromABM000.Micropayment.001
		char sSnDate[15]={0};
		long2date(oReqBalanceSvc.m_iRequestTime,sSnDate);
		
		snprintf(Head.m_sSrvFlowId,sizeof(Head.m_sSrvFlowId),"%s%s",receiveDccHead.m_sSrvFlowId,"test---");
		
		snprintf(Head.m_sSrvFlowId,sizeof(Head.m_sSrvFlowId),"%s08%d00-00100-000000003001-1-010009-RechargeFromABM000.Micropayment.001",sSnDate,lDccRecordSeq);
		
		// 发送CCR消息头赋值
		memcpy((void *)(m_poPack->m_sDccBuf),(void *)&Head,sizeof(Head));
		
		m_poPack->m_iCmdID = ABMCMD_ACTIVE_ALLOCATE_BALANCE;
		
		
		// 打包
		// 打包 - 第一层服务信息 CCR	
		if (!m_poActivePack->addReqBalanceSvc(oReqBalanceSvc))
		{
			__DEBUG_LOG0_(0, "AcctiveAllocateBalance::deductBalanceOut 第一层CCR打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw ACTIVE_PACK_CCR_OUT_ERR;
		}
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:第一层服务CCA打包完成");
		
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		// 保存发出的CCR信息
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:保存发出的DCC包信息入TT");
		iRet = m_dccOperation->insertDccInfo(Head,dccType,lDccRecordSeq,receiveDccHead.m_sSessionId);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:保存DCC业务信息失败");
			throw SAVE_PLATFOR_CCR_BIZ_ERR;
		}
		
		// 扩展一张表用来保存主动发出去的CCR和请求流水的关系，用于省里未返回CCA时的
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:保存发出的CCR,SESSION_ID与业务请求流水信息入TT");
		__DEBUG_LOG1_(0, "请求流水：oReqBalanceSvc.m_requestId:[%s]",oReqBalanceSvc.m_requestId);
		__DEBUG_LOG1_(0, "SESSION_ID:[%s]",Head.m_sSessionId);
		__DEBUG_LOG1_(0, "ORG_ID:[%s]",sOrgId);
		
		char sActionType[4]={0};
		strncpy(sActionType,"001",sizeof(sActionType));
		iRet = m_dccOperation->insertActiveCCR(Head.m_sSessionId,oReqBalanceSvc.m_requestId,sActionType,sOrgId);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:保存CCR信息失败");
			throw SAVE_PLATFOR_CCR_BIZ_ERR;
		}
	}
	catch(TTStatus oSt)
	{
		iRet = ACTIVE_DEDUCT_TT_ERR;
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,主动划出]: oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "[主动划拨-收发CCR,主动划出]: iRet=%d",value);
	}
	__DEBUG_LOG1_(0, "[主动划拨-收发CCR,主动划出]-ActiveAllocateBalance::deductBalanceOut,iRet=:[%d]",iRet);
	return iRet;
}

