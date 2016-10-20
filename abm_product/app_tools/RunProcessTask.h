/*VER: 1*/

# ifndef RUN_PROCESS_TASK_H
# define RUN_PROCESS_TASK_H

#include "BillingCycleAdmin.h"
#include "Process.h"


class ProcessParam
{
	public:
		ProcessParam()
		{
			m_iTaskID = 0;
			m_iProcessID = 0;
			memset(m_sParamName,0,31);
			memset(m_sParamValue,0,101);
			m_iValueType = 0;
			m_poNextParam = NULL;
		}
		~ProcessParam(){}
	public:
		int m_iTaskID;
		int m_iProcessID;
		char m_sParamName[31];
		char m_sParamValue[101];
		int m_iValueType;
		ProcessParam *m_poNextParam;	
		
};

class TaskStepProcess
{
	public:
		TaskStepProcess()
		{
			m_iTaskProcessID = 0;
			m_iTaskID = 0;
			m_iProcessID = 0;
			m_iAppID = 0;
			m_iRunSeq = 0;
			//m_iAutoFlag = 0;
			m_iAppType = 1;
			memset(m_sExeName,0,41);
			memset(m_sSqlStatement,0,2001);
			memset(m_sExecPath,0,65);
			memset(m_sErrStr,0,4001);
			memset(m_sParameter,0,81);
			m_oFirstParam = NULL;
			m_oSibStepProc = NULL;
			
		}
		~TaskStepProcess(){}

		bool doExecute(int iBillingCycleID);
	public:
		int m_iTaskProcessID;
		int m_iTaskID;
		int m_iProcessID;
		int m_iAppID;
		int m_iRunSeq;
		//int m_iAutoFlag;
		char m_sExeName[41];
		char m_sExecPath[65];
		char m_sSqlStatement[2001];
		char m_sParameter[81];
		int m_iAppType;  //0:后台程序，1:pl/sql块，3.unix shell脚本
		char m_sErrStr[4001];
		TaskStepProcess *m_oSibStepProc;   //指向相同步骤的下个proc
		ProcessParam *m_oFirstParam; //指向第一个参数

		char *getErrStr(){return ( (char *)m_sErrStr);}
	private:
		void writeLogDetail(int iBillingCycleID, char * sNewState, char *sLogInfo,char cOpType);		
		bool executeSql(int iBillingCycleID);
		bool executeSh(int iBillingCycleID);
		void repalceParam(char *sSql,char *sLogSql,int iBillingCycleID);
};


class TaskStep
{
	public:
		TaskStep(int iTaskID,int iRunSeq)
		{
			m_iTaskID = iTaskID;
			m_iRunSeq = iRunSeq;
			m_poTaskNextStep = NULL;
			m_poStepFirstProc = NULL;
			//m_iSuccRunSeq = 0;
			//m_iMaxRunSeq = 0;
			m_iStepID = 0;
		}
		TaskStep(){}			
		~TaskStep(){}	
		
		bool runOneStep(int iBillingCycleID);		
		
	public:
		int m_iStepID;
		int m_iTaskID;		
		int m_iRunSeq;		
		TaskStepProcess*m_poStepFirstProc;
		TaskStep *m_poTaskNextStep;
	private:		
		void writeLog(int iBillingCycleID,char *sNewState,char cOpType);
		bool checkPreState(int iBillingCycleID);
		
};

class ProcessTask
{
	public:
		ProcessTask()
		{
			m_iTaskID = 0;
			memset(m_sTaskState,0,4);
		}
		ProcessTask(int iTaskID){m_iTaskID = iTaskID;}
		~ProcessTask(){}

		int m_iTaskID;
		char m_sTaskState[4];
		int runByStep(int iBillingCycleID);
		TaskStep *m_poFirstStep;
	private:
		int m_iSuccRunSeq;
		int m_iMaxRunSeq;			
		int checkState();	
		int 	getTaskRunInfo(int iBillingCycleID);
		int	checkAllowRun(int iBillingCycleID);
		TaskStep * getStepFirstProc(int iRunSeq);
};

class TaskInfo
{
	public:
		TaskInfo(){}
		TaskInfo(int iTaskID,int iBillingCycleID)
			{
				m_iTaskID = iTaskID;
				m_iBillingCycleID = iBillingCycleID;
				m_iTaskCnt = 0;
			}
		~TaskInfo(){}

		static bool m_bLoad;
		static ProcessTask *m_poTaskList;
		static TaskStep *m_poTaskStepList;
		static TaskStepProcess *m_poStepProcList;
		static ProcessParam *m_poProcParamList;
		static HashList<ProcessTask *> *m_poTaskIndex;
		//static HashList<TaskStep *> *m_poTaskStepIndex;		

		int run();    //这里暂时必须制定task_id执行
	private:
		int m_iBillingCycleID;
		int m_iTaskID;
		int m_iTaskCnt;
		void unload();
		void loadAll();			
};

#endif
