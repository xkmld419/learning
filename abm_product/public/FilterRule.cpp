/*VER: 1*/ 
#include "FilterRule.h"
#include "Environment.h"
#include "Exception.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

FilterRuleResult :: FilterRuleResult()
{
	m_iEventTypeID = 0;
	memset (m_sEffDate, 0, sizeof (m_sEffDate));
	memset (m_sExpDate, 0, sizeof (m_sExpDate));
}

FilterRuleResult :: FilterRuleResult(int iEventTypeID, char * sEffDate, char * sExpDate)
{
	m_iEventTypeID = iEventTypeID;
	strcpy (m_sEffDate, sEffDate);
	strcpy (m_sExpDate, sExpDate);
}


FilterRuleCheckValue::FilterRuleCheckValue()
{
	m_bDefault = false;
	m_iDefault = 0;
}

FilterRuleCheckValue::~FilterRuleCheckValue()
{
}


int FilterRuleCheckValue::filter(StdEvent * pEvent)
{
#ifndef PRIVATE_MEMORY
		FilterRuleResult * pRet = getFilterRuleResult(pEvent);
		if(!pRet)
			return -1;
		if(strcmp (pEvent->m_sStartDate, pRet->m_sEffDate)>=0 && strcmp (pEvent->m_sStartDate, pRet->m_sExpDate)<0 ) 
		{
			pEvent->m_iEventTypeID = pRet->m_iEventTypeID;
			return pEvent->m_iEventTypeID;
		} else {
			return -1;
		}
#endif	
}


/*
函数说明:根据标准事件获取对应的FilterRuleResult
参数说明:
返回值: true成功
*/
FilterRuleResult* FilterRuleCheckValue::getFilterRuleResult(StdEvent * pEvent)
{
#ifndef PRIVATE_MEMORY
			m_bDefault = false;
			m_iDefault = 0;
			memset(&m_oFilterRuleResult,'\0',sizeof(FilterRuleResult));
			
	 		//检测LongType
			m_iSwitchLongType = pEvent->m_oEventExt.m_iSwitchLongType;
			
			bool bRes = checkSwitchIDIndex(pEvent);
			if(bRes)
			{
				bRes = checkTrunkInIndex(pEvent);
				if(bRes)
				{
					bRes = checkTrunkOutIndex(pEvent);
					if(bRes)
					{
						bRes = checkGatherPotIndex(pEvent);
						if(bRes)
						{
							bRes = checkAreaCodeIndex(pEvent);
							if(bRes)
							{
								bRes = checkCallingIndex(pEvent);
								if(bRes)
								{
									bRes = checkCalledIndex(pEvent);
									if(bRes)
										return &m_oFilterRuleResult;
								}
							}	 
						} 
					} 
				} 
			} 
#endif			
			return NULL;
}

/*
函数说明:根据标准事件获取对应层的索引是否有这个值
参数说明:
返回值: -1失败 ; 0 成功; 其它正值是带失败原因的返回
*/
bool FilterRuleCheckValue::checkSwitchIDIndex(StdEvent * pEvent)
{
#ifndef PRIVATE_MEMORY
	 bool bDefault = false;
	 m_iSwitchID = pEvent->m_iSwitchID;
	 char sKey[300] = {0};	
	 sprintf(sKey,"%d_%d",pEvent->m_oEventExt.m_iSwitchLongType,m_iSwitchID);
	 if(m_iSwitchID == -1)		
		bDefault = true;
	 
	 unsigned int iRuleRes = 0;
	 if (!G_PPARAMINFO->m_poRuleCheckSwitchIDIndex->get (sKey, &iRuleRes)) 
	 {	//看是否有默认值 没有就返回失败//m_iSwitchID == -1
		if(bDefault){
			return false;
		} else {
			sKey[0] = '\0';
			m_iSwitchID = -1;
			sprintf(sKey,"%d_%d",pEvent->m_oEventExt.m_iSwitchLongType,m_iSwitchID);
			if (!G_PPARAMINFO->m_poRuleCheckSwitchIDIndex->get (sKey, &iRuleRes)) 
	 		{			
				return false;
			} else {
				m_iDefault = FILTER_SWITCHID;
				if(!m_bDefault)
					m_bDefault = true;
				return true;
			}
		}
	 }
#endif
	 return true;
}

