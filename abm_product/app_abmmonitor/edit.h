/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident	"@(#)edit.h	1.0	2001/01/11	ruanyongjiang" */
#ifndef _EDIT_H
#define _EDIT_H

#include <curses.h>
#include "control.h"
#include "CommonMacro.h"

/*  与外部调用无关       defined in edit.c   */
extern int  EditEntry(Control *pCtl,long lParam); /* Edit 执行函数入口 */
extern int  DoCheck(Control *pCtl, char *pStrBuf,int ch,int iInputType,int iWidth,int icnt);
						  /* Edit 检查输入字符，并做相应处理 */
extern int  EditKeyPress(Control *pCtl,int ich,long lParam);
						  /* Edit 键盘事件 */
#endif

