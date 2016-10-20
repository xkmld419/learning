/*
*   根据三个日志组写的ABM内部日志类 模仿ABMException, 主要是为了解决计费和HSS底层不兼容
*   多线程需要加锁 
*/

#ifndef __HSS_DIRECT_LOG_H_INCLUDED_
#define __HSS_DIRECT_LOG_H_INCLUDED_

#include "MBC_ABM.h"

//支持前面定义的宏
#define DIRECT_LOG directLog
#define ADD_LOG    directLog
#define FLUSH_LOG       

//addLog 参数说明

//log level iLevel 
#define V3_FATAL 1   
#define V3_ERROR 2
#define V3_WARN  3
#define V3_INFO  4
#define V3_DEBUG 5

//log type  int iTypeID
#define V3_SERVICE   1     //业务日志
#define V3_PARAM_MGR 2     //参数管理日志
#define V3_SYS_MGR   3     //系统管理日志
#define V3_SCHEDUL   4     //进程调度日志
#define V3_DB        5     //数据库关键数据操作日志

//log iErrID  参照标准错误码 errorcode.h

//进程正常启动日志

struct HSSLogStruct
{
#define HSSLOGBUF_MAX 512
    int m_iModuID;    //=appID
    int m_iProcID;
    int m_iLevel;
    int m_iTypeID;
    int m_iErrNo;    //== iCodeID
    int m_iTime;
    char m_sErrMsg[HSSLOGBUF_MAX];
};

    
    //直接日志 写入日志接口才返回
extern void directLog(int iLevel, int iTypeID, int iErrNo, char *fmt, ...);
    
    //间接日志， 写入缓存， 调用flushLog 或者累积到最大缓存数 提交到接口
    //static void addLog(int iLevel, int iTypeID, int iErrNo, char *fmt, ...);
    
    //日志刷新 将缓存中日志刷新到日志接口
   // static void flushLog();
//信息点鉴权日志专用   
extern void authLog(int iNeID, int iServiceID, char *pServiceName, char *pErrMsg);

//增加全局核心参数的获取 防止两次链接内存
class CommandCom;

extern CommandCom* getKernel();      

#endif/*__HSS_DIRECT_LOG_H_INCLUDED_*/
