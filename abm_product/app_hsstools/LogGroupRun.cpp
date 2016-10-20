#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "Log.h"
#include "Process.h"
#include "interrupt.h"
#include "Environment.h"
#include "ThreeLogGroup.h"
//#include "HbAdminMgr.h"

class LogGroupRun: public Process
{
public:
    LogGroupRun ();
    ~LogGroupRun ();

public:
    int run();
private:
    bool CheckShm();
    void GetSysParamInfo();

    ThreeLogGroupMgr* m_poMgr;
};

DEFINE_MAIN (LogGroupRun)

LogGroupRun::LogGroupRun ()
{
    m_poMgr = new ThreeLogGroupMgr();
    Log::init(MBC_APP_loggrum);
}

LogGroupRun::~LogGroupRun()
{
    if(m_poMgr)
    {
	    delete m_poMgr;
        m_poMgr = 0;
    }
}

bool LogGroupRun::CheckShm()
{
    int iSysID=0;
    iSysID = shmget (SHM_LOG_GROUP_DATA,0,0);
    if(iSysID<0)
        return false;
    return true;
}

void LogGroupRun::GetSysParamInfo()
{
    vector<SysParamHis> m_vtSysParamHis;

    Process::m_pCommandCom->FlushParam(&m_vtSysParamHis);

    for(int i=0;i<m_vtSysParamHis.size();i++)
    {
        if(!strcmp(m_vtSysParamHis[i].sSectionName,"LOG"))
        {
            if(!strcmp(m_vtSysParamHis[i].sParamName,"log_alarm_level"))
            {
                int temInfoLevl = atoi(m_vtSysParamHis[i].sParamValue);
                if(temInfoLevl<-1)
                {
                    Log::log(0,"核心参数更新---修改超过逻辑范围,请重新修改");
                    continue;
                }               
                int lastLevl = Log::m_iAlarmLevel;
                Log::m_iAlarmLevel = temInfoLevl;
                Log::log(0,"核心参数更新---告警信息点日志等级参数由%d级变更为%d级",lastLevl,temInfoLevl);    
            }
            else if(!strcmp(m_vtSysParamHis[i].sParamName,"log_alarmdb_level"))
            {
                int temInfoLevl = atoi(m_vtSysParamHis[i].sParamValue);
                if(temInfoLevl<-1)
                {
                    Log::log(0,"核心参数更新---修改超过逻辑范围,请重新修改");
                    continue;
                }               
                int lastLevl = Log::m_iAlarmDbLevel;
                Log::m_iAlarmDbLevel = temInfoLevl;
                Log::log(0,"核心参数更新---告警信息点入库等级参数由%d级变更为%d级",lastLevl,temInfoLevl);    
            }
            else if(!strcmp(m_vtSysParamHis[i].sParamName,"log.interval"))
            {
                int teminval = atoi(m_vtSysParamHis[i].sParamValue);
                if(teminval<0)
                {
                    Log::log(0,"核心参数更新---修改超过逻辑范围,请重新修改");
                    continue;                      
                }
                int lastinval = Log::m_interval;
                Log::m_interval = teminval;
                Log::log(0,"核心参数更新---信息点输出间隔参数由%d 秒变更为%d 秒",lastinval,teminval);
            }
            else if(!strcmp(m_vtSysParamHis[i].sParamName,"log_group_mode"))
            {
                int temMode = atoi(m_vtSysParamHis[i].sParamValue);
                if(temMode<0)
                {
                    Log::log(0,"核心参数更新---修改超过逻辑范围,请重新修改");
                    continue;                    
                }
                m_poMgr->SetGroupWriteMode(temMode);
                Log::log(0,"核心参数更新---日志组写数据方式参数变更为%d ",temMode);
            }
            else if(!strcmp(m_vtSysParamHis[i].sParamName,"log_group_checkpoint_size"))
            {
                int iTouchSize = atoi(m_vtSysParamHis[i].sParamValue);
                if(iTouchSize<=0 || iTouchSize>=100)
                {
                    Log::log(0,"核心参数更新---修改超过逻辑范围,请重新修改");
                    continue;                     
                }
                m_poMgr->SetGroupCheckTouch(iTouchSize);
                Log::log(0,"核心参数更新---checkpoint切换日志组阀值变更为%d ",iTouchSize);
            }
        }
    }
    m_vtSysParamHis.clear(); //清一下历史信息
    return;
}
int LogGroupRun::run()
{
    string errMsg;
    int i=1;
    //int bIsLogin =-1;

 	//HbAdminMgr * cHbLogin = new HbAdminMgr();
	//bIsLogin = HbAdminMgr::getAuthenticate();

//	if(bIsLogin==-1)
//	{
//		Log::log(0, "请先登录认证!");
//		return 0;
//	}
//	
//	if(bIsLogin==0)
//	{
//		Log::log(0, "未登录认证,请先运行登录认证程序!");
//		return 0;
//	}


    Log::log (0, "%s", "hbloggrun run!");
    while(1)
    {
        if(!CheckShm())
        {
            Log::log(0,"关联共享内存失败，请检查是否创建");
            return -1;
        }
        GetSysParamInfo();
        if(m_poMgr->GetGroupWriteMode()!=WRITESHARE)
            continue;
        if(!m_poMgr->exist())
        {
            Log::log(0,"请检查日志组共享内存是否创建\n");
            return -1;
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
        if(!m_poMgr->Check75CheckPoint())
        {
            Log::log(0,"清仓进程执行checkpoint失败\n");
            return -1;                
        }
        i = Log::m_interval>0 ? Log::m_interval : 1;
        sleep(i);
    }
    return 0;
}

