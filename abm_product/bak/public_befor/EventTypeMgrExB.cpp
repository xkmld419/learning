#include "EventTypeMgrEx_B.h"


EventTypeMgrEx_B* EventTypeMgrEx_B::m_gEventTypeMgrEx_B = 0;
EventTypeMgrEx_B::EventTypeMgrEx_B(void)
{
}

EventTypeMgrEx_B::~EventTypeMgrEx_B(void)
{
    m_pOrgMgr = 0;

    ExitInstance();
}

EventTypeMgrEx_B* EventTypeMgrEx_B::GetInstance(void)
{
    if(0== EventTypeMgrEx_B::m_gEventTypeMgrEx_B)
    {
        EventTypeMgrEx_B::m_gEventTypeMgrEx_B = new EventTypeMgrEx_B();
        m_gEventTypeMgrEx_B->InitInstance();
        return EventTypeMgrEx_B::m_gEventTypeMgrEx_B;
    }else
        return EventTypeMgrEx_B::m_gEventTypeMgrEx_B;
}


//判断两个ORG_ID 在哪一级相同
int EventTypeMgrEx_B::CheckOrgLevel(int lOrg1,int lOrg2,int &nParentID)
{
    if(!m_pOrgMgr)
        return 0;
 
    int nLevel1 = m_pOrgMgr->getLevel(lOrg1);
    int nLevel2 = m_pOrgMgr->getLevel(lOrg2);

    if(lOrg1 == lOrg2)
    {
        nParentID = m_pOrgMgr->getLevelParentID(lOrg2,nLevel1);
        return nLevel1;
    }
    int nOrg1,nOrg2;
    int nParentOrgID = 0;

    //确保nOrg1   高于 nOrg2
    if(nLevel1 <nLevel2)
    {
        nOrg1=lOrg1;
        nOrg2=lOrg2;
    }else
    {
        nOrg1 = lOrg2;
        nOrg2 =lOrg1;
        nLevel1 = nLevel2+nLevel1;
        nLevel2 =nLevel1 -nLevel2;
        nLevel1 = nLevel1 -nLevel2;
    }
    while(1)
    {
        nParentOrgID = m_pOrgMgr->getLevelParentID(nOrg2,nLevel1);
        if(nParentOrgID != nOrg1)
        {
            nOrg1 = m_pOrgMgr->getParentID(nOrg1);
            nLevel1 = m_pOrgMgr->getLevel(nOrg1);
        }else
            break;
    }
    nParentID = nParentOrgID;
    return nLevel1;
}


