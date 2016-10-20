/*VER: 2*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.
#include <string.h>
#include "Environment.h"
#include "BalanceMgr.h"
#include "IpcKey.h" //add by yks
#include "ParamDefineMgr.h"

SHMData<BalanceData> * BalanceMgr::m_poBalanceData = 0;
SHMIntHashIndex * BalanceMgr::m_poBalanceAcctIndex = 0;
SHMIntHashIndex * BalanceMgr::m_poBalanceServIndex = 0;

bool BalanceMgr::m_bUploaded = false;

static long getIpcKey(int i_iBillFlowID,char * i_sIpcName)
{

    //add by yangks 2007.1.15
    return IpcKeyMgr::getIpcKey (i_iBillFlowID, i_sIpcName);

	char sSql[1000];
	DEFINE_QUERY(QueryKey);
	
	sprintf(sSql,"SELECT IPC_KEY FROM B_IPC_CFG WHERE BILLFLOW_ID=%d AND IPC_NAME='%s' ",i_iBillFlowID,i_sIpcName);
	QueryKey.close();
	QueryKey.setSQL(sSql);
	QueryKey.open();
	QueryKey.next();
	return QueryKey.field("IPC_KEY").asInteger();
}

BalanceMgr::BalanceMgr()
{
	if (m_bUploaded) return;

	m_poBalanceData = new SHMData<BalanceData> (IpcKeyMgr::getIpcKey(-1,"SHM_BalanceBuf"));
	m_poBalanceAcctIndex = new SHMIntHashIndex (IpcKeyMgr::getIpcKey(-1,"SHM_BalanceServBuf"));
	m_poBalanceServIndex = new SHMIntHashIndex (IpcKeyMgr::getIpcKey(-1,"SHM_BalanceAcctBuf"));

	if (!m_poBalanceData->exist () || !m_poBalanceAcctIndex->exist () || !m_poBalanceServIndex->exist ()) {
		m_bUploaded = false;
		load ();
	} else {
		m_bUploaded = true;
	}
}

BalanceMgr::~BalanceMgr()
{
}

void BalanceMgr::unload()
{
	if (m_poBalanceData->exist()) m_poBalanceData->remove ();
	if (m_poBalanceServIndex->exist ()) m_poBalanceServIndex->remove ();
	if (m_poBalanceAcctIndex->exist ()) m_poBalanceAcctIndex->remove ();
}

void BalanceMgr::load()
{
	DEFINE_QUERY (qry);
	int iCount, iTemp;
	BalanceData * pData;
	unsigned int k;
  char	 sSql[2000],sDate[16];
  Date d;
  char sBalanceType[1000];
  
  if (!ParamDefineMgr::getParam ("RT_MAKE_STOP","NO_BALANCE_TYPE", sBalanceType) )
        strcpy(sBalanceType,"0");
        
	if (m_poBalanceData->exist ()) m_poBalanceData->remove ();
	if (m_poBalanceServIndex->exist ()) m_poBalanceServIndex->remove ();
	if (m_poBalanceAcctIndex->exist ()) m_poBalanceAcctIndex->remove ();

  strncpy(sDate,d.toString(),8);
  sDate[8]=0;
  
/*  
	qry.setSQL ("select count(*) from acct_balance");
	qry.open (); qry.next ();
	iCount = qry.field (0).asInteger () + 1000;
	qry.close ();

	m_poBalanceData->create (iCount);
	m_poBalanceServIndex->create (iCount);
	m_poBalanceAcctIndex->create (iCount);
*/
//这里用配置参数

	iCount = BALANCEBUF_DATA_COUNT;
	
	m_poBalanceData->create (iCount);
	m_poBalanceServIndex->create (iCount);
	m_poBalanceAcctIndex->create (iCount);

	pData = *m_poBalanceData;

	sprintf (sSql,"SELECT ACCT_ID,NVL(SERV_ID,-1) SERV_ID,NVL(ITEM_GROUP_ID,-1) ACCT_ITEM_TYPE_ID,"
	" nvl(sum(nvl(decode(cycle_upper,Null,balance,decode(sign(cycle_upper-balance),-1,cycle_upper,balance)),0) ),0) BALANCE "
							" FROM ACCT_BALANCE "
							" WHERE to_char(sysdate,'yyyymmdd') BETWEEN to_char(nvl(eff_date,to_date('20000101','yyyymmddhh24miss')),'yyyymmdd') and to_char(nvl(exp_date,to_date('20600101','yyyymmdd')),'yyyymmdd') "
							" AND STATE='10A' "
							" AND balance_type_id NOT IN (%s) "
							" GROUP BY ACCT_ID,NVL(SERV_ID,-1),"
							" NVL(ITEM_GROUP_ID,-1) ",sBalanceType);

	qry.setSQL (sSql);
	
	qry.open();
	
	while (qry.next ()) {
		iTemp = m_poBalanceData->malloc ();
		
		if (!iTemp) THROW (MBC_LoadBalance+1);

		pData[iTemp].lAcctID = qry.field (0).asLong ();
		pData[iTemp].lServID = qry.field (1).asLong ();
		pData[iTemp].iAcctItemTypeID = qry.field (2).asInteger ();
		pData[iTemp].iBalance = qry.field (3).asInteger ();
        pData[iTemp].iNextAcctOffset = 0;
        pData[iTemp].iNextServOffset = 0;


		if (m_poBalanceAcctIndex->get (pData[iTemp].lAcctID, &k)) {
			pData[iTemp].iNextAcctOffset = pData[k].iNextAcctOffset;
			pData[k].iNextAcctOffset  = iTemp;
		} else {
			m_poBalanceAcctIndex->add (pData[iTemp].lAcctID, iTemp);
		}

		if (m_poBalanceServIndex->get (pData[iTemp].lServID, &k)) {
			pData[iTemp].iNextServOffset = pData[k].iNextServOffset;
			pData[k].iNextServOffset  = iTemp;
		} else {
			m_poBalanceServIndex->add (pData[iTemp].lServID, iTemp);
		}
	
	  if (iTemp%50000 == 0)
	  	 cout << "已加载条数:" << iTemp << endl;
	}

	if (iTemp%50000 != 0)
	   cout << "已加载条数:" << iTemp << endl;

	cout << "余额加载完毕!" << endl;
	qry.close ();
	printBalanceMemory();
}


