/*VER: 1*/ 
#ifndef _TBL_SERV_CLASS_H__
#define _TBL_SERV_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
typedef struct _TBL_SERV_STR_{
	long	SERV_ID;
	long	AGREEMENT_ID;
	long	CUST_ID;
	long	PRODUCT_ID;
	char	CREATE_DATE[15];
	char	STATE[4];
	char	STATE_DATE[15];
	char	RENT_DATE[15];
	char	NSC_DATE[15];
	long	CYCLE_TYPE_ID;
	long	BILLING_MODE_ID;
	char	ACC_NBR[33];
	long	FREE_TYPE_ID;
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "SERV_ID:["<<SERV_ID<<"]"<<endl;
			os << "AGREEMENT_ID:["<<AGREEMENT_ID<<"]"<<endl;
			os << "CUST_ID:["<<CUST_ID<<"]"<<endl;
			os << "PRODUCT_ID:["<<PRODUCT_ID<<"]"<<endl;
			os << "CREATE_DATE:["<<CREATE_DATE<<"]"<<endl;
			os << "STATE:["<<STATE<<"]"<<endl;
			os << "STATE_DATE:["<<STATE_DATE<<"]"<<endl;
			os << "RENT_DATE:["<<RENT_DATE<<"]"<<endl;
			os << "NSC_DATE:["<<NSC_DATE<<"]"<<endl;
			os << "CYCLE_TYPE_ID:["<<CYCLE_TYPE_ID<<"]"<<endl;
			os << "BILLING_MODE_ID:["<<BILLING_MODE_ID<<"]"<<endl;
			os << "ACC_NBR:["<<ACC_NBR<<"]"<<endl;
			os << "FREE_TYPE_ID:["<<FREE_TYPE_ID<<"]"<<endl;
	};
	_TBL_SERV_STR_(){
			SERV_ID=0;
			AGREEMENT_ID=0;
			CUST_ID=0;
			PRODUCT_ID=0;
			CREATE_DATE[0]='\0';
			STATE[0]='\0';
			STATE_DATE[0]='\0';
			RENT_DATE[0]='\0';
			NSC_DATE[0]='\0';
			CYCLE_TYPE_ID=0;
			BILLING_MODE_ID=0;
			ACC_NBR[0]='\0';
			FREE_TYPE_ID=0;
	};
	void	ClearBuf(void){
			memset(&SERV_ID,0,sizeof(_TBL_SERV_STR_));
	};
}_TBL_SERV_STR_;

inline ostream& operator<< (ostream& os, const _TBL_SERV_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_SERV_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_SERV_CLASS{
	friend class MEM_SERV_CLASS;
	private:
		_TBL_SERV_STR_		item;
		_TBL_SERV_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_SERV_CLASS();
		~TBL_SERV_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_SERV_ID(void);
		long	Get_AGREEMENT_ID(void);
		long	Get_CUST_ID(void);
		long	Get_PRODUCT_ID(void);
		char*	Get_CREATE_DATE(void);
		char*	Get_STATE(void);
		char*	Get_STATE_DATE(void);
		char*	Get_RENT_DATE(void);
		char*	Get_NSC_DATE(void);
		long	Get_CYCLE_TYPE_ID(void);
		long	Get_BILLING_MODE_ID(void);
		char*	Get_ACC_NBR(void);
		long	Get_FREE_TYPE_ID(void);
		void	Set_SERV_ID(long	lParam);
		void	Set_AGREEMENT_ID(long	lParam);
		void	Set_CUST_ID(long	lParam);
		void	Set_PRODUCT_ID(long	lParam);
		void	Set_CREATE_DATE(char	* sParam);
		void	Set_STATE(char	* sParam);
		void	Set_STATE_DATE(char	* sParam);
		void	Set_RENT_DATE(char	* sParam);
		void	Set_NSC_DATE(char	* sParam);
		void	Set_CYCLE_TYPE_ID(long	lParam);
		void	Set_BILLING_MODE_ID(long	lParam);
		void	Set_ACC_NBR(char	* sParam);
		void	Set_FREE_TYPE_ID(long	lParam);
		void	AddItem(void);
		void	ClearBuf(void);
		void	SetBuf(int iBufLen=ARRAY_BUFLEN);
		void	SaveFlush(void);
		void	FreeBuf(void);
		void	Print(ostream& os) const {
				os<<item;
		};
};

inline ostream& operator<< (ostream& os, const TBL_SERV_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_SERV_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_SERV_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_SERV_STR_>		a_item;
		static	hash_map<const long ,_TBL_SERV_STR_* , hash<long>, MEM_SERV_CLASS::eqlong>		hashmap;
	public:
		MEM_SERV_CLASS(char * sTableName=NULL);
		~MEM_SERV_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_SERV_STR_ *	operator [](long opt);
};

#endif
