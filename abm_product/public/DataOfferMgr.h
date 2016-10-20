/*VER: 2*/ 
#ifndef DATA_OFFER_MGR_H
#define DATA_OFFER_MGR_H

#include <map>
using  namespace std;

class DataProductOffer
{
	public:
	 	DataProductOffer():m_iControlID(0),m_iOfferID(0),m_iOfferDetailID(0),
	 					   m_iRefID(0),m_lThresholdValue(0),m_iStopType(0),
	 					   m_iOffsetCycleEnd(0),m_iRemindOfferDetailID(0),m_iOrgID(0)
	 	{
	 		memset(m_sRemark, 0, sizeof(m_sRemark));
	 		memset(m_sThresholdUnitName, 0, sizeof(m_sThresholdUnitName));
	 		memset(m_sJudgeInstStartDate, 0, sizeof(m_sJudgeInstStartDate));
	 	}
	 	
	 	~DataProductOffer()
	 	{
	 	}
	 	
	 	//序列号（PK）
    	int m_iControlID;	
   		//商品编码
		int m_iOfferID;
		//停机对象ID;
		int m_iOfferDetailID;	
		//判断对象，对应到累积量
		int m_iRefID;	
		//停机门槛值
		long m_lThresholdValue;
		//停机工单类型（直接写入工单）
		int m_iStopType;
		//账期末偏移时间（分钟），处理时间离账期末偏移时间量
		int m_iOffsetCycleEnd;
		//短信提醒对象ID（后续进行考虑）
		int m_iRemindOfferDetailID;
		//短信提醒工单类型
		int m_iRemindType;
		//注释
		char m_sRemark[101];
		//机构标识（下发用）
		int m_iOrgID;	
		//单位名称 add 2009.11.20
		char m_sThresholdUnitName[21];
		//此时间点后才生效的套餐用户参与流量信控
		char m_sJudgeInstStartDate[15];
};

typedef map<int, DataProductOffer *> DATA_OFFER_DETAIL_MAP;
typedef map<int, DATA_OFFER_DETAIL_MAP *> DATA_OFFER_MAP;

class OfferInsQO;

class DataProductOfferMgr
{
public:
    DataProductOfferMgr(); 
    ~DataProductOfferMgr();
    	
    void load();
    void unload();
    
public:
	//默认必须判断detail_id
	DataProductOffer * getInfoByOfferInst(OfferInsQO *opOfferInst);

    DataProductOffer * getInfoByOfferID(int iOfferID);
    DataProductOffer * getInfoByOfferID(int iOfferID, char *sInstEffDate);
    
    DataProductOffer * getInfoByOfferDetailID(int iOfferID, int iOfferDetailID);
    DataProductOffer * getInfoByOfferDetailID(int iOfferID, int iOfferDetailID, char *sInstEffDate);
    
    bool isDataOfferInst(int iOfferID);
    bool isDataOfferInst(int iOfferID, char *sInstEffDate);
    
    bool isDataOfferInstDetail(int iOfferID, int iOfferDetailID);
    bool isDataOfferInstDetail(int iOfferID, int iOfferDetailID, char *sInstEffDate);

private:
    DATA_OFFER_MAP m_poOfferInfoMap;
    DATA_OFFER_MAP::iterator m_pOfferIter;

    DATA_OFFER_DETAIL_MAP::iterator m_pOfferDetailIter;    	
    
    static bool m_bUploaded;
    
    bool m_bJudgeInstEffDate;
    
    //bool m_bJudgeInstDetail;默认必须判断detail_id
};

#endif
