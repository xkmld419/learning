/*VER: 4*/ 
// Copyright (c) 2007,联创科技股份有限公司电信事业部

#ifndef PROD_OFFER_AGGR_H
#define PROD_OFFER_AGGR_H

#include <sys/sem.h>
#include <vector>
#include "SHMData.h"
#include "SHMStringTreeIndex.h"
#include "SHMIntHashIndex.h"
#include "SeqMgrExt.h"
#include "CSemaphore.h"
#ifdef MEM_REUSE_MODE
#include "SHMData_B.h"
#include "SHMIntHashIndex_KR.h"
#endif
#ifdef HAVE_ABM
#include "abmcmdaccu.h"
#include "LocalCacheInfo.h"
#endif

using namespace std;

struct Value;

class ProdOfferDetailData                 //商品实例总量优惠输入输出分摊缓冲区结构
{				
public:
	long    m_lServID;                  //用户标识
	int	    m_iAcctItemTypeID;		    //账目类型
//	bool    m_bInOutType;               //输入输出类型（true::输入，false：输出）
	long    m_lInCharge;					//事件累积值
	long    m_lOutCharge;
	long    m_lOffset;
	bool	m_bNewRec;				    //是否为新增记录(true:新增记录,false:在老记录上累积)
	long    m_lOffsetCur;        //新增当前aggrdetail的offset，=ocs的aggrdetail的主键	
  int   m_iBIllingCycleID  ;  //sunjy 	
	#ifdef ABM_FIELD_EXTEND
	long m_lItemDetailSeq;
	long m_lVersionID;//从1开始计数
	#endif
};

class ProdOfferDetailDataEx : public ProdOfferDetailData {
public:
    long    m_lOfferInstID;
};

struct ProdOfferDetailBase
{
	long	m_lOfferInstID;
	long	m_lItemDisctSeq;
	long	m_lServID;   
	int		m_iAcctItemTypeID;
	int		m_iBillingCycleID;
	long	m_lInCharge;
	long	m_lOutCharge;
};

class ProdOfferAggrData
{
public:
     long   m_lItemDisctSeq;
     long	m_lProdOfferID;
     int	m_iBillingCycleID;
     int    m_iProdOfferCombine;
     int    m_iPricingMeasure;
     int    m_iItemGroupID;
     int    m_iInType;              //输入类型：15：成员实例，2：用户，12：商品，3：客户，16：商品下的成员
     long   m_lInValue;
     long   m_lValue;
     long	m_lDisctValue;
     long   m_lAggrOfferSet;       
     long   m_lDetailOfferSet;
     #ifdef ORG_BILLINGCYCLE
       int m_iOrgBillingCycleID;
     #endif
	 #ifdef ABM_FIELD_EXTEND
	 long m_lVersionID;//从1开始计数
	 #endif
};

class ProdOfferTotalData
{
    public:
     long   m_lItemDisctSeq;
     long	m_lProdOfferID;
     int	m_iBillingCycleID;
     int    m_iProdOfferCombine;
     int    m_iPricingMeasure;
     int    m_iItemGroupID;
     int    m_iInType;              //输入类型：15：成员实例，2：用户，12：商品，3：客户，16：商品下的成员
     long   m_lInValue;
     long   m_lServID;
     int	m_iAcctItemTypeID;		    //账目类型
     long   m_lValue;
     bool   m_bInOutType;               //输入输出类型（true:输入，false：输出）
     bool	m_bNewRec;				    //是否为新增记录(false:新增记录,true:在老记录上累积)
     #ifdef ORG_BILLINGCYCLE
       int m_iOrgBillingCycleID;
     #endif
};


class ProdOfferAggrAttach
{
public:
    
	ProdOfferAggrAttach();
    void FreeProdOfferAggrShm();
	void ProdOfferAggrReset();
#ifdef MEM_REUSE_MODE
	void revoke(const int iBillingCycleID, bool bLockedAggr );

#endif
protected:
#ifdef MEM_REUSE_MODE
    static SHMData_B<ProdOfferDetailData> *m_poProdOfferDetailData;
    static SHMData_B<ProdOfferAggrData> *m_poProdOfferAggrData;
    static SHMIntHashIndex_KR * m_poProdOfferAggrIndex;

#else
    static SHMData<ProdOfferDetailData> *m_poProdOfferDetailData;
    static SHMData<ProdOfferAggrData> *m_poProdOfferAggrData;
    static SHMIntHashIndex * m_poProdOfferAggrIndex;

#endif
    
    static ProdOfferAggrData * m_poProdOfferAggr;
    static bool m_bAttached;
    static bool m_bDetailAttached;
    static bool m_bAttachIndex;
    
    //sunjy
    static SHMIntHashIndex * m_poProdOfferAggrSeqIndex;    
    static bool m_bAttachSeqIndex;    
};

class ProdOfferDisctAggr:public ProdOfferAggrAttach
{
public:
    ProdOfferDisctAggr();
	~ProdOfferDisctAggr();
	void getIVPNAggrValue(long lOfferInstID,int iBillingCycleID,long &inValue,long &outValue);

	void doProdOfferDisct(ProdOfferTotalData * poAcctItem,
                vector<ProdOfferAggrData > &v_ProdOfferAggr,
                vector<ProdOfferDetailDataEx > &v_ProdOfferDetailData);

