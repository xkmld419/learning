/*VER: 1*/ 
#ifndef _TBL_PAYMENT_CLASS_H__
#define _TBL_PAYMENT_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
using namespace std;
typedef struct _TBL_PAYMENT_STR_{
	long	PAYMENT_ID;
	long	PAYMENT_METHOD;
	long	PAYED_METHOD;
	char	OPERATION_TYPE[4];
	long	OPERATED_PAYMENT_SERIAL_NBR;
	long	AMOUNT;
	char	PAYMENT_DATE[15];
	char	STATE[4];
	char	STATE_DATE[15];
	char	CREATED_DATE[15];
	long	STAFF_ID;
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "PAYMENT_ID:["<<PAYMENT_ID<<"]"<<endl;
			os << "PAYMENT_METHOD:["<<PAYMENT_METHOD<<"]"<<endl;
			os << "PAYED_METHOD:["<<PAYED_METHOD<<"]"<<endl;
			os << "OPERATION_TYPE:["<<OPERATION_TYPE<<"]"<<endl;
			os << "OPERATED_PAYMENT_SERIAL_NBR:["<<OPERATED_PAYMENT_SERIAL_NBR<<"]"<<endl;
			os << "AMOUNT:["<<AMOUNT<<"]"<<endl;
			os << "PAYMENT_DATE:["<<PAYMENT_DATE<<"]"<<endl;
			os << "STATE:["<<STATE<<"]"<<endl;
			os << "STATE_DATE:["<<STATE_DATE<<"]"<<endl;
			os << "CREATED_DATE:["<<CREATED_DATE<<"]"<<endl;
			os << "STAFF_ID:["<<STAFF_ID<<"]"<<endl;
	};
	_TBL_PAYMENT_STR_(){
			PAYMENT_ID=0;
			PAYMENT_METHOD=0;
			PAYED_METHOD=0;
			OPERATION_TYPE[0]='\0';
			OPERATED_PAYMENT_SERIAL_NBR=0;
			AMOUNT=0;
			PAYMENT_DATE[0]='\0';
			STATE[0]='\0';
			STATE_DATE[0]='\0';
			CREATED_DATE[0]='\0';
			STAFF_ID=0;
	};
	void	ClearBuf(void){
			memset(&PAYMENT_ID,0,sizeof(_TBL_PAYMENT_STR_));
	};
}_TBL_PAYMENT_STR_;

inline ostream& operator<< (ostream& os, const _TBL_PAYMENT_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_PAYMENT_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_PAYMENT_CLASS{
	friend class MEM_PAYMENT_CLASS;
	private:
		_TBL_PAYMENT_STR_		item;
		_TBL_PAYMENT_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_PAYMENT_CLASS();
		~TBL_PAYMENT_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_PAYMENT_ID(void);
		long	Get_PAYMENT_METHOD(void);
		long	Get_PAYED_METHOD(void);
		char*	Get_OPERATION_TYPE(void);
		long	Get_OPERATED_PAYMENT_SERIAL_NBR(void);
		long	Get_AMOUNT(void);
		char*	Get_PAYMENT_DATE(void);
		char*	Get_STATE(void);
		char*	Get_STATE_DATE(void);
		char*	Get_CREATED_DATE(void);
		long	Get_STAFF_ID(void);
		void	Set_PAYMENT_ID(long	lParam);
		void	Set_PAYMENT_METHOD(long	lParam);
		void	Set_PAYED_METHOD(long	lParam);
		void	Set_OPERATION_TYPE(char	* sParam);
		void	Set_OPERATED_PAYMENT_SERIAL_NBR(long	lParam);
		void	Set_AMOUNT(long	lParam);
		void	Set_PAYMENT_DATE(char	* sParam);
		void	Set_STATE(char	* sParam);
		void	Set_STATE_DATE(char	* sParam);
		void	Set_CREATED_DATE(char	* sParam);
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

inline ostream& operator<< (ostream& os, const TBL_PAYMENT_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_PAYMENT_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_PAYMENT_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_PAYMENT_STR_>		a_item;
		static	hash_map<const long ,_TBL_PAYMENT_STR_* , hash<long>, MEM_PAYMENT_CLASS::eqlong>		hashmap;
	public:
		MEM_PAYMENT_CLASS(char * sTableName=NULL);
		~MEM_PAYMENT_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_PAYMENT_STR_ *	operator [](long opt);
};

#endif
