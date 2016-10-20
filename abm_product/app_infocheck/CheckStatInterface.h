#ifndef __CHECK_STAT_INTERFACE_H_HEADER_SUN
#define __CHECK_STAT_INTERFACE_H_HEADER_SUN

//#define _JUST_DBLINK_

#define MODULE_CHECK_PREP  1

#include <time.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <signal.h>
#include <map>
#include <fstream>
#include <dirent.h>
#include <string>
#include <vector>

using namespace std;

#include "Log.h"
#include "Environment.h"
#include "ParamDefineMgr.h"
#include "HashList.h"
#include "OrgMgr.h"
#include "EventTypeMgr.h"
#include "AcctItemMgr.h"

///////////////////////////////////////////////////////////////////////////////
//#include "../../config/LocalDefine.h"
#include "../../etc/LocalDefine.h"
#include "../app_json/JsonMgr.h"

#ifndef CHECK_ROOT_ORG_ID
#define CHECK_ROOT_ORG_ID  1
#endif

//03.12, bsn监控要求默认大区标志1
#ifdef NOTUSE_DEFAULT_REGION_DEFINE
#else
    #ifndef CHECK_REGION_DEFINE
    #define CHECK_REGION_DEFINE "1"
    #endif
#endif
///////////////////////////////////////////////////////////////////////////////

#define BOSSNM  "BOSSNM" //代表计费（BOSS）网管
//const int NEID = 3006;//省级计费（BOSS）网管
//const int NEID = 3002;//省级离线计费
const int NEID = 3012;//省级HSS
const int SYS_FLAG = 0;//标识信息点文件是由容灾系统的哪个系统节点产生的
const int DR_ID = 0;//容灾标识
const int MaxLineLength = 100*1024;

const int MODULE_CHECK_NORMAL_FILE = 1000;
const int MODULE_CHECK_ERR_FILE = 2000;
const int MODULE_CHECK_FILE_PREP_BALANCE = 3000;
const int MODULE_CHECK_FILE_PRICING_BALANCE = 4000;
const int MODULE_CHECK_FILE_INSTORE_BALANCE = 5000;
const int MODULE_CHECK_DELAY_FILE =14000;

const int INCEPT_APP_ID = 1;
const int PREP_APP_ID = 2;
const int FORMAT_APP_ID = 3;
const int CHECKDUP_APP_ID = 7;
const int PRICING_APP_ID = 10;
const int MERGER_APP_ID = 32;
//const int INSTORE_APP_ID = 20;

/*
#ifdef DEF_YUNNAN
const int INSTORE_APP_ID = 90001;
#elif DEF_JIANGSU
const int INSTORE_APP_ID = 90001;
#elif DEF_NINGXIA
const char* INSTORE_APP_ID = "20,90001";
#else
const int INSTORE_APP_ID = 20;
#endif
*/
//const char* INSTORE_APP_ID = "20,90001";
#define INSTORE_APP_ID  "20,90001"

const int DISCT_APP_ID = 302;
const int RENT_APP_ID = 5;

const int MAX_CHECK_FILES = 1000;

const int DEFAULT_ORG_LEVEL = 200;


#define  PREP_CAPABILITY_KPI 1000
#define  PREP_CAPABILITY_NAME "预处理每秒处理话单数"

#define  PRICING_CAPABILITY_KPI 2000
#define  PRICING_CAPABILITY_NAME  "批价处理每秒处理话单数"

#define  DISCT_CAPABILITY_KPI 3003
#define  DISCT_CAPABILITY_NAME  "帐期末帐务优惠每秒处理用户数"

#define  RENT_CAPABILITY_KPI  3004
#define  RENT_CAPABILITY_NAME  "租费计算每秒处理用户数"

//add by sunjy
//H1-高频(每15分钟一次)；M1-中频(每1小时一次)；L1-低频（每1天一次）；L2-低频(每月一次)
#define HIGH_FREG "H1" 
#define MIDL_FREG  "M1" 
#define LOW1_FREG "L1" 
#define LOW2_FREG "L2" 
#define SPLIT_FLAG '|'
 
