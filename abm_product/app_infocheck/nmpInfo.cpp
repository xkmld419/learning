#include "nmpInfo.h"
DEFINE_MAIN(CnmpInfo)

std::vector<T_oidInfo*> CnmpInfo::m_lowInfo;
std::vector<T_oidInfo*> CnmpInfo::m_highInfo;
std::vector<T_oidInfo*> CnmpInfo::m_trapInfo;
TOCIQuery* CnmpInfo::m_pQry = NULL;
std::string CnmpInfo::m_sCityCode;
CmutexLock* CnmpInfo::m_pMutexLock = NULL;

CnmpInfo::CnmpInfo()
{
    m_pLogManage = NULL;
    m_pQry = NULL;
}

CnmpInfo::~CnmpInfo()
{
    FREE(m_pLogManage);
    FREE(m_pQry);
    FREE(m_pMutexLock);
    for(std::vector<T_oidInfo*> ::iterator iter = m_lowInfo.begin(); iter !=m_lowInfo.end(); iter++)
    {
        FREE(*iter);
    }
    for(std::vector<T_oidInfo*> ::iterator iter = m_highInfo.begin(); iter !=m_highInfo.end(); iter++)
    {
        FREE(*iter);
    }
    for(std::vector<T_oidInfo*> ::iterator iter = m_trapInfo.begin(); iter !=m_trapInfo.end(); iter++)
    {
        FREE(*iter);
    }
}

int CnmpInfo::init()
{
    m_pMutexLock = new CmutexLock;
    try
    {
        char szSql[MAX_SQL_LENGTH]= {0};

        m_pQry->close();
        memset(szSql,0x00,sizeof(szSql));
        char* username = getenv("LOGNAME");
        char hostname[100];
        memset(hostname,0x00,sizeof(hostname));
        int iret =  gethostname(hostname,100);
        if(iret < 0)
        {
            LOG_DEBUG("get host name error");
            return -1;
        }
        sprintf(szSql,"select t.param_value  \
                                from b_param_define t  \
                                where t.param_segment = 'BSN_NMP_PARAM'  \
                                and t.param_code = 'CITY_CODE'  \
                                and t.sys_username in '%s'  \
                                and t.host_id in  \
                                (select t.host_id from wf_hostinfo t where t.host_name = '%s') ",username,hostname);

        m_pQry->setSQL(szSql);
        m_pQry->open();
        if(m_pQry->next() == true)
        {
            m_sCityCode = m_pQry->field( 0 ).asString();
        }
        else
        {
            LOG_DEBUG("请配置b_param_define中的CITY_CODE");
            return -1;
        }

        //高频信息点
        m_highInfo.clear();
        m_pQry->close();
        memset(szSql,0x00,sizeof(szSql));
        sprintf(szSql,"select t.oid,to_char(t.sql_value),t.remark,t.type from tb_nmp_info_def t  where t.frequency = 2");
        m_pQry->setSQL(szSql);
        m_pQry->open();
        while(m_pQry->next())
        {
            int i = 0;
            T_oidInfo* pOidInfo = new T_oidInfo;
            pOidInfo->s_sOid = m_pQry->field( i++ ).asString();
            pOidInfo->s_sSqlValue = m_pQry->field( i++ ).asString();
            pOidInfo->s_sContext = m_pQry->field( i++ ).asString();
            pOidInfo->s_iType = m_pQry->field( i++ ).asInteger();
            m_highInfo.push_back(pOidInfo);
        }
        m_pQry->close();

        //低频信息点
        m_lowInfo.clear();
        m_pQry->close();
        memset(szSql,0x00,sizeof(szSql));
        sprintf(szSql,"select t.oid,to_char(t.sql_value),t.remark,t.type from tb_nmp_info_def t  where t.frequency = 1");
        m_pQry->setSQL(szSql);
        m_pQry->open();
        while(m_pQry->next())
        {
            int i = 0;
            T_oidInfo* pOidInfo = new T_oidInfo;
            pOidInfo->s_sOid = m_pQry->field( i++ ).asString();
            pOidInfo->s_sSqlValue = m_pQry->field( i++ ).asString();
            pOidInfo->s_sContext = m_pQry->field( i++ ).asString();
            pOidInfo->s_iType = m_pQry->field( i++ ).asInteger();
            m_lowInfo.push_back(pOidInfo);
        }
        m_pQry->close();
    }
    catch ( TOCIException &e )
    {
        std::ostringstream os_log;
        os_log.str( "" );
        os_log << "***********************sql error*********************** \n" << std::endl
        << e.getErrCode()<< std::endl << e.GetErrMsg() << std::endl << e.getErrSrc() << std::endl;
        LOG_DEBUG( os_log.str().c_str());
        return  - 1;
    }
    return 1;
}

