#ifndef InstTableListMgrKerEx_H_HEADER_INCLUDED
#define InstTableListMgrKerEx_H_HEADER_INCLUDED

#include <stdio.h>
#include <errno.h>
#include "hash_list.h"
#include <list>
#include <map>
using namespace std;

class InstTableListEx{
public:
	int iBillingCycleID;
	int iOrgID;
	int iTableType;
	char sTableName[50];
};

class InstTableListMgrKerEx
{
public:
    InstTableListMgrKerEx(char const *sTable);
    ~InstTableListMgrKerEx();
    bool getTableName(int i_iOrgID,int i_iBillingCycleID,char * o_sTableName);
    char m_sTableName[64];
		int  m_iCnt;
		int  m_iMinOrgLevel;
private:
    void load(char const *sTable);
    bool m_bLoad;
    THashList table_list;
};

#endif /* InstTableListMgrKerEx_H_HEADER_INCLUDED */
