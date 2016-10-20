/*VER: 3*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "FormatMethod.h"
#include "EventSection.h"
#include "LongTypeMgr.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

extern char g_sFormatRecord[MAX_FORMAT_RECORD_LEN];


//##ModelId=4279B41E02B4
KeyTree<Trunk>* FormatMethod::m_poTrunkTree = NULL;

KeyTree<TServiceInfo2>* FormatMethod::m_poServiceInfo = NULL;

LocalHeadMgr* FormatMethod::m_poLocalHeadMgr = NULL;

//##ModelId=4279D0650064
KeyTree<int>* FormatMethod::m_pAreaCode = NULL;

//##ModelId=4279D44A02E9
KeyTree<HLR>* FormatMethod::m_pHLR = NULL;

//##ModelId=4279E935022A
KeyTree<int>* FormatMethod::m_pCarrier = NULL;

//##ModelId=427B3DF0029F
TServiceInfo* FormatMethod::m_aServiceInfo = NULL;

PNSegBilling* FormatMethod::m_aPNSegBilling = NULL;

//##ModelId=427B0F7A0057
int FormatMethod::m_iMaxPreCodeLen;

//##ModelId=427B0F990265
int FormatMethod::m_iMinPreCodeLen;

//##ModelId=427B3E34026B
int FormatMethod::m_iMaxAreaCodeLen;

//##ModelId=427B3E4802E1
int FormatMethod::m_iMinAreaCodeLen;

//##ModelId=427C2957029D
int FormatMethod::m_iMaxSegCodeLen;

//##ModelId=427C29730095
int FormatMethod::m_iMinSegCodeLen;
//新增参数 临时记录的参数
unsigned int FormatMethod::iMaxPreCodeLen = 0;
unsigned int FormatMethod::iMinPreCodeLen = 0;
unsigned int FormatMethod::iMaxAreaCodeLen = 0;
unsigned int FormatMethod::iMinAreaCodeLen = 0;
unsigned int FormatMethod::iMaxSegCodeLen = 0;
unsigned int FormatMethod::iMinSegCodeLen = 0;

//##ModelId=427C5AD6010C
TSwitchInfo* FormatMethod::m_aSwitchInfo = NULL;

//##ModelId=427C5BD00274
KeyTree<int>* FormatMethod::m_pSwitchInfoPosTree = NULL;

KeyTree<int>* FormatMethod::m_pSwitchOPCInfoTree = NULL;


//跳次    
KeyTree<int>* FormatMethod::m_poTimesType = NULL;

//##ModelId=427C71EC0042
int FormatMethod::m_iServiceInfoCnt;

//##ModelId=427C727F00E3
int FormatMethod::m_iSwitchInfoCnt;

int FormatMethod::m_iPNSegBillingCnt;

OrgMgr * FormatMethod::m_poOrgMgr = NULL;

int FormatMethod::m_bAreaCodeCycleFit = 0;

int FormatMethod::m_iSelAreaPart = 0;

KeyTree<VisitAreaCodeRule>* FormatMethod::m_pVisitAreaCode = NULL;

KeyTree<EdgeRoamRule>* FormatMethod::m_pEdgeRoam = NULL;


FilterRule * FormatMethod::m_poFilterRule = NULL;

CustomFilter * FormatMethod::m_poCustomFilter=NULL;

KeyTree<SwitchItemType>* FormatMethod::m_poSwitchItemTypeTree = NULL;

vector<string> FormatMethod::m_strLocalAreaCode;
    
KeyTree<TrunkBilling>* FormatMethod::m_poTrunkBilling = NULL;   
    

int FormatMethod::m_iCallingOrgIDRule = 0;

int FormatMethod::m_iCalledOrgIDRule = 0;
    
int FormatMethod::m_iTrimMobHeadZero = 0;



//##ModelId=427986E40143
FormatMethod::FormatMethod(bool _InitAll)
{
    m_poTimesType = NULL;
    m_poTrunkTree = NULL;
    m_pAreaCode = NULL;
    m_pHLR = NULL;
    m_pCarrier = NULL;
    m_aServiceInfo = NULL;
    m_aPNSegBilling = NULL;
    m_aSwitchInfo = NULL;
    m_pSwitchInfoPosTree = NULL;
    m_poSwitchItemTypeTree = NULL;
    m_poTrunkBilling = NULL;

    m_poCustomFilter=NULL;

    if (!_InitAll) {        
        return;
    }

    loadTrunk();
    loadAreaCode();
    loadHLR();
    loadCarrier();
    //loadServiceInfo();
    loadServiceInfo2();
    loadSwitchInfo();
    
    loadTrunkBilling();
    loadTimesType();

}

//##ModelId=427986FF0083
FormatMethod::~FormatMethod()
{
    unloadTrunk();
    unloadAreaCode();
    unloadHLR();
    unloadCarrier();
    //unloadServiceInfo();
    unloadServiceInfo2();
    unloadSwitchInfo();
    unloadPNSegBilling();
    unloadTimesType();

    if (m_poCustomFilter) {
        m_poCustomFilter->unload ();
        delete m_poCustomFilter;
        m_poCustomFilter=NULL;
    }

    
    if (m_poLocalHeadMgr)
        delete m_poLocalHeadMgr;
    
    if (m_poOrgMgr)
        delete m_poOrgMgr;
    
}

//##ModelId=4279B2710389
void FormatMethod::loadTrunk()
{
    unloadTrunk();
    
    m_poTrunkTree = new KeyTree<Trunk>;
    if (!m_poTrunkTree)
        THROW(MBC_FormatMethod+1);
    
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL(" select switch_id||'+'||trunk_code||'+'||'0' skey, "
        " trunk_type_id, to_number(need_billing) need_billing "
        " from b_trunk  where trunk_side <> '1' "
        "     union  "
        " select switch_id||'+'||trunk_code||'+'||'1' skey,  "
        " trunk_type_id, to_number(need_billing) need_billing  "   
        " from b_trunk where trunk_side <> '0' ");
    qry.open();
    
    while (qry.next()) {
        Trunk oTrunk;
        char sTemp[30];
        oTrunk.m_iTrunkTypeID = qry.field("trunk_type_id").asInteger();
        //strncpy(sTemp,qry.field("need_billing").asString(),2);
        //oTrunk.m_cNeedBilling = sTemp[0];
        oTrunk.m_iNeedBilling = qry.field("need_billing").asInteger();
        strcpy(sTemp,qry.field("skey").asString());

        m_poTrunkTree->add(sTemp,oTrunk);
    }
    
    qry.close();
}

//##ModelId=4279B96B0387
void FormatMethod::unloadTrunk()
{
    if (m_poTrunkTree) {
        delete m_poTrunkTree;
        m_poTrunkTree = NULL;
    }
}


void FormatMethod::loadServiceInfo2()
{
    unloadServiceInfo2();
    
    m_poServiceInfo = new KeyTree<TServiceInfo2>;
    if (!m_poServiceInfo)
        THROW(MBC_FormatMethod+2);
    
    DEFINE_QUERY(qry);
    
    qry.setSQL("select service_type_id,party_role_id,org_id,to_char(org_id) s_org_id from b_sp_service");
    qry.open();
    
    while (qry.next()) {
        TServiceInfo2 oSvcInfo;
        char sOrgIDKey[10];
        oSvcInfo.m_iOrgID = qry.field("org_id").asInteger();
        oSvcInfo.m_iPartyRoleID = qry.field("party_role_id").asInteger();
        oSvcInfo.m_iServiceTypeID = qry.field("service_type_id").asInteger();
        strcpy(sOrgIDKey,qry.field("s_org_id").asString());
        
        m_poServiceInfo->add(sOrgIDKey,oSvcInfo);
    }
    qry.close();
    
    
    char sTempValue[10];
    
    m_iCallingOrgIDRule = 0;
    m_iCalledOrgIDRule = 0;
    
    memset (sTempValue, 0, 10);
    if (ParamDefineMgr::getParam ("FORMAT_METHOD","CALLING_ORG_ID_RULE", sTempValue) )
        m_iCallingOrgIDRule = atoi (sTempValue);
    
    memset (sTempValue, 0, 10);
    if (ParamDefineMgr::getParam ("FORMAT_METHOD","CALLED_ORG_ID_RULE", sTempValue) )
        m_iCalledOrgIDRule = atoi (sTempValue);
}

void FormatMethod::unloadServiceInfo2()
{
    if (m_poServiceInfo) {
        delete m_poServiceInfo;
        m_poServiceInfo = NULL;
    }
}



//##ModelId=4279B29B011D
#ifdef PRIVATE_MEMORY
bool FormatMethod::getTrunkByKey(char *skey, Trunk *pTrunk)
{
    if (!pTrunk) return false;
    
    if (!m_poTrunkTree) //return false;
        loadTrunk();
    
    if (m_poTrunkTree->get(skey,pTrunk))
        return true;
    
    return false;
}
#else
bool FormatMethod::getTrunkByKey(char *skey, Trunk *pTrunk)
{
    if (!pTrunk) 
        return false;

    unsigned int iIdx = 0;

	if(!(G_PPARAMINFO->m_poTrunkIndex->get(skey, &iIdx)))
        return false;
    else
    {
		*pTrunk = *(G_PPARAMINFO->m_poTrunkList + iIdx);
        return true;
    }
}
#endif


//##ModelId=4279B60802E0

bool FormatMethod::getTrunk(int _iSwitchID, char *_sTrunkCode, char _cSide, Trunk* _pTrunk)
{
    static char sKey[100];
    
    sprintf(sKey,"%d+%s+%c",_iSwitchID,_sTrunkCode,_cSide);
    if (getTrunkByKey(sKey,_pTrunk))
        return true;

/*    
    //双向中继
    sprintf(sKey,"%d+%s+2",_iSwitchID,_sTrunkCode);
    if (getTrunkByKey(sKey,_pTrunk))
        return true;
*/    
    return false;
}


//##ModelId=4279D21D0099
void FormatMethod::loadAreaCode()
{
    unloadAreaCode();
    
    m_pAreaCode = new KeyTree<int>;
    if (!m_pAreaCode)
        THROW(MBC_FormatMethod+3);

    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL("select area_code,long_group_type_id from b_area_code");
    qry.open();
    
    while (qry.next()) {
        m_pAreaCode->add(qry.field("area_code").asString(),qry.field("long_group_type_id").asInteger());
    }

    qry.close();
    qry.setSQL("select max(length(area_code)) max_len, min(length(area_code)) min_len  from b_area_code");
    qry.open();
    if (qry.next()) {
        m_iMaxAreaCodeLen = qry.field("max_len").asInteger();
        m_iMinAreaCodeLen = qry.field("min_len").asInteger();
    }
    else {
        m_iMaxAreaCodeLen = 0;
        m_iMinAreaCodeLen = 0;
    }
    
    qry.close();
    
    //## 取得区号匹配规则信息(是否循环匹配、循环区号的选择)
    char sValue[1000];
    sValue[0] = 0;
    ParamDefineMgr::getParam("PRE_PROCESS","AREACODE_CYC_FIT",sValue);
    m_bAreaCodeCycleFit = atoi(sValue);
    
    sValue[0] = 0;
    ParamDefineMgr::getParam("PRE_PROCESS","SEL_AREA_PART",sValue);
    m_iSelAreaPart = atoi(sValue);
    
    sValue[0] = 0;
    ParamDefineMgr::getParam("FORMAT_METHOD","TRIM_MOBILE_HEAD_ZERO", sValue);
    m_iTrimMobHeadZero = atoi(sValue);    
}

//##ModelId=4279D235031E
void FormatMethod::unloadAreaCode()
{
    if (m_pAreaCode) {
        delete m_pAreaCode;
        m_pAreaCode = NULL;
    }
}

//##ModelId=4279D24E00C1
#ifdef PRIVATE_MEMORY
bool FormatMethod::getAreaCodeInfo(char* _sAreaCode, int &_iLongGroupType)
{
    if (!m_pAreaCode)
        loadAreaCode();
    
    int bRet = m_pAreaCode->get(_sAreaCode,&_iLongGroupType);
    
    if (bRet) return true;
    
    return false;
}
#else
bool FormatMethod::getAreaCodeInfo(char* _sAreaCode, int &_iLongGroupType)
{
	unsigned int iVal = 0;
    if(!(G_PPARAMINFO->m_poAreaCodeIndex->get(_sAreaCode, &iVal)))
	{
        return false;    
    } else{
		_iLongGroupType = (int)iVal;
        return true;
	}
}
#endif

//##ModelId=4279D2A2019E
void FormatMethod::loadHLR()
{
    unloadHLR();
    
    m_pHLR = new KeyTree<HLR>;
    if (!m_pHLR) THROW(MBC_FormatMethod+4);
    
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL("select code_segment,area_code from b_hlr");
    qry.open();
    
    while (qry.next()) {
        HLR oHlr;
        strcpy(oHlr.m_sAreaCode, qry.field("area_code").asString());
        m_pHLR->add(qry.field("code_segment").asString(), oHlr);
    }

    qry.close();
    qry.setSQL("select max(length(code_segment)) max_len,min(length(code_segment)) min_len from b_hlr");
    qry.open();
    if (qry.next()) {
        m_iMaxSegCodeLen = qry.field("max_len").asInteger();
        m_iMinSegCodeLen = qry.field("min_len").asInteger();
    }
    else {
        m_iMaxSegCodeLen = 0;
        m_iMinSegCodeLen = 0;
    }
    
    qry.close();
}

//##ModelId=4279D2DA0217
void FormatMethod::unloadHLR()
{
    if (m_pHLR) {
        delete m_pHLR;
        m_pHLR = NULL;
    }
}


void FormatMethod::loadVisitAreaCode()
{
    unloadVisitAreaCode();
    m_pVisitAreaCode = new KeyTree<VisitAreaCodeRule>;
    if (!m_pVisitAreaCode)
        THROW(MBC_FormatMethod+12);
    
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL("select trim(msc)||decode(lac,null,'',('-'||trim(lac)||decode(cell_id,null,'','-'||trim(cell_id)))) sKey, "
        " trim(area_code) area_code from b_visit_area_code");
    qry.open();
    
    while (qry.next()) {
        VisitAreaCodeRule oCodeRule;
        strcpy(oCodeRule.m_sAreaCode, qry.field("area_code").asString());
        m_pVisitAreaCode->add(qry.field("sKey").asString(), oCodeRule);
    }
    
    qry.close();
}

void FormatMethod::unloadVisitAreaCode()
{
    if (m_pVisitAreaCode) {
        delete m_pVisitAreaCode;
        m_pVisitAreaCode = NULL;
    }
}

//## 根据 _sKey,找到对应的 VisitAreaCodeRule 结构
#ifdef PRIVATE_MEMORY
bool FormatMethod::getVisitAreaCodeRule(char* _sKey, VisitAreaCodeRule *_pRule)
{
    if (!m_pVisitAreaCode)
        loadVisitAreaCode();
    
    return m_pVisitAreaCode->get(_sKey,_pRule);
}
#else
bool FormatMethod::getVisitAreaCodeRule(char* _sKey, VisitAreaCodeRule *_pRule)
{
	unsigned int iIdx = 0;
    if(G_PPARAMINFO->m_pVisitAreaCodeIndex->get(_sKey,&iIdx))
	{
		*_pRule = *(G_PPARAMINFO->m_poVisitAreaCodeRuleList + iIdx);
		return true;
	} else {
		return false;
	}
}
#endif

//## 根据 _sMSC,_sLAC,_sCellID , 查找相应的 VisitAreaCodeRule
//## 找到,置相应的到访地区号_sVisitAreaCode; 找不到,不作任何改变
bool FormatMethod::getVisitAreaCodeByMSC(char* _sMSC, char* _sLAC, char* _sCellID,
        char *_sVisitAreaCode)
{
    static char s_sKey[40];
    static VisitAreaCodeRule s_oCodeRule;
    
    sprintf(s_sKey,"%s-%s-%s", _sMSC, _sLAC, _sCellID);
    
    if (getVisitAreaCodeRule(s_sKey, &s_oCodeRule)) {
        strcpy(_sVisitAreaCode, s_oCodeRule.m_sAreaCode);
        return true;
    }
    
    sprintf(s_sKey,"%s-%s", _sMSC, _sLAC);
    if (getVisitAreaCodeRule(s_sKey, &s_oCodeRule)) {
        strcpy(_sVisitAreaCode, s_oCodeRule.m_sAreaCode);
        return true;
    }
        
    sprintf(s_sKey,"%s", _sMSC);
    if (getVisitAreaCodeRule(s_sKey, &s_oCodeRule)) {
        strcpy(_sVisitAreaCode, s_oCodeRule.m_sAreaCode);
        return true;
    }
    
    //模糊匹配MSC
    char sTempMSC[30];
    int iLen = strlen(_sMSC)-1;
    strcpy(sTempMSC,_sMSC);
    while (iLen>0) {
        sTempMSC[iLen] = 0;
        if (getVisitAreaCodeRule(sTempMSC, &s_oCodeRule)){
            strcpy(_sVisitAreaCode, s_oCodeRule.m_sAreaCode);
            return true;
        }
        iLen--;
    }
    
    return false;
}

void FormatMethod::loadEdgeRoam()
{
    unloadEdgeRoam();
    m_pEdgeRoam = new KeyTree<EdgeRoamRule>;
    if (!m_pEdgeRoam)
        THROW(MBC_FormatMethod+13);
    
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL("select trim(msc)||decode(lac,null,'',('-'||trim(lac)||decode(cell_id,null,'','-'||trim(cell_id)))) sKey, "
        " trim(area_code) area_code from b_edge_roam");
    qry.open();
    
    while (qry.next()) {
        EdgeRoamRule oCodeRule;
        strcpy(oCodeRule.m_sAreaCode, qry.field("area_code").asString());
        m_pEdgeRoam->add(qry.field("sKey").asString(), oCodeRule);
    }
    
    qry.close();
}

void FormatMethod::unloadEdgeRoam()
{
    if (m_pEdgeRoam) {
        delete m_pEdgeRoam;
        m_pEdgeRoam = NULL;
    }
}

//## 根据 _sKey,找到对应的 EdgeRoamRule 结构
#ifdef PRIVATE_MEMORY
bool FormatMethod::getEdgeRoamRule(char* _sKey, EdgeRoamRule *_pRule)
{
    if (!m_pEdgeRoam)
        loadEdgeRoam();
    
    return m_pEdgeRoam->get(_sKey,_pRule);
}
#else
bool FormatMethod::getEdgeRoamRule(char* _sKey, EdgeRoamRule *_pRule)
{
	unsigned int iIdx = 0;
    if(G_PPARAMINFO->m_pEdgeRoamIndex->get(_sKey,&iIdx))
	{
		*_pRule = *(G_PPARAMINFO->m_poEdgeRoamRuleList + iIdx);
		return true;
	} else {
		_pRule = 0;
		return false;
	}
}
#endif

