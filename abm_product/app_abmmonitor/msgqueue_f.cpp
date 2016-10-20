/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)msgqueue_f.c    1.0    2007/04/09    <AutoCreate>" */
#include <signal.h>
#include <unistd.h>
#include <sys/msg.h>

#include <iostream>
#include "GetinfoMonitor.h"
#include "MessageQueueH.h"
#include "ABMException.h"
#include "mainmenu.h"


#define _JUST_DBLINK_
#include "Environment.h"





#include "StdEvent.h"
#include "msgqueue.h"

Control *msgqueue_handle;

extern int g_iflowid[16];
extern int gStr2Arr(char *strfid);
extern int gArr2Str(char *strfid);
extern int IsInArr(int ifid);



#define MAX_MQ_NUM__    2048
char *msg_list_index[MAX_MQ_NUM__];    // 和List控件相关
char msg_list_data[MAX_MQ_NUM__][80];    // List控件显示的内容存储
typedef struct {
    int iFlowID;
    int iProcessID;
    int iMQID;
    char sProcName[PROCESS_NAME_LEN];
    char sState[20];
    int iMsgNum;
	int iMsgSize;
    int iMaxMsgNum;
}TMQInfo;
/* mq列表的数据存储 */
TMQInfo g_tMQList[MAX_MQ_NUM__];

/* mq数量 */
static int g_iMQListCount;

/* 实时刷新标识 */
static bool g_bRealtimeFlag = false;

        
void SetMQListData ();
void MsgMonitorEnd ()
{
    g_bRealtimeFlag = false;
}


int msgqueue_SysHotKeyPress(Control *pCtl, int ch, long lParam)
{   
    /* Add your self code here: */
    if (ch == 27) { /* ESC */
    return FORM_KILL;
    }

    return ch;
}

void msgqueue_entry(Control *pHandle) /* msgqueue 创建时触发调用 */
{   
    Control *pFlowEdit = 0;
    msgqueue_handle = pHandle;
    /* Add your self code here: */

    pFlowEdit = GetCtlByID(msgqueue_handle, 13);
    char tmp[32] = {0};    
    if(g_iflowid[0])
    {
        gArr2Str(tmp);
        sprintf( pFlowEdit->sData,"%s\0",tmp );
    }
   SetMQListData ();
}

int msgqueueCtl1Press(Control *pCtl,int ch,long lParam)
{   /* 刷新button的按键消息处理 */
    /* Add your self code here: */
    if (ch == '\r') {
   SetMQListData ();
        Control *pList = GetCtlByID (msgqueue_handle, 6);
        CtlShow (pList);
        CtlAtv (pCtl, pCtl);
    }

    return ch;
}
int msgqueueCtl2Press(Control *pCtl,int ch,long lParam)
{   /* 实时刷新button的按键消息处理 */
    /* Add your self code here: */
    if (ch == '\r') {
        signal( SIGINT,  NULL);
        signal( SIGINT,  (void (*)(int))MsgMonitorEnd );
        g_bRealtimeFlag = true;
        Control *pList = GetCtlByID (msgqueue_handle, 6);
        
        while ( g_bRealtimeFlag ) {
            SetMQListData ();
            CtlShow(pList);
            sleep(1);
        }
        CtlAtv( pCtl,pCtl );
    }
    return ch;
}
int msgqueueCtl3Press(Control *pCtl,int ch,long lParam)
{   /* 创建队列button的按键消息处理 */
    /* Add your self code here: */
    ABMException oabmExp;
    if (ch == '\r') {
            Control *pList = GetCtlByID (msgqueue_handle, 6);
            
            if (ListSelNum (pList) == 0) {
                prtmsg (pCtl, 20, 40, "请在队列列表框中按空格选择队列");
                return ch;
            }
            for (int i=0; i<g_iMQListCount; i++) {
                if (ListItemState (pList, i) != SELECTED)
                    continue;
                
                if(g_tMQList[i].iMQID<=0) continue;
                MessageQueue mq(g_tMQList[i].iMQID);
                mq.open(oabmExp,true,true);
            }
            
   SetMQListData ();
            CtlShow (pList);
            CtlAtv (pCtl, pCtl);        
    }
    return ch;
}
int msgqueueCtl4Press(Control *pCtl,int ch,long lParam)
{   /* 删除队列button的按键消息处理 */
    /* Add your self code here: */
    ABMException oabmExp;
    if (ch == '\r') {
        Control *pList = GetCtlByID (msgqueue_handle, 6);
        
        if (ListSelNum (pList) == 0) {
            prtmsg (pCtl, 20, 40, "请在队列列表框中按空格选择队列");
            return ch;
        }
        if (yorn (pCtl, 20, 40, "WARNING:确实要删除?(y/n)")==0) 
            return ch;
        for (int i=0; i<g_iMQListCount; i++) {
            if (ListItemState (pList, i) != SELECTED)
                continue; 
            if(g_tMQList[i].iMQID<=0) continue; 
            MessageQueue mq(g_tMQList[i].iMQID);
            mq.remove(oabmExp);        
        }
        
        SetMQListData ();
        CtlShow (pList);
        CtlAtv (pCtl, pCtl);    
    }
    
    return ch;

}
int msgqueueCtl5Press(Control *pCtl,int ch,long lParam)
{   /* 退出的按键消息处理 */
    /* Add your self code here: */
    if (ch == '\r') {
        mainmenu mm;
        mm.run();
        return FORM_KILL;
    }
    
    return ch;
}
int msgqueueCtl6Press(Control *pCtl,int ch,long lParam)
{   /* 主框架的按键消息处理 */
    /* Add your self code here: */
    if (ch == '\r') {   
        if (ListSelNum (pCtl) == 0) {
            ListSetItem (pCtl, ALL_ITEMS, SELECTED);
            CtlShow (pCtl);
        }               
    }
    
  return ch;
}
int msgqueueCtl13Press(Control *pCtl,int ch,long lParam)
{   /* 流程号的按键消息处理 */
    /* Add your self code here: */
    char* sFidtmp = 0;
    if (ch == '\r') {
        sFidtmp = GetCtlByID( msgqueue_handle, 13)->sData;
        if( strcmp( sFidtmp,"*" ) ==0 )
            g_iflowid[0] = 0;
        else
            gStr2Arr(sFidtmp);
        msgqueueCtl1Press(pCtl,ch,lParam);
    }
    //CtlAtv(pCtl,pBtn1 );
    return ch;
}

