#include "PasswdIdentifyBiz.h"
#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"
#include "PublicConndb.h"

static vector<PasswdIdentifyData *> g_vLoginPasswdData;    // HSS端返回CCA 
static vector<PasswdIdentifyData *>::iterator g_itr1;

static char cPreSessionId[100]={0};

extern bool g_toPlatformFlag; // 控制消息发往自服务平台队列

PasswdIdentifyBiz::PasswdIdentifyBiz()
{
		m_poUnpack = NULL;
		m_poPack = NULL;		
		m_dccOperation = new DccOperation;	
}

PasswdIdentifyBiz::~PasswdIdentifyBiz()
{
	if (m_dccOperation != NULL)
		delete m_dccOperation;
}

int PasswdIdentifyBiz::deal(ABMCCR *pRecivePack, ABMCCA *pSendPack)
{
	__DEBUG_LOG0_(0, "=========>PasswdIdentifyBiz::deal 响应服务开始");
	__DEBUG_LOG0_(0, "=========>1：接收全国HSS返回的CCA...");
	
	LoginPasswdStruct reqStruct;
	LoginPasswdCond reqCond;
	
	//LoginPasswdData oCCAData;	//中心ABM响应自服务平台的CCA返回信息
	
	int iRet=0;
	int iSize=0;
	int LoginServType; //用户类型：互联网用户 OR 通信证用户
	
	// 获取pRecivePack的公共CCA消息头
	char sSessionId[64];
	struDccHead Head;

	char dccType[1];	// DCC消息类型
	strcpy(dccType,"A"); 
	long lDccRecordSeq=0L; // 流水号，主键
	
	try
	{
		// 取消息包的消息头信息
		memcpy((void *)&Head,(void *)(pRecivePack->m_sDccBuf),sizeof(Head));
		// 收到CCA包Session-Id
		__DEBUG_LOG1_(0, "=======>收到全国HSS返回的CCA的消息头，SESSION_ID:[%s]",Head.m_sSessionId);
		//根据CCA响应流水去找自服务平台请求CCR的会话ID
		iRet = m_dccOperation->qryPreSessionId(Head.m_sSessionId,sSessionId);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "查询CCA会话ID的源Session Id失败.");
		}
		__DEBUG_LOG1_(0, "========>自服务平台发起CCR的SESSION_ID:[%s]",sSessionId);
		strcpy(cPreSessionId,sSessionId);
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "查询DCC_INFO_RECORD_SEQ失败");
			throw -1;
		}
		// 将收到的CCA消息头入TT
		iRet = m_dccOperation->insertDccInfo(Head,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存全国HSS返回CCA包消息头信息失败");
			throw -1;
		}
		// 根据sessionId取TT对应的CCR信息
		__DEBUG_LOG0_(0, "=========>根据sessionId取TT对应的CCR[自服务平台CCR请求信息]信息:");
	
		iRet = m_dccOperation->getLoginIdentifyCCRInfo(sSessionId,reqStruct,reqCond);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "获取自服务平台CCR请求业务信息失败");
			throw -1;
		}
		__DEBUG_LOG1_(0, "getCCRInfo,iRet=[%d]",iRet);
		
		//用户类型：互联网用户OR通行证用户
		LoginServType=reqCond.m_hDestType;
		// 用户号码
		char sAccNbr[32]={0};
		strcpy(sAccNbr,reqCond.m_sDestAccount);
		
		//解包
		m_poPack = (LoginPasswdCCA *)pSendPack;
		m_poUnpack = (PasswdIdentifyCCA *)pRecivePack;
		
		if(iSize=m_poUnpack->getPasswdIdentify(g_vLoginPasswdData) == 0)
			{
				__DEBUG_LOG0_(0, "PasswdIdentifyBiz::deal 全国HSS返回CCA，请求服务消息包为NULL");
				m_poPack->setRltCode(ECODE_NOMSG);
				throw  -1;
			}
			else if(iSize < 0)
				{
					__DEBUG_LOG0_(0, "PasswdIdentifyBiz::deal 消息解包出错!");
					m_poPack->setRltCode(ECODE_UNPACK_FAIL);
					throw -1;
				}
			
		//CCA解包处理
		for(g_itr1=g_vLoginPasswdData.begin();g_itr1!=g_vLoginPasswdData.end();++g_itr1)
		{
			iRet=_deal(reqStruct,*g_itr1,sAccNbr);
			__DEBUG_LOG1_(0, "PasswdIdentifyBiz::deal iRet=%d", iRet);
		}
	}
	catch(TTStatus oSt)
	{
		iRet = -1;
		__DEBUG_LOG1_(0, "PasswdIdentifyBiz::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "PasswdIdentifyBiz::QryPayoutInfo value=%d",value);
	}
	
	if(iRet != 0)
	{
		// 打返回包，赋错误码
		m_poPack->clear();
		m_poPack->setRltCode(ECODE_TT_FAIL);
	}
	
	return iRet;	
}

