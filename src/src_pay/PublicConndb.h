#ifndef __PUBLIC_CONNDB_H_INCLUDED_
#define __PUBLIC_CONNDB_H_INCLUDED_
#include "ABMException.h"
#include "TTTns.h"
#include "abmpaycmd.h"
#include "abmcmd.h"
#include "LogV2.h"
#include <time.h>
#include <iostream>
using namespace std;

extern TimesTenCMD *m_pBalanceTransReset;//更新划拨规则
extern TimesTenCMD *m_pPasswordResetUpdate;//支付密码修改
extern TimesTenCMD *m_pPasswordResetDelete;//支付密码修改
extern TimesTenCMD *m_pPasswordResetInsert;//支付密码修改
extern TimesTenCMD *m_pPayStatusResetUpdate;//支付密码修改
extern TimesTenCMD *m_pPayRulesReset;//支付规则修改
extern TimesTenCMD *m_pTransferRulesInsert;//划拨支付规则新增

extern TimesTenCMD *m_poPublicConn;//查询
extern TimesTenCMD *m_pInsertPayout;//支出
extern TimesTenCMD *m_pInsertPayment;//记录业务操作
extern TimesTenCMD *m_pInsertRela;//记录支出来源关系
extern TimesTenCMD *m_pUpdateBalance;//更新账本
extern TimesTenCMD *m_pUpdateSource;//更新来源
extern TimesTenCMD *m_pInsertServInfo;//serv
extern TimesTenCMD *m_pInsertAcctInfo;//acct
extern TimesTenCMD *m_pInsertServAcctInfo;//serv_acct
extern TimesTenCMD *m_pInsertAcctBalance;//acct_balance
extern TimesTenCMD *m_pInsertBalanceSource;//balance_source

extern TimesTenCMD *m_pUpdatePayoutState;//更新支出记录状态
extern TimesTenCMD *m_pUpdateRealState;// 更新支出来源关系状态
extern TimesTenCMD *m_pInsertAttUserInfo;// att_user_info
extern TimesTenCMD *m_pInsertAppUserInfo;// app_user_info


extern TimesTenCMD *m_pInsertServHis;//记录SERV历史信息表
extern TimesTenCMD *m_pInsertAcctHis;//记录ACCT历史信息表
extern TimesTenCMD *m_pInsertServAcctHis;//记录SERV_ACCT历史信息表
extern TimesTenCMD *m_pDeleteAcctBalance;//删除余额账单余额为0数据
extern TimesTenCMD *m_pDeleteServInfo;//删除SERV表信息
extern TimesTenCMD *m_pDeleteAcctInfo;//删除ACCT表信息
extern TimesTenCMD *m_pDeleteServAcctInfo;//删除SERV_ACCT表信息
extern TimesTenCMD *m_pChangeInservHis;//换号用记录SERV历史信息表
extern TimesTenCMD *m_pChangeNbr;//换号
extern TimesTenCMD *m_pChangeAcct;//过户创建ACCT_ID
extern TimesTenCMD *m_pUpdateServAcct;//过户更改SERV_ACCT

extern TimesTenCMD *m_pUpdateServAddState;	//用户信息增量表同步状态更新
extern TimesTenCMD *m_pInsertSendSql;	//写短信工单表

extern TimesTenCMD *m_pLoginSaveDcc;	//登陆密码鉴权
extern TimesTenCMD *m_pPasswdChangeDcc; //登陆密码修改

extern TimesTenCMD *m_pGetSeq;//取循环序列
extern TimesTenCMD *m_pInserUserAddErr;	//资料同步增量表记录错误码

extern TimesTenCMD *m_pSaveActiveCCR; // 插入CCR请求流水
extern TimesTenCMD *m_pUpdateCCR;// 更新CCR请求流水记录状态

class ABMException;
#define FreeObject(x) \
	if (x){ \
		delete x ;x=NULL; \
	}
#define FreeVector(X) {\
		for (unsigned int u=0; u<X.size();u++) { \
			delete X[u]; X[u] = NULL; \
		} \
		X.clear(); \
	}
inline char *ltoa(const long lValue)
{
	static char sTemp[32];
	sprintf(sTemp,"%ld",lValue);
	return (char*)sTemp;
};
inline int GetDate(char *date)
{
	time_t   Time;
	struct tm *T;
	time(&Time);
	T=localtime(&Time);
	sprintf(date, "%04d%02d%02d",T->tm_year+1900,T->tm_mon+1,T->tm_mday);
	return 0;
};
inline int GetTime(char *date)
{
	time_t   Time;
	struct tm *T;
	time(&Time);
	T=localtime(&Time);
	sprintf(date, "%04d%02d%02d",T->tm_year+1900,T->tm_mon+1,T->tm_mday,T->tm_hour, T->tm_min, T->tm_sec);
	return 0;
};
//将时间转换为秒数
//返回值加上1900~1970的秒数+8小时时差
inline unsigned int date2long( long  x){
    int b ;
    int y;
    struct tm tmtime;  
    b =x/100000000;
    b= b/100;
    tmtime.tm_year=b-1900;
    b= ((x/1000)/100000)%100;
    tmtime.tm_mon=b-1;
    b= ((x/1000)/1000)%100;
    tmtime.tm_mday=b;
    b= ((x/1000)/10)%100;
    tmtime.tm_hour=b;
    b= (x/100)%100;
    tmtime.tm_min=b;
    b= x%100;
    tmtime.tm_sec=b;
    y=mktime(&tmtime);
    cout<<endl;
    return y+2209017599; 
}
//将秒数转换为字符串类型时间
//目前入参为从1900开始的秒数，要先减去从1900~1970的秒数再减去8小时时差再计算
inline int long2date(unsigned int U32Var,char *str)
{
   U32Var -=2209017599;
   time_t t;
   time_t nowtime;
   long x;    
   struct tm *ptm;
   nowtime = (time_t)U32Var;
   ptm = localtime(&nowtime);
   
   sprintf(str, "%04d%02d%02d%02d%02d%02d", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec); 
   return 0;
};

class PublicConndb : public TTTns
{

public:

	PublicConndb();

	~PublicConndb();

	//初始化数据库的链接
	int init(ABMException &oExp);
	//提交数据
	int Commit(ABMException &oExp);
	//回滚数据
	int RollBack(ABMException &oExp);
};

#endif/*__PUBLIC_CONNDB_H_INCLUDED_*/

