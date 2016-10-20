#include "LogFileMgr.h"
#include "../app_init/BaseFunc.h"
#define MAX_LINE_LEN	60
#define MAX_LINE_NUM	1023

LogFileMgr::LogFileMgr()
{
	m_sErrInfo[0] = 0;
	m_sFileName[0] = 0;
}

LogFileMgr::~LogFileMgr()
{
}

bool LogFileMgr::getLogData(int ProcessID,vector<string> &vec_log,int iBegineLine)
{
	//先获取数据 再检索
	memset(m_sFileName,'\0',sizeof(m_sFileName));
	FILE  * fp = 0;
	char *p = 0;
	int i = 0;
	int iLN = 0;
	char m_sTIBS_HOME[256] = {0};/* TIBS_HOME */
	if ((p=getenv ("TIBS_HOME")) == NULL)
		sprintf (m_sTIBS_HOME, "/home/bill/TIBS_HOME");
	else 
		sprintf (m_sTIBS_HOME, "%s", p);
		
	if (m_sTIBS_HOME[strlen(m_sTIBS_HOME)-1] == '/') {
		sprintf (m_sFileName, "%s%s%d%s",m_sTIBS_HOME,"log/process_",ProcessID,".log");
	} else {
		sprintf (m_sFileName, "%s%s%d%s",m_sTIBS_HOME,"/log/process_",ProcessID,".log");
	}
    if ((fp = fopen( m_sFileName,"r+")) == NULL)
    {
        if (( fp = fopen( m_sFileName,"r"))==NULL)
        {
            sprintf(m_sErrInfo,"%s%s","[file open error]file_name:", m_sFileName);
            return false;
        }
    }
    
    char sline[MAX_LINE_LEN] = {0};
 	for(int i=0;i<MAX_LINE_NUM;i++)
	{
		if(!fgets(sline,MAX_LINE_LEN,fp))
			break;
		int size = strlen(sline);		
		if(is_zh_ch(sline[size-1]) == 0)
		{
			sline[size-1] = '\0';
			fseek(fp,-1,SEEK_CUR);				
		} else {
			if(!checkLine(sline))
			{
				sprintf(m_sErrInfo,"%d%s",i,"存在回车字符");
				continue;
			}
		}
		vec_log.push_back(string(sline));
        if (feof (fp))
            break;
	}
	 fclose( fp );
	 m_sErrInfo[0] = 0;
	 return true;
	
}

/********
检查字符p是否是双字节汉字的一个字节
返回值： 0:  是汉字 1:  不是汉字
********/
int LogFileMgr::is_zh_ch(char p)
{
        /*if(~(p >> 8) == 0)
        {
                return(0);        
        }
        return(1);*/
		if((p&0x80)!=0) 
		{
			return 0;
		} else {
			return 1;
		}
		
}

bool LogFileMgr::checkLine(char *str)
{
	 char *p = str;
	 int size = strlen(str);
	 for(int i = 0;i<size;i++)
	 {
	 	if((is_zh_ch(str[i])!=0) && (str[i] == '\n'))
		{
			if(i != (size-1))
				return false;
		}
	 }
	 return true;
}
