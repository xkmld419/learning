#ifndef _STATE_FLAG_H_
#define _STATE_FLAG_H_

#include "ABMException.h"
#include "CSemaphore.h"
#include "SHMAccess.h"


#define STATE_FLAG_BAD 0
#define STATE_FLAG_GOOD 1


class TTFlag
{
public:
    TTFlag();  
    ~TTFlag();
    int init(ABMException &oExp);
    int removeSHM(ABMException &oExp);
    int setGoodFlag(ABMException &oExp, int iLevel = 0);
    int setBadFlag(ABMException &oExp, int iLevel = 0);
    int getState(ABMException &oExp, int iLevel = 0);
private:
    int setFlag(ABMException &oExp, int iLevel, int iValue);
private:
    SimpleSHM *m_poSHM;
    CSemaphore *m_poSem;
    long *m_plState;
};

enum TT_STATE {
    TT_BAD = 0,
    TT_GOOD = 1
};

int SetTTState(ABMException &oExp, TT_STATE iState, int iLevel = 0);
int MallocTTFlagSHM(ABMException &oExp);
int RemoveTTFlagSHM(ABMException &oExp);

#endif

