/*VER: 1*/ 
#include "TStatTask.h"
#include "iostream.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <ticket.h>
#include <string.h>
#include "billcommon.h"
#include <unistd.h>

#define TestOBJID 11
#define _FILE_NAME_ "TESTOBJ.CPP"
#define FA_T_STAT_DAY	 1
#define	FA_T_STAT_HOUR	 2
#define FA_T_SWITCH_ID   3
#define FA_T_TICKET_TYPE 4
#define FA_T_PAGE        5
#define FA_T_DURATION	 6
#define FA_T_CHARGE      7
#define  FA_T_BUREAU_ID	 8
TOCIDatabase gpDBLink,gpDBLink_Temp;

TCallTicket	tCallTicket;

class TestObj : public TStatClass
{
public:
	TestObj();
	~TestObj();
	POINT_TYPE GetMaxPoint(char * sTableName,POINT_TYPE Point);	
	POINT_TYPE GetMinPoint(char * sTableName,POINT_TYPE Point);	
	void OpenCursor(char * sTableName,POINT_TYPE MinPoint,POINT_TYPE MaxPoint);
	bool GetRecordFromTable(POINT_TYPE &BreakPoint);
    void CloseCursor();
    int  GetFieldValue(int iFieldID,TFieldValue * pTFieldValue);
	int  GetMonth();
	bool GetRecord();
};

TestObj::TestObj():TStatClass(TestOBJID)
{
	cout<<"TestObj Init !"<<endl;
}

TestObj::~TestObj()
{
	cout<<"TestObj Exit!"<<endl;
}

void TestCreateCallTicket(void)
{
	time_t t_Now;
	char sNowTimeBuf[15];
	struct tm* ptm;
	int 	iHour=0;
	
	time(&t_Now);
	
	srandom((int)t_Now);

	iHour=random()%24;
	
	if (random()%2==1){
		if (random()%2==1)
			t_Now=t_Now-random()%31*24*3600-iHour*3600;
		else
			t_Now=t_Now-random()%31*24*3600+iHour*3600;
	}
	else{ 
		if (random()%2==1)
			t_Now=t_Now+random()%31*24*3600-iHour*3600;
		else
			t_Now=t_Now+random()%31*24*3600+iHour*3600;
	}
	ptm=localtime(&t_Now);
	strftime(sNowTimeBuf,sizeof(sNowTimeBuf),"%Y%m%d%H%M%S",ptm);
	
	strcpy(tCallTicket.sStartTime,sNowTimeBuf);
	
	tCallTicket.iTicketType=random()%31;
	
	tCallTicket.iTrunkID=random()%31;
	
	tCallTicket.iCharge=random()%1000;
	
	tCallTicket.iDuration=random()%500;
	
	tCallTicket.iExchID=random()%100;
	
	tCallTicket.iBureauID=random()%10;
	
}

POINT_TYPE TestObj::GetMaxPoint(char * sTableName,POINT_TYPE Point)
{
	return 1;
}
POINT_TYPE TestObj::GetMinPoint(char * sTableName,POINT_TYPE Point)
{
	return 1;
}
	
void TestObj::OpenCursor(char * sTableName,POINT_TYPE MinPoint,POINT_TYPE MaxPoint)
{
	return ;
}
bool TestObj::GetRecordFromTable(POINT_TYPE &BreakPoint)
{
	BreakPoint=1;
	TestCreateCallTicket();
	return true;
}

bool TestObj::GetRecord()
{
	TestCreateCallTicket();
	return true;
}

void TestObj::CloseCursor()
{
}


