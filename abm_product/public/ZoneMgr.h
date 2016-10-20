/*VER: 2*/ 
#ifndef ZONEMGR_H_HEADER_INCLUDED_BDA9B276
#define ZONEMGR_H_HEADER_INCLUDED_BDA9B276

#include "CommonMacro.h"
#include "HashList.h"
#include "KeyTree.h"

//##ModelId=4255FA280150
class ZoneItem
{
    friend class ZoneMgr;
    friend class ParamInfoMgr;

  public:
    //##ModelId=4255FA8D0354
    ZoneItem();


    //##ModelId=4255FA3D03BD
    //##Documentation
    //## 区表标识
    int m_iZoneID;

    //##ModelId=4255FA4A0073
    //##Documentation
    //## 区表节点标识
    int m_iZoneItemID;

    //##ModelId=4255FA530274
    //##Documentation
    //## 区表节点父节点标识
    int m_iParentItemID;

    //##ModelId=42561667032F
    ZoneItem *m_poParent;

#ifndef PRIVATE_MEMORY
    //value
    char m_sZoneItemValue[32];

    unsigned int m_iNextDifOffset;
    unsigned int m_iNextOffset;
#endif
  private:

};



//##ModelId=4255F9B6025B
//##Documentation
//## 区表管理类
class ZoneMgr
{
  public:
    //##ModelId=4256019403D3
    static void load();

    //##ModelId=425601990113
    static void unload();

    static bool reload();
    
    //##ModelId=425601CF00A3
    bool check(
        //##Documentation
        //## 区表标识
        int iZoneID, 
        //##Documentation
        //## 传入的值（字符串型）
        char *sValue, int iZoneItemID);

    bool checkMax(
        int iZoneID,
        char *sValue, int iZoneItemID);

    //##ModelId=4256026A00A6
    ZoneMgr();

  private:

    //##ModelId=4256010801FB
    static bool m_bUploaded;

    //##ModelId=4256088602F3
    char m_sKeyBuf[80];

    //##ModelId=42561C00016D
    static HashList<KeyTree<ZoneItem *> *> *m_poIndex;

    //##ModelId=425616620309
    static ZoneItem *m_poZoneItemList;

    //##ModelId=4256166401E0
    static HashList<ZoneItem *> *m_poTempIndex;

};

#endif /* ZONEMGR_H_HEADER_INCLUDED_BDA9B276 */
