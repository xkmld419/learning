/*VER: 1*/ 
#ifndef _STAT_OTHER_EVENT_H_
#define _STAT_OTHER_EVENT_H_

/////////////////////////////
//
// OTHER_EVENT类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_OTHER_EVENT_CLASS.h"
//#include "TBL_SERV_CLASS.h"
#include "UserInfoReader.h"
#include "Date.h"
/****OtherEvents*****/
/* 定义域 */
#define		FA_OTHER_BILLFLOW_ID				1401
#define		FA_OTHER_BILLING_ORG_ID 		1402
#define		FA_OTHER_STAT_DAY    				1403
#define		FA_OTHER_STAT_HOUR				  1404
#define		FA_OTHER_SWITCH_ID  				1405
#define		FA_OTHER_TRUNK_IN     			1406
#define		FA_OTHER_TRUNK_OUT    			1407
#define		FA_OTHER_FILE_ID  					1408
#define		FA_OTHER_EVENT_STATE				1409
#define		FA_OTHER_ERROR_ID   				1410
#define		FA_OTHER_CYCLE_BEGIN_DAY		1411
#define		FA_OTHER_CYCLE_END_DAY			1412
                                            
/* 结果域 */                                
#define		FA_OTHER_PAGE								1430
#define		FA_OTHER_DURATION						1431
#define		FA_OTHER_BILL_DURATION			1432
#define		FA_OTHER_SEND_BYTES					1433
#define		FA_OTHER_RECV_BYTES					1434
#define		FA_OTHER_ORG_CHARGE1				1435
#define		FA_OTHER_CYCLE_DAY					1436



class StatOtherEvent;
#define  OID_OTHER_EVENT				120

class StatOtherEvent: public TStatClass
{
public:
    StatOtherEvent();
    ~StatOtherEvent();
    
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
    TBL_OTHER_EVENT_CLASS 	* pRead;
    //TBL_SERV_CLASS			TblServClass;
    /*TBL_CUST_CLASS			TblCustClass;*/
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

