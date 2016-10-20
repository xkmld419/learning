/*VER: 1*/ 
#ifndef __IPC__KEY__H___
#define __IPC__KEY__H___

#include "TOCIQuery.h"

class IpcKeyMgr {
  public:
    /*
        获取系统中定义的 当前主机(通过hostname调用获取)、
            当前操作系统用户(通过环境变量LOGNAME获取)、
            指定流程、IPC名称 对应的IPC_KEY 值
        IPC_KEY 定义在表 B_IPC_CFG 中
        成功, 返回值为获取的 IPC_KEY
        失败, 返回值 小于 0
    */
    static long getIpcKey (int _iBillFlowID, char const * _sIpcName);
    static long getIpcKeyEx (int _iBillFlowID, char const * _sIpcName);
	static long getIpcKey(bool bForceBillFlow,int _iBillFlowID, char const * _sIpcName);
  static long getNextIpcKey(int iHostID,char const * pUserName,char const * _sIpcName,long lKeyBegin,long lKeyEnd);
    // 尝试取当前流程对应的配置数据, 若找不到, 找流程ID为 -1 的.(即系统公用配置)
    static long getIpcKey (char const * _sIpcName);
    
    // 成功: 返回当前主机的ID, 主机名称_sHostName
    // 失败: 返回 -1
    static int getHostInfo (char *_sHostName);
	static int getHostInfo (char *_sHostName,TOCIQuery &qry);

    /* 
     *   添加系统中需要的当前主机,当前操作系统用户,指定流程,IPC名称对应的值 
     */
    static bool setIpcKey(int _iBillFlowID, char const * _sIpcName,char const * sRemark, int _iIpcKey);
    static bool setIpcKey(char const * _sIpcName,char const * sRemark);
    static bool seekIpcKey(int iHostID,long lIpcKey);
};

#endif

