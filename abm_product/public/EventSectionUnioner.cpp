/*VER: 1*/ 
#include "EventSectionUnioner.h"
#include <stdlib.h>
#include <string.h>
#include "Environment.h"

#include "Exception.h"
#include "MBC.h"

void EventSectionUnioner::adjust(int iCharge)
{
	if (!iCharge || m_iLastI==-1) return;

	m_aoChargeInfo[m_iLastI].m_iStdCharge += iCharge;
	m_aoChargeInfo[m_iLastI].m_iCharge += iCharge;
}

//##ModelId=426DAD8000DC
void EventSectionUnioner::clear()
{
	m_iChargeNum = 0;
	m_iCurMeasure = 0;
	m_iLastI = -1;
}

//##ModelId=426DAD880084
void EventSectionUnioner::insert(ChargeInfo *pChargeInfo)
{
	int i;
	for (i=0; i< m_iChargeNum ; i++) {
		if (m_aoChargeInfo[i].m_iAcctItemTypeID == pChargeInfo->m_iAcctItemTypeID) {
			m_aoChargeInfo[i].m_iStdCharge += pChargeInfo->m_iStdCharge;
			m_aoChargeInfo[i].m_lBillMeasure += pChargeInfo->m_lBillMeasure;
			m_aoChargeInfo[i].m_iCharge += pChargeInfo->m_iCharge;

			m_iLastI = i;

			return;
		}
	}

	if (m_iChargeNum >= MAX_CHARGE_NUM) THROW (MBC_EventSectionUnioner+1);

	memcpy (m_aoChargeInfo+i, pChargeInfo, sizeof (ChargeInfo));

	m_iChargeNum++;
	
	return;
}

//##ModelId=426DADB50287
int EventSectionUnioner::getCharge(int iAcctItemType)
{
/*
	2005年5月12日，大家讨论决定，m_iDiscountAcctItemTypeID不用，插入的和取累积量的
	都使用m_iAcctItemType，值取Charge
*/

/*	20050619 iAcctItemType为账目组 */

	int i;
	int iRet = 0;

	for (i=0; i<m_iChargeNum ; i++) {
		if (G_PACCTITEMMGR->getBelongGroupB (m_aoChargeInfo[i].m_iAcctItemTypeID ,iAcctItemType)) {
			iRet += m_aoChargeInfo[i].m_iCharge;
		} 
	}

	return iRet;
}

//##ModelId=426DAE05007A
void EventSectionUnioner::copyToEvent(StdEvent *pEvent)
{
	int i;
	for (i=0; i<m_iChargeNum; i++) {
		memcpy (pEvent->m_oChargeInfo+i, m_aoChargeInfo+i, sizeof (ChargeInfo));
	}
}

//##ModelId=426DB00D037A
EventSectionUnioner::EventSectionUnioner()
{
	m_iChargeNum = 0;
	m_iCurMeasure = 0;
	m_iLastI = -1;
}


//##ModelId=4272E61602A2
void EventSectionUnioner::setCurMeasure(int iMeasure)
{
	m_iCurMeasure = iMeasure;
}

//##ModelId=4272E62902F9
int EventSectionUnioner::getCurMeasure()
{
	return m_iCurMeasure;
}

