#include "LoggWeb.h"
#include <algorithm>
#include "MBC_SGW.h"

//int g_argc;
//char ** g_argv;

extern int g_iFdFront;
extern int g_iStart;
extern int g_iEnd;
extern char g_sStartDate[16];
extern char g_sEndDate[16];

LoggMgr::LoggMgr():m_iMode(0),m_iGroupId(-1),m_poLoggMgr(0),m_poLogg(0)
                    ,m_bPigeonhole(false),m_bParamErr(false),m_RD(0)
{
    memset(m_sParam,'\0',sizeof(m_sParam));
    memset(m_sPath,'\0',sizeof(m_sPath));
    memset(m_sFileName,'\0',sizeof(m_sFileName));
    memset(m_sParamFile,'\0',sizeof(m_sParamFile));
   // m_pCommandComm = 0;
    m_RD = new ReadIni();
    m_iNum=0;
   
   //modify by liyr
   // pPoint= new CheckPointMgr();
   pPoint = 0;
}

LoggMgr::~LoggMgr()
{
    if(m_poLoggMgr)
    {
        delete m_poLoggMgr;
        m_poLoggMgr = 0;
    }
    if(m_poLogg)
    {
        delete m_poLogg;
        m_poLogg = 0;
    }
    if(m_RD)
    {
        delete m_RD;
        m_RD = 0;
    }
    if(pPoint)
    {
    	delete pPoint;
    	pPoint=0;
    }
    DisplayLogg::m_vAppInfo.clear();
    DisplayLogg::m_vProInfo.clear();
    DisplayLogg::m_vAlarmInfo.clear();
    DisplayLogg::m_vParamInfo.clear();
    DisplayLogg::m_vSystemInfo.clear();
    DisplayLogg::m_vSysOperInfo.clear();
}


/*
 *	函 数 名:   printUsage
 *	函数功能:   帮助函数
 *	时    间:   2011年3月15日
 *	返 回 值:   void
 *	参数说明:  
*/
void LoggMgr::printUsage()
{
    cout << endl << endl;
    cout << "*********************************************************************************" << endl;
    cout << "                                  展现类命令:                                    " <<endl;  
    cout << "sgwlog" << " "  << "-param   [parameter] :                    显示[指定参数名]的日志内容"<<endl;
    cout << "sgwlog" << " "  << "-process [process] :                      显示所有[指定进程ID]的日志内容"<<endl;
    cout << "sgwlog" << " "  << "-module  [module] :                       显示所有[指定模块ID|Name]的日志内容"<<endl;
    cout << "sgwlog" << " "  << "-class   [class] :                        显示所有[指定类别ID]的日志内容"<<endl;
    cout << "sgwlog" << " "  << "-level   [level] :                        显示所有[指定等级ID]的日志内容"<<endl;
	cout << "sgwlog" << " "  << "-code    CodeID  :                        显示指定CodeID的日志内容" << endl;
//    cout << "sgwlog" << " "  << "-type    [type] :                         显示所有[指定分类ID]的日志内容"<<endl;
    cout << endl;
    cout << "                                  控制类命令:                                    " <<endl;
    cout << "sgwlog" << " "  << "-c :                                      创建日志组共享内存" << endl;
    cout << "sgwlog" << " "  << "-f :                                      释放日志组共享内存" << endl;
    cout << "sgwlog" << " "  << "-a :                                      日志组手动归档" << endl;
    cout << "sgwlog" << " "  << "-b groupid path(不指定备份到默认目录) :   备份指定日志组到指定目录" << endl;
    cout << "sgwlog" << " "  << "-d groupid filename :                     删除日志组文件" << endl;
    cout << "sgwlog" << " "  << "-e param :                                设置日志组归档" << endl;
    cout << "sgwlog" << " "  << "-i groupid path(不配取默认路径) :         增加日志组文件" << endl;
//    cout << "sgwlogg" << " "  << "-g :                                      增加日志组" << endl;
    cout << "sgwlog" << " "  << "-l (groupid)(带上groupid打印指定组) :     显示(当前/指定)日志组信息" << endl;
    cout << "sgwlog" << " "  << "-s groupid(不指定自动切换当前组) :        切换日志组" << endl;
    cout << "*********************************************************************************" << endl;
    cout << endl << endl;
    return ;
}

/*
 *	函 数 名:   AnalyzesParam
 *	函数功能:   解析命令参数
 *	时    间:   2011年3月15日
 *	返 回 值:   bool
 *	参数说明:  
*/
bool LoggMgr::AnalyzesParam(char * sParam, char * sReturn)
{
    if(!sParam)
        return true;
    char *p = strchr(sParam,'[');
    if(!p)
    {
        char *m = strchr(sParam,'-');
        if(!m)
            strcpy(sReturn,sParam);
        return true;
    }
    p++;
    char *q = strchr(sParam,']');
    if(!q)
        return false;
    strncpy(sReturn,p,q-p);
    return true;
}

/*
 *	函 数 名:   CheckParamName
 *	函数功能:   检查参数名是否正确
 *	时    间:   2011年3月15日
 *	返 回 值:   bool
 *	参数说明:  
*/
bool LoggMgr::CheckParamName()
{
  if(!m_pCommandComm )
    {
        m_pCommandComm = new CommandCom();
        if(!m_pCommandComm->InitClient())
        {
            Log::log(0,"注册核心参数失败");
        }	
    }
    char ParamSec[50],ParamKey[50];

    memset(ParamSec,0,sizeof(ParamSec));
    memset(ParamKey,0,sizeof(ParamKey));
    
    char *p = strchr(m_sParam,'.');
    if(!p)
    {
        return false;
    }
    strncpy(ParamSec,m_sParam,p-m_sParam);
    p++;
    strncpy(ParamKey,p,strlen(p));

    if(!m_pCommandComm)
    {
        return false;
    }
    //SysParamHis oParam;
    //if(!m_pCommandComm->getSectionParam(ParamSec,ParamKey,oParam))
      //  return false;
    char sStr[128]={0};
    m_pCommandComm->readIniString(ParamSec,ParamKey,sStr,NULL);
	
	if(!strlen(sStr))return false;
	
    return true;
}

/*
 *	函 数 名:   GetField
 *	函数功能:   取数据字段信息
 *	时    间:   2011年6月28日
 *	返 回 值:   bool
 *	参数说明:  
*/
bool LoggMgr::GetField(int iSeq, char *sBuf,char *sParam)
{
    if(!sBuf)
        return false;
    char *p = strchr((char *)sBuf,'|');
    char *p1= sBuf;
    int cnt = 1;
    while(p)
    {
        if(cnt==iSeq)
        {
            strncpy(sParam,p1,p-p1);
            break;
        }
        cnt++;
		p++;
		p1=p;
		p=strchr(p1,'|');
    }
    return true;
}

