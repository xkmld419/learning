#ifndef __HSSMODE_H_
#define __HSSMODE_H_

#include "CommandCom.h"
//#include "../app_tools/CheckPointMgr.h"
#include "syscfg.h"
//add by jx 20100805
#define _SYS_STATE_ 1
#define _RUN_STATE_ 2
#define _NET_STATE_ 3
#define _HBMODE_SECTION "SYSTEM"

/*
#ifdef __IN_HB_MONITOR__
#define printf(format,args...)
fstream fout;
#define cout fout
#endif
*/

class TMode
{

	public:
		TMode();
		~TMode();
	public:
		int getNowSysState( int iStatType ,char *sNowStat);
		int getNowMidState( int iStatType ,char *sNowStat);
		int changeSysState( int iStatType, char *sOrgStat,char *sNewStat,int iFource = 0 ,int iFlag = 0);
		int changeSysState( int iStatType, char *sOrgStat,char *sNewStat,bool Ismonitor ,int iFource,int iFlag);
		int showSysState();
		int showHelp();
		int getProcessFault();
		ThreeLogGroup m_oLog;
	public:
		bool init(int iTimeOut,CommandCom *pCommandCom);
		int deal(char* sArgv[],int iArgc);
	private:
		//CheckPointMgr *m_pCheckPointMgr;
		static int m_iTimeOut;
		static bool m_bInitFlag;
		CommandCom *m_pCommandCom;
};

#endif //__HSSMODE_H_
