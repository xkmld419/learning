#ifndef COMMANDBASE_H_HEADER
#define COMMANDBASE_H_HEADER

#include <stdio.h>
#include <string>
#include <unistd.h>
#include "../app_init/CommandCom.h"
#include "Environment.h"
#include "Date.h"
#include "Log.h"


#define DEFINE_COMMAND_MAIN(x) CommandBase *GetCommandBase () { return (new x()); }
#define DEFINE_SPLIT "|"

class 	CommandBase
{
    public:

	//纯虚函数
	virtual int run()=0;
	
	CommandBase();
	virtual ~CommandBase();

	bool WriteSocket(int ConnectSocket,char * msg);  //给前台发消息
	bool WriteSocket(int ConnectSocket,string & msg);  //给前台发消息

	int log(int log_level, char *format,  ...);  //日志接口
    //和前台协议的前4行的输出格式
    //sCommand:命令，iSize:输出条数，
    //sLable:输出的列名的拼接，ifTable:表示是否是表格格式输出给前台
	void FourOutForWeb(int iSize,char * sLable,int ifTable=1);
	void ErrOutForWeb(char *sErrMsg);
	void EndOutForWeb();
	void SpecialOutForWeb(char *sMsg,int ifTable=0);  //操作类输出

	int ConnectSocket;   //socket套接字
	CommandCom * m_pCommandComm;
};
#endif




