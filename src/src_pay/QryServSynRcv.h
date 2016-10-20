#ifndef __QRYSERVSYNRCV_H_INCLUDED_
#define __QRYSERVSYNRCV_H_INCLUDED_

/*
*资料同步短信下发处理类
*/
//#include "ABMSyn.h"
#include "ReadAppConf.h"
#include "MessageQueue.h"
#include "QryServSynSql.h"
#include "LogV2.h"
#include "abmcmd.h" 

class QryServSynRcv
{

public:

    QryServSynRcv();

    ~QryServSynRcv();

    int init(ABMException &oExp);
	
    int deal(QryServSynCCA *oCCA);			//资料同步短信下发

private:
	 int _deal(QryServSynData *pData,char *m_oSessionId);    

private:

    QryServSynSql *m_poSql;
	
	QryServSynCCA *oCCA;
};



#endif/*__QRYSERVSYNRCV_H_INCLUDED_*/