#include "GetAcctAggrForYear.h"
#include "strcasecmp.h"
#include "Environment.h"



GetAcctAggrYear::GetAcctAggrYear()
{
	m_iBillingCycleId = 0;
	m_iOfferCount = 0;
	m_iCount = 0;
	memset(m_sTableName,0,sizeof(m_sTableName));
	memset(m_sCycleBeginDate,0,sizeof(m_sCycleBeginDate));
	memset(m_sCycleEndDate,0,sizeof(m_sCycleEndDate));
	
	m_vOfferId.clear();
	m_vOfferInstId.clear();
	m_vAcctItem.clear();
	m_pOfferRule = new HashList<OfferData* >(1024);
    m_pServId = new HashList<long>(10000);
}

GetAcctAggrYear::~GetAcctAggrYear()
{
	m_vOfferId.clear();
    m_vOfferInstId.clear();
	m_vAcctItem.clear();
    
    if(m_pOfferRule)
    {
	    delete m_pOfferRule;
        m_pOfferRule = NULL;
    }
    if(m_pServId)
    {
        delete m_pServId;
        m_pServId = NULL;
    }
}

/*
 *	函 数 名 : GetOfferId
 *	函数功能 : 提取配置表中的offer_id,并解析成员类别及归并标识
 *	时    间 : 2010年1月7日
 *	返 回 值 : 
 *	参数说明 : MEMBER_TYPE:		10A-用户 10J-账户 10C-商品实例 10I-客户
 *			 : BILLING_MERGER:	0-不归并 1-归并
*/
bool GetAcctAggrYear::GetOfferId()
{
	int count=0,temp=0;
	int offerId=0;
	char sql[1024];

	OfferData *pTemData = 0;
	HashList<int> *phlOfferId = new HashList<int>(1024);

	memset(sql,0,sizeof(sql));
	DEFINE_QUERY (qry);
	sprintf(sql,"SELECT OFFER_ID,NVL(MEMBER_TYPE,''),EFF_DATE,EXP_DATE,BILLING_MERGER FROM B_CONSUME_YEAR_CHECK_OFFER ORDER BY MEMBER_TYPE DESC");
	qry.setSQL(sql); 
	qry.open();
	while(qry.next())
	{
        OfferData *pData = new OfferData();
		offerId = qry.field(0).asInteger();
		pData->m_iOfferId = offerId;
		strcpy(pData->m_sMemberType,qry.field(1).asString());
		strcpy(pData->EffDate,qry.field(2).asString());
		strcpy(pData->ExpDate,qry.field(3).asString());
		pData->m_iBillMerger= qry.field(4).asInteger();
		pData->bMerger=false;
		pData->bCust = false;
		pData->bOfferIns = false;
		pData->bServ = false;
		pData->bAcct = false;
		pData->bAll = false;
		if(strlen(pData->m_sMemberType)==0)
		{//成员标识为空,sql按MemberType降序,会先将空成员标识找出来
			phlOfferId->add(offerId,0);
			if(pData->m_iBillMerger==1)
			{//如果成员标识为空,归并字段为1，则程序对帐期进行归并
				pData->bMerger=true;
			}
			pData->bAll = true;
			m_pOfferRule->add(offerId,pData);
			m_vOfferId.push_back(offerId);
		}
		else
		{//成员标识非空
			if(phlOfferId->get(offerId,&temp))
			{//此处为同个OFFERID多个记录
				if(temp==0)
				{//空与非空并存，按空逻辑处理，非空的数据就不在重复处理了
				    if(pData->m_iBillMerger==1)
				    {//空与非空并存，且出现非空成员标识带有归并标识
				     //按空逻辑此处也该归并帐期
                        if(m_pOfferRule->get(offerId,&pTemData))
                            pTemData->bMerger = true;    
				    }
				}
				else
				{//非空的其他成员，整合处理
					if(m_pOfferRule->get(offerId,&pTemData))
					{
						if(strcmp(pData->m_sMemberType,"10A")==0)
							pTemData->bServ = true;
						else if(strcmp(pData->m_sMemberType,"10C")==0)
							pTemData->bOfferIns = true;
						else if(strcmp(pData->m_sMemberType,"10I")==0)
							pTemData->bCust = true;
						else if(strcmp(pData->m_sMemberType,"10J")==0)
							pTemData->bAcct = true;
						else
						{
							Log::log (0, "获取到异常的成员类别，请检查B_CONSUME_YEAR_CHECK_OFFER表");
							return false;
						}			
					}
				}
			}
			else
			{//记录首个成员标识非空的数据
                if(strcmp(pData->m_sMemberType,"10A")==0)
				    pData->bServ = true;
				else if(strcmp(pData->m_sMemberType,"10C")==0)
					pData->bOfferIns = true;
				else if(strcmp(pData->m_sMemberType,"10I")==0)
					pData->bCust = true;
				else if(strcmp(pData->m_sMemberType,"10J")==0)
					pData->bAcct = true;
				else
				{
				    Log::log (0, "获取到异常的成员类别，请检查B_CONSUME_YEAR_CHECK_OFFER表");
					return false;
				}		
				phlOfferId->add(offerId,1);
				m_pOfferRule->add(offerId,pData);
				m_vOfferId.push_back(offerId);
			}
		}
		count++;
	}
	qry.close();

	m_iOfferCount = count;

	return true;
}

