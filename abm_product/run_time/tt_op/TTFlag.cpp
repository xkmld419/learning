#include "TTFlag.h"
#include "ReadCfg.h"

TTFlag::TTFlag()
{
    m_poSem = NULL;
    m_poSHM = NULL;
    m_plState = NULL;
}

TTFlag::~TTFlag()
{
    if (m_poSem == NULL) {
        delete m_poSem;
        m_poSem = NULL;
    }

    if (m_poSHM) {
        delete m_poSHM;
        m_poSHM = NULL;
    }
}

int TTFlag::init(ABMException &oExp)
{
    ReadCfg cfg;
    char asFileName[1024] = {0};
    long lKey;
    char asSemName[32] = {0};
    int iNum = 0;
    int iRet = 0;

    char *penv = getenv("ABMAPP_DEPLOY");
    if (!penv) {
        ADD_EXCEPT0(oExp, "miss environment ABMAPP_DEPLOY, please add it to your .profile");
        return -1;
    }

    if (penv[strlen(penv)-1] != '/') {
        snprintf (asFileName, sizeof(asFileName)-1, "%s/config/abm_app_cfg.ini", penv);
    } else {
        snprintf (asFileName, sizeof(asFileName)-1, "%sconfig/abm_app_cfg.ini", penv);
    }    

    iRet = cfg.read(oExp, asFileName, "STATE_FLAG", '|');
    if (iRet) {
        ADD_EXCEPT1(oExp, "cfg.read(%s,\"STATE_FLAG\",'|') failed", asFileName); 
        return -1;
    }

    if (!cfg.getValue (oExp, lKey, "SHM_KEY", 1)) {
        ADD_EXCEPT0(oExp, "cfg.getValue(lKey, \"SHM_KEY\",1) failed");
        return -1;
    }

    if (!cfg.getValue(oExp, asSemName, "SEM_NAME", 1, sizeof(asSemName) - 1)) {
        ADD_EXCEPT0(oExp, "获取信息号信息出错（SEM_NAME)");
        return -1;
    }

    m_poSem = new CSemaphore;
    if (m_poSem == NULL) {
        ADD_EXCEPT0(oExp, "生成信息号对象失败");
        return -1;
    }

    if (!m_poSem->getSem(asSemName)) {
        ADD_EXCEPT0(oExp, "分配信息号失败");
        return -1;
    }

    if (!m_poSem->P()) {
        ADD_EXCEPT0(oExp, "信号量上锁失败");
        return -1;
    }

    do {

        m_poSHM = new SimpleSHM(lKey, sizeof(long));
        if (m_poSHM == NULL) {
            ADD_EXCEPT0(oExp, "创建对象失败");
            iRet = -1;
            break;
        }

        if (m_poSHM->open(oExp, false, 0600) != 0) {
            if (m_poSHM->open(oExp, true, 0600) != 0) {
                ADD_EXCEPT0(oExp, "共享内存创建失败!");
                iRet = -1;
                break;
            }   
            *(long *)(m_poSHM->getPointer()) = STATE_FLAG_BAD;
        }       
       
        if (iRet) {
            m_poSHM->remove(oExp);
            break;
        }
        
        m_plState = (long *)(m_poSHM->getPointer());
    } while (false);

    if (!m_poSem->V()) {
        ADD_EXCEPT0(oExp, "释放信号锁失败");
        return -1;
    }

    return iRet;
}

int TTFlag::removeSHM(ABMException &oExp)
{
    if (m_poSHM) {
        return m_poSHM->remove(oExp);
    }
    return 0;
}

int TTFlag::setGoodFlag(ABMException &oExp, int iLevel)
{
    return setFlag(oExp, iLevel, STATE_FLAG_GOOD);
}

int TTFlag::setBadFlag(ABMException &oExp, int iLevel)
{
    return setFlag(oExp, iLevel, STATE_FLAG_BAD);
}

int TTFlag::setFlag(ABMException &oExp, int iLevel, int iValue)
{    
    if (m_plState == NULL) {
        ADD_EXCEPT0(oExp, "未初始化数据信息");
        return -1;
    }
    
    if (iValue) {
        *m_plState |=  iValue << iLevel;
    } else {
        *m_plState &= 0xffffffff ^ (1 << iLevel);
    }
    return 0;
}

int TTFlag::getState(ABMException &oExp, int iLevel)
{
    unsigned int uiValue = STATE_FLAG_BAD;
    if (m_plState == NULL) {
        ADD_EXCEPT0(oExp, "未初始化数据信息");
        return -1;
    }

    if (!iLevel) {
        if (*m_plState != 0 ) {
            return STATE_FLAG_GOOD;
        } else {
            return STATE_FLAG_BAD;
        }
    }
    
    return (*m_plState >> iLevel) & 0x1;
}

int SetTTState(ABMException &oExp, TT_STATE iState, int iLevel)
{
    static TTFlag *poTTState = NULL;
    if (!poTTState) {
         poTTState = new TTFlag;
        if (!poTTState) {
            ADD_EXCEPT0(oExp, "malloc TTFlag err");
            return -1;
        }
        if (poTTState->init(oExp)) {
            delete poTTState;
            poTTState = NULL; 
            return -1;
        }
    }
    if (iState == TT_GOOD) {
        poTTState->setGoodFlag(oExp, iLevel);
    } else {
        poTTState->setBadFlag(oExp, iLevel);
    }
    return 0;
}

int MallocTTFlagSHM(ABMException &oExp)
{
    TTFlag oTTFlag;
    return oTTFlag.init(oExp);
}

int RemoveTTFlagSHM(ABMException &oExp)
{
    TTFlag oTTFlag;
    if (oTTFlag.init(oExp)) {
        return -1;
    }
    return oTTFlag.removeSHM(oExp);
}

