//ActiveSendReverseBiz.cpp
#include "ActiveSendReverseBiz.h"
#include "ActiveSendReverseBizSql.h"
#include "ABMException.h"
#include "abmcmd.h"
#include "LogV2.h"
#include "PublicConndb.h"
#include "DccOperation.h"

static ABMException oExp;
class ActiveSendReverseBizSql;

bool g_flag = true;

ActiveSendReverseBiz::ActiveSendReverseBiz()
{
	m_pPublicConndb=NULL;
	m_pActiveReverseSql = NULL;
	m_dccOperation = NULL;
}

ActiveSendReverseBiz:: ~ActiveSendReverseBiz()
{
	FreeObject(m_pPublicConndb);
	FreeObject(m_pActiveReverseSql);
	FreeObject(m_dccOperation);
}

int ActiveSendReverseBiz::init(ABMException &oExp)
{
	if (m_pPublicConndb->init(oExp) != 0) 
	{
		ADD_EXCEPT0(oExp, "ActiveSendReverseBiz::init m_pPublicConndb->init failed!");
		return -1;
	}
	
	if ((m_pActiveReverseSql = new ActiveSendReverseBizSql) == NULL)
	{
		ADD_EXCEPT0(oExp, "ActiveSendReverseBiz::init malloc failed!");
		return -1;
	}
	
	
	if ((m_dccOperation = new DccOperation) == NULL)
	{
		ADD_EXCEPT0(oExp, "ActiveSendReverseBiz::init malloc failed!");
		return -1;
	}
	
	return 0;
}

