#ifndef _SGWDEFINE_H_
#define _SGWDEFINE_H_

#include <iostream>
#include "Log.h"

using namespace std;

#define SGW_FROM_BILLNET     12 //內到外
#define SGW_FROM_EXTERNALNET 21 //外到內

#ifdef DEF_SICHUAN
const int PASS_MSG = 1; // 透传消息
const int SERV_NAME_LEN = 50;//服务名长度
#endif

#define CONLEN		20
#define CONADDR		32
#define SERVICENUM 20
#define SERVICELEN 72

#define	COMM_LEN	20

#define MAX_PROTOCOL_CODE_LEN 50
#define DCC_HEAD_LEN 20

#define MAX_PRODUCTNAME_SIZE        50
#define MAX_HOST_NAME_SIZE          128 //orgin-host最大长度
#define MAX_REALM_SIZE              100 //orgin-realm最大长度
#define MAX_SERVICE_CONTEXT_ID_SIZE 128 //业务类型
#define MAX_SERVICE_SIZE 2
#define MAX_ILLEGAL_KEYWORD_SIZE 100
#define MAX_USER_NAME_LEN 30
#define MAX_PASSWD_LEN 1000
#define MAX_IPADDR_SIZE 20
#define MAX_DATETIME_LEN 24
#define MAX_USER_CODE_LEN 12
#define MAX_NETINFO_CODE_LEN 9
#define MAX_SERVICE_CONTEXT_LIST_LEN 100
#define DATA_LEN 16
#define DCC_BUFF_SIZE 1024*16
#define MAX_CERTPATH_SIZE 200
#define MAX_BUSIKEY_SIZE 2048
#define	MAX_SESSION_ID_SIZE 256  //session_id最大长度
#define MAX_NETINFO_NAME_SIZE 100
#define MAX_SERVICECONTEXTIDCODE_SIZE 20
#define MAX_SOAPURL_SIZE 200
#define MAX_STREAM_PRE 500

//Public AvpCode define
#define ID_Session_Id			263
#define ID_Origin_Host			264
#define ID_Origin_Realm			296
#define ID_Auth_Application_Id	258
#define ID_CC_Request_Type		416
#define ID_CC_Request_Number	415
#define ID_CC_Result_Code		268

#define ID_CC_ServiceInformation 873
#define ID_CC_Service_Result_Code 20631

#define ID_CC_OutPlatformDescription 80409
#define ID_CC_Service_Flow_Id 80172

#define ID_Origin_State_Id		278
#define ID_Destination_Realm	283
#define ID_Destination_Host		293
#define ID_Service_Context_Id	461
#define ID_Event_Timestamp		55
#define ID_Termination_Cause	295
#define ID_Requested_Action		436

// 数据清单消息类型
#define MSG_TYPE_REQUEST_IN			"1"
#define MSG_TYPE_REQUEST_OUT		"2"
#define MSG_TYPE_ANSWEAR_IN			"3"
#define MSG_TYPE_ANSWEAR_OUT		"4"
#define MSG_TYPE_CHILD_REQUEST_IN	"5"
#define MSG_TYPE_CHILD_REQUEST_OUT	"6"
#define MSG_TYPE_CHILD_ANSWEAR_IN	"7"
#define MSG_TYPE_CHILD_ANSWEAR_OUT  "8"

const int DCC_CMD_CODE_CC = 272;
const int DCC_CMD_CODE_CE = 257;
const int MSG_CCR = 1;
const int MSG_CCA = 2;
const int CHILD_MSG_CCR = 3;
const int CHILD_MSG_CCA = 4;
const int SCSP_HEAD_BODY_LEN = 4;
const int SCSP_HEAD_LEN = 8;
const int SCSP_VERSION_LEN = 1;
const char SCSP_VERSION = 0x01;
const int SIG_LEN = 512;
const int SCSP_RTT_LEN = 17;
const int SCSP_RC_LEN = 8;
const int MSG_UID_LEN = 64;
const int MSG_PWD_LEN = 64;
const int MSG_RSV_LEN = 256;
const int MSG_DS_LEN = 256;
const int MAX_PEER_ERRCODE_LEN = 8; //对等端返回错误码长度
const int MAX_PEER_ERRMSG_LEN = 256; //对等端返回错误消息长度
const int MAX_TRACE_INFO_LEN = 1024; //消息走向跟踪信息

// 映射消息路径长度
const int MAX_MSG_MAP_PATH_LEN = 256;
// avp值长度
const int MAX_AVP_LEN = 2048;
// xml 节点名 属性名长度
const int MAX_NODE_NAME_LEN = 60;
//tuxedo最大接收长度
const int MAX_TUX_RECV_LEN = 10240;
//tuxedo最大发送长度
const int MAX_TUX_SEND_LEN = 10240;
// 消息映射可选性配置长度
const int MAX_SELECTIVE_LEN = 16;
// 变量名长度
const int MAX_VARIABLE_LEN = 100;

const int MAX_IP_LEN = 15;
// flowid len
const int MAX_FLOW_ID_SIZE = 6;

const int MAX_SERVICE_FLOW_ID = 300;
const int MAX_OUT_PLAT_LEN = 100;
const int TUXEDO_RTT_LEN = 17;
const int TUXEDO_RC_LEN = 8;

/* 映射消息的方向 */
typedef enum
{
	EN_DATA_TO_DCC = 1, EN_DCC_TO_DATA
} E_CONVERT_TYPE;

/* 映射消息bool 与 int 对应该值 */
typedef enum
{
	EN_FALSE = 0, EN_TRUE
} E_BOOL_INT;

const int MSG_MAP_TYPE_COUNT = 16;

