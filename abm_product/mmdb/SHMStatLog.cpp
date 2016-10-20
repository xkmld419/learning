/*VER: 1.0*/ 
//#include "DataIncrementMgr.h"
#include <stdlib.h>
#include "Environment.h"
#include "Log.h"
#include "SeqMgrExt.h"
#include "SHMStatLog.h"


SHMData_A<SHMStatLogData> * SHMStatLogBase::m_poSHMStatLogData = 0;
//SHMIntHashIndex * SHMStatLogBase::m_poFileIDIndex = 0;
SHMIntHashIndex_A * SHMStatLogBase::m_poFileAppIndex = 0;
SHMStatLogData * SHMStatLogBase::m_poLogData = 0;
bool SHMStatLogBase::m_bAttached = false;
	
CSemaphore * SHMStatLogMgr::m_poDataLock = 0;
CSemaphore * SHMStatLogMgr::m_poIndexLock = 0;

SHMStatLogBase::SHMStatLogBase()	
{
	if (m_bAttached)
        return;
    m_bAttached = true; 


    ATTACH_DATA_A(m_poSHMStatLogData, SHMStatLogData, SHM_STAT_LOG_DATA);
    //ATTACH_INT_INDEX(m_poFileIDIndex, SHM_STAT_LOG_INDEX);
   ATTACH_INT_INDEX_A(m_poFileAppIndex,SHM_STAT_LOG_INDEX) ;

    if (m_bAttached)
        bindData ();        
}
void SHMStatLogBase::bindData()
{
    m_poLogData = (SHMStatLogData *)(*m_poSHMStatLogData);
}

void SHMStatLogBase::freeAll()
{
    if (m_poSHMStatLogData) {
        delete m_poSHMStatLogData;
        m_poSHMStatLogData = 0;
    }
   /*
    if (m_poFileIDIndex) {
        delete m_poFileIDIndex;
        m_poFileIDIndex = 0;
    }
    */
    if (m_poFileAppIndex) {
        delete m_poFileAppIndex;
        m_poFileAppIndex = 0;
    }    
}

SHMStatLogCtl::SHMStatLogCtl() : SHMStatLogBase()
{
}

bool SHMStatLogCtl::exist()
{
    return m_bAttached;
}

void SHMStatLogCtl::remove()
{
    if (m_poSHMStatLogData->exist ()) {
        m_poSHMStatLogData->remove ();
    }

    if (m_poFileAppIndex->exist ()) {
        m_poFileAppIndex->remove ();
    }    
}

void SHMStatLogCtl::create()
{
    if(!m_poSHMStatLogData->exist())
        m_poSHMStatLogData->create (SHM_STAT_LOG_COUNT);
    
    if(!m_poFileAppIndex->exist())
        m_poFileAppIndex->create (SHM_STAT_LOG_INDEX_COUNT);
      
    bindData ();
    m_bAttached = true;
}
	
SHMStatLogMgr::SHMStatLogMgr() : SHMStatLogBase()
{
	
	  m_poSeqExt = new SeqMgrExt ("seq_stat_log_id", 10000);
	  
    char sTemp[32];

    if (!m_poDataLock) {
        m_poDataLock = new CSemaphore ();
        sprintf (sTemp, "%d", SHM_STAT_LOG_LOCK);
        m_poDataLock->getSem (sTemp, 1, 1);
    }

    if (!m_poIndexLock) {
        m_poIndexLock = new CSemaphore ();
        sprintf (sTemp, "%d", SHM_STAT_LOG_INDEX_LOCK);
        m_poIndexLock->getSem (sTemp, 1, 1);
    }	    
    
    char sValue[4+1]={0};
    if (ParamDefineMgr::getParam("CHECK_STAT", "PROVINCE_CODE", sValue, -1))
    {
    	strncpy(m_sProvCode, sValue, 4);    	
    }     
    m_iProvCode = atoi(m_sProvCode);
}
	

