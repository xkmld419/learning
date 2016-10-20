/*VER: 1*/ 
#include "BillingCycleSchedule.h"


//##ModelId=42A4101002BF
bool BillingCycleSchedule::check()
{
	Date d;
	
	return (m_oDate.diffSec (d) < 2);
}



