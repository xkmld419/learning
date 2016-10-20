#include "Hbinstalltest.h"
#include "ProcessInfo.h"
#include "process.h"
#include <string.h>

Hbinstalltest::Hbinstalltest()
{
	  memset(GATHER_PART_ROOT,0,sizeof(GATHER_PART_ROOT));
		memset(GATHER_PART_NODE,0,sizeof(GATHER_PART_NODE));
		memset(TEST_LIST,0,sizeof(TEST_LIST));
		memset(LIST_NAME,0,sizeof(LIST_NAME));
		memset(SQL_USERLOAD,0,sizeof(SQL_USERLOAD));
		memset(SQL_BILLFLOWCFG,0,sizeof(SQL_BILLFLOWCFG));
		memset(GATHER_PART_NODE2,0,sizeof(GATHER_PART_NODE2));
		memset(DUPPATH,0,sizeof(DUPPATH));
}

Hbinstalltest::~Hbinstalltest()
{
	
}

int Hbinstalltest::getFullPath(const char * str1,const char * str2,const char * str3,char * pp,const char * scat,int pro)
{
	char *ptr = NULL;
	char pwdbuf[1024] = {0};
	
	if (NULL == getcwd(pwdbuf, sizeof(pwdbuf)))
	{
		Log::log(0,"取不到当前程序运行的路径");
		return -1;
	}
	
	ptr = RD.readIniString((char *)str1,(char *)str2,(char *)str3,pp,NULL);
	if (  ptr == NULL || strlen(pp) == 0)
	{
		sprintf(pp,"%s/%s",pwdbuf,(char *)scat);
	}
	
	if (pro !=0)
	{
		if(access(pp, F_OK))
		{
			Log::log(0,"取不到 %s 路径",pp);
			return -1;
		}
	}
	
	if(strlen(pp) == 0)
	{
		Log::log(0,"取出字段是空值 %s",str3);
		return -1;
	}
		
	return 0;
}
int Hbinstalltest::Init()
{
	int ret = -1;
	
	char * pPath =  getenv("BILLDIR");
	char   petcPath[1024] = {0};
	if(NULL == pPath)
	{
		Log::log(0,"取不到系统 BILLDIR 环境变量");
		return -1;
	}
	else
	{
		sprintf(petcPath,"%s/%s",pPath,"etc/hbtest");
	}
	
	ret = getFullPath(petcPath,"BILL_FILE","GATHER_PART_NODE",GATHER_PART_NODE,"data/yzy_src");
	if(ret != 0) return -1;
	ret = getFullPath(petcPath,"BILL_FILE","GATHER_PART_NODE2",GATHER_PART_NODE2,"data/yzy_src");
	if(ret != 0) return -1;
	ret = getFullPath(petcPath,"BILL_FILE","DUPPATH",DUPPATH,"data/dupdata");
	if(ret != 0) return -1;
	ret = getFullPath(petcPath,"BILL_FILE","DBPATH",DBPATH,"data/filedb");
	if(ret != 0) return -1;
	memset(TEST_LIST,0,sizeof(TEST_LIST));
	ret = getFullPath(petcPath,"BILL_FILE","TEST_LIST",TEST_LIST,"");
	if(ret != 0) return -1;
	ret = getFullPath(petcPath,"BILL_FILE","LIST_NAME",LIST_NAME,"");
	if(ret != 0) return -1;
	
	ret = getFullPath(petcPath,"OTHER_PATH","SQL_USERLOAD",SQL_USERLOAD,"sql/test2_loaduserinfo.sql");
	if(ret != 0) return -1;
	ret = getFullPath(petcPath,"OTHER_PATH","SQL_BILLFLOWCFG",SQL_BILLFLOWCFG,"sql/test2_billflow_cfg.sql");
	if(ret != 0) return -1;
	ret = getFullPath(petcPath,"OTHER_PATH","SET",SET,"shell/set.sh");
	if(ret != 0) return -1;
	
	/*
	ptr = RD.readIniString(petcPath,"OTHER_PATH","SQL_USERLOAD",SQL_USERLOAD,NULL);
	if ( access(SQL_USERLOAD, F_OK) || ptr == NULL || strlen(SQL_USERLOAD) == 0)
	{
		Log::log(0,"取不到 SQL_USERLOAD 路径 ,请看 hbtest 是否配置");
		return -1;
	}
	
	ptr = RD.readIniString(petcPath,"OTHER_PATH","SQL_BILLFLOWCFG",SQL_BILLFLOWCFG,NULL);
	if ( access(SQL_BILLFLOWCFG, F_OK) || ptr == NULL || strlen(SQL_BILLFLOWCFG) == 0)
	{
		Log::log(0,"取不到 SQL_BILLFLOWCFG 路径 ,请看 hbtest 是否配置");
		return -1;
	}
	
	ptr = RD.readIniString(petcPath,"OTHER_PATH","SET",SET,NULL);
	if ( access(SET, F_OK) || ptr == NULL || strlen(SET) == 0)
	{
		Log::log(0,"取不到 SET 路径 ,请看 hbtest 是否配置");
		return -1;
	}
	
	ptr = RD.readIniString(petcPath,"OTHER_PATH","BILLFLOWRUN",BILLFLOWRUN,NULL);
	if ( access(BILLFLOWRUN, F_OK) || ptr == NULL || strlen(BILLFLOWRUN) == 0)
	{
		Log::log(0,"取不到 BILLFLOWRUN 路径 ,请看 hbtest 是否配置");
		return -1;
	}
	
	ptr = RD.readIniString(petcPath,"OTHER_PATH","BILLFLOWDOWN",BILLFLOWDOWN,NULL);
	if ( access(BILLFLOWDOWN, F_OK) || ptr == NULL || strlen(BILLFLOWDOWN) == 0)
	{
		Log::log(0,"取不到 BILLFLOWDOWN 路径 ,请看 hbtest 是否配置");
		return -1;
	}
	*/
	return ret;
}

