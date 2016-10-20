/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// 
// 
// 
// All rights reserved.

#ifndef LOCALEVENTTYPERULE_H_HEADER_INCLUDED_BD7E31FB
#define LOCALEVENTTYPERULE_H_HEADER_INCLUDED_BD7E31FB

#include "Environment.h"
#include "OrgMgr.h"

class LocalEventTypeRuleMgr;

//##ModelId=4281B4CD0186
class LocalEventTypeRule
{
  friend class LocalEventTypeRuleMgr;
#ifndef PRIVATE_MEMORY
    friend class ParamInfoMgr;
	friend class SeekSHMTool;
#endif
  
  public:
    //##ModelId=4281BC4603CB
    int getRuleID() const;

    //##ModelId=4281BC5C014C
    int getOutEventTypeID() const;

    //##ModelId=4281BC7B033B
    LocalEventTypeRule() {	
        m_poNextSourceEvent = 0;
        m_poNextInEventType = 0;
        m_poNextCallingOrgID = 0;
        m_poNextCalledOrgID = 0;
        m_pCallingTree = 0;
        m_pCalledTree = 0;
        m_poNextRule = 0; 
    };
    
    void addTree (KeyTree <LocalEventTypeRule *> *pTree,
        int _OrgID, LocalEventTypeRule * pt);
    
    //##ModelId=4281BC890237
    ~LocalEventTypeRule();
    
    void print();
    
  private:
	//add by yangks 05.06.16
	//源事件类型
	int m_iSourceEventType;

    //##ModelId=4281B614032A
    int m_iRuleID;

    //##ModelId=4281B62400DF
    int m_iInEventTypeID;
  
  public:
    //##ModelId=4281B62502E9
    int m_iCallingOrgID;

    //##ModelId=4281B6270057
    int m_iCalledOrgID;
  
  private:
    //##ModelId=4281B62800F8
    int m_iCallingSPTypeID;

    //##ModelId=4281BBDD0226
    int m_iCalledSPTypeID;

    //##ModelId=4281BBE80249
    int m_iCallingServiceTypeID;

    //##ModelId=4281BBFA0322
    int m_iCalledServiceTypeID;

    //##ModelId=4281BC060012
    int m_iCarrierTypeID;

    //##ModelId=4281BC2B0174
    int m_iBusinessKey;

    //##ModelId=4281BC360101
    int m_iOutEventTypeID;

    //##ModelId=4281BD31036F
    LocalEventTypeRule *m_poNextSourceEvent;
    
    LocalEventTypeRule *m_poNextInEventType;
    LocalEventTypeRule *m_poNextCallingOrgID;
    LocalEventTypeRule *m_poNextCalledOrgID;
    
    //##ModelId=4281BD4703A3
    LocalEventTypeRule *m_poNextRule;

    KeyTree <LocalEventTypeRule *> *m_pCallingTree;
    KeyTree <LocalEventTypeRule *> *m_pCalledTree;

	#ifndef PRIVATE_MEMORY
	unsigned int m_iNextOffset;
	char m_sCallingOrgPath[50];
	char m_sCalledOrgPath[50];
	#endif

};



//##ModelId=4281BE0F0224
class LocalEventTypeRuleMgr
{
  public:
    //##ModelId=4281BE2D021D
    LocalEventTypeRuleMgr();

    //##ModelId=4281BE3A02DA
    ~LocalEventTypeRuleMgr();

    //##ModelId=4281BE5201B2
    static void loadRule();
    static void load_Rule_V2();

    //##ModelId=4281BE730119
    static void unloadRule();

    //##ModelId=4281BE86036F
    static bool getRule(StdEvent const *_pEvent, 
    		LocalEventTypeRule ** _ppRule);
    
    static bool get_Rule_V2(StdEvent const *_pEvent, 
    		LocalEventTypeRule ** _ppRule);
    
  private:
    static bool getEventOrgPath (int iOrg, char *sPath);
        
  private:
    //##ModelId=4281BF760220
    static bool m_bInit;
    
    //##ModelId=4281BEFF01ED
    static LocalEventTypeRule *m_poRule;

    //##ModelId=4281C5E003C6
    static OrgMgr *m_poOrgMgr;
    
    static int m_iFirstOrgID;

};


class OrgPath { 
  public:
    OrgPath () {m_iOrgID = -1; m_sOrgPath[0] = 0; m_poParent = 0x0;}
    
    int m_iOrgID;
    
    int m_iRowID;
    
    char m_sOrgPath[50];
    
    OrgPath *m_poParent;
	
	#ifndef PRIVATE_MEMORY
	unsigned int m_iParentOffset;
	#endif
};

class OrgPathMgr {
  public:
    static OrgPath *getOrgPath (int _iOrgID);
    
    static bool getParentOrgPath (const char *sPath, char *sRetPath)
    {
        static int iPos;
        strcpy (sRetPath, sPath);
        iPos = strlen(sRetPath) - 3;
        while (iPos > 0)
        {
            if (sRetPath[iPos] == '|') {
                sRetPath[iPos+1] = 0;
                return true;
            }
            iPos --;
        }
        return false;
    }
    
  private:
    static void init ();

  private:
    static bool m_bLoaded;
    
    static OrgMgr *m_poOrgMgr;
    
    static OrgPath *m_poOrgPathList;
    
    static KeyTree<OrgPath*> *m_poOrgPathTree;
};


#endif /* LOCALEVENTTYPERULE_H_HEADER_INCLUDED_BD7E31FB */
