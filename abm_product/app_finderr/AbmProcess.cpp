#include "AbmProcess.h"
//#include "mntapi.h"
#define ABMPROCONFIG "etc/configtest.ini"

/***
flag为false时，表示初始化检查
flag为true时，表示为进程启动检查

***/
ABMException oExp;
char HomeEnv[128] = {0};
char *_pUser = getenv("USER");

int findENV()
{
	char *penv = NULL;
    char HomeEnv[128] = {0};
    if ((penv=getenv("ABM_PRO_DIR")) == NULL) {
        ADD_EXCEPT0(oExp, "环境变量\"HSSCONFIGDIR\"没有设置, GetConnectInfo 返回错误");
        return -1;
    }
    strncpy(HomeEnv, penv, sizeof(HomeEnv)-2);
    if (HomeEnv[strlen(HomeEnv)-1] != '/') {
        HomeEnv[strlen(HomeEnv)] = '/';
    }
	
	}



void itoaX (int n,char s[])
{ int i,j,sign;
if((sign=n)<0)//记录符号 
n=-n;//使n成为正数    
i=0; 
do
{ 
s[i++]=n%10+'0';//取下一个数字 
}
while ((n/=10)>0);//删除该数字
if(sign<0) 
s[i++]='-'; 
s[i]='\0'; 
for(j=i;j>=0;j--)//生成的数字是逆序的，所以要逆序输出 
printf("%c",s[j]);
}

int ABMProcess::CheckProcess(int iProcessId)
{
	findENV();
	char sCommand[256] = {0};
	char *sAudit=NULL;
	char *sAuditT=NULL;
	char sBuff[1][128];
	long numProcess=0;
	FILE *fp = 0;
  
	   sprintf (sCommand, "ps -ef|grep '%d'|grep -v grep|grep -v abmprocess|wc -l",iProcessId);
	   fp = popen (sCommand, "r");
	   if (fp == NULL)
	   {
	   	return -1;
	   }
	   
	   while(!feof(fp))
	   {
	   	if( !fgets(sBuff[1],128,fp) )
	   	{	break;}
	   }
	   fclose(fp);
	   numProcess= strtol(sBuff[1], &sAuditT, 10);
	   if(numProcess>0){
	      
	      return 2;
	   }
	   return 0;
		
	   return 0;
		
}


int ABMProcess::StartProcess(int iProcessId)
{
	findENV();
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
	  int TableCol = 0;
    char path[1024];
	  int checkI=0;
	  int checkJ=0;
	  snprintf(path, sizeof(path), "%s%s", HomeEnv,ABMPROCONFIG);
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
	    	//mvaddstr(8,(COLS_LENS - strlen((char *)"调用系统命令失败"))/2,(char *)"调用系统命令失败");
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
	 return 2	;
	}
   
		
}

