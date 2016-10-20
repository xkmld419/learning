#include "Log.h"
#include "Date.h"
//#include "CommandCom.h"
#include "Environment.h"
#include "CheckInformation.h"

//PlanItem *StatInterface::m_pPlan = 0x0; //指向当前正在执行的计划

Information::Information()
{
	
}

Information::~Information()
{
	
}

void Information::print(vector<string> vec)
{
	vector<string>::iterator testit;
	for(testit = vec.begin();testit != vec.end(); ++testit)
	{
		cout <<(*testit).c_str()<<endl;
	}
}

bool Information::init(int iFlag)
{
	m_iDelayTimeSecs=0;
	m_mode=0;
	m_iFlag=0;

	if(iFlag==MODULE_GATHER)
	{
		/*
		CommandCom *m_pCommandComm;
		m_pCommandComm =new CommandCom();
		if(!m_pCommandComm->InitClient())
		{
			Log::log(0,"核心参数初始化失败。");
			return false;
		}
		else
		{
			m_pCommandComm->GetInitParam("LOG");
  		m_pCommandComm->SubscribeCmd("LOG",MSG_PARAM_CHANGE);
			Log::m_iAlarmLevel = m_pCommandComm->readIniInteger("LOG","log_alarm_level",-1);
			Log::m_iAlarmDbLevel = m_pCommandComm->readIniInteger("LOG","log_alarmdb_level",-1);
			m_pCommandComm->GetInitParam("");
  		m_pCommandComm->SubscribeCmd("",MSG_PARAM_CHANGE);
			m_iDelayTimeSecs = m_pCommandComm->readIniInteger("","",-1);
		}
		if(m_pCommandComm)
		{
			delete m_pCommandComm;
			m_pCommandComm=NULL;
		}*/
		
		char temp1[5]={0};
		if(ParamDefineMgr::getParam("INFO_DATA", "DELAY_TIMESECS",temp1 )){
			 m_iDelayTimeSecs = atoi(temp1);
		}else{
			 m_iDelayTimeSecs = 10;	
		   Log::log (0,"[INFO_DATA][DELAY_TIMESECS][限定阀值没有配置,默认取10]");	
		}		
	}
	
	//初始化性能指标信息
	 m_mapCDRInfo.clear();

	DEFINE_QUERY(qry);
  
  qry.setSQL("select trim(code_id),trim(code_name) from b_info_code");
  qry.open();
  while (qry.next())
  {
  	m_pCodeList[qry.field(0).asString()]=qry.field(1).asString();
  	
  	int iBusi=atoi(qry.field(0).asString());
  	CDR_Info sCDRInfo;
  	memset(&sCDRInfo,0,sizeof(sCDRInfo)); 	  	
  	m_mapCDRInfo[iBusi] = sCDRInfo;
  }
	
	qry.close();
	
	switch(iFlag)
	{
		case MODULE_GATHER:
			qry.setSQL("select kpi_id,kpi_name from b_info_kpi where kpi_id like '40-30-02-01-00%'");
			break;
		case MODULE_PREP:
			qry.setSQL("select kpi_id,kpi_name from b_info_kpi where kpi_id like '40-30-02-02-01%'");
			break;
		case MODULE_PRICING:
			qry.setSQL("select kpi_id,kpi_name from b_info_kpi where kpi_id like '40-30-02-02-02%'");
			break;
		case MODULE_INSTORE:
			qry.setSQL("select kpi_id,kpi_name from b_info_kpi where kpi_id like '40-30-02-02-03%'");
			break;
		default:
			qry.setSQL("select kpi_id,kpi_name from b_info_kpi");
			break;
	}
	qry.open();
  while (qry.next())
  {
		m_pKpiList[qry.field(0).asString()]=qry.field(1).asString();
  }
  
  qry.close();
  
  return true;
}