void SHMStatLogMgr::initRecord(SHMStatLogData * pData,int iFileID,int iAppID,char *sFileName)
{
	 memset(pData,0,sizeof(SHMStatLogData));
	 pData->m_iFileID = iFileID ;
   pData->m_iAppID = iAppID;
   pData->m_iDrID = DR_ID ;
   pData->m_iSourceID = m_iProvCode;
   memset(pData->m_sFileName,0,MAX_FILE_NAME_LEN);
   strncpy(pData->m_sFileName,sFileName,MAX_FILE_NAME_LEN-1);
   Date dt ;
   memset(pData->m_sStartTime,0,sizeof(pData->m_sStartTime));
   strncpy(pData->m_sStartTime,dt.toString(),14);
   
	pData->m_sInLog.m_iType=0;   
	for(int i=0;i<CDR_TYPE_NUMS;i++)
	 {
	  pData->m_sALLInLog[i].m_iType= i+1;
	}
	
	pData->m_sOutLog.m_iType=0;
	for(int i=0;i<CDR_TYPE_NUMS;i++)
	 {
	  pData->m_sALLOutLog[i].m_iType= i+1;
	}	
}

void SHMStatLogMgr::setOver(int iFileID,int iAppID)
{
	  unsigned int i ;
	  getStringIndex(iFileID,iAppID) ;
	  SHMStatLogData * p = m_poLogData;
	 if (!m_poFileAppIndex->get (m_lLongIndex, &i)) {
	 
        Log::log (1, "THE LOG FILEID:%d--APP_ID:%d IS NOT EXIST!",iFileID,iAppID);        
        return ;
    }	 
	 
  // p[i].m_iIsOver = 1;
   return ;            
}
	
	
//记录LogInfo
void SHMStatLogMgr::addLogInfo(LogInfo *pLog,int iCnt,StdEvent *pEvt)
{
     pLog->m_iNum += iCnt;
     for(int i=0 ;i<MAX_CHARGE_NUM;i++)
     {
      pLog->m_lCharge += pEvt->m_oChargeInfo[i].m_lCharge;
     }    
     pLog->m_lDulation += pEvt->m_lDuration ;
     pLog->m_lAmount +=  pEvt->m_lSumAmount ; 	
}	


//记录输入记录数函数
bool SHMStatLogMgr::addInEvent(int iFileID,int iProcessID,int iCnt,StdEvent *pEvt)
{
	  unsigned int i ;
	  char sTmpTime[14+1]={0};
	  SHMStatLogData * p = m_poLogData;
	  //int iAppID = getAppIDByProcess(iProcessID);
	  int iAppID = iProcessID ;
	  getStringIndex(iFileID,iAppID) ;
	 if (!m_poFileAppIndex->get (m_lLongIndex, &i)) {
	 	    m_poDataLock->P();
        i = m_poSHMStatLogData->malloc ();
        if(!i) THROW(MBC_ParamShmMgr+4);
        m_poIndexLock->P();
        m_poFileAppIndex->add(m_lLongIndex,i);
        m_poIndexLock->V();
        initRecord(p+i,iFileID,iAppID,pEvt->m_oEventExt.m_sFileName) ;
        p[i].m_iModuleID = getBusiclassBySwitch(pEvt->m_iSwitchID);
        p[i].m_sInLog.m_iNum += iCnt;
       for(int i=0 ;i<MAX_CHARGE_NUM;i++)
        {
         p[i].m_sInLog.m_lCharge += pEvt->m_oChargeInfo[i].m_lCharge;
        }
        p[i].m_sInLog.m_lDulation += pEvt->m_lDuration ;
        p[i].m_sInLog.m_lAmount +=  pEvt->m_lSumAmount ; 
        addInEvent(p+i,pEvt->m_iEventTypeID,iCnt,pEvt);     
        m_poDataLock->V();               
        return true ;

    }
 
   m_poDataLock->P();
   p[i].m_sInLog.m_iNum += iCnt;
    for(int i=0 ;i<MAX_CHARGE_NUM;i++)
    {
     p[i].m_sInLog.m_lCharge += pEvt->m_oChargeInfo[i].m_lCharge;
    }
   p[i].m_sInLog.m_lDulation += pEvt->m_lDuration ;
   p[i].m_sInLog.m_lAmount +=  pEvt->m_lSumAmount ;   
   addInEvent(p+i,pEvt->m_iEventTypeID,iCnt,pEvt);   
   m_poDataLock->V();       
   return true;

  
}