/*
 *	函 数 名:   GreaterMark
 *	函数功能:   比较函数
 *	时    间:   2011年6月28日
 *	返 回 值:   bool
 *	参数说明:  
*/
bool LoggMgr::GreaterMark(const string & s1, const string & s2)
{
    char sFile1[LOG_DATA_SIZE+1],sFile2[LOG_DATA_SIZE+1];
    memset(sFile1,0,sizeof(sFile1));
    memset(sFile2,0,sizeof(sFile2));
    GetField(9,(char *)s1.c_str(),sFile1);
    GetField(9,(char *)s2.c_str(),sFile2);
    if(strcmp(sFile1,sFile2)>=0)
        return true;
    else 
        return false;
}


/*
 *	函 数 名:   StringCopy
 *	函数功能:   将vSrc中的内容复制到vDest中
 *	时    间:   2011年8月22日
 *	返 回 值:   void
 *	参数说明:  
*/
void LoggMgr::StringCopy(vector<string> &vDest, vector<string> &vSrc)
{
	for(size_t i = 0; i != vSrc.size(); ++i)
	{
		vDest.push_back(vSrc[i]);
	}
}

/*
 *	函 数 名:   DisplayInfo
 *	函数功能:   展现日志信息方法
 *	时    间:   2011年3月15日
 *	返 回 值:   bool
 *	参数说明:  
*/
bool LoggMgr::DisplayInfo(int iMode)
{
    int iLogType = 0,iLogClass = 0,iLevelId = 0;
    int iCondId=-1;
    vector<string> vLogInfo;
    vLogInfo.clear();

	if(!g_iFdFront) {
		memset(g_sStartDate,0,sizeof(g_sStartDate));
		memset(g_sEndDate,0,sizeof(g_sEndDate));
	}
	
    switch(iMode)
    {
    case GROUPPARAM:
        iLogType = LOG_TYPE_PARAM;
        //m_poLogg->GetInfoFromMem(DisplayLogg::m_vParamInfo,iLogType,GROUPPARAM,iLogClass,m_sParam);
        m_poLogg->GetInfoFromMem(DisplayLogg::m_vTypeParam,iMode,LOG_TYPE_PARAM,m_sParam,g_sStartDate,g_sEndDate);  
		if(g_iFdFront) DisPlayInfoForWeb(DisplayLogg::m_vTypeParam);
        break;

    case GROUPINFOLV:
        if(strlen(m_sParam))
        {
        	if(isdigit(m_sParam[0]))
        	{
            	iCondId = atoi(m_sParam);
        	} else {
				if(strcasecmp(m_sParam,"fatal") == 0)
					iCondId = LOG_LEVEL_FATAL;
				else if(strcasecmp(m_sParam,"error") == 0)
					iCondId = LOG_LEVEL_ERROR;
				else if(strcasecmp(m_sParam,"warning") == 0)
					iCondId = LOG_LEVEL_WARNING;
				else if(strcasecmp(m_sParam,"info") == 0)
					iCondId = LOG_LEVEL_INFO;
				else if(strcasecmp(m_sParam,"debug") == 0)
					iCondId = LOG_LEVEL_DEBUG;
				sprintf(m_sParam,"%d",iCondId);
			}
			
            if(iCondId==LOG_LEVEL_FATAL) {
               m_poLogg->GetInfoFromMem(DisplayLogg::m_vLevFatal,iMode,LOG_LEVEL_FATAL,m_sParam,g_sStartDate,g_sEndDate); 
               if(g_iFdFront) DisPlayInfoForWeb(DisplayLogg::m_vLevFatal);
            } 
            else if(iCondId==LOG_LEVEL_ERROR) {
               m_poLogg->GetInfoFromMem(DisplayLogg::m_vLevError,iMode,LOG_LEVEL_ERROR,m_sParam,g_sStartDate,g_sEndDate);  
               if(g_iFdFront) DisPlayInfoForWeb( DisplayLogg::m_vLevError); 	
            }
            else if(iCondId==LOG_LEVEL_WARNING) {
              m_poLogg->GetInfoFromMem(DisplayLogg::m_vLevWarn,iMode,LOG_LEVEL_WARNING,m_sParam,g_sStartDate,g_sEndDate); 
              if(g_iFdFront) DisPlayInfoForWeb( DisplayLogg::m_vLevWarn);
            }	
            else if(iCondId==LOG_LEVEL_INFO) {
               m_poLogg->GetInfoFromMem(DisplayLogg::m_vLevInfo,iMode,LOG_LEVEL_INFO,m_sParam,g_sStartDate,g_sEndDate);
               if(g_iFdFront) DisPlayInfoForWeb(DisplayLogg::m_vLevInfo);
            }
            else if(iCondId==LOG_LEVEL_DEBUG) {
              m_poLogg->GetInfoFromMem(DisplayLogg::m_vLevDebug,iMode,LOG_LEVEL_DEBUG,m_sParam,g_sStartDate,g_sEndDate); 
              if(g_iFdFront) DisPlayInfoForWeb( DisplayLogg::m_vLevDebug);
            }	               	 
            else
            {
            		char errstr[500]={0};
            		strcat(errstr,"错误的日志等级! \n日志等级取值如下：\n");
            	 	strcat(errstr,"    1:FATAL  2:ERROR  3:WARNGING  4:INFO  5:DEBUG \n");
            		if(g_iFdFront)
            		{                 
                    ErrOutForWeb(errstr);
                    EndOutForWeb();            			
            		} else {
            	 		printf("%s",errstr);
            		}
               return false;   
            }     	
        }else{
             m_poLogg->GetInfoFromMem(DisplayLogg::m_vLevFatal,iMode,LOG_LEVEL_FATAL,m_sParam,g_sStartDate,g_sEndDate);  
             m_poLogg->GetInfoFromMem(DisplayLogg::m_vLevError,iMode,LOG_LEVEL_ERROR,m_sParam,g_sStartDate,g_sEndDate);  
             m_poLogg->GetInfoFromMem(DisplayLogg::m_vLevWarn,iMode,LOG_LEVEL_WARNING,m_sParam,g_sStartDate,g_sEndDate); 
             m_poLogg->GetInfoFromMem(DisplayLogg::m_vLevInfo,iMode,LOG_LEVEL_INFO,m_sParam,g_sStartDate,g_sEndDate);  
             m_poLogg->GetInfoFromMem(DisplayLogg::m_vLevDebug,iMode,LOG_LEVEL_DEBUG,m_sParam,g_sStartDate,g_sEndDate); 
             if(g_iFdFront)
             {
            	 StringCopy(vLogInfo,DisplayLogg::m_vLevFatal);
            	 StringCopy(vLogInfo,DisplayLogg::m_vLevError);
            	 StringCopy(vLogInfo,DisplayLogg::m_vLevWarn);		
             	 StringCopy(vLogInfo,DisplayLogg::m_vLevInfo);
               	 StringCopy(vLogInfo,DisplayLogg::m_vLevDebug);
				 DisPlayInfoForWeb(vLogInfo);
             }
        }
        break;    	    	
    case GROUPTYPE:
        if(strlen(m_sParam))
        {
            iCondId = atoi(m_sParam);
            if(iCondId==LOG_TYPE_BUSI) {
               m_poLogg->GetInfoFromMem(DisplayLogg::m_vTypeBusi,iMode,LOG_TYPE_BUSI,m_sParam,g_sStartDate,g_sEndDate);  
                if(g_iFdFront) DisPlayInfoForWeb(DisplayLogg::m_vTypeBusi);		
            }
            else if(iCondId==LOG_TYPE_PARAM) {
               m_poLogg->GetInfoFromMem(DisplayLogg::m_vTypeParam,iMode,LOG_TYPE_PARAM,m_sParam,g_sStartDate,g_sEndDate);  
               if(g_iFdFront) DisPlayInfoForWeb( DisplayLogg::m_vTypeParam); 	
            }
            else if(iCondId==LOG_TYPE_SYSTEM) {
              m_poLogg->GetInfoFromMem(DisplayLogg::m_vTypeSystem,iMode,LOG_TYPE_SYSTEM,m_sParam,g_sStartDate,g_sEndDate); 
              if(g_iFdFront) DisPlayInfoForWeb(DisplayLogg::m_vTypeSystem);
           }
            else if(iCondId==LOG_TYPE_PROC) {
               m_poLogg->GetInfoFromMem(DisplayLogg::m_vTypeStart,iMode,LOG_TYPE_PROC,m_sParam,g_sStartDate,g_sEndDate); 
               if(g_iFdFront) DisPlayInfoForWeb( DisplayLogg::m_vTypeStart);	
            } 
            else if(iCondId==LOG_TYPE_DATEDB) {
              m_poLogg->GetInfoFromMem(DisplayLogg::m_vTypeOracle,iMode,LOG_TYPE_DATEDB,m_sParam,g_sStartDate,g_sEndDate);  
              if(g_iFdFront) DisPlayInfoForWeb( DisplayLogg::m_vTypeOracle);	
            }              	 
            else
            {
            		char errstr[500]={0};
            		strcat(errstr,"错误的日志类别! \n日志类别取值如下：\n");
            	 	strcat(errstr,"    1:业务日志  2:参数管理日志  3:系统管理日志  4:进程调度日志  5:操作类日志 \n");
            	 	
             		if(g_iFdFront)
            		{                 
                    ErrOutForWeb(errstr);
                    EndOutForWeb();            			
            		}else {
            	 		printf("%s",errstr);
            		}
            		
            		return false;
            }     	     	
        }else{
           m_poLogg->GetInfoFromMem(DisplayLogg::m_vTypeBusi,iMode,LOG_TYPE_BUSI,m_sParam,g_sStartDate,g_sEndDate);  
           m_poLogg->GetInfoFromMem(DisplayLogg::m_vTypeParam,iMode,LOG_TYPE_PARAM,m_sParam,g_sStartDate,g_sEndDate);  
           m_poLogg->GetInfoFromMem(DisplayLogg::m_vTypeSystem,iMode,LOG_TYPE_SYSTEM,m_sParam,g_sStartDate,g_sEndDate); 
           m_poLogg->GetInfoFromMem(DisplayLogg::m_vTypeStart,iMode,LOG_TYPE_PROC,m_sParam,g_sStartDate,g_sEndDate);  
           m_poLogg->GetInfoFromMem(DisplayLogg::m_vTypeOracle,iMode,LOG_TYPE_DATEDB,m_sParam,g_sStartDate,g_sEndDate); 
           if(g_iFdFront)
           {
            	 StringCopy(vLogInfo,DisplayLogg::m_vTypeBusi);
            	 StringCopy(vLogInfo,DisplayLogg::m_vTypeParam);
            	 StringCopy(vLogInfo,DisplayLogg::m_vTypeSystem);		
             	 StringCopy(vLogInfo,DisplayLogg::m_vTypeStart);
                 StringCopy(vLogInfo,DisplayLogg::m_vTypeOracle);
				 DisPlayInfoForWeb(vLogInfo);
           }            	  
        }
        break;
    case GROUPMODULE:
    case GROUPPROCESS:        
       m_poLogg->GetInfoFromMem(DisplayLogg::m_vClaError,iMode,LOG_CLASS_ERROR,m_sParam,g_sStartDate,g_sEndDate);    
       m_poLogg->GetInfoFromMem(DisplayLogg::m_vClaWarn,iMode,LOG_CLASS_WARNING,m_sParam,g_sStartDate,g_sEndDate);    
       m_poLogg->GetInfoFromMem(DisplayLogg::m_vClaInfo,iMode,LOG_CLASS_INFO,m_sParam,g_sStartDate,g_sEndDate);
       if(g_iFdFront)
       {
       		StringCopy(vLogInfo,DisplayLogg::m_vClaError);
       		StringCopy(vLogInfo,DisplayLogg::m_vClaWarn);
       		StringCopy(vLogInfo,DisplayLogg::m_vClaInfo);
			DisPlayInfoForWeb(vLogInfo);
       }
       	break ;
    case GROUPCLASS:       	         	       		
        if(strlen(m_sParam))
        {//带条件选择，复用m_vAlarmInfo存储类别信息
            iCondId = atoi(m_sParam);
            if(iCondId==LOG_CLASS_ERROR) {
               m_poLogg->GetInfoFromMem(DisplayLogg::m_vClaError,iMode,LOG_CLASS_ERROR,m_sParam,g_sStartDate,g_sEndDate);  
               if(g_iFdFront)  DisPlayInfoForWeb(DisplayLogg::m_vClaError);
             }
            else if(iCondId==LOG_CLASS_WARNING) {
               m_poLogg->GetInfoFromMem(DisplayLogg::m_vClaWarn,iMode,LOG_CLASS_WARNING,m_sParam,g_sStartDate,g_sEndDate);  
               if(g_iFdFront) DisPlayInfoForWeb( DisplayLogg::m_vClaWarn);	
            }
            else if(iCondId==LOG_CLASS_INFO) {
              m_poLogg->GetInfoFromMem(DisplayLogg::m_vClaInfo,iMode,LOG_CLASS_INFO,m_sParam,g_sStartDate,g_sEndDate); 
              if(g_iFdFront) DisPlayInfoForWeb( DisplayLogg::m_vClaInfo); 	 
            }
            else
            {
            	 char errstr[500]={0};
            	 strcat(errstr,"错误的日志类别.\n 日志类别如下\n");
            	 strcat(errstr,"1:FATAL/ERROR  2:WARNGING  3:INFO/DEBUG\n");
             		if(g_iFdFront)
            		{                 
                    ErrOutForWeb(errstr);
                    EndOutForWeb();            			
            		}else {
            	 		printf("%s",errstr);
            		}
               return false; 
             }    
           
        }
        else
        {//日志类别，三类，复用m_vAppInfo，m_vProInfo，m_vSystemInfo
            m_poLogg->GetInfoFromMem(DisplayLogg::m_vClaError,iMode,LOG_CLASS_ERROR,m_sParam,g_sStartDate,g_sEndDate);
            m_poLogg->GetInfoFromMem(DisplayLogg::m_vClaWarn,iMode,LOG_CLASS_WARNING,m_sParam,g_sStartDate,g_sEndDate);
            m_poLogg->GetInfoFromMem(DisplayLogg::m_vClaInfo,iMode,LOG_CLASS_INFO,m_sParam,g_sStartDate,g_sEndDate);
            if(g_iFdFront)
            {
            	StringCopy(vLogInfo,DisplayLogg::m_vClaError);
            	StringCopy(vLogInfo,DisplayLogg::m_vClaWarn);
            	StringCopy(vLogInfo,DisplayLogg::m_vClaInfo);
				DisPlayInfoForWeb(vLogInfo);
            }
        }
        break;
		case GROUPCODE:
        if(strlen(m_sParam))
        {
        	int iCode = atoi(m_sParam);
        	m_poLogg->GetInfoFromMem(DisplayLogg::m_vCodeInfo,iMode,iCode,m_sParam,g_sStartDate,g_sEndDate);
        }
		if(g_iFdFront) DisPlayInfoForWeb(DisplayLogg::m_vCodeInfo);
		
        break;
    default:
        return false;
    }
    /*
    sort(DisplayLogg::m_vAppInfo.begin(),DisplayLogg::m_vAppInfo.end(),GreaterMark);
    sort(DisplayLogg::m_vProInfo.begin(),DisplayLogg::m_vProInfo.end(),GreaterMark);
    sort(DisplayLogg::m_vAlarmInfo.begin(),DisplayLogg::m_vAlarmInfo.end(),GreaterMark);
    sort(DisplayLogg::m_vParamInfo.begin(),DisplayLogg::m_vParamInfo.end(),GreaterMark);
    sort(DisplayLogg::m_vSystemInfo.begin(),DisplayLogg::m_vSystemInfo.end(),GreaterMark);
    sort(DisplayLogg::m_vSysOperInfo.begin(),DisplayLogg::m_vSysOperInfo.end(),GreaterMark);
    
    sort(DisplayLogg::m_vLevFatal.begin(),DisplayLogg::m_vLevFatal.end(),GreaterMark);
    sort(DisplayLogg::m_vLevError.begin(),DisplayLogg::m_vLevError.end(),GreaterMark);
    sort(DisplayLogg::m_vLevWarn.begin(),DisplayLogg::m_vLevWarn.end(),GreaterMark);
    sort(DisplayLogg::m_vLevInfo.begin(),DisplayLogg::m_vLevInfo.end(),GreaterMark);
    sort(DisplayLogg::m_vLevDebug.begin(),DisplayLogg::m_vLevDebug.end(),GreaterMark);
    sort(DisplayLogg::m_vClaError.begin(),DisplayLogg::m_vClaError.end(),GreaterMark);
    sort(DisplayLogg::m_vClaWarn.begin(),DisplayLogg::m_vClaWarn.end(),GreaterMark);
    sort(DisplayLogg::m_vClaInfo.begin(),DisplayLogg::m_vClaInfo.end(),GreaterMark);
    sort(DisplayLogg::m_vTypeBusi.begin(),DisplayLogg::m_vTypeBusi.end(),GreaterMark);
    sort(DisplayLogg::m_vTypeParam.begin(),DisplayLogg::m_vTypeParam.end(),GreaterMark);
    sort(DisplayLogg::m_vTypeSystem.begin(),DisplayLogg::m_vTypeSystem.end(),GreaterMark);
    sort(DisplayLogg::m_vTypeStart.begin(),DisplayLogg::m_vTypeStart.end(),GreaterMark);
    sort(DisplayLogg::m_vTypeOracle.begin(),DisplayLogg::m_vTypeOracle.end(),GreaterMark);   
    */ 
    if(g_iFdFront == 0)
    {
    	DisplayLogg::LoggProcessMain(iMode,m_sParam);
    }
    return true;
}


