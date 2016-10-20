/*VER: 2*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)process_f.c	1.0	2008/04/15	<AutoCreate>" */
#ifndef _PROCESS_Q_F_H_
#define	_PROCESS_Q_F_H_
#endif
#include "process.h"
#include "mainmenu.h"

#include "ProcessInfo.h"
#include "string.h"

//#include <iostream>
#include <signal.h>
#include <time.h>
#include <unistd.h>


Control *process_handle;
extern TProcessInfo *pProcInfoHead;
extern THeadInfo *pInfoHead;
extern char *gsState[];
//
extern int g_iflowid[16];
extern int gStr2Arr(char *strfid);
extern int gArr2Str(char *strfid);
extern int IsInArr(int ifid);

extern int g_iappid[16];
extern int gStr2Appid(char *strfid);
extern int gAppid2Str(char *strfid);
extern int IsInAppid(int ifid);




char *proc_list_index[MAX_APP_NUM];
char proc_list_data[MAX_APP_NUM][80];

int  aiProcessID[MAX_APP_NUM]; //add by yks

int g_proc_monitor_flag;

int GetProcInfo ();
void ProcMonitorEnd ();
int GetSysProcInfo (int iPID, char *sMem, char *sCpu);

TProcessInfo *InfoList[MAX_APP_NUM];
void InitList();

//List中的进程个数
int l_lprocnum;

void InitList()
{
	for (int i = 0;i<MAX_APP_NUM;i++)
		InfoList[i] = 0;
}


int process_SysHotKeyPress(Control *pCtl, int ch, long lParam)
{   
    /* Add your self code here: */
    if (ch == 27) { /* ESC */
    	return FORM_KILL;
    }

    return ch;
}

void process_entry(Control *pHandle) /* process 创建时触发调用 */
{   
    process_handle = pHandle;
    /* Add your self code here: */
		Control *pProcList;
		Control *pFlowEdit;
	
		//
		//	g_iflowid = 0;
		int rows = 0;
	
		pFlowEdit = GetCtlByID(process_handle, 13);
		
		char tmp[32] = {0};
		if(g_iflowid[0])
		{
			gArr2Str(tmp);
			sprintf( pFlowEdit->sData,"%s\0",tmp );
		}
		l_lprocnum = 0;
			
		pProcList = GetCtlByID (process_handle, 6);
		//pProcList->iAdditional = pInfoHead->iProcNum;
	
		rows = GetProcInfo ();
		pProcList->iAdditional = rows;
		pProcList->pData = proc_list_index;

}

int processCtl1Press(Control *pCtl,int ch,long lParam)
{   /* 刷新button的按键消息处理 */
    /* Add your self code here: */
		Control *pProcList;
		
		int rows = 0;
		if (ch == '\r') {
			if ((rows = GetProcInfo ()) < 0) {
			    prtmsg (pCtl, 20, 41, "连接共享内存错误");
			    return 0;
			}
		
			pProcList = GetCtlByID (process_handle, 6);
		
			pProcList->iAdditional = rows;
			pProcList->pData = proc_list_index;
		
			CtlShow (pProcList);
			CtlAtv (pCtl, pCtl);
		}
		
		return ch;

}
int processCtl2Press(Control *pCtl,int ch,long lParam)
{   /* 实时刷新button的按键消息处理 */
    /* Add your self code here: */
		Control *List;
		if (ch == '\r') {
		signal( SIGINT,  NULL);
		signal( SIGINT,  (void (*)(int))ProcMonitorEnd );
		g_proc_monitor_flag = 1;
		List = GetCtlByID (process_handle, 6);
		int rows = 0;
		
		while ( g_proc_monitor_flag ) {
		    if ((rows = GetProcInfo ()) < 0) {
		    prtmsg (pCtl, 20, 41, "连接共享内存错误");
		    return 0;
		    }
			List->iAdditional = rows;
			List->pData = proc_list_index;
		    CtlShow(List);
		    sleep(1);
		}
		CtlAtv( pCtl,pCtl );
		}
	
		return ch;

}

int processCtl3Press(Control *pCtl,int ch,long lParam)
{   /* 进程启动button的按键消息处理 */
    /* Add your self code here: */
	Control *pList;
	int i, iFlag = 1;

	if (ch == '\r') {

	pList = GetCtlByID (process_handle, 6);

	for (i=0; i<l_lprocnum; i++) {
	    if (ListItemState (pList, i) != SELECTED)
	    	continue;

	    iFlag = 0;
		
	    if (InfoList[i]->iState != ST_RUNNING && \
	            InfoList[i]->iState != ST_INIT) {
	        InfoList[i]->iState = ST_WAIT_BOOT;
	    }
	}
	if (iFlag)
	    prtmsg (pCtl, 20, 40, "请在进程列表中用空格选择进程");

	GetProcInfo ();
	CtlShow (pList);
	CtlAtv (pCtl, pCtl);
	}

	return ch;
}

