/*VER: 1*/ 
#ifndef _TBL_BALANCE_PAYOUT_CLASS_H__
#define _TBL_BALANCE_PAYOUT_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
using namespace std;
typedef struct _TBL_BALANCE_PAYOUT_STR_{
	long	OPER_PAYOUT_ID;
	long	ACCT_BALANCE_ID;
	long	BILLING_CYCLE_ID;
	long	BILL_ID;
	char	OPER_TYPE[4];
	long	STAFF_ID;
	char	OPER_DATE[15];
	long	AMOUNT;
	long	BALANCE;
	long	PRN_COUNT;
	char	STATE[4];
	char	STATE_DATE[15];
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "OPER_PAYOUT_ID:["<<OPER_PAYOUT_ID<<"]"<<endl;
			os << "ACCT_BALANCE_ID:["<<ACCT_BALANCE_ID<<"]"<<endl;
			os << "BILLING_CYCLE_ID:["<<BILLING_CYCLE_ID<<"]"<<endl;
			os << "BILL_ID:["<<BILL_ID<<"]"<<endl;
			os << "OPER_TYPE:["<<OPER_TYPE<<"]"<<endl;
			os << "STAFF_ID:["<<STAFF_ID<<"]"<<endl;
			os << "OPER_DATE:["<<OPER_DATE<<"]"<<endl;
			os << "AMOUNT:["<<AMOUNT<<"]"<<endl;
			os << "BALANCE:["<<BALANCE<<"]"<<endl;
			os << "PRN_COUNT:["<<PRN_COUNT<<"]"<<endl;
			os << "STATE:["<<STATE<<"]"<<endl;
			os << "STATE_DATE:["<<STATE_DATE<<"]"<<endl;
	};
	_TBL_BALANCE_PAYOUT_STR_(){
			OPER_PAYOUT_ID=0;
			ACCT_BALANCE_ID=0;
			BILLING_CYCLE_ID=0;
			BILL_ID=0;
			OPER_TYPE[0]='\0';
			STAFF_ID=0;
			OPER_DATE[0]='\0';
			AMOUNT=0;
			BALANCE=0;
			PRN_COUNT=0;
			STATE[0]='\0';
			STATE_DATE[0]='\0';
	};
	void	ClearBuf(void){
			memset(&OPER_PAYOUT_ID,0,sizeof(_TBL_BALANCE_PAYOUT_STR_));
	};
}_TBL_BALANCE_PAYOUT_STR_;

inline ostream& operator<< (ostream& os, const _TBL_BALANCE_PAYOUT_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_BALANCE_PAYOUT_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_BALANCE_PAYOUT_CLASS{
	friend class MEM_BALANCE_PAYOUT_CLASS;
	private:
		_TBL_BALANCE_PAYOUT_STR_		item;
		_TBL_BALANCE_PAYOUT_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_BALANCE_PAYOUT_CLASS();
		~TBL_BALANCE_PAYOUT_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_OPER_PAYOUT_ID(void);
		long	Get_ACCT_BALANCE_ID(void);
		long	Get_BILLING_CYCLE_ID(void);
		long	Get_BILL_ID(void);
		char*	Get_OPER_TYPE(void);
		long	Get_STAFF_ID(void);
		char*	Get_OPER_DATE(void);
		long	Get_AMOUNT(void);
		long	Get_BALANCE(void);
		long	Get_PRN_COUNT(void);
		char*	Get_STATE(void);
		char*	Get_STATE_DATE(void);
		void	Set_OPER_PAYOUT_ID(long	lParam);
		void	Set_ACCT_BALANCE_ID(long	lParam);
		void	Set_BILLING_CYCLE_ID(long	lParam);
		void	Set_BILL_ID(long	lParam);
		void	Set_OPER_TYPE(char	* sParam);
		void	Set_STAFF_ID(long	lParam);
		void	Set_OPER_DATE(char	* sParam);
		void	Set_AMOUNT(long	lParam);
		void	Set_BALANCE(long	lParam);
		void	Set_PRN_COUNT(long	lParam);
		void	Set_STATE(char	* sParam);
		void	Set_STATE_DATE(char	* sParam);
		void	AddItem(void);
		void	ClearBuf(void);
		void	SetBuf(int iBufLen=ARRAY_BUFLEN);
		void	SaveFlush(void);
		void	FreeBuf(void);
		void	Print(ostream& os) const {
				os<<item;
		};
};

inline ostream& operator<< (ostream& os, const TBL_BALANCE_PAYOUT_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_BALANCE_PAYOUT_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_BALANCE_PAYOUT_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_BALANCE_PAYOUT_STR_>		a_item;
		static	hash_map<const long ,_TBL_BALANCE_PAYOUT_STR_* , hash<long>, MEM_BALANCE_PAYOUT_CLASS::eqlong>		hashmap;
	public:
		MEM_BALANCE_PAYOUT_CLASS(char * sTableName=NULL);
		~MEM_BALANCE_PAYOUT_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_BALANCE_PAYOUT_STR_ *	operator [](long opt);
};

#endif
