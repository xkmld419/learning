#include "ReverseDepositInfo.h"
#include "PaymentInfoSql.h"
#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"


static vector<ReverseDeposit *> g_vServInfo;
static vector<ReverseDeposit *>::iterator g_itrInfo;

ReverseDepositInfo::ReverseDepositInfo()
{
    m_poSql = new PaymentInfoSql;
    pAllocateBalanceSql =new AllocateBalanceSql;
    m_poUnpack = NULL;
    m_poPack = NULL;
    m_dccOperation = new DccOperation;	
}

ReverseDepositInfo:: ~ReverseDepositInfo()
{
    FreeObject(m_poSql);
    FreeObject(pAllocateBalanceSql);
    FreeObject(m_dccOperation );
}

int ReverseDepositInfo::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
    int iRet=0;
    
    m_poUnpack = (ReverseDepositInfoCCR *)pCCR;
    m_poPack = (ReverseDepositInfoCCA *)pCCA;
    m_poPack->clear();
    g_vServInfo.clear();
    ResponsReverseDeposit oRes={0};
    try 
    {
    
	    __DEBUG_LOG0_(0, "充值冲正，解包消息包");
	    //消息解包
	    if ((iRet=m_poUnpack->getReverseDepositInfo(g_vServInfo)) == 0)
	    {
		__DEBUG_LOG0_(0, "充值冲正，解包 消息包为NULL");
		m_poPack->setRltCode(ECODE_NOMSG);
		throw REVERSE_DEP_UNPACK_ERR;
	    }
	    else if (iRet < 0) 
	    {
		__DEBUG_LOG0_(0, "充值冲正， 消息解包出错!");
		m_poPack->setRltCode(ECODE_UNPACK_FAIL);
		throw REVERSE_DEP_UNPACK_ERR;
	    }
	    for(unsigned  int i=0;i<g_vServInfo.size();i++)
	    {
		    __DEBUG_LOG2_(0,"充值冲正解包:[%d].m_sReqSerial=%s",i,g_vServInfo[i]->m_sReqSerial );
		    __DEBUG_LOG2_(0,"充值冲正解包:[%d].m_sPaymentID=%s",i, g_vServInfo[i]->m_sPaymentID);
		    __DEBUG_LOG2_(0,"充值冲正解包:[%d].m_sDestinationType=%s",i, g_vServInfo[i]->m_sDestinationType);
		    __DEBUG_LOG2_(0,"充值冲正解包:[%d].m_sDestinationAccount=%s",i,g_vServInfo[i]->m_sDestinationAccount );
		    __DEBUG_LOG2_(0,"充值冲正解包:[%d].m_iDestinationAttr=%d",i, g_vServInfo[i]->m_iDestinationAttr);
	    }

	    for(g_itrInfo=g_vServInfo.begin();g_itrInfo!=g_vServInfo.end();g_itrInfo++)
	    {
			iRet=_deal(*g_itrInfo);
			if(iRet!=0)
				throw iRet;
	    }
	    __DEBUG_LOG0_(0, "充值冲正: _deal  打包!");
	    oRes.m_iRltCode=0;
	    strcpy(oRes.m_sReqSerial,g_vServInfo[0]->m_sReqSerial);
	    if (!m_poPack->addRespReverseDepositCCA(oRes)) 
	    {
		__DEBUG_LOG0_(0, "充值冲正:: _deal m_poPack 打包失败!");
		m_poPack->setRltCode(ECODE_PACK_FAIL);
		throw REVERSE_DEP_PACK_ERR;
	    }

    }
    catch(TTStatus oSt) 
    {
	iRet=REVERSE_DEP_TT_ERR;
        __DEBUG_LOG1_(0, "充值冲正失败::deal oSt.err_msg=%s", oSt.err_msg);
    }
    catch(int &value)
    {
	iRet=value;
        __DEBUG_LOG1_(0, "充值冲正失败::deal value=%d", value);
	
    }
    if(iRet!=0)
    {
	     __DEBUG_LOG0_(0, "充值冲正失败,组失败包!");
	    oRes.m_iRltCode=iRet;
	    strcpy(oRes.m_sReqSerial,g_vServInfo[0]->m_sReqSerial);
	    if (!m_poPack->addRespReverseDepositCCA(oRes)) 
	    {
		__DEBUG_LOG0_(0, "充值冲正失败:: _deal m_poPack 打包失败!");
		m_poPack->setRltCode(ECODE_PACK_FAIL);
	    }
    }
    return iRet;
}