////////////////////////////////////////////////////////////////////////////////

class PlanItem 
{
  public:
    int  m_iPlanID;
    int  m_iAppType;
    char m_sStatName[2049];
    int  m_iIntervalType;
    char m_sTimeInterval[9];
    char m_sNextRunTime[15];
    char m_sParameter[513];
    char m_sState[4];
    int  m_iDebugFlag;
    
    int  m_iDataTimeOffset;
    
    int  m_iDataType;          //## 统计的数据类型    0--动态刷新指标, 1--增量指标
    char m_sDataBeginTime[15]; //## 被统计的数据的起始时间 (闭区间) --统计增量指标时用
    char m_sDataEndTime[15];   //## 被统计的数据的结束时间 (开区间) --统计整理指标时用
    
    char m_sDirectory[1000];   //## 文件信息点生成路径
    
    PlanItem *m_pNext;
    
    PlanItem ()
    {
        m_pNext = NULL;
        //memset(m_sDirectory, 0, sizeof(m_sDirectory));
        memset(this, 0, sizeof(PlanItem));

    }
    ~PlanItem ()
    {
        if (m_pNext) {
            delete m_pNext;
        }
        m_pNext = NULL;
    }
};


class StatInterface {
  public:
    long  lStatID;
    char  sKpiCode[21];
    char  sValue[31];
    int   iState;
    char  sAreaCode[10];
    int   iDealFlag;
    int   iModuleID;
    char  sDimensions[301];
    int   iDimensionFlag;
    int   iStatDateFlag;
    int   iDBSourceID;
    
    char  sCreateDate[15];

    char  sStatBeginDate[15];
    char  sEndDate[15];
    
    static char  m_sDefaultAreaCode[10];
    static PlanItem *m_pPlan;
    
  public:    
  	StatInterface()
  	{
        iState = 1;  iDealFlag = 1;  iModuleID = 1;  
        iDimensionFlag = 1; iStatDateFlag=3;  iDBSourceID = 1;
        
        #ifdef CHECK_REGION_DEFINE
        iDBSourceID = atoi (CHECK_REGION_DEFINE);
        #endif
        
        strcpy(sValue,"0"); 
        sDimensions[0]=0; 
        Date d; 
        d.getTimeString (sStatBeginDate);
        d.getTimeString (sEndDate);
        
        if (m_pPlan) {
            strcpy (sStatBeginDate, m_pPlan->m_sDataBeginTime);
            strcpy (sEndDate, m_pPlan->m_sDataEndTime);
        }
        
        if (!m_sDefaultAreaCode[0]) {
            DEFINE_QUERY (qry);
            qry.close();
            qry.setSQL ("select nvl(min(area_code),'0') from b_area_code "
                        " where long_group_type_id = 10 ");
            qry.open(); qry.next();
            strcpy (m_sDefaultAreaCode, qry.field(0).asString());
            qry.close();
        }
        
        strcpy (sAreaCode, m_sDefaultAreaCode); //必须有真实的
        strcpy (sCreateDate, d.toString());
    } 
     		
