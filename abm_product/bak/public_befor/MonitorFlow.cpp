#include "MonitorFlow.h"

//监控开账期，账期末优惠，下账，将信息插入到江苏各地统计库stat_interface表中


MonitorFlow::MonitorFlow()
{
    getDefaultLogin();
}

MonitorFlow::~MonitorFlow()
{
    try {
       gpDBLinkMF.disconnect ();
    } catch (...) {
	
    };
}

void MonitorFlow::getDefaultLogin()
{
    char * p;
    
    if ((p=getenv ("TIBS_HOME")) == NULL)
      sprintf (sHome, "/bill/rtbill/TIBS_HOME/");
    else
      sprintf (sHome, "%s", p);
    
    ReadIni reader;
    char sFile[254];
    
    if (sHome[strlen(sHome)-1] == '/') {
      sprintf (sFile, "%setc/data_access_new.ini", sHome);
    } 
    else {
      sprintf (sFile, "%s/etc/data_access_new.ini", sHome);
    }
    
    reader.readIniString (sFile, "STAT", "username", m_sDBUser, "");
    
    char sTemp[254];		
    reader.readIniString (sFile, "STAT", "password", sTemp,"");
    decode(sTemp,m_sDBPwd);
    
    reader.readIniString (sFile, "STAT", "connstr", m_sDBStr, "");
    
    gpDBLinkMF.connect(m_sDBUser,m_sDBPwd,m_sDBStr);
    m_bConnect = true;

    TOCIQuery qry(&gpDBLinkMF);
    
    qry.setSQL("select AREA_CODE from s_local_define");
    qry.open(); qry.next();
    strcpy(m_sAreaCode,qry.field(0).asString());
    qry.close();

}


long MonitorFlow::getNextStatSeq()
{
    static TOCIQuery qry(&gpDBLinkMF);
    
    qry.close();
    qry.setSQL("select seq_mnt_stat_id.nextval stat_id from dual");
    qry.open();
    if (!qry.next())
    {
    	cout << "取统计库seq_mnt_stat_id序列出错" << endl;
    	return 0;
    }
    
    long m_lCurStatID = qry.field("stat_id").asLong();
    
    qry.close();
    
    return m_lCurStatID;
}

int MonitorFlow::MonitorBillingCycle(int iBillingCycleID,char *sState)
{
    if (!m_bConnect)
    {
       getDefaultLogin();
       m_bConnect = true;
    }
    
    char sSql[1024];
    Date runDate;
    long m_StatID = getNextStatSeq();

    memset(m_DIMENSIONS,0,1024);
    if (sState[2] == 'R')
       sprintf(m_DIMENSIONS,"JF_CZ_FW_01;%d;%s;1;%s",iBillingCycleID,m_sAreaCode,runDate.toString());
    else if ( sState[2] == 'A' )
       sprintf(m_DIMENSIONS,"JF_CZ_FW_02;%d;%s;1;%s",iBillingCycleID,m_sAreaCode,runDate.toString());
    else if ( sState[2] == 'E' )
       sprintf(m_DIMENSIONS,"JF_CZ_FW_05;%d;%s;1;%s",iBillingCycleID,m_sAreaCode,runDate.toString());


   try{
    
    TOCIQuery qry(&gpDBLinkMF);

    sprintf(sSql,
              " insert into stat_interface(stat_id,kpi_code,value,stat_begin_date,stat_end_date,state, "
              " area_code,create_date,deal_flag,module_id,dimensions, "
              " dimension_flag,stat_date_flag,db_source_id) "
              " values(%d, '%s', '1', sysdate, sysdate, '0', '%s', sysdate, 1, 1, '%s', 1, 2, 1)",
              m_StatID,"JF_FLOW_CZ_TIME",m_sAreaCode,m_DIMENSIONS); 

    qry.setSQL( sSql );
    
    qry.execute();
    qry.commit();
    qry.close();

    m_StatID = getNextStatSeq();
    memset(m_DIMENSIONS,0,1024);
    sprintf(m_DIMENSIONS,"%d;%s;%s",iBillingCycleID,m_sAreaCode,sState);
    
    sprintf(sSql,
              " insert into stat_interface(stat_id,kpi_code,value,stat_begin_date,stat_end_date,state, "
              " area_code,create_date,deal_flag,module_id,dimensions, "
              " dimension_flag,stat_date_flag,db_source_id) "
              " values(%d, '%s', '1', sysdate, sysdate, '0', '%s', sysdate, 1, 1, '%s', 1, 2, 1)",
              m_StatID,"JF_FLOW_CZ_STATE",m_sAreaCode,m_DIMENSIONS); 
    
    qry.setSQL( sSql );
    
    qry.execute();
    qry.commit();
    qry.close();
   }
   catch( ... )
   {
    cout << "执行insert into stat_interface错误，检查统计库表" << endl;
   }

}

