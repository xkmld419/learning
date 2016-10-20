#include "HssQuery.h"
#include "HssQuerySql.h"
#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"
#include "Date.h"
#include <string.h>

#define HSS_QRY_BILLING_CYCLE_ERROR	13730		

static vector<QueryBalance *> g_vBal;
static vector<RechargeRecQuery *> g_vRec;
static vector<QueryPayLogByAccout *> g_vPay;
static vector<RechargeBillQuery *> g_vBil;

extern bool g_toPlatformFlag;

HssQuery::HssQuery()
{
    m_poSql = NULL;
    
    m_poHBalCCR = NULL;
    m_poHBalCCA = NULL;
    
    m_poHRecCCR = NULL;
    m_poHRecCCA = NULL;
    
    m_poHPayCCR = NULL;
    m_poHPayCCA = NULL;
    
    m_poHBilCCR = NULL;
    m_poHBilCCA = NULL;

    m_iCount = 0;
}

HssQuery::~HssQuery()
{
    g_vBal.clear();
    g_vRec.clear();
    g_vPay.clear();
    g_vBil.clear();
    #define __FREE_PTR_(p) if(p != NULL)\
            {\
                delete p;\
                p = NULL;\
            }
    __FREE_PTR_(m_poSql);
    __FREE_PTR_(m_poHBalCCA);
    __FREE_PTR_(m_poHRecCCA);
    __FREE_PTR_(m_poHPayCCA);
    __FREE_PTR_(m_poHBilCCA);

}

int HssQuery::init(ABMException &oExp)
{
    m_poSql = new HssQuerySql;

    if(m_poSql == NULL)
    {
        ADD_EXCEPT0(oExp, "申请内存失败"); 
        return -1;
    }

    return m_poSql->init(oExp);
}
int HssQuery::deal(ABMCCR* poQueryMsg, ABMCCA * poQueryRlt)
{
    //dTestQuery(); //测试
    int  iRet = -1;
    long  lSeq;
    struct struDccHead savedcc;
    memset((void *)&savedcc,0x00,sizeof(savedcc));
    memcpy((void *)&savedcc,(void *)(poQueryMsg->m_sDccBuf),sizeof(savedcc));
    if ((m_oDccOperation.QueryDccRecordSeq(lSeq)) != 0)
    {
        __DEBUG_LOG0_(0, "HssQuery::deal 获取插入dcc头的sequence失败");
        return -1;
    }
    if ((m_oDccOperation.insertDccInfo(savedcc,"R",lSeq)) !=0 )
    {
        __DEBUG_LOG0_(0, "HssQuery::deal 保存dcc头失败");
        return -1;
    }
    
    // add 2011.7.8 m_sOrignHost 判断是否是自服务平台发起
    if(strcmp(savedcc.m_sOrignHost,"ZFP")==0)
    {
    	g_toPlatformFlag = true;	
    }
    
    poQueryRlt->memcpyh(poQueryMsg);
    
    switch (poQueryMsg->m_iCmdID) {
    case ABMCMD_QRY_BAL_INFOR:
    {	// 余额查询-3.4.6.13
        memset((void *)&m_oBalanceRes,0x00,sizeof(m_oBalanceRes));
        iRet = dQueryBalance(poQueryMsg, poQueryRlt);// 调用业务
        if (iRet != 0)
        {
            m_oBalanceRes.m_uiRltCode = iRet;
            m_oBalanceRes.m_lTotalBalAvail = 0;
            m_poHBalCCA->clear();
            if (!m_poHBalCCA->addRespons(m_oBalanceRes))
            {
                __DEBUG_LOG0_(0, "HssQuery::deal 打包返回错误码失败");
                m_poHBalCCA->clear();
            }
        }
        break;
    }
    case ABMCMD_QRY_REC_INFOR:
    {	// 充退记录查询.14
    	g_toPlatformFlag = true;
        memset((void *)&m_oRes,0x00,sizeof(m_oRes));
        iRet = dRechargeRecQuery(poQueryMsg, poQueryRlt);
        if (iRet != 0)
        {
            m_oRes.m_uiRltCode = iRet;
            m_poHRecCCA->clear();
            if (!m_poHRecCCA->addRespons(m_oRes))
            {
                __DEBUG_LOG0_(0, "HssQuery::deal 打包返回错误码失败");
                m_poHRecCCA->clear();
            }
        }
        break;
    }
    case ABMCMD_QRY_PAY_INFOR:
    {	// 交易记录查询.15
    	g_toPlatformFlag = true;
        memset((void *)&m_oPayRes,0x00,sizeof(m_oPayRes));
        iRet = dQueryPayLogByAccout(poQueryMsg, poQueryRlt);
        if (iRet != 0)
        {
            m_oPayRes.m_uiRltCode = iRet;
            m_poHPayCCA->clear();
            if (!m_poHPayCCA->addRespons(m_oPayRes))
            {
                __DEBUG_LOG0_(0, "HssQuery::deal 打包返回错误码失败");
                m_poHPayCCA->clear();
            }
        }
        break;
    }
    case ABMCMD_QRY_BIL_INFOR:
    {	// 电子账单查询.16
    	g_toPlatformFlag = true;
        memset((void *)&m_oBilRes,0x00,sizeof(m_oBilRes));
        iRet = dRechargeBillQuery(poQueryMsg, poQueryRlt);
        if (iRet != 0)
        {
            m_oBilRes.m_uiRltCode = iRet;
            m_poHBilCCA->clear();
            if (!m_poHBilCCA->addRespons(m_oBilRes))
            {
                __DEBUG_LOG0_(0, "HssQuery::deal 打包返回错误码失败");
                m_poHBilCCA->clear();
            }
        }
        break;
    }
    default:
        __DEBUG_LOG1_(0, "HssQuery::deal Receive unknown Command ID = %d", poQueryMsg->m_iCmdID);
        break;
    }
    return 0;
}

