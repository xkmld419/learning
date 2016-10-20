/*VER: 1*/ 
#ifndef SEQ_MGR_H
#define SEQ_MGR_H

#include "Environment.h"

class SeqMgr
{
  public:
	static long getNextVal(char const *sSeqName) {
		DEFINE_QUERY (qry);
		char sSQL[512];

		sprintf (sSQL, "select %s.nextval from dual", sSeqName);
		qry.setSQL (sSQL); qry.open (); qry.next ();
		
		long lRet = qry.field (0).asLong ();

		qry.close ();

		return lRet;
	}
};
#endif
