#include "SysParamClient.h"
#include "Exception.h"
#include "MBC.h"
#include "Date.h"
#include "ParamDefineMgr.h"
#include "ThreeLogGroup.h"
#include "MBC_ABM.h"


#define SHOW_RULE	9 //显示规则
#define EFFDATE     "effdate"

SysParamClient::SysParamClient()
{ 
	m_poCmdCom = 0;
	m_iWaitMax = 0;
	m_bMONDebug = false;
	m_bLogOut = true;
	m_poBaseRuleMgr = 0;
	//Log::init(508);

    m_bFront=false;
    m_iBegin=0;
    m_iEnd=0;
    m_pSplit=0;
	m_poSysParamHis  = 0;
	m_poSysParamHisEx = 0;
	m_pSysParamGet = 0;
#ifdef _ARBITRATION_
	m_pArbitrationMgr= ArbitrationMgr::GetInstance();
	if(!m_pArbitrationMgr)
	{
			throw(MBC_Arbitration+1);
	}
	m_iState = m_pArbitrationMgr->getSystemState();		//获得主备系统
	if(m_iState <0)
	{
			throw(MBC_Arbitration+2);
	}
	
    char instanceID[10]={0};
	sprintf(instanceID,"%d",1000);//由于仲裁平台初始化要传入模块名、实例名。自己定义实例名：1000
	int iTemp=m_pArbitrationMgr->Init("hbparam",instanceID); 	
	if(iTemp !=0)
	{
		Log::log(0,"仲裁平台初始化不成功！");
		throw(MBC_Arbitration+3);
		return ;
	}
	DenyInterrupt_A();
#endif
}

SysParamClient::SysParamClient(int iMode)
{
}

SysParamClient::~SysParamClient()
{
	if(m_poBaseRuleMgr){
		delete m_poBaseRuleMgr;
		m_poBaseRuleMgr = 0;
	}
}

/*
函数功能:初始化
参数说明:获得hbadmin或外围提供的CommandCom指针以及设置的最大等待时间(单位是百毫秒)
返回值说明:无
举例:init(pCmdCom,30)
*/ 
void SysParamClient::init(CommandCom *pCmdCom,int iWaitTime)
{
	 m_poCmdCom = pCmdCom;
	 m_iWaitMax = iWaitTime*100;
}

/*
函数功能:参数格式基本检测
参数说明:sParamDate 用户输入有效数据 , lCMD命令ID
返回值说明:true通过检测
举例:checkBaseFormat("SYSTEM.location=beijing,",MSG_PARAM_S_N);处理失败;"SYSTEM.location=beijing,"多了个';'
*/ 
bool SysParamClient::checkBaseFormat(char *sParamDate,long lCMD)
{
	 int iPos = 0;
	 string strParamDate(sParamDate);
	 if(lCMD == MSG_PARAM_S_N)
	 {
	 	if(sParamDate[0] == '\0')
			return false;
	    iPos = strParamDate.rfind("=");
	 	if(iPos>0)
			return false;
	 } else if(lCMD == MSG_PARAM_CHANGE)
	 {
	 	if(sParamDate[0] == '\0')
			return false;
	    iPos = strParamDate.rfind("=");
		if(iPos<=0)
			return false;
	 } else if(lCMD == MSG_PARAM_S_C)
	 {
	 	if(strParamDate.size()==0)
			return false;
	 	iPos = strParamDate.rfind('=');
		if(iPos<=0)
			return false;
	 } else if(lCMD == MSG_PARAM_S_D)
	 {
	 	if(strParamDate.size()==0)
			return false;
	 	/*iPos = strParamDate.rfind('=');
		if(iPos>0)
			return false;*/
	 }
	 return true;
}

/*
函数功能:解析用户输入数据,处理得到最终结果,显示给用户
参数说明:sArgv, iArgc 等同main函数的两个参数
返回值说明:>0解析成功;=0无需继续处理;<0解析失败
举例:sArgv = {"hbparam","-list","SYSTEM"} iArgc=3
*/ 
int SysParamClient::resolveHBParam(char **Argv,int Argc)
{
	char sArgv[ARG_NUM][ARG_VAL_LEN];
	for(int i=0;i<Argc;i++)
	{
		memset(sArgv[i],'\0',strlen(sArgv[i]));
		strcpy(sArgv[i],Argv[i]);
	}
	
	return resolveMemParam(sArgv,Argc);
}

/*
函数功能:解析用户输入数据,处理得到最终结果,显示给用户
参数说明:sArgv, iArgc 等同main函数的两个参数
返回值说明:>0解析成功;=0无需继续处理;<0解析失败
举例:sArgv = {"hbparam","-list","SYSTEM"} iArgc=3
*/ 
int SysParamClient::resolveMemParam(char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc)
{
    int iRes = resolveUsrParam(sArgv,iArgc);
    if(iRes>0)
    {
        char sTmpName[ARG_VAL_LEN] = {0};
        analyseParamName(m_sParamsValue,sTmpName);
        iRes = dealMem(sTmpName,m_sParamsValue,m_lSrcCMDID,true);
        if(iRes>0)
        {
			
            if(showMemMsgValues()<0)
            {
                return -1;
            }
			return 1;
        } else {
			if(m_lSrcCMDID == MSG_PARAM_ACTIVE_MOUDLE_S)
			{
				return	showMemMsgValues();
			}
        	return -1;
		}
    } else if(iRes == 0)
    {
        return 1;
    }
    return -1; 
}