void SHMStatLogMgr::addInEvent(SHMStatLogData * pData,int iEvtTypeID,int iCnt,StdEvent *pEvt)
{
	if(!pData)
		{
			return ;
		}
	 int iEvtType = 0;

    iEvtType = iEvtTypeID;
    LogInfo *pLog=NULL;
    
    switch (iEvtType) {
      case CYCLE_FEE_EVENT_TYPE: //周期性费用事件
      case CALL_EVENT_TYPE:      //语音事件      	
      case DATA_EVENT_TYPE:      //数据事件     
      case VALUE_ADDED_EVENT_TYPE: //增值事件     
      case 0: //预处理正常事件      	 	 	
      	  pLog = pData->m_sALLInLog+NORMAL ;   
      	  break ;   
      case DUP_EVENT_TYPE: //重复
      	  pLog = pData->m_sALLInLog+DUP ;	
          break;
      case NOOWNER_EVENT_TYPE: //无主
      	  pLog = pData->m_sALLInLog+BlACK_ERR ;  	
          break;
      case PP_PARAM_ERR_EVENT_TYPE: //预处理参数错误
      	  pLog = pData->m_sALLInLog+PREP_ERR ;      	   	
          break;
      case RATE_PARAM_ERR_EVENT_TYPE: //批价参数错误
      	  pLog = pData->m_sALLInLog+PRIC_ERR ;      	  	
          break;
      case OTHER_EVENT_TYPE: //无效
      	  pLog = pData->m_sALLInLog+INVALID ;      	    	
          break;
      case ERR_EVENT_TYPE:   //格式错误(预处理)
      	  pLog = pData->m_sALLInLog+FORM_ERR ;      	    	
          break;

      default:
      	  pLog = pData->m_sALLInLog+NORMAL ;      	 	
    }    
     if(pLog)
     {	
       addLogInfo( pLog,iCnt,pEvt) ;   
     }             
}

	
//记录输出记录数函数
bool SHMStatLogMgr::addOutEvent(int iFileID,int iAppID,int iCnt,StdEvent *pEvt)
{
	  unsigned int i ;
   SHMStatLogData * p = m_poLogData;	 
    getStringIndex(iFileID,iAppID) ; 
	 if (!m_poFileAppIndex->get (m_lLongIndex, &i)) {

        Log::log (1, "THE LOG FILEID:%d--PROCESS_ID:%d IS NOT EXIST!",iFileID,iAppID);        
        return false ;
    }

    m_poDataLock->P();
    p[i].m_sOutLog.m_iNum += iCnt;
    for(int i=0 ;i<MAX_CHARGE_NUM;i++)
    {
     p[i].m_sOutLog.m_lCharge += pEvt->m_oChargeInfo[i].m_lCharge;
    }    
    p[i].m_sOutLog.m_lDulation += pEvt->m_lDuration ;
    p[i].m_sOutLog.m_lAmount +=  pEvt->m_lSumAmount ; 
    addOutEvent(p+i,pEvt->m_iEventTypeID,iCnt,pEvt); 
    m_poDataLock->V(); 
   	return true;

}

