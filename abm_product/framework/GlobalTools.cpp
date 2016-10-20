#include "GlobalTools.h"
#include <unistd.h>
#define	MAXFILE	65535
#include "Env.h"
#include <sys/stat.h>

bool GlobalTools::CheckDateString(const char* sDate)
{
    if((0== sDate)||(sDate[0]==0)||(strlen(sDate)>14)||(atol(sDate)<=0))
        return false;
    static Date d1;
    d1.parse(sDate);
    return !strncmp(d1.toString(),sDate,strlen(sDate));
};
char* GlobalTools::upperLowerConvert( char* srcStr , const int flag )
{
    int iStrLen = strlen(srcStr);
    if ( iStrLen == 0 )
        return 0;

    if ( flag == _CONVERT_UPPER )
    {
        for ( int i = 0; i < iStrLen ; i++ )
        {
            if( srcStr[ i ] >= 'a' && srcStr[ i ] <= 'z' )
                srcStr[ i ] -= 32;
        }
    }
    else if ( flag == _CONVERT_LOWER )
    {
        for ( int i = 0; i < iStrLen ; i++ )
        {
            if( srcStr[ i ] >= 'A' && srcStr[ i ] <= 'Z' )
                srcStr[ i ] += 32;
        }
    }
    return srcStr;
};

bool GlobalTools::CheckProcessIsStop(pid_t pid)
{
    if(pid==0)
        return true;
    if((kill(pid, 0) == -1 && errno == ESRCH))
        return true;
    return false;
};

bool GlobalTools::CheckProcessIsStop(const char* cstrAppName)
{
    if(0== cstrAppName)
        return true;//不存在表示已经停止
    char sCheckShell[256]={0};
    sprintf(sCheckShell," ps -ef|grep %s |grep -iv 'grep'| "
        " awk '{print $1,$2}' |grep  `whoami` |awk '{print $2}' ",cstrAppName);
    FILE* fd = popen(sCheckShell,"r");
    if(fd==0)
        return true;
    if(feof(fd)==EOF)
        return true;
    char sPid[32]={0};
    if(fscanf(fd,"%s",sPid)==EOF)
        return true;
    pclose(fd);
    return CheckProcessIsStop(atoi(sPid)); 
};

int GlobalTools::StartApp(const char* pAppName,char* argv[32],const char* pBinDir)
{
    if(-1 == access(pAppName,X_OK|F_OK|R_OK|W_OK)){
        return -1;
    }
    //我不知道这样能否避免在多线程启动的程序产生死锁
    //如果有人发现有问题 ... 
    //死锁的描述: 多线程中会不可避免的使用到锁，比如string
    //当线程产生锁，这时又调用了fork那按照unix的写时复制原则新的进程这个锁状态
    //也是锁住的，且没有人对他关闭
    pid_t pid;
    int i;
    pid=fork();
    if(pid<0){
        printf("error in fork\n");
        exit(1);
    }else if(pid>0){
        return pid;
    }
    sleep(1);
    setsid();

    if(pBinDir==0)
        chdir("/");
    else
        chdir(pBinDir);
    umask(0);
    for(i=0;i<MAXFILE;i++)
        close(i);
    execve(pAppName,argv,environ);
    return 0;
}
bool GlobalTools::KillAppByShell(const char* pAppName)
{
    if(!pAppName)
        return true;

    char sCheckShell[256]={0};
    sprintf(sCheckShell," kill -9 `ps -ef|grep %s |grep -iv 'grep'| "
        " awk '{print $1,$2}' |grep  `whoami` |awk '{print $2}' ` ",pAppName);
    FILE* fd = popen(sCheckShell,"r");
    if(fd==0)
        return true;
    if(feof(fd)==EOF)
        return true;
    pclose(fd);
    return CheckProcessIsStop(pAppName);
}
int GlobalTools::CheckProcessNum(const char* sAppName)
{
    int iNum = 0;
    if(0== sAppName)
        return iNum;//不存在表示已经停止
    char sCheckShell[256]={0};
    sprintf(sCheckShell," ps -ef|grep %s |grep -iv 'grep' |grep -iv 'gdb' | "
        " awk '{print $1,$2}' |grep  `whoami` |awk '{print $2}' ", sAppName);
    FILE* fd = popen(sCheckShell,"r");
    if(fd==0)
        return iNum;
    if(feof(fd)==EOF)
        return iNum;
    char sPid[32]={0};
    while(fscanf(fd, "%s", sPid) != EOF)
    {
        if (!CheckProcessIsStop(atoi(sPid)))
            iNum++;
    }
    pclose(fd);
    return iNum;
}
int GlobalTools::StartShell(const char* pAppName,char* argv[32],const char* pBinDir)
{
    pid_t pid;
    pid=fork();
    if(pid<0){
        printf("error in fork\n");
        exit(1);
    }else if(pid>0){
        return pid;
    }
    if(pid<0){
        printf("error in fork\n");
        exit(1);
    }else if(pid>0){
        exit(0);
    }
    sleep(4);
    char sAppName[256]={0};
    sprintf(sAppName,"%s/%s %s",pBinDir,pAppName,argv[1]);
    execlp("/bin/sh","sh","-c",sAppName,NULL);
    return 0;
}


namespace StandardDevelopMent
{
    void    StandardTools::GetStdConfigFile(string& strFileName)
    {
        StandardTools::GetConfigPath(strFileName);
        strFileName.append("/etc/");
        strFileName.append(CONFIG_FILE); 
        if(-1== access(strFileName.c_str(),R_OK|F_OK)){
            cerr<<"文件不存在或者文件不可读写, 请检查文件 :"<<strFileName;
            return ;
        }
    }

    void StandardTools::GetConfigPath(string& strFilePath)
    {
        char *p =NULL;
        if((p=getenv(ENV_HOME))==0){
            strFilePath="/opt/";
        }else
            strFilePath=p;
        return;
    }
};