/*
 *	函 数 名 : GetInstInfoById
 *	函数功能 : 根据获取的OFFERID最终获取到实例资料
 *	时    间 : 2010年1月13日
 *	返 回 值 : int 0:成功，1:失败
 *	参数说明 : 
*/
int GetAcctAggrYear::GetInstInfoById()
{
	vector<int>::iterator iter;
	vector<long>::iterator lter;

	for(iter=m_vOfferId.begin();iter!=m_vOfferId.end();iter++)
	{
		GetInstIdFromDb(*iter);
	}
	for(lter=m_vOfferInstId.begin();lter!=m_vOfferInstId.end();lter++)
	{//遍历所有的实例
		GetServInfo(*lter,0);			
	}
	return 0;
}

/*
 *	函 数 名 : GetInstIdFromDb
 *	函数功能 : 根据offerid查询所有的实例ID
 *	时    间 : 2010年1月13日
 *	返 回 值 : int 0:成功，1:失败
 *	参数说明 : 
*/
int GetAcctAggrYear::GetInstIdFromDb(int OfferId)
{
	char sql[1024];
	long InstId=0;

	memset(sql,0,sizeof(sql));
	DEFINE_QUERY (qry);
	sprintf(sql,"select PRODUCT_OFFER_INSTANCE_ID from PRODUCT_OFFER_INSTANCE where "
				"PRODUCT_OFFER_ID=%d AND EXP_DATE >=TO_DATE(%s,'yyyymmddhh24miss') ORDER BY PRODUCT_OFFER_INSTANCE_ID",OfferId,m_sCycleBeginDate);
	qry.setSQL(sql); 
	qry.open();
	while(qry.next())
	{
		InstId = qry.field(0).asLong();
		m_vOfferInstId.push_back(InstId);
	}
	qry.close();
	
	return 0;
}

