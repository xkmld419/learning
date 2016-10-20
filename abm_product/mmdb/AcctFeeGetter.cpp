/*VER: 4*/ 
#include "TransactionMgr.h"
#include "Environment.h"
#include "AcctFeeGetter.h"

//空的构造函数
AcctFeeGetter::AcctFeeGetter():UserInfoReader(USER_INFO_BASE)
{
}

#ifdef SHM_FILE_USERINFO

//根据帐户ID和查询帐期得到用户的费用
long AcctFeeGetter::getFee(long lAcctID, int iBillingCycleID)
{
	unsigned int i;
	long lRet (0);
	//查找帐户的开始索引
	if(!m_pSHMFileExternal->m_poServAcctIndex->get(lAcctID,&i))
	{
		return 0;
	}
	//循环帐户下的每一个用户
	for (; i; i=m_pSHMFileExternal->m_poServAcct[i].m_iAcctNextOffset)
	{
		unsigned int j;
		if(m_pSHMFileExternal->m_poServAcct[i].m_iState == 1)
		{
			//根据用户ID查找用户帐目的开始索引
			if(!AcctItemBase::m_poServIndex->get(m_pSHMFileExternal->m_poServAcct[i].m_lServID,&j))
			{
				continue;
			}
			//循环每一个用户帐目
			for (; j;j=m_poAcctItem[j].m_iServNext)
			{
				AcctItemData * p = m_poAcctItem +j;
				if(p->m_iBillingCycleID==iBillingCycleID &&
				   p->m_iPayMeasure==PAY_MEASURE_MONEY)
				   {
				   		lRet+=p->m_lValue;
				   }
			}
	  }
	}
	return lRet;
}

//根据帐户ID和查询帐期得到用户的费用
long AcctFeeGetter::getFee(long lAcctID, int iBillingCycleID,vector<long> &rvAcctItemAggrData)
{
	unsigned int i,k,j,h=0,len=0;
	long iRet (0);
	long lVal=0;
	//查找帐户的开始索引
	if(!m_pSHMFileExternal->m_poServAcctIndex->get(lAcctID,&i))
	{
		return 0;
	}
	k = i;
	//取得数组长度
	for (; i; i=m_pSHMFileExternal->m_poServAcct[i].m_iAcctNextOffset)
	{
		if(m_pSHMFileExternal->m_poServAcct[i].m_iState == 1)
		{
			if(!AcctItemBase::m_poServIndex->get(m_pSHMFileExternal->m_poServAcct[i].m_lServID,&j))
			{
				continue;
			}
	  }
		len++;
	}
	long * lServFee = new long[len*2];
	//取帐户下所有用户及索引
	len = 0;
	for (; k; k=m_pSHMFileExternal->m_poServAcct[k].m_iAcctNextOffset)
	{
		if(m_pSHMFileExternal->m_poServAcct[k].m_iState == 1)
		{
			if(!AcctItemBase::m_poServIndex->get(m_pSHMFileExternal->m_poServAcct[k].m_lServID,&j))
			{
				continue;
			}
			*(lServFee+(len*2)+0)=m_pSHMFileExternal->m_poServAcct[k].m_lServID;
			*(lServFee+(len*2)+1)=j;
			len++;
	  }
	}
	//对用户排序
	j = 1;
	for (i=0; i<len; i++)
	{
		for (; j<len; j++)
		{
			if(*(lServFee+(i*2)+0)>*(lServFee+(j*2)+0))
			{
				lVal = *(lServFee+(j*2)+0);
				h = *(lServFee+(j*2)+1);
				*(lServFee+(j*2)+0) = *(lServFee+(i*2)+0);
				*(lServFee+(j*2)+1) = *(lServFee+(i*2)+1);
				*(lServFee+(i*2)+0) = lVal;
				*(lServFee+(i*2)+1) = h;
			}
		}
		j=i+2;
	}
	for (i=0; i<len; i++)
	{
		j = *(lServFee+(i*2)+1);
		//循环每一个用户帐目
		for (; j; j=m_poAcctItem[j].m_iServNext)
		{
			AcctItemData * p = m_poAcctItem + j;
			if(p->m_iBillingCycleID==iBillingCycleID &&
			   p->m_iPayMeasure==PAY_MEASURE_MONEY)
			   {
			   		iRet+=p->m_lValue;
			   		//添加地址信息
			   		rvAcctItemAggrData.push_back((long)(p));
			   }
		}
	}
	
	/*
	//循环帐户下的每一个用户
	for (; i; i=m_poServAcct[i].m_iAcctNextOffset)
	{
		unsigned int j;
		//根据用户ID查找用户帐目的开始索引
		if(!AcctItemBase::m_poServIndex->get(m_poServAcct[i].m_lServID,&j))
		{
			continue;
		}
		//循环每一个用户帐目
		for (; j;j=m_poAcctItem[j].m_iServNext)
		{
			AcctItemData * p = m_poAcctItem +j;
			if(p->m_iBillingCycleID==iBillingCycleID &&
			   p->m_iPayMeasure==PAY_MEASURE_MONEY)
			   {
			   		iRet+=p->m_lValue;
			   		//添加地址信息
			   		rvAcctItemAggrData.push_back((long)(p));
			   }
		}
	}
	*/
	delete [] lServFee;
	return iRet;	
}