int BalanceMgr::getServBalance(long lServID)
{
	if (!m_bUploaded) THROW (MBC_LoadBalance+2);

	BalanceData * pData =  *m_poBalanceData;
	unsigned int k;
	int iRet = 0;

	if (!m_poBalanceServIndex->get (lServID, &k)) return 0;
	
	while (k) {
		iRet += pData[k].iBalance;
		k = pData[k].iNextServOffset;
	} 

	return iRet;
}

int BalanceMgr::getAcctBalance(long lAcctID)
{
	if (!m_bUploaded) THROW  (MBC_LoadBalance+2);
	
	BalanceData * pData =  *m_poBalanceData;
	unsigned int k;
	int iRet = 0;
	
	if (!m_poBalanceAcctIndex->get (lAcctID, &k)) return 0;
	
	while (k)
	{
		iRet += pData[k].iBalance;
		k = pData[k].iNextAcctOffset;
	}
	
	return iRet;
}

long BalanceMgr::GetAcctUServBalance(long lServID,long lAcctID)
{
	if (!m_bUploaded) THROW  (MBC_LoadBalance+2);
	
	BalanceData * pData =  *m_poBalanceData;
	unsigned int k;
	long lRet = 0;
	
	if (!m_poBalanceAcctIndex->get (lAcctID, &k)) return 0;
	
	while (k)
	{
		if (pData[k].lServID==-1 && pData[k].iAcctItemTypeID==-1)
		{
			lRet += pData[k].iBalance;
		}
		else {
			if (pData[k].lServID==lServID)
				lRet += pData[k].iBalance;
		}
		
		k = pData[k].iNextAcctOffset;
	}
	
	return lRet;
}