    StatInterface (const char *_sKpiCode, const char *_sCreateDate=0) 
    {        
        strcpy (sKpiCode, _sKpiCode);
        
        iState = 1;  iDealFlag = 1;  iModuleID = 1;  
        iDimensionFlag = 1; iStatDateFlag=3;  iDBSourceID = 1;
        
        #ifdef CHECK_REGION_DEFINE
        iDBSourceID = atoi (CHECK_REGION_DEFINE);
        #endif
        
        strcpy(sValue,"0"); 
        sDimensions[0]=0; 
        Date d; 
        d.getTimeString (sStatBeginDate);
        d.getTimeString (sEndDate);
        
        if (m_pPlan) {
            strcpy (sStatBeginDate, m_pPlan->m_sDataBeginTime);
            strcpy (sEndDate, m_pPlan->m_sDataEndTime);
        }
        
        if (!m_sDefaultAreaCode[0]) {
            DEFINE_QUERY (qry);
            qry.close();
            qry.setSQL ("select nvl(min(area_code),'0') from b_area_code "
                        " where long_group_type_id = 10 ");
            qry.open(); qry.next();
            strcpy (m_sDefaultAreaCode, qry.field(0).asString());
            qry.close();
        }
        
        strcpy (sAreaCode, m_sDefaultAreaCode); //必须有真实的
        
        if (_sCreateDate && checkDateString (_sCreateDate))
            strcpy (sCreateDate, _sCreateDate);
        else {
            Date d;
            strcpy (sCreateDate, d.toString());
        }
        
           	 m_iDuplNum=0;
    } 
      
    StatInterface (const char *_sAuditId,int iType,const char *_sFreq)   
    {
	    memset(m_sAuditId,0,sizeof(m_sAuditId)) ; 
	    strncpy(m_sAuditId,_sAuditId,4);
	    m_iType = iType;  
	    memset(m_sFreq,0,sizeof(m_sFreq)) ;   
	    strncpy(m_sFreq,_sFreq,3);

        iState = 1;  iDealFlag = 1;  iModuleID = 1;  
        iDimensionFlag = 1; iStatDateFlag=3;  iDBSourceID = 1;
        
        #ifdef CHECK_REGION_DEFINE
        iDBSourceID = atoi (CHECK_REGION_DEFINE);
        #endif
        
        strcpy(sValue,"0"); 
        sDimensions[0]=0; 
        Date d; 
        d.getTimeString (sStatBeginDate);
        d.getTimeString (sEndDate);
        
        if (m_pPlan) {
            strcpy (sStatBeginDate, m_pPlan->m_sDataBeginTime);
            strcpy (sEndDate, m_pPlan->m_sDataEndTime);
        }
        
        if (!m_sDefaultAreaCode[0]) {
            DEFINE_QUERY (qry);
            qry.close();
            qry.setSQL ("select nvl(min(area_code),'0') from b_area_code "
                        " where long_group_type_id = 10 ");
            qry.open(); qry.next();
            strcpy (m_sDefaultAreaCode, qry.field(0).asString());
            qry.close();
        }
        
        strcpy (sAreaCode, m_sDefaultAreaCode); //必须有真实的
        strcpy (sCreateDate, d.toString());
          m_iDuplNum=0;
          
             
    }
    
    	
public:

   static const char *getSwitchCode (int iSwitchID)
    {
        static map<int, string> *pMapSwitchCode = 0x0;
        if (!pMapSwitchCode) 
        {
            pMapSwitchCode = new map<int, string>();
            
            DEFINE_QUERY (qry);
            qry.setSQL ("select switch_id, nvl(switch_name, '默认交换机') from b_switch_info");
            qry.open();
            while (qry.next())
            {
                (*pMapSwitchCode)[qry.field(0).asInteger()] = string(qry.field(1).asString());
            }
            qry.close();
        }
        
        map<int, string>::iterator iter = pMapSwitchCode->find(iSwitchID);
        if (iter != pMapSwitchCode->end())
            return iter->second.c_str();
        else
            return "默认交换机";
    }	
   static bool checkDateString (const char *dtString)
   {
       //稽核时间合法性
       char sDate[9];
       strncpy (sDate, dtString, 8);
       sDate[8] = 0;
       
       if (checkDate (sDate) < 0)
           return false;
       
       if (checkTime (dtString+8) < 0)
           return false;
       
       return true;
   }