bool EventTypeMgrEx_B::InitInstance(void)
{

    char sSql[1024]={0};

    char sOrgTable[32]="ORG_EX";
    m_pOrgMgr = new OrgMgrEx(sOrgTable);
    //m_pOrgMgr->load();

    TOCIQuery qry(Environment::getDBConn());

    int nOrg1 = 0;
    int nOrg2 =0;
    int nOrg3 =0;
    int nOrg4 =0;
    int nOrgLevel1 =0;
    int nOrgLevel2=0;
    int nOrgLevel3=0;
    int nEventTypeID = 0;

    m_pRoamTypeRule = new HashList<RoamTypeRule*>(500);
    m_pLocalHeadMgr = new LocalHeadMgr();
    m_plstLongTypeRule = new HashList<LongTypeRule*>(500);
    m_plstLongTypeRuleV = new HashList<LongTypeRule*>(500);
    

    int nParentOrgID = 0;

    int nLevelTmp = 0;
    char sKey[32]={0};
    strcpy(sSql," SELECT CALLING_AD_ORG,CALLING_VIST_ORG,"
        " CALLED_AD_ORG,CALLED_VIST_ORG,NVL(ALL_SAME,999999999),LONG_TYPE, "
        " IN_LONG_TYPE_ID,CALL_TYPE_ID FROM LONG_TYPE_RULE "
        " ORDER BY NVL(ALL_SAME,999999999) ");
    qry.setSQL(sSql);
    qry.open();
    while(qry.next())
    {
        LongTypeRule* pRule = new LongTypeRule;
        memset(pRule,0,sizeof(LongTypeRule));
        pRule->nCalledOrgIDAd = qry.field(2).asInteger();
        pRule->nCalledOrgIDVisit = qry.field(3).asInteger();
        pRule->nCallingOrgIDAd = qry.field(0).asInteger();
        pRule->nCallingOrgIDVisit = qry.field(1).asInteger();
        pRule->nAllSame = qry.field(4).asInteger();
        pRule->nLongTypeID = qry.field(5).asInteger();
        pRule->nInLongTypeID= qry.field(6).asInteger();
        pRule->nCallTypeID = qry.field(7).asInteger();
        
        nOrgLevel1=CheckOrgLevel(nOrg1,nOrg2,pRule->nParentOrgCallingV2CallingAD);
        nOrgLevel2=CheckOrgLevel(nOrg1,nOrg3,pRule->nParentOrgCallingV2CalledAD);
        nOrgLevel3=CheckOrgLevel(nOrg1,nOrg4,pRule->nParentOrgCallingV2CalledV);
        m_lstRule.push_back(pRule);
        sprintf(sKey,"%d_%d_%d_%d_%d",pRule->nParentOrgCallingV2CallingAD, pRule->nParentOrgCallingV2CalledAD
            ,pRule->nParentOrgCallingV2CalledV,pRule->nInLongTypeID,pRule->nCallTypeID);


        if(nOrgLevel1 - nOrgLevel2 >0){
            nLevelTmp = nOrgLevel2 >nOrgLevel3 ?
                nOrgLevel3:nOrgLevel2;
        }else{
            nLevelTmp = nOrgLevel1 >nOrgLevel3 ?
                nOrgLevel3:nOrgLevel1;
        }

        LongTypeRule* pTmpRuleH =0;
        LongTypeRule* pTmpRuleV=0;
        if(m_plstLongTypeRule->get(sKey,&pTmpRuleH))
        {
            while(!pTmpRuleH->pNextH)
                pTmpRuleH = pTmpRuleH->pNextH;
            pTmpRuleH->pNextH = pRule;
        }else
            m_plstLongTypeRule->add(sKey,pRule);

        sprintf(sKey,"%d",nLevelTmp);
        if(m_plstLongTypeRule->get(sKey,&pTmpRuleV))
        {
            while(!pTmpRuleV->pNextV)
                pTmpRuleV = pTmpRuleV->pNextV;
            pTmpRuleV->pNextV = pRule;
        }else
            m_plstLongTypeRuleV->add(sKey,pRule);
    }

    qry.close();

    loadHLR();




    m_iMinSegCodeLen =0;
    m_iMaxSegCodeLen =0;

    return true;
}


