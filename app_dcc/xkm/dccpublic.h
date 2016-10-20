#ifndef _DCC_PUBLIC_H_
#define _DCC_PUBLIC_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <dcc_ra.h>
//#include "abmsendcmd.h"
#include "DccMsgParser.h"
#include "abmobject.h"
#include "abmcmd.h"
#include "errorcode.h"
#include "LogV2.h"

#define SECS_70_YEARS 2207520000   //=70*365*24*60*60,70年的秒数，没考虑366填的年份
#define MSG_SIZE 4096

//从socket接收和发送消息的统一数据结构
typedef struct StruMqMsg {
    long m_lMsgType;
    char m_sMsg[MSG_SIZE];
}StruMqMsg;


//以16进制显示缓存区内容
void disp_buf(char *buf, int size);


//根据service-context-id的第一个域判断业务类型，返回业务员类型
int switchCmd(string &sCmd);


//获取dcc请求包的公共信息，存入结构体，成功返回请求消息的业务类型（正数）
int getCCRHead(DccMsgParser *poDcc, ABMCCR *pCCR);


//打包dcc应答包的公共信息，部分信息取自公共头部结构体，成功返回0，失败-1
int setCCAHead(DccMsgParser *poDcc, ABMCCA *pCCA);


//资料查询并开户
int parserInfoQryR(DccMsgParser *poDcc, ABMCCR *oSendBuf);
int packInfoQryA(DccMsgParser *poDcc, ABMCCA *oSendBuf);


//3.4.2.1支付
int parserChargeR(DccMsgParser *poDcc, ABMCCR *poCCR);
int packChargeA(DccMsgParser *poDcc, ABMCCA *poCCA);

//3.4.5.1
int AllocateBalanceSvcR(DccMsgParser *poDcc, ABMCCR *poCCR);
int AllocateBalanceSvcA(DccMsgParser *poDcc, ABMCCA *poCCA);

int AllocateBalanceSvcR(DccMsgParser *poDcc, ABMCCR *poCCR);
int AllocateBalanceSvcA(DccMsgParser *poDcc, ABMCCA *poCCA);
//3.4.6.16
int RechargeBillQueryR(DccMsgParser *poDcc, ABMCCR *poCCR);
int RechargeBillQueryA(DccMsgParser *poDcc, ABMCCA *poCCA);

//3.4.6.15
int QueryPayLogByAccoutR(DccMsgParser *poDcc, ABMCCR *poCCR);
int QueryPayLogByAccoutA(DccMsgParser *poDcc, ABMCCA *poCCA);

//3.4.6.14
int RechargeRecQueryR(DccMsgParser *poDcc, ABMCCR *poCCR);
int RechargeRecQueryA(DccMsgParser *poDcc, ABMCCA *poCCA);

//3.4.5.5
int ReqReverseDeductSvcR(DccMsgParser *poDcc, ABMCCR *poCCR);
int ReqReverseDeductSvcA(DccMsgParser *poDcc, ABMCCA *poCCA);



#endif