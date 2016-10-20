/*VER: 1*/ 
#ifndef PLAN_TYPE_H
#define PLAN_TYPE_H

#include "HashList.h"

/*
	把定价计划的类型用起来.
	为了解决数据业务的 账号和固定电话 的问题; 
	因为现在设计不支持真正意义上的其他方付费
*/

class PlanTypeMgr
{
  public:
	int getPlanType(int iPricingPlanID);

  public:
	PlanTypeMgr();
	
	void load();
	void unload();

  private:
	static bool m_bUploaded;
	static HashList<int> * m_poIndex;
  	
};

#endif
