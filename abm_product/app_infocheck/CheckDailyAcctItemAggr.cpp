#include "CheckDailyAcctItemAggr.h"
#include "DataIncrementMgr.h"


const char* DEFAULT_PRE_BACKUP_TIME = "20000101000000";
const char* BILL_INCLUDE_ITEM_SOURCE_LIST = "11, 12, 13";//14
const char * BILL_TABLE_TYPE_LIST = "1 , 2, 3, 4, 21, 22, 23, 24, 51";

const int TICKET_FEE = 1;//	详单费用
const int CYCLE_FEE = 2;//	周期性费用
const int VALUE_ADD_FEE = 3;//	代收费用
const int ONCE_TIME_FEE = 4;//	一次性费用
const int OFFSET_FEE = 5;//	补收抵减
const int DISCT_FEE = 6;//	帐务优惠费用
const int OTHER_FEE = 7;//	其他费用


//int CheckDailyAcctItemAggr::m_iBillingCycleID = 0;   
//char CheckDailyAcctItemAggr::m_sCheckDate[16] = {0};	
 
CheckDailyAcctItemAggr::CheckDailyAcctItemAggr()
{
	Environment::useUserInfo();
	memset(m_sThisBackupTime, 0, sizeof(m_sThisBackupTime));
	memset(m_sPreBackupTime, 0, sizeof(m_sPreBackupTime));
	memset(m_sAcctItemDate, 0, sizeof(m_sAcctItemDate));
	memset(m_sPreAcctItemDate, 0, sizeof(m_sPreAcctItemDate));
	m_bStatType = false;
	m_pOrgMgrEx = OrgMgrEx::GetInstance();
		
   	memset(m_sDefaultAreaCode, 0, sizeof(m_sDefaultAreaCode));
   	memset(m_sCheckDate, 0, sizeof(m_sCheckDate));
    m_iBillingCycleID = 0;
#ifdef DEF_JIANGSU
    char sValue[1001] = {0};
    if (ParamDefineMgr::getParam("CHECK_STAT", "CITY_CODE", sValue, -1))
    {
    	strncpy(m_sDefaultAreaCode, sValue, 4);    	
    }else
    {
    	strncpy(m_sDefaultAreaCode, StatInterface::m_sDefaultAreaCode, 4);    	
    }
    
	char sUser[32] = {0};
	char sPwd[32] = {0};
	char sStr[32] = {0};	
    getConnectInfo("STAT",sUser,sPwd,sStr);
   	m_TOCITicketDBConn.connect(sUser,sPwd,sStr);
    
#else    
	strncpy(m_sDefaultAreaCode, StatInterface::m_sDefaultAreaCode, 4);


    memset(m_sProveCode,0,sizeof(m_sProveCode));
    char sValue[4+1]={0};    
   	 if (!ParamDefineMgr::getParam("CHECK_STAT", "PROVINCE_CODE", sValue, -1))
     {
    	 strcpy(sValue,"0");
     }    
   	 strncpy(m_sProveCode, sValue, 4); 
   	 m_iProveNum=atoi(m_sProveCode);
   	 
 #endif
   	 		
}	

void CheckDailyAcctItemAggr::getConnectInfo(char const * sIniHeader,char *sUserName,char *sPasswd,char *sConnStr)
{
	ReadIni reader;
	char sUser[32] = {0};
	char sPwd[32] = {0};
	char sStr[32] = {0};
	char sFile[254] = {0};
    char sTIBS_HOME[80] = {0};
		
	char *p =  NULL; 
		
	/* TIBS_HOME */
	if ((p=getenv ("TIBS_HOME")) == NULL)
		sprintf (sTIBS_HOME, "/opt/opthb");
	else 
		sprintf (sTIBS_HOME, "%s", p);
	
	if (sTIBS_HOME[strlen(sTIBS_HOME)-1] == '/') {
    	//sprintf (sFile, "%sconfig/data_access.ini", m_sTIBS_HOME);
		sprintf (sFile, "%setc/data_access_new.ini", sTIBS_HOME);
	} else {
		//sprintf (sFile, "%s/config/data_access.ini", m_sTIBS_HOME);
		sprintf (sFile, "%s/etc/data_access_new.ini", sTIBS_HOME);
	}

	reader.readIniString (sFile, sIniHeader, "username", sUser, (char *)"");
//	reader.readIniString (sFile, sIniHeader, "password", sPwd, "");
	
	char sTemp[254] = {0};	
	reader.readIniString (sFile, sIniHeader, "password", sTemp,(char *)"");
	decode(sTemp,sPwd);
	
	reader.readIniString (sFile, sIniHeader, "connstr", sStr, (char *)"");
	
	strcpy(sUserName,sUser);
	strcpy(sPasswd,sPwd);
	strcpy(sConnStr,sStr);
		
	return;	
}

CheckDailyAcctItemAggr::~CheckDailyAcctItemAggr()
{
	if (!m_oBackupTableNames.empty())
	{
		m_oBackupTableNames.clear();
	}	
	
	if (!m_oBillTableNames.empty())
	{
		m_oBillTableNames.clear();
	}	
	
	if (!m_oTicketTableNames.empty())
	{
		m_oTicketTableNames.clear();
	}	
	
	if (!m_vAcctTableNames.empty())
	{
		m_vAcctTableNames.clear();
	}		
	
	if (!m_poStatInfoBuf.empty())
	{
    	vector<StatResultInfo *> ::iterator iter;
    	for(iter = m_poStatInfoBuf.begin(); iter != m_poStatInfoBuf.end(); ++iter)	
    	{
    		delete (*iter);    		
    	}
		m_poStatInfoBuf.clear();
	}
	
	if (!m_mapDailyAggr.empty())
	{
    	map<string, StatResultInfo*>::iterator iter;
    	for(iter = m_mapDailyAggr.begin(); iter != m_mapDailyAggr.end(); ++iter)	
    	{
    		delete iter->second;
    	}
		m_mapDailyAggr.clear();
	}	
	
	if (!m_mapMonthAggr.empty())
	{
    	for(m_mIter = m_mapMonthAggr.begin(); m_mIter != m_mapMonthAggr.end(); ++m_mIter)	
    	{
    		delete m_mIter->second;
    	}
		m_mapMonthAggr.clear();
	}		
	
	m_pOrgMgrEx->ExitInstance();
}	

bool CheckDailyAcctItemAggr::setStatType()
{
    char sValue[1001];
    int iLength = -1;      
    if (ParamDefineMgr::getParam("CHECK_DAILY_ACCT", "STAT_TYPE", sValue, iLength))
    {	
        long lTemp = strtol(sValue, 0, 10);
        if (lTemp >= 1)
        {        	
            m_bStatType = true;   
            Log::log(0, "所有数据采用全量稽核方式");
        }	
        else
        {
            Log::log(0, "非周期性清单表数据采用增量稽核方式");        	
        }	
        return true;	        	
    }
   
    m_bStatType = false;
    Log::log(0, "非周期性清单表数据采用增量稽核方式,请确保所有清单表create_date建立所引");        	
    
	return true;//不设置stat_type参数也不退出
   	//return false;	
}	

bool CheckDailyAcctItemAggr::setBillingCycleID()
{
    BillingCycleMgr bcm;
    //Date d;                                                      
    //d.addDay(-1);                                                
    BillingCycle *bc = bcm.getOccurBillingCycle(1, StatInterface::m_pPlan->m_sDataEndTime);
    
    if (!bc)
    {    
        return false;
    }
    
    m_iBillingCycleID = bc->getBillingCycleID();
    
    //m_iBillingCycleID = 200708;//测试
    
    return true;    	
}

bool CheckDailyAcctItemAggr::setBillingCycleID(const char* sDate)
{
    BillingCycleMgr bcm;
                                             
    BillingCycle *bc = bcm.getOccurBillingCycle(1, (char *)sDate);
    
    if (!bc)
    {    
        return false;
    }
    
    m_iBillingCycleID = bc->getBillingCycleID();
       
    return true;    	
}

bool CheckDailyAcctItemAggr::initParam()
{	
    if (!setBillingCycleID())
    {
        Log::log(0, "帐期未能获取");
    	return false;
    }	
    
    if (!setStatType())
    {
    	Log::log(0, "统计方式未能获取,检查b_param_define表");
    	return false;
    }

#ifdef DEF_JIANGSU
	getBillTables(m_TOCITicketDBConn);
#else    
    getBillTables();    
#endif    

    if (m_oBillTableNames.empty())
    {
        Log::log(0, "%d帐期清单表名未能获取,检查b_inst_table_list_cfg", m_iBillingCycleID);    	    	
    	return false;
    }		
    
    return true;    
}

bool CheckDailyAcctItemAggr::getCheckDate(char *sAcctItemDate, char *sLastBackupTime)
{
	if (!sAcctItemDate||!sLastBackupTime)
		return false;		
		
	strcpy(m_sThisBackupTime, sLastBackupTime);
	strcpy(m_sAcctItemDate, sAcctItemDate);	
	
    getBackupTables();    
    if (m_oBackupTableNames.empty())
    {
        Log::log(0, "%s日账备份表名未能获取", sAcctItemDate);    	    	
    	return false;
    }	
    	
    Log::log(0, "开始稽核%s日日账", m_sAcctItemDate);
	
    getPreBackupTime();
    
    Date d;
    d.getTimeString(m_sCheckDate);
    
	if (!m_poStatInfoBuf.empty())//出现连续统计2日的情况 清除前一日的数据
	{
    	vector<StatResultInfo *> ::iterator iter;
    	for (iter = m_poStatInfoBuf.begin(); iter != m_poStatInfoBuf.end(); ++iter)	
    	{
    		delete (*iter);
    	}
		m_poStatInfoBuf.clear();
	}	    
        
    return true;    
}

void CheckDailyAcctItemAggr::getPreBackupTime()
{
	DEFINE_QUERY(qry);
	char sSql[1024];
    sprintf(sSql, " SELECT nvl(MAX(t.acct_item_date), 0), " 
                 "        nvl(to_char(MAX(t.backup_time), 'yyyymmddhh24miss'), '%s') " 
                 " FROM   b_daily_acct_item_log t "
                 " WHERE  t.state = 'END' "
                 " AND    t.check_flag = 1 ", DEFAULT_PRE_BACKUP_TIME);
    qry.setSQL(sSql);
    qry.open();	
    if (qry.next())
    {	
        strcpy(m_sPreAcctItemDate, qry.field(0).asString());    
        strcpy(m_sPreBackupTime, qry.field(1).asString());
    }
    qry.close();
    
    if (atoi(m_sPreAcctItemDate) != 0)
    {
    	BillingCycleMgr bcm;
    	BillingCycle *bc = bcm.getOccurBillingCycle(1, m_sPreAcctItemDate);
    	if (bc->getBillingCycleID() != m_iBillingCycleID)
    	{
    		strcpy(m_sPreAcctItemDate, "0");
    		strcpy(m_sPreBackupTime, DEFAULT_PRE_BACKUP_TIME);
    	}
    	else
    	{	
    	    Log::log(0, "前次成功稽核日账日期为：%s", m_sPreAcctItemDate);	
    	}    	
    }	
}	

void CheckDailyAcctItemAggr::getBackupTables()
{
	if (!m_oBackupTableNames.empty())
	{
		m_oBackupTableNames.clear();
	}		
    DEFINE_QUERY(qry);
    char sSql[1024];
    memset(sSql, 0, sizeof (sSql));    	    
    sprintf(sSql, " SELECT DISTINCT (b.dest_tablename) "
                 " FROM b_inst_table_list_cfg a, "
                 "      b_backup_tablename_relation b "
                 " WHERE a.billing_cycle_id = %d "
                 " AND a.table_type = %d "
                 " AND b.backup_date = '%s' "
                 " AND upper(a.table_name) = upper(b.src_tablename) ",
                 m_iBillingCycleID, TABLE_TYPE_ACCT_ITEM_AGGR, m_sAcctItemDate);
    qry.setSQL(sSql);
    qry.open();	
    while (qry.next())
    {
    	TableInfo m_poTableInfo;    
        strcpy(m_poTableInfo.m_sTableName, qry.field(0).asString());
        m_oBackupTableNames.push_back(m_poTableInfo);    	
    }
    qry.close();
}	

