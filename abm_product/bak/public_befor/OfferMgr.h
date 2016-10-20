/*VER: 1*/ 
/*
**
**
**
**  2007.10.17 xueqt 
**      增加loadFreeOffer; isFreeOffer; m_poFreeOfferIndex
*/

#include "CommonMacro.h"
#include "HashList.h"
#ifndef OFFERMGR_H_HEADER_INCLUDED_BDB745F9
#define OFFERMGR_H_HEADER_INCLUDED_BDB745F9
enum OfferType
{
    OFFER_TYPE_BASE = 0,
    OFFER_TYPE_DISCT
};

class OfferAttr
{
	public:
		int m_iAttrId;
		char m_sAttrValue[32];
		OfferAttr *m_poNext;
};
      
class Offer
{
public:
    //商品ID
    int m_iOfferID;
    //定价计划表示
    int m_iPricingPlan;
    //优先级
    int m_iPriority;
    //商品类型：基本、套餐
    OfferType m_iOfferType;
    //生效日期
    char m_sEffDate[16];
    //失效日期
    char m_sExpDate[16];
    //品牌信息
    int m_iBrandID;
    //商品属性
    OfferAttr *m_poOfferAttr;
		//add by zhaoziwei
		//商品协议期月数
		int m_iAgreementLength;
		//end
};
class OfferMgr
{
  //商品
  Offer * m_poOfferInfoList;

  //商品存储的索引
  HashList<Offer *> *m_poOfferInfoIndex;
  
  //商品属性
  OfferAttr *m_poOfferAttrList;
  
  //商品属性索引
  HashList<OfferAttr *> *m_poOfferAttrIndex;
    
  HashList<int> * m_poFreeOfferIndex;

	HashList<int>  * m_poSpeicalOfferAttrIndex;
    
	HashList<int>  * m_poNumberOfferAttrIndex;
	
  static bool m_bUploaded;
    
public:
    //## 加载定商品，建立相关索引
  void load();
    
  void loadFreeOffer();

	//加载属性类型为15的属性ID与属性值对应有关系
	void loadSpecialAttr();

	//加载数字型的商品属性
	void loadNumberAttr();

    //## 卸载所加载的信息
  void unload();
    
  Offer * getOfferInfoByID(int iOfferID);
    
  bool isFreeOffer(int iOfferID);

	bool checkSpecialAttr(int iAttrID,int &iOperation);

	bool checkNumberAttr(int iAttrID);
	
	char * getOfferAttrValue(int iOfferId,int iAttrId,char *sDate);
 
	bool getbrand(int iOfferId,int &ibrandid );    
    
  OfferMgr();
    
  ~OfferMgr();

private:
	int m_iFlag;
	
	int m_iUsedDefaultAttr;
};
#endif
