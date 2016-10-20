/*VER: 1*/ 
// // Copyright (c) 2005,联创科技股份有限公司电信事业部
// // All rights reserved.

#include "AttrTransMgr.h"
#include "Process.h"
#include "Log.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

char g_sFormatRecord[MAX_FORMAT_RECORD_LEN];
int  g_iAttrTransMgrFmtStep = 0;

//##ModelId=42510090028F
#ifdef PRIVATE_MEMORY
AttrTransMgr::AttrTransMgr(int iProcessID)
{
	m_iProcID = iProcessID;
	m_poStdEvent = NULL;
	m_poFormatGroup = NULL;
	m_poFormatComb = NULL;
		
	if (m_poFormatGroup == NULL)
		reloadRule();

	char *pt;
	
	m_iNeedSavFormatRec = 0;
	
	if ((pt = getenv("RECORD_RULE")) != NULL) 
	{
		m_iNeedSavFormatRec = atoi(pt);
	}
	
	if (!m_iNeedSavFormatRec && g_iAttrTransMgrFmtStep) {
		m_iNeedSavFormatRec = 1;
	}
}
#else
AttrTransMgr::AttrTransMgr(int iProcessID)
{
	m_iProcID = iProcessID;
	m_poStdEvent = NULL;
	m_poFormatGroup = NULL;
	m_poFormatComb = NULL;

	char *pt;
	
	m_iNeedSavFormatRec = 0;
	
	if ((pt = getenv("RECORD_RULE")) != NULL) 
	{
		m_iNeedSavFormatRec = atoi(pt);
	}
	
	if (!m_iNeedSavFormatRec && g_iAttrTransMgrFmtStep) {
		m_iNeedSavFormatRec = 1;
	}
}
#endif

//##ModelId=4259E98B002E
#ifdef PRIVATE_MEMORY
AttrTransMgr::~AttrTransMgr()
{
	unloadRule();
}
#else
AttrTransMgr::~AttrTransMgr()
{
}
#endif


