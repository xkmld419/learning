#include "LogGroup.h"
#include "CmdMsg.h"
#include <curses.h>
#include <sys/types.h>
#include <signal.h>
#ifdef clear
#undef clear
#endif

vector<string> LogGroup::m_vConverInfo;
vector<string> LogGroup::m_vDisplayInfo;
vector<string> LogGroup::m_vAlarmInfo;
vector<string> LogGroup::m_vParamInfo;
vector<string> LogGroup::m_vSysInfo;
vector<string> LogGroup::m_vAttemperInfo;
vector<string> LogGroup::m_vAccessInfo;
vector<string> LogGroup::m_vDmlInfo;

LogGroup::LogGroup():m_iMode(0),m_iGroupId(-1),m_bPigeonhole(false),
                          m_iLine(0),m_iModuleId(-1),m_iDisMode(1),m_bQryFile(0)
{
    memset(m_sPath,0,sizeof(m_sPath));
    memset(m_sFileName,0,sizeof(m_sFileName));
    memset(m_sBatch,0,sizeof(m_sBatch));
    memset(m_sParam,0,sizeof(m_sParam));
    Log::init(MBC_COMMAND_hblogg);
    LOGG(ATTEMPERLOG,"--进程启动--");
    if(Log::m_pologg)
        m_poLogGroup = Log::m_pologg;
    else
        m_poLogGroup = new ThreeLogGroupMgr();
    if(!m_poLogGroup)
    {
        Log::log(0,"创建日志组管类对象失败");
        ALARMLOG(0,MBC_Log_Group+5,"%s","创建日志组管类对象失败");
        THROW(MBC_Log_Group+5);
    }

    m_poCheckPoint = 0;
    m_poCmdCom = 0;
    LogGroup::m_vDisplayInfo.clear();
    char sMode[10]={0};
    ParamDefineMgr::getParam("HBLOGG","DISPLAY_LOG_MODE",sMode,50);
    m_iDisMode = atoi(sMode);
        
}

LogGroup::~LogGroup()
{
    LOGG(ATTEMPERLOG,"--进程退出(析构)--");
    if(m_poLogGroup)
        delete m_poLogGroup;
    m_poLogGroup = 0;

    if(m_poCheckPoint)
        delete m_poCheckPoint;
    m_poCheckPoint = 0;

    if(m_poCmdCom)
        delete m_poCmdCom;
    m_poCmdCom = 0;
    
    LogGroup::m_vConverInfo.clear();
    LogGroup::m_vDisplayInfo.clear();
    LogGroup::m_vAlarmInfo.clear();
    LogGroup::m_vParamInfo.clear();
    LogGroup::m_vSysInfo.clear();
    LogGroup::m_vAttemperInfo.clear();
    LogGroup::m_vAccessInfo.clear();
    LogGroup::m_vDmlInfo.clear();
}

bool LogGroup::init()
{
    memset(m_sPath,0,sizeof(m_sPath));
    memset(m_sFileName,0,sizeof(m_sFileName));
    memset(m_sBatch,0,sizeof(m_sBatch));
    memset(m_sParam,0,sizeof(m_sParam));
    if(!m_poLogGroup)
    {
        if(Log::m_pologg)
            m_poLogGroup = Log::m_pologg;
        else
            m_poLogGroup = new ThreeLogGroupMgr();
    }
    if(!m_poLogGroup)
    {
        Log::log(0,"创建日志组管类对象失败");
        ALARMLOG(0,MBC_Log_Group+5,"%s","创建日志组管类对象失败");
        THROW(MBC_Log_Group+5);
    }

    m_poCheckPoint = 0;
    m_poCmdCom = 0;
    m_vDisplayInfo.clear();
    char sMode[10]={0};
    ParamDefineMgr::getParam("HBLOGG","DISPLAY_LOG_MODE",sMode,50);
    m_iDisMode = atoi(sMode);   
    return true;
}

void LogGroup::printUsage()
{
    cout << endl << endl;
    cout << "*********************************************************************************" << endl;
    cout << "hblogg" << " "  << "-c :                                      创建日志组共享内存" << endl;
    cout << "hblogg" << " "  << "-f :                                      释放日志组共享内存" << endl;
    cout << "hblogg" << " "  << "-a :                                      日志组手动归档" << endl;
    cout << "hblogg" << " "  << "-b groupid path(不指定备份到默认目录) :   备份指定日志组到指定目录" << endl;
    cout << "hblogg" << " "  << "-d groupid filename :                     删除日志组文件" << endl;
    cout << "hblogg" << " "  << "-e param :                                设置日志组归档,param为0或1" << endl;
    cout << "hblogg" << " "  << "-i groupid path(不配取默认路径) :         增加日志组文件" << endl;
//    cout << "hblogg" << " "  << "-j BatchId:                               加载checkpoint文件" << endl;    
    cout << "hblogg" << " "  << "-l :                                      显示(当前/指定)日志组信息" << endl;
//    cout << "hblogg" << " "  << "-o groupid :                              展现最新数据内容" << endl;
    cout << "hblogg" << " "  << "-s :                                      切换日志组" << endl;
//    cout << "hblogg" << " "  << "-w :                                      落地checkpoint信息" << endl;
    cout << "hblogg" << " "  << "-param[parameter] :                       显示所有[指定参数名]的日志内容"<<endl;
    cout << "hblogg" << " "  << "-checkpoint :                             显示checkpoint的日志内容"<<endl;
    cout << "hblogg" << " "  << "-process[process] :                      显示所有[指定进程名]的日志内容"<<endl;
    cout << "hblogg" << " "  << "-module[module] :                        显示所有[指定模块名]的日志内容"<<endl;
    cout << "hblogg" << " "  << "-class[class] :                          显示所有[指定类别名]的日志内容"<<endl;
    cout << "hblogg" << " "  << "-level[level] :                          显示所有[指定等级]的日志内容"<<endl;
    cout << "hblogg" << " "  << "-staffid[staffid] :                      显示[指定工号]的日志内容"<<endl;
    cout << "hblogg" << " "  << "-tablename[tablename] :                  显示[指定表名]的日志内容"<<endl;
    cout << "hblogg" << " "  << "-sqltype[sqltype] :                      显示[指定操作类型]的日志内容"<<endl;
    cout << "*********************************************************************************" << endl;
    cout << endl << endl;

}

bool LogGroup::IfCDisplayInfo()
{
	string m_key;

	cout<<"c:继续 其他:退出"<<endl;
	cin>>m_key;
	if((!strcmp(m_key.c_str(),"c"))||(!strcmp(m_key.c_str(),"C")))
		return true;
	return false;
}

bool LogGroup::AnalyParamInfo(int LogFlag,char *sStandId1, vector < string > & vParamInfo,vector < string > & vDisInfo)
{
    /*
    string strinfo;
    unsigned int m=0;
    int loc=0;
    char rule[100]={0};
    switch(LogFlag)
	{
        case PARAMLOG:
        {
            if(strlen(sStandId1))
                sprintf(rule,"[PARAM:%s]",sStandId1);
            else
                strcpy(rule,"[PARAM:");
        }
        break;
        
        case SYSTEMLOG:
        {
            sprintf(rule,"[STAFFID:%s]",sStandId1);
        }
        break;
        
        default:
            strcpy(rule,"[");
           break;
	}
    vector<string>::reverse_iterator r_iter;
    if(vParamInfo.size()==0)
    {
        return false;
    }
    for(r_iter=vParamInfo.rbegin();r_iter!=vParamInfo.rend();++r_iter)
    {
        loc = (*r_iter).find(rule,0);
        if(loc!=string::npos)
            vDisInfo.push_back(*r_iter);
    }
    */
    return true;
}