void SHMStatLogMgr::addOutEvent(SHMStatLogData * pData,int iEvtTypeID,int iCnt,StdEvent *pEvt)
{
	 if(!pData)
	 	 return  ;
	 int iEvtType = 0;

    iEvtType = iEvtTypeID;
    LogInfo *pLog=NULL;
    switch (iEvtType) {
      case CYCLE_FEE_EVENT_TYPE: //周期性费用事件
      case CALL_EVENT_TYPE:      //语音事件
      case DATA_EVENT_TYPE:      //数据事件
      case VALUE_ADDED_EVENT_TYPE: //增值事件
      case 0: //预处理正常事件
          pLog = pData->m_sALLOutLog + NORMAL ;
          break;
      case DUP_EVENT_TYPE: //重复 	
          pLog = pData->m_sALLOutLog + NORMAL ;
          break;
      case NOOWNER_EVENT_TYPE: //无主    	
          pLog = pData->m_sALLOutLog + NORMAL ;
          break;
      case PP_PARAM_ERR_EVENT_TYPE: //预处理参数错误   	
          pLog = pData->m_sALLOutLog + NORMAL ;
          break;
      case RATE_PARAM_ERR_EVENT_TYPE: //批价参数错误 	
          pLog = pData->m_sALLOutLog + NORMAL ;
          break;
      case OTHER_EVENT_TYPE: //无效 	
          pLog = pData->m_sALLOutLog + NORMAL ;
          break;
      case ERR_EVENT_TYPE:   //格式错误(预处理) 	
          pLog = pData->m_sALLOutLog + NORMAL ;
          break;

      default:
      	pLog = pData->m_sALLOutLog + NORMAL ;
    }  
    if(pLog)
      addLogInfo( pLog,iCnt,pEvt) ;   
}


//
int SHMStatLogMgr::getALLInfo(int iAppID,char *sStartTime, char *sEndTime,vector<SHMStatLogData> &vRetvec)
{
	  unsigned int i ;
	  int iCount=0;
	  SHMStatLogData * p = m_poLogData;
	  int num = m_poSHMStatLogData->getCount();
	  vRetvec.clear();
	  if(!sStartTime) 
	  	 setStartTime(sStartTime);
	  if(!sEndTime)
	  	 setEndTime(sEndTime);
	  for(int j=1 ;j<=num;j++)
	  {
	  	if(p[j].m_iAppID == iAppID &&
	  		 strcmp(p[j].m_sStartTime,sStartTime)>=0 &&
	  		 strcmp(p[j].m_sEndTime,sEndTime)<=0 )
	  	 {
	  	 	 vRetvec.push_back(p[j]);
	  	 	 iCount++;
	  	 }
	  }

	 return  iCount ;      
}


int SHMStatLogMgr::getSumInfo(int iAppID,int iBussiID,char *sStartTime, char *sEndTime,SHMStatLogData *pData)
{
    memset(pData,0,sizeof(SHMStatLogData ));
    unsigned int i ;
	  int iCount=0;
	  SHMStatLogData * p = m_poLogData;
	  int num = m_poSHMStatLogData->getCount();
	  if(!sStartTime) 
	  	 setStartTime(sStartTime);
	  if(!sEndTime)
	  	 setEndTime(sEndTime);
	  for(int j=1 ;j<=num;j++)
	  {
	  	if(p[j].m_iAppID == iAppID && 
	  		 p[j].m_iModuleID == iBussiID &&
	  		 strcmp(p[j].m_sStartTime,sStartTime)>=0 &&
	  		 strcmp(p[j].m_sEndTime,sEndTime)<=0 )
	  	 {
	  	 	 sumDataInfo(pData,p+j);	  	 	 
	  	 	 iCount++;
	  	 }
	  }

	 return  iCount ;  
    
}

//提交信息点。落地（入库+写文件）
void SHMStatLogMgr::writeFile(SHMStatLogData *pData)
{

   return  ;            
}

