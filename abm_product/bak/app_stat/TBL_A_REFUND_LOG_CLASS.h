/*VER: 1*/ 
#ifndef _TBL_A_REFUND_LOG_CLASS_H__
#define _TBL_A_REFUND_LOG_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
using namespace std;
typedef struct _TBL_A_REFUND_LOG_STR_{
	long	REFUND_LOG_ID;
	long	STAFF_ID;
	char	REFUND_REASON[251];
	long	CHARGE;
	char	CREATED_DATE[15];
	long	ACCT_ID;
	long	SERV_ID;
	long	OPER_PAYOUT_ID;
	char	STATE[4];
	long	OLD_REFUND_LOG_ID;
	long	A_REFUNDS_APPLY_ID;
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "REFUND_LOG_ID:["<<REFUND_LOG_ID<<"]"<<endl;
			os << "STAFF_ID:["<<STAFF_ID<<"]"<<endl;
			os << "REFUND_REASON:["<<REFUND_REASON<<"]"<<endl;
			os << "CHARGE:["<<CHARGE<<"]"<<endl;
			os << "CREATED_DATE:["<<CREATED_DATE<<"]"<<endl;
			os << "ACCT_ID:["<<ACCT_ID<<"]"<<endl;
			os << "SERV_ID:["<<SERV_ID<<"]"<<endl;
			os << "OPER_PAYOUT_ID:["<<OPER_PAYOUT_ID<<"]"<<endl;
			os << "STATE:["<<STATE<<"]"<<endl;
			os << "OLD_REFUND_LOG_ID:["<<OLD_REFUND_LOG_ID<<"]"<<endl;
			os << "A_REFUNDS_APPLY_ID:["<<A_REFUNDS_APPLY_ID<<"]"<<endl;
	};
	_TBL_A_REFUND_LOG_STR_(){
			REFUND_LOG_ID=0;
			STAFF_ID=0;
			REFUND_REASON[0]='\0';
			CHARGE=0;
			CREATED_DATE[0]='\0';
			ACCT_ID=0;
			SERV_ID=0;
			OPER_PAYOUT_ID=0;
			STATE[0]='\0';
			OLD_REFUND_LOG_ID=0;
			A_REFUNDS_APPLY_ID=0;
	};
	void	ClearBuf(void){
			memset(&REFUND_LOG_ID,0,sizeof(_TBL_A_REFUND_LOG_STR_));
	};
}_TBL_A_REFUND_LOG_STR_;

inline ostream& operator<< (ostream& os, const _TBL_A_REFUND_LOG_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_A_REFUND_LOG_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_A_REFUND_LOG_CLASS{
	friend class MEM_A_REFUND_LOG_CLASS;
	private:
		_TBL_A_REFUND_LOG_STR_		item;
		_TBL_A_REFUND_LOG_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_A_REFUND_LOG_CLASS();
		~TBL_A_REFUND_LOG_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_REFUND_LOG_ID(void);
		long	Get_STAFF_ID(void);
		char*	Get_REFUND_REASON(void);
		long	Get_CHARGE(void);
		char*	Get_CREATED_DATE(void);
		long	Get_ACCT_ID(void);
		long	Get_SERV_ID(void);
		long	Get_OPER_PAYOUT_ID(void);
		char*	Get_STATE(void);
		long	Get_OLD_REFUND_LOG_ID(void);
		long	Get_A_REFUNDS_APPLY_ID(void);
		void	Set_REFUND_LOG_ID(long	lParam);
		void	Set_STAFF_ID(long	lParam);
		void	Set_REFUND_REASON(char	* sParam);
		void	Set_CHARGE(long	lParam);
		void	Set_CREATED_DATE(char	* sParam);
		void	Set_ACCT_ID(long	lParam);
		void	Set_SERV_ID(long	lParam);
		void	Set_OPER_PAYOUT_ID(long	lParam);
		void	Set_STATE(char	* sParam);
		void	Set_OLD_REFUND_LOG_ID(long	lParam);
		void	Set_A_REFUNDS_APPLY_ID(long	lParam);
		void	AddItem(void);
		void	ClearBuf(void);
		void	SetBuf(int iBufLen=ARRAY_BUFLEN);
		void	SaveFlush(void);
		void	FreeBuf(void);
		void	Print(ostream& os) const {
				os<<item;
		};
};

inline ostream& operator<< (ostream& os, const TBL_A_REFUND_LOG_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_A_REFUND_LOG_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_A_REFUND_LOG_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_A_REFUND_LOG_STR_>		a_item;
		static	hash_map<const long ,_TBL_A_REFUND_LOG_STR_* , hash<long>, MEM_A_REFUND_LOG_CLASS::eqlong>		hashmap;
	public:
		MEM_A_REFUND_LOG_CLASS(char * sTableName=NULL);
		~MEM_A_REFUND_LOG_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_A_REFUND_LOG_STR_ *	operator [](long opt);
};

#endif
