#include "InitServer.h"
#include<signal.h>
#include <iostream>
#include "ReadIni.h"
#include "IniFileMgr.h"
#include "BaseRuleMgr.h"


#include <sys/types.h>
#include <sys/wait.h>
#include "Log.h"
#include "CSemaphore.h"
#include <memory>
using namespace std;


#include "ThreeLogGroupMgr.h"
#include "Env.h"

#include <assert.h>
DEFINE_MAIN(CInitServer)

CInitServer::CInitServer(void):BaseProcess(BaseProcess::USER)
{
}


CInitServer::~CInitServer(void)
{
}
bool CInitServer::GetStartProcess()
{
	
	if(m_strTIBSHOME_CfgFile.empty())
		return false;

	char sKeyValue[LINEMAXCHARS]={0};

	ReadIni oReadIni;
	oReadIni.readIniString(m_strTIBSHOME_CfgFile.c_str(),PROCESS_SECTION,PROCESS_START_NUM,sKeyValue,0);
	m_iProcessNum = atoi(sKeyValue);
	char sProcessName[256]={0};
	char sProcessParam[256]={0};
	char sProcessRunOnce[256]={0};
	char sProcessType[256]={0};
	for(int i=0;i<m_iProcessNum;i++)
	{
		memset(sProcessName,0,sizeof(sProcessName));
		memset(sProcessParam,0,sizeof(sProcessParam));
		sprintf(sProcessName,"%s_%d",PROCESS_NAME,i);
		sprintf(sProcessParam,"%s_%d",PROCESS_PARAM,i);
		sprintf(sProcessRunOnce,"%s_%d",PROCESS_RUN_ONCE,i);
		sprintf(sProcessType,"%s_%d",PROCESS_TYPE,i);
		if(!oReadIni.readIniString(m_strTIBSHOME_CfgFile.c_str(),
			PROCESS_SECTION,sProcessName,sKeyValue,0))
		{
			continue;
		}
		ProcessExecInfo oExecInfo;
                memset(&oExecInfo,0,sizeof(ProcessExecInfo));
		string strProcessName=sKeyValue;
		string strProcessParam;
		if(oReadIni.readIniString(m_strTIBSHOME_CfgFile.c_str(),
			PROCESS_SECTION,sProcessParam,sKeyValue,0))
			strProcessParam=sKeyValue;

		strProcessParam.erase(0,strProcessParam.find_first_not_of(' '));
		strProcessParam.erase(strProcessParam.find_last_not_of(' ')+1);
		memset(sKeyValue,0,sizeof(sKeyValue));
		if(oReadIni.readIniString(m_strTIBSHOME_CfgFile.c_str(),
			PROCESS_SECTION,sProcessType,sKeyValue,0))
		{
			oExecInfo.iProcType=atoi(sKeyValue);	
		}

		if(oReadIni.readIniString(m_strTIBSHOME_CfgFile.c_str(),
			PROCESS_SECTION,sProcessRunOnce,sKeyValue,0)){}
			
		char* pBillDir = getenv( "ABM_PRO_DIR" );
		if(0!=pBillDir)
			sprintf(oExecInfo.sProcessPath,"%s/bin/",pBillDir);
		sprintf(oExecInfo.sProcessName,"%s",strProcessName.c_str());
		sprintf(oExecInfo.sArgv[0],"%s",strProcessName.c_str());
		oExecInfo.iRunOnce = atoi(sKeyValue);
		oExecInfo.iProcessState=3;
		if(!strProcessParam.empty())
		{
			int iPos =0;
			int i =1;
			int iPosp=0;
			while( ((iPos=	strProcessParam.find(' ',iPos))>0)&&(i<3))
			{
				string strTemp;
				strTemp.append(strProcessParam.begin()+iPosp,strProcessParam.begin()+iPos);
				strTemp.erase(0,strTemp.find_first_not_of(' '));
				strTemp.erase(strTemp.find_last_not_of(' ')+1);
				if(!strTemp.empty())
				{
					strcpy(oExecInfo.sArgv[i++],strTemp.c_str());
				}
				iPosp=iPos++;
				
			}
			if(((iPosp>0)&&(i<3))||(!strProcessParam.empty()))
			{
				string strTemp;
				strTemp.append(strProcessParam.begin()+iPosp,strProcessParam.end());
				strTemp.erase(0,strTemp.find_first_not_of(' '));
				strTemp.erase(strTemp.find_last_not_of(' ')+1);
				if(!strTemp.empty())
				{
					strcpy(oExecInfo.sArgv[i++],strTemp.c_str());
				}
			}
		}
		m_strStartProcess.push_back(oExecInfo);
	}
	return true;
}
void CInitServer::MonitorProcess(void)
{
	vector<ProcessExecInfo>::iterator it = m_strStartProcess.begin();
	for(;it!= m_strStartProcess.end();it++)
	{
		if((*it).iProcessState==0)
			continue;
		waitpid((*it).iPid,NULL,WNOHANG);
		if(((*it).iPid!=0)&&((*it).iRunOnce==1))
			continue;	
		if(((*it).iPid==0)||((kill((*it).iPid, 0) == -1 && errno == ESRCH)))
		{
			Log::log(0,"%s","进程状态异常");
			if((*it).iProcessStartTimes<m_iRestartTimes)
			{
				char sSystem[128]={0};
			}
			if((*it).iProcessStartTimes>=m_iRestartTimes)
			{
				Log::log(0,"%s","进程重启次数超限");
				continue;
			}
			StartProcess((*it));
		}
	}
	if(0 ==m_iRunCnt)
	{
		SetServiceState(SERVICE_ACTIVE);
		if(0<m_iPid)
		{
			if(!(kill(m_iPid, 0) == -1 && errno == ESRCH))
			{
				sigqueue(m_iPid,SIGUSR1,m_unsigval);
				m_iRunCnt=1;
			}
		}	
	}
}


