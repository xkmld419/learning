#include "ReadParamConf.h"
#include "ABMException.h"
#include "HSSLogV3.h"
#include "CommandCom.h"
#include "ReadCfg.h"

// #define SHMCONFIG_FROM_KERNEL

#ifdef SHMCONFIG_FROM_KERNEL
#include "CommandCom.h"

#define SHM_HEAD_CODE_DATA_KEY  "SHM_HEAD_CODE_DATA_KEY"
#define SHM_HEAD_CODE_INDEX_KEY "SHM_HEAD_CODE_INDEX_KEY"
#define SHM_MIN_INFO_DATA_KEY "SHM_MIN_INFO_DATA_KEY"
#define SHM_MIN_INFO_INDEX_KEY "SHM_MIN_INFO_INDEX_KEY"
#define SHM_IMSI_INFO_DATA_KEY "SHM_IMSI_INFO_DATA_KEY"
#define SHM_IMSI_INFO_INDEX_KEY "SHM_IMSI_INFO_INDEX_KEY" 

#define HCODE_DATA_KEY "hcodedata.ipc_key_value"
#define HCODE_DATA_SIZE "hcodedata.max_shm_size"
#define HCODE_INDEX_KEY "hcodeindex.ipc_key_value"
#define HCODE_INDEX_SIZE "hcodeindex.max_shm_size"

#define MIN_DATA_KEY "mindata.ipc_key_value"
#define MIN_DATA_SIZE "mindata.max_shm_size"
#define MIN_INDEX_KEY "minindex.ipc_key_value"
#define MIN_INDEX_SIZE "minindex.max_shm_size"

#define IMSI_DATA_KEY "imsidata.ipc_key_value"
#define IMSI_DATA_SIZE "imsidata.max_shm_size"
#define IMSI_INDEX_KEY "imsiindex.ipc_key_value"
#define IMSI_INDEX_SIZE "imsiindex.max_shm_size"

static CommandCom *g_poKern = NULL;

#endif

ReadCfg *ReadParamConf::m_poConf = NULL;
#define ABM_PRO_HOME "ABM_PRO_HOME"

