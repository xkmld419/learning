#ifndef CMDMSGQUEUE_H_HEADER_INCLUDED_B4734679
#define CMDMSGQUEUE_H_HEADER_INCLUDED_B4734679

#include "MessageQueueEx.h"
#include "CmdMsg.h"
#include "MBC.h"
#include "Exception.h"


#define	MAX_CMD_MSG_LEN			208600
#define	MAX_CMD_MSG_NUM					5
#define	QUEUE_BLOCK_FLAG		1
#define	QUEUE_BLOCK					0x00000001			//阻塞
#define	QUEUE_NONBLOCK			0x00000002			//非阻塞

//##ModelId=4BD1589A00E2
class CmdMsgQueue
{
  public:
    //##ModelId=4BD1589C03E7
    bool Init(unsigned int iSysMQID,bool bBlock);


    //##ModelId=4BD1589D0005
    virtual ~CmdMsgQueue();
    //更改消息队列
    //##ModelId=4BF63906001E
    bool ChangeQueueID(int iQueueID);
	unsigned int GetMQKey(void)
	{
		return m_iMQKey;
	};

  protected:
    //##ModelId=4BD1589D0007
    CmdMsgQueue();
    //##ModelId=4BD1589C03E2
    MessageQueueEx* pQueue;
	unsigned int m_iMQKey;
	bool m_bBlock;
};



#endif /* CMDMSGQUEUE_H_HEADER_INCLUDED_B4734679 */