//## 根据 _sMSC,_sLAC,_sCellID , 查找相应的 EdgeRoamRule
//## 找到,置相应的边界漫游地区号_sEdgeRoam; 找不到,不作任何改变
bool FormatMethod::getEdgeRoamByMSC(char* _sMSC, char* _sLAC, char* _sCellID,
        char *_sEdgeRoam)
{
    static char s_sKey[40];
    static EdgeRoamRule s_oRoamRule;
    
    sprintf(s_sKey,"%s-%s-%s", _sMSC, _sLAC, _sCellID);
    if (getEdgeRoamRule(s_sKey, &s_oRoamRule)) {
        strcpy(_sEdgeRoam, s_oRoamRule.m_sAreaCode);
        return true;
    }

    sprintf(s_sKey,"%s-%s", _sMSC, _sLAC);
    if (getEdgeRoamRule(s_sKey, &s_oRoamRule)) {
        strcpy(_sEdgeRoam, s_oRoamRule.m_sAreaCode);
        return true;
    }

    sprintf(s_sKey,"%s", _sMSC);
    if (getEdgeRoamRule(s_sKey, &s_oRoamRule)) {
        strcpy(_sEdgeRoam, s_oRoamRule.m_sAreaCode);
        return true;
    }
    
    return false;
}

void FormatMethod::getVisitAreaCodeOf3G(EventSection *_poES)
{
    //首先求取到访地信息
    char sHcode[20];
    //对于MOC话单:
    //主叫号码通过网络信息求取到访地区号；
    //被叫号码如果有动态漫游号（一般MOC即主叫话单才有动态漫游号），通过调用动态漫游号取到访地区号函数,如果动态漫游号为空，那么将被叫号码的归属地区号放到被叫号码的到访地区号上;
    //同时把主叫号码到访地区号置给计费号码的到访地区号。    
    if(CALLTYPE_MOC == _poES->m_iCallTypeID)
    {
        if (!getVisitAreaCodeByMSC(_poES->m_sMSC, _poES->m_sLAC, _poES->m_sCellID,
            _poES->m_oEventExt.m_sCallingVisitAreaCode))
        {    
            //"找不到" 并且 "之前也未预置" 计费方到访地区号 则置为错单 
            if (0 == _poES->m_oEventExt.m_sCallingVisitAreaCode[0]) 
            {
                _poES->m_iErrorID = 1800;
                return;
            }
        }
        
        if(0 != _poES->m_sRomingNBR[0])
        {
            if (!getHLRInfoByMobileNBR(_poES->m_sRomingNBR,_poES->m_oEventExt.m_sCalledVisitAreaCode,sHcode))
            {    
                //"找不到" 并且 "之前也未预置" 计费方到访地区号 则置为错单
                if (0 == _poES->m_oEventExt.m_sCalledVisitAreaCode[0]) 
                {
                    _poES->m_iErrorID = 1801;
                    return;
                }
            }
        }
        else
        {
            strcpy(_poES->m_oEventExt.m_sCalledVisitAreaCode,_poES->m_sCalledAreaCode);
        }
        
        strcpy(_poES->m_sBillingVisitAreaCode,_poES->m_oEventExt.m_sCallingVisitAreaCode); 
    }
    //对于MTC话单
    //被叫号码通过网络信息求取到访地区号;
    //主叫号码的到访地区号就取主叫号码的归属地区号。同时把被叫号码到访地区号置给计费号码的到访地区号。
    else if(CALLTYPE_MTC == _poES->m_iCallTypeID)  
    {
        if (!getVisitAreaCodeByMSC(_poES->m_sMSC, _poES->m_sLAC, _poES->m_sCellID,
            _poES->m_oEventExt.m_sCalledVisitAreaCode))
        {    
            //"找不到" 并且 "之前也未预置" 计费方到访地区号 则置为错单 
            if (0 == _poES->m_oEventExt.m_sCalledVisitAreaCode[0]) 
            {
                _poES->m_iErrorID = 1802;
                return;
            }
        }
        strcpy(_poES->m_oEventExt.m_sCallingVisitAreaCode,_poES->m_sCallingAreaCode); 
        strcpy(_poES->m_sBillingVisitAreaCode,_poES->m_oEventExt.m_sCalledVisitAreaCode); 
    }
    //呼转话单
    //对于呼转话单，分为主叫号码A、呼转号码B和呼转目标号码C。
    //话单中主叫号码放的是A，被叫号码放的是C，第三方号码放的是B
    //主叫号码的到访地区号就取归属地区号；
    //呼转号码作为第三方号码，同时是计费号码，呼转号码通过网络信息求取到访地区号；
    //话单中有呼转目标号码的动态漫游号码，通过调用动态漫游号取到访地区号函数，如果动态漫游号为空，那么将呼转目标号码的归属地区号放到呼转号码的到访地区号上（目前系统没有呼转目标号码的到访地区号这个字段，需要增加）。
    //同时把呼转号码到访地区号置给计费号码的到访地区号。
    else if(CALLTYPE_FORWARD == _poES->m_iCallTypeID)    
    {
        strcpy(_poES->m_oEventExt.m_sCallingVisitAreaCode,_poES->m_sCallingAreaCode);
        //特别说明，目前由于不想修改StdEvent的结构，所以暂时用m_oEventExt.m_sFileName字段放置第三方号码的到访地区号
        if (!getVisitAreaCodeByMSC(_poES->m_sMSC, _poES->m_sLAC, _poES->m_sCellID,
            _poES->m_oEventExt.m_sFileName))
        {    
            //"找不到" 并且 "之前也未预置" 计费方到访地区号 则置为错单 
            if (0 == _poES->m_oEventExt.m_sFileName[0]) 
            {
                _poES->m_iErrorID = 1802;
                return;
            }
        }
        strcpy(_poES->m_sBillingVisitAreaCode,_poES->m_oEventExt.m_sFileName); 
        //呼转目标号码作为被叫号码
        if(0 != _poES->m_sRomingNBR[0])
        {
            if (!getHLRInfoByMobileNBR(_poES->m_sRomingNBR,_poES->m_oEventExt.m_sCalledVisitAreaCode,sHcode))
            {    
                //"找不到" 并且 "之前也未预置" 计费方到访地区号 则置为错单
                if (0 == _poES->m_oEventExt.m_sCalledVisitAreaCode[0]) 
                {
                    _poES->m_iErrorID = 1801;
                    return;
                }
            }
        }
        else
        {
            strcpy(_poES->m_oEventExt.m_sCalledVisitAreaCode,_poES->m_sCalledAreaCode);
        }
    }
}

void FormatMethod::judgeEdgeRoamOf3G(EventSection *_poES)
{
    //判断边界漫游
    //计费方号码通过MSC、LAC 和CellID来得到边界交换机覆盖地区区号（AREA_CODE），
    //求取方法是先将话单中MSC、LAC 和CellID3个字段带到这张表去求取，
    //如果求不到，就将MSC、LAC 和空字段带到这张表去求取，
    //如果还是求不到，那么将MSC、空字段和空字段带进去去求取。
    //如果还是求不到，说明此交换机不涉及边界漫游的情况。
    //如果求到有覆盖，那么将漫游类型置成边界漫游，同时将计费到访地区号置成边界交换机覆盖地区区号，
    //针对MOC话单把主叫到访地区号置成边界交换机覆盖地区区号，
    //针对MTC话单把被叫到访地区号置成边界交换机覆盖地区区号。
    char sRoamArea[20];
    if (getEdgeRoamByMSC(_poES->m_sMSC, _poES->m_sLAC, _poES->m_sCellID, sRoamArea)) 
    {
        if(CALLTYPE_MOC == _poES->m_iCallTypeID)
        {
            //如果边界漫游覆盖地跟主叫归属地一样，那么将主叫到访地置成主叫归属地
            if(!strcmp(sRoamArea,_poES->m_sCallingAreaCode))
            {
                strcpy(_poES->m_oEventExt.m_sCallingVisitAreaCode,_poES->m_sCallingAreaCode);
                strcpy(_poES->m_sBillingVisitAreaCode,_poES->m_oEventExt.m_sCallingVisitAreaCode); 
                //置边界漫游类型
                _poES->m_oEventExt.m_iRoamTypeID = ROAMTYPE_ROAM_BORD;
            }
        }
        else if(CALLTYPE_MTC == _poES->m_iCallTypeID)
        {
            //如果边界漫游覆盖地跟被叫归属地一样，那么将被叫到访地区号置成被叫归属地区号
            if(!strcmp(sRoamArea,_poES->m_sCalledAreaCode))
            {
                strcpy(_poES->m_oEventExt.m_sCalledVisitAreaCode,_poES->m_sCalledAreaCode);
                strcpy(_poES->m_sBillingVisitAreaCode,_poES->m_oEventExt.m_sCalledVisitAreaCode); 
                //置边界漫游类型
                _poES->m_oEventExt.m_iRoamTypeID = ROAMTYPE_ROAM_BORD;
            }
        }
        else if(CALLTYPE_FORWARD == _poES->m_iCallTypeID)
        {
            //如果边界漫游覆盖地跟第三方归属地一样，那么将第三方到访地区号置成被叫归属地区号
            if(!strcmp(sRoamArea,_poES->m_sThirdPartyAreaCode))
            {
                strcpy(_poES->m_oEventExt.m_sFileName,_poES->m_sThirdPartyAreaCode);
                strcpy(_poES->m_sBillingVisitAreaCode,_poES->m_oEventExt.m_sFileName); 
                //置边界漫游类型
                _poES->m_oEventExt.m_iRoamTypeID = ROAMTYPE_ROAM_BORD;
            }
        }
    }
}

void FormatMethod::getRoamTypeOf3G(EventSection *_poES)
{
    int iLongGroupTypeHome,iLongGroupTypeVisit;
    
    //漫游类型判断
    //如果计费号码的归属地区号跟计费号码的到访地区号相同，那么漫游类型为0。
    if(!strcmp(_poES->m_sBillingVisitAreaCode,_poES->m_sBillingAreaCode))
    {
        _poES->m_oEventExt.m_iRoamTypeID = ROAMTYPE_NOT_ROAM;
    }
    //如果计费号码的归属地区号跟计费号码的到访地区号不相同，则置不同的漫游类型
    else
    {
        getAreaCodeInfo(_poES->m_sBillingAreaCode, iLongGroupTypeHome);
        getAreaCodeInfo(_poES->m_sBillingVisitAreaCode, iLongGroupTypeVisit);
        //如果计费号码的归属地区号的长途组类型是省内，到访地区号的长途组类型是省内，
        //那么漫游类型为省内漫游；
        if((iLongGroupTypeHome == LONG_GROUPTYPE_PROV)&&(iLongGroupTypeVisit == LONG_GROUPTYPE_PROV))
        {
            _poES->m_oEventExt.m_iRoamTypeID = ROAMTYPE_ROAM_PROV;
        }
        //如果计费号码的归属地区号的长途组类型是省内，到访地区号的长途组类型是国内,
        //那么漫游类型为国内漫出；
        else if((iLongGroupTypeHome == LONG_GROUPTYPE_PROV)&&(iLongGroupTypeVisit == LONG_GROUPTYPE_NATION))
        {
            _poES->m_oEventExt.m_iRoamTypeID = ROAMTYPE_ROAM_NATION_OUT;
        }
        //如果计费号码的归属地区号的长途组类型是国内，到访地区号的长途组类型是省内，
        //那么漫游类型为国内漫入；
        else if((iLongGroupTypeHome == LONG_GROUPTYPE_NATION)&&(iLongGroupTypeVisit == LONG_GROUPTYPE_PROV))
        {
            _poES->m_oEventExt.m_iRoamTypeID = ROAMTYPE_ROAM_NATION_IN;
        }
        //如果计费号码的归属地区号的长途组类型是国际，到访地区号的长途组类型是省内，
        //那么漫游类型为国际漫入
        else if
        (
              (
                  (iLongGroupTypeHome == LONG_GROUPTYPE_INTER)||
                (iLongGroupTypeHome == LONG_GROUPTYPE_THIRT)||
                 (iLongGroupTypeHome == LONG_GROUPTYPE_AMERC)||
                 (iLongGroupTypeHome == LONG_GROUPTYPE_KONG)||
                 (iLongGroupTypeHome == LONG_GROUPTYPE_XK)||
                 (iLongGroupTypeHome == LONG_GROUPTYPE_AM)||
                 (iLongGroupTypeHome == LONG_GROUPTYPE_TW)||
                 (iLongGroupTypeHome == LONG_GROUPTYPE_HS)
              )
              &&
              (iLongGroupTypeVisit == LONG_GROUPTYPE_PROV)
        )
        {
            _poES->m_oEventExt.m_iRoamTypeID = ROAMTYPE_ROAM_NATION_IN;
        }
        //如果计费号码的归属地区号的长途组类型是省内，到访地区号的长途组类型是国际，
        //那么漫游类型为国际漫出
        else if
        (
            (iLongGroupTypeHome == LONG_GROUPTYPE_PROV)
            &&
            (
                (iLongGroupTypeVisit == LONG_GROUPTYPE_THIRT)||
                 (iLongGroupTypeVisit == LONG_GROUPTYPE_AMERC)||
                 (iLongGroupTypeVisit == LONG_GROUPTYPE_KONG)||
                 (iLongGroupTypeVisit == LONG_GROUPTYPE_XK)||
                 (iLongGroupTypeVisit == LONG_GROUPTYPE_AM)||
                 (iLongGroupTypeVisit == LONG_GROUPTYPE_TW)||
                 (iLongGroupTypeVisit == LONG_GROUPTYPE_HS)
             )    
        )
        {
            _poES->m_oEventExt.m_iRoamTypeID = ROAMTYPE_ROAM_NATION_OUT;
        }
    }  
    
    judgeEdgeRoamOf3G(_poES);
    return;
    
}

void FormatMethod::getMOCLongTypeOf3G(EventSection *_poES)
{
    //首先设置默认值
    _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN; 
    
    //判断是否同城
    //判断主叫号码的拜访地和被叫号码的拜访地相同，包括没有漫游和同城漫游的情况
    if(!strcmp(_poES->m_oEventExt.m_sCalledVisitAreaCode,_poES->m_oEventExt.m_sCallingVisitAreaCode))
    {
        _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN; 
        return;
    }
    
    //判断是否特殊同城漫游
    if
    (    //判断主叫归属地和被叫到访地相同
        (!strcmp(_poES->m_sCallingAreaCode,_poES->m_oEventExt.m_sCalledVisitAreaCode))
        &&
        //并且判断被叫归属地和主叫到访地相同
        (!strcmp(_poES->m_sCalledAreaCode,_poES->m_oEventExt.m_sCallingVisitAreaCode))
    )
    {
        _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN; 
        return;
    }
    
    //判断是否本地呼叫，针对主叫到访地号码和被叫归属地号码
    if(!strcmp(_poES->m_oEventExt.m_sCallingVisitAreaCode,_poES->m_sCalledAreaCode))
    {
        _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN; 
        return;
    }
    

    //其他情况直接取被叫号码的归属地的长途组类型作为长途类型；
    int _iLongGroupType;
    getAreaCodeInfo(_poES->m_sCalledAreaCode,_iLongGroupType);
    getLongType(_poES,_iLongGroupType);
    
    return;
}

void FormatMethod::getMTCLongTypeOf3G(EventSection *_poES)
{
    //首先设置默认值
    _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN; 
    
    //判断是否同城
    //判断主叫号码的拜访地和被叫号码的拜访地相同，包括没有漫游和同城漫游的情况
    if(!strcmp(_poES->m_oEventExt.m_sCalledVisitAreaCode,_poES->m_oEventExt.m_sCallingVisitAreaCode))
    {
        _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN; 
        return;
    }
    
    //判断是否发生特殊同城漫游
    if
    (    //判断主叫归属地和被叫到访地相同
        (!strcmp(_poES->m_sCallingAreaCode,_poES->m_oEventExt.m_sCalledVisitAreaCode))
        &&
        //并且判断被叫归属地和主叫到访地相同
        (!strcmp(_poES->m_sCalledAreaCode,_poES->m_oEventExt.m_sCallingVisitAreaCode))
    )
    {
        _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN; 
        return;
    }
    
    //判断被叫归属地号码和被叫到访地号码是否相同
    if(!strcmp(_poES->m_oEventExt.m_sCalledVisitAreaCode,_poES->m_sCalledAreaCode))
    {
        _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN; 
        return;
    }
    
    
    //其他情况直接取被叫号码的到访地的长途组类型作为长途类型；
    int _iLongGroupType;
    getAreaCodeInfo(_poES->m_oEventExt.m_sCalledVisitAreaCode,_iLongGroupType);
    getLongType(_poES,_iLongGroupType);
    
    return;
}    

void FormatMethod::getFORWARDLongTypeOf3G(EventSection *_poES)
{
    //首先设置默认值
    _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN;
    
    int iLongType1,iLongType2;
    int _iLongGroupType;
        
    //对于呼转话单，分为主叫号码A、呼转号码B和呼转目标号码C。
    //话单中主叫号码放的是A，被叫号码放的是C，第三方号码放的是B
    //主叫号码的到访地区号就取归属地区号；
    //呼转号码作为第三方号码，同时是计费号码，呼转号码通过网络信息求取到访地区号；
    //话单中有呼转目标号码的动态漫游号码，通过调用动态漫游号取到访地区号函数，如果动态漫游号为空，那么将呼转目标号码的归属地区号放到呼转号码的到访地区号上（目前系统没有呼转目标号码的到访地区号这个字段，需要增加）。
    //同时把呼转号码到访地区号置给计费号码的到访地区号。
    //特别说明，目前由于不想修改StdEvent的结构，所以暂时用m_oEventExt.m_sFileName字段放置第三方号码的到访地区号
    
    //如果通过比较呼转号码的归属地和到访地，确认呼转号码没有发生漫游,
    if(!strcmp(_poES->m_oEventExt.m_sFileName,_poES->m_sThirdPartyAreaCode))
    {
        //判断是否发生同城漫游
        //判断是否同城
        //判断呼转号码的拜访地和呼转目标号码的拜访地相同，包括没有漫游和同城漫游的情况
        if(!strcmp(_poES->m_oEventExt.m_sFileName,_poES->m_oEventExt.m_sCalledVisitAreaCode))
        {
            _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN; 
            return;
        }
        
        //判断是否特殊同城漫游
        if
        (    //判断呼转号码的到访地和呼转目标号码的归属地相同
            (!strcmp(_poES->m_oEventExt.m_sFileName,_poES->m_sCalledAreaCode))
            &&
            //并且判断呼转号码归属地和呼转目标号码的到访地相同
            (!strcmp(_poES->m_sThirdPartyAreaCode,_poES->m_oEventExt.m_sCalledVisitAreaCode))
        )
        {
            _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN; 
            return;
        }
        
        //判断呼转号码的到访地和呼转目标号码的归属地是否相同
        if(!strcmp(_poES->m_sCalledAreaCode,_poES->m_oEventExt.m_sFileName))
        {
            _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN; 
            return;
        }
        
        //那么长途类型取决于呼转目标号码的归属地
        getAreaCodeInfo(_poES->m_sCalledAreaCode,_iLongGroupType);
        getLongType(_poES,_iLongGroupType);
        return;
    }
        
    //下面是呼转号码发生漫游的情况，
    if(strcmp(_poES->m_oEventExt.m_sFileName,_poES->m_sThirdPartyAreaCode)!=0)
    {
        //首先取得呼转号码从归属地到漫游地的长途类型
        //那么长途类型取决于呼转号码的到访地
        getAreaCodeInfo(_poES->m_oEventExt.m_sFileName,_iLongGroupType);
        getLongType(_poES,_iLongGroupType);
        
        //首先只考虑只有一段长途的情况:
        //第1种情况，第三方号码的到访地跟被叫号码的归属地相同
        //另外一种情况，虽然第三方号码的到访地跟被叫号码的归属地不同
        //但是第三方号码的到访地和被叫的到访地相同，也就是发生同城漫游的情况
        //这2种情况下都只可能一段长途
        if
        (
            (!strcmp(_poES->m_oEventExt.m_sFileName,_poES->m_sCalledAreaCode))
            ||
            (!strcmp(_poES->m_oEventExt.m_sFileName,_poES->m_oEventExt.m_sCalledVisitAreaCode))
        )
        {
            //已经取到长途类型
            return;
        }
        else if(_poES->m_iBusinessKey == BUSINESSKEY_FORWARDTYPE_UNCOND)
        {
            //只计算一段长途，已经取到长途类型
            return;
        }
        else
        //将会发生2段长途
        {
            iLongType1 = _poES->m_oEventExt.m_iLongTypeID;
            _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN;
            //再取第2段长途
            //那么长途类型取决于呼转号码的到访地
            getAreaCodeInfo(_poES->m_oEventExt.m_sCalledVisitAreaCode,_iLongGroupType);
            getLongType(_poES,_iLongGroupType);
            iLongType2 = _poES->m_oEventExt.m_iLongTypeID;
            //不需要再判断呼转号码的到访地和呼转目标号码的到访地之间是否同城，因为同城已经在前面判断过了！
            if((iLongType1 == LONGTYPE_PROV)&&(iLongType2 == LONGTYPE_NATION))
            {
                _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN_PROV_NATION;
                return;
            }
            else if((iLongType1 == LONGTYPE_NATION)&&(iLongType2 == LONGTYPE_NATION))
            {
                _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN_NATION_NATION;
                return;
            }
            else if((iLongType1 == LONGTYPE_PROV)&&(iLongType2 == LONGTYPE_PROV))
            {
                _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN_PROV_PROV;
                return;
            }
            else if((iLongType1 == LONGTYPE_NATION)&&(iLongType2 == LONGTYPE_PROV))
            {
                _poES->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN_NATION_PROV;
                return;
            }
        }
    }
    
    return;
    
}    

