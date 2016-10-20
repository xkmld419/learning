/*VER: 3*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.
#define _FILE_NAME_ "public/InstTableListMgr.cpp"

#include <iostream>
#include <list>
#include "Environment.h"
#include "Log.h"
#include "hash_list.h"
#include "OrgMgr.h"
#include "InstTableListMgr.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif


OrgMgr * gpOrgMgr;

InstTableListMgrKer * InstTableListMgr::m_poRead = 0;
InstTableListMgrKer * InstTableListMgr::m_poWrite = 0;

int InstTableListMgr::m_iDbFlag = BILL_DB_FLAG;

InstTableListMgr::InstTableListMgr(){
    #ifdef PRIVATE_MEMORY
    if (!m_poRead) {
        m_poRead = new InstTableListMgrKer ('R');
    }

    if (!m_poWrite) {
        m_poWrite = new InstTableListMgrKer ('W');
    }
    #else
    if (BILL_DB_FLAG != m_iDbFlag) {
        if (!m_poRead) {
            m_poRead = new InstTableListMgrKer ('R');
        }

        if (!m_poWrite) {
            m_poWrite = new InstTableListMgrKer ('W');
        }
    }
    #endif
}


InstTableListMgr::~InstTableListMgr()
{
}


bool InstTableListMgr::getReBillTableName(
                                         int iBillingCycleID,int iBillFlowID,
                                         int iTableType,char* sReBillTableName, bool ifRead)
{
    #ifdef PRIVATE_MEMORY 
    if (ifRead) {
        return m_poRead->getReBillTableName (iBillingCycleID,
                                             iBillFlowID, iTableType, sReBillTableName);
    } else {
        return m_poWrite->getReBillTableName (iBillingCycleID,
                                              iBillFlowID, iTableType, sReBillTableName);
    }
    #else
    //回退接口表
    //以后需要增加对回退表的配置修改此处 
    if ( BILL_DB_FLAG != m_iDbFlag ) {
        if (ifRead) {
            return m_poRead->getReBillTableName (iBillingCycleID,
                                                 iBillFlowID, iTableType, sReBillTableName);
        } else {
            return m_poWrite->getReBillTableName (iBillingCycleID,
                                                  iBillFlowID, iTableType, sReBillTableName);
        }
    } else {
        if ( 0== sReBillTableName)
            return false;
        else {
            strcpy(sReBillTableName,REBILL_TABLE_NAME);
            return true;
        }
    }
    #endif
}




bool InstTableListMgr::getRBTableType(int nTableType,
                                      int& nRBTable, bool ifRead)
{
    #ifdef PRIVATE_MEMORY
    if (ifRead) {
        return m_poRead->getRBTableType(nTableType, nRBTable);
    } else {
        return m_poWrite->getRBTableType(nTableType, nRBTable);
    }
    #else 
    if (BILL_DB_FLAG != m_iDbFlag) {
        if (ifRead) {
            return m_poRead->getRBTableType(nTableType, nRBTable);
        } else {
            return m_poWrite->getRBTableType(nTableType, nRBTable);
        }
    } else {
        EventTableType *pTableType = 0;
        EventTableType *pType =0;
        unsigned int iIdx = 0;

        pType = (EventTableType *)G_PPARAMINFO->m_poTableTypeList;
        if (G_PPARAMINFO->m_poTableTypeIndex->get(nTableType,&iIdx)) {
            pTableType = pType + iIdx;
            if (pTableType->iRbTableTypeID != -2) {
                nRBTable = pTableType->iRbTableTypeID;
                return true;
            } else
                return false;
        } else
            return false;
    }
    #endif

}



bool InstTableListMgr::getTableName(int i_iBillingCycleID,
                                    int i_iBillFlowID,
                                    int i_iTableType,
                                    int i_iOrgID,
                                    char * o_sTableName,
                                    char* o_sTableNameRB, bool ifRead)
{
    #ifdef PRIVATE_MEMORY
    if (ifRead) {
        return m_poRead->getTableName(i_iBillingCycleID,
                                      i_iBillFlowID, i_iTableType, i_iOrgID, o_sTableName,
                                      o_sTableNameRB);
    } else {
        return m_poWrite->getTableName(i_iBillingCycleID,
                                       i_iBillFlowID, i_iTableType, i_iOrgID, o_sTableName,
                                       o_sTableNameRB);
    }
    #else 
    if ( BILL_DB_FLAG != m_iDbFlag) {
        if (ifRead) {
            return m_poRead->getTableName(i_iBillingCycleID,
                                          i_iBillFlowID, i_iTableType, i_iOrgID, o_sTableName,
                                          o_sTableNameRB);
        } else {
            return m_poWrite->getTableName(i_iBillingCycleID,
                                           i_iBillFlowID, i_iTableType, i_iOrgID, o_sTableName,
                                           o_sTableNameRB);
        }
    } else {
        InstTableList * pTableList = 0;
        InstTableList * pList = 0;
        unsigned int iIdx = 0;

        pList = (InstTableList*)G_PPARAMINFO->m_poInstTableList;
        char sKey[32]={0};
        sprintf (sKey, "%dB%dT%d", i_iBillingCycleID,i_iBillFlowID,i_iTableType);
        if (G_PPARAMINFO->m_poInstTableIndex->get(sKey,&iIdx)) {
            pTableList = pList + iIdx;
            while (1) {
                if ( (pTableList->iBillingCycleID == i_iBillingCycleID||pTableList->iBillingCycleID == -2)
                     && (pTableList->iBillFlowID == i_iBillFlowID ||pTableList->iBillFlowID == -2)
                     && (pTableList->iTableType == i_iTableType )
                     && (gpOrgMgr->getBelong(i_iOrgID,pTableList->iOrgID)||(pTableList->iOrgID == i_iOrgID)||(pTableList->iOrgID==-2))
                   )
                    if ((ifRead ==true&&(pTableList->m_sRole[0]=='R'||(strcmp(pTableList->m_sRole,"00")==0)))||
                        (ifRead ==false&&(pTableList->m_sRole[0]=='W'||(strcmp(pTableList->m_sRole,"00")==0)))) {
                        strcpy(o_sTableName,pTableList->sTableName);
                        if (0 != o_sTableNameRB) {
                            strcpy(o_sTableNameRB,pTableList->sTableNameRB);
                            if (0== o_sTableNameRB[0]) {
                                int nRbTableType = 0;
                                if ( getRBTableType(i_iTableType,nRbTableType,ifRead)) {
                                    if (nRbTableType != i_iTableType) {
                                        getTableName(i_iBillingCycleID,i_iBillFlowID,nRbTableType,i_iOrgID,o_sTableNameRB,NULL,ifRead);
                                    }
                                }
                            }
                        }
                        if (strlen (o_sTableName) == 0) return false;
                        return true;
                    }

                if (pTableList->m_iNextOffset)
                    pTableList = pList + pTableList->m_iNextOffset;
                else
                    break;
            }
        }

        sprintf (sKey, "%dB-2T%d", i_iBillingCycleID,i_iTableType);
        if (G_PPARAMINFO->m_poInstTableIndex->get(sKey,&iIdx)) {
            pTableList = pList + iIdx;
            while (1) {
                if ( (pTableList->iBillingCycleID == i_iBillingCycleID)
                     && (pTableList->iBillFlowID == i_iBillFlowID ||pTableList->iBillFlowID == -2)
                     && (pTableList->iTableType == i_iTableType )
                     && (gpOrgMgr->getBelong(i_iOrgID,pTableList->iOrgID)||(i_iOrgID==-2&&pTableList->iOrgID==-2))
                   )
                    if ((ifRead ==true&&(pTableList->m_sRole[0]=='R'||(strcmp(pTableList->m_sRole,"00")==0)))||
                        (ifRead ==false&&(pTableList->m_sRole[0]=='W'||(strcmp(pTableList->m_sRole,"00")==0)))) {
                        strcpy(o_sTableName,pTableList->sTableName);
                        if (0 != o_sTableNameRB) {
                            strcpy(o_sTableNameRB,pTableList->sTableNameRB);
                            if (0== o_sTableNameRB[0]) {
                                int nRbTableType = 0;
                                if ( getRBTableType(i_iTableType,nRbTableType,ifRead)) {
                                    if (nRbTableType != i_iTableType) {
                                        getTableName(i_iBillingCycleID,i_iBillFlowID,nRbTableType,i_iOrgID,o_sTableNameRB,NULL,ifRead);
                                    }
                                }
                            }
                        }
                        if (strlen (o_sTableName) == 0) return false;
                        return true;
                    }

                if (pTableList->m_iNextOffset)
                    pTableList = pList + pTableList->m_iNextOffset;
                else
                    break;
            }

        }

        sprintf (sKey, "-2B%dT%d", i_iBillFlowID,i_iTableType);
        if (G_PPARAMINFO->m_poInstTableIndex->get(sKey,&iIdx)) {
            pTableList = pList + iIdx;
            while (1) {
                if ( (pTableList->iBillingCycleID == i_iBillingCycleID || pTableList->iBillingCycleID== -2)
                     && (pTableList->iBillFlowID == i_iBillFlowID )
                     && (pTableList->iTableType == i_iTableType )
                     && (gpOrgMgr->getBelong(i_iOrgID,pTableList->iOrgID)||(pTableList->iOrgID==-2))
                   )
                    if ((ifRead ==true&&(pTableList->m_sRole[0]=='R'||(strcmp(pTableList->m_sRole,"00")==0)))||
                        (ifRead ==false&&(pTableList->m_sRole[0]=='W'||(strcmp(pTableList->m_sRole,"00")==0)))) {
                        strcpy(o_sTableName,pTableList->sTableName);
                        if (0 != o_sTableNameRB) {
                            strcpy(o_sTableNameRB,pTableList->sTableNameRB);
                            if (0== o_sTableNameRB[0]) {
                                int nRbTableType = 0;
                                if ( getRBTableType(i_iTableType,nRbTableType,ifRead)) {
                                    if (nRbTableType != i_iTableType) {
                                        getTableName(i_iBillingCycleID,i_iBillFlowID,nRbTableType,i_iOrgID,o_sTableNameRB,NULL,ifRead);
                                    }
                                }
                            }
                        }
                        if (strlen (o_sTableName) == 0) return false;
                        return true;
                    }

                if (pTableList->m_iNextOffset)
                    pTableList = pList + pTableList->m_iNextOffset;
                else
                    break;
            }

        }

        sprintf (sKey, "-2B-2T%d", i_iTableType);
        if (G_PPARAMINFO->m_poInstTableIndex->get(sKey,&iIdx)) {
            pTableList = pList + iIdx;
            while (1) {
                if ( (pTableList->iBillingCycleID == i_iBillingCycleID||pTableList->iBillingCycleID == -2)
                     && (pTableList->iBillFlowID == i_iBillFlowID ||pTableList->iBillFlowID == -2)
                     && (pTableList->iTableType == i_iTableType )
                     && (gpOrgMgr->getBelong(i_iOrgID,pTableList->iOrgID)||(pTableList->iOrgID == i_iOrgID)||(pTableList->iOrgID==-2))
                   )
                    if ((ifRead ==true&&(pTableList->m_sRole[0]=='R'||(strcmp(pTableList->m_sRole,"00")==0)))||
                        (ifRead ==false&&(pTableList->m_sRole[0]=='W'||(strcmp(pTableList->m_sRole,"00")==0)))) {
                        strcpy(o_sTableName,pTableList->sTableName);
                        if (0 != o_sTableNameRB) {
                            strcpy(o_sTableNameRB,pTableList->sTableNameRB);
                            if (0== o_sTableNameRB[0]) {
                                int nRbTableType = 0;
                                if ( getRBTableType(i_iTableType,nRbTableType,ifRead)) {
                                    if (nRbTableType != i_iTableType) {
                                        getTableName(i_iBillingCycleID,i_iBillFlowID,nRbTableType,i_iOrgID,o_sTableNameRB,NULL,ifRead);
                                    }
                                }
                            }
                        }
                        if (strlen (o_sTableName) == 0) return false;
                        return true;
                    }

                if (pTableList->m_iNextOffset)
                    pTableList = pList + pTableList->m_iNextOffset;
                else
                    break;
            }

        }

        return false;
    }
    #endif
}



InstTableListMgrKer::InstTableListMgrKer(char cRole)
{
    m_bLoad = false;
    if (!gpOrgMgr)
        gpOrgMgr = new OrgMgr ();

    m_cRole = cRole;

    if (!m_bLoad) {
        load();
        m_bLoad = true;
    }
}

InstTableListMgrKer::~InstTableListMgrKer()
{
    m_bLoad = false;
    delete gpOrgMgr;
    free();
}

void InstTableListMgrKer::free()
{
    freeHashList(table_list);
}

void InstTableListMgrKer::load()
{
    char sSql[4096];

    sprintf (sSql, "SELECT "
             "NVL(BILLING_CYCLE_ID,-2) BILLING_CYCLE_ID, "
             "NVL(BILLFLOW_ID,-2) BILLFLOW_ID, "
             "TABLE_TYPE, "
             "NVL(ORG_ID,-2) ORG_ID, "
             "TABLE_NAME "
             "FROM B_INST_TABLE_LIST_CFG "
             "where nvl(op_role, '%c')='%c' "
             " ORDER BY NVL(BILLING_CYCLE_ID,-2), "
             "BILLFLOW_ID,TABLE_TYPE,NVL(ORG_ID,-2)", m_cRole, m_cRole);

    int iCnt;
    InstTableList temp;
    char key[20];

    if (m_bLoad) return;

    initHashList (&table_list);

    DEFINE_QUERY(qry);
    DEFINE_QUERY(qry1);


    char sSql1[4096];

    sprintf (sSql1, "SELECT TABLE_NAME "
             "FROM B_INST_TABLE_LIST_CFG A, "
             "(SELECT nvl(RB_TABLE_TYPE_ID,0) RB_TABLE_TYPE_ID FROM "
             "B_EVENT_TABLE_TYPE B WHERE B.EVENT_TABLE_TYPE_ID "
             "=:EVENT_TABLE_TYPE_ID ) B "
             "WHERE A.TABLE_TYPE = B.RB_TABLE_TYPE_ID "
             "AND NVL(A.BILLFLOW_ID,-2)=:BILLFLOW_ID "
             "AND A.ORG_ID=:ORG_ID "
             "AND A.BILLING_CYCLE_ID=:BILLING_CYCLE_ID "
             "and nvl(a.op_role, '%c')='%c'", m_cRole, m_cRole); 


    qry.setSQL(sSql);
    qry.open();

    iCnt = 0;

    while (qry.next())
        iCnt++; 

    if (iCnt == 0) {
        printf("InstTableListMgrKer::load[B_INST_TABLE_LIST表为空]\n");
        Log::log(0,"InstTableListMgrKer::load[B_INST_TABLE_LIST表为空]\n");
        return;
    }

    qry.close();
    qry.setSQL( sSql);
    qry.open();

    while (qry.next()) {
        temp.iBillingCycleID = qry.field("BILLING_CYCLE_ID").asInteger();
        temp.iBillFlowID = qry.field("BILLFLOW_ID").asInteger();
        temp.iTableType = qry.field("TABLE_TYPE").asInteger();
        temp.iOrgID = qry.field("ORG_ID").asInteger();
        strcpy(temp.sTableName,qry.field("TABLE_NAME").asString());

        sprintf (key, "%dB%dT%d", temp.iBillingCycleID,temp.iBillFlowID,temp.iTableType);

        qry1.setSQL(sSql1);
        qry1.setParameter("EVENT_TABLE_TYPE_ID",temp.iTableType);            
        qry1.setParameter("BILLFLOW_ID",temp.iBillFlowID);
        qry1.setParameter("ORG_ID",temp.iOrgID);
        qry1.setParameter("BILLING_CYCLE_ID",temp.iBillingCycleID);
        qry1.open();
        if (qry1.next()) {
            strcpy(temp.sTableNameRB,qry1.field(0).asString());
        } else {
            memset(temp.sTableNameRB,0,sizeof(temp.sTableNameRB));
        }
        qry1.close();
        if (insertHashList(table_list,key,&temp,sizeof (InstTableList)) == -1)
            THROW(MBC_InstTableListMgr+1);
    }

    qry.close();
    sprintf(sSql,"SELECT EVENT_TABLE_TYPE_ID,RB_TABLE_TYPE_ID FROM \
                    B_EVENT_TABLE_TYPE A WHERE A.RB_TABLE_TYPE_ID \
                    IS NOT NULL");
    qry.setSQL(sSql);
    qry.open();
    while (qry.next()) {
        int nTableType = qry.field(0).asInteger();
        int nRBTableType = qry.field(1).asInteger();
        m_MapTableID[nTableType]=nRBTableType;       
    }
    qry.close();

}

int cmp (const void *list_data, const void *cmp_data)
{
    InstTableList *p1;
    InstTableList *p2;

    p1 = (InstTableList *)list_data;
    p2 = (InstTableList *)cmp_data;


    if (
       (p1->iBillingCycleID == p2->iBillingCycleID || p1->iBillingCycleID==-2) && 
       (p1->iBillFlowID == p2->iBillFlowID || p1->iBillFlowID == -2) && 
       (p1->iTableType == p2->iTableType) && 
       (gpOrgMgr->getBelong(p2->iOrgID,p1->iOrgID) || (p2->iOrgID==p1->iOrgID)||(p1->iOrgID==-2))
       )
        return 0;
    return 1;
}

int cmp3 (const void *list_data, const void *cmp_data)
{                   
    InstTableList *p1; 
    InstTableList *p2;

    p1 = (InstTableList *)list_data;
    p2 = (InstTableList *)cmp_data;


    if (
       (p1->iBillFlowID == p2->iBillFlowID)&&
       (p1->iBillingCycleID == p2->iBillingCycleID || p1->iBillingCycleID==-2) &&
       (p1->iTableType == p2->iTableType) &&
       (gpOrgMgr->getBelong(p2->iOrgID,p1->iOrgID) || ( p1->iOrgID==-2 ) )
       )
        return 0;
    return 1;
}


int cmp2 (const void *list_data, const void *cmp_data)
{
    InstTableList *p1;
    InstTableList *p2;

    p1 = (InstTableList *)list_data;
    p2 = (InstTableList *)cmp_data;


    if (
       (p1->iBillingCycleID == p2->iBillingCycleID)&&
       (p1->iBillFlowID == p2->iBillFlowID || p1->iBillFlowID == -2) &&
       (p1->iTableType == p2->iTableType) &&
       (gpOrgMgr->getBelong(p2->iOrgID,p1->iOrgID)||(p1->iOrgID==-2&&p2->iOrgID==-2))
       )
        return 0;
    return 1;
}

int cmp1 (const void *list_data, const void *cmp_data)
{
    InstTableList *p1;
    InstTableList *p2;

    p1 = (InstTableList *)list_data;
    p2 = (InstTableList *)cmp_data;


    if (
       (p1->iBillFlowID == p2->iBillFlowID) && 
       (p1->iTableType == p2->iTableType) && 
       gpOrgMgr->getBelong(p2->iOrgID,p1->iOrgID)
       )
        return 0;
    return 1;
}


bool InstTableListMgrKer::getRBTableType(int nTableType,int& nRBTable)
{
    if (m_MapTableID.empty())
        return false;

    map<int,int>::iterator it = m_MapTableID.find(nTableType);
    if (it == m_MapTableID.end())
        return false;
    nRBTable = it->second;
    return true; 
}
bool InstTableListMgrKer::getReBillTableName(int i_iBillingCycleID,int i_iBillFlowID,
                                             int i_iTableType,char* o_sReBillTableName)
{
    //回退接口表
    //以后需要增加对回退表的配置修改此处
    if ( 0== o_sReBillTableName)
        return false;
    else {
        strcpy(o_sReBillTableName,REBILL_TABLE_NAME);
        return true;
    }
}


bool InstTableListMgrKer::getTableName(int i_iBillingCycleID,int i_iBillFlowID,int i_iTableType,
                                       int i_iOrgID,char * o_sTableName,char* o_sTableNameRB)
{       
    InstTableList temp;
    InstTableList * node;
    // char key[11];
    // modified by xueqt [20060522fwj]
    char key[32];

    temp.iBillingCycleID = i_iBillingCycleID;
    temp.iBillFlowID = i_iBillFlowID;
    temp.iTableType = i_iTableType;
    temp.iOrgID = i_iOrgID;

    sprintf (key, "%dB%dT%d", i_iBillingCycleID,i_iBillFlowID,i_iTableType);

    node = (InstTableList *)searchHashList(table_list, key, &temp, &cmp);

    if (node == NULL) {
        sprintf (key, "%dB-2T%d",i_iBillingCycleID,i_iTableType);
        node = (InstTableList *)searchHashList(table_list, key, &temp, &cmp2);

        if (node == NULL) {
            sprintf(key,"-2B%dT%d",i_iBillFlowID,i_iTableType);
            node = (InstTableList *)searchHashList(table_list,key,&temp,&cmp3);
            if (node == NULL) {
                sprintf(key,"-2B-2T%d",i_iTableType);
                node = (InstTableList* )searchHashList(table_list,key,&temp,&cmp);
                if (node == NULL)
                    return false;
            }
        }
    }
    strcpy(o_sTableName,node->sTableName);
    if (0 != o_sTableNameRB) {
        strcpy(o_sTableNameRB,node->sTableNameRB);
        if (0== o_sTableNameRB[0]) {
            int nRbTableType = 0;
            if ( getRBTableType(i_iTableType,nRbTableType)) {
                if (nRbTableType != i_iTableType) {
                    getTableName(i_iBillingCycleID,i_iBillFlowID,nRbTableType,i_iOrgID,o_sTableNameRB);
                }
            }
        }
    }
    if (strlen (o_sTableName) == 0) return false;

    return true;
}