// 余额查询-3.4.6.13
int HssQuery::dQueryBalance(ABMCCR * poQueryMsg, ABMCCA * poQueryRlt)
{

    int iRet;

    m_poHBalCCR = (HssQueryBalanceCCR *)poQueryMsg;
    m_poHBalCCA = (HssQueryBalanceCCA *)poQueryRlt;
    m_poHBalCCA->clear();
    g_vBal.clear();
    
    vector<QueryBalance *>::iterator itvBal;
    
    //消息解包
    if ((iRet=m_poHBalCCR->getReqBal(g_vBal)) == 0) {
        __DEBUG_LOG0_(0, "getReqBal::deal 消息包为NULL");
        m_poHBalCCA->setRltCode(ECODE_NOMSG);
        return QRY_ECODE_NOMSG_ERR;
    }
    else if (iRet < 0) {
        __DEBUG_LOG0_(0, "getReqBal::deal 消息解包出错!");
        m_poHBalCCA->setRltCode(ECODE_UNPACK_FAIL);
        return QRY_ECODE_UNPACK_FAIL;
    }
    if (iRet != 1){
        __DEBUG_LOG0_(0, "getReqBal::deal 消息解包出错，请求信息有多条!");
        m_poHBalCCA->setRltCode(ECODE_UNPACK_FAIL);
        return QRY_ECODE_UNPACK_FAIL;
    }

    try {
        for (itvBal=g_vBal.begin(); itvBal!=g_vBal.end(); ++itvBal) 
        {
            iRet = _dQueryBalance(*itvBal);
            return iRet;
        }
        return 0;
    }
    catch(TTStatus oSt) {
        __DEBUG_LOG1_(0, "getReqBal::deal oSt.err_msg=%s", oSt.err_msg);
    }
    
    m_poHBalCCA->clear();
    m_poHBalCCA->setRltCode(ECODE_TT_FAIL);
    
    return 0;

}

