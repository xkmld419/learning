/*VER: 2*/ 
#include "DataMaker.h"


DataMaker::DataMaker()
{
    m_iEventType = -1;
    m_iBillingCycle = -1;
    Environment::useUserInfo ();
}
        
int DataMaker::run()
{
    if (!prepare ()) return 1; 
    
    ServIteration servIter;
    Serv theServ;

    char sDate[16];
    strcpy(sDate, m_oBeginDate.toString() );
    
    if (!G_PUSERINFO->getServIteration(servIter) ) {
        Log::log(0, "内存里面没有用户信息..");
        return 1;
    }        
    
    memset(m_poEvent, 0, sizeof(StdEvent) );

//    //时长
//    long  m_lDuration;
//    // 下行流量
//    long  m_lRecvAmount;
//    // 上行流量
//    long  m_lSendAmount;
//    //## 总流量
//    long  m_lSumAmount;    

    m_poEvent->m_lDuration    = 10 ;
    m_poEvent->m_lRecvAmount  = 100;
    m_poEvent->m_lSendAmount  = 50 ;
    m_poEvent->m_lSumAmount   = 150;
    m_poEvent->m_iEventTypeID = m_iEventType; 
    m_poEvent->m_iBillingCycleID = m_iBillingCycle; 
    m_poEvent->m_oEventExt.m_iSourceEventType = 3;
    m_poEvent->m_iEventSourceType = 1;
    strcpy (m_poEvent->m_sEventState, "NOM");
    m_poEvent->m_iOffers[4] = Process::m_iProcID;
    
    char * startDate = m_poEvent->m_sStartDate;
   
    long lCnt=0;
    while(servIter.next(theServ)){
        #ifdef SHM_FILE_USERINFO
        G_SHMFILE;
        #endif
        
        m_poEvent->m_iServID = theServ.getServID();
        if((theServ.getRentDate())&&strcmp(theServ.getRentDate(), sDate) > 0 ){
            strcpy(m_poEvent->m_sStartDate,theServ.getRentDate());
        }else{
            strcpy(m_poEvent->m_sStartDate,sDate);
        }
        m_poEvent->m_iBillingOrgID = theServ.getOrgID(startDate);
        if(m_poEvent->m_iBillingOrgID < 0){
            m_poEvent->m_iBillingOrgID = 1;
        }
        
        m_poEvent->m_iProdID = theServ.getProductID(startDate);;
        m_poEvent->m_iCustID = theServ.getCustID ();
        strcpy (m_poEvent->m_sBillingNBR, theServ.getAccNBR (25, startDate));
        m_poEvent->m_iBillingTypeID = atoi(theServ.getAttr (ID_BILLING_MODE));
        m_poEvent->m_iOffers[0] = theServ.getBaseOfferID ();

        m_poEvent->m_lEventID = EventIDMaker::getNextEventID ();
        
        m_poSender->send (m_poEvent);
        stat(m_poEvent);
        lCnt++;
        
    }
    SEND_FORCE_COMMIT_EVENT ();
    Log::log(0, "生成完毕,共生成 %ld 条.", lCnt);
}
     
void DataMaker::printUsage()
{
    Log::log(0,"\n\n\t*********使用方法**********");
    Log::log(0,"\t datamaker v1.1.0");
    Log::log(0,"\t datamaker -t event_type -b BillingCycleID -s startdate") ;   
    Log::log(0,"\t***************************\n\n");
}
 
bool DataMaker::prepare()
{
    int iTemp, jTemp;
    jTemp = g_argc;
    
    if(jTemp<2){
        printUsage ();
        return false;
    }

    for (iTemp=1; iTemp<jTemp; iTemp += 2) {
        if (g_argv[iTemp][0] != '-') {
            printUsage ();
            return false;
        }

        switch (g_argv[iTemp][1] | 0x20) {

          case 't':
            m_iEventType = strtol (g_argv[iTemp+1], 0, 10);
            break;
           
          case 'b':
            m_iBillingCycle = strtol (g_argv[iTemp+1], 0, 10);
            break;

          case 's':
            m_oBeginDate.parse (g_argv[iTemp+1]);
            break;
            
          default:
            printUsage ();
            return false;
            break;
        }
        
    }

    if(m_iEventType<0){
        Log::log(0,"event_type_id 不合法");
        return false;
    }
    if(m_iBillingCycle<0){
        Log::log(0,"m_iBillingCycle 不合法");
        return false;
    }

return true;

}
 
 
 
void DataMaker::stat(StdEvent *pEvent)
{
    AddTicketStat (TICKET_NORMAL);
    TStatLogMgr::addOutEvent(pEvent->m_iFileID, pEvent->m_iEventTypeID);
}
 
DEFINE_MAIN (DataMaker);

