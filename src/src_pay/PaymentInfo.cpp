#include "PaymentInfo.h"
#include "PaymentInfoSql.h"
#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"
#include "dcc_ra.h"


static vector<AbmPayCond *> g_vServ;
static vector<AbmPayCond *>::iterator g_itr;

PaymentInfo::PaymentInfo()
{
    m_poSql = new PaymentInfoSql;
    pAllocateBalanceSql =new AllocateBalanceSql;
    m_poUnpack = NULL;
    m_poPack = NULL;
    m_dccOperation = new DccOperation;	
}

PaymentInfo:: ~PaymentInfo()
{
    FreeObject(m_poSql);
    FreeObject(pAllocateBalanceSql);
    FreeObject(m_dccOperation );
}

int PaymentInfo::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
    int iRet;
    m_poUnpack = (AbmPaymentCCR *)pCCR;
    m_poPack = (AbmPaymentCCA *)pCCA;
    m_poPack->clear();
    g_vServ.clear();
    
    __DEBUG_LOG0_(0, "支付信息::unpack 消息包");
    //消息解包
    if ((iRet=m_poUnpack->getPayInfo(g_vServ)) == 0)
    {
        __DEBUG_LOG0_(0, "支付信息::deal 消息包为NULL");
        m_poPack->setRltCode(ECODE_NOMSG);
        return PAYMENT_UNPACK_ERR;
    }
    else if (iRet < 0) 
    {
        __DEBUG_LOG0_(0, "支付信息::deal 消息解包出错!");
        m_poPack->setRltCode(ECODE_UNPACK_FAIL);
        return PAYMENT_UNPACK_ERR;
    }
    for(unsigned  int i=0;i<g_vServ.size();i++)
    {
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_sOperatype=%s",i,g_vServ[i]->m_sOperatype );
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_sOperateSeq=%s",i, g_vServ[i]->m_sOperateSeq);
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_sSpID=%s",i,g_vServ[i]->m_sSpID );
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_sSpName=%s",i,g_vServ[i]->m_sSpName );
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_sServicePlatformID=%s",i, g_vServ[i]->m_sServicePlatformID);
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_iOperateAction=%d",i, g_vServ[i]->m_iOperateAction );

	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_iStaffID=%d",i,g_vServ[i]->m_iStaffID );
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_sMicropayType=%s",i, g_vServ[i]->m_sMicropayType);
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_sDestinationAccount=%s",i,g_vServ[i]->m_sDestinationAccount );
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_sDestinationtype=%s",i, g_vServ[i]->m_sDestinationtype);
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_iDestinationAttr=%d",i, g_vServ[i]->m_iDestinationAttr );

	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_sPayPassword=%s",i,g_vServ[i]->m_sPayPassword );
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_sRandomPassword=%s",i, g_vServ[i]->m_sRandomPassword);
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_sPayDate=%s",i,g_vServ[i]->m_sPayDate );
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_iPayDate=%d",i,g_vServ[i]->m_iPayDate );
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_sOrderID=%s",i, g_vServ[i]->m_sOrderID);
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_sDescription=%s",i, g_vServ[i]->m_sDescription );
	    __DEBUG_LOG2_(0,"支付信息解包:[%d].m_lPayAmount=%d",i, g_vServ[i]->m_lPayAmount );
    }

    try 
    {
        iRet=_deal(g_vServ);
        __DEBUG_LOG1_(0, "支付结果::iRet=%d", iRet);
    }
    catch(TTStatus oSt) 
    {
        __DEBUG_LOG1_(0, "支付失败::oSt.err_msg=%s", oSt.err_msg);
	m_poPack->clear();
    	m_poPack->setRltCode(ECODE_TT_FAIL);
    }
    return iRet;
}

