/*VER: 1*/ 
#ifndef _STAT_BALANCE_PAYOUT_H_
#define _STAT_BALANCE_PAYOUT_H_

/////////////////////////////
//
// BalancePayout类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_BALANCE_PAYOUT_CLASS.h"
#include "UserInfoReader.h"

/****BalancePayout*****/
/* 定义域 */
#define		FA_BP_ORG_LEVELID100				9501
#define		FA_BP_ORG_LEVELID200				9502
#define		FA_BP_ORG_LEVELID300				9503
#define		FA_BP_ORG_LEVELID400				9504
#define		FA_BP_ORG_LEVELID500				9505
#define		FA_BP_BALANCE_TYPE_ID				9506
#define		FA_BP_BALANCE_LEVEL					9507
#define		FA_BP_STATE_DATE					9508
#define		FA_BP_STATE							9509
#define		FA_BP_STAT_DATE						9510

/* 结果域 */                                
#define		FA_BP_BALANCE						9511



class StatBalancePayout;
#define  OID_BALANCE_PAYOUT						117

class StatBalancePayout: public TStatClass
{
public:
    StatBalancePayout();
    ~StatBalancePayout();
    
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
     TBL_BALANCE_PAYOUT_CLASS			*pRead;
    
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

