/*VER: 1*/ 
#ifndef _TBL_STAFF_CLASS_H__
#define _TBL_STAFF_CLASS_H__
#include <iostream>
#include "TOCIQuery.h"
#include <string>
#include <vector>
#include <hash_map.h>
#define ARRAY_BUFLEN  500
typedef struct _TBL_STAFF_STR_{
	long	STAFF_ID;
	char	STAFF_CODE[16];
	char	PASSWD[31];
	char	STAFF_DESC[251];
	long	ORGAN_ID;
	long	PARENT_PARTYROLEID;
	long	BSS_ORG_ID;
	long	WORK_GROUP_ID;
	void	Print(ostream& os) const {
			os << "******************************"<<endl; 
			os << "STAFF_ID:["<<STAFF_ID<<"]"<<endl;
			os << "STAFF_CODE:["<<STAFF_CODE<<"]"<<endl;
			os << "PASSWD:["<<PASSWD<<"]"<<endl;
			os << "STAFF_DESC:["<<STAFF_DESC<<"]"<<endl;
			os << "ORGAN_ID:["<<ORGAN_ID<<"]"<<endl;
			os << "PARENT_PARTYROLEID:["<<PARENT_PARTYROLEID<<"]"<<endl;
			os << "BSS_ORG_ID:["<<BSS_ORG_ID<<"]"<<endl;
			os << "WORK_GROUP_ID:["<<WORK_GROUP_ID<<"]"<<endl;
	};
	_TBL_STAFF_STR_(){
			STAFF_ID=0;
			STAFF_CODE[0]='\0';
			PASSWD[0]='\0';
			STAFF_DESC[0]='\0';
			ORGAN_ID=0;
			PARENT_PARTYROLEID=0;
			BSS_ORG_ID=0;
			WORK_GROUP_ID=0;
	};
	void	ClearBuf(void){
			memset(&STAFF_ID,0,sizeof(_TBL_STAFF_STR_));
	};
}_TBL_STAFF_STR_;

inline ostream& operator<< (ostream& os, const _TBL_STAFF_STR_ &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const _TBL_STAFF_STR_ *obj) {
	obj->Print(os);
	return os;
}

class TBL_STAFF_CLASS{
	friend class MEM_STAFF_CLASS;
	private:
		_TBL_STAFF_STR_		item;
		_TBL_STAFF_STR_		*a_item;
		int		iCurPos;
		TOCIQuery	*		m_qry;
		string		sConditionSql;
		string		sTableName;
		int		iBufferSize;
	public:
		TBL_STAFF_CLASS();
		~TBL_STAFF_CLASS();
		void	AddCondition(string sql);
		void	SetTableName(string sTableName);
		void	Open();
		bool	Next();
		long	Get_STAFF_ID(void);
		char*	Get_STAFF_CODE(void);
		char*	Get_PASSWD(void);
		char*	Get_STAFF_DESC(void);
		long	Get_ORGAN_ID(void);
		long	Get_PARENT_PARTYROLEID(void);
		long	Get_BSS_ORG_ID(void);
		long	Get_WORK_GROUP_ID(void);
		void	Set_STAFF_ID(long	lParam);
		void	Set_STAFF_CODE(char	* sParam);
		void	Set_PASSWD(char	* sParam);
		void	Set_STAFF_DESC(char	* sParam);
		void	Set_ORGAN_ID(long	lParam);
		void	Set_PARENT_PARTYROLEID(long	lParam);
		void	Set_BSS_ORG_ID(long	lParam);
		void	Set_WORK_GROUP_ID(long	lParam);
		void	AddItem(void);
		void	ClearBuf(void);
		void	SetBuf(int iBufLen=ARRAY_BUFLEN);
		void	SaveFlush(void);
		void	FreeBuf(void);
		void	Print(ostream& os) const {
				os<<item;
		};
};

inline ostream& operator<< (ostream& os, const TBL_STAFF_CLASS &obj) {
	obj.Print(os);
	return os;
}

inline ostream& operator<< (ostream& os, const TBL_STAFF_CLASS *obj) {
	obj->Print(os);
	return os;
}

class MEM_STAFF_CLASS{
	private:
		struct eqlong
		{
			bool	operator()(const long s1, const long s2) const
			{
				return (s1==s2);
			}
		};

		static	vector<_TBL_STAFF_STR_>		a_item;
		static	hash_map<const long ,_TBL_STAFF_STR_* , hash<long>, MEM_STAFF_CLASS::eqlong>		hashmap;
	public:
		MEM_STAFF_CLASS(char * sTableName=NULL);
		~MEM_STAFF_CLASS();
		void	Load(char *sTableName=NULL);
		void	UnLoad(void);
		_TBL_STAFF_STR_ *	operator [](long opt);
};

#endif
