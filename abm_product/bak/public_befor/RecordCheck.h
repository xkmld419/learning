/*VER: 4*/
#ifndef TIBS__RECORDCHECK_H_
#define TIBS__RECORDCHECK_H_

#include "HashList.h"
#include "EventSection.h"
#include "CommonMacro.h"
using namespace std;

class CheckRuleInfo;
#ifndef PRIVATE_MEMORY
    class ParamInfoMgr;
#endif

class CheckArgument
{
	friend class CheckRuleInfo;
#ifndef PRIVATE_MEMORY
	friend class ParamInfoMgr;
#endif	
public:
    CheckArgument();	
    ~CheckArgument();
    
public:
    int m_iLocation;    //参数在函数中的位置（起始位置为1）
    char m_sValue[50];  //参数值，统一为字符串类型   
    
    //下一个位置的第一个参数
    CheckArgument *m_poNextSibArgument;

    //同一个位置的下一个参数
    CheckArgument *m_poNextArgument;
    
    //test code
    void showMe(int iLevel=0);
#ifndef PRIVATE_MEMORY
	unsigned int m_iNextSibArgument;
	unsigned int m_iNextArgument;
#endif

    
private:
    int m_iArgID;       //参数ID
    int m_iSubRuleID;   //本参数所归属的子规则ID
        
};

class CheckSubRule
{	
	friend class CheckRuleInfo;
	friend class CheckRule;
#ifndef PRIVATE_MEMORY
	friend class ParamInfoMgr;
#endif
public:
    CheckSubRule();    
    ~CheckSubRule();

	//执行本子规则，如果校验通过，即没有错误发生，则返回true
	bool execute(const EventSection *poEventSection, int iTAG);
    
//private:
    int m_iSubRuleID;   //子规则ID
    int m_iRuleID;      //本子规则所归属的规则ID
    int m_iFuncID;      //执行本子规则所用到的函数ID
    int m_iCheckSeq;    //执行校验的次序（从小到大）
    int m_iNOT;         //否定标识

    //下一个次序较大的子规则，这个子规则也归属于同一个规则
    CheckSubRule *m_opNextSubRule;

    //指向子规则的第一个参数
    CheckArgument *m_opFirstAgrument;
#ifndef PRIVATE_MEMORY	
	unsigned int m_iNextSubRule;
	unsigned int m_iFirstAgrument;
#endif	
    
public:
    //test code
    void showMe(int iLevel=1);
};


class CheckRule
{
	friend class CheckRuleInfo;
#ifndef PRIVATE_MEMORY
	friend class ParamInfoMgr;
	friend class RecordCheck;
#endif	
public:
	CheckRule();	
	~CheckRule();

	//执行本规则，如果校验通过，即没有错误发生，则返回true
	//bCheckWarning是一个参数开关，用于控制是否对警告级别的规则执行校验，默认执行
	bool execute(EventSection *poEventSection, char *sAllowNull, int iBillType,int iRecordType,bool bCheckWarning=true );
	
//private:	
    int m_iRuleID;         //规则ID
    int m_iTAG;            //TAG值
    int m_iErrCode;        //错误代码
    int m_iErrLevel;       //错误级别 1:Fatal-致命错误 2:severe-严重错误 2:warning-警告
    
    //指向第一个子规则
    CheckSubRule *m_opFirstSubRule;
    
#ifndef PRIVATE_MEMORY
	unsigned int m_iFirstSubRule;
#endif    
public:
    //test code
    void showMe(int iLevel=1);
    
};

class CheckRelation
{
	friend class CheckRuleInfo;
	friend class RecordCheck;
#ifndef PRIVATE_MEMORY
	friend class ParamInfoMgr;
#endif	
public:
    CheckRelation();
    ~CheckRelation();
        
//private:
    int m_iRelationID;   //校验关系ID
    int m_iRuleID;       //规则ID
    int m_iBillType;     //话单类型，如：1省际漫游语音话单、2国际漫游语音话单
    int m_iRecordType;   //记录类型，如：对于语音话单，1主叫、2被叫、3呼转
    int m_iCheckSeq;     //执行校验的次序（从小到大）
    char m_sAllowNull[2];  //在本规则中空值是否算正确