bool Information::checkPerformance(int mode)
{
	m_mode=mode;
	string kpiId;
	string kpiName;
	string kbpId;
	string kbpName;
	string min;
	string avg;
	string max;
	string temp;
	
	for(m_mapIter= m_mapCDRInfo.begin(); m_mapIter!=m_mapCDRInfo.end() ;m_mapIter++)
	{
		//memset(&m_mapIter->second,0,sizeof(CDR_Info));
		m_mapIter->second.init();
	}
	
	 switch(m_mode)
	{
		case MODULE_GATHER:
			gatherInfo();
			break;
		case MODULE_PREP:
			prepInfo();
			break;
		case MODULE_PRICING:
			pricingInfo();
			break;
		case MODULE_INSTORE:
			indbInfo();
			break;
		default:
			break;
	}
		
 		
	map<string,string>::iterator iter;
	for(iter = m_pCodeList.begin();iter != m_pCodeList.end(); ++iter)
	{
		kbpId=iter->first;
	  int iBusi = atoi(kbpId.c_str());
		kbpName=iter->second;
		memset(m_source,0,sizeof(m_source));
		memset(m_busiclass,0,sizeof(m_busiclass));
		/*
		switch(atoi(kbpId.c_str()))
		{
			case 10:
				sprintf(m_source,"%d",10);
				sprintf(m_busiclass,"%d,%d",1,10);
				break;
			case 20:
				sprintf(m_source,"%d",20);
				sprintf(m_busiclass,"%d,%d,%d",3,4,11);
				break;
			case 30:
				sprintf(m_source,"%d",30);
				sprintf(m_busiclass,"%d",7);
				break;
			case 40:
				sprintf(m_source,"%d",40);
				sprintf(m_busiclass,"%d",6);
				break;
			case 50:
				sprintf(m_source,"%d",50);
				sprintf(m_busiclass,"%d",9);
				break;
			case 60:
				sprintf(m_source,"%d",60);
				sprintf(m_busiclass,"%d",6);
				break;
			default:
				sprintf(m_source,"%d",10);
				sprintf(m_busiclass,"%d,%d",1,10);
				break;
		}
		*/
		kbpId=string("/HB/")+kbpId;
	
    m_mapIter = m_mapCDRInfo.find(iBusi);
		map<string,string>::iterator iter1;
		m_mapIter->second.parse();
		for(iter1 = m_pKpiList.begin();iter1 != m_pKpiList.end(); ++iter1)
		{
			kpiId=iter1->first;
			kpiName=iter1->second;
			temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|");
			memset(buf,0,sizeof(buf));
			switch(m_mode)
			{
				case 1:
				{
					//计费接收
					if(!strcmp(kpiId.c_str(),"40-30-02-01-00-001"))
					{
						m_iFlag=1;
						sprintf(buf,"%ld",m_mapIter->second.recvAllFileCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-01-00-002"))
					{
						m_iFlag=1;
						sprintf(buf,"%ld",m_mapIter->second.recvDelayFileCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-01-00-003"))
					{
						m_iFlag=1;
						if(m_mapIter->second.recvAllFileCount)
						  ratio=(float)m_mapIter->second.recvDelayFileCount/m_mapIter->second.recvAllFileCount;
						else 
							ratio =0 ;
						sprintf(buf,"%.3f",ratio);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-01-00-004"))
					{
						m_iFlag=1;
						sprintf(buf,"%ld",m_mapIter->second.recvDelayRecordCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-01-00-005"))
					{
						m_iFlag=1;
						sprintf(buf,"%ld",m_mapIter->second.recvAllRecordCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-01-00-006"))
					{
						m_iFlag=1;
						if(m_mapIter->second.recvAllRecordCount)
						   ratio=(float)m_mapIter->second.recvDelayRecordCount/m_mapIter->second.recvAllRecordCount;
						else 
							 ratio = 0 ;
						sprintf(buf,"%.3f",ratio);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-01-00-007"))
					{
						m_iFlag=1;
  					sprintf(buf,"%ld",m_mapIter->second.recvErrFileCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-01-00-008"))
					{
						m_iFlag=1;
						if(m_mapIter->second.recvAllFileCount+m_mapIter->second.recvErrFileCount)
						   ratio=(float)m_mapIter->second.recvErrFileCount/(m_mapIter->second.recvAllFileCount+m_mapIter->second.recvErrFileCount);
						else 
							 ratio = 0 ;						
						sprintf(buf,"%.3f",ratio);
					}
					else
						m_iFlag=0;
					break;
				}
				case 2:
				{
					//话单处理-预处理
					if(!strcmp(kpiId.c_str(),"40-30-02-02-01-001"))
					{
						m_iFlag=2;
						sprintf(buf,"%ld",m_mapIter->second.prepAllRecordCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-01-002"))
					{
						m_iFlag=2;
						sprintf(buf,"%ld",m_mapIter->second.prepErrRecordCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-01-003"))
					{
						m_iFlag=2;
						if(m_mapIter->second.prepAllRecordCount)
						  ratio=(float)m_mapIter->second.prepErrRecordCount/m_mapIter->second.prepAllRecordCount;
						else
							ratio = 0 ;
						sprintf(buf,"%.3f",ratio);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-01-004"))
					{
						m_iFlag=2;
						sprintf(buf,"%ld",m_mapIter->second.prepDupRecordCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-01-005"))
					{
						m_iFlag=2;
						if(m_mapIter->second.prepAllRecordCount)
						  ratio=(float)m_mapIter->second.prepDupRecordCount/m_mapIter->second.prepAllRecordCount ;
						else
							ratio = 0 ;
						sprintf(buf,"%.3f",ratio);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-01-006"))
					{
						m_iFlag=2;
						if(atol(StatInterface::m_pPlan->m_sTimeInterval))
						  ratio=m_mapIter->second.prepAllRecordCount/atol(StatInterface::m_pPlan->m_sTimeInterval);
						else
							ratio = 0 ;
						sprintf(buf,"%.3f",ratio);
					}
					else
						m_iFlag=0;
					break;
				}
				case 3:
				{
			  	//话单处理-批价
					if(!strcmp(kpiId.c_str(),"40-30-02-02-02-001"))
					{
						m_iFlag=3;
						sprintf(buf,"%ld",m_mapIter->second.pricingOkRecordCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-02-002"))
					{
						m_iFlag=3;
						if(atol(StatInterface::m_pPlan->m_sTimeInterval))
						  ratio=m_mapIter->second.pricingOkRecordCount/atol(StatInterface::m_pPlan->m_sTimeInterval);
						else 
							ratio = 0 ;
						sprintf(buf,"%.3f",ratio);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-02-003"))
					{
						m_iFlag=3;
						sprintf(buf,"%ld",m_mapIter->second.pricingUseLessRecordCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-02-004"))
					{
						m_iFlag=3;
						if(m_mapIter->second.pricingAllRecordCount)
						   ratio=(float)m_mapIter->second.pricingUseLessRecordCount/m_mapIter->second.pricingAllRecordCount;
						else
							 ratio = 0;
						sprintf(buf,"%.3f",ratio);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-02-005"))
					{
						m_iFlag=3;
						sprintf(buf,"%ld",m_mapIter->second.pricingErrRecordCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-02-006"))
					{
						m_iFlag=3;
						if(m_mapIter->second.pricingAllRecordCount)
						   ratio=(float)m_mapIter->second.pricingErrRecordCount/m_mapIter->second.pricingAllRecordCount;
						else
							 ratio = 0;
						sprintf(buf,"%.3f",ratio);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-02-007"))
					{
						m_iFlag=3;
						sprintf(buf,"%ld",m_mapIter->second.pricingDupRecordCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-02-008"))
					{
						m_iFlag=3;
						if(m_mapIter->second.pricingAllRecordCount)
						  ratio=(float)m_mapIter->second.pricingDupRecordCount/m_mapIter->second.pricingAllRecordCount;
						else
							ratio = 0 ;
						sprintf(buf,"%.3f",ratio);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-02-009"))
					{
						m_iFlag=3;
						sprintf(buf,"%ld",m_mapIter->second.pricingBlackRecordCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-02-010"))
					{
						m_iFlag=3;
						if(m_mapIter->second.pricingAllRecordCount)
						  ratio=(float)m_mapIter->second.pricingBlackRecordCount/m_mapIter->second.pricingAllRecordCount;
						else
							ratio = 0 ;
						sprintf(buf,"%.3f",ratio);
					}
					else
						m_iFlag=0;
				  break;
				}
				case 4:
				{
					//话单处理-入库
					if(!strcmp(kpiId.c_str(),"40-30-02-02-03-001"))
					{
						m_iFlag=4;
						sprintf(buf,"%ld",m_mapIter->second.indbOutAllRecordCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-03-002"))
					{
						m_iFlag=4;
						if(atol(StatInterface::m_pPlan->m_sTimeInterval))
						   ratio=m_mapIter->second.indbOutAllRecordCount/atol(StatInterface::m_pPlan->m_sTimeInterval);
						else
						 	 ratio = 0 ;
						sprintf(buf,"%.3f",ratio);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-03-003"))
					{
						m_iFlag=4;
						sprintf(buf,"%ld",m_mapIter->second.indbErrRecordCount);
					}
					else if(!strcmp(kpiId.c_str(),"40-30-02-02-03-004"))
					{
						m_iFlag=4;
						if(m_mapIter->second.indbOutAllRecordCount)
						  ratio=(float)m_mapIter->second.indbErrRecordCount/m_mapIter->second.indbOutAllRecordCount;
						else
							ratio = 0 ;
						sprintf(buf,"%.3f",ratio);
					}
					else
						m_iFlag=0;
					break;
				}
				default:
				{
					m_iFlag=0;
					break;
				}
			}
			
			strValue=string(buf);
			min=strValue;
			avg=strValue;
			max=strValue;
			temp=temp+min+string("|")+avg+string("|")+max;
			switch(m_iFlag)
			{
				case 1:
				{
					m_vGatherInfo.push_back(temp);
					break;
				}
				case 2:
				{
					m_vPrepInfo.push_back(temp);
					break;
				}
				case 3:
			  {
					m_vPricingInfo.push_back(temp);
					break;
				}
				case 4:
				{
					m_vInDbInfo.push_back(temp);
					break;
				}
				default:
  				break;
			}
		}
	}
	
  return true;
}

bool Information::gatherInfo()
{
	DEFINE_QUERY(qry);
	char sSQL[1024];
	int iBusi ;
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select  busi_class ,count(*),sum(record_cnt) from b_event_file_list_cfg "
	" where state='END' "
	" and created_date between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and switch_id >0 "
	" and record_cnt>=0 "
	" group by busi_class "
	,StatInterface::m_pPlan->m_sDataBeginTime,StatInterface::m_pPlan->m_sDataEndTime);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  { 
  	iBusi = qry.field(0).asInteger() ;
  	m_mapCDRInfo[iBusi].recvAllFileCount=qry.field(1).asLong();
  	m_mapCDRInfo[iBusi].recvAllRecordCount=qry.field(2).asLong();
  }
  qry.close();
	
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select busi_class,count(*),sum(record_cnt) "
	" from b_event_file_list_cfg "
	" where created_date between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and (created_date - file_date)*(3600*24) > %d "
	" and switch_id >0 "
	" and record_cnt>=0 "	
	" and state='END' "	
	" group by busi_class " ,
	StatInterface::m_pPlan->m_sDataBeginTime,StatInterface::m_pPlan->m_sDataEndTime,m_iDelayTimeSecs);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	iBusi = qry.field(0).asInteger() ;
  	m_mapCDRInfo[iBusi].recvDelayFileCount=qry.field(1).asLong();
  	m_mapCDRInfo[iBusi].recvDelayRecordCount=qry.field(2).asLong();
  }
  qry.close();
	
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"Select COUNT(*), decode(source_event_type, 3, 20, "
                                         " 4, 20, "
                                         " 6, 60, "
                                         " 7, 30, "
                                         " 9, 50, "
                                         " 11, 20, "
                                         "    10) busi_class"
        " FROM  (SELECT DISTINCT a.FILENAME,c.SOURCE_EVENT_TYPE "
        " FROM b_gather_change_log a,b_gather_task b ,b_switch_info c "
        " WHERE a.TASK_ID=b.TASK_ID "
        " AND   b.SWITCH_ID=c.SWITCH_ID "
        " AND  a.GATHER_DATE between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') ) "
        " GROUP BY source_event_type " ,
        StatInterface::m_pPlan->m_sDataBeginTime,StatInterface::m_pPlan->m_sDataEndTime );
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	iBusi = qry.field(1).asInteger() ;
  	m_mapCDRInfo[iBusi].recvErrFileCount=qry.field(0).asLong();
  }
  qry.close();
  
  memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"Select COUNT(*), decode(source_event_type, 3, 20, "
                                         " 4, 20, "
                                         " 6, 60, "
                                         " 7, 30, "
                                         " 9, 50, "
                                         " 11, 20, "
                                         "    10) busi_class"
        " FROM  (SELECT DISTINCT a.FILE_NAME,c.SOURCE_EVENT_TYPE "
        " FROM b_gather_dup_log a,b_gather_task b ,b_switch_info c "
        " WHERE a.TASK_ID=b.TASK_ID "
        " AND   b.SWITCH_ID=c.SWITCH_ID "
        " AND  a.state_date between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') ) "
        " GROUP BY source_event_type " ,
        StatInterface::m_pPlan->m_sDataBeginTime,StatInterface::m_pPlan->m_sDataEndTime );
        	  
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	iBusi = qry.field(1).asInteger() ;
  	long i=qry.field(0).asLong();
  	m_mapCDRInfo[iBusi].recvErrFileCount+=i;
  }
  qry.close();
  
  return true;
}

bool Information::prepInfo()
{
	DEFINE_QUERY(qry);
	char sSQL[1024];
	int iBusi ;
	memset(sSQL, 0, sizeof(sSQL));
 /*
	sprintf(sSQL,"select c.busi_class,sum(out_normal_event),sum(b.out_format_err+b.out_black_event+b.out_pre_err_event) "
	" from wf_stat_log b,b_event_file_list_cfg c "
	" where b.process_id in (select a.process_id from WF_PROCESS a where a.app_id=%d) "
	" and to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and b.file_id=c.file_id "
	" group by busi_class ",
	FORMAT_APP_ID,StatInterface::m_pPlan->m_sDataBeginTime,StatInterface::m_pPlan->m_sDataEndTime);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
    iBusi = qry.field(0).asInteger() ;
  	m_mapCDRInfo[iBusi].prepAllRecordCount=qry.field(1).asLong()+qry.field(2).asLong();
  	m_mapCDRInfo[iBusi].prepErrRecordCount=qry.field(2).asLong();
  }
  qry.close();
  
  memset(sSQL, 0, sizeof(sSQL));
  */
  char sBegTime[6+1]={0};
  char sEndTime[6+1]={0};
  char sBegDate[8+1]={0};
  Date dts(StatInterface::m_pPlan->m_sDataBeginTime);
  Date dte(StatInterface::m_pPlan->m_sDataEndTime);
  dts.addSec(1);
  strncpy(sBegDate, dts.toString(), 8);	
  strncpy(sBegTime,dts.toString()+8,6);
  strncpy(sEndTime,dte.toString()+8,6);
	sprintf(sSQL,"select busi_class,sum(out_normal_event),sum(b.out_format_err+b.out_black_event+b.out_pre_err_event),sum(b.out_dup_enent) "
	" from wf_stat_log b,b_event_file_list_cfg c "
	" where b.process_id in (select a.process_id from WF_PROCESS a where a.app_id=%d) "
	//" and to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and b.file_id=c.file_id "
	" and b.msg_date = '%s' "
	" and b.msg_time between '%s' and '%s' "
	" group by busi_class ",
	CHECKDUP_APP_ID,sBegDate,sBegTime,sEndTime);
	// CHECKDUP_APP_ID,StatInterface::m_pPlan->m_sDataBeginTime,StatInterface::m_pPlan->m_sDataEndTime);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	iBusi = qry.field(0).asInteger() ;
  	m_mapCDRInfo[iBusi].prepAllRecordCount=qry.field(1).asLong()+qry.field(2).asLong();
  	m_mapCDRInfo[iBusi].prepErrRecordCount=qry.field(2).asLong();  	
  	m_mapCDRInfo[iBusi].prepDupRecordCount=qry.field(3).asLong();
  }
  qry.close();
  					
	return true;
}

bool Information::pricingInfo()
{
	DEFINE_QUERY(qry);
	char sSQL[1024];
	int iBusi ;
  char sBegTime[6+1]={0};
  char sEndTime[6+1]={0};
  char sBegDate[8+1]={0};
  Date dts(StatInterface::m_pPlan->m_sDataBeginTime);
  Date dte(StatInterface::m_pPlan->m_sDataEndTime);
  dts.addSec(1);
  strncpy(sBegDate, dts.toString(), 8);	
  strncpy(sBegTime,dts.toString()+8,6);
  strncpy(sEndTime,dte.toString()+8,6);
  	
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select busi_class,sum(b.out_normal_event+b.out_black_event+b.out_dup_enent+b.out_useless_event+b.out_pric_err_event+b.out_pre_err_event+b.out_format_err),"
	" sum(b.out_useless_event),sum(b.out_pric_err_event),sum(b.out_dup_enent),sum(b.out_black_event),sum(b.out_normal_event) "
	" from wf_stat_log b,b_event_file_list_cfg c "
	" where b.process_id in (select a.process_id from WF_PROCESS a where a.app_id=%d) "
	//" and to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and b.file_id=c.file_id "
	" and b.msg_date = '%s' "
	" and b.msg_time between '%s' and '%s' "
	" group by busi_class ",
	CHECKDUP_APP_ID,sBegDate,sBegTime,sEndTime);
	//PRICING_APP_ID,StatInterface::m_pPlan->m_sDataBeginTime,StatInterface::m_pPlan->m_sDataEndTime);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	iBusi = qry.field(0).asInteger() ;
  	m_mapCDRInfo[iBusi].pricingAllRecordCount=qry.field(1).asLong();
  	m_mapCDRInfo[iBusi].pricingUseLessRecordCount=qry.field(2).asLong();
  	m_mapCDRInfo[iBusi].pricingErrRecordCount=qry.field(3).asLong();
  	m_mapCDRInfo[iBusi].pricingDupRecordCount=qry.field(4).asLong();
  	m_mapCDRInfo[iBusi].pricingBlackRecordCount=qry.field(5).asLong();
  	m_mapCDRInfo[iBusi].pricingOkRecordCount=qry.field(6).asLong();
  }
  
  qry.close();
  
  return true;
}

bool Information::indbInfo()
{
	DEFINE_QUERY(qry);
	char sSQL[1024];
	int iBusi ;
	
  char sBegTime[6+1]={0};
  char sEndTime[6+1]={0};
  char sBegDate[8+1]={0};
  Date dts(StatInterface::m_pPlan->m_sDataBeginTime);
  Date dte(StatInterface::m_pPlan->m_sDataEndTime);
  dts.addSec(1);
  strncpy(sBegDate, dts.toString(), 8);	
  strncpy(sBegTime,dts.toString()+8,6);
  strncpy(sEndTime,dte.toString()+8,6);
  	
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select busi_class,sum(b.in_normal_event+b.in_black_event+b.in_dup_event+b.in_pre_err_event+b.in_useless_event+b.in_pric_err_event),"
	"sum(b.out_normal_event+b.out_black_event+b.out_dup_enent+b.out_pre_err_event+b.out_pric_err_event+b.out_useless_event) "
	" from wf_stat_log b,b_event_file_list_cfg c "
	" where b.process_id in (select a.process_id from WF_PROCESS a where a.app_id in (%s)) "
	//" and to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and b.file_id=c.file_id "
	" and b.msg_date = '%s' "
	" and b.msg_time between '%s' and '%s' "
	" group by busi_class ",
	INSTORE_APP_ID,sBegDate,sBegTime,sEndTime);  
	//INSTORE_APP_ID,StatInterface::m_pPlan->m_sDataBeginTime,StatInterface::m_pPlan->m_sDataEndTime);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	iBusi = qry.field(0).asInteger() ;
  	m_mapCDRInfo[iBusi].indbInAllRecordCount=qry.field(1).asLong();
  	m_mapCDRInfo[iBusi].indbOutAllRecordCount=qry.field(2).asLong();
  	m_mapCDRInfo[iBusi].indbErrRecordCount=qry.field(1).asLong()-qry.field(2).asLong();
  }
  
  qry.close();
  
  return true;
}

bool Information::checkGather()
{
	StatInterface oGatherInfoStat("0100",40,MIDL_FREG);
	
	if(!init(MODULE_GATHER))
		return false;
	m_vGatherInfo.clear();
	m_vGatherInfo.push_back(CAPABILITY_TITLE);
	checkPerformance(MODULE_GATHER);
	
	print(m_vGatherInfo);
	oGatherInfoStat.writeJsonFile(m_vGatherInfo,SPLIT_FLAG);
	
	return true;
}

bool Information::checkPerp()
{
	StatInterface oPrepInfoStat("0201",40,MIDL_FREG);
	
	init(MODULE_PREP);
	m_vPrepInfo.clear();
	m_vPrepInfo.push_back(CAPABILITY_TITLE);
	checkPerformance(MODULE_PREP);
	
	print(m_vPrepInfo);
	oPrepInfoStat.writeJsonFile(m_vPrepInfo,SPLIT_FLAG);
	
  return true;
}

bool Information::checkPricing()
{
	StatInterface oPricingInfoStat("0202",40,MIDL_FREG);
	
	init(MODULE_PRICING);
	m_vPricingInfo.clear();
	m_vPricingInfo.push_back(CAPABILITY_TITLE);
	checkPerformance(MODULE_PRICING);
	
	print(m_vPricingInfo);
	oPricingInfoStat.writeJsonFile(m_vPricingInfo,SPLIT_FLAG);
	
	return true;
}

bool Information::checkIndb()
{
	StatInterface oInDbInfoStat("0203",40,MIDL_FREG);
	
	init(MODULE_INSTORE);
	m_vInDbInfo.clear();
	m_vInDbInfo.push_back(CAPABILITY_TITLE);
	checkPerformance(MODULE_INSTORE);
	
	print(m_vInDbInfo);
	oInDbInfoStat.writeJsonFile(m_vInDbInfo,SPLIT_FLAG);
	
  return true;
}

bool Information::checkAllProductInfo()
{
	string kpiId;
	string kpiName;
	string kbpId;
	string kbpName;
	string action;
	string value;
	
	vector<string> vProductInfo;
	StatInterface oProductInfoStat("0403",30,LOW1_FREG);
	vProductInfo.clear();
	vProductInfo.push_back(CONFIG_TITLE);
	
	DEFINE_QUERY(qry);
	//qry.setSQL("select product_code,product_name,product_description,product_tariff,product_classification from b_info_product_28");
	
	string sTemp(StatInterface::m_pPlan->m_sDataEndTime);
	string sTemp2(sTemp.substr(6,2));
	if(sTemp2.compare("01")  ==  0){
	 	qry.setSQL("select nvl(a.product_code,a.product_id) product_code,a.product_name,a.product_description, " 
								"       nvl(b.pricing_plan_id, 1 ) pricing_plan_code,product_classification,0 change_type,sysdate-10" 
								"  from product a, pricing_plan b " 
								" where a.pricing_plan_id = b.pricing_plan_id(+)");
	}else{
		
		 qry.setSQL("select product_code,product_name,product_description,pricing_plan_code,product_classification,changed_type,create_date from b_info_product "
		" where create_date between to_date(:stat_begin_date, 'yyyymmddhh24miss') and to_date(:stat_end_date, 'yyyymmddhh24miss')");
		
		qry.setParameter("stat_begin_date", StatInterface::m_pPlan->m_sDataBeginTime);
		qry.setParameter("stat_end_date", StatInterface::m_pPlan->m_sDataEndTime);

	}
	
	qry.open();
  while (qry.next())
  {
  	string temp;
		
  	kbpId=string("/HB/")+qry.field(0).asString();
		kbpName=qry.field(1).asString();
	  

		action=qry.field(5).asString();

		
		kpiId=string("30-30-02-04-03-001");
		kpiName=string("产品名称");
		value=qry.field(1).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vProductInfo.push_back(temp);
		
		kpiId=string("30-30-02-04-03-003");
		kpiName=string("产品描述");
		value=qry.field(2).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vProductInfo.push_back(temp);
		
		/*kpiId=string("30-30-02-04-03-004");
		kpiName=string("资费标准");
		value=qry.field(3).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vProductInfo.push_back(temp);*/
		
		kpiId=string("30-30-02-04-03-005");
		kpiName=string("产品类型");
		string type=qry.field(4).asString();
		if(!strcmp(type.c_str(),"11A"))
		{
			value=string("主产品");
		}
		else
		{
			value=string("附属产品");
		}
		
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;

		vProductInfo.push_back(temp);		
		
		kpiId=string("30-30-02-04-03-006");
		kpiName=string("定价计划编码");
		value=qry.field(3).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vProductInfo.push_back(temp);
		
		kpiId=string("30-30-02-04-03-007");
		kpiName=string("修改时间");
		value=qry.field(6).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vProductInfo.push_back(temp);
  }
  
  qry.close();
	
	print(vProductInfo);
	oProductInfoStat.writeJsonFile(vProductInfo,SPLIT_FLAG);
	
	return true;
}

bool Information::checkAllProductOfferInfo()
{
	string kpiId;
	string kpiName;
	string kbpId;
	string kbpName;
	string action;
	string value;
	
	vector<string> vProductOfferInfo;
	StatInterface oProductOfferInfoStat("0404",30,LOW1_FREG);
	vProductOfferInfo.clear();
	vProductOfferInfo.push_back(CONFIG_TITLE);
	
	DEFINE_QUERY(qry);
	//qry.setSQL("select offer_code,offer_name,offer_description,offer_tariff from b_info_product_offer_28");
	
	string sTemp(StatInterface::m_pPlan->m_sDataEndTime);
	string sTemp2(sTemp.substr(6,2));
	if(sTemp2.compare("01")  ==  0){
		
		/*qry.setSQL("select nvl(a.offer_code,a.offer_id) offer_code,a.offer_name,a.offer_description, " 
								"       nvl(b.pricing_desc, '资费描述') offer_tariff,0 change_type " 
								"  from product_offer a, pricing_plan b " 
								" where a.pricing_plan_id = b.pricing_plan_id(+)");*/

		qry.setSQL("select nvl(a.offer_code,a.offer_id) offer_code,a.offer_name,a.offer_description, " 
								"      nvl(a.pricing_plan_id, '1') pricing_plan_id,0 change_type,a.create_date " 
								"  from product_offer a " );
								
	}else{	
		
	 qry.setSQL("select offer_code,offer_name,pricing_plan_code,offer_tariff,change_type,create_date from b_info_product_offer "
		" where create_date between to_date(:stat_begin_date, 'yyyymmddhh24miss') and to_date(:stat_end_date, 'yyyymmddhh24miss')");
		
		qry.setParameter("stat_begin_date", StatInterface::m_pPlan->m_sDataBeginTime);
		qry.setParameter("stat_end_date", StatInterface::m_pPlan->m_sDataEndTime);
	}
	
	qry.open();
  while (qry.next())
  {
  	string temp;
		
  	kbpId=string("/HB/")+qry.field(0).asString();
		kbpName=qry.field(1).asString();
		

		action=qry.field(4).asString();

		
		kpiId=string("30-30-02-04-04-001");
		kpiName=string("商品名称");
		value=qry.field(1).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vProductOfferInfo.push_back(temp);
		
		kpiId=string("30-30-02-04-04-003");
		kpiName=string("商品描述");
		value=qry.field(2).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vProductOfferInfo.push_back(temp);
		
		kpiId=string("30-30-02-04-04-005");
		kpiName=string("定价计划编码");
		value=qry.field(3).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vProductOfferInfo.push_back(temp);		
		
		kpiId=string("30-30-02-04-04-006");
		kpiName=string("修改时间");
		value=qry.field(5).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vProductOfferInfo.push_back(temp);	
  }
  
  qry.close();
	
	print(vProductOfferInfo);
	oProductOfferInfoStat.writeJsonFile(vProductOfferInfo,SPLIT_FLAG);
	
  return true;
}

bool Information::checkAllPricingPlanInfo()
{
	string kpiId;
	string kpiName;
	string kbpId;
	string kbpName;
	string action;
	string value;
	
	vector<string> vProductOfferInfo;
	StatInterface oProductOfferInfoStat("0405",30,LOW1_FREG);
	vProductOfferInfo.clear();
	vProductOfferInfo.push_back(CONFIG_TITLE);
	
	DEFINE_QUERY(qry);
	
	string sTemp(StatInterface::m_pPlan->m_sDataEndTime);
	string sTemp2(sTemp.substr(6,2));
	if(sTemp2.compare("01")  ==  0){
		

		qry.setSQL("SELECT PRICING_PLAN_ID,PRICING_PLAN_NAME,PRICING_DESC,SYSDATE-9  " 
								"  ,0 change_type FROM PRICING_PLAN " );
								
	}else{	
		
	 qry.setSQL("select PRICING_PLAN_ID, PRICING_PLAN_NAME,PRICING_PLAN_TARIFF ,create_date,change_type from B_INFO_PRICING_PLAN "
		" where create_date between to_date(:stat_begin_date, 'yyyymmddhh24miss') and to_date(:stat_end_date, 'yyyymmddhh24miss')");
		
		qry.setParameter("stat_begin_date", StatInterface::m_pPlan->m_sDataBeginTime);
		qry.setParameter("stat_end_date", StatInterface::m_pPlan->m_sDataEndTime);
	}
	
	qry.open();
  while (qry.next())
  {
  	string temp;
		
  	kbpId=string("/HB/")+qry.field(0).asString();
		kbpName=qry.field(1).asString();
		

		action=qry.field(4).asString();

		
		kpiId=string("30-30-02-04-05-001");
		kpiName=string("定价计划名称"); 
		value=qry.field(1).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vProductOfferInfo.push_back(temp);
		
		kpiId=string("30-30-02-04-05-002");
		kpiName=string("资费说明");
		value=qry.field(2).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vProductOfferInfo.push_back(temp);
		
		kpiId=string("30-30-02-04-05-002");
		kpiName=string("修改时间");
		value=qry.field(3).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vProductOfferInfo.push_back(temp);		
		
  }
  
  qry.close();
	
	print(vProductOfferInfo);
	oProductOfferInfoStat.writeJsonFile(vProductOfferInfo,SPLIT_FLAG);
	
  return true;
}

bool Information::checkChangedProductInfo()
{
	string kpiId;
	string kpiName;
	string kbpId;
	string kbpName;
	string action;
	string value;
	
	vector<string> vChangedProductInfo;
	StatInterface oChangedProductInfoStat("0403",30,LOW1_FREG);
	vChangedProductInfo.clear();
	vChangedProductInfo.push_back(CONFIG_TITLE);
	
	DEFINE_QUERY(qry);
	qry.setSQL("select product_code,product_name,product_description,product_tariff,product_classification,changed_type from product_changed "
	" where create_date between to_date(:stat_begin_date, 'yyyymmddhh24miss') and to_date(:stat_end_date, 'yyyymmddhh24miss')");
	//qry.setParameter("stat_begin_date", "20100801000000");
	//qry.setParameter("stat_end_date", "20100930000000");
	qry.setParameter("stat_begin_date", StatInterface::m_pPlan->m_sDataBeginTime);
	qry.setParameter("stat_end_date", StatInterface::m_pPlan->m_sDataEndTime);
	qry.open();
  while (qry.next())
  {
  	string temp;
		
  	kbpId=string("/HB/")+qry.field(0).asString();
		kbpName=qry.field(1).asString();
		int change=qry.field(5).asInteger();
		switch(change)
		{
			case 1:
			{
				action=string("1");
				break;
			}
			case 2:
			{
				action=string("2");
				break;
			}
			case 3:
			{
				action=string("3");
				break;
			}
			default :
			{
				//action=string("0");
				break;
			}
		}
		
		kpiId=string("30-30-02-04-03-001");
		kpiName=string("产品名称");
		value=qry.field(1).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vChangedProductInfo.push_back(temp);
		
		kpiId=string("30-30-02-04-03-003");
		kpiName=string("产品描述");
		value=qry.field(2).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vChangedProductInfo.push_back(temp);
		
		kpiId=string("30-30-02-04-03-004");
		kpiName=string("资费标准");
		value=qry.field(3).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vChangedProductInfo.push_back(temp);
		
		kpiId=string("30-30-02-04-03-005");
		kpiName=string("产品类型");
		string type=qry.field(4).asString();
		if(!strcmp(type.c_str(),"11A"))
		{
			value=string("主产品");
		}
		else
		{
			value=string("附属产品");
		}
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;

		vChangedProductInfo.push_back(temp);		
  }
  
  qry.close();
  
  print(vChangedProductInfo);
  oChangedProductInfoStat.writeJsonFile(vChangedProductInfo,SPLIT_FLAG);

  return true;
}

bool Information::checkChangedProductOfferInfo()
{
	string kpiId;
	string kpiName;
	string kbpId;
	string kbpName;
	string action;
	string value;
	
	vector<string> vChangedProductOfferInfo;
	StatInterface oChangedProductOfferInfoStat("0404",30,LOW1_FREG);
	vChangedProductOfferInfo.clear();
	vChangedProductOfferInfo.push_back(CONFIG_TITLE);
	
	DEFINE_QUERY(qry);
	qry.setSQL("select offer_code,offer_name,offer_description,offer_tariff,changed_type from b_info_product_offer_changed "
	" where create_date between to_date(:stat_begin_date, 'yyyymmddhh24miss') and to_date(:stat_end_date, 'yyyymmddhh24miss')");
	//qry.setParameter("stat_begin_date", "20100801000000");
	//qry.setParameter("stat_end_date", "20100930000000");
	qry.setParameter("stat_begin_date", StatInterface::m_pPlan->m_sDataBeginTime);
	qry.setParameter("stat_end_date", StatInterface::m_pPlan->m_sDataEndTime);
	qry.open();
  while (qry.next())
  {
  	string temp;
  						
  	kbpId=string("/HB/")+qry.field(0).asString();
		kbpName=qry.field(1).asString();
		int change=qry.field(4).asInteger();
		switch(change)
		{
			case 1:
			{
				action=string("1");
				break;
			}
			case 2:
			{
				action=string("2");
				break;
			}
			case 3:
			{
				action=string("3");
				break;
			}
			default :
			{
				//action=string("0");
				break;
			}
		}
		
		kpiId=string("30-30-02-04-04-001");
		kpiName=string("商品名称");
		value=qry.field(1).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vChangedProductOfferInfo.push_back(temp);
		
		kpiId=string("30-30-02-04-04-003");
		kpiName=string("商品描述");
		value=qry.field(2).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vChangedProductOfferInfo.push_back(temp);
		
		kpiId=string("30-30-02-04-04-004");
		kpiName=string("资费说明");
		value=qry.field(3).asString();
		temp=kpiId+string("|")+kpiName+string("|")+kbpId+string("|")+kbpName+string("|")+action+string("|")+value;
		
		vChangedProductOfferInfo.push_back(temp);		
  }
  
  qry.close();
  
  print(vChangedProductOfferInfo);
  oChangedProductOfferInfoStat.writeJsonFile(vChangedProductOfferInfo,SPLIT_FLAG);
    		
  return true;
}

bool Information::insertAlarm(string kpiId,string kpiName,string alarmId,string kbpId,string kbpName,Date occurTime,int modeType)
{
		DEFINE_QUERY(qry);
		char sSQL[1024];
		memset(sSQL, 0, sizeof(sSQL));
		sprintf(sSQL,"insert into B_ALARM_LIST(KPIID,KPINAME,ALARMID,KBPID,KBPNAME,OCCURTIME,EVENTLEVEL,EVENTSTATE,ALARMCOUNT,MODETPYE) "
		"values ('%s','%s','%s','%s','%s',to_date('%s','yyyy-mm-dd hh24:mi:ss'),2,10,1,%d)",
		kpiId.c_str(),kpiName.c_str(),alarmId.c_str(),kbpId.c_str(),kbpName.c_str(),StatInterface::m_pPlan->m_sDataEndTime,modeType);
		
		qry.setSQL(sSQL);
		qry.execute();
		qry.commit();
		qry.close();
		
	  return true;
}

bool Information::checkBalance(int mode)
{
	string kpiId;
	string kpiName;
	string alarmId;
	string kbpId;
	string kbpName;
	string occurTime;
	
	DEFINE_QUERY(qry);
  qry.setSQL("select trim(code_id),trim(code_name) from b_info_code");
  qry.open();
  while (qry.next())
  {
  	m_pCodeList[qry.field(0).asString()]=qry.field(1).asString();
  }
	qry.close();
	
	map<string,string>::iterator iter;
	for(iter = m_pCodeList.begin();iter != m_pCodeList.end(); ++iter)
	{
		m_bPrepInOut=false;
		m_bRecvOutPrepIn=false;
		m_bPricingInOut=false;
		m_bPrepOutPricingIn=false;
		m_bPricingOutIndbIn=false;
		kbpId=iter->first;
		kbpName=iter->second;
		memset(m_source,0,sizeof(m_source));
		memset(m_busiclass,0,sizeof(m_busiclass));
		switch(atoi(kbpId.c_str()))
		{
			case 10:
				sprintf(m_source,"%d",10);
				sprintf(m_busiclass,"%d,%d",1,10);
				break;
			case 20:
				sprintf(m_source,"%d",20);
				sprintf(m_busiclass,"%d,%d,%d",3,4,11);
				break;
			case 30:
				sprintf(m_source,"%d",30);
				sprintf(m_busiclass,"%d",7);
				break;
			case 40:
				sprintf(m_source,"%d",40);
				sprintf(m_busiclass,"%d",6);
				break;
			case 50:
				sprintf(m_source,"%d",50);
				sprintf(m_busiclass,"%d",9);
				break;
			case 60:
				sprintf(m_source,"%d",60);
				sprintf(m_busiclass,"%d",6);
				break;
			default:
				sprintf(m_source,"%d",10);
				sprintf(m_busiclass,"%d,%d",1,10);
				break;
		}
		kbpId=string("/HB/")+kbpId;
		switch(mode)
		{
			case MODULE_PREP:
			{
				checkPrepAlarm();
				
				Date occurTime;
				if(m_bPrepInOut)
				{
					alarmId=string("/HB/")+string("0201101");
					kpiId=string("20-30-02-02-01-101");
					kpiName=string("预处理处理话单数不平衡");
					insertAlarm(kpiId,kpiName,alarmId,kbpId,kbpName,occurTime,MODULE_PREP);
				}
				if(m_bRecvOutPrepIn)
				{
					alarmId=string("/HB/")+string("0201102");
					kpiId=string("20-30-02-02-01-102");
					kpiName=string("计费接收模块正常输出话单数与预处理接收话单数不平衡");
					insertAlarm(kpiId,kpiName,alarmId,kbpId,kbpName,occurTime,MODULE_PREP);
				}
				break;
			}
			case MODULE_PRICING:
			{
				checkPricingAlarm();
				
				Date occurTime;
				if(m_bPricingInOut)
				{
					alarmId=string("/HB/")+string("0202101");
					kpiId=string("20-30-02-02-02-101");
					kpiName=string("批价处理话单数不平衡");
					insertAlarm(kpiId,kpiName,alarmId,kbpId,kbpName,occurTime,MODULE_PRICING);
				}
				if(m_bPrepOutPricingIn)
				{
					alarmId=string("/HB/")+string("0202102");
					kpiId=string("20-30-02-02-02-102");
					kpiName=string("预处理正常输出话单数与批价接收话单数不平衡");
					insertAlarm(kpiId,kpiName,alarmId,kbpId,kbpName,occurTime,MODULE_PRICING);
				}
				break;
			}
			case MODULE_INSTORE:
			{
				checkIndbAlarm();
				
				Date occurTime;
				if(m_bPricingOutIndbIn)
				{
					alarmId=string("/HB/")+string("0203101");
					kpiId=string("20-30-02-02-03-101");
					kpiName=string("批价正常输出话单数与入库接收话单数不平衡");
					insertAlarm(kpiId,kpiName,alarmId,kbpId,kbpName,occurTime,MODULE_INSTORE);
				}
				break;
			}
			default:
				break;
		}
	}
	
	return true;
}

bool Information::checkPrepAlarm()
{
	DEFINE_QUERY(qry);
	char sSQL[1024];
	map<int,long> prepInRecordCount;
	map<int,long> prepOutRecordCount;
	map<int,long> recvOutRecordCount;
	
  char sBegTime[6+1]={0};
  char sEndTime[6+1]={0};
  char sBegDate[8+1]={0};
  Date dts(StatInterface::m_pPlan->m_sDataBeginTime);
  Date dte(StatInterface::m_pPlan->m_sDataEndTime);
  dts.addSec(1);
  strncpy(sBegDate, dts.toString(), 8);	
  strncpy(sBegTime,"000000",6);
  strncpy(sEndTime,"235959",6);
  	
	
	//初始化
  qry.setSQL("select trim(code_id),code_name from b_info_code");
  qry.open();
  while (qry.next())
  {
  	m_pCodeList[qry.field(0).asString()]=qry.field(1).asString();
  }
	qry.close();
	
	map<string,string>::iterator iter;
	for(iter = m_pCodeList.begin();iter != m_pCodeList.end(); ++iter)
	{
		int iCode = atoi((iter->first).c_str());
		prepInRecordCount[iCode] =0;
		prepOutRecordCount[iCode] =0;
		recvOutRecordCount[iCode] =0;
	}
  
	//接收到的数据
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select busi_class,sum(record_cnt) from b_event_file_list_cfg "
	" where  created_date between to_date(%s,'yyyymmddhh24miss') and to_date(%s,'yyyymmddhh24miss') "
	" and state='END' group by busi_class"
	,StatInterface::m_pPlan->m_sDataBeginTime,StatInterface::m_pPlan->m_sDataEndTime);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	recvOutRecordCount[qry.field(0).asInteger()] = qry.field(1).asLong();
  }  
  qry.close();
  
  //prepInRecordCount=getAppInfo(PREP_APP_ID,true);
	//prepOutRecordCount=getAppInfo(CHECKDUP_APP_ID,false);
	
  //输入的数据
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select busi_class,sum(b.in_normal_event+b.in_black_event+b.in_dup_event+b.in_pre_err_event+b.in_useless_event+b.in_pric_err_event+b.format_err),"
	" sum(b.out_normal_event+b.out_black_event+b.out_dup_enent+b.out_pre_err_event+b.out_useless_event+b.out_pric_err_event+b.out_format_err) "
	" from wf_stat_log b,b_event_file_list_cfg c "
	" where b.process_id in (select a.process_id from WF_PROCESS a where a.app_id in (%d)) "
	//" and to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and b.file_id=c.file_id "
	" and b.msg_date = '%s' "
	" and b.msg_time between '%s' and '%s' "
	" group by busi_class"
	,PREP_APP_ID, sBegDate,sBegTime,sEndTime);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	prepInRecordCount[qry.field(0).asInteger()] = qry.field(1).asLong();
  }  
  qry.close();  
  
  //输出的数据
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select busi_class, sum(b.in_normal_event+b.in_black_event+b.in_dup_event+b.in_pre_err_event+b.in_useless_event+b.in_pric_err_event+b.format_err),"
	" sum(b.out_normal_event+b.out_black_event+b.out_dup_enent+b.out_pre_err_event+b.out_useless_event+b.out_pric_err_event+b.out_format_err) "
	" from wf_stat_log b,b_event_file_list_cfg c "
	" where b.process_id in (select a.process_id from WF_PROCESS a where a.app_id in (%d)) "
	//" and to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and b.file_id=c.file_id "
	" and b.msg_date = '%s' "
	" and b.msg_time between '%s' and '%s' "	
	" group by busi_class"
	,CHECKDUP_APP_ID,sBegDate,sBegTime,sEndTime);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	prepOutRecordCount[qry.field(0).asInteger()] = qry.field(2).asLong();
  }	
  qry.close();
	
	//拼接文件
	string kpiId;
	string kpiName;
	string alarmId;
	string kbpId;
	string kbpName;
	string occurTime;
	
	for(iter = m_pCodeList.begin();iter != m_pCodeList.end(); ++iter)
	{
		m_bPrepInOut=false;
		m_bRecvOutPrepIn=false;
		m_bPricingInOut=false;
		m_bPrepOutPricingIn=false;
		m_bPricingOutIndbIn=false;
		kbpId=iter->first;
		kbpName=iter->second;
		memset(m_source,0,sizeof(m_source));
		memset(m_busiclass,0,sizeof(m_busiclass));

		kbpId=string("/HB/")+kbpId;

		Date occurTime;
		int iCode = atoi((iter->first).c_str());
		if(prepInRecordCount[iCode] != prepOutRecordCount[iCode])
		{
			alarmId=string("/HB/")+string("0201101");
			kpiId=string("20-30-02-02-01-101");
			kpiName=string("预处理处理话单数不平衡");
			insertAlarm(kpiId,kpiName,alarmId,kbpId,kbpName,occurTime,MODULE_PREP);
		}
		if(recvOutRecordCount[iCode] != prepInRecordCount[iCode])
		{
			alarmId=string("/HB/")+string("0201102");
			kpiId=string("20-30-02-02-01-102");
			kpiName=string("计费接收模块正常输出话单数与预处理接收话单数不平衡");
			insertAlarm(kpiId,kpiName,alarmId,kbpId,kbpName,occurTime,MODULE_PREP);
		}
	}	
	
	/*
	if(prepInRecordCount!=prepOutRecordCount)
	{
		m_bPrepInOut=true;
	}
	if(recvOutRecordCount!=prepInRecordCount)
	{
		m_bRecvOutPrepIn=true;
	}
	*/
	
  return true;
}

bool Information::checkPricingAlarm()
{
	DEFINE_QUERY(qry);
	char sSQL[1024];
	map<int,long> prepOutRecordCount;
	map<int,long> pricingInRecordCount;
	map<int,long> pricingOutRecordCount;
	
  char sBegTime[6+1]={0};
  char sEndTime[6+1]={0};
  char sBegDate[8+1]={0};
  Date dts(StatInterface::m_pPlan->m_sDataBeginTime);
  Date dte(StatInterface::m_pPlan->m_sDataEndTime);
  dts.addSec(1);
  strncpy(sBegDate, dts.toString(), 8);	
  strncpy(sBegTime,"000000",6);
  strncpy(sEndTime,"235959",6);
	
	//初始化
  qry.setSQL("select trim(code_id),code_name from b_info_code");
  qry.open();
  while (qry.next())
  {
  	m_pCodeList[qry.field(0).asString()]=qry.field(1).asString();
  }
	qry.close();
	
	map<string,string>::iterator iter;
	for(iter = m_pCodeList.begin();iter != m_pCodeList.end(); ++iter)
	{
		int iCode = atoi((iter->first).c_str());
		prepOutRecordCount[iCode] =0;
		pricingInRecordCount[iCode] =0;
		pricingOutRecordCount[iCode] =0;
	}
	//prepOutRecordCount=getAppInfo(CHECKDUP_APP_ID,false);
	//pricingInRecordCount=getAppInfo(PRICING_APP_ID,true);
	//pricingOutRecordCount=getAppInfo(PRICING_APP_ID,false);
	
	//参与处理的数据
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select busi_class,sum(b.in_normal_event+b.in_black_event+b.in_dup_event+b.in_pre_err_event+b.in_useless_event+b.in_pric_err_event+b.format_err),"
	" sum(b.out_normal_event+b.out_black_event+b.out_dup_enent+b.out_pre_err_event+b.out_useless_event+b.out_pric_err_event+b.out_format_err) "
	" from wf_stat_log b,b_event_file_list_cfg c "
	" where b.process_id in (select a.process_id from WF_PROCESS a where a.app_id in (%d)) "
	//" and to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and b.file_id=c.file_id "
  " and b.msg_date = '%s' "
	" and b.msg_time between '%s' and '%s' "	
	" group by busi_class"
	,CHECKDUP_APP_ID,sBegDate,sBegTime,sEndTime);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	prepOutRecordCount[qry.field(0).asInteger()] = qry.field(2).asLong();
  }  
  qry.close(); 
  
  //输入输出
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select busi_class,sum(b.in_normal_event+b.in_black_event+b.in_dup_event+b.in_pre_err_event+b.in_useless_event+b.in_pric_err_event+b.format_err),"
	" sum(b.out_normal_event+b.out_black_event+b.out_dup_enent+b.out_pre_err_event+b.out_useless_event+b.out_pric_err_event+b.out_format_err) "
	" from wf_stat_log b,b_event_file_list_cfg c "
	" where b.process_id in (select a.process_id from WF_PROCESS a where a.app_id in (%d)) "
	//" and to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and b.file_id=c.file_id "
  " and b.msg_date = '%s' "
	" and b.msg_time between '%s' and '%s' "		
	" group by busi_class"
	,PRICING_APP_ID,sBegDate,sBegTime,sEndTime);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	pricingInRecordCount[qry.field(0).asInteger()] = qry.field(1).asLong();
  	pricingOutRecordCount[qry.field(0).asInteger()] = qry.field(2).asLong();
  }  
  qry.close();   
  
  //拼接文件
	string kpiId;
	string kpiName;
	string alarmId;
	string kbpId;
	string kbpName;
	string occurTime;
	
	for(iter = m_pCodeList.begin();iter != m_pCodeList.end(); ++iter)
	{
		m_bPrepInOut=false;
		m_bRecvOutPrepIn=false;
		m_bPricingInOut=false;
		m_bPrepOutPricingIn=false;
		m_bPricingOutIndbIn=false;
		kbpId=iter->first;
		kbpName=iter->second;
		memset(m_source,0,sizeof(m_source));
		memset(m_busiclass,0,sizeof(m_busiclass));

		kbpId=string("/HB/")+kbpId;

		Date occurTime;
		int iCode = atoi((iter->first).c_str());
		if(pricingInRecordCount[iCode] != pricingOutRecordCount[iCode])
		{
			alarmId=string("/HB/")+string("0202101");
			kpiId=string("20-30-02-02-02-101");
			kpiName=string("批价处理话单数不平衡");
			insertAlarm(kpiId,kpiName,alarmId,kbpId,kbpName,occurTime,MODULE_PRICING);
		}
		if(prepOutRecordCount[iCode] != pricingInRecordCount[iCode])
		{
			alarmId=string("/HB/")+string("0202102");
			kpiId=string("20-30-02-02-02-102");
			kpiName=string("预处理正常输出话单数与批价接收话单数不平衡");
			insertAlarm(kpiId,kpiName,alarmId,kbpId,kbpName,occurTime,MODULE_PRICING);
		}
	}	
	/*if(pricingInRecordCount!=pricingOutRecordCount)
	{
		m_bPricingInOut=true;
	}
	if(prepOutRecordCount!=pricingInRecordCount)
	{
		m_bPrepOutPricingIn=true;
	}*/
	
  return true;
}

bool Information::checkIndbAlarm()
{
	DEFINE_QUERY(qry);
	char sSQL[1024];
	map<int,long> pricingOutRecordCount;
	map<int,long> IndbInRecordCount;
	
	char sBegTime[6+1]={0};
  char sEndTime[6+1]={0};
  char sBegDate[8+1]={0};
  Date dts(StatInterface::m_pPlan->m_sDataBeginTime);
  Date dte(StatInterface::m_pPlan->m_sDataEndTime);
  dts.addSec(1);
  strncpy(sBegDate, dts.toString(), 8);	
  strncpy(sBegTime,"000000",6);
  strncpy(sEndTime,"235959",6);
  
  //初始化
  qry.setSQL("select trim(code_id),code_name from b_info_code");
  qry.open();
  while (qry.next())
  {
  	m_pCodeList[qry.field(0).asString()]=qry.field(1).asString();
  }
	qry.close();
	
	map<string,string>::iterator iter;
	for(iter = m_pCodeList.begin();iter != m_pCodeList.end(); ++iter)
	{
		int iCode = atoi((iter->first).c_str());
		pricingOutRecordCount[iCode] =0;
		IndbInRecordCount[iCode] =0;
	}
	
  //pricingOutRecordCount=getAppInfo(PRICING_APP_ID,false);
  //IndbInRecordCount=getAppInfo(INSTORE_APP_ID,true);
  
  //pricingOutRecordCount
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select busi_class,sum(b.in_normal_event+b.in_black_event+b.in_dup_event+b.in_pre_err_event+b.in_useless_event+b.in_pric_err_event+b.format_err),"
	" sum(b.out_normal_event+b.out_black_event+b.out_dup_enent+b.out_pre_err_event+b.out_useless_event+b.out_pric_err_event+b.out_format_err) "
	" from wf_stat_log b,b_event_file_list_cfg c "
	" where b.process_id in (select a.process_id from WF_PROCESS a where a.app_id in (%d)) "
	//" and to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and b.file_id=c.file_id "
  " and b.msg_date = '%s' "
	" and b.msg_time between '%s' and '%s' "	
	" group by busi_class"
	,PRICING_APP_ID,sBegDate,sBegTime,sEndTime);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	pricingOutRecordCount[qry.field(0).asInteger()] = qry.field(2).asLong();
  }  
  qry.close(); 
  
  //IndbInRecordCount
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select busi_class,sum(b.in_normal_event+b.in_black_event+b.in_dup_event+b.in_pre_err_event+b.in_useless_event+b.in_pric_err_event+b.format_err),"
	" sum(b.out_normal_event+b.out_black_event+b.out_dup_enent+b.out_pre_err_event+b.out_useless_event+b.out_pric_err_event+b.out_format_err) "
	" from wf_stat_log b,b_event_file_list_cfg c "
	" where b.process_id in (select a.process_id from WF_PROCESS a where a.app_id in (%s)) "
	//" and to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and b.file_id=c.file_id "
  " and b.msg_date = '%s' "
	" and b.msg_time between '%s' and '%s' "	
	" group by busi_class"
	,INSTORE_APP_ID,sBegDate,sBegTime,sEndTime);
	//cout<<sSQL<<endl;
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	IndbInRecordCount[qry.field(0).asInteger()] = qry.field(1).asLong();
  }  
  qry.close();   
  
   /////拼接文件
	string kpiId;
	string kpiName;
	string alarmId;
	string kbpId;
	string kbpName;
	string occurTime;
	
	for(iter = m_pCodeList.begin();iter != m_pCodeList.end(); ++iter)
	{
		kbpId=iter->first;
		kbpName=iter->second;
		memset(m_source,0,sizeof(m_source));
		memset(m_busiclass,0,sizeof(m_busiclass));

		kbpId=string("/HB/")+kbpId;

		Date occurTime;
		int iCode = atoi((iter->first).c_str());
		if(pricingOutRecordCount[iCode] != IndbInRecordCount[iCode])
		{
				alarmId=string("/HB/")+string("0203101");
				kpiId=string("20-30-02-02-03-101");
				kpiName=string("批价正常输出话单数与入库接收话单数不平衡");
				insertAlarm(kpiId,kpiName,alarmId,kbpId,kbpName,occurTime,MODULE_INSTORE);
		}
	}	
	
	/*if(pricingOutRecordCount!=IndbInRecordCount)
	{
		m_bPricingOutIndbIn=true;
	}*/
	
  return true;
}

long Information::getAppInfo(int appId,bool in)
{
	long inRecordCount;
	long outRecordCount;
	
	DEFINE_QUERY(qry);
	char sSQL[1024];
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select sum(b.in_normal_event+b.in_black_event+b.in_dup_event+b.in_pre_err_event+b.in_useless_event+b.in_pric_err_event+b.format_err),"
	" sum(b.out_normal_event+b.out_black_event+b.out_dup_enent+b.out_pre_err_event+b.out_useless_event+b.out_pric_err_event+b.out_format_err) "
	" from wf_stat_log b,b_event_file_list_cfg c "
	" where b.process_id in (select a.process_id from WF_PROCESS a where a.app_id=%d) "
	"	and c.switch_id in (select a.switch_id from b_switch_info a where a.source_event_type in (%s)) "
	" and to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and b.file_id=c.file_id",appId,m_busiclass,StatInterface::m_pPlan->m_sDataBeginTime,StatInterface::m_pPlan->m_sDataEndTime);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	inRecordCount=qry.field(0).asLong();
  	outRecordCount=qry.field(1).asLong();
  }
  
  qry.close();
  
  if(in)
  	return inRecordCount;
  else
  	return outRecordCount;
}

bool Information::writePrepAlarm()
{
	StatInterface oPrepAlarmStat("0201",20,LOW1_FREG);
	
	vector<string> m_vPrepAlarm;
	m_vPrepAlarm.clear();
	m_vPrepAlarm.push_back(ALARM_TITLE);
	getAppAlarm(MODULE_PREP,m_vPrepAlarm);
	
	print(m_vPrepAlarm);
	oPrepAlarmStat.writeJsonFile(m_vPrepAlarm,SPLIT_FLAG);
	
  return true;
}

bool Information::writePricingAlarm()
{
	StatInterface oPricingAlarmStat("0202",20,LOW1_FREG);
	
	vector<string> m_vPricingAlarm;
	m_vPricingAlarm.clear();
	m_vPricingAlarm.push_back(ALARM_TITLE);
	getAppAlarm(MODULE_PRICING,m_vPricingAlarm);
	
	print(m_vPricingAlarm);
	oPricingAlarmStat.writeJsonFile(m_vPricingAlarm,SPLIT_FLAG);
	
  return true;
}

bool Information::writeIndbAlarm()
{
	StatInterface oIndbAlarmStat("0203",20,LOW1_FREG);
	
	vector<string> m_vIndbAlarm;
	m_vIndbAlarm.clear();
	m_vIndbAlarm.push_back(ALARM_TITLE);
	getAppAlarm(MODULE_INSTORE,m_vIndbAlarm);
	
	print(m_vIndbAlarm);
	oIndbAlarmStat.writeJsonFile(m_vIndbAlarm,SPLIT_FLAG);
	
  return true;
}

bool Information::getAppAlarm(int mode,vector<string> &vec)
{
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
	string count;
	
	DEFINE_QUERY(qry);
	char sSQL[1024];
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select a.kpiid,a.kpiname,a.alarmid,a.kbpid,a.kbpname,min(a.occurtime),max(a.occurtime),a.eventlevel,a.eventstate,count(*)"
	" From b_alarm_list a where a.modetpye=%d "
	" and a.occurtime between to_date(%s,'yyyymmddhh24miss') and to_date(%s,'yyyymmddhh24miss') "
	" group by a.kpiid,a.kpiname,a.alarmid,a.kbpid,a.kbpname,a.eventlevel,a.eventstate",
	mode,StatInterface::m_pPlan->m_sDataBeginTime,StatInterface::m_pPlan->m_sDataEndTime);
	
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
		count=qry.field(9).asString();
  	
  	temp=kpiId+string("|")+kpiName+string("|")+alarmId+string("|")+kbpId+string("|")+kbpName+string("|")+firstTime+string("|")
  	+lastTime+string("|")+eventLevel+string("|")+eventState+string("|")+string("|")+string("|")+string("|")+string("|")+count;
  	vec.push_back(temp);
  }
  
  return true;
}

//add 2010-11-22
long Information::getAppInfo(const char  *appId,bool in)
{
	long inRecordCount;
	long outRecordCount;
	
	DEFINE_QUERY(qry);
	char sSQL[1024];
	memset(sSQL, 0, sizeof(sSQL));
	sprintf(sSQL,"select sum(b.in_normal_event+b.in_black_event+b.in_dup_event+b.in_pre_err_event+b.in_useless_event+b.in_pric_err_event+b.format_err),"
	" sum(b.out_normal_event+b.out_black_event+b.out_dup_enent+b.out_pre_err_event+b.out_useless_event+b.out_pric_err_event+b.out_format_err) "
	" from wf_stat_log b,b_event_file_list_cfg c "
	" where b.process_id in (select a.process_id from WF_PROCESS a where a.app_id in (%s)) "
	"	and c.switch_id in (select a.switch_id from b_switch_info a where a.source_event_type in (%s)) "
	" and to_date(trim(b.msg_date)||trim(b.msg_time),'yyyymmddhh24miss') between to_date('%s','yyyymmddhh24miss')+1/(3600*24) and to_date('%s','yyyymmddhh24miss') "
	" and b.file_id=c.file_id",appId,m_busiclass,StatInterface::m_pPlan->m_sDataBeginTime,StatInterface::m_pPlan->m_sDataEndTime);
	
	qry.setSQL(sSQL);
	qry.open();
  while (qry.next())
  {
  	inRecordCount=qry.field(0).asLong();
  	outRecordCount=qry.field(1).asLong();
  }
  
  qry.close();
  
  if(in)
  	return inRecordCount;
  else
  	return outRecordCount;
}


void CDR_Info::init()
{
	recvAllFileCount = 0;
	recvDelayFileCount = 0;
	recvAllRecordCount = 0;
	recvDelayRecordCount = 0;
	recvErrFileCount = 0;
	prepAllRecordCount = 0;
	prepErrRecordCount = 0;
	prepDupRecordCount = 0;
	pricingAllRecordCount = 0;
	pricingUseLessRecordCount = 0;
	pricingErrRecordCount = 0;
	pricingDupRecordCount = 0;
	pricingBlackRecordCount = 0;
	pricingOkRecordCount = 0;
	indbInAllRecordCount = 0;
	indbOutAllRecordCount = 0;
	indbErrRecordCount = 0;	
}

void CDR_Info::parse()
{
	if(recvAllFileCount < 0)           recvAllFileCount = 0;          
	if(recvDelayFileCount < 0)         recvDelayFileCount = 0;        
	if(recvAllRecordCount < 0)         recvAllRecordCount = 0;        
	if(recvDelayRecordCount < 0)       recvDelayRecordCount = 0;      
	if(recvErrFileCount < 0)           recvErrFileCount = 0;          
	if(prepAllRecordCount < 0)         prepAllRecordCount = 0;        
	if(prepErrRecordCount < 0)         prepErrRecordCount = 0;        
	if(prepDupRecordCount < 0)         prepDupRecordCount = 0;        
	if(pricingAllRecordCount < 0)      pricingAllRecordCount = 0;     
	if(pricingUseLessRecordCount < 0)  pricingUseLessRecordCount = 0; 
	if(pricingErrRecordCount < 0)      pricingErrRecordCount = 0;     
	if(pricingDupRecordCount < 0)      pricingDupRecordCount = 0;     
	if(pricingBlackRecordCount < 0)    pricingBlackRecordCount = 0;   
	if(pricingOkRecordCount < 0)       pricingOkRecordCount = 0;      
	if(indbInAllRecordCount < 0)       indbInAllRecordCount = 0;      
	if(indbOutAllRecordCount < 0)      indbOutAllRecordCount = 0;     
	if(indbErrRecordCount < 0)	        indbErrRecordCount = 0;	 		
}