/*
函数功能:解析用户输入数据
参数说明:sArgv, iArgc 等同main函数的两个参数
返回值说明:>0解析成功;=0无需继续处理;<0解析失败
举例:sArgv = {"hbparam","-list","SYSTEM"} iArgc=3
*/ 
int SysParamClient::resolveUsrParam(char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc)
{
	int res = 0;
	reSetBaseParams(); //数据重置	
	m_iArgc = iArgc; //if((strcmp(sArgv[0],"hbparam") == 0) || (strcmp(sArgv[0],"hbinitserver") == 0)) 	
	
	char sFlag[128] = {0};
	strncpy(sFlag,sArgv[1],sizeof(sFlag));
	//Chang2Lower(sFlag);
	
	int i = 1;
	if(strcmp(sFlag,"-set")==0)
	{
	    m_bSetParams = true;
		m_sParamsValue[0] = '\0';
		i++;
		if(i<iArgc)
		{
            strcpy(m_sParamsValue,sArgv[i]);
        }
	} 
	else if(strcmp(sFlag,"-show")==0)
	{
		//m_bListAllParams = true;
		m_bListParams = true;
		m_sParamsValue[0] = '\0';
    }						
	else if(strcmp(sFlag,"-kill") == 0) 
	{ 	//这个命令是为hbinitserver做的
		if(strcmp(sArgv[0],"hbinitserver") == 0)
	    {					
			m_lSrcCMDID = MSG_PROCESS_STOP_SIG;
		}
	}
	else if(strcmp(sFlag,"-list") == 0)
	{
		m_bListParams = true;
		m_sParamsValue[0] = '\0';
		i++;
		if (i<iArgc)
		{
            strcpy (m_sParamsValue, sArgv[i]); //多个参数设置
        }
	} 
	else if(strcmp(sFlag,"-listall") == 0)
	{
		m_bListAllParams = true;
    }    	
    else if(strcmp(sFlag,"-ls")==0)
	{
		m_bListParams=true;
		m_sParamsValue[0]=0;

	}
	else if(strcmp(sFlag,"-create") == 0)
	{
		m_bCreateParams = true;
		m_sParamsValue[0] = '\0';
		i++;
		if (i<iArgc){
            strcpy (m_sParamsValue, sArgv[i]); //多个参数设置
        }
    }   	
	else if(strcmp(sFlag,"-delete") == 0)
	{
		m_bDeleteParams = true;
		i++;
		if (i<iArgc){
			m_sParamsValue[0] = '\0';
            strcpy (m_sParamsValue, sArgv[i]); //多个参数设置
		}
	}    	
    else if(strcmp(sFlag,"-refresh") == 0)
    {
		m_bRefreshParams = true;
		i++;
		if (i<iArgc)
		{
				m_sParamsValue[0] = '\0';
                strcpy (m_sParamsValue, sArgv[i]); //多个参数设置
		}
	}				
	else if(strcmp(sFlag,"-rule")==0)
	{
		string strval;
		for(int j=2;j<iArgc;j++)
		{
			if(sArgv[j][0] != '\0')
			strval += sArgv[j];
		}
		if(!m_poBaseRuleMgr)
			m_poBaseRuleMgr = new BaseRuleMgr();
		m_poBaseRuleMgr->setDebugState(m_bMONDebug);
		m_poBaseRuleMgr->paramParseExplain(strval.c_str());
		return 0;
	}else {
		Log::log(0,"参数中有此命令[%s%s]不支持的选项",sArgv[0],sArgv[1]);
	    showHowToUseCMD();
	    return 0;
	}
	if(m_iArgc>4)
	{	
		//
		if(m_bSetParams && strcmp(sArgv[3],"=")==0)
		{
			//for(int j=4;j<iArgc;j++)
			//{
				strcat(m_sParamsValue,sArgv[3]);
				strcat(m_sParamsValue,sArgv[4]);
			//}
			
		}
		//
		//setFrontInfo(true);
	}
	if(m_bListAllParams)
	{
		m_lSrcCMDID = MSG_PARAM_S_ALL;
	} else if(m_bSetParams){ //-set
		if(m_sParamsValue[0] != '\0' ) // hbparam -set name1=vaue1,effdate=201010101010
		{
			m_lSrcCMDID = MSG_PARAM_CHANGE; 
		} else {
			m_lSrcCMDID = -1;
			Log::log(0,"参数中有此命令[%s%s]缺少的必选项",sArgv[0],sArgv[1]);
			return -1;
		}
	} else 	if(m_bListParams){
		if(m_sParamsValue[0] != '\0' ) // hbparam -list SectionName.name
		{ 
			m_lSrcCMDID = MSG_PARAM_S_N; //这个值到MSG.H设置
		} else {
		    m_lSrcCMDID = MSG_PARAM_S_ALL;//MSG_PARAM_S_ALL;//MSG_LIST_SECTION;
				
		}
	} else if(m_bCreateParams){
		if(m_sParamsValue[0] != '\0' ) // hbparam -create SectionName.ParamName1=value
		{ 
			m_lSrcCMDID = MSG_PARAM_S_C; //这个值到MSG.H设置
		} else {
			m_lSrcCMDID = -1;
			Log::log(0,"参数中有此命令[%s%s]缺少的必选项",sArgv[0],sArgv[1]);
			return -1;
		}
	} else if(m_bDeleteParams)
	{
		if(m_sParamsValue[0] != '\0' ){ // hbparam -delete SectionName.ParamName
			m_lSrcCMDID = MSG_PARAM_S_D; //这个值到MSG.H设置
		} else {
			m_lSrcCMDID = -1;
			Log::log(0,"参数中有此命令[%s%s]缺少的必选项",sArgv[0],sArgv[1]);
			return -1;
		}
	} else if(m_bRefreshParams){ // hbparam -refresh -m[-s]				
		if(m_sParamsValue[0] != '\0' )
		{ 
			//Chang2Lower(m_sParamsValue);
			if(strcmp(m_sParamsValue,"-s")==0)
			{
				m_lSrcCMDID = MSG_PARAM_ACTIVE_MOUDLE_S; 
			} /*else if(strcmp(m_sParamsValue,"-m")==0)
			{
				m_lSrcCMDID = MSG_PARAM_ACTIVE_MOUDLE_M; 
			}*/
		} else {
			m_lSrcCMDID = MSG_PARAM_ACTIVE_MOUDLE_S; //默认是内存刷到文件
		}
	} else {
		Log::log(0,"参数中有此命令[%s%s]不支持选项",sArgv[0],sArgv[1]);
		showHowToUseCMD();
		return -1;
	}
	//先检测一下参数基本格式
	/*if(!checkBaseFormat(sArgv[2],m_lSrcCMDID))
	{
			Log::log(0,"命令[%s%s]错误取值",sArgv[0],sArgv[1]);
			printf("参数[%s]格式错误\n",sArgv[2]);
			return -1;
	}*/
    /*if(!checkFrontSet(sArgv,iArgc))
	{
		return -1;
	}*/
	return 1;
}

bool SysParamClient::checkFrontSet(char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc)
{
	 if(m_iArgc<8)
	 {	
		//
		return true;
		//
		//setFrontInfo(true);
	 }
	 //如果是-set
	 if(m_bSetParams)
	 {
	 	//
		if(strcmp(sArgv[3],"=")==0)
		{
			//分段的那种 abmparam -set system.desc = 123这样的
			return getFrontBeginEnd(5,7,sArgv,iArgc);
		} else {
			//分段的那种 abmparam -set system.desc=123 -start 1 -end 2 -adm 1 这样的
			return getFrontBeginEnd(3,5,sArgv,iArgc);
		}
	 } else if(m_bRefreshParams) {
	 	//abmparam -refresh -adm 1 这样的
	 	if(strcmp(sArgv[2],"-adm")==0)
		{
			m_bFront = true;
		}
	 } else if(m_bListParams)
	 {
	 	//abmparam -list system.desc -start 1 -end 2 -adm 1 这样的
		return getFrontBeginEnd(3,5,sArgv,iArgc);
	 } else if(m_bListAllParams)
	 {
	 	//abmparam -list -start 1 -end 2 -adm 1 这样的
		return getFrontBeginEnd(2,4,sArgv,iArgc);
	 }
	 return true;
}

bool SysParamClient::getFrontBeginEnd(int begineOffset, int endOffset,char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc)
{
			m_bFront = false;
			if(strcmp(sArgv[begineOffset],"-start")==0)
			{
				for(int i=0;i<strlen(sArgv[begineOffset+1]);i++)
				{
					if(i==0 && sArgv[begineOffset+1][0] == '-')
						i++;
					if(!isdigit(sArgv[begineOffset+1][i]))
						return false;
				}
				m_iBegin = atoi(sArgv[begineOffset+1]);
			} else {
				return false;
			}
			if(strcmp(sArgv[endOffset],"-end")==0)
			{
				for(int i=0;i<strlen(sArgv[endOffset+1]);i++)
				{
					if(i==0 && sArgv[endOffset+1][0] == '-')
						i++;
					if(!isdigit(sArgv[endOffset+1][i]))
						return false;
				}
				m_iBegin = atoi(sArgv[endOffset+1]);
			} else {
				return false;
			}
			m_bFront = true;
			return true;
}

/*
函数功能:参数重置
*/ 
void SysParamClient::reSetBaseParams()
{
	m_bSetParams = false;
	m_bRefreshParams = false;
	m_bCreateParams = false;
	m_bDeleteParams = false;
	m_bListParams = false;
	m_bListAllParams = false;
	m_lSrcCMDID = 0;
	m_iArgc = 0;
	memset(m_sParamsValue,'\0',sizeof(m_sParamsValue));
	memset(m_sParamSetVal,'\0',sizeof(m_sParamSetVal));
	
	m_bShowHelp = false;
	clearPtr();
}

/*
函数说明:这个是为HSS做的参数形式转化
返回值:转化成功true 或 失败false
参数说明:原始值 value  , 转化后的值 strdesc
举例:bool res = change2hssString("SYSTEM.location=china,effdate=2019,SYSTEM.server_name=server1",str);
*/
bool SysParamClient::change2hssString(char *value,string &strdesc)
{
	vector<string> vec_str;
	getTagStringN(value,vec_str,',');
	if(vec_str.size()==0){
		return false;
	}
	int i = 0;
	for(vector<string>::iterator itr = vec_str.begin();itr!=vec_str.end();itr++)
	{
		trimN(*itr);
		if((*itr).size()==0)
		 	continue;
		if((*itr).size()>=7)
		{
		 	string tmp;
			tmp.assign((*itr),0,7);
			if(tmp.compare(EFFDATE)==0||tmp.compare("EFFDATE")==0)
			{
				if(itr == vec_str.begin())
					return false;
				//一致说明这个string应该
				strdesc += string(",");
				strdesc += (*itr);
			} else {
				if(itr == vec_str.begin()){
		 			strdesc += (*itr);
				} else {
					strdesc += string("+");
					strdesc += (*itr);
				}
			}
		} else {
		 	if(itr == vec_str.begin()){
		 		strdesc += (*itr);
			} else {
				strdesc += string("+");
				strdesc += (*itr);
			}
		}
	}
	return true;
}


