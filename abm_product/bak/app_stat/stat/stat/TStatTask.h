/*VER: 1*/ 
#ifndef _REALSTAT_TASK_CLASS_H__
#define _REALSTAT_TASK_CLASS_H__
#include "TOCIQuery.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <new.h>

enum{VT_UNKNOW=-1,VT_INT,VT_STRING} ;

#ifndef U_LONG_T 
#define U_LONG_T
typedef		unsigned long  	U_LONG;
#endif

#ifndef U_LLONG_T 
#define U_LLONG_T
typedef		unsigned long long  	U_LLONG;
#endif

#define BIT32OS

#ifdef BIT32OS
typedef	U_LLONG 	POINT_TYPE;
#define FORMAT_STR	"%llu"
#endif

#ifdef BIT64OS
typedef  U_LONG		POINT_TYPE;
#define FORMAT_STR	"%lu"
#endif

#define  FA_Expr                   10000

class StatMonthList;

/******统计域值节点*****/
class TFieldValue
{
public:
	TFieldValue(void);
	TFieldValue(long);
	TFieldValue(char *);
	TFieldValue(TFieldValue &);
	~TFieldValue();
	void SetValue(long);				/*	未知域类型情况下，直接赋值，并根据变量类型赋值域类型 */
	void SetValue(char *);				/*	未知域类型情况下，直接赋值，并根据变量类型赋值域类型 */
	TFieldValue& operator += (TFieldValue& d2 ) ;
	TFieldValue& operator = (TFieldValue& d2 ) ;
	bool		operator == (TFieldValue& d2 ) const;
	bool		operator != (TFieldValue& d2 ) const;
	
	void AutoSetValue(long);			/* 已知域类型情况下，自动转换赋值类型，并赋值
											1.若域是整形，则直接赋值
											2.若域是字符型，则先将整型变量转换成字符型后再赋值给域
										*/
	void AutoSetValue(char*);			/* 已知域类型情况下，自动转换赋值类型，并赋值
											1.若域是字符型，则直接赋值
											2.若域是整型，则先将字符型变量转换成整型后再赋值给域
										*/
	void SetDefaultValue(void);			/* 已知域类型情况下，设置默认值
											1.若域是字符型，则域值赋值为'NULL'
											2.若域是整型，则域值赋值为-1
										*/
	
	char	ValType;					/*VT_UNKNOW:未知类型； VT_INT:整形； VT_STRING：字符型*/
	union{
		long	lValue;					/*integer值*/
		char*	sValue;					/*string值*/
	}value;
};


class TStatClass{
public:
	TStatClass(int iStatClassID);
	TStatClass();
	~TStatClass();
	void SetClassID(int iStatClassID);	/* 设置统计大类标识 */
	
	void OutputResult();  				/* 数据入库 */
    void ProcessStat();	  				/* 处理当前记录 */
	void FreeResult();					/* 释放长期没有使用的内存节点 */
	bool GetRecord();					/* 取数据上游模块调用
											true:	有记录要统计；
											false：	无数据要统计；
										*/
	void Commit();						/* 提交入库 */
	/* 断点要求从1开始标号编号*/
	
	virtual POINT_TYPE GetMaxPoint(char * sTableName,POINT_TYPE Point)=NULL;	
										/*
											取对应表中小于Point的最大断点值,无数据返回0;
										*/
	virtual POINT_TYPE GetMinPoint(char * sTableName,POINT_TYPE Point)=NULL;	
										/*
											取对应表中大于Point的最小断点值,无数据返回0; 
										*/

	virtual void OpenCursor(char * sTableName,POINT_TYPE MinPoint,POINT_TYPE MaxPoint)=NULL;
										/*
											对于'INS'类型的统计类
											打开对应表的[MinPoint,MaxPoint]闭包区间的游标
											
											对于'UPD'类型的统计类
											打开对应表游标，MinPoint和MaxPoint无含有
										*/
										
	virtual bool GetRecordFromTable(POINT_TYPE &BreakPoint)=NULL;		
										/* 
										  取数据,提供本模块调用下游模块 
											对于'INS'类型的统计类：
												true:	有记录要统计；
												false：	无数据要统计；
												lBreakPoint ：返回当前的断点值；
											对于'UPD'类型的统计类：
												true:	有记录要统计；
												false：	无数据要统计；
												lBreakPoint ：无须返回；
										*/
										
	virtual void CloseCursor()=NULL;	/* 关闭本次打开的游标 */
	
    virtual int  GetFieldValue(int iFieldID,TFieldValue * pTFieldValue)=NULL;
    									/* 提供下游模块调用
    										-1 : 失败
    										0 ：成功完成
    										1 ：本次成功完成，下次还有数据,
    											适合于返回多次数值
    									*/	
    virtual int  GetCycle()=NULL;		/* 取的分表周期 供自身调用*/
    

	int 			m_ResultCount;		/* 内存中的统计记录数 */
	
private:
	void			SavePoint(POINT_TYPE BreakPoint);
	void 			SavePointToDB();
	bool 			GetRecord_INS();
	bool			GetRecord_UDP();
	int				iStatClassID;		/* 统计类标识*/
	StatMonthList	*pStatMonthListHead;/* 月统计链表头*/
	int				iRecordNum;			/* 已经处理记录数*/		
	int				TotalCount;			/* 总的统计记录数 */
	time_t			NowTime;			/* 输出时获取的时间	*/
	bool			iOutPutFlag;		/* 输出标志		*/
	
	char			sStatClassType[4];	/* 统计类的类型 :
											'INS': 数据源为只插入类型 ,基于清单级的统计
 											'UPD'：数据源为insert和update的类型，基于集市级的统计
										*/
										
	/* 以下为断点处理数据定义 */
	char 			sTableName[50];		/* 当前处理的表名,需要初始化 */
	POINT_TYPE		BreakPoint;			/* 当前表的处理段点 */
	bool			bFlushBreakPointFlag;/* 刷新断点标志,需要初始化 */
	bool 			bOpenFlag;	/* 需要初始化变量 */
	bool			bOpenCurFlag,bTurnOutFlag;
	POINT_TYPE 		lBeginPoint,lEndPoint,lCurPoint;
	POINT_TYPE		lMaxPoint1,lMinPoint1,lMaxPoint2,lMinPoint2;
	
	
friend class TStatTask;

};

extern void my_new_error();

#endif
