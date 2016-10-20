#ifndef DATAEXPORTOR_H_HEADER_INCLUDED
#define DATAEXPORTOR_H_HEADER_INCLUDED

#include "HashList.h"
#include "SHMSqlMgr.h"
#include "ThreeLogGroup.h"
//#include "CommandCom.h"
#include "ReadCfg.h"
#include "TableStruct.h"

#define DATAFILE  "datafile/"

#define ABMPROCONFIG  "etc/"

int connectTT(char *memDSN);


class DataExportor:public TTTns {
public:
    int g_argc;
    char **g_argv;
    char* m_sMemDsn;              //内存标识：Key或者内存表名
    char* m_sDbTable;               //数据库表名
    char* m_sFileName;              //导出的文件名
    int m_iMode;                    //1:导出到数据库表,2:导出到文件
    bool m_bIgnore;                 //不用将导出输出到屏幕上
    int m_iTableType;
    HashList<int> *m_phList;        //内存标识，表类型  
    ThreeLogGroup *m_poLogGroup;
    //CommandCom *m_poCommandCom;
    
    //static TimesTenCMD  *qryCursor2;
	
    //SHMSqlMgr *g_poSql;
public:
    DataExportor(int argc, char **argv);
    ~DataExportor();
    int run();
	
	int check();
	char m_sConnTns[256];

public:
    ReadCfg *m_poConfS; 
	int TableCol;
	int ColFlag[20];
	char ColName[20][30];
  string sql;
	int QurSql();
	int expdata();
	int iTotalCnt;
	FILE *fp;
private:
    void reset();
    void init();
    void printUsage();
    bool prepare();
};


#endif //#ifndef DATAEXPORTOR_H_HEADER_INCLUDED
