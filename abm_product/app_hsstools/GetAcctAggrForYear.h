#ifndef __GET_ACCT_AGGR_FOR_YEAR_H__
#define __GET_ACCT_AGGR_FOR_YEAR_H__
/*VER: 1.01*/ 
#include <iostream>
#include "Date.h"
#include "Log.h"
#include <vector>
#include "AcctItemAccu.h"
#include "UserInfoReader.h"
#include "BillingCycleMgr.h"
#include "MBC.h"


using namespace std;

struct OfferData
{
	int  m_iOfferId;
	char m_sMemberType[4];			//商品成员标识
	char EffDate[16];
	char ExpDate[16];
	int	 m_iBillMerger;				//是否归并
	bool bMerger;					//是否归并帐期
	bool bServ;
	bool bCust;
	bool bOfferIns;
	bool bAcct;
	bool bAll;
};

struct YearAcctItemData 
{
    long AcctItemID;
    long ServID;
    int	BillingCycleID;
    int	AcctItemTypeId; 
    long Charge;
    long AcctID;
    char Sate[4];
    char StateDate[16];
    int Free_Flag;
    int Sett_Flag;
    int ItemSourceID;
    long OfferInstID;
	int OfferId;
    int MeasureType;
    int ItemClassId;    
};


class GetAcctAggrYear : public AcctItemAccuMgr,public UserInfoBase
{  public:	
	GetAcctAggrYear();
	~GetAcctAggrYear();
  public:
	int m_iBillingCycleId;
	char m_sCycleBeginDate[18];
    char m_sCycleEndDate[18];
	int m_iOfferCount;					//配置表获取OFFERID个数
	int m_iCount;						//提取数据总数
	char m_sTableName[30];				//备份his表

	int GetInstInfoById();
	int GetInstIdFromDb(int OfferId);
	int GetServInfo(long OfferInstId,int Flag);
	int GetAllAcctAggr(long lServID,int iBillingCycleID,vector<AcctItemData *> &v);
	bool GetOfferId();
	bool CheckUserOper();
	bool CheckBillingData();
	bool DeleteAggrFromHis();				
	bool GetBillingCycleTime();
	bool CheckIfMergerBilling();
	bool MergerBillingDb(int OfferId,int iBillingCycleID);
	bool InsertDataDb();
	bool UserOperLogDb(int Flag);

  private:
  	vector<int> m_vOfferId;				//记录有效的offerid
  	vector<long> m_vOfferInstId;		//记录商品实例ID
	vector<AcctItemData *> m_vAcctItem;
	HashList<OfferData* >* m_pOfferRule;//为整合配置表中一个offerid多条数据的情况做记录
	HashList<long> * m_pServId;			//存储提取过的servid,以便后面过滤
};


#endif
