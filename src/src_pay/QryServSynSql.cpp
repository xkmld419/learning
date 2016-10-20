#include "QryServSynSql.h"
#include "ABMException.h"
#include "LogV2.h"

extern TimesTenCMD *m_pUpdateServAddState;	//用户信息增量表同步状态更新
extern TimesTenCMD *m_pInsertSendSql;	//写短信工单表
extern TimesTenCMD *m_pInserUserAddErr;	//资料同步增量表记录错误码

TimesTenCMD *m_poTest;

QryServSynSql::QryServSynSql()
{
    m_poTest = NULL;   
}

QryServSynSql::~QryServSynSql()
{
#define __FREE_PTR(x) if (x != NULL) delete x
    __FREE_PTR(m_poTest);
}

int QryServSynSql::init(ABMException &oExp)
{
	try{
        if (GET_CURSOR(m_poTest, m_poTTConn, m_sConnTns, oExp) != 0) {
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

/*扫描用户信息增量表*/
int QryServSynSql::selectServAdd(ABMException &oExp)
{
	__DEBUG_LOG0_(0, "enter selectServAdd...");
	time_t   Time;
	struct tm *T;
	int m_Seq;
	QryServSynCond synBuffer;
	char sql[1024];
	try{
		//发送CCR参照用时间
		snprintf(sql,sizeof(sql),"%s","update USER_INFO_ADD set state_date=SYSDATE, send_date=SYSDATE,session_id='ABM2@ChinaTelecom.com'||';'||to_char(sysdate,'YYYYMMDDHH24MISS')||';'||to_char(dcc_info_record_seq.nextval) where syn_state is NULL or(syn_state='ERR' and err_nbr in(15000,15001,15020,15021,15022,15026))");
		m_poTest->Prepare(sql);
		m_poTest->Commit();
		m_poTest->Execute();
		m_poTest->Commit();
		m_poTest->Close();
		
		//检索需要生成CCR信息的数据
		snprintf(sql,sizeof(sql),"%s","select request_id_seq.nextval,A.USER_TYPE,A.USER_LOGIN_ID,A.USER_ATTR,A.SERV_ID,A.ACCT_ID,B.ATTR_VALUES,C.SERVICE_PLATFORM_ID	"
						"from USER_INFO_ADD A,ATT_USER_INFO B,APP_USER_INFO C "
						"where  (A.SYN_STATE is NULL or (syn_state='ERR' and err_nbr in (15000,15001,15020,15021,15022,15026))) AND B.ATTR_TYPE='03' AND A.SERV_ID=B.SERV_ID	"
						"AND A.SERV_ID=C.SERV_ID");

        m_poTest->Prepare(sql);
        m_poTest->Commit();
        m_poTest->Execute();
        
        while (!m_poTest->FetchNext()) {
        __DEBUG_LOG0_(0, "enter m_poTest->FetchNext...");
        m_poTest->getColumn(1, &m_Seq);
        m_poTest->getColumn(2, &synBuffer.m_hDsnType);
        m_poTest->getColumn(3, synBuffer.m_sDsnNbr);
        m_poTest->getColumn(4, &synBuffer.m_hDsnAttr);
        m_poTest->getColumn(5, &synBuffer.m_lServID);
        m_poTest->getColumn(6, &synBuffer.m_lAcctID);
        m_poTest->getColumn(7, synBuffer.m_sAccPin);
        m_poTest->getColumn(8, synBuffer.m_sServPlatID);
	
	__DEBUG_LOG1_(0, "发送HSS资料同步CCR,号码:[%s]",synBuffer.m_sDsnNbr);
	__DEBUG_LOG1_(0, "发送HSS资料同步CCR,账号值类型:[%d]",synBuffer.m_hDsnType);
	__DEBUG_LOG1_(0, "发送HSS资料同步CCR,用户属性:[%d]",synBuffer.m_hDsnAttr);
		 
		 time(&Time);
		 T=localtime(&Time);
        sprintf(synBuffer.m_sReqID,"002%04d%02d%02d%02d%02d%02d%d00",T->tm_year+1900,T->tm_mon+1,T->tm_mday,T->tm_hour, T->tm_min,T->tm_sec,m_Seq);
        strncpy(synBuffer.m_sActType, "01", sizeof(synBuffer.m_sActType)-1);
        
        vec.push_back(synBuffer);
    }
    m_poTest->Close();
    return 0;
				}
		catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "QryServInforSql::init oSt.err_msg=%s", oSt.err_msg);
    }
    return QRY_USER_INFO_ADD_ERR;
}

/*session_id  struct QryServSynCond sCond*/
int QryServSynSql::selectSession(char *m_oDsnNbr,ABMException &oExp)
{
	__DEBUG_LOG0_(0, "enter selectSession...");
	char sql[1024];
	char m_oSessionId[64];
	sprintf(m_oSessionId,"\0");
	
	try{
		//发送CCR参照用session_id
		snprintf(sql,sizeof(sql),"%s","select session_id from user_info_add where user_login_id=:p0");
		
		m_poTest->Prepare(sql);
		m_poTest->Commit();
		m_poTest->setParam(1,m_oDsnNbr);
		m_poTest->Execute();
		
		while (!m_poTest->FetchNext()) {
        __DEBUG_LOG0_(0, "enter m_poTest->FetchNext...");
        m_poTest->getColumn(1, m_oSessionId);
        ses.push_back(m_oSessionId);
		}
		m_poTest->Close();
		return 0;
	}
	catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "QryServInforSql::init oSt.err_msg=%s", oSt.err_msg);
    }
	return QRY_SESSION_ID_ERR;
}