bool CInitServer::GetLogInfo(void)
{
	if(m_strTIBSHOME_CfgFile.empty())
		return false;

	char sKeyValue[LINEMAXCHARS]={0};

	ReadIni oReadIni;
	if(!oReadIni.readIniString(m_strTIBSHOME_CfgFile.c_str(),LOG_SECTION,LOG_PATH,sKeyValue,0))
	{
		Log::log(0,"核心参数没有找到,%s,%s,%s",LOG_SECTION,LOG_PATH,sKeyValue);
		return false;
	}
	m_strMsgLogFilePath=sKeyValue;


	
	if(!oReadIni.readIniString(m_strTIBSHOME_CfgFile.c_str(),MONITOR_SECTION,MONITOR_RESTART_TIMES,sKeyValue,0))
	{
		Log::log(0,"核心参数没有找到,%s,%s,%s",MONITOR_SECTION,MONITOR_RESTART_TIMES,sKeyValue);
	}
	m_iRestartTimes = atoi(sKeyValue);
	if(0==m_iRestartTimes)
		m_iRestartTimes=3;
	return true;
}
bool CInitServer::GetSHMInfo(void)
{
    if(m_strTIBSHOME_CfgFile.empty())
        return false;

    char sInitParamSize[LINEMAXCHARS]={0};
    char sKeyValue[256]={0};

    ReadIni oReadIni;
    //创建核心参数共享内存
    if(!oReadIni.readIniString(m_strTIBSHOME_CfgFile.c_str(),MEMORY_SECTION,
        MEMORY_PARAM_NAME,sKeyValue, 0))
    {
        Log::log(0,"核心参数文件读取失败:%s,%s,%s",m_strTIBSHOME_CfgFile.c_str(),
            MEMORY_SECTION,MEMORY_PARAM_NAME);
        return false;
    }
    m_strInitShmName = sKeyValue;

    if(!oReadIni.readIniString(m_strTIBSHOME_CfgFile.c_str(),MEMORY_SECTION,
        MEMORY_INIT_SIZE,sInitParamSize,0))
    {
        Log::log(0,"核心参数文件读取失败:%s,%s,%s",m_strTIBSHOME_CfgFile.c_str(),
            MEMORY_SECTION,MEMORY_INIT_SIZE);
        return false;
    }
    m_lInitShmSize=atol(sInitParamSize);	
	return true;
}
void CInitServer::ReplaceIniKernelParam(stIniCompKey &oIniCompKey)
{
	if(strcasecmp(oIniCompKey.sSectionName,MEMORY_SECTION)==0)
	{
		if(strcasecmp(oIniCompKey.sKeyName,MEMORY_PARAM_NAME)==0)
		{
			m_strInitShmName = oIniCompKey.sKeyValue[0];
			return;
		}
		if(strcasecmp(oIniCompKey.sKeyName,MEMORY_INIT_SIZE)==0)
		{
			m_lInitShmSize = atol(oIniCompKey.sKeyValue[0]);
			return;
		}
	}
	return ;
}

