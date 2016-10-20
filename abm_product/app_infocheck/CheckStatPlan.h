#ifndef  __CHECK_STAT_PLAN_H_HEADER_INCLUDE_201009
#define  __CHECK_STAT_PLAN_H_HEADER_INCLUDE_201009


#define _JUST_DBLINK_

#include <time.h>
#include <unistd.h>
#include <vector>
#include <iostream>
using namespace std;

#include "Log.h"
#include "Environment.h"
#include "ParamDefineMgr.h"

#include "CheckStatInfo.h"
//#include "CheckDailyAcctItemAggr.h"
#include "CheckInformation.h"
/*
#include "CheckStatFile.h"
#include "CheckProcess.h"
#include "StatAcctItemAccu.h"

#include "StatCallEventOffer.h"

#ifndef DEF_JIANGSU
#include "CheckBillRecord.h"
#endif

#include "CheckGatherFile.h"
#include "CheckStatEvent.h"
#include "CheckFileInStore.h"
#include "StatUserInfo.h"
#include "CheckSharedMemory.h"
#include "CheckHighFee.h"
#include "CheckKpi.h"
#include "CheckTransLog.h"
#include "CheckTicketLog.h"
*/

class CheckStatPlan {
  private:
    static PlanItem * m_pPlan;
        
    static TOCIQuery *m_pLockQry;
      
  public:    
    static char ** m_argv;

    CheckStatPlan ()  {m_pPlan = NULL; m_pLockQry = NULL;}
    ~CheckStatPlan () { 
        if (m_pPlan) {
            delete m_pPlan; 
            m_pPlan = NULL;
        }
        if (m_pLockQry) {
            delete m_pLockQry;
            m_pLockQry = NULL;
        }
    }
    
    //## 打开
    static void openPlan ();
    //## 获取
    static bool getPlan (PlanItem &oPlan);    
    //## 关闭
    static void closePlan ();
    
    //## ture:需要立即执行, false:不执行
    static bool analyzePlan (PlanItem &oPlan);    
    
    static bool checkRun (int iPlanID);
    
    // 锁定计划表
    static void lockPlan ();
    // 解锁
    static void unlockPlan ();
    
    //成功返回true
    static bool updateState (const int iPlanID, const char *sState, 
                const char *sMsg, const bool bLock=false);
        
    static void startPlan (PlanItem &oPlan, char ** argv);
    
    static void setNextRunTime (PlanItem &oPlan);
    
    static void waitDebug (PlanItem &oPlan);
    
    static void parsePlan (PlanItem *p);
    
    static pid_t regMainProcess (pid_t &PID)
    {
        DEFINE_QUERY (qry);
        //检查有无 plan_id=0 的记录, 无, insert
        qry.setSQL ("select sys_pid, exec_name, state from B_Check_Stat_Plan "
            " where plan_id = 0 ");
        qry.open ();
        if (!qry.next ()) {
            insertMainProcPlan();
        }
        else if (!strcmp(qry.field("state").asString(),"RUN")) 
        {
            if (checkRun (0)) {
                PID = qry.field("sys_pid").asLong();
                return 0; //主进程在运行,返回0
            }
            else //前一次异常退出
                updateState (0, "ERR", "检测到主进程上次为异常退出");
        }
        qry.close();
        qry.setSQL ("update B_Check_Stat_Plan set open_state=1 where plan_id=0");
        qry.execute();
        qry.commit();
        
        return getpid();
    }
    
    static bool getQuitFlag ()
    {
        DEFINE_QUERY (qry);
        qry.setSQL ("select open_state from B_Check_Stat_Plan "
            " where plan_id = 0 ");
        qry.open();
        if (!qry.next()) {
            qry.close();
            return true;
        }
        
        if (qry.field(0).asInteger() == 0) {
            qry.close();
            return true;
        }
        qry.close();
        return false;
    }

    static void setQuitFlag ()
    {
        DEFINE_QUERY (qry);
        qry.setSQL ("update B_Check_Stat_Plan set open_state=0 where plan_id=0");
        qry.execute();
        qry.commit();
    }
    
    static void insertMainProcPlan()
    {
        DEFINE_QUERY (qry);
        qry.setSQL ("Insert into B_Check_Stat_Plan ( "
            " plan_id, item_desc, open_state, app_type, "
            " stat_name, interval_type, time_interval ) "
            " values (0,'主进程',1,0,  0,1,1) "
        );
        qry.execute();
        qry.commit();
    }
    
    static void trimExeName (char sExeName[])
    {
        char sTemp[256];
        strcpy (sTemp, sExeName);
        int iSel = -1;
        for (int iPos=0; iPos<strlen(sTemp); iPos++)
        {
            if (sTemp[iPos] == '/')
                iSel = iPos;
        }
        strcpy (sExeName, sTemp+iSel+1);        
    }
    
    static int getNextStatLogID ()
    {
        DEFINE_QUERY (qry);
        qry.setSQL ("select SEQ_CHECK_STAT_LOG.nextval from dual");
        qry.open();
        qry.next();
        return qry.field(0).asInteger();
    }
    
    static void getNextMonth (char sDate[], const char sTimeInterval[]);
    
    static void writeErrIntoInterface (int iPlanID, const char *sMsg);
    
    void checkRecvBalance();
};


#endif
