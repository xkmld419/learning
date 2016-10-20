#include "abmclearfile.h"

ClearFile::ClearFile() 
{
	getTaskPath() ;
	getErroeFilePath() ;
	getMergeFilePath() ;
	getWdbHisPath() ;
	getAsnBackupPath() ;
	getRSexbakPath() ;
}

ClearFile::~ClearFile()
{
}

int ClearFile::checkPermission(const char *filePath) 
{
	struct stat buf ;

	stat(filePath,&buf) ;
	if(buf.st_uid == geteuid()) {
		chmod(filePath,S_IWUSR|S_IRUSR|S_IXUSR) ;
		return 1 ;
	}
	if(buf.st_gid == getegid()) {
		if(access(filePath,W_OK)<0) {
			Log::log(0,"%s permission denied",filePath) ;
			return 0 ;
		}
		return 1 ;
	}
	if(access(filePath,W_OK)<0) {
			Log::log(0,"%s permission denied",filePath) ;
			return 0 ;
	}
	return 1 ;
}

char * ClearFile::CFtrim( char *sp )
{
	char sDest[200]={0};
	char *pStr;
	int i = 0;

	if ( sp == NULL )
		return NULL;

	pStr = sp;
	while ( *pStr == ' ' || *pStr == '\t' )  pStr ++;
	strcpy( sDest, pStr );

	i = strlen( sDest ) - 1;
	while((i>=0)&&(sDest[i] == ' ' || sDest[i] == '\t' || sDest[i] == '\r'|| sDest[i] == '\n' ) )
	{
		sDest[i] = '\0';
		i -- ;
	}

	strcpy( sp, sDest );

	return ( sp );
}

void ClearFile::setEndTime(char *t)  {
	strcpy(endtime,t) ;
}

bool ClearFile::getTaskPath()
{
	char sql[200]={0} ;
	
	DEFINE_QUERY(qry) ;
	sprintf(sql,"select distinct des_path from b_gather_task") ;
	qry.setSQL(sql) ;
	qry.open() ;
	while(qry.next())  {
		m_gTaskPath.push_back(string(qry.field(0).asString())) ;
	}
	qry.close() ;
	return 1 ;
}

int ClearFile::clearGaherTaskFile()
{
	char tName[100]={0} ;
	//DIR *dp ;
	FILE *fd ;
	string tStr ;
	int num = 0 ;
	struct tm *ptr ;
	char sTmp[500]={0};
	char sTime[60]={0};
	char sCmd[500]={0};
	struct stat statbuf;
	//struct dirent *dirp ;
	vector<string>::iterator iter = m_gTaskPath.begin() ;
	for(;iter!=m_gTaskPath.end();iter++)  {
		sprintf(sTmp,"%s",iter->c_str());
		fd = popen((string("ls ")+*iter).c_str(),"r") ;
		while(fgets(tName,100,fd))  
		{
			tStr.clear() ;
			tStr = *iter ;
			CFtrim(tName) ;
			if(strcmp(tName,endtime)<0)   
			{
				tStr.append("/") ;
				tStr.append(tName) ;
				//printf("%s\n",(string("rm -r ")+tStr).c_str());
				system((string("rm -r ")+tStr).c_str()) ;
				memset(tName,0,sizeof(tName));
				num++ ;
		  	}
		  	if(tName[0] == '0')
		  		continue;
		  	if(sTmp[strlen(sTmp)-1]=='/')
		  		sprintf(sTmp,"%s%s",iter->c_str(),tName);
		  	else
		  		sprintf(sTmp,"%s/%s",iter->c_str(),tName);
			if(lstat(sTmp, &statbuf)<0|| S_ISREG(statbuf.st_mode)==0)
			{
			      continue;
			}
	    		ptr = localtime(&statbuf.st_mtime) ;
			strftime(sTime,50,"%Y%m%d",ptr) ;
			if(strcmp(sTime,endtime)<0) 
			{
				snprintf(sCmd,500,"rm %s",sTmp) ;
				system(sCmd) ;
				num++ ;
			}		  	
		}
	}
	return num ;
}

