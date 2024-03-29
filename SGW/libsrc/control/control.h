/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident	"@(#)control.h	1.0	2001/03/23	ruanyongjiang" */

/**********************************************
           控件结构以及调用方法  
   功能概要：
       提供字符终端的控件显示以及控件行为
   使用说明：
       主程序的编写：        
       #include "controls.h"
       #include "FormSample.h"
       main(){
            startwin();				  // 启动curses环境       
	    FormSample fs;
	    fs.run ();
            endwin();				  // 结束curses环境       
	}
       界面的编写：
	    1.  编写  .rc 文件。 ( 详见 ../src/中的例子 )
            2.  用可执行程序 makeform 来生成界面程序及对用户接口程序以及头文件 .
		如： makeform login.rc ==> 生成 login_j.c 和 login_f.c 以及 login.h头文件
	    3.  用户对login_f.c 中的各控件键盘事件填写  

       编译：gcc -o main main.c login_j.c login_f.c -lcontrols -lcurses

       另外提供了一些控制屏幕显示的函数： drawbox() ,drawblank() , prtmsg() ;
                                
 **********************************************/

#ifndef CONTROL_H
#define CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>

#include "macro.h"

typedef int  (* FUNC)(void *pObject,long lParam); /* 控件执行函数入口 */
typedef int  (* KEYPRESS)(void *pObject,int ich,long lParam);
					      /* 自定义控件键盘事件 */
typedef enum{BOX,LABEL,BUTTON,EDIT,LIST }TYPE;/* 控件类型 */

typedef struct TControl{
    int      iId;
    int      iTop,iLeft,iWidth,iHeight;   /* 控件的位置与大小 */  
    TYPE     eType;                       /* 控件的类别 */
    int      bFrame;                      /* 0:不要边框 1: 要边框 */
    int      iHotKey;			  /* 控件的热键  2001.2.5 增加 */
    int      iAdditional;                 /* 控件的其它特点 */
                                          /*  在EDIT: 表示输入类型 
						      可选：AMOUNT,NUMERIC,NUMBER,PASSWD,STATIC,STRING
					      在LIST: 添入list中的 ITEMS 数目  
    					  */     
    int      bSingle;                     /* LIST控件使用� 1:单选  其他:复选  */
    WINDOW   *pWin;                       /* 控件所在窗口的指针 */
    char     sData[MAX_ITEMS];            /* 控件的串空间 */
    char     **pData;                     /* 控件的指针型指针 */
    struct TControl *pIndex,*pTab,*pUp,*pDown,*pLeft,*pRight;  /* 控件周围的控件 pTab指TAB顺序*/
    FUNC     pFunc;                       /* 控件的执行函数入口 */
    KEYPRESS pKeyPress;                   /* 控件的按键触发事件 添加方式 。如返回等于0则不运行控件提供的键盘事件*/
    KEYPRESS pHotKeyPress;                /* 控件的系统热键触发事件 添加方式 。如返回等于0则不运行控件提供的键盘事件*/
} Control;
/*  控件对外提供的接口       defined in control.c */
Control *CtlInit(TYPE eType,int iId,int iTop,int iLeft,int iWidth,int iHeight);
		        		  /* 控件初试化 */
void     CtlSetDir(Control *pCtl,Control *pUp,Control *pDown,Control *pLeft,Control *pRight,Control *pTab,Control *pIndex);
					  /* 设置控件周围控件及TAB顺序 */
int      FormRun(Control *pFormHandle);     /* 进入控件事件循环 */
Control *GetCtlByID(Control *pFormHandle,int iId); /* 得到指定ID的控件 */

/*  一般为控件内部实现使用 */  
int      FormClear(Control *pFormHandle);   /* 对Form中的Edit List控件的内容清空 */
int      FormKill(Control *pFormHandle);    /* 释放所有控件 */
int      FormShow(Control *pFormHandle);    /* 所有控件的显示 */
void     CtlShow(Control *pCtl);          /* 控件的显示  */
void     CtlRedraw(Control *pCtl);        /* 控件显示的重画 */
Control *CtlChg(Control *pFirst,Control *curbtn,Control *chgbtn,int ich); /* 改变当前控件  */
int      CtlAtv(Control *pCurBtn,Control *pNxtBtn); /* 改变焦点    */
void     CtlClear (Control *pCtl);        /* clear display */

/* 对外提供的一些常用函数      defined in control.c */
void     drawbox(int iTop,int iLeft,int iWidth,int iHeight);   /* 画box */
void     drawblank(int iTop,int iLeft,int iWidth,int iHeight); /* 画空白区域用来对显示的控制，如FORM的切换 */
void     prtmsg(Control *pCtl,int iTop,int iLeft,char []);     /* 类似于MessageBox功能 */ 
void     startwin( );					       /* 启动curses环境       */
int      yorn(Control *pCtl,int iTop,int iLeft,char []);       /* Yes or No 的键盘等待回答 */
int      g_date(int day[3]);

int SelfBox (Control *pCtl);
#endif