//##ModelId=41EF8BBF026F
//##Documentation
//## 对当前事件进行属性规整
//## 事件规整正常返回0
//## 事件规整异常返回异常编码
#ifdef PRIVATE_MEMORY
int AttrTransMgr::trans(StdEvent* pStdEvent)
{
	m_poStdEvent = pStdEvent;

	FormatRule *pFormatRule;
	FormatStep *pFormatStep;
	FormatGroup *pGroup;
	FormatComb *pFormatComb;
	
	//查找当前源事件类型对应的规则组
	for (pFormatComb=m_poFormatComb; pFormatComb; pFormatComb=pFormatComb->m_poNext )
	{
		if( pFormatComb->m_iSourceEventType == pStdEvent->m_oEventExt.m_iSourceEventType )
		{
			pGroup = pFormatComb->m_poFormatGroup;
			break;
		}
	}
	
	if(!pFormatComb) 
	{ //未找到当前源事件类型对应的规则组
		return 0; //允许不配置规则组
	}
	
	g_sFormatRecord[0] = 0;
	
	for (pFormatStep = pGroup->m_poFormatStep; pFormatStep; pFormatStep=pFormatStep->m_poNext) 
	{// 步骤循环
		for (pFormatRule=pFormatStep->m_poFormatRule; pFormatRule; pFormatRule=pFormatRule->m_poNext )
		{// 规则循环
			
			// 当前事件的事件类型 不同于 规则的输入类型 && 规则的输入类型有效
			if (pFormatRule->m_iInEventType >= 0 && 
			        pFormatRule->m_iInEventType != pStdEvent->m_iEventTypeID )
			    continue;
			
          //  Log::log(0,"%s",pFormatRule->m_sRuleName);
			// 如果事件有开始时间,则判断规则的时间有效性.
			//if (strlen(pStdEvent->m_sStartDate)) {
			if (pStdEvent->m_sStartDate[0]
				/*&& pFormatRule->m_sEffDate[0] != '0' && //规则限制了生效/失效时间
			    pFormatRule->m_sExpDate[0] != '9'*/
			) 
			{
				if (strcmp(pStdEvent->m_sStartDate,pFormatRule->m_sEffDate)<0 || 
						strcmp(pStdEvent->m_sStartDate,pFormatRule->m_sExpDate)>0 )
					continue;
			}
			
			// 条件判断, 执行转换操作
			if (Process::m_poConditionMgr->check(pFormatRule->m_poCondition/*m_iConditionID*/, pStdEvent))
			{
				
				// 执行运算
				Process::m_poOperationMgr->execute(pFormatRule->m_poOperation/*m_iOperationID*/, pStdEvent);
			
				// 判断是否需要改变事件的 输出时 事件类型
				if (pFormatRule->m_iOutEventType >= 0)
					pStdEvent->m_iEventTypeID = pFormatRule->m_iOutEventType;
				
				// 判断是否要记录规整的过程
				if (m_iNeedSavFormatRec) 
				{
					strncpy(g_sFormatRecord,g_sFormatRecord,MAX_FORMAT_RECORD_LEN-200);
					g_sFormatRecord[MAX_FORMAT_RECORD_LEN-200]=0;
					sprintf(g_sFormatRecord,"%s\n====\n"
						" FmtID:%d StepSeq(步骤):%d-%s -- RuleSeq(规则):%d-%s",
						g_sFormatRecord,pFormatStep->m_iFormatID, 
						pFormatStep->m_iStepSeq, pFormatStep->m_sStepName,
						pFormatRule->m_iRuleSeq, pFormatRule->m_sRuleName
					);
					//Log::log (0, "%s",g_sFormatRecord);
					usleep(m_iNeedSavFormatRec); // 休息 m_iNeedSavFormatRec 毫秒
					
					if (g_iAttrTransMgrFmtStep==1) 
					{
						char cPt[10];
						printf("\nStepSeq(步骤):%d-%s     RuleSeq(规则):%d-%s\n",
							pFormatStep->m_iStepSeq, pFormatStep->m_sStepName,
							pFormatRule->m_iRuleSeq, pFormatRule->m_sRuleName
						);
						cout<<"\nPrint Event?(y/n): ";
						cin >> cPt;
						if (cPt[0]=='y' || cPt[0]=='Y')
							printEvent(pStdEvent);
					}
				}

				if (pFormatRule->m_iExitFlag == 1) //退出规整
                {
                    //Log::log(0,"----------%s",pFormatRule->m_sRuleName);
						sprintf(pStdEvent->m_sBillingRecord,"Format_id=%d Step_Seq=%d Rule_Seq=%d",
						pFormatStep->m_iFormatID,pFormatStep->m_iStepSeq,pFormatRule->m_iRuleSeq);

					return FORMART_NORMAL;
                }
			}
            
            
						
		}
	}
	sprintf(pStdEvent->m_sBillingRecord,"Format_id=-1 Step_Seq=-1 Rule_Seq=-1");
	return FORMART_NORMAL;
}
#else
int AttrTransMgr::trans(StdEvent* pStdEvent)
{
    int i = 0;
	m_poStdEvent = pStdEvent;
    FormatGroup *pFormatGroup = 0;
    FormatStep  *pFormatStep  = 0;
    FormatRule  *pFormatRule  = 0;
    FormatComb  *pFormatComb  = 0;
    FormatStep  *pFormatBeginStep  = 0;
    FormatRule  *pFormatBeginRule  = 0;
    int iCombCnt = G_PPARAMINFO->m_poFormatCombData->getCount();
	
	//查找当前源事件类型对应的规则组
	for(i = 1; i <= iCombCnt; i++)
	{
        pFormatComb = G_PPARAMINFO->m_poFormatCombList + i;
        if(pFormatComb->m_iSourceEventType == pStdEvent->m_oEventExt.m_iSourceEventType
           &&pFormatComb->m_iProcessID == m_iProcID)
        {
			pFormatGroup = G_PPARAMINFO->m_poFormatGroupList + pFormatComb->m_iFormatGroupOffset;
			break;
		}
    }
	
	if(i > iCombCnt) 
	{   //未找到当前源事件类型对应的规则组
		return 0; //允许不配置规则组
	}
	
	g_sFormatRecord[0] = 0;

    pFormatStep = G_PPARAMINFO->m_poFormatStepList + pFormatGroup->m_iFormatStepOffset;
    pFormatBeginStep = pFormatStep;
    for(int j = 0; j < pFormatGroup->m_iFormatStepCnt; j++)
    {
        pFormatStep = pFormatBeginStep + j;
        pFormatRule = G_PPARAMINFO->m_poFormatRuleList + pFormatStep->m_iFormatRuleOffset;
        pFormatBeginRule = pFormatRule;
        for(int k = 0; k < pFormatStep->m_iFormatRuleCnt; k++)
        {
            pFormatRule = pFormatBeginRule + k;
			
			// 当前事件的事件类型 不同于 规则的输入类型 && 规则的输入类型有效
			if (pFormatRule->m_iInEventType >= 0 && 
			        pFormatRule->m_iInEventType != pStdEvent->m_iEventTypeID )
			    continue;
			
            //Log::log(0,"%s",pFormatRule->m_sRuleName);
			// 如果事件有开始时间,则判断规则的时间有效性.
			//if (strlen(pStdEvent->m_sStartDate)) {
			if (pStdEvent->m_sStartDate[0]
				/*&& pFormatRule->m_sEffDate[0] != '0' && //规则限制了生效/失效时间
			    pFormatRule->m_sExpDate[0] != '9'*/
			) 
			{
				if (strcmp(pStdEvent->m_sStartDate,pFormatRule->m_sEffDate)<0 || 
						strcmp(pStdEvent->m_sStartDate,pFormatRule->m_sExpDate)>0 )
					continue;
			}
			
			// 条件判断, 执行转换操作
			if (Process::m_poConditionMgr->check(pFormatRule->m_iConditionID/*m_poCondition*/, pStdEvent))
			{
				
				// 执行运算
				Process::m_poOperationMgr->execute(pFormatRule->m_iOperationID/*m_poOperation*/, pStdEvent);
			
				// 判断是否需要改变事件的 输出时 事件类型
				if (pFormatRule->m_iOutEventType >= 0)
					pStdEvent->m_iEventTypeID = pFormatRule->m_iOutEventType;
				
				// 判断是否要记录规整的过程
				if (m_iNeedSavFormatRec) 
				{
					strncpy(g_sFormatRecord,g_sFormatRecord,MAX_FORMAT_RECORD_LEN-200);
					g_sFormatRecord[MAX_FORMAT_RECORD_LEN-200]=0;
					sprintf(g_sFormatRecord,"%s\n====\n"
						" FmtID:%d StepSeq(步骤):%d-%s -- RuleSeq(规则):%d-%s",
						g_sFormatRecord,pFormatStep->m_iFormatID, 
						pFormatStep->m_iStepSeq, pFormatStep->m_sStepName,
						pFormatRule->m_iRuleSeq, pFormatRule->m_sRuleName
					);
					//Log::log (0, "%s",g_sFormatRecord);
					usleep(m_iNeedSavFormatRec); // 休息 m_iNeedSavFormatRec 毫秒
					
					if (g_iAttrTransMgrFmtStep==1) 
					{
						char cPt[10];
						printf("\nStepSeq(步骤):%d-%s     RuleSeq(规则):%d-%s\n",
							pFormatStep->m_iStepSeq, pFormatStep->m_sStepName,
							pFormatRule->m_iRuleSeq, pFormatRule->m_sRuleName
						);
						cout<<"\nPrint Event?(y/n): ";
						cin >> cPt;
						if (cPt[0]=='y' || cPt[0]=='Y')
							printEvent(pStdEvent);
					}
				}

				if (pFormatRule->m_iExitFlag == 1) //退出规整
                {
                    //Log::log(0,"----------%s",pFormatRule->m_sRuleName);
						sprintf(pStdEvent->m_sBillingRecord,"Format_id=%d Step_Seq=%d Rule_Seq=%d",
						pFormatStep->m_iFormatID,pFormatStep->m_iStepSeq,pFormatRule->m_iRuleSeq);

					return FORMART_NORMAL;
                }
			}
            
            
						
		}
	}
	sprintf(pStdEvent->m_sBillingRecord,"Format_id=-1 Step_Seq=-1 Rule_Seq=-1");
	
	return FORMART_NORMAL;
}
#endif