/*
 *	函 数 名 : GetServInfo
 *	函数功能 : 获取所有成员的用户信息
 *	时    间 : 2010年1月13日
 *	返 回 值 : int 0:成功，1:失败
 *	参数说明 : Flag=0:正常提取,Flag=1:递归提取
*/
int GetAcctAggrYear::GetServInfo(long OfferInstId,int Flag)
{
	static vector<long> vServID;
	vector<long>::iterator iter;
	static vector<ServAcctInfo> vPayInfo;

	ProdOfferIns *pIns = G_PUSERINFO->getOfferIns(OfferInstId);
	if(!pIns)
	{
		Log::log (0, "获取%ld实例资料失败",OfferInstId);
		return 1;
	}
	//找的商品实例
	OfferData *pTemData = 0;
	if(!m_pOfferRule->get(pIns->m_iOfferID,&pTemData) && Flag==0)
	{
		Log::log (0, "配置表里查找不到该offerid",pIns->m_iOfferID);
		return 1;
	}
	unsigned int k = pIns->m_iOfferDetailInsOffset;
	for(;k;k=m_poOfferDetailIns[k].m_iNextDetailOffset)
	{//获取商品实例明细
		if(strncmp(m_poOfferDetailIns[k].m_sExpDate,m_sCycleBeginDate,16)<0) 
		{
			continue;
		}
		switch (m_poOfferDetailIns[k].m_iInsType)
		{
			case SERV_INSTANCE_TYPE:
			{
				if(Flag==1 || pTemData->bAll==true || pTemData->bServ==true)
					GetAllAcctAggr(m_poOfferDetailIns[k].m_lInsID,
						 				m_iBillingCycleId,m_vAcctItem);
			}
				break;
			case CUST_TYPE:
			{
				if(Flag==1 || pTemData->bAll==true || pTemData->bCust==true)
				{
					vServID.clear();							
					G_PUSERINFO->getCustServ(m_poOfferDetailIns[k].m_lInsID,
												vServID);
					for(iter=vServID.begin();iter!=vServID.end();iter++)
					{
						GetAllAcctAggr(*iter,m_iBillingCycleId,m_vAcctItem);	
					}
				}
			}
            	break;
            case ACCT_TYPE:
			{
				int size=0,p=0;
				if(Flag==1 || pTemData->bAll==true || pTemData->bAcct==true)
				{
					vPayInfo.clear();							
					G_PUSERINFO->getPayInfo(vPayInfo,m_poOfferDetailIns[k].m_lInsID);
					size = vPayInfo.size();
					for (p=0;p<size;p++)
					{
						GetAllAcctAggr(vPayInfo[p].m_lServID,
										m_iBillingCycleId,m_vAcctItem);		
					}	
				}  
			}
                 break;
			case OFFER_INSTANCE_TYPE:
			{
				if(Flag==1 || pTemData->bAll==true || pTemData->bOfferIns==true)
					GetServInfo(m_poOfferDetailIns[k].m_lInsID,1);			
			}
            	break;
			default:
			{
				Log::log (0, "%ld有异常的成员类型",OfferInstId);
                break;
			}
		}     
	}
	
	return 0;
}

/*
 *	函 数 名 : GetBillingCycleTime
 *	函数功能 : 获取输入帐期的开始时间和结束时间
 *	时    间 : 2010年1月11日
 *	返 回 值 : 
 *	参数说明 : 
*/
bool GetAcctAggrYear::GetBillingCycleTime()
{
	BillingCycleMgr* pbcm = new BillingCycleMgr();

	if(0 == pbcm)
	{
		THROW(MBC_BillingCycleMgr+1);
	}

	BillingCycle* pBillingCycle= pbcm->getBillingCycle(m_iBillingCycleId);
	if(0 ==pBillingCycle)
	{
		Log::log(0,"查找BILLING_CYCLE_ID失败:%d",m_iBillingCycleId);
		THROW(MBC_BillingCycle);
	}

	Date d(pBillingCycle->getStartDate());
//	d.addSec(1);
	strcpy(m_sCycleBeginDate,d.toString());
	d.parse(pBillingCycle->getEndDate());
//	d.addSec(-1);
	strcpy(m_sCycleEndDate,d.toString());

	delete pbcm; 
	pbcm = 0;
	return true;
}

