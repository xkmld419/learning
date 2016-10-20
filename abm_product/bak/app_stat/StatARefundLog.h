/*VER: 1*/ 
#ifndef _STAT_AREFUNDLOG_H_
#define _STAT_AREFUNDLOG_H_

/////////////////////////////
//
// A_Refund_Log类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_A_REFUND_LOG_CLASS.h"
#include "UserInfoReader.h"

/****CallEvents*****/
/* 定义域 102*/
#define		FA_REFUNDLOG_BSS_ORG_LEVELID100			20001
#define		FA_REFUNDLOG_BSS_ORG_LEVELID200			20002
#define		FA_REFUNDLOG_BSS_ORG_LEVELID300			20003
#define		FA_REFUNDLOG_BSS_ORG_LEVELID400			20004
#define		FA_REFUNDLOG_BSS_ORG_LEVELID500			20005
#define		FA_REFUNDLOG_STAFF_ID					20006
#define		FA_REFUNDLOG_PAYED_METHOD				20007
#define		FA_REFUNDLOG_BALANCE_TYPE_ID			20008
#define		FA_REFUNDLOG_STATE						20009
#define		FA_REFUNDLOG_STAT_DATE					20010
/* 结果域 */                                
#define		FA_REFUNDLOG_BALANCE					20051
#define		FA_REFUNDLOG_BILL_CNT					20052

class StatARefundLog;
#define  OID_AREFUNDLOG							200

class StatARefundLog: public TStatClass
{
public:
    StatARefundLog();
    ~StatARefundLog();
    
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
     TBL_A_REFUND_LOG_CLASS			*pRead;
    
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

