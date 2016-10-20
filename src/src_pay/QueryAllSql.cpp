#include "QueryAllSql.h"
#include "TimesTenAccess.h"

const char *sQueryAll = "SELECT "
                        "A.MEASURE_UNIT, "
                        "B.BALANCE, "
                        "F.ORG_ID "
                    "FROM "                                         
                        "BALANCE_TYPE A, "                            
                        "ACCT_BALANCE B, "                            
                        "SERV C, "                               
                        "SERV_ACCT D, "
                        "APP_USER_INFO E, "
                        "AREA_ORG F "
                    "WHERE "                                        
                        "C.ACC_NBR = ? AND "        
                        //"C.USER_TYPE = ? AND "
                        //"C.USER_ATTR = ? AND "
                        //"E.SERVICE_PLATFORM_ID = ? AND "
                        "E.SERV_ID = C.SERV_ID AND "
                        "C.SERV_ID = D.SERV_ID AND "                  
                        "D.ACCT_ID = B.ACCT_ID AND "
                        "E.LOCAL_AREA_ID = F.LOCAL_AREA_ID AND "
                        "B.BALANCE_TYPE_ID =  A.BALANCE_TYPE_ID ;";
const char *sInsertDcc = "insert into "
    "DCC_INFO_RECORD(session_id,origin_host,origin_realm,dest_realm,srv_context_id,srv_flow_id,record_id) "
    "values(?,?,?,?,?,?,?)";

const char *sInsertMsg = "insert into abm_micro_pay(session_id,unittype_id,BalAmount,request_id) "
    "values(?,?,?,?)";

const char *sGetDcc = "select session_id,origin_host,origin_realm,dest_realm,srv_context_id,srv_flow_id "
    "from dcc_info_record where session_id=?";

const char *sGetMsg = "select UnitType_ID,BalAmount,request_id "
    "from abm_micro_pay where session_id=? ";

const char *sGetSeq = "SELECT DCC_INFO_RECORD_SEQ.NEXTVAL FROM DUAL  ";


QueryAllSql::QueryAllSql()
{
    m_poQueryAll = NULL;
	
    m_poInsertDcc = NULL;
    
    m_poInsertMsg = NULL;
    
    m_poGetDcc = NULL;
    
    m_poGetMsg = NULL;

    m_poGetSeq = NULL;
}
QueryAllSql::~QueryAllSql()
{
    m_poQueryAll->Drop();
    m_poTTConn->disconnect();
    #define __FREE_PTR_(p) if( p != NULL )\
        {\
        delete p;\
        p = NULL;\
        }
    __FREE_PTR_(m_poQueryAll);
    __FREE_PTR_(m_poInsertDcc);
    __FREE_PTR_(m_poInsertMsg);
    __FREE_PTR_(m_poGetDcc);
    __FREE_PTR_(m_poGetMsg);
    __FREE_PTR_(m_poGetSeq);
    __FREE_PTR_(m_poTTConn);
}
int QueryAllSql::init(ABMException &oExp)
{
    try {
    
        if (GET_CURSOR(m_poQueryAll, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "QueryAllSql::init m_poQueryAll init failed!");
            return -1;
        }
	if (GET_CURSOR(m_poInsertDcc, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "QueryAllSql::init m_poInsertDcc init failed!");
            return -1;
        }
        
        if (GET_CURSOR(m_poInsertMsg, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "QueryAllSql::init m_poInsertMsg init failed!");
            return -1;
        }
        
        if (GET_CURSOR(m_poGetDcc, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "QueryAllSql::init m_poGetDcc init failed!");
            return -1;
        }
        
        if (GET_CURSOR(m_poGetMsg, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "QueryAllSql::init m_poGetMsg init failed!");
            return -1;
        }
        if (GET_CURSOR(m_poGetSeq, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "QueryAllSql::init m_poGetSeq init failed!");
            return -1;
        }
    
        m_poQueryAll->Prepare(sQueryAll);
        m_poQueryAll->Commit();
		
        m_poInsertDcc->Prepare(sInsertDcc);
        m_poInsertDcc->Commit();
        
        m_poInsertMsg->Prepare(sInsertMsg);
        m_poInsertMsg->Commit();
        
        m_poGetDcc->Prepare(sGetDcc);
        m_poGetDcc->Commit();
        
        m_poGetMsg->Prepare(sGetMsg);
        m_poGetMsg->Commit();

        m_poGetSeq->Prepare(sGetSeq);
        m_poGetSeq->Commit();
        return 0;
    }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "QueryAllSql::init oSt.err_msg=%s", oSt.err_msg);
		cout<<"errmsg:"<<oSt.err_msg<<endl;
    }
    
    return -1;
}

int QueryAllSql::GetSeq(long &lSessionIdSeq)
{
    try
    {
        m_poGetSeq->Execute();
        if(!m_poGetSeq->FetchNext())
        {
            m_poGetSeq->getColumn(1,&lSessionIdSeq);
        }
        m_poGetSeq->Close();
        return 0;
    }
    catch(TTStatus oSt)
    {
        cout<<"get seq failed";
        cout<<"oSt.err_msg="<<oSt.err_msg<<endl;
    }
    return -1;
}
