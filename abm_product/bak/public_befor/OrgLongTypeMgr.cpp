/*VER: 2*/ 
#include "OrgLongTypeMgr.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

bool OrgLongTypeMgr::m_bIsLoad = false;
map<int,OrgLongType*>* OrgLongTypeMgr::m_gpHashOrg2LongTypeID=0;
OrgLongType* OrgLongTypeMgr::m_gpOrgLongType =0;

OrgLongTypeMgr::OrgLongTypeMgr(void)
{
}

OrgLongTypeMgr::~OrgLongTypeMgr(void)
{
}

void OrgLongTypeMgr::Load(void)
{
    

    OrgMgrEx* pOrgMgrEx = OrgMgrEx::GetInstance();
	DEFINE_QUERY(qry);

    qry.setSQL(" SELECT COUNT(*) CNT FROM B_REGION_DISTANCE ");
    int nCnt = 0;
    qry.open();
    if(qry.next())
    {
        nCnt = qry.field(0).asInteger()*2;
    }
    qry.close();


    OrgLongTypeMgr::m_gpHashOrg2LongTypeID = new map<int, OrgLongType*>();
    OrgLongTypeMgr::m_gpOrgLongType = new OrgLongType[nCnt];

    qry.setSQL("SELECT ORG_ID_A,ORG_ID_B,DISTANCE_TYPE_ID FROM B_REGION_DISTANCE union all "
        "  select ORG_ID_B,ORG_ID_A,DISTANCE_TYPE_ID from B_REGION_DISTANCE  ");

    int iOrg1,iOrg2;
    int iLongTypeID;
    int iLevel1,iLevel2;
    qry.open();
    int i = 0;
    while(qry.next())
    {
        iOrg1 = qry.field(0).asInteger();
        iOrg2 = qry.field(1).asInteger();
        iLongTypeID = qry.field(2).asInteger();
        iLevel1 = pOrgMgrEx->getLevel(iOrg1);
        iLevel2 = pOrgMgrEx->getLevel(iOrg2);
    //    if(iLevel1>iLevel2)
     //   {
      //      OrgLongTypeMgr::m_gpOrgLongType[i].m_iOrg = iOrg2;
      //  }else
        {
            OrgLongTypeMgr::m_gpOrgLongType[i].m_iOrg = iOrg1;
            iOrg1 = iOrg2;
            iOrg2 = OrgLongTypeMgr::m_gpOrgLongType[i].m_iOrg;
        }
        OrgLongType* pLongType= 0;
        OrgLongType* pLongTypePre= 0;
        OrgLongTypeMgr::m_gpOrgLongType[i].m_iLongType = iLongTypeID;
        map<int,OrgLongType*>::iterator it = OrgLongTypeMgr::m_gpHashOrg2LongTypeID->find(iOrg1);
        if(it!=OrgLongTypeMgr::m_gpHashOrg2LongTypeID->end())
        {
            pLongType = it->second;
            if(iOrg2 == it->second->m_iOrg)
            {
                i++;
                continue;
            }
            iLevel2= pOrgMgrEx->getLevel(OrgLongTypeMgr::m_gpOrgLongType[i].m_iOrg);
            iLevel1= pOrgMgrEx->getLevel(it->second->m_iOrg);
            if(iLevel1<iLevel2)
            {
                OrgLongType* pOrgLongType=&OrgLongTypeMgr::m_gpOrgLongType[i];  
                OrgLongTypeMgr::m_gpOrgLongType[i].m_pNext = it->second;
                (*OrgLongTypeMgr::m_gpHashOrg2LongTypeID)[iOrg1]=pOrgLongType; 
                i++;
                continue;
            }else
            {
                bool bGet=false;
                pLongTypePre = pLongType;
		pLongType = pLongType->m_pNext;
                while(pLongType!=0)
                {
                    //pLongTypePre = pLongType;
                    iLevel1 = pOrgMgrEx->getLevel(pLongType->m_iOrg);
                    if(iLevel1<iLevel2)
                    {
                        OrgLongTypeMgr::m_gpOrgLongType[i].m_pNext =
                            pLongType;
                        pLongTypePre->m_pNext = &OrgLongTypeMgr::m_gpOrgLongType[i];
                        bGet =true;
                        break;
                    }
		    pLongTypePre = pLongType;
		    pLongType = pLongType->m_pNext; 
                }
                if(bGet)
                {
                    i++; continue;
                }
                if(pLongType ==0)
                {
                    pLongTypePre->m_pNext = &OrgLongTypeMgr::m_gpOrgLongType[i];
                }
                i++;
                continue;
            }
        }
        (*OrgLongTypeMgr::m_gpHashOrg2LongTypeID)[iOrg1]= &OrgLongTypeMgr::m_gpOrgLongType[i];
        i++;
    }
    qry.close();
    OrgLongTypeMgr::m_bIsLoad = true;
}

