#include "CheckStatInterface.h"

//PlanItem * StatInterface::m_pPlan = NULL;


  
long StatInterface::insert (bool bCommit) 
{
    lStatID = getStatID ();
    
    adjustDimensions ();
    
    DEFINE_QUERY (qry);
    qry.setSQL ("insert into Stat_Interface ( "
        " STAT_ID         , "
        " KPI_CODE        , "
        " VALUE           , "
        " STAT_BEGIN_DATE , "
        " STAT_END_DATE   , "
        " STATE           , "
        " AREA_CODE       , "
        " CREATE_DATE     , "
        " DEAL_FLAG       , "
        " DEAL_DATE       , "
        " MODULE_ID       , "
        " DIMENSIONS      , "
        " DIMENSION_FLAG  , "
        " STAT_DATE_FLAG  , "
        " DB_SOURCE_ID    , "
        " MIN_STAT_DATE   , "
        " MAX_STAT_DATE   ) "
        " values (:vStatID, :vKpiID, :vValue, "
        "  to_date(:vStatBeginDate,'yyyymmddhh24miss'), "
        "  to_date(:vEndDate, 'yyyymmddhh24miss'), "
        "  :vState, :vAreaCode, to_date(:vCreateDate,'yyyymmddhh24miss'),"
        "  :vDealFlag, sysdate, "
        "  :vModID, :vDimensions, :vDimFlag, :vStatDateFlag,  "
        "  :vDbSourceID, sysdate, sysdate)");
    qry.setParameter ("vStatID", lStatID);
    qry.setParameter ("vKpiID", sKpiCode);
    qry.setParameter ("vValue", sValue);
    qry.setParameter ("vStatBeginDate", sStatBeginDate);
    qry.setParameter ("vEndDate", sEndDate);
    qry.setParameter ("vState", iState);
    qry.setParameter ("vAreaCode", sAreaCode);
    qry.setParameter ("vCreateDate", sCreateDate);
    qry.setParameter ("vDealFlag", iDealFlag);
    qry.setParameter ("vModID", iModuleID);
    qry.setParameter ("vDimensions", sDimensions);
    qry.setParameter ("vDimFlag", iDimensionFlag);
    qry.setParameter ("vStatDateFlag", iStatDateFlag);
    qry.setParameter ("vDbSourceID", iDBSourceID);
    
    qry.execute ();
    if (bCommit) qry.commit ();
    qry.close ();
    
    return lStatID;
}

long StatInterface::getStatID () 
{
    DEFINE_QUERY (qry);
    qry.setSQL ("select SEQ_MNT_STAT_ID.Nextval from dual");
    qry.open ();  qry.next ();
    long lId = qry.field(0).asLong();
    qry.close ();
    return lId;
}
    
int StatInterface::getDefaultOrgID()
{
  DEFINE_QUERY(qry);
  qry.setSQL (" select nvl(max(org_id),:vRootOrgID) from org where parent_org_id is null ");
  qry.setParameter ("vRootOrgID", CHECK_ROOT_ORG_ID);
  qry.open();
  if (qry.next())
  {
    return qry.field(0).asInteger();
  } 
  
  return 1;     
}
/*
void StatInterface::adjustAreaCodeByOrgID (const int iOrgID)
{
    static HashList<string> *s_pOrgAreaHash = 0x0;
    static OrgMgr *s_pOrgMgr = 0x0;
    
    if (!s_pOrgMgr) {
        s_pOrgMgr = new OrgMgr();
    }
    if (!s_pOrgAreaHash) {
        s_pOrgAreaHash = new HashList<string> (13);
        DEFINE_QUERY (qry);
        qry.setSQL ("select org_id, area_code from check_org_area_relation");
        qry.open ();
        while (qry.next())
        {
            long lOrgID = qry.field(0).asLong();
            string strAreaCode (qry.field(1).asString());
            s_pOrgAreaHash->add (lOrgID, strAreaCode);
        }
        qry.close();
    }
    
    long lTmpOrgID = iOrgID;
    string strTmp;
    while (lTmpOrgID > 0) {            
        if (s_pOrgAreaHash->get (lTmpOrgID, &strTmp)) {
            strcpy (sAreaCode, strTmp.c_str());
            break;
        }
        lTmpOrgID = s_pOrgMgr->getParentID(lTmpOrgID);
    }
}
*/
void StatInterface::getAreaBySwitch (int _iSwitchID, char *_sAreaCode) 
{
    static HashList<string> *s_pSwitch = 0x0;
    
    if (!s_pSwitch) 
    {
        s_pSwitch = new HashList<string> (37);
        
        DEFINE_QUERY (qry);
        qry.setSQL ("select switch_id, nvl(area_code,:vAreaCode) from b_switch_info");
        qry.setParameter ("vAreaCode", m_sDefaultAreaCode);
        qry.open(); 
        while (qry.next())
        {
            string strAreaCode (qry.field(1).asString());
            s_pSwitch->add (qry.field(0).asLong(), strAreaCode);
        }
        qry.close();
    }
    
    string strTmp;
    if (s_pSwitch->get (_iSwitchID, &strTmp))
        strcpy (_sAreaCode, strTmp.c_str());
    else
        strcpy (_sAreaCode, m_sDefaultAreaCode);
}

