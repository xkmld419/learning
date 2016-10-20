#include "CheckStatPlan.h"

#include "Date.h"
#include "EventSection.h"

#ifndef _check_stat_sleep
#define _check_stat_sleep  3 //s
#endif

//HashList<TAreaCode> *CheckProcess::m_pProcAreaHash = 0;
char StatInterface::m_sDefaultAreaCode[10] = {0};
    
PlanItem *StatInterface::m_pPlan = 0x0; //## 指向当前正在执行的计划

static void ShowCtl (char cMethod);

static void WritePlanInfo ();

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
    
    if (p)
        sprintf (sLogFile, "%s/log/CheckStat.log", p);
    else
        strcpy (sLogFile, "CheckStat.log");
    
    Log::setFileName (sLogFile);
    
    CheckStatPlan::m_argv = argv;
    
    int iOperation = 1; //1-启动; 2-停止
    
    int iRunTimes = -1;    
    if (argc >= 2) {
        iRunTimes = atoi (argv[1]);
        if (iRunTimes == 0) {
            iRunTimes = -1;
            if (!strcmp(argv[1],"d"))
                iOperation = 2;
            else if (!strcmp(argv[1],"v") || !strcmp(argv[1],"V")) {
                ShowCtl (argv[1][0]);
                exit (0);
            }
            else {
                cout << "参数不合法" << endl;
                return -1;
            }             
        }
    }
    
    pid_t pExistPID = 0;
    pid_t pMianPid = CheckStatPlan::regMainProcess (pExistPID);
    
    //## 当前是停止主进程操作 
    if (iOperation == 2) {
        if (pExistPID) {
            cout << "[Message] 结束主进程-CheckStat ..." << endl;
            CheckStatPlan::setQuitFlag ();
        }
        else
            cout << "[Warning] CheckStat-主进程不存在!" << endl;
        
        return 0;
    }
    
    if (pMianPid == 0) 
    {        
        Log::log (0, "主进程(%s)已经存在一个运行实例 PID:%ld ", argv[0], pExistPID);
        return -1;
    }
    
    CheckStatPlan::updateState (0, "RUN", "主进程启动...");
    Log::log (0, "%s (主进程--%ld)开始运行...", argv[0], pMianPid);
    
    while (iRunTimes && !CheckStatPlan::getQuitFlag ())
    {
        PlanItem oPlan;
        
        CheckStatPlan::openPlan ();
        
        while (CheckStatPlan::getPlan (oPlan))
        {
            if (!CheckStatPlan::analyzePlan (oPlan))
                continue;
            
            DB_LINK->disconnect();
            
            int iSon = fork();
            int iGrandSon;
            
            switch (iSon)
            {
                case 0:
                    iGrandSon = fork();
                    usleep (1000);
                    switch (iGrandSon) 
                    {
                        case 0:
                            usleep (1000);
                            DB_LINK->connect();
                            CheckStatPlan::startPlan (oPlan, argv);
                            Log::log (0, "+++ GrandSon process Success, PlanID=%d, PID:%d",oPlan.m_iPlanID, getpid());

							_exit(0);
                      
                        case -1:
                            Log::log (0, "GrandSon process fork err");
                            break;
                        default:
                            Log::log (0, "--- GrandSon process Created, PlanID=%d, PID:%d",oPlan.m_iPlanID, iGrandSon);
                            break;
                    }

					_exit(0);
                  
                case -1:
                    Log::log (0, "Son process fork err");
                    break;
                default:
                    Log::log (99, "=> Son process Created, pid:%d", iSon);
                    waitpid (iSon, NULL, 0);
                    break;
            }
                        
            DB_LINK->connect();
        }
        
        CheckStatPlan::closePlan ();
        
        if (iRunTimes > 0)
            iRunTimes --;
        
        WritePlanInfo ();
        
        sleep (_check_stat_sleep);
    }
    
    CheckStatPlan::updateState (0, "END", "主进程已停止!");
    cout<<endl;
    Log::log (0, "%s (主进程)退出运行!", argv[0]);
    
    return 0;
}


