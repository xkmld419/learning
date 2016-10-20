#include "FileMgr.h"
#include "Date.h"
#include "Log.h"
#include "MBC_28.h"
#include <dirent.h>
#include <sys/stat.h>


char sFieldData[FIELD_NUM][FIELD_LEN];

FileMgr::FileMgr(int iMode){
    memset(m_sFileName, 0, sizeof(m_sFileName));
    memset(m_sFilePath, 0, sizeof(m_sFilePath));
    memset(m_sFileFieldInfo, 0, sizeof(m_sFileFieldInfo));

    m_sData = (char*)malloc(DATA_LEN);
    if(!m_sData)
    {
    	m_poLogGroup->log(MBC_MEM_MALLOC_FAIL,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"内存分配失败!");
    	THROW(1);
    }
    fp = 0;
    m_iTableType = 0;
    m_iFileFieldCnt = 0;
    m_iDataStructSize = 0;
    m_poLogGroup =0;
/*
		m_pCommandComm =new CommandCom();
		m_pCommandComm->InitClient();
		m_pCommandComm->GetInitParam("LOG");
		m_pCommandComm->SubscribeCmd("LOG",MSG_PARAM_CHANGE);
*/
    init(iMode);
}

FileMgr::~FileMgr(){
    if ( m_sData) {
        free(m_sData);
        m_sData = NULL;
    }
}

void FileMgr::init(int iMode){
    m_iMode = iMode;
		
		memset(m_sFilePath ,'\0',sizeof(m_sFilePath));
		char * p = getenv("BILLDIR");
		sprintf(m_sFilePath,"%s%s\0",p,"/log/check_point/");
		
    if ( 1==m_iMode ) {
        strcat(m_sFilePath, "datain/");
    } else if ( 2==m_iMode ) {
        strcat(m_sFilePath, "dataout/");
    }
		
    m_sDivChar = ',';
		
    if (!m_poLogGroup)
    {
        m_poLogGroup = new ThreeLogGroup();
				m_poLogGroup->m_iLogProid = -1;
				m_poLogGroup->m_iLogAppid = -1;
    }
    if (!m_poLogGroup) {
        Log::log(0, "创建日志组对象失败!");
        THROW(1);
    }
		
/*
    char sPath[100],sFileSize[10],sGroupSize[10],sBakPath[100];
    memset(sPath,0,sizeof(sPath));
    memset(sBakPath,0,sizeof(sBakPath));
    memset(sFileSize,0,sizeof(sFileSize));
    memset(sGroupSize,0,sizeof(sGroupSize));
    
    //注册核心参数，预约初始值
    CommandCom* pCmdCom =m_pCommandComm;
    if (! pCmdCom->InitClient()) {
        Log::log(0, "核心参数初始化失败。");
        THROW(1);
    }
    pCmdCom->GetInitParam("LOG");
    pCmdCom->SubscribeCmd("LOG",MSG_PARAM_CHANGE);
    Log::m_iAlarmLevel = pCmdCom->readIniInteger("LOG","log_alarm_level",-1);
    Log::m_iAlarmDbLevel = pCmdCom->readIniInteger("LOG","log_alarmdb_level",-1);
    pCmdCom->readIniString("LOG","log_size",sFileSize,NULL);
    if (!m_poLogGroup->SetGroupMaxFileSize(atoi(sFileSize))) {
        Log::log (0, "核心参数LOG.log_size配置错误");
        THROW(MBC_Log_Group+5);
    }
    pCmdCom->readIniString("LOG","log_file_dir_size",sGroupSize,NULL);
    if (!m_poLogGroup->SetGroupMaxSize(atoi(sGroupSize))) {
        Log::log (0, "核心参数LOG.log_file_dir_size配置错误");
        THROW(MBC_Log_Group+5);
    }
    pCmdCom->readIniString("LOG","log_path",sPath,NULL);
    if (!m_poLogGroup->SetGroupDefaultPath(sPath)) {
        Log::log (0, "核心参数LOG.log_path配置错误");
        THROW(MBC_Log_Group+5);
    }
    pCmdCom->readIniString("LOG","log_path_bak",sBakPath,NULL);
    if (!m_poLogGroup->SetGroupBakPath(sBakPath)) {
        Log::log (0, "核心参数LOG.log_path_bak配置错误");
        THROW(MBC_Log_Group+5);
*/
}

