
#include "LocalCacheCtl.h"
#include "TOCIQuery.h"
#include "Log.h"
#include "Environment.h"
#include <stdio.h>


bool LocalCacheCtl::check(){

    if ( !m_bAttached ) {
        return false;
    }

    return true;
}

void LocalCacheCtl::exportAll(){

    exportEvent(LOCAL_CACHE_TABLE_WAIT_EVENT,0);          
    exportEvent(LOCAL_CACHE_TABLE_CACHE_EVENT,1);         
    exportEventPrice(LOCAL_CACHE_TABLE_EVENT_PRICE);         
    exportEventAccu(LOCAL_CACHE_TABLE_EVENT_AGGR);          
    exportDisctAggr(LOCAL_CACHE_TABLE_ACCT_DISCT_AGGR);     
    exportDisctDetail(LOCAL_CACHE_TABLE_ACCT_DISCT);          
    exportAcctItem(LOCAL_CACHE_TABLE_ACCT_ITEM_AGGR);
    exportLockKey(LOCAL_CACHE_TABLE_LOCK_KEY_DATA);
    exportMiddleInfo(LOCAL_CACHE_TABLE_MIDDLE_INFO);
    exportMiddleData(LOCAL_CACHE_TABLE_MIDDLE_DATA);
    exportAddress(LOCAL_CACHE_TABLE_ADDRESS_DATA);
}

void LocalCacheCtl::exportEvent(const char *sTableName, const int &iWho){


    vector<StdEvent > res;
    vector<StdEvent>::iterator itr;
    int count = 0, iRealCount=0, i ;
    int iTotal = 0;
    int iStart = 1;
    char sSQL[4096]={0};

    Log::log(0, "开始导出事件信息到表%s...", sTableName );
    if ( 0 == iWho) {
        count = m_poEventWaitData->getTotal();
    } else {
        count = m_poEventCacheData->getTotal();
    }

    DEFINE_QUERY(qry);
    sprintf(sSQL, " TRUNCATE TABLE %s ", sTableName );
    qry.setSQL(sSQL);
    qry.execute();
    qry.commit();
    qry.close();
    memset(sSQL, 0, sizeof(sSQL) );
    sprintf(sSQL, " INSERT INTO %s(event_id, bill_flow_id,file_id, billing_cycle_id, event_type_id, billing_area_code, "
            " billing_nbr,billing_org_id, calling_area_code, calling_nbr, called_area_code, called_nbr,"
            " start_date, end_date, duration, serv_id, cust_id, product_id, serv_product_id,"
            " billing_record, offer_id1, offer_id2, offer_id3, offer_id4, offer_id5, "
            " charge1, acct_item_type_id1, bill_measure1, measure_type1, acct_id1, offer_instance_id1,"
            " charge2, acct_item_type_id2, bill_measure2, measure_type2, acct_id2, offer_instance_id2, "
            " item_source_id1, item_source_id2, accu_record) "
            " VALUES(:event_id, :bill_flow_id,:file_id, :billing_cycle_id, :event_type_id, :billing_area_code, "
            " :billing_nbr,:billing_org_id, :calling_area_code, :calling_nbr, :called_area_code, :called_nbr,"
            " DECODE(:start_date,NULL, SYSDATE, TO_DATE(:start_date,'yyyy-mm-dd hh24:mi:ss')), "
            " DECODE(:end_date,NULL, SYSDATE, TO_DATE(:end_date,'yyyy-mm-dd hh24:mi:ss')), "
            " :duration, :serv_id, :cust_id, :product_id, :serv_product_id,"
            " :billing_record, :offer_id1, :offer_id2, :offer_id3, :offer_id4, :offer_id5, "
            " :charge1, :acct_item_type_id1, :bill_measure1, :measure_type1, :acct_id1, :offer_instance_id1,"
            " :charge2, :acct_item_type_id2, :bill_measure2, :measure_type2, :acct_id2, :offer_instance_id2, "
            " :item_source_id1, :item_source_id2, :accu_record ) ", 
            sTableName);
    qry.setSQL(sSQL);

    try {
        while (count) {
            i = count > batchnum ? batchnum : count;
            res.clear();
            iRealCount = 0;
            for (int j = iStart; j< iStart + i; ++j ) {
                if ( 0 == iWho ) {
                    if ( m_poEventWait[j].m_oData.m_lEventID ) {
                        res.push_back(m_poEventWait[j].m_oData);
                        iRealCount++;
                    }
                } else {
                    if ( m_poEventCache[j].m_lEventID ) {
                        res.push_back(m_poEventCache[j]);
                        iRealCount++;
                    }
                }
            }
            iStart += i ;

            if ( 0 == iRealCount ) {
                count -= i;
                continue;
            }
            itr = res.begin();

            qry.setParamArray("event_id", &(itr->m_lEventID), sizeof(StdEvent));
            qry.setParamArray("bill_flow_id", &(itr->m_iFlowID), sizeof(StdEvent));
            qry.setParamArray("file_id", &(itr->m_iFileID), sizeof(StdEvent));
            qry.setParamArray("billing_cycle_id", &(itr->m_iBillingCycleID), sizeof(StdEvent));
            qry.setParamArray("event_type_id", &(itr->m_iEventTypeID), sizeof(StdEvent));
            qry.setParamArray("billing_area_code", (char**)&(itr->m_sBillingAreaCode), sizeof(StdEvent), 
                              sizeof(itr->m_sBillingAreaCode) );
            qry.setParamArray("billing_nbr", (char**)&(itr->m_sBillingNBR), sizeof(StdEvent), 
                              sizeof(itr->m_sBillingNBR));
            qry.setParamArray("billing_org_id", &(itr->m_iBillingOrgID), sizeof(StdEvent));
            qry.setParamArray("calling_area_code", (char**)&(itr->m_sCallingAreaCode), sizeof(StdEvent),
                              sizeof(itr->m_sCallingAreaCode));
            qry.setParamArray("calling_nbr", (char**)&(itr->m_sCallingNBR), sizeof(StdEvent),
                              sizeof(itr->m_sCallingNBR) );
            qry.setParamArray("called_area_code", (char**)&(itr->m_sCalledAreaCode), sizeof(StdEvent),
                              sizeof(itr->m_sCalledAreaCode) );
            qry.setParamArray("called_nbr", (char**)&(itr->m_sCalledNBR), sizeof(StdEvent), 
                              sizeof(itr->m_sCalledNBR) );
            qry.setParamArray("start_date", (char**)&(itr->m_sStartDate), sizeof(StdEvent),
                              sizeof(itr->m_sStartDate));
            qry.setParamArray("end_date", (char**)&(itr->m_sEndDate), sizeof(StdEvent), 
                              sizeof(itr->m_sEndDate) );
            qry.setParamArray("duration", &(itr->m_lDuration), sizeof(StdEvent));
            qry.setParamArray("serv_id", &(itr->m_iServID), sizeof(StdEvent));
            qry.setParamArray("cust_id", &(itr->m_iFileID), sizeof(StdEvent));
            qry.setParamArray("product_id", &(itr->m_iProdID), sizeof(StdEvent));
            qry.setParamArray("serv_product_id", &(itr->m_iServProdID), sizeof(StdEvent));
            qry.setParamArray("billing_record", (char**)&(itr->m_sBillingRecord), sizeof(StdEvent),
                              sizeof(itr->m_sBillingRecord));
            qry.setParamArray("offer_id1", &(itr->m_lOffers[0]), sizeof(StdEvent));
            qry.setParamArray("offer_id2", &(itr->m_lOffers[1]), sizeof(StdEvent));
            qry.setParamArray("offer_id3", &(itr->m_lOffers[2]), sizeof(StdEvent));
            qry.setParamArray("offer_id4", &(itr->m_lOffers[3]), sizeof(StdEvent));
            qry.setParamArray("offer_id5", &(itr->m_lOffers[4]), sizeof(StdEvent));
            qry.setParamArray("charge1", &(itr->m_oChargeInfo[0].m_lCharge), sizeof(StdEvent));
            qry.setParamArray("acct_item_type_id1", &(itr->m_oChargeInfo[0].m_iAcctItemTypeID), sizeof(StdEvent));
            qry.setParamArray("bill_measure1", &(itr->m_oChargeInfo[0].m_lBillMeasure), sizeof(StdEvent));
            qry.setParamArray("measure_type1", &(itr->m_oChargeInfo[0].m_iMeasureType), sizeof(StdEvent));
            qry.setParamArray("acct_id1", &(itr->m_oChargeInfo[0].m_lAcctID), sizeof(StdEvent));
            qry.setParamArray("offer_instance_id1", &(itr->m_oChargeInfo[0].m_lOfferInstID), sizeof(StdEvent));
            qry.setParamArray("item_source_id1", &(itr->m_oChargeInfo[0].m_iItemSourceID), sizeof(StdEvent));
            qry.setParamArray("charge2", &(itr->m_oChargeInfo[1].m_lCharge), sizeof(StdEvent));
            qry.setParamArray("acct_item_type_id2", &(itr->m_oChargeInfo[1].m_iAcctItemTypeID), sizeof(StdEvent));
            qry.setParamArray("bill_measure2", &(itr->m_oChargeInfo[1].m_lBillMeasure), sizeof(StdEvent));
            qry.setParamArray("measure_type2", &(itr->m_oChargeInfo[1].m_iMeasureType), sizeof(StdEvent));
            qry.setParamArray("acct_id2", &(itr->m_oChargeInfo[1].m_lAcctID), sizeof(StdEvent));
            qry.setParamArray("offer_instance_id2", &(itr->m_oChargeInfo[1].m_lOfferInstID), sizeof(StdEvent));
            qry.setParamArray("item_source_id2", &(itr->m_oChargeInfo[1].m_iItemSourceID), sizeof(StdEvent));
            qry.setParamArray("accu_record", (char**)&(itr->m_sAccuRecord), sizeof(StdEvent), 
                              sizeof(itr->m_sAccuRecord) );

            qry.execute(iRealCount);
            qry.commit();

            count -= i;
            iTotal += iRealCount;
        }
        qry.close();
    } catch (TOCIException &e) {
        qry.close();
        Log::log(0, "\nError Source: %s\nError Message: %s", 
                 e.getErrSrc(), e.getErrMsg() );
    }
    Log::log(0, "共导出事件信息 %d 条", iTotal);

}

