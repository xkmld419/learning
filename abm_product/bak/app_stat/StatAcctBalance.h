/*VER: 1*/ 
#ifndef _STAT_ACCT_BALANCE_H_
#define _STAT_ACCT_BALANCE_H_

/////////////////////////////
//
// AcctBalance类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_ACCT_BALANCE_CLASS.h"
#include "UserInfoReader.h"

/****AcctBalance*****/
/* 定义域 */
#define		FA_AB_ORG_LEVELID100				9301
#define		FA_AB_ORG_LEVELID200				9302
#define		FA_AB_ORG_LEVELID300				9303
#define		FA_AB_ORG_LEVELID400				9304
#define		FA_AB_ORG_LEVELID500				9305
#define		FA_AB_BALANCE_TYPE_ID				9306
#define		FA_AB_BALANCE_LEVEL					9307
#define		FA_AB_STATE_DATE					9308
#define		FA_AB_STATE							9309

/* 结果域 */                                
#define		FA_AB_BALANCE						9310



class StatAcctBalance;
#define  OID_ACCT_BALANCE						115

class StatAcctBalance: public TStatClass
{
public:
    StatAcctBalance();
    ~StatAcctBalance();
    
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
     TBL_ACCT_BALANCE_CLASS			*pRead;
    
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