//根据用户ID和帐期得到用户的费用
long AcctFeeGetter::getServFee(long lServID, int iBillingCycleID,vector<long> &rvAcctItemAggrData)
{
	long iRet (0);
	unsigned int j;
	//根据用户ID查找用户帐目的开始索引
	if(!AcctItemBase::m_poServIndex->get(lServID,&j))
	{
		return iRet;
	}
	//循环每一个用户帐目
	for (; j;j=m_poAcctItem[j].m_iServNext)
	{
		AcctItemData * p = m_poAcctItem +j;
		if(p->m_iBillingCycleID==iBillingCycleID &&
		   p->m_iPayMeasure==PAY_MEASURE_MONEY)
		{
		   		iRet+=p->m_lValue;
		   		//添加地址信息
		   		rvAcctItemAggrData.push_back((long)(p));
		}
	}
	return iRet;
}

long AcctFeeGetter::getFee(long lAcctID, long lServID,int iBillingCycleID,vector<long> &rvAcctItemAggrData)
{
	unsigned int i;
	long iRet (0);
	//查找帐户的开始索引
	if(!m_pSHMFileExternal->m_poServAcctIndex->get(lAcctID,&i))
	{
		return 0;
	}
	//循环帐户下的每一个用户
	for (; i; i=m_pSHMFileExternal->m_poServAcct[i].m_iAcctNextOffset)
	{
		unsigned int j;
		if(m_pSHMFileExternal->m_poServAcct[i].m_iState == 1)
		{
			//根据用户ID查找用户帐目的开始索引
			if(!AcctItemBase::m_poServIndex->get(m_pSHMFileExternal->m_poServAcct[i].m_lServID,&j))
			{
				continue;
			}
			//循环每一个用户帐目
			if(m_pSHMFileExternal->m_poServAcct[i].m_lServID==lServID)
			{
				for (; j;j=m_poAcctItem[j].m_iServNext)
				{
					AcctItemData * p = m_poAcctItem +j;
					if(p->m_iBillingCycleID==iBillingCycleID &&
					   p->m_iPayMeasure==PAY_MEASURE_MONEY)
					   {
					   		iRet+=p->m_lValue;
					   		//添加地址信息
					   		rvAcctItemAggrData.push_back((long)(p));
					   }
				}
			}
	  }
	}
	return iRet;
}

int  AcctFeeGetter::getOfferID(long lProdOfferInstID)
{
	unsigned int k;
	if (!m_pSHMFileExternal->m_poProdOfferInsIndex->get (lProdOfferInstID, &k)) return 0;
	return m_pSHMFileExternal->m_poProdOfferIns[k].m_iOfferID;
}

#else

//根据帐户ID和查询帐期得到用户的费用
long AcctFeeGetter::getFee(long lAcctID, int iBillingCycleID)
{
	unsigned int i;
	long lRet (0);
	//查找帐户的开始索引
	if(!m_poServAcctIndex->get(lAcctID,&i))
	{
		return 0;
	}
	//循环帐户下的每一个用户
	for (; i; i=m_poServAcct[i].m_iAcctNextOffset)
	{
		unsigned int j;
		if(m_poServAcct[i].m_iState == 1)
		{
			//根据用户ID查找用户帐目的开始索引
			if(!AcctItemBase::m_poServIndex->get(m_poServAcct[i].m_lServID,&j))
			{
				continue;
			}
			//循环每一个用户帐目
			for (; j;j=m_poAcctItem[j].m_iServNext)
			{
				AcctItemData * p = m_poAcctItem +j;
				if(p->m_iBillingCycleID==iBillingCycleID &&
				   p->m_iPayMeasure==PAY_MEASURE_MONEY)
				   {
				   		lRet+=p->m_lValue;
				   }
			}
	  }
	}
	return lRet;
}

