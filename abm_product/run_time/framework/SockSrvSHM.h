/*
*	共享内存中存放 -P ProcID SndMQ, RcvMQ, SndThread RcvThread SockStus 
*		local ip, loalport , remote ip ,remote port
*
*/

#ifndef __SOCK_SRV_SHM_H_INCLUDED_
#define __SOCK_SRV_SHM_H_INCLUDED_

#include <stdio.h>
#include <string.h>

#define SYS_NODE_MAX 128 //系统支持的最大节点数
typedef unsigned long ul32;

struct SockLinkStruct
{
	ul32 m_ulLinkID;
    ul32 m_ulProcID;
	char m_sLocIp[16];
	char m_sRemIp[16];
	ul32 m_ulLocPort;
	ul32 m_ulRemPort;
	ul32 m_ulSndThread;
	ul32 m_ulRcvThread;
	ul32 m_ulSndMQ;
	ul32 m_ulRcvMQ;
	ul32 m_ulSockStus;
	ul32 m_ulThreadStus;

    void setProc(const ul32 ulProc) {
        m_ulProcID = ulProc;
    }

	void setLink(const ul32 ulLink) {
		m_ulLinkID = ulLink;	
	}

	void setLocalIp(const char *ip) {
	
		snprintf(m_sLocIp, sizeof(m_sLocIp), "%s", ip);
	}

	void setLocalPort(const ul32 port) {
		m_ulLocPort = port;
	}

	void setRemoteIp(const char *ip) {
		snprintf(m_sRemIp, sizeof(m_sRemIp), "%s", ip);
	}

	void setRemotePort(const ul32 port) {
		m_ulRemPort = port;
	} 

	void setSndThread(const ul32 ulthread) {
		m_ulSndThread = ulthread;	
	}
	
	void setRcvThread(const ul32 ulthread) {
		m_ulRcvThread = ulthread;		
	}

	void setSndMQ(const ul32 mq) {
		m_ulSndMQ = mq;	
	}

	void setRcvMQ(const ul32 mq) {
		m_ulRcvMQ = mq;	
	}
	
	void setSockStus(const ul32 status) {
		m_ulSockStus = status;	
	}

	void setThreadStus(const ul32 status) {
		m_ulThreadStus = status;	
	}

};


struct SockAppStruct
{
	SockLinkStruct m_oLink[SYS_NODE_MAX];   //目前内存中展示的最大连接数

	void clear() {
		memset(m_oLink, 0, sizeof(m_oLink));
	}

	int size() {
		return SYS_NODE_MAX;	
	}

	SockLinkStruct* getLink(const ul32 ulLink) {
        if (ulLink >= size()) 
            return NULL;
        return &m_oLink[ulLink];
	}
};

#endif /*__SOCK_SRV_SHM_H_INCLUDED_*/

