#include "LogDB.h"
#include "Process.h"

bool LogDB::m_bInit = false;
int LogDB::m_iLogLevel = -1;
int LogDB::m_iSysProcID = 0;
char LogDB::m_sInitPath[80] = "";
char LogDB::m_sLockPath[100] = "";
CSemaphore * LogDB::m_poLogDBFileLock = 0;
LogDBBuf * LogDB::m_pLogDBBuf = 0;
TOCIQuery * LogDB::m_pQry = 0;
int LogDB::m_iLogSeqID = 0;
int LogDB::m_iLogSeqIDMax = 0;
int LogDB::m_iLogDBSwitch = 0;

LogDB::LogDB()
{
}

LogDB::~LogDB()
{
    if(m_poLogDBFileLock){
        delete m_poLogDBFileLock;
        m_poLogDBFileLock = 0;
    }
    
    if(m_pLogDBBuf){
        delete m_pLogDBBuf;
        m_pLogDBBuf = 0;
    }
    
    if(m_pQry){
        delete m_pQry;
        m_pQry = 0;
    }
}

void LogDB::init()
{
    char *p =  NULL;
    char sTemp[32];
    
	if((p = getenv("TIBS_HOME")) == NULL)
		sprintf (m_sInitPath, "/home/bill/TIBS_HOME");
	else
		sprintf (m_sInitPath, "%s", p);
    
    sprintf(m_sLockPath, "%s/lockpath/LogDB", m_sInitPath);
    
    if(!m_poLogDBFileLock){
        m_poLogDBFileLock = new CSemaphore ();
        sprintf(sTemp, "%d", LOGDB_FILE_LOCK);
        m_poLogDBFileLock->getSem (sTemp, 1, 1);
    }
    
    m_pLogDBBuf = new LogDBBuf();
    
    m_iLogSeqIDMax = MAX_SHMBUF_NUM;
    
    //logFileOper();
}

int LogDB::getSysProc()
{
    int fd;
    int iSysProcID = 0;
    char buf[10];
    
    m_poLogDBFileLock->P();
    if((fd = open(m_sLockPath, O_RDONLY)) < 0)
        iSysProcID = 0;
    else{
        memset(buf, 0, sizeof(buf));
        read(fd, buf, sizeof(buf));
        close(fd);
        
        iSysProcID = atoi(buf);
    }
    m_poLogDBFileLock->V();

    return iSysProcID;
}