//## 规整3G业务的漫游类型/长途类型
void FormatMethod::formatRoamAndLongTypeOf3G(EventSection *_poES)
{
    if
    (
        (_poES->m_iCallTypeID!=CALLTYPE_MOC)
        &&
        (_poES->m_iCallTypeID!=CALLTYPE_MTC)
        &&
        (_poES->m_iCallTypeID!=CALLTYPE_FORWARD)
    )
    {
        //不需要处理，退出
        return;
    }
    
    
    //以下是根据新的讨论决定采用的处理步骤：
    //首先求取到访地
    getVisitAreaCodeOf3G(_poES);
    
    if(_poES->m_iErrorID > 0)
    {
        return;
    }

    //求取漫游类型
    getRoamTypeOf3G(_poES);

    //判断长途类型
    if(CALLTYPE_MOC == _poES->m_iCallTypeID)
    {
        getMOCLongTypeOf3G(_poES);
        return;
    }
    else if(CALLTYPE_MTC == _poES->m_iCallTypeID)
    {
        getMTCLongTypeOf3G(_poES);
        return;
    }
    if(CALLTYPE_FORWARD == _poES->m_iCallTypeID)
    {
        getFORWARDLongTypeOf3G(_poES);
        return;
    }
    
    return;
    
}


//__________________________________________________________


//##ModelId=4279D2E6008E
#ifdef PRIVATE_MEMORY
bool FormatMethod::getHLRInfo(char* _sCodeSegment, HLR* _pHLR)
{
    if (!_pHLR) return false;
    
    if (!m_pHLR)
        loadHLR();
    
    int bRet = m_pHLR->get(_sCodeSegment,_pHLR);
    
    if (bRet) return true;
    
    return false;
}
#else
bool FormatMethod::getHLRInfo(char* _sCodeSegment, HLR* _pHLR)
{
    if (!_pHLR) return false;  
    unsigned int iIdx = 0;
    if(!(G_PPARAMINFO->m_poHLRIndex->get(_sCodeSegment, &iIdx)))
        return false;
    else
    {
        *_pHLR = *(G_PPARAMINFO->m_poHLRList + iIdx);
        return true;
    }
}
#endif

//此函数为了正确获得用户资料从而得到其正确开始时间；
//因为代收事件可能没有开始时间，无法正确正确获得用户资料；
//而且代收用户可能改号，所以开始时间需要验证是取帐期开始时间还是去结束时间；
//系统在规整的初步，给事件加上帐期开始时间和结束时间，分别加在开始时间和结束时间2个字段上；
//该函数先用帐期结束时间去尝试用户资料；
//如果不成功，那么用帐期开始时间去尝试用户资料；
//对于后续可能改变计费号码的专网业务，现在不考虑，因为专网默认不会改号
void FormatMethod::getStartDateForDS(EventSection *_poEventSection)
{
    if((_poEventSection->m_oEventExt.m_iSourceEventType!=6)&&(_poEventSection->m_oEventExt.m_iSourceEventType!=9))
        return;
    
    if(_poEventSection->m_iErrorID != 0)
        return ;
    
    char sStartDate[15];
    strncpy(sStartDate,_poEventSection->m_sStartDate,14);
    sStartDate[14]=0;
        
    //## 通过规整初步设置的结束时间（帐期末时间）试图邦定
    strcpy(_poEventSection->m_sStartDate,_poEventSection->m_sEndDate);
    _poEventSection->bindUserInfo();
    
    //## 如果邦定成功就返回
    if (_poEventSection->m_iServID > 0)
    {
        //需要清空serv_id,原因在于可能计费号码在下面的规整过程中需要改变，例如改为专网号码等；
        _poEventSection->m_iServID = 0 ;
        _poEventSection->m_iErrorID = 0;
        return;
    }
    //如果绑定不成功
    else
    {
        //采用本帐期开始时间进行绑定
        strcpy(_poEventSection->m_sStartDate,sStartDate);
        g_poEventSection->bindUserInfo();
        
        if(_poEventSection->m_iServID > 0 )
        {
            //需要清空serv_id,原因在于可能计费号码在下面的规整过程中需要改变，例如改为专网号码等；
            _poEventSection->m_iServID = 0 ;
            _poEventSection->m_iErrorID = 0;
            return;
        }
        else
        //如果还是找不到用户资料，把通话开始时间还是修改为帐期开始时间
        {
            strcpy(_poEventSection->m_sStartDate,sStartDate);
            _poEventSection->m_iServID = 0 ;
            _poEventSection->m_iErrorID = 0;
            return;
        }
    }
}

//##ModelId=4279E8200358
void FormatMethod::loadCarrier()
{
    unloadCarrier();
    
    m_pCarrier = new KeyTree<int>;
    if (!m_pCarrier) THROW(MBC_FormatMethod+5);
    
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL("select precode,carrier_type_id from b_carrier");
    qry.open();
    
    while (qry.next()) {
        m_pCarrier->add(qry.field("precode").asString(),qry.field("carrier_type_id").asInteger());
    }
    
    qry.close();
    qry.setSQL("select max(length(precode)) max_len, min(length(precode)) min_len  from b_carrier");
    qry.open();
    if (qry.next()) {
        m_iMaxPreCodeLen = qry.field("max_len").asInteger();
        m_iMinPreCodeLen = qry.field("min_len").asInteger();
    }
    else {
        m_iMaxPreCodeLen = 0;
        m_iMinPreCodeLen = 0;
    }
    
    qry.close();
}

//##ModelId=4279E84C0199
void FormatMethod::unloadCarrier()
{
    if (m_pCarrier) {
        delete m_pCarrier;
        m_pCarrier = NULL;
    }
}

//##ModelId=4279E86C0308
#ifdef PRIVATE_MEMORY
bool FormatMethod::getCarrierInfo(char* _sPreCode, int &_iCarrierType)
{
    if (!m_pCarrier)
        loadCarrier();
    
    int bRet = m_pCarrier->get(_sPreCode, &_iCarrierType);
    
    if (bRet) return true;
    
    return false;
}
#else
bool FormatMethod::getCarrierInfo(char* _sPreCode, int &_iCarrierType)
{
	unsigned int iCarrierType = 0;
    if(G_PPARAMINFO->m_poCarrierIndex->get(_sPreCode, &iCarrierType))
	{
		_iCarrierType = (int)iCarrierType;
		return true;
	} 
	return false;
}
#endif

//##ModelId=427AE7520204
//## 供规整前台指定调用 FuncName:"承载类型规整"
//## 功能: 获取网络承载类型->g_poStdEventExt.m_iCarrierTypeID、去除被叫号码相应的字冠
//## 前提条件: 被叫号码的承载字冠前无其他号码
#ifdef PRIVATE_MEMORY  
void FormatMethod::formatCarrier(EventSection *_poEventSection)
{
    //char sTempNbr[MAX_CALLED_NBR_LEN] = {0};
    
    if (!m_pCarrier)
        loadCarrier();
    
/*
    int iNbrLen = strlen(_poEventSection->m_sCalledNBR);                //被叫号码长度
    int iCodeLen = m_iMaxPreCodeLen<iNbrLen ? m_iMaxPreCodeLen:iNbrLen;  //求需要截取比较的长度
    
    strncpy(sTempNbr, _poEventSection->m_sCalledNBR, iCodeLen);  //截取比较的长度
    
    for (int iPos=iCodeLen; iPos>=m_iMinPreCodeLen; iPos--) {
        
        int iCarType;
        
        sTempNbr[iPos]=0;
        
        if (getCarrierInfo(sTempNbr, iCarType) ) {
            
            _poEventSection->m_oEventExt.m_iCarrierTypeID = iCarType;
            
            char *pt = &(_poEventSection->m_sCalledNBR[iPos]);
            
            strcpy(_poEventSection->m_sCalledNBR,pt);
            
            return;
        }
    }
*/
    static int iCarrierType, iMatchLen;
    if (m_pCarrier->getMax(_poEventSection->m_sCalledNBR, &iCarrierType, &iMatchLen)) 
    {
        strcpy(_poEventSection->m_sCalledNBR, (_poEventSection->m_sCalledNBR)+iMatchLen);
        _poEventSection->m_oEventExt.m_iCarrierTypeID = iCarrierType;
        return;
    }
    
    _poEventSection->m_oEventExt.m_iCarrierTypeID = 0;
}
#else //
void FormatMethod::formatCarrier(EventSection *_poEventSection)
{
    static unsigned int SHM_iCarrierType,SHM_iMatchLen;
	//
    if (G_PPARAMINFO->m_poCarrierIndex->getMax(_poEventSection->m_sCalledNBR, &SHM_iCarrierType,&SHM_iMatchLen)) 
    {
        strcpy(_poEventSection->m_sCalledNBR, (_poEventSection->m_sCalledNBR)+SHM_iMatchLen);
        _poEventSection->m_oEventExt.m_iCarrierTypeID = (int)SHM_iCarrierType;
        return;
    }
    _poEventSection->m_oEventExt.m_iCarrierTypeID = 0;
}
#endif

//##ModelId=427B155301D6
//## 规整中继类型前,需要先确定交换机ID
void FormatMethod::formatTrunkTypeID(EventSection *_poEventSection)
{
    static Trunk obTrunk ;
    
    _poEventSection->m_iTrunkNotBillFlag = 0;
    
    //取入中继信息
    if (getTrunk(_poEventSection->m_iSwitchID, _poEventSection->m_sTrunkIn, '0', &obTrunk)) 
    {
        _poEventSection->m_iTrunkInTypeID = obTrunk.m_iTrunkTypeID;
        if (obTrunk.m_iNeedBilling == 0) _poEventSection->m_iTrunkNotBillFlag = 1;
    }
    else
        _poEventSection->m_iTrunkInTypeID = 0;
    
    //取出中继信息
    if (getTrunk(_poEventSection->m_iSwitchID, _poEventSection->m_sTrunkOut, '1', &obTrunk)) 
    {
        _poEventSection->m_iTrunkOutTypeID = obTrunk.m_iTrunkTypeID;
        if (obTrunk.m_iNeedBilling == 0) _poEventSection->m_iTrunkNotBillFlag = 1;
    }
    else
        _poEventSection->m_iTrunkOutTypeID = 0;
}


//##ModelId=427B21E60370
//## 规整主、被叫区号(包括固定业务、移动业务),将固定业务的主被叫号码的区号截掉,
//## 同时获取主/被叫长途组类型
void FormatMethod::formatAreaCode(EventSection *_poEventSection)
{
    static int  s_iCalledInterFlag = -1; //0-被叫号码区号不截取, 1-被叫号码区号截取掉
    static char sTempCalledNBR[MAX_ORG_CALLED_NBR_LEN];
    if (s_iCalledInterFlag == -1) {
        char sTemp[1000];
        if (ParamDefineMgr::getParam("FORMAT_METHOD", "CALLED_INTER_FLAG", sTemp)) {
            if (sTemp[0] == '1')  s_iCalledInterFlag = 1;
            else                  s_iCalledInterFlag = 0;
        }
        else //未指定
            s_iCalledInterFlag = 1; //## 默认要截取被叫区号
    }
    if (!s_iCalledInterFlag) strcpy (sTempCalledNBR, _poEventSection->m_sCalledNBR);

    
    if (_poEventSection->m_sCallingNBR[0])    //## 主叫号码
        formatAccNBR(_poEventSection->m_sCallingNBR, _poEventSection->m_sCallingAreaCode,
            1,_poEventSection->m_iSwitchID);
            
    if (_poEventSection->m_sCalledNBR[0])     //## 被叫号码
    {
        formatAccNBR(_poEventSection->m_sCalledNBR, _poEventSection->m_sCalledAreaCode,
            2,_poEventSection->m_iSwitchID);
        
        if (!s_iCalledInterFlag)
            strcpy (_poEventSection->m_sCalledNBR, sTempCalledNBR);
    }
    
    if (_poEventSection->m_sThirdPartyNBR[0]) //## 如果第三方号码不空,则求其区号
        formatAccNBR(_poEventSection->m_sThirdPartyNBR,_poEventSection->m_sThirdPartyAreaCode,
            3,_poEventSection->m_iSwitchID);
    
    if (_poEventSection->m_sAccountNBR[0])    //## 数据业务帐号
        formatAccNBR(_poEventSection->m_sAccountNBR,_poEventSection->m_sAccountAreaCode,
            4,_poEventSection->m_iSwitchID);
    
    // 置长途组类型
    int iLongGrpTypeID;
    
    if (_poEventSection->m_sCallingAreaCode[0] &&
            getAreaCodeInfo(_poEventSection->m_sCallingAreaCode,iLongGrpTypeID))
        _poEventSection->m_oEventExt.m_iCallingLongGroupTypeID = iLongGrpTypeID;
    else
        _poEventSection->m_oEventExt.m_iCallingLongGroupTypeID = 0;
    
    if (_poEventSection->m_sCalledAreaCode[0] &&
            getAreaCodeInfo(_poEventSection->m_sCalledAreaCode,iLongGrpTypeID))
        _poEventSection->m_oEventExt.m_iCalledLongGroupTypeID = iLongGrpTypeID; 
    else
        _poEventSection->m_oEventExt.m_iCalledLongGroupTypeID = 0;

}

// add by yangks 2007.03.03  for SC
//## 规整主、被叫区号(包括固定业务、移动业务),将固定业务的主被叫号码的区号截掉,
//## 同时获取主/被叫长途组类型
void FormatMethod::format_AreaCode_V2(EventSection *_poEventSection)
{
    if (_poEventSection->m_sCallingNBR[0])    //## 主叫号码
        format_AccNBR_V2 (_poEventSection->m_sCallingNBR, _poEventSection->m_sCallingAreaCode,
            1,_poEventSection->m_iSwitchID);
            
    if (_poEventSection->m_sCalledNBR[0])     //## 被叫号码
        format_AccNBR_V2 (_poEventSection->m_sCalledNBR, _poEventSection->m_sCalledAreaCode,
            2,_poEventSection->m_iSwitchID);
    
    if (_poEventSection->m_sThirdPartyNBR[0]) //## 如果第三方号码不空,则求其区号
        format_AccNBR_V2 (_poEventSection->m_sThirdPartyNBR,_poEventSection->m_sThirdPartyAreaCode,
            3,_poEventSection->m_iSwitchID);
    
    if (_poEventSection->m_sAccountNBR[0])    //## 数据业务帐号
        format_AccNBR_V2 (_poEventSection->m_sAccountNBR,_poEventSection->m_sAccountAreaCode,
            4,_poEventSection->m_iSwitchID);
    
    // 置长途组类型
    int iLongGrpTypeID;
    
    if (_poEventSection->m_sCallingAreaCode[0] &&
            getAreaCodeInfo(_poEventSection->m_sCallingAreaCode,iLongGrpTypeID))
        _poEventSection->m_oEventExt.m_iCallingLongGroupTypeID = iLongGrpTypeID;
    else
        _poEventSection->m_oEventExt.m_iCallingLongGroupTypeID = 0;
    
    if (_poEventSection->m_sCalledAreaCode[0] &&
            getAreaCodeInfo(_poEventSection->m_sCalledAreaCode,iLongGrpTypeID))
        _poEventSection->m_oEventExt.m_iCalledLongGroupTypeID = iLongGrpTypeID; 
    else
        _poEventSection->m_oEventExt.m_iCalledLongGroupTypeID = 0;

}


