/*VER: 1*/ 
#ifndef _TBL_ERROR_EVENT_CLASS_H__
#define _TBL_ERROR_EVENT_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
using namespace std;
typedef struct _TBL_ERROR_EVENT_STR_{
	long	EVENT_ID;
	long	BILL_FLOW_ID;
	long	BILLING_ORG_ID;
	char	ORG_CALLING_NBR[33];
	char	ORG_CALLED_NBR[33];
	char	THIRD_PARTY_AREA_CODE[11];
	char	START_DATE[15];
	long	DURATION;
	char	TRUNK_IN[13];
	char	TRUNK_OUT[13];
	char	ACCOUNT_AREA_CODE[11];
	char	ACCOUNT_NBR[31];
	char	NAS_IP[21];
	long	NAS_PORT_ID;
	long	RECV_BYTES;
	long	SEND_BYTES;
	long	RECV_PACKETS;
	long	SEND_PACKETS;
	long	SWITCH_ID;
	long	FILE_ID;
	char	CREATED_DATE[15];
	long	EVENT_STATE;
	char	STATE_DATE[15];
	long	ERROR_ID;
	long	EXTEND_USED_NUM;
	long	EXTEND_FIELD_ID1;
	char	EXTEND_VALUE1[101];
	long	EXTEND_FIELD_ID2;
	char	EXTEND_VALUE2[101];
	long	EXTEND_FIELD_ID3;
	char	EXTEND_VALUE3[101];
	long	EXTEND_FIELD_ID4;
	char	EXTEND_VALUE4[101];
	long	EXTEND_FIELD_ID5;
	char	EXTEND_VALUE5[101];
	long	EXTEND_FIELD_ID6;
	char	EXTEND_VALUE6[101];
	long	EXTEND_FIELD_ID7;
	char	EXTEND_VALUE7[101];
	long	EXTEND_FIELD_ID8;
	char	EXTEND_VALUE8[101];
	long	EXTEND_FIELD_ID9;
	char	EXTEND_VALUE9[101];
	long	EXTEND_FIELD_ID10;
	char	EXTEND_VALUE10[101];
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "EVENT_ID:["<<EVENT_ID<<"]"<<endl;
			os << "BILL_FLOW_ID:["<<BILL_FLOW_ID<<"]"<<endl;
			os << "BILLING_ORG_ID:["<<BILLING_ORG_ID<<"]"<<endl;
			os << "ORG_CALLING_NBR:["<<ORG_CALLING_NBR<<"]"<<endl;
			os << "ORG_CALLED_NBR:["<<ORG_CALLED_NBR<<"]"<<endl;
			os << "THIRD_PARTY_AREA_CODE:["<<THIRD_PARTY_AREA_CODE<<"]"<<endl;
			os << "START_DATE:["<<START_DATE<<"]"<<endl;
			os << "DURATION:["<<DURATION<<"]"<<endl;
			os << "TRUNK_IN:["<<TRUNK_IN<<"]"<<endl;
			os << "TRUNK_OUT:["<<TRUNK_OUT<<"]"<<endl;
			os << "ACCOUNT_AREA_CODE:["<<ACCOUNT_AREA_CODE<<"]"<<endl;
			os << "ACCOUNT_NBR:["<<ACCOUNT_NBR<<"]"<<endl;
			os << "NAS_IP:["<<NAS_IP<<"]"<<endl;
			os << "NAS_PORT_ID:["<<NAS_PORT_ID<<"]"<<endl;
			os << "RECV_BYTES:["<<RECV_BYTES<<"]"<<endl;
			os << "SEND_BYTES:["<<SEND_BYTES<<"]"<<endl;
			os << "RECV_PACKETS:["<<RECV_PACKETS<<"]"<<endl;
			os << "SEND_PACKETS:["<<SEND_PACKETS<<"]"<<endl;
			os << "SWITCH_ID:["<<SWITCH_ID<<"]"<<endl;
			os << "FILE_ID:["<<FILE_ID<<"]"<<endl;
			os << "CREATED_DATE:["<<CREATED_DATE<<"]"<<endl;
			os << "EVENT_STATE:["<<EVENT_STATE<<"]"<<endl;
			os << "STATE_DATE:["<<STATE_DATE<<"]"<<endl;
			os << "ERROR_ID:["<<ERROR_ID<<"]"<<endl;
			os << "EXTEND_USED_NUM:["<<EXTEND_USED_NUM<<"]"<<endl;
			os << "EXTEND_FIELD_ID1:["<<EXTEND_FIELD_ID1<<"]"<<endl;
			os << "EXTEND_VALUE1:["<<EXTEND_VALUE1<<"]"<<endl;
			os << "EXTEND_FIELD_ID2:["<<EXTEND_FIELD_ID2<<"]"<<endl;
			os << "EXTEND_VALUE2:["<<EXTEND_VALUE2<<"]"<<endl;
			os << "EXTEND_FIELD_ID3:["<<EXTEND_FIELD_ID3<<"]"<<endl;
			os << "EXTEND_VALUE3:["<<EXTEND_VALUE3<<"]"<<endl;
			os << "EXTEND_FIELD_ID4:["<<EXTEND_FIELD_ID4<<"]"<<endl;
			os << "EXTEND_VALUE4:["<<EXTEND_VALUE4<<"]"<<endl;
			os << "EXTEND_FIELD_ID5:["<<EXTEND_FIELD_ID5<<"]"<<endl;
			os << "EXTEND_VALUE5:["<<EXTEND_VALUE5<<"]"<<endl;
			os << "EXTEND_FIELD_ID6:["<<EXTEND_FIELD_ID6<<"]"<<endl;
			os << "EXTEND_VALUE6:["<<EXTEND_VALUE6<<"]"<<endl;
			os << "EXTEND_FIELD_ID7:["<<EXTEND_FIELD_ID7<<"]"<<endl;
			os << "EXTEND_VALUE7:["<<EXTEND_VALUE7<<"]"<<endl;
			os << "EXTEND_FIELD_ID8:["<<EXTEND_FIELD_ID8<<"]"<<endl;
			os << "EXTEND_VALUE8:["<<EXTEND_VALUE8<<"]"<<endl;
			os << "EXTEND_FIELD_ID9:["<<EXTEND_FIELD_ID9<<"]"<<endl;
			os << "EXTEND_VALUE9:["<<EXTEND_VALUE9<<"]"<<endl;
			os << "EXTEND_FIELD_ID10:["<<EXTEND_FIELD_ID10<<"]"<<endl;
			os << "EXTEND_VALUE10:["<<EXTEND_VALUE10<<"]"<<endl;
	};
	_TBL_ERROR_EVENT_STR_(){
			EVENT_ID=0;
			BILL_FLOW_ID=0;
			BILLING_ORG_ID=0;
			ORG_CALLING_NBR[0]='\0';
			ORG_CALLED_NBR[0]='\0';
			THIRD_PARTY_AREA_CODE[0]='\0';
			START_DATE[0]='\0';
			DURATION=0;
			TRUNK_IN[0]='\0';
			TRUNK_OUT[0]='\0';
			ACCOUNT_AREA_CODE[0]='\0';
			ACCOUNT_NBR[0]='\0';
			NAS_IP[0]='\0';
			NAS_PORT_ID=0;
			RECV_BYTES=0;
			SEND_BYTES=0;
			RECV_PACKETS=0;
			SEND_PACKETS=0;
			SWITCH_ID=0;
			FILE_ID=0;
			CREATED_DATE[0]='\0';
			EVENT_STATE=0;
			STATE_DATE[0]='\0';
			ERROR_ID=0;
			EXTEND_USED_NUM=0;
			EXTEND_FIELD_ID1=0;
			EXTEND_VALUE1[0]='\0';
			EXTEND_FIELD_ID2=0;
			EXTEND_VALUE2[0]='\0';
			EXTEND_FIELD_ID3=0;
			EXTEND_VALUE3[0]='\0';
			EXTEND_FIELD_ID4=0;
			EXTEND_VALUE4[0]='\0';
			EXTEND_FIELD_ID5=0;
			EXTEND_VALUE5[0]='\0';
			EXTEND_FIELD_ID6=0;
			EXTEND_VALUE6[0]='\0';
			EXTEND_FIELD_ID7=0;
			EXTEND_VALUE7[0]='\0';
			EXTEND_FIELD_ID8=0;
			EXTEND_VALUE8[0]='\0';
			EXTEND_FIELD_ID9=0;
			EXTEND_VALUE9[0]='\0';
			EXTEND_FIELD_ID10=0;
			EXTEND_VALUE10[0]='\0';
	};
	void	ClearBuf(void){
			memset(&EVENT_ID,0,sizeof(_TBL_ERROR_EVENT_STR_));
	};
}_TBL_ERROR_EVENT_STR_;

