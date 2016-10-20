/*VER: 1*/ 
#ifndef _CONVERT_FUNC_H__
#define _CONVERT_FUNC_H__

#include "TicketTlv.h"
#include "Brtp.h"
#include "StdEvent.h"
#include "IDParser.h"


class ParseRule {

  public:
    int m_iTag;
    
    int m_iBeginPos;
    
    int m_iDataLen;
    
    int m_iFuncID;
    
    int m_iAttrID; //StdEvent的事件属性ID
    
    int m_iBitPosition; //bit 位位置
    
    ParseRule *m_pNext;
	
#ifndef PRIVATE_MEMORY
	int m_iParseGroupID;
	unsigned int m_iNextOffset;
#endif

  public:
#ifndef PRIVATE_MEMORY
    ParseRule () 
    {
        m_iTag=0; m_iBeginPos=0; m_iDataLen=0; m_iFuncID=0; m_iAttrID=0; 
		m_iParseGroupID=0; m_pNext=0; m_iNextOffset=0;
    };
#else
	ParseRule () 
    {
        m_iTag=0; m_iBeginPos=0; m_iDataLen=0; m_iFuncID=0; m_iAttrID=0; 
		m_pNext=0;
};
#endif
};





class ParseRuleMgr {

  public:

	#ifdef PRIVATE_MEMORY
    ParseRuleMgr(int iProcessID) 
    {
        m_iProcessID = iProcessID;
        initRule ();
    }
	#else
    ParseRuleMgr(int iProcessID) 
    {
        m_iProcessID = iProcessID;
    }
	#endif

	
	#ifdef PRIVATE_MEMORY
    ~ParseRuleMgr() {destoryRule ();}
	#else
	~ParseRuleMgr() {}
	#endif
    
    void initRule ();
    
    void destoryRule ();
    
    // 根据Tag位, 返回相应tag位对应的解析规则头指针
    ParseRule *getParseRule (int _iTag);
    
	#ifndef PRIVATE_MEMORY
	ParseRule *getHeadParseRule();
	#endif
  
  private:
    int m_iProcessID;


    ParseRule *m_pParseRule[MAX_TLV_FLAGS+1];


};





class ParseComb
{
	public:
		ParseComb();

		int m_iParseGroupID;
		int m_iProcessID;
		unsigned int m_iParseGroupOffset;
};

class ConvertMgr {

  public:
    static void  convTLV (int iTlvFlag, int iTlvLen, BIT8 *src, ParseRuleMgr *pRuleMgr,
                                TicketTlv *pTlv, StdEvent *pStd);
    
    static void  convert (BIT8 *src, Value *pValue, int _iFuncID, int _iLen, int _iBitPos);
    
  public:    
    // 将bin格式字段转换成为ASC, 转换为对应字符串输出
    static void  binToAscii (BIT8 * src,char * dest,int length);
        

    // 将 BCD 格式转换成 long
    static long   bcdToLong (BIT8 *src, int length);
    
    // 将 BIN 格式转换成 long
    static unsigned long   binToLong (BIT8 *src, int length);


    // 将 BIN 格式转换成 long
    static  long   binToLong2 (BIT8 *src, int length);
        
    // 将bcd格式转换为Ascii格式(左对齐)
    static void  bcdToAscii_Left_D (BIT8 * src,BIT8 * dest, int length);
    
    // 将bcd格式转换为Ascii格式(左对齐, 允许非数字字符)
    static void  bcdToAscii_Left_X (BIT8 * src,BIT8 * dest, int length);
    
    static int   getBitValue (unsigned char cValue, int _iBitPos);
    
    static long  asciiToLong (BIT8 *src, int length);
    
    // 将无符号的数值型直接转换成字符串 如数值 123 转换成字符串 "123"
    static void  longToString (BIT8 * src,BIT8 * dest, int length);

};

#endif

