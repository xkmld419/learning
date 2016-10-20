/*VER: 1*/ 
#ifndef _TBL_SERV_ACCT_CLASS_H__
#define _TBL_SERV_ACCT_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
using namespace std;
typedef struct _TBL_SERV_ACCT_STR_{
	long	ACCT_ID;
	long	SERV_ID;
	long	ITEM_GROUP_ID;
	long	BILL_REQUIRE_ID;
	long	INVOICE_REQUIRE_ID;
	long	PAYMENT_RULE_ID;
	char	STATE[4];
	char	STATE_DATE[15];
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "ACCT_ID:["<<ACCT_ID<<"]"<<endl;
			os << "SERV_ID:["<<SERV_ID<<"]"<<endl;
			os << "ITEM_GROUP_ID:["<<ITEM_GROUP_ID<<"]"<<endl;
			os << "BILL_REQUIRE_ID:["<<BILL_REQUIRE_ID<<"]"<<endl;
			os << "INVOICE_REQUIRE_ID:["<<INVOICE_REQUIRE_ID<<"]"<<endl;
			os << "PAYMENT_RULE_ID:["<<PAYMENT_RULE_ID<<"]"<<endl;
			os << "STATE:["<<STATE<<"]"<<endl;
			os << "STATE_DATE:["<<STATE_DATE<<"]"<<endl;
	};
	_TBL_SERV_ACCT_STR_(){
			ACCT_ID=0;
			SERV_ID=0;
			ITEM_GROUP_ID=0;
			BILL_REQUIRE_ID=0;
			INVOICE_REQUIRE_ID=0;
			PAYMENT_RULE_ID=0;
			STATE[0]='\0';
			STATE_DATE[0]='\0';
	};
	void	ClearBuf(void){
			memset(&ACCT_ID,0,sizeof(_TBL_SERV_ACCT_STR_));
	};
}_TBL_SERV_ACCT_STR_;

inline ostream& operator<< (ostream& os, const _TBL_SERV_ACCT_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_SERV_ACCT_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_SERV_ACCT_CLASS{
	friend class MEM_SERV_ACCT_CLASS;
	private:
		_TBL_SERV_ACCT_STR_		item;
		_TBL_SERV_ACCT_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_SERV_ACCT_CLASS();
		~TBL_SERV_ACCT_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_ACCT_ID(void);
		long	Get_SERV_ID(void);
		long	Get_ITEM_GROUP_ID(void);
		long	Get_BILL_REQUIRE_ID(void);
		long	Get_INVOICE_REQUIRE_ID(void);
		long	Get_PAYMENT_RULE_ID(void);
		char*	Get_STATE(void);
		char*	Get_STATE_DATE(void);
		void	Set_ACCT_ID(long	lParam);
		void	Set_SERV_ID(long	lParam);
		void	Set_ITEM_GROUP_ID(long	lParam);
		void	Set_BILL_REQUIRE_ID(long	lParam);
		void	Set_INVOICE_REQUIRE_ID(long	lParam);
		void	Set_PAYMENT_RULE_ID(long	lParam);
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

inline ostream& operator<< (ostream& os, const TBL_SERV_ACCT_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_SERV_ACCT_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_SERV_ACCT_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_SERV_ACCT_STR_>		a_item;
		static	hash_map<const long ,_TBL_SERV_ACCT_STR_* , hash<long>, MEM_SERV_ACCT_CLASS::eqlong>		hashmap;
	public:
		MEM_SERV_ACCT_CLASS(char * sTableName=NULL);
		~MEM_SERV_ACCT_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_SERV_ACCT_STR_ *	operator [](long opt);
};

#endif
