#include "petri.h"
#include "TOCIQuery.h"
#include "Exception.h"
#include "tblflds.h"

extern "C" {

int tpsvrinit(int argc, char **argv)
{
	userlog("%s Raise\n",argv[0]);
  return 0;
}

//######################################################################

void PETRI_FIRE(TPSVCINFO *rqst)
{
	long ltransID=0;
	int iRet=0;
	char swsnaddr[30];
	FBFR32 *sendbuf;
	FBFR32 *recvbuf;
	
	char *sendstring;
	char *sendbuf2=NULL;

	int iLen = rqst->len;
	long  i;
	sendstring = new char[iLen];
	memset(sendstring,'\0',iLen);
	sendbuf2 = (char *) tpalloc("STRING", NULL, MAX_CMAMSG_LEN);
	
	long lRecLen = 0;
	char cRouteKey[30];
	char tmpBuff[30];
	memset(swsnaddr,0,sizeof(swsnaddr));

	userlog("1");
	/*
	if ((i=tpinit(NULL))<0)
 	{       
		sprintf(sendbuf2,"<iResult=1><sMsg=tpinit err>");
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	*/
	userlog("2");

	if ((sendbuf = (FBFR32 *)tpalloc("FML32",NULL,MAX_CMAMSG_LEN)) == NULL)
	{
		sprintf(sendbuf2,"<iResult=1><sMsg=sendbuf err>");
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	
	if ((recvbuf = (FBFR32 *)tpalloc("FML32",NULL,MAX_CMAMSG_LEN)) == NULL)
	{
		sprintf(sendbuf2,"<iResult=1><sMsg=recvbuf err>");
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	userlog("3");
	memcpy(sendstring, rqst->data, iLen);
	sendstring[iLen] = 0;
	TrimHeadNull(sendstring,iLen);
	::PETRI::P_RECVBUFF.reset();
	::PETRI::P_RECVBUFF.parseBuff(sendstring);
	ltransID = ::PETRI::P_RECVBUFF.getTransitionID();
	userlog("2,ltransID=[%ld]",ltransID);

	
	TOCIQuery query (::PETRI::P_DB.getDB());
	try {
		string sql = "select b.wsnaddr from P_TRANSITION_HOST_INFO a, WF_HOST_WSNADDR b where a.wsnaddr_id = b.wsnaddr_id and a.transition_id = :TRANSID";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("TRANSID",ltransID);
		query.open();
		while(query.next()) {
			strcpy( swsnaddr,query.field(0).asString() );
		}
		query.close();
		
		string sql2 = "select HOST_AREA||HOST_DOMAIN from P_TRANSITION_HOST_INFO where transition_id = :TRANSID";
		query.close();
		query.setSQL(sql2.c_str());
		query.setParameter("TRANSID",ltransID);
		query.open();
		while(query.next()) {
			strcpy( tmpBuff,query.field(0).asString());
		}
		query.close();
		query.commit();
	}catch (TOCIException & toe){
		sprintf(sendbuf2,"<iResult=1><sMsg=get WSNADDR err:%s>",toe.getErrMsg());
		query.close();
		query.commit();
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	} catch(...) {
		sprintf(sendbuf2,"<iResult=1><sMsg=get WSNADDR other err>");
		query.close();
		query.commit();
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	if(0 == swsnaddr[0]){
		sprintf(sendbuf2,"<iResult=1><sMsg=Transition have no WSNADDR >");
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	userlog("3");
	/*
	iRet = setenv("WSNADDR",swsnaddr,1);
	if( -1 == iRet){
		sprintf(rcvbuf,"<iResult=1><sMsg=set WSNADDR fail>");
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,rcvbuf,strlen(rcvbuf),0L);
	};
	*/
	strcpy(cRouteKey, tmpBuff);		
	iRet = Fchg32(sendbuf,ROUTE_KEY,0,(char *)cRouteKey,0);
	if(iRet < 0)
	{
		userlog("cRouteKey=[%s],iRet=[%d]",cRouteKey,iRet);
		sprintf(sendbuf2,"<iResult=1><sMsg=cslfput to ROUTE_KEY is error>");
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	
	iRet = Fchg32(sendbuf,SEND_STRING,0,(char *)sendstring,0);
	if(iRet < 0)
	{
		userlog("sendstring=[%s],iRet=[%d]",sendstring,iRet);

		sprintf(sendbuf2,"<iResult=1><sMsg=cslfput to SEND_STRING is error>");
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	
	iRet = tpcall("P_F_LOCAL",(char *)sendbuf,0,(char **)&recvbuf,&lRecLen,0);
  if (iRet < 0){
		userlog("4");
		tpfree((char *)sendbuf);
		sprintf(sendbuf2,"<iResult=1><sMsg=call localsvc err,tperrno:%d>",tperrno);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	userlog("5");
	
	tpfree((char *)sendbuf);
	tpfree((char *)recvbuf);
	sprintf(sendbuf2,"<iResult=0><sMsg=you tpcall  success>");
	tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);	
}

void P_CHANGE_STATUS(TPSVCINFO *rqst)
{
	long ltransID=0;
	int iRet=0;
	char swsnaddr[30];
	FBFR32 *sendbuf;
	FBFR32 *recvbuf;
	
	char *sendstring;
	char *sendbuf2=NULL;
	int iLen = rqst->len;
	long  i;
	sendstring = new char[iLen];
	memset(sendstring,'\0',iLen);
	sendbuf2 = (char *) tpalloc("STRING", NULL, MAX_CMAMSG_LEN);
	
	//char *rcvbuf = NULL;
	//char *sendbuf = NULL;
	long lRecLen = 0;
	char cRouteKey[30];
	char tmpBuff[30];
	memset(swsnaddr,0,sizeof(swsnaddr));
	//rcvbuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	//sendbuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	if ((sendbuf = (FBFR32 *)tpalloc("FML32",NULL,MAX_CMAMSG_LEN)) == NULL)
	{
		sprintf(sendbuf2,"<iResult=1><sMsg=sendbuf err>");
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	
	if ((recvbuf = (FBFR32 *)tpalloc("FML32",NULL,MAX_CMAMSG_LEN)) == NULL)
	{
		sprintf(sendbuf2,"<iResult=1><sMsg=recvbuf err>");
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	
	//int iLen = rqst->len;
	memcpy(sendstring, rqst->data, iLen);
	sendstring[iLen] = 0;
	TrimHeadNull(sendstring,iLen);
	::PETRI::P_RECVBUFF.reset();
	::PETRI::P_RECVBUFF.parseBuff(sendstring);
	ltransID = ::PETRI::P_RECVBUFF.getTransitionID();
	
	TOCIQuery query (::PETRI::P_DB.getDB());
	try {
		string sql = "select b.wsnaddr from P_TRANSITION_HOST_INFO a, WF_HOST_WSNADDR b where a.wsnaddr_id = b.wsnaddr_id and a.transition_id = :TRANSID";					
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("TRANSID",ltransID);
		query.open();
		while(query.next()) {
			strcpy( swsnaddr,query.field(0).asString() );
		}
		query.close();

		string sql2 = "select HOST_AREA||HOST_DOMAIN from P_TRANSITION_HOST_INFO where transition_id = :TRANSID";
		query.close();
		query.setSQL(sql2.c_str());
		query.setParameter("TRANSID",ltransID);
		query.open();
		while(query.next()) {
			strcpy( tmpBuff,query.field(0).asString());
		}
		query.close();
		query.commit();
		
	}catch (TOCIException & toe){
		sprintf(sendbuf2,"<iResult=1><sMsg=get WSNADDR err:%s>",toe.getErrMsg());
		query.close();
		query.commit();
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	} catch(...) {
		sprintf(sendbuf2,"<iResult=1><sMsg=get WSNADDR other err>");
		query.close();
		query.commit();
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	if(0 == swsnaddr[0]){
		sprintf(sendbuf2,"<iResult=1><sMsg=Transition have no WSNADDR >");
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
/*
	iRet = setenv("WSNADDR",swsnaddr,1);
	if( -1 == iRet){
		sprintf(rcvbuf,"<iResult=1><sMsg=set WSNADDR fail>");
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,rcvbuf,strlen(rcvbuf),0L);
	};
	*/
	strcpy(cRouteKey, tmpBuff);		
	//iRet = Fchg32(&sendbuf,ROUTE_KEY,0,(char*)cRouteKey,(FLDLEN32)10);
	iRet = Fchg32(sendbuf,ROUTE_KEY,0,(char *)cRouteKey,0);
	if(iRet < 0)
	{
		userlog("cRouteKey=[%s],iRet=[%d]",cRouteKey,iRet);
		sprintf(sendbuf2,"<iResult=1><sMsg=cslfput to ROUTE_KEY is error>");
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	
	iRet = Fchg32(sendbuf,SEND_STRING,0,(char *)sendstring,0);
	if(iRet < 0)
	{
		userlog("sendstring=[%s],iRet=[%d]",sendstring,iRet);

		sprintf(sendbuf2,"<iResult=1><sMsg=cslfput to SEND_STRING is error>");
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	
	
	iRet = tpcall("P_C_S_LOCAL",(char *)sendbuf,0,(char **)&recvbuf,&lRecLen,0);   
    if (iRet < 0){
		sprintf(sendbuf2,"<iResult=1><sMsg=call localsvc err,tperrno:%d>",tperrno);
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	
	tpfree((char *)sendbuf);
	sprintf(sendbuf2,"<iResult=0><sMsg=you tpcall  success>");
	tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	
}


void P_GET_TRANSINFO(TPSVCINFO *rqst)
{
	long drawID = 0;
	int iRet = 0;
	char swsnaddr[30];
	char *allrcvbuf = NULL;
	char *rcvbuf = NULL;
	FBFR32 *sendbuf;
	FBFR32 *recvbuf;
	
	char *sendstring;
	char *sendbuf2=NULL;
	int iLen = rqst->len;
	long  i;
	sendstring = new char[iLen];
	memset(sendstring,'\0',iLen);
	sendbuf2 = (char *) tpalloc("STRING", NULL, MAX_CMAMSG_LEN);

	//char *sendbuf = NULL;
	long lRecLen = 0;
	string s_rcvbuff;
	char cRouteKey[30];
	char tmpBuff[30];

	
	memset(swsnaddr,0,sizeof(swsnaddr));
	allrcvbuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	//rcvbuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	
	if ((sendbuf = (FBFR32 *)tpalloc("FML32",NULL,MAX_CMAMSG_LEN)) == NULL)
	{
		sprintf(sendbuf2,"<iResult=1><sMsg=sendbuf err>");
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	
	if ((recvbuf = (FBFR32 *)tpalloc("FML32",NULL,MAX_CMAMSG_LEN)) == NULL)
	{
		sprintf(sendbuf2,"<iResult=1><sMsg=recvbuf err>");
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}

	//sendbuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	memset(sendstring,0,MAX_CMAMSG_LEN);
	s_rcvbuff.clear();
	
	memcpy(sendstring, rqst->data, iLen);
	sendstring[iLen] = 0;
	TrimHeadNull(sendstring,iLen);
	::PETRI::P_RECVBUFF.reset();
	::PETRI::P_RECVBUFF.parseBuff(sendstring);
	drawID = (long) ::PETRI::P_RECVBUFF.getDrawID();
	
	vector<long> transList;
	transList.clear();
	TOCIQuery query (::PETRI::P_DB.getDB());

	try {
		string sql = "SELECT TRANSITION_ID FROM P_TRANSITION_ATTR WHERE DRAW_ID= :DRAWID AND EXE_STATE = 'R'";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("DRAWID",drawID);
		query.open();
		while(query.next()) {
			transList.push_back(query.field(0).asLong());
		}
		query.close();
		query.commit();
	} catch (TOCIException& toe) {
		query.close();
		query.commit();
		sprintf(sendbuf2,"<iResult=1><sMsg=Get Transition of The Draw Error! %s>",toe.getErrMsg());
		tpfree(rcvbuf);
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	} catch (...) {
		query.close();
		query.commit();
		sprintf(sendbuf2,"<iResult=1><sMsg=Get Transition of The Draw Error!> other error");
		tpfree(rcvbuf);
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	};

	if (0 == transList.size()){
		memset(sendbuf2,0,MAX_CMAMSG_LEN);
		tpfree(rcvbuf);
		tpfree((char *)sendbuf);
		query.commit();
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
	
	for (int idx = 0;idx <transList.size();idx ++ ) {

	try {
		string sql = "select b.wsnaddr from P_TRANSITION_HOST_INFO a, WF_HOST_WSNADDR b where a.wsnaddr_id = b.wsnaddr_id and a.transition_id = :TRANSID";			
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("TRANSID",transList[idx]);
		query.open();
		while(query.next()) {
			strcpy( swsnaddr,query.field(0).asString() );
		}
		query.close();
		
		string sql2 = "select HOST_AREA||HOST_DOMAIN from P_TRANSITION_HOST_INFO where transition_id = :TRANSID";
		query.close();
		query.setSQL(sql2.c_str());
		query.setParameter("TRANSID",transList[idx]);
		query.open();
		while(query.next()) {
			strcpy( tmpBuff,query.field(0).asString());
		}
		query.close();
		query.commit();
	}catch (TOCIException & toe){
		sprintf(sendbuf2,"<iResult=1><lTransitionID=%ld><sMsg=get WSNADDR err:%s>",transList[idx],toe.getErrMsg());
		query.close();
		query.commit();
	} catch(...) {
		sprintf(sendbuf2,"<iResult=1><lTransitionID=%ld><sMsg=get WSNADDR other err>",transList[idx]);
		query.close();
		query.commit();
	}
	if(0 == swsnaddr[0]){
		sprintf(sendbuf2,"<iResult=1><lTransitionID=%ld><sMsg=Transition have no WSNADDR >",transList[idx]);
	}
/*
	iRet = setenv("WSNADDR",swsnaddr,1);
	if( -1 == iRet){
		sprintf(rcvbuf,"<iResult=1><lTransitionID=%ld><sMsg=set WSNADDR fail>",transList[idx]);
	};
*/
	sprintf(sendbuf2,"<lTransitionID=%ld>%s\0",transList[idx],sendstring);
	
	strcpy(cRouteKey, tmpBuff);		
	//iRet = cslfput(&sendbuf,ROUTE_KEY,0,cRouteKey);
	//iRet = Fchg32(&sendbuf,ROUTE_KEY,0,(char*)cRouteKey,(FLDLEN32)10);
	strcpy(cRouteKey, tmpBuff);		
	iRet = Fchg32(sendbuf,ROUTE_KEY,0,(char *)cRouteKey,0);
	//iRet = Fchg32((FBFR32*)(&sendbuf),ROUTE_KEY,0,(char*)cRouteKey,(FLDLEN32)20);
	if(iRet < 0)
	{
		userlog("cRouteKey=[%s],iRet=[%d]",cRouteKey,iRet);
		sprintf(sendbuf2,"<iResult=1><sMsg=cslfput to ROUTE_KEY is error>\0");
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
		
	iRet = Fchg32(sendbuf,SEND_STRING,0,(char *)sendbuf2,0);
	if(iRet < 0)
	{
		userlog("sendstring=[%s],iRet=[%d]",sendstring,iRet);

		sprintf(sendbuf2,"<iResult=1><sMsg=cslfput to SEND_STRING is error>\0");
		tpfree((char *)sendbuf);
		tpreturn(TPSUCCESS,0L,sendbuf2,MAX_CMAMSG_LEN,0L);
	}
		

	
	iRet = tpcall("P_G_T_LOCAL",(char *)sendbuf,0,(char **)&recvbuf,&lRecLen,0);  


    if (iRet < 0){
		sprintf(sendbuf2,"<iResult=1><lTransitionID=%ld><sMsg=call localsvc err,tperrno:%d>\0",transList[idx],tperrno);
	}
	userlog(sendbuf2);
	s_rcvbuff = s_rcvbuff + "$" + sendbuf2;
	}
	
	sprintf (allrcvbuf,"%s\0",s_rcvbuff.c_str());
	tpfree((char *)sendbuf);
	tpfree((char *)rcvbuf);
	
	tpreturn(TPSUCCESS,0L,allrcvbuf,MAX_CMAMSG_LEN,0L);
	
}

};