/* 映射消息类型 */
typedef enum
{
	EN_SCSP_2_BILLDCC_CCR = 1, EN_BILLDCC_2_SCSP_CCA, EN_BILLDCC_2_SCSP_CCR, EN_SCSP_2_BILLDCC_CCA,

	EN_SOAP_2_BILLDCC_CCR, EN_BILLDCC_2_SOAP_CCA, EN_BILLDCC_2_SOAP_CCR, EN_SOAP_2_BILLDCC_CCA,

	EN_DCC_2_BILLDCC_CCR, EN_BILLDCC_2_DCC_CCA, EN_BILLDCC_2_DCC_CCR, EN_DCC_2_BILLDCC_CCA,

	EN_TUXEDO_2_BILLDCC_CCR, EN_BILLDCC_2_TUXEDO_CCA, EN_BILLDCC_2_TUXEDO_CCR, EN_TUXEDO_2_BILLDCC_CCA
} E_MSG_TYPE;

/* avp数据类型 */
typedef enum
{
	EN_OctetString = 1,
	EN_Integer32,
	EN_Integer64,
	EN_Unsigned32,
	EN_Unsigned64,
	EN_Float32,
	EN_Float64,
	EN_Grouped,
	EN_Address,
	EN_Time,
	EN_UTF8String,
	EN_DiameterIdentity,
	EN_Enumerated,
} E_AVP_DATA_TYPE;

/* 消息映射关系信息 */
typedef struct _Msg_Map_Info_
{
	int m_iID; // 编号
	int m_iDataType; // 数据类型 E_AVP_DATA_TYPE
	int m_iDesDataType;// 目标数据类型
	int m_iMsgType; // 消息类型 E_MSG_TYPE
	int m_iContextFilter; // 是否需要内容过滤 0不过滤 1过滤
	int m_iDataLimited; // 数据限定 如长度限定0 表示不限定
	int m_iXmlCData; // 值为CDATA类型
#ifdef DEF_SICHUAN
	bool m_bIsAcctID;// 是否为计费号码
#endif
	bool m_bMajorAvp; // 信息点数据
	bool m_bGlobCfg;// 是否合并了全局转换
	char m_szSelective[MAX_SELECTIVE_LEN + 1]; // 可选性配置
	char m_szServiceContextID[MAX_SERVICE_CONTEXT_ID_SIZE + 1]; // 能力
	char m_szDccNode[MAX_MSG_MAP_PATH_LEN + 1]; // DCC路径
	char m_szDataNode[MAX_MSG_MAP_PATH_LEN + 1]; // 数据路径
	char m_szDccCode[MAX_MSG_MAP_PATH_LEN + 1]; // code
	char m_szDccVendor[MAX_MSG_MAP_PATH_LEN + 1]; // vendor_id
	char m_szFormat[MAX_AVP_LEN + 1];// 格式化串
} SMsgMapInfo, *PSMsgMapInfo;

/*流量控制状态*/
typedef enum
{
	ROMAL = 1, WARNING = 2, DISCONNECT = 3,
} E_STREAM_CONTROL_STATE;

/* TCP连接方式 */
typedef enum
{
	EN_CONN_KEEP_ALIVE = 1, //长连接
	EN_CONN_ONE_OFF
//短连接
} E_TCP_CONN_STYLE;

/*路由分发标识*/
typedef enum
{
	ONE2TWO = 0,
	ONE2THRE = 1,
	TWO2FOUR1 = 1,
	TWO2FOUR2 = 2,
	TWO2FOUR3 = 3,
	TWO2FOUR4 = 4,
	TWO2THRE = 5,
	TWO2ONE = 6,
	TWO2FIVE = 7,
	TWO2SIX = 8,
	THRE2ONE = 1,
	THRE2TWO = 2,
	THRE2FIVE = 3,
	THRE2SIX = 4,
	FOUR12THRE = 0,
	FOUR22THRE = 0,
	FOUR32THRE = 0,
	FOUR42THRE = 0,
	FIVE2TWO = 0,
	SIX2TWO = 0,
} E_ROUTE_RULE;

/*是否断链*/
typedef enum
{
	EN_NOT_DISCON = 0, //不需要断链
	EN_NEED_DISCON, //需要断链
	EN_ALREADY_DISCON,
//已经断链
} E_DISCON_TYPE;

/*活动状态*/
typedef enum
{
	EN_ACTIVITY_UNKNOWN = -1, //未知
	EN_NOT_ACTIVE = 0, //未激活
	EN_ALREADY_ACTIVE = 1, //已激活
	//EN_SUBSTITUTE_ACTIVE = 3, //代激活
	EN_PRE_UNACTIVE = 210,//预去激活,即去激活的中间状态(1 to 0)
	EN_PRE_ACTIVE = 201
//预激活,即激活的中间状态(0 to 1)
} E_ACTIVE_STATUS;

/*如何激活*/
typedef enum
{
	EN_ACTIVE_BY_SELF = 0, //网元激活
	EN_ACTIVE_BY_SGW
//代网元激活
} E_HOW_ACTIVE;

/*激活动作*/
typedef enum
{
	E_DO_UNACTIVE = 0,//去激活
	E_DO_ACTIVE = 1
//激活
} E_ACTION_DIRECTION;

/*激活范围*/
typedef enum
{
	E_DOACTION_BY_USERNAME = 1,//按单个网元
	E_DOACTION_BY_NETFLAG = 2,//按网元类型
	E_DOACTION_BY_ALL = 3
//所有
} E_ACTION_BOUND;

