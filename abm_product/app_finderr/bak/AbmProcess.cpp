#include "AbmProcess.h"
//#include "mntapi.h"

/***
flag为false时，表示初始化检查
flag为true时，表示为进程启动检查

***/

int ABMProcess::CheckProcess(int iProcessId)
{
	char sCommand[256] = {0};
	char *sAudit=NULL;
	char *sAuditT=NULL;
	long numProcess=0;
	FILE *fp = 0;
  
	   sprintf (sCommand, "ps -ef|grep -v grep|grep '%d'|wc -l",iProcessId);
	   fp = popen (sCommand, "r");
	   if (fp == NULL)
	   {
	   	return -1;
	   }
	   
	   while(!feof(fp))
	   {
	   	if( !fgets(sAudit,128,fp) )
	   	{	break;}
	   }
	   fclose(fp);
	   numProcess= strtol(sAudit, &sAuditT, 10);
	   if(numProcess>0){
	      
	      return 2;
	   }
	   return 0;
		
	   return 0;
		
}


int ABMProcess::StartProcess(int iProcessId)
{
	char sCommand[256] = {0};
  long lMinProc=0;
  long lMaxProc=0;
	FILE *fp = 0;
	char *sModelProc;
	char *sParammeter;
	char *sVodID;
	bool iFlag=false;
  
	m_poConfS = NULL; 
  if (m_poConfS == NULL) { 
      if ((m_poConfS=new ReadCfg) == NULL) {
           Log::log(0, "ABMConf::getConf m_poConfS malloc failed!");
           return -1;
        }
    }
	  TableCol = 0;
    char path[1024];
	  int checkI=0;
	  int checkJ=0;
	  snprintf(path, sizeof(path), "%s%s", HomeEnv,ABMPROCONFIG, "EXPIMP.ini");
    //snprintf(path, sizeof(path), "/cbilling/app/petri/test.ini");
    if (m_poConfS->read(oExp, path, "SYSTEM_MONITOR") != 0) {
            Log::log(0, "check table config failed!");
            return -1;
    }
	TableCol = m_poConfS->getRows();
	for(checkI=0;checkI<TableCol;checkI++)
	{
	    checkJ = checkI+1;
	    //cout<<"处理的数据条数"<<checkJ<<endl;
	    if (!m_poConfS->getValue(oExp,lMinProc , "process_id_min", checkJ)) {
                  Log::log(0, "配置文件中COLFLAG,读取失败!");
                  return -1;
      }
			  
	    if (!m_poConfS->getValue(oExp, lMaxProc, "process_id_max", checkJ)) {
                    Log::log(0, "配置文件中COLNAME,读取失败!");
                    return -1;
      }
      if((iProcessId>lMinProc)&&(iProcessId<lMaxProc)){
      	iFlag=true;
      	
      }
      else{
      	
      	break;
      	}
      
      if (!m_poConfS->getValue(oExp, sModelProc, "app_exec", checkJ)) {
                    Log::log(0, "配置文件中COLNAME:app_exec,读取失败!");
                    return -1;
      }
      if (!m_poConfS->getValue(oExp, sParammeter, "parammeter", checkJ)) {
                    Log::log(0, "配置文件中COLNAME:parammeter,读取失败!");
                    return -1;
      }
      if (!m_poConfS->getValue(oExp, sVodID, "vod_id", checkJ)) {
                    Log::log(0, "配置文件中COLNAME:vod_id,读取失败!");
                    return -1;
      }
      
      
      
	}
	
	if(iFlag){
	    string  sTCommnd;
      
      sTCommnd+="nohup ";
      sTCommnd+=sModelProc;
      sTCommnd+=" -p";
      sTCommnd+=iProcessId;
      sTCommnd+=" ";
      sTCommnd+=sParammeter;
      sTCommnd+=" ";
      sTCommnd+=" -S";
      sTCommnd+=sVodID;
      
	    sprintf (sCommand, sTCommnd.c_str());
      
	    fp = popen (sCommand, "r");
	    if (fp == NULL)
	    {
	    	mvaddstr(8,(COLS_LENS - strlen((char *)"调用系统命令失败"))/2,(char *)"调用系统命令失败");
	    	return -1;
	    }
	    while(!feof(fp))
	    {

			     break;

	    }
	    fclose(fp);
	    return 0;
	}
	else{
	 return 2	
	}
   
		
}

/*
   功能：启动进程
*/
int ABMProcess::RunProcess(int iProcessId)
{
	cout <<"..检查启动PROCESSID是否已经存在,PROCESSID:"<<iProcessId<<endl;
	if(RunProcess(int iProcessId)==0){
		 cout <<"..目前没有启动,PROCESSID:"<<iProcessId<<endl;
		 cout <<"..开始启动进程:"<<iProcessId<<endl;	
	}else{
		cout <<"..启动进程已经存在:"<<iProcessId<<endl;
		return 0;
	}
	
	if(StartProcess(int iProcessId)==0){
		if(RunProcess(int iProcessId)==0){
		    cout <<"..启动进程未成功,PROCESSID:"<<iProcessId<<endl;
		    
	  }else{
		   cout <<"..启动进程成功:"<<iProcessId<<endl;
	  }
	}else{
		cout <<"..启动进程未成功:"<<iProcessId<<endl;		
	}
	

   return 0;
}


int ABMProcess::StopProcess(int iProcessId)
{
	FILE *fp = 0;
	string  sTCommnd;
	char sCommand[256] = {0};
	cout <<"..检查启动PROCESSID是否已经存在,PROCESSID:"<<iProcessId<<endl;
	if(RunProcess(int iProcessId)==0){
		 cout <<"..目前没有启动,PROCESSID:"<<iProcessId<<endl;
		 return 0;
		 //cout <<"..开始启动进程:"<<iProcessId<<endl;	
	}else{
	 sTCommnd+="ps -ef|grep "
	 sTCommnd+=iProcessId
	 sTCommnd+="grep -v grep|grep petri| awk '{print $2";
   sTCommnd+="}'";
   sTCommnd+="|xargs kill"
	
		sprintf (sCommand, sTCommnd.c_str());

	fp = popen (sCommand, "r");
	if (fp == NULL)
	{
		return ;
	}
	//---------------------------------------------------------

  //sprintf(sBuff[0],"%s","登录用户：登录方式：IP：日期：开始时间：结束时间：持续时间"); 
	int n = 0;
	
	while(!feof(fp))
	{
		break;
	}
	fclose(fp);
	
	if(RunProcess(int iProcessId)==0){
		    cout <<"进程已经停止"<<iProcessId<<endl;
		    
	  }else{
		   cout <<"进程停止未成功"<<iProcessId<<endl;
	  }
		return 0;
	}
	
}




