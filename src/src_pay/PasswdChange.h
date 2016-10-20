#ifndef __PASSWD_CHANGE_H_INCLUDED_
#define __PASSWD_CHANGE_H_INCLUDED_

#include "abmcmd.h"
#include "DccOperation.h"
#include "DccOperationSql.h"

class ABMException;
class DccOperation;

/*
*  登陆密码修改类
*/

class PasswdChange
{
	public:
		PasswdChange();
		~PasswdChange();
		
		//int init(ABMException &oExp);
		int deal(ABMCCR *pCCR, ABMCCA *pSendPack); // 仅处理收的CCR
		
	private:
		 int _deal(PasswdChangeStruct *oRes,vector<PasswdChangeCond *> g_vPasswdChangeCond);//登陆密码鉴权处理函数
		 int PasswdHSSChange(PasswdChangeStruct *oRes,PasswdChangeCond *g_itr2);
		 
	private:
		ABMException *oExp;
		DccOperation *m_dccOperation;
		PasswdChangeCCR *m_poPasswdPack; // 打包-向全国HSS端请求登陆密码修改的CCR包
		PasswdChangeCCR *m_poUnpack; // 解包-自服务平台的CCR包
		PasswdChangeCCA *m_poPack;//打包-返回子服务平台的CCA包
};



#endif/*__PASSWD_CHANGE_H_INCLUDED_*/