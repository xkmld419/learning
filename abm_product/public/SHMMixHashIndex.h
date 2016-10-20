/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef SHMMIXINDEX_H_HEADER_INCLUDED_BDBB74AD
#define SHMMIXINDEX_H_HEADER_INCLUDED_BDBB74AD
#include "SHMAccess.h"
class SimpleSHM;

#define MAX_MIX_TYPE_LEN 	5

#define ATTACH_MIX_INDEX(X, Z) \
        X = new SHMMixHashIndex (Z); \
        if (!(X)) { \
            freeAll (); \
            THROW(MBC_UserInfo+1); \
        } \
        if (!(X->exist())) { \
            m_bAttached = false; \
        }


//## 这里要考虑内存8个字节对齐
class SHMMixStringList
{
public:
    unsigned int m_iValue;
    unsigned int m_iNext;
    //
    char m_sKey[8];
};

//
class SHMMixStringTree
{
public:
	//TREE的10个指向
    unsigned int m_iNext[10];
    unsigned int m_iValue;
};

//
class SHMMixTypeInfo
{
public:
	char m_sType[MAX_MIX_TYPE_LEN];
    unsigned int m_iKeyLen;
};

//## long以及字符串型共享内存索引，主要用于内存切换使用的索引，
//## 该类采用KeyTree算法，很耗空间,一般没有超过10^MIX_STRING_KEY_LEN次方的数据量,所以TREE结构体数目暂不设限
//## 大数据量的时候，建议使用SHMMixHashIndex。
class SHMMixHashIndex : public SHMAccess
{
  public:
    //
	SHMMixHashIndex(char *shmname);
	SHMMixHashIndex(long shmkey);
	SHMMixHashIndex(unsigned int iType,char *pAddr);
	SHMMixHashIndex(char *sType,char *pAddr);
	
	void add(long lKey,unsigned int value);
	bool get(long lKey, unsigned int *pvalue);
	char *m_sMixKey;
    //##ModelId=424429AC001A
    ~SHMMixHashIndex();

    //##ModelId=42442A8B0093
    void add(char *key, unsigned int value);

    //##ModelId=42442A9101C8
    bool get(char *key, unsigned int *pvalue);

    bool getMax(char *key, unsigned int *pvalue);
	
    //## 创建数据区
    void create(
        //## 最多元素个数
        unsigned int itemnumber, 
        //## key的最大长度 ,设置一个默认值主要为long类型的数据准备的
        unsigned int keylen=18, 
        //## keytree的节点数是元素数目的倍数,这个值在内存配置参数加载的时候设置要大 种类越多值越大
        unsigned int multiple = 3,unsigned int keyAddLen = 3,unsigned int reuseNum = 0, bool VarRay = false) ;
    void empty();
	//modify by jinx for app_guard 070402
	//这些是提供总的内存使用情况
	unsigned int getTCount();
	unsigned int getTTotal();
	unsigned int getlCount();
	unsigned int getlTotal();
	
	//这个用于提供对应m_iType数据获取总数以及实际使用量的函数
	unsigned int getCount(bool ReInitCount=false);
	unsigned int getTotal();
	
	unsigned int getType()
	{
		return m_iType;
	}
	bool typeExist();
public://实验功能
	//
	bool removeIndex(char *key); //最终对外提供的函数
	// 错误码
	unsigned int m_iErrNo;
	char m_sErrDesc[128];
	
	//复用功能 申请复用无效的数据空间
	unsigned int ReuseMalloc();
	//复用参数
	unsigned int  m_iMultiple;//以后作为临时变量
	unsigned int  m_iReuseListTotalNum;//以后作为临时变量
	unsigned int  m_iReuseTreeTotalNum;//临时变量
	// 获取一个可用的LIST节点
	SHMMixStringList *getList();
	// 获取一个可用的TREE节点
	SHMMixStringTree *getTree();
	// 申请可以复用的TREE地址
	unsigned int MallocReuseTree();
	// 申请可以复用的list地址
	unsigned int MallocReuseList();
	// 
	bool addReuseList(unsigned int iListOffset);
	bool addReuseTree(unsigned int iTreeOffset);
	// 
	bool deleteList(unsigned int iListOffset);
	bool deleteTree(unsigned int iTreeOffset);
	SHMMixStringTree *getTree(unsigned int offset);
  private:
  	char *m_pAddr;//地址
  	//
	char *m_sType;//以后这个为主
	unsigned int  m_iType;//以后作为临时变量
	
	unsigned int  m_iCount;// 提升搜索效率的
	bool m_bInitCount;//与m_iCount一起使用
    //
    void open(char *pAddr = 0);
    //
    inline SHMMixStringList *getList(unsigned int offset);
	
	
    //
    unsigned int *m_piListTotal;

    //##ModelId=4244332E0088
    unsigned int *m_piListUsed;

    //##ModelId=424433510223
    unsigned int *m_piTreeTotal;

    //##ModelId=4244335F02B9
    unsigned int *m_piTreeUsed;

    //## SHMStringList的真正长度
    unsigned int *m_piListLen;
	
	SHMMixTypeInfo *m_poTypeInfo;//暂时未使用
	unsigned int *m_piTypNum;//支持变长的最多类型数目 //暂时未使用
	
	//复用功能
	unsigned int *m_piReuseList;//记录可以复用的LIST的偏移
	//unsigned int *m_piListCurAdd;//这个可以取消了
	unsigned int *m_piListCurGet;
	unsigned int *m_piReuseListCanUse;
	unsigned int *m_piReuseListTotal;
	//
	unsigned int *m_piReuseTree;
	//unsigned int *m_piTreeCurAdd;//这个可以取消了
	unsigned int *m_piTreeCurGet;
	unsigned int *m_piReuseTreeCanUse;
	unsigned int *m_piReuseTreeTotal;
    //
    char *m_poList;

    //
    SHMMixStringTree *m_poTree;
    //
    unsigned int *m_piKeyLen;


};


#endif /*  */
