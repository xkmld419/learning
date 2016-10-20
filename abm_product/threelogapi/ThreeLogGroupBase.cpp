#include "ThreeLogGroupBase.h"

bool ThreeLogGroupBase::m_bAttached = false;
ThreeLogGoupData * ThreeLogGroupBase::m_poLogGroup = 0;
SHMIntHashIndex_A * ThreeLogGroupBase::m_poLogGroupIndex = 0;
SHMData<ThreeLogGoupData> * ThreeLogGroupBase::m_poLogGroupData = 0;

LogInfoData * ThreeLogGroupBase::m_poInfoGroup = 0;

SHMData_A<LogInfoData> * ThreeLogGroupBase::m_poInfoDataAll[GROUP_LOG_NUMS+1]={0};
SHMIntHashIndex_A * ThreeLogGroupBase::m_poInfoIndexAll[GROUP_LOG_NUMS+1]={0};

CSemaphore * ThreeLogGroupBase::m_poDataLock = 0;
CSemaphore * ThreeLogGroupBase::m_poIndexLock = 0;
CSemaphore * ThreeLogGroupBase::m_poInfoDataLock = 0;
CSemaphore * ThreeLogGroupBase::m_poLogGroupMgrLock = 0;

SHMData_A<LogInfoData> * ThreeLogGroupBase::m_poInfoData=0;
SHMIntHashIndex_A * ThreeLogGroupBase::m_poInfoIndex=0;

long ThreeLogGroupBase::SHM_LOG_GROUP_DATA         =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INDEX        =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_LOCK         =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INDEX_LOCK   =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INFO_LOCK    =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_MGR_LOCK     =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INFO_DATA_1  =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INFO_DATA_2  =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INFO_DATA_3  =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INFO_DATA_4  =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INFO_DATA_5  =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INFO_DATA_6  =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INFO_INDEX_1 =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INFO_INDEX_2 =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INFO_INDEX_3 =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INFO_INDEX_4 =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INFO_INDEX_5 =0;
long ThreeLogGroupBase::SHM_LOG_GROUP_INFO_INDEX_6 =0; 
bool ThreeLogGroupBase::SHM_KEY_IFLOAD = false ;
	
void ThreeLogGroupBase::initBase()
{
    if(m_bAttached)
        return;
        
    if(!getIpcCfg()) 
    	 return ;
    	         

    ATTACH_DATA(m_poLogGroupData, ThreeLogGoupData, SHM_LOG_GROUP_DATA);
    ATTACH_INT_INDEX_A(m_poLogGroupIndex, SHM_LOG_GROUP_INDEX);

    ATTACH_DATA_A(m_poInfoDataAll[1], LogInfoData, SHM_LOG_GROUP_INFO_DATA_1);
    ATTACH_DATA_A(m_poInfoDataAll[2], LogInfoData, SHM_LOG_GROUP_INFO_DATA_2);
    ATTACH_DATA_A(m_poInfoDataAll[3], LogInfoData, SHM_LOG_GROUP_INFO_DATA_3);
    //ATTACH_DATA_A(m_poInfoDataAll[4], LogInfoData, SHM_LOG_GROUP_INFO_DATA_4);
    //ATTACH_DATA_A(m_poInfoDataAll[5], LogInfoData, SHM_LOG_GROUP_INFO_DATA_5);
    //ATTACH_DATA_A(m_poInfoDataAll[6], LogInfoData, SHM_LOG_GROUP_INFO_DATA_6);       

    ATTACH_INT_INDEX_A(m_poInfoIndexAll[1], SHM_LOG_GROUP_INFO_INDEX_1);
    ATTACH_INT_INDEX_A(m_poInfoIndexAll[2], SHM_LOG_GROUP_INFO_INDEX_2);
    ATTACH_INT_INDEX_A(m_poInfoIndexAll[3], SHM_LOG_GROUP_INFO_INDEX_3);
    //ATTACH_INT_INDEX_A(m_poInfoIndexAll[4], SHM_LOG_GROUP_INFO_INDEX_4);
    //ATTACH_INT_INDEX_A(m_poInfoIndexAll[5], SHM_LOG_GROUP_INFO_INDEX_5);
    //ATTACH_INT_INDEX_A(m_poInfoIndexAll[6], SHM_LOG_GROUP_INFO_INDEX_6);

    m_bAttached = true;
    bindData();
    
    char sTemp[32];
    memset(sTemp,0,sizeof(sTemp));
    if(!m_poDataLock)
    {
        m_poDataLock = new CSemaphore ();
        sprintf (sTemp, "%d", SHM_LOG_GROUP_LOCK);
        m_poDataLock->getSem (sTemp, 1, 1);
    }
    
    memset(sTemp,0,sizeof(sTemp));
    if(!m_poInfoDataLock)
    {
        m_poInfoDataLock = new CSemaphore();
        sprintf (sTemp, "%d", SHM_LOG_GROUP_INFO_LOCK);
        m_poInfoDataLock->getSem (sTemp, 1, 1);
    }

    memset(sTemp,0,sizeof(sTemp));
    if(!m_poIndexLock)
    {
        m_poIndexLock = new CSemaphore();
        sprintf (sTemp, "%d", SHM_LOG_GROUP_INDEX_LOCK);
        m_poIndexLock->getSem(sTemp, 1, 1);
    }

	 memset(sTemp,0,sizeof(sTemp));
	 if(!m_poLogGroupMgrLock)
	 {
        m_poLogGroupMgrLock = new CSemaphore();
        sprintf (sTemp, "%d", SHM_LOG_GROUP_MGR_LOCK);
        m_poLogGroupMgrLock->getSem(sTemp, 1, 1);	 
	 }
}
	