bool FilterRuleCheckValue::checkTrunkInIndex(StdEvent * pEvent)
{
#ifndef PRIVATE_MEMORY
	 bool bDefault = false;
	 char sKey[300] = {0};
	 memset(m_sTrunkIn,'\0',sizeof(m_sTrunkIn));	
	 if(pEvent->m_sTrunkIn[0] != '*'){
	 	sprintf(sKey,"%d_%s",m_iSwitchID,pEvent->m_sTrunkIn);
		strcpy(m_sTrunkIn,pEvent->m_sTrunkIn);
	 } else {
	 	sprintf(sKey,"%d_%s",m_iSwitchID,"*");
		strcpy(m_sTrunkIn,"*");
		bDefault = true;
	 }
	 unsigned int iRuleRes = 0;
	 if (!G_PPARAMINFO->m_poRuleCheckTrunkInIndex->get (sKey, &iRuleRes)) 
	 {  //pEvent->m_sTrunkIn[0] == '*'
		if(bDefault){
			return false;
		} else {
			sKey[0] = '\0';
			memset(m_sTrunkIn,'\0',sizeof(m_sTrunkIn));
			strcpy(m_sTrunkIn,"*");
			sprintf(sKey,"%d_%s",m_iSwitchID,m_sTrunkIn);
			if (!G_PPARAMINFO->m_poRuleCheckTrunkInIndex->get (sKey, &iRuleRes)) 
	 		{			
				return false;
			} else {
				m_iDefault = FILTER_TRUNCKIN;
				if(!m_bDefault)
					m_bDefault = true;
				return true;
			}
		}
	 } 
#endif
	 return true;
}

bool FilterRuleCheckValue::checkTrunkOutIndex(StdEvent * pEvent)
{
#ifndef PRIVATE_MEMORY
	 memset(m_sTrunkOut,'\0',sizeof(m_sTrunkOut));
	 bool bDefault = false;
	 char sKey[300] = {0};	
	 if(pEvent->m_sTrunkOut[0] != '*'){
	 	sprintf(sKey,"%s_%s",m_sTrunkIn,pEvent->m_sTrunkOut);
		strcpy(m_sTrunkOut,pEvent->m_sTrunkOut);
	 } else {
	 	sprintf(sKey,"%s_%s",m_sTrunkIn,"*");
		strcpy(m_sTrunkOut,"*");
		bDefault = true;
	 }
	 unsigned int iRuleRes = 0;
	 if (!G_PPARAMINFO->m_poRuleCheckTrunkOutIndex->get (sKey, &iRuleRes)) 
	 {
	 	if(bDefault){
			return false;
		} else {
			sKey[0] = '\0';
			memset(m_sTrunkOut,'\0',sizeof(m_sTrunkOut));
			strcpy(m_sTrunkOut,"*");
			sprintf(sKey,"%s_%s",m_sTrunkIn,m_sTrunkOut);
			if (!G_PPARAMINFO->m_poRuleCheckTrunkOutIndex->get (sKey, &iRuleRes)) 
	 		{			
				return false;
			} else {
				m_iDefault = FILTER_TRUNCKOUT;
				if(!m_bDefault)
					m_bDefault = true;
				return true;
			}
		}
	 } 
#endif 
	 return true;
}

bool FilterRuleCheckValue::checkGatherPotIndex(StdEvent * pEvent)
{
#ifndef PRIVATE_MEMORY
	 bool bDefault = false;
	 char sKey[300] = {0};	
	 m_iPot = atoi(pEvent->m_sReservedField[3]);
	 sprintf(sKey,"%s_%d",m_sTrunkOut,m_iPot);
	 if(m_iPot == -1)
	 	bDefault = true;
	 
	 unsigned int iRuleRes = 0;
	 if (!G_PPARAMINFO->m_poRuleCheckGatherPotIndex->get (sKey, &iRuleRes)) 
	 {	//看是否有默认值 没有就返回失败
	 	if(bDefault) {
	 		return false;
		} else {
			sKey[0] = '\0';
			m_iPot = -1;
			sprintf(sKey,"%s_%d",m_sTrunkOut,m_iPot);
			if (!G_PPARAMINFO->m_poRuleCheckGatherPotIndex->get (sKey, &iRuleRes)) 
	 		{			
				return false;
			} else {
				m_iDefault = FILTER_GATHERPOT;
				if(!m_bDefault)
					m_bDefault = true;
				return true;
			}
		}
	} 
#endif 
	return true;
}

