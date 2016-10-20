#include "SHMParamInfo.h"
#include "ABMException.h"
#include "ReadParamConf.h"
#include "HSSLogV3.h"
#include <vector>
#include <sys/time.h>

static bool g_bParamInit = false;

#define SHM_HEAD_CODE_DATA_KEY  "SHM_HEAD_CODE_DATA_KEY"
#define SHM_HEAD_CODE_INDEX_KEY "SHM_HEAD_CODE_INDEX_KEY"
#define SHM_MIN_INFO_DATA_KEY "SHM_MIN_INFO_DATA_KEY"
#define SHM_MIN_INFO_INDEX_KEY "SHM_MIN_INFO_INDEX_KEY"
#define SHM_IMSI_INFO_DATA_KEY "SHM_IMSI_INFO_DATA_KEY"
#define SHM_IMSI_INFO_INDEX_KEY "SHM_IMSI_INFO_INDEX_KEY" 

SHMData<HCodeInfoStruct> *SHMParamInfo::m_poHCodeData = NULL;
SHMStringTreeIndex *SHMParamInfo::m_poHCodeIndex = NULL;
SHMData<MinInfoStruct> *SHMParamInfo::m_poMinData = NULL;
SHMStringTreeIndex *SHMParamInfo::m_poMinIndex = NULL;
SHMData<IMSIInfoStruct> *SHMParamInfo::m_poIMSIData = NULL;
SHMStringTreeIndex      *SHMParamInfo::m_poIMSIIndex = NULL;     
    
static long getDayTime()
{
    struct timeval tvstart;
    struct timezone tz;
    gettimeofday(&tvstart, &tz);
    struct tm *p = localtime(&tvstart.tv_sec);
    return ((p->tm_year+1900)*100 + p->tm_mon+1)*100 + p->tm_mday;   
}            

SHMParamInfo::SHMParamInfo()
{

}

SHMParamInfo::~SHMParamInfo()
{

}

int SHMParamInfo::openSHM(ABMException &oExp, bool bIfCrt)
{
	if (g_bParamInit)
		return 0;

    int iRet;
    try {
        if ((iRet=openHCode(oExp, bIfCrt)) != 0)
            return -1;
        if ((iRet=openMinInfo(oExp, bIfCrt)) != 0)
            return -1;
        if ((iRet=openIMSIInfo(oExp, bIfCrt)) != 0)
            return -1;
		g_bParamInit = true;
        return 0;    
    }
    catch(...) {
        ADD_EXCEPT0(oExp, "共享内存分配失败 检查内存参数设置!");
        DIRECT_LOG(V3_FATAL, V3_PARAM_MGR, MBC_MEM_MALLOC_FAIL, "共享内存分配失败 检查内存参数设置!");       
    }                      
    return -1;    
}

