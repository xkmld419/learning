#ifndef __PASSWD_IDENTIFY_H_INCLUDED_
#define __PASSWD_IDENTIFY_H_INCLUDED_

#include "abmcmd.h"
#include "DccOperation.h"
#include "DccOperationSql.h"

class ABMException;
class DccOperation;

/*
*   登陆密码鉴权类
*/

class PasswdIdentify
{
	public:
		PasswdIdentify();
		~PasswdIdentify();
		
		//int init(ABMException &oExp);
		int deal(ABMCCR *pCCR, ABMCCA *pSendPack); // 仅处理收的CCR
		
	private:
		 int _deal(LoginPasswdStruct *oRes,vector<LoginPasswdCond *> g_vLoginPasswdCond);//登陆密码鉴权处理函数
		 int loginHSSIdentify(LoginPasswdStruct *oRes,LoginPasswdCond *g_itr2);
		 
	private:
		ABMException *oExp;
		DccOperation *m_dccOperation;
		PasswdIdentifyCCR *m_poPasswdPack; // 打包-向全国HSS端请求登陆密码鉴权的CCR包
		//PasswdIdentifyCCA *m_poPasswdUnpack;//解包-全国HSS端返回鉴权结果的CCA包
		LoginPasswdCCR *m_poUnpack; // 解包-自服务平台的CCR包
		LoginPasswdCCA *m_poPack;//打包-返回子服务平台的CCA包
};



#endif/*__PASSWD_IDENTIFY_H_INCLUDED_*/