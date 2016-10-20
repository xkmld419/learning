#ifndef SYSPARAMCLIENT_H_HEADER_INCLUDED_BDD
#define SYSPARAMCLIENT_H_HEADER_INCLUDED_BDD
#include <vector>

//#include "CmdMsg.h"
#include "interrupt.h"
#include <iostream>
#include  <stdlib.h>
#include "syscfg.h"
#include "MsgDefineClient.h"
#include "MsgDefineServer.h"
#include "SysParamInfo.h"
#include "SysParamDefine.h"
#include "BaseFunc.h"
#include "CommandCom.h"
#include "Log.h"
#include "IniFileMgr.h"
#include "BaseRuleMgr.h"

#include <map>

#ifdef _ARBITRATION_
#include "../app_accustd/ArbitrationMgr.h"
#include "../cai/interrupt_A.h"
#endif
#define PAGE_MAX_NUM   7    // 一页面最多显示的消息队列传来的CMDMSG数目
#define MAX_WAIT_LONG_TIME  30   // 等待返回消息的时间,单位是1秒
#define MAX_WAIT_T  10
#define PAGE_MAX_SHOW_NUM   12    // 一页面最多显示的参数数目

#define HBPARAM		"HBPARAM"
#define LIST_LISTALL	"LIST_LISTALL"

class SysParamClient 
{
      public:
             SysParamClient();
			 SysParamClient(int iMode);
             ~SysParamClient();
			 int resolveHBParam(char **Argv,int Argc);
			 // 初始化参数
			 void init(CommandCom *pCmdCom,int iWaitTime=MAX_WAIT_T);
			 // 合并新增,主要是解析命令参数
			 int resolveUsrParam(char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc);
			 
			 // 检测参数格式以及数值是否符合规定
             bool IfSuitRule(long lCMDID,char *sParamSetVal,string &strDsc,bool Show=true);
			 
	 		 //新功能
			 int resolveMemParam(char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc);
			 int dealWithMem(char *sSectName,char *sSysParamValue,long lCMDID,bool ShowErrInfo);
			 int dealMem(char *sSectName, string sSysParamValue, long lCMDID,bool Show);
			 //显示处理结果
			 long showMemMsgValues();
			 SysParamHis *m_poSysParamHis;
			 SysParamHis *m_poSysParamHisEx;
			 int m_iCnt;
			 int m_iCntEx;
			 unsigned int m_iErrNo;
			 
			 //MEMORY_DB 的提示
			 bool prompt(string sysparam,bool show = false);
			 // 现在提供帮助信息
			 void showHowToUseCMD();
			 void showResults( void *pData, int iNum);
			 // 参数重置
			 void reSetBaseParams();
			 // 参数格式转化
			 long VecToSysParamInfo(ParamHisRecord *pParamHisRecord,int &iParamNum,long lCMDID);
			 //
	         int resolveParam(char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc);
	  		 //获得规规整后的最终传给initserver的数据strDsc
	  		 bool paramParser(char *sAllParamVal,string &strDsc);
			 ParamHisRecord mRecord[1];//解析后的基础数据
	  public:
	  		 bool change2hssString(char *value,string &strdesc);
			 //ThreeLogGroup TLog;
	  public: //monitor	  		 
			 // 专门为monitor做的函数[主要是去除了部分不必要的认证以及错误显示]
			 int deal4Monitor(char *sParamName, string sSysParamValue, long lCMDID, CommandCom *pCmdCom);	 	
			 int deal4Monitor(char *sParamName, string sSysParamValue, long lCMDID);
			 //
			 void setDebugState(bool _Debug);//_Debug为true不显示错误提示[显示错误的话monitor界面有问题]			 	 
	  public:
	  		 //前台控制
			 void setFrontInfo(bool bFront,int iBegin, int iEnd, const char *sSplit);		 	 
			 long m_lSrcCMDID;//记录每次操作的命令ID			 	 
	  private:
	         ////前台相关的控制
	         bool m_bFront;
	         int  m_iBegin;
	         int  m_iEnd;
	         const char *m_pSplit;   
	  		 /* 这段代码是基础数据*/
			 bool m_bSetParams;
			 bool m_bRefreshParams;
			 bool m_bListParams;
			 bool m_bCreateParams;
			 bool m_bDeleteParams;
			 bool m_bListAllParams;
			 bool m_bShowHelp;
			 
