#include "HssQuerySql.h"
#include "TimesTenAccess.h"

const char *sBalSql = "SELECT "
                        "A.BALANCE_TYPE_NAME, "
                        "A.MEASURE_UNIT, "
                        "B.BALANCE, "
                        "X.SU, "                	
                        "TO_CHAR(B.EFF_DATE,\'YYYYMMDDHH24MISS\'), "
                        "TO_CHAR(B.EXP_DATE,\'YYYYMMDDHH24MISS\')  "
                    "FROM "                                         
                        "BALANCE_TYPE A, "                            
                        "ACCT_BALANCE B, "                            
                        "SERV C, "                               
                        "SERV_ACCT D, "
                        "(SELECT SUM(E.AMOUNT) SU "
                        "FROM "
                            "SERV C, "
                            "ACCT_BALANCE B, " 
                            "SERV_ACCT D, "
                            "BALANCE_PAYOUT E "
                        "WHERE "
                            "C.ACC_NBR = ? AND "        
                            //"C.USER_TYPE = ? AND "                      
                            //"C.USER_ATTR = ? AND "                     
                            "C.SERV_ID = D.SERV_ID AND "                  
                            "D.ACCT_ID = B.ACCT_ID AND "                  
                            "B.ACCT_BALANCE_ID = E.ACCT_BALANCE_ID  AND "
                            "E.OPER_TYPE = \'5UI\' AND "
                            "E.STATE = \'00A\' AND "
                            "TO_CHAR(E.OPER_DATE,\'YYYYMM\') = ? "  
                            ") X "    
                    "WHERE "                                        
                        "C.ACC_NBR = ? AND "        
                        //"C.USER_TYPE = ? AND "
                        //"C.USER_ATTR = ? AND "
                        "TO_CHAR(NVL(B.EFF_DATE,SYSDATE),\'YYYYMMDD\') <= TO_CHAR(SYSDATE,'YYYYMMDD') AND "
                        "TO_CHAR(NVL(B.EXP_DATE,SYSDATE),\'YYYYMMDD\') >= TO_CHAR(SYSDATE,'YYYYMMDD') AND "
                        "C.SERV_ID = D.SERV_ID AND "                  
                        "D.ACCT_ID = B.ACCT_ID AND "                  
                        "B.BALANCE_TYPE_ID =  A.BALANCE_TYPE_ID ";

const char *sBal2Sql = "SELECT "
                        "A.BALANCE_TYPE_NAME, "
                        "A.MEASURE_UNIT, "
                        "B.BALANCE, "
                        "X.SU, "                	
                        "TO_CHAR(B.EFF_DATE,\'YYYYMMDDHH24MISS\'), "
                        "TO_CHAR(B.EXP_DATE,\'YYYYMMDDHH24MISS\')  "
                    "FROM "                                         
                        "BALANCE_TYPE A, "                            
                        "ACCT_BALANCE B, "                            
                        "SERV C, "                               
                        "SERV_ACCT D, "
                        "(SELECT SUM(E.AMOUNT) SU "
                        "FROM "
                            "SERV C, "
                            "ACCT_BALANCE B, " 
                            "SERV_ACCT D, "
                            "BALANCE_PAYOUT E "
                        "WHERE "
                            "C.SERV_ID = ? AND "        
                            //"C.USER_TYPE = ? AND "                      
                            //"C.USER_ATTR = ? AND "                     
                            "C.SERV_ID = D.SERV_ID AND "                  
                            "D.ACCT_ID = B.ACCT_ID AND "                  
                            "B.ACCT_BALANCE_ID = E.ACCT_BALANCE_ID  AND "
                            "E.OPER_TYPE = \'5UI\' AND "
                            "E.STATE = \'00A\' AND "
                            "TO_CHAR(E.OPER_DATE,\'YYYYMM\') = ? "  
                            ") X "    
                    "WHERE "                                        
                        "C.SERV_ID = ? AND "        
                        //"C.USER_TYPE = ? AND "                      
                        //"C.USER_ATTR = ? AND "                      
                        "C.SERV_ID = D.SERV_ID AND "                  
                        "D.ACCT_ID = B.ACCT_ID AND "                  
                        "B.BALANCE_TYPE_ID =  A.BALANCE_TYPE_ID ";