bool LogGroup::AnalyInfoByFile(int LogFlag,int StandId1,char *sStandId1,vector<string> &vDisplayInfo)
{
    FILE *fp;
    DIR *dirp=NULL;
    struct dirent *dp=NULL;
    struct stat statbuf;
    int istate=-1,count=0,filecount=0,upCount=0,inCount=0;
    char spath[500]={0};
    char sName[500]={0},buf[1024]={0};
    char rule[100]={0};
    char rule_1[100]={0};
    char rule_2[100]={0};

    switch(LogFlag)
	{
	case OPERATIONLOG:
        if(StandId1==PINFOPOINT)
        {//按等级查询信息点
            int iLevelid = atoi(sStandId1);
            if(iLevelid==1)
                strcpy(rule,"[LV-1]");
            else if(iLevelid==2)
                strcpy(rule,"[LV-2]");
            else if(iLevelid==3)
                strcpy(rule,"[LV-3]");
            else
                strcpy(rule,"[LV-");
        }
        else if(StandId1==DISALARM)
        {
            if(strlen(sStandId1))
                sprintf(rule,"ModuleId:[%s]",sStandId1);
            else
                strcpy(rule,"[ALARM]");
        }
        else if(StandId1==DISPLAYALL)
        {
            strcpy(rule,"[LV");
            strcpy(rule_1,"[ALARM]");
            strcpy(rule_2,"DML[");
        }
		break;
	case SYSTEMLOG:
        if(StandId1==DISPLAYALL)
        {
            strcpy(rule,"[checkpoint]");
            strcpy(rule_1,"[STAFFID:");
            strcpy(rule_2,"[ADMIN]");
        }
        else if(StandId1==DISBYCHECK)
            strcpy(rule,"[checkpoint]");
        else
            sprintf(rule,"[%s]",sStandId1);
		break;
	case PARAMLOG:
        if(StandId1==DISPLAYALL)
            strcpy(rule,"[PARAM:");
        else
            sprintf(rule,"[PARAM:%s]",sStandId1);
		break;
    case ATTEMPERLOG:
        if(StandId1==DISPLAYALL)
            strcpy(rule,"[PRO:");
        else
            sprintf(rule,"[PRO:%s]",sStandId1);
        break;
    case HBACCESS:
        if(StandId1==DISPLAYALL)
            strcpy(rule,"TABLE_NAME:[");
        else
            sprintf(rule,"[%s]",sStandId1);
        break;

	default:
		return false;
	}

    if(strlen(rule)==0)
        return false;
    
    char *p=NULL;
    char sLog_HOME[100];
    memset(sLog_HOME,0,sizeof(sLog_HOME));
    if ((p=getenv ("BILLDIR")) == NULL)
	    sprintf (sLog_HOME, "/opt/opthb/log");
	else 
	    sprintf (sLog_HOME, "%s/log", p);

    sprintf(spath,"%s/LOGG_ANALY/",sLog_HOME);
    
    if (spath[strlen(spath)-1] != '/')
    {
        strcat(spath, "/");
    }
    dirp = opendir(spath);
    if(dirp==NULL)
    {
        Log::log (0, "打开指定目录出错");
        return false;
    }

    for(dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
    {
        strncpy(sName, dp->d_name, 500);
        if(strcmp(sName, ".")==0 || strcmp(sName, "..")==0)
            continue;
        char TempName[1000]={0};
        sprintf(TempName,"%s%s/",spath,sName);
        if(lstat(TempName, &statbuf)<0)
        {
            perror("istate");
            Log::log (0, "获取文件状态出错，文件名:%s",sName);
            continue;    
        }
        if(S_ISDIR(statbuf.st_mode)==1)
        {//是目录
            continue;
        }
 		if (0 == statbuf.st_size)
        {
            Log::log (0, "获取文件大小为空，文件名:%s",sName);
 		    continue;
        }        
        fp = fopen(TempName,"r");
        if(fp==NULL)
        {
            Log::log (0, "打开文件失败，文件名:%s",sName);
            continue;
        }
        string sAnalyStr;
        int maxcnt=0;
        while(fgets(buf,sizeof(buf),fp))
        {
            string::size_type loc=0,loc1=0;
            if(strcmp(buf,"\n")==0)
                 continue;
            if(maxcnt>=DISPLAYMAXCNT)
                break;
            sAnalyStr=buf;
            loc = sAnalyStr.find(rule,0);
            if(loc!=string::npos)
            {
                vDisplayInfo.push_back(sAnalyStr);
                maxcnt++;
            }
            if(strlen(rule_1))
            {
                loc1 = sAnalyStr.find(rule_1,0);
                if(loc1!=string::npos)
                {
                    vDisplayInfo.push_back(sAnalyStr);
                    maxcnt++;
                }
            }
            if(strlen(rule_2))
            {
                loc1 = sAnalyStr.find(rule_2,0);
                if(loc1!=string::npos)
                {
                    vDisplayInfo.push_back(sAnalyStr);
                    maxcnt++;
                }
            }
        }
        fclose(fp);
    }
    closedir(dirp);
    return true;
}

bool LogGroup::AnalyAlarmInfoFromFile(char *sStandId1,vector<string> &vDisplayInfo)
{
    FILE *fp;
    char rule[100]={0};
    char buf[1024]={0};
    char TempName[1024]={0};

    vector<string>::iterator iter;
    if(strlen(sStandId1))
        sprintf(rule,"ModuleId:[%s]",sStandId1);
    else
        strcpy(rule,"[ALARM]");

    char *p=NULL;
    char sLog_HOME[100];
    memset(sLog_HOME,0,sizeof(sLog_HOME));
    if ((p=getenv ("BILLDIR")) == NULL)
	    sprintf (sLog_HOME, "/opt/opthb/log");
	else 
	    sprintf (sLog_HOME, "%s/log", p);

    sprintf(TempName,"%s/Log_bottom_alarm.log",sLog_HOME);
    fp = fopen(TempName,"r");
    if(fp==NULL)
    {
        return false;
    }
    string sAnalyStr;
    while(fgets(buf,sizeof(buf),fp))
    {
        int loc=0,loc1=0;
        if(strcmp(buf,"\n")==0)
             continue;
        if(strlen(buf)==0)
            continue;
        sAnalyStr=buf;
        loc = sAnalyStr.find(rule,0);
        if(loc!=string::npos)
        {
            iter = vDisplayInfo.begin();
            if(iter!=vDisplayInfo.end())
                vDisplayInfo.insert(iter,sAnalyStr);
            else
                vDisplayInfo.push_back(sAnalyStr);
        }
    }
    fclose(fp);
    return true;
}

bool LogGroup::AnalyzesParam(char *sParam,char *sReturn)
{
    char *p = strchr(sParam,'[');
    if(!p)
        return false;
    p++;
    char *q = strchr(sParam,']');
    if(!q)
        return false;
    strncpy(sReturn,p,q-p);
    return true;
}

bool LogGroup::GetAppIdByName(char *sParam,char *sReturn)
{
    char sSql[1024]={0};
    int iAppId=0;

    DEFINE_QUERY(qry);
    try
    {
    sprintf(sSql,"select distinct(a.app_id) from wf_application a where "
        "Upper(a.exec_name) like Upper('%s')",m_sParam);
    qry.setSQL(sSql);
    qry.open ();
    if(qry.next())
        iAppId = qry.field(0).asInteger();
    qry.close();

    if(iAppId==0)
    {
        return false;
    }
    memset(sReturn,0,sizeof(sReturn));
    sprintf(sReturn,"%d",iAppId);
    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return false;
    }
    return true;
}

bool LogGroup::GetModuleIdByName(char * sParam, char * sReturn)
{
    char sSql[1024]={0};
    int iModId=0;

    memset(sReturn,0,sizeof(sReturn));
    DEFINE_QUERY(qry);
    try
    {
    sprintf(sSql,"select distinct(a.module_id) from wf_application a where "
        "Upper(a.exec_name) like Upper('%s')",m_sParam);
    qry.setSQL(sSql);
    qry.open ();
    if(qry.next())
        iModId = qry.field(0).asInteger();
    qry.close();

    if(iModId==0)
    {
        return false;
    }
    sprintf(sReturn,"%d",iModId);
    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return false;
    }
    return true;   
}

bool LogGroup::CheckParamModule()
{
    char sSql[1024]={0};
    int cnt=0;

    DEFINE_QUERY(qry);
    try
    {
    sprintf(sSql,"select count(*) from wf_module where Upper(MODULE_NAME) like Upper('%s') ",m_sParam);
    qry.setSQL(sSql);
    qry.open ();
    if(qry.next())
        cnt = qry.field(0).asInteger();
    qry.close();

    if(cnt==0)
    {
        printf("------------Err Msg------------\r\n");
        printf("\n找不到模块名称为[%s]的相关记录,请查询模块定义列表[wf_module]获取正确的模块名\n\n",m_sParam);
        return false;
    }
    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return false;
    }
    return true;
}

bool LogGroup::CheckModuleForMem()
{
    char sSql[1024]={0};
    bool bfind = false;

    DEFINE_QUERY(qry);
    try
    {
    if(strlen(m_sParam)==0)
//        sprintf(sSql,"select distinct(c.process_id) from wf_module a ,wf_application b,wf_process c "
//            "where a.module_id = b.module_id and b.app_id = c.app_id ");
          sprintf(sSql,"select distinct(module_id) from wf_module ");
    else
//        sprintf(sSql,"select distinct(c.process_id) from wf_module a ,wf_application b,wf_process c "
//            "where a.module_id = b.module_id and b.app_id = c.app_id "
//            "and Upper(a.MODULE_NAME) like Upper('%s')",m_sParam);
          sprintf(sSql,"select distinct(module_id) from wf_module where Upper(MODULE_NAME) like Upper('%s')",m_sParam);
    qry.setSQL(sSql);
    qry.open ();
    while(qry.next())
    {
        m_iModuleId= qry.field(0).asInteger();
        if(m_iModuleId>0)
        {
            char sOrder[20]={0};
            memset(sOrder,0,sizeof(sOrder));
            sprintf(sOrder,"%d",m_iModuleId);
            if(!m_bQryFile)
            {
                m_poLogGroup->AnalyzesDataInfo(OPERATIONLOG,DISALARM,sOrder,m_vDisplayInfo);
                if(m_iDisMode==1)
                {
                    if(!DisplayDataInfo(m_iLine))
                        continue;
                    bfind = true;
                }
            }
            else
            {
                AnalyInfoByFile(OPERATIONLOG,DISALARM,sOrder,LogGroup::m_vDisplayInfo);
                AnalyAlarmInfoFromFile(sOrder,LogGroup::m_vDisplayInfo);
                if(m_iDisMode==1)
                {
                    if(!DisplayDataInfo(m_iLine))
                        continue;
                    bfind = true;
                }
            }                
        }
    }
    if(m_iDisMode==1)
    {
        if(bfind==false)
            printf("**************没有查询到符合的信息**************\n");
    }
    qry.close();
    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return false;
    }
    return bfind; 
}

bool LogGroup::CheckParamProcess()
{
    char sSql[1024]={0};
    int cnt=0;

    DEFINE_QUERY(qry);
    try
    {
    sprintf(sSql,"select count(*) from wf_application a where Upper(a.exec_name) like Upper('%s')",m_sParam);
    qry.setSQL(sSql);
    qry.open ();
    if(qry.next())
        cnt = qry.field(0).asInteger();
    qry.close();
    if(cnt==0)
    {
        printf("------------Err Msg------------\r\n");
        printf("\n找不到进程名称为[%s]的相关记录,请查询进程定义列表[wf_application]获取正确的进程名\n\n",m_sParam);
        return false;
    }
    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return false;
    }
    return true;
}

bool LogGroup::CheckProcessForAlarm()
{

    char sSql[1024]={0};
    int cnt=0;
    bool bfind=false;

    DEFINE_QUERY(qry);
    try
    {
    sprintf(sSql,"select distinct(a.process_id) from wf_process a, wf_application b where "
        "a.app_id=b.app_id and Upper(b.exec_name) like Upper('%s')",m_sParam);
    qry.setSQL(sSql);
    qry.open ();
    while(qry.next())
    {
        m_iModuleId = qry.field(0).asInteger();
        if(m_iModuleId>0)
        {
            char sOrder[20]={0};
            memset(sOrder,0,sizeof(sOrder));
            sprintf(sOrder,"%d",m_iModuleId);
            m_poLogGroup->AnalyzesDataInfo(OPERATIONLOG,DISALARM,sOrder,m_vDisplayInfo);
            if(!DisplayDataInfo(m_iLine))
                continue;
            bfind = true;
        }
    }
    qry.close();
    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return false;
    }
    return bfind;

}

int LogGroup::CheckParamClass()
{
    char sSql[1024]={0};
    int itypeid = -1;

    DEFINE_QUERY(qry);
    try
    {
    sprintf(sSql,"select log_type from b_class_define a where Upper(a.log_class) like Upper('%s')",m_sParam);
    qry.setSQL(sSql);
    qry.open ();
    if(qry.next())
        itypeid = qry.field(0).asInteger();
    qry.close();

    if(itypeid==-1)
    {
        printf("------------Err Msg------------\r\n");
        printf("\n找不到日志类别为[%s]的相关记录,请查询日志类别定义列表[b_class_define]获取正确的日志类别名\n\n",m_sParam);
    }
    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
    }
    return itypeid;
}


bool LogGroup::CheckParamName()
{
    char ParamSec[50],ParamKey[50];

    memset(ParamSec,0,sizeof(ParamSec));
    memset(ParamKey,0,sizeof(ParamKey));
    
    char *p = strchr(m_sParam,'.');
    if(!p)
        return false;
    strncpy(ParamSec,m_sParam,p-m_sParam);
    p++;
    strncpy(ParamKey,p,strlen(p));

    if(!m_poLogGroup->m_poCmdCom)
    {
        printf("获取核心参数失败");
        return false;
    }
    if(!m_poLogGroup->m_poCmdCom->GetInitParam(ParamSec,ParamKey))
        return false;
    return true;
}

void LogGroup::DisplayLogInfo(char *pFile,char *pPath)
{

	char scmd[600]={0},sFile[500]={0};
    char tempFileName_1[500]={0},tempFileName_2[500]={0};
    struct stat statbuf;
    int cnt=0;
    bool bFind=false;
    
    if (pPath[strlen(pPath)-1] != '/')
    {
        strcat(pPath, "/");
    }
    sprintf(tempFileName_1,"%s%s",pPath,pFile);
    if(lstat(tempFileName_1, &statbuf)<0)
    {
        Log::log (0, "检索信息失败");
        return;
    }
    if(0 == statbuf.st_size)
    {
        Log::log (0, "获取数据为空");
        return;
    }  
    sprintf(tempFileName_2,"%sLogGroupTemp.log",pPath);
    sprintf(scmd,"tail -n%d %s > %s",m_iLine+2,tempFileName_1,tempFileName_2);
    system(scmd);

    ifstream infofile(tempFileName_2);
    
    if(!infofile)
    {
        Log::log (0, "检索信息失败");
        return;
    }
    cout<<"*************************************"<<endl;
    while(!infofile.eof())
    {
        string::size_type pos = 0;
        infofile.getline(sFile,sizeof(sFile));
        if(infofile.gcount()==0)	
        {// 什么都没有读到
            continue;
        }
        int len = strlen(sFile);
        if(len==0)
            continue;
        if((cnt++)==m_iLine)
            break;
        string strFile = sFile;
        if(bFind)
        {
            cout<<strFile<<endl;
            continue;
        }
        pos = strFile.find_first_of( '[', pos );
        if(pos == string::npos)
            continue;
        bFind = true;
        string temp = strFile.substr( pos, len - pos );
        cout<<temp<<endl;
     }
    cout<<"*************************************"<<endl;
    memset(scmd,0,sizeof(scmd));
    sprintf(scmd,"rm %s",tempFileName_2);
    system(scmd); 
    return ;
}

void LogGroup::DisplayInfoByModuleDB()
{
    char sSql[1024]={0};
    int cnt=0;

    DEFINE_QUERY(qry);
    try
    {
    if(strlen(m_sParam)==0)
        sprintf(sSql,"select to_char(a.LOG_TIME,'yyyy-mm-dd hh24:mi:ss'),a.MODULE_ID,b.MODULE_NAME,a.PROCESS_ID,a.FILE_NAME,"
                    "a.LOG_FUNCTION,a.ERROR_ID,a.MSG from B_ALARM_LOG a,WF_MODULE b where a.module_id=b.module_id "
                    "and rownum<%d order by LOG_TIME desc ",m_iLine);
    else
        sprintf(sSql,"select to_char(a.LOG_TIME,'yyyy-mm-dd hh24:mi:ss'),a.MODULE_ID,b.MODULE_NAME,a.PROCESS_ID"
                    ",a.FILE_NAME,a.LOG_FUNCTION,a.ERROR_ID,a.MSG from B_ALARM_LOG a ,WF_MODULE b "
                    "where Upper(b.MODULE_NAME) like Upper('%s') and a.module_id=b.module_id and  rownum<%d order by LOG_TIME desc ",m_sParam,m_iLine);
    qry.setSQL(sSql);
    qry.open ();
    printf("---------------------------------------------------------系统模块日志信息-----------------------------------------------------------\r\n\n");
    while(qry.next())
    {
        cnt++;
        printf("[LOG_TIME]%s[MODULE_ID]%d[MODULE_NAME]%s[PROCESS_ID]%d"
               "[FILE_NAME]%s[LOG_FUNCTION]%s[ERROR_ID]%s"
               "[MSG]%s\r\n"
               ,qry.field(0).asString(),qry.field(1).asInteger(),qry.field(2).asString()
               ,qry.field(3).asInteger(),qry.field(4).asString(),qry.field(5).asString()
               ,qry.field(6).asString(),qry.field(7).asString());
    }
    qry.close();
    if(cnt==0)
        printf("没有该模块名(%s)的错误，告警等相关信息\n",m_sParam);
    printf("\n");

    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return;
    }
    return;
}

void LogGroup::DisplayInfoByProcessDB()
{
    char sSql[1024]={0};
    int cnt=0;

    DEFINE_QUERY(qry);
    try
    {
    if(strlen(m_sParam)==0)
        sprintf(sSql,"select to_char(LOG_TIME,'yyyy-mm-dd hh24:mi:ss'),PROCESS_ID,b.EXEC_NAME,FILE_NAME,"
                    "LOG_FUNCTION,ERROR_ID,MSG from B_ALARM_LOG a,WF_APPLICATION b where a.module_id=b.module_id "
                    "and rownum<%d order by LOG_TIME desc ",m_iLine);
    else
        sprintf(sSql,"select to_char(a.LOG_TIME,'yyyy-mm-dd hh24:mi:ss'),a.PROCESS_ID,b.EXEC_NAME"
                    ",a.FILE_NAME,a.LOG_FUNCTION,a.ERROR_ID,a.MSG from B_ALARM_LOG a ,WF_APPLICATION b "
                    "where Upper(b.exec_name) like Upper('%s') and a.module_id=b.module_id and  rownum<%d order by LOG_TIME desc ",m_sParam,m_iLine);
    qry.setSQL(sSql);
    qry.open ();
    printf("------------------------------------------------------系统进程错误/告警日志信息-----------------------------------------------------\r\n\n");
    while(qry.next())
    {
        cnt++;
        printf("[LOG_TIME]%s[PROCESS_ID]%d[PRCESS_NAME]%s"
               "[FILE_NAME]%s[LOG_FUNCTION]%s[ERROR_ID]%s"
               "[MSG]%s\r\n"
               ,qry.field(0).asString(),qry.field(1).asInteger(),qry.field(2).asString()
               ,qry.field(3).asString(),qry.field(4).asString()
               ,qry.field(5).asString(),qry.field(6).asString());
    }
    qry.close();
    if(cnt==0)
        printf("没有该进程名(%s)的错误，告警等相关信息\n",m_sParam);
    printf("\n");
    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return;
    }
    return;
}

void LogGroup::DisplayCheckPointDB()
{
    char sSql[1024]={0};
    int cnt=0;

    DEFINE_QUERY(qry);
    try
    {
    sprintf(sSql,"select to_char(TIME_FLAG,'yyyy-mm-dd hh24:mi:ss'),BATCH_ID,DATA_TYPE from B_CHECK_POINT_RECORD where ROWNUM<%d",m_iLine);
    qry.setSQL(sSql);
    qry.open ();
    printf("-------------CheckPoint模块日志信息统计-----------------\r\n\n");
    while(qry.next())
    {
        cnt++;
        printf("[TIME_FLAG]%s[BATCH_ID]%s[DATA_TYPE]%s\r\n"
               ,qry.field(0).asString(),qry.field(1).asString(),qry.field(2).asString());
    }
    qry.close();
    if(cnt==0)
        printf("目前没有CHECKPOINT的相关日志信息\n");
    printf("\n");
    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return;
    }
    return;
}

void LogGroup::DisplayStaffidDB()
{
    char sSql[1024]={0};
    int cnt=0;

    DEFINE_QUERY(qry);
    try
    {
    sprintf(sSql,"select Upper(a.staff_id),to_char(a.operate_date,'yyyy-mm-dd hh24:mi:ss'),"
                 "Upper(a.table_name),a.operate_type,a.operate_result,"
                 "a.operate_cmd from HBACCESS_OPERATE_LOG A "
                 "where Upper(a.staff_id) like Upper(%s) and ROWNUM<%d",m_sParam,m_iLine);
    qry.setSQL(sSql);
    qry.open ();
    printf("------------------------------------------------------数据库操作查询信息-----------------------------------------------------\r\n\n");
    while(qry.next())
    {
        cnt++;
        printf("STAFF_ID:[%s] OPERATE_DATE:[%s] TABLE_NAME:[%s] OPERATE_TYPE:[%d] OPERATE_RESULT:[%s] OPERATE_CMD:[%s]\r\n"
               ,qry.field(0).asString(),qry.field(1).asString(),qry.field(2).asString(),
               qry.field(3).asInteger(),qry.field(4).asString(),qry.field(5).asString());
    }
    qry.close();
    if(cnt==0)
        printf("没有工号[%s]的相关操作日志信息\n\n",m_sParam);
    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return;
    }
    return;  
}

void LogGroup::DisplayInfoByTableNameDB()
{
    char sSql[1024]={0};
    int cnt=0;

    DEFINE_QUERY(qry);
    try
    {
    sprintf(sSql,"select Upper(a.staff_id),to_char(a.operate_date,'yyyy-mm-dd hh24:mi:ss'),"
                 "Upper(a.table_name),a.operate_type,a.operate_result,"
                 "a.operate_cmd from HBACCESS_OPERATE_LOG A "
                 "where Upper(a.table_name) like Upper('%s') and ROWNUM<%d",m_sParam,m_iLine);
    qry.setSQL(sSql);
    qry.open ();
    printf("------------------------------------------------------数据库操作查询信息-----------------------------------------------------\r\n\n");
    while(qry.next())
    {
        cnt++;
        printf("TABLE_NAME:[%s] STAFF_ID:[%s] OPERATE_DATE:[%s]  OPERATE_TYPE:[%d] OPERATE_RESULT:[%s] OPERATE_CMD:[%s]\r\n"
               ,qry.field(2).asString(),qry.field(0).asString(),qry.field(1).asString(),
               qry.field(3).asInteger(),qry.field(4).asString(),qry.field(5).asString());
    }
    qry.close();
    if(cnt==0)
        printf("没有表名[%s]的相关操作日志信息\n\n",m_sParam);
    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return;
    }
    return; 
}

void LogGroup::DisplayInfoBySqlTypeDB()
{
    char sSql[1024]={0};
    int cnt=0;
    int itypeid=0;

    if(strcmp(m_sParam,"insert")==0)
        itypeid = 1;
    else if(strcmp(m_sParam,"delete")==0)
        itypeid = 2;
    else if(strcmp(m_sParam,"update")==0)
        itypeid = 3;
    else if(strcmp(m_sParam,"drop")==0)
        itypeid = 4;
    
    DEFINE_QUERY(qry);
    try
    {
    sprintf(sSql,"select Upper(a.staff_id),to_char(a.operate_date,'yyyy-mm-dd hh24:mi:ss'),"
                 "Upper(a.table_name),a.operate_type,a.operate_result,"
                 "a.operate_cmd from HBACCESS_OPERATE_LOG A "
                 "where a.operate_type=%d and ROWNUM<%d",itypeid,m_iLine);
    qry.setSQL(sSql);
    qry.open ();
    printf("------------------------------------------------------数据库操作查询信息-----------------------------------------------------\r\n\n");
    while(qry.next())
    {
        cnt++;
        printf("OPERATE_TYPE:[%d] TABLE_NAME:[%s] STAFF_ID:[%s] OPERATE_DATE:[%s] OPERATE_RESULT:[%s] OPERATE_CMD:[%s]\r\n"
               ,qry.field(3).asInteger(),qry.field(2).asString(),qry.field(0).asString(),qry.field(1).asString(),
               qry.field(4).asString(),qry.field(5).asString());
    }
    qry.close();
    if(cnt==0)
        printf("没有操作类型[%s]的相关操作日志信息\n\n",m_sParam);
    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return;
    }
    return;   
}


bool LogGroup::DisplayDataInfo(int idisNum,bool bspecify)
{
//    vector<string>::reverse_iterator r_iter;
    vector<string>::iterator iter;
    int ifindcnt=0;
    
    if(m_vDisplayInfo.size()==0)
    {
//        cout<<"**************没有查询到符合的信息**************"<<endl;
        return false;
    }
//    for(r_iter=m_vDisplayInfo.rbegin();r_iter!=m_vDisplayInfo.rend();++r_iter)
    for(iter=m_vDisplayInfo.begin();iter!=m_vDisplayInfo.end();++iter)
    {
        if(ifindcnt>=idisNum)
        {
            if(bspecify==true)
                break;
            if(!IfCDisplayInfo())
                break;
            ifindcnt = 0;
        }
        cout<<*iter<<endl;;
        ifindcnt++;
    }
    cout<<endl;
    m_vDisplayInfo.clear();
    return true;
}


int LogGroup::DealForModule()
{
    if(strlen(m_sParam)>0)
    {
        if(!CheckParamModule())
            return 1;
    }
    if(m_bQryFile)
    {
//        DisplayInfoByModuleDB();
        if(m_iDisMode==1)
            printf("------------------------------------------------------系统模块告警日志信息----------------------------------------------------------\r\n\n");
        CheckModuleForMem();
        if(m_iDisMode==2)
            DisplayForInterface(GROUPMODULE);
    }
    else
    {
        if(m_iDisMode==1)
            printf("------------------------------------------------------系统模块告警日志信息----------------------------------------------------------\r\n\n");
        CheckModuleForMem();
        if(m_iDisMode==2)
            DisplayForInterface(GROUPMODULE);
    }
    return 0;
}

int LogGroup::DealForProcess()
{
    if(strlen(m_sParam)>0)
    {
        if(!CheckParamProcess())
            return 1;
    }
    if(m_bQryFile)
    {
        if(strlen(m_sParam))
            AnalyInfoByFile(ATTEMPERLOG,-1,m_sParam,LogGroup::m_vDisplayInfo);
        else
             AnalyInfoByFile(ATTEMPERLOG,DISPLAYALL,m_sParam,LogGroup::m_vDisplayInfo);
    }
    else
    {
        if(strlen(m_sParam))
        {
            m_poLogGroup->AnalyzesDataInfo(ATTEMPERLOG,-1,m_sParam,m_vDisplayInfo);
        }
        else
            m_poLogGroup->AnalyzesDataInfo(ATTEMPERLOG,DISPLAYALL,m_sParam,m_vDisplayInfo);
    }
    if(m_iDisMode==1)
    {
        printf("------------------------------------------------------系统进程调度日志信息----------------------------------------------------------\r\n\n");
        if(!DisplayDataInfo(m_iLine))
                printf("**************没有查询到符合的信息**************\n");
        
        printf("------------------------------------------------------系统进程告警日志信息----------------------------------------------------------\r\n\n");
    }
    if(m_bQryFile)
    {
//      DisplayInfoByProcessDB();
        char sOrder[50]={0};
        GetModuleIdByName(m_sParam,sOrder);
        AnalyInfoByFile(OPERATIONLOG,DISALARM,sOrder,LogGroup::m_vAlarmInfo);
        AnalyAlarmInfoFromFile(m_sParam,LogGroup::m_vAlarmInfo);
    }
    else
    {
        char sOrder[50]={0};
        GetModuleIdByName(m_sParam,sOrder);
        if(m_iDisMode==1)
        {
            m_poLogGroup->AnalyzesDataInfo(OPERATIONLOG,DISALARM,sOrder,m_vDisplayInfo);
            AnalyAlarmInfoFromFile(sOrder,LogGroup::m_vDisplayInfo);
        }
        else if(m_iDisMode==2)
        {
            m_poLogGroup->AnalyzesDataInfo(OPERATIONLOG,DISALARM,sOrder,m_vAlarmInfo);
            AnalyAlarmInfoFromFile(sOrder,LogGroup::m_vAlarmInfo);
        }
    }
    if(m_iDisMode==1)
    {
        if(!DisplayDataInfo(m_iLine))
            printf("**************没有查询到符合的信息**************\n"); 
    }
    else if(m_iDisMode==2)
        DisplayForInterface(GROUPPROCESS);
    
    return 0;
}

int LogGroup::DealForParam()
{
    if(strlen(m_sParam))
    {
        if(!CheckParamName())
        {
            printf("------------Err Msg------------\r\n");
            printf("\n找不到参数名称为[%s]的相关记录,请查询核心参数配置文件获取正确的参数名\n\n",m_sParam);
            return 1;
        }
    }
    if(m_bQryFile)
    {
//        DisplayInfoBySqlTypeDB();
        if(strlen(m_sParam))
        {
            AnalyInfoByFile(PARAMLOG,-1,m_sParam,LogGroup::m_vDisplayInfo);
        }
        else
            AnalyInfoByFile(PARAMLOG,DISPLAYALL,m_sParam,LogGroup::m_vDisplayInfo);
    }
    else
    {
//        vector<string> vParam;
//        CmdMsgFactory *pCmdMsg = new CmdMsgFactory();
//        pCmdMsg->GetHistoryMsgasString(vParam,NULL,NULL);
//        AnalyParamInfo(PARAMLOG,m_sParam,vParam,m_vDisplayInfo);
        if(strlen(m_sParam))
        {
            m_poLogGroup->AnalyzesDataInfo(PARAMLOG,DISBYPARAM,m_sParam,m_vDisplayInfo);
        }
        else
            m_poLogGroup->AnalyzesDataInfo(PARAMLOG,DISPLAYALL,m_sParam,m_vDisplayInfo);
    }
    if(m_iDisMode==1)
    {
        printf("------------------------------------------------------系统参数管理日志信息----------------------------------------------------------\r\n\n");
        if(!DisplayDataInfo(m_iLine))
            printf("**************没有查询到符合的信息**************\n");
    }
    else if(m_iDisMode==2)
        DisplayForInterface(GROUPPARAM);

    return 0;
}

int LogGroup::DealForLevel()
{
    int ilevel = atoi(m_sParam);
    if(ilevel<0 || ilevel>3)
    {
        printf("------------Err Msg------------\r\n");
        printf("\n指定日志等级出错，目前日志等级分为3级,请输入类似hblogg -level[1/2/3]展示\n");
        return 1;
    }
    if(m_bQryFile)
    {
        AnalyInfoByFile(OPERATIONLOG,PINFOPOINT,m_sParam,LogGroup::m_vDisplayInfo);
    }
    else
    {
        m_poLogGroup->AnalyzesDataInfo(OPERATIONLOG,PINFOPOINT,m_sParam,m_vDisplayInfo);
    }
    if(m_iDisMode==1)
    {
        printf("------------------------------------------------------系统业务进程日志信息----------------------------------------------------------\r\n\n");
        if(!DisplayDataInfo(m_iLine))
            printf("**************没有查询到符合的信息**************\n");
    }
    else if(m_iDisMode==2)
        DisplayForInterface(GROUPINFOLV);

    return 0;
}

int LogGroup::DealForTableName()
{
    if(m_bQryFile)
    {
//      DisplayInfoByTableNameDB();
        AnalyInfoByFile(HBACCESS,-1,m_sParam,LogGroup::m_vDisplayInfo);
    }
    else
    {
        m_poLogGroup->AnalyzesDataInfo(HBACCESS,-1,m_sParam,m_vDisplayInfo);
    }
    if(m_iDisMode==1)
    {
        printf("------------------------------------------------------系统数据库修改日志信息--------------------------------------------------------\r\n\n");
        if(!DisplayDataInfo(m_iLine))
            printf("**************没有查询到符合的信息**************\n");
    }
    else if(m_iDisMode==2)
        DisplayForInterface(GROUPTABNAME);
    
    return 0;
}

int LogGroup::DealForSqlType()
{
    if(m_bQryFile)
    {
        AnalyInfoByFile(HBACCESS,-1,m_sParam,LogGroup::m_vDisplayInfo);             
    }
    else
    {
        m_poLogGroup->AnalyzesDataInfo(HBACCESS,-1,m_sParam,m_vDisplayInfo);
    }
    if(m_iDisMode==1)
    {
        printf("------------------------------------------------------系统数据库修改日志信息--------------------------------------------------------\r\n\n");
        if(!DisplayDataInfo(m_iLine))
            printf("**************没有查询到符合的信息**************\n");
    }
    else if(m_iDisMode==2)
        DisplayForInterface(GROUPSQLTYPE);

    return 0;
}

int LogGroup::DealForClass()
{
    int LogType=0;
    char tempInfo[100]={0};
    if(m_iDisMode==2)
    {
        if(strlen(m_sParam))
        {
            LogType = CheckParamClass();
            if(LogType==-1)
                return 1;
            if(LogType==OPERATIONLOG)
            {
                if(m_bQryFile)
                {
                    AnalyInfoByFile(LogType,PINFOPOINT,NULL,LogGroup::m_vDisplayInfo);
                    AnalyInfoByFile(LogType,DISALARM,NULL,LogGroup::m_vAlarmInfo);
                    AnalyAlarmInfoFromFile(NULL,LogGroup::m_vAlarmInfo);
                    AnalyInfoByFile(LogType,DISBYDML,NULL,LogGroup::m_vDmlInfo);
                }
                else
                {
                    m_poLogGroup->AnalyzesDataInfo(OPERATIONLOG,PINFOPOINT,NULL,m_vDisplayInfo);
                    m_poLogGroup->AnalyzesDataInfo(OPERATIONLOG,DISALARM,NULL,m_vAlarmInfo);
                    AnalyAlarmInfoFromFile(NULL,LogGroup::m_vAlarmInfo);
                    m_poLogGroup->AnalyzesDataInfo(OPERATIONLOG,DISBYDML,NULL,m_vDmlInfo);
                }
                DisplayForInterface(DISOPERAT);
            }
            else if(LogType==PARAMLOG)
            {
                if(m_bQryFile)
                    AnalyInfoByFile(PARAMLOG,DISPLAYALL,m_sParam,LogGroup::m_vDisplayInfo);
                else
                {
//                    vector<string> vParam;
//                    CmdMsgFactory *pCmdMsg = new CmdMsgFactory();
//                    pCmdMsg->GetHistoryMsgasString(vParam,NULL,NULL);
//                    AnalyParamInfo(PARAMLOG,m_sParam,vParam,LogGroup::m_vParamInfo);
                    m_poLogGroup->AnalyzesDataInfo(PARAMLOG,DISPLAYALL,m_sParam,LogGroup::m_vDisplayInfo);
                }
                DisplayForInterface(DISPARAM);
            }
            else if (LogType==ATTEMPERLOG)
            {
                if(m_bQryFile)
                    AnalyInfoByFile(LogType,DISPLAYALL,NULL,LogGroup::m_vDisplayInfo);
                else
                    m_poLogGroup->AnalyzesDataInfo(ATTEMPERLOG,DISPLAYALL,NULL,m_vDisplayInfo);
                //此处借用module展现
                DisplayForInterface(DISATTEMP);
            }
            else if (LogType==SYSTEMLOG)
            {
                if(m_bQryFile)
                    AnalyInfoByFile(LogType,DISPLAYALL,NULL,LogGroup::m_vDisplayInfo);
                else
                {
//                    vector<string> vParam1;
//                    CmdMsgFactory *pCmdMsg = new CmdMsgFactory();
//                    pCmdMsg->GetHistoryMsgasString(vParam1,NULL,NULL);
//                    AnalyParamInfo(PARAMLOG,m_sParam,vParam1,LogGroup::m_vDisplayInfo);
                    m_poLogGroup->AnalyzesDataInfo(SYSTEMLOG,DISPLAYALL,NULL,LogGroup::m_vDisplayInfo);
                    m_poLogGroup->AnalyzesDataInfo(PARAMLOG,DISBYSTAFF,NULL,LogGroup::m_vDisplayInfo);
                }
                DisplayForInterface(DISSYS);
            }
            else if (LogType==HBACCESS)
            {//此处借用tablename展现
                if(m_bQryFile)
                    AnalyInfoByFile(LogType,DISPLAYALL,NULL,LogGroup::m_vDisplayInfo);
                else
                    m_poLogGroup->AnalyzesDataInfo(HBACCESS,DISPLAYALL,NULL,m_vDisplayInfo);
                DisplayForInterface(DISACCESS);
            }
            return 0;
        }
        if(m_bQryFile)
        {
            AnalyInfoByFile(OPERATIONLOG,PINFOPOINT,NULL,LogGroup::m_vDisplayInfo);
            AnalyInfoByFile(OPERATIONLOG,DISALARM,NULL,LogGroup::m_vAlarmInfo);
            AnalyAlarmInfoFromFile(NULL,LogGroup::m_vAlarmInfo);
            AnalyInfoByFile(OPERATIONLOG,DISBYDML,NULL,LogGroup::m_vDmlInfo);
            AnalyInfoByFile(PARAMLOG,DISPLAYALL,m_sParam,LogGroup::m_vParamInfo);
            AnalyInfoByFile(ATTEMPERLOG,DISPLAYALL,NULL,LogGroup::m_vAttemperInfo);
            AnalyInfoByFile(SYSTEMLOG,DISPLAYALL,NULL,LogGroup::m_vSysInfo);
            AnalyInfoByFile(HBACCESS,DISPLAYALL,NULL,LogGroup::m_vAccessInfo);
            DisplayForInterface(DISFORALL);
            return 0;
        }
        //业务日志
        m_poLogGroup->AnalyzesDataInfo(OPERATIONLOG,PINFOPOINT,NULL,m_vDisplayInfo);
        m_poLogGroup->AnalyzesDataInfo(OPERATIONLOG,DISALARM,NULL,m_vAlarmInfo);
        AnalyAlarmInfoFromFile(NULL,LogGroup::m_vAlarmInfo);
        m_poLogGroup->AnalyzesDataInfo(OPERATIONLOG,DISBYDML,NULL,m_vDmlInfo);
        //系统管理
        m_poLogGroup->AnalyzesDataInfo(SYSTEMLOG,DISPLAYALL,NULL,m_vSysInfo);
        //数据库修改
        m_poLogGroup->AnalyzesDataInfo(HBACCESS,DISPLAYALL,m_sParam,m_vAccessInfo);
        //参数管理
//        vector<string> vParam_2;
//        CmdMsgFactory *pCmdMsg = new CmdMsgFactory();
//        pCmdMsg->GetHistoryMsgasString(vParam_2,NULL,NULL);
//        AnalyParamInfo(PARAMLOG,NULL,vParam_2,LogGroup::m_vParamInfo);
          m_poLogGroup->AnalyzesDataInfo(PARAMLOG,DISPLAYALL,NULL,LogGroup::m_vParamInfo);
        //进程调度
        m_poLogGroup->AnalyzesDataInfo(ATTEMPERLOG,DISPLAYALL,NULL,m_vAttemperInfo);
        DisplayForInterface(DISFORALL);
        return 0;
    }
    
    if(strlen(m_sParam))
    {
        LogType = CheckParamClass();
        if(LogType==-1)
            return 1;
        if(m_bQryFile)
            AnalyInfoByFile(LogType,DISPLAYALL,m_sParam,LogGroup::m_vDisplayInfo);
        else
            m_poLogGroup->AnalyzesDataInfo(LogType,DISPLAYALL,m_sParam,m_vDisplayInfo);
        AnalyAlarmInfoFromFile(NULL,LogGroup::m_vDisplayInfo);
        if(LogType==OPERATIONLOG)
            strcpy(tempInfo,"业务进程处理日志");
        else if(LogType==PARAMLOG)
            strcpy(tempInfo,"参数管理日志");
        else if (LogType==ATTEMPERLOG)
            strcpy(tempInfo,"进程调度日志");
        else if (LogType==HBACCESS)
            strcpy(tempInfo,"数据库修改日志");
        else if (LogType==SYSTEMLOG)
            strcpy(tempInfo,"系统管理日志");
        printf("---------------------------------------------------------系统%s信息-----------------------------------------------------\r\n\n",tempInfo);
        if(!DisplayDataInfo(m_iLine))
            printf("**************没有查询到符合的信息**************\n");
        return 0;
    }
    printf("------------------------------------------------------系统业务进程日志信息----------------------------------------------------------\r\n\n");
    if(m_bQryFile)
    {
        AnalyInfoByFile(OPERATIONLOG,DISPLAYALL,NULL,LogGroup::m_vDisplayInfo);
    }
    else
    {
        m_poLogGroup->AnalyzesDataInfo(OPERATIONLOG,DISPLAYALL,NULL,m_vDisplayInfo);
        m_poLogGroup->AnalyzesDataInfo(OPERATIONLOG,DISBYDML,NULL,m_vDisplayInfo);
    }
    AnalyAlarmInfoFromFile(NULL,LogGroup::m_vDisplayInfo);
    if(!DisplayDataInfo(m_iLine,true))
        printf("**************没有查询到符合的信息**************\n");
    
    printf("------------------------------------------------------系统进程调度日志信息----------------------------------------------------------\r\n\n");
    if(m_bQryFile)
    {
        AnalyInfoByFile(ATTEMPERLOG,DISPLAYALL,NULL,LogGroup::m_vDisplayInfo);
    }
    else
    {
        m_poLogGroup->AnalyzesDataInfo(ATTEMPERLOG,DISPLAYALL,NULL,m_vDisplayInfo);
    }
    if(!DisplayDataInfo(m_iLine,true))
        printf("**************没有查询到符合的信息**************\n");
    
    printf("------------------------------------------------------系统管理日志信息--------------------------------------------------------------\r\n\n");
    if(m_bQryFile)
    {
        AnalyInfoByFile(SYSTEMLOG,DISPLAYALL,NULL,LogGroup::m_vDisplayInfo);
    }
    else
    {
        m_poLogGroup->AnalyzesDataInfo(SYSTEMLOG,DISPLAYALL,NULL,m_vDisplayInfo);
    }
    if(!DisplayDataInfo(m_iLine,true))
        printf("**************没有查询到符合的信息**************\n");
    
    printf("------------------------------------------------------系统参数管理日志信息----------------------------------------------------------\r\n\n");
    if(m_bQryFile)
    {
        AnalyInfoByFile(PARAMLOG,DISPLAYALL,NULL,LogGroup::m_vDisplayInfo);
    }
    else
    {
//        vector<string> vParam;
//        CmdMsgFactory *pCmdMsg = new CmdMsgFactory();
//        pCmdMsg->GetHistoryMsgasString(vParam,NULL,NULL);
//        AnalyParamInfo(PARAMLOG,m_sParam,vParam,m_vDisplayInfo);
          m_poLogGroup->AnalyzesDataInfo(PARAMLOG,DISPLAYALL,NULL,LogGroup::m_vDisplayInfo);
    }
    if(!DisplayDataInfo(m_iLine,true))
        printf("**************没有查询到符合的信息**************\n");
    
    printf("------------------------------------------------------系统数据库修改日志信息--------------------------------------------------------\r\n\n");
    if(m_bQryFile)
    {
        AnalyInfoByFile(HBACCESS,DISPLAYALL,NULL,LogGroup::m_vDisplayInfo);
    }
    else
    {
        m_poLogGroup->AnalyzesDataInfo(HBACCESS,DISPLAYALL,NULL,m_vDisplayInfo);
    }
    if(!DisplayDataInfo(m_iLine,true))
        printf("**************没有查询到符合的信息**************\n");
    
    return 0;
}

int LogGroup::DealForCheckPoint()
{
    if(m_bQryFile)
    {
//      DisplayCheckPointDB();
        AnalyInfoByFile(SYSTEMLOG,DISBYCHECK,NULL,LogGroup::m_vDisplayInfo);
    }
    else
    {
        m_poLogGroup->AnalyzesDataInfo(SYSTEMLOG,DISBYCHECK,NULL,m_vDisplayInfo);
    }
    if(m_iDisMode==0)
    {
        printf("------------------------------------------------------系统管理日志信息--------------------------------------------------------------\r\n\n");
        if(!DisplayDataInfo(m_iLine))
            printf("**************没有查询到符合的信息**************\n");
    }
    else if(m_iDisMode==2)
        DisplayForInterface(GROUPCHECKPT);

    return 0;
}

int LogGroup::DealForStaffId()
{
    printf("------------------------------------------------------系统工号管理日志信息--------------------------------------------------------\r\n\n");
    if(m_bQryFile)
    {
//        DisplayStaffidDB();
        AnalyInfoByFile(SYSTEMLOG,-1,m_sParam,LogGroup::m_vDisplayInfo);
        if(m_iDisMode==1)
        {
            if(!DisplayDataInfo(m_iLine))
                printf("**************没有查询到符合的信息**************\n");
        }
    }
    else
    {
//        bool bFind = true;
//        vector<string> vParam;
//        CmdMsgFactory *pCmdMsg = new CmdMsgFactory();
//        pCmdMsg->GetHistoryMsgasString(vParam,NULL,NULL);
//        AnalyParamInfo(SYSTEMLOG,m_sParam,vParam,m_vDisplayInfo);
        m_poLogGroup->AnalyzesDataInfo(PARAMLOG,DISBYSTAFF,m_sParam,LogGroup::m_vDisplayInfo);
//        if(m_iDisMode==1)
//        {
//            if(!DisplayDataInfo(m_iLine))
//                bFind = false;
//        }
        m_poLogGroup->AnalyzesDataInfo(HBACCESS,-1,m_sParam,m_vDisplayInfo);
        if(m_iDisMode==1)
        {
            if(!DisplayDataInfo(m_iLine))
                printf("**************没有查询到符合的信息**************\n");
        }
    }
    if(m_iDisMode==2)
        DisplayForInterface(GROUPSTAFFID);

    return 0;
}

bool LogGroup::prepare(char g_argv[ARG_NUM][ARG_VAL_LEN],int g_argc)
{  
    int i=0;

    if(g_argc== 1)
    {
        printUsage();
        return false;
    }
    
    while (i<g_argc) 
    {
        if (g_argv[i][0] != '-') 
        {
            i++;
            continue;
        }
        int ilen = strlen(g_argv[i]);
        switch(g_argv[i][1])
        {
             case 'a':
                    {//日志组归档
                        if(strcmp(g_argv[i],"-a")!=0)
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确（不支持的选项）");
                            THROW(MBC_Log_Group+9);
                        }
                        m_iMode = PIGEONHOLE;
                    }
                    break;
             case 'b':
                    {//备份
                        if(strcmp(g_argv[i],"-b")!=0)
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                            THROW(MBC_Log_Group+9);
                        }
                        m_iMode = GROUPBAK;
                        if(strlen(g_argv[++i]))
                        {
                            m_iGroupId = atoi(g_argv[i]);
                            if(m_iGroupId<1 || m_iGroupId>3)
                            {
                                ALARMLOG28(0,MBC_CLASS_Parameter,169,"%s","应用进程参数不正确(错误的取值),日志组标识取值错误");
                                THROW(MBC_Log_Group+9);
                            }
                        }
                        else
                            m_iGroupId = -1;
                        if(strlen(g_argv[++i]))
                            strcpy(m_sPath,g_argv[i]);
                    }
                    break;
             case 'c':
                    {//创建共享内存
                        if(strncmp(g_argv[i],"-checkpoint",11)==0)
                        {
                            m_iMode = GROUPCHECKPT;
                            if(strcmp(g_argv[i],"-checkpoint")!=0)
                            {
                                ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                                THROW(MBC_Log_Group+9);
                            }
                            if(strlen(g_argv[++i]))
                            {
                                if(strcmp(g_argv[i],"-file")==0)
                                {
                                    m_bQryFile = true;
                                    if(strlen(g_argv[++i]))
                                        m_iLine = atoi(g_argv[i]);
                                    else
                                        m_iLine = LIENUM;
                                }
                                else
                                {
                                    m_iLine = atoi(g_argv[i]);
                                }
                            }
                            else
                                m_iLine = LIENUM;
                        }
                        else if(strncmp(g_argv[i],"-class",6)==0)
                        {
                            m_iMode = GROUPCLASS;
                            if(strcmp(g_argv[i],"-class")!=0)
                            {//解析【】获取参数
                                if(g_argv[i][6]=='[' && g_argv[i][ilen-1]==']')
                                    AnalyzesParam(g_argv[i],m_sParam);
                                else
                                {
                                    ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                                    THROW(MBC_Log_Group+9);
                                }
                            }
                            if(strlen(g_argv[++i]))
                            {
                                if(strcmp(g_argv[i],"-file")==0)
                                {
                                    m_bQryFile = true;
                                    if(strlen(g_argv[++i]))
                                        m_iLine = atoi(g_argv[i]);
                                    else
                                        m_iLine = LIENUM;
                                }
                                else
                                {
                                    m_iLine = atoi(g_argv[i]);
                                }
                            }
                            else
                                m_iLine = LIENUM;;
                        }
                        else
                        {
                            if(strcmp(g_argv[i],"-c")!=0)
                            {
                                ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                                THROW(MBC_Log_Group+9);
                            }
                            m_iMode = GROUPCREAT;
                        }
                    }
                    break;
             case 'd':
                    {//删除日志组文件
                        if(strcmp(g_argv[i],"-d")!=0)
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                            THROW(MBC_Log_Group+9);
                        }
                        m_iMode = GROUPDELETE;
                        if(strlen(g_argv[++i]))
                        {
                            m_iGroupId = atoi(g_argv[i]);
                            if(m_iGroupId<1 ||m_iGroupId>3)
                            {
                                ALARMLOG28(0,MBC_CLASS_Parameter,169,"%s","应用进程参数不正确(错误的取值),日志组标识取值错误");
                                THROW(MBC_Log_Group+9);
                            }
                        }
                        else
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,167,"%s","应用进程参数不正确(缺少必选项)，请指定日志组");
                            THROW(MBC_Log_Group+9);
                        }
                        if(strlen(g_argv[++i]))
                            strcpy(m_sFileName,g_argv[i]);
                        else
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,167,"%s","应用进程参数不正确(缺少必选项),请指定要删除的文件");
                            THROW(MBC_Log_Group+9);
                        }
                    }
                    break;
             case 'e':
                    {//设置归档标识
                        if(strcmp(g_argv[i],"-e")!=0)
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                            THROW(MBC_Log_Group+9);
                        }
                        m_iMode = GROUPSET;
                        if(strlen(g_argv[++i]))
                        {
                            m_bPigeonhole = atoi(g_argv[i]);
                            int phole = atoi(g_argv[i]);
                            if(phole<0 || phole>1)
                            {
                                ALARMLOG28(0,MBC_CLASS_Parameter,169,"%s","应用进程参数不正确(错误的取值),归档参数取值错误");
                                THROW(MBC_Log_Group+9);
                            }
                        }
                        else
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,167,"%s","应用进程参数不正确(缺少必选项)，请指定归档参数");
                            THROW(MBC_Log_Group+9);
                        } 
                    }
                    break;
             case 'f':
                    {//释放共享内存
                        if(strcmp(g_argv[i],"-f")!=0)
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                            THROW(MBC_Log_Group+9);
                        }
                        m_iMode = GROUPFREE;
                    }
                    break;
             case 'h':
                    {//帮助信息
                        if(strcmp(g_argv[i],"-help")!=0)
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                            THROW(MBC_Log_Group+9);
                        }
                    }
                    break;
             case 'i':
                    {//添加文件
                        if(strcmp(g_argv[i],"-i")!=0)
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                            THROW(MBC_Log_Group+9);
                        }
                        m_iMode = GROUPADD;
                        if(strlen(g_argv[++i]))
                        {
                            if(strcmp(g_argv[i],"all"))
                            {
                                m_iGroupId = atoi(g_argv[i]);
                                if(m_iGroupId<1 ||m_iGroupId>3)
                                {
                                    ALARMLOG28(0,MBC_CLASS_Parameter,169,"%s","应用进程参数不正确(错误的取值),日志组标识取值错误");
                                    THROW(MBC_Log_Group+9);
                                }
                            }
                            else
                                m_iGroupId = 0;
                        }
                        else
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,167,"%s","应用进程参数不正确(缺少必选项)，请指定日志组标识");
                            THROW(MBC_Log_Group+9);
                        }
                        if(strlen(g_argv[++i]))
                            strcpy(m_sPath,g_argv[i]);
                    }
                    break;
             case 'j':
                    {//加载checkpoint
                        if(strcmp(g_argv[i],"-j")!=0)
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                            THROW(MBC_Log_Group+9);
                        }
                        m_iMode = FILECLOAD;
                        if(strlen(g_argv[++i]))
                            strcpy(m_sBatch,g_argv[i]);
                        else
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,167,"%s","应用进程参数不正确(缺少必选项),请指定批次号");
                            THROW(MBC_Log_Group+9);
                        }
                    }
                    break;
             case 'l':
                    {//打印当前日志组信息
                        if(strncmp(g_argv[i],"-level",6)==0)
                        {
                            m_iMode = GROUPINFOLV;
                            if(strcmp(g_argv[i],"-level")!=0)
                            {//解析【】获取参数ID
                                if(g_argv[i][6]=='[' && g_argv[i][ilen-1]==']')
                                    AnalyzesParam(g_argv[i],m_sParam);
                                else
                                {
                                    ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                                    THROW(MBC_Log_Group+9);
                                }
                            }
                            if(strlen(g_argv[++i]))
                            {
                                if(strcmp(g_argv[i],"-file")==0)
                                {
                                    m_bQryFile = true;
                                    if(strlen(g_argv[++i]))
                                        m_iLine = atoi(g_argv[i]);
                                    else
                                        m_iLine = LIENUM;
                                }
                                else
                                {
                                    m_iLine = atoi(g_argv[i]);
                                }
                            }
                            else
                                m_iLine = LIENUM;
                        }
                        else
                        {
                            if(strcmp(g_argv[i],"-l")!=0)
                            {
                                ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                                THROW(MBC_Log_Group+9);
                            }
                            m_iMode = GROUPPRINTF;
                            if(strlen(g_argv[++i]))
                            {
                                m_iGroupId = atoi(g_argv[i]);
                                if(m_iGroupId<1 ||m_iGroupId>3)
                                {
                                    ALARMLOG28(0,MBC_CLASS_Parameter,169,"%s","应用进程参数不正确(错误的取值),日志组标识取值错误");
                                    THROW(MBC_Log_Group+9);
                                }
                            }
                        }
                    }
                    break;
             case 'm':
                    {
                        if(strncmp(g_argv[i],"-module",7)!=0)
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                            THROW(MBC_Log_Group+9);                            
                        }
                        m_iMode = GROUPMODULE;
                        if(strcmp(g_argv[i],"-module")!=0)
                        {//解析【】获取参数ID
                            if(g_argv[i][7]=='[' && g_argv[i][ilen-1]==']')
                                AnalyzesParam(g_argv[i],m_sParam);
                            else
                            {
                                ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                                THROW(MBC_Log_Group+9);
                            }
                        }
                        if(strlen(g_argv[++i]))
                        {
                            if(strcmp(g_argv[i],"-file")==0)
                            {
                                m_bQryFile = true;
                                if(strlen(g_argv[++i]))
                                    m_iLine = atoi(g_argv[i]);
                                else
                                    m_iLine = LIENUM;
                            }
                            else
                            {
                                m_iLine = atoi(g_argv[i]);
                            }
                        }
                        else
                            m_iLine = LIENUM;
                    }
                    break;
             case 'o':
                    {//输出最新信息
                        if(strcmp(g_argv[i],"-o")!=0)
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                            THROW(MBC_Log_Group+9);
                        }
                        m_iMode = GROUPOUT;
                        if(strlen(g_argv[++i]))
                        {
                            m_iGroupId= atoi(g_argv[i]);
                            if(m_iGroupId<1 ||m_iGroupId>3)
                            {
                                ALARMLOG28(0,MBC_CLASS_Parameter,169,"%s","应用进程参数不正确(错误的取值),日志组标识取值错误");
                                THROW(MBC_Log_Group+9);
                            }
                            if(strlen(g_argv[++i]))
                               m_iLine = atoi(g_argv[i]);
                            else
                               m_iLine = LIENUM;
                        }
                        else
                        {
                            m_iGroupId = 0;
                            m_iLine = LIENUM;
                        }
                    }
                    break;
             case 'p':
                    {
                        if(strncmp(g_argv[i],"-process",8)==0)
                        {
                            m_iMode = GROUPPROCESS;
                            if(strcmp(g_argv[i],"-process")!=0)
                            {//解析【】获取参数ID
                                if(g_argv[i][8]=='[' && g_argv[i][ilen-1]==']')
                                    AnalyzesParam(g_argv[i],m_sParam);
                                else
                                {
                                    ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                                    THROW(MBC_Log_Group+9);
                                }
                            }
                            if(strlen(g_argv[++i]))
                            {
                                if(strcmp(g_argv[i],"-file")==0)
                                {
                                    m_bQryFile = true;
                                    if(strlen(g_argv[++i]))
                                        m_iLine = atoi(g_argv[i]);
                                    else
                                        m_iLine = LIENUM;
                                }
                                else
                                {
                                    m_iLine = atoi(g_argv[i]);
                                }
                            }
                            else
                                m_iLine = LIENUM;
                        }
                        else if(strncmp(g_argv[i],"-param",6)==0)
                        {
                            m_iMode = GROUPPARAM;
                            if(strcmp(g_argv[i],"-param")!=0)
                            {
                                if(g_argv[i][6]=='[' && g_argv[i][ilen-1]==']')
                                    AnalyzesParam(g_argv[i],m_sParam);
                                else
                                {
                                    ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                                    THROW(MBC_Log_Group+9);
                                } 
                            }
                            if(strlen(g_argv[++i]))
                            {
                                if(strcmp(g_argv[i],"-file")==0)
                                {
                                    m_bQryFile = true;
                                    if(strlen(g_argv[++i]))
                                        m_iLine = atoi(g_argv[i]);
                                    else
                                        m_iLine = LIENUM;
                                }
                                else
                                {
                                    m_iLine = atoi(g_argv[i]);
                                }
                            }
                            else
                                m_iLine = LIENUM;
                        }
                    }
                    break;
             case 's':
                    {//切换日志组
                        if(strncmp(g_argv[i],"-staffid",8)==0)
                        {
                            m_iMode = GROUPSTAFFID;
                            if(strcmp(g_argv[i],"-staffid")!=0)
                            {//解析【】获取参数ID
                                if(g_argv[i][8]=='[' && g_argv[i][ilen-1]==']')
                                    AnalyzesParam(g_argv[i],m_sParam);
                                else
                                {
                                    ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                                    THROW(MBC_Log_Group+9);
                                }
                                if(strlen(g_argv[++i]))
                                {
                                    if(strcmp(g_argv[i],"-file")==0)
                                    {
                                        m_bQryFile = true;
                                        if(strlen(g_argv[++i]))
                                            m_iLine = atoi(g_argv[i]);
                                        else
                                            m_iLine = LIENUM;
                                    }
                                    else
                                    {
                                        m_iLine = atoi(g_argv[i]);
                                    }
                                }
                                else
                                    m_iLine = LIENUM;
                            }
                            else
                            {
                                ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                                THROW(MBC_Log_Group+9);                                
                            }
                        }
                        else if(strncmp(g_argv[i],"-sqltype",8)==0)
                        {
                            m_iMode = GROUPSQLTYPE;
                            if(strcmp(g_argv[i],"-sqltype")!=0)
                            {//解析【】获取参数ID
                                if(g_argv[i][8]=='[' && g_argv[i][ilen-1]==']')
                                    AnalyzesParam(g_argv[i],m_sParam);
                                else
                                {
                                    ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                                    THROW(MBC_Log_Group+9);
                                }
                                if(strlen(g_argv[++i]))
                                {
                                    if(strcmp(g_argv[i],"-file")==0)
                                    {
                                        m_bQryFile = true;
                                        if(strlen(g_argv[++i]))
                                            m_iLine = atoi(g_argv[i]);
                                        else
                                            m_iLine = LIENUM;
                                    }
                                    else
                                    {
                                        m_iLine = atoi(g_argv[i]);
                                    }
                                }
                                else
                                    m_iLine = LIENUM;
                            }
                            else
                            {
                                ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                                THROW(MBC_Log_Group+9);                                
                            }
                        }
                        else
                        {
                            if(strcmp(g_argv[i],"-s")!=0)
                            {
                                ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确(不支持的选项)");
                                THROW(MBC_Log_Group+9);
                            }
                            m_iMode = GROUPQUIT;
                            if(strlen(g_argv[++i]))
                            {
                                m_iGroupId = atoi(g_argv[i]);
                                if(m_iGroupId<1 ||m_iGroupId>3)
                                {
                                    ALARMLOG28(0,MBC_CLASS_Parameter,169,"%s","应用进程参数不正确(错误的取值),日志组标识取值错误");
                                    THROW(MBC_Log_Group+9);
                                }
                            }
                        }
                    }
                    break;
             case 'w':
                    {//落地checkpoint文件
                        if(strcmp(g_argv[i],"-w")!=0)
                        {
                            ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确（不支持的选项）");
                            THROW(MBC_Log_Group+9);
                        }
                        m_iMode = FILECDOWN;
                    }
                    break;
             case 't':
                    {
                        if(strncmp(g_argv[i],"-tablename",10)==0)
                        {
                            m_iMode = GROUPTABNAME;
                            if(strcmp(g_argv[i],"-tablename")!=0)
                            {//解析【】获取参数ID
                                if(g_argv[i][10]=='[' && g_argv[i][ilen-1]==']')
                                    AnalyzesParam(g_argv[i],m_sParam);
                                else
                                {
                                    ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确（不支持的选项）");
                                    THROW(MBC_Log_Group+9);
                                }
                                if(strlen(g_argv[++i]))
                                {
                                    if(strcmp(g_argv[i],"-file")==0)
                                    {
                                        m_bQryFile = true;
                                        if(strlen(g_argv[++i]))
                                            m_iLine = atoi(g_argv[i]);
                                        else
                                            m_iLine = LIENUM;
                                    }
                                    else
                                    {
                                        m_iLine = atoi(g_argv[i]);
                                    }
                                }
                                else
                                    m_iLine = LIENUM;
                            }
                            else
                            {
                                ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确（不支持的选项）");
                                THROW(MBC_Log_Group+9);                                
                            }
                        }
                        else
                        {
                            m_iMode = FILETEST;
                            if(strlen(g_argv[++i]))
                                m_iLine = atoi(g_argv[i]);
                            else
                                m_iLine = LIENUM;
                        }
                    }
                    break;
             default:
                    {
                        printUsage();
                        ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确（不支持的选项）");
                        return false;
                    }
                    break;
        }
        return true;
    }
    return true;
}

