#include "SockSrvConf.h"
#include "ReadCfg.h"
#include "LogV2.h"

ReadCfg * ReadSockConf::m_poRead = NULL;

//根据传入的进程ID 读取该进程配置下的SOCKET服务IP PORT
int ReadSockConf::getListenConf(int iProc, SockSrvConf &oConf)
{
    int iRet = -1;
    ABMException oExp;
    char sFile[1024];
    if (m_poRead == NULL) {
        if ((m_poRead=new ReadCfg) == NULL) {
            __DEBUG_LOG0_(0, "ReadSockConf::getListenConf m_poRead malloc failed!");
            return -1;    
        }    
    }
    char *p = getenv("ABMAPP_DEPLOY");
    if (!p) {
        __DEBUG_LOG0_(0, "ReadSockConf::getListenConf ABMAPP_DEPLOY 环境变量缺失!");
        return -1;
    }
    if (p[strlen(p)-1] != '/')
        snprintf (sFile, sizeof(sFile)-1, "%s/config/abm_app_cfg.ini", p);
    else 
        snprintf (sFile, sizeof(sFile)-1, "%sconfig/abm_app_cfg.ini", p);
   
    //读取配置    
    if (m_poRead->read(oExp, sFile, "SOCK_SRV" , '|') == -1) {
        __DEBUG_LOG1_(0, "ReadSockConf::getListenConf read \"%s\" failed!", sFile);
        return -1;
    }    
    
    char sValue[64];
    int iRows = m_poRead->getRows();
    for (int i=1; i<=iRows; ++i) {
        if (!m_poRead->getValue(oExp, sValue, "proc", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"proc\", %d) failed", i);
			return -1;	
		}
		if (atoi(sValue) != iProc)
		    continue;
		if (!m_poRead->getValue(oExp, sValue, "localip", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"localip\", %d) failed", i);
			return -1;	
		}
		snprintf(oConf.m_sLocIP, sizeof(oConf.m_sLocIP), "%s", sValue);
		if (!m_poRead->getValue(oExp, sValue, "localport", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"localport\", %d) failed", i);
			return -1;	
		} 
		oConf.m_iLocPort = atoi(sValue);
		iRet = 0;
		break;   
    }
    
    return iRet;
}

//根据传入的进程ID IP PORT 去配置文件中读取其他配置
int ReadSockConf::getAcceptConf(int iProc, char *pIP, int iPort, SockSrvConf &oConf)
{
    if ((pIP==NULL) || (iPort <= 0)) {
        __DEBUG_LOG2_(0, "ReadSockConf::getAcceptConf pIP=%s,iPort=%d error", pIP, iPort);
        return -1;        
    }
    int iRet = -1;
    ABMException oExp;
    char sFile[1024];
    if (m_poRead == NULL) {
        if ((m_poRead=new ReadCfg) == NULL) {
            __DEBUG_LOG0_(0, "ReadSockConf::getAcceptConf m_poRead malloc failed!");
            return -1;    
        }    
    }
    char *p = getenv("ABMAPP_DEPLOY");
    if (!p) {
        __DEBUG_LOG0_(0, "ReadSockConf::getListenConf ABMAPP_DEPLOY 环境变量缺失!");
        return -1;
    }
    if (p[strlen(p)-1] != '/')
        snprintf (sFile, sizeof(sFile)-1, "%s/config/abm_app_cfg.ini", p);
    else 
        snprintf (sFile, sizeof(sFile)-1, "%sconfig/abm_app_cfg.ini", p);
   
    //读取配置    
    if (m_poRead->read(oExp, sFile, "SOCK_SRV" , '|') == -1) {
        __DEBUG_LOG1_(0, "ReadSockConf::getListenConf read \"%s\" failed!", sFile);
        return -1;
    }    
    
    char sValue[64];
    int iRows = m_poRead->getRows();
    for (int i=1; i<=iRows; ++i) {
        if (!m_poRead->getValue(oExp, sValue, "proc", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"proc\", %d) failed", i);
			return -1;	
		}
		if (atoi(sValue) != iProc)
		    continue;
		if (!m_poRead->getValue(oExp, sValue, "remoteip", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"remoteip\", %d) failed", i);
			return -1;	
		}   
		if (strcmp(pIP, sValue))
		    continue;
		if (!m_poRead->getValue(oExp, sValue, "remoteport", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"remoteport\", %d) failed", i);
			return -1;	
		}   
		if (iPort != atoi(sValue))
		    continue; 
		        
		if (!m_poRead->getValue(oExp, sValue, "rcvmq", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"rcvmq\", %d) failed", i);
			return -1;	
		}
		oConf.m_iRcvMQ = atoi(sValue);
		if (!m_poRead->getValue(oExp, sValue, "sndmq", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"sndmq\", %d) failed", i);
			return -1;	
		}
		oConf.m_iSndMQ = atoi(sValue);
		if (!m_poRead->getValue(oExp, sValue, "linkid", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"linkid\", %d) failed", i);
			return -1;	
		} 
		oConf.m_iLinkID = atoi(sValue);
		if (!m_poRead->getValue(oExp, sValue, "option", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"option\", %d) failed", i);
			return -1;	
		} 
		oConf.m_iOpt = atoi(sValue);
		
		iRet = 0;
		break;   
    }
    
    return iRet;        
}

