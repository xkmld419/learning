/*VER: 1*/ 
#ifndef _TBL_ACCT_ITEM_CLASS_H__
#define _TBL_ACCT_ITEM_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
typedef struct _TBL_ACCT_ITEM_STR_{
	long	ACCT_ITEM_ID;
	long	ITEM_SOURCE_ID;
	long	BILL_ID;
	long	ACCT_ITEM_TYPE_ID;
	long	BILLING_CYCLE_ID;
	long	ACCT_ID;
	long	SERV_ID;
	long	AMOUNT;
	char	CREATED_DATE[15];
	long	FEE_CYCLE_ID;
	long	PAYMENT_METHOD;
	char	STATE[4];
	char	STATE_DATE[15];
	char	INV_OFFER[4];
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "ACCT_ITEM_ID:["<<ACCT_ITEM_ID<<"]"<<endl;
			os << "ITEM_SOURCE_ID:["<<ITEM_SOURCE_ID<<"]"<<endl;
			os << "BILL_ID:["<<BILL_ID<<"]"<<endl;
			os << "ACCT_ITEM_TYPE_ID:["<<ACCT_ITEM_TYPE_ID<<"]"<<endl;
			os << "BILLING_CYCLE_ID:["<<BILLING_CYCLE_ID<<"]"<<endl;
			os << "ACCT_ID:["<<ACCT_ID<<"]"<<endl;
			os << "SERV_ID:["<<SERV_ID<<"]"<<endl;
			os << "AMOUNT:["<<AMOUNT<<"]"<<endl;
			os << "CREATED_DATE:["<<CREATED_DATE<<"]"<<endl;
			os << "FEE_CYCLE_ID:["<<FEE_CYCLE_ID<<"]"<<endl;
			os << "PAYMENT_METHOD:["<<PAYMENT_METHOD<<"]"<<endl;
			os << "STATE:["<<STATE<<"]"<<endl;
			os << "STATE_DATE:["<<STATE_DATE<<"]"<<endl;
			os << "INV_OFFER:["<<INV_OFFER<<"]"<<endl;
	};
	_TBL_ACCT_ITEM_STR_(){
			ACCT_ITEM_ID=0;
			ITEM_SOURCE_ID=0;
			BILL_ID=0;
			ACCT_ITEM_TYPE_ID=0;
			BILLING_CYCLE_ID=0;
			ACCT_ID=0;
			SERV_ID=0;
			AMOUNT=0;
			CREATED_DATE[0]='\0';
			FEE_CYCLE_ID=0;
			PAYMENT_METHOD=0;
			STATE[0]='\0';
			STATE_DATE[0]='\0';
			INV_OFFER[0]='\0';
	};
	void	ClearBuf(void){
			memset(&ACCT_ITEM_ID,0,sizeof(_TBL_ACCT_ITEM_STR_));
	};
}_TBL_ACCT_ITEM_STR_;

inline ostream& operator<< (ostream& os, const _TBL_ACCT_ITEM_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_ACCT_ITEM_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_ACCT_ITEM_CLASS{
	friend class MEM_ACCT_ITEM_CLASS;
	private:
		_TBL_ACCT_ITEM_STR_		item;
		_TBL_ACCT_ITEM_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_ACCT_ITEM_CLASS();
		~TBL_ACCT_ITEM_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_ACCT_ITEM_ID(void);
		long	Get_ITEM_SOURCE_ID(void);
		long	Get_BILL_ID(void);
		long	Get_ACCT_ITEM_TYPE_ID(void);
		long	Get_BILLING_CYCLE_ID(void);
		long	Get_ACCT_ID(void);
		long	Get_SERV_ID(void);
		long	Get_AMOUNT(void);
		char*	Get_CREATED_DATE(void);
		long	Get_FEE_CYCLE_ID(void);
		long	Get_PAYMENT_METHOD(void);
		char*	Get_STATE(void);
		char*	Get_STATE_DATE(void);
		char*	Get_INV_OFFER(void);
		void	Set_ACCT_ITEM_ID(long	lParam);
		void	Set_ITEM_SOURCE_ID(long	lParam);
		void	Set_BILL_ID(long	lParam);
		void	Set_ACCT_ITEM_TYPE_ID(long	lParam);
		void	Set_BILLING_CYCLE_ID(long	lParam);
		void	Set_ACCT_ID(long	lParam);
		void	Set_SERV_ID(long	lParam);
		void	Set_AMOUNT(long	lParam);
		void	Set_CREATED_DATE(char	* sParam);
		void	Set_FEE_CYCLE_ID(long	lParam);
		void	Set_PAYMENT_METHOD(long	lParam);
		void	Set_STATE(char	* sParam);
		void	Set_STATE_DATE(char	* sParam);
		void	Set_INV_OFFER(char	* sParam);
		void	AddItem(void);
		void	ClearBuf(void);
		void	SetBuf(int iBufLen=ARRAY_BUFLEN);
		void	SaveFlush(void);
		void	FreeBuf(void);
		void	Print(ostream& os) const {
				os<<item;
		};
};

inline ostream& operator<< (ostream& os, const TBL_ACCT_ITEM_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_ACCT_ITEM_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_ACCT_ITEM_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_ACCT_ITEM_STR_>		a_item;
		static	hash_map<const long ,_TBL_ACCT_ITEM_STR_* , hash<long>, MEM_ACCT_ITEM_CLASS::eqlong>		hashmap;
	public:
		MEM_ACCT_ITEM_CLASS(char * sTableName=NULL);
		~MEM_ACCT_ITEM_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_ACCT_ITEM_STR_ *	operator [](long opt);
};

#endif
