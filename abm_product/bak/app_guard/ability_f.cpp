/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)ability_f.c	1.0	2011/07/15	<AutoCreate>" */
#ifndef _ABILITY_J_H__
#define _ABILITY_J_H__

#include <signal.h>
#include <unistd.h>

#include "TTTns.h"
#include "mainmenu.h"
#include "ability.h"

 


class AccessTT:public TTTns
{
	public:		
	AccessTT()
	{
		m_poHeadle = NULL;
	};
	int Init()
	{
		
		m_poHeadle = NULL;
		try 
	  {
        if (GET_CURSOR(m_poHeadle, m_poTTConn, m_sConnTns, m_oExp) != 0) 
		    {
            return -1;
        }
    }
    catch(...) 
	  {
        return -1;
    }
	
    return 0;
	}; 
	TimesTenCMD *m_poHeadle;
	ABMException m_oExp;		 
};




char* ability_list_index[1024];
char  ability_list_data[1024][80];

extern int g_iflowid[16];
extern int gStr2Arr(char *strfid);
extern int IsInArr(int ifid);

struct AbilityInfo
{
	int  iAbilityID;
	char cAbilityName[30];
	int  iNodeID;
	int  cNodeName;
	char cState[4];	
};

AbilityInfo AbilityInfoArray[1024];

int iShowNum = 0;

int GetAbilityInfo();
Control *ability_handle;

int ability_SysHotKeyPress(Control *pCtl, int ch, long lParam)
{   
    /* Add your self code here: */

    return ch;
}

void ability_entry(Control *pHandle) /* ability 创建时触发调用 */
{   
    ability_handle = pHandle;
    /* Add your self code here: */
   		int rows = 0;
   Control *pList;
				
		pList = GetCtlByID (ability_handle, 6);
		//pProcList->iAdditional = pInfoHead->iProcNum;
	
		rows = GetAbilityInfo ();
		pList->iAdditional = rows;
		pList->pData = ability_list_index;

}

int abilityCtl1Press(Control *pCtl,int ch,long lParam)
{   /* 刷新button的按键消息处理 */
    /* Add your self code here: */

    Control *pProcList;
		
		int rows = 0;
		if (ch == '\r') 
		{
			if ((rows = GetAbilityInfo ()) < 0)
		  {			    
			   return 0;
			}
		
			pProcList = GetCtlByID (ability_handle, 6);
		
			pProcList->iAdditional = rows;
			pProcList->pData = ability_list_index;
			
		
			CtlShow (pProcList);
			CtlAtv (pCtl, pCtl);
		}
    return ch;
}
int abilityCtl2Press(Control *pCtl,int ch,long lParam)
{   /* 能力启用button的按键消息处理 */
    /* Add your self code here: */
    
    Control *pList;
	int i, iFlag = 1;

	if (ch == '\r') 
	{

	   pList = GetCtlByID (ability_handle, 6);
	 
	    char sSql[1024] = {0};
	    sprintf(sSql,"update HSS_ACCESS_CAPABILITY_CONTROL set state = '10A' where accessed_capability_id = :m_accessed_capability_id");
	 AccessTT objTT;
	 
	  try
	  {
	     
	     objTT.Init();
	     
	     objTT.m_poHeadle->Prepare(sSql);
	     
	     objTT.m_poHeadle->Commit();
		  
	 
	  for (i=0; i<iShowNum; i++) 
	  {
	    if (ListItemState (pList, i) != SELECTED)
	    	continue;

	    iFlag = 0;
	     objTT.m_poHeadle->setParam(1 ,AbilityInfoArray[i].iAbilityID);		 
	    objTT.m_poHeadle->Execute();  
	  }
	  

	  if (iFlag)
	  {
	    prtmsg (pCtl, 20, 40, "请在列表中用空格选择");
	    objTT.m_poHeadle->Close();
    }
    else
    {
      objTT.m_poHeadle->Commit();
      objTT.m_poHeadle->Close();
      pList->iAdditional = GetAbilityInfo ();
	    pList->pData = ability_list_index;
	
	    CtlShow (pList);
	    CtlAtv (pCtl, pCtl);
	  }
	
	}
	catch(...)
	{
		objTT.rollback();
		prtmsg (pCtl, 20, 40, "变更失败");
	}
	
  }
  
    return ch;
}
int abilityCtl3Press(Control *pCtl,int ch,long lParam)
{   /* 能力停止button的按键消息处理 */
    /* Add your self code here: */
  Control *pList;
	int i, iFlag = 1;

 	if (ch == '\r') 
	{

	   pList = GetCtlByID (ability_handle, 6);
	 
	    char sSql[1024] = {0};
	    sprintf(sSql,"update HSS_ACCESS_CAPABILITY_CONTROL set state = '10X' where accessed_capability_id = :m_accessed_capability_id");
	  AccessTT objTT;
	 
	  try
	  {
	    
	     objTT.Init();
	     
	     objTT.m_poHeadle->Prepare(sSql);
	     
	     objTT.m_poHeadle->Commit();
		  
	 
	  for (i=0; i<iShowNum; i++) 
	  {
	    if (ListItemState (pList, i) != SELECTED)
	    	continue;

	    iFlag = 0;
	     objTT.m_poHeadle->setParam(1 ,AbilityInfoArray[i].iAbilityID);		 
	    objTT.m_poHeadle->Execute();  
	  }
	  

	  if (iFlag)
	  {
	    prtmsg (pCtl, 20, 40, "请在列表中用空格选择");
	    objTT.m_poHeadle->Close();
    }
    else
    {
      objTT.m_poHeadle->Commit();
      objTT.m_poHeadle->Close();
      pList->iAdditional = GetAbilityInfo ();
	    pList->pData = ability_list_index;
	
	    CtlShow (pList);
	    CtlAtv (pCtl, pCtl);
	  }
	
	}
	catch(...)
	{
		
		objTT.rollback();
		prtmsg (pCtl, 20, 40, "变更失败");
	}
	
  }
    return ch;
}
int abilityCtl4Press(Control *pCtl,int ch,long lParam)
{   /* 退出的按键消息处理 */
    /* Add your self code here: */
	 if (ch == '\r') 
	 {
		 mainmenu mm;
		 mm.run();
		 return FORM_KILL;
	 }

    return ch;
}
int abilityCtl6Press(Control *pCtl,int ch,long lParam)
{   /* 主框架的按键消息处理 */
    /* Add your self code here: */


    return ch;
}
int abilityCtl13Press(Control *pCtl,int ch,long lParam)
{   /* 网元号的按键消息处理 */
    /* Add your self code here: */


    return ch;
}

