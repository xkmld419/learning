#include "SHMFileInfo.h"

UserInfoBase * SHMFileInfo::m_pSHMFileExternal = 0;
UserInfoBase * SHMFileInfo::m_pSHMInternal = 0;
UserInfoBase * SHMFileInfo::m_pFileInternal = 0;
SHMFileCtl * SHMFileInfo::m_pSHMFileCtl = 0;

SHMFileInfo::SHMFileInfo()
{
    int iTemp = 0;
    
    if(!m_pSHMFileCtl){
        m_pSHMFileCtl = new SHMFileCtl (iTemp);
        refleshPointer();
    }
}

SHMFileInfo::~SHMFileInfo()
{
    /*if(m_pSHMInternal){
        delete m_pSHMInternal;
        m_pSHMInternal = 0;
    }
    
    if(m_pFileInternal){
        delete m_pFileInternal;
        m_pFileInternal = 0;
    }
    
    if(m_pSHMFileCtl){
        delete m_pSHMFileCtl;
        m_pSHMFileCtl = 0;
    }*/
}

void SHMFileInfo::refleshPointer()
{
    if(m_pSHMFileCtl->m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] == FILE_POINT_STATE){
        if(m_pSHMInternal){
            m_pSHMInternal->closeAll();
            m_pSHMInternal->freeAll();
            m_pSHMInternal->m_bAttached = false;
            delete m_pSHMInternal;
            m_pSHMInternal = 0;
            printf("detach share mem succ!\n");
        }
        
        if(!m_pFileInternal){
            m_pFileInternal = new UserInfoBase(m_pSHMFileCtl->getFileName());
            m_pSHMFileExternal = m_pFileInternal;
            
            m_pSHMFileCtl->setProcessMode(FILE_POINT_STATE, getpid());
            
            printf("attach file map mem succ!\n");
        }
        
        if(!m_pFileInternal && m_pSHMInternal){
            m_pSHMInternal = new UserInfoBase();
            m_pSHMFileExternal = m_pSHMInternal;
            printf("attach file map mem err!\n");
        }
    }else if(m_pSHMFileCtl->m_poStateData->m_iState[EXTERNAL_INTERFACE_STATE] == SHM_POINT_STATE){
        if(m_pFileInternal){
            m_pFileInternal->closeAll();
            m_pFileInternal->freeAll();
            m_pFileInternal->m_bAttached = false;
            delete m_pFileInternal;
            m_pFileInternal = 0;
            printf("detach file map mem succ!\n");
        }
        
        if(!m_pSHMInternal){
            m_pSHMInternal = new UserInfoBase();
            m_pSHMFileExternal = m_pSHMInternal;
            
            m_pSHMFileCtl->setProcessMode(SHM_POINT_STATE, getpid());
            
            printf("attach share mem succ!\n");
        }
        
        if(!m_pSHMInternal && m_pFileInternal){
            m_pFileInternal = new UserInfoBase(m_pSHMFileCtl->getFileName());
            m_pSHMFileExternal = m_pFileInternal;
            printf("attach share mem err!\n");
        }
    }else
        printf("userinfo change state err!\n");
}