// 余额查询
int HssQuery::_dQueryBalance(QueryBalance *pIn)
{
    strcpy(m_oBalanceRes.m_sResID,pIn->m_sReqID);   // 响应流水
    m_oBalanceRes.m_uiRltCode = 0;
    m_oBalanceRes.m_lTotalBalAvail = 0;
    ResQueryBalance oData;
    vector<ResQueryBalance> vData;
    vData.clear();
    char strTmp[20];
    Date cur;
    double dtmp;
    char strEffDate[20];
    char strExpDate[20];
    int  iServID = 0;
	__DEBUG_LOG0_(0, "打印余额查询请求消息");
    __DEBUG_LOG1_(0, "pIn->m_sDestID=%s\n",pIn->m_sDestID);
    __DEBUG_LOG1_(0, "pIn->m_iDestIDType=%d\n",pIn->m_iDestIDType);
    __DEBUG_LOG1_(0, "pIn->m_iDestAttr=%d\n",pIn->m_iDestAttr);
    TimesTenCMD *pTmp = NULL;
    if (pIn->m_iDestIDType == 2)
    {
        pTmp = m_poSql->m_poBal;
        pTmp->setParam(1, pIn->m_sDestID);
        strncpy(strTmp, cur.toString("yyyymm"),6);
        strTmp[6] = '\0';
        pTmp->setParam(2, strTmp);
        pTmp->setParam(3, pIn->m_sDestID);
    }
    else if (pIn->m_iDestIDType == 1)
    {
        pTmp = m_poSql->m_poBal2;
        char *str=pIn->m_sDestID;
        if (isNumberStr(pIn->m_sDestID) != 0)
        {    
            m_oBalanceRes.m_uiRltCode = QRY_BAL_SERV_NOTDIGIT;
            return QRY_BAL_SERV_NOTDIGIT;
        }
        iServID = atoi(pIn->m_sDestID);
        pTmp->setParam(1, iServID);
        strncpy(strTmp, cur.toString("yyyymm"),6);
        strTmp[6] = '\0';
        pTmp->setParam(2, strTmp);
        pTmp->setParam(3, iServID);
    }
    else
    {
        __DEBUG_LOG1_(0, "未识别的用户类型pIn->m_iDestIDType=%d\n",pIn->m_iDestIDType);
        m_oBalanceRes.m_uiRltCode = QRY_BAL_UNKNOWNTYPE;
        return QRY_BAL_UNKNOWNTYPE;
    }
    __DEBUG_LOG1_(0, "查询类型pIn->m_iDestIDType=%d\n",pIn->m_iDestIDType);
   
    //pTmp->setParam(1, pIn->m_sDestID);
    //取消用户类型和用户属性的匹配
    //snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_iDestIDType);
    //pTmp->setParam(2, strTmp);
    //snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_iDestAttr);
    //pTmp->setParam(3, strTmp);
    //pTmp->setParam(4, pIn->m_iQueryFlg);
    //strncpy(strTmp, cur.toString("yyyymm"),6);
    //strTmp[6] = '\0';
    //pTmp->setParam(2, strTmp);
    //pTmp->setParam(3, pIn->m_sDestID);
    //snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_iDestIDType);
    //pTmp->setParam(4, strTmp);
    //snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_iDestAttr);
    //pTmp->setParam(5, strTmp);
    pTmp->Execute();
    while (!pTmp->FetchNext()) {
        memset((void *)&oData, 0x00, sizeof(oData));
        pTmp->getColumn(1, oData.m_sBalItmDetail);
        pTmp->getColumn(2, &oData.m_iUnitTypID);
        pTmp->getColumn(3, &oData.m_lBalAmount);
        oData.m_lBalAvailAble = oData.m_lBalAmount;
        m_oBalanceRes.m_lTotalBalAvail += oData.m_lBalAmount;
        //接口往月欠费，删除
        //oData.m_lBalanceOweUsed = 0;
        pTmp->getColumn(4, &dtmp);
        oData.m_lBalUsed = (long)dtmp;
        oData.m_lBalReserved = oData.m_lBalAmount;
        
        memset(strEffDate, '\0', sizeof(strEffDate));
        pTmp->getColumn(5, strEffDate);
        __DEBUG_LOG1_(0, "生效日期,strEffDate:[%s]",strEffDate);
        if (strlen(strEffDate) != 0)
        {
            oData.m_uiEffDate = compute(strEffDate);
    	}
    	else
    	{
    	    oData.m_uiEffDate = 0;
    	}
    	__DEBUG_LOG1_(0, "生效日期,m_uiEffDate:[%ld]",oData.m_uiEffDate);
    	
        //sscanf(strTmp, "%ud", &oData.m_uiEffDate);
        memset(strExpDate, '\0', sizeof(strExpDate));
        pTmp->getColumn(6, strExpDate);
        __DEBUG_LOG1_(0, "失效日期,strExpDate:[%s]",strExpDate);
        if (strlen(strExpDate) != 0)
        {
            oData.m_uiExpDate = compute(strExpDate);
    	}
    	else
    	{
    	    oData.m_uiExpDate = 0;
    	}
    	__DEBUG_LOG1_(0, "失效日期,m_uiExpDate:[%ld]",oData.m_uiExpDate);
        //sscanf(strTmp, "%ud", &oData.m_uiExpDate);
        vData.push_back(oData);
    }
    pTmp->Close();
    
    if (vData.size() == 0)
    {
        __DEBUG_LOG0_(0, "RechargeBillQuery:: _deal m_poHBilCCA 查询无记录!");
        m_oBalanceRes.m_uiRltCode = QRY_BAL_NORECORD;
        return QRY_BAL_NORECORD;
    }
    /*
    else if (vData.size() != 1)
    {
        __DEBUG_LOG0_(0, "RechargeBillQuery:: _deal m_poHBilCCA 查询多条记录!");
        m_oBalanceRes.m_uiRltCode = QRY_BAL_MULRECORD;
    }
    else if(vData.size() == 1)
    {
        if (strlen(strEffDate) != 0)
        {
            oData.m_uiEffDate = compute(strEffDate);
    	}
    	else
    	{
    	    oData.m_uiEffDate = 0;
    	}
        if (strlen(strExpDate) != 0)
        {
            Date cur;
            if (strncmp(cur.toString(), strExpDate, 14) > 0)
            {
                __DEBUG_LOG0_(0, "RechargeBillQuery:: _deal m_poHBilCCA 余额账本过期!");
                m_oBalanceRes.m_uiRltCode = OVERDUE_FLG;
            }
    		
            oData.m_uiExpDate = compute(strExpDate);
        }
    	else
    	{
    	    oData.m_uiExpDate = 0;
    	}
    }
    */

    __DEBUG_LOG0_(0, "打印余额查询返回消息结果码");
    __DEBUG_LOG1_(0, "oRes.m_lTotalBalAvail=%ld\n",m_oBalanceRes.m_lTotalBalAvail);
    __DEBUG_LOG1_(0, "oRes.m_uiRltCode=%u\n",m_oBalanceRes.m_uiRltCode);
    __DEBUG_LOG1_(0, "oRes.size=%d\n",sizeof(m_oBalanceRes));
    __DEBUG_LOG1_(0, "oRes.m_sReqID=%s\n",m_oBalanceRes.m_sResID);

    if (!m_poHBalCCA->addRespons(m_oBalanceRes)) 
    {
        __DEBUG_LOG0_(0, "QueryBalance:: _deal m_poHBalCCA 打包业务结果失败!");
        m_poHBalCCA->setRltCode(ECODE_PACK_FAIL);
        return QRY_PACK_FAIL_ERR;
    }
    __DEBUG_LOG0_(0, "打包余额查询返回消息结果码完成");
	
    __DEBUG_LOG1_(0, "查询余额业务数据个数vData.size=%d\n",vData.size());
    
    for (int i=0; i<vData.size(); ++i) 
    { 
        __DEBUG_LOG0_(0, "打印余额查询返回消息业务数据");
        __DEBUG_LOG1_(0, "oData.m_sBalItmDetail=%s\n",vData[i].m_sBalItmDetail);
        __DEBUG_LOG1_(0, "oData.m_iUnitTypID=%d\n",vData[i].m_iUnitTypID);
        __DEBUG_LOG1_(0, "oData.m_lBalAmount=%ld\n",vData[i].m_lBalAmount);
        __DEBUG_LOG1_(0, "oData.m_lBalAvailAble=%ld\n",vData[i].m_lBalAvailAble);
        //__DEBUG_LOG1_(0, "oData.m_lBalanceOweUsed=%ld\n",oData.m_lBalanceOweUsed);
        __DEBUG_LOG1_(0, "oData.m_lBalUsed=%ld\n",vData[i].m_lBalUsed);
        __DEBUG_LOG1_(0, "oData.m_lBalReserved=%ld\n",vData[i].m_lBalReserved);
        __DEBUG_LOG1_(0, "oData.m_uiEffDate=%u\n",vData[i].m_uiEffDate);
        __DEBUG_LOG1_(0, "oData.m_uiExpDate=%u\n",vData[i].m_uiExpDate);
        if (!m_poHBalCCA->addResBal(vData[i]))
        {
            __DEBUG_LOG0_(0, "QueryBalance:: _deal m_poHBalCCA 打包查询结果失败!");
            m_poHBalCCA->setRltCode(ECODE_PACK_FAIL);
            return QRY_PACK_DATA_ERR;
        }
    }
    __DEBUG_LOG0_(0, "打包余额查询返回消息业务数据完成");
    return 0;
}        