int GetAbilityInfo()
{	
	  Control *pEdit = GetCtlByID(ability_handle, 13);
	  gStr2Arr(pEdit->sData);	
	  

	   
	  
	    char cSql[1024] = {0};
	    
	    int iPos = 0;
	    

	    sprintf(cSql , "select a.accessed_capability_id, \
       b.capability_name, \
       c.node_id, \
       c.node_name, \
       a.state \
  from HSS_ACCESS_CAPABILITY_CONTROL a, \
       HSS_CAPABILITY_INFO           b, \
       HSS_SERVICE_NODE_INFO         c \
 where a.capability_id = b.capability_id \
   and a.node_id = c.node_id;");
	     AccessTT objTT;
	     objTT.Init();
	     
	     (objTT.m_poHeadle)->Prepare(cSql);
	     
	     objTT.m_poHeadle->Commit();
		   objTT.m_poHeadle->Execute();
		   
		while(!(objTT.m_poHeadle->FetchNext()))
		{
			memset(&(AbilityInfoArray[iPos]),0,sizeof(AbilityInfo));
			objTT.m_poHeadle->getColumn(1, &(AbilityInfoArray[iPos].iAbilityID));
			objTT.m_poHeadle->getColumn(2, AbilityInfoArray[iPos].cAbilityName);
			objTT.m_poHeadle->getColumn(3, &(AbilityInfoArray[iPos].iNodeID));
			objTT.m_poHeadle->getColumn(4, &(AbilityInfoArray[iPos].cNodeName));
			objTT.m_poHeadle->getColumn(5, AbilityInfoArray[iPos].cState);
			
			   if(g_iflowid[0] && (!IsInArr(AbilityInfoArray[iPos].iNodeID)))
    	   continue;
			
    	 memset(ability_list_data[iPos],0,80);
    	 sprintf(ability_list_data[iPos],"%-10d%-30s%-10d%-10s%-10s",AbilityInfoArray[iPos].iAbilityID,
    	                                               AbilityInfoArray[iPos].cAbilityName,
    	                                               AbilityInfoArray[iPos].iNodeID,
    	                                               AbilityInfoArray[iPos].cNodeName,
    	                                               AbilityInfoArray[iPos].cState    	                                               
    	                                           );
    	 ability_list_index[iPos] = ability_list_data[iPos];
    	 iPos++;
		}
		objTT.m_poHeadle->Close();
	     
    iShowNum = iPos;
    return iPos;
}
#endif

               