//##ModelId=427B4AA501F2
//## 将 _sAccNBR 分解为 号码、区号, 分别置在 _sAccNBR, _sAreaCode 上
//## 前置条件: 当前号码为合法号码,合法号码的情况如下：
//##    cond_010: 区号+固定号码   02587654321
//                区号+固定号码   2587654321  (此情况一般是主叫,需要在前台规整规则中加"0",转换成cond_010)
//           区号+区号+固定号码   02502587654321/0252587654321 (当需要做循环匹配时 
//                                  b_param_define.param_code='AREACODE_CYC_FIT' 对应param_value=1
//##    cond_020: 区号+移动号码   02513913930122 (截取区号)
//##    cond_030: 移动号码        13345678999
//##    cond_031: 移动号码        013345678999 
//##    cond_032: 3G  号码        (0)15......... [广州测试 157........(广州)/158........(东莞)]
//##    cond_040: 固定号码无区号  87654321    (目前针对主被叫都作判断,根据交换机求其区号)
void FormatMethod::formatAccNBR(char* _sAccNBR, char* _sAreaCode, int _iCallerOrCalled, int _iSwitchID)
{
    int iLongGrpTypeID;
#ifdef PRIVATE_MEMORY
    if (!m_pAreaCode) //如果区号未上载, m_iMaxAreaCodeLen 为0,所以,必须先上载
        loadAreaCode();
#else
	char *pKey = "CycleFit";
	unsigned int iSHMRes = 0;
	if(!G_PPARAMINFO->m_poAreaCodeIndex->get(pKey,&iSHMRes))
	{
		m_bAreaCodeCycleFit = 0;
	}
	m_bAreaCodeCycleFit = (int)iSHMRes;
	
	pKey = "MobHeadZero";
	if(!G_PPARAMINFO->m_poAreaCodeIndex->get(pKey,&iSHMRes))
	{
		m_iTrimMobHeadZero = 0;
	}
	m_iTrimMobHeadZero = (int)iSHMRes;
	
	//
	pKey = "SelAreaPart";
	if(!G_PPARAMINFO->m_poAreaCodeIndex->get(pKey,&iSHMRes))
	{
		m_iSelAreaPart = 0;
	}
	m_iSelAreaPart = (int)iSHMRes;
#endif    
    // cond_010, cond_020  (截取掉 _sAccNBR 前的区号)
    if (_sAccNBR[0] == '0' && strncmp(_sAccNBR,"013",3) && strncmp(_sAccNBR,"015",3)&&strncmp(_sAccNBR,"014",3)&&strncmp(_sAccNBR,"018",3)) { 
        
        int iRet;
        if (m_bAreaCodeCycleFit)
            iRet = interceptAreaCodeCycle(_sAccNBR, _sAreaCode, iLongGrpTypeID);
        else
            iRet = interceptAreaCode(_sAccNBR, _sAreaCode, iLongGrpTypeID);
        
        if (!iRet) {            
            _sAreaCode[0]=0;
            return;
        }       
        return;
    }
    
    // cond_030:  _sAccNBR = "13345678999"
    if ((!strncmp(_sAccNBR,"13",2))||(!strncmp(_sAccNBR,"15",2))||(!strncmp(_sAccNBR,"14",2))||(!strncmp(_sAccNBR,"18",2))) {
#ifdef USE_HLR_CONVERT
        char sHcode[20];
        getHLRInfoByMobileNBR(_sAccNBR, _sAreaCode, sHcode);
#else
        TSwitchInfo obSwitchInfo;
        //根据交换机找默认区号
        if (getSwitchInfoBySwitchID(_iSwitchID, &obSwitchInfo)) {
            strcpy(_sAreaCode, obSwitchInfo.m_sAreaCode);
        }
#endif
        return ;
    }
    
    // cond_031:  _sAccNBR = "013345678999"
    if ((!strncmp(_sAccNBR,"013",3))||(!strncmp(_sAccNBR,"015",3))||(!strncmp(_sAccNBR,"014",3))||(!strncmp(_sAccNBR,"018",3))) {
#ifdef USE_HLR_CONVERT
        char sHcode[20];
        getHLRInfoByMobileNBR(_sAccNBR, _sAreaCode, sHcode);
#else
        strcpy(_sAreaCode,"0"); //## "0"为国内默认区号
#endif

        if (m_iTrimMobHeadZero)
            strcpy(_sAccNBR, _sAccNBR+1);
        
        return;
    }
    
    // cond_040:  固定号码无区号则置交换机区号  87654321
    int iLenTemp = strlen(_sAccNBR);
    if (_sAccNBR[0]!='0' && iLenTemp<=D_M_ACCNBR_LEN) {//## D_M_ACCNBR_LEN 固话号码最大长度 在CommonMacro.h中定义
        TSwitchInfo obSwitchInfo;
        //根据交换机找默认区号
        if (getSwitchInfoBySwitchID(_iSwitchID, &obSwitchInfo)) {
            strcpy(_sAreaCode, obSwitchInfo.m_sAreaCode);
            return;
        }
    }
    
    // 其他未知情况: 默认取交换机区号
    TSwitchInfo obSwitchInfo;
    if (getSwitchInfoBySwitchID(_iSwitchID, &obSwitchInfo)) {
        strcpy(_sAreaCode, obSwitchInfo.m_sAreaCode);
    }
}

//第二版本 add by yangks 2007.03.03
//## 将 _sAccNBR 分解为 号码、区号, 分别置在 _sAccNBR, _sAreaCode 上
//## 前置条件: 当前号码为合法号码,合法号码的情况如下：
//##    cond_010: 区号+固定号码   02587654321
//                区号+固定号码   2587654321  (此情况一般是主叫,需要在前台规整规则中加"0",转换成cond_010)
//           区号+区号+固定号码   02502587654321/0252587654321 (当需要做循环匹配时 
//                                  b_param_define.param_code='AREACODE_CYC_FIT' 对应param_value=1
//##    cond_020: 区号+移动号码   02513913930122 (截取区号)
//##    cond_030: 移动号码        13345678999
//##    cond_031: 移动号码        013345678999 
//##    cond_032: 3G  号码        (0)15......... [广州测试 157........(广州)/158........(东莞)]
//##    cond_040: 固定号码无区号  87654321    (目前针对主被叫都作判断,根据交换机求其区号)
void FormatMethod::format_AccNBR_V2 (char* _sAccNBR, char* _sAreaCode, 
                int _iCallerOrCalled, int _iSwitchID)
{
    int iLongGrpTypeID;
#ifdef PRIVATE_MEMORY
    if (!m_pAreaCode) //如果区号未上载, m_iMaxAreaCodeLen 为0,所以,必须先上载
        loadAreaCode();
#else
	char *pKey = "CycleFit";
	unsigned int iSHMRes = 0;
	if(!G_PPARAMINFO->m_poAreaCodeIndex->get(pKey,&iSHMRes))
	{
		m_bAreaCodeCycleFit = 0;
	}
	m_bAreaCodeCycleFit = (int)iSHMRes;
	
	pKey = "MobHeadZero";
	if(!G_PPARAMINFO->m_poAreaCodeIndex->get(pKey,&iSHMRes))
	{
		m_iTrimMobHeadZero = 0;
	}
	m_iTrimMobHeadZero = (int)iSHMRes;
	
	//
	pKey = "SelAreaPart";
	if(!G_PPARAMINFO->m_poAreaCodeIndex->get(pKey,&iSHMRes))
	{
		m_iSelAreaPart = 0;
	}
	m_iSelAreaPart = (int)iSHMRes;
#endif    
    // cond_010, cond_020  (截取掉 _sAccNBR 前的区号)
    if (_sAccNBR[0] == '0' && strncmp(_sAccNBR,"013",3) && strncmp(_sAccNBR,"015",3)
        &&strncmp(_sAccNBR,"014",3)&&strncmp(_sAccNBR,"018",3)
    ) {         
        int iRet;
        if (m_bAreaCodeCycleFit)
            iRet = interceptAreaCodeCycle(_sAccNBR, _sAreaCode, iLongGrpTypeID);
        else
            iRet = interceptAreaCode(_sAccNBR, _sAreaCode, iLongGrpTypeID);
        
        if (!iRet) {            
            _sAreaCode[0]=0;
            return;
        }       
        return;
    }
    
    // cond_030:  _sAccNBR = "13345678999"
    if ((!strncmp(_sAccNBR,"13",2))||(!strncmp(_sAccNBR,"15",2))||(!strncmp(_sAccNBR,"14",2))||(!strncmp(_sAccNBR,"18",2))) {
        char sHcode[20];
        bool bRet = getHLRInfoByMobileNBR(_sAccNBR, _sAreaCode, sHcode);
        if (!bRet) {
            TSwitchInfo obSwitchInfo;
            //根据交换机找默认区号
            if (getSwitchInfoBySwitchID(_iSwitchID, &obSwitchInfo)) {
                strcpy(_sAreaCode, obSwitchInfo.m_sAreaCode);
            }
        }
        return ;
    }
    
    // cond_031:  _sAccNBR = "013345678999"
    if ((!strncmp(_sAccNBR,"013",3))||(!strncmp(_sAccNBR,"015",3))||(!strncmp(_sAccNBR,"014",3))||(!strncmp(_sAccNBR,"018",3))) {
        char sHcode[20];
        bool bRet = getHLRInfoByMobileNBR(_sAccNBR, _sAreaCode, sHcode);

        if (!bRet)
            strcpy(_sAreaCode,"0"); //## "0"为国内默认区号

#ifdef DEF_HEBEI
        // called_nbr must be get rid of 0, because called_area_code was assigned 0
        strcpy(_sAccNBR, _sAccNBR+1);
#else
        if (m_iTrimMobHeadZero)
            strcpy(_sAccNBR, _sAccNBR+1);
#endif
        return;
    }
     
    // cond_040:  固定号码无区号则置交换机区号  87654321
    int iLenTemp = strlen(_sAccNBR);
    if (_sAccNBR[0]!='0' && iLenTemp<=D_M_ACCNBR_LEN) {//## D_M_ACCNBR_LEN 固话号码最大长度 在CommonMacro.h中定义
        TSwitchInfo obSwitchInfo;
        //根据交换机找默认区号
        if (getSwitchInfoBySwitchID(_iSwitchID, &obSwitchInfo)) {
            strcpy(_sAreaCode, obSwitchInfo.m_sAreaCode);
            return;
        }
    }
    
    // 其他未知情况: 默认取交换机区号
    TSwitchInfo obSwitchInfo;
    if (getSwitchInfoBySwitchID(_iSwitchID, &obSwitchInfo)) {
        strcpy(_sAreaCode, obSwitchInfo.m_sAreaCode);
    }
}

//##ModelId=427C5B31006D
void FormatMethod::loadSwitchInfo()
{
    unloadSwitchInfo();
    
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL("select count(*) cnt from b_switch_info");
    qry.open();
    qry.next();
    
    m_iSwitchInfoCnt = qry.field("cnt").asInteger();
    
    if (!m_iSwitchInfoCnt) return;
    
    m_aSwitchInfo = new TSwitchInfo[m_iSwitchInfoCnt];
    if (!m_aSwitchInfo) 
        THROW(MBC_FormatMethod+6);
    
    m_pSwitchInfoPosTree = new KeyTree<int>;
    if (!m_pSwitchInfoPosTree)
        THROW(MBC_FormatMethod+7);
    
    // add by yangks 2007.02.12 for Jiangsu Dev
    m_pSwitchOPCInfoTree = new KeyTree<int>;
    if (!m_pSwitchOPCInfoTree)
        THROW(MBC_FormatMethod+20);
    
    qry.close();
    
    /* mod by yks 2007.07.06  for JS_TIBS_REQ_20070706_001_yangks
       交换机默认的NetworkID统一为0
    qry.setSQL("select switch_id,file_name_rule,area_code,source_event_type,switch_type_id, "
        " nvl(network_id, -1) network_id, nvl(switch_opc_code,'#') switch_opc_code "
        " from b_switch_info ");
    */
    qry.setSQL("select switch_id,file_name_rule,area_code,source_event_type,switch_type_id, "
        " nvl(network_id, 0) network_id, nvl(switch_opc_code,'#') switch_opc_code "
        " from b_switch_info ");
    qry.open();
    
    for (int iPos=0; qry.next() && (iPos<m_iSwitchInfoCnt); iPos++) {
        m_aSwitchInfo[iPos].m_iSwitchID = qry.field("switch_id").asInteger();
        m_aSwitchInfo[iPos].m_iSourceEventType = qry.field("source_event_type").asInteger();
        m_aSwitchInfo[iPos].m_iSwitchTypeID = qry.field("switch_type_id").asInteger();
        strcpy(m_aSwitchInfo[iPos].m_sFileNameRule, qry.field("file_name_rule").asString());
        strcpy(m_aSwitchInfo[iPos].m_sAreaCode, qry.field("area_code").asString());

        //add by yangks 2007.02.12 for Jiangsu Dev
        m_aSwitchInfo[iPos].m_iNetworkID = qry.field("network_id").asInteger();
        strcpy(m_aSwitchInfo[iPos].m_sSwichOpcCode,qry.field("switch_opc_code").asString());
        if (m_aSwitchInfo[iPos].m_sSwichOpcCode[0] != '#')
            m_pSwitchOPCInfoTree->add(m_aSwitchInfo[iPos].m_sSwichOpcCode, iPos);
        
        m_pSwitchInfoPosTree->add(m_aSwitchInfo[iPos].m_iSwitchID,iPos);
    }
    
    qry.close();
}

//##ModelId=427C5B6003BD
void FormatMethod::unloadSwitchInfo()
{
    if (m_aSwitchInfo) {
        delete m_aSwitchInfo;
        m_aSwitchInfo = NULL;
    }
    
    if (m_pSwitchInfoPosTree) {
        delete m_pSwitchInfoPosTree;
        m_pSwitchInfoPosTree = NULL;
    }
}

//##ModelId=427C5B6D0091
#ifdef PRIVATE_MEMORY
bool FormatMethod::getSwitchInfoBySwitchID(int _iSwitchID, TSwitchInfo* _pSwitchInfo)
{
    if (!_pSwitchInfo) return false;
    
    if (!m_pSwitchInfoPosTree)
        loadSwitchInfo();
    
    int iPos;
    
    bool bRet = m_pSwitchInfoPosTree->get(_iSwitchID,&iPos);
    
    if (!bRet) return false; //not found
    
    *_pSwitchInfo = m_aSwitchInfo[iPos];
    
    return true;
}
#else
bool FormatMethod::getSwitchInfoBySwitchID(int _iSwitchID, TSwitchInfo* _pSwitchInfo)
{
    if (!_pSwitchInfo) return false;  
    unsigned int iPos = 0;
    bool bRet =  G_PPARAMINFO->m_poSwitchIndex->get(_iSwitchID, &iPos);
    if (!bRet) 	return false; //not found
    *_pSwitchInfo = *(G_PPARAMINFO->m_poSwitchList + iPos);//memcpy(_pSwitchInfo,G_PPARAMINFO->m_poSwitchList + iPos,sizeof(TSwitchInfo));
    return true;
}
#endif

#ifdef PRIVATE_MEMORY
bool FormatMethod::getSwitchInfoBySwitchOPC(char *_sOpcCode, TSwitchInfo* _pSwitchInfo)
{
    if (!_pSwitchInfo) return false;
    
    if (!m_pSwitchOPCInfoTree)
        loadSwitchInfo();
    
    int iPos;
    bool bRet =  m_pSwitchOPCInfoTree->get(_sOpcCode, &iPos);
    if (!bRet) return false;
    
    *_pSwitchInfo = m_aSwitchInfo[iPos];
    
    return true;
}
#else
bool FormatMethod::getSwitchInfoBySwitchOPC(char *_sOpcCode, TSwitchInfo* _pSwitchInfo)
{
    if (!_pSwitchInfo) return false;
    unsigned int iPos;
    bool bRet =  G_PPARAMINFO->m_poSwitchOPCIndex->get(_sOpcCode, &iPos);
    if (!bRet) return false;
    *_pSwitchInfo = *(G_PPARAMINFO->m_poSwitchList + iPos);
	//memcpy(_pSwitchInfo,G_PPARAMINFO->m_poSwitchList + iPos,sizeof(TSwitchInfo));
    return true;
}
#endif

//##ModelId=427C330302DA
//## 输入手机号码(允许号码前带0,函数不改变 _sMobileNBR 的值)
//## 输出区号(手机号码的地区号,如果未找到相应号段,不改变_sAreaCode的值)
//## 输出 手机号段(如果未找到,不改变_sHcode的值)
//#ifdef PRIVATE_MEMORY
bool FormatMethod::getHLRInfoByMobileNBR(char* _sMobileNBR, char *_sAreaCode, char* _sHcode)
{
    int iPos;
    char sTempNBR[32];
    HLR obHLR;
    
    strcpy(sTempNBR,_sMobileNBR);
    if (sTempNBR[0]=='0')
        strcpy(sTempNBR,sTempNBR+1);
#ifdef PRIVATE_MEMORY    
    if (!m_pHLR)
        loadHLR();
#else
	char *p ="max";
	if(!G_PPARAMINFO->m_poHLRIndex->get(p,&iMaxSegCodeLen))
	{
		iMaxSegCodeLen = 0;
	}
	p ="min";
	if(!G_PPARAMINFO->m_poHLRIndex->get(p,&iMinSegCodeLen))
	{
		iMinSegCodeLen = 0;
	}
	m_iMaxSegCodeLen = (int)iMaxSegCodeLen;
	m_iMinSegCodeLen = (int)iMinSegCodeLen;
#endif    
    for (iPos=m_iMaxSegCodeLen; iPos>=m_iMinSegCodeLen; iPos-- ) {
        sTempNBR[iPos] = 0;
        if (getHLRInfo(sTempNBR,&obHLR)) {
            strcpy(_sHcode, sTempNBR);
            strcpy(_sAreaCode, obHLR.m_sAreaCode);
            return true;
        }
    }
    
    //未匹配到号段
    return false;
}

//##ModelId=427B9CC600EB
//## 截取标准的 区号+号码 格式的_sAccNBR 中的区号, 返回给 _sAreaCode, _sAccNBR区号被截掉
bool FormatMethod::interceptAreaCode(char* _sAccNBR, char* _sAreaCode, int &_iLongGroupType)
{
    char sTemp[MAX_ORG_CALLED_NBR_LEN] = {0};
    char sTempAccNBR[MAX_ORG_CALLED_NBR_LEN] = {0};
#ifdef PRIVATE_MEMORY  
    if (!m_pAreaCode) //如果区号未上载, m_iMaxAreaCodeLen 为0,所以,必须先上载
        loadAreaCode();
#else
	char *p = "max";
	iMaxAreaCodeLen = 0;
	iMinAreaCodeLen = 0;
    if(!G_PPARAMINFO->m_poAreaCodeIndex->get(p,&iMaxAreaCodeLen))
	{
		iMaxAreaCodeLen = 0;
	}
	p = "min";
	if(!G_PPARAMINFO->m_poAreaCodeIndex->get(p,&iMinAreaCodeLen))
	{
		iMinAreaCodeLen = 0;
	}
	m_iMaxAreaCodeLen = (int)iMaxAreaCodeLen;
	m_iMinAreaCodeLen = (int)iMinAreaCodeLen;
#endif  
    int iAccNbrLen = strlen(_sAccNBR);
    int iMaxAreaLen = m_iMaxAreaCodeLen<iAccNbrLen ? m_iMaxAreaCodeLen:iAccNbrLen;
    
    strcpy(sTempAccNBR,_sAccNBR);
    strcpy(sTemp,_sAccNBR);
    
    for (int iPos=iMaxAreaLen; iPos>=m_iMinAreaCodeLen; iPos--) {
        sTemp[iPos] = 0;
        if (getAreaCodeInfo(sTemp,_iLongGroupType)) {
            strcpy(_sAreaCode,sTemp);
            strcpy(_sAccNBR,sTempAccNBR+iPos);
            return true;
        }
    }
    
    _iLongGroupType = 0;
    _sAreaCode[0] = 0;
    
    return false;
}

