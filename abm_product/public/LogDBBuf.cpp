#include "LogDBBuf.h"
#include "Environment.h"

USERINFO_SHM_DATA_TYPE<LogDBSHMData> *LogDBBuf::m_poLogDBData = 0;
CSemaphore * LogDBBuf::m_poLogDBDataLock = 0;

#define LOGDB_DATA      (IpcKeyMgr::getIpcKey(-1, "SHM_LogDBData"))
#define LOGDB_DATA_LOCK (IpcKeyMgr::getIpcKey(-1, "LOCK_LogDBData"))

LogDBBuf::LogDBBuf()
{
    char sTemp[32];
    
    if(!m_poLogDBDataLock){
        m_poLogDBDataLock = new CSemaphore ();
        sprintf(sTemp, "%d", LOGDB_DATA_LOCK);
        m_poLogDBDataLock->getSem (sTemp, 1, 1);
    }
    
    m_poLogDBData = new USERINFO_SHM_DATA_TYPE<LogDBSHMData> (LOGDB_DATA);
    
    if(!(m_poLogDBData)){
        if(m_poLogDBData){
            delete m_poLogDBData;
            m_poLogDBData = 0;
        }
        
        THROW(MBC_UserInfo+1);
    }
    
    if(!(m_poLogDBData->exist())){
        m_poLogDBData->create(1);
        init();
    }
}

LogDBBuf::~LogDBBuf()
{
    if(m_poLogDBData){
        delete m_poLogDBData;
        m_poLogDBData = 0;
    }
}

void LogDBBuf::unloadSHMBuf()
{
    if(m_poLogDBData->exist())
        m_poLogDBData->remove();
}

void LogDBBuf::init()
{
    LogDBSHMData * p = (LogDBSHMData *)(*m_poLogDBData);
    
    p[1].iWriteNum = 0;
    p[1].iReadNum = 0;
    p[1].iMaxNum = MAX_SHMBUF_NUM;
    p[1].iFullNum = MAX_SHMBUF_NUM - 1;
    
    memset(p[1].oLogDBData, 0, sizeof(LogDBData)*MAX_SHMBUF_NUM);
}

int LogDBBuf::inUseNum()
{
    LogDBSHMData * p = (LogDBSHMData *)(*m_poLogDBData);
    
    p[1].iWriteNum %= p[1].iMaxNum;
    p[1].iReadNum %= p[1].iMaxNum;
    
    if( p[1].iWriteNum > p[1].iReadNum )
        return p[1].iWriteNum - p[1].iReadNum;
    else if( p[1].iWriteNum < p[1].iReadNum )
        return p[1].iMaxNum - (p[1].iReadNum - p[1].iWriteNum);
    else
        return 0;
}

int LogDBBuf::freeUseNum()
{
    LogDBSHMData * p = (LogDBSHMData *)(*m_poLogDBData);
    
    return p[1].iFullNum - inUseNum();
}

int LogDBBuf::totleNum()
{
    LogDBSHMData * p = (LogDBSHMData *)(*m_poLogDBData);
    
    return p[1].iFullNum;
}

bool LogDBBuf::isFull()
{
    LogDBSHMData * p = (LogDBSHMData *)(*m_poLogDBData);
    
    return p[1].iFullNum == inUseNum();
}

bool LogDBBuf::isEmpty()
{
    return inUseNum() == 0;
}

bool LogDBBuf::writeCirBuf(LogDBData & oLogDBData)
{
    bool bRet = false;
    LogDBSHMData * p = (LogDBSHMData *)(*m_poLogDBData);
    
    m_poLogDBDataLock->P();
    if(!isFull()){
        memset(&p[1].oLogDBData[p[1].iWriteNum], 0, sizeof(LogDBData));
        p[1].oLogDBData[p[1].iWriteNum] = oLogDBData;
        p[1].iWriteNum ++;
        p[1].iWriteNum %= p[1].iMaxNum;
        bRet = true;
    }
    m_poLogDBDataLock->V();
    
    return bRet;
}

bool LogDBBuf::readCirBuf(LogDBData & oLogDBData)
{
    bool bRet = true;
    LogDBSHMData * p = (LogDBSHMData *)(*m_poLogDBData);
    
    m_poLogDBDataLock->P();
    if(isEmpty())
        bRet = false;
    else{
        oLogDBData = p[1].oLogDBData[p[1].iReadNum ++];
        p[1].iReadNum %= p[1].iMaxNum;
    }
    m_poLogDBDataLock->V();
    
    return bRet;
}