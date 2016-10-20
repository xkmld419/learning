#include "DisctQuery.h"
#include "Log.h"
#include "Environment.h"
#include <algorithm>
#include <string.h>

DisctQuery::DisctQuery()
{
    memset(m_sDate,0 ,sizeof(m_sDate) );
    m_iBillingCycleID = 0;

    Environment::useUserInfo(USER_INFO_BASE);
    
//    if(!G_PACCTITEMMGR){
//         G_PACCTITEMMGR = new AcctItemMgr ();
//         if (!G_PACCTITEMMGR) THROW(MBC_Environment+1);
//    }
       
    if(!G_PORGMGR){
         G_PORGMGR = new OrgMgr ();
         if (!G_PORGMGR) THROW(MBC_Environment+1);
    }
    m_poOfferInstBuf = new OfferInsQO[MAX_OFFER_INSTANCE_NUMBER];
    if(!m_poOfferInstBuf) throw 0;
    m_poProdOfferDisctAggr = new ProdOfferDisctAggr();
    if(!m_poProdOfferDisctAggr) throw 0;
    m_poBCM = new BillingCycleMgr ();
    if(!m_poBCM) throw 0;


    load();

}

void DisctQuery::load()
{
    
    this->loadItemName("product_offer",
        "select offer_id,decode(nvl(offer_query_name,'no'),'no',offer_name,offer_query_name) the_name"
            " from product_offer",
         &m_poOfferNameList,
         &m_poOfferNameIndex);

    this->loadItemName("acct_item_type",
        "select acct_item_type_id, name from acct_item_type",
         &m_poAcctItemNameList,
         &m_poAcctItemNameIndex);
 
    this->loadItemName("b_acct_item_group",
        "select item_group_id , item_group_name  from b_acct_item_group",
         &m_poAcctGroupNameList,
         &m_poAcctGroupNameIndex);

}

void DisctQuery::loadItemName(const char * sTableName, const char * sLoadSql,  
    ItemNameData **ppList, HashList<ItemNameData *> **ppIndex)
{      
    int iCount = 0;
    int m,n;
    char sSqlcode[512];

    TOCIQuery qry(Environment::getDBConn());
    
    memset(sSqlcode,0,sizeof(sSqlcode));
    sprintf (sSqlcode, "select count(1) from %s",sTableName);
    qry.setSQL (sSqlcode);
    qry.open ();
    if(qry.next ()) {
        iCount = qry.field(0).asInteger();
    }
    qry.close ();
    qry.commit();
    iCount = iCount+2;

    ItemNameData * pTemp;
    int i = 0;
    *ppList = new ItemNameData[iCount];
    memset(*ppList,0,(sizeof(ItemNameData))*iCount);
    if(!(*ppList) )
    {
        Log::log(0,"load中 new ItemNameData[iCount] 审请内存空间失败1！");
        THROW(1);
    }
    *ppIndex = new HashList<ItemNameData *>(iCount>>1);
    if(!(*ppIndex) )
    {
        Log::log(0,"load中 new HashList<ItemNameData *> 审请内存空间失败1！");
        THROW(1);
    }


    qry.setSQL (sLoadSql);
    qry.open ();
    while (qry.next ())
    {
        pTemp = *ppList+i;
        pTemp->m_iKeyID = qry.field(0).asInteger(); 
        strncpy(pTemp->m_sName,qry.field(1).asString(),sizeof(pTemp->m_sName)-1);
        (*ppIndex)->add(pTemp->m_iKeyID, pTemp);
        i++;
    }
    qry.close ();
    qry.commit();



}

DisctQuery::~DisctQuery()
{
    if(m_poOfferInstBuf){
        delete [] m_poOfferInstBuf;
        m_poOfferInstBuf = NULL;
    }

    if(m_poProdOfferDisctAggr){
        delete m_poProdOfferDisctAggr;
        m_poProdOfferDisctAggr = NULL;
    }
    if(m_poBCM){
        delete m_poBCM;
        m_poBCM = NULL;
    }

    if(m_poOfferNameIndex){ delete m_poOfferNameIndex; m_poOfferNameIndex=NULL;}
    if(m_poAcctItemNameIndex){ delete m_poAcctItemNameIndex; m_poAcctItemNameIndex=NULL;}
    if(m_poAcctGroupNameIndex){ delete m_poAcctGroupNameIndex; m_poAcctGroupNameIndex=NULL;}
        
    if(m_poOfferNameList){ delete [] m_poOfferNameList; m_poOfferNameList=NULL;}
    if(m_poAcctItemNameList){ delete [] m_poAcctItemNameList; m_poAcctItemNameList=NULL;}
    if(m_poAcctGroupNameList){ delete [] m_poAcctGroupNameList; m_poAcctGroupNameList=NULL;}

}