bool EventTypeMgrEx_B::ExitInstance()
{
    if(m_pEdgeRoam)
    {
        delete m_pEdgeRoam;
    }
    m_pEdgeRoam = 0;



    if(m_pRoamTypeRule)
        delete m_pRoamTypeRule;
    m_pRoamTypeRule = 0;

    if(m_pLocalHeadMgr)
        delete m_pLocalHeadMgr;
    m_pLocalHeadMgr = 0;
    return true;
}
void EventTypeMgrEx_B::LoadRomaRule(void)
{
    char sAreaCode[MAX_CALLED_AREA_CODE_LEN]={0};
    m_pEdgeRoam = new KeyTree<EdgeRoamRule3G*>;
    if (!m_pEdgeRoam)
        THROW(MBC_FormatMethod+13);
   
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL("select trim(msc)||decode(lac,null,'',('-'||trim(lac)||decode(cell_id,null,'','-'||trim(cell_id)))) sKey, "
        " nvl(area_code,-1) area_code,nvl(edge_roam_id,0) from b_edge_roam_3g");
    qry.open();
    
    while (qry.next()) {
        EdgeRoamRule3G* pRule = new EdgeRoamRule3G();
        strcpy(pRule->m_sAreaCode,qry.field(1).asString());
        pRule->m_nIsEdage = qry.field(2).asInteger();
        m_pEdgeRoam->add(qry.field(0).asString(), pRule);
        //m_lstRoamTypeRule.push_back(pRule);
    }

    qry.close();

    qry.setSQL(" SELECT ROAM_TYPE_ID,AD_ORG_ID,VISIT_ORG_ID,NVL(EDGE_ROAM_TYPE,0) "
        " NVL(VISIT_VISIT_SAME_LEVEL,999999999),NVL(VISIT_AD_SAME_LEVEL,999999999)  "
        " FROM B_ROAM_TYPE_RULE order by NVL(VISIT_SAME_LEVEL,999999999) ");
    qry.open();

    int nLevel = 0;
    int nParentOrgID=0;
    RoamTypeRule* pRoamRuleTmp=0 ;
    char sKey[40]={0};
    while(qry.next())
    {
        RoamTypeRule* pRoamRule = new RoamTypeRule;
        memset(pRoamRule,0,sizeof(RoamTypeRule));
        pRoamRule->nAdOrgID = qry.field(1).asInteger();
        pRoamRule->nVisitOrgID = qry.field(2).asInteger();
        pRoamRule->nRoamTypeID = qry.field(0).asInteger();
        pRoamRule->nEdgeRoamID = qry.field(3).asInteger();
        pRoamRule->nVVSameLevel = qry.field(4).asInteger();
        pRoamRule->nVDSameLevel = qry.field(5).asInteger();
        pRoamRule->nLevel = this->CheckOrgLevel(pRoamRule->nAdOrgID,pRoamRule->nVisitOrgID,nParentOrgID);
        sprintf(sKey,"%d_%d",pRoamRule->nLevel,pRoamRule->nEdgeRoamID);
         
        if(m_pRoamTypeRule->get(sKey,&pRoamRuleTmp))
        {
            while(pRoamRuleTmp->pNext)
                pRoamRuleTmp = pRoamRuleTmp->pNext;
            pRoamRuleTmp->pNext = pRoamRule;
            continue;
        }
        m_pRoamTypeRule->add(sKey,pRoamRule);
    }
    qry.close();

    HashList<RoamTypeRule*>::Iteration it =  m_pRoamTypeRule->getIteration();
    while(it.next(pRoamRuleTmp))
    {
        this->MakeRoamRuleSort(&pRoamRuleTmp);
        m_pRoamTypeRule->add(pRoamRuleTmp->nLevel,pRoamRuleTmp);
    }
}


int EventTypeMgrEx_B::MakeRoamRuleSort(RoamTypeRule** ppRuleType)
{
    if(!ppRuleType)
        return 0;
    RoamTypeRule* pRule = *ppRuleType;
    RoamTypeRule* pRuleTmp = 0;
    RoamTypeRule* pRuleTmp1 =0;
    RoamTypeRule* pRootRule = 0;

    int n1,n2;
    while(!pRule)
    {
        if(!pRootRule){
            pRootRule = pRule;
            continue;
        }
        pRuleTmp = pRootRule;
        while(!pRuleTmp)
        {
            n1 = pRuleTmp->nVDSameLevel+ pRuleTmp->nVVSameLevel;
            n2 = pRule->nVDSameLevel + pRule->nVVSameLevel;
            if(n1>n2)
            {
                //相同层次越小排在越前面
                pRule->pPrev = pRuleTmp->pPrev;
                pRule->pNext = pRuleTmp->pNext;
                pRuleTmp->pPrev = pRule;
                break;
            }
            pRuleTmp1 = pRuleTmp;
            pRuleTmp = pRuleTmp->pNext;
        }
        pRuleTmp1->pNext = pRule;
    }
    *ppRuleType = pRootRule;
    return 1;
}
//int EventTypeMgrEx_B::GetRomaTypeID(char *pBillingAdNbr,char* pBillingAreaCode,char *pBillingVisitNbr)
//{
//    if((!pBillingAcNbr)&&(!pBillingVisitNbr))
//        return 0;
//    //到访地为空的默认和归属地一样
//    if(!pBillingVisitNbr)
//        pBillingVisitNbr = pBillingAdNbr;
//    
//}

