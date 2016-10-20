/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident	"@(#)control.c	1.0	2001/03/23	ruanyongjiang" */
/*************************************
  对Control.h中的控件方法实现的定义
**************************************/

#ifdef DEF_HP
#define _XOPEN_SOURCE_EXTENDED
#endif

#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "controls.h"




extern int  BtnEntry(Control *pCtl,long lParam); /* 在button.c 中定义 */
extern int ListEntry(Control *pCtl,long lParam); /* 在listbox.c中定义 */
extern int EditEntry(Control *pCtl,long lParam); /* 在edit.c   中定义 */

/**********************************/

int MvwAddStr( WINDOW *w1, int y, int x, char *str );

/**********************************/

/**********************************************************************
*  功能概要：控件的初始化					      *
*  使用说明：对 eType 参数赋值，调用CtlInit( TYPE)来初始化	      *
*            不同控件的赋值要求 参见control.h 中关于Control结构的描述 *
***********************************************************************/
Control * CtlInit(TYPE eType ,int iId,int iTop,int iLeft,int iWidth,int iHeight)
{
    Control *pCtl;
    if( (pCtl = (Control *)malloc(sizeof(Control)))==NULL){
	prtmsg(NULL,20,21,"Malloc Control error");
	endwin();
	exit(0);
    }
    pCtl->iId    = iId;
    pCtl->iTop   = iTop; pCtl->iLeft = iLeft;pCtl->iWidth = iWidth;
    pCtl->iHeight= iHeight;pCtl->pWin= NULL; pCtl->iHotKey=-1;
    pCtl->pUp    = NULL; pCtl->pDown = NULL; pCtl->pLeft  = NULL; 
    pCtl->pRight = NULL; pCtl->pTab  = NULL; pCtl->eType  = eType;
    pCtl->pKeyPress=NULL;pCtl->pData = NULL; pCtl->bFrame = 0;
    pCtl->pIndex =NULL;  pCtl->pHotKeyPress = NULL;
    memset(pCtl->sData,0,MAX_ITEMS);

    switch( eType ) {
	case LABEL  :   pCtl->iHeight=1;break;
	case BOX    :   break;
	case BUTTON :	pCtl->pFunc=(FUNC )&BtnEntry;break;
	case EDIT   :	pCtl->pFunc=(FUNC )&EditEntry;
		        pCtl->iAdditional = STRING;  /*默认为字符串*/
			break;
	case LIST   :	pCtl->pFunc=(FUNC )&ListEntry;
			pCtl->iAdditional = 0;
			pCtl->bSingle = 0;
			break;
    }

    return pCtl;
}
/**********************************************************************
*  功能概要：控件的显示						      *
*  使用说明：对 pCtl 参数赋值，调用CtlShow( pCtl)来显示		      *
***********************************************************************/
void CtlShow(Control *pCtl)
{
    int iLen,i;
    if(pCtl->eType==LABEL)pCtl->iWidth=strlen(pCtl->sData);
    if (pCtl->pWin==NULL)
	pCtl->pWin  = newwin(pCtl->iHeight,pCtl->iWidth,pCtl->iTop,pCtl->iLeft);

    CtlClear (pCtl);
    switch(pCtl->eType){
        case LABEL  :    mvwaddstr(pCtl->pWin,0,0,pCtl->sData);break;
	case BOX    :    
#ifdef _CH_
			 box(pCtl->pWin,'|','-');
#else
			 SelfBox (pCtl);
#endif
			 break;
        case BUTTON :    if (pCtl->bFrame)
#ifdef _CH_
			         box(pCtl->pWin,'|','-');
#else
			 	 SelfBox (pCtl);
#endif
                         iLen = strlen(pCtl->sData);
                         mvwaddstr(pCtl->pWin,pCtl->iHeight/2,(pCtl->iWidth-iLen+1)/2,pCtl->sData);
                         break;
        case EDIT   :    
			if (pCtl->bFrame) {
			if(pCtl->iAdditional!=STATIC) 
			    wattron(pCtl->pWin,A_REVERSE);    /* MDF 2001.2.7 */
			}
			 if (!pCtl->bFrame) {
                         mvwaddch(pCtl->pWin,0,0,'[');
                         mvwaddch(pCtl->pWin,0,pCtl->iWidth-1,']');
			 }

                         for(i=1-pCtl->bFrame;i<pCtl->iWidth-1+pCtl->bFrame;i++)
                             mvwaddch(pCtl->pWin,0,i,' ');
			/* } else {
			     mvaddch(pCtl->iTop,pCtl->iLeft-1,'[');   
			     mvaddch(pCtl->iTop,pCtl->iLeft+pCtl->iWidth,']');
			 }*//* MDF 2001.2.7 */
			 if(pCtl->iAdditional==PASSWD)
			     memset(pCtl->sData,0,MAX_ITEMS);

			 mvwaddstr(pCtl->pWin,0,1-pCtl->bFrame,pCtl->sData);
			 if (pCtl->bFrame) {
			 if(pCtl->iAdditional!=STATIC)
                            wattroff(pCtl->pWin,A_REVERSE);
			 }
                         break;
	case LIST   :    if (pCtl->bFrame) {
#ifdef _CH_
			         box(pCtl->pWin,'|','-');
#else
			 	 SelfBox (pCtl);
#endif
			 } else box(pCtl->pWin,' ',' ');
	 	         ListDisplay(pCtl,0,0);
			 break;
    }                       
    touchwin(pCtl->pWin);
    refresh();
    wrefresh(pCtl->pWin);
}
/**********************************************************
*  功能概要：控件的刷新				          *
*  使用说明：调用该函数处理在屏幕切换时需要处理的控件刷新 *
***********************************************************/
void CtlRedraw(Control *pCtl)
{
    werase(pCtl->pWin);
    pCtl->pWin  = newwin(pCtl->iHeight,pCtl->iWidth,pCtl->iTop,pCtl->iLeft);
    CtlShow(pCtl);
    touchwin(pCtl->pWin);
    refresh();
    wrefresh(pCtl->pWin);
}       
/**********************************************************************
*  功能概要：控件的改变，或输入焦点转移				      *
*  使用说明：一般不在外部调用，提供内部调用（在FormRun中 ）           *
*	     输入： 当前指针，当前接收的键盘事件                      *
*	     返回： 改变后的当前控件指针                              *
***********************************************************************/
Control *CtlChg(Control *pFirst, Control *curbtn,Control *chgbtn,int ich)
{
     Control *pCurCtl;

     /* ADD 2001.11.26 RUANYJ */
     if (ich >= BASE_ID) {
        for (pCurCtl = pFirst; pCurCtl &&pCurCtl->iId != ich-BASE_ID; 
		pCurCtl=pCurCtl->pIndex);
	
	if (pCurCtl != NULL) {
		CtlAtv (curbtn, pCurCtl);
		return pCurCtl;
	}
     }
     switch(ich){
	 case    0      :      CtlAtv(curbtn,curbtn);return curbtn; /* MDF 2001.2.5 RUANYJ */
         case  KEY_UP   :  if (CtlAtv(curbtn,curbtn->pUp))   return curbtn->pUp;   else return curbtn;    
         case  KEY_DOWN :  if (CtlAtv(curbtn,curbtn->pDown))  return curbtn->pDown;  else return curbtn;  
         case  KEY_LEFT :  if (CtlAtv(curbtn,curbtn->pLeft))  return curbtn->pLeft;  else return curbtn;    
         case  KEY_RIGHT:  if (CtlAtv(curbtn,curbtn->pRight)) return curbtn->pRight; else return curbtn;   
         case  '\t'     :  if (CtlAtv(curbtn,curbtn->pTab)) return curbtn->pTab; else return curbtn;   
	 case  HOT_KEY  :  if (CtlAtv(curbtn,chgbtn)) return chgbtn; else return curbtn;  /* MDF ruanyj 2001.2.5 */
         default        :  return curbtn;     
     }
}
/**********************************************************************
*  功能概要：设置控件在Form中的位置				      *
*  使用说明：CtlSetDir(自己，上，下，左，右，tab ,index);
***********************************************************************/
void  CtlSetDir(Control *pCtl,Control *pUp,Control *pDown,Control *pLeft,Control *pRight, Control *pTab,Control *pIndex)
{
    pCtl->pUp    = pUp;
    pCtl->pDown  = pDown;
    pCtl->pLeft  = pLeft;
    pCtl->pRight = pRight;
    pCtl->pTab   = pTab;
    pCtl->pIndex = pIndex;
}