bool FileMgr::openPath(const char* sPathName)
{
    //确保目录存在
    DIR * dp;
    if ( ( dp=opendir(sPathName) ) == NULL ) {
    		m_poLogGroup->log(MBC_DIR_OPEN_FAIL,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"打开目录出错[%s]",sPathName);
        if ( EACCES == errno ) {
            Log::log(0, "权限不足或目录不存在，打开目录[ %s ]失败!", sPathName);
        } else if (ENOENT == errno ) {
            Log::log(0, "目录[ %s ]不存在，试图创建...", sPathName);
        }
        //不能打开目录,试图创建该目录
        if ( mkdir(sPathName,0770)==0 ) {
            Log::log(0, "创建目录[ %s ]成功!", sPathName);
        } else {
						Log::log(0, "创建目录[ %s ]失败!", sPathName);
            return false;
        }
    }
    if (dp) {
        closedir(dp);
        dp = NULL;
    }
    
    return true;
}

bool FileMgr::openFile(const char* sFullFileName, char* sMode){
    fp = NULL;

    if (NULL == (fp = fopen(sFullFileName, sMode)) ) {
        m_poLogGroup->log(MBC_FILE_OPEN_FAIL,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"打开文件失败[%s]",sFullFileName);
        return false;
    }

    return true;
}

//关闭文件
bool FileMgr::closeFile(){
    if (fp) {
        fclose(fp);
        fp = NULL;
    }

    return true;
}

//从文件导入内存
bool FileMgr::fileToMem(const char *sFileName, const int iTableType,bool bShow ,bool bReplace){
    if (0 == sFileName)
        return false;

    setFileName(sFileName);
    setTableType(iTableType);

    char sFullFileName[500]={0};
    char sLineTmp[DATA_LEN]={0};

    if ('/' != m_sFileName[0] ) {
        strcat(sFullFileName, m_sFilePath);
        strcat(sFullFileName, m_sFileName);
    }else {
    	for(int j=(strlen(sFileName)-1);j>0;j--)
    	{
    		if(m_sFileName[j]=='/')
    		{
    			strncpy(m_sFilePath,m_sFileName,j);
    			break;
    		}
    	}
    	strcat(sFullFileName, m_sFileName);
    }
    
		if(!openPath(m_sFilePath)) {
			return false;
		}
    Log::log(0, "导入文件的路径是：%s", m_sFilePath);
    
    if ( access(sFullFileName , R_OK|W_OK) ) {
    		m_poLogGroup->log(MBC_FILE_NOT_EXIST,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"指定文件不存在[%s]",sFullFileName);
        Log::log(0, "文件[%s]不存在,请重新指定文件名后再试!", sFullFileName);
        return false;
    }
    if ( !setColumnInfo(iTableType) ) {
        return false;
    }
    if (!openFile(sFullFileName, "r")) {
        return false;
    }

    //读取文件内字段名
    if ( getLine(sLineTmp, DATA_LEN-1, '\n') ) {
        if ( !checkColumn(sLineTmp) )
            return false;
    } else {
        Log::log(0, "读取文件内字段名信息出错，请检查文件!");
        return false;
    }
    //处理每一行
    int iTotalCnt = 0;
    int iUpdCnt = 0;
    Log::log(0, "开始导入数据...");
    while ( getLine(sLineTmp, DATA_LEN-1, '\n') ) {
        if ( 0 == sLineTmp[0] ) {
            continue;
        }
        if ( !flushToMem(sLineTmp, bShow, iUpdCnt,bReplace) ) {                //每行数据写入内存
            Log::log(0, "无法加载数据，此行为：\n%s", sLineTmp);
            return false;    
        }
        ++iTotalCnt;
    }
    closeFile();
    Log::log(0, "共导入 %d 条数据:更新 %d 条，新增 %d 条!", iTotalCnt, iUpdCnt, iTotalCnt - iUpdCnt);

    return true;
}

//读取文件行
bool FileMgr::getLine(char* sLine,const size_t iSize,const char cFlag){
    sLine[0] = 0;
    if (feof(fp)) {
        Log::log(0, "文件读取结束!");
        return false;
    }
    if ( !fgets(sLine, iSize, fp) ) {
        return false;
    }
    if (sLine[strlen(sLine)-1]== cFlag) {
        sLine[strlen(sLine)-1] = 0;
    }
    return true;
}