//## 根据 _sMSC,_sLAC,_sCellID , 查找相应的 EdgeRoamRule
//## 找到,置相应的边界漫游地区号_sEdgeRoam; 找不到,不作任何改变
bool EventTypeMgrEx_B::getEdgeRoamByMSC(char* _sMSC, char* _sLAC, char* _sCellID,
        char  *pAreaCode,int *pEdgeType)
{
   static char s_sKey[40];
    static EdgeRoamRule3G* s_poRoamRule= new EdgeRoamRule3G();
    if(! pAreaCode)
        return false;
    
    sprintf(s_sKey,"%s-%s-%s", _sMSC, _sLAC, _sCellID);
    if (m_pEdgeRoam->get(s_sKey, &s_poRoamRule)) {
        strcpy(pAreaCode,s_poRoamRule->m_sAreaCode);
        *pEdgeType = s_poRoamRule->m_nIsEdage;
        return true;
    }

    sprintf(s_sKey,"%s-%s", _sMSC, _sLAC);
    if (m_pEdgeRoam->get(s_sKey, &s_poRoamRule)) {
        strcpy(pAreaCode,s_poRoamRule->m_sAreaCode);
        *pEdgeType = s_poRoamRule->m_nIsEdage;
        return true;
    }

    sprintf(s_sKey,"%s", _sMSC);
    if (m_pEdgeRoam->get(s_sKey, &s_poRoamRule)) {
        strcpy(pAreaCode,s_poRoamRule->m_sAreaCode);
        *pEdgeType = s_poRoamRule->m_nIsEdage;
        return true;
    }

    char sTempMSC[30];
    int iLen = strlen(_sMSC)-1;
    strcpy(sTempMSC,_sMSC);
    while (iLen>0) {
        sTempMSC[iLen] = 0;
        if (m_pEdgeRoam->get(sTempMSC, &s_poRoamRule)){
            strcpy(pAreaCode, s_poRoamRule->m_sAreaCode);
            *pEdgeType = s_poRoamRule->m_nIsEdage;
            return true;
        }
        iLen--;
    }

    return false;
}


int EventTypeMgrEx_B::GetRoamTypeID(RoamTypeRule* pRoamTypeRule,int nEdgeID,int nVDSameLevel,int nVVSameLevel)