bool ClearFile::getErroeFilePath()
{
	if (!ParamDefineMgr::getParam ("CHECK_FILE","REJECT_FILE_PATH",m_cErrorFilePath))
      m_cErrorFilePath[0] = 0;
  CFtrim(m_cErrorFilePath) ;
  return true ;
}

int ClearFile::clearCheckErrorFile()
{
	int num = 0 ;
	if( m_cErrorFilePath[0] == '\0')
	{
    Log::log(0, "校验错误文件目录没有配置.程序退出...\n");
    return 0;      
  }
  if(strlen(m_cErrorFilePath)>0 && m_cErrorFilePath[strlen(m_cErrorFilePath)-1]=='/')
     m_cErrorFilePath[strlen(m_cErrorFilePath)-1]='\0';
     
	DIR *dirp=NULL;
	struct tm *ptr ;
  struct dirent *dp=NULL;
  char sName[500]={0};
  char sTmp[500]={0};
  char sTime[500]={0};
  char sCmd[500]={0};
  struct stat statbuf;  
  
  dirp = opendir(m_cErrorFilePath);
  if(dirp==NULL)
  {
      Log::log(0, "写文件的目录不能打开.程序退出...\n");
      return 0;        
  }
  for(dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
  {
    strncpy(sName, dp->d_name, 500);
    if(strcmp(sName, ".")==0 || strcmp(sName, "..")==0)
      continue;      
    
    snprintf(sTmp, 500, "%s/%s", m_cErrorFilePath, sName);
    
    if(lstat(sTmp, &statbuf)<0|| S_ISREG(statbuf.st_mode)==0) {
      continue;
    }
    ptr = localtime(&statbuf.st_mtime) ;
		strftime(sTime,50,"%Y%m%d",ptr) ;
		if(strcmp(sTime,endtime)<0) {
			snprintf(sCmd,500,"rm %s",sTmp) ;
			//printf("%s\n",sCmd) ;
			system(sCmd) ;
			num++ ;
		}
	}
	return num ;
}
    
  

bool ClearFile::getMergeFilePath() 
{
	 FILE *fd ;
	 string sPath ;
	 char *p = getenv( "TIBS_HOME" );
	 if ( p != NULL )
	 {
	    sPath = string(p);
	 }
	 printf("%s\n",sPath.c_str());
	 fd = fopen( ( sPath + string( "/etc/Merge.ini" ) ).c_str(),"r" );
   char sBuf[512]={0};
   if ( fgets(sBuf,512,fd) )
   {  
   	  CFtrim(sBuf) ;	
      m_mMergeFilePath = string(sBuf) ;
   }
   
   return 1 ;
}

int ClearFile::clearDataMergeFile()   
{
	//////文件数据的最后修改时间
	FILE *fd ;
	struct stat buf ;
	struct tm *ptr ;
	char tmp[50]={0} ;
	char m_name[100] = {0} ;
 // char *p ,*q;
  time_t t;
	fd = popen((string("ls ")+m_mMergeFilePath).c_str(),"r") ;
	if(!fd){
		Log::log(0,"请检查文件%s是否存在!",(string("ls ")+m_mMergeFilePath).c_str()) ;
		return 1;
	}
	while(fgets(m_name,100,fd)) {
		int size = m_mMergeFilePath.size() ;
		if(m_mMergeFilePath[size-1]!='/')
			m_mMergeFilePath.append("/") ;
		CFtrim(m_name) ;
		string st= m_mMergeFilePath + string(m_name) ;		
		if(stat(st.c_str(),&buf)==-1)
			Log::log(0,"请检查文件%s是否存在!",st.c_str()) ;		
		ptr = localtime(&buf.st_mtime) ;
		strftime(tmp,50,"%Y%m%d",ptr) ;
		if(strcmp(tmp,endtime)<0) {
			//printf("%s\n",(string("rm -r ")+st).c_str()) ;
			system((string("rm -r ")+st).c_str()) ;
		}
	}
	return 0 ;
}    

int ClearFile::clearDupHisFile()
{
	char sql[512]={0} ;	
	DEFINE_QUERY(qry) ;
	sprintf(sql,"select check_data_path,filename from b_checkdup_filelist "
               "where to_char(data_date,'yyyymmdd')<='%s'",endtime) ;
	qry.setSQL(sql) ;
	qry.open() ;
	while(qry.next())  {
		string ph(qry.field(0).asString()) ;
		int size = ph.size() ;
		if(ph[size-1]!='/')
			ph.append("/") ;
		string t = string(ph) +string(qry.field(1).asString()) ;
		//printf("%s\n",(string("rm ")+t).c_str()) ;
		system((string("rm ")+t).c_str()) ;		
	}
	qry.close() ;
	memset(sql,0,512) ;
	sprintf(sql,"delete from b_checkdup_filelist where to_char(data_date,'yyyymmdd')<=%s",endtime) ;
	qry.setSQL(sql) ;
	qry.execute() ;  
	qry.commit () ;
  qry.close() ;
	return 1 ;
}

bool ClearFile::getWdbHisPath()
{
	 if(true!=ParamDefineMgr::getParam("FILEDB","FILEDBPATH", m_wdbHisPath, 500 ))
   {
      m_wdbHisPath[0]='\0';
      return false ;
   }
   return true ;
}

int ClearFile::clearwdHisFile() 
{
	///写文件路径（配置路径+流程id+日期）
	///删除指定日期的文件和垃圾文件（没有放在指定路径）
	if(m_wdbHisPath[0]=='\0')
  {
    Log::log(0, "写文件的目录没有配置.程序退出...\n");
    return 0;      
  } 
  if(strlen(m_wdbHisPath)>0 && m_wdbHisPath[strlen(m_wdbHisPath)-1]=='/')
     m_wdbHisPath[strlen(m_wdbHisPath)-1]='\0';
     
	DIR *dirp=NULL;
  struct dirent *dp=NULL;
  char sName[500]={0};
  char sTmp[500]={0};
  char sCmd[500]={0};
  struct stat statbuf;  
  
  dirp = opendir(m_wdbHisPath);
  if(dirp==NULL)
  {
      Log::log(0, "写文件的目录不能打开.程序退出...\n");
      return 0;        
  }
  for(dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
    {
    strncpy(sName, dp->d_name, 500);
    if(strcmp(sName, ".")==0 || strcmp(sName, "..")==0)
      continue;      
    
    snprintf(sTmp, 500, "%s/%s", m_wdbHisPath, sName);
    
    if(lstat(sTmp, &statbuf)<0|| S_ISDIR(statbuf.st_mode)==0) {
    	system((string("rm ")+string(sTmp)).c_str()) ;
      continue;
    }

    DIR * dirp2=NULL;
    struct dirent *dp2=NULL;
    
    dirp2= opendir(sTmp);
    if(dirp2==NULL)
      continue;
    while( (dp2 = readdir(dirp2) )!=NULL  )
      {
      strncpy(sName, dp2->d_name, 500);
      if(strcmp(sName, ".")==0 || strcmp(sName, "..")==0)
        continue;   
      snprintf(sName, 500, "%s/%s", sTmp, dp2->d_name);     
      if(lstat(sName, &statbuf)<0|| S_ISDIR(statbuf.st_mode)==0 )  {
      	system((string("rm ")+string(sName)).c_str()) ;
        continue;
      }
      if( strcmp(dp2->d_name, endtime)<0 )
        {
         snprintf(sCmd, 500, "rm -r %s", sName);
         cout<<sCmd<<endl;
         if( system(sCmd)!=0 )
            Log::log(0,  "删除目录失败:( %s ),请手工处理..\n", sCmd);
        }
      }
    closedir(dirp2);
    }
  closedir(dirp); 
  return 0;
}

int ClearFile::clearTdtransTmpFile()
{
	////MERGE_FILE_DETAIL b_package_file_list_trans  b_package_state_trans
	struct stat buf ;
	char m_path[512]={0} ;
	char sql[512]={0} ;	
	DEFINE_QUERY(qry) ;  
	sprintf(sql,"select file_path,file_name from b_package_file_list_trans "
							"where package_id in(select package_id from b_package_state_trans "
							"	where to_char(state_date,'yyyymmdd') <'%s' )",endtime) ;
	qry.setSQL(sql) ;
	qry.open() ;
	while(qry.next()) {
		string str(qry.field(0).asString()) ;
		int size = str.size() ;
		if(str[size-1]!='/')
			str.append("/") ;
		string t = string(str) +string(qry.field(1).asString()) ;
		//printf("%s\n",t.c_str()) ;
		if(stat(t.c_str(),&buf)<0)
			continue ;
		if(checkPermission(t.c_str()) == 0 )
			continue ;
		//printf("%s\n",(string("rm ")+t).c_str()) ;
	  system((string("rm ")+t).c_str()) ;
	  string tStr = string("rm -r ")+string(qry.field(0).asString()) ;
	 // printf("%s\n",tStr.c_str()) ;
	  system(tStr.c_str()) ;
  }
  qry.close() ;
  memset(sql,0,512) ;
  sprintf(sql,"delete from MERGE_FILE_DETAIL a where a.file_id in ( "
               " select file_id from b_package_file_list_trans where package_id "
                " in (select package_id from b_package_state_trans "     
                " where to_char(state_date,'yyyymmdd') <'%s' ) )",endtime) ;
  qry.setSQL(sql) ;
	qry.execute() ;  
	qry.commit () ;
  qry.close() ;
  memset(sql,0,512) ;
  sprintf(sql,"delete from b_package_file_list_trans "
							"where package_id in(select package_id from b_package_state_trans "
							"	where to_char(state_date,'yyyymmdd') <'%s' )",endtime) ;
	qry.setSQL(sql) ;
	qry.execute() ;  
	qry.commit () ;
  qry.close() ;
  memset(sql,0,512) ;
  sprintf(sql,"delete from b_package_state_trans "
							"	where to_char(state_date,'yyyymmdd') <'%s' ",endtime) ;
	qry.setSQL(sql) ;
	qry.execute() ;  
	qry.commit () ;
  qry.close() ;
  return 1 ;
}

bool ClearFile::getAsnBackupPath()
{
	char sql[512]={0} ;
	vector<string>::iterator iter ;	
	DEFINE_QUERY(qry) ;
	sprintf(sql,"SELECT ATTR_VALUE FROM B_ASN_FILE_TYPE_INST_SET WHERE ATTR_ID=1011") ;
	qry.setSQL(sql) ;
	qry.open() ;
	while(qry.next())  {
		string str(qry.field(0).asString()) ;
		if(0 == m_asnBackupPath.size())
		  m_asnBackupPath.push_back(str) ;
		else  {
			iter = find(m_asnBackupPath.begin(),m_asnBackupPath.end(),str) ;
			if(iter == m_asnBackupPath.end()) 
				m_asnBackupPath.push_back(str) ;
		}
	}
	qry.close() ;
	memset(sql,0,512) ;
	sprintf(sql,"select * from b_asn_sys_param where param_code = 'BAK_DIR'") ;
	qry.setSQL(sql) ;
	qry.open() ;
	while(qry.next())  {
		string str(qry.field(0).asString()) ;
		if(0 == m_asnBackupPath.size())
		  m_asnBackupPath.push_back(str) ;
		else  {
			iter = find(m_asnBackupPath.begin(),m_asnBackupPath.end(),str) ;
			if(iter == m_asnBackupPath.end()) 
				m_asnBackupPath.push_back(str) ;
		}
	}
	qry.close() ;
	return true ;
}

int ClearFile::clearAsnBkFile()
{
	char sName[500] = {0} ;
	char sCmd[500] = {0} ;
	struct stat statbuf; 
	
	if(0 == m_asnBackupPath.size())  {
		Log::log(0, "asn编码前的备份文件目录没有配置.程序退出...\n");
    return 0;      
  } 
  vector<string>::iterator iter = m_asnBackupPath.begin() ;
  for(;iter!=m_asnBackupPath.end();iter++) 
  {
	  DIR * dirp=NULL;
	  struct dirent *dp=NULL;
	  
	  dirp= opendir((*iter).c_str());
	  if(dirp==NULL)
	    continue;
	  while( (dp = readdir(dirp) )!=NULL  )
	  {
	    strncpy(sName, dp->d_name, 500);
	    if(strcmp(sName, ".")==0 || strcmp(sName, "..")==0)
	      continue;   
	    snprintf(sName, 500, "%s/%s", (*iter).c_str(), dp->d_name);     
	    if(lstat(sName, &statbuf)<0|| S_ISDIR(statbuf.st_mode)==0 )  {//垃圾数据
	    	system((string("rm ")+string(sName)).c_str()) ;
	    	//printf("%s\n",(string("rm ")+string(sName)).c_str()) ;
	      continue;
	    }
	    if( strcmp(dp->d_name, endtime)<0 )///过期文件
	    {
	     snprintf(sCmd, 500, "rm -r %s", sName);
	     cout<<sCmd<<endl;
	     if( system(sCmd)!=0 )
	        Log::log(0,  "删除目录失败:( %s ),请手工处理..\n", sCmd);
	    }
	  }
	}
	return 1 ;
}
	
bool ClearFile::getRSexbakPath()
{
	ReadIni reader;
  char * p = getenv ("BILLDIR");
  if (!p) 
  	THROW (10);  	
  char sFile[256];
  char m_sFileName[256];
  memset (sFile, 0, sizeof (sFile));
  memset (m_sFileName, 0, sizeof (m_sFileName));
  strcpy (sFile, p);
  if (p[strlen(p)-1] != '/') strcat (sFile, "/");
  strcat (sFile, "etc/rent_log.ini"); 
  if (!reader.readIniString (sFile, "LOG_DIR", "dir_1", m_sFileName, 0)) {
      Log::log (0, "please check $BILLDIR/etc/rent_log ");
      return false;
      //THROW (11);
  }
  if (m_sFileName[strlen(m_sFileName)-1] != '/') 
      strcat (m_sFileName, "/");
  snprintf(m_eRSexbakPath, sizeof(m_eRSexbakPath), "%sbak/", m_sFileName);
  return true ;
}

int ClearFile::clearRSexbakFile()
{
   if(m_eRSexbakPath[0]=='\0')
	 {
	    Log::log(0, "租费套餐费备份目录没有配置.程序退出...\n");
	    return 0;      
	 }
	 char sIfBakHis[2] = {0} ;
	 int i=0 ;
   ParamDefineMgr::getParam("SETEX", "BAK_HIS", sIfBakHis, sizeof(sIfBakHis));
   if('0' == sIfBakHis[0] ){
   	 Log::log(0,"没有配置每次备份套餐费回退文件") ;
   	 	i++ ;
   }
   memset(sIfBakHis,0,2) ;
   ParamDefineMgr::getParam("RENTEX", "BAK_HIS", sIfBakHis, sizeof(sIfBakHis));
   if('0' == sIfBakHis[0] ){
   	 Log::log(0,"没有配置每次备份租费回退文件") ;
   	 	i++ ;
   }	
   if(2 == i )
   	 return 1 ;
   	 
   DIR *dirp=NULL;
	 struct dirent *dp=NULL;
	 char sName[500]={0};
	 char sTmp[500]={0};
	 char sCmd[500]={0};
	 char *p=0 ;
	 char sTime[50] ={0} ;
	 struct stat statbuf;  
	 
	 dirp = opendir(m_eRSexbakPath);
	 if(dirp==NULL)
	 {
	   Log::log(0, "租费套餐费备份目录不能打开.程序退出...\n");
	   return 0;        
	 }
	 for(dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
	 {
	   strncpy(sName, dp->d_name, 500);
	   if(strcmp(sName, ".")==0 || strcmp(sName, "..")==0)
	     continue;      
	 
	   snprintf(sTmp, 500, "%s%s", m_eRSexbakPath, sName);
	 
	   if(lstat(sTmp, &statbuf)<0|| S_ISREG(statbuf.st_mode)==0) {
	     continue;
	   }
	   p = strrchr(sName,'.') ;
     strcpy(sTime,p+1) ;
     if(sTime<endtime)  {
     	snprintf(sCmd,500,"rm %s",sTmp) ;
     	//printf("%s\n",sCmd) ;
     	system(sCmd) ;
    }
  }
  return 1 ;
}
   