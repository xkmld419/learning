#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/msg.h>

#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdio>
using namespace std;

#include "Process.h"
#include "Log.h"
#include "Socket.h"
#include "StdEvent.h"
#include "EventSender.h"
#include "EventReceiver.h"
#include "Date.h"

#include "msg.h"
#include "OcpMsgParser.h"

EventSender *g_poSender;            // 消息队列发送
EventReceiver *g_poReceiver;        // 消息队列接收
#define PORT   3868

Socket g_Socket;


typedef basic_string<char>::size_type S_T;
static const S_T npos = -1;

////使用一个完整的串delimit（而不是其中的某个字符）来分割src串,没有trim选项，即严格分割。
vector<string> split(const string& src, string delimit, 
			string null_subst="")
{
	if( src.empty() || delimit.empty() ) throw "split: empty string\0";

	vector<string> v;
	S_T deli_len = delimit.size();
	long index = npos, last_search_position = 0;
	while( (index=src.find(delimit, last_search_position))!=npos )
	{
		if(index==last_search_position)
			v.push_back(null_subst);
		else
			v.push_back( src.substr(last_search_position, index-last_search_position) );
		last_search_position = index + deli_len;
	}
	string last_one = src.substr(last_search_position);
	v.push_back( last_one.empty()? null_subst:last_one );
	return v;
}

/*
 *int getMsgFromFile(vector<string> &vecMsg)
 *{
 *    //vector<string> vecMsg;
 *    vector<string> strVecSplit;
 *    char sBuf[3000] = {0};
 *    char sMsg[1000] = {0};
 *    int nSplitNum;
 *    int i = 0;
 *
 *    ifstream ifs("msg.txt");
 *    while(ifs.getline(sBuf, 3000))
 *    {
 *        sBuf[strlen(sBuf)] = '\0';
 *        strVecSplit = split(string(sBuf, 3000), " ");
 *        nSplitNum = strVecSplit.size();
 *        for (i=0; i<nSplitNum; ++i)
 *            sscanf(strVecSplit[i].c_str(), "%x", &sMsg[i]);
 *        vecMsg.push_back(string(sMsg, nSplitNum));
 *        memset(sBuf, 0, 3000);
 *    }
 *    ifs.close();
 *    return vecMsg.size();
 *}
 */
int getMsgFromFile()
{
	//vector<string> vecMsg;
	vector<string> strVecSplit;
	char sBuf[3000] = {0};
	int nSplitNum;
	int i = 0;
	int nAVPNum = 0;

	ifstream ifs("msg.txt");
	while(ifs.getline(sBuf, 3000))
	{
		nAVPNum++;
		sBuf[strlen(sBuf)] = '\0';
		strVecSplit = split(string(sBuf, 3000), " ");
		nSplitNum = strVecSplit.size();
		for (i=0; i<nSplitNum; ++i)
			sscanf(strVecSplit[i].c_str(), "%x", &g_ssAVPBuf[nAVPNum-1][i]);
		memset(sBuf, 0, 3000);
	}
	ifs.close();
	return nAVPNum;
}


void recvDccMsg()
{
	char *recvBuf = new char[sizeof(StdEvent)*5];
	int ret, msg_len, recv_len;
	char *data = recvBuf;
	while(1)
	{
		if (getppid() == 1)    // 父进程已退出，子进程退出
		{
			Log::log(0, "父进程已退出，子进程退出");
			return ;
		}

		msg_len = 20;
		recv_len = 0;
		memset(recvBuf, 0, sizeof(StdEvent)*5);
		// 读取Dcc头部20个字节
		for( ; recv_len < msg_len  ;  )
		{
			if( (ret = g_Socket.read((unsigned char *)(data+recv_len), msg_len - recv_len)) < 0 )
			{
				Log::log(0, "receive dcc head from socket error!");
				return ;
			}
			recv_len += ret;
		}
		unsigned char buf[4] = {0};
		buf[1] = data[1];
		buf[2] = data[2];
		buf[3] = data[3];

		msg_len = buf[1]*65536+buf[2]*256+buf[3];//此为网络序转为字节序修改灵活一些
		if (msg_len >= 99999)
		{
			Log::log(0, "DCC message is too length!");
		}

		// 读取DCC消息体
		for( ; recv_len < msg_len  ;  )
		{
			if( (ret = g_Socket.read((unsigned char *)(data+recv_len), msg_len - recv_len)) < 0 )
			{
				Log::log(0, "receive dcc body from socket err!");
			}
			recv_len += ret;
		}
		g_poSender->send(data);
		AddTicketStat(TICKET_NORMAL);
		Log::log(0, "-------------------------------------------Receive Message");
	}
	delete []recvBuf;
}