//字段信息
bool FileMgr::setColumnInfo(const int iTableType){

    if ( 0==iTableType ) {
        return false;
    }

    resetInfo();

    switch (iTableType) {
    case TABLE_HCODE_INFO:
        {
            HCodeInfoStruct* pBase=0;

            writeFieldBuf("HEAD_ID",FILEMGR_TRANS_INT,
                          ((char*)&(pBase->m_uiHeadID)-(char*)pBase),
                          sizeof(pBase->m_uiHeadID)); 

            writeFieldBuf("HEAD",FILEMGR_TRANS_STRING,
                          ((char*)&(pBase->m_sHead)-(char*)pBase),
                          sizeof(pBase->m_sHead)); 

            writeFieldBuf("TSP_ID",FILEMGR_TRANS_STRING,
                          ((char*)&(pBase->m_sTspID)-(char*)pBase),
                          sizeof(pBase->m_sTspID));

            writeFieldBuf("ZONE_CODE",FILEMGR_TRANS_STRING,
                          ((char*)&(pBase->m_sZoneCode)-(char*)pBase),
                          sizeof(pBase->m_sZoneCode));

            writeFieldBuf("EFF_DATE",FILEMGR_TRANS_EVTDATE,
                          ((char*)(pBase->m_sEffDate)-(char*)pBase),
                          sizeof(pBase->m_sEffDate));  

            writeFieldBuf("EXP_DATE",FILEMGR_TRANS_EVTDATE,
                          ((char*)(pBase->m_sExpDate)-(char*)pBase),
                          sizeof(pBase->m_sExpDate)); 
        }
        break;
    case TABLE_LOCAL_INFO:
        {
            HCodeInfoStruct* pBase=0;

            writeFieldBuf("HEAD_ID",FILEMGR_TRANS_INT,
                          ((char*)&(pBase->m_uiHeadID)-(char*)pBase),
                          sizeof(pBase->m_uiHeadID)); 

            writeFieldBuf("HEAD",FILEMGR_TRANS_STRING,
                          ((char*)&(pBase->m_sHead)-(char*)pBase),
                          sizeof(pBase->m_sHead)); 

            writeFieldBuf("EMULATORY_PARTNER_ID",FILEMGR_TRANS_INT,
                          ((char*)&(pBase->m_sTspID)-(char*)pBase),
                          sizeof(pBase->m_sTspID));

            writeFieldBuf("EXCHANGE_ID",FILEMGR_TRANS_INT,
                          ((char*)&(pBase->m_sZoneCode)-(char*)pBase),
                          sizeof(pBase->m_sZoneCode));

            writeFieldBuf("EFF_DATE",FILEMGR_TRANS_EVTDATE,
                          ((char*)(pBase->m_sEffDate)-(char*)pBase),
                          sizeof(pBase->m_sEffDate));  

            writeFieldBuf("EXP_DATE",FILEMGR_TRANS_EVTDATE,
                          ((char*)(pBase->m_sExpDate)-(char*)pBase),
                          sizeof(pBase->m_sExpDate)); 
        }
        break;
    case TABLE_MIN_INFO:
        {
            MinInfoStruct* pBase=0;

            writeFieldBuf("MIN_INFO_ID",FILEMGR_TRANS_INT,
                          ((char*)&(pBase->m_uiMinInfoID)-(char*)pBase),
                          sizeof(pBase->m_uiMinInfoID)); 

            writeFieldBuf("BEGIN_MIN",FILEMGR_TRANS_STRING,
                          ((char*)&(pBase->m_sBeginMin)-(char*)pBase),
                          sizeof(pBase->m_sBeginMin)); 

            writeFieldBuf("END_MIN",FILEMGR_TRANS_STRING,
                          ((char*)&(pBase->m_sEndMin)-(char*)pBase),
                          sizeof(pBase->m_sEndMin));

            writeFieldBuf("USER_FLAG",FILEMGR_TRANS_INT,
                          ((char*)&(pBase->m_iUserFlag)-(char*)pBase),
                          sizeof(pBase->m_iUserFlag));  

            writeFieldBuf("COUNTRY_ID",FILEMGR_TRANS_INT,
                          ((char*)&(pBase->m_iCountryID)-(char*)pBase),
                          sizeof(pBase->m_iCountryID)); 

            writeFieldBuf("HOME_CARRIER_CODE",FILEMGR_TRANS_STRING,
                          ((char*)&(pBase->m_sHomeCarrCode)-(char*)pBase),
                          sizeof(pBase->m_sHomeCarrCode)); 

            writeFieldBuf("ZONE_CODE",FILEMGR_TRANS_STRING,
                          ((char*)&(pBase->m_sZoneCode)-(char*)pBase),
                          sizeof(pBase->m_sZoneCode)); 

            writeFieldBuf("USER_TYPE",FILEMGR_TRANS_INT,
                          ((char*)&(pBase->m_iUserType)-(char*)pBase),
                          sizeof(pBase->m_iUserType));

            writeFieldBuf("EFF_DATE",FILEMGR_TRANS_EVTDATE,
                          ((char*)(pBase->m_sEffDate)-(char*)pBase),
                          sizeof(pBase->m_sEffDate));  

            writeFieldBuf("EXP_DATE",FILEMGR_TRANS_EVTDATE,
                          ((char*)(pBase->m_sExpDate)-(char*)pBase),
                          sizeof(pBase->m_sExpDate)); 
        }
        break;
    case TABLE_IMSI_INFO:
        {
            IMSIInfoStruct* pBase=0;

            writeFieldBuf("ISMI_ID",FILEMGR_TRANS_INT,
                          ((char*)&(pBase->m_uiIMSIID)-(char*)pBase),
                          sizeof(pBase->m_uiIMSIID)); 

            writeFieldBuf("IMSI_BEGIN",FILEMGR_TRANS_STRING,
                          ((char*)&(pBase->m_sBeginIMSI)-(char*)pBase),
                          sizeof(pBase->m_sBeginIMSI)); 

            writeFieldBuf("IMSI_END",FILEMGR_TRANS_STRING,
                          ((char*)&(pBase->m_sEndIMSI)-(char*)pBase),
                          sizeof(pBase->m_sEndIMSI));

            writeFieldBuf("IMSI_TYPE",FILEMGR_TRANS_INT,
                          ((char*)&(pBase->m_iIMSIType)-(char*)pBase),
                          sizeof(pBase->m_iIMSIType));

            writeFieldBuf("SPONSOR_CODE",FILEMGR_TRANS_STRING,
                          ((char*)(pBase->m_sSponsorCode)-(char*)pBase),
                          sizeof(pBase->m_sSponsorCode)); 

            writeFieldBuf("ZONE_CODE",FILEMGR_TRANS_STRING,
                          ((char*)(pBase->m_sZoneCode)-(char*)pBase),
                          sizeof(pBase->m_sZoneCode));  

            writeFieldBuf("EFF_DATE",FILEMGR_TRANS_EVTDATE,
                          ((char*)(pBase->m_sEffDate)-(char*)pBase),
                          sizeof(pBase->m_sEffDate));  

            writeFieldBuf("EXP_DATE",FILEMGR_TRANS_EVTDATE,
                          ((char*)(pBase->m_sExpDate)-(char*)pBase),
                          sizeof(pBase->m_sExpDate)); 
        }
        break;
    default:
        Log::log(0, "未知表类型：%d", iTableType);
        return false;
    }

    return true;
}

