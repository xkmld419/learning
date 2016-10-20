/*
*   version abm-center 1.0 at 20110504
*/
#ifndef    __ABMOBJECT_H_INCLUDED_
#define    __ABMOBJECT_H_INCLUDED_

#include <vector>
#include <memory.h>
#include <iostream>
#include "LogV2.h"

//using namespace std;

#define ABMCMD_BUF_MAX  61440
#define ABMCMD_DCC_MAX  sizeof(struDccHead)
#define ABMCMD_OFFSET   (ABMCMD_DCC_MAX + 40)
#define ABMCMD_BUF_SIZE (ABMCMD_BUF_MAX-ABMCMD_OFFSET)

typedef long        __DFT_T1_;
typedef __DFT_T1_   __DFT_T2_;
typedef __DFT_T1_   __DFT_T3_;


//Dcc消息公共部分
struct struDccHead {
    char m_sSessionId[64];    //一次会话的唯一标识，主动请求的消息需要业务填写
    char m_sSrvTextId[64];    //通过这个字段区分业务消息类型
    char m_sOutPlatform[64];  //SGW必须填写外部平台的相关信息
    
    char m_sOrignHost[64];
    char m_sOrignRealm[32];
    char m_sDestRealm[32];   //主动请求的消息需要业务填写
    
    char m_sSrvFlowId[192];  //编码格式：sn-qqqwe-aaabbbcccddd-version-flowid-track
                             //          24   5     12         max<40>   6    <>;<>;.... 
    int  m_iAuthAppId; 
    int  m_iReqType;
    int  m_iReqNumber;
    int  m_iResultCode;
	unsigned int m_iHH;
    unsigned int m_iEE;
    
#ifdef _DEBUG_DCC_    
    void print()
    {
        cout<<"==========================="<<endl;
        cout<<"public message of dcc head:"<<endl;
        cout<<"  session-id---------"<<m_sSessionId<<endl;
        cout<<"  service-context-id-"<<m_sSrvTextId<<endl;
        cout<<"  outp-latform-id----"<<m_sOutPlatform<<endl;
        cout<<"  orgin-host---------"<<m_sOrignHost<<endl;
        cout<<"  orgin-realm--------"<<m_sOrignRealm<<endl;
        cout<<"  dst-realm----------"<<m_sDestRealm<<endl;
        cout<<"  Service-Flow-Id----"<<m_sSrvFlowId<<endl;
        cout<<"  auth-app-id--"<<m_iAuthAppId<<",request-type--"<<m_iReqType
            <<",request-number--"<<m_iReqNumber<<",result-code--"<<m_iResultCode<<endl;
        return;
    }
#endif

};

/***************************************************************************
*	类名：ABMObject
*	功能：定义了ABM基本数据结构体，作为接收和发送的容器
***************************************************************************/

class ABMObject 
{
    
public:
    
    ABMObject() {};
	
	ABMObject(int iCmd): m_iCmdID(iCmd) {};
	    
    void clear() {
        m_iSize = ABMCMD_OFFSET;
        m_iFirNum = 0;
        m_iSecNum = 0;
        m_iFlag = 0;
        m_iRltCode = 0;
    }

    void memcpyh(ABMObject const *pSrc) {
        if (pSrc != NULL) {
            memcpy((void *)&m_lType, (const void *)pSrc, ABMCMD_OFFSET);
            clear();
        }    
    }

    int getRltCode() {
        return m_iRltCode;
    }

    void setRltCode(int iCode) {
        m_iRltCode = iCode;
    }

public:	    
	/*消息类型*/
	long m_lType;  
	
	/*dcc消息头*/
	char m_sDccBuf[ABMCMD_DCC_MAX];  
	
	/*命令ID, 标志ABM系统内的一种处理业务*/
	int m_iCmdID;
	
	/*消息大小, 标志此条消息的总大小*/
	int m_iSize;

protected:
    	
	int m_iFirNum;
	
	int m_iSecNum;
	
	int m_iFlag;
	
	int m_iRltCode;
	
	long *m_plNum;
	
	char m_sDataBuf[ABMCMD_BUF_SIZE];
	
};

class ABMCCR : public ABMObject
{
};

class ABMCCA : public ABMObject
{
}; 

