/*VER: 1*/ 
#ifndef _EXT_SERV_H_
#define _EXT_SERV_H_

//#include "UserInfo.h"
//#include "UserInfoCtl.h"
#include "DataEventMem.h"
#include "MBC.h"

#define EXT_SERV_DATA_NUM	"EXT_SERV_DATA_NUM"
#define EXT_SERV_INDEX_NUM	"EXT_SERV_INDEX_NUM"

#define DATA_BUFFER			1
#define INDEX_BUFFER		2
#define COMMIT_LEN			1000

struct EXT_SERV_CHANGE 
{	
	char sAreaCode[7];
	char sExtNbr[MAX_CALLED_NBR_LEN];
	char sExtType[4];
	char sState[4];
	char sEffDate[15];
	char sExpDate[15];	
    char sAreaName[31];
	char sRemark1[31];
	char sRemark2[101];
	char sRowID[21];
};

typedef struct EXT_SERV_CHANGE ExtServChange;

class ExtServCtl:public DataEventMemCtl
{
public:
	ExtServCtl(int iGroup);
	~ExtServCtl();
	void create();
	void loadExtServ();
	void unloadExtServ();
	void updateExtServ();
	bool getBillingCycleOffsetTime(int iOffset,char * strDate);	
#ifdef  USERINFO_REUSE_MODE
	void  revokeExtServ();//将b_ext_serv已过期的资料占用内存释放
#endif		
private:
	void linkExtNbr(ExtServ &poExtServ,unsigned int iPos);
	bool chk_upRec(ExtServChange &pServChange,TOCIQuery &qryIns,TOCIQuery &qryUpd);	
};

class ExtServMgr:public DataEventMemBase
{
public:
	ExtServMgr(int iGroup);
	~ExtServMgr();
	bool getAreaCodeByExtNbr(ExtServ *poExtServ,char *sAccNbr,char *sNbrType,char *sStartDate);	
	void checkMemUse();  
	void dispUserInfo(char *sExtNbr,char *sExtType);
	void dispAll();
	void alarmMemUser(char *sRcvAlarmAccNbr);   //使用量告警
	
private:
	int percentOfMemUsed(int iMemMode);	
};

#endif
