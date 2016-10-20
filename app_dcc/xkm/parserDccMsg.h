#ifndef _PARSER_DCC_MESG_H_
#define _PARSER_DCC_MESG_H_

#include "abmobject.h"
#include "abmcmd.h"
#include "Application.h"
#include "MessageQueue.h"
#include "LogV2.h"
#include "DccMsgParser.h"
#include "dccpublic.h"
#include "dcc_ra.h"
#include "../src/ReadAppConf.h"


class ParserMsgMgr : public Application {

public:
	
	ParserMsgMgr() {};
	
	virtual ~ParserMsgMgr() { if (m_poDcc!=NULL) delete m_poDcc; };
    
	virtual int init(ABMException &oExp);
	
	virtual int run();
	
	virtual int destroy();
	
private:
    
    DccMsgParser *m_poDcc;
	
};

DEFINE_MAIN(ParserMsgMgr)

#endif
