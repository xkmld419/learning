/*VER: 1*/ 
#ifndef _STAT_PAYMENT_H_
#define _STAT_PAYMENT_H_

/////////////////////////////
//
// Payment类
//
/////////////////////////////

#include "TStatTask.h"

#include "StdEvent.h"

#include "TBL_PAYMENT_CLASS.h"
#include "UserInfoReader.h"

/****PayMent*****/
/* 定义域 */
#define		FA_P_BSS_ORG_LEVELID100			9601
#define		FA_P_BSS_ORG_LEVELID200			9602
#define		FA_P_BSS_ORG_LEVELID300			9603
#define		FA_P_BSS_ORG_LEVELID400			9604
#define		FA_P_BSS_ORG_LEVELID500			9605
#define		FA_P_STAFF_ID					9606
#define		FA_P_PAYED_METHOD				9607
#define 	FA_P_OPERATION_TYPE				9608
#define		FA_P_STATE_DATE					9609
#define		FA_P_STATE						9610
#define		FA_P_STAT_DATE					9611
                                             
/* 结果域 */                                 
#define		FA_P_BALANCE					9612
#define		FA_P_PAYMENT_CNT				9613



class StatPayment;
#define  OID_PAYMENT						118

class StatPayment: public TStatClass
{
public:
    StatPayment();
    ~StatPayment();
    
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
     TBL_PAYMENT_CLASS			*pRead;
    
    int GetExprFieldValue( int iFieldID, TFieldValue * pTFieldValue);
    
	UserInfoInterface 	interface;
    ServInfo			servinfo,*pservinfo;
    CustInfo			custinfo,*pcustinfo;
    
    Serv				serv,*pServ;
    
};

#endif

