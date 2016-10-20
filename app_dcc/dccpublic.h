#ifndef _DCC_PUBLIC_H_
#define _DCC_PUBLIC_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include "DccMsgParser.h"
#include "abmobject.h"
#include "abmcmd.h"
#include "errorcode.h"
#include "LogV2.h"
#include "ReadDccConf.h"
#include "TimesTenAccess.h"
//#include "../src/src_test2/dcc_ra.h"
#include "DccSql.h"

#define SECS_70_YEARS 2208988800   //70年的秒数
//#define SECS_70_YEARS 2207520000   //=70*365*24*60*60,70年的秒数，没考虑366天的年份
#define MSG_SIZE 4096
#define _SEED_NUM_ 17

//从socket接收和发送消息的统一数据结构
typedef struct StruMqMsg {
    long m_lMsgType;
    char m_sMsg[MSG_SIZE];
}StruMqMsg;

//int getH2H();
//int getE2E();

//以16进制显示缓存区内容
void disp_buf(char *buf, int size);

unsigned int getE2E();
unsigned int getH2H();

//根据service-context-id(请求包)或service-context-id(应答包)
//的第一个域判断业务类型，返回业务员类型
int switchCmd(string &sCmd, int iReqFlag);


//获取dcc请求包的公共信息，存入结构体，成功返回请求消息的业务类型（正数）
//iReqFlag: 默认是1，表示接收到的消息是请求消息，由于全国中心abm系统具有
//          主动发送CCR的功能，因此收到的是对应的CCA时,iReqFlag赋0
int getDccHead(DccMsgParser *poDcc, ABMCCR *pCCR, int iReqFlag = 1, TimesTenCMD *poTTCmd = NULL);

//int getDccHead(DccMsgParser *poDcc, ABMCCR *pCCR, int iReqFlag = 1);
//打包dcc应答包的公共信息，部分信息取自公共头部结构体，成功返回0，失败-1
int setDccHead(DccMsgParser *poDcc, ABMCCA *pCCA, int iReqFlag,char *sessCCR,char *srealmCCR);

//几个非业务应答打包
int packCEA(DccMsgParser *poDcc, StruMqMsg *pSendMsg);
int packDWA(DccMsgParser *poDcc, StruMqMsg *pSendMsg);
int packDPA(DccMsgParser *poDcc, StruMqMsg *pSendMsg);
int packNetCtrl(DccMsgParser *poDcc, StruMqMsg *pSendMsg);

//3.4.1.1 资料查询并开户
int parserInfoQryR(DccMsgParser *poDcc, ABMCCR *oSendBuf);
int packInfoQryA(DccMsgParser *poDcc, ABMCCA *oSendBuf);


//3.4.2.1 支付
int parserChargeR(DccMsgParser *poDcc, ABMCCR *poCCR);
int packChargeA(DccMsgParser *poDcc, ABMCCA *poCCA);


//3.4.2.5 二次认证的密码下发
int parserRandPswdR(DccMsgParser *poDcc, ABMCCR *poCCR);
int packRandPaswA(DccMsgParser *poDcc, ABMCCA *poCCA);


//3.4.5.1
int AllocateBalanceSvcR(DccMsgParser *poDcc, ABMCCR *poCCR);
int AllocateBalanceSvcA(DccMsgParser *poDcc, ABMCCA *poCCA);

//jiancheng20110530
//3.4.5.1 --全国中心abm主动发出
int packDeductBalR(DccMsgParser *poDcc, ABMCCA *poCCR);
int parserDeductBalA(DccMsgParser *poDcc, ABMCCR *poCCA);

//3.4.5.5
int ReqReverseDeductSvcR(DccMsgParser *poDcc, ABMCCR *poCCR);
int ReqReverseDeductSvcA(DccMsgParser *poDcc, ABMCCA *poCCA);
//jiancheng20110530
//3.4.5.5--全国中心abm主动发出
int packRvsChargeR(DccMsgParser *poDcc, ABMCCA *poCCR);
int parserRvsChargeA(DccMsgParser *poDcc, ABMCCR *poCCA);