int StatInterface::getOrgBySwitch (int _iSwitchID) 
{
    static HashList<int> *s_pSwitch = 0x0;
    
    if (!s_pSwitch) 
    {
        s_pSwitch = new HashList<int> (37);
        
        DEFINE_QUERY (qry);
        qry.setSQL ("select switch_id, nvl(org_id, -1) from check_switch_org");
        qry.open(); 
        while (qry.next())
        {
            s_pSwitch->add (qry.field(0).asLong(), qry.field(1).asInteger());
        }
        qry.close();
    }
    
    int i;
    if (s_pSwitch->get (_iSwitchID, &i))
    {
      return i;
    } 
    else
    {
      return -1;
    } 
} 

int StatInterface::getSwitchByFileID (int _iFileID) 
{
    static HashList<int> *s_pSwitch = 0x0;
    
    if (!s_pSwitch) 
    {
        s_pSwitch = new HashList<int> (5000);
        
        DEFINE_QUERY (qry);
        qry.setSQL (" SELECT file_id, switch_id FROM b_event_file_list_cfg WHERE switch_id > 0 "); 
        qry.open(); 
        while (qry.next())
        {
            s_pSwitch->add (qry.field(0).asLong(), qry.field(1).asInteger());
        }
        qry.close();
    }
    
    int i;
    if (s_pSwitch->get (_iFileID, &i))
    {
      return i;
    } 
    else
    {
      return -1;
    } 
}       

//## 四川区分A、B大区(表里用1、2表示), 在Dimensions域增加大区标志
void StatInterface::adjustDimensions ()
{        
    char sRegion[10];
    #ifdef CHECK_REGION_DEFINE  //in LocalDefine.h  etc.(#define CHECK_REGION_DEFINE "2")
    sprintf (sRegion, ";%s", CHECK_REGION_DEFINE);
    #else
    sRegion[0] = 0;
    #endif
    
    strcat (sDimensions, sRegion);
}

void StatInterface::writeFile(vector<string> &v)
{
	if (createFile())
	{
		writeFileHead(v.size());
		
		if (!v.empty())
		{
			vector<string>::iterator iter;
			for (iter = v.begin(); iter != v.end(); iter++)
			{
				writeFileInfo((*iter).c_str());
			}
		}	
		
		writeFileEnd();
	}    	
}

bool StatInterface::createFile()
{
  if (!setFullFlieName())
  {
	 Log::log(0, "文件名生成失败,请检查配置");
	 return false;
  }

  m_oOfstream.open(m_sFullFileName);
  if (!m_oOfstream.good())
  {
	return false;
  }

  return true;
}
    
bool StatInterface::writeFileHead(long lRecordCnt)
{
	m_lRecord = lRecordCnt;
	
	if (!m_oOfstream.good())
	{
		return false;
	}		
			
	m_oOfstream<<"STA"<<'/'<<m_lRecord<<endl;
	
	return true;			  
}

