/*VER: 1*/ 
#ifndef BSS_ORGMGR_H_HEADER_INCLUDED_BDB76CF5
#define BSS_ORGMGR_H_HEADER_INCLUDED_BDB76CF5

#include "CommonMacro.h"
#include "HashList.h"


//##ModelId=42732DA90096
class BssOrg
{
  public:
    //##ModelId=427333930396
    BssOrg();

  private:
    friend class BssOrgMgr;

    //##ModelId=42732DF10108
    int m_iOrgID;

    //##ModelId=42732DF60051
    int m_iParentID;

    //##ModelId=42732DFA03E6
    int m_iLevel;

    //##ModelId=42732E420169
    BssOrg *m_poParent;

};

//##ModelId=4248F7ED019E
//##Documentation
//## 电信运维机构
class BssOrgMgr
{
  public:
    //##ModelId=4248F8440199
    void load();

    //##ModelId=4248F84401D5
    void unload();

    //##ModelId=4248F8440211
    bool getBelong(int iChildOrg, int iParentOrg);

    //##ModelId=4248F844024D
    BssOrgMgr();

    //##ModelId=4248F8440289
    ~BssOrgMgr();
    //##ModelId=42732CFB01F5
    int getParentID(int iOrgID);


    //##ModelId=42732D1800AC
    int getLevel(int iOrgID);

  private:
    //##ModelId=4248F8730331
    static HashList<BssOrg *> *m_poOrgIndex;
    //##ModelId=42732EB50024
    static bool m_bUploaded;


    //##ModelId=42732E62022D
    static BssOrg *m_poOrgList;


};


#endif /* ORGMGR_H_HEADER_INCLUDED_BDB76CF5 */
