/*VER: 1*/ 
#ifndef _TBL_ACCT_BALANCE_CLASS_H__
#define _TBL_ACCT_BALANCE_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
using namespace std;
typedef struct _TBL_ACCT_BALANCE_STR_{
	long	ACCT_BALANCE_ID;
	long	BALANCE_TYPE_ID;
	char	EFF_DATE[15];
	char	EXP_DATE[15];
	long	BALANCE;
	long	CYCLE_UPPER;
	long	CYCLE_LOWER;
	char	CYCLE_UPPER_TYPE[4];
	char	CYCLE_LOWER_TYPE[4];
	char	BANK_ACCT[31];
	char	STATE[4];
	char	STATE_DATE[15];
	long	ACCT_ID;
	long	SERV_ID;
	long	ITEM_GROUP_ID;
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "ACCT_BALANCE_ID:["<<ACCT_BALANCE_ID<<"]"<<endl;
			os << "BALANCE_TYPE_ID:["<<BALANCE_TYPE_ID<<"]"<<endl;
			os << "EFF_DATE:["<<EFF_DATE<<"]"<<endl;
			os << "EXP_DATE:["<<EXP_DATE<<"]"<<endl;
			os << "BALANCE:["<<BALANCE<<"]"<<endl;
			os << "CYCLE_UPPER:["<<CYCLE_UPPER<<"]"<<endl;
			os << "CYCLE_LOWER:["<<CYCLE_LOWER<<"]"<<endl;
			os << "CYCLE_UPPER_TYPE:["<<CYCLE_UPPER_TYPE<<"]"<<endl;
			os << "CYCLE_LOWER_TYPE:["<<CYCLE_LOWER_TYPE<<"]"<<endl;
			os << "BANK_ACCT:["<<BANK_ACCT<<"]"<<endl;
			os << "STATE:["<<STATE<<"]"<<endl;
			os << "STATE_DATE:["<<STATE_DATE<<"]"<<endl;
			os << "ACCT_ID:["<<ACCT_ID<<"]"<<endl;
			os << "SERV_ID:["<<SERV_ID<<"]"<<endl;
			os << "ITEM_GROUP_ID:["<<ITEM_GROUP_ID<<"]"<<endl;
	};
	_TBL_ACCT_BALANCE_STR_(){
			ACCT_BALANCE_ID=0;
			BALANCE_TYPE_ID=0;
			EFF_DATE[0]='\0';
			EXP_DATE[0]='\0';
			BALANCE=0;
			CYCLE_UPPER=0;
			CYCLE_LOWER=0;
			CYCLE_UPPER_TYPE[0]='\0';
			CYCLE_LOWER_TYPE[0]='\0';
			BANK_ACCT[0]='\0';
			STATE[0]='\0';
			STATE_DATE[0]='\0';
			ACCT_ID=0;
			SERV_ID=0;
			ITEM_GROUP_ID=0;
	};
	void	ClearBuf(void){
			memset(&ACCT_BALANCE_ID,0,sizeof(_TBL_ACCT_BALANCE_STR_));
	};
}_TBL_ACCT_BALANCE_STR_;

inline ostream& operator<< (ostream& os, const _TBL_ACCT_BALANCE_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_ACCT_BALANCE_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_ACCT_BALANCE_CLASS{
	friend class MEM_ACCT_BALANCE_CLASS;
	private:
		_TBL_ACCT_BALANCE_STR_		item;
		_TBL_ACCT_BALANCE_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_ACCT_BALANCE_CLASS();
		~TBL_ACCT_BALANCE_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_ACCT_BALANCE_ID(void);
		long	Get_BALANCE_TYPE_ID(void);
		char*	Get_EFF_DATE(void);
		char*	Get_EXP_DATE(void);
		long	Get_BALANCE(void);
		long	Get_CYCLE_UPPER(void);
		long	Get_CYCLE_LOWER(void);
		char*	Get_CYCLE_UPPER_TYPE(void);
		char*	Get_CYCLE_LOWER_TYPE(void);
		char*	Get_BANK_ACCT(void);
		char*	Get_STATE(void);
		char*	Get_STATE_DATE(void);
		long	Get_ACCT_ID(void);
		long	Get_SERV_ID(void);
		long	Get_ITEM_GROUP_ID(void);
		void	Set_ACCT_BALANCE_ID(long	lParam);
		void	Set_BALANCE_TYPE_ID(long	lParam);
		void	Set_EFF_DATE(char	* sParam);
		void	Set_EXP_DATE(char	* sParam);
		void	Set_BALANCE(long	lParam);
		void	Set_CYCLE_UPPER(long	lParam);
		void	Set_CYCLE_LOWER(long	lParam);
		void	Set_CYCLE_UPPER_TYPE(char	* sParam);
		void	Set_CYCLE_LOWER_TYPE(char	* sParam);
		void	Set_BANK_ACCT(char	* sParam);
		void	Set_STATE(char	* sParam);
		void	Set_STATE_DATE(char	* sParam);
		void	Set_ACCT_ID(long	lParam);
		void	Set_SERV_ID(long	lParam);
		void	Set_ITEM_GROUP_ID(long	lParam);
		void	AddItem(void);
		void	ClearBuf(void);
		void	SetBuf(int iBufLen=ARRAY_BUFLEN);
		void	SaveFlush(void);
		void	FreeBuf(void);
		void	Print(ostream& os) const {
				os<<item;
		};
};

inline ostream& operator<< (ostream& os, const TBL_ACCT_BALANCE_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_ACCT_BALANCE_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_ACCT_BALANCE_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_ACCT_BALANCE_STR_>		a_item;
		static	hash_map<const long ,_TBL_ACCT_BALANCE_STR_* , hash<long>, MEM_ACCT_BALANCE_CLASS::eqlong>		hashmap;
	public:
		MEM_ACCT_BALANCE_CLASS(char * sTableName=NULL);
		~MEM_ACCT_BALANCE_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_ACCT_BALANCE_STR_ *	operator [](long opt);
};

#endif
