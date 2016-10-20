/*VER: 3*/
#include "EdgeRoamMgr.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#include "SHMStringTreeIndexEx.h"
#endif


bool MobileRegionMgr::m_bIsLoad = false;
KeyTree<EdgeRoamRuleEx* >* MobileRegionMgr::m_gpEdgeRoam = 0;
map<int,KEYTREE_ROAMRULE* >* MobileRegionMgr::m_gpmapEdgeRoamRule=0 ;
KeyTree<EdgeRoamRuleEx* >* MobileRegionMgr::m_gpmapMsc =0;
EdgeRoamRuleEx * MobileRegionMgr::m_gpEdgeRoamRuleEx=0;

MobileRegionMgr::MobileRegionMgr(void)
{
}

MobileRegionMgr::~MobileRegionMgr(void)
{
}

bool MobileRegionMgr::reLoadRule(void)
{
    if(MobileRegionMgr::m_bIsLoad == true){
        MobileRegionMgr::unLoadRule();
        MobileRegionMgr::LoadRule();
        MobileRegionMgr::m_bIsLoad = true;
    }
    return true;
}

bool MobileRegionMgr::unLoadRule(void)
{
    
    if(m_gpmapEdgeRoamRule){
        map<int,KEYTREE_ROAMRULE*>::iterator it  = MobileRegionMgr::m_gpmapEdgeRoamRule->begin();
        while (it!= MobileRegionMgr::m_gpmapEdgeRoamRule->end())
        {
            KEYTREE_ROAMRULE *pKeyTree = (KEYTREE_ROAMRULE*)(it->second);
            pKeyTree->destroy();
            delete pKeyTree; pKeyTree = 0;
            it++;
        }
        MobileRegionMgr::m_gpmapEdgeRoamRule->clear();
    }
    
    if(m_gpEdgeRoam){
        //delete m_gpEdgeRoam;
        m_gpEdgeRoam = 0;
    }
    if(m_gpmapMsc){
        m_gpmapMsc->destroy();
        delete m_gpmapMsc;
        m_gpmapMsc = 0;
    }
    if(m_gpEdgeRoamRuleEx){
        delete [] m_gpEdgeRoamRuleEx;
        m_gpEdgeRoamRuleEx = 0;
    }
    return true;
}