/*获取环境变量*/
int Hbinstalltest::getenviroment()
{
	char *p = NULL;
	char *u = NULL;
	struct hostent*     lpHostEnt = NULL;
	struct sockaddr_in addr;
	char _sHostName[200] = {0};
  
	//取路径
	if((p=getenv("BILLDIR")) == NULL)
	{	
		Log::log(0,"取不到系统路径");
		return -1;
	}
	else
		sprintf(h_path,"%s",p);
		
	if(access(h_path, F_OK))
	{	
		Log::log(0,"系统路径,BILLDIR 不存在 ");
		return -1;
	}
		
	/*
	char Temp[300] = {0};
	sprintf(Temp,"%s%s",h_path,"/bin/hbprocess");
	if(access(Temp, F_OK))
	{	
		Log::log(0,"系统路径,TIBS_HOME/bin 不存在 hbprocess 可执行程序");
		return -1;
	}	
	*/
	//取用户
	if((u=getenv("LOGNAME")) == NULL)
	{
		Log::log(0,"取不到系统用户");
		return -1;
	}
	else
		sprintf(h_user,"%s",u);
		
    //获取主机name
    int iret = gethostname(_sHostName,100);
    
    if (iret) 
    {
        Log::log(0,"取不到主机名");
        return -1;
    }
    strcpy(h_hostname,_sHostName);
    //获取主机ip
    if ( !( lpHostEnt = gethostbyname( _sHostName ) ) )
        {
             Log::log(0,"取不到lpHostEnt");
             return -1;
        }
    addr.sin_addr = *( (struct in_addr*) *lpHostEnt->h_addr_list );//ip地址赋值
    inet_ntop(AF_INET, &addr.sin_addr,h_hostip, 16);  //地址转化
    
    return 0;
}

