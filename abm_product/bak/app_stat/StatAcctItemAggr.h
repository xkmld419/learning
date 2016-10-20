/*VER: 1*/ 
#ifndef _STAT_ACCT_ITEM_AGGR_H_
#define _STAT_ACCT_ITEM_AGGR_H_

/////////////////////////////
//
// AcctItemAggr类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_ACCT_ITEM_AGGR_CLASS.h"
#include "UserInfoReader.h"

/****ACCT_ITEM_AGGR*****/
/* 定义域 */
#define		FA_AGGR_ORG_LEVELID100				10101
#define		FA_AGGR_ORG_LEVELID200				10102
#define		FA_AGGR_ORG_LEVELID300				10103
#define		FA_AGGR_ORG_LEVELID400				10104
#define		FA_AGGR_ORG_LEVELID500				10105
#define		FA_AGGR_STATE									10106
#define		FA_AGGR_PARTNER_ID						10107
#define		FA_AGGR_CUST_TYPE_ID					10108
#define		FA_AGGR_ACCT_ITEM_TYPE_ID			10109
#define		FA_AGGR_BILLING_CYCLE_ID			10110
#define		FA_AGGR_PAYMENT_METHOD				10111
#define		FA_AGGR_STAT_DATE							10112
#define		FA_AGGR_SERV_STATE						10113
#define		FA_AGGR_BILLING_MODE					10114
#define		FA_AGGR_SERV_ID								10116
#define		FA_AGGR_ACCT_ID								10117
#define		FA_AGGR_OFFER_ID							10118
#define		FA_AGGR_FREE_TYPE_ID					10119
#define 	FA_AGGR_PRODUCT_ID						10121

/* 结果域 */                             
#define		FA_AGGR_CHARGE								10115
#define		FA_AGGR_PAGE									10120

class StatAcctItemAggr;
#define  OID_ACCT_ITEM_AGGR							124

class StatAcctItemAggr: public TStatClass
{
public:
    StatAcctItemAggr();
    ~StatAcctItemAggr();
    
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
     TBL_ACCT_ITEM_AGGR_CLASS			*pRead;
    
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

