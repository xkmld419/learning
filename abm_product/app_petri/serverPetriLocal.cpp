#include "petri.h"
#include "tblflds.h"

#define WAIT_TIME 10

extern "C" {

long pid;

int tpsvrinit(int argc, char **argv)
{
	userlog("%s Raise\n",argv[0]);
	pid = (long) getpid();
  return 0;
}

void P_F_LOCAL(TPSVCINFO *rqst)
{
	char *sQqstBuf;
	sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	int iRet = 0;
	FBFR32  *recvbuf = NULL;
	//recvbuf=(FBFR32 *) tpalloc("FML32",NULL,MAX_CMAMSG_LEN);
	
	FBFR32  *sendbuf = NULL;
	sendbuf=(FBFR32 *) tpalloc("FML32",NULL,MAX_CMAMSG_LEN);	
	
	int iLen = rqst->len;
	//memcpy(sQqstBuf, rqst->data, iLen);
	//memcpy(recvbuf, rqst->data, iLen);
	recvbuf = (FBFR32 *)rqst->data;
	
	Fget32(recvbuf, SEND_STRING, 0, sQqstBuf, 0);
	
	//sQqstBuf[iLen] = 0;
	TrimHeadNull(sQqstBuf,iLen);

	::PETRI::P_CMDMSG->mtype = (long)pid;
	sprintf(::PETRI::P_CMDMSG->mbody,"<iNeedAck=1>%s\0",sQqstBuf);
	while ( ::PETRI::P_MQ.ackMQ.receive(pid) ){
		userlog("%ld,%s\n",::PETRI::P_ACKMSG->mtype,::PETRI::P_ACKMSG->mbody);
	};
	memset(recvbuf,0,MAX_CMAMSG_LEN);
	if(!(::PETRI::P_MQ.cmdMQ.send())){
		tpfree((char *)recvbuf);
		memset(sQqstBuf,0,MAX_CMAMSG_LEN);
		strcpy(sQqstBuf,"<iResult=1><sMsg=MQ Send Fail>");
		iRet = Fchg32(recvbuf,SEND_STRING,0,(char *)sQqstBuf,0);
		if( iRet > 0 )
			tpreturn(TPSUCCESS,0L,(char *)recvbuf,MAX_CMAMSG_LEN,0L);
		else
			userlog("P_F_LOCAL Error sQqstBuf!");
		tpfree((char *)recvbuf);	
	}
	int iTime=0;
	bool ifAck = false;
	//tpfree((char *)recvbuf);
	while ( iTime++ < WAIT_TIME ){
		if (::PETRI::P_MQ.ackMQ.receive(pid)){ 
			ifAck = true;
			break;
		}
		sleep(1);
	}

	if (ifAck) {
		strcpy(sQqstBuf,::PETRI::P_ACKMSG->mbody);
		userlog(::PETRI::P_ACKMSG->mbody);
	} 
	/*
	else {
		while ( ::PETRI::P_MQ.cmdMQ.receive(pid) ){};
		strcpy(sQqstBuf,"<iResult=1><sMsg=schd order send OK,wait result timeOut>");
	}
*/
	tpfree((char *)recvbuf);
	//strcpy(sQqstBuf,"OK");
	char sTemp[]="OK";
	//iRet = Fchg32(recvbuf,SEND_STRING,0,(char *)sQqstBuf,0);
	iRet = Fchg32(sendbuf,SEND_STRING,0,(char *)sTemp,0);
	tpreturn(TPSUCCESS,0L,(char *)sendbuf,MAX_CMAMSG_LEN,0);	
};

void P_C_S_LOCAL(TPSVCINFO *rqst)
{
	char *sQqstBuf;
	//sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	//int iLen = rqst->len;
	//memcpy(sQqstBuf, rqst->data, iLen);
	
	sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	int iRet = 0;
	FBFR32  *recvbuf = NULL;
	//recvbuf=(FBFR32 *) tpalloc("FML32",NULL,MAX_CMAMSG_LEN);
	
	FBFR32  *sendbuf = NULL;
	sendbuf=(FBFR32 *)tpalloc("FML32",NULL,MAX_CMAMSG_LEN);	
	
	int iLen = rqst->len;
	//memcpy(sQqstBuf, rqst->data, iLen);
	//memcpy(recvbuf, rqst->data, iLen);
	recvbuf=(FBFR32 *)rqst->data;
	
	Fget32(recvbuf, SEND_STRING, 0, sQqstBuf, 0);
	

	
	//sQqstBuf[iLen] = 0;
	TrimHeadNull(sQqstBuf,iLen);


	::PETRI::P_CMDMSG->mtype = (long)pid;
	sprintf(::PETRI::P_CMDMSG->mbody,"<iNeedAck=1>%s\0",sQqstBuf);
	while ( ::PETRI::P_MQ.ackMQ.receive(pid) ){
		userlog("%ld,%s\n",::PETRI::P_ACKMSG->mtype,::PETRI::P_ACKMSG->mbody);
	};
	if(!(::PETRI::P_MQ.cmdMQ.send())){
		tpfree((char *)recvbuf);
		memset(sQqstBuf,0,MAX_CMAMSG_LEN);
		strcpy(sQqstBuf,"<iResult=1><sMsg=MQ Send Fail>");
		iRet = Fchg32(recvbuf,SEND_STRING,0,(char *)sQqstBuf,0);
		if( iRet > 0 )
			tpreturn(TPSUCCESS,0L,(char *)recvbuf,strlen(sQqstBuf),0L);
		else
			userlog("P_F_LOCAL Error sQqstBuf!");
	}
	int iTime=0;
	bool ifAck = false;

	while ( iTime++ < WAIT_TIME ){
		if (::PETRI::P_MQ.ackMQ.receive(pid)){ 
			ifAck = true;
			break;
		}
		sleep(1);
	}

	if (ifAck) {
		strcpy(sQqstBuf,::PETRI::P_ACKMSG->mbody);
		userlog(::PETRI::P_ACKMSG->mbody);
	} 
	/*
	else {
		while ( ::PETRI::P_MQ.cmdMQ.receive(pid) ){};
		strcpy(sQqstBuf,"<iResult=1><sMsg=schd order send OK,wait result timeOut>");
	}
*/
	//strcpy(sQqstBuf,"OK");
	//tpfree((char *)recvbuf);
	char sTemp[10]="OK";
	//iRet = Fchg32(recvbuf,SEND_STRING,0,(char *)sQqstBuf,0);
	iRet = Fchg32(sendbuf,SEND_STRING,0,(char *)sTemp,0);
	userlog("iRet=[%d]",iRet);
	tpreturn(TPSUCCESS,0L,(char *)sendbuf,MAX_CMAMSG_LEN,0);	


	//tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);



}

void P_G_T_LOCAL(TPSVCINFO *rqst)
{
	char *sQqstBuf;
	long ltransID=0;
	
	sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	int iRet = 0;
	FBFR32  *recvbuf = NULL;
	//recvbuf=(FBFR32 *) tpalloc("FML32",NULL,MAX_CMAMSG_LEN);
	
	FBFR32  *sendbuf = NULL;
	sendbuf=(FBFR32 *) tpalloc("FML32",NULL,MAX_CMAMSG_LEN);	
	
	int iLen = rqst->len;
	//memcpy(sQqstBuf, rqst->data, iLen);
	//memcpy(recvbuf, rqst->data, iLen);
	recvbuf=(FBFR32 *)rqst->data;

	Fget32(recvbuf, SEND_STRING, 0, sQqstBuf, 0);
	
	
	//sQqstBuf = tpalloc("STRING",NULL,MAX_CMAMSG_LEN);
	//int iLen = rqst->len;
	//memcpy(sQqstBuf, rqst->data, iLen);
	//sQqstBuf[iLen] = 0;
	TrimHeadNull(sQqstBuf,iLen);
	::PETRI::P_RECVBUFF.reset();
	::PETRI::P_RECVBUFF.parseBuff(sQqstBuf);
	ltransID = ::PETRI::P_RECVBUFF.getTransitionID();

	::PETRI::P_CMDMSG->mtype = (long)pid;
	sprintf(::PETRI::P_CMDMSG->mbody,"<iNeedAck=1>%s\0",sQqstBuf);
	while ( ::PETRI::P_MQ.ackMQ.receive(pid) ){
		userlog("%ld,%s\n",::PETRI::P_ACKMSG->mtype,::PETRI::P_ACKMSG->mbody);
	};
	if(!(::PETRI::P_MQ.cmdMQ.send())){
		tpfree((char *)recvbuf);
		memset(sQqstBuf,0,MAX_CMAMSG_LEN);
		sprintf(sQqstBuf,"<iResult=1><lTransitionID=%ld><sMsg=MQ Send Fail>",ltransID);
		iRet = Fchg32(recvbuf,SEND_STRING,0,(char *)sQqstBuf,0);
		if( iRet > 0 )
			tpreturn(TPSUCCESS,0L,(char *)recvbuf,strlen(sQqstBuf),0L);
		else
			userlog("P_F_LOCAL Error sQqstBuf!");

	}
	int iTime=0;
	bool ifAck = false;

	while ( iTime++ < WAIT_TIME ){
		if (::PETRI::P_MQ.ackMQ.receive(pid)){ 
			ifAck = true;
			break;
		}
		sleep(1);
	}

	if (ifAck) {
		strcpy(sQqstBuf,::PETRI::P_ACKMSG->mbody);
		userlog(::PETRI::P_ACKMSG->mbody);
	} 
	/*
	else {
		while ( ::PETRI::P_MQ.cmdMQ.receive(pid) ){};
		sprintf(sQqstBuf,"<iResult=1><lTransitionID=%ld><sMsg=schd order send OK,wait result timeOut>",ltransID);
	}
	*/
	userlog(sQqstBuf);
		
	//strcpy(sQqstBuf,"OK");
	char sTemp[]="OK";
	//iRet = Fchg32(recvbuf,SEND_STRING,0,(char *)sQqstBuf,0);
	iRet = Fchg32(sendbuf,SEND_STRING,0,(char *)sTemp,0);
	tpreturn(TPSUCCESS,0L,(char *)sendbuf,MAX_CMAMSG_LEN,0);	

	//tpreturn(TPSUCCESS,0L,sQqstBuf,strlen(sQqstBuf),0L);



}

}