void CheckDailyAcctItemAggr::getBillTables()
{
    DEFINE_QUERY(qry);
    char sSql[1024] = {0};
    
    sprintf(sSql," SELECT DISTINCT upper(table_name), table_type "
                " FROM b_inst_table_list_cfg t "
                " WHERE t.billing_cycle_id = :billing_cycle_id "
                " AND t.table_type in (%s) ",BILL_TABLE_TYPE_LIST);

    qry.setSQL(sSql);                
    qry.setParameter("billing_cycle_id", m_iBillingCycleID);

    qry.open();
    while (qry.next())
    {
    	TableInfo oTableInfo;    
        strcpy(oTableInfo.m_sTableName, qry.field(0).asString());
        oTableInfo.m_iTableType = qry.field(1).asInteger();
        m_oBillTableNames.push_back(oTableInfo);
    }
    qry.close();	
}	

void CheckDailyAcctItemAggr::getBillTables(TOCIDatabase &oTOCIDatabase)
{
	TOCIQuery qry(&oTOCIDatabase);
	
   /* char sSql[1024] = {0};
    
    sprintf(sSql," SELECT DISTINCT upper(table_name), table_type "
                " FROM b_inst_table_list_cfg t "
                " WHERE t.billing_cycle_id = :billing_cycle_id "
                " AND t.table_type in (1 , 2, 3, 21, 22, 23, 24) ");*/

    qry.setSQL(" select DISTINCT upper(table_name), sub_class_id "
			   " from s_table_def "
 			   " where billing_cycle_id = :billing_cycle_id "
 			   " and sub_class_id in(1, 2 ,3, 5, 43 ,44 ,45, 46) ");
 				 
    qry.setParameter("billing_cycle_id", m_iBillingCycleID);

    qry.open();
    while (qry.next())
    {
    	TableInfo oTableInfo;    
        strcpy(oTableInfo.m_sTableName, qry.field(0).asString());
        oTableInfo.m_iTableType = qry.field(1).asInteger();
        m_oTicketTableNames.push_back(oTableInfo);
    }
    qry.close();	
    
	DEFINE_QUERY (qry1);
	
    char sSql1[1024] = {0};
    
    sprintf(sSql1," SELECT DISTINCT upper(table_name), table_type "
                " FROM b_inst_table_list_cfg t "
                " WHERE t.billing_cycle_id = :billing_cycle_id "
                " AND t.table_type in (51,4) ");

    qry1.setSQL(sSql1);                
    qry1.setParameter("billing_cycle_id", m_iBillingCycleID);

    qry1.open();
    while (qry1.next())
    {
    	TableInfo oTableInfo;    
        strcpy(oTableInfo.m_sTableName, qry1.field(0).asString());
        oTableInfo.m_iTableType = qry1.field(1).asInteger();
        m_oBillTableNames.push_back(oTableInfo);
    }
    qry1.close();    
}

void CheckDailyAcctItemAggr::getAcctTables()
{
    DEFINE_QUERY(qry);
    char sSql[1024] = {0};
    
    sprintf(sSql," SELECT DISTINCT upper(table_name), table_type "
                " FROM b_inst_table_list_cfg t "
                " WHERE t.billing_cycle_id = :billing_cycle_id "
                " AND t.table_type in (%d) ",TABLE_TYPE_ACCT_ITEM_AGGR);
    qry.setSQL(sSql);
                
    qry.setParameter("billing_cycle_id", m_iBillingCycleID);
    qry.open();
    while (qry.next())
    {
    	TableInfo oTableInfo;    
        strcpy(oTableInfo.m_sTableName, qry.field(0).asString());
        oTableInfo.m_iTableType = qry.field(1).asInteger();
        m_vAcctTableNames.push_back(oTableInfo);
    }
    qry.close();	
}	

void CheckDailyAcctItemAggr::getAcctTables(TOCIDatabase &oTOCIDatabase)
{
	TOCIQuery qry(&oTOCIDatabase);
    char sSql[1024] = {0};
    
    sprintf(sSql," SELECT DISTINCT upper(table_name), table_type "
                " FROM b_inst_table_list_cfg t "
                " WHERE t.billing_cycle_id = :billing_cycle_id "
                " AND t.table_type in (%d) ",TABLE_TYPE_ACCT_ITEM_AGGR);
    qry.setSQL(sSql);
                
    qry.setParameter("billing_cycle_id", m_iBillingCycleID);
    qry.open();
    while (qry.next())
    {
    	TableInfo oTableInfo;    
        strcpy(oTableInfo.m_sTableName, qry.field(0).asString());
        oTableInfo.m_iTableType = qry.field(1).asInteger();
        m_vAcctTableNames.push_back(oTableInfo);
    }
    qry.close();	
}

/*void CheckDailyAcctItemAggr::loadAcctItemGroups()
{
	DEFINE_QUERY(qry);
	int iAcctItemGroupID = 0;
	int iAcctItemNumber = 1;
	
	qry.setSQL(" SELECT acct_item_group_id FROM b_check_daily_acct_item_group "
	           " WHERE acct_item_group_id <> -1 ");
	qry.open();
	while (qry.next())
	{
		iAcctItemGroupID = qry.field(0).asInteger();
		oAcctItemMgr.getAcctItemsB(iAcctItemGroupID, &iAcctItemNumber);
		if (iAcctItemNumber == 0)
		{
			Log::log(0, "账目组:%d没有账目配置,不列为归并项", iAcctItemGroupID);
		}	
		else
		{
		    m_oAcctItemGroups.push_back(iAcctItemGroupID);
		}	
	}
	qry.close();
}	           

bool CheckDailyAcctItemAggr::checkAcctItemGroups()
{
	vector<int *> vct;
    vector<int >::iterator iter;
    for (iter = m_oAcctItemGroups.begin(); iter != m_oAcctItemGroups.end(); iter++)	
    {
    	int *poAcctItemID = new int[MAX_ACCTITEM_PERGROUP];	
    	int *p = oAcctItemMgr.getAcctItemsB((*iter));
    	int i = 0;
        while (p[i] != 0 )
        {
            poAcctItemID[i] = p[i];
            i++;
        }       	
    	vct.push_back(poAcctItemID);
    }	
    
    vector<int *>::iterator iter1;
    vector<int *>::iterator iter2;  
    
    for (iter1 = vct.begin(); iter1 != vct.end(); ++iter1)	 	
    {
    	vector<int *>::iterator iter3 = vct.end(); 
    	for(iter2 = --iter3; iter2 != iter1; --iter2)
    	{
    		for(int i = 0; (*iter2)[i] != -1; i++ )
    		{
    			for(int j = 0; (*iter1)[j] != -1; j++)
    			{
    				if((*iter2)[i] == (*iter1)[j])
    				{
    					Log::log(0, "%d属于2个归并账目,会导致日账数据出现差异",(*iter2)[i]);
   					    for(iter1 = vct.begin(); iter1 != vct.end(); iter1++)	 	
   					    {
   					    	delete[] (*iter1);
   					    }
	
    					vct.clear();
    					return false;	
    				}	
    			}
    		}
    	}
    } 
    
    for (iter1 = vct.begin(); iter1 != vct.end(); ++iter1)	 	
   	{
   		delete[] (*iter1);   		
   	}    
    vct.clear();
    
    return true;
    	
}*/	

void CheckDailyAcctItemAggr::setCheckFlagEnd()
{
    DEFINE_QUERY(qry);
    char sSql[1024];
    memset(sSql, 0, sizeof (sSql));    	    
    sprintf(sSql, " UPDATE b_daily_acct_item_log t "
                 " SET t.check_flag = 1, "
                 "     t.check_time = to_date(%s, 'yyyymmddhh24miss')"
                 " WHERE t.acct_item_date = '%s' ",
                 m_sCheckDate, m_sAcctItemDate);
    qry.setSQL(sSql);
    qry.execute();
    //qry.commit();
}
/*
int CheckDailyAcctItemAggr::getStatType(const char *sTableName)	
{
	DEFINE_QUERY(qry);
	char sSql[2048];
	memset(sSql, 0, sizeof(sSql));
	
	int iAcctItemTypeID = 0;
	int iBelongGroupID1 = -1;
	int iBelongGroupID2 = -1;
	memset(sSql, 0, sizeof(sSql));
	           sTableName, sTableName, sTableName, sTableName, sTableName, sTableName);
    qry.setSQL(sSql);
    qry.open();
    while (qry.next())
    {
        iAcctItemTypeID = qry.field(0).asInteger();
        vector<int >::iterator iter;
        	
        for (iter = m_oAcctItemGroups.begin(); iter != m_oAcctItemGroups.end(); ++iter)	
        {
        	if (oAcctItemMgr.getBelongGroupB(iAcctItemTypeID, (*iter)))
        	{
        		iBelongGroupID1 = (*iter);
        	    if (iBelongGroupID2 == -1)
        	    {
        	    	iBelongGroupID2 = iBelongGroupID1;
        	    }
        	    else
        	    {
        	    	if (iBelongGroupID2 != iBelongGroupID1)
        	    	{
        	    		qry.close();
        	    		return -100 ;
        	    	}	
        	    }
        	}	
        }
    }        
    qry.close();    
    return iBelongGroupID2;                           
}	

int CheckDailyAcctItemAggr::getBelong(int iAcctItemID)
{
	vector<int >::iterator iter;
	for (iter = m_oAcctItemGroups.begin(); iter != m_oAcctItemGroups.end(); ++iter)	
	{
		if (G_PACCTITEMMGR->getBelongGroupB(iAcctItemID, (*iter)))
	    {
	    	return (*iter);
	    }
	}
    return -1;
}	*/

