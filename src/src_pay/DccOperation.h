#ifndef __DCC_OPERATION_H_INCLUDED_
#define __DCC_OPERATION_H_INCLUDED_

#include "abmcmd.h"
#include "dcc_ra.h"

class ABMException;
class DccOperationSql;


// 对收到和发送的DCC进行入库操作
class DccOperation
{
	public:
		DccOperation();
		~DccOperation();
		
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
		
		int insertDccInfo(struDccHead &dccHead,char *dccType,long &lRecordIdSeq,char *preSessionId);
		
		int qryPreSessionId(char *sSessionId,char *sPreSessionId);
		
		int updateDccInfo(char *sCCASessionId);
		
		int insertActiveCCR(char *sSessionId,char *sRequestId,char *sActionType,char *sOrgId);
		
		int updateActiveCCR(char *sCCARequestId);
		
	private:	
		DccOperationSql *pDccSql;
	
};

#endif /* __DCC_OPERATION_H_INCLUDED_ */ 