/*SGW系统状态*/
typedef enum
{
	EN_SGW_UNKNOWN = -1, //未知
	EN_SGW_OFFLINE = 0, //离线
	EN_SGW_ONLINE = 1,//在线
	EN_SGW_NORMAL_STOPPING = 2, //正常停止中(中间状态)
	EN_SGW_NORMAL_STARTTING = 3, //正常启动中(中间状态)
	EN_SGW_FORCE_STOPPING = 4, //强制停止中(中间状态)
	EN_SGW_FORCE_STARTTING = 5
//强制启动中(中间状态)
} E_SGW_SYSTEM_STATUS;

/*是否马上做断链操作*/
typedef enum
{
	E_NOT_DO_DISCONN, E_DO_DISCONN
} E_DISCONN_ATONCE;

/*链路进程信息*/
struct ConnInfo
{
	int m_iRecvPid;
	int m_iSendPid;
	long m_lRecvMsgNum;
	long m_lSendMsgNum;
};

struct ServerInfo_Data
{
	bool m_beReport;
	int m_iPriority;
	int m_iPort;
	int m_iTimeOut;
	int m_beActive; /*get_set*/
	int m_beConnect; /*get_set */
	int m_iNetflag;
	char m_strIp[CONLEN];
	char m_strrIp[CONLEN];
	char m_strOrignHost[CONADDR];
	char m_strOrignRealm[CONADDR];
	char m_strNeHost[CONADDR];
	char m_strServiceContext[SERVICENUM][SERVICELEN]; //表示这个网元有哪些能力，1000不会置位这个
	int m_beServiceContextActive[SERVICENUM]; //表示这个SR下面网元的激活情况，1000不会置位这个
	int m_iRePid;
	ConnInfo m_ConnInfo;
};
//struct StructServerInfo;

//能力池
typedef struct
{
	char m_strHostName[MAX_HOST_NAME_SIZE + 1];
	int m_iPriority;
} NamePriorityPair;

typedef struct
{
	char m_strServiceContextId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	NamePriorityPair m_cDestination[MAX_SERVICE_SIZE];
} ServiceData;

typedef enum
{
	EN_DCC = 1, EN_SOAP, EN_SCSP, EN_TUXEDO
} EN_PROTO_TYPE;

//1-SR 2-SGW 3-外部网元 4-SC 5-内部业务网元
typedef enum
{
	EN_NETFLAG_SR = 1, //SR
	EN_NETFLAG_SGW = 2, //SGW
	EN_NETFLAG_EXTERIOR = 3, //外部网元
	EN_NETFLAG_SC = 4,//SC
	EN_NETFLAG_INTERIOR = 5
//内部业务网元
} EN_NETELEMENT_TYPE;

