#ifndef _HIGHFEE_H_
#define _HIGHFEE_H_ 

#include <stdlib.h>   
#include <stdio.h>  
#include "Environment.h"
#include <map>
#include <vector>

using namespace std;

class HighFeePlan
{
public:
	HighFeePlan():
	  m_iHiFeePlanID(0),
	  m_iOrgID(0),
	  m_iHiFeeStrategyID(0)
	  {
		  memset(m_sEffDate,0,sizeof(m_sEffDate));
		  memset(m_sExpDate,0,sizeof(m_sExpDate));
	  };
	~HighFeePlan(){};

public:
	int		m_iHiFeePlanID;		//计划标识	
	int		m_iOrgID;			//区域标识	
	int		m_iHiFeeStrategyID;	//策略标识	
	char	m_sEffDate[14];		//生效时间	
	char	m_sExpDate[14];		//失效时间
};

class HighFeeSection
{
public:
	HighFeeSection():
	  m_iHiFeeSectionID(0),
	  m_iHiFeeStrategyID(0),
	  m_iHiFeeLevelID(0),
	  m_lStartValue(0),
	  m_lEndValue(0)
	  {
	  };
	~HighFeeSection(){};
		
public:
	int		m_iHiFeeSectionID;	//段落标识	
	int		m_iHiFeeStrategyID;	//策略标识	
	int		m_iHiFeeLevelID;	//级别标识	
	long	m_lStartValue;		//段落起始值
	long	m_lEndValue;		//段落终止值
	//HighFeeSection *m_poNext;
};

//高额计划
typedef map<int,HighFeePlan *> MAPHIPLAN;
//高额段落
typedef vector<HighFeeSection*> VECHISECTION;
typedef map<int,VECHISECTION*> MAPHISECTION;

class HighFee
{
public:
	HighFee(void);
	~HighFee(void);

public:
	MAPHIPLAN		mapHiPlan;
	MAPHISECTION	    mapHiSection;
	
public:
	int LoadHiFeeParam();
	int GetLevelID(int iOrgID, long lValue,long lCharge);
	long GetThreshold(int iOrgID,int iLevelID);
};

#endif