			 //long m_lSrcCMDID;//记录每次操作的命令ID	
			 int  m_iArgc;//记录每次的参数个数			 			 
			 //连接initserver的参数
			 CommandCom *m_poCmdCom; 	
			 
			 //用户数据(去除了hbparam -set这些之后的数据,实际数据内容)				 
			 char m_sParamsValue[ARG_VAL_LEN];
			 			 
			 //规则检验,统一去除静态变量的使用
			 BaseRuleMgr *m_poBaseRuleMgr;				 	  		 
	  private:		 		 				 	 
			 // 解析数据,解析出段名赋给sParamName
			 void analyseParamName(char *sParamValue,char *sParamName);	
			 // 参数格式检测,主要检测数据的基本格式的
			 bool checkBaseFormat(char *sParamDate,long lCMD);
			 //显示一页参数
			 void showOnePage(SysParamHis *pData,int iNowPos,int iTotalNum);
			 //参数分页显示控制	 
			 int showControl(int &iPos);
			 SysParamHis *m_pSysParamGet;
	  public://参数仲裁使用 这个只在有参数仲裁平台的机器使用
	         #ifdef _ARBITRATION_
	  		 	ArbitrationMgr *m_pArbitrationMgr;
			 	int m_iState;
			 #endif
			 void send2Arbi(string str);
	  		 bool arbi;
			 bool getArbiResult(char *sValue);	
			 void recData();
	  
	  public:	 //2.8入网测试版本使用 现在已不使用	 			 
			 int deal(char *sParamName, string sSysParamValue, long lCMDID, CommandCom *pCmdCom);//跟initserver通信
			 int deal(char *sParamName, string sSysParamValue, long lCMDID,bool Show=true);			 			  		 			 
			 		 		 
			 // 清除消息队列数据	
			 void clearMsg();
			 // 时间等待控制
			 int waitTimer(int &iTimer,long lCMDID,bool Show=false);
			 // 最大等待时间
			 int m_iWaitMax;
			 //返回消息的缓冲,可以自由处理这些数据	 
			 //vector<CmdMsg*> vec_CmdMsg; 			 
			 // 清空vector数据			 			 
			 void clearPtr(); 	
			 int m_iVecNum;//vec_CmdMsg的参数个数
			 //跟initserver 通信的基础函数[一次通信]
	 	  	 int dealWithServer(char *sSectName,char *sSysParamValue,long lCMDID,bool ShowErrInfo=true);	 
			 //显示处理结果
			 long showMsgValues(bool Show=true);	//显示数据
			 // 初始化参数
			 void initParam(char *sSectionName);
			 char *getInitParamValue();
			 char m_sInitParamValue[2048];
			 char m_sInitSectionName[2048];
	  private://2.8入网版本 现在已不使用	 
			 //void showAgain(int i,vector<CmdMsg*>::iterator it);
			
			 bool checkFrontSet(char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc);
			 bool getFrontBeginEnd(int begineOffset, int endOffset,char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc);
	  public://测试函数
	  		 void show();
	  public: //这些是以前合并处理的功能,现在部分拆分出去 将不使用
	         /* 这些是获取数据的 */
			 char sLogLevel[512];
			 void setLogLevel();
			 bool m_bLogOut;
			 //bool paramFormatCheck(string sParamVal,long lCMD); // 参数格式检验
			 bool check();	// 规则检测	
			 bool m_bMONDebug;//TRUE不显示错误提示	
			 bool ParamLink; //单元用	
			 char m_sParamSetVal[ARG_VAL_LEN*2];//用户数据(去除了hbparam -set这些之后的数据,实际数据内容)	
			 char m_sMONErrInfo[70];//错误信息记录				 	
			 int dealParam(char *sParamName, string sSysParamValue, long lCMDID);	
			 // 检测参数是否符合规则设置值
			 int ruleCheck(long lCMDID,char *m_sSysParamValue);	
			 
};

#endif

