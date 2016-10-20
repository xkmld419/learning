#ifndef CMDSENDER_H_HEADER_INCLUDED_B4733C21
#define CMDSENDER_H_HEADER_INCLUDED_B4733C21
#include "CmdMsgQueue.h"
#include "CSemaphore.h"
#include "IpcKey.h"
#include <iostream>
using namespace std;
class CmdMsg;



//##ModelId=4B8CD83903B8
class CmdSender : public CmdMsgQueue
{
friend class CmdSendRecv;
  public:
   bool SendCmd(CmdMsg* pCmdMsg,long lType,bool bSendLock=false); 
	//##ModelId=4B8CDFC8032B
    bool SendCmd(CmdMsg* pCmdMsg,bool bSendLock=false);
    //##ModelId=4B8DC56A0186
    CmdSender();

  
    //##ModelId=4B8DC57103E6
    ~CmdSender();

  protected:
    //##ModelId=4B8CEA780102
    bool SendData(void* pData,unsigned int iSize,long lType=0);

private:
	CSemaphore* m_pLock;

};



#endif /* CMDSENDER_H_HEADER_INCLUDED_B4733C21 */
