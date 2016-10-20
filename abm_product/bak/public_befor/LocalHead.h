/*VER: 2*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部 
// All rights reserved.


#ifndef LOCALHEAD_H_HEADER_INCLUDED_BD7FE5BA
#define LOCALHEAD_H_HEADER_INCLUDED_BD7FE5BA
#include "KeyTree.h"

const int MAX_AREA_CODE_LEN = 10;
const int MAX_HEAD_LEN = 21;
const int MAX_ACCNBR_LEN = 32;

//##ModelId=42802B30011D
//##Documentation
//## 号头 local_head 类
class LocalHead
{
  friend class LocalHeadMgr;
  friend class ParamInfoMgr;
	
  public:
	  LocalHead()
	  {
		  m_iOrgID=-2;
		  memset(m_sAreaCode,0,sizeof(m_sAreaCode));
		  memset(m_sHead,0,sizeof(m_sHead));
		  m_iLen = 0;
		  memset(m_sEffDate,0,sizeof(m_sEffDate));
		  memset(m_sExpDate,0,sizeof(m_sExpDate));
		  m_pNext= 0;
	  };
    //##ModelId=42803E6602C3
    int getOrgID() const;


  
    //##ModelId=42803E810055
    char* getAreaCode() const;

  
    //##ModelId=42803E940193
    int getAccNbrLen() const;

  
    //##ModelId=42803EAC00CF
    char* getHead() const;

//  private:
    //##ModelId=42802BCF019E
    //##Documentation
    //## 电信运维机构
    int m_iOrgID;

    //##ModelId=42802C2D0384
    //##Documentation
    //## 号头的区号
    char m_sAreaCode[MAX_AREA_CODE_LEN];

    //##ModelId=42803C980391
    char m_sHead[MAX_ACCNBR_LEN];

    //##ModelId=42802C9D0280
    //##Documentation
    //## 号头适用的号码长度
    int m_iLen;

    //##ModelId=42802CD900EC
    char m_sEffDate[15];

    //##ModelId=42802CF103AE
    char m_sExpDate[15];

	LocalHead* m_pNext;

#ifndef PRIVATE_MEMORY
    unsigned int m_iNextOffset;
#endif

};



//##ModelId=428030E900AC
//##Documentation
//## 本地号头管理类
class LocalHeadMgr
{
  public:
    //##ModelId=42803ACA03E7
    LocalHeadMgr();

  
    //##ModelId=42803ADA030E
    ~LocalHeadMgr();
    //##ModelId=42803B8001A4
    static bool searchLocalHead(const char *_sAccNbr, const char *_sAreaCode, const char *_sTime, LocalHead *pLocalHead);


  private:
    //##ModelId=42803AE60329
    static void loadLocalHead();

    //##ModelId=42803AF3021A
    //##ModelId=42803AF3021A
    static void unloadLocalHead();

    //##ModelId=4280316802FD
    static KeyTree<LocalHead*>* m_poLocalHead;

    //##ModelId=4280447C0292
    static int m_iMaxHeadLen;

    //##ModelId=42804495031A
    static int m_iMinHeadLen;

};

#endif /* LOCALHEAD_H_HEADER_INCLUDED_BD7FE5BA */
