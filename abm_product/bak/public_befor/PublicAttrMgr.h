#ifndef __LINKAGE_PUBLIC_ATTR_MGR__
#define __LINKAGE_PUBLIC_ATTR_MGR__

#include "HashList.h"

struct PublicAttrData
{
	int m_iOfferID;
	int m_iMemberID;
	int m_iAttrID;
	PublicAttrData * m_poNext;
};

class PublicAttrMgr {

public:
    static bool queryByOfferID (int iOfferID,int &iMemberID);
	static bool queryByOfferIDAndMember (int iOfferID,int iInMemberID,int &iOutMemberID);
    PublicAttrMgr();
	~PublicAttrMgr();

private:
    static void load();

private:
    static HashList<PublicAttrData *>  * m_poDataIndex;
	static PublicAttrData * m_poData;
    static bool bInit;

};

#endif //__LINKAGE_PUBLIC_ATTR_MGR__