int CnmpInfo::run()
{
    //DenyInterrupt();
    //创建日志组件
    m_pLogManage = CLogManage::getInstance();
    if ( m_pLogManage->setLog( GetProcessID()) < 0 )
    {
        std::cerr << __FILE__ << "  " << __LINE__ << "  " << "log error" << std::endl;
        return -1;
    }

    //创建数据库连接对象
    TOCIDatabase* pDB = Environment::getDBConn();
    if ( NULL == pDB )
    {
        LOG_DEBUG( "get db info error" );
        return -1;
    }

    m_pQry = new TOCIQuery( pDB );
    if ( NULL == m_pQry )
    {
        LOG_DEBUG( "create oci qry object error" );
        return -1;
    }

    if(init()< 0)
    {
        LOG_DEBUG( "init error" );
        return -1;
    }

    LOG_DEBUG( "NMP信息点提取开始运行..." );
    char szLog[MAX_STRING_LENGTH]= {0};
    pthread_t thread_id1;
    pthread_t thread_id2;
    int iRet = pthread_create(&thread_id1,NULL,thread_highInfo,NULL);
    if(iRet!=0)
    {
        LOG_DEBUG("create thread error");
        return -1;
    }
    iRet = pthread_create(&thread_id2,NULL,thread_lowInfo,NULL);
    if(iRet!=0)
    {
        LOG_DEBUG("create thread error");
        return -1;
    }
	
    //主线程实时检测子线程是否退出
    //如退出则进程退出
    while(true)
    {
        int pthread_kill_err =-1;
        //检查线程是否还在
        pthread_kill_err = pthread_kill(thread_id1,0);
        if(pthread_kill_err == ESRCH)
        {
            memset(szLog,0x00,sizeof(szLog));
            sprintf(szLog,"ID为0x%x的线程不存在或者已经退出",(unsigned int)thread_id1);
            LOG_DEBUG(szLog);
            return -1;
        }
        else if(pthread_kill_err == EINVAL)
        {
            LOG_DEBUG("发送信号非法");
        }
        else
        {
            memset(szLog,0x00,sizeof(szLog));
            sprintf(szLog,"ID为0x%x的线程目前仍然存活。",(unsigned int)thread_id1);
        }

        //检查线程是否还在
        pthread_kill_err = pthread_kill(thread_id2,0);
        if(pthread_kill_err == ESRCH)
        {
            memset(szLog,0x00,sizeof(szLog));
            sprintf(szLog,"ID为0x%x的线程不存在或者已经退出",(unsigned int)thread_id2);
            LOG_DEBUG(szLog);
            return -1;
        }
        else if(pthread_kill_err == EINVAL)
        {
            LOG_DEBUG("发送信号非法");
        }
        else
        {
            memset(szLog,0x00,sizeof(szLog));
            sprintf(szLog,"ID为0x%x的线程目前仍然存活。",(unsigned int)thread_id2);
        }
        usleep(99999);
    }

    return 1;

}

void* CnmpInfo::thread_highInfo(void* _args)
{
    pthread_detach(pthread_self()) ;

    char szLog[MAX_STRING_LENGTH]= {0};
    char szSql[MAX_SQL_LENGTH*4]= {0};
    while(true)
    {
        for(std::vector<T_oidInfo*>::const_iterator iter = m_highInfo.begin(); iter != m_highInfo.end(); iter++)
        {
            memset(szSql,0x00,sizeof(szSql));
            try
            {
                sprintf(szSql,"insert into stat_interface    \
                                        (STAT_ID,    \
                                        KPI_CODE,    \
                                        VALUE,    \
                                        STAT_BEGIN_DATE,    \
                                        STAT_END_DATE,    \
                                        STATE,    \
                                        AREA_CODE,    \
                                        CREATE_DATE,    \
                                        DEAL_FLAG,    \
                                        DEAL_DATE,    \
                                        MODULE_ID,    \
                                        DIMENSIONS,    \
                                        DIMENSION_FLAG,    \
                                        STAT_DATE_FLAG,    \
                                        DB_SOURCE_ID,    \
                                        MIN_STAT_DATE,    \
                                        MAX_STAT_DATE)    \
                                        select SEQ_MNT_STAT_ID.Nextval,    \
                                        '%s',     \
                                        0,    \
                                        sysdate,    \
                                        sysdate,    \
                                        0,    \
                                        '%s',     \
                                        sysdate,    \
                                        1,    \
                                        sysdate,    \
                                        1,    \
                                        a.t,    \
                                        %d,     \
                                        2,    \
                                        1,     \
                                        sysdate,sysdate    \
                                        from ( %s ) a  where a.t is not null",(*iter)->s_sOid.c_str(),m_sCityCode.c_str(),(*iter)->s_iType,(*iter)->s_sSqlValue.c_str());
                //共享的数据库连接对象在多线程情况下要加锁
                m_pMutexLock->lock();
                m_pQry->close();
                m_pQry->setSQL(szSql);
                m_pQry->execute();
                m_pQry->commit();
                m_pQry->close();
                //解锁
                m_pMutexLock->unlock();
                memset(szLog,0x00,sizeof(szLog));
                sprintf(szLog,"高频信息点....................\nOID : %s \nCONTEXT: %s \nSQL: %s",(*iter)->s_sOid.c_str(),(*iter)->s_sContext.c_str(),(*iter)->s_sSqlValue.c_str());
                LOG_DEBUG(szLog);
            }
            catch ( TOCIException &e )
            {
                std::ostringstream os_log;
                os_log.str( "" );
                os_log << "***********************sql error*********************** \n" << std::endl
                << e.getErrCode()<< std::endl << e.GetErrMsg() << std::endl << e.getErrSrc() << std::endl<<
                "$----------------------$\n"<<szSql;
                LOG_DEBUG( os_log.str().c_str());
                //pthread_exit(NULL);
            }
        }

        LOG_DEBUG("高频信息点入表一次,等待三分钟之后继续...");
        sleep(3*60);
    }
    return NULL;
}