bool FilterRuleCheckValue::checkAreaCodeIndex(StdEvent * pEvent)
{
#ifndef PRIVATE_MEMORY
	 bool bDefault = false;
	 char sKey[300] = {0};
	 memset(m_sSwitchAreaCode,'\0',sizeof(m_sSwitchAreaCode));
	 if(pEvent->m_oEventExt.m_sSwitchAreaCode[0] != '*'){	
	 	sprintf(sKey,"%d_%s",m_iPot,pEvent->m_oEventExt.m_sSwitchAreaCode);
		strcpy(m_sSwitchAreaCode,pEvent->m_oEventExt.m_sSwitchAreaCode);
	 } else {
	 	sprintf(sKey,"%d_%s",m_iPot,"*");
		strcpy(m_sSwitchAreaCode,"*");
		bDefault = true;
	 }
	 unsigned int iRuleRes = 0;
	 if (!G_PPARAMINFO->m_poRuleCheckAreaCodeIndex->get (sKey, &iRuleRes)) 
	 {	//看是否有默认值 没有就返回失败
	 	if(bDefault) {
	 		return false;
		} else {
			sKey[0] = '\0';
			memset(m_sSwitchAreaCode,'\0',sizeof(m_sSwitchAreaCode));
			strcpy(m_sSwitchAreaCode,"*");
			sprintf(sKey,"%d_%s",m_iPot,m_sSwitchAreaCode);
			if (!G_PPARAMINFO->m_poRuleCheckAreaCodeIndex->get (sKey, &iRuleRes)) 
	 		{
				return false;
			} else {
				m_iDefault = FILTER_AREACODE;
				if(!m_bDefault)
					m_bDefault = true;
				return true;
			}
		}
	} 
#endif 
	return true;
}

bool FilterRuleCheckValue::checkCallingIndex(StdEvent * pEvent)
{
#ifndef PRIVATE_MEMORY
	 bool bDefault = false;
	 char sKey[300] = {0};	
	 memset(m_sOrgCallingNBR,'\0',sizeof(m_sOrgCallingNBR));
	 if(pEvent->m_sOrgCallingNBR[0] != '*'){
	 	sprintf(sKey,"%s_%s",m_sSwitchAreaCode,pEvent->m_sOrgCallingNBR);
		strcpy(m_sOrgCallingNBR,pEvent->m_sOrgCallingNBR);
	 } else {
	 	sprintf(sKey,"%s_%s",m_sSwitchAreaCode,"*");
		strcpy(m_sOrgCallingNBR,"*");
		bDefault = true;
	 }	
	 unsigned int iRuleRes = 0;
	 if (!G_PPARAMINFO->m_poRuleCheckCallingIndex->getMax (sKey, &iRuleRes)) 
	 {	//看是否有默认值 没有就返回失败
	 	if(bDefault){
	 		return false;
		} else {
			sKey[0] = '\0';
			memset(m_sOrgCallingNBR,'\0',sizeof(m_sOrgCallingNBR));
			strcpy(m_sOrgCallingNBR,"*");
			sprintf(sKey,"%s_%s",m_sSwitchAreaCode,m_sOrgCallingNBR);
			if (!G_PPARAMINFO->m_poRuleCheckCallingIndex->getMax (sKey, &iRuleRes)) 
	 		{
				return false;
			} else {
				m_iDefault = FILTER_CALLING;
				if(!m_bDefault)
					m_bDefault = true;
				return true;
			}
		}
	 } 
#endif 
	 return true;
}

