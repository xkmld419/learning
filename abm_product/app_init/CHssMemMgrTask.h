#ifndef CHBMEMMGRTASK_H_HEADER_INCLUDED_B3FCE5A2
#define CHBMEMMGRTASK_H_HEADER_INCLUDED_B3FCE5A2
#include "HssTask.h"

//##ModelId=4C0311CF005F
class CHbMemMgrTask : public CHbTaskBase
{
  public:
    //##ModelId=4C0311EF02CB
    CHbMemMgrTask();

    //##ModelId=4C0311EF02F9
    virtual ~CHbMemMgrTask();

    //##ModelId=4C0311F603B9
    bool  getTaskResult(CmdMsg* vt,long& lMsgID,variant& vtRet,int* iRetSize=0);

    //##ModelId=4C03121403D7
    bool Init();

private:
	bool CreateShm(long lKey,unsigned long size);

	bool DestoryShm(long lKey);
};



#endif /* CHBMEMMGRTASK_H_HEADER_INCLUDED_B3FCE5A2 */
