#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "Log.h"
#include "interrupt.h"
#include "Environment.h"
#include "ThreeLogGroup.h"
#include "CommandCom.h"
//#include "../app_tools/CheckPointMgr.h"
#include "interrupt.h"


class LogGroupProcess
{
public:
    LogGroupProcess ();
    ~LogGroupProcess ();

public:
    int run();
    int init();
private:
    void GetSysParamInfo();

    ThreeLogGroup* m_poMgr;
    CommandCom *m_pCommandCom ;
    int m_interval;
    //CheckPointMgr *m_pCheckPointMgr;
};


LogGroupProcess::LogGroupProcess ():m_interval(0),m_poMgr(0)
{
    m_poMgr = new ThreeLogGroup();
    m_pCommandCom = new CommandCom();
    //m_pCheckPointMgr = new CheckPointMgr();

}

LogGroupProcess::~LogGroupProcess()
{
    if(m_poMgr)
    {
	    delete m_poMgr;
        m_poMgr = 0;
    }
    
    if(m_pCommandCom)
    {
	    delete m_pCommandCom;
        m_pCommandCom = 0;
    }    
}

void LogGroupProcess::GetSysParamInfo()
{
	  static bool bIntervalFlag = true;
	  if(bIntervalFlag){
      m_interval =m_pCommandCom->readIniInteger("LOG","interval",0);
      bIntervalFlag = false;
  }
    int temMode = m_pCommandCom->readIniInteger("LOG","log_group_mode",-1);
    if(temMode<0)
    {
        Log::log(0,"核心参数更新---修改超过逻辑范围,请重新修改");
    }
    m_poMgr->SetGroupWriteMode(temMode);
    int iCheckPoint = m_pCommandCom->readIniInteger("LOG","log_percent_load",75);
    m_poMgr->SetCheckPointTouch(iCheckPoint);
    
    int ilevel = m_pCommandCom->readIniInteger("LOG","level",1);
	static int iLastLevel = -1;
	if(ilevel != iLastLevel)
	{
	    m_poMgr->SetLogLevel(ilevel);
		iLastLevel = ilevel;
	}
	
	int iFileCheckPercent = m_pCommandCom->readIniInteger("LOG","file_sw_pc",90);
	static int iLastFileCheckPercent = 0;
	if(iFileCheckPercent != iLastFileCheckPercent)
	{
		m_poMgr->SetFileCheckPercent(iFileCheckPercent);
		iLastFileCheckPercent = iFileCheckPercent;
	}

	int iFileSizeThreshold = m_pCommandCom->readIniInteger("LOG","size",200);
	static int iLastFileSizeThreshold = 0;
	if(iFileSizeThreshold != iLastFileSizeThreshold)
	{
		m_poMgr->RefreshGroupMaxFileSize(iFileSizeThreshold);
		iLastFileSizeThreshold = iFileSizeThreshold;
	}

	//处理日志路径输出变化
	static bool bLogPathFlag = true;
	if(bLogPathFlag) {
		static const int iGroupNum = m_poMgr->GetGroupNum();
		for(int i = 1; i <= iGroupNum; ++i)
		{
			char sParam[16];
			char sPath[100];
			for(int j = 1; j <= 2; ++j)
			{
				memset(sPath,0,sizeof(sPath));
				sprintf(sParam,"%d.%d.path",i,j);
				if(m_pCommandCom->readIniString("log",sParam,sPath,NULL))
				{
					m_poMgr->RefreshLogFilePath(i,j,sPath);
				}
			}
		}
		bLogPathFlag = false; 
  }
	
    return;
}

int LogGroupProcess::init()
{
    if(!m_pCommandCom)
        return -1;
    if(!m_pCommandCom->InitClient())
    {
        Log::log(0,"核心参数初始化,请检查");
        return -1;
    }
    m_interval =m_pCommandCom->readIniInteger("LOG","interval",0);
    if(m_interval==0)
    {
        Log::log(0,"获取核心参数log.interval值错误，请检查");
        return -1;
    }
    return 0;
}

int LogGroupProcess::run()
{
    string errMsg;
    int i=1;

    if(!m_poMgr)
    {
        Log::log(0,"logremain 初始化失败");
        return -1;
    }
    if(init()!=0)
    {
        Log::log(0,"logremain init失败");
        return -1;
    }
    Log::log (0, "%s", "logremain run!");

	Date dBegin;
	Date dCur;

	DenyInterrupt();
   
	//while(1)
	while (!GetInterruptFlag())
    {
        GetSysParamInfo();  //刷新核心参数

		dCur.getCurDate();
		if(dCur.getDay() != dBegin.getDay())
		{
			dBegin = dCur;
			m_poMgr->UpdateLogFileArchiveSeq();
		}
	/*	
        if(!m_poMgr->CheckInfoCallBack())
        {
            Log::log(0,"清仓进程执行回收共享内存失败\n");
            return -1;
        }
        */
        if(!m_poMgr->CommitInfoFile())
        {
            Log::log(0,"清仓进程执行落地内存文件失败\n");
            return -1;
        }
        bool bCheck=false ;
        if(!m_poMgr->Check75CheckPoint(bCheck))
        {
            Log::log(0,"清仓进程执行checkpoint失败\n");
            return -1;
        }
        //if(bCheck) point.touchInfoDown();
        //if(bCheck)m_pCheckPointMgr->touchInfoDown();
        i = m_interval>0 ? m_interval : 1;
        sleep(i);
    }
    return 0;
}

int main(int argc,char **argv)
{
	LogGroupProcess logprocess;
	logprocess.run();

	return 0;
}


