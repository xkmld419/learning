#include "SrvSHMMgr.h"
#include "SHMAccess.h"
#include "ABMException.h"
#include "ReadParamConf.h"

#define HSS_SRV_SHM_KEY "HSS_SRV_SHM_KEY"
#define HSS_GUARD_SHM_KEY "HSS_GUARD_SHM_KEY"
#define HSS_TERMINAL_SHM_KEY "HSS_TERMINAL_SHM_KEY"

SHMAccess *SrvSHMMgr::m_poSHM = NULL;
MessBlock *SrvSHMMgr::m_poMess = NULL;
CurActiveCnt *SrvSHMMgr::m_poAtv = NULL; 

SHMAccess *SrvSHMMgr::m_poGuardSHM = NULL;
__HSS::TSHMCMA   *SrvSHMMgr::m_poGuard = NULL;
    
SHMAccess *SrvSHMMgr::m_poTerminalSHM = NULL;
__HSS::THeadInfo   *SrvSHMMgr::m_poTerminal = NULL;         

int SrvSHMMgr::openSHM(ABMException &oExp, bool bIfCrt)
{
	if (m_poSHM) {
		return 0;
	}	
    SHMParamConf oConf;
    if (ReadParamConf::getSHMParamConf(oExp, HSS_SRV_SHM_KEY, oConf) != 0) {
        ADD_EXCEPT1(oExp, "SrvSHMMgr::openSHM key= \"%s\" read config failed!", HSS_SRV_SHM_KEY);
        return -1; 
    }
    if ((m_poSHM=new SHMAccess(oConf.m_ulKey)) == NULL) {
        ADD_EXCEPT1(oExp, "SrvSHMMgr::openSHM key=%d malloc failed!", oConf.m_ulKey);
        return -1;    
    }
    if (!m_poSHM->exist()) {
        if (!bIfCrt) {
            ADD_EXCEPT1(oExp, "SrvSHMMgr::openSHM attach SHM key=%d failed! create first!", oConf.m_ulKey);
            return -1;
        }    
        m_poSHM->create(sizeof(SrvSHMData));        
    }
    m_poMess = &((SrvSHMData *)(m_poSHM->getMemAddr()))->m_oMessData;
    m_poAtv = &((SrvSHMData *)(m_poSHM->getMemAddr()))->m_oActive;
    return 0;           
}

int SrvSHMMgr::openGuardSHM(ABMException &oExp)
{
    SHMParamConf oConf;
    if (ReadParamConf::getSHMParamConf(oExp, HSS_GUARD_SHM_KEY, oConf) != 0) {
        ADD_EXCEPT1(oExp, "SrvSHMMgr::openGuardSHM key= \"%s\" read config failed!", HSS_GUARD_SHM_KEY);
        return -1; 
    }
    if ((m_poGuardSHM=new SHMAccess(oConf.m_ulKey)) == NULL) {
        ADD_EXCEPT1(oExp, "SrvSHMMgr::openGuardSHM key=%d malloc failed!", oConf.m_ulKey);
        return -1;    
    }
    if (!m_poGuardSHM->exist()) {
        ADD_EXCEPT1(oExp, "SrvSHMMgr::openGuardSHM attach SHM key=%d failed! create first!", oConf.m_ulKey);
        return -1;       
    }
    m_poGuard = ((__HSS::TSHMCMA *)(m_poGuardSHM->getMemAddr()));
    return 0;           
}

int SrvSHMMgr::openTerminalSHM(ABMException &oExp)
{
    SHMParamConf oConf;
    if (ReadParamConf::getSHMParamConf(oExp, HSS_TERMINAL_SHM_KEY, oConf) != 0) {
        ADD_EXCEPT1(oExp, "SrvSHMMgr::openTerminalSHM key= \"%s\" read config failed!", HSS_TERMINAL_SHM_KEY);
        return -1; 
    }
    if ((m_poTerminalSHM=new SHMAccess(oConf.m_ulKey)) == NULL) {
        ADD_EXCEPT1(oExp, "SrvSHMMgr::openTerminalSHM key=%d malloc failed!", oConf.m_ulKey);
        return -1;    
    }
    if (!m_poTerminalSHM->exist()) {
        ADD_EXCEPT1(oExp, "SrvSHMMgr::openGuardSHM attach SHM key=%d failed! create first!", oConf.m_ulKey);
        return -1;       
    }
    m_poTerminal = ((__HSS::THeadInfo *)(m_poTerminalSHM->getMemAddr()));
    return 0;           
}     
