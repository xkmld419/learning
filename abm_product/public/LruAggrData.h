#ifndef _HSS_LRU_AGGR_DATA_H_
#define _HSS_LRU_AGGR_DATA_H_

struct LruAggrData
{
   long lServID ;
   long lAcctID ;
   long lCustID ;
   long lProductID ;
   long lOfferID ;
   char sArea[4];
   char sExpDate[14];
   char sEffDate[14];

};

#endif