//协议解析到业务结构体
typedef struct
{
	int m_iProtoType;//--1-dcc 2-soap 3-scsp 4-tuxedo
	int m_iDirection;// SGW_FROM_BILLNET 内->外  SGW_FROM_EXTERNALNET 外->内
	int m_iSgwError;// 0 无错误 非0异常
	int m_iBuffLen;// 消息长度
	char m_strSgwError[MSG_DS_LEN + 1];// 异常信息
	char m_strHostName[MAX_HOST_NAME_SIZE + 1];
	char m_strServiceContextId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	char m_strSessionID[MAX_SESSION_ID_SIZE + 1];
	char m_strUserName[MAX_USER_NAME_LEN + 1];
	char m_strPasswd[MAX_PASSWD_LEN + 1];
	char m_strDccBuff[DCC_BUFF_SIZE + 1];
	int m_iCapPriority; //加一个能力优先级
	int m_iNePriority; //加一个网元优先级
	long m_lSendTime; //通讯层收到消息的时间
	long m_lWaitTime; //消息等待时间
	int m_iSocket;
	unsigned long m_ulSocketPtr;
	int m_iFromPort; // 消息来源端口
	char m_strFromIP[MAX_IP_LEN + 1]; // 消息来源IP
	int m_iMsgType; // MSG_CCR MSG_CCA CHILD_MSG_CCR CHILD_MSG_CCA
	int m_iEe;
	int m_iHh;
	int m_iRequestedAction;
	int m_iREPTAG; // scsp soap 重发标识
	int m_iCcRequestType;
	int m_iCcRequestNumber;
	unsigned int m_uiAuthApplicationId;
	char m_strDestinationHost[MAX_HOST_NAME_SIZE + 1];
	char m_strSIG[SIG_LEN + 1]; //加密字段,对传输的消息加密使用
	char m_strRSV1[MSG_RSV_LEN + 1]; //保留字段1
	char m_strRSV2[MSG_RSV_LEN + 1]; //保留字段2
	char m_strServiceFlowID[MAX_SERVICE_FLOW_ID + 1];
	char m_strOutPlat[MAX_OUT_PLAT_LEN + 1];
	char m_strNeCode[MAX_NETINFO_CODE_LEN + 1];//网元编码
	char m_strUserCode[MAX_USER_CODE_LEN + 1];//用户编码
	int m_iUserSeq; // 用户序列
	int m_iNetinfoSeq; // 网元序列
	int m_iDoDisConn; //是否马上激活	E_NOT_DO_DISCONN:否;E_DO_DISCONN：是
#ifdef DEF_SICHUAN
	int m_iSerPid;							// tuxedo 服务代理进程号
	char m_szFmlBuff[DCC_BUFF_SIZE + 1];	// fml 缓存
	int m_iFmlLen;							// fml 长度
	int m_iPassMsg;							// 0 不透传 1透传
	int m_iToServ;							// 0 (到SR） 非0目标用户的seq
	int m_iTargetServ;						// 目标tuxedo服务编号
	int m_iSourceServ;						// 源tuxedo服务编号
	int m_iServerResult;					// 业务结果
	char m_szServerResult[1024];			// 业务结果信息
#endif
	int m_iOldBuffLen;//原始数据长度
	char m_szTraceInfo[MAX_TRACE_INFO_LEN + 1];// 跟踪信息 由核心参数log.log_process_time 控制
	char m_szRequestValue[MAX_AVP_LEN + 1];// 请求关键数据(多个字段名与值)
	char m_szAnswerValue[MAX_AVP_LEN + 1];// 应答关键数据(多个字段名与值)
	char m_szAvpValue[MAX_AVP_LEN + 1];//关键字段值（信息点）
	char m_szReqFrom[2];// 接入网元归属 A.表示外部网元  B.表示内部网元
	char m_szOldSessionID[MAX_SESSION_ID_SIZE + 1];
	void AddTrace(const char * pszTraceInfo)
	{
		if (m_szTraceInfo[0] == '\0')
			strcpy(m_szTraceInfo,"<?xml version=\"1.0\" encoding=\"UTF-8\"?><DATA>");
		else
			strcat(m_szTraceInfo,pszTraceInfo);
	}
	void Display(void)
	{
		Log::log(0, "--------- Display ProtoToBusiData start ---------");
		Log::log(0, "m_iProtoType=%d", m_iProtoType);
		Log::log(0, "m_iDirection=%d", m_iDirection);
		Log::log(0, "m_iSgwError=%d", m_iSgwError);
		Log::log(0, "m_iBuffLen=%d", m_iBuffLen);
		Log::log(0, "m_strSgwError=%s", m_strSgwError);
		Log::log(0, "m_strHostName=%s", m_strHostName);
		Log::log(0, "m_strServiceContextId=%s", m_strServiceContextId);
		Log::log(0, "m_strSessionID=%s", m_strSessionID);
		Log::log(0, "m_strUserName=%s", m_strUserName);
		Log::log(0, "m_strPasswd=%s", m_strPasswd);
		Log::log(0, "m_strDccBuff=%s", m_strDccBuff);
		Log::log(0, "m_iCapPriority=%d", m_iCapPriority);
		Log::log(0, "m_iNePriority=%d", m_iNePriority);
		Log::log(0, "m_lSendTime=%ld", m_lSendTime);
		Log::log(0, "m_lWaitTime=%ld", m_lWaitTime);
		Log::log(0, "m_iSocket=%d", m_iSocket);
		Log::log(0, "m_ulSocketPtr=%lu", m_ulSocketPtr);
		Log::log(0, "m_iFromPort=%d", m_iFromPort);
		Log::log(0, "m_strFromIP=%s", m_strFromIP);
		Log::log(0, "m_iMsgType=%d", m_iMsgType);
		Log::log(0, "m_iEe=%d", m_iEe);
		Log::log(0, "m_iHh=%d", m_iHh);
		Log::log(0, "m_iRequestedAction=%d", m_iRequestedAction);
		Log::log(0, "m_iREPTAG=%d", m_iREPTAG);
		Log::log(0, "m_iCcRequestType=%d", m_iCcRequestType);
		Log::log(0, "m_iCcRequestNumber=%d", m_iCcRequestNumber);
		Log::log(0, "m_uiAuthApplicationId=%u", m_uiAuthApplicationId);
		Log::log(0, "m_strDestinationHost=%s", m_strDestinationHost);
		Log::log(0, "m_strSIG=%s", m_strSIG);
		Log::log(0, "m_strRSV1=%s", m_strRSV1);
		Log::log(0, "m_strRSV2=%s", m_strRSV2);
		Log::log(0, "m_strServiceFlowID=%s", m_strServiceFlowID);
		Log::log(0, "m_strOutPlat=%s", m_strOutPlat);
		Log::log(0, "m_strNeCode=%s", m_strNeCode);
		Log::log(0, "m_strUserCode=%s", m_strUserCode);
		Log::log(0, "m_iUserSeq=%d", m_iUserSeq);
		Log::log(0, "m_iNetinfoSeq=%d", m_iNetinfoSeq);
#ifdef DEF_SICHUAN
		Log::log(0, "m_iSerPid=%d", m_iSerPid);
		Log::log(0, "m_iFmlLen=%d", m_iFmlLen);
		Log::log(0, "m_iPassMsg=%d", m_iPassMsg);
		Log::log(0, "m_iToServ=%d", m_iToServ);
		Log::log(0, "m_iTargetServ=%d", m_iTargetServ);
#endif
		Log::log(0, "m_iOldBuffLen=%d", m_iOldBuffLen);
		Log::log(0, "m_szTraceInfo=%s", m_szTraceInfo);
		Log::log(0, "m_szRequestValue=%s", m_szRequestValue);
		Log::log(0, "m_szAnswerValue=%s", m_szAnswerValue);
		Log::log(0, "m_szAvpValue=%s", m_szAvpValue);
		Log::log(0, "m_szReqFrom=%s", m_szReqFrom);
		Log::log(0, "m_szOldSessionID=%s", m_szOldSessionID);
		Log::log(0, "--------- Display ProtoToBusiData end ---------");
	}
} ProtoToBusiData;

//内容过滤  表:context_filter  方法：select
typedef struct
{
	int m_iFilterSeq;
	char m_strServiceContextId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	char m_strIllegalKeyword[MAX_ILLEGAL_KEYWORD_SIZE + 1];
} ContextData;

