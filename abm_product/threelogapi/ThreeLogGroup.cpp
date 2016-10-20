#include "ThreeLogGroup.h"
#include "ThreeLogGroupBase.h"
#include "MBC_ABM.h"

ThreeLogGroup::ThreeLogGroup(bool bGreat):ThreeLogGroupBase(),m_lMaxLogFileSize(0),
                m_lMaxLogDirSize(0),m_iCurUseGroupId(0),m_poSeqExt(0),m_iWriteMode(1),
                m_lInfoDataSize(0),m_iMaxGroupNum(0),m_iCheckPointTouch(75)
{
    char sTemp[32];
    memset(m_sBakLogGroupPath,0,sizeof(m_sBakLogGroupPath));
    memset(m_sDefaultLogGroupPath,0,sizeof(m_sDefaultLogGroupPath));
    memset(sTemp,0,sizeof(sTemp));
               

    init();
    if(bGreat==false)
    {
        GetUseGroup();
        bindGroupData(m_iCurUseGroupId);
        GetCurUsedInfoData(m_iCurUseGroupId);
        GetCurUsedInfoIndex(m_iCurUseGroupId);
        GetCurUsedInfoLock(m_iCurUseGroupId);

		//修正从配置文件读取的值,三个日志组共享内存大小一致，任意取
		m_lInfoDataSize = m_poInfoDataAll[1]->getTotal();
    }
    m_poSeqExt = new SeqMgrExt ("seq_log_group_id",1);
    
     m_iLogProid=-1;
     m_iLogAppid=-1;
     m_iLogLevel=0;    
     memset(m_ssAreaCode,0,sizeof(m_ssAreaCode));
     
     m_pErrorCode = new HBErrorCode();
     if(!m_pErrorCode)
     	 printf("create m_pErrorCode error\n");
     m_pErrorCode->init();
     
}

ThreeLogGroup::~ThreeLogGroup()
{
    if(m_poSeqExt)
    {
        delete m_poSeqExt;
        m_poSeqExt=0;
    }
}


/*
 *	函 数 名 : init
 *	函数功能 : 日志组获取核心参数的接口
 *	时    间 : 2010年9月13日
 *	返 回 值 : bool
 *	参数说明 : bRegister-是否注册成功
*/
bool ThreeLogGroup::init()
{
    char sPath[100],sBakPath[100];
    bool balarm = false;

    memset(sPath,0,sizeof(sPath));
    memset(sBakPath,0,sizeof(sBakPath));

    //注册核心参数失败，从配置文件读
    char sHome[254]={0};
    char *p;
    ReadIni RD;

    if ((p=getenv (ENV_HOME)) == NULL)
        sprintf (sHome, "/opt/opthb/etc");
    else
        sprintf (sHome, "%s/etc", p);
        
    char sFile[254];

    if (sHome[strlen(sHome)-1] == '/') {
        sprintf (sFile, "%sabmconfig", sHome);
    } else {
        sprintf (sFile, "%s/abmconfig", sHome);
    }
	
    m_lInfoDataSize = RD.readIniInteger(sFile,"MEMORY","log_data_size",100000);

	int iFileSize = RD.readIniInteger(sFile,"LOG","log_size",0);
    if(iFileSize==0)
    {
        Log::log(0,"核心参数log.size没有配置，取默认文件大小");
        iFileSize = 200;
    }
    SetGroupMaxFileSize(iFileSize);
		
    int iGroupSize = RD.readIniInteger(sFile,"LOG","log_file_dir_size",0);
    if(iGroupSize==0)
    {
        Log::log(0,"核心参数log.log_file_dir_size没有配置，取默认日志组大小");
        iGroupSize = 700;
    }
    SetGroupMaxSize(iGroupSize);
	
    m_iCheckPointTouch = RD.readIniInteger(sFile,"LOG","log_percent_load",75);
    
    m_iWriteMode = RD.readIniInteger(sFile,"LOG","log_group_mode",1);
    
    RD.readIniString(sFile,"LOG","log_group_path",sPath,NULL);
    SetGroupDefaultPath(sPath);
    
    memset(sPath,0,sizeof(sPath));
    RD.readIniString(sFile,"LOG","1.1.path",sPath,NULL);
    
    SetGroupFilePath(1,1,sPath);
    memset(sPath,0,sizeof(sPath));
    RD.readIniString(sFile,"LOG","1.2.path",sPath,NULL);
    SetGroupFilePath(1,2,sPath);
    memset(sPath,0,sizeof(sPath));
    RD.readIniString(sFile,"LOG","2.1.path",sPath,NULL);
    SetGroupFilePath(2,1,sPath);
    memset(sPath,0,sizeof(sPath));
    RD.readIniString(sFile,"LOG","2.2.path",sPath,NULL);
    SetGroupFilePath(2,2,sPath);
    memset(sPath,0,sizeof(sPath));
    RD.readIniString(sFile,"LOG","3.1.path",sPath,NULL);
    SetGroupFilePath(3,1,sPath);
    memset(sPath,0,sizeof(sPath));
    RD.readIniString(sFile,"LOG","3.2.path",sPath,NULL);
    SetGroupFilePath(3,2,sPath);
    /*
	  memset(sPath,0,sizeof(sPath));
	  RD.readIniString(sFile,"log","4.1.path",sPath,NULL);
	  SetGroupFilePath(4,1,sPath);
	  memset(sPath,0,sizeof(sPath));
	  RD.readIniString(sFile,"log","4.2.path",sPath,NULL);
	  SetGroupFilePath(4,2,sPath);
	  memset(sPath,0,sizeof(sPath));
	  RD.readIniString(sFile,"log","5.1.path",sPath,NULL);
	  SetGroupFilePath(5,1,sPath);
	  memset(sPath,0,sizeof(sPath));
	  RD.readIniString(sFile,"log","5.2.path",sPath,NULL);
	  SetGroupFilePath(5,2,sPath);
	  memset(sPath,0,sizeof(sPath));
	  RD.readIniString(sFile,"log","6.1.path",sPath,NULL);
	  SetGroupFilePath(6,1,sPath);
	  memset(sPath,0,sizeof(sPath));
	  RD.readIniString(sFile,"log","6.2.path",sPath,NULL);
	  SetGroupFilePath(6,2,sPath);
    */
		
     RD.readIniString(sFile,"LOG","log_archive_path",sBakPath,NULL);
     SetGroupBakPath(sBakPath);
     
     m_iMaxGroupNum = RD.readIniInteger(sFile,"LOG","group_num",0);
     if(m_iMaxGroupNum==0)
     {
         Log::log(0,"获取核心参数log.group_num没有配置，取默认值");
         m_iMaxGroupNum = 3;
     }
       
     RD.readIniString(sFile,"SYSTEM","area_code",m_ssAreaCode,NULL);
        
     return true;
}


/*
 *	函 数 名 : InitLogGroup
 *	函数功能 : 初始化日志组
 *	时    间 : 2010年6月26日
 *	返 回 值 : void
 *	参数说明 : 
*/
void ThreeLogGroup::InitLogGroup()
{
    ThreeLogGoupData * p = m_poLogGroup;
    if(!p)
    {
        LoggLog(0,"日志组获取共享内存信息失败");
        THROW(MBC_Log_Group+4);
    }
    for(int num=1;num<=m_iMaxGroupNum;++num)
    {//初始化日志组，并创建两个默认配置的成员文件
        if(!AddLogGroup(num)) continue ;
        char stemp[50],stempPath[500];
        memset(stemp,0,sizeof(stemp));
        memset(stempPath,0,sizeof(stempPath));
        sprintf(stemp,"%d_1",num);
        string sFileId_1(stemp);
        strcpy(stempPath,m_mFilePathMap.find(sFileId_1)->second.c_str());
        if(AddLogGroupFile(num,stempPath,true)==false)
        {
		Log::log(0,"%s",stempPath);
            THROW(MBC_Log_Group+7);
        }
        memset(stemp,0,sizeof(stemp));
        memset(stempPath,0,sizeof(stempPath));
        sprintf(stemp,"%d_2",num);
        string sFileId_2(stemp);
        strcpy(stempPath,m_mFilePathMap.find(sFileId_2)->second.c_str());
        if(AddLogGroupFile(num,stempPath,true)==false)
        {
            THROW(MBC_Log_Group+7);            
        }
    }
}


/*
 *	函 数 名 : AddLogGroupFile
 *	函数功能 : 添加日志组成员文件
 *	时    间 : 2010年6月26日
 *	返 回 值 : bool
 *	参数说明 : GroupId-日志组标识;pPath-文件路径;bmml-是否外围控制增加
 *             此功能函数可于外围命令添加调用或者是日志组内部切换更换流水文件调用
*/
bool ThreeLogGroup::AddLogGroupFile(int GroupId,char * pPath,bool bmml)
{
    unsigned int i ;
    int count=-1,fd;
    long seq=0;
    char tempPath[500]={0};
    ThreeLogGoupData * p = m_poLogGroup;

    if(!p)
    {
        return false;
    }
    
    if(!m_poLogGroupIndex->get(GroupId, &i))
    {
        return false;        
    }

    if(bmml == true)
    {//是否来源于外围命令
        m_poDataLock->P();
        p[i].m_iSeq++;
        if(p[i].m_iSeq > GROUP_FILE_CNT)
        {
            p[i].m_iSeq--;
            m_poDataLock->V();
            Log::log(0,"超过日志组成员文件最大数");
            return  false;
        }
        m_poDataLock->V();
        if(strlen(pPath)==0)
        {//20100803增加成员文件默认存储路径的核心参数
            if(strlen(m_sDefaultLogGroupPath))
                strcpy(pPath,m_sDefaultLogGroupPath);
            else
            {
                return false;
            }
        }
        if(strlen(pPath))
        {
            for(int j=0;j!=GROUP_FILE_CNT;j++)
            {
                if(strlen(p[i].m_File[j].m_sFileName)==0)
                {
                    count = j;
                    sprintf(p[i].m_File[count].m_sGroupPath,"%s",pPath);
                    break;
                }
            }            
        }
        if(count == -1)
        {
            return false;
        }
        m_poDataLock->P();
        strcpy(p[i].m_File[count].m_sFileName,"abmlog");
        p[i].m_File[count].m_lFileSize = 0;
        m_poDataLock->V();
        CheckLogDir(p[i].m_File[count].m_sGroupPath);
        sprintf(tempPath,"%s%s",p[i].m_File[count].m_sGroupPath,p[i].m_File[count].m_sFileName);
        if((fd=open(tempPath, O_RDWR|O_CREAT|O_TRUNC, 0770)) < 0)
        {
            return false;
        }
        close(fd);
        return true;
    }
    else
    {
        for(int j=0;j!=GROUP_FILE_CNT;j++)
        {  
            if(strlen(p[i].m_File[j].m_sFileName)==0)
                continue;
            m_poDataLock->P();
            strcpy(p[i].m_File[j].m_sFileName,"abmlog");
            p[i].m_File[j].m_lFileSize = 0;
            m_poDataLock->V();
            CheckLogDir(p[i].m_File[j].m_sGroupPath);
            sprintf(tempPath,"%s%s",p[i].m_File[j].m_sGroupPath,p[i].m_File[j].m_sFileName);
            if((fd=open (tempPath, O_RDWR|O_CREAT|O_TRUNC, 0770)) < 0)
            {
                return false;
            }
            close(fd);
        }    
    }
    return true;
}


/*
 *	函 数 名 : AddLogGroup
 *	函数功能 : 添加日志组
 *	时    间 : 2010年6月26日
 *	返 回 值 : bool
 *	参数说明 : GroupId-日志组标识
*/
bool ThreeLogGroup::AddLogGroup(int GroupId)
{
    unsigned int i ;
	ThreeLogGoupData * p = m_poLogGroup;

    if(!m_poLogGroupIndex->get(GroupId, &i)) 
    {
        i = m_poLogGroupData->malloc();
        if(!i) 
        {
            LoggLog(0,"日志组申请内存失败");
            THROW(MBC_Log_Group+1);
        }
        m_poIndexLock->P();
        m_poLogGroupIndex->add(GroupId,i);
        m_poIndexLock->V();
        m_poDataLock->P();
        p[i].m_iGoupId = GroupId;
        p[i].m_iSeq = 0;
        if(p[i].m_iGoupId == 1)
            p[i].m_iState = CURRENT;
        else
            p[i].m_iState = INACTIVE;
        p[i].m_lCurFileSize = 0;
        p[i].m_lCurDirSize = 0;
		p[i].m_lPredictFileSize = 0;
        p[i].m_bPigeonhole = true;
        p[i].m_bCheckPoint = false;
        p[i].m_bPigeonholeState = UNDOWN;
        p[i].m_iWritSeq = 0;
        p[i].m_iMemSeq = 0;
        p[i].m_iLevel = 5;
		p[i].m_iFileCheckPercent = 90;
		p[i].m_lFileSizeThreshold = m_lMaxLogFileSize;
        if(GroupId!=m_iMaxGroupNum)
            p[i].m_iNext = GroupId +1;
        else
            p[i].m_iNext = 1;
        m_poDataLock->V();
        return true;
    }
   return false ;

}

//add by liyr
bool  ThreeLogGroup::AddInfoDataInfo(int iCodeId,int iLogProcId,int iThreadId,int iLogAppId,int iLogLevel,
						 int iLogClassId,int iLogTypeId,int iAction,int iNetId,const char *sFlow,const char *pExpand,const char *pData,ThreeLogGoupData* p)
{
    unsigned int i=0;
    unsigned int m=0;

    if(!m_poIndexLock || !m_poDataLock || !m_poInfoDataLock)
        return false;
    if(!m_poInfoGroup ||!m_poInfoData ||!m_poInfoIndex)
        return false;

    LogInfoData * pInfo = m_poInfoGroup;
    if(!pInfo)
    {
        LoggLog(0,"获取日志组数据区共享内存信息失败\n");
        return false;
    }
    m_poInfoDataLock->P();
    i = m_poInfoData->malloc();
    m_poInfoDataLock->V();
    if(i<=0)
    {
        LoggLog(0,"日志组内存数据区申请内存失败\n");
        m_poInfoDataLock->V();
        return false;
    }
    m_dDT.getCurDate();
    pInfo[i].m_illCode = iCodeId ;
    pInfo[i].m_illAppId = iLogAppId;
    pInfo[i].m_illProId = iLogProcId;
	pInfo[i].m_iThreadId = iThreadId;
    pInfo[i].m_illLevelId = iLogLevel;
    pInfo[i].m_illClassId = iLogClassId;
    pInfo[i].m_illTypeId = iLogTypeId;
    pInfo[i].m_illActionId = iAction;
    if(pExpand==NULL) strcpy(pInfo[i].m_sActionName,"NULL");
    else              strcpy(pInfo[i].m_sActionName,pExpand);
    strncpy(pInfo[i].m_sInfo,pData,LOG_DATA_SIZE-1);
    //strncpy(pInfo[i].m_sLogTime,m_dDT.toString(),14);
	sprintf(pInfo[i].m_sLogTime,"%s%03d",m_dDT.toString(),m_dDT.getMilSec()/1000);
    if(strlen(m_ssAreaCode)==0) strcpy(pInfo[i].m_sAreaCode,"025");
    else   strncpy(pInfo[i].m_sAreaCode,m_ssAreaCode,4);
    //strcpy(pInfo[i].m_sCodeName,	m_pErrorCode->getCauseInfo(iCodeId) );
    strncpy(pInfo[i].m_sCodeName,m_pErrorCode->getCauseInfo(iCodeId),GROUP_NAME_LENGTH-1);

	if(sFlow == NULL) {
		strcpy(pInfo[i].m_sFlow,"NULL");
	} else {
		strcpy(pInfo[i].m_sFlow,sFlow);
	}
	
    pInfo[i].m_iNextOffset = 0;
    m_poIndexLock->P();
    if(m_poInfoIndex->get(iCodeId,&m))
    {
        pInfo[i].m_iNextOffset = pInfo[m].m_iNextOffset;
        pInfo[m].m_iNextOffset = i;
    }
    else
        m_poInfoIndex->add(iCodeId,i);
    m_poIndexLock->V();

	char sStr[1024] = {0};

	//获得落地文件大小
	combineLog(pInfo+i,sStr,2);
	int iLen = strlen(sStr) + 1;
    m_poDataLock->P();
    p->m_iMemSeq++;
	p->m_lPredictFileSize += iLen;
    m_poDataLock->V();
    return true;

}


