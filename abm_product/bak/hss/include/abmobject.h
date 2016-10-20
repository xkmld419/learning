/*
*   version 1.0 js at 2010-09-09
*   v 1.1 
*         1. 增加8个字节保留字段 用宏EXTEND_ABMOBJECT 控制
*         2. 把原来分配在AccuTpl后面的16字节放到m_uBody中.
*         3. 原来在各业务类里实现的clear 放到了ABMObject.
*         4. 新增了memcpyh 用于copy
*        2,3,4 仅对累积量有效  
*/
#ifndef    __ABMOBJECT_H_INCLUDED_
#define    __ABMOBJECT_H_INCLUDED_

#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

#define BUF_SIZE_MAX 61440

#ifdef EXTEND_ABMOBJECT
#define OFFSET_BUF 64
#else
#define OFFSET_BUF 56
#endif

#define CMD_BUF_SIZE (BUF_SIZE_MAX-OFFSET_BUF)

/***************************************************************************
*	类名：ABMObject
*	功能：定义了查询ABM累积量的基本数据结构体，作为接收和发送的容器
***************************************************************************/

class ABMObject 
{
    
public:
    
    ABMObject() {};
	
	ABMObject(int cmd): m_iCmdID(cmd) {};
    //设置序列号
    void setSeqID(int iSeqID) {
        m_iSeqID = iSeqID;
    }
    
    //Accu的清理函数
    void clearA(int iCode=0) {
        m_iSize = OFFSET_BUF + sizeof(m_uBody.m_tNum);
        memset(&m_uBody.m_tNum, 0x00, sizeof(m_uBody.m_tNum));
        m_iRltCode = iCode;    
    }
    //Accu的专用函数
    void memcpyh(ABMObject const *pSrc) {
        memcpy((void *)&m_lHash, (const void *)pSrc, OFFSET_BUF);
        clearA();    
    }
		
public:
    
    /*API中根据此值,调用HASH函数 算出分发到哪个ABM业务处理点*/
    //当大小端转换时 此字段还作为是否做过转换的标志 = -1 转换过
    long m_lHash;
	
	/*客户端节点ID, 跟业务里的transID组合 ,
	  唯一确定一个事务(在socket客户端中赋值)*/
	int m_iClntID;   
	 
	/*返回时的消息队列优先级,API中赋值*/
	int m_iMqType;
	
	/*ABM内部的序列号, 只在同步接口时有用 API中赋值*/
	long m_lTransID;
	
	/*入ABM系统时间, 性能统计用*/
	long m_lInTm;
	
	/*出ABM系统时间, 性能统计用*/
	long m_lOutTm;	
	
	/*ABM业务端处理结果, 调用端可根据此字段判断ABM是否成功处理*/
	int m_iRltCode;
	
	int m_iSeqID;
	
	/*命令ID, 标志ABM系统内的一种处理业务*/
	int m_iCmdID;
	
	/*消息大小, 标志此条消息的总大小*/
	int m_iSize;
	
	#ifdef EXTEND_ABMOBJECT 
	
	/*保留字段*/
	long m_lReserval;
	
	#endif
	
	/*消息体*/
	union {	
		/*消息头*/
		struct {
			int m_iFirstNum;   //混合包中第一种业务的数目
			int m_iSecondNum;   //混合包中第二中业务的数目
			int m_iFlag;
			long *m_plNum;
		} m_tNum;
		
		/*缓冲区*/
		char m_sBuf[CMD_BUF_SIZE];
		
	} m_uBody;
	
};

typedef ABMObject ABMCmd;

class ABMResult : public ABMObject 
{

public:
    
    ABMResult() {};
    
    ABMResult(int cmd): ABMObject(cmd) {};
    
    //获得ABM返回码
	int getResultCode() {
		return m_iRltCode;
	};
	
	//获得交易序列号
	long getTransID() {
		return m_lTransID;
	};
	
	//获得ABM命令ID
	int getClassID() {
	    return m_iCmdID;    
	}
	       
    //获取序列号
    int getSeqID() {
        return m_iSeqID;
    }
};

//以文件形式返回
#define RLT_CODE_FILE_RETURN 1000


class ABMFile:public ABMResult {
public:
    char *getFileName() {
        return m_uBody.m_sBuf;
    }

     //ptr 返回当前可用指针地址
    int setFileName(char *sFileName, char *&ptr) {
        ptr = m_uBody.m_sBuf;
        m_iSize = (long)ptr - (long)this;
        int iLen = ((strlen(sFileName) >> 3) + 1) << 3;
        if (m_iSize + iLen > sizeof(*this)) {
            return -1;
        }
        strncpy(ptr, sFileName, iLen);
        ptr += iLen;
        m_iSize += iLen;
        return 0;
    }

    int setFileName(char *sFileName) {
       char *ptr = NULL;
       return setFileName(sFileName, ptr);
   }
};

#ifdef ABM_LITTLE_ENDIAN
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

#endif/*__ABMOBJECT_H_INCLUDED_*/

