#ifndef __LINKAGE_SPECIAL_TARIFF_MGR__
#define __LINKAGE_SPECIAL_TARIFF_MGR__

#include "HashList.h"
#include "EventSection.h"
#include "CommonMacro.h"

struct SpecialTariff
{
public:
	int		m_iUpDownFlag;
	int		m_iBillingType;
	char	m_sSPFLAG[MAX_SPFLAG_LEN];
	char	m_sNBR[MAX_CALLING_NBR_LEN];
	char	m_sServiceNBR[MAX_CALLING_NBR_LEN];
	char	m_sEffDate[MAX_START_DATE_LEN];
	char	m_sExpDate[MAX_START_DATE_LEN];
	int		m_iCharge;

	SpecialTariff * m_poNext;
	unsigned int m_iNext;
};

class SpecialTariffMgr {
public:
    static bool getFeeByInfo(EventSection * oEvent,int &iCharge);
	static bool checkFeeValid(EventSection * oEvent,long lCharge);
	static bool checkIsTestCdr(EventSection * oEvent);
    SpecialTariffMgr();
	~SpecialTariffMgr();

private:
    static bool load();
	static void getAttrValue(EventSection * oEvent,SpecialTariff &oTariff);
	//static long getChargeValue(EventSection * oEvent);
	static void initParam();
	static bool checkFeeExist(SpecialTariff * poTariff);
private:
    static HashList<SpecialTariff *>  * m_poSpecialTariffIndex;
	static SpecialTariff * m_poSpecialTariffData;
    static HashList<SpecialTariff *>  * m_poSpecialTariffCheckIndex;
	static SpecialTariff * m_poSpecialTariffCheckData;
	/*四个属性的顺序是：上下行标识、SP代码、接入码、业务码
		这四个字段如果写死的话可以取StdEvent中的：m_iCallTypeID,m_sSPFLAG,m_sCalledNBR,m_sBindAttr
	*/
	static int m_iaAttrID[4];
	static int m_iType;
	static bool bInit;
	//static long m_lCharge;	
	//static char m_sChargeAttrID[32];
};

#endif
