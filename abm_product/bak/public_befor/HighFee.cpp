/*VER: 2*/ 
#include "TOCIQuery.h"
#include "HighFee.h"

#include "OrgMgrEx.h"

HighFee::HighFee(void)
{
	this->LoadHiFeeParam();
}

HighFee::~HighFee(void)
{
	MAPHIPLAN::iterator	itr;
	MAPHISECTION::iterator itr1;
	HighFeePlan *pHFP = NULL;
	//清高额计划
	for( itr = mapHiPlan.begin();itr != mapHiPlan.end();itr ++ )
	{
		pHFP = itr->second;
		if( pHFP != NULL )
			delete pHFP;
	}

	//清高额段落
	VECHISECTION *pVec =NULL;
	HighFeeSection *pHFS =NULL;
	for( itr1 = mapHiSection.begin();itr1 != mapHiSection.end();itr1 ++ )
	{
		if( itr1->second != NULL )
		{
			pVec = itr1->second;
			for( int i =0;i < pVec->size();i ++ )
			{
				pHFS = (*pVec)[i];
				if( pHFS != NULL )
					delete pHFS;
			}
			delete pVec;
		}
	}
}

int HighFee::LoadHiFeeParam()
{
	//载入高额计划
	char sSqlPlan[]	= "select highfee_plan_id,org_id,highfee_strategy_id,to_char(eff_date,'yyyymmddhh24miss') effdate, \
					  to_char(exp_date,'yyyymmddhh24miss') expdate from R_HIGHFEE_PLAN \
					  where sysdate > eff_date and sysdate< exp_date";
	HighFeePlan *pHiFeePlan = NULL;

	TOCIQuery qryPlan(Environment::getDBConn());
	qryPlan.close();
	qryPlan.setSQL(sSqlPlan);
	qryPlan.open();

	while( qryPlan.next() ) 
	{ 
		pHiFeePlan = new HighFeePlan();

		pHiFeePlan->m_iHiFeePlanID		= qryPlan.field(0).asInteger();
		pHiFeePlan->m_iOrgID			= qryPlan.field(1).asInteger();
		pHiFeePlan->m_iHiFeeStrategyID	= qryPlan.field(2).asInteger();
		strcpy( pHiFeePlan->m_sEffDate,qryPlan.field(3).asString() );
		strcpy( pHiFeePlan->m_sExpDate,qryPlan.field(4).asString() );
		mapHiPlan[pHiFeePlan->m_iOrgID] = pHiFeePlan;
	}
	qryPlan.close();

	//载入高额段落
	HighFeeSection *pHiFeeSection = NULL;
	MAPHISECTION::iterator itr;
	VECHISECTION	*vecHiSection = NULL;

	char sSqlSection[]	= "select highfee_section_id,highfee_strategy_id,highfee_level_id,highfee_start_value, \
						  highfee_end_value from R_HIGHFEE_SECTION";
	qryPlan.close();
	qryPlan.setSQL(sSqlSection);
	qryPlan.open();

	while( qryPlan.next() ) 
	{ 
		pHiFeeSection = new HighFeeSection();

		pHiFeeSection->m_iHiFeeSectionID	= qryPlan.field(0).asInteger();
		pHiFeeSection->m_iHiFeeStrategyID	= qryPlan.field(1).asInteger();
		pHiFeeSection->m_iHiFeeLevelID		= qryPlan.field(2).asInteger();
		pHiFeeSection->m_lStartValue		= qryPlan.field(3).asLong();
		pHiFeeSection->m_lEndValue			= qryPlan.field(4).asLong();
		
		//插入map
		itr = mapHiSection.find(pHiFeeSection->m_iHiFeeStrategyID);
		if( itr == mapHiSection.end() )
		{
			vecHiSection = new VECHISECTION();
			mapHiSection[pHiFeeSection->m_iHiFeeStrategyID] = vecHiSection;
		}
		else
		{
			vecHiSection = itr->second;
		}
		vecHiSection->push_back(pHiFeeSection);
	}

	return 1;
}

int HighFee::GetLevelID(int iOrgID,long lValue,long lCharge)
{
	int iHiFeeStrategyID = 0;
	VECHISECTION	*vecHiSection = NULL;
	HighFeeSection	*pHiFeeSection = NULL;
	int iLevelID = 0;

	//OrgID归并
	int iDestOrgID = 0;
	int iTmpOrg = 0;
	
	OrgMgrEx * poOrgMgr = OrgMgrEx::GetInstance ();

	if (!poOrgMgr) return -1;

	//通过OrgID找策略
	MAPHIPLAN::iterator itr;

	while (iOrgID != -1) {
		itr = mapHiPlan.find(iOrgID);

		if( itr != mapHiPlan.end() ) {
			break;
		}

		iOrgID = poOrgMgr->getParentID(iOrgID);
	}

	if (iOrgID == -1) {
		return -1;
	}

	iHiFeeStrategyID = itr->second->m_iHiFeeStrategyID;
	//找通过策略标识找高额标识
	MAPHISECTION::iterator itr1;
	itr1 = mapHiSection.find(iHiFeeStrategyID);
	if( itr1 == mapHiSection.end() )
		return -1;
	vecHiSection = itr1->second;

	for( int i = 0;i < vecHiSection->size();i++ )
	{
		pHiFeeSection = (*vecHiSection)[i];
		if( lValue >= pHiFeeSection->m_lStartValue && lValue < pHiFeeSection->m_lEndValue )
		{
			/*
			//如果越级的才送
			if( (lValue - lCharge) < pHiFeeSection->m_lStartValue )
				iLevelID = pHiFeeSection->m_iHiFeeLevelID;
			*/
			//modifed by jx 20090409
			//由于话单与累积量可能不同步，且现在由语音统一触发。可能导致高额丟单。
			//现修改为每条话单都触发，需要依赖后续排重进程。
			//一定要配置排重
			iLevelID = pHiFeeSection->m_iHiFeeLevelID;
			break;
		}
	}

	return iLevelID;
}

long HighFee::GetThreshold(int iOrgID,int iLevelID)
{
	int iHiFeeStrategyID = 0;
	VECHISECTION	*vecHiSection = NULL;
	HighFeeSection	*pHiFeeSection = NULL;
	long lStartValue = -1;

	//OrgID归并
	int iDestOrgID = 0;
	int iTmpOrg = 0;

	OrgMgrEx * poOrgMgr = OrgMgrEx::GetInstance ();

	if (!poOrgMgr) return -1;

	//通过OrgID找策略
	MAPHIPLAN::iterator itr;

	while (iOrgID != -1) {
		itr = mapHiPlan.find(iOrgID);

		if( itr != mapHiPlan.end() ) {
			break;
		}

		iOrgID = poOrgMgr->getParentID(iOrgID);
	}

	if (iOrgID == -1) {
		return -1;
	}

	iHiFeeStrategyID = itr->second->m_iHiFeeStrategyID;
	//找门限值
	MAPHISECTION::iterator itr1;
	itr1 = mapHiSection.find(iHiFeeStrategyID);
	if( itr1 == mapHiSection.end() )
		return -1;
	vecHiSection = itr1->second;

	for( int i = 0;i < vecHiSection->size();i++ )
	{
		pHiFeeSection = (*vecHiSection)[i];
		if( iLevelID == pHiFeeSection->m_iHiFeeLevelID )
		{
			lStartValue = pHiFeeSection->m_lStartValue;
			break;
		}
	}

	return lStartValue;
}
