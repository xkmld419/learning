#include <math.h>
#include <string.h>
#include <stdio.h>
#include "AutoClearABMTT.h"
//#include "Log.h"


#define MDB_ABM_ACCU_DEL_SQL "delete from %s where %s<%s and rownum<=1024"
#define MDB_ABM_BAL_DEL_SQL "delete from %s where %s<to_date('%s', 'YYYYMMDDHH24MISS') and rownum<=1024"

#define MDb_ABM_ACCU_NUMSQL "select count(*) from %s where %s<%d"
#define MDB_ABM_BAL_NBUMSQL "select count(*) from %s where %s<to_date('%s', 'YYYYMMDDHH24MISS')"

#define MDB_ABM_DEL_MAX 1024
static char g_oConn[32]="TIMESTEN";

ABMClearProduct::ABMClearProduct()
{
    m_iLineNum=0;
    m_poTTconn = NULL;
    m_poClearMDB = NULL;
    m_sArchiveSQL = "";
    m_iFieldNum =0 ;
    m_poArchiveData = NULL;
    memset(m_sDateTime, 0x00, sizeof(m_sDateTime));
    memset(m_sFileName, 0x00, sizeof(m_sFileName));        
}

int ABMClearProduct::clearInit(ABMException &oExp)
{
    char *penv = getenv("ABM_PRO_DIR");       
    if (!penv) {
        ADD_EXCEPT0(oExp, "miss environment ABMAPP_DEPLOY, please add it to your .profile");
        return -1;
    }
    if (penv[strlen(penv)-1] != '/') {
        snprintf (m_sFileName, sizeof(m_sFileName)-1, "%s/etc/abm_dataio.ini",penv);
    } else {
        snprintf (m_sFileName, sizeof(m_sFileName)-1, "%setc/abm_dataio.ini",penv);
    }  
    return 0;  
}


ABMClearProduct::~ABMClearProduct()
{
    RelTTCursor(m_poClearMDB);
    if(m_poTTconn) {
        m_poTTconn->disconnect();
        delete m_poTTconn;
        m_poTTconn = NULL;
    }
    if(m_poArchiveData!=NULL)
	    fclose(m_poArchiveData);    
}

int ABMClearProduct::getABMClearConf(ABMException &oExp, char *pTableName, char *pTTDBName)
{
    if((pTableName == NULL) || (pTTDBName == NULL))
    {
        ADD_EXCEPT0(oExp, "miss TableName or TTDBName, please add it to your TableName and TTDBName");
        return -1;
    } 
    if (m_poTableCfg.read(oExp, m_sFileName, "ABMClearTT" , '|') == -1) {
        ADD_EXCEPT1(oExp, "ReadCfg::read(%s, \"ABMClearTT\", '|') failed",m_sFileName);
        return -1;     
    }
    m_iLineNum = m_poTableCfg.getRows();
    
    char sValue[64+1];
    int iLines =1;
    ABMClearInfo oABMClearConf;
    while(iLines <= m_iLineNum) {
        memset(sValue, 0x00, sizeof(sValue));
        if(!m_poTableCfg.getValue(oExp, sValue, "TTtable_name", iLines)) {
            ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed", "TTtable_name", iLines);
            return -1;
        }
        if(0 != strcasecmp(sValue , pTableName)) {
            ++iLines;
            continue;
        }
        strncpy(oABMClearConf.m_cMDBTable, sValue, sizeof(oABMClearConf.m_cMDBTable)-1);
        
        memset(sValue, 0x00, sizeof(sValue));
        if(!m_poTableCfg.getValue(oExp, sValue, "TT_name",iLines)) {
            ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed", "DBtable_name", iLines);
            return -1;
        }
        if(0 != strcasecmp(sValue , pTTDBName)) {
            ++iLines;
            continue;
        }        
        strncpy(oABMClearConf.m_cMDBName, pTTDBName, sizeof(oABMClearConf.m_cMDBName)-1); 

        memset(sValue, 0x00, sizeof(sValue));
        if(!m_poTableCfg.getValue(oExp, sValue, "time_param",iLines)) {
            ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed", "time_param", iLines);
            return -1;
        }
        strncpy(oABMClearConf.m_cTimeName, sValue, sizeof(oABMClearConf.m_cTimeName)-1);       

        if(!m_poTableCfg.getValue(oExp, oABMClearConf.m_iTableType, "table_type",iLines)) {
            ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed", "table_type", iLines);
            return -1;
        }
        m_oABMClearConf.push_back(oABMClearConf);
        return 0;                              
    } 
    int tttnum=m_oABMClearConf.size();
    printf("%s",tttnum);
    return 0;          
}
int ABMClearProduct::connectTT(ABMException &oExp)
{
    m_poTTconn = new TimesTenConn();
    if(m_poTTconn == NULL) {            
                return -1;
    }
    if (ConnectTT(oExp, *m_poTTconn, m_sTTStr)) {
        ADD_EXCEPT0(oExp, "AutoClearLogSql::init ConnectTT链接tt出错！");
        return -1;
    }
    if (GET_CURSOR(m_poClearMDB, m_poTTconn, g_oConn, oExp) != 0) {
        ADD_EXCEPT0(oExp, "AutoClearLogSql::init m_poClearLog 游标初始化失败!");
        return -1;
    }
    return 0;    
}



