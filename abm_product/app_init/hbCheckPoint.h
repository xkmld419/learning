#ifndef	_HB_CHECKPOINT_H_
#define	_HB_CHECKPOINT_H_

//#include "CheckPointMgr.h"
//#include "CheckPointInfo.h"
#include "ProcessControler.h"

class hbCheckPoint
{
public:
	hbCheckPoint();
	~hbCheckPoint();
	bool Init(void);

	int CreateChkPoint(void);
	//检查是否可以落地内存
	bool CheckCanDownShm(void);
	//hss
	bool CheckCanDownShm(string strAppName,int time=30);
	bool StartProc(string strAppName);
	
//停止所有的数据源
	bool StopAllProc(const string&,int iSec=0);

	bool GetAllProc(const string& strAppName,vector<int>&);
	
	bool StartAllProc(void);

	bool CheckAllMsg(int iSec=0);

private:
	HBProcessControl* m_pProcessControler;

	vector<int>m_vecProcID;

};



#endif
