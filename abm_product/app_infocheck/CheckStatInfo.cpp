#include "CheckStatInfo.h"
#include "ParamDefineMgr.h"
//#include "AcctItemAccu.h"
//#include "../../config/LocalDefine.h"
#include "../../etc/LocalDefine.h"
#include "ThreeLogGroup.h"
#include "MBC_HSS.h"
#include "HBECodeParse.h"
#include <algorithm>
#include <vector>
using namespace std;


CheckStatInfo::~CheckStatInfo()
{
	
/*	if(m_poCmdCom){
		delete m_poCmdCom;
		m_poCmdCom = NULL;
	}
	if(m_pThreeLog){
		delete m_pThreeLog ;
		m_pThreeLog = NULL ;
	}*/
}
/*
 *	函 数 名 : checkNormalFile
 *	函数功能 : 提取计费接收的正常处理日志,AuditId=0001，Type=50,中频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 :  
*/
int CheckStatInfo::checkNormalFile()
{
    cout <<"CheckStatInfo::checkNormalFile(AuditId=0001，Type=50,中频传送) ..."<<endl;
    
    StatInterface oNormalFileStat("0001",50,MIDL_FREG);     
    vector<string> vNormaFileInfo;
    
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|I-PROV_CODE|I-DATA_SOURCE|I-BATCH_CODE|S-SWITCH_CODE|S-COLLECT_NODE|"
                         "I-BUSI_CLASS|S-FILE_NAME|I-FILE_BLOCK_CNT|I-RECEIVE_CNT|I-DURATION|"
                         "I-VOLUME|I-FEE|S-BILLING_DATE" );
                         
    vNormaFileInfo.push_back(string(tmpbuf));
    long lFileID = 0;
    long lRecordCnt = 0;
    int iSwitchID = 0;
   	char sCreateDate[16] = {0};
   	long lSize = 0;
   	int iBusiClass = 10;
   	int iDataSource = 0;
   	char sFileName[100] = {0};
   	char *p = NULL;
   	
	long lDuration = 0;
	long lAmount = 0;
	long lCharge = 0;   	
   	
   	char sSwitchCode[100] = {0};   	
   	char sLineBuf[MaxLineLength] = {0};
        
    DEFINE_QUERY(qry);
    qry.setSQL ("select file_id,nvl(record_cnt,0),switch_id,file_name, nvl(file_size,0), "
        " to_char(created_date, 'yyyymmddhh24miss') created_date,  nvl(busi_class, 0), nvl(data_source, 0), "
        " nvl(duration, 0), nvl(amount, 0), nvl(charge, 0)"
        " from b_event_file_list_cfg a where created_date>=sysdate-:vForwardDays "
        " and switch_id>0"
        " and state='END'"
        " and state_date < sysdate - :vSec/(3600*24)"
        " and not exists "
        "(select 1 from b_check_file_log c where c.MODULE_ID = :vMod "
        "  and c.file_id=a.file_id )" 
        " and rownum < :vMaxCheckFile "
    );
    
    qry.setParameter ("vForwardDays", m_iForwardDays);            
    qry.setParameter ("vMod", m_iModuleID);
    qry.setParameter ("vSec", m_iDelayStatSecs);
    qry.setParameter ("vMaxCheckFile", MAX_CHECK_FILES);
    
    qry.open ();
    
    while (qry.next()) 
    {
    	lFileID = qry.field(0).asLong();
    	lRecordCnt = qry.field(1).asLong();
    	iSwitchID = qry.field(2).asInteger();
    	 	
    char *pTmp=qry.field(3).asString();
		if ((p = strrchr(qry.field(3).asString(), '/')) == NULL)
		{
			strncpy(sFileName, pTmp, 100);
		}else
		{
			strncpy(sFileName, p+1, 100);
		}
		
		lSize = qry.field(4).asLong();
		
    	memset(sCreateDate, 0, sizeof(sCreateDate));
		strncpy(sCreateDate, qry.field(5).asString(),15);
		

	   	iBusiClass = qry.field(6).asInteger(); 	
	   	
	   	if (iBusiClass == 0)
	   	{
	   		iBusiClass = oNormalFileStat.getBusiclassBySwitch(iSwitchID);
	   	}	
		
    	iDataSource = qry.field(7).asInteger();
    	if (iDataSource == 0)
    	{	
			iDataSource = oNormalFileStat.getDataSourceBySwitch(iSwitchID);
		}
	   	
	   	memset(sSwitchCode, 0, sizeof(sSwitchCode));
	   	strncpy(sSwitchCode, oNormalFileStat.getSwitchCode(iSwitchID), 100);
	   	
	   	lDuration = qry.field(8).asLong();
	   	lAmount = qry.field(9).asLong();
	   	lCharge = qry.field(10).asLong();

    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%d|%d|%ld|%s|%d|%d|%s|%ld|%ld|%ld|%ld|%ld|%s", DR_ID,m_iProveNum, 
    	                   iDataSource, lFileID, sSwitchCode, iSwitchID, iBusiClass, 
    											sFileName, lSize, lRecordCnt, lDuration, lAmount, lCharge, sCreateDate);
    	
    	vNormaFileInfo.push_back(string(sLineBuf));
    	    	
    	insertCheckLog(lFileID, m_iModuleID, "OK");
    }
    
    qry.close ();
    
    //调用接口入库
	  //oNormalFileStat.writeFile(vNormaFileInfo);
    oNormalFileStat.writeJsonFile(vNormaFileInfo,SPLIT_FLAG);
    DB_LINK->commit();
       
    return 0;
}  	

/*
 *	函 数 名 : checkErrFile
 *	函数功能 : 提取计费接收的异常文件和文件延迟到达,AuditId=0002，Type=50,中频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkErrFile()
{
    cout <<"CheckStatInfo::checkErrFile(AuditId=0002，Type=50,中频) ..."<<endl;
    
    StatInterface oErrFileStat("0002",50,MIDL_FREG);
    vector<string> vErrFileInfo;
    
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-DATA_SOURCE|"
                        "I-BATCH_CODE|"
                        "S-SWITCH_CODE|"
                        "S-COLLECT_NODE|"
                        "I-BUSI_CLASS|"
                        "S-FILE_NAME|"
                        "I-FILE_BLOCK_CNT|"
                        "S-CHECK_RESULT|"
                        "S-BILLING_DATE");                        
    vErrFileInfo.push_back(string(tmpbuf));
        
    long lFileID = 0;
    long lRecordCnt = 0;
    int iSwitchID = 0;
   	char sCreateDate[16] = {0};
   	long lSize = 0;
   	int iBusiClass = 10;
   	int iDataSource = 0;
   	char sFileName[100] = {0};
   	char *p = NULL;
   	
   	char sSwitchCode[100] = {0};   	   	
   	char sLineBuf[MaxLineLength] = {0};
        
    DEFINE_QUERY(qry);
    qry.setSQL ("select file_id,nvl(record_cnt,0),switch_id,file_name, nvl(file_size,0), "
        " to_char(created_date, 'yyyymmddhh24miss') created_date, nvl(busi_class, 0), nvl(data_source, 0) "
        " from b_event_file_list_cfg a where created_date>=sysdate-:vForwardDays "
        " and switch_id>0"
        " and state='ERR'"
        " and state_date < sysdate - :vSec/(3600*24)"
        " and not exists "
        "(select 1 from b_check_file_log c where c.MODULE_ID = :vMod "
        "  and c.file_id=a.file_id )" 
        " and rownum < :vMaxCheckFile "
    );
    
    qry.setParameter ("vForwardDays", m_iForwardDays);        
    qry.setParameter ("vMod", m_iModuleID);
    qry.setParameter ("vSec", m_iDelayStatSecs);
    qry.setParameter ("vMaxCheckFile", MAX_CHECK_FILES);
    
    qry.open ();
    
    while (qry.next()) 
    {
    	lFileID = qry.field(0).asLong();
    	lRecordCnt = qry.field(1).asLong();
    	iSwitchID = qry.field(2).asInteger();

		if ((p = strrchr(qry.field(3).asString(), '/')) == NULL)
		{
			strncpy(sFileName, p, 100);
		}else
		{
			strncpy(sFileName, p+1, 100);
		}
		
		lSize = qry.field(4).asLong();
		
    	memset(sCreateDate, 0, sizeof(sCreateDate));
		strncpy(sCreateDate, qry.field(5).asString(),15);


	   	if (iBusiClass == 0)
	   	{
	   		iBusiClass = oErrFileStat.getBusiclassBySwitch(iSwitchID);
	   	}	

    	iDataSource = qry.field(7).asInteger();
    	if (iDataSource == 0)
    	{	
			iDataSource = oErrFileStat.getDataSourceBySwitch(iSwitchID);
		}
	   	
	   	memset(sSwitchCode, 0, sizeof(sSwitchCode));
	   	strncpy(sSwitchCode, oErrFileStat.getSwitchCode(iSwitchID), 100);
	 
	   	
    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%d|%d|%ld|%s|%d|%d|%s|%ld|ERR|%s", DR_ID,m_iProveNum, iDataSource, lFileID, sSwitchCode, iSwitchID, iBusiClass, 
    											sFileName, lSize, sCreateDate);
    	
    	vErrFileInfo.push_back(string(sLineBuf));
    	    	
    	insertCheckLog(lFileID, m_iModuleID, "OK");
    }
    
    qry.close ();
    
    //调用接口入库
	  //oErrFileStat.writeFile(vErrFileInfo);
    oErrFileStat.writeJsonFile(vErrFileInfo,SPLIT_FLAG);
    DB_LINK->commit();
       
    return 0;
}

/*
 *	函 数 名 : checkPrepBalance
 *	函数功能 : 提取计费预处理的输入和输出，主要用于预处理的准确性校验,AuditId=0003，Type=50,中频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkPrepBalance()
{
    cout <<"CheckStatInfo::checkPrepBalance(AuditId=0003，Type=50,中频) ..."<<endl;
    
    StatInterface oCheckPrepBalance("0003",50,MIDL_FREG);   
    vector<string> vPrepBalance;
    vPrepBalance.clear();
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-DATA_SOURCE|"
                        "I-BATCH_CODE|"
                        "S-SWITCH_CODE|"
                        "I-BUSI_CLASS|"
                        "I-IN_RECORD_CNT|"
                        "I-OUT_OK_CNT|"
                        "I-OUT_ERR_CNT|"
                        "I-OUT_DUPL_CNT|"
                        "I-OUT_MERG_CNT|"
                        "I-OUT_SPLIT_CNT|"
                        "I-OUT_NBILL_CNT|"
                        "I-OUT_IGN_CNT|"
                        "S-END_TIME");                        
    vPrepBalance.push_back(string(tmpbuf));  
      
    long lFileID = 0;
    long lInRecord = 0;
    int iSwitchID = 0;
   	int iBusiClass = 10;
   	int iDataSource = 0;
   	
   	long lOutNormalRecord = 0;
   	long lOutErrRecord = 0;
   	long lOutDupRecord = 0;
   	long lOutMergRecord = 0;
   	long lOutSplitRecord = 0;
   	long lOutNbillRecord = 0;
   	long lOutIgnRecord = 0;
   	
   	char sEndDate[16] = {0};
   	
   	char sSwitchCode[100] = {0};
   	char sLineBuf[MaxLineLength] = {0};
        
    DEFINE_QUERY(qry);
    qry.setSQL ("select a.file_id, nvl(record_cnt,0), switch_id, nvl(busi_class, 0), nvl(data_source, 0)"
        " from b_event_file_list_cfg a where a.created_date>=sysdate-:vForwardDays "
        " and a.switch_id>0"
        " and a.state='END'"
        " and a.state_date < sysdate - :vSec/(3600*24)"
        " and not exists "
        	"(select 1 from b_check_file_log c where c.MODULE_ID = :vMod "
        "  and c.file_id=a.file_id )" 
        " and rownum < :vMaxCheckFile "
    );
    
    qry.setParameter ("vForwardDays", m_iForwardDays);    
    qry.setParameter ("vMod", m_iModuleID);
    qry.setParameter ("vSec", m_iDelayStatSecs);
    qry.setParameter ("vMaxCheckFile", MAX_CHECK_FILES);
    
    qry.open ();
    
    while (qry.next()) 
    {
    	lFileID = qry.field(0).asLong();
    	lInRecord = qry.field(1).asLong();
    	iSwitchID = qry.field(2).asInteger();
	   	iBusiClass = qry.field(3).asInteger();
	   	
	   	if (iBusiClass == 0)
	   	{
	   		iBusiClass = oCheckPrepBalance.getBusiclassBySwitch(iSwitchID);
	   	}	
	   	 	
    	iDataSource = qry.field(4).asInteger();
    	if (iDataSource == 0)
    	{	
			iDataSource = oCheckPrepBalance.getDataSourceBySwitch(iSwitchID);
		}	
	   	
	   	lOutSplitRecord = getSplitRecord(lFileID);
	   	lOutMergRecord = getMergeRecord(lFileID);
	   	
	   	DEFINE_QUERY(qry1);
	   	int iCnt = 0;
	   	qry1.setSQL(" select sum(t.out_normal_event),sum(t.out_black_event+t.out_pre_err_event+t.out_pric_err_event+t.out_format_err) out_err_event, "
       				" sum(t.out_dup_enent), sum(t.out_useless_event), max(trim(msg_date)||trim(msg_time)) end_date "
 					" from wf_stat_log t where t.file_id=:file_id "
					" and t.process_id in (select a.process_id from wf_process a where a.app_id = :dup_app_id) "
					" group by file_id ");
					
    	qry1.setParameter ("file_id", lFileID);    
    	qry1.setParameter ("dup_app_id", CHECKDUP_APP_ID);					
					
		qry1.open();
		
		while(qry1.next())
		{
			iCnt++;
			lOutNormalRecord =  qry1.field(0).asLong();
			lOutErrRecord = qry1.field(1).asLong();
			lOutDupRecord = qry1.field(2).asLong();
			lOutNbillRecord = qry1.field(3).asLong();
			memset(sEndDate,0,sizeof(sEndDate));
			strcpy(sEndDate, qry1.field(4).asString());
		}
		
		qry1.close();
				
		if (iCnt == 0)
		{
	   		DEFINE_QUERY(qry1);
	   		qry1.setSQL(" select sum(t.out_normal_event),sum(t.out_black_event+t.out_pre_err_event+t.out_pric_err_event+t.out_format_err) out_err_event, "
       					" sum(t.out_dup_enent), sum(t.out_useless_event), max(trim(msg_date)||trim(msg_time)) end_date "
 						" from wf_stat_log t where t.file_id=:file_id "
						" and t.process_id in (select a.process_id from wf_process a where a.app_id = :format_app_id) "
						" group by file_id ");
						
    		qry1.setParameter ("file_id", lFileID);    
    		qry1.setParameter ("format_app_id", FORMAT_APP_ID);					
						
			qry1.open();
			
			while(qry1.next())
			{
				iCnt++;		
				lOutNormalRecord =  qry1.field(0).asLong();
				lOutErrRecord = qry1.field(1).asLong();
				lOutDupRecord = qry1.field(2).asLong();
				lOutNbillRecord = qry1.field(3).asLong();
				strcpy(sEndDate, qry1.field(4).asString());				
			}
			
			qry1.close();			
		}	
		
		if (iCnt == 0)
		{
	   		DEFINE_QUERY(qry1);
	   		qry1.setSQL(" select sum(t.out_normal_event),sum(t.out_black_event+t.out_pre_err_event+t.out_pric_err_event+t.out_format_err) out_err_event, "
       					" sum(t.out_dup_enent), sum(t.out_useless_event), max(trim(msg_date)||trim(msg_time)) end_date "
 						" from wf_stat_log t where t.file_id=:file_id "
						" and t.process_id in (select a.process_id from wf_process a where a.app_id = :prep_app_id) "
						" group by file_id ");
						
    		qry1.setParameter ("file_id", lFileID);    
    		qry1.setParameter ("prep_app_id", PREP_APP_ID);					
						
			qry1.open();
			
			while(qry1.next())
			{
				iCnt++;		
				lOutNormalRecord =  qry1.field(0).asLong();
				lOutErrRecord = qry1.field(1).asLong();
				lOutDupRecord = qry1.field(2).asLong();
				lOutNbillRecord = qry1.field(3).asLong();
				strcpy(sEndDate, qry1.field(4).asString());				
			}
			
			qry1.close();			
		}		
		
		if (iCnt == 0)//理论上肯定不会有，格式化完才会置0
			continue;			
	   	
	   	memset(sSwitchCode, 0, sizeof(sSwitchCode));
	   	strncpy(sSwitchCode, oCheckPrepBalance.getSwitchCode(iSwitchID),100);
	   	
    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%d|%d|%ld|%s|%d|%ld|%ld|%ld|%ld|%ld|%ld|%ld|%ld|%s", 
    	                   DR_ID,m_iProveNum, iDataSource, lFileID, sSwitchCode, iBusiClass, 
    											lInRecord, lOutNormalRecord, lOutErrRecord, lOutDupRecord, lOutMergRecord, lOutSplitRecord,
    											lOutNbillRecord, lOutIgnRecord, sEndDate);
    	cout<<sLineBuf<<endl;
    	vPrepBalance.push_back(sLineBuf);
    	    	
    	insertCheckLog(lFileID, m_iModuleID, "OK");
    }
    
    qry.close ();
    
    //调用接口写文件
   // oCheckPrepBalance.writeFile(vPrepBalance);
    oCheckPrepBalance.writeJsonFile(vPrepBalance,SPLIT_FLAG);
    DB_LINK->commit();
       
    return 0;	
}

/*
 *	函 数 名 : checkPricingBalance
 *	函数功能 : 提取计费记录批价处理的输入和输出，主要用于批价处理的准确性校验,AuditId=0004，Type=50,中频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkPricingBalance()
{
    cout <<"CheckStatInfo::checkPricingBalance(AuditId=0004，Type=50,中频) ..."<<endl;
    
    StatInterface oCheckPricingBalance("0004",50,MIDL_FREG);    
    vector<string> vPricingBalance;
    vPricingBalance.clear();
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                   "S-PROV_CODE|"
                   "I-DATA_SOURCE|"
                   "I-BATCH_CODE|"
                   "S-SWITCH_CODE|"
                   "I-BUSI_CLASS|"
                   "I-IN_RECORD_CNT|"
                   "I-OUT_OK_CNT|"
                   "I-OUT_ERR_CNT|"
                   "S-END_TIME");                        
    vPricingBalance.push_back(string(tmpbuf));
    
    long lFileID = 0;
    int iDataSource = 0;
    int iSwitchID = 0;        
   	int iBusiClass = 10;
   	
   	long lInRecord = 0;
   	long lOutNormalRecord = 0;
   	long lOutErrRecord = 0;
   	
   	char sSwitchCode[100] = {0};   	
   	char sLineBuf[MaxLineLength] = {0};
        
    DEFINE_QUERY(qry);
    qry.setSQL ("select a.file_id, max(nvl(a.data_source,0)) data_source, max(switch_id) switch_id, max(nvl(busi_class,0)) busi_class, "
        " sum(b.in_normal_event+b.in_pric_err_event) in_record, "
        " sum(b.out_normal_event) out_normal_event, sum(b.out_pric_err_event) out_pric_err_event"
        " from b_event_file_list_cfg a, wf_stat_log b where created_date>=sysdate-:vForwardDays "
        " and a.switch_id>0"
        " and a.state='END'"
        " and a.state_date < sysdate - :vSec/(3600*24)"
        " and not exists "
        	"(select 1 from b_check_file_log c where c.MODULE_ID = :vMod "
        "  and c.file_id=a.file_id )"
        " and a.file_id = b.file_id "
        " and b.process_id in (select process_id from wf_process where app_id = :vPricing_app_id)"
        " group by a.file_id "
    );
    
    qry.setParameter ("vForwardDays", m_iForwardDays);    
    qry.setParameter ("vMod", m_iModuleID);
    qry.setParameter ("vSec", m_iDelayStatSecs);
    qry.setParameter ("vPricing_app_id", PRICING_APP_ID);
    qry.open ();
    
    while (qry.next()) 
    {
    	lFileID = qry.field(0).asLong();
    	iDataSource = qry.field(1).asInteger();
    	iSwitchID = qry.field(2).asInteger();
    	iBusiClass = qry.field(3).asInteger();
    	
	   	if (iBusiClass == 0)
	   	{
	   		iBusiClass = oCheckPricingBalance.getBusiclassBySwitch(iSwitchID);
	   	}	
	   	 	
    	if (iDataSource == 0)
    	{	
			iDataSource = oCheckPricingBalance.getDataSourceBySwitch(iSwitchID);
		}	    	
    	
		lInRecord = qry.field(4).asLong();
	   	
		lOutNormalRecord = qry.field(5).asLong();
		lOutErrRecord = qry.field(6).asLong();
	   	
	   	if (lInRecord != lOutNormalRecord+lOutErrRecord)
	   	{
	   		//是否本地处理掉
	   	}	
	   	
	   	memset(sSwitchCode, 0, sizeof(sSwitchCode));
	   	strncpy(sSwitchCode, oCheckPricingBalance.getSwitchCode(iSwitchID),100);	   	
	   	
    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%d|%d|%ld|%s|%d|%ld|%ld|%ld|%s", DR_ID, m_iProveNum,iDataSource, lFileID, sSwitchCode, iBusiClass, 
    											lInRecord, lOutNormalRecord, lOutErrRecord, oCheckPricingBalance.sEndDate);
    	
    	vPricingBalance.push_back(string(sLineBuf));
    	    	
    	insertCheckLog(lFileID, m_iModuleID, "OK");
    	
    	if(vPricingBalance.size() >= MAX_CHECK_FILES)
    		break;	
    }
    
    qry.close ();
    
    //调用接口写文件
    //oCheckPricingBalance.writeFile(vPricingBalance);
    oCheckPricingBalance.writeJsonFile(vPricingBalance,SPLIT_FLAG);
    
    DB_LINK->commit();
       
    return 0;	
}

/*
 *	函 数 名 : checkInStoreBalance
 *	函数功能 : 提取记录入库处理的输入和输出，主要用于入库处理的准确性校验,AuditId=0005，Type=50,中频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkInStoreBalance()
{
    cout <<"CheckStatInfo::checkInStoreBalance(AuditId=0005，Type=50,中频) ..."<<endl;
    
    StatInterface oCheckInStoreBalance("0005",50,MIDL_FREG);    
    vector<string> vInStoreBalance;
    vInStoreBalance.clear();
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-DATA_SOURCE|"
                        "I-BATCH_CODE|"
                        "S-SWITCH_CODE|"
                        "I-BUSI_CLASS|"
                        "I-IN_RECORD_CNT|"
                        "I-OUT_OK_CNT|"
                        "I-OUT_ERR_CNT|"
                        "I-OUT_DUPL_CNT|"
                        "I-OUT_NBILL_CNT|"
                        "I-OUT_SETTERR_CNT|"
                        "I-OUT_ERR_CNT|"
                        "S-END_TIME");                        
    vInStoreBalance.push_back(string(tmpbuf));
    
    long lFileID = 0;
    int iDataSource = 0;
    int iSwitchID = 0;        
   	int iBusiClass = 10;
   	
   	long lInRecord = 0;
   	
   	long lOutNormalRecord = 0;
   	long lOutPrepErrRecord = 0;
	  long lOutDupRecord = 0;   	   	
   	long lOutUseLess = 0;
   	long lOutPricErrRecord = 0;
   	long lOutInStoreErrRecord = 0;
   	
   	char sSwitchCode[100] = {0};   	
   	char sLineBuf[MaxLineLength] = {0};
        
    DEFINE_QUERY(qry);

    char sSql[2048]={0};
    sprintf(sSql,"select a.file_id, max(nvl(a.data_source,0)) data_source, max(switch_id) switch_id, max(nvl(busi_class,0)) busi_class, "
        " sum(b.in_normal_event+b.in_black_event+b.in_dup_event+b.in_pre_err_event+b.in_useless_event+b.in_pric_err_event) in_record, "
		    " sum(b.out_normal_event) out_normal_event,sum(b.out_format_err+b.out_pre_err_event) out_pre_err_event, "
		    " sum(b.out_dup_enent) out_dup_enent, sum(b.out_useless_event) out_useless_event, sum(b.out_pric_err_event+out_black_event) out_pric_err_event"        
        " from b_event_file_list_cfg a, wf_stat_log b where created_date>=sysdate-%d "
        " and a.switch_id>0"
        " and a.state='END'"
        " and a.state_date < sysdate - %d/(3600*24)"
        " and not exists "
        	"(select 1 from b_check_file_log c where c.MODULE_ID = %d "
        "  and c.file_id=a.file_id )"
        " and a.file_id = b.file_id "
		    " and b.process_id in (select process_id from wf_process where app_id in (%s)) "
        " group by a.file_id " , m_iForwardDays ,m_iDelayStatSecs,m_iModuleID,INSTORE_APP_ID);
    qry.setSQL(sSql);

   /* 
    qry.setSQL ("select a.file_id, max(nvl(a.data_source,0)) data_source, max(switch_id) switch_id, max(nvl(busi_class,0)) busi_class, "
        " sum(b.in_normal_event+b.in_black_event+b.in_dup_event+b.in_pre_err_event+b.in_useless_event+b.in_pric_err_event) in_record, "
		    " sum(b.out_normal_event) out_normal_event,sum(b.out_format_err+b.out_pre_err_event) out_pre_err_event, "
		    " sum(b.out_dup_enent) out_dup_enent, sum(b.out_useless_event) out_useless_event, sum(b.out_pric_err_event+out_black_event) out_pric_err_event"        
        " from b_event_file_list_cfg a, wf_stat_log b where created_date>=sysdate-:vForwardDays "
        " and a.switch_id>0"
        " and a.state='END'"
        " and a.state_date < sysdate - :vSec/(3600*24)"
        " and not exists "
        	"(select 1 from b_check_file_log c where c.MODULE_ID = :vMod "
        "  and c.file_id=a.file_id )"
        " and a.file_id = b.file_id "
		    " and b.process_id in (select process_id from wf_process where app_id in (:vInstore_app_id)) "
        " group by a.file_id "        
    );
    
    qry.setParameter ("vForwardDays", m_iForwardDays);        
    qry.setParameter ("vMod", m_iModuleID);
    qry.setParameter ("vSec", m_iDelayStatSecs);
    qry.setParameter ("vInstore_app_id", INSTORE_APP_ID);
    */

    qry.open ();
    
    while (qry.next()) 
    {
    	lFileID = qry.field(0).asLong();
    	iDataSource = qry.field(1).asInteger();
    	iSwitchID = qry.field(2).asInteger();
    	iBusiClass = qry.field(3).asInteger();
    	
	   	if (iBusiClass == 0)
	   	{
	   		iBusiClass = oCheckInStoreBalance.getBusiclassBySwitch(iSwitchID);
	   	}	
	   	 	
    	if (iDataSource == 0)
    	{	
			iDataSource = oCheckInStoreBalance.getDataSourceBySwitch(iSwitchID);
		}	    	
    	
		lInRecord = qry.field(4).asLong();
	   	
		lOutNormalRecord = qry.field(5).asLong();
		lOutPrepErrRecord = qry.field(6).asLong();
		lOutDupRecord = qry.field(7).asLong();
		lOutUseLess = qry.field(8).asLong();
		lOutPricErrRecord = qry.field(9).asLong();
		
		lOutInStoreErrRecord = 0;
		
	   	memset(sSwitchCode, 0, sizeof(sSwitchCode));
	   	strncpy(sSwitchCode, oCheckInStoreBalance.getSwitchCode(iSwitchID),100);	   			
		
    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%d|%d|%ld|%s|%d|%ld|%ld|%ld|%ld|%ld|%ld|%ld|%s", 
    	                    DR_ID, m_iProveNum,iDataSource, lFileID, sSwitchCode, iBusiClass, 
    											lInRecord, lOutNormalRecord, lOutPrepErrRecord,
    											lOutDupRecord, lOutUseLess, lOutPricErrRecord,
    											lOutInStoreErrRecord, oCheckInStoreBalance.sEndDate);
    	
    	vInStoreBalance.push_back(string(sLineBuf));
    	    	
    	insertCheckLog(lFileID, m_iModuleID, "OK");
    	
    	if(vInStoreBalance.size() >= MAX_CHECK_FILES)
    		break;	    	
    }
    
    qry.close ();
    
    //调用接口写文件
   // oCheckInStoreBalance.writeFile(vInStoreBalance);
    oCheckInStoreBalance.writeJsonFile(vInStoreBalance,SPLIT_FLAG);
    DB_LINK->commit();
       
    return 0;	
}


