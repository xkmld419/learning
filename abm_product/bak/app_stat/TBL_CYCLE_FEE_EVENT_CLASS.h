/*VER: 1*/ 
#ifndef _TBL_CYCLE_FEE_EVENT_CLASS_H__
#define _TBL_CYCLE_FEE_EVENT_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
using namespace std;
typedef struct _TBL_CYCLE_FEE_EVENT_STR_{
	long	EVENT_ID;
	long	BILL_FLOW_ID;
	long	BILLING_CYCLE_ID;
	long	EVENT_TYPE_ID;
	long	BILLING_ORG_ID;
	long	FILE_ID;
	char	CREATED_DATE[15];
	char	EVENT_STATE[4];
	char	STATE_DATE[15];
	long	PARTER_ID;
	long	SERV_ID;
	long	SERV_PRODUCT_ID;
	char	CYCLE_BEGIN_DATE[15];
	char	CYCLE_END_DATE[15];
	long	SERV_PROD_ATTR_USED_NUM;
	long	ATTR_ID1;
	char	ATTR_VALUE1[31];
	char	ATTR_EFF_DATE1[15];
	char	ATTR_EXP_DATE1[15];
	long	ATTR_ID2;
	char	ATTR_VALUE2[31];
	char	ATTR_EFF_DATE2[15];
	char	ATTR_EXP_DATE2[15];
	long	OFFER_ID1;
	long	OFFER_ID2;
	long	OFFER_ID3;
	long	OFFER_ID4;
	long	OFFER_ID5;
	long	STD_CHARGE1;
	long	CHARGE1;
	long	ACCT_ITEM_TYPE_ID1;
	long	DISCOUNT_ACCT_ITEM_TYPE_ID1;
	long	BILL_MEASURE1;
	char	BILLING_RECORD[101];
	long	PRICING_COMBINE_ID;
	char	START_DATE[15];
	char	END_DATE[15];
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "EVENT_ID:["<<EVENT_ID<<"]"<<endl;
			os << "BILL_FLOW_ID:["<<BILL_FLOW_ID<<"]"<<endl;
			os << "BILLING_CYCLE_ID:["<<BILLING_CYCLE_ID<<"]"<<endl;
			os << "EVENT_TYPE_ID:["<<EVENT_TYPE_ID<<"]"<<endl;
			os << "BILLING_ORG_ID:["<<BILLING_ORG_ID<<"]"<<endl;
			os << "FILE_ID:["<<FILE_ID<<"]"<<endl;
			os << "CREATED_DATE:["<<CREATED_DATE<<"]"<<endl;
			os << "EVENT_STATE:["<<EVENT_STATE<<"]"<<endl;
			os << "STATE_DATE:["<<STATE_DATE<<"]"<<endl;
			os << "PARTER_ID:["<<PARTER_ID<<"]"<<endl;
			os << "SERV_ID:["<<SERV_ID<<"]"<<endl;
			os << "SERV_PRODUCT_ID:["<<SERV_PRODUCT_ID<<"]"<<endl;
			os << "CYCLE_BEGIN_DATE:["<<CYCLE_BEGIN_DATE<<"]"<<endl;
			os << "CYCLE_END_DATE:["<<CYCLE_END_DATE<<"]"<<endl;
			os << "SERV_PROD_ATTR_USED_NUM:["<<SERV_PROD_ATTR_USED_NUM<<"]"<<endl;
			os << "ATTR_ID1:["<<ATTR_ID1<<"]"<<endl;
			os << "ATTR_VALUE1:["<<ATTR_VALUE1<<"]"<<endl;
			os << "ATTR_EFF_DATE1:["<<ATTR_EFF_DATE1<<"]"<<endl;
			os << "ATTR_EXP_DATE1:["<<ATTR_EXP_DATE1<<"]"<<endl;
			os << "ATTR_ID2:["<<ATTR_ID2<<"]"<<endl;
			os << "ATTR_VALUE2:["<<ATTR_VALUE2<<"]"<<endl;
			os << "ATTR_EFF_DATE2:["<<ATTR_EFF_DATE2<<"]"<<endl;
			os << "ATTR_EXP_DATE2:["<<ATTR_EXP_DATE2<<"]"<<endl;
			os << "OFFER_ID1:["<<OFFER_ID1<<"]"<<endl;
			os << "OFFER_ID2:["<<OFFER_ID2<<"]"<<endl;
			os << "OFFER_ID3:["<<OFFER_ID3<<"]"<<endl;
			os << "OFFER_ID4:["<<OFFER_ID4<<"]"<<endl;
			os << "OFFER_ID5:["<<OFFER_ID5<<"]"<<endl;
			os << "STD_CHARGE1:["<<STD_CHARGE1<<"]"<<endl;
			os << "CHARGE1:["<<CHARGE1<<"]"<<endl;
			os << "ACCT_ITEM_TYPE_ID1:["<<ACCT_ITEM_TYPE_ID1<<"]"<<endl;
			os << "DISCOUNT_ACCT_ITEM_TYPE_ID1:["<<DISCOUNT_ACCT_ITEM_TYPE_ID1<<"]"<<endl;
			os << "BILL_MEASURE1:["<<BILL_MEASURE1<<"]"<<endl;
			os << "BILLING_RECORD:["<<BILLING_RECORD<<"]"<<endl;
			os << "PRICING_COMBINE_ID:["<<PRICING_COMBINE_ID<<"]"<<endl;
			os << "START_DATE:["<<START_DATE<<"]"<<endl;
			os << "END_DATE:["<<END_DATE<<"]"<<endl;
	};
	_TBL_CYCLE_FEE_EVENT_STR_(){
			EVENT_ID=0;
			BILL_FLOW_ID=0;
			BILLING_CYCLE_ID=0;
			EVENT_TYPE_ID=0;
			BILLING_ORG_ID=0;
			FILE_ID=0;
			CREATED_DATE[0]='\0';
			EVENT_STATE[0]='\0';
			STATE_DATE[0]='\0';
			PARTER_ID=0;
			SERV_ID=0;
			SERV_PRODUCT_ID=0;
			CYCLE_BEGIN_DATE[0]='\0';
			CYCLE_END_DATE[0]='\0';
			SERV_PROD_ATTR_USED_NUM=0;
			ATTR_ID1=0;
			ATTR_VALUE1[0]='\0';
			ATTR_EFF_DATE1[0]='\0';
			ATTR_EXP_DATE1[0]='\0';
			ATTR_ID2=0;
			ATTR_VALUE2[0]='\0';
			ATTR_EFF_DATE2[0]='\0';
			ATTR_EXP_DATE2[0]='\0';
			OFFER_ID1=0;
			OFFER_ID2=0;
			OFFER_ID3=0;
			OFFER_ID4=0;
			OFFER_ID5=0;
			STD_CHARGE1=0;
			CHARGE1=0;
			ACCT_ITEM_TYPE_ID1=0;
			DISCOUNT_ACCT_ITEM_TYPE_ID1=0;
			BILL_MEASURE1=0;
			BILLING_RECORD[0]='\0';
			PRICING_COMBINE_ID=0;
			START_DATE[0]='\0';
			END_DATE[0]='\0';
	};
	void	ClearBuf(void){
			memset(&EVENT_ID,0,sizeof(_TBL_CYCLE_FEE_EVENT_STR_));
	};
}_TBL_CYCLE_FEE_EVENT_STR_;

