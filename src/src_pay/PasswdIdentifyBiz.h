#ifndef __PASSWD_IDENTIFY_BIZ_H_INCLUDED_
#define __PASSWD_IDENTIFY_BIZ_H_INCLUDED_

#include "abmcmd.h"
#include "DccOperation.h"
#include "DccOperationSql.h"

class ABMException;
class DccOperation;

/*
*   登陆密码鉴权类
*	 全国HSS端接收到全国ABM的登陆密码鉴权CCR 对登陆密码鉴权返回结果给全国中心ABM
*/

class PasswdIdentifyBiz
{
	public:
		PasswdIdentifyBiz();
		~PasswdIdentifyBiz();
		
		//int init(ABMException &oExp);
		int deal(ABMCCR *pRecivePack, ABMCCA *pSendPack); // 处理接收的CCA，主动发CCA
	private:
		int _deal(LoginPasswdStruct oRes,PasswdIdentifyData *pData,char *sAccNbr);
	private:
		ABMException *oExp;
		DccOperation *m_dccOperation;
		PasswdIdentifyCCA *m_poUnpack;	//解包-全国HSS返回的CCA包 全国HSS到全国ABM
		LoginPasswdCCA *m_poPack;	//打包-返回给自服务平台的CCA包		
};

#endif /*__PASSWD_IDENTIFY_BIZ_H_INCLUDED_*/