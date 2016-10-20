/*VER: 2*/ 
#include "SetBillingCycle.h"
#include "Log.h"
#include "MonitorFlow.h"
#include "Date.h"


SetBillingCycle::SetBillingCycle()
{
}


int SetBillingCycle::UpdateOcsCloseCycle(int OrgId,int BillingCycleId)
{
    char sql[1024];
    memset(sql,0,sizeof(sql));
    
    DEFINE_QUERY (qry);
    sprintf(sql,"select state from ocs_hb_billing_cycle_close where billing_cycle_id=%d"
	            " and org_id=%d",BillingCycleId,OrgId);
    qry.setSQL(sql);
    qry.open();
    if(qry.next())
    {//已经更新过状态
        qry.close();
        return 0;
    }
    qry.close();

    memset(sql,0,sizeof(sql));
    sprintf(sql,"insert into ocs_hb_billing_cycle_close(ORG_ID,BILLING_CYCLE_ID,STATE,HB_CLOSE_TIME,OCS_CLOSE_TIME,TIME_DIFF) "
					 "values(%d,%d,'10A',sysdate,'','')",OrgId,BillingCycleId);
    qry.setSQL(sql);
    qry.execute();
    qry.commit();
    qry.close();
    return 0;
}
void SetBillingCycle::PrintBillingCycle(int iBillingCycleTypeID, char sTime[])
{
    BillingCycleMgr bcm;
    BillingCycle * pCycle;
    unsigned int iOffset,iTemp;
    int iCurBillingCycleID;

    pCycle = *m_poCycleData;

    if (!m_poTypeIndex->get (iBillingCycleTypeID, &iOffset)) {
		Log::log (0, "所指定的帐期不存在[1]");
		return;
	}

    iCurBillingCycleID = pCycle[iOffset].m_iBillingCycleID;

    while (pCycle[iOffset].m_iPre) {
        iOffset = pCycle[iOffset].m_iPre;
    }

    while (iOffset) {

            if (strcmp(sTime,"-1")==0
             || strncmp(sTime,pCycle[iOffset].m_sStartDate,strlen(sTime))==0 )
                printf("%d;%d;%s;%s;%s\n",pCycle[iOffset].m_iBillingCycleType,
                     pCycle[iOffset].m_iBillingCycleID,
                     pCycle[iOffset].m_sStartDate,
                     pCycle[iOffset].m_sEndDate,
                     pCycle[iOffset].m_sState);

        iOffset = pCycle[iOffset].m_iNext;
    }

    printf("当前帐期指向-->%d\n",iCurBillingCycleID);
    //Log::log (0, "所指定的帐期类型不存在");
    return;
}

void SetBillingCycle::setBillingCycle(int iBillingCycleID, char sState[],int iOrgId)
{
	BillingCycleMgr bcm;
	BillingCycle * pCycle = bcm.getBillingCycle (iBillingCycleID);
	if (!pCycle) {
		Log::log (0, "所指定的帐期不存在[0]");
		return;
	}

	int iCycleType = pCycle->getBillingCycleType ();
	unsigned int iOffset;
	char * sStartDate = pCycle->getStartDate ();
	int iTemp;
	bool bUpdateOcs=false;

	pCycle = *m_poCycleData;

	if (!m_poTypeIndex->get (iCycleType, &iOffset)) {
		Log::log (0, "所指定的帐期不存在[1]");
		return;
	}

	while (iOffset) {
		if (pCycle[iOffset].m_iBillingCycleID == iBillingCycleID) {
			DEFINE_QUERY (qry);
			char sSQL[2048];

/*2006.06.13 允许重复刷新 mdf by xudl
            if (strcmp(pCycle[iOffset].m_sState,sState)==0)
                return;
*/
#ifdef OCS_CYCLE_CLOSE_JS
            if(strcmp(pCycle[iOffset].m_sState,"10R")==0 && sState[2]!='R')
            {//关闭帐期，提醒输入参数orgid
                if(iOrgId==0)
                {
                    Log::log (0, "关闭帐期请带上参数orgid!且orgid要为200级的地市ID，以便维护OCS帐期关闭状态表 as setbillingcycle -u 10912 10E 100");
                    exit(0);    
                }
                else
                {//更新帐期状态表
                    bUpdateOcs = true;     
                }
            }
#endif
            
			sprintf (sSQL, "update billing_cycle set state='%s' where "
				"billing_cycle_id=%d",sState,iBillingCycleID);
			strcpy(pCycle[iOffset].m_sState,sState);

            /*2006.06.13 考虑四川情况，执行顺序往后调整*/
			//qry.setSQL (sSQL); qry.execute (); qry.close ();
            //printf("sSql:%s\n",sSQL);

            if (sState[2]=='R')
			    m_poTypeIndex->add (iCycleType, iOffset);
            
            qry.setSQL (sSQL); qry.execute (); qry.close ();
			DB_LINK->commit ();
			if(bUpdateOcs==true)
			{
			    UpdateOcsCloseCycle(iOrgId,iBillingCycleID);
			}
			
			//add by sunjy for 2.8test
			int iCycleID=iBillingCycleID;
			if (sState[2]=='B')
				updateCycleStat(iCycleID,1);
		  if (sState[2]=='E')
		  	updateCycleStat(iCycleID,2);
			
			return;
		}

		iTemp = strcmp (sStartDate, pCycle[iOffset].m_sStartDate);
		if (iTemp > 0) {
			iOffset = pCycle[iOffset].m_iNext;
		} else {
			iOffset = pCycle[iOffset].m_iPre;
		}
	}

	Log::log (0, "所指定的帐期不存在[2]");
	return;
}

