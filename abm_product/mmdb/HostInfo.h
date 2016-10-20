 /*VER: 3*/
// Copyright (c) 20091214,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef HOSTINFO_H_HEADER_INCLUDED_BDAE6AE77
#define HOSTINFO_H_HEADER_INCLUDED_BDAE6AE77

class HostInfo
{
    public:
	       int  m_iHostID;
		   char m_sIP[50];
		   char m_sUserName[50];
		   char m_sPassword[50];
		   int  m_iListMode;
		   int  m_iOrgID;
}; 
#endif