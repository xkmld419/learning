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

#include "StatCallEvent.h"
#include "StatServ.h"
#include "StatAcctItem.h"
#include "StatAcctBalance.h"
#include "StatBalanceSource.h"
#include "StatBalancePayout.h"
#include "StatPayment.h"
#include "StatBill.h"

#define _FILE_NAME_ "billstat.CPP"
TOCIDatabase gpDBLink,gpDBLink_Temp;


void InitAll()
{
}

int main (int argc ,char *argv[])
{
	int i=0;
	
	TStatClass * pStat;
	
	Environment evt;
	
	try{
	
	evt.setDBLogin ("comm", "comm_sys", "lch_bill");
	
	int iStatClassID=-1;
	int cGet=0;
	int process_id,flow_id;

    /*参数处理*/
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
    
    /*ConnectOciDB(gpDBLink);
	ConnectOciDB(gpDBLink_Temp);*/
	gpDBLink.connect("stat","stat","lch_bill");
	gpDBLink_Temp.connect("stat","stat","lch_bill");
	
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
		cout<<"Error occured ... in "<< _FILE_NAME_ <<endl;
		cout<<oe.GetErrMsg()<<endl;
		cout<<oe.getErrSrc()<<endl;
		exit(1);
	}
	catch (TException &e) {
		cout<<"Error occured ... in "<< _FILE_NAME_ <<endl;
		cout<<e.GetErrMsg()<<endl;
		exit(1);
	}
	catch (...) {
		cout<<"Error occured ... in "<< _FILE_NAME_ <<endl;
		exit(1);
	}

    printf("统计程序正常结束!\n");

}

