#include "onlineUpdate.h"
#include "ReadIni.h"
#include "Log.h"
#include "Process.h"
#include <sys/timeb.h>
#include<dirent.h>
#include <stdio.h>
#include <exception>
//using namespace std;

static TProcessInfo *pProcInfoHead = NULL;
static THeadInfo *pInfoHead = NULL;

onlineUpdate::onlineUpdate()
{
	;
}
onlineUpdate::~onlineUpdate()
{
	;
}
void onlineUpdate::log(int log_level, char const *format, ...)
{
    char *p = NULL, sTemp[10000]={0}, sLine[10000]={0};
    int fd = 0;
    int iLeng=0;
    char sLogFileName[10240] = {0};

    /* LOG_LEVEL */
    if ((p=getenv("HSSDIR")) == NULL)
        return ;
        
    iLeng=strlen(p);

    if (iLeng != 0)
    {
			if(p[iLeng] == '/')
			{
				sprintf(sLogFileName,"%slog/%s",p,"UpdateFile.log");
			}
			else
			{
				sprintf(sLogFileName,"%s/log/%s",p,"UpdateFile.log");
			}
    }
    else
    {
    	return ;
  	}	

    if ((fd=open (sLogFileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0) {
        printf ("[ERROR]: onlineUpdate::log() open %s error\n", sLogFileName);
                return;
    }

    Date dt;
    sprintf (sTemp, "[%s]: %s\n", dt.toString("yyyy-mm-dd hh:mi:ss"), format);

    va_list ap;
    va_start(ap, format);
    vsnprintf (sLine, 9999, sTemp, ap);
    va_end(ap);
    printf ("%s", sLine);
    write (fd, sLine, strlen (sLine));
    close (fd);
}

double  onlineUpdate::get_time()
{
        struct timeb tm = {0};
        double Req_Begin=0,Req_Begin_ms=0,Res_Begin_Time=0;
        ftime(&tm);
        Req_Begin = tm.time;
        return Req_Begin;
}
int onlineUpdate::RunProcess(int RunProcess[1024],int count,int iTimeOut)
{
	const int iMAXAPPNUM=1000;
	bool ierror=false;
	if(iTimeOut == 0)
	{
		this->log (0, "等待时间必须 > 0");
		return -1;
	}
	//联入共享内存
  if (AttachShm (iMAXAPPNUM*sizeof (THeadInfo), (void **)&pInfoHead) >= 0)
  {
      pProcInfoHead = &pInfoHead->ProcInfo;
	}    
	else
	{
		  this->log (0, "AttachShm 失败"); 
	    return -1;
	} 
	
	for (int i=0; i<pInfoHead->iProcNum; i++) 
  {
  		for(int j=0;j<count;j++)
  		{
          if (((pProcInfoHead+i)->iState != ST_RUNNING &&
          	   (pProcInfoHead+i)->iState != ST_INIT &&
          	   (pProcInfoHead+i)->iState != ST_WAIT_BOOT)&&
          		 (pProcInfoHead+i)->iProcessID == RunProcess[j])
      		{   
      			  			
            (pProcInfoHead+i)->iState = ST_WAIT_BOOT;	
						this->log (0, "进程 %d 开始启动",RunProcess[j]);
          
	          double start_time =0, end_time = 0;
	          int Rate =0;
	          start_time = get_time();
	          while(true)
	          {
	          	sleep (1);
	          	
	          	if (((pProcInfoHead+i)->iState == ST_RUNNING))
					  	{
					  		this->log (0, "进程%d,启动成功",RunProcess[j]); 
								break;
					  	}
					  	
					  	end_time = get_time();
							Rate = end_time - start_time;
							if(Rate >= iTimeOut)
							{
								this->log(0,"等待进程 %d 启动超时 %d",RunProcess[j],iTimeOut);
								ierror = true;
								break;
							}
	        	}
	        }
      } 
  }
  
  if (ierror)
  	return -1;
  else
  	return 0;
}
int onlineUpdate::StopProcess(int RunProcess[1024],int count,int iTimeOut)
{
	const int iMAXAPPNUM=1000;
	if(iTimeOut == 0)
	{
		this->log (0, "等待时间必须 > 0");
		return -1;
	}
	//联入共享内存
  if (AttachShm (iMAXAPPNUM*sizeof (THeadInfo), (void **)&pInfoHead) >= 0)
  {
      pProcInfoHead = &pInfoHead->ProcInfo;
	}    
	else
	{
		  this->log (0, "AttachShm 失败"); 
	    return -1;
	} 
	
	for (int i=0; i<pInfoHead->iProcNum; i++) 
  {
  		for(int j=0;j<count;j++)
  		{
          if (((pProcInfoHead+i)->iState == ST_RUNNING ||
          	   (pProcInfoHead+i)->iState == ST_INIT ||
          	   (pProcInfoHead+i)->iState == ST_WAIT_BOOT)&&
          		 (pProcInfoHead+i)->iProcessID == RunProcess[j])
      		{   
      			  			
            (pProcInfoHead+i)->iState = ST_WAIT_DOWN;	
						this->log (0, "进程 %d 开始退出",RunProcess[j]);
          
	          double start_time =0, end_time = 0;
	          int Rate =0;
	          start_time = get_time();
	          while(true)
	          {
	          	sleep (1);
	          	
	          	if (((pProcInfoHead+i)->iState == ST_SLEEP))
					  	{
					  		this->log (0, "进程%d,退出成功",RunProcess[j]); 
								break;
					  	}
					  	
					  	end_time = get_time();
							Rate = end_time - start_time;
							if(Rate >= iTimeOut)
							{
								this->log(0,"等待进程 %d 退出超时 %d",RunProcess[j],iTimeOut);
								return -1;
							}
	        	}
	        }
      } 
  }
  
  return 0;
}
int onlineUpdate::GetProName(int aAppid[1000],int count,string sProName[1000],int &ProNaCount)
{
	DEFINE_QUERY (qry);
	char sql[1024] = {0};
	
	for(int i=0;i<count&&aAppid[i]!=0;i++)	
	{
		sprintf(sql, "select EXEC_NAME from wf_application where APP_ID = %d and Rownum = 1",aAppid[i]);
		qry.setSQL (sql);
		qry.open ();
		bool bnodata=true;
		while (qry.next())
		{
			sProName[i]=qry.field(0).asString();
			bnodata = false;
			ProNaCount ++;
		}
		
		qry.close();
		if(bnodata)
		{
			this->log (0, "wf_application 中 找不到 %d 对应的 EXEC_NAME ，请检查表 wf_application",aAppid[i]); 
			return -1;
		}
	}		
	
	return 0;
}
int onlineUpdate::DisProcessId(int aAppid[1000],int count,int RunProID[1000],int &RunCount)
{
	DEFINE_QUERY (qry);
	char sql[1024] = {0};
	const int iMAXAPPNUM=1000;
	
	for(int i=0;i<count&&aAppid[i]!=0;i++)	
	{
		sprintf(sql, "select PROCESS_ID,CAPTION from WF_PROCESS where APP_ID = %d",aAppid[i]);
		qry.setSQL (sql);
		qry.open ();
		bool bnodata=true;
		while (qry.next())
		{
			int iproid = qry.field(0).asInteger();
			mapProcessID[iproid]=qry.field(1).asString();
			bnodata = false;
		}
		
		qry.close();
		if(bnodata)
		{
			this->log (0, "找不到 %d 对应的 PROCESS_ID ，请检查表 WF_PROCESS",aAppid[i]); 
			return -1;
		}
	}		
	
	//联入共享内存
  if (AttachShm (iMAXAPPNUM*sizeof (THeadInfo), (void **)&pInfoHead) >= 0)
  {
      pProcInfoHead = &pInfoHead->ProcInfo;
	}    
	else
	{
		  this->log (0, "AttachShm 失败"); 
	    return -1;
	} 
	
	for (int i=0; i<pInfoHead->iProcNum; i++) 
  {
  	map<int, string>::iterator  iter;

    for(iter = mapProcessID.begin(); iter != mapProcessID.end(); iter++)
		{
	    if (((pProcInfoHead+i)->iState == ST_RUNNING ||
	    	   (pProcInfoHead+i)->iState == ST_INIT ||
	    	   (pProcInfoHead+i)->iState == ST_WAIT_BOOT)&&
	    		 (pProcInfoHead+i)->iProcessID == iter->first)
			{
				RunProID[RunCount] = iter->first;
				RunCount ++;
	      this->log (0, "进程 %d %s 正在运行",iter->first,iter->second); 
	    }
	   }
      
  }
	
	return 0;
	
}

int onlineUpdate::GetUpdateFile(char* filepath,char* UpFile)
{
	if(NULL == filepath)
		return -1;
		
	ReadIni reader; 
	char sFile[254] = {0};
	char *pfile = filepath;
	char sUpfile[1024] = {0};
	sprintf(sFile,"%s",pfile);
	try
	{
		FILE *fd = fopen(sFile,"r");
		if(fd == NULL)
		{
            ALARMLOG28(0,MBC_CLASS_File,101,"打开文件失败: File[%s]",sFile);
            if(errno == ENOENT)
                ALARMLOG28(0,MBC_CLASS_File,106,"指定文件不存在: File[%s]",sFile);
				this->log (0, "fopen失败, %s",filepath); 
				return -1;
		}	
		reader.readIniStr (fd, "PROCESSFILE", "UPDATEFILE",sUpfile,0);
		if(strlen(sUpfile) == 0)
			return -1;
		else
			sprintf(UpFile,"%s",sUpfile);
	}
	catch(exception& e)
	{
		this->log (0, "fopen 异常 %s %s",e.what(),filepath); 
		return -1;
	}
	
	return 0;
}
	
int onlineUpdate::GetUpdatePro(char* filepath,char* process)
{
	if(NULL == filepath)
		return -1;
		
	ReadIni reader; 
	char sFile[254] = {0};
	char *pfile = filepath;
	char sProApp[1024] = {0};
	sprintf(sFile,"%s",pfile);
	try
	{
		FILE *fd = fopen(sFile,"r");
		if(fd == NULL)
		{
            ALARMLOG28(0,MBC_CLASS_File,101,"打开文件失败: File[%s]",sFile);
            if(errno == ENOENT)
                ALARMLOG28(0,MBC_CLASS_File,106,"指定文件不存在: File[%s]",sFile);
				this->log (0, "fopen失败, %s",filepath); 
				return -1;
		}	
		reader.readIniStr (fd, "PROCESSFILE", "PROCESSAPP",sProApp,0);
		if(strlen(sProApp) == 0)
			return -1;
		else
			sprintf(process,"%s",sProApp);
	}
	catch(exception& e)
	{
		this->log (0, "fopen 异常 %s %s",e.what(),filepath); 
		return -1;
	}
	
	return 0;
}

int main(int argc,char **argv )
{		
	char pPara[10]= {0};
	char sfile[1024] = {0};
	char sprocess[1024] = {0};
	char strcmd[100] = {0};
	int iproid[1000] = {0};
  
	  
	strncpy( pPara,argv[1],sizeof(pPara));
	onlineUpdate Update;
	
	if(strcmp(pPara,"-p") == 0)
	{
		sprintf(sfile,"%s",argv[2]);
		int iret = Update.GetUpdatePro(sfile,sprocess);
		if(0 == iret)
		{
			try
			{
				sprintf(strcmd,"echo \"export APPID=\\\"%s \\\"\"  > $HSSDIR/log/onlineUpdate.sh",sprocess);
				system(strcmd);
			}
			catch(exception& e)
			{
				Update.log (0, "- p system 异常 %s %s",e.what(),strcmd); 
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	else if(strcmp(pPara,"-f") == 0)
	{
		char sUpfile[1024] = {0};
		sprintf(sfile,"%s",argv[2]);
		int iret = Update.GetUpdateFile(sfile,sUpfile);
		if(0 == iret)
		{
			try
			{
				sprintf(strcmd,"echo \"export UPDATEFILE=\\\"%s \\\"\"  > $HSSDIR/log/onlineUpdate.sh",sUpfile);
				system(strcmd);
			}
			catch(exception& e)
			{
				Update.log (0, "- f system 异常 %s %s",e.what(),strcmd); 
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	else if(strcmp(pPara,"-n") == 0)
	{
		int j = 0;
		int iproid[1000] = {0};
		string sProName[1000]={0};
		int iProNameCount=0;
		for(int i=2;i<argc;i++)
		{
			iproid[j++]=atoi(argv[i]);
		}
		
		if(0 == iproid[0])
			return -1;
		
		int iret = Update.GetProName(iproid,j,sProName,iProNameCount);
	  if(iret != 0)
	  	return -1;
	  
	  if(iProNameCount == 0 || sProName[0].length() == 0)
	  {
	  	Update.log (0, "没有对应的 EXEC_NAME"); 
	  }
	  else
	  {
	  	char pProName[1024] = {0};
	  	for(int i=0;i<iProNameCount;i++)
	  	{
	  		sprintf(pProName,"%s %s",pProName,sProName[i].c_str());
	  	}
	  	try
			{
				sprintf(strcmd,"echo \"export PROCESSNAME=\\\"%s \\\"\"  > $HSSDIR/log/onlineUpdate.sh",pProName);
				system(strcmd);
			}
			catch(exception& e)
			{
				Update.log (0, "- n system 异常 %s %s",e.what(),strcmd); 
				return -1;
			}
		}
	}
	else if(strcmp(pPara,"-l") == 0)
	{
		int j = 0;
		int iRunProID[1000]={0};
		int iRunCount=0;
		for(int i=2;i<argc;i++)
		{
			iproid[j++]=atoi(argv[i]);
		}
		
		if(0 == iproid[0])
			return -1;
		
		int iret = Update.DisProcessId(iproid,j,iRunProID,iRunCount);
	  if(iret != 0)
	  	return -1;
	  
	  if(iRunCount == 0 || iRunProID[0] == 0)
	  {
	  	Update.log (0, "没有对应的活动进程在运行"); 
	  }
	  else
	  {
	  	char pRunProid[1024] = {0};
	  	for(int i=0;i<iRunCount;i++)
	  	{
	  		sprintf(pRunProid,"%s %d",pRunProid,iRunProID[i]);
	  	}
	  	try
			{
				sprintf(strcmd,"echo \"export RUNPROCESSID=\\\"%s \\\"\"  > $HSSDIR/log/onlineUpdate.sh",pRunProid);
				system(strcmd);
			}
			catch(exception& e)
			{
				Update.log (0, "- l system 异常 %s %s",e.what(),strcmd); 
				return -1;
			}
		}
	}
	else if(strcmp(pPara,"-b") == 0)
	{
		int aRunProcess[1024]={0};
		int icount = 0;
		int iTime=0;
		
		iTime=atoi(argv[2]);
		
		if(0 == iTime || strlen(argv[3]) == 0)
			return -1;
		
		for(int i=0;i<1000 && atoi(argv[i+3]) !=0;i++)
		{
			aRunProcess[i]=atoi(argv[i+3]);
			icount ++;
		}
		
		int iret = Update.StopProcess(aRunProcess,icount,iTime);
		if(iret != 0)
	  	return -1;
	}
	else if(strcmp(pPara,"-r") == 0)
	{
		int aRunProcess[1024]={0};
		int icount = 0;
		int iTime=0;
		
		iTime=atoi(argv[2]);
		
		if(0 == iTime || strlen(argv[3]) == 0)
			return -1;
		
		for(int i=0;i<1000 && atoi(argv[i+3]) !=0;i++)
		{
			aRunProcess[i]=atoi(argv[i+3]);
			icount ++;
		}
		
		int iret = Update.RunProcess(aRunProcess,icount,iTime);
		if(iret != 0)
	  	return -1;
	}
	else
	{
		return -1;
	}		
			
	return 0;
}
