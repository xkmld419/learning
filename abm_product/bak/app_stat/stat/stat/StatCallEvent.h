/*VER: 1*/ 
#ifndef _STAT_CALL_EVENT_H_
#define _STAT_CALL_EVENT_H_

/////////////////////////////
//
// CALL_EVENT类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_CALL_EVENT_CLASS.h"
#include "TBL_SERV_CLASS.h"
#include "UserInfoReader.h"

/****CallEvents*****/
/* 定义域 */
#define		FA_C_PAYMENT_METHOD				9001
#define		FA_C_ACCT_ITEM_BILL_STATE		9002
#define		FA_C_CUST_TYPE_ID				9003
#define		FA_C_EVENT_TYPE_ID				9004
#define		FA_C_CALLED_AREA				9005
#define		FA_C_SERV_CREATE_DATE			9006
#define		FA_C_ACCT_ITEM_TYPE_ID			9007
#define		FA_C_SERV_STATE					9008
#define		FA_C_FREE_TYPE_ID				9009
#define		FA_C_BILLING_MODE				9010
#define		FA_C_STAT_HOUR					9011
#define		FA_C_STAT_DAY					9012
#define		FA_C_SWITCH_ID					9013
#define		FA_C_ORG_LEVELID100				9014
#define		FA_C_ORG_LEVELID200				9015
#define		FA_C_ORG_LEVELID300				9016
#define		FA_C_ORG_LEVELID400				9017
#define		FA_C_ORG_LEVELID500				9018
#define		FA_C_BILLING_CYCLE_ID			9019
#define		FA_C_PRODUCT_ID					9020
#define		FA_C_OFFER_ID					9021
#define		FA_C_CALLING_SP_TYPE_ID			9022
#define		FA_C_CALLED_SP_TYPE_ID			9023
#define		FA_C_BILLING_VISIT_AREA_CODE	9024
                                            
/* 结果域 */                                
#define		FA_C_PAGE						9025
#define		FA_C_DURATION					9026
#define		FA_C_BILL_DURATION				9027
#define		FA_C_COUNTS						9028
#define		FA_C_ORG_CHARGE1				9029
#define		FA_C_STD_CHARGE1				9030
#define		FA_C_CHARGE1					9031



class StatCallEvent;
#define  OID_CALL_EVENT				2

class StatCallEvent: public TStatClass
{
public:
    StatCallEvent();
    ~StatCallEvent();
    
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
    TBL_CALL_EVENT_CLASS 	* pRead;
    TBL_SERV_CLASS			TblServClass;
    /*TBL_CUST_CLASS			TblCustClass;*/
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