int PaymentInfo::_deal(vector<AbmPayCond *>&vIn)
{
	//校验密码
	//检验二次验证随机密码
	//校验扣费类型8–扣费
	//根据用户标识查询账户明细，校验是否足够余额扣费(划拨)，记录业务记录信息表,
	//查询余额来源信息，每条来源信息更新此条来源的当前余额，记录余额支出表，记录余额来源支出关系表，更新余额账本表
	int iRet=0;
	bool bState=false;
	ResponsPayOne oRes={0};
	ResponsPayTwo oData={0};

	PaymentInfoALLCCR pPaymentInfoCCR={0};
	PaymentInfoALLCCA pPaymentInfoCCA={0};
	ServAcctInfo oServAcct={0};

	vector<ResponsPayTwo>vRespTwo;
	ResponsPayTwo pRespTwo={0};
	vRespTwo.clear();
	ABMException oExp;
	struDccHead ccaHead={0};
	char dccType[2]="R";
	long lRecordIdSeq=0;
	
	try 
	{

	
		iRet=m_dccOperation->QueryDccRecordSeq(lRecordIdSeq);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "支付取DCC序列错=[%d]",iRet);
			throw iRet;
		}
		memcpy((void *)&ccaHead,(void *)(m_poUnpack->m_sDccBuf),sizeof(ccaHead));
		iRet=m_dccOperation->insertDccInfo(ccaHead,dccType,lRecordIdSeq);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "支付保存DCC头错=[%d]",iRet);
			throw iRet;
		}
		
	
		for(vector<AbmPayCond *>::iterator iter=vIn.begin();iter!=vIn.end();iter++)
		{
			memset(&pPaymentInfoCCR,0,sizeof(PaymentInfoALLCCR));
			memset(&pPaymentInfoCCA,0,sizeof(PaymentInfoALLCCA));
			memset(&oServAcct,0,sizeof(oServAcct));

			strcpy(pPaymentInfoCCR.m_sOperateSeq,(*iter)->m_sOperateSeq);
			pPaymentInfoCCR.m_iBalance_unit=1;
			strcpy(pPaymentInfoCCR.m_sSpID,(*iter)->m_sSpID);
			strcpy(pPaymentInfoCCR.m_sSpName,(*iter)->m_sSpName);
			strcpy(pPaymentInfoCCR.m_sServicePlatformID,(*iter)->m_sServicePlatformID);
			pPaymentInfoCCR.m_iStaffID=(*iter)->m_iStaffID;
			strcpy(pPaymentInfoCCR.m_sMicropayType,(*iter)->m_sMicropayType);
			strcpy(pPaymentInfoCCR.m_sDestinationAccount,(*iter)->m_sDestinationAccount);
			__DEBUG_LOG1_(0, "支付号码:m_sDestinationAccount=[%s]",(*iter)->m_sDestinationAccount);
			__DEBUG_LOG1_(0, "支付号码::pPaymentInfoCCR.m_sDestinationAccount=[%s]",pPaymentInfoCCR.m_sDestinationAccount);
			strcpy(pPaymentInfoCCR.m_sDestinationtype,(*iter)->m_sDestinationtype);
			pPaymentInfoCCR.m_iDestinationAttr=(*iter)->m_iDestinationAttr;
			memset((*iter)->m_sPayDate,'\0',sizeof((*iter)->m_sPayDate));
			if(((*iter)->m_iPayDate)>0)
				long2date((*iter)->m_iPayDate,(*iter)->m_sPayDate);
			else
				GetTime((*iter)->m_sPayDate);
			__DEBUG_LOG1_(0, "支付时间::pPaymentInfoCCR.m_sPayDate=[%s]",(*iter)->m_sPayDate);
			strcpy(pPaymentInfoCCR.m_sDeductDate,(*iter)->m_sPayDate);
			strcpy(pPaymentInfoCCR.m_sOrderID,(*iter)->m_sOrderID);
			strcpy(pPaymentInfoCCR.m_sDescription,(*iter)->m_sDescription);
			pPaymentInfoCCR.m_lPayAmount=(*iter)->m_lPayAmount;
			strcpy(pPaymentInfoCCR.m_sCapabilityCode,"0003");
			strcpy(pPaymentInfoCCR.m_sOrderState,"C0C");
			//查询用户信息
			iRet=pAllocateBalanceSql->preQryServAcctInfo(pPaymentInfoCCR.m_sDestinationAccount,2,oServAcct,oExp);
			if(iRet!=0)
			{
				__DEBUG_LOG1_(0, "查询用户资料失败=[%d]",iRet);
				throw iRet;
			}
			pPaymentInfoCCR.m_lServID=oServAcct.m_servId;
			__DEBUG_LOG1_(0, "pPaymentInfoCCR.m_lServID==[%d]",pPaymentInfoCCR.m_lServID);
			//校验支付密码
			bState=true;
			iRet=m_poSql->ComparePasswd(*iter,pPaymentInfoCCR,bState);
			__DEBUG_LOG1_(0, " 校验支付密码结果=[%d]",iRet);
			if(iRet!=0)
			{
				throw iRet;
			}
			//校验二次随即密码
			bState=false;
			iRet=m_poSql->ComparePasswd(*iter,pPaymentInfoCCR,bState);
			__DEBUG_LOG1_(0, " 二次随即密码校验结果=[%d]",iRet);
			if(iRet!=0)
			{
				throw iRet;
			}
			//扣费动作
			iRet=RecodePaymentInfo( pPaymentInfoCCR,pPaymentInfoCCA);
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "支付失败!");
				throw iRet;
			}
			sprintf(pRespTwo.m_sReqSerial,"%d",pPaymentInfoCCA.m_lPaymentID);
			strcpy(pRespTwo.m_sOperateSeq,(*iter)->m_sOperateSeq);
			vRespTwo.push_back(pRespTwo);
		}

		
	}
	catch(TTStatus oSt) 
	{
		iRet=PAYMENT_TT_ERR;
		__DEBUG_LOG1_(0, "支付失败 oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "支付失败 value=%d",value);
	}
	oRes.m_iRltCode=iRet;
	if (!m_poPack->addRespons(oRes)) 
	{
		__DEBUG_LOG0_(0, "支付 m_poPack 打包失败!");
		m_poPack->setRltCode(ECODE_PACK_FAIL);
		return PAYMENT_PACK_ERR;
	}
	__DEBUG_LOG2_(0, "支付:: _deal m_poPack 打1包=[%d][%d]!",oRes.m_iRltCode,iRet);
	if( iRet!=0 )
	{
	    sprintf(pRespTwo.m_sReqSerial,"%d",-1);
	    strcpy(pRespTwo.m_sOperateSeq,vIn[0]->m_sOperateSeq);
	    vRespTwo.push_back(pRespTwo);
	    
	}
	for(unsigned int k=0;k<vRespTwo.size();k++)
	{
	    __DEBUG_LOG1_(0, "支付:: _deal m_poPack 打2包=[%s]!",vRespTwo[k].m_sReqSerial);
	    __DEBUG_LOG1_(0, "支付:: _deal m_poPack 打2包=[%s]!",vRespTwo[k].m_sOperateSeq);
		if (!m_poPack->addPayData(vRespTwo[k]))
		{
			__DEBUG_LOG0_(0, "支付:: _deal m_poPack 打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			return  PAYMENT_PACK_ERR;
		}
	}

	return iRet;
} 
//根据用户标识查询账户明细，校验是否足够余额扣费(划拨)，记录业务记录信息表,
//查询余额来源信息，每条来源信息更新此条来源的当前余额，记录余额支出表，记录余额来源支出关系表，更新余额账本表
int PaymentInfo::RecodePaymentInfo(PaymentInfoALLCCR &pPaymentInfoCCR,PaymentInfoALLCCA &pPaymentInfoCCA)
{

	bool bState=false;
	int iRet=0;
	long lPaymentAmount=0L;
	long m_lBalance=0L;
	char sDate[15];
	string  m_sOperType;


	vector<AcctBalanceInfo*>vAcctBalanceInfo;
	vector<BalanceSourceInfo *>vBalanceSourceInfo;
	vAcctBalanceInfo.clear();
	vBalanceSourceInfo.clear();
	StructPaymentSeqInfo  sStructPaymentSeqInfo={0};
	
	try 
	{

		
		//取当前系统时间
		memset(sDate,'\0',sizeof(sDate));
		GetDate(sDate);

		//查询账户明细
		iRet=m_poSql->QueryAcctBalanceInfo(pPaymentInfoCCR,vAcctBalanceInfo);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "支付，查询账户明细失败==[%d]",iRet);
			throw iRet;
		}
		__DEBUG_LOG1_(0, "支付，查询账户明细结果.size==[%d]",vAcctBalanceInfo.size());
		
		//校验是否足够扣费
		for(vector<AcctBalanceInfo*>::iterator it=vAcctBalanceInfo.begin();it!=vAcctBalanceInfo.end();it++)
		{
			if(((strlen((*it)->m_sEffDate)>=8)&&(memcmp((*it)->m_sEffDate,sDate,8)>0))||((strlen((*it)->m_sExDate)>=8)&&(memcmp((*it)->m_sExDate,sDate,8)<0)))
			{
				continue;
			}

			m_lBalance+=(*it)->m_lBalance;
		}
		if( (m_lBalance) < (pPaymentInfoCCR.m_lPayAmount) )
		{
			__DEBUG_LOG0_(0, "支付，账本余额不足以支付!");
			throw BALANCE_NOT_PAYMENT_ERR;
		}
		//查询业务流水
		iRet=m_poSql->QueryPaySeq(sStructPaymentSeqInfo,bState);
		if(iRet!=0)
        	{
			__DEBUG_LOG0_(0, "支付，查询业务流水失败!");
			throw iRet;
        	}
		pPaymentInfoCCA.m_lPaymentID=sStructPaymentSeqInfo.m_lPaymentSeq;
		//记录业务信息表
		iRet=m_poSql->InsertPaymentInfo(pPaymentInfoCCR,sStructPaymentSeqInfo);
		if(iRet!=0)
        	{
			__DEBUG_LOG0_(0, "支付，记录业务信息表失败!");
			throw iRet;
        	}
		for(vector<AcctBalanceInfo*>::iterator it=vAcctBalanceInfo.begin();it!=vAcctBalanceInfo.end();it++)
		{
			__DEBUG_LOG1_(0, " 支付，账本m_lAcctBalanceID=[%d] ",(*it)->m_lAcctBalanceID);
			FreeVector(vBalanceSourceInfo);
			//判断账户有效期
			if(((strlen((*it)->m_sEffDate)>=8)&&(memcmp((*it)->m_sEffDate,sDate,8)>0))||((strlen((*it)->m_sExDate)>=8)&&(memcmp((*it)->m_sExDate,sDate,8)<0)))
			{
			    	__DEBUG_LOG0_(0, "支付账户已失效!");
			    	__DEBUG_LOG2_(0, "支付账户已失效,生效时间=[%s],当前时间=[%s]!",(*it)->m_sEffDate,sDate);
			    	__DEBUG_LOG2_(0, "支付账户已失效,失效时间=[%s],当前时间=[%s]!",(*it)->m_sExDate,sDate);
				continue;

			}
			//查询余额来源表
			iRet=m_poSql->QueryBalanceSourceInfo((*it)->m_lAcctBalanceID,vBalanceSourceInfo);
			if(iRet!=0)
            		{
			    	__DEBUG_LOG0_(0, "支付，查询余额来源表失败!");
				throw iRet;
            		}
			//循环更新来源信息表当前余额
			vector<BalanceSourceInfo*>::iterator iter=vBalanceSourceInfo.begin();
			while((pPaymentInfoCCR.m_lPayAmount)>0&&(iter!=vBalanceSourceInfo.end()))
			{
				if( (*iter)->m_lBalance<=0 )
				{
					__DEBUG_LOG1_(0, "支付来源余额不足支付，继续寻找一下支付来源,seq=[%d]!",(*iter)->m_lOperIncomeID);
					continue;
				}
				if((pPaymentInfoCCR.m_lPayAmount)>=((*iter)->m_lBalance))
				{
					sStructPaymentSeqInfo.m_lPaymentAmount=(*iter)->m_lBalance;
					pPaymentInfoCCR.m_lPayAmount =pPaymentInfoCCR.m_lPayAmount-(*iter)->m_lBalance;
				}
				else
				{
					sStructPaymentSeqInfo.m_lPaymentAmount=pPaymentInfoCCR.m_lPayAmount;
					pPaymentInfoCCR.m_lPayAmount=0;
				}
				bState=true;
				//查询业务流水
				iRet=m_poSql->QueryPaySeq(sStructPaymentSeqInfo,bState);
				if(iRet!=0)
                		{
			        	__DEBUG_LOG0_(0, "支付，查询业务流水失败!");
					throw iRet;
                		}
				__DEBUG_LOG1_(0, "支付:m_lPaymentSeq[%d]!",sStructPaymentSeqInfo.m_lPaymentSeq);
				__DEBUG_LOG1_(0, "支付:m_lBalanceSourceSeq [%d]!",sStructPaymentSeqInfo.m_lBalanceSourceSeq);
				__DEBUG_LOG1_(0, "支付:m_lBalancePayoutSeq [%d]!",sStructPaymentSeqInfo.m_lBalancePayoutSeq);
				__DEBUG_LOG1_(0, "支付:m_lSourcePayoutSeq [%d]!",sStructPaymentSeqInfo.m_lSourcePayoutSeq);
				__DEBUG_LOG1_(0, "待支付:m_lPaymentAmount [%d]!",sStructPaymentSeqInfo.m_lPaymentAmount);
				__DEBUG_LOG1_(0, "未支付:m_lPaymentAmount [%d]!",pPaymentInfoCCR.m_lPayAmount);
				//记录余额支出表
				m_sOperType.clear();
				m_sOperType="5UI";
				iRet=m_poSql->InsertBalancePayout(sStructPaymentSeqInfo,*iter,m_sOperType);
				if(iRet!=0)
                		{
			        	__DEBUG_LOG0_(0, "支付，记录余额支出表失败!");
					throw iRet;
                		}
				//记录来源支出关系表
				iRet=m_poSql->InsertBalanceSourcePayoutRela(sStructPaymentSeqInfo,*iter);
				if(iRet!=0)
                		{	
			        	__DEBUG_LOG0_(0, "支付，记录来源支出关系表失败!");
					throw iRet;
                 		}
				//更新余额账本表
				iRet=m_poSql->updateAcctBalance(sStructPaymentSeqInfo,*it);
				if(iRet!=0)
                		{
			        	__DEBUG_LOG0_(0, "支付，更新余额账本表失败!");
					throw iRet;
                		}
				//更新余额来源表
				iRet=m_poSql->updateBalanceSource(sStructPaymentSeqInfo,*iter);
				if(iRet!=0)
                		{
			        	__DEBUG_LOG0_(0, "支付，更新余额来源表失败!");
					throw iRet;
                		}
				iter++;
			}
		}
	}
	catch(TTStatus oSt) 
	{
		iRet=PAYMENT_TT_ERR;
		__DEBUG_LOG1_(0, "支付失败::oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "支付失败:: value=%d",value);
	}
	FreeVector(vAcctBalanceInfo);
	FreeVector(vBalanceSourceInfo);
	return iRet;

}

