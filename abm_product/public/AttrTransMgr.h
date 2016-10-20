/*VER: 1*/ 
// // Copyright (c) 2005,联创科技股份有限公司电信事业部
// // All rights reserved.

#ifndef ORGATTRFORMATMGR_H_HEADER_INCLUDED_BDB33138
#define ORGATTRFORMATMGR_H_HEADER_INCLUDED_BDB33138

#include "BillConditionMgr.h"
#include "BillOperationMgr.h"
#include "StdEvent.h"
#include "TOCIQuery.h"
#include "Environment.h"
#include "FormatMethod.h"
#include <iostream>

#define RULE_EXIT 1
#define RULE_NOT_EXIT 0
#define MAX_RULE_SQL_LEN 1000

const int FORMART_NORMAL = 0;

//##ModelId=4250AF2E0300
//##Documentation
//## 属性规整规则
class FormatRule
{
  public:
    //##ModelId=42595EB50064
    FormatRule();

    //##ModelId=4250E5BB022E
    int m_iRuleSeq;

    //##ModelId=4250E62C00F0
    int m_iConditionID;

    void * m_poCondition;

    //##ModelId=4250E63D0325
    int m_iOperationID;

    void * m_poOperation;

    //##ModelId=4250E93E0082
    int m_iInEventType;

    //##ModelId=4250E9640145
    int m_iOutEventType;

    //##ModelId=4250E9F703D1
    //##Documentation
    //## 0：不退出  1：退出规整
    int m_iExitFlag;
    
    char m_sRuleName[80];

    //##ModelId=4250EA30004E
    char m_sEffDate[15];

    //##ModelId=4250EA5501B0
    char m_sExpDate[15];

    //##ModelId=4250EC840120
    FormatRule *m_poNext;


};

//##ModelId=4250AF01003E
//##Documentation
//## 属性规整步骤
class FormatStep
{
  public:
    //##ModelId=42595E9D0240
    FormatStep();

    //##ModelId=4250EB1A03C6
    int m_iStepSeq;

    //##ModelId=42510D3801C6
    int m_iFormatID;
    
    char m_sStepName[80];
    
    //##ModelId=4250EC300132
    FormatRule *m_poFormatRule;
    
    //##ModelId=4250EC30013C
    FormatStep *m_poNext;

#ifndef PRIVATE_MEMORY
    unsigned int m_iFormatRuleOffset;
    unsigned int m_iFormatRuleCnt;
#endif

};


//##ModelId=42548B92033E
//##Documentation
//## 属性规整组
class FormatGroup
{
  public:
    //##ModelId=42595D3E024F
    FormatGroup();

    //##ModelId=42548CF80299
    int m_iFormatID;

    
    //##ModelId=4254C3090226
    FormatGroup *m_poNext;
    
    //##ModelId=4254C3950351
    FormatStep *m_poFormatStep;

#ifndef PRIVATE_MEMORY
    unsigned int m_iFormatStepOffset;
    unsigned int m_iFormatStepCnt;
#endif

};


//##ModelId=4259FE4A019E
//##Documentation
//## 属性规整组合,一个进程可对应多个属性规整(组),分别用来处理不同的事件类型(语音/数据等)
class FormatComb
{
  public:
    //##ModelId=4259FEC102E9
    FormatComb();

    //##ModelId=4259FECD01A6
    //##Documentation
    //## 源事件类型
    int m_iSourceEventType;

    //##ModelId=4259FF1C0222
    int m_iFormatID;

    //##ModelId=4259FF780333
    FormatGroup *m_poFormatGroup;
    
    //##ModelId=425A00DF00B8
    FormatComb *m_poNext;

#ifndef PRIVATE_MEMORY
    int m_iProcessID;
    unsigned int m_iFormatGroupOffset;
#endif

};

//##ModelId=41CF5D0E0355
//##Documentation
//## 原始属性规整是根据配置的业务规则来求取和整理事件属性。事件属性的求取最终目的是为了求取事件类型。
//## 原始属性规整调用参数接口类获取基本参数和处理规则，调用公共类
//## BillConditionMgr和BillOperationMgr进行条件和运算的获取、判断以及调用公
//## 共的底层运算函数进行操作处理，对于复杂的业务处理函数调用
//## FormatMethod提供的方法。
//## 在处理过程中，如果发现异常则置事
//## 件错误号。
//## 
//## 
//## 对事件属性的操作函数分为这样几类：
//## 1。满足一定的条件进行直接赋值、关系运算和简单的业务处理，这样的操作步骤不需要太多，
//##    由底层公共类完成；
//## 2。复杂的业务处理需要查询参数表并且表内容很多，由各个模块提供函数。
//##    所有的操作都是由后台提供相应的处理方式，公共部分由底层公共部分
//## 完成，复杂的业务处理由各个模块实现。系统提供关于所有操作的说明，
//## 用户在前台配置整个属性规整的处理流程，选择需要的操作函数。
//## 
//## 
class AttrTransMgr
{
  public:
    //##ModelId=42510090028F
    AttrTransMgr(int iProcessID);

    //##ModelId=41EF8BBF026F
    //##Documentation
    //## 对当前事件进行属性规整
    //## 事件规整正常返回0
    //## 事件规整异常返回异常编码
    int trans(StdEvent* pStdEvent);

    //##ModelId=4250DD97000B
    //##Documentation
    //## 上载/重新上载属性规整规则
    void reloadRule();
    
    //##ModelId=4250F0750395
    void unloadRule();
    
    //##ModelId=425952FB005D
    void printRule();
    
    static void printEvent(StdEvent *_StdEvent);

    //##ModelId=4259E98B002E
    ~AttrTransMgr();


  private:
    //##ModelId=425123D60045
    //##Documentation
    //## 有错误，返回错误号
    //## 无错误，返回0
    int checkAttrErr();

	int existChar(const char * in_string);
	
    //##ModelId=425124ED019B
    //##ModelId=425124ED019B
    //##Documentation
    //## 有错误，返回错误号
    //## 无错误，返回0
    int checkTimeEffect(char *sTime);
    
    //##ModelId=425104F40277
    int m_iProcID;
    
    //##ModelId=4254BD4E0261
    StdEvent* m_poStdEvent;
    //##ModelId=4254C0170125
    
    //##Documentation
    //## 属性规整组合头指针
    FormatComb * m_poFormatComb;
    
    //##ModelId=425A087C0056
    //##Documentation
    //## 属性规整组头指针
    FormatGroup *m_poFormatGroup;
    
    int m_iNeedSavFormatRec;
};



#endif /* ORGATTRFORMATMGR_H_HEADER_INCLUDED_BDB33138 */

