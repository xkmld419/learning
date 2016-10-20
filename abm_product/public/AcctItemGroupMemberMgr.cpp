/*VER: 1*/ 
#include "AcctItemGroupMemberMgr.h"
#include "Environment.h"
HASH_MULTIMAPINT 	ACCT_ITEM_GROUP_MEMBER_MGR::hash_multimapint;
bool							ACCT_ITEM_GROUP_MEMBER_MGR::bLoadFlag=false;
	
ACCT_ITEM_GROUP_MEMBER_MGR::ACCT_ITEM_GROUP_MEMBER_MGR()
{	
		if (bLoadFlag) return;
		
		DEFINE_QUERY(qry);
		ACCT_ITEM_GROUP_MEMBER		temp;
		
		qry.close();
		qry.setSQL("SELECT ACCT_ITEM_TYPE_ID,ITEM_GROUP_ID FROM B_ACCT_ITEM_GROUP_MEMBER ");
		qry.open();
		for(;qry.next();){
				temp.iAcctItemTypeID=qry.field("ACCT_ITEM_TYPE_ID").asInteger();
				temp.iItemGroupID=qry.field("ITEM_GROUP_ID").asInteger();
				hash_multimapint.insert(HASH_MULTIMAPINT::value_type(temp.iItemGroupID,temp));
		}
		
		bLoadFlag=true;
}

ACCT_ITEM_GROUP_MEMBER_MGR::~ACCT_ITEM_GROUP_MEMBER_MGR()
{
		bLoadFlag=false;
		hash_multimapint.clear();
}