//## 功能描述: 循环匹配/截取号码的区号
// 若能匹配到正常区号, 返回1, 若匹配到默认国内区号"0",则返回2
//      并将 _sAccNBR 的区号截取下来放在 _sAreaCode上, 并置长途组类型 _iLongGroupType
// 如果匹配不到,返回0, 置 _sAreaCode为空; _iLongGroupType=0;
int FormatMethod::interceptAreaCodeCycle(char* _sAccNBR, char* _sAreaCode, int &_iLongGroupType)
{
    static char sFirstFitArea[MAX_ORG_CALLED_NBR_LEN] = {0};
    static int iFirstLongGrpType = 0;
    
    char sTemp[MAX_ORG_CALLED_NBR_LEN] = {0};
    char sTempAccNBR[MAX_ORG_CALLED_NBR_LEN] = {0};
#ifdef PRIVATE_MEMORY    
    if (!m_pAreaCode) //如果区号未上载, m_iMaxAreaCodeLen 为0,所以,必须先上载
        loadAreaCode();
#else
	char *p = "max";
	iMaxAreaCodeLen = 0;
	iMinAreaCodeLen = 0;
    if(!G_PPARAMINFO->m_poAreaCodeIndex->get(p,&iMaxAreaCodeLen))
	{
		iMaxAreaCodeLen = 0;
	}
	p = "min"; 
	if(!G_PPARAMINFO->m_poAreaCodeIndex->get(p,&iMinAreaCodeLen))
	{
		iMinAreaCodeLen = 0;
	}
	m_iMaxAreaCodeLen = (int)iMaxAreaCodeLen;
	m_iMinAreaCodeLen = (int)iMinAreaCodeLen;
	
	p = "SelAreaPart";
	unsigned int iSHMRes = 0;
	if(!G_PPARAMINFO->m_poAreaCodeIndex->get(p,&iSHMRes))
	{
		m_iSelAreaPart = 0;
	}
	m_iSelAreaPart = (int)iSHMRes;
#endif  
    int iAccNbrLen = strlen(_sAccNBR);
    int iMaxAreaLen = m_iMaxAreaCodeLen<iAccNbrLen ? m_iMaxAreaCodeLen:iAccNbrLen;

/*
    if (_sAccNBR[0]!='0' && strlen(_sAccNBR)>=D_M_ACCNBR_LEN+1) {
        sprintf(sTemp,"0%s",_sAccNBR);
    }
    else {
        strcpy(sTemp,_sAccNBR);
    }
*/
    strcpy(sTemp,_sAccNBR);
    strcpy(sTempAccNBR,sTemp);

    for (int iPos=iMaxAreaLen; iPos>=m_iMinAreaCodeLen; iPos--) {
        sTemp[iPos] = 0;
        if (getAreaCodeInfo(sTemp,_iLongGroupType)) {
            strcpy(_sAreaCode,sTemp);
            strcpy(_sAccNBR,sTempAccNBR+iPos);
            
            if (m_iSelAreaPart && sFirstFitArea[0] == 0) { //## 当前是第一次匹配区号
                strcpy(sFirstFitArea,sTemp);
                iFirstLongGrpType = _iLongGroupType;
            }
            
            if (!strcmp(_sAreaCode,"0"))     //@@ 只匹配到国内默认区号"0", (不继续做循环匹配)
                return 2;
            
            if (!strncmp(_sAreaCode,"00",2)) //@@ 匹配到国际区号,后面的号码情况不可预知,不再做循环匹配
                return 1;
                
            //## 截取一次区号后的 _sAccNBR 可能情况:
            //  1. 87654321 或者 短号码 110
            //  2. 13566666666
            //  3. 013566666666
            //  4. 0512******** (带0的区号＋号码)
            //  5. 512********  (不带0的区号＋号码)  [暂不考虑这种情况,号码中的区号必须有0]
            int iAccNbrLen = strlen(_sAccNBR);
            if (iAccNbrLen <= D_M_ACCNBR_LEN && _sAccNBR[0] != '0')             //## 处理情况: 1
                return 1;
            if (iAccNbrLen>=D_M_MOBNBR_LEN-1 && iAccNbrLen<=D_M_MOBNBR_LEN+1 
                    && !strncmp(_sAccNBR,"13",2)&& !strncmp(_sAccNBR,"15",2)&& !strncmp(_sAccNBR,"18",2)&& !strncmp(_sAccNBR,"14",2))                               //## 处理情况: 2
                return 1;
            if (iAccNbrLen>=D_M_MOBNBR_LEN && iAccNbrLen<=D_M_MOBNBR_LEN+2
                    && !strncmp(_sAccNBR,"013",3)&& !strncmp(_sAccNBR,"014",3)&& !strncmp(_sAccNBR,"015",3)&& !strncmp(_sAccNBR,"018",3))                              //## 处理情况: 3
                return 1;
            
            if (
                (_sAccNBR[0]=='0')                                              //## 处理情况: 4
            )
            {
                int iLongGroupTypeSav = _iLongGroupType; //## 保存当前值
                int iRet = FormatMethod::interceptAreaCodeCycle(_sAccNBR, _sAreaCode, _iLongGroupType);
                if (!iRet || iRet==2) { //未匹配到 或者 只匹配到国内默认区号"0"
                    _iLongGroupType = iLongGroupTypeSav;
                    strcpy(_sAreaCode,sTemp);
                    if (iRet==2) {
                        sprintf(sTemp,"0%s",_sAccNBR);
                        strcpy(_sAccNBR,sTemp);
                    }
                }
            }
            
            if (m_iSelAreaPart) {
                strcpy(_sAreaCode,sFirstFitArea);
                _iLongGroupType = iFirstLongGrpType;
                sFirstFitArea[0] = 0;
            }
            
            return 1;
        }
    }
    
    _iLongGroupType = 0;
    _sAreaCode[0] = 0;
    
    return 0;
}


void FormatMethod::loadPNSegBilling()
{
    char sMaxEndNBRAhead[MAX_CALLING_NBR_LEN] = {0};
    
    unloadPNSegBilling();
    
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL("select count(*) cnt from b_pn_seg_billing");
    qry.open();
    qry.next();
    
    m_iPNSegBillingCnt = qry.field("cnt").asInteger();
    
    if (!m_iPNSegBillingCnt) {
        m_aPNSegBilling = new PNSegBilling[1]; //目的是 m_aPNSegBilling非空,表示已上载过
        return;
    }
    
    m_aPNSegBilling = new PNSegBilling[m_iPNSegBillingCnt];
    if (!m_aPNSegBilling)
        THROW(MBC_FormatMethod+8);
    
    qry.close();
    qry.setSQL(" select  area_code, begin_acc_nbr, end_acc_nbr, acc_nbr, "
        " to_char(eff_date,'yyyymmddhh24miss') eff_date, "
        " to_char(exp_date,'yyyymmddhh24miss') exp_date, "
        " to_number(nvl(seg_flag,0) ) seg_flag "
        " from b_pn_seg_billing order by begin_acc_nbr, end_acc_nbr " );
    qry.open();
    
    // 按 begin_nbr 从小到大字符串排序, 放入数组 m_aPNSegBilling 中
    for (int iPos=0; qry.next() && (iPos<m_iPNSegBillingCnt); iPos++) {
        strcpy(m_aPNSegBilling[iPos].m_sAreaCode, qry.field("area_code").asString());
        strcpy(m_aPNSegBilling[iPos].m_sBeginNBR, qry.field("begin_acc_nbr").asString());
        strcpy(m_aPNSegBilling[iPos].m_sEndNBR, qry.field("end_acc_nbr").asString());       
        strcpy(m_aPNSegBilling[iPos].m_sAccNBR, qry.field("acc_nbr").asString());
        strcpy(m_aPNSegBilling[iPos].m_sEffDate, qry.field("eff_date").asString());
        strcpy(m_aPNSegBilling[iPos].m_sExpDate, qry.field("exp_date").asString());
        
        strcpy(m_aPNSegBilling[iPos].m_sMaxEndNBRAhead,sMaxEndNBRAhead);
        if (strcmp(m_aPNSegBilling[iPos].m_sEndNBR, sMaxEndNBRAhead)>0)
            strcpy(sMaxEndNBRAhead, m_aPNSegBilling[iPos].m_sEndNBR);

        m_aPNSegBilling[iPos].m_iSegFlag=qry.field("seg_flag").asInteger();


        
    }
    
    qry.close();
}


void FormatMethod::unloadPNSegBilling()
{
    if (m_aPNSegBilling) {
        delete m_aPNSegBilling;
        m_aPNSegBilling = NULL;
    }
}

#ifdef PRIVATE_MEMORY 
bool FormatMethod::getYGPNSegBilling(char *_sAccNBR, char *_sAreaCode,
        char *_sDate, PNSegBilling &_oSeg)
{
    if (!m_aPNSegBilling)
        loadPNSegBilling();
    
    int iHead = 0, iTail = m_iPNSegBillingCnt-1;
    int iMid ;
    if (m_iPNSegBillingCnt == 0) return false;
    // 折半查找, 试图在m_aPNSegBilling中找到一个 m_sBeginNBR == _sAccNBR 的m_aPNSegBilling记录
    // 可能的结果: 
    //     1. 找到, iMid 指向 一个 m_sBeginNBR == _sAccNBR 的m_aPNSegBilling记录
    //     2. 未找到, 可能情况: 
    //          a. iMid 指向最后一个 m_sBeginNBR < _sAccNBR 的m_aPNSegBilling记录
    //          b. iMid 指向最前一个 m_sBeginNBR > _sAccNBR 的m_aPNSegBilling记录
    while (iHead <= iTail) {
        iMid = (iHead + iTail)/2;
        int iRet = strcmp(_sAccNBR, m_aPNSegBilling[iMid].m_sBeginNBR);
        if (iRet > 0) {
            iHead = iMid+1;
            continue;
        }
        if (iRet < 0) {
            iTail = iMid-1;
            continue;
        }
        //found:
        break;
    }
    
    //目标: 让iMid 指向 最后一个 (m_sBeginNBR <= _sAccNBR) 的 m_aPNSegBilling 记录.
    int iRet = strcmp(_sAccNBR,m_aPNSegBilling[iMid].m_sBeginNBR);
    if (iRet < 0) {
        iMid--;
    }
    else if (iRet == 0) { //游标试图向后<增大>查找是否有与当前m_sBeginNBR相同的记录
        while (iMid < m_iPNSegBillingCnt-1) {
            iMid++;
            if (strcmp(_sAccNBR,m_aPNSegBilling[iMid].m_sBeginNBR)<0) {
                iMid--;
                break;
            }
        }   
    }
    
    // 向前遍历匹配
    for ( ; iMid>=0 ; iMid--) {
        
        int iRetA = strcmp(_sAccNBR,m_aPNSegBilling[iMid].m_sEndNBR);
        //huangxy 20050718 modified,iRetA < 0 --->iRetA <= 0
        if (iRetA <= 0) { // _sAccNBR 在当前号段内
            if(strlen(m_aPNSegBilling[iMid].m_sEndNBR) == strlen(_sAccNBR) &&
            	 strlen(m_aPNSegBilling[iMid].m_sBeginNBR) == strlen(_sAccNBR) )  { ///严格匹配
	            if (strcmp(_sAreaCode, m_aPNSegBilling[iMid].m_sAreaCode)) //区号不一致
	                continue;
	            
	            if (strcmp(_sDate,m_aPNSegBilling[iMid].m_sEffDate)*
	                    strcmp(_sDate,m_aPNSegBilling[iMid].m_sExpDate)<=0 )
	            {
	                _oSeg = m_aPNSegBilling[iMid];
	                return true;
	            }
	        }
	        else 
	        	continue ;
        }
        
        // 判断向前是否还有 m_sEndNBR 不比 _sAccNBR 小的记录
        if (strcmp(_sAccNBR, m_aPNSegBilling[iMid].m_sMaxEndNBRAhead)>0) { //没有
            break;
        }
    }
    return false;
}
#else
bool FormatMethod::getYGPNSegBilling(char *_sAccNBR, char *_sAreaCode,
        char *_sDate, PNSegBilling &_oSeg)
{
	m_iPNSegBillingCnt = G_PPARAMINFO->m_aPNSegBillingData->getCount();
	PNSegBilling *pPNSegBilling = G_PPARAMINFO->m_aPNSegBilling;
	
    int iHead = 1, iTail = m_iPNSegBillingCnt;
    int iMid ;
    if (m_iPNSegBillingCnt == 0) return false;
    // 折半查找, 试图在m_aPNSegBilling中找到一个 m_sBeginNBR == _sAccNBR 的m_aPNSegBilling记录
    // 可能的结果: 
    //     1. 找到, iMid 指向 一个 m_sBeginNBR == _sAccNBR 的m_aPNSegBilling记录
    //     2. 未找到, 可能情况: 
    //          a. iMid 指向最后一个 m_sBeginNBR < _sAccNBR 的m_aPNSegBilling记录
    //          b. iMid 指向最前一个 m_sBeginNBR > _sAccNBR 的m_aPNSegBilling记录
    while (iHead <= iTail) {
        iMid = (iHead + iTail)/2;
        int iRet = strcmp(_sAccNBR, pPNSegBilling[iMid].m_sBeginNBR);
        if (iRet > 0) {
            iHead = iMid+1;
            continue;
        }
        if (iRet < 0) {
            iTail = iMid-1;
            continue;
        }
        //found:
        break;
    }
    
    //目标: 让iMid 指向 最后一个 (m_sBeginNBR <= _sAccNBR) 的 m_aPNSegBilling 记录.
    int iRet = strcmp(_sAccNBR,pPNSegBilling[iMid].m_sBeginNBR);
    if (iRet < 0) {
        iMid--;
    }
    else if (iRet == 0) { //游标试图向后<增大>查找是否有与当前m_sBeginNBR相同的记录
        while (iMid < m_iPNSegBillingCnt) {
            iMid++;
            if (strcmp(_sAccNBR,pPNSegBilling[iMid].m_sBeginNBR)<0) {
                iMid--;
                break;
            }
        }   
    }
    
    // 向前遍历匹配
    for ( ; iMid>0 ; iMid--) {
        
        int iRetA = strcmp(_sAccNBR,pPNSegBilling[iMid].m_sEndNBR);
        //huangxy 20050718 modified,iRetA < 0 --->iRetA <= 0
        if (iRetA <= 0) { // _sAccNBR 在当前号段内
            if(strlen(pPNSegBilling[iMid].m_sEndNBR) == strlen(_sAccNBR) &&
            	 strlen(pPNSegBilling[iMid].m_sBeginNBR) == strlen(_sAccNBR) )  { ///严格匹配
	            if (strcmp(_sAreaCode, pPNSegBilling[iMid].m_sAreaCode)) //区号不一致
	                continue;
	            
	            if (strcmp(_sDate,pPNSegBilling[iMid].m_sEffDate)*
	                    strcmp(_sDate,pPNSegBilling[iMid].m_sExpDate)<=0 )
	            {
	                _oSeg = pPNSegBilling[iMid];
	                return true;
	            }
	        }
	        else 
	        	continue ;
        }
        
        // 判断向前是否还有 m_sEndNBR 不比 _sAccNBR 小的记录
        if (strcmp(_sAccNBR, pPNSegBilling[iMid].m_sMaxEndNBRAhead)>0) { //没有
            break;
        }
    }
    return false;
}
#endif

#ifdef PRIVATE_MEMORY 
bool FormatMethod::getPNSegBilling(char *_sAccNBR, char *_sAreaCode,
        char *_sDate, PNSegBilling &_oSeg)
{
    if (!m_aPNSegBilling)
        loadPNSegBilling();
    
    int iHead = 0, iTail = m_iPNSegBillingCnt-1;
    int iMid ;
    if (m_iPNSegBillingCnt == 0) return false;
    // 折半查找, 试图在m_aPNSegBilling中找到一个 m_sBeginNBR == _sAccNBR 的m_aPNSegBilling记录
    // 可能的结果: 
    //     1. 找到, iMid 指向 一个 m_sBeginNBR == _sAccNBR 的m_aPNSegBilling记录
    //     2. 未找到, 可能情况: 
    //          a. iMid 指向最后一个 m_sBeginNBR < _sAccNBR 的m_aPNSegBilling记录
    //          b. iMid 指向最前一个 m_sBeginNBR > _sAccNBR 的m_aPNSegBilling记录
    while (iHead <= iTail) {
        iMid = (iHead + iTail)/2;
        int iRet = strcmp(_sAccNBR, m_aPNSegBilling[iMid].m_sBeginNBR);
        if (iRet > 0) {
            iHead = iMid+1;
            continue;
        }
        if (iRet < 0) {
            iTail = iMid-1;
            continue;
        }
        //found:
        break;
    }
    
    //目标: 让iMid 指向 最后一个 (m_sBeginNBR <= _sAccNBR) 的 m_aPNSegBilling 记录.
    int iRet = strcmp(_sAccNBR,m_aPNSegBilling[iMid].m_sBeginNBR);
    if (iRet < 0) {
        iMid--;
    }
    else if (iRet == 0) { //游标试图向后<增大>查找是否有与当前m_sBeginNBR相同的记录
        while (iMid < m_iPNSegBillingCnt-1) {
            iMid++;
            if (strcmp(_sAccNBR,m_aPNSegBilling[iMid].m_sBeginNBR)<0) {
                iMid--;
                break;
            }
        }   
    }
    
    // 向前遍历匹配
    for ( ; iMid>=0 ; iMid--) {
        
        int iRetA = strcmp(_sAccNBR,m_aPNSegBilling[iMid].m_sEndNBR);
        //huangxy 20050718 modified,iRetA < 0 --->iRetA <= 0
        if (iRetA <= 0) { // _sAccNBR 在当前号段内
            
            if (strcmp(_sAreaCode, m_aPNSegBilling[iMid].m_sAreaCode)) //区号不一致
                continue;
            
            if (strcmp(_sDate,m_aPNSegBilling[iMid].m_sEffDate)*
                    strcmp(_sDate,m_aPNSegBilling[iMid].m_sExpDate)<=0 )
            {
                _oSeg = m_aPNSegBilling[iMid];
                return true;
            }
            
        }
        
        // 判断向前是否还有 m_sEndNBR 不比 _sAccNBR 小的记录
        if (strcmp(_sAccNBR, m_aPNSegBilling[iMid].m_sMaxEndNBRAhead)>0) { //没有
            break;
        }
    }
    return false;
}
#else //
bool FormatMethod::getPNSegBilling(char *_sAccNBR, char *_sAreaCode,
        char *_sDate, PNSegBilling &_oSeg)
{
	int iHead = 1;  
    m_iPNSegBillingCnt = G_PPARAMINFO->m_aPNSegBillingData->getCount();
	PNSegBilling *pPNSegBilling = G_PPARAMINFO->m_aPNSegBilling;
	int iTail = m_iPNSegBillingCnt;
    int iMid ;
    if (m_iPNSegBillingCnt == 0) return false;
    // 折半查找, 试图在m_aPNSegBilling中找到一个 m_sBeginNBR == _sAccNBR 的m_aPNSegBilling记录
    // 可能的结果: 
    //     1. 找到, iMid 指向 一个 m_sBeginNBR == _sAccNBR 的m_aPNSegBilling记录
    //     2. 未找到, 可能情况: 
    //          a. iMid 指向最后一个 m_sBeginNBR < _sAccNBR 的m_aPNSegBilling记录
    //          b. iMid 指向最前一个 m_sBeginNBR > _sAccNBR 的m_aPNSegBilling记录
    while (iHead <= iTail) {
        iMid = (iHead + iTail)/2;
        int iRet = strcmp(_sAccNBR, pPNSegBilling[iMid].m_sBeginNBR);
        if (iRet > 0) {
            iHead = iMid+1;
            continue;
        }
        if (iRet < 0) {
            iTail = iMid-1;
            continue;
        }
        //found:
        break;
    } 
    //目标: 让iMid 指向 最后一个 (m_sBeginNBR <= _sAccNBR) 的 m_aPNSegBilling 记录.
    int iRet = strcmp(_sAccNBR,pPNSegBilling[iMid].m_sBeginNBR);
    if (iRet < 0) {
        iMid--;
    }
    else if (iRet == 0) { //游标试图向后<增大>查找是否有与当前m_sBeginNBR相同的记录
        while (iMid < m_iPNSegBillingCnt) {
            iMid++;
            if (strcmp(_sAccNBR,pPNSegBilling[iMid].m_sBeginNBR)<0) {
                iMid--;
                break;
            }
        }   
    }
    // 向前遍历匹配
    for ( ; iMid>0 ; iMid--) {     
        int iRetA = strcmp(_sAccNBR,pPNSegBilling[iMid].m_sEndNBR);
        //huangxy 20050718 modified,iRetA < 0 --->iRetA <= 0
        if (iRetA <= 0) { // _sAccNBR 在当前号段内           
            if (strcmp(_sAreaCode, pPNSegBilling[iMid].m_sAreaCode)) //区号不一致
                continue;           
            if (strcmp(_sDate,pPNSegBilling[iMid].m_sEffDate)*
                    strcmp(_sDate,pPNSegBilling[iMid].m_sExpDate)<=0 )
            {
                _oSeg = pPNSegBilling[iMid];
                return true;
            }          
        }    
        // 判断向前是否还有 m_sEndNBR 不比 _sAccNBR 小的记录
        if (strcmp(_sAccNBR, pPNSegBilling[iMid].m_sMaxEndNBRAhead)>0) { //没有
            break;
        }
    }
    return false;
}
#endif
bool FormatMethod::formatYGPNSegAccNBR(EventSection *poEvt)
{
#ifdef  USE_formatPNSegAccNbrEx
    //## 海南要求专网计费时需要限制 交换机/中继 信息
    return SegBillingMgr::formatPNSegAccNbrEx (poEvt);
    //return;
#endif
  
#ifdef  ENABLE_PN_SEG_NOT_BILLING
    /////特殊处理,对于部分号码,虽然在专网号段,但是不需要去替换成专网号码
    if( SegBillingMgr::segNotBilling(
            poEvt->m_sBillingNBR,
            poEvt->m_sBillingAreaCode,
            poEvt->m_sStartDate) )
        return false;
#endif
  
  
    static PNSegBilling oPNSegBilling;
    
    if (getYGPNSegBilling(poEvt->m_sBillingNBR,poEvt->m_sBillingAreaCode,poEvt->m_sStartDate, oPNSegBilling))
    {
//////使用号段标志来判断处理方式
        if(oPNSegBilling.m_iSegFlag==0)
        {//////使用专网计费
            strcpy(poEvt->m_sBillingNBR, oPNSegBilling.m_sAccNBR);
            poEvt->unbindUserInfo();      
            return true;      
        }
        else
        {//////专网和主叫计费方式并存
            if(poEvt->m_iServID <=0 )
            {
                poEvt->unbindUserInfo();
                /////
                if( poEvt->m_sBillingAreaCode[0]=='\0')
                {
                    strcpy(poEvt->m_sBillingAreaCode, poEvt->m_sCallingAreaCode);
                }                    
                if( poEvt->m_sBillingNBR[0]=='\0')
                {
                    strcpy(poEvt->m_sBillingNBR, poEvt->m_sCallingNBR);
                }
                poEvt->bindUserInfo();
            }
            if(poEvt->m_iServID >0 )
            {////有档用户 按照主叫计费
                /////这里不用操作
            }
            else
            {///无档用户 使用专网计费
                strcpy(poEvt->m_sBillingNBR, oPNSegBilling.m_sAccNBR);
                poEvt->unbindUserInfo();      
                return true;             
            }
        }
    }
    return false;
}

