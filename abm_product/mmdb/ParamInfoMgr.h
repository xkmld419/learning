#ifndef PARAMINFOMGR_H_HEADER_INCLUDED
#define PARAMINFOMGR_H_HEADER_INCLUDED

#include "ParamInfo.h"
#define MAX_EULECHECK_KEY_LEN	56
class ParamInfoMgr : public ShmParamInfo
{
public:
    ParamInfoMgr();
	ParamInfoMgr(int iNo);
    ~ParamInfoMgr();
	int getConnnum();
	bool Needload(char const *_sCode);
	bool Needload(char const *_sSegment, char const *_sCode);
                void getState();
                void LogSHMMEMInfo(char* pMemCfgName, char* pMemCfgMode,long lSHMID, int nUsed, int nTotal);
template <class dataT >
void printRecordInfo( dataT * pClass, char const * sClass,char const * sType);  
    bool load();
    void empty();
    void unload();

    void loadLogicStatement();
    void loadAssignStatement();
    void loadFormula();
    
    void loadAttrTrans();
	void loadMServAcct();
	bool getBillingCycleOffsetTime(int iOffset,char * strDate,TOCIQuery &qry);
    void loadTrunk();
    void loadAreaCode();
    void loadHLR();
	void loadVisitAreaCode();
	void loadEdgeRoamEx();
	unsigned int loadPNSegBilling(char *sWhere,int &iSegBillingCnt);
	void loadPNSegBillingEx();//
	void loadTrunkTreeNode();
	int getSwitchIDTrunkInRec(char *sWhere,SwitchIDTrunkInRec *&pSwitchIDTrunkIn);
	SwitchIDTrunkInRec *pSwitchRec;
	SwitchIDTrunkInRec *pSwitchTrunkRec;
	//
	void loadNotPNSegBilling();
	
	void loadServiceInfo2();
    void loadCarrier();
    void loadService2();

//    void loadSwitchInfo();
    void loadTrunkBilling();
	void loadSpecTrunkProduct();
	void loadSwitchItemType();
	void loadLocalAreaCode();
    void loadTimesType();
	//
	void loadFilterRuleResult();
	void RuleCheckCalledInsert(char * sOrgCalledNbr,unsigned int iFilterRuleResult,char * sOrgCallingNbr);
	void RuleCheckCallingInsert(char * sOrgCallingNbr, char * sOrgCalledNbr,unsigned int iFilterRuleResult,char *sSwitchAreaCode);
	void RuleCheckAreaCodeInsert(char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr,unsigned int iFilterRuleResult,int iGatherPot);
	void RuleCheckGatherPotInsert(int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr,unsigned int iFilterRuleResult,char *sTrunkOut);
	void RuleCheckTrunkOutInsert(char * sTrunkOut, int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, unsigned int iFilterRuleResult,char * sTrunkIn);
	void RuleCheckTrunkInInsert(char * sTrunkIn, char * sTrunkOut, int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr,unsigned int iFilterRuleResult,int iSwitchID);
	void RuleCheckSwitchIDInsert(int iSwitchID, char * sTrunkIn, char * sTrunkOut,  int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, unsigned int iFilterRuleResult,int iLongType);
	void RuleCheckLongTypeInsert(int iLongType, int iSwitchID, char * sTrunkIn, char * sTrunkOut,  int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, unsigned int iFilterRuleResult);

    void loadCityInfo();

    void loadOrg();
    void loadOrgEx();
    void loadOrgLongType();
    void loadHeadEx();
    void loadLocalHead();

    void loadEventType();
    void loadEventTypeGroup();

    void loadOffer();
    void loadOfferSpecAttr();
    void loadOfferNumAttr();
    void loadOfferFree();

    void loadNbrMap();

    void loadAcctItem();

    void loadLongEventTypeRule();
    void loadLongEventTypeRuleEx();

    void loadPricingPlanType();

    void loadStateConvert();
    void loadValueMap();
    void loadZoneItem();
    void loadSplitAcctItemGroup();
    void loadSplitGroupEvent();

    void loadDistanceType();

    void loadEdgeRoam();
    
        void loadSection();
        void loadTariff();
        void loadDiscount();
        void loadPercent();
        void loadLifeCycle();
        void loadPricingMeasure();
        void loadCombine();
        void transDisctApplyer(DisctApplyer *pDisctApplyer, char *sApplyMethod);
        void transApplyObject(unsigned int iApplyTmp, char *sOwnerID, int iObjectID, int iReferenceID);
        void setMeasureOffset(unsigned int iMeasure,int iMethod);
        void makeTree( PricingSection *pSection, PricingCombine *pCombine );
        void makeFilter(PricingCombine *pCombine);
        void insertFilter3(unsigned int m_iMeasureTmp, int iExt, int iMember, int iGroup);
        void preCompile(PricingCombine *pCombine);
        int setAcctBreakPoint (unsigned int iMeasure, LogicStatement *statement, PricingSection *pSection,vector<ConditionPoint> & vPoint, int &iOp);
        int setBreakPoint(unsigned int iMeasure,LogicStatement *statement, PricingSection *pSection, vector<ConditionPoint> & vPoint, int &iOp);
        
        void loadUsageType();
        bool UsageTypeNeedCal(int nUsageTypeID);
        bool queryBySplitID (int iSplitID,SplitCfg * pCfg);
        
        void exChangeValue(unsigned int m_iFirst, unsigned int m_iSecond);
        void preCompileB(PricingCombine *pCombine);
        void getCombineType(PricingCombine *pCombine);
        void insertPoint(vector<ConditionPoint> &vDes,vector<ConditionPoint> &vSrc);
        void analysePoint (vector< vector<ConditionPoint> > &vPoint,vector<int> &vOp, 
                        PricingSection *pSectionTemp, unsigned int *pNum, PricingSection **ppElse, int iType, unsigned int iSection);
        
