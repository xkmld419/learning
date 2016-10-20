/*VER: 1*/ 
#ifndef _STAT_VALUE_ADDED_EVENT_H_
#define _STAT_VALUE_ADDED_EVENT_H_

/////////////////////////////
//
// VALUE_ADDED_EVENT类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_VALUE_ADDED_EVENT_CLASS.h"
#include "TBL_SERV_CLASS.h"
#include "UserInfoReader.h"

/****ValueAddedEvents*****/
/* 定义域 */
#define		FA_ADDVALUE_PAYMENT_METHOD					10001
#define		FA_ADDVALUE_ACCT_ITEM_BILL_STATE		10002
#define		FA_ADDVALUE_CUST_TYPE_ID						10003
#define		FA_ADDVALUE_EVENT_TYPE_ID						10004
#define		FA_ADDVALUE_CALLED_AREA							10005
#define		FA_ADDVALUE_SERV_CREATE_DATE				10006
#define		FA_ADDVALUE_ACCT_ITEM_TYPE_ID				10007
#define		FA_ADDVALUE_SERV_STATE							10008
#define		FA_ADDVALUE_FREE_TYPE_ID						10009
#define		FA_ADDVALUE_BILLING_MODE						10010
#define		FA_ADDVALUE_STAT_HOUR								10011
#define		FA_ADDVALUE_STAT_DAY								10012
#define		FA_ADDVALUE_SWITCH_ID								10013
#define		FA_ADDVALUE_ORG_LEVELID100					10014
#define		FA_ADDVALUE_ORG_LEVELID200					10015
#define		FA_ADDVALUE_ORG_LEVELID300					10016
#define		FA_ADDVALUE_ORG_LEVELID400					10017
#define		FA_ADDVALUE_ORG_LEVELID500					10018
#define		FA_ADDVALUE_BILLING_CYCLE_ID				10019
#define		FA_ADDVALUE_PRODUCT_ID							10020
#define		FA_ADDVALUE_OFFER_ID								10021
#define		FA_ADDVALUE_CALLING_SP_TYPE_ID			10022
#define		FA_ADDVALUE_CALLED_SP_TYPE_ID				10023
#define		FA_ADDVALUE_BILLING_VISIT_AREA_CODE	10024
                                              
/* 结果域 */                                  
#define		FA_ADDVALUE_PAGE										10025
#define		FA_ADDVALUE_DURATION								10026
#define		FA_ADDVALUE_BILL_DURATION						10027
#define		FA_ADDVALUE_COUNTS									10028
#define		FA_ADDVALUE_ORG_CHARGE1							10029
#define		FA_ADDVALUE_STD_CHARGE1							10030
#define		FA_ADDVALUE_CHARGE1									10031



class StatValueAddedEvent;
#define  OID_VALUE_ADDED_EVENT				123

class StatValueAddedEvent: public TStatClass
{
public:
    StatValueAddedEvent();
    ~StatValueAddedEvent();
    
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
    TBL_VALUE_ADDED_EVENT_CLASS 	* pRead;
    TBL_SERV_CLASS			TblServClass;
    /*TBL_CUST_CLASS			TblCustClass;*/
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

