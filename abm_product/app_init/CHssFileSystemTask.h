#ifndef CHBFILESYSTEMTASK_H_HEADER_INCLUDED_B4066B6F
#define CHBFILESYSTEMTASK_H_HEADER_INCLUDED_B4066B6F
#include "HssTask.h"
#include <string.h>


#define MAX_DISK_NUM 50
#define MAX_STRING_LEN 512 

//##ModelId=4BF9E62701C1
class CHbFileSystemTask : public CHbTaskBase
{
  public:
    //##ModelId=4BF9E6AB0019
    CHbFileSystemTask();

    //##ModelId=4BF9E6AB007C
    ~CHbFileSystemTask();

    //##ModelId=4BFE32A901CF
	bool getTaskResult(CmdMsg * vt,long &lMsgID,variant& vtRet,int* piRetSize=0);
    //##ModelId=4BFE32A9028B
	bool Init(void);
};



#endif /* CHBFILESYSTEMTASK_H_HEADER_INCLUDED_B4066B6F */
