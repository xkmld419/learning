/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident	"@(#)listbox.c	1.0	2001/01/11	ruanyongjiang" */

#ifdef DEF_HP
#define _XOPEN_SOURCE_EXTENDED
#endif

#include "control.h" 
#include "listbox.h"


/* define function for TList */
int ListDisplay(Control *pList,int iStartItem,int iKey)
{
    int i;

    if(iStartItem>=0&&iStartItem<pList->iAdditional){
	if (iKey==0) {
	    //scrollok(pList->pWin,TRUE);
	    werase(pList->pWin); 
	    if (pList->bFrame) {
#ifndef  _CH_
		 SelfBox(pList);
#else            
		 box(pList->pWin,'|','-');
#endif
	    } else box(pList->pWin,' ',' ');
	    for(i=0;i<pList->iHeight-2;i++){
	       if(iStartItem+i<pList->iAdditional){
	           if(pList->sData[iStartItem+i] == SELECTED)
		       mvwaddch(pList->pWin,i+1,2,'*');
	           else 
                       mvwaddch(pList->pWin,i+1,2,' ');
		   mvwaddstr(pList->pWin,i+1,4,(char *)(pList->pData[iStartItem+i]));
	       }	
	    }
	} else if (iKey==KEY_DOWN){
	   box(pList->pWin,' ',' ');
	   wscrl(pList->pWin,1);
	   if (pList->bFrame) {
#ifndef  _CH_
		 SelfBox(pList);
#else            
		 box(pList->pWin,'|','-');
#endif
	   } else box(pList->pWin,' ',' ');
	   if(pList->sData[pList->iHeight-3+iStartItem] == SELECTED)
	       mvwaddch(pList->pWin,pList->iHeight-2,2,'*');
	   mvwaddstr(pList->pWin,pList->iHeight-2,4,(char *)(pList->pData[pList->iHeight-3+iStartItem]));
	    /*   wrefresh(pList->pWin); */
	} else if (iKey==KEY_UP){
	   box(pList->pWin,' ',' ');
	   winsertln(pList->pWin);
	   if (pList->bFrame) {
#ifndef  _CH_
		 SelfBox(pList);
#else            
		 box(pList->pWin,'|','-');
#endif
	   } else box(pList->pWin,' ',' ');
	   if(pList->sData[iStartItem] == SELECTED)
	       mvwaddch(pList->pWin,1,2,'*');
	   mvwaddstr(pList->pWin,1,4,(char *)(pList->pData[iStartItem]));
	    
	}
	if(iStartItem>0)
	    mvwaddstr(pList->pWin,1,pList->iWidth-2,"^ ");	
	if(pList->iAdditional-1-iStartItem>pList->iHeight-3)
	    mvwaddstr(pList->pWin,pList->iHeight-2,pList->iWidth-2,"v ");
    }
	return 0;
}

