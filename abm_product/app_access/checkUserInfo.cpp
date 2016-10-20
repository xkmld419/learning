#include "checkUserInfo.h"

USERMap CheckUserInfo::m_mParamMap ;	
char m_sStr[16]={0};
/*
*   函数介绍：获取工号验证结果
*   输入参数：错误码
*   输出参数：无
*   返回值  ：true/false
*/
bool CheckUserInfo::checkUerInfo(char * sUser,char *sPwd)
{
	  if(m_mParamMap.size()==0) 
	  	 ParseFile();
	  
	   if(strcmp(sPwd,m_sStr)==0) 
	   	  return true ;
	   	
	   string strfirst(sUser);
	   string strsecond;
	   map<string,string>::iterator m_mIterator ;
	   		 m_mIterator=m_mParamMap.find(sUser);
	 if(m_mIterator!=m_mParamMap.end()){
	      strsecond=m_mIterator->second;
       if(strcmp(strsecond.c_str(),sPwd)==0)
     	   return true ;	      
	 }

     	 
     return false ;
}

/*
*   函数介绍：解析文件，获取工号密码信息
*   输入参数：无
*   输出参数：无
*   返回值  ：成功返回0，否则返回指定错误
*/
int CheckUserInfo::ParseFile()
{
    char *penv = NULL;
    char sLibEnv[128] = {0};
    char sfile[256] = {0};
    char Pwd[128] = {0};
    if ((penv=getenv("ABM_PRO_DIR")) == NULL) {
        printf( "环境变量ABMLIB没有设置, init HBErrorCode Information failed返回错误");             
        return -1;
    }
    strncpy(sLibEnv, penv, sizeof(sLibEnv)-2);
    if (sLibEnv[strlen(sLibEnv)-1] != '/') {
        sLibEnv[strlen(sLibEnv)] = '/';             
    }

    snprintf(sfile, sizeof(sfile), "%s%s", sLibEnv,"etc/userinfo");

    char sBuffer[1024 + 1];
    char sCode[10 + 1];
    char *p = NULL;
    char *pBegin = NULL;
    unsigned int iRow = 0;
    unsigned int iLen = 0;
    memset(sBuffer, 0x00, sizeof(sBuffer));
    memset(sCode, 0x00, sizeof(sCode));
    int     iECode = 0;
    FILE *fp;
    
    char sBuff[1024 + 1];
	  memset (sBuff, 0, 256);
	  fread (sBuff, 256, 1, fp);
	  int iNum = strlen ((char *)sBuff);
	  int i;
	  for (i=0; i<iNum; i++) {
	  	m_sStr[i] = ~sBuff[i];
	  }
	  m_sStr[i] = 0;
	  fclose (fp);
	     
    if (!(fp = fopen(sfile, "r"))) {
        return -1;          // 打开文件失败
    }
    while(fgets(sBuffer, 1024, fp)) {
    	  iRow++ ;
        pBegin=sBuffer;
        p = strchr(pBegin, '|');
        if (NULL == p) {
            printf("配置数据有问题, 行号；%d, %s\n", iRow, sBuffer);
            break;
        }

        *p=0;
        char  suser[16]={0};
        strcpy(suser,pBegin);
        pBegin=p+1;
        p = strchr(pBegin, '|');     
        if(*p)
        {
        	*p=0 ;
        } 
        p = strchr(pBegin, '\n');     
        if(*p)
        {
        	*p=0 ;
        }             
        //strcpy(spwd,pBegin);       
        
        string strf(suser);
        string strs(pBegin);
        m_mParamMap.insert(pair<string ,string>(strf,strs));
        memset(sBuffer, 0x00, sizeof(sBuffer));        

    }
   	fclose (fp);
   	
    return 0;
}