void LocalCacheCtl::exportEventAccu(const char *sTableName){

    vector<AccuCacheData > res;
    vector<AccuCacheData>::iterator itr;
    int count = 0, iRealCount=0, i ;
    int iTotal = 0;
    int iStart = 1;
    char sSQL[1024]={0};

    Log::log(0, "开始导出缓冲事件累积量信息到表%s...", sTableName );
    count = m_poAccuCacheData->getTotal();

    DEFINE_QUERY(qry);
    sprintf(sSQL, " TRUNCATE TABLE %s ", sTableName );
    qry.setSQL(sSQL);
    qry.execute();
    qry.commit();
    qry.close();
    memset(sSQL, 0, sizeof(sSQL) );
    sprintf(sSQL, " INSERT INTO %s(accu_inst_id, offer_inst_id, member_id, accu_type_id, serv_id,"
            " billing_cycle_id, cycle_type_id, begin_date, end_date,split_by, cycle_inst_id,"
            " value, add_value, operation, event_id, file_id, event_source_type, org_id, state_date  ) "
            " VALUES( :accu_inst_id,:offer_inst_id, :member_id, :accu_type_id, :serv_id, "
            " :billing_cycle_id, :cycle_type_id, DECODE(:begin_date,0, SYSDATE, TO_DATE(:begin_date,'yyyy-mm-dd hh24:mi:ss') ), "
            " DECODE(:end_date,0, SYSDATE, TO_DATE(:end_date,'yyyy-mm-dd hh24:mi:ss') ), :split_by, :cycle_inst_id ,"
            " :value, :add_value, CHR(:operation), :event_id, "
            " :file_id, :event_source_type, :org_id ,"
            " DECODE(:state_date, NULL, SYSDATE, TO_DATE(:state_date,'yyyy-mm-dd hh24:mi:ss')) ) ", 
            sTableName);
    qry.setSQL(sSQL);

    try {
        while (count) {
            i = count > batchnum ? batchnum : count;
            res.clear();
            iRealCount = 0;
            for (int j = iStart; j< iStart + i; ++j ) {
                if ( m_poAccuCache[j].m_lAccuInstID ) {
                    res.push_back(m_poAccuCache[j]);
                    iRealCount++;
                }
            }
            iStart += i ;

            if ( 0 == iRealCount ) {
                count -= i;
                continue;
            }
            itr = res.begin();

            qry.setParamArray("accu_inst_id", &(itr->m_lAccuInstID), sizeof(AccuCacheData));
            qry.setParamArray("offer_inst_id", &(itr->m_lOfferInstID), sizeof(AccuCacheData));
            qry.setParamArray("member_id", &(itr->m_lMemberID), sizeof(AccuCacheData));
            qry.setParamArray("accu_type_id", &(itr->m_iAccuTypeID), sizeof(AccuCacheData));
            qry.setParamArray("serv_id", &(itr->m_lServID), sizeof(AccuCacheData));
            qry.setParamArray("billing_cycle_id", &(itr->m_iBillingCycleID), sizeof(AccuCacheData));
            qry.setParamArray("cycle_type_id", &(itr->m_lCycleTypeID), sizeof(AccuCacheData));
            qry.setParamArray("begin_date", &(itr->m_lBeginDate), sizeof(AccuCacheData));
            qry.setParamArray("end_date", &(itr->m_lEndDate), sizeof(AccuCacheData));
            qry.setParamArray("split_by", (char**)&(itr->m_sSplitby), sizeof(AccuCacheData),
                              sizeof(itr->m_sSplitby));
            qry.setParamArray("cycle_inst_id", &(itr->m_lCycleInstID), sizeof(AccuCacheData));
            qry.setParamArray("value", &(itr->m_lValue), sizeof(AccuCacheData));
            qry.setParamArray("add_value", &(itr->m_lAddValue), sizeof(AccuCacheData));
            qry.setParamArray("operation", &(itr->m_cOperation), sizeof(AccuCacheData));
            qry.setParamArray("state_date", (char**)&(itr->m_sStateDate), sizeof(AccuCacheData), 
                              sizeof(itr->m_sStateDate) );
            qry.setParamArray("event_id", &(itr->m_lEventID), sizeof(AccuCacheData));
            qry.setParamArray("file_id", &(itr->m_iFileID), sizeof(AccuCacheData));
            qry.setParamArray("event_source_type", &(itr->m_iEventSourceType), sizeof(AccuCacheData));
            qry.setParamArray("org_id", &(itr->m_iOrgID), sizeof(AccuCacheData));

            qry.execute(iRealCount);
            qry.commit();

            count -= i;
            iTotal += iRealCount;
        }
        qry.close();
    } catch (TOCIException &e) {
        qry.close();
        Log::log(0, "\nError Source: %s\nError Message: %s", 
                 e.getErrSrc(), e.getErrMsg() );
    }
    Log::log(0, "共导出缓冲事件累积量信息 %d 条", iTotal);

}
void LocalCacheCtl::exportEventPrice(const char *sTableName){

    vector<EventPriceData > res;
    vector<EventPriceData>::iterator itr;
    int count = 0, iRealCount=0, i ;
    int iTotal = 0;
    int iStart = 1;
    char sSQL[1024]={0};

    Log::log(0, "开始导出缓冲定价轨迹信息到表%s...", sTableName );
    count = m_poEventPriceCacheData->getTotal();

    DEFINE_QUERY(qry);
    sprintf(sSQL, " TRUNCATE TABLE %s ", sTableName );
    qry.setSQL(sSQL);
    qry.execute();
    qry.commit();
    qry.close();
    memset(sSQL, 0, sizeof(sSQL) );
    sprintf(sSQL, " INSERT INTO %s( serv_id, acct_id, offer_inst_id, billing_cycle_id,"
            " pay_measure, item_group_id, rule_type, rule_id, value, duration, event_type_id, "
            " operation ,event_id, file_id, event_source_type, org_id"
            //" ,state_date"
            #ifdef ACCT_ITEM_AGGR_OFFER_ID
            ", offer_id"
            #endif
            ") "
            " VALUES( :serv_id, :acct_id, :offer_inst_id, :billing_cycle_id, :pay_measure,"
            " :item_group_id, :rule_type, :rule_id, :value, :duration,"
            " :event_type_id,  "
            " CHR(:operation), :event_id, :file_id, :event_source_type, :org_id "
            //" , DECODE(:state_date, NULL,sysdate, TO_DATE(:state_date, 'yyyy-mm-dd hh24:mi:ss') )"
            #ifdef ACCT_ITEM_AGGR_OFFER_ID
            ",:offer_id"
            #endif
            ") ", 
            sTableName);
    qry.setSQL(sSQL);

    try {
        while (count) {
            i = count > batchnum ? batchnum : count;
            res.clear();
            iRealCount = 0;
            for (int j = iStart; j< iStart + i; ++j ) {
                if ( m_poEventPriceCache[j].m_oData.lServID ) {
                    res.push_back(m_poEventPriceCache[j].m_oData);
                    iRealCount++;
                }
            }
            iStart += i ;

            if ( 0 == iRealCount ) {
                count -= i;
                continue;
            }
            itr = res.begin();

            qry.setParamArray("serv_id", &(itr->lServID), sizeof(EventPriceData));
            qry.setParamArray("acct_id", &(itr->lAcctID), sizeof(EventPriceData));
            qry.setParamArray("offer_inst_id", &(itr->lOfferInstID), sizeof(EventPriceData));
            qry.setParamArray("billing_cycle_id", &(itr->iBillingCycleID), sizeof(EventPriceData));
            qry.setParamArray("pay_measure", &(itr->iPayMeasure), sizeof(EventPriceData));
            qry.setParamArray("item_group_id", &(itr->iPayItemTypeID), sizeof(EventPriceData));
            qry.setParamArray("rule_type", &(itr->iRuleType), sizeof(EventPriceData));
            qry.setParamArray("rule_id", &(itr->iRuleID), sizeof(EventPriceData));
            qry.setParamArray("value", &(itr->lValue), sizeof(EventPriceData));
            qry.setParamArray("duration", &(itr->lDuration), sizeof(EventPriceData));
            qry.setParamArray("event_type_id", &(itr->iEventTypeID), sizeof(EventPriceData));
            //qry.setParamArray("state_date", (char**)&(itr->sStateDate), sizeof(EventPriceData), 
            //                  sizeof(itr->sStateDate));
            qry.setParamArray("operation", &(itr->cOperation), sizeof(EventPriceData));
            qry.setParamArray("event_id", &(itr->lEventID), sizeof(EventPriceData));
            qry.setParamArray("file_id", &(itr->iFileID), sizeof(EventPriceData));
            qry.setParamArray("event_source_type", &(itr->iEventSourceType), sizeof(EventPriceData));
            qry.setParamArray("org_id", &(itr->iOrgID), sizeof(EventPriceData));
            #ifdef ACCT_ITEM_AGGR_OFFER_ID
            qry.setParamArray("offer_id", &(itr->iOfferID), sizeof(EventPriceData));
            #endif

            qry.execute(iRealCount);
            qry.commit();

            count -= i;
            iTotal += iRealCount;
        }
        qry.close();
    } catch (TOCIException &e) {
        qry.close();
        Log::log(0, "\nError Source: %s\nError Message: %s", 
                 e.getErrSrc(), e.getErrMsg() );
    }
    Log::log(0, "共导出缓冲定价轨迹信息 %d 条", iTotal);
}


