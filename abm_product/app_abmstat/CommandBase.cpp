#include "CommandBase.h"
#ifdef __IBM_AIX__
#include <va_list.h>
#endif

CommandBase * g_pCommand = 0;
int g_argc;
char ** g_argv;
int g_iFdFront;
int g_iStart;
int g_iEnd;
char g_sStartDate[16];
char g_sEndDate[16];


CommandBase::CommandBase()
{
   ConnectSocket = 0;
   m_pCommandComm = 0;
}

CommandBase::~CommandBase()
{
   if(ConnectSocket)
      close(ConnectSocket);
}

bool CommandBase::WriteSocket(int ConnectSocket,char * msg)
{
  if(ConnectSocket==0)
  	return false;
  
  int num=0;
  int ret;
  int Len = strlen(msg);

  while(num < Len)
  {
    ret = write(ConnectSocket,(char*)&msg[num],Len-num);
	if(ret<0)	
    {
       printf ("[ERROR]: write error\n");
	   return false;
    }
	num += ret;
  }
  return true;
  
}

bool CommandBase::WriteSocket(int ConnectSocket,string & msg)
{
  if(ConnectSocket==0)
  	return false;
  
  int num=0;
  int ret;
  int Len = msg.size();

  while(num < Len)
  {
    ret = write(ConnectSocket,(char*)(&((msg.c_str())[num])),Len-num);
	if(ret<0)	
    {
       printf ("[ERROR]: write error\n");
	   return false;
    }
	num += ret;
  }
  return true;  
}

int CommandBase::log(int log_level, char *format,  ...)
{
	va_list ap;
	Log::log (log_level, format, ap);

	return 1;
}
//以表格格式输出前四行提示信息
void  CommandBase::FourOutForWeb(int iSize,char * sLable,int ifTable)
{
   printf("<%s>\n","WEBFORSGW");
   printf("%d\n",ifTable);
   printf("%d\n",iSize);
   printf("%s\n",sLable);
   
}

//以表格格式输出结果集的时候用这个
//配合FourOutForWeb使用
void CommandBase::EndOutForWeb()
{
   printf("</%s>\n","WEBFORSGW");
}

//前台操作类输出，一般都是一行结果
void CommandBase::SpecialOutForWeb(char *sMsg,int ifTable)
{
  printf("<%s>\n","WEBFORSGW");
  printf("%d\n",ifTable);
  printf("%s\n",sMsg);
  printf("</%s>\n","WEBFORSGW");
}

//执行中遇到错误，输出到前台
void CommandBase::ErrOutForWeb(char *sErrMsg)
{
	printf("<%s>\n","ERRFORSGW");
	printf("%s\n",sErrMsg);
	printf("</%s>\n","ERRFORSGW");
}