bool checkAppID(int iAppID) {
    if (g_iflowid[0] == 0) {
        return true;
    }

    for (int i = 1; i <= g_iflowid[0]; i++) {
        if (g_iflowid[i] == iAppID) {
            return true;
        }
    }
    return false;
}

void SetMQListData ()
{

    int i = 0;
    
    char sTemp[100];
        
    Control *pEdit = GetCtlByID(msgqueue_handle, 13);
    gStr2Arr(pEdit->sData);
    char tmp[256] = {0};
    char sql[1024] = {0};
    char sysuser[32] = {0};
    
    sprintf( sysuser,"'%s'\0",getenv("LOGNAME") );

    gArr2Str(tmp);
    if(!strlen(tmp))
        strcpy(tmp,"0\0");

    MonitorInfo oInfoMonitor;
    oInfoMonitor.getInfoCfg(); 
    oInfoMonitor.m_oSHMCSemaphore.P();
    int j = 0;
    while(i<(*oInfoMonitor.m_piNum)){
 
        if (!checkAppID(oInfoMonitor.m_poMonitorCfg[i].m_iAppID)) {
            i++;
            continue;
        } else {
            if(oInfoMonitor.m_poMonitorCfg[i].m_iRecvMsg <=0){
                i++;
                continue;
            }
        } 
        g_tMQList[j].iFlowID = oInfoMonitor.m_poMonitorCfg[i].m_iAppID;
        g_tMQList[j].iProcessID = oInfoMonitor.m_poMonitorCfg[i].m_iProcessID;
        strncpy (g_tMQList[j].sProcName, oInfoMonitor.m_poMonitorCfg[i].m_sAppName, PROCESS_NAME_LEN);
        g_tMQList[j].iMQID = oInfoMonitor.m_poMonitorCfg[i].m_iRecvMsg;
        g_tMQList[j].iMaxMsgNum = 1;
	    g_tMQList[j].iMsgSize = MQ_SIZE;
        msg_list_index[j] = msg_list_data[j];
        j++;
        i++; 
        
    }
    oInfoMonitor.m_oSHMCSemaphore.V();
    
    /* 设置控件中条目的数量 */
    Control *pList = GetCtlByID (msgqueue_handle, 6);
    pList->iAdditional = g_iMQListCount = j;
    for (i=0; i<g_iMQListCount; i++) {
        MessageQueue mq(g_tMQList[i].iMQID);
        //MessageQueue *mq = new MessageQueue(g_tMQList[i].iMQID);
        if(g_tMQList[i].iMQID>0) {
            if (!mq.exist ()) {
            //if (!mq->exist ()) {
                strcpy (g_tMQList[i].sState, "----");
            } else {
                strcpy (g_tMQList[i].sState, "存在");
            }
        } else{
            strcpy (g_tMQList[i].sState, "----");
        }
        sprintf (msg_list_data[i], "%-4d %6d %-20s %8d%5s% 8d%5d%%",
            g_tMQList[i].iFlowID,
            g_tMQList[i].iProcessID,
            g_tMQList[i].sProcName,
            g_tMQList[i].iMQID,
            g_tMQList[i].sState,
            mq.getMessageNumber (),
            //mq->getMessageNumber(),            
            mq.getOccurPercent()
            //mq->getOccurPercent()
            
        );
        
    }
    pList->pData = msg_list_index;
}