int HssQuery::dRechargeRecQuery(ABMCCR * poQueryMsg, ABMCCA * poQueryRlt)
{   // 充退记录查询
    int iRet;
    m_poHRecCCR = (HssRechargeRecQueryCCR *)poQueryMsg;
    m_poHRecCCA = (HssRechargeRecQueryCCA *)poQueryRlt;
    m_poHRecCCA->clear();
    g_vRec.clear();

    //消息解包
    if ((iRet=m_poHRecCCR->getRec(g_vRec)) == 0) {
        __DEBUG_LOG0_(0, "getRec::deal 消息包为NULL");
        m_poHRecCCA->setRltCode(ECODE_NOMSG);
        return QRY_PAY_PACK_FAILE;
    }
    else if (iRet < 0) {
        __DEBUG_LOG0_(0, "getRec::deal 消息解包出错!");
        m_poHRecCCA->setRltCode(ECODE_UNPACK_FAIL);
        return QRY_PAY_PACK_FAILE;
    }
    if (iRet != 1)
    {
        __DEBUG_LOG0_(0, "getRec::deal 请求信息有多条!");
        m_poHRecCCA->setRltCode(ECODE_UNPACK_FAIL);
        return QRY_PAY_PACK_FAILE;
    }

    try {
        for (int i=0; i<g_vRec.size(); ++i) 
        {
            return _dRechargeRecQuery(g_vRec[i]);
        }
    }
    catch(TTStatus oSt) {
        __DEBUG_LOG1_(0, "getRec::deal oSt.err_msg=%s", oSt.err_msg);
    }
    
    m_poHRecCCA->clear();
    m_poHRecCCA->setRltCode(ECODE_TT_FAIL);

    return 0;
}

int HssQuery::_dRechargeRecQuery(RechargeRecQuery * pIn)
{
    Date Seq;
    /* 响应流水
    snprintf(m_oRes.m_sSeq, sizeof(m_oRes.m_sSeq), 
        "002%s%08d00\0", Seq.toString(),m_iCount++);
    */
    m_oRes.m_uiRltCode = 0;
    
    ResRechargeRecQuery oData;
    vector<ResRechargeRecQuery> vData;
    vData.clear();
    char strTmp[20];
    int iServPlatID = 0;
    int iRechargeAmount = 0;
    
    TimesTenCMD *pTmp = m_poSql->m_poRec;
    pTmp->setParam(1, pIn->m_sDestAcct);
    //snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_iDestType);
    //pTmp->setParam(2, strTmp);
    //snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_iDestAttr);
    //pTmp->setParam(3, strTmp);
    sscanf(pIn->m_sSrvPlatID,"%d", &iServPlatID);
    pTmp->setParam(2, iServPlatID);
    snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_uiStartTime);
    pTmp->setParam(3, strTmp);
    snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_uiExpTime);
    pTmp->setParam(4, strTmp);
    
        // add 2011.7.11
     __DEBUG_LOG0_(0, "RechargeBillQuery:: 查询条件:");
     __DEBUG_LOG1_(0, "查询流水号,m_sOperSeq:[%s]", pIn->m_sOperSeq);
     __DEBUG_LOG1_(0, "用户号码,m_sDestAcct:[%s]", pIn->m_sDestAcct);
     __DEBUG_LOG1_(0, "平台ID,iServPlatID:[%d]", iServPlatID);
     __DEBUG_LOG1_(0, "起始时间,m_uiStartTime:[%d]", pIn->m_uiStartTime);
     __DEBUG_LOG1_(0, "结束时间,m_uiExpTime:[%d]", pIn->m_uiExpTime);
    
    pTmp->Execute();
    while (!pTmp->FetchNext()) {
        memset((void *)&oData, 0x00, sizeof(oData));
        oData.m_uiResult = 0;
        oData.m_uiOperAct = 10;
        pTmp->getColumn(1, oData.m_sRechargeTime);
        pTmp->getColumn(2, oData.m_sDestAcct);
        memset(strTmp, '\0', sizeof(strTmp));
        pTmp->getColumn(3, strTmp);
        sscanf(strTmp, "%d", &oData.m_iDestAttr);
        pTmp->getColumn(4, &iServPlatID);
	    snprintf(oData.m_sSrvPlatID,sizeof(oData.m_sSrvPlatID),"%d\0",iServPlatID);
        memset(strTmp, '\0', sizeof(strTmp));
        pTmp->getColumn(5, strTmp);
	    if (strncmp(strTmp,"5VF",3) == 0)
            oData.m_uiChargeType = 1;
	    if (strncmp(strTmp,"5VA",3) == 0)
	        oData.m_uiChargeType = 1;
        pTmp->getColumn(6, &iRechargeAmount);
        oData.m_uiRechargeAmount = iRechargeAmount;
        
        __DEBUG_LOG1_(0, "操作类型,oData.m_uiOperAct:[%d]", oData.m_uiOperAct);
        __DEBUG_LOG1_(0, "充退时间,oData.m_sRechargeTime:[%s]", oData.m_sRechargeTime);
        __DEBUG_LOG1_(0, "用户号码,oData.m_sDestAcct:[%s]", oData.m_sDestAcct);
        __DEBUG_LOG1_(0, "被充值/退费用户属性,oData.m_iDestAttr:[%d]", oData.m_iDestAttr);
        __DEBUG_LOG1_(0, "平台ID,oData.m_sSrvPlatID:[%s]", oData.m_sSrvPlatID);
        __DEBUG_LOG1_(0, "充退来源,oData.m_uiChargeType:[%d]", oData.m_uiChargeType);
        __DEBUG_LOG1_(0, "充退金额,oData.m_uiRechargeAmount:[%d]", oData.m_uiRechargeAmount);
        
        vData.push_back(oData); 
    }
    pTmp->Close();

    if (vData.size() == 0)
    {
        __DEBUG_LOG0_(0, "RechargeBillQuery:: _deal 查询无结果!");
        m_oRes.m_uiRltCode = TT_NO_RECORD;
        m_oRes.m_uiRltCode = QRY_PAY_NORECORD_ERR;
    }
    strncpy(m_oRes.m_sSeq,pIn->m_sOperSeq,sizeof(m_oRes.m_sSeq));
    __DEBUG_LOG1_(0, "响应流水,m_oRes.m_sSeq:[%s]", m_oRes.m_sSeq);
    
    if (!m_poHRecCCA->addRespons(m_oRes)) {
        __DEBUG_LOG0_(0, "RechargeRecQuery:: _deal m_poHRecCCA 打包业务结果失败!");
        m_poHRecCCA->setRltCode(ECODE_PACK_FAIL);
        return QRY_PAY_PACKRLT_FAIL;
    }
    if (m_oRes.m_uiRltCode != 0)
    {
        return 0;
    }

    for (int i=0; i<vData.size(); ++i)
    {
        if (!m_poHRecCCA->addRec(vData[i]))
        {
            __DEBUG_LOG0_(0, "RechargeRecQuery:: _deal m_poHRecCCA 打包查询结果失败!");
            m_poHRecCCA->setRltCode(ECODE_PACK_FAIL);
            return QRY_PAY_PACKDATA_FAIL;
        }
    }
	
    return 0;
}

