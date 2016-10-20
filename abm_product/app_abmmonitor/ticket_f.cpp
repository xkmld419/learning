/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)ticket_f.c	1.0	2007/03/26	<AutoCreate>" */
#include "ticket.h"
#include "ProcessInfo.h"
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "mainmenu.h"

Control *ticket_handle;
extern TProcessInfo *pProcInfoHead;
extern THeadInfo *pInfoHead;
extern char *gsState[];

extern int g_iflowid[16];
extern int gStr2Arr(char *strfid);
extern int gArr2Str(char *strfid);
extern int IsInArr(int ifid);

int g_ticket_monitor_flag;

char *ticket_list_index[MAX_APP_NUM];
char ticket_list_data[MAX_APP_NUM][80];

int GetTicketInfo ();
void TicketMonitorEnd ();

//List中的进程个数
int l_lpnum;

int ticket_SysHotKeyPress(Control *pCtl, int ch, long lParam)
{   
    /* Add your self code here: */
	if (ch == 27) { /* ESC */
		return FORM_KILL;
	}     
	
	return ch;
}

void ticket_entry(Control *pHandle) /* ticket 创建时触发调用 */
{   
	Control *pTicketList;
	Control *pFlowEdit = 0;
	ticket_handle = pHandle;
	/* Add your self code here: */

	pFlowEdit = GetCtlByID(ticket_handle, 13);
	char tmp[32] = {0};
	if(g_iflowid[0])
	{
		gArr2Str(tmp);
		sprintf( pFlowEdit->sData,"%s\0",tmp );
	}
	pTicketList = GetCtlByID (ticket_handle, 6);
	
	GetTicketInfo ();
	pTicketList->iAdditional = l_lpnum;
	pTicketList->pData = ticket_list_index;

}

int ticketCtl1Press(Control *pCtl,int ch,long lParam)
{   /* 刷新button的按键消息处理 */
    /* Add your self code here: */

	Control *pTicketList;
	
	if (ch == '\r') {
		
		if (GetTicketInfo () < 0) {
			prtmsg (pCtl, 20, 41, "连接共享内存错误");
			return 0;
		}
		
		pTicketList = GetCtlByID (ticket_handle, 6);
		pTicketList->iAdditional = l_lpnum;
		pTicketList->pData = ticket_list_index;		
		CtlShow (pTicketList);
		CtlAtv (pCtl, pCtl);
		
	}
	
	return ch;
}

int ticketCtl2Press(Control *pCtl,int ch,long lParam)
{   /* 实时刷新button的按键消息处理 */
    /* Add your self code here: */
Control *List;
if (ch == '\r') {
	signal( SIGINT,  NULL);
	signal( SIGINT,  (void (*)(int))TicketMonitorEnd );
	g_ticket_monitor_flag = 1;
	List = GetCtlByID (ticket_handle, 6);
	while ( g_ticket_monitor_flag ) {
		if (GetTicketInfo () < 0) {
			prtmsg (pCtl, 20, 41, "连接共享内存错误");
			return 0;
		}
		List->iAdditional = l_lpnum;
		List->pData = ticket_list_index;	
		CtlShow(List);
		sleep(1);
	}
	CtlAtv( pCtl,pCtl );
}

return ch;
}
int ticketCtl3Press(Control *pCtl,int ch,long lParam)
{   /* 退出的按键消息处理 */
    /* Add your self code here: */
	if (ch == '\r') {
		mainmenu mm;
		mm.run();
		return FORM_KILL;
	}
	
	return ch;

}
int ticketCtl6Press(Control *pCtl,int ch,long lParam)
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
int ticketCtl13Press(Control *pCtl,int ch,long lParam)
{   /* 流程号的按键消息处理 */
    /* Add your self code here: */
	char* sFidtmp = 0;
    if (ch == '\r') {
		sFidtmp = GetCtlByID( ticket_handle, 13)->sData;
		if( strcmp( sFidtmp,"*" ) ==0 )
			g_iflowid[0] = 0;
		else
			gStr2Arr(sFidtmp);
		ticketCtl1Press(pCtl,ch,lParam);
    }
    return ch;
}

void TicketMonitorEnd ()
{
	g_ticket_monitor_flag = 0;
}

int GetTicketInfo ()
{
	int i = 0;
	l_lpnum = 0;

	Control *pEdit = GetCtlByID(ticket_handle, 13);
	gStr2Arr(pEdit->sData);
	
	DetachShm ();	
	if (AttachShm (0, (void **)&pInfoHead) >= 0) {
		pProcInfoHead = &pInfoHead->ProcInfo;
	} else {
		return -1;
	}
	char sProcName[16];
	for (i=0; i<pInfoHead->iProcNum; i++) {
		
//		if( g_iflowid && (pProcInfoHead+i)->iBillFlowID != g_iflowid )
//			continue;
		if( g_iflowid[0] && !IsInArr( (pProcInfoHead+i)->iBillFlowID ) )
			continue;
		
		if( (pProcInfoHead+i)->iBillFlowID == 0||(pProcInfoHead+i)->iProcessID == 0 )
			continue;
		strncpy( sProcName, (pProcInfoHead+i)->sName, 16 );
		sProcName[15] = '\0';
		int t = 0;
		int l = strlen(sProcName);
		for ( int m = 0; m < l; ++m )
		{
			if ( sProcName[m] & 0x00000080 ) t++;
		}
		if ( t % 2 == 1 && l > 0 ) sProcName[ l-1 ] = '\0';

		sprintf (ticket_list_data[i], "%-4d %6d %-20s%10d%10d%8d%8d",
				(pProcInfoHead+i)->iBillFlowID,
				(pProcInfoHead+i)->iProcessID,
				sProcName,
				//(pProcInfoHead+i)->iAllTickets,
				(pProcInfoHead+i)->iNormalTickets,
				(pProcInfoHead+i)->iBlackTickets,
				(pProcInfoHead+i)->iErrTickets,
				(pProcInfoHead+i)->iOtherTickets
				);
		ticket_list_index[l_lpnum] = ticket_list_data[i];
		l_lpnum ++;
	}

	return 0;
}