int CheckDailyAcctItemAggr::statBillCharge()
{
	vector<TableInfo >::iterator iter;

	DEFINE_QUERY(qry);
	char sSql[1024];
	char sKey[50];
	char sTableName[51];   
	memset(sTableName, 0, sizeof (sTableName));
	memset(sSql, 0, sizeof (sSql));
	memset(sKey, 0, sizeof (sKey));
		
	int iTableType = 0;	
	int iAcctItemGroupID = 0;
	long lCharge = 0;
    KeyTree<StatResultInfo *> m_oStatInfoTree;	
	StatResultInfo *poStatResultInfo;    		
	
	StatMergeAcctItemMgr oStatMergeAcctItemMgr((char*)"JF_TJ_XTJK_RK_6");
	
	int iTotalOrInrement = 0;//0:增量 1 全量
	int iBillOrAcct = 0; //0:清单费用 1 帐单费用
	
	for (iter = m_oBillTableNames.begin(); iter != m_oBillTableNames.end(); iter++)
	{ 
		strcpy(sTableName, (*iter).m_sTableName);
		iTableType = (*iter).m_iTableType;
		Log::log(0, "统计表%s费用", sTableName);

		if (!m_bStatType&&(m_sPreAcctItemDate[0] != '0')&&
			(iTableType == EVENT_TABLE_TYPE_CALL_EVENT||
			 iTableType == EVENT_TABLE_TYPE_DATA_EVENT||
			 iTableType == EVENT_TABLE_TYPE_VALUE_ADDED_EVENT))//增量
		{				    	
		    sprintf(sSql, " SELECT /*+ parallel (t,10)*/ "
		                  " t.charge1, t.acct_item_type_id1, "
		                  " t.charge2, t.acct_item_type_id2, "		                  
		                  " t.charge3, t.acct_item_type_id3, "		                  
		                  " t.charge4, t.acct_item_type_id4, "		                  
		                  " t.charge5, t.acct_item_type_id5, "		                  
		                  " t.charge6, t.acct_item_type_id6 "
		                  " FROM %s t "
                          " WHERE t.created_date BETWEEN to_Date(%s, 'yyyymmddhh24miss') + 1/(24*3600) "
                                               " AND to_Date(%s, 'yyyymmddhh24miss') ",
                           sTableName, m_sPreBackupTime, m_sThisBackupTime);				    					    	                                   
		}	  
		else
		{	             
		    sprintf(sSql, " SELECT /*+ parallel (t,10)*/ "
		                  " t.charge1, t.acct_item_type_id1, "
		                  " t.charge2, t.acct_item_type_id2, "		                  
		                  " t.charge3, t.acct_item_type_id3, "		                  
		                  " t.charge4, t.acct_item_type_id4, "		                  
		                  " t.charge5, t.acct_item_type_id5, "		                  
		                  " t.charge6, t.acct_item_type_id6 "		                  
		                  " FROM %s t "
                          " WHERE t.created_date <= to_Date(%s, 'yyyymmddhh24miss') ",
                           sTableName, m_sThisBackupTime);
            iTotalOrInrement = 1;                                    
        }   
        qry.setSQL(sSql);
        qry.open();
        while (qry.next())              
        {
        	for (int i = 0; i<6; i++)
        	{
        		lCharge = qry.field(i*2).asLong() ;
        		if (lCharge == 0)
        		{
        			continue;
        		}	
        		
        	    iAcctItemGroupID = oStatMergeAcctItemMgr.getMergeItemIDByAcctItem(qry.field(i*2+1).asInteger());
        	    
        	    //将费用归并到对应的账目组
                sprintf(sKey,"%d-%d-%d", iAcctItemGroupID, iTotalOrInrement, iBillOrAcct); 
                if (!m_oStatInfoTree.get(sKey, &poStatResultInfo))
                {            	
                    poStatResultInfo = addStatInfoNode(iAcctItemGroupID, iTotalOrInrement, iBillOrAcct);
                    m_oStatInfoTree.add(sKey, poStatResultInfo);   
                }       
                poStatResultInfo->m_lCharge += lCharge;                	
            }
        }
        qry.close();                           
		//if(iTableType == EVENT_TABLE_TYPE_CALL_EVENT||
		//   (getStatType(sTableName) == -100))
		//{
		//	for(int i = 1; i<=6; i++)
		//	{
		//		if(!m_bStatType&&(m_sPreAcctItemDate[0] != '0')&&
		//			(iTableType == EVENT_TABLE_TYPE_CALL_EVENT||
		//			 iTableType == EVENT_TABLE_TYPE_DATA_EVENT||
		//			 iTableType == EVENT_TABLE_TYPE_VALUE_ADDED_EVENT))//增量
		//		{				    	
		//		    sprintf(sSql, " SELECT /*+ parallel (t,10)*/t.charge%d, t.acct_item_type_id%d "
		//		                  " FROM %s t "
        //                          " WHERE t.charge%d<>0 "
        //                          " AND t.created_date BETWEEN to_Date(%s-1, 'yyyymmddhh24miss') AND to_Date(%s, 'yyyymmddhh24miss') ",
        //                           i, i, sTableName, i, m_sPreBackupTime, m_sThisBackupTime);				    					    	                                   
		//		}	  
		//		else
		//		{	             
		//		    sprintf(sSql, " SELECT /*+ parallel (t,10)*/t.charge%d, t.acct_item_type_id%d "
		//		                  " FROM %s t "
        //                          " WHERE charge%d<>0 "
        //                          " and t.created_date <= to_Date(%s, 'yyyymmddhh24miss') ",
        //                           i, i, sTableName, i, m_sThisBackupTime);
        //            iTotalOrInrement = 1;                                    
        //        }   
        //        qry.setSQL(sSql);
        //        qry.open();
        //        while(qry.next())              
        //        {
        //        	iAcctItemGroupID = getBelong(qry.field(1).asInteger());
        //        	
        //        	//将费用归并到对应的账目组
        //	        sprintf(sKey,"%d-%d-%d", iAcctItemGroupID, iTotalOrInrement, iBillOrAcct); 
        //            if(!m_oStatInfoTree.get(sKey, &poStatResultInfo))
        //            {            	
        //                poStatResultInfo = addStatInfoNode(iAcctItemGroupID, iTotalOrInrement, iBillOrAcct);
        //                m_oStatInfoTree.add(sKey, poStatResultInfo);   
        //            }       
        //            poStatResultInfo->m_lCharge += qry.field(0).asLong();                	
        //        }
        //        qry.close();                           
		//	}
		//}
		    
		//else
		//{
		//	iAcctItemGroupID = getStatType(sTableName);
		//	if(!m_bStatType&&(iTableType == EVENT_TABLE_TYPE_CALL_EVENT||
		//		           iTableType == EVENT_TABLE_TYPE_DATA_EVENT||
		//		           iTableType == EVENT_TABLE_TYPE_VALUE_ADDED_EVENT))
		//	{				
		//		sprintf(sSql, " SELECT /*parallel (a,5)*/nvl(sum(a.charge1+a.charge2+a.charge3+a.charge4+a.charge5+a.charge6),0) CHARGE "
        //                      " FROM %s a "
        //                      " WHERE a.created_date BETWEEN to_Date(%s, 'yyyymmddhh24miss') AND to_Date(%s, 'yyyymmddhh24miss')",
        //                      sTableName, m_sPreBackupTime, m_sThisBackupTime);                
		//	}	    
		//	else
		//	{
   	    //        sprintf(sSql, " SELECT /*parallel (a,5)*/nvl(sum(a.charge1+a.charge2+a.charge3+a.charge4+a.charge5+a.charge6),0) CHARGE "
    	//                      " FROM %s a " , sTableName);
    	//        iTotalOrInrement = 1;               
		//	}	       
        //	sprintf(sKey,"%d-%d-%d", iAcctItemGroupID, iTotalOrInrement, iBillOrAcct); 
        //    if(!m_oStatInfoTree.get(sKey, &poStatResultInfo))
        //    {            	
        //        poStatResultInfo = addStatInfoNode(iAcctItemGroupID, iTotalOrInrement, iBillOrAcct);
        //        m_oStatInfoTree.add(sKey, poStatResultInfo);   
        //    }   	                 	              
        //    qry.setSQL(sSql);
        //    qry.open();   
        //    if(qry.next()) 	               
        //    {
        //        poStatResultInfo->m_lCharge += qry.field(0).asLong();                	
        //    }	    
        //    qry.close();                                       	                 		
		//}			
	   	    	   	
	}
	return 0;
}


int CheckDailyAcctItemAggr::statDailyFromTicket(bool bCycleEnd)
{
	vector<TableInfo >::iterator iter;

	TOCIQuery qry(&m_TOCITicketDBConn);
	
	char sSql[1024] = {0};
	char sKey[100] = {0};
	char sTableName[100] = {0};   
		
	int iTableType = 0;	
	int iOrgID = 0;
	char sAreaCode[5] = {0};
	long lCharge = 0;
	long lServID = 0;
	
	int iDataSource = 0;
	
	int iAcctItemGroupID = 0;
	
	int iOrg = 0;
	
    KeyTree<StatResultInfo *> m_oStatInfoTree;	
	StatResultInfo *poStatResultInfo;    		
		
	int iTotalOrInrement = 0;//0:增量 1 全量
	int iBillOrAcct = 0; //0:清单费用 1 帐单费用
	
	for (iter = m_oTicketTableNames.begin(); iter != m_oTicketTableNames.end(); ++iter)
	{ 
		strcpy(sTableName, (*iter).m_sTableName);
		iTableType = (*iter).m_iTableType;
		Log::log(0, "统计表%s费用", sTableName);

		if (!m_bStatType&&!bCycleEnd&&
			(iTableType == EVENT_TABLE_TYPE_CALL_EVENT||
			 iTableType == EVENT_TABLE_TYPE_DATA_EVENT||
			 iTableType == EVENT_TABLE_TYPE_VALUE_ADDED_EVENT||
			 iTableType == 21||iTableType == 22||
			 iTableType == 23||iTableType == 24||
			 iTableType == 5||iTableType == 43||
			 iTableType == 44||iTableType == 45||
			 iTableType == 46))//增量
		{	
	    	sprintf(sSql, " SELECT /*+ parallel (t,5)*/ "
	    				  " billing_area_code,"	
	            	      " (t.charge1+t.charge2+t.charge3+t.charge4) charge"	                  
	                	  " FROM %s t "
                       	  " WHERE t.start_date BETWEEN to_Date(:statBeginDate, 'yyyymmddhh24miss') + 1/(24*3600) "
        	              " AND to_Date(:statEndDate, 'yyyymmddhh24miss') ",
                           sTableName);
			iTotalOrInrement = 0;
			iAcctItemGroupID = TICKET_FEE;               
		}  
		else
		{	            
    		if (iTableType == EVENT_TABLE_TYPE_SET_FEE_EVENT)//套餐费表没有区号、BILLING_ORG_ID	
    		{
    	    	sprintf(sSql, " SELECT /*+ parallel (t,5)*/ t.serv_id, (t.charge1+t.charge2+t.charge3+t.charge4) CHARGE "
    	        	         " FROM %s t " ,sTableName); 
    	        	          
            	iAcctItemGroupID = CYCLE_FEE;    	        	            		
    		}		 
    		else if (iTableType == EVENT_TABLE_TYPE_CYCLE_FEE_EVENT)
    		{	
    	    	sprintf(sSql, " SELECT /*+ parallel (t,5)*/ t.billing_org_id, (t.charge1+t.charge2+t.charge3+t.charge4) CHARGE "
    	                 " FROM %s t " ,sTableName); 
    	                       		
            	iAcctItemGroupID = CYCLE_FEE;    	                 
    		}else
    		{
    			if (bCycleEnd)//月底不管怎样都全量统计一次
    			{
		    		sprintf(sSql, " SELECT /*+ parallel (t,5)*/ "
		    				  	  " billing_area_code,"	
		                	  	  " (t.charge1+t.charge2+t.charge3+t.charge4) charge"	                  
		                  		  " FROM %s t ",
                           		  sTableName);     				
    			}else
    			{	
		    		sprintf(sSql, " SELECT /*+ parallel (t,5)*/ "
		    				  	  " billing_area_code,"	
		                	  	  " (t.charge1+t.charge2+t.charge3+t.charge4) charge"	                  
		                  		  " FROM %s t "
                          	  	" WHERE t.start_date <= to_Date(:statEndDate, 'yyyymmddhh24miss') ",
                           	sTableName); 
                }           	
            	iAcctItemGroupID = TICKET_FEE;                             			
    		}	
    		
            iTotalOrInrement = 1;                                    
        }

        qry.setSQL(sSql);
        
		if (iTotalOrInrement == 0)
		{
        	qry.setParameter("statBeginDate", StatInterface::m_pPlan->m_sDataBeginTime);			
		}
        
        if (iTableType != EVENT_TABLE_TYPE_CYCLE_FEE_EVENT&&iTableType != EVENT_TABLE_TYPE_SET_FEE_EVENT&&!bCycleEnd)
        {
        	qry.setParameter("statEndDate", StatInterface::m_pPlan->m_sDataEndTime);
        }
        	
        qry.open();
        
        while (qry.next())              
        {
        	lCharge = qry.field(1).asLong();
        	
        	if (lCharge == 0)
        		continue;
        	
			memset(sAreaCode, 0, sizeof(sAreaCode));
			
            if (iTableType == EVENT_TABLE_TYPE_SET_FEE_EVENT||iTableType == EVENT_TABLE_TYPE_CYCLE_FEE_EVENT) 
            {         	
            	if (iTableType == EVENT_TABLE_TYPE_SET_FEE_EVENT)
            	{
            		lServID = qry.field(0).asLong();            		
            		
            		if (G_PUSERINFO->getServ(G_SERV, lServID, StatInterface::m_pPlan->m_sDataBeginTime))
            		{
    	        	    //m_pOrgMgrEx->getAreaCode(G_SERV.getOrgID(), sAreaCode, 4, DEFAULT_ORG_LEVEL);
       	    			iOrg = m_pOrgMgrEx->getLevelParentID(G_SERV.getOrgID(), DEFAULT_ORG_LEVEL);
	            	}      	
	            }else
	            {
   	        	    iOrg = m_pOrgMgrEx->getLevelParentID(qry.field(0).asInteger(), DEFAULT_ORG_LEVEL);	            	
	            }	
	            
				m_pOrgMgrEx->getAreaCode(iOrg, sAreaCode, 4);	
				
            }else
            {
	            	strncpy(sAreaCode, qry.field(0).asString(), 4);		            	
            }
            
	        if (sAreaCode[0] == '\0'||strcmp(sAreaCode,"0")==0)
	        {	            	
	           	strcpy(sAreaCode, m_sDefaultAreaCode);
	        }            
            
            if (sAreaCode[0] == '0')
            {	
            	iDataSource = atoi(sAreaCode+1);
            }else
            {
            	iDataSource = atoi(sAreaCode);
            }	

            /*switch (iTableType)
            {                	
            	case EVENT_TABLE_TYPE_CALL_EVENT:
            	case EVENT_TABLE_TYPE_DATA_EVENT:
            	case EVENT_TABLE_TYPE_VALUE_ADDED_EVENT:
            	case 21:
            	case 22:
            	case 23:            		
            	case 24:            		            		
            		iAcctItemGroupID = TICKET_FEE;
            		break;
            	case EVENT_TABLE_TYPE_CYCLE_FEE_EVENT:
            	case EVENT_TABLE_TYPE_SET_FEE_EVENT:
            		iAcctItemGroupID = CYCLE_FEE;
            		break;    	              				
            	default:
            		iAcctItemGroupID = TICKET_FEE;            		
            		break;			
            } */     	        	
            
            sprintf(sKey,"%d-%d", iDataSource, iAcctItemGroupID); 
            if (!m_oStatInfoTree.get(sKey, &poStatResultInfo))
            {            	
                poStatResultInfo = addStatInfoNode(iAcctItemGroupID, iTotalOrInrement, iBillOrAcct, iDataSource);
                m_oStatInfoTree.add(sKey, poStatResultInfo);   
            }
            
            if (iTotalOrInrement == 0) 
            {	           
            	poStatResultInfo->m_lCharge += lCharge;
				poStatResultInfo->m_lDailyRecord += 1;
            }else
            {
            	poStatResultInfo->m_lMonthCharge += lCharge;
				poStatResultInfo->m_lRecord += 1;                           	            	
            }	
            
        }
        
        qry.close();                           		
	   	    	   	
	}
	
	return 0;		
}