//表:UserInfo 方法：select
struct UserInfoData//ServerInfo_Data
{
	int m_iUserSeq;
	char m_strUserName[MAX_USER_NAME_LEN + 1];
	char m_strRealm[MAX_REALM_SIZE + 1];
	char m_strIpAddr[MAX_IPADDR_SIZE + 1];
	int m_iPort;
	int m_iSgwSoapPort;
	int m_iAdaptiveInfoId;
	int m_iStatus;
	char m_strRegisterTime[MAX_DATETIME_LEN + 1];
	char m_strUpdateTime[MAX_DATETIME_LEN + 1];
	//add by yehao 2011-11-08 增加密钥更新时间
	char m_strPriUpdateTime[MAX_DATETIME_LEN + 1];
	//add end
	char m_strPasswd[MAX_PASSWD_LEN + 1];
	bool m_bIsEncrypt;
	bool m_bContextIssign;
	char m_strUserCode[MAX_USER_CODE_LEN + 1];
	int m_iNetFlag; //1-SR 2-SGW 3-外部网元 4-SC 5-内部业务网元
	int m_iActive; /*get_set *///0-未激活 1-已激活 3-代激活
	int m_beConnect; /*get_set *///含义：等于0-表示不可用  大于0表示连接数 小于0表示虚拟连接数
	int m_iTimeOut;
	int m_iAction;
	int m_iShortLinkFlag;
	int m_iNetinfoSeq;
	char m_strServiceContext[SERVICENUM][SERVICELEN];
	int m_iBeServiceContextActive[SERVICENUM];
	char m_strExternNetCertPath[MAX_CERTPATH_SIZE + 1];
	char m_sBusiPublicKey[MAX_CERTPATH_SIZE + 1];
	char m_sBusiPrivateKey[MAX_CERTPATH_SIZE + 1];
	char m_sBusiKeyExpDate[MAX_CERTPATH_SIZE + 1];
	char m_strSGWCertPath[MAX_CERTPATH_SIZE + 1];
	int m_iRePid;
	int m_iIsDisConn; //0-不需要断链 1-需要断链 2-已经断开
	long m_lReConnTime; //单位秒 <=0 不重连
	ConnInfo s_ConnInfo;
	int m_iPaddingType; //1-RSA_PKCS1_PADDING 2-RSA_SSLV23_PADDING 3-RSA_NO_PADDING 4-RSA_PKCS1_OAEP_PADDING 5-RSA_X931_PADDING
	int m_iPriority_level;
	char m_strSoapUrl[MAX_SOAPURL_SIZE + 1];
	int m_iNext;
	int m_iPasErrTime;
	int m_iSerErrTime;
	long m_lUnLockTime;
	int m_iSerConErr;
	int m_iSerConNoActive;
	int m_iWinElecServer;
	int m_iWinElecClient;
};

/*
 typedef struct {
 int  m_iUserSeq;
 char m_strUserName[MAX_USER_NAME_LEN+1];
 char m_strRealm[MAX_REALM_SIZE+1];
 char m_strIpAddr[MAX_IPADDR_SIZE+1];
 int  m_iPort;
 int  adaptive_info_id;
 int  m_iStatus;
 char m_strRegisterTime[MAX_DATETIME_LEN+1];
 char m_strUpdateTime[MAX_DATETIME_LEN+1];
 char m_strPasswd[MAX_PASSWD_LEN+1];
 bool m_bIsEncrypt;
 bool m_bContextIssign;
 char m_strUserCode[MAX_USER_CODE_LEN+1];
 int  m_iNetFlag;
 int  m_iActive;
 int  m_iTimeOut;
 char m_strExternNetCertPath[MAX_CERTPATH_SIZE+1];
 char m_strSGWCertPath[MAX_CERTPATH_SIZE+1];
 char m_strBusiPublicKey[MAX_BUSIKEY_SIZE+1];
 char m_strBusiPrivateKey[MAX_BUSIKEY_SIZE+1];
 char m_strBusiKeyExpDate[MAX_DATETIME_LEN+1];
 int  m_iNetinfoSeq;
 } UserInfoData;
 */
//表：SERVER_ROUTE_LIST 方法：select
typedef struct
{
	int m_Priority;
	char m_strHostIp[MAX_IPADDR_SIZE + 1];
	int m_iPort;
	int m_iTimeOut;
	char m_strHost[MAX_HOST_NAME_SIZE + 1];
	char m_strRealm[MAX_REALM_SIZE + 1];
	int m_iActive;
	char m_strRoutrIp[MAX_HOST_NAME_SIZE + 1];
	int m_iNetFlag;
} ServerRouteListData;

//表： NetInfo  方法： select
typedef struct
{
	int m_iNetinfoSeq;
	char m_strNetinfoName[MAX_NETINFO_NAME_SIZE + 1];
	char m_strRegisterTime[MAX_DATETIME_LEN + 1];
	char m_strUpdateTime[MAX_DATETIME_LEN + 1];
	char m_strNetinfoCode[MAX_NETINFO_CODE_LEN + 1];
	int m_iPriorityLevel;
	char m_strDepolyAddr[MAX_NETINFO_NAME_SIZE + 1];
	char m_strEventdorName[MAX_NETINFO_NAME_SIZE + 1];
	char m_strNeVersion[MAX_NETINFO_NAME_SIZE + 1];
	int m_iNodeId;
	int m_iNext;
} NetInfoData;

//表：service_context_list 方法：select
typedef struct
{
	int m_iServiceContextListId;
	char m_strServiceContextId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	int m_iActive;
	int m_iUserSeq;
	int m_iSign;//0 不签名 1签名
	char m_strFlowId[MAX_FLOW_ID_SIZE + 1];
	int m_iNext;
} ServiceContextListData;

//表：service_context_list_base 方法:
/*typedef struct {
 char m_strServiceContextId[MAX_SERVICE_CONTEXT_ID_SIZE+1];
 char m_strServiceContextIdName[MAX_SERVICE_CONTEXT_ID_SIZE+1];
 char m_strCreateDate[20+1];
 int  m_iServicePackageType;
 int  m_iLogicMode;//1-串行 2-并行 3-串并行
 char m_strServiceContextIdCode[MAX_SERVICECONTEXTIDCODE_SIZE+1];//
 int  m_iPriorityLevel;
 int m_iNext;
 } ServiceContextListBaseData;*/