/*用户信息增量同步状态更新*/
int QryServSynSql::updateServAddState(char *m_oSessionId,ABMException &oExp)
{
	char sql[1024];
	try{
			m_pUpdateServAddState->setParam(1,m_oSessionId);
			m_pUpdateServAddState->Execute();
			m_pUpdateServAddState->Close();
			
			return 0;
		}
		catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "QryServInforSql::init oSt.err_msg=%s", oSt.err_msg);
    }
    return UPDATE_USER_INFO_ADD_SYNSTATE_ERR;
}

/*写短信工单表*/
int QryServSynSql::insertSendSql(char *m_oSessionId,ABMException &oExp)
{
	char msg[1024];
	char sql[1024];
	sprintf(msg,"尊敬的客户您好,欢迎使用中国电信通讯业务,你已开户成功");
   try {
      //snprintf(sql, sizeof(sql), "%s", "INSERT INTO SMS_SEND(SMS_SEND_ID,SERV_ID,ACC_BNR,MSG_TYPE_ID,CREATED_DATE,STATE, MSG_CONTENT)select SMS_SEND_SEQ.nextval,serv_id,user_login_id,:p0,sysdate,:p1,:p2 from user_info_add where session_id=:p3 ") ;

      //m_poTest->Prepare(sql);
      //m_poTest->Commit();
      
		m_pInsertSendSql->setParam(1,2);
		m_pInsertSendSql->setParam(2,0);
		m_pInsertSendSql->setParam(3,msg);
		m_pInsertSendSql->setParam(4,m_oSessionId);
		
		m_pInsertSendSql->Execute();
      m_pInsertSendSql->Commit();
      m_pInsertSendSql->Close();
      
      return 0;
      }
  catch (TTStatus oSt) {
      ADD_EXCEPT1(oExp, "QryPasswordSendSql::init oSt.err_msg=%s", oSt.err_msg);
  }
  return INSERT_SMS_SEND_ERR;
}

int QryServSynSql::insertUserAddErr(char *m_oSessionId,int m_oErrId,ABMException &oExp)
{
	try{
		m_pInserUserAddErr->setParam(1,m_oErrId);
		m_pInserUserAddErr->setParam(2,m_oSessionId);
		m_pInserUserAddErr->Execute();
      m_pInserUserAddErr->Close();
	
		return 0;
	}
	catch (TTStatus oSt) {
      ADD_EXCEPT1(oExp, "QryPasswordSendSql::init oSt.err_msg=%s", oSt.err_msg);
  }
  return INFO_SYN_ERROR_ID_ERR;
}