int SHMParamInfo::openHCode(ABMException &oExp, bool bIfCrt)    
{
    SHMParamConf oParamConf;
    //数据内存
    if (ReadParamConf::getSHMParamConf(oExp, SHM_HEAD_CODE_DATA_KEY, oParamConf) != 0) {
        ADD_EXCEPT1(oExp, "SHMParamInfo::openHCode key= \"%s\" read config failed!", SHM_HEAD_CODE_DATA_KEY);
        return -1; 
    }
    if ((m_poHCodeData=new SHMData<HCodeInfoStruct>(oParamConf.m_ulKey)) == NULL) {
        ADD_EXCEPT0(oExp, "SHMParamInfo::openHCode m_poHCodeData malloc failed!");
        return -1;
    }
    if (!m_poHCodeData->exist()) {
        if (bIfCrt)
            m_poHCodeData->create(oParamConf.m_uiNum);
        else {
            //共享内存未创建 连接失败
            ADD_EXCEPT1(oExp, "SHMParamInfo::openHCode key=%d SHM not exist! create first!", oParamConf.m_ulKey);
            DIRECT_LOG(V3_FATAL, V3_PARAM_MGR, MBC_MEM_LINK_FAIL, \
                "shmget key=%d 链接共享内存失败, 请先运行内存管理程序shmmgr 创建！", oParamConf.m_ulKey);
            return -1;
        }
    }
    
    //索引内存
    if (ReadParamConf::getSHMParamConf(oExp, SHM_HEAD_CODE_INDEX_KEY, oParamConf) != 0) {
        ADD_EXCEPT1(oExp, "ReadParamConf::getSHMParamKey key= \"%s\" read config failed!", SHM_HEAD_CODE_INDEX_KEY);
        return -1;
    }
    if ((m_poHCodeIndex=new SHMStringTreeIndex(oParamConf.m_ulKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ReadParamConf::getSHMParamKey m_poHCodeIndex malloc failed!");
        return -1;
    }
    if (!m_poHCodeIndex->exist()) {
        if (bIfCrt)
            m_poHCodeIndex->create(oParamConf.m_uiNum, oParamConf.m_uiStrLen, oParamConf.m_uiMulti);
        else {
            //共享内存未创建 连接失败
            ADD_EXCEPT1(oExp, "SHMParamInfo::openHCode key=%d SHM not exist! create first!", oParamConf.m_ulKey);
            DIRECT_LOG(V3_FATAL, V3_PARAM_MGR, MBC_MEM_LINK_FAIL, \
                "shmget key=%d 链接共享内存失败, 请先运行内存管理程序shmmgr 创建！", oParamConf.m_ulKey);
            return -1;
        }
    }
    
    return 0;
}

bool SHMParamInfo::getHeadCode(char *pHead, HCodeInfoStruct &oValue, long lTime)
{
    std::vector<unsigned int> vIdx;
    if (SHM(m_poHCodeIndex)->getAll(pHead, vIdx) == 0)
        return false;
    HCodeInfoStruct *pData;
    HCodeInfoStruct *pBegin = (HCodeInfoStruct *)(*SHM(m_poHCodeData));
    if (lTime == 0) {
        lTime = getDayTime();        
    }
    
    for (int i=0; i<vIdx.size(); ++i) {    
        pData = pBegin + vIdx[i];
        if ((lTime<atol(pData->m_sEffDate)) || (lTime>=atol(pData->m_sExpDate)))
            continue;
        memcpy((void *)&oValue, (const void *)pData, sizeof(oValue));
        return true;
    }
    return false;
}

int SHMParamInfo::openMinInfo(ABMException &oExp, bool bIfCrt)
{
    SHMParamConf oParamConf;
    //数据内存
    if (ReadParamConf::getSHMParamConf(oExp, SHM_MIN_INFO_DATA_KEY, oParamConf) != 0) {
        ADD_EXCEPT1(oExp, "SHMParamInfo::openHCode key= \"%s\" read config failed!", SHM_MIN_INFO_DATA_KEY);
        return -1; 
    }
    if ((m_poMinData=new SHMData<MinInfoStruct>(oParamConf.m_ulKey)) == NULL) {
        ADD_EXCEPT0(oExp, "SHMParamInfo::openMinInfo m_poMinData malloc failed!");
        return -1;
    }
    if (!m_poMinData->exist()) {
        if (bIfCrt) 
            m_poMinData->create(oParamConf.m_uiNum);
        else {
            //共享内存未创建 连接失败
            ADD_EXCEPT1(oExp, "SHMParamInfo::openMinInfo key=%d SHM not exist! create first!", oParamConf.m_ulKey);
            DIRECT_LOG(V3_FATAL, V3_PARAM_MGR, MBC_MEM_LINK_FAIL, \
                "shmget key=%d 链接共享内存失败, 请先运行内存管理程序shmmgr 创建！", oParamConf.m_ulKey);
            return -1;    
        }                 
    }
    
    //索引内存
    if (ReadParamConf::getSHMParamConf(oExp, SHM_MIN_INFO_INDEX_KEY, oParamConf) != 0) {
        ADD_EXCEPT1(oExp, "ReadParamConf::getSHMParamKey key= \"%s\" read config failed!", SHM_MIN_INFO_INDEX_KEY);
        return -1; 
    }
    if ((m_poMinIndex=new SHMStringTreeIndex(oParamConf.m_ulKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ReadParamConf::getSHMParamKey m_poMinIndex malloc failed!");
        return -1;
    }
    if (!m_poMinIndex->exist()) {
        if (bIfCrt) 
            m_poMinIndex->create(oParamConf.m_uiNum, oParamConf.m_uiStrLen, oParamConf.m_uiMulti);
        else {
            //共享内存未创建 连接失败
            ADD_EXCEPT1(oExp, "SHMParamInfo::openMinInfo key=%d SHM not exist! create first!", oParamConf.m_ulKey);
            DIRECT_LOG(V3_FATAL, V3_PARAM_MGR, MBC_MEM_LINK_FAIL, \
                "shmget key=%d 链接共享内存失败, 请先运行内存管理程序shmmgr 创建！", oParamConf.m_ulKey);
            return -1;    
        }
    }
    
    return 0;    
}

bool SHMParamInfo::getMinInfo(char *psMinBegin, MinInfoStruct &oValue, long lTime)
{
    std::vector<unsigned int> vIdx;
    if (SHM(m_poMinIndex)->getAll(psMinBegin, vIdx) == 0)
        return false;
    MinInfoStruct *pData;    
    MinInfoStruct *pBegin = (MinInfoStruct *)(*SHM(m_poMinData));
    if (lTime == 0) {
        lTime = getDayTime();        
    } 
    
    for (int i=0; i<vIdx.size(); ++i) {    
        pData = pBegin + vIdx[i];
        if ((lTime<atol(pData->m_sEffDate)) || (lTime>=atol(pData->m_sExpDate)))
            continue;
        memcpy((void *)&oValue, (const void *)pData, sizeof(oValue));
        return true;
    }    
    return false;
}

int SHMParamInfo::openIMSIInfo(ABMException &oExp, bool bIfCrt)
{
    SHMParamConf oParamConf;
    //数据内存
    if (ReadParamConf::getSHMParamConf(oExp, SHM_IMSI_INFO_DATA_KEY, oParamConf) != 0) {
        ADD_EXCEPT1(oExp, "SHMParamInfo::openIMSIInfo key= \"%s\" read config failed!", SHM_IMSI_INFO_DATA_KEY);
        return -1; 
    }
    if ((m_poIMSIData=new SHMData<IMSIInfoStruct>(oParamConf.m_ulKey)) == NULL) {
        ADD_EXCEPT0(oExp, "SHMParamInfo::openIMSIInfo m_poMinData malloc failed!");
        return -1;
    }
    if (!m_poIMSIData->exist()) {
        if (bIfCrt) 
            m_poIMSIData->create(oParamConf.m_uiNum);
        else {
            //共享内存未创建 连接失败
            ADD_EXCEPT1(oExp, "SHMParamInfo::m_poIMSIData key=%d SHM not exist! create first!", oParamConf.m_ulKey);
            DIRECT_LOG(V3_FATAL, V3_PARAM_MGR, MBC_MEM_LINK_FAIL, \
                "shmget key=%d 链接共享内存失败, 请先运行内存管理程序shmmgr 创建！", oParamConf.m_ulKey);
            return -2;    
        }                 
    }
    
    //索引内存
    if (ReadParamConf::getSHMParamConf(oExp, SHM_IMSI_INFO_INDEX_KEY, oParamConf) != 0) {
        ADD_EXCEPT1(oExp, "ReadParamConf::getSHMParamKey key= \"%s\" read config failed!", SHM_IMSI_INFO_INDEX_KEY);
        return -1; 
    }
    if ((m_poIMSIIndex=new SHMStringTreeIndex(oParamConf.m_ulKey)) == NULL) {
        ADD_EXCEPT0(oExp, "ReadParamConf::getSHMParamKey m_poMinIndex malloc failed!");
        return -1;
    }
    if (!m_poIMSIIndex->exist()) {
        if (bIfCrt) 
            m_poIMSIIndex->create(oParamConf.m_uiNum, oParamConf.m_uiStrLen, oParamConf.m_uiMulti);
        else {
            //共享内存未创建 连接失败
            ADD_EXCEPT1(oExp, "SHMParamInfo::openIMSIInfo key=%d SHM not exist! create first!", oParamConf.m_ulKey);
            DIRECT_LOG(V3_FATAL, V3_PARAM_MGR, MBC_MEM_LINK_FAIL, \
                "shmget key=%d 链接共享内存失败, 请先运行内存管理程序shmmgr 创建！", oParamConf.m_ulKey);
            return -2;    
        }
    }
    
    return 0;    
}    

bool SHMParamInfo::getIMSI(char *psIMSIBegin, IMSIInfoStruct &oValue, long lTime)
{
    std::vector<unsigned int> vIdx;
    if (SHM(m_poIMSIIndex)->getAll(psIMSIBegin, vIdx) == 0)
        return false;
    IMSIInfoStruct *pData;
    IMSIInfoStruct *pBegin = (IMSIInfoStruct *)(*SHM(m_poIMSIData));
    if (lTime == 0) {
        lTime = getDayTime();
    }
    
    for (int i=0; i<vIdx.size(); ++i) {
        pData = pBegin + vIdx[i];
        if ((lTime<atol(pData->m_sEffDate)) || (lTime>=atol(pData->m_sExpDate)))
            continue;
        memcpy((void *)&oValue, (const void *)pData, sizeof(oValue));
        return true;
    }
    return false;
}
