#include "AssistProcess.h"
#include <iostream>
#include <algorithm>
#include "Date.h"

using namespace std;

extern int g_argcex;
extern char ** g_argvex;

DEFINE_MAIN_EX(CAssistProcess)
bool SysParamHisgreater ( const SysParamHis& elem1, const SysParamHis& elem2 )
{
	if(elem1.sDate[0]==0)
		return false;
	if(elem2.sDate[0]==0)
		return true;
   if(strcmp(elem1.sDate,elem2.sDate)>0)
	   return false;
   else
	   return true;
}


bool CAssistProcess::ChangeParam(const char* pSection,const char* pParam,
								 const char* pValue,
						const char* pDate)
{
	if((0==pSection)||(0==pParam)||(0==pValue))
		return false;
	string strContent;
	strContent.append(pSection);
	strContent.append(".");
	if(0!=pDate)
	{
		strContent.append(pParam);
		strContent.append(".");
		strContent.append(OLD_VALUE);
       		strContent.append("=");
        	strContent.append(pValue);
		strContent.append(",");
		strContent.append(pSection);
		strContent.append(".");
		strContent.append(pParam);
		strContent.append(".");
		strContent.append(NEW_VALUE);
		strContent.append("=");
		strContent.append(pValue);
		strContent.append(",");
		strContent.append(pSection);
		strContent.append(".");
		strContent.append(pParam);
		strContent.append(".");
		strContent.append(EFF_DATE_NAME);
		strContent.append("=");
		strContent.append(pDate);
	}else
	{
		strContent.append(pParam);
        strContent.append("=");
        strContent.append(pValue);
	}	
	variant vt;
	vt = strContent;
	cout<<strContent<<endl;
	CmdMsg* pMsg = CreateNewMsg(MSG_NEW_VALUE_EFFECT,"\\ROOT",vt);
	 if(!send(pMsg))
	 {
		 delete pMsg;pMsg=0;
		 return false;
	 }
	 delete pMsg;pMsg=0;
	return true;
}
CAssistProcess::CAssistProcess(void)
{
}

CAssistProcess::~CAssistProcess(void)
{
}


bool CAssistProcess::AddParam(SysParamHis &oSysParam)
{
	vector<SysParamHis>::iterator it = m_lstSysParamHis.begin();
	bool bFind=false;
	for(; it!=m_lstSysParamHis.end();it++)
	{
			if((strcmp((*it).sSectionName,oSysParam.sSectionName)==0)&&(strcmp((*it).sParamName,oSysParam.sParamName)==0))
			{
				strcpy((*it).sParamValue,oSysParam.sParamValue);
				cout<<(*it).sParamValue<<endl;
				strcpy((*it).sParamValue,oSysParam.sParamOrValue);
				cout<<(*it).sParamValue<<endl;
				strcpy((*it).sDate,oSysParam.sDate);
				cout<<(*it).sDate<<endl;
				bFind=true;
			}
	}
	if(!bFind)
		m_lstSysParamHis.push_back(oSysParam);
	sort( m_lstSysParamHis.begin(),m_lstSysParamHis.end(),SysParamHisgreater);
	return true;
}

void CAssistProcess::GenerateParamClock(void)
{
	if(m_lstSysParamHis.empty())
		return ;
	sort( m_lstSysParamHis.begin(), m_lstSysParamHis.end(),SysParamHisgreater);
	vector<SysParamHis>::iterator it = m_lstSysParamHis.begin();
	Date d1,d2;
	for(;it!= m_lstSysParamHis.end();it++)
	{
		 GetInitParam((*it).sSectionName);
                SubscribeCmd((*it).sSectionName,MSG_PARAM_CHANGE);
	//	SubscribeCmd((*it).sSectionName,MSG_NEW_VALUE_EFFECT);
	}
	while(!m_lstSysParamHis.empty())	
	{
		SysParamHis oParamHis =*m_lstSysParamHis.begin();
		d2.parse(oParamHis.sDate);
		if(d1>d2)
		{
			if(((oParamHis).sParamOrValue[0]!=0)&&(strcmp((oParamHis).sParamValue,(oParamHis).sParamOrValue)!=0))
			{
				ChangeParam((oParamHis).sSectionName,(oParamHis).sParamName,(oParamHis).sParamOrValue,(oParamHis).sDate);
			}
			m_lstSysParamHis.erase(m_lstSysParamHis.begin());
			continue;
		}else
			break;
	}
}

int CAssistProcess::run()
{
	if(!InitClient())
	{
		//注册服务端失败
		cout<<"InitClient Error"<<endl;
		return 0;
	}
	CmdMsg* pMsg = CreateNewMsg(MSG_PARAM_S_TIMING,"\\ROOT");
	
	 GetInitParam("system");
	  SubscribeCmd("system",MSG_PARAM_CHANGE);
	 if(!send(pMsg))
	 {
		 return 0;
	 }
	 delete pMsg; pMsg=0;
	 int iSleepTimes=10;
	 while(iSleepTimes--)
	 {
		 if(!(pMsg = receive()))
		 {
			 sleep(1);
		 }else
		 {
			 //需要增加是否改消息返回的
			 long lCmd = pMsg->GetMsgCmd();
			 if(pMsg->GetMsgCmd()==MSG_OP_RETURN)
			 {
				 //判断消息类型
				 variant vt ;
				 pMsg->GetMsgContent(vt);
				 if(vt.isPtr())
				 {
					 unsigned int iCnt = pMsg->GetVariantSize(vt);
					 SysParamHis* p = (SysParamHis*)vt.asPtr();
					 SysParamHis sTemp;
					 sTemp =*p;
					 assert(iCnt%sizeof(SysParamHis)==0);
					 iCnt=iCnt/(sizeof(SysParamHis));
					 while(iCnt-->0)
					 {
						 //需要判断是否重复
						 m_lstSysParamHis.push_back(sTemp);
						 p++;
						 sTemp=*p;
					 }
				 }
				 break;
			 }
		 }
	 }
	 delete pMsg; pMsg=0;
	GenerateParamClock();
	 vector <SysParamHis> vecParamHis;
	 while(1)
	 {
		 sleep(1);
		 vecParamHis.clear();
		 if(FlushParam(&vecParamHis))
		 {
			 vector <SysParamHis>::iterator it = vecParamHis.begin();
			 for( ;it!= vecParamHis.end();it++){
			 	if(strcmp((*it).sSectionName,"system")==0)
			 		{
			 			if(strcmp((*it).sParamName,"state")==0)
			 				{
			 					if(strcmp((*it).sParamValue,"online")==0)
			 						{
			 							Date d1;
			 							string strChange="system.billing_online_time=";
			 							strChange.append(d1.toString());
			 							variant vt = strChange;
			 							CmdMsg* pMsg = CreateNewMsg(MSG_PARAM_CHANGE,"system",vt);
	 									if(!send(pMsg))
	 									{
		 									delete pMsg;pMsg=0;
		 									return false;
	 									}
	 									delete pMsg;pMsg=0;
			 						}						
			 				}
			 		}
				 AddParam(*it);
				}
		 }
		 if(!m_lstSysParamHis.empty())
		 {
			 SysParamHis oParamHis =*m_lstSysParamHis.begin();
			 Date d1,d2;
			 d2.parse(oParamHis.sDate);
			 if(d1>d2)
			 {
				 ChangeParam(oParamHis.sSectionName,oParamHis.sParamName,oParamHis.sParamOrValue,oParamHis.sDate);
				 m_lstSysParamHis.erase(m_lstSysParamHis.begin());
			 }
		 }
	 }
}