int LogGroup::Deal()
{
    switch(m_iMode)
    {
        case GROUPADD:
        {           
            if(m_iGroupId==0)
            {//初始加载，默认两个文件
                for(int num=1;num<=GROUP_LOG_NUMS;++num)
                {
                    if(!m_poLogGroup->AddLogGroupFile(num,m_sPath,true))
                        Log::log(0,"增加日志组文件失败");
                    if(!m_poLogGroup->AddLogGroupFile(num,m_sPath,true))
                        Log::log(0,"增加日志组文件失败");
                }
                Log::log(0,"增加日志组文件完成");
            }
            else
            {
                if(m_poLogGroup->AddLogGroupFile(m_iGroupId,m_sPath,true))
                    Log::log(0,"增加日志组文件成功");
                else
                    Log::log(0,"增加日志组文件失败");
            }
            break;
        }

        case GROUPBAK:
        {
            if(strlen(m_sPath)==0)
            {
                char *pPath = m_poLogGroup->GetGroupBakPath();
                strcpy(m_sPath,pPath);
            }
            m_poLogGroup->bakGroupFile(m_iGroupId,m_sPath);            
            Log::log(0,"日志组备份完成");
            break;
        }

        case GROUPCREAT:
        {
            if(m_poLogGroup->create())
                Log::log(0,"日志组共享内存创建成功!");
            else
            {
                Log::log(0,"日志组共享内存创建失败");
                ALARMLOG(0,MBC_Log_Group+2,"%s","日志组共享内存创建失败");
                THROW(MBC_Log_Group+2);
            }
            break;
        }
        
        case GROUPDELETE:
        {
            if(m_poLogGroup->ClearGroupFile(m_iGroupId,m_sFileName,true))
                Log::log(0,"删除日志组文件成功");
            else
            {
                Log::log(0,"删除日志组文件失败");
                ALARMLOG(0,MBC_Log_Group+10,"%s","删除日志组文件失败");
                THROW(MBC_Log_Group+10); 
            }
            break;
        }
        
        case GROUPFREE:
        {
            if(m_poLogGroup->remove())
                Log::log(0,"日志组共享内存删除成功!");
            else
            {
                Log::log(0,"日志组共享内存删除失败!");
                ALARMLOG(0,MBC_Log_Group+3,"%s","日志组共享内存删除失败!");
                THROW(MBC_Log_Group+3);
            }
            break;
        }
        
        case GROUPPRINTF:
        {
            m_poLogGroup->DisplayLogGroup(m_iGroupId);
            break;
        }

        case GROUPQUIT:
        {
            int iGroupId = m_poLogGroup->GetUseGroup();
            if(m_poLogGroup->ChangeLogGroup(false,m_iGroupId)==true)
            {
                Log::log(0,"切换日志组成功!");
                char stemp[200]={0};
                int itempId = m_poLogGroup->GetUseGroup();
                sprintf(stemp,"[logg] 日志组由组%d切换到组%d",iGroupId,itempId);
                m_poLogGroup->LogGroupWriteFile(SYSTEMLOG,stemp);
            }
            else
            {
                Log::log(0,"切换日志组失败");
                ALARMLOG(0,MBC_Log_Group+6,"%s","切换日志组失败");
                THROW(MBC_Log_Group+6);;
            }

            m_poLogGroup->DisplayLogGroup(m_iGroupId);            
            break;
        }

        case GROUPSET:
        {
            if(m_poLogGroup->SetLogGroupPigeonhole(m_bPigeonhole))
            {
                if(m_bPigeonhole==true)
                    Log::log(0,"日志组归档开关已打开");
                else
                    Log::log(0,"日志组归档开关已关闭");
            }
            break;
        }

        case GROUPOUT:
        {
            char FileName[500],FilePath[500];
            memset(FileName,0,sizeof(FileName));
            memset(FilePath,0,sizeof(FilePath));
            m_poLogGroup->GetCurUseFileName(m_iGroupId,FileName,FilePath);
            DisplayLogInfo(FileName,FilePath);
            break;
        }

        case FILECDOWN:
        {
            if(!m_poCheckPoint)
            {
                m_poCheckPoint = new CheckPointMgr();
                if(!m_poCheckPoint)
                {
                    Log::log(0,"创建CheckPointMgr管理对象失败");
                    THROW(MBC_Log_Group+5);
                }
            }
            char* sBachId = m_poCheckPoint->getCharBatchID();
            m_poLogGroup->CheckPointOut(sBachId);

            break;
        }

        case FILECLOAD:
        {
            m_poLogGroup->CheckPointIn(m_sBatch);
            break;
        }

        case PIGEONHOLE:
        {
            int lastGourpId=m_poLogGroup->GetUseGroup();
            if(m_poLogGroup->ChangeLogGroup(true,m_iGroupId)==false)
            {
                Log::log(0,"切换日志组失败");
                ALARMLOG(0,MBC_Log_Group+6,"%s","切换日志组失败");
                THROW(MBC_Log_Group+6);;
            }
            int curGroupId=m_poLogGroup->GetUseGroup();
            char temp[500];
            memset(temp,0,sizeof(temp));
            sprintf(temp,"[LOGG] 日志组由组%d切换到组%d",lastGourpId,curGroupId);
            m_poLogGroup->LogGroupWriteFile(SYSTEMLOG,temp);
            break;
        }

        case GROUPHELP:
        {
            printUsage();
            break;
        }

        case GROUPMODULE:
        {
            DealForModule();
            break;
        }

        case GROUPPROCESS:
        {
            DealForProcess();
            break;
        }

        case GROUPCHECKPT:
        {
            DealForCheckPoint();
            break;
        }

        case GROUPINFOLV:
        {
            DealForLevel();
            break;
        }

        case GROUPCLASS:
        {
            DealForClass();
            break;
        }

        case GROUPSTAFFID:
        {
            DealForStaffId();
            break;
        }

        case GROUPTABNAME:
        {
            DealForTableName();
            break;
        }

        case GROUPSQLTYPE:
        {
            DealForSqlType();
            break;
        }

        case GROUPPARAM:
        {
            DealForParam();
            break; 
        }
        
        case FILETEST:
        {
            for(int i=0;i!=100000;i++)
            {
                m_poLogGroup->LogGroupWriteFile(OPERATIONLOG, "[ALARM][ALARM_TYPE:0]--ProcId:[-1] ModuleId:[155] File:[InitServer.cpp] LineNo:[350] function:[CInitServer::ServiceStart()] ErrId:[100002004] ErrNo:[0][Msg]参数不符合规则:SectionName:CHECKPOINT ParamName:wait_time");
                m_poLogGroup->LogGroupWriteFile(OPERATIONLOG,"[LV-1][m_oEventExtractor.initialize]码制转换控制类初始化完成");
            }
            break;
        }
        
        default:
        {
            ALARMLOG28(0,MBC_CLASS_Parameter,168,"%s","应用进程参数不正确（不支持的选项）");
//            printUsage();
            return -1;
        }
        break;
    }
    
    return 0;
}

