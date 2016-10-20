#include "ThreeLogGroupMgr.h"
#include "Log.h"

ThreeLogGroupMgr::ThreeLogGroupMgr():ThreeLogGroupBase()
{
	init();
	
	if(m_iMaxGroup == 0) 
	{
		m_iMaxGroup = 3;
	}
}

ThreeLogGroupMgr::~ThreeLogGroupMgr()
{
}

bool ThreeLogGroupMgr::create()
{		
    bool bNew = false;
    if(!m_poLogGroupData->exist())
    {
        m_poLogGroupData->create (SHM_LOG_GROUP_COUNT);
        bNew = true;
    }
    if(!m_poLogGroupIndex->exist())
    {
        m_poLogGroupIndex->create (SHM_LOG_GROUP_COUNT);
        bNew = true;
    }
    for(int i=1;i<=m_iMaxGroup;i++)
    {
        if(!m_poInfoDataAll[i]->exist())
            m_poInfoDataAll[i]->create(m_lInfoDataCount);
    }
    for(int i=1;i<=m_iMaxGroup;i++)
    {
        if(!m_poInfoIndexAll[i]->exist())
            m_poInfoIndexAll[i]->create(m_lInfoDataCount);
    }

    bindData();
    m_bAttached = true;
    return true;
}

bool ThreeLogGroupMgr::exist()
{
    if(!m_poLogGroupData->exist())
        return false;
    if(!m_poLogGroupIndex->exist())
        return false;
    for(int i=1;i<=m_iMaxGroup;i++)
    {
        if(!m_poInfoDataAll[i]->exist())
            return false;
    }
    for(int i=1;i<=m_iMaxGroup;i++)
    {
        if(!m_poInfoIndexAll[i]->exist())
            return false;
    }
    return true;
}

bool ThreeLogGroupMgr::remove()
{
    if(m_poLogGroupData->exist())
        m_poLogGroupData->remove();

    if(m_poLogGroupIndex->exist()) 
        m_poLogGroupIndex->remove();

    for(int i=1;i<=m_iMaxGroup;i++)
    {
        if(m_poInfoDataAll[i]->exist())
            m_poInfoDataAll[i]->remove();
    }
    
    for(int i=1;i<=m_iMaxGroup;i++)
    {
        if(m_poInfoIndexAll[i]->exist())
            m_poInfoIndexAll[i]->remove();
    }
    m_bAttached = false;

    freeAll();

    return true;
}

bool ThreeLogGroupMgr::setGroupNum(int iNum)
{
    this->m_iMaxGroup = iNum;
    return true;
}

bool ThreeLogGroupMgr::setGroupInfoDataCount(long lCount)
{
    this->m_lInfoDataCount = lCount;
    return true;
}

int ThreeLogGroupMgr::GetShmDataTotalCount(int iGroupId)
{
    return m_poInfoDataAll[iGroupId]->getTotal();
}

int ThreeLogGroupMgr::GetShmIndexTotalCount(int iGroupId)
{
    return m_poInfoIndexAll[iGroupId]->getTotal();
}

int ThreeLogGroupMgr::GetShmDataCount(int iGroupId)
{
    return m_poInfoDataAll[iGroupId]->getCount();
}

int ThreeLogGroupMgr::GetShmIndexCount(int iGroupId)
{
    return m_poInfoIndexAll[iGroupId]->getCount();
}

int ThreeLogGroupMgr::GetShmIndexDataCount(int iGroupId,int iIndex)
{
    int iCnt = 0;
    unsigned int m=0;
    LogInfoData * p = (LogInfoData *)(*(m_poInfoDataAll[iGroupId]));
    if(m_poInfoIndexAll[iGroupId]->get(iIndex,&m))
    {
       while(m)
        {
            iCnt++;
            m = p[m].m_iNextOffset;
       }
    }
    return iCnt;
}