/*
 *	函 数 名 : CheckUserOper
 *	函数功能 : 检查程序操作日志表，查看是否已经提取过，并告警
 *	时    间 : 2010年1月7日
 *	返 回 值 : bool:false-已经提交过,true－没有提交过
 *	参数说明 : 如果数据库里有该帐期ID操作记录,说明已经提取过返回false
*/
bool GetAcctAggrYear::CheckUserOper()
{
	char sql[1024];
	int count=0;

    memset(sql,0,sizeof(sql));
    DEFINE_QUERY (qry);
    sprintf(sql,"select count(*) from B_CONSUME_YEAR_OPER_LOG where BILLING_CYCLE_ID=%d",m_iBillingCycleId);
    qry.setSQL(sql); 
	qry.open();
	if(qry.next())
		count = qry.field(0).asInteger();
	qry.close();
	if(count!=0)
		return false;
	else
		return true;
}

/*
 *	函 数 名 : CheckBillingData
 *	函数功能 : 检查内存中输入帐期是否有数据，无则报警退出
 *	时    间 : 2010年1月7日
 *	返 回 值 : bool:false-失败,true－成功
 *	参数说明 : 
*/
bool GetAcctAggrYear::CheckBillingData()
{
	unsigned int count=0;
	int i;

	count = m_poAcctItemData->getCount();
	AcctItemData *pAcct = m_poAcctItem+1;
	for(i=0;i<count;i++)
	{
		if(pAcct[i].m_iBillingCycleID==m_iBillingCycleId)
            return true;
	}
    return false;
}

/*
 *	函 数 名 : CheckIfMergerBilling
 *	函数功能 : 检查是否需要归并帐期
 *	时    间 : 2010年1月13日
 *	返 回 值 : bool:false-失败,true－成功
 *	参数说明 : 
*/
bool GetAcctAggrYear::CheckIfMergerBilling()
{
	vector<int>::iterator iter;
	OfferData *pTemData = 0;

	for(iter=m_vOfferId.begin();iter!=m_vOfferId.end();iter++)
	{
		if(m_pOfferRule->get(*iter,&pTemData))
		{
			if(pTemData->bMerger==true)
			{//归并帐期
			    Log::log(0,"归并offer_id:%d的帐期",*iter);
				return MergerBillingDb(*iter,m_iBillingCycleId);		  
			}
		}
	}
	return true;
}

/*
 *	函 数 名 : MergerBillingDb
 *	函数功能 : 归并帐期，更新his表该OFFERID的所有帐期为参数帐期
 *	时    间 : 2010年1月13日
 *	返 回 值 : bool:false-失败,true－成功
 *	参数说明 : 
*/
bool GetAcctAggrYear::MergerBillingDb(int OfferId,int iBillingCycleID)
{
	char sql[1024];

	memset(sql,0,sizeof(sql));
	
#ifndef ACCT_ITEM_AGGR_OFFER_ID
	Log::log(0,"无法归并offer_id:%d的帐期",OfferId);
	return false;
#endif

	DEFINE_QUERY (qry);
	try
	{
	sprintf(sql,"update %s set BILLING_CYCLE_ID=%d "
					"where OFFER_ID =%d and BILLING_CYCLE_ID!=%d",
						m_sTableName,iBillingCycleID,OfferId,iBillingCycleID);
	qry.setSQL(sql);
    qry.execute();
    qry.commit();
    qry.close();
	}
	catch(TOCIException & e)
	{
		Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
		return false;
	}
	return true;
}

/*
 *	函 数 名 : DeleteAggrFromHis
 *	函数功能 : 重复提取时会调用此函数，删除备份his表中输入帐期的已经提交的数据
 *	时    间 : 2010年1月7日
 *	返 回 值 : bool:false-失败,true－成功
 *	参数说明 : 
*/
bool GetAcctAggrYear::DeleteAggrFromHis()
{
	char sql[1024];
	char BeginTime[16]={0};
	char EndTime[16]={0};

	strncpy(BeginTime,m_sCycleBeginDate,16);
	strncpy(EndTime,m_sCycleEndDate,16);
	memset(sql,0,sizeof(sql));
	DEFINE_QUERY(qry);
    try
    {
    sprintf(sql,"delete from %s where billing_cycle_id = %d and state_date>to_date(%s,'yyyymmddhh24miss')"
				"and state_date <=to_date(%s,'yyyymmddhh24miss')",m_sTableName,m_iBillingCycleId,BeginTime,EndTime);
    qry.setSQL(sql);
    qry.execute();
    qry.commit();
    qry.close();
    }
    catch(TOCIException & e)
	{
		Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
		return false;
	}
	return true;
}

