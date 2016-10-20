#include "ExtractFileName.h"
#include "StatOneTwoGroup.h"
#include "Date.h"
#include "Log.h"
#include"interrupt.h"
#include "Environment.h"

ErrorData::ErrorData()
{
 	m_iOneFileID = 0;
	m_iTwoFileID = 0;
	m_iOneCount = 0;
	m_iTwoCount = 0;
 
 	memset(m_sOneFileName,0,sizeof(m_sOneFileName));
	memset(m_sTwoFileName,0,sizeof(m_sTwoFileName));
}

ErrorData::~ErrorData()
{
	;
}

int StatOneTwoGroup::LoadConfig()
{
	memset(m_sBeginTime,0,sizeof(m_sBeginTime));
	memset(m_sTypeList,0,sizeof(m_sTypeList));
	Date dThisTime;
	strncpy (m_sThisTime, dThisTime.toString(),sizeof(m_sThisTime));
	
	char psql[SQLLENG] = {0};
	sprintf(psql,"%s","SELECT begin_time,next_time,wait_time,type_list,STAT_TIME FROM comm.B_STAT_ONE_TWO_GROUP");
	
	DEFINE_QUERY(qry);
  qry.close();
  qry.setSQL (psql);
  qry.open();
  if (qry.next())
  {
		if(strlen(qry.field(1).asString()) > 0)
			strcpy (m_sBeginTime, qry.field(1).asString());
		else
			strcpy (m_sBeginTime, qry.field(0).asString());
	
		m_lWaitTime = qry.field(2).asInteger();
		strcpy (m_sTypeList, qry.field(3).asString());
		m_lStatTime = qry.field(4).asLong();
		m_lAddTime = qry.field(4).asLong();
		
		//取统计的开始时间
		Date dBegT(m_sBeginTime);
		if(dBegT > dThisTime)
		{
			Log::log (0, "统计的开始时间(%s)>当前时间(%s)",dBegT.toString(),dThisTime.toString());
			return 1;
		}
		else
		{
			;
		}
		
		//取一批统计的结束时间
		Date dEndTime(m_sBeginTime);
		dEndTime.addSec(m_lStatTime);
		if(dEndTime>dThisTime)
		{
			Log::log (0, "一批统计的结束时间(%s)>当前时间(%s)",dEndTime.toString(),dThisTime.toString());
			m_lWaitTime=dEndTime.diffSec(dThisTime);
		  return 2;
		}
		else
		{
			strncpy(m_sOneEndTime,dEndTime.toString(),sizeof(m_sOneEndTime));
		}
		
		//取二批统计的结束时间
		dEndTime.addSec(m_lAddTime);
		strncpy(m_sTwoEndTime,dEndTime.toString(),sizeof(m_sTwoEndTime));
		
	}
	else
	{
		Log::log (0, "表 comm.B_STAT_ONE_TWO_GROUP 没有配置请配置");
		qry.close();
		return -1;
	}
	qry.close();
}

StatOneTwoGroup::StatOneTwoGroup()
{
	m_iCommitCount = 0;
}

StatOneTwoGroup::~StatOneTwoGroup()
{
	;
}

bool StatOneTwoGroup::CommitAll(int iOpType)
{
	char psql[SQLLENG] = {0};
	DEFINE_QUERY(qry);
	if((0 == iOpType && m_iCommitCount > 1000) || (1 == iOpType && m_iCommitCount>0))
	{
		sprintf(psql,"%s","INSERT INTO B_STAT_ONE_TWO_GROUP_LOG \
		(ONE_FILE_ID, TWO_FILE_ID, ONE_RECORD_CNT, TWO_RECORD_CNT, \
		ONE_FILE_NAME, TWO_FILE_NAME, ERROR_MSG) \
		VALUES(:ONE_FILE_ID,:TWO_FILE_ID,:ONE_RECORD_CNT,\
		:TWO_RECORD_CNT,:ONE_FILE_NAME,:TWO_FILE_NAME,:ERROR_MSG)");
		
		qry.setSQL(psql);
		qry.setParamArray("ONE_FILE_ID",&(m_vErrorData[0].m_iOneFileID),sizeof(m_vErrorData[0]));
		qry.setParamArray("TWO_FILE_ID",&(m_vErrorData[0].m_iTwoFileID),sizeof(m_vErrorData[0]));
		qry.setParamArray("ONE_RECORD_CNT",&(m_vErrorData[0].m_iOneCount),sizeof(m_vErrorData[0]));
		qry.setParamArray("TWO_RECORD_CNT",&(m_vErrorData[0].m_iTwoCount),sizeof(m_vErrorData[0]));
		qry.setParamArray("ONE_FILE_NAME",(char **)&(m_vErrorData[0].m_sOneFileName),sizeof(m_vErrorData[0]),sizeof(m_vErrorData[0].m_sOneFileName));
		qry.setParamArray("TWO_FILE_NAME",(char **)&(m_vErrorData[0].m_sTwoFileName),sizeof(m_vErrorData[0]),sizeof(m_vErrorData[0].m_sTwoFileName));
		qry.setParamArray("ERROR_MSG",(char **)&(m_vErrorData[0].m_sErrorMsg),sizeof(m_vErrorData[0]),sizeof(m_vErrorData[0].m_sErrorMsg));
		
		try
		{       
		  qry.execute(m_iCommitCount);
	  }
	  catch(TOCIException& e) 
	  {
		  qry.executeDebug(m_iCommitCount);
	  }
	  qry.commit();
	  qry.close();
	  m_vErrorData.clear();
	  m_iCommitCount = 0;
	}
	else
	{
		;
	}
}

