#ifndef __AUTO_CLEAR_ABMTT_H_
#define __AUTO_CLEAR_ABMTT_H_

#include "TimesTenAccess.h"
#include "ABMException.h"
#include "ReadCfg.h"

//#include "ThreeLogGroup.h"


struct ABMClearInfo
{
    char m_cMDBTable[32+1]; //内存库表名
    char m_cMDBName[32+1];//tt库名
    char m_cTimeName[32+1];//时间字段名    
    int  m_iTableType; //表的类型，0代表累计量，1代表余额
};

struct ABMArchiveData
{
    char m_cVectorData[1024];
};



class ABMClearProduct
{
public:
    ABMClearProduct();
    ~ABMClearProduct();
    int clearInit(ABMException &oExp);
    int getABMClearConf(ABMException &oExp, char *pTableName, char *pTTDBName);
    int executeClear(ABMException &oExp);  //abmclear
     
    int connectTT(ABMException &oExp);
    int executeArchive(ABMException &oExp,char *sDbName);
    int executeArchiveSQL(ABMException &oExp, char *sDbTable, char *cDbName);   
private:
    vector<ABMClearInfo> m_oABMClearConf;
    
    char m_sTTStr[32+1]; 
    ReadCfg m_poTableCfg;
    int m_iLineNum;
    char m_sFileName[1024]; 
    
    
    TimesTenConn *m_poTTconn; //TT库连接
    TimesTenCMD *m_poClearMDB; //TT 游标 
    
    string m_sArchiveSQL;
    vector<ABMArchiveData> m_oABMArchiveConf;
    int m_iFieldNum; //字段数
    
    
    
    /*
        int m_iColFlag[20];
    char m_cColName[20][30];
    */
public:
    char  m_sDateTime[14 + 1]; //时间 
    FILE *m_poArchiveData;
   
};

#endif