/*
void LogGroup::ParamMain(char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc)
{
    init();
    DisInit();
    if(!prepare(sArgv,iArgc))
        return ;
    Deal();
    LogGroup::m_vDisplayInfo.clear();
    return ;
}
*/

bool LogGroup::DisplayForInterface(int LogFlag)
{
    switch(LogFlag)
    {
        case GROUPMODULE:
        case GROUPCHECKPT:
        case GROUPINFOLV:
        case GROUPSTAFFID:
        case GROUPTABNAME:
        case GROUPSQLTYPE:
        case GROUPPARAM:
        case DISPARAM:
        case DISSYS:
        case DISATTEMP:
        case DISACCESS:
            LoggProcessMain_1();
            break;
        case GROUPPROCESS:
        case DISOPERAT:
        case DISFORALL:
            LoggProcessMain(LogFlag);
            break;
        default	:
            break;
    }
    LogGroup::m_vAlarmInfo.clear();
    LogGroup::m_vParamInfo.clear();
    LogGroup::m_vSysInfo.clear();
    LogGroup::m_vAttemperInfo.clear();
    LogGroup::m_vAccessInfo.clear();
    LogGroup::m_vConverInfo.clear();
    LogGroup::m_vDmlInfo.clear();
    return true;
}

void DisplayLogg::DisInit()
{
   //触发报警的时间间隔(单位：秒)
    int INTERVAL = 1;
    //菜单单页显示的行数
    int iLoggPageSize = 30;
    int Logg_Main2SelectedRow = 1;
    int Logg_Main2StartRow = 1;
    int Logg_Main3SelectedRow =1;
    int Logg_Main3StartRow = 1;
    int iDisParamCnt=0;
    int iDisParamCnt_ALL=0;
    //全局变量：纪录在菜单之间传递的参数
    int Logg_ProblemProcSelectedRow = 1;
    int Logg_ProblemProcStartRow = 1;
    int NOW_USED=0;
    Log_Menu* Logg_pMenuInfo = NULL;
    Log_Menu* Logg_pMenuInfoAll = NULL;
    int LogFlagAll=0; 
}