int ListEntry(Control *pList,long lParam)
{
    int ch,iReturn,i;
    int iTmp=0,iRow=0;
    ListDisplay(pList,iTmp,0);
    wmove(pList->pWin,iRow+1,3); 
    wrefresh(pList->pWin); 
    noecho();
    /* if (pList->pKeyPress==NULL) */
    do{  
        ch = getch();
	if (pList->pHotKeyPress!=NULL) {
            iReturn = pList->pHotKeyPress(pList,ch,0);
            if(!iReturn||iReturn==FORM_KILL||iReturn==FORM_KILL_OK||iReturn==FORM_CLEAR||iReturn!=ch)
	    return iReturn;
        }
	if (pList->pKeyPress!=NULL) {
            iReturn = pList->pKeyPress(pList,ch,0);
            if(!iReturn||iReturn==FORM_KILL||iReturn==FORM_KILL_OK||iReturn==FORM_CLEAR||iReturn!=ch)
	    return iReturn;
        }
        switch(ch){
            case KEY_LEFT      :  return KEY_LEFT;
            case KEY_RIGHT     :  return KEY_RIGHT;
	        case 13        :  return '\t';
	        case '\t'      :  return '\t';
      
	        case 32        :  if(pList->iAdditional<1)break;
	                          if(pList->bSingle !=1){
			                      if(pList->sData[iTmp+iRow] == SELECTED){
			                          mvwaddch(pList->pWin,iRow+1,2,' ');
		 	                          pList->sData[iTmp+iRow] = ' ';
			                      } else {
			                          mvwaddch(pList->pWin,iRow+1,2,'*');
		 	                          pList->sData[iTmp+iRow] = SELECTED;
			                      }
			                  } else {
			                          if(pList->sData[iTmp+iRow] == SELECTED){
			                              mvwaddch(pList->pWin,iRow+1,2,' ');
		 	                              pList->sData[iTmp+iRow] = ' ';
			                          } else {
				                          for(i=0;i<pList->iAdditional;i++)
				                              pList->sData[i] = ' ';
			                              for(i=0;i<pList->iHeight-2;i++)
				                              mvwaddch(pList->pWin,i+1,2,' ');
				                          mvwaddch(pList->pWin,iRow+1,2,'*');
		 	                              pList->sData[iTmp+iRow] = SELECTED;
				                          wmove(pList->pWin,iRow+1,3);
			                          }

			                  }
		 	                  break;
	    case KEY_DOWN   :  if( iRow<pList->iHeight-3 &&iRow+iTmp<pList->iAdditional-1){
			                      wmove(pList->pWin,++iRow+1,3);
			                      break;
			                  }
			                  /* if (iRow>= pList->iHeight-3&&iRow+iTmp>=pList->iAdditional-1)
					      return ch;  */
			                  if (iRow>= pList->iHeight-3&&iRow+iTmp<pList->iAdditional-1)
			                      //ListDisplay(pList,++iTmp,ch);
			                      ListDisplay(pList,++iTmp,0);
			                  wmove(pList->pWin,iRow+1,3);
			                  break;
            case KEY_UP   :   if( iRow > 0 ){
			                      wmove(pList->pWin,--iRow+1,3);
			                      break;
			                  }
			      if (iRow==0&&iTmp==0)
				  			return ch; 
			      if (iRow==0&&iTmp>0)
			          //ListDisplay(pList,--iTmp,ch);
			          ListDisplay(pList,--iTmp,0);
			      /* if (iRow==0&&iTmp==0)return ch; */
			      wmove(pList->pWin,iRow+1,3);
			      break;
	       default   :       break;
        }
        wrefresh(pList->pWin); 
    }while(1); 
    return 0;
}

int ListItemState(Control *pList, int iNo)
{

    if( iNo<pList->iAdditional )
	return pList->sData[iNo];
    else
	return UNSELECTED;
}
char *ListGetSelItem(Control *pList,int iNo)
{
    int i=0,j=0;
    if( iNo>=pList->iAdditional||iNo<0 )
        return (char *)NULL;    
    
    for( i=0;i<pList->iAdditional;i++){
	    if( pList->sData[i] == SELECTED )
            j++;
    	if( j == iNo+1 )  break;
    }
    if( j==iNo+1 )     
        return  pList->pData[i];
    else 	 
       return (char *)NULL;
}

char *ListGetItem(Control *pList,int iNo)
{
    int i=0,j=0;
    if( iNo>=pList->iAdditional||iNo<0 )
        return (char *)NULL;    
    
    for( i=0;i<pList->iAdditional;i++){
	    if( pList->sData[i] == SELECTED )
            j++;
    	if( j == iNo+1 )  break;
    }
    if( j==iNo+1 )     
        return  pList->pData[i];
    else 	 
       return (char *)NULL;
}
void ListSetItem(Control *pCtl,int iItemNo,int iState)
{
    int i;
    if( iItemNo<pCtl->iAdditional )
	pCtl->sData[iItemNo] = iState ;
    else if( iItemNo == ALL_ITEMS ) {
	for(i=0;i<pCtl->iAdditional;i++) pCtl->sData[i] = iState;
    }
}
int ListSelNum(Control *pCtl)
{
    int i=0,ret=0;
    while( i<pCtl->iAdditional )
	if(pCtl->sData[i++]==SELECTED)ret++;
    return ret;
}