/**********************************************************************
*  功能概要：控件的激活状态显示					      *
*  使用说明：一般不在外部调用，提供内部调用（在FormRun中 ）           *
*	     输入： 当前指针，next指针  	                      *
*	     返回： 是否改变 1-改变  0-不变                           *
***********************************************************************/
int   CtlAtv(Control *pCurBtn,Control *pNxtBtn)
{
    if (pNxtBtn!=NULL) {
	if(pCurBtn->eType == BUTTON) BtnChgClr(pCurBtn,A_NORMAL);
        switch(pNxtBtn->eType){
            case  BUTTON :   BtnChgClr(pNxtBtn,A_REVERSE);
                             return 1;      
            case  LIST   :   return 1;
            case  EDIT   :   touchwin(pNxtBtn->pWin);
                             wrefresh(pNxtBtn->pWin);
                             refresh();
                             return 1;      
	    default	 :   return 0;
        }           
    }
    return 0;
}
/**********************************************************************
*  功能概要：进入控件组（且称为Form）的事件循环，		      *
*  使用说明：输入： 当前指针，当前接收的键盘事件                      *
*	     返回： FORM_KILL :接收到结束Form的事件并返回             *
*  修改    ：增加键盘的热键处理，   2001.2.5                          *
***********************************************************************/
int FormRun(Control *pFormHandle)
{
    Control *curbtn,*pTmpCtl,*pTabStart;
    int isHotKey=0,ch=0; 
    if (pFormHandle==NULL) return -1;
    FormShow(pFormHandle);
    curbtn = pFormHandle;
    while( curbtn->pTab==NULL )curbtn=curbtn->pIndex;
    if (!curbtn) {
	prtmsg (pFormHandle, 20,21, "Active first tab CONTROL error");
	FormKill (pFormHandle);
	pFormHandle = NULL;
	return FORM_KILL_OK;
    }
    pTabStart = curbtn;
    CtlAtv(curbtn,curbtn);           /*  激活首控件  */
    do{ 
	isHotKey = 0;
        ch=curbtn->pFunc(curbtn,0);
	switch(ch){
	    case FORM_KILL    : FormKill(pFormHandle); /*  如果是结束当前Form则KILL,然后返回FORM_KILL_OK消息 */
				pFormHandle = NULL;
	    			return FORM_KILL_OK;
	    case FORM_CLEAR   :	FormClear(pFormHandle);
	    			FormShow(pFormHandle);
	    			curbtn = pTabStart;
	    			CtlAtv(curbtn,curbtn);
				break;
	    case FORM_KILL_OK :	FormShow(pFormHandle);
	    			CtlAtv(curbtn,curbtn);
				break;
	}
	pTmpCtl= pFormHandle;          /* 以下判断是否定义了该热键 */
	while( pTmpCtl!=NULL ) {
	    if( ch==pTmpCtl->iHotKey ) { 
		isHotKey = 1;
		break;
	     }
	    pTmpCtl = pTmpCtl->pIndex;
	}
	if( isHotKey )
	     curbtn = CtlChg(pFormHandle, curbtn,pTmpCtl,HOT_KEY);
        else curbtn = CtlChg(pFormHandle, curbtn,NULL,ch);

      }while(1);

}
/**********************************************************************
*  功能概要：画BOX。仅用来显示，显示结束就释放空间，不同于控件。      *
*  使用说明：						              *
*	     输入： 行，列，宽，高		                      *
*	     返回： 无			                              *
***********************************************************************/
void drawbox(int iTop,int iLeft,int iWidth,int iHeight)
{
    Control pCtl;
    pCtl.pWin = newwin(iHeight,iWidth,iTop,iLeft);
    pCtl.iTop		= iTop;
    pCtl.iLeft		= iLeft;
    pCtl.iWidth		= iWidth;
    pCtl.iHeight	= iHeight;

#ifdef _CH_
    box(pCtl.pWin,'|','-');
#else
    SelfBox (&pCtl);
#endif
    touchwin(pCtl.pWin);
    refresh();
    wrefresh(pCtl.pWin);
    delwin(pCtl.pWin);
}
/**********************************************************************
*  功能概要：画空白区域。仅用来clear，不是控件。		      *
*  使用说明：						              *
*	     输入： 行，列，宽，高		                      *
*	     返回： 无			                              *
***********************************************************************/
void drawblank(int iTop,int iLeft,int iWidth,int iHeight)
{
    WINDOW *pWin;
    pWin = newwin(iHeight,iWidth,iTop,iLeft);
    touchwin(pWin);
    refresh();
    wrefresh(pWin);
    delwin(pWin);
}
/**********************************************************************
*  功能概要：类似于MessageBox的功能，在指定的行列输出string,	      *
*	     然后等待any key返回到原光标位置                          *
*  使用说明：						              *
*	     输入： 当前控件指针，行，列，显示串	              *
*	     返回： 无			                              *
***********************************************************************/
void prtmsg(Control *pCtl,int iTop,int iLeft,char pStr[])
{
    short iLen,ixx,iyy;
/*add by jindh  in wx 2001/12/28*/
/*begin*/
short	iTemp=0;
char 	*pTempStr=NULL;
/*end*/
    getyx(stdscr,iyy,ixx);

    iLen =strlen(pStr);

    attron(A_REVERSE);
    mvaddstr(iTop,iLeft,pStr);
    refresh();

    noecho();
    getch();

    attroff(A_REVERSE);
    attrset(A_NORMAL);
/*add by jindh  in wx 2001/12/28*/
/*begin*/    
pTempStr=(char*) malloc(iLen+1);
if(pTempStr!=NULL)
{
	for(iTemp=0;iTemp<iLen;iTemp++)
	{
		pTempStr[iTemp]=' ';
	}
	pTempStr[iLen]='\0';
	mvaddstr(iTop,iLeft,pTempStr);
    refresh();
    free(pTempStr);
    pTempStr=NULL;
}
/*end*/


    drawblank(iTop,iLeft,iLen+3,1);
/*    if(pCtl->eType == BUTTON)move(0,0); */ /* MDF RUAN 2001.2.5 */

#ifdef CUR_RETURN0  /* 光标返回到0点 */
    refresh();
    move(0,0);
#else
    refresh();
    if(pCtl==NULL)move(0,0);
    CtlAtv (pCtl, pCtl);
    //else wmove(pCtl->pWin,iyy,ixx+1);
#endif

}
/**********************************************************************
*  功能概要：结束控件组（且称为Form）的事件循环，释放占用的内存	      *
*  使用说明：输入： Form首指针                     		      *
*	     返回： 0: 正常返回				              *
***********************************************************************/
int FormKill(Control *pFormHandle)
{
    Control *pTmp1,*pTmp2;
    if (pFormHandle == NULL)
	return 0;
    pTmp1 = pFormHandle;
    pTmp2 = pFormHandle->pIndex;
    while(pTmp2!=NULL){
	delwin(pTmp1->pWin);
	free(pTmp1);
	pTmp1 = NULL;
	if(pTmp2==NULL)return 0;
	pTmp1 = pTmp2;
	pTmp2 = pTmp2->pIndex;
    }
    return 0;
}
int FormShow(Control *pFormHandle)
{
    Control *pTmp;
    drawblank(0,0,80,24); 
    pTmp = pFormHandle;
    while(pTmp!=NULL){
	CtlShow(pTmp);
	pTmp = pTmp->pIndex;
    }
    return 0;
}
int FormClear(Control *pFormHandle)
{
    Control *pTmp;
    pTmp = pFormHandle;
    while(pTmp!=NULL){
	switch( pTmp->eType ){
	    case EDIT    : memset(pTmp->sData,0,MAX_ITEMS);break;
	    case LIST    : memset(pTmp->sData,0,MAX_ITEMS);
			   pTmp->pData = NULL;
			   pTmp->iAdditional = 0;
			   werase(pTmp->pWin);
			   break;
	    default	 : break;
	
	}
	pTmp = pTmp->pIndex;
    }
    return 0;
}
Control *GetCtlByID (Control *pHandle, int iId)
{
    Control *tmp;
    tmp = pHandle;
    while (tmp != NULL) {
	if (tmp->iId == iId) return (Control *)tmp;
	tmp = tmp->pIndex;
    }
    return (Control *)NULL;
}
void startwin()
{
    initscr(); 
    keypad(stdscr,TRUE);
    noecho();
    nonl();
}
int yorn(Control *pCtl,int iTop,int iLeft,char pStr[])
{
    short iLen,ixx,iyy;
    int ch,ret;
/*add by jindh  in wx 2001/12/28*/
/*begin*/
short	iTemp=0;
char 	*pTempStr=NULL;
/*end*/
    getyx(stdscr,iyy,ixx);

    iLen = strlen(pStr);
    attron(A_REVERSE);
    mvaddstr(iTop,iLeft,pStr);
    refresh();

    noecho();
    do {
	ch=getch();
	if(ch=='1'||ch=='y'||ch=='Y'){ ret=1;break; }
	if(ch=='0'||ch=='n'||ch=='N'){ ret=0;break; }
    }while(1);
    attroff(A_REVERSE);
    attrset(A_NORMAL);
/*add by jindh  in wx 2001/12/28*/
/*begin*/    
pTempStr=(char*) malloc(iLen+1);
if(pTempStr!=NULL)
{
	for(iTemp=0;iTemp<iLen;iTemp++)
	{
		pTempStr[iTemp]=' ';
	}
	pTempStr[iLen]='\0';
	mvaddstr(iTop,iLeft,pTempStr);
    refresh();
    free(pTempStr);
    pTempStr=NULL;
}
/*end*/    
    drawblank(iTop,iLeft,iLen+3,1);
    if(pCtl==NULL)move(0,0);
    else wmove(pCtl->pWin,iyy,ixx+1);
    refresh();
    CtlAtv (pCtl, pCtl);
    return ret;

}