bool StatOneTwoGroup::UpdateCfgTable()
{
	char psql[SQLLENG]={0};
	sprintf(psql,"UPDATE comm.B_STAT_ONE_TWO_GROUP t SET t.next_time=TO_DATE('%s', 'yyyymmdd HH24:MI:SS')",m_sOneEndTime);
	DEFINE_QUERY(qry);
  qry.close();
  qry.setSQL (psql);
  if (qry.execute() == false)    
 		return false;
    
  if (qry.commit() == false)    
        return false;    
    
	qry.close ();
	
  return true;
  
  
}

bool StatOneTwoGroup::CompareGroup()
{
	if(0 == m_mapOne.size() || 0 == m_mapTwo.size())
		return true;
	
	map<string, string>::iterator iter;
	for(iter = m_mapOne.begin(); iter != m_mapOne.end(); iter++)
	{
		string sFileName = iter->first;
		char Temp[TEMPLENG] = {0};
		
		map<string, string>::iterator iter2;
    iter2 = m_mapTwo.find(sFileName);
		if(iter2 != m_mapTwo.end())
		{
			strncpy(Temp,(iter->second).c_str(),sizeof(Temp));
			char *pcount1 = ExtractFileName::GetSepData(Temp);
		  int  count1 = atoi(pcount1);
		  
		  memset(Temp,0,sizeof(Temp));
		  strncpy(Temp,(iter2->second).c_str(),sizeof(Temp));
		  char *pcount2 = ExtractFileName::GetSepData(Temp);
		  int  count2 = atoi(pcount2);
		  
		  if (count1 != count2)
		  {
		  	ErrorData errData;
		  	memset(Temp,0,sizeof(Temp));
		 		strncpy(Temp,(iter->second).c_str(),sizeof(Temp));
		 		char *pFileId = ExtractFileName::GetSepData(Temp,2);
		 		if(NULL == pFileId)
					errData.m_iOneFileID=0;
				else
					errData.m_iOneFileID=atoi(pFileId);
				
				char *pCount = ExtractFileName::GetSepData(Temp,1);
				if(NULL == pCount)
					errData.m_iOneCount = 0;
				else
					errData.m_iOneCount = atoi(pCount);
					
				memset(Temp,0,sizeof(Temp));
		 		strncpy(Temp,(iter2->second).c_str(),sizeof(Temp));
		 		pFileId = ExtractFileName::GetSepData(Temp,2);
		 		if(NULL == pFileId)
					errData.m_iTwoFileID=0;
				else
					errData.m_iTwoFileID=atoi(pFileId);
					
				pCount = ExtractFileName::GetSepData(Temp,1);
				if(NULL == pCount)
					errData.m_iTwoCount = 0;
				else
					errData.m_iTwoCount = atoi(pCount);
					
				sprintf(errData.m_sErrorMsg,"一二批存在文件 %s [%s] | %s [%s]，两文件话单数不一样",\
		  		        (iter->first).c_str(),(iter->second).c_str(),\
		  		        (iter2->first).c_str(),(iter2->second).c_str());
		  		        
		  	Log::log (0, "%s",errData.m_sErrorMsg);
		  	m_vErrorData.push_back(errData);
				++m_iCommitCount;
			}
			else
			{
				;
			}
		}
		else
		{
			ErrorData errData;
			memset(Temp,0,sizeof(Temp));
		 	strncpy(Temp,(iter->second).c_str(),sizeof(Temp));
		 	char *pFileId = ExtractFileName::GetSepData(Temp,2);
		 	if(NULL == pFileId)
				errData.m_iOneFileID=0;
			else
				errData.m_iOneFileID=atoi(pFileId);
				
			char *pCount = ExtractFileName::GetSepData(Temp,1);
			if(NULL == pCount)
				errData.m_iOneCount = 0;
			else
				errData.m_iOneCount = atoi(pCount);
				
			strncpy(errData.m_sOneFileName,(iter->first).c_str(),sizeof(errData.m_sOneFileName));
			sprintf(errData.m_sErrorMsg,"一批存在文件 %s [%s] ，二批不存在",(iter->first).c_str(),(iter->second).c_str());
			
			Log::log (0, "%s",errData.m_sErrorMsg);
			m_vErrorData.push_back(errData);
			++m_iCommitCount;
		}
	}
	
	CommitAll(1);
	
	return true;
}