bool MobileRegionMgr::LoadRule(void)
{
    /*MobileRegionMgr::m_pEdgeRoam = new KeyTree<EdgeRoamRuleEx*>;
    if (!m_pEdgeRoam)
        THROW(MBC_FormatMethod+13);*/
    if(0 == MobileRegionMgr::m_gpmapEdgeRoamRule)
    {
        MobileRegionMgr::m_gpmapEdgeRoamRule = new map<int,KEYTREE_ROAMRULE* >();
    }else
    {
        map<int,KEYTREE_ROAMRULE*>::iterator it  = MobileRegionMgr::m_gpmapEdgeRoamRule->begin();
        while (it!= MobileRegionMgr::m_gpmapEdgeRoamRule->end())
        {
            KEYTREE_ROAMRULE *pKeyTree = (KEYTREE_ROAMRULE*)(it->second);
            pKeyTree->destroy();
            delete pKeyTree; pKeyTree = 0;
            it++;
        }
        MobileRegionMgr::m_gpmapEdgeRoamRule->clear();
    }

    if(m_gpmapMsc){ delete m_gpmapMsc;m_gpmapMsc =0;}
    m_gpmapMsc = new KeyTree<EdgeRoamRuleEx* >;
    
    if(m_gpEdgeRoamRuleEx) { delete [] m_gpEdgeRoamRuleEx;m_gpEdgeRoamRuleEx=0;}
    
    DEFINE_QUERY(qry);

    int iCount = 1;
    qry.close();
    qry.setSQL("select count(1)+1 from ("
        "select '1-'||trim(msc)||decode(lac,null,('-'),('-'||trim(lac)))||decode(cell_id,null,'','-'||trim(cell_id)) sKey, "
        " REGION_ID,nvl(type,'12A') TYPE,1 ORG_TYPE,TO_CHAR(NVL(EFF_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')),'YYYYMMDDHH24MISS'), "
        "  TO_CHAR(NVL(EXP_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')),'YYYYMMDDHH24MISS'),trim(msc) msc from B_MOBILE_EQUIP_REGION  A ,ORG B "
         " where a.REGION_ID =B.ORG_ID "
         " UNION "
         " ( "
         " select '2-'||trim(msc)||decode(lac,null,('-'),('-'||trim(lac)))||decode(cell_id,null,'','-'||trim(cell_id)) sKey,"
         " a.REGION_ID,nvl(type,'12A') TYPE,2 ORG_TYPE,TO_CHAR(NVL(EFF_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')), 'YYYYMMDDHH24MISS'),"
         " TO_CHAR(NVL(EXP_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')),'YYYYMMDDHH24MISS'),trim(msc) msc from B_MOBILE_EQUIP_REGION  A ,STAT_REGION B "
         " where a.REGION_ID =B.REGION_ID  ) "
         " union "
         " ( "
         "     select REGION_TYPE_ID||'-'||trim(msc)||decode(lac,null,('-'),('-'||trim(lac)))||decode(cell_id,null,'','-'||trim(cell_id)) sKey,"
         " a.REGION_ID,nvl(type,'12A') TYPE ,b.REGION_TYPE_ID ORG_TYPE,TO_CHAR(NVL(EFF_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')), 'YYYYMMDDHH24MISS'),"
         " TO_CHAR(NVL(EXP_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')),'YYYYMMDDHH24MISS') ,trim(msc) msc from B_MOBILE_EQUIP_REGION  A ,B_SELF_DEFINE_REGION B "
         " where a.REGION_ID =B.REGION_ID "
         " )   order by TYPE "
         ")");
    qry.open();
    if( qry.next() ){
        iCount = qry.field(0).asInteger();
    }

    qry.close();
    qry.setSQL("select '1-'||trim(msc)||decode(lac,null,('-'),('-'||trim(lac)))||decode(cell_id,null,'','-'||trim(cell_id)) sKey, "
        " REGION_ID,nvl(type,'12A') TYPE,1 ORG_TYPE,TO_CHAR(NVL(EFF_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')),'YYYYMMDDHH24MISS'), "
        "  TO_CHAR(NVL(EXP_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')),'YYYYMMDDHH24MISS'),trim(msc) msc from B_MOBILE_EQUIP_REGION  A ,ORG B "
         " where a.REGION_ID =B.ORG_ID "
         " UNION "
         " ( "
         " select '2-'||trim(msc)||decode(lac,null,('-'),('-'||trim(lac)))||decode(cell_id,null,'','-'||trim(cell_id)) sKey,"
         " a.REGION_ID,nvl(type,'12A') TYPE,2 ORG_TYPE,TO_CHAR(NVL(EFF_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')), 'YYYYMMDDHH24MISS'),"
         " TO_CHAR(NVL(EXP_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')),'YYYYMMDDHH24MISS'),trim(msc) msc from B_MOBILE_EQUIP_REGION  A ,STAT_REGION B "
         " where a.REGION_ID =B.REGION_ID  ) "
         " union "
         " ( "
         "     select REGION_TYPE_ID||'-'||trim(msc)||decode(lac,null,('-'),('-'||trim(lac)))||decode(cell_id,null,'','-'||trim(cell_id)) sKey,"
         " a.REGION_ID,nvl(type,'12A') TYPE ,b.REGION_TYPE_ID ORG_TYPE,TO_CHAR(NVL(EFF_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')), 'YYYYMMDDHH24MISS'),"
         " TO_CHAR(NVL(EXP_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')),'YYYYMMDDHH24MISS') ,trim(msc) msc from B_MOBILE_EQUIP_REGION  A ,B_SELF_DEFINE_REGION B "
         " where a.REGION_ID =B.REGION_ID "
         " )   order by TYPE ");
    qry.open();

    int iEdgeType =-1;
    int iEdgeTypePrev = -1;
    KeyTree<EdgeRoamRuleEx* >* pRuleKeyTree = 0;
    EdgeRoamRuleEx* pRule = 0;
    EdgeRoamRuleEx* pRuleTmp=0;
    char sType[4]={0};
    
    m_gpEdgeRoamRuleEx = new EdgeRoamRuleEx[iCount] ;

    
    while (qry.next()) {        
        strcpy(sType,qry.field(2).asString());
        switch(sType[2]|0x20)
        {
        case 'a':
            {
                iEdgeType =0;
            }
            break;
        case 'b':
        default:
            {
                iEdgeType = 1;
            }
        }
        if((iEdgeTypePrev!= iEdgeType)||(-1 == iEdgeTypePrev))
        {
            //不同的覆盖类型
            pRuleKeyTree = new KeyTree<EdgeRoamRuleEx*>;
            (*MobileRegionMgr::m_gpmapEdgeRoamRule)[iEdgeType] = pRuleKeyTree;
        }else
        {
            map<int,KeyTree<EdgeRoamRuleEx* >* >::iterator it = 
                MobileRegionMgr::m_gpmapEdgeRoamRule->find(iEdgeType);
            if(it == MobileRegionMgr::m_gpmapEdgeRoamRule->end())
            {
                assert(false);
            }
            pRuleKeyTree =it->second;
        }
        iCount--;
        if(0==iCount){
            qry.close();
            return false;
        }
        pRule = m_gpEdgeRoamRuleEx+iCount; //new EdgeRoamRuleEx();
        pRule->m_iOrgID = qry.field(1).asInteger();
        pRule->m_iOrgType = qry.field(3).asInteger();
        pRule->m_nIsEdage = iEdgeType;
        strcpy(pRule->m_szEffDate,qry.field(4).asString());
        strcpy(pRule->m_szExpDate,qry.field(5).asString());
        if(pRuleKeyTree->get(qry.field(0).asString(),&pRuleTmp))
        {
        		pRule->pNext = pRuleTmp->pNext;
            pRuleTmp->pNext=pRule;
        }else
        {
            pRuleKeyTree->add(qry.field(0).asString(), pRule);

            m_gpmapMsc->add(qry.field(6).asString(),pRule);
        }
        iEdgeTypePrev = pRule->m_nIsEdage;
    }

    qry.close();

    return true;
}


