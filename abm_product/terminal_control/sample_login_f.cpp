/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)login_f.c	1.0	2006/01/11	<AutoCreate>" */
#include "login.h"

Control *login_handle;

int login_SysHotKeyPress(Control *pCtl, int ch, long lParam)
{   
    /* Add your self code here: */

    return ch;
}

void login_entry(Control *pHandle) /* login 创建时触发调用 */
{   
    login_handle = pHandle;
    /* Add your self code here: */


}

int loginCtl7Press(Control *pCtl,int ch,long lParam)
{   /* 密码输入框的按键消息处理 */
    /* Add your self code here: */


    return ch;
}
int loginCtl8Press(Control *pCtl,int ch,long lParam)
{   /* 确认按钮的按键消息处理 */
    /* Add your self code here: */


    return ch;
}
int loginCtl9Press(Control *pCtl,int ch,long lParam)
{   /* 退出按钮的按键消息处理 */
    /* Add your self code here: */


    return ch;
}
int loginCtl10Press(Control *pCtl,int ch,long lParam)
{   /* 改密码按钮的按键消息处理 */
    /* Add your self code here: */


    return ch;
}