//##ModelId=4250DD97000B
void AttrTransMgr::reloadRule()
{
	int  iStepSeq, iFormatID;
	char sTmpSql[MAX_RULE_SQL_LEN];
	
	this->unloadRule();
	
	TOCIQuery qryFmt(DB_LINK);
	memset(sTmpSql,0,MAX_RULE_SQL_LEN);
	strcpy(sTmpSql,"select distinct format_id format_id  from b_attribute_format "
		" where format_id in (select format_id from b_attribute_format_combin where process_id = :iProcID)");
	qryFmt.close();
	qryFmt.setSQL(sTmpSql);
	qryFmt.setParameter("iProcID",m_iProcID);
	qryFmt.open();
	
	while(qryFmt.next())
	{ //当前进程对应的规整规则组循环
		
		FormatGroup **ppFormat;
		
		for (ppFormat=&m_poFormatGroup; *ppFormat; ppFormat=&((*ppFormat)->m_poNext) );
		
		*ppFormat = new FormatGroup;
		if (!*ppFormat)
			THROW (MBC_AttrTransMgr+1);
		
		(*ppFormat)->m_iFormatID = qryFmt.field("format_id").asInteger();
		
		TOCIQuery qryStep(DB_LINK);
		memset(sTmpSql,0,MAX_RULE_SQL_LEN);
		strcpy(sTmpSql,
			" select  step_seq, format_id, step_name from b_attribute_format_step "
			" where format_id  = :iFormatID order by step_order "
		);
		qryStep.close();
		qryStep.setSQL(sTmpSql);
		qryStep.setParameter("iFormatID",(*ppFormat)->m_iFormatID);
		qryStep.open();
		
		while(qryStep.next()) 
		{ //当前规整组的规整步骤循环
		
			FormatStep **ppStep;
			
			iStepSeq = qryStep.field("step_seq").asInteger();
			iFormatID = qryStep.field("format_id").asInteger();
			
			for (ppStep= &((*ppFormat)->m_poFormatStep); *ppStep; ppStep = &((*ppStep)->m_poNext));
			
			*ppStep = new FormatStep();
			if (!*ppStep)
				THROW (MBC_AttrTransMgr+2);

			(*ppStep)->m_iStepSeq = iStepSeq;
			(*ppStep)->m_iFormatID = iFormatID;
			strcpy((*ppStep)->m_sStepName,qryStep.field("step_name").asString());
	
			TOCIQuery qryRule(DB_LINK);
			memset(sTmpSql,0,MAX_RULE_SQL_LEN);
			strcpy(sTmpSql,
				" select rule_seq,nvl(operation_id,-1) operation_id, rule_name, "
				" nvl(condition_id,-1) condition_id,exit_flag, "
				" nvl(in_event_type,-1) in_event_type,nvl(out_event_type,-1) out_event_type, "
				" nvl(to_char(eff_date,'yyyymmddhh24miss'),'0') eff_date, "
				" nvl(to_char(exp_date,'yyyymmddhh24miss'),'9') exp_date, "
				" trunc(sysdate - nvl(eff_date,sysdate-1000)) EffLen, "
                " trunc(nvl(exp_date,sysdate+1000) - sysdate) ExpLen "
				" from b_attribute_format_rule "
				" where format_id = :iFormatID and step_seq = :iStepSeq "
				" order by rule_seq "
			);
			qryRule.close();
			qryRule.setSQL(sTmpSql);
			qryRule.setParameter("iFormatID",iFormatID);
			qryRule.setParameter("iStepSeq",iStepSeq);
			qryRule.open();
	
			while(qryRule.next()) 
			{ //属性规整规则循环
	
				FormatRule **ppRule;
	
				for (ppRule=&((*ppStep)->m_poFormatRule); *ppRule; ppRule=&((*ppRule)->m_poNext));

				*ppRule = new FormatRule();
				if (!*ppRule)
					THROW (MBC_AttrTransMgr+3);
				
				(*ppRule)->m_iRuleSeq = qryRule.field("rule_seq").asInteger();
				(*ppRule)->m_iOperationID = qryRule.field("operation_id").asInteger();
				(*ppRule)->m_iConditionID = qryRule.field("condition_id").asInteger();

				(*ppRule)->m_poOperation = 
					Process::m_poOperationMgr->getOperation ((*ppRule)->m_iOperationID);
				(*ppRule)->m_poCondition = 
					Process::m_poConditionMgr->getCondition ((*ppRule)->m_iConditionID);

				(*ppRule)->m_iExitFlag = qryRule.field("exit_flag").asInteger();
				(*ppRule)->m_iInEventType = qryRule.field("in_event_type").asInteger();
				(*ppRule)->m_iOutEventType = qryRule.field("out_event_type").asInteger();
				strncpy((*ppRule)->m_sEffDate,qryRule.field("eff_date").asString(),14);
				strncpy((*ppRule)->m_sExpDate,qryRule.field("exp_date").asString(),14);
				(*ppRule)->m_sEffDate[14]=0;
				(*ppRule)->m_sExpDate[14]=0;
				
				if (qryRule.field("EffLen").asInteger() > 365) //#生效时间在当前时间之前365天前的,认为不限制
				    strcpy ((*ppRule)->m_sEffDate, "0");
				if (qryRule.field("ExpLen").asInteger() > 365)
				    strcpy ((*ppRule)->m_sExpDate, "9");
				
				strcpy((*ppRule)->m_sRuleName,qryRule.field("rule_name").asString());
			}
			qryRule.close();
		}
		qryStep.close();
	}
	qryFmt.close();
	
	
	TOCIQuery qryFmtCmb(DB_LINK);
	memset(sTmpSql,0,MAX_RULE_SQL_LEN);
	strcpy(sTmpSql," select source_event_type,format_id from b_attribute_format_combin "
		" where process_id = :iProcID ");
	qryFmtCmb.close();
	qryFmtCmb.setSQL(sTmpSql);
	qryFmtCmb.setParameter("iProcID",m_iProcID);
	qryFmtCmb.open();
	
	while( qryFmtCmb.next() ) 
	{ //将源事件类型与其对应的规整组连接起来
		
		FormatComb **ppFormatComb;
		
		for (ppFormatComb = &m_poFormatComb; *ppFormatComb; ppFormatComb=&((*ppFormatComb)->m_poNext) );
		
		*ppFormatComb = new FormatComb;
		if (!*ppFormatComb)
			THROW (MBC_AttrTransMgr+1);
		
		(*ppFormatComb)->m_iSourceEventType = qryFmtCmb.field("source_event_type").asInteger();
		(*ppFormatComb)->m_iFormatID = qryFmtCmb.field("format_id").asInteger();
		
		for (FormatGroup *pFmtGrp=m_poFormatGroup; pFmtGrp; pFmtGrp=pFmtGrp->m_poNext) 
		{
			if (pFmtGrp->m_iFormatID == (*ppFormatComb)->m_iFormatID ) 
			{
				(*ppFormatComb)->m_poFormatGroup = pFmtGrp;
				break;
			}
		}
	}
	qryFmtCmb.close();
}


