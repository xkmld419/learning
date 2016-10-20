/*VER: 1*/ 
#ifndef NETWORKMGR_H_HEADER_INCLUDED_BDB745F9
#define NETWORKMGR_H_HEADER_INCLUDED_BDB745F9
/*
**
**  根据网络来源类型确定查找网络平台的优先级顺序
**  数字大的优先级别高，优先匹配用户档案
**  对于网络来源类型不明确的（m_iNetworkSourceID为0）按照网络平台的优先级别从高到低查找
**  2007.12.12 caidk 
**      created
*/

#include "CommonMacro.h"
#include "HashList.h"
class NetworkPriority
{
public:
    NetworkPriority()
    {
        Init(0,0);
    }
    NetworkPriority(int source, int networkid)
    {
    		Init(source,networkid);
    }
    bool Init(int source,int networkid)
    {
    	  m_iNetworkSourceID = source;
        m_iNetworkID = networkid;
        m_iPriority = 0;
        m_poNext = 0;    		
    		return true;
    }
    //网络来源类型标识
    int m_iNetworkSourceID;
    //网络类型标识
    int m_iNetworkID;
    //优先级
    int m_iPriority;
    //下一条记录
    NetworkPriority * m_poNext;
    
    static NetworkPriority NETWORK_NONE;
    
#ifndef PRIVATE_MEMORY
	unsigned int m_iNextOffset;
#endif

};
class NetworkPriorityMgr
{
    //网络类型优先级别
    NetworkPriority * m_poNetworkPriList;
    //网络类型优先级别的索引根据m_iNetworkSourceID做索引
    HashList<NetworkPriority *> *m_poNetworkPriIndex;
    
    bool m_bUploaded;
    
public:
    //## 加载定商品，建立相关索引
    void load();

    //## 卸载所加载的信息
    void unload();
    
    NetworkPriority * getPriority(int iNetworkSourceID);
    
#ifndef PRIVATE_MEMORY
	
	NetworkPriority *getHeadNetworkPriority();

#endif 
    
    NetworkPriorityMgr();
    
    ~NetworkPriorityMgr();
};
#endif