int ReverseDepositInfo::_deal(ReverseDeposit *&pInOne)
{
	//充值冲正，
	//1：查资料，
	//
	int iRet=0;
	ServAcctInfo oServAcct={0};
	
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
			__DEBUG_LOG1_(0, "充值冲正保存DCC头错=[%d]",iRet);
			throw iRet;
		}
		//查询用户信息
		iRet=pAllocateBalanceSql->preQryServAcctInfo(pInOne->m_sDestinationAccount,2,oServAcct,oExp);
		switch(iRet)
		{
		
			case 0://有资料
				{
					__DEBUG_LOG0_(0, "充值冲正，找到用户资料");
					iRet=DoReverseDeposit(pInOne,oServAcct);
					if(iRet!=0)
					{
						__DEBUG_LOG1_(0, "已有用户资料，充值冲正  失败==[%d]",iRet);
						throw iRet;
					}
				}
				break;
			default://出错
				{
					__DEBUG_LOG1_(0, "充值冲正，查询用户资料出错=[%d]",iRet);
					throw iRet;
				}
		}
	
		return 0;

		
	}
	catch(TTStatus oSt) 
	{
		iRet=REVERSE_DEP_TT_ERR;
		__DEBUG_LOG1_(0, "充值冲正失败，错误信息=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "充值冲正失败，错误信息=%d",value);
	}
	

	return iRet;
} 

