/*VER: 1*/ 
#include "BillingCycle.h"
#include "Date.h"
#include <string.h>

//##ModelId=424164500201
int BillingCycle::getBillingCycleID() const
{
    return m_iBillingCycleID;
}


//##ModelId=424164500251
int BillingCycle::getBillingCycleType() const
{
    return m_iBillingCycleType;
}


//##ModelId=424164500369
char *BillingCycle::getBlockDate() const
{
    return (char *)m_sBlockDate;
}


//##ModelId=42416450039B
char *BillingCycle::getDueDate() const
{
    return (char *)m_sDueDate;
}


//##ModelId=4241645003D7
char *BillingCycle::getEndDate() const
{
    return (char *)m_sEndDate;
}

bool BillingCycle::isLastDay(Date &d)
{
	Date d1(m_sEndDate);

	if (d1-d == 1) {
		return true;
	} else {
		return false;
	}
}

bool BillingCycle::isLastDay(char *sDate)
{
	Date d1(m_sEndDate);
	Date d(sDate);

	if (d1-d == 1) {
		return true;
	} else {
		return false;
	}
}

bool BillingCycle::isFirstDay(Date &d)
{
	Date d1(m_sStartDate);
	if (d1-d == 0) {
		return true;
	} else {
		return false;
	}
}

bool BillingCycle::isFirstDay(char *sDate)
{
	return !(strncmp (sDate, m_sStartDate, 8));
}


//##ModelId=424164510021
char *BillingCycle::getStartDate() const
{
    return (char *)m_sStartDate;
}


//##ModelId=424164510071
char *BillingCycle::getState() const
{
    return (char *)m_sState;
}


//##ModelId=4241645100AE
char *BillingCycle::getStateDate() const
{
    return (char *)m_sStateDate;
}


//##ModelId=425342B50168
BillingCycle::BillingCycle()
{
}


//##ModelId=425B76C800D6
int BillingCycle::getDays()
{
	return Date(m_sEndDate) - Date(m_sStartDate);
}




