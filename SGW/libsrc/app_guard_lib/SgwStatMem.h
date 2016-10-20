#ifndef SGW_STAT_MEM_H
#define SGW_STAT_MEM_H


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
//#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#include <pthread.h>
#include <sys/shm.h>

#include "ProcessInfo.h"
#include "CSemaphore.h"

#include "CommandCom.h"

#define MAX_STAT_EXTNET_INFO 2048		//外部网元个数
#define MAX_STAT_NETINFO_COUNT 2048		//网元处理情况
#define MAX_STAT_SERVICE_COUNT 512		//业务能力,优先级
#define MAX_STAT_PROCESS_INFO 1024		//进程处理情况
#define MAX_STAT_BLOCK_INFO 1024		//拥塞情况
#define MAX_STAT_INFOPOINT_INFO 1024	//拥塞情况

enum E_SHOWFLAG
{
	ON_TODAY = 1,
	ON_ONLINE = 2,
	ON_HOUR =3
};

//外部网元
struct S_ExtNetInfo{
	long lExtNetInfoID;			//外部网元标示
	char sNetInfoName[100];		//网元名
	long lUserID;				//用户标示
	char sUserName[30];			//用户名
	//long lServiceID;			//业务能力标示
	char sServiceName[128];		//业务能力名
	long lToatl;				//处理总数
	long lSuccNum;				//成功处理数
	S_ExtNetInfo(){
		lExtNetInfoID = 0;
		memset(sUserName,0,30);
		memset(sServiceName,0,30);
		lExtNetInfoID = 0;
		lUserID = 0;
	}
};

struct S_ExtNetInfo_Buff{
	int iTotal;
	int iDay;
	int iHour;
	struct S_ExtNetInfo aExtNetInfo_[MAX_STAT_EXTNET_INFO];
};

//网元处理情况
struct S_NetInfoCount{
	long lNetInfoID;			//网元标示
	char sNetInfoName[100];		//网元名
	//long lServiceID;			//业务能力标示
	char sServiceName[128];		//业务能力名
	long lReqNum;				//请求数
	int lReqSuccNum;			//请求成功数
	long lReqData;				//数据量(k)
	long lCallServiceNum;		//能力调用次数
	long lCallServiceSucc;		//调用成功数
	S_NetInfoCount(){
		memset(sNetInfoName,0,30);
		memset(sServiceName,0,30);
		lNetInfoID = 0;
		lReqNum = 0;
		lReqSuccNum = 0;
		lReqData = 0;
		lCallServiceNum = 0;
		lCallServiceSucc = 0;		
	}
};

struct S_NetInfoCount_Buff{
	int iTotal;
	int iDay;
	int iHour;
	struct S_NetInfoCount aNetInfoCount_[MAX_STAT_NETINFO_COUNT];
};

//业务能力优先级
struct S_ServiceCount{
	//long lServiceID;			//业务能力标示
	char sServiceName[128];		//业务能力名
	int iPLevel;				//优先级
	long lTotal;				//处理总数
	long lSuccNum;				//成功处理数
};

struct S_ServiceCount_Buff{
	int iTotal;
	int iDay;
	int iHour;
	struct S_ServiceCount aServiceCount_[MAX_STAT_SERVICE_COUNT];
};


//进程处理情况
struct S_ProcessInfo{
	int iProcessID;//进程ID
	long iAllTickets;
	long iNormalTickets;
	long iBlackTickets;
	long iErrTickets;
	long iOtherTickets;

};

struct S_ProcessInfo_Buff{
	int iTotal;
	int iDay;
	int iHour;
	struct S_ProcessInfo aProcessInfo_[MAX_STAT_PROCESS_INFO];
};



//统计信息
struct S_AllCount{
	long lReqNum;		//1)	请求次数、
	long lTimeOutNum;	//2)	请求超时次数(此情况下不回复应答消息)、
	long lSuccNum;		//3)	应答成功次数（包含业务成功处理和业务处理失败的情况）、
	long lFaultNum;		//4)	应答失败次数（包含SGW发起的回复次数和SR路由失败的次数）、
	long lAuthNum;		//5)	能力鉴权失败次数、
	long lCodeFaultNum;	//6)	协议转换失败次数、
	long lTranNum;		//7)	消息转发失败次数、
	long lMarkCheckNum;	//8)	签名验证失败次数、
	long lSRFaultNum;	//9)	SR路由失败次数
	long lOtherNum;//其他错误
};

struct S_AllCount_Buff{
	int iDay;
	int iHour;
	struct S_AllCount aAllCount_;
};

//拥塞情况
struct S_BlockInfo{
	//long lServiceID;			//业务能力标示
	char sServiceName[128];		//业务能力名
	int iPLevel;				//优先级
	long lInNum;				//入消息
	long lOutNum;				//出消息
	long lDiscardNum;			//丢弃消息
	
};

struct S_BlockInfo_Buff{
	int iTotal;
	struct S_BlockInfo aBlockInfo_[MAX_STAT_BLOCK_INFO];
};

