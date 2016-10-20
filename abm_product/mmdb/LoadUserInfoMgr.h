/*VER: 3*/ 
#ifndef LOAD_USER_INFO_MGR_H
#define LOAD_USER_INFO_MGR_H

#include "UserInfoCtl.h"
#include "Log.h"
#include "Environment.h"
#include "LoadThreadMgr.h"

using namespace std;

class UserInfoAdmin;

class LoadUserInfoMgr
{
public:
	static int  thread_num;
	static bool bAcct;
	static int  thread_exception_flag;
	static bool bForce;


public:
	static LoadUserInfoMgr* getInstance();
	~LoadUserInfoMgr();

	void freeThread();
	void createThreadRun(int iMode,char *message,UserInfoAdmin * admin,int iFlag=0);
	void loadAll();
	void loadTable(const int iMode);
	void loadParallel();
	void loadSuper();
	void unLoad();

    void setForceFlag(bool i_boolF ){ bForce=i_boolF; };
    bool getForceFlag(){ return bForce; };

protected:
	LoadUserInfoMgr();

private:
	static LoadUserInfoMgr* uniqueInstance;
	LoadThreadManager * threadManger;
    
    #ifdef SHM_FILE_USERINFO
    SHMFileCtl * m_pSHMFile;
	#endif
};

#endif /* LOAD_USER_INFO_MGR_H */
