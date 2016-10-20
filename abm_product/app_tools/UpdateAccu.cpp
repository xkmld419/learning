/*VER: 1*/ 
#include <iostream>
#include <string.h>
#include <vector>
#include "Environment.h"
#include "Process.h"
#include "Date.h"
#include "Log.h"
#include "OfferAccuMgr.h"

#ifdef DEF_AIX
    #ifdef v_data
    #undef v_data
    #endif
#endif

class UpdateAccu
{
public:
	int run();

  public:
	UpdateAccu();
	~UpdateAccu();

private:
	bool getInstID(long &lInstID);
	bool getAccuByInst(long lInstID, vector<AccuData> &v_data);
	bool prepare();
	void printUsage();
	int updateInstAccu();
private:
	int m_iCount;
	int m_iCount1;
	int m_iCount2;
	int m_iCount3;	
	char *m_sTableInstID;
	char *m_sTableAccu;
	int iBillingCycleID;
private:
	OfferAccuMgr * m_poAccuMgr;
};

int main(int argc, char *argv[])
{
	g_argc = argc;
	g_argv = argv;

	UpdateAccu x;
	return x.run ();
}

bool UpdateAccu::prepare()
{
	int iTemp, jTemp;
	iTemp = 1;
	jTemp = g_argc;
	if(jTemp == 1)
	{
        printUsage();
        return false;
	}
	bool bU(false);
	if (g_argv[iTemp][0] != '-')
	{
		printUsage();
		return false;
	}
	switch (g_argv[iTemp][1] | 0x20) 
	{
       case 'u':
	      if(jTemp == 4)
		  {
			  bU = true;
			  m_sTableInstID = g_argv[iTemp+1];
			  m_sTableAccu = g_argv[iTemp+2];
			  break; 
		  }
	   default:
		 printUsage ();
		 return false;
		 break;
	}
    if (!bU)
    {
        printUsage();
        return false;
    }
	return true;
}

bool UpdateAccu::getInstID(long &lInstID)
{
	static TOCIQuery * qry = NULL;

	if (NULL == qry)
	{
		qry = new TOCIQuery (DB_LINK);
		if (NULL == qry)
		{
			THROW (10);
		}
		char sSQL[248];
		memset(sSQL,0,sizeof(sSQL));
		sprintf (sSQL, "SELECT OFFER_INST_ID FROM %s", m_sTableInstID);
		qry->setSQL (sSQL);
		qry->open ();
	}

	if (!qry->next ())
	{
		qry->commit();
		qry->close ();
		delete qry;
		qry = NULL;
		return false;
	}
	lInstID = qry->field (0).asLong ();
	m_iCount ++;
	return true;
}

bool UpdateAccu::getAccuByInst(long lInstID, vector<AccuData> &v_data)
{
	bool bRet(false);
	DEFINE_QUERY (qry);
	char sSQL[2048];
#ifndef SPLIT_ACCU_BY_BILLING_CYCLE
	sprintf (sSQL, "SELECT event_aggr_id,acc_type_id,cycle_inst_id,value,"
		"inst_member_id,serv_id,split_by"
		" from %s where offer_inst_id=%ld", m_sTableAccu,lInstID);
#else
    sprintf (sSQL, "SELECT event_aggr_id,acc_type_id,cycle_inst_id,value,"
        "inst_member_id,serv_id,split_by,billing_cycle_id"
        " from %s where offer_inst_id=%ld", m_sTableAccu,lInstID);
#endif
	qry.setSQL (sSQL);
	qry.open ();
	while (qry.next ())
	{
		AccuData accuData;
		memset(&accuData,0,sizeof(accuData));
		//成员变量赋值
		accuData.m_lOfferInstID = lInstID;
		accuData.m_lAccuInstID = qry.field (0).asLong ();
		accuData.m_iAccuTypeID = qry.field (1).asInteger ();
		accuData.m_lCycleInstID = qry.field (2).asLong ();
		accuData.m_lValue = qry.field (3).asLong ();
		accuData.m_lMemberID = qry.field (4).asLong ();
		accuData.m_lServID = qry.field (5).asLong ();
        strcpy(accuData.m_sSplitby,qry.field (6).asString());
#ifdef SPLIT_ACCU_BY_BILLING_CYCLE
		accuData.m_iBillingCycleID = qry.field (7).asInteger();
#endif

		v_data.push_back(accuData);
		bRet = true;
	}
	qry.commit();
	qry.close ();
	return bRet;
}

int UpdateAccu::updateInstAccu()
{
	int i = 0;
	bool bRet(false);
	long lInstID,lAccuValue;
	AccuData accuData;
	memset (&accuData, 0, sizeof (accuData));
	vector<AccuData> v_data;

	while(getInstID(lInstID))
	{
		if(getAccuByInst(lInstID,v_data))
		{
			vector<AccuData>::iterator iter_data = v_data.begin();
			while(iter_data != v_data.end())
			{
				accuData = (AccuData)(*iter_data);
				lAccuValue = m_poAccuMgr->getAccByInst(accuData.m_lOfferInstID,accuData.m_iAccuTypeID,
					accuData.m_lCycleInstID,accuData.m_lServID,accuData.m_lMemberID,accuData.m_sSplitby);
				accuData.m_lValue -= lAccuValue;

				//lAccuValue为主健,bRet为新增或修改，暂时不用，可以为入库做预留
#ifndef SPLIT_ACCU_BY_BILLING_CYCLE
            	lAccuValue = m_poAccuMgr->updateAccByInst(accuData.m_lOfferInstID,accuData.m_iAccuTypeID,
					accuData.m_lCycleInstID,accuData.m_lServID,accuData.m_lMemberID,accuData.m_sSplitby,
					accuData.m_lValue,&bRet);
#else
                lAccuValue = m_poAccuMgr->updateAccByInst(accuData.m_lOfferInstID,accuData.m_iAccuTypeID,
                    accuData.m_lCycleInstID,accuData.m_lServID,accuData.m_lMemberID,accuData.m_sSplitby,
                    accuData.m_lValue,accuData.m_iBillingCycleID,&bRet);
#endif
				
if(bRet)
				{
					m_iCount1++;
					m_iCount3++;
				}else
				{
					m_iCount1++;
					m_iCount2++;
				}
				iter_data++;
			}
		}
		v_data.clear();
	}
	return 0;
}

int UpdateAccu::run()
{
	if(!prepare())
	{
		return 1;
	}
	updateInstAccu();
	Log::log (0, "成功处理商品实例数:  [%d ]",  this->m_iCount);
	Log::log (0, "成功处理记录总条数:  [%d ]",  this->m_iCount1);
	Log::log (0, "成功更新累积量条数:  [%d ]",  this->m_iCount2);	
	Log::log (0, "成功新增累积量条数:  [%d ]",  this->m_iCount3);	
	return 0;
}

UpdateAccu::UpdateAccu()
{
	//Environment::useUserInfo ();
	m_poAccuMgr = new OfferAccuMgr(true);
	m_iCount = 0;
	m_iCount1 = 0;
	m_iCount2 = 0;
	m_iCount3 = 0;
}

UpdateAccu::~UpdateAccu()
{
	delete m_poAccuMgr;
}

void UpdateAccu::printUsage()
{
	Log::log (0,    "\n***********************************************************************************\n"
                    "更新单个或批量商品实例的累积量：\n"
                    "调用模式:\n"
                    "	updateaccu -u 表名1 表名2 从表名1中取出OFFER_INST_ID到表名2中找出要更新的累积量信息\n"
                    "**************************************************************************************\n"
              );
}
