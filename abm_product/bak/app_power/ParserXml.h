#ifndef __PARSERXML_H__
#define __PARSERXML_H__

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include <unistd.h>
#include <atmi.h>               /* TUXEDO  Header File */
#include <tx.h>             /* TUXEDO  Header File */
#include <fml32.h>      /* TUXEDO  Header File */
#include <sys/timeb.h>

using namespace std;

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/TranscodingException.hpp>
#include <xercesc/dom/DOM_DOMException.hpp>
#include <xercesc/parsers/DOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOM_Node.hpp>
#include <xercesc/util/XMLString.hpp>

//#include "PowerQuery.h"
//#include "PowerXml.h"
//#include "PowerFile.h"
#include "power_comm.h"

#ifdef XERCES_CPP_NAMESPACE_USE
XERCES_CPP_NAMESPACE_USE
#endif


class ParserXml
{
	public:
	  ParserXml();
	 ~ParserXml();

		void GetNode(DOM_Node node,int iDataSource);
		int  TopTree(DOM_Node Node,int iDataSource);

		int  ProcessMessage(char* xmlbuf,int iDataSource);
		void SetValue(char *parent_name,char *sChFieldVaue,int iDataSource);
		void initall();
		
		//void PackXmlinfo(char *m_sState,int nRet,char *retbuf);
		void PackXmlinfo(char *sBusiName,char *sPackName,char *sState,RespMsg &pResp,char *retbuf);

		void PackXmlLastTotalinfo(long mSumAmount,RespMsg &pResp,char *retbuf);
		void PackXmlFluxinfo(vector<GprsAmount> &v_Vec,RespMsg &pResp,char *retbuf);
		void GetRespCode(int nRet,char *sRet);
		
		void PackXmlHLRformnum(char * formnum,RespMsg &pResp,char *retbuf);
		void PackXmlHLRInfo(HlrFormnum &hlr,RespMsg &pRespt,char *retbuf);
		void PackXmlForHLR(char *sAccNbr,int iAreaID,char *sStaffID,char *retbuf);
		void GetCdmaStatus(char *sStatus);

	public:
		char sFileName[40];
		OperationIn m_pin;
		HLRMSG	m_Hlr;
};

class StrFormatTarget:public XMLFormatTarget
{
public:
	 char * GetResult(char * retbuf);
    StrFormatTarget()  {};
    ~StrFormatTarget () {};
    void writeChars(const   XMLByte* const  toWrite,
                    const   unsigned int    count,
                    XMLFormatter * const formatter);
private:
	char * buffer;
   StrFormatTarget (const StrFormatTarget & other);
   void operator=(const StrFormatTarget & rhs);
};


class StrTransformer
{
public:
	StrTransformer(char * encoding);
	~StrTransformer();
	char *ChangeStr(const DOMString& s,char * retbuf);
	char *ChangeStr(const XMLCh * str,char * retbuf);
	XMLFormatter * format;
	StrFormatTarget * target;
	DOMString * m_dom;
};



#endif //__PARSERXML_H__
