/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)sharedmemory_f.c    1.0    2007/04/03    <AutoCreate>" */
#include "sharedmemory.h"

#define BILLINGCYCLEMGR_H_HEADER_INCLUDED_BD5BA9DC

#ifdef erase
#undef erase
#endif
#ifdef clear
#undef clear
#endif
#ifdef move
#undef move
#endif

#include "SimpleSHM.h"
#include "ReadCfg.h"



#define CUST_INFO_DATA         100
#define  CUST_INFO_INDEX       200
//#include "BillingCycleMgr.h"
//#include "MemInfo.h"
#include "mainmenu.h"


Control *sharedmemory_handle;

#define MAX_SHM_NUM    100
char *shm_list_index[MAX_SHM_NUM];
char shm_list_data[MAX_SHM_NUM][80];
static int g_iSHMListCount;

struct TSHMInfo{
    long  iSHMID;
    const char *sSHMID;
    const char *sSHMName;
    const int iType;     //1:SHMData,2:SHMIntHashIndex,3:SHMStringTreeIndex
};

int SetSHMListData (SharedMemInfo *pMemInfo)
{
/*
#ifdef DEF_HP	
    setlocale(LC_ALL,"zh_CN.gb18030");
#else
    setlocale(LC_ALL,"zh_CN");
#endif 
*/   
    ABMException oExp;
    //MemInfo meminfo;
    long lused = 0;
    char sFileName[1024];
    char *penv = getenv("ABM_PRO_DIR");
    if (!penv) {
        ADD_EXCEPT0(oExp, "miss environment ABMAPP_DEPLOY, please add it to your.profile");
        return 0;
    }
    if (penv[strlen(penv)-1] != '/') {
        snprintf (sFileName, sizeof(sFileName)-1, "%s/etc/abm_app_cfg.ini",penv);
    } else {
        snprintf (sFileName, sizeof(sFileName)-1, "%setc/abm_app_cfg.ini",penv);
    }
    ReadCfg shmCfg;
    ReadCfg shmTempCfg;
    if (shmCfg.read(oExp, sFileName, "MONITOR_SHM" , '|') == -1) {
        ADD_EXCEPT1(oExp, "ReadCfg::read(%s, \"SYSTEM_MONITOR\", '|') failed",sFileName);
        return -1;     
    }
    int irows = shmCfg.getRows();
    long  iSHMID=0;
    char sValue[64];
    char sSHMID[64];
    char sSHMName[64];
    for(int i=1;i<=irows;i++){
        memset(sValue, 0x00, sizeof(sValue));
        if (!shmCfg.getValue(oExp, sValue, "app_key", i)) {
            ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed","app_key", i);
            return -1;     
        }
        strncpy(sSHMID, sValue,sizeof(sSHMID)-1);
        memset(sValue, 0x00, sizeof(sValue));
        if (!shmCfg.getValue(oExp, sValue, "app_name", i)) {
            ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed","app_name", i);
            return -1;     
        }
        strncpy(sSHMName, sValue,sizeof(sSHMName)-1);
        if (shmTempCfg.read(oExp, sFileName, sSHMID , '|') == -1) {
            ADD_EXCEPT1(oExp, "ReadCfg::read(%s, \"SYSTEM_MONITOR\", '|') failed","sSHMID");
            return -1;     
        }
        memset(sValue, 0x00, sizeof(sValue));
        if (!shmCfg.getValue(oExp, sValue, "shm_key", i)) {
            ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed","shm_key", i);
            return -1;     
        }
        if (!shmTempCfg.getValue(oExp, iSHMID, sValue,1)) {
            ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\",%d) failed","iSHMID",0);
            return -1;     
        }
        if(iSHMID>0){
            SimpleSHM *m_oSHM= new SimpleSHM(iSHMID, 0);
            if(m_oSHM->open(oExp,false,0600)==0){
                lused = m_oSHM->size();
                sprintf (shm_list_data[i - 1], "0x%08x %-28s  存在     %10d", 
                    iSHMID, sSHMName,lused);
            }else{
                lused = 0;
                sprintf (shm_list_data[i - 1], "0x%08x %-28s  ----     %10d", 
                    iSHMID, sSHMName,0);
            }
            m_oSHM->close ();
            
            if (pMemInfo) {
                pMemInfo[i - 1].m_iSHMID = iSHMID;
                strcpy (pMemInfo[i - 1].m_sIpcName, sSHMID);
                strcpy (pMemInfo[i - 1].m_sSHMName, sSHMName);            
            }
            
            shm_list_index[i - 1] = shm_list_data[i - 1];
        }
    }
    
    g_iSHMListCount = irows;
    
    if (pMemInfo)
        return g_iSHMListCount;
    return g_iSHMListCount;
}


#ifndef _ONLY_USE_SET_SHM_LIST_DATA

int sharedmemory_SysHotKeyPress(Control *pCtl, int ch, long lParam)
{   
    /* Add your self code here: */
    if (ch == 27) { /* ESC */
        return FORM_KILL;
    }

    return ch;
}

void sharedmemory_entry(Control *pHandle) /* sharedmemory 创建时触发调用 */
{   
    sharedmemory_handle = pHandle;
    /* Add your self code here: */

    SetSHMListData ();

    Control *pList = GetCtlByID (sharedmemory_handle, 6);
    pList->iAdditional = g_iSHMListCount;
    pList->pData = shm_list_index;
    
    CtlShow (pList);
    //CtlAtv (pCtl, pCtl);

}

int sharedmemoryCtl1Press(Control *pCtl,int ch,long lParam)
{   /* 刷新button的按键消息处理 */
    /* Add your self code here: */

    if (ch == '\r') {
        SetSHMListData ();
    
        Control *pList = GetCtlByID (sharedmemory_handle, 6);
        pList->iAdditional = g_iSHMListCount;
        pList->pData = shm_list_index;
    
        CtlShow (pList);
        CtlAtv (pCtl, pCtl);
    }

    return ch;
}


int sharedmemoryCtl5Press(Control *pCtl,int ch,long lParam)
{   /* 退出的按键消息处理 */
    /* Add your self code here: */
    if (ch == '\r') {
        mainmenu mm;
        mm.run();
        return FORM_KILL;
    }
    
    return ch;
}
int sharedmemoryCtl6Press(Control *pCtl,int ch,long lParam)
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

#endif
