/*VER: 1*/ 
#ifndef _STAT_DATA_EVENT_H_
#define _STAT_DATA_EVENT_H_

/////////////////////////////
//
// DATA_EVENT类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_DATA_EVENT_CLASS.h"
#include "TBL_SERV_CLASS.h"
#include "UserInfoReader.h"

/****CallEvents*****/
/* 定义域 */
#define		FA_DATA_PAYMENT_METHOD					9801
#define		FA_DATA_ACCT_ITEM_BILL_STATE		9802
#define		FA_DATA_CUST_TYPE_ID						9803
#define		FA_DATA_EVENT_TYPE_ID					9804
#define		FA_DATA_CALLED_AREA						9805
#define		FA_DATA_SERV_CREATE_DATE				9806
#define		FA_DATA_ACCT_ITEM_TYPE_ID			9807
#define		FA_DATA_SERV_STATE							9808
#define		FA_DATA_FREE_TYPE_ID						9809
#define		FA_DATA_BILLING_MODE						9810
#define		FA_DATA_STAT_HOUR							9811
#define		FA_DATA_STAT_DAY								9812
#define		FA_DATA_SWITCH_ID							9813
#define		FA_DATA_ORG_LEVELID100				9814
#define		FA_DATA_ORG_LEVELID200				9815
#define		FA_DATA_ORG_LEVELID300				9816
#define		FA_DATA_ORG_LEVELID400				9817
#define		FA_DATA_ORG_LEVELID500				9818
#define		FA_DATA_BILLING_CYCLE_ID			9819
#define		FA_DATA_PRODUCT_ID						9820
#define		FA_DATA_OFFER_ID							9821
#define		FA_DATA_CALLING_SP_TYPE_ID		9822
#define		FA_DATA_CALLED_SP_TYPE_ID			9823
#define		FA_DATA_BILLING_VISIT_AREA_CODE	9824
             
/* 结果域 */                               
#define		FA_DATA_PAGE									9825
#define		FA_DATA_DURATION							9826
#define		FA_DATA_BILL_DURATION				9827
#define		FA_DATA_COUNTS								9828
#define		FA_DATA_ORG_CHARGE1					9829
#define		FA_DATA_STD_CHARGE1					9830
#define		FA_DATA_CHARGE1							9831
#define		FA_DATA_RECV_BYTES					9832
#define		FA_DATA_SEND_BYTES					9833
#define		FA_DATA_RECV_PACKETS				9834
#define		FA_DATA_SEND_PACKETS				9835


class StatDataEvent;
#define  OID_DATA_EVENT				121

class StatDataEvent: public TStatClass
{
public:
    StatDataEvent();
    ~StatDataEvent();
    
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
    TBL_DATA_EVENT_CLASS 	* pRead;
    TBL_SERV_CLASS			TblServClass;
    /*TBL_CUST_CLASS			TblCustClass;*/
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