// 支付冲正 
int PaymentInfo::ReversePayoutInfo(char *sOldRequestId,ReversePaySvc &oRvsInfo,vector<BalancePayoutInfo*> &vBalancePayOut)
{
	int iRet;
	
	long lOperPayoutId;
	long lAcctBalanceId;
	long lAmount;
	long lOptId;
	bool bFlag = false;
	
	StructPaymentSeqInfo  sStructPaymentSeqInfo={0};// 业务流水信息
	BalanceSourceInfo *pBalanceSourceInfo=NULL;
	
	AcctBalanceInfo *pAcctBalanceInfo=new AcctBalanceInfo;
	try
	{
		PaymentInfoALLCCR pPaymentInfoCCR={0};
		StructPaymentSeqInfo  sStructPaymentSeqInfo={0};
		// 生成业务流水号
		iRet = m_poSql->QueryPaySeq(sStructPaymentSeqInfo,bFlag);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "取业务流水号失败。");
			throw iRet;
		}
		
		strcpy(pPaymentInfoCCR.m_sOperateSeq ,oRvsInfo.sOperationSeq); // 外部流水号
		strcpy(pPaymentInfoCCR.m_sCapabilityCode,"0006"); // 能力编码
		strcpy(pPaymentInfoCCR.m_sServicePlatformID,oRvsInfo.sServicePlatformId);// 平台ID
		strcpy(pPaymentInfoCCR.m_sSpID,oRvsInfo.sSpId);// 商家ID
		strcpy(pPaymentInfoCCR.m_sSpName,oRvsInfo.sSpName);// 商家名称
		strcpy(pPaymentInfoCCR.m_sOrderState,"COF");
		strcpy(pPaymentInfoCCR.m_sDescription,"支付冲正");// 操作描述
		
		// todo:需增加根据外部流水号，查找业务流水号，记录被冲正业务流水号
		//char sForeignId[64]={0};
		
		//strcpy(sForeignId,ltoa(lOldRequestId));
		long lOptId;
		long lOldRequestId=0L;
		iRet = m_poSql->qryOptidByForeignId(sOldRequestId,lOptId);
		lOldRequestId = lOptId; // 
		
		__DEBUG_LOG0_(0, "根据外部流水查业务流水号");
		__DEBUG_LOG1_(0, "业务流水号：%d",lOptId);
		
		sStructPaymentSeqInfo.m_lRbkPaymentSeq = lOldRequestId; // 被冲正流水号 
		// 插入划拨冲正业务流水记录	
		/* 根据支出记录，统计冲正金额
		iRet = m_poSql->InsertPaymentInfo(pPaymentInfoCCR,sStructPaymentSeqInfo);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "插入业务流水号失败。");
			throw iRet;
		}
		*/
		
		// 1.更新被冲正流水号的状态为C0R
		iRet = m_poSql->updateOperation(lOptId);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "被冲正业务流水号状态修改失败。");
			throw iRet;
		}
		
		// 查询支出信息
		// todo:传入的应该是opt_id
		iRet=m_poSql->qryPayoutInfo(lOldRequestId,vBalancePayOut);
		
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "查询被冲正余额支出失败。");
			throw iRet;
		}
		long lTotalAmount=0L;
		for(vector<BalancePayoutInfo*>::iterator it=vBalancePayOut.begin();it!=vBalancePayOut.end();it++)
		{
			lAcctBalanceId = (*it)->m_lAcctBalanceId;
			lAmount = (*it)->m_lAmount;
			lOptId = (*it)->m_lOptId;
			lOperPayoutId = (*it)->m_lOperPayoutId;
			lTotalAmount+=lAmount;// 被冲正总金额
			__DEBUG_LOG1_(0, "余额支出lAcctBalanceId=[%ld] ",lAcctBalanceId);
			__DEBUG_LOG1_(0, "余额支出lAmount=[%ld]",lAmount);
			__DEBUG_LOG1_(0, "余额支出lOptId=[%ld]",lOptId);
			__DEBUG_LOG1_(0, "余额支出ID[%ld]",lOperPayoutId);
			
			// 查询被冲正记录对应的余额来源,余额支出ID，来源支出关系表
			iRet = m_poSql->qryBalanceSource(lOptId,pBalanceSourceInfo);
			
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "PaymentInfo::QryPayoutInfo 冲正记录余额来源不存在或查询失败");
				throw iRet;
			}
			
			
			// 更新余额来源，剩余金额+lAmount
			//pBalanceSourceInfo->m_lBalance = pBalanceSourceInfo->m_lBalance + lAmount;
			sStructPaymentSeqInfo.m_lPaymentAmount = -lAmount;		
			iRet = m_poSql->updateBalanceSource(sStructPaymentSeqInfo,pBalanceSourceInfo);
			
			// 更新余额支出记录状态10X
			iRet = m_poSql->updateBalancePayout(lOperPayoutId);
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "更新余额支出记录状态10X失败");
				throw iRet;
			}
			
			
			// 余额来源和支出记录关系表 - 更新状态10X
			iRet = m_poSql->updateBalanceSourcePayoutRela(lOperPayoutId);
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "余额来源和支出记录关系表 - 更新状态10X失败");
				throw iRet;
			}
			
			
			// 5.对应余额帐本表记录更新
			pAcctBalanceInfo->m_lAcctBalanceID = lAcctBalanceId;
			
			// 查询余额帐本
			iRet = m_poSql->qryAcctBalance(lAcctBalanceId,pAcctBalanceInfo);
			
			
			// 余额帐本金额
			iRet = m_poSql->updateAcctBalance(sStructPaymentSeqInfo,pAcctBalanceInfo);			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "余额帐本金额更新失败");
				throw iRet;
			}	

	
		}
		
		pPaymentInfoCCR.m_lPayAmount = lTotalAmount;
		// 插入划拨冲正业务流水记录	
		iRet = m_poSql->InsertPaymentInfo(pPaymentInfoCCR,sStructPaymentSeqInfo);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "插入业务流水号失败。");
			throw iRet;
		}
	}
	catch(TTStatus oSt) 
	{
		iRet=REVERSE_PAYMENT_TT_ERR;
		__DEBUG_LOG1_(0, "PaymentInfo::QryPayoutInfo oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "PaymentInfo::QryPayoutInfo value=%d",value);
	}
	
	FreeObject(pAcctBalanceInfo);
	FreeObject(pBalanceSourceInfo);
	
	return iRet;

}

