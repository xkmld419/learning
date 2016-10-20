#include "petri.h"
/*
extern "C" {

//ªÒ»°π≤œÌª∫≥Âµÿ÷∑
int GetShmAddress(TSHMCMA **ppTable,int iShmKey)
{
    int iShmID;
    
    //ªÒ»°π≤œÌƒ⁄¥ÊID
    if ((iShmID = shmget(iShmKey,0,0))<0){
        return -1;
    }

    //get the buff addr.
    *ppTable = (TSHMCMA *)shmat(iShmID,(void *)0,0);
    
    if (-1 == (long)(*ppTable)){
        return -2;
    }

    return 0;
}

} // End extern "C"
*/
void PETRI::_P_DB::getConnectInfo()
{
	static bool ifget =false;
	if (ifget) {
		return;
	}else {
		ifget = true;
	};
	ReadIni reader;
	char sFile[254];		
	char sTIBS_HOME[80];
	char sIniHeader[32];
	char * p;
	/* TIBS_HOME */
#ifdef HB_28
	if ((p=getenv ("BILLDIR")) == NULL)
		sprintf (sTIBS_HOME, "/opt/opthb");
	else 
		sprintf (sTIBS_HOME, "%s", p);
	
	if (sTIBS_HOME[strlen(sTIBS_HOME)-1] == '/') {
		sprintf (sFile, "%setc/billingconfig", sTIBS_HOME);
	} else {
		sprintf (sFile, "%s/etc/billingconfig", sTIBS_HOME);
	}
#else
	if ((p=getenv ("TIBS_HOME")) == NULL)
		sprintf (sTIBS_HOME, "/bill/TIBS_HOME");
	else 
		sprintf (sTIBS_HOME, "%s", p);
	
	if (sTIBS_HOME[strlen(sTIBS_HOME)-1] == '/') {
		sprintf (sFile, "%setc/data_access_new.ini", sTIBS_HOME);
	} else {
		sprintf (sFile, "%s/etc/data_access_new.ini", sTIBS_HOME);
	}
#endif
	
	strcpy(sIniHeader,"PETRI");

	reader.readIniString (sFile, sIniHeader, "username", s_user, "");
	
	char sTemp[254];		
	reader.readIniString (sFile, sIniHeader, "password", sTemp,"");
	decode(sTemp,s_passwd);
	
	reader.readIniString (sFile, sIniHeader, "connstr", s_constr, "");
	
	return;	
};

void PETRI::_P_DB:: setConnectInfo(char *puser,char *ppasswd,char *pconstr)
{
	memset(s_user,0,sizeof(s_user));
	memset(s_passwd,0,sizeof(s_passwd));
	memset(s_constr,0,sizeof(s_constr));

	strcpy(s_user,puser);
	strcpy(s_passwd,ppasswd);
	strcpy(s_constr,pconstr);	
}


TOCIDatabase* PETRI::_P_DB::getDB(bool reLogin, char *puser , char *ppasswd ,char *pconstr)
{
	if ( b_Connect && !reLogin) {
		return &db;
	};

	if (reLogin) {
		try {
			db.disconnect();
			b_Connect = false;
		} catch (TOCIException &toe) {
			LOG(toe.getErrMsg());
		} catch(...) {
		};

	};

	if ( puser && ppasswd && pconstr){
		setConnectInfo(puser,ppasswd,pconstr);
	}else{
		getConnectInfo();
	}

	try {
		b_Connect = db.connect(s_user,s_passwd,s_constr);
	} catch (TOCIException &toe) {
		LOG(toe.getErrMsg());
	} catch(...) {
	}

	if (!b_Connect)
		LOG("Connect to DB err\n");
	//if (!b_Connect) b_Connect = true;
	return &db;

};