void DisplayLogg::Loggstartwin()
{
    //忽略中止信号
    signal(SIGINT,SIG_IGN);
    initscr();
    noecho();
    intrflush(stdscr,true); 
    savetty();
    keypad(stdscr,TRUE);
}
void DisplayLogg::LoggLoadGeneralInfo()
{
    //----------------------------加载菜单的通用信息--------------------------------
    //加载总标题
    attron(A_BOLD); 
    mvaddstr(0,(COLS_LENS - strlen((char *)"日志查询"))/2,(char *)"日志查询");   	   	
    attroff(A_BOLD);
    
    /*加载菜单界限*/
    mvhline(3,0,'=',COLS_LENS);
    mvhline(5 + iLoggPageSize + 1,0,'=',COLS_LENS);
}
void DisplayLogg::LoggLoadEmptyMenu()
{
    mvhline(2,0,' ',112);//清标题
    mvhline(4,0,' ',COLS_LENS);//清标题
    for(int i=6;i<iLoggPageSize+6;i++)
    {
        mvhline(i,0,' ',COLS_LENS);	
    }
    mvhline(5 + iLoggPageSize + 3,0,' ',COLS_LENS);
    mvaddstr(5 + iLoggPageSize + 3,13,(char *)"Up Arrow = 向上   Down Arrow = 向下   Left Arrow = 前页   Right Arrow = 后页   ESC = 退出   Enter = 进入");

    return;
}