int HssQuery::dQueryPayLogByAccout(ABMCCR *poQueryMsg, ABMCCA *poQueryRlt)
{   // 交易记录查询
    int iRet;
    m_poHPayCCR = (HssQueryPayLogByAccoutCCR *)poQueryMsg;
    m_poHPayCCA = (HssQueryPayLogByAccoutCCA *)poQueryRlt;
    m_poHPayCCA->clear();
    g_vPay.clear();
    
    vector<QueryPayLogByAccout *>::iterator itvPay;

    //消息解包
    if ((iRet=m_poHPayCCR->getPay(g_vPay)) == 0) {
        __DEBUG_LOG0_(0, "getPay::deal 消息包为NULL");
        m_poHPayCCA->setRltCode(ECODE_NOMSG);
        return QRY_REC_UNPACK_FAIL;
    }
    else if (iRet < 0) {
        __DEBUG_LOG0_(0, "getPay::deal 消息解包出错!");
        m_poHPayCCA->setRltCode(ECODE_UNPACK_FAIL);
        return QRY_REC_UNPACK_FAIL;
    }
    if (iRet != 1)
    {
        __DEBUG_LOG0_(0, "getPay::deal 消息解包出错!");
        return QRY_REC_UNPACK_FAIL;
    }

    try {
        for (itvPay=g_vPay.begin(); itvPay!=g_vPay.end(); ++itvPay)
        {
            return _dQueryPayLogByAccout(*itvPay);
        }
    }
    catch(TTStatus oSt) {
        __DEBUG_LOG1_(0, "getPay::deal oSt.err_msg=%s", oSt.err_msg);
    }

    m_poHPayCCA->clear();
    m_poHPayCCA->setRltCode(ECODE_TT_FAIL);
    
    return 0;
}