/*
TOCIDatabase* PETRI::_P_DB::getDBchild(bool reLogin)
{
	if ( b_Connectchild && !reLogin) {
		return &dbchild;
	};
	getConnectInfo();
	//db.disconnect();
	b_Connectchild = dbchild.connect(s_user,s_passwd,s_constr);
	if (!b_Connectchild)
		printf("Connect to DB err\n");
	//if (!b_Connect) b_Connect = true;
	return &dbchild;

};
*/
/*
TSHMCMA * PETRI::_P_MNT::getShmTable()
{
    
	if (p_shmTable) return p_shmTable;

  long lShmKey = IpcKeyMgr::getIpcKey (-1,"SHM_WorkFlowKey");
  if (lShmKey < 0) {
		p_shmTable=NULL;
    return NULL;
  }
  
  //Ëé∑ÂèñÂÖ±‰∫´ÁºìÂÜ≤Âú∞ÂùÄ
  if (::GetShmAddress(&p_shmTable,lShmKey)<0 ){
    p_shmTable = NULL;
    return NULL;
  }

	return p_shmTable;
 
};
*/
/*
bool PETRI::_P_MNT::login()
{
 
    //lock the signal.
		if (!p_shmTable)
			getShmTable();
		bool bRet = false;
    LockSafe(p_shmTable->m_iMonitorLock,0);
    
    p_shmTable->MntReqInfo.m_iStaffID = 1;
    p_shmTable->MntReqInfo.m_iLogMode = LOG_AS_CONTROL;
    //P_RECVBUFF.getHostAddr("");
    //P_RECVBUFF.getOrgIDStr("1");
    
    p_shmTable->MntReqInfo.m_iStatus = MNT_REQ_REGISTER;

    int iTimeOut = 0;
   	while( p_shmTable->MntReqInfo.m_iStatus == MNT_REQ_REGISTER)
    {//Á≠âÂæÖ‰∏ªÂÆàÊä§ËøõÁ®ãÂ§ÑÁêÜ...
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) {  //TIME_OUT = 500
            p_shmTable->MntReqInfo.m_iStatus = WF_WAIT;
            UnlockSafe(p_shmTable->m_iMonitorLock,0);
						return bRet;
        }
    }

    //‰∏ªÂÆàÊä§ËøõÁ®ãÂ§ÑÁêÜÂêéÁöÑÁä∂ÊÄÅÂèØËÉΩÊúâ: REG_REJECT_MAXNUM,REG_ACCEPT
    if (p_shmTable->MntReqInfo.m_iStatus == REG_ACCEPT) {
        i_index = p_shmTable->MntReqInfo.m_iIndex;
        i_authID = p_shmTable->MntReqInfo.m_iAuthID;
				bRet = true;
    }

    //unlock signal.
    UnlockSafe(p_shmTable->m_iMonitorLock,0);
		return bRet;
};
*/
/*
bool PETRI::_P_MNT::logout()
{
 
    //lock the signal.
		bool bRet = false;
    LockSafe(p_shmTable->m_iMonitorLock,0);
    
		p_shmTable->MntReqInfo.m_iIndex = i_index;
		p_shmTable->MntReqInfo.m_iAuthID = i_authID;
    p_shmTable->MntReqInfo.m_iStatus = MNT_REQ_REGISTER;

    int iTimeOut = 0;
   	while( p_shmTable->MntReqInfo.m_iStatus == MNT_REQ_UNREGISTER)
    {
        usleep(USLEEP_LONG);
        if (iTimeOut++ > TIME_OUT) {  //TIME_OUT = 500
            p_shmTable->MntReqInfo.m_iStatus = WF_WAIT;
            UnlockSafe(p_shmTable->m_iMonitorLock,0);
						return bRet;
        }
    }
		bRet = true;


    //unlock signal.
    UnlockSafe(p_shmTable->m_iMonitorLock,0);
		return bRet;
};
*/
	
void PETRI::gotKilled(int sig) 
{
	LOG("got Killed and Exit\n");
	exit (_KILLED);
};

/*
bool PETRI::_P_MNT::KeepAlive()
{
	GetLogInfo();
	if ( i_index != -1 && i_authID != -1 ) {



	}

}
*/
/*

void PETRI::_P_MNT::GetLogInfo(bool reLogin)
{
	if (reLogin) {
		login();
		SaveLogInfo();
		return;
	}

	DEFINE_PETRI_QUERY(query);
	try {
		string sql = "SELECT DECODE(NAME,'LOG_ID',1,'AUTH_ID',2) VALUE_ID,NVL(TO_NUMBER(VALUE),-1) VALUE FROM P_PETRI_RUNNING_INFO WHERE NAME IN ('LOG_ID','AUTH_ID') ORDER BY VALUE_ID";
		query.close();
		query.setSQL(sql.c_str());
		query.open();
		if (query.next()) {
			i_index = query.field(1).asInteger();
		}
		if (query.next()) {
			i_authID = query.field(1).asInteger();
		}


	} catch(TOCIException& toe) {
		LOG(toe.getErrMsg());

	} catch (...) {

	}

	return;



}

void PETRI::_P_MNT::SaveLogInfo()
{

	DEFINE_PETRI_QUERY(query);
	try {
		string sql = "UPDATE P_PETRI_RUNNING_INFO SET VALUE=TO_CHAR(:V) WHERE NAME = :N";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("N","LOG_ID");
		query.setParameter("V",i_index);
		query.execute();

		query.setParameter("N","AUTH_ID");
		query.setParameter("V",i_authID);
		query.execute();



	} catch(TOCIException& toe) {
		LOG(toe.getErrMsg());

	} catch (...) {

	}

	return;

}
*/