int CheckDailyAcctItemAggr::statDailyAcctAggr(bool bCycleEnd)
{
	vector<TableInfo >::iterator iter;

	DEFINE_QUERY(qry);
	char sSql[1024] = {0};
	char sKey[100] = {0};
	char sTableName[100] = {0};   
		
	int iTableType = 0;	
	int iOrgID = 0;
	char sAreaCode[5] = {0};
	long lCharge = 0;
	long lServID = 0;
	
	int iDataSource = 0;
	
	int iAcctItemGroupID = 0;
	int iOrg = 0;
	
    KeyTree<StatResultInfo *> m_oStatInfoTree;	
	StatResultInfo *poStatResultInfo;    		
		
	int iTotalOrInrement = 0;//0:增量 1 全量
	int iBillOrAcct = 0; //0:清单费用 1 帐单费用
	
	for (iter = m_oBillTableNames.begin(); iter != m_oBillTableNames.end(); iter++)
	{ 
		strcpy(sTableName, (*iter).m_sTableName);
		iTableType = (*iter).m_iTableType;
		Log::log(0, "统计表%s费用", sTableName);

		if (!m_bStatType&&!bCycleEnd&&
			(iTableType == EVENT_TABLE_TYPE_CALL_EVENT||
			 iTableType == EVENT_TABLE_TYPE_DATA_EVENT||
			 iTableType == EVENT_TABLE_TYPE_VALUE_ADDED_EVENT||
			 iTableType == 21||iTableType == 22||
			 iTableType == 23||iTableType == 24))//增量
		{	
	    	sprintf(sSql, " SELECT /*+ parallel (t,5)*/ "
	    				  " billing_area_code,"	
	            	      " (t.charge1+t.charge2+t.charge3+t.charge4) charge"	                  
	                	  " FROM %s t "
                       	  " WHERE t.created_date BETWEEN to_Date(:statBeginDate, 'yyyymmddhh24miss') + 1/(24*3600) "
        	              " AND to_Date(:statEndDate, 'yyyymmddhh24miss') ",
                           sTableName);
			iTotalOrInrement = 0;
			iAcctItemGroupID = TICKET_FEE;               
		}  
		else
		{	            
    		if (iTableType == EVENT_TABLE_TYPE_SET_FEE_EVENT)//套餐费表没有区号、BILLING_ORG_ID	
    		{
    	    	sprintf(sSql, " SELECT /*+ parallel (t,5)*/ t.serv_id, (t.charge1+t.charge2+t.charge3+t.charge4) CHARGE "
    	        	         " FROM %s t " ,sTableName); 
    	        	          
            	iAcctItemGroupID = CYCLE_FEE;    	        	            		
    		}		 
    		else if (iTableType == EVENT_TABLE_TYPE_CYCLE_FEE_EVENT)
    		{	
    	    	sprintf(sSql, " SELECT /*+ parallel (t,5)*/ t.billing_org_id, (t.charge1+t.charge2+t.charge3+t.charge4) CHARGE "
    	                 " FROM %s t " ,sTableName); 
    	                       		
            	iAcctItemGroupID = CYCLE_FEE;    	                 
    		}else
    		{
    			if (bCycleEnd)//月底不管怎样都全量统计一次
    			{
		    		sprintf(sSql, " SELECT /*+ parallel (t,5)*/ "
		    				  	  " billing_area_code,"	
		                	  	  " (t.charge1+t.charge2+t.charge3+t.charge4) charge"	                  
		                  		  " FROM %s t ",
                           		  sTableName);     				
    			}else
    			{	
		    		sprintf(sSql, " SELECT /*+ parallel (t,5)*/ "
		    				  	  " billing_area_code,"	
		                	  	  " (t.charge1+t.charge2+t.charge3+t.charge4) charge"	                  
		                  		  " FROM %s t "
                          	  	" WHERE t.created_date <= to_Date(:statEndDate, 'yyyymmddhh24miss') ",
                           	sTableName); 
                }           	
            	iAcctItemGroupID = TICKET_FEE;                             			
    		}	
    		
            iTotalOrInrement = 1;                                    
    }

        qry.setSQL(sSql);
        
		if (iTotalOrInrement == 0)
		{
        	qry.setParameter("statBeginDate", StatInterface::m_pPlan->m_sDataBeginTime);			
		}
        
        if (iTableType != EVENT_TABLE_TYPE_CYCLE_FEE_EVENT&&iTableType != EVENT_TABLE_TYPE_SET_FEE_EVENT&&!bCycleEnd)
        {
        	qry.setParameter("statEndDate", StatInterface::m_pPlan->m_sDataEndTime);
        }
        	
        qry.open();
        
        while (qry.next())              
        {
        	lCharge = qry.field(1).asLong();
        	
        	if (lCharge == 0)
        		continue;
        	
			memset(sAreaCode, 0, sizeof(sAreaCode));
			
            if (iTableType == EVENT_TABLE_TYPE_SET_FEE_EVENT||iTableType == EVENT_TABLE_TYPE_CYCLE_FEE_EVENT) 
            {         	
            	if (iTableType == EVENT_TABLE_TYPE_SET_FEE_EVENT)
            	{
            		lServID = qry.field(0).asLong();            		
            		
            		if (G_PUSERINFO->getServ(G_SERV, lServID, StatInterface::m_pPlan->m_sDataBeginTime))
            		{
    	        	    //m_pOrgMgrEx->getAreaCode(G_SERV.getOrgID(), sAreaCode, 4, DEFAULT_ORG_LEVEL);
       	    			iOrg = m_pOrgMgrEx->getLevelParentID(G_SERV.getOrgID(), DEFAULT_ORG_LEVEL);
	            	}      	
	            }else
	            {
   	        	    iOrg = m_pOrgMgrEx->getLevelParentID(qry.field(0).asInteger(), DEFAULT_ORG_LEVEL);	            	
	            }	
	            
				m_pOrgMgrEx->getAreaCode(iOrg, sAreaCode, 4);	            
            }else
            {
	            	strncpy(sAreaCode, qry.field(0).asString(), 4);		            	
            }
            
	        if (sAreaCode[0] == '\0'||strcmp(sAreaCode,"0")==0)
	        {	            	
	           	strcpy(sAreaCode, m_sDefaultAreaCode);
	        }            
            
            if (sAreaCode[0] == '0')
            {	
            	iDataSource = atoi(sAreaCode+1);
            }else
            {
            	iDataSource = atoi(sAreaCode);
            }	

            /*switch (iTableType)
            {                	
            	case EVENT_TABLE_TYPE_CALL_EVENT:
            	case EVENT_TABLE_TYPE_DATA_EVENT:
            	case EVENT_TABLE_TYPE_VALUE_ADDED_EVENT:
            	case 21:
            	case 22:
            	case 23:            		
            	case 24:            		            		
            		iAcctItemGroupID = TICKET_FEE;
            		break;
            	case EVENT_TABLE_TYPE_CYCLE_FEE_EVENT:
            	case EVENT_TABLE_TYPE_SET_FEE_EVENT:
            		iAcctItemGroupID = CYCLE_FEE;
            		break;    	              				
            	default:
            		iAcctItemGroupID = TICKET_FEE;            		
            		break;			
            } */     	        	
            
            sprintf(sKey,"%d-%d", iDataSource, iAcctItemGroupID); 
            if (!m_oStatInfoTree.get(sKey, &poStatResultInfo))
            {            	
                poStatResultInfo = addStatInfoNode(iAcctItemGroupID, iTotalOrInrement, iBillOrAcct, iDataSource);
                m_oStatInfoTree.add(sKey, poStatResultInfo);   
            }
            
            if (iTotalOrInrement == 0) 
            {	           
            	poStatResultInfo->m_lCharge += lCharge;
				poStatResultInfo->m_lDailyRecord += 1;
            }else
            {
            	poStatResultInfo->m_lMonthCharge += lCharge;
				poStatResultInfo->m_lRecord += 1;                           	            	
            }	
            
        }
        
        qry.close();                           		
	   	    	   	
	}
	
	return 0;	
}

