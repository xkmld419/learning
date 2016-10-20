#include "SrvShmMgr.h"
#include "ReadCfg.h"

SimpleSHM * SrvShmMgr::m_poSHM = 0;
StatData * SrvShmMgr::m_poStatData = 0;
SockAppStruct * SrvShmMgr:: m_poSockData = NULL;
AccuTTStruct * SrvShmMgr:: m_poAccuTTData = NULL;

long SrvShmMgr::getKey(ABMException & oExp)
{
    ReadCfg cfg;
    char sFileName[1024];

    char *penv = getenv("ABMAPP_DEPLOY");
    if (!penv) {
        ADD_EXCEPT0(oExp, "miss environment ABMAPP_DEPLOY, please add it to your .profile");
        return 0;
    }

    if (penv[strlen(penv)-1] != '/') {
        snprintf (sFileName, sizeof(sFileName)-1, "%s/config/abm_app_cfg.ini", penv);
    } else {
        snprintf (sFileName, sizeof(sFileName)-1, "%sconfig/abm_app_cfg.ini", penv);
    }
    
    int iRet = cfg.read(oExp, sFileName, "ABM_SRV", '|');
    if (iRet) {
        ADD_EXCEPT1(oExp, "cfg.read(%s,\"ABM_SERV\",'|') failed", sFileName); 
        return 0;
    }

    long lKey;

    if (!cfg.getValue (oExp, lKey, "shm_key", 1)) {
        ADD_EXCEPT0(oExp, "cfg.getValue(lKey, \"shm_key\",1) failed");
        return 0;
    }

    return lKey;
}

int SrvShmMgr::open(ABMException &oExp, bool bWrite, bool bRead)
{
    unsigned int iPermission;

    if (m_poStatData) return 0;

    if (bWrite) {
        iPermission = 0600;
    } else {
        iPermission = 0400;
    }

    int iRet = openSHM(oExp, false, iPermission);
    if (iRet) {
        ADD_EXCEPT1(oExp, "openSHM(false, %o) failed. did abminit run?", iPermission);
        return iRet;
    }


    return 0;
}

int SrvShmMgr::removeMem(ABMException &oExp)
{
    int iRet = openSHM(oExp, false, 0600);
    if (!iRet) {
        oExp.clear ();
        return m_poSHM->remove(oExp);
    }
    return 0;
}

int SrvShmMgr::mallocMem(ABMException &oExp)
{
    int iRet = openSHM(oExp, true, 0600);

    if (iRet) {
        ADD_EXCEPT0(oExp, "SrvShmMgr::openSHM(true, 0600) failed");
        return iRet;
    }

    SrvShmData *pData = (SrvShmData *)(m_poSHM->getPointer ());
    memset (pData, 0, sizeof (SrvShmData));

    return iRet;
}

int SrvShmMgr::openSHM(ABMException &oExp, bool ifcreate, unsigned int iPermission)
{
    long lkey = getKey (oExp);

    if (lkey <=0 ) {
        ADD_EXCEPT0(oExp, "SrvShmMgr::getKey() failed");
        return -2;
    }

    m_poSHM = new SimpleSHM(lkey, sizeof (SrvShmData));
    if (!m_poSHM) {
        ADD_EXCEPT2(oExp, "new SimpleSHM(%ld, %d) failed", lkey, sizeof(SrvShmData));
        return -1;
    }

    int iRet = m_poSHM->open (oExp, ifcreate, iPermission);
    if (iRet) {
        ADD_EXCEPT2(oExp, "m_poSHM->open(%s, %o) failed", ifcreate?"true":"false", iPermission);
        return iRet;
    }

    //°ó¶¨±äÁ¿
    m_poStatData = &(((SrvShmData *)(m_poSHM->getPointer()))->m_oStatData);
    m_poSockData = &(((SrvShmData *)(m_poSHM->getPointer()))->m_oSockData);
    m_poAccuTTData = &(((SrvShmData *)(m_poSHM->getPointer()))->m_oAccuTTData);
    return 0;
}
