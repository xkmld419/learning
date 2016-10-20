/*VER: 1*/ 
#ifndef _STAT_SERV_ACCT_H_
#define _STAT_SERV_ACCT_H_

/////////////////////////////
//
// ServAcct类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_SERV_ACCT_CLASS.h"
#include "UserInfoReader.h"

/****CallEvents*****/
/* 定义域 */
#define		FA_SA_ORG_LEVELID100			8101
#define		FA_SA_ORG_LEVELID200			8102
#define		FA_SA_ORG_LEVELID300			8103
#define		FA_SA_ORG_LEVELID400			8104
#define		FA_SA_ORG_LEVELID500			8105
#define		FA_SA_STATE_DATE				8107
#define		FA_SA_PRODUCT_ID				8108
#define		FA_SA_STATE						8111
#define		FA_SA_OFFER_ID					8112
#define  	FA_SA_SERV_ID					8115
#define 	FA_SA_ACCT_ID					8116
#define		FA_SA_ITEM_GROUP_ID				8117
#define 	FA_SA_BILLING_CYCLE_ID			8118

class StatServAcct;
#define  OID_SERV_ACCT						127

class StatServAcct: public TStatClass
{
public:
    StatServAcct();
    ~StatServAcct();
    
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
	TBL_SERV_ACCT_CLASS			*pRead;
    
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