{
    if(0 ==pRoamTypeRule)
        return -1;
    RoamTypeRule* pRule = pRoamTypeRule;
    RoamTypeRule* pRulePre=0;
    while(pRule!=0)
    {
        pRulePre = pRule;
        if((pRule->nEdgeRoamID == nEdgeID)
            &&(pRule->nVDSameLevel = nVDSameLevel)
            &&(pRule->nVVSameLevel = nVVSameLevel))
            return pRule->nRoamTypeID;
        pRule = pRule->pNext;
    }
    return pRulePre->nRoamTypeID;
}
int EventTypeMgrEx_B::GetRoamTypeID(const char* pAdNbr,const char* pAdAreaCode,const char* pVisitNbr,
                                const char* pVisitAreaCode,const char* pOtherVisitNbr,
                                const char* pOtherVisitAreaCode, const char * pOtherAdNbr,
                                const char* pOtherAdAreaCode,
                                const int nEdgeType, const char* sTime)
{
    int nOrgIDAd = 0, nOrgIDVisit = 0;
    int nOtherOrgIDVisit = 0,nOtherOrgIDAd = 0;
    char sTmp[32]={0};
    int nRoamType=0;

    //归属地Org
    if(!m_pLocalHeadMgr->searchLocalHead(pAdNbr,pAdAreaCode,sTime,&m_oLocalHead))
    {
        Log::log(0,"没有找到对应号码的ORG  :%s_%s",pAdNbr,pAdAreaCode);
        nOrgIDAd = 1;
    }else
        nOrgIDAd =  m_oLocalHead.getOrgID();

    //到访地Org
    if(!m_pLocalHeadMgr->searchLocalHead(pVisitNbr,pVisitAreaCode,sTime,&m_oLocalHead))
    {
        Log::log(0,"没有找到对应号码的ORG  :%s_%s",pAdNbr,pAdAreaCode);
        nOrgIDVisit = 1;
    }else
        nOrgIDVisit = m_oLocalHead.getOrgID();

    if(!m_pLocalHeadMgr->searchLocalHead(pOtherVisitNbr,pOtherVisitAreaCode,sTime,&m_oLocalHead))
    {
        Log::log(0,"没有找到对应号码的ORG  :%s_%s",pOtherVisitAreaCode,pOtherVisitNbr);
        nOtherOrgIDVisit = 1;
    }else
        nOtherOrgIDVisit = m_oLocalHead.getOrgID();

    if(!m_pLocalHeadMgr->searchLocalHead(pOtherAdNbr,pOtherAdAreaCode,sTime,&m_oLocalHead))
    {
        Log::log(0,"没有找到对应号码的ORG　:%s_%s",pOtherAdAreaCode,pOtherAdNbr);
        nOtherOrgIDAd =1;
    }else
        nOtherOrgIDAd = m_oLocalHead.getOrgID();


    int nParentID=0;

    //归属地和漫游地归属于那一层ORG
    int nLevel1 = this->CheckOrgLevel(nOrgIDAd,nOrgIDVisit,nParentID);
    if(!nLevel1)
        assert(false);  //ORG_ID没有相同的归属
    //通话方归属地属于哪一层ORG;
    int nLevel2 = this->CheckOrgLevel(nOrgIDAd,nOtherOrgIDAd,nParentID);
    if(!nLevel2)
        assert(false);


    RoamTypeRule* pRoamRule = 0;
    while(!m_pRoamTypeRule->get(nLevel1,&pRoamRule))
    {
        nOrgIDVisit = nParentID;
        nLevel1 = this->CheckOrgLevel(nOrgIDAd,nOrgIDVisit,nParentID);
        if(!nLevel1)
            assert(false);  //ORG_ID没有相同的归属
    }
    if(!pRoamRule)
    {
        //没有匹配漫游规则
        Log::log(0,"没有匹配漫游规则 :%s_%s  %s_%s",pAdAreaCode,pAdNbr,pVisitAreaCode,pVisitNbr);
        return 0;
    }



    int nLevel3 = this->CheckOrgLevel(nOrgIDVisit,nOrgIDVisit,nParentID);
    if(!nLevel3)
        assert(false);  //ORG_ID没有相同的归属
    return this->GetRoamTypeID(pRoamRule,nEdgeType,nLevel2,nLevel3);
}


