#include "WfStatLogMgr.h"

#define _JUST_DBLINK_
#include "Environment.h"

const int MAX_STAT_LOG_RECORD = 10000;
const int MAX_STAT_LOG_FILE = 1000;

bool __InterruptFlag;

void __Interrupt(int Signo)
{
	signal(Signo,__Interrupt);
	__InterruptFlag = true;
}

WfStatLogMgr::WfStatLogMgr()
{
	m_lLogFileList.clear();
	
	memset(m_sLogCloseFilePath, 0, sizeof(m_sLogCloseFilePath));
	snprintf(m_sLogCloseFilePath, sizeof(m_sLogCloseFilePath), "%s/closefile", Process::m_sLogFilePath);
		
	m_pStatLog = new TStatLog[MAX_STAT_LOG_RECORD]();
	m_iStatLogCnt = 0;
}	

WfStatLogMgr::~WfStatLogMgr()
{
	if (m_pStatLog != NULL)
	{
		delete [] m_pStatLog;
		m_pStatLog = NULL; 
	}		
}	

bool WfStatLogMgr::getLogFile()
{
	//m_lLogFileList.clear();
	
	//m_pDirectory->getFileRecursive(m_sLogCloseFilePath, m_lLogFileList);
	
	getFileRecursive(m_sLogCloseFilePath, m_lLogFileList);
	
	if (m_lLogFileList.empty())
		return false;
		
	return true;	
}

int WfStatLogMgr::indbStatLog()
{
	TStatLog *pStatLog = m_pStatLog;
	int iCnt = 0;
	
	for (m_pLogFile = m_lLogFileList.begin(); m_pLogFile != m_lLogFileList.end(); ++m_pLogFile)
	{
		
		m_pFile = fopen((*m_pLogFile).getFullFileName(), "rb+");
		 
		//while (fread(pStatLog++, sizeof(TStatLog), 1, m_pFile) == 1)
		while (fread(pStatLog, sizeof(TStatLog), 1, m_pFile) == 1)
		{
			++iCnt;
			//加入键值合并逻辑
			if (mergeStatLog(pStatLog))
			{
				continue;
			}
				
			pStatLog++;
			++m_iStatLogCnt;
			
			if (m_iStatLogCnt == MAX_STAT_LOG_RECORD)
			{
				//TStatLogMgr::indbStatLog(m_pStatLog, m_iStatLogCnt);
				writeWfStatLog();
				pStatLog = m_pStatLog;
			}	
		}
		
		fclose (m_pFile); 
		m_pFile = NULL;
	}
	
	if (m_iStatLogCnt > 0)
	{
		//TStatLogMgr::indbStatLog(m_pStatLog, m_iStatLogCnt);
		writeWfStatLog();
		pStatLog = m_pStatLog;		
	}
	
	return iCnt;
}

int WfStatLogMgr::delLogFile()
{
	int iCnt = 	m_lLogFileList.size();
	
	for (m_pLogFile = m_lLogFileList.begin(); m_pLogFile != m_lLogFileList.end(); ++m_pLogFile)
	{
		remove((*m_pLogFile).getFullFileName());
	}
	
	m_lLogFileList.clear();
	
	return iCnt;
}	

int WfStatLogMgr::bakLogFile()
{
	int iCnt = 	m_lLogFileList.size();
	
	for (m_pLogFile = m_lLogFileList.begin(); m_pLogFile != m_lLogFileList.end(); ++m_pLogFile)
	{
		//rename
	}
	
	m_lLogFileList.clear();
	
	return iCnt;	
}	

int WfStatLogMgr::getFileRecursive(char * sPath , list<File> &logFileList)
{
	struct dirent *dp;
	struct stat statbuf;
	char sFileName[500] = {0};
	int iFileCnt = 0;
	
	DIR * pDir = NULL;
	
	pDir = opendir(sPath);
	if (pDir == NULL)
		return 0;
	
	while ((dp = readdir(pDir)) != NULL) 
	{
		if (dp->d_name[0] == '.')
			continue;
			
		snprintf(sFileName, 500, "%s/%s", sPath, dp->d_name);        
		if(lstat(sFileName, &statbuf)<0 )
			continue;
		  
		if( S_ISDIR(statbuf.st_mode) == 0  )
		{ ////是文件
			File file(sPath, dp->d_name);
			logFileList.push_back(file);
			
			iFileCnt++;
			if(iFileCnt >= MAX_STAT_LOG_FILE) 
				break;
			
			continue; 
		}
		
		strcat(sFileName, "/");  
		this->getFileRecursive(sFileName, logFileList);
	} 
	
	closedir (pDir);
	pDir = NULL;
	return 1;
}