void DisplayLogg::LoggRebuildProblemProc()
{
    //此变量用于自上而下绘制菜单
    int lv_iTotalRow = NOW_USED;
    int lv_iCurrentRow = 0;
    static char Buffer[COLS_LENS];
    
    LoggLoadEmptyMenu();
    //以下四个判断主要用于纠正一些非法的
    //如果全局选中行数<=0，则回指到gv_iMag_ProcSelectedRowinTotalRow 行
    if (Logg_ProblemProcSelectedRow  <= 0)
    {
       Logg_ProblemProcSelectedRow  = lv_iTotalRow ;
    }
    //如果全局选中行数> lv_iTotalRow ，则回指到第一行
    if (Logg_ProblemProcSelectedRow  > lv_iTotalRow)
    {
       Logg_ProblemProcSelectedRow  = 1;
    }
    //如果g_iStartRow <= 0，人为置为第一行	
    if (Logg_ProblemProcStartRow <= 0)
    {
       Logg_ProblemProcStartRow = 1;
    }
    if (Logg_ProblemProcStartRow > Logg_ProblemProcSelectedRow )
    {
       Logg_ProblemProcStartRow = Logg_ProblemProcSelectedRow ;
    }
    if (Logg_ProblemProcSelectedRow  - Logg_ProblemProcStartRow >= iLoggPageSize)
    {
       Logg_ProblemProcStartRow = Logg_ProblemProcSelectedRow  - iLoggPageSize + 1;
    }
    // --确定lv_iCurrentRow初始值
    while(1)
    {
        if (lv_iCurrentRow >= Logg_ProblemProcStartRow )
        {
            break;
        }
        else
        {
            lv_iCurrentRow++;
        }
    }	
    for(int i=0; i<iLoggPageSize; i++)
    {
        if ((lv_iCurrentRow > lv_iTotalRow)||(lv_iCurrentRow >= Logg_ProblemProcStartRow + iLoggPageSize))
        {
            break;
        }
        if (lv_iCurrentRow == Logg_ProblemProcSelectedRow)
        {
            standout();
        }
        mvhline(lv_iCurrentRow - Logg_ProblemProcStartRow + 5,0,' ',COLS_LENS);
        memset(Buffer,0,COLS_LENS);
        if(Logg_ProblemProcStartRow+i<=NOW_USED)
            sprintf(Buffer,"%s",(LogGroup::m_vConverInfo[Logg_ProblemProcStartRow+i-1]).c_str());
        mvaddstr(lv_iCurrentRow - Logg_ProblemProcStartRow + 5,0,Buffer);
        if (lv_iCurrentRow == Logg_ProblemProcSelectedRow)
        {
            standend();
        }
        lv_iCurrentRow++;
    }
    //未满的行填入空白		
    while (lv_iCurrentRow < Logg_ProblemProcStartRow + iLoggPageSize)
    {
        mvhline(lv_iCurrentRow - Logg_ProblemProcStartRow + 5,0,' ',COLS_LENS);
        lv_iCurrentRow ++;
    }
    wmove(stdscr,Logg_ProblemProcSelectedRow - Logg_ProblemProcStartRow + 5,0);//将指针移到选定行首
    refresh();
}
void DisplayLogg::LoggTimeBreak_ProblemProc(int signo)
{
    LoggRebuildProblemProc();
    return;
}
void DisplayLogg::LoggStartProblemProc()
{
    while(1)
    {
        LoggRebuildProblemProc();
        signal(SIGALRM, LoggTimeBreak_ProblemProc);
        alarm(INTERVAL); 
        int Key = getch();
        alarm(0);
        //如果键入Esc，则退出菜单
        if (Key == 27)
        {
          Logg_ProblemProcSelectedRow=1;
          Logg_ProblemProcStartRow=1;
          break;
        }
        switch (Key)
        {
            //键入回车
            case '\n':
            case '\r':
              break;
            case KEY_UP:
                Logg_ProblemProcSelectedRow--;
              break;
            case KEY_DOWN:
                Logg_ProblemProcSelectedRow++;
              break;
            case KEY_LEFT:
                Logg_ProblemProcStartRow -= iLoggPageSize;
                Logg_ProblemProcSelectedRow -= iLoggPageSize;
              break;
            case KEY_RIGHT:
                Logg_ProblemProcStartRow += iLoggPageSize;
                Logg_ProblemProcSelectedRow += iLoggPageSize;
              break;
            default	:
              break;
        }
    }
}
void DisplayLogg::LoggRebuildProcess()
{
    //此变量用于自上而下绘制菜单
    int lv_iTotalRow = 0;
    int lv_iCurrentRow = 0;
    static char Buffer[COLS_LENS];
    
    LoggLoadEmptyMenu();
    lv_iTotalRow = iDisParamCnt;
    lv_iCurrentRow =1;
    //以下四个判断主要用于纠正一些非法的
    //如果全局选中行数<=0，则回指到gv_iMainTotalRow 行
    if (Logg_Main2SelectedRow  <= 0)
    {
       Logg_Main2SelectedRow  = lv_iTotalRow ;
    }
    //如果全局选中行数> lv_iTotalRow ，则回指到第一行
    if (Logg_Main2SelectedRow  > lv_iTotalRow)
    {
       Logg_Main2SelectedRow  = 1;
    }
    //如果g_iStartRow <= 0，人为置为第一行	
    if (Logg_Main2StartRow <= 0)
    {
       Logg_Main2StartRow = 1;
    }
    if (Logg_Main2StartRow > Logg_Main2SelectedRow )
    {
       Logg_Main2StartRow = Logg_Main2SelectedRow ;
    }
    if (Logg_Main2SelectedRow  - Logg_Main2StartRow >= iLoggPageSize)
    {
       Logg_Main2StartRow = Logg_Main2SelectedRow  - iLoggPageSize + 1;
    }
    while(1)
    {
        if (lv_iCurrentRow >= Logg_Main2StartRow )
        {
           break;
        }
        else
        {
            lv_iCurrentRow++;
        }
    }
    while(lv_iCurrentRow <= lv_iTotalRow)
    {
        if (lv_iCurrentRow >= Logg_Main2StartRow + iLoggPageSize)
        {
            break;
        }
        if (lv_iCurrentRow == Logg_Main2SelectedRow)
        {
           standout();
        }
        mvhline(lv_iCurrentRow - Logg_Main2StartRow + 5,0,' ',COLS_LENS);
        memset(Buffer,0,COLS_LENS);
        
        //输出序号
        sprintf(Buffer,"%d",lv_iCurrentRow);
        mvaddstr(lv_iCurrentRow - Logg_Main2StartRow + 5,0,Buffer);
        //输出类别
//        mvaddstr(lv_iCurrentRow - Logg_Main2StartRow + 5,10,(char*)"日志信息查询");
        //输出名字
        memset(Buffer,0,COLS_LENS);
        sprintf(Buffer,"%s",Logg_pMenuInfo[lv_iCurrentRow-1].ChName);
        mvaddstr(lv_iCurrentRow - Logg_Main2StartRow + 5,10,Buffer);
        if (lv_iCurrentRow == Logg_Main2SelectedRow)
        {
            standend();
        }
        lv_iCurrentRow++;
    }
      //未满的行填入空白		
    while (lv_iCurrentRow < Logg_Main2StartRow + iLoggPageSize)
    {
        mvhline(lv_iCurrentRow - Logg_Main2StartRow + 5,0,' ',COLS_LENS);
        lv_iCurrentRow ++;
    }
    wmove(stdscr,Logg_Main2SelectedRow - Logg_Main2StartRow + 5,0);//将指针移到选定行首
    refresh();
}
void DisplayLogg::LoggTimeBreak_Process(int signo)
{
    LoggRebuildProcess();
    return;
}