void CInitServer::CheckParam(vector<stIniCompKey>& vtCompKey,
								   vector<stIniCompKey>& vtBadCompKey,
								   vector<stIniCompKey>& vtRtCompKey)

{
	//检查参数规则
	BaseRuleMgr* pRuleMgr = new BaseRuleMgr();
	vector<stIniCompKey>::iterator it = vtCompKey.begin();
	for( ; it!=vtCompKey.end();it++)
	{
		if((*it).flag ==OP_DEL)
		{
			//需要删除的判断是否是必须的
			if(0>pRuleMgr->checkParamNeed((*it).sSectionName,(*it).sKeyName))
			{
				vtBadCompKey.push_back(*it);
			}else
			{
				string strTmp;
				strTmp.append((*it).sSectionName);
				strTmp.append(".");
				strTmp.append((*it).sKeyName);
				strTmp.append("=");
				strTmp.append((*it).sKeyValue[0]);
				cout<<"Check It:"<<strTmp<<endl;
				if(0<pRuleMgr->check((*it).sSectionName,(*it).sKeyName,(*it).sKeyValue[0],0,0,false))
				{
					vtBadCompKey.push_back(*it);
				}
				else
				{
					vtRtCompKey.push_back(*it);
				}
			}
			continue;
		}
		if(((*it).flag == OP_NEW)||((*it).flag== OP_FIX))
		{
			//判断规则是否适用
			string strTmp;
				strTmp.append((*it).sSectionName);
				strTmp.append(".");
				strTmp.append((*it).sKeyName);
				strTmp.append("=");
				strTmp.append((*it).sKeyValue[0]);
				cout<<"Check It:"<<strTmp<<endl;
				if(0<pRuleMgr->check((*it).sSectionName,(*it).sKeyName,(*it).sKeyValue[0],(*it).sEffDate,0,false))
				{
					vtBadCompKey.push_back(*it);
				}else
					vtRtCompKey.push_back(*it);
			continue;
		}
	}
	//delete pRuleMgr; pRuleMgr=0;
}

bool CInitServer::ServiceExit(pid_t iPid)
{
	sleep(4);
	return true;
}


