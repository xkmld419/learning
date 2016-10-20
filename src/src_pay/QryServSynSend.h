#ifndef __QRYSERVSYNSEND_H_INCLUDED_
#define __QRYSERVSYNSEND_H_INCLUDED_

/*
*资料同步短信下发处理类
*/
//#include "ABMSynSend.h"
#include "ReadAppConf.h"
#include "MessageQueue.h"
#include "QryServSynSql.h"
#include "LogV2.h"
#include "abmcmd.h"
#include "Application.h"
#include "DccOperation.h"
#include "DccOperationSql.h"
//#include "PublicConndb.h"

class DccOperation;

class QryServSynSend
{

public:

    QryServSynSend();

    ~QryServSynSend();

    int init(ABMException &oExp);
	
    int deal();			//资料同步短信下发

private:

    QryServSynSql *m_poSql;
    DccOperation *m_dccOperation;
	
	//QryServSynCCR *oCCR;
	
	//QryServSynCCA *oCCA;
};



#endif/*__QRYSERVSYNSEND_H_INCLUDED_*/
