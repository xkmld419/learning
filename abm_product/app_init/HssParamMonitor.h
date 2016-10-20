#ifndef PARAM_MONITOR_F_H
#define PARAM_MONITOR_F_H

#include  <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "HashList.h"
#include "CommandCom.h"
#include "../app_init/SysParamClient.h"
#include "macro.h"
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

#define MAX_PARAM_SELECT_NUM  1024
#define MAX_PARAM_VAL_LEN  1024 //参数大小

#define FIR_NAME_LEN			71

#define MAX_ROW_CHAR_NUM		80 //一行最多字符数目

#define HBPARAM_NAME		63   //<=75
#define HBPARAM_LINE_VAL	70  // <=75

#define MAX_HB_SECT_LEN		256

#define _DBUG_T //调试的宏标识

struct StdParmMap
{
	int iStart;//开始的行数值
	int iLen; //行数,总用了几个param_list_data
	
	int iParamNameLen; //参数名用了几行(这些值是*10之后的)
	int iParamName4OrLen; //
	int iParamName4NewLen; //
	int iOldValLen;//原值行数
	int iNewValLen;//新值行数
	int iEffDateLen;//生效日期行数
	
	int getEndSeq()//结束的下标
	{
		return iStart+iLen-1;
	}
	
};



class HBParamMonitor
{
      public: 
             HBParamMonitor();
             ~HBParamMonitor();
             void init(int iWaitTime,CommandCom *pCmdCom);
			 // 参数信息(按显示行索引)  
			 char *param_list_index[MAX_ITEMS];
			 // 连接外部参数与索引
			 void linkParamListIndex(char pData[MAX_ITEMS][80],char *pIndex[MAX_ITEMS]);
			 // 实际处理函数() //参数格式:sVal例如SYSTEM.location=beijing,lCMDID为MSG_PARAM_CHANGE
			 long dealParamOpera(char *sSectName, char *sVal, long lCMDID);
			 // 根据之前获取的数据的行编号,从0开始
			 long dealParamOpera(int iLine ,char *sNewVal,char *sEffDate, long lCMDID);   
			 int  getRows();    //获取显示行数     
			 bool getErrInfo(char *sErrInfo); 
			 void clearEnv(); //数据全部清除   
	 public: //参数控制函数		   		 			 
			 char m_sErrInfo[70];//错误信息
			 int ruleCheck(long lCMD,char *sWholeVlaue);
			 ParamHisRecord* getParamRecordByOffset(int iOffset);//现用			 
			 SysParamClient *m_poCmdOpera; //调用这个的处理  					   	   			 			   			  			   
			 ParamHisRecord *getParamRecordByLine(int iLine); 					   	   			 			   			  			   
	 private:	 	 		 	 
			 CommandCom *m_poCmdCom; //跟initserver通信的变量		
	 		 int  m_iWaitTime;	//最久等待时间
			 char param_list_data[MAX_ITEMS][MAX_PARAM_VAL_LEN]; //最终数据	 		 
			 ParamHisRecord m_poParamHisRecord[MAX_ITEMS]; //记录获得initserver的参数数据信息,全局sys参数的变量
			 
             int m_iParamNum; // 参数个数 
			 int m_iRowNum;	  // 显示用的行数
			 HashList<int> *pIndexList;//记录行号与参数数据的对应关系
			 //规则检验
			 BaseRuleMgr *m_poBaseRuleMgr;
	private:		   
			 // 获取参数信息，结果排版到param_list_data以及param_list_index
	  	     long GetParamInfoAll();	
			 // 行字符数目,data的开始下标;同时建立index与data的联系,sStrAdd:要在参数前增加的字符,如'.OldValue='
			 int analyseParamInfo(int &iListPos,char *sParamName,char *sParam,char *sStrAdd,int iSysParamKey);		 
			 
			 // 参数重置为0			 			 
			 void resetParams(); 			 
			 void memsetSysInfo();
			 void FormatParamHisRecord();//格式规整
			 
	  public: //测试用函数
	  		 void show(long lCMD=0);		 
	  protected:			 			        			 
	  		 void copyParamHisRecord(ParamHisRecord *pPRecord);			   		 		 		   			 
			 ParamHisRecord pParamRollBack[10];
			 int iRoolBackTime;//回滚次数	
			 // 显示用的全局参数(不存在段的概念,但也记录段信息)<行,TSysParamInfo下标>	 			 
	 		 static TSysParamInfo *SysParamList; 
};

#endif
