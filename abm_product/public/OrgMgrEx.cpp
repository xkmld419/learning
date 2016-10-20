/*VER: 2*/ 
#include "OrgMgrEx.h"

#include "Environment.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif
//##ModelId=4248F8730331

//##ModelId=42732EB50024

//##ModelId=42732E62022D

OrgMgrEx* OrgMgrEx::m_gOrgMgrEx = 0;
#ifdef PRIVATE_MEMORY
OrgMgrEx* OrgMgrEx::GetInstance(void)
{
    if(0==OrgMgrEx::m_gOrgMgrEx)
    {
        OrgMgrEx::m_gOrgMgrEx = new OrgMgrEx();
        OrgMgrEx::m_gOrgMgrEx->Init();
    }
    return OrgMgrEx::m_gOrgMgrEx;
}
#else
OrgMgrEx* OrgMgrEx::GetInstance(void)
{
    if(0==OrgMgrEx::m_gOrgMgrEx)
    {
        OrgMgrEx::m_gOrgMgrEx = new OrgMgrEx();
    }
    return OrgMgrEx::m_gOrgMgrEx;
}
#endif

#ifdef PRIVATE_MEMORY
bool OrgMgrEx::reload(void)
{
    if( OrgMgrEx::m_gOrgMgrEx){
        
        OrgMgrEx::m_gOrgMgrEx->ExitInstance();
        OrgMgrEx::m_gOrgMgrEx->Init();

    }
    
    return true;
}
#else
bool OrgMgrEx::reload(void)
{
    return true;
}
#endif


OrgMgrEx::OrgMgrEx(void)
{
    m_pOrgMgrEx_Bill = 0;
    m_poOrgExIndex =0;
    m_poOrgExList=0;
}

bool OrgMgrEx::Init(void)
{

    char sTableName[1024]={0};
    sprintf(sTableName,"SELECT ORG_ID, nvl(PARENT_ORG_ID, -2), nvl(ORG_LEVEL_ID, -2), "
       " NVL(ORG_AREA_CODE,'') FROM  ORG ");
    OrgMgrEx* pOrgMgrEx = new OrgMgrEx();
    pOrgMgrEx->load(sTableName);
    m_mapType2OrgEx[BILL_ORG] = pOrgMgrEx;
    m_pOrgMgrEx_Bill = pOrgMgrEx;
    pOrgMgrEx = new OrgMgrEx();

    sprintf(sTableName,"select REGION_ID,NVL(PARENT_REGION_ID,-2),"
        "NVL(REGION_LEVEL,-2),NVL(ORG_AREA_CODE,'') from STAT_REGION ");
    pOrgMgrEx->load(sTableName);
    m_mapType2OrgEx[STAT_ORG] = pOrgMgrEx;


    DEFINE_QUERY(qry);
    char sSql[1024]={0};
    sprintf(sSql,"SELECT DISTINCT(REGION_TYPE_ID ) from B_SELF_DEFINE_REGION ");
    qry.setSQL (sSql);
    qry.open ();

    while(qry.next())
    {
        int iTypeID= qry.field(0).asInteger();
        pOrgMgrEx = new OrgMgrEx();
        sprintf(sTableName,"select REGION_ID,NVL(PARENT_REGION_ID,-2),"
            "NVL(REGION_LEVEL,-2),NVL(REGION_AREA_CODE,'') from "
            "B_SELF_DEFINE_REGION WHERE REGION_TYPE_ID =%d",iTypeID);
        pOrgMgrEx->load(sTableName);
        m_mapType2OrgEx[iTypeID]=pOrgMgrEx;
    }
    qry.commit();
    qry.close();
    m_ItEnd = m_mapType2OrgEx.end();
    return true;
}

OrgMgrEx::~OrgMgrEx(void)
{
}