/*测试话单生成*/
bool Hbinstalltest::writeTestList()
{
	DIR *dp = NULL;	
	FILE *fp = NULL;
	char p[ALLPATH] = {0};
	char w_path_root[ALLPATH] ={0};
	char w_path_node[ALLPATH] ={0};
	char w_file_list[ALLPATH] ={0};
	
	//strncpy(w_path_root,h_path,strlen(h_path)+1);
	strncpy(w_path_root,GATHER_PART_ROOT,sizeof(w_path_root));
	//strncpy(w_path_node,h_path,strlen(h_path)+1);
	strncpy(w_path_node,GATHER_PART_NODE,sizeof(w_path_node));
	//确定写测试话单的路径
	if((dp = opendir(w_path_node)) == NULL)
		{
			if(mkdir(w_path_node,0770) != 0)
				{
					if(mkdir(w_path_root,0770) == 0 && mkdir(w_path_node,0770) == 0);
					else{
						char path[100];
						strcpy(path,w_path_node);
						Log::log(0,"存放测试话单目录%s创建失败！",path);
						THROW( MBC_Directory );
						return false;
					}
				}	
		}
	if(dp != NULL)
		closedir(dp);
		
	//写入测试话单
	strncpy(p,TEST_LIST,sizeof(p));
	strcpy(w_file_list,w_path_node);
	if(w_file_list[strlen(w_file_list)-1] != '/')
		strcat(w_file_list,"/");
	strcat(w_file_list,LIST_NAME);
	fp = fopen(w_file_list,"w+");
	if(fp == NULL)
		return false;
	else{
		fwrite(p,strlen(p),1,fp);
		fclose(fp);
	}
	return true;
}

/*测试用户及订购资料导入*/
bool Hbinstalltest::createInfo()
{
	FILE *fp = NULL;
	char sLine[2048] = {0};
	char sSql[2048] = {0};
	char userLodePath[100] ={0};
	
	//strcpy(userLodePath,h_path);
	strncpy(userLodePath,SQL_USERLOAD,sizeof(userLodePath));
	DEFINE_QUERY (qry);
	fp = fopen(userLodePath,"r");					//打开测试用三户和订购资料文件
	if(fp == NULL){
		Log::log(0,"三户及订购资料信息文件%s打开失败！",userLodePath);
		return false;	
	}else{
			while(fgets(sLine,2048,fp) != NULL){
				if (('\0' == sLine[0]) || ('#' == sLine[0]) || ('-' == sLine[0]) || ('\n' == sLine[0])) //忽略空行和注释行
					continue;
				try
				{
					if(sLine[strlen(sLine)-2] == ';')
						sLine[strlen(sLine)-2] = '\n';
					else if(sLine[strlen(sLine)-3] == ';')
						sLine[strlen(sLine)-3] = '\n';
					strcpy(sSql,sLine);
					qry.setSQL(sSql);
					qry.execute();
					qry.close();
					continue;
				}catch(TOCIException & e){
					Log::log(0,"三户及订购资料上载失败!");
					Log::log(0,e.getErrMsg());
					Log::log(0,e.getErrSrc());
					throw e;
					return false;
				}
			}
		qry.commit();
		qry.close();
		
	return true;
	}
}