static int getPlanInfo (int iPID, char *sMem, char *sCpu)
{
    char sCommand[256];
    FILE *fp;
    int ret;

    strcpy (sMem, "0");
    strcpy (sCpu, "0");    
    
#ifndef _linux_
    sprintf (sCommand, "UNIX95= ps -p %d -o pcpu -o vsz |grep -v ""CPU"" |awk '{print $1, $2}'", iPID);
    fp = popen (sCommand, "r");
    if (fp == NULL)
        return -1;

    while(1){
        if (feof(fp)) break;
        if (fscanf (fp, "%s %s\n", sCpu, sMem) == EOF)
            ret = -1;
        else
            ret = 0;
    }
#else
    sprintf (sCommand, "ps -p %d -o '%%C %%z' |grep -v ""CPU"" |awk '{print $1, $2}'", iPID);

    fp = popen (sCommand, "r");
    if (fp == NULL)
        return -1;

    if (fscanf (fp, "%s %s\n", sCpu, sMem) == EOF)
        ret = -1;
    else
        ret = 0;
#endif

	int iCpu = atoi(sCpu);
	int iMem = atoi(sMem)/1024;
    memset( sCpu,0,strlen(sCpu) );
	memset( sMem,0,strlen(sMem) );
	sprintf( sCpu,"%d",iCpu );
	sprintf( sMem,"%d",iMem );
    
    pclose (fp);
    return ret;
}

class _TPlanInfo {
  public:
    int   m_iPlanID;
    char  m_sItemDesc[61];
    char  m_sState[6];
    char  m_sStateDate[15];
    int   m_iSysPid;
    char  m_sMem[10];
    char  m_sCpu[4];
};

#define __MAX_PLAN_NUM 500
static _TPlanInfo s_oPlanInfo[__MAX_PLAN_NUM];
static long s_lTimes = 0;
static long s_lLines = 0;

//刷新计划信息
static void FreshPlan (int iWriteFlag = 0)
{
    memset (&s_oPlanInfo, 0, sizeof(_TPlanInfo) * __MAX_PLAN_NUM);

    DEFINE_QUERY (qry);
    qry.setSQL ("select plan_id, trim(substr(item_desc,1,30)) item_desc,"
        " nvl(state,'END') state, nvl(sys_pid,0) sys_pid ,"
        " to_char(state_date,'MM/DD hh24:mi:ss') state_date "
        " from b_check_stat_plan "
        " where plan_id >= 0 order by plan_id");
    qry.open ();
    int iPos = 0;
    
    while (qry.next())
    {
        s_oPlanInfo[iPos].m_iPlanID = qry.field("plan_id").asInteger();
        strcpy (s_oPlanInfo[iPos].m_sItemDesc, qry.field("item_desc").asString());
        strcpy (s_oPlanInfo[iPos].m_sState, qry.field("state").asString());
        s_oPlanInfo[iPos].m_iSysPid = qry.field ("sys_pid").asInteger();
        strcpy (s_oPlanInfo[iPos].m_sStateDate, qry.field ("state_date").asString());
        
        if (strcmp (s_oPlanInfo[iPos].m_sState, "RUN")) {
            s_oPlanInfo[iPos].m_iSysPid = 0;
            s_oPlanInfo[iPos].m_sMem[0] = '0';
            s_oPlanInfo[iPos].m_sCpu[0] = '0';
        }
        else {
            int iRet = getPlanInfo (s_oPlanInfo[iPos].m_iSysPid,
                        s_oPlanInfo[iPos].m_sMem, s_oPlanInfo[iPos].m_sCpu);
            
            if (iWriteFlag && iRet==0 && s_oPlanInfo[iPos].m_iPlanID!=0) 
            {//## 将当前进程的资源占用情况写入接口表 Stat_interface
                //MNT_SELF_CAPA_01: 任务ID;采样时间;内存占用(M);cpu占用率
                StatInterface oIt ("MNT_SELF_CAPA_01");
                Date d;
                sprintf (oIt.sDimensions, "%d;%s;%s;%s", 
                        s_oPlanInfo[iPos].m_iPlanID, 
                        d.toString(),
                        s_oPlanInfo[iPos].m_sMem,
                        s_oPlanInfo[iPos].m_sCpu
                );
                oIt.insert (true);
            }                
        }
        
        if (!strcmp (s_oPlanInfo[iPos].m_sState, "ERR"))
            strcpy (s_oPlanInfo[iPos].m_sState, "error");
        
        iPos ++;
    }
    
    s_lLines = iPos;
}