typedef struct
{
	char m_strServiceContextId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	char m_strServiceContextIdName[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	char m_strServiceContextIdCode[20 + 1];
	char m_strCreateDate[20 + 1];
	int m_iServicePackageType;
	int m_iLogicMode;
	char m_strServiceContextIdDesc[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	int m_iPriorityLevel;
	char m_strMajorAvp[MAX_MSG_MAP_PATH_LEN + 1];
	int m_iNext;
} ServiceContextListBaseData;

//表: stream_ctrl_info 方法:
typedef struct
{
	char m_sUserName[MAX_USER_NAME_LEN + 1];
	char m_sSerConId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	int m_iCtrlCycle;
	int m_iWarningNumber;
	int m_iWarningFlow;
	int m_iDisconnectNum;
	int m_iDisconnectFlow;
	int m_iLenarnSelf;
	int m_iNext;
	int m_iReConnect;
} StreamControlInfo;

typedef struct
{
	int m_iOrgID;
	int m_iOrgLevelID;
	int m_iParentOrgID;
	char m_szOrgAreaCode[20];
}Sgw_Org_Route;

// 号头信息
typedef struct
{
	int m_iOrgID;
	char m_szHead[30];
	char m_szEffDate[22];
	char m_szExpDate[22];
	char m_szTableNmae[22]; //增加一个字段标示，这样到时候可以看出哪个表的数据上载有问题
}Sgw_Org_Head;



typedef struct
{
	char m_sUserName[MAX_USER_NAME_LEN + 1];
	char m_sSerConId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	double m_lSunFlow;
	int m_iSumNumber;
	char m_sStartDate[DATA_LEN];
	char m_sEndDate[DATA_LEN];
	int m_iSate;
	int m_iWarned;
	int m_iDisConnect;
	int m_iNext;
	int m_iPreMonth;
	double m_dCurFlow;
	long m_lCurNum;
	double m_dMonthData[12][2]; //[0][0]:消息流量 [0][1]:次数
	//	int m_iPreDay;
	//	int m_iPreData;
	//	double m_dDayData[31][2];
	//	double m_dPredata[MAX_STREAM_PRE][2];
} StreamControlData;

//表：SGW_SLA_QUE_REL  方法：select
typedef struct
{
	int m_iQueuePriority;
	int m_iServiceLevel;
	int m_iNetLevel;
	int m_iServicePercent;
	int m_iNetPercent;
	int m_iMessageTotalNum;
	int m_iNext;
} SgwSlaQueue;

typedef struct
{
	int m_iRequestType;
	int m_iRequestNumber;
	int m_iAuthApplicationId;
	int m_iRequestedAction;
	int m_iHh;
	int m_iEe;
	int m_lHostId;
	long m_sEventTimestamp;
	int m_iProtoType;//--1-dcc 2-soap 3-scsp
	int m_iREPTAG;
	int m_iSocket;
	unsigned long m_ulSocketPtr;
	char m_sSessionId[MAX_SESSION_ID_SIZE + 1];
	char m_sOriginHost[MAX_HOST_NAME_SIZE + 1];
	char m_strUserName[MAX_USER_NAME_LEN + 1];
	char m_sOriginRealm[MAX_REALM_SIZE + 1];
	char m_sDestinationHost[MAX_HOST_NAME_SIZE + 1];
	char m_sDestinationRealm[MAX_REALM_SIZE + 1];
	char m_sServiceContextId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	char m_svRoute_Record[10][COMM_LEN];
	char m_DateTime[DATA_LEN + 1];
	char m_sUID[MSG_UID_LEN + 8];
	char m_sServiceFlowID[MAX_SERVICE_FLOW_ID + 1];
	char m_sOutPlat[MAX_OUT_PLAT_LEN + 1];
	char m_sNeCode[MAX_NETINFO_CODE_LEN + 1];//网元编码
	char m_sUserCode[MAX_USER_CODE_LEN + 1];//用户编码
#ifdef DEF_SICHUAN
	int m_iSerPid;							// tuxedo 服务代理进程号
	int m_iPassMsg;							// 0 不透传 1透传
	int m_iToServ;							// 0 (到SR） 非0目标用户的seq
	int m_iTargetServ;						// 目标tuxedo服务编号
#endif
	char m_szReqFrom[2];// 接入网元归属 A.表示外部网元  B.表示内部网元
	int m_iDirection;
	long m_lSendTime; //通讯层收到消息的时间
	int m_iNext;
} SessionData;

//系统拥塞等级上报SC
enum eSysCongestLevel
{
	iNoCongest = 0, iLightCongest = 1000, iServeCongest = 2100, iCancelActive = 9999
};

typedef struct
{
	int m_iLevel; //拥塞等级 0,1000,2100,9999
	int m_irawLevel; //拥塞程度
	int m_iGeneMq; //消息队列使用率因子
	int m_iGeneCpu; //CPU使用率因子
	int m_iGeneMem; //内存使用率因子
	int m_iLimitSpeed; //文件传输的限制速率
	//int m_iCurSpeed;                   //文件传输的当前速率
	char m_sCreateTime[20]; //产生拥塞信息时间
	int m_iConfirmFlag; //拥塞上报成功标志 0--失败 1--成功
	int m_iNext;
} CongestLevel;

typedef struct
{
	char m_sServiceContextId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	int m_iPrority; // --消息优先级
	int m_iInMsgNum; // --入消息
	int m_iOutMsgNum; // --出消息
	int m_iDiscardNum; // --丢弃消息
	//int m_iProcessID;            // --进程ID
	//int m_iMsqID;                // --消息队列ID
	int m_iNext;
} StatisticsCongestMsg;

//负载均衡结构
typedef struct
{
	char SR_Host[MAX_HOST_NAME_SIZE]; //SR_hostname
	unsigned int Load_Balance_Rate; //负载均衡比例
	int m_iNext;
} LoadBalanceData;

typedef struct
{
	int m_iProcessID;
	int m_iMqID;
} ST_WF_PROCESS_MQ;

///////////////////////////////////////////////////////////////////////////
// 能力封装相关

typedef struct _Packet_Service_Session_
{
	char m_szSessionID[MAX_SESSION_ID_SIZE + 1]; // 源会话
	char m_szSubSessionID[MAX_SESSION_ID_SIZE + 1]; // 子会话
	char m_strServiceContextId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	int m_iRouteSeq; // 节点ID
	char m_szMsg[DCC_BUFF_SIZE + 1]; // 源的CCR、子的CCA
	int m_iBufSize;
	int m_iNext;
	int m_iSubNext;
} CPacketSession, SPacketSession, *PSPacketSession;

typedef struct //基础方法表
{
	int m_iBaseMethodID;
	char m_strBaseMethodName[51];
	char m_strBaseMethodDesc[101];
} CBaseMethod;

//能力封装CCA
typedef struct
{
	enum _Avp_Option_CCA_
	{
		EN_M_FIXED = 1, EN_C_FIXED = 2, EN_M_NOT_FIXED = 3, EN_C_NOT_FIXED = 4
	};
	int m_iServicePackageCCASeq;
	char m_strServiceContextId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	char m_strAvpName[MAX_MSG_MAP_PATH_LEN + 1];
	int m_iAvpType;
	int m_iAvpOption; //1-表示固定必选AVP 2-表示固定可选AVP 3-表示非固定必选AVP 4-表示非固定可选AVP
	int m_iFuncID;
	int m_iNext;
} CServicePackageCCA;

typedef struct
{//能力封装节点表
	int m_iNodeSeq;
	char m_strServiceContextId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	char m_strNodeSourceID[MAX_MSG_MAP_PATH_LEN + 1];
	int m_iNodeType;
	char m_strNodeName[MAX_MSG_MAP_PATH_LEN + 1];
	int m_iFlowId;
	int m_iNext;
} ServicePackageNode, CServicePackageNode;

typedef struct
{//能力封装路径
	int m_iRouteSeq;
	int m_iSourceNodeSeq;
	int m_iTargetNodeSeq;
	int m_iRouteCondition;
	char m_strServicePackageId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	int m_iFlowId;
	int m_iNext;
} ServicePackageRoute, CServicePackageRoute;

typedef struct
{//能力封装路径CCR
	enum _Avp_Option_CCR_
	{
		EN_C_AVP = 0, EN_M_AVP = 1,
	};
	int m_iRouteCCRSeq;
	int m_iRouteSeq;
	char m_strAvpName[MAX_MSG_MAP_PATH_LEN + 1];
	int m_iAvpType;
	int m_iAvpOption; //0-可选AVP 1-必选AVP
	int m_iFuncID;
	int m_iNext;
} CServicePackageRouteCCR;

typedef struct
{//能力封装判断条件
	int m_iJudgeConditionSeq;
	int m_iNodeSeq;
	int m_iJudgeConditionGroup;
	char m_strConditionName[MAX_MSG_MAP_PATH_LEN];
	int m_iOperaterID;
	int m_iCompareLeft;
	int m_iCompareRight;
	int m_iNext;
} CServicePackageJudge;

typedef struct
{
	enum _Package_Variable
	{
		EN_STATIC_VALUE = 1, EN_FATHER_CCR = 2, EN_SUB_SERVIC_CCA = 3
	};
	int m_iVariableID;
	int m_iVariableType;
	int m_iGetValueMode;// 1-固定值 2-主能力CCR 3-子能力CCA
	int m_iSourceAVPType;
	char m_szVariableName[MAX_VARIABLE_LEN + 1];
	char m_szSourceAVP[MAX_MSG_MAP_PATH_LEN + 1];
	char m_szSourceData[MAX_MSG_MAP_PATH_LEN + 1];
	int m_iNext;
} CServicePackageVariable;

typedef struct
{
	enum _Func_Param_Type_
	{
		EN_FUNC_VARIABLE = 1,// 变量
		EN_FUNC_PACKFUNC = 2,
	// 函数调用
	};
	int m_iCallID;
	int m_iCallFuncID;
	int m_iFuncParam1;
	int m_iFuncParam1Type;
	int m_iFuncParam2;
	int m_iFuncParam2Type;
	int m_iNext;
} CServicePackageFunc;

/* TUXEDO客户端信息 */
typedef struct
{
	int m_iClientPort; //客户端端口
	char m_sClientIPAddr[MAX_IP_LEN + 1]; //客户IP
} CTuxClientData;

/*SessionClear*/
typedef struct
{
	char m_sSessionID[MAX_SESSION_ID_SIZE + 1];
	char m_sServiceContextId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	int m_iMsgType; // MSG_CCR MSG_CCA CHILD_MSG_CCR CHILD_MSG_CCA
	long m_lTime;
	int m_iIsFlag;
	int m_iNext;
	int m_iIndexNext;
} SessionClearData;

typedef struct
{
	int m_iFree;
	int m_iPreDeal;
} SessionHead;

//端口信息
typedef struct
{
	int m_iPortId;
	char m_sProtoCode[MAX_PROTOCOL_CODE_LEN + 1];
	int m_iPort;
	int m_iState;//0-断开 1-监听
	char sDateTime[MAX_DATETIME_LEN + 1];
} PortInfo;

//
typedef struct
{
	long m_lTime;
	long m_lData;
	int m_iNext;
} StreamCtrlStudy;

// 通用配置
typedef struct
{
	int m_iGID;
	char m_szServiceContextID[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	char m_szSourceAvp[MAX_MSG_MAP_PATH_LEN + 1];
	char m_szDesAvp[MAX_MSG_MAP_PATH_LEN + 1];
	char m_szSelective[MAX_SELECTIVE_LEN + 1]; // 可选性配置
	char m_szFormat[MAX_AVP_LEN + 1];
	int m_iOperate;
	int m_iMsgType;
	int m_iSourceType;
	int m_iDesType;
}SgwGlobalMap;

// org 信息
typedef struct
{
	int m_iOrgID;
	int m_iOrgLevelID;
	int m_iParentOrgID;
	char m_szOrgAreaCode[20];
}SgwOrgRoute;

// 号头信息
typedef struct
{
	int m_iOrgID;
	char m_szHead[30];
	char m_szEffDate[22];
	char m_szExpDate[22];
	char m_szTableNmae[22]; //增加一个字段标示，这样到时候可以看出哪个表的数据上载有问题
}SgwOrgHead;

// 渠道表
typedef struct
{
	char m_szChannelID[30];
	char m_szChannelName[1008];
	char m_szCheckFlag[2];
	char m_szEnCode[10];
	char m_szEffDate[20];
	char m_szExpDate[20];
	char m_szNetInfoCode[10];
	int m_iUserSeq;
	int m_iNext;
}SgwChannelInfo;

//// 工号表
//typedef struct
//{
//	long m_lStaffID;
//	char m_szChannelID[30];
//}SgwStaffInfo;

//// 网元渠道关系表
//typedef struct
//{
//	int m_iChannelNetID;
//	char m_szNetInfoCode[10];
//	char m_szChannelID[30];
//	int m_iChannelNext;
//	int m_iCodeNext;
//}SgwNetChannel;

// 用户工号关系表
typedef struct
{
	int m_iUserStaffID;
	//int m_iUserSeq;
	long m_lStaffID;
	char m_szStaffName[257];
	char m_szChannelID[30];
	int m_iNext;
}SgwUserStaff;

// 区路由表
typedef struct
{
	int m_iRouteID;
	int m_iUserSeq;
	char m_szNetInfoCode[10];
	//char m_szOrgID[16];
	int m_iOrgID;
	int m_iNext;
}SgwOrgRouteRule;

// 代表计费区
typedef struct
{
	int m_iAreaSeq;
	int m_iUserSeq;
	int m_iOrgID;
	//char m_szOrgID[16];
}SgwAreaOrg;

// tuxedo服务与能力关系表(外到内)
typedef struct
{
	int m_iRelationID;
	char m_szServiceContextID[100];
	int m_iServicesID;
}SgwTuxedoRelationIn;

// tuxedo服务与能力关系表(内到外)
typedef struct
{
	int m_iRelationID;
	char m_szServiceContextID[100];
	int m_iServicesID;
	int m_iNext;
}SgwTuxedoRelationOut;

// tuxedo服务列表
typedef struct
{
	int m_iServicesID;
	char m_szServicesName[101];
	char m_szServicesDesc[1001];
	char m_szEffDate[20];
	char m_szExpDate[20];
	int m_iNext;
}SgwServicesList;

// 服务参数对应表
typedef struct
{
	int m_iMappingID;
	int m_iServicesID;
	int m_iParamID;
	int m_iParamFlag;
	char m_szParamName[101];
	int m_iParamVal;
	//char m_szParamType[101];
	int m_iParamType;
	//int m_iParamPub;
	char m_szParamDesc[51];
	int m_iParamLog;
	int m_iNext;
}SgwServicesParamRelation;

// 服务参数列表
typedef struct
{
	int m_iParamID;
	char m_szParamName[101];
	int m_iParamVal;
	//char m_szParamType[101];
	//int m_iParamPub;
	int m_iParamType;
	char m_szParamDesc[51];
}SgwServicesParamList;

// 服务注册表(tuxedo服务映射)
typedef struct
{
	int m_iServicesInstanceID;
	//char m_szNetInfoCode[10];
	char m_szChannelID[30];
	char m_szStateDate[20];
	int m_iSrcServicesID;
	char m_szSrcServicesName[101];
	int m_iMapServicesID;
	char m_szMapServicesName[101];
	int m_iAreaUser;
	//long m_lBlockTime;
	char m_szState[6];
	int m_iNext;
}SgwServiceRegister;

// 服务参数实例化表
typedef struct
{
	int m_iMappingID;
	int m_iServicesInstanceID;
	int m_iSrcParamID;
	char m_szSrcParamName[51];
	//char m_szSrcParamType[11];
	int m_iSrcParamType;
	int m_iSrcParamVal;
	int m_iDestParamID;
	char m_szDestParamName[51];
	//char m_szDestParamType[11];
	int m_iDestParamType;
	int m_iDestParamVal;
	char m_szSrcParamDesc[101];
	char m_szDestParamDesc[101];
	int m_iParamFlag;// 1入参 2出参
	int m_iParamLog;// 是否记录清单信息 1 是 0 否
	int m_iNext;
}SgwParamInstanceList;

// 全国代表区号
typedef struct  
{
	int m_iAreaSeq;
	char m_szAreaCode[6];
	int m_iOrgID;
	char m_szAreaName[128];
}SgwEnumArea;

#endif
