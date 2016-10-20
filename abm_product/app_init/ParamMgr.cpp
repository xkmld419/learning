#include "ParamMgr.h"
DEFINE_MAIN(SgwParam)
extern int g_iFdFront;
extern int g_iStart;
extern int g_iEnd;
SgwParam::SgwParam()
{
}

SgwParam::~SgwParam()
{
}

int SgwParam::run()//int argc, char* argv[])
{
	ThreeLogGroup TLog;
 

	GroupDroitMgr *pGroupDroitCtl = new GroupDroitMgr();
	char *pChr = pGroupDroitCtl->getUsrName();
	CommandCom *m_poCmdCom = new CommandCom();
	
	if(m_poCmdCom) 
	{	
		if(!m_poCmdCom->InitClient())
		{			
			printf("注册失败,请检查abmparam以及initserver!\n");	
			Log::log(0,"%s","向initserver注册失败");	
			return -1;
		}
	} else {
		printf("获取空间失败!\n");	
		Log::log(0,"%s","获取空间失败");	
		return -1;
	}
	m_poCmdCom->setParamUse(true);

	SysParamClient *m_poSysParam = new SysParamClient();
	if(m_poSysParam)
	{   	
		m_poSysParam->init(m_poCmdCom,HBPARAM_WAIT_TIME_NEED);	
	} else {
		printf("获取空间失败!\n");	
		Log::log(0,"%s","获取空间失败");	
		return -1;
	}
	char sArgvTmp[1024] = {0};
	char pTmp[1024] = {0};
	for(int i=0;i<g_argc;i++)
	{

		strcat(sArgvTmp,g_argv[i]);
		strcat(sArgvTmp," ");
		 
	}
	
	char sData[1024] = {0};
	char datatri[128] = {0};
	try
	{
			   	  
		int iRes = m_poSysParam->resolveHBParam(g_argv,g_argc);
		
		if(iRes>0)
		{			
			sprintf(sData,"%s%s%s%s%s\0","用户[",pChr,"]执行[ ",sArgvTmp," ]命令处理成功");
		} else if(iRes<0){ 	
			sprintf(sData,"%s%s%s%s%s\0","用户[",pChr,"]执行[ ",sArgvTmp," ]命令处理失败");	
		}else{
			sprintf(sData,"%s%s%s%s%s\0","用户[",pChr,"]执行 ",sArgvTmp," ]命令处理结束");
		}
		
		Log::log(0,sData);
	} catch (...)
	{
		sprintf(sData,"%s%s%s%s%s\0","用户[",pChr,"]执行 ",sArgvTmp," ]命令处理异常结束");
		//
		sprintf(pTmp,"%s","命令处理异常结束!");
		TLog.log(MBC_COMMAND_ABMPARAM,LOG_LEVEL_WARNING,LOG_CLASS_INFO,LOG_TYPE_PARAM,-1,pTmp,sData);
		Log::log(0,sData);
	}

	
	return 0;
}
