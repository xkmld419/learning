/*VER: 13*/ 
#ifndef FREE_ACCU_QUERY_H
#define FREE_ACCU_QUERY_H

#include <vector>
#include "OfferAccuMgr.h"
#include "ParamDefineMgr.h"
#include "ProdOfferAggr.h"
#include "LifeCycleMgr.h"
#include "UserInfoReader.h"
#include "HashList.h"
#include "ID.h"
#include "Value.h"
#include <math.h>
#include "AttrValueCalc.h"
#include "BillConditionMgr.h"
#include "PricingInfoMgr.h"
#include "AcctItemMgr.h"

using namespace std;

const int OPER_FLAG_CG = 1;
const int OPER_FLAG_PARSER = 2;
const int OPER_FLAG_SECTIONPARSER = 3;
const int OPER_FLAG_OFFERNOPERCENT = 4;
const int OPER_FLAG_SECTIONPARSER_IN = 5;
const int JUST_FLAG_ESTU=2;

struct ItemName
{
	char m_sName[128];
	int  m_iCalRuleId;
	//扩展累积量类型:如 话费，短信，宽带，WAP上网，等等；
	int m_RemindTypeID;
};

struct TableData
{
	int		m_iBillingCycleID;
	int		m_iOrgID;
	char	m_sTableName[51];
};

struct ConditionVal 
{
	int m_iOfferID;
	int m_iCombineID;
	int m_iObjectID;
	ID * m_poParamID1;
	ID * m_poDesParamID;
	LifeCycle *m_poLifeCycle;
	int m_iMeasureSeq;	
    ConditionVal * m_poNext;
};

struct ConditionValTemp
{
	int  m_iOfferID;
	int  m_iCombineID;
	int  m_iObjectID;
	char m_sParamID1[32];
	char m_sDesParamID[32];
	int  m_iLifeCycleID;
	int  m_iMeasureSeq;	
};

struct ConditionResult
{
	int  m_iCombineID;
	long m_lValue ;
	int m_iMeasureSeq;
};
//sunjy 
struct FreeOfferAttrEx
{
	int  m_iQueryID;
	int  m_iOfferID;
	int  m_iAttrID;
	char m_sAttrValue[32];
	int  m_iValue;
	int  m_iType;
	FreeOfferAttrEx *m_poNext;
};

class FreeAccuQueryResult {
public:
    char m_sOfferName[128];     //商品名称
    char m_sExtendItem[200];    //可消费项目 wuan 2010-4-29
    char m_sAccuName[128];      //累积量名称
    char m_sStartTime[16];      //开始时间
    char m_sEndTime[16];        //结束时间
    char m_sUnitName[32];       // 单位
    char m_sTotal[16];          //总量
    char m_sAlready[16];        //已赠送
    char m_sLeft[16];           //剩余
    char m_sRemark[128];        //备注 被扩展用于存放查询名称
	int	  m_iQueryID;			  //主健
	int	  m_iQueryFlag;			  //查询渠道标志
	int	  m_iOfferID;			  //商品ID
	int	  m_iCalRuleID;			  //b_accu_cal_rule表的主健
	int  m_iAccuType;           //累积量的类型:0 -- 费用 1 -- 使用量
	int	 m_iRemindType;         //扩展累积量类型:如 话费，短信，宽带，WAP上网，等等；
    int m_iAccuTypeID;          //累积类型标示
	char m_sSplitby[32];		//拆分依据
};

class FreeAccuCfg {
    public:
		int m_iQueryID;
        //int m_iAccuTypeID; 
		ID * m_poObject;
        //ID * m_poFreeValue;
        int m_iUnitNume;
        int m_iUnitDeno;
        char m_sUnitName[32];
		int m_iQueryFlag;
		int m_iAdjustVal;
		int m_iAdjustFlag;
		char m_sQueryDesc[128];
		int m_iOperFlag;
		int m_iOfferID;
		char m_sExtendItem[200];    //可消费项目wuan 2010-4-29
    	int m_iMergeFlag;           //累积量查询结果合并标志：wuan 2010-05-17
        FreeAccuCfg * m_poNext;
    };

