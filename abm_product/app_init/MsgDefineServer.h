#ifndef _MSG_DEFINE_SERVER_H_
#define _MSG_DEFINE_SERVER_H_

#define				SRC_ROOT								"\\ROOT"
#define				SRC_SHM								"\\ROOT\\SHM"
#define				SRC_RETURN						"\\ROOT\\RETURN"

#define				SRC_ROOT_APP_HBADMIN			"\\ROOT\\1_APPLICATION"
#define				SRC_ROOT_APP_SCHEDLUE			"\\ROOT\\1_APPLICATION"
#define				SRC_ROOT_APP_OTHER					"\\ROOT\\2_APPLICATION"
#define				SRC_ROOT_APP_INFOMONITOR	"\\ROOT\\2_INFOMONITOR"
#define				SRC_ROOT_APP_ASSISTPROC		"\\ROOT\\1_ASSISTPROC"
#define				SRC_ROOT_APP_MEMMGR		"\\ROOT\\1_MEMMGR"

#define				ROLE_MANAGER							1			//一号进程
#define				ROLE_BUSINESS							2			//业务进程
#define				ROLE_INFOMONITOR					3			//	监控进程

#define				ROLE_MEMMGR						4

#define				MSG_REGISTER						0x10000001			//注册消息
#define				MSG_SUBSCRIBE					0x10000002			//预约消息
#define				MSG_BROAD							0x10000003			//广播消息
#define				MSG_UNREGISTER				0x10000004			//注销消息
#define				MSG_RETURN						0x10000005			//返回消息

#define				MSG_START							0x10000006			//Ft6/7~Nq
#define				MSG_PROCESS_ONEXIT		0x10000007
#define				MSG_PROCESS_EXIT		0x10000008

#define				MSG_OP_RETURN				0x20000000			//确认消息
//#define				MSG_OP_ERROR				0x20000001
#define				MSG_OP_RETURN_OK				0x20000000			//确认消息
#define				MSG_OP_RETURN_ERROR				0x20000001

#define				MASK_MSG_TYPE				0xFFFF0000
#define				MASK_RETURN_MSG			0xF0000000
#define				MASK_OP_MSG					0x0FFFFFFF

#define				MSG_PARAM_SHM_ATTACH	0x03001100	

#define             MSG_PROCESS_START           0x04000001
#define             MSG_PROCESS_STOP            0x04000002
#define             MSG_PROCESS_INFO            0x04000003

#define				MSG_PROCESS_STOP_SIG		0x05000002			//	进程停止信号
#define				MSG_PROCESS_START_SIG		0x05000001			//进程启动信号

#define				MSG_SYSTEM_FILE_SYSTEM	0x06000001			//获取文件存储信息

#define				MSG_MEM_CREATE					0x07000001			//创建共享内存
#define				MSG_MEM_DESTORY				0x07000002			//销毁共享内存
#define				MSG_PARAM_SHM_ATTACH	0x03001100		//挂接参数共享内存
#define				MSG_PARAM_SAVE_IT				0x03001101		//CmdParser自己保存的变量
#define				MSG_PARAM_LOG_PATH			0x03001102	



#define				INIT_ERROR									100001000		
#define				INIT_ERROR_REGISTER				100001100		//注册失败
//@@	100001100 注册失败 服务端没有响应注册消息
#define				INIT_MESSAGEQUEUE_DUP		100001101		//消息队列重复
//@@	100001101 同一进程消息队列配置重复  同一进程消息队列配置重复修改配置
#define				INIT_ERROR_CREATE_SENDER	100001102		//消息队列创建失败
//@@	100001200 消息队列创建失败	服务端返回消息队列创建失败
#define				INIT_ERROR_NO_PARAM			100001001		//参数不存在
//@@ 100001001 查询参数不存在 请检查核心参数中是否存在改参数
#define				INIT_ERROR_UNRECOGNIZED	100001002		//命令无法识别
//@@ 100001002 命令无法识别 服务端无法识别改命令
#define				INIT_ERROR_PARAM_LOCK_KEY	100001201	//核心参数锁没有设置
//@@ 100001201 核心参数锁没有设置 核心参数锁没有设置检查ini配置文件
#define				INIT_ERROR_PARAM_MEM_KEY	100001202	//核心参数内存地址没有指定
//@@	100001202 核心参数内存地址没有指定 核心参数内存地址没有指定检查ini配置文件
#define				INIT_ERROR_PARAM_MEM			100001300	
//@@	100001301	核心参数共享内存过小 核心参数共享内存过小

#define				INIT_ERROR_INI_FILE						100002000	//核心参数文件
//@@	100002001	核心参数文件不存在	文件不存在或者不可访问	
//@@	100002002	核心参数文件错误		文件内不存在指定配置
//@@  100002003	核心参数重载失败		核心参数重载失败




#define				INIT_PROCESS_ERROR			100004000	//Ft6/2NJ}
//@@	100004001	Ft6/2NJ}N^7(J61p	Ft6/2NJ}N^7(J61p
//@@	100004002	Ft6/D#J=N^7(J61p	Ft6/D#J=N^7(J61p=x3LFt6/J'0\
//@@	100004100	=x3LforkJ'0\		=x3LforkJ'0\
//@@	100004200	7~NqFt6/J'0\		7~NqFt6/J'0\
//@@	100004201	7~NqFt6/3,J1		7~NqFt6/3,J1
//@@	100004202	7~NqFt6/3I9&		7~NqFt6/3I9&
#define				INIT_OP_OK								000000000	//成功

#define				INIT_ERROR_DBACCESS					100005000

//#define				ROLE_INFOMONITOR					100006000

typedef struct  _PROCESS_EXEC_INFO_
{
        char sProcessName[256];
        char     sProcessPath[256];
        char sArgv[3][256];
		int	iProcessStartTimes;
	int	iPid;
	int	iProcessState;	
	int	iRunOnce;
	int 	iProcType;
}ProcessExecInfo;

#endif