int ABMClearProduct::executeClear(ABMException &oExp) 
{
    char sSql[256];
    vector<ABMClearInfo>::iterator icf;
    long inum=0;
    try {
        for(icf = m_oABMClearConf.begin(); icf != m_oABMClearConf.end(); ++icf) {

            snprintf(m_sTTStr,sizeof(m_sTTStr),"dsn=%s",icf->m_cMDBName);
            //snprintf(m_sTTStr,sizeof(m_sTTStr),icf->m_cMDBName);
            connectTT(oExp);
            //int iNumClear=
            if(0 == icf->m_iTableType) {
                memset(sSql, 0x00, 256);
                snprintf(sSql, sizeof(sSql), MDb_ABM_ACCU_NUMSQL, icf->m_cMDBTable, icf->m_cTimeName, m_sDateTime);     
                m_poClearMDB->Prepare(sSql);
                m_poClearMDB->Execute();
                while(!m_poClearMDB->FetchNext()){
                    m_poClearMDB->getColumn(1, &inum);
                }
                m_poClearMDB->Commit();
                m_poClearMDB->Close();
                memset(sSql, 0x00, 256);
                snprintf(sSql, sizeof(sSql), MDB_ABM_ACCU_DEL_SQL, icf->m_cMDBTable, icf->m_cTimeName, m_sDateTime);   
            } else if(1 == icf->m_iTableType) {
                memset(sSql, 0x00, 256);
                snprintf(sSql, sizeof(sSql), MDB_ABM_BAL_NBUMSQL, icf->m_cMDBTable, icf->m_cTimeName, m_sDateTime);     
                m_poClearMDB->Prepare(sSql);
                m_poClearMDB->Execute();
                while(!m_poClearMDB->FetchNext()){
                    m_poClearMDB->getColumn(1, &inum);
                }
                m_poClearMDB->Commit();
                m_poClearMDB->Close();
                memset(sSql, 0x00, 256);
                snprintf(sSql, sizeof(sSql), MDB_ABM_BAL_DEL_SQL, icf->m_cMDBTable, icf->m_cTimeName, m_sDateTime);
            } else {
                //log::log();
                return -1;
            }
            int iTemp=(inum/MDB_ABM_DEL_MAX);
            for (int i=0; i < (iTemp+1); ++i){
                m_poClearMDB->Prepare(sSql);    
                m_poClearMDB->Execute();
                m_poClearMDB->Commit();
                m_poClearMDB->Close();
            }
            //释放空间
            RelTTCursor(m_poClearMDB);
            if(m_poTTconn) {
                m_poTTconn->disconnect();
                delete m_poTTconn;
                m_poTTconn = NULL;
            }
            
        }    
    }
    catch(TTStatus st){
        
    }
    return 0;        
}

