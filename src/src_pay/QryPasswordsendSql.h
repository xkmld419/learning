#ifndef __QRY_PASSWORD_SEND_SQL_H_INCLUDED_
#define __QRY_PASSWORD_SEND_SQL_H_INCLUDED_

#include "TTTns.h"
#include "abmsendcmd.h"
#include "LogV2.h"

class ABMException;

class QryPasswordSendSql : public TTTns
{

public:

    QryPasswordSendSql();

    ~QryPasswordSendSql();

	 int init(ABMException &oExp);
    int selectServNbr(QryPasswordCond *psd,ABMException &oExp);		//下发号码验证
    int insertpwSql(char *passwd,PasswordResponsStruct *Res,QryPasswordCond *psd,ABMException &oExp);			//写随机密码表
    int insertSendSql(long t_servID,char *msg,QryPasswordCond *psd,ABMException &oExp);			//写短信工单表
    int updatepwSql(char *passwd,QryPasswordCond *psd,ABMException &oExp);			//更改随机密码表
    int selectServID(QryPasswordCond *psd,ABMException &oExp);			//根据手机号码查用户ID
public:
	long servId;				//下发手机号码对应SERVID
    
};

#endif/*__QRY_PASSWORD_SEND_SQL_H_INCLUDED_*/