/*
 *	函 数 名:   DisPlayInfoForWeb
 *	函数功能:   预处理解析参数
 *	时    间:   2011年3月15日
 *	返 回 值:   bool
 *	参数说明:  
*/
bool LoggMgr::DisPlayInfoForWeb(vector < string > & v_list)
{
    int cnt = v_list.size();
    char sOut[1024]={0};

    sprintf(sOut,"时间%s级别%s类别%s编码%s编码名称%s进程ID%s线程ID%s模块ID%s对端网元%s流向%s工号%s操作类型%s操作字段%s日志内容",
    							DEFINE_SPLIT,DEFINE_SPLIT,DEFINE_SPLIT,DEFINE_SPLIT,DEFINE_SPLIT,DEFINE_SPLIT,DEFINE_SPLIT,DEFINE_SPLIT,
                  DEFINE_SPLIT,DEFINE_SPLIT,DEFINE_SPLIT,DEFINE_SPLIT,DEFINE_SPLIT);
    FourOutForWeb(cnt,sOut);
   
    int i = (g_iStart > 0) ?  (g_iStart - 1) : 0;	
    int j = (g_iEnd < cnt) ? g_iEnd : cnt;

	int iLen = 0;
	
    for(; i != j; ++i)
    {	
      iLen = v_list[i].length();
	  if(iLen <= 0) continue;
	  
	  if(v_list[i][iLen-1] != '\n')
	  {
        cout << v_list[i].c_str() << endl;
	  }
	  else
	  {
	  	cout << v_list[i].c_str();
	  }
	}
    
    EndOutForWeb();
    
    return true;
}