	//指向m_iRuleID对应的校验规则
	CheckRule *m_poCheckRule;

    //指向兄弟话单类型的第一个校验关系
    CheckRelation *m_poSibBillRelation;

    //指向兄弟记录类型的第一个校验关系
    CheckRelation *m_poSibRecordRelation;

    //指向相同类型（即话单类型、记录类型都相同）的下一个次序较大的检验关系
    CheckRelation *m_poNextRelation;
    
#ifndef PRIVATE_MEMORY	
	unsigned int m_iCheckRule;
	unsigned int m_iSibBillRelation;
	unsigned int m_iSibRecordRelation;
	unsigned int m_iNextRelation;
#endif	
public:
    //test code
    void showMe(int iLevel=1);
    
};

//校验相关的表都由这个类来负责加载
class CheckRuleInfo
{
public:
    CheckRuleInfo();	
    ~CheckRuleInfo();
    
    static void load();
	
    static void unload();
	
public:
    static HashList<CheckRule *> *s_poRuleIndex;       
	static HashList<CheckSubRule *> *s_poSubRuleIndex;
	static HashList<CheckArgument *> *s_poArgumentIndex;  

    static CheckRule *s_poRuleList;
	static CheckSubRule *s_poSubRuleList;
	static CheckArgument *s_poArgumentList;
	static CheckRelation *s_poRelationList;
	static int nRuleSize;
	static int nSubRuleSize;
	static int nArgumentSize;
    static int nRelationSize;

private:
	static bool s_bUploaded;		//全部上载完成，置true
	static bool s_bRuleUploaded;    //Rule上载完成，置true
	static bool s_bSubRuleUploaded; //类上
	static bool s_bArgumentUploaded; 
	static bool s_bRelationUploaded;

	static void loadRule();
	static void loadSubRule();
	static void loadArgument();
	static void loadRelation();

	static void unloadRule();
	static void unloadSubRule();
	static void unloadArgument();
	static void unloadRelation();
	
public:
	//test code
	static void showMe()
	{
	    int i=0;
	    printf("\nRelation\n");
	    CheckRelation *pBill, *pRecord;
	    if (nRelationSize>0){
	        pBill = &s_poRelationList[0];
	        while (pBill!=NULL && i<nRelationSize){
	            
	            int j = 1;
	            pRecord = pBill;
	            while (pRecord!=NULL){
	                pRecord->showMe(j++);
	                pRecord = pRecord->m_poSibRecordRelation;
	                i++;
	            }
	            pBill = pBill->m_poSibBillRelation;
	        }
	            
	    }  
	    printf("共%d个校验关系\n", nRelationSize);
	    
	   i = 0;	   
	   printf("\nRule\n");
	   while (i < nRuleSize){
	        s_poRuleList[i++].showMe(1);
	   }
	   printf("共%d个校验规则\n", i);
	   
	   i=0;
	   printf("\nSubRule\n");
	   while (i < nSubRuleSize){
	        s_poSubRuleList[i++].showMe(1);
	   }
	   printf("共%d个子规则\n", i);
	   
	   i=0;
	   printf("\nArgument\n");
	   while (i < nArgumentSize){
	        s_poArgumentList[i++].showMe(1);
	   }
	   printf("共%d个参数\n", i);
	   
	}

};    

//向外提供话单检验的接口
class RecordCheck
{
public:
	RecordCheck();
	~RecordCheck();
	
	static bool check(EventSection *poEventSection, bool bCheckWarning=true );

public:
	static int getBillType(EventSection *poEventSection);
	static int getRecordType(EventSection *poEventSection);
	
};

class TransFileMgr;

