#include "CheckStatPlan.h"
#include "Date.h"
#include "EventSection.h"

//HashList<TAreaCode> *CheckProcess::m_pProcAreaHash = 0;
char StatInterface::m_sDefaultAreaCode[10] = {0};
    
PlanItem *StatInterface::m_pPlan = 0x0; //## 指向当前正在执行的计划



/*
    命令行参数说明:
     无参数         ---- 启动主进程, 无限制循环    CheckStat
     参数是数字N    ---- 启动主进程, 循环N次退出   CheckStat N
     参数是字符d    ---- 停止主进程                CheckStat d
     参数是字符v|V  ---- 查看任务状态[V:只查看在运行的任务]
     CheckStat 进程的启、停操作已经在 mnt 脚本里添加控制参数:
         mnt -bstat: 启动系统监控信息点统计程序[CheckStat]
         mnt -dstat: 停止系统监控信息点统计程序[CheckStat d]
*/
int main (int argc, char ** argv)
{
    cout<<endl;
    
    char *p = getenv ("TIBS_HOME");
    char sLogFile[100];
   	//Log::init(MBC_APP_testinfo); 
    if (p)
        sprintf (sLogFile, "%s/log/CheckStat.log", p);
    else
        strcpy (sLogFile, "CheckStat.log");
    
    Log::setFileName (sLogFile);
    
    CheckStatPlan::m_argv = argv;
    int iTaskId =0 ;    
    if (argc >= 2) {
        iTaskId = atoi (argv[1]);
    }else {
    	cout<<"请输入测试任务ID"<<endl ;
    }
    
    /*
    pid_t pExistPID = 0;
    pid_t pMianPid = CheckStatPlan::regMainProcess (pExistPID);
        
    if (pMianPid == 0) 
    {        
        Log::log (0, "主进程(%s)已经存在一个运行实例 PID:%ld ", argv[0], pExistPID);
        return -1;
    }
    
    CheckStatPlan::updateState (0, "RUN", "主进程启动...");
    Log::log (0, "%s (主进程--%ld)开始运行...", argv[0], pMianPid);
    */
    PlanItem oPlan;   
    CheckStatPlan::openPlan ();        
    while (CheckStatPlan::getPlan (oPlan))
    {
    	if( oPlan.m_iPlanID != iTaskId )
    		continue ;
    		
    // if (!CheckStatPlan::analyzePlan (oPlan) )
    //      continue;
          
      CheckStatPlan::startPlan (oPlan, argv);
        break ;
    }
        
    
    CheckStatPlan::updateState (0, "END", "主进程已停止!");
    Log::log (0, "%s (主进程)退出运行!", argv[0]);
    
    return 0;
}


