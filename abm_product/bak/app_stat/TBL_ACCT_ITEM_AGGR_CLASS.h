/*VER: 1*/ 
#ifndef _TBL_ACCT_ITEM_AGGR_CLASS_H__
#define _TBL_ACCT_ITEM_AGGR_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
using namespace std;
typedef struct _TBL_ACCT_ITEM_AGGR_STR_{
	long	ACCT_ITEM_ID;
	long	SERV_ID;
	long	BILLING_CYCLE_ID;
	long	ACCT_ITEM_TYPE_ID;
	long	CHARGE;
	long	ACCT_ID;
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "ACCT_ITEM_ID:["<<ACCT_ITEM_ID<<"]"<<endl;
			os << "SERV_ID:["<<SERV_ID<<"]"<<endl;
			os << "BILLING_CYCLE_ID:["<<BILLING_CYCLE_ID<<"]"<<endl;
			os << "ACCT_ITEM_TYPE_ID:["<<ACCT_ITEM_TYPE_ID<<"]"<<endl;
			os << "CHARGE:["<<CHARGE<<"]"<<endl;
			os << "ACCT_ID:["<<ACCT_ID<<"]"<<endl;
	};
	_TBL_ACCT_ITEM_AGGR_STR_(){
			ACCT_ITEM_ID=0;
			SERV_ID=0;
			BILLING_CYCLE_ID=0;
			ACCT_ITEM_TYPE_ID=0;
			CHARGE=0;
			ACCT_ID=0;
	};
	void	ClearBuf(void){
			memset(&ACCT_ITEM_ID,0,sizeof(_TBL_ACCT_ITEM_AGGR_STR_));
	};
}_TBL_ACCT_ITEM_AGGR_STR_;

inline ostream& operator<< (ostream& os, const _TBL_ACCT_ITEM_AGGR_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_ACCT_ITEM_AGGR_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_ACCT_ITEM_AGGR_CLASS{
	friend class MEM_ACCT_ITEM_AGGR_CLASS;
	private:
		_TBL_ACCT_ITEM_AGGR_STR_		item;
		_TBL_ACCT_ITEM_AGGR_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_ACCT_ITEM_AGGR_CLASS();
		~TBL_ACCT_ITEM_AGGR_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_ACCT_ITEM_ID(void);
		long	Get_SERV_ID(void);
		long	Get_BILLING_CYCLE_ID(void);
		long	Get_ACCT_ITEM_TYPE_ID(void);
		long	Get_CHARGE(void);
		long	Get_ACCT_ID(void);
		void	Set_ACCT_ITEM_ID(long	lParam);
		void	Set_SERV_ID(long	lParam);
		void	Set_BILLING_CYCLE_ID(long	lParam);
		void	Set_ACCT_ITEM_TYPE_ID(long	lParam);
		void	Set_CHARGE(long	lParam);
		void	Set_ACCT_ID(long	lParam);
		void	AddItem(void);
		void	ClearBuf(void);
		void	SetBuf(int iBufLen=ARRAY_BUFLEN);
		void	SaveFlush(void);
		void	FreeBuf(void);
		void	Print(ostream& os) const {
				os<<item;
		};
};

inline ostream& operator<< (ostream& os, const TBL_ACCT_ITEM_AGGR_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_ACCT_ITEM_AGGR_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_ACCT_ITEM_AGGR_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_ACCT_ITEM_AGGR_STR_>		a_item;
		static	hash_map<const long ,_TBL_ACCT_ITEM_AGGR_STR_* , hash<long>, MEM_ACCT_ITEM_AGGR_CLASS::eqlong>		hashmap;
		static	bool	bLoadFlag;
	public:
		MEM_ACCT_ITEM_AGGR_CLASS(char * sTableName=NULL);
		~MEM_ACCT_ITEM_AGGR_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_ACCT_ITEM_AGGR_STR_ *	operator [](long opt);
};

#endif
