/*VER: 1*/ 
#ifndef _TBL_VALUE_ADDED_EVENT_CLASS_H__
#define _TBL_VALUE_ADDED_EVENT_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
using namespace std;
typedef struct _TBL_VALUE_ADDED_EVENT_STR_{
	long	BILLING_ORG_ID;
	char	BILLING_VISIT_AREA_CODE[11];
	char	CALLING_AREA_CODE[11];
	char	CALLING_NBR[33];
	char	ORG_CALLING_NBR[33];
	long	CALLING_SP_TYPE_ID;
	long	CALLING_SERVICE_TYPE_ID;
	char	CALLED_AREA_CODE[11];
	char	CALLED_NBR[33];
	char	ORG_CALLED_NBR[33];
	long	CALLED_SP_TYPE_ID;
	long	CALLED_SERVICE_TYPE_ID;
	char	THIRD_PARTY_AREA_CODE[11];
	char	THIRD_PARTY_NBR[33];
	char	START_DATE[15];
	long	DURATION;
	char	SWITCH_ID[11];
	long	FILE_ID;
	char	CREATED_DATE[15];
	char	EVENT_STATE[4];
	char	STATE_DATE[15];
	char	CARD_NO[33];
	char	BILLING_TYPE[2];
	long	PARTER_ID;
	long	SERV_ID;
	long	OFFER_ID1;
	long	OFFER_ID2;
	long	OFFER_ID3;
	long	OFFER_ID4;
	long	OFFER_ID5;
	long	ORG_CHARGE1;
	long	STD_CHARGE1;
	long	CHARGE1;
	long	ACCT_ITEM_TYPE_ID1;
	long	DISCOUNT_ACCT_ITEM_TYPE_ID1;
	long	BILL_MEASURE1;
	long	ORG_CHARGE2;
	long	STD_CHARGE2;
	long	CHARGE2;
	long	ACCT_ITEM_TYPE_ID2;
	long	BILL_MEASURE2;
	long	DISCOUNT_ACCT_ITEM_TYPE_ID2;
	long	PRICING_COMBINE_ID;
	long	EVENT_ID;
	long	BILL_FLOW_ID;
	long	BILLING_CYCLE_ID;
	long	EVENT_TYPE_ID;
	char	BILLING_AREA_CODE[11];
	char	BILLING_NBR[33];
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "BILLING_ORG_ID:["<<BILLING_ORG_ID<<"]"<<endl;
			os << "BILLING_VISIT_AREA_CODE:["<<BILLING_VISIT_AREA_CODE<<"]"<<endl;
			os << "CALLING_AREA_CODE:["<<CALLING_AREA_CODE<<"]"<<endl;
			os << "CALLING_NBR:["<<CALLING_NBR<<"]"<<endl;
			os << "ORG_CALLING_NBR:["<<ORG_CALLING_NBR<<"]"<<endl;
			os << "CALLING_SP_TYPE_ID:["<<CALLING_SP_TYPE_ID<<"]"<<endl;
			os << "CALLING_SERVICE_TYPE_ID:["<<CALLING_SERVICE_TYPE_ID<<"]"<<endl;
			os << "CALLED_AREA_CODE:["<<CALLED_AREA_CODE<<"]"<<endl;
			os << "CALLED_NBR:["<<CALLED_NBR<<"]"<<endl;
			os << "ORG_CALLED_NBR:["<<ORG_CALLED_NBR<<"]"<<endl;
			os << "CALLED_SP_TYPE_ID:["<<CALLED_SP_TYPE_ID<<"]"<<endl;
			os << "CALLED_SERVICE_TYPE_ID:["<<CALLED_SERVICE_TYPE_ID<<"]"<<endl;
			os << "THIRD_PARTY_AREA_CODE:["<<THIRD_PARTY_AREA_CODE<<"]"<<endl;
			os << "THIRD_PARTY_NBR:["<<THIRD_PARTY_NBR<<"]"<<endl;
			os << "START_DATE:["<<START_DATE<<"]"<<endl;
			os << "DURATION:["<<DURATION<<"]"<<endl;
			os << "SWITCH_ID:["<<SWITCH_ID<<"]"<<endl;
			os << "FILE_ID:["<<FILE_ID<<"]"<<endl;
			os << "CREATED_DATE:["<<CREATED_DATE<<"]"<<endl;
			os << "EVENT_STATE:["<<EVENT_STATE<<"]"<<endl;
			os << "STATE_DATE:["<<STATE_DATE<<"]"<<endl;
			os << "CARD_NO:["<<CARD_NO<<"]"<<endl;
			os << "BILLING_TYPE:["<<BILLING_TYPE<<"]"<<endl;
			os << "PARTER_ID:["<<PARTER_ID<<"]"<<endl;
			os << "SERV_ID:["<<SERV_ID<<"]"<<endl;
			os << "OFFER_ID1:["<<OFFER_ID1<<"]"<<endl;
			os << "OFFER_ID2:["<<OFFER_ID2<<"]"<<endl;
			os << "OFFER_ID3:["<<OFFER_ID3<<"]"<<endl;
			os << "OFFER_ID4:["<<OFFER_ID4<<"]"<<endl;
			os << "OFFER_ID5:["<<OFFER_ID5<<"]"<<endl;
			os << "ORG_CHARGE1:["<<ORG_CHARGE1<<"]"<<endl;
			os << "STD_CHARGE1:["<<STD_CHARGE1<<"]"<<endl;
			os << "CHARGE1:["<<CHARGE1<<"]"<<endl;
			os << "ACCT_ITEM_TYPE_ID1:["<<ACCT_ITEM_TYPE_ID1<<"]"<<endl;
			os << "DISCOUNT_ACCT_ITEM_TYPE_ID1:["<<DISCOUNT_ACCT_ITEM_TYPE_ID1<<"]"<<endl;
			os << "BILL_MEASURE1:["<<BILL_MEASURE1<<"]"<<endl;
			os << "ORG_CHARGE2:["<<ORG_CHARGE2<<"]"<<endl;
			os << "STD_CHARGE2:["<<STD_CHARGE2<<"]"<<endl;
			os << "CHARGE2:["<<CHARGE2<<"]"<<endl;
			os << "ACCT_ITEM_TYPE_ID2:["<<ACCT_ITEM_TYPE_ID2<<"]"<<endl;
			os << "BILL_MEASURE2:["<<BILL_MEASURE2<<"]"<<endl;
			os << "DISCOUNT_ACCT_ITEM_TYPE_ID2:["<<DISCOUNT_ACCT_ITEM_TYPE_ID2<<"]"<<endl;
			os << "PRICING_COMBINE_ID:["<<PRICING_COMBINE_ID<<"]"<<endl;
			os << "EVENT_ID:["<<EVENT_ID<<"]"<<endl;
			os << "BILL_FLOW_ID:["<<BILL_FLOW_ID<<"]"<<endl;
			os << "BILLING_CYCLE_ID:["<<BILLING_CYCLE_ID<<"]"<<endl;
			os << "EVENT_TYPE_ID:["<<EVENT_TYPE_ID<<"]"<<endl;
			os << "BILLING_AREA_CODE:["<<BILLING_AREA_CODE<<"]"<<endl;
			os << "BILLING_NBR:["<<BILLING_NBR<<"]"<<endl;
	};
	_TBL_VALUE_ADDED_EVENT_STR_(){
			BILLING_ORG_ID=0;
			BILLING_VISIT_AREA_CODE[0]='\0';
			CALLING_AREA_CODE[0]='\0';
			CALLING_NBR[0]='\0';
			ORG_CALLING_NBR[0]='\0';
			CALLING_SP_TYPE_ID=0;
			CALLING_SERVICE_TYPE_ID=0;
			CALLED_AREA_CODE[0]='\0';
			CALLED_NBR[0]='\0';
			ORG_CALLED_NBR[0]='\0';
			CALLED_SP_TYPE_ID=0;
			CALLED_SERVICE_TYPE_ID=0;
			THIRD_PARTY_AREA_CODE[0]='\0';
			THIRD_PARTY_NBR[0]='\0';
			START_DATE[0]='\0';
			DURATION=0;
			SWITCH_ID[0]='\0';
			FILE_ID=0;
			CREATED_DATE[0]='\0';
			EVENT_STATE[0]='\0';
			STATE_DATE[0]='\0';
			CARD_NO[0]='\0';
			BILLING_TYPE[0]='\0';
			PARTER_ID=0;
			SERV_ID=0;
			OFFER_ID1=0;
			OFFER_ID2=0;
			OFFER_ID3=0;
			OFFER_ID4=0;
			OFFER_ID5=0;
			ORG_CHARGE1=0;
			STD_CHARGE1=0;
			CHARGE1=0;
			ACCT_ITEM_TYPE_ID1=0;
			DISCOUNT_ACCT_ITEM_TYPE_ID1=0;
			BILL_MEASURE1=0;
			ORG_CHARGE2=0;
			STD_CHARGE2=0;
			CHARGE2=0;
			ACCT_ITEM_TYPE_ID2=0;
			BILL_MEASURE2=0;
			DISCOUNT_ACCT_ITEM_TYPE_ID2=0;
			PRICING_COMBINE_ID=0;
			EVENT_ID=0;
			BILL_FLOW_ID=0;
			BILLING_CYCLE_ID=0;
			EVENT_TYPE_ID=0;
			BILLING_AREA_CODE[0]='\0';
			BILLING_NBR[0]='\0';
	};
	void	ClearBuf(void){
			memset(&BILLING_ORG_ID,0,sizeof(_TBL_VALUE_ADDED_EVENT_STR_));
	};
}_TBL_VALUE_ADDED_EVENT_STR_;