/*
函数功能:检测参数格式以及数值是否符合规定
参数说明:sParamSetVal用户输入的有效数据,Show是否显示错误提示
返回值说明:true符合规则设定,false不合规定
举例:IfSuitRule(lCMDID,"SYSTEM.location=beijing",true)
*/ 
bool SysParamClient::IfSuitRule(long lCMDID,char *sParamSetVal,string &strDsc,bool Show)
{
    if(lCMDID<0) 
	{
		m_lSrcCMDID = 0;
		if(Show)
		    printf("%s\n","参数格式有误!");
		return false;
	}
    if(lCMDID != MSG_PARAM_CHANGE)
	{
		strDsc = sParamSetVal;
		return true;
	}
	if(!m_poBaseRuleMgr)
	{
		try 
		{
			m_poBaseRuleMgr = new BaseRuleMgr();
		} catch(...) {
			if(Show)
				printf("%s\n","参数规则配置有不合规范的,请检查!");
			Log::log(0,"参数[%s]错误取值",sParamSetVal);
			return false;
		}	
	}
		string strFrom;
		if(!change2hssString(sParamSetVal,strFrom))
		{
			if(Show)
				printf("%s\n","参数格式不符合规范,请检查!");
			return false;
		}
	string strUse;
	string strUse2;
	int iStartGet=0;
	int iStartFind = 0;
	vector<string> vecParams;
	while(1){
		int iDepart = strFrom.find('+', iStartFind);
		if(iDepart>0){
		    if( strFrom.c_str()[iDepart-1] != '\\' ){
    		    strUse.append(strFrom, iStartGet, iDepart-iStartGet);
    		    strUse2.clear();
    		    bool bParser = paramParser((char*)strUse.c_str(),strUse2);
    		    if(!bParser ){
    		        printf("参数[%s]格式解析错误", strUse.c_str() );	
    		        return false;
    		    }
    		    if(!m_poBaseRuleMgr->check((char*)strUse.c_str(),Show) ){
    		        m_poBaseRuleMgr->paramParseExplain(Show);
    		        return false;
    		    }
    		    if( iStartGet>0 ){ strDsc += ","; }
    		    strDsc += strUse2;
    		    
    		    string strParam(mRecord[0].sSectName);
    		    strParam.append(".");
    		    strParam.append(mRecord[0].sParamName);
    		    vecParams.push_back(strParam);

    		    iStartGet = iDepart+1;
    		    strUse.clear();
    		}else{
    		    if(iDepart-iStartGet-1>0)
    		        strUse.append(strFrom, iStartGet, iDepart-iStartGet-1);
    		    strUse.append("+");
    		    iStartGet = iDepart+1;
    		}
		    iStartFind = iDepart+1;
	    }else{
	        break;
	    }
	    
	}
 
	if( strFrom.size() > iStartGet){
	    strUse.append(strFrom, iStartGet, strFrom.size());
	}
	
    strUse2.clear();
    bool bParser = paramParser((char*)strUse.c_str(),strUse2);
    if(!bParser ){
        printf("参数[%s]格式解析错误", strUse.c_str() );	
        return false;
    }
    if(!m_poBaseRuleMgr->check((char*)strUse.c_str(),Show) ){
        m_poBaseRuleMgr->paramParseExplain(Show);
        return false;
    }
    if( vecParams.size() >0 ){ strDsc += ","; }
    strDsc += strUse2;
    
    string strParam(mRecord[0].sSectName);
    strParam.append(".");
    strParam.append(mRecord[0].sParamName);
    vecParams.push_back(strParam);

	//检测一下参数组规则
	bool resRule = m_poBaseRuleMgr->checkGroupParam(vecParams,Show);
	if(!resRule)
	{
		if(Show)
			printf("%s\n","GROUP检验失败!");
	}	
	return resRule;
}

//2.8最新版本
int SysParamClient::dealMem(char *sSectName, string sSysParamValue, long lCMDID,bool Show)
{ 
	if(lCMDID == MSG_PARAM_CHANGE)
		prompt(sSysParamValue,Show);
	string strDsc;
	bool bRes = IfSuitRule(lCMDID,(char*)sSysParamValue.c_str(),strDsc,Show);
	if(bRes)
	{
	 	return dealWithMem(sSectName, (char*)strDsc.c_str(), lCMDID, Show);
	} else {
		if(Show)
			printf("参数%s规则检效失败\n",strDsc.c_str());
		ThreeLogGroup TLog;
		char paramfullname[256] = {0};
		sprintf(paramfullname,"%s.%s",mRecord[0].sSectName,mRecord[0].sParamName);
		char woringinof[512] = {0};
		char username[32] = {0};
		FILE* fp = popen("whoami","r");
		if(fp)
		{
			fgets(username,32,fp);
		}
		pclose(fp);
		for(int i=0;i<strlen(username);i++)
		{
			if(username[i] == '\n')
				username[i] = '\0';
		}
		sprintf(woringinof,"用户%s执行abmparam -set修改参数的操作中 参数数据[%s]设置异常,不符合规则设定!",username,(char*)sSysParamValue.c_str());
		TLog.log(MBC_COMMAND_ABMPARAM,LOG_LEVEL_WARNING,LOG_CLASS_INFO,LOG_TYPE_PARAM,-1,paramfullname,woringinof);
		return -1;;
	}
}