bool FilterRuleCheckValue::checkCalledIndex(StdEvent * pEvent)
{
#ifndef PRIVATE_MEMORY
	 bool bDefault = false;
	 char sKey[300] = {0};	
	 if(pEvent->m_sOrgCalledNBR[0] != '*'){
	 	sprintf(sKey,"%s_%s",m_sOrgCallingNBR,pEvent->m_sOrgCalledNBR);
	 } else {
	 	sprintf(sKey,"%s_%s",m_sOrgCallingNBR,"*");
		bDefault = true;
	 }	
	 unsigned int iRuleRes = 0;
	 if (!G_PPARAMINFO->m_poRuleCheckCalledIndex->getMax (sKey, &iRuleRes)) 
	 {	//看是否有默认值 没有就返回失败
	 	if(bDefault){
	 		return false;
		} else {
			sKey[0] = '\0';
			sprintf(sKey,"%s_%s",m_sOrgCallingNBR,"*");
			if (!G_PPARAMINFO->m_poRuleCheckCalledIndex->getMax (sKey, &iRuleRes)) 
	 		{
				return false;
			} else {
				m_oFilterRuleResult = *(G_PPARAMINFO->m_poFilterRuleResultList + iRuleRes);
				m_iDefault = FILTER_CALLED;
				if(!m_bDefault)
					m_bDefault = true;
				return true;
			}
		}
	 } else {
	 	 m_oFilterRuleResult =  *(G_PPARAMINFO->m_poFilterRuleResultList + iRuleRes);
		 return true;
	 }
#endif 
	
}

inline int RuleCheckCalled::check(StdEvent * pEvent)
{
	FilterRuleResult * pRet = 0;

	if (!m_oCheck.getMax (pEvent->m_sOrgCalledNBR, &pRet) || 
		strcmp (pEvent->m_sStartDate, pRet->m_sEffDate)<0 ||
		strcmp (pEvent->m_sStartDate, pRet->m_sExpDate)>=0 ) {

		if ((pRet=m_poDefaultResult) && 
			strcmp (pEvent->m_sStartDate, pRet->m_sEffDate)>=0 && 
			strcmp (pEvent->m_sStartDate, pRet->m_sExpDate)<0 ) {
				;

		} else {

			return -1;
		}
	}


	pEvent->m_iEventTypeID = pRet->m_iEventTypeID;
	return pEvent->m_iEventTypeID;
}


void RuleCheckCalled::insert(char * sOrgCalledNbr, FilterRuleResult * pResult)
{
	if (sOrgCalledNbr[0] == '*') {
		m_poDefaultResult = pResult;
	} else {
		m_oCheck.add (sOrgCalledNbr, pResult);
	}
}

#ifdef PRIVATE_MEMORY
RuleCheckCalled::RuleCheckCalled()
{
	m_poDefaultResult = 0;
}
#else
RuleCheckCalled::RuleCheckCalled()
{
	m_poDefaultResult = 0;
}
#endif

inline int RuleCheckCalling::check(StdEvent * pEvent)
{
	RuleCheckCalled * pCalled;
	int iRet;

	if (m_oCheck.getMax (pEvent->m_sOrgCallingNBR, &pCalled)) {

		if ((iRet = pCalled->check (pEvent)) != -1) {
			return iRet;
		}
	}

	if (m_poDefaultCheck) {
		return m_poDefaultCheck->check (pEvent);
	}

	return -1;
}

void RuleCheckCalling::insert(char * sOrgCallingNbr, char * sOrgCalledNbr, FilterRuleResult * pResult)
{
	RuleCheckCalled * pCalled;

	if (sOrgCallingNbr[0] == '*') {
		if (!m_poDefaultCheck) {
			m_poDefaultCheck = new RuleCheckCalled ();
			if (!m_poDefaultCheck) THROW (MBC_FilterRule + 1);
		} 

		m_poDefaultCheck->insert (sOrgCalledNbr, pResult);
	
		return;
	}
			
	if (!m_oCheck.get (sOrgCallingNbr, &pCalled)) {
		pCalled = new RuleCheckCalled ();
		if (!pCalled) THROW (MBC_FilterRule + 1);
		m_oCheck.add (sOrgCallingNbr,  pCalled);
	}


	pCalled->insert (sOrgCalledNbr, pResult);
}

