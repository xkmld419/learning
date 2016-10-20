//DccOperation.cpp
#include "DccOperation.h"
#include "DccOperationSql.h"
ABMException oExp3;
DccOperation::DccOperation()
{
	pDccSql = new DccOperationSql;
}

DccOperation:: ~DccOperation()
{
	FreeObject(pDccSql);
}

// 查询SessionId Seq. 函数名待修改一下
int DccOperation::QueryPaySeq(long &lSessionIdSeq)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作");
	try
	{
		iRet = pDccSql->QueryPaySeq(lSessionIdSeq);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::QueryPaySeq oSt.err_msg=%s", oSt.err_msg);
	}

	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作结束");
	return iRet;
}

// 保存业务平台发出的CCR请求信息
int DccOperation::savePlatformCCR(PlatformDeductBalanceCCR *receiveCCR)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作");
	try
	{
		iRet = pDccSql->savePlatformCCR(receiveCCR);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::savePlatformCCR oSt.err_msg=%s", oSt.err_msg);
	}
	
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作结束");
	return iRet;
}	

// 保存业务平台发出CCR请求的业务信息
int DccOperation::savePlatformCCRBiz(char *sSessionId,ReqPlatformSvc *pSvcIn,ReqPlatformPayInfo *pSvcInBiz)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行CCR业务数据保存操作");
	
	try
	{
		iRet = pDccSql->savePlatformCCRBiz(sSessionId,pSvcIn,pSvcInBiz);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::savePlatformCCRBiz oSt.err_msg=%s", oSt.err_msg);
	}
	
	__DEBUG_LOG0_(0, "调用SQL执行CCR业务数据保存操作结束");
	return iRet;
}

// 获取自服务平台的CCR业务请求信息
int DccOperation::getCCRInfo(char *sSessionId,ReqPlatformSvc &reqSvc,ReqPlatformPayInfo &reqPayInfo)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作");
	
	try
	{
		iRet = pDccSql->getCCRInfo(sSessionId,reqSvc,reqPayInfo);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::savePlatformCCRBiz oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG1_(0, "===>DccOperation::getCCRInfo,iRet=[%d]",iRet);
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作结束");
	return iRet;
}

//  获取用户号码所属机构代码
int DccOperation::qryAccountOrgId(char *accountNbr,char *orgId,char *localAreaId)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作");
	
	try
	{
		iRet = pDccSql->qryAccountOrgId(accountNbr,orgId,localAreaId);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::savePlatformCCRBiz oSt.err_msg=%s", oSt.err_msg);
	}
	
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作结束");
	return iRet;
}

int DccOperation::QueryDccRecordSeq(long &lRecordIdSeq)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "查询DCC_INFO_RECORD_SEQ操作");
	try
	{
		iRet = pDccSql->QueryDccRecordSeq(lRecordIdSeq);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::QueryDccRecordSeq oSt.err_msg=%s", oSt.err_msg);
	}
	
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作结束");
	return iRet;
}

// 保存收发的DCC消息头信息
int DccOperation::insertDccInfo(struDccHead &dccHead,char *dccType,long &lRecordIdSeq)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作");
	try
	{
		iRet = pDccSql->insertDccInfo(dccHead,dccType,lRecordIdSeq);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::savePlatformCCRBiz oSt.err_msg=%s", oSt.err_msg);
	}
	
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作结束");
	return iRet;
}

// 保存收发的DCC消息头信息-主动发包时，保存源请求回话的SessionId
int DccOperation::insertDccInfo(struDccHead &dccHead,char *dccType,long &lRecordIdSeq,char *sPreSessionId)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作");
	try
	{
		iRet = pDccSql->insertDccInfo(dccHead,dccType,lRecordIdSeq,sPreSessionId);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::savePlatformCCRBiz oSt.err_msg=%s", oSt.err_msg);
	}
	
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作结束");
	return iRet;
}

