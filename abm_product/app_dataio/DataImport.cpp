#include "DataImport.h"
#include "FileMgr.h"
#include "Log.h"
#include "MBC_28.h"
//#include "Exception.h"
//#include "Environment.h"

#define MAXLINE 1024

ABMException oExp;
char HomeEnv[128] = {0};


TTConnection *TTConn;
TimesTenConn *m_poTTConn;
TimesTenCMD *m_poPublicConn;

void itoaX (int n,char s[])
{ int i,j,sign;
if((sign=n)<0)//记录符号 
n=-n;//使n成为正数    
i=0; 
do
{ 
s[i++]=n%10+'0';//取下一个数字 
}
while ((n/=10)>0);//删除该数字
if(sign<0) 
s[i++]='-'; 
s[i]='\0'; 
for(j=i;j>=0;j--)//生成的数字是逆序的，所以要逆序输出 
printf("%c",s[j]);
}

int connectTT(char *memDSN){

  if (GET_CURSOR(m_poPublicConn, m_poTTConn, memDSN, oExp) != 0) 
	{
		Log::log(0, "连接数据库失败GET_CURSOR");
    return -1;
  }
        
   
	TTConn = m_poTTConn->getDBConn(true);
	if (TTConn == NULL)
	{
		Log::log(0, "连接数据库失败TTConn");
			return -1;
	}
    
    m_poPublicConn= new TimesTenCMD(TTConn);
	 if (m_poPublicConn == NULL)
	 {
		Log::log(0, "连接数据库失败m_poPublicConn");
			return -1;
	 }
	return 0;
}

void DataImport::printUsage(){
    printf("使用方法：\n");
    printf("======================= 数据导出工具:ABMexport version1.0.0 =======================\n"
           "hssimport -k [-q] -f | -t\n"
           "  -k 内存数据库的库名\n"
           "  -q 不用将导出输出到屏幕上\n"
           "  -f 从指定文件导入数据\n"
           "  -t 数据库表名\n"
           "示例：\n"
           "  ABMexport -k ACCU1 -t ABM_TABLE -f File: 把ABM_TABLE内存库中的数据导出到File文件中\n"
           "  导出时内存表时请先在数据库参数表中加以配置:\n"
           "==================================================================================\n");
}

DataImport::DataImport(int argc, char **argv) : g_argc(argc), g_argv(argv){
    Log::init(MBC_COMMAND_hbimport);
    reset();

    Log::log(0, "--进程启动--");
}

DataImport::~DataImport(){
    Log::log(0, "--进程退出(析构)--");
}

void DataImport::reset(){
		m_poLogGroup =0;
    m_bIgnore = false;
    m_iMode = 0;
    m_phList = 0;
    m_iTableType = 0;
    m_sMemDsn = 0;
    m_sDbTable = 0;
    m_sFileName = 0;
    init();
}