bool WfStatLogMgr::mergeStatLog(TStatLog *pStatLog)
{
	//PROCESS_ID_FileID_FROMPROCESSID_DESPROCESSUD相同的则合并记录,记录日期取新的,其他属性累加
    static char sTMapKey[64];
    static string sMapKey;
    
    memset(sTMapKey, 0 ,sizeof(sTMapKey));
	sprintf(sTMapKey, "%d_%d_%d_%d", pStatLog->m_iProcessID, pStatLog->m_iFileID, pStatLog->m_iFromProcessID, pStatLog->m_iDescProcessID);
	sMapKey = sTMapKey;
	
	m_pMergeIter = m_mapStatLog.find(sMapKey);
	
	if (m_pMergeIter ==  m_mapStatLog.end())
	{
		m_mapStatLog[sMapKey] = pStatLog;
		return false;
	}	
	
	TStatLog *pMergeStatLog = m_pMergeIter->second;
	
	//更新时间
	strcpy(pMergeStatLog->m_sMsgDate, pStatLog->m_sMsgDate);
	strcpy(pMergeStatLog->m_sMsgTime, pStatLog->m_sMsgTime);	   
	
	//合并属性
	pMergeStatLog->m_iInNormal          += pStatLog->m_iInNormal;         
	pMergeStatLog->m_iInBlack           += pStatLog->m_iInBlack;          
	pMergeStatLog->m_iInDup             += pStatLog->m_iInDup;            
	pMergeStatLog->m_iInPreErr          += pStatLog->m_iInPreErr;         
	pMergeStatLog->m_iInUseless         += pStatLog->m_iInUseless;        
	pMergeStatLog->m_iOutNormal         += pStatLog->m_iOutNormal;        
	pMergeStatLog->m_iOutBlack          += pStatLog->m_iOutBlack;         
	pMergeStatLog->m_iOutDup            += pStatLog->m_iOutDup;           
	pMergeStatLog->m_iOutPreErr         += pStatLog->m_iOutPreErr;        
	pMergeStatLog->m_iOutPricErr        += pStatLog->m_iOutPricErr;       
	pMergeStatLog->m_iOutUseless        += pStatLog->m_iOutUseless;       
	pMergeStatLog->m_iFormatErr         += pStatLog->m_iFormatErr;        
	pMergeStatLog->m_iInPricErr         += pStatLog->m_iInPricErr;        
	pMergeStatLog->m_iOutFormatErr      += pStatLog->m_iOutFormatErr;     
	pMergeStatLog->m_iRemainA           += pStatLog->m_iRemainA;          
	pMergeStatLog->m_iRemainB           += pStatLog->m_iRemainB;          
	pMergeStatLog->m_iDistributeRemainA += pStatLog->m_iDistributeRemainA;
	pMergeStatLog->m_iDistributeRemainB += pStatLog->m_iDistributeRemainB;
	pMergeStatLog->m_lNormalDuration    += pStatLog->m_lNormalDuration;   
	pMergeStatLog->m_lNormalAmount      += pStatLog->m_lNormalAmount;     
	pMergeStatLog->m_lNormalCharge      += pStatLog->m_lNormalCharge;     
	pMergeStatLog->m_lBlackDuration     += pStatLog->m_lBlackDuration;    
	pMergeStatLog->m_lBlackAmount       += pStatLog->m_lBlackAmount;      
	pMergeStatLog->m_lBlackCharge       += pStatLog->m_lBlackCharge;      
	pMergeStatLog->m_lDupDuration       += pStatLog->m_lDupDuration;      
	pMergeStatLog->m_lDupAmount         += pStatLog->m_lDupAmount;        
	pMergeStatLog->m_lDupCharge         += pStatLog->m_lDupCharge;        
	pMergeStatLog->m_lPreErrDuration    += pStatLog->m_lPreErrDuration;   
	pMergeStatLog->m_lPreErrAmount      += pStatLog->m_lPreErrAmount;     
	pMergeStatLog->m_lPreErrCharge      += pStatLog->m_lPreErrCharge;     
	pMergeStatLog->m_lPricErrDuration   += pStatLog->m_lPricErrDuration;  
	pMergeStatLog->m_lPricErrAmount     += pStatLog->m_lPricErrAmount;    
	pMergeStatLog->m_lPricErrCharge     += pStatLog->m_lPricErrCharge;    
	pMergeStatLog->m_lUseLessDuration   += pStatLog->m_lUseLessDuration;  
	pMergeStatLog->m_lUseLessAmount     += pStatLog->m_lUseLessAmount;    
	pMergeStatLog->m_lUseLessCharge     += pStatLog->m_lUseLessCharge;    
	pMergeStatLog->m_lFormatErrDuration += pStatLog->m_lFormatErrDuration;
	pMergeStatLog->m_lFormatErrAmount   += pStatLog->m_lFormatErrAmount;  
	pMergeStatLog->m_lFormatErrCharge	+= pStatLog->m_lFormatErrCharge;   
	
	return true;
}	

