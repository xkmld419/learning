#ifndef __TCP_SOCK_H_INCLUDED_
#define __TCP_SOCK_H_INCLUDED_

//select 的iFlag参数 分别表示 可读 可写 异常
#define SELECT_RD_FLAG 0             
#define SELECT_WT_FLAG 1
#define SELECT_EP_FLAG 2

//recvn 返回的自定义错误号
#define ECODE_WOULDBLOCK  1    //无数据可读
#define ECODE_BADSOCKET   2    //不可用的SOCKET
#define ECODE_INVALID     3    //参数错误
#define ECODE_SYSCALL     4    //系统调用失败


//设计 仅支持 阻塞 如果支持非阻塞 需要增加判断
class TcpSock
{

public:
    
    TcpSock();
    
    virtual ~TcpSock();
    
    int getSock() {
        return m_iSockID;    
    }
    
    int close();
    
    int selectRD(int iSec) {
        return select(m_iSockID, SELECT_RD_FLAG, iSec);    
    }
    
protected:
    
    //成功返回0 失败返回错误号
    int sendn(int iSock, const void *pBuff, int iSize);
    
    //成功返回0 失败返回错误号
    int recvn(int iSock, void *pBuff, int iSize, int iSec=0);
    
    int select(int iSock, int iFlag, int iSec);

protected:
    
    int m_iSockID;           
};

class AcceptSock;

class ListenSock : public TcpSock
{
    
public:
    
    ListenSock();
    
    ~ListenSock();
    
    int listen(char *pIP, int iPort);
    
    int accept(AcceptSock &aptSock);    
            
};

class AcceptSock : public TcpSock
{

public:
    
    friend int ListenSock::accept(AcceptSock &aptSock);
        
    AcceptSock();
    
    ~AcceptSock();
    
    char *getRemIP() {
        return m_sRemIP;    
    }
    
    int getRemPort() {
        return m_iRemPort;    
    }
    
    int sendDCC(const void *pBuff, int iSize);
    
    int recvDCC(void *pBuff, int iMaxSize);
    
private:
        
    char m_sRemIP[16];
    
    int  m_iRemPort;             
};

class ConnSock : public TcpSock
{

public:
    
    ConnSock();
    
    ~ConnSock();
    
    int connect(char *pLocIP, int iLocPort, char *pRemIP, int iRemPort);
    
    int sendDCC(const void *pBuff, int iSize);

    int sendCer(const void *pBuff, int iSize);
    
    int recvDCC(void *pBuff, int iMaxSize);
    
    int rebuiltSock() {
        this->close();
        return reconnect();    
    }
    
private:
    
    int reconnect();
    
public:
        
    char m_sLocIP[16];
    
    char m_sRemIP[16];
    
    int m_iLocPort;
    
    int m_iRemPort;        
};    

#endif/*__TCP_SOCK_H_INCLUDED_*/