/*
 *	函 数 名 : checkTicketLog
 *	函数功能 : 提取记录入库处理后各种话务量情况,AuditId=0006，Type=50,中频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkTicketLog()
{
  cout <<"CheckTicketLog::checkTicketLog(AuditId=0006，Type=50,中频) ..."<<endl;
    
	StatInterface oCheckTicketLog("0006",50,MIDL_FREG);
	vector<string> vCheckTicketLog;
	vCheckTicketLog.clear();
	  char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-DATA_SOURCE|"
                        "S-SWITCH_CODE|"
                        "I-BUSI_CLASS|"
                        "I-TICKET_TYPE_ID|"
                        "I-TICKET_TYPE|"
                        "I-CURR_REC_CNT|"
                        "I-CURR_AMOUNT|"
                        "I-VOLUME_AMOUNT|"
                        "I-CURR_FEE|"
                        "S-STAT_DATE");                        
    vCheckTicketLog.push_back(string(tmpbuf));  
	int iFileID = 0;
	int iProcessID = 0;
	int iDataSource = 0;
	int iSwitchID = 0;
   	int iBusiClass = 10;
   	
   	char sKey[100] = {0};
   	
	long lNormalEvent = 0;
	long lPricingErrEvent = 0;
	long lDupEvent = 0;
	long lPrepEvent = 0;
	long lUselessEvent = 0;
		
	DEFINE_QUERY(qry);
	

   char sSql[2048]={0};
   sprintf(sSql,"select t.file_id, t.process_id, b.switch_id, nvl(b.busi_class, 0), nvl(b.data_source, 0), "
   				" t.out_normal_event, nvl(t.normal_duration, 0), nvl(t.normal_amount, 0), nvl(t.normal_charge, 0), "
   				" (t.out_black_event+t.out_pric_err_event) pric_err_event, nvl(t.black_duration+pricerr_duration, 0) pricerr_duration, nvl(t.black_amount+pricerr_amount, 0) pricerr_amount, nvl(t.black_charge+pricerr_charge, 0) pricerr_charge, "
   				" t.out_dup_enent, nvl(t.dup_duration, 0), nvl(t.dup_amount, 0), nvl(t.dup_charge, 0), "
   				" (t.out_pre_err_event+t.out_format_err) pre_err_event, nvl(t.preerr_duration+formaterr_duration, 0) preerr_duration, nvl(t.preerr_amount+formaterr_amount, 0) preerr_amount, nvl(t.preerr_charge+formaterr_charge, 0) preerr_charge, "
   				" t.out_useless_event, nvl(t.useless_duration, 0), nvl(t.useless_amount, 0), nvl(t.useless_charge, 0) "
				" from wf_stat_log t,b_event_file_list_cfg b "
				" where to_date(trim(t.msg_date)||trim(t.msg_time), 'yyyymmddhh24miss') between to_date(%s, 'yyyymmddhh24miss')+ 1/(24*3600) and to_date(%s, 'yyyymmddhh24miss') "
				" and t.file_id=b.file_id"
				" and exists "
				" (select 1 from wf_process a "
				" where t.process_id=a.process_id "
				" and a.app_id in (%s)) ",oCheckTicketLog.sStatBeginDate,oCheckTicketLog.sEndDate,INSTORE_APP_ID);
		qry.setSQL(sSql);

/*
	qry.setSQL(" select t.file_id, t.process_id, b.switch_id, nvl(b.busi_class, 0), nvl(b.data_source, 0), "
   				" t.out_normal_event, nvl(t.normal_duration, 0), nvl(t.normal_amount, 0), nvl(t.normal_charge, 0), "
   				" (t.out_black_event+t.out_pric_err_event) pric_err_event, nvl(t.black_duration+pricerr_duration, 0) pricerr_duration, nvl(t.black_amount+pricerr_amount, 0) pricerr_amount, nvl(t.black_charge+pricerr_charge, 0) pricerr_charge, "
   				" t.out_dup_enent, nvl(t.dup_duration, 0), nvl(t.dup_amount, 0), nvl(t.dup_charge, 0), "
   				" (t.out_pre_err_event+t.out_format_err) pre_err_event, nvl(t.preerr_duration+formaterr_duration, 0) preerr_duration, nvl(t.preerr_amount+formaterr_amount, 0) preerr_amount, nvl(t.preerr_charge+formaterr_charge, 0) preerr_charge, "
   				" t.out_useless_event, nvl(t.useless_duration, 0), nvl(t.useless_amount, 0), nvl(t.useless_charge, 0) "
				" from wf_stat_log t,b_event_file_list_cfg b "
				" where to_date(trim(t.msg_date)||trim(t.msg_time), 'yyyymmddhh24miss') between to_date(:statBeginDate, 'yyyymmddhh24miss')+ 1/(24*3600) and to_date(:statEndDate, 'yyyymmddhh24miss') "
				" and t.file_id=b.file_id"
				" and exists "
				" (select 1 from wf_process a "
				" where t.process_id=a.process_id "
				" and a.app_id in (:instore_app_id)) ");
				 
	qry.setParameter("statBeginDate" ,oCheckTicketLog.sStatBeginDate);
	qry.setParameter("statEndDate" ,oCheckTicketLog.sEndDate);
	qry.setParameter("instore_app_id", INSTORE_APP_ID);
  */
	
	qry.open();
	
	while(qry.next())
	{
		iFileID = qry.field(0).asInteger();
		iProcessID = qry.field(1).asInteger();
		
		iSwitchID = qry.field(2).asInteger();
		iBusiClass = qry.field(3).asInteger();
		iDataSource = qry.field(4).asInteger();
		
	   	if (iBusiClass == 0)
	   	{
	   		iBusiClass = oCheckTicketLog.getBusiclassBySwitch(iSwitchID);
	   	}	
	   	 	
    	if (iDataSource == 0)
    	{	
			iDataSource = oCheckTicketLog.getDataSourceBySwitch(iSwitchID);
		}			
		
		lNormalEvent = qry.field(5).asLong();
		
		if (lNormalEvent > 0)
		{
			memset(sKey, 0, sizeof(sKey));
			sprintf(sKey, "%d_%d_%d_%d", iDataSource, iSwitchID, iBusiClass, NORMAL_TICKET_TYPE_ID);
			
			m_iter = m_mapTicketLog.find(string(sKey));
			if (m_iter == m_mapTicketLog.end())
			{
				newTicketLog(sKey, iDataSource, iSwitchID, iBusiClass, NORMAL_TICKET_TYPE_ID,
					 			lNormalEvent, qry.field(6).asLong(), qry.field(7).asLong(), qry.field(8).asLong());			
			}else
			{
				addTicketLog(m_iter->second, iDataSource, iSwitchID, iBusiClass, NORMAL_TICKET_TYPE_ID,
					 			lNormalEvent, qry.field(6).asLong(), qry.field(7).asLong(), qry.field(8).asLong());					
			}	
		}
		
		lPricingErrEvent = qry.field(9).asLong();
		
		if (lPricingErrEvent > 0)
		{
			memset(sKey, 0, sizeof(sKey));
			sprintf(sKey, "%d_%d_%d_%d", iDataSource, iSwitchID, iBusiClass, PRICING_ERR_TICKET_TYPE_ID);
			
			m_iter = m_mapTicketLog.find(string(sKey));
			if (m_iter == m_mapTicketLog.end())
			{
				newTicketLog(sKey, iDataSource, iSwitchID, iBusiClass, PRICING_ERR_TICKET_TYPE_ID,
					 			lPricingErrEvent, qry.field(10).asLong(), qry.field(11).asLong(), qry.field(12).asLong());			
			}else
			{
				addTicketLog(m_iter->second, iDataSource, iSwitchID, iBusiClass, PRICING_ERR_TICKET_TYPE_ID,
					 			lPricingErrEvent, qry.field(10).asLong(), qry.field(11).asLong(), qry.field(12).asLong());			
			}	
		}		
		
		lDupEvent = qry.field(13).asLong();
		
		if (lDupEvent > 0)
		{
			memset(sKey, 0, sizeof(sKey));
			sprintf(sKey, "%d_%d_%d_%d", iDataSource, iSwitchID, iBusiClass, DUP_TICKET_TYPE_ID);
			
			m_iter = m_mapTicketLog.find(string(sKey));
			if (m_iter == m_mapTicketLog.end())
			{
				newTicketLog(sKey, iDataSource, iSwitchID, iBusiClass, DUP_TICKET_TYPE_ID,
					 			lDupEvent, qry.field(14).asLong(), qry.field(15).asLong(), qry.field(16).asLong());			
			}else
			{
				addTicketLog(m_iter->second, iDataSource, iSwitchID, iBusiClass, DUP_TICKET_TYPE_ID,
					 			lDupEvent, qry.field(14).asLong(), qry.field(15).asLong(), qry.field(16).asLong());			
			}	
		}
		
		lPrepEvent = qry.field(17).asLong();
		
		if (lPrepEvent > 0)
		{
			memset(sKey, 0, sizeof(sKey));
			sprintf(sKey, "%d_%d_%d_%d", iDataSource, iSwitchID, iBusiClass, PREP_ERR_TICKET_TYPE_ID);
			
			m_iter = m_mapTicketLog.find(string(sKey));
			if (m_iter == m_mapTicketLog.end())
			{
				newTicketLog(sKey, iDataSource, iSwitchID, iBusiClass, PREP_ERR_TICKET_TYPE_ID,
					 			lPrepEvent, qry.field(18).asLong(), qry.field(19).asLong(), qry.field(20).asLong());			
			}else
			{
				addTicketLog(m_iter->second, iDataSource, iSwitchID, iBusiClass, PREP_ERR_TICKET_TYPE_ID,
					 			lPrepEvent, qry.field(18).asLong(), qry.field(19).asLong(), qry.field(20).asLong());			
			}	
		}
		
		lUselessEvent = qry.field(21).asLong();

		if (lUselessEvent > 0)
		{
			memset(sKey, 0, sizeof(sKey));
			sprintf(sKey, "%d_%d_%d_%d", iDataSource, iSwitchID, iBusiClass, USELESS_TICKET_TYPE_ID);
			
			m_iter = m_mapTicketLog.find(string(sKey));
			if (m_iter == m_mapTicketLog.end())
			{
				newTicketLog(sKey, iDataSource, iSwitchID, iBusiClass, USELESS_TICKET_TYPE_ID,
					 			lUselessEvent, qry.field(22).asLong(), qry.field(23).asLong(), qry.field(24).asLong());			
			}else
			{
				addTicketLog(m_iter->second, iDataSource, iSwitchID, iBusiClass, USELESS_TICKET_TYPE_ID,
					 			lUselessEvent, qry.field(22).asLong(), qry.field(23).asLong(), qry.field(24).asLong());			
			}	
		}		
	}
	
	qry.close();
	
   char sSwitchCode[100] = {0};   		
	 char sLineBuf[MaxLineLength] = {0};
	 char sTicketTypeName[50] = {0};   	
	
	for (m_iter = m_mapTicketLog.begin(); m_iter != m_mapTicketLog.end(); ++m_iter)	
	{
	   	memset(sSwitchCode, 0, sizeof(sSwitchCode));
	   	strncpy(sSwitchCode, oCheckTicketLog.getSwitchCode(m_iter->second->m_iSwitchID),100);	   		   			
		
	   	memset(sTicketTypeName, 0, sizeof(sTicketTypeName));
		
		switch(m_iter->second->m_iTicketTypeID)
		{
			case 10:
				strcpy(sTicketTypeName, "正常单");
				break;
			case 21:
				strcpy(sTicketTypeName, "预处理异常单");
				break;
			case 22:
				strcpy(sTicketTypeName, "批价误单");
				break;
			case 30:
				strcpy(sTicketTypeName, "重复话单");
				break;
			case 40:	
				strcpy(sTicketTypeName, "不计费话单");
				break;		
				
			default:
				strcpy(sTicketTypeName, "正常单");
				break;	
		}
		
    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%d|%d|%s|%d|%d|%s|%ld|%ld|%ld|%ld|%s", 
    	                  DR_ID,m_iProveNum, m_iter->second->m_iDataSource, sSwitchCode, m_iter->second->m_iBusiClass, 
    	                  m_iter->second->m_iTicketTypeID,sTicketTypeName,
    							      m_iter->second->m_lCurrRecCnt, m_iter->second->m_lCurrDuration, 
    							      m_iter->second->m_lCurrAmount, m_iter->second->m_lCurrFee, oCheckTicketLog.sEndDate);
    	
    	vCheckTicketLog.push_back(string(sLineBuf));
	}
    
    //调用接口入库
	  //oCheckTicketLog.writeFile(vCheckTicketLog);
    oCheckTicketLog.writeJsonFile(vCheckTicketLog,SPLIT_FLAG);
    DB_LINK->commit();       
    return 0;    	
}

/*
 *	函 数 名 : checkTransLog
 *	函数功能 : 提取记录计费系统通过IDEP传输文件的传输日志信息,AuditId=0007，Type=50,高频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkTransLog()
{
    cout <<"CheckTransLog::checkTransLog(AuditId=0007，Type=50,高频) ..."<<endl;
    
    StatInterface oCheckTransLog("0007",50,HIGH_FREG);    
    vector<string> vCheckTransLog;
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-DATA_SOURCE|"
                        "I-INTERFACE_ID|"
                        "I-TRANS_DIRECTION|"
                        "I-PRODUCT_ID|"
                        "S-FILE_NAME|"
                        "I-FILE_SIZE|"
                        "I-OUT_RECORD_CNT|"
                        "I-DURATION|"
                        "I-VOLUME|"
                        "I-SETT_FEE|"
                        "S-FILE_TIME|"
                        "S-TRANS_BEGIN_DATE|"
                        "S-TRANS_END_DATE	");                        
    vCheckTransLog.push_back(string(tmpbuf));	    
    
   	char sFileName[100] = {0};
   	char sDate[16] = {0};
   	char sEndDate[16] = {0};
   	long lSize = 0;
   	long lCnt = 0;
   	long lDuration = 0;
   	long lAmount = 0;
   	long lCharge = 0;
   	
   	int iDataSource = atoi(oCheckTransLog.m_sDefaultAreaCode+1);
   	
	  char *p = NULL;
   	char sLineBuf[MaxLineLength] = {0};
        
    DEFINE_QUERY(qry);
    qry.setSQL (" select t.merge_file_name, to_char(t.state_date,'yyyymmddhh24miss'), "
       			" to_char(t.state_date+1/(24*3600),'yyyymmddhh24miss'),nvl(t.file_size,0), "
       			" nvl(t.record_cnt,0),nvl(t.duration,0),nvl(t.amount,0),nvl(t.charge,0) "
    			" from B_PACKAGE_STATE_MERGE t "
				" where t.state_date between to_date(:statBeginDate, 'yyyymmddhh24miss')+ 1/(24*3600) and to_date(:statEndDate, 'yyyymmddhh24miss') ");
				
	  qry.setParameter("statBeginDate", oCheckTransLog.sStatBeginDate);				
	  qry.setParameter("statEndDate", oCheckTransLog.sEndDate);
 
    qry.open ();
    
    while (qry.next()) 
    {
    	memset(sFileName, 0, sizeof(sFileName));
    	strncpy(sFileName, qry.field(0).asString(), 100);	   	
	   	strcpy(sDate, qry.field(1).asString());
	   	strcpy(sEndDate, qry.field(2).asString());
	   	
   		lSize = qry.field(3).asLong();
   		lCnt = qry.field(4).asLong();;
   		lDuration = qry.field(5).asLong();;
   		lAmount = qry.field(6).asLong();;
   		lCharge = qry.field(7).asLong();;	   	
	   	
    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%d|%d|%d|%d|%d|%s|%ld|%ld|%ld|%ld|%ld|%s|%s|%s",
    	                  DR_ID, m_iProveNum,iDataSource, 3009, 2, 1000, sFileName, lSize, 
    	                  lCnt, lDuration, lAmount, lCharge, sDate, sDate, sEndDate);
    	
    	vCheckTransLog.push_back(string(sLineBuf));    	
    }
    
    qry.close ();
    
    //调用接口入库
  //	oCheckTransLog.writeFile(vCheckTransLog);
    oCheckTransLog.writeJsonFile(vCheckTransLog,SPLIT_FLAG); 
    DB_LINK->commit();
       
    return 0;    
}

/*
AuditId=0008 ,AuditId=0010 调用CheckDailyAcctItemAggr类中的函数，这里就不移植过来了
AuditId=0009 日帐信息点就不强制要求
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-DATA_SOURCE|"
                        "S-ACCT_ITEM_TYPE_NAME|"
                        "I-FEE_TYPE|"
                        "I-RECORD_CNT|"
                        "I-SETT_FEE|"
                        "S-STAT_DATE");                        
    vNormaFileInfo.push_back(string(tmpbuf));
    
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-DATA_SOURCE|"
                        "S-ACCT_ITEM_TYPE_NAME|"
                        "I-FEE_TYPE|"
                        "I-RECORD_CNT|"
                        "I-SETT_FEE|"
                        "S-ACCOUNT_CYCLE|"
                        "S-ACCUM_DATE");                        
    vNormaFileInfo.push_back(string(tmpbuf));    
    
    
*/


/*
 *	函 数 名 : checkSignHighFee
 *	函数功能 : 提取计费专业网管系统处理过的国内疑似高额记录,AuditId=0011，Type=50,高频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkSignHighFee()
{
    cout <<"CheckStatInfo::checkSignHighFee(AuditId=0011，Type=50,高频) ..."<<endl;

    StatInterface oCheckHighFee("0011",50,HIGH_FREG);
    vector<string> vCheckHighFee;
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-HIGH_SIGN_ID|"
                        "S-PROV_CODE|"
                        "S-SIGN_TIME|"
                        "S-SIGN_STAFF|"
                        "S-STAFF_CONTACT|"
                        "I-IF_HIGH_QUOTA|"
                        "I-BUSI_CLASS|"
                        "S-FOUND_TIME|"
                        "S-FINISH_TIME|"
                        "S-BILLING_NUMBER|"
                        "S-DURATION|"
                        "S-DATA_VOLUME|"
                        "S-FEE|"
                        "S-SITU_DESC|"
                        "I-DEAL_STATE|"
                        "S-DEAL_MEASURES|"
                        "I-CHEAT|"
                        "I-WHITE_LIST|"
                        "S-NOTIFY_TIME|"
                        "S-FEEDBACK_TIME|"
                        "S-RESULT_DEAL_TIME|"
                        "S-WHITE_LIST_TIME|"
                        "S-REMARK");                        
    vCheckHighFee.push_back(string(tmpbuf));
    
    long lFileID = 0;
    int iSwitchID = 0;
   	int iBusiClass = 0;
   	int iBusiType = 0;
   	char sStartDate[30] = {0};
   	char sEndDate[30] = {0};
   	char sSignTime[30] = {0};
   	
   	char sNotifyTime[15] = {0};
   	char sFeedBackTime[15] = {0};
   	char sResultDealTime[15] = {0};

   	int iSeq = 0;
   	char sAccNbr[50] = {0};
   	
	long lDuration = 0;
	long lAmount = 0;
	long lCharge = 0;   
	
	char sAreaCode[4] = {0};
	
	if (strlen(oCheckHighFee.m_sDefaultAreaCode)>3)
	{
		strncpy(sAreaCode, oCheckHighFee.m_sDefaultAreaCode+1, 4);
	}else
	{
		strncpy(sAreaCode, oCheckHighFee.m_sDefaultAreaCode, 4);
	}	
	
	char sHignSign[20] = {0};
	char sSignStaff[50]	= {0};
	char sStaffContact[50] = {0};

	Date d;
	char sDate[8] = {0};
	strcpy(sDate, d.toString((char *)"yyyymmdd")+2);
   	
   	char sLineBuf[MaxLineLength] = {0};
        
    DEFINE_QUERY(qry);
    qry.setSQL (" select a.BUSI_CLASS, to_number(substr(a.FILE_NAME,-4,4)) seq, "
       			" a.sign_nbr, nvl(a.duration,0), nvl(a.amount,0), nvl(a.fee,0), a.file_id, a.switch_id, "
       			" to_char(start_date, 'YYYY-MM-DD HH24:MI:SS'), to_char(end_date, 'YYYY-MM-DD HH24:MI:SS'), "
                " to_char(sysdate, 'YYYY-MM-DD HH24:MI:SS'), a.file_date, a.created_date, a.state_date,  "
       			" b.sign_staff, b.staff_contact "
				" from v_sign_high_fee_table a, b_high_fee_sign_info b "
				" where a.staff_id=b.staff_id "
				" and a.created_date>=sysdate-:vForwardDays "
        		" and a.switch_id>0"
        		" and a.state_date between to_date(:statBeginDate, 'yyyymmddhh24miss')+ 1/(24*3600) and to_date(:statEndDate, 'yyyymmddhh24miss') ");
    
    qry.setParameter("vForwardDays", 2);            
	qry.setParameter("statBeginDate", oCheckHighFee.sStatBeginDate);				
	qry.setParameter("statEndDate", oCheckHighFee.sEndDate);

    
    qry.open ();
    
    while (qry.next()) 
    {
	   	iBusiType = qry.field(0).asInteger();	
		iSeq = qry.field(1).asInteger();
		
		memset(sAccNbr, 0, sizeof(sAccNbr));
		strncpy(sAccNbr, qry.field(2).asString(), 50);
		
	   	lDuration = qry.field(3).asLong();
	   	lAmount = qry.field(4).asLong();
	   	lCharge = qry.field(5).asLong();		
		
    	lFileID = qry.field(6).asLong();
    	iSwitchID = qry.field(7).asInteger();
		
		memset(sStartDate, 0, sizeof(sStartDate));
		strncpy(sStartDate, qry.field(8).asString(), 30);
		
		memset(sEndDate, 0, sizeof(sEndDate));
		strncpy(sEndDate, qry.field(9).asString(), 30);		

		memset(sSignTime, 0, sizeof(sSignTime));
		strncpy(sSignTime, qry.field(10).asString(), 30);	
		    	     	 	    	 			
    	memset(sNotifyTime, 0, sizeof(sNotifyTime));
		strncpy(sNotifyTime, qry.field(11).asString(),15);
	   	
    	memset(sFeedBackTime, 0, sizeof(sFeedBackTime));
		strncpy(sFeedBackTime, qry.field(12).asString(),15);
		
    	memset(sResultDealTime, 0, sizeof(sResultDealTime));
		strncpy(sResultDealTime, qry.field(13).asString(),15);	
		
    	memset(sSignStaff, 0, sizeof(sSignStaff));
		strncpy(sSignStaff, qry.field(14).asString(),50);
		
    	memset(sStaffContact, 0, sizeof(sStaffContact));
		strncpy(sStaffContact, qry.field(15).asString(),50);		

	   	iBusiClass = oCheckHighFee.getBusiclassBySwitch(iSwitchID);		
		
    	memset(sHignSign, 0, sizeof(sHignSign));
		snprintf(sHignSign, 20, "%d%s%s%2d", iBusiClass,sAreaCode,sDate,iSeq);
	   	
    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%s|%04s|%s|%s|%s|%d|%d|%s|%s|%s|%ld|%ld|%ld|%s|%d|%s|%d|%d|%s|%s|%s|%s|%s", 
    	                   DR_ID, sHignSign, m_sProveCode, sSignTime, sSignStaff, sStaffContact, 0,
    	                    iBusiType, sStartDate, sEndDate, sAccNbr, lDuration, lAmount, lCharge, 
    	                    "处理完毕", 2, "确认完毕", 0, 0,sNotifyTime, sFeedBackTime, sResultDealTime, 
    	                    sResultDealTime, "无备注");
    	
    	vCheckHighFee.push_back(string(sLineBuf));    	    	
    }
    
    qry.close ();
    /*
    {
    	memset(sLineBuf, 0, sizeof(sLineBuf));	  
    	sprintf(sLineBuf, "%d|%s|%04s|%s|%s|%s|%d|%d|%s|%s|%s|%ld|%ld|%ld|%s|%d|%s|%d|%d|%s|%s|%s|%s|%s", 
    	                   DR_ID, "1002520100310", m_sProveCode, "20101004030000", "", "13815001200", 0,
    	                    101, "20101004023000", "20101004024000", "13812001500", 10000, 10000, 10000, 
    	                    "处理完毕", 2, "确认完毕", 0, 0,"20101004030000", "20101004030000", "20101004030000", 
    	                    "20101004030000", "无备注");
    	 vCheckHighFee.push_back(string(sLineBuf)); 
   }                   
    */
    //调用接口入库
	  //oCheckHighFee.writeFile(vCheckHighFee);
    oCheckHighFee.writeJsonFile(vCheckHighFee,SPLIT_FLAG);

    DB_LINK->commit();
       
    return 0;
}	


/*
 *	函 数 名 : checkHighFeeServ
 *	函数功能 : 提取采集计费系统中省级疑似高额记录,AuditId=0012，Type=50,高频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
/*
int CheckStatInfo::checkHighFeeServ()
{
    cout <<"CheckStatInfo::checkHighFeeServ(AuditId=0012，Type=50,高频) ..."<<endl;
	   Environment::useUserInfo ();  
    StatInterface oCheckHighFee("0012",50,HIGH_FREG);
    vector<string> vCheckHighFee;
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-BUSI_CLASS|"
                        "S-BUSI_CLASS_NAME|"
                        "S-FOUND_TIME|"
                        "S-BILLING_NUMBER|"
                        "S-DURATION|"
                        "S-DATA_VOLUME|"
                        "S-FEE|"
                        "I-WHITE_LIST|"
                        "I-CREDIT_VALUE|"
                        "I-STOP_FLAG|"
                        "I-USER_AMOUNT");                        
    vCheckHighFee.push_back(string(tmpbuf));    
        	             
   if(!setBillingCycleID(oCheckHighFee.sStatBeginDate))
    {
    	return -1;
    }	
    
    char sValue[1001] = {0};
    char sProvinceCode[5] = {0};
    	
   	if (!ParamDefineMgr::getParam("CHECK_STAT", "PROVINCE_CODE", sValue, -1))
    {
    	return -1;
    }    
   	strncpy(sProvinceCode, sValue, 4);    
    
   	char sLineBuf[MaxLineLength] = {0};
    
    BalanceMgr oBalanceMgr;
	  AcctItemAccuMgr oAcctItemAccuMgr;
    ServIteration oServIteration;
    if (!G_PUSERINFO->getServIteration(oServIteration))
    {    
        Log::log(0, "共享内存连接失败");    
        return -1;      
    }
    
    long lAllCharge;
    long lCredit;
    //int iAcctItemType;
    long lAcctID;
    long lAcctBalance;
    int iStopFlag;
    long lServID;
    int iBillingModeID;
    long lUserBalace ;
    
    vector<AcctItemData *> vct;
    vector<AcctItemData *>::iterator iter;
    
    
    while (oServIteration.next(G_SERV))    
    {
    	lAllCharge = 0;
    	lUserBalace = 0;
        //iBaseOfferID = G_SERV.getBaseOfferID();
        lServID = G_SERV.getServID();        
		
		iBillingModeID = 0;
  		char *pBillingMode=G_SERV.getAttr(ID_BILLING_MODE);
  		if (pBillingMode != NULL)
   		{
			iBillingModeID = atoi(pBillingMode);
   		}
    	
		//增加若取不到属性值,则取billing_mode_id
		if (iBillingModeID == 0)
		{	
			iBillingModeID = G_SERV.getServInfo()->m_iBillingMode;
		}
		
		if (iBillingModeID != 2)//只稽核预付费用户
			continue;

        //iOrgID = G_SERV.getOrgID();                

        oAcctItemAccuMgr.getAll(lServID, m_iBillingCycleID, vct);
        
        if (!vct.empty())
        {                        
            for (iter = vct.begin(); iter != vct.end(); ++iter)
            {    
                lAllCharge += (*iter)->m_lValue;
                lAcctID = (*iter)->m_lAcctID;
            }
            
            vct.clear();                          
        }
        
        if(lAllCharge <= 5000)
        	continue;
        
        //lAcctID = G_SERV.getAcctID(iAcctItemType);
       	
        lAcctBalance = oBalanceMgr.getAcctBalance(lAcctID);
        lUserBalace = lAcctBalance - lAllCharge ;
        lCredit = G_PUSERINFO->getAcctCreditAmount(lAcctID);
        
        if (lCredit + lAcctBalance - lAllCharge >= -5000)
    		continue;
    	
    	//lRealAcctBalance = getAcctBalance(lAcctID);
    	//if (lRealAcctBalance - lAllCharge <= -5000)
    	//{
    		//continue;
    	//}	
    	if (strcmp(G_SERV.getServInfo()->m_sState,"2HA") == 0)
    	{	
    		iStopFlag = 0;
    	}else
    	{
    		iStopFlag = 1;
    	}		
    	
    	memset(sLineBuf, 0, sizeof(sLineBuf));
    	sprintf(sLineBuf, "%d|%04s|%d|%s|%s|%s|%ld|%ld|%ld|0|%ld|%d|%ld", 
    	        DR_ID, sProvinceCode,200,"省级高额" ,oCheckHighFee.sCreateDate,G_SERV.getServInfo()->m_sAccNbr, 
    	        0,0,lAllCharge,lCredit, iStopFlag,lUserBalace);    
    	
    	vCheckHighFee.push_back(string(sLineBuf));   
    }
        	
    
    //调用接口入库
	  //oCheckHighFee.writeFile(vCheckHighFee);
    oCheckHighFee.writeJsonFile(vCheckHighFee,SPLIT_FLAG);
    DB_LINK->commit();
    
    return 0;     
}
*/	

/*
 *	函 数 名 : checkHighFeeServNew
 *	函数功能 : 提取采集计费系统中省级疑似高额记录,AuditId=0012，Type=50,高频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
/*
int CheckStatInfo::checkHighFeeServNew()
{
    cout <<"CheckStatInfo::checkHighFeeServ(AuditId=0012，Type=50,高频) ..."<<endl;
	   Environment::useUserInfo ();  
    StatInterface oCheckHighFee("0012",50,HIGH_FREG);
    vector<string> vCheckHighFee;
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-BUSI_CLASS|"
                        "S-BUSI_CLASS_NAME|"
                        "S-FOUND_TIME|"
                        "S-BILLING_NUMBER|"
                        "S-DURATION|"
                        "S-DATA_VOLUME|"
                        "S-FEE|"
                        "I-WHITE_LIST|"
                        "I-CREDIT_VALUE|"
                        "I-STOP_FLAG|"
                        "I-USER_AMOUNT");                        
    vCheckHighFee.push_back(string(tmpbuf));    
        	
     BalanceMgr oBalanceMgr;
     AcctItemAccuMgr oAcctItemAccuMgr;
     vector<AcctItemData *> vct;
     vector<AcctItemData *>::iterator iter;
    
    char sLineBuf[MaxLineLength] = {0};    
    long lHighfeeID ;
    int  iBusiID ;
	  char sBusiName[50]={0};
	  char sFoundTime[14+1]={0};
	  char sBillingNbr[20]={0};
	  char sCDRTime[14+1]={0};
   	long lDuration ;
   	long lAmount ;
   	long lCharge ;
   	long iStopFlag;
   	long lServID;
   	long lAcctID;
   	long lAllCharge;
   	long lAcctBalance ;
    long lUserBalace  ;
    long lCredit ;  
        
    DEFINE_QUERY(qry);
    qry.setSQL ("select HIGH_FEE_ID,BUSI_CLASS ,BUSI_CLASS_NAME,FOUND_TIME , "
                " BILLING_NBR  ,nvl(DURATION,0) ,nvl(DATA_VOLUME,0) ,nvl(FEE,0),START_DATE " 
    			      " from B_HIGHFEE_ALARM_LOG t "
				        " where OUT_DB_FLAG= 0 " );
				      //  " and to_date(FOUND_TIME, 'yyyymmddhh24miss') between  to_date(:statBeginDate, 'yyyymmddhh24miss')+ 1/(24*3600)  and  to_date(:statEndDate, 'yyyymmddhh24miss') ");
				
	 // qry.setParameter("statBeginDate", oCheckHighFee.sStatBeginDate);				
	  //qry.setParameter("statEndDate", oCheckHighFee.sEndDate);
 
    qry.open ();
    
    while (qry.next()) 
    {
      lHighfeeID = qry.field(0).asLong();
      iBusiID = qry.field(1).asInteger();
      memset(sBusiName,0,sizeof(sBusiName));
	   	strcpy(sBusiName, qry.field(2).asString());
	   	memset(sFoundTime,0,sizeof(sFoundTime));
	   	strncpy(sFoundTime,qry.field(3).asString(),14);
	   	memset(sBillingNbr,0,sizeof(sBillingNbr));
	   	strcpy(sBillingNbr,qry.field(4).asString());
   		lDuration = qry.field(5).asLong();
   		lAmount = qry.field(6).asLong();
   		lCharge = qry.field(7).asLong();	  
      memset(sCDRTime,0,sizeof(sCDRTime));
      strncpy(sCDRTime,qry.field(8).asString(),14);
      
    	if (!G_PUSERINFO->getServ (G_SERV, sBillingNbr, sFoundTime))
	     {	
	       iStopFlag = 0;
       }	
      if (strcmp(G_SERV.getServInfo()->m_sState,"2HA") == 0)
    	 {	
    		 iStopFlag = 0;
    	 }else
    	 {
    		 iStopFlag = 1;
    	 }
    	 
    	lServID = G_SERV.getServID();
    	
    	if(!setBillingCycleID(sCDRTime))
    		 setBillingCycleID(sFoundTime) ;
    	
    	//cout<<"查询用户"<<lServID<<"--账期为"<<m_iBillingCycleID<<endl;
      oAcctItemAccuMgr.getAll(lServID, m_iBillingCycleID, vct);   
      lAllCharge= 0;     
        if (!vct.empty())
        {                        
            for (iter = vct.begin(); iter != vct.end(); ++iter)
            {    
                //lAllCharge += (*iter)-> m_iValue;\
                lAllCharge += (*iter)-> m_iValue;
                lAcctID = (*iter)->m_lAcctID;
            }
            
            vct.clear();                          
        }    	   
      lAcctBalance = oBalanceMgr.getAcctBalance(lAcctID);
      lUserBalace = lAcctBalance - lAllCharge ;
      lCredit = G_PUSERINFO->getAcctCreditAmount(lAcctID);  
  
    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%04s|%d|%s|%s|%s|%ld|%ld|%ld|0|%ld|%d|%ld", 
    	        DR_ID, m_sProveCode,iBusiID,sBusiName ,sFoundTime,sBillingNbr, 
    	        lDuration,lAmount,lCharge,lCredit, iStopFlag,lUserBalace);    
    	
    	vCheckHighFee.push_back(string(sLineBuf));   
    	upHighFeeRecord(lHighfeeID) 	;
    }
    
     qry.close ();
    	
    
    //调用接口入库
	  //oCheckHighFee.writeFile(vCheckHighFee);
    oCheckHighFee.writeJsonFile(vCheckHighFee,SPLIT_FLAG);
    DB_LINK->commit();
    
    return 0;     
}	
*/

