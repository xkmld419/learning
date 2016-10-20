/*VER: 1*/
#ifndef _REGION_BILLING_H_
#define _REGION_BILLING_H_
#include "HashList.h"
#include "TOCIQuery.h"
#include "Environment.h"

using namespace std ;

struct RegionBill
{
	char m_sCellID[9];
	int  m_iGroupID;
	RegionBill * m_poNext;
};

struct RegionNasBill
{
	char m_sNasIP[MAX_NAS_IP_LEN];
	int  m_iGroupID;
	RegionNasBill * m_poNext;
};


class RegionBilling
{
public:
    static bool  CheckRelation(char* sCellID,int iGroupID);
    static bool  CheckNasRelation(char* sNasIP,int iGroupID);
    

private:
    static bool LoadRule(void);
	static HashList<RegionBill *>  * m_poRelation;
	static bool m_bIsLoad;

	static HashList<RegionNasBill *>  * m_poNasRelation;
    static bool LoadNasRule(void);

};

#endif
