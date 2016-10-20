//ActiveSendReverseBizSql.h
#ifndef __ACTIVE_SEND_REVERSE_BIZ_SQL_H_INCLUDED_
#define __ACTIVE_SEND_REVERSE_BIZ_SQL_H_INCLUDED_

#include "TTTns.h"
#include "PublicConndb.h"
#include <vector>

#define REVERSE_DEDUCT_DELAY_SECOND	"1001";// 系统开关，控制主动发送划拨冲正延迟秒数

struct ActiveSendLog
{
	char sRequestId[32];
	char sRequestType[4];
	char sState[4];	
	char sOrgId[5];
};


struct SysSwitch
{
	char *sSwitchId[5];
	char *sValue[2];
	char *sDesc[101];	
};

class ABMException;

class ActiveSendReverseBizSql : public TTTns
{
	public:
		ActiveSendReverseBizSql();
		~ActiveSendReverseBizSql();
		
		// 查询未收到省回复的集团主动发出CCR
		int qryActiveSendLog(vector<ActiveSendLog> &vActiveSendLog,ABMException &oExp);
		
		// 更新CCR记录状态
		int updateCCRInfo(char *sRequestId);
		
		// 查询系统开关，
		int getSystemSwitch(char *switchId,char *switchValue);
		
		// 查询发送冲正的延迟秒数
		int getDelaySecond(long &lDelaySecond);
		
		// 根据延时发送秒数查询记录
		int qryActiveSendLog(vector<ActiveSendLog> &vActiveSendLog,long &lDelaySecond,ABMException &oExp);
};

#endif/*__ACTIVE_SEND_REVERSE_BIZ_SQL_H_INCLUDED_*/