bool FormatMethod::formatPNSegAccNBR(EventSection *poEvt)
{
#ifdef  USE_formatPNSegAccNbrEx
    //## 海南要求专网计费时需要限制 交换机/中继 信息
    return SegBillingMgr::formatPNSegAccNbrEx (poEvt);
    //return;
#endif
  
#ifdef  ENABLE_PN_SEG_NOT_BILLING
    /////特殊处理,对于部分号码,虽然在专网号段,但是不需要去替换成专网号码
    if( SegBillingMgr::segNotBilling(
            poEvt->m_sBillingNBR,
            poEvt->m_sBillingAreaCode,
            poEvt->m_sStartDate) )
        return false;
#endif
  
  
    static PNSegBilling oPNSegBilling;
    
    if (getPNSegBilling(poEvt->m_sBillingNBR,poEvt->m_sBillingAreaCode,poEvt->m_sStartDate, oPNSegBilling))
    {
//////使用号段标志来判断处理方式
        if(oPNSegBilling.m_iSegFlag==0)
        {//////使用专网计费
            strcpy(poEvt->m_sBillingNBR, oPNSegBilling.m_sAccNBR);
            poEvt->unbindUserInfo();      
            return true;      
        }
        else
        {//////专网和主叫计费方式并存
            if(poEvt->m_iServID <=0 )
            {
                poEvt->unbindUserInfo();
                /////
                if( poEvt->m_sBillingAreaCode[0]=='\0')
                {
                    strcpy(poEvt->m_sBillingAreaCode, poEvt->m_sCallingAreaCode);
                }                    
                if( poEvt->m_sBillingNBR[0]=='\0')
                {
                    strcpy(poEvt->m_sBillingNBR, poEvt->m_sCallingNBR);
                }
                poEvt->bindUserInfo();
            }
            if(poEvt->m_iServID >0 )
            {////有档用户 按照主叫计费
                /////这里不用操作
            }
            else
            {///无档用户 使用专网计费
                strcpy(poEvt->m_sBillingNBR, oPNSegBilling.m_sAccNBR);
                poEvt->unbindUserInfo();      
                return true;             
            }
        }
    }
    return false;
}


//##ModelId=427B27FD0371
void FormatMethod::loadServiceInfo()
{
    char sMaxEndNBRAhead[MAX_CALLING_NBR_LEN] = {0};
    
    unloadServiceInfo();
    
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL("select count(*) cnt from b_sp_service");
    qry.open();
    qry.next();
    
    m_iServiceInfoCnt = qry.field("cnt").asInteger();
    
    if (!m_iServiceInfoCnt) return;
    
    m_aServiceInfo = new TServiceInfo[m_iServiceInfoCnt];
    if (!m_aServiceInfo)
        THROW(MBC_FormatMethod+9);
    
    qry.close();
    qry.setSQL("select decode(substr(area_code,1,1),'*','*',area_code) area_code, "
        " begin_nbr, end_nbr, service_type_id, party_role_id "
        " from B_SP_SERVICE order by begin_nbr, end_nbr " );
    qry.open();
    
    // 按 begin_nbr 从小到大字符串排序, 放入数组 m_aServiceInfo 中
    for (int iPos=0; qry.next() && (iPos<m_iServiceInfoCnt); iPos++) {
        strcpy(m_aServiceInfo[iPos].m_sAreaCode, qry.field("area_code").asString());
        strcpy(m_aServiceInfo[iPos].m_sBeginNBR, qry.field("begin_nbr").asString());
        strcpy(m_aServiceInfo[iPos].m_sEndNBR, qry.field("end_nbr").asString());
        m_aServiceInfo[iPos].m_iServiceTypeID = qry.field("service_type_id").asInteger();
        m_aServiceInfo[iPos].m_iPartyRoleID = qry.field("party_role_id").asInteger();
        
        strcpy(m_aServiceInfo[iPos].m_sMaxEndNBRAhead,sMaxEndNBRAhead);
        if (strcmp(m_aServiceInfo[iPos].m_sEndNBR, sMaxEndNBRAhead)>0)
            strcpy(sMaxEndNBRAhead, m_aServiceInfo[iPos].m_sEndNBR);
    }
    
    qry.close();
}

//##ModelId=427B28560008
void FormatMethod::unloadServiceInfo()
{
    if (m_aServiceInfo) {
        delete m_aServiceInfo;
        m_aServiceInfo = NULL;
    }
}


//##ModelId=427B286F0267
bool FormatMethod::getServiceInfo(char* _sAccNBR, TServiceInfo &_info, char* _sAreaCode)
{
    if (!m_aServiceInfo)
        loadServiceInfo();
    
    int iHead = 0, iTail = m_iServiceInfoCnt-1;
    int iMid ;
    
    // 折半查找, 试图在m_aServiceInfo中找到一个 m_sBeginNBR == _sAccNBR 的m_aServiceInfo记录
    // 可能的结果: 
    //     1. 找到, iMid 指向 一个 m_sBeginNBR == _sAccNBR 的m_aServiceInfo记录
    //     2. 未找到, 可能情况: 
    //          a. iMid 指向最后一个 m_sBeginNBR < _sAccNBR 的m_aServiceInfo记录
    //          b. iMid 指向最前一个 m_sBeginNBR > _sAccNBR 的m_aServiceInfo记录
    while (iHead <= iTail) {
        iMid = (iHead + iTail)/2;
        int iRet = strcmp(_sAccNBR, m_aServiceInfo[iMid].m_sBeginNBR);
        if (iRet > 0) {
            iHead = iMid+1;
            continue;
        }
        if (iRet < 0) {
            iTail = iMid-1;
            continue;
        }
        //found:
        break;
    }
    
    //目标: 让iMid 指向 最后一个 (m_sBeginNBR <= _sAccNBR) 的 m_aServiceInfo 记录.
    int iRet = strcmp(_sAccNBR,m_aServiceInfo[iMid].m_sBeginNBR);
    if (iRet < 0) {
        iMid--;
    }
    else if (iRet == 0) { //游标试图向后<增大>查找是否有与当前m_sBeginNBR相同的记录
        while (iMid<m_iServiceInfoCnt) {
            iMid++;
            if (strcmp(_sAccNBR,m_aServiceInfo[iMid].m_sBeginNBR)<0) {
                iMid--;
                break;
            }
        }   
    }
    
    // 向前遍历匹配
    for ( ; iMid>=0 ; iMid--) {
        
        int iRetA = strcmp(_sAccNBR,m_aServiceInfo[iMid].m_sEndNBR);
        
        if (iRetA <= 0) { // _sAccNBR 在当前号段内
            
            //如果号段不限制区号,则一定满足条件
            if (m_aServiceInfo[iMid].m_sAreaCode[0]=='*') {
                _info = m_aServiceInfo[iMid];
                return true;
            }
            
            //当前号段需要限制区号
            if (_sAreaCode) { //参数有区号
                if (!strcmp(_sAreaCode, m_aServiceInfo[iMid].m_sAreaCode)) {//区号一致
                    _info = m_aServiceInfo[iMid];
                    return true;
                }
            }               
        }
        
        // 判断向前是否还有 m_sEndNBR 不比 _sAccNBR 小的记录
        if (strcmp(_sAccNBR, m_aServiceInfo[iMid].m_sMaxEndNBRAhead)>0) { //没有
            break;
        }
    }
    return false;
}

bool FormatMethod::getServiceInfo2(int _iLongGrpType, TServiceInfo2 *_pInfo, 
        char* _sNBR, char* _sStartDate, char* _sAreaCode, int iOrgIDRule, int iNetworkID,bool bIsZ)
{
    static char sTemp[100];
    char sAreaCode[20];
    
    if (!m_poOrgMgr) {
        m_poOrgMgr = new OrgMgr;
        if (!m_poOrgMgr)
            THROW(MBC_FormatMethod+10);
    }
    
    LocalHead oLocalHead;
    
    int iOrgID = 0;
    
    //first, find the org_id
    if (_iLongGrpType == LONG_GROUPTYPE_PROV) { //省内长途组类型
        strcpy(sAreaCode,_sAreaCode);
        
        //如果参数定义从资料中取OrgID
        if (iOrgIDRule == 1) {			
			if(bIsZ)
			{
				if(G_PUSERINFO->getServ_Z(G_SERV, _sAreaCode, _sNBR,_sStartDate))
				{
					iOrgID = G_PSERV->getOrgID();
				}
			}else
			{
				if(G_PUSERINFO->getServ_A(G_SERV, _sAreaCode,_sNBR, _sStartDate, iNetworkID))
				{
					iOrgID = G_PSERV->getOrgID();
				}
			}
        }
    }
    else if (_iLongGrpType == LONG_GROUPTYPE_NATION) { //国内长途组类型
        strcpy(sAreaCode,"0");
    }
    else if (_iLongGrpType == 0){ //长途组类型未赋值
        strcpy(sAreaCode,_sAreaCode);
    }
    else { //其他: 即 国际 13国 美加 港澳台 等 长途组类型
        strcpy(sAreaCode,"00");
    }
    
    if (!m_poLocalHeadMgr) {
        m_poLocalHeadMgr = new LocalHeadMgr;
        if (!m_poLocalHeadMgr)
            THROW(MBC_FormatMethod+11);
    }
    
    //if (!iOrgID)
    if (iOrgID <= 0) 
    {
        if (!m_poLocalHeadMgr->searchLocalHead(_sNBR, sAreaCode, _sStartDate,&oLocalHead)) {//找不到号头
            //>> 置错单类型
            iOrgID = 0;
            return false;       
        }    
        iOrgID = oLocalHead.getOrgID();
    }
    
    int iSrcOrgID = iOrgID;    
    
    
    //second, find the ServiceInfo by Org_id.    
    static char sOrgID[10];

    while (1) { //org_id 由当前层向父节点依次查找
        sprintf(sOrgID,"%d",iOrgID);
#ifdef PRIVATE_MEMORY
        if (m_poServiceInfo->get(sOrgID,_pInfo)){
            _pInfo->m_iOrgID = iSrcOrgID; // OrgID 置 深层节点的值
            return true;
        }
#else
		unsigned int iOffset = 0;
		if (G_PPARAMINFO->m_poServiceInfo2Index->get(sOrgID,&iOffset))
		{
			*_pInfo = *(G_PPARAMINFO->m_poServiceInfo2List + iOffset);
			_pInfo->m_iOrgID = iSrcOrgID;// OrgID 置 深层节点的值
            return true;
        }
#endif
        iOrgID = m_poOrgMgr->getParentID(iOrgID);
        
        if (iOrgID == -1)
            break;
    }
    
    //>>未找到业务类型信息,但是要返回OrgID
    _pInfo->m_iOrgID = iSrcOrgID;
    _pInfo->m_iServiceTypeID = 0;
    _pInfo->m_iPartyRoleID = 0;
    
    return true;
}

//##ModelId=427B1EE20076
void FormatMethod::formatServiceInfo(EventSection *_poEventSection)
{
    TServiceInfo obSvcInfo;
    
    if (getServiceInfo(_poEventSection->m_sCallingNBR, obSvcInfo,
            _poEventSection->m_sCallingAreaCode)) {
        _poEventSection->m_iCallingSPTypeID = obSvcInfo.m_iPartyRoleID;
        _poEventSection->m_iCallingServiceTypeID = obSvcInfo.m_iServiceTypeID;
    }
    
    if (getServiceInfo(_poEventSection->m_sCalledNBR, obSvcInfo,
            _poEventSection->m_sCalledAreaCode)) {
        _poEventSection->m_iCalledSPTypeID = obSvcInfo.m_iPartyRoleID;
        _poEventSection->m_iCalledServiceTypeID = obSvcInfo.m_iServiceTypeID;
    }
}


void FormatMethod::formatServiceInfo2_Old(EventSection *_poEvt)
{
	static TServiceInfo2 oSvcInfo;
#ifdef PRIVATE_MEMORY   
    if (!m_poServiceInfo)
        loadServiceInfo2();    
#else
  	unsigned int iOff = 0;
	char *pKey = "m_iCalledOrgIDRule";
	if (G_PPARAMINFO->m_poServiceInfo2Index->get(pKey,&iOff))
	{
		m_iCalledOrgIDRule = (int)iOff;
	} else {
		m_iCalledOrgIDRule = 0;
	}
	
	pKey = "m_iCallingOrgIDRule";
	if (G_PPARAMINFO->m_poServiceInfo2Index->get(pKey,&iOff))
	{
		m_iCallingOrgIDRule = (int)iOff;
	} else {
		m_iCallingOrgIDRule = 0;
	}
#endif    
    if (getServiceInfo2(_poEvt->m_oEventExt.m_iCallingLongGroupTypeID,&oSvcInfo,
            _poEvt->m_sCallingNBR, _poEvt->m_sStartDate, _poEvt->m_sCallingAreaCode, 
            m_iCallingOrgIDRule, _poEvt->m_iNetworkID,false)) {
        _poEvt->m_iCallingSPTypeID = oSvcInfo.m_iPartyRoleID;
        _poEvt->m_iCallingServiceTypeID = oSvcInfo.m_iServiceTypeID;
        _poEvt->m_oEventExt.m_iCallingOrgID = oSvcInfo.m_iOrgID;
    }
    
    if (getServiceInfo2(_poEvt->m_oEventExt.m_iCalledLongGroupTypeID,&oSvcInfo,
            _poEvt->m_sCalledNBR, _poEvt->m_sStartDate, _poEvt->m_sCalledAreaCode,
            m_iCalledOrgIDRule, _poEvt->m_iNetworkID,true)) {
        _poEvt->m_iCalledSPTypeID = oSvcInfo.m_iPartyRoleID;
        _poEvt->m_iCalledServiceTypeID = oSvcInfo.m_iServiceTypeID;
        _poEvt->m_oEventExt.m_iCalledOrgID = oSvcInfo.m_iOrgID;
    }
};
void FormatMethod::formatServiceInfo2(EventSection *_poEvt)
{
    static TServiceInfo2 oSvcInfo;
#ifdef PRIVATE_MEMORY    
    if (!m_poServiceInfo)
        loadServiceInfo2();    
#else
  	unsigned int iOff = 0;
	char *pKey = "m_iCalledOrgIDRule";
	if (G_PPARAMINFO->m_poServiceInfo2Index->get(pKey,&iOff))
	{
		m_iCalledOrgIDRule = (int)iOff;
	} else {
		m_iCalledOrgIDRule = 0;
	}
	
	pKey = "m_iCallingOrgIDRule";
	if (G_PPARAMINFO->m_poServiceInfo2Index->get(pKey,&iOff))
	{
		m_iCallingOrgIDRule = (int)iOff;
	} else {
		m_iCallingOrgIDRule = 0;
	}   
#endif
	char * pBillingAreaCode;
	char * pBillingNBR;
	char * pOtherAreaCode;
	char * pOtherNBR;
	if(CALLTYPE_MTC != _poEvt->m_iCallTypeID)
	{
		pBillingAreaCode = _poEvt->m_sCallingAreaCode;
		pBillingNBR = _poEvt->m_sCallingNBR;
		pOtherAreaCode = _poEvt->m_sCalledAreaCode;
		pOtherNBR = _poEvt->m_sCalledNBR;
	}else
	{
		pBillingAreaCode = _poEvt->m_sCalledAreaCode;
		pBillingNBR = _poEvt->m_sCalledNBR;
		pOtherAreaCode = _poEvt->m_sCallingAreaCode;
		pOtherNBR = _poEvt->m_sCallingNBR;
	}
    
    if (getServiceInfo2(_poEvt->m_oEventExt.m_iCallingLongGroupTypeID,&oSvcInfo,
            pBillingNBR, _poEvt->m_sStartDate, pBillingAreaCode, 
            m_iCallingOrgIDRule, _poEvt->m_iNetworkID,false)) {
        _poEvt->m_iCallingSPTypeID = oSvcInfo.m_iPartyRoleID;
        _poEvt->m_iCallingServiceTypeID = oSvcInfo.m_iServiceTypeID;
        _poEvt->m_oEventExt.m_iCallingOrgID = oSvcInfo.m_iOrgID;
    }
    
    if (getServiceInfo2(_poEvt->m_oEventExt.m_iCalledLongGroupTypeID,&oSvcInfo,
            pOtherNBR, _poEvt->m_sStartDate,pOtherAreaCode,
            m_iCalledOrgIDRule, _poEvt->m_iNetworkID,true)) {
        _poEvt->m_iCalledSPTypeID = oSvcInfo.m_iPartyRoleID;
        _poEvt->m_iCalledServiceTypeID = oSvcInfo.m_iServiceTypeID;
        _poEvt->m_oEventExt.m_iCalledOrgID = oSvcInfo.m_iOrgID;
    }   
}