void SHMStatLogMgr::commitDB(SHMStatLogData *pData)
{
  //输入信息
   Date dt ;
   memset(pData->m_sEndTime,0,sizeof(pData->m_sEndTime));
   strncpy(pData->m_sEndTime,dt.toString(),14);
     
	if(pData->m_sInLog.m_iNum>0)
		insertLog(pData,&(pData->m_sInLog),1);
	
	for(int i=0;i<CDR_TYPE_NUMS;i++)
	 {
	 if(pData->m_sALLInLog[i].m_iNum>0)
		 insertLog(pData,&(pData->m_sALLInLog[i]),1);
	}
	
	//输出信息
	if(pData->m_sOutLog.m_iNum>0)
		insertLog(pData,&(pData->m_sOutLog),2);
	
	for(int i=0;i<CDR_TYPE_NUMS;i++)
	 {
	 if(pData->m_sALLOutLog[i].m_iNum>0)
		 insertLog(pData,&(pData->m_sALLOutLog[i]),2);
	}	

}

void SHMStatLogMgr::insertLog(SHMStatLogData *pData,LogInfo *pLog,int iLogType)
{

    /*
    qry.close();
    qry.setSQL("select seq_stat_log_id.nextval log_id from dual");
    qry.open();
    qry.next();
    long lLogID = qry.field("log_id").asInteger();
    
    long lLogID=m_poSeqExt->getNextVal ();
    DEFINE_QUERY(qry);
    qry.setSQL ("insert into WF_INFO_LOG( "
                "  STAT_LOG_ID,  "
                "  FILE_ID,      "
                "  FILE_NAME,    "
                "  APP_ID,       "
                "  DR_ID,        "
                "  SOURCE_ID,    "
                "  MODULE_ID,    "
                "  TICKET_FLAG,  "
                "  LOG_TYPE,     "
                "  CDR_TYPE ,    "
                "  CDR_NUM,      "
                "  CHARGE,       "
                "  DULATION,     "
                "  AMOUNT, "
                "  STATE_DATE,     "
                "  END_DATE        "
        "  ) values "
        "(:STAT_LOG_ID,:FILE_ID,:FILE_NAME, :APP_ID,:DR_ID,:SOURCE_ID,:MODULE_ID, "
        " :TICKET_FLAG,:LOG_TYPE,:CDR_TYPE,:CDR_NUM, :CHARGE,:DULATION,:AMOUNT, "
         " to_date(:STATE_DATE,'yyyymmddhh24miss'),to_date(:END_DATE,'yyyymmddhh24miss') )"
    );
    
    qry.setParameter ("STAT_LOG_ID", lLogID);
    qry.setParameter ("FILE_ID", pData->m_iFileID);
    qry.setParameter ("FILE_NAME", pData->m_sFileName);    
    qry.setParameter ("APP_ID", pData->m_iAppID);
    qry.setParameter ("DR_ID",  pData->m_iDrID);
    qry.setParameter ("SOURCE_ID",  pData->m_iSourceID);
    qry.setParameter ("MODULE_ID",  pData->m_iModuleID);
    qry.setParameter ("TICKET_FLAG",  pData->m_iTicketFlag); 
    qry.setParameter ("LOG_TYPE", iLogType);  
    qry.setParameter ("CDR_TYPE", pLog->m_iType);     
    qry.setParameter ("CDR_NUM", pLog->m_iNum);     
    qry.setParameter ("CHARGE", pLog->m_lCharge);     
    qry.setParameter ("DULATION", pLog->m_lDulation);                    
    qry.setParameter ("AMOUNT", pLog->m_lAmount);      
    qry.setParameter ("STATE_DATE", pData->m_sStartTime); 
    qry.setParameter ("END_DATE", pData->m_sEndTime);     	  
                    
    qry.execute();
    qry.commit();
    qry.close();
    */
}


void SHMStatLogMgr::getStringIndex(int iFileID,int iAppID)
{
	m_lLongIndex=0;
	m_lLongIndex=iFileID*100000000+iAppID%100000000 ;
}

int SHMStatLogMgr::getBusiclassBySwitch(int iSwitchID)
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