void LocalCacheCtl::exportDisctAggr(const char * sTableName){

    vector<DisctCacheData > res;
    vector<DisctCacheData>::iterator itr;
    int count = 0, iRealCount=0, i ;
    int iTotal = 0;
    int iStart = 1;
    char sSQL[1024]={0};

    Log::log(0, "开始导出缓冲优惠汇总信息到表%s...", sTableName );
    count = m_poDisctCacheData->getTotal();

    DEFINE_QUERY(qry);
    sprintf(sSQL, " TRUNCATE TABLE %s ", sTableName );
    qry.setSQL(sSQL);
    qry.execute();
    qry.commit();
    qry.close();
    memset(sSQL, 0, sizeof(sSQL) );
    sprintf(sSQL, " INSERT INTO %s(item_disct_seq, offer_inst_id, billing_cycle_id"
            #ifdef ORG_BILLINGCYCLE
            ",org_billing_cycle_id"
            #endif
            " ,pricing_combine_id,measure_seq, item_group_id, owner_type, owner_id, value, disct_value,"
            " add_value, add_disct_value, operation, file_id, event_source_type, org_id  ) "
            " VALUES( :item_disct_seq, :offer_inst_id, :billing_cycle_id"
            #ifdef ORG_BILLINGCYCLE
            ", :org_billing_cycle_id"
            #endif
            " ,:pricing_combine_id,:measure_seq, :item_group_id, :owner_type, :owner_id, :value, :disct_value,"
            " :add_value, :add_disct_value, CHR(:operation), :file_id, :event_source_type, :org_id ) ", 
            sTableName);
    qry.setSQL(sSQL);

    try {
        while (count) {
            i = count > batchnum ? batchnum : count;
            res.clear();
            iRealCount = 0;
            for (int j = iStart; j< iStart + i; ++j ) {
                if ( m_poDisctCache[j].m_lItemDisctSeq ) {
                    res.push_back(m_poDisctCache[j]);
                    iRealCount++;
                }
            }
            iStart += i ;

            if ( 0 == iRealCount ) {
                count -= i;
                continue;
            }
            itr = res.begin();

            qry.setParamArray("item_disct_seq", &(itr->m_lItemDisctSeq), sizeof(DisctCacheData));
            qry.setParamArray("offer_inst_id", &(itr->m_lOfferInstID), sizeof(DisctCacheData));
            qry.setParamArray("billing_cycle_id", &(itr->m_iBillingCycleID), sizeof(DisctCacheData));
            #ifdef ORG_BILLINGCYCLE
            qry.setParamArray("org_billing_cycle_id", &(itr->m_iOrgBillingCycleID), sizeof(DisctCacheData));
            #endif
            qry.setParamArray("pricing_combine_id", &(itr->m_iCombineID), sizeof(DisctCacheData));
            qry.setParamArray("measure_seq", &(itr->m_iMeasureID), sizeof(DisctCacheData));
            qry.setParamArray("item_group_id", &(itr->m_iItemGroupID), sizeof(DisctCacheData));
            qry.setParamArray("owner_type", &(itr->m_iInType), sizeof(DisctCacheData));
            qry.setParamArray("owner_id", &(itr->m_lInValue), sizeof(DisctCacheData));
            qry.setParamArray("value", &(itr->m_lValue), sizeof(DisctCacheData));
            qry.setParamArray("disct_value", &(itr->m_lDisctValue), sizeof(DisctCacheData));
            qry.setParamArray("add_value", &(itr->m_lAddValue), sizeof(DisctCacheData));
            qry.setParamArray("add_disct_value", &(itr->m_lAddDisctValue), sizeof(DisctCacheData));
            qry.setParamArray("operation", &(itr->m_cOperation), sizeof(DisctCacheData));
            qry.setParamArray("file_id", &(itr->m_iFileID), sizeof(DisctCacheData));
            qry.setParamArray("event_source_type", &(itr->m_iEventSourceType), sizeof(DisctCacheData));
            qry.setParamArray("org_id", &(itr->m_iOrgID), sizeof(DisctCacheData));

            qry.execute(iRealCount);
            qry.commit();

            count -= i;
            iTotal += iRealCount;
        }
        qry.close();
    } catch (TOCIException &e) {
        qry.close();
        Log::log(0, "\nError Source: %s\nError Message: %s", 
                 e.getErrSrc(), e.getErrMsg() );
    }
    Log::log(0, "共导出缓冲优惠汇总信息 %d 条", iTotal);


    return;
}


