/*VER: 1*/ 
#ifndef _STAT_BALANCE_SOURCE_H_
#define _STAT_BALANCE_SOURCE_H_

/////////////////////////////
//
// BalanceSource类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_BALANCE_SOURCE_CLASS.h"
#include "UserInfoReader.h"

/****BalanceSource*****/
/* 定义域 */
#define		FA_BS_ORG_LEVELID100				9401
#define		FA_BS_ORG_LEVELID200				9402
#define		FA_BS_ORG_LEVELID300				9403
#define		FA_BS_ORG_LEVELID400				9404
#define		FA_BS_ORG_LEVELID500				9405
#define		FA_BS_BALANCE_TYPE_ID				9406
#define		FA_BS_BALANCE_LEVEL					9407
#define		FA_BS_STATE_DATE					9408
#define		FA_BS_STATE							9409
#define		FA_BS_STAT_DATE						9410

/* 结果域 */                                
#define		FA_BS_BALANCE						9411



class StatBalanceSource;
#define  OID_BALANCE_SOURCE						116

class StatBalanceSource: public TStatClass
{
public:
    StatBalanceSource();
    ~StatBalanceSource();
    
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
     TBL_BALANCE_SOURCE_CLASS			*pRead;
    
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