/*
函数功能: 发送命令参数给initserver,得到处理结果
参数描述: sSectName段, sSysParamValue 参数值(段.参数=值,effdate=2011可无生效日期), lCMDID命令ID, ShowErrInfo是否显示报错信息
返回值: 失败返回值<0;超时0;获取到值为1
举例: dealWithMem("SYSTEM","SYSTEM.location=beijing",MSG_PARAM_S_N, true)
*/
int SysParamClient::dealWithMem(char *sSectName,char *sSysParamValue,long lCMDID,bool ShowErrInfo)
{
	m_iErrNo = 0;
	if(!m_poCmdCom){
		m_iErrNo = 1;
		return 0;
	}
	if(m_poSysParamHis)
		delete []m_poSysParamHis;
	m_poSysParamHis = 0;
	
	switch(lCMDID)
	{
		case MSG_PARAM_CHANGE:
		{
			if(!sSysParamValue || sSysParamValue[0] == '\0')
				return 0;
			string strParamVal(sSysParamValue);
			if(!m_poCmdCom->changeSectionKey(strParamVal))
			{
				m_iErrNo = 2;
				return 0;
			}
			string sect;
			string param;
			string full;
			string strvalue(sSysParamValue);
			int iRes = strvalue.find('.');
			int iResEnd = strvalue.rfind('=');
			int iResTmp=strvalue.rfind(".new_value");
			if(iResTmp>0){
				iResEnd=iResTmp;
			}
			if(iRes)
			{
				//
				sect.assign(strvalue,0,iRes);
				param.assign(strvalue,iRes+1,iResEnd-iRes-1);
			}
			full.assign(strvalue,0,iResEnd);
			//
						
			//dealWithMem((char*)sect.c_str(),(char*)full.c_str(),MSG_PARAM_S_N,ShowErrInfo);
			break;
		}
		case MSG_PARAM_S_ALL:
		{
			if(!m_poCmdCom->getSection(NULL,&m_poSysParamHis,m_iCnt))
			{
					return 0;
			}
			break;
		}
		case MSG_PARAM_ACTIVE_MOUDLE_S:
		{
			m_poSysParamHis = 0;
			m_iCnt = 0;
			/*if(!m_poCmdCom->FlashMemory(&m_poSysParamHis,m_iCnt))
			{
				m_iErrNo = 4;
				return 0;
			}*/
			if(!m_poCmdCom->FlashMemoryEx(&m_poSysParamHis,m_iCnt,&m_poSysParamHisEx,m_iCntEx))
			{
				m_iErrNo = 4;
				return 0;
			}
			break;
		}
		case MSG_PARAM_S_C:
		{
			if(!sSysParamValue || sSysParamValue[0] == '\0')
				return 0;
			string strParamVal(sSysParamValue);
			string createstr;
			createstr = "#create ";
			createstr += strParamVal;
			memset(mRecord,'\0',sizeof(ParamHisRecord));
			int pos = strParamVal.find('=');
			if(pos<=0)
				return 0;
			string tmpval;
			tmpval.assign(strParamVal,0,pos);
			int pos2 = tmpval.find('.');
			strncpy(mRecord[0].sSectName,(char*)tmpval.c_str(),pos2);
			string tmp;
			tmp.assign(tmpval,pos2+1,tmpval.size());
			strcpy(mRecord[0].sParamName,(char*)tmp.c_str());
			if(!m_poCmdCom->changeSectionKey(createstr));
				return 0;
			break;
		}
		case MSG_PARAM_S_D:
		{
			if(!sSysParamValue || sSysParamValue[0] == '\0')
				return 0;
			string strParamVal(sSysParamValue);
			string delstr;
			delstr = "#delete  ";
			delstr += strParamVal;
			memset(mRecord,'\0',sizeof(ParamHisRecord));
			int pos = strParamVal.find('.');
			strncpy(mRecord[0].sSectName,(char*)strParamVal.c_str(),pos);
			string tmp;
			tmp.assign(strParamVal,pos+1,strParamVal.size());
			strcpy(mRecord[0].sParamName,(char*)tmp.c_str());
			if(!m_poCmdCom->changeSectionKey(delstr));
				return 0;
			break;
		}
		/*case MSG_PARAM_ACTIVE_MOUDLE_M:
		{
			m_iErrNo = 0;
			return 1;
		}*/
		case MSG_LIST_SECTION:
		{
			m_poSysParamHis = 0;
			m_iCnt = 0;
			if(!m_poCmdCom->getAllSections(&m_poSysParamHis,m_iCnt))
			{
				return 0;
			}
			break;
		}
		case MSG_PARAM_S_N:
		{
			if(!sSysParamValue || sSysParamValue[0] == '\0')
				return 0;
			string strParamVal(sSysParamValue);
			int iRes = strParamVal.find('.');
			if(iRes>0)
			{
				string strTmp =strParamVal;
				strTmp.erase(0,strTmp.find_first_of('.')+1);
				trimN(strTmp);
				//-list SYSTEM.*等同 -list SYSTEM
				if(strTmp.size()>0 && strTmp.compare("*")==0)
				{
					if(!m_poCmdCom->getSection(sSectName,&m_poSysParamHis,m_iCnt))
					{
						return 0;
					}
				} else {
			//		m_poSysParamHis = new SysParamHis[1];
				//	if(!m_poCmdCom->getSectionParam(sSectName,strTmp.c_str(),*m_poSysParamHis))
					if(!m_poCmdCom->getSectionParams(sSectName,strTmp.c_str(),&m_poSysParamHis,m_iCnt))
					{
						return 0;
					}else {
						//if(m_poSysParamHis->sSectionName[0] == '\0'){
						if(m_iCnt<=0){
							if(ShowErrInfo)
								printf("参数[%s]查找失败!\n",sSysParamValue);
							return 0;
						}
					}
				}
			} else {				
				if(!m_poCmdCom->getSection(sSectName,&m_poSysParamHis,m_iCnt))
				{
					return 0;
				}
			}
			break;
		}
		default:
		{
			return 0;
			//break;
		}
	}
	return 1;
}


/*
函数：showMemMsgValues
功能：显示从共享内存获得的数据进行合理显示
参数描述：Show是否显示结果
举例:showMemMsgValues(true)
*/
long SysParamClient::showMemMsgValues()
{	
	 	switch(m_lSrcCMDID)
		{
			case MSG_PARAM_S_C:
	 		case MSG_PARAM_CHANGE:
			{
				/*printf("<%s>\n","WEBFORSGW");
    			printf("%d\n",1);
    			printf("%d\n",1);
    			printf("%s\n","处理结果如下");*/
				if( !m_bFront )
				{
				if(m_poBaseRuleMgr)//这边如果还没有有效说明NEW失败了
				{
					BaseSysParamRule *pParamRule = m_poBaseRuleMgr->getParamRule(mRecord[0].sSectName,mRecord[0].sParamName);
					if(pParamRule)
					{
						if(pParamRule->iEffDateFlag == 1)
						{
								printf("%s\n","立即生效");
						} else if(pParamRule->iEffDateFlag == 2){
								printf("%s\n","重启生效");
						} else if(pParamRule->iEffDateFlag == 3){
								Date mDate(mRecord[0].sDate);
								printf("%s%s%s\n","按照设定的生效日期",mDate.toString(),"生效");
						} else {
								printf("%s\n","重启生效");
			 			}
					  } else {
								printf("%s\n","重启生效");
			 		  }	
				} else {
		  		  	printf("%s\n","重启生效");
				}
				} 
				//printf("</%s>\n","WEBFORSGW");
				break;
			}
			case MSG_PARAM_S_D:
			{		
				printf("%s\n","处理成功!");
				break;
			}
			case MSG_PARAM_S_ALL:
			{	
				
				showResults(m_poSysParamHis,m_iCnt); // 显示参数
				delete []m_poSysParamHis;
				break;
			}
			case MSG_PARAM_ACTIVE_MOUDLE_S://文件刷内存
			{	
				/*printf("<%s>\n","WEBFORSGW");
    			printf("%d\n",1);
    			printf("%d\n",1);
    			printf("%s\n","处理结果如下");*/
				if( !m_bFront )
				{
				if(m_iCnt)
				{
					printf("%s\n","以下参数刷新失败,请检查");
					showResults(m_poSysParamHis,m_iCnt); // 显示参数 
					delete []m_poSysParamHis;
					return -1;
				} else {
						printf("%s\n","参数刷新成功");
						if(m_iCntEx)
						{
							printf("%s\n","以下参数数值发生变化:");
							showResults(m_poSysParamHisEx,m_iCntEx); // 显示参数 
							delete []m_poSysParamHisEx;
							return 1;
						}
						
				}
				} 
				//printf("</%s>\n","WEBFORSGW");		
				break;
			}
			/*case MSG_PARAM_ACTIVE_MOUDLE_M://内存刷文件
			{//内存刷文件 能走到这里就是成功
				printf("%s\n","参数刷新成功");
			}*/
			case MSG_LIST_SECTION:
			case MSG_PARAM_S_N:
			{		
				if(m_iCnt)
				{					
					showResults(m_poSysParamHis,m_iCnt); // 显示参数 
					delete []m_poSysParamHis;
				} else {
					printf("%s\n","参数不存在,查看失败");
					return -1;
				}	
				break;
			}
			default:
			{
				break;
			}
		}		
	 	return 1;//走到这边的默认都是成功
}


/*
函数：showMsgValues
功能：显示从消息队列获得的消息进行合理显示
参数描述：Show是否显示结果
举例:showMsgValues(true)
*/
long SysParamClient::showMsgValues(bool Show)
{
	 return 0;
}


void SysParamClient::setFrontInfo(bool bFront,int iBegin, int iEnd, const char *sSplit)
{
     m_bFront=bFront;
     m_iBegin=iBegin;
     m_iEnd=iEnd;
     m_pSplit=sSplit;
}


