#ifndef __SHM_SQL_MGR_H_INCLUDED_
#define __SHM_SQL_MGR_H_INCLUDED_

#include "ABMException.h"
#include "TimesTenAccess.h"
#include "TTTns.h"
#include "SHMParamStruct.h"

class SHMSqlMgr : public TTTns
{
public:

    SHMSqlMgr();

    ~SHMSqlMgr();

		int init(int iTableType,char const *sTable,bool isInsert=false);

public:

		TimesTenCMD *m_poHCodeIn;
		
		TimesTenCMD *m_poLocalHeadIn;
		
		TimesTenCMD *m_poMinIn;
		
		TimesTenCMD *m_poImsiIn;
		
		TimesTenCMD *m_poHCodeOut;
		
		//TimesTenCMD *m_poLocalHeadOut;
		
		TimesTenCMD *m_poMinOut;
		
		TimesTenCMD *m_poImsiOut;
		
		TimesTenCMD *m_poMenDb;
};

#endif/*__SHM_SQL_MGR_H_INCLUDED_*/