//信息点
//信息点1
//index [m_szServiceContextId,m_szOriginHost,m_szProtocolType,m_iResultCode,
//	m_iSuccessFlag,m_iReqDir,m_szProtocolType,m_iMsgType]
struct S_InfoPoint
{
	int	m_iMsgType;			// 消息类型
	int	m_iSuccessFlag;			// 是否成功
	int	m_iReqDir;			// 方向
	long m_lReqCount;			// 消息数 累加
	long m_lReqBytes;			// 消息大小 Byte 累加
	int	m_iResultCode;			// 返回码
	char m_szKey[100];			// 能力||消息类型（1|0）
	char m_szServiceContextId[100];		// 能力名称
	char m_szOriginHost[100];		// 请求 OriginHost
	char m_szProtocolType[10];		// 协议类型（SOAP|SCSP|TUXEDO|DCC）
};
struct S_InfoPoint_Buff
{
	int iTotal;
	struct S_InfoPoint aInfoPoint_[MAX_STAT_INFOPOINT_INFO];
};

//信息点2 top 10
//index【m_szService_context_id,m_szMajor_avp_value,m_szProtocolType】
struct S_InfoPointTop
{
	long m_lCount;				// 消息数 累加
	char m_szService_context_id[100];	// 能力名称
	char m_szMajor_avp_value[100];		// 关键字段值
	char m_szKey[100];			// 能力||消息类型（1|0）
	char m_szProtocolType[10];		// 协议类型（SOAP|SCSP|TUXEDO|DCC）
};

struct S_InfoPointTop_Buff
{
	int iTotal;
	struct S_InfoPointTop aInfoPointTop_[MAX_STAT_INFOPOINT_INFO];
};


struct S_StatAllInfo
{
	//负载均衡配比
	int iBalancePer;

	//拥塞相关
	int iBlockLevel;		//拥塞级别
	char sBlockTime[16];	//拥塞发现时间
	int iRateCfg;			//配置的文件速率
	int iRate;				//文件速率


	//统计信息
	struct S_AllCount_Buff aAllCount[3];

	//进程
	struct S_ProcessInfo_Buff aProcessInfo[3];
	
	//业务能力优先级
	struct S_ServiceCount_Buff aServiceCount[3];

	//网元处理
	struct S_NetInfoCount_Buff aNetInfoCount[3];

	//外部网元
	struct S_ExtNetInfo_Buff aExtNetInfo[3];

	//拥塞情况
	struct S_BlockInfo_Buff aBlockInfo;

	//信息点
	struct S_InfoPoint_Buff aInfoPoint;
	struct S_InfoPointTop_Buff aInfoPointTop;
};



class StatAllInfoMgr
{
	public:
		StatAllInfoMgr();
		~StatAllInfoMgr();
	public:
		static struct S_StatAllInfo *pStatAllInfo;
		static TProcessInfo *pProcInfoHead;
		static THeadInfo *pInfoHead;
		static bool AttachStatShm();
		static bool DetachStatShm();
		static bool GetStatMemKey();
		static bool GetStatLock();
		static bool CreateStatShm();
		static bool DestroyStatShm();
		static bool ClearStatShm();
		static bool ConnectGuardMain();
		static void GetDayAndHour(char* sDatetime,int& iDay,int& iHour);
		
	private:
		static long m_lKey;
		static long m_lLockKey;
		static CSemaphore *pSem;
		
		static CommandCom* m_pCmdCom;
	public:
		//以下函数涉及到处理量，传入都为增量

		//网元处理情况接口
		static bool addNetInfoCount(long lNetInfoID,char *sNetInfoName, char *sServiceName,int iReqNumAdd/*增量*/,
			bool bReqSucc, int iReqDataAdd/*增量*/,int iCallServiceAdd,bool bCallSucc,char* sDatetime = NULL );

		//外部网元情况接口
		static bool addExtNetInfo(long lExtNetInfoID,char* sNetInfoName,long lUserID, char* sUserName, 
			char* sServiceName,int iNum,bool bSucc,char* sDatetime= NULL );

		//业务优先级
		static bool addServiceCount(char *sServiceName,int iPLevel,int iNum,bool bSucc,char* sDatetime= NULL);

		//统计信息
		static bool addAllInfo(int iReqNum, int iTimeOutNum,int iSuccNum,int iFaultNum,
			int iAuthNum,int iCodeFaultNum,int iTranNum,int iMarkCheckNum,int iSRFaultNum,
			int iOtherNum,char* sDatetime= NULL);
		
		//话单统计：进程维度每个进程处理结束后调用。
		static bool addTicket(int iProcessID,int iType/*0:Normal;1:Error*/,int iNum,char* sDatetime= NULL);

		//拥塞级别
		static bool setBlockLevel(int iBlockLevel,int iRateCfg,char* sBlockTime);

		//拥塞时文件速率
		static bool setBlockRate(int iRate);

		//拥塞统计
		static bool addBlockInfo( char* sServiceName,int iPLevel, int lInNum, int lOutNum,int lDiscardNum,
			char* sDatetime= NULL);

		//设置负载均衡配比
		static bool setBalancePer(int iPer);

		//信息点
		static bool addInfoPoint(struct S_InfoPoint *pInfoPoint);
		static int getInfoPoint(struct S_InfoPoint *pInfoPoint,int iBuffLen);
		static bool delInfoPoint();

		static bool addInfoPointTop(struct S_InfoPointTop *pInfoPointTop);
		static int getInfoPointTop(struct S_InfoPointTop *pInfoPointTop,int iBuffLen);
		static bool delInfoPointTop();
		
};


#endif

