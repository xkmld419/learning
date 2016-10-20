/*VER: 1*/ 
#ifndef _STAT_ACCT_ITEM_H_
#define _STAT_ACCT_ITEM_H_

/////////////////////////////
//
// AcctItem类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_ACCT_ITEM_CLASS.h"
#include "UserInfoReader.h"

/****CallEvents*****/
/* 定义域 */
#define		FA_A_ORG_LEVELID100				9201
#define		FA_A_ORG_LEVELID200				9202
#define		FA_A_ORG_LEVELID300				9203
#define		FA_A_ORG_LEVELID400				9204
#define		FA_A_ORG_LEVELID500				9205
#define		FA_A_STATE						9206
#define		FA_A_PARTNER_ID					9207
#define		FA_A_CUST_TYPE_ID				9208
#define		FA_A_ACCT_ITEM_TYPE_ID			9209
#define		FA_A_BILLING_CYCLE_ID			9210
#define		FA_A_PAYMENT_METHOD				9211
#define		FA_A_STATE_DATE					9212
#define		FA_A_SERV_STATE					9213
#define		FA_A_BILLING_MODE				9214
#define		FA_A_SERV_ID					9216
#define		FA_A_ACCT_ID					9217
#define		FA_A_OFFER_ID					9218
/* 结果域 */                                
#define		FA_A_CHARGE						9215


class StatAcctItem;
#define  OID_ACCT_ITEM						114

class StatAcctItem: public TStatClass
{
public:
    StatAcctItem();
    ~StatAcctItem();
    
    /* 断点要求从1开始标号编号*/
    
   	POINT_TYPE GetMaxPoint(char * sTableName,POINT_TYPE Point);	
										/*
											取对应表中小于Point的最大断点值,无数据返回0;
										*/
	POINT_TYPE GetMinPoint(char * sTableName,POINT_TYPE Point);	
										/*
											取对应表中大于Point的最小断点值,无数据返回0; 
										*/

	void OpenCursor(char * sTableName,POINT_TYPE MinPoint,POINT_TYPE MaxPoint);
										/*
											打开[MinPoint,MaxPoint]闭包区间的游标
										*/
	bool GetRecordFromTable(POINT_TYPE &BreakPoint);		
										/* 取数据,提供本模块调用下游模块 
											true:	有记录要统计；
											false：	无数据要统计；
											lBreakPoint ：返回当前的断点值；
										*/
	void CloseCursor();					/* 关闭本次打开的游标 */
	
    int  GetFieldValue(int iFieldID,TFieldValue * pTFieldValue);
    									/* 提供下游模块调用
    										-1 : 失败
    										0 ：成功完成
    										1 ：本次成功完成，下次还有数据,
    											适合于返回多次数值
    									*/	
    int  GetCycle();					/* 取的分表周期 供自身调用*/
    
private:
     TBL_ACCT_ITEM_CLASS			*pRead;
    
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