void* CnmpInfo::thread_lowInfo(void* _args)
{
    pthread_detach(pthread_self()) ;

    char szLog[MAX_STRING_LENGTH]= {0};
    char szSql[MAX_SQL_LENGTH]= {0};
    while(true)
    {
        for(std::vector<T_oidInfo*>::const_iterator iter = m_lowInfo.begin(); iter != m_lowInfo.end(); iter++)
        {
            memset(szSql,0x00,sizeof(szSql));
            try
            {
                sprintf(szSql,"insert into stat_interface    \
                                        (STAT_ID,    \
                                        KPI_CODE,    \
                                        VALUE,    \
                                        STAT_BEGIN_DATE,    \
                                        STAT_END_DATE,    \
                                        STATE,    \
                                        AREA_CODE,    \
                                        CREATE_DATE,    \
                                        DEAL_FLAG,    \
                                        DEAL_DATE,    \
                                        MODULE_ID,    \
                                        DIMENSIONS,    \
                                        DIMENSION_FLAG,    \
                                        STAT_DATE_FLAG,    \
                                        DB_SOURCE_ID,    \
                                        MIN_STAT_DATE,    \
                                        MAX_STAT_DATE)    \
                                        select SEQ_MNT_STAT_ID.Nextval,    \
                                        '%s',     \
                                        0,    \
                                        sysdate,    \
                                        sysdate,    \
                                        0,    \
                                        '%s',     \
                                        sysdate,    \
                                        1,    \
                                        sysdate,    \
                                        1,    \
                                        a.t,    \
                                        %d,     \
                                        2,    \
                                        1,     \
                                        sysdate,sysdate    \
                                        from ( %s ) a where a.t is not null",(*iter)->s_sOid.c_str(),m_sCityCode.c_str(),(*iter)->s_iType,(*iter)->s_sSqlValue.c_str());
                //共享的数据库连接对象在多线程情况下要加锁
                m_pMutexLock->lock();
                m_pQry->close();
                m_pQry->setSQL(szSql);
                m_pQry->execute();
                m_pQry->commit();
                m_pQry->close();
                //解锁
                m_pMutexLock->unlock();
                memset(szLog,0x00,sizeof(szLog));
                sprintf(szLog,"低频信息点....................\nOID : %s \nCONTEXT: %s \nSQL: %s",(*iter)->s_sOid.c_str(),(*iter)->s_sContext.c_str(),(*iter)->s_sSqlValue.c_str());
                LOG_DEBUG(szLog);
            }
            catch ( TOCIException &e )
            {
                std::ostringstream os_log;
                os_log.str( "" );
                os_log << "***********************sql error*********************** \n" << std::endl
                << e.getErrCode()<< std::endl << e.GetErrMsg() << std::endl << e.getErrSrc() << std::endl<<
                "$----------------------$\n"<<szSql;
                LOG_DEBUG( os_log.str().c_str());
                //pthread_exit(NULL);
            }
        }

        LOG_DEBUG("低频信息点入表一次,等待一天后继续...");
        sleep(24*60*60);
    }
    return NULL;

}

void* CnmpInfo::thread_trapInfo(void* _args)
{
    return NULL;
}