void LocalCacheCtl::exportDisctDetail(const char * sTableName){

    vector<DisctDetailCacheData > res;
    vector<DisctDetailCacheData>::iterator itr;
    int count = 0, iRealCount=0, i ;
    int iTotal = 0;
    int iStart = 1;
    char sSQL[1024]={0};

    Log::log(0, "开始导出缓冲优惠明细信息到表%s...", sTableName );
    count = m_poDisctDetailCacheData->getTotal();

    DEFINE_QUERY(qry);
    sprintf(sSQL, " TRUNCATE TABLE %s ", sTableName );
    qry.setSQL(sSQL);
    qry.execute();
    qry.commit();
    qry.close();
    memset(sSQL, 0, sizeof(sSQL) );
    sprintf(sSQL, " INSERT INTO %s(item_disct_seq,item_detail_seq, serv_id, billing_cycle_id,acct_item_type_id,"
            " value, disct_value, add_value, add_disct_value, operation, "
            " event_id, file_id, event_source_type, org_id  ) "
            " VALUES( :item_disct_seq,:item_detail_seq, :serv_id, :billing_cycle_id, :acct_item_type_id,"
            " :value, :disct_value, :add_value, :add_disct_value, CHR(:operation), "
            " :event_id, :file_id, :event_source_type, :org_id ) ", 
            sTableName);
    qry.setSQL(sSQL);

    try {
        while (count) {
            i = count > batchnum ? batchnum : count;
            res.clear();
            iRealCount = 0;
            for (int j = iStart; j< iStart + i; ++j ) {
                if ( m_poDisctDetailCache[j].m_lItemDisctSeq ) {
                    res.push_back(m_poDisctDetailCache[j]);
                    iRealCount++;
                }
            }
            iStart += i ;

            if ( 0 == iRealCount ) {
                count -= i;
                continue;
            }
            itr = res.begin();

            qry.setParamArray("item_disct_seq", &(itr->m_lItemDisctSeq), sizeof(DisctDetailCacheData));
            qry.setParamArray("item_detail_seq", &(itr->m_lItemDetailSeq), sizeof(DisctDetailCacheData));
            qry.setParamArray("serv_id", &(itr->m_lServID), sizeof(DisctDetailCacheData));
            qry.setParamArray("billing_cycle_id", &(itr->m_iBillingCycleID), sizeof(DisctDetailCacheData));
            qry.setParamArray("acct_item_type_id", &(itr->m_iAcctItemTypeID), sizeof(DisctDetailCacheData));
            qry.setParamArray("value", &(itr->m_lValue), sizeof(DisctDetailCacheData));
            qry.setParamArray("disct_value", &(itr->m_lDisctValue), sizeof(DisctDetailCacheData));
            qry.setParamArray("add_value", &(itr->m_lAddValue), sizeof(DisctDetailCacheData));
            qry.setParamArray("add_disct_value", &(itr->m_lAddDisctValue), sizeof(DisctDetailCacheData));
            qry.setParamArray("operation", &(itr->m_cOperation), sizeof(DisctDetailCacheData));
            qry.setParamArray("event_id", &(itr->m_lEventID), sizeof(DisctDetailCacheData));
            qry.setParamArray("file_id", &(itr->m_iFileID), sizeof(DisctDetailCacheData));
            qry.setParamArray("event_source_type", &(itr->m_iEventSourceType), sizeof(DisctDetailCacheData));
            qry.setParamArray("org_id", &(itr->m_iOrgID), sizeof(DisctDetailCacheData));

            qry.execute(iRealCount);
            qry.commit();

            count -= i;
            iTotal += iRealCount;
        }
        qry.close();
    } catch (TOCIException &e) {
        qry.close();
        Log::log(0, "\nError Source: %s\nError Message: %s", 
                 e.getErrSrc(), e.getErrMsg() );
    }
    Log::log(0, "共导出缓冲优惠明细信息 %d 条", iTotal);


    return;
}

