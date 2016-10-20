/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef TRANSACTIONMGR_H
#define TRANSACTIONMGR_H

#include "CommonMacro.h"
#include <vector>

#include "OfferAccuMgr.h"
#include "AcctItemAccu.h"
#include "StdEvent.h"

using namespace std;

/*
这个是兼容性开发了
*/

class AggrMgr : public AcctItemAccuMgr, public OfferAccuMgr
{
public:

	//事件累积量查询
		//查询用户指定帐期指定事件类型的结果
		long getEventAggr(long lServID,int iBillingCycleID,
	                                        int iEventAggrTypeID);	                                        
	    //查询用户指定帐期事件记录数及记录集合
		int getAllEventAggr(long lServID, int iBillingCycleID, 
	                                        vector<AccuData *> & v);
	                                        
	    //帐目累积量查询
		//查询用户指定帐期指定帐目类型的费用
		long getAcctItemAggr(long lServID,int iBillingCycleID,
	                                            int iAcctItemTypeID);	                                            
		long getPartyRoleAggr(long lServID, int iBillingCycleID,int iPartyRoleID);

		//查询用户指定帐期指定帐目类型的记录数及记录集合
		int getDisctItemAggr(long lServID, int iBillingCycleID, 
	                                    vector<AcctItemData *> & vAcctItemAggr);	    
	    //查询用户指定帐期记录数及记录集合
		int getAllItemAggr(long lServID, int iBillingCycleID, 
	                                    vector<AcctItemData *> & vAcctItemAggr,
										int iOrgBillingCycleID = 0);

		//修改内存中的数据
		long insertAcctItemAggr(AcctItemData &itemData, bool &bNew, bool isModifyAcct=false);

	    //帐户或帐户下用户帐目累积量查询
		/*
	    //查询指定帐户指定用户指定帐期记录数及记录集合
		int getAcctServAggr(long lAcctID,long lServID,int iBillingCycleID,
		                                        vector <long> &rvAcctItemData); 
	
	    //获得帐户下所有用户的指定帐期的费用
		int  getAcctAllAggr(long lAcctID, int iBillingCycleID);
		
		//获得帐户下所有用户的指定帐期的费用
		long  getAcctAllAggr(long lAcctID, int iBillingCycleID, 
	                                    vector<long> & rvAcctItemData);	                                
		*/
};

//##ModelId=42368B4B03BE
class TransactionMgr : public AcctItemAccuMgr, public OfferAccuMgr
{
  public:
    //##ModelId=42368E37000C
    TransactionMgr();

    //##ModelId=42368E41027D
    ~TransactionMgr();

	//事件累积量查询
		//查询用户指定帐期指定事件类型的结果
	    long selectEventAggr(long lServID,int iBillingCycleID,
	                                            int iEventAggrTypeID);
		//查询用户指定帐期事件记录数及记录集合
	    int selectAllEventAggr(long lServID, int iBillingCycleID, 
	                                        vector<AccuData *> & vEventAggr);

	//帐目累积量查询
		//查询用户指定帐期指定帐目类型的费用
	    long selectAcctItemAggr(long lServID,int iBillingCycleID,
	                                            int iAcctItemTypeID);                                            
		//查询用户指定帐期指定帐目类型的记录数及记录集合
	    int selectDisctItemAggr(long lServID, int iBillingCycleID, 
	                                    vector<AcctItemData *> & vAcctItemAggr);
	    //查询用户指定帐期记录数及记录集合
	    int selectAllItemAggr(long lServID, int iBillingCycleID, 
	                                vector<AcctItemData *> & vAcctItemAggr,int iOrgBillingCycleID = 0);

		//查询内存中用户历史所有总账记录数及记录集合
		int selectHisItemAggr(long lServID,vector<AcctItemData *> & vAcctItemAggr);

};

#endif /* TRANSACTIONMGR_H_HEADER_INCLUDED_BDBC8749 */
