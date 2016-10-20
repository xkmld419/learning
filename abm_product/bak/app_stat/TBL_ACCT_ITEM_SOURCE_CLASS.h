/*VER: 1*/ 
#ifndef _TBL_ACCT_ITEM_SOURCE_CLASS_H__
#define _TBL_ACCT_ITEM_SOURCE_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
using namespace std;
typedef struct _TBL_ACCT_ITEM_SOURCE_STR_{
	long	ITEM_SOURCE_ID;
	long	ACCT_ITEM_TYPE_ID;
	char	ITEM_SOURCE_TYPE[4];
	char	NAME[51];
	char	DESCRIPT[251];
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "ITEM_SOURCE_ID:["<<ITEM_SOURCE_ID<<"]"<<endl;
			os << "ACCT_ITEM_TYPE_ID:["<<ACCT_ITEM_TYPE_ID<<"]"<<endl;
			os << "ITEM_SOURCE_TYPE:["<<ITEM_SOURCE_TYPE<<"]"<<endl;
			os << "NAME:["<<NAME<<"]"<<endl;
			os << "DESCRIPT:["<<DESCRIPT<<"]"<<endl;
	};
	_TBL_ACCT_ITEM_SOURCE_STR_(){
			ITEM_SOURCE_ID=0;
			ACCT_ITEM_TYPE_ID=0;
			ITEM_SOURCE_TYPE[0]='\0';
			NAME[0]='\0';
			DESCRIPT[0]='\0';
	};
	void	ClearBuf(void){
			memset(&ITEM_SOURCE_ID,0,sizeof(_TBL_ACCT_ITEM_SOURCE_STR_));
	};
}_TBL_ACCT_ITEM_SOURCE_STR_;

inline ostream& operator<< (ostream& os, const _TBL_ACCT_ITEM_SOURCE_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_ACCT_ITEM_SOURCE_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_ACCT_ITEM_SOURCE_CLASS{
	friend class MEM_ACCT_ITEM_SOURCE_CLASS;
	private:
		_TBL_ACCT_ITEM_SOURCE_STR_		item;
		_TBL_ACCT_ITEM_SOURCE_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_ACCT_ITEM_SOURCE_CLASS();
		~TBL_ACCT_ITEM_SOURCE_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_ITEM_SOURCE_ID(void);
		long	Get_ACCT_ITEM_TYPE_ID(void);
		char*	Get_ITEM_SOURCE_TYPE(void);
		char*	Get_NAME(void);
		char*	Get_DESCRIPT(void);
		void	Set_ITEM_SOURCE_ID(long	lParam);
		void	Set_ACCT_ITEM_TYPE_ID(long	lParam);
		void	Set_ITEM_SOURCE_TYPE(char	* sParam);
		void	Set_NAME(char	* sParam);
		void	Set_DESCRIPT(char	* sParam);
		void	AddItem(void);
		void	ClearBuf(void);
		void	SetBuf(int iBufLen=ARRAY_BUFLEN);
		void	SaveFlush(void);
		void	FreeBuf(void);
		void	Print(ostream& os) const {
				os<<item;
		};
};

inline ostream& operator<< (ostream& os, const TBL_ACCT_ITEM_SOURCE_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_ACCT_ITEM_SOURCE_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_ACCT_ITEM_SOURCE_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_ACCT_ITEM_SOURCE_STR_>		a_item;
		static	hash_map<const long ,_TBL_ACCT_ITEM_SOURCE_STR_* , hash<long>, MEM_ACCT_ITEM_SOURCE_CLASS::eqlong>		hashmap;
		static  bool bLoadFlag;
	public:
		MEM_ACCT_ITEM_SOURCE_CLASS(char * sTableName=NULL);
		~MEM_ACCT_ITEM_SOURCE_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_ACCT_ITEM_SOURCE_STR_ *	operator [](long opt);
};

#endif