   static int checkDate(const char *pchString)
   {
       int num;
       char tmp[10];
       int iYear,iMonth,i;
   
       if(strlen(pchString) != 8)
           return -1;
       int iLen = strlen(pchString);
       for (i = 0; i < iLen; i++)
       {
           if ((pchString[i] < '0') || (pchString[i] > '9'))
               return -1;
       }
   
       strncpy(tmp , pchString , 4);
       tmp[4] = 0;
       num = atoi(tmp);
       iYear = num;
       if((num < 1900) || (num > 2099))
           return -1;
       strncpy(tmp , pchString + 4 , 2);
       tmp[2] = 0;
       num = atoi(tmp);
       iMonth = num;
       if((num < 1) || (num > 12))
           return -1;
       strncpy(tmp , pchString + 6 , 2);
       tmp[2] = 0;
       num = atoi(tmp);
       if((num < 1) || (num > 31))
           return -1;
       switch(iMonth)
       {
           case 1:
           case 3:
           case 5:
           case 7:
           case 8:
           case 10:
           case 12:
               if((num < 1) || (num > 31))
                   return -1;
               break;
           case 2:
               if((iYear%4==0&&iYear%100!=0)||iYear%400==0) //judge leap year
               {
                   if((num < 1) || (num > 29))
                       return -1;
               }
               else
               {
                   if((num < 1) || (num > 28))
                       return -1;
               }
               break;
           default :
               if((num < 1) || (num > 30))
                   return -1;
       }
       return 1;
   }
   
   static int checkTime(const char *pchString)
   {
       int num,i;
       char tmp[10];
   
       if(strlen(pchString) != 6)
           return -1;
       int iLen = strlen(pchString);
       for (i=0;i<iLen;i++)
       {
           if ((pchString[i]<'0')||(pchString[i]>'9'))
               return -1;
       }
       strncpy(tmp , pchString , 2);
       tmp[2] = 0;
       num = atoi(tmp);
       if((num < 0) || (num > 23))
           return -1;
       strncpy(tmp , pchString + 2 , 2);
       tmp[2] = 0;
       num = atoi(tmp);
       if((num < 0) || (num > 59))
           return -1;
       strncpy(tmp , pchString + 4 , 2);
       tmp[2] = 0;
       num = atoi(tmp);
       if((num < 0) || (num > 59))
           return -1;
   
       return 1;
   }
   
   static void setRunParameter (int iPlanID, const char sParameter[], bool bCommit=false)
   {
       DEFINE_QUERY (qry);
       qry.setSQL ("update B_Check_Stat_Plan "
           " set run_parameter = :vPara "
           " where plan_id = :vPlanID ");
       qry.setParameter ("vPara", sParameter);
       qry.setParameter ("vPlanID", iPlanID);
       qry.execute();
       
       if (bCommit)
           qry.commit();
   }  
   
   static const char *getOrgName (const long lOrgID)
   {
       static HashList<string> *pOrgNameHash = 0x0;
       if (!pOrgNameHash) 
       {
           pOrgNameHash = new HashList<string> (123);
           DEFINE_QUERY (qry);
           qry.setSQL ("select org_id, replace(name,';','') from org");
           qry.open();
           while (qry.next())
           {
               long lKey = qry.field(0).asLong();
               string strName (qry.field(1).asString());
               pOrgNameHash->add (lKey, strName);
           }
           qry.close();
       }
       
       string strTmp;
       if (pOrgNameHash->get (lOrgID, &strTmp))
           return strTmp.c_str();
       else
           return "未知OrgID";
   }   

public :   
   long insert (bool bCommit=false) ;
   long getStatID () ; 
   int getDefaultOrgID();
   void adjustAreaCodeByOrgID (const int iOrgID);
   void getAreaBySwitch (int _iSwitchID, char *_sAreaCode) ;
   int getOrgBySwitch (int _iSwitchID) ;
   int getSwitchByFileID (int _iFileID) ;
   void adjustDimensions ();
   void writeFile(vector<string> &v);
   bool createFile(); 
   bool writeFileHead(long lRecordCnt);
   bool writeFileEnd();
   bool writeFileInfo(const char* sBuf);
   bool setFullFlieName();
   bool setFileDirectory();
   bool setFileDate();
   bool setFileSeq();
   bool setProvinceCode();
   bool setCityCode();   
   bool setFileName();
   bool renameFileName();
   void writeFileLog();
   int getBusiclassBySwitch(int iSwitchID)	;
   int getDataSourceBySwitch(int iSwitchID)	;    
     
