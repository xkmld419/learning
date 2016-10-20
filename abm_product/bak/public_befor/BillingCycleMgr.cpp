/*VER: 2*/ 
#include "BillingCycleMgr.h"
#include "BillingCycle.h"
#include "Environment.h"

//##ModelId=42451C960210
BillingCycleMgr::BillingCycleMgr()
{
	if (!m_bAttached) THROW (MBC_BillingCycleMgr);
}

int BillingCycleMgr::diffBillingCycle(int iBillingCycle1, int iBillingCycle2)
{
	int iRet = 0;
	BillingCycle * p1 = getBillingCycle (iBillingCycle1);
	BillingCycle * p2 = getBillingCycle (iBillingCycle2);

	if (!p1 || !p2) return -1;

	if (iBillingCycle1 == iBillingCycle2) return 0;

	if (strcmp (p1->getStartDate(), p2->getStartDate())>0 ) {
		while (p1 && iBillingCycle2!=p1->getBillingCycleID()) {
			p1 = getPreBillingCycle (p1->getBillingCycleID ());
			iRet ++;
		}
		if (!p1) {
			iRet = -1;  
		}
	} else {
		while (p2 && iBillingCycle1!=p2->getBillingCycleID ()) {
			p2 = getPreBillingCycle (p2->getBillingCycleID ());
			iRet++;
		}

		if (!p2) {
			iRet = -1;
		}
	}

	return iRet;
}

//##ModelId=42451C960242
BillingCycleMgr::~BillingCycleMgr()
{
}

BillingCycle * BillingCycleMgr::getOccurBillingCycle(int iCycleType, char *sTime)
{
    unsigned int iOffset, iCurOffset;
    BillingCycle * pResult;
    int  iTemp;
    char sStartDate[10];

    strcpy(sStartDate,sTime);
    sStartDate[8]=0;

    if (!m_poTypeIndex->get (iCycleType, &iOffset)) return 0;

    pResult = *m_poCycleData;

    iCurOffset = iOffset;

    while (true) {

        if ( strncmp(sStartDate,pResult[iCurOffset].m_sStartDate,8)>=0
          && strncmp(sStartDate,pResult[iCurOffset].m_sEndDate,8)<0 )
            return pResult+iCurOffset;

        iTemp = strncmp (sStartDate, pResult[iCurOffset].m_sStartDate,8);
        if (iTemp > 0) {
            if(!pResult[iCurOffset].m_iNext)
                break;
            iCurOffset = pResult[iCurOffset].m_iNext;
        } else {
            if(!pResult[iCurOffset].m_iPre)
                break;
            iCurOffset = pResult[iCurOffset].m_iPre;
        }
    }

    //没有匹配，返回当前
    return pResult+iCurOffset;

}


//##ModelId=42451C9602BA
/*2007.05.12 苏州 查找逻辑修改成在索引指向10R范围内按时间最优匹配*/
BillingCycle *BillingCycleMgr::getBillingCycle(int iCycleType, char *sTime)
{
	unsigned int iOffset, iPreOffset, iCurOffset, iLogicNext;
	BillingCycle * pResult;
    int iTemp;
    char sStartDate[10];

    strcpy(sStartDate,sTime);
    sStartDate[8]=0;

	if (!m_poTypeIndex->get (iCycleType, &iOffset)) return 0;

	pResult = *m_poCycleData;

    iCurOffset = iOffset;

/*移至脱机那一段
	if (strcmp (sTime, pResult[iOffset].m_sStartDate) >= 0)
		return pResult+iOffset;
*/
	iPreOffset = pResult[iOffset].m_iPre;

#ifdef REAL_BILLING_CYCLE

    if (strncmp(sStartDate, pResult[iCurOffset].m_sStartDate,8) >= 0)
        iLogicNext = pResult[iCurOffset].m_iNext;
    else
        iLogicNext = pResult[iCurOffset].m_iPre;

	/*	取实时的帐期	*/
    while (iCurOffset) {
        if ( strncmp(sStartDate,pResult[iCurOffset].m_sStartDate,8)>=0
          && strncmp(sStartDate,pResult[iCurOffset].m_sEndDate,8)<0 
          && pResult[iCurOffset].m_sState[2]=='R' )
            return pResult+iCurOffset;

        //边界的10R
        if ( pResult[iCurOffset].m_sState[2]=='R' ){
            if (iLogicNext == 0)
                return pResult+iCurOffset;
            if ( pResult[iLogicNext].m_sState[2]!='R' )
                return pResult+iCurOffset;
        }   

        iTemp = strncmp (sStartDate, pResult[iCurOffset].m_sStartDate,8);
        if (iTemp > 0) {
            iCurOffset = pResult[iCurOffset].m_iNext;
            iLogicNext = pResult[iCurOffset].m_iNext;
        } else {
            iCurOffset = pResult[iCurOffset].m_iPre;
            iLogicNext = pResult[iCurOffset].m_iPre;
        }
    }

/*
	if (!pResult[iOffset].m_iPre || pResult[iPreOffset].m_sState[2]!='R')
		return pResult+iOffset;

	if (strcmp (sTime, pResult[iPreOffset].m_sStartDate) < 0)
		return pResult+iOffset;

	return pResult+iPreOffset;
*/
#else

	/*	按脱机模式	*/
	if (strcmp (sTime, pResult[iOffset].m_sStartDate) >= 0)
		return pResult+iOffset;

	while (iPreOffset && strcmp (sTime, pResult[iPreOffset].m_sStartDate) < 0) {
		iPreOffset = pResult[iOffset].m_iPre;
	}

	if (iPreOffset) {
		return pResult+iPreOffset;
	} else {
		return pResult+iOffset;
	}

#endif

    return 0;
}