/*
 *	函 数 名 : checkAlertLog
 *	函数功能 : 提取计费系统所有进程的告警日志和关键事件信息,AuditId=0013，Type=50,高频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkAlertLog()
{
   cout <<"CheckProcess::checkAlertLog(AuditId=0013，Type=50,高频) ..."<<endl;
   
	 StatInterface oCheckAlertLog("0013",50,HIGH_FREG);
	 vector<string> vCheckAlertLog;
	 vCheckAlertLog.clear();
	  char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-LOG_GRADE|"
                        "S-ERROR_CODE|"
                        "S-ERROR_NAME|"
                        "S-LOG_CONTENT|"
                        "S-EXPERT_ADVICE|"
                        "S-LOG_BORN_TIME");                        
    vCheckAlertLog.push_back(string(tmpbuf));
	 
	 int iProcessID = 0;
	 int iOrgAlertCode = 0;
	 int iDefineAlertCode = 0;
	 int iLogGrade = 0;
	 char sErrorCode[10] = {0};
	 char sErrorName[50] = {0};
	 char sLogContent[2000] = {0};
	 char sExpertAdvice[2000] = {0};
	 char sCreateDate[16] = {0};
	 
	 char sLineBuf[MaxLineLength] = {0};
	 
	 DEFINE_QUERY(qry);
	 
	 qry.setSQL(" select a.process_id, nvl(a.alert_code, 0) org_alert_code, \
    				 REPLACE(REPLACE(REPLACE(a.alert_content,'|','') ,chr(10),''),chr(13),'') alert_content, \
    				  nvl(b.alert_code,0) iDefineAlertCode, to_number(nvl(b.alert_level,3)), \
    				 nvl(b.deal_method,0), nvl(b.alert_explain, 0), trim(a.msg_date)||trim(a.msg_time) log_date \
    				  from wf_alert_log a,wf_alert_define b \
	 		 where a.alert_code=b.alert_code(+) \
	 		 and to_date(trim(a.msg_date)||trim(a.msg_time), 'yyyymmddhh24miss') between to_date(:statBeginDate, 'yyyymmddhh24miss')+ 1/(24*3600) and to_date(:statEndDate, 'yyyymmddhh24miss') ");
	 		 
   qry.setParameter("statBeginDate" ,oCheckAlertLog.sStatBeginDate);
   qry.setParameter("statEndDate" ,oCheckAlertLog.sEndDate);
   
   qry.open();
   
   while(qry.next())
   {
   	iProcessID = qry.field(0).asInteger();
   	
   	char sTempAlertCode[5] = {0};
   	strncpy(sTempAlertCode, qry.field(1).asString(), 4);
   	iOrgAlertCode = atoi(sTempAlertCode);
   	
   	memset(sTempAlertCode, 0, sizeof(sTempAlertCode));
   	strncpy(sTempAlertCode, qry.field(3).asString(), 4);
   	iDefineAlertCode = atoi(sTempAlertCode);
   	
   	iLogGrade = qry.field(4).asInteger();
   	
   	if(iOrgAlertCode != 0 && iDefineAlertCode == 0)
   	{
   		continue;
   	}			
   	
   	memset(sErrorName, 0, sizeof(sErrorName));	
   	memset(sLogContent, 0, sizeof(sLogContent));	
   	memset(sExpertAdvice, 0, sizeof(sExpertAdvice));	
   	
   	strncpy(sLogContent, qry.field(2).asString(), 2000);
   	
   	if (iOrgAlertCode != 0 && iDefineAlertCode != 0)
   	{
   		snprintf(sErrorCode, 10, "10%d%d", iProcessID, iOrgAlertCode);
   		//strncpy(sLogContent, qry.field(2).asString(), 2000);
   		strncpy(sExpertAdvice, qry.field(5).asString(), 2000);
   		strncpy(sErrorName, qry.field(6).asString(), 50);
   	}
   	
   	strcpy(sCreateDate, qry.field(7).asString());
   	
   	memset(sLineBuf, 0, sizeof(sLineBuf));	
   	sprintf(sLineBuf, "%d|%d|%d|%s|%s|%s|%s|%s", 
   	                DR_ID, m_iProveNum,iLogGrade, sErrorCode, sErrorName, 
   									sLogContent, sExpertAdvice, sCreateDate);
     cout<<sLineBuf<<endl;									
   	vCheckAlertLog.push_back(string(sLineBuf));
   	
   }
   
     //调用接口入库
    // oCheckAlertLog.writeFile(vCheckAlertLog);
     oCheckAlertLog.writeJsonFile(vCheckAlertLog,SPLIT_FLAG);
     DB_LINK->commit();
     return 0;
}

/*
 *	函 数 名 : checkHSSAlertLog
 *	函数功能 : 提取HSS系统所有进程的告警日志和关键事件信息,AuditId=0006，Type=50,高频
 *	时    间 : 2011年6月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkHSSAlertLog()
{
   cout <<"CheckStatInfo::checkHSSAlertLog(AuditId=0006，Type=50,高频) ..."<<endl;
   
	 StatInterface oCheckAlertLog("0006",50,HIGH_FREG);
	 vector<string> vCheckAlertLog;
	 vCheckAlertLog.clear();
	  char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "I-DATA_SOURCE|"
                        "I-LOG_GRADE|"
                        "S-ERROR_CODE|"
                        "S-ERROR_NAME|"
                        "S-LOG_CONTENT|"
                        "S-EXPERT_ADVICE|"
                        "S-LOG_BORN_TIME");                        
    vCheckAlertLog.push_back(string(tmpbuf));
	 
	 int iProcessID = 0;
	 int iOrgAlertCode = 0;
	 int iDefineAlertCode = 0;
	 int iLogGrade = 0;
	 char sErrorCode[10] = {0};
	 char sErrorName[50] = {0};
	 char sLogContent[2000] = {0};
	 char sExpertAdvice[2000] = {0};
	 char sCreateDate[16] = {0};
	 int iDataSource = atoi(oCheckAlertLog.getCityCode());
	 
	 char sLineBuf[MaxLineLength] = {0};
	 
/*	 DEFINE_QUERY(qry);
	 
	 qry.setSQL(" select a.process_id, nvl(a.alert_code, 0) org_alert_code, \
    				 REPLACE(REPLACE(REPLACE(a.alert_content,'|','') ,chr(10),''),chr(13),'') alert_content, \
    				  nvl(b.alert_code,0) iDefineAlertCode, to_number(nvl(b.alert_level,3)), \
   				 nvl(b.deal_method,0), nvl(b.alert_explain, 0), trim(a.msg_date)||trim(a.msg_time) log_date \
   				  from wf_alert_log a,wf_alert_define b \
	 		 where a.alert_code=b.alert_code(+) \
	 		 and to_date(trim(a.msg_date)||trim(a.msg_time), 'yyyymmddhh24miss') between to_date(:statBeginDate, 'yyyymmddhh24miss')+ 1/(24*3600) and to_date(:statEndDate, 'yyyymmddhh24miss') ");
	 		 
   qry.setParameter("statBeginDate" ,oCheckAlertLog.sStatBeginDate);
   qry.setParameter("statEndDate" ,oCheckAlertLog.sEndDate);
   
   qry.open();
   
   while(qry.next())
   {
   	iProcessID = qry.field(0).asInteger();
   	
   	char sTempAlertCode[5] = {0};
   	strncpy(sTempAlertCode, qry.field(1).asString(), 4);
   	iOrgAlertCode = atoi(sTempAlertCode);
   	
   	memset(sTempAlertCode, 0, sizeof(sTempAlertCode));
   	strncpy(sTempAlertCode, qry.field(3).asString(), 4);
   	iDefineAlertCode = atoi(sTempAlertCode);
   	
   	iLogGrade = qry.field(4).asInteger();
   	
   	if(iOrgAlertCode != 0 && iDefineAlertCode == 0)
   	{
   		continue;
   	}			
   	
   	memset(sErrorName, 0, sizeof(sErrorName));	
   	memset(sLogContent, 0, sizeof(sLogContent));	
   	memset(sExpertAdvice, 0, sizeof(sExpertAdvice));	
   	
   	strncpy(sLogContent, qry.field(2).asString(), 2000);
   	
   	if (iOrgAlertCode != 0 && iDefineAlertCode != 0)
   	{
   		snprintf(sErrorCode, 10, "10%d%d", iProcessID, iOrgAlertCode);
   		//strncpy(sLogContent, qry.field(2).asString(), 2000);
   		strncpy(sExpertAdvice, qry.field(5).asString(), 2000);
   		strncpy(sErrorName, qry.field(6).asString(), 50);
   	}
   	
   	strcpy(sCreateDate, qry.field(7).asString());
*/
   	/*sprintf(sLineBuf, "%d|%d|%d|%s|%s|%s|%s|%s", 
   	                DR_ID, iDataSource, iLogGrade, sErrorCode, sErrorName, 
   									sLogContent, sExpertAdvice, sCreateDate);*/
    ThreeLogGroup Log;
    vector<LogInfoData> vRet;  
    vRet.clear();
    HBErrorCode *pHBErrorCode  = new HBErrorCode();
    pHBErrorCode->init();
    Log.GetLoggInfo(vRet,GROUPTYPE,LOG_TYPE_PROC,oCheckAlertLog.sStatBeginDate,oCheckAlertLog.sEndDate); //进程启停
    if(vRet.size())
   { 
   	 for(int i = 0; i<vRet.size();i++)
   	 {
   	   memset(sLineBuf, 0, sizeof(sLineBuf));	
       sprintf(sLineBuf, "%d|%d|%d|%s|%s|%s|%s|%s",
                    DR_ID,atoi(vRet[i].m_sAreaCode),3,NULL,NULL,
                    vRet[i].m_sInfo,NULL,vRet[i].m_sLogTime);
       vCheckAlertLog.push_back(string(sLineBuf));
     }
    }
    vRet.clear();
    Log.GetLoggInfo(vRet,GROUPINFOLV,LOG_LEVEL_FATAL,oCheckAlertLog.sStatBeginDate,oCheckAlertLog.sEndDate); //严重告警
    if(vRet.size())
    {
    	for(int i = 0; i<vRet.size();i++)
   	  {
   	   memset(sLineBuf, 0, sizeof(sLineBuf));	
       sprintf(sLineBuf, "%d|%d|%d|%d|%s|%s|%s|%s",
                    DR_ID,atoi(vRet[i].m_sAreaCode),1,vRet[i].m_illCode,pHBErrorCode->getCauseInfo(vRet[i].m_illCode),
                    vRet[i].m_sInfo,pHBErrorCode->getAdviceInfo(vRet[i].m_illCode),vRet[i].m_sLogTime);
       vCheckAlertLog.push_back(string(sLineBuf));
      }
    }
    vRet.clear();
    Log.GetLoggInfo(vRet,GROUPINFOLV,LOG_LEVEL_ERROR,oCheckAlertLog.sStatBeginDate,oCheckAlertLog.sEndDate); //错误告警  
    if(vRet.size())
    {
    	for(int i = 0; i<vRet.size();i++)
   	  {
   	   memset(sLineBuf, 0, sizeof(sLineBuf));	
       sprintf(sLineBuf, "%d|%d|%d|%d|%s|%s|%s|%s",
                    DR_ID,atoi(vRet[i].m_sAreaCode),2,vRet[i].m_illCode,pHBErrorCode->getCauseInfo(vRet[i].m_illCode),
                    vRet[i].m_sInfo,pHBErrorCode->getAdviceInfo(vRet[i].m_illCode),vRet[i].m_sLogTime);
       vCheckAlertLog.push_back(string(sLineBuf));
      }
    }                     
//     cout<<sLineBuf<<endl;	 
     //调用接口入库
    // oCheckAlertLog.writeFile(vCheckAlertLog);
    delete pHBErrorCode;
    pHBErrorCode = NULL;
     oCheckAlertLog.writeJsonFile(vCheckAlertLog,SPLIT_FLAG);
  //   DB_LINK->commit();
     return 0;
}

/*
 *	函 数 名 : checkDelayFile
 *	函数功能 : 提取记录计费处理的从话单通话结束时间到话单入库后的处理时长统计信息,AuditId=0014，Type=50,高频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkDelayFile()
{
    cout <<"CheckStatInfo::checkDelayFile(AuditId=0014，Type=50,高频) ..."<<endl;
    
    StatInterface oDelayFileStat("0014",50,HIGH_FREG);    
    vector<string> vDelayFileInfo;
    vDelayFileInfo.clear();
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-DATA_SOURCE|"
                        "I-BATCH_CODE|"
                        "S-SWITCH_CODE|"
                        "I-BUSI_CLASS|"
                        "I-DELAY_CNT|"
                        "I-TOTAL_CNT|"
                        "S-STAT_TIME");                        
    vDelayFileInfo.push_back(string(tmpbuf));       
    
    long lFileID = 0;
    long lRecordCnt = 0;
    long lRecord = 0;
    int iSwitchID = 0;
   	int iBusiClass = 10;
   	int iDataSource = 0;
   	
   	char sEndDate[14] = {0};
   	char sSwitchCode[100] = {0};
   	char sLineBuf[MaxLineLength] = {0};
   	
   	int iCnt = 0;
        
    DEFINE_QUERY(qry);

    char sSql[2048]={0};
    sprintf(sSql,"select a.file_id, max(nvl(record_cnt,0)) record_cnt, max(switch_id) switch_id, max(nvl(busi_class,0)) busi_class, max(nvl(data_source,0)) data_source, "
        " sum(b.in_normal_event+b.in_black_event+b.in_dup_event+b.in_pre_err_event+b.in_useless_event+b.in_pric_err_event) record, "
        " max(to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') - a.created_date)*24*3600 Sec "
        " from b_event_file_list_cfg a, wf_stat_log b where created_date>=sysdate-%d "
        " and a.switch_id>0"
        " and a.state='END'"
        " and a.state_date < sysdate - %d/(3600*24)"
        " and not exists "
        	"(select 1 from b_check_file_log c where c.MODULE_ID = %d "
        "  and c.file_id=a.file_id )" 
        " and a.file_id = b.file_id "
        " and b.process_id in (select process_id from wf_process where app_id in (%s))"
        " group by a.file_id  " , m_iForwardDays ,m_iDelayStatSecs,m_iModuleID,INSTORE_APP_ID);
    qry.setSQL(sSql);

/*   
    qry.setSQL ("select a.file_id, max(nvl(record_cnt,0)) record_cnt, max(switch_id) switch_id, max(nvl(busi_class,0)) busi_class, max(nvl(data_source,0)) data_source, "
        " sum(b.in_normal_event+b.in_black_event+b.in_dup_event+b.in_pre_err_event+b.in_useless_event+b.in_pric_err_event) record, "
        " max(to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') - a.created_date)*24*3600 Sec "
        " from b_event_file_list_cfg a, wf_stat_log b where created_date>=sysdate-:vForwardDays "
        " and a.switch_id>0"
        " and a.state='END'"
        " and a.state_date < sysdate - :vSec/(3600*24)"
        " and not exists "
        	"(select 1 from b_check_file_log c where c.MODULE_ID = :vMod "
        "  and c.file_id=a.file_id )" 
        " and a.file_id = b.file_id "
        " and b.process_id in (select process_id from wf_process where app_id in (:vInstore_app_id))"
        //" and to_date(b.msg_date||b.msg_time,'yyyymmddhh24miss') - 86400/(3600*24) >= a.created_date "
        " group by a.file_id "
    );
    
    qry.setParameter ("vForwardDays", m_iForwardDays);                
    qry.setParameter ("vMod", m_iModuleID);
    qry.setParameter ("vSec", m_iDelayStatSecs);
    qry.setParameter ("vInstore_app_id", INSTORE_APP_ID);
*/
   
    qry.open ();
    
    while (qry.next()) 
    {
    	iCnt++;
    	lFileID = qry.field(0).asLong();
    	lRecordCnt = qry.field(1).asLong();
    	iSwitchID = qry.field(2).asInteger();
		
	   	iBusiClass = qry.field(3).asInteger();
	   		   	
	   	if (iBusiClass == 0)
	   	{
	   		iBusiClass = oDelayFileStat.getBusiclassBySwitch(iSwitchID);
	   	}	
	   	 	
    	iDataSource = qry.field(4).asInteger();
    	if (iDataSource == 0)
    	{	
			iDataSource = oDelayFileStat.getDataSourceBySwitch(iSwitchID);
		}	   	
	   	
	   	lRecord = qry.field(5).asInteger();
	   	
	   	memset(sSwitchCode, 0, sizeof(sSwitchCode));
	   	strncpy(sSwitchCode, oDelayFileStat.getSwitchCode(iSwitchID), 100);	   	
	   	
	   	memset(sEndDate,0,sizeof(sEndDate));
	   	strncpy(sEndDate, oDelayFileStat.sEndDate, 12);
	   	
	   	insertCheckLog(lFileID, m_iModuleID, "OK");
	   	
	   	if (qry.field(6).asLong() < 86400)
	   	{
	   		continue;
	   	}	
	   	
    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%d|%d|%ld|%s|%d|%ld|%ld|%s", 
    	                    DR_ID,m_iProveNum, iDataSource, lFileID, sSwitchCode, 
    	                    iBusiClass, lRecord, lRecordCnt, sEndDate);
    	
    	vDelayFileInfo.push_back(string(sLineBuf));
    	
    	if(iCnt >= MAX_CHECK_FILES||vDelayFileInfo.size() >= MAX_CHECK_FILES)
    		break;	    	
    }
    
    qry.close ();
        	
    oDelayFileStat.writeJsonFile(vDelayFileInfo,SPLIT_FLAG);
    DB_LINK->commit();
       
    return 0;
}

/*
 *	函 数 名 : checkMouldeCapabilityKpi
 *	函数功能 : 提取记录计费系统各个模块的处理性能指标,AuditId=0015，Type=50,中频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkMouldeCapabilityKpi()
{
    cout <<"CheckKpi::checkMouldeCapabilityKpi(AuditId=0015，Type=50,中频) ..."<<endl;
    
    StatInterface oMouldeCapabilityKpi("0015",50,MIDL_FREG);    
    vector<string> vMouldeCapabilityKpi;
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-KPI_ID|"
                        "S-KPI_NAME|"
                        "S-VALUE|"
                        "S-STAT_TIME");                        
    vMouldeCapabilityKpi.push_back(string(tmpbuf));       
    
   	long lLogID = 0;
    int iKpiID = 0;
   	char sKpiCode[50] = {0};
   	long lKpiValue = 0;
    char sKpiValue[20+1]={0};
   	char sEndDate[14] = {0};
   	char sLineBuf[MaxLineLength] = {0};
        
    DEFINE_QUERY(qry);
    qry.setSQL (" select t.kpi_log_id,t.kpi_id,t.kpi_name,trim(t.kpi_value) "
				" from B_MODULE_CAPABILITY_KPI t "
				" where nvl(t.deal_flag, 0) = 0 ");

    qry.open ();
    
    while (qry.next()) 
    {
    	lLogID = qry.field(0).asLong();
    	iKpiID = qry.field(1).asInteger();
    	
    	memset(sKpiCode, 0, sizeof(sKpiCode));
    	strncpy(sKpiCode, qry.field(2).asString(), 50);
    	
    	//lKpiValue = qry.field(3).asLong();
    	memset(sKpiValue,0,sizeof(sKpiValue));
	   	strncpy(sKpiValue,qry.field(3).asString(),20);
	   	strncpy(sEndDate, oMouldeCapabilityKpi.sEndDate, 12);	   	
	   	
    	memset(sLineBuf, 0, sizeof(sLineBuf));	  	
    	sprintf(sLineBuf, "%d|%d|%d|%s|%s|%s", 
    	                  DR_ID,m_iProveNum, iKpiID, sKpiCode, sKpiValue, sEndDate);
    	
    	vMouldeCapabilityKpi.push_back(string(sLineBuf));
    	
    	updateMouldeCapabilityKpiLog(lLogID);
    }
    
    qry.close ();
    
    //调用接口入库
	  //oMouldeCapabilityKpi.writeFile(vMouldeCapabilityKpi);
    oMouldeCapabilityKpi.writeJsonFile(vMouldeCapabilityKpi,SPLIT_FLAG);
    DB_LINK->commit();
       
    return 0;    
}

/*
 *	函 数 名 : checkSystemLossKpi
 *	函数功能 : 记录计费系统的处理异常情况指标，主要用于表征计费系统的运行情况,AuditId=0016，Type=50,中频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkSystemLossKpi()
{
    cout <<"CheckKpi::checkSystemLossKpi(AuditId=0016，Type=50,中频) ..."<<endl;
    
    StatInterface oSystemLossKpi("0016",50,MIDL_FREG);    
    vector<string> vSystemLossKpi;
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-KPI_ID|"
                        "S-KPI_NAME|"
                        "S-VALUE|"
                        "S-STAT_TIME");                        
    vSystemLossKpi.push_back(string(tmpbuf));       
    
   	long lLogID = 0;
    int iKpiID = 0;
   	char sKpiCode[50] = {0};
   	long lKpiValue = 0;
    char sKpiValue[20+1]={0};
	  char sEndDate[14] = {0};
   	char sLineBuf[MaxLineLength] = {0};
        
    DEFINE_QUERY(qry);
    qry.setSQL (" select t.kpi_log_id,t.kpi_id,t.kpi_name,trim(t.kpi_value) "
				        " from B_SYSTEM_LOSS_KPI t "
				        " where nvl(t.deal_flag, 0) = 0 ");

    qry.open ();
    
    while (qry.next()) 
    {
    	lLogID = qry.field(0).asLong();
    	iKpiID = qry.field(1).asInteger();
    	
    	memset(sKpiCode, 0, sizeof(sKpiCode));
    	strncpy(sKpiCode, qry.field(2).asString(), 50);
    	
    	//lKpiValue = qry.field(3).asLong();
    	memset(sKpiValue,0,sizeof(sKpiValue));
	   	strncpy(sKpiValue,qry.field(3).asString(),20);
	   	strncpy(sEndDate, oSystemLossKpi.sEndDate, 12);	   	
	   	
    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%d|%d|%s|%s|%s", DR_ID,m_iProveNum, iKpiID, sKpiCode, sKpiValue, sEndDate);
    	
    	vSystemLossKpi.push_back(string(sLineBuf));
    	
    	updateMouldeCapabilityKpiLog(lLogID);
    }
    
    qry.close ();
    
    //调用接口入库
	 //oSystemLossKpi.writeFile(vSystemLossKpi);
    oSystemLossKpi.writeJsonFile(vSystemLossKpi,SPLIT_FLAG); 
    DB_LINK->commit();
       
    return 0;    
}

/*
 *	函 数 名 : checkDayAcctFee
 *	函数功能 : 用来定期（15或是30分钟）提取每个月1号0点截止到目前的总账的信息,AuditId=0060 ，Type=50
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
    来源业务系统    NeType（默认为"HB"）
		地区            areaCode
		收入            income（单位为分）
		统计时间        pickdate（‘yyyymmddhh24mi’）
		确认标识        flag（默认送0）
		确认值          confirmIncome(默认和收入送一样的)
		确认人          confirmStaff（默认送0）
		审核人          verifyStaff（默认送0)

*/
int CheckStatInfo::checkDayAcctFee()
{
   /* cout <<"CheckKpi::checkDayAcctFee(AuditId=0060，Type=50,高频) ..."<<endl;
    
    StatInterface oCheckDayAcctFee("0060",50,HIGH_FREG);    
    vector<string> vCheckDayAcctFee;
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","S-NeType|"
                        "S-areaCode|"
                        "L-income|"
                        "S-pickdate|"
                        "I-flag|"
                        "L-confirmIncome|"
                        "I-confirmStaff|"
                        "I-verifyStaff" );                        
    vCheckDayAcctFee.push_back(string(tmpbuf));       
   
   	char sLineBuf[MaxLineLength] = {0};
   	char sValue[1001] = {0};
    char sProvinceCode[5] = {0};
    char sDate[15]={0};
    long lValue = 0;
    int  iBillingCycleID = 0;
    	
   	if (!ParamDefineMgr::getParam("CHECK_STAT", "CITY_CODE", sValue, -1))
    {
    	Log::log (0, "请检查CHECK_STAT的CITY_CODE配置");
    	return -1;
    }   
    
   	strncpy(sProvinceCode, sValue, 4);  
   	
   	{
	    BillingCycleMgr bcm;
	    BillingCycle *bc = bcm.getOccurBillingCycle(1,oCheckDayAcctFee.sStatBeginDate);
   
	    if (!bc)
	    {    
	    	Log::log(0, "帐期获取失败");        	    	
	      return false;
	    }
	    
	    iBillingCycleID = bc->getBillingCycleID();   
	    cout<<"统计帐期:"<<iBillingCycleID<<endl;
    }
    
    
   	AcctItemCtl oAcctCtl;
	  lValue = oAcctCtl.getAllfee(iBillingCycleID);
	  
	  strncpy(sDate, oCheckDayAcctFee.sEndDate, 12);	   
     
    sprintf(sLineBuf, "HB|%s|%ld|%s|0|%ld|0|0",sProvinceCode, lValue, sDate,lValue);
    cout<<sLineBuf<<endl;
    vCheckDayAcctFee.push_back(string(sLineBuf));
    oCheckDayAcctFee.writeJsonFile(vCheckDayAcctFee,SPLIT_FLAG); 
    DB_LINK->commit();*/
       
    return 0;    
}

/*--------------------Param fun--------------------*/
/*
 *	函 数 名 : checkParamBalance
 *	函数功能 : 检查每日话单参数,话单总数，总时长，总流量
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/

bool CheckStatInfo::sortVector(const LogInfoData & m1,const LogInfoData & m2)
{
	return atol(m1.m_sLogTime) < atol( m2.m_sLogTime);
}
void CheckStatInfo::checkParamBalance()
{
     SBusiSUM  sTotalSum[3];
     StatInterface oCheckParamPlan;  
     for(int i=0 ; i<3;i++)
     {
     	 sTotalSum[i].m_lLCnt=0;
     	 sTotalSum[i].m_lDuration = 0 ;
   	   sTotalSum[i].m_lAmount = 0;
   	 }
   	   	        
    int iSwitchID=0; 	        
    DEFINE_QUERY(qry);
    qry.setSQL (" select t.switch_id,sum(t.record_cnt),sum(t.duration),sum(t.amount) "
    			" from B_EVENT_FILE_LIST t "
				  " where t.switch_id>0 and t.CREATED_DATE between to_date(:statBeginDate, 'yyyymmddhh24miss')+ 1/(24*3600) and to_date(:statEndDate, 'yyyymmddhh24miss') "
				  " group by t.switch_id " );
	  qry.setParameter("statBeginDate", oCheckParamPlan.sStatBeginDate);				
	  qry.setParameter("statEndDate", oCheckParamPlan.sEndDate);
 
    qry.open ();
    int iBusiClass=0;
    while (qry.next()) 
    {   	
			iSwitchID = qry.field(0).asLong();
	    iBusiClass = oCheckParamPlan.getBusiclassBySwitch(iSwitchID);  
	    switch(iBusiClass) {
	    	case 10:
   	     sTotalSum[0].m_lLCnt += qry.field(1).asLong();
         sTotalSum[0].m_lDuration += qry.field(2).asLong();
         sTotalSum[0].m_lAmount += qry.field(3).asLong();	    		
	    		break;
	    	case 20:
   	     sTotalSum[1].m_lLCnt += qry.field(1).asLong();
         sTotalSum[1].m_lDuration += qry.field(2).asLong();
         sTotalSum[1].m_lAmount += qry.field(3).asLong();	    	    		
	    		break;
	    	case 30:
   	     sTotalSum[2].m_lLCnt += qry.field(1).asLong();
         sTotalSum[2].m_lDuration += qry.field(2).asLong();
         sTotalSum[2].m_lAmount += qry.field(3).asLong();	     	    		
	    		break;
	    	default:
	    		break;
	    	}   		
    }
    //获取核心参数配置
    //initParam();
    //比对数据
    /*
    for(int i=0 ;i<3 ;i++)
    {
    	if(sTotalSum[i].m_lLCnt > m_sBusiParam[i].m_lTicketNums*(100+m_sBusiParam[i].m_iTicketNumsSwing)/100)
    		//ALARMLOG(1,2242310,"%s业务%s",m_cBusiDesc[i],"话单总数高于波动范围");
    		ALARMLOG28(1,MBC_CLASS_HighFee,2242310,"%s业务%s",m_cBusiDesc[i],"话单总数高于波动范围");
    	if(sTotalSum[i].m_lLCnt < m_sBusiParam[i].m_lTicketNums*(100-m_sBusiParam[i].m_iTicketNumsSwing)/100)
    		//ALARMLOG(1,2242310,"%s业务%s",m_cBusiDesc[i],"话单总数低于波动范围");
    		ALARMLOG28(1,MBC_CLASS_HighFee,2242310,"%s业务%s",m_cBusiDesc[i],"话单总数低于波动范围");
    	if(sTotalSum[i].m_lDuration > m_sBusiParam[i].m_lTicketDuration*(100+m_sBusiParam[i].m_iTicketDurationSwing)/100)
    		//ALARMLOG28(1,2242312,"%s业务%s",m_cBusiDesc[i],"话单总时长高于波动范围");
    		ALARMLOG28(1,MBC_CLASS_HighFee,2242312,"%s业务%s",m_cBusiDesc[i],"话单总时长高于波动范围");
    	if(sTotalSum[i].m_lDuration < m_sBusiParam[i].m_lTicketDuration*(100-m_sBusiParam[i].m_iTicketDurationSwing)/100)
    		//ALARMLOG28(1,2242312,"%s业务%s",m_cBusiDesc[i],"话单总时长低于波动范围");
    		ALARMLOG28(1,MBC_CLASS_HighFee,2242312,"%s业务%s",m_cBusiDesc[i],"话单总时长低于波动范围");
    	if(sTotalSum[i].m_lAmount > m_sBusiParam[i].m_lTicketAccumulation*(100+m_sBusiParam[i].m_iTicketAccumulationSwing)/100)
    		//ALARMLOG28(1,2242314,"%s业务%s",m_cBusiDesc[i],"话单总流量高于波动范围");
    		ALARMLOG28(1,MBC_CLASS_HighFee,2242314,"%s业务%s",m_cBusiDesc[i],"话单总流量高于波动范围");
    	if(sTotalSum[i].m_lAmount < m_sBusiParam[i].m_lTicketAccumulation*(100-m_sBusiParam[i].m_iTicketAccumulationSwing)/100)
    		//ALARMLOG28(1,2242314,"%s业务%s",m_cBusiDesc[i],"话单总流量低于波动范围"); 
    		ALARMLOG28(1,MBC_CLASS_HighFee,2242314,"%s业务%s",m_cBusiDesc[i],"话单总流量低于波动范围"); 
    }   		    	
     */
    qry.close ();
    
    return ;    
}


/*
 *	函 数 名 : checkRoamServs
 *	函数功能 : 检查每日最新的漫入漫出用户数
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/

void CheckStatInfo::checkRoamServs()
{
	

		/* m_iRoamInUsers = m_poCmdCom->readIniInteger("MONITOR","roam_in_users",-1); 
     m_iRoamOutsers = m_poCmdCom->readIniInteger("MONITOR","roam_out_users",-1); 
     int iInServs = 1;
     int iOutServs = 1;	
     int iType = 1;
    char sSql[2048]={0};
     
    DEFINE_QUERY(qry);
    DEFINE_QUERY(qry1);
    //qry.setSQL ("select sel_code ,type_id from b_info_roam_sele ");
    qry.setSQL ("select type_id ,seq_code from b_info_roam_sele ");
    qry.open ();
    
    while (qry.next()	)
	  {
       iType=qry.field(0).asInteger();
       memset(sSql,0,sizeof(sSql));
       strcpy(sSql,qry.field(1).asString());
       if(!sSql[0])
       	  return ;
       qry1.setSQL(sSql);
       qry1.open ();
       if(iType==1)//漫入
       	{
         if(qry1.next())
         	 iInServs = qry1.field(0).asInteger();
         }	 
        if(iType==2)//漫出
       	{
         if(qry1.next())
         	 iOutServs = qry1.field(0).asInteger();
         }	       	  
       qry1.close();
    }
    
    qry.close();
    
    if(m_iRoamInUsers < iInServs)
    	//ALARMLOG(1,2242316,"%s","漫入用户数超过告警阀值");
    	ALARMLOG28(1,MBC_CLASS_HighFee,2242316,"%s","漫入用户数超过告警阀值");
    if(m_iRoamOutsers < iOutServs)
      //ALARMLOG(1,2242317,"%s","漫出用户数超过告警阀值");
       ALARMLOG28(1,MBC_CLASS_HighFee,2242317,"%s","漫出用户数超过告警阀值");
      
      */
}	