   //add by sunjy for 2.8 test
   void writeJsonFile(vector<string> &v,char cTag);
      void ChangeFullFileName(int i ,char *m_sFileName) ;
   void insertFileList(char *sAutid,int iType,char *sFileName) ;  
   
   //add by dongzhi for hss test
   const char *getCityCode();
private:
    char m_sDirectory[1000];
	char m_sFileName[200];
	char m_sFileDate[9];
	int m_iFileSeq;
	long m_lRecord;
	char m_sFullFileName[1200];
	char m_sDesFielPath[1000];
	ofstream m_oOfstream;
	

	

public:		
	char m_sProvinceCode[3+1];
	char m_sCityCode[5];    
	
	//new
	char m_sAuditId[4+1] ; //信息点编码
	int m_iType ;  //信息点类型
	char m_sFreq[3]  ; //信息点频率
	//H1-高频(每15分钟一次)；M1-中频(每1小时一次)；L1-低频（每1天一次）；L2-低频(每月一次)；
	int m_iDuplNum;  // 重传次数
};


///////////////////////////////////////////////////////////////////////////////
/*
class StatMergeItemMgr {
  public:
    StatMergeItemMgr (char *sItemCode)
    {
        m_pNodeHash = 0x0;
        
        DEFINE_QUERY (qry);
        
        char sql[1024];
        char sTemp[512];
        sTemp[0] = 0;
        #ifdef  CHECK_REGION_DEFINE
        sprintf (sTemp, " and nvl(AREA_FLAG,1) = %d ", atoi(CHECK_REGION_DEFINE));
        #endif

        sprintf (sql, "select d.merge_item_id, d.note_id  "
               " from  s_mnt_module a,  "
               "       S_MNT_CODE_MERGE_module_comp b, "
               "       S_MNT_STAT_MERGE_ITEM c, "
               "       S_MNT_MI_ATOM_COMP d "
               " where a.module_id = b.module_id "
               "   and b.merge_item_code = c.merge_item_code "
               "   and c.merge_item_id = d.merge_item_id "
               "   and a.module_code = :vItemCode %s", sTemp);
        
        qry.setSQL (sql);
        qry.setParameter ("vItemCode", sItemCode);
        
        qry.open();
        
        while (qry.next()) {
            if (!m_pNodeHash)
                m_pNodeHash = new HashList<long> (123);
            
            m_pNodeHash->add (qry.field("note_id").asLong(), qry.field("merge_item_id").asLong());
        }
        qry.close();
    }
    
    ~StatMergeItemMgr()
    {
        if (!m_pNodeHash)
            return;
        
        m_pNodeHash->destroy();
        delete m_pNodeHash;
        m_pNodeHash = 0x0;
    }
    
    long getMergeItemID (long lNodeID)
    {
        if (!m_pNodeHash) return 0;
        
        static long lRet;
        
        if (m_pNodeHash->get (lNodeID, &lRet))
            return lRet;
        
        return 0;
    }
    
    long getMergeItemIDByEvtTypeID (const int _iEvtTypeID)
    {
        static EventTypeMgr* s_pEvtMgr = 0x0;
        
        if (!s_pEvtMgr)
            s_pEvtMgr = new EventTypeMgr();
        
        int iEvtTypeID = _iEvtTypeID;
        
        while (iEvtTypeID > 0) {
            long lRet = getMergeItemID (iEvtTypeID);            
            if (lRet > 0) 
                return lRet;
            iEvtTypeID = s_pEvtMgr->getParent(iEvtTypeID);
        }
        return 0;
    }
    
    long getMergeItemIDByOrgID (const int _iOrgID)
    {
        static OrgMgr* s_pOrgMgr = 0x0;
        
        if (!s_pOrgMgr)
            s_pOrgMgr = new OrgMgr();
        
        int iOrgID = _iOrgID;
        
        while (iOrgID > 0) {
            long lRet = getMergeItemID (iOrgID);
            if (lRet > 0)
                return lRet;
            iOrgID = s_pOrgMgr->getParentID(iOrgID);
        }
        return 0;
    }
    
    static const char *getMergeItemName (const long _lMergeItemID, char *sName=0x0)
    {
        static HashList<string>* s_ItemNameHash = 0x0;
        
        if (!s_ItemNameHash) {
            s_ItemNameHash = new HashList<string> (123);
        
            DEFINE_QUERY(qry);
            qry.setSQL ("select a.merge_item_id, replace(a.out_item_name,';','') "
                    " from S_MNT_STAT_MERGE_ITEM a");
            qry.open();
            while (qry.next()) {
                string  oName (qry.field(1).asString());
                s_ItemNameHash->add (qry.field(0).asLong(), oName);
            }
            qry.close();
        }
                
        string oItemName;
        if (s_ItemNameHash->get (_lMergeItemID, &oItemName)) {
            if (sName)
                strcpy (sName, oItemName.c_str());
            return oItemName.c_str();
        }
        
        //return 0x0;  
        return "其他";    
    }
    
  private:
    HashList<long> *m_pNodeHash;
};
*/
///////////////////////////////////////////////////////////////////////