//##ModelId=4250F0750395
void AttrTransMgr::unloadRule()
{
	FormatGroup *pGroupCur, *pGroupNext;
	FormatStep  *pStepCur,  *pStepNext;
	FormatRule  *pRuleCur,  *pRuleNext;
	
	FormatComb  *pCombCur, *pCombNext;
	
	for (pGroupCur=m_poFormatGroup; pGroupCur; pGroupCur=pGroupNext)
	{
		cout<<endl;
		for (pStepCur=pGroupCur->m_poFormatStep; pStepCur; pStepCur=pStepNext)
		{
			cout<<endl;
			for (pRuleCur=pStepCur->m_poFormatRule; pRuleCur; pRuleCur=pRuleNext)
			{
				pRuleNext = pRuleCur->m_poNext;
				#ifdef _RULE_TEST_
				cout<<"----delete pRuleCur->m_iRuleSeq="<<pRuleCur->m_iRuleSeq<<endl;
				#endif
				delete pRuleCur;
			}
			pStepNext = pStepCur->m_poNext;
			#ifdef _RULE_TEST_
			cout<<"--delete pStepCur->m_iStepSeq="<<pStepCur->m_iStepSeq<<endl;
			#endif
			delete pStepCur;
		}		
		pGroupNext = pGroupCur->m_poNext;
		#ifdef _RULE_TEST_
		cout<<"*delete pGroupCur->m_iFormatID="<<pGroupCur->m_iFormatID<<endl;
		#endif
		delete pGroupCur;
	}
	
	cout<<endl;
	for (pCombCur=m_poFormatComb; pCombCur; pCombCur=pCombNext) {
		pCombNext = pCombCur->m_poNext;
		#ifdef _RULE_TEST_
		cout<<"**** Delete pCombCur m_iFormatID:"<<pCombCur->m_iFormatID<<endl;
		#endif
		delete pCombCur;
	}
}