//将计划的当前状态信息秀至屏幕
static void ShowPlan (char cMethod)
{
    system ("clear");
    cout << "\n==== BSN.网管监控信息点提取任务状态信息("<<++s_lTimes<<") ====\n\n";
    cout << "PlanID State   SYS_PID MEM(M) CPU(%) STATE_DATE      PlanDesc" << endl;
    char sMsg[1024];
    for (int iPos=0; iPos<s_lLines; iPos ++)
    {
        sprintf (sMsg, "%6d %-5s %9d %6s  %3s%%  %14s  %-60s",
            s_oPlanInfo[iPos].m_iPlanID,
            s_oPlanInfo[iPos].m_sState,
            s_oPlanInfo[iPos].m_iSysPid,
            s_oPlanInfo[iPos].m_sMem,
            s_oPlanInfo[iPos].m_sCpu,
            s_oPlanInfo[iPos].m_sStateDate,
            s_oPlanInfo[iPos].m_sItemDesc
        );
        
        if (cMethod=='V' && strcmp (s_oPlanInfo[iPos].m_sState, "RUN"))
            continue;
        
        cout << sMsg << endl;
    }
    cout << endl << "(Note: Ctrl+\\ 退出!)"<< endl << endl;
}

static void Quit (int isignal)
{
    exit(0);
}

static void ShowCtl (char cMethod)
{
    signal (SIGQUIT, Quit);
    signal (SIGINT, Quit);
    signal (SIGTERM, Quit);
    signal (SIGABRT, Quit);
    signal (SIGTSTP, Quit);
    
    memset (&s_oPlanInfo, 0, sizeof(_TPlanInfo) * __MAX_PLAN_NUM);
    
    while (true)
    {
        FreshPlan ();
        ShowPlan (cMethod);
        sleep (2);
    }
}

static void WritePlanInfo ()
{
    static int s_iCnt = 0;
    
    #ifndef WRITE_PLAN_INFO_TIME_LEN
    #define WRITE_PLAN_INFO_TIME_LEN  60 //s
    #endif
    if (++s_iCnt < WRITE_PLAN_INFO_TIME_LEN/_check_stat_sleep)
        return;
    
    s_iCnt = 0;
    
    //FreshPlan (1);
    FreshPlan (0); //modify for hss test 2011.06.28 缺表
}

