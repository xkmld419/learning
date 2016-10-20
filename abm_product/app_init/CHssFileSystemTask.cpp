#include "CHssFileSystemTask.h"
#include <stdio.h>

//##ModelId=4BF9E6AB0019
CHbFileSystemTask::CHbFileSystemTask()
{
}


//##ModelId=4BF9E6AB007C
CHbFileSystemTask::~CHbFileSystemTask()
{
}

bool CHbFileSystemTask::Init(void)
{
	return true;
}

bool CHbFileSystemTask::getTaskResult(CmdMsg * vt,long& lMsgID,variant & vtRet,int* iRetSize)
{
	FILE* fp = popen("/usr/bin/df -g","r");
    if(!fp)
    {
        return 0;
    }
    char sLine[MAX_STRING_LEN];
    char sFileSys[50] = {0};
    char sFree[10] = {0};
    char sUsed[10] = {0};
    int iDisk = 0;
    fgets(sLine,MAX_STRING_LEN,fp);//
	char m_pDiskArray [MAX_DISK_NUM][MAX_STRING_LEN] ={0};
    while(fgets(sLine,MAX_STRING_LEN,fp))
    {
        strncpy(sFileSys,strtok(sLine," "),50);//Filesystem
        strtok(NULL," ");//GB blocks
        strncpy(sFree,strtok(NULL," "),10);//Free
        strncpy(sUsed,strtok(NULL," "),10);//Used
        sprintf(m_pDiskArray[iDisk++],"%-15s   %6s   %5s",sFileSys,sFree,sUsed);
    }
    pclose(fp);
	return m_pDiskArray;
}
