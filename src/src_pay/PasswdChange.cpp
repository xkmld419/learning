#include "PasswdChange.h"
#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"
#include "PublicConndb.h"

static vector<PasswdChangeStruct *> g_vPasswdChangeStruct;
static vector<PasswdChangeStruct *>::iterator g_itr1; 
	
static vector<PasswdChangeCond *> g_vPasswdChangeCond;
static vector<PasswdChangeCond *>::iterator g_itr2;

PasswdChange::PasswdChange()
{
    m_poPasswdPack = NULL;
    m_poPack = NULL;
    m_dccOperation = new DccOperation;
}

PasswdChange:: ~PasswdChange()
{
    if (m_dccOperation != NULL)
		delete m_dccOperation;
}

int PasswdChange::deal(ABMCCR *pCCR, ABMCCA *pSendPack)
{
	__DEBUG_LOG0_(0, "=========>PasswdChange::deal 请求服务开始");
	__DEBUG_LOG0_(0, "=========>1：接收来自服务的请求CCR...");
	int iRet=0;
	int iSize=0;
	
	m_poUnpack = (PasswdChangeCCR *)pCCR; // 解包请求CCR
	m_poPack= (PasswdChangeCCA *)pSendPack;
	m_poPasswdPack = (PasswdChangeCCR *)pSendPack;    // 打包请求CCR 
	
	m_poPack->clear();
	m_poPasswdPack->clear();
	g_vPasswdChangeStruct.clear();
	g_vPasswdChangeCond.clear();
	
	//消息解包 - 第一层
	if ((iSize=m_poUnpack->getPasswdChangeRespons(g_vPasswdChangeStruct)) == 0 )
	{
		__DEBUG_LOG0_(0, "PasswdChange::deal 自服务平台发起登陆密码验证，请求服务消息包为NULL");
		m_poPack->setRltCode(ECODE_NOMSG);
		return -1;
	}
	else if (iSize < 0)
	{
		__DEBUG_LOG0_(0, "PasswdChange::deal 消息解包出错!");
		m_poPack->setRltCode(ECODE_UNPACK_FAIL);
		return -1;
	}
	
	try
	{		
		// 处理第一层信息
		for (g_itr1=g_vPasswdChangeStruct.begin(); g_itr1!=g_vPasswdChangeStruct.end(); ++g_itr1)
		{
			// 解detail包 - 第二层
			if ((iSize=m_poUnpack->getPasswdChange(*g_itr1,g_vPasswdChangeCond)) == 0 )
			{
				__DEBUG_LOG0_(0, "PasswdChange::deal 请求明细消息包为NULL");
				m_poPack->setRltCode(ECODE_NOMSG);
				return -1;
			}
			else if (iSize < 0)
			{
				__DEBUG_LOG0_(0, "PasswdChange::deal 消息解包出错!");
				m_poPack->setRltCode(ECODE_UNPACK_FAIL);
				return -1;
			}
			 
			__DEBUG_LOG0_(0, "=========>2：_deal.处理自服务平台所请求的CCR信息.");			
			// 处理
			if (_deal(*g_itr1,g_vPasswdChangeCond) != 0)
				break;
		}
		__DEBUG_LOG1_(0, "xxxxxx=PasswdChange::deal iRet=%d",iRet);
	}
	catch(TTStatus oSt)
	{
		iRet = -1;
		__DEBUG_LOG1_(0, "PasswdChange::deal oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "PasswdChange::QryPayoutInfo value=%d",value);
	}
	
	// 业务处理失败，打返回包，TODO：
	if(iRet!=0)
	{
		m_poPack->clear();
		m_poPack->setRltCode(ECODE_TT_FAIL);
	}
	__DEBUG_LOG1_(0, "PasswdChange::deal iRet=%d",iRet);
	return iRet;
}

int PasswdChange::_deal(PasswdChangeStruct *oRes,vector<PasswdChangeCond *> g_vPasswdChangeCond)
{
	// 测试--取第一层信息
	__DEBUG_LOG0_(0, "=======================登陆密码修改=======================");
	__DEBUG_LOG0_(0, "=========>取自服务平台请求CCR第一层服务信息:");
	__DEBUG_LOG1_(0, "[登陆密码修改-收发CCR,PasswdChange],请求流水：m_sReqID:%s",oRes->m_sReqID);
	__DEBUG_LOG1_(0, "[登陆密码修改-收发CCR,PasswdChange],请求时间：m_iReqTime:%d",oRes->m_iReqTime);
	
	//if(oRes->m_iReqTime > 0)
		//long2date(oRes->m_iReqTime,oRes->m_requestTime);
	//else
		//GetTime(oRes->m_requestTime);
	
	int iRet=0;
	int LoginServType; //用户类型：互联网用户 OR 通信证用户
	
	try
	{
		for (g_itr2=g_vPasswdChangeCond.begin(); g_itr2!=g_vPasswdChangeCond.end(); ++g_itr2)
		{
			__DEBUG_LOG0_(0, "[登陆密码修改-收发CCR,PasswdChange]:取自服务平台请求CCR第二层支付信息：");
			 LoginServType = (*g_itr2)->m_hDestType;
			__DEBUG_LOG1_(0, "[登陆密码修改-收发CCR,PasswdChange],判断用户类型：[%d](1-通行证帐号 2-业务平台帐号)",LoginServType);
			
			switch(LoginServType)
			{
				case 2:
					//互联网用户
					//调用3.4.1.4HSS密码修改接口
					__DEBUG_LOG0_(0, "[登陆密码修改-收发CCR,PasswdChange]:互联网用户调用HSS密码修改接口");
					
					iRet = PasswdHSSChange(oRes,*g_itr2);
			 		if(iRet!=0)
			 		{
			 			throw -1;
			 			__DEBUG_LOG0_(0, "[登陆密码修改-收发CCR,PasswdChange]:登陆密码鉴权到全国中心HSS修改失败");
			 		}
			 		break;
				case 1:
					//通行证用户
					//调用3.4.3.1通行证用户密码修改接口
					__DEBUG_LOG0_(0, "[登陆密码修改-收发CCR,PasswdChange]:通行证用户调用通行证用户修改接口");
					//通信证用户密码鉴权函数
					break;
			}
		}
	}
	catch(TTStatus oSt) 
	{
		iRet=-1;
		__DEBUG_LOG1_(0, "[登陆密码修改-收发CCR,PasswdChange] oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "[登陆密码修改-收发CCR,PasswdChange] iRet=%d",value);
	}
	
	return iRet;	
}

//互联网用户发往全国HSS端密码修改
int PasswdChange::PasswdHSSChange(PasswdChangeStruct *oRes,PasswdChangeCond *g_itr2)
{
	int iRet=0;
	
	//主动请求CCR信息
	PasswdChangeStruct oReqPasswdStruct;
	PasswdChangeCond oReqPasswdChange={0};
	vector<PasswdChangeCond> vResPasswdChangeItem;
	
	//保存CCR消息
	__DEBUG_LOG0_(0, "========================请求中心ABM登陆密码修改CCR入TT=======================");
	__DEBUG_LOG0_(0, "[登陆密码修改-收发CCR,自服务平台]自服平台请求CCR明细:");
	__DEBUG_LOG1_(0, "请求流水：m_sReqID:[%s]",oRes->m_sReqID);
	__DEBUG_LOG1_(0, "请求时间：m_iReqTime:[%d]",oRes->m_iReqTime);
	__DEBUG_LOG1_(0, "用户号码：m_sDestAccount:[%s]",g_itr2->m_sDestAccount);
	__DEBUG_LOG1_(0, "标识类型：m_hDestType:[%d]",g_itr2->m_hDestType);
	__DEBUG_LOG1_(0, "用户属性：m_hDestAttr:[%d]",g_itr2->m_hDestAttr);
	__DEBUG_LOG1_(0, "业务平台标识：m_sServPlatID:[%s]",g_itr2->m_sServPlatID);
	__DEBUG_LOG1_(0, "当前密码：m_sOldPasswd:[%s]",g_itr2->m_sOldPasswd);
	__DEBUG_LOG1_(0, "重置密码：m_sNewPasswd:[%s]",g_itr2->m_sNewPasswd);
	
	struct struDccHead receiveDccHead;
	char dccType[1];	// DCC消息类型
	strcpy(dccType,"R"); 
	long lDccRecordSeq=0L;
	
	try
	{
		// 取DCC_INFO_RECORD_SEQ
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		
		// dcc包消息头信息
		__DEBUG_LOG0_(0, "[登陆密码修改-收发CCR,自服务平台]:保存CCR包消息头信息，入TT");
		// 获取收到的CCR包中的消息头
		memcpy((void *)&receiveDccHead,(void *)(m_poUnpack->m_sDccBuf),sizeof(receiveDccHead));
		// 保存CCR消息头，入TT
		iRet = m_dccOperation->insertDccInfo(receiveDccHead,dccType,lDccRecordSeq);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[登陆密码修改-收发CCR,自服务平台]:保存CCR包消息头信息失败");
			throw -1;
		}
		
		// dcc包业务数据信息
		__DEBUG_LOG0_(0, "[登陆密码修改-收发CCR,自服务平台]:保存CCR包业务信息，入TT");
		iRet = m_dccOperation->savePlatformCCRforChange(receiveDccHead.m_sSessionId,oRes,g_itr2);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[登陆密码修改-收发CCR,发往HSS端]:保存CCR业务信息失败");
			throw -1;
		}
		
		// 组CCR请求信息
		__DEBUG_LOG0_(0, "========================组登陆密码修改CCR请求信息=======================");
		
		//数据准备
		//第一层信息
		GetTime(oReqPasswdStruct.m_requestTime);		//请求时间
		time_t sTime;
		sTime = time(NULL);
		oReqPasswdStruct.m_iReqTime=sTime;
		
		strcpy(oReqPasswdStruct.m_sReqID,oRes->m_sReqID);	//请求流水
		
		//第二层明细
		strcpy(oReqPasswdChange.m_sDestAccount,g_itr2->m_sDestAccount);	//用户号码
		oReqPasswdChange.m_hDestType = g_itr2->m_hDestType;		//用户类型
		oReqPasswdChange.m_hDestAttr = g_itr2->m_hDestAttr;	//用户属性
		strcpy(oReqPasswdChange.m_sServPlatID,g_itr2->m_sServPlatID);	//业务平台标识
		strcpy(oReqPasswdChange.m_sOldPasswd,g_itr2->m_sOldPasswd);	//当前密码
		strcpy(oReqPasswdChange.m_sNewPasswd,g_itr2->m_sNewPasswd);	//重置密码
		
		__DEBUG_LOG1_(0, "请求流水：PasswdChangeStruct.m_sReqID:[%s]",oReqPasswdStruct.m_sReqID);
		__DEBUG_LOG1_(0, "请求时间：PasswdChangeStruct.m_requestTime:[%s]",oReqPasswdStruct.m_requestTime);
		__DEBUG_LOG1_(0, "用户号码：oReqPasswdChange.m_sDestAccount:[%s]",oReqPasswdChange.m_sDestAccount);
		__DEBUG_LOG1_(0, "用户类型：oReqPasswdChange.m_hDestType:[%d]",oReqPasswdChange.m_hDestType);
		__DEBUG_LOG1_(0, "用户属性：oReqPasswdChange.m_hDestAttr:[%d]",oReqPasswdChange.m_hDestAttr);
		__DEBUG_LOG1_(0, "业务平台标识：oReqPasswdChange.m_sServPlatID:[%s]",oReqPasswdChange.m_sServPlatID);
		__DEBUG_LOG1_(0, "登陆密码：oReqPasswdChange.m_sOldPasswd:[%s]",oReqPasswdChange.m_sOldPasswd);
		__DEBUG_LOG1_(0, "登陆密码：oReqPasswdChange.m_sNewPasswd:[%s]",oReqPasswdChange.m_sNewPasswd);
		
		vResPasswdChangeItem.push_back(oReqPasswdChange);
		
		//设置消息头
		// 生成Session-id
		long lSessionIdSeq;
		iRet = m_dccOperation->QueryPaySeq(lSessionIdSeq);
		if(iRet !=0)
		{
			__DEBUG_LOG0_(0, "[登陆密码修改-收发CCR,发往HSS端]:查询DCC会话标识Session-Id失败");
			throw -1;
		}
		
		struct struDccHead Head;
		char sSessionId[64];
		time_t tTime;
		tTime = time(NULL);
		
		// 2011.8.1
		snprintf(Head.m_sSessionId,sizeof(Head.m_sSessionId),"%s%010d%s%04d\0","ABM@001.ChinaTelecom.com;",tTime,";",lSessionIdSeq);
		__DEBUG_LOG1_(0, "SESSION_ID:[%s]",Head.m_sSessionId);
	
		strcpy(Head.m_sSrvTextId,"LogonPassword_Reset.Micropayment@001.ChinaTelecom.com");
		strcpy(Head.m_sOutPlatform,"001.ChinaTelecom.com");
		strcpy(Head.m_sOrignHost,"ABM@001.ChinaTelecom.com"); // 2011.8.1
		strcpy(Head.m_sOrignRealm,"001.ChinaTelecom.com");
		strcpy(Head.m_sDestRealm,"001.ChinaTelecom.com");
/*	
		char sOrgId[4];
		
		char sAccoutNbr[32];
		strcpy(sAccoutNbr,"18951765356");
		//iRet = m_dccOperation->qryAccountOrgId(reqPayInfo.m_iDestinationAttr,sOrgId); 

		if(iRet !=0 )
		{
			__DEBUG_LOG0_(0, "error");
			throw -1;
		}
		
		snprintf(Head.m_sDestRealm,sizeof(Head.m_sDestRealm),"%s.ChinaTelecom.com",sOrgId);
		__DEBUG_LOG1_(0, "Head.m_sDestRealm:[%s]",Head.m_sDestRealm);
*/	
		Head.m_iAuthAppId = 4;
		Head.m_iReqType = 4;
		Head.m_iReqNumber  = 0;
		Head.m_iResultCode = 0;
		
		// 发送CCR消息头赋值
		memcpy((void *)(m_poPasswdPack->m_sDccBuf),(void *)&Head,sizeof(Head));
		
		m_poPasswdPack->m_iCmdID = ABMCMD_PASSWD_CHANGE_CCA;
		
		//打包第一层CCR
		if (!m_poPasswdPack->addPasswdChangeRespons(oReqPasswdStruct))
		{
			__DEBUG_LOG0_(0, "PasswdChange::PasswdHSSChange 第一层CCR打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			return -1;
		}
		__DEBUG_LOG1_(0, "m_poPasswdPack->m_iCmdID=%d",m_poPasswdPack->m_iCmdID);
		
		__DEBUG_LOG0_(0, "第一层服务CCA打包完成");
		
		// 打包 - 第二层扣费明细
		for(int j=0;j<vResPasswdChangeItem.size();j++)
		{
			if (!m_poPasswdPack->addPasswdChange(vResPasswdChangeItem[j]))
			{
				__DEBUG_LOG0_(0, "PasswdChange:: PasswdHSSChange m_poPack 打包失败!");
				m_poPack->setRltCode(ECODE_PACK_FAIL);
				throw -1;
			}	
		}
		
		__DEBUG_LOG0_(0, "[登陆密码修改-收发CCR,发往HSS端]:CCA打包完成");
		
		// 取DCC_INFO_RECORD_SEQ
		iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
		// 保存发出的CCR信息
		__DEBUG_LOG0_(0, "[登陆密码修改-收发CCR,发往HSS端]:保存发出的DCC包信息入TT");
		iRet = m_dccOperation->insertDccInfo(Head,dccType,lDccRecordSeq,receiveDccHead.m_sSessionId);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "[登陆密码修改-收发CCR,发往HSS端]:保存DCC业务信息失败");
			throw -1;
		}
	}
	catch(TTStatus oSt)
	{
		iRet = -1;
		__DEBUG_LOG1_(0, "[登陆密码修改-收发CCR,发往HSS端]: oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "[登陆密码修改-收发CCR,发往HSS端]: iRet=%d",value);
	}
	__DEBUG_LOG1_(0, "[登陆密码修改-收发CCR,发往HSS端]-ActiveAllocateBalance::deductBalanceOut,iRet=:[%d]",iRet);
	
	return iRet;
}