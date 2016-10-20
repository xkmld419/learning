#ifndef _SHM_SHOW_DATA_H_
#define _SHM_SHOW_DATA_H_
#include "SHMData.h"
#include "SHMData_A.h"
#include "SHMIntHashIndex.h"
#include "SHMIntHashIndex_A.h"
#include "SHMStringTreeIndex.h"
#include "ABMException.h"
#include "TimesTenAccess.h"
#include <vector.h>
#include "ThreeLogGroup.h"
#include "SHMLruData.h"
#include "LruAggrData.h"

//ThreeLogGroup LOG;

struct SHMINFO
{
	long m_lIpcKey;
	string m_strIpcName;	
	int m_iIpcType;
	int m_iTotal;
	int m_iUsed;
	int m_iState ;
	unsigned long m_lMemAddr;
	unsigned long m_lDate;
	long m_lSize;
	float m_fUsedRate;
	float m_fUnUsed;
	char m_sTableName[32];	
	char m_sState[16];
};

struct SEMINFO
{
	long m_lIpcKey;
	char m_sTableName[32];	
	string m_strIpcName;	
	int m_iState ; //0 ø…”√ ;1 …œÀ¯	
};

struct LRUINFO
{
	long m_lIpcKey ;
	int m_iExtSize ;
	int  m_iExtNum;
	int m_iLRUType ;
	string m_strIpcName ;
	string m_strFileName ;
} ;

class ShmMgr
{
 public:
   ShmMgr(){ m_poInfo = NULL; }
   ~ShmMgr(){
   	m_vSHMInfo.clear() ;
   	m_vSEMInfo.clear() ;   	
   	};
  	bool show();  
 public:
 	bool getIpcKey();
 	void printUsage();
  int  getStatMem();
  int  getStatMem(vector<SHMINFO> &vRet)  ;
  int  getStatSem();
 	void showStatMem();
 	void showTTMem();
  void showLogMem();
  void showTaleMem(char * sTableName ,char *sCondtion=0)  	; 	
  void showCountMem(char * sTableName ,char *sCondtion=0);
  void showSemStat(char * sTableName=NULL);
  void showSemReset(char * sTableName=NULL )	;	 
  void lockSemStat(char * sKey=0 );  
  void creatSem(char * sTableName=0 );
  void creatOne(char *sKey);
  int  showLRUMem();
   	
  int  connectTT()  ;
public:
	int g_argc ;
	char **g_argv ;	
  vector<SHMINFO> m_vSHMInfo; 
  vector<SEMINFO> m_vSEMInfo;
  vector<LRUINFO> m_vLRUInfo;
  TimesTenCMD *m_poInfo;	  
};

#endif
