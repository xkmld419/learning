/*VER: 1*/ 
#include  "LoadThreadMgr.h"

LoadThreadManager* LoadThreadManager::_instance = NULL;

LoadThreadManager::LoadThreadManager()
{
	_threadMap.clear();
}

LoadThreadManager::~LoadThreadManager()
{
	vector<UserInfoAdmin *>::iterator  iter;
	for (iter =_threadMap.begin();iter !=_threadMap.end() ;iter++ )
	{
		delete (*iter);
	}
	_threadMap.clear();
}

LoadThreadManager* LoadThreadManager::getInstance()
{
	if (_instance==NULL)
	{
		_instance = new LoadThreadManager;
	}
	return _instance;
}

void LoadThreadManager::add(UserInfoAdmin* pTreadInfo)
{
	_threadMap.push_back(pTreadInfo);
}

void LoadThreadManager::free()
{
	_threadMap2.clear();
	vector<UserInfoAdmin *>::iterator  iter;
	for (iter =_threadMap.begin();iter !=_threadMap.end();iter++)
	{
		UserInfoAdmin * tmp_thread = (*iter);
		if (tmp_thread->isSuccess)
		{
			delete tmp_thread;			
		}else
		{
			_threadMap2.push_back(tmp_thread);
		}
	}
	_threadMap.clear();	
	for (iter =_threadMap2.begin();iter !=_threadMap2.end();iter++)
	{
		UserInfoAdmin * tmp_thread2 = (*iter);
		_threadMap.push_back(tmp_thread2);
	}
	_threadMap2.clear();
}
