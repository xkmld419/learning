#include "CSemaphore.h"
#include "ThreeLogGroup.h"

ThreeLogGroup LOG;
struct SEMINFO
{
	long m_lIpcKey;
	char m_sTableName[32];	
	string m_strIpcName;	
	int m_iState ; //0 可用 ;1 上锁	
}
;

bool getIpcKey(vector<SEMINFO> m_vSEMInfo)
{
	 char sHome[500],sFile[500];
   char temp[20],sKeyName[200];	    
   memset(sHome,'\0',sizeof(sHome));
   memset(sFile,'\0',sizeof(sFile));   
   SEMINFO oSEMInfo;
   m_vSEMInfo.clear();
   
   char *p=0;
   if ((p=getenv ("BILLDIR")) == NULL)
         sprintf (sHome, "/opt/opthb/etc");
     else
         sprintf (sHome, "%s/etc", p);
     if(sHome[strlen(sHome)-1] == '/') 
         sprintf (sFile, "%abmconfig", sHome);
     else
         sprintf (sFile, "%s/abmconfig", sHome);
 
    memset(temp,'\0',sizeof(temp));
    memset(sKeyName,'\0',sizeof(sKeyName));
    ReadIni *m_RD = new ReadIni();
    if(m_RD==NULL)   
    {
        Log::log(0,"获取核心参数失败");
        return -false;
    }  
    
    //信号量
    
    oSEMInfo.m_lIpcKey = m_RD->readIniLong(sFile,"MEMORY","loggroupdata_thr.ipc_key_value",0);
    m_RD->readIniString(sFile,"MEMORY","param.sem_key_name",sKeyName,NULL);
    oSEMInfo.m_strIpcName = sKeyName;
    strcpy(oSEMInfo.m_sTableName,"system");
    m_vSEMInfo.push_back(oSEMInfo);    
    
    oSEMInfo.m_lIpcKey = m_RD->readIniLong(sFile,"MEMORY","loggroupinfolock.ipc_key_value",0);
    m_RD->readIniString(sFile,"MEMORY","loggroupinfolock.ipc_key_name",sKeyName,NULL);
    oSEMInfo.m_strIpcName = sKeyName;
    strcpy(oSEMInfo.m_sTableName,"log");    
    m_vSEMInfo.push_back(oSEMInfo);    
    
    delete m_RD;    
    return true;
}

bool creatSem(vector<SEMINFO> m_vSEMInfo)
{
	
	 CSemaphore * pSemLock =0;
	 char sTemp[32]={0};
	 int iSize=m_vSEMInfo.size();
	 for(int i=0;i<iSize;i++)
	 {

	 	 	pSemLock = new CSemaphore();   
	 	 	if(!pSemLock) return false ;
	 	 	memset(sTemp,0,sizeof(sTemp));
      sprintf (sTemp, "%d", m_vSEMInfo[i].m_lIpcKey);
      if(!pSemLock->getSem (sTemp, 1, 1))
      	LOG.log(MBC_SEMP_CREATE_FAIL,LOG_LEVEL_FATAL,
                LOG_TYPE_SYSTEM,"创建信号量失败，请检查key值[%s]!",sTemp);
       delete pSemLock ;
       pSemLock =0;
     }
}

int main(int argc,char **argv)
{
 try{
	vector<SEMINFO> m_vSEMInfo;
	if(argc==1) getIpcKey(m_vSEMInfo);
	else{
		SEMINFO oSEMInfo;
		for(int i=1 ;i<argc;i++)
		{
			long key=atol(argv[1]);
			oSEMInfo.m_lIpcKey=key;
			m_vSEMInfo.push_back(oSEMInfo);
		}
	}
	 creatSem(m_vSEMInfo);
 }catch(...){
 	
 }
	return 0 ;
}

