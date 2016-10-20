/*******************************************************************************************
Copyright 2011 Asiainfo-Linkage Co.,Ltd. Inc. All Rights Reversed   
********************************************************************************************
**Class : OnlineTime
**Description : sgwstat -s 列出当前系统的启动时间（格式：**年**月**日**时**分**秒）
**Author : xn  
**StartTime : 20110331
**Last Change Time  : 20110331
*******************************************************************************************/
#ifndef _ONLINE_TIME_H
#define _ONLINE_TIME_H

#include "ReadIni.h"
#include "CommandCom.h"

class OnlineTime: public StatFormBase
{
	public:
		OnlineTime();
		~OnlineTime();
	public:
		void Rebuild();
	private:
		ReadIni m_ReadFile;
		CommandCom* m_pCmd;
};

#endif