//int g_date(day)
//int day[3];
int g_date(int day[3])
{
 long clock;
 struct tm *tim;

// time((void *)&clock);
 time((time_t *)&clock);
 tim=(struct tm *)localtime((const time_t *)&clock);

 day[2]=tim->tm_year+1900;
 day[0]=tim->tm_mon+1;
 day[1]=tim->tm_mday;
 return(0);
}

int SelfBox (Control *pCtl)
{
  int  i, j;
  int col;

  int line,icol, y, x;

  line = pCtl->iHeight;
  icol = pCtl->iWidth;
  y    = pCtl->iTop;
  x    = pCtl->iLeft;
  
  col = (int)( icol / 2 ) - 1;
  line = line -1;
  MvwAddStr( pCtl->pWin, 0, 0, "┌" );
  for ( i = 1; i < col; i++ )
    MvwAddStr( pCtl->pWin, 0, i*2, "─" );
  MvwAddStr( pCtl->pWin, 0, col*2, "┐" );
  for ( i = 1; i < line; i++ )
    MvwAddStr( pCtl->pWin, i, 0, "│" );
  for ( i = 1; i < line; i++ )
   MvwAddStr( pCtl->pWin, i, col*2, "│" );
  MvwAddStr( pCtl->pWin, line, 0, "└" );
  for ( i = 1; i < col; i++ )
   MvwAddStr( pCtl->pWin, line, i*2, "─" );
  j = MvwAddStr( pCtl->pWin, line, col*2, "┘" );

  wrefresh(pCtl->pWin);

  return 0;
}

int MvwAddStr( WINDOW *w1, int y, int x, char *str )
{
  int y1,x1;
  y1 = y;
  x1 = x;

  wmove(w1,y1,x1);
  waddstr(w1,str);
  return (1);
}

void CtlClear (Control *pCtl)
{
        int i,j;

        for (i=0; i<pCtl->iHeight; i++)
            for (j=0; j<pCtl->iWidth; j++) {
                        mvwaddch (pCtl->pWin, i, j, ' ');
            }
}

