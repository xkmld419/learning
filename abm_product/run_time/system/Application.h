#ifndef __APPLICATION_H_INCLUDED_
#define __APPLICATION_H_INCLUDED_

#define ENV_LEN 128
#define HOST_NAME_LEN 64

class ABMException;
class Application;

extern int g_iModuID;    //模块ID
extern int g_iProcID;    //全局进程ID
extern int g_iNodeID;    //全局节点ID
extern char g_sAppName[32];  //应用程序名称


/*安全退出信号宏*/
#define RECEIVE_STOP() ReceiveStop()
/*信号处理注册函数*/
extern int SigHandlerSet(int sig, void (*handler)(int));
/*安全退出函数, true为收到安全退出信号*/
extern bool ReceiveStop(void);
extern bool RcvExcepSig(void);
extern void SigHandler(int sig);
#define DEFINE_MAIN(x) Application *GetApplication(){ return (new x()); }

class Application 
{	

public:

	Application();

	virtual ~Application();

	int appInit(ABMException &oExp); 

	virtual int init(ABMException &oExp) {
		return 0;
	}
	
	static int tryLock(const char *pfname, ABMException &oExp);

//	virtual int destory();

	virtual int run() = 0;		           

    char const * getHomeDir() { return m_sEnvAppDly; }

protected:

	char m_sEnvAppDly[ENV_LEN];//ABMAPP_DEPLOY环境变量

	char m_sHostName[HOST_NAME_LEN];   //主机名称

};

#endif



