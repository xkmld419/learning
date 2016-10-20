
#ifndef _VIRTUAL_PROCESSOR_H_
#define _VIRTUAL_PROCESSOR_H_

#include <iostream>
#include "StdAfx.h"
using namespace std;


#define START											0			//启进程
#define STOP											1			//停进程
#define NOACTION_STATE						2			//水位状态初变，不做动作
#define NOACTION_TIME							3			//时间阀值未到，不做动作
#define NOACTION_NOMAL						4			//正常水位，不做动作
#define NOACTION_NEXT							5			//下个通道也是高水，不做动作

//虚处理器在配置系统状态下对应的状态
#define MODULE_STOP								0			//停止					0作为停看起来正常
#define MODULE_RUNING							1			//正常，有业务处理  1作为起 看起来正常
#define MODULE_MAINTAINANCE 			2			//维保

#define MODULE_ABORT							3			//异常
#define MODULE_FULL								4			//满载
#define MODULE_IDLE								5			//空闲，无业务处理
#define MODULE_PROC_RUN_ABORT					6			//有进程异常,同时有进程运行
#define MODULE_PROC_STOP_ABORT					7			//有进程异常,无进程在运行

#define MAX_OWN_APP_NUM						100		//一个虚处理器下最大的进程数

class Channel;						//通道

//进程结构
struct ProcessInfo{
	int iPos;								//在GuardMain中的索引位置
	int iOpFlag;						//操作过的痕迹 0未操作过 1操作过
	int iDestState;					//目标状态
	int iCheckCnt;					//到达目标态后的确认次数
	ProcessInfo(){
		iPos = -1;
		iOpFlag = 0;
		iDestState = -1;
		iCheckCnt = 0;
	}
};
//虚处理器结构
class VirtualProcessor
{
	public:
		VirtualProcessor(int ID);
		~VirtualProcessor();	
	public:
		int getVirtualState(int& state);										//获取虚处理器当前状态
		int getAllProcDetailNum(int& boot,int& init,int& run,int& kill,int& destory,int& abort,int& sleep); //获取所有状态下进程个数
		int refreshEx();																		//重新读取配置
		int getProcInfoEx();
		int getVirtualActionEx();														//获取需处理器应做的动作
		int isDataSrc();																		//判断该虚处理器是否是数据源
		int getProcInfoFromGuardMain();											//获取虚处理下的进程
		int startOneProcEx();																//启动某个进程，不做成功失败判断
		int stopOneProcEx();																//停止某个进程，不做成功失败的判断
		int checkProcState();																//检测虚处理器下进程状态是否达到目标态
		int getOneBusyProcEx();															// 获取一个正在运行的进程，用于停止
		int getOneIdleProcEx();															// 获取一个已经终止的进程，用于启动
		int checkVirtualState();														//检测虚处理器，响应高低水，和参数变更
		int getBusyProcNumEx();															//返回ST_INIT ST_WAIT_BOOT ST_RUNNING的进程个数，同时对ST_INIT ST_WAIT_BOOT的进程打上待检测标记
		int startMinProcEx();																//开启最小化配置
		int firstCheckProc();																//第一次检测，如果是要起就直接起，要停得如果是数据源头这则直接停并打上标识，否则先做标记
		int stopAllSrcProc();																//直接停数据源头
		int stopLeftProc();																	//把剩下的进程该停的停掉
		int checkTask();																		//检测虚处理器通道的水位，置m_iCanStop标记
		int forceStopProc();																//强制停止虚处理器中进程，不检测水位及状态
		int isErrExist();																		//查看虚处理器中是否还有进程状态不正常
		int isBusy();																				//查看是否超负荷
		int isBackUpSrc();																	//是否是备份态源
		int checkBackUpTask();															//备份态时，下游水位特殊处理
	public:
		int m_iVirtualId;																		//虚处理器ID
		int m_iVirtualState;																//虚处理器的当前状态
		int m_iDestState;																		//当前系统状态下虚处理器的目标状态
		int m_iCheckFlag;																		//用于判断有没有必要检测
		int m_iCanStop;																			//进程都可以停了
		int m_iSrcStopCheckFlag;														//本虚处理器是否是数据源头
		int m_iForceStop;																		//强杀的标志
		int m_iBusyFlag;																		//该虚处理器的超负荷标记
		int m_iAutoSchedule;																//二级自动调度开关，如果该值为0则不会参与高低水调度
		int m_iBackUpSrc;																		//备份态源
	private:
		int m_iMinProc; 																		//最少进程数
		int m_iMaxProc;																			//最大进程数
		int m_iMaxShiftProc;																//档位对应的最大进程数
		int m_iDurTimeOut;																			//状态持续时间阀值
		//Date m_dLastTime;																		//上次状态时间
		time_t m_iLastTime;																	//上次状态时间
		int m_iChLastState;																	//虚处理器对应通道的上一次的水位状态
		int m_iCnt;																					//虚处理器下进程的个数
		int m_iAbort;																				//abort态进程是否重启的开关
		int m_iShift;																				//系统档位
		int m_iIfDelete;                                    //当最大进程数变小时，是否根据变化删除进程数
		int m_iIfInsert;                                    //当最小，最大进程数变大时，是否根据变化增加进程数
		
		int m_iSysMaxProcCnt;																	//系统允许的最大进程个数
		int m_iBusyTimeOut;																			//系统超负荷时间阀值
		ProcessInfo m_ProcInfoList[MAX_OWN_APP_NUM];				//虚处理器拥有的进程
	public:
		static int g_RunProcNum;														//当前已经在运行的全局进程个数
};
#endif