/*
int SetBillingCycleMgr::run()
{
	int iBillingCycleID;

	if (g_argc < 2) {
		Log::log (0, "setbillingcycle BILLING_CYCLE_ID");
		return 0;
	}

	iBillingCycleID = atoi (g_argv[1]);

	SetBillingCycle * pSet = new SetBillingCycle ();

	pSet->setBillingCycle (iBillingCycleID);

	delete pSet; pSet = 0;

	return 0;
}


DEFINE_MAIN (SetBillingCycleMgr);
*/

int main(int argc ,char *argv[])
{

/*    //测试getOccurBillingCycle
    BillingCycleMgr bcm;
    BillingCycle * ptest;
    char sTime1[15];

    strcpy(sTime1,argv[1]);

    ptest = bcm.getBillingCycle(1, sTime1);

    printf("id:%d,begin:%s,end:%s,state:%s\n",ptest->m_iBillingCycleID,ptest->m_sStartDate,ptest->m_sEndDate,ptest->m_sState);
*/
/*
	BillingCycleMgr bcm;
	Date d;
	BillingCycle * p = bcm.getBillingCycle (1, d.toString ()); 
	int i,num =5,j;

	for (i=0; i<num; i++) 
	{
		if (!p) return 0;
		printf("billing_cycle_id=%d\n",p->m_iBillingCycleID);
		j = p->getBillingCycleID ();
		p = bcm.getPreBillingCycle (j);
	}

	if (!p) return 0; 
*/


    int  iBillingCycleID,iBillingCycleTypeID,iOrgID;
    char sState[3],sTime[10];

    if (argc < 3) {
        printf("    1:update billingcycle state in memory\n");
        printf("      ex:setbillingcycle -u BILLING_CYCLE_ID STATE ORGID\n");
        printf("         setbillingcycle -u 10702 10R 100(billing_cycle_id=10702,state=10R orgid=100)\n");
        printf("    2:print billingcycle info from memory\n");
        printf("      ex:setbillingcycle -p BILLING_CYCLE_TYPE [TIME_LIMIT]\n");
        printf("         setbillingcycle -p 1 2007 (billing_cycle_type=1,only print info in 2007. )\n");
        
        return 0;
    }

    SetBillingCycle * pSet = new SetBillingCycle ();

    if (strcasecmp (argv[1],"-u") == 0 )
    {
        iBillingCycleID = atoi (argv[2]);
        strcpy(sState,argv[3]);
        iOrgID = atoi(argv[4]);

        pSet->setBillingCycle (iBillingCycleID,sState,iOrgID);

        #ifdef MONITOR_FLOW_CONTROL
        if (sState[2]=='R' || sState[2]=='A' || sState[2]=='E')
        {
        	MonitorFlow mfw;
        	mfw.MonitorBillingCycle(iBillingCycleID,sState);
        }
        #endif	

    }

    strcpy(sTime,"-1");
    if (strcasecmp (argv[1],"-p") == 0)
    {
        iBillingCycleTypeID = atoi(argv[2]);
        if (argc==4)
            strcpy(sTime,argv[3]);

        printf("输出cycle_type=%d的帐期数据[时间限制:%s]\n",iBillingCycleTypeID,sTime);
        pSet->PrintBillingCycle (iBillingCycleTypeID,sTime);

    }

    delete pSet; pSet = 0;

    return 0;


}

//add by sunjy for 2.8test
void SetBillingCycle::updateCycleStat(int iBillingCycleID,int iFlag)
{
	
	  DEFINE_QUERY(qry);
	  char sSql[1024]={0};
	  int iCount=0;
	  char sStartTime[14+1]={0};
	  sprintf(sSql,"select count(*),to_char(START_TIME,'yyyymmddhh24miss')"
	               " from b_info_capability_cycle where cycle_id= %d group by START_TIME",iBillingCycleID);
	  qry.setSQL(sSql);
	  qry.open();
    if(qry.next())
    	{
        iCount=qry.field(0).asInteger() ;
        strncpy(sStartTime,qry.field(1).asString(),14);
      }
	  
	  qry.close();
	  memset(sSql,0,sizeof(sSql));
	  if(iFlag == 1)  //记录开始时间
	  {
	    if(iCount) 
	    {
	  	  sprintf(sSql,"update b_info_capability_cycle t set  "
	  	             "t.START_TIME=SYSDATE,is_over=0,used_time=0,deal_flag=1 "
	  	             "where t.cycle_id = %d",iBillingCycleID) ;
      }else{
      	sprintf(sSql,"insert into b_info_capability_cycle( "
      	             "log_id,cycle_id,start_time,end_time,  is_over ,used_time,deal_flag ) "
                     "values (seq_info_cycle_id.NEXTVAL , %d,SYSDATE,NULL,0,0,1) ",iBillingCycleID );
       }
	  	  qry.setSQL(sSql);
	  	  qry.execute();
        qry.commit();	  	  
	  	  qry.close(); 
	  	  	       
	  } 
	  if(iFlag == 2)  //记录开始时间
	  {
	    if(iCount) 
	    {
	    	Date dt;
	    	Date dt_s(sStartTime);
	    	long ldiff=dt.diffSec(dt_s);
	  	  sprintf(sSql,"update b_info_capability_cycle t set  "
	  	             "t.end_time=SYSDATE,is_over=1,used_time=%ld,deal_flag=0 "
	  	             "where t.cycle_id = %d",ldiff,iBillingCycleID);
	  	  qry.setSQL(sSql);
	  	  qry.execute();
        qry.commit();	  	  	  	  
	  	  qry.close(); 	  	            
      }else{
        cout<<"账期没经过10B的设置，就做了10E的设置"<<endl;
      } 
        
	  } 
	  
	 return ;  	       
}