int CheckDailyAcctItemAggr::statMonthAcctAggr()
{
    if (!setBillingCycleID(StatInterface::m_pPlan->m_sDataBeginTime))
    {
        Log::log(0, " CheckDailyAcctItemAggr::statMonthAcctAggr帐期未能获取 ");
    	return -1;
    }	

#ifdef DEF_JIANGSU
	getAcctTables(m_TOCITicketDBConn);
#else   	
	getAcctTables();
#endif	
	if (m_vAcctTableNames.empty())
	{
        Log::log(0, " CheckDailyAcctItemAggr::statMonthAcctAggr总账表未能获取 ");		
	}
		
    DEFINE_QUERY(qry);
    char sql[1024] = {0};
    char sKey[100] = {0};
    char sAreaCode[5] = {0};
    
    long lServID = 0;
    long lCharge = 0;      
    int  iTableType = 0;
    int  iItemSourceID = 0;
	int  iDataSource = 0; 
	int iOrg = 0;   
    
    MonthStatResultInfo *poStatResultInfo;    
    vector<TableInfo >::iterator iter;
       	
    for (iter = m_vAcctTableNames.begin(); iter != m_vAcctTableNames.end(); ++iter)	
    {
    	Log::log(0, "统计%s表费用",( *iter).m_sTableName);
    	iTableType = (*iter).m_iTableType;
    	if (iTableType == TABLE_TYPE_ACCT_ITEM_AGGR)
    	{
    	    sprintf(sql, " SELECT /*+ parallel (a,5)*/ a.serv_id, a.charge, a.item_source_id "
    	                 " FROM %s a WHERE a.charge<>0 " ,(*iter).m_sTableName );     		
    	}
           
        qry.setSQL(sql);
        qry.open();
        while (qry.next())
        {	
        	lServID = qry.field(0).asLong() ;
        	lCharge = qry.field(1).asLong() ;
			iItemSourceID = qry.field(2).asInteger() ;
			memset(sAreaCode, 0, sizeof(sAreaCode));
			
			iOrg = -1;						
            if (G_PUSERINFO->getServ(G_SERV, lServID, StatInterface::m_pPlan->m_sDataBeginTime))
            {
    	        //m_pOrgMgrEx->getAreaCode(G_SERV.getOrgID(), sAreaCode, 4, DEFAULT_ORG_LEVEL);
       	    	iOrg = m_pOrgMgrEx->getLevelParentID(G_SERV.getOrgID(), DEFAULT_ORG_LEVEL);
	        }

			m_pOrgMgrEx->getAreaCode(iOrg, sAreaCode, 4);
	            
	        if (sAreaCode[0] == '\0'||strcmp(sAreaCode,"0")==0)
	        {	            	
	          	strcpy(sAreaCode, m_sDefaultAreaCode);
	        }
     		
            if (sAreaCode[0] == '0')
            {	
            	iDataSource = atoi(sAreaCode+1);
            }else
            {
            	iDataSource = atoi(sAreaCode);
            }	
			
			m_mIter = m_mapMonthAggr.find(iDataSource);                   	
            if (m_mIter != m_mapMonthAggr.end())
            {            	
                poStatResultInfo = m_mIter->second;
            }else
            {
            	poStatResultInfo = new MonthStatResultInfo();
            	m_mapMonthAggr[iDataSource] = poStatResultInfo;
            }

			poStatResultInfo->m_iDataSource = iDataSource;            		
            if (iItemSourceID == ITEM_SOURCE_STD||
            	iItemSourceID == ITEM_SOURCE_DISCOUNT)
            {
            	poStatResultInfo->m_lBaiseFee += lCharge;        			
            }	
            else
            {
            	poStatResultInfo->m_lDisctFee += lCharge;
            	poStatResultInfo->m_lDisctRecord += 1;            	
            }	            	
        }    
		qry.close();   	    	                                      	                
    }             	
	
    Log::log(0, "CheckDailyAcctItemAggr::statMonthAcctAggr月帐统计结束");
    return 1;	
}	

StatResultInfo *CheckDailyAcctItemAggr::addStatInfoNode(int iAcctItemGroupID, int iTotalOrIncrement, int iBillOrAcct, int iDataSource)
{
	StatResultInfo *poStatResultInfo = new StatResultInfo();
	
	strcpy(poStatResultInfo->m_sAcctItemDate, m_sAcctItemDate);
	poStatResultInfo->m_iAcctItemGroupID = iAcctItemGroupID;
    poStatResultInfo->m_iTotalOrInrement = iTotalOrIncrement;
	poStatResultInfo->m_iBillOrAcct = iBillOrAcct;
	
	poStatResultInfo->m_iDataSource = iDataSource;	
	
    m_poStatInfoBuf.push_back(poStatResultInfo);
    return poStatResultInfo;
}	

int CheckDailyAcctItemAggr::statAcctCharge()
{
	vector<TableInfo >::iterator iter;
	StatResultInfo *poStatResultInfo;
		
	DEFINE_QUERY(qry);
	char sSql[1024];		
	char sTableName[51];
	char sKey[50];
	int iAcctItemGroupID = 0;
	memset(sTableName, 0, sizeof (sTableName));
	memset(sSql, 0, sizeof (sSql));	
	memset(sKey, 0, sizeof (sKey));	
	int iTotalOrInrement = 1;//0:增量 1 全量
	int iBillOrAcct = 1; //0:清单费用 1 帐单费用	
	
	StatMergeAcctItemMgr oStatMergeAcctItemMgr((char*)"JF_TJ_XTJK_RK_6");
	
    KeyTree<StatResultInfo * > m_oStatInfoTree;	
	for (iter = m_oBackupTableNames.begin(); iter != m_oBackupTableNames.end(); ++iter)
	{
		strcpy(sTableName, (*iter).m_sTableName);
		Log::log(0, "统计表%s费用", sTableName);
		
	    sprintf(sSql, " SELECT /*+ parallel (t,5)*/t.charge, t.acct_item_type_id "
	                  " FROM %s t "
                      " WHERE t.charge<>0 "
                      " AND t.item_source_id IN (%s) ", sTableName, BILL_INCLUDE_ITEM_SOURCE_LIST);		//加上item_source_ID判断
        qry.setSQL(sSql);
        qry.open();
        while (qry.next())              
        {
            iAcctItemGroupID = oStatMergeAcctItemMgr.getMergeItemIDByAcctItem(qry.field(1).asInteger());        
            
        	sprintf(sKey,"%d-%d-%d", iAcctItemGroupID, iTotalOrInrement, iBillOrAcct); 
            if (!m_oStatInfoTree.get(sKey, &poStatResultInfo))
            {            	
                poStatResultInfo = addStatInfoNode(iAcctItemGroupID, iTotalOrInrement, iBillOrAcct);
                m_oStatInfoTree.add(sKey, poStatResultInfo);   
            }       
            poStatResultInfo->m_lCharge += qry.field(0).asLong();                        
        }        
        qry.close();	
	}
	return 1;
}	

void CheckDailyAcctItemAggr::CalcTotalAggr()
{
    vector<StatResultInfo *>::iterator iter;
    long lPreCharge = 0; 	
    for (iter = m_poStatInfoBuf.begin(); iter != m_poStatInfoBuf.end(); ++iter) 
    {
    	if ((*iter)->m_iTotalOrInrement == 0)
    	{
    		lPreCharge = getPreAggr(m_sPreAcctItemDate, (*iter)->m_iAcctItemGroupID);
    		(*iter)->m_lCharge += lPreCharge;    
    		(*iter)->m_iTotalOrInrement = 1;		
    	}	
    }		
}

int CheckDailyAcctItemAggr::CalcDailyAggr()
{
    vector<StatResultInfo *>::iterator v_iter;
	StatResultInfo *pPreStatResultInfo = NULL;	

	if (strcmp(m_sAcctItemDate+6, "01") == 0)//月初
	{
    	for (v_iter = m_poStatInfoBuf.begin(); v_iter != m_poStatInfoBuf.end(); ++v_iter) 
    	{
    		pPreStatResultInfo = (*v_iter);
    		if ((*v_iter)->m_iTotalOrInrement == 0)
    		{
    			(*v_iter)->m_lMonthCharge = (*v_iter)->m_lCharge;
    			(*v_iter)->m_lRecord = (*v_iter)->m_lDailyRecord;
    			
    			(*v_iter)->m_iTotalOrInrement = 1;		    			   
    		}else
    		{
    			(*v_iter)->m_lCharge = (*v_iter)->m_lMonthCharge;
    			(*v_iter)->m_lDailyRecord= (*v_iter)->m_lRecord;
    		}
    	}
    	
    	return 1;		
	}	
	
	loadDailyAggr();//载入前次日帐
	
	char sKey[100] = {0};
	//StatResultInfo *pPreStatResultInfo = NULL;	
	map<string, StatResultInfo*>::iterator m_iter;
	
    for (v_iter = m_poStatInfoBuf.begin(); v_iter != m_poStatInfoBuf.end(); ++v_iter) 
    {
    	pPreStatResultInfo = (*v_iter);
		sprintf(sKey, "%d_%d", (*v_iter)->m_iDataSource, (*v_iter)->m_iAcctItemGroupID);
		m_iter = m_mapDailyAggr.find(string(sKey));  
		
		pPreStatResultInfo =  m_iter->second;
		//if (m_iter == m_mapDailyAggr.end())
    	if ((*v_iter)->m_iTotalOrInrement == 0)
    	{
    		if (m_iter == m_mapDailyAggr.end())
    		{
    			(*v_iter)->m_lMonthCharge = (*v_iter)->m_lCharge;
    			(*v_iter)->m_lRecord = (*v_iter)->m_lDailyRecord;    			
    		}else
    		{	
    			(*v_iter)->m_lMonthCharge = m_iter->second->m_lMonthCharge + (*v_iter)->m_lCharge;
   				(*v_iter)->m_lRecord = m_iter->second->m_lRecord + (*v_iter)->m_lDailyRecord;
   				
    		}
    		(*v_iter)->m_iTotalOrInrement = 1;		
    	}else
    	{
    		if (m_iter == m_mapDailyAggr.end())
			{
    			(*v_iter)->m_lCharge = (*v_iter)->m_lMonthCharge;
    			(*v_iter)->m_lDailyRecord= (*v_iter)->m_lRecord;				
			}else
			{	
    			if ((*v_iter)->m_iAcctItemGroupID == TICKET_FEE)
    			{
    				(*v_iter)->m_lDailyRecord =(*v_iter)->m_lRecord - m_iter->second->m_lRecord;
    			}else
    			{
    				(*v_iter)->m_lDailyRecord= (*v_iter)->m_lRecord;    			
    			}	
	    		(*v_iter)->m_lCharge = (*v_iter)->m_lMonthCharge - m_iter->second->m_lMonthCharge;    
    			
    		}    			
    	}	
    }	
    
    return 0;
}

void CheckDailyAcctItemAggr::loadDailyAggr()
{
	DEFINE_QUERY(qry);
	char sKey[100] = {0};
	char sMonth[8] = {0};
	
	strncpy(sMonth, m_sAcctItemDate, 6);	
	
	qry.setSQL(" select acct_item_date,daily_fee,month_fee, "
			   " record_cnt,daily_cnt,acct_item_group_id,data_source "
			   " from CHECK_DAILY_ACCT_ITEM_AGGR_LOG t "
			   " where substr(t.acct_item_date,0,6) = :billing_month "
			   " order by acct_item_date desc ");//保证当前账期最大日期的日帐
	qry.setParameter("billing_month", sMonth);
			   
	qry.open();
	
	while(qry.next())
	{
		StatResultInfo *poStatResultInfo = new StatResultInfo();
		
		strcpy(poStatResultInfo->m_sAcctItemDate, qry.field(0).asString());
		poStatResultInfo->m_lCharge = qry.field(1).asLong();     		
		poStatResultInfo->m_lMonthCharge = qry.field(2).asLong();
    	poStatResultInfo->m_lRecord = qry.field(3).asLong();
    	poStatResultInfo->m_lDailyRecord = qry.field(4).asLong();
		poStatResultInfo->m_iAcctItemGroupID = qry.field(5).asInteger();
		poStatResultInfo->m_iDataSource = qry.field(6).asLong();	
		
		sprintf(sKey, "%d_%d", poStatResultInfo->m_iDataSource, poStatResultInfo->m_iAcctItemGroupID);
		
		
		if (m_mapDailyAggr.find(string(sKey)) != m_mapDailyAggr.end())//只载入当前账期最大日期的日帐
			continue;
		
    	m_mapDailyAggr[string(sKey)] = poStatResultInfo;		
	}
	
}

int CheckDailyAcctItemAggr::CalcMonthAggr()
{
	/*while(loadMonthAggr() == 0)//日帐未稽核完毕
	{
		sleep(60);
		continue;
	}*/
	if(loadMonthAggr() == 0)//日帐未稽核完毕
	{
		Log::log(0,"日帐未稽核完毕，无法稽核月帐，请检查日帐稽核");
		return -1;		
	}	
	
	for (m_mIter = m_mapMonthAggr.begin(); m_mIter != m_mapMonthAggr.end(); ++m_mIter)
	{
		if (m_mIter->second->m_lTicketFee+m_mIter->second->m_lCycleFee != m_mIter->second->m_lBaiseFee)
		{
			m_mIter->second->m_lOffsetFee = m_mIter->second->m_lBaiseFee - (m_mIter->second->m_lTicketFee+m_mIter->second->m_lCycleFee);
		}	
	}
	
	return 1;
}

