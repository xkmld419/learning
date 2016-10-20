#include "SgwParam.h"
DEFINE_COMMAND_MAIN(SgwParam)
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
    /*int iFdFront = 0;
	int srcgc = g_argc;
    if( g_argc>=3 && strcmp("-adm", g_argv[g_argc-2])==0 ){
    
        iFdFront = atoi(g_argv[g_argc-1]);
        //g_argc-=2;
        if(iFdFront<0) iFdFront = 0;
        if( iFdFront){
            if( dup2(iFdFront, STDOUT_FILENO )!= STDOUT_FILENO){
                printf("初始化前台通讯失败!\n");
            }else{
                setvbuf(stdout,NULL,_IOLBF,0);
            }
        }
    }*/

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
	//m_poCmdCom->GetInitParam("LOG");
	//m_poCmdCom->SubscribeCmd("LOG",MSG_PARAM_CHANGE);
	SysParamClient *m_poSysParam = new SysParamClient();
	if(m_poSysParam)
	{   	
		if(g_iFdFront)
			m_poSysParam->setFrontInfo(true,g_iStart,g_iEnd,"|");
		else
			m_poSysParam->setFrontInfo(false,g_iStart,g_iEnd,"|");
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
		/*if(i > 1){
			strcat(pTmp,g_argv[i]);
		}*/
		strcat(sArgvTmp,g_argv[i]);
		strcat(sArgvTmp," ");
		 
	}
	
	char sData[1024] = {0};
	char datatri[128] = {0};
	try
	{
			   	  
		int iRes = m_poSysParam->resolveHBParam(g_argv,g_argc);
		/*
		switch(m_poSysParam->m_lSrcCMDID)
		{
			case MSG_PARAM_CHANGE:
			{
				 if(m_poSysParam->m_iCnt == 1)
				 {
				 	sprintf(pTmp,"%s.%s",m_poSysParam->m_poSysParamHis->sSectionName,m_poSysParam->m_poSysParamHis->sParamName);
					sprintf(datatri,"%s%s%s%s"," 原值:",m_poSysParam->m_poSysParamHis->sParamOrValue,"  新值:",m_poSysParam->m_poSysParamHis->sParamValue);
				 }  else {
				 	sprintf(datatri,"%s","修改失败");
				 }
				 break;
			}
			case MSG_PARAM_ACTIVE_MOUDLE_S:
			{
				 if(m_poSysParam->m_iCnt > 0)
				 {				 	
					sprintf(datatri,"%s%d%s","有",m_poSysParam->m_iCnt,"参数刷新失败!");
				 }  else {
				 	if(m_poSysParam->m_iCntEx > 0)
				 	{
						sprintf(datatri,"%s%d%s","有",m_poSysParam->m_iCntEx,"个参数值发送改变,刷新正常结束!");
					} else {
				 		sprintf(datatri,"%s","刷新正常结束!");
					}
				 }
				 break;
			}
		}
		*/
		if(iRes>0)
		{			
			sprintf(sData,"%s%s%s%s%s\0","用户[",pChr,"]执行[ ",sArgvTmp," ]命令处理成功");
		} else if(iRes<0){ 	
			sprintf(sData,"%s%s%s%s%s\0","用户[",pChr,"]执行[ ",sArgvTmp," ]命令处理失败");	
		}else{
			sprintf(sData,"%s%s%s%s%s\0","用户[",pChr,"]执行 ",sArgvTmp," ]命令处理结束");
		}
		//if(strlen(datatri)>0)
		//{
		//	strcat(sData,datatri);
		//}
		/*switch(m_poSysParam->m_lSrcCMDID)
		{
			case MSG_PARAM_ACTIVE_MOUDLE_S:
			{
				if(m_poSysParam->m_iCnt > 0)
				{
					TLog.log(MBC_COMMAND_SGWPARAM,LOG_LEVEL_WARNING,LOG_CLASS_INFO,LOG_TYPE_PARAM,-1,pTmp,sData);
				} else {
					TLog.log(MBC_COMMAND_SGWPARAM,LOG_LEVEL_INFO,LOG_CLASS_INFO,LOG_TYPE_PARAM,-1,pTmp,sData);
				}
				break;
			}
		}*/
		//TLog.log(MBC_COMMAND_SGWPARAM,LOG_LEVEL_INFO,LOG_CLASS_INFO,LOG_TYPE_PARAM,-1,pTmp,sData);
		//if(!g_iFdFront){
		Log::log(0,sData);
	} catch (...)
	{
		sprintf(sData,"%s%s%s%s%s\0","用户[",pChr,"]执行 ",sArgvTmp," ]命令处理异常结束");
		//
		sprintf(pTmp,"%s","命令处理异常结束!");
		TLog.log(MBC_COMMAND_SGWPARAM,LOG_LEVEL_WARNING,LOG_CLASS_INFO,LOG_TYPE_PARAM,-1,pTmp,sData);
		Log::log(0,sData);
	}

	
	return 0;
}
