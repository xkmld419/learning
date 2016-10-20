/*******************************************************************************************
Copyright 2011 Asiainfo-Linkage Co.,Ltd. Inc. All Rights Reversed   
********************************************************************************************
**Class : StatBaseMgr
**Description : srstat的入口，继承命令集框架
**Author : xn  
**StartTime : 20110331
**Last Change Time  : 20110331
*******************************************************************************************/

#ifndef _SR_STAT_MGR_H
#define _SR_STAT_MGR_H

#include "BaseProcess.h"

class StatBaseMgr : public BaseProcess
{
	public:
		StatBaseMgr();
		~StatBaseMgr();
		void PrintUsage();
	public:
		int run();
	private:
		StatFormBase* pDraw;
};

#endif


