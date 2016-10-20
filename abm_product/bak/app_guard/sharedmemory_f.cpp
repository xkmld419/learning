/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)sharedmemory_f.c    1.0    2007/04/03    <AutoCreate>" */
#include "ID.h"
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
//#include "UserInfo.h"
#include "SHMData.h"
#include "SHMData_A.h"
#include "SHMData_B.h"
#include "SHMIntHashIndex.h"
#include "SHMIntHashIndex_KR.h"
#include "SHMStringTreeIndex.h"

#include "OfferAccuMgr.h"
#include "AcctItemAccu.h"
#include "ProdOfferAggr.h"
#include "UserInfo.h"

//优惠记录明细数据
#define PROD_OFFER_DETAILBUF_KEY        IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERDISCTBUF")
//优惠数据数据区
#define PROD_OFFER_AGGRBUF_KEY             IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERTOTALBUF")
//优惠数据索引区
#define PROD_OFFER_DISCT_INDEX_KEY     IpcKeyMgr::getIpcKey(-1,"SHM_PRODOFFERDISCTINDEX")



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

#ifdef USERINFO_REUSE_MODE
///打开重使用的模式
const int USERINFO_DATA_MODE=4;
const int USERINFO_INT_INDEX_MODE=6;
const int USERINFO_STR_INDEX_MODE=7;

#else
const int USERINFO_DATA_MODE=1;
const int USERINFO_INT_INDEX_MODE=2;
const int USERINFO_STR_INDEX_MODE=3;

#endif


