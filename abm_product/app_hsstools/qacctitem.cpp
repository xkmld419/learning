/*VER: 3*/ 
#include <iostream>
#include <string.h>
#include <vector>
#include "Environment.h"
#include "Process.h"
#include "Date.h"
#include "Log.h"
#include "TransactionMgr.h"
#include "StdEvent.h"

using namespace std;

#ifdef DEF_AIX
    #ifdef v_data
    #undef v_data
    #endif
#endif

class QAcctItem : public AggrMgr
{
  public:
        int run();

  public:
        QAcctItem();

private:
        bool getServ(long &lServID);
        bool update (AcctItemData &itemData);
        bool getServAcctItemAll(long lServID, vector<AcctItemData> &v_data);
        bool prepare();
        void printUsage();
        int updateServFee();
        int queryServFee();
  private:
        int m_iCount;
        int m_iCount2;
        int m_iCount3;
        char *m_sTableServ;
        char *m_sTableFee;
        long m_lServID;
        int iBillingCycleID;
};

int main(int argc, char *argv[])
{
        g_argc = argc;
        g_argv = argv;

        QAcctItem x;

        return x.run ();
}

bool QAcctItem::prepare()
{
        int iTemp, jTemp;
        iTemp = 1;
        jTemp = g_argc;
        if(jTemp == 1)
        {
        printUsage();
        return false;
        }
        bool bServ(false),bU(false);
        if (g_argv[iTemp][0] != '-')
        {
                if (jTemp > 1) 
                {
                        m_lServID = strtol (g_argv[1], 0, 10);
                        bServ = true;
                }
                if (jTemp > 2)
                {
                        iBillingCycleID = atoi (g_argv[2]);
                } else 
                {
                        Date d;
                        if (!G_PUSERINFO->getServ (G_SERV, m_lServID, d.toString ()))
                        {
                                cout<<" 无有效的用户资料 "<<endl;
                                return false;
                        }
                        BillingCycle const * pCycle = G_PSERV->getBillingCycle ();
                        if (!pCycle)
                        {
                                cout <<" 取帐期失败 "<<endl;
                                return false;
                        }
                        iBillingCycleID = pCycle->getBillingCycleID ();
                }
                return true;
        }
        switch (g_argv[iTemp][1] | 0x20) 
        {
       case 'u':
              if(jTemp == 4)
                  {
                          bU = true;
                          m_sTableServ = g_argv[iTemp+1];
                          m_sTableFee = g_argv[iTemp+2];
                          break; 
                  }
           default:
                 printUsage ();
                 return false;
                 break;
        }
    if (!bServ && !bU)
    {
        printUsage();
        return false;
    }
        return true;
}


int QAcctItem::queryServFee()
{
    vector<AcctItemData *> v;
    getAllItemAggr(m_lServID, iBillingCycleID, v);
    int j = v.size ();
    cout << endl;
    cout << "用户: " << m_lServID << "\t" << "帐期: " << iBillingCycleID << endl;
    cout << endl;
    #ifdef ORG_BILLINGCYCLE
    cout << "ACCT_ITEM_ID OFFER_INS_ID VALUE   ITEM_TYPE SO CS ACCT_ID      STATE_DATE     CLASS_ID ORG_BC"
    << endl;
    #else
    cout << "ACCT_ITEM_ID OFFER_INS_ID VALUE   ITEM_TYPE SO CS ACCT_ID      STATE_DATE     CLASS_ID"
            << endl;
    #endif
    cout << "------------ ------------ ------- --------- -- -- ------------ -----------------------"
            << endl;

    long lTotal (0);
    
    #ifdef SHM_FILE_USERINFO
    G_SHMFILE;
    #endif
    
    for (int i=0; i<j; i++) {
        AcctItemData * p = v[i];

        char sTemp[1024];

        #ifdef ORG_BILLINGCYCLE
        sprintf (sTemp, "%12ld %12ld %12ld %9d %2d %2d %12ld %s %6d %8d", 
                 p->m_lAcctItemID , p->m_lOfferInstID,
                 p->m_lValue , p->m_iPayItemTypeID , 
                 p->m_iItemSourceID, p->m_iItemClassID, p->m_lAcctID, p->m_sStateDate,p->m_iItemClassID,
                 p->m_iOrgBillingCycleID);
        #else
        sprintf (sTemp, "%12ld %12ld %12ld %9d %2d %2d %12ld %s %6d", 
                            p->m_lAcctItemID , p->m_lOfferInstID,
                        p->m_lValue , p->m_iPayItemTypeID , 
                 p->m_iItemSourceID, p->m_iItemClassID, p->m_lAcctID, p->m_sStateDate,p->m_iItemClassID);
        #endif

        lTotal += p->m_lValue;

        cout << sTemp << endl;

    }

    char sTemp[1024];

    sprintf (sTemp, "TOTAL: %ld", lTotal);
    cout << endl << sTemp << endl;
    return 0;
}

bool QAcctItem::update(AcctItemData &itemData)
{
        bool bNew;
        //内存更新
        insertAcctItemAggr(itemData,bNew);
        if(bNew)
                m_iCount3++;
        else
                m_iCount2++;
        return true;
}

