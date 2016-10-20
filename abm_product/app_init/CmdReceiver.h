#ifndef CMDRECEIVER_H_HEADER_INCLUDED_B4732080
#define CMDRECEIVER_H_HEADER_INCLUDED_B4732080
#include "CmdMsgQueue.h"
#include "MBC.h"
#include <iostream>
using namespace std;
class CmdMsg;

//##ModelId=4B8CB9DD005B
class CmdReceiver : public CmdMsgQueue
{
  public:
    //##ModelId=4B8CDB250003
    bool RecvCmd(CmdMsg* pCmdMsg,long lType=0);
    //##ModelId=4B8DC54A0263
    CmdReceiver();

  
    //##ModelId=4B8DC55B00AB
    ~CmdReceiver();

  protected:
    //##ModelId=4B8CEA6B0114
    bool RecvData(void** pData,long lType=0);

  private:
	  char* m_pBuf;
	  int m_iMqMaxSize;

};



#endif /* CMDRECEIVER_H_HEADER_INCLUDED_B4732080 */
