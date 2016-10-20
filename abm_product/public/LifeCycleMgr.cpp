/*VER: 1*/ 
#include "LifeCycleMgr.h"
#include "TOCIQuery.h"
#include "Environment.h"
#include <string.h>
#include "Date.h"
#include "UserInfo.h"

#include "MBC.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

//##ModelId=424E14A5030D
bool LifeCycle::check(char *sDate)
{
	if (strcmp(sDate, m_sEffDate) >= 0 &&
		strcmp(sDate, m_sExpDate) < 0 )
		return true;

	return false;
}

bool LifeCycle::check(char *sDate, ProdOfferIns *pOfferInst)
{
    switch (m_iCycleType) {
      case ABSOLUTE_TYPE:
        return (strcmp(sDate, m_sEffDate)>=0 &&
                    strcmp (sDate, m_sExpDate)<0);
    
      case ABSOLUTE_NOYEAR_TYPE: {
        char sTemp[12];
        strncpy (sTemp, sDate+4, 4);
        sTemp[4] = 0;

        int iTemp = atoi (sTemp);

        if (m_iEffValue <= m_iExpValue) {
            return (iTemp>=m_iEffValue && iTemp<m_iExpValue);
        } else {
            return (iTemp>=m_iEffValue || iTemp<m_iExpValue);
        }
      }

      default:
        break;
    }

    int iDiffValue;
    char const *sEffDate = pOfferInst->m_sEffDate;

    Date dn(sDate);
    Date dold((char*)sEffDate);

    bool bRet;

    if (m_iCycleType == MOD_MONTH_TYPE) {
        iDiffValue = dn.diffStrictMon (dold);
        if (iDiffValue % m_iEffValue == m_iExpValue) {
            return true;
        } else {
            return false;
        }
    } else if (m_iCycleType == MOD_CYCLE_TYPE) {
        BillingCycleMgr bcm;

        int iBeginCycleID = bcm.getOccurBillingCycle (1, 
                                    (char *)(sEffDate))->getBillingCycleID ();

        int iNowCycleID = bcm.getOccurBillingCycle (1,
                                     sDate)->getBillingCycleID ();

        int iDiffValue = bcm.diffBillingCycle (iNowCycleID, iBeginCycleID);

        if (iDiffValue % m_iEffValue == m_iExpValue) {
            return true;
        } else {
            return false;
        }
    } else if (m_iCycleType == RELATIVE_CYCLE_TYPE) {
/*
        BillingCycleMgr bcm;

        int iBeginCycleID = bcm.getOccurBillingCycle (1, 
                                    (char *)(sEffDate))->getBillingCycleID ();

        int iNowCycleID = bcm.getOccurBillingCycle (1,
                                     sDate)->getBillingCycleID ();

        int iDiffValue = bcm.diffBillingCycle (iNowCycleID, iBeginCycleID);
*/

        
        dold.setDay (1);

        iDiffValue =dn.diffStrictMon (dold);

        return iDiffValue>=m_iEffValue && iDiffValue<m_iExpValue;

    }else if(m_iCycleType == MOD_NORMAL_MONTH_TYPE)
    {
        iDiffValue = dn.diffMon (dold);
        if (iDiffValue % m_iEffValue == m_iExpValue) 
        {
            return true;
        } else 
        {
            return false;
        }
    }

    switch (m_iCycleType) {
      case RELATIVE_YEAR_TYPE:
      case MIX_YEAR_TYPE:
        iDiffValue = dn.diffStrictYear (dold);
        break;

      case RELATIVE_MONTH_TYPE:
      case MIX_MONTH_TYPE:
        iDiffValue = dn.diffStrictMon (dold);
        break;

      case RELATIVE_DAY_TYPE:
      case MIX_DAY_TYPE:
        iDiffValue = dn.diffStrictDay (dold);
        break;

      default:
        iDiffValue = -1;
        break;
    }

    bRet = (iDiffValue>=m_iEffValue && iDiffValue<m_iExpValue);

    switch (m_iCycleType) {
      case RELATIVE_YEAR_TYPE:
      case RELATIVE_MONTH_TYPE:
      case RELATIVE_DAY_TYPE:
        return bRet;

      case MIX_YEAR_TYPE:
      case MIX_MONTH_TYPE:
      case MIX_DAY_TYPE:
        return bRet || check(sDate);

      default:
        return bRet;
    }

    return false;
}