//##ModelId=425123D60045
int AttrTransMgr::checkAttrErr()
{
	int iRet = 0;
	if (m_poStdEvent->m_oEventExt.m_iSourceEventType == 1)
		iRet = checkTimeEffect(m_poStdEvent->m_sStartDate);	
	if (iRet) {
		m_poStdEvent->m_iErrorID = iRet;
		return iRet;
	}

/*	
	iRet = existChar(m_poStdEvent->m_sOrgCallingNBR);
	if (iRet) {
		m_poStdEvent->m_iErrorID = iRet;
		return iRet;
	}
*/
/*	
	iRet = existChar(m_poStdEvent->m_sOrgCalledNBR);
	if (iRet) {
		m_poStdEvent->m_iErrorID = iRet;
		return iRet;
	}
*/
	return 0;
}

int AttrTransMgr::existChar(const char * in_string)
{
    if( in_string == NULL )
        return 0;

    while( *in_string != '\0' ){
        if( !isdigit(*in_string) )
            return NBR_EXIST_CHAR;
        in_string ++;
    }
    return 0;
}

//##ModelId=425952FB005D
void AttrTransMgr::printRule()
{
	FormatComb  *pCombCur;
	FormatGroup *pGroupCur;
	FormatStep  *pStepCur;
	FormatRule  *pRuleCur;
	
	for (pCombCur=m_poFormatComb; pCombCur; pCombCur=pCombCur->m_poNext)
	{
		cout<<endl;
		cout<<"Comb: "<<"EventType="<<pCombCur->m_iSourceEventType<<endl;
		for (pGroupCur=pCombCur->m_poFormatGroup; pGroupCur; pGroupCur=pGroupCur->m_poNext)
		{
			cout<<"--Group: "<<"GroupID="<<pGroupCur->m_iFormatID<<endl;
			for (pStepCur=pGroupCur->m_poFormatStep; pStepCur; pStepCur=pStepCur->m_poNext)
			{
				cout<<"----Step: "<<"StepID="<<pStepCur->m_iStepSeq<<endl;	
				for (pRuleCur=pStepCur->m_poFormatRule; pRuleCur; pRuleCur=pRuleCur->m_poNext)
				{
					cout<<"-------Rule: "<<"RuleID="<<pRuleCur->m_iRuleSeq;
					cout<<" ConditionID="<<pRuleCur->m_iConditionID;
					cout<<" OperationID="<<pRuleCur->m_iOperationID;
					cout<<endl;
				}
			}		
		}
	}
}