/**************************************************************************
*   类名：ABMTpl
*   功能：定义了ABM的命令体的模板类
*   说明：
*       1. __clear(): 数据结构体初始化
*       2. __add() :  封装累积量的数据
*       3. __getValue(): 获取本命令中的累积量数据
***************************************************************************/

template <typename T1, typename T2, typename T3, int CMD>
class ABMTpl : public ABMObject
{
    
public:
    
    ABMTpl() : ABMObject(CMD) {
    	clear();
    };
    
protected:  
    
    bool __add1(T1 &data) {
        if((m_iSize+sizeof(T1)) > ABMCMD_BUF_MAX)
            return false;
        if (m_iFlag != 0)
            return false;        
        memcpy((void *)&m_sDataBuf[m_iSize-ABMCMD_OFFSET], (const void *)&data, sizeof(T1));
        m_iSize += sizeof(T1);
        m_iFirNum++;
        return true;
    };
    
    bool __add2(T2 &data) {      
        if((m_iSize+sizeof(T2)+sizeof(long)) > ABMCMD_BUF_MAX)
            return false;
            
        m_iFlag = 1;     
   __DEBUG_LOG1_(0, "add2  size=[%d]",sizeof(T2));
        memcpy((void *)&m_sDataBuf[m_iSize-ABMCMD_OFFSET], (const void *)&data, sizeof(T2));
        m_iSize += sizeof(T2);
   __DEBUG_LOG1_(0, "add2  size=[%d]",sizeof(T2));
        m_iSecNum ++;
        
        m_plNum = (long *)&m_sDataBuf[m_iSize-ABMCMD_OFFSET];
        *m_plNum = 0;
        m_iSize += sizeof(long);
        return true;
    }
    
    bool __add3(T3 &data) {
        if((m_iSize+sizeof(T3)) > ABMCMD_BUF_MAX)
            return false;
        if (m_iFlag == 0) 
            return false;
        (*m_plNum) ++;
        memcpy((void *)&m_sDataBuf[m_iSize-ABMCMD_OFFSET], (const void *)&data, sizeof(T3));
        m_iSize += sizeof(T3);  
        return true;        
    }
    
    int __getValue1(vector<T1 *> &vec) {
    	int _size;
        for (int i=0; i<m_iFirNum; ++i) {
        	_size = i*sizeof(T1);
        	if (_size >= ABMCMD_BUF_SIZE)
        		return -1;
            vec.push_back((T1 *)&m_sDataBuf[_size]);  
        }   
        return vec.size();                  
    }
    
    //有明细的
    int __getValue2(vector<T2 *> &vec) {
        long *plNum;
        if (m_iSecNum == 0)
            return 0;
    	else if (m_iSecNum < 0)
    		return -1;
        //获得T2的起始地址
        int _size = m_iFirNum*sizeof(T1);
        if (_size >= ABMCMD_BUF_SIZE) 
            return -1;
        vec.push_back((T2 *)&m_sDataBuf[_size]);
        
        for (int i=1; i<m_iSecNum; ++i) {
            _size += sizeof(T2);  
            //获得T2明细T3的个数
            plNum = (long *)&m_sDataBuf[_size];
            //获得下一个T2的地址
            _size += ((*plNum) * sizeof(T3) + sizeof(long));
            if (_size > ABMCMD_BUF_SIZE) 
                return -1; 
            vec.push_back((T2 *)&m_sDataBuf[_size]);    
        }   
        
     	_size += sizeof(T2);
     	plNum = (long *)&m_sDataBuf[_size];
     	_size += ((*plNum) * sizeof(T3) + sizeof(long));
     	if (_size > ABMCMD_BUF_SIZE) 
     	    return -1;
        return vec.size();      
    }
    
    //根据优惠累积量的地址 获取相关的明细
    long __getValue3(T2 *addr, vector<T3 *> &vec) {
        long *plNum;
        plNum = (long *) (addr+1);
        T3 *pData = (T3 *)(plNum+1);
        for (int i=0; i<*plNum; ++i) {
            vec.push_back(pData);
            pData += 1; 
        }   
        return vec.size();
    }
 
};   

#endif/*__ABMOBJECT_H_INCLUDED_*/