/*
 *	函 数 名 : bakGroupFile
 *	函数功能 : 备份指定日志组文件
 *	时    间 : 2010年6月26日
 *	返 回 值 : 
 *	参数说明 : LogGroupId-要备份的日志组标识; path-要备份到的目录
*/
void ThreeLogGroup::bakGroupFile(int LogGroupId,char *path)
{
    char scmd[1024]={0};
    char TempPath[500],tempName[500],tempbak[500];
    unsigned int i;

    if(strlen(path)==0)
    {
        Log::log(0,"未指定备份目录\n");
        return;
    }
    CheckLogDir(path);
    if(LogGroupId==-1)
        LogGroupId = GetUseGroup();
    if(!m_poLogGroupIndex->get(LogGroupId, &i))
    {
        Log::log(0,"关联日志组共享内存失败\n");
        return ;        
    }
    ThreeLogGoupData * p = m_poLogGroup + i;
    if(!p)
    {
        Log::log(0, "获取日志组共享内存数据失败\n");
        return;
    }
    for(int j=0;j!=GROUP_FILE_CNT;j++)
    {
        if(strlen(p->m_File[j].m_sGroupPath) && strlen(p->m_File[j].m_sFileName))
        {
            memset(tempName,0,sizeof(tempName));
            memset(TempPath,0,sizeof(TempPath));
            memset(tempbak,0,sizeof(tempbak));
            strcpy(tempName,p->m_File[j].m_sFileName);
            sprintf(TempPath,"%s%s",p->m_File[j].m_sGroupPath,p->m_File[j].m_sFileName);
            if(!GetPigeonholeFileName(tempName))
                sprintf(tempbak,"%s%s",path,p->m_File[j].m_sFileName);
            else
                sprintf(tempbak,"%s%s",path,tempName);
            sprintf(scmd,"cp -f %s %s",TempPath,tempbak);
            system(scmd);
        }
    }
}