//文件内字段与结构体成员是否对应
bool FileMgr::checkColumn(char * sColumn){
    char *sFileField, *sMemField;
    if ( strlen(sColumn) != strlen(m_sFileFieldInfo) ) {
        Log::log(0, "文件中的数据格式与内存数据格式不一致，请检查!");
        return false;
    }
    sFileField = sColumn;
    while ( *sFileField ) {
        if (isalpha(*sFileField))
            *sFileField &= 0x20;
        sFileField++;
    }
		
    for (sFileField = sColumn , sMemField = m_sFileFieldInfo; 
        *sFileField ;sFileField++,sMemField++) {
        if ( *sFileField != *sMemField) {
            Log::log(0, "文件中的数据格式与内存数据格式不一致，请检查!");
            return false;
        }
    }
		
    return true;
}

void  FileMgr::writeFieldBuf(const char* sColumn, const char cTransType,
                             unsigned int iFieldOffset, unsigned int iFieldMaxLen){
    if (m_iFileFieldCnt!=0)
        strcat(m_sFileFieldInfo,",");

    strcat(m_sFileFieldInfo, sColumn);
    m_sFileTransType[m_iFileFieldCnt]= cTransType;
    m_iFileFieldOffSet[m_iFileFieldCnt]= iFieldOffset;
    m_iFileFieldLen[m_iFileFieldCnt] = iFieldMaxLen; 
    ++m_iFileFieldCnt;
}

