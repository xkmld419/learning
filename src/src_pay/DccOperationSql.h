// DccOperationSql.h
#ifndef __DCC_OPERATION_SQL_H_INCLUDED_
#define __DCC_OPERATION_SQL_H_INCLUDED_

#include "TTTns.h"
#include "abmpaycmd.h"
#include "dcc_ra.h"
#include "LogV2.h"
#include "PublicConndb.h"
#include "abmobject.h"

class ABMException;
class PlatformDeductBalanceCCR;
class ReqPlatformSvc;
class ReqPlatformPayInfo;

class LoginPasswdCCR;
class PasswdChangeCCR;

class DccOperationSql : public TTTns
{
	public:
		DccOperationSql();
		~DccOperationSql();		
		
		int QueryPaySeq(long &lSessionIdSeq);
		
		int savePlatformCCR(PlatformDeductBalanceCCR *receiveCCR);	
		int savePlatformCCRBiz(char *sSessionId,ReqPlatformSvc *pSvcIn,ReqPlatformPayInfo *pSvcInBiz);
		int savePlatformCCRforIdentify(char *sSessionId,LoginPasswdStruct *pStruct,LoginPasswdCond *pCond); 	//登陆密码鉴权新加函数
		int savePlatformCCRforChange(char *sSessionId,PasswdChangeStruct *pStruct,PasswdChangeCond *pCond);	//登陆密码修改新加函数
		
		int getCCRInfo(char *sSessionId,ReqPlatformSvc &reqSvc,ReqPlatformPayInfo &reqPayInfo);
		int getLoginIdentifyCCRInfo(char *sSessionId,LoginPasswdStruct &pStruct,LoginPasswdCond &pCond);	//登陆密码鉴权新加函数
		int getPasswdChangeInfo(char *sSessionId,PasswdChangeStruct &pStruct,PasswdChangeCond &pCond); //登陆密码修改新加函数
		
		int qryAccountOrgId(char *accountNbr,char *orgId,char *localAreaId);
		
		int QueryDccRecordSeq(long &lRecordIdSeq);

		int insertDccInfo(struDccHead &dccHead,char *dccType,long &lRecordIdSeq);
		
		int insertDccInfo(struDccHead &dccHead,char *dccType,long &lRecordIdSeq,char *sPreSessionId);	
		
		int qryPreSessionId(char *sSessionId,char *sPreSessionId);
		
		int updateDccInfo(char *sCCASessionId);
		
		int insertActiveCCR(char *sSessionId,char *sRequestId,char *sActionType,char *sOrgId);
		
		int updateActiveCCR(char *sCCARequestId);
};

#endif /* __DCC_OPERATION_SQL_H_INCLUDED_ */

