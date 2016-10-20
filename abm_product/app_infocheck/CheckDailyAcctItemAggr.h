#ifndef CHECK_DAILY_ACCT_ITEM_AGGR_H
#define CHECK_DAILY_ACCT_ITEM_AGGR_H

#include <vector>

#include "CheckStatInterface.h"
#include "OrgMgrEx.h"

class TableInfo
{
	public:
		TableInfo():m_iTableType(0), m_iOrgID(0)
		{
			memset(m_sTableName, 0, sizeof (m_sTableName));
		}	
		
		char m_sTableName[51];
		int  m_iTableType;		  
		int  m_iOrgID;
};

class StatResultInfo
{
	public:
		StatResultInfo():m_lCharge(0), m_iAcctItemGroupID(0), 
			             m_iTotalOrInrement(0), m_iBillOrAcct(0),
			             m_lRecord(0),m_iDataSource(0),m_lMonthCharge(0),m_lDailyRecord(0)
		{
			memset(m_sAcctItemDate, 0, sizeof (m_sAcctItemDate));			
		}	
		
		char m_sAcctItemDate[9];
		long m_lCharge;
		long m_lMonthCharge;
		int m_iAcctItemGroupID;
		int m_iTotalOrInrement;//0:增量 1 全量
		int m_iBillOrAcct; //0:清单费用 1 帐单费用
		
		long m_lRecord;
		long m_lDailyRecord;
		int m_iDataSource;
	
};

class MonthStatResultInfo
{
	public:
		MonthStatResultInfo()
		{
			memset(this, 0, sizeof(MonthStatResultInfo));
		}	
		
		int m_iBillingCycleID;
		
		long m_lTicketFee;
		long m_lTicketRecord;
		long m_lCycleFee;
		long m_lCycleRecord;		
		
		long m_lBaiseFee;
		//long m_lBaiseRecord;				
		long m_lDisctFee;
		long m_lDisctRecord;		

		long m_lOffsetFee;
		long m_lOffsetRecord;				
		
		int m_iDataSource;
};

class CheckDailyAcctItemAggr
{
	public:		
        CheckDailyAcctItemAggr();
        ~CheckDailyAcctItemAggr();
        
        int processMain();
        int statBillCharge();
        int statAcctCharge();
        
        //集团日账需求
        int checkDailyAcctItemAggr();
        
        int checkMonthAcctItemAggr();
        
        //add by sunjy 201009 for 2.8 test
        int checkDailyAcctItemAggrNew() ;
        int checkMonthAcctItemAggrNew() ;  	
        	
        
    private:   
    	bool initParam();
    	
		bool getCheckDate(char *sAcctItemDate, char *sLastBackupTime);
		
		void getPreBackupTime();
		    	
    	char m_sThisBackupTime[16];
		char m_sPreBackupTime[16];	 
		char m_sAcctItemDate[9];
		char m_sPreAcctItemDate[9];
		
		char m_sCheckDate[16];
		
	 char m_sProveCode[4+1];
	 int m_iProveNum;
				
	    StatResultInfo *addStatInfoNode(int iAcctItemGroupID, 
	                                   int iTotalOrIncrement = 0,
	                                   int iBillOrAcct = 0,
	                                   int iDataSource = 0);
    	vector<StatResultInfo *> m_poStatInfoBuf;

		void getBillTables();		
		void getBillTables(TOCIDatabase &oTOCIDatabase);
		vector<TableInfo >  m_oBillTableNames;
		vector<TableInfo >  m_oTicketTableNames;
		
		void getBackupTables();		
		vector<TableInfo >  m_oBackupTableNames;
		
		void getAcctTables();
		void getAcctTables(TOCIDatabase &oTOCIDatabase);
		vector<TableInfo >  m_vAcctTableNames;
		
		void setCheckFlagEnd();
		
		bool setBillingCycleID();
		bool setBillingCycleID(const char* sDate);
		int m_iBillingCycleID;	
		
		
		void clearGarbageData(); 		
		void writeDB();
        void writeStatInterface();            		
		
		long getPreAggr(char *sPreAcctItemDate, int iAcctItemGroupID);
		void CalcTotalAggr();
		
		bool m_bStatType;//true 全部全量//false部分清单表 增量
		bool setStatType();
		
	private://集团需求		
		int statDailyAcctAggr(bool bCycleEnd = false);
		int statDailyFromTicket(bool bCycleEnd = false);
		
		int statMonthAcctAggr();
		OrgMgrEx *m_pOrgMgrEx;
		
		char m_sDefaultAreaCode[5];	
			
		int CalcDailyAggr();	
		void loadDailyAggr();
		
		int CalcMonthAggr();	
		int loadMonthAggr();		

		map<string, StatResultInfo*> m_mapDailyAggr;
		map<int, MonthStatResultInfo*> m_mapMonthAggr;
		map<int, MonthStatResultInfo*>::iterator m_mIter;
		
		
		void writeDailyAcctAggrLog();
		void writeMonthAcctAggrLog();
		
		TOCIDatabase m_TOCITicketDBConn;
		void getConnectInfo(char const * sIniHeader,char *sUserName,char *sPasswd,char *sConnStr);		
};

#endif