int WfStatLogMgr::writeWfStatLog()
{                               
	int iCnt = m_iStatLogCnt;
	
	DEFINE_QUERY(qry);
	
    qry.setSQL ("insert into wf_stat_log(stat_log_id,process_log_id,msg_date,msg_time, "
        " process_id,FromProcessID,DESCPROCESSID,billflow_id,file_id,in_normal_event,in_black_event,   "
        " in_dup_event,in_pre_err_event,in_useless_event,out_normal_event, "
        " out_black_event,out_dup_enent,out_pre_err_event,out_pric_err_event,out_useless_event, "
        " format_err,in_pric_err_event,out_format_err,remain_a,remain_b , "
        " distributeRemian_A,distributeRemian_B, "
        " normal_duration, normal_amount, normal_charge, "
        " black_duration, black_amount, black_charge, "
        " dup_duration, dup_amount, dup_charge, "
        " preerr_duration, preerr_amount, preerr_charge, "
        " pricerr_duration, pricerr_amount, pricerr_charge, "
        " useless_duration, useless_amount, useless_charge, "
        " formaterr_duration, formaterr_amount, formaterr_charge "
        "  ) values "
        "(seq_stat_log_id.nextval, :ProcLogID, :msg_date, :msg_time, "
        " :iProcessID, :FromProcessID, :iDescProcessID, :g_iBillflowID, :m_iFileID, :m_iInNormal, :m_iInBlack, "
        " :m_iInDup,    :m_iInPreErr,  :m_iInUseless,   :m_iOutNormal, "
        " :m_iOutBlack, :m_iOutDup,    :m_iOutPreErr,   :m_iOutPricErr,:m_iOutUseless, "
        " :m_iFormatErr,:m_iInPricErr, :m_iOutFormatErr,:m_iRemainA,:m_iRemainB, "
        " :m_iDistributeRemainA, :m_iDistributeRemainB, "
        " :m_lNormalDuration, :m_lNormalAmount, :m_lNormalCharge, "
        " :m_lBlackDuration, :m_lBlackAmount, :m_lBlackCharge, "
        " :m_lDupDuration, :m_lDupAmount, :m_lDupCharge, "
        " :m_lPreErrDuration, :m_lPreErrAmount, :m_lPreErrCharge, "
        " :m_lPricErrDuration, :m_lPricErrAmount, :m_lPricErrCharge, "
        " :m_lUseLessDuration, :m_lUseLessAmount, :m_lUseLessCharge, "
        " :m_lFormatErrDuration, :m_lFormatErrAmount, :m_lFormatErrCharge) "
    );	
    
    qry.setParamArray ("ProcLogID", &(m_pStatLog[0].m_iProcLogID), sizeof(m_pStatLog[0]));
    qry.setParamArray ("msg_date", (char **)&(m_pStatLog[0].m_sMsgDate), sizeof(m_pStatLog[0]), sizeof(m_pStatLog[0].m_sMsgDate));
    qry.setParamArray ("msg_time", (char **)&(m_pStatLog[0].m_sMsgTime), sizeof(m_pStatLog[0]), sizeof(m_pStatLog[0].m_sMsgTime));    
    qry.setParamArray ("iProcessID", &(m_pStatLog[0].m_iProcessID), sizeof(m_pStatLog[0]));    
    qry.setParamArray ("FromProcessID",&(m_pStatLog[0].m_iFromProcessID), sizeof(m_pStatLog[0]));
	qry.setParamArray ("iDescProcessID",&(m_pStatLog[0].m_iDescProcessID), sizeof(m_pStatLog[0]));
    qry.setParamArray ("g_iBillflowID", &(m_pStatLog[0].m_iBillflowID), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iFileID", &(m_pStatLog[0].m_iFileID), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iInNormal", &(m_pStatLog[0].m_iInNormal), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iInBlack", &(m_pStatLog[0].m_iInBlack), sizeof(m_pStatLog[0]));
    
    qry.setParamArray ("m_iInDup", &(m_pStatLog[0].m_iInDup), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iInPreErr", &(m_pStatLog[0].m_iInPreErr), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iInUseless", &(m_pStatLog[0].m_iInUseless), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iOutNormal", &(m_pStatLog[0].m_iOutNormal), sizeof(m_pStatLog[0]));
    
    qry.setParamArray ("m_iOutBlack", &(m_pStatLog[0].m_iOutBlack), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iOutDup", &(m_pStatLog[0].m_iOutDup), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iOutPreErr", &(m_pStatLog[0].m_iOutPreErr), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iOutPricErr", &(m_pStatLog[0].m_iOutPricErr), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iOutUseless", &(m_pStatLog[0].m_iOutUseless), sizeof(m_pStatLog[0]));
    
    qry.setParamArray ("m_iFormatErr", &(m_pStatLog[0].m_iFormatErr), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iInPricErr", &(m_pStatLog[0].m_iInPricErr), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iOutFormatErr", &(m_pStatLog[0].m_iOutFormatErr), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iRemainA", &(m_pStatLog[0].m_iRemainA), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iRemainB", &(m_pStatLog[0].m_iRemainB), sizeof(m_pStatLog[0]));
		
	qry.setParamArray ("m_iDistributeRemainA", &(m_pStatLog[0].m_iDistributeRemainA), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_iDistributeRemainB", &(m_pStatLog[0].m_iDistributeRemainB), sizeof(m_pStatLog[0]));
    
    qry.setParamArray ("m_lNormalDuration", &(m_pStatLog[0].m_lNormalDuration), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lNormalAmount", &(m_pStatLog[0].m_lNormalAmount), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lNormalCharge", &(m_pStatLog[0].m_lNormalCharge), sizeof(m_pStatLog[0]));
    
    qry.setParamArray ("m_lBlackDuration", &(m_pStatLog[0].m_lBlackDuration), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lBlackAmount", &(m_pStatLog[0].m_lBlackAmount), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lBlackCharge", &(m_pStatLog[0].m_lBlackCharge), sizeof(m_pStatLog[0]));
    
    qry.setParamArray ("m_lDupDuration", &(m_pStatLog[0].m_lDupDuration), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lDupAmount", &(m_pStatLog[0].m_lDupAmount), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lDupCharge", &(m_pStatLog[0].m_lDupCharge), sizeof(m_pStatLog[0]));
    
    qry.setParamArray ("m_lPreErrDuration", &(m_pStatLog[0].m_lPreErrDuration), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lPreErrAmount", &(m_pStatLog[0].m_lPreErrAmount), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lPreErrCharge", &(m_pStatLog[0].m_lPreErrCharge), sizeof(m_pStatLog[0]));
    
    qry.setParamArray ("m_lPricErrDuration", &(m_pStatLog[0].m_lPricErrDuration), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lPricErrAmount", &(m_pStatLog[0].m_lPricErrAmount), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lPricErrCharge", &(m_pStatLog[0].m_lPricErrCharge), sizeof(m_pStatLog[0]));
    
    qry.setParamArray ("m_lUseLessDuration", &(m_pStatLog[0].m_lUseLessDuration), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lUseLessAmount", &(m_pStatLog[0].m_lUseLessAmount), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lUseLessCharge", &(m_pStatLog[0].m_lUseLessCharge), sizeof(m_pStatLog[0]));
    
    qry.setParamArray ("m_lFormatErrDuration", &(m_pStatLog[0].m_lFormatErrDuration), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lFormatErrAmount", &(m_pStatLog[0].m_lFormatErrAmount), sizeof(m_pStatLog[0]));
    qry.setParamArray ("m_lFormatErrCharge", &(m_pStatLog[0].m_lFormatErrCharge), sizeof(m_pStatLog[0]));
    
	qry.execute(m_iStatLogCnt);
	qry.commit();
	qry.close();
	
	m_iStatLogCnt = 0;   //清零 
	m_mapStatLog.clear(); //目前只能基于批次的合并
	
	return iCnt;
}

int WfStatLogMgr::run()	
{
    // 接管中断     
	signal(SIGINT,__Interrupt);
	signal(SIGQUIT,__Interrupt);
	signal(SIGTERM,__Interrupt);

	signal(SIGFPE,__Interrupt);
	signal(SIGUSR1,__Interrupt);
	    
    while (!__InterruptFlag)
    {
    	while (getLogFile())
    	{
    		AddTicketStat (TICKET_NORMAL, indbStatLog());//add
    		//indbStatLog();
    		delLogFile();
    	}
    	sleep(30);
    }    
    
    return 1;
}

DEFINE_MAIN(WfStatLogMgr)