//##ModelId=4250DF7B00DE
int BillingCycleMgr::getBillingCycles(vector <BillingCycle *> &ret, int cycleType, char *beginDate, char *endDate)
{
        BillingCycle *pCycle;
	unsigned int iOffset;
        int i = 0;

        if (!(m_poTypeIndex->get (cycleType, &iOffset)))
                return 0;

	pCycle = *m_poCycleData;

        //##先移到头
        while (pCycle[iOffset].m_iPre) {
		iOffset = pCycle[iOffset].m_iPre;
        }

        while (iOffset) {
		if ( (strcmp (beginDate, pCycle[iOffset].m_sEndDate)<0 &&
			strcmp (beginDate, pCycle[iOffset].m_sStartDate) >=0) ||
		     (strcmp (endDate, pCycle[iOffset].m_sEndDate)<=0 &&
			strcmp (endDate, pCycle[iOffset].m_sStartDate) > 0) ||
		     (strcmp (endDate, pCycle[iOffset].m_sStartDate) > 0 &&
			strcmp (beginDate, pCycle[iOffset].m_sStartDate) <=0) ) {

                        i++;
                        ret.push_back (pCycle+iOffset);
                }

		iOffset = pCycle[iOffset].m_iNext;
        }

        return i;
}

bool BillingCycleMgr::endBillingCycle(int iBillingCycleID, bool changeDB)
{
	BillingCycle * p = getBillingCycle (iBillingCycleID);
	if (!p) return false;

	p->m_sState[2] = 'E';

	if (changeDB) {

		DEFINE_QUERY (qry);
		char sSQL[2048];

		sprintf (sSQL, "update billing_cycle set state='10E', state_date=sysdate "
				" where billing_cycle_id=%d", iBillingCycleID);

		qry.setSQL (sSQL); qry.execute (); qry.close (); qry.commit ();
		
	}
	return true;
}

//##ModelId=425B765D0137
BillingCycle *BillingCycleMgr::getBillingCycle(int iBillingCycleID)
{
	unsigned int iOffset;
	BillingCycle * pResult;

	pResult = *m_poCycleData;

	for (iOffset=1; iOffset<=m_poCycleData->getCount(); iOffset++) {
		if (pResult[iOffset].m_iBillingCycleID == iBillingCycleID)
			return pResult+iOffset;
	}

	return 0;
}

BillingCycle *BillingCycleMgr::getPreBillingCycle(int iBillingCycleID)
{
	unsigned int iOffset;
	BillingCycle * pResult;

	pResult = *m_poCycleData;

	for (iOffset=1; iOffset<=m_poCycleData->getCount(); iOffset++) {
		if (pResult[iOffset].m_iBillingCycleID == iBillingCycleID) {
			if (!pResult[iOffset].m_iPre) return 0;

			return pResult+pResult[iOffset].m_iPre;
		}
	}

	return 0;
}


BillingCycle *BillingCycleMgr::getCurBillingCycle(int i_iCycleType, int i_iOffset)
{
    BillingCycle *pResult;
    unsigned int uiOffset;
    int i = 0;
    pResult = *m_poCycleData;


    if (!(m_poTypeIndex->get(i_iCycleType, &uiOffset)))
        return 0;
    if( 0 == i_iOffset)
        return  pResult + uiOffset;
    
    do{
        if( i_iOffset >0 ){
            if (!pResult[uiOffset].m_iNext) return 0;
            uiOffset = pResult[uiOffset].m_iNext;
            i_iOffset--;
                
        }else{

            if (!pResult[uiOffset].m_iPre) return 0;
            uiOffset = pResult[uiOffset].m_iPre;
            i_iOffset++;
        }
        
    }while( i_iOffset != 0 );
    
    return pResult + uiOffset;
    
	
}