/*
 *	函 数 名 : LoggLog
 *	函数功能 : 日志组内部日志
 *	时    间 : 2010年6月26日
 *	返 回 值 : 
 *	参数说明 : iLogType=0 :维护日志 iLogType=1:备份日志
*/
void ThreeLogGroup::LoggLog(int iLogType,char const * format,...)
{
    char sTemp[10000], sLine[10000];
    int fd;
    char sLoggName[256];
    
    memset(sLoggName,0,sizeof(sLoggName));

    if(iLogType==0)
	    sprintf (sLoggName, "%s/Log_group_alarm.log",m_sDefaultLogGroupPath);
	else
	    sprintf (sLoggName, "%s/Log_group_bak.log",m_sBakLogGroupPath);

    Date dt;
    sprintf (sTemp, "[%s]: %s\n", dt.toString("yyyy-mm-dd hh:mi:ss"), format);

    va_list ap;
    va_start(ap, format);
    vsnprintf (sLine, 9999, sTemp, ap);
    va_end(ap);

    if((fd=open (sLoggName, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
        return;
    write (fd, sLine, strlen (sLine));
    close (fd);
}


/*
 *	函 数 名 : CheckLogDir
 *	函数功能 : 检查配置目录是否创建
 *	时    间 : 2010年6月26日
 *	返 回 值 : 
 *	参数说明 : 
*/
void ThreeLogGroup::CheckLogDir(char* Path)
{
    DIR *dirp=NULL;

    if(strlen(Path)==0)
        return;
    dirp = opendir(Path);
    if(dirp==NULL)
    {
        mkdir(Path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
    }
    else
        closedir(dirp);
    if(Path[strlen(Path)-1] != '/')
    {
        strcat(Path, "/");
    }
}

/*
 *	函 数 名 : CheckLogFile
 *	函数功能 : 检查日志组文件
 *	时    间 : 2010年6月26日
 *	返 回 值 : 
 *	参数说明 : 
*/
bool ThreeLogGroup::CheckLogFile(int iGroupId)
{
    unsigned int i;
    char tempPath[500];
    ThreeLogGoupData * p = m_poLogGroup;

   // for(int num=1;num<=m_iMaxGroupNum;++num)
    //{
        if(!m_poLogGroupIndex->get(iGroupId,&i))
            THROW(MBC_Log_Group+4);
        if(p[i].m_iSeq==0)
        {//如果当前日志组文件数位0，则默认创建两个
            AddLogGroupFile(iGroupId,m_sDefaultLogGroupPath,true);
            AddLogGroupFile(iGroupId,m_sDefaultLogGroupPath,true);
            GetGroupFilePath(iGroupId);
            //continue;
        }
        for(int j=0;j!=GROUP_FILE_CNT;j++)
        {
            if(strlen(p[i].m_File[j].m_sFileName)==0)
                continue;
            if(strlen(p[i].m_File[j].m_sGroupPath)==0)
                continue;
            CheckLogDir(p[i].m_File[j].m_sGroupPath);
            memset(tempPath,0,sizeof(tempPath));
            sprintf(tempPath,"%s%s",p[i].m_File[j].m_sGroupPath,p[i].m_File[j].m_sFileName);
            if(access(tempPath, F_OK)!=0)
            {
                Log::log(0,"日志组%d的成员文件:%s被删除，请检查！",iGroupId,tempPath);
                log(MBC_FILE_LOG_FAIL,LOG_LEVEL_WARNING,LOG_TYPE_SYSTEM,"日志组%d的成员文件:%s被删除，请检查！",iGroupId,tempPath);
				return false;
            }
        }
    //}

	return true;
}

/*
 *	函 数 名:   CheckParam
 *	函数功能:   检查数据字段是否符合查询条件
 *	时    间:   2011年3月15日
 *	返 回 值:   bool
 *	参数说明:   iLogClassId-日志类别，sParam-执行参数,iLogTypeId-分类标识
*/
bool ThreeLogGroup::CheckParam(int iLogTypeId,int iLogDisFlag,int iLogClassId,char *sParam,LogInfoData *pInfo)
{
    int iAppid=-1,iProid=-1,iLevelId=-1,iClassId=-1;

    if(iLogTypeId!=pInfo->m_illTypeId)
        return false;

    switch(iLogDisFlag)
    {
    case GROUPPARAM:
        if(strlen(sParam)==0)
            return true;
        if(strcmp(sParam,pInfo->m_sActionName)!=0)
            return false;
        break;
    case GROUPMODULE:
        if(strlen(sParam)==0)
            return true;
        iAppid = atoi(sParam);
        if(pInfo->m_illAppId!=iAppid)
            return false;
        break;
    case GROUPPROCESS:
        if(strlen(sParam)==0)
            return true;
        iProid = atoi(sParam);
        if(pInfo->m_illProId!=iProid)
            return false;
        break;
    case GROUPINFOLV:
        if(strlen(sParam)==0)
            return true;
        iLevelId = atoi(sParam);
        if(pInfo->m_illLevelId!=iLevelId)
            return false;
        break;
    case GROUPCLASS:
        if(iLogClassId==0)
            return true;
        if(iLogClassId!=pInfo->m_illClassId)
            return false;
        break;
    default:
        return false;
    }
    return true;
}

//刷新归档路径，归档前调用
void ThreeLogGroup::refreshArchivePath()
{
	char sHome[254]={0};
	char *p;
	ReadIni RD;
	
	if ((p=getenv (ENV_HOME)) == NULL)
		sprintf (sHome, "/opt/opthb/etc");
	else
		sprintf (sHome, "%s/etc", p);
	 
	char sFile[254] = {0};
	
	if (sHome[strlen(sHome)-1] == '/') {
		sprintf (sFile, "%sabmconfig", sHome);
	} else {
		sprintf (sFile, "%s/abmconfig", sHome);
	}

	char sBakPath[100] = {0};
	RD.readIniString(sFile,"LOG","log_archive_path",sBakPath,NULL);
    SetGroupBakPath(sBakPath);
}

/*
 *	函 数 名 : RefreshLogFilePath
 *	函数功能 : 刷新日志文件路径
 *	时    间 : 2011年9月28日
 *	返 回 值 : bool
 *	参数说明 : GroupId-日志组标识  iFileSeq-文件的个数顺序   sNewPath-新路径
*/
bool  ThreeLogGroup::RefreshLogFilePath(int iGroupID,int iFileSeq,char *sNewPath)
{
    unsigned int i ;
	
	if(iGroupID > m_iMaxGroupNum || iGroupID < 1)
	{
		return false;
	}

	if(iFileSeq < 1 || iFileSeq > GROUP_FILE_CNT)
	{
		return false;
	}

	if(strlen(sNewPath) == 0)
	{
		return false;
	}
	    
    if(!m_poLogGroupIndex->get(iGroupID, &i))
    {
        return false;
    }

	ThreeLogGoupData * p = m_poLogGroup + i;
	if(!p)
    {
        return false;
    }

	char sCmd[1024] = {0};
	iFileSeq--;
	CheckLogDir(sNewPath);
	if(strcmp(p->m_File[iFileSeq].m_sGroupPath,sNewPath))
	{
		sprintf(sCmd,"cp -f %s%s %s",p->m_File[iFileSeq].m_sGroupPath,p->m_File[iFileSeq].m_sFileName,sNewPath);
				
		system(sCmd);
		
		memset(p->m_File[iFileSeq].m_sGroupPath,0,sizeof(p->m_File[iFileSeq].m_sGroupPath));

		sprintf(p->m_File[iFileSeq].m_sGroupPath,"%s",sNewPath);
	}

	return true;
}

/*
 *	函 数 名 : ChangeLogGroup
 *	函数功能 : 切换日志组
 *	时    间 : 2010年6月26日
 *	返 回 值 : bool
 *	参数说明 : GroupId-要切换到的日志组标识;bmml-是否来自外围命令
*/
bool ThreeLogGroup::ChangeLogGroup(bool bmml,int GroupId)
{
    unsigned int icur,ilast;

    if(!m_poLogGroupIndex->get(m_iCurUseGroupId,&icur))
    {
        LoggLog(0,"关联日志组共享内存失败\n");
        return false;        
    }
    ThreeLogGoupData *pCur = m_poLogGroup + icur;
    if(!pCur)
    {
        LoggLog(0,"关联日志组共享内存失败\n");
        return false;
    }
    ThreeLogGoupData *pNext = 0;
    if(GroupId!=-1)
    {//指定目标切换
        if(GroupId >GROUP_LOG_NUMS || GroupId < 1 || GroupId> m_iMaxGroupNum)
        {
            LoggLog(0,"无法识别的日志组\n");
            return false;
        }
        if(!m_poLogGroupIndex->get(GroupId, &ilast))
        {
            LoggLog(0,"关联日志组共享内存失败\n");
            return false;
        }
        pNext =  m_poLogGroup + ilast;
        if(!pNext)
        {
            LoggLog(0,"关联日志组共享内存失败\n");
            return false;
        }
        if(pNext->m_iState!=INACTIVE)
        {
            if(m_iWriteMode==WRITESHARE)
            {//给出告警
                LoggLog(0,"指定切换的日志组不为空闲态，请重新指定\n");
                return false;
            }
        }
    }
    else
    {//自动切换
        bool bFind = false;
        int iNextId = pCur->m_iNext;
        for(int i=1;i<=m_iMaxGroupNum;i++)
        {
            if(!m_poLogGroupIndex->get(iNextId, &ilast))
            {
                LoggLog(0,"关联日志组共享内存失败\n");
                return false;
            }
            pNext =  m_poLogGroup + ilast;
            if(!pNext)
            {
                LoggLog(0,"关联日志组共享内存失败\n");
                return false;
            }
            if(pNext->m_iState!=INACTIVE)
            {
                if(m_iWriteMode==WRITESHARE)
                    iNextId = pNext->m_iNext;
            }
            else
            {
                bFind = true;
				break;
            }
        }
        if(bFind==false)
        {//无空闲的日志组
            LoggLog(0,"当前没有空闲的日志组，请检查日志组及清仓进程状态\n");
            return false;
        }
    }
    if(pNext->m_iSeq==0)                                   
    {//如果当前日志组文件数为0，则默认创建两个           
        AddLogGroupFile(pNext->m_iGoupId,m_sDefaultLogGroupPath,true);
        AddLogGroupFile(pNext->m_iGoupId,m_sDefaultLogGroupPath,true);
        GetGroupFilePath(pNext->m_iGoupId);
    }
    if(pNext->m_lCurDirSize == 0)
    {//如果切换的日志组为空，直接切换
        m_poDataLock->P();
        pCur->m_iState = ACTIVE;
        if(m_iWriteMode==WRITESHARE)
        {
            //if(bmml)
            //{//来源于命令的归档
            //    pCur->m_bPigeonholeState = NEEDDOWN;
            //}
			pCur->m_bPigeonholeState = NEEDDOWN;
        }
        pNext->m_iState = CURRENT;
        pNext->m_bCheckPoint = false;
        pNext->m_bPigeonholeState = UNDOWN;
		pNext->m_lPredictFileSize = 0;
        m_iCurUseGroupId = pNext->m_iGoupId;
        m_poDataLock->V();
		if(bmml)
		{
			CommitInfoFile();
		}
        //重新定位数据区
        bindGroupData(m_iCurUseGroupId);
        GetCurUsedInfoData(m_iCurUseGroupId);
        GetCurUsedInfoIndex(m_iCurUseGroupId);
        GetCurUsedInfoLock(m_iCurUseGroupId);
        return true;
    }

    if(pNext->m_bPigeonhole == true && pNext->m_bPigeonholeState != ALLDOWN)
    {//需要归档
    	refreshArchivePath();
        bakGroupFile(pNext->m_iGoupId,m_sBakLogGroupPath);    
        m_poDataLock->P();
        pNext->m_iState = INACTIVE;
        pNext->m_bPigeonholeState=ALLDOWN;
        m_poDataLock->V();
    }
    char tempPath[500];
    for(int num =0;num!=GROUP_FILE_CNT;++num)
    {//不需要归档，直接清理
        if(strlen(pNext->m_File[num].m_sFileName)==0)
            continue;
        memset(tempPath,0,sizeof(tempPath));
        sprintf(tempPath,"%s%s",pNext->m_File[num].m_sGroupPath,pNext->m_File[num].m_sFileName);
        if(ClearGroupFile(pNext->m_iGoupId,tempPath)==false)
            return false;
    }
    if(AddLogGroupFile(pNext->m_iGoupId,NULL)==false)
        return false;

    m_poDataLock->P();
    pCur->m_iState = ACTIVE;
    //if(bmml)
    //{//来源于命令的归档
    //    pCur->m_bPigeonholeState = NEEDDOWN;
    //}

	pCur->m_bPigeonholeState = NEEDDOWN;
	
    pNext->m_iState = CURRENT;
    pNext->m_bCheckPoint = false;
    pNext->m_bPigeonholeState=UNDOWN;
    m_iCurUseGroupId = pNext->m_iGoupId;
    pNext->m_lCurDirSize = 0;
    pNext->m_lCurFileSize = 0;
	pNext->m_lPredictFileSize = 0;
    m_poDataLock->V();
	if(bmml)
	{
		CommitInfoFile();
	}
    //重新定位数据区
    bindGroupData(m_iCurUseGroupId);
    GetCurUsedInfoData(m_iCurUseGroupId);
    GetCurUsedInfoIndex(m_iCurUseGroupId);
    GetCurUsedInfoLock(m_iCurUseGroupId);
    return true;
}


/*
 *	函 数 名 : ClearTempGroup
 *	函数功能 : 删除日志组成员文件
 *	时    间 : 2010年6月26日
 *	返 回 值 : bool
 *	参数说明 : GroupId-日志组标识; pFileName-成员文件名; bmml-是否外围命令增加
*/
bool ThreeLogGroup::ClearGroupFile(int LogGroupId,char *pFileName,bool bmml)
{
    char scmd[1024]={0};
    char tempPath[500];
    unsigned int i;

    if(LogGroupId >GROUP_LOG_NUMS || LogGroupId < 1 || LogGroupId > m_iMaxGroupNum)
    {
        LoggLog(0,"无法识别的日志组\n");
        return false;        
    }
    if(!m_poLogGroupIndex->get(LogGroupId,&i))
    {
        LoggLog(0,"关联日志组共享内存失败\n");
        return false;        
    }

    ThreeLogGoupData * p = m_poLogGroup + i;
        
    if(!p)
    {
        LoggLog(0,"关联日志组共享内存失败\n");
        return false;
    }

    for(int j=0;j!=GROUP_FILE_CNT;j++)
    {
        if(strlen(p->m_File[j].m_sFileName))
        {
            memset(tempPath,0,sizeof(tempPath));
            sprintf(tempPath,"%s%s",p->m_File[j].m_sGroupPath,p->m_File[j].m_sFileName);
            if(strcmp(tempPath,pFileName)==0)
            {
                sprintf(scmd,"rm -r %s",tempPath);
                system(scmd);
                m_poDataLock->P();
                p->m_File[j].m_lFileSize = 0;
                if(bmml==true)
                {
                    LoggLog(0,"日志组%d中删除了一个成员文件:%s",p->m_iGoupId,p->m_File[j].m_sFileName);
                    p->m_iSeq--;
                    memset(p->m_File[j].m_sFileName,0,sizeof(p->m_File[j].m_sFileName));
                    memset(p->m_File[j].m_sGroupPath,0,sizeof(p->m_File[j].m_sGroupPath));
                }
                m_poDataLock->V();
                return true;
            }
        }           
    }
    LoggLog(0,"该日志组没有找到符合条件的文件，请检查\n");
    return false;
}

/*
 *	函 数 名 : DisplayLogGroup
 *	函数功能 : 展现指定日志组信息
 *	时    间 : 2010年6月26日
 *	返 回 值 : 
 *	参数说明 : GroupId-要展现的日志组标识
*/
bool ThreeLogGroup::DisplayLogGroup(int GroupId,char * sDest)
{
    unsigned int i;
/*
    if(m_bAttached==false)
    {
        THROW(MBC_Log_Group+4);
    }
*/
	if(m_bAttached == false)
	{
		initBase();
		GetUseGroup();
		bindGroupData(m_iCurUseGroupId);
		GetCurUsedInfoData(m_iCurUseGroupId);
		GetCurUsedInfoIndex(m_iCurUseGroupId);
		GetCurUsedInfoLock(m_iCurUseGroupId);
	}

    if(GetUseGroup()<=0)
    {
        THROW(MBC_Log_Group+12);
    }
    if(GroupId == -1)
        GroupId = m_iCurUseGroupId;           

    if(GroupId > GROUP_LOG_NUMS || GroupId <1 || GroupId > m_iMaxGroupNum)
    {
        THROW(MBC_Log_Group+11);
    }

    if(!m_poLogGroupIndex->get(GroupId, &i))
    {
        THROW(MBC_Log_Group+4);
    }

    ThreeLogGoupData * p = m_poLogGroup + i;
    if(!p)
    {
        THROW(MBC_Log_Group+4);
    }


	char sStr[10240] = {0},sStrTmp[1024] = {0};
	sprintf(sStrTmp,"日志组标识:%d(%d)\n",p->m_iGoupId,m_iMaxGroupNum);
	strcat(sStr,sStrTmp);
    if(p->m_iState==CURRENT)
        sprintf(sStrTmp,"日志组状态:CURRENT\n");
    else if(p->m_iState==INACTIVE)
        sprintf(sStrTmp,"日志组状态:INACTIVE\n");
    else if(p->m_iState==ACTIVE)
        sprintf(sStrTmp,"日志组状态:ACTIVE\n");
    else
        return false;
	strcat(sStr,sStrTmp);
	
    if(p->m_bPigeonhole)
        sprintf(sStrTmp,"日志组归档标识:打开\n");
    else
        sprintf(sStrTmp,"日志组归档标识:关闭\n");
	strcat(sStr,sStrTmp);

    if(p->m_bPigeonholeState==UNDOWN)
        sprintf(sStrTmp,"日志组归档状态:未归档\n");
    else if(p->m_bPigeonholeState==ALLDOWN)
        sprintf(sStrTmp,"日志组归档状态:已归档\n");
    else if(p->m_bPigeonholeState==NEEDDOWN)
        sprintf(sStrTmp,"日志组归档状态:等待归档\n");
    else if(p->m_bPigeonholeState==FILEDOWN)
        sprintf(sStrTmp,"日志组归档状态:已落地\n");	
	strcat(sStr,sStrTmp);

    if(m_iWriteMode==WRITEFILE)
        sprintf(sStrTmp,"日志组写方式: 文件\n");
    else if(m_iWriteMode==WRITESHARE)
        sprintf(sStrTmp,"日志组写方式: 内存\n");
	strcat(sStr,sStrTmp);

	sprintf(sStrTmp,"日志组等级:%d\n",p->m_iLevel);
	strcat(sStr,sStrTmp);
        
    sprintf(sStrTmp,"日志组文件数:%d\n",p->m_iSeq);
	strcat(sStr,sStrTmp);

	sprintf(sStrTmp,"日志组成员文件列表:\n");
	strcat(sStr,sStrTmp);

	DisplayFileList(p->m_iGoupId,sStrTmp);
	strcat(sStr,sStrTmp);

    if(m_iWriteMode!=WRITESHARE)
    {//直接写文件的时候才需要展示
        double size = GetGroupSize(GroupId);
        double used = GetGroupPercent(GroupId);
        double unused = 1-used;
        printf("逻辑空间使用:%6.3f M\n",size);
        printf("逻辑空间使用率:%6.3f %%\n",used*100);
        printf("逻辑空间可用率:%6.3f %%\n",unused*100);
    }
    sprintf(sStrTmp,"物理序数:%d \n",p->m_iWritSeq);
	strcat(sStr,sStrTmp);
    sprintf(sStrTmp,"内存序数:%d \n",p->m_iMemSeq);
	strcat(sStr,sStrTmp);
    sprintf(sStrTmp,"内存使用:%d \n",GetGroupInfoUsed(GroupId));
	strcat(sStr,sStrTmp);
    //sprintf(sStrTmp,"内存总数:%d \n",m_lInfoDataSize);
	sprintf(sStrTmp,"内存总数:%d \n",GetGroupInfoTotal(GroupId));
	strcat(sStr,sStrTmp);
    sprintf(sStrTmp,"内存使用率:%6.3f %%\n",(GetGroupInfoPercent(GroupId))*100);
	strcat(sStr,sStrTmp);
    //sprintf(sStrTmp,"文件大小限制:%ld M\n",(m_lMaxLogFileSize/(1024*1024)));
    sprintf(sStrTmp,"文件大小限制:%ld M\n",(p->m_lFileSizeThreshold/(1024*1024)));
	strcat(sStr,sStrTmp);
    sprintf(sStrTmp,"日志组大小限制:%ld M\n",(m_lMaxLogDirSize/(1024*1024)));
	strcat(sStr,sStrTmp);
    sprintf(sStrTmp,"日志组归档文件备份目录:%s \n\n",m_sBakLogGroupPath);
	strcat(sStr,sStrTmp);

	if(sDest)
	{
		strcpy(sDest,sStr);
	} 
	else
	{
		printf("*******************************************\n");	
		printf("%s",sStr);
		printf("*******************************************\n");
	}
	return true;
}


/*
 *	函 数 名 : DisplayFileList
 *	函数功能 : 展现指定日志组成员文件信息
 *	时    间 : 2010年6月26日
 *	返 回 值 : 
 *	参数说明 : GroupId-要展现的日志组标识
*/
void ThreeLogGroup::DisplayFileList(int GroupId,char *sDest)
{
    unsigned int i;
    
    if(!m_poLogGroupIndex->get(GroupId, &i))
    {
        THROW(MBC_Log_Group+4);
    }
    ThreeLogGoupData * p = m_poLogGroup + i;
    if(!p)
    {
        THROW(MBC_Log_Group+4);
    }

	char sStr[2048] = {0},sStrTmp[1024] = {0};
    for(int j=0;j!=GROUP_FILE_CNT;++j)
    {
        double dFileSize=0;
        if(strlen(p->m_File[j].m_sFileName)==0)
            continue;
        
        sprintf(sStrTmp," 文件名  :%s\n",p->m_File[j].m_sFileName);
		strcat(sStr,sStrTmp);
		
        sprintf(sStrTmp," 文件路径:%s\n",p->m_File[j].m_sGroupPath);
		strcat(sStr,sStrTmp);
		
        if(p->m_File[j].m_lFileSize < 1024)
            sprintf(sStrTmp," 文件大小:%ld Byte\n",p->m_File[j].m_lFileSize);
        else if(p->m_File[j].m_lFileSize < 1024 *1024)
        {
            dFileSize = p->m_File[j].m_lFileSize/1024.000;
            sprintf(sStrTmp," 文件大小:%6.3f K\n",dFileSize);
        }
        else
        {
            dFileSize = ((int)(p->m_File[j].m_lFileSize/1024.000)/1024.000);
            sprintf(sStrTmp," 文件大小:%6.3f M\n",dFileSize);
        }
		strcat(sStr,sStrTmp);
       	strcat(sStr,"\n");
    }

	if(sDest)
	{
		strcpy(sDest,sStr);
	}
	else
	{
		printf("%s",sStr);
	}
	
    return ;
}

/*
 *	函 数 名 : GetGroupNum
 *	函数功能 : 获取当前日志组块数
 *	时    间 : 2010年6月26日
 *	返 回 值 : int
 *	参数说明 : 
*/
int ThreeLogGroup::GetGroupNum()
{
    return m_iMaxGroupNum;
}


/*
 *	函 数 名 : GetUseGroup
 *	函数功能 : 获取当前正在运行的日志组标识
 *	时    间 : 2010年6月26日
 *	返 回 值 : int
 *	参数说明 : 
*/
int ThreeLogGroup::GetUseGroup()
{
    unsigned int i;
    ThreeLogGoupData * p = m_poLogGroup;
    bool bFind = false;
/*
    if(m_bAttached==false)
       return -1;
*/    
    if(!p)
    {
//        Log::log(0,"日志组获取共享内存信息失败\n");
        LoggLog(0,"日志组获取共享内存信息失败\n");
        return -1;
    }

    for(int num=1;num<=m_iMaxGroupNum;++num)
    {
        if(!m_poLogGroupIndex->get(num,&i))
        {
//            Log::log(0,"日志组获取共享内存信息失败\n");
            LoggLog(0,"日志组获取共享内存信息失败\n");
            return -1;            
        }
        p = m_poLogGroup + i;
        if(p->m_iState==CURRENT)
        {
            m_iCurUseGroupId = p->m_iGoupId;
            bFind = true;
            break;
        }
    }
    if(bFind==false)
    {
//        Log::log(0,"日志组状态出错\n");
        LoggLog(0,"日志组状态出错\n");
        return -1;    
    }
    return m_iCurUseGroupId;
}


/*
 *	函 数 名 : GetGroupSize
 *	函数功能 : 获取指定日志组大小
 *	时    间 : 2010年6月26日
 *	返 回 值 : 日志组大小(单位M)
 *	参数说明 : GroupId-日志组标识
*/
double ThreeLogGroup::GetGroupSize(int GroupId)
{
    unsigned int i;
    
    if(GroupId >GROUP_LOG_NUMS || GroupId < 1 || GroupId>m_iMaxGroupNum)
        GroupId = m_iCurUseGroupId;
    if(!m_poLogGroupIndex->get(GroupId,&i))
    {
        THROW(MBC_Log_Group+4);
    }
    ThreeLogGoupData * p  = m_poLogGroup + i;
    if(!p)
    {
        THROW(MBC_Log_Group+4);
    }
    if(p->m_iGoupId == GroupId)
    {
       double dGroupSize = ((int)(p->m_lCurDirSize/1024.000)/1024.000);
       dGroupSize = floor(dGroupSize*100.000)/100.000;
       return dGroupSize;
    }
    return 0;
}

/*
 *	函 数 名 : GetGroupPercent
 *	函数功能 : 获取指定日志组利用率
 *	时    间 : 2010年6月26日
 *	返 回 值 : double
 *	参数说明 : GroupId-日志组标识
*/

double ThreeLogGroup::GetGroupPercent(int GroupId)
{
    unsigned int i;
    
    if(GroupId >GROUP_LOG_NUMS || GroupId < 1 || GroupId>m_iMaxGroupNum)
        GroupId = m_iCurUseGroupId;    
    if(!m_poLogGroupIndex->get(GroupId,&i))
    {
        THROW(MBC_Log_Group+4);
    }
    ThreeLogGoupData * p  = m_poLogGroup + i;
    if(!p)
    {
        THROW(MBC_Log_Group+4);
    }
    if(p->m_iGoupId == GroupId)
    {
        double temp = p->m_lCurDirSize/(m_lMaxLogDirSize*1.000);
        return temp;
    }
    return 0;
}

/*
 *	函 数 名 : GetGroupInfoPercent
 *	函数功能 : 获取在用日志组共享内存区利用率
 *	时    间 : 2010年9月3日
 *	返 回 值 : double
 *	参数说明 : GroupId-日志组标识
*/

double ThreeLogGroup::GetGroupInfoPercent(int GroupId)
{
    int usedcnt = GetGroupInfoUsed(GroupId);
    if(usedcnt==0)
        return 0;
    //double temp = usedcnt/(m_lInfoDataSize*1.000);

    int totalcnt = GetGroupInfoTotal(GroupId);

	double temp = usedcnt*1.0/totalcnt;  
	
    return temp;
}

/*
 *	函 数 名 : GetGroupState
 *	函数功能 : 获取指定日志组状态
 *	时    间 : 2010年6月26日
 *	返 回 值 : double
 *	参数说明 : GroupId-日志组标识
*/
int ThreeLogGroup::GetGroupState(int GroupId,char *sRetVal)
{
    unsigned int i;
	if(m_bAttached == false)
	{
		initBase();
		GetUseGroup();
		bindGroupData(m_iCurUseGroupId);
		GetCurUsedInfoData(m_iCurUseGroupId);
		GetCurUsedInfoIndex(m_iCurUseGroupId);
		GetCurUsedInfoLock(m_iCurUseGroupId);
	}


    if(GroupId >GROUP_LOG_NUMS || GroupId < 1 || GroupId>m_iMaxGroupNum)
        GroupId = m_iCurUseGroupId;    
    if(!m_poLogGroupIndex->get(GroupId,&i))
    {
        THROW(MBC_Log_Group+4);
    }
    ThreeLogGoupData * p  = m_poLogGroup + i;
    if(!p)
    {
        THROW(MBC_Log_Group+4);
    }
    if(p->m_iGoupId == GroupId)
    {
    	if(sRetVal == NULL) 
		{
			return p->m_iState;
    	}

		strcpy(sRetVal,"UnKnown");
		if(p->m_iState == CURRENT)
    	{
    		strcpy(sRetVal,"在用");
    	} 
		else if (p->m_iState == INACTIVE)
		{
			strcpy(sRetVal,"空闲");
		}
		else if(p->m_iState == ACTIVE)
		{
			strcpy(sRetVal,"待清仓");
		}
		
        return p->m_iState;
    }
	
    return 0;
}

/*
 *	函 数 名 : GetGroupWriteMode
 *	函数功能 : 获取日志组写数据方式
 *	时    间 : 2010年9月9日
 *	返 回 值 : int
 *	参数说明 : 0-写文件 1-写共享内存
*/
int ThreeLogGroup::GetGroupWriteMode()
{
    return m_iWriteMode;
}

/*
 *	函 数 名 : GetGroupPigeonhole
 *	函数功能 : 获取指定日志组归档标识状态
 *	时    间 : 2010年9月9日
 *	返 回 值 : bool
 *	参数说明 : GroupId-日志组标识
*/
bool ThreeLogGroup::GetGroupPigeonhole(int GroupId)
{
    unsigned int i;
    
    if(GroupId >GROUP_LOG_NUMS || GroupId < 1 || GroupId>m_iMaxGroupNum)
        GroupId = m_iCurUseGroupId;    
    if(!m_poLogGroupIndex->get(GroupId,&i))
    {
        THROW(MBC_Log_Group+4);
    }
    ThreeLogGoupData * p  = m_poLogGroup + i;
    if(!p)
    {
        THROW(MBC_Log_Group+4);
    }
    if(p->m_iGoupId == GroupId)
        return p->m_bPigeonhole;
    return false;    
}

/*
 *	函 数 名 : UpdateLogFileArchiveSeq
 *	函数功能 : 设置sequence从1开始
 *	时    间 : 2011年9月15日
 *	返 回 值 : void
 *	参数说明 : 
*/
void ThreeLogGroup::UpdateLogFileArchiveSeq()
{
	#ifdef __SQLITE__
		
	DEFINE_QUERY (qry);
	char sSQL[512] = {0};
	sprintf (sSQL, "update sqlite_sequence set seq = 1 where name = 'seq_log_group_id'");
	qry.setSQL (sSQL); 
	qry.execute ();
	qry.commit ();
	qry.close ();

	#endif

	return;
}


/*
 *	函 数 名 : GetPigeonholeFileName
 *	函数功能 : 获取归档文件名
 *	时    间 : 2010年9月15日
 *	返 回 值 : bool
 *	参数说明 : pFileName -返回的文件名
*/
bool ThreeLogGroup::GetPigeonholeFileName(char *pFileName)
{
    char *p;

/*    
    char temp[500];

    p = strchr(pFileName,'_');
    if(!p)
        return false;
    p++;
    memset(temp,0,sizeof(temp));
    strcpy(temp,p);
*/
    memset(pFileName,0,sizeof(pFileName));
    long seq = m_poSeqExt->getNextVal();
    Date dt;
    sprintf(pFileName,"abmlog_archive_%s%04ld",dt.toString("yyyymmdd"),seq);
    return true;
}

/*
 *	函 数 名 : GetGroupFileSeq
 *	函数功能 : 获取指定日志组成员文件数
 *	时    间 : 2010年9月9日
 *	返 回 值 : bool
 *	参数说明 : GroupId-日志组标识
*/
int ThreeLogGroup::GetGroupFileSeq(int GroupId)
{
    unsigned int i;
    
    if(GroupId >GROUP_LOG_NUMS || GroupId < 1 || GroupId>m_iMaxGroupNum)
        GroupId = m_iCurUseGroupId;    
    if(!m_poLogGroupIndex->get(GroupId,&i))
    {
        THROW(MBC_Log_Group+4);
    }
    ThreeLogGoupData * p  = m_poLogGroup + i;
    if(!p)
    {
        THROW(MBC_Log_Group+4);
    }
    if(p->m_iGoupId == GroupId)
        return p->m_iSeq;
    return 0;
}

/*
 *	函 数 名 : GetGroupInfoUsed
 *	函数功能 : 获取指定日志组内存块使用数
 *	时    间 : 2010年9月9日
 *	返 回 值 : int
 *	参数说明 : GroupId-日志组标识
*/
int ThreeLogGroup::GetGroupInfoUsed(int GroupId)
{
    if(GroupId<=0 || GroupId>GROUP_LOG_NUMS || GroupId > m_iMaxGroupNum)
        return 0;
    else
        return m_poInfoDataAll[GroupId]->getCount();
}

int ThreeLogGroup::GetGroupInfoTotal(int GroupId)
{
    if(GroupId<=0 || GroupId>GROUP_LOG_NUMS || GroupId > m_iMaxGroupNum)
        return 1;
    else
        return m_poInfoDataAll[GroupId]->getTotal();

}

/*
 *	函 数 名 : GetGroupMaxFileSize
 *	函数功能 : 获取日志组文件大小限制
 *	时    间 : 2010年7月23日
 *	返 回 值 : long
 *	参数说明 : 返回单位为M
*/
long ThreeLogGroup::GetGroupMaxFileSize()
{
    return m_lMaxLogFileSize/(1024*1024);    
}

/*
 *	函 数 名 : GetGroupMaxSize
 *	函数功能 : 获取日志组大小限制
 *	时    间 : 2010年7月23日
 *	返 回 值 : long
 *	参数说明 : 返回单位为M
*/
long ThreeLogGroup::GetGroupMaxSize()
{
    return m_lMaxLogDirSize/(1024*1024);
}

/*
 *	函 数 名 : GetGroupBakPath
 *	函数功能 : 获取日志组归档后的备份目录
 *	时    间 : 2010年7月23日
 *	返 回 值 : char
 *	参数说明 : 
*/
char* ThreeLogGroup::GetGroupBakPath()
{
    return m_sBakLogGroupPath;
}

/*
 *	函 数 名 : GetCurUseFileName
 *	函数功能 : 获取当前正在写的文件名
 *	时    间 : 2010年7月28日
 *	返 回 值 : bool
 *	参数说明 : 
*/
bool ThreeLogGroup::GetCurUseFileName(int GroupId,char *pFileName,char *pFilePath)
{
    unsigned int i;

    if(GroupId >GROUP_LOG_NUMS || GroupId < 1 || GroupId>m_iMaxGroupNum)
        GroupId = m_iCurUseGroupId;   
    if(!m_poLogGroupIndex->get(GroupId,&i))
    {
        THROW(MBC_Log_Group+4);
    }
    ThreeLogGoupData * p  = m_poLogGroup + i;
    if(!p)
    {
        THROW(MBC_Log_Group+4);
    }
    for(int j=0;j!=GROUP_FILE_CNT;++j)
    {
        if(strlen(p->m_File[j].m_sFileName)==0)
            continue;
        strcpy(pFileName,p->m_File[j].m_sFileName);
        strcpy(pFilePath,p->m_File[j].m_sGroupPath);
        return true;
    }
    return false;
}

/*
 *	函 数 名 : GetCurUsedInfoData
 *	函数功能 : 获取当前正在用的共享内存数据区
 *	时    间 : 2010年9月18日
 *	返 回 值 : void
 *	参数说明 : GroupId-当前日志组标识
*/
bool ThreeLogGroup::GetCurUsedInfoData(int GroupId)
{
    if(GroupId<=0 || GroupId>GROUP_LOG_NUMS || GroupId>m_iMaxGroupNum)
        m_poInfoData=0;
    else
        m_poInfoData = m_poInfoDataAll[GroupId];

    return true;
}

/*
 *	函 数 名 : GetCurUsedInfoIndex
 *	函数功能 : 获取当前正在用的共享内存索引区
 *	时    间 : 2010年9月18日
 *	返 回 值 : void
 *	参数说明 : GroupId-当前日志组标识
*/
bool ThreeLogGroup::GetCurUsedInfoIndex(int GroupId)
{
    if(GroupId<=0 || GroupId>GROUP_LOG_NUMS || GroupId>m_iMaxGroupNum)
        m_poInfoIndex = 0;
    else
        m_poInfoIndex = m_poInfoIndexAll[GroupId];

    return true;    
}

/*
 *	函 数 名 : GetCurUsedInfoLock
 *	函数功能 : 获取当前正在用的共享内存区数据锁
 *	时    间 : 2010年9月18日
 *	返 回 值 : void
 *	参数说明 : GroupId-当前日志组标识
*/
bool ThreeLogGroup::GetCurUsedInfoLock(int GroupId)
{
    return true;    
}

/*
 *	函 数 名 : GetGroupFilePath
 *	函数功能 : 获取当前日志组文件路径配置
 *	时    间 : 2010年9月8日
 *	返 回 值 : 
 *	参数说明 : GroupId-日志组标识
*/
bool ThreeLogGroup::GetGroupFilePath(int GroupId)
{
    unsigned int i;
    int filecnt=0;
    map<string, string>::iterator mIter;
    
    for(int num=1;num<=m_iMaxGroupNum;++num)
    {
        if(!m_poLogGroupIndex->get(num, &i))
        {
            THROW(MBC_Log_Group+4);
        }
        ThreeLogGoupData * p = m_poLogGroup + i;
        if(!p)
        {
            THROW(MBC_Log_Group+4);
        }
        if(p->m_iGoupId!=GroupId)
            continue;
        for(int j=0;j!=GROUP_FILE_CNT;++j)
        {
            if(strlen(p->m_File[j].m_sFileName)==0)
                continue;
            filecnt++;
            char temp[10]={0};
            sprintf(temp,"%d_%d",num,filecnt);
            string sFileId(temp);
            mIter = m_mFilePathMap.find(sFileId);
            if(mIter == m_mFilePathMap.end())
            {//没有找到
                continue;
            }
            string sPath (m_mFilePathMap.find(sFileId)->second);
            m_poDataLock->P();
            strcpy(p->m_File[j].m_sGroupPath,sPath.c_str());
            m_poDataLock->V();
        }
    }
    return true;
}


/*
 *	函 数 名 : GetProNameByModule
 *	函数功能 : 根据进程名获取模块号
 *	时    间 : 2010年9月23日
 *	返 回 值 : bool
 *	参数说明 : 
*/
bool ThreeLogGroup::GetProNameByModule(int iParam, char * sReturn)
{
    char sSql[1024]={0};
    int iModId=0;

    memset(sReturn,0,sizeof(sReturn));
    DEFINE_QUERY(qry);
    try
    {
    sprintf(sSql,"select a.exec_name from wf_application a where "
        "a.module_id = %d",iParam);
    qry.setSQL(sSql);
    qry.open ();
    if(qry.next())
        strcpy(sReturn,qry.field(0).asString());
    qry.close();

    if(strlen(sReturn)==0)
        return false;
    }
    catch(TOCIException & e)
    {
        Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
        return false;
    }
    return true;
}

/*
 *	函 数 名:   GetField
 *	函数功能:   取数据中指定字段数据
 *	时    间:   2011年6月28日
 *	返 回 值:   bool
 *	参数说明:   iSeq-字段序号，sBuf-数据区，sParam返回查询的字段
*/
bool ThreeLogGroup::GetField(int iSeq,char *sBuf,char *sParam)
{
    if(!sBuf)
        return false;
    char *p = strchr(sBuf,'|');
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
 *	函 数 名:   GetInfoFromMem
 *	函数功能:   从日志组提取数据
 *	时    间:   2011年3月15日
 *	返 回 值:   bool
 *	参数说明:  iLogTypeId-分类标识，iLogClassId-类别标识，iLogDisFlag-展现分类，
 *	           sParam-检索条件，v_list-返回存储数据
*/
bool ThreeLogGroup::GetInfoFromMem(vector<string> &v_list,int iLogTypeId,
	           int iLogDisFlag,int iLogClassId,char *sParam)
{
    unsigned int m=0;
    int cnt=0;
    char sBuf[1000];

    int iCurGroupId = GetUseGroup();
    if(iCurGroupId<=0)
    {
        printf("获取当前的日志组标识符失败，请检查\n");
        return false;
    }
    bindGroupData(iCurGroupId);
    GetCurUsedInfoData(iCurGroupId);
    GetCurUsedInfoIndex(iCurGroupId);
    GetCurUsedInfoLock(iCurGroupId);

    LogInfoData * pInfo = m_poInfoGroup;
    if(!pInfo)
    {
        printf("定位当前日志组数据区失败,请检查\n");
        return false;
    }
    if(!m_poInfoIndex)
    {
        printf("定位当前日志组索引区失败,请检查\n");
        return false;        
    }
    unsigned int lPrm=0;
    
    if(iLogDisFlag==GROUPCLASS)
    {//全量扫描
        for(int i=1;i<=m_lInfoDataSize;i++)
        {
            if(strlen(pInfo[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
            if(CheckParam(iLogTypeId,iLogDisFlag,iLogClassId,sParam,&pInfo[i]))
            {
                memset(sBuf,0,sizeof(sBuf));
                sprintf(sBuf,"%s|%d|%s|%d|%d|%d|%d|%d|%d|%d|%s|%s|%s",
                            pInfo[i].m_sLogTime,pInfo[i].m_illCode,pInfo[i].m_sAreaCode,
                            pInfo[i].m_illProId,pInfo[i].m_illAppId,
                            pInfo[i].m_illLevelId,pInfo[i].m_illClassId,pInfo[i].m_illTypeId,
                            pInfo[i].m_illEmployee,pInfo[i].m_illActionId,
                            pInfo[i].m_sActionName,pInfo[i].m_sCodeName,pInfo[i].m_sInfo);
                v_list.push_back(sBuf);
                cnt++;
            }
        }
        return true;
    }
    if(m_poInfoIndex->get(iLogTypeId,&m))
    {
        while(m)
        {
            if(lPrm==m)
                break;
            if(cnt>=DISPLAYMAXCNT)
                break;
            if(CheckParam(iLogTypeId,iLogDisFlag,iLogClassId,sParam,&pInfo[m]))
            {
                memset(sBuf,0,sizeof(sBuf));
                sprintf(sBuf,"%s|%d|%s|%d|%d|%d|%d|%d|%d|%d|%s|%s|%s",
                            pInfo[m].m_sLogTime,pInfo[m].m_illCode,pInfo[m].m_sAreaCode,
                            pInfo[m].m_illProId,pInfo[m].m_illAppId,
                            pInfo[m].m_illLevelId,pInfo[m].m_illClassId,pInfo[m].m_illTypeId,
                            pInfo[m].m_illEmployee,pInfo[m].m_illActionId,
                            pInfo[m].m_sActionName,pInfo[m].m_sCodeName,pInfo[m].m_sInfo);
                v_list.push_back(sBuf);
                cnt++;
            }
            lPrm = m;
            m = pInfo[m].m_iNextOffset;
        }
    }
    return true;
}



/*
 *	函 数 名 : SetLogGroupPigeonhole
 *	函数功能 : 设置日志组归档标识
 *	时    间 : 2010年6月26日
 *	返 回 值 : bool
 *	参数说明 : bParam-归档标识
*/
bool ThreeLogGroup::SetLogGroupPigeonhole(bool bParam)
{
    unsigned int i;

    for(int num=1;num<=m_iMaxGroupNum;++num)
    {
        if(!m_poLogGroupIndex->get(num,&i))
        {
            THROW(MBC_Log_Group+4);
        }
        ThreeLogGoupData * p  = m_poLogGroup + i;
        if(!p)
        {
            THROW(MBC_Log_Group+4);
        }
        m_poDataLock->P();
        p->m_bPigeonhole = bParam;
        m_poDataLock->V();
    }
    return true;
}

/*
 *	函 数 名 : SetLogLevel
 *	函数功能 : 设置日志组日志等级
 *	时    间 : 2011年7月24日
 *	返 回 值 : bool
 *	参数说明 : level日志等级
*/
bool ThreeLogGroup::SetLogLevel(int level)
{
    unsigned int i;

    for(int num=1;num<=m_iMaxGroupNum;++num)
    {
        if(!m_poLogGroupIndex->get(num,&i))
        {
            THROW(MBC_Log_Group+4);
        }
        ThreeLogGoupData * p  = m_poLogGroup + i;
        if(!p)
        {
            THROW(MBC_Log_Group+4);
        }
        m_poDataLock->P();
        p->m_iLevel = level;
        m_poDataLock->V();
    }
    return true;
}

/*
 *	函 数 名 : SetFileCheckPercent
 *	函数功能 : 设置文件大小百分比，达到此值时，切换日志组
 *	时    间 : 2011年9月20日
 *	返 回 值 : bool
 *	参数说明 : 文件大小切换百分比
*/
bool  ThreeLogGroup::SetFileCheckPercent(int iValue)
{
	if(iValue <= 0 || iValue > 100)
	{
		iValue = 90;
	}
	
    unsigned int i;

    for(int num=1;num<=m_iMaxGroupNum;++num)
    {
        if(!m_poLogGroupIndex->get(num,&i))
        {
            THROW(MBC_Log_Group+4);
        }
        ThreeLogGoupData * p  = m_poLogGroup + i;
        if(!p)
        {
            THROW(MBC_Log_Group+4);
        }
		m_poDataLock->P();
        p->m_iFileCheckPercent = iValue;
		m_poDataLock->V();
    }
    return true;
}

/*
 *	函 数 名 : SetCheckPointTouch
 *	函数功能 : 设置日志组checkpoint百分比
 *	时    间 : 2011年7月18日
 *	返 回 值 : bool
 *	参数说明 : 
*/
bool ThreeLogGroup::SetCheckPointTouch(int num)
{
    m_iCheckPointTouch = num;
    return true;
}

/*
 *	函 数 名 : SetGroupMaxFileSize
 *	函数功能 : 设置日志组文件大小限制
 *	时    间 : 2010年7月23日
 *	返 回 值 : bool
 *	参数说明 : 
*/
bool ThreeLogGroup::SetGroupMaxFileSize(int size)
{
    if(size<0)
        return false;
    if(size<=FILEMIN)
    {
        Log::log(0,"日志文件大小设置过小,可能会导致频繁切换,请重新设置.建议范围:200~300M");
        size = 200;
    }
    if(size>=FILEMAX)
    {
        Log::log(0,"日志文件大小设置过大,请重新设置.建议范围:200~300M");
        size = 200;
    }
    m_lMaxLogFileSize = size*1024*1024;
	return true;
}

bool ThreeLogGroup::RefreshGroupMaxFileSize(int size)
{
	if(size <= 0 || size >= 500)
	{
		size = 200;
	}
	
	unsigned int i;
	
    for(int num=1;num<=m_iMaxGroupNum;++num)
    {
        if(!m_poLogGroupIndex->get(num,&i))
        {
            THROW(MBC_Log_Group+4);
        }
        ThreeLogGoupData * p  = m_poLogGroup + i;
        if(!p)
        {
            THROW(MBC_Log_Group+4);
        }
		m_poDataLock->P();
        p->m_lFileSizeThreshold = size*1024*1024;
		m_poDataLock->V();
    }

	return true;
}
/*
 *	函 数 名 : SetGroupMaxSize
 *	函数功能 : 设置日志组大小限制
 *	时    间 : 2010年7月23日
 *	返 回 值 : bool
 *	参数说明 : 
*/
bool ThreeLogGroup::SetGroupMaxSize(int size)
{
    if(size<0)
        return false;
    m_lMaxLogDirSize = size*1024*1024;
    return true;
}

/*
 *	函 数 名 : SetGroupDefaultPath
 *	函数功能 : 设置日志组成员文件的默认存储目录
 *	时    间 : 2010年8月3日
 *	返 回 值 : bool
 *	参数说明 : 
*/
bool ThreeLogGroup::SetGroupDefaultPath(char *path)
{
    if(strlen(path)==0)
        return false;
    strcpy(m_sDefaultLogGroupPath,path);
    CheckLogDir(m_sDefaultLogGroupPath);
    return true;
}

/*
 *	函 数 名 : SetGroupDefaultPath
 *	函数功能 : 设置日志组成员文件存储目录
 *	时    间 : 2010年8月3日
 *	返 回 值 : void
 *	参数说明 : 
*/
void ThreeLogGroup::SetGroupFilePath(int GroupId,int FileSeq,char *path)
{
    char temp[10]={0};

    sprintf(temp,"%d_%d",GroupId,FileSeq);
    string sFileId(temp);
    m_mFilePathMap.insert(map<string,string>::value_type(sFileId,path));
}

/*
 *	函 数 名 : SetGroupBakPath
 *	函数功能 : 设置日志组归档后的备份目录
 *	时    间 : 2010年7月23日
 *	返 回 值 : bool
 *	参数说明 : 
*/
bool ThreeLogGroup::SetGroupBakPath(char *path)
{
    if(strlen(path)==0)
        return false;
    strcpy(m_sBakLogGroupPath,path);
    CheckLogDir(m_sBakLogGroupPath);
    return true;
}

/*
 *	函 数 名 : SetGroupWriteMode
 *	函数功能 : 设置日志组写数据方式
 *	时    间 : 2010年9月7日
 *	返 回 值 : bool
 *	参数说明 : 
*/
bool ThreeLogGroup::SetGroupWriteMode(int mode)
{
    m_iWriteMode = mode;
    return true;
}

/*
 *	函 数 名 : FileDownInfo
 *	函数功能 : 落地内存信息到文件
 *	时    间 : 2010年9月16日
 *	返 回 值 : bool
 *	参数说明 : 
*/
bool ThreeLogGroup::FileDownInfo(int GroupId,bool bPrintFlag)
{
    DIR *dirp=NULL;
    char tempPath[500],sInfo[1024];
    int fd;
    long lCnt=0,MixCnt=0;
    unsigned int i;
    ssize_t  sRet;

    if(m_iWriteMode!=WRITESHARE)
        return true;
/*        
    if(m_bAttached==false)
        THROW(MBC_Log_Group+4);
*/
    if(GroupId > GROUP_LOG_NUMS || GroupId <1)
        THROW(MBC_Log_Group+11);
    if(!m_poLogGroupIndex->get(GroupId, &i))
        THROW(MBC_Log_Group+4);
    ThreeLogGoupData * p = m_poLogGroup + i;
    if(!p)
        THROW(MBC_Log_Group+4);
	
	m_poLogGroupMgrLock->P();
		
	//检查日志组下面的文件是否全部存在
	bool bLogGroupExist = CheckLogFile(GroupId);
	
		int iCnt = m_poInfoDataAll[GroupId]->getCount();
		
    if(iCnt<=p->m_iWritSeq && bLogGroupExist)
    {
    	m_poLogGroupMgrLock->V();
        return true;
    }

	LogInfoData *pInfo = 0;
    pInfo = (LogInfoData *)(*(m_poInfoDataAll[GroupId]));
    if(!pInfo) {
        return false;
		m_poLogGroupMgrLock->V();
    }

	//add by liyr    
	m_poDataLock->P();
	long iMemSeq = p->m_iMemSeq;
	m_poDataLock->V();
	bool bFlag = true;
    for(int j=0;j!=GROUP_FILE_CNT;j++)
    {
        if(strlen(p->m_File[j].m_sFileName)==0)
            continue;
        dirp = opendir(p->m_File[j].m_sGroupPath);
        if(dirp==NULL)
        {
            mkdir(p->m_File[j].m_sGroupPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
        }
        else
            closedir(dirp);
        memset(tempPath,0,sizeof(tempPath));
        sprintf(tempPath,"%s%s",p->m_File[j].m_sGroupPath,p->m_File[j].m_sFileName);



		bool bFileExist = true;
		//如果日志文件不存在，则要将现有内存数据写入日志文件
		if(access(tempPath,F_OK) == -1) 
		{
			bFileExist = false;
		}
		
		if((fd=open (tempPath, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
        {        
            Log::log(0,"打开日志组成员文件失败");
            sprintf(tempPath,"%s.temp",tempPath);
            if((fd=open (tempPath, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
            {
                Log::log(0,"写日志组成员文件失败");
				m_poLogGroupMgrLock->V();
                return false;
            }
        }
        lCnt=0;
        MixCnt=0;
        //for(int i=1;i<=m_lInfoDataSize;i++)


		int i;
		if(!bFileExist) {
			i = 1;
		} else {
			i = p->m_iWritSeq+1;
		}
		for(;i<=iMemSeq;i++)
        //for(int i=p->m_iWritSeq;i<=p->m_iMemSeq;i++)
        //for(int i=p->m_iWritSeq;i<=iCnt;i++)
        {
            if(strlen(pInfo[i].m_sInfo)==0)
                continue;
        //    lCnt++;
        //    if(lCnt<=p->m_iWritSeq)
        //        continue;
           // strncpy(sInfo,pInfo[i].m_sInfo,LOG_DATA_SIZE-1);
           /*snprintf(sInfo,LOG_DATA_SIZE-1,
                   "[%d][%s][%d][%d][%s]",pInfo[i].m_illCode,pInfo[i].m_sLogTime,
                   						pInfo[i].m_illProId,pInfo[i].m_illAppId,pInfo[i].m_sInfo);
                   						*/
		
            combineLog(pInfo+i, sInfo,2);
            if(sInfo[strlen(sInfo)-1]!='\n')
                sprintf(sInfo,"%s\n",sInfo);
            sRet = write (fd, sInfo, strlen (sInfo));
            if(sRet<0)
            {
                sprintf(tempPath,"%s.temp",tempPath);
                if((fd=open (tempPath, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
                {
                    Log::log(0,"写日志组成员文件失败");
					m_poLogGroupMgrLock->V();
                    return false;
                }
                sRet = write (fd, sInfo, strlen (sInfo));
                if(sRet<0)
                {
                    close (fd);
					m_poLogGroupMgrLock->V();
                    return false;
                }
            }


			//落地一次，只需要更新一次大小信息
			if(bFlag) {

				//处理对于文件删除的情况
				if(i <= p->m_iWritSeq) 
				{
					continue;
				}
				
           		p->m_lCurFileSize += strlen(sInfo)*sizeof(char );
           		//p->m_lCurDirSize += strlen(sInfo) * sizeof(char ) * p->m_iSeq;
           		p->m_lCurDirSize += strlen(sInfo) * sizeof(char );
			}
            //p->m_lCurFileSize+=strlen(sInfo)*sizeof(char );
        }
        close (fd);
        //MixCnt = lCnt-p->m_iWritSeq;
        //MixCnt=p->m_iMemSeq-p->m_iWritSeq;
        MixCnt=iMemSeq-p->m_iWritSeq;
		bFlag = false;
        if(MixCnt>0)
        {
            m_poDataLock->P();
            p->m_File[j].m_lFileSize = p->m_lCurFileSize;
            m_poDataLock->V();
        }
    }
    if(MixCnt>0)
    {
        m_poDataLock->P();
        p->m_iWritSeq+=MixCnt;
        m_poDataLock->V();
        //Log::log(0,"日志组清仓进程写文件[GroupId:%d]: %ld 条\n",GroupId,MixCnt);
		Date dt;
		if( bPrintFlag)
		{
			printf("[%s]: 日志组落地写入文件[GroupId:%d]: %ld 条\n",dt.toString("yyyy-mm-dd hh:mi:ss"),GroupId,MixCnt);
		}
	}

	m_poLogGroupMgrLock->V();
	
    return true;
}


/*
 *	函 数 名 : CallBackInfo
 *	函数功能 : 回收共享内存数据和索引
 *	时    间 : 2010年9月16日
 *	返 回 值 : bool
 *	参数说明 : 
*/
bool ThreeLogGroup::CallBackInfo(int GroupId)
{
    unsigned int iOffset=0;
    unsigned int i;
    int iMaxCn=0;
  
    if(m_iWriteMode!=WRITESHARE)
        return true;
/*
    if(m_bInfoAttached==false)
        return false;
*/
    if(GroupId > GROUP_LOG_NUMS || GroupId <1)
        THROW(MBC_Log_Group+11);
    if(!m_poLogGroupIndex->get(GroupId, &i))
        THROW(MBC_Log_Group+4);
    ThreeLogGoupData * p = m_poLogGroup + i;  
    if(!p)  return false ;          
    	
    LogInfoData *pInfo = 0;
    pInfo = (LogInfoData *)(*(m_poInfoDataAll[GroupId]));

    if(!pInfo)
        return false;
        
    //ThreeLogGoupData * p = m_poLogGroup + i ;
      
    //数据区
    m_poInfoDataLock->P();
    //long cnt=m_lInfoDataSize;
    long cnt = m_poInfoDataAll[GroupId]->getTotal();
   // int cnt=m_poInfoDataAll[GroupId]->getCount() ;
   // for(int i=1;i<=m_lInfoDataSize;i++)
   // for(int i=1;i<=p->m_iMemSeq;i++)
    for(long i=1;i<=cnt;i++)
    {
        if(strlen(pInfo[i].m_sInfo)==0)
            continue;
        m_poInfoDataAll[GroupId]->revoke(i);
    }
	m_poInfoDataAll[GroupId]->reset();
	
    m_poInfoDataLock->V();
    //索引区
    m_poIndexLock->P();
    SHMIntHashIndex_A::Iteration iterd = m_poInfoIndexAll[GroupId]->getIteration();
    long iIndexkey=0;
    unsigned int iIndexOffset=0;
    while(iterd.next(iIndexkey,iIndexOffset))
    {
        if(iMaxCn>=m_lInfoDataSize)
            break;            
        m_poInfoIndexAll[GroupId]->revokeIdx(iIndexkey,iIndexOffset);
        iMaxCn++;
    }
    m_poIndexLock->V();
    m_poDataLock->P();
    p->m_iWritSeq=0;
    p->m_iMemSeq=0;
    m_poDataLock->V();
    return true;
}

/*
 *	函 数 名 : CheckInfoCallBack
 *	函数功能 : 检查共享内存是否需要回收
 *	时    间 : 2010年9月16日
 *	返 回 值 : bool
 *	参数说明 : 
*/
bool ThreeLogGroup::CheckInfoCallBack()
{
    unsigned int i;
    int GroupId=0;

    if(m_iWriteMode!=WRITESHARE)
        return true;
/*
    if(m_bAttached==false || m_bInfoAttached==false)
        THROW(MBC_Log_Group+4);
*/
    GroupId = GetUseGroup();
    double iPercent = GetGroupInfoPercent(GroupId)*100;
    if(iPercent>=CALLBACKPOINT)
    {
        //落地文件
        if(!FileDownInfo(GroupId))
        {
            Log::log(0,"清仓进程落地文件失败");
            return false;
        }
        //落地完要回收
        if(!CallBackInfo(GroupId))
        {
            Log::log(0,"清仓进程回收共享内存失败");
            return false;
        } 
    }
    return true;
}

/*
 *	函 数 名 : CommitInfoFile
 *	函数功能 : 内存块数据落地文件
 *	时    间 : 2010年9月6日
 *	返 回 值 : bool
 *	参数说明 :
*/
bool ThreeLogGroup::CommitInfoFile()
{
    unsigned int i;
    ThreeLogGoupData * p = m_poLogGroup;
    //先检查是否连接共享内存
/*    
    if(m_bAttached==false)
    {
        Log::log(0,"关联日志组信息区共享内存失败，请查看是否创建");
        THROW(MBC_Log_Group+4);
    }
    if(m_bInfoAttached==false)
    {
        Log::log(0,"关联日志组数据区共享内存失败，请查看是否创建");
        THROW(MBC_Log_Group+4);        
    }
*/
    //检查下日志组文件状态
    //CheckLogFile();

    //查找清仓态的日志组
    for(int num=1;num<=m_iMaxGroupNum;++num)
    {
        if(!m_poLogGroupIndex->get(num,&i))
            return false;
       
        if(p[i].m_iState != INACTIVE)
        {
          if(!FileDownInfo(num))
          {
            Log::log(0,"清仓进程落地文件失败");
            return false;
          }
        }

        m_poLogGroupMgrLock->P();

        if(p[i].m_iState!=ACTIVE)
        {
            m_poLogGroupMgrLock->V();
            continue;  
        }
        //落地完要回收
        if(!CallBackInfo(num))
        {
            Log::log(0,"清仓进程回收共享内存失败");
            m_poLogGroupMgrLock->V();
            return false;
        }
        //回收完了更改下状态为可用态     
        m_poDataLock->P();
        p[num].m_iState = INACTIVE;
        if(p[num].m_bPigeonholeState!=NEEDDOWN)
            p[num].m_bPigeonholeState = FILEDOWN;
        p[num].m_iWritSeq = 0;
        m_poDataLock->V();
        //看是否需要归档
        if(p[num].m_bPigeonhole==true && p[num].m_bPigeonholeState==NEEDDOWN)
        {//需要归档，比如来自手动归档命令
        	refreshArchivePath();
            bakGroupFile(p[num].m_iGoupId,m_sBakLogGroupPath);
            char tempPath[500];
            for(int fnum =0;fnum!=GROUP_FILE_CNT;++fnum)
            {
                if(strlen( p[num].m_File[fnum].m_sFileName)==0)
                    continue;
                memset(tempPath,0,sizeof(tempPath));
                sprintf(tempPath,"%s%s",p[num].m_File[fnum].m_sGroupPath,p[num].m_File[fnum].m_sFileName);
                if(ClearGroupFile( p[num].m_iGoupId,tempPath)==false)
                {
                    m_poLogGroupMgrLock->V();
                    return false;
                }
            }
            if(AddLogGroupFile(p[num].m_iGoupId,NULL)==false)
            {
                m_poLogGroupMgrLock->V();
                return false;
            }
            m_poDataLock->P();
            p[num].m_iState = INACTIVE;
            p[num].m_bPigeonholeState = ALLDOWN;
            p[num].m_lCurDirSize = 0;
            p[num].m_lCurFileSize = 0;
            p[num].m_lPredictFileSize = 0;
            m_poDataLock->V();
        }
		
        m_poLogGroupMgrLock->V();
    }
    return true;
}

/*
 *	函 数 名 : Check75CheckPoint
 *	函数功能 : 逻辑日志写满75%的时候触发checkpoint
 *	时    间 : 2011年7月18日
 *	返 回 值 : bool
 *	参数说明 : 
*/
bool ThreeLogGroup::Check75CheckPoint(bool &bCheck)
{
    unsigned int i;
    int GroupId=0;

    if(m_iWriteMode!=WRITESHARE)
        return true;
    GroupId = GetUseGroup();
    if(GroupId > GROUP_LOG_NUMS || GroupId <1)
        THROW(MBC_Log_Group+11);
    if(!m_poLogGroupIndex->get(GroupId, &i))
        THROW(MBC_Log_Group+4);
    ThreeLogGoupData * p = m_poLogGroup + i;
    if(!p)
        THROW(MBC_Log_Group+4);
    if(p->m_bCheckPoint == true)
        return true;
    double iPercent = GetGroupInfoPercent(GroupId)*100;
    if(iPercent<m_iCheckPointTouch)
        return true;
    //逻辑日志75%触发checkpoint
    bCheck=true;
    m_poDataLock->P();
    p->m_bCheckPoint = true;
    m_poDataLock->V();
	m_poLogGroupMgrLock->P();
	int iGroupId = GetUseGroup();
	char stemp[200]={0};
    if(ChangeLogGroup(false,-1)==true)
    {
        int itempId = GetUseGroup();
        sprintf(stemp,"[logg] 日志组由组%d切换到组%d",iGroupId,itempId);
    }
	m_poLogGroupMgrLock->V();

	log(MBC_ACTION_LOG_CONVERT,LOG_LEVEL_INFO,LOG_TYPE_SYSTEM,"%s",stemp);

	//checkpoint接口
    
    return true;
}

/*
 *	函 数 名 : log
 *	函数功能 : 三个日志组写数据接口
 *	时    间 : 2010年6月26日
 *	返 回 值 : 
 *	参数说明 : iCodeId：日志编码 ，LogClass:日志类别，iLogTypeId:分类别，LogLevel:日志等级
 *             LogProid:日志来源PorcessId,LogAppid:日志来源AppId
 *             iAction：参数修改或者数据库sql操作时用,pExpand:扩展字段（参数名/表名）
*/
void ThreeLogGroup::log(int iCodeId,int iLogLevel,int iLogClassId,
	                      int iLogTypeId,int iAction,const char *pExpand,const char* format,...)
{	
	if(m_bAttached == false)
	{
		initBase();
		GetUseGroup();
		bindGroupData(m_iCurUseGroupId);
		GetCurUsedInfoData(m_iCurUseGroupId);
		GetCurUsedInfoIndex(m_iCurUseGroupId);
		GetCurUsedInfoLock(m_iCurUseGroupId);
	}

	if(!m_pErrorCode->getCauseInfo(iCodeId))
	{
		return ;
		//THROW(MBC_ERRORCODE+1);
	}

    char pData[LOG_DATA_SIZE]={0};
    va_list ap;
    va_start(ap, format);
    vsnprintf (pData, LOG_DATA_SIZE-1, format, ap);
    va_end(ap);
    
    
    struct dirent *dp=NULL;
    struct stat statbuf;
    DIR *dirp=NULL;
    long tempSize=0,MaxFileSize=0,MaxGroupSize=0;
    int fd;
    char tempPath[500];
    unsigned int i = 0,j=0;
	int icLastId=0;

    if(m_iWriteMode==-1)
    {
        return;
    }

    ThreeLogGoupData* p;

    m_poLogGroupIndex->get(m_iCurUseGroupId,&i);
    p = m_poLogGroup + i;
    if(!p)
    {
        LoggLog(0,"日志组获取共享内存信息失败\n");
        LoggLog(1,pData);
        return;
    }

    if(p->m_iLevel < iLogLevel)
    	return ;

	//如果日志组已经切换，需要重定位数据区    	
    if(p->m_iState!=CURRENT)
    {
        if(GetUseGroup()<=0)
        {
            LoggLog(0,"日志组获取当前使用组失败\n");
            LoggLog(1,pData);
            return ;
        }
        m_poLogGroupIndex->get(m_iCurUseGroupId,&i);
        p = m_poLogGroup + i;
        if(!p)
        {
            LoggLog(0,"日志组获取共享内存信息失败\n");
            LoggLog(1,pData);
            return;
        }
        bindGroupData(m_iCurUseGroupId);
        GetCurUsedInfoData(m_iCurUseGroupId);
        GetCurUsedInfoIndex(m_iCurUseGroupId);
        GetCurUsedInfoLock(m_iCurUseGroupId);
    }

    if(p->m_lCurDirSize > m_lMaxLogDirSize || p->m_lCurFileSize > p->m_lFileSizeThreshold * p->m_iFileCheckPercent/100.0
       || p->m_iMemSeq >= m_lInfoDataSize-1 || p->m_lPredictFileSize > p->m_lFileSizeThreshold * p->m_iFileCheckPercent/100.0) 
    {
    	//超出日志组的最大资源数，切换日志组
	    //防止多进程同时切换，所以上锁，切换前还要判断一下，满足条件就不要切换了
	    //在里面加锁判断是为了减少竞态条件，和log接口一样
		bool bChange = false;
		m_poLogGroupMgrLock->P(); 
		//上锁之后，重新定位，以防止已经有进程切换了
		if(GetUseGroup()<=0)
		{
            LoggLog(0,"日志组获取当前使用组失败\n");
            LoggLog(1,pData);
			m_poLogGroupMgrLock->V();
            return ;		
		}
		
        m_poLogGroupIndex->get(m_iCurUseGroupId,&i);
        p = m_poLogGroup + i;
        if(!p)
        {
            LoggLog(0,"日志组获取共享内存信息失败\n");
            LoggLog(1,pData);
			m_poLogGroupMgrLock->V();
            return;
        }
        bindGroupData(m_iCurUseGroupId);
        GetCurUsedInfoData(m_iCurUseGroupId);
        GetCurUsedInfoIndex(m_iCurUseGroupId);
        GetCurUsedInfoLock(m_iCurUseGroupId);

		//如果还是和以前一样，就切换
		if(p->m_lCurDirSize > m_lMaxLogDirSize || p->m_lCurFileSize > p->m_lFileSizeThreshold * p->m_iFileCheckPercent/100.0
      		 || p->m_iMemSeq >= m_lInfoDataSize-1 || p->m_lPredictFileSize > p->m_lFileSizeThreshold * p->m_iFileCheckPercent/100.0)
		{
      		 
	        icLastId = m_iCurUseGroupId;
	        if(ChangeLogGroup(false)==false)
	        {
	            LoggLog(0,"切换日志组失败\n");
	            LoggLog(1,pData);
				m_poLogGroupMgrLock->V(); 
	            return;
	        } 
			
			bChange = true;			
	        //m_poLogGroupIndex->get(p->m_iNext,&i);
	        m_poLogGroupIndex->get(m_iCurUseGroupId,&i);
    		p = m_poLogGroup + i;
			
		}
		m_poLogGroupMgrLock->V(); 

		if(bChange) 
		{
			log(MBC_THREELOG_CHANGE,LOG_LEVEL_INFO,LOG_TYPE_SYSTEM,"[logg]日志组由%d切换到%d",icLastId,m_iCurUseGroupId);
		}
    }

	//写共享内存模式
    if(m_iWriteMode==WRITESHARE)
    {
       if(AddInfoDataInfo(iCodeId,m_iLogProid,-1,m_iLogAppid,iLogLevel,iLogClassId,
	   					  iLogTypeId,iAction,-1,"NULL",pExpand,pData,p)==false)
					   LoggLog(1,pData);

        return ;
    }


    //后面为直接写文件模式
    if(p->m_iSeq==0)
    {//如果当前日志组文件数为0，则默认创建两个
        AddLogGroupFile(p->m_iGoupId,m_sDefaultLogGroupPath,true);
        AddLogGroupFile(p->m_iGoupId,m_sDefaultLogGroupPath,true);
        GetGroupFilePath(p->m_iGoupId);
    }
    for(int j=0;j!=GROUP_FILE_CNT;j++)
    {
        if(strlen(p->m_File[j].m_sFileName)==0)
            continue;

        dirp = opendir(p->m_File[j].m_sGroupPath);
        if(dirp==NULL)
        {
            mkdir(p->m_File[j].m_sGroupPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
        }
        else
            closedir(dirp);
        memset(tempPath,0,sizeof(tempPath));
        sprintf(tempPath,"%s%s",p->m_File[j].m_sGroupPath,p->m_File[j].m_sFileName);
        if((fd=open (tempPath, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
        {
            LoggLog(0,"打开日志组成员文件失败\n");
            return;
        }
        char sInfo[LOG_DATA_SIZE]={0};
        m_dDT.getCurDate();
        snprintf(sInfo,LOG_DATA_SIZE-1,
                 "[%d][%s][%s]",iCodeId, m_dDT.toString(),pData);
        write (fd, sInfo, strlen (pData));
        close (fd);
        if(lstat(tempPath,&statbuf)<0)
        {
            LoggLog(0,"日志组成员文件状态出错\n");
            return;
        }
        else
        {
            m_poDataLock->P();
            tempSize = statbuf.st_size - p->m_File[j].m_lFileSize;
            p->m_File[j].m_lFileSize = statbuf.st_size;
            p->m_lCurFileSize = statbuf.st_size;
            p->m_lCurDirSize += tempSize;
            m_poDataLock->V();
        }
    }
    return;
}
void ThreeLogGroup::log(int iCodeId,int iLogLevel, int iLogTypeId,const char* format,...)
{	     
	  int iAction=-1;
    char pExpand[GROUP_NAME_LENGTH]={0};
    int iLogClassId =getClassID(iLogLevel);
    char pData[LOG_DATA_SIZE]={0};
    va_list ap;
    va_start(ap, format);
    vsnprintf (pData, LOG_DATA_SIZE-1, format, ap);
    va_end(ap);    
    log(iCodeId,iLogLevel,iLogClassId,iLogTypeId,iAction,pExpand,pData);
}  

//add by liyr
void ThreeLogGroup::Product_log(int iCodeId,int iProcId,int iAppId,int iLogLevel,int iLogTypeId,
				int iNetId,const char *sFlow,const char *format,...)
{
	int iThreadId=0;
	if(m_bAttached == false)
	{
		initBase();
		GetUseGroup();
		bindGroupData(m_iCurUseGroupId);
		GetCurUsedInfoData(m_iCurUseGroupId);
		GetCurUsedInfoIndex(m_iCurUseGroupId);
		GetCurUsedInfoLock(m_iCurUseGroupId);
	}
	
	if(!m_pErrorCode->getCauseInfo(iCodeId))
	{
		THROW(MBC_ERRORCODE+1);
	}
    char pData[LOG_DATA_SIZE]={0};
    va_list ap;
    va_start(ap, format);
    vsnprintf (pData, LOG_DATA_SIZE-1, format, ap);
    va_end(ap);
    
    
    struct dirent *dp=NULL;
    struct stat statbuf;
    DIR *dirp=NULL;
    long tempSize=0,MaxFileSize=0,MaxGroupSize=0;
    int fd;
    char tempPath[500];
    unsigned int i = 0,j=0;
    int icLastId=0;

    if(m_iWriteMode==-1)
    {
        return;
    }

    ThreeLogGoupData* p;

    m_poLogGroupIndex->get(m_iCurUseGroupId,&i);
    p = m_poLogGroup + i;
    if(!p)
    {
        LoggLog(0,"日志组获取共享内存信息失败\n");
        LoggLog(1,pData);
        return;
    }
    
    if(p->m_iLevel < iLogLevel)
    	return ;

	//如果日志组已经切换，需要重定位数据区    	
    if(p->m_iState!=CURRENT)
    {
        if(GetUseGroup()<=0)
        {
            LoggLog(0,"日志组获取当前使用组失败\n");
            LoggLog(1,pData);
            return ;
        }
        m_poLogGroupIndex->get(m_iCurUseGroupId,&i);
        p = m_poLogGroup + i;
        if(!p)
        {
            LoggLog(0,"日志组获取共享内存信息失败\n");
            LoggLog(1,pData);
            return;
        }
        bindGroupData(m_iCurUseGroupId);
        GetCurUsedInfoData(m_iCurUseGroupId);
        GetCurUsedInfoIndex(m_iCurUseGroupId);
        GetCurUsedInfoLock(m_iCurUseGroupId);
    }

    if(p->m_lCurDirSize > m_lMaxLogDirSize || p->m_lCurFileSize > p->m_lFileSizeThreshold * p->m_iFileCheckPercent/100.0
       || p->m_iMemSeq >= m_lInfoDataSize-1 || p->m_lPredictFileSize > p->m_lFileSizeThreshold * p->m_iFileCheckPercent/100.0) 
    {
    	//超出日志组的最大资源数，切换日志组
	    //防止多进程同时切换，所以上锁，切换前还要判断一下，满足条件就不要切换了
	    //在里面加锁判断是为了减少竞态条件，和log接口一样
		bool bChange = false;
		m_poLogGroupMgrLock->P(); 
		//上锁之后，重新定位，以防止已经有进程切换了
		if(GetUseGroup()<=0)
		{
            LoggLog(0,"日志组获取当前使用组失败\n");
            LoggLog(1,pData);
			m_poLogGroupMgrLock->V();
            return ;		
		}
		
        m_poLogGroupIndex->get(m_iCurUseGroupId,&i);
        p = m_poLogGroup + i;
        if(!p)
        {
            LoggLog(0,"日志组获取共享内存信息失败\n");
            LoggLog(1,pData);
			m_poLogGroupMgrLock->V();
            return;
        }
        bindGroupData(m_iCurUseGroupId);
        GetCurUsedInfoData(m_iCurUseGroupId);
        GetCurUsedInfoIndex(m_iCurUseGroupId);
        GetCurUsedInfoLock(m_iCurUseGroupId);

		//如果还是和以前一样，就切换
		if(p->m_lCurDirSize > m_lMaxLogDirSize || p->m_lCurFileSize > p->m_lFileSizeThreshold * p->m_iFileCheckPercent/100.0
      		 || p->m_iMemSeq >= m_lInfoDataSize-1 || p->m_lPredictFileSize > p->m_lFileSizeThreshold * p->m_iFileCheckPercent/100.0)
		{
      		 
	        icLastId = m_iCurUseGroupId;
	        if(ChangeLogGroup(false)==false)
	        {
	            LoggLog(0,"切换日志组失败\n");
	            LoggLog(1,pData);
				m_poLogGroupMgrLock->V(); 
	            return;
	        }  

			bChange = true;
	        //m_poLogGroupIndex->get(p->m_iNext,&i);
	        m_poLogGroupIndex->get(m_iCurUseGroupId,&i);
    		p = m_poLogGroup + i;
		}
		
		m_poLogGroupMgrLock->V(); 

		if(bChange)
		{
			log(MBC_THREELOG_CHANGE,LOG_LEVEL_INFO,LOG_TYPE_SYSTEM,"[logg]日志组由%d切换到%d",icLastId,m_iCurUseGroupId);
		}
    }
    
    int iLogClassId =getClassID(iLogLevel);
    int iAction=-1;
    char pExpand[GROUP_NAME_LENGTH]={0};

	//写共享内存模式
	if(m_iWriteMode==WRITESHARE)
    {
	   if(AddInfoDataInfo(iCodeId,iProcId,iThreadId,iAppId,iLogLevel,iLogClassId,
	   						iLogTypeId,iAction,iNetId,sFlow,pExpand,pData,p)==false)
					   LoggLog(1,pData);

        return ;
    }


    //后面为直接写文件模式
    if(p->m_iSeq==0)
    {//如果当前日志组文件数为0，则默认创建两个
        AddLogGroupFile(p->m_iGoupId,m_sDefaultLogGroupPath,true);
        AddLogGroupFile(p->m_iGoupId,m_sDefaultLogGroupPath,true);
        GetGroupFilePath(p->m_iGoupId);
    }
    for(int j=0;j!=GROUP_FILE_CNT;j++)
    {
        if(strlen(p->m_File[j].m_sFileName)==0)
            continue;

        dirp = opendir(p->m_File[j].m_sGroupPath);
        if(dirp==NULL)
        {
            mkdir(p->m_File[j].m_sGroupPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
        }
        else
            closedir(dirp);
        memset(tempPath,0,sizeof(tempPath));
        sprintf(tempPath,"%s%s",p->m_File[j].m_sGroupPath,p->m_File[j].m_sFileName);
        if((fd=open (tempPath, O_RDWR|O_CREAT|O_APPEND, 0770)) < 0)
        {
            LoggLog(0,"打开日志组成员文件失败\n");
            return;
        }
        write (fd, pData, strlen (pData));
        close (fd);
        if(lstat(tempPath,&statbuf)<0)
        {
            LoggLog(0,"日志组成员文件状态出错\n");
            return;
        }
        else
        {
            m_poDataLock->P();
            tempSize = statbuf.st_size - p->m_File[j].m_lFileSize;
            p->m_File[j].m_lFileSize = statbuf.st_size;
            p->m_lCurFileSize = statbuf.st_size;
            p->m_lCurDirSize += tempSize;
            m_poDataLock->V();
        }
    }
    return;

}


/*
 *	函 数 名:   GetInfoFromMem
 *	函数功能:   从日志组提取数据
 *	时    间:   2011年7月7日
 *	返 回 值:   bool
 *	参数说明:  iLogFlag-展现分类，iLogCond 条件
 *	           sParam-检索条件，v_list-返回存储数据
*/

bool ThreeLogGroup::GetInfoFromMem(vector<string> &v_list,int iLogFlag,int iLogCond, char *sParam,char *sBeginTime, char *sEndTime)
{
    unsigned int m=0;
    int cnt=0;
    char sBuf[1024];

	if(m_bAttached == false)
	{
		initBase();
	}

    int iCurGroupId = GetUseGroup();

    if(iCurGroupId<=0)
    {
        printf("获取当前的日志组标识符失败，请检查\n");
        return false;
    }
    bindGroupData(iCurGroupId);
    GetCurUsedInfoData(iCurGroupId);
    GetCurUsedInfoIndex(iCurGroupId);
    GetCurUsedInfoLock(iCurGroupId);
    LogInfoData * p = m_poInfoGroup;
    if(!p)
    {
        printf("定位当前日志组数据区失败,请检查\n");
        return false;
    }
    if(!m_poInfoIndex)
    {
        printf("定位当前日志组索引区失败,请检查\n");
        return false;        
    }
   ThreeLogGoupData * pGroup = m_poLogGroup + iCurGroupId;  
   //int iCount=pGroup->m_iMemSeq;
   //int iCount=m_poInfoDataAll[iCurGroupId]->getCount() ; 
   //int iCount=m_lInfoDataSize;
   int iCount = m_poInfoDataAll[iCurGroupId]->getTotal(); 
   unsigned int lPrm=0;
    
    if(iLogFlag==GROUPPROCESS)
    {
    		int iCondition = 0 ;
    	  if(strlen(sParam) == 0) {iCondition=-1;}
    		else                  {iCondition=atoi(sParam);}
    			
        for(int i=iCount;i>=1;i--)
        {
            if(strlen(p[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
            if(p[i].m_illClassId!=iLogCond)
            	  continue ;
        	  if( ( p[i].m_illProId > 0 && iCondition==-1 ) ||
        	  	  ( p[i].m_illProId ==iCondition && iCondition>=0 ) )
             {
             	if(sBeginTime) {
             		if(sBeginTime[0] && strcmp(p[i].m_sLogTime,sBeginTime) < 0) continue;
             	}
				if(sEndTime) {
					if(sEndTime[0] && strcmp(p[i].m_sLogTime,sEndTime) > 0) continue;
				}
				
                memset(sBuf,0,sizeof(sBuf));
                /*sprintf(sBuf,"%s|%d|%s|%d|%d|%d|%d|%d|%d|%d|%s|%s|%s",
                            p[i].m_sLogTime,p[i].m_illCode,p[i].m_sCodeName,
                            p[i].m_illProId,p[i].m_illAppId,
                            p[i].m_illLevelId,p[i].m_illClassId,p[i].m_illTypeId,
                            p[i].m_illEmployee,p[i].m_illActionId,
                            p[i].m_sActionName,p[i].m_sAreaCode,p[i].m_sInfo); */
                //日期     
                combineLog(p+i,sBuf);
                v_list.push_back(sBuf);
                cnt++;
             }
        }
        return true;    		
    }

    if(iLogFlag==GROUPMODULE)
    {
    		int iCondition = 0 ;
    	  if(strlen(sParam)==0) iCondition=-1;
    		else                  iCondition=atoi(sParam);
    			
         for(int i=iCount;i>=1;i--)
        {
            if(strlen(p[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
            if(p[i].m_illClassId!=iLogCond)
            	  continue ;
        	  if( ( p[i].m_illAppId > 0 && iCondition==-1 ) ||
        	  	  ( p[i].m_illAppId ==iCondition && iCondition>=0 ) )
             {
				 if(sBeginTime) {
					 if(sBeginTime[0] && strcmp(p[i].m_sLogTime,sBeginTime) < 0) continue;
				 }
				 if(sEndTime) {
					 if(sEndTime[0] && strcmp(p[i].m_sLogTime,sEndTime) > 0) continue;
				 }
				  
                memset(sBuf,0,sizeof(sBuf));
                /*sprintf(sBuf,"%s|%d|%s|%d|%d|%d|%d|%d|%d|%d|%s|%s|%s",
                            p[i].m_sLogTime,p[i].m_illCode,p[i].m_sCodeName,
                            p[i].m_illProId,p[i].m_illAppId,
                            p[i].m_illLevelId,p[i].m_illClassId,p[i].m_illTypeId,
                            p[i].m_illEmployee,p[i].m_illActionId,
                            p[i].m_sActionName,p[i].m_sAreaCode,p[i].m_sInfo);
                            */
                combineLog(p+i,sBuf);
                v_list.push_back(sBuf);
                cnt++;
             }
        }
        return true;    		
    }  
    
    if(iLogFlag==GROUPCLASS)
    {    			
         for(int i=iCount;i>=1;i--)
        {
            if(strlen(p[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
            if(p[i].m_illClassId==iLogCond)
             {
				 if(sBeginTime) {
					 if(sBeginTime[0] && strcmp(p[i].m_sLogTime,sBeginTime) < 0) continue;
				 }
				 if(sEndTime) {
					 if(sEndTime[0] && strcmp(p[i].m_sLogTime,sEndTime) > 0) continue;
				 }
				 
                memset(sBuf,0,sizeof(sBuf));
                /*sprintf(sBuf,"%s|%d|%s|%d|%d|%d|%d|%d|%d|%d|%s|%s|%s",
                            p[i].m_sLogTime,p[i].m_illCode,p[i].m_sCodeName,
                            p[i].m_illProId,p[i].m_illAppId,
                            p[i].m_illLevelId,p[i].m_illClassId,p[i].m_illTypeId,
                            p[i].m_illEmployee,p[i].m_illActionId,
                            p[i].m_sActionName,p[i].m_sAreaCode,p[i].m_sInfo);*/
                combineLog(p+i,sBuf);
                v_list.push_back(sBuf);
                cnt++;
             }
        }
        return true;    		
    } 
    
    if(iLogFlag==GROUPINFOLV)
    {    			
         for(int i=iCount;i>=1;i--)
        {
            if(strlen(p[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
            if(p[i].m_illLevelId==iLogCond)
             {
				 if(sBeginTime) {
					 if(sBeginTime[0] && strcmp(p[i].m_sLogTime,sBeginTime) < 0) continue;
				 }
				 if(sEndTime) {
					 if(sEndTime[0] && strcmp(p[i].m_sLogTime,sEndTime) > 0) continue;
				 }
				 
                memset(sBuf,0,sizeof(sBuf));
                /*sprintf(sBuf,"%s|%d|%s|%d|%d|%d|%d|%d|%d|%d|%s|%s|%s",
                            p[i].m_sLogTime,p[i].m_illCode,p[i].m_sCodeName,
                            p[i].m_illProId,p[i].m_illAppId,
                            p[i].m_illLevelId,p[i].m_illClassId,p[i].m_illTypeId,
                            p[i].m_illEmployee,p[i].m_illActionId,
                            p[i].m_sActionName,p[i].m_sAreaCode,p[i].m_sInfo);
                            */
                combineLog(p+i,sBuf);
                v_list.push_back(sBuf);
                cnt++;
             }
        }
        return true;    		
    } 
    
    if(iLogFlag==GROUPTYPE)
    {    			
        for(int i=iCount;i>=1;i--)
        {
            if(strlen(p[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
            if(p[i].m_illTypeId==iLogCond)
             {
				 if(sBeginTime) {
					 if(sBeginTime[0] && strcmp(p[i].m_sLogTime,sBeginTime) < 0) continue;
				 }
				 if(sEndTime) {
					 if(sEndTime[0] && strcmp(p[i].m_sLogTime,sEndTime) > 0) continue;
				 }
				 
                memset(sBuf,0,sizeof(sBuf));
                /*sprintf(sBuf,"%s|%d|%s|%d|%d|%d|%d|%d|%d|%d|%s|%s|%s",
                            p[i].m_sLogTime,p[i].m_illCode,p[i].m_sCodeName,
                            p[i].m_illProId,p[i].m_illAppId,
                            p[i].m_illLevelId,p[i].m_illClassId,p[i].m_illTypeId,
                            p[i].m_illEmployee,p[i].m_illActionId,
                            p[i].m_sActionName,p[i].m_sAreaCode,p[i].m_sInfo);*/
                combineLog(p+i,sBuf);
                v_list.push_back(sBuf);
                cnt++;
             }
        }
        return true;    		
    }  
    
    if(iLogFlag==GROUPPARAM)
    {
      for(int i=iCount;i>=1;i--)
      {
        if(strlen(p[i].m_sInfo)==0)
            continue;
        if(cnt>=DISPLAYMAXCNT)
            break;
        if(p[i].m_illTypeId==LOG_TYPE_PARAM && 
        	(strlen(sParam)==0 || strcmp(sParam,p[i].m_sActionName)==0) )
         {
				 if(sBeginTime) {
					 if(sBeginTime[0] && strcmp(p[i].m_sLogTime,sBeginTime) < 0) continue;
				 }
				 if(sEndTime) {
					 if(sBeginTime[0] && strcmp(p[i].m_sLogTime,sEndTime) > 0) continue;
				 }
				 
            memset(sBuf,0,sizeof(sBuf));
            /*sprintf(sBuf,"%s|%d|%s|%d|%d|%d|%d|%d|%d|%d|%s|%s|%s",
                        p[i].m_sLogTime,p[i].m_illCode,p[i].m_sCodeName,
                        p[i].m_illProId,p[i].m_illAppId,
                        p[i].m_illLevelId,p[i].m_illClassId,p[i].m_illTypeId,
                        p[i].m_illEmployee,p[i].m_illActionId,
                        p[i].m_sActionName,p[i].m_sAreaCode,p[i].m_sInfo);
                      */
            combineLog(p+i,sBuf);
            v_list.push_back(sBuf);
            cnt++;
         }
      }
      return true;		
    }
    
    
    if(iLogFlag==GROUPCODE)
    {    			
        for(int i=iCount;i>=1;i--)
        {
            if(strlen(p[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
            if(p[i].m_illCode==iLogCond)
             {
 				 if(sBeginTime) {
					 if(sBeginTime[0] && strcmp(p[i].m_sLogTime,sBeginTime) < 0) continue;
				 }
				 if(sEndTime) {
					 if(sEndTime[0] && strcmp(p[i].m_sLogTime,sEndTime) > 0) continue;
				 }
				 
                memset(sBuf,0,sizeof(sBuf));
                /*sprintf(sBuf,"%s|%d|%s|%d|%d|%d|%d|%d|%d|%d|%s|%s|%s",
                            p[i].m_sLogTime,p[i].m_illCode,p[i].m_sCodeName,
                            p[i].m_illProId,p[i].m_illAppId,
                            p[i].m_illLevelId,p[i].m_illClassId,p[i].m_illTypeId,
                            p[i].m_illEmployee,p[i].m_illActionId,
                            p[i].m_sActionName,p[i].m_sAreaCode,p[i].m_sInfo);*/
				combineLog(p+i,sBuf);
                v_list.push_back(sBuf);
                cnt++;
             }
        }
        return true;    		
    }      
}

void ThreeLogGroup::combineLog(LogInfoData *p, char *sDest,int iMode)
{
	char sLevel[8] = {0};
	if(p->m_illLevelId == LOG_LEVEL_FATAL) {
		strcpy(sLevel,"fatal");
	} else if(p->m_illLevelId == LOG_LEVEL_ERROR) {
		strcpy(sLevel,"error");
	} else if(p->m_illLevelId == LOG_LEVEL_WARNING) {
		strcpy(sLevel,"warning");
	} else if(p->m_illLevelId == LOG_LEVEL_INFO) {
		strcpy(sLevel,"info");
	} else if(p->m_illLevelId == LOG_LEVEL_DEBUG) {
		strcpy(sLevel,"debug");
	}

	//日志查询使用
	if(iMode == 1) {
    	sprintf(sDest,"%s|%s|%d|%d|%s|"
					 "%d|%d|%d|"
					 "%d|%s|"
					 "%d|%d|%s|%s",
              	  	 p->m_sLogTime,sLevel,p->m_illTypeId,p->m_illCode,p->m_sCodeName,
              	  	 p->m_illProId,p->m_iThreadId,p->m_illAppId,
               	 	 p->m_iNetId,p->m_sFlow,
                  	 p->m_illEmployee,p->m_illActionId,p->m_sActionName,p->m_sInfo);
	} 
	else if(iMode == 2) 
	{
    	sprintf(sDest,"%s|%s|%d|"
					 "%d|%d|%d|"
					 "%d|%s|%d|%s",
              	  	 p->m_sLogTime,sLevel,p->m_illTypeId,
              	  	 p->m_illProId,p->m_iThreadId,p->m_illAppId,
               	 	 p->m_iNetId,p->m_sFlow,p->m_illCode,p->m_sInfo);	
	}
}

/*
 *	函 数 名:   GetLoggInfo
 *	函数功能:   从日志组提取数据
 *	时    间:   2011年7月9日
 *	返 回 值:   bool
 *	参数说明:  iLogFlag-展现分类，iLogCond 条件
 *	           sParam-检索条件，v_list-返回存储数据
*/
/*
bool ThreeLogGroup::GetLoggInfo(vector<LogInfoData> &v_list,int iLogFlag,int iLogCond, char *sParam)
{
    unsigned int m=0;
    int cnt=0;
    char sBuf[1000];

    int iCurGroupId = GetUseGroup();

    if(iCurGroupId<=0)
    {
        printf("获取当前的日志组标识符失败，请检查\n");
        return false;
    }
    bindGroupData(iCurGroupId);
    GetCurUsedInfoData(iCurGroupId);
    GetCurUsedInfoIndex(iCurGroupId);
    GetCurUsedInfoLock(iCurGroupId);
    LogInfoData * p = m_poInfoGroup;
    if(!p)
    {
        printf("定位当前日志组数据区失败,请检查\n");
        return false;
    }
    if(!m_poInfoIndex)
    {
        printf("定位当前日志组索引区失败,请检查\n");
        return false;        
    }
   ThreeLogGoupData * pGroup = m_poLogGroup + iCurGroupId;  
   int iCount=pGroup->m_iMemSeq;
     
    unsigned int lPrm=0;
    
    if(iLogFlag==GROUPPROCESS)
    {    			
        for(int i=1;i<=iCount;i++)
        {
            if(strlen(p[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
        	  if( ( p[i].m_illProId >=0 && iLogCond==-1 ) ||
        	  	  ( p[i].m_illProId ==iLogCond && iLogCond>=0 ) )
             {
                v_list.push_back(p[i]);
                cnt++;
             }
        }
        return true;    		
    }

    if(iLogFlag==GROUPMODULE)
    {

        for(int i=1;i<=iCount;i++)
        {
            if(strlen(p[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
        	  if( ( p[i].m_illAppId >=0 && iLogCond==-1 ) ||
        	  	  ( p[i].m_illAppId ==iLogCond && iLogCond>=0 ) )
             {
                v_list.push_back(p[i]);
                cnt++;
             }
        }
        return true;    		
    }  
    
    if(iLogFlag==GROUPCLASS)
    {    			
        for(int i=1;i<=iCount;i++)
        {
            if(strlen(p[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
            if(p[i].m_illClassId==iLogCond)
             {
                v_list.push_back(p[i]);
                cnt++;
             }
        }
        return true;    		
    } 
    
    if(iLogFlag==GROUPINFOLV)
    {    			
        for(int i=1;i<=iCount;i++)
        {
            if(strlen(p[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
            if(p[i].m_illLevelId==iLogCond)
             {
                v_list.push_back(p[i]);
                cnt++;
             }
        }
        return true;    		
    } 
    
    if(iLogFlag==GROUPTYPE)
    {    			
        for(int i=1;i<=iCount;i++)
        {
            if(strlen(p[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
            if(p[i].m_illTypeId==iLogCond)
             {
                v_list.push_back(p[i]);
                cnt++;
             }
        }
        return true;    		
    }  
    
    if(iLogFlag==GROUPPARAM)
    {
      for(int i=1;i<=iCount;i++)
      {
        if(strlen(p[i].m_sInfo)==0)
            continue;
        if(cnt>=DISPLAYMAXCNT)
            break;
        if(p[i].m_illTypeId==LOG_TYPE_PARAM && 
        	(strlen(sParam)==0 || strcmp(sParam,p[i].m_sActionName)==0) )
         {
             v_list.push_back(p[i]);
             cnt++;
         }
      }
        	
      return true;		

    }
    
    if(iLogFlag==GROUPCODE)
    {
      if(m_poInfoIndex->get(iLogCond,&m))
      {
        while(m)
        {
            if(lPrm==m)
                break;
            if(cnt>=DISPLAYMAXCNT)
                break;
            v_list.push_back(p[m]);
                cnt++; 
            lPrm = m;
            m = p[m].m_iNextOffset;
          }
      }    
      return true;  		
    }           
}
*/
/*
 *	函 数 名:   GetLoggInfo
 *	函数功能:   从日志组提取数据
 *	时    间:   2011年7月9日
 *	返 回 值:   bool
 *	参数说明:   iLogFlag-展现分类，iLogCond 条件
 *	            char *sBTime,char *sETime-检索时间条件，
 *              v_list-返回存储数据
*/
bool ThreeLogGroup::GetLoggInfo(vector<LogInfoData> &v_list,int iLogFlag,int iLogCond, char *sBTime,char *sETime)
{
    unsigned int m=0;
    int cnt=0;
    char sBuf[1000];

	if(m_bAttached == false)
	{
		initBase();
	}

    int iCurGroupId = GetUseGroup();

    if(iCurGroupId<=0)
    {
        printf("获取当前的日志组标识符失败，请检查\n");
        return false;
    }
    bindGroupData(iCurGroupId);
    GetCurUsedInfoData(iCurGroupId);
    GetCurUsedInfoIndex(iCurGroupId);
    GetCurUsedInfoLock(iCurGroupId);
    LogInfoData * p = m_poInfoGroup;
    if(!p)
    {
        printf("定位当前日志组数据区失败,请检查\n");
        return false;
    }
    if(!m_poInfoIndex)
    {
        printf("定位当前日志组索引区失败,请检查\n");
        return false;        
    }
    unsigned int i;
   m_poLogGroupIndex->get(iCurGroupId,&i);
   ThreeLogGoupData * pGroup = m_poLogGroup + i;  
   //int iCount=pGroup->m_iMemSeq;
   //int iCount=m_poInfoDataAll[iCurGroupId]->getCount() ; 
   //int iCount=m_lInfoDataSize;    
   int iCount = m_poInfoDataAll[iCurGroupId]->getTotal();
   unsigned int lPrm=0;
    
    if(iLogFlag==GROUPPROCESS)
    {    			
        for(int i=iCount;i>=1;i--)
        {
           if(strlen(p[i].m_sInfo)==0)
                continue;
           if(cnt>=DISPLAYMAXCNT)
                break;
        	 if( p[i].m_illProId !=iLogCond && iLogCond>=0  )
        	  	  continue ;
        	 if(strcmp(p[i].m_sLogTime,sBTime)<0 )
        	 	    continue ;
        	  if(sETime==NULL ||  strcmp(p[i].m_sLogTime,sETime )<0 )          	  	  
             {
                v_list.push_back(p[i]);
                cnt++;
             }
        }
        return true;    		
    }

    if(iLogFlag==GROUPMODULE)
    {

        for(int i=iCount;i>=1;i--)
        {
           if(strlen(p[i].m_sInfo)==0)
                continue;
           if(cnt>=DISPLAYMAXCNT)
                break;
           if( p[i].m_illAppId !=iLogCond && iLogCond>=0 )
            	  continue ;
        	 if(strcmp(p[i].m_sLogTime,sBTime)<0 )
        	 	    continue ;
        	  if(sETime==NULL ||  strcmp(p[i].m_sLogTime,sETime )<0 )        
             {
                v_list.push_back(p[i]);
                cnt++;
             }
        }
        return true;    		
    }  
    
    if(iLogFlag==GROUPCLASS)
    {    			
        for(int i=iCount;i>=1;i--)
        {
            if(strlen(p[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
            if(p[i].m_illClassId!=iLogCond)
            	 continue ;
        	 if(strcmp(p[i].m_sLogTime,sBTime)<0 )
        	 	    continue ;
        	  if(sETime==NULL ||  strcmp(p[i].m_sLogTime,sETime )<0  )                 	 
             {
                v_list.push_back(p[i]);
                cnt++;
             }
        }
        return true;    		
    } 
    
    if(iLogFlag==GROUPINFOLV)
    {    			
        for(int i=iCount;i>=1;i--)
        {
            if(strlen(p[i].m_sInfo)==0)
                continue;
            if(cnt>=DISPLAYMAXCNT)
                break;
            if(p[i].m_illLevelId!=iLogCond)
            	continue ;
        	 if(strcmp(p[i].m_sLogTime,sBTime)<0 )
        	 	    continue ;
        	  if(sETime==NULL ||  strcmp(p[i].m_sLogTime,sETime )<0  )                  	
             {
                v_list.push_back(p[i]);
                cnt++;
             }
        }
        return true;    		
    } 
    
    if(iLogFlag==GROUPTYPE)
    {    			
        for(int i=iCount;i>=1;i--)
        {
           if(strlen(p[i].m_sInfo)==0)
                continue;
           if(cnt>=DISPLAYMAXCNT)
                break;
           if(p[i].m_illTypeId!=iLogCond)
            	  continue ;
        	 if(strcmp(p[i].m_sLogTime,sBTime)<0 )
        	 	    continue ;
        	  if(sETime==NULL ||  strcmp(p[i].m_sLogTime,sETime )<0  )                  	  
             {
                v_list.push_back(p[i]);
                cnt++;
             }
        }
        return true;    		
    }  
    
    if(iLogFlag==GROUPPARAM)
    {
      for(int i=iCount;i>=1;i--)
      {
        if(strlen(p[i].m_sInfo)==0)
            continue;
        if(cnt>=DISPLAYMAXCNT)
            break;
        if(p[i].m_illTypeId!=LOG_TYPE_PARAM )
        	  continue;
        if(strcmp(p[i].m_sLogTime,sBTime)<0 )
        	 	    continue ;
        if(sETime==NULL ||  strcmp(p[i].m_sLogTime,sETime )<0  )        
         {
             v_list.push_back(p[i]);
             cnt++;
         }
      }
        	
      return true;		

    }
    
    if(iLogFlag==GROUPCODE)
    {
      if(m_poInfoIndex->get(iLogCond,&m))
      {
        while(m)
        {
            if(lPrm==m)
                break;
            if(cnt>=DISPLAYMAXCNT)
                break;
        	 if(strcmp(p[m].m_sLogTime,sBTime)<0 )
        	 	{
        	 		  m = p[m].m_iNextOffset;
        	 	    continue ;
        	 	 }
        	  if(sETime==NULL ||  strcmp(p[m].m_sLogTime,sETime )<0 )  
        	 {                
            v_list.push_back(p[m]);
                cnt++; 
            lPrm = m;
            m = p[m].m_iNextOffset;
           }
         m = p[m].m_iNextOffset;
        }
      }    
      return true;  		
    }           
}

bool ThreeLogGroup::GetAllLoggInfo(vector<LogInfoData> &v_list)
{
    unsigned int m=0;
    int cnt=0;
    char sBuf[1000];
    v_list.clear();

	if(m_bAttached == false)
	{
		initBase();
	}
	
    int iCurGroupId = GetUseGroup();
    if(iCurGroupId<=0)
    {
        printf("获取当前的日志组标识符失败，请检查\n");
        return false;
    }
    bindGroupData(iCurGroupId);
    GetCurUsedInfoData(iCurGroupId);
    GetCurUsedInfoIndex(iCurGroupId);
    GetCurUsedInfoLock(iCurGroupId);
    LogInfoData * p = m_poInfoGroup;
    if(!p)
    {
        printf("定位当前日志组数据区失败,请检查\n");
        return false;
    }
    if(!m_poInfoIndex)
    {
        printf("定位当前日志组索引区失败,请检查\n");
        return false;        
    }
   //ThreeLogGoupData * pGroup = m_poLogGroup + iCurGroupId; 
    unsigned int i;
   m_poLogGroupIndex->get(iCurGroupId,&i);
   ThreeLogGoupData * pGroup = m_poLogGroup + i;      
   //int iCount=pGroup->m_iMemSeq;	
   //int iCount=m_poInfoDataAll[iCurGroupId]->getCount() ; 
   //int iCount=m_lInfoDataSize;  
   int iCount=m_poInfoDataAll[iCurGroupId]->getTotal() ;  
    for(int i=iCount;i>=1;i--)
    {
        if(strlen(p[i].m_sInfo)==0)
            continue;
        if(cnt>=DISPLAYMAXCNT)
            break;
         v_list.push_back(p[i]);
        cnt++;

    }
    return true;   
}

int ThreeLogGroup::getClassID(int iLevelID)
{
	int iRet=1;
	switch(iLevelID) {
		case 1:
		case 2:
			iRet=1;
			break;
		case 3:
			iRet=2;
			break;	
	  case 4:		
	  case 5:
			iRet=3;
			break;	
	 default :
	 	  break ;
	 	}
	 	
	 return iRet ;	
			
}

