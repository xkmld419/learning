#ifndef _LIB_THREE_LOG_MGR_H_
#define _LIB_THREE_LOG_MGR_H_

#include "ThreeLogGroupBase.h"
#include "Env.h"

class ThreeLogGroupMgr : public ThreeLogGroupBase
{
 public:
    ThreeLogGroupMgr();
    ~ThreeLogGroupMgr();
 public:
    bool create();
    bool exist();
    bool remove();
    bool setGroupNum(int iNum);
    bool setGroupInfoDataCount(long lCount);

    int  GetShmDataTotalCount(int iGroupId);
    int  GetShmIndexTotalCount(int iGroupId);

    int  GetShmDataCount(int iGroupId);
    int  GetShmIndexCount(int iGroupId);
    int  GetShmIndexDataCount(int iGroupId,int iIndex);
    int  GetShmIndexDataTotal(int iGroupId);
	void showShmInfo();

	int getLogGroupNum();

	SHMData_A<LogInfoData> *getLogGroupDataAddr(int iGroupId);
	SHMIntHashIndex_A *getLogGroupIndexAddr (int iGroupId);

    bool init();
 private:    
    int m_iMaxGroup;                    //日志组块数
    long m_lInfoDataCount;              //数据区数量
};

#endif //_LIB_THREE_LOG_MGR_H_
