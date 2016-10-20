#include "QueryAll.h"
#include "QueryAllSql.h"
#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"
#include "Date.h"
#include "abmobject.h"
#include <string.h>

static vector<QueryAllPay *> g_vAll;

extern bool g_toPlatformFlag;

QueryAll::QueryAll()
{
    m_poSql = NULL;
    
    m_poQueryAllCCR = NULL;
    
    m_poQueryAllCCA = NULL;
    
    m_poQueryAllSndCCR = NULL;

    m_iCount = 0;
    
    m_dccOperation = NULL;
    
}

QueryAll::~QueryAll()
{
    g_vAll.clear();
    #define __FREE_PTR_(p) if(p != NULL)\
        {\
            delete p;\
            p = NULL;\
        }
    __FREE_PTR_(m_poSql);
    __FREE_PTR_(m_dccOperation);

}

int QueryAll::init(ABMException &oExp)
{
    m_poSql = new QueryAllSql;

    if(m_poSql == NULL)
    {
        ADD_EXCEPT0(oExp, "申请内存失败"); 
        return -1;
    }

    return m_poSql->init(oExp);
    
    m_dccOperation = new DccOperation;
    
}
int QueryAll::deal(ABMCCR *poQueryMsg, ABMCCA *poQueryRlt)
{
    int iRet = 0;
    __DEBUG_LOG0_(0, "余额划拨查询处理开始");
    poQueryRlt->memcpyh(poQueryMsg);
    memset((void *)&m_oRlt,0x00,sizeof(m_oRlt));
    memset((void *)&m_oCCRHead,0x00,sizeof(m_oCCRHead));
    
	char sDestRealm[32]={0};
    //获取DCC消息头
	memcpy((void *)&m_oCCRHead,(void *)(poQueryMsg->m_sDccBuf),sizeof(m_oCCRHead));
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_iAuthAppId=%d",m_oCCRHead.m_iAuthAppId);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_iReqNumber=%d",m_oCCRHead.m_iReqNumber);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_iReqType=%d",m_oCCRHead.m_iReqType);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_iResultCode=%d",m_oCCRHead.m_iResultCode);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sDestRealm=%s",m_oCCRHead.m_sDestRealm);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sOrignHost=%s",m_oCCRHead.m_sOrignHost);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sOutPlatform=%s",m_oCCRHead.m_sOutPlatform);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sSessionId=%s",m_oCCRHead.m_sSessionId);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sSrvFlowId=%s",m_oCCRHead.m_sSrvFlowId);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sSrvTextId=%s",m_oCCRHead.m_sSrvTextId);
    
    
	// 消息头中获取目的地址
	//如果目的地址为空则认为是省里返回的CCA，如果不为空则认为是自服务平台发来的请求。
	strcpy(sDestRealm,m_oCCRHead.m_sDestRealm);
    if (strlen(sDestRealm) != 0)
    {
    	__DEBUG_LOG0_(0, "处理自服务平台请求的CCR,并发CCR给省里[dQueryAllCCR]");
        iRet = dQueryAllCCR(poQueryMsg, poQueryRlt);
    }
    else
    {
    	__DEBUG_LOG0_(0, "处理省返回的CCA,并返CCA给自服务平台.[dQueryAllCCA]");
        iRet = dQueryAllCCA(poQueryMsg, poQueryRlt);
    }
    if (iRet != 0)
    {
        HssResMicroPayCCA * tmp = NULL;
        tmp = (HssResMicroPayCCA *)poQueryRlt;
        m_oRlt.m_uiResTime = time(NULL);
        m_oRlt.m_uiRltCode = iRet;
        if (!tmp->addRlt(m_oRlt))
        {
            __DEBUG_LOG0_(0, "deal::deal 打包错误消息失败");
            tmp->clear();
        }
    }

    __DEBUG_LOG0_(0, "业务处理成功");
    return 0;
}

// 余额划拨查询-处理请求CCR
int QueryAll::dQueryAllCCR(ABMCCR * poQueryMsg, ABMCCA * poQueryRlt)
{
    __DEBUG_LOG0_(0, "处理自服务平台的CCR请求.");
    int iRet;
    poQueryRlt->memcpyh(poQueryMsg);
    
    m_poQueryAllCCR = (HssPaymentQueryCCR *)poQueryMsg;
    m_poQueryAllCCA = (HssResMicroPayCCA *)poQueryRlt;
    m_poQueryAllSndCCR = (HssQueryBalanceCCR *)poQueryRlt;
    m_poQueryAllSndCCR->clear();
    m_poQueryAllCCA->clear();
    g_vAll.clear();

    vector<QueryAllPay *>::iterator itvAll;

    //消息解包
    if ((iRet=m_poQueryAllCCR->getReq(g_vAll)) == 0) {
        __DEBUG_LOG0_(0, "getReq::deal 消息包为NULL");
        m_poQueryAllCCA->setRltCode(ECODE_NOMSG);
        return QRY_ECODE_NOMSG_ERR;
    }
    else if (iRet < 0) {
        __DEBUG_LOG0_(0, "getReqBal::deal 消息解包出错!");
        m_poQueryAllCCA->setRltCode(ECODE_UNPACK_FAIL);
        return QRY_ECODE_UNPACK_FAIL;
    }
    if (iRet != 1){
        __DEBUG_LOG0_(0, "getReqBal::deal 消息解包出错，请求信息有多条!");
        m_poQueryAllCCA->setRltCode(ECODE_UNPACK_FAIL);
        return QRY_ECODE_UNPACK_FAIL;
    }
    
    try {
        for (itvAll=g_vAll.begin(); itvAll!=g_vAll.end(); ++itvAll) 
        {
            __DEBUG_LOG0_(0, "调用划拨查询业务处理.");
            iRet = _dQueryAll(*itvAll);
            return iRet;
        }
    }
    catch(TTStatus oSt) {
        __DEBUG_LOG1_(0, "getReqBal::deal oSt.err_msg=%s", oSt.err_msg);
    }
    
    m_poQueryAllCCA->clear();
    m_poQueryAllCCA->setRltCode(ECODE_TT_FAIL);
    
    return 0;

}