/*
 *	函 数 名:   prepare
 *	函数功能:   预处理解析参数
 *	时    间:   2011年3月15日
 *	返 回 值:   bool
 *	参数说明:  
*/
bool LoggMgr::prepare()
{
    int i = 0;
    bool bErrParam = false,bHasNone = false,bNoSup = false;
    bool bFind = false;
    
    if(g_argc == 1)
    {
        printUsage();
        return true;
    }
    while(i<g_argc)
    {
        if(g_argv[i][0]!='-')
        {
            i++;
            continue;
        }
        bFind = true;
        int ilen = strlen(g_argv[i]);
        switch(g_argv[i][1])
        {
            case 'a':
                {
                    if(strcmp(g_argv[i],"-a")!=0)
                        bNoSup = true;
                    m_iMode = PIGEONHOLE;
                }
                break;
            case 'b':
                {
                    if(strcmp(g_argv[i],"-b")!=0)
                        bNoSup = true;
                    m_iMode = GROUPBAK;
                    if(g_argv[++i])
                    {
                         m_iGroupId = atoi(g_argv[i]);
                         if(m_iGroupId<1 ||m_iGroupId>GROUP_LOG_NUMS)
                            bErrParam= true;
                    }
                    else
                        m_iGroupId = -1;
                    if(g_argv[++i])
                        strcpy(m_sPath,g_argv[i]);
                }
                break;
            case 'c':
                {
                    if(strcmp(g_argv[i],"-c")==0)
                        m_iMode = GROUPCREAT;
                    else if(strncmp(g_argv[i],"-class",6)==0)
                    {
						//modify by liyr:现在没有type，class类似以前的type
						//m_iMode = GROUPCLASS;
                        m_iMode = GROUPTYPE;
						
                        AnalyzesParam(g_argv[g_argc-1],m_sParam);
                    }
					else if(strcmp(g_argv[i],"-code") == 0)
					{
						m_iMode = GROUPCODE;
						if(i == g_argc-1)
						{
							printf("请输入错误码或者操作码!\n");
							return false;
						}
						AnalyzesParam(g_argv[g_argc-1],m_sParam);
					}
					else
                        bNoSup = true;
                    i++;
                }
                break;
            case 'd':
                {
                    if(strcmp(g_argv[i],"-d")!=0)
                        bNoSup = true;
                    m_iMode = GROUPDELETE;
                    if(g_argv[++i])
                    {
                        m_iGroupId = atoi(g_argv[i]);
                        if(m_iGroupId<1 ||m_iGroupId>GROUP_LOG_NUMS)
                            bErrParam = true;
                    }
                    else
                        bHasNone = true;
                    if(g_argv[++i])
                        strcpy(m_sFileName,g_argv[i]);
                    else
                        bHasNone = true;
                }
                break;
            case 'e':
                {
                    if(strcmp(g_argv[i],"-e")!=0)
                        bNoSup = true;
                    m_iMode = GROUPSET;
                    if(g_argv[++i])
                    {
                        m_bPigeonhole = atoi(g_argv[i]);
                        int phole = atoi(g_argv[i]);
                        if(phole<0 || phole>1)
                            bErrParam = true;
                    }
                    else
                        bHasNone = true;
                }
                break;
            case 'f':
                {
                    if(strcmp(g_argv[i],"-f")!=0)
                        bNoSup = true;
                    m_iMode = GROUPFREE;
                }
                break;
            case 'i':
                {
                    if(strcmp(g_argv[i],"-i")!=0)
                        bNoSup = true;
                    m_iMode = GROUPFILEADD;
                    if(g_argv[++i])
                    {
                        if(strcmp(g_argv[i],"all"))
                        {
                            m_iGroupId = atoi(g_argv[i]);
                            if(m_iGroupId<1 ||m_iGroupId>GROUP_LOG_NUMS)
                                bErrParam = true;
                        }
                        else
                            m_iGroupId = 0;
                    }
                    else
                        bHasNone = true;
                    if(g_argv[++i])
                        strcpy(m_sPath,g_argv[i]);
                }
                break;

            case 'g':
                {
                    if(strcmp(g_argv[i],"-g")!=0)
                        bNoSup = true;
                    m_iMode = GROUPADD;
                }
                break;
            case 'l':
                {
                    if(strncmp(g_argv[i],"-level",6)==0)
                    {
                        m_iMode = GROUPINFOLV;
                        AnalyzesParam(g_argv[g_argc-1],m_sParam);
                    }
                    else if(strcmp(g_argv[i],"-l")==0)
                    {
                        m_iMode = GROUPPRINTF;
                        if(g_argv[++i])
                        {
                            if(i<g_argc) {
                            	m_iGroupId = atoi(g_argv[i]);
                            	if(m_iGroupId<1 ||m_iGroupId>GROUP_LOG_NUMS)
                                	bErrParam = true;    
                            }
                        } 
                    }
                    else
                        bNoSup = true;
                    i++;
                }
                break;
            case 'p':
                {
                    if(strncmp(g_argv[i],"-param",6)==0)
                    {
                        m_iMode = GROUPPARAM;
                        AnalyzesParam(g_argv[g_argc-1],m_sParam);
                    }
                    else if(strncmp(g_argv[i],"-process",8)==0)
                    {
                        m_iMode = GROUPPROCESS;
                        AnalyzesParam(g_argv[g_argc-1],m_sParam);
                    }
                    else
                        bNoSup = true;
                    i++;
                }
                break;
            case 'r':
                {
                    if(strcmp(g_argv[i],"-reset")==0)
                       m_iMode = GROUPRESET;
                    if(g_argv[++i])
                    {
                         m_iGroupId = atoi(g_argv[i]);
                    }else{
                         m_iGroupId=0;
                     } 
                    
                                         
                }
                break;                
            case 's':
                {
                    if(strcmp(g_argv[i],"-s")==0)
                    {
                        m_iMode = GROUPQUIT;
                        if(g_argv[++i])
                        {
                       		if(i<g_argc) {
                           		 m_iGroupId = atoi(g_argv[i]);
                           		 if(m_iGroupId<1 ||m_iGroupId>GROUP_LOG_NUMS)
                               		 bErrParam = true;    
                       		}
                        }                        
                    }
                    else if(strncmp(g_argv[i],"-stat",5)==0)
                        m_iMode = GROUPSTAT;
                    else
                        bNoSup = true;

                }
                break;
            case 'm':
                {
                    if(strncmp(g_argv[i],"-module",7)==0)
                    {
                        m_iMode = GROUPMODULE;
                        AnalyzesParam(g_argv[g_argc-1],m_sParam);
                    }
                    else
                        bNoSup = true;
                    i++;
                }
                break;
            case 't':
                {
                	if(strncmp(g_argv[i],"-type",6)==0)
                    {
                        m_iMode = GROUPTYPE;
                        AnalyzesParam(g_argv[g_argc-1],m_sParam);
                    }
                    else{
                      m_iMode = GROUPTEST;
                      AnalyzesParam(g_argv[g_argc-1],m_sParam);
                    }
                }
                break;
            default:
                {
                    Log::log(0,"应用进程参数不正确（请参考提示）");
                    printUsage();
                    return false;
                }
                break;
        }
        if(bHasNone==true)
        {
            Log::log(0,"应用进程参数不正确(缺少必选项)");
            return false;
        }
        if(bNoSup==true)
        {
            Log::log(0,"应用进程参数不正确（不支持的选项）");
            return false;
        }
        if(bErrParam==true)
        {
            Log::log(0,"应用进程参数不正确（错误的取值）");
            return false; 
        }
        i++;
    }
    
    if(bFind==false)
    {
        printUsage();
        Log::log(0,"应用进程参数不正确（不支持的选项）");
        return false;
    }
    
    //add by liyr
    if(m_iMode == GROUPQUIT || m_iMode == PIGEONHOLE)
    {
    	pPoint= new CheckPointMgr();
    }
    
    return true;
}

