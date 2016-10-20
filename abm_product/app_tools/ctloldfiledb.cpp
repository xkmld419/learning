/*VER: 1*/ 
#include "Date.h"
#include "Log.h"
#include "ParamDefineMgr.h"
#include "Environment.h"


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc ,char *argv[])
{
  
  int iOldDays=0;
  Date dt;


    if (argc ==1) {
        Log::log(0, "使用方法: ctloldfiledb -xxx; ## xxx 表示保留文件的天数");
        Log::log(0, " 例如 ctloldfiledb -35\n");
        return 0;
    }


    if (argc == 2 && argv[1][0]=='-')
    {
      iOldDays = atoi(argv[1]);
      
    }
    if( iOldDays >= -1 )
      {
      Log::log(0, "保留文件的天数不合理.程序退出...\n");
      return 0;
    }
    
    dt.addDay(iOldDays);
    char sFilePath[500];
    sFilePath[0]='\0';
    char sCmd[500];
    sCmd[0]='\0';

ParamDefineMgr::getParam("FILEDB", "FILEDBPATH", sFilePath, 500);
    
if(sFilePath[0]=='\0')
     {
      Log::log(0, "写文件的目录没有配置.程序退出...\n");
      return 0;      
     }   

if(strlen(sFilePath)>0 && sFilePath[strlen(sFilePath)-1]=='/')
  sFilePath[strlen(sFilePath)-1]='\0';




  DIR *dirp=NULL;
  struct dirent *dp=NULL;
  char sName[500];
  struct stat statbuf;  
  

  
  dirp = opendir(sFilePath);
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
      
    char sTmp[500];
    snprintf(sTmp, 500, "%s/%s", sFilePath, sName);
    
    if(lstat(sTmp, &statbuf)<0 
      || S_ISDIR(statbuf.st_mode)==0  )
      continue;

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
      if(lstat(sName, &statbuf)<0
         || S_ISDIR(statbuf.st_mode)==0  )
        continue;
      if( strcmp(dp2->d_name, dt.toString() )<0 )
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

