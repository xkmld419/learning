#include "DepositInfo.h"
#include "PaymentInfoSql.h"
#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"


static vector<DepositServCondOne *> g_vServInfo;
static vector<DepositServCondTwo *> g_vServData;
static vector<DepositServCondOne *>::iterator g_itrInfo;
static vector<DepositServCondTwo *>::iterator g_itrData;
vector<long>vAcctBalanceID;

DepositInfo::DepositInfo()
{
    m_poSql = new PaymentInfoSql;
    pAllocateBalanceSql =new AllocateBalanceSql;
    m_poUnpack = NULL;
    m_poPack = NULL;
    m_dccOperation = new DccOperation;
}

DepositInfo:: ~DepositInfo()
{
    FreeObject(m_poSql);
    FreeObject(pAllocateBalanceSql);
    FreeObject(m_dccOperation );
}

int DepositInfo::deal(ABMCCR *pCCR, ABMCCA *pCCA)
{
    int iRet;
    string sEffDate,sExDate;
    PaymentInfoALLCCR pPaymentInfoCCR={0};
    vAcctBalanceID.clear();
    
    m_poUnpack = (DepositInfoCCR *)pCCR;
    m_poPack = (DepositInfoCCA *)pCCA;
    m_poPack->clear();
    g_vServInfo.clear();
    g_vServData.clear();
    ResponsDepositOne oRes={0};
    ResponsDepositTwo pDepositTwo={0};
    vector<AcctBalanceInfo*>vAcctBalanceInfo;
    vAcctBalanceInfo.clear();
    try 
    {
    
	    __DEBUG_LOG0_(0, "vc充值 消息包");
	    //消息解包
	    if ((iRet=m_poUnpack->getDepositOneCCR(g_vServInfo)) == 0)
	    {
		__DEBUG_LOG0_(0, "vc充值::deal 消息包为NULL");
		m_poPack->setRltCode(ECODE_NOMSG);
		throw DEPOSIT_UNPACK_ERR;
	    }
	    else if (iRet < 0) 
	    {
		__DEBUG_LOG0_(0, "vc充值::deal 消息解包出错!");
		m_poPack->setRltCode(ECODE_UNPACK_FAIL);
		throw DEPOSIT_UNPACK_ERR;
	    }
	    for(unsigned  int i=0;i<g_vServInfo.size();i++)
	    {
		    g_vServInfo[i]->m_iBonusStartTime=0;
		    g_vServInfo[i]->m_iBonusExpirationTime=0;
		    /*
		    if(strlen(g_vServInfo[i]->m_sBalanceType)<=0)
		    	memset(g_vServInfo[i]->m_sBalanceType,'\0',sizeof(g_vServInfo[i]->m_sBalanceType));
		    */
		    if(g_vServInfo[i]->m_iBalanceType<=0)
			    g_vServInfo[i]->m_iBalanceType=10;
				
		    __DEBUG_LOG2_(0,"vc充值解包:[%d].m_sReqSerial=%s",i,g_vServInfo[i]->m_sReqSerial );
		    //__DEBUG_LOG2_(0,"vc充值解包:[%d].m_sBalanceType=%s",i, g_vServInfo[i]->m_sBalanceType);
		    __DEBUG_LOG2_(0,"vc充值解包:[%d].m_sBalanceType=%d",i, g_vServInfo[i]->m_iBalanceType);
		    __DEBUG_LOG2_(0,"vc充值解包:[%d].m_iDestinationBalanceUnit=%d",i,g_vServInfo[i]->m_iDestinationBalanceUnit );
		    __DEBUG_LOG2_(0,"vc充值解包:[%d].m_iVoucherValue=%d",i, g_vServInfo[i]->m_iVoucherValue);
		    __DEBUG_LOG2_(0,"vc充值解包:[%d].m_iProlongDays=%d",i, g_vServInfo[i]->m_iProlongDays );

		    __DEBUG_LOG2_(0,"vc充值解包:[%d].m_sDestinationType=%s",i, g_vServInfo[i]->m_sDestinationType);
		    __DEBUG_LOG2_(0,"vc充值解包:[%d].m_sDestinationAccount=%s",i,g_vServInfo[i]->m_sDestinationAccount );
		    __DEBUG_LOG2_(0,"vc充值解包:[%d].m_iDestinationAttr=%d",i, g_vServInfo[i]->m_iDestinationAttr);
		    __DEBUG_LOG2_(0,"vc充值解包:[%d].m_sAreaCode=%s",i, g_vServInfo[i]->m_sAreaCode);
	    }

	    for(g_itrInfo=g_vServInfo.begin();g_itrInfo!=g_vServInfo.end();g_itrInfo++)
	    {
        	iRet=m_poUnpack->getDepositTwoCCR(*g_itrInfo,g_vServData);
        	__DEBUG_LOG1_(0, "vc充值第二层包数据::_deal iRet=%d", iRet);
        	if ( iRet>= 0)
		{
			iRet=_deal(*g_itrInfo,g_vServData);
			if(iRet!=0)
				throw iRet;
		}
		else
		{
			throw iRet;
		}
	    }
	    __DEBUG_LOG0_(0, "vc充值成功:: 打1包!");
	    oRes.m_iRltCode=0;
	    strcpy(oRes.m_sReqSerial,g_vServInfo[0]->m_sReqSerial);
	    if (!m_poPack->addRespDepositOneCCA(oRes)) 
	    {
		__DEBUG_LOG0_(0, "vc充值:: _deal m_poPack 打1包失败!");
		m_poPack->setRltCode(ECODE_PACK_FAIL);
		throw DEPOSIT_PACK_ERR;
	    }
	    __DEBUG_LOG1_(0, "vc充值账户明细条数:[%d]",vAcctBalanceID.size());
    	    AcctBalanceInfo *m_pAcctBalanceInfo=new AcctBalanceInfo;
	    if(!m_pAcctBalanceInfo)
	    {
	    	__DEBUG_LOG0_(0, "vc充值分配内存出错");
	    	throw ECODE_MEM_MALLOC_FAIL;
	    }
	    for(unsigned int ii=0;ii<vAcctBalanceID.size();ii++)
	    {
		m_pAcctBalanceInfo->clear();
	    	__DEBUG_LOG2_(0, "vc充值账户明细:[%d][%ld]",ii,vAcctBalanceID[ii]);
	    	//查询账户明细
		iRet=0;
	    	iRet=m_poSql->qryAcctBalance(vAcctBalanceID[ii],m_pAcctBalanceInfo);
		if(iRet!=1)
		{
			__DEBUG_LOG1_(0, "查询账户明细,失败=[%d]",iRet);
			throw iRet;
		}
		sEffDate.clear();
		sExDate.clear();
		memset(&pDepositTwo,0,sizeof(ResponsDepositTwo));
		sEffDate=m_pAcctBalanceInfo->m_sEffDate;
		sExDate=m_pAcctBalanceInfo->m_sExDate;

		pDepositTwo.m_iDestinationBalance=m_pAcctBalanceInfo->m_lBalance;
		//sprintf(pDepositTwo.m_sBalanceType,"%d",m_pAcctBalanceInfo->m_lBalanceTypeID);
		pDepositTwo.m_iBalanceType=m_pAcctBalanceInfo->m_lBalanceTypeID;
		pDepositTwo.m_iDestinationBalanceUnit=0;
		pDepositTwo.m_iDestinationEffectiveTime=atoi(sEffDate.substr(0,8).c_str());
		pDepositTwo.m_iDestinationExpirationTime=atoi(sExDate.substr(0,8).c_str());

		__DEBUG_LOG1_(0, "vc充值返回包:: m_iDestinationBalance 打2包=[%d]!",pDepositTwo.m_iDestinationBalance);
		//__DEBUG_LOG1_(0, "vc充值返回包:: m_sBalanceType 打2包=[%s]!",pDepositTwo.m_sBalanceType);
		__DEBUG_LOG1_(0, "vc充值返回包:: m_iBalanceType 打2包=[%d]!",pDepositTwo.m_iBalanceType);
		__DEBUG_LOG1_(0, "vc充值返回包:: m_iBalanceUnit 打2包=[%d]!",pDepositTwo.m_iDestinationBalanceUnit);
		__DEBUG_LOG1_(0, "vc充值返回包:: m_iEff 打2包=[%d]!",pDepositTwo.m_iDestinationEffectiveTime);
		__DEBUG_LOG1_(0, "vc充值返回包:: m_iExp 打2包=[%d]!",pDepositTwo.m_iDestinationExpirationTime);

		if (!m_poPack->addRespDepositTwoCCA(pDepositTwo))
		{
			__DEBUG_LOG0_(0, "vc充值返回包:: _deal m_poPack 打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw  DEPOSIT_PACK_ERR;
		}
		iRet=0;
	    }

	    /*
	    strcpy(pPaymentInfoCCR.m_sDestinationAccount,g_vServInfo[0]->m_sDestinationAccount);
	    //查询账户明细
	    iRet=m_poSql->QueryAcctBalanceInfo(pPaymentInfoCCR,vAcctBalanceInfo);
	    if(iRet!=0)
	    {
			__DEBUG_LOG1_(0, "查询账户明细,失败=[%d]",iRet);
			throw iRet;
	    }
	    for(vector<AcctBalanceInfo*>::iterator it=vAcctBalanceInfo.begin();it!=vAcctBalanceInfo.end();it++)
	    {
		sEffDate.clear();
		sExDate.clear();
		memset(&pDepositTwo,0,sizeof(ResponsDepositTwo));
		sEffDate=(*it)->m_sEffDate;
		sExDate=(*it)->m_sExDate;

		pDepositTwo.m_iDestinationBalance=(*it)->m_lBalance;
		sprintf(pDepositTwo.m_sBalanceType,"%d",(*it)->m_lBalanceTypeID);
		pDepositTwo.m_iDestinationBalanceUnit=0;
		pDepositTwo.m_iDestinationEffectiveTime=atoi(sEffDate.substr(0,8).c_str());
		pDepositTwo.m_iDestinationExpirationTime=atoi(sExDate.substr(0,8).c_str());

		__DEBUG_LOG1_(0, "vc充值返回包:: m_iDestinationBalance 打2包=[%d]!",pDepositTwo.m_iDestinationBalance);
		__DEBUG_LOG1_(0, "vc充值返回包:: m_sBalanceType 打2包=[%s]!",pDepositTwo.m_sBalanceType);
		__DEBUG_LOG1_(0, "vc充值返回包:: m_iBalanceUnit 打2包=[%d]!",pDepositTwo.m_iDestinationBalanceUnit);
		__DEBUG_LOG1_(0, "vc充值返回包:: m_iEff 打2包=[%d]!",pDepositTwo.m_iDestinationEffectiveTime);
		__DEBUG_LOG1_(0, "vc充值返回包:: m_iExp 打2包=[%d]!",pDepositTwo.m_iDestinationExpirationTime);

		if (!m_poPack->addRespDepositTwoCCA(pDepositTwo))
		{
			__DEBUG_LOG0_(0, "vc充值返回包:: _deal m_poPack 打包失败!");
			m_poPack->setRltCode(ECODE_PACK_FAIL);
			throw  DEPOSIT_PACK_ERR;
		}
	   }
	   */

    }
    catch(TTStatus oSt) 
    {
        __DEBUG_LOG1_(0, "vc充值失败:: 失败信息=%s", oSt.err_msg);
    }
    catch(int &value)
    {
	iRet=value;
        __DEBUG_LOG1_(0, "vc充值失败:: 失败信息 value=%d", value);
	
    }
    if(iRet!=0)
    {
	     __DEBUG_LOG0_(0, "充值失败!");
	    oRes.m_iRltCode=iRet;
	    strcpy(oRes.m_sReqSerial,g_vServInfo[0]->m_sReqSerial);
	    if (!m_poPack->addRespDepositOneCCA(oRes)) 
	    {
		__DEBUG_LOG0_(0, "DepositInfo:: _deal m_poPack 打包失败!");
		m_poPack->setRltCode(ECODE_PACK_FAIL);
	    }
    }
    for(vector<AcctBalanceInfo*>::iterator it=vAcctBalanceInfo.begin();it!=vAcctBalanceInfo.end();it++)
    {
	delete *it;
    }
    vAcctBalanceInfo.clear();
    
    return iRet;
}

int DepositInfo::_deal(DepositServCondOne *&pInOne,vector<DepositServCondTwo *>&vInTwo)
{
	//充值，开户
	//校验资料是否存在（否，先开户）,充值
	int iRet=0;
	bool bState=false;
	ServAcctInfo oServAcct={0};
	ServAcctInfoSeq oServAcctInfoSeq={0};
	ServAcctInfoSeq oSendSeq={0};
	DepositServCondOne   *pInTwo=NULL;
	PaymentInfoALLCCR pPaymentInfoCCR={0};
	StructPaymentSeqInfo  sStructPaymentSeqInfo={0};
	
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
			__DEBUG_LOG1_(0, "充值保存DCC头错=[%d]",iRet);
			throw iRet;
		}
		//普通充值
		//查询用户信息
		iRet=pAllocateBalanceSql->preQryServAcctInfo(pInOne->m_sDestinationAccount,2,oServAcct,oExp);
		switch(iRet)
		{
			case 1://无资料
				{
					__DEBUG_LOG0_(0, "没有资料，开始创建用户信息,然后充值  ");
					iRet=CreateServAcctInfo(pInOne,oServAcctInfoSeq);
					if(iRet!=0)
					{
						__DEBUG_LOG1_(0, "创建用户信息  失败==[%d]",iRet);
						throw iRet;
					}
				}
				break;
			case 0://有资料
				{
					__DEBUG_LOG0_(0, "已有用户资料，充值");
					oServAcctInfoSeq.m_lServID=oServAcct.m_servId;
					oServAcctInfoSeq.m_lAcctID=oServAcct.m_acctId;
					iRet=UpdateAcctInfo(pInOne,oServAcctInfoSeq);
					if(iRet!=0)
					{
						__DEBUG_LOG1_(0, "已有用户资料，充值  失败==[%d]",iRet);
						throw iRet;
					}
				}
				break;
			default://出错
				{
					__DEBUG_LOG1_(0, "查询用户资料出错=[%d]",iRet);
					throw iRet;
				}
		}
	
		//查询业务流水
		bState=false;
		iRet=m_poSql->QueryPaySeq(sStructPaymentSeqInfo,bState);
		if(iRet!=0)
        	{
			__DEBUG_LOG0_(0, "查询业务流水失败  error");
			throw iRet;
        	}
		//查询其他序列
		bState=true;
		iRet=m_poSql->QueryPaySeq(sStructPaymentSeqInfo,bState);
		if(iRet!=0)
        	{
			__DEBUG_LOG0_(0, "查询其他序列出错  error");
			throw iRet;
        	}
		//记录业务表
		strcpy(pPaymentInfoCCR.m_sOperateSeq,pInOne->m_sReqSerial);
		strcpy(pPaymentInfoCCR.m_sCapabilityCode,"0001");
		pPaymentInfoCCR.m_lServID=oServAcctInfoSeq.m_lServID;
		sStructPaymentSeqInfo.m_lRbkPaymentSeq=0;
		strcpy(pPaymentInfoCCR.m_sSpID,"1");
		strcpy(pPaymentInfoCCR.m_sServicePlatformID,"1");
		strcpy(pPaymentInfoCCR.m_sOrderID,"1");
		strcpy(pPaymentInfoCCR.m_sDescription,"VC充值");
		strcpy(pPaymentInfoCCR.m_sMicropayType,"0");
		strcpy(pPaymentInfoCCR.m_sOrderState,"C0C");
		pPaymentInfoCCR.m_lPayAmount=pInOne->m_iVoucherValue;
		iRet=m_poSql->InsertPaymentInfo(pPaymentInfoCCR,sStructPaymentSeqInfo);
		if(iRet!=0)
        	{
			__DEBUG_LOG0_(0, "记录业务流水表出错!");
			throw iRet;
        	}
		
		sStructPaymentSeqInfo.m_lAcctBalanceID=oServAcctInfoSeq.m_lAcctBalanceID;
		strcpy(pInOne->m_sSourceType,"5VA");
		strcpy(pInOne->m_sSourceDesc,"VC普通充值");
		strcpy(pInOne->m_sAllowDraw,"ADY");
		//记录来源表
		iRet=m_poSql->InsertBalanceSource(pInOne,sStructPaymentSeqInfo);
		if(iRet!=0)
        	{
			__DEBUG_LOG0_(0, "记录余额来源表失败! ");
			throw iRet;
        	}
		vAcctBalanceID.push_back(sStructPaymentSeqInfo.m_lAcctBalanceID);

		//优惠充值,只记录账本+来源表
		for(vector<DepositServCondTwo *>::iterator iter=vInTwo.begin();iter!=vInTwo.end();iter++)
		{

			__DEBUG_LOG0_(0, "充值存在优惠,开始优惠充值");
			__DEBUG_LOG1_(0, "优惠数据 (*iter)->m_iBonusUnit=[%d]",(*iter)->m_iBonusUnit);
			//__DEBUG_LOG1_(0, "优惠数据 (*iter)->m_sBalanceType=[%s]",(*iter)->m_sBalanceType);
			__DEBUG_LOG1_(0, "优惠数据 (*iter)->m_iBalanceType=[%d]",(*iter)->m_iBalanceType);
			__DEBUG_LOG1_(0, "优惠数据 (*iter)->m_iBonusStartTime=[%d]",(*iter)->m_iBonusStartTime);
			__DEBUG_LOG1_(0, "优惠数据 (*iter)->m_iBonusExpirationTime=[%d]",(*iter)->m_iBonusExpirationTime);
			__DEBUG_LOG1_(0, "优惠数据 (*iter)->m_iBonusAmount=[%d]",(*iter)->m_iBonusAmount);
			pInTwo=new DepositServCondOne;
			pInTwo->m_iDestinationBalanceUnit=(*iter)->m_iBonusUnit;//优惠额单位，0－分，1－条
			pInTwo->m_iVoucherValue=(*iter)->m_iBonusAmount;//赠与的优惠金额
			//strcpy(pInTwo->m_sBalanceType,(*iter)->m_sBalanceType);//余额类型
			pInTwo->m_iBalanceType=(*iter)->m_iBalanceType;//余额类型
			pInTwo->m_iBonusStartTime=(*iter)->m_iBonusStartTime;//优惠有效期起始时间，yyyymmdd
			pInTwo->m_iBonusExpirationTime=(*iter)->m_iBonusExpirationTime;//优惠有效期终结时间，yyyymmdd
			strcpy(pInTwo->m_sDestinationAccount,pInOne->m_sDestinationAccount);
			iRet=UpdateAcctInfo(pInTwo,oServAcctInfoSeq);
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "更新用户账户失败  ");
				throw iRet;
			}
			strcpy(pInTwo->m_sSourceType,"5VF");
			strcpy(pInTwo->m_sSourceDesc,"VC优惠充值");
			strcpy(pInTwo->m_sAllowDraw,"ADN");
			bState=false;
			memset(&oSendSeq,0,sizeof(ServAcctInfoSeq));
			iRet=m_poSql->QueryServAcctSeq(oSendSeq,bState);
			if(iRet!=0)
			{
				__DEBUG_LOG1_(0, "查询用户序列失败=[%d]!",iRet);
				throw iRet;
			}
			sStructPaymentSeqInfo.m_lBalanceSourceSeq=oSendSeq.m_lBalanceSourceID;
			sStructPaymentSeqInfo.m_lAcctBalanceID=oServAcctInfoSeq.m_lAcctBalanceID;
			//记录来源表
			iRet=m_poSql->InsertBalanceSource(pInTwo,sStructPaymentSeqInfo);
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "记录余额来源失败!");
				throw iRet;
			}
			FreeObject(pInTwo);
			vAcctBalanceID.push_back(sStructPaymentSeqInfo.m_lAcctBalanceID);
		}
		return 0;

		
	}
	catch(TTStatus oSt) 
	{
		iRet=DEPOSIT_TT_ERR;
		__DEBUG_LOG1_(0, "充值失败，错误信息=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "充值失败，错误信息=%d",value);
	}
	FreeObject(pInTwo);
	

	return iRet;
} 
int DepositInfo::CreateServAcctInfo(DepositServCondOne *&pInOne,ServAcctInfoSeq &oServAcctInfoSeq)
{
	int iRet;
	bool bState=true;
	try
	{
		__DEBUG_LOG0_(0, "无用户资料,开始创建用户资料");
		//查询序列,暂时serv_id等都用序列代替 
		iRet=m_poSql->QueryServAcctSeq(oServAcctInfoSeq,bState);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "查询用户序列失败=[%d]",iRet);
			throw iRet;
		}
		//记录serv表
		iRet=m_poSql->InsertServInfo(oServAcctInfoSeq,pInOne);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "记录用户信息表失败 =[%d]",iRet);
			throw iRet;
		}
		//记录acct表
		iRet=m_poSql->InsertAcctInfo(oServAcctInfoSeq,pInOne);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "记录账户表失败 =[%d]",iRet);
			throw iRet;
		}
		//记录serv_acct表
		iRet=m_poSql->InsertServAcctInfo(oServAcctInfoSeq,pInOne);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "记录用户账户对应关系表失败  =[%d]",iRet);
			throw iRet;
		}
		//记录用户信息工单
		iRet=m_poSql->InsertUserInfoAdd(oServAcctInfoSeq,pInOne);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "记录用户信息工单  =[%d]",iRet);
			throw iRet;
		}
		//生成用户支付密码
		bState=true;
		iRet=m_poSql->InsertAttUserInfo(oServAcctInfoSeq,bState);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "生成用户支付密码  =[%d]",iRet);
			throw iRet;
		}
		//生成用户查询密码
		bState=false;
		iRet=m_poSql->InsertAttUserInfo(oServAcctInfoSeq,bState);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "生成用户支付密码  =[%d]",iRet);
			throw iRet;
		}
		//记录用户附加信息表（主要记录区号）
		iRet=m_poSql->InsertAppUserInfo(pInOne,oServAcctInfoSeq);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "生成用户区号  =[%d]",iRet);
			throw iRet;
		}

		//记录acct_balance表
		pInOne->m_iBonusStartTime=0;
		pInOne->m_iBonusExpirationTime=0;
		iRet=m_poSql->InsertAcctBalance(oServAcctInfoSeq,pInOne);
		if(iRet!=0)
		{
			__DEBUG_LOG1_(0, "记录余额账本表失败  =[%d]",iRet);
			throw iRet;
		}
		return 0;
	}
	catch(TTStatus oSt) 
	{
		iRet=DEPOSIT_TT_ERR;
		__DEBUG_LOG1_(0, "创建用户资料失败 ,错误信息为=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "创建用户资料失败 ,错误信息为=%d",value);
	}

	return iRet;
}
int DepositInfo::UpdateAcctInfo(DepositServCondOne *&pInOne,ServAcctInfoSeq &oServAcctInfoSeq)
{
	int iRet;
	bool bState=false;
	long lDepositAmount=0;
	StructPaymentSeqInfo sStructPaymentSeqInfo={0};
	AcctBalanceInfo *pAcctBalanceInfo=NULL;
	try
	{
		//根据账本类型查询账本
		pAcctBalanceInfo=new AcctBalanceInfo;
		if(!pAcctBalanceInfo)
		{
			__DEBUG_LOG0_(0, "分配内存失败");
			throw ECODE_MEM_MALLOC_FAIL;
		}
		/*
		if(strlen(pInOne->m_sBalanceType)<=0)
		{
			strcpy(pInOne->m_sBalanceType,"10");
		}
		*/
		iRet=m_poSql->QueryAcctBalanceSimple(pInOne,pAcctBalanceInfo);
		switch(iRet)
		{
			case 1://更新
				{
					__DEBUG_LOG0_(0, "存在用户余额账本,更新");
					oServAcctInfoSeq.m_lAcctBalanceID=pAcctBalanceInfo->m_lAcctBalanceID;
					lDepositAmount=pInOne->m_iVoucherValue;
					sStructPaymentSeqInfo.m_lPaymentAmount = (-1)*lDepositAmount;
					iRet=m_poSql->updateAcctBalance(sStructPaymentSeqInfo,pAcctBalanceInfo);
					if(iRet!=0)
					{
						__DEBUG_LOG1_(0, "更新用户余额账本失败  =[%d]",iRet);
						throw iRet;
					}
				}
				break;
			case 0://记录
				{
					__DEBUG_LOG0_(0, "创建用户余额账本，记录");
					iRet=m_poSql->QueryServAcctSeq(oServAcctInfoSeq,bState);
					if(iRet!=0)
					{
						__DEBUG_LOG1_(0, "查询余额账本序列,失败 =[%d]",iRet);
						throw iRet;
					}
					iRet=m_poSql->InsertAcctBalance(oServAcctInfoSeq,pInOne);
					if(iRet!=0)
					{
						__DEBUG_LOG1_(0, "记录余额账本表失败  =[%d]",iRet);
						throw iRet;
					}
				}
				break;
			default:
				{
					__DEBUG_LOG1_(0, "查询余额账本,失败[%d]",iRet);
					throw iRet;
				}
				break;
		}
		FreeObject(pAcctBalanceInfo);
		
		return 0;
	}
	catch(TTStatus oSt) 
	{
		iRet=DEPOSIT_TT_ERR;
		__DEBUG_LOG1_(0, "存在用户资料并记录信息失败,错误信息=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "存在用户资料并记录信息失败,错误信息=%d",value);
	}
	FreeObject(pAcctBalanceInfo);

	return iRet;
}

int DepositInfo::_dealIn(DepositServCondOne *&pInOne,vector<DepositServCondTwo *>&vInTwo)
{
	//充值，开户
	//校验资料是否存在（否，先开户）,充值
	int iRet=0;
	bool bState=false;
	ServAcctInfo oServAcct={0};
	ServAcctInfoSeq oServAcctInfoSeq={0};
	ServAcctInfoSeq oSendSeq={0};
	DepositServCondOne   *pInTwo=NULL;
	PaymentInfoALLCCR pPaymentInfoCCR={0};
	StructPaymentSeqInfo  sStructPaymentSeqInfo={0};
	
	ABMException oExp;
	try 
	{
	
		//普通充值
		//查询用户信息
		iRet=pAllocateBalanceSql->preQryServAcctInfo(pInOne->m_sDestinationAccount,2,oServAcct,oExp);
		switch(iRet)
		{
			case 1://无资料
				{
					__DEBUG_LOG0_(0, "没有资料，开始创建用户信息,然后充值  ");
					iRet=CreateServAcctInfo(pInOne,oServAcctInfoSeq);
					if(iRet!=0)
					{
						__DEBUG_LOG1_(0, "创建用户信息  失败==[%d]",iRet);
						throw iRet;
					}
				}
				break;
			case 0://有资料
				{
					__DEBUG_LOG0_(0, "已有用户资料，充值");
					oServAcctInfoSeq.m_lServID=oServAcct.m_servId;
					oServAcctInfoSeq.m_lAcctID=oServAcct.m_acctId;
					iRet=UpdateAcctInfo(pInOne,oServAcctInfoSeq);
					if(iRet!=0)
					{
						__DEBUG_LOG1_(0, "已有用户资料，充值  失败==[%d]",iRet);
						throw iRet;
					}
				}
				break;
			default://出错
				{
					__DEBUG_LOG1_(0, "查询用户资料出错=[%d]",iRet);
					throw iRet;
				}
		}
	
		//查询业务流水
		bState=false;
		iRet=m_poSql->QueryPaySeq(sStructPaymentSeqInfo,bState);
		if(iRet!=0)
        	{
			__DEBUG_LOG0_(0, "查询业务流水失败  error");
			throw iRet;
        	}
		//查询其他序列
		bState=true;
		iRet=m_poSql->QueryPaySeq(sStructPaymentSeqInfo,bState);
		if(iRet!=0)
        	{
			__DEBUG_LOG0_(0, "查询其他序列出错  error");
			throw iRet;
        	}
		//记录业务表
		strcpy(pPaymentInfoCCR.m_sOperateSeq,pInOne->m_sReqSerial);
		strcpy(pPaymentInfoCCR.m_sCapabilityCode,"0005");
		pPaymentInfoCCR.m_lServID=oServAcctInfoSeq.m_lServID;
		sStructPaymentSeqInfo.m_lRbkPaymentSeq=0;
		strcpy(pPaymentInfoCCR.m_sSpID,"1");
		strcpy(pPaymentInfoCCR.m_sServicePlatformID,"1");
		strcpy(pPaymentInfoCCR.m_sOrderID,"1");
		strcpy(pPaymentInfoCCR.m_sDescription,"划拨充值");
		strcpy(pPaymentInfoCCR.m_sMicropayType,"0");
		strcpy(pPaymentInfoCCR.m_sOrderState,"C0C");
		pPaymentInfoCCR.m_lPayAmount=pInOne->m_iVoucherValue;
		iRet=m_poSql->InsertPaymentInfo(pPaymentInfoCCR,sStructPaymentSeqInfo);
		if(iRet!=0)
        	{
			__DEBUG_LOG0_(0, "记录业务流水表出错!");
			throw iRet;
        	}
		
		sStructPaymentSeqInfo.m_lAcctBalanceID=oServAcctInfoSeq.m_lAcctBalanceID;
		strcpy(pInOne->m_sSourceType,"5VB");
		strcpy(pInOne->m_sSourceDesc,"划拨充值");
		strcpy(pInOne->m_sAllowDraw,"ADY");
		//记录来源表
		iRet=m_poSql->InsertBalanceSource(pInOne,sStructPaymentSeqInfo);
		if(iRet!=0)
        	{
			__DEBUG_LOG0_(0, "记录余额来源表失败! ");
			throw iRet;
        	}

		//优惠充值,只记录账本+来源表
		for(vector<DepositServCondTwo *>::iterator iter=vInTwo.begin();iter!=vInTwo.end();iter++)
		{

			__DEBUG_LOG0_(0, "充值存在优惠,开始优惠充值");
			__DEBUG_LOG1_(0, "优惠数据 (*iter)->m_iBonusUnit=[%d]",(*iter)->m_iBonusUnit);
			//__DEBUG_LOG1_(0, "优惠数据 (*iter)->m_sBalanceType=[%s]",(*iter)->m_sBalanceType);
			__DEBUG_LOG1_(0, "优惠数据 (*iter)->m_iBalanceType=[%d]",(*iter)->m_iBalanceType);
			__DEBUG_LOG1_(0, "优惠数据 (*iter)->m_iBonusStartTime=[%d]",(*iter)->m_iBonusStartTime);
			__DEBUG_LOG1_(0, "优惠数据 (*iter)->m_iBonusExpirationTime=[%d]",(*iter)->m_iBonusExpirationTime);
			__DEBUG_LOG1_(0, "优惠数据 (*iter)->m_iBonusAmount=[%d]",(*iter)->m_iBonusAmount);
			pInTwo=new DepositServCondOne;
			pInTwo->m_iDestinationBalanceUnit=(*iter)->m_iBonusUnit;//优惠额单位，0－分，1－条
			pInTwo->m_iVoucherValue=(*iter)->m_iBonusAmount;//赠与的优惠金额
			//strcpy(pInTwo->m_sBalanceType,(*iter)->m_sBalanceType);//余额类型
			pInTwo->m_iBalanceType=(*iter)->m_iBalanceType;//余额类型
			pInTwo->m_iBonusStartTime=(*iter)->m_iBonusStartTime;//优惠有效期起始时间，yyyymmdd
			pInTwo->m_iBonusExpirationTime=(*iter)->m_iBonusExpirationTime;//优惠有效期终结时间，yyyymmdd
			strcpy(pInTwo->m_sDestinationAccount,pInOne->m_sDestinationAccount);
			iRet=UpdateAcctInfo(pInTwo,oServAcctInfoSeq);
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "更新用户账户失败  ");
				throw iRet;
			}
			strcpy(pInTwo->m_sSourceType,"5VF");
			strcpy(pInTwo->m_sSourceDesc,"VC优惠充值");
			strcpy(pInTwo->m_sAllowDraw,"ADN");
			bState=false;
			memset(&oSendSeq,0,sizeof(ServAcctInfoSeq));
			iRet=m_poSql->QueryServAcctSeq(oSendSeq,bState);
			if(iRet!=0)
			{
				__DEBUG_LOG1_(0, "查询用户序列失败=[%d]!",iRet);
				throw iRet;
			}
			sStructPaymentSeqInfo.m_lBalanceSourceSeq=oSendSeq.m_lBalanceSourceID;
			sStructPaymentSeqInfo.m_lAcctBalanceID=oServAcctInfoSeq.m_lAcctBalanceID;
			//记录来源表
			iRet=m_poSql->InsertBalanceSource(pInTwo,sStructPaymentSeqInfo);
			if(iRet!=0)
			{
				__DEBUG_LOG0_(0, "记录余额来源失败!");
				throw iRet;
			}
			FreeObject(pInTwo);
		}
		return 0;

		
	}
	catch(TTStatus oSt) 
	{
		iRet=-1;
		__DEBUG_LOG1_(0, "充值失败，错误信息=%s", oSt.err_msg);
	}
	catch(int &value ) 
	{
		iRet=value;
		__DEBUG_LOG1_(0, "充值失败，错误信息=%d",value);
	}
	FreeObject(pInTwo);
	

	return iRet;
} 