/*
 *	函 数 名 : GetAllAcctAggr
 *	函数功能 : 提取内存中满足条件的账单数据
 *	时    间 : 2010年1月7日
 *	返 回 值 : int
 *	参数说明 : 
*/
int GetAcctAggrYear::GetAllAcctAggr(long lServID, int iBillingCycleID, 
vector<AcctItemData *> &v)
{
    long temp = 0;
    //已经提取过的就不提取了
    if(!m_pServId->get(lServID,&temp))
        m_pServId->add(lServID,1);
    else
        return 0;
	return AcctItemAccuMgr::getAll (lServID, iBillingCycleID, v);
}


/*
 *	函 数 名 : InsertDataDb
 *	函数功能 : 账单数据入库
 *	时    间 : 2010年1月13日
 *	返 回 值 : bool:false-失败,true－成功
 *	参数说明 : 
*/
bool GetAcctAggrYear::InsertDataDb()
{
	int iSize=0,cnt=0;
	char sSql[4056]={0};
	bool bHasOfferId=false;

	if(m_vAcctItem.empty())
	{	
		return true;
	}
	iSize = m_vAcctItem.size();

#ifdef ACCT_ITEM_AGGR_OFFER_ID
	bHasOfferId = true;	
#endif

    YearAcctItemData *pBuf = new YearAcctItemData[iSize];
    vector<AcctItemData *>::iterator iter;

	for(iter=m_vAcctItem.begin();iter!=m_vAcctItem.end();iter++) 
    {
        pBuf[cnt].AcctItemID = (*iter)->m_lAcctItemID;
        pBuf[cnt].ServID = (*iter)->m_lServID;
        pBuf[cnt].BillingCycleID = (*iter)->m_iBillingCycleID;
        pBuf[cnt].AcctItemTypeId = (*iter)->m_iPayItemTypeID;
        pBuf[cnt].Charge = (*iter)->m_lValue;
        pBuf[cnt].AcctID = (*iter)->m_lAcctID;
		strcpy(pBuf[cnt].Sate,"50C");
        strcpy(pBuf[cnt].StateDate,(*iter)->m_sStateDate);                        
        pBuf[cnt].Free_Flag = 0;                        
        pBuf[cnt].Sett_Flag = 0;                        
        pBuf[cnt].ItemSourceID = (*iter)->m_iItemSourceID;
		pBuf[cnt].OfferInstID = (*iter)->m_lOfferInstID;
		if(bHasOfferId==true)
		{
			ProdOfferIns *pIns = G_PUSERINFO->getOfferIns((*iter)->m_lOfferInstID);
			if (pIns)
				pBuf[cnt].OfferId = pIns->m_iOfferID;
			else
				pBuf[cnt].OfferId = 0;
		}
		else
			pBuf[cnt].OfferId = 0;
		pBuf[cnt].MeasureType = (*iter)->m_iPayMeasure;
		pBuf[cnt].ItemClassId = (*iter)->m_iItemClassID;
        cnt++;                 
    }

	TOCIQuery qry(Environment::getDBConn());
	try
	{
	if(bHasOfferId == true)
    	sprintf(sSql,"INSERT INTO %s (ACCT_ITEM_ID,SERV_ID,BILLING_CYCLE_ID,ACCT_ITEM_TYPE_ID,CHARGE,ACCT_ID,STATE,"
                     "STATE_DATE,FREE_FLAG,SETT_FLAG,ITEM_SOURCE_ID,OFFER_INST_ID,OFFER_ID,MEASURE_TYPE,ITEM_CLASS_ID)"
					" VALUES (:ACCT_ITEM_ID,:SERV_ID,:BILLING_CYCLE_ID,:ACCT_ITEM_TYPE_ID,:CHARGE,:ACCT_ID,:STATE,"
					"TO_DATE(:STATE_DATE,'YYYYMMDDHH24MISS'),'','',:ITEM_SOURCE_ID,:OFFER_INST_ID,:OFFER_ID,:MEASURE_TYPE,:ITEM_CLASS_ID)",m_sTableName);
	else
		sprintf(sSql,"INSERT INTO %s (ACCT_ITEM_ID,SERV_ID,BILLING_CYCLE_ID,ACCT_ITEM_TYPE_ID,CHARGE,ACCT_ID,STATE,"
                     "STATE_DATE,FREE_FLAG,SETT_FLAG,ITEM_SOURCE_ID,OFFER_INST_ID,MEASURE_TYPE,ITEM_CLASS_ID)"
					" VALUES (:ACCT_ITEM_ID,:SERV_ID,:BILLING_CYCLE_ID,:ACCT_ITEM_TYPE_ID,:CHARGE,:ACCT_ID,:STATE,"
					"TO_DATE(:STATE_DATE,'YYYYMMDDHH24MISS'),'','',:ITEM_SOURCE_ID,:OFFER_INST_ID,:MEASURE_TYPE,:ITEM_CLASS_ID)",m_sTableName);
    qry.setSQL(sSql);
    
	qry.setParamArray (
        "ACCT_ITEM_ID", &(pBuf[0].AcctItemID), sizeof (pBuf[0]));
    qry.setParamArray (
        "SERV_ID", &(pBuf[0].ServID), sizeof (pBuf[0]));
    qry.setParamArray (
        "BILLING_CYCLE_ID", &(pBuf[0].BillingCycleID),sizeof (pBuf[0]));
    qry.setParamArray (
        "ACCT_ITEM_TYPE_ID",&(pBuf[0].AcctItemTypeId),sizeof (pBuf[0]));
    qry.setParamArray (
        "CHARGE",&(pBuf[0].Charge),sizeof (pBuf[0]));
    qry.setParamArray (
        "ACCT_ID",&(pBuf[0].AcctID),sizeof (pBuf[0]));
	qry.setParamArray (
        "STATE",(char **)&(pBuf[0].Sate),sizeof (pBuf[0]),sizeof(pBuf[0].Sate));
    qry.setParamArray (
        "STATE_DATE",(char **)&(pBuf[0].StateDate),sizeof (pBuf[0]),sizeof(pBuf[0].StateDate));
	qry.setParamArray (
        "ITEM_SOURCE_ID",&(pBuf[0].ItemSourceID),sizeof (pBuf[0]));
    qry.setParamArray (
        "OFFER_INST_ID",&(pBuf[0].OfferInstID),sizeof (pBuf[0]));
	if(bHasOfferId == true)
	{
    	qry.setParamArray (
        	"OFFER_ID",&(pBuf[0].OfferId),sizeof (pBuf[0]));
	}
    qry.setParamArray (
        "MEASURE_TYPE",&(pBuf[0].MeasureType),sizeof (pBuf[0]));
    qry.setParamArray (
        "ITEM_CLASS_ID",&(pBuf[0].ItemClassId),sizeof (pBuf[0]));

    qry.execute(cnt);
    qry.commit();
    qry.close();
	}
	catch(TOCIException & e)
	{
		Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
		return false;
	}
	m_iCount = cnt;
	delete[] pBuf;   
	return true;
}