inline ostream& operator<< (ostream& os, const _TBL_CYCLE_FEE_EVENT_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_CYCLE_FEE_EVENT_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_CYCLE_FEE_EVENT_CLASS{
	friend class MEM_CYCLE_FEE_EVENT_CLASS;
	private:
		_TBL_CYCLE_FEE_EVENT_STR_		item;
		_TBL_CYCLE_FEE_EVENT_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_CYCLE_FEE_EVENT_CLASS();
		~TBL_CYCLE_FEE_EVENT_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_EVENT_ID(void);
		long	Get_BILL_FLOW_ID(void);
		long	Get_BILLING_CYCLE_ID(void);
		long	Get_EVENT_TYPE_ID(void);
		long	Get_BILLING_ORG_ID(void);
		long	Get_FILE_ID(void);
		char*	Get_CREATED_DATE(void);
		char*	Get_EVENT_STATE(void);
		char*	Get_STATE_DATE(void);
		long	Get_PARTER_ID(void);
		long	Get_SERV_ID(void);
		long	Get_SERV_PRODUCT_ID(void);
		char*	Get_CYCLE_BEGIN_DATE(void);
		char*	Get_CYCLE_END_DATE(void);
		long	Get_SERV_PROD_ATTR_USED_NUM(void);
		long	Get_ATTR_ID1(void);
		char*	Get_ATTR_VALUE1(void);
		char*	Get_ATTR_EFF_DATE1(void);
		char*	Get_ATTR_EXP_DATE1(void);
		long	Get_ATTR_ID2(void);
		char*	Get_ATTR_VALUE2(void);
		char*	Get_ATTR_EFF_DATE2(void);
		char*	Get_ATTR_EXP_DATE2(void);
		long	Get_OFFER_ID1(void);
		long	Get_OFFER_ID2(void);
		long	Get_OFFER_ID3(void);
		long	Get_OFFER_ID4(void);
		long	Get_OFFER_ID5(void);
		long	Get_STD_CHARGE1(void);
		long	Get_CHARGE1(void);
		long	Get_ACCT_ITEM_TYPE_ID1(void);
		long	Get_DISCOUNT_ACCT_ITEM_TYPE_ID1(void);
		long	Get_BILL_MEASURE1(void);
		char*	Get_BILLING_RECORD(void);
		long	Get_PRICING_COMBINE_ID(void);
		char*	Get_START_DATE(void);
		char*	Get_END_DATE(void);
		void	Set_EVENT_ID(long	lParam);
		void	Set_BILL_FLOW_ID(long	lParam);
		void	Set_BILLING_CYCLE_ID(long	lParam);
		void	Set_EVENT_TYPE_ID(long	lParam);
		void	Set_BILLING_ORG_ID(long	lParam);
		void	Set_FILE_ID(long	lParam);
		void	Set_CREATED_DATE(char	* sParam);
		void	Set_EVENT_STATE(char	* sParam);
		void	Set_STATE_DATE(char	* sParam);
		void	Set_PARTER_ID(long	lParam);
		void	Set_SERV_ID(long	lParam);
		void	Set_SERV_PRODUCT_ID(long	lParam);
		void	Set_CYCLE_BEGIN_DATE(char	* sParam);
		void	Set_CYCLE_END_DATE(char	* sParam);
		void	Set_SERV_PROD_ATTR_USED_NUM(long	lParam);
		void	Set_ATTR_ID1(long	lParam);
		void	Set_ATTR_VALUE1(char	* sParam);
		void	Set_ATTR_EFF_DATE1(char	* sParam);
		void	Set_ATTR_EXP_DATE1(char	* sParam);
		void	Set_ATTR_ID2(long	lParam);
		void	Set_ATTR_VALUE2(char	* sParam);
		void	Set_ATTR_EFF_DATE2(char	* sParam);
		void	Set_ATTR_EXP_DATE2(char	* sParam);
		void	Set_OFFER_ID1(long	lParam);
		void	Set_OFFER_ID2(long	lParam);
		void	Set_OFFER_ID3(long	lParam);
		void	Set_OFFER_ID4(long	lParam);
		void	Set_OFFER_ID5(long	lParam);
		void	Set_STD_CHARGE1(long	lParam);
		void	Set_CHARGE1(long	lParam);
		void	Set_ACCT_ITEM_TYPE_ID1(long	lParam);
		void	Set_DISCOUNT_ACCT_ITEM_TYPE_ID1(long	lParam);
		void	Set_BILL_MEASURE1(long	lParam);
		void	Set_BILLING_RECORD(char	* sParam);
		void	Set_PRICING_COMBINE_ID(long	lParam);
		void	Set_START_DATE(char	* sParam);
		void	Set_END_DATE(char	* sParam);
		void	AddItem(void);
		void	ClearBuf(void);
		void	SetBuf(int iBufLen=ARRAY_BUFLEN);
		void	SaveFlush(void);
		void	FreeBuf(void);
		void	Print(ostream& os) const {
				os<<item;
		};
};

inline ostream& operator<< (ostream& os, const TBL_CYCLE_FEE_EVENT_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_CYCLE_FEE_EVENT_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_CYCLE_FEE_EVENT_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_CYCLE_FEE_EVENT_STR_>		a_item;
		static	hash_map<const long ,_TBL_CYCLE_FEE_EVENT_STR_* , hash<long>, MEM_CYCLE_FEE_EVENT_CLASS::eqlong>		hashmap;
	public:
		MEM_CYCLE_FEE_EVENT_CLASS(char * sTableName=NULL);
		~MEM_CYCLE_FEE_EVENT_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_CYCLE_FEE_EVENT_STR_ *	operator [](long opt);
};

#endif
