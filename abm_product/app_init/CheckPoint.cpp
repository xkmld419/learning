#include "CheckPoint.h"
#include  <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <signal.h>
#include <unistd.h>
#include "ReadIni.h"
#include "Date.h"
#include "ParamDefineMgr.h"
#include <sys/wait.h>

int iResult;

void ProcessEnd ()
{
     iResult = -1;
}

CheckPoint::CheckPoint()
{
	m_pCheckPoint = 0;
	INFOPOINT(3,"%s","[CheckPoint.CheckPoint]初始化数据");
}

CheckPoint::~CheckPoint()
{
	INFOPOINT(3,"%s","[CheckPoint.~CheckPoint]析构数据");
}

void CheckPoint::getBatch()
{
     this->sBatch[0] = '\0';
	 Date *p = new Date();
	 strcpy(this->sBatch,p->toString());
	 delete p;
	 p = 0;
	 INFOPOINT(3,"%s","[CheckPoint.getBatch]获得批次编号");
}

void CheckPoint::chkrec()
{
	 int pid = getpid();
	 char filename[512] = {0};
	 FILE *fp; 
	 char *p = getenv("BILLDIR");
	 getBatch();
	 if(p[strlen(p)-1] != '/')
	 	sprintf(filename,"%s%s%s",p,"//log/check_point/chkfile_",this->sBatch);
	 else
	 	sprintf(filename,"%s%s%s",p,"log/check_point/chkfile_",this->sBatch);
	 if ( (fp = fopen(filename,"r+"))== NULL)
	 {
	 	if((fp=fopen(filename,"wb")) ==NULL) 
	 	{ 
			Log::log(0,"PID[%d]写文件失败",pid);
			return;
		}
	} 
	char sline[256] = {0};
	sprintf(sline,"%s%s%s%d","本次落地数据, 批次[",this->sBatch,"] PID: ",pid);
	/*Date date;
	strcpy(fileHeadInfo.date , date.toString());
	strcpy(fileHeadInfo.batchid,this->sBatch);
	fileHeadInfo.pid = pid;
	fileHeadInfo.cline = '\n';*/
	/*if (feof (fp)){
		fwrite(fileHeadInfo.sline,strlen(FileHeadInfo),1,fp);
	} else {
		FileHeadInfo fileInfo;
		fread (&fileInfo, sizeof(FileHeadInfo), 1, fp);
		
		if(fileHeadInfo.pid != fileInfo.pid)
		{	
			fwrite(fileHeadInfo.sline,strlen(lines),1,fp);
		}
	}*/
	fwrite(sline,strlen(sline),1,fp);
	fclose(fp); 
	
}

bool CheckPoint::touchInfoDown()
{
	 INFOPOINT(3,"%s","[CheckPoint.touchInfoDown]落地共享内存数据");
	 if(!m_pCheckPoint)
	 {
	 	m_pCheckPoint = new CheckPointMgr();
	 }
	string process;
	process.clear();
	chkrec();
	return m_pCheckPoint->touchInfoDown();
}


int CheckPoint::run()
{
	/*char *tmp = getenv("HBCONFIGDIR");
	string sIniFile = tmp;
	tmp = getenv("BILLINGCONFIG");
	sIniFile += "/";
	sIniFile += tmp;
	ReadIni m_oIni;*/
	
	int iRecord = 1;
	iResult = 0;
	iWaitTime = 0;
	//Process::m_pCommandCom->GetInitParam("CHECKPOINT");
	//CommandCom *pCommandCom = new CommandCom();
	//pCommandCom->InitClient();
	char sHome[254];
	char * p;
	if ((p=getenv ("BILLDIR")) == NULL)
		sprintf (sHome, "/home/bill/BILLDIR");
	else
		sprintf (sHome, "%s", p);
	
	
	char sFile[254];
	if (sHome[strlen(sHome)-1] == '/') {
		sprintf (sFile, "%setc/hssconfig", sHome);
	} else {
		sprintf (sFile, "%s/etc/hssconfig", sHome);
	}
	ReadIni mIni;
	mIni.readIniString (sFile, "CHECKPOINT", "log_checkpoint_timeout", sWaitTime,NULL);
	if(sWaitTime[0] == '\0')
	{
		printf("获取初始等待参数[CHECKPOINT.log_checkpoint_timeout]失败!\n");
		return -1;
	} else {
		iWaitTime = atoi(sWaitTime);
	}
	while(1)
	{		
		signal( SIGINT,  (void (*)(int))ProcessEnd );
		signal(SIGSTOP,(void (*)(int))ProcessEnd );
		signal(SIGKILL,(void (*)(int))ProcessEnd );	
		signal (SIGHUP, (void (*)(int))ProcessEnd);
		signal (SIGQUIT, (void (*)(int))ProcessEnd);
		if(iResult == -1){
				return -1;
		}
		printf("%s%d%s\n","第",iRecord,"次落地!");
		
		touchInfoDown();
		iRecord++;	
		char sValue[16]={0};
		ParamDefineMgr::getParam("CHECKPOINT","CHECKPOINT",sValue);
		if(sValue[0] != '\0')
		{
			int iValue = atoi(sValue);
			if(iValue!=0)
			{
				int iPid=0;
		 		if((iPid= fork()==0))
         		{
        			execlp("/bin/sh","sh","-c","bkevent.sh",NULL);
					exit(3);
         		}
         		wait(0);
			}
		}
		if(iWaitTime>0)
    		sleep(iWaitTime);
		sWaitTime[0] = 0;
		mIni.readIniString (sFile, "CHECKPOINT", "log_checkpoint_timeout", sWaitTime,NULL);
		if(sWaitTime[0] != '\0')
		{
			iWaitTime = atoi(sWaitTime);
		}
	}
	
}

/*
int main()
{
	CheckPoint *p = new CheckPoint();
	if(p)
	{
		return p->run();
	}
	return 0;
}*/