#ifndef _PACK_DCC_MESG_H_
#define _PACK_DCC_MESG_H_

#include "abmobject.h"
#include "abmcmd.h"
#include "Application.h"
#include "MessageQueue.h"
#include "LogV2.h"
#include "DccMsgParser.h"
#include "dccpublic.h"
#include "ReadDccConf.h"
#include "OracleConn.h"


class PackMsgMgr : public Application {

public:
	
	PackMsgMgr() {};
	
	virtual ~PackMsgMgr() { if (m_poDcc!=NULL) delete m_poDcc; };
    
	virtual int init(ABMException &oExp);
	
	virtual int run();
	
	virtual int destroy();
	
private:
    
    DccMsgParser *m_poDcc;
	
};

DEFINE_MAIN(PackMsgMgr)

#endif