inline ostream& operator<< (ostream& os, const _TBL_VALUE_ADDED_EVENT_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_VALUE_ADDED_EVENT_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_VALUE_ADDED_EVENT_CLASS{
	friend class MEM_VALUE_ADDED_EVENT_CLASS;
	private:
		_TBL_VALUE_ADDED_EVENT_STR_		item;
		_TBL_VALUE_ADDED_EVENT_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_VALUE_ADDED_EVENT_CLASS();
		~TBL_VALUE_ADDED_EVENT_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_BILLING_ORG_ID(void);
		char*	Get_BILLING_VISIT_AREA_CODE(void);
		char*	Get_CALLING_AREA_CODE(void);
		char*	Get_CALLING_NBR(void);
		char*	Get_ORG_CALLING_NBR(void);
		long	Get_CALLING_SP_TYPE_ID(void);
		long	Get_CALLING_SERVICE_TYPE_ID(void);
		char*	Get_CALLED_AREA_CODE(void);
		char*	Get_CALLED_NBR(void);
		char*	Get_ORG_CALLED_NBR(void);
		long	Get_CALLED_SP_TYPE_ID(void);
		long	Get_CALLED_SERVICE_TYPE_ID(void);
		char*	Get_THIRD_PARTY_AREA_CODE(void);
		char*	Get_THIRD_PARTY_NBR(void);
		char*	Get_START_DATE(void);
		long	Get_DURATION(void);
		char*	Get_SWITCH_ID(void);
		long	Get_FILE_ID(void);
		char*	Get_CREATED_DATE(void);
		char*	Get_EVENT_STATE(void);
		char*	Get_STATE_DATE(void);
		char*	Get_CARD_NO(void);
		char*	Get_BILLING_TYPE(void);
		long	Get_PARTER_ID(void);
		long	Get_SERV_ID(void);
		long	Get_OFFER_ID1(void);
		long	Get_OFFER_ID2(void);
		long	Get_OFFER_ID3(void);
		long	Get_OFFER_ID4(void);
		long	Get_OFFER_ID5(void);
		long	Get_ORG_CHARGE1(void);
		long	Get_STD_CHARGE1(void);
		long	Get_CHARGE1(void);
		long	Get_ACCT_ITEM_TYPE_ID1(void);
		long	Get_DISCOUNT_ACCT_ITEM_TYPE_ID1(void);
		long	Get_BILL_MEASURE1(void);
		long	Get_ORG_CHARGE2(void);
		long	Get_STD_CHARGE2(void);
		long	Get_CHARGE2(void);
		long	Get_ACCT_ITEM_TYPE_ID2(void);
		long	Get_BILL_MEASURE2(void);
		long	Get_DISCOUNT_ACCT_ITEM_TYPE_ID2(void);
		long	Get_PRICING_COMBINE_ID(void);
		long	Get_EVENT_ID(void);
		long	Get_BILL_FLOW_ID(void);
		long	Get_BILLING_CYCLE_ID(void);
		long	Get_EVENT_TYPE_ID(void);
		char*	Get_BILLING_AREA_CODE(void);
		char*	Get_BILLING_NBR(void);
		void	Set_BILLING_ORG_ID(long	lParam);
		void	Set_BILLING_VISIT_AREA_CODE(char	* sParam);
		void	Set_CALLING_AREA_CODE(char	* sParam);
		void	Set_CALLING_NBR(char	* sParam);
		void	Set_ORG_CALLING_NBR(char	* sParam);
		void	Set_CALLING_SP_TYPE_ID(long	lParam);
		void	Set_CALLING_SERVICE_TYPE_ID(long	lParam);
		void	Set_CALLED_AREA_CODE(char	* sParam);
		void	Set_CALLED_NBR(char	* sParam);
		void	Set_ORG_CALLED_NBR(char	* sParam);
		void	Set_CALLED_SP_TYPE_ID(long	lParam);
		void	Set_CALLED_SERVICE_TYPE_ID(long	lParam);
		void	Set_THIRD_PARTY_AREA_CODE(char	* sParam);
		void	Set_THIRD_PARTY_NBR(char	* sParam);
		void	Set_START_DATE(char	* sParam);
		void	Set_DURATION(long	lParam);
		void	Set_SWITCH_ID(char	* sParam);
		void	Set_FILE_ID(long	lParam);
		void	Set_CREATED_DATE(char	* sParam);
		void	Set_EVENT_STATE(char	* sParam);
		void	Set_STATE_DATE(char	* sParam);
		void	Set_CARD_NO(char	* sParam);
		void	Set_BILLING_TYPE(char	* sParam);
		void	Set_PARTER_ID(long	lParam);
		void	Set_SERV_ID(long	lParam);
		void	Set_OFFER_ID1(long	lParam);
		void	Set_OFFER_ID2(long	lParam);
		void	Set_OFFER_ID3(long	lParam);
		void	Set_OFFER_ID4(long	lParam);
		void	Set_OFFER_ID5(long	lParam);
		void	Set_ORG_CHARGE1(long	lParam);
		void	Set_STD_CHARGE1(long	lParam);
		void	Set_CHARGE1(long	lParam);
		void	Set_ACCT_ITEM_TYPE_ID1(long	lParam);
		void	Set_DISCOUNT_ACCT_ITEM_TYPE_ID1(long	lParam);
		void	Set_BILL_MEASURE1(long	lParam);
		void	Set_ORG_CHARGE2(long	lParam);
		void	Set_STD_CHARGE2(long	lParam);
		void	Set_CHARGE2(long	lParam);
		void	Set_ACCT_ITEM_TYPE_ID2(long	lParam);
		void	Set_BILL_MEASURE2(long	lParam);
		void	Set_DISCOUNT_ACCT_ITEM_TYPE_ID2(long	lParam);
		void	Set_PRICING_COMBINE_ID(long	lParam);
		void	Set_EVENT_ID(long	lParam);
		void	Set_BILL_FLOW_ID(long	lParam);
		void	Set_BILLING_CYCLE_ID(long	lParam);
		void	Set_EVENT_TYPE_ID(long	lParam);
		void	Set_BILLING_AREA_CODE(char	* sParam);
		void	Set_BILLING_NBR(char	* sParam);
		void	AddItem(void);
		void	ClearBuf(void);
		void	SetBuf(int iBufLen=ARRAY_BUFLEN);
		void	SaveFlush(void);
		void	FreeBuf(void);
		void	Print(ostream& os) const {
				os<<item;
		};
};

inline ostream& operator<< (ostream& os, const TBL_VALUE_ADDED_EVENT_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_VALUE_ADDED_EVENT_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_VALUE_ADDED_EVENT_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_VALUE_ADDED_EVENT_STR_>		a_item;
		static	hash_map<const long ,_TBL_VALUE_ADDED_EVENT_STR_* , hash<long>, MEM_VALUE_ADDED_EVENT_CLASS::eqlong>		hashmap;
		static	bool	bLoadFlag;
	public:
		MEM_VALUE_ADDED_EVENT_CLASS(char * sTableName=NULL);
		~MEM_VALUE_ADDED_EVENT_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_VALUE_ADDED_EVENT_STR_ *	operator [](long opt);
};

#endif