/*
 *	函 数 名 : UserOperLogDb
 *	函数功能 : 程序操作日志入库
 *	时    间 : 2010年1月7日
 *	返 回 值 : bool:false-失败,true－成功
 *	参数说明 : int Flag:0-首次入库,1-重复提取,2-未获取到数据
*/
bool GetAcctAggrYear::UserOperLogDb(int Flag)
{
	char sql[1024];
	char TempState[4]={0};
	
    memset(sql,0,sizeof(sql));
	DEFINE_QUERY (qry);
	try
	{
	if(Flag==0)
	{
		sprintf(sql,"insert into B_CONSUME_YEAR_OPER_LOG (BILLING_CYCLE_ID,STATE,CREAT_TIME,STATE_TIME) "
					 "values(%d,'10A',sysdate,sysdate)",m_iBillingCycleId);
	}
	else
	{
		if(Flag==1)
			strcpy(TempState,"10B");
		else
			strcpy(TempState,"10C");
		sprintf(sql,"update B_CONSUME_YEAR_OPER_LOG set STATE = '%s',STATE_TIME = sysdate "
						"where BILLING_CYCLE_ID = %d",TempState,m_iBillingCycleId);
	}
	qry.setSQL(sql);
    qry.execute();
    qry.commit();
    qry.close();
	}
	catch(TOCIException & e)
	{
		Log::log(0,"数据库错误，信息如下：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
		return false;
	}
	return true;
}


int main(int argc,char* argv[])
{
	bool bcompel = false;
	int OperFlag = 0;
	
	if(argc<=3)
	{
		printf("一般执行说明: getacctforyear -c 帐期ID his表名\n");
		printf("强制提交说明: getacctforyear -b 帐期ID his表名\n");
		return 0;
	}
	GetAcctAggrYear *pGetAggr = new GetAcctAggrYear();
	Log::log (0, "------getacctforyear 启动------");
	if(!pGetAggr)
		return 1;
	if(strcasecmp(argv[1],"-b")==0)
	{//强制提交
		bcompel = true;
	}
	else if(strcasecmp(argv[1],"-c")!=0)
	{
		printf("一般执行说明: getacctforyear -c 帐期ID his表名\n");
		return 1;	
	}
	pGetAggr->m_iBillingCycleId = atoi(argv[2]);
	strcpy(pGetAggr->m_sTableName,argv[3]);
	if(pGetAggr->CheckBillingData()==false)
	{
		Log::log (0, "内存中无该帐期的数据");
		return 1;
	}
	Log::log (0, "帐期ID:%d",pGetAggr->m_iBillingCycleId);
	Log::log (0, "备份his表名:%s",pGetAggr->m_sTableName);
	if(pGetAggr->GetBillingCycleTime()==false)
	{
		Log::log (0, "获取输入帐期ID的时间失败");
		return 1;
	}
	if(pGetAggr->CheckUserOper()==false)
	{//已经提交过该帐期的数据
		if(bcompel==true)
		{//强制提交，首先要删除备份his表中数据，以免重复
			if(pGetAggr->DeleteAggrFromHis()==false)
			{
				Log::log (0, "删除备份his表失败!");	
				return 1;
			}
			OperFlag = 1;
		}
		else
		{
			Log::log (0, "该帐期数据已经提取过!!!如果想重新提取，请用强制提取参数启动");
			return 1;
		}
	}
	Log::log (0, "开始提取数据!");
	if(pGetAggr->GetOfferId()==false)
	{
		return 1;
	}
	if(pGetAggr->m_iOfferCount!=0)
	{
		Log::log (0, "从配置表提取的商品ID个数为:%d",pGetAggr->m_iOfferCount);	
	}
	else
	{
		Log::log (0, "从配置表提取的商品ID个数为0");
		return 1;
	}
	pGetAggr->GetInstInfoById();
	if(pGetAggr->InsertDataDb()==false)
	{
		Log::log (0, "提取数据入库失败");
		return 1;
	}
	if(pGetAggr->m_iCount==0)
		OperFlag = 2;
	Log::log (0, "提取到的数据总数为:%d条",pGetAggr->m_iCount);
	if(pGetAggr->CheckIfMergerBilling()==false)
	{
		Log::log (0, "归并帐期失败!");

	}
	if(pGetAggr->UserOperLogDb(OperFlag)==false)
	{
		Log::log (0, "写操作日志失败");
		return 1;	
	}
	else
		Log::log (0, "数据提取完成!");
	Log::log (0, "------getacctforyear 关闭------");
	
	delete pGetAggr;
	pGetAggr = NULL;
	return 0;
}
