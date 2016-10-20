#ifndef __CHECK_INFO_STAT_H_HEADER
#define __CHECK_INFO_STAT_H_HEADER

#include "CheckStatInterface.h"
//#include "AcctItemAccu.h"
//#include "AcctItemMgr.h"
//#include "BalanceMgr.h"
#include "KeyTree.h"

//#include "CommandCom.h"
//#include "ThreeLogGroup.h"

#include "ABMException.h"
#include "TimesTenAccess.h"
 
/*class CommandCom
{
	public:
	int readIniInteger(const char *Section, const char *Ident, int defint)
	{
	  return 1;	
	};
};
*/
class ThreeLogGroupMgr
{
	public:
};

#define EVENT_TITLE   "S-KPIID|S-KPIName|S-NotifyID|S-KBP|S-KBPName|S-FirstTime|S-LastTime|S-Event|I-Count"
#define CAUTION_TITLE "S-KPIID|S-KPIName|S-AlarmID|S-KBP|S-KBPName|\
                       S-FirstTime|S-LastTime|I-EventLevel|I-EventState|\
                       S-AckTime|S-AckUser|S-ClearTime|S-ClearUser|I-Count"
                       
#define CAPAB_TITLE "S-KPIID|S-KPIName|S-KBP|S-KBPName|F-Min|F-Avg|F-Max"
#define ALARM_TITLE "S-KPIID|S-KPIName|S-AlarmID|S-KBP|S-KBPName|D-FirstTime|D-LastTime|I-EventLevel|I-EventState|D-AckTime|S-AckUser|D-ClearTime|S-ClearUser|I-Count"

const int NORMAL_TICKET_TYPE_ID = 10;
const int PREP_ERR_TICKET_TYPE_ID = 21;
const int PRICING_ERR_TICKET_TYPE_ID = 22;
const int DUP_TICKET_TYPE_ID = 30;
const int USELESS_TICKET_TYPE_ID = 40;

struct SBusiParam
{
		long m_lTicketNums ;
		int  m_iTicketNumsSwing ;
		long m_lTicketDuration ;
		int  m_iTicketDurationSwing ;
		long m_lTicketAccumulation ;
		int  m_iTicketAccumulationSwing ;
} ;

struct SBusiSUM
{
	  long m_lLCnt ;
   	long m_lDuration ;
   	long m_lAmount ;
};

class TicketLog
{
public:
	TicketLog()
	{
		memset(this, 0, sizeof(TicketLog));
	}	
	int m_iDataSource;
	int m_iSwitchID;
	int m_iBusiClass;
	int m_iTicketTypeID;
	long m_lCurrRecCnt;
	long m_lCurrDuration;
	long m_lCurrAmount;
	long m_lCurrFee;
};

class CheckStatInfo {
  public:
    CheckStatInfo()
    {
    	m_iModuleID=MODULE_CHECK_NORMAL_FILE;
      m_iDelayStatSecs = 300; //默认5分钟
      m_iForwardDays = 31; //先这样搞 以后搞成取账期天数
      memset(m_sProveCode,0,sizeof(m_sProveCode));
      char sValue[4+1]={0};    
   	 if (!ParamDefineMgr::getParam("CHECK_STAT", "PROVINCE_CODE", sValue, -1))
     {
    	 strcpy(sValue,"0");
     }    
   	 strncpy(m_sProveCode, sValue, 4);   
   	 m_iProveNum=atoi(m_sProveCode);   
//     m_poCmdCom = new CommandCom();  
//     m_pThreeLog = new ThreeLogGroupMgr();
     regeditParam();	  	   	 
     //连接timesten
     m_poInfo = NULL;
     connectTT();   
	  }  	
    CheckStatInfo(const int iModuleID)
    {
    	m_iModuleID = iModuleID;
      m_iDelayStatSecs = 300; //默认5分钟
      m_iForwardDays = 31; //先这样搞 以后搞成取账期天数
      memset(m_sProveCode,0,sizeof(m_sProveCode));
      char sValue[4+1]={0};    
   	 if (!ParamDefineMgr::getParam("CHECK_STAT", "PROVINCE_CODE", sValue, -1))
     {
    	 strcpy(sValue,"0");
     }    
   	 strncpy(m_sProveCode, sValue, 4);  
   	 m_iProveNum=atoi(m_sProveCode);    
   	 
	  memset(sValue,0,sizeof(sValue));
   	 if (!ParamDefineMgr::getParam("INFO_DATA", "ACCT_ALARM", sValue, -1))
     {
    	 strcpy(sValue,"0");
     }    
     m_iAcctAlarmLev = atoi(sValue);	 
        	   	       	 
 //     m_poCmdCom = new CommandCom();  
 //     m_pThreeLog = new ThreeLogGroupMgr();      
     regeditParam();	   	        	        
	  }
	  

     	  
	  ~CheckStatInfo() ;
	