void LocalCacheCtl::exportAcctItem(const char * sTableName){

    vector<AcctItemAddAggrData > res;
    vector<AcctItemAddAggrData>::iterator itr;
    int count = 0, iRealCount=0, i ;
    int iTotal = 0;
    int iStart = 1;
    char sSQL[1024]={0};

    Log::log(0, "开始导出缓冲总账信息到表%s...", sTableName );
    count = m_poAcctItemCacheData->getTotal();

    DEFINE_QUERY(qry);
    sprintf(sSQL, " TRUNCATE TABLE %s ", sTableName );
    qry.setSQL(sSQL);
    qry.execute();
    qry.commit();
    qry.close();
    memset(sSQL, 0, sizeof(sSQL) );
    sprintf(sSQL, " INSERT INTO %s(acct_item_id, offer_inst_id, serv_id, acct_id, item_source_id, "
            " item_class_id, billing_cycle_id,acct_item_type_id, charge, measure_type, "
            " pricing_combine_id, state_date, operation, event_id, file_id, event_source_type, org_id "
            #ifdef ACCT_ITEM_AGGR_OFFER_ID
            ", offer_id"
            #endif
            #ifdef ORG_BILLINGCYCLE
            ", org_billing_cycle_id"
            #endif
            ") "
            " VALUES( :acct_item_id,:offer_inst_id, :serv_id, :acct_id, :item_source_id, :item_class_id, "
            " :billing_cycle_id, :acct_item_type_id, :charge, :measure_type, :pricing_combine_id,"
            " TO_DATE(NVL(:state_date, SYSDATE), 'yyyy-mm-dd hh24:mi:ss'), CHR(:operation), "
            " :event_id, :file_id, :event_source_type, :org_id "
            #ifdef ACCT_ITEM_AGGR_OFFER_ID
            ",:offer_id"
            #endif
            #ifdef ORG_BILLINGCYCLE
            ", :org_billing_cycle_id"
            #endif
            ") ", 
            sTableName);
    qry.setSQL(sSQL);

    try {
        while (count) {
            i = count > batchnum ? batchnum : count;
            res.clear();
            iRealCount = 0;
            for (int j = iStart; j< iStart + i; ++j ) {
                if ( m_poAcctItemCache[j].m_oData.lAcctItemID ) {
                    res.push_back(m_poAcctItemCache[j].m_oData);
                    iRealCount++;
                }
            }
            iStart += i ;

            if ( 0 == iRealCount ) {
                count -= i;
                continue;
            }
            itr = res.begin();

            qry.setParamArray("acct_item_id", &(itr->lAcctItemID), sizeof(AcctItemAddAggrData));
            qry.setParamArray("offer_inst_id", &(itr->lOfferInstID), sizeof(AcctItemAddAggrData));
            qry.setParamArray("serv_id", &(itr->lServID), sizeof(AcctItemAddAggrData));
            qry.setParamArray("acct_id", &(itr->lAcctID), sizeof(AcctItemAddAggrData));
            qry.setParamArray("item_source_id", &(itr->iItemSourceID), sizeof(AcctItemAddAggrData));
            qry.setParamArray("item_class_id", &(itr->iItemClassID), sizeof(AcctItemAddAggrData));
            qry.setParamArray("billing_cycle_id", &(itr->iBillingCycleID), sizeof(AcctItemAddAggrData));
            qry.setParamArray("acct_item_type_id", &(itr->iPayItemTypeID), sizeof(AcctItemAddAggrData));
            qry.setParamArray("charge", &(itr->lCharge), sizeof(AcctItemAddAggrData));
            qry.setParamArray("measure_type", &(itr->iPayMeasure), sizeof(AcctItemAddAggrData));
            qry.setParamArray("pricing_combine_id", &(itr->iCombineID), sizeof(AcctItemAddAggrData));
            qry.setParamArray("state_date", (char**)&(itr->sStateDate), sizeof(AcctItemAddAggrData), 
                              sizeof((itr->sStateDate)));
            qry.setParamArray("operation", &(itr->cOperation), sizeof(AcctItemAddAggrData));
            qry.setParamArray("event_id", &(itr->lEventID), sizeof(AcctItemAddAggrData));
            qry.setParamArray("file_id", &(itr->iFileID), sizeof(AcctItemAddAggrData));
            qry.setParamArray("event_source_type", &(itr->iEventSourceType), sizeof(AcctItemAddAggrData));
            qry.setParamArray("org_id", &(itr->iOrgID), sizeof(AcctItemAddAggrData));
            #ifdef ACCT_ITEM_AGGR_OFFER_ID
            qry.setParamArray("offer_id", &(itr->iOfferID), sizeof(AcctItemAddAggrData));
            #endif
            #ifdef ORG_BILLINGCYCLE
            qry.setParamArray("org_billing_cycle_id", &(itr->m_iOrgBillingCycleID), sizeof(AcctItemAddAggrData));
            #endif

            qry.execute(iRealCount);
            qry.commit();

            count -= i;
            iTotal += iRealCount;
        }
        qry.close();
    } catch (TOCIException &e) {
        qry.close();
        Log::log(0, "\nError Source: %s\nError Message: %s", 
                 e.getErrSrc(), e.getErrMsg() );
    }
    Log::log(0, "共导出缓冲总账信息 %d 条", iTotal);


    return;

}