/*
 *	函 数 名 : checkAddItem
 *	函数功能 : 提取累帐性能指标（AuditId=0302，Type=40)
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
void CheckStatInfo::checkAddItem()
{
    cout <<"CheckKpi::checkAddItem(AuditId=0302，Type=40,中频) ..."<<endl;
    
    char *sKPIID="40-30-02-03-02-001";
    char *sKPINAME="累帐每秒处理记录数";
    char *sKPB="/HB/";
    StatInterface oAddItemAggr("0302",40,MIDL_FREG);    
    vector<string> vAddItemAggr;
    vAddItemAggr.clear();
    vAddItemAggr.push_back(CAPAB_TITLE);
    
    int iPackageID=0;
    long lRecordCnt=0;
    long lDiffTime=0;
    float fAverage=0;
    float fSumAve=0;
    float fValue=0;    
    char sLineBuf[MaxLineLength] = {0};
    DEFINE_QUERY(qry);
    char sSql[1024]={0};
    sprintf(sSql," SELECT a.PACKAGE_ID,sum(b.RECORD_CNT),to_char(b.CREATED_DATE,'yyyymmddhh24miss'),to_char(a.STATE_DATE ,'yyyymmddhh24miss') "
                "FROM B_PACKAGE_STATE a,B_PACKAGE_FILE_LIST b "
                "WHERE a.PACKAGE_ID=b.PACKAGE_ID "
                "AND   a.STATE_DATE BETWEEN to_date('%s','yyyymmddhh24miss')+ 1/(24*3600) AND to_date('%s','yyyymmddhh24miss') "
                "AND   a.STATE='END'  "
                "GROUP BY a.PACKAGE_ID,b.CREATED_DATe,a.STATE_DATE ",
                oAddItemAggr.sStatBeginDate,oAddItemAggr.sEndDate);
	  //qry.setParameter("statBeginDate", oAddItemAggr.sStatBeginDate);				
	  //qry.setParameter("statEndDate", oAddItemAggr.sEndDate);      
	  cout<<sSql<<endl;          
    qry.setSQL(sSql);
    int iNum=0;
    qry.open ();
    
    while (qry.next()) 
    {    
    	 iNum++;
    	 iPackageID = qry.field(0).asInteger();
    	 lRecordCnt = qry.field(1).asLong();
    	 Date dt_s(qry.field(2).asString()) ;
    	 Date dt_e(qry.field(3).asString()) ;
    	 lDiffTime= dt_e.diffSec(dt_s); 
    	 if(!lDiffTime)
    	 	  lDiffTime=1;
    	 fAverage = lRecordCnt / lDiffTime ;
    	 fSumAve += fAverage ;	 
    }
    qry.close();   
    if(iNum) 
      fValue=fSumAve/iNum;     
    
    //fValue=floor(fValue*100.000)/100.000;
    char buf[32]={0};
    sprintf(buf,"%.3f",fValue);
    sprintf(sLineBuf, "%s|%s|%s||%d|%s|%d", sKPIID,sKPINAME, sKPB, 0,buf , 0);
    cout<<sLineBuf<<endl;
    vAddItemAggr.push_back(sLineBuf);  
    oAddItemAggr.writeJsonFile(vAddItemAggr,SPLIT_FLAG); 
    DB_LINK->commit();   
    
    return ;        	
}


/*
 *	函 数 名 : checkAddItemCaution
 *	函数功能 : 提取累帐告警指标（AuditId=0302，Type=20)
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
void CheckStatInfo::checkAddItemCaution()
{
	
	char sDealNums[128] = {0};	

    cout <<"CheckKpi::checkAddItemCaution(AuditId=0302，Type=20,中频) ..."<<endl;
    
    char *sKPIID="20-30-02-03-02-001";
    char *sKPINAME="累帐性能超阀值告警";
    char *sKPB="/HB/";
    char *sAlarmID="030220001";
    StatInterface oAddItemAggr("0302",20,MIDL_FREG);    
    vector<string> vAddItemAggr;
    vAddItemAggr.clear();
    vAddItemAggr.push_back(ALARM_TITLE);
    
    int iPackageID=0;
    long lRecordCnt=0;
    long lDiffTime=0;
    float fAverage=0;
    float fSumAve=0;
    float fValue=0 ;  
    char sLineBuf[MaxLineLength] = {0};
    DEFINE_QUERY(qry);
    char sSql[1024]={0};
    sprintf(sSql," SELECT a.PACKAGE_ID,sum(b.RECORD_CNT),to_char(b.CREATED_DATE,'yyyymmddhh24miss'),to_char(a.STATE_DATE ,'yyyymmddhh24miss') "
                "FROM B_PACKAGE_STATE a,B_PACKAGE_FILE_LIST b "
                "WHERE a.PACKAGE_ID=b.PACKAGE_ID "
                "AND   a.STATE_DATE BETWEEN to_date('%s','yyyymmddhh24miss')+ 1/(24*3600) AND to_date('%s','yyyymmddhh24miss') "
                "AND   a.STATE='END'  "
                "GROUP BY a.PACKAGE_ID,b.CREATED_DATe,a.STATE_DATE ",
                oAddItemAggr.sStatBeginDate,oAddItemAggr.sEndDate);
	  //qry.setParameter("statBeginDate", oAddItemAggr.sStatBeginDate);				
	  //qry.setParameter("statEndDate", oAddItemAggr.sEndDate);      
	  cout<<sSql<<endl;          
    qry.setSQL(sSql);
    int iNum=0;
    qry.open ();
    
    while (qry.next()) 
    {    
    	 iNum++;
    	 iPackageID = qry.field(0).asInteger();
    	 lRecordCnt = qry.field(1).asLong();
    	 Date dt_s(qry.field(2).asString()) ;
    	 Date dt_e(qry.field(3).asString()) ;
    	 lDiffTime= dt_e.diffSec(dt_s); 
    	 if(!lDiffTime)
    	 	  lDiffTime=1;
    	 fAverage = lRecordCnt / lDiffTime ;
    	 fSumAve += fAverage ;	 
    }
    qry.close(); 
    if(iNum) 
      fValue=fSumAve/iNum;
      
    //int iDealNums = m_poCmdCom->readIniInteger("MONITOR","max_deal_item",-1);   
	int iDealNums;
    if( ParamDefineMgr::getParam("MONITOR", "max_deal_item", sDealNums, sizeof(sDealNums)) )
    		iDealNums = atoi(sDealNums);
    else
    		iDealNums	= 10;

    if( fValue > (float)iDealNums)
    {    
     Date dt;
     sprintf(sLineBuf, "%s|%s|%s|%s||%s|%s|%d|%d|||||%d", 
                     sKPIID,sKPINAME,sAlarmID, sKPB, dt.toString(),dt.toString(),
                     1,10,1);
     cout<<sLineBuf<<endl;
     vAddItemAggr.push_back(sLineBuf);  
    }
    oAddItemAggr.writeJsonFile(vAddItemAggr,SPLIT_FLAG); 
    return ;        	
}
/*
 *	函 数 名 : checkRoamServs
 *	函数功能 : 提取出月帐性能指标（AuditId=0304，Type=40)
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
void CheckStatInfo::checkMonthCap()
{
    cout <<"CheckKpi::checkMonthCap(AuditId=0304，Type=40,低频) ..."<<endl;
    
    char *sKPIID="40-30-02-03-04-001";
    char *sKPINAME="出月帐完成时长（秒）";
    char *sKPB="/HB/";
    
    StatInterface oMonthCap("0304",40,LOW2_FREG);    
    vector<string> vMonthCap;
    vMonthCap.clear();
    vMonthCap.push_back(CAPAB_TITLE);
    
    char sLineBuf[MaxLineLength] = {0};
    DEFINE_QUERY(qry);
    qry.setSQL (" select log_id,used_time "
				        " from b_info_capability_cycle t "
				        " where t.deal_flag = 0 ");

    qry.open ();
    
    while (qry.next()) 
    {    
    	 int   iLogID = qry.field(0).asInteger();
    	 long  used_time=qry.field(1).asLong();
       memset(sLineBuf,0,MaxLineLength);
       sprintf(sLineBuf, "%s|%s|%s||%d|%ld|%d", sKPIID,sKPINAME, sKPB, 0, used_time, 0);
       vMonthCap.push_back(sLineBuf);
       updateMonthCapa(iLogID);
    }
    qry.close();
    oMonthCap.writeJsonFile(vMonthCap,SPLIT_FLAG); 
    DB_LINK->commit();         
    return ;        	
}

/*
 *	函 数 名 : checkAcctCap
 *	函数功能 : 提取销账性能指标（AuditId=0301，Type=40)
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
void CheckStatInfo::checkAcctCap(char *sAudit,int iType,char *sFreg)
{
    cout <<"CheckKpi::checkMonthCap(AuditId="<<sAudit<<"Type="<<iType<<"中频) ..."<<endl;
    
    StatInterface oACCTCap(sAudit,iType,sFreg);    
    vector<string> vACCTCap;
    vACCTCap.clear();
    vACCTCap.push_back(CAPAB_TITLE);
    
    char sLineBuf[MaxLineLength] = {0};
    int iLogID=0;
    DEFINE_QUERY(qry);
     char sSql[1024]={0};
    sprintf(sSql,"SELECT a.KPI_LOG,a.KPIID,a.KPINAME,a.KBP,a.MIN_VALUE,a.AVG_VALUE,a.MAX_VALUE "
                "FROM B_INFO_CAPABILITY_INTERFACE a "
                " WHERE a.DEAL_FLAG=0 AND   a.KPI_AUDITID='%s' "
                " AND   a.KPI_TYPE = %d "
                " AND   a.CREATE_DATE between to_date('%s', 'yyyymmddhh24miss')+ 1/(24*3600) and to_date('%s', 'yyyymmddhh24miss')",
                  sAudit,iType,oACCTCap.sStatBeginDate,oACCTCap.sEndDate);
     qry.setSQL(sSql);           
     qry.open ();
    
    while (qry.next()) 
    {    
    	iLogID = qry.field(0).asInteger();
    	memset(sLineBuf,0,MaxLineLength);
    	sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         qry.field(1).asString(),
                         qry.field(2).asString(),
                         qry.field(3).asString(),
                         atof(qry.field(4).asString()),
                         atof(qry.field(5).asString()),
                         atof(qry.field(6).asString())
                         );
       vACCTCap.push_back(sLineBuf);
       updateACCTCapa(iLogID);
    }
    //判断是否取到数据，如果没取到则造手工数据
    if(vACCTCap.size()==1)
    {
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-02-03-01-001","帐户销帐每秒处理笔数","HB","0","0","0");
        vACCTCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-02-03-01-002","批销每秒处理记录数","HB","0","0","0");
        vACCTCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-02-03-01-003","销账损耗","HB","0","0","0");
        vACCTCap.push_back(sLineBuf);

        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-02-04-01-001","银行业务处理总数","HB","0","0","0");
        vACCTCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-02-04-01-002","银行业务处理成功数","HB","0","0","0");
        vACCTCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-02-04-01-003","银行业务处理成功率","HB","0","0","0");
        vACCTCap.push_back(sLineBuf);

        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-02-04-02-001","充值卡接口处理总数","HB","0","0","0");
        vACCTCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-02-04-02-002","充值卡接口处理成功数","HB","0","0","0");
        vACCTCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-02-04-02-003","充值卡接口业务处理成功率","HB","0","0","0");
        vACCTCap.push_back(sLineBuf);

        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-02-09-00-001","用户查询失败率","HB","0","0","0");
        vACCTCap.push_back(sLineBuf);

    }
    qry.close();
    oACCTCap.writeJsonFile(vACCTCap,SPLIT_FLAG); 
    DB_LINK->commit();         
    return ;        	
}

// ohter fun  

long CheckStatInfo::getSplitRecord(long lFileID)
{
    DEFINE_QUERY(qry);
    long lSplitRecord = 0;
    
    qry.setSQL(" select sum(t.remain_a) split_record from wf_stat_log t where t.file_id=:file_id "
			   " and t.process_id in (select a.process_id from wf_process a where a.app_id = :prep_app_id) ");

    qry.setParameter ("file_id", lFileID);    
    qry.setParameter ("prep_app_id", PREP_APP_ID);
    
	 qry.open();    
    
    if (qry.next())
    {
    	lSplitRecord = qry.field(0).asLong();
    }
    
    return lSplitRecord;
}	

long CheckStatInfo::getMergeRecord(long lFileID)
{
    DEFINE_QUERY(qry);
    long lMergeRecord = 0;
    
    qry.setSQL(" select sum(t.remain_b) merge_record from wf_stat_log t where t.file_id=:file_id "
			   " and t.process_id in (select a.process_id from wf_process a where a.app_id = :merge_app_id) ");

    qry.setParameter ("file_id", lFileID);    
    qry.setParameter ("merge_app_id", MERGER_APP_ID);
    
    qry.open();
    
    if (qry.next())
    {
    	lMergeRecord = qry.field(0).asLong();
    }
    
    return lMergeRecord;	
}

bool CheckStatInfo::newTicketLog(const char* sKey, int iDataSource, int iSwitchID, int iBusiClass, int iTicketTypeID,
					long lEventRec, long lDuration, long lAmount, long lCharge)
{
	TicketLog *pTicketLog = new TicketLog();
	pTicketLog->m_iDataSource = iDataSource;
	pTicketLog->m_iSwitchID = iSwitchID;
	pTicketLog->m_iBusiClass = iBusiClass;
	pTicketLog->m_iTicketTypeID = iTicketTypeID;
	pTicketLog->m_lCurrRecCnt = lEventRec;
	pTicketLog->m_lCurrDuration = lDuration;
	pTicketLog->m_lCurrAmount = lAmount;
	pTicketLog->m_lCurrFee = lCharge;
	
	m_mapTicketLog[string(sKey)] = pTicketLog;
	
	return true;
}					
					
bool CheckStatInfo::addTicketLog(TicketLog *pTicketLog, int iDataSource, int iSwitchID, int iBusiClass, int iTicketTypeID,
					long lEventRec, long lDuration, long lAmount, long lCharge)		
{
	//pTicketLog->m_iDataSource += iDataSource;
	//pTicketLog->m_iSwitchID += iSwitchID;
	//pTicketLog->m_iBusiClass += iBusiClass;
	//pTicketLog->m_iTicketTypeID += iTicketTypeID;
	pTicketLog->m_lCurrRecCnt += lEventRec;
	pTicketLog->m_lCurrDuration += lDuration;
	pTicketLog->m_lCurrAmount += lAmount;
	pTicketLog->m_lCurrFee += lCharge;
	
	return true;	
}			

void CheckStatInfo::updateMouldeCapabilityKpiLog(long lLogID)
{
  DEFINE_QUERY(qry);
	qry.setSQL(" update B_MODULE_CAPABILITY_KPI t "
			   " set t.deal_flag = 1 "
			   " where t.kpi_log_id =:log_id ");

	qry.setParameter("log_id", lLogID);			   	
	qry.execute();		   
}	

void CheckStatInfo::insertMouldeCapabilityKpiLog(int iMouldeID, long lValue, const char* sKpiName)
{
    DEFINE_QUERY(qry);
	
	qry.setSQL(" insert into B_MODULE_CAPABILITY_KPI "
				" (kpi_log_id, kpi_id, kpi_name, kpi_value, create_date, deal_flag) "
				" values "
				" (B_MODULE_CAPABILITY_KPI_SEQ.Nextval, :kpi_id, :kpi_name, :kpi_value, sysdate, 0) ");
	
	qry.setParameter("kpi_id", iMouldeID);
	qry.setParameter("kpi_value", lValue);
	qry.setParameter("kpi_name", sKpiName);
	
	qry.execute();
	qry.commit();
}


long CheckStatInfo::prepMouldeCapability()
{
    DEFINE_QUERY(qry);
    
    long lRecordCnt = 0;
    
    qry.setSQL(" select sum(record_cnt) record_cnt from b_event_file_list_cfg t "
				" where exists "
				" (select 1 from wf_process a "
				" where t.process_id=a.process_id "
				" and a.app_id=:prep_app_id) "
				" and t.state = 'END' "
				" and t.state_date between to_date(:stat_begin_date, 'yyyymmddhh24miss')+1/(24*3600) and to_date(:stat_end_date, 'yyyymmddhh24miss')");

	qry.setParameter("prep_app_id", PREP_APP_ID);
	qry.setParameter("stat_begin_date", StatInterface::m_pPlan->m_sDataBeginTime);
	qry.setParameter("stat_end_date", StatInterface::m_pPlan->m_sDataEndTime);
	
	qry.open();
	
	if(qry.next())
	{
		lRecordCnt = qry.field(0).asLong();
	}
	
	qry.close();
	
	long lCapability = lRecordCnt/atol(StatInterface::m_pPlan->m_sTimeInterval);
	
	insertMouldeCapabilityKpiLog(PREP_CAPABILITY_KPI, lCapability, PREP_CAPABILITY_NAME);
	
	return lCapability;
}	

long CheckStatInfo::pricingMouldeCapability()
{
    DEFINE_QUERY(qry);
    
    long lRecordCnt = 0;
    
    qry.setSQL(" select sum(out_normal_event) record_cnt from wf_stat_log t "
				" where exists "
				" (select 1 from wf_process a "
				" where t.process_id=a.process_id "
				" and a.app_id=:pricing_app_id) "
				" and t.msg_date||t.msg_time between :stat_begin_date and :stat_end_date ");

	qry.setParameter("pricing_app_id", PRICING_APP_ID);
	qry.setParameter("stat_begin_date", StatInterface::m_pPlan->m_sDataBeginTime);
	qry.setParameter("stat_end_date", StatInterface::m_pPlan->m_sDataEndTime);
	
	qry.open();
	
	if(qry.next())
	{
		lRecordCnt = qry.field(0).asLong();
	}
	
	qry.close();
	
	long lCapability = lRecordCnt/atol(StatInterface::m_pPlan->m_sTimeInterval);
	
	insertMouldeCapabilityKpiLog(PRICING_CAPABILITY_KPI, lCapability, PRICING_CAPABILITY_NAME);
	
	return lCapability;
}


void CheckStatInfo::upHighFeeRecord(long lLogID)
{
  DEFINE_QUERY(qry);
	qry.setSQL(" update B_HIGHFEE_ALARM_LOG t "
			   " set t.OUT_DB_FLAG = 1 "
			   " where t.HIGH_FEE_ID =:log_id ");

	qry.setParameter("log_id", lLogID);			   	
	qry.execute();	
	qry.commit();
	qry.close();	   
}	

bool CheckStatInfo::setBillingCycleID(const char* sDate)
{
/*
    BillingCycleMgr bcm;
    BillingCycle *bc = bcm.getOccurBillingCycle(1, (char *)sDate);
    strncpy (m_sCheckDate, sDate, 8);    
    if (!bc)
    {    
    	Log::log(0, "帐期获失败");        	    	
      return false;
    }
    
    m_iBillingCycleID = bc->getBillingCycleID();    
*/  
	m_iBillingCycleID = 20110612;      
    //Log::log(0, "帐期:%d,稽核日期:%s", m_iBillingCycleID, m_sCheckDate);    
    cout<<"帐期:"<<m_iBillingCycleID<<"稽核日期:"<<m_sCheckDate<<endl;
    return true;	
}

void  CheckStatInfo::insertCheckLog (long _lFileID, int _iModuleID, const char *_state, bool bCommit) 
{
    DEFINE_QUERY (qry);
    qry.setSQL ("insert into b_check_file_log (file_id, MODULE_ID, check_state, check_date) "
            " values (:vFileID, :vMod, :vState, sysdate) " );
    qry.setParameter ("vFileID", _lFileID);
    qry.setParameter ("vMod", _iModuleID);
    qry.setParameter ("vState", _state);
    qry.execute();
    if (bCommit)
    {	
    	qry.commit();
    }
    qry.close();
}


void CheckStatInfo::regeditParam()
{
	
	 //modified by zhangyz 2010/11/08
    
   /*if(!m_poCmdCom->InitClient())
    {
       Log::log(0,"注册核心参数失败");
       	return ;
    }        
    m_poCmdCom->GetInitParam("BUSINESS");
    m_poCmdCom->SubscribeCmd("BUSINESS",MSG_PARAM_CHANGE);    
        
    m_poCmdCom->GetInitParam("MONITOR");
    m_poCmdCom->SubscribeCmd("MONITOR",MSG_PARAM_CHANGE);   
    
    m_poCmdCom->GetInitParam("LOG");
    m_poCmdCom->SubscribeCmd("LOG",MSG_PARAM_CHANGE);   
    
	  Log::m_iAlarmLevel = m_poCmdCom->readIniInteger("LOG","log_alarm_level",-1);    
	  Log::m_iAlarmDbLevel = m_poCmdCom->readIniInteger("LOG","log_alarmdb_level",-1);  
	  */    

    for(int i=0;i<3;i++)
    {
      memset(m_cBusiDesc[i],0 ,5);
    }
    strcpy(m_cBusiDesc[0],"call");
    strcpy(m_cBusiDesc[1],"data");   
    strcpy(m_cBusiDesc[2],"sms");    
    
     
                         	
}

void CheckStatInfo::initParam()
{
	char sTicketNums[128] = {0};
	 char sTicketNumsSwing[128] = {0};	
	 char sTicketDuration[128] = {0};
	 char sTicketDurationSwing[128] = {0};
	 char sTicketAccumulation[128] = {0};	
	 char sTicketAccumulationSwing[128] = {0};

   char cBusi[64+1]={0};   
   for(int i=0;i<3;i++)
   { 
   	// memset(cBusi,0,sizeof(cBusi));
   	// sprintf(cBusi,"%s.%s",m_cBusiDesc[i],"ticket_nums");
		// m_sBusiParam[i].m_lTicketNums = m_poCmdCom->readIniInteger("BUSINESS",cBusi,-1); 
		if (ParamDefineMgr::getParam("INFO_DATA", "ticket_nums", sTicketNums, sizeof(sTicketNums)))
		 		m_sBusiParam[i].m_lTicketNums = atoi(sTicketNums);
		 else 
		 		m_sBusiParam[i].m_lTicketNums = 10;


   	 //memset(cBusi,0,sizeof(cBusi));
   	 //sprintf(cBusi,"%s.%s",m_cBusiDesc[i],"ticket_nums_swing");		 
		// m_sBusiParam[i].m_iTicketNumsSwing = m_poCmdCom->readIniInteger("BUSINESS",cBusi,-1);
	if (ParamDefineMgr::getParam("INFO_DATA", "ticket_nums_swing", sTicketNumsSwing, sizeof(sTicketNumsSwing)))
		 		m_sBusiParam[i].m_iTicketNumsSwing = atoi(sTicketNumsSwing);
		 else 
		 		m_sBusiParam[i].m_iTicketNumsSwing = 10;	


   	// memset(cBusi,0,sizeof(cBusi));
   	 //sprintf(cBusi,"%s.%s",m_cBusiDesc[i],"ticket_duration");		  
		// m_sBusiParam[i].m_lTicketDuration = m_poCmdCom->readIniInteger("BUSINESS",cBusi,-1); 
	if (ParamDefineMgr::getParam("INFO_DATA", "ticket_duration", sTicketDuration, sizeof(sTicketDuration)))
		 		m_sBusiParam[i].m_lTicketDuration = atoi(sTicketDuration);
		 else 
		 		m_sBusiParam[i].m_lTicketDuration = 10;	


   	// memset(cBusi,0,sizeof(cBusi));
   	 //sprintf(cBusi,"%s.%s",m_cBusiDesc[i],"ticket_duration_swing");		 
		// m_sBusiParam[i].m_iTicketDurationSwing = m_poCmdCom->readIniInteger("BUSINESS",cBusi,-1);
	if (ParamDefineMgr::getParam("INFO_DATA", "ticket_duration_swing", sTicketDurationSwing, sizeof(sTicketDurationSwing)))
		 		m_sBusiParam[i].m_iTicketDurationSwing = atoi(sTicketDurationSwing);
		 else 
		 		m_sBusiParam[i].m_iTicketDurationSwing = 10;	



   	// memset(cBusi,0,sizeof(cBusi));
   	 //sprintf(cBusi,"%s.%s",m_cBusiDesc[i],"ticket_accumulation");		 
		// m_sBusiParam[i].m_lTicketAccumulation = m_poCmdCom->readIniInteger("BUSINESS",cBusi,-1);
	if (ParamDefineMgr::getParam("INFO_DATA", "ticket_accumulation", sTicketAccumulation, sizeof(sTicketAccumulation)))
		 		m_sBusiParam[i].m_lTicketAccumulation = atoi(sTicketAccumulation);
		 else 
		 		m_sBusiParam[i].m_lTicketAccumulation = 10;	



   	// memset(cBusi,0,sizeof(cBusi));
   	 //sprintf(cBusi,"%s.%s",m_cBusiDesc[i],"ticket_accumulation_swing");		
		// m_sBusiParam[i].m_iTicketAccumulationSwing = m_poCmdCom->readIniInteger("BUSINESS",cBusi,-1);    
		if (ParamDefineMgr::getParam("INFO_DATA", "ticket_accumulation", sTicketAccumulationSwing, sizeof(sTicketAccumulationSwing)))
		 		m_sBusiParam[i].m_iTicketAccumulationSwing = atoi(sTicketAccumulationSwing);
		 else 
		 		m_sBusiParam[i].m_iTicketAccumulationSwing = 10;


	 }  	
}
/*
 *	函 数 名 : checkEventInfo
 *	函数功能 : 提取系统状态的各种事件信息点（AuditId=0900，Type=60）
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 

void CheckStatInfo::checkEventInfo()
{
	  cout <<"CheckProcess::checkAlertLog(AuditId=0900，Type=60,高频) ..."<<endl;
   
	 StatInterface oSystemCaution("0900",60,HIGH_FREG);
	 vector<string> vCheckEvenLog;
	 vCheckEvenLog.clear();
	 vCheckEvenLog.push_back(EVENT_TITLE);
	 
	  char sStatBeginDate[14+1]={0};
    char sEndDate[14+1]={0};
   	char sTempAlertCode[5] = {0};	 
   	char sTempAlertContent[256+1] = {0};
   	char sTime[14+1]={0};
   	int iProcessID ;
   	int iOrgAlertCode;
   	char *SKPI_START="60-30-02-09-00-001" ;
   	char *SKIP_START_NAME="系统启动成功事件通知";
   	char *SKPI_END= "60-30-02-09-00-002";
   	char *SKIP_END_NAME= "系统停止成功事件通知" ;
   	
   	char stmp[1024]={0};
   	
   	strncpy(sStatBeginDate,oSystemCaution.sStatBeginDate,14);
    strncpy(sEndDate,oSystemCaution.sEndDate,14);  

    memset(stmp,0,sizeof(stmp));
    if(strstr(sTempAlertContent,"进程启动"))
    {
    	sprintf(stmp,"%s|%s|%s|HSS||%s|%s|%s|1",
    	SKPI_START,SKIP_START_NAME,vRet[i].m_illCode,sTime,sTime,sTempAlertContent );
    	vCheckEvenLog.push_back(stmp);
    }
    if(strstr(sTempAlertContent,"正常退出"))
    {
    	sprintf(stmp,"%s|%s|090060001|HSS||%s|%s|%s|1",
    	SKPI_END,SKIP_END_NAME,vRet[i].m_illCode,sTime,sTime,sTempAlertContent );     		
    	vCheckEvenLog.push_back(stmp);     		
     }
  
   oCheckEvenLog.writeJsonFile(vCheckEvenLog,'|');
}


/*
 *	函 数 名 : checkEventInfoNew
 *	函数功能 : 提取系统状态的各种事件信息点（AuditId=0900，Type=60）
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
void CheckStatInfo::checkEventInfoNew()
{
	  cout <<"CheckProcess::checkEventInfoNew(AuditId=0900，Type=60,高频) ..."<<endl;
   
	 StatInterface oCheckEvenLog("0900",60,HIGH_FREG);
	 vector<string> vCheckEvenLog;
	 vCheckEvenLog.clear();
	 vCheckEvenLog.push_back(EVENT_TITLE);
	 
	 
   	char sTempAlertCode[5] = {0};	 
   	char sTempAlertContent[256+1] = {0};
   	char sTime[14+1]={0};
   	int iProcessID ;
   	int iOrgAlertCode;
   	char *SKPI_START="60-30-02-09-00-001" ;
   	char *SKIP_START_NAME="系统启动成功事件通知";
   	char *SKPI_END= "60-30-02-09-00-002";
   	char *SKIP_END_NAME= "系统停止成功事件通知" ;
   	char *KPI_EVENT_PRODUCT ="60-30-02-09-00-003";	
    char *KPI_EVENT_PROD_NAME ="产品目录更新事件";     	
    char *KPI_EVENT_OFFER ="60-30-02-09-00-004";
    char *KPI_EVENT_OFFER_NAME ="商品目录更新事件";     
    char *KPI_EVENT_JIYA ="60-30-02-09-00-005";
    char *KPI_EVENT_JIYA_NAME ="定价计划更新时间";    
    char *KPI_EVENT_YICH ="60-30-02-09-00-006";	
    char *KPI_EVENT_YICH_NAME ="当日对外接口无异常事件";    
    char *KPI_EVENT_PARAM ="60-30-02-09-00-007";	
    char *KPI_EVENT_PARAM_NAME ="参数加载成功";   
   	
    char stmp[1024]={0};
    char sSql[1024]={0};
    char sStatBeginDate[14+1]={0};
    char sEndDate[14+1]={0};
    strncpy(sStatBeginDate,oCheckEvenLog.sStatBeginDate,14);
    strncpy(sEndDate,oCheckEvenLog.sEndDate,14);  
		
	ThreeLogGroup Log; 	
    vector<LogInfoData> vRet;  
    vRet.clear();
    
    Log.GetLoggInfo(vRet,GROUPTYPE,LOG_TYPE_PROC,sStatBeginDate,sEndDate); //系统启动成功 
    cout<<"get success"<<endl;
   	int iVecSize = vRet.size();
   		
    sort(vRet.begin(),vRet.end(),sortVector);   
    if (iVecSize)
    { 
   		for(int i = 0; i<vRet.size();i++)
   	  	{
   	   		memset(stmp, 0, sizeof(stmp));	  	   		   	
      		sprintf(stmp,"%s|%s|60-30-12-04-00-001|/HSS|%s|%s|%s|系统启动成功|%d",
    		        SKPI_START,SKIP_END_NAME,vRet[0].m_sLogTime,vRet[iVecSize-1].m_sLogTime,iVecSize); 		           
        	vCheckEvenLog.push_back(string(stmp));
      	}
    }  

    Log.GetLoggInfo(vRet,GROUPTYPE,LOG_TYPE_PROC,sStatBeginDate,sEndDate); //系统停止成功
    iVecSize = vRet.size();
    if (iVecSize)
    { 
   		for(int i = 0; i<vRet.size();i++)
   	  	{
   	   		memset(stmp, 0, sizeof(stmp));	   	   		   	
      		sprintf(stmp,"%s|%s|60-30-12-04-00-002|/HSS|%s|%s|%s|系统停止|%d",
    				SKPI_START,SKIP_END_NAME,vRet[i].m_illCode,vRet[0].m_sLogTime,vRet[iVecSize-1].m_sLogTime,iVecSize );   		           
        	vCheckEvenLog.push_back(string(stmp));
      	}
    }
    Log.GetLoggInfo(vRet,GROUPTYPE,LOG_TYPE_PROC,sStatBeginDate,sEndDate); //参数加载成功
    iVecSize = vRet.size();
    if (iVecSize)
    { 
   		for (int i = 0; i < vRet.size(); i++)
   	  	{
   	   		memset(stmp, 0, sizeof(stmp));	   	   		   	
      		sprintf(stmp,"%s|%s|60-30-12-04-00-003|/HSS|%s|%s|%s|参数加载|%d",
    		        SKPI_START,SKIP_END_NAME,vRet[i].m_illCode,vRet[0].m_sLogTime,vRet[iVecSize-1].m_sLogTime,iVecSize );  		           
        	vCheckEvenLog.push_back(string(stmp));
      	}
    }
    
    oCheckEvenLog.writeJsonFile(vCheckEvenLog,SPLIT_FLAG);
}

/*
 *	函 数 名 : checkHSSEventInfo
 *	函数功能 : 提取HSS系统状态的各种事件信息点rec
 （AuditId=0400，Type=60）
 *	时    间 : 2011年6月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkHSSEventInfo()
{
	  cout <<"CheckProcess::checkHSSEventInfo(AuditId=0400，Type=60,高频) ..."<<endl;
   
	 StatInterface oCheckEvenLog("0400",60,HIGH_FREG);
	 vector<string> vCheckEvenLog;
	 vCheckEvenLog.clear();
	 vCheckEvenLog.push_back(EVENT_TITLE);
	 
	 
   	char sTempAlertCode[5] = {0};	 
   	char sTempAlertContent[256+1] = {0};
   	char sTime[14+1]={0};
   	int iProcessID ;
   	int iOrgAlertCode;
   	char *SKPI_START="60-30-12-04-00-001" ;
   	char *SKIP_START_NAME="系统启动成功事件通知";
   	char *SKPI_END= "60-30-12-04-00-002";
   	char *SKIP_END_NAME= "系统停止成功事件通知" ;  
    char *KPI_EVENT_PARAM ="60-30-12-04-00-003";	
    char *KPI_EVENT_PARAM_NAME ="参数加载成功";   
   	
    char stmp[1024]={0};
    char sSql[1024]={0};
    char sStatBeginDate[14+1]={0};
    char sEndDate[14+1]={0};
    strncpy(sStatBeginDate,oCheckEvenLog.sStatBeginDate,14);
    strncpy(sEndDate,oCheckEvenLog.sEndDate,14);  
		
	ThreeLogGroup Log;  
	
    vector<LogInfoData> vRet;  
    vRet.clear();  
    Log.GetLoggInfo(vRet,GROUPCODE,MBC_ACTION_SYSTEM_START,sStatBeginDate,sEndDate); //系统启动成功 
       	
   	int iVecSize = vRet.size();
   		
    sort(vRet.begin(),vRet.end(),sortVector);   
    if (iVecSize)
    { 
   	  		char AlarmID[10] = {0};
   	  		sprintf(AlarmID,"%d",MBC_ACTION_SYSTEM_START);
   	   		memset(stmp, 0, sizeof(stmp));	  	   		   	
      		sprintf(stmp,"%s|%s|%s|/HSS||%s|%s|%s|%d",
    		        SKPI_START,SKIP_START_NAME,AlarmID,vRet[0].m_sLogTime,vRet[iVecSize-1].m_sLogTime,
    		        vRet[0].m_sCodeName,iVecSize); 		           
        	vCheckEvenLog.push_back(string(stmp));
    }  
    
    vRet.clear();
    Log.GetLoggInfo(vRet,GROUPCODE,MBC_ACTION_SYSTEM_END,sStatBeginDate,sEndDate); //系统停止成功
    iVecSize = vRet.size();
    if (iVecSize)
    { 
    	    char AlarmID[10] = {0};
   	  		sprintf(AlarmID,"%d",MBC_ACTION_SYSTEM_END);
   	   		memset(stmp, 0, sizeof(stmp));	   	   		   	
      		sprintf(stmp,"%s|%s|%s|/HSS||%s|%s|%s|%d",
    				SKPI_END,SKIP_END_NAME,AlarmID,vRet[0].m_sLogTime,vRet[iVecSize-1].m_sLogTime,
    				vRet[0].m_sCodeName,iVecSize );   		           
        	vCheckEvenLog.push_back(string(stmp));
    }
    
    vRet.clear();
    Log.GetLoggInfo(vRet,GROUPCODE,MBC_ACTION_LOAD_PARAM,sStatBeginDate,sEndDate); //参数加载成功
    iVecSize = vRet.size();
    if (iVecSize)
    { 
    	    char AlarmID[10] = {0};
   	  		sprintf(AlarmID,"%d",MBC_ACTION_LOAD_PARAM);
   	   		memset(stmp, 0, sizeof(stmp));	   	   		   	
      		sprintf(stmp,"%s|%s|%s|/HSS||%s|%s|%s|%d",
    		        KPI_EVENT_PARAM,KPI_EVENT_PARAM_NAME,AlarmID,vRet[0].m_sLogTime,vRet[iVecSize-1].m_sLogTime,
    		        vRet[0].m_sCodeName,iVecSize );  		           
        	vCheckEvenLog.push_back(string(stmp));
    }
    
    oCheckEvenLog.writeJsonFile(vCheckEvenLog,SPLIT_FLAG);
    
    return 1;
}

int CheckStatInfo::updateMonthCapa(int iLogID)
{
	DEFINE_QUERY(qry);
	qry.setSQL(" update b_info_capability_cycle t "
			   " set t.deal_flag = 1 "
			   " where t.log_id =:log_id ");

	qry.setParameter("log_id", iLogID);			   	
	qry.execute();	
	qry.commit();
	qry.close();	 
	return 1; 
}

int CheckStatInfo::updateACCTCapa(int iLogID)
{
	DEFINE_QUERY(qry);
	qry.setSQL(" update B_INFO_CAPABILITY_INTERFACE t "
			   " set t.deal_flag = 1 ,t.STATE_DATE=sysdate "
			   " where t.KPI_LOG =:log_id ");

	qry.setParameter("log_id", iLogID);			   	
	qry.execute();	
	qry.commit();
	qry.close();	  
	return 1;
}


/*                   从日志组获取信息的信息点接口函数版本                     */