int ABMClearProduct::executeArchive(ABMException &oExp,char *sDbName)
{
    snprintf(m_sTTStr,sizeof(m_sTTStr),"dsn=%s",sDbName);    
    connectTT(oExp);
    m_poClearMDB->Prepare(m_sArchiveSQL.c_str());
    m_poClearMDB->Execute();
    int i;
    while (!m_poClearMDB->FetchNext()) 
    {	
        string sDataTemp="";
	    i=1;
	    char cValueTemp[30];
	    memset(cValueTemp, 0x00, sizeof(cValueTemp));
		ABMArchiveData poArchiveData;
		while(i < m_iFieldNum+1) {

		    m_poClearMDB->getColumn(i++, cValueTemp);
			sDataTemp +=cValueTemp;
			if(i < m_iFieldNum+1){
			    sDataTemp +="," ;
			    memset(cValueTemp, 0x00, sizeof(cValueTemp));
			}
		}
		sprintf(poArchiveData.m_cVectorData,"%s",sDataTemp.c_str());
		m_oABMArchiveConf.push_back(poArchiveData);
		if(m_oABMArchiveConf.size()>999) {
		    
	        for (i=0; i<m_oABMArchiveConf.size(); ++i)
	        {
		        fprintf(m_poArchiveData,"%s \n",m_oABMArchiveConf[i].m_cVectorData);
	        }
            m_oABMArchiveConf.clear();     
	    }			
    }
    if(m_oABMArchiveConf.size() > 0) {
	    for (i=0; i<m_oABMArchiveConf.size(); ++i)
	    {
	        fprintf(m_poArchiveData,"%s \n",m_oABMArchiveConf[i].m_cVectorData);
	    }
        m_oABMArchiveConf.clear();        
    }
    RelTTCursor(m_poClearMDB);
    if(m_poTTconn) {
        m_poTTconn->disconnect();
        delete m_poTTconn;
        m_poTTconn = NULL;
    }
    return 0;
    
}
int ABMClearProduct::executeArchiveSQL(ABMException &oExp, char *sDbTable, char *cDbName)
{    
    char sTableTempName[1024+1];
    memset(sTableTempName, 0x00, sizeof(sTableTempName));
    strncpy(sTableTempName,sDbTable,sizeof(sTableTempName)-1);
    for(int ii=0; ii<(sizeof(sTableTempName)-1);++ii)
        sTableTempName[ii]=toupper(sTableTempName[ii]);
    if (m_poTableCfg.read(oExp, m_sFileName, sTableTempName , '|') == -1) {
        ADD_EXCEPT1(oExp, "ReadCfg::read(%s, \"ABMClearTT\", '|') failed",m_sFileName);
        return -1;     
    }
    m_iLineNum = m_poTableCfg.getRows();
   
    int iColFlag = 0;
    //char cColName[30];
    char sValue[64+1];
    m_iFieldNum = m_iLineNum;
    //int iLines;
    //int iLineTemp=0;
    
    //for(iLines=0; iLines < iSize; ++iLines)
    m_sArchiveSQL += " SELECT ";
    while(m_iLineNum > 0)
    {
        //iLineTemp = iLines+1;
        if(!m_poTableCfg.getValue(oExp, iColFlag, "COLFLAG",m_iLineNum)) {
            ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed", "COLFLAG", m_iLineNum);
            return -1;
        }
        memset(sValue, 0x00, sizeof(sValue));
        if(!m_poTableCfg.getValue(oExp, sValue, "COLNAME",m_iLineNum)) {
            ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed", "COLNAME", m_iLineNum);
            return -1;
        }
        if(iColFlag ==1) {
            m_sArchiveSQL +="to_char(";
            m_sArchiveSQL +=sValue;
            m_sArchiveSQL +=")";        
        } else  if(iColFlag ==3) {
            m_sArchiveSQL +="to_char(";
            m_sArchiveSQL +=sValue;
            m_sArchiveSQL +=")";
        } else {
            m_sArchiveSQL +=sValue;
        }
        if(m_iLineNum > 1){
            m_sArchiveSQL +=",";    
        }
        m_iLineNum--;                
    } 
    m_sArchiveSQL +=" from ";
	m_sArchiveSQL +=sDbTable;
	m_sArchiveSQL +=" where "; 
    if (m_poTableCfg.read(oExp, m_sFileName, "ABMClearTT" , '|') == -1) {
        ADD_EXCEPT1(oExp, "ReadCfg::read(%s, \"ABMcDbName\", '|') failed",m_sFileName);
        return -1;     
    }
    m_iLineNum = m_poTableCfg.getRows();	
    while(m_iLineNum >0)
    {
        memset(sValue, 0x00, sizeof(sValue));
        if(!m_poTableCfg.getValue(oExp, sValue, "TTtable_name", m_iLineNum)) {
            ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed", "TTtable_name", m_iLineNum);
            return -1;
        }
        if(0 != strcasecmp(sValue , sDbTable)) {
            --m_iLineNum;
            continue;
        }        
        memset(sValue, 0x00, sizeof(sValue));
        if(!m_poTableCfg.getValue(oExp, sValue, "TT_name",m_iLineNum)) {
            ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed", "DBtable_name", m_iLineNum);
            return -1;
        }
        if(0 != strcasecmp(sValue , cDbName)) {
            --m_iLineNum;
            continue;
        }
        if(!m_poTableCfg.getValue(oExp, iColFlag, "table_type",m_iLineNum)) {
            ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed", "table_type", m_iLineNum);
            return -1;
        }        
        memset(sValue, 0x00, sizeof(sValue));
        if(iColFlag == 0) {
            if(!m_poTableCfg.getValue(oExp, sValue, "time_param",m_iLineNum)) {
                ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed", "time_param", m_iLineNum);
                return -1;
            } 
            m_sArchiveSQL +=sValue;
            m_sArchiveSQL +=" < ";
            m_sArchiveSQL +=m_sDateTime;
            break;           
        }
        if(iColFlag == 1) {
            if(!m_poTableCfg.getValue(oExp, sValue, "time_param",m_iLineNum)) {
                ADD_EXCEPT2(oExp, "ReadCfg::getValue(\"%s\", %d) failed", "time_param", m_iLineNum);
                return -1;
            } 
            m_sArchiveSQL += sValue;
            m_sArchiveSQL += " <to_date(' ";
            m_sArchiveSQL += m_sDateTime; 
            m_sArchiveSQL += "', 'YYYYMMDDHH24MISS') " ;
            break;          
        }      
    }  
    
    
    return 0;            
}




