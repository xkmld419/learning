/*VER: 1*/ 
#include "Environment.h"
#include "Log.h"
#include "TransactionMgr.h"
#include "Rollback.h"
#include "StdEvent.h"

Rollback::Rollback()
{
	Environment::useUserInfo ();
}

void Rollback::rollbackEventAggr(char *sTableName, char *sCondition)
{
	DEFINE_QUERY (qry);
	char sSQL[2048];
	StdEvent event;

	memset (&event, 0, sizeof (event));

	sprintf (sSQL, "select a.event_id, a.serv_id, a.billing_cycle_id, a.file_id, "
			"a.event_aggr_type_id, a.value*-1 from event_aggr_rollback a, %s b"
			" where a.event_id=b.event_id", sTableName);
	if (sCondition) {
		strcat (sSQL, " and ");
		strcat (sSQL, sCondition);
	}

	qry.setSQL (sSQL); qry.open ();
	while (qry.next ()) {
		event.m_lEventID = qry.field (0).asLong ();
		event.m_iServID = qry.field (1).asLong ();
		event.m_iBillingCycleID = qry.field (2).asInteger ();
		event.m_iFileID = qry.field (3).asInteger ();

		G_PTRANSMGR->insertEventAggr (&event, qry.field (4).asInteger (), qry.field (5).asInteger ());

		if (G_PTRANSMGR->ifFull ()) {
			G_PTRANSMGR->commit ();
		}
	}

	G_PTRANSMGR->commit ();

	qry.close ();

	sprintf (sSQL, "delete from event_aggr_rollback where event_id in (select event_id from %s",
							 sTableName);
	if (sCondition) {
		strcat (sSQL, " where "); strcat (sSQL, sCondition); strcat (sSQL, ")");
	} else {
		strcat (sSQL, ")");
	}
	qry.setSQL (sSQL); qry.execute (); qry.commit (); qry.close ();

}

void Rollback::rollbackAcctAggr(char *sTableName, char *sCondition)
{
	DEFINE_QUERY (qry);
	char sSQL[2048];
	StdAcctItem acctitem;

	int iCount = 0;

	memset (&acctitem, 0, sizeof (acctitem));

	sprintf (sSQL, "select a.event_id, a.serv_id, a.billing_cycle_id, a.file_id, "
			"a.acct_item_type_id, a.charge*-1 from acct_item_aggr_rollback a, %s b"
			" where a.event_id=b.event_id", sTableName);
	if (sCondition) {
		strcat (sSQL, " and ");
		strcat (sSQL, sCondition);
	}

	qry.setSQL (sSQL); qry.open ();
	while (qry.next ()) {
		acctitem.lEventID = qry.field (0).asLong ();
		acctitem.iServID = qry.field (1).asLong ();
		acctitem.iBillingCycleID = qry.field (2).asInteger ();
		acctitem.iFileID = qry.field (3).asInteger ();
		acctitem.iAcctItemTypeID = qry.field (4).asInteger ();
		acctitem.iCharge = qry.field (5).asInteger ();

                if (G_PUSERINFO->getServ (G_SERV, acctitem.iServID, sRollDate)) {
                        acctitem.iOrgID = G_SERV.getOrgID ();
                } else {
                        acctitem.iOrgID = 1;
                }

		G_PTRANSMGR->insertAcctItemAggr (&acctitem);
		iCount++;

		if (iCount == 5000) {
			G_PTRANSMGR->setForceCommit ();
			G_PTRANSMGR->commit ();
			iCount = 0;
		}
	}

	G_PTRANSMGR->setForceCommit ();
	G_PTRANSMGR->commit ();

	qry.close ();

	DEFINE_QUERY (qry1);

	sprintf (sSQL, "delete from acct_item_aggr_rollback where event_id=:EVENT_ID");
	qry1.setSQL (sSQL);
	long lBuf[10000];

	memset (lBuf, 0, sizeof (lBuf));

	sprintf (sSQL, "select event_id from %s", sTableName);
	if (sCondition) {
		strcat (sSQL, " where "); strcat (sSQL, sCondition); 
	} else {
	}
	qry.setSQL (sSQL); qry.open ();

	while (qry.next ()) {
		lBuf[iCount++] = qry.field (0).asLong ();

		if (iCount == sizeof (lBuf) / sizeof (long)) {
			qry1.setParamArray ("EVENT_ID", &(lBuf[0]), sizeof (long));
			qry1.execute (iCount);
			qry1.commit ();
			iCount = 0;
		}
	}

	if (iCount) {
		qry1.setParamArray ("EVENT_ID", lBuf, sizeof (long));
		qry1.execute (iCount);
		qry1.commit ();
	}

	qry.close ();
}

void Rollback::rollbackEvent(char *sTableName, char *sCondition)
{
	DEFINE_QUERY (qry);
	char sSQL[2048];
	
	if (sCondition) {
		sprintf (sSQL, "delete from %s where %s", sTableName, sCondition);
	} else {
		sprintf (sSQL, "truncate table %s", sTableName);
	}

	qry.setSQL (sSQL); qry.execute (); qry.commit ();
}

bool Rollback::rollback(char * sTableName, char *sCondition)
{

//	rollbackEventAggr (sTableName, sCondition);
	rollbackAcctAggr (sTableName, sCondition);
	rollbackEvent (sTableName, sCondition);

	return true;
}
