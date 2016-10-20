/*VER: 1*/ 
#define SQLCA_NONE
#define ORACA_NONE

#include "TStatTask.h"
#include "iostream.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "Environment.h"
#include "Process.h"
#include "StatCallEvent.h"
#include "StatServ.h"
#include "StatAcctItem.h"
#include "StatAcctBalance.h"
#include "StatBalanceSource.h"
#include "StatBalancePayout.h"
#include "StatPayment.h"
#include "StatBill.h"
#include "StatOtherEvent.h"
#include "StatDataEvent.h"
#include "StatCycleFeeEvent.h"
#include "StatValueAddedEvent.h"
#include "StatAcctItemAggr.h"
#include "StatErrorEvent.h"
#include "StatRedoEvent.h"
#include "StatARefundLog.h"

TOCIDatabase gpDBLink,gpDBLink_Temp;

class billstat :public Process
{
		public: 
				int run();
};

DEFINE_MAIN(billstat)

void InitAll()
{
}

int billstat:: run()
{
	int i=0;
	
	TStatClass * pStat;
		
	int iStatClassID=-1;
	int iRunMode=0;
	int iPageInOutMode=1;
	int cGet=0;
	int process_id,flow_id;
	
	while((cGet=getopt(g_argc,g_argv,"c:rp"))!=EOF)
	{
		switch(cGet)
		{
			case 'c':
                iStatClassID = atoi(optarg);
                break;
			case 'r':
								iRunMode=1;
								break;
			case 'p':
								iPageInOutMode=0;
								break;
			default:
                printf("billstat:参数错误\n");
                printf("Usg billstat -c ? [-r] [-p]\n");
				return -1;
		}
	}
	
	if (iStatClassID==-1){
		printf("billstat:参数错误\n");
        printf("Usg billstat -c ? [-r] [-p]\n");
		return -1;
	}
	
	gpDBLink.connect("stat","group_by","tibs_stat");
	gpDBLink_Temp.connect("stat","group_by","tibs_stat");
	
	set_new_handler(my_new_error);

	InitAll();

	switch ( iStatClassID){
        case OID_CALL_EVENT:
            pStat = new StatCallEvent();
            break;
        case OID_SERV:
        	pStat = new StatServ();
        	break;
        case OID_ACCT_ITEM:
        	pStat = new StatAcctItem();
        	break;
        case OID_ACCT_BALANCE:
        	pStat = new StatAcctBalance();
        	break;
        case OID_BALANCE_SOURCE:
        	pStat = new StatBalanceSource();
        	break;
        case OID_BALANCE_PAYOUT:
        	pStat = new StatBalancePayout();
        	break;
        case OID_PAYMENT:
        	pStat = new StatPayment();
        	break;
        case OID_BILL:
        	pStat = new StatBill();
        	break;
        case OID_OTHER_EVENT:
        	pStat = new StatOtherEvent();
        	break;
        case OID_DATA_EVENT:
        	pStat = new StatDataEvent();
        	break;
        case OID_CYCLE_FEE_EVENT:
        	pStat = new StatCycleFeeEvent();
        	break;
        case OID_VALUE_ADDED_EVENT:
        	pStat = new StatValueAddedEvent();
        	break;
        case OID_ACCT_ITEM_AGGR:
        	pStat = new StatAcctItemAggr();
        	break;
        case OID_ERROR:
        	pStat = new StatError();
        	break;
        case OID_REDO_EVENT:
        	pStat = new StatRedoEvent();
        	break;
        case OID_AREFUNDLOG:
        	pStat = new StatARefundLog();
        	break;
        default:
            printf("统计类:[%d]不存在!\n",iStatClassID);
            return -1;
  }
  
  if (iRunMode)
				pStat->SetRunMode(iRunMode);
	
	pStat->SetPageInOutMode(iPageInOutMode);
	
	try{
			for (;;){
					
					if (pStat->GetRecord()){
  		
						pStat->ProcessStat();
						i++;
						
						if (i%500==0)
							usleep(1000);
							
						if (i%10000==0){
							pStat->OutputResult();
							pStat->Commit();
						}
						if (i%50000==0){
							pStat->FreeResult();
							pStat->Commit();
						}
					}
					else 
						break;
			}
			pStat->FreeResult();
			pStat->Commit();
			delete pStat;
			printf("统计程序正常结束!\n");
			return 1;
	}
	catch (TOCIException &oe) {
		cout<<"Error occured ... in "<< __FILE__ <<endl;
		cout<<oe.GetErrMsg()<<endl;
		cout<<oe.getErrSrc()<<endl;
		throw oe;
	}
	catch (TException &e) {
		cout<<"Error occured ... in "<< __FILE__ <<endl;
		cout<<e.GetErrMsg()<<endl;
		throw e;
	}
	catch (...) {
		cout<<"Error occured ... in "<< __FILE__ <<endl;
		throw TException("Error occured ... ");
	}
}

