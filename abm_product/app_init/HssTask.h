#ifndef HBTASK_H_HEADER_INCLUDED_B40C09D9
#define HBTASK_H_HEADER_INCLUDED_B40C09D9
#include "Variant.h"
#include "CmdMsg.h"
#include "Log.h"

//##ModelId=4BF3D1C7026D
class CHbTaskBase
{
  public:
    //##ModelId=4BF3D25B007F
    CHbTaskBase();

    //##ModelId=4BF3D25B009A
    virtual ~CHbTaskBase();
    //##ModelId=4BF9E720032D
    virtual bool  getTaskResult(CmdMsg * vt,long& lMsgID,variant & vtRet,int* iRetSize);

    //##ModelId=4BFE32E001B5
	virtual bool Init(void);
	virtual void OnExit() {return;};
protected:
		variant m_vtRet ;
};



#endif /* HBTASK_H_HEADER_INCLUDED_B40C09D9 */