/*
 *	函 数 名:   init
 *	函数功能:   初始化
 *	时    间:   2011年3月15日
 *	返 回 值:   bool
 *	参数说明:  
*/
bool LoggMgr::init()
{
    //if(m_bParamErr)
    //{
        if(!m_RD)
        {
            Log::log(0,"获取核心参数失败");
            return false;
        }
        char *p;
        char sHome[500];
        memset(sHome,'\0',sizeof(sHome));
        if ((p=getenv ("SGW_HOME")) == NULL)
            sprintf (sHome, "/opt/opthb/etc");
        else
            sprintf (sHome, "%s/etc", p);
        if(sHome[strlen(sHome)-1] == '/') 
            sprintf (m_sParamFile, "%ssgwconfig", sHome);
        else
            sprintf (m_sParamFile, "%s/sgwconfig", sHome);
    //}
    
    if(m_iMode==GROUPCREAT || m_iMode==GROUPFREE || m_iMode==GROUPSTAT)
    {
        if(!m_poLoggMgr)
            m_poLoggMgr = new ThreeLogGroupMgr();
        
        long lCount = 0;

        m_iNum = m_RD->readIniInteger(m_sParamFile,"LOG","group_num",0);;

        lCount =  m_RD->readIniInteger(m_sParamFile,"MEMORY","log_data_size",0);

        if(m_iNum < 3)
        {
            Log::log(0,"获取核心参数日志组块数取值范围错误，请检查");
            return false;
        }
        if(lCount <= 0)
        {
            Log::log(0,"获取核心参数日志组数据区大小错误，请检查");
            return false;                    
        }
        m_poLoggMgr->setGroupNum(m_iNum);
        m_poLoggMgr->setGroupInfoDataCount(lCount);
    }
    else
    {
        if(!m_poLogg)
            m_poLogg = new ThreeLogGroup();
    }
    
    /*
    if(!m_pCommandComm && m_iMode != GROUPCREAT)
    {
        m_pCommandComm = new CommandCom();
        if(!m_pCommandComm->InitClient())
        {
            Log::log(0,"注册核心参数失败");
            //return false;
            m_bParamErr = true;
        }
        m_bParamErr= true;
      */
        /*
        if(!m_pCommandComm->GetInitParam("LOG"))
        {
            Log::log(0,"核心参数获取段(LOG)失败");
            //return false;
            m_bParamErr = true;
        }
        if(!m_pCommandComm->GetInitParam("MEMORY"))
        {
            Log::log(0,"核心参数获取段(MEMORY)失败");
            //return false;
            m_bParamErr = true;
        }
        */
      
    return true;
}


