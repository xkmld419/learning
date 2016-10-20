/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident	"@(#)button.c	1.0	2001/01/11	ruanyongjiang" */
//#include "/usr/include/../old/usr/include/curses.h"

#ifdef DEF_HP
#define _XOPEN_SOURCE_EXTENDED
#endif


#include <curses.h>
#include "control.h"
#include "button.h"
/* 与外部调用无关 */
void  BtnChgClr(Control *pButton,int iAttr)
{
    int iLen,i,j;
    iLen = strlen(pButton->sData);
    if (iAttr==A_NORMAL) 
        wattrset(pButton->pWin,A_NORMAL);
    else 
        wattron(pButton->pWin,iAttr);
    for(i=0;i<pButton->iHeight/2;i++)
        for(j=0;j<pButton->iWidth;j++)
            mvwaddch(pButton->pWin,i,j,' ');
    for(i=0;i<(pButton->iWidth-iLen)/2;i++)
        mvwaddch(pButton->pWin,pButton->iHeight/2,i,' ');
    for(i=(pButton->iWidth-iLen)/2;i<pButton->iWidth;i++)
        mvwaddch(pButton->pWin,pButton->iHeight/2,i,' ');
    mvwaddstr(pButton->pWin,pButton->iHeight/2,(pButton->iWidth-iLen)/2,pButton->sData);
    for(i=pButton->iHeight/2+1;i<pButton->iHeight;i++)
        for(j=0;j<pButton->iWidth;j++)
            mvwaddch(pButton->pWin,i,j,' ');
    if (pButton->bFrame)
#ifndef _CH_
	SelfBox (pButton);
#else   
	box(pButton->pWin,'|','-');
#endif
    if (iAttr!=A_NORMAL) 
        wattroff(pButton->pWin,iAttr);
    touchwin(pButton->pWin);
    wmove(pButton->pWin,pButton->iHeight-1,pButton->iWidth-1);
    wrefresh(pButton->pWin);
    /* refresh(); */
}
int  BtnEntry(Control *pButton,long lParam)
{
   int ch,iReturn;
   do{  
        ch = getch();
        if (pButton->pHotKeyPress!=NULL){
           iReturn = pButton->pHotKeyPress(pButton,ch,0);
           if(!iReturn||iReturn==FORM_KILL||iReturn==FORM_KILL_OK||iReturn==FORM_CLEAR||iReturn !=ch)
		return iReturn;
	}
        if (pButton->pKeyPress!=NULL){
           iReturn = pButton->pKeyPress(pButton,ch,0);
           if(!iReturn||iReturn==FORM_KILL||iReturn==FORM_KILL_OK||iReturn==FORM_CLEAR||iReturn !=ch )
		return iReturn;
	}
	iReturn = BtnKeyPress(pButton,ch,0);        
        if (iReturn) return iReturn;
   }while(1);   

}

int  BtnKeyPress(Control *pButton,int ich,long lParam)
{
    return ich;   /* MDF RUAN 2001.2.5 */
/*
    switch(ich){
        case '\r' : return '\t';
        case '\t' : return '\t';
        case KEY_DOWN :  return KEY_DOWN; 
        case KEY_UP   :  return KEY_UP;
        case KEY_LEFT :  return KEY_LEFT;
        case KEY_RIGHT:  return KEY_RIGHT;
    }
    return 0;
*/
}