/*
 *	函 数 名 : checkHighFeeServ
 *	函数功能 : 提取采集计费系统中省级疑似高额记录,AuditId=0012，Type=50,高频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkHighFeeLog()
{/*
    cout <<"CheckStatInfo::checkHighFeeServ(AuditId=0012，Type=50,高频) ..."<<endl;
	   Environment::useUserInfo ();  
    StatInterface oCheckHighFee("0012",50,HIGH_FREG);
    vector<string> vCheckHighFee;
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-BUSI_CLASS|"
                        "S-BUSI_CLASS_NAME|"
                        "S-FOUND_TIME|"
                        "S-BILLING_NUMBER|"
                        "S-DURATION|"
                        "S-DATA_VOLUME|"
                        "S-FEE|"
                        "I-WHITE_LIST|"
                        "I-CREDIT_VALUE|"
                        "I-STOP_FLAG|"
                        "I-USER_AMOUNT");                        
    vCheckHighFee.push_back(string(tmpbuf));    
        	
     BalanceMgr oBalanceMgr;
     AcctItemAccuMgr oAcctItemAccuMgr;
     vector<AcctItemData *> vct;
     vector<AcctItemData *>::iterator iter;
     	
     vector<string> vHihgFeeLog ;
     vector<string> vFiledTag;
     vHihgFeeLog.clear();
     vFiledTag.clear();
     
     m_pThreeLog->GetHighFeeLog(vHihgFeeLog);
     //[2010-10-02 02:39:30]: [ALARM][HIGHFEE][2901|国内短信高额欺诈|20101002023930|13368917929|21600|180000|210000|20100920231612]
     // string teststr("[2010-10-02 02:39:30]: [ALARM][HIGHFEE][2901|国内短信高额欺诈|20101002023930|13368917929|21600|180000|210000|20100920231612]");
     // vHihgFeeLog.push_back(teststr);
     char sLine[1024];
     char *sAlarm="[HIGHFEE]" ;
     char cDiv='|';
     vector<string>::iterator iterS;
     cout<<"共接受原始信息数据"<<vHihgFeeLog.size()<<endl;
     for(iterS=vHihgFeeLog.begin();iterS!=vHihgFeeLog.end();++iterS)
     {
     	  cout<<(*iterS)<<endl;
     	  if(!(*iterS).compare(sAlarm))
     	  	 continue ;
     	  	 
     	 memset(sLine,0,sizeof(sLine));
     	 strncpy(sLine,(*iterS).c_str(), 1023);
     	 char *pbegin=sLine;
     	 char *p=0;
     	 //解析时间[yyyy-mm-dd hh:mi:ss]
     	 pbegin++;
     	 p=strchr(pbegin,']');
     	 if(p) *p=0;
     	 Date dt(pbegin,"yyyy-mm-dd hh:mi:ss");
     	 char sLogTime[14+1]={0};
     	 strncpy(sLogTime,dt.toString(),14);
     	 if( strcmp(sLogTime,oCheckHighFee.sStatBeginDate) < 0 ||
     	 	   strcmp(sLogTime,oCheckHighFee.sEndDate) > 0 )
     	 	 continue ;
     	 
     	 vFiledTag.clear();
     	 //解析高额告警数据  [ALARM][HIGHFEE][ID|Name|Time|Nbr|duration|amount|charge|CDRTime]
     	 for(int i=0;i<2;i++)
     	 { 
     	   p++;
     	   pbegin = p ;
     	   p=strchr(pbegin,']');
     	 }
     	 
     	 p=p+2;
     	 pbegin=p;
     	 p=strchr(pbegin,cDiv);
       while(p){
          *p = 0;
          if(*pbegin)vFiledTag.push_back(pbegin);
          p++;
          pbegin = p;
          p = strchr(pbegin, cDiv );
       }
      if( pbegin && (*pbegin) )
        vFiledTag.push_back(pbegin);
         
      char sLineBuf[MaxLineLength] = {0};    
      long lHighfeeID ;
      int  iBusiID ;
	    char sBusiName[50]={0};
	    char sFoundTime[14+1]={0};
	    char sCdrTime[14+1]={0};
	    char sBillingNbr[20]={0};
   	  long lDuration ;
   	  long lAmount ;
   	  long lCharge ;
   	  long iStopFlag;
   	  long lServID;
   	  long lAcctID;
   	  long lAllCharge;
   	  long lAcctBalance ;
      long lUserBalace  ;
      long lCredit ;  
        
      iBusiID = atoi(vFiledTag[0].c_str());
      strcpy(sBusiName,vFiledTag[1].c_str());
      strncpy(sFoundTime,vFiledTag[2].c_str(),14);
	   	strcpy(sBillingNbr,vFiledTag[3].c_str());
   		lDuration = atol(vFiledTag[4].c_str());
   		lAmount = atol(vFiledTag[5].c_str());
   		lCharge = atol(vFiledTag[6].c_str());	 
   		strncpy(sCdrTime,vFiledTag[7].c_str(),14);

    	if (!G_PUSERINFO->getServ (G_SERV, sBillingNbr, sFoundTime))
	     {	
	       iStopFlag = 0;
       }	
      if (strcmp(G_SERV.getServInfo()->m_sState,"2HA") == 0)
    	 {	
    		 iStopFlag = 0;
    	 }else
    	 {
    		 iStopFlag = 1;
    	 }
    	 
    	lServID = G_SERV.getServID();
    	
    	if(!setBillingCycleID(sCdrTime)) 
    		   setBillingCycleID(sFoundTime) ;
    	
      oAcctItemAccuMgr.getAll(lServID, m_iBillingCycleID, vct);   
      lAllCharge= 0;     
        if (!vct.empty())
        {                        
            for (iter = vct.begin(); iter != vct.end(); ++iter)
            {    
                //lAllCharge += (*iter)-> m_iValue;\
                lAllCharge += (*iter)-> m_iValue;
                lAcctID = (*iter)->m_lAcctID;
            }
            
            vct.clear();                          
        }    	   
      lAcctBalance = oBalanceMgr.getAcctBalance(lAcctID);
      lUserBalace = lAcctBalance - lAllCharge ;
      lCredit = G_PUSERINFO->getAcctCreditAmount(lAcctID);  
  
    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%s|%d|%s|%s|%s|%ld|%ld|%ld|0|%ld|%d|%ld", 
    	        DR_ID, m_sProveCode,iBusiID,sBusiName ,sFoundTime,sBillingNbr, 
    	        lDuration,lAmount,lCharge,lCredit, iStopFlag,lUserBalace);    
    	cout<<sLineBuf<<endl;
    	vCheckHighFee.push_back(string(sLineBuf));   
      //  	upHighFeeRecord(lHighfeeID) ;  
     }
       
    //调用接口入库
	  //oCheckHighFee.writeFile(vCheckHighFee);
    oCheckHighFee.writeJsonFile(vCheckHighFee,SPLIT_FLAG);
    DB_LINK->commit();
    */
    return 0;     
}	


/*
 *	函 数 名 : checkAlertLog
 *	函数功能 : 提取计费系统所有进程的告警日志和关键事件信息,AuditId=0013，Type=50,高频
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkProcessLog()
{
	/*
   cout <<"CheckProcess::checkProcessLog(AuditId=0013，Type=50,高频) ..."<<endl;
   
	 StatInterface oCheckAlertLog("0013",50,HIGH_FREG);
	 vector<string> vCheckAlertLog;
   vector<string> vHihgFeeLog ;	 
	 vCheckAlertLog.clear();
	 vHihgFeeLog.clear();
	 char tmpbuf[1024]={0};
	 char strbuf[1024]={0};
   sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-LOG_GRADE|"
                        "S-ERROR_CODE|"
                        "S-ERROR_NAME|"
                        "S-LOG_CONTENT|"
                        "S-EXPERT_ADVICE|"
                        "S-LOG_BORN_TIME");                        
   vCheckAlertLog.push_back(string(tmpbuf));
	 
	 int iLogGrade = 0;
	 char sErrorCode[4+1] = {0};
	 char sErrorName[50] = {0};
	 char sLogContent[2000] = {0};
	 char sExpertAdvice[2000] = {0};
   char sLineBuf[MaxLineLength] = {0};
	 
   Date dt; 
   char sTime[14+1]={0};
   strncpy(sTime,dt.toString(),14);
   char sLine[1024];
   char *sTag1="启动" ;
   char *sTag2="退出(析构)" ; 
   char cDiv='[';  
   vector<string>::iterator iterS;     
     	      
	  //业务启停信息
	  Date dt_s(oCheckAlertLog.sStatBeginDate);
	  Date dt_e(oCheckAlertLog.sEndDate);
	  int diff=(int)dt_e.diffSec(dt_s);   
	  m_pThreeLog->GetAttemperLog(oCheckAlertLog.sStatBeginDate,diff,vHihgFeeLog);
	  cout<<"共接受程序启停原始信息数据"<<vHihgFeeLog.size()<<endl;
    for(iterS=vHihgFeeLog.begin();iterS!=vHihgFeeLog.end();++iterS)
     {
       	memset(strbuf, 0, sizeof(strbuf));	
     	  strncpy(strbuf,(*iterS).c_str(), 1023);    	   
     	   if(!strstr(strbuf,sTag1) )  //程序启动
     	   	  {
     	   	    cout<<(*iterS)<<endl;
     	   	  	iLogGrade= 3 ;
     	   	  	char *pbegin=strbuf;
     	   	  	char *p=0;
     	        pbegin++;
     	        p=strchr(pbegin,']');
     	        if(p) *p=0;	  
     	        Date dt(pbegin,"yyyy-mm-dd hh:mi:ss");	
     	   	  	memset(sLineBuf, 0, sizeof(sLineBuf));	
              sprintf(sLineBuf, "%d|%d|%d|||%s||%s", 
   	                DR_ID, m_iProveNum,iLogGrade,(*iterS).c_str(), dt.toString());
              vCheckAlertLog.push_back(string(sLineBuf));  
             insertProcessSE(1,(*iterS).c_str(), dt.toString());               	                
   	        }  
     	   if( !strstr(strbuf,sTag2))  //程序停止
     	   	  {
     	   	    cout<<(*iterS)<<endl;
     	   	  	iLogGrade= 3 ;
     	   	  	char *pbegin=strbuf;
     	   	  	char *p=0;
     	        pbegin++;
     	        p=strchr(pbegin,']');
     	        if(p)
     	           *p=0;	  	
     	        Date dt(pbegin,"yyyy-mm-dd hh:mi:ss");
     	   	  	memset(sLineBuf, 0, sizeof(sLineBuf));	
              sprintf(sLineBuf, "%d|%d|%d|||%s||%s", 
   	                DR_ID, m_iProveNum,iLogGrade,(*iterS).c_str(), dt.toString());
              vCheckAlertLog.push_back(string(sLineBuf));   	
              insertProcessSE(0,(*iterS).c_str(), dt.toString());                                 
   	        }    	             
   	 }
     
     //0号进程启动停止
      DEFINE_QUERY(qry);
      char sSql[1024]={0};
      sprintf(sSql,"select state,STATE_DATE from B_INIT_LOG "
              "where STATE_DATE between to_date('%s','yyyymmddhh24miss')+1/(24*3600) and to_date('%s','yyyymmddhh24miss') ",
               oCheckAlertLog.sStatBeginDate,oCheckAlertLog.sEndDate );
      qry.setSQL(sSql);
      qry.open();
      while(qry.next())
      {
      	int iType= qry.field(0).asInteger();
      	char tmpTime[14+1]={0};
      	strncpy(tmpTime,qry.field(1).asString(),14);
      	iLogGrade= 3 ;
      	char sTmp[256]={0};
     	  memset(sLineBuf, 0, sizeof(sLineBuf));	
        if(iType == 1)
        	 sprintf(sTmp,"initserver进程启动!");

         if(iType == 0)   
         	 sprintf(sTmp,"initserver进程退出!");
         	 
        sprintf(sLineBuf, "%d|%d|%d|||%s||%s", DR_ID, m_iProveNum,iLogGrade,sTmp,sTime); 
        vCheckAlertLog.push_back(string(sLineBuf));  
        insertProcessSE(iType,sTmp,tmpTime);
     }           	
      qry.close();
      
      
    //告警信息
    vHihgFeeLog.clear();
    m_pThreeLog->GetAlarmLog(oCheckAlertLog.sStatBeginDate,diff,vHihgFeeLog); 
   // vHihgFeeLog.push_back(string("[2010-10-04 03:33:33]: [ALARM][ALARM_TYPE:0]--ProcId:[-1] ModuleId:[0] File:[../app_guard/mntapi.cpp]LineNo:[263] function:[AttachShm] ErrId:[4] ErrNo:[2][Msg]连接共享内存失败。"));    
     cout<<"共接受程序告警信息数据"<<vHihgFeeLog.size()<<endl; 
     for(iterS=vHihgFeeLog.begin();iterS!=vHihgFeeLog.end();++iterS)
     {
     	  iLogGrade= 1 ;
     	  memset(strbuf, 0, sizeof(strbuf));	
     	  strncpy(strbuf,(*iterS).c_str(), 1023);     	       	  
     	  memset(sErrorCode,0,sizeof(sErrorCode));
     	  memset(sLogContent,0,sizeof(sLogContent));
     	  memset(sErrorName,0,sizeof(sErrorName));
     	  memset(sExpertAdvice,0,sizeof(sExpertAdvice));    
     	   	  

     	  char *pbegin=strstr(strbuf,"ErrId");
    	  char *pbegin2=strstr(strbuf,"Msg");     	  
     	  char *p=strchr(pbegin,cDiv);   	       	  
     	  pbegin=p+1;
     	  p=strchr(pbegin,']');
     	  if(p) *p=0;
     	  //strcpy(sErrorCode,pbegin);
     	  sprintf(sErrorCode,"%04s",pbegin);
     	  HbFindErrByFile(sErrorCode,sErrorName,sExpertAdvice);
     	  if(strcmp(sErrorCode,"0100")>0)
     	  	iLogGrade= 2 ;
     	  
     	  p=pbegin2+4;
     	  strcpy(sLogContent,p);
        sprintf(sLineBuf, "%d|%d|%d|%s|%s|%s|%s|%s", 
   	            DR_ID, m_iProveNum,iLogGrade,sErrorCode,sErrorName,sLogContent, sExpertAdvice,sTime);
   	    vCheckAlertLog.push_back(string(sLineBuf));            
   	 }    
    	
    	
    //账务告警
    if(m_iAcctAlarmLev>0)
    {  
    	DEFINE_QUERY(qry);
    	char sSql[1024]={0};
    	sprintf(sSql,"Select err_code ,to_char(begin_time,'yyyymmddhh24miss'),ERR_TRACK From a_service_xml_record " 
                   "Where err_code <>'0' AND err_code <>'-1' AND err_track IS NOT NULL "
                   "AND   begin_time BETWEEN TO_date('%s','yyyymmddhh24miss') AND TO_date('%s','yyyymmddhh24miss')" ,
                   oCheckAlertLog.sStatBeginDate,oCheckAlertLog.sEndDate );
      //cout<<sSql<<endl;
      qry.setSQL (sSql);
      qry.open();
      while (qry.next()	)
	    {	
	    	memset(sErrorCode,0,sizeof(sErrorCode));
	    	memset(sErrorName,0,sizeof(sErrorName));
	    	memset(sExpertAdvice,0,sizeof(sExpertAdvice));
	    	memset(sLogContent,0,sizeof(sLogContent));
	    	memset(sTime,0,sizeof(sTime));
	    	sprintf(sErrorCode,"%04s",qry.field(0).asString());
	    	strncpy(sTime,qry.field(1).asString(),14);
	    	iLogGrade = 1;
     	  HbFindErrByFile(sErrorCode,sErrorName,sExpertAdvice);
     	  if(strcmp(sErrorCode,"0100")>0)
     	  	iLogGrade= 2 ;
     	  strcpy(sLogContent,qry.field(2).asString());
     	  memset(sLineBuf,0,sizeof(sLineBuf));
        sprintf(sLineBuf, "%d|%d|%d|%s|%s|%s|%s|%s", 
   	            DR_ID, m_iProveNum,iLogGrade,sErrorCode,sErrorName,sLogContent, sExpertAdvice,sTime);
   	    vCheckAlertLog.push_back(string(sLineBuf));   
   	    printf("%s\n",sLineBuf);  	    	
	    }
	    qry.close();
	  }
	  
     //调用接口入库
    // oCheckAlertLog.writeFile(vCheckAlertLog);
     oCheckAlertLog.writeJsonFile(vCheckAlertLog,SPLIT_FLAG);
     DB_LINK->commit();
     */
     return 0;
}

int CheckStatInfo::HbFindErrByFile(char * sErrCode,char * sCause,char * sAction)
{
	FILE *fp;
	char * p, sFileName[256], sTemp[1024];
	int i=0;
	
	if ((p=getenv ("BILLDIR")) == NULL) 
		strcpy (sFileName, "/opt/opthb/src/lib/MBC_28.h");
	else
		sprintf (sFileName, "%s/src/lib/MBC_28.h", p);

	if ((fp = fopen (sFileName, "r")) != NULL) {

		while (fgets(sTemp, 1024, fp) != NULL) {
			if (strncmp (sTemp, "//", 2) != 0)
				continue;
      
      if((sTemp[2]<'0')||(sTemp[2]>'9'))
      	continue;
	char sTempa[1024];
	strncpy(sTempa,sTemp+2,4);
	sTempa[4]=0;
      int iErrorFileGet = atoi(sTempa);
	int iErrorArgv =atoi(sErrCode);
      if((strncmp(sTemp+2,sErrCode,4)!=0)&&(iErrorFileGet!=iErrorArgv))
      	continue;
      char* sTok = " \t";
			p = strtok (sTemp, sTok);
			strncpy(sErrCode,p+2,10);
			p = strtok (NULL, sTok);
			strncpy (sCause, p+2, 255);
			p = strtok (NULL, sTok);
			strncpy (sAction, p+2, 255);
			i=1;
			break;
		 }
		fclose (fp);
	}
	if(i){return 1;}
		return 0;
}

void CheckStatInfo::insertProcessSE(int itype,char *name ,char *time)
{
    DEFINE_QUERY (qry);
    char sSql[1024]={0};
    sprintf(sSql,"insert into b_info_process_se (type_id, type_name, create_time ) "
            " values (%d,'%s',to_date('%s','yyyymmddhh24miss') ) ",
              itype,name,time);
    qry.setSQL(sSql);            
    qry.execute();
    qry.commit();
  
    qry.close();	
}
 


/*
 *	函 数 名 : checkSystemCaution
 *	函数功能 : 提取系统告警指标（AuditId=0900，Type=20)
 *	时    间 : 2010年11月
 *	返 回 值 : 
 *	参数说明 : 
*/
void CheckStatInfo::checkSystemCaution()
{
	
	char sDealNums[128] = {0};	

    cout <<"CheckKpi::checkAddItemCaution(AuditId=0302，Type=20,中频) ..."<<endl;
    
    char *sKPIID_PARAM="20-30-02-09-00-001";
    char *sKPINAME_PARAM="参数加载失败告警";
    char *sAlarmID_PARAM="090020001";    
    
    char *sKPIID_SYSTEM="20-30-02-09-00-002";
    char *sKPINAME_SYSTEM="系统启动失败告警";
    char *sAlarmID_SYSTEM="090020002"; 
               
    char *KPI_EVENT_JIYA ="20-30-02-09-00-003";
    char *KPI_EVENT_JIYA_NAME ="当日文件处理积压告警";    
    char *KPI_EVENT_YICH ="20-30-02-09-00-004";	
    char *KPI_EVENT_YICH_NAME ="当日对外接口异常告警"; 
    
    char *sKPB="/HB";
    
    char sStatBeginDate[14+1]={0};
    char sEndDate[14+1]={0};


    StatInterface oSystemCaution("0900",20,HIGH_FREG);    
    vector<string> vCautionVec;
    vCautionVec.clear();
    vCautionVec.push_back(ALARM_TITLE);
       strncpy(sStatBeginDate,oSystemCaution.sStatBeginDate,14);
   strncpy(sEndDate,oSystemCaution.sEndDate,14);  
    
    // 提取参数更新失败告警
    char sLineBuf[1024]={0};
    DEFINE_QUERY(qry);
    char sSql[1024]={0};
    sprintf(sSql," SELECT count(*),min(load_time),max(load_time) "
                "FROM B_INFO_PARAM_LOAD_RET a "
                "WHERE a.load_flag=2 "
                "AND   a.load_time BETWEEN to_date('%s','yyyymmddhh24miss')+ 1/(24*3600) AND to_date('%s','yyyymmddhh24miss') ",
                oSystemCaution.sStatBeginDate,oSystemCaution.sEndDate);
   
	  cout<<sSql<<endl;          
    qry.setSQL(sSql);
    qry.open ();
    
    if (qry.next()) 
    {    
    	int icount= qry.field(0).asInteger();
   	  char tmpStime[14+1]={0};
   	  char tmpEtime[14+1]={0};
   	  strncpy(tmpStime,qry.field(1).asString(),14);
   	  strncpy(tmpEtime,qry.field(2).asString(),14);   
   	  if(icount>0) 
   	 {
       memset(sLineBuf,0,sizeof(sLineBuf));
       sprintf(sLineBuf, "%s|%s|%s|%s||%s|%s|%d|%d|||||%d", 
                     sKPIID_PARAM,sKPINAME_PARAM,sAlarmID_PARAM, sKPB, tmpStime,tmpEtime,
                     1,10,icount);
       cout<<sLineBuf<<endl;
       vCautionVec.push_back(sLineBuf);     
     }	 	
   	 	
    }
    qry.close();
    
    // 提取系统启动失败告警
    memset(sSql,0,sizeof(sSql));
    sprintf(sSql,"SELECT count(*),MIN(a.CREATE_TIME),MAX(a.CREATE_TIME) "
                " FROM b_info_process_se a "
                " WHERE a.CREATE_TIME BETWEEN to_date('%s','yyyymmddhh24miss')+1/(24*3600) AND to_date('%s','yyyymmddhh24miss') "
                " and  a.type_id=3 ",oSystemCaution.sStatBeginDate,oSystemCaution.sEndDate);
    qry.setSQL(sSql);
    qry.open ();  
    if (qry.next()) 
    {    
    	int icount= qry.field(0).asInteger();
   	  char tmpStime[14+1]={0};
   	  char tmpEtime[14+1]={0};
   	  strncpy(tmpStime,qry.field(1).asString(),14);
   	  strncpy(tmpEtime,qry.field(2).asString(),14);   
   	  if(icount>0) 
   	 {
       memset(sLineBuf,0,sizeof(sLineBuf));
       sprintf(sLineBuf, "%s|%s|%s|%s||%s|%s|%d|%d|||||%d", 
                     sKPIID_SYSTEM,sKPINAME_SYSTEM,sAlarmID_SYSTEM, sKPB, tmpStime,tmpEtime,
                     1,10,icount);
       cout<<sLineBuf<<endl;
       vCautionVec.push_back(sLineBuf);     
     }	 	
   	 	
    } 
    
     //当日挤压和无异常事件
     //目前不知道从哪里提取数据源 写死
   
   	 /*memset(sLineBuf,0,sizeof(sLineBuf));
     sprintf(sLineBuf,"%s|%s|090060003|/HB||%s|%s|有积压事件|%d",
    		        KPI_EVENT_JIYA,KPI_EVENT_JIYA_NAME,sStatBeginDate,sEndDate,1 );
     vCautionVec.push_back(sLineBuf);  
     
   	 memset(sLineBuf,0,sizeof(sLineBuf));
     sprintf(sLineBuf,"%s|%s|090060004|/HB||%s|%s|对外接口有异常事件|%d",
    		        KPI_EVENT_YICH,KPI_EVENT_YICH_NAME,sStatBeginDate,sEndDate,1 );
     vCautionVec.push_back(sLineBuf);  
     */
 
     oSystemCaution.writeJsonFile(vCautionVec,SPLIT_FLAG); 


    return ;        	
}

/*
 *	函 数 名 : checkHSSSystemCaution
 *	函数功能 : 提取HSS系统告警指标（AuditId=0400，Type=20)
 *	时    间 : 2011年06月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkHSSSystemCaution()
{
	
	char sDealNums[128] = {0};	
    cout <<"CheckStatInfo::checkHSSSystemCaution(AuditId=0400，Type=20,高频) ..."<<endl;
    
    char *sKPIID_PARAM="20-30-12-04-00-001";
    char *sKPINAME_PARAM="参数加载失败告警";
    char *sAlarmID_PARAM="040020001";    
    char *sKPIID_SYSTEM="20-30-12-04-00-002";
    char *sKPINAME_SYSTEM="系统启动失败告警";
    char *sAlarmID_SYSTEM="090020002"; 
    char *sKPB="/HSS";   
    char sStatBeginDate[15]={0};
    char sEndDate[15]={0};
    char sLineBuf[MaxLineLength] = {0};
    StatInterface oSystemCaution("0400",20,HIGH_FREG);    
    vector<string> vCheckEvenLog;
    vCheckEvenLog.clear();
    vCheckEvenLog.push_back(ALARM_TITLE);
    strncpy(sStatBeginDate,oSystemCaution.sStatBeginDate,14);
    strncpy(sEndDate,oSystemCaution.sEndDate,14);  
    
    ThreeLogGroup Log;  
    vector<LogInfoData> vRet;  
    vRet.clear();
  	
  	//系统启动失败
   	Log.GetLoggInfo(vRet,GROUPCODE,MBC_SYSTEM_START_ERROR,sStatBeginDate,sEndDate); 
   	
   	int iVecSize = vRet.size(); 		
  	sort(vRet.begin(),vRet.end(),sortVector);   
     
    if (iVecSize)
    {   	   		   	
    	 char sAlarmID[10] = {0};
    	 sprintf(sAlarmID,"%d",MBC_SYSTEM_START_ERROR);
   	   memset(sLineBuf,0,sizeof(sLineBuf));
       sprintf(sLineBuf, "%s|%s|%s|/HSS||%s|%s|%d|%d|||||%d", 
                     sKPIID_PARAM,sKPINAME_PARAM,sAlarmID, vRet[0].m_sLogTime,vRet[iVecSize-1].m_sLogTime,
                     1,10,iVecSize);
       vCheckEvenLog.push_back(string(sLineBuf));
    } 
     
    //参数加载失败
    vRet.clear();
   Log.GetLoggInfo(vRet,GROUPCODE,MBC_PARAM_LOAD_ERROR,sStatBeginDate,sEndDate); 
   iVecSize = vRet.size();
   	sort(vRet.begin(),vRet.end(),sortVector);   
    if (iVecSize)
    { 
    	 char sAlarmID[10] = {0};
    	 sprintf(sAlarmID,"%d",MBC_PARAM_LOAD_ERROR);
   	   memset(sLineBuf, 0, sizeof(sLineBuf));		   	
       sprintf(sLineBuf, "%s|%s|%s|/HSS||%s|%s|%d|%d|||||%d", 
                     sKPIID_SYSTEM,sKPINAME_SYSTEM,sAlarmID, vRet[0].m_sLogTime,vRet[iVecSize-1].m_sLogTime,
                     1,10,iVecSize);
        	vCheckEvenLog.push_back(string(sLineBuf));
    } 
    oSystemCaution.writeJsonFile(vCheckEvenLog,SPLIT_FLAG); 
     
    return 1;        	
}
/*
 *	函 数 名 : checkAcctCaution
 *	函数功能 : 提取销账告警指标（AuditId=0305，Type=40)
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
void CheckStatInfo::checkAcctCaution(char *sAudit,int iType,char *sFreg)
{
    cout <<"CheckKpi::checkMonthCap(AuditId="<<sAudit<<"Type="<<iType<<"中频) ..."<<endl;
    
    StatInterface oACCTCap(sAudit,iType,sFreg);    
    vector<string> vACCTCap;
    vACCTCap.clear();
    vACCTCap.push_back(ALARM_TITLE);
    
    string temp;
	  string kpiId;
	  string kpiName;
	  string alarmId;
	  string kbpId;
	  string kbpName;
	  string firstTime;
	  string lastTime;
	  string eventLevel;
	  string eventState;
	  string acktime;
	  string ackuser ;
	  string cleartime ;
	  string clearuser ;
	  string count;
	  
	  DEFINE_QUERY(qry);
	  char sSQL[1024];
	  memset(sSQL, 0, sizeof(sSQL));
	  sprintf(sSQL,"select a.kpiid,a.kpiname,a.alarmid,a.kbp,a.kbp_name, "
    " a.FIRSTTIME,a.LASTTIME,a.EVENTLEVEL,a.EVENTSTATE, "
    " a.ACKTIME,a.ACKUSER,a.CLEARTIME,a.CLEARUSER,a.COUNT_NUM ,a.KPI_LOG"
	  " From B_INFO_CAUTION_INTERFACE a "
	  " where a.KPI_AUDITID='%s' AND a.KPI_TYPE= %d AND  a.DEAL_FLAG=0 ",
	   sAudit,iType);
	  
	  qry.setSQL(sSQL);
	  qry.open();
    while (qry.next())
    {
	  	kpiId=qry.field(0).asString();
	  	kpiName=qry.field(1).asString();
	  	alarmId=qry.field(2).asString();
	  	kbpId=qry.field(3).asString();
	  	kbpName=qry.field(4).asString();
	  	firstTime=qry.field(5).asString();
	  	lastTime=qry.field(6).asString();
	  	eventLevel=qry.field(7).asString();
	  	eventState=qry.field(8).asString();
	  	acktime= qry.field(9).asString();
	  	ackuser = qry.field(10).asString();
	  	cleartime = qry.field(11).asString();
	  	clearuser = qry.field(12).asString();
	  	count=qry.field(13).asString();
	  	
	  	long lKipLog = qry.field(14).asLong();
    	
    	temp=kpiId+string("|")+kpiName+string("|")+alarmId+string("|")+kbpId+string("|")+kbpName+string("|")
    	+firstTime+string("|")+lastTime+string("|")+eventLevel+string("|")+eventState+string("|")+
    	acktime+string("|")+ackuser+string("|")+cleartime+string("|")+clearuser+string("|")+count;
    	vACCTCap.push_back(temp);
    	upCautionKipLog(lKipLog);
    }
    qry.close();
    oACCTCap.writeJsonFile(vACCTCap,SPLIT_FLAG); 
    DB_LINK->commit();         
    return ;        	
}

/*
 *	函 数 名 : checkDataRecvBalance
 *	函数功能 : 数据接收平衡信息点（AuditId=0305，Type=40)
 *	时    间 : 2010年9月
 *	返 回 值 : 空
 *	参数说明 : 
*/
void CheckStatInfo::checkDataRecvBalance(char *sAudit, int iType, char *sFreg)
{	
	char StringTemp [100];
	int lKipLog ;
	char sSQL[1024] = {0};
	cout <<"CheckStatInfo::checkDataRecvBalance(AuditId="<<sAudit<<"Type="<<50<<"中频) ..."<<endl;
  	string temp;
    StatInterface oDATACap(sAudit,iType,sFreg);    
    vector<string> vDATACap;
    vDATACap.clear();
    
 	char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "I-DATA_SOURCE|"
                        "S-IN_NET_ID|"
                        "I-RECV_MSG_NUM|"
                        "I-SUCC_MSG_NUM|"
                        "I-REJECT_MSG_NUM|"
                        "I-FAIL_MSG_NUM|"
                        "S-END_TIME");             
    vDATACap.push_back(string(tmpbuf));
    
    char sStatBeginDate[15]={0};
    char sEndDate[15]={0};
    long lEndTime = 0;
    int iDataSrc,iInNeedID,iRecvNum, iSuccNum, iRjctNum,iFailNum;
    char sDataSrc[20]={0},sInNeedID[20]={0}; 
    char sRecvNum[20]={0},sFailNum[20]={0},sSuccNum[20]={0},sRjctNum[20]={0},sEndTime[20],sDR_ID[20];     		
	
	strncpy(sStatBeginDate,oDATACap.sStatBeginDate,14);
  	strncpy(sEndDate,oDATACap.sEndDate,14);   
	memset(sSQL, 0, sizeof(sSQL));	
	sprintf (sSQL, "select DATA_SRC,IN_NE,"
					" recv_num,"
					" succ_num,"
					" rjct_num,"
					" fail_num,"
					" sys_time "
				   	" FROM hss_balance_mess WHERE balance_type = 0 and sys_time*100 >= %ld "
				   	" and sys_time*100<%ld",atol(sStatBeginDate),atol(sEndDate)); 									  
	
	try{
		m_poInfo->Prepare(sSQL);
    	m_poInfo->Commit();	
   		m_poInfo->Execute();
    
    	while (!m_poInfo->FetchNext()) {    	 	
    		m_poInfo->getColumn(1, &iDataSrc);
    		iDataSrc = m_iProveNum;
       		m_poInfo->getColumn(2, &iInNeedID);      		    	
      		m_poInfo->getColumn(3, &iRecvNum);
      	    m_poInfo->getColumn(4, &iSuccNum);     
      	    m_poInfo->getColumn(5, &iRjctNum);
            m_poInfo->getColumn(6, &iFailNum);
            m_poInfo->getColumn(7, &lEndTime);
            lEndTime = lEndTime*100;
        
            sprintf(sDataSrc,"%d",iDataSrc);      	
        	sprintf(sInNeedID,"%d",iInNeedID);
        	sprintf(sRecvNum,"%d",iRecvNum);      	
        	sprintf(sSuccNum,"%d",iSuccNum);      	
        	sprintf(sRjctNum,"%d",iRjctNum);      	
        	sprintf(sFailNum,"%d",iFailNum); 
      	  	sprintf(sEndTime,"%ld",lEndTime);      	       
            sprintf(sDR_ID,"%ld",DR_ID);  
                   	  	
        	 temp = string(sDR_ID)+ string("|")+\
        	  		string(sDataSrc)+string("|")+\
        	 		string(sInNeedID)+string("|")+\
        			 string(sRecvNum)+string("|")+\
             		string(sSuccNum) +string("|")+\
            		 string(sRjctNum) +string("|")+\
        			 string(sFailNum)+string("|")+\
        			 string(sEndTime);
             	        	    
        	vDATACap.push_back(temp);
       	}
       		
   		 m_poInfo->Close();
	}
	catch(TTStatus st) {
        m_poInfo->Close();
        printf("checkDataRecvBalance failed! err_msg=%s", st.err_msg);
    }
    oDATACap.writeJsonFile(vDATACap,SPLIT_FLAG); 
      
    return ; 
	
}

