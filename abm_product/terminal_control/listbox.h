/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident	"@(#)listbox.h	1.0	2001/01/11	ruanyongjiang" */
#ifndef _TIBS_LIST_H
#define _TIBS_LIST_H

#include <curses.h>
#include "control.h"
/*  与外部调用无关    defined in listbox.c*/
extern int   ListDisplay(Control *pList,int iStartItem,int iKey); 
							/* ListBox 的显示 */
extern int   ListEntry(Control *pList,long lParam);     /* ListBox 的执行函数入口 */


/*  提供外部调用  defined in listbox.c */
int ListItemState(Control *pList, int iNo);		    /* 取得listbox第iNo个的选择状态 */
extern char *ListGetItem(Control *pList,int iNo);           /* 取得listbox第iNo个被选中的item串 */ 
extern char *ListGetSelItem(Control *pList,int iNo);        /* 取得listbox第iNo个被选中的item串 */ 
void ListSetItem(Control *pList,int iItemNo,int iState);    /* 设置item 的状态  */ 
int  ListSelNum(Control  *pList);			    /* 得到被选中的 items 数量 */
#endif