int QueryAll::_dQueryAll(QueryAllPay *pIn)
{
    int iRet = -1;
    __DEBUG_LOG0_(0, "_dQueryAll.开始");
    MicroPaySrv tmpSrv;
    snprintf(m_oRlt.m_sResID, sizeof(m_sReqID),"%s",pIn->m_sReqID);
    vector<MicroPaySrv> vData;
    vData.clear();
    char sORG_ID[8];
    char strTmp[20];
    double dtmp;
    int iServPlatForm = 0;
    
    /*
    if (pIn->m_iDestType == 2)
    {
    	__DEBUG_LOG0_(0, "标识类型：1.通行证账号（默认），2.业务平台账号");
        return QRY_USER_TYPE_ERR;
    }
    */
    __DEBUG_LOG1_(0, "打印接受到的请求消息m_iDestAttr=%d",pIn->m_iDestAttr);
    __DEBUG_LOG1_(0, "打印接受到的请求消息m_iDestType=%d",pIn->m_iDestType);
    __DEBUG_LOG1_(0, "打印接受到的请求消息m_iQueryFlg=%d",pIn->m_iQueryFlg);
    __DEBUG_LOG1_(0, "打印接受到的请求消息m_iQueryItemType=%d",pIn->m_iQueryItemType);
    __DEBUG_LOG1_(0, "打印接受到的请求消息m_sDestAcct=%s",pIn->m_sDestAcct);
    __DEBUG_LOG1_(0, "打印接受到的请求消息m_sReqID=%s",pIn->m_sReqID);// 自服务平台请求ID
    __DEBUG_LOG1_(0, "打印接受到的请求消息m_sServPlatID=%s",pIn->m_sServPlatID);
    __DEBUG_LOG1_(0, "打印接受到的请求消息m_uiReqTime=%d",pIn->m_uiReqTime);
    
    // add 2011.7.19
    if(pIn->m_iDestType == 2)
    {
    	// 互联网用户帐号,仅查询支付帐户余额,返回自服务平台	
    	iRet = _dQueryCenterABM(pIn);
    	return iRet;
    }

    TimesTenCMD *pTmp = m_poSql->m_poQueryAll;
    pTmp->setParam(1, pIn->m_sDestAcct);

    //删除匹配用户类型和用户属性
    //snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_iDestType);
    //pTmp->setParam(2, strTmp);
    //snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_iDestAttr);
    //pTmp->setParam(3, strTmp);
    //pTmp->setParam(4, pIn->m_iQueryFlg);
    //去掉平台ID
    //sscanf(pIn->m_sServPlatID,"%d",&iServPlatForm);
    //pTmp->setParam(2, iServPlatForm);
    pTmp->Execute();
    while (!pTmp->FetchNext()) {
        memset((void *)&tmpSrv, 0x00, sizeof(tmpSrv));
        tmpSrv.m_iBalSrc = 2;
        pTmp->getColumn(1, &tmpSrv.m_iUnitTypID);
        pTmp->getColumn(2, &tmpSrv.m_lBalAmount);
        pTmp->getColumn(3, sORG_ID);
        
        vData.push_back(tmpSrv);
    }
    pTmp->Close();
    
    if (vData.size() == 0 )
    {
        __DEBUG_LOG1_(0, "_dQueryAll:: _deal  查询无记录!err_code = %d",QRY_BALANCE_NODATA_ERR);
        return QRY_BALANCE_NODATA_ERR;
    }
    if (vData.size() != 1)
    {
        __DEBUG_LOG1_(0, "_dQueryAll:: _deal  查询多条记录!err_code = %d",QRY_MUL_RECORD_ERR);
        return QRY_MUL_RECORD_ERR;
    }
    
    long lseq;
    if (m_poSql->GetSeq(lseq) != 0)
    {
        __DEBUG_LOG1_(0, "_dQueryAll:: _deal get seq failed !err_code=%d",QRY_GET_SEQUENCE_FAIL_ERR);
        return QRY_GET_SEQUENCE_FAIL_ERR;
    }
    
    //保存自服务平台的请求消息头DCC头
    /*
    TimesTenCMD *pTmpInsDcc = m_poSql->m_poInsertDcc;
    try{
        pTmpInsDcc->setParam(1, m_oCCRHead.m_sSessionId);
    	pTmpInsDcc->setParam(2, m_oCCRHead.m_sOrignHost);
    	pTmpInsDcc->setParam(3, m_oCCRHead.m_sOrignRealm);
    	pTmpInsDcc->setParam(4, m_oCCRHead.m_sDestRealm);
    	pTmpInsDcc->setParam(5, m_oCCRHead.m_sSrvTextId);
    	pTmpInsDcc->setParam(6, m_oCCRHead.m_sSrvFlowId);
    	pTmpInsDcc->setParam(7, lseq);
    	pTmpInsDcc->Execute();
    	pTmpInsDcc->Commit();
    }
    catch (TTStatus oSt)
    {
        __DEBUG_LOG1_(0, "_dQueryAll:: _deal  oSt.err_msg=%s!",oSt.err_msg);
        __DEBUG_LOG1_(0, "_dQueryAll:: _deal  保存DCC消息头失败,err_code=%d!",QRY_INSERT_DCC_FAIL_ERR);
        return QRY_INSERT_DCC_FAIL_ERR;
    }
    __DEBUG_LOG0_(0, "_dQueryAll:: _deal  保存DCC消息头完成");
   */	
	
    // 保存CCR消息头，入TT-add 2011.7.9
    	char dccType[1];	// DCC消息类型
	strcpy(dccType,"R"); 
	iRet = m_dccOperation->insertDccInfo(m_oCCRHead,dccType,lseq);
	if(iRet != 0)
	{
		__DEBUG_LOG0_(0, "[划拨查询-接收自服务平台CCR]:保存自服务平台CCR包消息头信息失败");
		throw SAVE_PLATFOR_CCR_ERR;
	}	
	
	
    //保存全国ABM查询的数据
    TimesTenCMD *pTmpInsMsg = m_poSql->m_poInsertMsg;
    try{
        
    	pTmpInsMsg->setParam(1, m_oCCRHead.m_sSessionId);
    	pTmpInsMsg->setParam(2, tmpSrv.m_iUnitTypID);
    	pTmpInsMsg->setParam(3, tmpSrv.m_lBalAmount);
    	pTmpInsMsg->setParam(4, pIn->m_sReqID); // 保存自服务平台的请求流水号
    	pTmpInsMsg->Execute();
    	pTmpInsMsg->Commit();
    }
    catch (TTStatus oSt)
    {
        __DEBUG_LOG1_(0, "_dQueryAll:: _deal  oSt.err_msg=%s!",oSt.err_msg);
        __DEBUG_LOG1_(0, "_dQueryAll:: _deal  保存全国中心ABM余额失败,err_code=%d!",QRY_SAVEABMDATA_FAIL_ERR);
        return QRY_SAVEABMDATA_FAIL_ERR;
    }
    __DEBUG_LOG0_(0, "_dQueryAll:: _deal  保存全国中心业务数据完成");
    //设置消息头
    struct struDccHead Head;
    memset((void *)&Head,0x00,sizeof(Head));
    Date sessionid;
    // modify 2011.7.9
    // snprintf(Head.m_sSessionId,sizeof(Head.m_sSessionId),"%s",m_oCCRHead.m_sSessionId);
    // 组发送的CCR session-id
    
    // 生成Session-id
	long lSessionIdSeq;
	iRet = m_dccOperation->QueryPaySeq(lSessionIdSeq);
	if(iRet !=0)
	{
		__DEBUG_LOG0_(0, "查询DCC会话标识Session-Id失败");
		throw QRY_SESSION_ID_ERR;
	}
    
    	time_t tTime;
	tTime = time(NULL);
	// 2011.8.1
	snprintf(Head.m_sSessionId,sizeof(Head.m_sSessionId),"%s%010d%s%04d\0","ABM@001.ChinaTelecom.com;",tTime,";",lSessionIdSeq);
		
    snprintf(Head.m_sSrvTextId, sizeof(Head.m_sSrvTextId),"Query.Balance\@%s.ChinaTelecom.com",sORG_ID);
    snprintf(Head.m_sDestRealm, sizeof(Head.m_sDestRealm),"%s.ChinaTelecom.com",sORG_ID);
    snprintf(Head.m_sOrignHost,sizeof(Head.m_sOrignHost),"%s","ABM2\@001.ChinaTelecom.com");
    strcpy(Head.m_sOrignRealm,"001.ChinaTelecom.com");
    strcpy(Head.m_sSrvFlowId,"qryTest"); // 按规则，取源m_sSrvFlowId后追加
    Head.m_iAuthAppId = 4;
    Head.m_iReqType = 4;
    
    __DEBUG_LOG0_(0, "_dQueryAll:: _deal  转发消息的消息头");
    __DEBUG_LOG1_(0, "[主动划拨-收发CCR,划拨查询]:SESSION_ID:[%s]",Head.m_sSessionId);
    __DEBUG_LOG1_(0, "_dQueryAll:: 发送到省里的m_sDestRealm=%s!",Head.m_sDestRealm);
    __DEBUG_LOG1_(0, "_dQueryAll:: 发送到省里的m_sSessionId=%s!",Head.m_sSessionId);
    __DEBUG_LOG1_(0, "_dQueryAll:: 发送到省里的m_sSrvTextId=%s!",Head.m_sSrvTextId);
    __DEBUG_LOG1_(0, "_dQueryAll:: 发送到省里的m_sOrignHost=%s!",Head.m_sOrignHost);
    m_poQueryAllSndCCR->m_iCmdID = ABMCMD_QRY_BAL_INFOR;
    m_poQueryAllSndCCR->m_lType = 1;
    QueryBalance tmpQueryBalance;
    memset((void *)&tmpQueryBalance,0x00,sizeof(tmpQueryBalance));
    strcpy(tmpQueryBalance.m_sDestID,pIn->m_sDestAcct);
    tmpQueryBalance.m_iDestIDType = 2; //暂写死.用户号码类型,0-客户ID,1-用户ID,2-用户号码
    tmpQueryBalance.m_iDestAttr = pIn->m_iDestAttr;
    // 区号
    char sOrgId[4];
    char sLocalAreaId[4];
    iRet = m_dccOperation->qryAccountOrgId(pIn->m_sDestAcct,sOrgId,sLocalAreaId);
    if(iRet != 0 )
    {
    	__DEBUG_LOG0_(0, "_dQueryAll:: _deal  查询号码区号失败.");
    }
    strcpy(tmpQueryBalance.m_sAreaCode,sLocalAreaId);
    
    tmpQueryBalance.m_iQueryFlg = pIn->m_iQueryFlg;
    tmpQueryBalance.m_iQueryItemTyp = pIn->m_iQueryItemType;
    memcpy((void *)m_poQueryAllSndCCR->m_sDccBuf,(void *)&Head,sizeof(Head));
    m_poQueryAllSndCCR->clear();

    __DEBUG_LOG0_(0,"打印转发的请求消息");
    __DEBUG_LOG1_(0,"打印转发的请求消息m_iDestAttr=%d",tmpQueryBalance.m_iDestAttr);
    __DEBUG_LOG1_(0,"打印转发的请求消息m_iDestIDType=%d",tmpQueryBalance.m_iDestIDType);
    __DEBUG_LOG1_(0,"打印转发的请求消息m_iQueryFlg=%d",tmpQueryBalance.m_iQueryFlg);
    __DEBUG_LOG1_(0,"打印转发的请求消息m_iQueryItemTyp=%d",tmpQueryBalance.m_iQueryItemTyp);
    __DEBUG_LOG1_(0,"打印转发的请求消息m_sAreaCode=%s",tmpQueryBalance.m_sAreaCode);
    __DEBUG_LOG1_(0,"打印转发的请求消息m_sDestID=%s",tmpQueryBalance.m_sDestID);
    __DEBUG_LOG1_(0,"打印转发的请求消息m_iCmdID=%d",m_poQueryAllSndCCR->m_iCmdID);
    if (!m_poQueryAllSndCCR->addReqBal(tmpQueryBalance)) 
    {
        __DEBUG_LOG1_(0, "_dQueryAll:: _deal m_poQueryAllSndCCR 打包发送到省里的请求失败,err_code!",QRY_PACKMSG_TO_PROC_ERR);
        m_poQueryAllSndCCR->setRltCode(ECODE_PACK_FAIL);
        return QRY_PACKMSG_TO_PROC_ERR;
    }
    __DEBUG_LOG0_(0,"打包请求消息成功");
    
    // add 2011.7.9
	// 取DCC_INFO_RECORD_SEQ
	long lDccRecordSeq = 0L;
	strcpy(dccType,"R"); 
	
	iRet = m_dccOperation->QueryDccRecordSeq(lDccRecordSeq);
	
	// 保存发出的CCR信息
	__DEBUG_LOG0_(0, "[划拨查询-收发CCR,划拨查询]:保存发出的DCC包信息入TT");
	// modify.增加保存源DCC会话Session-id
	iRet = m_dccOperation->insertDccInfo(Head,dccType,lDccRecordSeq,m_oCCRHead.m_sSessionId);
	if(iRet != 0)
	{
		__DEBUG_LOG0_(0, "[主动划拨-收发CCR,划拨查询]:保存DCC业务信息失败");
		throw SAVE_PLATFOR_CCR_BIZ_ERR;
	}
    
    return 0;
}  