void LocalCacheCtl::exportLockKey(const char * sTableName){

    vector<LockKeyData > res;
    vector<LockKeyData>::iterator itr;
    int count = 0, iRealCount=0, i ;
    int iTotal = 0;
    int iStart = 1;
    char sSQL[1024]={0};

    Log::log(0, "开始导出锁关键字信息到表%s...", sTableName );
    count = m_poKeyCacheData->getTotal();

    DEFINE_QUERY(qry);
    sprintf(sSQL, " TRUNCATE TABLE %s ", sTableName );
    qry.setSQL(sSQL);
    qry.execute();
    qry.commit();
    qry.close();
    memset(sSQL, 0, sizeof(sSQL) );
    sprintf(sSQL, " INSERT INTO %s(key , next_offset, other_offset, offset1, other_offset1) "
            " VALUES( :key , :next_offset, :other_offset, :offset1, :other_offset1) ", 
            sTableName);
    qry.setSQL(sSQL);

    try {
        while (count) {
            i = count > batchnum ? batchnum : count;
            res.clear();
            iRealCount = 0;
            for (int j = iStart; j< iStart + i; ++j ) {
                if ( m_poKeyCache[j].m_lKey ) {
                    res.push_back(m_poKeyCache[j]);
                    iRealCount++;
                }
            }
            iStart += i ;

            if ( 0 == iRealCount ) {
                count -= i;
                continue;
            }
            itr = res.begin();

            qry.setParamArray("key", &(itr->m_lKey), sizeof(LockKeyData));
            qry.setParamArray("next_offset", (int *)&(itr->m_iNextOffset), sizeof(LockKeyData));
            qry.setParamArray("other_offset",(int*) &(itr->m_iOtherOffset), sizeof(LockKeyData));
            qry.setParamArray("offset1",(int*) &(itr->m_iOffset1), sizeof(LockKeyData));
            qry.setParamArray("other_offset1",(int*) &(itr->m_iOtherOffset1), sizeof(LockKeyData));

            qry.execute(iRealCount);
            qry.commit();

            count -= i;
            iTotal += iRealCount;
        }
        qry.close();
    } catch (TOCIException &e) {
        qry.close();
        Log::log(0, "\nError Source: %s\nError Message: %s", 
                 e.getErrSrc(), e.getErrMsg() );
    }
    Log::log(0, "共导出缓冲锁关键字信息 %d 条", iTotal);


    return;

}
void LocalCacheCtl::exportMiddleInfo(const char * sTableName){

    vector<MiddleInfoData > res;
    vector<MiddleInfoData>::iterator itr;
    int count = 0, iRealCount=0, i ;
    int iTotal = 0;
    int iStart = 1;
    char sSQL[1024]={0};

    Log::log(0, "开始导出缓冲中间信息到表%s...", sTableName );
    count = m_poMiddleInfoData->getTotal();

    DEFINE_QUERY(qry);
    sprintf(sSQL, " TRUNCATE TABLE %s ", sTableName );
    qry.setSQL(sSQL);
    qry.execute();
    qry.commit();
    qry.close();
    memset(sSQL, 0, sizeof(sSQL) );
    sprintf(sSQL, " INSERT INTO %s(process_id , data_begin_offset, data_end_offset, wait_offset) "
            " VALUES( :process_id , :data_begin_offset, :data_end_offset, :wait_offset) ", 
            sTableName);
    qry.setSQL(sSQL);

    try {
        while (count) {
            i = count > batchnum ? batchnum : count;
            res.clear();
            iRealCount = 0;
            for (int j = iStart; j< iStart + i; ++j ) {
                if ( m_poMiddleInfo[j].m_iProcessID ) {
                    res.push_back(m_poMiddleInfo[j]);
                    iRealCount++;
                }
            }
            iStart += i ;

            if ( 0 == iRealCount ) {
                count -= i;
                continue;
            }
            itr = res.begin();

            qry.setParamArray("process_id",(int*) &(itr->m_iProcessID), sizeof(MiddleInfoData));
            qry.setParamArray("data_begin_offset",(int*) &(itr->m_iDataBegOffset), sizeof(MiddleInfoData));
            qry.setParamArray("data_end_offset",(int*) &(itr->m_iDataEndOffset), sizeof(MiddleInfoData));
            qry.setParamArray("wait_offset", (int*)&(itr->m_iWaitOffset), sizeof(MiddleInfoData));

            qry.execute(iRealCount);
            qry.commit();

            count -= i;
            iTotal += iRealCount;
        }
        qry.close();
    } catch (TOCIException &e) {
        qry.close();
        Log::log(0, "\nError Source: %s\nError Message: %s", 
                 e.getErrSrc(), e.getErrMsg() );
    }
    Log::log(0, "共导出缓冲中间信息 %d 条", iTotal);


    return;

}
void LocalCacheCtl::exportMiddleData(const char * sTableName){

    vector<MiddleCacheData > res;
    vector<MiddleCacheData>::iterator itr;
    int count = 0, iRealCount=0, i ;
    int iTotal = 0;
    int iStart = 1;
    char sSQL[1024]={0};

    Log::log(0, "开始导出缓冲中间数据到表%s...", sTableName );
    count = m_poMiddleInfoData->getTotal();

    DEFINE_QUERY(qry);
    sprintf(sSQL, " TRUNCATE TABLE %s ", sTableName );
    qry.setSQL(sSQL);
    qry.execute();
    qry.commit();
    qry.close();
    memset(sSQL, 0, sizeof(sSQL) );
    sprintf(sSQL, " INSERT INTO %s(wait_event_offset , address_offset, next_offset, other_offset, "
            " offset1, other_offset1,) "
            " VALUES( :wait_event_offset , :address_offset, :next_offset, :other_offset, "
            " :offset1, :other_offset1, ) ", 
            sTableName);
    qry.setSQL(sSQL);

    try {
        while (count) {
            i = count > batchnum ? batchnum : count;
            res.clear();
            iRealCount = 0;
            for (int j = iStart; j< iStart + i; ++j ) {
                if ( m_poMiddleCache[j].m_iWaitEventOffset ) {
                    res.push_back(m_poMiddleCache[j]);
                    iRealCount++;
                }
            }
            iStart += i ;

            if ( 0 == iRealCount ) {
                count -= i;
                continue;
            }
            itr = res.begin();

            qry.setParamArray("wait_event_offset", (int*)&(itr->m_iWaitEventOffset), sizeof(MiddleCacheData));
            qry.setParamArray("address_offset", (int*)&(itr->m_iAddressOffset), sizeof(MiddleCacheData));
            qry.setParamArray("next_offset", (int*)&(itr->m_iNextOffset), sizeof(MiddleCacheData));
            qry.setParamArray("other_offset", (int*)&(itr->m_iOtherOffset), sizeof(MiddleCacheData));
            qry.setParamArray("offset1", (int*)&(itr->m_iOffset1), sizeof(MiddleCacheData));
            qry.setParamArray("other_offset1", (int*)&(itr->m_iOtherOffset1), sizeof(MiddleCacheData));

            qry.execute(iRealCount);
            qry.commit();

            count -= i;
            iTotal += iRealCount;
        }
        qry.close();
    } catch (TOCIException &e) {
        qry.close();
        Log::log(0, "\nError Source: %s\nError Message: %s", 
                 e.getErrSrc(), e.getErrMsg() );
    }
    Log::log(0, "共导出缓冲中间数据 %d 条", iTotal);

    return;
}
void LocalCacheCtl::exportAddress(const char * sTableName){


    vector<AddressCacheData > res;
    vector<AddressCacheData>::iterator itr;
    int count = 0, iRealCount=0, i ;
    int iTotal = 0;
    int iStart = 1;
    char sSQL[1024]={0};

    Log::log(0, "开始导出地址索引信息到表%s...", sTableName );
    count = m_poAddressCacheData->getTotal();

    DEFINE_QUERY(qry);
    sprintf(sSQL, " TRUNCATE TABLE %s ", sTableName );
    qry.setSQL(sSQL);
    qry.execute();
    qry.commit();
    qry.close();
    memset(sSQL, 0, sizeof(sSQL) );
    sprintf(sSQL, " INSERT INTO %s(event_id, event_offset , event_price_offset, event_accu_offset, disct_aggr_offset, "
            " acct_item_offset, lock_key_offset, begin_date, send, type, process_id) "
            " VALUES( :event_id, :event_offset , :event_price_offset, :event_accu_offset, :disct_aggr_offset, "
            " :acct_item_offset, :lock_key_offset, "
            " DECODE(:begin_date,0, SYSDATE, TO_DATE(:begin_date,'yyyy-mm-dd hh24:mi:ss') ), :send, :type, "
            " :process_id) ", 
            sTableName);
    qry.setSQL(sSQL);

    try {
        while (count) {
            i = count > batchnum ? batchnum : count;
            res.clear();
            iRealCount = 0;
            for (int j = iStart; j< iStart + i; ++j ) {
                if ( m_poAddressCache[j].m_lEventID ) {
                    res.push_back(m_poAddressCache[j]);
                    iRealCount++;
                }
            }
            iStart += i ;

            if ( 0 == iRealCount ) {
                count -= i;
                continue;
            }
            itr = res.begin();

            qry.setParamArray("event_id", &(itr->m_lEventID), sizeof(AddressCacheData));
            qry.setParamArray("event_offset", (int*)&(itr->m_iStdEventOffset), sizeof(AddressCacheData));
            qry.setParamArray("event_price_offset", (int*)&(itr->m_iEventPriceOffset), sizeof(AddressCacheData));
            qry.setParamArray("event_accu_offset", (int*)&(itr->m_iEventAccuOffset), sizeof(AddressCacheData));
            qry.setParamArray("disct_aggr_offset", (int*)&(itr->m_iDisctAggrOffset), sizeof(AddressCacheData));
            qry.setParamArray("acct_item_offset", (int*)&(itr->m_iAcctItemOffset), sizeof(AddressCacheData));
            qry.setParamArray("lock_key_offset", (int*)&(itr->m_iLockKeyOffset), sizeof(AddressCacheData));
            qry.setParamArray("begin_date", &(itr->m_lBeginDate), sizeof(AddressCacheData));
            qry.setParamArray("send", (int*)&(itr->m_iSend), sizeof(AddressCacheData));
            qry.setParamArray("type", (int*)&(itr->m_iType), sizeof(AddressCacheData));
            qry.setParamArray("process_id", (int*)&(itr->m_iProcessID), sizeof(AddressCacheData));
            qry.execute(iRealCount);
            qry.commit();

            count -= i;
            iTotal += iRealCount;
        }
        qry.close();
    } catch (TOCIException &e) {
        qry.close();
        Log::log(0, "\nError Source: %s\nError Message: %s", 
                 e.getErrSrc(), e.getErrMsg() );
    }
    Log::log(0, "共导出地址索引信息 %d 条", iTotal);

    return;
}