int SetSHMListData (SharedMemInfo *pMemInfo)
{
    static struct TSHMInfo g_tSHMList[] = {
    
        {CUST_INFO_DATA,             "SHM_CUST_INFO_DATA",             "数据区_客户基本信息",USERINFO_DATA_MODE},
        {CUST_INFO_INDEX,            "SHM_CUST_INFO_INDEX",            "索引区_客户基本信息",USERINFO_INT_INDEX_MODE},
        
        {PRODUCT_INFO_DATA,          "SHM_PRODUCT_INFO_DATA",          "数据区_产品",USERINFO_DATA_MODE},
        {PRODUCT_INFO_INDEX,         "SHM_PRODUCT_INFO_INDEX",         "索引区_产品",USERINFO_INT_INDEX_MODE},
        
        {SERV_INFO_DATA,             "SHM_SERV_INFO_DATA",             "数据区_主产品实例",USERINFO_DATA_MODE},
        {SERV_INFO_INDEX,            "SHM_SERV_INFO_INDEX",            "索引区_主产品实例",USERINFO_INT_INDEX_MODE},
        {SERVLOCATION_INFO_DATA,     "SHM_SERVLOCATION_INFO_DATA",     "数据区_主产品实例地域信息",USERINFO_DATA_MODE},
        //new
        {SERVLOCATION_INFO_INDEX,    "SHM_SERVLOCATION_INFO_INDEX",    "索引区_主产品实例地域信息",USERINFO_INT_INDEX_MODE},
        {SERVSTATE_INFO_DATA,        "SHM_SERVSTATE_INFO_DATA",        "数据区_主产品状态信息",USERINFO_DATA_MODE},
        //new
        {SERVTYPE_INFO_DATA,         "SHM_SERVTYPE_INFO_DATA",         "数据区_主产品类型信息",USERINFO_DATA_MODE},
        {SERVATTR_INFO_DATA,         "SHM_SERVATTR_INFO_DATA",         "数据区_主产品属性信息",USERINFO_DATA_MODE},
        {SERVIDENT_INFO_DATA,        "SHM_SERVIDENT_INFO_DATA",        "数据区_主产品标识",USERINFO_DATA_MODE},
        {SERVIDENT_INFO_INDEX,       "SHM_SERVIDENT_INFO_INDEX",       "索引区_主产品标识",USERINFO_STR_INDEX_MODE},
        
        {SERVPRODUCT_INFO_DATA,      "SHM_SERVPRODUCT_INFO_DATA",      "数据区_附属产品实例",USERINFO_DATA_MODE},
        {SERVPRODUCT_INFO_INDEX,     "SHM_SERVPRODUCT_INFO_INDEX",     "索引区_附属产品实例",USERINFO_INT_INDEX_MODE},
        {SERVPRODUCTATTR_INFO_DATA,  "SHM_SERVPRODUCTATTR_INFO_DATA",  "数据区_附属产品实例属性",USERINFO_DATA_MODE},
        
        {SERVACCT_INFO_DATA,         "SHM_SERVACCT_INFO_DATA",         "数据区_帐务关系",USERINFO_DATA_MODE},
            
        {ACCT_INFO_DATA,             "SHM_ACCT_INFO_DATA",             "数据区_帐户基本信息",USERINFO_DATA_MODE},
        {ACCT_INFO_INDEX,            "SHM_ACCT_INFO_INDEX",            "索引区_帐户基本信息",USERINFO_INT_INDEX_MODE},
            //new
        {SERVACCT_ACCT_INDEX,        "SHM_SERVACCT_ACCT_INDEX",        "索引区_用户支付关系信息",USERINFO_INT_INDEX_MODE},
        {PRODOFFERINS_DATA,          "SHM_PRODOFFERINS_DATA",          "数据区_商品实例信息",USERINFO_DATA_MODE},
        {PRODOFFERINS_INDEX,         "SHM_PRODOFFERINS_INDEX",         "索引区_商品实例信息",USERINFO_INT_INDEX_MODE},
        {OFFERDETAILINS_DATA,        "SHM_OFFERDETAILINS_DATA",        "数据区_商品实例明细信息",USERINFO_DATA_MODE},
        {OFFERDETAILINS_INSIDOFFER_INDEX, "SHM_OFFERDETAILINS_INSIDOFFER_INDEX","索引区_商品实例明细信息",USERINFO_INT_INDEX_MODE},
        {PRODOFFERINSATTR_DATA,      "SHM_PRODOFFERINSATTR_DATA",      "数据区_商品实例属性信息",USERINFO_DATA_MODE},
        //add by zhaoziwei
				{PRODOFFERINSAGREEMENT_DATA, "SHM_PRODOFFERINSATTR_DATA",      "数据区_商品实例协议期信息",USERINFO_DATA_MODE},
				//end
				
        //add by jinx 070412
        //define in OfferAccuMgr.h
#ifdef MEM_REUSE_MODE
        {ACC_DATA_DATA,              "SHM_AccuData",                     "数据区_累积量信息",5},
        {ACC_DATA_INDEX,             "SHM_AccuData_Index_O",             "索引区_累积量1",8},
        {ACC_DATA_S_INDEX,           "SHM_AccuData_Index_S",             "索引区_累积量2",8},
        {ACC_DATA_I_INDEX,           "SHM_AccuData_Index_I",             "索引区_累积量3",8},
#else
        {ACC_DATA_DATA,              "SHM_AccuData",                     "数据区_累积量信息",1},
        {ACC_DATA_INDEX,             "SHM_AccuData_Index_O",             "索引区_累积量1",2},
        {ACC_DATA_S_INDEX,           "SHM_AccuData_Index_S",             "索引区_累积量2",2},
        {ACC_DATA_I_INDEX,           "SHM_AccuData_Index_I",             "索引区_累积量3",2},
#endif


#ifdef MEM_REUSE_MODE
        {ACC_CYCLE_DATA,             "SHM_AccuCycleData",                "数据区_周期实例",5},
        {ACC_CYCLE_INDEX,            "SHM_AccuCycleData_Index",          "索引区_周期实例",8},
#else
        {ACC_CYCLE_DATA,             "SHM_AccuCycleData",                "数据区_周期实例",1},
        {ACC_CYCLE_INDEX,            "SHM_AccuCycleData_Index",          "索引区_周期实例",2},
#endif        

        
        //define in AcctItemAccu.h
#ifdef MEM_REUSE_MODE
        {ACCT_ITEM_DATA,             "SHM_AcctItemData",               "数据区_总账信息",5},
        {ACCT_ITEM_SERV_INDEX,       "SHM_AcctItemIndex_S",            "索引区_总账信息",8},
#else
        {ACCT_ITEM_DATA,             "SHM_AcctItemData",               "数据区_总账信息",1},
        {ACCT_ITEM_SERV_INDEX,       "SHM_AcctItemIndex_S",            "索引区_总账信息",2},
#endif
        
        //define in ProdOfferAggr.h(meminfo.h)
#ifdef MEM_REUSE_MODE
        {PROD_OFFER_DETAILBUF_KEY,   "SHM_PRODOFFERDISCTBUF",          "优惠记录明细数据",5},
        {PROD_OFFER_AGGRBUF_KEY,     "SHM_PRODOFFERTOTALBUF",          "数据区_优惠数据信息",5},
        {PROD_OFFER_DISCT_INDEX_KEY, "SHM_PRODOFFERDISCTINDEX",        "索引区_优惠数据信息",8},
#else
        {PROD_OFFER_DETAILBUF_KEY,   "SHM_PRODOFFERDISCTBUF",          "优惠记录明细数据",1},
        {PROD_OFFER_AGGRBUF_KEY,     "SHM_PRODOFFERTOTALBUF",          "数据区_优惠数据信息",1},
        {PROD_OFFER_DISCT_INDEX_KEY, "SHM_PRODOFFERDISCTINDEX",        "索引区_优惠数据信息",2},
#endif

        
    
        {-1,NULL, NULL,-1}
    };

    int i=0;
    long ltotal =0;
    long lused = 0;
    int iconnnum = 0;
    long llisttol = 0;
    long llistusd = 0;
    int per = 0;
    int listpar = 0;
    
    //MemInfo meminfo;
    
    while (g_tSHMList[i].iSHMID != -1)
    {
        /*
        meminfo.GetMemInfo(g_tSHMList[i].sSHMID, lused, ltotal,iconnnum);
        meminfo.GetStrIdxInfo(g_tSHMList[i].sSHMID, llistusd, llisttol);
        */
        //1:SHMData,2:SHMIntHashIndex,3:SHMStringTreeIndex
        ltotal     = 0;
        lused        = 0;
        llisttol= 0;
        llistusd= 0;
        switch( g_tSHMList[i].iType )
        {
            case 1:
                {
                SHMData<int> sdata(g_tSHMList[i].iSHMID);
                if( sdata.exist() )
                {
                    ltotal     = sdata.getTotal();
                    lused        = sdata.getCount();
                }
                else
                {
                    ltotal     = 0;
                    lused        = 0;                    
                }
                }
                break;
                
            case 2:
                {
                SHMIntHashIndex siidx(g_tSHMList[i].iSHMID);
                if( siidx.exist() )
                {
                    ltotal     = siidx.getTotal();
                    lused        = siidx.getCount();                    
                }
                else
                {
                    ltotal     = 0;
                    lused        = 0;    
                }
                }
                break;
            case 3:
                {
                SHMStringTreeIndex ssidx(g_tSHMList[i].iSHMID);
                if( ssidx.exist() )
                {
                    ltotal     = ssidx.getTotal();
                    lused        = ssidx.getCount();
                    llisttol= ssidx.getlTotal();
                    llistusd= ssidx.getlCount();            
                }
                else
                {
                    ltotal     = 0;
                    lused        = 0;
                    llisttol= 0;
                    llistusd= 0;
                }
                }
                break;
            case 4:
                {
                SHMData_A<int> sdata(g_tSHMList[i].iSHMID);
                if( sdata.exist() )
                {
                    ltotal     = sdata.getTotal();
                    lused        = sdata.getCount();
                }
                else
                {
                    ltotal     = 0;
                    lused        = 0;
                }
                }
                break;
            case 5:
                {
                SHMData_B<int> sdata(g_tSHMList[i].iSHMID);
                if( sdata.exist() )
                {
                    ltotal     = sdata.getTotal();
                    lused        = sdata.getCount();
                }
                else
                {
                    ltotal     = 0;
                    lused        = 0;
                }
                }
                break;

            case 6:
                {
                SHMIntHashIndex_A siidx(g_tSHMList[i].iSHMID);
                if( siidx.exist() )
                {
                    ltotal     = siidx.getTotal();
                    lused        = siidx.getCount();                    
                }
                else
                {
                    ltotal     = 0;
                    lused        = 0;    
                }
                }
                break;
            case 8:
                {
				SHMIntHashIndex_KR ssidx(g_tSHMList[i].iSHMID);
                if( ssidx.exist() )
                {
                    ltotal     = ssidx.getTotal();
                    lused        = ssidx.getCount();
                    llisttol= ssidx.getTotal();
                    llistusd= ssidx.getCount();            
                }
                else
                {
                    ltotal     = 0;
                    lused        = 0;
                    llisttol= 0;
                    llistusd= 0;
                }
                }break;
            case 7:
                {
                SHMStringTreeIndex_A ssidx(g_tSHMList[i].iSHMID);
                if( ssidx.exist() )
                {
                    ltotal     = ssidx.getTotal();
                    lused        = ssidx.getCount();
                    llisttol= ssidx.getlTotal();
                    llistusd= ssidx.getlCount();            
                }
                else
                {
                    ltotal     = 0;
                    lused        = 0;
                    llisttol= 0;
                    llistusd= 0;
                }
                }
                break;
            default:
                {
                ltotal     = 0;
                lused        = 0;    
                llisttol= 0;
                llistusd= 0;
                }
                break;
        }
        
        if (pMemInfo) {
            pMemInfo[i].m_iSHMID = g_tSHMList[i].iSHMID;
            strcpy (pMemInfo[i].m_sIpcName, g_tSHMList[i].sSHMID);
            strcpy (pMemInfo[i].m_sSHMName, g_tSHMList[i].sSHMName);            
        }
        
        if (ltotal)
        {
            per = (lused*100/ltotal);
            if( ((lused*100)%ltotal) > 0 )
                per ++;
            
            if(    llisttol )
            {
                listpar = (int)((llistusd*100/llisttol));
                if( ((llistusd*100)%llisttol) > 0 )
                    listpar ++;            
                sprintf (shm_list_data[i], "0x%06x %-28s存在 %10d|%d%%(%d|%d%%)",
                    g_tSHMList[i].iSHMID, g_tSHMList[i].sSHMName, ltotal,per,llisttol,listpar);
                
                if (pMemInfo) {
                    sprintf (pMemInfo[i].m_sUsedInfo, "%d|%d%%(%d|%d%%)", ltotal,per,llisttol,listpar);
                    pMemInfo[i].m_iUserdPercent  =  per > listpar ? per:listpar;
                }
            }
            else {
                sprintf (shm_list_data[i], "0x%06x %-28s存在 %10d|%-d%%",
                    g_tSHMList[i].iSHMID, g_tSHMList[i].sSHMName, ltotal,per);

                if (pMemInfo) {
                    sprintf (pMemInfo[i].m_sUsedInfo, "%d|%d%%", ltotal,per);
                    pMemInfo[i].m_iUserdPercent  =  per;
                }
            }
            
            if (pMemInfo) {
                strcpy (pMemInfo[i].m_sExistState, "存在");
            }
        } 
        else 
        {
            sprintf (shm_list_data[i], "0x%06x %-28s---- %10d|%-d%%", 
                g_tSHMList[i].iSHMID, g_tSHMList[i].sSHMName,0, 0);
            
            if (pMemInfo) {
                strcpy (pMemInfo[i].m_sExistState, "不存在");
                sprintf (pMemInfo[i].m_sUsedInfo, "%d|%d%%", 0,0);
                pMemInfo[i].m_iUserdPercent = 0;                
            }
        }
        
        shm_list_index[i] = shm_list_data[i];
        i++;
    }
    g_iSHMListCount = i;
    
    if (pMemInfo)
        return g_iSHMListCount;
    
	Environment::getDBConn()->commit();
	    
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