	int getAllItemAggr(ProdOfferAggrData * pProdAggrData, Value *pResult);

	int getAllDisctAggr(ProdOfferAggrData * pProdAggrData, Value *pResult); //四川取优惠掉金额
    //2010-07-14 wuan
	int getAllDisctAggrB(ProdOfferAggrData * pProdAggrData, Value *pResult);
	int GetItemDetail(ProdOfferTotalData *pProdOfferData,Value *pResult);

	int UpProdOfferAggr(ProdOfferTotalData * pProdOfferData,
                vector<ProdOfferAggrData > &v_ProdOfferAggr,
                vector<ProdOfferDetailDataEx > &v_ProdOfferDetailData);
	int RefreshOfferAggr(ProdOfferTotalData * pProdOfferData, bool bCreate=false);
	
	int GetItemTotalHead(ProdOfferTotalData *pProdOfferData,
                ProdOfferAggrData **ppProdOfferAggrHead);

	int GetItemDetailHead(long lOffset,
                ProdOfferDetailData **ppProdOfferDetailHead);

    ProdOfferDetailData * convertDetail(long lOffset);
	int GetItemDetailNum(long lOffset);
    void getAll(long lServID,long lOfferInstID,int iBillingCycleID, vector<ProdOfferTotalData>& voPOTData, int iOrgBillingCycleID = 0);
    void getAllByInst(long lOfferInstID,int iBillingCycleID, vector<ProdOfferTotalData>& voPOTData, int iOrgBillingCycleID = 0);
	bool UpdateOfferAggrZero(ProdOfferAggrData * pProdAggrData);
	bool UpdateOfferAggr(ProdOfferAggrData * pProdAggrData ,bool &bNew,bool bReplace);
	bool UpdateOfferDetail(ProdOfferDetailBase * pOfferDetailData,bool &bNew,bool bReplace);
//add by zhaoziwei	
void getAllDisctAggrByInst(long lOfferInstID,int iBillingCycleID, vector<ProdOfferAggrData *>&voPOTData, int iOrgBillingCycleID = 0);
void getAllDisctByInst(long lItemDisctSeq,long lOfferInstID,int iBillingCycleID, vector<ProdOfferDetailData *>&voPOTData, int iOrgBillingCycleID = 0);
//end
    
    

    //修改内存的alue，该走接口方式
	void setAggrOutput(long lOffset,  ProdOfferAggrData *pAggr,  long iValue,
                      int iAddedValue);
	void setDetailOutput(long lOffset,  ProdOfferDetailData *pDetail,  long  iValue,  
                      int iAddedValue);	
	
	#ifdef HAVE_ABM
    long updateAuditOfferAggr(DisctAccuData *pDisct, bool isSycn = false);
    bool updateAuditOfferDetail(long lAggrOffset, 
                                AuditDisctAccuDetailData *pData);
    long updateSynOfferAggr(SynDisctAccuData *pAggr);
    bool updateSynOfferDetail(long lAggrOffset ,
                              SynDisctAccuDetailData *pDetail );

	void getAllAggrAndDetail(long lOfferInstID,int iBillingCycleID,vector<DisctAccu>& vOutData);
	unsigned int updateAggrByCache(DisctCacheData * pCacheData);
	void updateDetailByCache(DisctDetailCacheData * pCacheData,unsigned int iAggOffset);
	#endif

    bool lockAllLock(){
        m_poAggrLock->P ();
        m_poAggrDetailLock->P ();
        m_poAggrIndexLock->P ();        
        return true;
    };

    bool freeAllLock(){
        m_poAggrLock->V ();
        m_poAggrDetailLock->V ();
        m_poAggrIndexLock->V ();
        return true;
    };    
protected:

	//lock
    static CSemaphore * m_poAggrLock;
    static CSemaphore * m_poAggrDetailLock;
    static CSemaphore * m_poAggrIndexLock;
    //序列
    static SeqMgrExt * m_pItemDisctSeq;
};

class ProdOfferDisctMgr:public ProdOfferAggrAttach
{
public:
    ProdOfferDisctMgr();
    int LoadOfferAggr(char *sTableName,char *sDetailTableName);
	int  loadOfferAggr(const char* sAggrTable);
	void LoadOfferAggr(int iFlowID, int iOrgID, int iBillingCycleID);
	void CreateOfferAggr();
	void DelOfferAggr();
	void GetOfferAggr(char *sTableName,char *sDetailTableName);
	
	int GetOfferAggrRec(long lProdOfferID);
	int GetOfferAggrDetail(long lItemDisctSeq,long lProdOfferID);
	bool getConnProcNum();
	bool GetCurProcNum();
	//--xgs--
	void insertTableCommit(char *sTableName,char *sDetailTableName);
    //上下载优化
    int NewLoadOfferAggr(char *sTableName,char *sDetailTableName);
     void ExportAggr(char *sTableName);
    void ExportDetail(char *sDetailTableName);  
       void showAggrData(ProdOfferAggrData &oData);	
         void showDetailData(ProdOfferDetailData &oData);
	  	  int loadOfferDetail(const char* sDetailTable);
	
#ifdef MEM_REUSE_MODE
    void showDetail(bool bClearRF);
#endif
private:
   vector<ProdOfferAggrData> vProdOfferAggrData;
   vector<ProdOfferDetailData> vProdOfferDetailData;
};
    
#endif