bool LifeCycle::check(char *sDate, ExPricingPlan & explan)
{
	if (m_iCycleType==ABSOLUTE_TYPE || explan.m_iCustPlanOffset==0) {
		return check (sDate);
	} else if (m_iCycleType==ABSOLUTE_NOYEAR_TYPE) {
		char sTemp[12];
		strncpy (sTemp, sDate+4, 4);
		sTemp[4] = 0;

		int iTemp = atoi (sTemp);

		if (m_iEffValue <= m_iExpValue) {
			return (iTemp>=m_iEffValue && iTemp<m_iExpValue);
		} else {
			return (iTemp>=m_iEffValue || iTemp<m_iExpValue);
		}
	}

	int iDiffValue;
	char const *sEffDate = G_PUSERINFO->getCustomPlanEffDate (explan.m_iCustPlanOffset);

	Date dn(sDate);
	Date dold((char*)sEffDate);

	bool bRet;

	if (m_iCycleType == MOD_MONTH_TYPE) {
		iDiffValue = dn.diffStrictMon (dold);
		if (iDiffValue % m_iEffValue == m_iExpValue) {
			return true;
		} else {
			return false;
		}
	} else if (m_iCycleType == MOD_CYCLE_TYPE) {
		BillingCycleMgr bcm;

		int iBeginCycleID = bcm.getOccurBillingCycle (1, (char *)(sEffDate))->getBillingCycleID ();
		int iNowCycleID = bcm.getOccurBillingCycle (1, sDate)->getBillingCycleID ();
		int iDiffValue = bcm.diffBillingCycle (iNowCycleID, iBeginCycleID);

		if (iDiffValue % m_iEffValue == m_iExpValue) {
			return true;
		} else {
			return false;
		}
	}else if(m_iCycleType == MOD_NORMAL_MONTH_TYPE)
    {
        iDiffValue = dn.diffMon (dold);
		if (iDiffValue % m_iEffValue == m_iExpValue) {
			return true;
		} else {
			return false;
		}
	}

	switch (m_iCycleType) {
	  case RELATIVE_YEAR_TYPE:
	  case MIX_YEAR_TYPE:
		iDiffValue = dn.diffStrictYear (dold);
		break;

	  case RELATIVE_MONTH_TYPE:
	  case MIX_MONTH_TYPE:
		iDiffValue = dn.diffStrictMon (dold);
		break;

	  case RELATIVE_DAY_TYPE:
	  case MIX_DAY_TYPE:
		iDiffValue = dn.diffStrictDay (dold);
		break;

	  default:
		iDiffValue = -1;
		break;
	}

	bRet = (iDiffValue>=m_iEffValue && iDiffValue<m_iExpValue);

	switch (m_iCycleType) {
	  case RELATIVE_YEAR_TYPE:
	  case RELATIVE_MONTH_TYPE:
	  case RELATIVE_DAY_TYPE:
		return bRet;

	  case MIX_YEAR_TYPE:
	  case MIX_MONTH_TYPE:
	  case MIX_DAY_TYPE:
		return bRet || check(sDate);

	  default:
		return bRet;
	}
}

bool LifeCycle::check(StdEvent * pEvent, ExPricingPlan & explan)
{
	return check (pEvent->m_sStartDate, explan);
}

//##ModelId=424E15B7027A
LifeCycle *LifeCycleMgr::getLifeCycle(int iLifeCycleID)
{
	#ifdef PRIVATE_MEMORY
	if (!m_bUploaded) return 0;

	LifeCycle * pCycle = 0;
	
	m_poIndex->get (iLifeCycleID, &pCycle);

	return pCycle;
	#else
	return getLifeCycleShm(iLifeCycleID);
	#endif	
}

#ifndef PRIVATE_MEMORY
LifeCycle *LifeCycleMgr::getLifeCycleShm(int iLifeCycleID)
{
	LifeCycle *pCycle = NULL;
    unsigned int iTmp = 0;
    if(!(G_PPARAMINFO->m_poLifeCycleIndex->get(iLifeCycleID, &iTmp)))
        return NULL;
    else
    {
        pCycle = (LifeCycle *)(G_PPARAMINFO->m_poLifeCycleList + iTmp);
        return pCycle;
    }
}
#endif

