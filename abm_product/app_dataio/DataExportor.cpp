#include "DataExportor.h"
#include "FileMgr.h"
#include "Log.h"
#include "MBC_28.h"
#include "Exception.h"
#include "TimesTenAccess.h"
//#include "Environment.h"
static vector<structData *> vstructData;

ABMException oExp;
char HomeEnv[128] = {0};

TTConnection *TTConn;
TimesTenConn *m_poTTConn;
TimesTenCMD *m_poPublicConn;
TimesTenCMD  *qryCursor1;

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
		Log::log(0, "连接数据库失败qryCursor1");
			return -1;
	 }
	return 0;
}

/****
*    函数名：DataExportor::printUsage()
*    作用：判断传入参数规范提示
*    作者：夏开明
*    时间：2012-02-13
*    修改：
***/
void DataExportor::printUsage(){
    printf("使用方法：\n");
    printf("======================= 数据导出工具:ABMexport version1.0.0 =======================\n"
           "hssexport -k [-q] -f | -t\n"
           "  -k 内存数据库的库名\n"
           "  -q 不用将导出输出到屏幕上\n"
           "  -f 从指定文件导入数据\n"
           "  -t 数据库表名\n"
           "示例：\n"
           "  ABMexport -t ABM_TABLE -f File: 把ABM_TABLE内存库中的数据导出到File文件中\n"
           "  导出时内存表时请先在数据库参数表中加以配置:\n"
           "==================================================================================\n");
}

DataExportor::DataExportor(int argc, char **argv):g_argc(argc),g_argv(argv){
    Log::init(MBC_COMMAND_hbexport);
    reset();

    Log::log(0, "--进程启动--");
}

DataExportor::~DataExportor(){
	if(m_poConfS != NULL){
	    delete m_poConfS;
		m_poConfS = NULL;
	}
    Log::log(0, "--进程退出(析构)--");
}


	

void DataExportor::reset(){
    m_poLogGroup =0;
    m_bIgnore = false;
    m_iMode = 0;
    m_phList = 0;
    m_iTableType = 0;
    m_sMemDsn = 0;
    m_sDbTable = 0;
    m_sFileName = 0;
    iTotalCnt=0;
	sql="";
    init();
	  
}
/****
*    函数名：DataExportor::prepare()
*    作用：判断传入参数是否符合程序规范
*    作者：夏开明
*    时间：2012-02-13
*    修改：
***/
bool DataExportor::prepare(){
    int iTemp=0; 
	  int jTemp=0;
    jTemp = g_argc;
    char *penv = NULL;
    
    char sfile[256] = {0};
    if ((penv=getenv("ABM_PRO_DIR")) == NULL) {
   	    Log::log(0, "环境变量\"ABM_PRO_DIR\"没有设置,  返回错误");
        //ADD_EXCEPT0(oExp, "环境变量\"ABM_PRO_DIR\"没有设置,  返回错误");
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
	    //m_poLogGroup->log(MBC_PARAM_THREAD_LOST_ERROR,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"输出文件为全路径");
	    Log::log(0, "输出文件为全路径!");
	}
	else
	{
		

    snprintf(sfile, sizeof(sfile), "%s%s%s", HomeEnv,DATAFILE, m_sFileName);	
    snprintf(m_sFileName, sizeof(sfile), "%s", sfile);		
	    //m_poLogGroup->log(MBC_PARAM_THREAD_LOST_ERROR,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"输出文件路径按照配置路径输出");
	    Log::log(0, "输出文件路径按照配置路径输出!");
	}
	
  
    return true;
}

void DataExportor::init(){
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
}

int DataExportor::check(){
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

int DataExportor::QurSql(){
        int i=0;
        
		sql +=" SELECT ";
		for(i=0;i<TableCol;i++){
		    if(ColFlag[i]==1){
		        sql +="to_char(";
				sql +=ColName[i];
				sql +=")";
		    }else {
			    if(ColFlag[i]==2)
				{
				    sql +=ColName[i];
				}else{
				    if(ColFlag[i]==3){
					    sql +="to_char(";
				        sql +=ColName[i];
				        sql +=")";
					}
				}
			}
			if(i<TableCol-1){
			    sql +=", ";
			}
		}
		sql +=" from ";
		sql +=m_sDbTable;
}

int DataExportor::expdata(){
	try{
    m_poPublicConn->Prepare(sql.c_str());
	m_poPublicConn->Execute();
    
	char Conect[30];
	while (!m_poPublicConn->FetchNext()) 
		{	string sConect="";
			int i=1;
			structData *pstructData = new structData;
			while(i<TableCol+1){
			    m_poPublicConn->getColumn(i++, Conect);
				sConect +=Conect;
				if(i<TableCol+1){
				sConect +=",";
				}
			}
			sprintf(pstructData->colConect,"%s",sConect.c_str());
			vstructData.push_back(pstructData);
			if (vstructData.size()>1000){
			iTotalCnt=iTotalCnt+vstructData.size();
		  for (i=0; i<vstructData.size(); i++)
	    {
		    fprintf(fp,"%s \n",vstructData[i]->colConect);
	    }
	    for(int j_a=0;j_a<vstructData.size();j_a++) {
        delete vstructData[j_a];
      } 
       vstructData.clear();
	    }
			
		}	
		if (vstructData.size()>0){
			iTotalCnt=iTotalCnt+vstructData.size();
		  for (int i=0; i<vstructData.size(); i++)
	    {
		    fprintf(fp,"%s \n",vstructData[i]->colConect);
	    }
	    for(int j_a=0;j_a<vstructData.size();j_a++) {
        delete vstructData[j_a];
      } 
       vstructData.clear();
	    }
	  }catch(TTStatus st) {
        m_poPublicConn->Close();
        Log::log(0, "expdata failed! err_msg=%s", st.err_msg);
        THROW(1);
    }
   return 0;
}	


int DataExportor::run(){
	
    if ( !prepare() )
	{
        return -1;
    }
    
    if(0!=connectTT(m_sMemDsn)){
     return -1;	
    }
  

	if (0!=check())
	{
	
	    return -1;
	}
	QurSql();
	fp=NULL;		
	fp=fopen(m_sFileName,"w+");	
    
	if(fp==NULL)
	{
		//m_poLogGroup->log(MBC_FILE_OPEN_FAIL,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"打开文件失败[%s]",m_sFileName);
		return -1;
	}
		
    Log::log(0, "开始导出数据...");
	int i=0;
	int iRet=-1;
	try{
	    if (0!=expdata())
	    {
	    	return -1;
	    }
	  
    }catch (...){
        //m_poLogGroup->log(MBC_MEM_EXPORT_FAIL,LOG_LEVEL_ERROR,LOG_CLASS_ERROR,LOG_TYPE_PROC,-1,NULL,"HCode内存数据导出异常!");
    }
                
    Log::log (0, "共导出 %d 条数据!", iTotalCnt);
	for(int j_a=0;j_a<vstructData.size();j_a++) {
        delete vstructData[j_a];
    } 
    vstructData.clear();
    if(fp!=NULL)
	fclose(fp);
    return 1;
}