/*
   功能：启动进程
*/
int ABMProcess::RunProcess(int iProcessId)
{
	findENV();
	cout <<"..检查启动PROCESSID是否已经存在,PROCESSID:"<<iProcessId<<endl;
	if(CheckProcess(iProcessId)==0){
		 cout <<"..目前没有启动,PROCESSID:"<<iProcessId<<endl;
		 cout <<"..开始启动进程:"<<iProcessId<<endl;	
	}else{
		cout <<"..启动进程已经存在:"<<iProcessId<<endl;
		return 0;
	}
	
	if(StartProcess(iProcessId)==0){
		if(RunProcess(iProcessId)==0){
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
	findENV();
	FILE *fp = 0;
	string  sTCommnd;
	char sCommand[256] = {0};
	char s[8];
	cout <<"..检查启动PROCESSID是否已经存在,PROCESSID:"<<iProcessId<<endl;
	if(CheckProcess(iProcessId)==0){
		 cout <<"..目前没有启动,PROCESSID:"<<iProcessId<<endl;
		 return 0;
		 //cout <<"..开始启动进程:"<<iProcessId<<endl;	
	}else{
		/**
		itoaX(iProcessId,s);
	 sTCommnd+="ps -ef|grep ";
	 sTCommnd+=s;
	 sTCommnd+="|grep -v grep|grep ";
	 sTCommnd+=_pUser;
	 sTCommnd+="| awk '{print $2";
   sTCommnd+="}'";
   sTCommnd+="|xargs kill";
	
		sprintf (sCommand, sTCommnd.c_str());
**/
sprintf (sCommand, "ps -ef|grep %d|grep -v grep|grep -v abmprocess|grep %s| awk '{print $2}'|xargs kill",iProcessId,_pUser);


	fp = popen (sCommand, "r");
	if (fp == NULL)
	{
		return -1;
	}
	//---------------------------------------------------------

  //sprintf(sBuff[0],"%s","登录用户：登录方式：IP：日期：开始时间：结束时间：持续时间"); 
	int n = 0;
	
	while(!feof(fp))
	{
		break;
	}
	fclose(fp);
	
	if(RunProcess(iProcessId)==0){
		    cout <<"进程已经停止"<<iProcessId<<endl;
		    
	  }else{
		   cout <<"进程停止未成功"<<iProcessId<<endl;
	  }
		return 0;
	}
	
}





int ABMProcess::ShowProcess(int iProcessId)
{
	findENV();
	FILE *fp = 0;
	int lv_iTotalRow = 0;
	string  sTCommnd;
	char sCommand[256] = {0};
	char sBuff[1000][128];
	
	cout <<"..检查启动PROCESSID是否已经存在,PROCESSID:"<<iProcessId<<endl;
	if(CheckProcess(iProcessId)==0){
		 cout <<"..目前没有启动,PROCESSID:"<<iProcessId<<endl;
		 return 0;
		 //cout <<"..开始启动进程:"<<iProcessId<<endl;	
	}else{
		/**
	 sTCommnd+="ps -ef|grep ";
	 sTCommnd+=iProcessId;
	 sTCommnd+="|grep -v grep|grep -v abmprocess|grep ";
	 sTCommnd+=_pUser;
	 
	
		sprintf (sCommand, sTCommnd.c_str());
		***/
		
		sprintf (sCommand, "ps -ef|grep %d|grep -v grep|grep -v abmprocess|grep %s",iProcessId,_pUser);

	fp = popen (sCommand, "r");
	if (fp == NULL)
	{
		return -1;
	}
	//---------------------------------------------------------

  //sprintf(sBuff[0],"%s","登录用户：登录方式：IP：日期：开始时间：结束时间：持续时间"); 
	int n = 0;
	
	while(!feof(fp))
	{
		if( !fgets(sBuff[n],128,fp) )
			break;
		n++;
		lv_iTotalRow ++;
	}
	fclose(fp);
	
	if(lv_iTotalRow==0){
		//mvaddstr(8,(COLS_LENS - strlen((char *)"无符合条件的选项"))/2,(char *)"无符合条件的选项");
		cout <<"无符合条件的选项，PROCESS_ID："<<iProcessId<<endl;
		return 0;
	}else{
		for(n=0;n<lv_iTotalRow;n++){
			cout<<sBuff[n]<<endl;
			n++;
		}
		 
		}
	
		return 0;
	}
	
}



int ABMProcess::ShowProcessInfoByApp(char *sAppName)
{
	findENV();
	FILE *fp = 0;
	int lv_iTotalRow = 0;
	string  sTCommnd;
	char sCommand[256] = {0};
	char sBuff[1000][128];
	cout <<"..检查启动PROCESSID是否已经存在,PROCESSID:"<<iProcessId<<endl;
/***
	 sTCommnd+="ps -ef|grep ";
	 sTCommnd+=sAppName;
	 sTCommnd+="grep -v grep|grep ";
	 sTCommnd+=_pUser;
	 
	
		sprintf (sCommand, sTCommnd.c_str());
		**/
		sprintf (sCommand, "ps -ef|grep %s|grep -v grep|grep -v abmprocess",sAppName);

	fp = popen (sCommand, "r");
	if (fp == NULL)
	{
		return -1;
	}
	//---------------------------------------------------------

  //sprintf(sBuff[0],"%s","登录用户：登录方式：IP：日期：开始时间：结束时间：持续时间"); 
	int n = 0;
	
	while(!feof(fp))
	{
		if( !fgets(sBuff[n],128,fp) )
			break;
		n++;
		lv_iTotalRow ++;
	}
	fclose(fp);
	
	if(lv_iTotalRow==0){
		//mvaddstr(8,(COLS_LENS - strlen((char *)"无符合条件的选项"))/2,(char *)"无符合条件的选项");
		cout <<"无符合条件的选项，PROCESS_ID："<<iProcessId<<endl;
		return 0;
	}else{
		for(n=0;n<lv_iTotalRow;n++){
			cout<<sBuff[n]<<endl;
			n++;
		}
		 
		}
	
		return 0;
	
}

ABMProcess::ABMProcess()
{
	
}

ABMProcess::~ABMProcess()
{
	
}