void  CheckStatInfo::checkDataLoadBalance(char *sAudit, int iType, char *sFreg)
{

	int lKipLog ;	
	char sSQL[1024];
	string temp ;
	char  StringTemp[100];
	cout <<"CheckStatInfo::checkDataLoadBalance(AuditId="<<sAudit<<"iType="<<50<<"中频) ..."<<endl;
    
    StatInterface oACCTCap(sAudit,iType,sFreg);    
    vector<string> vDATACap;
    vDATACap.clear();
 
 	char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "I-DATA_SOURCE|"
                        "S-IN_NET_ID|"
                        "I-INPUT_MSG_NUM|"
                        "I-SUCC_MSG_NUM|"
                        "I-REJECT_MSG_NUM|"
                        "I-FAIL_MSG_NUM|"
                        "S-END_TIME");             
    vDATACap.push_back(string(tmpbuf));
    
	char sStatBeginDate[15]={0};
    char sEndDate[15]={0};  
    long lEndTime = 0;
	int  iDataSrc, iInNetID,iRecvNum,iSuccNum,iRjctNum, iFailNum;
	char sTDataSrc[20],sTInNetID[20],sTRecvNum[20],sTSuccNum[20],sTRjctNum[20],sTFailNum[20],sEndTime[20],sDR_ID[20];
		
   	strncpy(sStatBeginDate,oACCTCap.sStatBeginDate,14);
    strncpy(sEndDate,oACCTCap.sEndDate,14);  

	memset(sSQL, 0, sizeof(sSQL));
	sprintf (sSQL, "select DATA_SRC,IN_NE,"
					" recv_num,"
					" succ_num,"
					" rjct_num,"
					" fail_num,"
					" sys_time "
				   	" FROM hss_balance_mess WHERE balance_type = 1 and sys_time*100 >= %ld "
				    " and sys_time*100<%ld",atol(sStatBeginDate),atol(sEndDate) ); 
	try{
	 	m_poInfo->Prepare(sSQL);
     	m_poInfo->Commit(); 
     	m_poInfo->Execute();
     	    	    	
   	 	while (!m_poInfo->FetchNext()){  	 			
    		m_poInfo->getColumn(1, &iDataSrc);
    		iDataSrc = m_iProveNum;
     		m_poInfo->getColumn(2, &iInNetID);        	
        	m_poInfo->getColumn(3, &iRecvNum);
        	m_poInfo->getColumn(4, &iSuccNum);
        	m_poInfo->getColumn(5, &iRjctNum);    	
        	m_poInfo->getColumn(6, &iFailNum); 
        	m_poInfo->getColumn(7, &lEndTime);
        	lEndTime = lEndTime*100;
        	                	
        	sprintf(sTDataSrc,"%d",iDataSrc); 
        	sprintf(sTInNetID,"%d",iInNetID); 
        	sprintf(sTRecvNum,"%d",iRecvNum); 
        	sprintf(sTSuccNum,"%d",iSuccNum);
        	sprintf(sTRjctNum,"%d",iRjctNum);   
          	sprintf(sTFailNum,"%d",iFailNum); 	
       	  	sprintf(sEndTime,"%ld",lEndTime); 
       	  	sprintf(sDR_ID,"%d",DR_ID);  	
	  		
	  		temp = string(sDR_ID)+ string("|") \
	  			 + string(sTDataSrc)+ string("|")\
	  	      	 + string(sTInNetID)+ string("|") \
	  		   	 + string(sTRecvNum)+ string("|")\
	  		  	 + string(sTSuccNum)+ string("|")\
	  		   	 + string(sTRjctNum)+ string("|")\
	  		  	 + string(sTFailNum)+ string("|")\
	  		  	 + string(sEndTime) ;
       	 	
       	 	vDATACap.push_back(temp);
    	} 
   	 	m_poInfo->Close();
	}
 	catch(TTStatus st) {
        m_poInfo->Close();
        printf("checkDataLoadBalance failed! err_msg=%s", st.err_msg);
    }
    oACCTCap.writeJsonFile(vDATACap,SPLIT_FLAG); 
 
    return ; 
}

void CheckStatInfo::checkDataInputBalance(char *sAudit, int iType, char *sFreg)
{	
	char sSQL[1024];
	char StringTemp[100];
	string temp;
	int   lKipLog;
	cout <<"CheckStatInfo::checkDataInputBalance(AuditId="<<sAudit<<"iType="<<50<<"中频) ..."<<endl;
    
    StatInterface oACCTCap(sAudit,iType,sFreg);    
    vector<string> vDATACap;
    vDATACap.clear();      
 	char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "I-DATA_SOURCE|"
                        "S-IN_NET_ID|"
                        "I-INPUT_MSG_NUM|"
                        "I-SUCC_MSG_NUM|"
                        "I-REJECT_MSG_NUM|"
                        "I-FAIL_MSG_NUM|"
                        "S-END_TIME"); 
                                    
    vDATACap.push_back(string(tmpbuf));
 
    char sStatBeginDate[15]={0};
    char sEndDate[15]={0};
     
    int iDataSrc,iInNetId;
    long lEndTime = 0;
    char sDataSrc[20],sInNeedID[20];

	int iRecvNum,isuccNum,irjctNum,ifailNum;
	char sRecvNum[20],ssuccRecvNum[20],srjctRecvNum[20],sfailRecvNum[20],sEndTime[20],sDR_ID[20];
   
 	strncpy(sStatBeginDate,oACCTCap.sStatBeginDate,14);
  	strncpy(sEndDate,oACCTCap.sEndDate,14);   
	memset(sSQL, 0, sizeof(sSQL));
	
	sprintf (sSQL, "select DATA_SRC,IN_NE,"
				   " recv_num,"
				   " succ_num,"
				   " rjct_num,"
				   " fail_num,"
				   " sys_time "
				   " FROM  hss_balance_mess "
				   " WHERE  balance_type = 2 and sys_time*100 >= %ld AND sys_time*100<%ld " 
                  ,atol(sStatBeginDate),atol(sEndDate) );                       
  	try{
  		m_poInfo->Prepare(sSQL);
    	m_poInfo->Commit();
				  
    	m_poInfo->Execute();   	    	
    	while (!m_poInfo->FetchNext()) {
        	
       		 m_poInfo->getColumn(1, &iDataSrc);
       		 iDataSrc = m_iProveNum;
       		 m_poInfo->getColumn(2, &iInNetId);  	
       		 m_poInfo->getColumn(3, &iRecvNum);
       		 m_poInfo->getColumn(4, &isuccNum);
       		 m_poInfo->getColumn(5, &irjctNum);       
       		 m_poInfo->getColumn(6, &ifailNum);   
       		 m_poInfo->getColumn(7, &lEndTime);    
       		 lEndTime = lEndTime*100;  	    
       			
       		 sprintf(sDataSrc,"%d",iDataSrc);
       		 sprintf(sInNeedID,"%d",iInNetId);
       		 sprintf(sRecvNum,"%d",iRecvNum);
       		 sprintf(ssuccRecvNum,"%d",isuccNum);
       		 sprintf(srjctRecvNum,"%d",irjctNum);
       		 sprintf(sfailRecvNum,"%d",ifailNum);          
       		 sprintf(sEndTime,"%ld",lEndTime);  
       		 sprintf(sDR_ID,"%ld",DR_ID);
       		 
       		 temp = string(sDR_ID)+ string("|") + \
       		 		string(sDataSrc)+ string("|") +\
       		 	    string(sInNeedID)+ string("|") +\
       			    string(sRecvNum) + string("|") +\
       		 	    string(ssuccRecvNum) + string("|") +\
       		 	    string(srjctRecvNum) + string("|") +\
       		 	    string(sfailRecvNum)+string("|") +\
       		 	    string(sEndTime);      		 	   
       		 	           	   
       		 vDATACap.push_back(temp); 
    	}         
		m_poInfo->Close();
	} 
    catch(TTStatus st) {
        m_poInfo->Close();
        printf("checkDataInputBalance failed! err_msg=%s", st.err_msg);
    }
    oACCTCap.writeJsonFile(vDATACap,SPLIT_FLAG); 
       
    return ; 
}