void printUsage(const char * cmd){
    printf(" Usage: 缓冲区导出工具 %s\n"
           "\t-a:导出所有缓冲数据, 表名分别为：\n"
           "\t\t等待事件%s\n"
           "\t\t缓冲事件%s\n"
           "\t\t事件累积量%s\n"
           "\t\t定价轨迹%s\n"
           "\t\t优惠汇总%s\n"
           "\t\t优惠明细%s\n"
           "\t\t总账%s\n"
           "\t\t锁信息%s\n"
           "\t\t中间信息\n"
           "\t\t中间数据\n"
           "\t\t地址索引信息\n"
           "\t-e tablename:导出等待事件信息\n"
           "\t-f tablename:导出缓冲事件信息\n"
           "\t-u tablename:导出缓冲事件累积量信息\n"
           "\t-p tablename:导出缓冲定价轨迹信息\n"
           "\t-d tablename:导出缓冲优惠汇总信息\n"
           "\t-c tablename:导出缓冲优惠明细信息\n"
           "\t-i tablename:导出缓冲总账信息\n"
           "\t-z tablename:导出缓冲锁信息\n"
           "\t-y tablename:导出缓冲中间信息\n"
           "\t-x tablename:导出缓冲中间数据\n"
           "\t-w tablename:导出地址索引信息\n",
           cmd, 
           LOCAL_CACHE_TABLE_WAIT_EVENT, LOCAL_CACHE_TABLE_CACHE_EVENT, 
           LOCAL_CACHE_TABLE_EVENT_AGGR, LOCAL_CACHE_TABLE_EVENT_PRICE,
           LOCAL_CACHE_TABLE_ACCT_DISCT_AGGR, LOCAL_CACHE_TABLE_ACCT_DISCT,
           LOCAL_CACHE_TABLE_ACCT_ITEM_AGGR,
           LOCAL_CACHE_TABLE_LOCK_KEY_DATA,
           LOCAL_CACHE_TABLE_MIDDLE_INFO,
           LOCAL_CACHE_TABLE_MIDDLE_DATA ,
           LOCAL_CACHE_TABLE_ADDRESS_DATA);

    return;
}

