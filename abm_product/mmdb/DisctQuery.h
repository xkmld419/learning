#ifndef DISCT_QUERY_H
#define DISCT_QUERY_H

/*
* Copyright (c) 2007,
* All rights reserved.
*
* 文件名称： DisctQuery.h
* 文件标识：
* 摘    要： 查询优惠轨迹接口，查询服务的代码里面调用
*
* 当前版本：1.0
* 作    者：张阿品
* 完成日期：2009年 月 日
*
*/


#include <vector>
#include "OfferAccuMgr.h"
#include "ParamDefineMgr.h"
#include "ProdOfferAggr.h"
#include "UserInfoReader.h"
#include "HashList.h"
#include "FreeAccuQuery.h"

using namespace std;


struct ItemNameData
{
    char m_sName[128];
    int  m_iKeyID;
};


class DisctQueryResult {
public:
    char m_sOfferName[128];     //商品名称
    char m_sElement[128];       //成员名称
    char m_sAccNBR[128];      //号码
    char m_sAcctGroupName[128];  ///账目组名称
    char m_sAcctItemName[128];  ///账目名称
    char m_sQueryName[128];        //查询名称
    int  m_iQueryID;              //主健
    int  m_iQueryFlag;            //查询渠道标志
    int  m_iOfferID;              //商品ID
    int  m_iItemGroupID;         ///账目组
    int  m_iAcctItemTypeID;      ///账目类型
    long m_lValue;               //钱
    long m_lItemDisctSeq;        //表的主健
};




class DisctQuery{
public:

    //sDate如果不传，　就取sysdate，返回vector里面的记录数,如果返回值小于0表示出错
    int query(vector<DisctQueryResult> &vResult,
            long lServID, char *sDate=0);

    //sDate如果不传，　就取sysdate，返回vector里面的记录数,如果返回值小于0表示出错
    int queryDB(vector<DisctQueryResult> &vResult,
            long lServID, char *sDate=0);
public:
    DisctQuery();
    ~DisctQuery();


private:
    void load();
    void loadItemName(const char * sTableName, const char * sLoadSql,  
            ItemNameData **pList, HashList<ItemNameData *> **pIndex);
    void transRes( DisctQueryResult *pResDisct, ProdOfferTotalData * pData );
    
private:

    int m_iBillingCycleID;
    long m_lServID;
    char m_sDate[16];

    OfferInsQO   *m_poOfferInstBuf;
    BillingCycleMgr * m_poBCM;
    ProdOfferDisctAggr * m_poProdOfferDisctAggr;

    HashList<ItemNameData *> *m_poOfferNameIndex;
    HashList<ItemNameData *> *m_poAcctItemNameIndex;
    HashList<ItemNameData *> *m_poAcctGroupNameIndex;

    ItemNameData * m_poOfferNameList;
    ItemNameData * m_poAcctItemNameList;
    ItemNameData * m_poAcctGroupNameList;

    vector<TableData> vet85;
    vector<TableData> vet87;
    
};

/*
class ProdOfferTotalData
{
    public:
     long   m_lItemDisctSeq;
     long	m_lProdOfferID; -------注意，内存里面放的是offer_inst_id，优惠轨迹查询这里放offerid
     int	m_iBillingCycleID;
     int    m_iProdOfferCombine;
     int    m_iPricingMeasure;
     int    m_iItemGroupID;
     int    m_iInType;              //输入类型：15：成员实例，2：用户，12：商品，3：客户，16：商品下的成员
     long   m_lInValue;
     long   m_lServID;
     int	m_iAcctItemTypeID;		    //账目类型
     int    m_iValue;
     bool   m_bInOutType;               //输入输出类型（true:输入，false：输出）
     bool	m_bNewRec;				    //是否为新增记录(false:新增记录,true:在老记录上累积)
}
*/

#endif

