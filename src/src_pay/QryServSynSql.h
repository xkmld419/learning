#ifndef __QRY_SERV_SYN_SQL_H_INCLUDED_
#define __QRY_SERV_SYN_SLQ_H_INCLUDED_

#include "TTTns.h"
#include "abmcmd.h"
#include "PublicConndb.h"
#include "errorcode_pay.h"

/*
* 资料同步短信下发功能类
*/

class ABMException;

class QryServSynSql : public TTTns
{

public:

    QryServSynSql();

    ~QryServSynSql();

    int init(ABMException &oExp);
    int selectServAdd(ABMException &oExp);		//取用户信息增量数据(针对开户)
    int selectSession(char *m_oDsnNbr,ABMException &oExp);
    int updateServAddState(char *m_oSessionId,ABMException &oExp);		//用户信息增量同步状态更新
    int insertSendSql(char *m_oSessionId,ABMException &oExp);	       //写短信工单表
    int insertUserAddErr(char *m_oSessionId,int m_oErrId,ABMException &oExp);	//用户信息增量表同步返回错误码
    
public:
	vector<struct QryServSynCond> vec;
	//char m_oSessionId[64];
	vector<char *> ses;
};

#endif/*__QRY_SERV_SYN_SQL_H_INCLUDED_*/