ThreeLogGroupBase::ThreeLogGroupBase()
{
	this->initBase();
}

ThreeLogGroupBase::~ThreeLogGroupBase()
{
    if(m_poDataLock)
    {
        delete m_poDataLock;
        m_poDataLock=0;
    }
    if(m_poInfoDataLock)
    {
        delete m_poInfoDataLock;
        m_poInfoDataLock = 0;
    }
    if(m_poIndexLock)
    {
        delete  m_poIndexLock;
        m_poIndexLock =0;
    }
	if(m_poLogGroupMgrLock)
	{
		delete m_poLogGroupMgrLock;
		m_poLogGroupMgrLock = 0;
	}

	if(m_poLogGroupData) 
	{
		m_poLogGroupData->close();
	}

	//m_poInfoDataAll[1]->close();
   //m_poInfoDataAll[2]->close();
   //m_poInfoDataAll[3]->close();
   // m_poInfoDataAll[4]->close();
   // m_poInfoDataAll[5]->close();
   // m_poInfoDataAll[6]->close();

	//m_poInfoIndexAll[1]->close();
	//m_poInfoIndexAll[2]->close();
	//m_poInfoIndexAll[3]->close();
	//m_poInfoIndexAll[4]->close();
	//m_poInfoIndexAll[5]->close();
	//m_poInfoIndexAll[6]->close();

	for(int i = 1; i <= GROUP_LOG_NUMS; ++i)
	{
		if(m_poInfoDataAll[i])
		{
			m_poInfoDataAll[i]->close();
		}

		if(m_poInfoIndexAll[i])
		{
			m_poInfoIndexAll[i]->close();
		}
	}

	if(m_poLogGroupIndex)
	{
		m_poLogGroupIndex->close();
	}
	m_bAttached=false;
}

void ThreeLogGroupBase::bindData()
{
    m_poLogGroup = (ThreeLogGoupData *)(*m_poLogGroupData);
}

void ThreeLogGroupBase::bindGroupData(int GroupId)
{
    if(GroupId<0 || GroupId>GROUP_LOG_NUMS)
        m_poInfoGroup=0;
    else
        m_poInfoGroup = (LogInfoData *)(*(m_poInfoDataAll[GroupId]));
}

void ThreeLogGroupBase::freeAll()
{
    if(m_poLogGroupData) 
    {
        delete m_poLogGroupData;
        m_poLogGroupData = 0;
    }
    if(m_poLogGroupIndex) 
    {
        delete m_poLogGroupIndex;
        m_poLogGroupIndex = 0;
    }
    for(int i=1;i<=GROUP_LOG_NUMS;i++)
    {
        if(m_poInfoDataAll[i])
        {
            delete m_poInfoDataAll[i];
            m_poInfoDataAll[i]=0;
        }
    }
    for(int i=1;i<=GROUP_LOG_NUMS;i++)
    {
        if(m_poInfoIndexAll[i])
        {
            delete m_poInfoIndexAll[i];
            m_poInfoIndexAll[i]=0;
        }
    }
    if(m_poInfoIndex)
    {
        delete m_poInfoIndex;
        m_poInfoIndex =0;
    }
    if(m_poInfoData)
    {
        delete m_poInfoData;
        m_poInfoData=0;
    }
}


