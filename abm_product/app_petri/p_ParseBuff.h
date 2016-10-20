/**********************************************************
 * 原有代码不好扩展，故从原有代码中拷贝一份过来
 *
ClassName: P_ParseBuff
Function : 解析/格式化 内部定义格式的 进程间 通讯字符串
comment  : 格式化串的组成单位格式为 "<可识别域名=域值>"
  可识别的域名称有--
   iBillFlowID/iProcessID/iHostID/sHostAddr/iProcessCnt/iMsgType
   iStaffID/iPlanID/iLogMode/iLogID/iAuthID/iMainGuardIndex/iResult
   iOperation(指令: 1-start 2-stop 3-ChangeStatus 4-Refresh)
   cState(A-未启动 R-运行中 E-运行结束 X-异常 K-前台调度界面发送停止命令)
   sMsg
Restrict : 
 1.只有 "sMsg" 域重复出现在格式串中是合法的
 2."sMsg" 外的其他域重复出现时,解析器将用该域最后一次出现的值覆盖前面的值
 3.以 "i" 开头的可识别域名对应的域值必需是数值型的字符串,如2046(不加引号)
 4.以 "s" 开头的可识别域名对应的域值可以是任意字符串,但不能含:"<",">","="
 5.域名大小写敏感
 6."iProcessCnt" 是当前进程处理的事件量(无符号长整型),其他以"i"开头为整型
Demo     : <iBillFlowID=3><iProcessID=2046>...<sMsg=Str_1>...<sMsg=Str_N>
---------------------------------------------------------*/

#ifndef _P_PARSEBUFF_H_
#define _P_PARSEBUFF_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "WfPublic.h"

//#define  MAX_PARSEBUFF_LEN 1024*4


#define DEFAULT_BUFF_LEN  1024
#define MSG_MAXLEN        256  
#define MSG_MAXCNT        100 



//const int MEMBER_NULL = -99;
//const int USLEEP_LONG = 10000;
//const int MAX_SQL_LEN = 2048;
//const int ORGID_STR_LEN = 256;
//const int MAX_ORG_CNT = 100;
//const int OUT_MSG_MIN_CNT = 3;



namespace PETRI {

void AllTrim(char sBuf[]);

const int MEMBER_NULL = -99;
//#define MEMBER_NULL = -99;

class P_ParseBuff{

	/*
	static inline P_ParseBuff& instance(){
		static P_ParseBuff pbuff;
		return pbuff;
	}
	*/

    
private:
    bool parse(); //if (m_str) parse it.
    void formatBuff(); //将 m_sMsg 缓冲中的分隔符替换掉
    void parseResultSave(char *pAtt, char *pValue);
    bool build(); //将内部成员变量build成字符流 => m_Str
    
    unsigned long stringToUL (char *pValue);
    
    char m_Str[MAX_PARSEBUFF_LEN+1]; //存放封装好的字符串

		//新增的成员
  	unsigned long	m_lTransactionID;
		unsigned long	m_lTransitionID;
		unsigned long m_lDrawID;
		int m_iNeedAck; //0 不需要回复 1 需要回复
		char	m_sSrcState[3];
		char	m_sDescState[3];
		char	m_sStartTime[15];
		char	m_sEndTime[15];

   
    int  m_iBillFlowID;
    int  m_iProcessID;
    unsigned long m_lProcessCnt;
    unsigned long m_lNormalCnt;
    unsigned long m_lErrorCnt;
    unsigned long m_lBlackCnt;
    unsigned long m_lOtherCnt;
    int  m_iHostID;
    char m_sHostAddr[16];
    char m_sSysUserName[32];
    int  m_iStaffID;
    int  m_iPlanID;
    int  m_iLogMode;
    int  m_iLogID;
    int  m_iAuthID;
    int  m_iMainGuardIndex;
    int  m_iOperation; // 1-start 2-stop  3-ChangeStatus 4-Refresh
    char m_cState;     //CHANGE_APPSTATUS (A-未启动 R-运行中 E-运行结束 X-异常 K-前台调度界面发送停止命令))
    int  m_iResult;    //MONITOR_LOGIN
    char m_sOrgIDStr[ORGID_STR_LEN+1];
    char m_sMsg[MSG_MAXCNT][MSG_MAXLEN+1]; //应用文本信息
		int  m_iMsgOff;    //消息缓冲m_sMsg的下条消息存储偏移量


public:

    void reset(); //数值型的成员变量初始化为 MEMBER_NULL, 字符串为空
    bool parseBuff(char *sInBuff); //sInBuff => m_Str and parse it.

    int  getBillFlowID() { return  m_iBillFlowID; }
    int  getProcessID()  { return  m_iProcessID; }
    int  getHostID()     { return  m_iHostID; }
    int  getStaffID()    { return  m_iStaffID; }
    int  getPlanID()     { return  m_iPlanID; }
    int  getLogMode()    { return  m_iLogMode; } 
    int  getLogID()      { return  m_iLogID; } 
    int  getAuthID()     { return  m_iAuthID; }
    int  getOperation()  { return  m_iOperation; } 
    int  getResult()     { return m_iResult; }