//acct_balance表中内容刷新内存，由定时程序调用
int BalanceMgr::UpdateBalanceMemory()
{
    DEFINE_QUERY (qry);
    int iCount, iTemp, iNewFlag;
    BalanceData * pData;
    BalanceData strData;
    unsigned int k;
    char sSql[2000],sDate[16];
    Date d;

    strncpy(sDate,d.toString(),8);
    sDate[8]=0;

    pData = *m_poBalanceData;
    char sBalanceType[1000];
  
    if (!ParamDefineMgr::getParam ("RT_MAKE_STOP","NO_BALANCE_TYPE", sBalanceType) )
     strcpy(sBalanceType,"0");
        
//加入取10X的条件
/*
    sprintf (sSql,"SELECT ACCT_ID,NVL(SERV_ID,-1) SERV_ID,NVL(ITEM_GROUP_ID,-1) ACCT_ITEM_TYPE_ID,"
    " nvl(sum(nvl(decode(cycle_upper,Null,balance,decode(sign(cycle_upper-balance),-1,cycle_upper,balance)),0) ),0) BALANCE " 
        " FROM ACCT_BALANCE "
        " WHERE to_char(sysdate,'yyyymmdd') BETWEEN to_char(nvl(eff_date,to_date('20000101','yyyymmddhh24miss')),'yyyymmdd') and to_char(nvl(exp_date,to_date('20600101','yyyymmdd')),'yyyymmdd') "
        " AND (STATE='10A' or ( state='10X' and trunc(state_date)>=trunc(sysdate-30) ) ) "
        " AND BALANCE_TYPE_ID NOT IN (%s) "
        " GROUP BY ACCT_ID,NVL(SERV_ID,-1),"
        " NVL(ITEM_GROUP_ID,-1) ",sBalanceType);
*/
//这里增加自然失效的记录 2010.12 yangzhou
sprintf (sSql," select acct_id,serv_id,ACCT_ITEM_TYPE_ID,nvl(sum(BALANCE),0) BALANCE from "
              " ( "
              " SELECT ACCT_ID,NVL(SERV_ID,-1) SERV_ID,NVL(ITEM_GROUP_ID,-1) ACCT_ITEM_TYPE_ID, "
              " nvl(decode(cycle_upper,Null,balance,decode(sign(cycle_upper-balance),-1,cycle_upper,balance)),0) BALANCE "
              " FROM ACCT_BALANCE "
              " WHERE to_char(sysdate,'yyyymmdd') BETWEEN to_char(nvl(eff_date,to_date('20000101','yyyymmdd')),'yyyymmdd') " 
              " and to_char(nvl(exp_date,to_date('20600101','yyyymmdd')),'yyyymmdd') "
              " AND STATE='10A' AND BALANCE_TYPE_ID NOT IN (%s) "
              " union all "
              " SELECT ACCT_ID,NVL(SERV_ID,-1) SERV_ID,NVL(ITEM_GROUP_ID,-1) ACCT_ITEM_TYPE_ID,0 BALANCE "
              " FROM ACCT_BALANCE "
              " WHERE ((state='10X' and trunc(state_date) between trunc(sysdate-30) and trunc(sysdate)) "
              " or (exp_date is not null and trunc(exp_date) between trunc(sysdate-30) and trunc(sysdate))) "
              " AND BALANCE_TYPE_ID NOT IN (%s) "
              " ) "
              " group by acct_id,serv_id,ACCT_ITEM_TYPE_ID ", sBalanceType, sBalanceType);

    qry.setSQL (sSql);

    qry.open();

    while (qry.next ()) {
        iNewFlag = 0;
        strData.lAcctID = qry.field (0).asLong ();
        strData.lServID = qry.field (1).asLong ();
        strData.iAcctItemTypeID = qry.field (2).asInteger ();
        strData.iBalance = qry.field (3).asInteger ();
        strData.iNextAcctOffset = 0;
        strData.iNextServOffset = 0;

        //get acct
        if (m_poBalanceAcctIndex->get (strData.lAcctID, &k))
        {
            //serv_id,acct_item_type_id
            iTemp = k;
            do 
            {
                if ( ( pData[iTemp].lServID == strData.lServID )
                && ( pData[iTemp].iAcctItemTypeID == strData.iAcctItemTypeID ) )
                {
                    if (pData[iTemp].iBalance != strData.iBalance)
                        pData[iTemp].iBalance = strData.iBalance;
                    iNewFlag = 1;
                    break;
                }
                else
                {

                }
                iTemp = pData[iTemp].iNextAcctOffset;
            }while(iTemp);
        }
        //get new data
        if (iNewFlag == 0  && strData.iBalance != 0 )
        {
            iTemp = m_poBalanceData->malloc ();
            if (!iTemp) THROW (MBC_LoadBalance+1);

            pData[iTemp].lAcctID = strData.lAcctID;
            pData[iTemp].lServID = strData.lServID;
            pData[iTemp].iAcctItemTypeID = strData.iAcctItemTypeID;
            pData[iTemp].iBalance = strData.iBalance;

            if (m_poBalanceAcctIndex->get (pData[iTemp].lAcctID, &k)) 
            {
    	        pData[iTemp].iNextAcctOffset = pData[k].iNextAcctOffset;
                pData[k].iNextAcctOffset  = iTemp;
            }
            else 
            {
                m_poBalanceAcctIndex->add (pData[iTemp].lAcctID, iTemp);
            }

            if (m_poBalanceServIndex->get (pData[iTemp].lServID, &k)) 
            {
                pData[iTemp].iNextServOffset = pData[k].iNextServOffset;
                pData[k].iNextServOffset  = iTemp;
            }
            else
            {
                m_poBalanceServIndex->add (pData[iTemp].lServID, iTemp);
            }

        }
    }
		printBalanceMemory();
    return 0;
}


int BalanceMgr::getAcctBalanceV(long lAcctID,vector<struct BalanceData> & vData)
{
	if (!m_bUploaded) THROW  (MBC_LoadBalance+2);
	
	BalanceData * pData =  *m_poBalanceData;
	unsigned int k;
	
	if (!m_poBalanceAcctIndex->get (lAcctID, &k)) return 0;
	
	while (k)
	{
		if ( pData[k].iBalance != 0 && pData[k].iAcctItemTypeID == -1 )
			vData.push_back(pData[k]);
		k = pData[k].iNextAcctOffset;
	}
	
	return 0;
}

void BalanceMgr::printBalanceMemory()
{
	unsigned int iTemp,temp;
	
	iTemp = m_poBalanceData->getTotal();
	temp = m_poBalanceData->getCount();
	cout<<"数据区总数："<<iTemp<<"  已使用："<<temp<<endl;
	
	iTemp = m_poBalanceServIndex->getTotal();
	temp = m_poBalanceServIndex->getCount();
	cout<<"索引区A总数："<<iTemp<<"  已使用："<<temp<<endl;
	
	iTemp = m_poBalanceAcctIndex->getTotal();
	temp = m_poBalanceAcctIndex->getCount();
	cout<<"索引区B总数："<<iTemp<<"  已使用："<<temp<<endl;

}