//3.4.6.16
int RechargeBillQueryR(DccMsgParser *poDcc, ABMCCR *poCCR);
int RechargeBillQueryA(DccMsgParser *poDcc, ABMCCA *poCCA);


//3.4.6.15
int QueryPayLogByAccoutR(DccMsgParser *poDcc, ABMCCR *poCCR);
int QueryPayLogByAccoutA(DccMsgParser *poDcc, ABMCCA *poCCA);


//3.4.6.14
int RechargeRecQueryR(DccMsgParser *poDcc, ABMCCR *poCCR);
int RechargeRecQueryA(DccMsgParser *poDcc, ABMCCA *poCCA);


//3.4.6.4 余额划拨查询 --- abm中心收到外围请求
int parserQryBalDrawR(DccMsgParser *poDcc, ABMCCR *poCCR);
int packQryBalDrawA(DccMsgParser *poDcc, ABMCCA *poCCA);
//3.4.6.4 余额划拨查询 --- abm中心主动发起
int parserQryBalDrawA(DccMsgParser *poDcc, ABMCCR *poCCA);
int packQryBalDrawR(DccMsgParser *poDcc, ABMCCA *poCCR);

//3.4.6.13,  3.4.5.3,  3.4.5.4 余额查询
int QueryBalanceR(DccMsgParser *poDcc, ABMCCR *poCCR);
int QueryBalanceA(DccMsgParser *poDcc, ABMCCA *poCCA);
//jiancheng20110530
//3.4.6.13,  3.4.5.3,  3.4.5.4 余额查询 --- abm中心主动发起
int packQryBalanceR(DccMsgParser *poDcc, ABMCCA *poCCR,char *serNUMCCR);
int parserQryBalanceA(DccMsgParser *poDcc, ABMCCR *poCCA);
/*****************************************************
 *
 *    3.4.4.1 开户充值 VC规范2.1  张成 
 *    开户充值CCR    
 *
 *****************************************************/
//解包
int DepositServCondOneR(DccMsgParser *poDcc, ABMCCR *poCCR);
int DepositServCondOneA(DccMsgParser *poDcc, ABMCCA *poCCA);
//3.4.4.3 开户充值 VC规范2.3 张成 
int ReverseDepositR(DccMsgParser *poDcc, ABMCCR *poSendBuf);
int ReverseDepositA(DccMsgParser *poDcc, ABMCCA *poSendBuf);

/****
3.4.6.5  余额划拨
****/

int ReqBalanceSvcA(DccMsgParser *poDcc, ABMCCR *poCCA);
int ReqBalanceSvcR(DccMsgParser *poDcc, ABMCCA *poCCR);

/*************
3.4.5.2 全国中心余额划出
************/
int ReqPlatformSvcR(DccMsgParser *poDcc, ABMCCR *poCCR);
int ReqPlatformSvcA(DccMsgParser *poDcc, ABMCCA *poCCA);

//3.4.2.4  支付冲正
int ReversePaySvcR(DccMsgParser *poDcc, ABMCCR *poSendBuf);
int ReversePaySvcA(DccMsgParser *poDcc, ABMCCA *poSendBuf);

//3.4.1.2 资料同步  HSS发CCR
int QryServSynCondR(DccMsgParser *poDcc, ABMCCR *poSendBuf);
int QryServSynCondA(DccMsgParser *poDcc, ABMCCA *poSendBuf);

//3.4.1.2 资料同步  ABM发CCR
int QryServSynCondRT(DccMsgParser *poDcc, ABMCCA *poCCR);
int QryServSynCondAT(DccMsgParser *poDcc, ABMCCR *poCCA);

//3.4.1.3 密码鉴权  ABM发HSS
int PasswdIdentifyCondRT(DccMsgParser *poDcc, ABMCCA *poCCR);
int PasswdIdentifyCondAT(DccMsgParser *poDcc, ABMCCR *poCCA);

//3.4.1.6 登陆鉴权  自服平台发给ABM
int LoginPasswdCCRR(DccMsgParser *poDcc, ABMCCR *poCCR);
int LoginPasswdCCRA(DccMsgParser *poDcc, ABMCCA *poCCA);

#endif