int CheckDailyAcctItemAggr::loadMonthAggr()
{
	if (m_mapMonthAggr.empty())
	{
		Log::log(0,"总账表没有数据,请检查");
		return -1;
	}  	
    BillingCycleMgr bcm;                                               
    BillingCycle *bc = bcm.getBillingCycle(m_iBillingCycleID);
    
    Date d(bc->getEndDate());
    d.addDay(-1);
    
    char sMonthEndDate[9] ={0};
    strncpy(sMonthEndDate, d.toString(), 8);
    
    int iDataSource = 0;
    int iAcctItemGroupID = 0;
    long lCharge = 0;
    long lRecord = 0;
    int iCnt = 0;
    
	DEFINE_QUERY(qry);
	
	qry.setSQL(" select data_source, acct_item_group_id, month_fee,record_cnt "
			   " from CHECK_DAILY_ACCT_ITEM_AGGR_LOG t "
			   " where t.acct_item_date = :month_end_date ");
			   
	qry.setParameter("month_end_date", sMonthEndDate);
	qry.open();
	
	while(qry.next())	    
	{
		iDataSource = qry.field(0).asInteger();
		iAcctItemGroupID = qry.field(1).asInteger();
		lCharge = qry.field(2).asLong();
		lRecord = qry.field(3).asLong();
		
		m_mIter = m_mapMonthAggr.find(iDataSource);
		if (m_mIter == m_mapMonthAggr.end())
		{
			m_mIter = m_mapMonthAggr.begin();
		}
		
		if (iAcctItemGroupID == TICKET_FEE)
		{	
			m_mIter->second->m_lTicketFee += lCharge;
			m_mIter->second->m_lTicketRecord += lRecord;

		}else if (iAcctItemGroupID == CYCLE_FEE)
		{
			m_mIter->second->m_lCycleFee += lCharge;
			m_mIter->second->m_lCycleRecord += lRecord;			
		}
			
		iCnt++;
	}
	
	return iCnt;
}


void CheckDailyAcctItemAggr::clearGarbageData()
{
	DEFINE_QUERY(qry);
	char sSql[1024];
	char sCycleBeginDate[16];
	memset(sSql, 0, sizeof(sSql));
	memset(sCycleBeginDate, 0, sizeof(sCycleBeginDate));
    BillingCycleMgr bcm;  
    BillingCycle *pCycle = bcm.getBillingCycle (m_iBillingCycleID); 	
    strcpy (sCycleBeginDate, pCycle->getStartDate());
	
	sprintf(sSql, " BEGIN "
	              " DELETE daily_acct_item_stat_Results t "
                  " WHERE  t.acct_item_date = '%s'; "
                  " DELETE daily_acct_item_stat_Results t "
                  " WHERE  to_date(t.acct_item_date,'yyyymmdd') < trunc(to_date(%s, 'yyyymmddhh24miss')); "
                  //" DELETE stat_interface t "
                  //" WHERE t.kpi_code = 'JF_TJ_XTJK_RK_6' "
                  //" AND substr(t.dimensions, 0, 8) = '%s';"
                  " END; ", 
                  m_sAcctItemDate, sCycleBeginDate/*, m_sAcctItemDate*/);//清理本日或前月垃圾数据
    qry.setSQL(sSql);
    qry.execute();
    //qry.commit();    
}	

long CheckDailyAcctItemAggr::getPreAggr(char *sPreAcctItemDate, int iAcctItemGroupID)
{
	DEFINE_QUERY(qry);
	char sSql[1024];
	long lPreCharge = 0;
	memset(sSql, 0, sizeof (sSql));
	sprintf(sSql, " SELECT t.lcharge "
	              " FROM daily_acct_item_stat_Results t "
                  " WHERE t.acct_item_date = '%s' "
                  " AND t.acct_item_group_id= %d", sPreAcctItemDate, iAcctItemGroupID);
    qry.setSQL(sSql);
    qry.open();              
    if (qry.next())    
    {
    	lPreCharge = qry.field(0).asInteger() ;
    }	          
    else
    {
    	Log::log(0, "获取账目组%d的%s日日账费用失败,前日数据缺失,需改变统计方式", iAcctItemGroupID, sPreAcctItemDate);
    }	
    qry.close();
    return lPreCharge;    
}

void CheckDailyAcctItemAggr::writeDB()
{
	if (m_poStatInfoBuf.empty())
		return;
    int iSize = m_poStatInfoBuf.size();  
    int cnt = 0 ;
    StatResultInfo *oBufArray = new StatResultInfo[iSize]() ;
    vector<StatResultInfo *>::iterator iter;

    DEFINE_QUERY(qry);
	char sSql[1024] = {0};
    strcpy(sSql," INSERT INTO daily_acct_item_stat_Results "
                " (acct_item_date, lCharge, acct_item_group_id, " 
                "  charge_type, charge_belong, stat_date) "                                                                     
                " VALUES "
                " (:acct_item_date, :lCharge, :acct_item_group_id, "
                "  :charge_type, :charge_belong, sysdate) ");
    
    qry.setSQL(sSql);
		   
    for (iter = m_poStatInfoBuf.begin(); iter != m_poStatInfoBuf.end(); ++iter) 
    {
        oBufArray[cnt].m_iAcctItemGroupID = (*iter)->m_iAcctItemGroupID;
        oBufArray[cnt].m_lCharge = (*iter)->m_lCharge;
        oBufArray[cnt].m_iTotalOrInrement = (*iter)->m_iTotalOrInrement;
        oBufArray[cnt].m_iBillOrAcct = (*iter)->m_iBillOrAcct;                       
        strcpy(oBufArray[cnt].m_sAcctItemDate, (*iter)->m_sAcctItemDate);    
        cnt++;                 
    }                  
    qry.setParamArray("acct_item_group_id", &(oBufArray[0].m_iAcctItemGroupID), sizeof(oBufArray[0]));
    qry.setParamArray("lCharge", &(oBufArray[0].m_lCharge), sizeof(oBufArray[0]));
    qry.setParamArray("charge_type", &(oBufArray[0].m_iTotalOrInrement), sizeof(oBufArray[0]));
    qry.setParamArray("charge_belong", &(oBufArray[0].m_iBillOrAcct), sizeof(oBufArray[0]));     
    qry.setParamArray("acct_item_date", (char**)&(oBufArray[0].m_sAcctItemDate), sizeof(oBufArray[0]), sizeof(oBufArray[0].m_sAcctItemDate));     
    qry.execute(cnt);
    //qry.commit();
    delete[] oBufArray;	
}

void CheckDailyAcctItemAggr::writeDailyAcctAggrLog()
{
	if (m_poStatInfoBuf.empty())
		return;
    int iSize = m_poStatInfoBuf.size();  
    int cnt = 0 ;
    StatResultInfo *oBufArray = new StatResultInfo[iSize]() ;
    vector<StatResultInfo *>::iterator iter;

    DEFINE_QUERY(qry);
    
    qry.setSQL( " INSERT INTO CHECK_DAILY_ACCT_ITEM_AGGR_LOG "
                " (ACCT_ITEM_DATE, DAILY_FEE, MONTH_FEE, " 
                "  RECORD_CNT, DAILY_CNT, ACCT_ITEM_GROUP_ID, DATA_SOURCE, STAT_DATE) "                                                                     
                " VALUES "
                " (:ACCT_ITEM_DATE, :DAILY_FEE, :MONTH_FEE, "
                "  :RECORD_CNT, :DAILY_CNT, :ACCT_ITEM_GROUP_ID, :DATA_SOURCE, sysdate) ");
       
    for (iter = m_poStatInfoBuf.begin(); iter != m_poStatInfoBuf.end(); ++iter) 
    {
        oBufArray[cnt].m_iAcctItemGroupID = (*iter)->m_iAcctItemGroupID;
        oBufArray[cnt].m_lCharge = (*iter)->m_lCharge;
        oBufArray[cnt].m_lMonthCharge = (*iter)->m_lMonthCharge;
        oBufArray[cnt].m_lRecord = (*iter)->m_lRecord;
        oBufArray[cnt].m_lDailyRecord = (*iter)->m_lDailyRecord;
        oBufArray[cnt].m_iDataSource = (*iter)->m_iDataSource;
        strcpy(oBufArray[cnt].m_sAcctItemDate, (*iter)->m_sAcctItemDate);    
        cnt++;                 
    }                  
    qry.setParamArray("ACCT_ITEM_GROUP_ID", &(oBufArray[0].m_iAcctItemGroupID), sizeof(oBufArray[0]));
    qry.setParamArray("DAILY_FEE", &(oBufArray[0].m_lCharge), sizeof(oBufArray[0]));
    qry.setParamArray("MONTH_FEE", &(oBufArray[0].m_lMonthCharge), sizeof(oBufArray[0]));
    qry.setParamArray("RECORD_CNT", &(oBufArray[0].m_lRecord), sizeof(oBufArray[0]));
    qry.setParamArray("DAILY_CNT", &(oBufArray[0].m_lDailyRecord), sizeof(oBufArray[0]));
    qry.setParamArray("DATA_SOURCE", &(oBufArray[0].m_iDataSource), sizeof(oBufArray[0]));
    qry.setParamArray("ACCT_ITEM_DATE", (char**)&(oBufArray[0].m_sAcctItemDate), sizeof(oBufArray[0]), sizeof(oBufArray[0].m_sAcctItemDate));     
    qry.execute(cnt);
    //qry.commit();
    qry.close();
    
    delete[] oBufArray;		
}

void CheckDailyAcctItemAggr::writeMonthAcctAggrLog()
{
	if (m_mapMonthAggr.empty())
		return;
    int iSize = m_mapMonthAggr.size();  
    int cnt = 0 ;
    MonthStatResultInfo *oBufArray = new MonthStatResultInfo[iSize]();

    DEFINE_QUERY(qry);
    qry.setSQL( " INSERT INTO CHECK_MONTH_ACCT_ITEM_AGGR_LOG "
                " (BILLING_CYCLE_ID, BAISC_FEE, DISCT_FEE, DISCT_FEE_RECOND," 
                "  TICKET_FEE, TICKET_FEE_RECOND, CYCLE_FEE, CYCLE_FEE_RECOND, "
                "  OFFSET_FEE, OFFSET_FEE_RECOND, DATA_SOURCE, STAT_DATE) "                                                                     
                " VALUES "
                " (:BILLING_CYCLE_ID, :BAISC_FEE, :DISCT_FEE, :DISCT_FEE_RECOND," 
                "  :TICKET_FEE, :TICKET_FEE_RECOND, :CYCLE_FEE, :CYCLE_FEE_RECOND, "
                "  :OFFSET_FEE, :OFFSET_FEE_RECOND, :DATA_SOURCE, sysdate) ");
       
    for (m_mIter = m_mapMonthAggr.begin(); m_mIter != m_mapMonthAggr.end(); ++m_mIter) 
    {
        oBufArray[cnt].m_iBillingCycleID = m_mIter->second->m_iBillingCycleID;
        oBufArray[cnt].m_lTicketFee =  m_mIter->second->m_lTicketFee;
        oBufArray[cnt].m_lTicketRecord =  m_mIter->second->m_lTicketRecord;
        oBufArray[cnt].m_lCycleFee =  m_mIter->second->m_lCycleFee;
        oBufArray[cnt].m_lCycleRecord =  m_mIter->second->m_lCycleRecord;
        
        oBufArray[cnt].m_lBaiseFee =  m_mIter->second->m_lBaiseFee;
        oBufArray[cnt].m_lDisctFee =  m_mIter->second->m_lDisctFee;
        oBufArray[cnt].m_lDisctRecord =  m_mIter->second->m_lDisctRecord;
        oBufArray[cnt].m_lOffsetFee = m_mIter->second->m_lOffsetFee;
        oBufArray[cnt].m_lOffsetRecord =  m_mIter->second->m_lOffsetRecord;    
        oBufArray[cnt].m_iDataSource =  m_mIter->second->m_iDataSource;
        cnt++;                 
    }                  
    qry.setParamArray("BILLING_CYCLE_ID", &(oBufArray[0].m_iBillingCycleID), sizeof(oBufArray[0]));
    qry.setParamArray("BAISC_FEE", &(oBufArray[0].m_lBaiseFee), sizeof(oBufArray[0]));
    qry.setParamArray("DISCT_FEE", &(oBufArray[0].m_lDisctFee), sizeof(oBufArray[0]));
    qry.setParamArray("DISCT_FEE_RECOND", &(oBufArray[0].m_lDisctRecord), sizeof(oBufArray[0]));
    qry.setParamArray("TICKET_FEE", &(oBufArray[0].m_lTicketFee), sizeof(oBufArray[0]));
    qry.setParamArray("TICKET_FEE_RECOND", &(oBufArray[0].m_lTicketRecord), sizeof(oBufArray[0]));
    qry.setParamArray("CYCLE_FEE", &(oBufArray[0].m_lCycleFee), sizeof(oBufArray[0]));
    qry.setParamArray("CYCLE_FEE_RECOND", &(oBufArray[0].m_lCycleRecord), sizeof(oBufArray[0]));
    qry.setParamArray("OFFSET_FEE", &(oBufArray[0].m_lOffsetFee), sizeof(oBufArray[0]));
    qry.setParamArray("OFFSET_FEE_RECOND", &(oBufArray[0].m_lOffsetRecord), sizeof(oBufArray[0]));    
    qry.setParamArray("DATA_SOURCE", &(oBufArray[0].m_iDataSource), sizeof(oBufArray[0]));
    qry.execute(cnt);
    //qry.commit();
    delete[] oBufArray;		
}

