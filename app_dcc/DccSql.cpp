#include "DccSql.h"

char DccSql::m_sConnTns[256] = "abm";
    
DccSql::~DccSql()
{
    if (m_poQrySession != NULL)
        delete m_poQrySession;
    if (m_poTTConn != NULL)
        delete m_poTTConn;
}

int DccSql::init(ABMException &oExp)
{
    char sql[1024];
    
    try {
        snprintf(sql, sizeof(sql), 
           "select SRV_CONTEXT_ID from DCC_INFO_RECORD where session_id= ?");
        if (GET_CURSOR(m_poQrySession, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "DccSql::init m_poQrySession init failed!");
            return -1;
        }
        m_poQrySession->Prepare(sql);
        m_poQrySession->Commit();
        
        return 0;
    } catch (TTStatus &st) {
        ADD_EXCEPT1(oExp, "DccSql::init error:err_msg=%s", st.err_msg);
    }
    return -1;
}

int DccSql::ccrInsert(ABMException &oExp)
{
    char sql[1024];
    
    try {
        sprintf(sql,"INSERT INTO DCC_INFO_RECORD(RECORD_ID,SESSION_ID, ORIGIN_HOST, ORIGIN_REALM,SRV_FLOW_ID,RECORD_TIME,RECCORD_TYPE) VALUES \
	        (DCC_INFO_RECORD_SEQ.NEXTVAL,?,?,?,?,sysdate,?)");
			
        if (GET_CURSOR(m_poQrySession, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "DccSql::init m_poQrySession init failed!");
            return -1;
        }
        m_poQrySession->Prepare(sql);
        m_poQrySession->Commit();
        
        return 0;
    } catch (TTStatus &st) {
        ADD_EXCEPT1(oExp, "DccSql::init error:err_msg=%s", st.err_msg);
    }
    return -1;
}
         