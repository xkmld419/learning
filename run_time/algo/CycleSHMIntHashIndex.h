/*VER: 1*/
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef CYCLESHMINTHASHINDEX_H_HEADER_INCLUDED_BDBB0F0A
#define CYCLESHMINTHASHINDEX_H_HEADER_INCLUDED_BDBB0F0A
#include <memory.h>
#include "SHMAccess.h"

#define Cycle_ATTACH_INT_INDEX(X, Z) \
    X = new CycleSHMIntHashIndex (Z); \
    if (!(X)) { \
    freeAll (); \
    THROW(MBC_UserInfo+1); \
    } \
    if (!(X->exist())) { \
    m_bAttached = false; \
    }

//##ModelId=4244323E0173
class CycleSHMIntList
{
    friend class CycleSHMIntHashIndex;

    //##ModelId=424432520367
    long m_lKey;

    //##ModelId=4244325B026F
    unsigned int m_iValue;

    //##ModelId=424432640377
    unsigned int m_iNext;

    int iIdleNext;//-1表示此节点已经被使用

};

//##ModelId=424423010070
//##Documentation
//## Key为整数的共享内存索引
//## 采用HASHLIST算法
class CycleSHMIntHashIndex : public SHMAccess
{
public:
    //##ModelId=424426D6039B
    CycleSHMIntHashIndex(ABMException &oExp, char *shmname);

    //##ModelId=424426D603B9
    CycleSHMIntHashIndex(ABMException &oExp, long shmkey);

    //##ModelId=424426D603D7
    ~CycleSHMIntHashIndex();

    //##ModelId=42442A29038B
    void add(long key, unsigned int value);

    //##ModelId=42442A5B0134
    bool get(long key, unsigned int *pvalue);

    bool erase(long key, unsigned int &value);

    void reset();

    //##ModelId=42442B02030B
    //##Documentation
    //## 创建数据区
    void create(ABMException &oExp, 
        //##Documentation
        //## 最多元素个数
        unsigned int itemnumber,

        unsigned int iPermission = 0777) ;

    //##清空索引，为数据业务内存交换使用
    void empty();

    //modify by jinx for app_guard 070402
    unsigned int getCount()
    {
        if (*m_piUsed)
            return (*m_piUsed);

        return 0;
    };

    unsigned int getTotal()
    {
        if (*m_piTotal)
            return (*m_piTotal);

        return 0;
    };

    long getValue(unsigned int key);

private:
    //##ModelId=4244D04101A3
    void open();

    //##ModelId=42442E01039A
    //##Documentation
    //## hash离散度
    unsigned int *m_piHashValue;

    //##ModelId=42442F710136
    unsigned int *m_piTotal;

    //##ModelId=42442F8A02FF
    unsigned int *m_piUsed;

    unsigned int *m_piHead;//头结点
    unsigned int *m_piTail;//尾接点

    //##ModelId=42442FA101B7
    unsigned int *m_poHash;

    //##ModelId=424432820167
    CycleSHMIntList *m_poList;

};




#endif /* CYCLESHMINTHASHINDEX_H_HEADER_INCLUDED_BDBB0F0A */