bool CInitServer::Init(void)
{
    //先创建日志组共享内存
    ThreeLogGroupMgr *pThreeLogGroupMgr =new ThreeLogGroupMgr();
    if((!pThreeLogGroupMgr->init())||(!pThreeLogGroupMgr->create())){
        Log::log(0,"日志组初始化失败");
        return false;
    }
    delete pThreeLogGroupMgr;
    pThreeLogGroupMgr=0; 

    char* p=NULL;
    string strConfig=CONFIG_FILE;
	string strKey;
	unsigned int iSize=0;

	char sKeyValue[256]={0};
	char sKeyLockKey[256]={0};
	char sInitParamSize[32]={0};

	auto_ptr<CIniFileMgr> pIniFileMgr(new CIniFileMgr());
	auto_ptr<CIniFileMgr> pIniFileMgrStd(new CIniFileMgr());


    m_strTIBSHOME_Path=BaseProcess::m_sAPP_HOME;

	if(m_strTIBSHOME_Path[m_strTIBSHOME_Path.length()]!='/')
		m_strTIBSHOME_CfgFile =m_strTIBSHOME_Path+"/";
	else
		m_strTIBSHOME_CfgFile=m_strTIBSHOME_Path;	
    m_strTIBSHOME_CfgFile +="etc/";
	m_strTIBSHOME_CfgFile += strConfig;
	m_strTIBSHOME_CfgStdFile = m_strTIBSHOME_CfgFile+".std";

	if(-1== access(m_strTIBSHOME_CfgStdFile.c_str(),R_OK|F_OK)){
        char sLogMsg[256] = {0};
        sprintf(sLogMsg,"文件不存在或者文件不可读写, 请检查文件 :%s",m_strTIBSHOME_CfgStdFile.c_str());
		Log::log(0,"%s",sLogMsg);
		return false;
	}
	//判断文件是否存在且可读
	if (-1 ==access( m_strTIBSHOME_CfgFile.c_str(), R_OK|  F_OK ) )
	{
        char sLogMsg[256] = {0};
        sprintf(sLogMsg,"文件不存在或者文件不可读写, 请检查文件 :%s",m_strTIBSHOME_CfgFile.c_str());
		Log::log(0,"%s",sLogMsg);
		return false;
	}
		
	//读取必要的信息
	if(!GetSHMInfo()||!GetLogInfo()){
		return false;
	}
	

    vector<stIniCompKey> vtBadIniKey;
    vector<stIniCompKey> vtRetIniKey;
    bool bShm=false;
    if(!pIniFileMgr->GetDataFromShm(m_strInitShmName.c_str())){
        //共享内存不存在
        if(!pIniFileMgr->LoadIniFromFile(m_strTIBSHOME_CfgFile.c_str())){
            //装载配置文件失败
            return false;
        }
        //装载标准文件
        if(!m_strTIBSHOME_CfgStdFile.empty()){	
            if(!pIniFileMgrStd->LoadIniFromFile(
                m_strTIBSHOME_CfgStdFile.c_str())){
                    m_strTIBSHOME_CfgStdFile.clear();
            }
        }
    }else{
        bShm=true;
        if(!pIniFileMgrStd->LoadIniFromFile(
            m_strTIBSHOME_CfgFile.c_str())){
                m_strTIBSHOME_CfgFile.clear();
        }
    }

    //比较和标准文件的差异
    vector<stIniCompKey> vctIniKey;
    if(pIniFileMgrStd->Compare(pIniFileMgr.get(),vctIniKey))
    {
        //和标准文件不一样，需要检查规则
        CheckParam(vctIniKey,vtBadIniKey,vtRetIniKey);
        assert(vctIniKey.size()==(vtBadIniKey.size()+vtRetIniKey.size()));
        //提示这些无法处理
        vector<stIniCompKey>::iterator it = vtBadIniKey.begin();
        for( ;it!= vtBadIniKey.end();it++)
        {
            Log::log(0,"参数规则校验失败文件名:%s,Section:%s,Key:%s,Value:%s",
                m_strTIBSHOME_CfgFile.c_str(),(*it).sSectionName,(*it).sKeyName,(*it).sKeyValue);
            Log::log(0,"参数不符合规则:SectionName:%s ParamName:%s,Value:%s",(*it).sSectionName,(*it).sKeyName,(*it).sKeyValue);  

            stIniCompKey oIniCompKey;
            if(pIniFileMgrStd->GetIniKey((*it).sSectionName,(*it).sKeyName,oIniCompKey))
            {
                oIniCompKey.flag=OP_FIX;
                vtRetIniKey.push_back(oIniCompKey);
                pIniFileMgr->ReplaceKey(oIniCompKey);
                pIniFileMgr->SaveKey(m_strTIBSHOME_CfgFile.c_str(),oIniCompKey);
            }
        }
        for(vector<stIniCompKey>::iterator it= vtRetIniKey.begin();
            it!= vtRetIniKey.end();it++)
        {
            ReplaceIniKernelParam(*it);
        }
    }
    if(!bShm){
        try{
            if(!pIniFileMgr->CopyToShm(m_strInitShmName.c_str(), m_lInitShmSize))
            {
                return false;
            }
        }
        catch (Exception &e) {
            Log::log(0,"%s","内存分配失败");
            exit(0);
        }
    }
    return true;
}


bool CInitServer::CheckProcessIsDown(pid_t iPid,int iSecs)
{
	while(iSecs-->0)
	{
		if(!(kill(iPid, 0) == -1 && errno == ESRCH))
		{
		}else
		{
			sleep(1);
		}
	}
	return true;
}


