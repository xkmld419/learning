/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident	"@(#)control.h	1.0	2001/03/23	ruanyongjiang" */

/**********************************************
           ¿Ø¼ş½á¹¹ÒÔ¼°µ÷ÓÃ·½·¨  
   ¹¦ÄÜ¸ÅÒª£º
       Ìá¹©×Ö·ûÖÕ¶ËµÄ¿Ø¼şÏÔÊ¾ÒÔ¼°¿Ø¼şĞĞÎª
   Ê¹ÓÃËµÃ÷£º
       Ö÷³ÌĞòµÄ±àĞ´£º        
       #include "controls.h"
       #include "FormSample.h"
       main(){
            startwin();				  // Æô¶¯curses»·¾³       
	    FormSample fs;
	    fs.run ();
            endwin();				  // ½áÊøcurses»·¾³       
	}
       ½çÃæµÄ±àĞ´£º
	    1.  ±àĞ´  .rc ÎÄ¼ş¡£ ( Ïê¼û ../src/ÖĞµÄÀı×Ó )
            2.  ÓÃ¿ÉÖ´ĞĞ³ÌĞò makeform À´Éú³É½çÃæ³ÌĞò¼°¶ÔÓÃ»§½Ó¿Ú³ÌĞòÒÔ¼°Í·ÎÄ¼ş .
		Èç£º makeform login.rc ==> Éú³É login_j.c ºÍ login_f.c ÒÔ¼° login.hÍ·ÎÄ¼ş
	    3.  ÓÃ»§¶Ôlogin_f.c ÖĞµÄ¸÷¿Ø¼ş¼üÅÌÊÂ¼şÌîĞ´  

       ±àÒë£ºgcc -o main main.c login_j.c login_f.c -lcontrols -lcurses

       ÁíÍâÌá¹©ÁËÒ»Ğ©¿ØÖÆÆÁÄ»ÏÔÊ¾µÄº¯Êı£º drawbox() ,drawblank() , prtmsg() ;
                                
 **********************************************/

#ifndef CONTROL_H
#define CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>

#include "macro.h"

typedef int  (* FUNC)(void *pObject,long lParam); /* ¿Ø¼şÖ´ĞĞº¯ÊıÈë¿Ú */
typedef int  (* KEYPRESS)(void *pObject,int ich,long lParam);
					      /* ×Ô¶¨Òå¿Ø¼ş¼üÅÌÊÂ¼ş */
typedef enum{BOX,LABEL,BUTTON,EDIT,LIST }TYPE;/* ¿Ø¼şÀàĞÍ */

typedef struct TControl{
    int      iId;
    int      iTop,iLeft,iWidth,iHeight;   /* ¿Ø¼şµÄÎ»ÖÃÓë´óĞ¡ */  
    TYPE     eType;                       /* ¿Ø¼şµÄÀà±ğ */
    int      bFrame;                      /* 0:²»Òª±ß¿ò 1: Òª±ß¿ò */
    int      iHotKey;			  /* ¿Ø¼şµÄÈÈ¼ü  2001.2.5 Ôö¼Ó */
    int      iAdditional;                 /* ¿Ø¼şµÄÆäËüÌØµã */
                                          /*  ÔÚEDIT: ±íÊ¾ÊäÈëÀàĞÍ 
						      ¿ÉÑ¡£ºAMOUNT,NUMERIC,NUMBER,PASSWD,STATIC,STRING
					      ÔÚLIST: ÌíÈëlistÖĞµÄ ITEMS ÊıÄ¿  
    					  */     
    int      bSingle;                     /* LIST¿Ø¼şÊ¹ÓÃª 1:µ¥Ñ¡  ÆäËû:¸´Ñ¡  */
    WINDOW   *pWin;                       /* ¿Ø¼şËùÔÚ´°¿ÚµÄÖ¸Õë */
    char     sData[MAX_ITEMS];            /* ¿Ø¼şµÄ´®¿Õ¼ä */
    char     **pData;                     /* ¿Ø¼şµÄÖ¸ÕëĞÍÖ¸Õë */
    struct TControl *pIndex,*pTab,*pUp,*pDown,*pLeft,*pRight;  /* ¿Ø¼şÖÜÎ§µÄ¿Ø¼ş pTabÖ¸TABË³Ğò*/
    FUNC     pFunc;                       /* ¿Ø¼şµÄÖ´ĞĞº¯ÊıÈë¿Ú */
    KEYPRESS pKeyPress;                   /* ¿Ø¼şµÄ°´¼ü´¥·¢ÊÂ¼ş Ìí¼Ó·½Ê½ ¡£Èç·µ»ØµÈÓÚ0Ôò²»ÔËĞĞ¿Ø¼şÌá¹©µÄ¼üÅÌÊÂ¼ş*/
    KEYPRESS pHotKeyPress;                /* ¿Ø¼şµÄÏµÍ³ÈÈ¼ü´¥·¢ÊÂ¼ş Ìí¼Ó·½Ê½ ¡£Èç·µ»ØµÈÓÚ0Ôò²»ÔËĞĞ¿Ø¼şÌá¹©µÄ¼üÅÌÊÂ¼ş*/
} Control;
/*  ¿Ø¼ş¶ÔÍâÌá¹©µÄ½Ó¿Ú       defined in control.c */
Control *CtlInit(TYPE eType,int iId,int iTop,int iLeft,int iWidth,int iHeight);
		        		  /* ¿Ø¼ş³õÊÔ»¯ */
