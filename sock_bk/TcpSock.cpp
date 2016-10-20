#include "TcpSock.h"
#include "LogV2.h"
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socketvar.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef DEF_HP
typedef int SOCKLEN_TYPE;
#else
typedef socklen_t SOCKLEN_TYPE;
#endif

#define SOCKET_BIND_MAX   128
#define SOCKET_LISTEN_MAX 128
#define SOCKET_BUFFER_MIN 102400

TcpSock::TcpSock()
{
    m_iSockID = -1;
}

TcpSock::~TcpSock()
{

};

int TcpSock::select(int iSock, int iFlag, int iSec)
{
    int iRet;
    struct timeval oTm;
    oTm.tv_sec = iSec;
	oTm.tv_usec = 0;

	fd_set oSet;
	memset((void *)&oSet, 0x00, sizeof(oSet));
	FD_SET(iSock, &oSet);
	if (iFlag == SELECT_RD_FLAG)   //是否有可读的文件
	    iRet = ::select(iSock+1, &oSet, NULL, NULL, &oTm);
	else  //是否有可写的文件
	    iRet = ::select(iSock+1, NULL, &oSet, NULL, &oTm);

	if (iRet < 0) {
	    if (errno == EINTR)
	        return 0;
	    else
	        return -1;
	}
	return iRet;
}

int TcpSock::sendn(int iSock, const void *pBuff, int iSize)
{
    if ((!pBuff) || (iSize<=0))
        return ECODE_INVALID;
    char *pStart = (char *)pBuff;
    int iSend, iLeft = iSize;
    do {
        if ((iSend=::send(iSock, pStart, iLeft, 0)) == iLeft)
            break;
        else if (iSend == -1) {
            if (errno != EINTR) {
                __DEBUG_LOG3_(0, "TcpSock::sendn iSock=%d send msg size= %d failed! errno=%d", iSock, iSize, errno);
                return ECODE_SYSCALL;
            }
        }
        else {
            iLeft -= iSend;
            pStart += iSend;
        }
    } while (iLeft > 0);

    return 0;
}

int TcpSock::recvn(int iSock, void *pBuff, int iSize, int iSec)
{
    if ((!pBuff) || (iSize<=0))
        return ECODE_INVALID;
    char *pStart = (char *)pBuff;
    int iRet, iLeft = iSize;

    if (iSec > 0) {
        if ((iRet=this->select(iSock, SELECT_RD_FLAG, iSec)) < 0) {
            __DEBUG_LOG3_(0, "TcpSock::recvn iSock=%d select failed! iSec=%d, errno=%d", iSock, iSec, errno);
            return ECODE_SYSCALL;
        }
        else if (iRet == 0)
            return ECODE_WOULDBLOCK;
    }

    do {
        if ((iRet=::recv(iSock, pStart, iLeft, 0)) == iLeft)
            break;
        else if (iRet <= 0) {   //非阻塞情况 不做考虑
            if (errno == EINTR)
                continue;
            else {
                __DEBUG_LOG1_(0, "TcpSock::recvn iSock=%d 检测到对方关闭!", iSock);
                return ECODE_BADSOCKET;
            }
        }
        else {
            iLeft -= iRet;
            pStart += iRet;
        }

    } while (iLeft > 0);

    return 0;
}

int TcpSock::close()
{
    while (::close(m_iSockID) == -1) {
		if (errno == EINTR)
	    	continue;
	    else if (errno == EBADF)
	    	break;
	    else {
	        __DEBUG_LOG2_(0, "TcpSock::close m_iSockID=%d close failed! errno=%d", m_iSockID, errno);
	        return -1;
	    }
	}
	return 0;
}

ListenSock::ListenSock()
{

}

ListenSock::~ListenSock()
{

}

