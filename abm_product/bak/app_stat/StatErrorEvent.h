/*VER: 1*/ 
#ifndef _STAT_ERROR_EVENT_H_
#define _STAT_ERROR_EVENT_H_

/////////////////////////////
//
// ERROR_EVENT类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_ERROR_EVENT_CLASS.h"
#include "TBL_SERV_CLASS.h"
#include "UserInfoReader.h"

/****CallEvents*****/
/* 定义域 */

#define 	FA_ERROR_BILL_FLOW_ID						10201
#define		FA_ERROR_BILLING_ORG_ID					10202
#define		FA_ERROR_START_DATE							10203
#define		FA_ERROR_SWITCH_ID							10204
#define		FA_ERROR_FILE_ID								10205
#define		FA_ERROR_CREATED_DATE						10206
#define		FA_ERROR_EVENT_STATE						10207
#define		FA_ERROR_STATE_DATE							10208
#define		FA_ERROR_ERROR_ID								10209

             
/* 结果域 */
                          
#define		FA_DATA_PAGE										10210



class StatError;
#define  OID_ERROR										125

class StatError: public TStatClass
{
public:
    StatError();
    ~StatError();
    
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
    TBL_ERROR_EVENT_CLASS 	* pRead;
    TBL_SERV_CLASS			TblServClass;
    /*TBL_CUST_CLASS			TblCustClass;*/
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