void     CtlSetDir(Control *pCtl,Control *pUp,Control *pDown,Control *pLeft,Control *pRight,Control *pTab,Control *pIndex);
					  /* ÉèÖÃ¿Ø¼şÖÜÎ§¿Ø¼ş¼°TABË³Ğò */
int      FormRun(Control *pFormHandle);     /* ½øÈë¿Ø¼şÊÂ¼şÑ­»· */
Control *GetCtlByID(Control *pFormHandle,int iId); /* µÃµ½Ö¸¶¨IDµÄ¿Ø¼ş */

/*  Ò»°ãÎª¿Ø¼şÄÚ²¿ÊµÏÖÊ¹ÓÃ */  
int      FormClear(Control *pFormHandle);   /* ¶ÔFormÖĞµÄEdit List¿Ø¼şµÄÄÚÈİÇå¿Õ */
int      FormKill(Control *pFormHandle);    /* ÊÍ·ÅËùÓĞ¿Ø¼ş */
int      FormShow(Control *pFormHandle);    /* ËùÓĞ¿Ø¼şµÄÏÔÊ¾ */
void     CtlShow(Control *pCtl);          /* ¿Ø¼şµÄÏÔÊ¾  */
void     CtlRedraw(Control *pCtl);        /* ¿Ø¼şÏÔÊ¾µÄÖØ»­ */
Control *CtlChg(Control *pFirst,Control *curbtn,Control *chgbtn,int ich); /* ¸Ä±äµ±Ç°¿Ø¼ş  */
int      CtlAtv(Control *pCurBtn,Control *pNxtBtn); /* ¸Ä±ä½¹µã    */
void     CtlClear (Control *pCtl);        /* clear display */

/* ¶ÔÍâÌá¹©µÄÒ»Ğ©³£ÓÃº¯Êı      defined in control.c */
void     drawbox(int iTop,int iLeft,int iWidth,int iHeight);   /* »­box */
void     drawblank(int iTop,int iLeft,int iWidth,int iHeight); /* »­¿Õ°×ÇøÓòÓÃÀ´¶ÔÏÔÊ¾µÄ¿ØÖÆ£¬ÈçFORMµÄÇĞ»» */
void     prtmsg(Control *pCtl,int iTop,int iLeft,char []);     /* ÀàËÆÓÚMessageBox¹¦ÄÜ */ 
void     startwin( );					       /* Æô¶¯curses»·¾³       */
int      yorn(Control *pCtl,int iTop,int iLeft,char []);       /* Yes or No µÄ¼üÅÌµÈ´ı»Ø´ğ */
int      g_date(int day[3]);

int SelfBox (Control *pCtl);
#endif





