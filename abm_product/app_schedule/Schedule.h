
#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "StdAfx.h"
#include "MBC.h"
//#include "MBC_jt.h"
#include "SysParamDefine.h"



class Schedule
{
	public:
		
		Schedule();
		
		~Schedule();
		
	public:

		int isTimeOut();															//开始下一轮调度

		int refresh();																//刷新配置

		int modifySysState(char* state);						//修改系统状态SYSTEM.state 
		
		int modifySysRunState(char* state);						//检测修改系统运行状态
		
		int checkSysRunStateEx();											//系统运行状态检测
				
		int registerToInitEx();												//向initserver发起注册

		int getCurVirProcessor();											//获取系统当前状态下的虚处理器
		
		int testwhile();
		
		int runEx();																	//循环调度

		int getSysConfigEx();													//读取系统配置
				
		char* parserState();													//根据传进来的中间态解析出目标态
		//char* parserState(char* sMidState,char* sState);
		
		int doDynamicScheduleEx();										//做高低水
		
		int stateTransitionEx();											//响应状态变迁
		
		int checkPoint();															//响应checkpoint
		
		int modifyCheckPointState(int iState);									//修改checkpoint的状态
		
		int doBackUp();																//响应备份态
		
		int moveBackData();														//把备份态的临时表数据挪回来
		
		bool GetMemInfo(); //得到内存信息
		
		bool GetCpuInfo(); //得到cpu信息
		
	private:
		
		int m_iWaitTime;															//采样时间
		
		int m_iShift;																	//档位信息
		
		int m_iAutoCtlFlag;														//系统是否启用高低水自动控制
		
		char m_sSysState[15];													//系统状态
		
		char m_sMidSysState[50];											//系统中间态 0|ONLINE|OFFLINE
				
		int m_iStartTime;															//启动停止超时阀值
		
		vector<VirtualProcessor*> m_vVirtual;					//该状态下涉及的虚处理器
		
		int m_iCpuHighValue;													//CPU门限
		int m_iMemHighValue;													//MEM门限
		int m_iStateTimeOut;													//系统状态变迁超时判断
		int m_iCheckPointState;												//checkPoint状态
		int m_iMemPercent;                            //内存使用率
		int m_iCpuPercent;                            //cpu使用率
		
		
	public:
		
		char m_sRunState[30];													//SYSTEM.run_state
		
		vector<SysParamHis> m_vtSysParamHis;				//预约的更改
};
#endif