//根据帐户ID和查询帐期得到用户的费用
long AcctFeeGetter::getFee(long lAcctID, int iBillingCycleID,vector<long> &rvAcctItemAggrData)
{
	unsigned int i,k,j,h=0,len=0;
	long iRet (0);
	long lVal=0;
	//查找帐户的开始索引
	if(!m_poServAcctIndex->get(lAcctID,&i))
	{
		return 0;
	}
	k = i;
	//取得数组长度
	for (; i; i=m_poServAcct[i].m_iAcctNextOffset)
	{
		if(m_poServAcct[i].m_iState == 1)
		{
			if(!AcctItemBase::m_poServIndex->get(m_poServAcct[i].m_lServID,&j))
			{
				continue;
			}
	  }
		len++;
	}
	long * lServFee = new long[len*2];
	//取帐户下所有用户及索引
	len = 0;
	for (; k; k=m_poServAcct[k].m_iAcctNextOffset)
	{
		if(m_poServAcct[k].m_iState == 1)
		{
			if(!AcctItemBase::m_poServIndex->get(m_poServAcct[k].m_lServID,&j))
			{
				continue;
			}
			*(lServFee+(len*2)+0)=m_poServAcct[k].m_lServID;
			*(lServFee+(len*2)+1)=j;
			len++;
	  }
	}
	//对用户排序
	j = 1;
	for (i=0; i<len; i++)
	{
		for (; j<len; j++)
		{
			if(*(lServFee+(i*2)+0)>*(lServFee+(j*2)+0))
			{
				lVal = *(lServFee+(j*2)+0);
				h = *(lServFee+(j*2)+1);
				*(lServFee+(j*2)+0) = *(lServFee+(i*2)+0);
				*(lServFee+(j*2)+1) = *(lServFee+(i*2)+1);
				*(lServFee+(i*2)+0) = lVal;
				*(lServFee+(i*2)+1) = h;
			}
		}
		j=i+2;
	}
	for (i=0; i<len; i++)
	{
		j = *(lServFee+(i*2)+1);
		//循环每一个用户帐目
		for (; j; j=m_poAcctItem[j].m_iServNext)
		{
			AcctItemData * p = m_poAcctItem + j;
			if(p->m_iBillingCycleID==iBillingCycleID &&
			   p->m_iPayMeasure==PAY_MEASURE_MONEY)
			   {
			   		iRet+=p->m_lValue;
			   		//添加地址信息
			   		rvAcctItemAggrData.push_back((long)(p));
			   }
		}
	}
	
	/*
	//循环帐户下的每一个用户
	for (; i; i=m_poServAcct[i].m_iAcctNextOffset)
	{
		unsigned int j;
		//根据用户ID查找用户帐目的开始索引
		if(!AcctItemBase::m_poServIndex->get(m_poServAcct[i].m_lServID,&j))
		{
			continue;
		}
		//循环每一个用户帐目
		for (; j;j=m_poAcctItem[j].m_iServNext)
		{
			AcctItemData * p = m_poAcctItem +j;
			if(p->m_iBillingCycleID==iBillingCycleID &&
			   p->m_iPayMeasure==PAY_MEASURE_MONEY)
			   {
			   		iRet+=p->m_lValue;
			   		//添加地址信息
			   		rvAcctItemAggrData.push_back((long)(p));
			   }
		}
	}
	*/
	delete [] lServFee;
	return iRet;	
}

//根据用户ID和帐期得到用户的费用
long AcctFeeGetter::getServFee(long lServID, int iBillingCycleID,vector<long> &rvAcctItemAggrData)
{
	long iRet (0);
	unsigned int j;
	//根据用户ID查找用户帐目的开始索引
	if(!AcctItemBase::m_poServIndex->get(lServID,&j))
	{
		return iRet;
	}
	//循环每一个用户帐目
	for (; j;j=m_poAcctItem[j].m_iServNext)
	{
		AcctItemData * p = m_poAcctItem +j;
		if(p->m_iBillingCycleID==iBillingCycleID &&
		   p->m_iPayMeasure==PAY_MEASURE_MONEY)
		{
		   		iRet+=p->m_lValue;
		   		//添加地址信息
		   		rvAcctItemAggrData.push_back((long)(p));
		}
	}
	return iRet;
}

