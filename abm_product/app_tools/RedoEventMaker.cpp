/*VER: 1*/ 
#include <stdio.h>
#include <stdlib.h>
#include "Process.h"
#include "Environment.h"
#include "UserInfoCtl.h"
#include "Log.h"
#include "WorkFlow.h"

//#define DEBUG_MODE_NO_DELETE

class RedoEventMaker: public Process
{
	public:
		int run ();
		RedoEventMaker ();
		~RedoEventMaker ();
		long getNextEventSeq();
		void deleteEvent(long lEventID, char *state);
		int getNextFileID();

};

RedoEventMaker::RedoEventMaker ()
{
	Environment::useUserInfo ();
	//Environment::useEventSectionUnion ();
}

RedoEventMaker::~RedoEventMaker ()
{

}

int RedoEventMaker::run ()
{
	StdEvent event;
	long lOldEventID;
	char sSqlcode[1024];
	long lCount = 0, lBlackCount=0, lOKCount=0;
	char sSqlwhere[200];

	char sTableName[64];

	strcpy (sTableName, "redo_event");

	if (g_argc == 2) {
		sprintf (sSqlwhere, "and %s", g_argv[1]);
	} else if (g_argc == 3) {
		strcpy (sTableName, g_argv[1]);
		sprintf (sSqlwhere, "and %s", g_argv[2]);
	} else {
		strcpy (sSqlwhere, "and error_id in (2000, 4001, 4002)");
	}

	DEFINE_QUERY (qry);
	sprintf (sSqlcode, "select billing_area_code, "
				   "billing_nbr, "
				   "calling_area_code, "
				   "calling_nbr, "
				   "called_area_code, "
				   "called_nbr, "
				   "to_char(start_date, 'yyyymmddhh24miss'), "
				   "duration, "
				   "switch_id, "
				   "trunk_in, "
				   "trunk_out, "
				   "extend_field_id10, "
				   "event_id, "
				   "file_id "
			      "from %s "
			     "where 1=1 %s", sTableName,  sSqlwhere);

	qry.setSQL (sSqlcode);
	qry.open ();
	printf ("rebill black serv...\n");

	while (qry.next ()) {
		memset (&event, 0, sizeof (event));
		
		if (++lCount % 100 == 0) {
            #ifdef SHM_FILE_USERINFO
            G_SHMFILE;
            #endif
            
			Pprintf (0, false, 0, "已回收:%d, 未回收:%d", lOKCount, lBlackCount);
		}

		strcpy (event.m_sBillingAreaCode, 	qry.field(0).asString());
		strcpy (event.m_sBillingNBR, 		qry.field(1).asString());
		strcpy (event.m_sCallingAreaCode, 	qry.field(2).asString());
		strcpy (event.m_sCallingNBR, 		qry.field(3).asString());
		strcpy (event.m_sCalledAreaCode, 	qry.field(4).asString());
		strcpy (event.m_sCalledNBR, 		qry.field(5).asString());
		strcpy (event.m_sStartDate, 		qry.field(6).asString());
		event.m_lDuration = 			qry.field(7).asInteger();
		event.m_iSwitchID = 			qry.field(8).asInteger();
		strcpy (event.m_sTrunkIn, 		qry.field(9).asString());
		strcpy (event.m_sTrunkOut, 		qry.field(10).asString());
		event.m_iEventTypeID   =		qry.field(11).asLong();

		lOldEventID = 	qry.field(12).asLong();
		event.m_iFileID   =			qry.field(13).asInteger();

		EventSection *ec = (EventSection *)&event;
		ec->bindUserInfo ();
		if (ec->m_iServID < 1) {
			event.m_iErrorID = 2000;	
			lBlackCount++;
			continue;
		}
	
		event.m_lEventID = getNextEventSeq ();
		if (m_poSender->send (&event)) {
		    SEND_FILE_END_EVENT (event.m_iFileID,event.m_iProcessID);
#ifndef DEBUG_MODE_NO_DELETE
			deleteEvent(lOldEventID, "BLD");
			Log::log (0, "delete: %d %s%s %s%s %s%s %s %d %d %s %s", 
					event.m_iEventTypeID,
					event.m_sBillingAreaCode,
					event.m_sBillingNBR,
					event.m_sCallingAreaCode,
					event.m_sCallingNBR,
					event.m_sCalledAreaCode,
					event.m_sCalledNBR,
					event.m_sStartDate,
					event.m_lDuration,
					event.m_iSwitchID,
					event.m_sTrunkIn,
					event.m_sTrunkOut
					);
#endif
			lOKCount++;
		}
	}

	Pprintf (0, false, 0, 
		"回收结束, 共查找[%d]个, 转正[%d]户, 未转正[%d]户", lCount, lOKCount, lBlackCount);

	if (lOKCount>0) {
		//SEND_FILE_END_EVENT (iFileID);
	} else {
		Pprintf (0, false, 0, "没有需要转正的话单", lCount);
	}
	return 0;

}

void RedoEventMaker::deleteEvent(long lEventID, char *state)
{
	char sTemp[1024];
	strcpy (sTemp, "delete redo_event where event_id=:event_id");
	DEFINE_QUERY (qry);
	qry.setSQL (sTemp);
	qry.setParameter ("event_id", lEventID);
	qry.execute ();
	qry.commit ();

}

long RedoEventMaker::getNextEventSeq()
{
        static TOCIQuery qry(DB_LINK);

        qry.close();
        qry.setSQL("select seq_event_id.nextval event_id from dual");
        qry.open();

        if (!qry.next() ) THROW(EVENT_ID_INCREMENT);

        long lCurEventID = qry.field("event_id").asLong();

        qry.close();

        return lCurEventID;
}

int RedoEventMaker::getNextFileID()
{
        static TOCIQuery qry(DB_LINK);

        qry.close();
        qry.setSQL("select SEQ_FILE_ID.nextval file_id from dual");
        qry.open();

        if (!qry.next() ) THROW(EVENT_ID_INCREMENT);

        int iFileID = qry.field("file_id").asInteger ();

        qry.close();

        return iFileID;
}

DEFINE_MAIN(RedoEventMaker);

