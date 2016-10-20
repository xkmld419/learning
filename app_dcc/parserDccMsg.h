#ifndef _PARSER_DCC_MESG_H_
#define _PARSER_DCC_MESG_H_

#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include "abmobject.h"
#include "abmcmd.h"
#include "Application.h"
#include "MessageQueue.h"
#include "LogV2.h"
#include "DccMsgParser.h"
#include "dccpublic.h"
#include "ReadDccConf.h"
//#include "DccSql.h"
#include "OracleConn.h"


class ParserMsgMgr : public Application {

public:
	
	ParserMsgMgr(): m_poDcc(0), m_poSql(0) {}
	
	virtual ~ParserMsgMgr() 
	{
	    if (m_poDcc!=NULL) delete m_poDcc;
	    if (m_poSql!=NULL) delete m_poSql;
    }
    
	virtual int init(ABMException &oExp);
	
	virtual int run();
	
	virtual int destroy();
	
private:
    
    DccMsgParser *m_poDcc;
    
    DccSql *m_poSql;
	
};

DEFINE_MAIN(ParserMsgMgr)

#endif