int SHMStatLogMgr::getAppIDByProcess(int iProcessID)
{
	static HashList<int> *s_pAppMap = 0x0;
	int iProcess ;
	int iAppID ;
	if(!s_pAppMap)
		{
			 s_pAppMap = new HashList<int> (5000);
      TOCIQuery qry (Environment::getDBConn());
      char sSqlcode[1024]={0};
	    sprintf (sSqlcode, "select a.process_id,a.app_id from wf_process a" );
	    qry.setSQL(sSqlcode);
	    qry.open();
	    if (qry.next()) {
		     iProcess = qry.field(0).asInteger();
		     iAppID = qry.field(1).asInteger();
		      s_pAppMap->add (iProcessID, iAppID);
		    }
      qry.close();			    
		 }
  int i=0;
   if (s_pAppMap->get (iProcessID, &i))
   {
     return i;
   } 
   else
   {
     return iProcessID;//默认批价
   } 			

}
	
void SHMStatLogCtl::display()
{   
	  SHMStatLogData * p = m_poLogData;
    int iused2 = m_poFileAppIndex->getCount();
    int iall2 = m_poFileAppIndex->getTotal();
    cout<<"已用索引内存条:"<<iused2<<endl;
    cout<<"共有索引内存条:"<<iall2<<endl; 
    	  
    int iused = m_poSHMStatLogData->getCount();
    int iall = m_poSHMStatLogData->getTotal();
    cout<<"已用数据内存块:"<<iused<<endl;
    cout<<"共有数据内存块:"<<iall<<endl;  
    
    SHMStatLogData tmpdate;
    unsigned int iRet;
    SHMData_A<SHMStatLogData>::Iteration iter = m_poSHMStatLogData->getIteration();
    while(iter.next(tmpdate,iRet) )
    {
      printf("%d,%s,%d,%d,%d,%d,%d,%d,%s,%s\n",	
      tmpdate.m_iFileID,
      tmpdate.m_sFileName,
      tmpdate.m_iAppID ,
      tmpdate.m_iDrID,
      tmpdate.m_iSourceID ,
      tmpdate.m_iModuleID , 
      tmpdate.m_sInLog.m_iNum ,
      tmpdate.m_sOutLog.m_iNum ,
      tmpdate.m_sStartTime,
      tmpdate.m_sEndTime);   	
    }
    /*
     int num=100;
    if(num>iused)
    	 num=iused;
    for(int i=1;i<=num;i++)
    {
    printf("%d,%s,%d,%d,%d,%d,%d,%d,%s,%s\n",	
    p[i].m_iFileID,
    p[i].m_sFileName,
    p[i].m_iAppID ,
    p[i].m_iDrID,
    p[i].m_iSourceID ,
    p[i].m_iModuleID , 
    p[i].m_sInLog.m_iNum ,
    p[i].m_sOutLog.m_iNum ,
    p[i].m_sStartTime,
    p[i].m_sEndTime);
    }
    */
}
	
