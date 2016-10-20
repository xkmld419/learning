/*VER: 1*/ 
#ifndef LOAD_THREAD_MGR_H
#define LOAD_THREAD_MGR_H

#include "UserInfoCtl.h"
#include <pthread.h>
#include <vector>

class UserInfoAdmin;

class LoadThreadManager
{
public :
	LoadThreadManager();
	~LoadThreadManager();

public :
	static LoadThreadManager* getInstance();
	void add(UserInfoAdmin * pThreadInfo);
	void free();

protected :
	static LoadThreadManager* _instance;

private :
	vector<UserInfoAdmin *> _threadMap;
	vector<UserInfoAdmin *> _threadMap2;
};


#endif
