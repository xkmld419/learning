/*VER: 1*/ 
#ifndef LIFECYCLEMGR_H_HEADER_INCLUDED_BDADBDA2
#define LIFECYCLEMGR_H_HEADER_INCLUDED_BDADBDA2

#include "CommonMacro.h"
#include "HashList.h"
#include "UserInfoReader.h"
#include "StdEvent.h"

class ProdOfferIns;

//##ModelId=424E12BC027D
//##Documentation
//## 生命周期
class LifeCycle
{
  public:

    //##ModelId=424E14A5030D
    //##Documentation
    //## 查看某个日期是否属于这个生命周期
    //## 这个用于绝对生命周期
    bool check(char *sDate);

    bool check(char *sDate, ProdOfferIns *pOfferInst);

    //## 以后，定价组合调用这个接口先
    bool check(char *sDate, ExPricingPlan & explan);

    bool check(StdEvent *pEvent, ExPricingPlan & explan);

    //##ModelId=425334D8022D
    LifeCycle()
    {
	m_iCycleID = 0;
	memset (m_sEffDate, 0, sizeof (m_sEffDate));
	memset (m_sExpDate, 0, sizeof (m_sExpDate));
	m_iEffValue = 0;
	m_iExpValue = 0;
	m_iCycleType = 0;
    }
	

  public:

    //##ModelId=424E13B201A5
    int m_iCycleID;

    //##ModelId=424E1396030D
    char m_sEffDate[16];

    //##ModelId=424E13A200BB
    char m_sExpDate[16];

    int m_iEffValue;

    int m_iExpValue;

    //##ModelId=424E13C1002A
    int m_iCycleType;

};

//##ModelId=424E14E303D4
class LifeCycleMgr
{
  public:
    //##ModelId=424E15B7027A
    LifeCycle *getLifeCycle(int iLifeCycleID);

	LifeCycle *getLifeCycleShm(int iLifeCycleID);
    //##ModelId=424E166002BA
    void load();

    //##ModelId=424E169000B0
    void unload();

    //##ModelId=424E220203AC
    LifeCycleMgr();

  private:
    //##ModelId=424E15EB0211
    static bool m_bUploaded;

    //##ModelId=424E156301F7
    static HashList<LifeCycle *> *m_poIndex;

    //##ModelId=42521B790041
    static LifeCycle *m_poLifeCycleList;

};



#endif /* LIFECYCLEMGR_H_HEADER_INCLUDED_BDADBDA2 */
