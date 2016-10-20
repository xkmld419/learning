/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)test1_f.c	1.0	2012/03/13	<AutoCreate>" */
#include "test1.h"

Control *test1_handle;

int test1_SysHotKeyPress(Control *pCtl, int ch, long lParam)
{   
    /* Add your self code here: */

    return ch;
}

void test1_entry(Control *pHandle) /* test1 创建时触发调用 */
{   
    test1_handle = pHandle;
    /* Add your self code here: */


}

int test1Ctl1Press(Control *pCtl,int ch,long lParam)
{   /* ABM系统表名浏览button的按键消息处理 */
    /* Add your self code here: */

if (ch == '\r') {
	  //hblogin lg;
   // lg.run();
		return FORM_KILL;
	}
    return ch;
}
int test1Ctl2Press(Control *pCtl,int ch,long lParam)
{   /* SQL功能button的按键消息处理 */
    /* Add your self code here: */


    return ch;
}
int test1Ctl3Press(Control *pCtl,int ch,long lParam)
{   /* 应用命令button的按键消息处理 */
    /* Add your self code here: */


    return ch;
}
int test1Ctl4Press(Control *pCtl,int ch,long lParam)
{   /* 清理过期文件button的按键消息处理 */
    /* Add your self code here: */


    return ch;
}
int test1Ctl5Press(Control *pCtl,int ch,long lParam)
{   /* 退出的按键消息处理 */
    /* Add your self code here: */

if (ch == '\r') {
	  //hblogin lg;
   // lg.run();
		return FORM_KILL;
	}
    return ch;
}

