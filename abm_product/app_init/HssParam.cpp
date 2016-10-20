#include "SysParamClient.h"
#include "MBC.h"
#include "CommandCom.h"
#include "GroupDroitMgr.h"
#include "Log.h"
#define	HBPARAM_WAIT_TIME_NEED	100

int main(int argc, char* argv[])
{
    int iFdFront = 0;
    if( argc>=3 && strcmp("-adm", argv[argc-2])==0 ){
    
        iFdFront = atoi(argv[argc-1]);
        argc-=2;
        if(iFdFront<0) iFdFront = 0;
        if( iFdFront){
            if( dup2(iFdFront, STDOUT_FILENO )!= STDOUT_FILENO){
                printf("初始化前台通讯失败!\n");
            }else{
                setvbuf(stdout,NULL,_IOLBF,0);
            }
        }
    }

	GroupDroitMgr *pGroupDroitCtl = new GroupDroitMgr();
	char *pChr = pGroupDroitCtl->getUsrName();
	CommandCom *m_poCmdCom = new CommandCom();
	
	if(m_poCmdCom) 
	{	
		if(!m_poCmdCom->InitClient())
		{			
			printf("注册失败,请检查hbparam以及initserver!\n");	
			Log::log(0,"%s","向initserver注册失败");	
			return -1;
		}
	} else {
		printf("获取空间失败!\n");	
		Log::log(0,"%s","获取空间失败");	
		return -1;
	}
	//m_poCmdCom->GetInitParam("LOG");
	//m_poCmdCom->SubscribeCmd("LOG",MSG_PARAM_CHANGE);
	SysParamClient *m_poSysParam = new SysParamClient();
	if(m_poSysParam)
	{   	
		m_poSysParam->init(m_poCmdCom,HBPARAM_WAIT_TIME_NEED);	
	} else {
		printf("获取空间失败!\n");	
		Log::log(0,"%s","获取空间失败");	
		return -1;
	}
	char sArgvTmp[2048] = {0};
	for(int i=0;i<argc;i++)
	{
		strcat(sArgvTmp,argv[i]);
		strcat(sArgvTmp," ");
	}
	try
	{
			   	  
		int iRes = m_poSysParam->resolveHBParam(argv,argc);
		char sData[1024] = {0};
		if(iRes>0)
		{			
			sprintf(sData,"%s%s%s%s%s\0","[ADMIN]用户[",pChr,"]执行[ ",sArgvTmp," ]命令成功");
		} else if(iRes<0){ 	
			sprintf(sData,"%s%s%s%s%s\0","[ADMIN]用户[",pChr,"]执行[ ",sArgvTmp," ]命令失败");
		}else{
			sprintf(sData,"%s%s%s%s%s\0","[ADMIN]用户[",pChr,"]执行 ",sArgvTmp," ]命令结束");
		}
		printf("%s\n",sData);
		//return 0;
	} catch (...)
	{
		char sData[1024] = {0};
		sprintf(sData,"%s%s%s%s%s\0","[ADMIN]用户[",pChr,"]执行 ",sArgvTmp," ]命令异常结束");
		printf("%s\n",sData);
	}
	if(iFdFront){
        printf("<END>\n");
    }
	
	return 0;
}
