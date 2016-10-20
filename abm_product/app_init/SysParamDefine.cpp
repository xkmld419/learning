#include "SysParamDefine.h"
#include <memory.h>
#include <iostream>
using namespace std;
SysParamHis::SysParamHis()
{
	memset(sSectionName,0,sizeof(sSectionName));
	memset(sParamName,0,sizeof(sParamName));
	memset(sParamValue,0,sizeof(sParamValue));
	memset(sParamOrValue,0,sizeof(sParamOrValue));
	memset(sDate,0,sizeof(sDate));
}

SysParamHis::SysParamHis(const SysParamHis& param)
{
	memcpy(sSectionName,param.sSectionName,sizeof(sSectionName));
	memcpy(sParamName,param.sParamName,sizeof(sParamName));
	memcpy(sParamValue,param.sParamValue,sizeof(sParamValue));
	memcpy(sParamOrValue,param.sParamOrValue,sizeof(sParamOrValue));
	memcpy(sDate,param.sDate,sizeof(sDate));
}
SysParamHis& SysParamHis::operator =(const SysParamHis& param)
{
	memcpy(sSectionName,param.sSectionName,sizeof(sSectionName));
	memcpy(sParamName,param.sParamName,sizeof(sParamName));
	memcpy(sParamValue,param.sParamValue,sizeof(sParamValue));
	memcpy(sParamOrValue,param.sParamOrValue,sizeof(sParamOrValue));
	memcpy(sDate,param.sDate,sizeof(sDate));
	return *this ;
}
bool splitParamInfo(string& strContent,
									 string& strSection,	string& strParam,string& strValue)
{
	if(strContent.empty())
		return false;

	//去头尾空格
	strContent.erase(0,strContent.find_first_not_of(' '));
	strContent.erase(strContent.find_last_not_of(' ')+1);

	string strTmp ;

	int iPosBegin = strContent.find_first_of(',');
	strTmp.append(strContent,0,iPosBegin);
	if(strTmp.empty())
		return false;
	if(-1!= iPosBegin)
		strContent.erase(0,iPosBegin+1);
	else
		strContent.clear();

	iPosBegin = strTmp.find_first_of('.');
	strSection.append(strTmp,0,iPosBegin);
	if(strSection.empty())
		return false;
	if(-1==iPosBegin)//只有section;
		return true;

	int iPosEnd = strTmp.find_last_of('=');
	if(-1 == iPosEnd)
	{
		strParam.append(strTmp,iPosBegin+1,strTmp.length()-iPosBegin);
		return true;
	}
	if(iPosEnd<=iPosBegin)
		return false;
	strParam.append(strTmp,iPosBegin+1,iPosEnd-iPosBegin-1);
	strValue.append(strTmp,iPosEnd+1,strTmp.length()-iPosEnd);

	/*if((strParam.empty())||(strValue.empty()))
		return false;*/
	return true;
}
void TransIniKeyToSysParamHis(SysParamHis& oSysParamHis,stIniCompKey& oIniCompKey)
{
	strcpy(oSysParamHis.sSectionName,oIniCompKey.sSectionName);
	strcpy(oSysParamHis.sParamName,oIniCompKey.sKeyName);
	strcpy(oSysParamHis.sParamValue,oIniCompKey.sKeyValue[0]);
	if(oIniCompKey.sEffDate[0]!=0) {
		strcpy(oSysParamHis.sDate,oIniCompKey.sEffDate);
		strcpy(oSysParamHis.sParamOrValue,oIniCompKey.sKeyValue[1]);
	}else
		strcpy(oSysParamHis.sParamOrValue,oIniCompKey.sKeyValue[MAX_STACK_LEN-1]);
}
void badalloc(bad_alloc &ba)
{
	    cout<<ba.what()<<endl;
		  exit(0);
}