//##ModelId=427B20FD028E
void FormatMethod::formatRoamAndLongType(EventSection *_poEventSection)
{
    //if (_poEventSection->m_iCallTypeID <= 2 && strlen(_poEventSection->m_sBillingVisitAreaCode)==0)
    if (strlen(_poEventSection->m_sBillingVisitAreaCode)==0)
        strcpy(_poEventSection->m_sBillingVisitAreaCode, _poEventSection->m_sCallingAreaCode);
    
    //如果计费方号码为空,置默认计费方号码/区号(将主叫,主叫区号赋值给计费方,计费方区号)
    if (_poEventSection->m_sBillingNBR[0]==0) {
        strcpy(_poEventSection->m_sBillingNBR, _poEventSection->m_sCallingNBR);
        strcpy(_poEventSection->m_sBillingAreaCode, _poEventSection->m_sCallingAreaCode);
        _poEventSection->unbindUserInfo();
    }
    
    getRoamType(_poEventSection);

    if (_poEventSection->m_iCallTypeID <= 1) { //主叫话单
        
        //主叫到访地区号!=被叫归属地区号 && Event.漫游类型!="同城漫游"
        if (strcmp(_poEventSection->m_sBillingVisitAreaCode,_poEventSection->m_sCalledAreaCode)
                && _poEventSection->m_oEventExt.m_iRoamTypeID != ROAMTYPE_ROAM_SAME) 
        {
            //求具体的长途类型
            getLongType(_poEventSection,_poEventSection->m_oEventExt.m_iCalledLongGroupTypeID);
        }
        else {
            _poEventSection->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN; //长途类型-市话
        }
    }
    else //if (_poEventSection->m_iCallTypeID == 2)
    { //被叫话单
        //主叫到访地区号!=被叫归属地区号 && Event.漫游类型!="同城漫游"
        if (strcmp(_poEventSection->m_sBillingVisitAreaCode,_poEventSection->m_sCalledAreaCode)
                && _poEventSection->m_oEventExt.m_iRoamTypeID != ROAMTYPE_ROAM_SAME) 
        {
            //求具体的长途类型
            getLongType(_poEventSection,_poEventSection->m_oEventExt.m_iCalledLongGroupTypeID);
        }
        else {
            _poEventSection->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN; //长途类型-市话
        }       
        //getLongType(_poEventSection,_poEventSection->m_oEventExt.m_iCalledLongGroupTypeID);
/*      if (被叫到访地区号!=被叫归属地区号 && Event.漫游类型!="同城漫游")
            Event.长途类型 = "长途";
        Else
            Event.长途类型 = "市话";
*/  
    }
}

#ifdef PRIVATE_MEMORY
int FormatMethod::filter(EventSection *_poEventSection)
{
    if (!m_poFilterRule) {
        m_poFilterRule = new FilterRule ();
        m_poFilterRule->load ();
    }

    return m_poFilterRule->filter ((StdEvent *)_poEventSection);
}
#else
int FormatMethod::filter(EventSection *_poEventSection)
{
    FilterRuleCheckValue mFilterRuleCheckValue;
    return mFilterRuleCheckValue.filter ((StdEvent *)_poEventSection);
}
#endif

int FormatMethod::customFilter(EventSection *_poEventSection)
{
    if (!m_poCustomFilter) {
        m_poCustomFilter = new CustomFilter ();
        m_poCustomFilter->load ();
    }

    return m_poCustomFilter->filter ((StdEvent *)_poEventSection);
}


//##ModelId=427C9762021B
void FormatMethod::getLongType(EventSection *_poEventSection, int _iLongGroupType)
{
    if (LongTypeMgr::getLongType (_poEventSection))
        return;
    
    switch (_iLongGroupType) {
      case  LONG_GROUPTYPE_PROV :
        _poEventSection->m_oEventExt.m_iLongTypeID = LONGTYPE_PROV ;
        break;
      case  LONG_GROUPTYPE_NATION :
        _poEventSection->m_oEventExt.m_iLongTypeID = LONGTYPE_NATION ;
        break;
      case  LONG_GROUPTYPE_INTER :
        _poEventSection->m_oEventExt.m_iLongTypeID = LONGTYPE_INTER ;
        break;
      case  LONG_GROUPTYPE_THIRT :
        _poEventSection->m_oEventExt.m_iLongTypeID = LONGTYPE_THIRT ;
        break;
      case  LONG_GROUPTYPE_AMERC :
        _poEventSection->m_oEventExt.m_iLongTypeID = LONGTYPE_AMERC ;
        break;

    // b_area_code 表中区号对应的长途组类型可能会把港、澳、台分开
      case  LONG_GROUPTYPE_KONG : //港澳台长途组类型
        _poEventSection->m_oEventExt.m_iLongTypeID = LONGTYPE_KONG ;
        break;

      case  LONG_GROUPTYPE_XK :   //香港长途组类型
        _poEventSection->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN_XK ;
        break;
      case  LONG_GROUPTYPE_AM :   //澳门长途组类型
        _poEventSection->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN_AM ;
        break;
      case  LONG_GROUPTYPE_TW :   //台湾长途组类型
        _poEventSection->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN_TW ;
        break;
      case  LONG_GROUPTYPE_HS :   //台湾长途组类型
        _poEventSection->m_oEventExt.m_iLongTypeID = LONGTYPE_LATN_HS ;
        break;
    }
}


//##ModelId=427C96D00121
void FormatMethod::getRoamType(EventSection *_poEventSection)
{
    // 计费方到访地区号 == 计费方归属地区号
    if (!strcmp(_poEventSection->m_sBillingVisitAreaCode,_poEventSection->m_sBillingAreaCode)) {
        _poEventSection->m_oEventExt.m_iRoamTypeID = ROAMTYPE_NOT_ROAM; //漫游类型-非漫游
        return;
    }
    else {
        //_poEventSection->m_oEventExt.m_iRoamTypeID = ROAMTYPE_ROAM; //漫游类型-漫游
    }
    
    //判定边界漫游,规则尚未定  //>...
    if (0) {
        _poEventSection->m_oEventExt.m_iRoamTypeID = ROAMTYPE_ROAM_BORD; //漫游类型-边界漫游
        
        // 计费到访地区号 <- 计费归属地区号
        strcpy(_poEventSection->m_sBillingVisitAreaCode,_poEventSection->m_sBillingAreaCode);
        return;
    }
    
    //判断同城漫游特例 //>...
    //主叫方到访地区号(是主叫还是计费方?)==被叫到访地区号 && 被叫到访地区号!=被叫归属地区号
    if (!strcmp(_poEventSection->m_sBillingVisitAreaCode,"" ) 
            && strcmp("","" )) {
        _poEventSection->m_oEventExt.m_iRoamTypeID = ROAMTYPE_ROAM_SAME; //漫游类型-同城漫游
        return;
    }
    
    //>... 具体的漫游类型 可由 前台规整规则指定
}


//##ModelId=4281CFED0068
bool FormatMethod::formatLongEventType(EventSection *_poEventSection)
{
    LongEventTypeRule *pRule;
    
    if (LongEventTypeRuleMgr::getRule(_poEventSection,&pRule)) {

        #ifdef RECORD_EVENT_RULE
            sprintf(g_sFormatRecord,"%s\n LongID:%d",g_sFormatRecord,pRule->getRuleID());
        #endif
            
        _poEventSection->m_iEventTypeID = pRule->getOutEventTypeID();
    }
    return true;
}

//##ModelId=4281D05B028D
bool FormatMethod::formatLocalEventType(EventSection *_poEventSection)
{
    LocalEventTypeRule *pRule;
    
    if (LocalEventTypeRuleMgr::getRule(_poEventSection,&pRule)) {
        
        #ifdef RECORD_EVENT_RULE
            sprintf(g_sFormatRecord,"%s\n LocalID:%d",g_sFormatRecord,pRule->getRuleID());
        #endif
        
        _poEventSection->m_iEventTypeID = pRule->getOutEventTypeID();
    }
    return true;
}

bool  FormatMethod::getPhysicsNbr(EventSection *_poEventSection)
{
    char const * OrgCallingphNBR;
    char const * OrgCalledphNBR;
    char const * ThirdPartyphNBR;
    
    OrgCallingphNBR=G_PUSERINFO->convertAccNBR(_poEventSection->m_sOrgCallingNBR,25,1,_poEventSection->m_sStartDate);
    OrgCalledphNBR=G_PUSERINFO->convertAccNBR(_poEventSection->m_sOrgCalledNBR,25,1,_poEventSection->m_sStartDate);
    ThirdPartyphNBR=G_PUSERINFO->convertAccNBR(_poEventSection->m_sThirdPartyNBR,25,1,_poEventSection->m_sStartDate);
    

    if(_poEventSection->m_sOrgCallingPhysicalNBR[0]=='\0')
    {
        if(OrgCallingphNBR!=NULL)
        {
            strcpy(_poEventSection->m_sOrgCallingPhysicalNBR,OrgCallingphNBR);
        }
        else  
        {
            strcpy(_poEventSection->m_sOrgCallingPhysicalNBR,_poEventSection->m_sOrgCallingNBR);
        }
    }    
      

    if(_poEventSection->m_sOrgCalledPhysicalNBR[0]=='\0')
    { 
        if(OrgCalledphNBR!=NULL)
        {
            strcpy(_poEventSection->m_sOrgCalledPhysicalNBR,OrgCalledphNBR);
        }
        else 
        {
            strcpy(_poEventSection->m_sOrgCalledPhysicalNBR,_poEventSection->m_sOrgCalledNBR);
        }
    }
      
    
    if(_poEventSection->m_sThirdPartyPhysicalNBR[0]=='\0')
    {
        if(ThirdPartyphNBR!=NULL)
        {
            strcpy(_poEventSection->m_sThirdPartyPhysicalNBR,ThirdPartyphNBR);
        }
        else 
        {
            strcpy(_poEventSection->m_sThirdPartyPhysicalNBR,_poEventSection->m_sThirdPartyNBR);
        }
    }

    strcpy(_poEventSection->m_sCallingPhysicalNBR,_poEventSection->m_sOrgCallingPhysicalNBR);
    strcpy(_poEventSection->m_sCalledPhysicalNBR,_poEventSection->m_sOrgCalledPhysicalNBR);
      
    return true; 
}


//## 将主叫号码从物理号码转换成逻辑号码 (源自海南需求)
void FormatMethod::convertPhyNbrToLogicNbr (EventSection *pEvt)
{
    if (!pEvt || !pEvt->m_sCallingNBR[0])
        return;
    
    if (!G_PUSERINFO->getServ (G_SERV, pEvt->m_sCallingNBR, pEvt->m_sStartDate, 
            pEvt->m_iNetworkID, true))
        return;
    
    static char const *pNbr;
    
    pNbr = G_SERV.getAccNBR (25, pEvt->m_sStartDate);
    
    if (!pNbr) return;
    
    strcpy (pEvt->m_sCallingNBR, pNbr);
}

//## 将主叫号码从移动号码转换成逻辑号码 (源自海南需求)
void FormatMethod::convertMobNbrToLogicNbr (EventSection *pEvt)
{
    if (!pEvt || !pEvt->m_sCallingNBR[0])
        return;
    
    if (!G_PUSERINFO->getServ (G_SERV, pEvt->m_sCallingNBR, pEvt->m_sStartDate, 
            pEvt->m_iNetworkID))
        return;
    
    static char const *pNbr;
    
    pNbr = G_SERV.getAccNBR (25, pEvt->m_sStartDate);
    
    if (!pNbr) return;
    
    strcpy (pEvt->m_sCallingNBR, pNbr);
}

//初始化中继计费
void FormatMethod::loadTrunkBilling()
{
    unloadTrunkBilling();
    m_poTrunkBilling = new KeyTree<TrunkBilling>;
    
    DEFINE_QUERY(qry);
        
    qry.close();

#ifdef  DEF_SICHUAN
//////四川的需要用到采集点来中继计费

    qry.setSQL(" select switch_id, gather_pot, trunk_code, "
        " to_char(eff_date,'yyyymmddhh24miss') eff_date, "
        " to_char(exp_date,'yyyymmddhh24miss') exp_date, "
        " acc_nbr, to_number(trunk_side) trunk_side, nvl(switch_id,-1)||'+'|| nvl(gather_pot,-1) || '+' ||trunk_code skey "
        " from b_trunk_billing order by switch_id " );
  
    qry.open();
    
    while (qry.next()) {
        TrunkBilling oTrunkBilling;
        char sKey[30]; memset(sKey, 0, sizeof(sKey));
        oTrunkBilling.m_iSwitch_id = qry.field("switch_id").asInteger();
        oTrunkBilling.m_iGatherPot = qry.field("gather_pot").asInteger();//////gather_pot
        strcpy(oTrunkBilling.m_sTrunk_Code, qry.field("trunk_code").asString());
        strcpy(oTrunkBilling.m_sEffDate, qry.field("eff_date").asString());
        strcpy(oTrunkBilling.m_sExpDate, qry.field("exp_date").asString());
        strcpy(oTrunkBilling.m_sAccNBR, qry.field("acc_nbr").asString());
        oTrunkBilling.m_iTrunkSide = qry.field("trunk_side").asInteger();
        strcpy(sKey, qry.field("skey").asString());
        
        m_poTrunkBilling->add(sKey, oTrunkBilling);
    }    


  #else     

    qry.setSQL(" select  switch_id, trunk_code, "
        " to_char(eff_date,'yyyymmddhh24miss') eff_date, "
        " to_char(exp_date,'yyyymmddhh24miss') exp_date, "
        " acc_nbr, to_number(trunk_side) trunk_side, switch_id||'+'||trunk_code skey "
        " from b_trunk_billing order by switch_id " );
    qry.open();
    
    while (qry.next()) {
        TrunkBilling oTrunkBilling;
        char sKey[30]; memset(sKey, 0, sizeof(sKey));
        oTrunkBilling.m_iSwitch_id = qry.field(0).asInteger();
        strcpy(oTrunkBilling.m_sTrunk_Code, qry.field(1).asString());
        strcpy(oTrunkBilling.m_sEffDate, qry.field(2).asString());
        strcpy(oTrunkBilling.m_sExpDate, qry.field(3).asString());
        strcpy(oTrunkBilling.m_sAccNBR, qry.field(4).asString());
        oTrunkBilling.m_iTrunkSide = qry.field(5).asInteger();
        strcpy(sKey, qry.field(6).asString());
        
        m_poTrunkBilling->add(sKey, oTrunkBilling);
    }
    
    
  #endif

   
    qry.close();
}


void FormatMethod::unloadTrunkBilling()
{
    if (m_poTrunkBilling) {
        delete m_poTrunkBilling;
        m_poTrunkBilling = NULL;
    }
}

#ifdef PRIVATE_MEMORY
bool FormatMethod::getTrunkBilling(int _iSwitchID, char *_sTrunkInCode, char *_sTrunkOutCode,char *_sDate,TrunkBilling *_oTrunk)
{
    if(!m_poTrunkBilling)
        loadTrunkBilling();
    
    bool bRet = false;
    char sKey[30];
    
    //先用入中继查找
    sprintf(sKey, "%d+%s", _iSwitchID, _sTrunkInCode);    
    bRet = m_poTrunkBilling->get(sKey, _oTrunk);

    if (bRet) 
    {   
        if (strcmp(_sDate,_oTrunk->m_sEffDate)>=0 && strcmp(_sDate,_oTrunk->m_sExpDate) < 0 )
        {
            //双向中继或入中继
            if (_oTrunk->m_iTrunkSide == 1 || _oTrunk->m_iTrunkSide == 2)
                return true;
        }
    }               

    //找不到，再用出中继查找
    sprintf(sKey,"%d+%s", _iSwitchID, _sTrunkOutCode);    
    bRet = m_poTrunkBilling->get(sKey, _oTrunk);

    if (bRet) 
    {   
        if (strcmp(_sDate,_oTrunk->m_sEffDate)>=0 && strcmp(_sDate,_oTrunk->m_sExpDate) < 0 )
        {
            //双向中继或出中继
            if (_oTrunk->m_iTrunkSide == 1 || _oTrunk->m_iTrunkSide == 3)
                return true;
        }
    }   
    
    return false;
}
#else
bool FormatMethod::getTrunkBilling(int _iSwitchID, char *_sTrunkInCode, char *_sTrunkOutCode,char *_sDate,TrunkBilling *_oTrunk)
{
    bool bRet = false;
    char sKey[30];
    
    //先用入中继查找
    sprintf(sKey, "%d+%s", _iSwitchID, _sTrunkInCode);    
	unsigned int iOffset = 0;    
    bRet = G_PPARAMINFO->m_poTrunkBillingIndex->get(sKey, &iOffset);
    if (bRet) 
    {   
		*_oTrunk = *(G_PPARAMINFO->m_poTrunkBillingList + iOffset);
        if (strcmp(_sDate,_oTrunk->m_sEffDate)>=0 && strcmp(_sDate,_oTrunk->m_sExpDate) < 0 )
        {
            //双向中继或入中继
            if (_oTrunk->m_iTrunkSide == 1 || _oTrunk->m_iTrunkSide == 2)
                return true;
        }
    }               
    //找不到，再用出中继查找
    sprintf(sKey,"%d+%s", _iSwitchID, _sTrunkOutCode);    
    bRet = G_PPARAMINFO->m_poTrunkBillingIndex->get(sKey, &iOffset);
    if (bRet) 
    {   
		*_oTrunk = *(G_PPARAMINFO->m_poTrunkBillingList + iOffset);
        if (strcmp(_sDate,_oTrunk->m_sEffDate)>=0 && strcmp(_sDate,_oTrunk->m_sExpDate) < 0 )
        {
            //双向中继或出中继
            if (_oTrunk->m_iTrunkSide == 1 || _oTrunk->m_iTrunkSide == 3)
                return true;
        }
    }    
    return false;
}
#endif