// 如果为互联网用户，只查询支付账户后返回
int QueryAll::_dQueryCenterABM(QueryAllPay *pIn)
{
    int iRet = -1;
    __DEBUG_LOG0_(0, "_dQueryCenterABM.互联网用户，自服务平台余额划拨查询");
    // 返回自服务平台的CCA
    HssResMicroPayCCA * tmpRlt = NULL;
    tmpRlt = (HssResMicroPayCCA *)m_poQueryAllCCA;	
	
    
    MicroPaySrv tmpSrv;
    // 响应流水
    snprintf(m_oRlt.m_sResID, sizeof(m_sReqID),"%s",pIn->m_sReqID);
    vector<MicroPaySrv> vData;
    vData.clear();
    char sORG_ID[8];
    char strTmp[20];
    double dtmp;
    int iServPlatForm = 0;

    __DEBUG_LOG1_(0, "打印自服务平台请求消息m_iDestAttr=%d",pIn->m_iDestAttr);
    __DEBUG_LOG1_(0, "打印自服务平台请求消息m_iDestType=%d",pIn->m_iDestType);
    __DEBUG_LOG1_(0, "打印自服务平台请求消息m_iQueryFlg=%d",pIn->m_iQueryFlg);
    __DEBUG_LOG1_(0, "打印自服务平台请求消息m_iQueryItemType=%d",pIn->m_iQueryItemType);
    __DEBUG_LOG1_(0, "打印自服务平台请求消息m_sDestAcct=%s",pIn->m_sDestAcct);
    __DEBUG_LOG1_(0, "打印自服务平台请求消息m_sReqID=%s",pIn->m_sReqID);// 自服务平台请求ID
    __DEBUG_LOG1_(0, "打印自服务平台请求消息m_sServPlatID=%s",pIn->m_sServPlatID);
    __DEBUG_LOG1_(0, "打印自服务平台请求消息m_uiReqTime=%d",pIn->m_uiReqTime);
   
    TimesTenCMD *pTmp = m_poSql->m_poQueryAll;
    pTmp->setParam(1, pIn->m_sDestAcct);
    pTmp->Execute();
    while (!pTmp->FetchNext()) {
        memset((void *)&tmpSrv, 0x00, sizeof(tmpSrv));
        tmpSrv.m_iBalSrc = 2;
        pTmp->getColumn(1, &tmpSrv.m_iUnitTypID);
        pTmp->getColumn(2, &tmpSrv.m_lBalAmount);
        pTmp->getColumn(3, sORG_ID);
        
        vData.push_back(tmpSrv);
    }
    pTmp->Close();
    
    if (vData.size() == 0 )
    {
        __DEBUG_LOG1_(0, "_dQueryAll:: _deal  查询无记录!err_code = %d",QRY_BALANCE_NODATA_ERR);
        return QRY_BALANCE_NODATA_ERR;
    }
    if (vData.size() != 1)
    {
        __DEBUG_LOG1_(0, "_dQueryAll:: _deal  查询多条记录!err_code = %d",QRY_MUL_RECORD_ERR);
        return QRY_MUL_RECORD_ERR;
    }

    //======================
    Date cur;
    m_oRlt.m_uiRltCode = 0;
    m_oRlt.m_uiResTime = compute(cur.toString());
    // Modfiy 2011.7.11 响应流水应该为自服务平台的请求流水
    strncpy(m_oRlt.m_sResID,pIn->m_sReqID,sizeof(m_oRlt.m_sResID));
    
    tmpRlt->m_iCmdID = ABMCMD_QRY_MICROPAY_INFO;
    __DEBUG_LOG0_(0, "打印余额划拨响应消息");
    __DEBUG_LOG1_(0, "打印余额划拨响应消息:处理结果:m_uiRltCode=[%d]",m_oRlt.m_uiRltCode);
    __DEBUG_LOG1_(0, "打印余额划拨响应消息m_sResID=%s",m_oRlt.m_sResID);
    __DEBUG_LOG1_(0, "打印余额划拨响应消息:响应时间:m_uiResTime=[%d]",m_oRlt.m_uiResTime);
    __DEBUG_LOG1_(0, "打印余额划拨响应消息:m_sParaFieldResult=[%s]",m_oRlt.m_sParaFieldResult);
    __DEBUG_LOG1_(0, "打印余额划拨响应消息:m_iCmdID=[%d]",tmpRlt->m_iCmdID);
    iRet = tmpRlt->addRlt(m_oRlt);
    if (iRet != true)
    {
        __DEBUG_LOG0_(0, "打包返回到自服务平台的消息结果码出错!");
        return QRY_PACK_SERVICE_FAIL_ERR;
    }
    __DEBUG_LOG0_(0, "打包余额划拨查询响应的结果码完成。");

    __DEBUG_LOG0_(0, "打包全国中心数据");
    __DEBUG_LOG1_(0, "打包全国中心数据m_iBalSrc=%d",tmpSrv.m_iBalSrc);
    __DEBUG_LOG1_(0, "打包全国中心数据m_iUnitTypID=%d",tmpSrv.m_iUnitTypID);
    __DEBUG_LOG1_(0, "打包全国中心数据m_lBalAmount=%ld",tmpSrv.m_lBalAmount);
    iRet = tmpRlt->addSrv(tmpSrv);
    if (iRet != true)
    {
        __DEBUG_LOG0_(0, "打包返回到自服务平台的消息全国中心余额出错!");
        return QRY_PACK_SERVICE_FAIL_ERR;
    }

    // 返自服务平台的CCA
    g_toPlatformFlag = true;
    
    return 0;
}