int DisctQuery::query(vector<DisctQueryResult> &vResult,
            long lServID, char *sDate )
{
    int i, j, r;
    int iTemp= 0,jTemp=0;

    if (!sDate || sDate[0]==0 ) {
        Date d;
        d.getTimeString( m_sDate );
    }else{
        strncpy(m_sDate, sDate, 15);
    }
    
    #ifdef SHM_FILE_USERINFO
    G_SHMFILE;
    #endif

    if (!G_PUSERINFO->getServ(G_SERV, lServID, m_sDate))
        return 0;
    
    m_lServID = lServID;
    
    i = G_PSERV->getOfferIns (m_poOfferInstBuf);
    if (!i) return 0;


    BillingCycle * pCycle = m_poBCM->getOccurBillingCycle(1,m_sDate);
    if (pCycle) {
        m_iBillingCycleID = pCycle->getBillingCycleID();
    }else{
        Log::log(0,"根据时间查询帐期不存在，时间为：%s",m_sDate);
        return -1;
    }

    r = 0;
    
    vector<ProdOfferTotalData> voPOTData;
    DisctQueryResult resDisct;
    for(iTemp=0; iTemp<i; iTemp++){
        voPOTData.clear();
        G_POFFERINSQO = &m_poOfferInstBuf[iTemp];
        m_poProdOfferDisctAggr->getAllByInst(
            m_poOfferInstBuf[iTemp].m_poOfferIns->m_lProdOfferInsID,
            m_iBillingCycleID, 
            voPOTData);
        
        for(jTemp=0; jTemp<voPOTData.size(); jTemp++ ){
            
            if( voPOTData[jTemp].m_bInOutType ){
                ////输入值不用展现
                continue;
            }
            memset( &resDisct, 0, sizeof(DisctQueryResult) );
            voPOTData[jTemp].m_lProdOfferID = m_poOfferInstBuf[iTemp].m_poOfferIns->m_iOfferID;
            this->transRes( &resDisct, &voPOTData[jTemp] );
            vResult.push_back( resDisct );
            r++;
            
        }//
    
    }//
    
    return r;
          
}


void DisctQuery::transRes( DisctQueryResult *pResDisct, ProdOfferTotalData * pData )
{
    static Serv theServ;
    pResDisct->m_lItemDisctSeq = pData->m_lItemDisctSeq;
    pResDisct->m_lValue = pData->m_lValue;
    pResDisct->m_iAcctItemTypeID = pData->m_iAcctItemTypeID;
    pResDisct->m_iItemGroupID = pData->m_iItemGroupID;
    pResDisct->m_iOfferID = pData->m_lProdOfferID;
    
    ItemNameData * pItemName=NULL;

    if( m_poOfferNameIndex->get( pResDisct->m_iOfferID, &pItemName) ){
        strncpy(pResDisct->m_sOfferName, pItemName->m_sName, sizeof(pResDisct->m_sOfferName) );
    }
    
    if( m_poAcctItemNameIndex->get( pResDisct->m_iAcctItemTypeID, &pItemName) ){
        strncpy(pResDisct->m_sAcctItemName, pItemName->m_sName, sizeof(pResDisct->m_sAcctItemName) );
    }
    
    if( m_poAcctGroupNameIndex->get( pResDisct->m_iItemGroupID, &pItemName) ){
        strncpy(pResDisct->m_sAcctGroupName, pItemName->m_sName, sizeof(pResDisct->m_sAcctGroupName) );
    }

    
    if( pData->m_lServID == m_lServID ){
        strncpy(pResDisct->m_sAccNBR, G_PSERV->getAccNBR (25,m_sDate), sizeof(pResDisct->m_sAccNBR) );
    }else{
        if( G_PUSERINFO->getServ(theServ,  pData->m_lServID, m_sDate) ){
            strncpy(pResDisct->m_sAccNBR, theServ.getAccNBR (25,m_sDate),sizeof(pResDisct->m_sAccNBR));
        }
        
    }

//    G_POFFERINSQO->m_poDetailIns->m_iMemberID;
//    pResDisct->m_sElement


}


