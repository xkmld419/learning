/*VER: 1*/ 
#ifndef EVENTSECTION_H_HEADER_INCLUDED_BDA5C464
#define EVENTSECTION_H_HEADER_INCLUDED_BDA5C464

#include "StdEvent.h"
#include "UserInfoReader.h"

#include <string>

#include <iostream>
using namespace std;

class ExtendTempValue
{
  private:
    long  aLongValue[100];
    char  aStringValue[100][MAX_STRING_VALUE_LEN];

    bool  bModFlag;
    
  public:
    ExtendTempValue () { bModFlag = true; }
    
    void setLongValue (int iPos, long lValue)
    {
        if (iPos<0 || iPos>99) return;
        aLongValue[iPos] = lValue;        
        bModFlag = true;
    }
    void setStringValue (int iPos, char *sValue)
    {
        if (iPos<0 || iPos>99) return;
	int iLen =strlen(sValue)>(MAX_STRING_VALUE_LEN-1)?MAX_STRING_VALUE_LEN-1:strlen(sValue);
        strncpy (aStringValue[iPos], sValue,iLen);
        aStringValue[iPos][iLen]=0;
	
	//strcpy (aStringValue[iPos], sValue);	
	bModFlag = true;
    }
    
    long getLongValue (int iPos)
    {
        if (iPos<0 || iPos>99) return 0;
        return aLongValue[iPos];
    }
    void getStringValue (int iPos, char *sValue)
    {
        if (iPos<0 || iPos>99) return ;
        strcpy (sValue, aStringValue[iPos]);
    }
    
    void reset () {
        if (bModFlag) {
            memset (this, 0, sizeof (ExtendTempValue));
        }
    }
};

//##ModelId=4254DFF30253
class EventSection : public StdEvent
{
  public:
    //##ModelId=4254E05601AB
    static void reset();
        
    //##ModelId=4254E6D2022B
    EventSection *malloc();

    //##ModelId=4254E84C0007
    static EventSection *initMemory(int number);

    //##ModelId=425A154500A0
    static void freeMemory();
    
    //##ModelId=42647B21027B
    EventSection *insert();
    
    //##ModelId=4272EF3A0243
    void bindUserInfo(bool bRebind = false);
    
    void unbindUserInfo();

    void setUserInfo (Serv *pServ = 0x0);

    //##ModelId=4254E43100E2
    EventSection *m_poNext;
    //##ModelId=4266423B0286
    int m_iMeasureBegin;

	//客户个性化定价信息偏移
	unsigned int m_iCustPlanOffset;
        
  private:
    //##ModelId=4254E80D0396
    static int m_iTotalNumber;

    //##ModelId=4254E92A02BA
    static int m_iUsedNumber;

    //##ModelId=42672FE303C4
    static EventSection *m_poList;

  public:
    //扩展临时属性(模块内部使用,IDParser已支持)
    ExtendTempValue  m_oExtTempValue;
    
    //出流量溢出次数
    int  m_iOutOverTimes;
    
    //入流量溢出次数 
    int  m_iInOverTimes;
    
    //IPv6出流量溢出次数
    int  m_iOutOverTimesV6; 
    
    //IPv6入流量溢出次数
    int  m_iInOverTimesV6; 
    
    //中继不计费标识
    int  m_iTrunkNotBillFlag;
    
    //(排重域)主叫号码
    //<作为键树索引, 要求此属性不为空, 且是作为任何重单类型判断的"完全匹配"条件>
    //<在排重模块默认会置上 主叫区号+号码> ,与老版本保持一致
    char m_sDupChkCallingNBR[DUP_CHK_MAX_CALLING_LEN]; //目前定义41位(可用40位)
    
    //(排重域)被叫号码
    //<在排重模块默认会置上 被叫号码>, 与老版本保持一致
    char m_sDupChkCalledNBR[DUP_CHK_MAX_CALLED_LEN];   //目前定义41位(可用40位)
    
    //排重组合域
    char m_sMixStr[MAX_STRING_VALUE_LEN]; 
    //序列主键域 
	char m_sSeqKey[DUP_CHK_MAX_SEQKEY_LEN];
	//序列ID域
	char m_sSeqIDCode[DUP_CHK_MAX_SEQID_LEN];
    void resetExtAttrValue ()
    {
	    m_iOutOverTimes = 0;
	    m_iInOverTimes = 0;
	    m_iOutOverTimesV6 = 0;
	    m_iInOverTimesV6 = 0;
	    m_iTrunkNotBillFlag = 0;
	    m_sDupChkCallingNBR[0] = 0;
	    m_sDupChkCalledNBR[0] = 0;
	    m_sMixStr[0] = 0;
		m_sSeqKey[0] = 0;
		m_sSeqIDCode[0] = 0;
	   // memset(m_sMixStr,' ',sizeof(m_sMixStr));
	    
	    m_oExtTempValue.reset ();        
	 m_iBillingOrgIDEx_AD = 0;
        m_iBillingOrgIDEx_VS = 0;
        m_iCallingOrgIDEx_AD = 0;
        m_iCallingOrgIDEx_VS =0;
        m_iCalledOrgIDEx_AD =0;
        m_iCalledOrgIDEx_VS =0;
        m_iThirdOrgIDEx_AD =0;
        m_iThirdOrgIDEx_VS =0;
        m_iCalledCarrier = 0; 
	m_iOtherOrgIDEx_AD = 0;
	m_iOtherOrgIDEx_VS =0;
    }
	    //移动事件类型使用
    int m_iBillingOrgIDEx_AD;
    int m_iBillingOrgIDEx_VS;
    int m_iOtherOrgIDEx_AD;
    int m_iOtherOrgIDEx_VS;
    int m_iCallingOrgIDEx_AD;
    int m_iCallingOrgIDEx_VS;
    int m_iCalledOrgIDEx_AD;
    int m_iCalledOrgIDEx_VS;
    int m_iThirdOrgIDEx_AD;
    int m_iThirdOrgIDEx_VS;
    int m_iCalledCarrier;
};


#define SET_SECTION_VALUE(x, y) \
	*((StdEvent *)(x)) = *((StdEvent *)y)

#endif /* EVENTSECTION_H_HEADER_INCLUDED_BDA5C464 */