bool EventTypeMgrEx_B::fillVisitAreaCodeOf3G(EventSection* pEventSection)
{
    char sHcode[20];

    if(CALLTYPE_MOC == pEventSection->m_iCallTypeID)
    {
        //求主叫漫游地AreaCode
        if(!getEdgeRoamByMSC(pEventSection->m_sMSC,pEventSection->m_sLAC,
            pEventSection->m_sCellID,pEventSection->m_sCallingAreaCode,&(((StdEvent*)pEventSection)->m_iRoamFlag)))
        {
            //没有配置没有漫游置
            if (0 == pEventSection->m_oEventExt.m_sCallingVisitAreaCode[0]) 
            {
                pEventSection->m_iErrorID = 1800;
                return false;
            }
        }

        if(0 != pEventSection->m_sRomingNBR[0])
        {
            if (!getHLRInfoByMobileNBR(pEventSection->m_sRomingNBR,
                pEventSection->m_oEventExt.m_sCalledVisitAreaCode,sHcode))
            {    
                //"找不到" 并且 "之前也未预置" 计费方到访地区号 则置为错单
                if (0 == pEventSection->m_oEventExt.m_sCalledVisitAreaCode[0]) 
                {
                    pEventSection->m_iErrorID = 1801;
                    return false;
                }
            }
        }
        else
        {
            strcpy(pEventSection->m_oEventExt.m_sCalledVisitAreaCode,pEventSection->m_sCalledAreaCode);
        }

        strcpy(pEventSection->m_sBillingVisitAreaCode,pEventSection->m_sCallingAreaCode);
    } else if(CALLTYPE_MTC == pEventSection->m_iCallTypeID)  
    {
        //对于MTC话单
        //被叫号码通过网络信息求取到访地区号;
        //主叫号码的到访地区号就取主叫号码的归属地区号。同时把被叫号码到访地区号置给计费号码的到访地区号。
        if (!getEdgeRoamByMSC(pEventSection->m_sMSC, pEventSection->m_sLAC, pEventSection->m_sCellID,
            pEventSection->m_oEventExt.m_sCalledVisitAreaCode,
            &(((StdEvent*)pEventSection)->m_iRoamFlag)))
        {    
            //"找不到" 并且 "之前也未预置" 计费方到访地区号 则置为错单 
            if (0 == pEventSection->m_oEventExt.m_sCalledVisitAreaCode[0]) 
            {
                pEventSection->m_iErrorID = 1802;
                return false;
            }
        }
        strcpy(pEventSection->m_oEventExt.m_sCallingVisitAreaCode,pEventSection->m_sCallingAreaCode); 
        strcpy(pEventSection->m_sBillingVisitAreaCode,pEventSection->m_oEventExt.m_sCalledVisitAreaCode); 
    }else if (CALLTYPE_FORWARD == pEventSection->m_iCallTypeID)    
    {
        assert(false);
    }
    return true;
}