bool ThreeLogGroupBase::getIpcCfg()
{            
	  if(SHM_KEY_IFLOAD ) return true;
	  	
    char ConfigIni[256] = {0};
    
    char *p = NULL;
    if ((p=getenv (ENV_HOME)) == NULL){
    	printf("error: fail to get envionment variable->%s \n",ENV_HOME); 
    	return false;
    }
    if (p[strlen(p)-1] == '/') {
    	sprintf(ConfigIni,"%setc/%s",p,CONFIG_FILE);
    } else {
    	sprintf(ConfigIni,"%s/etc/%s",p,CONFIG_FILE);
    }
    ReadIni* pReadFile = new ReadIni();
    if(!pReadFile)
    {
    	printf("error: new ReadIni() \n");
    	return false;
    }
   SHM_LOG_GROUP_DATA   = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupdata.ipc_key_value",0);
   SHM_LOG_GROUP_INDEX  = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupindex.ipc_key_value",0); 
   SHM_LOG_GROUP_LOCK   = pReadFile->readIniLong(ConfigIni,"MEMORY","loggrouplock.ipc_key_value",0); 
   SHM_LOG_GROUP_INDEX_LOCK = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupindexlock.ipc_key_value",0);  
   SHM_LOG_GROUP_INFO_LOCK = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupinfolock.ipc_key_value",0);
   SHM_LOG_GROUP_MGR_LOCK = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupmgrlock.ipc_key_value",0);
                                
   SHM_LOG_GROUP_INFO_DATA_1  = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupdata_one.ipc_key_value",0); 
   SHM_LOG_GROUP_INFO_DATA_2  = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupdata_two.ipc_key_value",0);  
   SHM_LOG_GROUP_INFO_DATA_3  = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupdata_thr.ipc_key_value",0);  
   //SHM_LOG_GROUP_INFO_DATA_4  = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupdata_for.ipc_key_value",0);  
   //SHM_LOG_GROUP_INFO_DATA_5  = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupdata_fiv.ipc_key_value",0);  
   //SHM_LOG_GROUP_INFO_DATA_6  = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupdata_six.ipc_key_value",0);  
                                
   SHM_LOG_GROUP_INFO_INDEX_1 = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupindex_one.ipc_key_value",0);  
   SHM_LOG_GROUP_INFO_INDEX_2 = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupindex_two.ipc_key_value",0);   
   SHM_LOG_GROUP_INFO_INDEX_3 = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupindex_thr.ipc_key_value",0);   
   //SHM_LOG_GROUP_INFO_INDEX_4 = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupindex_for.ipc_key_value",0);   
   //SHM_LOG_GROUP_INFO_INDEX_5 = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupindex_fiv.ipc_key_value",0);   
   //SHM_LOG_GROUP_INFO_INDEX_6 = pReadFile->readIniLong(ConfigIni,"MEMORY","loggroupindex_six.ipc_key_value",0);  
    delete pReadFile;
    
    if(SHM_LOG_GROUP_DATA<=0 || SHM_LOG_GROUP_INDEX<=0 ||SHM_LOG_GROUP_INFO_LOCK <=0 ||
    	  SHM_LOG_GROUP_INFO_DATA_1<=0 || SHM_LOG_GROUP_INFO_DATA_2<=0 || 
        SHM_LOG_GROUP_INFO_DATA_3<=0 || 
        //SHM_LOG_GROUP_INFO_DATA_4<=0 || 
        //SHM_LOG_GROUP_INFO_DATA_5<=0 || 
        //SHM_LOG_GROUP_INFO_DATA_6<=0 || 
        SHM_LOG_GROUP_INFO_INDEX_1<=0 ||
        SHM_LOG_GROUP_INFO_INDEX_2<=0 ||
        SHM_LOG_GROUP_INFO_INDEX_3<=0 
        //SHM_LOG_GROUP_INFO_INDEX_4<=0 ||
        //SHM_LOG_GROUP_INFO_INDEX_5<=0 ||
        //SHM_LOG_GROUP_INFO_INDEX_6<=0 
        )
        
        return false;
        
    SHM_KEY_IFLOAD = true ;   
    return true;
}


long ThreeLogGroupBase::getLogGroupDataKey(int iGroupId)
{
	if(iGroupId == 1) {
		return SHM_LOG_GROUP_INFO_DATA_1;
	} else if(iGroupId == 2) {
		return SHM_LOG_GROUP_INFO_DATA_2;
	} else if(iGroupId == 3) {
		return SHM_LOG_GROUP_INFO_DATA_3;
	}

	return -1;
}
long ThreeLogGroupBase::getLogGroupIndexKey(int iGroupId)
{
	if(iGroupId == 1) {
		return SHM_LOG_GROUP_INFO_INDEX_1;
	} else if(iGroupId == 2) {
		return SHM_LOG_GROUP_INFO_INDEX_2;
	} else if(iGroupId == 3) {
		return SHM_LOG_GROUP_INFO_INDEX_3;
	}

	return -1;
}

