/*VER: 1*/ 
#ifndef _STAT_CYCLE_FEE_EVENT_H_
#define _STAT_CYCLE_FEE_EVENT_H_

/////////////////////////////
//
// CYCLE_FEE_EVENT类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_CYCLE_FEE_EVENT_CLASS.h"
#include "TBL_SERV_CLASS.h"
#include "UserInfoReader.h"

/****CYCLE_FEE_Events*****/
/* 定义域 */
#define		FA_CYCLE_PAYMENT_METHOD						9901
#define		FA_CYCLE_ACCT_ITEM_BILL_STATE			9902
#define		FA_CYCLE_CUST_TYPE_ID							9903
#define		FA_CYCLE_EVENT_TYPE_ID						9904
#define		FA_CYCLE_CALLED_AREA							9905
#define		FA_CYCLE_SERV_CREATE_DATE					9906
#define		FA_CYCLE_ACCT_ITEM_TYPE_ID				9907
#define		FA_CYCLE_SERV_STATE								9908
#define		FA_CYCLE_FREE_TYPE_ID							9909
#define		FA_CYCLE_BILLING_MODE							9910
#define		FA_CYCLE_STAT_HOUR								9911
#define		FA_CYCLE_STAT_DAY									9912
#define		FA_CYCLE_SWITCH_ID								9913
#define		FA_CYCLE_ORG_LEVELID100						9914
#define		FA_CYCLE_ORG_LEVELID200						9915
#define		FA_CYCLE_ORG_LEVELID300						9916
#define		FA_CYCLE_ORG_LEVELID400						9917
#define		FA_CYCLE_ORG_LEVELID500						9918
#define		FA_CYCLE_BILLING_CYCLE_ID					9919
#define		FA_CYCLE_PRODUCT_ID								9920
#define		FA_CYCLE_OFFER_ID									9921
#define		FA_CYCLE_CALLING_SP_TYPE_ID				9922
#define		FA_CYCLE_CALLED_SP_TYPE_ID				9923
#define		FA_CYCLE_BILLING_VISIT_AREA_CODE	9924
#define 	FA_CYCLE_BILL_FLOW_ID							9932
#define 	FA_CYCLE_START_DATE								9933
#define		FA_CYCLE_END_DATE									9934
#define		FA_CYCLE_CYCLE_DATE_CNT						9935                                      
/* 结果域 */                           
#define		FA_CYCLE_PAGE											9925
#define		FA_CYCLE_DURATION									9926
#define		FA_CYCLE_BILL_DURATION						9927
#define		FA_CYCLE_COUNTS										9928
#define		FA_CYCLE_ORG_CHARGE1							9929
#define		FA_CYCLE_STD_CHARGE1							9930
#define		FA_CYCLE_CHARGE1									9931



class StatCycleFeeEvent;
#define  OID_CYCLE_FEE_EVENT				122

class StatCycleFeeEvent: public TStatClass
{
public:
    StatCycleFeeEvent();
    ~StatCycleFeeEvent();
    
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
    TBL_CYCLE_FEE_EVENT_CLASS 	* pRead;
    TBL_SERV_CLASS			TblServClass;
    /*TBL_CUST_CLASS			TblCustClass;*/
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