int EventTypeMgrEx_B::GetRoamTypeID(EventSection* pEventSection)
{
    if(!pEventSection)
        return 0;
    int nEdge = 0;
    char sAreaCode[11]={0};
    getEdgeRoamByMSC(pEventSection->m_sMSC,pEventSection->m_sLAC,
        pEventSection->m_sCellID,sAreaCode,&nEdge);
    if(strcpy(pEventSection->m_sBillingVisitAreaCode,sAreaCode)!=0)
        nEdge = 0;
    return GetRoamTypeID(pEventSection->m_sBillingNBR,pEventSection->m_sBillingAreaCode,
        pEventSection->m_sBillingNBR,pEventSection->m_sBillingVisitAreaCode,
        pEventSection->m_sCalledNBR,pEventSection->m_oEventExt.m_sCalledVisitAreaCode,
        nEdge,pEventSection->m_sStartDate);
}
int EventTypeMgrEx_B::GetEventTypeID(EventSection* pEventSection)
{
    if(! pEventSection)
        return 0;
    ((StdEvent*)pEventSection)->m_oEventExt.m_iLongTypeID = GetLongTypeID(pEventSection);

    if(!((StdEvent*)pEventSection)->m_oEventExt.m_iLongTypeID)
    {
        Log::log(0,"未能匹配事件类型");
        assert(false);
    }

    ((StdEvent*)pEventSection)->m_iRoamFlag = this->GetRoamTypeID(pEventSection);

    return ((StdEvent*)pEventSection)->m_iEventTypeID;
}
int EventTypeMgrEx_B::GetLongTypeID(EventSection *pEventSection)
{

    int nBillingOrgIDAd =1,nBillingOrgIDVisit=1,nCalledOrgIDAd=1,nCalledOrgVisit=1;

    //计费方归属地 Org
    if(m_pLocalHeadMgr->searchLocalHead(pEventSection->m_sBillingAreaCode,
        pEventSection->m_sBillingNBR,pEventSection->m_sStartDate,&m_oLocalHead))
        nBillingOrgIDAd = m_oLocalHead.getOrgID();
    else
        assert(false);

    //计费方漫游地 Org
    if(m_pLocalHeadMgr->searchLocalHead(pEventSection->m_sBillingVisitAreaCode,
        pEventSection->m_sBillingNBR,pEventSection->m_sStartDate,&m_oLocalHead))
        nBillingOrgIDVisit = m_oLocalHead.getOrgID();
    else
        assert(false);

    //被叫方归属地 Org
    if(m_pLocalHeadMgr->searchLocalHead(pEventSection->m_sCalledAreaCode,
        pEventSection->m_sCalledNBR,pEventSection->m_sStartDate,&m_oLocalHead))
        nCalledOrgIDAd = m_oLocalHead.getOrgID();
    else
        assert(false);

    //被叫漫游地 Org
    if(m_pLocalHeadMgr->searchLocalHead(pEventSection->m_oEventExt.m_sCalledVisitAreaCode,
        pEventSection->m_sCalledNBR,pEventSection->m_sStartDate,&m_oLocalHead))
        nCalledOrgVisit= m_oLocalHead.getOrgID();
    else
        assert(false);

    int nLevelCallingAd2CallingVisit=0,nLevelCallingAd2CalledAd=0,nLevelCallingAd2CalledVisit =0;

    int nParentID1 = 0,nParentID2=0,nParentID3=0;
    int nLevel1 =0,nLevel2=0,nLevel3=0;
    nLevel1 = this->CheckOrgLevel(nBillingOrgIDAd,nBillingOrgIDVisit,nParentID1);
    nLevel2 = CheckOrgLevel(nBillingOrgIDAd,nCalledOrgIDAd,nParentID2);
    nLevel3= CheckOrgLevel(nBillingOrgIDAd,nCalledOrgVisit,nParentID3);

    char sKey[32] ={0};
    sprintf(sKey,"%d_%d_%d_%d_%d",nParentID1,nParentID2,nParentID3,0,pEventSection->m_iCallTypeID);

    LongTypeRule* pLongRule = 0;
    if(!m_plstLongTypeRule->get(sKey,&pLongRule))
    {
        Log::log(0,"无法完全匹配长途类型");
        //assert(false);
    }
    
    int nLevelTmp = 0;
    int nOrgIDTmp =0;
    if(nLevel1<nLevel2)
    {
        if(nLevel1 >nLevel3){
            nLevelTmp = nLevel3;
            nOrgIDTmp = nParentID3;
        }else{
            nLevelTmp = nLevel1 ;
            nOrgIDTmp =nParentID1;
        }
    }else
    {
        if(nLevel2>nLevel3){
            nLevelTmp = nLevel3;
            nOrgIDTmp = nParentID3;
        }else{
            nLevelTmp = nLevel2 ;
            nOrgIDTmp =nParentID2;
        }
    }

    
    while(nParentID2!=-1)
    {
        nLevelTmp = m_pOrgMgr->getLevel(nParentID2);
        sprintf(sKey,"%d",nLevelTmp);
        if(m_plstLongTypeRule->get(sKey,&pLongRule))
        {
            while(pLongRule!=0)
            {
                if((pLongRule->nCalledOrgIDAd == nBillingOrgIDAd)
                    ||m_pOrgMgr->getBelong(nBillingOrgIDAd,pLongRule->nCalledOrgIDAd))
                {
                    if((pLongRule->nCalledOrgIDVisit ==nBillingOrgIDVisit)
                        ||m_pOrgMgr->getBelong(nBillingOrgIDVisit,pLongRule->nCalledOrgIDVisit))
                    {
                        if((nCalledOrgIDAd ==pLongRule->nCallingOrgIDAd)
                            ||m_pOrgMgr->getBelong(nCalledOrgIDAd,pLongRule->nCallingOrgIDAd))
                        {
                            if((nCalledOrgVisit==pLongRule->nCallingOrgIDVisit)
                                ||m_pOrgMgr->getBelong(nCalledOrgVisit,pLongRule->nCallingOrgIDVisit))
                            {
                                if(pLongRule->nAllSame==0)
                                    return pLongRule->nLongTypeID;
                                else
                                {
                                    pLongRule = pLongRule->pNextV;
                                }
                            }
                        }
                    }
                }
            }
        }else
            nParentID2 = m_pOrgMgr->getParentID(nParentID2);        
    }
    assert(false);



    return 0;
}