void FileMgr::resetInfo(){
    m_iFileFieldCnt = 0;

    memset(m_sFileFieldInfo, 0, sizeof(m_sFileFieldInfo) );
    memset(m_sFileTransType, 0, sizeof(m_sFileTransType) );
    memset(m_iFileFieldOffSet, 0, sizeof(m_iFileFieldOffSet) );
    memset(m_iFileFieldLen, 0, sizeof(m_iFileFieldLen) );
}

//分析每行数据，写入内存
bool FileMgr::flushToMem(const char *sData, bool bShow, int &iUpdCnt,bool bReplace){
    if ( !parseString(sData) ) {
        return false;
    }

    bool bNew = true ;
    for (int i=0; i<m_iFileFieldCnt; ++i) {
        if ( !writePair( (void**)&(m_sData),
                         sFieldData[i],
                         m_sFileTransType[i],
                         m_iFileFieldOffSet[i], 
                         m_iFileFieldLen[i] )) {
            return false;
        }
    }

    switch ( m_iTableType ) {
    case TABLE_HCODE_INFO:
        {
						SHMInfoCtl::updateHCodeInfo((HCodeInfoStruct *)m_sData,bNew,bReplace);
            if (bShow)
            {
               SHMInfoCtl::showHCodeInfo(*((HCodeInfoStruct*)m_sData));
            }
        }
        break;
    case TABLE_MIN_INFO:
        {
						SHMInfoCtl::updateMinInfo((MinInfoStruct *)m_sData,bNew,bReplace);
            if (bShow)
            {
               SHMInfoCtl::showMinInfo(*((MinInfoStruct*)m_sData));
            }
        }
        break;
    case TABLE_LOCAL_INFO:
        {
						
        }
        break;
    case TABLE_IMSI_INFO:
        {
						SHMInfoCtl::updateImsiInfo((IMSIInfoStruct *)m_sData,bNew,bReplace);
            if (bShow)
            {
               SHMInfoCtl::showImsiInfo(*((IMSIInfoStruct*)m_sData));
            }
        }
        break;
    default:

        break;
    }
    if ( !bNew ) {
        iUpdCnt++;
    }

    return true;
}

//分析每行数据,填入内存结构
bool FileMgr::parseString(const char *sData){
    if ( NULL == sData)
        return false;
    int nPrePos = 0;
    int nCurrentPos=0;
    int nEndPos=0;
    int j  =0;

    //把字段字符串分割成字符串数组
    while ( (sData[nCurrentPos]!=0) && (j<m_iFileFieldCnt) ) {
        if (sData[nCurrentPos] == m_sDivChar) {
            if ( (sData[nPrePos] == m_sDivChar) && (nPrePos!=0) && (nCurrentPos == nPrePos+1) ) {
                sFieldData[j][0]=0;
            } else {
                if (nPrePos !=0) {
                    strncpy(sFieldData[j],&sData[nPrePos+1],nCurrentPos-nPrePos-1);
                    sFieldData[j][nCurrentPos-nPrePos-1]=0;
                } else {
                    strncpy(sFieldData[j],&sData[nPrePos],nCurrentPos-nPrePos);
                    sFieldData[j][nCurrentPos-nPrePos]=0;
                }
            }
            nPrePos = nCurrentPos;
            j++;
        }
        nCurrentPos++;
    }

    return true;
}

