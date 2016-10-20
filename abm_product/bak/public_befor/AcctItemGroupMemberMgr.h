/*VER: 1*/ 
#ifndef ACCTITEM_GROUPMEMBER_MGR
#define ACCTITEM_GROUPMEMBER_MGR

#include <iostream>

#ifdef DEF_HP
#include <rw/stdex/hashmmap.h>
#else
#include <hash_map.h>
#endif

class ACCT_ITEM_GROUP_MEMBER
{
	public:
	friend class ACCT_ITEM_GROUP_MEMBER_MGR;
	int	iAcctItemTypeID;
	int	iItemGroupID;
	struct eqint
	{
		bool operator()(const int s1, const int s2) const
  	{
    	return (s1==s2);
  	}
	};	
};

typedef hash_multimap<const int , ACCT_ITEM_GROUP_MEMBER, hash<int>,ACCT_ITEM_GROUP_MEMBER::eqint> HASH_MULTIMAPINT;

class ACCT_ITEM_GROUP_MEMBER_MGR{
		private:
			static HASH_MULTIMAPINT hash_multimapint;
			static bool bLoadFlag;
		public:
			ACCT_ITEM_GROUP_MEMBER_MGR();
			~ACCT_ITEM_GROUP_MEMBER_MGR();
			struct const_iterator{
					pair<HASH_MULTIMAPINT::const_iterator, HASH_MULTIMAPINT::const_iterator> p ;
					HASH_MULTIMAPINT::const_iterator i;
					
					const_iterator(int iAcctItemGroup)
					{
							p=hash_multimapint.equal_range(iAcctItemGroup);
							i=p.first;
					};
					
					const ACCT_ITEM_GROUP_MEMBER* next()
					{
							if (i==p.second)
								return NULL;
							else{
								return &((*i++).second);
							}
					};
					
			};
};

#endif