int MonitorFlow::MonitorDiskMake(int iBillingCycleID,int mStart)
{

    if (!m_bConnect)
    {
       getDefaultLogin();
       m_bConnect = true;
    }
    
    char sSql[1024];
    Date runDate;
    long m_StatID = getNextStatSeq();
    
    if(mStart == 2)
      sprintf(m_DIMENSIONS,"JF_CZ_FW_03;%d;%s;2;%s",iBillingCycleID,m_sAreaCode,runDate.toString());
    else
    	sprintf(m_DIMENSIONS,"JF_CZ_FW_03;%d;%s;1;%s",iBillingCycleID,m_sAreaCode,runDate.toString());
    
    try{    
    TOCIQuery qry(&gpDBLinkMF);
    
    sprintf(sSql,
              " insert into stat_interface(stat_id,kpi_code,value,stat_begin_date,stat_end_date,state, "
              " area_code,create_date,deal_flag,module_id,dimensions, "
              " dimension_flag,stat_date_flag,db_source_id) "
              " values(%d, :kpi_code, '1', sysdate, sysdate, '0', '%s', sysdate, 1, 1, '%s', 1, 2, 1)",
              m_StatID,m_sAreaCode,m_DIMENSIONS); 
    
    qry.setSQL(sSql);
    qry.setParameter("kpi_code","JF_FLOW_CZ_TIME");
    
    qry.execute();
    qry.commit();
    qry.close();

    return 0;
   }
   catch (...)
   {
     cout << "执行insert into stat_interface错误，检查统计库表" << endl;
   }
}

int MonitorFlow::MonitorVirement(int iBillingCycleID,int mStart)
{
    if (!m_bConnect)
    {
       getDefaultLogin();
       m_bConnect = true;
    }
    
    char sSql[1024];
    Date runDate;
    long m_StatID = getNextStatSeq();
    
    
    try{

    if(mStart == 1 )
    {
    	char temp_DIMENSIONS[128];
    	sprintf(temp_DIMENSIONS,"JF_CZ_FW_04;%d",iBillingCycleID);
    	
    	TOCIQuery qry(&gpDBLinkMF);
    	sprintf(sSql,
    	         " select count(*) cnt from stat_interface "
    	         " where kpi_code = '%s' and substr(dimensions,0,18) = '%s'","JF_FLOW_CZ_TIME",temp_DIMENSIONS);
    	qry.setSQL( sSql );
      qry.open();qry.next();
      
      int m_Cnt = qry.field(0).asInteger();//这里简单的从表里面取锁了
      if ( m_Cnt > 0)
      {	
        qry.close();
        return 0; 
      }
    	
      sprintf(m_DIMENSIONS,"JF_CZ_FW_04;%d;%s;1;%s",iBillingCycleID,m_sAreaCode,runDate.toString());      

      sprintf(sSql,
              " insert into stat_interface(stat_id,kpi_code,value,stat_begin_date,stat_end_date,state, "
              " area_code,create_date,deal_flag,module_id,dimensions, "
              " dimension_flag,stat_date_flag,db_source_id) "
              " values(%d, '%s', '1', sysdate, sysdate, '0', '%s', sysdate, 1, 1, '%s', 1, 2, 1)",
              m_StatID,"JF_FLOW_CZ_TIME",m_sAreaCode,m_DIMENSIONS); 

      qry.close();   
      qry.setSQL(sSql);  
      
      qry.execute();
      qry.commit();
      qry.close();
      
      return 0;
    }
    else if (mStart == 2)
    {
     	sleep(1);
      int cnt = -2;
      //redirectDB ("BILL"); //需要连到计费库
      if ( (strcmp(m_sAreaCode,"0513") != 0) && (strcmp(m_sAreaCode,"0511") != 0 ) ) //南通镇江特殊处理
      {
     	  DEFINE_QUERY (qry) ;
     	  sprintf(sSql,"select count(*) cnt from b_redo_log where CUR_STATUS_ID <> 2 and INT_PARAM1 = %d",iBillingCycleID);
     	  qry.setSQL(sSql);
     	  qry.open(); qry.next();
     	  cnt = qry.field(0).asInteger();
     	  qry.close();
      }

     	if(cnt > 0)
     	{
     		return 0;
      }
      else
      {
          sprintf(m_DIMENSIONS,"JF_CZ_FW_04;%d;%s;2;%s",iBillingCycleID,m_sAreaCode,runDate.toString());
          
          char sSql[1024];
          TOCIQuery qry1(&gpDBLinkMF);
          
          sprintf(sSql,
                  " insert into stat_interface(stat_id,kpi_code,value,stat_begin_date,stat_end_date,state, "
                  " area_code,create_date,deal_flag,module_id,dimensions, "
                  " dimension_flag,stat_date_flag,db_source_id) "
                  " values(%d, '%s', '1', sysdate, sysdate, '0', '%s', sysdate, 1, 1, '%s', 1, 2, 1)",
                  m_StatID,"JF_FLOW_CZ_TIME",m_sAreaCode,m_DIMENSIONS); 
          
          qry1.setSQL(sSql);
          
          qry1.execute();
          qry1.commit();
          qry1.close();
      }
    
    }
   
   return 0;
   }
   catch (...)
   {
      cout << "执行insert into stat_interface错误，检查统计库表" << endl;
   }
     
}

bool MonitorFlow::m_bConnect = false;
