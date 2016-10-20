// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef ENVIRONMENT_H_HEADER_INCLUDED_BDBDC8E4
#define ENVIRONMENT_H_HEADER_INCLUDED_BDBDC8E4
#include "TOCIQuery.h"
#include "TSQLIteQuery.h"
//#include "ABM_SQL_MGR.h"

#ifndef _JUST_DBLINK_
//#include "EventSectionUnioner.h"
//#include "EventSection.h"
//#include "TransactionMgr.h"
//#include "ReferenceObjectMgr.h"

#include "platform.h"
#include "ReadIni.h"
#include "encode.h"
#endif

#ifdef DEF_LINUX
#include <getopt.h>
#endif


#include "platform.h"

#ifdef __SQLITE__
#define DEFINE_QUERY(x) SQLITEQuery x

#else

#define DEFINE_QUERY(x) TOCIQuery x (Environment::getDBConn())

#endif

/*
#ifndef _JUST_DBLINK_
#define G_PUSERINFO (Environment::m_poUserInfoInterface)
#define G_SERV (*(Environment::m_poServ))
#define G_PSERV (Environment::m_poServ)
#define G_PESU (Environment::m_poEventSectionUnioner)
#define G_PTRANSMGR (Environment::m_poTransMgr)
#define G_PAGGRMGR (Environment::m_poAggrMgr)
#define G_PROBJECTMGR (Environment::m_poReferenceObjectMgr)
#define G_PACCTITEMMGR (Environment::m_poAcctItemMgr)
#define G_PEVENTTYPEMGR (Environment::m_poEventTypeMgr)
#define G_PORGMGR (Environment::m_poOrgMgr)

extern EventSection * g_poEventSection;
#endif
*/
#define DB_CONNECTED (Environment::m_bConnect)

extern int g_argc;
extern char ** g_argv;

#ifdef DEF_AIX
int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
#else
#endif

//##ModelId=424BBCEA011D
//##Documentation
//## 运行环境
class Environment
{
  public:
    //## 由Process调用
    static void setDBLogin(char *name, char *pwd, char *str);

    //## 由各模块调用
    static TOCIDatabase *getDBConn(bool bReconnect = false);
    static void useUserInfo();
    static void commit();

   static void rollback();
   
    static void disconnect();

	static void initSQLStatement();
	static void Terminate();
	
/*#ifndef _JUST_DBLINK_
    
    static void useEventSectionUnion();
    
    static UserInfoInterface *m_poUserInfoInterface;
    
    static Serv *m_poServ;
    
    static EventSectionUnioner *m_poEventSectionUnioner;
    
    static TransactionMgr *m_poTransMgr;
    static AggrMgr *m_poAggrMgr;
    
    static ReferenceObjectMgr *m_poReferenceObjectMgr;

    static AcctItemMgr * m_poAcctItemMgr;

    static OrgMgr * m_poOrgMgr;

    static EventTypeMgr * m_poEventTypeMgr;
#endif
*/

    static bool m_bConnect;
    static void getConnectInfo(
            char * sIniHeader, char *sUserName, char *sPasswd, char *sConnStr);

  private:
    static char m_sDBUser[24];
    static char m_sDBPwd[24];
    static char m_sDBStr[32];
    static TOCIDatabase m_oDBConn;
    
    static bool m_bDefaultConn;

    static void getDefaultLogin();
};



#endif /* ENVIRONMENT_H_HEADER_INCLUDED_BDBDC8E4 */