////四川需要使用采集点来中继计费(替换号码)
#ifdef PRIVATE_MEMORY
bool FormatMethod::getTrunkBilling(int _iSwitchID, int _iGatherPot,
    char *_sTrunkInCode, char *_sTrunkOutCode,char *_sDate, TrunkBilling *_oTrunk)
{
    if(!m_poTrunkBilling)
        loadTrunkBilling();
    
    bool bRet = false;
    char sKey[30];

////先使用交换机+采集点+中继.然后单独用交换机,再单独用采集点,
    
    //先用入中继查找
    sprintf(sKey, "%d+%d+%s", _iSwitchID, _iGatherPot,  _sTrunkInCode);     
    bRet = m_poTrunkBilling->get(sKey, _oTrunk);
    
   /* if(bRet==false)
        {
        sprintf(sKey, "%d+%d+%s", _iSwitchID, -1,   _sTrunkInCode);      
        bRet = m_poTrunkBilling->get(sKey, _oTrunk);
        if(bRet==false)
            {
            sprintf(sKey, "%d+%d+%s",-1,  _iGatherPot,   _sTrunkInCode);    
            bRet = m_poTrunkBilling->get(sKey, _oTrunk);        
            }
        }
    */

    if (bRet) 
    {   
        if (strcmp(_sDate,_oTrunk->m_sEffDate)>=0 && strcmp(_sDate,_oTrunk->m_sExpDate) < 0 )
        {
            //双向中继或入中继
            if (_oTrunk->m_iTrunkSide == 1 || _oTrunk->m_iTrunkSide == 2)
                return true;
        }
    }               


    //找不到，再用出中继查找
    sprintf(sKey, "%d+%d+%s", _iSwitchID, _iGatherPot,  _sTrunkOutCode);     
    bRet = m_poTrunkBilling->get(sKey, _oTrunk);
    /*
    if(bRet==false)
        {
        sprintf(sKey, "%d+%d+%s", _iSwitchID,  -1,  _sTrunkOutCode);      
        bRet = m_poTrunkBilling->get(sKey, _oTrunk);
        if(bRet==false)
            {
            sprintf(sKey, "%d+%d+%s", -1, _iGatherPot,  _sTrunkOutCode);    
            bRet = m_poTrunkBilling->get(sKey, _oTrunk);        
            }
        }

    */

    if (bRet) 
    {   
        if (strcmp(_sDate,_oTrunk->m_sEffDate)>=0 && strcmp(_sDate,_oTrunk->m_sExpDate) < 0 )
        {
            //双向中继或出中继
            if (_oTrunk->m_iTrunkSide == 1 || _oTrunk->m_iTrunkSide == 3)
                return true;
        }
    }   

    
    return false;
}
#else
bool FormatMethod::getTrunkBilling(int _iSwitchID, int _iGatherPot,
    char *_sTrunkInCode, char *_sTrunkOutCode,char *_sDate, TrunkBilling *_oTrunk)
{    
    bool bRet = false;
    char sKey[30];

////先使用交换机+采集点+中继.然后单独用交换机,再单独用采集点,
    
    //先用入中继查找
	unsigned int iOffset = 0; 
    sprintf(sKey, "%d+%d+%s", _iSwitchID, _iGatherPot,  _sTrunkInCode);     
    bRet = G_PPARAMINFO->m_poTrunkBillingIndex->get(sKey, &iOffset);

    if (bRet) 
    {   
		*_oTrunk = *(G_PPARAMINFO->m_poTrunkBillingList + iOffset);
        if (strcmp(_sDate,_oTrunk->m_sEffDate)>=0 && strcmp(_sDate,_oTrunk->m_sExpDate) < 0 )
        {
            //双向中继或入中继
            if (_oTrunk->m_iTrunkSide == 1 || _oTrunk->m_iTrunkSide == 2)
                return true;
        }
    }               

    //找不到，再用出中继查找
    sprintf(sKey, "%d+%d+%s", _iSwitchID, _iGatherPot,  _sTrunkOutCode);     
    bRet = G_PPARAMINFO->m_poTrunkBillingIndex->get(sKey, &iOffset);

    if (bRet) 
    {   
		*_oTrunk = *(G_PPARAMINFO->m_poTrunkBillingList + iOffset);
        if (strcmp(_sDate,_oTrunk->m_sEffDate)>=0 && strcmp(_sDate,_oTrunk->m_sExpDate) < 0 )
        {
            //双向中继或出中继
            if (_oTrunk->m_iTrunkSide == 1 || _oTrunk->m_iTrunkSide == 3)
                return true;
        }
    }   
    return false;
}
#endif




bool FormatMethod::formatTrunkAccNBR(EventSection *_poEvt)
{

    TrunkBilling oTrunkBilling;

#ifdef  DEF_SICHUAN    
////// _poEvt->m_iOffers[3],
if( getTrunkBilling(
    _poEvt->m_iSwitchID,atoi( _poEvt->m_sReservedField[3] ), 
    _poEvt->m_sTrunkIn,_poEvt->m_sTrunkOut,_poEvt->m_sStartDate, &oTrunkBilling)  )
{
    strcpy(_poEvt->m_sBillingNBR, oTrunkBilling.m_sAccNBR);
    _poEvt->unbindUserInfo();
    
    return true;
}

#else

    if (getTrunkBilling(_poEvt->m_iSwitchID,_poEvt->m_sTrunkIn,_poEvt->m_sTrunkOut,_poEvt->m_sStartDate, &oTrunkBilling))
    {
       strcpy(_poEvt->m_sBillingNBR, oTrunkBilling.m_sAccNBR);
       _poEvt->unbindUserInfo();
       
        #ifdef DEF_SHANXI
		#ifdef PRIVATE_MEMORY
        static HashList<int> *  pHSpecProduct=NULL;
        if(!pHSpecProduct){
            pHSpecProduct = new HashList<int>(20);
            DEFINE_QUERY(qry);
            qry.close();
            qry.setSQL("select product_id, state from b_spec_trunk_product");
            qry.open();
        	while( qry.next() ){
        	    if( qry.field(1).asString()[2] == 'A'){
        	        pHSpecProduct->add( qry.field(0).asInteger(), 1 );
        	    }
        	}
        	qry.close();
            	
        }        
            //add by fangzj 2006.12.13
            //除回拨外的话批、长途直联业务：以计费号码的营业区来判断通话类型[目前：oTrunkBilling.m_sAccNBR带区号]
            int    productId=0;
            int    iTemp=0;
            if (G_PUSERINFO->getServ (G_SERV, oTrunkBilling.m_sAccNBR, _poEvt->m_sStartDate)) 
            {   
                //主产品非回拨业务,主叫区号取计费号区号,主叫号码取计费号码
                productId=G_SERV.getProductID(_poEvt->m_sStartDate);    
                ////if ((productId!=-1)&&(productId!=708))    
                if ((productId!=-1)&&(! pHSpecProduct->get(productId, &iTemp) ) )
                {  
                   //置主叫区号、主叫号码、主叫长途组(强置成省内长途)     
                   strncpy(_poEvt->m_sCallingAreaCode, oTrunkBilling.m_sAccNBR,4);
                   _poEvt->m_sCallingAreaCode[4]='\0';
                   
                   strcpy(_poEvt->m_sCallingNBR,oTrunkBilling.m_sAccNBR+4);
                   
                   _poEvt->m_oEventExt.m_iCallingLongGroupTypeID=10;
                }
            }
		#else
			//除回拨外的话批、长途直联业务：以计费号码的营业区来判断通话类型[目前：oTrunkBilling.m_sAccNBR带区号]
            int    productId=0;
            int    iTemp=0;
			unsigned int    iSHMTemp=0;
            if (G_PUSERINFO->getServ (G_SERV, oTrunkBilling.m_sAccNBR, _poEvt->m_sStartDate)) 
            {   
                //主产品非回拨业务,主叫区号取计费号区号,主叫号码取计费号码
                productId=G_SERV.getProductID(_poEvt->m_sStartDate);    
                ////if ((productId!=-1)&&(productId!=708))    
                if ((productId!=-1)&&(! G_PPARAMINFO->m_poSpecTrunkProductIndex->get(productId, &iSHMTemp) ) )
                {  
                   //置主叫区号、主叫号码、主叫长途组(强置成省内长途)     
                   strncpy(_poEvt->m_sCallingAreaCode, oTrunkBilling.m_sAccNBR,4);
                   _poEvt->m_sCallingAreaCode[4]='\0';
                   
                   strcpy(_poEvt->m_sCallingNBR,oTrunkBilling.m_sAccNBR+4);
                   
                   _poEvt->m_oEventExt.m_iCallingLongGroupTypeID=10;
                }
            }
		#endif
        #endif  
            
        #ifdef DEF_LIAONING
            //原始主叫作为主叫号码
            strcpy(_poEvt->m_sCallingNBR, _poEvt->m_sOrgCallingNBR);

            //中继号码的区号作为主叫区号
            if (strncmp(oTrunkBilling.m_sAccNBR, "024", 3)==0)
            {
                strncpy(_poEvt->m_sCallingAreaCode, oTrunkBilling.m_sAccNBR, 3);
                _poEvt->m_sCallingAreaCode[3]='\0';
            }   
            else
            {
                strncpy(_poEvt->m_sCallingAreaCode, oTrunkBilling.m_sAccNBR, 4);
                _poEvt->m_sCallingAreaCode[4]='\0';
            }   

            _poEvt->m_oEventExt.m_iCallingLongGroupTypeID=10;
        #endif  
                
        return true;
    }
#endif

    return false;
}

void FormatMethod::unloadSwitchItemType()
{
    if (m_poSwitchItemTypeTree) {
        delete m_poSwitchItemTypeTree;
        m_poSwitchItemTypeTree = NULL;
    }
}

void FormatMethod::loadSwitchItemType()
{
    unloadSwitchItemType();
    
    m_poSwitchItemTypeTree = new KeyTree<SwitchItemType>;
    
    DEFINE_QUERY(qry);
    
    qry.close();
    qry.setSQL(" select switch_id||'+'||nvl(tariff_type,-1)||bill_mode skey, switch_id,  "
        " nvl(tariff_type,-1) tariff_type, event_type_id, acct_item_type_id, bill_mode "
        " from b_switch_item_type ");
    qry.open();
    
    while (qry.next()) {
        SwitchItemType oSwitchItemType;
        char sKey[30];
        oSwitchItemType.m_iSwitchID = qry.field("switch_id").asInteger();
        oSwitchItemType.m_iTariffType = qry.field("tariff_type").asInteger();
        oSwitchItemType.m_iEventTypeID = qry.field("event_type_id").asInteger();
        oSwitchItemType.m_iAcctItemTypeID = qry.field("acct_item_type_id").asInteger();
        oSwitchItemType.m_iBillMode = qry.field("bill_mode").asInteger();       
        strcpy(sKey, qry.field("skey").asString());

        m_poSwitchItemTypeTree->add(sKey, oSwitchItemType);
    }
    
    qry.close();
}

#ifdef PRIVATE_MEMORY
void FormatMethod::formatSwitchItem(EventSection *_pEvt)
{
    if (!m_poSwitchItemTypeTree)
        loadSwitchItemType();
            
    char sKey[100] = {0};   
    bool bRet = false;
    SwitchItemType oSwitchItemType;

    //保留字段2都设置为tariff_type
    sprintf(sKey,"%d+%s%s", _pEvt->m_iSwitchID, _pEvt->m_sReservedField[1], _pEvt->m_sReservedField[0]);

    bRet = m_poSwitchItemTypeTree->get(sKey, &oSwitchItemType);

    if (bRet) {
        _pEvt->m_iEventTypeID = oSwitchItemType.m_iEventTypeID;
        _pEvt->m_oChargeInfo[0].m_iAcctItemTypeID = oSwitchItemType.m_iAcctItemTypeID;
        return;
    } 
    else  //用空的费率类型查
    {
        sprintf(sKey,"%d+-1%s", _pEvt->m_iSwitchID, _pEvt->m_sReservedField[0]);
    
        bRet = m_poSwitchItemTypeTree->get(sKey, &oSwitchItemType);
        
        if (bRet) {
            _pEvt->m_iEventTypeID = oSwitchItemType.m_iEventTypeID;
            _pEvt->m_oChargeInfo[0].m_iAcctItemTypeID = oSwitchItemType.m_iAcctItemTypeID;
        }   
    }
    return;
}
#else
void FormatMethod::formatSwitchItem(EventSection *_pEvt)
{
    char sKey[100] = {0};   
    bool bRet = false;
    SwitchItemType oSwitchItemType;

    //保留字段2都设置为tariff_type
    sprintf(sKey,"%d+%s%s", _pEvt->m_iSwitchID, _pEvt->m_sReservedField[1], _pEvt->m_sReservedField[0]);
	unsigned int iOffset = 0;
    bRet = G_PPARAMINFO->m_poSwitchItemTypeIndex->get(sKey, &iOffset);

    if (bRet) {
		oSwitchItemType = *(G_PPARAMINFO->m_poSwitchItemTypeList + iOffset);
        _pEvt->m_iEventTypeID = oSwitchItemType.m_iEventTypeID;
        _pEvt->m_oChargeInfo[0].m_iAcctItemTypeID = oSwitchItemType.m_iAcctItemTypeID;
        return;
    } 
    else  //用空的费率类型查
    {
        sprintf(sKey,"%d+-1%s", _pEvt->m_iSwitchID, _pEvt->m_sReservedField[0]);
    
        bRet = G_PPARAMINFO->m_poSwitchItemTypeIndex->get(sKey, &iOffset);
        
        if (bRet) {
			oSwitchItemType = *(G_PPARAMINFO->m_poSwitchItemTypeList + iOffset);
            _pEvt->m_iEventTypeID = oSwitchItemType.m_iEventTypeID;
            _pEvt->m_oChargeInfo[0].m_iAcctItemTypeID = oSwitchItemType.m_iAcctItemTypeID;
        }   
    }
    return;
}
#endif


void FormatMethod::loadLocalAreaCode()
{
    DEFINE_QUERY (qry) ;
    m_strLocalAreaCode.clear();

    //获得SQL语句
    string sSql = "select area_code from b_area_code where long_group_type_id=10 order by area_code";

    qry.setSQL (sSql.c_str());
    qry.open ();

    while(qry.next ())
        m_strLocalAreaCode.push_back(qry.field(0).asString());

    qry.close ();
}

#ifdef PRIVATE_MEMORY
void FormatMethod::formatLocalAreaCode(EventSection *_poEventSection)
{
    vector<string>::iterator itr;
    char sTemp[33];
    
    if (m_strLocalAreaCode.empty())
        loadLocalAreaCode();
    
    //循环用区号+号码查找资料
    for (itr=m_strLocalAreaCode.begin(); itr!=m_strLocalAreaCode.end(); itr++)
    {
        strcpy(sTemp, (*itr).c_str());
        strcat(sTemp, _poEventSection->m_sBillingNBR);
    
        //找到资料，将区号赋值计费区号
        if (G_PUSERINFO->getServ (G_SERV, sTemp, _poEventSection->m_sStartDate, _poEventSection->m_iNetworkID)) {
            strcpy(_poEventSection->m_sBillingAreaCode, (*itr).c_str());
            _poEventSection->unbindUserInfo();
            break;
        }
    }
}
#else
void FormatMethod::formatLocalAreaCode(EventSection *_poEventSection)
{
    char sTemp[33];
    
    if (G_PPARAMINFO->m_poStrLocalAreaCodeData->getCount() == 0)
		return;
    
    //循环用区号+号码查找资料
	unsigned int i = 1;
    for (; i<=G_PPARAMINFO->m_poStrLocalAreaCodeData->getCount(); i++)
    {
        strcpy(sTemp, (G_PPARAMINFO->m_poStrLocalAreaCodeList + i)->m_sAreaCode);
        strcat(sTemp, _poEventSection->m_sBillingNBR);
    
        //找到资料，将区号赋值计费区号
        if (G_PUSERINFO->getServ (G_SERV, sTemp, _poEventSection->m_sStartDate, _poEventSection->m_iNetworkID)) {
            strcpy(_poEventSection->m_sBillingAreaCode, (G_PPARAMINFO->m_poStrLocalAreaCodeList + i)->m_sAreaCode);
            _poEventSection->unbindUserInfo();
            break;
        }
    }
}
#endif


//add by yangks 2006.12.31  增加跳次计算功能(河北需求)
void FormatMethod::loadTimesType()
{
    unloadTimesType();
    
    m_poTimesType = new KeyTree<int>;
    
    DEFINE_QUERY(qry);
    
    int iEventTypeID;
    int iTimesType;
    
    qry.close();
    qry.setSQL("select event_type_id,times_type from b_times_type ");
    qry.open();
    
    while (qry.next()) 
    {
        iEventTypeID = qry.field("event_type_id").asInteger();
        iTimesType = qry.field("times_type").asInteger();
        m_poTimesType->add(iEventTypeID,iTimesType);
    }
    
    qry.close();

}

//add by yangks 2006.12.31  增加跳次计算功能(河北需求)
void FormatMethod::unloadTimesType()
{
    if (m_poTimesType) {
        delete m_poTimesType;
        m_poTimesType = NULL;
    }
}

//add by yangks 2006.12.31  增加跳次计算功能(河北需求)
#ifdef PRIVATE_MEMORY
void FormatMethod::formatTimes(EventSection *_pEvt)
{
    if (!m_poTimesType)
        loadTimesType();
            
    int iType;   
    bool bRet = false;
    
    bRet = m_poTimesType->get(_pEvt->m_iEventTypeID,&iType);

    if (bRet) 
    {
        switch(iType)
        {
        //区内：
        case 1:
            if(_pEvt->m_lDuration <= 180)
            {
                _pEvt->m_iTimes = 2; 
            }
            else
            {
                _pEvt->m_iTimes = (_pEvt->m_lDuration+59)/60-1 ; 
            }
            break;
        //区间
        case 2:
            _pEvt->m_iTimes = (_pEvt->m_lDuration+59)/60 ;
            break;            
        default:
            _pEvt->m_iTimes = 0;
            break;
        }
    } 
    
    return;
}
#else 
void FormatMethod::formatTimes(EventSection *_pEvt)
{     
    unsigned int iType;   
    bool bRet = false;
    bRet = G_PPARAMINFO->m_poTimesTypeIndex->get(_pEvt->m_iEventTypeID,&iType);
	
    if (bRet) 
    {
        switch(iType)
        {
        //区内：
        case 1:
            if(_pEvt->m_lDuration <= 180)
            {
                _pEvt->m_iTimes = 2; 
            }
            else
            {
                _pEvt->m_iTimes = (_pEvt->m_lDuration+59)/60-1 ; 
            }
            break;
        //区间
        case 2:
            _pEvt->m_iTimes = (_pEvt->m_lDuration+59)/60 ;
            break;            
        default:
            _pEvt->m_iTimes = 0;
            break;
        }
    } 
    return;
}
#endif

int FormatMethod::getSwitchTypeID(int _iSwitchID)
{
    TSwitchInfo obSwitchInfo;
    //根据交换机找默认区号
    if (getSwitchInfoBySwitchID(_iSwitchID, &obSwitchInfo)) {
        return obSwitchInfo.m_iSwitchTypeID;
    }
    
    return -1;
}

