#ifndef __PASSWD_CHANGE_BIZ_H_INCLUDED_
#define __PASSWD_CHANGE_BIZ_H_INCLUDED_

#include "abmcmd.h"
#include "DccOperation.h"
#include "DccOperationSql.h"

class ABMException;
class DccOperation;

/*
*   登陆密码修改类
*	 全国HSS端接收到全国ABM的登陆密码修改CCR 对登陆密码鉴权返回结果给全国中心ABM
*/

class PasswdChangeBiz
{
	public:
		PasswdChangeBiz();
		~PasswdChangeBiz();
		
		//int init(ABMException &oExp);
		int deal(ABMCCR *pRecivePack, ABMCCA *pSendPack); // 处理接收的CCA，主动发CCA
	private:
		int _deal(PasswdChangeStruct oRes,PasswdChangeData *pData);
	private:
		ABMException *oExp;
		DccOperation *m_dccOperation;
		PasswdChangeCCA *m_poUnpack;	//解包-全国HSS返回的CCA包 全国HSS到全国ABM
		PasswdChangeCCA *m_poPack;	//打包-返回给自服务平台的CCA包		
};

#endif /*__PASSWD_CHANGE_BIZ_H_INCLUDED_*/