void AttrTransMgr::printEvent(StdEvent *_StdEvent)
{
	cout<<endl<<"==============================="<<endl<<endl;
	cout<<"m_lEventID              : "<<_StdEvent->m_lEventID <<endl;
	cout<<"m_iSwitchID             : "<<_StdEvent->m_iSwitchID << endl;
	cout<<"m_iBillingCycleID       : "<<_StdEvent->m_iBillingCycleID <<endl;
	cout<<"m_sThirdPartyAreaCode   : "<<_StdEvent->m_sThirdPartyAreaCode <<endl;
	cout<<"m_sThirdPartyNBR        : "<<_StdEvent->m_sThirdPartyNBR <<endl;
	cout<<"m_iServID               : "<<_StdEvent->m_iServID <<endl;
	cout<<"m_sStartDate            : "<<_StdEvent->m_sStartDate<<endl;
	cout<<"m_iCarrierTypeID        : "<<_StdEvent->m_oEventExt.m_iCarrierTypeID <<endl; 
	cout<<endl;
	cout<<"m_iBillingOrgID         : "<<_StdEvent->m_iBillingOrgID<<endl;
	cout<<"m_sEventState           : "<<_StdEvent->m_sEventState<<endl;
	cout<<"m_iCallingOrgID         : "<<_StdEvent->m_oEventExt.m_iCallingOrgID<<endl;
	cout<<"m_iCalledOrgID          : "<<_StdEvent->m_oEventExt.m_iCalledOrgID<<endl;
	cout<<"m_sAccountNBR           : "<<_StdEvent->m_sAccountNBR<<endl;
	cout<<endl;	
	cout<<"m_iCallingSPTypeID      : "<<_StdEvent->m_iCallingSPTypeID <<endl;
	cout<<"m_iCallingServiceTypeID : "<<_StdEvent->m_iCallingServiceTypeID <<endl;
	cout<<"m_iCalledSPTypeID       : "<<_StdEvent->m_iCalledSPTypeID <<endl;
	cout<<"m_iCalledServiceTypeID  : "<<_StdEvent->m_iCalledServiceTypeID <<endl;
	cout<<endl;
	cout<<"m_iErrorID              : "<<_StdEvent->m_iErrorID <<endl;
	cout<<"m_iRoamTypeID           : "<<_StdEvent->m_oEventExt.m_iRoamTypeID <<endl; 
	cout<<"m_iLongTypeID           : "<<_StdEvent->m_oEventExt.m_iLongTypeID <<endl; 
	cout<<endl;
	cout<<"m_sCallingAreaCode      : "<<_StdEvent->m_sCallingAreaCode <<endl;
	cout<<"m_sCalledAreaCode       : "<<_StdEvent->m_sCalledAreaCode <<endl;
	cout<<"iCallingLongGroupTypeID : "<<_StdEvent->m_oEventExt.m_iCallingLongGroupTypeID <<endl; 
	cout<<"iCalledLongGroupTypeID  : "<<_StdEvent->m_oEventExt.m_iCalledLongGroupTypeID <<endl;
	cout<<"m_sCallingNBR           : "<<_StdEvent->m_sCallingNBR <<endl;
	cout<<"m_sCalledNBR            : "<<_StdEvent->m_sCalledNBR <<endl;
	cout<<"m_sBillingAreaCode      : "<<_StdEvent->m_sBillingAreaCode <<endl;
	cout<<"m_sBillingNBR           : "<<_StdEvent->m_sBillingNBR <<endl;
	cout<<endl;
	cout<<"m_sBillingVisitAreaCode : "<<_StdEvent->m_sBillingVisitAreaCode<<endl;
	cout<<"m_sCallingVisitAreaCode : "<<_StdEvent->m_oEventExt.m_sCallingVisitAreaCode<<endl;
	cout<<"m_sCalledVisitAreaCode  : "<<_StdEvent->m_oEventExt.m_sCalledVisitAreaCode<<endl;
	cout<<"m_iEventTypeID          : "<<_StdEvent->m_iEventTypeID <<endl;
	
	//getchar();
}