// 余额划拨查询-处理省返回的CCA，并返CCA至自服务平台
int QueryAll::dQueryAllCCA(ABMCCR * poQueryMsg, ABMCCA * poQueryRlt)
{
    __DEBUG_LOG0_(0, "处理省返回的CCA，并返回CCA至自服务平台");
    int iRet;
    HssQueryBalanceCCA * tmp = NULL;
    HssResMicroPayCCA * tmpRlt = NULL;
    tmp = (HssQueryBalanceCCA *)poQueryMsg;
    tmpRlt = (HssResMicroPayCCA *)poQueryRlt;
    memset((void *)poQueryRlt, 0x00,sizeof(ABMCCA)); //设置返回到自服务平台的消息为空
    vector<ResBalanceCode *> vPayRlt;
    vector<ResQueryBalance *> vPaySrv;
    MicroPaySrv tmpSrv;
    vPayRlt.clear();
    vPaySrv.clear();
    
    struct struDccHead Head;
    memset((void *)&Head,0x00,sizeof(Head));
    memcpy((void *)&Head, tmp->m_sDccBuf, sizeof(Head));
    __DEBUG_LOG0_(0, "打印接收到的省里的响应的消息头");
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_iAuthAppId=%d",Head.m_iAuthAppId);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_iReqNumber=%d",Head.m_iReqNumber);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_iReqType=%d",Head.m_iReqType);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_iResultCode=%d",Head.m_iResultCode);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sDestRealm=%s",Head.m_sDestRealm);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sOrignHost=%s",Head.m_sOrignHost);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sOutPlatform=%s",Head.m_sOutPlatform);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sSessionId=%s",Head.m_sSessionId);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sSrvFlowId=%s",Head.m_sSrvFlowId);
    __DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sSrvTextId=%s",Head.m_sSrvTextId);
     
     char sSessionId[101]={0};
     struct struDccHead ttHead;
     memset((void *)&ttHead, 0x00, sizeof(ttHead));
     TimesTenCMD *pTmpGetDcc = m_poSql->m_poGetDcc;
     try
     {
	// add 2011.7.9
	// 根据省里返回的CCA-session-id,找自服务平台的请求session-id
	//根据CCA响应流水去找自服务平台请求CCR的会话ID
	
	iRet = m_dccOperation->qryPreSessionId(Head.m_sSessionId,sSessionId);
	if(iRet != 0)
	{
		__DEBUG_LOG0_(0, "查询CCA会话ID的源Session Id失败.");
		return QRY_PRE_SESSION_ID_ERR;
	}
	__DEBUG_LOG1_(0, "========>自服务平台发起CCR的SESSION_ID:[%s]",sSessionId);

	//查询数据库,根据session_id查询出自服务平台的请求消息头
        pTmpGetDcc->setParam(1, Head.m_sSessionId);
        pTmpGetDcc->Execute();
        if(!pTmpGetDcc->FetchNext()) 
        {
            pTmpGetDcc->getColumn(1, ttHead.m_sSessionId);
            pTmpGetDcc->getColumn(2, ttHead.m_sOrignHost);
            pTmpGetDcc->getColumn(3, ttHead.m_sOrignRealm);
            pTmpGetDcc->getColumn(4, ttHead.m_sDestRealm);
            pTmpGetDcc->getColumn(5, ttHead.m_sSrvTextId);
            pTmpGetDcc->getColumn(6, ttHead.m_sSrvFlowId);
        }
        
         pTmpGetDcc->Close();
	if (strlen(ttHead.m_sSessionId) == 0)
	{
		__DEBUG_LOG0_(0, "deal::deal 查询DCC消息头为空,err_code=%d",QRY_GET_DCC_NOMSG_ERR);
		return QRY_GET_DCC_NOMSG_ERR;
	}
	
	__DEBUG_LOG0_(0, "打印根据接受的消息session_id查询出的消息头");
	__DEBUG_LOG1_(0, "打印接受到的消息的消息头m_iAuthAppId=%d",ttHead.m_iAuthAppId);
	__DEBUG_LOG1_(0, "打印接受到的消息的消息头m_iReqNumber=%d",ttHead.m_iReqNumber);
	__DEBUG_LOG1_(0, "打印接受到的消息的消息头m_iReqType=%d",ttHead.m_iReqType);
	__DEBUG_LOG1_(0, "打印接受到的消息的消息头m_iResultCode=%d",ttHead.m_iResultCode);
	__DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sDestRealm=%s",ttHead.m_sDestRealm);
	__DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sOrignHost=%s",ttHead.m_sOrignHost);
	__DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sOutPlatform=%s",ttHead.m_sOutPlatform);
	__DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sSessionId=%s",ttHead.m_sSessionId);
	__DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sSrvFlowId=%s",ttHead.m_sSrvFlowId);
	__DEBUG_LOG1_(0, "打印接受到的消息的消息头m_sSrvTextId=%s",ttHead.m_sSrvTextId);
    }
    catch(TTStatus oSt)
    {
        __DEBUG_LOG1_(0, "deal::deal 查询DCC消息头失败,err_msg=%s",oSt.err_msg);
        __DEBUG_LOG1_(0, "deal::deal 查询DCC消息头失败,err_code=%d",QRY_GET_DCC_FAIL_ERR);
        return QRY_GET_DCC_FAIL_ERR;
    }
   
    
    
    //查询数据库,根据session_id查询出全国中心ABM的数据
    memset((void *)&tmpSrv, 0x00, sizeof(tmpSrv));
    TimesTenCMD *pTmpGetMsg = m_poSql->m_poGetMsg;
    try
    {
    	// todo:应该根据自服务平台的session-id去查询
        //pTmpGetMsg->setParam(1, Head.m_sSessionId);
        pTmpGetMsg->setParam(1, sSessionId);
        pTmpGetMsg->Execute();
        if(!pTmpGetMsg->FetchNext()) 
        {
            tmpSrv.m_iBalSrc = 2;
            pTmpGetMsg->getColumn(1, &tmpSrv.m_iUnitTypID);
            pTmpGetMsg->getColumn(2, &tmpSrv.m_lBalAmount);
            pTmpGetMsg->getColumn(3, tmpSrv.m_sRequestId);
        }
    }
    catch(TTStatus oSt)
    {
        __DEBUG_LOG1_(0, "deal::deal 查询全国ABM数据失败,err_msg=%s",oSt.err_msg);
        __DEBUG_LOG1_(0, "deal::deal 查询全国ABM数据失败,err_code=%d",QRY_GET_ABMDATA_FAIL_ERR);
        return QRY_GET_ABMDATA_FAIL_ERR;
    }
    pTmpGetDcc->Close();
    if (tmpSrv.m_iBalSrc == 0)
    {
        __DEBUG_LOG0_(0, "deal::deal 查询保存的全国ABM数据为空,err_code=%d",QRY_GET_ABMDATA_NODATA_ERR);
        return QRY_GET_ABMDATA_NODATA_ERR;
    }

    __DEBUG_LOG0_(0, "打印根据接受的消息session_id查询出的全国中心的数据");
    __DEBUG_LOG1_(0, "查询全国中心的数据-余额帐本来源:m_iBalSrc=%d",tmpSrv.m_iBalSrc);
    __DEBUG_LOG1_(0, "查询全国中心的数据-余额类型:m_iUnitTypID=%d",tmpSrv.m_iUnitTypID);
    __DEBUG_LOG1_(0, "查询全国中心的数据-余额:m_lBalAmount=%ld",tmpSrv.m_lBalAmount);
    __DEBUG_LOG1_(0, "查询全国中心的数据-返回自服务平台的响应流水:m_sRequestId:%s",tmpSrv.m_sRequestId);
    
    //设置消息头
    memcpy((void *)tmpRlt->m_sDccBuf,(void *)&ttHead,sizeof(ttHead));
    tmpRlt->clear();
    __DEBUG_LOG0_(0, "设置返回自服务平台的余额划拨查询请求的CCA的消息头完成");

    //消息解包
    if ((iRet=tmp->getResPons(vPayRlt)) == 0) {
        __DEBUG_LOG0_(0, "getRlt::deal 解析省里返回的消息出错err_msg=%d",QRY_UNPACK_PROC_FAIL_ERR);
        m_oRlt.m_uiRltCode = QRY_UNPACK_PROC_FAIL_ERR;
        return QRY_UNPACK_PROC_FAIL_ERR;
    }
    else if (iRet < 0) {
        __DEBUG_LOG0_(0, "getRlt::deal 解析省里返回的消息出错err_msg=%d",QRY_UNPACK_PROC_FAIL_ERR);
        m_oRlt.m_uiRltCode = QRY_UNPACK_PROC_FAIL_ERR;
        return QRY_UNPACK_PROC_FAIL_ERR;
    }
    if (iRet != 1){
        __DEBUG_LOG0_(0, "getRlt::deal 解析省里返回的消息出错err_msg=%d",QRY_UNPACK_PROC_FAIL_ERR);
        m_oRlt.m_uiRltCode = QRY_UNPACK_PROC_FAIL_ERR;
        return QRY_UNPACK_PROC_FAIL_ERR;
    }
    __DEBUG_LOG0_(0, "解析省里返回的消息的返回码部分:完成.第一层响应信息：");
    __DEBUG_LOG1_(0, "解析省里返回的消息的返回码部分:可以划拨到支付帐户的总余额:m_lTotalBalAvail=%ld",vPayRlt[0]->m_lTotalBalAvail);
    __DEBUG_LOG1_(0, "解析省里返回的消息的返回码部分:m_sParaFieldResult=%s",vPayRlt[0]->m_sParaFieldResult);
    __DEBUG_LOG1_(0, "解析省里返回的消息的返回码部分:m_uiRltCode=%d",vPayRlt[0]->m_uiRltCode);
    
    
    for (int i=0; i<vPayRlt.size(); ++i)
    {
        tmp->getResBal(vPayRlt[i], vPaySrv);
        break;
    }

    /*
    if (vPaySrv.size() != 1)
    {
        __DEBUG_LOG0_(0, "省ABM返回的消息解包出错!");
        m_oRlt.m_uiRltCode = QRY_UNPACK_PROC_FAIL_ERR;
        return QRY_UNPACK_PROC_FAIL_ERR;
    }
    */
    
    for(int j=0;j<vPaySrv.size();j++)
    {
    __DEBUG_LOG0_(0, "解析省里返回的消息的业务部分完成");
    __DEBUG_LOG1_(0, "解析省里返回的消息的业务部分完成:m_iUnitTypID=%d",vPaySrv[j]->m_iUnitTypID);
    __DEBUG_LOG1_(0, "解析省里返回的消息的业务部分完成:余额帐本剩余金额:m_lBalAmount=[%ld]",vPaySrv[j]->m_lBalAmount);
    __DEBUG_LOG1_(0, "解析省里返回的消息的业务部分完成:余额帐本当月可使用余额:m_lBalAvailAble=[%ld]",vPaySrv[j]->m_lBalAvailAble);
    __DEBUG_LOG1_(0, "解析省里返回的消息的业务部分完成:m_lBalReserved=%ld",vPaySrv[j]->m_lBalReserved);
    __DEBUG_LOG1_(0, "解析省里返回的消息的业务部分完成:m_lBalUsed=%ld",vPaySrv[j]->m_lBalUsed);
    __DEBUG_LOG1_(0, "解析省里返回的消息的业务部分完成:m_sBalItmDetail=%s",vPaySrv[j]->m_sBalItmDetail);
    __DEBUG_LOG1_(0, "解析省里返回的消息的业务部分完成:m_uiEffDate=%ld",vPaySrv[j]->m_uiEffDate);
    __DEBUG_LOG1_(0, "解析省里返回的消息的业务部分完成:m_uiExpDate=%ld",vPaySrv[j]->m_uiExpDate);
    }

    Date cur;
    m_oRlt.m_uiRltCode = 0;
    m_oRlt.m_uiResTime = compute(cur.toString());
    // Modfiy 2011.7.11 响应流水应该为自服务平台的请求流水
    //snprintf(m_oRlt.m_sResID, sizeof(m_oRlt.m_sResID), 
    //    "002%s%08d00\0", cur.toString(),m_iCount++);
    //snprintf(m_oRlt.m_sResID,sizeof(m_oRlt.m_sResID),"%s\0",);
    strncpy(m_oRlt.m_sResID,tmpSrv.m_sRequestId,sizeof(m_oRlt.m_sResID));
    
    tmpRlt->m_iCmdID = ABMCMD_QRY_MICROPAY_INFO;
    __DEBUG_LOG0_(0, "打印余额划拨响应消息");
    __DEBUG_LOG1_(0, "打印余额划拨响应消息:处理结果:m_uiRltCode=[%d]",m_oRlt.m_uiRltCode);
    __DEBUG_LOG1_(0, "打印余额划拨响应消息m_sResID=%s",m_oRlt.m_sResID);
    __DEBUG_LOG1_(0, "打印余额划拨响应消息:响应时间:m_uiResTime=[%d]",m_oRlt.m_uiResTime);
    __DEBUG_LOG1_(0, "打印余额划拨响应消息:m_sParaFieldResult=[%s]",m_oRlt.m_sParaFieldResult);
    __DEBUG_LOG1_(0, "打印余额划拨响应消息:m_iCmdID=[%d]",tmpRlt->m_iCmdID);
    iRet = tmpRlt->addRlt(m_oRlt);
    if (iRet != true)
    {
        __DEBUG_LOG0_(0, "打包返回到自服务平台的消息结果码出错!");
        return QRY_PACK_SERVICE_FAIL_ERR;
    }
    __DEBUG_LOG0_(0, "打包余额划拨查询响应的结果码完成。");

    __DEBUG_LOG0_(0, "打包全国中心数据");
    __DEBUG_LOG1_(0, "打包全国中心数据m_iBalSrc=%d",tmpSrv.m_iBalSrc);
    __DEBUG_LOG1_(0, "打包全国中心数据m_iUnitTypID=%d",tmpSrv.m_iUnitTypID);
    __DEBUG_LOG1_(0, "打包全国中心数据m_lBalAmount=%ld",tmpSrv.m_lBalAmount);
    iRet = tmpRlt->addSrv(tmpSrv);
    if (iRet != true)
    {
        __DEBUG_LOG0_(0, "打包返回到自服务平台的消息全国中心余额出错!");
        return QRY_PACK_SERVICE_FAIL_ERR;
    }
    __DEBUG_LOG0_(0, "打包全国中心数据完成。");
    memset((void *)&tmpSrv,0x00,sizeof(tmpSrv));
    tmpSrv.m_iBalSrc = 1;
    tmpSrv.m_iUnitTypID = vPaySrv[0]->m_iUnitTypID;
    // Modfiy 2011.7.11 暂时取帐本剩余金额
    //tmpSrv.m_lBalAmount = vPayRlt[0]->m_lTotalBalAvail;
    tmpSrv.m_lBalAmount = vPayRlt[0]->m_lTotalBalAvail;
    
    __DEBUG_LOG0_(0, "打包省余额数据。");
    __DEBUG_LOG1_(0, "打包省余额数据:m_iBalSrc=%d",tmpSrv.m_iBalSrc);
    __DEBUG_LOG1_(0, "打包省余额数据:m_iUnitTypID=%d",tmpSrv.m_iUnitTypID);
    __DEBUG_LOG1_(0, "打包省余额数据:m_lBalAmount=%ld",tmpSrv.m_lBalAmount);
    iRet = tmpRlt->addSrv(tmpSrv);
    if (iRet != true)
    {
        __DEBUG_LOG0_(0, "打包返回到自服务平台的消息省ABM余额出错!");
        return QRY_PACK_SERVICE_FAIL_ERR;
    }
    __DEBUG_LOG0_(0, "打包省余额数据完成。");
    
    // 返自服务平台的CCA
    g_toPlatformFlag = true;
    
    return 0;

}

unsigned int QueryAll::compute(char *str)
{
    char strdate[20];
    strncpy(strdate,str,14);
    strdate[14] = '\0';
    struct tm t;
    t.tm_sec = atoi (strdate+12);
    strdate[12] = 0;
    t.tm_min = atoi (strdate+10);
    strdate[10] = 0;
    t.tm_hour = atoi (strdate+8);
    strdate[8] = 0;
    t.tm_mday = atoi (strdate+6);
    strdate[6] = 0;
    t.tm_mon = atoi (strdate+4) - 1;
    strdate[4] = 0;
    t.tm_year = atoi (strdate) - 1900;
    return (unsigned int)mktime(&t);
}