int SHMStatLogMgr::revokeAll()
{   
	  SHMStatLogData * p = m_poLogData;
	  unsigned int j ,uiRet;
    int iused = m_poSHMStatLogData->getCount();
    int iall = m_poSHMStatLogData->getTotal();
    cout<<"已用数据内存块:"<<iused<<endl;
    cout<<"共有数据内存块:"<<iall<<endl;  
 
    int iused2 = m_poFileAppIndex->getCount();
    int iall2 = m_poFileAppIndex->getTotal();
    cout<<"已用索引内存条:"<<iused2<<endl;
    cout<<"共有索引内存条:"<<iall2<<endl; 
    
    cout<<"开始回收数据&索引"<<endl;
    SHMStatLogData tmpdate;
    unsigned int iRet;
    SHMData_A<SHMStatLogData>::Iteration iter = m_poSHMStatLogData->getIteration();
    while(iter.next(tmpdate,iRet) )
    {
    	  getStringIndex(tmpdate.m_iFileID,tmpdate.m_iAppID);
        m_poDataLock->P();
        m_poSHMStatLogData->revoke(iRet);
        m_poDataLock->V();   
        	
        m_poIndexLock->P();
        if (m_poFileAppIndex->get(m_lLongIndex, &j))
        {
          if(!m_poFileAppIndex->revokeIdx(m_lLongIndex,uiRet))
          	 cout<<"删除索引失败"<<m_lLongIndex<<"--"<<j<<"--"<<uiRet<<endl;
          }	 
        m_poIndexLock->V();         	
    }        
    
    /*	
    for(int i=1;i<=iused;i++)
    {
    	  getStringIndex(p[i].m_iFileID,p[i].m_iAppID);
        m_poDataLock->P();
        m_poSHMStatLogData->revoke(i);
        m_poDataLock->V();   

        m_poIndexLock->P();
        if (m_poFileAppIndex->get(m_lLongIndex, &j))
        {
          if(!m_poFileAppIndex->revokeIdx(m_lLongIndex,uiRet))
          	 cout<<"删除索引失败"<<m_lLongIndex<<"--"<<j<<"--"<<uiRet<<endl;
          }	 
        m_poIndexLock->V();         	
    }
    */
    cout<<"数据&索引回收完成"<<endl;
    iused = m_poSHMStatLogData->getCount();
    cout<<"已用数据内存块:"<<iused<<endl;
 
    iused2 = m_poFileAppIndex->getCount();
    cout<<"共有索引内存条:"<<iused2<<endl;    
     
    return 0;
}

void SHMStatLogMgr::addDisctInEvent(int iFileID,int iAppID,int iCnt,StdEvent *pEvt)
{
    unsigned int i ;
	  char sTmpTime[14+1]={0};
	  SHMStatLogData * p = m_poLogData;
	  getStringIndex(iFileID,iAppID) ;
	 if (!m_poFileAppIndex->get (m_lLongIndex, &i)) {
	 	    m_poDataLock->P();
        i = m_poSHMStatLogData->malloc ();  
        if(!i) THROW(MBC_ParamShmMgr+4);
        m_poIndexLock->P();
        m_poFileAppIndex->add(m_lLongIndex,i);
        m_poIndexLock->V();
        initRecord(p+i,iFileID,iAppID,pEvt->m_oEventExt.m_sFileName) ;
        p[i].m_iModuleID = getBusiclassBySwitch(pEvt->m_iSwitchID);
        p[i].m_sInLog.m_iNum += iCnt;  
        m_poDataLock->V();             
        return  ;
    }
 
   m_poDataLock->P();
   p[i].m_sInLog.m_iNum += iCnt;
   m_poDataLock->V();       
   return ;  
}	


void SHMStatLogMgr::commitDisctLog(int iFileID,int iAppID)
{
	  unsigned int i;
    SHMStatLogData * p = m_poLogData;
	  getStringIndex(iFileID,iAppID) ;
	  if (!m_poFileAppIndex->get (m_lLongIndex, &i)) {	
	  	return ;
	  }
    int DISCT_MOULDE_CAPABILITY_KPI = 3003;
    char* DISCT_MOULDE_CAPABILITY_KPI_NAME = "帐期末帐务优惠每秒处理用户数";
	  Date dt_e ;
	  strncpy(p[i].m_sEndTime,dt_e.toString(),14);
	  Date dt_s(p[i].m_sStartTime);
	  long lDiff=dt_e.diffSec(dt_s);
	  if(lDiff<=0) lDiff=1;
	  long lCapability = p[i].m_sInLog.m_iNum/lDiff ;
	  insertMouldeCapabilityKpiLog(DISCT_MOULDE_CAPABILITY_KPI, lCapability, DISCT_MOULDE_CAPABILITY_KPI_NAME);
	  
}