bool DataImport::prepare(){
    int iTemp=0; 
	  int jTemp=0;
    jTemp = g_argc;
    char *penv = NULL;
    
    char sfile[256] = {0};
    if ((penv=getenv("ABM_PRO_DIR")) == NULL) {
   	    Log::log(0, "环境变量\"ABM_PRO_DIR\"没有设置,  返回错误");
        ADD_EXCEPT0(oExp, "环境变量\"ABM_PRO_DIR\"没有设置,  返回错误");
        return -1;
    }
    strncpy(HomeEnv, penv, sizeof(HomeEnv)-2);
    if (HomeEnv[strlen(HomeEnv)-1] != '/') {
        HomeEnv[strlen(HomeEnv)] = '/';
    }

    for ( iTemp=1; iTemp<jTemp; ++iTemp ) {
        if (g_argv[iTemp][0] != '-') {
            m_poLogGroup->log(MBC_PARAM_THREAD_NONSUPP_ERROR,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"应用进程参数不正确（不支持的选项）!");
            printUsage ();
            return false;
        }

        switch (g_argv[iTemp][1] | 0x20) {
        case 'k':
            m_sMemDsn = g_argv[++iTemp];
            //bM = true;
            break;
        case 't':
            m_sDbTable = g_argv[++iTemp];
            m_iMode = 1;
            //bT = true;
            break;
        case 'f':
            m_sFileName = g_argv[++iTemp];
            //bF = true;
            m_iMode = 2;
            break;
        case 'q':
            m_bIgnore = true;
            break;
        default:
            m_poLogGroup->log(MBC_PARAM_THREAD_NONSUPP_ERROR,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"应用进程参数不正确（不支持的选项）!");
            printUsage ();
            return false;
            break;
        }
    }

	
	if(!strcmp(m_sMemDsn,"")){
	    m_poLogGroup->log(MBC_PARAM_THREAD_LOST_ERROR,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"应用进程参数不正确（缺少必选项）!");
        printUsage();
	    Log::log(0, "未发现-k 参数信息 !");
		return false;
	}
	
	if(!strcmp(m_sDbTable,"")){
	    m_poLogGroup->log(MBC_PARAM_THREAD_LOST_ERROR,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"应用进程参数不正确（缺少必选项）!");
        printUsage();
	    Log::log(0, "未发现-t 参数信息 !");
		return false;
	}
	
	if(!strcmp(m_sFileName,"")){
	    m_poLogGroup->log(MBC_PARAM_THREAD_LOST_ERROR,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"应用进程参数不正确（缺少必选项）!");
        printUsage();
	    Log::log(0, "未发现-f 导出文件信息 !");
		return false;
	}
    if(0==strcspn(m_sFileName,"/")){
	    m_poLogGroup->log(MBC_PARAM_THREAD_LOST_ERROR,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"输入文件为全路径");
	    Log::log(0, "输出文件为全路径!");
	}
	else
	{
    snprintf(sfile, sizeof(sfile), "%s%s%s", HomeEnv,DATAFILE, m_sFileName);	
    snprintf(m_sFileName, sizeof(sfile), "%s", sfile);
	    m_poLogGroup->log(MBC_PARAM_THREAD_LOST_ERROR,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"输入文件路径按照配置路径输出");
	    Log::log(0, "输出文件路径按照配置路径输出!");
	}
	

    return true;
}

int DataImport::check(){
    m_poConfS = NULL; 
    if (m_poConfS == NULL) { 
        if ((m_poConfS=new ReadCfg) == NULL) {
            Log::log(0, "ABMConf::getConf m_poConfS malloc failed!");
            return -1;
        }
    }
	  TableCol = 0;
    char path[1024];
	  int checkI=0;
	  int checkJ=0;
	  snprintf(path, sizeof(path), "%s%s%s", HomeEnv,ABMPROCONFIG, "abm_dataio.ini");
    //snprintf(path, sizeof(path), "/cbilling/app/petri/test.ini");
    if (m_poConfS->read(oExp, path, m_sDbTable) != 0) {
            Log::log(0, "check table config failed!");
            return -1;
    }
	TableCol = m_poConfS->getRows();
	for(checkI=0;checkI<TableCol;checkI++)
	{
	    checkJ = checkI+1;
	    cout<<"处理的数据条数"<<checkJ<<endl;
	    if (!m_poConfS->getValue(oExp, ColFlag[checkI], "COLFLAG", checkJ)) {
                  Log::log(0, "配置文件中COLFLAG,读取失败!");
                  return -1;
              }
			  
	          if (!m_poConfS->getValue(oExp, ColName[checkI], "COLNAME", checkJ)) {
                    Log::log(0, "配置文件中COLNAME,读取失败!");
                    return -1;
              }
	}
	return 0;
}

int DataImport::QurSql(){
        int i=0;
        char s[4];
		sql +=" insert into  ";
		sql +=m_sDbTable;

		sql +=" (";
		for(i=0;i<TableCol;i++){
			sql +=ColName[i];
			if(i<TableCol-1){
			    sql +=", ";
			}
		}
		
		sql +=") ";

		sql +=" values(";
		for(i=0;i<TableCol;i++){
		    sql +=":p";
		    itoaX(i,s);
			sql +=s;
			if(i<TableCol-1){
			    sql +=", ";
			}
		}
		sql +=")";
}

