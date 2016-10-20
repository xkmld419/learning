/*
*   version js at 2010-09-09
*/
#ifndef    __HSSOBJECT_H_INCLUDED_
#define    __HSSOBJECT_H_INCLUDED_

#include <arpa/inet.h>
#include <iostream>

#define CMD_BUF_MAX  6144000
#define CMD_DCC_MAX  568
#define CMD_OFFSET   (CMD_DCC_MAX + 104)
#define CMD_BUF_SIZE (CMD_BUF_MAX-CMD_OFFSET)


struct DccHeadInfo
{
	char m_sSessionID[128];
	char m_sOrignHost[64];
	char m_sSrvTextId[64];       //通过这个字段区分业务消息类型
	char m_sSrvFlowId[256];
	char m_sDestRealm[32];       //主动请求的消息需要业务填写	
	int  m_iAuthAppId;
	int  m_iReqType;
	int  m_iReqNumber;
	int  m_iResultCode;
	int  m_iHH;
	int  m_iEE;
	
#ifdef _DEBUG_DCC_    
    void print()
    {
        std::cout<<"==========================="<<std::endl;
        std::cout<<"public message of dcc head:"<<std::endl;
        std::cout<<"  session-id--------- "<<m_sSessionID<<std::endl;
        std::cout<<"  service-context-id- "<<m_sSrvTextId<<std::endl;        
        std::cout<<"  orgin-host--------- "<<m_sOrignHost<<std::endl;
        //std::cout<<"  orgin-realm-------- "<<m_sOrignRealm<<std::endl;
        std::cout<<"  dst-realm---------- "<<m_sDestRealm<<std::endl;
        std::cout<<"  Service-Flow-Id---- "<<m_sSrvFlowId<<std::endl;
        std::cout<<"  auth-app-id-- "<<m_iAuthAppId<<",request-type-- "<<m_iReqType
                 <<",request-number-- "<<m_iReqNumber<<",result-code-- "<<m_iResultCode<<std::endl;
        return;
    }
#endif
};

/***************************************************************************
*	类名：HSSObject
*	功能：定义了查询HSS累积量的基本数据结构体，作为接收和发送的容器
***************************************************************************/

class HSSObject 
{
    
public:
    
  HSSObject() {};
	
	HSSObject(int cmd): m_iCmdID(cmd) {};

    void clear()
    {
        m_iSize = CMD_OFFSET + sizeof(m_uBody.m_tNum);
        memset(&m_uBody.m_tNum, 0x00, sizeof(m_uBody.m_tNum));
    }
    
    void memcpyh(HSSObject const *pSrc) {
        if (pSrc != NULL) {
            memcpy((void *)&m_lType, (const void *)pSrc, CMD_OFFSET);
            clear();
        }
    }
		
public:
    
    long m_lType;
    
    /*dcc消息头*/
	char m_sDccBuf[CMD_DCC_MAX];
     	
	/*消息大小, 标志此条消息的总大小*/
	int m_iSize;

	/*命令ID, 标志HSS系统内的一种处理业务*/
	int m_iCmdID;

	/*HSS内部的序列号, 只在同步接口时有用 API中赋值*/
	long m_lTransID;
	

	int m_iMsgSource;//消息来源：1.文件 ，2.DCC（传输的消息的来源，是从文件还是dcc转换过来）

	int m_iMsgType;//消息类型：1.文件，2消息（传输的是文件，还是消息）

		/*客户端节点ID, 跟业务里的transID组合 ,
	  唯一确定一个事务(在socket客户端中赋值)*/
	int m_iClntID;  

	int m_iSrvID;//服务节点	

	/*入HSS系统时间, 性能统计用*/
	long m_lInTm;

	/*出HSS系统时间, 性能统计用*/
	long m_lOutTm;

    /*返回错误*/
    int  m_iRltCode;

    /*业务级的错误码，如果不用，则用来对齐*/
    int  m_iSrvRltCode;
	int m_iFlag;
	int m_iFlag2;
	int *m_pSrvSize;
	int *m_pDataSize;
	long *m_pTableNum;
	long *m_pDataNum;
	/*消息体*/
	union {	
		/*消息头*/
		struct {
			int m_iFirstNum;   //混合包中第一中业务的数目（OrderInfo）
			int m_iSecondNum;   //混合包中第二中业务的数目（xmlInfo）
		} m_tNum;
		
		/*缓冲区*/
		char m_sBuf[CMD_BUF_SIZE];
		
	} m_uBody;
	
};

/***************************************************************************
*   DccOrderInfo
*   功能：定义了DCC协议中订单请求的数据结构体
***************************************************************************/



class HSSCCR : public HSSObject
{
};

class HSSCCA : public HSSObject
{
};
//typedef HSSObject HSSCCR;
//typedef HSSObject HSSCCA;

#ifdef HSS_LITTLE_ENDIAN
//高地位转换
inline long htonl64(long dig)
{   	
	union {
		long l64;
		struct {
			int a1;
			int a2;	
		} st64;	
	} u1, u2;
	
	u1.l64 = dig;
	u2.st64.a1 = htonl(u1.st64.a2);
	u2.st64.a2 = htonl(u1.st64.a1);
	return u2.l64;
};
#else
#define htonl64
#endif

#endif/*__HSSOBJECT_H_INCLUDED_*/