bool StatOneTwoGroup::GetOneGroup()
{
	char psql[SQLLENG] = {0};
	
	sprintf(psql,"SELECT t.merge_file_name,t.record_cnt,t.merge_file_id \
  FROM BILL.B_PACKAGE_STATE_MERGE T \
 WHERE T.STATE_DATE >= TO_DATE('%s', 'yyyymmdd HH24:MI:SS') \
   AND T.STATE_DATE < TO_DATE('%s', 'yyyymmdd HH24:MI:SS') \
   AND T.TRANS_FILE_TYPE_ID %s",m_sBeginTime,m_sOneEndTime,m_sTypeList);
   
  printf("GetOneGroup: \n %s \n",psql);
   
  DEFINE_QUERY(qry);
  qry.close();
  qry.setSQL (psql);
  qry.open();
  while (qry.next())
  {
  	string sFileName(qry.field(0).asString());
  	char pTemp[TEMPLENG] = {0};
  	sprintf(pTemp,"%d_%d",qry.field(1).asInteger(),qry.field(2).asInteger());
  	
  	 m_mapOne.insert(pair<string, string>(sFileName.c_str() ,pTemp));
	}
   
  qry.close();
  return true;
}

bool StatOneTwoGroup::GetTwoGroup()
{	
	char psql[SQLLENG] = {0};
	
	sprintf(psql,"SELECT t.file_name,t.record_cnt,t.file_id \
  FROM bill.b_event_file_list T \
 WHERE T.created_date >= TO_DATE('%s', 'yyyymmdd HH24:MI:SS') \
   AND T.created_date <= TO_DATE('%s', 'yyyymmdd HH24:MI:SS')",m_sBeginTime,m_sTwoEndTime);
   
  printf("GetTwoGroup: \n %s \n",psql);
  
  DEFINE_QUERY(qry);
  qry.close();
  qry.setSQL (psql);
  qry.open();
  while (qry.next())
  {
		char *pFileName = ExtractFileName::GetFileName(qry.field(0).asString(),'/');
		if (NULL == pFileName)
			return false;
			
  	char pTemp[TEMPLENG] = {0};
  	sprintf(pTemp,"%d_%d",qry.field(1).asInteger(),qry.field(2).asInteger());
  	
  	m_mapTwo.insert(pair<string, string>(pFileName ,pTemp));
	}
	
	qry.close();
  return true;
}

bool StatOneTwoGroup::ControlMgr()
{
	// 接管中断
	DenyInterrupt();
	
	while(!GetInterruptFlag())
	{
		int iLoadCfg = LoadConfig();
		if(iLoadCfg == -1)
			return -1;
		else if(iLoadCfg == 1 || iLoadCfg == 2)
		{
			printf("统计一次结束 等待 %d 秒 \n",m_lWaitTime);
			sleep(m_lWaitTime);
			continue;
		}
		else
		{
			;
		}
			
		if( !GetOneGroup())
			return -1;
			
		if(!GetTwoGroup())
			return -1;
			
		if(!CompareGroup())
			return -1;
			
		if(!UpdateCfgTable())
			return -1;
			
		printf("统计一次结束 等待 %d 秒 \n",m_lWaitTime);
			
		sleep(m_lWaitTime);
	}
}
	
int main()
{
	int i =0;
	StatOneTwoGroup objStatOneTwoGroup;
	objStatOneTwoGroup.ControlMgr();
	return 0;
}

