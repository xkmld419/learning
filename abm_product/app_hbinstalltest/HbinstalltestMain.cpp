#include "Hbinstalltest.h"
#include "MessageQueue.h"
#include "StdEvent.h"
#include "msgqueue.h"
#include "IpcKey.h"

//#include <error.h>
#include <iostream>
using namespace std;

int main()
{	
	char setorder[500] = {0};
	char runorder[500]= {0};
	char downorder[500]= {0};
	char billset[500]= {0};
	char billrun[500]= {0};
	char billdown[500]= {0};
	
	MessageQueue mq3(13);
	MessageQueue mq4(14);
	MessageQueue mq5(15);
	
	Log::log(0,"------测试实例------");
	Log::log(0,"------客户ID为     : 10001\n");
	Log::log(0,"------主叫号码     : 18922353301\n");
	Log::log(0,"------通话时长     : 30000分钟\n");
	Log::log(0,"------订购商品为   : 商品测试\n");
	Log::log(0,"------费用配置     : 每分钟2角\n");
	Log::log(0,"------预计产生话费 : 600000 \n");
	Log::log(0,"------计费安装验证开始------");
	Hbinstalltest *Hbtest = new Hbinstalltest();
	
	if(Hbtest->Init() !=0) return -1;
	//取本系统环境变量
	if(Hbtest->getenviroment() !=0) return -1;
	//写测试话单
	Log::log(0,"------测试话单导入");
	if(Hbtest->writeTestList())
		Log::log(0,"------写测试话单成功!\n\n");
	else 
		Log::log(0,"------失败 !\n\n");
	
	//导入测试三户资料和订购信息
	
	Log::log(0,"------三户资料和订购信息导入");
	if(Hbtest->createInfo()){
		Log::log(0,"------测试用户信息上载成功!\n\n");
	}
	else
		{
			Log::log(0,"------测试用户信息上载失败!\n\n");
			return -1;
		}
	
	
	//流程及相关表配置
	Log::log(0,"------测试流程及相关表配置");
	if(Hbtest->billflowCfg())
		Log::log(0,"------测试流程配置成功!\n\n");
	else
	{
		Log::log(0,"------测试流程配置失败!\n\n");
		return -1;
	}
	
		
	//根据当前主机信息跟新数据库表
	Hbtest->checkTableByInfo();

	//系统环境部署(启gardmain,设定账期,重载累计量,重建总账,用户信息上载等)
	Log::log(0,"------环境部署");
	Log::log(0,"------环境部署开始...\n");
	
	strncpy(billset,Hbtest->SET,sizeof(billset));
	strcpy(setorder,"sh ");
	strcat(setorder,billset);
	int ire = system(setorder);
	
	
	sleep(2);

	//创建必要的消息队列
  mq3.open (true, true, sizeof (StdEvent), 20);
  mq4.open (true, true, sizeof (StdEvent), 20);
  mq5.open (true, true, sizeof (StdEvent), 20);
  
  Log::log(0,"------创建必要的消息队列");
  sleep (10);
  
  bool bRunRest=true;
	//程序顺序启动
	//if(Hbtest->beginRunPro(GATHER_PROCESS) < 0) bRunRest=false;
	if(Hbtest->beginRunPro(PREP_PROCESS) < 0) bRunRest=false;
	if(Hbtest->beginRunPro(FORMAT_PROCESS) < 0) bRunRest=false;
	if(Hbtest->beginRunPro(DUP_PROCESS) < 0) bRunRest=false;
	if(Hbtest->beginRunPro(PRICING_PROCESS) < 0) bRunRest=false;
	if(Hbtest->beginRunPro(WRITEFILEDB_PROCESS) < 0) bRunRest=false;
	if(Hbtest->beginRunPro(FILEINSTORE_PROCESS) < 0) bRunRest=false;
		
	if(bRunRest)
	{
		sleep (10);
		if(/*Hbtest->processState(GATHER_PROCESS) == 0
		&&*/ Hbtest->processState(PREP_PROCESS) == 0
		&& Hbtest->processState(FORMAT_PROCESS) == 0
		&& Hbtest->processState(DUP_PROCESS) == 0
		&& Hbtest->processState(PRICING_PROCESS) == 0
		&& Hbtest->processState(WRITEFILEDB_PROCESS) == 0
		&& Hbtest->processState(FILEINSTORE_PROCESS) == 0)
	  {
			Log::log(0,"所有的流程正常运行!\n");
			
		}
		else//停止所有进程
		{
			Hbtest->stopPro(GATHER_PROCESS);
			Hbtest->stopPro(PREP_PROCESS);
			Hbtest->stopPro(FORMAT_PROCESS);
			Hbtest->stopPro(DUP_PROCESS);
			Hbtest->stopPro(PRICING_PROCESS);
			Hbtest->stopPro(WRITEFILEDB_PROCESS);
			Hbtest->stopPro(FILEINSTORE_PROCESS);
			
			//杀消息队列
      mq3.remove ();
      mq4.remove ();
      mq5.remove ();
      
      return -1;
			
		}
		
	}
	else//有进程启动不起来，有错误
	{
		Hbtest->stopPro(GATHER_PROCESS);
		Hbtest->stopPro(PREP_PROCESS);
		Hbtest->stopPro(FORMAT_PROCESS);
		Hbtest->stopPro(DUP_PROCESS);
		Hbtest->stopPro(PRICING_PROCESS);
		Hbtest->stopPro(WRITEFILEDB_PROCESS);
		Hbtest->stopPro(FILEINSTORE_PROCESS);
		
		//杀消息队列
    mq3.remove ();
    mq4.remove ();
    mq5.remove ();
    
    return -1;
	}
	
	Log::log(0,"请稍等 20 秒 正在检查数据是否正常!!!\n");
	sleep (20);
	
	if(Hbtest->printTestinfo())
		Log::log(0,"------流程测试成功!\n");
	else
		Log::log(0,"------未能成功入库,流程测试失败,请检查LOG日志!\n");
		
		
  Hbtest->stopPro(GATHER_PROCESS);
	Hbtest->stopPro(PREP_PROCESS);
	Hbtest->stopPro(FORMAT_PROCESS);
	Hbtest->stopPro(DUP_PROCESS);
	Hbtest->stopPro(PRICING_PROCESS);
	Hbtest->stopPro(WRITEFILEDB_PROCESS);
	Hbtest->stopPro(FILEINSTORE_PROCESS);
	
	//杀消息队列
	Log::log(0,"------删除消息队列!!!\n");
	
  mq3.remove ();
  mq4.remove ();
  mq5.remove ();
	
	if(Hbtest != NULL)
		delete Hbtest;
	return 0;
}