int main(int argc, char ** argv){

    if ( argc < 2) {
        printUsage(argv[0]);
        return 0;
    }

    LocalCacheCtl *control = new LocalCacheCtl();

    if ( !control ) {
        Log::log(0, "申请资源失败!");
        return 0;
    }
    if ( !control->check() ) {
        Log::log(0, "无法连接共享内存，导出失败!");
        return 0;
    }

    for (int iTemp = 1; iTemp < argc; ++iTemp ) {
        switch (argv[iTemp][1] | 0x20 ) {
        case 'a':
            {
                control->exportAll();
                break;
            }
        case 'e':
            {   
                control->exportEvent(argv[++iTemp], 0);
                break;
            }
        case 'f':
            {   
                control->exportEvent(argv[++iTemp], 1);
                break;
            }
        case 'u':
            {   
                control->exportEventAccu(argv[++iTemp]);
                break;
            }
        case 'p':
            {   
                control->exportEventPrice(argv[++iTemp]);
                break;
            }
        case 'd':
            {
                control->exportDisctAggr(argv[++iTemp]);
                break;
            }
        case 'c':
            {   
                control->exportDisctDetail(argv[++iTemp]);
                break;
            }
        case 'i':
            {   
                control->exportAcctItem(argv[++iTemp]);
                break;
            }
        case 'z':
            {   
                control->exportLockKey(argv[++iTemp]);
                break;
            }
        case 'y':
            {   
                control->exportMiddleInfo(argv[++iTemp]);
                break;
            }
        case 'x':
            {   
                control->exportMiddleData(argv[++iTemp]);
                break;
            }
        case 'w':
            {
                control->exportAddress(argv[++iTemp]);
                break;
            }
        default:
            printUsage(argv[0]);
            break;
        }
    }

    delete control;

    return 0;
}