RuleCheckCalling::RuleCheckCalling()
{
	m_poDefaultCheck = 0;
}

inline int RuleCheckAreaCode::check(StdEvent * pEvent)
{
	RuleCheckCalling * pCalling;
	int iRet;

	if (m_oCheck.get (pEvent->m_oEventExt.m_sSwitchAreaCode, &pCalling)) {
		if ((iRet = pCalling->check (pEvent)) != -1) {
			return iRet;
		}
	}

	if (m_poDefaultCheck) {
		return m_poDefaultCheck->check (pEvent);
	}

	return -1;
}

RuleCheckAreaCode::RuleCheckAreaCode()
{
	m_poDefaultCheck = 0;
}

void RuleCheckAreaCode::insert(char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, 
		FilterRuleResult * pResult)
{
	RuleCheckCalling * pCalling;

	if (sSwitchAreaCode[0] == '*') {
		if (!m_poDefaultCheck) {
			m_poDefaultCheck = new RuleCheckCalling ();
			if (!m_poDefaultCheck) THROW (MBC_FilterRule + 1);
		}

		m_poDefaultCheck->insert (sOrgCallingNbr, sOrgCalledNbr, pResult);
	
		return;
	}

	if (!m_oCheck.get (sSwitchAreaCode, &pCalling)) {
		pCalling = new RuleCheckCalling ();
		if (!pCalling) THROW (MBC_FilterRule + 1);
		m_oCheck.add (sSwitchAreaCode, pCalling);
	}

	pCalling->insert (sOrgCallingNbr, sOrgCalledNbr, pResult);
}


/////////////s

RuleCheckGatherPot::RuleCheckGatherPot()
{
	m_poDefaultCheck = 0;
}

inline int RuleCheckGatherPot::check(StdEvent * pEvent)
{
	RuleCheckAreaCode * pAreaCode =0;
	int iRet;

//// 话单的采集点使用offerID4,也就是m_iOffers[3]
///因为以前是使用的OFFERID4 即后台的m_iOffers[3]
///后来发现在新系统中这个值在查找用户资料后会变动,因此
///把他换成保留字段4
    int iPot= atoi(pEvent->m_sReservedField[3]);
    if(m_oCheck.getCount() >0 && iPot>=0)
    {
        if (m_oCheck.get ( iPot, &pAreaCode)) {
            if ((pAreaCode)&&(iRet = pAreaCode->check (pEvent)) != -1) {
                return iRet;
            }
        }
    }

    if (m_poDefaultCheck) {
        return m_poDefaultCheck->check (pEvent);
    }

    return -1;
}


void RuleCheckGatherPot::insert( int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, 
		FilterRuleResult * pResult)
{
	RuleCheckAreaCode * pAreaCode;

	if (iGatherPot == -1) {
		if (!m_poDefaultCheck) {
			m_poDefaultCheck = new RuleCheckAreaCode ();
			if (!m_poDefaultCheck) THROW (MBC_FilterRule + 1);
		}

		m_poDefaultCheck->insert (sSwitchAreaCode,sOrgCallingNbr, sOrgCalledNbr, pResult);
	
		return;
	}

	if (!m_oCheck.get (iGatherPot, &pAreaCode)) {
		pAreaCode = new RuleCheckAreaCode ();
		if (!pAreaCode) THROW (MBC_FilterRule + 1);
		m_oCheck.add (iGatherPot, pAreaCode);
	}

	pAreaCode->insert (sSwitchAreaCode,sOrgCallingNbr, sOrgCalledNbr, pResult);
}




///------
RuleCheckTrunkOut::RuleCheckTrunkOut()
{
	m_poDefaultCheck = 0;
}

inline int RuleCheckTrunkOut::check(StdEvent * pEvent)
{
	RuleCheckGatherPot * pGatherPot;
	int iRet;

	if (m_oCheck.get (pEvent->m_sTrunkOut, &pGatherPot)) {
		if ((iRet = pGatherPot->check (pEvent)) != -1) {
			return iRet;
		}
	}

	if (m_poDefaultCheck) {
		return m_poDefaultCheck->check (pEvent);
	}

	return -1;
}


