/*VER: 1*/ 
#include "BillingCycleBase.h"

#define FREE(x) \
	if (x) { \
		delete x; \
		x = 0; \
	}

//##ModelId=42A410DE021A
BillingCycleBase::BillingCycleBase()
{
	if (m_bAttached) return;
	
	m_bAttached = true;
	
	m_poCycleData = new SHMData<BillingCycle> (SHM_BILLINGCYCLE_DATA);
	if (!m_poCycleData) goto __FREEALL;

	if (!m_poCycleData->exist ()) m_bAttached = false;
	
	m_poTypeIndex = new SHMIntHashIndex (SHM_BILLINGCYCLE_TYPEINDEX);
	if (!m_poTypeIndex) goto __FREEALL;
	
	if (!m_poTypeIndex->exist ()) m_bAttached = false;

	return; 

__FREEALL:
	FREE (m_poCycleData);
	FREE (m_poTypeIndex);

	THROW (MBC_BillingCycleBase+1);
}

#undef FREE


//##ModelId=42A3F16D0299
SHMData <BillingCycle> *BillingCycleBase::m_poCycleData = 0;

//##ModelId=42A3F19C0156
SHMIntHashIndex *BillingCycleBase::m_poTypeIndex = 0;

//##ModelId=42A3F68902A4
bool BillingCycleBase::m_bAttached = false;

