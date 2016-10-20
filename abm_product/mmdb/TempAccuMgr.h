/***************************************************************************
*	Copyright (c) 2010,亚信联创科技股份有限公司电信计费研发部
*	All rights reserved.
*	
*	文件名: TempAccuMgr.h
*	功能:	批价中间累积量缓存管理封装
*	作者：	wangs
*	日期：	2010-10-09
*	版本：	1.0
*	更改记录：
****************************************************************************/

#ifndef _TEMP_ACCU_MGR_H
#define _TEMP_ACCU_MGR_H

#include "LocalCacheInfo.h"
#include "ProdOfferAggr.h"
#include "OfferAccuMgr.h"
#include "AccuTypeCfg.h"
#include <vector>

using namespace std;

/*
//LocalCacheInfo.h中有定义
struct DisctAccu
{
	DisctCacheData  m_oDisct;
	vector<DisctDetailCacheData > m_voDetail;
}
*/

//AccuTypeCfg.h中有定义（事件累积量查询条件定义）
/*struct EventAccuQuery
{
	long m_lInstID;
	vector<int> m_viAccuTypeID;
};*/

const int DISCT_AGGR_NUM		= 128;

class OfferAccuMgr;

class TempAccuMgr
{
	friend class OcsEngine;
public:
	TempAccuMgr(OfferAccuMgr * pAccuMgr);
	
	//环境初始化
	void init();

	//添加事件累积量查询条件
	void addEventAccuCond(EventAccuQuery * pCond);

	//添加优惠累积量查询条件
	void addDisctAccuCond(long lInstID);

	//使用得到的商品实例ID及相关条件进行数据查询
	void queryData();

	//添加参考的事件累积量数据并返回值
	long addRefEventAccuData(long lOfferInstID,int iAccuTypeID,long lCycleInstID,
		long lServID,long lMemberID,char sSplitby[]);

	//查询参考的优惠累积量数据
	long getRefDisctData(ProdOfferAggrData * pData);

	//添加优惠累积量数据
	void addDisctData(ProdOfferTotalData *pData,int iOutNum = 0);

	//添加变化的事件累积量数据
	void addUpdateEventAccuData(vector<EventAddAggrData> &voData);

	//查询优惠累积量的实际容量
	int getDisctAggrNum();

	//查询是否有累积量数据
	bool isAggrEmpty();

public:
	//查询得到优惠累积量的头指针
	int getDisctAggrHead(ProdOfferTotalData *pData,DisctAccu **ppDisctAccu);

public:
	//事件累积量最终结果
	vector<AccuCacheData> m_voEventAccu;

	//优惠累积量最终结果
	vector<DisctAccu> m_voDisctAccu;

private:
	//事件累积量相关指针定义
	AccuTypeCfg * m_poAccuCfg ;
	OfferAccuMgr  * m_poAccuMgr;

private:
	//查询到的事件累积量集合
	vector<AccuData> m_voEventAccuBase;

	//事件累积量查询条件集合
	vector<EventAccuQuery> m_vEventAccuCond;

	//优惠累积量查询条件集合
	vector<long> m_vDisctAccuCond;

	//临时序列器
	long m_lSeq;

	//这几个不用每次清理
	long m_lCycleInstID;
	int	  m_iCycleTypeID;
	char m_sBeginTime[16];
	char m_sEndTime[16];
};

#endif //_TEMP_ACCU_MGR_H
