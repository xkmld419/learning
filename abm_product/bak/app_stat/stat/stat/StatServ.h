/*VER: 1*/ 
#ifndef _STAT_SERV_H_
#define _STAT_SERV_H_

/////////////////////////////
//
// Serv类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_SERV_CLASS.h"
#include "UserInfoReader.h"

/****CallEvents*****/
/* 定义域 */
#define		FA_S_ORG_LEVELID100				9101
#define		FA_S_ORG_LEVELID200				9102
#define		FA_S_ORG_LEVELID300				9103
#define		FA_S_ORG_LEVELID400				9104
#define		FA_S_ORG_LEVELID500				9105
#define		FA_S_SERV_CREATE_DATE			9106
#define		FA_S_STATE_DATE					9107
#define		FA_S_PRODUCT_ID					9108
#define		FA_S_BILLING_MODE				9109
#define		FA_S_CUST_TYPE_ID				9110
#define		FA_S_STATE						9111
#define		FA_S_OFFER_ID					9112
#define		FA_S_AGENT_ID					9113
/* 结果域 */                                
#define		FA_S_SERV_CNT					9114

class StatServ;
#define  OID_SERV							3

class StatServ: public TStatClass
{
public:
    StatServ();
    ~StatServ();
    
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
     TBL_SERV_CLASS			*pRead;
    
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

