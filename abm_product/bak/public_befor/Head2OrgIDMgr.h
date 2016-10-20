/*VER: 2*/ 
#ifndef _HEAD2ORGID_MGR_H_
#define _HEAD2ORGID_MGR_H_


#include "KeyTree.h"
#include "OrgMgrEx.h"

#include <vector>

const int MAX_AREA_CODE_LEN_EX = 10;
const int MAX_HEAD_LEN_EX = 64;
const int MAX_ACCNBR_LEN_EX = 32;
const int MAX_TYPE = 30;

//##ModelId=42802B30011D
//##Documentation
//## 号头 local_head 类
class HeadEx
{
  friend class Head2OrgIDMgr;
  friend class ParamInfoMgr;
    
  public:
    //##ModelId=42803E6602C3
    int getOrgID() const;

    HeadEx(){m_iPartyID =0;m_iPartnerID=0;m_pNext=0;};
    
    //##ModelId=42803E810055
    char* getAreaCode() const;

  
    //##ModelId=42803E940193
    int getAccNbrLen() const;

  
    //##ModelId=42803EAC00CF
    char* getHead() const;

    int getPartyID(void) const
    {
        return m_iPartyID;
    };
    int getPartnerID(void) const
    {
        return m_iPartnerID;
    };
//add by wudong 
    char * getExpDate(void)
    {
        return m_sExpDate;
    };
    char * getEffDate(void)
    {
        return m_sEffDate;
    };
    char * getHeadex(void)
    {
        return m_sHead;
    };
    char * getTypeCode(void)
    {
        return m_sTypeCode;
    };
//add by wudong

  private:
    //##ModelId=42802BCF019E
    //##Documentation
    //## 电信运维机构
    int m_iOrgID;

    //##ModelId=42802C2D0384
    //##Documentation
    //## 号头的区号
    char m_sAreaCode[MAX_AREA_CODE_LEN_EX];

    //##ModelId=42803C980391
    char m_sHead[MAX_ACCNBR_LEN_EX];

    ////##ModelId=42802C9D0280
    ////##Documentation
    ////## 号头适用的号码长度
    //int m_iLen;

    //##ModelId=42802CD900EC
    char m_sEffDate[15];

    //##ModelId=42802CF103AE
    char m_sExpDate[15];

    char m_sTypeCode[4];
    int m_iPartyID;
    int m_iPartnerID;
    //int m_nCarrier;
public:
    HeadEx * m_pNext;

#ifndef PRIVATE_MEMORY
    unsigned int m_iNextOffset;
#endif

};



class Head2OrgIDMgr
{
public:
    Head2OrgIDMgr(void);
    ~Head2OrgIDMgr(void);

    static bool searchLocalHead(const char *_sAccNbr,
        const char *_sTime, HeadEx *pLocalHead,const char* pType="50Z",const int iOrgType =BILL_ORG );

    static bool getOrgID(const char *pHead,
        const char* pTime,int& iOrgID,const char* pType="50Z",const int iOrgType = BILL_ORG);
    static bool getOrgIDs(const char* pHead, const char *pTime, 
        vector<int> & pvOrgID, const char* pType="50Z",const int iOrgType = BILL_ORG);

    static bool reloadLocalHead();
    
    // add by zhangch 20100603
    static bool isBelongOrg(const char *pHead,
        const char* pTime,int iOrgID,const char* pType="50Z",const int iOrgType = BILL_ORG);  
  private:
    //##ModelId=42803AE60329
    static void loadLocalHead();

    //##ModelId=42803AF3021A
    //##ModelId=42803AF3021A
    static void unloadLocalHead();

    //##ModelId=4280316802FD
    static KeyTree<HeadEx>* m_ppoLocalHead[MAX_TYPE][MAX_TYPE];

    //##ModelId=4280447C0292
    static int m_iMaxHeadLen;

    //##ModelId=42804495031A
    static int m_iMinHeadLen;

    static HeadEx m_goHeadEx;
    static bool m_bIsLoad;
    
    static vector<HeadEx*> vecSameHead;
    
    // add by zhangch 20100603
    static OrgMgrEx *m_pOrgMgrEx;
};


#endif
