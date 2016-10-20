/*VER: 1*/ 
#include "OfferAccuMgr.h"

#ifdef MEM_REUSE_MODE
SHMIntHashIndex_KR *OfferAccuBase::m_poAccuIndex = 0;
SHMIntHashIndex_KR *OfferAccuBase::m_poAccuServIndex = 0;
SHMIntHashIndex_KR *OfferAccuBase::m_poAccuInstIndex = 0;
    
SHMData_B<AccuData> *OfferAccuBase::m_poAccuData = 0;

SHMData_B<CycleInstance> *OfferAccuBase::m_poCycleInstData=0;
SHMIntHashIndex_KR *OfferAccuBase::m_poCycleInstIndex = 0;
        
#else
SHMIntHashIndex *OfferAccuBase::m_poAccuIndex = 0;
SHMIntHashIndex *OfferAccuBase::m_poAccuServIndex = 0;
SHMIntHashIndex *OfferAccuBase::m_poAccuInstIndex = 0;
/*
AccuData *OfferAccuBase::m_poAccu = 0;
CycleInstance *OfferAccuBase::m_poCycleInst = 0;
*/
SHMData<AccuData> *OfferAccuBase::m_poAccuData = 0;

SHMData<CycleInstance> *OfferAccuBase::m_poCycleInstData=0;
SHMIntHashIndex *OfferAccuBase::m_poCycleInstIndex = 0;
#endif


AccuData *OfferAccuBase::m_poAccu = 0;
CycleInstance *OfferAccuBase::m_poCycleInst = 0;



bool OfferAccuBase::m_bAttached = false;

CSemaphore * OfferAccuMgr::m_poAccuDataLock = 0;
CSemaphore * OfferAccuMgr::m_poAccuIndexLock = 0;
CSemaphore * OfferAccuMgr::m_poCycleDataLock = 0;
CSemaphore * OfferAccuMgr::m_poCycleIndexLock = 0;

OfferAccuBase::OfferAccuBase()
{
    if (m_bAttached)
        return;

    m_bAttached = true;	

#ifdef MEM_REUSE_MODE
    ATTACH_DATA_B(m_poAccuData, AccuData, ACC_DATA_DATA);
    ATTACH_INT_INDEX_KR(m_poAccuIndex, ACC_DATA_INDEX);
    ATTACH_INT_INDEX_KR(m_poAccuServIndex, ACC_DATA_S_INDEX);
    ATTACH_INT_INDEX_KR(m_poAccuInstIndex, ACC_DATA_I_INDEX);
#else
    ATTACH_DATA(m_poAccuData, AccuData, ACC_DATA_DATA);

    ATTACH_INT_INDEX(m_poAccuIndex, ACC_DATA_INDEX);
    ATTACH_INT_INDEX(m_poAccuServIndex, ACC_DATA_S_INDEX);
    ATTACH_INT_INDEX(m_poAccuInstIndex, ACC_DATA_I_INDEX);
#endif



#ifdef MEM_REUSE_MODE
    ATTACH_DATA_B(m_poCycleInstData, CycleInstance, ACC_CYCLE_DATA);
    ATTACH_INT_INDEX_KR(m_poCycleInstIndex, ACC_CYCLE_INDEX);

#else
    ATTACH_DATA(m_poCycleInstData, CycleInstance, ACC_CYCLE_DATA);
    ATTACH_INT_INDEX(m_poCycleInstIndex, ACC_CYCLE_INDEX);

#endif    


    if (m_bAttached)
        bindData ();
}

//##ModelId=4247790103D4
void OfferAccuBase::freeAll()
{
    #define FREE_O(X) \
    if (X) { \
        delete X; \
        X = 0; \
    }

    FREE_O(m_poAccuData);

    FREE_O(m_poAccuIndex);
    FREE_O(m_poAccuServIndex);
    FREE_O(m_poAccuInstIndex);

    FREE_O(m_poCycleInstData);
    FREE_O(m_poCycleInstIndex);
}

void OfferAccuBase::bindData()
{
    m_poAccu = (AccuData *)(*m_poAccuData);
    m_poCycleInst = (CycleInstance *)(*m_poCycleInstData);
}

OfferAccuBase::operator bool()
{
    return m_bAttached;
}