const char *sRecSql = "SELECT "
                        "TO_CHAR(F.OPER_DATE, \'YYYYMMDDHHMISS\'), "
                        "B.ACC_NBR, "
                        "B.USER_ATTR, "
                        "A.SERVICE_PLATFORM_ID, "
                        "F.SOURCE_TYPE, "
                        "F.AMOUNT "
                    "FROM "
                        "APP_USER_INFO A, "
                        "SERV B, "
                        "SERV_ACCT C, "
                        "ACCT_BALANCE D, "
                        "BALANCE_SOURCE F "
                    "WHERE "
                        "F.OPER_TYPE =\'5UA\' AND "
                        "F.STATE = \'10A\' AND "
                        "F.SOURCE_TYPE IN(\'5VF\',\'5VA\') AND "
                        "B.ACC_NBR = ? AND "
                        //"B.USER_TYPE = ? AND "
                        //"B.USER_ATTR = ? AND "
                        "A.SERVICE_PLATFORM_ID = ? AND "
                        "B.SERV_ID = C.SERV_ID AND "
                        "B.SERV_ID = A.SERV_ID AND "
                        "C.ACCT_ID = D.ACCT_ID AND "
                        "F.ACCT_BALANCE_ID = D.ACCT_BALANCE_ID AND "
                        "TO_CHAR(F.OPER_DATE,\'YYYYMMDD\') >= ? AND "
                        "TO_CHAR(F.OPER_DATE,\'YYYYMMDD\') < ? ";

const char *sPaySql = "SELECT "
                        "A.ORDER_ID, "
                        "A.ORDER_DESC, "
                        "A.SP_ID, "
                        "A.SERVICE_PLATFORM_ID, "
                        "TO_CHAR(A.OPT_DATE, \'YYYYMMDDHH24MISS\'), "
                        "A.ORDER_STATE, "
                        "A.PAY_TYPE, "
                        "B.AMOUNT "
                    "FROM "
                        "OPERATION A, "
                        "BALANCE_PAYOUT B, "
                        //"APP_USER_INFO C, "
                        "SERV D "
                    "WHERE "
                    "B.OPER_TYPE = \'5UI\' AND "
                    "B.STATE = \'00A\' AND "
                    "A.SP_ID = ? AND "
                    "A.SERVICE_PLATFORM_ID = ? AND "
                    //"C.SERVICE_PLATFORM_ID = ? AND "
                    "D.ACC_NBR = ? AND "
                    //"D.USER_TYPE = ? AND "
                    //"D.USER_ATTR = ? AND "
                    //"C.SERV_ID = D.SERV_ID AND "
                    //"C.SERV_ID = A.SERV_ID AND "
                    "A.OPT_ID = B.OPT_ID AND "
                    "TO_CHAR(A.OPT_DATE, \'YYYYMMDD\') >= ? AND "
                    "TO_CHAR(A.OPT_DATE, \'YYYYMMDD\') < ? ";

const char *sSimpleTest = "select ACC_NBR from SERV;";

HssQuerySql::HssQuerySql()
{    
    m_poBal = NULL;
    m_poBal2 = NULL;
    m_poRec = NULL;
    m_poPay = NULL;
    m_poBil = NULL;
    m_poTest = NULL;
}
HssQuerySql::~HssQuerySql()
{
    m_poBal->Drop();
    m_poBal2->Drop();
    m_poRec->Drop();
    m_poPay->Drop();
    m_poBil->Drop();
    m_poTest->Drop();
    m_poTTConn->disconnect();
    #define __FREE_PTR_(p) if( p != NULL )\
        {\
        delete p;\
        p = NULL;\
        }
    __FREE_PTR_(m_poBal);
    __FREE_PTR_(m_poBal2);
    __FREE_PTR_(m_poRec);
    __FREE_PTR_(m_poPay);
    __FREE_PTR_(m_poBil);
	__FREE_PTR_(m_poTest);
    __FREE_PTR_(m_poTTConn);
}
int HssQuerySql::init(ABMException &oExp)
{
    try {
    
        if (GET_CURSOR(m_poBal, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "HssQuerySql::init m_poBal init failed!");
            return -1;
        }
        if (GET_CURSOR(m_poBal2, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "HssQuerySql::init m_poBal init failed!");
            return -1;
        }
        if (GET_CURSOR(m_poRec, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "HssQuerySql::init m_poRec init failed!");
            return -1;
        }
        if (GET_CURSOR(m_poPay, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "HssQuerySql::init m_poPay init failed!");
            return -1;
        }
        if (GET_CURSOR(m_poBil, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "HssQuerySql::init m_poBil init failed!");
            return -1;
        }
		if (GET_CURSOR(m_poTest, m_poTTConn, m_sConnTns, oExp) != 0) {
            ADD_EXCEPT0(oExp, "HssQuerySql::init m_poTest init failed!");
            return -1;
        }
    }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "HssQuerySql::init oSt.err_msg=%s", oSt.err_msg);
        return -1;
    }

    try 
    {
        m_poBal->Prepare(sBalSql);
        m_poBal->Commit();
        m_poBal2->Prepare(sBal2Sql);
        m_poBal2->Commit();
        m_poRec->Prepare(sRecSql);
        m_poRec->Commit();
        m_poPay->Prepare(sPaySql);
        m_poPay->Commit();
        m_poTest->Prepare(sSimpleTest);
        m_poTest->Commit();
        return 0;
    }
    catch (TTStatus oSt) {
        ADD_EXCEPT1(oExp, "HssQuerySql::init oSt.err_msg=%s", oSt.err_msg);
    }
    
    return -1;
}
