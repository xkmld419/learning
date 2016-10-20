/*VER: 1*/ 
#ifndef _TBL_BALANCE_SOURCE_CLASS_H__
#define _TBL_BALANCE_SOURCE_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
typedef struct _TBL_BALANCE_SOURCE_STR_{
	long	PAYMENT_ID;
	long	BALANCE;
	long	OPER_INCOME_ID;
	long	ACCT_BALANCE_ID;
	char	OPER_TYPE[4];
	long	STAFF_ID;
	char	OPER_DATE[15];
	long	AMOUNT;
	char	SOURCE_TYPE[4];
	char	SOURCE_DESC[251];
	char	STATE[4];
	char	STATE_DATE[15];
	long	ACCT_BALANCE_OBJ_ID;
	char	INV_OFFER[4];
	char	ALLOW_DRAW[4];
	char	CURR_STATE[4];
	char	CURR_DATE[15];
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "PAYMENT_ID:["<<PAYMENT_ID<<"]"<<endl;
			os << "BALANCE:["<<BALANCE<<"]"<<endl;
			os << "OPER_INCOME_ID:["<<OPER_INCOME_ID<<"]"<<endl;
			os << "ACCT_BALANCE_ID:["<<ACCT_BALANCE_ID<<"]"<<endl;
			os << "OPER_TYPE:["<<OPER_TYPE<<"]"<<endl;
			os << "STAFF_ID:["<<STAFF_ID<<"]"<<endl;
			os << "OPER_DATE:["<<OPER_DATE<<"]"<<endl;
			os << "AMOUNT:["<<AMOUNT<<"]"<<endl;
			os << "SOURCE_TYPE:["<<SOURCE_TYPE<<"]"<<endl;
			os << "SOURCE_DESC:["<<SOURCE_DESC<<"]"<<endl;
			os << "STATE:["<<STATE<<"]"<<endl;
			os << "STATE_DATE:["<<STATE_DATE<<"]"<<endl;
			os << "ACCT_BALANCE_OBJ_ID:["<<ACCT_BALANCE_OBJ_ID<<"]"<<endl;
			os << "INV_OFFER:["<<INV_OFFER<<"]"<<endl;
			os << "ALLOW_DRAW:["<<ALLOW_DRAW<<"]"<<endl;
			os << "CURR_STATE:["<<CURR_STATE<<"]"<<endl;
			os << "CURR_DATE:["<<CURR_DATE<<"]"<<endl;
	};
	_TBL_BALANCE_SOURCE_STR_(){
			PAYMENT_ID=0;
			BALANCE=0;
			OPER_INCOME_ID=0;
			ACCT_BALANCE_ID=0;
			OPER_TYPE[0]='\0';
			STAFF_ID=0;
			OPER_DATE[0]='\0';
			AMOUNT=0;
			SOURCE_TYPE[0]='\0';
			SOURCE_DESC[0]='\0';
			STATE[0]='\0';
			STATE_DATE[0]='\0';
			ACCT_BALANCE_OBJ_ID=0;
			INV_OFFER[0]='\0';
			ALLOW_DRAW[0]='\0';
			CURR_STATE[0]='\0';
			CURR_DATE[0]='\0';
	};
	void	ClearBuf(void){
			memset(&PAYMENT_ID,0,sizeof(_TBL_BALANCE_SOURCE_STR_));
	};
}_TBL_BALANCE_SOURCE_STR_;

inline ostream& operator<< (ostream& os, const _TBL_BALANCE_SOURCE_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_BALANCE_SOURCE_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_BALANCE_SOURCE_CLASS{
	friend class MEM_BALANCE_SOURCE_CLASS;
	private:
		_TBL_BALANCE_SOURCE_STR_		item;
		_TBL_BALANCE_SOURCE_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_BALANCE_SOURCE_CLASS();
		~TBL_BALANCE_SOURCE_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_PAYMENT_ID(void);
		long	Get_BALANCE(void);
		long	Get_OPER_INCOME_ID(void);
		long	Get_ACCT_BALANCE_ID(void);
		char*	Get_OPER_TYPE(void);
		long	Get_STAFF_ID(void);
		char*	Get_OPER_DATE(void);
		long	Get_AMOUNT(void);
		char*	Get_SOURCE_TYPE(void);
		char*	Get_SOURCE_DESC(void);
		char*	Get_STATE(void);
		char*	Get_STATE_DATE(void);
		long	Get_ACCT_BALANCE_OBJ_ID(void);
		char*	Get_INV_OFFER(void);
		char*	Get_ALLOW_DRAW(void);
		char*	Get_CURR_STATE(void);
		char*	Get_CURR_DATE(void);
		void	Set_PAYMENT_ID(long	lParam);
		void	Set_BALANCE(long	lParam);
		void	Set_OPER_INCOME_ID(long	lParam);
		void	Set_ACCT_BALANCE_ID(long	lParam);
		void	Set_OPER_TYPE(char	* sParam);
		void	Set_STAFF_ID(long	lParam);
		void	Set_OPER_DATE(char	* sParam);
		void	Set_AMOUNT(long	lParam);
		void	Set_SOURCE_TYPE(char	* sParam);
		void	Set_SOURCE_DESC(char	* sParam);
		void	Set_STATE(char	* sParam);
		void	Set_STATE_DATE(char	* sParam);
		void	Set_ACCT_BALANCE_OBJ_ID(long	lParam);
		void	Set_INV_OFFER(char	* sParam);
		void	Set_ALLOW_DRAW(char	* sParam);
		void	Set_CURR_STATE(char	* sParam);
		void	Set_CURR_DATE(char	* sParam);
		void	AddItem(void);
		void	ClearBuf(void);
		void	SetBuf(int iBufLen=ARRAY_BUFLEN);
		void	SaveFlush(void);
		void	FreeBuf(void);
		void	Print(ostream& os) const {
				os<<item;
		};
};

inline ostream& operator<< (ostream& os, const TBL_BALANCE_SOURCE_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_BALANCE_SOURCE_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_BALANCE_SOURCE_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_BALANCE_SOURCE_STR_>		a_item;
		static	hash_map<const long ,_TBL_BALANCE_SOURCE_STR_* , hash<long>, MEM_BALANCE_SOURCE_CLASS::eqlong>		hashmap;
	public:
		MEM_BALANCE_SOURCE_CLASS(char * sTableName=NULL);
		~MEM_BALANCE_SOURCE_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_BALANCE_SOURCE_STR_ *	operator [](long opt);
};

#endif