/*显示测试结果*/
bool Hbinstalltest::printTestinfo()
{	
	char m_serv_id[100] = {0};
	char m_acct_id[100] = {0};
	char m_billing_cycle_id[100] = {0};
	char m_charge[100] = {0};
	
	char m_event_type_id[100] = {0};
	char m_billing_nbr[100]={0};
	char m_billing_org_id[100]={0};
	char m_calling_area_code[100]={0};
	char m_calling_nbr[100]={0};
	char m_called_nbr[100]={0};

	DEFINE_QUERY (qry);
	qry.close();
	char sSql[1024]={0};
	sprintf(sSql,"select serv_id,acct_id,billing_cycle_id,charge from acct_item_aggr_test");
	qry.setSQL(sSql);
	qry.open();
	
	if(qry.next())
	{
		//m_serv_id = qry.field(0).asString();
		strcpy(m_serv_id,qry.field(0).asString());
		//m_acct_id = qry.field(1).asString();
		strcpy(m_acct_id,qry.field(1).asString());
		//m_billing_cycle_id = qry.field(2).asString();
		strcpy(m_billing_cycle_id,qry.field(2).asString());
		//m_charge = qry.field(3).asString();
		strcpy(m_charge,qry.field(3).asString());
	}
	else
	{
		Log::log(0,"总账表中无数据");
		return false;
	}
	qry.close();
	
	memset(sSql,0,sizeof(sSql));
	sprintf(sSql,"select event_type_id,billing_nbr,billing_org_id,calling_area_code,calling_nbr,called_nbr from call_event_test");
	qry.setSQL(sSql);
	qry.open();
	if(qry.next())
	{
		//m_event_type_id = qry.field(0).asString();
		strcpy(m_event_type_id,qry.field(0).asString());
		strcpy(m_billing_nbr,qry.field(1).asString());
		//m_billing_org_id = qry.field(2).asString();
		strcpy(m_billing_org_id,qry.field(2).asString());
		strcpy(m_calling_area_code,qry.field(3).asString());
		strcpy(m_calling_nbr,qry.field(4).asString());
		strcpy(m_called_nbr,qry.field(5).asString());
	}
	else
	{
		Log::log(0,"语音清单表中无数据");
		return false;
	}
	
	qry.close();
		printf("\n总账表信息：\n");
		printf("用户 : %s , 账户 : %s , 账期 : %s , 费用：%s\n",m_serv_id,m_acct_id,m_billing_cycle_id,m_charge);
		printf("\n清单表信息：\n");
		printf("事件类型 : %s, 计费号码 : %s, 计费org : %s, 主叫区号 : %s, 主叫号码 : %s, 被叫号码 : %s\n ",
		m_event_type_id,m_billing_nbr,m_billing_org_id,m_calling_area_code,m_calling_nbr,m_called_nbr);
		printf("\n实际产生话费 : %s分\n",m_charge);
		return true;
}

/*进程启停状态判断*/
int Hbinstalltest::processState(int m_process)
{
	THeadInfo *pInfoHead = NULL;
  	TProcessInfo *pProcInfoHead = NULL;
  	
  	if((AttachShm(0,(void **)&pInfoHead))<0)  //联入共享内存，取进程信息
   {
   	Log::log(0,"联入监控共享缓冲区不存在");
   	return -1;
   }
   pProcInfoHead = &(pInfoHead->ProcInfo);
   for(int i=0; i<pInfoHead->iProcNum;i++)
  {
  	 if(pProcInfoHead[i].iProcessID != m_process);
  	 else
  	 	{
  	 		while(true)
  	 		{
		  	 	if(pProcInfoHead[i].iState == 2 || \
		  	 		 pProcInfoHead[i].iState == 8)
		  		{
		  	 		Log::log(0,"进程%d启动成功",m_process);
		  	 		return 0;
	  	 		}
	  	 		else if(pProcInfoHead[i].iState == 6 || \
	  	 			      pProcInfoHead[i].iState == 5  || \
	  	 			      pProcInfoHead[i].iState == 0  || \
	  	 			      pProcInfoHead[i].iState == 7)
	  	 		{
	  	 			Log::log(0,"进程%d未能启动,请检查log目录下process_%d.log",m_process,m_process);
	  	 			return -1;
	  	 	  }	
	  	 		else
	  	 		{
	  	 			sleep (2);
	  	 			continue;
	  	 		}
	  	 	}
  	 }
  }
  
  Log::log(0,"没有找到要检查的进程 %d",m_process);
  return -1;
}