bool FileMgr::writePair(void **sDes,const char* sSrc, const char cTransType, 
                        const int iOffset, const int iFieldLen){
    if (!sDes || !sSrc)
        return false;
    void* pData = *sDes;

    switch (cTransType) {
    case FILEMGR_TRANS_NULL:
        {
            Log::log(0,"转换无法识别字段类型");
            return false;
        }
        break;
    case FILEMGR_TRANS_CHAR:
        {
            *(int*)((char*)pData+iOffset)= (int)sSrc[0];
        }
        break;
    case FILEMGR_TRANS_INT:
        {
            *(int*)((char*)pData+iOffset) = atoi(sSrc);
        }
        break;
    case FILEMGR_TRANS_LONG:
        {
            *(long*)((char*)pData+iOffset) = atol(sSrc);
        }
        break;
    case FILEMGR_TRANS_FLOAT:
        {
            *(float*)((char*)pData+iOffset) = atof(sSrc);   
        }
        break;
    case FILEMGR_TRANS_DOUBLE:
        {
            *(double*)((char*)pData+iOffset) = atof(sSrc);                   
        }
        break;
    case FILEMGR_TRANS_STRING:
    case FILEMGR_TRANS_EVTDATE:
        {
            if (iFieldLen>2) {
                strncpy((char*)((char*)pData+iOffset),sSrc,iFieldLen-1);
                *(char*)((char*)pData+iOffset+iFieldLen-1) =0;
            } else {
                strcpy((char*)((char*)pData+iOffset),sSrc);
            }
        }
        break;
    default:
        {
            Log::log(0,"转换无法识别字段类型");
            return false;
        }
    }
    return true;
}

bool FileMgr::memToFile(const char *sFileName, const int iTableType, bool bShow){
    if ( !sFileName ) {
        return false;
    }

    setFileName(sFileName);
    setTableType(iTableType);

    char sFullFileName[500]={0};
    if ('/' != m_sFileName[0] ) {
        strcat(sFullFileName, m_sFilePath);
        strcat(sFullFileName, m_sFileName);
    }else {
    	for(int j=(strlen(sFileName)-1);j>0;j--)
    	{
    		if(m_sFileName[j]=='/')
    		{
    			strncpy(m_sFilePath,m_sFileName,j);
    			break;
    		}
    	}
    	strcat(sFullFileName, m_sFileName);
    }
    
		if(!openPath(m_sFilePath)) {
			return false;
		}
    Log::log(0, "导出文件的路径是：%s", m_sFilePath);

    if ( !access(sFullFileName , R_OK|W_OK) ) {
        Log::log(0, "文件[%s]已存在,请重新指定文件名后再试!", sFullFileName);
        return false;
    }
    if ( !setColumnInfo(iTableType) ) {
        return false;
    }
    if ( !openDbFile(sFullFileName) ) {
        return false;
    }
    int iTotalCnt = 0;          //待处理数据量

    Log::log(0, "开始导出数据...");
    switch (m_iTableType) {
    	case TABLE_HCODE_INFO:
    	    {
            HCodeInfoStruct *pData = NULL;

            setDataStructSize(sizeof(HCodeInfoStruct));
            iTotalCnt = SHMInfoCtl::getCount(TABLE_HCODE_INFO);
            SHMInfoCtl::getHeadData(&pData, 1);
            addData((char*)pData, iTotalCnt);
            if (bShow) {
                for (int i=0;i<iTotalCnt;++i)
                {
                    SHMInfoCtl::showHCodeInfo(*(pData+i));
                }
            }
    	    }
    	    break;
    	case TABLE_MIN_INFO:
    	    {
            MinInfoStruct *pData = NULL;

            setDataStructSize(sizeof(MinInfoStruct));
            iTotalCnt = SHMInfoCtl::getCount(TABLE_MIN_INFO);
            SHMInfoCtl::getMinData(&pData, 1);
            addData((char*)pData, iTotalCnt);
            if (bShow) {
                for (int i=0;i<iTotalCnt;++i)
                {
                    SHMInfoCtl::showMinInfo(*(pData+i));
                }
            }
    	    }
    	    break;
    	case TABLE_LOCAL_INFO:
    	    {
						
    	    }
    	    break;
    	case TABLE_IMSI_INFO:
    	    {
            IMSIInfoStruct *pData = NULL;

            setDataStructSize(sizeof(IMSIInfoStruct));
            iTotalCnt = SHMInfoCtl::getCount(TABLE_IMSI_INFO);
            SHMInfoCtl::getImsiData(&pData, 1);
            addData((char*)pData, iTotalCnt);
            if (bShow) {
                for (int i=0;i<iTotalCnt;++i)
                {
                    SHMInfoCtl::showImsiInfo(*(pData+i));
                }
            }
    	    }
    	    break;
    default:
        {
            Log::log(0, "未知表类型：%d", iTableType);
            return false;
        }
        break;
    }

    closeFile();
    Log::log(0, "共导出 %d 条数据!", iTotalCnt);

    return true;
}