int LogDB::logFileOper()
{
    int fd;
    char buf[10];
    int iForkRet = 0;
    int iReadVal = 0;
    
    m_poLogDBFileLock->P();
    if((fd = open(m_sLockPath, O_RDONLY)) < 0)
        m_iSysProcID = 0;
    else{
        memset(buf, 0, sizeof(buf));
        iReadVal = read(fd, buf, sizeof(buf));
        close(fd);
        
        m_iSysProcID = atoi(buf);
    }
    
    if(!m_iSysProcID || kill(m_iSysProcID, 0) != 0){
        iForkRet = fork();
        switch(iForkRet){
            case 0:
                writeLogDB();
                break;
                
            case -1:
                perror("fork");
                cout<<"start_Command: fork err"<<endl;
                exit(0);
                break;
                
            default:
                cout<<"\n====>> Child process create ,pid:"<<iForkRet<<endl;
                if((fd = open(m_sLockPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
                    return -1;
                
                m_iSysProcID = iForkRet;
                
                if(ftruncate(fd, 0) < 0)
                    return -1;
                
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "%d\n", m_iSysProcID);
                if(write(fd, buf, strlen(buf)) != (int)strlen(buf)){
                    close(fd);
                    return -1;
                }
                close(fd);
                Environment::getDBConn(true);
                break;
        }
    }
    m_poLogDBFileLock->V();
    
    return 0;
}

void LogDB::freeSHMBuf()
{
    m_pLogDBBuf->unloadSHMBuf();
}

void LogDB::logDB(int log_level, int log_type, char const *format, ...)
{
    char *p, sTemp[MAX_STRING_NUM], sLine[MAX_STRING_NUM];
    LogDBData oTempLogInfo;
    Date dTemp;
    
    if(m_bInit != true){
        m_bInit = true;
        
        ParamDefineMgr::getParam("LOGDB", "LOGDB_SWITCH", sTemp, 5);
        m_iLogDBSwitch = atoi(sTemp);
        if(!m_iLogDBSwitch)
            return;
        //init();
        m_pLogDBBuf = new LogDBBuf();
    }
    
    if(m_iLogLevel == -1){
        if ((p = getenv("LOG_LEVEL")) == NULL)
            m_iLogLevel = 0;
        else
            m_iLogLevel = atoi(p);
    }
    
    if(log_level > m_iLogLevel || !m_iLogDBSwitch)
         return;
    
    memset(&oTempLogInfo, 0, sizeof(oTempLogInfo));
    oTempLogInfo.iProcID = Process::m_iProcID;
    oTempLogInfo.iFlowID = Process::m_iFlowID;
    oTempLogInfo.iAppID = Process::m_iAppID;
    oTempLogInfo.iLogLevel = log_level;
    oTempLogInfo.iLogType = log_type;
    strcpy(oTempLogInfo.sCreatedDate, dTemp.toString((char *)"YYYYMMDDHHMISS"));
    oTempLogInfo.sCreatedDate[MAX_CREATED_DATE_LEN - 1] = '\0';
    
    va_list ap;
    va_start(ap, format);
    vsnprintf(sLine, MAX_STRING_NUM-1, format, ap);
    va_end(ap);
    strncpy(oTempLogInfo.sLogInfo, sLine, MAX_STRING_NUM-1);
    
    while(! m_pLogDBBuf->writeCirBuf(oTempLogInfo))  // ¹²ÏíÄÚ´æÂú
        usleep(100);
}

void LogDB::writeLogDB()
{
    LogDBData oTempLogDBData;
    
    Environment::getDBConn(true);
    m_pQry = new TOCIQuery (Environment::getDBConn());
    m_pQry->setSQL(
        "insert into b_log_info(log_id, proc_id, flow_id, "
        "app_id, created_date, log_level, log_type, log_info) values "
        "(:vLogID, :vProcID, :vFlowID, :vAppID, "
        "to_date(:vCreatedDate, 'yyyymmddhh24miss'), :vLogLevel, "
        ":vLogType, :vLogInfo)"
    );
    while(1){
        if(m_pLogDBBuf->isEmpty()){
            usleep(200);
            continue;
        }
        
        memset(&oTempLogDBData, 0, sizeof(oTempLogDBData));
        m_pLogDBBuf->readCirBuf(oTempLogDBData);
        insertDBData(oTempLogDBData);
    }
}

void LogDB::insertDBData(LogDBData & oLogInfo)
{
    static long lLogID = 0;
    
    if(m_iLogSeqID == 0 || m_iLogSeqID == m_iLogSeqIDMax){
        m_iLogSeqID = 0;
        
        DEFINE_QUERY(qry);
        qry.setSQL("select seq_log_info_id.nextval from dual");
        qry.open();
        qry.next();
        lLogID = qry.field(0).asLong();
        qry.close();
    }
    
    m_pQry->setParameter("vLogID", lLogID + m_iLogSeqID++);
    m_pQry->setParameter("vProcID", oLogInfo.iProcID);
    m_pQry->setParameter("vFlowID", oLogInfo.iFlowID);
    m_pQry->setParameter("vAppID", oLogInfo.iAppID);
    m_pQry->setParameter("vLogLevel", oLogInfo.iLogLevel);
    m_pQry->setParameter("vLogType", oLogInfo.iLogType);
    m_pQry->setParameter("vCreatedDate", oLogInfo.sCreatedDate);
    m_pQry->setParameter("vLogInfo", oLogInfo.sLogInfo);
    m_pQry->execute();
    m_pQry->commit();
}

