#ifndef __HSS_SRV_SHM_MGR_H_INCLUDED_
#define __HSS_SRV_SHM_MGR_H_INCLUDED_

#include "SHMHSSMess.h"
#include "SHMHSSGuardStruct.h"

class SHMAccess;
class ABMException;

#define SRVSHM(x) SrvSHMMgr::x
#define GET_MESSDATA(x) x = SrvSHMMgr::getMessData();

//当前用户记数
struct CurActiveCnt
{
    unsigned int m_uiServCnt;           //用户当前激活数
    unsigned int m_uiOfferInstCnt;     //销售品当前激活数
};

struct SrvSHMData
{
    MessBlock m_oMessData;
    CurActiveCnt m_oActive;
};

class SrvSHMMgr
{

public:
    
    
    static int openGuardSHM(ABMException &oExp);
    static __HSS::TSHMCMA *getGuardData() { return m_poGuard; }
        
    static int openTerminalSHM(ABMException &oExp);
    static __HSS::THeadInfo *getTerminalData() { return m_poTerminal;}    

    static int openSHM(ABMException &oExp, bool bIfCrt=false);
    static MessBlock *getMessData() {
        if (m_poMess) {             
            m_poMess->init(); 
            return m_poMess;
        }
        else
            return NULL;    
    }
	static void setSrvCnt(unsigned int iCnt) { m_poAtv->m_uiServCnt = iCnt;}
	static void setOfferInstCnt(unsigned int iCnt) { m_poAtv->m_uiOfferInstCnt = iCnt; }
    static unsigned int getServCnt() { return m_poAtv->m_uiServCnt;}
    static unsigned int getOfferInstCnt() { return m_poAtv->m_uiOfferInstCnt; }
    static void addServCnt(int iCnt=1) { m_poAtv->m_uiServCnt += iCnt; }
    static void addOfferInstCnt(int iCnt=1) { m_poAtv->m_uiOfferInstCnt += iCnt; } 

private:

    //
    static SHMAccess *m_poSHM;        
    static MessBlock *m_poMess;
    static CurActiveCnt *m_poAtv;
    
    //链接计费GUARD_MAIN的指针
    static SHMAccess *m_poGuardSHM;    
    static __HSS::TSHMCMA *m_poGuard;
        
    //链接TERMINAL MONITOR
    static SHMAccess *m_poTerminalSHM;
    static __HSS::THeadInfo *m_poTerminal;    
};

#endif/*__HSS_SRV_SHM_MGR_H_INCLUDED_*/
