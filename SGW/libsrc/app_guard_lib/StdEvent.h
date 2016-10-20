// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef STDEVENT_H_HEADER_INCLUDED_BDB42691
#define STDEVENT_H_HEADER_INCLUDED_BDB42691

#include "CommonMacro.h"
#include <string.h>


class StdEventExt
{
  public:
   

};


//##ModelId=41CF742F03B3
//##Documentation
//## 此结构采用系统所涉及的所有事件属性的全集，另外再加一部分保留字段。
class StdEvent
{
	public:
 		char m_sXMLTreeBuf[MAX_XML_TREE_BUF_LENGTH];	
 	 
 	 
	public:  //function
		void resetMember()
		{
			memset(m_sXMLTreeBuf,0,MAX_XML_TREE_BUF_LENGTH);
		}
}; 		


#endif /* STDEVENT_H_HEADER_INCLUDED_BDB42691 */




