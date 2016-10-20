/*******************************************************************************************
Copyright 2011 Asiainfo-Linkage Co.,Ltd. Inc. All Rights Reversed   
********************************************************************************************
**Class : StatFormBase
**Description : srstatµÄ»æÍ¼»ùÀà
**Author : xn  
**StartTime : 20110310
**Last Change Time  : 20110310
*******************************************************************************************/
#ifdef DEF_HP
#define _XOPEN_SOURCE_EXTENDED
#endif

#ifndef _SR_STAT_H
#define _SR_STAT_H


#include <curses.h>

#ifdef erase
#undef erase
#endif

#ifdef clear
#undef clear
#endif

#ifdef move
#undef move
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
//#include "platform.h"
#include <algorithm>
#include <map>
#include <string>
#include <sys/shm.h>
#include "ProcessInfo.h"

#if defined DEF_HP || DEF_AIX || DEF_LINUX || DEF_SOLARIS
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#endif

#ifdef DEF_HP
#include <strings.h>
#endif
//#include "platform.h"

#include "../../include/Env.h"

#define COLS_LENS 130
#define LINE_SIZE 30

class StatFormBase
{
	public:
		StatFormBase();
		virtual ~StatFormBase();
	public:
		void StartWin();
		void EndWin();
		static void TimeBreak(int signal);
		void LoadEmptyMenu();
		void LoadGeneralInfo();
		void DisplayTime();
		virtual void Rebuild();
		virtual void Build();
		static void SetInterval(int iInterval);
		static void SetMoudle(char *sMoudle);
		static void SetShowFlag(int iShowFlag);
		bool ConnectGuardMain();
	public:
		static TProcessInfo *pProcInfoHead;
		static THeadInfo *pInfoHead;
	public:
		bool m_bCursesOn;
		int m_iSelectedRow;
		int m_iStartRow;
		static int m_iInterval;
		static char m_sMoudle[32];
		static int m_iShowFlag;
	public:
		static StatFormBase* pThis;
};


#endif


