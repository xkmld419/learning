/*VER: 1*/ 
#include "VirtualFileMaker.h"
#include "Environment.h"
#include "SeqMgr.h"

int VirtualFileMaker::makeVirtualFile(char const *sFileName, int iProcessID)
{
	DEFINE_QUERY (qry);
	char sSQL[4096];

	int iFileID = SeqMgr::getNextVal ("seq_file_id");

	sprintf (sSQL, "insert into b_event_file_list "
	    "(FILE_ID,FILE_NAME,SWITCH_ID,FILE_SIZE,CREATED_DATE,STATE,STATE_DATE,"
	    "START_SEQ,RECORD_CNT,PROCESS_ID,OUTPUT_FLOW_ID,SOURCE_TYPE)"
	    "values (%d, '%s',"
	    "-1, -1, sysdate, 'END', sysdate, -1, -1, %d, null, -1)",
	    iFileID, sFileName, iProcessID);
	qry.setSQL (sSQL); qry.execute (); qry.close (); qry.commit ();

	return iFileID;
}

void VirtualFileMaker::setSourceType(int iFileID, int iType)
{
	DEFINE_QUERY (qry);
	char sSQL[4096];

	sprintf (sSQL, "update b_event_file_list set source_type=%d, state_date=sysdate where file_id=%d",
			iType, iFileID);
	qry.setSQL (sSQL); qry.execute (); qry.close (); qry.commit ();
}

void VirtualFileMaker::setFileState(int iFileID, char const *sState)
{
	DEFINE_QUERY (qry);
	char sSQL[4096];

	sprintf (sSQL, "update b_event_file_list set state='%s', state_date=sysdate where file_id=%d",
			sState, iFileID);
	qry.setSQL (sSQL); qry.execute (); qry.close (); qry.commit ();
}