bool QAcctItem::getServ(long &lServID)
{
        static TOCIQuery * qry = NULL;

        if (NULL == qry)
        {
                qry = new TOCIQuery (DB_LINK);
                if (NULL == qry)
                {
                        THROW (10);
                }
                char sSQL[2048];
                sprintf (sSQL, "SELECT SERV_ID FROM %s", m_sTableServ);
                qry->setSQL (sSQL);
                qry->open ();
        }

        if (!qry->next ())
        {
                qry->close ();
                delete qry;
                qry = NULL;
                return false;
        }
        lServID = qry->field (0).asLong ();
        m_iCount ++;
        return true;
}
bool QAcctItem::getServAcctItemAll(long lServID, vector<AcctItemData> &v_data)
{
        DEFINE_QUERY (qry);
        char sSQL[2048];
        #ifdef ORG_BILLINGCYCLE
        sprintf (sSQL, "SELECT acct_item_id,billing_cycle_id,acct_item_type_id,"
        "charge,acct_id,item_source_id,measure_type,offer_inst_id,state_date,"
             "item_class_id, cor_billing_cycle_id "
             " FROM %s where serv_id=%ld", m_sTableFee,lServID);
    #else
    sprintf (sSQL, "SELECT acct_item_id,billing_cycle_id,acct_item_type_id,"
             "charge,acct_id,item_source_id,measure_type,offer_inst_id,state_date,"
        "item_class_id"
        " FROM %s where serv_id=%ld", m_sTableFee,lServID);
    #endif
        qry.setSQL (sSQL);
        qry.open ();
        while (qry.next ())
        {
                AcctItemData itemData;
                //成员变量赋值
                itemData.m_lServID = lServID;
                itemData.m_lAcctItemID = qry.field (0).asLong ();
                itemData.m_iBillingCycleID = qry.field (1).asInteger ();
                itemData.m_iPayItemTypeID = qry.field (2).asInteger ();
                itemData.m_lValue = qry.field (3).asLong ();
                itemData.m_lAcctID = qry.field (4).asLong ();
                itemData.m_iItemSourceID = qry.field (5).asInteger ();
                itemData.m_iPayMeasure = qry.field (6).asInteger ();
                itemData.m_lOfferInstID = qry.field (7).asLong ();
                strcpy(itemData.m_sStateDate,qry.field (8).asString());
                itemData.m_iItemClassID = qry.field (9).asInteger ();
                #ifdef ORG_BILLINGCYCLE
        itemData.m_iOrgBillingCycleID = qry.field(10).asInteger();
        #endif
                v_data.push_back(itemData);
        }
        qry.close ();
        return true;
}

int QAcctItem::updateServFee()
{
        int i = 0,iCycleID = 0;
        bool bRet(false);
        AcctItemData itemData;
        AcctItemData * m_poItemData;
        memset (&itemData, 0, sizeof (itemData));
        vector<AcctItemData> v_data;
        vector<AcctItemData *> v_ServAcctItemAggr;

        while(getServ(m_lServID))
        {
            #ifdef SHM_FILE_USERINFO
            G_SHMFILE;
            #endif
                //不同帐期分别处理
                if(getServAcctItemAll(m_lServID,v_data))
                {
                        vector<AcctItemData>::iterator iter_data = v_data.begin();
                        vector<AcctItemData *>::iterator iter;
                        while(iter_data != v_data.end())
                        {
                                itemData = (AcctItemData)(*iter_data);
                                if(iCycleID == 0)
                                {
                                        v_ServAcctItemAggr.clear();
                                        iCycleID = itemData.m_iBillingCycleID;
                                        getAllItemAggr(m_lServID,itemData.m_iBillingCycleID,v_ServAcctItemAggr);
                                }
                                if(iCycleID != itemData.m_iBillingCycleID)
                                {
                                        v_ServAcctItemAggr.clear();
                                        getAllItemAggr(m_lServID,itemData.m_iBillingCycleID,v_ServAcctItemAggr);
                                }
                                iter=v_ServAcctItemAggr.begin();
                                while(iter != v_ServAcctItemAggr.end())
                                {
                                        m_poItemData=(AcctItemData *)v_ServAcctItemAggr[i];
                                        if(m_poItemData->m_lAcctItemID == itemData.m_lAcctItemID)
                                        {
                                                m_poItemData->m_lValue=0;
                                                break;
                                        }
                                        iter++;
                                        i++;
                                }
                                update(itemData);
                                i = 0;
                                iter_data++;
                        }
                        iCycleID = 0;
                }
                v_data.clear();
        }
        return 0;
}

int QAcctItem::run()
{
        if(!prepare())
        {
                return 1;
        }
        if(m_lServID == 0)
        {
                updateServFee();
                Log::log (0, "成功处理用户数:  [%d ]",  this->m_iCount);
                Log::log (0, "成功更新用户帐目条数:  [%d ]",  this->m_iCount2); 
                Log::log (0, "成功新增用户帐目条数:  [%d ]",  this->m_iCount3); 
        }else
        {
                queryServFee();
        }
        return 0;
}

QAcctItem::QAcctItem()
{
        Environment::useUserInfo(USER_INFO_BASE);
        m_iCount = 0;
        m_iCount2 = 0;
        m_lServID = 0;
}

void QAcctItem::printUsage()
{
        Log::log (0,    "\n*******************************************************************************\n"
                    "查看单个用户或更新用户内存总帐：\n"
                    "调用模式:\n"
                                        "       qacctitem SERV_ID [BILLING_CYCLE_ID] 查询用户内存总帐\n"
                    "   qacctitem -u 表名1 表名2 从表名1中取出SERV_ID到表名2中找出要更新的总帐\n"
                    "*********************************************************************************\n"
              );
}