/*测试流程配置*/
bool Hbinstalltest::billflowCfg()
{
	FILE *fp = NULL;
	char sLine[2048] = {0};
	char sSql[2048] = {0};
	char billflowCfgPath[100] = {0};

	DEFINE_QUERY (qry);
	//strcpy(billflowCfgPath,h_path);
	strncpy(billflowCfgPath,SQL_BILLFLOWCFG,sizeof(billflowCfgPath));
	fp = fopen(billflowCfgPath,"r");
	if(fp == NULL){
		Log::log(0,"测试流程配置文件%s打开失败！",billflowCfgPath);
		return false;	
	}else{
			while(fgets(sLine,2048,fp) != NULL){
				if (('\0' == sLine[0]) || ('#' == sLine[0]) || ('-' == sLine[0]) || ('\n' == sLine[0])) //忽略空行和注释行
					continue;
				try
				{
					if(sLine[strlen(sLine)-2] == ';')
						sLine[strlen(sLine)-2] = '\n';
					strcpy(sSql,sLine);
					qry.setSQL(sSql);
					qry.execute();
					qry.close();
					continue;
				}catch(TOCIException & e){
					Log::log(0,"测试流程配置出错!");
					Log::log(0,e.getErrMsg());
					Log::log(0,e.getErrSrc());
					throw e;
					return false;
				}
			}
		qry.commit();
		qry.close();
		return true;
	}
}

/*相关表字段检查和主机信息同步
*由于各个主机(主机名,主机IP,主机登录名,起始路径)的不同，所以当表插入
*完成后检查特定表的字段中值是否对应现有的主机,若不对应则UPDATE
*/
void Hbinstalltest::checkTableByInfo()
{
	DEFINE_QUERY (qry);
	char sSql[1024]={0};
	char c_path[100]={0};
	char c_despath[100]={0};
	char c_srcpath[100]={0};
	char c_duppath[100]={0};
	char c_dbpath[100]={0};
	char c_hostname[100] = {0};
	int  ihostid = -1;
	char c_EventfileName[1024] = {0};
	
	if(gethostname(c_hostname,100) !=0)
	{
		Log::log(0,"gethostname is err");
		throw -1;
	}
	
	qry.close();
	//更新wf_hostinfo
	sprintf(sSql,"select host_id from wf_hostinfo where host_name='%s'",c_hostname);
	qry.setSQL(sSql);
	qry.open();
	if(qry.next())
	{
		ihostid = qry.field(0).asInteger();
	}
	else
	{	
		Log::log(0,"%s,error",sSql);
		throw -1;
	}
	qry.close();
	
	memset(sSql,0,sizeof(sSql));
	//更新b_gather_task
	sprintf(sSql,"update b_gather_task set SRC_PATH = :SRC_PATH,DES_PATH = :DES_PATH,HOST_ID = :HOST_ID where task_id=3");
	qry.setSQL(sSql);
	strncpy(c_srcpath,GATHER_PART_NODE,sizeof(c_srcpath));
	strncpy(c_despath,GATHER_PART_NODE2,sizeof(c_despath));
	qry.setParameter("SRC_PATH",c_srcpath);
	qry.setParameter("DES_PATH",c_despath);
	qry.setParameter("HOST_ID",ihostid);
	qry.execute();
	qry.close();
	
	memset(sSql,0,sizeof(sSql));
	//更新 b_event_file_list
	sprintf(sSql,"update b_event_file_list set FILE_NAME = :FILE_NAME where FILE_ID=9999");
	qry.setSQL(sSql);
	strncpy(c_srcpath,GATHER_PART_NODE,sizeof(c_srcpath));
	strncpy(c_despath,GATHER_PART_NODE2,sizeof(c_despath));
	sprintf(c_EventfileName,"%s/%s",c_despath,LIST_NAME);
	qry.setParameter("FILE_NAME",c_EventfileName);
	qry.execute();
	qry.close();
	
	memset(sSql,0,sizeof(sSql));
	//更新wf_process
	sprintf(sSql,"update wf_process set EXEC_PATH = :EXEC_PATH,SYS_USERNAME = :SYS_USERNAME ,HOST_ID = :HOST_ID where billflow_id=10");
	qry.setSQL(sSql);
	strcpy(c_path,h_path);
	strcat(c_path,"/bin");
	qry.setParameter("EXEC_PATH",c_path);
	qry.setParameter("SYS_USERNAME",h_user);
	qry.setParameter("HOST_ID",ihostid);
	qry.execute();
	qry.close();
  
  memset(sSql,0,sizeof(sSql));
	//更新b_dup_data_path
	sprintf(sSql,"update b_dup_data_path set DATA_PATH = :DATA_PATH where process_id=54");
	qry.setSQL(sSql);
	
	strncpy(c_duppath,DUPPATH,sizeof(c_duppath));
	qry.setParameter("DATA_PATH",c_duppath);
	qry.execute();
	
	qry.commit();
	qry.close();
	
	//更新 B_IPC_CFG
	memset(sSql,0,sizeof(sSql));
	sprintf(sSql,"update B_IPC_CFG set HOST_ID = :HOST_ID,SYS_USERNAME = :SYS_USERNAME  where billflow_id=10");
	qry.setSQL(sSql);
	qry.setParameter("SYS_USERNAME",h_user);
	qry.setParameter("HOST_ID",ihostid);
	qry.execute();
	qry.close();
	
}
//根据processId 停止启动进程
int Hbinstalltest::stopPro(int iProcessId)
{
	 if(iProcessId <= 0) return -1;
	 	
	 THeadInfo *pInfoHead = NULL;
	 TProcessInfo *pProcInfoHead = NULL;
	 
   if((AttachShm(0,(void **)&pInfoHead))<0)  //联入共享内存，取进程信息
   {
   	Log::log(0,"联入监控共享缓冲区不存在");
   	return -1;
   }
   
   pProcInfoHead = &(pInfoHead->ProcInfo);
   
   if(pProcInfoHead == NULL) return -1;
   
   for (int i=0; i<pInfoHead->iProcNum; i++) 
   {
   	
   	if(pProcInfoHead[i].iProcessID==iProcessId)
   	 {
   	 	 if (pProcInfoHead[i].iState==ST_RUNNING || pProcInfoHead[i].iState==ST_INIT)
   	 	 {
		      pProcInfoHead[i].iState = ST_WAIT_DOWN;
	        Log::log(0,"成功致标志位,进程 process_id=%d 停止!!",iProcessId);
	        return 0;
	     		
		   }
	   	 
	   }
	   
	   continue;
	   
	}
	
  return 0;	
}