class TransFileType
{
friend class TransFileMgr;
#ifndef PRIVATE_MEMORY
	friend class ParamInfoMgr;
#endif	
public:
    TransFileType():m_iTypeID(0){
           memset(m_sTypeName, 0, sizeof(m_sTypeName));
           memset(m_sPath, 0, sizeof(m_sPath));
           memset(m_sTmpPath, 0, sizeof(m_sTmpPath));
    }
    
    ~TransFileType(){};
    
//private:
    
    //trans_file_type_id
    int  m_iTypeID;
    
    //trans_file_type_name
    char m_sTypeName[64];
    
    //file_path
    char m_sPath[MAX_FILE_NAME_LEN];
    
    //temp_file_path
    char m_sTmpPath[MAX_FILE_NAME_LEN];

    //错误文件是否需要头记录
    int m_iErrHead;

    //错误文件是否需要尾记录
    int m_iErrTail;

//是否需要写错误文件，add by yuanp 20100331
    int m_iWriteFile;
        
};

class TransFileMgr
{
public:
    TransFileMgr(){};
    ~TransFileMgr(){};
    
    static void load();
    static void unload();
    
    static const char* getFileTypeName(int iTypeID);
    static const char* getFileTypePath(int iTypeID);
    static const char* getFileTypeTmpPath(int iTypeID);
    
    //return iTypeID
    static int getFileTypeID(const char *sFileTypeName);
    
    static const char* getFileTypeName(const char *sFileTypeName);
    static const char* getFileTypePath(const char *sFileTypeName);
    static const char* getFileTypeTmpPath(const char *sFileTypeName);
    //add 
    static int getFileErrHead(const char *sFileTypeName);
    static int getFileErrHead(int iTypeID);
    static int getFileErrTail(const char *sFileTypeName);
    static int getFileErrTail(int iTypeID);
    static int getWriteErrFile(int iTypeID);
    static int getWriteErrFile(const char *sFileTypeName);
    
private:
    static int nSize;
    static bool s_bUploaded;
    
    static HashList<TransFileType *> *s_poFileTypeIndex;
    static TransFileType *s_poFileTypeList;
};

class TagSeqMgr;

class TagSeq
{
friend class TagSeqMgr;
#ifndef PRIVATE_MEMORY
	friend class ParamInfoMgr;
#endif
public:
    TagSeq()
    :   m_iGroupID(0), m_iTAG(0), m_iAttrID(0), m_iSeq(0),
        m_poNextSibTagSeq(NULL), m_poNextTagSeq(NULL)
    {
        memset(m_sName, 0, sizeof(m_sName));
    }
    ~TagSeq(){};
    
    const TagSeq * nextSib() const
    {   
        return m_poNextSibTagSeq;
    }
    
    const TagSeq * next() const
    {   
        return m_poNextTagSeq;
    }
    
    int getAttrID() const
    {
        return m_iAttrID;
    }
    
    int getTag() const
    {
        return m_iTAG;
    }
    
    int getSeq() const
    {
        return m_iSeq;
    }
    
    const char * getName() const
    {
        return m_sName;
    }
    
    int getGroupID() const
    {
        return m_iGroupID;
    }
    
    void showMe(int iLevel=1);
private:
    int m_iGroupID;
    int m_iTAG;
    int m_iAttrID;
    int m_iSeq;
    char m_sName[50];
    
    TagSeq *m_poNextSibTagSeq;  //指向下一个GroupID的首个TagSeq.
    TagSeq *m_poNextTagSeq;     //指向同组的下一个TagSeq;
#ifndef PRIVATE_MEMORY	
	unsigned int m_iNextSibTagSeq;
	unsigned int m_iNextTagSeq;
#endif		
};

class TagSeqMgr
{
public:
    TagSeqMgr(){}
    
    ~TagSeqMgr(){}

    static void load();
    static void unload();
    
    static const TagSeq *getFirstTagSeq(int iGroupID);
    static int getArrtID(int iGroupID, int iTag);
    
    //test code
    static void showMe();
private:
    static int nSize;
    static bool s_bUploaded;
    static TagSeq *s_poTagSeqList;
};

#endif /* End of TIBS__RECORDCHECK_H_  */