/*
函数功能:显示获取的所有参数数据
参数说明:pData :SysParamHis类型的数据源首指针,iNum:数据源总数据数目
返回值说明:无
举例:showResults((void*)pData,100)
*/
void SysParamClient::showResults(void *pData, int iNum)
{ 
   SysParamHis *pParamHis = 0;
   pParamHis = (SysParamHis*)(pData);

if( !m_bFront )
{

   int iSeqNum = iNum/PAGE_MAX_SHOW_NUM;
   if(iNum%PAGE_MAX_SHOW_NUM)
		 iSeqNum++;		 
	for(int i = 0;i<iSeqNum;)
	{
		showOnePage(pParamHis,i,iNum);
		if(iSeqNum == 1)
			i++;
		if(iSeqNum>1)
		{			 
			int iRes = showControl(i);	
			if(iRes == -9)
			    return;	
			else if(iRes == -1)
			{
				if(i>0)
					i--;
			}
			else if(iRes == 1)
			{
				i++;
		    } else {
			 	i++;
			}	 
		 }
	}	 
}else{
    char sLable[128]={0};
    sprintf(sLable,"段名%s参数名%s参数原值%s参数新值%s参数生效日期%s生效方式%s取值范围",
      m_pSplit,m_pSplit,m_pSplit,m_pSplit ,m_pSplit ,m_pSplit);
	//if(iNum > 1)
	//{
    //printf("%d\n%d\n%s\n", 1, iNum, sLable );
	printf("<%s>\n","WEBFORSGW");
    printf("%d\n",1);
    printf("%d\n",iNum);
	//}
    printf("%s\n",sLable);
    if( m_iBegin==m_iEnd && m_iEnd==0){
        m_iEnd=9999;
    }
	int iEndSeq = m_iEnd;
	for(int j=( m_iBegin-1>=0?m_iBegin-1:0); j<iEndSeq && j<iNum; j++ )
	{
		pParamHis = (SysParamHis*)(pData)+j;
		if(pParamHis->sSectionName[0] == '\0'){
			iEndSeq++;
			continue;
		}
        printf("%s%s%s", pParamHis->sSectionName, m_pSplit , pParamHis->sParamName);
		//获取参数规则
		if(!m_poBaseRuleMgr)
			m_poBaseRuleMgr = new BaseRuleMgr();
		char effType[32] = {0};
		BaseSysParamRule *pParamRule = 0;
		if(m_poBaseRuleMgr)//这边如果还没有有效说明NEW失败了
		{
			pParamRule = m_poBaseRuleMgr->getParamRule(pParamHis->sSectionName,pParamHis->sParamName);
			if(pParamRule)
			{
				if(pParamRule->iEffDateFlag == 1)
				{
					sprintf(effType,"%s","立即生效");
				} else if(pParamRule->iEffDateFlag == 2){
					sprintf(effType,"%s","重启生效");
				} else if(pParamRule->iEffDateFlag == 3){
					sprintf(effType,"%s%","按照设定的生效日期生效");
				} else {
					sprintf(effType,"%s","重启生效");
			 	}
			} else {
				sprintf(effType,"%s","重启生效");
			}
		}
		char valuerule[512] = {0};
		if(pParamRule)
		{
			if(pParamRule->sMaxVal[0] != '\0'){
				sprintf(valuerule,"%s%s","最大值:",pParamRule->sMaxVal);
			}
			if(pParamRule->sMinVal[0] != '\0'){
				strcat(valuerule," 最小值:");
				strcat(valuerule,pParamRule->sMinVal);
			}
			if(pParamRule->sEnumVal[0][0] != '\0'){
				strcat(valuerule," 枚举值:");
				for(int i =0;i<pParamRule->iEnumPos;i++)
				{
					if(pParamRule->sEnumVal[i][0] != '\0')
						strcat(valuerule,pParamRule->sEnumVal[i]);
					if(pParamRule->sEnumVal[i+1][0] == '\0'){
						break;
					} else 
						strcat(valuerule,",");
				}
			}
			switch(pParamRule->iValueChange)
			{
				case 1:
					strcat(valuerule," 此参数只能增加");
					break;
				case 2:
					strcat(valuerule," 此参数不能修改");
					break;
				case 3:
					if(pParamRule->sValueChange[0] != '\0'){
						strcat(valuerule," 参数匹配的规则:");
						strcat(valuerule,pParamRule->sValueChange);
					}
					break;
				case 4:
					strcat(valuerule," 此参数只能减小");
					break;
				case 5:
					strcat(valuerule," 此参数只能是ip");
					break;
			}
			if(pParamRule->sDisobey[0] != '\0')
			{
				strcat(valuerule," 参数须规避字符:");
				strcat(valuerule,pParamRule->sDisobey);
			}
			if(pParamRule->sTime24Start[0] != '\0' && pParamRule->sTime24End[0] != '\0')
			{
				strcat(valuerule," 时间最小值:");
				strcat(valuerule,pParamRule->sTime24Start);
				strcat(valuerule," 时间最大值:");
				strcat(valuerule,pParamRule->sTime24End);
			}
			if(pParamRule->iParamNeed>0)
			{
				strcat(valuerule," 此参数必须存在");
			}
			if(pParamRule->sGroupParamName[0] != '\0')
			{
				strcat(valuerule," 此参数属于参数组:");
				strcat(valuerule,pParamRule->sGroupParamName);
			}
		} 
        printf("%s%s%s%s%s%s%s%s%s%s\n", 
            m_pSplit,pParamHis->sParamOrValue,
            m_pSplit,pParamHis->sParamValue,
            m_pSplit,pParamHis->sDate,
			m_pSplit,effType,m_pSplit,valuerule);

	}
	//if(iNum > 1)
		printf("</%s>\n","WEBFORSGW");
}

}