#ifdef PRIVATE_MEMORY
int OrgLongTypeMgr::GetLongTypeByOrg(int iOrg1,int iOrg2)
{
    OrgMgrEx* pOrgMgrEx = OrgMgrEx::GetInstance();
    if(!OrgLongTypeMgr::m_bIsLoad )
        OrgLongTypeMgr::Load();
    int iOrgTmp;
    int iLevel1,iLevel2,iLevel ;
    iLevel1 = pOrgMgrEx->getLevel(iOrg1);
    iLevel2 = pOrgMgrEx->getLevel(iOrg2);
    OrgLongType* pLongType= 0;
    OrgLongType* pLongTypeTmp=0;

    map<int,OrgLongType*>::iterator it = OrgLongTypeMgr::m_gpHashOrg2LongTypeID->begin();
    int iOrgTmp1,iOrgTmp2;
    if(iLevel1>iLevel2)
    {
        iOrgTmp1 = iOrg1;
        iOrgTmp2 = iOrg2;
    }else
    {
        iOrgTmp1 = iOrg2;
        iOrgTmp2 = iOrg1;
    }
    while(1)
    {
        it = OrgLongTypeMgr::m_gpHashOrg2LongTypeID->find(iOrgTmp1);
        if((it== OrgLongTypeMgr::m_gpHashOrg2LongTypeID->end())
            &&(-1 != iOrgTmp1))
            iOrgTmp1 = pOrgMgrEx->getParentID(iOrgTmp1);
        else
        {
            if(it == OrgLongTypeMgr::m_gpHashOrg2LongTypeID->end())
                return -1;            
            pLongType = it->second;
            break;
        }
    }
    if(0== pLongType)
        return -1;
    else
    {
        while(pLongType !=0)
        {
            if((iOrgTmp2 == pLongType->m_iOrg)
                ||(pOrgMgrEx->getBelong(iOrgTmp2,pLongType->m_iOrg)))
            {
                //printf("OrgID:%d Distance :%d\n",pLongType->m_iOrg,pLongType->m_iLongType);
                return pLongType->m_iLongType;
            }else
                pLongType = pLongType->m_pNext;
        }
    }
    iOrgTmp1  = pOrgMgrEx->getParentID(iOrgTmp1);
    return GetLongTypeByOrg(iOrgTmp1,iOrgTmp2);
}
#else
int OrgLongTypeMgr::GetLongTypeByOrg(int iOrg1,int iOrg2)
{
    OrgMgrEx* pOrgMgrEx = OrgMgrEx::GetInstance();
    OrgLongType* pLongType= 0;
    unsigned int iIdx = 0;
    int iLevel1,iLevel2;
    int iOrgTmp1,iOrgTmp2;
    
    iLevel1 = pOrgMgrEx->getLevel(iOrg1);
    iLevel2 = pOrgMgrEx->getLevel(iOrg2);
    if(iLevel1 > iLevel2)  //目标是通过LEVEL高的作为索引找对应LEVEL低的OrgMgrEx
    {
        iOrgTmp1 = iOrg1;
        iOrgTmp2 = iOrg2;
    }
    else
    {
        iOrgTmp1 = iOrg2;
        iOrgTmp2 = iOrg1;
    }

    while(1)//iOrgTmp1索引到的链表,找不到找其父亲的,直到父亲为-1
    {
        if(!G_PPARAMINFO->m_poOrgLongTypeIndex->get(iOrgTmp1, &iIdx))
        {
            if(iOrgTmp1 != -1)
                iOrgTmp1 = pOrgMgrEx->getParentID(iOrgTmp1);
            else
                return -1;
        }
        else
        {
            pLongType = (OrgLongType *)G_PPARAMINFO->m_poOrgLongTypeList + iIdx;
            break;
        }
    }
    //下面肯定找到了链表,接下来就从链表中查找iOrgTmp2 
    OrgLongType *pHead = (OrgLongType *)G_PPARAMINFO->m_poOrgLongTypeList;
    while(pLongType != pHead) 
    {
        if(iOrgTmp2 == pLongType->m_iOrg || (pOrgMgrEx->getBelong(iOrgTmp2,pLongType->m_iOrg)))
        {
            return pLongType->m_iLongType;
        }
        else
        {
            pLongType = (OrgLongType *)G_PPARAMINFO->m_poOrgLongTypeList + pLongType->m_iNextOffset;
        }
    }
    //找完了链表可能还没有找到iOrgTmp2,就去找iOrgTmp1的父亲的,递归
    iOrgTmp1  = pOrgMgrEx->getParentID(iOrgTmp1);
    return GetLongTypeByOrg(iOrgTmp1,iOrgTmp2);
}

#endif

