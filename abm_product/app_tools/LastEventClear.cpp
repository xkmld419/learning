/* add by  zhuangxf */
/* date:20110428 */

#include "LastEventClear.h"
#include "Exception.h"
#include "mntapi.h"
#include <dirent.h>
#include <stdio.h>
#include <iostream>
#include <stdarg.h>
static TProcessInfo *pProcInfoHead = NULL;
static THeadInfo *pInfoHead = NULL;

ClearLastEvent::ClearLastEvent()
{
	if(p_shmTable)
		p_shmTable = NULL;
}

//-------------------------------------------------------------------------------
ClearLastEvent::~ClearLastEvent()
{
	if(p_shmTable)
		p_shmTable = NULL;
}

//-------------------------------------------------------------------------------
TSHMCMA * ClearLastEvent::GetShmTable()
{
	long lShmKey = IpcKeyMgr::getIpcKey (-1,"SHM_WorkFlowKey");
	if (lShmKey < 0)
	{
		p_shmTable=NULL;
		return NULL;
	}
	if (GetShmAddress(&p_shmTable,lShmKey)<0 )
	{
		p_shmTable = NULL;
		return NULL;
	}

	return p_shmTable;
}

//-------------------------------------------------------------------------------
int ClearLastEvent::GetShmAddress(TSHMCMA **ppTable,long iShmKey)
{
	int iShmID;

	//获取共享内存ID
	if ((iShmID = shmget(iShmKey,0,0))<0){
		return -1;
	}

	//get the buff addr.
	*ppTable = (TSHMCMA *)shmat(iShmID,(void *)0,0);

	if (-1 == (long)(*ppTable)){
		return -2;
	}

	return 0;
}
//-------------------------------------------------------------------------------
bool ClearLastEvent::SetEventStateProcessID(int iProcessID,int iState )
{
	char cResultValue[512]={0};
	this->GetShmTable();//联接共享内存
	if(!p_shmTable)
	{
		this->log("联接共享内存失败!");		
		THROW(MBC_SimpleSHM+6);
	}
	int iLastEventState=0;  //最后一条话单的状态
	TAppSaveBuff *MyAppbuff=NULL;
	if (!MyAppbuff && p_shmTable) 
		MyAppbuff = p_shmTable->getAppSaveBuffByProcID(iProcessID);
	if (!MyAppbuff)
	{
		sprintf(cResultValue,"在内存中PROCESS_ID=%d无缓冲信息!",iProcessID);
		this->log(cResultValue);
		return false;
	}
	if(MyAppbuff->getEventState(&iLastEventState))
	{
		MyAppbuff->setEventState(iState); //设置最后一条话单的状态为1:已经处理状态	
		sprintf(cResultValue,"PROCESS_ID=%d::LastEventstate:%d---->%d",iProcessID,iLastEventState,iState);
		this->log(cResultValue);
		return true;
	}
	else
	{
		sprintf(cResultValue,"进程PROCESS_ID=%d无最后一条话单信息",iProcessID);
		this->log(cResultValue);	
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------
void ClearLastEvent::log(char *format,...)
{
	char *p = NULL, sTemp[10000]={0}, sLine[10000]={0};
	int fd = 0;
	int iLeng=0;
	char sLogFileName[10240] = {0};

	if ((p=getenv("BILLDIR")) == NULL)
		return ;

	iLeng=strlen(p);

	if (iLeng != 0)
	{
		if(p[iLeng] == '/')
			sprintf(sLogFileName,"%slog/%s",p,"ClearEvent.log");
		else
			sprintf(sLogFileName,"%s/log/%s",p,"ClearEvent.log");
	}
	else
		return ;

	if ((fd=open (sLogFileName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0) 
	{
		printf ("[ERROR]: ClearEvent::log() openFile %s error\n", sLogFileName);
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

//-------------------------------------------------------------------------------
void showExpHelp()
{
	printf("\n ****************************************************************   \n") ;
	printf("  1:设置所有最后一条话单处理状态为已经处理 \n") ;
	printf("     ex1:cleanlastevent -a \n");
	printf("  2:设置流程最后一条话单处理状态为已经处理  \n");
	printf("     ex2:cleanlastevent -b BILLFLOW_ID \n");
	printf("  3:设置进程最后一条话单处理状态为已经处理  \n");
	printf("     ex3:cleanlastevent -p PROCESS_ID \n");
	printf("  4:设置进程最后一条话单处理状态为未处理  \n");
	printf("     ex4:cleanlastevent -s PROCESS_ID \n");	
	printf(" *****************************************************************   \n") ;
}

//-------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	int iBillflowID=-1;	//流程ID
	int iProcessID=-1;	//进程ID
	int iCount = 0; //共修改的进程数
	char cTmpResult[512]={0};
	TAppSaveBuff * cMyAppBuff=NULL;
	ClearLastEvent *tClearEvent = new ClearLastEvent();	//处理类
	if(argc > 1)
	{
		if(argc == 2 && strcmp(argv[1],"-help")==0)
		{
			showExpHelp();
			return 0;
		}
		try 
		{	//coding here
			if(pInfoHead)
				pInfoHead = NULL;			
			if (AttachShm (1000*sizeof (THeadInfo), (void **)&pInfoHead) >= 0)
			{
				pProcInfoHead = &pInfoHead->ProcInfo;
			}    
			else
			{
				tClearEvent->log ( "联接共享内存AttachShm 失败"); 
				return -1;
			} 		
			if(strcmp(argv[1],"-b")==0 || strcmp(argv[1],"-B")==0)
			{
				//对一个流程的最后一条话单进行处理
				iBillflowID = atoi(argv[2]);
				for(int i=0; i<pInfoHead->iProcNum;i++)
				{
					if((pProcInfoHead+i)->iBillFlowID == iBillflowID) 
					{
						if((pProcInfoHead+i)->iState != ST_RUNNING && 
							(pProcInfoHead+i)->iState != ST_INIT &&
							(pProcInfoHead+i)->iState != ST_WAIT_BOOT)
	       	   			{
							if(!tClearEvent->SetEventStateProcessID((pProcInfoHead+i)->iProcessID,1))
								continue;
							iCount++;
	       	   			}
	       	   			else
	       	   			{
							sprintf(cTmpResult,"PROCESSS_ID:%d进程的状态不是正常停止状态不能清理",(pProcInfoHead+i)->iProcessID);
							tClearEvent->log(cTmpResult);
							continue;
	       	   			}
	       	   		}
				}
				if(iCount>0)
				{
					sprintf(cTmpResult,"共更新%d个进程的最后一条话单状态",iCount);	
					tClearEvent->log(cTmpResult);
				}
				else
					tClearEvent->log("未找到相应的进程进行更新");
			}
			else if(strcmp(argv[1],"-p")==0 || strcmp(argv[1],"-P")==0)
			{
				//对一个进程的最后一条话单进行处理
				iProcessID = atoi(argv[2]);
				
				for(int i=0; i<pInfoHead->iProcNum;i++)
				{
					if((pProcInfoHead+i)->iProcessID == iProcessID) 
					{
						iCount++;
						if((pProcInfoHead+i)->iState != ST_RUNNING && 
							(pProcInfoHead+i)->iState != ST_INIT &&
							(pProcInfoHead+i)->iState != ST_WAIT_BOOT)
	       	   			{
							if(!tClearEvent->SetEventStateProcessID((pProcInfoHead+i)->iProcessID,1))
							{
								break;								
							}
							else
							{
								sprintf(cTmpResult,"PROCESS_ID=%d更新成功",iProcessID);
								tClearEvent->log(cTmpResult);
								break;
							}
	       	   			}
	       	   			else
	       	   			{
							sprintf(cTmpResult,"PROCESSS_ID:%d进程的状态不是正常停止状态不能清理",(pProcInfoHead+i)->iProcessID);
							tClearEvent->log(cTmpResult);
							break;
	       	   			}
	       	   		}
				}
				if(iCount==0)
				{
					sprintf(cTmpResult,"未找到PROCESSS_ID:%d进程",iProcessID);
					tClearEvent->log(cTmpResult);
				}					
			}
			else if(strcmp(argv[1],"-s")==0 || strcmp(argv[1],"-S")==0)
			{
				//对一个进程的最后一条话单进行处理
				iProcessID = atoi(argv[2]);
				
				for(int i=0; i<pInfoHead->iProcNum;i++)
				{
					if((pProcInfoHead+i)->iProcessID == iProcessID) 
					{
						iCount++;
						if((pProcInfoHead+i)->iState != ST_RUNNING && 
							(pProcInfoHead+i)->iState != ST_INIT &&
							(pProcInfoHead+i)->iState != ST_WAIT_BOOT)
	       	   			{
							if(!tClearEvent->SetEventStateProcessID((pProcInfoHead+i)->iProcessID,0))
							{
								break;								
							}
							else
							{
								sprintf(cTmpResult,"PROCESS_ID=%d更新成功",iProcessID);
								tClearEvent->log(cTmpResult);
								break;
							}
	       	   			}
	       	   			else
	       	   			{
							sprintf(cTmpResult,"PROCESSS_ID:%d进程的状态不是正常停止状态不能清理",(pProcInfoHead+i)->iProcessID);
							tClearEvent->log(cTmpResult);
							break;
	       	   			}
	       	   		}
				}
				if(iCount==0)
				{
					sprintf(cTmpResult,"未找到PROCESSS_ID:%d进程",iProcessID);
					tClearEvent->log(cTmpResult);
				}					
			}			
			else if(strcmp(argv[1],"-a")==0  || strcmp(argv[1],"-A")==0)
			{
				//对所有流程的最后一条话单进行处理
				char ch;
				cout<<"确实要清理吗?清理后只能用-s进行恢复!(y/n)"<<endl;
				cin>>ch;
				if(!(ch=='1' ||ch =='y' || ch=='Y'))  //不确认进行退出
					return 0;
				for(int i=0; i<pInfoHead->iProcNum;i++)
				{
					if((pProcInfoHead+i)->iProcessID >0)
					{
						if((pProcInfoHead+i)->iState != ST_RUNNING && 
							(pProcInfoHead+i)->iState != ST_INIT &&
							(pProcInfoHead+i)->iState != ST_WAIT_BOOT)
	       	   			{
							if(!tClearEvent->SetEventStateProcessID((pProcInfoHead+i)->iProcessID,1))
								continue;
	       	   			}
	       	   			else
	       	   			{
							sprintf(cTmpResult,"PROCESSS_ID:%d进程的状态不是正常停止状态不能清理",(pProcInfoHead+i)->iProcessID);
							tClearEvent->log(cTmpResult);
							continue;
	       	   			}
	       	   		}
				}
			}
			else
			{
				showExpHelp();
				return 0;
			}	
			if(tClearEvent)  //释放内存
			{
				delete tClearEvent;
				tClearEvent = NULL;
			}
			if(pProcInfoHead || pInfoHead)
			{
				pInfoHead = NULL;
				pProcInfoHead=NULL;
			}			
		}
		catch (Exception &e) 
		{
			sprintf(cTmpResult,"收到Exception退出\n[EXCEPTION]: %s",e.descript());
			tClearEvent->log(cTmpResult);	
			if(tClearEvent)
			{
				delete tClearEvent;
				tClearEvent = NULL;
			}
			if(pProcInfoHead || pInfoHead)
			{
				pInfoHead = NULL;
				pProcInfoHead=NULL;
			}			
		}
		catch (...) 
		{
			cout << "[EXCEPTION]: Exception catched" <<endl;		
			if(tClearEvent)
			{
				delete tClearEvent;
				tClearEvent = NULL;
			}
			if(pProcInfoHead || pInfoHead)
			{
				pInfoHead = NULL;
				pProcInfoHead=NULL;
			}
		}
	}
	else
	{
		showExpHelp();
		return 0;	
	}
}
//-------------------------------------------------------------------------------

