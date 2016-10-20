#ifndef CMDPARSERPOOLMGR_H_HEADER_INCLUDED_B40C5E9A
#define CMDPARSERPOOLMGR_H_HEADER_INCLUDED_B40C5E9A

#include "HssTask.h"
#include "CmdReceiver.h"
#include "CmdSender.h"
#include "CmdParserProc.h"
#include "Log.h"
#include "CmdMsg.h"
#include "InDependProcess.h"
#include "TOCIQuery.h"
#include "Environment.h"
#include "IpcKey.h"
#include "CSemaphore.h"
#include <vector>

using namespace std;

//CmdParser消息处理进程池
//##ModelId=4BF3C2300237
class CmdParserPoolMgr
{
  public:
    //##ModelId=4BF3CABD0020
    CmdParserPoolMgr();

    //##ModelId=4BF3CAC5034E
    virtual ~CmdParserPoolMgr();
    //初始化进程池
    //##ModelId=4BF3CF49003E
    bool Init(void );

	void addDefaultMsg(CmdMsg* pMsg);

    //将任务提交给进程池执行
    //##ModelId=4BF3CFA9032C
    void addTask(CmdMsg* pMsg);
    //##ModelId=4BF3D3D10290
    unsigned int const& getMaxProcessCnt() const;


    //##ModelId=4BF3D3D102ED
    void setMaxProcessCnt(unsigned int left);

  
    //##ModelId=4BF3D3D103C2
    unsigned int const& getMixProcessCnt() const;


    //##ModelId=4BF3D3D2001A
    void setMixProcessCnt(unsigned int left);
    //##ModelId=4BF5F26A0322
    void setMsgSender(
        //消息传送器用于将消息发送个执行进程
		CmdSender* pMsgSender);


    //##ModelId=4BF5F2A801D2
    void setMsgRecverCmdParser(
        //消息接收器,用户接受init的发送消息
        CmdReceiver* pMsgRecver);

	void setMsgSenderCmdParser(
		//消息传送器用于子进程执行完结果后向initserver汇报结果
		CmdSender* pMsgSender);


//public:
//	static CSemaphore* getPoolSem(void);

  private:
    //增加一个进程
    //##ModelId=4BF3D045033F
    void addProcess();

    //##ModelId=4BF3D0550312
    //减少一个进程
    //##ModelId=4BF3D0550312
    bool subProcess();

    //##ModelId=4BF3D30103D0
    vector<pid_t> m_vectorPid;
    //##ModelId=4BF3CEE9026A
    unsigned int m_iMaxProcessCnt;
    //##ModelId=4BF3CEFA0229
    unsigned int m_iMixProcessCnt;
    //##ModelId=4BF5F3E90308
    CmdSender* m_pMsgSender;
    
    //##ModelId=4BF5F3FE0226
    CmdReceiver* m_pMsgRecverCmdParser;

	CmdSender* m_pMsgSenderCmdParser;

	CSemaphore* m_pSemaphore;

	vector<CmdMsg* > m_vctDefaultMsg;
};



#endif /* CMDPARSERPOOLMGR_H_HEADER_INCLUDED_B40C5E9A */