/*
函数功能:显示一个页面的数据
参数说明:pData :SysParamHis类型的数据源,iNowPos:总数据源的当前偏移量(每个页面显示都从这个偏移量对应数据开始显示);iTotalNum:数据源总数据数目
返回值说明:无
举例:showOnePage(pData,0,100)
*/
void SysParamClient::showOnePage(SysParamHis *pData,int iNowPos,int iTotalNum)
{
	SysParamHis *pParamHis = 0;
	int iNumLeft = iTotalNum-iNowPos*PAGE_MAX_SHOW_NUM;
	int iShowNum = iNumLeft>PAGE_MAX_SHOW_NUM?PAGE_MAX_SHOW_NUM:iNumLeft;
	int iEffDateFlag = 2; //默认重启
	
	for(int j=0;j<iShowNum;j++)
	{
		pParamHis = (pData+PAGE_MAX_SHOW_NUM*iNowPos)+j;
		if(pParamHis->sSectionName[0] == '\0')
			continue;
		//获取参数规则
		if(!m_poBaseRuleMgr)
			m_poBaseRuleMgr = new BaseRuleMgr();
		char effType[32] = {0};
		BaseSysParamRule *pParamRule = 0;
		if(m_poBaseRuleMgr)//这边如果还没有有效说明NEW失败了
		{
			pParamRule = m_poBaseRuleMgr->getParamRule(pParamHis->sSectionName,pParamHis->sParamName);
			if(pParamRule)
			{
				iEffDateFlag = pParamRule->iEffDateFlag;
				if(pParamRule->iEffDateFlag == 1)
				{
					sprintf(effType,"%s","立即生效");
 			  } else if(pParamRule->iEffDateFlag == 2){
					sprintf(effType,"%s","重启生效");
				} else if(pParamRule->iEffDateFlag == 3){
					sprintf(effType,"%s%","按照设定的生效日期生效");
				} else {
					sprintf(effType,"%s","重启生效");
			 	}
			} else {
				sprintf(effType,"%s","重启生效");
			}
		}
		printf("\n");
	 	printf("%-18s%s%s%-18s\n", "[参数名]: ",pParamHis->sSectionName,"." ,pParamHis->sParamName);
		if(pParamRule && pParamRule->sDesc[0] != '\0'){
			printf("%-18s%-18s\n","参数描述: ",pParamRule->sDesc);
		}
		switch (iEffDateFlag)
		{
			case 1:
				if(pParamHis->sParamValue[0] != '\0' && pParamHis->sParamOrValue[0] != '\0' 
						&& strcmp(pParamHis->sParamValue,pParamHis->sParamOrValue)!=0)
				{
	 				printf("%-18s%-18s\n", "  原值: ", pParamHis->sParamOrValue);
	 				printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamValue);					
				}
				else if(pParamHis->sParamValue[0] != '\0' && pParamHis->sParamOrValue[0] == '\0')
				{
	 				printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamValue);
	 			} 
	 			else if(pParamHis->sParamOrValue[0] != '\0')
	 			{
					printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamOrValue);
				}
				break;
			case 2:
				if(pParamHis->sParamValue[0] != '\0' && pParamHis->sParamOrValue[0] != '\0' 
						&& strcmp(pParamHis->sParamValue,pParamHis->sParamOrValue)!=0)
				{
	 				printf("%-18s%-18s\n", "    当前值: ", pParamHis->sParamOrValue);
	 				printf("%-18s%-18s\n", "重启后新值: ", pParamHis->sParamValue);					
				}
				else if(pParamHis->sParamValue[0] != '\0' && pParamHis->sParamOrValue[0] == '\0')
				{
	 				printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamValue);
	 			} 
	 			else if(pParamHis->sParamOrValue[0] != '\0')
	 			{
					printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamOrValue);
				}			
				break;
			case 3:
				if(pParamHis->sDate[0] != '\0')
				{
					if(pParamHis->sParamValue[0] != '\0' && pParamHis->sParamOrValue[0] != '\0' 
							&& strcmp(pParamHis->sParamValue,pParamHis->sParamOrValue)!=0)
					{
							printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamOrValue);
							printf("%-18s%-18s\n", "未来值: ", pParamHis->sParamValue);
					}
					else if(pParamHis->sParamValue[0] != '\0' && pParamHis->sParamOrValue[0] == '\0')
					{
		 				printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamValue);
		 			} 
		 			else if(pParamHis->sParamOrValue[0] != '\0')
		 			{
						printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamOrValue);
					}
				}
				else
				{
					if(pParamHis->sParamValue[0] != '\0' && pParamHis->sParamOrValue[0] != '\0' 
							&& strcmp(pParamHis->sParamValue,pParamHis->sParamOrValue)!=0)
					{
							printf("%-18s%-18s\n", "  原值: ", pParamHis->sParamOrValue);
							printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamValue);
					}
					else if(pParamHis->sParamValue[0] != '\0' && pParamHis->sParamOrValue[0] == '\0')
					{
		 				printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamValue);
		 			} 
		 			else if(pParamHis->sParamOrValue[0] != '\0')
		 			{
						printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamOrValue);
					}					
				}
				break;
		}
		/*
		if(pParamHis->sParamValue[0] != '\0' && pParamHis->sParamOrValue[0] != '\0' 
			&& strcmp(pParamHis->sParamValue,pParamHis->sParamOrValue)!=0)
		{
			if(pParamHis->sDate[0] != '\0')
			{
				printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamOrValue);
				printf("%-18s%-18s\n", "未来值: ", pParamHis->sParamValue);
				printf("%-18s%-18s\n", "未来生效日期: ", pParamHis->sDate);
				if(effType[0] != '\0'){
					printf("%-18s%-18s\n", "生效方式: ", effType);
				}
				continue;
			} else {
				if(pParamRule && pParamRule->iEffDateFlag == 1)
				{
	 				printf("%-18s%-18s\n", "  原值: ", pParamHis->sParamOrValue);
	 				printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamValue);
				} else 
				{
					printf("%-18s%-18s\n", "    当前值: ", pParamHis->sParamOrValue);
	 				printf("%-18s%-18s\n", "重启后新值: ", pParamHis->sParamValue);
				}
			}
	 	} else if(pParamHis->sParamValue[0] != '\0' && pParamHis->sParamOrValue[0] == '\0')
		{
	 		printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamValue);
	 	} else if(pParamHis->sParamOrValue[0] != '\0'){
			printf("%-18s%-18s\n", "当前值: ", pParamHis->sParamOrValue);
		}
		*/
		
		if(pParamHis->sDate[0] != '\0'){
			printf("%-18s%-18s\n", "参数生效日期: ", pParamHis->sDate);
		}
		if(effType[0] != '\0'){
			printf("%-18s%-18s\n", "生效方式: ", effType);
		}
		if(pParamRule)
		{
			if(pParamRule->sMaxVal[0] != '\0'){
				printf("%-18s%-18s\n","最大值: ",pParamRule->sMaxVal);
			}
			if(pParamRule->sMinVal[0] != '\0'){
				printf("%-18s%-18s\n","最小值: ",pParamRule->sMinVal);
			}
			if(pParamRule->sEnumVal[0][0] != '\0'){
				printf("%-18s","枚举值: ");
				for(int i =0;i<pParamRule->iEnumPos;i++)
				{
					if(pParamRule->sEnumVal[i][0] != '\0')
						printf("%s",pParamRule->sEnumVal[i]);
					if(pParamRule->sEnumVal[i+1][0] == '\0'){
						printf("\n");
						break;
					} else 
						printf("%s",", ");
				}
			}
			switch(pParamRule->iValueChange)
			{
				case 1:
					printf("%-18s%-18s\n", "参数值规则: " ,"此参数只能增加");
					break;
				case 2:
					printf("%-18s%-18s\n", "参数值规则: ","此参数不能修改");
					break;
				case 3:
					if(pParamRule->sValueChange[0] != '\0'){
						printf("%-18s%-18s\n","参数匹配的规则:", pParamRule->sValueChange);
					}
					break;
				case 4:
					printf("%-18s%-18s\n", "参数值规则: ","此参数只能减小");
					break;
				case 5:
					printf("%-18s%-18s\n", "参数值规则: ","此参数只能是ip");
					break;
			}
			if(pParamRule->sDisobey[0] != '\0')
			{
				printf("%-18s%-18s\n","参数须规避字符:",pParamRule->sDisobey);
			}
			if(pParamRule->sTime24Start[0] != '\0' && pParamRule->sTime24End[0] != '\0')
			{
				printf("%s%s%s%s","时间最小值: ",pParamRule->sTime24Start ," 时间最大值: ",pParamRule->sTime24End);
				printf("\n");
			}
			if(pParamRule->iParamNeed>0)
			{
				printf("%-18s%-18s\n","参数值规则: ","此参数必须存在");
			}
			if(pParamRule->sGroupParamName[0] != '\0')
			{
				printf("%-18s%-18s\n","此参数属于参数组:",pParamRule->sGroupParamName);
			}
		} 
	}
}
/*
函数功能:控制更换页面
参数说明:iPos:当前页面开始显示数据的偏移量(当前页面从这个偏移量对应数据开始显示)
返回值说明:1:继续显示;-1:返回上个页面显示;-9:退出显示
举例:showControl(1)
*/
int SysParamClient::showControl(int &iPos)
{
	 char sChoose[32] = {0};
	 printf("Please select: \n"
                   "c: 继续下页显示 | "
                   "b: 返回上页显示 | "
                   "q: 退出,不继续显示\n");
	 scanf("%s", &sChoose);
	 if(strcmp(sChoose,"c")==0 || strcmp(sChoose,"C")==0)
	 {
	 	return 1;
	 } else if(strcmp(sChoose,"b")==0 || strcmp(sChoose,"B")==0)
	 {
	 	return -1;
	 } else {
	 	return -9;
	 }
}

/*
函数功能:从参数获取段名
参数说明:sParamValue参数;sParamName存放获取后的段名
返回值说明:无
举例:analyseParamName("SYSTEM.location=beijing",sParamName);sParamName=SYSTEM
*/
void SysParamClient::analyseParamName(char *sParamValue,char *sParamName)
{
	string str(sParamValue);
	int iPos = str.find(".");
	sParamName[0] = '\0';
	if(iPos>0)
	    strncpy(sParamName,sParamValue,iPos*sizeof(char));
	else{
		strcpy(sParamName,sParamValue);
	}
	
}


/*
函数功能:为monitor处理的函数,去除了参数规则检验功能以及错误提示功能(这些功能放到monitorC文件处理)
参数说明:
返回值说明:0无数据返回;1-有数据返回
举例:deal4Monitor("SYSTEM","SYSTEM.location",MSG_PARAM_S_N,pCmdCom)获取SYSTEM.location字段的值
*/
int SysParamClient::deal4Monitor(char *sParamName, string sSysParamValue, long lCMDID, CommandCom *pCmdCom)
{
	m_poCmdCom = pCmdCom;
	if(NULL == m_poCmdCom)
			return -1;
	return deal4Monitor(sParamName,sSysParamValue,lCMDID);
}