inline ostream& operator<< (ostream& os, const _TBL_ERROR_EVENT_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_ERROR_EVENT_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_ERROR_EVENT_CLASS{
	friend class MEM_ERROR_EVENT_CLASS;
	private:
		_TBL_ERROR_EVENT_STR_		item;
		_TBL_ERROR_EVENT_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_ERROR_EVENT_CLASS();
		~TBL_ERROR_EVENT_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_EVENT_ID(void);
		long	Get_BILL_FLOW_ID(void);
		long	Get_BILLING_ORG_ID(void);
		char*	Get_ORG_CALLING_NBR(void);
		char*	Get_ORG_CALLED_NBR(void);
		char*	Get_THIRD_PARTY_AREA_CODE(void);
		char*	Get_START_DATE(void);
		long	Get_DURATION(void);
		char*	Get_TRUNK_IN(void);
		char*	Get_TRUNK_OUT(void);
		char*	Get_ACCOUNT_AREA_CODE(void);
		char*	Get_ACCOUNT_NBR(void);
		char*	Get_NAS_IP(void);
		long	Get_NAS_PORT_ID(void);
		long	Get_RECV_BYTES(void);
		long	Get_SEND_BYTES(void);
		long	Get_RECV_PACKETS(void);
		long	Get_SEND_PACKETS(void);
		long	Get_SWITCH_ID(void);
		long	Get_FILE_ID(void);
		char*	Get_CREATED_DATE(void);
		long	Get_EVENT_STATE(void);
		char*	Get_STATE_DATE(void);
		long	Get_ERROR_ID(void);
		long	Get_EXTEND_USED_NUM(void);
		long	Get_EXTEND_FIELD_ID1(void);
		char*	Get_EXTEND_VALUE1(void);
		long	Get_EXTEND_FIELD_ID2(void);
		char*	Get_EXTEND_VALUE2(void);
		long	Get_EXTEND_FIELD_ID3(void);
		char*	Get_EXTEND_VALUE3(void);
		long	Get_EXTEND_FIELD_ID4(void);
		char*	Get_EXTEND_VALUE4(void);
		long	Get_EXTEND_FIELD_ID5(void);
		char*	Get_EXTEND_VALUE5(void);
		long	Get_EXTEND_FIELD_ID6(void);
		char*	Get_EXTEND_VALUE6(void);
		long	Get_EXTEND_FIELD_ID7(void);
		char*	Get_EXTEND_VALUE7(void);
		long	Get_EXTEND_FIELD_ID8(void);
		char*	Get_EXTEND_VALUE8(void);
		long	Get_EXTEND_FIELD_ID9(void);
		char*	Get_EXTEND_VALUE9(void);
		long	Get_EXTEND_FIELD_ID10(void);
		char*	Get_EXTEND_VALUE10(void);
		void	Set_EVENT_ID(long	lParam);
		void	Set_BILL_FLOW_ID(long	lParam);
		void	Set_BILLING_ORG_ID(long	lParam);
		void	Set_ORG_CALLING_NBR(char	* sParam);
		void	Set_ORG_CALLED_NBR(char	* sParam);
		void	Set_THIRD_PARTY_AREA_CODE(char	* sParam);
		void	Set_START_DATE(char	* sParam);
		void	Set_DURATION(long	lParam);
		void	Set_TRUNK_IN(char	* sParam);
		void	Set_TRUNK_OUT(char	* sParam);
		void	Set_ACCOUNT_AREA_CODE(char	* sParam);
		void	Set_ACCOUNT_NBR(char	* sParam);
		void	Set_NAS_IP(char	* sParam);
		void	Set_NAS_PORT_ID(long	lParam);
		void	Set_RECV_BYTES(long	lParam);
		void	Set_SEND_BYTES(long	lParam);
		void	Set_RECV_PACKETS(long	lParam);
		void	Set_SEND_PACKETS(long	lParam);
		void	Set_SWITCH_ID(long	lParam);
		void	Set_FILE_ID(long	lParam);
		void	Set_CREATED_DATE(char	* sParam);
		void	Set_EVENT_STATE(long	lParam);
		void	Set_STATE_DATE(char	* sParam);
		void	Set_ERROR_ID(long	lParam);
		void	Set_EXTEND_USED_NUM(long	lParam);
		void	Set_EXTEND_FIELD_ID1(long	lParam);
		void	Set_EXTEND_VALUE1(char	* sParam);
		void	Set_EXTEND_FIELD_ID2(long	lParam);
		void	Set_EXTEND_VALUE2(char	* sParam);
		void	Set_EXTEND_FIELD_ID3(long	lParam);
		void	Set_EXTEND_VALUE3(char	* sParam);
		void	Set_EXTEND_FIELD_ID4(long	lParam);
		void	Set_EXTEND_VALUE4(char	* sParam);
		void	Set_EXTEND_FIELD_ID5(long	lParam);
		void	Set_EXTEND_VALUE5(char	* sParam);
		void	Set_EXTEND_FIELD_ID6(long	lParam);
		void	Set_EXTEND_VALUE6(char	* sParam);
		void	Set_EXTEND_FIELD_ID7(long	lParam);
		void	Set_EXTEND_VALUE7(char	* sParam);
		void	Set_EXTEND_FIELD_ID8(long	lParam);
		void	Set_EXTEND_VALUE8(char	* sParam);
		void	Set_EXTEND_FIELD_ID9(long	lParam);
		void	Set_EXTEND_VALUE9(char	* sParam);
		void	Set_EXTEND_FIELD_ID10(long	lParam);
		void	Set_EXTEND_VALUE10(char	* sParam);
		void	AddItem(void);
		void	ClearBuf(void);
		void	SetBuf(int iBufLen=ARRAY_BUFLEN);
		void	SaveFlush(void);
		void	FreeBuf(void);
		void	Print(ostream& os) const {
				os<<item;
		};
};

inline ostream& operator<< (ostream& os, const TBL_ERROR_EVENT_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_ERROR_EVENT_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_ERROR_EVENT_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_ERROR_EVENT_STR_>		a_item;
		static	hash_map<const long ,_TBL_ERROR_EVENT_STR_* , hash<long>, MEM_ERROR_EVENT_CLASS::eqlong>		hashmap;
		static	bool	bLoadFlag;
	public:
		MEM_ERROR_EVENT_CLASS(char * sTableName=NULL);
		~MEM_ERROR_EVENT_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_ERROR_EVENT_STR_ *	operator [](long opt);
};

#endif
