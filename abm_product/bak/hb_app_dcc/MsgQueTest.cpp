#ifndef __MSG_QUE_TEST_H
#define __MSG_QUE_TEST_H
#include "Process.h"
#include "dcc_FileMgr.h"
#include "dcc_ParamsMgr.h"

class MsgQueTest : public Process
{
public:
	int run();
};

int MsgQueTest::run()
{
	char pSendMsg[61441]; // = "Hello MsgQueTest!";
	char pReceMsg[61441];

	FileToDccMgr *gpFileMgr = FileToDccMgr::getInstance();
	gpFileMgr->init();
	int iFileTypeID = gpFileMgr->getFileTypeByServiceContext("CtoG_Imsi.Para@025.ChinaTelecom.com");
	printf("filetype:%d\n",iFileTypeID);

	dccParamsMgr oParamMgr;
	char sServiceContextID[50]={0};
	sprintf(sServiceContextID,"Active.Net-Ctrl@025.ChinaTelecom.com");
	oParamMgr.getDccHeadValue(sServiceContextID);
	printf("session_id=%s,hh=%u,ee=%u\n",oParamMgr.m_sSessionID,oParamMgr.m_iHopByHop,oParamMgr.m_iEndToEnd);

	memset(pSendMsg, 0, sizeof(pSendMsg));
	strcpy(pSendMsg, "Hello MsgQueTest!");

	for (int i=0; i<10; ++i)
		m_poSender->send(pSendMsg);

	for (int i=0; i<10; ++i)
	{
		memset(pReceMsg, 0, sizeof(pReceMsg));
		m_poReceiver->receive(pReceMsg);

		printf("%s\n", pReceMsg);
	}
	return 0;
}

DEFINE_MAIN (MsgQueTest)


#endif