/*
函数功能:为monitor处理的函数,去除了参数规则检验功能以及错误提示功能(这些功能放到monitorC文件处理)
参数说明:
返回值说明:0无数据返回;1-有数据返回
举例:deal4Monitor("SYSTEM","SYSTEM.location",MSG_PARAM_S_N)获取SYSTEM.location字段的值
*/
int SysParamClient::deal4Monitor(char *sParamName, string sSysParamValue, long lCMDID)
{ 
	return dealWithMem(sParamName, (char*)sSysParamValue.c_str(), lCMDID,false);
}

/*
函数功能:获取initserver数据前的消息队列清空处理
参数说明:
返回值说明:无
举例:
*/
void SysParamClient::clearMsg()
{
}

/*
函数功能:获取initserver数据后的数据格式转化
参数说明:
返回值说明:转化数据格式(一般hbmonitor使用)[-1-参数获取失败;-2-intserver提示的处理失败;-3-得到的返回参数格式错误;0-是指针形式的返回;正值是错误码]
举例:
*/
long SysParamClient::VecToSysParamInfo(ParamHisRecord *pParamHisRecord,int &iParamNum,long lCMDID)
{
	if(!pParamHisRecord)
		return -1;
	switch(lCMDID)
	{
		case MSG_PARAM_CHANGE:
		case MSG_PARAM_ACTIVE_MOUDLE_S:
		{
			break;
		}
		case MSG_PARAM_S_ALL:
		case MSG_LIST_SECTION:
		case MSG_PARAM_S_N:
		{
			for(int i = 0;i<m_iCnt;i++)
			{
				strcpy(pParamHisRecord[i].sSectName,m_poSysParamHis[i].sSectionName);
				strcpy(pParamHisRecord[i].sParamName,m_poSysParamHis[i].sParamName);
				strcpy(pParamHisRecord[i].sParamOrValue,m_poSysParamHis[i].sParamOrValue);
				strcpy(pParamHisRecord[i].sParamValue,m_poSysParamHis[i].sParamValue);
				strcpy(pParamHisRecord[i].sDate,m_poSysParamHis[i].sDate);
			}
			iParamNum = m_iCnt;
			return m_iCnt;
		}
		default:
		{
			return 0;
		}
	}
	return 0;
}

/*
函数功能:显示帮助信息
参数说明:
返回值说明:无
举例:
*/
void SysParamClient::showHowToUseCMD()
{
     cout <<" ******************************************************************" << endl;
     cout <<"  使用方式:" << endl;
     cout <<"  abmparam -list <段名.参数名>                    :显示参数信息"<< endl;
     cout <<"  abmparam -set 段名.参数名=参数值	:设定参数值(=两边不留空格)"<< endl;
     cout <<"  abmparam -refresh :将已修改的参数刷新，并立即生效" << endl;
	 cout <<" ******************************************************************" << endl;
}


/*
函数名:paramParser
函数功能:解析数据得出initserver能识别的数据,给initserver处理
参数说明:sAllParamVal是用户的输入,未解析的数据;strDsc解析后最终数据赋给的string
返回值说明:true解析成功;false解析失败
举例:string strDsc; paramParser("SYSTEM.location=beijing,effdate=2011", strDsc)
	strDsc结果数据是"SYSTEM.location=beijing,SYSTEM.location.effdate=2011"
*/
bool SysParamClient::paramParser(char *sAllParamVal,string &strDsc)
{
	 
	 if(sAllParamVal[0] != '\0')
	 {
	 	memset(mRecord,'\0',sizeof(struct ParamHisRecord));	
	 } else {
	 	return false;
	 }
	 
	 string strEffdate;
	 string sPureEff;//生效日期值
	 string sEffFlag;//标识effdate
	 string sParam;
	 string strAllParam(sAllParamVal);
	 int iParamEffPos = strAllParam.rfind(",");
	 if(iParamEffPos == 0)
	    return false;
	 if(iParamEffPos>0)
	 {
	    strEffdate.assign(strAllParam,iParamEffPos+1,strAllParam.size());
		int iEffPos = strEffdate.find("=");
		if(iEffPos>0){
			sPureEff.assign(strEffdate,iEffPos+1,strEffdate.size());
			sEffFlag.assign(strEffdate,0,iEffPos);
			char sEffDateFlag[32] = {0};
			strncpy(sEffDateFlag,(char*)sEffFlag.c_str(),sizeof(sEffDateFlag));
			//Chang2Lower(sEffDateFlag);
			if( strcmp(sEffDateFlag,EFFDATE) != 0 )
			{
				return false;
			}
		}
	    sParam.assign(strAllParam,0,iParamEffPos);
	 } else {
	    sParam = strAllParam;
	 }
	 int iValPos = sParam.rfind('=');
	 int iParamPos = sParam.find('.');
	 if(iValPos<0 || iParamPos<0)
	 {		
		return false;
	 }
	 string sPureSect(sParam,0,iParamPos);
	 string sPureParam(sParam,iParamPos+1,iValPos-iParamPos-1);
	 string sPureVal(sParam,iValPos+1,sParam.size());
	 trimN(sPureSect);
	 trimN(sPureParam);
	 trimN(sPureVal);
	 trimN(sPureEff);
	 if(sPureSect.size()>0 && sPureParam.size()>0)
	 {
	 	strncpy(mRecord[0].sSectName,(char*)sPureSect.c_str(),sizeof(mRecord[0].sSectName));
	 	strncpy(mRecord[0].sParamName,(char*)sPureParam.c_str(),sizeof(mRecord[0].sParamName));
	 	//Chang2Upper(mRecord[0].sSectName);
	 	//Chang2Lower(mRecord[0].sParamName);
		strDsc+=sPureSect;
		strDsc+=".";
		strDsc+=sPureParam;
	 } else {
	 	return false;
	 }
	 
	 if(sPureVal.size()>0)
	 {
	 	strncpy(mRecord[0].sParamValue,(char*)sPureVal.c_str(),sizeof(mRecord[0].sParamValue));
		if(sPureEff.size()>0)
	 	{
			strDsc += ".";
			strDsc += NEW_VALUE;
		}
		strDsc += "=";
		strDsc+=sPureVal;

	 	if(sPureEff.size()>0)
	 	{
			string strTmp;
			strTmp = "," + sPureSect + "." + sPureParam + "." + EFF_DATE_NAME;
			strDsc += strTmp;
			strDsc += "=";
			strDsc += sPureEff;
			strncpy(mRecord[0].sDate,(char*)sPureEff.c_str(),sizeof(mRecord[0].sDate));
	 	}
	 	//return true;
	 }
	 return true;
}

//MEMORY_DB 的提示
bool SysParamClient::prompt(string sysparam,bool show)
{
	 if(!show)
	 	return false;
	 string str = sysparam;
	 trimN(str);
	 int pos = str.find('.');
	 if(pos>0)
	 {
	 	//继续处理 "MEMORY_DB"
		string sect;
		sect.assign(str,0,pos);
		int posend = str.rfind('=');
		if(posend<=pos)
			return false;
		string param;
		param.assign(str,pos+1,posend-pos-1);
		char *set = new char[sect.size()+1];
		char *aram = new char[param.size()+1];
		if(!set || !aram)
		{
			printf("%s\n","内存不足");
			THROW(1);
		}
		strcpy(set,(char*)sect.c_str());
		strcpy(aram,(char*)param.c_str());
		//Chang2Upper(set);
		//Chang2Lower(aram);
		if(strcmp(set,"MEMORY_DB")==0)
		{
			if(strcmp(aram,"acctitemcount")==0)
			{
				printf("%s\n","帐内存请使用ctlacctitem重载");
			}
			if(strcmp(aram,"acc_data_count")==0)
			{
				printf("%s\n","累积量实例请使用ctlaccu 重载");
			}
			if(strcmp(aram,"acc_cycle_count")==0)
			{
				printf("%s\n","累计周期实例请使用ctlaccu重载");
			}
			
		}
		if(strcmp(set,"PRODOFFERAGGR")==0)
		{
			if(strcmp(aram,"totaldata")==0)
			{
				printf("%s\n","优惠总账请使用ctlofferaggr重载 ");
			}
			if(strcmp(aram,"detaildata")==0)
			{
				printf("%s\n","优惠明细请使用ctlofferaggr重载");
			}
		}
		delete []set;
		delete []aram;
	 }
	 return true;
}			 