//##ModelId=424E166002BA
void LifeCycleMgr::load()
{
	unload();

	int i;
	LifeCycle * pCycle;
	char sTemp[4];

	TOCIQuery qry (Environment::getDBConn ());

	qry.setSQL ("select count(*) from life_cycle");
	qry.open (); qry.next ();

	i = qry.field(0).asInteger () + 16;
	
	qry.close ();

	m_poLifeCycleList = new LifeCycle[i];
	if (!m_poLifeCycleList) THROW(MBC_LifeCycleMgr+1);

	m_poIndex = new HashList<LifeCycle *> ((i>>2) + 16);
	if (!m_poIndex) THROW(MBC_LifeCycleMgr+1);

	qry.setSQL ("select life_cycle_id, eff_type, "
		"nvl(to_char(eff_date, 'yyyymmddhh24miss'), '19000101000000') eff_date, "
		"nvl(to_char(exp_date, 'yyyymmddhh24miss'), '19000101000000') exp_date,"
		"nvl(eff_value, -1) eff_value,"
		"nvl(exp_value, -1) exp_value "
		"from life_cycle");
	qry.open ();

	while (qry.next ()) {
		i--;
		if (i<0) THROW(MBC_LifeCycleMgr+2);
	
		pCycle = m_poLifeCycleList + i;

		pCycle->m_iCycleID = qry.field(0).asInteger ();
		strcpy (pCycle->m_sEffDate, qry.field(2).asString ());
		strcpy (pCycle->m_sExpDate, qry.field(3).asString ());
		pCycle->m_iEffValue = qry.field (4).asInteger ();
		pCycle->m_iExpValue = qry.field (5).asInteger ();

		strcpy (sTemp, qry.field(1).asString ());

		switch (sTemp[2] | 0X20) {
		  case 'a':
			pCycle->m_iCycleType = ABSOLUTE_TYPE;
			break;

		  case 'b':
			pCycle->m_iCycleType = RELATIVE_YEAR_TYPE;
			break;

		  case 'c':
			pCycle->m_iCycleType = RELATIVE_MONTH_TYPE;
			break;

		  case 'd':
			pCycle->m_iCycleType = RELATIVE_DAY_TYPE;
			break;

		  case 'e':
			pCycle->m_iCycleType = MIX_YEAR_TYPE;
			break;

		  case 'f':
			pCycle->m_iCycleType = MIX_MONTH_TYPE;
			break;

		  case 'g':
			pCycle->m_iCycleType = MIX_DAY_TYPE;
			break;

		  case 'h':
			pCycle->m_iCycleType = MOD_MONTH_TYPE;
			break;

		  case 'i':
			pCycle->m_iCycleType = MOD_CYCLE_TYPE;
			break;

		  case 'j':
			pCycle->m_iCycleType = ABSOLUTE_NOYEAR_TYPE;
			break;

          case 'k':
            pCycle->m_iCycleType = RELATIVE_CYCLE_TYPE;
            break;

          case 'l':
              pCycle->m_iCycleType = MOD_NORMAL_MONTH_TYPE;
              break;
		  default:
			pCycle->m_iCycleType = 0;
			break;
		}

		m_poIndex->add (pCycle->m_iCycleID, pCycle);	
	}

	qry.close ();

	m_bUploaded = true;
}

//##ModelId=424E169000B0
void LifeCycleMgr::unload()
{
	m_bUploaded = false;
	if (m_poIndex) {
		delete m_poIndex;
		m_poIndex = 0;
	}

	if (m_poLifeCycleList) {
		delete [] m_poLifeCycleList;
		m_poLifeCycleList = 0;
	}
}

//##ModelId=424E220203AC
LifeCycleMgr::LifeCycleMgr()
{
#ifdef PRIVATE_MEMORY
	if (!m_bUploaded) load ();
#endif
}

//##ModelId=424E15EB0211
bool LifeCycleMgr::m_bUploaded = false;

//##ModelId=424E156301F7
HashList<LifeCycle *> *LifeCycleMgr::m_poIndex = 0;

//##ModelId=42521B790041
LifeCycle *LifeCycleMgr::m_poLifeCycleList = 0;