int ReverseDepositInfo::DoReverseDeposit(ReverseDeposit *&pInOne,ServAcctInfo &oServAcct)
{
	//2：记录业务操作表，同时更新源记录状态及时间
	//3：查询余额来源表，判断充值金额与余额是否有变化，
	//4：更新余额来源余额为0及时间，
	//5：更新余额账本状态及金额，
	//6：记录余额支出表，
	//7：记录来源支持关系表，
	int iRet=0;
	bool bState=false;
	string m_sOperType;
	StructPaymentSeqInfo sStructPaymentSeqInfo={0};
	AcctBalanceInfo *pAcctBalanceInfo=NULL;
	vector<BalanceSourceInfo *>vBalanceSourceInfo;
	vBalanceSourceInfo.clear();
	PaymentInfoALLCCR pPaymentInfoCCR={0};
	long lReversePaymentID=0L,lPaymentID=0L;
	try
	{
		iRet=m_poSql->QueryOptID(pInOne->m_sPaymentID,lPaymentID);
		if(iRet!=1)
		{
			__DEBUG_LOG0_(0, "查询原业务记录opt_id失败!");
			throw REVERSE_DEP_QRY_OPTID_ERR;
		}
		iRet=m_poSql->QryPaymentInfo(lPaymentID,pPaymentInfoCCR);
		if(iRet!=1)
		{
			__DEBUG_LOG0_(0, "查询原业务记录失败!");
			throw REVERSE_DEP_QRY_DATA_ERR;
		}

		iRet=m_poSql->QueryPaySeq(sStructPaymentSeqInfo,bState);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "冲正，查询序列失败  =[%d]",iRet);
			throw iRet;
		}
		lReversePaymentID=sStructPaymentSeqInfo.m_lPaymentSeq;

		//记录业务表
		memset(pPaymentInfoCCR.m_sOperateSeq,'\0',sizeof(pPaymentInfoCCR.m_sOperateSeq));
		strcpy(pPaymentInfoCCR.m_sOperateSeq,pInOne->m_sReqSerial);

		memset(pPaymentInfoCCR.m_sCapabilityCode,'\0',sizeof(pPaymentInfoCCR.m_sCapabilityCode));
		strcpy(pPaymentInfoCCR.m_sCapabilityCode,"0017");

		sStructPaymentSeqInfo.m_lRbkPaymentSeq=lPaymentID;

		memset(pPaymentInfoCCR.m_sDescription,'\0',sizeof(pPaymentInfoCCR.m_sDescription));
		strcpy(pPaymentInfoCCR.m_sDescription,"充值冲正");

		memset(pPaymentInfoCCR.m_sOrderState,'\0',sizeof(pPaymentInfoCCR.m_sOrderState));
		strcpy(pPaymentInfoCCR.m_sOrderState,"C0F");
		//记录F级别业务流水
		iRet=m_poSql->InsertPaymentInfo(pPaymentInfoCCR,sStructPaymentSeqInfo);
		if(iRet!=0)
		{
			__DEBUG_LOG0_(0, "记录业务记录失败!");
			throw iRet;
		}

		//更新业务流水
		iRet=m_poSql->updateOperation(lPaymentID);
		if(iRet!=0)
		{
			__DEBUG_LOG0_(0, "更新原业务记录失败!");
			throw iRet;
		}

		//查询余额来源	
		iRet=m_poSql->qryBalanceSource(lPaymentID,vBalanceSourceInfo);
		if(iRet!=0)
		{
			__DEBUG_LOG0_(0, "查询余额来源记录失败!");
			throw iRet;
		}

		//未找到符合条件的余额来源记录
		if(vBalanceSourceInfo.size()<=0)
		{
			__DEBUG_LOG0_(0, "未找到符合条件的余额来源记录!");
			throw -1;
		}
		for(vector<BalanceSourceInfo *>::iterator it=vBalanceSourceInfo.begin();it!=vBalanceSourceInfo.end();++it)
		{
			memset(&sStructPaymentSeqInfo,0,sizeof(StructPaymentSeqInfo));
			if((*it)->m_lAmount!=(*it)->m_lBalance)
			{
				__DEBUG_LOG0_(0, "充值冲正余额来源金额已发生变化!");
				throw REVERSE_DEP_BALANCE_CHANGE_ERR;
			}

			//更新余额来源金额为0
			sStructPaymentSeqInfo.m_lPaymentAmount=(*it)->m_lBalance;
			iRet=m_poSql->updateBalanceSource(sStructPaymentSeqInfo,*it);
			if(iRet!=0)
			{
				__DEBUG_LOG1_(0, "充值冲正，更新更新余额来源失败[%d]!",iRet);
				throw iRet;
			}

			pAcctBalanceInfo =new AcctBalanceInfo;
			iRet=m_poSql->qryAcctBalance((*it)->m_lAcctBalanceId,pAcctBalanceInfo);
			if(iRet!=1)
			{
				__DEBUG_LOG2_(0, "充值冲正，查找余额账本失败  =[%d],acct_balance_id=[%d]",iRet,(*it)->m_lAcctBalanceId);
				throw iRet;
			}

			pAcctBalanceInfo->m_lBalance=(-1)*((*it)->m_lBalance);

			__DEBUG_LOG2_(0, "充值冲正，更新用户余额账本金额  =[%ld],[%ld]",pAcctBalanceInfo->m_lBalance,(*it)->m_lBalance);
			iRet=m_poSql->updateAcctBalance((*it)->m_lAcctBalanceId,pAcctBalanceInfo->m_lBalance);
			if(iRet!=0)
			{
				__DEBUG_LOG1_(0, "充值冲正，更新用户余额账本失败  =[%d]",iRet);
				throw iRet;
			}
			bState=true;
			iRet=m_poSql->QueryPaySeq(sStructPaymentSeqInfo,bState);
			if(iRet!=0)
			{
				__DEBUG_LOG1_(0, "充值冲正，查询序列失败  =[%d]",iRet);
				throw iRet;
			}
			sStructPaymentSeqInfo.m_lPaymentSeq=lReversePaymentID;
			//记录支出表
			m_sOperType.clear();
			m_sOperType="5UH";
			iRet=m_poSql->InsertBalancePayout(sStructPaymentSeqInfo,*it,m_sOperType);
			if(iRet!=0)
			{
				__DEBUG_LOG1_(0, "充值冲正，记录余额支出失败  =[%d]",iRet);
				throw iRet;
			}
			//记录来源支出关系表
			iRet=m_poSql->InsertBalanceSourcePayoutRela(sStructPaymentSeqInfo,*it);
			if(iRet!=0)
			{
				__DEBUG_LOG1_(0, "充值冲正，记录余额来源支出关系失败  =[%d]",iRet);
				throw iRet;
			}
			FreeObject(pAcctBalanceInfo);

		}
		FreeVector(vBalanceSourceInfo);
		
		return 0;
	}
	catch(TTStatus oSt) 
	{
		iRet=REVERSE_DEP_TT_ERR;
		__DEBUG_LOG1_(0, "充值冲正失败,错误信息=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "充值冲正失败,错误信息=%d",value);
	}
	FreeObject(pAcctBalanceInfo);
	FreeVector(vBalanceSourceInfo);

	return iRet;
}




