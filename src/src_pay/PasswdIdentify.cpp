#include "PasswdIdentify.h"
#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"
#include "PublicConndb.h"

static vector<LoginPasswdStruct *> g_vLoginPasswdStruct;
static vector<LoginPasswdStruct *>::iterator g_itr1; 
	
static vector<LoginPasswdCond *> g_vLoginPasswdCond;
static vector<LoginPasswdCond *>::iterator g_itr2;
	
PasswdIdentify::PasswdIdentify()
{
    m_poPasswdPack = NULL;
    m_poUnpack = NULL;
    m_poPack = NULL;
    m_dccOperation = new DccOperation;
}

PasswdIdentify:: ~PasswdIdentify()
{
    if (m_dccOperation != NULL)
		delete m_dccOperation;
}

int PasswdIdentify::deal(ABMCCR *pCCR, ABMCCA *pSendPack)
{
	__DEBUG_LOG0_(0, "=========>PasswdIdentify::deal 请求服务开始");
	__DEBUG_LOG0_(0, "=========>1：接收来自服务的请求CCR...");
	int iRet=0;
	int iSize=0;
	
	m_poUnpack = (LoginPasswdCCR *)pCCR; // 解包请求CCR
	m_poPack= (LoginPasswdCCA *)pSendPack;
	m_poPasswdPack = (PasswdIdentifyCCR *)pSendPack;    // 打包请求CCR
	
	m_poPack->clear();
	m_poPasswdPack->clear();
	g_vLoginPasswdStruct.clear();
	g_vLoginPasswdCond.clear();
	
	//消息解包 - 第一层
	if ((iSize=m_poUnpack->getLoginPasswdRespons(g_vLoginPasswdStruct)) == 0 )
	{
		__DEBUG_LOG0_(0, "PasswdIdentify::deal 自服务平台发起登陆密码验证，请求服务消息包为NULL");
		m_poPack->setRltCode(ECODE_NOMSG);
		return -1;
	}
	else if (iSize < 0)
	{
		__DEBUG_LOG0_(0, "PasswdIdentify::deal 消息解包出错!");
		m_poPack->setRltCode(ECODE_UNPACK_FAIL);
		return -1;
	}
	
	try
	{		
		// 处理第一层信息
		for (g_itr1=g_vLoginPasswdStruct.begin(); g_itr1!=g_vLoginPasswdStruct.end(); ++g_itr1)
		{
			// 解detail包 - 第二层
			if ((iSize=m_poUnpack->getLoginPasswd(*g_itr1,g_vLoginPasswdCond)) == 0 )
			{
				__DEBUG_LOG0_(0, "PasswdIdentify::deal 请求明细消息包为NULL");
				m_poPack->setRltCode(ECODE_NOMSG);
				return -1;
			}
			else if (iSize < 0)
			{
				__DEBUG_LOG0_(0, "PasswdIdentify::deal 消息解包出错!");
				m_poPack->setRltCode(ECODE_UNPACK_FAIL);
				return -1;
			}
			 
			__DEBUG_LOG0_(0, "=========>2：_deal.处理自服务平台所请求的CCR信息.");			
			// 处理
			if (_deal(*g_itr1,g_vLoginPasswdCond) != 0)
				break;
		}
		__DEBUG_LOG1_(0, "xxxxxx=PasswdIdentify::deal iRet=%d",iRet);
	}
	catch(TTStatus oSt)
	{
		iRet = -1;
		__DEBUG_LOG1_(0, "PasswdIdentify::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "PasswdIdentify::QryPayoutInfo value=%d",value);
	}
	
	// 业务处理失败，打返回包，TODO：
	if(iRet!=0)
	{
		m_poPack->clear();
		m_poPack->setRltCode(ECODE_TT_FAIL);
	}
	__DEBUG_LOG1_(0, "PasswdIdentify::deal iRet=%d",iRet);
	return iRet;
}

int PasswdIdentify::_deal(LoginPasswdStruct *oRes,vector<LoginPasswdCond* > g_vLoginPasswdCond)
{
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "=======================登陆密码鉴权=======================");
	__DEBUG_LOG0_(0, "=========>取自服务平台请求CCR第一层服务信息:");
	__DEBUG_LOG1_(0, "[登陆密码鉴权-收发CCR,PasswdIdentify],请求流水：m_sReqID:%s",oRes->m_sReqID);
	__DEBUG_LOG1_(0, "[登陆密码鉴权-收发CCR,PasswdIdentify],请求时间：m_iReqTime:%d",oRes->m_iReqTime);
	
	if(oRes->m_iReqTime > 0)
		long2date(oRes->m_iReqTime,oRes->m_requestTime);
	else
		GetTime(oRes->m_requestTime);
	
	int iRet=0;
	int LoginServType; //用户类型：互联网用户 OR 通信证用户
	
	try
	{
		for (g_itr2=g_vLoginPasswdCond.begin(); g_itr2!=g_vLoginPasswdCond.end(); ++g_itr2)
		{
			__DEBUG_LOG0_(0, "[登陆密码鉴权-收发CCR,PasswdIdentify]:取自服务平台请求CCR第二层支付信息：");
			 LoginServType = (*g_itr2)->m_hDestType;
			//__DEBUG_LOG1_(0, "[登陆密码鉴权-收CCR],用户号码,m_sDestAccount:[%s]",(*g_itr2)->m_sDestAccount);
			__DEBUG_LOG1_(0, "[登陆密码鉴权-收CCR],标识类型,m_hDestType:[%d](1-通行证帐号 2-业务平台帐号)",LoginServType);
			//__DEBUG_LOG1_(0, "[登陆密码鉴权-收CCR],用户属性,m_hDestAttr:[%d]",(*g_itr2)->m_hDestAttr);
			//__DEBUG_LOG1_(0, "[登陆密码鉴权-收CCR],业务平台标识,m_sServPlatID:[%s]",(*g_itr2)->m_sServPlatID);
			//__DEBUG_LOG1_(0, "[登陆密码鉴权-收CCR],登录密码,m_sLoginPasswd:[%s]",(*g_itr2)->m_sLoginPasswd);
			
			switch(LoginServType)
			{
				case 2:
					//互联网用户
					//调用3.4.1.3HSS密码鉴权接口
					__DEBUG_LOG0_(0, "[登陆密码鉴权-收发CCR,PasswdIdentify]:互联网用户调用HSS密码鉴权接口");
					
					iRet = loginHSSIdentify(oRes,*g_itr2);
			 		if(iRet!=0)
			 		{
			 			throw -1;
			 			__DEBUG_LOG0_(0, "[登陆密码鉴权-收发CCR,PasswdIdentify]:登陆密码鉴权到全国中心HSS鉴权失败");
			 		}
			 		break;
				case 1:
					//通行证用户
					//调用3.4.3.1通行证用户鉴权接口-全国中心UDB认证,暂无法实现
					__DEBUG_LOG0_(0, "[登陆密码鉴权-收发CCR,PasswdIdentify]:通行证用户调用通行证用户鉴权接口");
					//通信证用户密码鉴权函数
					
					iRet = loginHSSIdentify(oRes,*g_itr2);
			 		if(iRet!=0)
			 		{
			 			throw -1;
			 			__DEBUG_LOG0_(0, "[登陆密码鉴权-收发CCR,PasswdIdentify]:登陆密码鉴权到全国中心HSS鉴权失败");
			 		}
					
					break;
			}
		}
	}
	catch(TTStatus oSt) 
	{
		iRet=-1;
		__DEBUG_LOG1_(0, "[登陆密码鉴权-收发CCR,PasswdIdentify] oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "[登陆密码鉴权-收发CCR,PasswdIdentify] iRet=%d",value);
	}
	
	return iRet;	
}

//互联网用户发往全国HSS端密码鉴权
int PasswdIdentify::loginHSSIdentify(LoginPasswdStruct *oRes,LoginPasswdCond * g_itr2)
{
	int iRet=0;
	
	//保存CCR消息
	__DEBUG_LOG0_(0, "========================请求中心ABM登陆密码鉴权CCR入TT=======================");
	__DEBUG_LOG0_(0, "[登陆密码鉴权-收发CCR,自服务平台]自服平台请求CCR明细:");
	__DEBUG_LOG1_(0, "请求流水：m_sReqID:[%s]",oRes->m_sReqID);
	__DEBUG_LOG1_(0, "请求时间：m_iReqTime:[%d]",oRes->m_iReqTime);
	__DEBUG_LOG1_(0, "用户号码：m_sDestAccount:[%s]",g_itr2->m_sDestAccount);
	__DEBUG_LOG1_(0, "标识类型：m_hDestType:[%d]",g_itr2->m_hDestType);
	__DEBUG_LOG1_(0, "用户属性：m_hDestAttr:[%d]",g_itr2->m_hDestAttr);
	__DEBUG_LOG1_(0, "业务平台标识：m_sServPlatID:[%s]",g_itr2->m_sServPlatID);
	__DEBUG_LOG1_(0, "登陆密码：m_sLoginPasswd:[%s]",g_itr2->m_sLoginPasswd);
	
	struct struDccHead receiveDccHead;
	char dccType[1];	// DCC消息类型
	strcpy(dccType,"R"); 
	long lDccRecordSeq=0L;
	
	try
	{
		// 取DCC_INFO_RECORD_SEQ
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		
		// dcc包消息头信息
		__DEBUG_LOG0_(0, "[登陆密码鉴权-收发CCR,自服务平台]:保存CCR包消息头信息，入TT");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[登陆密码鉴权-收发CCR,自服务平台]:保存CCR包消息头信息失败");
			throw -1;
		}
		
		// dcc包业务数据信息
		__DEBUG_LOG0_(0, "[登陆密码鉴权-收发CCR,自服务平台]:保存CCR包业务信息，入TT");
		iRet = m_dccOperation->savePlatformCCRforIdentify(receiveDccHead.m_sSessionId,oRes,g_itr2);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[主动划拨-收发CCR,主动划出]:保存CCR业务信息失败");
			throw -1;
		}
		
		// 组CCR请求信息
		__DEBUG_LOG0_(0, "========================组登陆密码鉴权CCR请求信息=======================");
		
		// HSS端密码鉴权CCR
		PasswdIdentifyCond oReqPasswdIdentify;
		
		//数据准备
		strcpy(oReqPasswdIdentify.m_sReqID,oRes->m_sReqID);	//请求流水
		oReqPasswdIdentify.m_hDsnType = g_itr2->m_hDestType;		//用户类型
		strcpy(oReqPasswdIdentify.m_sDsnNbr,g_itr2->m_sDestAccount);	//用户号码
		oReqPasswdIdentify.m_hDsnAttr = g_itr2->m_hDestAttr;	//用户属性
		strcpy(oReqPasswdIdentify.m_sServPlatID,g_itr2->m_sServPlatID);	//业务平台标识
		strcpy(oReqPasswdIdentify.m_sLoginPasswd,g_itr2->m_sLoginPasswd);	//登陆密码
		
		__DEBUG_LOG1_(0, "请求流水：oReqPasswdIdentify.m_sReqID:[%s]",oReqPasswdIdentify.m_sReqID);
		__DEBUG_LOG1_(0, "用户类型：oReqPasswdIdentify.m_hDsnType:[%d]",oReqPasswdIdentify.m_hDsnType);
		__DEBUG_LOG1_(0, "用户号码：oReqPasswdIdentify.m_sDsnNbr:[%s]",oReqPasswdIdentify.m_sDsnNbr);
		__DEBUG_LOG1_(0, "用户属性：oReqPasswdIdentify.m_hDsnAttr:[%d]",oReqPasswdIdentify.m_hDsnAttr);
		__DEBUG_LOG1_(0, "业务平台标识：oReqPasswdIdentify.m_sServPlatID:[%s]",oReqPasswdIdentify.m_sServPlatID);
		__DEBUG_LOG1_(0, "登陆密码：oReqPasswdIdentify.m_sLoginPasswd:[%s]",oReqPasswdIdentify.m_sLoginPasswd);
		
		//设置消息头
		// 生成Session-id
		long lSessionIdSeq;
		iRet = m_dccOperation->QueryPaySeq(lSessionIdSeq);
		if(iRet !=0)
		{
			__DEBUG_LOG0_(0, "[登陆密码鉴权-收发CCR,发往HSS端]:查询DCC会话标识Session-Id失败");
			throw -1;
		}
		
		struct struDccHead Head;
		char sSessionId[64];
		time_t tTime;
		tTime = time(NULL);
		// 2011.8.1
		snprintf(Head.m_sSessionId,sizeof(Head.m_sSessionId),"%s%010d%s%04d\0","ABM@001.ChinaTelecom.com;",tTime,";",lSessionIdSeq);
		__DEBUG_LOG1_(0, "SESSION_ID:[%s]",Head.m_sSessionId);
			
		strcpy(Head.m_sOutPlatform,""); // 外部平台,SGW使用
		//strcpy(Head.m_sOutPlatform,"001.ChinaTelecom.com");
		strcpy(Head.m_sOrignHost,"ABM@001.ChinaTelecom.com"); // 2011.8.1
		strcpy(Head.m_sOrignRealm,"001.ChinaTelecom.com");

		char sOrgId[5]={0};
		char sLocalAreaId[5]={0};
		iRet = m_dccOperation->qryAccountOrgId(oReqPasswdIdentify.m_sDsnNbr,sOrgId,sLocalAreaId);
		
		__DEBUG_LOG1_(0, "[登陆密码鉴权-收发CCR,发往HSS端]:号码所属机构:[%s]",sOrgId);
		__DEBUG_LOG1_(0, "[登陆密码鉴权-收发CCR,发往HSS端]:号码所属区域:[%s]",sLocalAreaId);

		if(iRet !=0 )
		{
			__DEBUG_LOG0_(0, "error");
			throw -1;
		}
		
		snprintf(Head.m_sDestRealm,sizeof(Head.m_sDestRealm),"%s.ChinaTelecom.com","001");
		__DEBUG_LOG1_(0, "Head.m_sDestRealm:[%s]",Head.m_sDestRealm);
		
		// 取归属地OrgId
		snprintf(Head.m_sSrvTextId,sizeof(Head.m_sSrvTextId),"PaymentPasswdAuthen.Micropayment@001.ChinaTelecom.com");
		//strcpy(Head.m_sSrvTextId,"PaymentPasswdAuthen.Micropayment@%s.ChinaTelecom.com",sOrgId);
		
		Head.m_iAuthAppId = 4;
		Head.m_iReqType = 4;
		Head.m_iReqNumber  = 0;
		Head.m_iResultCode = 0;
		strcpy(Head.m_sSrvFlowId,""); // 按规则，取源m_sSrvFlowId后追加
		snprintf(Head.m_sSrvFlowId,sizeof(Head.m_sSrvFlowId),"%s%s",receiveDccHead.m_sSrvFlowId,"test---");
		
		// 发送CCR消息头赋值
		memcpy((void *)(m_poPasswdPack->m_sDccBuf),(void *)&Head,sizeof(Head));
		
		m_poPasswdPack->m_iCmdID = ABMCMD_PASSWD_IDENTIFY;
		
		//打包
		if(!m_poPasswdPack->addPasswdIdentify(oReqPasswdIdentify))
			{
			__DEBUG_LOG0_(0, "PasswdIdentify::loginHSSIdentify CCR打包失败!");
			m_poPasswdPack->setRltCode(ECODE_PACK_FAIL);
			throw -1;
		}
		__DEBUG_LOG0_(0, "[登陆密码鉴权-收发CCR,发往HSS端]:CCA打包完成");
		
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		// 保存发出的CCR信息
		__DEBUG_LOG0_(0, "[登陆密码鉴权-收发CCR,发往HSS端]:保存发出的DCC包信息入TT");
		iRet = m_dccOperation->insertDccInfo(Head,dccType,lDccRecordSeq,receiveDccHead.m_sSessionId);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[登陆密码鉴权-收发CCR,发往HSS端]:保存DCC业务信息失败");
			throw -1;
		}
	}
	catch(TTStatus oSt)
	{
		iRet = -1;
		__DEBUG_LOG1_(0, "[登陆密码鉴权-收发CCR,发往HSS端]: oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "[登陆密码鉴权-收发CCR,发往HSS端]: iRet=%d",value);
	}
	__DEBUG_LOG1_(0, "[登陆密码鉴权-收发CCR,发往HSS端]-ActiveAllocateBalance::deductBalanceOut,iRet=:[%d]",iRet);
	
	return iRet;
}