/*
 *	函 数 名:   prepare
 *	函数功能:   解析后处理函数
 *	时    间:   2011年3月15日
 *	返 回 值:   bool
 *	参数说明:  
*/
bool LoggMgr::deal()
{
    switch(m_iMode)
    {
        case GROUPCREAT:
            {
                if(m_poLoggMgr->create())
                    Log::log(0,"日志组共享内存创建成功!");
                else
                    Log::log(0,"日志组共享内存创建失败");
                //首次创建日志组
                if(!m_poLogg)
                    m_poLogg = new ThreeLogGroup(true);
                m_poLogg->InitLogGroup();
            }
            break;
        case GROUPFILEADD:
            {
                if(m_iGroupId==0)
                {//初始加载，默认两个文件
                    int iGroupNum = m_poLogg->GetGroupNum();
                    for(int num=1;num<=iGroupNum;++num)
                    {
                        if(!m_poLogg->AddLogGroupFile(num,m_sPath,true))
                            Log::log(0,"增加日志组文件失败");
                        if(!m_poLogg->AddLogGroupFile(num,m_sPath,true))
                            Log::log(0,"增加日志组文件失败");
                    }
                    Log::log(0,"增加日志组文件完成");
                }
                else
                {
                    if(m_poLogg->AddLogGroupFile(m_iGroupId,m_sPath,true))
                        Log::log(0,"增加日志组文件成功");
                    else
                        Log::log(0,"增加日志组文件失败");
                }
            }
            break;
        case GROUPBAK:
            {
                if(strlen(m_sPath)==0)
                {
                    char *pPath = m_poLogg->GetGroupBakPath();
                    strcpy(m_sPath,pPath);
                }
                m_poLogg->bakGroupFile(m_iGroupId,m_sPath);            
                Log::log(0,"日志组备份完成");
            }
            break;
        case GROUPDELETE:
            {
                if(m_poLogg->ClearGroupFile(m_iGroupId,m_sFileName,true))
                    Log::log(0,"删除日志组文件成功");
                else
                {
                    Log::log(0,"删除日志组文件失败");
                    return false;
                }
            }
            break;
        case GROUPFREE:
            {
                if(m_poLoggMgr->remove())
                    Log::log(0,"日志组共享内存删除成功!");
                else
                {
                    Log::log(0,"日志组共享内存删除失败!");
                    return false;
                }
            }
            break;
        case GROUPPRINTF:
            {
                m_poLogg->DisplayLogGroup(m_iGroupId);
            }
            break;
        case GROUPQUIT:
            {
                int iGroupId = m_poLogg->GetUseGroup();
                if(m_poLogg->ChangeLogGroup(false,m_iGroupId)==true)
                {
                    Log::log(0,"切换日志组成功!");
                    char stemp[200]={0};
                    int itempId = m_poLogg->GetUseGroup();
                    sprintf(stemp,"[logg] 日志组由组%d切换到组%d",iGroupId,itempId);
					Log::log(0,"%s",stemp);
					m_poLogg->log(MBC_THREELOG_CHANGE,LOG_LEVEL_INFO,LOG_TYPE_SYSTEM,"sgwlog -s 切换日志组成功:%s",stemp);
                }
                else
                {
                    Log::log(0,"切换日志组失败");
					m_poLogg->log(MBC_THREELOG_CHANGE,LOG_LEVEL_WARNING,LOG_TYPE_SYSTEM,"sgwlog -s 切换日志组失败");
                    return false;
                }
                pPoint->touchInfoDown();
                printf("CHECKPOINT执行成功\n");
                m_poLogg->DisplayLogGroup(m_iGroupId); 
            }
            break;
        case GROUPSET:
            {
                if(m_poLogg->SetLogGroupPigeonhole(m_bPigeonhole))
                {
                    if(m_bPigeonhole==true)
                        Log::log(0,"日志组归档开关已打开");
                    else
                        Log::log(0,"日志组归档开关已关闭");
                }
            }
            break;
        case PIGEONHOLE:
            {
                if(m_poLogg->ChangeLogGroup(true,m_iGroupId)==false)
                {
                    Log::log(0,"切换日志组失败");
					m_poLogg->log(MBC_THREELOG_PIGE,LOG_LEVEL_WARNING,LOG_TYPE_SYSTEM,"sgwlog -a 手动归档日志组失败");
                    return false;
                }
                else
                {
                	m_poLogg->log(MBC_THREELOG_PIGE,LOG_LEVEL_INFO,LOG_TYPE_SYSTEM,"sgwlog -a 手动归档日志组成功");
                    Log::log(0,"手动归档成功");
                }
            }
                pPoint->touchInfoDown();
                printf("CHECKPOINT执行成功\n");
            break;
        case GROUPPARAM:
            {
                if(strlen(m_sParam))
                {
                    if(!CheckParamName())
                    {
                    	   if(g_iFdFront)
                         {
                            char errstr[500]={0};
                            sprintf(errstr,"输入的核心参数【%s】不存在,请通过sgwparam -list命令查询所有参数",m_sParam);
                            ErrOutForWeb(errstr);
                            EndOutForWeb();
                         }
                         else                         
                        	printf("输入的核心参数【%s】不存在,请通过sgwparam -list命令查询所有参数\n",m_sParam);                        
                       
                        return false;
                    }
                }
                //走统一接口20100623
                if(!DisplayInfo(m_iMode))
                {
                    //Log::log(0,"提取数据检索失败,请检查提取条件是否正确！");
                    return false;
                }
            }
            break;

        case GROUPMODULE:
        	    if(strlen(m_sParam))
                { 
                	int iAppid;
					bool bAppID;
					if(isdigit(m_sParam[0])) {
						iAppid = atoi(m_sParam) ;
						bAppID = true;
        	    	} else {
        	    		iAppid = GetAppIDByName(m_sParam);
						bAppID = false;
        	    	}
					
                    if(!CheckAppId(iAppid))
                    {
                    	  if(g_iFdFront)
                         {
                            char errstr[500]={0};
                            sprintf(errstr,"输入的Module%s【%s】不存在,请通过sgwstat -ses查询模块信息\n",(bAppID?"ID":"Name"),m_sParam);
                            ErrOutForWeb(errstr);
                            EndOutForWeb();
                         }
                         else
                        	printf("输入的Module%s【%s】不存在,请通过sgwstat -ses查询模块信息\n",(bAppID?"ID":"Name"),m_sParam);
                        		
                        return false;
                    }
					if(!bAppID) sprintf(m_sParam,"%d",iAppid);
                }
                DisplayInfo(m_iMode);
               break ;
        case GROUPPROCESS:
        	    if(strlen(m_sParam))
                {
                	  int iProcessid=atoi(m_sParam) ;
                    if(!CheckProcessId(iProcessid))
                    {
                    	   if(g_iFdFront)
                         {
                            char errstr[500]={0};
                            sprintf(errstr,"输入的ProcessID【%s】不存在,请通过sgwstat -ses查询进程信息\n",m_sParam);
                            ErrOutForWeb(errstr);
                            EndOutForWeb();
                         }
                         else
                        	printf("输入的ProcessID【%s】不存在,请通过sgwstat -ses查询进程信息\n",m_sParam);
                   
                        return false;
                    }
                }
                DisplayInfo(m_iMode);
               break ;        	
        case GROUPCLASS:        	
        case GROUPINFOLV:
        case GROUPTYPE:
		case GROUPCODE:
            {
                if(!DisplayInfo(m_iMode))
                {
                    //Log::log(0,"提取数据检索失败,请检查提取条件是否正确！");
                    return false;
                }
            }
            break;
        case GROUPSTAT:
            {
                //int iNum = m_pCommandComm->readIniInteger("LOG","group_num",-1);
               /* int iNum=m_iNum ;
                printf("*********************************************************\n");
                for(int i=1;i<=iNum;i++)
                {
                    printf("GROUP_ID:%d\n",i);
                    printf("\n");
                    int iDataCnt = m_poLoggMgr->GetShmDataCount(i);
                    int iDataTotal = m_poLoggMgr->GetShmDataTotalCount(i);
                    printf("SHM_DATA_USED:%d\n",iDataCnt);
                    printf("SHM_DATA_TOTAL:%d\n",iDataTotal);
                    printf("\n");
                    int iIndexCnt = m_poLoggMgr->GetShmIndexCount(i);
                    int iIndexTotal = m_poLoggMgr->GetShmIndexTotalCount(i);
                    printf("SHM_INDEX_USED:%d\n",iIndexCnt);
                    printf("SHM_INDEX_TOTAL:%d\n",iIndexTotal);
                    printf("\n");

                    int iIndexDataTotal = m_poLoggMgr->GetShmIndexDataTotal(i);
                    printf("SHM_INDEX_DATA_TOTAL:%d\n",iIndexDataTotal);

                    for(int j=1;j<iIndexTotal;j++)
                    {
                        int iIndexDataCount = m_poLoggMgr->GetShmIndexDataCount(i,j);
                        if(iIndexDataCount>0)
                            printf("INDEX(%d):COUNT:%d\n",j,iIndexDataCount);
                    }
                    printf("-------------------\n");
                }
                printf("*********************************************************\n");
               */
			m_poLoggMgr->showShmInfo();
            }
            break;
        case GROUPTEST:
            {
                int cnt = atoi(m_sParam);
                for(int i=0;i!=cnt;i++)
                {                
                    m_poLogg->log(1000,7,2,LOG_TYPE_PROC,0,"","7|1000|2|1|10|0|888|OcpTranMgr.cpp|20110628151814|||0|0||进程调度\n");
                    m_poLogg->log(1000,7,2,LOG_TYPE_PROC,0,"LOG.log_info_level","7|1000|2|2|10|0|888|OcpTranMgr.cpp|20110628151814|LOG.log_info_level||0|0||参数修改\n");
                    m_poLogg->log(1000,7,2,LOG_TYPE_PROC,0,"","7|1000|2|3|10|0|888|OcpTranMgr.cpp|20110628151815|||0|0||业务信息\n");
                    m_poLogg->log(1000,7,2,LOG_TYPE_PROC,0,"","7|1000|2|4|10|0|888|OcpTranMgr.cpp|20110628151812|||0|0||状态变更\n");
                    m_poLogg->log(1000,7,2,LOG_TYPE_PROC,0,"","7|1000|2|5|10|0|888|OcpTranMgr.cpp|20110628151816|||0|0||系统操作\n");
                }
            }
            break;
        case GROUPRESET:
            {
                if(m_iGroupId==0)
                {//初始加载，默认两个文件
                    int iGroupNum = m_poLogg->GetGroupNum();
                    for(int num=1;num<=iGroupNum;++num)
                    {
                        if(!m_poLogg->CallBackInfo(num))
                            printf("清空日志组[%d]内存失败\n",num);
                    }
                    printf("清空日志组完成\n");
                }
                else
                {
                     if(!m_poLogg->CallBackInfo(m_iGroupId))
                         printf("清空日志组[%d]内存失败\n",m_iGroupId);
                    else
                         printf("清空日志组完成\n");
                }
            }
        	 break ;
        default:
            break;
    }
    return true;
}

