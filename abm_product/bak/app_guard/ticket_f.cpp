/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)ticket_f.c	1.0	2007/03/26	<AutoCreate>" */
#include "TTTns.h"
#include "ticket.h"
#include "ProcessInfo.h"
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "mainmenu.h"


class AccessTT:public TTTns
{
	public:		
	AccessTT()
	{
		m_poHeadle = NULL;
	};
	int Init()
	{
		
		m_poHeadle = NULL;
		try 
	  {
        if (GET_CURSOR(m_poHeadle, m_poTTConn, m_sConnTns, m_oExp) != 0) 
		    {
            return -1;
        }
    }
    catch(...) 
	  {
        return -1;
    }
	
    return 0;
	}; 
	TimesTenCMD *m_poHeadle;
	ABMException m_oExp;		 
};

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

struct MessageInfo
{
	int iNetID;
	int iRecv_num;
	int iSucc_num;
	int iRjct_num;
	int iFail_num;
};

#define MAX_INFO_NUM 1024
MessageInfo MessageInfoArray[MAX_INFO_NUM];

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
		l_lpnum = GetTicketInfo();
		if (l_lpnum < 0) {
			prtmsg (pCtl, 20, 41, "获取统计信息失败");
			return 0;
		}
		else if(l_lpnum == 0)
		{
			prtmsg (pCtl, 20, 41, "没有统计信息");
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
		l_lpnum = GetTicketInfo();
		if (l_lpnum < 0) {
			prtmsg (pCtl, 20, 41, "获取统计信息失败");
			return 0;
		}
		else if(l_lpnum == 0)
		{
			prtmsg (pCtl, 20, 41, "没有统计信息");
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
	
	 memset(MessageInfoArray,0,sizeof(MessageInfoArray));
	 char cSql[1024] = {0};
	    
	     int iPos = 0;	    
	     AccessTT objTT;
	     objTT.Init();
	     
	     sprintf(cSql,"select max(sys_time) from hss_balance_mess where  balance_type = 0");
	     
	     
	     (objTT.m_poHeadle)->Prepare(cSql);
	     
	     long ltime;
	     
	     objTT.m_poHeadle->Commit();
		   objTT.m_poHeadle->Execute();
		   if(objTT.m_poHeadle->FetchNext())
		   {
		     objTT.m_poHeadle->getColumn(1, &ltime);
		   }
		   else
		   {
		   	return 0;
		   }
		   objTT.m_poHeadle->Close();
		   
		   memset(cSql,0,1024);
		   sprintf(cSql , "select in_ne, recv_num,succ_num,rjct_num,fail_num from hss_balance_mess where balance_type = 0 \
		   and sys_time = %ld",ltime);
		   
		   objTT.m_poHeadle->Prepare(cSql);
	     
	     objTT.m_poHeadle->Commit();
		   objTT.m_poHeadle->Execute(); 
		while(!(objTT.m_poHeadle->FetchNext()))
		{
			if(iPos >= MAX_INFO_NUM)
			{			 
				return iPos;
			}
			  memset(&(MessageInfoArray[iPos]),0,sizeof(MessageInfo));
			  objTT.m_poHeadle->getColumn(1, &(MessageInfoArray[iPos].iNetID));
			  objTT.m_poHeadle->getColumn(2, &(MessageInfoArray[iPos].iRecv_num));
			  objTT.m_poHeadle->getColumn(3, &(MessageInfoArray[iPos].iSucc_num));
			  objTT.m_poHeadle->getColumn(4, &(MessageInfoArray[iPos].iRjct_num));
			  objTT.m_poHeadle->getColumn(5, &(MessageInfoArray[iPos].iFail_num));
      
      
			
			 //  if(g_iflowid[0] && (!IsInArr(AbilityInfoArray[iPos].iNodeID)))
    	  // continue;
			
    	 memset(ticket_list_data[iPos],0,80);
    	 sprintf(ticket_list_data[iPos],"%-10d%%-10d%-10d%-10d%-10d",
    	                                               MessageInfoArray[iPos].iNetID,
    	                                               MessageInfoArray[iPos].iRecv_num,
    	                                               MessageInfoArray[iPos].iSucc_num,
    	                                               MessageInfoArray[iPos].iRjct_num,
    	                                               MessageInfoArray[iPos].iFail_num  	                                               
    	                                           );
    	 ticket_list_index[iPos] = ticket_list_data[iPos];
    	 iPos++;
		}
		objTT.m_poHeadle->Close();
	
	return iPos;
}