void SHMStatLogMgr::addRentInEvent(int iFileID,int iAppID,int iCnt,StdEvent *pEvt)
{
    unsigned int i ;
	  char sTmpTime[14+1]={0};
	  SHMStatLogData * p = m_poLogData;
	  getStringIndex(iFileID,iAppID) ;
	  if (!m_poFileAppIndex->get (m_lLongIndex, &i)) {
	 	    m_poDataLock->P();
        i = m_poSHMStatLogData->malloc ();
        if(!i) THROW(MBC_ParamShmMgr+4);
        m_poIndexLock->P();
        m_poFileAppIndex->add(m_lLongIndex,i);
        m_poIndexLock->V();
        initRecord(p+i,iFileID,iAppID,pEvt->m_oEventExt.m_sFileName) ;
        p[i].m_iModuleID = getBusiclassBySwitch(pEvt->m_iSwitchID);
        p[i].m_sInLog.m_iNum += iCnt;  
        m_poDataLock->V();             
        return  ;
    }
 
   m_poDataLock->P();
   p[i].m_sInLog.m_iNum += iCnt;
   m_poDataLock->V();       
   return ;  
}	

void SHMStatLogMgr::commitRentLog(int iFileID,int iAppID)
{
	  unsigned int i;
    SHMStatLogData * p = m_poLogData;
	  getStringIndex(iFileID,iAppID) ;
	  if (!m_poFileAppIndex->get (m_lLongIndex, &i)) {	
	  	return ;
	  }
    int RENT_MOULDE_CAPABILITY_KPI = 3004;
    char* RENT_MOULDE_CAPABILITY_KPI_NAME = "租费计算每秒处理用户数";	  
	  Date dt_e ;
	  strncpy(p[i].m_sEndTime,dt_e.toString(),14);
	  Date dt_s(p[i].m_sStartTime);
	  long lDiff=dt_e.diffSec(dt_s);
	  if(lDiff<=0) lDiff=1;
	  long lCapability = p[i].m_sInLog.m_iNum/lDiff ;
	  insertMouldeCapabilityKpiLog(RENT_MOULDE_CAPABILITY_KPI, lCapability, RENT_MOULDE_CAPABILITY_KPI_NAME);
	  
}


void SHMStatLogMgr::insertMouldeCapabilityKpiLog(int iMouldeID, long lValue, const char* sKpiName)
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

void SHMStatLogMgr::setStartTime(char *sTime)
{
	   Date oDate ;	   
     oDate.setHour(0);
     oDate.setMin (0);
     oDate.setSec (0);
     strncpy(sTime,oDate.toString(),14);
}

void SHMStatLogMgr::setEndTime(char *sTime)
{
	   Date oDate ;	   
     oDate.setHour(23);
     oDate.setMin (59);
     oDate.setSec (59);
     strncpy(sTime,oDate.toString(),14);
}

void SHMStatLogMgr::sumDataInfo(SHMStatLogData * pDes,SHMStatLogData *pSor)
{
	sumLogInfo(&(pDes->m_sInLog),&(pSor->m_sInLog));
  sumLogInfo(&(pDes->m_sOutLog),&(pSor->m_sOutLog));
	for(int i=0;i<CDR_TYPE_NUMS;i++)
	{
	  sumLogInfo(pDes->m_sALLInLog+i,pSor->m_sALLInLog+i);
	  sumLogInfo(pDes->m_sALLOutLog+i,pSor->m_sALLOutLog+i);
	}	  
}

void SHMStatLogMgr::sumLogInfo(LogInfo * pDes,LogInfo *pSor)
{
	 pDes->m_iNum += pSor->m_iNum; //话单数
	 pDes->m_lCharge += pSor->m_lCharge;
	 pDes->m_lDulation += pSor->m_lDulation; //
	 pDes->m_lAmount += pSor->m_lAmount;
}


void SHMStatLogMgr::commitLog(int iFileID,int iAppID)
{
	
}