bool EventTypeMgrEx_B::getHLRInfoByMobileNBR(char* _sMobileNBR, char *_sAreaCode, char* _sHcode)
{
    int iPos;
    char sTempNBR[32];
    HLR obHLR;
    
    if(_sMobileNBR[0]=='0')
        strcpy(sTempNBR,sTempNBR+1);
    

    for (iPos=m_iMaxSegCodeLen; iPos>=m_iMinSegCodeLen; iPos-- ) {
        sTempNBR[iPos] = 0;
        if (m_pHLR->get(sTempNBR,&obHLR)) {
            strcpy(_sHcode, sTempNBR);
            strcpy(_sAreaCode, obHLR.m_sAreaCode);
            return true;
        }
    }
    
    //未匹配到号段
    return false;
}

void EventTypeMgrEx_B::loadHLR()
{
    
    m_pHLR = new KeyTree<HLR>;
    if (!m_pHLR) THROW(MBC_FormatMethod+4);
    
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL("select code_segment,area_code from b_hlr");
    qry.open();
    
    while (qry.next()) {
        HLR oHlr;
        strcpy(oHlr.m_sAreaCode, qry.field("area_code").asString());
        m_pHLR->add(qry.field("code_segment").asString(), oHlr);
    }

    qry.close();
    qry.setSQL("select max(length(code_segment)) max_len,min(length(code_segment)) min_len from b_hlr");
    qry.open();
    if (qry.next()) {
        m_iMaxSegCodeLen = qry.field("max_len").asInteger();
        m_iMinSegCodeLen = qry.field("min_len").asInteger();
    }
    else {
        m_iMaxSegCodeLen = 0;
        m_iMinSegCodeLen = 0;
    }
    
    qry.close();
}


OrgMgrEx::OrgMgrEx(char* pOrgTable)
{
    if (!m_bUploaded)
                load(pOrgTable);
}

OrgMgrEx::~OrgMgrEx(void)
{
}

void OrgMgrEx::load(char* pOrgTable)
{
    if(0== pOrgTable)
        OrgMgr::load();
    int count, i;

    unload ();

    TOCIQuery qry(Environment::getDBConn());

    qry.setSQL ("select count(*) from org");
    qry.open ();

    qry.next ();
    count = qry.field(0).asInteger () + 16;
    qry.close ();

    m_poOrgList = new Org[count];
    m_poOrgIndex = new HashList<Org *> ((count>>1) | 1);

    if (!m_poOrgList || !m_poOrgIndex)
        THROW(MBC_OrgMgr+1);

    qry.setSQL ("SELECT ORG_ID, nvl(PARENT_ORG_ID, -2), nvl(ORG_LEVEL_ID, -2) FROM ORG");
    qry.open ();

    i = 0;

    while (qry.next ()) {
        if (i==count) THROW (MBC_OrgMgr+2);

        m_poOrgList[i].m_iOrgID = qry.field(0).asInteger ();
        m_poOrgList[i].m_iParentID = qry.field(1).asInteger ();
        m_poOrgList[i].m_iLevel = qry.field(2).asInteger ();

        m_poOrgIndex->add (m_poOrgList[i].m_iOrgID, &(m_poOrgList[i]));

        i++;
    }
    qry.close ();

    while (i--) {
        m_poOrgIndex->get (m_poOrgList[i].m_iParentID, &(m_poOrgList[i].m_poParent));
    }

    m_bUploaded = true;
}

