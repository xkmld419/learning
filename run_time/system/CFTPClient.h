/*VER: 1*/ 
/********************************************************

 * class name:CFTPClient

 * purpose:ftp 客户端

 * Language:c++ 

 * version:4.1.4    

 * OS Platform: hp,sun, aix, dec

 * Author:  huangjb 

 * Date:  2002.02.05

 * modify: 2002.03.12, 修复linux ftp server 退出登陆时出现core dump问题！

                并且对代码作了一些检查与修补

 *  2002.03.27, 增加登陆时不成功的返回状态

 *  2002.03.28, 增加对超时自由设定的修改以及对传输一个文件的最大时间限制的自由设定

 *  2002.04.04, 增加SIZE, STAT,RENAME命令, 4.0.8

 *  2002.04.04, 增加FTPCmd返回缓冲区最大长度的参数，取默认值时实现使用strcpy,否则使用strncpy, 4.1.0

 *  2002.04.04, 修改处理多行返回的情况， 4.1.1

 *  2002.05.09, 修复PutFile中STOR命令参数的设定（应该是使用STOR REMOTEFILE)， 4.1.2

 *  2002.05.20, 修复断点续传打开文件模式不对，另外将STAT命令改成STATCMD命令，4.1.3

 *  2002.06.10, 增加对错误后关闭套接口的检查，4.1.4

 * Cpryight (C)  Nanjing Linkage System Integration CO.,LTD.

 ********************************************************/


/*
//////////
zhangap
2007 05 
修正程序:
1. 使用ip地址登录时导致不能登录的bug
2. 获取文件列表时,有的服务器对于空文件夹不会在data端口传输数据,而是直接在控制端口返回传输完毕
   导致程序判断获取文件列表失败的假象
3.增加 pasv 模式,直接在函数里面加判断, 有点难看了   
/////////

*/



#ifndef __CFTP_H_CLIENT

#define __CFTP_H_CLIENT



#define FTP_E_CONNECT -4
#define FTP_E_USER  -3
#define FTP_E_PASS  -2
#define FTP_E_OTHER -1
#define FTP_E_FILE  -5


typedef int   OPTVAL;



enum  QUERY{NO=(OPTVAL)0,YES=(OPTVAL)1,MAYBE=(OPTVAL)2};
enum  FTPOPTION{TIMEOUT,  DATATYPE,  RESUME, MAXWAITTIME, PASSIVE};
enum  FTPCMDTYPE{CHDIR, CDUP, DEL, LS, PWD, MKDIR, RMDIR, SYSTYPE, SIZE, STATCMD, RENAME, PASV ,MDTM};


#define BINARY    (OPTVAL)1
#define AUTO    (OPTVAL)1
#define ASCII   (OPTVAL)0



class CFTPClient

{

public:

  CFTPClient();

  virtual ~CFTPClient();

    

public: 

  int FTPLogin(const char *strServer, const char *strUser,
      const char *strPassword=0, const int i_iPort=21);  

  int FTPCmd(FTPCMDTYPE cmd, const char* cmdvalue=0, char *returnvalue=0, int retmaxsize = 0);

  int GetFile(const char* strRemote, const char* strLocal = 0);

  int PutFile(const char* strLocal, const char* strRemote,int *iPct); 

  void FTPLogout();

  int FTPOption(FTPOPTION option, OPTVAL value);

    
  char   m_strReplyBuffer[10240];  //////方便外面取出来

    
private:

  int    m_hControlChannel; 

  int    m_hListenSocket;      

  int    m_hDataSocket;



  /* 主机字节序 */

  int   m_IsBigOrder;

  /* 超时设置 */

  OPTVAL  m_iTimeOut;

  OPTVAL  m_iResume;

  OPTVAL  m_iMaxWaitTime;

  OPTVAL  m_iPassive;

private:

  int   IsBigOrder();

  int CreateListenSocket();

  int WaitResult(int fd);

  int FTPCommand(const char* strCommand);

  int Accept(int fd,struct sockaddr* addr,int* len);

  int Write(int fd,char* buff,int *len);

  int Read(int fd,char* buff,int *len);

  int GetLine( char* src, char** nextline);

};


#endif /* __CFTP_H_CLIENT */

