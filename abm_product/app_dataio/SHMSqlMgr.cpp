#include "SHMSqlMgr.h"
#include "Log.h"


SHMSqlMgr::SHMSqlMgr()
{
    m_poHCodeIn = NULL;
    m_poLocalHeadIn = NULL;
    m_poMinIn = NULL;
    m_poImsiIn = NULL;
    
    m_poHCodeOut = NULL;
    //m_poLocalHeadOut = NULL;
    m_poMinOut = NULL;
    m_poImsiOut = NULL;
    
    m_poMenDb = NULL;
}

SHMSqlMgr::~SHMSqlMgr()
{
    #define __FREE_PTR(x) if (x != NULL) REL_CURSOR(x)
		
    __FREE_PTR(m_poHCodeIn);
    __FREE_PTR(m_poLocalHeadIn);
    __FREE_PTR(m_poMinIn);
    __FREE_PTR(m_poImsiIn);
    
    __FREE_PTR(m_poHCodeOut);
    //__FREE_PTR(m_poLocalHeadOut);
    __FREE_PTR(m_poMinOut);
    __FREE_PTR(m_poImsiOut);
    
    __FREE_PTR(m_poMenDb);
}

int SHMSqlMgr::init(int iTableType,char const *sTable,bool isInsert)
{
		ABMException oExp;
	  char sSQL[2048];
	  memset(sSQL,0,sizeof(sSQL));
//    try
    {
    	switch(iTableType)
    	{
    		case TABLE_HCODE_INFO:
        	if(isInsert)
        	{
        		if(GET_CURSOR(m_poHCodeOut, m_poTTConn, m_sConnTns, oExp) != 0) {
        	    Log::log(0, "SHMSqlMgr::init m_poHCodeOut init failed!");
        	    return -1;
        		}
        		sprintf(sSQL, "INSERT INTO %s (HEAD, TSP_ID, ZONE_CODE, EFF_DATE, EXP_DATE, HEAD_ID) VALUES(:P0,:P1,:P2,:P3,:P4,:P5)" , sTable);
        		m_poHCodeOut->Prepare(sSQL);
        		m_poHCodeOut->Commit();
        	}
        	else
        	{
        		if(GET_CURSOR(m_poHCodeIn, m_poTTConn, m_sConnTns, oExp) != 0) {
        	    Log::log(0, "SHMSqlMgr::init m_poHCodeIn init failed!");
        	    return -1;
        		}
        		sprintf(sSQL, "SELECT HEAD, TSP_ID, ZONE_CODE, EFF_DATE, EXP_DATE, HEAD_ID FROM %s" , sTable);
        		m_poHCodeIn->Prepare(sSQL);
        		m_poHCodeIn->Commit();
        	}
        	break;
        case TABLE_MIN_INFO:
        	if(isInsert)
        	{
        		if (GET_CURSOR(m_poMinOut, m_poTTConn, m_sConnTns, oExp) != 0) {
        		    Log::log(0, "SHMSqlMgr::init m_poMinOut init failed!");
        		    return -1;    
        		}
        		sprintf(sSQL, "INSERT INTO %s (BEGIN_MIN, END_MIN, USER_FLAG, COUNTRY_ID, HOME_CARRIER_CODE, ZONE_CODE,USER_TYPE, EFF_DATE, EXP_DATE, MIN_INFO_ID) VALUES(:P0,:P1,:P2,:P3,:P4,:P5,:P6,:P7,:P8,:P9)" , sTable);
        		m_poMinOut->Prepare(sSQL);
        		m_poMinOut->Commit();
        	}
        	else
        	{
        		if (GET_CURSOR(m_poMinIn, m_poTTConn, m_sConnTns, oExp) != 0) {
        		    Log::log(0, "SHMSqlMgr::init m_poMinIn init failed!");
        		    return -1;    
        		}
        		sprintf(sSQL, "SELECT BEGIN_MIN, END_MIN, USER_FLAG, COUNTRY_ID, HOME_CARRIER_CODE, ZONE_CODE,USER_TYPE, EFF_DATE, EXP_DATE, MIN_INFO_ID FROM %s" , sTable);
        		m_poMinIn->Prepare(sSQL);
        		m_poMinIn->Commit();
        	}
        	break;
        case TABLE_LOCAL_INFO:
        	if(isInsert)
        	{
        		
        	}
        	else
        	{
        		if (GET_CURSOR(m_poLocalHeadIn, m_poTTConn, m_sConnTns, oExp) != 0) {
        		    Log::log(0, "SHMSqlMgr::init m_poLocalHeadIn init failed!");
        		    return -1;
        		}
        		sprintf(sSQL, "SELECT HEAD, EMULATORY_PARTNER_ID, EXCHANGE_ID, EFF_DATE, EXP_DATE, HEAD_ID FROM %s" , sTable);
        		m_poLocalHeadIn->Prepare(sSQL);
        		m_poLocalHeadIn->Commit();
        	}
        	break;
        case TABLE_IMSI_INFO:
        	if(isInsert)
        	{
        		if (GET_CURSOR(m_poImsiOut, m_poTTConn, m_sConnTns, oExp) != 0) {
        		    Log::log(0, "SHMSqlMgr::init m_poImsiOut init failed!");
        		    return -1;    
        		}
        		sprintf(sSQL, "INSERT INTO %s (IMSI_BEGIN, IMSI_END, IMSI_TYPE, SPONSOR_CODE, ZONE_CODE, EFF_DATE, EXP_DATE, ISMI_ID) VALUES(:P0,:P1,:P2,:P3,:P4,:P5,:P6,:P7)" , sTable);
        		m_poImsiOut->Prepare(sSQL);
        		m_poImsiOut->Commit();
        	}
        	else
        	{
        		if (GET_CURSOR(m_poImsiIn, m_poTTConn, m_sConnTns, oExp) != 0) {
        		    Log::log(0, "SHMSqlMgr::init m_poImsiIn init failed!");
        		    return -1;    
        		}
        		sprintf(sSQL, "SELECT IMSI_BEGIN, IMSI_END, IMSI_TYPE, SPONSOR_CODE, ZONE_CODE, EFF_DATE, EXP_DATE, ISMI_ID FROM %s" , sTable);
        		m_poImsiIn->Prepare(sSQL);
        		m_poImsiIn->Commit();
        	}
        	break;
        case TABLE_MENDB_INFO:
        	if(isInsert)
        	{
        		return -1;
        	}
        	else
        	{
        		if (GET_CURSOR(m_poMenDb, m_poTTConn, m_sConnTns, oExp) != 0) {
        		    Log::log(0, "SHMSqlMgr::init m_poMenDb init failed!");
        		    return -1;    
        		}
        		sprintf(sSQL, "SELECT MEM_TABLE_NAME, TABLE_TYPE_ID FROM %s" , sTable);
        		m_poMenDb->Prepare(sSQL);
        		m_poMenDb->Commit();
        	}
        	break;
      }
    }/*
    catch(TTStatus st)
    {
        Log::log(0, "SHMSqlMgr::init st.err_msg = %s", st.err_msg);
        return -1;
    }*/
    return 0;
}
