/******************************
description:
author: xn
version: 1.0
date: 2010-04-28
******************************/
/*状态对应表
	"SLP",               #define ST_SLEEP       0
	"INIT", //准备启动   #define ST_INIT        1
	"RUN",               #define ST_RUNNING     2
	"BOOT",              #define ST_WAIT_BOOT   3
	"DOWN",              #define ST_WAIT_DOWN   4
	"KILL",              #define ST_KILLED      5
	"ABRT",              #define ST_ABORT       6
	"Down"               #define ST_DESTROY     7
*/
#ifndef _Channel_H_
#define _Channel_H_
#include "CommandCom.h"
#include "StdAfx.h"
//#include "MBC_jt.h"
#include "MBC.h"
#include "ThreeLogGroup.h"

#define CHANNEL_EMPTY 0			//空
#define CHANNEL_NOMAL 1			//正常
#define CHANNEL_LOW   2			//低水
#define CHANNEL_HIGH  3			//高水
#define CHANNEL_FULL  4			//满

#define MAX_STATE_LEN 30		//系统状态长度

class Channel
{
	public:
		Channel();
		~Channel();
	public:
		static int connectGuardMain();
		static int getProcTask(int iProcID);
		static int calcWater(int iVpID);
		static char* trim(char* sp);
	public:
		static CommandCom* m_pCmdCom;												//和服务通讯
		static THeadInfo *m_pInfoHead;											//GuadMain首地址
		static TProcessInfo *m_pProcInfoHead;								//进程信息首地址
		static char m_sSysRunState[MAX_STATE_LEN];							//系统运行状态（全局）
		static ThreeLogGroup* m_pLog;                         //日志告警
};
#endif