bool StatInterface::writeFileEnd()
{
	if (!m_oOfstream.good())
		return false;
			
	m_oOfstream<<"END"<<endl;
	m_oOfstream.close();	
	m_oOfstream.clear();
	
	if (!renameFileName())
		return false;
	
	writeFileLog();				
	
	return true;
}

bool StatInterface::writeFileInfo(const char* sBuf)
{
	if (!m_oOfstream.good())
	{
		return false;
	}
			
	m_oOfstream<<sBuf<<endl;	
	
	return true;
}	
  
bool StatInterface::setFullFlieName()
{
  if (!setFileDate())
  		return false;
  		
  if (!setFileDirectory())
  		return false;    		
  
  if (!setFileSeq())
  		return false;   
  		
  if (!setProvinceCode())
  		return false;       
  		
  	setCityCode();//不强制要求			
  		
  if (!setFileName())
  		return false;
  		
  	memset(m_sFullFileName, 0, sizeof(m_sFullFileName));
  	
  	sprintf(m_sFullFileName, "%s%s", m_sDirectory, m_sFileName);

return true;
}

bool StatInterface::setFileDirectory()
{
	memset(m_sDirectory, 0, sizeof(m_sDirectory));
	
  if (strlen(m_pPlan->m_sDirectory) <= 0)
  {
	Log::log(0, "为配置Plan_id为:%d的文件生成路径请检查B_Check_Stat_Plan配置", m_pPlan->m_iPlanID);			
	return false;
  }

strncpy(m_sDirectory, m_pPlan->m_sDirectory, 1000);

    if (m_sDirectory[strlen(m_sDirectory)-1] != '/')
    {
    	strcat(m_sDirectory, "/");
    }

   	//strcat(m_sDirectory, m_sFileDate);
   	//strcat(m_sDirectory, "/");        
           
	DIR*    dp;
	if ( ( dp = opendir( m_sDirectory ) ) == NULL )
	{   ///不能打开目录,试图创建该目录
    	if ( mkdir( m_sDirectory, 0777 ) != 0 )
   		{
        	Log::log( 0, "目录创建失败!(%s),请手工创建", m_sDirectory );
        	return false;
    	}

	}		
return true;
}

bool StatInterface::setFileDate()
{
	memset(m_sFileDate, 0, sizeof(m_sFileDate));
	
	/*
	if (m_pPlan->m_iDataType == 0)
	{	
		strncpy(m_sFileDate, m_pPlan->m_sDataBeginTime, 8);
	}else
	{
		strncpy(m_sFileDate, m_pPlan->m_sDataEndTime, 8);
	}
	*/
		strncpy(m_sFileDate, m_pPlan->m_sDataBeginTime, 8);	
	return true;
}

bool StatInterface::setFileSeq()
{
	//按天、按月周期直接置成1
	if ((m_pPlan->m_iIntervalType == 1&&atoi(m_pPlan->m_sTimeInterval) >= 86400)
		||m_pPlan->m_iIntervalType != 1)
	{
		m_iFileSeq = 1;
		return true;
	}
		
	//其他的走数据库，防止出现异常的情况
	DEFINE_QUERY(qry);
	
	qry.setSQL(" select nvl(max(file_seq), 0) from B_Check_Stat_File_log t \
			 where t.plan_id = :plan_id \
				 and t.check_file_date = to_date(:file_date, 'yyyymmdd') ");
				 
    qry.setParameter ("plan_id", m_pPlan->m_iPlanID);
    qry.setParameter ("file_date", m_sFileDate);
    
    qry.open();
    
    if (qry.next())
    {
    	m_iFileSeq = 1 + qry.field(0).asInteger();
    }else
    {
    	m_iFileSeq = 1;
    }
    
    qry.close();
    
    return true;		 
}