int  TestObj::GetFieldValue(int iFieldID,TFieldValue * pTFieldValue)
{
	char	sTemp[15];
	
	switch(iFieldID){
		case FA_T_STAT_DAY:
				strncpy(sTemp,tCallTicket.sStartTime,8);
				sTemp[8]='\0';
				if (pTFieldValue->ValType==VT_INT){
					pTFieldValue->SetValue(atol(sTemp));
				}
				else if ( pTFieldValue->ValType==VT_STRING){
					pTFieldValue->SetValue(sTemp);
				}
				break;
		case FA_T_STAT_HOUR:
				strncpy(sTemp,tCallTicket.sStartTime+8,2);
				sTemp[2]='\0';
				if (pTFieldValue->ValType==VT_INT){
					pTFieldValue->SetValue(atol(sTemp));
				}
				else if ( pTFieldValue->ValType==VT_STRING){
					pTFieldValue->SetValue(sTemp);
				}
				break;
		case FA_T_SWITCH_ID:
				if (pTFieldValue->ValType==VT_INT){
					pTFieldValue->SetValue(tCallTicket.iExchID);
				}
				else if ( pTFieldValue->ValType==VT_STRING){
					sprintf(sTemp,"%d",tCallTicket.iExchID);
					pTFieldValue->SetValue(sTemp);
				}
				break;
		case FA_T_TICKET_TYPE:
				if (pTFieldValue->ValType==VT_INT){
					pTFieldValue->SetValue(tCallTicket.iTicketType);
				}
				else if ( pTFieldValue->ValType==VT_STRING){
					sprintf(sTemp,"%d",tCallTicket.iTicketType);
					pTFieldValue->SetValue(sTemp);
				}
				break;
		case FA_T_PAGE:
				if (pTFieldValue->ValType==VT_INT){
					pTFieldValue->SetValue(1);
				}
				else if ( pTFieldValue->ValType==VT_STRING){
					sprintf(sTemp,"1");
					pTFieldValue->SetValue(sTemp);
				}
				break;
		case FA_T_DURATION:
				if (pTFieldValue->ValType==VT_INT){
					pTFieldValue->SetValue(tCallTicket.iDuration);
				}
				else if ( pTFieldValue->ValType==VT_STRING){
					sprintf(sTemp,"%d",tCallTicket.iDuration);
					pTFieldValue->SetValue(sTemp);
				}
				break;
		case FA_T_CHARGE:
				if (pTFieldValue->ValType==VT_INT){
					pTFieldValue->SetValue(tCallTicket.iCharge);
				}
				else if ( pTFieldValue->ValType==VT_STRING){
					sprintf(sTemp,"%d",tCallTicket.iCharge);
					pTFieldValue->SetValue(sTemp);
				}
				break;
		case FA_T_BUREAU_ID:
				if (pTFieldValue->ValType==VT_INT){
					pTFieldValue->SetValue(tCallTicket.iBureauID);
				}
				else if ( pTFieldValue->ValType==VT_STRING){
					sprintf(sTemp,"%d",tCallTicket.iBureauID);
					pTFieldValue->SetValue(sTemp);
				}
				break;
		default:
				if (pTFieldValue->ValType==VT_INT){
					pTFieldValue->SetValue(-1);
				}
				else if ( pTFieldValue->ValType==VT_STRING){
					sprintf(sTemp,"-1");
					pTFieldValue->SetValue(sTemp);
				}
				break;
	}
	return 0;
}

int  TestObj::GetMonth()
{
	int 	iResult;
	char 	sTemp[7];
	
	strncpy(sTemp,tCallTicket.sStartTime,6);
	sTemp[6]='\0';
	iResult=atoi(sTemp);
	return iResult;
}

int main (int argc ,char *argv[])
{
	ConnectOciDB(gpDBLink);
	ConnectOciDB(gpDBLink_Temp);
	
	try{
	int i=0;
	TestObj Test;
	
	set_new_handler(my_new_error);
	
	
		for (;;){
			
			if (Test.GetRecord()){
				Test.ProcessStat();
				i++;
				
				if (i%500==0)
					usleep(1000);
					
				if (i%5000==0){
					Test.OutputResult();
					gpDBLink.Commit();
				}
				if (i%30000==0){
					Test.FreeResult();
					gpDBLink.Commit();
				}
			}
			else 
				break;
		}
		
		Test.FreeResult();
		gpDBLink.Commit();
	}
	catch (TOCIException &oe) {
		cout<<"Error occured ... in "<< _FILE_NAME_ <<endl;
		cout<<oe.GetErrMsg()<<endl;
		cout<<oe.GetErrSrc()<<endl;
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

    printf("统计处理结束.\n");	
}