/*获取VC接口性能*/
void CheckStatInfo::checkVCCable(char *sAudit,int iType,char *sFreg)
{
	
	cout <<"CheckStatInfo::checkVCCable(AuditId="<<sAudit<<"Type="<<iType<<"中频) ..."<<endl;
    
    StatInterface oACCTCap(sAudit,iType,sFreg);    
    vector<string> vACCTCap;
    vACCTCap.clear();
    vACCTCap.push_back(CAPAB_TITLE);
    char  StringTemp[100]={0};
    char sSQL[1024]={0};
    char sLineBuf[MaxLineLength] = {0};
    int iLogID=0;
   	float fMinRecvMsgNum, fMaxRecvMsgNum, fAvgRecvMsgNum;
    float fMinRejMsgNum, fMaxRejMsgNum, fAvgRejMsgNum;
    float fMinFailMsgNum, fMaxFailMsgNum, fAvgFailMsgNum;
    float fMinSuccMsgNum, fMaxSuccMsgNum, fAvgSuccMsgNum;

    sprintf(sSQL,"SELECT "
    			 " min (recv_num)*1.0, avg (recv_num)*1.0, max (recv_num)*1.0, "
				 " min (rjct_num)*1.0, avg (rjct_num)*1.0, max (rjct_num)*1.0, "
				 " min (fail_num)*1.0, avg (fail_num)*1.0, max (fail_num)*1.0, " 
				 " min (succ_num)*1.0, avg (succ_num)*1.0, max (succ_num)*1.0 "
                 " FROM hss_balance_mess  "              
                 " where in_ne = 3007 "   //省级统一充值平台
                 " and balance_type = 3 AND sys_time*100 >= %ld and sys_time*100 < %ld" ,
               	  atol(oACCTCap.sStatBeginDate),atol(oACCTCap.sEndDate));
               	 
    try{              
 		m_poInfo->Prepare(sSQL);
    	m_poInfo->Commit();	
    	m_poInfo->Execute();
    	while (!m_poInfo->FetchNext()) {   
    	           	    	
     		m_poInfo->getColumn(1, &fMinRecvMsgNum);
       		m_poInfo->getColumn(2, &fAvgRecvMsgNum);
        	m_poInfo->getColumn(3, &fMaxRecvMsgNum);       	
        	m_poInfo->getColumn(4, &fMinRejMsgNum);
       		m_poInfo->getColumn(5, &fAvgRejMsgNum);
        	m_poInfo->getColumn(6, &fMaxRejMsgNum);
        	m_poInfo->getColumn(7, &fMinFailMsgNum);
       		m_poInfo->getColumn(8, &fAvgFailMsgNum);
        	m_poInfo->getColumn(9, &fMaxFailMsgNum);
        	m_poInfo->getColumn(10, &fMinSuccMsgNum);
       		m_poInfo->getColumn(11, &fAvgSuccMsgNum);
        	m_poInfo->getColumn(12, &fMaxSuccMsgNum);
        		 
        	 if(fMaxRecvMsgNum ==0) break;
      		 memset(sLineBuf,0,MaxLineLength);
      		 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-03-001",
                         "发布总消息数",
                         "/HSS",
                      	 fMinRecvMsgNum,
                   		 fAvgRecvMsgNum,
                   		 fMaxRecvMsgNum,
                   		 0                  
                         );
      		 vACCTCap.push_back(sLineBuf);
       
       		memset(sLineBuf,0,MaxLineLength);
      		 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-03-002",
                         "发布拒收总消息数",
                         "/HSS",
                         fMinRejMsgNum,
                   		 fAvgRejMsgNum,
                   		 fMaxRejMsgNum                  
                         );
       		vACCTCap.push_back(sLineBuf);
       
      		 memset(sLineBuf,0,MaxLineLength);
      		 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-03-003",
                         "发布失败总消息数",
                         "/HSS",
                         fMinFailMsgNum,
                   		 fAvgFailMsgNum,
                   		 fMaxFailMsgNum                  
                         );
       		vACCTCap.push_back(sLineBuf);
       
      		 memset(sLineBuf,0,MaxLineLength);
       		sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-03-004",
                         "发布拒收消息包比率",
                         "/HSS",
                      	 fMinRejMsgNum / fMinRecvMsgNum,
                   		 fAvgRejMsgNum / fAvgRecvMsgNum,
                   		 fMaxRejMsgNum / fMaxRecvMsgNum              
                         );
           vACCTCap.push_back(sLineBuf);
                         
             memset(sLineBuf,0,MaxLineLength);
     		  sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-03-005",
                         "发布处理失败消息包比率",
                         "/HSS",
                         fMinFailMsgNum / fMinRecvMsgNum,
                   		 fAvgFailMsgNum / fAvgRecvMsgNum,
                   		 fMaxFailMsgNum / fMaxRecvMsgNum             
                         );
                                      
      		 vACCTCap.push_back(sLineBuf);          
   		 }
	}
    catch(TTStatus st) {
        	m_poInfo->Close();
        	printf("checkVCCable failed! err_msg=%s", st.err_msg);
    }
    //判断是否取到数据，如果没取到则造手工数据
    if(vACCTCap.size()==1)
    {
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-03-001","发布总消息数","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-03-002","发布拒收消息包数","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-03-003","发布失败消息包数","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);

        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-03-004","发布拒收消息包比率","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-03-005","发布处理失败消息包比率","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
    }
    
    oACCTCap.writeJsonFile(vACCTCap,SPLIT_FLAG); 
 
    return ;        	
}
/*获取查询接口性能*/
void CheckStatInfo::checkSeekInterCap(char *sAudit,int iType,char *sFreg)
{
	
	cout <<"CheckStatInfo::checkSeekInterCap(AuditId="<<sAudit<<"Type="<<iType<<"中频) ..."<<endl;
	
	char sStatBeginDate[15]={0};
    char sEndDate[15]={0};   
    StatInterface oACCTCap(sAudit,iType,sFreg);    
    vector<string> vACCTCap;
    vACCTCap.clear();
    vACCTCap.push_back(CAPAB_TITLE);
    
   	strncpy(sStatBeginDate,oACCTCap.sStatBeginDate,14);
  	strncpy(sEndDate,oACCTCap.sEndDate,14);   
  	 
    char sLineBuf[MaxLineLength] = {0};
    char sSQL[1024]={0};
    int iLogID=0;
  	 
  	float iMinQryUsr,iAvgQryUsr,iMaxQryUsr; 
  	float iMinQryCust,iAvgQryCust,iMaxQryCust;
  	float iMinQryProdoffer,iAvgQryProdoffer,iMaxQryProdoffer;
  	float iMinQryAcct,iAvgQryAcct,iMaxQryAcct;
  	float iMinQryMember,iAvgQryMember,iMaxQryMember;
  	float iMinQryInst,iAvgQryIns,iMaxQryIns;	
  	float iMinQryspecialhead,iAvgQryspecialhead,iMaxQryspecialhead;	
    float iMinQrylocnbr,iAvgQrylocnbr,iMaxQrylocnbr;	
    float iMinQrylocimsi,iAvgQrylocimsi,iMaxQrylocimsi;	
    float iMinQryhcode,iAvgQryhcode,iMaxQryhcode;    
    float iMinQrylochead,iAvgQrylochead,iMaxQrylochead;   
    float iMinQryedgermt,iAvgQryedgermt,iMaxQryedgermt;   
    float iMinQrycintlrmt,iAvgQrycintlrmt,iMaxQrycintlrmt;   
    float iMinQrydiffimsi,iAvgQrydiffimsi,iMaxQrydiffimsi;     
    float iMinQrymin,iAvgQrymin,iMaxQrymin;     
    float iMinQrynbrportab,iAvgQrynbrportab,iMaxQrynbrportab; 
    float iMinQrysp,iAvgQrysp,iMaxQrysp;       
    
    memset(sSQL,0,sizeof(sSQL));
    sprintf(sSQL," SELECT min (a.qry_usr)*1.0, avg(a.qry_usr)*1.0, max(a.qry_usr)*1.0,"
    			  " min(a.qry_cust)*1.0, avg(a.qry_cust)*1.0, max(qry_cust)*1.0,"
    			  " min(a.qry_prodoffer)*1.0, avg(a.qry_prodoffer)*1.0,max(a.qry_prodoffer)*1.0,"
    			  " min(a.qry_acct)*1.0, avg(a.qry_acct)*1.0, max(a.qry_acct)*1.0,"
    			  " min(a.qry_member)*1.0, avg(a.qry_member)*1.0,max(a.qry_member)*1.0,"
    			  " min(a.qry_inst)*1.0, avg(a.qry_inst)*1.0, max(a.qry_inst)*1.0,"
    			  " min(a.qry_specialhead)*1.0, avg(a.qry_specialhead)*1.0,max(a.qry_specialhead)*1.0,"
                  " min(a.qry_locnbr)*1.0, avg(a.qry_locnbr)*1.0, max(a.qry_locnbr)*1.0,"
                  " min(a.qry_locimsi)*1.0, avg(a.qry_locimsi)*1.0, max(a.qry_locimsi)*1.0,"
                  " min(a.qry_hcode)*1.0, avg(a.qry_hcode)*1.0, max(a.qry_hcode)*1.0,"
                  " min(a.qry_lochead)*1.0, avg(a.qry_lochead)*1.0, max(a.qry_lochead)*1.0,"
                  " min(a.qry_edgermt)*1.0, avg(a.qry_edgermt)*1.0, max(a.qry_edgermt)*1.0,"
                  " min(a.qry_cintlrmt)*1.0, avg(a.qry_cintlrmt)*1.0, max(a.qry_cintlrmt)*1.0,"
                  " min(a.qry_diffimsi)*1.0, avg(a.qry_diffimsi)*1.0, max(a.qry_diffimsi)*1.0,"
                  " min(a.qry_min)*1.0, avg(a.qry_min)*1.0, max(a.qry_min)*1.0,"
                  " min(a.qry_nbrportab)*1.0, avg(a.qry_nbrportab)*1.0, max(a.qry_nbrportab)*1.0,"
                  " min (a.qry_sp)*1.0, avg(a.qry_sp)*1.0, max(a.qry_sp)*1.0"
                  " FROM hss_qry_mess a where a.sys_time*100 >= %ld AND a.sys_time*100 < %ld",
                  atol(sStatBeginDate), atol(sEndDate));
    try{              
		m_poInfo->Prepare(sSQL);
   		 m_poInfo->Commit();	
    	  	
    	m_poInfo->Execute();
    	while (!m_poInfo->FetchNext()) {    
    	
   	 		m_poInfo->getColumn(1, &iMinQryUsr);
   	 		m_poInfo->getColumn(2, &iAvgQryUsr);
   	 		m_poInfo->getColumn(3, &iMaxQryUsr);
   	 		m_poInfo->getColumn(4, &iMinQryCust);
    		m_poInfo->getColumn(5, &iAvgQryCust);
    		m_poInfo->getColumn(6, &iMaxQryCust);
    		m_poInfo->getColumn(7, &iMinQryProdoffer);
    		m_poInfo->getColumn(8, &iAvgQryProdoffer);  
    		m_poInfo->getColumn(9, &iMaxQryProdoffer);    	
    		m_poInfo->getColumn(10, &iMinQryAcct);
    		m_poInfo->getColumn(11, &iAvgQryAcct);  
    		m_poInfo->getColumn(12, &iMaxQryAcct);
    		m_poInfo->getColumn(13, &iMinQryMember);
    		m_poInfo->getColumn(14, &iAvgQryMember);  
    		m_poInfo->getColumn(15, &iMaxQryMember);
    		m_poInfo->getColumn(16,&iMinQryInst);        
    		m_poInfo->getColumn(17,&iAvgQryIns);
    		m_poInfo->getColumn(18,&iMaxQryIns);
   			m_poInfo->getColumn(19,&iMinQryspecialhead);
    		m_poInfo->getColumn(20,&iAvgQryspecialhead);
    		m_poInfo->getColumn(21,&iMaxQryspecialhead);
    		m_poInfo->getColumn(22,&iMinQrylocnbr);
    		m_poInfo->getColumn(23,&iAvgQrylocnbr);
    		m_poInfo->getColumn(24,&iMaxQrylocnbr);
    		m_poInfo->getColumn(25,&iMinQrylocimsi);
    		m_poInfo->getColumn(26,&iAvgQrylocimsi);
    		m_poInfo->getColumn(27,&iMaxQrylocimsi);
    		m_poInfo->getColumn(28,&iMinQryhcode);
    		m_poInfo->getColumn(29,&iAvgQryhcode);
    		m_poInfo->getColumn(30,&iMaxQryhcode);
    		m_poInfo->getColumn(31,&iMinQrylochead);
    		m_poInfo->getColumn(32,&iAvgQrylochead);
    		m_poInfo->getColumn(33,&iMaxQrylochead);    	
    		m_poInfo->getColumn(34,&iMinQryedgermt);
    		m_poInfo->getColumn(35,&iAvgQryedgermt);
    		m_poInfo->getColumn(36,&iMaxQryedgermt);    
    		m_poInfo->getColumn(37,&iMinQrycintlrmt);
    		m_poInfo->getColumn(38,&iAvgQrycintlrmt);
    		m_poInfo->getColumn(39,&iMaxQryedgermt);   	
    		m_poInfo->getColumn(40,&iMinQrydiffimsi);
    		m_poInfo->getColumn(41,&iAvgQrydiffimsi);
    		m_poInfo->getColumn(42,&iMaxQrydiffimsi);    	   	
    		m_poInfo->getColumn(43,&iMinQrymin);
    		m_poInfo->getColumn(44,&iAvgQrymin);
    		m_poInfo->getColumn(45,&iMaxQrymin);    	
    		m_poInfo->getColumn(46,&iMinQrynbrportab);
    		m_poInfo->getColumn(47,&iAvgQrynbrportab);
    		m_poInfo->getColumn(48,&iMaxQrynbrportab);    	
    		m_poInfo->getColumn(49,&iMinQrysp);
    		m_poInfo->getColumn(50,&iAvgQrysp);
    		m_poInfo->getColumn(51,&iMaxQrysp);
    	
    		memset(sLineBuf,0,MaxLineLength);  	
       		sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-001",
                  	    		  "用户信息查询吞吐量",
                                  "/HSS",
                         		  iMinQryUsr,
                   		 		  iAvgQryUsr,
                   		          iMaxQryUsr                  
       		);          
     	 	vACCTCap.push_back(sLineBuf);

    		 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-002",
                  	    		  "客户信息查询吞吐量",
                                  "/HSS",
                         		  iMinQryCust,
                   		 		  iAvgQryCust,
                   		          iMaxQryCust                  
       	  );                    
     		vACCTCap.push_back(sLineBuf);
     		

      	sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-003",
                  	    		  "附属产品信息查询吞吐量",
                                  "/HSS",
                         		  iMinQryProdoffer,
                   		 		  iAvgQryProdoffer,
                   		          iMaxQryProdoffer                  
        );         
        vACCTCap.push_back(sLineBuf);
        
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-004",
                  	    		  "帐户信息查询吞吐量",
                                  "/HSS",
                         		  iMinQryAcct,
                   		 		  iAvgQryAcct,
                   		          iMaxQryAcct                  
        );                   
     	  vACCTCap.push_back(sLineBuf);
     	 	
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-005",
                  	    		  "通过群实例查找群成员吞吐量",
                                  "/HSS",
                         		  iMinQryInst,
                   		 		  iAvgQryIns,
                   		          iMaxQryIns                  
                         );                  
     	vACCTCap.push_back(sLineBuf);
  
     	sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-006",
                  	    		  "通过群成员查找群信息吞吐量",
                                  "/HSS",
                         		   iMinQryMember,
                   		 		   iAvgQryMember,
                   		           iMaxQryMember                  
                         );                  
     	vACCTCap.push_back(sLineBuf);
     	  
     	sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-007",
                  	    		  "特殊号头查询吞吐量",
                                  "/HSS",
                         		  iMinQryspecialhead,
                   		 		  iAvgQryspecialhead,
                   		          iMaxQryspecialhead                  
                        );                  
     	 vACCTCap.push_back(sLineBuf);
     	  
     	  		
     	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-008",
                  	    		  "归属查询（按号码）吞吐量",
                                  "/HSS",
                         		  iMinQrylocnbr,
                   		 		  iAvgQrylocnbr,
                   		          iMaxQrylocnbr                  
                         );                  
     	 vACCTCap.push_back(sLineBuf);    	  
     	  		
     	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-009",
                  	    		  "归属查询（按IMSI）吞吐量",
                                  "/HSS",
                         		  iMinQrylocimsi,
                   		 		  iAvgQrylocimsi,
                   		          iMaxQrylocimsi                  
                        );                  
     	  vACCTCap.push_back(sLineBuf);
     	  	  		
     		 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-010",
                  	    		  "H码查询吞吐量",
                                  "/HSS",
                         		  iMinQryhcode,
                   		 		  iAvgQryhcode,
                   		          iMaxQryhcode                  
                         );                  
     		  vACCTCap.push_back(sLineBuf);
     	  	
     	  		
     	 	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-011",
                  	    		  "本地号头表查询吞吐量",
                                  "/HSS",
                         		  iMinQrylochead,
                   		 		  iAvgQrylochead,
                   		          iMaxQrylochead                  
                         );                  
     	 	 vACCTCap.push_back(sLineBuf);
     	
     	 	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-012",
                  	    		  "边漫判断吞吐量",
                                  "/HSS",
                         		  iMinQryedgermt,
                   		 		  iAvgQryedgermt,
                   		          iMaxQryedgermt                  
                         );                  
     	 	 vACCTCap.push_back(sLineBuf);
     	  	    	  		
     	 	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-013",
                  	    		  "C网国际漫游运营商吞吐量",
                                  "/HSS",
                         		  iMinQrycintlrmt,
                   		 		  iAvgQrycintlrmt,
                   		          iMaxQryedgermt                  
                         );                  
     	 	 vACCTCap.push_back(sLineBuf);
     	       	  		
     	  	sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-014",
                  	    		  "不同制式IMSI码查询吞吐量",
                                  "/HSS",
                         		  iMinQrydiffimsi,
                   		 		  iAvgQrydiffimsi,
                   		          iMaxQrydiffimsi                  
                         );                  
     	 	 vACCTCap.push_back(sLineBuf);
     	  	
     		  sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-015",
                  	    		  "MIN码查询吞吐量",
                                  "/HSS",
                         		  iMinQrymin,
                   		 		  iAvgQrymin,
                   		          iMaxQrymin                  
                         );                  
     	 	 vACCTCap.push_back(sLineBuf);
     	  
     	  		
     	 	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-016",
                  	    		  "携号转网判断吞吐量",
                                  "/HSS",
                         		  iMinQrynbrportab,
                   		 		  iAvgQrynbrportab,
                   		          iMaxQrynbrportab                  
                         );                  
     	  	vACCTCap.push_back(sLineBuf);
       		               		
     	 	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "40-30-12-03-00-017",
                  	    		  "SP判断吞吐量",
                                  "/HSS",
                         		  iMinQrysp,
                   		 		  iAvgQrysp,
                   		          iMaxQrysp                  
                         );                  
     	  	vACCTCap.push_back(sLineBuf);       		               		
       	 // 	updateACCTCapa(iLogID);
   		}
	}

 	catch(TTStatus st) {                                              
        m_poInfo->Close();                                                
        printf("checkDataLoadBalance failed! err_msg=%s", st.err_msg);    
    }
     //判断是否取到数据，如果没取到则造手工数据                   
     if(vACCTCap.size()==1){                                                  
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-001","用户信息查询吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-002","客户信息查询吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-003","附属产品信息查询吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);

        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-004","帐户信息查询吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-005","通过群实例查找群成员吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-006","通过群成员查找群信息吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);

        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-007","特殊号头查询吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-008","归属查询（按号码）吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-009","归属查询（按IMSI）吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);

        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-010","H码查询吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
		
		memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-011","本地号头表查询吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-012","边漫判断吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-013","C网国际漫游运营商吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-014","不同制式IMSI码查询吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-015","MIN码查询吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-016","携号转网判断吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
        
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-03-00-017","SP判断吞吐量","/HSS",0.0,0.0,0.0);
        vACCTCap.push_back(sLineBuf);
    }
    oACCTCap.writeJsonFile(vACCTCap,SPLIT_FLAG);      
    return ;        	
	
	
}
/*获取查询接口告警*/
void CheckStatInfo::checkSeekInterAlarm(char *sAudit,int iType,char *sFreg)
{
		char sStatBeginDate[15]={0};
    char sEndDate[15]={0};   
	cout <<"CheckStatInfo::checkSeekInterAlarm(AuditId="<<sAudit<<"Type="<<iType<<"中频) ..."<<endl;
    StatInterface oACCTCap(sAudit,iType,sFreg);    
    vector<string> vACCTCap;
    vACCTCap.clear();
    vACCTCap.push_back(ALARM_TITLE);
    
    char sLineBuf[MaxLineLength] = {0};
    char sSQL[1024]={0};
    char  StringTemp[100];
       
   	strncpy(sStatBeginDate,oACCTCap.sStatBeginDate,14);
  	strncpy(sEndDate,oACCTCap.sEndDate,14);    
    int iLogID=0;
     
/*  float iMinQryUsr_e,iAvgQryUsr_e,iMaxQryUsr_e; 
  float iMinQryCust_e,iAvgQryCust_e,iMaxQryCust_e;
  float iMinQryProdoffer_e,iAvgQryProdoffer_e,iMaxQryProdoffer_e;
  float iMinQryAcct_e,iAvgQryAcct_e,iMaxQryAcct_e;
  float iMinQryMember_e,iAvgQryMember_e,iMaxQryMember_e;
  float iMinQryInst_e,iAvgQryIns_e,iMaxQryIns_e;	
  float iMinQryspecialhead_e,iAvgQryspecialhead_e,iMaxQryspecialhead_e;	
  float iMinQrylocnbr_e,iAvgQrylocnbr_e,iMaxQrylocnbr_e;	
  float iMinQrylocimsi_e,iAvgQrylocimsi_e,iMaxQrylocimsi_e;	
  float iMinQryhcode_e,iAvgQryhcode_e,iMaxQryhcode_e;    
  float iMinQrylochead_e,iAvgQrylochead_e,iMaxQrylochead_e;   
  float iMinQryedgermt_e,iAvgQryedgermt_e,iMaxQryedgermt_e;   
  float iMinQrycintlrmt_e,iAvgQrycintlrmt_e,iMaxQrycintlrmt_e;   
  float iMinQrydiffimsi_e,iAvgQrydiffimsi_e,iMaxQrydiffimsi_e;     
  float iMinQrymin_e,iAvgQrymin_e,iMaxQrymin_e;     
  float iMinQrynbrportab_e,iAvgQrynbrportab_e,iMaxQrynbrportab_e; 
  float iMinQrysp_e,iAvgQrysp_e,iMaxQrysp_e;    
     
    memset(sSQL,0,sizeof(sSQL));
    sprintf(sSQL," SELECT min (a.qry_usr_e)*1.0, avg(a.qry_usr_e)*1.0, max(a.qry_usr_e)*1.0,"
    			  " min(a.qry_cust_e)*1.0, avg(a.qry_cust_e)*1.0, max(qry_cust_e)*1.0,"
    			  " min(a.qry_prodoffer_e)*1.0, avg(a.qry_prodoffer_e)*1.0,max(a.qry_prodoffer_e)*1.0,"
    			  " min(a.qry_acct_e)*1.0, avg(a.qry_acct_e)*1.0, max(a.qry_acct_e)*1.0,"
    			  " min(a.qry_member_e)*1.0, avg(a.qry_member_e)*1.0,max(a.qry_member_e)*1.0,"
    			  " min(a.qry_inst_e)*1.0, avg(a.qry_inst_e)*1.0, max(a.qry_inst_e)*1.0,"
    			  " min(a.qry_specialhead_e)*1.0, avg(a.qry_specialhead_e)*1.0,max(a.qry_specialhead_e)*1.0,"
                  " min(a.qry_locnbr_e)*1.0, avg(a.qry_locnbr_e)*1.0, max(a.qry_locnbr_e)*1.0,"
                  " min(a.qry_locimsi_e)*1.0, avg(a.qry_locimsi_e)*1.0, max(a.qry_locimsi_e)*1.0,"
                  " min(a.qry_hcode_e)*1.0, avg(a.qry_hcode_e)*1.0, max(a.qry_hcode_e)*1.0,"
                  " min(a.qry_lochead_e)*1.0, avg(a.qry_lochead_e)*1.0, max(a.qry_lochead_e)*1.0,"
                  " min(a.qry_edgermt_e)*1.0, avg(a.qry_edgermt_e)*1.0, max(a.qry_edgermt_e)*1.0,"
                  " min(a.qry_cintlrmt_e)*1.0, avg(a.qry_cintlrmt_e)*1.0, max(a.qry_cintlrmt_e)*1.0,"
                  " min(a.qry_diffimsi_e)*1.0, avg(a.qry_diffimsi_e)*1.0, max(a.qry_diffimsi_e)*1.0,"
                  " min(a.qry_min_e)*1.0, avg(a.qry_min_e)*1.0, max(a.qry_min_e)*1.0,"
                  " min(a.qry_nbrportab_e)*1.0, avg(a.qry_nbrportab_e)*1.0, max(a.qry_nbrportab_e)*1.0,"
                  " min (a.qry_sp_e)*1.0, avg(a.qry_sp_e)*1.0, max(a.qry_sp_e)*1.0"
                  " FROM hss_qry_mess a where a.sys_time BETWEEN %ld AND %ld",
                  atol(sStatBeginDate), atol(sEndDate));          
 	 m_poInfo->Prepare(sSQL);
    m_poInfo->Commit();	
    	  	
    m_poInfo->Execute();
    while (!m_poInfo->FetchNext()) {    
    	
   	 	m_poInfo->getColumn(1, &iMinQryUsr_e);
   	 	m_poInfo->getColumn(2, &iAvgQryUsr_e);
   	 	m_poInfo->getColumn(3, &iMaxQryUsr_e);
   	 	m_poInfo->getColumn(4, &iMinQryCust_e);
    	m_poInfo->getColumn(5, &iAvgQryCust_e);
    	m_poInfo->getColumn(6, &iMaxQryCust_e);
    	m_poInfo->getColumn(7, &iMinQryProdoffer_e);
    	m_poInfo->getColumn(8, &iAvgQryProdoffer_e);  
    	m_poInfo->getColumn(9, &iMaxQryProdoffer_e);    	
    	m_poInfo->getColumn(10, &iMinQryAcct_e);
    	m_poInfo->getColumn(11, &iAvgQryAcct_e);  
    	m_poInfo->getColumn(12, &iMaxQryAcct_e);
    	m_poInfo->getColumn(13, &iMinQryMember_e);
    	m_poInfo->getColumn(14, &iAvgQryMember_e);  
    	m_poInfo->getColumn(15, &iMaxQryMember_e);
    	m_poInfo->getColumn(16,&iMinQryInst_e);        
    	m_poInfo->getColumn(17,&iAvgQryIns_e);
    	m_poInfo->getColumn(18,&iMaxQryIns_e);
   		m_poInfo->getColumn(19,&iMinQryspecialhead_e);
    	m_poInfo->getColumn(20,&iAvgQryspecialhead_e);
    	m_poInfo->getColumn(21,&iMaxQryspecialhead_e);
    	m_poInfo->getColumn(22,&iMinQrylocnbr_e);
    	m_poInfo->getColumn(23,&iAvgQrylocnbr_e);
    	m_poInfo->getColumn(24,&iMaxQrylocnbr_e);
    	m_poInfo->getColumn(25,&iMinQrylocimsi_e);
    	m_poInfo->getColumn(26,&iAvgQrylocimsi_e);
    	m_poInfo->getColumn(27,&iMaxQrylocimsi_e);
    	m_poInfo->getColumn(28,&iMinQryhcode_e);
    	m_poInfo->getColumn(29,&iAvgQryhcode_e);
    	m_poInfo->getColumn(30,&iMaxQryhcode_e);
    	m_poInfo->getColumn(31,&iMinQrylochead_e);
    	m_poInfo->getColumn(32,&iAvgQrylochead_e);
    	m_poInfo->getColumn(33,&iMaxQrylochead_e);    	
    	m_poInfo->getColumn(34,&iMinQryedgermt_e);
    	m_poInfo->getColumn(35,&iAvgQryedgermt_e);
    	m_poInfo->getColumn(36,&iMaxQryedgermt_e);    
    	m_poInfo->getColumn(37,&iMinQrycintlrmt_e);
    	m_poInfo->getColumn(38,&iAvgQrycintlrmt_e);
    	m_poInfo->getColumn(39,&iMaxQryedgermt_e);   	
    	m_poInfo->getColumn(40,&iMinQrydiffimsi_e);
    	m_poInfo->getColumn(41,&iAvgQrydiffimsi_e);
    	m_poInfo->getColumn(42,&iMaxQrydiffimsi_e);    	   	
    	m_poInfo->getColumn(43,&iMinQrymin_e);
    	m_poInfo->getColumn(44,&iAvgQrymin_e);
    	m_poInfo->getColumn(45,&iMaxQrymin_e);    	
    	m_poInfo->getColumn(46,&iMinQrynbrportab_e);
    	m_poInfo->getColumn(47,&iAvgQrynbrportab_e);
    	m_poInfo->getColumn(48,&iMaxQrynbrportab_e);    	
    	m_poInfo->getColumn(49,&iMinQrysp_e);
    	m_poInfo->getColumn(50,&iAvgQrysp_e);
    	m_poInfo->getColumn(51,&iMaxQrysp_e);
    	
    	memset(sLineBuf,0,MaxLineLength);  	
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-001",
                  	    		  "用户信息查询吞吐量",
                                  "/HSS",
                         		  iMinQryUsr_e,
                   		 		  iAvgQryUsr_e,
                   		          iMaxQryUsr_e                  
       	 );          
     	 vACCTCap.push_back(sLineBuf);

    	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-002",
                  	    		  "客户信息查询吞吐量",
                                  "/HSS",
                         		  iMinQryCust_e,
                   		 		  iAvgQryCust_e,
                   		          iMaxQryCust_e                  
         );                    
     	vACCTCap.push_back(sLineBuf);

    	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-003",
                  	    		  "附属产品信息查询异常的吞吐量",
                                  "/HSS",
                         		  iMinQryProdoffer_e,
                   		 		  iAvgQryProdoffer_e,
                   		          iMaxQryProdoffer_e                  
         );                    
     	vACCTCap.push_back(sLineBuf);     	

      	sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-004",
                  	    		  "帐户信息查询吞吐量",
                                  "/HSS",
                         		  iMinQryAcct_e,
                   		 		  iAvgQryAcct_e,
                   		          iMaxQryAcct_e                  
        );         
        vACCTCap.push_back(sLineBuf);
  	
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-005",
                  	    		  "通过群实例查找群成员吞吐量",
                                  "/HSS",
                         		  iMinQryMember_e,
                   		 		   iAvgQryMember_e,
                   		           iMaxQryMember_e                  
        );                  
     	vACCTCap.push_back(sLineBuf);
     	 	
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-006",
                  	    		  "通过群成员查找群信息吞吐量",
                                  "/HSS",
                         		  iMinQryInst_e,
                   		 		  iAvgQryIns_e,
                   		          iMaxQryIns_e                  
                         );                  
     	vACCTCap.push_back(sLineBuf);
  
     	sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-007",
                  	    		  "特殊号头查询吞吐量",
                                  "/HSS",
                         		  iMinQryspecialhead_e,
                   		 		  iAvgQryspecialhead_e,
                   		          iMaxQryspecialhead_e                  
                         );                  
     	vACCTCap.push_back(sLineBuf);
     	  
     	sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-008",
                  	    		  "归属查询（按号码）吞吐量",
                                  "/HSS",
                         		  iMinQrylocnbr_e,
                   		 		  iAvgQrylocnbr_e,
                   		          iMaxQrylocnbr_e                  
                        );                  
     	 vACCTCap.push_back(sLineBuf);
     	  
     	  		
     	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-009",
                  	    		  "归属查询（按IMSI）吞吐量",
                                  "/HSS",
                         		  iMinQrylocimsi_e,
                   		 		  iAvgQrylocimsi_e,
                   		          iMaxQrylocimsi_e                  
                         );                  
     	 vACCTCap.push_back(sLineBuf);
     	  
     	  		
     	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-010",
                  	    		  "H码查询吞吐量",
                                  "/HSS",
                         		  iMinQryhcode_e,
                   		 		  iAvgQryhcode_e,
                   		          iMaxQryhcode_e                  
                        );                  
     	  vACCTCap.push_back(sLineBuf);
     	  	  		
     		 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-011",
                  	    		  "本地号头表查询吞吐量",
                                  "/HSS",
                         		  iMinQrylochead_e,
                   		 		  iAvgQrylochead_e,
                   		          iMaxQrylochead_e                  
                         );                  
     		  vACCTCap.push_back(sLineBuf);
     	  	   	  		
     	 	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-012",
                  	    		  "边漫判断吞吐量",
                                  "/HSS",
                         		  iMinQryedgermt_e,
                   		 		  iAvgQryedgermt_e,
                   		          iMaxQryedgermt_e                  
                         );                  
     	 	 vACCTCap.push_back(sLineBuf);
     	
     	 	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-013",
                  	    		  "C网国际漫游运营商吞吐量",
                                  "/HSS",
                         		  iMinQrycintlrmt_e,
                   		 		  iAvgQrycintlrmt_e,
                   		          iMaxQrycintlrmt_e                  
                         );                  
     	 	 vACCTCap.push_back(sLineBuf);
     	  	    	  		
     	 	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-014",
                  	    		  "不同制式IMSI码查询吞吐量",
                                  "/HSS",
                         		  iMinQrydiffimsi_e,
                   		 		  iAvgQrydiffimsi_e,
                   		          iMaxQrydiffimsi_e                  
                         );                  
     	 	 vACCTCap.push_back(sLineBuf);
     	       	  		
     	  	sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-015",
                  	    		  "MIN码查询吞吐量",
                                  "/HSS",
                         		  iMinQrymin_e,
                   		 		  iAvgQrymin_e,
                   		          iMaxQrymin_e                  
                         );                  
     	 	 vACCTCap.push_back(sLineBuf);
     	  	
     		  sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-016",
                  	    		  "携号转网判断吞吐量",
                                  "/HSS",
                         		  iMinQrynbrportab_e,
                   		 		  iAvgQrynbrportab_e,
                   		          iMaxQrynbrportab_e                  
                         );                  
     	 	 vACCTCap.push_back(sLineBuf);
     	  
     	  		
     	 	 sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
            	    	          "20-30-12-03-00-017",
                  	    		  "SP判断吞吐量",
                                  "/HSS",
                         		  iMinQrysp_e,
                   		 		  iAvgQrysp_e,
                   		          iMaxQrysp_e                  
                         );                  
     	  	vACCTCap.push_back(sLineBuf);
       		               
     	  	vACCTCap.push_back(sLineBuf);      		
       	//  	updateACCTCapa(iLogID);
   		}*/
   	long lMinTime[24] = {0},lMaxTime[24] = {0}, lsys_time = 0,icount = 0;
   	char sMinTime[16] = {0},sMaxTime[16] = {0};
   	long  qry_usr_e = 0,qry_cust_e = 0,qry_prodoffer_e = 0;
   	long  qry_acct_e = 0,qry_member_e = 0,qry_inst_e = 0;
   	long  qry_specialhead_e = 0,qry_locnbr_e = 0,qry_locimsi_e = 0;
   	long  qry_hcode_e = 0,qry_lochead_e = 0,qry_edgermt_e = 0;
   	long  qry_cintlrmt_e = 0,qry_diffimsi_e = 0,qry_min_e = 0;
   	long  qry_nbrportab_e = 0,qry_sp_e = 0;
   	long  lAlarmID = 2500;  //自定义查询告警错误码 
   	int  iNum[24] = {0};
   	try{
   	m_poInfo->Prepare(" select sys_time, "
   	                  " qry_usr_e,qry_cust_e,qry_prodoffer_e, "
   	                  " qry_acct_e,qry_member_e,qry_inst_e, "
   	                  " qry_specialhead_e,qry_locnbr_e,qry_locimsi_e, "
   	                  " qry_hcode_e,qry_lochead_e,qry_edgermt_e, "
   	                  " qry_cintlrmt_e,qry_diffimsi_e,qry_min_e, "
   	                  " qry_nbrportab_e,qry_sp_e "
   	                  " from  hss_qry_mess "
   	                  " where sys_time >= :statBeginDate and sys_time < :statEndDate ");
   	m_poInfo->Commit();
    m_poInfo->setParam(1, atol(sStatBeginDate)/100);
    m_poInfo->setParam(2, atol(sEndDate)/100);
    m_poInfo->Execute();
    while(!m_poInfo->FetchNext())
    {
        icount++;
    	  m_poInfo->getColumn(1, &lsys_time);
    	  lsys_time = lsys_time*100;
    	  m_poInfo->getColumn(2, &qry_usr_e);
    	  if(qry_usr_e)
    	  {
    	  	iNum[0]++;
    	  	if(iNum[0] == 1)
    	  	{
    	  		lMinTime[0] = lsys_time;
    	  		lMaxTime[0] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[0] = (lMinTime[0]<lsys_time)?lMinTime[0]:lsys_time;
    	  		lMaxTime[0] = (lMaxTime[0]>lsys_time)?lMaxTime[0]:lsys_time;
    	  	}
    	  }
    	  m_poInfo->getColumn(3, &qry_cust_e);
    	  if(qry_cust_e)
    	  {
    	  	iNum[1]++;
    	  	if(iNum[1] == 1)
    	  	{
    	  		lMinTime[1] = lsys_time;
    	  		lMaxTime[1] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[1] = (lMinTime[1]<lsys_time)?lMinTime[1]:lsys_time;
    	  		lMaxTime[1] = (lMaxTime[1]>lsys_time)?lMaxTime[1]:lsys_time;
    	  	}
    	  }    	  
    	  m_poInfo->getColumn(4, &qry_prodoffer_e);
    	  if(qry_prodoffer_e)
    	  {
    	  	iNum[2]++;
    	  	if(iNum[2] == 1)
    	  	{
    	  		lMinTime[2] = lsys_time;
    	  		lMaxTime[2] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[2] = (lMinTime[2]<lsys_time)?lMinTime[2]:lsys_time;
    	  		lMaxTime[2] = (lMaxTime[2]>lsys_time)?lMaxTime[2]:lsys_time;
    	  	}
    	  }     	  
    	  m_poInfo->getColumn(5, &qry_acct_e);
    	  if(qry_acct_e)
    	  {
    	  	iNum[3]++;
    	  	if(iNum[3] == 1)
    	  	{
    	  		lMinTime[3] = lsys_time;
    	  		lMaxTime[3] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[3] = (lMinTime[3]<lsys_time)?lMinTime[3]:lsys_time;
    	  		lMaxTime[3] = (lMaxTime[3]>lsys_time)?lMaxTime[3]:lsys_time;
    	  	}
    	  }       	  
    	  m_poInfo->getColumn(6, &qry_member_e);
    	  if(qry_member_e)
    	  {
    	  	iNum[4]++;
    	  	if(iNum[4] == 1)
    	  	{
    	  		lMinTime[4] = lsys_time;
    	  		lMaxTime[4] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[4] = (lMinTime[4]<lsys_time)?lMinTime[4]:lsys_time;
    	  		lMaxTime[4] = (lMaxTime[4]>lsys_time)?lMaxTime[4]:lsys_time;
    	  	}
    	  }    	  
    	  m_poInfo->getColumn(7, &qry_inst_e);
    	  if(qry_inst_e)
    	  {
    	  	iNum[5]++;
    	  	if(iNum[5] == 1)
    	  	{
    	  		lMinTime[5] = lsys_time;
    	  		lMaxTime[5] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[5] = (lMinTime[5]<lsys_time)?lMinTime[5]:lsys_time;
    	  		lMaxTime[5] = (lMaxTime[5]>lsys_time)?lMaxTime[5]:lsys_time;
    	  	}
    	  }    	  
    	  m_poInfo->getColumn(8, &qry_specialhead_e);
    	  if(qry_specialhead_e)
    	  {
    	  	iNum[6]++;
    	  	if(iNum[6] == 1)
    	  	{
    	  		lMinTime[6] = lsys_time;
    	  		lMaxTime[6] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[6] = (lMinTime[6]<lsys_time)?lMinTime[6]:lsys_time;
    	  		lMaxTime[6] = (lMaxTime[6]>lsys_time)?lMaxTime[6]:lsys_time;
    	  	}
    	  }    	  
    	  m_poInfo->getColumn(9, &qry_locnbr_e);
    	  if(qry_locnbr_e)
    	  {
    	  	iNum[7]++;
    	  	if(iNum[7] == 1)
    	  	{
    	  		lMinTime[7] = lsys_time;
    	  		lMaxTime[7] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[7] = (lMinTime[7]<lsys_time)?lMinTime[7]:lsys_time;
    	  		lMaxTime[7] = (lMaxTime[7]>lsys_time)?lMaxTime[7]:lsys_time;
    	  	}
    	  }        	  
    	  m_poInfo->getColumn(10, &qry_locimsi_e);
    	  if(qry_locimsi_e)
    	  {
    	  	iNum[8]++;
    	  	if(iNum[8] == 1)
    	  	{
    	  		lMinTime[8] = lsys_time;
    	  		lMaxTime[8] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[8] = (lMinTime[8]<lsys_time)?lMinTime[8]:lsys_time;
    	  		lMaxTime[8] = (lMaxTime[8]>lsys_time)?lMaxTime[8]:lsys_time;
    	  	}
    	  }     	  
    	  m_poInfo->getColumn(11, &qry_hcode_e);
    	  if(qry_hcode_e)
    	  {
    	  	iNum[9]++;
    	  	if(iNum[9] == 1)
    	  	{
    	  		lMinTime[9] = lsys_time;
    	  		lMaxTime[9] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[9] = (lMinTime[9]<lsys_time)?lMinTime[9]:lsys_time;
    	  		lMaxTime[9] = (lMaxTime[9]>lsys_time)?lMaxTime[9]:lsys_time;
    	  	}
    	  }     	  
    	  m_poInfo->getColumn(12, &qry_lochead_e);
    	  if(qry_lochead_e)
    	  {
    	  	iNum[10]++;
    	  	if(iNum[10] == 1)
    	  	{
    	  		lMinTime[10] = lsys_time;
    	  		lMaxTime[10] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[10] = (lMinTime[10]<lsys_time)?lMinTime[10]:lsys_time;
    	  		lMaxTime[10] = (lMaxTime[10]>lsys_time)?lMaxTime[10]:lsys_time;
    	  	}
    	  }    	  
    	  m_poInfo->getColumn(13, &qry_edgermt_e);
    	  if(qry_edgermt_e)
    	  {
    	  	iNum[11]++;
    	  	if(iNum[11] == 1)
    	  	{
    	  		lMinTime[11] = lsys_time;
    	  		lMaxTime[11] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[11] = (lMinTime[11]<lsys_time)?lMinTime[11]:lsys_time;
    	  		lMaxTime[11] = (lMaxTime[11]>lsys_time)?lMaxTime[11]:lsys_time;
    	  	}
    	  }      	  
    	  m_poInfo->getColumn(14, &qry_cintlrmt_e);
    	  if(qry_cintlrmt_e)
    	  {
    	  	iNum[12]++;
    	  	if(iNum[12] == 1)
    	  	{
    	  		lMinTime[12] = lsys_time;
    	  		lMaxTime[12] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[12] = (lMinTime[12]<lsys_time)?lMinTime[12]:lsys_time;
    	  		lMaxTime[12] = (lMaxTime[12]>lsys_time)?lMaxTime[12]:lsys_time;
    	  	}
    	  }     	  
    	  m_poInfo->getColumn(15, &qry_diffimsi_e);
    	  if(qry_cintlrmt_e)
    	  {
    	  	iNum[13]++;
    	  	if(iNum[13] == 1)
    	  	{
    	  		lMinTime[13] = lsys_time;
    	  		lMaxTime[13] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[13] = (lMinTime[13]<lsys_time)?lMinTime[13]:lsys_time;
    	  		lMaxTime[13] = (lMaxTime[13]>lsys_time)?lMaxTime[13]:lsys_time;
    	  	}
    	  }       	  
    	  m_poInfo->getColumn(16, &qry_min_e);
    	  if(qry_min_e)
    	  {
    	  	iNum[14]++;
    	  	if(iNum[14] == 1)
    	  	{
    	  		lMinTime[14] = lsys_time;
    	  		lMaxTime[14] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[14] = (lMinTime[14]<lsys_time)?lMinTime[14]:lsys_time;
    	  		lMaxTime[14] = (lMaxTime[14]>lsys_time)?lMaxTime[14]:lsys_time;
    	  	}
    	  }     	  
    	  m_poInfo->getColumn(17, &qry_nbrportab_e);
    	  if(qry_nbrportab_e)
    	  {
    	  	iNum[15]++;
    	  	if(iNum[15] == 1)
    	  	{
    	  		lMinTime[15] = lsys_time;
    	  		lMaxTime[15] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[15] = (lMinTime[15]<lsys_time)?lMinTime[15]:lsys_time;
    	  		lMaxTime[15] = (lMaxTime[15]>lsys_time)?lMaxTime[15]:lsys_time;
    	  	}
    	  }     	  
    	  m_poInfo->getColumn(18, &qry_sp_e);
    	  if(qry_nbrportab_e)
    	  {
    	  	iNum[16]++;
    	  	if(iNum[16] == 1)
    	  	{
    	  		lMinTime[16] = lsys_time;
    	  		lMaxTime[16] = lsys_time;
    	  	}else
    	  	{
    	  		lMinTime[16] = (lMinTime[16]<lsys_time)?lMinTime[16]:lsys_time;
    	  		lMaxTime[16] = (lMaxTime[16]>lsys_time)?lMaxTime[16]:lsys_time;
    	  	}
    	  }     	  
    }
    	  if(iNum[0]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[0]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[0]);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-001","用户信息查询异常的吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[0]);
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[1]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[1]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[1]);        	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "40-30-12-03-00-002","客户信息查询异常的吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[1]);        
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[2]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[2]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[2]);        	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-003","附属产品信息查询异常的吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[2]);         
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[3]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[3]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[3]);         	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-004","帐户信息查询异常的吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[3]);                      
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[4]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[4]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[4]);         	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-005","通过群实例查找群成员异常的吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[4]);                    
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[5]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[5]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[5]);         	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-006","通过群成员查找群信息吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[5]);             
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[6]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[6]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[6]);         	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-007","特殊号头查询吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[6]);          
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[7]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[7]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[7]);         	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s||/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-008","归属查询（按号码）异常的吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[7]);             
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[8]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[8]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[8]);        	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-009","归属查询（按IMSI）异常的吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[8]);                 
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[9]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[9]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[9]);         	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-010","H码查询吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[9]);            
        vACCTCap.push_back(sLineBuf);
		    } 
		   
		    if(iNum[10]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[10]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[10]); 		    	
		    memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-011","本地号头表查询吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[10]);  		
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[11]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[11]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[11]);        	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-012","边漫判断异常的吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[11]);  		        
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[12]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[12]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[12]);           	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-013","C网国际漫游运营商异常的吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[12]);         
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[13]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[13]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[13]);         	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-014","不同制式IMSI码查询异常的吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[13]);         
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[14]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[14]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[14]);         	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-015","MIN码查询异常的吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[14]);         
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[15]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[15]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[15]);         	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-016","携号转网判断异常的吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[15]);         
        vACCTCap.push_back(sLineBuf);
        }
        
        if(iNum[16]>0){
    	  sprintf(sMinTime,"%ld",lMinTime[16]);
    	  sprintf(sMaxTime,"%ld",lMaxTime[16]);         	
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%ld|/HSS||%s|%s|%d|%d|||||%d", 
                     "20-30-12-03-00-017","SP判断异常的吞吐量",lAlarmID,sMinTime,sMaxTime,
                     1,10,iNum[16]);            
        vACCTCap.push_back(sLineBuf);
        }
     m_poInfo->Close();
  }catch(TTStatus st) {
        m_poInfo->Close();
        printf("checkSeekInterAlarm failed! err_msg=%s", st.err_msg);
    }
    oACCTCap.writeJsonFile(vACCTCap,SPLIT_FLAG); 

    return ;        	
		
}


void CheckStatInfo::upCautionKipLog(long lLogID)
{
  DEFINE_QUERY(qry);
	qry.setSQL(" update B_INFO_CAUTION_INTERFACE t "
			   " set t.DEAL_FLAG = 1 "
			   " where t.KPI_LOG =:log_id ");

	qry.setParameter("log_id", lLogID);			   	
	qry.execute();	
	qry.commit();
	qry.close();	   
}	

/*-----------------------hss--------------------*/
/*连接TTT数据库*/
int  CheckStatInfo::connectTT()
{
	TimesTenConn* pTTConn = NULL;
	char sConnTns[8] = "hss";
	ABMException oExp;
	
	try {
    	if (GET_CURSOR(m_poInfo, pTTConn, sConnTns, oExp) != 0) {
         	ADD_EXCEPT0(oExp, "connec to TT failed!");
            	return -1;
           }
        }
        catch(TTStatus st) {
              ADD_EXCEPT1(oExp, "connec to TT failed", st.err_msg);
            return -1;
        }
	return 0;
}

/*
 *	函 数 名 : checkOutputBalance
 *	函数功能 : 数据发布平衡,AuditId=0004，Type=50,中频
 *	时    间 : 2011年7月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkOutputBalance()
{
   cout <<"CheckStatInfo::checkOutputBalance(AuditId=0004，Type=50,中频) ..."<<endl;
   
	 StatInterface ocheckOutputBalance("0004",50,MIDL_FREG);
	 vector<string> vcheckOutputBalance;
	 vcheckOutputBalance.clear();
	  char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "I-DATA_SOURCE|"
                        "S-OUT_NET_ID|"
                        "I-OUTPUT_MSG_NUM|"
                        "I-SUCC_MSG_NUM|"
                        "I-REJECT_MSG_NUM|"
                        "I-FAIL_MSG_NUM|"
                        "S-END_TIME");                        
    vcheckOutputBalance.push_back(string(tmpbuf));
	 
	 int iDataSource = 0;
	 int iOutNetId = 0;
	 int iOutputMsgNum = 0;
   int iSuccMsgNum = 0;
   int iRejectMsgNum = 0;
   int iFailMsgNum = 0;
   long lEndTime = 0;
	 
	 char sLineBuf[MaxLineLength] = {0};
   
   try{
   m_poInfo->Prepare("SELECT data_src,in_ne,recv_num, "
                     " succ_num,rjct_num,fail_num,sys_time "
                     " FROM hss_balance_mess "
                     " where balance_type = 3 and "
                     " sys_time >= :statBeginDate and sys_time < :statEndDate");
   m_poInfo->Commit();
   m_poInfo->setParam(1, atol(ocheckOutputBalance.sStatBeginDate)/100);
   m_poInfo->setParam(2, atol(ocheckOutputBalance.sEndDate)/100);
   m_poInfo->Execute();
   while(!m_poInfo->FetchNext())
   {
   	 m_poInfo->getColumn(1, &iDataSource);
   	 iDataSource = m_iProveNum;
   	 m_poInfo->getColumn(2, &iOutNetId);
   	 m_poInfo->getColumn(3, &iOutputMsgNum);
   	 m_poInfo->getColumn(4, &iSuccMsgNum);
   	 m_poInfo->getColumn(5, &iRejectMsgNum);
   	 m_poInfo->getColumn(6, &iFailMsgNum);
   	 m_poInfo->getColumn(7, &lEndTime);
   	 lEndTime = lEndTime*100;
   	 
   	memset(sLineBuf, 0, sizeof(sLineBuf));	
   	sprintf(sLineBuf, "%d|%d|%d|%d|%d|%d|%d|%ld", 
   	                DR_ID, iDataSource, iOutNetId, iOutputMsgNum, iSuccMsgNum, 
   									iRejectMsgNum, iFailMsgNum, lEndTime);
    cout<<sLineBuf<<endl;									
   	vcheckOutputBalance.push_back(string(sLineBuf));
   }
   
     m_poInfo->Close(); 
    }catch(TTStatus st) {
        m_poInfo->Close();
        printf("checkOutputBalance failed! err_msg=%s", st.err_msg);
    }
     ocheckOutputBalance.writeJsonFile(vcheckOutputBalance,SPLIT_FLAG);
     return 0;
}


/*
 *	函 数 名 : checkInstanceUpdate
 *	函数功能 : 实例数变更,AuditId=0005，Type=50,中频
 *	时    间 : 2011年7月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkInstanceUpdate()
{
   cout <<"CheckStatInfo::checkInstanceUpdate(AuditId=0005，Type=50,中频) ..."<<endl;
   
	 StatInterface ocheckInstanceUpdate("0005",50,MIDL_FREG);
	 vector<string> vcheckInstanceUpdate;
	 vcheckInstanceUpdate.clear();
	  char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "I-DATA_SOURCE|"
                        "I-CUST_NUM|"
                        "I-SERV_NUM|"
                        "I-ACCT_NUM|"
                        "I-PRODUCT_OFFER_INST_NUM|"
                        "S-END_TIME");                        
    vcheckInstanceUpdate.push_back(string(tmpbuf));
	 
   int iDataSource = 0;
   int iCustNum = 0;
   int iServNum = 0;
   int iAcctNum = 0;
   int iProductOfferInstNum = 0;
	 long lEndTime = 0;
	 
	 char sLineBuf[MaxLineLength] = {0};
	 
	 try{
	 m_poInfo->Prepare("SELECT data_src,cust_num,user_num, "
	                   " acct_num,prodoffer_num,sys_time from hss_inst_update_mess "
	                   " where sys_time >= :statBeginDate and sys_time < :statEndDate");
	 
   m_poInfo->Commit();
   m_poInfo->setParam(1, atol(ocheckInstanceUpdate.sStatBeginDate)/100);
   m_poInfo->setParam(2, atol(ocheckInstanceUpdate.sEndDate)/100);
   m_poInfo->Execute();
   while(!m_poInfo->FetchNext())
   {
   	m_poInfo->getColumn(1, &iDataSource);
   	iDataSource = m_iProveNum;
   	m_poInfo->getColumn(2, &iCustNum);
   	m_poInfo->getColumn(3, &iServNum);
   	m_poInfo->getColumn(4, &iAcctNum);
   	m_poInfo->getColumn(5, &iProductOfferInstNum);
   	m_poInfo->getColumn(6, &lEndTime);
   	lEndTime = lEndTime*100;
   	
   	memset(sLineBuf, 0, sizeof(sLineBuf));	
   	sprintf(sLineBuf, "%d|%d|%d|%d|%d|%d|%ld", 
   	                DR_ID, iDataSource, iCustNum, iServNum, iAcctNum, 
   									iProductOfferInstNum, lEndTime);
    cout<<sLineBuf<<endl;									
   	vcheckInstanceUpdate.push_back(string(sLineBuf));
   	
   }
    
     m_poInfo->Close(); 
    }catch(TTStatus st) {
        m_poInfo->Close();
        printf("checkInstanceUpdate failed! err_msg=%s", st.err_msg);
    }
     ocheckInstanceUpdate.writeJsonFile(vcheckInstanceUpdate,SPLIT_FLAG);
     return 0;
}

/*
 *	函 数 名 : checkAuthenticationError
 *	函数功能 : 鉴权异常,AuditId=0008，Type=50,高频
 *	时    间 : 2011年7月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkAuthenticationError()
{
   cout <<"CheckStatInfo::checkAuthenticationError(AuditId=0008，Type=50,高频) ..."<<endl;
   
	 StatInterface ocheckAuthenticationError("0008",50,HIGH_FREG);
	 vector<string> vcheckAuthenticationError;
	 vcheckAuthenticationError.clear();
	  char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "I-DATA_SOURCE|"
                        "S-IN_NET_ID|"
                        "I-SERVICE_ID|"
                        "S-SERVICE_NAME|"
                        "S-ERR_MSG|"
                        "S-CREATE_TIME");                        
    vcheckAuthenticationError.push_back(string(tmpbuf));
	 
   int iDataSource = 0;
   int iInNetId = 0;
   int iServiceId = 0;
   char sServiceName[64] = {0};
   char sErrMsg[128] = {0};
	 char sCreateTime[16] = {0};
	 
	 char sLineBuf[MaxLineLength] = {0};
	 
/*	 m_poInfo->Prepare(" select data_source,in_net_id, "
	     "  service_id,service_name,err_msg, "
	     "  to_char(create_time,'yyyymmdd24miss') from hss_authentication_error_log " 
	 		 "  where create_time between to_date(:statBeginDate, 'yyyymmddhh24miss')+ 1/(24*3600) and to_date(:statEndDate, 'yyyymmddhh24miss') ");
	 
	 m_poInfo->Commit();
   m_poInfo->setParam(1, atol(ocheckAuthenticationError.sStatBeginDate));
   m_poInfo->setParam(2, atol(ocheckAuthenticationError.sEndDate));
   
   m_poInfo->Execute();
   while(!m_poInfo->FetchNext())
   {
   	m_poInfo->getColumn(1, &iDataSource);
   	iDataSource = m_iProveNum;
   	m_poInfo->getColumn(2, &iInNetId);
   	m_poInfo->getColumn(3, &iServiceId);
   	m_poInfo->getColumn(4, sServiceName);
   	m_poInfo->getColumn(5, sErrMsg);
   	m_poInfo->getColumn(6, sCreateTime);
   	
   	memset(sLineBuf, 0, sizeof(sLineBuf));	
   	sprintf(sLineBuf, "%d|%d|%d|%d|%s|%s|%s", 
   	                DR_ID, iDataSource, iInNetId, iServiceId, sServiceName, 
   									sErrMsg, sCreateTime);
    cout<<sLineBuf<<endl;									
   	vcheckAuthenticationError.push_back(string(sLineBuf));  	
   }
     
     m_poInfo->Close();
 */    
     ThreeLogGroup Log;
     vector<LogInfoData> vRet;  
     vRet.clear();
     //MBC_SYSTEM_START_ERROR需要调整
     Log.GetLoggInfo(vRet,GROUPCODE,MBC_HSS_AUTH_LOG,ocheckAuthenticationError.sStatBeginDate,ocheckAuthenticationError.sEndDate); 
     if(vRet.size())
     {
       for(int i = 0;i<vRet.size();i++)
       {
         memset(sLineBuf, 0, sizeof(sLineBuf));	 
         sprintf(sLineBuf, "%d|%d|%s|%s", 
   	                DR_ID,m_iProveNum,vRet[i].m_sInfo,vRet[i].m_sLogTime);
         cout<<sLineBuf<<endl;									
   	     vcheckAuthenticationError.push_back(string(sLineBuf));    	                
       }
     }
     
     ocheckAuthenticationError.writeJsonFile(vcheckAuthenticationError,SPLIT_FLAG);
     return 0;
}