int ThreeLogGroupMgr::GetShmIndexDataTotal(int iGroupId)
{
    int cnt = GetShmIndexTotalCount(iGroupId);
    int itotal = 0;
    for(int i=1;i<cnt;i++)
    {
        itotal+=GetShmIndexDataCount(iGroupId,i);
    }
    return itotal;
}

void ThreeLogGroupMgr::showShmInfo()
{
	if(m_bAttached == false)
	{
		initBase();
	}
	
 	printf("*********************************************************\n");
	printf("日志组共享内存信息:\n");
	for(int i=1;i<=m_iMaxGroup;i++)
	{
		printf("GROUP_ID:%d\n",i);
		printf("\n");
		int iDataCnt = GetShmDataCount(i);
		int iDataTotal = GetShmDataTotalCount(i);
		printf("SHM_DATA_USED:%d条\n",iDataCnt);
		printf("SHM_DATA_TOTAL:%d条\n",iDataTotal);
		printf("\n");
		int iIndexCnt = GetShmIndexCount(i);
		int iIndexTotal = GetShmIndexTotalCount(i);
		printf("SHM_INDEX_USED:%d条\n",iIndexCnt);
		printf("SHM_INDEX_TOTAL:%d条\n",iIndexTotal);
		printf("\n");

		int iIndexDataTotal = GetShmIndexDataTotal(i);
		printf("SHM_INDEX_DATA_TOTAL:%d条\n",iIndexDataTotal);

		for(int j=1;j<iIndexTotal;j++)
		{
			int iIndexDataCount = GetShmIndexDataCount(i,j);
			if(iIndexDataCount>0)
				printf("INDEX(%d):COUNT:%d条\n",j,iIndexDataCount);
			}
			printf("-------------------\n");
		}
	printf("*********************************************************\n");
}

int ThreeLogGroupMgr::getLogGroupNum()
{
	return m_iMaxGroup;
}

SHMData_A<LogInfoData> * ThreeLogGroupMgr::getLogGroupDataAddr(int iGroupId)
{
	if(iGroupId <= 0 ||iGroupId > m_iMaxGroup)
	{
		Log::log(0,"日志组ID取值为1至%d",m_iMaxGroup);
		return NULL;
	}

	if(m_bAttached == false)
	{
		initBase();
	}
	
	return m_poInfoDataAll[iGroupId];
}

SHMIntHashIndex_A * ThreeLogGroupMgr::getLogGroupIndexAddr (int iGroupId)

{
	if(iGroupId <= 0 ||iGroupId > m_iMaxGroup)
	{
		Log::log(0,"日志组ID取值为1至%d",m_iMaxGroup);
		return NULL;
	}	

	if(m_bAttached == false)
	{
		initBase();
	}
	
   return m_poInfoIndexAll[iGroupId];
}	
bool ThreeLogGroupMgr::init()
{
	  ReadIni * m_RD = new ReadIni();
    if(!m_RD)
    {
        Log::log(0,"获取核心参数失败");
        return false;
    }
    char *p;
    char sHome[256];
    char m_sParamFile[256] ;
    memset(sHome,'\0',sizeof(sHome));
    if ((p=getenv (ENV_HOME)) == NULL)
        sprintf (sHome, "/opt/opthb/etc");
    else
        sprintf (sHome, "%s/etc", p);
    if(sHome[strlen(sHome)-1] == '/') 
        sprintf (m_sParamFile, "%sabmconfig", sHome);
    else
        sprintf (m_sParamFile, "%s/abmconfig", sHome);
	   
    char sDataSize[100],temp[100];
    memset(sDataSize,0,sizeof(sDataSize));
    memset(temp,0,sizeof(temp));	            
	  m_RD->readIniString(m_sParamFile,"LOG","group_num",temp,NULL);
	  if(temp==NULL) return false ;
    this->m_iMaxGroup = atoi(temp);
    m_RD->readIniString(m_sParamFile,"MEMORY","log_data_size",sDataSize,NULL);
	  if(temp==sDataSize) return false ;    
    this->m_lInfoDataCount = atol(sDataSize);
    
    delete m_RD ;
    return true;
}