//根据传入的进程ID, 把配置信息读取到容器里
int ReadSockConf::getConnectConf(int iProc, vector<SockSrvConf> &vConf)
{
    int iRet = -1;
    ABMException oExp;
    SockSrvConf oConf;
    char sFile[1024];
    if (m_poRead == NULL) {
        if ((m_poRead=new ReadCfg) == NULL) {
            __DEBUG_LOG0_(0, "ReadSockConf::getConnectConf m_poRead malloc failed!");
            return -1;    
        }    
    }
    char *p = getenv("ABMAPP_DEPLOY");
    if (!p) {
        __DEBUG_LOG0_(0, "ReadSockConf::getConnectConf ABMAPP_DEPLOY 环境变量缺失!");
        return -1;
    }
    if (p[strlen(p)-1] != '/')
        snprintf (sFile, sizeof(sFile)-1, "%s/config/abm_app_cfg.ini", p);
    else 
        snprintf (sFile, sizeof(sFile)-1, "%sconfig/abm_app_cfg.ini", p);
   
    //读取配置    
    if (m_poRead->read(oExp, sFile, "SOCK_CLNT" , '|') == -1) {
        LogV2::logExcep(oExp);
        __DEBUG_LOG1_(0, "ReadSockConf::getConnectConf read \"%s\" failed!", sFile);
        return -1;
    }    
    
    char sValue[64];
    int iRows = m_poRead->getRows();
    for (int i=1; i<=iRows; ++i) {
        if (!m_poRead->getValue(oExp, sValue, "proc", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"proc\", %d) failed", i);
			return -1;	
		}
		if (atoi(sValue) != iProc)
		    continue;
		if (!m_poRead->getValue(oExp, sValue, "localip", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"localip\", %d) failed", i);
			return -1;	
		}
		snprintf(oConf.m_sLocIP, sizeof(oConf.m_sLocIP), "%s", sValue);
		if (!m_poRead->getValue(oExp, sValue, "localport", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"localport\", %d) failed", i);
			return -1;	
		} 
		oConf.m_iLocPort = atoi(sValue);
		if (!m_poRead->getValue(oExp, sValue, "remoteip", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"remoteip\", %d) failed", i);
			return -1;	
		} 
		snprintf(oConf.m_sRemIP, sizeof(oConf.m_sRemIP), "%s", sValue);  
		if (!m_poRead->getValue(oExp, sValue, "remoteport", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"remoteport\", %d) failed", i);
			return -1;	
		}
		oConf.m_iRemPort = atoi(sValue);   
		if (!m_poRead->getValue(oExp, sValue, "rcvmq", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"rcvmq\", %d) failed", i);
			return -1;	
		}
		oConf.m_iRcvMQ = atoi(sValue);
		if (!m_poRead->getValue(oExp, sValue, "sndmq", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"sndmq\", %d) failed", i);
			return -1;	
		}
		oConf.m_iSndMQ = atoi(sValue);
		if (!m_poRead->getValue(oExp, sValue, "linkid", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"linkid\", %d) failed", i);
			return -1;	
		} 
		oConf.m_iLinkID = atoi(sValue);
		if (!m_poRead->getValue(oExp, sValue, "option", i)) {
            __DEBUG_LOG1_(0, "ReadCfg::getValue(\"option\", %d) failed", i);
			return -1;	
		} 
		oConf.m_iOpt = atoi(sValue);
		vConf.push_back(oConf);
		iRet = 0;   
    }    
    return iRet;    
}        