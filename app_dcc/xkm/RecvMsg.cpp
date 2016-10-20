#include "abmobject.h"
#include "abmcmd.h"
#include "ABMException.h"
#include "MessageQueue.h"
#include "../dccpublic.h"
#include <stdio.h>
#include <iostream>
using namespace std;

int main(int argc, char**argv)
{
    ABMException oExp;
    long lKey;
    int iRecvLen;
    StruMqMsg oRcvBuf;
    int i,x=1;
    cin >> i;
    
    if (argc < 2) {
        cout<<"usage:"<<argv[0]<<" <mq_keyid>"<<endl;
        return -1;
    }
    
    lKey = atol(argv[1]);
    MessageQueue oMq(lKey);
    oMq.open(oExp, true,true);
    
    for(x;x<i;x++){
    if ((iRecvLen = oMq.Receive((void *)&oRcvBuf, sizeof(oRcvBuf), 0, true)) <0) {
		cout<<"应用程序从消息队列接收失败, 程序退出! errno="<<errno<<endl;
		return -1;
	}
	
	disp_buf((char *)&oRcvBuf, iRecvLen);
    }
	return 0;
}
	
	