bool MobileRegionMgr::CheckCellEdgeType(const char* sMsc, const char* sLac,const char* sCellID,
        const char* szTime,
        int iOrgID,
        int iEdgeType)
{
    int iOrgIDTmp = 0;
    int iOrgIDs[32]={0};
    int iCnt=0;
    if(!GetOrgIDByType(sMsc,sLac,sCellID,szTime,&iOrgIDs[0],iCnt,iEdgeType))
        return false;
    while(iCnt>0)
    {
        //是否需要加归属判断
        if(iOrgIDs[--iCnt]== iOrgID)
            return true;
    }
    return false;
}

#ifdef PRIVATE_MEMORY
bool MobileRegionMgr::GetOrgIDByType(const char* sMsc,const char* sLac,const char* sCellID,
                                     const char* szTime,
                                     int* pOrgID,int& iCnt,int iType,int iOrgType)
{
    if(MobileRegionMgr::m_bIsLoad == false)
    {
        MobileRegionMgr::LoadRule();
        MobileRegionMgr::m_bIsLoad = true;
    }
    static char s_sKey[40]={0};
    static EdgeRoamRuleEx* s_poRoamRule = 0;
    s_poRoamRule =0;


    map<int,KeyTree<EdgeRoamRuleEx* >* >::iterator it =
        m_gpmapEdgeRoamRule->find(iType);
    if(it == m_gpmapEdgeRoamRule->end())
        return false;
    m_gpEdgeRoam = it->second;
    bool bRet =false;

    sprintf(s_sKey,"%d-%s-%s-%s",iOrgType, sMsc, sLac, sCellID);
    if ((!bRet)&&m_gpEdgeRoam->get(s_sKey, &s_poRoamRule)) {
        //iOrgID = s_poRoamRule->m_iOrgID;
        //nEdgeType = s_poRoamRule->m_nIsEdage;
        bRet= true;
    }else
        sprintf(s_sKey,"%d-%s-%s",iOrgType, sMsc, sLac);
    
    if ((!bRet)&&m_gpEdgeRoam->get(s_sKey, &s_poRoamRule)) {
        //iOrgID = s_poRoamRule->m_iOrgID;
        //nEdgeType = s_poRoamRule->m_nIsEdage;
        bRet= true;
    }else
        sprintf(s_sKey,"%d-%s--%s",iOrgType,sMsc,sCellID);

    if((!bRet)&&m_gpEdgeRoam->get(s_sKey, &s_poRoamRule)) {
        //iOrgID = s_poRoamRule->m_iOrgID;
        //nEdgeType = s_poRoamRule->m_nIsEdage;
        bRet= true;
    }else
        sprintf(s_sKey,"%d-%s-", iOrgType,sMsc);

    
    if ((!bRet)&&m_gpEdgeRoam->get(s_sKey, &s_poRoamRule)) {
        //iOrgID = s_poRoamRule->m_iOrgID;
        //nEdgeType = s_poRoamRule->m_nIsEdage;
        bRet=true;
    }

    if(bRet)
    {
        while(s_poRoamRule !=0)
        {
            //if(iOrgType == s_poRoamRule->m_iOrgType)
          //  {
                if((0 ==szTime)||(szTime[0] ==0))
                {
                    *pOrgID =s_poRoamRule->m_iOrgID;
                    pOrgID++;
                    iCnt++;
                }else
                {
                    if((strcmp(szTime,s_poRoamRule->m_szEffDate)>=0)&&
                        (strcmp(szTime,s_poRoamRule->m_szExpDate)<0))
                    {
                        *pOrgID =s_poRoamRule->m_iOrgID;
                        pOrgID++;
                        iCnt++;
                    }
                }
          //  }
            s_poRoamRule = s_poRoamRule->pNext;
        }
        if(iCnt>0)
            return true;
        else
            return false;
    }

    //char sTempMSC[30];
    //int iLen = strlen(sMsc)-1;
    //strcpy(sTempMSC,sMsc);
    //while (iLen>0) {
    //    sTempMSC[iLen] = 0;
    //    if (m_gpEdgeRoam->get(sTempMSC, &s_poRoamRule)){
    //        while(s_poRoamRule !=0)
    //        {
    //            if(iType == s_poRoamRule->m_nIsEdage)
    //            {
    //                *pOrgID =s_poRoamRule->m_iOrgID;
    //                pOrgID++;
    //                iCnt++;
    //            }
    //            s_poRoamRule = s_poRoamRule->pNext;
    //        }
    //        return true;
    //    }
    //    iLen--;
    //}

    return false;
}
#else
bool MobileRegionMgr::GetOrgIDByType(const char* sMsc,const char* sLac,const char* sCellID,
                                     const char* szTime,
                                     int* pOrgID,int& iCnt,int iType,int iOrgType)
{
    
    char s_sKey[40] = {0};
    EdgeRoamRuleEx* pRoamRule = 0;
    EdgeRoamRuleEx* pHead = 0;

    SHMStringTreeIndexEx *pIndex = 0;
    unsigned int iIdx = 0;

    if(iType == 0)
        pIndex = G_PPARAMINFO->m_poEdgeRoamRuleExTypeAIndex;
    else if(iType == 1)
        pIndex = G_PPARAMINFO->m_poEdgeRoamRuleExTypeBIndex;
    else
        return false;

    bool bRet =false;

    sprintf(s_sKey,"%d-%s-%s-%s",iOrgType, sMsc, sLac, sCellID);
    if ((!bRet) && pIndex->get(s_sKey, &iIdx)) 
    {
        bRet= true;
    }
    else
        sprintf(s_sKey,"%d-%s-%s",iOrgType, sMsc, sLac);
    
    if ((!bRet) && pIndex->get(s_sKey, &iIdx)) 
    {
        bRet= true;
    }
    else
        sprintf(s_sKey,"%d-%s--%s",iOrgType,sMsc,sCellID);

    if((!bRet) && pIndex->get(s_sKey, &iIdx)) 
    {
        bRet= true;
    }
    else
        sprintf(s_sKey,"%d-%s-", iOrgType,sMsc);

    
    if((!bRet) && pIndex->get(s_sKey, &iIdx)) 
    {
        bRet=true;
    }

    if(bRet)
    {
        pHead = G_PPARAMINFO->m_poEdgeRoamRuleExList;
        pRoamRule = pHead + iIdx;
        while(pRoamRule != pHead)
        {
            if((0 ==szTime)||(szTime[0] ==0))
            {
                *pOrgID =pRoamRule->m_iOrgID;
                pOrgID++;
                iCnt++;
            }
            else
            {
                if( (strcmp(szTime, pRoamRule->m_szEffDate)>=0)&&
                    (strcmp(szTime, pRoamRule->m_szExpDate)<0))
                {
                    *pOrgID =pRoamRule->m_iOrgID;
                    pOrgID++;
                    iCnt++;
                }
                else
                {
                   // pRoamRule = pHead + pRoamRule->m_iNextOffset;
                }
            }
        		pRoamRule = pHead + pRoamRule->m_iNextOffset;
        }

        if(iCnt>0)
            return true;
        else
            return false;
    }
    else
        return false;
}
#endif