//根据processId 启动进程
int Hbinstalltest::beginRunPro(int iProcessId)
{
	 if(iProcessId <= 0) return -1;
	 	
	 THeadInfo *pInfoHead = NULL;
	 TProcessInfo *pProcInfoHead = NULL;
	 
   if((AttachShm(0,(void **)&pInfoHead))<0)  //联入共享内存，取进程信息
   {
   	Log::log(0,"联入监控共享缓冲区不存在");
   	return -1;
   }
   
   pProcInfoHead = &(pInfoHead->ProcInfo);
   
   if(pProcInfoHead == NULL) return -1;
   
   for (int i=0; i<pInfoHead->iProcNum; i++) 
   {
   	
   	if(pProcInfoHead[i].iProcessID==iProcessId)
   	 {
   	 	  int iSysPID = pProcInfoHead[i].iSysPID;
   	 	  if(iSysPID != 0)
   	 	  {
   	 	  	if(kill(iSysPID,0) == 0)
   	 	    {
   	 	    	Log::log(0,"需要启动的process_id=%d对应的程序已经启动，请检查！",iProcessId);
   	 	    	return 0;
   	 	    }
   	 	  }
   	 	  
	   	 if (pProcInfoHead[i].iState != ST_RUNNING && \
	            pProcInfoHead[i].iState != ST_INIT) 
	   	 {
	        pProcInfoHead[i].iState = ST_WAIT_BOOT;
	        Log::log(0,"成功致标志位,进程 process_id=%d 启动!!",iProcessId);
	        return 0;
	     }
	   }
	   
	   continue;
	   
	}
	
	Log::log(0,"共享内存找不到需要启动的 process_id=%d 进程 %d",iProcessId);
  return -1;	
}