int DisctQuery::queryDB(vector<DisctQueryResult> &vResult,
            long lServID, char *sDate )
{
    static int iQcnt=0;
    int  iInst, r;
    int  iTemp= 0,jTemp=0;
    bool bTable = false;
    
    if (!sDate || sDate[0]==0 ) {
        Date d;
        d.getTimeString( m_sDate );
    }else{
        strncpy(m_sDate, sDate, 15);
    }
    
    #ifdef SHM_FILE_USERINFO
    G_SHMFILE;
    #endif

    if (!G_PUSERINFO->getServ(G_SERV, lServID, m_sDate))
        return 0;
    
    m_lServID = lServID;
    
    iInst = G_PSERV->getOfferIns (m_poOfferInstBuf);
    if (!iInst) return 0;


    BillingCycle * pCycle = m_poBCM->getOccurBillingCycle(1,m_sDate);
    if (pCycle) {
        m_iBillingCycleID = pCycle->getBillingCycleID();
    }else{
        Log::log(0,"根据时间查询帐期不存在，时间为：%s",m_sDate);
        return -1;
    }

    r = 0;

    
    TableData tabData;
    TOCIQuery qry(Environment::getDBConn());
    
    if(iQcnt % 1024 == 0){
        
        int iTableType=0;
        qry.setSQL("select distinct table_type,nvl(billing_cycle_id,0) billing_cycle_id, "
                    "nvl(org_id,-1) org_id,table_name "
                    "from b_inst_table_list "
                    "where table_type in (85,87)"
                    "order by table_type,billing_cycle_id,org_id desc");
        qry.open();
        while(qry.next()){
            iTableType = qry.field(0).asInteger();
            if(iTableType == 85){
                bTable =  true;
                tabData.m_iBillingCycleID = qry.field(1).asInteger();
                tabData.m_iOrgID = qry.field(2).asInteger();
                strcpy(tabData.m_sTableName , qry.field(3).asString());
                vet85.push_back(tabData);
            }else{
                bTable = true;
                tabData.m_iBillingCycleID = qry.field(1).asInteger();
                tabData.m_iOrgID = qry.field(2).asInteger();
                strcpy(tabData.m_sTableName , qry.field(3).asString());
                vet87.push_back(tabData);
            }
        }
        qry.close();
        qry.commit();
    }
    if(!bTable)
        return -1;

    char sSql[256];

    ProdOfferTotalData tabDisct;
    DisctQueryResult resDisct;
    memset(&tabDisct, 0 , sizeof(ProdOfferTotalData) );
    memset(&resDisct, 0 , sizeof(DisctQueryResult) );
    vector<ProdOfferTotalData> voPOTData;
    vector<ProdOfferTotalData> voSeqData;
    
    HashList<int> ohTableName(128);
    int iTableExists=0;
    bool bGet=false;
    for(iTemp=0; iTemp<iInst; iTemp++){
        G_POFFERINSQO = &m_poOfferInstBuf[iTemp];
        bGet=false;
        ohTableName.clearAll();
        for(int j=0;j<vet87.size();j++){
            tabData =  vet87[j];
            if(tabData.m_iBillingCycleID>0 && tabData.m_iBillingCycleID!=m_iBillingCycleID) continue;

            if( ohTableName.get(tabData.m_sTableName, &iTableExists) )
                continue;
                
            if(tabData.m_iOrgID>0 ){
                if (!G_PORGMGR->getBelong(G_PSERV->getOrgID(),tabData.m_iOrgID)) continue;
            }

            ohTableName.add(tabData.m_sTableName, 1);
                            
            snprintf(sSql, sizeof(sSql),
                " select item_disct_seq,PRICING_COMBINE_ID,MEASURE_SEQ, item_group_id,disct_value from %s"
                " where offer_inst_id = %ld and billing_cycle_id = %d",
                tabData.m_sTableName,
                m_poOfferInstBuf[iTemp].m_poOfferIns->m_lProdOfferInsID,
                m_iBillingCycleID );
            qry.setSQL(sSql);
            qry.open();
            while(qry.next()){
                tabDisct.m_lItemDisctSeq = qry.field(0).asLong();
                tabDisct.m_lProdOfferID = m_poOfferInstBuf[iTemp].m_poOfferIns->m_iOfferID;
                tabDisct.m_iBillingCycleID = m_iBillingCycleID;
                tabDisct.m_iProdOfferCombine = qry.field(1).asInteger();
                tabDisct.m_iPricingMeasure = qry.field(2).asInteger();
                tabDisct.m_iItemGroupID = qry.field(3).asInteger();
                voSeqData.push_back(tabDisct);
                bGet = true;
            }
            qry.close();
            if(bGet)
                break;
        }
    }
    
    ohTableName.clearAll();
    for(int j=0;j<vet85.size();j++){
        tabData =  vet85[j];
        if(tabData.m_iBillingCycleID>0 && tabData.m_iBillingCycleID!=m_iBillingCycleID) continue;

        if( ohTableName.get(tabData.m_sTableName, &iTableExists) )
            continue;
                
        if(tabData.m_iOrgID>0 ){
            if (!G_PORGMGR->getBelong(G_PSERV->getOrgID(),tabData.m_iOrgID)) continue;
        }
        
        ohTableName.add(tabData.m_sTableName, 1);
        
        snprintf(sSql, sizeof(sSql),
            " select item_disct_seq,serv_id,acct_item_type_id,out_charge from %s"
            " where item_disct_seq = :vSeq",
            tabData.m_sTableName );
            
        for(int j2=0; j2<voSeqData.size(); j2++){
            qry.setSQL(sSql);
            qry.setParameter("vSeq", voSeqData[j2].m_lItemDisctSeq);
            qry.open();
            while(qry.next()){
                tabDisct = voSeqData[j2];
                tabDisct.m_lServID = qry.field(1).asLong();
                tabDisct.m_iAcctItemTypeID = qry.field(2).asInteger();
                tabDisct.m_lValue = qry.field(3).asLong();
                tabDisct.m_bInOutType = false;
                voPOTData.push_back(tabDisct);
            }
            qry.close();            
        }
        

    }


    /////////////////

    for(jTemp=0; jTemp<voPOTData.size(); jTemp++ ){
        
        memset( &resDisct, 0, sizeof(DisctQueryResult) );
        this->transRes( &resDisct, &voPOTData[jTemp] );
        vResult.push_back( resDisct );
        r++;
        
    }

    return r;
                                
}

