/***************************/
/* Create 04/04/2005.      */
/* Modify 04/04/2005       */
/***************************/

#ifndef MID_COMPONENT_COMMON_INTERFACE_H
#define MID_COMPONENT_COMMON_INTERFACE_H

#ifndef NULL
#define NULL 			0
#endif /* NULL */

/* Include 文件 */
#ifdef TUXEDO
#  include <atmi.h>
#  ifdef TX
#     include <tx.h>
#  endif /* TX */
#endif/*TUXEDO*/

#ifdef TONGEASY
#  include "./teapi.h"
#endif /*TONGEASY*/


/* 定义错误列表 */
#   define _COMMON_ERROR_        0
#   define _LOG_ERROR_           100
#   define _TM_ERROR_            200
#   define _CM_ERROR_            300

/* 为兼容TongEASY,TUXEDO声明的结构类型 */
#ifdef TUXEDO
   typedef struct pkt_info
   {	
	char Ver;		/* 数据包类型 */
	char PktType[5];	/* 数据包类型标识符，核心填写 */
	long TxFlag;		/* 交易包控制描述信息 */
				/* 0x00010000L:	数据包压缩 */
				/* 0x00020000L:	文件压缩 */
				/* 0x00040000L:	数据包加密?*/
				/* 0x00080000L:	文件加密 */
				/* 0x00001000L:	需要名字服务 */
	char Src[13];		/* 代理机构码 */
	char Snd[13];		/* 发送机构码 */
	char Rcv[13];		/* 接收机构码 */
	int  TID;		/* 交易序号 */
	char OptCode[7];	/* 申请的操作码 */
	char EchoCode[3];	/* 响应码 */
	int  DataLen;		/* 用户数据包长度 */
	int  FileLen;		/* 收发文件名长度 */
	int  TxType;		/* 交易类型 */
	int  EchoTime;		/* 等待应答时间 */
	int  AckTime;		/* 等待确认时间 */
	int  SvrTime;		/* server 运行时间 */
	int  FwdTime;		/* 等待转发时间 */
	int  FileNum;
	char **FName;
	int  CvsTblNo;	
	int  KeyDataLen;
   } TEPKTINFO;
   
   typedef struct _mcci_tuxedo_
   {
        int    ierrno;	       /* 错误码 */
        char   *errstr;	       /* 错误信息 */
   } MCCI_TUXEDO_ERROR;
#endif /*TUXEDO*/

#ifdef TONGEASY
   /* 内部使用通讯结构 */
#  define MAXTHREAD 100	/* 线程数限制 */

#  ifndef FILEMAXLEN
#     define FILEMAXLEN 80
#  endif 

   typedef struct csl_pkt
   {
      TEPKTINFO pinfo;	/* 交易信息结构 */
      char      *pdata;	/* 数据缓冲区 	*/
      char      *fname; /* 文件缓冲区	*/
   }CSLPKT;

   typedef struct csl_info
   {
      unsigned long	 thd;	/* 线程号	*/
      CSLPKT		*ptr;	/* 通讯结构指针 */
   }CSLINFO;

   typedef struct tpinfo_t
   {
      char	usrname[32];	/* client user name */
      char	cltname[32];	/* application client name */
      char	passwd[32];	/* application password */
      char	grpname[32];	/* client group name */
      long	flags;		/* initialization flags */
      long	datalen;	/* length of app specific data */
      long	data;		/* placeholder for app data */
   }TPINIT;
      
   /* client identifier structure */
   struct clientid_t
   {
     long clientdata[4];     /* reserved for internal use */
   };
   typedef struct clientid_t CLIENTID;

   /* interface to service routines */
   struct tpsvcinfo 
   {
#     define XATMI_SERVICE_NAME_LENGTH  32
      char	name[XATMI_SERVICE_NAME_LENGTH];/* service name invoked */
      long	flags;		/* describes service attributes */
      char	*data;		/* pointer to data */
      long	len;		/* request data length */
      int	cd;		/* reserved for future use */
      long	appkey;		/* application authentication client key */
      CLIENTID cltid;		/* client identifier for originating client */
   };
      