bool FileMgr::openDbFile(const char *sFullFileName){
    fp=NULL;
    /*
    若不覆盖,则打开,不写文件头 并且文件是必须先存在的
    经过几次实验，在HP机器上以wb方式打开文件的话，如果文件已经存在，则会截断他
    以追加方式打开的话，却又不能定位到文件头
    a or ab             append; open file for writing at
                        end of file, or create file or writing
    */
    if ( !openFile(sFullFileName, "ab") ) {
        /*
         w+, wb+, or w+b     truncate file to zero length or create file for update
        */
        if (fp==NULL) {         //新创建的文件
            if ( !openFile(sFullFileName, "wb+") )
                return false;
            fseek(fp , 0L, SEEK_SET);
        }
    }

    if ( !writeHeadInfo() ) {
        Log::log(0,"写文件列信息出错！" );
    }

    return true;
}

bool FileMgr::writeHeadInfo(){
    if ( !fp || !m_sFileFieldInfo[0]) {
        return false;
    }
    if (fwrite(m_sFileFieldInfo, strlen(m_sFileFieldInfo), 1, fp) != 1 ||
        fwrite("\n", 1, 1, fp) != 1) {
        m_poLogGroup->log(MBC_FILE_WRITE_FAIL,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"写文件失败[%s]",m_sFileName);
        Log::log(0, "写文件列信息出错！");
        return false;
    }

    return true;
}

bool FileMgr::addData(char *sData,  int iCnt){
    int iOffset;
    char *pSrc;

    while (iCnt-->0) {
        m_sData[0]='\0';
        iOffset=0;
        for (int i=0;i<m_iFileFieldCnt;i++) {

            switch (m_sFileTransType[i]) {
            case FILEMGR_TRANS_STRING:
            case FILEMGR_TRANS_EVTDATE:
                pSrc= sData+m_iFileFieldOffSet[i];
                while (*pSrc) {
                    m_sData[iOffset++]=*pSrc++ ;
                }
                break;

            case FILEMGR_TRANS_INT:
                sprintf(&m_sData[iOffset],"%d", *(int*)(sData+m_iFileFieldOffSet[i]) );
                while (1) {
                    iOffset++;
                    if (m_sData[iOffset]==NULL)
                        break;
                } 
                break;

            case FILEMGR_TRANS_LONG:
                sprintf( &m_sData[iOffset], "%ld", *(long*)(sData+m_iFileFieldOffSet[i]) );
                while (1) {
                    iOffset++;
                    if (m_sData[iOffset]==NULL)
                        break;
                }
                break;

            case FILEMGR_TRANS_CHAR:
                {
                    int iTempChar=*(int *)(sData+m_iFileFieldOffSet[i]);
                    m_sData[iOffset]= iTempChar & 0x007F;
                    iOffset++;
                    m_sData[iOffset]=NULL;
                }
                break;

            case FILEMGR_TRANS_FLOAT:
                sprintf(&m_sData[iOffset],"%f", *(float*)(sData+m_iFileFieldOffSet[i]) );
                while (1) {
                    iOffset++;
                    if (m_sData[iOffset]==NULL)
                        break;
                }
                break;

            case FILEMGR_TRANS_DOUBLE:
                sprintf(&m_sData[iOffset],"%lf", *(double*)(sData+m_iFileFieldOffSet[i]) );
                while (1) {
                    iOffset++;
                    if (m_sData[iOffset]==NULL)
                        break;
                }
                break;

            default:
                Log::log(0, "未知数据类型，无法转化！");
                break;

            }

            m_sData[iOffset]=m_sDivChar;
            iOffset++;
            m_sData[iOffset]=NULL;

        }//end for

        m_sData[iOffset++]='\n';
        m_sData[iOffset]=NULL;

        if ( !writeFileData(m_sData, iOffset) )
            return false;
        sData += m_iDataStructSize;

    }//end while

    return true;
}

bool FileMgr::writeFileData(char *sData, int iDataLen ){
    static int iOffset=0;

    if (fp==NULL)
        return false;

    if (fwrite(sData, iDataLen, 1, fp)  !=1)
        return false;

    return true;
}