bool MobileRegionMgr::GetOrgID(const char* sMsc,const char* sLac,const char* sCellID,const char* szTime,int& iOrgID)
{
    int iOrgIDs[32]={0};
    int iCnt =0;
    int  nEdgeType =0;
    if(GetOrgIDByType(sMsc,sLac,sCellID,szTime, &iOrgIDs[0],iCnt,nEdgeType))
    {
        iOrgID= iOrgIDs[0];
        return true;
    }else
        return false;
}

#ifdef PRIVATE_MEMORY
bool MobileRegionMgr::MscHasExists(const char* sMsc,char* szDate ,char* szAreaCode)
{

    //没有对szAreaCode判断 暂时不用

    if(MobileRegionMgr::m_bIsLoad == false)
    {
        MobileRegionMgr::LoadRule();
        MobileRegionMgr::m_bIsLoad = true;
    }
    //if(0 ==m_gpmapMsc)
    //    return false;
    EdgeRoamRuleEx* pRuleTmp=0;
    if(m_gpmapMsc->get((char*)sMsc,&pRuleTmp))
    {
        if(0 ==pRuleTmp)
            return false;
        while(pRuleTmp)
        {
            if((0 == szDate)||(0== szDate[0]))
                return true;
            else
            {
                if((strcmp(szDate,pRuleTmp->m_szEffDate)>=0)&&
                    (strcmp(szDate,pRuleTmp->m_szExpDate)<0))
                {
                    return true;
                }
            }
            pRuleTmp  = pRuleTmp->pNext;
        }
    }
    return false;
}
#else
bool MobileRegionMgr::MscHasExists(const char* sMsc,char* szDate ,char* szAreaCode)
{
    unsigned int iIdx = 0;
    EdgeRoamRuleEx* pRule=0;
    EdgeRoamRuleEx* pHead = G_PPARAMINFO->m_poEdgeRoamRuleExList;
    
    if(!(G_PPARAMINFO->m_poEdgeRoamRuleExMSCIndex->get((char*)sMsc, &iIdx)))
        return false;
    else
    {
        pRule = pHead + iIdx;
        while(pRule != pHead)
        {
            if((0 == szDate)||(0== szDate[0]))
                return true;
            else
            {
                if( (strcmp(szDate, pRule->m_szEffDate)>=0)&&
                    (strcmp(szDate, pRule->m_szExpDate)<0))
                {
                    return true;
                }
                else
                    pRule = pHead + pRule->m_iNextOffset;
            }
        }
        return false;
    }
    
}
#endif

