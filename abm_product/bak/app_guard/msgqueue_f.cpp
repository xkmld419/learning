/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)msgqueue_f.c    1.0    2007/04/09    <AutoCreate>" */
#include "CommandCom.h"
#include <signal.h>
#include <unistd.h>

#include <iostream>

#include "mainmenu.h"

#define _JUST_DBLINK_
#include "Environment.h"

#include "MessageQueue.h"
#include "StdEvent.h"
#include "msgqueue.h"
#include "IpcKey.h"


Control *msgqueue_handle;

extern int g_iflowid[16];
extern int gStr2Arr(char *strfid);
extern int gArr2Str(char *strfid);
extern int IsInArr(int ifid);

//DEFINE_QUERY (qry);

#define MAX_MQ_NUM__    250
char *msg_list_index[MAX_MQ_NUM__];    // 和List控件相关
char msg_list_data[MAX_MQ_NUM__][80];    // List控件显示的内容存储
typedef struct {
    int iFlowID;
    int iProcessID;
    int iMQID;
    char sProcName[PROCESS_NAME_LEN];
    char sState[20];
    int iMsgNum;
    int iMaxMsgNum;
    int iVpID;
}TMQInfo;
/* mq列表的数据存储 */
TMQInfo g_tMQList[MAX_MQ_NUM__];

/* mq数量 */
static int g_iMQListCount;

/* 实时刷新标识 */
static bool g_bRealtimeFlag = false;

/* 获取所有mq的sql语句 */
/*
static char *g_sMQListSql =  "select   a.billflow_id billflow_id, \
                a.process_id process_id, \
                b.exec_name exec_name, \
                c.mq_id mq_id, \
                d.max_msg_num max_msg_num, \
                a.caption caption \
            from wf_process a, wf_application b, wf_process_mq c, wf_mq_attr d \
            where a.app_id = b.app_id \
            and b.app_type=0 \
            and a.process_id=c.process_id \
            and c.mq_id=d.mq_id \
            and a.billflow_id=:bflowid \
            order by a.billflow_id, a.process_id";
*/
static char *g_sMQListSql =  "select   a.billflow_id billflow_id, \
                a.process_id process_id, \
                b.exec_name exec_name, \
                c.mq_id mq_id, \
                d.max_msg_num max_msg_num, \
				        substr(a.caption,1,10) caption ,\
				        a.vp_id \
            from wf_process a, wf_application b, wf_process_mq c, wf_mq_attr d \
            where a.app_id = b.app_id \
            and b.app_type=0 \
            and a.process_id=c.process_id \
            and c.mq_id=d.mq_id \
            and (a.billflow_id in (%s) or %d = 0) \
            and a.sys_username=%s  and a.host_id = %d \
            order by a.billflow_id, a.process_id";
            
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
    if (ch == '\r') {
            Control *pList = GetCtlByID (msgqueue_handle, 6);
            
            if (ListSelNum (pList) == 0) {
                prtmsg (pCtl, 20, 40, "请在队列列表框中按空格选择队列");
                return ch;
            }
            
            for (int i=0; i<g_iMQListCount; i++) {
                if (ListItemState (pList, i) != SELECTED)
                    continue;
            
                MessageQueue mq(g_tMQList[i].iMQID);
                mq.open (true, true, sizeof (StdEvent), g_tMQList[i].iMaxMsgNum);
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
            
            MessageQueue mq(g_tMQList[i].iMQID);
            mq.remove ();
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
    /*
    Control *plist = 0;
    Control *pBtn1 = 0;
    pBtn1 = GetCtlByID (msgqueue_handle,1);
    plist = GetCtlByID (msgqueue_handle,6);
    */
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

void SetMQListData ()
{
    DEFINE_QUERY (qry);

    int i = 0;
    
    char sTemp[100];
        
    /*
    char tmp[256] = {0};
    if( g_iflowid )
        sprintf(tmp,"and billflow_id = %d \0",g_iflowid);
    else
        strcpy(tmp," ");
    */
    /* 设置控件中的内容 */
    Control *pEdit = GetCtlByID(msgqueue_handle, 13);
    gStr2Arr(pEdit->sData);
    qry.close ();
    char tmp[256] = {0};
    char sql[1024] = {0};
    char sysuser[32] = {0};
    
    sprintf( sysuser,"'%s'\0",getenv("LOGNAME") );

    gArr2Str(tmp);
    if(!strlen(tmp))
        strcpy(tmp,"0\0");
    sprintf(sql,g_sMQListSql,tmp,g_iflowid[0],sysuser, IpcKeyMgr::getHostInfo(sTemp));
    qry.setSQL (sql);
//    qry.setParameter ("bflowid",g_iflowid);
    qry.open ();
    
    while (qry.next ()) {
        g_tMQList[i].iFlowID = qry.field (0).asInteger();
        g_tMQList[i].iProcessID = qry.field(1).asInteger();
        strncpy (g_tMQList[i].sProcName, qry.field(5).asString(), PROCESS_NAME_LEN);
        g_tMQList[i].iMQID = qry.field(3).asInteger();
        g_tMQList[i].iMaxMsgNum = qry.field(4).asInteger();
        g_tMQList[i].iVpID=qry.field(6).asInteger();
        msg_list_index[i] = msg_list_data[i];
        i++;
    }

    qry.commit();
    qry.close();
    
    /* 设置控件中条目的数量 */
    Control *pList = GetCtlByID (msgqueue_handle, 6);
    pList->iAdditional = g_iMQListCount = i;
    /*
    CommandCom *m_pCmdCom = NULL;
		m_pCmdCom = new CommandCom();
		m_pCmdCom->InitClient();
		int iPercent=0;
		int iQueueSize=1000;
		char sTemp2[20]={0};
		*/
    for (i=0; i<g_iMQListCount; i++) {
        MessageQueue mq(g_tMQList[i].iMQID);
        if (!mq.exist ()) {
            strcpy (g_tMQList[i].sState, "----");
        } else {
            strcpy (g_tMQList[i].sState, "存在");
        }
       /* 
       if(g_tMQList[i].iVpID>=1) {
          memset(sTemp2,0,sizeof(sTemp2));
          sprintf(sTemp2,"vp%d_queue_size",g_tMQList[i].iVpID/1000);
	        iQueueSize = m_pCmdCom->readIniInteger ("SCHEDULE", sTemp2, 0); //队列长度
	        if(iQueueSize>0){
	          iPercent=mq.getMessageNumber()*100/iQueueSize;
	        }else{
	          iPercent=mq.getOccurPercent();
	        }	        
	      }else{
	          iPercent=mq.getOccurPercent();
	      }
       */
        sprintf (msg_list_data[i], "%-4d %6d %-20s %8d%9s%5d%5d%%",
            g_tMQList[i].iFlowID,
            g_tMQList[i].iProcessID,
            g_tMQList[i].sProcName,
            g_tMQList[i].iMQID,
            g_tMQList[i].sState,
            mq.getMessageNumber (),
            //iPercent
            mq.getOccurPercent()
        );
        
        /*
        sprintf (msg_list_data[i], "%-4d %6d %-20s %8d%9s%5d%",
            g_tMQList[i].iFlowID,
            g_tMQList[i].iProcessID,
            g_tMQList[i].sProcName,
            g_tMQList[i].iMQID,
            g_tMQList[i].sState,
            mq.getMessageNumber ()
           // mq.getOccurPercent()
        );
        */
    }
    
    pList->pData = msg_list_index;
    
    /*
    if(m_pCmdCom)
    	{
    	delete m_pCmdCom;
    	m_pCmdCom=NULL; 
     }
     */
}