void DataImport::init()
{
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
    m_phList = new HashList<int>(101);
    if ( !m_phList )
    {
    		m_poLogGroup->log(MBC_MEM_MALLOC_FAIL,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"内存分配失败!");
        Log::log(0, "m_phList申请内存失败!");
        THROW(1);
    }
	/**
		g_poSql = NULL;
    if ((g_poSql=new SHMSqlMgr) == NULL) {
        Log::log(0, "DataImport::init g_poSql malloc failed!");
        THROW(1);
    }
    if (g_poSql->init(TABLE_MENDB_INFO,"MEM_DB_TABLE_RELATION") != 0) {
        Log::log(0, "DataImport::init g_poSql->init failed!");
        THROW(1);
    }
    TimesTenCMD *pTmp = g_poSql->m_poMenDb;
    try {
        pTmp->Execute();
        while (!pTmp->FetchNext()) {
        		char sMenName[16];
        		int  iTableType;
            pTmp->getColumn(1, sMenName);
            pTmp->getColumn(2, &iTableType);
            m_phList->add(sMenName, iTableType);
        }
        pTmp->Close();
    }
    catch(TTStatus st) {
        pTmp->Close();
        Log::log(0, "init failed! err_msg=%s", st.err_msg);
        THROW(1);
    }
	***/
}

int DataImport::run() {

    FILE *fp=NULL;
    string sFileName;
	sFileName.clear();
	char sLineContxt[1024]={0};	
	int iCnt=0;	
	char sCheckData[1024]={0};
	char *sDate="";
	char *delim =",";
	char *cp;
	int iRet =-1;
	int i=0;
	
    if ( !prepare() )
    {   
	    return -1;
	}
	
    if(0!=connectTT(m_sMemDsn))
	{
        return -1;	
    }
  

	if (0!=check())
	{
	
	    return -1;
	}
	QurSql();

    Log::log(0, "开始导入数据...");
	//fp=fopen(sFileName.c_str(),"r");
	fp=fopen(m_sFileName,"r");
	try {	
        while(1)
	    {
	        if(fgets(sLineContxt, MAXLINE, fp)==NULL)
	        {
	            Log::log(0, "文件处理结束");
	            break;
	        }
		    int iNum =1;
		    structDataT *pstructDataT = new structDataT;
		    m_poPublicConn->Close();
	        m_poPublicConn->Prepare(sql.c_str());
	        
	        if(ColFlag[iNum-1]==1){
		        cp = strtok(sLineContxt,delim);
		        //sDate
		        strcpy(pstructDataT->colConect,cp);
		        m_poPublicConn->setParam(iNum++, atol(pstructDataT->colConect));
		    }else{
		         if(ColFlag[iNum-1]==2){
		              cp = strtok(sLineContxt,delim);
		               //sDate
		              strcpy(pstructDataT->colConect,cp);
		              m_poPublicConn->setParam(iNum++, pstructDataT->colConect);
		         }else{
		             if(ColFlag[iNum-1]==3){
		               cp = strtok(sLineContxt,delim);
		               //sDate
		               strcpy(pstructDataT->colConect,cp);
		               m_poPublicConn->setParam(iNum++, pstructDataT->colConect);
		              }
		         }
		    }
		    while((cp=strtok(NULL,delim)))
		    {
		     //   if(iNum<TableCol){
		        if(ColFlag[iNum-1]==1)
			    {
			    	        strcpy(pstructDataT->colConect,cp);
                    m_poPublicConn->setParam(iNum++, atol(pstructDataT->colConect));
			    }
			    else{
			        if(ColFlag[iNum-1]==2)
			        {
			        	  strcpy(pstructDataT->colConect,cp);
			            m_poPublicConn->setParam(iNum++, pstructDataT->colConect);
			        }else{
				      if(ColFlag[iNum-1]==3)
				      {
				        strcpy(pstructDataT->colConect,cp);
			          m_poPublicConn->setParam(iNum++, pstructDataT->colConect);
				      }
				    }
			    }
			  //  }else{
			   //     Log::log(0, "不支持此表类型：TableType = %d", m_iTableType);
				   // return -1;
				//      continue;
			  //  }
						
		    }
		
		    m_poPublicConn->Execute();
	        m_poPublicConn->Commit();
		    m_poPublicConn->Close();
  
            //Log::log(0, "不支持此表类型：TableType = %d", m_iTableType);
        }
	    if(fp!=NULL)
	    fclose(fp);
	}
    catch(TTStatus st) {
        m_poPublicConn->Close();
        Log::log(0, "init failed! err_msg=%s", st.err_msg);
        THROW(1);
    }
    return 0;
}