class StatMergeAcctItemMgr {
  public:
    StatMergeAcctItemMgr (char *sItemCode)
    {        
        DEFINE_QUERY (qry);        
        char sql[1024];
        char sTemp[512];
        sTemp[0] = 0;
        #ifdef  CHECK_REGION_DEFINE
        sprintf (sTemp, " and nvl(AREA_FLAG,1) = %d ", atoi(CHECK_REGION_DEFINE));
        #endif

        sprintf (sql, "select d.merge_item_id, d.note_id  "
               " from  s_mnt_module a,  "
               "       S_MNT_CODE_MERGE_module_comp b, "
               "       S_MNT_STAT_MERGE_ITEM c, "
               "       S_MNT_MI_ATOM_COMP d "
               " where a.module_id = b.module_id "
               "   and b.merge_item_code = c.merge_item_code "
               "   and c.merge_item_id = d.merge_item_id "
               "   and a.module_code = :vItemCode %s", sTemp);
        
        qry.setSQL (sql);
        qry.setParameter ("vItemCode", sItemCode);
        qry.open();
        
        while (qry.next())
        {            
            m_oNodeMap[qry.field("note_id").asLong()] = qry.field("merge_item_id").asLong();
        }
        qry.close();
    }
    
    ~StatMergeAcctItemMgr()
    {
    m_oNodeMap.clear();
    }
    
    long getMergeItemID (long lNodeID)
    {
        if (m_oNodeMap.empty()) return 0;
        
        static long lRet;
        
        if (m_oNodeMap.find(lNodeID) != m_oNodeMap.end())
        { 
            lRet = (m_oNodeMap.find(lNodeID))->second;
            return lRet;
        }
        return 0;
    }
    
    long getMergeItemIDByAcctItem (const int _iAcctItemTypeID)
    {
      static AcctItemMgr* s_pAcctItemMgr = 0x0;
      if (!s_pAcctItemMgr)
        s_pAcctItemMgr = new AcctItemMgr();
      
      int iAcctItemTypeID = _iAcctItemTypeID;
      
      map<long, long>::iterator iter;
      
      for (iter = m_oNodeMap.begin(); iter != m_oNodeMap.end(); ++iter)
      {
        if (s_pAcctItemMgr->getBelongGroupB(iAcctItemTypeID, iter->first))
        {
          return getMergeItemID(iter->first);
        } 
      }
      
      return 0;
    }   
        
  private:
    map<long, long> m_oNodeMap;
};


#endif