int processCtl4Press(Control *pCtl,int ch,long lParam)
{   /* 进程kill.button的按键消息处理 */
    /* Add your self code here: */
	Control *pList;
	int i, iFlag = 1;

	if (ch == '\r') {

		pList = GetCtlByID (process_handle, 6);

		for (i=0; i<l_lprocnum; i++) {
		    if (ListItemState (pList, i) != SELECTED)
		    	continue;

		    iFlag = 0;
		    if (InfoList[i]->iState==ST_RUNNING || InfoList[i]->iState==ST_INIT)
		        InfoList[i]->iState = ST_WAIT_DOWN;		    
		}
		if (iFlag)
		    prtmsg (pCtl, 20, 40, "请在进程列表中用空格选择进程");

		GetProcInfo ();
		CtlShow (pList);
		CtlAtv (pCtl, pCtl);
	}

	return ch;
}

int processCtl5Press(Control *pCtl,int ch,long lParam)
{   /* 退出的按键消息处理 */
    /* Add your self code here: */
	if (ch == '\r') {
		mainmenu mm;
		mm.run();
		return FORM_KILL;
	}
 
    return ch;
}
int processCtl6Press(Control *pCtl,int ch,long lParam)
{   /* 主框架的按键消息处理 */
    /* Add your self code here: */
    if (ch == '\r') {
    if (ListSelNum (pCtl) == 0) {
                ListSetItem (pCtl, ALL_ITEMS,SELECTED);
                CtlShow (pCtl);
        }
    }

    return ch;
}
int processCtl13Press(Control *pCtl,int ch,long lParam)
{   /* 流程号的按键消息处理 */
    /* Add your self code here: */
	char* sFidtmp = 0;
	if (ch == '\r') {
	sFidtmp = GetCtlByID( process_handle, 13)->sData;
	if( strcmp( sFidtmp,"*" ) ==0 )
		g_iflowid[0] = 0;
	else
		gStr2Arr(sFidtmp);
		processCtl1Press(pCtl,ch,lParam);
	
	}
	return ch;
}
int processCtl15Press(Control *pCtl,int ch,long lParam)
{   /* APP_ID的按键消息处理 */
    /* Add your self code here: */
		char* sFidtmp = 0;
		if (ch == '\r') {
		sFidtmp = GetCtlByID( process_handle, 15)->sData;
		if( strcmp( sFidtmp,"*" ) ==0 )
			g_iappid[0] = 0;
		else
			gStr2Appid(sFidtmp);
			processCtl1Press(pCtl,ch,lParam);
		
		}

    return ch;
}

void ProcMonitorEnd ()
{
    g_proc_monitor_flag = 0;
}

// < 0 : 未找到
// >=0 : 返回值为找到的 pProcInfoHead 的偏移量
int SearchNextNode (int *pLastBillFlowID, int *pLastProcID, int *pLastAppID)
{
    int iTmpBillFlowID = *pLastBillFlowID;
    int iTmpProcessID = *pLastProcID;
    int iSelecetdPos = -1;
    
    for (int i=0; i<pInfoHead->iProcNum; i++) {
        
        if ((pProcInfoHead+i)->iProcessID == -1) //节点已空闲
            continue;
                    
        if ((pProcInfoHead+i)->iBillFlowID <  iTmpBillFlowID) 
            continue;
        if ((pProcInfoHead+i)->iBillFlowID == iTmpBillFlowID) {
            if ((pProcInfoHead+i)->iProcessID <= iTmpProcessID) 
                continue;
        }
        
        //比输入的大,再检查是否比已选择的小
        if (iSelecetdPos == -1 ) {
            if (iTmpProcessID!=(pProcInfoHead+i)->iProcessID)
                iSelecetdPos = i;
            continue;
        }
        
        if ((pProcInfoHead+i)->iBillFlowID > (pProcInfoHead+iSelecetdPos)->iBillFlowID)
            continue;
        if ((pProcInfoHead+i)->iBillFlowID ==(pProcInfoHead+iSelecetdPos)->iBillFlowID) {
            if ((pProcInfoHead+i)->iProcessID >= (pProcInfoHead+iSelecetdPos)->iProcessID)
                continue;
        }
        
        if ((pProcInfoHead+i)->iBillFlowID == 0 || (pProcInfoHead+i)->iProcessID==0)
            continue;
        
        iSelecetdPos = i;
    }
    
    if (iSelecetdPos>=0) {
        *pLastBillFlowID = (pProcInfoHead+iSelecetdPos)->iBillFlowID;
        *pLastProcID = (pProcInfoHead+iSelecetdPos)->iProcessID;
        *pLastAppID = (pProcInfoHead+iSelecetdPos)->iAppID;
    }
    
    return iSelecetdPos;
    
}