void DisplayLogg::LoggloadAll(vector<string> &m_vInfo)
{
    LogGroup::m_vConverInfo.clear();
    NOW_USED = m_vInfo.size();

    if(NOW_USED==0)
    {
        LogGroup::m_vConverInfo.push_back("**************没有查询到符合的信息**************");
        NOW_USED = LogGroup::m_vConverInfo.size();
        return ;
    }
    vector<string>::iterator iter;

    for(iter=m_vInfo.begin();iter!=m_vInfo.end();++iter)
    {
        int size = (*iter).size();
        if(size>COLS_LENS)
        {
            int index=0;
            char temp_1[512],temp_2[512];
            while(size>COLS_LENS)
            {
               memset(temp_1,0,sizeof(temp_1));
               (*iter).copy(temp_1,COLS_LENS,index);
               LogGroup::m_vConverInfo.push_back(temp_1);
               size -= COLS_LENS;
               index += COLS_LENS;
            }
            memset(temp_2,0,sizeof(temp_2));
            (*iter).copy(temp_2,COLS_LENS,index);
            LogGroup::m_vConverInfo.push_back(temp_2);
        }
        else
            LogGroup::m_vConverInfo.push_back(*iter);
    }

    NOW_USED = LogGroup::m_vConverInfo.size();
}

void DisplayLogg::LoggRebuildProcessAll()
{
    //此变量用于自上而下绘制菜单
    int lv_iTotalRow = 0;
    int lv_iCurrentRow = 0;
    static char Buffer[COLS_LENS];
    
    LoggLoadEmptyMenu();
    lv_iTotalRow = iDisParamCnt_ALL;
    lv_iCurrentRow =1;
    //以下四个判断主要用于纠正一些非法的
    //如果全局选中行数<=0，则回指到gv_iMainTotalRow 行
    if (Logg_Main3SelectedRow  <= 0)
    {
       Logg_Main3SelectedRow  = lv_iTotalRow ;
    }
    //如果全局选中行数> lv_iTotalRow ，则回指到第一行
    if (Logg_Main3SelectedRow  > lv_iTotalRow)
    {
       Logg_Main3SelectedRow  = 1;
    }
    //如果g_iStartRow <= 0，人为置为第一行	
    if (Logg_Main3SelectedRow <= 0)
    {
       Logg_Main3SelectedRow = 1;
    }
    if (Logg_Main3StartRow > Logg_Main3SelectedRow )
    {
       Logg_Main3StartRow = Logg_Main3SelectedRow ;
    }
    if (Logg_Main3SelectedRow  - Logg_Main2StartRow >= iLoggPageSize)
    {
       Logg_Main3StartRow = Logg_Main3SelectedRow  - iLoggPageSize + 1;
    }
    while(1)
    {
        if (lv_iCurrentRow >= Logg_Main3StartRow )
        {
           break;
        }
        else
        {
            lv_iCurrentRow++;
        }
    }
    while(lv_iCurrentRow <= lv_iTotalRow)
    {
        if (lv_iCurrentRow >= Logg_Main3StartRow + iLoggPageSize)
        {
            break;
        }
        if (lv_iCurrentRow == Logg_Main3SelectedRow)
        {
           standout();
        }
        mvhline(lv_iCurrentRow - Logg_Main3StartRow + 5,0,' ',COLS_LENS);
        memset(Buffer,0,COLS_LENS);
        
        //输出序号
        sprintf(Buffer,"%d",lv_iCurrentRow);
        mvaddstr(lv_iCurrentRow - Logg_Main3StartRow + 5,0,Buffer);
        //输出类别
//        mvaddstr(lv_iCurrentRow - Logg_Main2StartRow + 5,10,(char*)"日志信息查询");
        //输出名字
        memset(Buffer,0,COLS_LENS);
        sprintf(Buffer,"%s",Logg_pMenuInfoAll[lv_iCurrentRow-1].ChName);
        mvaddstr(lv_iCurrentRow - Logg_Main3StartRow + 5,10,Buffer);
        if (lv_iCurrentRow == Logg_Main3SelectedRow)
        {
            standend();
        }
        lv_iCurrentRow++;
    }
      //未满的行填入空白		
    while (lv_iCurrentRow < Logg_Main3StartRow + iLoggPageSize)
    {
        mvhline(lv_iCurrentRow - Logg_Main3StartRow + 5,0,' ',COLS_LENS);
        lv_iCurrentRow ++;
    }
    wmove(stdscr,Logg_Main3SelectedRow - Logg_Main3StartRow + 5,0);//将指针移到选定行首
    refresh();
}

