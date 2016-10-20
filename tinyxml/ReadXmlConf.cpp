#include "ReadXmlConf.h"

#define ABM_HOME "ABMAPP_DEPLOY"

ReadCfg * XmlConf::m_poConf = NULL;

int XmlConf::getXmlConf(int proc, XmlConfStru &conf, ABMException &oExp)
{
    char * const pLable = "XML_CONF";
    if (m_poConf == NULL) { 
        if ((m_poConf=new ReadCfg) == NULL) {
            ADD_EXCEPT0(oExp, "ABMConf::getConf m_poConf malloc failed!");
            return -1;
        }
    }    
    char *p = getenv(ABM_HOME);
    if (p == NULL) {
        ADD_EXCEPT0(oExp, "AccuConf::getConf ABMAPP_DEPLOY 环境变量没有设置!");
        return -1;
    }
    
    char sValue[128];
    char path[1024];
    char tmp[1024] = {0};
    strncpy(tmp, p, sizeof(tmp)-2);
    if (tmp[strlen(tmp)-1] != '/') {
        tmp[strlen(tmp)] = '/';
    }
    snprintf(path, sizeof(path), "%sconfig/abm_app_cfg.ini", tmp);
    if (m_poConf->read(oExp, path, pLable) != 0) {
        ADD_EXCEPT2(oExp, "AccuConf::getConf 读取配置文件%s中的%s出错", path, pLable);
        return -1;
    }  
    
    int iLines = m_poConf->getRows();

    while(iLines > 0) {
        memset(sValue, 0x00, sizeof(sValue));
        if (!m_poConf->getValue(oExp, sValue, "proc", iLines)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现proc选项,读取失败!", pLable);
            return -1;
        }
        if (proc != atoi(sValue)) {
            iLines--;
            continue;
        }
        
        memset(sValue, 0x00, sizeof(sValue));
        if (!m_poConf->getValue(oExp, sValue, "mqsnd", iLines)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现mqsnd选项,读取失败!", pLable);
            return -1;
        }
        conf.m_iSndKey = atoi(sValue);
        
        memset(sValue, 0x00, sizeof(sValue));
        if (!m_poConf->getValue(oExp, sValue, "mqsend_peer", iLines)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现mqsnd选项,读取失败!", pLable);
            return -1;
        }
        conf.m_iSndPeerKey = atoi(sValue);

        memset(sValue, 0x00, sizeof(sValue));
        if (!m_poConf->getValue(oExp, sValue, "mqrcv", iLines)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现mqrcv选项,读取失败!", pLable);
            return -1;
        }
        conf.m_iRcvKey = atoi(sValue);
		//回心跳包
		memset(sValue, 0x00, sizeof(sValue));
        if (!m_poConf->getValue(oExp, sValue, "mqsend_cca", iLines)) {
            ADD_EXCEPT1(oExp, "配置文件中%s下未发现mqrcv选项,读取失败!", pLable);
            return -1;
        }
        conf.m_iSndCCAKey = atoi(sValue);
        break;
    }
    
    conf.m_iProcID = proc;
    
    return !iLines;
}
