/* Copyright (c) 2001-<2010> Linkage, All rights Reserved */
/* #ident "@(#)hbclear_f.c	1.0	2010/08/30	<AutoCreate>" */
#include "abmclearfile.h"
#include "abmclear.h"
#include "abmmainmenu.h"

//增加登陆用户组鉴权
#include "GroupDroitMgr.h"

char *s_hbclear[10] ;
char m_hbclear[10][100] ;

Control *hbclear_handle;


int initlist() {
	sprintf(m_hbclear[0],"清理原始采集文件") ;
	sprintf(m_hbclear[1],"清理校验错误文件") ;
	sprintf(m_hbclear[2],"清理数据合并中间文件") ;
	sprintf(m_hbclear[3],"清理排重历史文件") ;
	sprintf(m_hbclear[4],"清理写文件历史文件") ;
	sprintf(m_hbclear[5],"清理tdtrans可能留下来的文件") ;
	sprintf(m_hbclear[6],"清理asn编码前的备份文件") ;
	sprintf(m_hbclear[7],"清理租费套餐费回退备份的回退文件") ;
	s_hbclear[0]=m_hbclear[0] ;
	s_hbclear[1]=m_hbclear[1] ;
	s_hbclear[2]=m_hbclear[2] ;
	s_hbclear[3]=m_hbclear[3] ;
	s_hbclear[4]=m_hbclear[4] ;
	s_hbclear[5]=m_hbclear[5] ;
	s_hbclear[6]=m_hbclear[6] ;
	s_hbclear[7]=m_hbclear[7] ;
	return 8 ;
}


int hbclear_SysHotKeyPress(Control *pCtl, int ch, long lParam)
{   
    /* Add your self code here: */

    return ch;
}

void hbclear_entry(Control *pHandle) /* hbclear 创建时触发调用 */
{   
    hbclear_handle = pHandle;
    /* Add your self code here: */
    Control *hbclear_list ;
    int num ;
    hbclear_list = GetCtlByID (hbclear_handle, 3) ;
    num = initlist() ;
    hbclear_list->iAdditional = num ;
	  hbclear_list->pData = s_hbclear ;
}

int hbclearCtl13Press(Control *pCtl,int ch,long lParam)
{   /* 时间的按键消息处理 */
    /* Add your self code here: */


    return ch;
}

int hbclearCtl1Press(Control *pCtl,int ch,long lParam)
{   /* 执行button的按键消息处理 */
    /* Add your self code here: */
    if (ch == '\r') {
    	Control *hbClear_List =NULL;
    	Control *hbClear_Edit =NULL;
    	hbClear_Edit = GetCtlByID(hbclear_handle,13) ;
    	
    	int flag = 0 ;
	    	try
	    	{
		    	ClearFile cf ;
		    	cf.setEndTime(hbClear_Edit->sData) ;
		    	hbClear_List = GetCtlByID (hbclear_handle, 3) ;

			/*
			GroupDroitMgr *MDroitMgr=new GroupDroitMgr();
			strcpy(sFullStaffID,MDroitMgr->getUsrName());
			UserFlag = MDroitMgr->IsRootUsr();
		    */
		    UserFlag=true;
		    	if(UserFlag){
		    		  if(strlen(hbClear_Edit->sData) == 0){
		    		  		prtmsg(pCtl,20,53,"请输入时间") ;
				    		 	return ch;
		    		  } 
						  for(int i = 0 ; i<hbClear_List->iAdditional ;i++)  {
								 if(hbClear_List->sData[0] == SELECTED )  {
								 	 cf.clearGaherTaskFile() ;
								 	 flag++ ;
								 }
								 if(hbClear_List->sData[1] == SELECTED )  {
								 	 cf.clearCheckErrorFile() ;
								 	 flag++ ;
								 }
								 if(hbClear_List->sData[2] == SELECTED )  {
								 	 cf.clearDataMergeFile() ;
								 	 flag++ ;
								 }
								 if(hbClear_List->sData[3] == SELECTED )  {
								 	 cf.clearDupHisFile() ;
								 	 flag++ ;
								 }
								 if(hbClear_List->sData[4] == SELECTED )  {
								 	 cf.clearwdHisFile() ;
								 	 flag++ ;
								 }
								 if(hbClear_List->sData[5] == SELECTED )  {
								 	 cf.clearTdtransTmpFile() ;
								 	 flag++ ;
								 }
								 if(hbClear_List->sData[6] == SELECTED )  {
								 	 cf.clearAsnBkFile() ;
								 	 flag++ ;
								 }
								 if(hbClear_List->sData[7] == SELECTED )  {
								 	 cf.clearRSexbakFile() ;
								 	 flag++ ;
								 }		  	
				    }
				 }else{
				 	  for(int j = 0 ; j<hbClear_List->iAdditional ;j++)  {
				    		 if(hbClear_List->sData[0] == SELECTED ) {
				    		 	      flag++ ;
				    		 		 		prtmsg(pCtl,20,53,"对不起,该用户没有清理文件的权限") ;
				    		 		 		break;
				    		 	}
		    		}
				 }
				 
		    if(!flag)
		      prtmsg(pCtl,20,53,"请用空格键选择要执行的命令") ;
	    }
	    catch(TOCIException &e)
	    {
	    		prtmsg(pCtl,20,53,e.getErrMsg()) ;
	    		return ch;	
	    }
    }
    return ch;
}

int hbclearCtl2Press(Control *pCtl,int ch,long lParam)
{   /* 退出的按键消息处理 */
    /* Add your self code here: */
    if (ch == '\r') {
     hbmainmenu tm;
	   tm.run() ;
		 return FORM_KILL;
	 }

    return ch;
}

int hbclearCtl3Press(Control *pCtl,int ch,long lParam)
{   /* 主框架的按键消息处理 */
    /* Add your self code here: */


    return ch;
}


