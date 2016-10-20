/*VER: 2*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部 
// All rights reserved.

#include "LocalHead.h"
#include <iostream>
using namespace std;

#include "Environment.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

//##ModelId=42803ACA03E7
#ifdef PRIVATE_MEMORY
LocalHeadMgr::LocalHeadMgr()
{
	loadLocalHead();
}
#else
LocalHeadMgr::LocalHeadMgr()
{
}
#endif

//##ModelId=42803ADA030E
LocalHeadMgr::~LocalHeadMgr()
{
//	unloadLocalHead();
}

//##ModelId=42803AE60329
void LocalHeadMgr::loadLocalHead()
{
	if (m_poLocalHead)
		return;
	
	cout<<"\nInit Local_Head ..."<<endl;
	
	m_poLocalHead = new KeyTree<LocalHead*>;
	
	LocalHead* pLocalHeadTmp =0;

	DEFINE_QUERY(qry);
		
	qry.close();
	qry.setSQL(" select org_id,area_code,head,nvl(len,0) len, "
		" to_char(eff_date,'yyyymmddhh24miss') eff_date, "
		" to_char(exp_date,'yyyymmddhh24miss') exp_date "
		" from local_head  order by len" );
	qry.open();
	
	char sKey[MAX_AREA_CODE_LEN+MAX_ACCNBR_LEN];
	while (qry.next()) {
		LocalHead* pLocalHead = new LocalHead();
		pLocalHead->m_iOrgID = qry.field("org_id").asInteger();
		pLocalHead->m_iLen = qry.field("len").asInteger();
		strcpy(pLocalHead->m_sAreaCode, qry.field("area_code").asString());
		strcpy(pLocalHead->m_sHead, qry.field("head").asString());
		strcpy(pLocalHead->m_sEffDate, qry.field("eff_date").asString());
		strcpy(pLocalHead->m_sExpDate, qry.field("exp_date").asString());
		
		sprintf(sKey,"%s-%s",pLocalHead->m_sAreaCode,pLocalHead->m_sHead);
		if((m_poLocalHead->get(sKey,&pLocalHeadTmp))&&(pLocalHeadTmp !=0))
		{
			//已经有值
			pLocalHead->m_pNext = pLocalHeadTmp;
		}
		m_poLocalHead->add(sKey, pLocalHead);
	}
	
	qry.close();
	qry.setSQL(" select max(length(head)) max_len, min(length(head)) min_len from local_head ");
	qry.open();
	if (qry.next()) {
		m_iMaxHeadLen = qry.field("max_len").asInteger();
		m_iMinHeadLen = qry.field("min_len").asInteger();
	}
	qry.close();
	
	cout<<"Local_Head inited.\n"<<endl;
}

//##ModelId=42803AF3021A
void LocalHeadMgr::unloadLocalHead()
{
	if (!m_poLocalHead)
		return;
	
	delete m_poLocalHead;
	
	m_poLocalHead = NULL;
}

//##ModelId=42803B8001A4
#ifdef PRIVATE_MEMORY
bool LocalHeadMgr::searchLocalHead(const char *_sAccNbr, const char *_sAreaCode, const char *_sTime, 
		LocalHead *_pLocalHead)
{
	if (!_pLocalHead)
		return false;
	
	if (!m_poLocalHead)
		loadLocalHead();
	
	char sTempAccNbr[MAX_ACCNBR_LEN+1];
	char sKey[MAX_AREA_CODE_LEN+MAX_ACCNBR_LEN];
		
	strncpy(sTempAccNbr,_sAccNbr,MAX_ACCNBR_LEN);
	sTempAccNbr[MAX_ACCNBR_LEN] = 0;

	int iAccNbrLen = strlen(sTempAccNbr);
	
	int iCmpNbrLen = iAccNbrLen<m_iMaxHeadLen ? iAccNbrLen:m_iMaxHeadLen;

    sTempAccNbr[iCmpNbrLen] = 0;
	LocalHead* pLocalHead =0;

    sprintf(sKey,"%s-%s",_sAreaCode,sTempAccNbr);

    int iPos = 1;
	
	while (iPos) {
		
		int iMatchLen;

		if (m_poLocalHead->getMax(sKey,&pLocalHead, &iMatchLen)) {			
			// 不在生效时间范围内
			while(0!= pLocalHead)
			{
				if(((pLocalHead->m_iLen == iAccNbrLen)||(0 == pLocalHead->m_iLen))&&
					(strcmp(_sTime,pLocalHead->m_sEffDate)>=0)
					&& (strcmp(_sTime,pLocalHead->m_sExpDate)<0))
				{
					//大于等于生效时间小于失效时间
					*_pLocalHead = *pLocalHead;
					return true;
				}else
				{
					pLocalHead = pLocalHead->m_pNext;
					continue;
				}
			}
			iPos = iMatchLen - 1;
			sKey[iPos] = 0;
		} else {
			return false;
		}
	}

	return false;
}
#else
bool LocalHeadMgr::searchLocalHead(const char *_sAccNbr, const char *_sAreaCode, const char *_sTime, 
		LocalHead *_pLocalHead)
{
	if(!_pLocalHead)  //为空无法无法将返回值拷贝给该参数
        return false;

    LocalHead* pLocalHead =0;
    LocalHead* pHead = 0;
    
    char sTempAccNbr[MAX_ACCNBR_LEN+1];
    memset(sTempAccNbr, '\0', MAX_ACCNBR_LEN+1);
	char sKey[MAX_AREA_CODE_LEN+MAX_ACCNBR_LEN];
    memset(sKey, '\0', MAX_AREA_CODE_LEN+MAX_ACCNBR_LEN);
		
	strncpy(sTempAccNbr,_sAccNbr,MAX_ACCNBR_LEN);
	sTempAccNbr[MAX_ACCNBR_LEN] = '\0';
    int iAccNbrLen = strlen(sTempAccNbr);  //待查询的账号串长度

    sprintf(sKey,"%s_%s",_sAreaCode,sTempAccNbr);
    int iStrLength = strlen(sKey);  //索引串的长度
    
    int i = 1;
    unsigned int iIdx = 0;
    pHead = (LocalHead *)G_PPARAMINFO->m_poLocalHeadList;
    while(i <= iAccNbrLen)
    {
        if(G_PPARAMINFO->m_poLocalHeadIndex->get(sKey, &iIdx))
        {
            pLocalHead = pHead + iIdx;
            while(pLocalHead != pHead)
            {
                if(((pLocalHead->m_iLen == iAccNbrLen)||(0 == pLocalHead->m_iLen))&&
					(strcmp(_sTime,pLocalHead->m_sEffDate)>=0)
					&& (strcmp(_sTime,pLocalHead->m_sExpDate)<0))
				{
					//大于等于生效时间小于失效时间
					*_pLocalHead = *pLocalHead;
					return true;
				}
                else
				{
					pLocalHead = pHead + pLocalHead->m_iNextOffset;
					continue;
				}
            }
        }

        sKey[iStrLength-i] = '\0';
        i++;
    }
    return false;
}
#endif


//##ModelId=42803E6602C3
int LocalHead::getOrgID() const
{
	return m_iOrgID;
}

//##ModelId=42803E810055
char* LocalHead::getAreaCode() const
{
	return (char *)m_sAreaCode;
}

//##ModelId=42803E940193
int LocalHead::getAccNbrLen() const
{
	return m_iLen;
}

//##ModelId=42803EAC00CF
char* LocalHead::getHead() const
{
	return (char *)m_sHead;
}

//##ModelId=4280447C0292
int LocalHeadMgr::m_iMaxHeadLen = 0;

//##ModelId=42804495031A
int LocalHeadMgr::m_iMinHeadLen = 0;

//##ModelId=4280316802FD
KeyTree<LocalHead*>* LocalHeadMgr::m_poLocalHead = NULL;