bool StatInterface::setProvinceCode()
{
	char sValue[1001] = {0};
	memset(m_sProvinceCode, 0, sizeof(m_sProvinceCode));
	
	if (!ParamDefineMgr::getParam("CHECK_STAT", "PROVINCE_CODE", sValue, -1))
	{
		return false;
	}
	
	strncpy(m_sProvinceCode, sValue, 3);
	
	return true;
}

bool StatInterface::setCityCode()
{
	char sValue[1001] = {0};
	memset(m_sCityCode, 0, sizeof(m_sCityCode));
	
	if (!ParamDefineMgr::getParam("CHECK_STAT", "CITY_CODE", sValue, -1))
	{
		return false;
	}
	
	strncpy(m_sCityCode, sValue, 4);
	
	return true;
}   

const char *StatInterface::getCityCode()
{
	return m_sCityCode;
}	
    
bool StatInterface::setFileName()
{
	memset(m_sFileName, 0, sizeof(m_sFileName));
	
	if (m_sCityCode[0] == '\0')
	{	
		sprintf(m_sFileName, "%s_%d_%d_%04s_%d_%02s_%s_%03s_%08d_%03d.json",
		                   BOSSNM, SYS_FLAG, NEID, m_sAuditId, m_iType,m_sFreq ,m_sFileDate ,
		                   m_sProvinceCode,m_iFileSeq,m_iDuplNum);
	}else
	{
		sprintf(m_sFileName, "%s_%d_%d_%04s_%d_%02s_%s_%03s_%04s%04d_%03d.json",
		            BOSSNM, SYS_FLAG, NEID, m_sAuditId, m_iType,m_sFreq ,m_sFileDate ,
		           m_sProvinceCode,m_sCityCode,m_iFileSeq,m_iDuplNum);
	}		
	
	return true;
}

bool StatInterface::renameFileName()
{
	char sTmpFileName[200] = {0};
	char sTmpFullFileName[1200] = {0};
	strcpy(sTmpFullFileName, m_sFullFileName);
	strcpy(sTmpFileName, m_sFileName);
	
  m_sFileName[strlen(sTmpFileName)-4] = '\0';
  m_sFullFileName[strlen(sTmpFullFileName)-4] = '\0';

  if (rename(sTmpFullFileName, m_sFullFileName) != 0)
  {
	  if (access (sTmpFullFileName, 0) == 0)
	  {	
		  unlink (sTmpFullFileName);
	  }
	
	  if (access (sTmpFullFileName, 0) == 0) 
	  {
	  	Log::log (0, "删除文件[%s]失败", sTmpFullFileName);
	  }

	  return false;
  }

  return true;
}

void StatInterface::writeFileLog()
{
	DEFINE_QUERY(qry);
	
	qry.setSQL(" insert into B_Check_Stat_File_log \
			 (file_log_id, plan_id, file_name, file_seq, check_file_date, \
                 stat_begin_date, stat_end_date, create_date, record_cnt) \
                 values \
                 (SEQ_Check_Stat_File_log_id.Nextval, :plan_id, :file_name, :file_seq, to_date(:check_file_date, 'yyyymmdd'), \
                 to_date(:stat_begin_date, 'yyyymmddhh24miss'), to_date(:stat_end_date, 'yyyymmddhh24miss'), sysdate, :record_cnt) ");
                 
    qry.setParameter("plan_id", m_pPlan->m_iPlanID);
    qry.setParameter("file_name", m_sFileName);
    qry.setParameter("file_seq", m_iFileSeq);
    qry.setParameter("check_file_date", m_sFileDate);
    qry.setParameter("stat_begin_date", m_pPlan->m_sDataBeginTime);
    qry.setParameter("stat_end_date", m_pPlan->m_sDataEndTime);
    qry.setParameter("record_cnt", m_lRecord);
    /*
    char sSQL[1024]={0};
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"insert into B_Check_Stat_File_log "
			         " (file_log_id, plan_id, file_name, file_seq, check_file_date,"
               "  stat_begin_date, stat_end_date, create_date, record_cnt) "
               "  values "
               "  (SEQ_Check_Stat_File_log_id.Nextval,%d,'%s', %d, to_date('%s', 'yyyymmdd'), "
               "  to_date('%s', 'yyyymmddhh24miss'), to_date('%s', 'yyyymmddhh24miss'), sysdate, %ld) ",
	               m_pPlan->m_iPlanID,m_sFileName,m_iFileSeq ,m_sFileDate,m_pPlan->m_sDataBeginTime,
               	m_pPlan->m_sDataEndTime,m_lRecord);
	
	qry.setSQL(sSQL);    
	*/
    qry.execute();
    qry.commit();                    
}
  
