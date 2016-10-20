/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident	"@(#)edit.c	1.0	2001/01/11	ruanyongjiang" */
//#include "/usr/include/../old/usr/include/curses.h"

#ifdef DEF_HP
#define _XOPEN_SOURCE_EXTENDED
#endif

#include <curses.h>
#include "control.h"
#include "edit.h"

/*function define for  edit */
int  EditEntry(Control *pCtl,long lParam)
{	
    int ch,icnt,iRet,iY;
    if (pCtl->bFrame) {
    if(pCtl->iAdditional!=STATIC)   /* ²»ÊÇ¾²Ì¬±à¼­¿ò */
       wattron(pCtl->pWin,A_REVERSE);
    }
    iY = pCtl->iHeight/2;
    icnt = strlen(pCtl->sData);   
    if(pCtl->iAdditional!=PASSWD) 
	mvwaddstr(pCtl->pWin,0,1-pCtl->bFrame,pCtl->sData);
    wmove(pCtl->pWin,iY,icnt+1-pCtl->bFrame);
    wrefresh(pCtl->pWin);
    if( pCtl->iAdditional==STATIC )return '\t';
        do{  
            ch = getch();
            if (pCtl->pHotKeyPress!=NULL) {
	        iRet = pCtl->pHotKeyPress(pCtl,ch,0);
		if(!iRet||iRet==FORM_KILL||iRet==FORM_KILL_OK||iRet==FORM_CLEAR||iRet!=ch||iRet>=BASE_ID)
		    return iRet;
	    }
            if (pCtl->pKeyPress!=NULL) {
	        iRet = pCtl->pKeyPress(pCtl,ch,0);
		if(!iRet||iRet==FORM_KILL||iRet==FORM_KILL_OK||iRet==FORM_CLEAR||iRet!=ch||iRet>=BASE_ID)
		    return iRet;
	    }
    	    if(pCtl->iAdditional==STATIC)return ch;   /* ÊÇ¾²Ì¬±à¼­¿ò */
	    iRet = EditKeyPress(pCtl,ch,0);
	    if (iRet) return iRet;
        }while(1);   
}

int DoCheck
(Control *pCtl, char *pStrBuf,int ch,int iInputType,int iWidth,int icnt)
{
	
    int iOkFlag;
    iOkFlag = 0;
    if( icnt+2*(1-pCtl->bFrame) >= iWidth) return 0;
    switch( iInputType ){
	
	    case  AMOUNT  :  if( (ch>='0'&&ch<='9')||ch=='.' )
	                     iOkFlag=1;
		                 break;
	    case  NUMERIC :  if( ch>='0'&&ch<='9' )
		                 iOkFlag=1;
		                 break;
	    case  STRING  :  if( ch>=32&&ch<=126)
		                 iOkFlag=1;
				 break;
	    case  PASSWD  :  iOkFlag=1;
			     break;
    }
    if(iOkFlag == 1){
        pStrBuf[icnt]=(char)ch;
        icnt++;
        pStrBuf[icnt]='\0';
        return icnt;
    }
    else  return 0;
}

int EditKeyPress(Control *pCtl,int ich,long lParam)
{
    int iBreak,iReturn,iY;
    int icnt;
    icnt=strlen(pCtl->sData); 
    iY = pCtl->iHeight/2;    
    iBreak = 0 ;
    switch( ich ){	
	case 8:
        case 127      :  if (icnt<=0) icnt=0; else icnt--;
                         pCtl->sData[icnt] = '\0';
                         mvwaddch(pCtl->pWin,iY,icnt+1-pCtl->bFrame,' ');
                         if (!icnt)  
                             wmove(pCtl->pWin,iY,1-pCtl->bFrame);
                         else 
                             wmove(pCtl->pWin,iY,icnt+1-pCtl->bFrame);
                         wrefresh(pCtl->pWin);
			             break;
	case  13      :  ich='\t'; iBreak = 1; break;
        case '\t'     :  ich='\t'; iBreak = 1; break;
        case KEY_DOWN :  iBreak = 1; break; 
        case KEY_UP   :  iBreak = 1; break;
        case KEY_LEFT :  iBreak = 1; break;
        case KEY_RIGHT:  iBreak = 1; break;
	default       :  iReturn = DoCheck (pCtl, pCtl->sData,ich,pCtl->iAdditional,pCtl->iWidth,icnt); 
                         if (iReturn!=0) {
                              icnt = iReturn;
			      if (pCtl->iAdditional==PASSWD) mvwaddch(pCtl->pWin,iY,icnt-pCtl->bFrame,'*');	
                              else mvwaddch(pCtl->pWin,iY,icnt-pCtl->bFrame,ich); 
                         }
                         wrefresh(pCtl->pWin);
	}
    if (iBreak){
        /* strcpy(pCtl->sData,sBuf);   */
        if (pCtl->bFrame) {
		if(pCtl->iAdditional!=STATIC)   /* ²»ÊÇ¾²Ì¬±à¼­¿ò */
		    wattroff(pCtl->pWin,A_REVERSE);
	}
        return ich;
    }
    else return 0;      
}

