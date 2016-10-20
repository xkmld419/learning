#ifndef BILLING_NODE_INFO_H_INTELLIGENT_SCHEDULER_PLATFORM
#define BILLING_NODE_INFO_H_INTELLIGENT_SCHEDULER_PLATFORM

#include <map> 
#include <vector>

#include "EventSection.h"
#include "FormatMethod.h"
#include "HashList.h"

using namespace std;

class BillingNodeInfo
{
	public:
		BillingNodeInfo();
		~BillingNodeInfo();
		
		int m_iNodeID;
		int m_iParentNodeID;
		int m_iOrgID;
		int m_iConditionID;
		
		BillingNodeInfo *m_poNext;
};

class BillingNodeInfoMgr
{
	public:
		BillingNodeInfoMgr();
		~BillingNodeInfoMgr();
		
		bool getNodeID(EventSection *pEvent);
	private:
		map<int, BillingNodeInfo*> m_mapOrg_NodeInfo;
		map<int, BillingNodeInfo*>::iterator m_mIter;
    #ifdef PRIVATE_MEMORY
		bool load();
		bool unload();
    #endif
		static bool m_bIsLoad;
};

class ProcessNodeInfo
{
	public:
		ProcessNodeInfo();
		ProcessNodeInfo(int iProcessNodeID);		
		~ProcessNodeInfo();
		
		bool init(int iProcessNodeID);
	public:
		int m_iNodeID;
		int m_iProcessNodeID;
		int m_iFilePoolID;//这个进程级节点发送的文件池下面的文件
		int m_iListenPortID;//使用这个监听端口
};

//------------------
//先放这儿 有空剥离
class SwitchInfoMgr
{
	public:
		SwitchInfoMgr();
		~SwitchInfoMgr();
		
		TSwitchInfo * getSwitchInfo(int iFileType, int iNodeID);
	private:
		HashList<TSwitchInfo *> *loadAreaSwithInfo(int iNodeID);		
			
		HashList<HashList<TSwitchInfo *> *> *m_pAreaCodeSwithInfo;								
};

#endif