int StatInterface::getBusiclassBySwitch(int iSwitchID)	
{
      static HashList<int> *s_pSwitch = 0x0;
      
      if (!s_pSwitch) 
      {
          s_pSwitch = new HashList<int> (5000);
          
          DEFINE_QUERY (qry);
          qry.setSQL (" Select switch_id, decode(source_event_type, 3, 20, "
                                         " 4, 20, "
                                         " 6, 60, "
                                         " 7, 30, "
                                         " 9, 50, "
                                         " 11, 20, "
                                             " 10) busi_class from b_switch_info "); 
          qry.open(); 
          while (qry.next())
          {
              s_pSwitch->add (qry.field(0).asInteger(), qry.field(1).asInteger());
          }
          qry.close();
      }
      
      int i=0;
      if (s_pSwitch->get (iSwitchID, &i))
      {
        return i;
      } 
      else
      {
        return 10;//默认语音业务
      } 		
}

int StatInterface::getDataSourceBySwitch(int iSwitchID)	
{
      static HashList<int> *s_pSwitch = 0x0;
      
      if (!s_pSwitch) 
      {
          s_pSwitch = new HashList<int> (5000);
          
          char sAreaCode[5] = {0};
          int iDataSource = 0;
          
          DEFINE_QUERY (qry);
          qry.setSQL (" select switch_id, nvl(area_code, 0) area_code from b_switch_info "); 
          qry.open(); 
          while (qry.next())
          {
          	strncpy(sAreaCode, qry.field(1).asString(), 4);
          	
          	if (strcmp(sAreaCode, "0") == 0)
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
          		
              s_pSwitch->add (qry.field(0).asInteger(), iDataSource);
          }
          qry.close();
      }
      
      int i=0;
      if (s_pSwitch->get (iSwitchID, &i))
      {
        return i;
      } 
      else
      {
        return atoi(m_sDefaultAreaCode+1);//默认城市
      } 		
}	
    
 void StatInterface::ChangeFullFileName(int i ,char *m_sFileName)
{
	//strstr(m_sFullFileName,".json") ;
	strncpy(m_sFileName,m_sFullFileName,strlen(m_sFullFileName)-5) ;
	sprintf(m_sFileName,"%s.%04d.json",m_sFileName,i) ;
}
    