/*
 *	函 数 名:   run
 *	函数功能:   主函数
 *	时    间:   2011年3月15日
 *	返 回 值:   bool
 *	参数说明:  
*/
int LoggMgr::run()
{
    if(!prepare())
    {
        Log::log(0,"sgwlog预处理失败");
        return -2;
    }
    if(m_iMode==0)
        return 0;
    if(!init())
    {
        Log::log(0,"sgwlog初始化失败");
        return -1;
    }
    if(!deal())
    {
        Log::log(0,"sgwlog执行失败");
        return -3;
    }
    return 0;
}

/*
int main(int argc, char **argv)
{
    g_argc = argc;
    g_argv = argv;
    LoggMgr * pMgr = new LoggMgr();
    if(!pMgr)
    {
        Log::log(0,"创建LoggMgr对象失败");
        return -1;
    }
    pMgr->run();
    delete pMgr;
    return 0;
}
*/

/*
 *	函 数 名:   CheckProcessId
 *	函数功能:   检查ProcessID是否正确
 *	时    间:   2011年7月24日
 *	返 回 值:   bool
 *	参数说明:  
*/
bool LoggMgr::CheckProcessId(int iProcessID)
{
	try{
		char sSqlcode[512]={0};
	  //TOCIQuery qry (Environment::getDBConn());
	  DEFINE_QUERY(qry );
	  sprintf (sSqlcode, "select a.process_id "
  						         "from wf_process a "
 						           "where  a.Process_Id =%d ",iProcessID);
	  qry.setSQL(sSqlcode);
	  qry.open();
	  if (qry.next()) 
	  {
	  	qry.close();
	  	return true ;
	  }

	  qry.close();
	  return false;
	}
	catch(TOCIException &e) 
	{
		ThreeLogGroup Log;
		if(e.getErrCode()==MBC_DB_LINK_FAIL)
			Log.log(MBC_DB_LINK_FAIL,LOG_LEVEL_FATAL,LOG_TYPE_SYSTEM,e.getErrMsg());
		if(e.getErrCode()==MBC_DB_SERV_PWD_ERROR)
			Log.log(MBC_DB_SERV_PWD_ERROR,LOG_LEVEL_FATAL,LOG_TYPE_SYSTEM,e.getErrMsg());
	}
	
}	

