#include "QryPasswordsendSql.h"
#include "ABMException.h"
TimesTenCMD *m_poSend;


QryPasswordSendSql::QryPasswordSendSql()
{
	m_poSend=NULL;
}

QryPasswordSendSql::~QryPasswordSendSql()
{
#define __FREE_PTR(x) if (x != NULL) delete x
    __FREE_PTR(m_poSend);
}

int QryPasswordSendSql::init(ABMException &oExp)
{
	try{
			if (GET_CURSOR(m_poSend, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "QryPasswordSendSql::init m_poSend init failed!");
            return -1;
        }
        
			return 0;
		}
		catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "QryPasswordSendSql::init oSt.err_msg=%s", oSt.err_msg);
    }
    return -1;
}

/*下发号码验证*/
int QryPasswordSendSql::selectServNbr(QryPasswordCond *psd,ABMException &oExp)
{
	__DEBUG_LOG0_(0, "selectServNbr...");
	__DEBUG_LOG1_(0, "m_RanAccNbr ：%s",psd->m_RanAccNbr);

	char sql[1024];
	long sBuffer;
	try{
			snprintf(sql,sizeof(sql),"%s","SELECT COUNT(1) FROM RAMDOM_PASSWD WHERE BAND_PHONE_ID=:p0");

        m_poSend->Prepare(sql);
        m_poSend->Commit();
        m_poSend->setParam(1,psd->m_RanAccNbr);

   		m_poSend->Execute();
 
   		while (!m_poSend->FetchNext())
   	  	{
        m_poSend->getColumn(1, &sBuffer);
   		}
 			__DEBUG_LOG1_(0, "sBuffer=%d",sBuffer);	
   		 m_poSend->Close();
 
   		 if(sBuffer == 0)
   		 	return 0;
   		 else
   		 	return 1;
		}
		catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "QryPasswordSendSql::init oSt.err_msg=%s", oSt.err_msg);
    }
    return -1;
}

/*更改随机密码表*/
int QryPasswordSendSql::updatepwSql(char *passwd,QryPasswordCond *psd,ABMException &oExp)
{
	char sql[1024];
	try{
			snprintf(sql,sizeof(sql),"%s","UPDATE RAMDOM_PASSWD SET RANDOM_PASSWD=:p0,OPT_DATE=SYSDATE WHERE BAND_PHONE_ID=:p1");

        m_poSend->Prepare(sql);
        m_poSend->Commit();
		  m_poSend->setParam(1,passwd);
		  m_poSend->setParam(2,psd->m_RanAccNbr);
		  
		  m_poSend->Execute();
		  m_poSend->Commit();
		  m_poSend->Close();        
        return 0;
		}
		catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "QryPasswordSendSql::init oSt.err_msg=%s", oSt.err_msg);
		}
		return -1;
}

/*写随机密码表*/
int QryPasswordSendSql::insertpwSql(char* passwd,PasswordResponsStruct *Res,QryPasswordCond *psd,ABMException &oExp)
{
	__DEBUG_LOG0_(0, "进入insertpwSql...");
	__DEBUG_LOG1_(0, "m_RanAccNbr ：%s",psd->m_RanAccNbr);
	__DEBUG_LOG1_(0, "passwd ：%s",passwd);
	__DEBUG_LOG1_(0, "m_ReqID ：%s",Res->m_ReqID);
	char sql[1024];
    try {
        snprintf(sql, sizeof(sql), "%s", "INSERT INTO RAMDOM_PASSWD(BAND_PHONE_ID,RANDOM_PASSWD,OPT_ID,OPT_DATE)VALUES(:p0,:p1,:p2,SYSDATE) ");
  	
        m_poSend->Prepare(sql);
        m_poSend->Commit();
        
        m_poSend->setParam(1,psd->m_RanAccNbr);
		  m_poSend->setParam(2,passwd);
		  m_poSend->setParam(3,Res->m_ReqID);
		  //m_poSend->setParam(4,"SYSDATE");
		  
		  		  
		  __DEBUG_LOG0_(0, "setParam");
		  m_poSend->Execute();
		  __DEBUG_LOG0_(0, "Execute");
		  m_poSend->Commit();
		  __DEBUG_LOG0_(0, "Commit");

		  m_poSend->Close();
        
        return 0;
        }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "QryPasswordSendSql::init oSt.err_msg=%s", oSt.err_msg);
    }
    return -1;
}

/*写短信工单表*/
int QryPasswordSendSql::insertSendSql(long t_servID,char *msg,QryPasswordCond *psd,ABMException &oExp)
{
	__DEBUG_LOG1_(0,"insertSendSql 进入...servID为%ld",t_servID);
	char sql[1024];
	long t_mservID=t_servID;
	
   try {
      snprintf(sql, sizeof(sql), "%s", "INSERT INTO SMS_SEND(SMS_SEND_ID,SERV_ID,ACC_BNR,MSG_TYPE_ID,CREATED_DATE,STATE, MSG_CONTENT) values (SMS_SEND_SEQ.nextval,:p1,:p2,:p3,SYSDATE,:p4,:p5) ") ;

      m_poSend->Prepare(sql);
      m_poSend->Commit();
      
      //m_poSend->setParam(1,SMS_SEND_SEQ.nextval);
		m_poSend->setParam(1,t_mservID);
		m_poSend->setParam(2,psd->m_RanAccNbr);
		m_poSend->setParam(3,2);
		//m_poSend->setParam(5,"SYSDATE");
		m_poSend->setParam(4,0);
		m_poSend->setParam(5,msg);
		
		__DEBUG_LOG1_(0,"SQL:%s",sql);		
		m_poSend->Execute();
		__DEBUG_LOG0_(0,"Execute()");
      m_poSend->Commit();
      __DEBUG_LOG0_(0,"Commit()");
      m_poSend->Close();
      
      return 0;
      }
  catch (TTStatus oSt) {
      ADD_EXCEPT1(oExp, "QryPasswordSendSql::init oSt.err_msg=%s", oSt.err_msg);
  }
  
  return -1;
}

/*根据用户号码查用户ID*/
int QryPasswordSendSql::selectServID(QryPasswordCond *psd,ABMException &oExp)
{
    __DEBUG_LOG0_(0,"selectServID进入...");
	char sql[1024];
	try {
      snprintf(sql, sizeof(sql), "SELECT SERV_ID FROM SERV WHERE ACC_NBR=:p0" ) ;

      m_poSend->Prepare(sql);
      m_poSend->Commit();
      
      m_poSend->setParam(1,psd->m_RanAccNbr);
	   m_poSend->Execute();
	   
	       while (!m_poSend->FetchNext()) {
        m_poSend->getColumn(1, &servId);
    		}
   		 m_poSend->Close();      
      return 0;
      }
  catch (TTStatus oSt) {
      ADD_EXCEPT1(oExp, "QryPasswordSendSql::init oSt.err_msg=%s", oSt.err_msg);
  }
  return -1;
}