long AcctFeeGetter::getFee(long lAcctID, long lServID,int iBillingCycleID,vector<long> &rvAcctItemAggrData)
{
	unsigned int i;
	long iRet (0);
	//查找帐户的开始索引
	if(!m_poServAcctIndex->get(lAcctID,&i))
	{
		return 0;
	}
	//循环帐户下的每一个用户
	for (; i; i=m_poServAcct[i].m_iAcctNextOffset)
	{
		unsigned int j;
		if(m_poServAcct[i].m_iState == 1)
		{
			//根据用户ID查找用户帐目的开始索引
			if(!AcctItemBase::m_poServIndex->get(m_poServAcct[i].m_lServID,&j))
			{
				continue;
			}
			//循环每一个用户帐目
			if(m_poServAcct[i].m_lServID==lServID)
			{
				for (; j;j=m_poAcctItem[j].m_iServNext)
				{
					AcctItemData * p = m_poAcctItem +j;
					if(p->m_iBillingCycleID==iBillingCycleID &&
					   p->m_iPayMeasure==PAY_MEASURE_MONEY)
					   {
					   		iRet+=p->m_lValue;
					   		//添加地址信息
					   		rvAcctItemAggrData.push_back((long)(p));
					   }
				}
			}
	  }
	}
	return iRet;
}

int  AcctFeeGetter::getOfferID(long lProdOfferInstID)
{
	unsigned int k;
	if (!m_poProdOfferInsIndex->get (lProdOfferInstID, &k)) return 0;
	return m_poProdOfferIns[k].m_iOfferID;
}

#endif

void AcctFeeGetter::dataConvert(vector<long> &vInAcctItemAggrData,vector<AcctItemDataEx> &vOutAcctItemDataEx)
{
	AcctItemDataEx data;
	AcctItemData * pTemp;
	for (int i =0;i<vInAcctItemAggrData.size();i++)
	{
		pTemp = (struct AcctItemData *)(vInAcctItemAggrData[i]);
		data.m_lAcctItemID		= pTemp->m_lAcctItemID;
		data.m_lServID			= pTemp->m_lServID;
		data.m_lAcctID			= pTemp->m_lAcctID;
		data.m_lOfferInstID		= pTemp->m_lOfferInstID;
		data.m_iBillingCycleID	= pTemp->m_iBillingCycleID;
		data.m_lValue			= pTemp->m_lValue;
		data.m_iPayMeasure		= pTemp->m_iPayMeasure;
		data.m_iPayItemTypeID	= pTemp->m_iPayItemTypeID;
		strcpy(data.m_sStateDate,pTemp->m_sStateDate);
		data.m_iItemSourceID	= pTemp->m_iItemSourceID;
		data.m_iItemClassID		= pTemp->m_iItemClassID;
		data.m_iOfferID			= getOfferID(pTemp->m_lOfferInstID);
		data.m_iServNext = 0;
		vOutAcctItemDataEx.push_back(data);
	}
	return;
}

long AcctFeeGetter::getFeeEx(long lAcctID, int iBillingCycleID,vector<AcctItemDataEx> &vAcctItemDataEx)
{
	long lRet;
	vector <long> vAcctItemAggrData;
	vAcctItemAggrData.clear();
	lRet = getFee(lAcctID,iBillingCycleID,vAcctItemAggrData);
	dataConvert(vAcctItemAggrData,vAcctItemDataEx);
	return lRet;
}

long AcctFeeGetter::getFeeEx(long lAcctID, long lServID,int iBillingCycleID,vector<AcctItemDataEx> &vAcctItemDataEx)
{
	long lRet;
	vector <long> vAcctItemAggrData;
	vAcctItemAggrData.clear();
	lRet = getFee(lAcctID,lServID,iBillingCycleID,vAcctItemAggrData);
	dataConvert(vAcctItemAggrData,vAcctItemDataEx);
	return lRet;
}

long AcctFeeGetter::getServFeeEx(long lServID, int iBillingCycleID,vector<AcctItemDataEx> &vAcctItemDataEx)
{
	long lRet;
	vector <long> vAcctItemAggrData;
	vAcctItemAggrData.clear();
	lRet = getServFee(lServID,iBillingCycleID,vAcctItemAggrData);
	dataConvert(vAcctItemAggrData,vAcctItemDataEx);
	return lRet;
}