void DisplayLogg::LoggInitAll_1()
{
    iDisParamCnt_ALL = 3;
    Logg_pMenuInfoAll = new Log_Menu[iDisParamCnt_ALL];
    Logg_pMenuInfoAll[0].Index  = 1;
    strcpy(Logg_pMenuInfoAll[0].ChName,"系统业务等级日志信息");
//    Logg_pMenuInfoAll[1].pFunc  = (GETNUM)&LoggStartProblemProc;
    Logg_pMenuInfoAll[1].Index  = 2;
    strcpy(Logg_pMenuInfoAll[1].ChName,"系统进程告警日志信息");
//    Logg_pMenuInfoAll[0].pFunc  = (GETNUM)&LoggStartProblemProc;
    Logg_pMenuInfoAll[2].Index  = 3;
    strcpy(Logg_pMenuInfoAll[2].ChName,"系统DML操作日志信息");
}


void DisplayLogg::LoggTimeBreak_ProblemProcAll(int signo)
{
    LoggRebuildProcessAll();
    return;
}
void DisplayLogg::LoggStartProblemProcAll()
{
    LoggLoadEmptyMenu();
    LoggInitAll_1();
    while(1)
    {
        LoggRebuildProcessAll();
        signal(SIGALRM, LoggTimeBreak_ProblemProcAll);
        alarm(INTERVAL); 
        int Key = getch();
        alarm(0);
        //如果键入Esc，则退出菜单
        if (Key == 27)
        {
          break;
        }
        switch (Key)
        {
            //键入回车
            case '\n':
            case '\r':
                    if(Logg_pMenuInfoAll[Logg_Main3SelectedRow-1].Index == 1)
                    {
                        LoggloadAll(LogGroup::m_vDisplayInfo);
                        LoggStartProblemProc();
                    }
                    else if(Logg_pMenuInfoAll[Logg_Main3SelectedRow-1].Index == 2)
                    {
                        LoggloadAll(LogGroup::m_vAlarmInfo);
                        LoggStartProblemProc();
                    }
                    else if(Logg_pMenuInfoAll[Logg_Main3SelectedRow-1].Index == 3)
                    {
                        LoggloadAll(LogGroup::m_vDmlInfo);
                        LoggStartProblemProc();
                    }
              break;
            case KEY_UP:
                Logg_Main3SelectedRow--;
              break;
            case KEY_DOWN:
                Logg_Main3SelectedRow++;
              break;
            default	:
              break;
        }
    }
}

void DisplayLogg::LoggInitProcessInfo(int flag)
{
    if(flag==GROUPPROCESS)
    {
        iDisParamCnt = 2;
        Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
        Logg_pMenuInfo[0].Index  = 1;
        strcpy(Logg_pMenuInfo[0].ChName,"系统进程调度日志信息");
//        Logg_pMenuInfo[0].pFunc  = (GETNUM)&LoggStartProblemProc;
        
        
        Logg_pMenuInfo[1].Index  = 2;
        strcpy(Logg_pMenuInfo[1].ChName,"系统进程告警日志信息");
//        Logg_pMenuInfo[1].pFunc  = (GETNUM)&LoggStartProblemProc;
    }
    else if(flag==GROUPMODULE)
    {
        iDisParamCnt = 1;
        Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
        Logg_pMenuInfo[0].Index  = 1;
        strcpy(Logg_pMenuInfo[0].ChName,"系统模块告警日志信息");
//        Logg_pMenuInfo[0].pFunc  = (GETNUM)&LoggStartProblemProc;
    }
    else if(flag==GROUPPARAM||flag==DISPARAM)
    {
        iDisParamCnt = 1;
        Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
        Logg_pMenuInfo[0].Index  = 1;
        strcpy(Logg_pMenuInfo[0].ChName,"系统参数修改日志信息");
//        Logg_pMenuInfo[0].pFunc  = (GETNUM)&LoggStartProblemProc;
    }
    else if(flag==GROUPCHECKPT||flag==DISSYS)
    {
        iDisParamCnt = 1;
        Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
        Logg_pMenuInfo[0].Index  = 1;
        strcpy(Logg_pMenuInfo[0].ChName,"checkpoint日志信息");
//        Logg_pMenuInfo[0].pFunc  = (GETNUM)&LoggStartProblemProc;
    }
    else if(flag==GROUPINFOLV)
    {
        iDisParamCnt = 1;
        Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
        Logg_pMenuInfo[0].Index  = 1;
        strcpy(Logg_pMenuInfo[0].ChName,"系统业务等级日志信息");
//        Logg_pMenuInfo[0].pFunc  = (GETNUM)&LoggStartProblemProc;        
    }
    else if(flag==GROUPSTAFFID)
    {
        iDisParamCnt = 1;
        Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
        Logg_pMenuInfo[0].Index  = 1;
        strcpy(Logg_pMenuInfo[0].ChName,"系统工号操作日志信息");
//        Logg_pMenuInfo[0].pFunc  = (GETNUM)&LoggStartProblemProc;   
    }
    else if(flag==GROUPTABNAME)
    {
        iDisParamCnt = 1;
        Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
        Logg_pMenuInfo[0].Index  = 1;
        strcpy(Logg_pMenuInfo[0].ChName,"系统hbaccess表操作日志信息");
//        Logg_pMenuInfo[0].pFunc  = (GETNUM)&LoggStartProblemProc;           
    }
    else if(flag==GROUPSQLTYPE)
    {
        iDisParamCnt = 1;
        Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
        Logg_pMenuInfo[0].Index  = 1;
        strcpy(Logg_pMenuInfo[0].ChName,"系统hbaccess类别操作日志信息");
//        Logg_pMenuInfo[0].pFunc  = (GETNUM)&LoggStartProblemProc;               
    }
    else if(flag==DISFORALL)
    {
        iDisParamCnt = 5;
        Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
        
        Logg_pMenuInfo[0].Index  = 1;
        strcpy(Logg_pMenuInfo[0].ChName,"参数管理日志");
//        Logg_pMenuInfo[0].pFunc  = (GETNUM)&LoggStartProblemProc;

        Logg_pMenuInfo[1].Index  = 2;
        strcpy(Logg_pMenuInfo[1].ChName,"进程调度日志");
//        Logg_pMenuInfo[1].pFunc  = (GETNUM)&LoggStartProblemProcAll;

        Logg_pMenuInfo[2].Index  = 3;
        strcpy(Logg_pMenuInfo[2].ChName,"系统管理日志");
//        Logg_pMenuInfo[2].pFunc  = (GETNUM)&LoggStartProblemProc;

        Logg_pMenuInfo[3].Index  = 4;
        strcpy(Logg_pMenuInfo[3].ChName,"业务进程处理日志");
//        Logg_pMenuInfo[3].pFunc  = (GETNUM)&LoggStartProblemProc;

        Logg_pMenuInfo[4].Index  = 5;
        strcpy(Logg_pMenuInfo[4].ChName,"hbaccess数据库修改日志");
//        Logg_pMenuInfo[4].pFunc  = (GETNUM)&LoggStartProblemProc;
    }
    else if(flag==DISOPERAT)
    {
        iDisParamCnt = 3;
        Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
        Logg_pMenuInfo[0].Index  = 1;
        strcpy(Logg_pMenuInfo[0].ChName,"系统业务等级日志信息");
//        Logg_pMenuInfo[0].pFunc  = (GETNUM)&LoggStartProblemProc;
        
        
        Logg_pMenuInfo[1].Index  = 2;
        strcpy(Logg_pMenuInfo[1].ChName,"系统进程告警日志信息");
//        Logg_pMenuInfo[1].pFunc  = (GETNUM)&LoggStartProblemProc;

        Logg_pMenuInfo[2].Index  = 3;
        strcpy(Logg_pMenuInfo[2].ChName,"系统DML操作日志信息");
    }
    else if(flag==DISACCESS)
    {
        iDisParamCnt = 1;
        Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
        Logg_pMenuInfo[0].Index  = 1;
        strcpy(Logg_pMenuInfo[0].ChName,"系统hbaccess操作日志信息");
//        Logg_pMenuInfo[0].pFunc  = (GETNUM)&LoggStartProblemProc;       
    }
    else if(flag==DISATTEMP)
    {
        iDisParamCnt = 1;
        Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
        Logg_pMenuInfo[0].Index  = 1;
        strcpy(Logg_pMenuInfo[0].ChName,"系统进程调度日志信息");
//        Logg_pMenuInfo[0].pFunc  = (GETNUM)&LoggStartProblemProc;   
    }
}


int DisplayLogg::LoggProcessMain(int flag)
{
    Loggstartwin();
    LoggLoadGeneralInfo();
    LoggInitProcessInfo(flag);
    while(1)
    {
        LoggRebuildProcess();
        signal(SIGALRM, LoggTimeBreak_Process);
        alarm(INTERVAL); 
        int Key = getch();
        alarm(0);
        
        //如果键入Esc，则退出菜单
        if (Key == 27)
        {
          break;
        }
        switch (Key)
        {
            //键入回车
            case '\n':
            case '\r':
                if(flag==GROUPPROCESS||flag==DISOPERAT)
                {
                    if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 1)
                    {
                        LoggloadAll(LogGroup::m_vDisplayInfo);
                        LoggStartProblemProc();
                    }
                    else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 2)
                    {
                        LoggloadAll(LogGroup::m_vAlarmInfo);
                        LoggStartProblemProc();
                    }
                    else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 3)
                    {
                        LoggloadAll(LogGroup::m_vDmlInfo);
                        LoggStartProblemProc();
                    }
                }
                else if(flag==GROUPMODULE||flag==GROUPPARAM||flag==GROUPCHECKPT||flag==GROUPINFOLV
                        ||flag==GROUPSTAFFID||flag==GROUPTABNAME||flag==GROUPSQLTYPE)
                {
                    if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 1)
                    {
                        LoggloadAll(LogGroup::m_vDisplayInfo);
                        LoggStartProblemProc();
                    }
                }
                else if(flag==DISPARAM||flag==DISSYS||flag==DISATTEMP||flag==DISACCESS)
                {
                    LoggloadAll(LogGroup::m_vDisplayInfo);
                    LoggStartProblemProc();
                }
                else if(flag==DISFORALL)
                {
                    if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index==1)
                    {
                        LoggloadAll(LogGroup::m_vParamInfo);
                        LoggStartProblemProc();
                    }
                    else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index==2)
                    {
                        LoggloadAll(LogGroup::m_vAttemperInfo);
                        LoggStartProblemProc();
                    }
                    else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index==3)
                    {
                        LoggloadAll(LogGroup::m_vSysInfo);
                        LoggStartProblemProc();
                    }
                    else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index==4)
                    {
                        LoggStartProblemProcAll();
                    }
                    else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index==5)
                    {
                        LoggloadAll(LogGroup::m_vAccessInfo);
                        LoggStartProblemProc();
                    }
                }
              break;
            case KEY_UP:
                Logg_Main2SelectedRow--;
              break;
            case KEY_DOWN:
                Logg_Main2SelectedRow++;
              break;
            default :
              break;
        }
    }
    //退出到命令行模式
    resetty();
    endwin();
    system("clear");
    return 0;
}


void DisplayLogg::LoggTimeBreak_1(int signo)
{
    LoggRebuildProblemProc();
    return;
}
void DisplayLogg::LoggProcessMain_1()
{
    Loggstartwin();
    LoggLoadGeneralInfo();
    LoggloadAll(LogGroup::m_vDisplayInfo);
    while(1)
    {
        LoggRebuildProblemProc();
        signal(SIGALRM, LoggTimeBreak_1);
        alarm(INTERVAL); 
        int Key = getch();
        alarm(0);
        //如果键入Esc，则退出菜单
        if (Key == 27)
        {
          break;
        }
        switch (Key)
        {
            //键入回车
            case '\n':
            case '\r':
              break;
            case KEY_UP:
                Logg_ProblemProcSelectedRow--;
              break;
            case KEY_DOWN:
                Logg_ProblemProcSelectedRow++;
              break;
            case KEY_LEFT:
                Logg_ProblemProcStartRow -= iLoggPageSize;
                Logg_ProblemProcSelectedRow -= iLoggPageSize;
              break;
            case KEY_RIGHT:
                Logg_ProblemProcStartRow += iLoggPageSize;
                Logg_ProblemProcSelectedRow += iLoggPageSize;
              break;
            default	:
              break;
        }
    }
    resetty();
    endwin();
    system("clear");
}


void LogGroupAdmin::ParamMain(char sArgv[ARG_NUM][ARG_VAL_LEN],int iArgc)
{
    LogGroup *pLogg = new LogGroup();
    pLogg->init();
    pLogg->DisInit();
    if(!pLogg->prepare(sArgv,iArgc))
        return ;
    pLogg->Deal();
    LogGroup::m_vDisplayInfo.clear();
    delete pLogg;
    pLogg = 0;
    return ;
}


   //触发报警的时间间隔(单位：秒)
int DisplayLogg::INTERVAL = 1;
//菜单单页显示的行数
int DisplayLogg::iLoggPageSize = 27;
int DisplayLogg::Logg_Main2SelectedRow = 1;
int DisplayLogg::Logg_Main2StartRow = 1;
int DisplayLogg::Logg_Main3SelectedRow =1;
int DisplayLogg::Logg_Main3StartRow = 1;
int DisplayLogg::iDisParamCnt=0;
int DisplayLogg::iDisParamCnt_ALL=0;
//全局变量：纪录在菜单之间传递的参数
int DisplayLogg::Logg_ProblemProcSelectedRow = 1;
int DisplayLogg::Logg_ProblemProcStartRow = 1;
int DisplayLogg::NOW_USED=0;
Log_Menu* DisplayLogg::Logg_pMenuInfo = NULL;
Log_Menu* DisplayLogg::Logg_pMenuInfoAll = NULL;
int DisplayLogg::LogFlagAll=0; 