//add by sunjy for 2.8 test
/*
 *	函 数 名 : writeJsonFile
 *	函数功能 : 把信息点数据写成json格式文件
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
void StatInterface::writeJsonFile(vector<string> &v,char cTag)
{
	  //writeFile(v);
	 char mFileSize[20]={0} ;
	static  int MaxFileSize = 0 ;
    if(!ParamDefineMgr::getParam("MEMORY_DB","MaxFileSize",mFileSize))
		MaxFileSize = 15*1024*1024*0.8 ;
	else
		MaxFileSize = atoi(mFileSize)*0.8 ;
	  m_lRecord=v.size()-1;
	if (!setFullFlieName())
  {
	 Log::log(0, "文件名生成失败,请检查配置");
	 return ;
  }
  vector<string> vt ;
  vector<string>::iterator iterV = v.begin() ;
  for(;iterV!=v.end();iterV++) {
  	vt.push_back((*iterV).c_str()) ;
  }
  	
  JsonMgr *pJosnMgr =new JsonMgr();  
  long filesize = pJosnMgr->writeJsonFile(m_sFullFileName,v,cTag) ;
  if(filesize == -1)
  {
	 Log::log(0, "写json文件失败，请检查");
	 return ;
  }
  if( filesize > MaxFileSize ) {
  	 char tCommand[512] ={0} ;
  	sprintf(tCommand,"rm %s" ,m_sFullFileName) ;
  	system(tCommand) ;
  	int num = 0 ;
  	char m_sFullFileName_split[200] ;
  	num = (filesize/(MaxFileSize))+1 ;
  	int record = (vt.size()-1)/num ;
  	//vector<string> m_sVecSplit ;
  	vector<string>::iterator iter=vt.begin() ;
  	char sLine[512]={0} ;
  	strcpy(sLine,(*iter).c_str()) ;
  	for(int i = 0; i<num ;i++) {
  		memset(m_sFullFileName_split,0,sizeof(m_sFullFileName_split)) ;
  		ChangeFullFileName(  (i+1) , m_sFullFileName_split) ;
  		if(i !=(num -1))   {
  		     vector<string> m_sVecSplit((iter+i*record+1),(iter+(i+1)*record+1)) ;
  		     m_sVecSplit.insert(m_sVecSplit.begin() , string(sLine)) ;
	  		if((pJosnMgr->writeJsonFile(m_sFullFileName_split,m_sVecSplit,cTag))==-1) 
	  		{
				 Log::log(0, "写json文件失败，请检查");
				 return ;
			 }
		}
  		else {
  			vector<string> m_sVecSplit((iter+i*record+1),vt.end()) ;
	  		m_sVecSplit.insert(m_sVecSplit.begin() , string(sLine)) ;
	  		if((pJosnMgr->writeJsonFile(m_sFullFileName_split,m_sVecSplit,cTag) )==-1 ) 
	  		{
				 Log::log(0, "写json文件失败，请检查");
				 return ;
			 }
		}
  		insertFileList(m_sAuditId,m_iType,m_sFullFileName_split);	  
  	}

  }
  else
  	insertFileList(m_sAuditId,m_iType,m_sFullFileName);
 
  //renameFileName();
  delete pJosnMgr;
  	writeFileLog();		  
    
    
  /* char sValue1[3+1]={0};
   ParamDefineMgr::getParam("INFO_DATA", "FILE_PATH", sValue1);
   int iValue=atoi(sValue1);
   if(iValue == 1){            
    char scmd[1024]={0};
    char tempbak[1024]={0};
    sprintf(tempbak,"%s%s",m_sDirectory,"BAK");
  	DIR*    dp;
	  if ( ( dp = opendir( tempbak ) ) == NULL )
	  {   ///不能打开目录,试图创建该目录
    	if ( mkdir( tempbak, 0777 ) != 0 )
   		{
        	Log::log( 0, "备份目录创建失败!(%s),请手工创建", m_sDirectory );
        	return ;
    	}

	  }	  
    sprintf(scmd,"cp -f %s %s",m_sFullFileName,tempbak);
    system(scmd);
  
    memset(scmd,0,sizeof(scmd));
    sprintf(scmd,"mv -f %s %s",m_sFullFileName,m_sDesFielPath);
    system(scmd);
  }
    */
}

/*
 *	函 数 名 : writeJsonFile
 *	函数功能 : 把信息点数据写成json格式文件
 *	时    间 : 2010年9月
 *	返 回 值 : 
 *	参数说明 : 
*/
void StatInterface::insertFileList(char *sAutid,int iType,char *sFileName)
{
	  Date dt;
    DEFINE_QUERY (qry);
    qry.setSQL ("insert into b_info_file_list ( "
           "file_list_id ,"
           "audit_id  ,"
           "type_id  ,"
           "file_name ,"
           "state_time )"
        " values (seq_info_file_list.NEXTVAL, :sAuditID, :iTypeID, :sFileName ,"
        "  to_date(:state_time,'yyyymmddhh24miss') )");
    qry.setParameter ("sAuditID", sAutid);
    qry.setParameter ("iTypeID",  iType);
    qry.setParameter ("sFileName", sFileName);
    qry.setParameter ("state_time", dt.toString());
    
    qry.execute ();
    qry.commit ();
    qry.close ();	
}