int HssQuery::_dQueryPayLogByAccout(QueryPayLogByAccout *pIn)
{
    Date Seq;
    /* 2011.7.11 去除自定义响应流水
    snprintf(m_oPayRes.m_sSeq, sizeof(m_oPayRes.m_sSeq), 
        "002%s%08d00\0", Seq.toString(),m_iCount++);
    */
    m_oPayRes.m_uiRltCode = 0;
    
    ResQueryPayLogByAccout oData;
    vector<ResQueryPayLogByAccout> vData;
    vData.clear();
    char strTmp[20];
    int iServPlatID = 0;
    int iSpID = 0;
    int iOrderID=0;
    
    TimesTenCMD *pTmp = m_poSql->m_poPay;
    //snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_uiOperAct);
    //pTmp->setParam(1, strTmp);
    sscanf(pIn->m_sSPID, "%d",&iSpID);
    pTmp->setParam(1, iSpID);
    __DEBUG_LOG1_(0, "pIn->m_sSPID=%s\n",pIn->m_sSPID);
    sscanf(pIn->m_sSrvPlatID,"%d", &iServPlatID);
    pTmp->setParam(2, iServPlatID);
    __DEBUG_LOG1_(0, "pIn->m_sSrvPlatID=%s\n",pIn->m_sSrvPlatID);
    pTmp->setParam(3, pIn->m_sDestAcct);
    __DEBUG_LOG1_(0, "pIn->m_sDestAcct=%s\n",pIn->m_sDestAcct);
    //snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_iDestType);
    __DEBUG_LOG1_(0, "pIn->m_iDestType=%d\n",pIn->m_iDestType);
    //pTmp->setParam(4, strTmp);
    //snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_iDestAttr);
    __DEBUG_LOG1_(0, "pIn->m_iDestAttr=%d\n",pIn->m_iDestAttr);
    //pTmp->setParam(5, strTmp);
    snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_uiStartTime);
    __DEBUG_LOG1_(0, "pIn->m_uiStartTime=%d\n",pIn->m_uiStartTime);
    pTmp->setParam(4, strTmp);
    snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_uiExpTime);
    __DEBUG_LOG1_(0, "pIn->m_uiExpTime=%d\n",pIn->m_uiExpTime);
    pTmp->setParam(5, strTmp);
    
     // add 2011.7.11
     __DEBUG_LOG0_(0, "dQueryPayLogByAccout:: 交易记录查询条件:");
     __DEBUG_LOG1_(0, "商家ID,m_sSPID:[%s]", pIn->m_sSPID);
     __DEBUG_LOG1_(0, "商家名称,m_sSPName:[%s]", pIn->m_sSPName);
     __DEBUG_LOG1_(0, "平台ID,m_sSrvPlatID:[%s]", pIn->m_sSrvPlatID);
     __DEBUG_LOG1_(0, "用户标识,m_sDestAcct:[%s]", pIn->m_sDestAcct);
     __DEBUG_LOG1_(0, "被查询用户属性,m_iDestType:[%d]", pIn->m_iDestType);
     __DEBUG_LOG1_(0, "开始时间,m_uiStartTime:[%d]", pIn->m_uiStartTime);
     __DEBUG_LOG1_(0, "结束时间,m_uiExpTime:[%d]", pIn->m_uiExpTime);
     
    pTmp->Execute();
    while (!pTmp->FetchNext()) {
        memset((void *)&oData, 0x00, sizeof(oData));
        pTmp->getColumn(1, &iOrderID);
        __DEBUG_LOG1_(0, "交易记录查询返回CCA信息:iOrderID=%d\n",iOrderID);
        snprintf(oData.m_sOderID, sizeof(oData.m_sOderID),"%d\0",iOrderID);
        pTmp->getColumn(2, oData.m_sDesc);
        __DEBUG_LOG1_(0, "交易记录查询返回CCA信息:oData.m_sDesc=%s\n",oData.m_sDesc);
        pTmp->getColumn(3, &iSpID);
        __DEBUG_LOG1_(0, "交易记录查询返回CCA信息:iSpID=%d\n",iSpID);
        snprintf(oData.m_sSPID, sizeof(oData.m_sSPID),"%d\0",iSpID);
        pTmp->getColumn(4, &iServPlatID);
        __DEBUG_LOG1_(0, "交易记录查询返回CCA信息:iServPlatID=%d\n",iServPlatID);
        snprintf(oData.m_sSrvPlatID, sizeof(oData.m_sSrvPlatID),
			"%d\0", iServPlatID);
        memset(strTmp, '\0', sizeof(strTmp));
        pTmp->getColumn(5, strTmp);
        oData.m_uiPayDate = compute(strTmp);
        __DEBUG_LOG1_(0, "交易记录查询返回CCA信息:time=%s\n",strTmp);
        pTmp->getColumn(6, oData.m_sStatus);
        __DEBUG_LOG1_(0, "交易记录查询返回CCA信息:oData.m_sStatus=%s\n",oData.m_sStatus);
        pTmp->getColumn(7, oData.m_sMicropayType);
        __DEBUG_LOG1_(0, "交易记录查询返回CCA信息:oData.m_sMicropayType=%s\n",oData.m_sMicropayType);
        pTmp->getColumn(8, &oData.m_lPayAmount);
        __DEBUG_LOG1_(0, "交易记录查询返回CCA信息:oData.m_lPayAmount=%ld\n",oData.m_lPayAmount);
        vData.push_back(oData);
    }
    pTmp->Close();

    if (vData.size() == 0)
    {
        __DEBUG_LOG0_(0, "QueryPayLogByAccout:: _deal m_poHPayCCA 查无结果!");
        m_oPayRes.m_uiRltCode = QRY_REC_NORECORD_ERR;
    }

    __DEBUG_LOG1_(0, "vData.size()=%d\n",vData.size());
    
    strncpy(m_oPayRes.m_sSeq,pIn->m_sOperSeq,sizeof(m_oPayRes.m_sSeq));
    __DEBUG_LOG1_(0, "响应流水,m_oPayRes.m_sSeq:[%s]", m_oPayRes.m_sSeq);
    if (!m_poHPayCCA->addRespons(m_oPayRes)) 
    {
        __DEBUG_LOG0_(0, "QueryPayLogByAccout:: _deal m_poHPayCCA 打包业务结果失败!");
        m_poHPayCCA->setRltCode(ECODE_PACK_FAIL);
        return QRY_REC_PACK_FAIL;
    }
    
    if (m_oPayRes.m_uiRltCode != 0)
    {
        return 0;
    }

    for (int i=0; i<vData.size(); ++i) 
    {
        if (!m_poHPayCCA->addPay(vData[i])) 
        {
            __DEBUG_LOG0_(0, "QueryPayLogByAccout:: _deal m_poHPayCCA 打包查询结果失败!");
            m_poHPayCCA->setRltCode(ECODE_PACK_FAIL);
            return QRY_REC_PACKDATA_FAIL;
        }
    }
    return 0;
}

