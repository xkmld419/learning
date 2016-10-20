/*VER: 1*/ 
#ifndef _E8_USER_H
#define _E8_USER_H

//#include "UserInfo.h"
#include "UserInfoCtl.h"
#include "DataEventMem.h"
#include <vector>

#define E8_USER_INFO_NUM	"E8_USER"
#define E8_EXP_TABLE		"E8_USER_TO_TABLE"
#define DATA_BUFFER			1
#define INDEX_BUFFER		2
#define COMMIT_BUFFER		2000	

using namespace std;

class E8UserCtl:public DataEventMemCtl
{
public:	
	E8UserCtl(int iGroup);
	~E8UserCtl();
	void create();
	void loadE8User();     //上载内存
	void unLoadE8User();   //卸载内存	
	//void checkMemUse();    //查看使用量	
	//void alarmMemUser();   //使用量告警		
	bool updateE8UserInfo(DataE8AccountInfo *pData,char *sAction);	
	bool getBillingCycleOffsetTime(int iOffset,char * strDate);
#ifdef  USERINFO_REUSE_MODE
	void  revokeE8User();//将Data_user已过期的资料占用内存释放
#endif		

private:	
	void linkUserName(DataE8AccountInfo &pData,int iPos);		
};


class E8UserMgr : public DataEventMemBase
{
public:
	E8UserMgr();	
	E8UserMgr(int iGroup);
	~E8UserMgr();
	//根据帐号获取信息
	bool getE8User(DataE8AccountInfo &E8Info,char *e8Account,char *sAreaCode,char *sStartTime,char cGetMode='N');
	void checkMemUse();    //查看使用量	
	void dispUserInfo(char *sUserName);
	void dispAll();
	void alarmMemUser(char *sRcvAlarmAccNbr);   //使用量告警
	void cmpMem_TabUserInfo(char *sEndDay,char *sRcvAlarmAccNbr);
	void expUserInfo(char *sEndDay,char *sExpTabName);
	void inTableCommit(char *sEndDay,char *sExpTabName);
private:
	//iMemMode ： 1－数据区使用情况，2－索引区使用情况
	int  percentOfMemUsed(int iMemMode);
	//void insMonitorInterface(char *sInfo,char *sRcvAlarmAccNbr);		
	vector<DataE8AccountInfo> vE8UserList;
	vector<int> vIntList;
};


#endif