int ReadParamConf::getSHMParamConf(ABMException &oExp, const char* pKey, SHMParamConf& oConf)
{
    char * const pLable = "SHM_PARAM";
    if (m_poConf == NULL) { 
        if ((m_poConf=new ReadCfg) == NULL) {
            ADD_EXCEPT0(oExp, "ReadParamConf::getSHMParamConf m_poConf malloc failed!");
            return -1;
        }
    }    
    char *p = getenv("ABM_PRO_DIR");
    if (p == NULL) {
        ADD_EXCEPT0(oExp, "ReadParamConf::getSHMParamConf ABM_PRO_DIR 环境变量没有设置!");
        return -1;
    }
    
    char sValue[128];
    char path[1024];
    char tmp[1024] = {0};
    strncpy(tmp, p, sizeof(tmp)-2);
    if (tmp[strlen(tmp)-1] != '/') {
        tmp[strlen(tmp)] = '/';
    }
    snprintf(path, sizeof(path), "%setc/hss_app_cfg.ini", tmp);
    if (m_poConf->read(oExp, path, pLable) != 0) {
        ADD_EXCEPT2(oExp, "ReadParamConf::getSHMParamConf 读取配置文件%s中的%s出错", path, pLable);
        return -1;
    }  
    
    int iLines = m_poConf->getRows();
		
    while(iLines > 0) {
        memset(sValue, 0x00, sizeof(sValue));
        if (!m_poConf->getValue(oExp, sValue, "key-name", iLines)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现key-id选项,读取失败!", pLable);
            return -1;
        }
        if (strcmp(pKey, sValue)) {
            iLines--;
            continue;
        }
        memset(sValue, 0x00, sizeof(sValue));
        if (!m_poConf->getValue(oExp, sValue, "key-id", iLines)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现mqsnd选项,读取失败!", pLable);
            return -1;
        }
        oConf.m_ulKey = atoi(sValue);
				
        memset(sValue, 0x00, sizeof(sValue));
        if (!m_poConf->getValue(oExp, sValue, "unit-num", iLines)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现unit-num选项,读取失败!", pLable);
            return -1;
        }
        oConf.m_uiNum = atoi(sValue);
        
        memset(sValue, 0x00, sizeof(sValue));
        if (!m_poConf->getValue(oExp, sValue, "string-len", iLines)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现string-len选项,读取失败!", pLable);
            return -1;
        }
        oConf.m_uiStrLen = atoi(sValue);
        
        memset(sValue, 0x00, sizeof(sValue));
        if (!m_poConf->getValue(oExp, sValue, "multi", iLines)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现multi选项,读取失败!", pLable);
            return -1;
        }
        oConf.m_uiMulti = atoi(sValue);
        break;
    }
    
    //从核心参数取值
    #ifdef SHMCONFIG_FROM_KERNEL
    g_poKern = getKernel();
    char * const pKernLabel = "MEMORY";
    char sKernVal[64], sDefVal[64];
    if (strcmp(pKey, SHM_HEAD_CODE_DATA_KEY) == 0) {
        memset(sKernVal, 0x00, sizeof(sKernVal));
        g_poKern->readIniString(pKernLabel, HCODE_DATA_KEY, sKernVal, sDefVal);
        if (sKernVal[0] != '\0')
            oConf.m_ulKey = atoi(sKernVal);
        memset(sKernVal, 0x00, sizeof(sKernVal));
        g_poKern->readIniString(pKernLabel, HCODE_DATA_SIZE, sKernVal, sDefVal);
        if (sKernVal[0] != '\0')
            oConf.m_uiNum = atoi(sKernVal);
    }
    else if (strcmp(pKey, SHM_HEAD_CODE_INDEX_KEY) == 0) {
        memset(sKernVal, 0x00, sizeof(sKernVal));
        g_poKern->readIniString(pKernLabel, HCODE_INDEX_KEY, sKernVal, sDefVal);
        if (sKernVal[0] != '\0')
            oConf.m_ulKey = atoi(sKernVal);
        memset(sKernVal, 0x00, sizeof(sKernVal));
        g_poKern->readIniString(pKernLabel, HCODE_INDEX_SIZE, sKernVal, sDefVal);
        if (sKernVal[0] != '\0')
            oConf.m_uiNum = atoi(sKernVal);
    }
    else if (strcmp(pKey, SHM_MIN_INFO_DATA_KEY) == 0) {
        memset(sKernVal, 0x00, sizeof(sKernVal));
        g_poKern->readIniString(pKernLabel, MIN_DATA_KEY, sKernVal, sDefVal);
        if (sKernVal[0] != '\0')
            oConf.m_ulKey = atoi(sKernVal);
        memset(sKernVal, 0x00, sizeof(sKernVal));
        g_poKern->readIniString(pKernLabel, MIN_DATA_SIZE, sKernVal, sDefVal);
        if (sKernVal[0] != '\0')
            oConf.m_uiNum = atoi(sKernVal);
    } 
    else if (strcmp(pKey, SHM_MIN_INFO_INDEX_KEY) == 0) {
        memset(sKernVal, 0x00, sizeof(sKernVal));
        g_poKern->readIniString(pKernLabel, MIN_INDEX_KEY, sKernVal, sDefVal);
        if (sKernVal[0] != '\0')
            oConf.m_ulKey = atoi(sKernVal);
        memset(sKernVal, 0x00, sizeof(sKernVal));
        g_poKern->readIniString(pKernLabel, MIN_INDEX_SIZE, sKernVal, sDefVal);
        if (sKernVal[0] != '\0')
            oConf.m_uiNum = atoi(sKernVal);
    } 
    else if (strcmp(pKey, SHM_IMSI_INFO_DATA_KEY) == 0) {
        memset(sKernVal, 0x00, sizeof(sKernVal));
        g_poKern->readIniString(pKernLabel, IMSI_DATA_KEY, sKernVal, sDefVal);
        if (sKernVal[0] != '\0')
            oConf.m_ulKey = atoi(sKernVal);
        memset(sKernVal, 0x00, sizeof(sKernVal));
        g_poKern->readIniString(pKernLabel, IMSI_DATA_SIZE, sKernVal, sDefVal);
        if (sKernVal[0] != '\0')
            oConf.m_uiNum = atoi(sKernVal);
    } 
    else if (strcmp(pKey, SHM_IMSI_INFO_INDEX_KEY) == 0) {
        memset(sKernVal, 0x00, sizeof(sKernVal));
        g_poKern->readIniString(pKernLabel, IMSI_INDEX_KEY, sKernVal, sDefVal);
        if (sKernVal[0] != '\0')
            oConf.m_ulKey = atoi(sKernVal);
        memset(sKernVal, 0x00, sizeof(sKernVal));
        g_poKern->readIniString(pKernLabel, IMSI_INDEX_SIZE, sKernVal, sDefVal);
        if (sKernVal[0] != '\0')
            oConf.m_uiNum = atoi(sKernVal);
    }                 
    __End:
    #endif
    
    return !iLines;
}
 
 
int ReadParamConf::getSemConf(const char* pKey, char *pValue, int iSize)
{
#ifdef SHMCONFIG_FROM_KERNEL    
    g_poKern = getKernel();
    char sKernVal[64], sDefVal[64];
    char * const pKernLabel = "IPC";
    memset(sKernVal, 0x00, sizeof(sKernVal));
    g_poKern->readIniString(pKernLabel, pKey, sKernVal, sDefVal);
    if (sKernVal[0] == '\0')
        return -2;
    else
        snprintf(pValue, iSize, "%s", sKernVal);    
    return 0;
#endif 
    return 0;               
}     