void RuleCheckTrunkOut::insert(char * sTrunkOut,  int iGatherPot, char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, 
		FilterRuleResult * pResult)
{
	RuleCheckGatherPot * pGatherPot;

	if (sTrunkOut[0] == '*') {
		if (!m_poDefaultCheck) {
			m_poDefaultCheck = new RuleCheckGatherPot ();
			if (!m_poDefaultCheck) THROW (MBC_FilterRule + 1);
		}

		m_poDefaultCheck->insert (iGatherPot,sSwitchAreaCode,sOrgCallingNbr, sOrgCalledNbr, pResult);
	
		return;
	}

	if (!m_oCheck.get (sTrunkOut, &pGatherPot)) {
		pGatherPot = new RuleCheckGatherPot ();
		if (!pGatherPot) THROW (MBC_FilterRule + 1);
		m_oCheck.add (sTrunkOut, pGatherPot);
	}

	pGatherPot->insert (iGatherPot,sSwitchAreaCode,sOrgCallingNbr, sOrgCalledNbr, pResult);
}





//--


RuleCheckTrunkIn::RuleCheckTrunkIn()
{
	m_poDefaultCheck = 0;
}

inline int RuleCheckTrunkIn::check(StdEvent * pEvent)
{
	RuleCheckTrunkOut * pTrunkOut;
	int iRet;

	if (m_oCheck.get (pEvent->m_sTrunkIn, &pTrunkOut)) {
		if ((iRet = pTrunkOut->check (pEvent)) != -1) {
			return iRet;
		}
	}

	if (m_poDefaultCheck) {
		return m_poDefaultCheck->check (pEvent);
	}

	return -1;
}


void RuleCheckTrunkIn::insert(char * sTrunkIn,char * sTrunkOut,  int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, 
		FilterRuleResult * pResult)
{
	RuleCheckTrunkOut * pTrunkOut;

	if (sTrunkIn[0] == '*') {
		if (!m_poDefaultCheck) {
			m_poDefaultCheck = new RuleCheckTrunkOut ();
			if (!m_poDefaultCheck) THROW (MBC_FilterRule + 1);
		}

		m_poDefaultCheck->insert (sTrunkOut,iGatherPot,sSwitchAreaCode,sOrgCallingNbr, sOrgCalledNbr, pResult);
	
		return;
	}

	if (!m_oCheck.get (sTrunkIn, &pTrunkOut)) {
		pTrunkOut = new RuleCheckTrunkOut ();
		if (!pTrunkOut) THROW (MBC_FilterRule + 1);
		m_oCheck.add (sTrunkIn, pTrunkOut);
	}

	pTrunkOut->insert (sTrunkOut,iGatherPot,sSwitchAreaCode,sOrgCallingNbr, sOrgCalledNbr, pResult);
}



//--------

RuleCheckSwitchID::RuleCheckSwitchID()
{
	m_poDefaultCheck = 0;
}
	
RuleCheckLongType::~RuleCheckLongType()
{
	
}

inline int RuleCheckSwitchID::check(StdEvent * pEvent)
{
	RuleCheckTrunkIn * pTrunkIn;
	int iRet;

	if (m_oCheck.get (pEvent->m_iSwitchID, &pTrunkIn)) {
		if ((iRet = pTrunkIn->check (pEvent)) != -1) {
			return iRet;
		}
	}

	if (m_poDefaultCheck) {
		return m_poDefaultCheck->check (pEvent);
	}

	return -1;
}


