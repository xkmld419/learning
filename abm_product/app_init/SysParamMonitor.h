#ifndef PARAM_MONITOR_F_H
#define PARAM_MONITOR_F_H

#include  <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "HashList.h"
#include "CommandCom.h"
#include "macro.h"
#include "SysParamInfo.h"
#include "SysParamDefine.h"

#ifdef ID(_n_)
#undef ID(_n_)
#endif

#ifdef erase
#undef erase
#endif

#ifdef move
#undef move
#endif

#include "BaseFunc.h"
#include "BaseRuleMgr.h"
#include "Log.h"


#define SYS_PARAM_LINE_VAL	74  // <=80
#define MAX_PARAM_VAL_LEN  80

class SysParamMonitor
{
      public: 
             SysParamMonitor();
             ~SysParamMonitor();
             void init(int iWaitTime,CommandCom *pCmdCom);
			 // 参数信息(按显示行索引)  
			 char *param_list_index[MAX_ITEMS];
			 int  getRows();    //获取显示行数     	
			 SysParamHis *getParamRecordByLine(int iLine);				   	   			 			   			  			   
			 SysParamHis* getParamHisByOffset(int iOffset);	 
			 int dealParamOpera(char *sSectName,char *sParamVal, long lCMDID);	
			 //规则检验
			 BaseRuleMgr *m_poBaseRuleMgr;
			 int is_zh_ch(char p);			   	   			 			   			  			   
	 private://最新使用的代码	 20110812 	 		 
			 CommandCom *m_poCmdCom;		
	 		 int  m_iWaitTime;	//最久等待时间
			 char param_list_data[MAX_ITEMS][MAX_PARAM_VAL_LEN]; //最终数据	 		 
			 SysParamHis *m_poSysParamHis;
			 int dealWithMem(char *sSectName, char *sSysParamValue, long lCMDID);
             int m_iCnt; // 参数个数 
			 int m_iRowNum;	  // 显示用的行数
			 HashList<int> *pIndexList;//记录行号与参数数据的对应关系
	private:		   
			 // 获取参数信息，结果排版到param_list_data以及param_list_index
	  	     int GetParamInfoAll();	
			 // 行字符数目,data的开始下标;同时建立index与data的联系,sStrAdd:要在参数前增加的字符,如'.OldValue='
			 int analyseParamInfo(int &iListPos,char *sParamName,char *sParam,char *sStrAdd,int iSysParamKey,int type=1);		 	 
};

#endif