        void loadTimeCond();
        void loadEventTypeCond();
        void loadCycleType();
        bool checkCycleType(const CycleType * pCycle);
        void loadAccuType();
        void loadOfferAccuRelation();
        void loadOfferAccuExist();
        void loadSwitchInfo();
#ifdef EVENT_AGGR_RECYCLE_USE           
        void loadSpecialCycleType();
#endif
        void loadVirtualCombine();
#ifdef GROUP_CONTROL
    void loadCGCombine();
#endif
    void loadCombineRelation();
#ifdef WHOLE_DISCOUNT
    void loadWholeDiscount();
#endif  
        //void newDirectory(Directory *pDirectory, char *srcPath);
        void loadSpecialNbrStrategy();
        void loadHost();
        void loadGatherTask();
        void loadSplit();       
        void loadOfferCombine();
        void loadProductCombine();      
        void loadProductAcctItem();  
        void addProductAcctItem(int iProductID, int iAcctItemTypeID);
        void loadSpecialTariff();
        void loadSpInfo();
        void loadGatherTaskListInfo();
        void loadSegment();
        void loadLongTypeRule();
        void TestSection2(unsigned int iSectionOffset);
        void TestCombine2(int iCombineID);
        bool getBelongGroupB(int acctitem, int bacctitemgroup);
        void loadCheckRule();
        void loadCheckSubRule();
        void loadArgument();
        void loadCheckRelation();
        void loadTransFile();
        void loadParseRule();
        void loadInstTableList();
        void loadRentCfg();
        void loadNetworkPriority();
        void loadTrunkToBeNBR();
        void loadCheckDup();
        void loadProductorIDCfg();
        void loadLocalEventTypeRule();
    //2010.11.17
    void loadFilePoolInfo();
    void loadFilePoolCondition();
    void loadFileTypeConfig();
    void loadTLVEncodeStruct();
    //void loadTLVDecodeStruct();
    void loadTLVParseStruct();
    void loadFileParseRule();
    void loadSwitchCode();
    void loadBillingNodeInfo();
    //2010.12.20
    void loadCarrierInfo( );
    void loadTariffInfo( );
    void loadSectorInfo( );
    void loadCountryInfo( );
    void loadSpecialGroupMember();
    void loadSponsorInfo();


        bool IsCrossed (const int iDupType);
        int CheckFilePath (char * filepath); 

public:
		long m_iTotalMem;
		unsigned int m_iCurMem;
		bool initMem;
		bool checkMem(unsigned int iDataMem);
		bool InitMem();
        //std
        /*bool createStdData();
        unsigned int  getStdDataOffset(StdChar8DataDesc &mDescUse);
        void updateStdDesc(StdChar8DataDesc mStdDesc);//更新共享信息
        void emptyStdData(SHMData<StdChar8DataDesc> *pStdDataDesc,bool reset=false);*/
        //prep
        void loadTemplateMapSwitch();
        void loadObjectDefine();
        void loadEventParserGroup();
        void loadField();
        void loadSwitchTypeInfo();
        //void loadSepInfo();//在loadEventHeadAndColunmCfg以及loadObjectDefine已经加载
        void loadEventAttr();
        //void loadFileInfo();
        void loadEventHeadAndColunmCfg();
        void loadSwitchIDMapSwitchInfo();
        void loadFieldCheckMethod();
        void loadFieldValueMap();
        void loadFieldMerge();
        
        void loadTap3FileInfo();
        void loadSepFile();
        //合并功能
        void loadMergeRule();
        //上传下发
        void loadTransParam();
public:
        //int getTotalNumByType(long lStructType);        
        //int getUsedNumByType(long lStructType);
        //StdChar8DataDesc *findByType(long lStructType);
public:
    float m_fRatio;
	int m_iNo;
		bool updateSHMInfo(long lIPCKey,char *sMEMCfgTable,unsigned int iUsed,unsigned int iTotal,char *sDate);
		
                /*int m_iMAXConditionPointNum;
                int m_iMAXSplitCfgNum;
                int m_iMAXFilterConditionNum;
                int m_iMAXPricingCombineNum;    
                int m_iMAXPricingMeasureNum;
                int m_iMAXLifeCycleNum;
                int m_iMAXPercentNum;
                int m_iMAXUsageTypeNum;
                int m_iMAXDisctApplyObjectNum;
                int m_iMAXDisctApplyerNum;
                int m_iMAXDiscountNum;
                int m_iMAXTariffNum;
                int m_iMAXPricingSectionNum;
                int m_iMAXWholeDiscountNum;
                int m_iMAXSpecNbrStrategyNum;
                int m_iMAXCombineRelationNum;
                int m_iMAXTimeCondNum;
                int m_iMAXEventTypeCondNum;
                int m_iMAXCycleTypeNum;
                int m_iMAXAccuTypeNum;
                int m_iMAXOfferAccuRelationNum;
                int m_iMAXCycleTypeIDNum;
                int m_iWholeDiscountNum;         
        int m_iMAXAssignNum;*/
                
                #ifdef EVENT_AGGR_RECYCLE_USE
                        int m_iArrayLen;
                        int     m_iDays;
                        //static int * m_poCycleTypeID;
                #endif
                int m_iAccNbrLen;               
                int m_iSpecNbrStratLen;         
                int m_iCurEventType;
};


#endif  //PARAMINFOMGR_H_HEADER_INCLUDED