// 余额划拨-划出
int PaymentInfo::DeductBalanceInfo(PaymentInfoALLCCR &pPaymentInfoCCR,PaymentInfoALLCCA &pPaymentInfoCCA)
{
	bool bState=false;
	int iRet=0;
	long lPaymentAmount=0L;
	long m_lBalance=0L;

	vector<AcctBalanceInfo*>vAcctBalanceInfo;
	vector<BalanceSourceInfo *>vBalanceSourceInfo;
	vAcctBalanceInfo.clear();
	vBalanceSourceInfo.clear();
	StructPaymentSeqInfo  sStructPaymentSeqInfo={0};
	
	try 
	{
		//查询余额帐本明细
		iRet=m_poSql->QueryAcctBalanceInfo(pPaymentInfoCCR,vAcctBalanceInfo);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "QueryAcctBalanceInfo==[%d]",iRet);
			throw iRet;
		}
		__DEBUG_LOG1_(0, "QueryAcctBalanceInfo.size==[%d]",vAcctBalanceInfo.size());
		
		//校验是否足够扣费
		for(vector<AcctBalanceInfo*>::iterator it=vAcctBalanceInfo.begin();it!=vAcctBalanceInfo.end();it++)
			m_lBalance+=(*it)->m_lBalance;
			
		__DEBUG_LOG1_(0, "余额帐本总金额m_lBalance==[%d]",m_lBalance);	
		__DEBUG_LOG1_(0, "需划出金额m_lBalance==[%d]",0-pPaymentInfoCCR.m_lPayAmount);
		
		// 帐户剩余金额
		pPaymentInfoCCA.m_lAcctBalance = m_lBalance + pPaymentInfoCCR.m_lPayAmount;
		
		if( (m_lBalance) < (0-pPaymentInfoCCR.m_lPayAmount))
		{
			__DEBUG_LOG0_(0, "余额划出 - 帐本余额不足以划拨!");
			throw -1;
		}
		//查询业务流水 - PaymentSeq
		iRet=m_poSql->QueryPaySeq(sStructPaymentSeqInfo,bState);
		if(iRet!=0)
		{
			__DEBUG_LOG0_(0, "余额划出 - 生成业务流水号失败");
			throw iRet;
		}
		// 响应信息
		pPaymentInfoCCA.m_lPaymentID=sStructPaymentSeqInfo.m_lPaymentSeq;  // 业务流水号
		
		//记录业务信息表
		iRet=m_poSql->InsertPaymentInfo(pPaymentInfoCCR,sStructPaymentSeqInfo); // 插入业务记录信息
		
		if(iRet!=0)
		{
			__DEBUG_LOG0_(0, "余额划出 - 插入业务信息表失败");
			throw iRet;
		}
		
		// 更新余额帐本金额
		long lPayAmount = 0 - pPaymentInfoCCR.m_lPayAmount; // 划拨金额
		long lBalance;
		BalanceSourceInfo balanceSourceInfo;
		for(vector<AcctBalanceInfo*>::iterator iter=vAcctBalanceInfo.begin();iter!=vAcctBalanceInfo.end();iter++)
		{
			__DEBUG_LOG1_(0, " vAcctBalanceInfo.m_lAcctBalanceID=[%d] ",(*iter)->m_lAcctBalanceID);

			lBalance = (*iter)->m_lBalance; // 余额帐本金额
			
			if(lPayAmount==0)	// 无需划拨
				break;
			
			if(lPayAmount >= lBalance)
			{
				lPayAmount = lPayAmount - lBalance; // 待划拨金额
				sStructPaymentSeqInfo.m_lPaymentAmount = lBalance;
				balanceSourceInfo.m_lAmount = 0-lBalance;
			}
			else
			{
				sStructPaymentSeqInfo.m_lPaymentAmount = lPayAmount; // 每次划拨金额
				balanceSourceInfo.m_lAmount = 0-lPayAmount; // 余额来源记录金额
				lPayAmount=0;	
			}
				
			//查询业务流水 - 余额来源记录流水
			bState = true;
			iRet=m_poSql->QueryPaySeq(sStructPaymentSeqInfo,bState);
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "余额划出 - 生成业务流水号失败");
				throw iRet;
			}

			// 更新余额帐本表 - 余额帐本扣费
			iRet=m_poSql->updateAcctBalance(sStructPaymentSeqInfo,*iter);
			if(iRet!=0)
			{
		        	__DEBUG_LOG0_(0, "余额划出 - 更新余额帐本失败");
				throw iRet;
        		}
			
			// 余额来源表插入一条对应扣费的负值
			// 余额来源数据准备
			
			// 插入余额来源记录表
			// 记录余额来源表 - 记录一笔负钱
			// 余额来源赋值
			balanceSourceInfo.m_lOperIncomeID = sStructPaymentSeqInfo.m_lBalanceSourceSeq;
			balanceSourceInfo.m_lAcctBalanceId = (*iter)->m_lAcctBalanceID;
			
			
			pPaymentInfoCCA.m_lAcctBalanceId = (*iter)->m_lAcctBalanceID; // 扣费账本标识
			pPaymentInfoCCA.m_lDeductAmountAll = sStructPaymentSeqInfo.m_lPaymentAmount; // 账本划拨金额
			// 账本剩余金额
			pPaymentInfoCCA.m_lAcctBalanceAmount = lBalance - pPaymentInfoCCA.m_lDeductAmountAll;
			
			//balanceSourceInfo.m_lAmount = -10;
			strcpy(balanceSourceInfo.m_sSourceType,"5VH");// 划出余额类型
			strcpy(balanceSourceInfo.m_sSourceDesc,"余额划拨");
			balanceSourceInfo.m_lPaymentID=sStructPaymentSeqInfo.m_lPaymentSeq;
			balanceSourceInfo.m_lBalance=balanceSourceInfo.m_lAmount;
			strcpy(balanceSourceInfo.m_sAllowDraw,"ADY");
	
			iRet = m_poSql->insertBalanceSource(balanceSourceInfo);
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "余额划出 - 插入余额来源失败");
				throw iRet;
			}	
		}
	}
	catch(TTStatus oSt) 
	{
		iRet=BALANCE_PAYOUT_TT_ERR;
		__DEBUG_LOG1_(0, "PaymentInfo::DeductBalanceInfo oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "PaymentInfo::DeductBalanceInfo value=%d",value);
	}
	FreeVector(vAcctBalanceInfo);
	FreeVector(vBalanceSourceInfo);
	return iRet;
}