class FreeAccuQuery {
public:

    //sDate如果不传，　就取sysdate，返回vector里面的记录数
    int query(vector<FreeAccuQueryResult> &vResult,
            long lServID, char *sDate=0);

    //sDate如果不传，　就取sysdate，返回vector里面的记录数
    int queryDB(vector<FreeAccuQueryResult> &vResult,
            long lServID, char *sDate=0);

    int remindQueryAccu(vector<FreeAccuQueryResult> &vResult, 
		        EventSection * poEvent, OfferInsQO *pQO);
  
	static FreeAccuQuery * m_pQuery;
public:
    FreeAccuQuery();
	~FreeAccuQuery();

private:
    int queryOfferInst(vector<FreeAccuQueryResult> &vResult,
            long lServID,OfferInsQO *pQO);
    
	int queryOfferInstDB(vector<FreeAccuQueryResult> &vResult,
            long lServID,OfferInsQO *pQO);

	//int getConditionVal(ID * pParam1ID,ConditionResult * pOutConditionResult);
	int getConditionVal(FreeAccuCfg * pAccuCfg,ConditionResult * pOutConditionResult);
	int operProcess(vector<FreeAccuQueryResult> &vResult,long lServID,
		OfferInsQO *pQO,FreeAccuCfg * pAccuCfg);
	int getAggrByServ(long lServID,long lOfferInstID,char * sEffDate, char * sExpDate,long &lVal);
	void addResult(vector<FreeAccuQueryResult> &vResult,char * sOfferName,char * sAccuName,
		char * sEffDate,char * sExpDate,long lMaxVal,long lUsedVal,
		FreeAccuCfg * pAccuCfg,int iCalRuleID = 0,int iRemindTypeID = 0,char * sSpliby = 0);
	bool getCurDays(int &iUseDays,int &iToalDays);
	void parserObject(FreeAccuCfg * pAccuCfg);
    void loadCfg();
	void loadInit();
	void unload();

private:
    static OfferAccuMgr *m_poAccuMgr;
    static OfferInsQO   *m_poOfferInstBuf;
	static ConditionResult   * m_poResultBuf;
	static BillingCycleMgr * m_poBCM;
	static ProdOfferDisctAggr * m_poProdOfferDisctAggr;
	static ConditionVal  * m_pConditionValList;
	static HashList<ConditionVal *> *m_pConditionVal;

private:
	float MyRound(float fVal)
	{
		return (float)((long)(fVal*1000+5)/10)/100;
	}

	static FreeAccuCfg *m_poAccuCfgList ;
	static ItemName * m_poOfferNameList;
	static ItemName * m_poAccuNameList;
    static HashList<FreeAccuCfg *> *m_poAccuIndex;
    static HashList<FreeAccuCfg *> *m_poItemIndex;
	static HashList<ItemName *> *m_poOfferNameIndex;
	static HashList<ItemName *> *m_poAccuNameIndex;   
	static HashList<int> * m_poOfferID;
	static HashList<int> * m_poTableName;//存放已经处理过的表名
	static AccuTypeCfg * m_pCfg;
	
	static int	m_iFirstMonth;
	static int m_iMemFlag;
	
	//sunjy
	static FreeOfferAttrEx *m_poAttrExlist;
	static HashList<FreeOfferAttrEx *> *m_poAttrExIndex;

	static BillConditionMgr    *m_poConditionMgr;
	static PricingInfoMgr      *m_poPricingInfoMgr;
	static HashList<int>		*m_poOfferSectionParser;
	static EventSection			*pEvent;

	static int	m_iDisplayRealCharge;//显示费用累积量已使用值为实际值的开关，1 显示
	static int	m_iDisplayRealAggr;  //显示时长类累积量已使用值为实际值的开关，1 显示
	static int m_iCount;	//累积量查询规则数量
	static int m_iCountVal; //数据库版规则定价规则解析数量
	static bool m_bUploaded;

	int m_iBillingCycleID;
	char m_sDate[15];
	int m_iComNum;
	int m_ioCombineID[MAX_PRICING_COMBINE_NUMBER];
};

#endif