int CInitServer::run()
{
	int iPid=0;
	if(g_argc>1)
		iPid= atoi(g_argv[1]);
	m_iPid=iPid;
	
	/*初始化*/
	if(!Init())
	{
		SetServiceState(SERVICE_ERROR);
		if(!(kill(iPid, 0) == -1 && errno == ESRCH))
		{
			sigqueue(iPid,SIGUSR1,m_unsigval);
		}
		Log::log(0,"初始化参数失败");
		return 0;
	}	
	SetServiceState(SERVICE_PROCESS_RUN);
	while(!GetInterruptFlag())
	{
		OnIdle();	
	}
	return 0;	
}
void CInitServer::ModifyTimingParameters(void)
{
	auto_ptr<CIniFileMgr> pIniFileMgr(new CIniFileMgr());
	pIniFileMgr->Init(m_strTIBSHOME_CfgFile.c_str());
	if(!pIniFileMgr->GetDataFromShm(m_strInitShmName.c_str())){
		Log::log(0,"Attach SHM Failed :%s ",m_strInitShmName.c_str());
		return ;
	}
	SysParamHis* pParamHis=0;
	int iCnt=0;
	bool bRet = pIniFileMgr->getAllSection(&pParamHis,iCnt);
	if(!bRet || iCnt<=0){
		Log::log(0,"%s","读取共享内存失败");
	}
	static map<string,string> mapChange;
	Date d1,d2;
	for(int i=0;i<iCnt;i++){
		if(pParamHis[i].sDate[0]!=0){
				string strKey=pParamHis[i].sSectionName;
				strKey+=".";
				strKey+=pParamHis[i].sParamName;
				strKey+="=";
				strKey+=pParamHis[i].sParamValue;
			map<string,string>::iterator it = mapChange.find(strKey);
			if((it!=mapChange.end())&&(it->second.compare(pParamHis[i].sDate)==0))
				continue;
			d2.parse(pParamHis[i].sDate);
			if(d2<=d1){
				Log::log(0,"%s",strKey.c_str());
				pIniFileMgr->changeSectionKey(strKey);
				mapChange[strKey]=pParamHis[i].sDate;
			}
		}
	}
	sleep(1);
	if(0!=pParamHis) delete[] pParamHis;
	return ;
}
void CInitServer::OnIdle()
{
	usleep(1000);
	MonitorProcess();
	ModifyTimingParameters();
}

bool CInitServer::StartProcess(ProcessExecInfo& pProcessInfo)
{
	if((pProcessInfo.iPid==0)||((kill(pProcessInfo.iPid, 0) == -1 && errno == ESRCH)))
	{
		//进程不存在
		//shell只执行一边
		if((pProcessInfo.iProcType==1)&&(pProcessInfo.iPid!=0))
			{
				return true;
			}
		if(pProcessInfo.iProcessStartTimes>=m_iRestartTimes)
		{
					Log::log(0,"%s","进程重启次数超限");
					false;;
		}
		pProcessInfo.iProcessStartTimes++;
		 pid_t iPid ;
		  switch(iPid=fork())
        {
                case 0:
                {
                sleep(10);
                //启动进程
                string strProcess=pProcessInfo.sProcessPath;
                strProcess.append(pProcessInfo.sProcessName);
                if(pProcessInfo.iProcType==1)
                	{
                		execlp("/bin/sh","sh","-c",pProcessInfo.sArgv[0],NULL);
                	}else
                		{
                			if(execl(strProcess.c_str(),pProcessInfo.sArgv[0],pProcessInfo.sArgv[1],NULL)<0)
                				{
                					Log::log(0,"%s","进程创建失败");
                					switch(errno)
                        {
                        			
                                case ENOENT:
                                        Log::log(0,"COMMAND OR FILENAME NOT FOUND");
                                        break;
                                case EACCES:
                                        Log::log(0,"YOU DO NOT HAVE RIGHT TO ACCESS");
                                        break;
                                default:
                                        Log::log(0,"SOME ERROR HAPPENED IN EXEC\n");

                        }
                        exit(0);
                				}
                		}
              }break;
              case -1:
              	{
              		pProcessInfo.iProcessState=0;
              	}break;
              	default:
              		{
              			if(pProcessInfo.iProcType==1)
              				{
              					wait(0);
              				}
              				pProcessInfo.iPid=iPid;
              		}break;
              	}
	}	
	return true;
}


void CInitServer::SetServiceState(SERVICE_STATE STATE)
{
	m_State = STATE;
	m_unsigval.sival_int=m_State;
}