int GetProcInfo ()
{
	struct tm *pre;
	long clock = 0;
	char sCPU[100], sMEM[100];
	char sProcName[PROCESS_NAME_LEN];   
	int iTmpBillFlowID=-1, iTmpProcessID=-1; //已经显示的最大ID
	
	int iTmpAppID = -1;
	int iSelecetdPos = -2;

	int iListPos=0;

	//  
	char tmp[128] = {0};
	int ret = 0;
	InitList();

	Control *pEdit = GetCtlByID(process_handle, 13);
	gStr2Arr(pEdit->sData);	
	
	Control *pEdit1 = GetCtlByID(process_handle, 15);
	gStr2Appid(pEdit1->sData);

	DetachShm ();

	if (AttachShm (0, (void **)&pInfoHead) >= 0) {
	    pProcInfoHead = &pInfoHead->ProcInfo;
	} else {
	    return -1;
	}

	time ((time_t *)&clock);
	    
	memset(aiProcessID, 0, MAX_APP_NUM*sizeof(int));


	//
	while (iListPos < pInfoHead->iProcNum)
	{
	    int iSelecetdPos = SearchNextNode(&iTmpBillFlowID, &iTmpProcessID,&iTmpAppID);
	    
	    if (iSelecetdPos < 0)
	        break;

	//
	//	if( g_iflowid[0] && iTmpBillFlowID != g_iflowid )
	//		continue;
	
			if( ((g_iflowid[0] && !IsInArr( iTmpBillFlowID )))||((g_iappid[0] && !IsInAppid( iTmpAppID ))) || 
				!(pProcInfoHead+iSelecetdPos)->iBillFlowID || !(pProcInfoHead+iSelecetdPos)->iProcessID )
				continue;
			
	    strncpy( sProcName, (pProcInfoHead+iSelecetdPos)->sName, PROCESS_NAME_LEN);
	    sProcName[PROCESS_NAME_LEN-1] =0;
	    
	    if ((pProcInfoHead+iSelecetdPos)->iState == ST_RUNNING) {
					memset(sCPU,0,sizeof(sCPU));
					memset(sMEM,0,sizeof(sMEM));
	        pre = localtime ((const time_t *)&(pProcInfoHead+iSelecetdPos)->lLoginTime);
	        GetSysProcInfo ((pProcInfoHead+iSelecetdPos)->iSysPID, sMEM, sCPU);
			memset( proc_list_data[iListPos],0,strlen(proc_list_data[iListPos]) );
	        sprintf (proc_list_data[iListPos], 
	        "%-4d %6d %-20s %02d%02d%02d%-10d%-4s %-5s %-3s%\0",
	        (pProcInfoHead+iSelecetdPos)->iBillFlowID,
	        (pProcInfoHead+iSelecetdPos)->iProcessID,
	        sProcName, 
	        pre->tm_mon+1,
	        pre->tm_mday,
	        pre->tm_hour,
	        pre->tm_min,
	        //(pProcInfoHead+iSelecetdPos)->iAllTickets,
	        gsState[(pProcInfoHead+iSelecetdPos)->iState], 
	        sMEM, 
	        sCPU
	        );
	    }
	    else {
	        sprintf (proc_list_data[iListPos], 
	        "%-4d %6d %-20s --------        %-4s %-5d %-3d%\0",
	        (pProcInfoHead+iSelecetdPos)->iBillFlowID,
	        (pProcInfoHead+iSelecetdPos)->iProcessID,
	        sProcName,
	        //(pProcInfoHead+iSelecetdPos)->iAllTickets,
	        gsState[(pProcInfoHead+iSelecetdPos)->iState], 
	        0,
	        0
	        );
	    }
	    proc_list_index[iListPos] = proc_list_data[iListPos];
	    aiProcessID[iListPos] = (pProcInfoHead+iSelecetdPos)->iProcessID;
	    
	    iListPos++;

		//插入InfoList
		InfoList[ret] = pProcInfoHead+iSelecetdPos;

		
	    ret ++;
	    iSelecetdPos = -1;
	}
	l_lprocnum = ret;
	return ret;
}

int GetSysProcInfo (int iPID, char *sMem, char *sCpu)
{
    char sCommand[256];
    FILE *fp;
    int ret;

    strcpy (sMem, "0");
    strcpy (sCpu, "0");
    
    
#ifdef DEF_HP
#define _guard_def_unix95__
#endif
#ifdef DEF_SOLARIS
#define _guard_def_unix95__
#endif

#ifdef DEF_UNIX95
#define _guard_def_unix95__
#endif

#ifdef _guard_def_unix95__
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

