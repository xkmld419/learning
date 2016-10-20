/*VER: 5*/ 
#ifndef ACCT_ITEM_H_HEADER_INCLUDED
#define ACCT_ITEM_H_HEADER_INCLUDED

#include "SHMData.h"

#ifdef MEM_REUSE_MODE
#include "SHMData_B.h"
#include "SHMData_A.h"
#include "SHMIntHashIndex_KR.h"
#else
#include "SHMIntHashIndex.h"

#endif

//add by zhaoziwei
#include <string>
#include "InstTableListMgrKerEx.h"
//end
#include "IpcKey.h"
#include "ParamDefineMgr.h"
#include "CSemaphore.h"

#include <vector>

using namespace std;

class SeqMgrExt;

int getFlowID();

#define ACCT_ITEM_DATA        (IpcKeyMgr::getIpcKeyEx(getFlowID (), "SHM_AcctItemData"))
#define ACCT_ITEM_SERV_INDEX  (IpcKeyMgr::getIpcKeyEx(getFlowID (), "SHM_AcctItemIndex_S"))
#define ACCT_ITEM_DATA_LOCK     (IpcKeyMgr::getIpcKeyEx(getFlowID (),  "SEM_AcctItemData"))
#define ACCT_ITEM_INDEX_LOCK     (IpcKeyMgr::getIpcKeyEx(getFlowID (),  "SEM_AcctItemIndex"))
#define ACCT_ITEM_COUNT \
                    (ParamDefineMgr::getLongParam(getFlowID (), "MEMORY_DB", "AcctItemCount"))
#define ACCT_ITEM_INDEX_COUNT \
            (ParamDefineMgr::getLongParam(getFlowID (), "MEMORY_DB", "AcctItemIndexCount"))

struct AcctItemData {
    long m_lAcctItemID;
    long m_lServID;
    long m_lAcctID;
    long m_lOfferInstID;
    int  m_iBillingCycleID;
    long  m_lValue;
    int  m_iPayMeasure;
    int  m_iPayItemTypeID;
    char m_sStateDate[16];

    int  m_iItemSourceID;   //1: 详单定价; 2: 预留; 3: 账单定价; 4: 预留
	int  m_iItemClassID;	//0: default; 1: 封锁帐单
    #ifdef ORG_BILLINGCYCLE
    int m_iOrgBillingCycleID;
    #endif
    
    unsigned int m_iServNext;
};

struct AcctItemAddAggrData;

class AcctItemBase {
public:
    AcctItemBase();

#ifdef MEM_REUSE_MODE
    void revoke(const int iBillingCycleID, bool bLocked);
#endif

protected:
    void bindData();

    static bool m_bAttached;
    static AcctItemData *m_poAcctItem;

#ifdef MEM_REUSE_MODE
    static SHMData_B<AcctItemData> *m_poAcctItemData;
    static SHMIntHashIndex_KR *m_poServIndex;
#else
    static SHMData<AcctItemData> *m_poAcctItemData;
    static SHMIntHashIndex *m_poServIndex;
#endif

    void freeAll();
};

class Ivpn;
class AcctItemAccuMgr : public AcctItemBase
{
	friend class AcctItemCtl;
	friend class Ivpn;	//Ivpn友元类 add by zhanyk
	friend class IvpnInfoMgr ;	
public:
    AcctItemAccuMgr();  


    long update(AcctItemAddAggrData *pData, bool &bNew, bool isModifyAcct=false);
	bool update(AcctItemData *pData,bool &bNew,bool bReplace = false);
    long  get(long lServID,int iBillingCycleID, int iAcctItemTypeID, int iOrgBillingCycleID=0);
    int  getDisct(long lServID, int iBillingCycleID, 
                  vector<AcctItemData *> & v, int iOrgBillingCycleID=0);
    long  getAcctItemDataPartyRoleID(long lServID, int iBillingCycleID, int iPartyRoleID,
                                    int iOrgBillingCycleID=0);
    int  getAll(long lServID, int iBillingCycleID, 
                vector<AcctItemData *> & v, int iOrgBillingCycleID=0);

    int  getHis(long lServID,vector<AcctItemData *> & v, int iOrgBillingCycleID=0);
    bool checkExist(long lServID, int iBillingCycleID, 
                    int iItemSourceID, int iOrgBillingCycleID = 0 );
    bool lockAllLock(){
        
        #ifdef USE_SERV_LOCK
            for (i=0; i<8; i++) {
                if (m_poServLock[i]){
                    m_poServLock[i]->P();
                }
            }
        #endif
        m_poDataLock->P ();
        m_poIndexLock->P ();
        return true;
    };

    bool freeAllLock(){
        #ifdef USE_SERV_LOCK
            for (i=0; i<8; i++) {
                if (m_poServLock[i]){
                    m_poServLock[i]->V();
                }
            }
        #endif
        m_poDataLock->V ();
        m_poIndexLock->V ();
        return true;
    };

private:
    SeqMgrExt * m_poSeqExt;
    static CSemaphore * m_poDataLock;
    static CSemaphore * m_poIndexLock;

#ifdef USE_SERV_LOCK
    static CSemaphore * m_poServLock[8];
#endif

};

//add by zhaoziwei
class CAcctData
{
public:

	vector<AcctItemData*> m_listAcctItemData;
	
	void AddData(AcctItemData* pData)
	{
		m_listAcctItemData.push_back(pData);
	};
	
	int getCount()
	{
		return m_listAcctItemData.size();
	};
};
//end

class AcctItemCtl : public AcctItemBase {
public:
    AcctItemCtl();
    void create();
    bool exist();
    void remove();
	bool GetCurProcNum();
#ifdef MEM_REUSE_MODE
    void showDetail(bool bClearRF);
#endif

public:
    void load(int iFlowID, int iOrgID, int iBillingCycleID);
    int  load(char const *sTable);
    int  out(char const *sTable);
	int  outForIvpn(char const *sTable, int iSourceID1, int iSourceID2 = 0, int iSourceID3 = 0,int iSourceID4 = 0,int iSourceID5 = 0);	//add by zhangyk
	 	bool getConnProcNum();
    int  outToTable(char const *sTable, unsigned int count, AcctItemData *pStart,unsigned int AllCount);
    int  outTableForIvpn(char const *sTable, int iSourceID1,
              unsigned int count, AcctItemData *p , int iSourceID2 = 0, int iSourceID3 = 0,int iSourceID4 = 0,int iSourceID5 = 0);
//add by zhaoziwei
bool dealAcctData();
int  outToTableEx(char const *sTable,AcctItemData *pAcctData,int count);
int  out_new(char const *sTable);
CAcctData* GetAcctData(int nOrgID,int iBillingCycleID);


    void showAcctItem(AcctItemData &oData);
map<string,CAcctData*> m_mapAcctData;
InstTableListMgrKerEx* m_pMgrInstTable;
unsigned int m_iAllCount;
//end
	long  getAllfee(int iBillingCycleID); //add by zhangyz for information	
	long  getAllfee(int iBillingCycleID,char *sDate); //add by zhangyz for information		

};

#endif