#ifdef PRIVATE_MEMORY
bool OrgMgrEx::getAreaCode(int iOrgID,char* pAreaCode,int nSize,int iLevelID,int iOrgType)
{
    if((0 == pAreaCode)||(nSize<=0))
        return false;
    OrgMgrEx* pOrgMgrEx=0;
    if(iOrgType ==BILL_ORG)
    {
        pOrgMgrEx =m_pOrgMgrEx_Bill;
    }else
    {
        map<int,OrgMgrEx*>::iterator it = 
            m_mapType2OrgEx.find(iOrgType);
        if( it !=m_ItEnd)
            pOrgMgrEx = it->second;
        return false;
    }
    OrgEx * pOrg = 0;
    if(!pOrgMgrEx->m_poOrgExIndex->get(iOrgID,&pOrg))
        return false;
    if((iLevelID ==0)||pOrg->m_iLevel ==iLevelID)
    {
        strncpy(pAreaCode,pOrg->sAreaCode,strlen(pOrg->sAreaCode)>nSize?nSize:strlen(pOrg->sAreaCode));
        pAreaCode[strlen(pOrg->sAreaCode)>nSize?nSize:strlen(pOrg->sAreaCode)]=0;
        return true;
    }else
    {
        iOrgID = pOrgMgrEx->getLevelParentID(iOrgID,iLevelID,iOrgType);
        if(-1 != iOrgID)
        {
            if(!pOrgMgrEx->m_poOrgExIndex->get(iOrgID,&pOrg))
                return false;
            strncpy(pAreaCode,pOrg->sAreaCode,strlen(pOrg->sAreaCode)>nSize?nSize:strlen(pOrg->sAreaCode));
            pAreaCode[strlen(pOrg->sAreaCode)>nSize?nSize:strlen(pOrg->sAreaCode)]=0;
            return true;
        }else
            return false;
    }
}
#else
bool OrgMgrEx::getAreaCode(int iOrgID,char* pAreaCode,int nSize,int iLevelID,int iOrgType)
{
    if((0 == pAreaCode)||(nSize<=0))
        return false;

    OrgEx *pOrgEx= 0;
    unsigned int iIdx = 0;
    char sKey[32];
    memset(sKey, '\0', 32);
    sprintf(sKey,"%d_%d", iOrgType, iOrgID);

    if(!G_PPARAMINFO->m_poOrgExIndex->get(sKey, &iIdx))
        return false;
    else
    {
        pOrgEx = (OrgEx *)G_PPARAMINFO->m_poOrgExList + iIdx;
        if(iLevelID == 0 || pOrgEx->m_iLevel == iLevelID)
        {
            strncpy(pAreaCode,pOrgEx->sAreaCode,strlen(pOrgEx->sAreaCode)>nSize?nSize:strlen(pOrgEx->sAreaCode));
            pAreaCode[strlen(pOrgEx->sAreaCode)>nSize?nSize:strlen(pOrgEx->sAreaCode)]=0;
            return true;
        }
        else
        {
            if(getLevelParentID(iOrgID, iLevelID) != -1)
            {
                strncpy(pAreaCode,pOrgEx->sAreaCode,strlen(pOrgEx->sAreaCode)>nSize?nSize:strlen(pOrgEx->sAreaCode));
                pAreaCode[strlen(pOrgEx->sAreaCode)>nSize?nSize:strlen(pOrgEx->sAreaCode)]=0;
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}
#endif

void OrgMgrEx::load(char* pOrgTable)
{
    int count, i;

    unload ();

    DEFINE_QUERY(qry);
    char sSql[1024]={0};

    sprintf(sSql,"select count(*) from ( %s ) ",pOrgTable);
    qry.setSQL (sSql);
    qry.open ();

    qry.next ();
    count = qry.field(0).asInteger () + 16;
    qry.close ();

    OrgMgrEx::m_poOrgExList = new OrgEx[count];
    m_poOrgExIndex = new HashList<OrgEx *> ((count>>1) | 1);

    if (!m_poOrgExList || !m_poOrgExIndex)
        THROW(MBC_OrgMgr+1);

    sprintf(sSql,"%s",pOrgTable);
    //sprintf(sSql," SELECT ORG_ID, nvl(PARENT_ORG_ID, -2), nvl(ORG_LEVEL_ID, -2), "
    //   " NVL(ORG_AREA_CODE,'') FROM %s ",pOrgTable);
    //qry.setSQL ("SELECT ORG_ID, nvl(PARENT_ORG_ID, -2), nvl(ORG_LEVEL_ID, -2), FROM ORG");
    qry.setSQL(sSql);
    qry.open ();

    i = 0;

    while (qry.next ()) {
        if (i==count) THROW (MBC_OrgMgr+2);

        m_poOrgExList[i].m_iOrgID = qry.field(0).asInteger ();
        m_poOrgExList[i].m_iParentID = qry.field(1).asInteger ();
        m_poOrgExList[i].m_iLevel = qry.field(2).asInteger ();
        strcpy(m_poOrgExList[i].sAreaCode,qry.field(3).asString());

        m_poOrgExIndex->add (m_poOrgExList[i].m_iOrgID, &(m_poOrgExList[i]));

        i++;
    }
    qry.close ();

    while (i--) {
        m_poOrgExIndex->get (m_poOrgExList[i].m_iParentID, &(m_poOrgExList[i].m_poParent));
    }

    m_bUploadedEx = true;
}


void OrgMgrEx::ExitInstance(void)
{
    map<int,OrgMgrEx*>::iterator it = 
        m_mapType2OrgEx.begin();
    for(;it!= m_mapType2OrgEx.end();it++)
    {
        OrgMgrEx* pOrgMgrEx= it->second;
        pOrgMgrEx->unload();
        delete pOrgMgrEx; pOrgMgrEx=0;
    }
    m_mapType2OrgEx.clear();
    
}
//##ModelId=4248F84401D5
void OrgMgrEx::unload()
{
	if (m_poOrgExIndex)
		delete m_poOrgExIndex;

	if (m_poOrgExList)
		delete[] m_poOrgExList;

        m_poOrgExIndex = 0;
        m_poOrgExList = 0;
}

//##ModelId=4248F8440211
#ifdef PRIVATE_MEMORY
bool OrgMgrEx::getBelong(int iChildOrg, int iParentOrg,int iOrgType)
{
	OrgEx * pOrg;
 
        OrgMgrEx* pOrgMgrEx=0;
    if(iOrgType ==BILL_ORG)
    {
        pOrgMgrEx =m_pOrgMgrEx_Bill;
    }else
    {
        map<int,OrgMgrEx*>::iterator it = 
            m_mapType2OrgEx.find(iOrgType);
        if( it !=m_ItEnd)
            pOrgMgrEx = it->second;
        return false;
    }

	if (!pOrgMgrEx->m_poOrgExIndex->get (iChildOrg, &pOrg)) return false;

    if (iChildOrg == iParentOrg ){
         return true;}

	while (pOrg) {
        if (pOrg->m_iParentID == iParentOrg) {
            return true;
        }
		pOrg = pOrg->m_poParent;
    }
        return false;
}
#else
bool OrgMgrEx::getBelong(int iChildOrg, int iParentOrg,int iOrgType)
{
	OrgEx *pOrgEx= 0;
    unsigned int iIdx = 0;
    char sKey[22];
    memset(sKey, '\0', 22);
    sprintf(sKey,"%d_%d", iOrgType, iChildOrg);

    if(!(G_PPARAMINFO->m_poOrgExIndex->get(sKey, &iIdx)))
        return false;
    else if(iChildOrg == iParentOrg)
        return true;
    else
    {
        pOrgEx = (OrgEx *)G_PPARAMINFO->m_poOrgExList + iIdx;
        while(1)
        {
            if(pOrgEx->m_iParentID == iParentOrg)
                return true;
            else if(pOrgEx->m_iParentOffset != 0)
                pOrgEx = (OrgEx *)G_PPARAMINFO->m_poOrgExList + pOrgEx->m_iParentOffset;
            else
                return false;
        }
        return false;
    }
    
}
#endif


//##ModelId=42732CFB01F5
#ifdef PRIVATE_MEMORY
int OrgMgrEx::getParentID(int iOrgID,int iOrgType)
{
	OrgEx * pOrg;

            OrgMgrEx* pOrgMgrEx=0;
    if(iOrgType ==BILL_ORG)
    {
        pOrgMgrEx =OrgMgrEx::m_gOrgMgrEx->m_pOrgMgrEx_Bill;
    }else
    {
        map<int,OrgMgrEx*>::iterator it = 
            OrgMgrEx::m_gOrgMgrEx->m_mapType2OrgEx.find(iOrgType);
        if( it !=m_ItEnd)
            pOrgMgrEx = it->second;
        return false;
    }


	if (!pOrgMgrEx->m_poOrgExIndex->get (iOrgID, &pOrg)) return -1;
	
	return pOrg->m_iParentID;
}
#else
int OrgMgrEx::getParentID(int iOrgID,int iOrgType)
{
	OrgEx * pOrgEx = 0; 
    unsigned int iIdx = 0;
    char sKey[22];
    memset(sKey, '\0', 22);
    sprintf(sKey,"%d_%d", iOrgType, iOrgID);
    
    if(G_PPARAMINFO->m_poOrgExIndex->get(sKey, &iIdx))
    {
        pOrgEx = (OrgEx *)G_PPARAMINFO->m_poOrgExList + iIdx;
        return pOrgEx->m_iParentID;
    }
    else
        return -1;
}
#endif

//added by ap//根据level来获取那一级的parentID
#ifdef PRIVATE_MEMORY
int OrgMgrEx::getLevelParentID(int iOrgID, int iLevel,int iOrgType)
{
	OrgEx * pOrg;

     OrgMgrEx* pOrgMgrEx=0;
    if(iOrgType ==BILL_ORG)
    {
        pOrgMgrEx =OrgMgrEx::m_gOrgMgrEx->m_pOrgMgrEx_Bill;
    }else
    {
        map<int,OrgMgrEx*>::iterator it = 
            OrgMgrEx::m_gOrgMgrEx->m_mapType2OrgEx.find(iOrgType);
        if( it !=m_ItEnd)
            pOrgMgrEx = it->second;
        return false;
    }
	if (!pOrgMgrEx->m_poOrgExIndex->get (iOrgID, &pOrg)) return -1;
	while (pOrg) {
		if (pOrg->m_iLevel == iLevel)
		 return pOrg->m_iOrgID;
		pOrg = pOrg->m_poParent;
	}
	
	return -1;
}
#else
int OrgMgrEx::getLevelParentID(int iOrgID, int iLevel,int iOrgType)
{
    OrgEx * pOrgEx = 0; 
    unsigned int iIdx = 0;
    char sKey[22];
    memset(sKey, '\0', 22);
    sprintf(sKey,"%d_%d", iOrgType, iOrgID);

    if(G_PPARAMINFO->m_poOrgExIndex->get(sKey, &iIdx))
    {
        pOrgEx = (OrgEx *)G_PPARAMINFO->m_poOrgExList + iIdx;
        while(1)
        {
            if(pOrgEx->m_iLevel == iLevel)
                return pOrgEx->m_iOrgID;
            else if(pOrgEx->m_iParentOffset != 0)
                pOrgEx = (OrgEx *)G_PPARAMINFO->m_poOrgExList + pOrgEx->m_iParentOffset;
            else
                return -1;
        }
    }
    else
        return -1;
}
#endif

//##ModelId=42732D1800AC
#ifdef PRIVATE_MEMORY
int OrgMgrEx::getLevel(int iOrgID,int iOrgType)
{
    OrgEx * pOrg;

    OrgMgrEx* pOrgMgrEx=0;
    if(iOrgType ==BILL_ORG)
    {
        pOrgMgrEx =OrgMgrEx::m_gOrgMgrEx->m_pOrgMgrEx_Bill;
    }else
    {
        map<int,OrgMgrEx*>::iterator it = 
            OrgMgrEx::m_gOrgMgrEx->m_mapType2OrgEx.find(iOrgType);
        if( it !=m_ItEnd)
            pOrgMgrEx = it->second;
        return false;
    }
	if (!pOrgMgrEx->m_poOrgExIndex->get (iOrgID, &pOrg)) return -1;

	return pOrg->m_iLevel;
}
#else
int OrgMgrEx::getLevel(int iOrgID,int iOrgType)
{
    OrgEx * pOrgEx = 0; 
    unsigned int iIdx = 0;
    char sKey[22];
    memset(sKey, '\0', 22);
    sprintf(sKey,"%d_%d", iOrgType, iOrgID);

    if(G_PPARAMINFO->m_poOrgExIndex->get(sKey, &iIdx))
    {
        pOrgEx = (OrgEx *)G_PPARAMINFO->m_poOrgExList + iIdx;
        return  pOrgEx->m_iLevel;
    }
    else
        return -1;
}
#endif