void RuleCheckSwitchID::insert(int iSwitchID, char * sTrunkIn,char * sTrunkOut,  int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, 
		FilterRuleResult * pResult)
{
	RuleCheckTrunkIn * pTrunkIn;

	if (iSwitchID == -1) {
		if (!m_poDefaultCheck) {
			m_poDefaultCheck = new RuleCheckTrunkIn ();
			if (!m_poDefaultCheck) THROW (MBC_FilterRule + 1);
		}

		m_poDefaultCheck->insert (sTrunkIn,sTrunkOut,iGatherPot,sSwitchAreaCode,sOrgCallingNbr, sOrgCalledNbr, pResult);
	
		return;
	}

	if (!m_oCheck.get (iSwitchID, &pTrunkIn)) {
		pTrunkIn = new RuleCheckTrunkIn ();
		if (!pTrunkIn) THROW (MBC_FilterRule + 1);
		m_oCheck.add (iSwitchID, pTrunkIn);
	}

	pTrunkIn->insert (sTrunkIn,sTrunkOut,iGatherPot,sSwitchAreaCode,sOrgCallingNbr, sOrgCalledNbr, pResult);
}




/////////////end


void RuleCheckLongType::insert(int iLongType, int iSwitchID, char * sTrunkIn,char * sTrunkOut,  int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr,
                        FilterRuleResult * pResult)
{
	if (iLongType >= MAX_LONG_TYPE) {
		THROW (MBC_FilterRule + 2);
	}

	if (iLongType == -1) {
		if (!m_poDefaultCheck) {
			m_poDefaultCheck = new RuleCheckSwitchID ();
			if (!m_poDefaultCheck) THROW (MBC_FilterRule + 1);
		}

		m_poDefaultCheck->insert (iSwitchID,sTrunkIn,sTrunkOut,iGatherPot,sSwitchAreaCode, sOrgCallingNbr, sOrgCalledNbr, pResult);
	} else {
		if (!m_poCheck[iLongType]) {
			m_poCheck[iLongType] = new RuleCheckSwitchID ();
			if (!m_poCheck[iLongType]) THROW (MBC_FilterRule + 1);
		}

		m_poCheck[iLongType]->insert (iSwitchID,sTrunkIn,sTrunkOut,iGatherPot,sSwitchAreaCode, sOrgCallingNbr, sOrgCalledNbr, pResult);
	}

}

int RuleCheckLongType::check(StdEvent * pEvent)
{
	RuleCheckSwitchID * pSwitchID;
	int iRet;

	if (pEvent->m_oEventExt.m_iSwitchLongType >= MAX_LONG_TYPE) {
		return -1;
	}

	if ((pSwitchID = m_poCheck [pEvent->m_oEventExt.m_iSwitchLongType]) != NULL) {
		if ((iRet = pSwitchID->check (pEvent)) > 0) {
			return iRet;
		}
	}

	if (m_poDefaultCheck) {
		return m_poDefaultCheck->check (pEvent);
	}

	return -1;
}

RuleCheckLongType::RuleCheckLongType()
{
	memset (m_poCheck, 0, sizeof (m_poCheck));
	m_poDefaultCheck = 0;
}

FilterRule::FilterRule()
{

}

FilterRule::~FilterRule()
{
}
void FilterRule::load()
{
	DEFINE_QUERY (qry);
	FilterRuleResult * pResult;

	qry.setSQL ("select switch_long_type,nvl(switch_id,-1),nvl(trunk_in,'*'),nvl(trunk_out,'*'),nvl( gather_pot,-1),"
			"switch_area_code, org_calling_nbr, org_called_nbr, "
			"event_type_id, nvl(eff_date, to_date('19800101', 'yyyymmdd')), "
			"nvl(exp_date, to_date('20500101', 'yyyymmdd')) from b_filter_rule");
	qry.open ();

	while (qry.next ()) {
		pResult = new FilterRuleResult (qry.field (8).asInteger (),
						qry.field (9).asString (),
						qry.field (10).asString ());
		if (!pResult) THROW (MBC_FilterRule + 1);
		m_oCheck.insert (qry.field (0).asInteger (),qry.field (1).asInteger (),  qry.field (2).asString (),qry.field (3).asString (), qry.field (4).asInteger (), 
				qry.field (5).asString (), qry.field(6).asString (),qry.field(7).asString (), pResult);
	}

	qry.close ();
}

void FilterRule::unload()
{
}

int FilterRule::filter(StdEvent *pEvent)
{
	return m_oCheck.check (pEvent);
}