int ActiveSendReverseBiz::deal(ABMCCR *pSendCCR)
{
	ABMException oExp;
	int iRet=0;
	vector<ActiveSendLog> vActiveSendLog;
	
	char sSwitchId[5]={0};
	char sSwitchValue[2]={0};
	long lDelaySecond=0L;
	strcpy(sSwitchId,"1001");
	// 取系统开关,SYS_SWITCH:1001,如果为1,查询符合条件的记录
	iRet = m_pActiveReverseSql->getSystemSwitch(sSwitchId,sSwitchValue);
	
	//__DEBUG_LOG1_(0, "系统开关1001,valuse:[%s]",sSwitchValue);	
	
	if(strcmp(sSwitchValue,"1") == 0)
	{
		// 取冲正时间延迟发送秒数
		iRet = m_pActiveReverseSql->getDelaySecond(lDelaySecond);
		
		//__DEBUG_LOG1_(0, "配置的延迟发送的秒数:[%ld]",lDelaySecond);	
		
		// 扫描主动发出的CCR日志记录表
		iRet = m_pActiveReverseSql->qryActiveSendLog(vActiveSendLog,lDelaySecond,oExp);
	}
	else
	{
		// 1001系统开关值为0，取所有记录,扫描主动发出的CCR日志记录表
		iRet = m_pActiveReverseSql->qryActiveSendLog(vActiveSendLog,oExp);
	}
	//__DEBUG_LOG1_(0, "扫描主动发送CCR信息日志,记录数:[%d]",vActiveSendLog.size());	
	//__DEBUG_LOG0_(0, "扫描主动发送CCR信息日志");
	
	if(vActiveSendLog.size() > 0)
	{
		ReverseDeductBalanceCCR *pSendPack = new ReverseDeductBalanceCCR;
		pSendPack = (ReverseDeductBalanceCCR *)pSendCCR;
		for(int i=0;i<vActiveSendLog.size();i++)
		{	 
			pSendPack->clear();
			
			ReqReverseDeductSvc reverseDeductSvc;
			// 赋值-组装CCR包-业务信息
			// 生成请求流水
			strcpy(reverseDeductSvc.m_sRequestId,"20110716"); // 请求流水
			
			// 当前时间,转成秒数
			time_t tTime; 
			tTime = time(NULL);
			char sCurDate[32]={0};
			long2date(tTime+2209017599,sCurDate);
			
			snprintf(reverseDeductSvc.m_sRequestId,sizeof(reverseDeductSvc.m_sRequestId),"002%s%08d00\0",sCurDate,1);
			reverseDeductSvc.m_iRequestTime = tTime;// 请求时间
			
			strcpy(reverseDeductSvc.m_sOldRequestId,vActiveSendLog[i].sRequestId);// 被冲正的划拨流水号
			strcpy(reverseDeductSvc.m_sABMId,"001"); // 发起冲正ABM标识,集团ABM
			if(strcmp(vActiveSendLog[i].sRequestType,"001") == 0)
			{
				reverseDeductSvc.m_iActionType = 0;	// 冲正操作类型,0:划入冲正,1:划出冲正
			}
			else if(strcmp(vActiveSendLog[i].sRequestType,"002") == 0)
			{
				reverseDeductSvc.m_iActionType = 1;
			}	
			
			// 查询出sOrgId
			char sOrgId[5]={0};
			strcpy(sOrgId,vActiveSendLog[i].sOrgId);
			
			__DEBUG_LOG1_(0, "划拨冲正CCR,请求流水,m_sRequestId:[%s]",reverseDeductSvc.m_sRequestId);
			__DEBUG_LOG1_(0, "划拨冲正CCR,请求时间,m_iRequestTime:[%d]",reverseDeductSvc.m_iRequestTime);
			__DEBUG_LOG1_(0, "划拨冲正CCR,被冲正流水号,m_sOldRequestId:[%s]",reverseDeductSvc.m_sOldRequestId);
			__DEBUG_LOG1_(0, "划拨冲正CCR,发起冲正ABM标识,m_sABMId:[%s]",reverseDeductSvc.m_sABMId);
			__DEBUG_LOG1_(0, "划拨冲正CCR,冲正类型,m_iActionType:[%d]",reverseDeductSvc.m_iActionType);
			__DEBUG_LOG1_(0, "划拨冲正CCR,所属机构,sOrgId:[%s]",sOrgId);
			__DEBUG_LOG0_(0, "=====================================");
			
			// CCR
			// 组CCR包消息头信息
			// 设置CCR消息头
			// DeductFromABM. Micropayment@<DomainName>.ChinaTelecom.com
			// 生成Session-id
			long lSessionIdSeq;
			iRet = m_dccOperation->QueryPaySeq(lSessionIdSeq);
			if(iRet !=0)
			{
				__DEBUG_LOG0_(0, "查询DCC会话标识Session-Id失败");
				throw QRY_SESSION_ID_ERR;
			}
			struct struDccHead Head;
			char sSessionId[64]={0};
			
			//time_t tTime;
			tTime = time(NULL);
			// 2011.8.1
			snprintf(Head.m_sSessionId,sizeof(Head.m_sSessionId),"%s%010d%s%04d\0","ABM@001.ChinaTelecom.com;",tTime,";",lSessionIdSeq);
			strcpy(Head.m_sOutPlatform,""); // 外部平台，SGW使用
			strcpy(Head.m_sOrignHost,"ABM@001.ChinaTelecom.com"); // 2011.8.1
			strcpy(Head.m_sOrignRealm,"001.ChinaTelecom.com");
			
			snprintf(Head.m_sDestRealm,sizeof(Head.m_sDestRealm),"%s.ChinaTelecom.com",sOrgId);
	
			snprintf(Head.m_sSrvTextId,sizeof(Head.m_sSrvTextId),"ReverseChargeFromABM.Micropayment@%s.ChinaTelecom.com",sOrgId);
			__DEBUG_LOG1_(0, "消息头，目标设备标识-Head.m_sDestRealm:[%s]",Head.m_sDestRealm);
			
			Head.m_iAuthAppId = 4;
			Head.m_iReqType = 4;
			Head.m_iReqNumber  = 0;
			Head.m_iResultCode = 0;
			strcpy(Head.m_sSrvFlowId,""); // 按规则，取源m_sSrvFlowId后追加
			//snprintf(Head.m_sSrvFlowId,sizeof(Head.m_sSrvFlowId),"%s%s","-","-");
			memcpy((void *)(pSendPack->m_sDccBuf),(void *)&Head,sizeof(Head));
			
			//pSendPack->addReqReverseDeductSvc(reverseDeductSvc);
			
			// 打包发送
			if (!pSendPack->addReqReverseDeductSvc(reverseDeductSvc) )
			{
				__DEBUG_LOG0_(0, "ActiveSendReverseBiz::deal 划拨冲正第一层CCR打包失败!");
				//pSendPack->setRltCode(ECODE_PACK_FAIL);
				//throw ACTIVE_PACK_CCR_OUT_ERR;
			}
			
			__DEBUG_LOG0_(0, "[主动划拨冲正]:第一层服务CCR打包完成");
			
			pSendPack->m_iCmdID = ABMCMD_REVERSE_DEDUCT_BALANCE;
			// 更新CCR记录状态
			
			iRet = m_pActiveReverseSql->updateCCRInfo(reverseDeductSvc.m_sOldRequestId);
			if(iRet != 0)
			{
				__DEBUG_LOG0_(0, "[主动划拨冲正]:更新CCR记录失败");
			}
			
			break;
		}
		
		delete pSendPack;
		pSendPack = NULL;
	}

	// 提交
	if(iRet==0)
	{
		//__DEBUG_LOG0_(0, "ActiveSendReverseBiz->deal  ---0");
		m_pPublicConndb->Commit(oExp);
	}
	else
	{
		//__DEBUG_LOG0_(0, "ActiveSendReverseBiz->deal  ---1");
		//m_pPublicConndb->RollBack(oExp);
	}
	
	return iRet;
}













