/*VER: 1*/ 
#ifndef _TBL_BILL_CLASS_H__
#define _TBL_BILL_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
typedef struct _TBL_BILL_STR_{
	long	BILL_ID;
	long	PAYMENT_ID;
	long	PAYMENT_METHOD;
	long	BILLING_CYCLE_ID;
	long	ACCT_ID;
	long	SERV_ID;
	char	ACC_NBR[33];
	long	BILL_AMOUNT;
	long	LATE_FEE;
	long	DERATE_LATE_FEE;
	long	BALANCE;
	long	LAST_CHANGE;
	long	CUR_CHANGE;
	char	CREATED_DATE[15];
	char	PAYMENT_DATE[15];
	long	USE_DERATE_BLANCE;
	long	INVOICE_ID;
	char	STATE[4];
	char	STATE_DATE[15];
	long	STAFF_ID;
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "BILL_ID:["<<BILL_ID<<"]"<<endl;
			os << "PAYMENT_ID:["<<PAYMENT_ID<<"]"<<endl;
			os << "PAYMENT_METHOD:["<<PAYMENT_METHOD<<"]"<<endl;
			os << "BILLING_CYCLE_ID:["<<BILLING_CYCLE_ID<<"]"<<endl;
			os << "ACCT_ID:["<<ACCT_ID<<"]"<<endl;
			os << "SERV_ID:["<<SERV_ID<<"]"<<endl;
			os << "ACC_NBR:["<<ACC_NBR<<"]"<<endl;
			os << "BILL_AMOUNT:["<<BILL_AMOUNT<<"]"<<endl;
			os << "LATE_FEE:["<<LATE_FEE<<"]"<<endl;
			os << "DERATE_LATE_FEE:["<<DERATE_LATE_FEE<<"]"<<endl;
			os << "BALANCE:["<<BALANCE<<"]"<<endl;
			os << "LAST_CHANGE:["<<LAST_CHANGE<<"]"<<endl;
			os << "CUR_CHANGE:["<<CUR_CHANGE<<"]"<<endl;
			os << "CREATED_DATE:["<<CREATED_DATE<<"]"<<endl;
			os << "PAYMENT_DATE:["<<PAYMENT_DATE<<"]"<<endl;
			os << "USE_DERATE_BLANCE:["<<USE_DERATE_BLANCE<<"]"<<endl;
			os << "INVOICE_ID:["<<INVOICE_ID<<"]"<<endl;
			os << "STATE:["<<STATE<<"]"<<endl;
			os << "STATE_DATE:["<<STATE_DATE<<"]"<<endl;
			os << "STAFF_ID:["<<STAFF_ID<<"]"<<endl;
	};
	_TBL_BILL_STR_(){
			BILL_ID=0;
			PAYMENT_ID=0;
			PAYMENT_METHOD=0;
			BILLING_CYCLE_ID=0;
			ACCT_ID=0;
			SERV_ID=0;
			ACC_NBR[0]='\0';
			BILL_AMOUNT=0;
			LATE_FEE=0;
			DERATE_LATE_FEE=0;
			BALANCE=0;
			LAST_CHANGE=0;
			CUR_CHANGE=0;
			CREATED_DATE[0]='\0';
			PAYMENT_DATE[0]='\0';
			USE_DERATE_BLANCE=0;
			INVOICE_ID=0;
			STATE[0]='\0';
			STATE_DATE[0]='\0';
			STAFF_ID=0;
	};
	void	ClearBuf(void){
			memset(&BILL_ID,0,sizeof(_TBL_BILL_STR_));
	};
}_TBL_BILL_STR_;

inline ostream& operator<< (ostream& os, const _TBL_BILL_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_BILL_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_BILL_CLASS{
	friend class MEM_BILL_CLASS;
	private:
		_TBL_BILL_STR_		item;
		_TBL_BILL_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_BILL_CLASS();
		~TBL_BILL_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_BILL_ID(void);
		long	Get_PAYMENT_ID(void);
		long	Get_PAYMENT_METHOD(void);
		long	Get_BILLING_CYCLE_ID(void);
		long	Get_ACCT_ID(void);
		long	Get_SERV_ID(void);
		char*	Get_ACC_NBR(void);
		long	Get_BILL_AMOUNT(void);
		long	Get_LATE_FEE(void);
		long	Get_DERATE_LATE_FEE(void);
		long	Get_BALANCE(void);
		long	Get_LAST_CHANGE(void);
		long	Get_CUR_CHANGE(void);
		char*	Get_CREATED_DATE(void);
		char*	Get_PAYMENT_DATE(void);
		long	Get_USE_DERATE_BLANCE(void);
		long	Get_INVOICE_ID(void);
		char*	Get_STATE(void);
		char*	Get_STATE_DATE(void);
		long	Get_STAFF_ID(void);
		void	Set_BILL_ID(long	lParam);
		void	Set_PAYMENT_ID(long	lParam);
		void	Set_PAYMENT_METHOD(long	lParam);
		void	Set_BILLING_CYCLE_ID(long	lParam);
		void	Set_ACCT_ID(long	lParam);
		void	Set_SERV_ID(long	lParam);
		void	Set_ACC_NBR(char	* sParam);
		void	Set_BILL_AMOUNT(long	lParam);
		void	Set_LATE_FEE(long	lParam);
		void	Set_DERATE_LATE_FEE(long	lParam);
		void	Set_BALANCE(long	lParam);
		void	Set_LAST_CHANGE(long	lParam);
		void	Set_CUR_CHANGE(long	lParam);
		void	Set_CREATED_DATE(char	* sParam);
		void	Set_PAYMENT_DATE(char	* sParam);
		void	Set_USE_DERATE_BLANCE(long	lParam);
		void	Set_INVOICE_ID(long	lParam);
		void	Set_STATE(char	* sParam);
		void	Set_STATE_DATE(char	* sParam);
		void	Set_STAFF_ID(long	lParam);
		void	AddItem(void);
		void	ClearBuf(void);
		void	SetBuf(int iBufLen=ARRAY_BUFLEN);
		void	SaveFlush(void);
		void	FreeBuf(void);
		void	Print(ostream& os) const {
				os<<item;
		};
};

inline ostream& operator<< (ostream& os, const TBL_BILL_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_BILL_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_BILL_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_BILL_STR_>		a_item;
		static	hash_map<const long ,_TBL_BILL_STR_* , hash<long>, MEM_BILL_CLASS::eqlong>		hashmap;
	public:
		MEM_BILL_CLASS(char * sTableName=NULL);
		~MEM_BILL_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_BILL_STR_ *	operator [](long opt);
};

#endif