// 余额划拨冲正
// 1.修改原业务流水记录状态为C0R
// 2.业务流水记录中充值一笔正钱，状态为C0F
// 3.更新余额来源，balance字段
// 4.同时更新余额帐本表
// 5.做余额支出，负钱
// 6.做余额来源支出关系记录

int PaymentInfo::ReverseDeductBalance(char* sOldRequestId,ReqReverseDeductSvc &oRvsInfo)
{
	int iRet=-1;
	
	long lOperPayoutId=0L;
	long lAcctBalanceId=0L;
	long lAmount=0L;
	long lOptId=0L;
	bool bFlag = false;
	string m_sOperType;
	
	long lBalanceSourceId=0L;
	
	long lServId=0L;
	
	
	StructPaymentSeqInfo  sStructPaymentSeqInfo={0};// 业务流水信息
	BalanceSourceInfo *pBalanceSourceInfo=NULL;
	
	AcctBalanceInfo *pAcctBalanceInfo=new AcctBalanceInfo;
	vector<BalanceSourceInfo *> vBalanceSourceInfo;
	try
	{
		PaymentInfoALLCCR pPaymentInfoCCR={0};
		StructPaymentSeqInfo  sStructPaymentSeqInfo={0};
		// 生成业务流水号
		iRet = m_poSql->QueryPaySeq(sStructPaymentSeqInfo,bFlag);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "取业务流水号失败。");
			throw iRet;
		}
		
		// 根据请求冲正流水号查找被冲正记录业务流水
		//iRet = m_poSql->QueryOptID(sOldRequestId,lOptId);
		//iRet = m_poSql->qryOptidByForeignId(sOldRequestId,lOptId);
		// Modify 2011.7.5 增加查询Serv_Id
		iRet = m_poSql->qryOptInfoByForeignId(sOldRequestId,lOptId,lServId);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "查找被冲正业务流水号失败。");
			throw iRet;
		}
		
		strcpy(pPaymentInfoCCR.m_sOperateSeq ,oRvsInfo.m_sRequestId); // 外部流水号
		strcpy(pPaymentInfoCCR.m_sCapabilityCode,"0018"); // 能力编码
		strcpy(pPaymentInfoCCR.m_sServicePlatformID,oRvsInfo.m_sABMId);// 平台ID
		strcpy(pPaymentInfoCCR.m_sDescription,"划拨冲正");// 操作描述
		strcpy(pPaymentInfoCCR.m_sOrderState,"COF");
		pPaymentInfoCCR.m_lServID= lServId; // 被冲正业务流水的用户标识
		__DEBUG_LOG1_(0, "被冲正业务流水的用户标识lServId=[%ld]",lServId);
		sStructPaymentSeqInfo.m_lRbkPaymentSeq = lOptId; // 被冲正业务流水号
		__DEBUG_LOG1_(0, "被冲正业务流水号lOptId=[%ld]",lOptId);
		
		// 1.更新被冲正流水号的状态为C0R
		iRet = m_poSql->updateOperation(lOptId);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "被冲正业务流水号状态修改失败。");
			throw iRet;
		}
		// m_lPayAmount 冲正金额，怎么获取？？？
		/*
		// 2.插入划拨冲正业务流水记录, 状态C0F	
		iRet = m_poSql->InsertPaymentInfo(pPaymentInfoCCR,sStructPaymentSeqInfo);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "插入业务流水号失败。");
			throw iRet;
		}
		*/
		
		long lTotalAmount=0L;
		
		
		// 3.根据被冲正流水号查询余额划拨时所对应的余额来源
		iRet = m_poSql->qryBalanceSource(lOptId,vBalanceSourceInfo);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "查询被冲正业务流水号余额来源信息失败。");
			throw iRet;
		}
		
		for(vector<BalanceSourceInfo*>::iterator it=vBalanceSourceInfo.begin();it!=vBalanceSourceInfo.end();it++)
		{
			// 生成余额支出、来源支出流水
			bFlag = true;
			iRet = m_poSql->QueryPaySeq(sStructPaymentSeqInfo,bFlag);
			if(iRet != 0)
			{
				__DEBUG_LOG0_(0, "取业务流水号失败。");
				throw iRet;
			}
			
			// 取余额来源基本信息
			lBalanceSourceId = (*it)->m_lOperIncomeID; // 余额来源标识
			lAcctBalanceId = (*it)->m_lAcctBalanceId; // 余额帐本标识
			lAmount = (*it)->m_lAmount;	// 金额
			
			// 重写来源流水号,此处未生成新的来源记录
			sStructPaymentSeqInfo.m_lBalanceSourceSeq = lBalanceSourceId;
			
			// 4.更新余额来源的Balance
			sStructPaymentSeqInfo.m_lPaymentAmount = lAmount; // 付款金额，即来源记录中的剩余金额
			iRet = m_poSql->updateBalanceSource(sStructPaymentSeqInfo,(*it));
			if(iRet != 0)
			{
				__DEBUG_LOG0_(0, "更新被冲正业务流水号余额来源信息失败。");
				throw iRet;
			}
			// 5.做余额支出记录 ***
			m_sOperType.clear();
			m_sOperType="5UK";
			iRet = m_poSql->InsertBalancePayout(sStructPaymentSeqInfo,(*it),m_sOperType);
			if(iRet != 0)
			{
				__DEBUG_LOG0_(0, "插入被冲正业务流水号余额支出信息失败。");
				throw iRet;
			}
			// 6.做来源支出记录
			iRet = m_poSql->InsertBalanceSourcePayoutRela(sStructPaymentSeqInfo,(*it));
			if(iRet != 0)
			{
				__DEBUG_LOG0_(0, "插入被冲正业务流水号余额来源支出信息失败。");
				throw iRet;
			}
			// 7.更新余额帐本记录
			long ltmp = 0-lAmount;
			iRet = m_poSql->updateAcctBalance(lAcctBalanceId,ltmp);
			if(iRet != 0)
			{
				__DEBUG_LOG0_(0, "更新余额帐本信息失败。");
				throw iRet;
			}
			
			lTotalAmount +=ltmp;
		}
		
		// 2.插入划拨冲正业务流水记录, 状态C0F	
		pPaymentInfoCCR.m_lPayAmount = lTotalAmount;
		iRet = m_poSql->InsertPaymentInfo(pPaymentInfoCCR,sStructPaymentSeqInfo);
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "插入业务流水号失败。");
			throw iRet;
		}
			
		/*
		// 查询支出信息
		iRet=m_poSql->qryPayoutInfo(lOldRequestId,vBalancePayOut);
		
		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "查询被冲正余额支出失败。");
			throw iRet;
		}
		
		for(vector<BalancePayoutInfo*>::iterator it=vBalancePayOut.begin();it!=vBalancePayOut.end();it++)
		{
			lAcctBalanceId = (*it)->m_lAcctBalanceId;
			lAmount = (*it)->m_lAmount;
			lOptId = (*it)->m_lOptId;
			lOperPayoutId = (*it)->m_lOperPayoutId;
			
			__DEBUG_LOG1_(0, "余额支出lAcctBalanceId=[%ld] ",lAcctBalanceId);
			__DEBUG_LOG1_(0, "余额支出lAmount=[%ld]",lAmount);
			__DEBUG_LOG1_(0, "余额支出lOptId=[%ld]",lOptId);
			__DEBUG_LOG1_(0, "余额支出ID[%ld]",lOperPayoutId);
			
			// 查询被冲正记录对应的余额来源,余额支出ID，来源支出关系表
			iRet = m_poSql->qryBalanceSource(lOptId,pBalanceSourceInfo);
			
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "PaymentInfo::QryPayoutInfo 冲正记录余额来源不存在或查询失败");
				throw iRet;
			}
			
			
			// 更新余额来源，剩余金额+lAmount
			//pBalanceSourceInfo->m_lBalance = pBalanceSourceInfo->m_lBalance + lAmount;
			sStructPaymentSeqInfo.m_lPaymentAmount = -lAmount;		
			iRet = m_poSql->updateBalanceSource(sStructPaymentSeqInfo,pBalanceSourceInfo);
			
			// 更新余额支出记录状态10X
			iRet = m_poSql->updateBalancePayout(lOperPayoutId);
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "更新余额支出记录状态10X失败");
				throw iRet;
			}
			
			
			// 余额来源和支出记录关系表 - 更新状态10X
			iRet = m_poSql->updateBalanceSourcePayoutRela(lOperPayoutId);
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "余额来源和支出记录关系表 - 更新状态10X失败");
				throw iRet;
			}
			
			
			// 5.对应余额帐本表记录更新
			pAcctBalanceInfo->m_lAcctBalanceID = lAcctBalanceId;
			
			// 查询余额帐本
			iRet = m_poSql->qryAcctBalance(lAcctBalanceId,pAcctBalanceInfo);
			
			
			// 余额帐本金额
			iRet = m_poSql->updateAcctBalance(sStructPaymentSeqInfo,pAcctBalanceInfo);
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "余额帐本金额更新失败");
				throw iRet;
			}	
		}
		*/
	}
	catch(TTStatus oSt) 
	{
		iRet=REVER_BALANCE_PAYOUT_TT_ERR;
		__DEBUG_LOG1_(0, "PaymentInfo::QryPayoutInfo oSt.err_msg=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "PaymentInfo::QryPayoutInfo value=%d",value);
	}
	
	FreeObject(pAcctBalanceInfo);
	FreeObject(pBalanceSourceInfo);
	
	return iRet;
}


