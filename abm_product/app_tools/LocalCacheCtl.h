
#ifndef _LOCAL_CACHE_CTL_H_
#define _LOCAL_CACHE_CTL_H_

#include "LocalCacheInfo.h"

const int batchnum = 2000;

//default table name define
#define LOCAL_CACHE_TABLE_WAIT_EVENT          "B_CACHE_WAIT_EVENT"
#define LOCAL_CACHE_TABLE_CACHE_EVENT         "B_CACHE_CACHE_EVENT"
#define LOCAL_CACHE_TABLE_EVENT_PRICE         "B_CACHE_EVENT_PRICE"
#define LOCAL_CACHE_TABLE_EVENT_AGGR          "B_CACHE_EVENT_AGGR"
#define LOCAL_CACHE_TABLE_ACCT_DISCT_AGGR     "B_CACHE_ACCT_DISCT_AGGR"
#define LOCAL_CACHE_TABLE_ACCT_DISCT          "B_CACHE_ACCT_DISCT"
#define LOCAL_CACHE_TABLE_ACCT_ITEM_AGGR      "B_CACHE_ACCT_ITEM_AGGR"
#define LOCAL_CACHE_TABLE_LOCK_KEY_DATA       "B_CACHE_LOCK_KEY_DATA"
#define LOCAL_CACHE_TABLE_MIDDLE_INFO         "B_CACHE_MIDDLE_INFO"
#define LOCAL_CACHE_TABLE_MIDDLE_DATA         "B_CACHE_MIDDLE_DATA"
#define LOCAL_CACHE_TABLE_ADDRESS_DATA        "B_CACHE_ADDRESS_DATA"


class LocalCacheCtl : public LocalCacheInfo {
private:
public:

    LocalCacheCtl():LocalCacheInfo() {};
    //export the most important members only
    void exportEvent(const char * sTableName, const int &iWho);
    void exportEventAccu(const char * sTableName);
    void exportEventPrice(const char *sTableName);
    void exportDisctAggr(const char * sTableName);
    void exportDisctDetail(const char * sTableName);
    void exportAcctItem(const char * sTableName);
    void exportLockKey(const char * sTableName);
    void exportMiddleInfo(const char * sTableName);
    void exportMiddleData(const char * sTableName);
    void exportAddress(const char * sTableName);

    void exportAll();


    bool check();

};


#endif //#ifndef _LOCAL_CACHE_CTL_H_