/*
create table B_Check_Stat_Plan (
    plan_id       number(9)  not null,     -- pk  特殊说明:plan_id=0的记录用来标记主进程的运行状态
    item_desc     varchar2(1024),          -- 统计项的描述
    open_state    number(1)  not null,     -- 是否开放此统计项 0-不开放; 1-开放 (如改为0,主进程[plan_id=0]会立即退出)
    app_type      number(1)  not null,     -- 0-释放子进程直接进行统计; 1-释放子进程调用stat_name中的PLSQL块
    stat_name     VARCHAR2(4000) not null, -- 当app_type=0时,表示统计的内容ID,ID为系统定义,不可配置; 当app_type=1时,表示调用的PLSQL块
    interval_type number(1)  not null,     -- 1-表示time_interval是时间间隔,  2-表示time_interval是每月的固定时间
    time_interval varchar2(8)  not null,   -- 当表示时间间隔时,单位为秒(0表示只在next_run_time时统计一次 统计执行前,程序将next_run_time置为无穷大); 当表示每月固定时间时,时间格式为:'DDHH24MISS',表示每个月固定时间启动
    next_run_time date,           -- 下次统计时间 (由程序自动置)
    run_parameter varchar2(512),  -- 下次启动时的输入参数
    sucess_times  number(12) default 0 not null, -- 成功次数(执行成功时,子进程执行计数++)
    failed_times  number(12) default 0 not null, -- 失败次数(由主进程检测子进程是否异常退出,执行计数++)
    state         varchar2(3),    -- 状态 RUN-正在执行, END-执行完毕, ERR-执行出错, NUL-app_type/stat_name无效
    debug_flag    number(1)  default 0 not null, -- 为1时, 指定子进程运行时进入调试状态(即子进程生成时,立即进入sleep循环,等待调试)
    state_date    date  ,         -- 状态时间
    sys_pid       number(9),      -- 后台系统进程号PID
    exec_name     varchar2(256),  -- 进程名 (可以根据sys_pid、exec_name确定统计子进程是否存在)
    message       varchar2(4000)  -- 最新信息
);
alter table B_CHECK_STAT_PLAN
  add constraint pk_check_stat_plan primary key (PLAN_ID);


comment on column B_CHECK_STAT_PLAN.PLAN_ID
  is 'pk  特殊说明:plan_id=0的记录用来标记主进程的运行状态';
comment on column B_CHECK_STAT_PLAN.ITEM_DESC
  is '统计项的描述';
comment on column B_CHECK_STAT_PLAN.OPEN_STATE
  is '是否开放此统计项 0-不开放; 1-开放 (如改为0,主进程[plan_id=0]会立即退出)';
comment on column B_CHECK_STAT_PLAN.APP_TYPE
  is '0-释放子进程直接进行统计; 1-释放子进程调用stat_name中的PLSQL块';
comment on column B_CHECK_STAT_PLAN.STAT_NAME
  is '当app_type=0时,表示统计的内容ID,ID为系统定义,不可配置; 当app_type=1时,表示调用的PLSQL块';
comment on column B_CHECK_STAT_PLAN.INTERVAL_TYPE
  is '1-表示time_interval是时间间隔,  2-表示time_interval是每月的固定时间';
comment on column B_CHECK_STAT_PLAN.TIME_INTERVAL
  is '当表示时间间隔时,单位为秒(0表示只在next_run_time时统计一次 统计执行前,程序将next_run_time置为无穷大); 当表示每月固定时间时,时间格式为:''DDHH24MISS'',表示每个月固定时间启动';
comment on column B_CHECK_STAT_PLAN.NEXT_RUN_TIME
  is '下次启动统计的时间 (由程序自动置)';
comment on column B_CHECK_STAT_PLAN.run_parameter
  is '下次启动时的输入参数';
comment on column B_CHECK_STAT_PLAN.SUCESS_TIMES
  is '成功次数(执行成功时,子进程执行计数++)';
comment on column B_CHECK_STAT_PLAN.FAILED_TIMES
  is '失败次数(由主进程检测子进程是否异常退出,执行计数++)';
comment on column B_CHECK_STAT_PLAN.STATE
  is '状态 RUN-正在执行, END-执行完毕, ERR-执行出错, NUL-app_type/stat_name无效';
comment on column B_CHECK_STAT_PLAN.DEBUG_FLAG
  is '为1时, 指定子进程运行时进入调试状态(即子进程生成时,立即进入sleep循环,等待调试)';
comment on column B_CHECK_STAT_PLAN.STATE_DATE
  is '状态时间';
comment on column B_CHECK_STAT_PLAN.SYS_PID
  is '后台系统进程号PID';
comment on column B_CHECK_STAT_PLAN.EXEC_NAME
  is '进程名 (可以根据sys_pid、exec_name确定统计子进程是否存在)';
comment on column B_CHECK_STAT_PLAN.MESSAGE
  is '最新信息';
*/