#endif /*TONGEASY*/


/* Flags to service routines */

#define CSLNOBLOCK	0x00000001	/* non-blocking send/rcv */
#define CSLSIGRSTRT	0x00000002	/* restart rcv on interrupt */
#define CSLNOREPLY	0x00000004	/* no reply expected */
#define CSLNOTRAN	0x00000008	/* not sent in transaction mode */
#define CSLTRAN		0x00000010	/* sent in transaction mode */
#define CSLNOTIME	0x00000020	/* no timeout */
#define CSLABSOLUTE	0x00000040	/* absolute value on tmsetprio */
#define CSLGETANY	0x00000080	/* get any valid reply */
#define CSLNOCHANGE	0x00000100	/* force incoming buffer to match */
#define RESERVED_BIT1	0x00000200	/* reserved for future use */
#define CSLCONV		0x00000400	/* conversational service */
#define CSLSENDONLY	0x00000800	/* send-only mode */
#define CSLRECVONLY	0x00001000	/* recv-only mode */
#define CSLACK		0x00002000	/* */

/* Flags to wwinit() */

#define CSLU_SIG		0x00000001	/* 通过信号选择未被主动请求的通告 */
#define CSLU_DIP		0x00000002	/* 通过dip-in选择未被主动请求的通告 */
#define CSLU_IGN		0x00000004	/* 忽略未被主动请求的通告 */

#define CSLSA_FASTPATH	0x00000008	/* System access == fastpath */
#define CSLSA_PROTECTED	0x00000010	/* System access == protected */

/* 声明rval取值*/
#define CSLFAIL	    0x00000001	        /* service FAILure for tpreturn */
#define CSLSUCCESS   0x00000002	        /* service SUCCESS for tpreturn */
#define CSLEXIT	    0x08000000	        /* service failue with server exit */



/* 声明应用程序信息结构 */
typedef  TPINIT    APPINIT;
typedef struct tpsvcinfo CSLSVCINFO;

/*************************************
*	FUNCTIONS DECLARE            *
**************************************/
/*
所有函数执行成功返回值>=0;失败则返回-ErrNo.ErrNo为错误代码
*/

extern "C"
{
int csllogin(int AppID, APPINIT *AppInfo);/*AppID	应用程序类型	AppInfo	应用程序信息*/
int csllogout(void);
int cslbegin(void);
int cslcommit(void);
int cslrollback(void);

/* 
svc	Service名字
idata	发送缓冲区地址
ilen	发送的数据包长度
odata	接收缓冲区的地址指针
olen	接收数据包长度的地址
*/
int cslcall(char *svc, char *idata, long ilen, char **odata, long *olen);

#ifdef COMMON

/*
rval	处理结果标志
rcode	返回代码
data	返回的数据包
len	返回的数据包长度
*/
int cslreturn_common(int rval,long rcode,char *data,long len);

/*
svc	请求的服务名
data	转发的数据包
len	转发的数据包长度
*/
int cslsvrfwd(char *svc,char *data,long len);

int cslopen_common();
int cslclose_common();

#endif /*COMMON*/

/*
svc	请求的服务名
data	发送的数据包
len	发送数据包长度
flags	控制标志
*/
int cslconnect(char *svc,char *data,long len,long flags);

int csldiscon(int cd);/* cd:由wwConnect()返回的会话描述符 */

/*
cd	由wwConnect()返回的会话描述符
data	发送的数据包
len	发送的数据包长度
flags	控制标志
*/
int cslsend(int cd,char *data,long len,long flags);

/*
cd	由wwConnect()返回的会话描述符
data	接收的数据包
len	接收的数据包长度
flags	控制标志
*/
int cslrecv(int cd,char **data,long *len,long flags);

/*
type	缓冲区类型
size	缓冲区大小
*/
char * cslalloc(char *type ,long size);


/*ptr:wwalloc()返回的地址指针*/
void  cslfree(char *ptr);

/*errno:错误号*/
char * cslerrstr(int ierrno);

}

#endif /*MID_COMPONENT_COMMON_INTERFACE_H*/

