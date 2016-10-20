#ifndef __TT_IMPORT_H_INCLUDED_
#define __TT_IMPORT_H_INCLUDED_

#include "TimesTenAccess.h"
#include "TTTns.h"
#include "TableStruct.h"

class TTimport //: public TTTns
{
public:

    TTimport();

    ~TTimport();

public:

		int qryXKMtest(TimesTenCMD *m_poPublicConn,vector<XKMtest *> &vXKMtest);
};

#endif/*__SHM_SQL_MGR_H_INCLUDED_*/