int HssQuery::dRechargeBillQuery(ABMCCR *poQueryMsg, ABMCCA *poQueryRlt)
{   // 电子账单查询
    int iRet;
    m_poHBilCCR = (HssRechargeBillQueryCCR *)poQueryMsg;
    m_poHBilCCA = (HssRechargeBillQueryCCA *)poQueryRlt;
    m_poHBilCCA->clear();
    g_vBil.clear();

    vector<RechargeBillQuery *>::iterator itvBil;

    //消息解包
    if ((iRet=m_poHBilCCR->getBill(g_vBil)) == 0) {
        __DEBUG_LOG0_(0, "getBill::deal 消息包为NULL");
        m_poHBilCCA->setRltCode(ECODE_NOMSG);
        return QRY_BIL_UNPACK_FAIL;
    }
    else if (iRet < 0) {
        __DEBUG_LOG0_(0, "getBill::deal 消息解包出错!");
        m_poHBilCCA->setRltCode(ECODE_UNPACK_FAIL);
        return QRY_BIL_UNPACK_FAIL;
    }
    if (iRet != 1)
    {
        __DEBUG_LOG0_(0, "getBill::deal 请求消息有多个!");
        m_poHBilCCA->setRltCode(ECODE_UNPACK_FAIL);
        return QRY_BIL_UNPACK_FAIL;
    }

    try {
        for (itvBil=g_vBil.begin(); itvBil!=g_vBil.end(); ++itvBil) 
        {
            return _dRechargeBillQuery(*itvBil);
        }
        return 0;
    }
    catch(TTStatus oSt) {
        __DEBUG_LOG1_(0, "getBill::deal oSt.err_msg=%s", oSt.err_msg);
    }
    m_poHBilCCA->clear();
    m_poHBilCCA->setRltCode(ECODE_TT_FAIL);
    return 0;	
}
int HssQuery::_dRechargeBillQuery(RechargeBillQuery *pIn)
{   // 电子账单查询处理
    
    Date Seq;
    /* 响应流水
    snprintf(m_oBilRes.m_sSeq, sizeof(m_oBilRes.m_sSeq), 
        "002%s%08d00\0", Seq.toString(),m_iCount++);
    cout<<pIn->m_sOperSeq<<endl;
    */
    
    m_oBilRes.m_uiRltCode = 0;
    ResRechargeBillQuery oData;
    vector<ResRechargeBillQuery> vData;
    vData.clear();
    Date cur;
    char strDateFrom[12];
    char strTmp[20];
    char sBilCurMonSql[1024];
    double damount = 0;
    cout<<pIn->m_uiBilCycID<<endl;
    snprintf(strDateFrom, sizeof(strDateFrom), "%d\0", pIn->m_uiBilCycID);
    
    if(strlen(strDateFrom)<6)
    {
    	strncpy(m_oBilRes.m_sSeq,pIn->m_sOperSeq,sizeof(m_oBilRes.m_sSeq));
    	__DEBUG_LOG1_(0, "响应流水,m_oPayRes.m_sSeq:[%s]", m_oBilRes.m_sSeq);
    	return HSS_QRY_BILLING_CYCLE_ERROR;
    }
    
    string date(strDateFrom);
    date[date.size()] = '\0';
    Date from(date.c_str(), "yyyymmdd");
    from.addMonth(1);
    string adddate(from.toString("yyyymmdd"));
    adddate[adddate.size()] = '\0';
    TimesTenCMD *pTmp = NULL;
    char sTableName[2][1024];
    memset(sTableName, '\0', sizeof(sTableName));
    snprintf(sTableName[0],sizeof(sTableName[0]), "ACCT_BALANCE_PHOTO_1%s\0", date.substr(2,4).c_str());
    if (strncmp(cur.toString(), date.c_str(), 6) == 0)
    {
        snprintf(sTableName[1],sizeof(sTableName[1]), "ACCT_BALANCE\0", sizeof("ACCT_BALANCE\0"));
    }
    else
    {
        snprintf(sTableName[1], sizeof(sTableName[1]), "ACCT_BALANCE_PHOTO_1%s\0", adddate.substr(2,4).c_str());
    }
    
     __DEBUG_LOG1_(0, "tables[0]:[%s]", sTableName[0]);
     __DEBUG_LOG1_(0, "tables[1]:[%s]", sTableName[1]);
    
    snprintf(sBilCurMonSql,sizeof(sBilCurMonSql), 
    
        "SELECT "
            //"X.SU,SUM(D.BALANCE) ,SUM(E.BALANCE)  "
            " X.SU,cast(SUM(D.BALANCE) AS  INT) BALANCE1 ,cast(SUM(E.BALANCE) AS INT) BALANCE2  "
        "FROM "
            "SERV B, "
            "%s D, "
            "%s E, "
            "SERV_ACCT G , "
            "("
            "SELECT SUM(A.AMOUNT) SU "
            "FROM "
            "SERV B, "
            "SERV_ACCT G , "
            "ACCT_BALANCE C, "
            "BALANCE_PAYOUT A "
            "WHERE "
            "B.ACC_NBR = ? AND "
            "B.SERV_ID = G.SERV_ID AND "
            "G.ACCT_ID = C.ACCT_ID AND "
            "C.ACCT_BALANCE_ID = A.ACCT_BALANCE_ID AND "
            "A.OPER_TYPE = \'5UI\' AND "
            "A.STATE = \'00A\' AND "
            "TO_CHAR(A.OPER_DATE,\'YYYYMM\') = ? "
            ") X "
        "WHERE "
            "B.ACC_NBR = ? AND "
            "B.SERV_ID = G.SERV_ID AND "
            "G.ACCT_ID = D.ACCT_ID AND "
            "G.ACCT_ID = E.ACCT_ID AND "
            "D.STATE='10A' AND " 
            "E.STATE='10A' "
            "GROUP BY X.SU ", sTableName[0], sTableName[1]);
    pTmp = m_poSql->m_poBil;
	try{
        pTmp->Drop();
        pTmp->Prepare(sBilCurMonSql);
        pTmp->Commit();
	}
	catch(TTStatus tt)
	{
		__DEBUG_LOG1_(0, "m_poBil::Prepare oSt.err_msg=%s", tt.err_msg);
        __DEBUG_LOG1_(0, "m_poBil::Prepare err_code=%d", QRY_BIL_PREPARE_FAIL);
		return QRY_BIL_PREPARE_FAIL;
	}
	
    pTmp->setParam(1, pIn->m_sDestAcct);
    cout<<pIn->m_sDestAcct<<endl;
    memset(strTmp, '\0', sizeof(strTmp));
    //snprintf(strTmp,sizeof(strTmp), "%d\0", pIn->m_iDestType);
    //pTmp->setParam(2, strTmp);
    //cout<<strTmp<<endl;
    //snprintf(strTmp,sizeof(strTmp), "%d\0", pIn->m_iDestAttr);
    //pTmp->setParam(3, strTmp);
    //cout<<strTmp<<endl;
    snprintf(strTmp,sizeof(strTmp), "%s\0", date.substr(0,6).c_str());
    pTmp->setParam(2, strTmp);
    cout<<strTmp<<endl;
    pTmp->setParam(3, pIn->m_sDestAcct);
    cout<<pIn->m_sDestAcct<<endl;
    //snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_iDestType);
    //pTmp->setParam(6, strTmp);
    //cout<<strTmp<<endl;
    //snprintf(strTmp, sizeof(strTmp), "%d\0", pIn->m_iDestAttr);
    //pTmp->setParam(7, strTmp);
    //cout<<strTmp<<endl;
    
     // add 2011.7.11
     __DEBUG_LOG0_(0, "dRechargeBillQuery:: 电子账单查询条件:");
     __DEBUG_LOG1_(0, "用户号码,m_sDestAcct:[%s]", pIn->m_sDestAcct);
     __DEBUG_LOG1_(0, "账期,strTmp:[%s]", strTmp);

    pTmp->Execute();
    while (!pTmp->FetchNext()) {
        memset((void *)&oData, 0x00, sizeof(oData));
        pTmp->getColumn(1, &damount);
        oData.m_lPayMount = (long)damount;
        pTmp->getColumn(2, &oData.m_iBalance);
        pTmp->getColumn(3, &oData.m_iLastBalance);
        
        __DEBUG_LOG0_(0, "dRechargeBillQuery:: 返回响应的信息:");
        __DEBUG_LOG1_(0, "当月帐户总支出,oData.m_lPayMount:[%ld]", oData.m_lPayMount);
        __DEBUG_LOG1_(0, "本次余额,soData.m_iBalance:[%d]", oData.m_iBalance);
        __DEBUG_LOG1_(0, "上次余额,oData.m_iLastBalance:[%d]", oData.m_iLastBalance);
        
        vData.push_back(oData);
    }
    pTmp->Close();

    if (vData.size() == 0 && vData.size() != 1)
    {
        __DEBUG_LOG0_(0, "RechargeBillQuery:: _deal m_poHBilCCA 电子账单查询失败!");
        m_oBilRes.m_uiRltCode = QRY_BIL_NORECORD_ERR;
    }

    strncpy(m_oBilRes.m_sSeq,pIn->m_sOperSeq,sizeof(m_oBilRes.m_sSeq));
    __DEBUG_LOG1_(0, "响应流水,m_oPayRes.m_sSeq:[%s]", m_oBilRes.m_sSeq);
    if (!m_poHBilCCA->addRespons(m_oBilRes)) {
        __DEBUG_LOG0_(0, "RechargeBillQuery:: _deal m_poHBilCCA 打包业务结果失败!");
        m_poHBilCCA->setRltCode(ECODE_PACK_FAIL);
        return QRY_BIL_PACK_FAIL;
    }

    if (m_oBilRes.m_uiRltCode != 0)
    {
        return 0;
    }

    for (int i=0; i<vData.size(); ++i) 
    {
        if (!m_poHBilCCA->addBil(vData[i]))
        {
            __DEBUG_LOG0_(0, "RechargeBillQuery:: _deal m_poHBilCCA 打包查询结果失败!");
            m_poHBilCCA->setRltCode(ECODE_PACK_FAIL);
            return QRY_BIL_PACKDATA_FAIL;
        }
    }
    return 0;
}

void HssQuery::dTestQuery()
{
    TimesTenCMD * tTest = NULL;
    tTest = m_poSql->m_poTest;
    tTest->Execute();
    char user_id[40];
    if (!tTest->FetchNext())
    {
        tTest->getColumn(1,user_id);
    }
    tTest->Close();
    cout<<"simple test:acc_nbr:"<<user_id<<endl;
    return ;
}

unsigned int HssQuery::compute(char *str)
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
    if(atoi(strdate) == 1970)
    {
    t.tm_year = 0;
    return 2209017599;
    }else
    t.tm_year = atoi (strdate) - 1900;
    return (unsigned int)mktime(&t) + 2209017599;
}

int HssQuery::isNumberStr(char *numStr)
{
    char *strTmp = numStr;
    for (int i=0;i<strlen(numStr);++i)
    {
        if (isdigit(strTmp[i]) != 0)
            return -1;
    }
    return 0;
}