/*2.8入网测试函数 已不使用*/

//2.8测试版本
int SysParamClient::deal(char *sParamName, string sSysParamValue, long lCMDID, CommandCom *pCmdCom)
{
	 m_poCmdCom = pCmdCom;
	 return deal(sParamName, sSysParamValue, lCMDID);
}

/*
函数功能: 发送命令参数给initserver,得到处理结果
参数描述: sSectName段, sSysParamValue 参数值(段.参数=值,effdate=2011可无生效日期), lCMDID命令ID, ShowErrInfo是否显示报错信息
返回值: 失败返回值<0;超时0;获取到值为1
举例: dealWithServer("SYSTEM","SYSTEM.location=beijing",MSG_PARAM_S_N, true)
*/
int SysParamClient::dealWithServer(char *sSectName,char *sSysParamValue,long lCMDID,bool ShowErrInfo)
{
	
	return 0;
}

/*
函数功能:获取initserver数据是的时间等待控制
参数说明:iTimer当前计数器数值标识, lCMDID命令ID, Show是否显示延时提示
返回值说明:>0可以继续延时,=0不可以继续延时
举例:waitTimer(iTimer,MSG_PARAM_S_ALL, true)
*/
int SysParamClient::waitTimer(int &iTimer,long lCMDID,bool Show)
{
	if(m_iWaitMax<=0)
		m_iWaitMax = 5;
	if(iTimer == 1)
	{
		if(lCMDID == MSG_PARAM_S_ALL)
			sleep(2);
		else
			sleep(1);
		iTimer++;
	} else {
		if(iTimer<m_iWaitMax){
			usleep(100);
			iTimer++;
		} else {
			if(Show)
				printf("接受数据超时!\n");			
			clearPtr();
			return 0;//获取信息失败 
		}	
	}
	return iTimer;
}

/*
函数：clearPtr
功能：清空临时存放数据
参数描述：
举例:
*/
void SysParamClient::clearPtr()
{
}

/*
函数：deal(variant sParamName, variant sSysParamValue, long lCMDID,bool Show)
功能：实现事件的发送与接，只发送一次，可以接多个消息；只做消息积压处理
参数描述：sParamName是要设置或改变的配置文件中的参数名，格式是：根目录/文件名/段名/参数
		 sSysParamValue 是 参数具体值 ; lCMDID 是这个命令的编号 ; Show 是否显示错误信息
返回值:  <0 表示失败  >=0成功
举例：   deal("ROOT", "ROOT.location=beijing", MSG_PARAM_CHANGE,true) 
*/
int SysParamClient::deal(char *sParamName, string sSysParamValue, long lCMDID,bool Show)
{ 
	string strDsc;
	bool bRes = IfSuitRule(lCMDID,(char*)sSysParamValue.c_str(),strDsc,Show);
		
	if(bRes)
	{
	 	return dealWithServer(sParamName, (char*)strDsc.c_str(), lCMDID, Show);
	} else {
		if(Show)
			printf("参数%s规则检效失败\n",strDsc.c_str());
		return -1;;
	}
}

/*
函数功能:仲裁功能发送数据(只提供修改参数使用)
参数说明:str用户输入的有效数据
返回值说明:
举例:send2Arbi("SYSTEM.location=beijing")
*/ 
void SysParamClient::send2Arbi(string str)
{
#ifdef _ARBITRATION_
	arbi = false;
	string strTmp = str;
	string strSect;
	string strParam;
	int iPos = strTmp.find(',');
	string first;
	if(iPos>0)
	{
		first.assign(strTmp,0,iPos);
	} else {
		first = strTmp;
	}
	int iSectPos = first.find('=');
	if(iSectPos>0)
	{
		strSect.assign(first,0,iSectPos);
		strParam.assign(first,iSectPos+1,first.size());
	}
	if(strSect.size()>0 && strSect.size()>0)
	{
		if(m_poBaseRuleMgr->neddArbi((char*)strSect.c_str(),(char*)strParam.c_str()tr))
		{				
				vector<string> vecSysValues;
				vecSysValues.clear();
				vecSysValues.push_back(str);
				if(m_iState==0)//主机
				{
					int iTemp=m_pArbitrationMgr->sendMulVal(vecSysValues,"vecSysValues");
					if(iTemp == -1)
					{
						//初始化索引失败
						throw(MBC_Arbitration +4);
					}
					else if( iTemp  != 0)
					{
						//传送索引变量失败
						throw(MBC_Arbitration +5);
					}
				}					
		}
	}
#endif
}

/*
函数功能:仲裁功能获取数据
参数说明:
返回值说明:
举例:
*/  
void SysParamClient::recData()
{
#ifdef _ARBITRATION_
	vector<string> vecSysValues;
	vecSysValues.clear();
	if(m_iState==1)//备机
	{
		 while(1)
    	 {
    		if (GetInterruptFlag_A())
			{
				break;
			}
			sleep(1);
		 }
		 DenyInterrupt_A();
		 int iTemp=m_pArbitrationMgr->receiveSingleVal(vecSysValues,"vecSysValues");
		 if(iTemp == -1)
		 {
		 	//初始化索引失败
		 	throw(MBC_Arbitration +4);
		 }
		 else if( iTemp  != 0)
		 {
		 	//接收索引变量失败
			throw(MBC_Arbitration +5);
		 }			
		 //取到	vecSysValues,再把取到的参数串取出来，逐个解析处理
		 for(int i =0;i<vecSysValues.size();i++)
		 {
		 	char sParamName[1024] = {0};
			analyseParamName(vecSysValues[i],sParamName);
		 	deal(sParamName, vecSysValues[i], MSG_PARAM_CHANGE, true);
		 }
	}
#endif
}

// "true","false"
bool SysParamClient::getArbiResult(char *sValue)
{
#ifdef _ARBITRATION_
	if(m_iState ==2)
	{
		//单点不用仲裁
		return true;
	}
	else
	{
		if(m_pArbitrationMgr->Audit((char *) sValue) == 1)
		{
			int iTemp=m_pArbitrationMgr->Success();
			if(iTemp == -1)
			{									
				throw(MBC_Arbitration +11);
			}
			//Log::log(0,"%s仲裁成功！！！",sValue);
			return true;
		}
		else
		{
			//Log::log(0,"%s仲裁失败！！！",sValue);
			int iTemp=m_pArbitrationMgr->Fail();
			return false;
		}	
	}					
#endif
	return true;
}

/*
函数功能:解析用户输入数据,处理得到最终结果,显示给用户
参数说明:sArgv, iArgc 等同main函数的两个参数
返回值说明:>0解析成功;=0无需继续处理;<0解析失败
举例:sArgv = {"hbparam","-list","SYSTEM"} iArgc=3
*/ 
int SysParamClient::resolveParam(char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc)
{
    return -1;
}

void SysParamClient::setLogLevel()
{
	sprintf(sLogLevel,"%s%d","LOG_LEVEL=",-2);
	putenv(sLogLevel);
}
//规则检效
int SysParamClient::ruleCheck(long lCMDID,char *m_sSysParamValue)
{
	switch(lCMDID)
	{
		case MSG_PARAM_CHANGE:	
				m_poBaseRuleMgr->setDebugState(m_bMONDebug);			
				if(!m_poBaseRuleMgr->check(m_sSysParamValue)){					
					m_poBaseRuleMgr->paramParseExplain(true);//check之后的参数规则说明
					return -1;
				}
				break;	
		default:
				break;	
	}
	return 1;
}

char *SysParamClient::getInitParamValue()
{
	return m_sInitParamValue;
}
void SysParamClient::setDebugState(bool _Debug)
{
	 m_bMONDebug = _Debug;
	 if(m_poBaseRuleMgr)
	 	m_poBaseRuleMgr->setDebugState(_Debug);
}

