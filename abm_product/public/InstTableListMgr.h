/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef INSTTABLELISTMGR_H_HEADER_INCLUDED
#define INSTTABLELISTMGR_H_HEADER_INCLUDED

#include <stdio.h>
#include <errno.h>
#include "hash_list.h"
#include <list>
#include <map>

#define     BILL_DB_FLAG        1
#define     STAT_DB_FLAG        2
#define     QUERY_DB_FLAG       3

#define     REBILL_TABLE_NAME       "S_REDO_RB_INTERFACE"

using namespace std;

class InstTableList {
public:
    int iBillingCycleID;
    int iBillFlowID;
    int iOrgID;
    int iTableType;
    char sTableName[50];
    char sTableNameRB[50];
#ifndef PRIVATE_MEMORY
    unsigned int m_iNextOffset;
    char m_sRole[2];
#endif

};

class EventTableType {
public:
    int iTableTypeID;
    char sPrefixTableName[31];
    int iRbTableTypeID;
    int iPriceTableTypeID;
    int iParentTableTypeID;
    int iDBFlag;
};



class InstTableListMgrKer {
public:
    InstTableListMgrKer(char cRole);
    ~InstTableListMgrKer();

    bool getReBillTableName(int i_iBillingCycleID,int i_iBillFlowID,int i_iTableType,char* o_sReBillTableName);

    bool getRBTableType(int nTableType,int& nRBTable);
    bool getTableName(int i_iBillingCycleID,int i_iBillFlowID,int i_iTableType,int i_iOrgID,char * o_sTableName,char* o_sTableNameRB=0);

private:
    void load();
    void free();
    bool m_bLoad;
    THashList table_list;
    map<int,int>m_MapTableID;   //主表和轨迹表的对应关系
    char m_cRole;


};

class InstTableListMgr {
public:
    InstTableListMgr();
    ~InstTableListMgr();

    bool getReBillTableName(int i_iBillingCycleID,
                            int i_iBillFlowID,
                            int i_iTableType,
                            char* o_sReBillTableName,
                            bool ifRead=true);

    bool getRBTableType(int nTableType,int& nRBTable, bool ifRead=true);

    bool getTableName(int i_iBillingCycleID,
                      int i_iBillFlowID,
                      int i_iTableType,
                      int i_iOrgID,
                      char * o_sTableName,
                      char* o_sTableNameRB=0, bool ifRead=true);

		bool getSpecialTableName(int i_iBillingCycleID,
                             int i_iBillFlowID,
                             int i_iTableType,
                             int i_iOrgID,
                             char * o_sTableName,
                             char* o_sTableNameRB=0, bool ifRead=true);
private:

    static InstTableListMgrKer *m_poRead;
    static InstTableListMgrKer *m_poWrite;

public:
    static int m_iDbFlag ;      //Global : 20101220

};



#endif /* INSTTABLELISTMGR_H_HEADER_INCLUDED */
