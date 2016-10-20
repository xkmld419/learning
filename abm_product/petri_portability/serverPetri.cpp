#include "petri.h"
#include <atmi.h>
#include <userlog.h>
#include </usr/include/stdlib.h>



void TrimHeadNull(char *src,int ilen)
{
    int iPos;
    for (iPos=0; iPos<ilen; iPos++) {
        if (src[iPos]!=0)
            break;        
    }
    if (iPos<ilen)
        strcpy(src,src+iPos);
}


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
	char *rcvbuf = NULL;
	char *sendbuf = NULL;
	long lRecLen = 0;
	memset(swsnaddr,0,sizeof(swsnaddr));
	rcvbuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	sendbuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	
	int iLen = rqst->len;
	memcpy(sendbuf, rqst->data, iLen);
	sendbuf[iLen] = 0;
	TrimHeadNull(sendbuf,iLen);
	::PETRI::P_RECVBUFF.reset();
	::PETRI::P_RECVBUFF.parseBuff(sendbuf);
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
	}catch (TOCIException & toe){
		sprintf(rcvbuf,"<iResult=1><sMsg=get WSNADDR err:%s>",toe.getErrMsg());
		query.close();
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,rcvbuf,strlen(rcvbuf),0L);
	} catch(...) {
		sprintf(rcvbuf,"<iResult=1><sMsg=get WSNADDR other err>");
		query.close();
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,rcvbuf,strlen(rcvbuf),0L);
	}
	if(0 == swsnaddr[0]){
		sprintf(rcvbuf,"<iResult=1><sMsg=Transition have no WSNADDR >");
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,rcvbuf,strlen(rcvbuf),0L);
	}
		char s[100];
	sprintf(s,"%s\0",swsnaddr);
	iRet = putenv(s);
	//iRet = setenv("WSNADDR",swsnaddr,1);
	if( -1 == iRet){
		sprintf(rcvbuf,"<iResult=1><sMsg=set WSNADDR fail>");
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,rcvbuf,strlen(rcvbuf),0L);
	};
	
	iRet = tpcall("P_F_LOCAL",(char *)sendbuf,0,(char **)&rcvbuf,&lRecLen,0);        
    if (iRet < 0){
		sprintf(rcvbuf,"<iResult=1><sMsg=call localsvc err,tperrno:%d>",tperrno);
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,rcvbuf,strlen(rcvbuf),0L);
	}
	
	tpfree(sendbuf);
	tpreturn(TPSUCCESS,0L,rcvbuf,lRecLen,0L);
	
}

void P_CHANGE_STATUS(TPSVCINFO *rqst)
{
	long ltransID=0;
	int iRet=0;
	char swsnaddr[30];
	char *rcvbuf = NULL;
	char *sendbuf = NULL;
	long lRecLen = 0;
	memset(swsnaddr,0,sizeof(swsnaddr));
	rcvbuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	sendbuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	
	int iLen = rqst->len;
	memcpy(sendbuf, rqst->data, iLen);
	sendbuf[iLen] = 0;
	TrimHeadNull(sendbuf,iLen);
	::PETRI::P_RECVBUFF.reset();
	::PETRI::P_RECVBUFF.parseBuff(sendbuf);
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
	}catch (TOCIException & toe){
		sprintf(rcvbuf,"<iResult=1><sMsg=get WSNADDR err:%s>",toe.getErrMsg());
		query.close();
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,rcvbuf,strlen(rcvbuf),0L);
	} catch(...) {
		sprintf(rcvbuf,"<iResult=1><sMsg=get WSNADDR other err>");
		query.close();
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,rcvbuf,strlen(rcvbuf),0L);
	}
	if(0 == swsnaddr[0]){
		sprintf(rcvbuf,"<iResult=1><sMsg=Transition have no WSNADDR >");
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,rcvbuf,strlen(rcvbuf),0L);
	}
	char s[100];
	sprintf(s,"%s\0",swsnaddr);
	iRet = putenv(s);

	//iRet = setenv("WSNADDR",swsnaddr,1);
	if( -1 == iRet){
		sprintf(rcvbuf,"<iResult=1><sMsg=set WSNADDR fail>");
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,rcvbuf,strlen(rcvbuf),0L);
	};
	iRet = tpcall("P_C_S_LOCAL",(char *)sendbuf,0,(char **)&rcvbuf,&lRecLen,0);        
    if (iRet < 0){
		sprintf(rcvbuf,"<iResult=1><sMsg=call localsvc err,tperrno:%d>",tperrno);
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,rcvbuf,strlen(rcvbuf),0L);
	}
	
	tpfree(sendbuf);
	tpreturn(TPSUCCESS,0L,rcvbuf,lRecLen,0L);
	
}