		unsigned long getTransactionID() {return m_lTransactionID; }
		unsigned long getTransitionID (){return m_lTransitionID; }
		unsigned long getDrawID (){return m_lDrawID; }

    unsigned long getProcessCnt() { return m_lProcessCnt; }
    unsigned long getNormalCnt() { return m_lNormalCnt; }
    unsigned long getErrorCnt() { return m_lErrorCnt; }
    unsigned long getBlackCnt() { return m_lBlackCnt; }
    unsigned long getOtherCnt() { return m_lOtherCnt; }

		//
		void getSrcState(char *sState) { if (sState) strcpy(sState,m_sSrcState); }
		void getDescState(char *sState) { if (sState) strcpy(sState,m_sDescState); }
		void getStartTime (char *sTime ) {if (sTime) strcpy(sTime,m_sStartTime); }
		void getEndTime (char *sTime) {if (sTime) strcpy(sTime,m_sEndTime);}
		int getNeedAck() {return m_iNeedAck;}

    
    int  getMainGuardIndex()      { return m_iMainGuardIndex; }
    void getHostAddr(char *ip)    { if (ip) strcpy(ip,m_sHostAddr); }
    void getSysUserName(char *usr)    { if (usr) strcpy(usr,m_sSysUserName); }
    void getOrgIDStr(char *str)   { if (str) strcpy(str,m_sOrgIDStr); }
    char const *getMsg(int iOffSet = 0)  {
        if (iOffSet <= m_iMsgOff)
            return m_sMsg[iOffSet];
        else return "";        
    }
    char getState()  { return m_cState; }
    
    void setBillFlowID(int id)  { m_iBillFlowID = id;}
    void setProcessID(int id)   { m_iProcessID = id;}
    void setHostID(int id)      { m_iHostID = id;}
    void setHostAddr(char* str) {
        strncpy(m_sHostAddr,str,15); 
        m_sHostAddr[15] = 0;
        AllTrim(m_sHostAddr);
    }
    void setSysUserName(char* str) {
        strncpy(m_sSysUserName,str,31); 
        m_sSysUserName[31] = 0;
        AllTrim(m_sSysUserName);
    }
    void setOrgIDStr(char* str) {
        strncpy(m_sOrgIDStr,str,ORGID_STR_LEN);
        m_sOrgIDStr[ORGID_STR_LEN] = 0;
        AllTrim(m_sOrgIDStr);
    }
    void setLogID(int id)  { m_iLogID = id;}
    void setAuthID(int id) { m_iAuthID = id;}
    void setResult(int id) { m_iResult = id;}
    void setStaffID(int id) { m_iStaffID = id;}
    void setPlanID(int id) { m_iPlanID =  id;}
    void setOperation(int id) { m_iOperation = id;}
    void setMainGuardIndex(int id) { m_iMainGuardIndex = id;}
    void setProcessCnt(unsigned long id) {m_lProcessCnt = id;}
    void setNormalCnt(unsigned long id) {m_lNormalCnt = id;}
    void setErrorCnt(unsigned long id) {m_lErrorCnt = id;}
    void setBlackCnt(unsigned long id) {m_lBlackCnt = id;}
    void setOtherCnt(unsigned long id) {m_lOtherCnt = id;}
    
    void setState(char _cState) { m_cState = _cState; }


		//begin
		void setTransactionID(unsigned long id) {m_lTransactionID = id;}
		void setTransitionID(unsigned long id) {m_lTransitionID = id;}
		void setDrawID(unsigned long id) {m_lDrawID = id;}
		void setNeedAck(int id) {m_iNeedAck = id;}
		void setSrcState(char* str) {
			strncpy(m_sSrcState,str,sizeof(m_sSrcState)-1);
			m_sSrcState[ sizeof(m_sSrcState)-1] = 0;
			AllTrim(m_sSrcState);
		}
 		void setDescState(char* str) {
			strncpy(m_sDescState,str,sizeof(m_sDescState)-1);
			m_sDescState[ sizeof(m_sDescState)-1] = 0;
			AllTrim(m_sDescState);
		}
 		void setStartTime(char* str) {
			strncpy(m_sStartTime,str,sizeof(m_sStartTime)-1);
			m_sStartTime[ sizeof(m_sStartTime)-1] = 0;
			AllTrim(m_sStartTime);
		}
 		void setEndTime(char* str) {
			strncpy(m_sEndTime,str,sizeof(m_sEndTime)-1);
			m_sEndTime[ sizeof(m_sEndTime)-1] = 0;
			AllTrim(m_sEndTime);
		}        
		//end
	
    void addMsg(const char *msg); //压入文本消息到m_sMsg
    bool getBuiltStr(char *sOutBuff); //获取m_Str
    char *getBuiltMsg(); //打包m_sMsg <sMsg=...>...<sMsg=...> 

//protect:
    P_ParseBuff(); 
    //P_ParseBuff(char *sInBuff); //sInBuff => m_Str.

};

}
#endif
