// g++ -g -o writelog.o -c writelog.cpp 
// g++ -o writelog writelog.o 

#include "writelog.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

WriteLog::WriteLog(int sec, int num, char *sPath, char *sPreFix)
{
	this->sec=sec;
	this->num=num;
	strcpy(this->sPath,sPath);
	strcpy(this->sPreFix,sPreFix);
	char szTmp[20] = {0};
	sprintf(szTmp,"%d_",getpid());
	strcat(this->sPreFix,szTmp);
	this->timeold=0;
	this->timenew=0;
	this->iNowNum=0;
	this->bf=NULL;
}

bool WriteLog::InsertLog(char * sBuf, int size)
{
	char sTimeBuf[32];
	time(&timenew);

	if ( timenew - timeold > sec  || iNowNum == 0)
	{
		strcpy(sFileName,sPath);
		if ( sFileName[strlen(sFileName)-1] != '/' )
			strcat(sFileName,"/");

		strcpy(m_szTmpFileName,sFileName);
		strcat(m_szTmpFileName,"~tmp_");
		strcat(sFileName,sPreFix);
		strcat(m_szTmpFileName,sPreFix);

		GetNowTime(timenew,sTimeBuf,"%Y%m%d%H%M%S",sizeof(sTimeBuf));
		strcat(sFileName,sTimeBuf);
		strcat(m_szTmpFileName,sTimeBuf);
		// 关闭旧文件
		if (bf != NULL)
		{
			fclose(bf);
			rename(m_szTmpFileNameOld,m_szFileNameOld);
		}
		// 查看文件是否存在
		bf = fopen(m_szTmpFileName,"r");
		if (bf == NULL)
			bf=fopen(m_szTmpFileName,"wb");
		else
		{
			fclose(bf);
			bf = fopen(m_szTmpFileName,"ab");
		}	
		strcpy(m_szTmpFileNameOld,m_szTmpFileName);
		strcpy(m_szFileNameOld,sFileName);
		if (bf == NULL)
		{
			strcpy(sErrMsg,sFileName);
			strcat(sErrMsg,"该文件无法创建，请检查路径设置是否正确！");
			return false;
		}

		timeold = timenew;
	}

	iNowNum= (iNowNum+1)%num;

	int iTimes = 1;
	for (; iTimes <= 2; iTimes++)
	{
		int isize = fwrite(sBuf,size,1,bf);

		if (isize == 1)
		{
			break;
		}
		else
		{
			if (iTimes == 1)
			{
				if (bf != NULL)
				{
					fclose(bf);
				}
				bf = fopen(m_szTmpFileName,"wb");
				continue;
			}
			else
			{
				sprintf(sErrMsg,"%s:%s\n fwrite error", __FILE__,__LINE__);
				return false;
			}
		}
	}
	return true;		

}

WriteLog::~WriteLog()
{
	if (bf!=NULL)
	{
		fclose(bf);
		rename(m_szTmpFileName,sFileName);
	}
}

int WriteLog::GetNowTime(time_t t,char * sTimeBuf,char * sFormat ,int iSizeof)
{
	struct tm *now;
	now=localtime(&t);
	strftime(sTimeBuf,iSizeof,sFormat,now);
}

int  WriteLog::flush()
{	
	return fflush(bf);
}

char * WriteLog::getErrMsg()
{
	return sErrMsg;
}

#ifdef DEBUG_MAIN
int main ()
{
	char sMsg[]="hello!ddddd\n";
	WriteLog  a(60,10,"./log/","lintest");

	for(;;){
		if (!a.InsertLog(sMsg,strlen(sMsg)) )
			cout<<a.getErrMsg();
		char c=getchar();
		if (c=='q')
			break;
		a.flush();
	}
}
#endif



