#ifndef __ABM_SYN_RCV_H_INCLUDED_
#define __ABM_SYN_RCV_H_INCLUDED_

#include "abmcmd.h"
#include "QryServSynSql.h"
#include "DccOperation.h"
#include "DccOperationSql.h"

class ABMException;
class DccOperation;

/*
*   开户资料操作类
*	 全国中心HSS接收到全国ABM发送的开户资料同步请求后，返回结果CCA
*/

class ABMSynRcv
{
	public:
		ABMSynRcv();
		~ABMSynRcv();
		
		int init(ABMException &oExp);
		int deal(ABMCCR *pRecivePack, ABMCCA *pSendPack); // 处理接收的CCA 主动发CCA

	private:
		int _deal(QryServSynData *pData,char *m_oSessionId);
		int _dealErr(QryServSynData *pData,char *m_oSessionId);
		
	private:
		ABMException *oExp;
		DccOperation *m_dccOperation;
		QryServSynCCA *m_poUnpack;	//解包-全国HSS返回的CCA包 全国HSS到全国ABM
		QryServSynSql *m_poSql;
};

#endif /*__ABM_SYN_RCV_H_INCLUDED_*/