void P_GET_TRANSINFO(TPSVCINFO *rqst)
{
	long drawID = 0;
	int iRet = 0;
	char swsnaddr[30];
	char *allrcvbuf = NULL;
	char *rcvbuf = NULL;
	char *sendbuf = NULL;
	char msgBuff[MAX_CMAMSG_LEN];
	long lRecLen = 0;
	string s_rcvbuff;
	memset(swsnaddr,0,sizeof(swsnaddr));
	allrcvbuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	rcvbuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	sendbuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	memset(msgBuff,0,MAX_CMAMSG_LEN);
	s_rcvbuff.clear();
	
	int iLen = rqst->len;
	memcpy(msgBuff, rqst->data, iLen);
	msgBuff[iLen] = 0;
	TrimHeadNull(msgBuff,iLen);
	::PETRI::P_RECVBUFF.reset();
	::PETRI::P_RECVBUFF.parseBuff(msgBuff);
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
	} catch (TOCIException& toe) {
		query.close();
		sprintf(allrcvbuf,"<iResult=1><sMsg=Get Transition of The Draw Error! %s>",toe.getErrMsg());
		tpfree(rcvbuf);
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,allrcvbuf,strlen(allrcvbuf),0L);
	} catch (...) {
		query.close();
		sprintf(allrcvbuf,"<iResult=1><sMsg=Get Transition of The Draw Error!> other error");
		tpfree(rcvbuf);
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,allrcvbuf,strlen(allrcvbuf),0L);
	};

	if (0 == transList.size()){
		memset(allrcvbuf,0,MAX_CMAMSG_LEN);
		tpfree(rcvbuf);
		tpfree(sendbuf);
		tpreturn(TPSUCCESS,0L,allrcvbuf,strlen(allrcvbuf),0L);
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
	}catch (TOCIException & toe){
		sprintf(rcvbuf,"<iResult=1><lTransitionID=%ld><sMsg=get WSNADDR err:%s>",transList[idx],toe.getErrMsg());
		query.close();
	} catch(...) {
		sprintf(rcvbuf,"<iResult=1><lTransitionID=%ld><sMsg=get WSNADDR other err>",transList[idx]);
		query.close();
	}
	if(0 == swsnaddr[0]){
		sprintf(rcvbuf,"<iResult=1><lTransitionID=%ld><sMsg=Transition have no WSNADDR >",transList[idx]);
	}
		char s[100];
	sprintf(s,"%s\0",swsnaddr);
	iRet = putenv(s);
	//iRet = setenv("WSNADDR",swsnaddr,1);
	if( -1 == iRet){
		sprintf(rcvbuf,"<iResult=1><lTransitionID=%ld><sMsg=set WSNADDR fail>",transList[idx]);
	};

	sprintf(sendbuf,"<lTransitionID=%ld>%s\0",transList[idx],msgBuff);
	
	iRet = tpcall("P_G_T_LOCAL",(char *)sendbuf,0,(char **)&rcvbuf,&lRecLen,0);        
    if (iRet < 0){
		sprintf(rcvbuf,"<iResult=1><lTransitionID=%ld><sMsg=call localsvc err,tperrno:%d>",transList[idx],tperrno);
	}
	userlog(rcvbuf);
	s_rcvbuff = s_rcvbuff + "$" + rcvbuf;
	}
	
	sprintf (allrcvbuf,"%s\0",s_rcvbuff.c_str());
	tpfree(rcvbuf);
	tpfree(sendbuf);
	tpreturn(TPSUCCESS,0L,allrcvbuf,strlen(allrcvbuf),0L);
	
}

};