void CheckDailyAcctItemAggr::writeStatInterface()
{
	if (m_poStatInfoBuf.empty())
		return;

    vector<StatResultInfo *>::iterator iter;    
    int iAcctItemGroupID = 0;
    long lChargeA;
    long lChargeB;
    char sTempA[1024];	
    char sTempB[1024];	

    memset(sTempA, 0, sizeof (sTempA));
    memset(sTempB, 0, sizeof (sTempA));    
    
    //strcpy(sTempA, "帐单费用");
    //strcpy(sTempB, "清单费用");
    sTempA[0] = 0; 
    sTempB[0] = 0;
    
    //JF_TJ_XTJK_RK_6:入库费用平衡校验
    //日期;本地清单费;长途清单费;上网清单费;其他清单费;本网账单费;长途账单费;上网账单费;其他账单费
    //平衡关系 -- 清单费用累加 == 账单费用累加
    Date d;
    StatInterface oRk_6 ("JF_TJ_XTJK_RK_6", d.toString());
    strcpy (oRk_6.sStatBeginDate, m_sThisBackupTime);
    strcpy (oRk_6.sEndDate, m_sCheckDate);  
    strcpy (oRk_6.sAreaCode, StatInterface::m_sDefaultAreaCode);
    
    DEFINE_QUERY(qry);    
    qry.setSQL(" select c.merge_item_id  "                   
			   " from  s_mnt_module a, "                     
			   "       S_MNT_CODE_MERGE_module_comp b, "     
			   "       S_MNT_STAT_MERGE_ITEM c "             			                                             
			   " where a.module_id = b.module_id "           
			   "   and b.merge_item_code = c.merge_item_code "
			   "   and a.module_code = 'JF_TJ_XTJK_RK_6' ");
    qry.open();  
    while (qry.next())
    {
    	iAcctItemGroupID = qry.field(0).asInteger();

    	lChargeA = 0;
    	lChargeB = 0;
        for (iter = m_poStatInfoBuf.begin(); iter != m_poStatInfoBuf.end(); ++iter) 
        {
        	if ((*iter)->m_iAcctItemGroupID == iAcctItemGroupID&&
        	    (*iter)->m_iBillOrAcct == 1)
        	{
        		lChargeA = (*iter)->m_lCharge;
        	}
        		
        	if ((*iter)->m_iAcctItemGroupID == iAcctItemGroupID&&
        	    (*iter)->m_iBillOrAcct == 0)
        	{
        		lChargeB = (*iter)->m_lCharge;
        	}	        	        	        	
        }
        sprintf(sTempA, "%s;%ld", sTempA, lChargeA);
        sprintf(sTempB, "%s;%ld", sTempB, lChargeB);
    }
    //其他费用 先这样改着
    lChargeA = 0;
    lChargeB = 0;
    for (iter = m_poStatInfoBuf.begin(); iter != m_poStatInfoBuf.end(); ++iter) 
    {
    	if ((*iter)->m_iAcctItemGroupID == 0&&
    	    (*iter)->m_iBillOrAcct == 1)
    	{
    		lChargeA = (*iter)->m_lCharge;
    	}
    		
    	if ((*iter)->m_iAcctItemGroupID == 0&&
    	    (*iter)->m_iBillOrAcct == 0)
    	{
    		lChargeB = (*iter)->m_lCharge;
    	}	        	        	        	
    }
    sprintf(sTempA, "%s;%ld", sTempA, lChargeA);
    sprintf(sTempB, "%s;%ld", sTempB, lChargeB);    
        	
	sprintf(oRk_6.sDimensions, "%s%s%s", m_sAcctItemDate, sTempA, sTempB);

    oRk_6.insert();   //未提交 
}	

int CheckDailyAcctItemAggr::processMain()
{
	if(!initParam())
	{
    	Log::log(0, "参数初始化失败");		
    	return -1;	
	}	
	DEFINE_QUERY(qry);
    qry.setSQL(" SELECT b.acct_item_date, to_char(b.backup_time, 'yyyymmddhh24miss') backup_time "
               " FROM   b_daily_acct_item_log b "
               " WHERE  b.check_flag = 0 "
               " AND    b.state = 'END' "
               " ORDER BY b.backup_time ");
    qry.open();
    
	char sAcctItemDate[9];    
    char sLastBackupTime[16];	
    int i = 0;
    while (qry.next())   
    {
    	i++;
    	strcpy(sAcctItemDate, qry.field(0).asString());
    	strcpy(sLastBackupTime, qry.field(1).asString());
    	
    	if (!getCheckDate(sAcctItemDate, sLastBackupTime))
    	{	
    		Log::log(0, "参数初始化失败");
    		qry.close();
    		return -1;
    	}	
    	
    	clearGarbageData();
    	statBillCharge();
    	statAcctCharge();
    	
    	if ((!m_bStatType)&&
    	    (m_sPreAcctItemDate[0] != '0'))
    	{    		
    		CalcTotalAggr();
    	}	
    	
    	writeDB();
        writeStatInterface();    	
    	setCheckFlagEnd();
    	DB_LINK->commit();
    } 
    qry.close();           
        
    if (i == 0)
    {
        Log::log(0, "暂无日账需要稽核");    	
    }	
    else
    {	
        Log::log(0, "共稽核%d天日账", i);
    }    	
    
    return 0;
}

int CheckDailyAcctItemAggr::checkDailyAcctItemAggr()
{
    cout <<"CheckDailyAcctItemAggr::checkDailyAcctItemAggr(50-30-02-00-00-008) ..."<<endl;
	
    StatInterface ocheckDailyAcctAggr("50-30-02-00-00-008");    
    vector<string> vDailyAcctAggr;
    vDailyAcctAggr.clear();
    
   	char sLineBuf[MaxLineLength] = {0};  
   	char sAcctItemName[100] = {0};

	strncpy(m_sAcctItemDate, ocheckDailyAcctAggr.sEndDate, 8);
    cout<<m_sAcctItemDate<<endl;

    DEFINE_QUERY(qry);
    qry.setSQL(" delete CHECK_DAILY_ACCT_ITEM_AGGR_LOG where acct_item_date >= :acct_item_date ");
    qry.setParameter("acct_item_date", m_sAcctItemDate);
    qry.execute();
    qry.commit();
    qry.close();
    	
	if(!initParam())
	{
    	Log::log(0, "checkDailyAcctItemAggr累帐平衡参数初始化失败");		
    	return -1;	
	}
	
	BillingCycleMgr bcm;                                           
    BillingCycle *bc = bcm.getBillingCycle(m_iBillingCycleID);
    
    bool bCycleEnd = false;
    char *pCyclceEndDate = bc->getEndDate();
	Date d1(ocheckDailyAcctAggr.sEndDate);
	Date d2(pCyclceEndDate);
	    
    if ( d2.diffStrictDay(d1) == 1 )
    {
    	bCycleEnd = true;
    }
    
    /*if (bCycleEnd)//靠配置便宜
    {
    	int iSec = 0
    	{
    		while (iSec < 1800)//月底等待半个小时
    		{
    			sleep(60);
    			iSec += 60;
    			continue;
    		}
    	}
    }	*/
	
	
	//strcpy(m_sPreBackupTime, ocheckDailyAcctAggr.sStatBeginDate);
	//strcpy(m_sThisBackupTime, ocheckDailyAcctAggr.sEndDate);
#ifdef DEF_JIANGSU
	statDailyFromTicket(bCycleEnd);
#endif	
	statDailyAcctAggr(bCycleEnd);//统计日清单
	
    CalcDailyAggr();//计算日帐以及截至目前的总账
    
    vector<StatResultInfo *>::iterator iter;
    for (iter = m_poStatInfoBuf.begin(); iter != m_poStatInfoBuf.end(); ++iter) 
	{
    	memset(sAcctItemName, 0, sizeof(sAcctItemName));	
    	
    	switch((*iter)->m_iAcctItemGroupID) 
    	{
    		case 1:
    			strcpy(sAcctItemName, "详单费用");
    			break;
    		case 2:
    			strcpy(sAcctItemName, "周期性费用");
    			break;
    		case 3:
    			strcpy(sAcctItemName, "代收费用");
    			break;
    		case 4:
    			strcpy(sAcctItemName, "一次性费用");
    			break;    			    			    			
    		case 5:
    			strcpy(sAcctItemName, "补收抵减");
    			break;    			
    		case 6:
    			strcpy(sAcctItemName, "帐务优惠费用");
    			break;
    		case 7:
    			strcpy(sAcctItemName, "其他费用");
    			break;
    		default:
    			strcpy(sAcctItemName, "其他费用");
    			break;    			    			    					
    	}  	
		
    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%d|%s|%d|%ld|%ld|%s", DR_ID, (*iter)->m_iDataSource, sAcctItemName, (*iter)->m_iAcctItemGroupID, (*iter)->m_lDailyRecord, 
													(*iter)->m_lCharge, ocheckDailyAcctAggr.sEndDate);
		vDailyAcctAggr.push_back(sLineBuf);												    			
	}
    
    ocheckDailyAcctAggr.writeFile(vDailyAcctAggr);    
    
	writeDailyAcctAggrLog();//日志入库
	
    DB_LINK->commit();
	
	return 1;	
}