int ListenSock::listen(char *pIP, int iPort)
{
    if ((!pIP) || (iPort<=0) || (iPort>65536))
        return -1;
    int iOpt = 1;
	int iBindTm = 0;
	struct sockaddr_in oSrvAddr;
	memset(&oSrvAddr, 0x00, sizeof(oSrvAddr));
	oSrvAddr.sin_family = AF_INET;
    oSrvAddr.sin_addr.s_addr = inet_addr(pIP);
    oSrvAddr.sin_port = htons(iPort);

    if ((m_iSockID=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		__DEBUG_LOG1_(0, "ListenSock::listen socket malloc failed! errno =%d", errno);
		return -1;
	}

	//端口重用
	if (setsockopt(m_iSockID, SOL_SOCKET, SO_REUSEADDR, (char *)&iOpt, (SOCKLEN_TYPE)sizeof(iOpt)) == -1) {
		__DEBUG_LOG1_(0, "ListenSock::listen setsockopt SO_REUSEADDR failed! errno =%d", errno);
		this->close();
		return -1;
	}
    //禁止TCP层打包 适合同步访问模式
	if (setsockopt(m_iSockID, IPPROTO_TCP, TCP_NODELAY, (char *)&iOpt, (SOCKLEN_TYPE)sizeof(iOpt)) == -1) {
            __DEBUG_LOG1_(0, "ListenSock::listen setsockopt TCP_NODELAY failed! errno =%d", errno);
            this->close();
			return -1;
	}

	while (bind(m_iSockID, (struct sockaddr*)&oSrvAddr, sizeof(oSrvAddr)) < 0) {
		if (errno == EADDRINUSE) {
			if (++iBindTm >= SOCKET_BIND_MAX) {
				this->close();
				return -1;
			}
			sleep(1);
			continue;
		}
		__DEBUG_LOG1_(0, "ListenSock::listen bind failed! errno =%d", errno);
		this->close();
        return -1;
    }

	if (::listen(m_iSockID, SOCKET_LISTEN_MAX) < 0) {
		__DEBUG_LOG1_(0, "ListenSock::listen listen failed! errno =%d", errno);
		this->close();
		return -1;
	}

	return 0;
}

int ListenSock::accept(AcceptSock &aptSock)
{
	sockaddr_in oRemAddr;
	memset(&oRemAddr, 0x00, sizeof(oRemAddr));
	SOCKLEN_TYPE len = sizeof(struct sockaddr_in);

    __Loop:
    #ifdef DEF_AIX
	aptSock.m_iSockID = ::accept(m_iSockID, (struct sockaddr*)&oRemAddr, (SOCKLEN_TYPE *)&len);
	#else
	aptSock.m_iSockID = ::accept(m_iSockID, (struct sockaddr*)&oRemAddr, (int *)&len);
	#endif

    if (aptSock.m_iSockID == -1) {
    	__DEBUG_LOG2_(0, "ListenSock::accept m_iSockID=%d accept failed! errno =%d", m_iSockID, errno);
    	switch (errno) {
    		case EINTR:
    		case ECONNRESET:
    		case EMFILE:
    		case ENFILE:
    		case EWOULDBLOCK:
    			goto __Loop;
    		default:
    			this->close();
    			return -1;
    	}
    }
    //增加强制关闭选项
    /*linger oLger = {1, 0};
    if (setsockopt(aptSock.m_iSockID, SOL_SOCKET, SO_LINGER, (const void *)&oLger, sizeof(linger)) != 0) {
        __DEBUG_LOG2_(0, "ListenSock::accept aptSock.m_iSockID=%d setsockopt \
                            SQL_SOCKET SO_LINGER failed! errno=%d", aptSock.m_iSockID, errno);
    }*/
    strncpy(aptSock.m_sRemIP, inet_ntoa(oRemAddr.sin_addr), sizeof(aptSock.m_sRemIP)-1);
    aptSock.m_iRemPort = ntohs(oRemAddr.sin_port);
    return 0;
}

//
AcceptSock::AcceptSock()
{
    memset(m_sRemIP, 0x00, sizeof(m_sRemIP));
    m_iRemPort = -1;
}

AcceptSock::~AcceptSock()
{
    
}

int AcceptSock::sendDCC(const void *pBuff, int iSize)
{
    if (sendn(m_iSockID, pBuff, iSize) != 0)
        return -1;
    else
        return iSize;            
}

int AcceptSock::recvDCC(void *pBuff, int iMaxSize)
{
    char *pStart = (char *)pBuff;
    if (recvn(m_iSockID, pBuff, 4) != 0)
        return -1;
    int iLen = ntohl(*(unsigned int *)pStart) & 0x00ffffff;
    if ((iLen>iMaxSize) ||(iLen<=4)) {
        __DEBUG_LOG2_(0, "AcceptSock::recvDCC m_iSockID=%d 收到非法DCC包 iSize=%d", m_iSockID, iLen);
        return -1;
    }    
    //if (recvn(m_iSockID, pBuff, iLen-4) != 0)
    if (recvn(m_iSockID, (char *)pBuff+4, iLen-4) != 0)
        return -1;
    return iLen;               
} 

//
ConnSock::ConnSock()
{
    memset(m_sRemIP, 0x00, sizeof(m_sRemIP));
    memset(m_sLocIP, 0x00, sizeof(m_sLocIP));
    m_iRemPort = -1;
    m_iLocPort = -1;    
}

ConnSock::~ConnSock()
{
    
}

int ConnSock::connect(char *pLocIP, int iLocPort, char *pRemIP, int iRemPort)
{
    if ((!pLocIP) || (!pRemIP) || (iLocPort>65536) || (iRemPort>65536))
        return -1;
    snprintf(m_sLocIP, sizeof(m_sLocIP), "%s", pLocIP);
    snprintf(m_sRemIP, sizeof(m_sRemIP), "%s", pRemIP);
    m_iLocPort = iLocPort;
    m_iRemPort = iRemPort;
    return reconnect();        
}                  

int ConnSock::reconnect()
{
    int iOpt = 1;
	int iCt = 0;
	int iValue, iLen;
	struct sockaddr_in oLocAddr;	
	struct sockaddr_in oSvrAddr;
	
	memset(&oLocAddr, 0x00, sizeof(oLocAddr));
    oLocAddr.sin_family = AF_INET;
    if (m_sLocIP[0] == '\0') 
    	oLocAddr.sin_addr.s_addr = inet_addr(INADDR_ANY);
    else 
    	oLocAddr.sin_addr.s_addr = inet_addr(m_sLocIP);  
	oLocAddr.sin_port = htons(m_iLocPort);
	
	memset(&oSvrAddr, 0x00, sizeof(oSvrAddr));
	oSvrAddr.sin_family = AF_INET;
    oSvrAddr.sin_addr.s_addr = inet_addr(m_sRemIP);
    oSvrAddr.sin_port = htons(m_iRemPort);
	
	while (true) {
		if ((m_iSockID=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			__DEBUG_LOG1_(0, "ConnSock::reconnect socket syscall failed! errno=%d", errno);
			return -1;
		}			
		
		if (setsockopt(m_iSockID, SOL_SOCKET, SO_REUSEADDR, (char *)&iOpt, (SOCKLEN_TYPE)sizeof(iOpt)) == -1) {
			__DEBUG_LOG1_(0, "ConnSock::reconnect setsockopt SO_REUSEADDR failed! errno=%d", errno);
			this->close();
			return -1;	                          	
		}
		iLen = sizeof(int);	
		if (getsockopt(m_iSockID, SOL_SOCKET, SO_SNDBUF, (void *)&iValue, (SOCKLEN_TYPE*)&iLen) == -1) {
		    __DEBUG_LOG1_(0, "ConnSock::reconnect getsockopt SQL_SOCKET SO_SNDBUF failed! errno=%d", errno);
            this->close();
            return -1;
        }
        if (iValue < SOCKET_BUFFER_MIN) {
            iValue = SOCKET_BUFFER_MIN;
            setsockopt(m_iSockID, SOL_SOCKET, SO_SNDBUF, (void *)&iValue, (SOCKLEN_TYPE)sizeof(iOpt));
        }    
        
        if (getsockopt(m_iSockID, SOL_SOCKET, SO_RCVBUF, (void *)&iValue, (SOCKLEN_TYPE*)&iLen) == -1) {
		    __DEBUG_LOG1_(0, "ConnSock::reconnect getsockopt SQL_SOCKET SO_RCVBUF failed! errno=%d", errno);
            this->close();
            return -1;
        }
        if (iValue < SOCKET_BUFFER_MIN) {
            iValue = SOCKET_BUFFER_MIN;
            setsockopt(m_iSockID, SOL_SOCKET, SO_RCVBUF, (void *)&iValue, (SOCKLEN_TYPE)sizeof(iOpt));
        }
            
		while (::bind(m_iSockID, (struct sockaddr*)&oLocAddr, sizeof(oLocAddr)) < 0) {
			if (errno == EADDRINUSE) {
				if (++iCt >= SOCKET_BIND_MAX) {
					__DEBUG_LOG0_(0,"SockClnt::reconnect Port has been used!");
					this->close();
					return -1;	
				}
				sleep(1);
				continue;	
			}
	    	__DEBUG_LOG1_(0, "SockClnt::reconnect bind syscall failed! errno=%d", errno);
	    	this->close();
            return -1;
        }
	    
	    if (::connect(m_iSockID, (struct sockaddr*)&oSvrAddr, sizeof(oSvrAddr)) < 0) {
	    	__DEBUG_LOG1_(0, "SockClnt::reconnect connect syscall failed! errno=%d", errno);
	    	sleep(1);
	    	this->close();
	    	continue;
	    }	
  
		break;
	}
	
	__DEBUG_LOG5_(0, "localip=%s,localport=%d connect remoteip=%s,remoteport=%d success! m_iSockID=%d", \
		m_sLocIP, m_iLocPort, m_sRemIP, m_iRemPort, m_iSockID);
		
	if (setsockopt(m_iSockID, IPPROTO_TCP, TCP_NODELAY, (char *)&iOpt, (SOCKLEN_TYPE)sizeof(iOpt)) == -1) {
        __DEBUG_LOG1_(0, "SockClnt::reconnect setsockopt syscall failed! errno=%d", errno);
        this->close();
		return -1;
    }	

	return 0;        
}    

int ConnSock::sendDCC(const void *pBuff, int iSize)
{
    while (sendn(m_iSockID, pBuff, iSize)) {
        __DEBUG_LOG2_(0 ,"ConnSock::sendDCC m_iSockID=%d sendDCC msg length=%d failed! sleep 3 repeat", m_iSockID, iSize);
        sleep(3);
        continue;    
    }
    return iSize;         
}

int ConnSock::sendCer(const void *pBuff, int iSize)
{
    if (sendn(m_iSockID, pBuff, iSize) == 0)
        return iSize;
    else
        return -1;
}

int ConnSock::recvDCC(void *pBuff, int iMaxSize)
{
    char *pStart = (char *)pBuff;
    int iLen;
    
    while (true) {
    
        if (recvn(m_iSockID, pBuff, 4) != 0) {
            __DEBUG_LOG1_(0, "ConnSock::recvDCC m_iSockID=%d 收到非法DCC包头", m_iSockID);
            break;       
        }
        iLen = ntohl(*(unsigned int *)pStart) & 0x00ffffff;
        if ((iLen>iMaxSize) ||(iLen<=4)) {
            __DEBUG_LOG2_(0, "ConnSock::recvDCC m_iSockID=%d 收到非法DCC包 iSize=%d", m_iSockID, iLen);
            break;
            
        }    
        if (recvn(m_iSockID, (char *)pBuff+4, iLen-4) != 0) {
            __DEBUG_LOG2_(0, "ConnSock::recvDCC m_iSockID=%d 接收DCC包体失败 iSize=%d", m_iSockID, iLen);
            break;
        }   
        
        return iLen;
    }        
    return -1;              
}    
    