int PasswdIdentifyBiz::_deal(LoginPasswdStruct oRes,PasswdIdentifyData *pData,char *sAccNbr)
{
	int iRet=0;
	LoginPasswdData oCCAData;	//中心ABM响应自服务平台的CCA返回信息
	char sSessionId[64];
	struDccHead Head;

	char dccType[1];	// DCC消息类型
	strcpy(dccType,"A"); 
	long lDccRecordSeq=0L; // 流水号，主键
	
	//CCA解包处理
	g_toPlatformFlag = true; // 发往自服务平台的消息
	try{
		//生成返回自服务平台的CCA包
		strcpy(oCCAData.m_sResID,oRes.m_sReqID);
		__DEBUG_LOG1_(0, "返回CCA的响应流水:[%s]",oCCAData.m_sResID);
		oCCAData.m_hRltCode = pData->m_hRltCode;
		__DEBUG_LOG1_(0, "业务级处理结果吗:[%d]",oCCAData.m_hRltCode);
		strcpy(oCCAData.m_sParaRes,pData->m_sParaRes);
		
		time_t tTime;
		tTime = time(NULL);
		oCCAData.m_iResTime=tTime;
					
		//设置消息头
		// Session-id - 返回自服务平台的Session需要根据请求流水号从DCC业务信息记录表中获取
		strcpy(Head.m_sSessionId,cPreSessionId);
		__DEBUG_LOG1_(0, "返回CCA的SESSION_ID:[%s]",Head.m_sSessionId);
		
		strcpy(Head.m_sSrvTextId,"Login.Micropayment@001.ChinaTelecom.com");
		strcpy(Head.m_sOutPlatform,"001.ChinaTelecom.com");
		strcpy(Head.m_sOrignHost,"ABM@001.ChinaTelecom.com"); // 2011.8.1
		strcpy(Head.m_sOrignRealm,"001.ChinaTelecom.com");
		
		// 获取请求划拨号码的归属地
		char sOrgId[4]={0};
		char sLocalAreaId[4]={0};
		char sAccoutNbr[32]={0};
		strcpy(sAccoutNbr,sAccNbr); // 用户号码
		iRet = m_dccOperation->qryAccountOrgId(sAccoutNbr,sOrgId,sLocalAreaId); 
		if(iRet !=0 )
		{
			__DEBUG_LOG0_(0, "取号码所属机构代码失败.");
			throw -1;
		}
		
		snprintf(Head.m_sDestRealm,sizeof(Head.m_sDestRealm),"%s.ChinaTelecom.com",sOrgId);
		__DEBUG_LOG1_(0, "消息头，目标设备标识-Head.m_sDestRealm:[%s]",Head.m_sDestRealm);
		
		Head.m_iAuthAppId = 4;
		Head.m_iReqType = 4;
		Head.m_iReqNumber  = 0;
		Head.m_iResultCode = 0;
		
		memcpy((void *)(m_poPack->m_sDccBuf),(void *)&Head,sizeof(Head));
		
		//返回自服务平台CCA打包
		if(!m_poPack->addLoginPasswdData(oCCAData))
		{
			__DEBUG_LOG0_(0, "PasswdIdentifyBiz::deal 返回自服务平台CCA打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw -1;
		}
		__DEBUG_LOG0_(0, "返回自服务平台CCA打包完成");
		
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		if(iRet !=0 )
		{
			__DEBUG_LOG0_(0, "取DCC_INFO_RECORD_SEQ失败.");
			throw -1;
		}
		m_poPack->m_iCmdID = ABMCMD_LOGIN_PASSWD;
		__DEBUG_LOG1_(0, "打包发出CCA的iCmdID:[%d]",m_poPack->m_iCmdID);
		
		
		
		// 保存发出的CCA信息
		__DEBUG_LOG0_(0, "保存发出的DCC包信息入TT");
		iRet = m_dccOperation->insertDccInfo(Head,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "保存DCC业务信息失败");
			throw -1;
		}
	}
	catch(TTStatus oSt)
	{
		iRet = -1;
		__DEBUG_LOG1_(0, "[登陆密码鉴权-收发CCA,发往自服务端]: oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "[登陆密码鉴权-收发CCA,发往自服务端]: iRet=%d",value);
	}
	return iRet;
}