	private :
    int m_iModuleID;
    
    int m_iDelayStatSecs; //文件 end 之后延时统计的时间间隔
    
    int m_iForwardDays; //向前退的未稽核文件时间
    
  public:
    int checkNormalFile();
    int checkErrFile();
    int checkPrepBalance();
    int checkPricingBalance();
    int checkInStoreBalance();
    int checkTicketLog();
    int checkTransLog();
    int checkSignHighFee();
    int checkHighFeeServNew();
    int checkHighFeeServ();
    int checkAlertLog();
    int checkDelayFile();
    int checkMouldeCapabilityKpi();
    int checkSystemLossKpi()	;		
    
    void checkParamBalance();
	void checkRoamServs()	;	
	void checkDataRecvBalance(char *sAudit,int iType,char *sFreg);	
	void checkDataLoadBalance(char *sAudit,int iType,char *sFreg);
	void checkDataInputBalance(char *sAudit, int iType, char *sFreg);
	void checkEventInfo();
		
		//new
	int checkHighFeeLog() ;
    void checkMonthCap();
    void checkAddItem() ;  
    void checkAcctCap(char *sAudit,int iType,char *sFreg)   ;  
    void checkAddItemCaution()  ;
    int  checkProcessLog();
    void checkEventInfoNew() ;
    void insertProcessSE(int itype,char *name ,char *time);  
   int checkDayAcctFee(); //add by zhangyz for information	
       
   void checkSystemCaution() ;   
   
   //hss 2011-07
   int  connectTT();
   int checkOutputBalance();
   int checkInstanceUpdate();
   int checkAuthenticationError();
   int checkNoBillingCap();
   int checkHssCap();
   int checkOfcsCap();
   int checkOcsCap();
   

   int checkRecvBalance();
   int checkHSSAlertLog();	
   int checkHSSEventInfo();
   int checkHSSSystemCaution();	
   int checkDataInputBalance();
   
   void checkVCCable(char *sAudit,int iType,char *sFreg);
   void checkSeekInterCap(char *sAudit,int iType,char *sFreg);
  
	void checkSeekInterAlarm(char *sAudit,int iType,char *sFreg);
		
    TimesTenCMD *m_poInfo;	
   
  public:
    long prepMouldeCapability() ;
    long pricingMouldeCapability();
    void insertMouldeCapabilityKpiLog(int iMouldeID, long lValue, const char* sKpiName);
    void updateMouldeCapabilityKpiLog(long lLogID);
    
    bool addTicketLog(TicketLog *pTicketLog, int iDataSource, int iSwitchID, int iBusiClass, int iTicketTypeID,
    					long lEventRec, long lDuration, long lAmount, long lCharge)		;
    					
    bool newTicketLog(const char* sKey, int iDataSource, int iSwitchID, int iBusiClass, int iTicketTypeID,
    					long lEventRec, long lDuration, long lAmount, long lCharge);
    					
    long getMergeRecord(long lFileID);
    long getSplitRecord(long lFileID)		;	
    	
   bool setBillingCycleID(const char* sDate)	  ;
   void upHighFeeRecord(long lLogID) ;   
   void insertCheckLog (long _lFileID, int _iModuleID, const char *_state, bool bCommit=true) ;
   
   void regeditParam()   ;
   void initParam() ;
   	
   int updateMonthCapa(int iLogID);	 
   int updateACCTCapa(int iLogID);   
   void upCautionKipLog(long lLogID) ;
   
  int HbFindErrByFile(char * sErrCode,char * sCause,char * sAction);   	
  void checkAcctCaution(char *sAudit,int iType,char *sFreg) ;
  static bool sortVector(const LogInfoData & m1,const LogInfoData & m2);
      
  private:
    map<string, TicketLog* > m_mapTicketLog;
	  map<string, TicketLog* >::iterator m_iter;    
	  	
	  int m_iBillingCycleID;
	  char m_sCheckDate[14+1];
	  
	private:
      SBusiParam m_sBusiParam[3];
      char m_cBusiDesc[3][4+1];
      
      int m_iRoamInUsers;
      int m_iRoamOutsers;
     // CommandCom *m_poCmdCom;
      //ThreeLogGroupMgr *m_pThreeLog;
      
      char m_sProveCode[4+1];
      int  m_iProveNum;
      int m_iAcctAlarmLev ;//销账告警开关
     
      
};

#endif


