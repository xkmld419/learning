#ifndef __QRY_SERVINFO_SYN_SQL_H_INCLUDED_
#define __QRY_SERVINFO_SYN_SQL_H_INCLUDED_

#include "TTTns.h"
#include "abmcmd.h"
#include "LogV2.h"

class ABMException;

class QryServInfosynSql : public TTTns
{

public:

    QryServInfosynSql();

    ~QryServInfosynSql();

	 //int init(ABMException &oExp);
	 
	 //销户、过户
	 int insertServHis(QryServSynCond *syn,ABMException &oExp);
	 int insertAcctHis(QryServSynCond *syn,ABMException &oExp);
	 int insertServAcctHis(QryServSynCond *syn,ABMException &oExp);
	 int deleteAcctBalance(QryServSynCond *syn,ABMException &oExp);
	 int deleteServInfo(QryServSynCond *syn,ABMException &oExp);
	 int deleteAcctInfo(QryServSynCond *syn,ABMException &oExp);
	 int deleteServAcctInfo(QryServSynCond *syn,ABMException &oExp);
	 
	 int createAcct(QryServSynCond *syn,ABMException &oExp);
	 int updateServAcct(QryServSynCond *syn,ABMException &oExp);
	 
	 //换号
	 int changeInservHis(QryServSynCond *syn,ABMException &oExp);
	 int changeNbr(QryServSynCond *syn,ABMException &oExp);
	 
	 
public:
	//long servId;				//下发手机号码对应SERVID
    
};

#endif/*__QRY_SERVINFO_SYN_SQL_H_INCLUDED_*/