int CheckDailyAcctItemAggr::checkMonthAcctItemAggr()
{
    cout <<"CheckDailyAcctItemAggr::checkMonthAcctItemAggr(50-30-02-00-00-010) ..."<<endl;
	
    StatInterface ocheckMonthAcctAggr("50-30-02-00-00-010");    
    vector<string> vMonthAcctAggr;
    vMonthAcctAggr.clear();	
    
   	char sLineBuf[MaxLineLength] = {0};
   	
   	statMonthAcctAggr();
   	
   	CalcMonthAggr();
   	
   	char sCycle[8] = {0};
	BillingCycleMgr bcm;                                           
    BillingCycle *bc = bcm.getBillingCycle(m_iBillingCycleID);
    
    strncpy(sCycle, bc->getStartDate(), 6);

	for (m_mIter = m_mapMonthAggr.begin(); m_mIter != m_mapMonthAggr.end(); ++m_mIter)
	{
		if (m_mIter->second->m_lTicketFee != 0)
		{	
    		memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    		sprintf(sLineBuf, "%d|%d|%s|%d|%ld|%ld|%s|%s", DR_ID, m_mIter->second->m_iDataSource, "详单费用", 1, m_mIter->second->m_lTicketRecord, 
													m_mIter->second->m_lTicketFee, sCycle, ocheckMonthAcctAggr.sEndDate);
			vMonthAcctAggr.push_back(sLineBuf);	
		}
		
		if (m_mIter->second->m_lCycleFee != 0)
		{	
    		memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    		sprintf(sLineBuf, "%d|%d|%s|%d|%ld|%ld|%s|%s", DR_ID, m_mIter->second->m_iDataSource, "周期性费用", 2, m_mIter->second->m_lDisctRecord, 
													m_mIter->second->m_lCycleFee, sCycle, ocheckMonthAcctAggr.sEndDate);
			vMonthAcctAggr.push_back(sLineBuf);
		}
		
		if (m_mIter->second->m_lDisctFee != 0)
		{	
    		memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    		sprintf(sLineBuf, "%d|%d|%s|%d|%ld|%ld|%s|%s", DR_ID, m_mIter->second->m_iDataSource, "帐务优惠费用", 6, m_mIter->second->m_lDisctRecord, 
													m_mIter->second->m_lDisctFee, sCycle, ocheckMonthAcctAggr.sEndDate);
			vMonthAcctAggr.push_back(sLineBuf);
		}	
		
		if (m_mIter->second->m_lOffsetFee != 0)
		{	
    		memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    		sprintf(sLineBuf, "%d|%d|%s|%d|%ld|%ld|%s|%s", DR_ID, m_mIter->second->m_iDataSource, "补收抵减", 5, m_mIter->second->m_lOffsetRecord, 
													m_mIter->second->m_lOffsetFee, sCycle, ocheckMonthAcctAggr.sEndDate);
			vMonthAcctAggr.push_back(sLineBuf);
		}						
	}   
	
    ocheckMonthAcctAggr.writeFile(vMonthAcctAggr);    
    
	writeMonthAcctAggrLog();//日志入库
	
    DB_LINK->commit();
	
	return 1;	
}

	
/*
int main()
{    
    try
    {   
    	CheckDailyAcctItemAggr p;
        p.processMain(); 
        Log::log(0, "日账稽核结束");        
    }	  
    catch (TOCIException &e) {
        cout << e.getErrMsg () << endl << e.getErrSrc() << endl;
        Log::log(0, "%s \n %s\n", e.getErrMsg(), e.getErrSrc());
    }
    catch (Exception &e) {
        cout << e.descript() << endl;
        Log::log (0, e.descript());
    }    
    catch (...)
    {
    	Log::log(0, "日账稽核失败");
    	return -1;	
    }
    return 0;
}

char StatInterface::m_sDefaultAreaCode[10] = {0};*/

//add by sunjy 
/*
 *	函 数 名 : checkDailyAcctItemAggrNew
 *	函数功能 : 提取记录累帐处理日志信息，主要用于当日的累帐费用准确性校验,AuditId=0008，Type=50,低频1
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckDailyAcctItemAggr::checkDailyAcctItemAggrNew()
{
    cout <<"CheckDailyAcctItemAggr::checkDailyAcctItemAggr(AuditId=0008，Type=50,低频) ..."<<endl;
	
    StatInterface ocheckDailyAcctAggr("0008",50,LOW1_FREG);  
    vector<string> vDailyAcctAggr;
    vDailyAcctAggr.clear();
    char tmpbuf[1024]={0};
    sprintf(tmpbuf,"%s","I-DR_ID|"
                        "S-PROV_CODE|"
                        "I-DATA_SOURCE|"
                        "S-ACCT_ITEM_TYPE_NAME|"
                        "I-FEE_TYPE|"
                        "I-RECORD_CNT|"
                        "I-SETT_FEE|"
                        "S-STAT_DATE");                        
    vDailyAcctAggr.push_back(string(tmpbuf));    
    
   	char sLineBuf[MaxLineLength] = {0};  
   	char sAcctItemName[100] = {0};

	  strncpy(m_sAcctItemDate, ocheckDailyAcctAggr.sEndDate, 8);
    cout<<m_sAcctItemDate<<endl;

    DEFINE_QUERY(qry);
    qry.setSQL(" delete CHECK_DAILY_ACCT_ITEM_AGGR_LOG where acct_item_date >= :acct_item_date ");
    qry.setParameter("acct_item_date", m_sAcctItemDate);
    qry.execute();
    qry.commit();
    qry.close();
    	
	if(!initParam())
	{
    	Log::log(0, "checkDailyAcctItemAggrNew");		
    	//return -1;	
	}
	
	BillingCycleMgr bcm;                                           
    BillingCycle *bc = bcm.getBillingCycle(m_iBillingCycleID);
    
    bool bCycleEnd = false;
    char *pCyclceEndDate = bc->getEndDate();
	Date d1(ocheckDailyAcctAggr.sEndDate);
	Date d2(pCyclceEndDate);
	    
    if ( d2.diffStrictDay(d1) == 1 )
    {
    	bCycleEnd = true;
    }
    

#ifdef DEF_JIANGSU
	statDailyFromTicket(bCycleEnd);
#endif	
	statDailyAcctAggr(bCycleEnd);//统计日清单
	
    CalcDailyAggr();//计算日帐以及截至目前的总账
    
    vector<StatResultInfo *>::iterator iter;
    for (iter = m_poStatInfoBuf.begin(); iter != m_poStatInfoBuf.end(); ++iter) 
	{
    	memset(sAcctItemName, 0, sizeof(sAcctItemName));	
    	
    	switch((*iter)->m_iAcctItemGroupID) 
    	{
    		case 1:
    			strcpy(sAcctItemName, "详单费用");
    			break;
    		case 2:
    			strcpy(sAcctItemName, "周期性费用");
    			break;
    		case 3:
    			strcpy(sAcctItemName, "代收费用");
    			break;
    		case 4:
    			strcpy(sAcctItemName, "一次性费用");
    			break;    			    			    			
    		case 5:
    			strcpy(sAcctItemName, "补收抵减");
    			break;    			
    		case 6:
    			strcpy(sAcctItemName, "帐务优惠费用");
    			break;
    		case 7:
    			strcpy(sAcctItemName, "其他费用");
    			break;
    		default:
    			strcpy(sAcctItemName, "其他费用");
    			break;    			    			    					
    	}  	
		
    	memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    	sprintf(sLineBuf, "%d|%d|%d|%s|%d|%ld|%ld|%s", DR_ID, m_iProveNum,(*iter)->m_iDataSource, sAcctItemName, (*iter)->m_iAcctItemGroupID, (*iter)->m_lDailyRecord, 
													(*iter)->m_lCharge, ocheckDailyAcctAggr.sEndDate);
		vDailyAcctAggr.push_back(sLineBuf);												    			
	}
	 /*
    {
    	memset(sLineBuf, 0, sizeof(sLineBuf));	 
      memset(sAcctItemName, 0, sizeof(sAcctItemName));	    	  	
    	strcpy(sAcctItemName, "详单费用");
    	sprintf(sLineBuf, "%d|25|25|%s|%d|%ld|%ld|%s", DR_ID, sAcctItemName,  1,100, 
													25000,"20101003045000");
			vDailyAcctAggr.push_back(sLineBuf);										
    	memset(sLineBuf, 0, sizeof(sLineBuf));	
      memset(sAcctItemName, 0, sizeof(sAcctItemName));	    	  													
    	strcpy(sAcctItemName, "周期性费用");
    	sprintf(sLineBuf, "%d|25|25|%s|%d|%ld|%ld|%s", DR_ID, sAcctItemName,2, 10, 
													10000,"20101003045000"); 		
      vDailyAcctAggr.push_back(sLineBuf);	
    }	
    */

  																						    	
   // ocheckDailyAcctAggr.writeFile(vDailyAcctAggr);    
    ocheckDailyAcctAggr.writeJsonFile(vDailyAcctAggr,SPLIT_FLAG); 
	  writeDailyAcctAggrLog();//日志入库
    DB_LINK->commit();
	
	return 1;	
}

/*
 *	函 数 名 : checkMonthAcctItemAggrNew
 *	函数功能 : 提取记录月帐处理后的相关日志信息，主要用于当月的帐目费用准确性校验，按月出,AuditId=0010，Type=50,低频2
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
int CheckDailyAcctItemAggr::checkMonthAcctItemAggrNew()
{
    cout <<"CheckDailyAcctItemAggr::checkMonthAcctItemAggr(AuditId=0010，Type=50,低频2) ..."<<endl;
	
    StatInterface ocheckMonthAcctAggr("0010",50,LOW2_FREG);  
    vector<string> vMonthAcctAggr;
    vMonthAcctAggr.clear();	
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
    vMonthAcctAggr.push_back(string(tmpbuf));        
   	char sLineBuf[MaxLineLength] = {0};
   	
   	statMonthAcctAggr();
   	
   	CalcMonthAggr();
   	
   	char sCycle[8] = {0};
	  BillingCycleMgr bcm;                                           
    BillingCycle *bc = bcm.getBillingCycle(m_iBillingCycleID);
    
    strncpy(sCycle, bc->getStartDate(), 6);

	for (m_mIter = m_mapMonthAggr.begin(); m_mIter != m_mapMonthAggr.end(); ++m_mIter)
	{
		if (m_mIter->second->m_lTicketFee != 0)
		{	
    		memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    		sprintf(sLineBuf, "%d|%d|%d|%s|%d|%ld|%ld|%s|%s", DR_ID, m_iProveNum,m_mIter->second->m_iDataSource, "详单费用", 1, m_mIter->second->m_lTicketRecord, 
													m_mIter->second->m_lTicketFee, sCycle, ocheckMonthAcctAggr.sEndDate);
			vMonthAcctAggr.push_back(sLineBuf);	
		}
		
		if (m_mIter->second->m_lCycleFee != 0)
		{	
    		memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    		sprintf(sLineBuf, "%d|%d|%d|%s|%d|%ld|%ld|%s|%s", DR_ID,m_iProveNum, m_mIter->second->m_iDataSource, "周期性费用", 2, m_mIter->second->m_lDisctRecord, 
													m_mIter->second->m_lCycleFee, sCycle, ocheckMonthAcctAggr.sEndDate);
			vMonthAcctAggr.push_back(sLineBuf);
		}
		
		if (m_mIter->second->m_lDisctFee != 0)
		{	
    		memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    		sprintf(sLineBuf, "%d|%d|%d|%s|%d|%ld|%ld|%s|%s", DR_ID,m_iProveNum, m_mIter->second->m_iDataSource, "帐务优惠费用", 6, m_mIter->second->m_lDisctRecord, 
													m_mIter->second->m_lDisctFee, sCycle, ocheckMonthAcctAggr.sEndDate);
			vMonthAcctAggr.push_back(sLineBuf);
		}	
		
		if (m_mIter->second->m_lOffsetFee != 0)
		{	
    		memset(sLineBuf, 0, sizeof(sLineBuf));	   	
    		sprintf(sLineBuf, "%d|%d|%d|%s|%d|%ld|%ld|%s|%s", DR_ID, m_iProveNum,m_mIter->second->m_iDataSource, "补收抵减", 5, m_mIter->second->m_lOffsetRecord, 
													m_mIter->second->m_lOffsetFee, sCycle, ocheckMonthAcctAggr.sEndDate);
			vMonthAcctAggr.push_back(sLineBuf);
		}						
	}   
	/*
	{
	    memset(sLineBuf, 0, sizeof(sLineBuf));	 
    	sprintf(sLineBuf, "%d|25|25|%s|%d|%d|%ld|%ld|%s", DR_ID, "详单费用", 2010, 1,100, 
													25000,"20101003045000");
			vMonthAcctAggr.push_back(sLineBuf);										
    	memset(sLineBuf, 0, sizeof(sLineBuf));	
    	sprintf(sLineBuf, "%d|25|25|%s|%d|%d|%ld|%ld|%s", DR_ID, "周期性费用", 1010,2 ,10, 
													10000,"20101003045000"); 		
      vMonthAcctAggr.push_back(sLineBuf);	
	}
	*/
   // ocheckMonthAcctAggr.writeFile(vMonthAcctAggr);    
    ocheckMonthAcctAggr.writeJsonFile(vMonthAcctAggr,SPLIT_FLAG);
	  writeMonthAcctAggrLog();//日志入库	
    DB_LINK->commit();
	
	return 1;	
}