/*
 *	函 数 名:   CheckAppId
 *	函数功能:   检查AppID是否正确
 *	时    间:   2011年7月24日
 *	返 回 值:   bool
 *	参数说明:  
*/
bool LoggMgr::CheckAppId(int iAppID)
{
	try{
		char sSqlcode[512]={0};
	  //TOCIQuery qry (Environment::getDBConn());
	  DEFINE_QUERY(qry );
	  sprintf (sSqlcode, "select a.app_id "
  						         "from wf_application a "
 						           "where  a.app_id =%d ",iAppID);
	  qry.setSQL(sSqlcode);
	  qry.open();
	  if (qry.next()) 
	  {
	  	qry.close();
	  	return true ;
	  }

	  qry.close();
	  return false;
	}
	catch(TOCIException &e) 
	{
		ThreeLogGroup Log;
		if(e.getErrCode()==MBC_DB_LINK_FAIL)
			Log.log(MBC_DB_LINK_FAIL,LOG_LEVEL_FATAL,LOG_TYPE_SYSTEM,e.getErrMsg());
		if(e.getErrCode()==MBC_DB_SERV_PWD_ERROR)
			Log.log(MBC_DB_SERV_PWD_ERROR,LOG_LEVEL_FATAL,LOG_TYPE_SYSTEM,e.getErrMsg());
	}
	
}	

int LoggMgr::GetAppIDByName(char *sAppName)
{
	char sSqlcode[512]={0};
	int iAppID = -1;
	DEFINE_QUERY(qry );
	sprintf (sSqlcode, "select a.APP_ID from wf_application a "
 					   "where  a.EXEC_NAME = '%s' ",sAppName);
	qry.setSQL(sSqlcode);
	qry.open();
	if (qry.next()) 
	{
		iAppID = qry.field(0).asInteger(); 
	}

	qry.close();
	
	return iAppID;
}


DEFINE_COMMAND_MAIN (LoggMgr);