/*
 *	函 数 名 : checkNoBillingCap
 *	函数功能 : 省内非计费域接口性能指标（AuditId=0101，Type=40),中频
 *	时    间 : 2011年7月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkNoBillingCap()
{
    cout <<"CheckStatInfo::checkNoBillingCap(AuditId=0101，Type=40,中频) ..."<<endl;
    
    StatInterface ocheckNoBillingCap("0101",40,MIDL_FREG);    
    vector<string> vcheckNoBillingCap;
    vcheckNoBillingCap.clear();
    vcheckNoBillingCap.push_back(CAPAB_TITLE);
    
    char sLineBuf[MaxLineLength] = {0};
    
    try{
    m_poInfo->Prepare("SELECT recv_num,rjct_num,fail_num "
                      " FROM   hss_balance_mess "
                      " where in_ne = 3004 "          //3004 省级crm 
                      " and balance_type = 0 "         //0  接收
                      " and sys_time >= :statBeginDate and sys_time < :statEndDate");

   m_poInfo->Commit();
   m_poInfo->setParam(1, atol(ocheckNoBillingCap.sStatBeginDate)/100);
   m_poInfo->setParam(2, atol(ocheckNoBillingCap.sEndDate)/100);
   m_poInfo->Execute();
   float fRecvNum = 0,fRjctNum = 0,fFailNum = 0;
   int iRecvNumSum = 0,iRjctNumSum = 0,iFailNumSum = 0;
   int iMinRecvNum = 0,iMaxRecvNum = 0,iMinRjctNum = 0,iMaxRjctNum = 0;
   int iMinFailNum = 0,iMaxFailNum = 0;
   float fAvgRecvNum = 0,fAvgRjctNum = 0,fAvgFailNum = 0;
   float fMinRjctNumPer = 0,fMaxRjctNumPer = 0,fAvgRjctNumPer = 0;
   float fMinFailNumPer = 0,fMaxFailNumPer = 0,fAvgFailNumPer = 0;
   int iCount = 0;
   while(!m_poInfo->FetchNext())
   {  
   	  iCount++;
   	  int iRecvNumTmp = 0,iRjctNumTmp = 0,iFailNumTmp = 0;
   	   m_poInfo->getColumn(1, &iRecvNumTmp);
       m_poInfo->getColumn(2, &iRjctNumTmp);
       m_poInfo->getColumn(3, &iFailNumTmp);
       fRecvNum = iRecvNumTmp;
       fRjctNum = iRjctNumTmp;
       fFailNum = iRjctNumTmp;
   	   if(iCount ==1 )
   	   {
   	   	 iMinRecvNum = iRecvNumTmp;
   	   	 iMaxRecvNum = iRecvNumTmp;
   	   	 iMinRjctNum = iRjctNumTmp;
   	   	 iMaxRjctNum = iRjctNumTmp;
   	   	 iMinFailNum = iFailNumTmp;
   	   	 iMaxFailNum = iFailNumTmp;
   	   	 fMinRjctNumPer = fRjctNum/fRecvNum;
   	   	 fMaxRjctNumPer = fRjctNum/fRecvNum;
   	   	 fMinFailNumPer = fFailNum/fRecvNum;
   	   	 fMaxFailNumPer = fFailNum/fRecvNum;
   	   }
   	   iMinRecvNum = (iRecvNumTmp < iMinRecvNum)?iRecvNumTmp:iMinRecvNum;
   	   iMaxRecvNum = (iRecvNumTmp > iMaxRecvNum)?iRecvNumTmp:iMaxRecvNum;
   	   iRecvNumSum += iRecvNumTmp;
   	   iMinRjctNum = (iRjctNumTmp < iMinRjctNum)?iRjctNumTmp:iMinRjctNum;
   	   iMaxRjctNum = (iRjctNumTmp > iMaxRjctNum)?iRjctNumTmp:iMaxRjctNum;
   	   iRjctNumSum += iRjctNumTmp;
   	   iMinFailNum = (iFailNumTmp < iMinFailNum)?iFailNumTmp:iMinFailNum;
   	   iMaxFailNum = (iFailNumTmp > iMaxFailNum)?iFailNumTmp:iMaxFailNum;
   	   iFailNumSum += iFailNumTmp;
   	   fMinRjctNumPer = (fRjctNum/fRecvNum < fMinRjctNumPer)?(fRjctNum/fRecvNum):fMinRjctNumPer;
   	   fMaxRjctNumPer = (fRjctNum/fRecvNum > fMaxRjctNumPer)?(fRjctNum/fRecvNum):fMaxRjctNumPer;
   	   fMinFailNumPer = (fFailNum/fRecvNum < fMinRjctNumPer)?(fFailNum/fRecvNum):fMinFailNumPer;
   	   fMaxFailNumPer = (fFailNum/fRecvNum > fMaxRjctNumPer)?(fFailNum/fRecvNum):fMaxFailNumPer;  	
    }
    if(iCount){
    	 fAvgRecvNum = ((float)iRecvNumSum)/iCount; 
       memset(sLineBuf,0,MaxLineLength);
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-01-01-001",
                         "接收总消息数",
                         "/HSS",
                         (float)iMinRecvNum,
                         fAvgRecvNum,
                         (float)iMaxRecvNum                      
                         );
       vcheckNoBillingCap.push_back(sLineBuf);

       fAvgRjctNum = ((float)iRjctNumSum)/iCount;
       memset(sLineBuf,0,MaxLineLength);
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-01-01-002",
                         "接收拒收消息包数",
                         "/HSS",
                         (float)iMinRjctNum,
                         fAvgRjctNum,
                         (float)iMaxRjctNum                       
                         );
       vcheckNoBillingCap.push_back(sLineBuf); 

       fAvgFailNum = ((float)iFailNumSum)/iCount;
       memset(sLineBuf,0,MaxLineLength);
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-01-01-003",
                         "接收处理失败消息包数",
                         "/HSS",
                         (float)iMinFailNum,
                         fAvgFailNum,
                         (float)iMaxFailNum                      
                         );
       vcheckNoBillingCap.push_back(sLineBuf);        
      
      
       fAvgRjctNumPer = ((float)iRjctNumSum)/iRecvNumSum;
       memset(sLineBuf,0,MaxLineLength);
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-01-01-004",
                         "接收拒收消息包比率",
                         "/HSS",
                         fMinRjctNumPer,
                         fAvgRjctNumPer,
                         fMaxRjctNumPer                   
                         );
       vcheckNoBillingCap.push_back(sLineBuf);        

       fAvgFailNumPer = ((float)iFailNumSum)/iRecvNumSum;
       memset(sLineBuf,0,MaxLineLength);
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-01-01-005",
                         "接收处理失败消息包比率",
                         "/HSS",
                         fMinFailNumPer,
                         fAvgFailNumPer,
                         fMaxFailNumPer                   
                         );
       vcheckNoBillingCap.push_back(sLineBuf);               
    }
    //判断是否取到数据，如果没取到则造手工数据
    if(vcheckNoBillingCap.size()==1)
    {
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-01-01-001","接收总消息数","/HSS",0.0,0.0,0.0);
        vcheckNoBillingCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-01-01-002","接收拒收消息包数","/HSS",0.0,0.0,0.0);
        vcheckNoBillingCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-01-01-003","接收处理失败消息包数","/HSS",0.0,0.0,0.0);
        vcheckNoBillingCap.push_back(sLineBuf);

        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-01-01-004","接收拒收消息包比率","/HSS",0.0,0.0,0.0);
        vcheckNoBillingCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-01-01-005","接收处理失败消息包比率","/HSS",0.0,0.0,0.0);
        vcheckNoBillingCap.push_back(sLineBuf);
    }
    
    m_poInfo->Close();
  }catch(TTStatus st) {
        m_poInfo->Close();
        printf("checkNoBillingCap failed! err_msg=%s", st.err_msg);
    }
    ocheckNoBillingCap.writeJsonFile(vcheckNoBillingCap,SPLIT_FLAG);      
    return 0;        	
}


/*
 *	函 数 名 : checkHssCap
 *	函数功能 : 集团HSS接口（AuditId=0101，Type=40),中频
 *	时    间 : 2011年7月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkHssCap()
{
    cout <<"CheckStatInfo::checkHssCap(AuditId=0102，Type=40,中频) ..."<<endl;
    
    StatInterface ocheckHssCap("0102",40,MIDL_FREG);    
    vector<string> vcheckHssCap;
    vcheckHssCap.clear();
    vcheckHssCap.push_back(CAPAB_TITLE);
    
    char sLineBuf[MaxLineLength] = {0};
    
    try{
    m_poInfo->Prepare("SELECT recv_num,rjct_num,fail_num "
                      " FROM   hss_balance_mess "
                      " where in_ne = 3061 "          //3061 集团HSS 
                      " and balance_type = 0 "         //0  接收
                      " and sys_time >= :statBeginDate and sys_time < :statEndDate");

   m_poInfo->Commit();
   m_poInfo->setParam(1, atol(ocheckHssCap.sStatBeginDate)/100);
   m_poInfo->setParam(2, atol(ocheckHssCap.sEndDate)/100);
   m_poInfo->Execute();
   float fRecvNum = 0,fRjctNum = 0,fFailNum = 0;
   int iRecvNumSum = 0,iRjctNumSum = 0,iFailNumSum = 0;
   int iMinRecvNum = 0,iMaxRecvNum = 0,iMinRjctNum = 0,iMaxRjctNum = 0;
   int iMinFailNum = 0,iMaxFailNum = 0;
   float fAvgRecvNum = 0,fAvgRjctNum = 0,fAvgFailNum = 0;
   float fMinRjctNumPer = 0,fMaxRjctNumPer = 0,fAvgRjctNumPer = 0;
   float fMinFailNumPer = 0,fMaxFailNumPer = 0,fAvgFailNumPer = 0;
   int iCount = 0;

   while(!m_poInfo->FetchNext())
   {
   	  iCount++;
   	  int iRecvNumTmp = 0,iRjctNumTmp = 0,iFailNumTmp = 0;
   	   m_poInfo->getColumn(1, &iRecvNumTmp);
       m_poInfo->getColumn(2, &iRjctNumTmp);
       m_poInfo->getColumn(3, &iFailNumTmp);
       fRecvNum = iRecvNumTmp;
       fRjctNum = iRjctNumTmp;
       fFailNum = iRjctNumTmp;
   	   if(iCount ==1 )
   	   {
   	   	 iMinRecvNum = iRecvNumTmp;
   	   	 iMaxRecvNum = iRecvNumTmp;
   	   	 iMinRjctNum = iRjctNumTmp;
   	   	 iMaxRjctNum = iRjctNumTmp;
   	   	 iMinFailNum = iFailNumTmp;
   	   	 iMaxFailNum = iFailNumTmp;
   	   	 fMinRjctNumPer = fRjctNum/fRecvNum;
   	   	 fMaxRjctNumPer = fRjctNum/fRecvNum;
   	   	 fMinFailNumPer = fFailNum/fRecvNum;
   	   	 fMaxFailNumPer = fFailNum/fRecvNum;
   	   }
   	   iMinRecvNum = (iRecvNumTmp < iMinRecvNum)?iRecvNumTmp:iMinRecvNum;
   	   iMaxRecvNum = (iRecvNumTmp > iMaxRecvNum)?iRecvNumTmp:iMaxRecvNum;
   	   iRecvNumSum += iRecvNumTmp;
   	   iMinRjctNum = (iRjctNumTmp < iMinRjctNum)?iRjctNumTmp:iMinRjctNum;
   	   iMaxRjctNum = (iRjctNumTmp > iMaxRjctNum)?iRjctNumTmp:iMaxRjctNum;
   	   iRjctNumSum += iRjctNumTmp;
   	   iMinFailNum = (iFailNumTmp < iMinFailNum)?iFailNumTmp:iMinFailNum;
   	   iMaxFailNum = (iFailNumTmp > iMaxFailNum)?iFailNumTmp:iMaxFailNum;
   	   iFailNumSum += iFailNumTmp;
   	   fMinRjctNumPer = (fRjctNum/fRecvNum < fMinRjctNumPer)?(fRjctNum/fRecvNum):fMinRjctNumPer;
   	   fMaxRjctNumPer = (fRjctNum/fRecvNum > fMaxRjctNumPer)?(fRjctNum/fRecvNum):fMaxRjctNumPer;
   	   fMinFailNumPer = (fFailNum/fRecvNum < fMinRjctNumPer)?(fFailNum/fRecvNum):fMinFailNumPer;
   	   fMaxFailNumPer = (fFailNum/fRecvNum > fMaxRjctNumPer)?(fFailNum/fRecvNum):fMaxFailNumPer;  	
    }  
   if(iCount){
   	   fAvgRecvNum = ((float)iRecvNumSum)/iCount;  	  
       memset(sLineBuf,0,MaxLineLength);     
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-01-02-001",
                         "接收总消息数",
                         "/HSS",
                         (float)iMinRecvNum,
                         fAvgRecvNum,
                         (float)iMaxRecvNum                    
                         );
       vcheckHssCap.push_back(sLineBuf);

       fAvgRjctNum = ((float)iRjctNumSum)/iCount;
       memset(sLineBuf,0,MaxLineLength);       
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-01-02-002",
                         "接收拒收消息包数",
                         "/HSS",
                         (float)iMinRjctNum,
                         fAvgRjctNum,
                         (float)iMaxRjctNum                      
                         );
       vcheckHssCap.push_back(sLineBuf); 

       fAvgFailNum = ((float)iFailNumSum)/iCount;
       memset(sLineBuf,0,MaxLineLength);    
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-01-02-003",
                         "接收处理失败消息包数",
                         "/HSS",
                         (float)iMinFailNum,
                         fAvgFailNum,
                         (float)iMaxFailNum                       
                         );
       vcheckHssCap.push_back(sLineBuf);        
      
       fAvgRjctNumPer = ((float)iRjctNumSum)/iRecvNumSum;
       memset(sLineBuf,0,MaxLineLength);      
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-01-02-004",
                         "接收拒收消息包比率",
                         "/HSS",
                         fMinRjctNumPer,
                         fAvgRjctNumPer,
                         fMaxRjctNumPer                      
                         );
       vcheckHssCap.push_back(sLineBuf);        
       
       fAvgFailNumPer = ((float)iFailNumSum)/iRecvNumSum;
       memset(sLineBuf,0,MaxLineLength);   
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-01-02-005",
                         "接收处理失败消息包比率",
                         "/HSS",
                         fMinFailNumPer,
                         fAvgFailNumPer,
                         fMaxFailNumPer                       
                         );
       vcheckHssCap.push_back(sLineBuf);               
    }
    //判断是否取到数据，如果没取到则造手工数据
    if(vcheckHssCap.size()==1)
    {
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-01-02-001","接收总消息数","/HSS",0.0,0.0,0.0);
        vcheckHssCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-01-02-002","接收拒收消息包数","/HSS",0.0,0.0,0.0);
        vcheckHssCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-01-02-003","接收处理失败消息包数","/HSS",0.0,0.0,0.0);
        vcheckHssCap.push_back(sLineBuf);

        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-01-02-004","接收拒收消息包比率","/HSS",0.0,0.0,0.0);
        vcheckHssCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-01-02-005","接收处理失败消息包比率","/HSS",0.0,0.0,0.0);
        vcheckHssCap.push_back(sLineBuf);
    }
    
    m_poInfo->Close();
  }catch(TTStatus st) {
        m_poInfo->Close();
        printf("checkHssCap failed! err_msg=%s", st.err_msg);
    }
    ocheckHssCap.writeJsonFile(vcheckHssCap,SPLIT_FLAG);   
    return 0;        	
}


/*
 *	函 数 名 : checkOfcsCap
 *	函数功能 : OFCS接口（AuditId=0201，Type=40),中频
 *	时    间 : 2011年7月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkOfcsCap()
{
    cout <<"CheckStatInfo::checkOfcsCap(AuditId=0201，Type=40,中频) ..."<<endl;
    
    StatInterface ocheckOfcsCap("0201",40,MIDL_FREG);    
    vector<string> vcheckOfcsCap;
    vcheckOfcsCap.clear();
    vcheckOfcsCap.push_back(CAPAB_TITLE);
    
    char sLineBuf[MaxLineLength] = {0};
    
    try{
    m_poInfo->Prepare("SELECT recv_num,rjct_num,fail_num "
                      " FROM   hss_balance_mess "
                      " where in_ne = 3002 "          //3002 省级离线计费 
                      " and balance_type = 3 "         //3  发布
                      " and sys_time >= :statBeginDate and sys_time < :statEndDate");

   m_poInfo->Commit();
   m_poInfo->setParam(1, atol(ocheckOfcsCap.sStatBeginDate)/100);
   m_poInfo->setParam(2, atol(ocheckOfcsCap.sEndDate)/100);
   m_poInfo->Execute();
   float fRecvNum = 0,fRjctNum = 0,fFailNum = 0;
   int iRecvNumSum = 0,iRjctNumSum = 0,iFailNumSum = 0;
   int iMinRecvNum = 0,iMaxRecvNum = 0,iMinRjctNum = 0,iMaxRjctNum = 0;
   int iMinFailNum = 0,iMaxFailNum = 0;
   float fAvgRecvNum = 0,fAvgRjctNum = 0,fAvgFailNum = 0;
   float fMinRjctNumPer = 0,fMaxRjctNumPer = 0,fAvgRjctNumPer = 0;
   float fMinFailNumPer = 0,fMaxFailNumPer = 0,fAvgFailNumPer = 0;
   int iCount = 0;   
   
   while(!m_poInfo->FetchNext())
   {   
      iCount++;
   	  int iRecvNumTmp = 0,iRjctNumTmp = 0,iFailNumTmp = 0;
   	   m_poInfo->getColumn(1, &iRecvNumTmp);
       m_poInfo->getColumn(2, &iRjctNumTmp);
       m_poInfo->getColumn(3, &iFailNumTmp);
       fRecvNum = iRecvNumTmp;
       fRjctNum = iRjctNumTmp;
       fFailNum = iRjctNumTmp;
   	   if(iCount ==1 )
   	   {
   	   	 iMinRecvNum = iRecvNumTmp;
   	   	 iMaxRecvNum = iRecvNumTmp;
   	   	 iMinRjctNum = iRjctNumTmp;
   	   	 iMaxRjctNum = iRjctNumTmp;
   	   	 iMinFailNum = iFailNumTmp;
   	   	 iMaxFailNum = iFailNumTmp;
   	   	 fMinRjctNumPer = fRjctNum/fRecvNum;
   	   	 fMaxRjctNumPer = fRjctNum/fRecvNum;
   	   	 fMinFailNumPer = fFailNum/fRecvNum;
   	   	 fMaxFailNumPer = fFailNum/fRecvNum;
   	   }
   	   iMinRecvNum = (iRecvNumTmp < iMinRecvNum)?iRecvNumTmp:iMinRecvNum;
   	   iMaxRecvNum = (iRecvNumTmp > iMaxRecvNum)?iRecvNumTmp:iMaxRecvNum;
   	   iRecvNumSum += iRecvNumTmp;
   	   iMinRjctNum = (iRjctNumTmp < iMinRjctNum)?iRjctNumTmp:iMinRjctNum;
   	   iMaxRjctNum = (iRjctNumTmp > iMaxRjctNum)?iRjctNumTmp:iMaxRjctNum;
   	   iRjctNumSum += iRjctNumTmp;
   	   iMinFailNum = (iFailNumTmp < iMinFailNum)?iFailNumTmp:iMinFailNum;
   	   iMaxFailNum = (iFailNumTmp > iMaxFailNum)?iFailNumTmp:iMaxFailNum;
   	   iFailNumSum += iFailNumTmp;
   	   fMinRjctNumPer = (fRjctNum/fRecvNum < fMinRjctNumPer)?(fRjctNum/fRecvNum):fMinRjctNumPer;
   	   fMaxRjctNumPer = (fRjctNum/fRecvNum > fMaxRjctNumPer)?(fRjctNum/fRecvNum):fMaxRjctNumPer;
   	   fMinFailNumPer = (fFailNum/fRecvNum < fMinRjctNumPer)?(fFailNum/fRecvNum):fMinFailNumPer;
   	   fMaxFailNumPer = (fFailNum/fRecvNum > fMaxRjctNumPer)?(fFailNum/fRecvNum):fMaxFailNumPer;  	            
   }
   if(iCount){
   	   fAvgRecvNum = ((float)iRecvNumSum)/iCount;
       memset(sLineBuf,0,MaxLineLength);
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-01-001",
                         "发布总消息数",
                         "/HSS",
                         (float)iMinRecvNum,
                         fAvgRecvNum,
                         (float)iMaxRecvNum                
                         );
       vcheckOfcsCap.push_back(sLineBuf);

       fAvgRjctNum = ((float)iRjctNumSum)/iCount;
       memset(sLineBuf,0,MaxLineLength);    
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-01-002",
                         "发布拒收消息包数",
                         "/HSS",
                         (float)iMinRjctNum,
                         fAvgRjctNum,
                         (float)iMaxRjctNum                     
                         );
       vcheckOfcsCap.push_back(sLineBuf); 

       fAvgFailNum = ((float)iFailNumSum)/iCount;
       memset(sLineBuf,0,MaxLineLength);    
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-01-003",
                         "发布处理失败消息包数",
                         "/HSS",
                         (float)iMinFailNum,
                         fAvgFailNum,
                         (float)iMaxFailNum                       
                         );
       vcheckOfcsCap.push_back(sLineBuf);        
      
       fAvgRjctNumPer = ((float)iRjctNumSum)/iRecvNumSum;
       memset(sLineBuf,0,MaxLineLength);       
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-01-004",
                         "发布拒收消息包比率",
                         "/HSS",
                         fMinRjctNumPer,
                         fAvgRjctNumPer,
                         fMaxRjctNumPer                       
                         );
       vcheckOfcsCap.push_back(sLineBuf);        

       fAvgFailNumPer = ((float)iFailNumSum)/iRecvNumSum;
       memset(sLineBuf,0,MaxLineLength);  
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-01-005",
                         "发布处理失败消息包比率",
                         "/HSS",
                         fMinFailNumPer,
                         fAvgFailNumPer,
                         fMaxFailNumPer                       
                         );
       vcheckOfcsCap.push_back(sLineBuf);               
    }
    //判断是否取到数据，如果没取到则造手工数据
    if(vcheckOfcsCap.size()==1)
    {
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-01-001","发布总消息数","/HSS",0.0,0.0,0.0);
        vcheckOfcsCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-01-002","发布拒收消息包数","/HSS",0.0,0.0,0.0);
        vcheckOfcsCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-01-003","发布处理失败消息包数","/HSS",0.0,0.0,0.0);
        vcheckOfcsCap.push_back(sLineBuf);

        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-01-004","发布拒收消息包比率","/HSS",0.0,0.0,0.0);
        vcheckOfcsCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-01-005","发布处理失败消息包比率","/HSS",0.0,0.0,0.0);
        vcheckOfcsCap.push_back(sLineBuf);
    }

    m_poInfo->Close();
  }catch(TTStatus st) {
        m_poInfo->Close();
        printf("checkOfcsCap failed! err_msg=%s", st.err_msg);
    }
    ocheckOfcsCap.writeJsonFile(vcheckOfcsCap,SPLIT_FLAG);    
    return 0;        	
}


/*
 *	函 数 名 : checkOcsCap
 *	函数功能 : OCS接口（AuditId=0202，Type=40),中频
 *	时    间 : 2011年7月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckStatInfo::checkOcsCap()
{
    cout <<"CheckStatInfo::checkOcsCap(AuditId=0202，Type=40,中频) ..."<<endl;
    
    StatInterface ocheckOcsCap("0202",40,MIDL_FREG);    
    vector<string> vcheckOcsCap;
    vcheckOcsCap.clear();
    vcheckOcsCap.push_back(CAPAB_TITLE);
    
    char sLineBuf[MaxLineLength] = {0};

    try{
    m_poInfo->Prepare("SELECT recv_num,rjct_num,fail_num "
                      " FROM   hss_balance_mess "
                      " where in_ne = 3003 "          //3003 省级在线计费 
                      " and balance_type = 3 "         //3  发布
                      " and sys_time >= :statBeginDate and sys_time < :statEndDate");

   m_poInfo->Commit();
   m_poInfo->setParam(1, atol(ocheckOcsCap.sStatBeginDate)/100);
   m_poInfo->setParam(2, atol(ocheckOcsCap.sEndDate)/100);
   m_poInfo->Execute();
   float fRecvNum = 0,fRjctNum = 0,fFailNum = 0;
   int iRecvNumSum = 0,iRjctNumSum = 0,iFailNumSum = 0;
   int iMinRecvNum = 0,iMaxRecvNum = 0,iMinRjctNum = 0,iMaxRjctNum = 0;
   int iMinFailNum = 0,iMaxFailNum = 0;
   float fAvgRecvNum = 0,fAvgRjctNum = 0,fAvgFailNum = 0;
   float fMinRjctNumPer = 0,fMaxRjctNumPer = 0,fAvgRjctNumPer = 0;
   float fMinFailNumPer = 0,fMaxFailNumPer = 0,fAvgFailNumPer = 0;
   int iCount = 0;   
   
   while(!m_poInfo->FetchNext())
   {
   	  iCount++;
   	  int iRecvNumTmp = 0,iRjctNumTmp = 0,iFailNumTmp = 0;
   	   m_poInfo->getColumn(1, &iRecvNumTmp);
       m_poInfo->getColumn(2, &iRjctNumTmp);
       m_poInfo->getColumn(3, &iFailNumTmp);
       fRecvNum = iRecvNumTmp;
       fRjctNum = iRjctNumTmp;
       fFailNum = iRjctNumTmp;
   	   if(iCount ==1 )
   	   {
   	   	 iMinRecvNum = iRecvNumTmp;
   	   	 iMaxRecvNum = iRecvNumTmp;
   	   	 iMinRjctNum = iRjctNumTmp;
   	   	 iMaxRjctNum = iRjctNumTmp;
   	   	 iMinFailNum = iFailNumTmp;
   	   	 iMaxFailNum = iFailNumTmp;
   	   	 fMinRjctNumPer = fRjctNum/fRecvNum;
   	   	 fMaxRjctNumPer = fRjctNum/fRecvNum;
   	   	 fMinFailNumPer = fFailNum/fRecvNum;
   	   	 fMaxFailNumPer = fFailNum/fRecvNum;
   	   }
   	   iMinRecvNum = (iRecvNumTmp < iMinRecvNum)?iRecvNumTmp:iMinRecvNum;
   	   iMaxRecvNum = (iRecvNumTmp > iMaxRecvNum)?iRecvNumTmp:iMaxRecvNum;
   	   iRecvNumSum += iRecvNumTmp;
   	   iMinRjctNum = (iRjctNumTmp < iMinRjctNum)?iRjctNumTmp:iMinRjctNum;
   	   iMaxRjctNum = (iRjctNumTmp > iMaxRjctNum)?iRjctNumTmp:iMaxRjctNum;
   	   iRjctNumSum += iRjctNumTmp;
   	   iMinFailNum = (iFailNumTmp < iMinFailNum)?iFailNumTmp:iMinFailNum;
   	   iMaxFailNum = (iFailNumTmp > iMaxFailNum)?iFailNumTmp:iMaxFailNum;
   	   iFailNumSum += iFailNumTmp;
   	   fMinRjctNumPer = (fRjctNum/fRecvNum < fMinRjctNumPer)?(fRjctNum/fRecvNum):fMinRjctNumPer;
   	   fMaxRjctNumPer = (fRjctNum/fRecvNum > fMaxRjctNumPer)?(fRjctNum/fRecvNum):fMaxRjctNumPer;
   	   fMinFailNumPer = (fFailNum/fRecvNum < fMinRjctNumPer)?(fFailNum/fRecvNum):fMinFailNumPer;
   	   fMaxFailNumPer = (fFailNum/fRecvNum > fMaxRjctNumPer)?(fFailNum/fRecvNum):fMaxFailNumPer;  	
    }   	
     if(iCount){
     	 fAvgRecvNum = ((float)iRecvNumSum)/iCount; 
     	 memset(sLineBuf,0,MaxLineLength);        
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-02-001",
                         "发布总消息数",
                         "/HSS",
                         (float)iMinRecvNum,
                         fAvgRecvNum,
                         (float)iMaxRecvNum                      
                         );
       vcheckOcsCap.push_back(sLineBuf);

       fAvgRjctNum = ((float)iRjctNumSum)/iCount;
       memset(sLineBuf,0,MaxLineLength);        
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-02-002",
                         "发布拒收消息包数",
                         "/HSS",
                         (float)iMinRjctNum,
                         fAvgRjctNum,
                         (float)iMaxRjctNum                      
                         );
       vcheckOcsCap.push_back(sLineBuf); 

       fAvgFailNum = ((float)iFailNumSum)/iCount;
       memset(sLineBuf,0,MaxLineLength);      
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-02-003",
                         "发布处理失败消息包数",
                         "/HSS",
                         (float)iMinFailNum,
                         fAvgFailNum,
                         (float)iMaxFailNum                     
                         );
       vcheckOcsCap.push_back(sLineBuf);        
      
       fAvgRjctNumPer = ((float)iRjctNumSum)/iRecvNumSum;
       memset(sLineBuf,0,MaxLineLength);       
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-02-004",
                         "发布拒收消息包比率",
                         "/HSS",
                         fMinRjctNumPer,
                         fAvgRjctNumPer,
                         fMaxRjctNumPer                       
                         );
       vcheckOcsCap.push_back(sLineBuf);        

       fAvgFailNumPer = ((float)iFailNumSum)/iRecvNumSum;
       memset(sLineBuf,0,MaxLineLength);        
       sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f", 
                         "40-30-12-02-02-005",
                         "发布处理失败消息包比率",
                         "/HSS",
                         fMinFailNumPer,
                         fAvgFailNumPer,
                         fMaxFailNumPer              
                         );
       vcheckOcsCap.push_back(sLineBuf);               
    }
    //判断是否取到数据，如果没取到则造手工数据
    if(vcheckOcsCap.size()==1)
    {
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-02-001","发布总消息数","/HSS",0.0,0.0,0.0);
        vcheckOcsCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-02-002","发布拒收消息包数","/HSS",0.0,0.0,0.0);
        vcheckOcsCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-02-003","发布处理失败消息包数","/HSS",0.0,0.0,0.0);
        vcheckOcsCap.push_back(sLineBuf);

        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf,"%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-02-004","发布拒收消息包比率","/HSS",0.0,0.0,0.0);
        vcheckOcsCap.push_back(sLineBuf);
        memset(sLineBuf,0,MaxLineLength);
        sprintf(sLineBuf, "%s|%s|%s||%.3f|%.3f|%.3f","40-30-12-02-02-005","发布处理失败消息包比率","/HSS",0.0,0.0,0.0);
        vcheckOcsCap.push_back(sLineBuf);
    }

    m_poInfo->Close();
  }catch(TTStatus st) {
        m_poInfo->Close();
        printf("checkOcsCap failed! err_msg=%s", st.err_msg);
    }
    ocheckOcsCap.writeJsonFile(vcheckOcsCap,SPLIT_FLAG);       
    return 0;   
    
}
      	