//##ModelId=4259FEC102E9
FormatComb::FormatComb()
{
	m_poFormatGroup = NULL;
	m_poNext = NULL;
}

//##ModelId=42595D3E024F
FormatGroup::FormatGroup()
{
	m_poNext = NULL;
	m_poFormatStep = NULL;
}

//##ModelId=42595E9D0240
FormatStep::FormatStep()
{
	m_poFormatRule = NULL;
	m_poNext = NULL;
}

//##ModelId=42595EB50064
FormatRule::FormatRule()
{
	m_poNext = NULL;
}


//##ModelId=425124ED019B
int AttrTransMgr::checkTimeEffect(char *sTime)
{
    char sTempYear[5],sTempMonth[3],sTempDay[3],sTempHour[3],
         sTempMin[3];
    int  iTempYear,iTempMonth,iTempDay,iTempHour,iTempMin,iTempSec;

    strncpy(sTempYear,sTime,4);sTempYear[4]=0;
    iTempYear=atoi(sTempYear);
    if(iTempYear>2020||iTempYear<2000) return START_TIME_YEAR_INVALID; 

    strncpy(sTempMonth,sTime+4,2);sTempMonth[2]=0;
    iTempMonth=atoi(sTempMonth) ;
    if(iTempMonth>12||iTempMonth<1) return START_TIME_MONTH_INVALID; 
    
    strncpy(sTempDay,sTime+6,2); sTempDay[2]=0;
    iTempDay=atoi(sTempDay);
    if(iTempDay>31||iTempDay<1) return START_TIME_DATE_INVALID;  

    if ( (iTempMonth == 4 || iTempMonth== 6 || iTempMonth==9 || iTempMonth==11 )&&iTempDay>30)
	return START_TIME_DATE_INVALID;

    if ( iTempMonth==2 && ( (iTempYear%4!=0 && iTempDay>28) || (iTempDay>29)) )
	return START_TIME_DATE_INVALID;	
    
    strncpy(sTempHour,sTime+8,2);sTempHour[2]=0;
    iTempHour=atoi(sTempHour);
    if(iTempHour>23||iTempHour<0) return START_TIME_HOUR_INVALID; 

    strncpy(sTempMin,sTime+10,2);sTempMin[2]=0;
    iTempMin=atoi(sTempMin);
    if(iTempMin>59||iTempMin<0) return START_TIME_MINUTE_INVALID; 
    
    iTempSec=atoi(sTime+12);
    if(iTempSec>59||iTempSec<0) return START_TIME_SECOND_INVALID;
	
    return 0;
    
}


