#ifndef CMD_SEND_RECV_H_
#define CMD_SEND_RECV_H_
#include "CommandCom.h"
//#include "ProcessIDMgr.h"
//#include "Variant.h"
#include "CmdMsg.h"
#include "hbcfg.h"
#include <vector>
#include "SysParamInfo.h"
#include "SysParamDefine.h"
#include  <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "MsgDefineClient.h"
#include "MsgDefineServer.h"
//#include "ProcessEx.h"
#define LONG_WAIT_TIME	200
#define WAIT_TIME	100

class CmdSendRecv
{
	public:
			CmdSendRecv();
			~CmdSendRecv();	
			//参数说明: 命令ID,段名,参数值,数据转化到的地址pParamRecord
			long dealParamValue(long lCMDID,char *sSect,char *sValue,ParamHisRecord *pParamRecord,int &m_iParamNum);			
						
	public:	//测试用函数	
			long sendAndGetDataNew(long lCMDID,char *sSect,char *sValue,ParamHisRecord *pParamRecord,CommandCom *pCmdCom);
			long sendAndGetDataEx(long lCMDID,char *sSect,char *sValue,ParamHisRecord *pParamRecord,CommandCom *pCmdCom,int iMsgID);
			int iParamNum; //参数个数[临时变量]
			void show();
			
			//参数说明: 命令ID,段名,参数值,数据转化到的地址
			long dealParamValue(long lCMDID,char *sSect,char *sValue);
			
	private: //测试用数据
			ParamHisRecord pRecord[1024];	//测试用的参数
	private:
			int m_iProcID;//获取的配置好的进程ID,主要是方便清除消息队列
			int m_iMsgID;// 消息队列ID
			
			vector<CmdMsg*> vec_CmdMsg;
			vector<CmdMsg>  vec_CmdMsgNew;//不用						
			
			static CommandCom *pCmd;
			//static ProcessIDMgr *pProID;
			static CmdMsgFactory *pMsgFactory;
			
			static CmdSender* pSender;
			
			bool m_bRegist;
			// 检测得到的参数数据是否是LONG类型(ptr - 1;long -2)
			int checkParam(CmdMsg *pCmdMsg);
			//发送接受数据
			int sendAndGetData(long lCMDID,char *sSect,char *sValue);
			// 转化数据格式为ParamHisRecord
			long VecToSysParamInfo(ParamHisRecord *pParamHisRecord,int &m_iParamNum);
			//返回值=1返回值[可能是报错的值];=2[成功,看参数个数];
			long  VecToSysParamInfo(ParamHisRecord *pParamHisRecord,CmdMsg *pCmdMsg);
			
			//发送接受数据
			//long sendAndGetData(vector<CmdMsg*> &vec_Msg,long lCMDID,char *sSect,char *sValue);
			// 转化数据格式为ParamHisRecord
			long VecToSysParamInfo(ParamHisRecord *pParamHisRecord,vector<CmdMsg*> &vec_Msg);
			// 数据重设
			void rePerpare();	
	protected:
					
};

#endif