void sendDccMsg()
{
	char *sRecvBuf = new char[sizeof(StdEvent)*5];
	char *sTempBuf = new char[sizeof(StdEvent)*5];
	char *sCCAArgs = new char[sizeof(StdEvent)*5];
	char *sSendBuf = NULL;
	int  msgType;
	int requestType;
	int  ret;
	bool bCeaOK = false;
	bool bActiveOK = false;
	int  nCcrFlag = 1;
	int  nCcrSeq = 4;
	int  nMsgNum;
	Date dtSendR;
	Date dtTemp;

	/*
	 *   读文件中的消息
	 *   0. CEA
	 *   1. 激活CCA
	 *   2. DWA
	 *   3. DWR
	 *   4. CCR
	 *   ...
	 *   n. CCR
	 */
	//nMsgNum = getMsgFromFile();
	nMsgNum = AVPNUM;
	int ilen;
	OcpMsgParser *pOcpMsgParser = new OcpMsgParser("dictionary.xml");
	pOcpMsgParser->initLocalInfo("HB@971.ChinaTelecom.com", "971.ChinaTelecom.com", "127.0.0.1");
	char sServiceContextId[256];                   // 解包后返回的Service-Context-Id


	while (1)
	{
		if (getppid() == 1)    // 若子进程退出，孙进程也退出
		{
			Log::log(0, "子进程已退出，孙进程退出");
			return ;
		}
		memset(sRecvBuf, 0, sizeof(StdEvent)*5);
		ret = g_poReceiver->receive(sRecvBuf, false);
		if (ret > 0)
		{
			Log::log(0, "received message from queque!");
			msgType = sRecvBuf[5]*65536+sRecvBuf[6]*256+sRecvBuf[7];
			requestType = sRecvBuf[4] >> 7;
			switch(msgType)
			{
			case 257:
				bCeaOK = true;
				// 赋CEA到sSendBuf
				Log::log(0, "send CEA-------------------------------------------");
				sSendBuf = g_ssAVPBuf[0];
				break;
			case 280:
				if (requestType)
				{
					// 赋DWA到sSendBuf
					Log::log(0, "send DWA-------------------------------------------");
					sSendBuf = g_ssAVPBuf[2];
					break;
				}
				else
				{
					sSendBuf = NULL;
					break;
				}
			case 282:
				Log::log(0, "-------------------------------------------received DPR");
				return;     // SR exit
				break;
			case 272:
				if (!bActiveOK)
				{
					bActiveOK = true;
					// 赋激活CCA到sSendBuf
					Log::log(0, "send active CCA-------------------------------------------");
					sSendBuf = g_ssAVPBuf[1];
				}
				else
				{
					memset(sTempBuf, 0, sizeof(StdEvent)*5);
					memset(sServiceContextId, 0, 256);
					pOcpMsgParser->parserPack(sRecvBuf, sizeof(StdEvent)*5,
								sTempBuf, sizeof(StdEvent)*5, sServiceContextId);

					memset(sCCAArgs, 0, sizeof(StdEvent)*5);
					strncpy(sCCAArgs, sTempBuf, strstr(sTempBuf, "^^^")-sTempBuf);
					char *ptr = strrchr(sCCAArgs, '|');
					*ptr = '\0';
					ptr = sTempBuf;
					while ((ptr=strstr(ptr, "^^^")) != NULL)
					{
						strcat(sCCAArgs, "^^^20001|0");
						ptr += 3;
					}

					// 赋CCA到sSendBuf
					pOcpMsgParser->createServCCA(sCCAArgs, sizeof(StdEvent)*5,
								sSendBuf, sizeof(StdEvent)*5);
					Log::log(0, "send CCA-------------------------------------------");
				}
				break;
			default:
				Log::log(0, "Unkown Dcc Message!");
			}
		}
		else
		{
			dtTemp.getCurDate();
			if (dtTemp.diffSec(dtSendR) < 30)
				continue;
			if (!bCeaOK)
			{
				Log::log(0, "receive message queque failed!\ncontinue");
                usleep(5000);
				continue;
			}
			if (bActiveOK && (nCcrFlag%3 == 0))
			{
				Log::log(0, "Send CCR-------------------------------------------");
				// 赋CCR到sSendBuf
				sSendBuf = g_ssAVPBuf[nCcrSeq];
				if (++nCcrSeq >= nMsgNum)
					nCcrSeq = 4;
				nCcrFlag = 1;
			}
			else
			{
				Log::log(0, "Send DWR-------------------------------------------");
				// 赋DWR到sSendBuf
				sSendBuf = g_ssAVPBuf[3];
			}
			nCcrFlag++;
			dtSendR.getCurDate();
		}
		ilen = sSendBuf[1]*65536+sSendBuf[2]*256+sSendBuf[3];
		g_Socket.write((unsigned char *)sSendBuf, ilen);
	}
	delete []sRecvBuf;
	delete []sTempBuf;
	delete []sCCAArgs;
}

class SRSimulator : public Process
{
public:
	int run()
	{
		int pid = getpid();
		int pid1, pid2;

		g_poSender = new EventSender (m_iProcID);
		g_poReceiver = new EventReceiver (m_iProcID);

		while (1)
		{
			int ret = g_Socket.listen(PORT);

			if (ret < 0)
			{
				Log::log(0, "socket listen error!");
				return -1;
			}
			// 连接monitor
			ConnectTerminalMonitor();

			pid1 = fork();
			switch(pid1)
			{
			case -1:
				Log::log(0, "fork 子进程 error!");
				break;
			case 0:
				pid2 = fork();
				switch(pid2)
				{
				case -1:
					Log::log(0, "fork 孙进程 error!");
					return -1;
				case 0:
					sleep(2);
					sendDccMsg();
					g_Socket.closeSocket();
					return 0;
					break;
				default:
					recvDccMsg();
					g_Socket.closeSocket();
					return 0;
					break;
				}
				break;
			default:
				waitpid(pid1, NULL, 0);
				sleep(10);
		//		Environment::getDBConn(true);
				g_Socket.closeSocket();
				break;
			}
		}

		Log::log(0, "SR 退出");

		return 0;
	}
};

DEFINE_MAIN (SRSimulator)