// 根据响应SESSION_ID 查找源SESSION_ID
int DccOperation::qryPreSessionId(char *sSessionId,char *sPreSessionId)
{
	int iRet=1;
	// 调用SQL执行
	try
	{
		__DEBUG_LOG0_(0, "查询PRE_SESSION_ID操作");
		iRet = pDccSql->qryPreSessionId(sSessionId,sPreSessionId);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::qryPreSessionId: oSt.err_msg=%s", oSt.err_msg);
	}
	
	__DEBUG_LOG1_(0, "查询PRE_SESSION_ID操作结束,sPreSessionId=[%s]",sPreSessionId);
	__DEBUG_LOG1_(0, "查询PRE_SESSION_ID操作结束,iRet=[%d]",iRet);
	return iRet;
}

//保存自服务平台发出CCR请求的业务信息-登陆密码鉴权用
int DccOperation::savePlatformCCRforIdentify(char *sSessionId,LoginPasswdStruct *pStruct,LoginPasswdCond *pCond)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行CCR业务数据保存操作");
	
	try
	{
		iRet = pDccSql->savePlatformCCRforIdentify(sSessionId,pStruct,pCond);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::savePlatformCCRforIdentify oSt.err_msg=%s", oSt.err_msg);
	}
	
	__DEBUG_LOG0_(0, "调用SQL执行CCR业务数据保存操作结束");
	return iRet;
}

//获取自服务平台的CCR业务请求信息-密码鉴权新增
int DccOperation::getLoginIdentifyCCRInfo(char *sSessionId,LoginPasswdStruct &pStruct,LoginPasswdCond &pCond)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作");
	
	try
	{
		iRet = pDccSql->getLoginIdentifyCCRInfo(sSessionId,pStruct,pCond);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::savePlatformCCRforIdentify oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG1_(0, "===>DccOperation::getLoginIdentifyCCRInfo,iRet=[%d]",iRet);
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作结束");
	return iRet;
}

//保存自服务平台发出CCR请求的业务信息-登陆密码修改用
int DccOperation::savePlatformCCRforChange(char *sSessionId,PasswdChangeStruct *pStruct,PasswdChangeCond *pCond)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行CCR业务数据保存操作");
	
	try
	{
		iRet = pDccSql->savePlatformCCRforChange(sSessionId,pStruct,pCond);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::savePlatformCCRforChange oSt.err_msg=%s", oSt.err_msg);
	}
	
	__DEBUG_LOG0_(0, "调用SQL执行CCR业务数据保存操作结束");
	return iRet;
}

//获取自服务平台的CCR业务请求信息-密码修改新增
int DccOperation::getPasswdChangeInfo(char *sSessionId,PasswdChangeStruct &pStruct,PasswdChangeCond &pCond)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作");
	
	try
	{
		iRet = pDccSql->getPasswdChangeInfo(sSessionId,pStruct,pCond);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::getPasswdChangeInfo oSt.err_msg=%s", oSt.err_msg);
	}
	__DEBUG_LOG1_(0, "===>DccOperation::getPasswdChangeInfo,iRet=[%d]",iRet);
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作结束");
	return iRet;
}

int DccOperation::updateDccInfo(char *sCCASessionId)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作");
	try
	{
		iRet = pDccSql->updateDccInfo(sCCASessionId);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::updateDccInfo oSt.err_msg=%s", oSt.err_msg);
	}
	
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作结束");
	return iRet;
}

// 保存主动发送的CCR-给冲正用
int DccOperation::insertActiveCCR(char *sSessionId,char *sRequestId,char *sActionType,char *sOrgId)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作-插入ACTIVE_CCR_LOG");
	try
	{
		iRet = pDccSql->insertActiveCCR(sSessionId,sRequestId,sActionType,sOrgId);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::insertActiveCCR oSt.err_msg=%s", oSt.err_msg);
	}
	
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作结束-插入ACTIVE_CCR_LOG");
	return iRet;
}

int DccOperation::updateActiveCCR(char *sCCARequestId)
{
	int iRet=1;
	// 调用SQL执行
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作-更新ACTIVE_CCR_LOG");
	try
	{
		iRet = pDccSql->updateActiveCCR(sCCARequestId);
	}
	catch (TTStatus oSt) 
	{
		ADD_EXCEPT1(oExp3, "DccOperation::updateActiveCCR oSt.err_msg=%s", oSt.err_msg);
	}
	
	__DEBUG_LOG0_(0, "调用SQL执行数据保存操作结束");
	return iRet;
}