/*
int main (int argc ,char *argv[])
{
	int i=0;
	
	TStatClass * pStat;
	
	Environment evt;
	
	try{
	
	evt.setDBLogin ("comm", "comm123", "lch_bill");
	
	int iStatClassID=-1;
	int cGet=0;
	int process_id,flow_id;

   
	while((cGet=getopt(argc,argv,"c:"))!=EOF)
	{
		switch(cGet)
		{
			case 'c':
                iStatClassID = atoi(optarg);
                break;
			default:
                printf("billstat:参数错误\n");
                printf("Usg billstat -c ? \n");
				exit(1);
		}
	}
	
	if (iStatClassID==-1){
		printf("billstat:参数错误\n");
        printf("Usg billstat -c ? \n");
		exit(1);
	}
    
  gpDBLink.connect("stat","stat","tibs_bill");
	gpDBLink_Temp.connect("stat","stat","tibs_bill");
	
	set_new_handler(my_new_error);

	InitAll();


    switch ( iStatClassID){
        case OID_CALL_EVENT:
            pStat = new StatCallEvent();
            break;
        case OID_SERV:
        	pStat = new StatServ();
        	break;
        case OID_ACCT_ITEM:
        	pStat = new StatAcctItem();
        	break;
        case OID_ACCT_BALANCE:
        	pStat = new StatAcctBalance();
        	break;
        case OID_BALANCE_SOURCE:
        	pStat = new StatBalanceSource();
        	break;
        case OID_BALANCE_PAYOUT:
        	pStat = new StatBalancePayout();
        	break;
        case OID_PAYMENT:
        	pStat = new StatPayment();
        	break;
        case OID_BILL:
        	pStat = new StatBill();
        	break;
        default:
            printf("统计类:[%d]不存在!\n",iStatClassID);
            exit(-1);
    }
	
	
	
	
		for (;;){
			
			if (pStat->GetRecord()){

				pStat->ProcessStat();
				i++;
				
				if (i%500==0)
					usleep(1000);
					
				if (i%5000==0){
					pStat->OutputResult();
					pStat->Commit();
				}
				if (i%30000==0){
					pStat->FreeResult();
					pStat->Commit();
				}
			}
			else 
				break;
		}
		
		pStat->FreeResult();
		pStat->Commit();
		delete pStat;
	}
	catch (TOCIException &oe) {
		cout<<"Error occured ... in "<< __FILE__ <<endl;
		cout<<oe.GetErrMsg()<<endl;
		cout<<oe.getErrSrc()<<endl;
		exit(1);
	}
	catch (TException &e) {
		cout<<"Error occured ... in "<< __FILE__ <<endl;
		cout<<e.GetErrMsg()<<endl;
		exit(1);
	}
	catch (...) {
		cout<<"Error occured ... in "<< __FILE__ <<endl;
		exit(1);
	}

    printf("统计程序正常结束!\n");

}
*/
