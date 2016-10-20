#include "TAvpParser.h"

#define GET_DATA_REF(dataType, data, containerEntryPtr) \
dataType &data = (containerEntryPtr)->dataRef(Type2Type<dataType>())

// sAvpHead: ver|len|(r,p,e,t,r,r,r,r)|code|appid|hh|ee
int TAvpHead::setHeadValue(const char *sAvpHead)
{
    if (sAvpHead == NULL)
    {
        Log::log(0, "The param of setHeadValue is null");
        return -1;
    }

    char *pos=(char *)sAvpHead;
    unsigned char sTemp[32];
    unsigned int num;
    int ret = 0;

    if (ret==0 && (pos=subStr2Int(pos, "|", (unsigned int &)this->Version))==NULL)
        ret = -1;

    if (ret==0 && (pos=subStr2Int(pos, "|", this->Length))==NULL)
        ret = -1;

    this->CommandFlag = 0x0;
    if (ret==0 && (pos=subStr2Int(pos+1, ",", num))==NULL)
        ret = -1;
    this->CommandFlag |= num<<7;
    if (ret==0 && (pos=subStr2Int(pos, ",", num))==NULL)
        ret = -1;
    this->CommandFlag |= num<<6;
    if (ret==0 && (pos=subStr2Int(pos, ",", num))==NULL)
        ret = -1;
    this->CommandFlag |= num<<5;
    if (ret==0 && (pos=subStr2Int(pos, ",", num))==NULL)
        ret = -1;
    this->CommandFlag |= num<<4;

    pos=strstr(pos, "|") + 1;
    if (ret==0 && (pos=subStr2Int(pos, "|", (unsigned int &)this->CommandCode))==NULL)
        ret = -1;
    if (ret==0 && (pos=subStr2Int(pos, "|", (unsigned int &)this->AppId))==NULL)
        ret = -1;
    if (ret==0 && (pos=subStr2Int(pos, "|", this->HopId))==NULL)
        ret = -1;
    if (ret==0 && (pos=subStr2Int(pos, "|", this->EndId))==NULL)
        ret = -1;

    if (ret == -1)
        Log::log(0, "param of setHeadValue: %s is not in correct format", sAvpHead);
    return ret;
}

int TAvpHead::initHead(eCommandCode _commandCode, bool _isRequest)
{
    Version =1;
    Length = 0;
    CommandCode = _commandCode;
    if(true == _isRequest)
    {
        if(_commandCode == COMMAND_CC)
        {
            CommandFlag = 0xC0;
        }
        else
        {
            CommandFlag = 0x80;
        }
    }
    else
    {
        CommandFlag = 0x00;
    }

    dccParamsMgr* pDccParamMgr = new dccParamsMgr;
    pDccParamMgr->getDccHeadValue(NULL);
    if(_commandCode != COMMAND_CC)
    {
        AppId = 0;
    }
    else
    {
        AppId = 4;
    }
    HopId = pDccParamMgr->m_iHopByHop;
    EndId = pDccParamMgr->m_iEndToEnd;
    delete pDccParamMgr;
    pDccParamMgr = NULL;
    return 1;
}

char* TAvpHead::subStr2Int(char *pos, const char *delim, unsigned int &num)
{
    if (pos == NULL)
        return NULL;

    char sTemp[32] = {0};
    char *pos1 = strstr(pos, delim);
    if (pos1 != NULL)
        strncpy(sTemp, pos, pos1-pos);
    else
        strcpy(sTemp, pos);
    num = (unsigned int)atol(sTemp);

    return pos1 + strlen(delim);
}

// sAvpHead: ver|len|(r,p,e,t,r,r,r,r)|code|appid|hh|ee
void TAvpHead::toCString(char *sAvpHead)
{
    if (sAvpHead == NULL)
        return;
    sprintf(sAvpHead,
            "%d|%ld|(%d,%d,%d,%d,%d,%d,%d,%d)|%d|%d|%ld|%ld",
            Version, Length, (CommandFlag>>7)&0x1, (CommandFlag>>6)&0x1,
            (CommandFlag>>5)&0x1, (CommandFlag>>4)&0x1, (CommandFlag>>3)&0x1,
            (CommandFlag>>2)&0x1, (CommandFlag>>1)&0x1, (CommandFlag>>0)&0x1,
            CommandCode, AppId, HopId, EndId);
}

// sAvpObject: avpHeadStr^^^avp_code:=avp_value^^^...^^^avp_code:=avp_value
int TAVPObject::setObjectValue(const char *sAvpObject)
{
#if DEBUG_TAVPPARSER
    Log::log(0, sAvpObject);
#endif
    if (sAvpObject == NULL)
        return -1;

    char *sPos = strstr((char*)sAvpObject, "^^^");
    if (sPos == NULL)
        return -1;

    char sAvpHead[256] = {0};
    strncpy(sAvpHead, sAvpObject, sPos-sAvpObject);
    avpHead.setHeadValue((const char*)sAvpHead);

    string strAvpObject(sPos+3);
    string::size_type loc1 = 0, loc2, loc3;

    this->clear();
    while (loc1!=string::npos && (loc2=strAvpObject.find(":=", loc1))!=string::npos)
    {
        if ((loc3=strAvpObject.find("^^^", loc2)) != string::npos)
        {
            this->insert(make_pair(strAvpObject.substr(loc1, loc2-loc1),
                                   strAvpObject.substr(loc2+2, loc3-loc2-2)));
            loc1 = loc3 + 3;
        }
        else
        {
            this->insert(make_pair(strAvpObject.substr(loc1, loc2-loc1),
                                   strAvpObject.substr(loc2+2)));
            loc1 = string::npos;
        }
    }
    return 0;
}

// sAvpObject: avpHead^^^avp_name:=avp_value^^^...^^^avp_name:=avp_value
void TAVPObject::toCString(char *sAvpObject)
{
    if (sAvpObject == NULL)
        return;

    avpHead.toCString(sAvpObject);

    iterator itr = begin();
    for (; itr!=end(); itr++)
    {
        strcat(sAvpObject, "^^^");
        strcat(sAvpObject, itr->first.c_str());
        strcat(sAvpObject, ":=");
        strcat(sAvpObject, itr->second.c_str());
    }
}

///
/// @brief  解析消息包的头信息
/// @param  输入: pDataBuffer (消息头指针) iDataSize (消息头长度)
/// @param  输出: pAvpHead (消息头对象)
/// @return 0为真成功；非0为失败
///
int TAvpParser::ParseHead(unsigned char *pDataBuffer, unsigned int iDataSize, TAvpHead *pAvpHead)
{
    AAAMessageBlock *aBuffer;
    HeaderParser hp;
    m_strFailedAvp = "";
    int code = 0;

    aBuffer = AAAMessageBlock::Acquire((char*)pDataBuffer, iDataSize);

    hp.setRawData(aBuffer);
    hp.setAppData(&m_oMsg->hdr);
    hp.setDictData(m_option);
    try
    {
        hp.parseRawToApp();
    }
    catch (AAAErrorStatus st)
    {
        Log::log(0, "parse header error");
        int type;
        string avp;
        st.get(type, code, avp);
        aBuffer->Release();
    }
    aBuffer->Release();

    AAADiameterHeader &h = m_oMsg->hdr;
    m_avpHead.Version = h.ver;
    m_avpHead.Length = h.length;
    m_avpHead.CommandCode = h.code;
    m_avpHead.CommandFlag = (h.flags.r << 7) | (h.flags.p << 6)  |
                            (h.flags.e << 5) | (h.flags.t << 4) | (h.flags.rsvd);
    m_avpHead.AppId = h.appId;
    m_avpHead.HopId = h.hh;
    m_avpHead.EndId = h.ee;

    if (pAvpHead != NULL)
        memcpy(pAvpHead, &m_avpHead, sizeof(TAvpHead));

    // 命令码不支持的作为CCR来解析体
    if (code == AAA_COMMAND_UNSUPPORTED)
    {
        m_oMsg->hdr.code = COMMAND_CODE_CC;
        m_oMsg->hdr.appId = 4;
    }

    return code;
}

///
/// @brief  解析出某一个DCC消息体内容
/// @param  输入: pDataBuffer (消息体指针) iDataSize (消息体长度)
/// @param  输出: pAVPObject (解析出的消息体结构)
/// @return 0为真成功；非0为失败
///
int TAvpParser::ParseBody(unsigned char *pDataBuffer, unsigned int iDataSize, TAVPObject *pAVPObject)
{
    releaseMsgAcl();
    ::setRequestFlag(m_oMsg->hdr.flags.r);
    setRequest_Flag(m_oMsg->hdr.flags.r);
    AAAMessageBlock *aBuffer;
    PayloadParser pp;

    aBuffer = AAAMessageBlock::Acquire((char*)pDataBuffer, iDataSize);
    aBuffer->size(m_oMsg->hdr.length-HEADER_SIZE);
    pp.setRawData(aBuffer);
    pp.setAppData(&m_oMsg->acl);
    // 先绑定默认字典，取出基本结构
    ::setDictHandle(*m_oMsg, "", "", "");
    pp.setDictData(m_oMsg->hdr.getDictHandle());

    if (m_oMsg->hdr.code == COMMAND_CODE_CC)
        pp.setListNum(4);
    else
        pp.setListNum(4);

    int type, code = 0;

    try
    {
        pp.parseRawToApp();
    }
    catch (AAAErrorStatus st)
    {
        Log::log(0, "1.payload parser error");
        // 存储失败的Avp到m_strFailedAvp
        st.get(type, code, m_strFailedAvp);
    }

    if (code==0 && m_oMsg->hdr.code==COMMAND_CODE_CC)
    {
        getServiceType();
        ::setDictHandle(*m_oMsg, "", m_strServiceType.c_str(), "");
        aBuffer->rd_ptr(aBuffer->base());
        aBuffer->size(m_oMsg->hdr.length-HEADER_SIZE);
        pp.setRawData(aBuffer);
        pp.setAppData(&m_oMsg->acl);
        pp.setDictData(m_oMsg->hdr.getDictHandle());
        pp.setListNum(4);

        releaseMsgAcl();
        try
        {
            pp.parseRawToApp();
        }
        catch (AAAErrorStatus st)
        {
            Log::log(0, "2.payload parser error");
            int type, code;
            // 存储失败的Avp到m_strFailedAvp
            st.get(type, code, m_strFailedAvp);
        }
    }

    aBuffer->Release();
    pAVPObject->clear();
    recordHeadLog();
    getAVPObject(pAVPObject);

    // CER 校验
    if (code==0 && m_oMsg->hdr.code==COMMAND_CODE_CE && m_oMsg->hdr.flags.r==1)
    {
        char sTmp[10] = {0};
        strcpy(sTmp, ((*pAVPObject)[string("258")]).c_str());
        Log::log(0, "Auth-Application-Id is %c", sTmp[0]);
        if (sTmp[0]!='\0' && sTmp[0]!='4')
            code = AAA_NO_COMMON_APPLICATION;
    }

    return  code;
}

// 对传入消息体进行编码
// 消息体编码前需根据应用场景设置m_strServiceType的值
int TAvpParser::BuildBody(TAVPObject *pAvpObject, unsigned char *pDataBuffer, unsigned int *iDataSize)
{
    releaseMsgAcl();
    TAvpHead *pAvpHead = &(pAvpObject->avpHead);

    m_oMsg->hdr.flags.r = (pAvpHead->CommandFlag>>7)&0x1;
    m_oMsg->hdr.code = pAvpHead->CommandCode;
    m_oMsg->hdr.appId = pAvpHead->AppId;
    ::setDictHandle(*m_oMsg, "", m_strServiceType.c_str(), "");

    if (setMsgAvp(pAvpHead->CommandCode, pAvpHead->CommandFlag>>7&0x1, m_strServiceType, pAvpObject) < 0)
        return -1;

    AAAMessageBlock *aBuffer;
    PayloadParser pp;
    aBuffer = AAAMessageBlock::Acquire((char*)pDataBuffer, BODY_LEN);
    pp.setRawData(aBuffer);
    pp.setAppData(&m_oMsg->acl);
    pp.setDictData(m_oMsg->hdr.getDictHandle());

    try
    {
        pp.parseAppToRaw();
    }
    catch (AAAErrorStatus st)
    {
        Log::log(0, "BuildBody failed");
        int type, code;
        // 存储失败的Avp到m_strFailedAvp
        st.get(type, code, m_strFailedAvp);
        aBuffer->Release();
        return code;
    }

    m_oMsg->hdr.length = aBuffer->wr_ptr()-aBuffer->base()+20;     // 消息头编码时不再设置
    if (iDataSize != NULL)
        *iDataSize = aBuffer->wr_ptr()-aBuffer->base();
    m_bBuiledBody = true;

    aBuffer->Release();
    return 0;
}

// 对消息头进行编码
int TAvpParser::BuildHead(TAvpHead *pAvpHead, unsigned char *pDataBuffer, unsigned int *iDataSize)
{
    if (!m_bBuiledBody)
    {
        Log::log(0, "Body is not builed!");
        return -1;
    }
    m_bBuiledBody = false;
    m_strFailedAvp = "";

    m_oMsg->hdr.ver = pAvpHead->Version;
    m_oMsg->hdr.flags.r = (pAvpHead->CommandFlag>>7)&0x1;
    m_oMsg->hdr.flags.p = (pAvpHead->CommandFlag>>6)&0x1;
    m_oMsg->hdr.flags.e = (pAvpHead->CommandFlag>>5)&0x1;
    m_oMsg->hdr.flags.t = (pAvpHead->CommandFlag>>4)&0x1;
    m_oMsg->hdr.flags.rsvd = 0x0;
    m_oMsg->hdr.code = pAvpHead->CommandCode;
    m_oMsg->hdr.appId = pAvpHead->AppId;
    m_oMsg->hdr.hh = pAvpHead->HopId;
    m_oMsg->hdr.ee = pAvpHead->EndId;
    if (pAvpHead->CommandCode == 272)
        m_oMsg->hdr.flags.p = 0x01;

    AAAMessageBlock *aBuffer;
    HeaderParser hp;
    aBuffer = AAAMessageBlock::Acquire((char*)pDataBuffer, 20);
    hp.setRawData(aBuffer);
    hp.setAppData(&m_oMsg->hdr);
    hp.setDictData(m_option);

    try
    {
        hp.parseAppToRaw();
    }
    catch (AAAErrorStatus st)
    {
        Log::log(0, "build header error");
        int type, code;
        string avp;
        st.get(type, code, avp);
        aBuffer->Release();
        return code;
    }

    if (iDataSize != NULL)
        *iDataSize = 20;
    aBuffer->Release();

    memcpy(&m_avpHead, pAvpHead, sizeof(TAvpHead));
    recordHeadLog();
    TAVPObject avpObj;
    getAVPObject(&avpObj);

    return 0;
}

// 判断消息头，根据基本格式测试的要求，跟业务相关的AVP不做判断
int TAvpParser::IsValidMsg(TAVPObject *pAVPObject)
{
    TAvpHead *pHead = &(pAVPObject->avpHead);
    char sHead[512];
    pHead->toCString(sHead);

    if (pHead->Version != 1)
    {
        Log::log(0, "%s Unsupported Version", sHead);
        return AAA_UNSUPPORTED_VERSION;
    }

    if (pHead->Length==0 || pHead->Length%4)
    {
        Log::log(0, "%s Invalid Message Length", sHead);
        return AAA_INVALID_MESSAGE_LENGTH;
    }

    if (pHead->CommandFlag&0x0f)
    {
        Log::log(0, "%s invalid bit", sHead);
        return AAA_INVALID_BIT_IN_HEADER;
    }

    if (AAACommandList::instance()
            ->search(pHead->CommandCode, pHead->AppId, pHead->CommandFlag&0x0f) == NULL)
    {
        Log::log(0, "%s Command_Unsupported", sHead);
        return AAA_COMMAND_UNSUPPORTED;
    }

    if (pHead->CommandFlag>>7&0x1 && pHead->CommandFlag>>5&0x1)
    {
        Log::log(0, "%s e flag cannot be set for request", sHead);
        return AAA_INVALID_HDR_BITS;
    }

    char sKey[256];
    switch (pHead->CommandCode)
    {
    case COMMAND_CODE_CC:
        sprintf(sKey, "%s", COMMAND_NAME_CC);
        break;
    case COMMAND_CODE_RA:
        sprintf(sKey, "%s", COMMAND_NAME_RA);
        break;
    case COMMAND_CODE_AS:
        sprintf(sKey, "%s", COMMAND_NAME_AS);
        break;
    case COMMAND_CODE_DW:
        sprintf(sKey, "%s", COMMAND_NAME_DW);
        break;
    case COMMAND_CODE_DP:
        sprintf(sKey, "%s", COMMAND_NAME_DP);
        break;
    case COMMAND_CODE_ST:
        sprintf(sKey, "%s", COMMAND_NAME_ST);
        break;
    case COMMAND_CODE_CE:
        sprintf(sKey, "%s", COMMAND_NAME_CE);
        break;
    default:
        break;
    }
    if (pHead->CommandFlag>>7&0x1 == 1)
        strcat(sKey, "-Request");
    else
        strcat(sKey, "-Answer");

    if (m_strServiceType != "")
    {
        strcat(sKey, "_");
        strcat(sKey, m_strServiceType.c_str());
    }

    DictCommand *pDictCommand;
    if (!m_pDictCommandIndex->get(sKey, &pDictCommand))
    {
        Log::log(0, "Command(name_scenario): %s is not found", sKey);
        return -1;
    }

    vector<string>::iterator itr = pDictCommand->m_vecFixedAvp.begin();
    for (; itr!=pDictCommand->m_vecFixedAvp.end(); itr++)
    {
        if (pAVPObject->find(*itr) == pAVPObject->end())
            return AAA_MISSING_AVP;
    }
    return 0;
}


void TAvpParser::initDictCommand()
{
    char sKey[256];
    AAACommandList *pCommandList = AAACommandList::instance();
    int count = pCommandList->size();
    m_pDictCommandIndex = new HashList<DictCommand *> ((count>>1) | 1);

    DictCommand *pDictCommand;
    AAAAvpList *pAAAAvpList = AAAAvpList::instance();
    count = pAAAAvpList->size();
    AAACommandList::iterator commandItr = pCommandList->begin();
    for (; commandItr!=pCommandList->end(); commandItr++)
    {
        if ((*commandItr)->scenario == "")
            sprintf(sKey, "%s", (*commandItr)->name.c_str());
        else
            sprintf(sKey, "%s_%s", (*commandItr)->name.c_str(), (*commandItr)->scenario.c_str());
        pDictCommand = new DictCommand();
        pDictCommand->m_pAvpFieldNameIndex = new HashList<AvpField *> ((count>>1) | 1);
        pDictCommand->m_pAvpFieldCodeIndex = new HashList<AvpField *> ((count>>1) | 1);
#if DEBUG_TAVPPARSER
        Log::log(0, "%s", sKey);
#endif
        initGroupedAvp(*commandItr, "", "", pDictCommand);
        initFixedAvp(*commandItr, "", pDictCommand);
        m_pDictCommandIndex->add(sKey, pDictCommand);
    }
}

void TAvpParser::initGroupedAvp(AAADictionary* pAAADictionary, const char* avpNamePrefix, const char* avpCodePrefix, DictCommand *pDictCommand)
{
    initQualiedAvp(pAAADictionary->avp_f, avpNamePrefix, avpCodePrefix, pDictCommand);
    initQualiedAvp(pAAADictionary->avp_r, avpNamePrefix, avpCodePrefix, pDictCommand);
    initQualiedAvp(pAAADictionary->avp_o, avpNamePrefix, avpCodePrefix, pDictCommand);
    initQualiedAvp(pAAADictionary->avp_f2, avpNamePrefix, avpCodePrefix, pDictCommand);
}

void TAvpParser::initQualiedAvp(AAAQualifiedAvpList* pQualifiedAvpList, const char* avpNamePrefix, const char* avpCodePrefix, DictCommand *pDictCommand)
{
    string avpName = avpNamePrefix;
    string avpCode = avpCodePrefix;
    char sTemp[10];
    AAAQualifiedAvpList::iterator qualifiedItr;
    for (qualifiedItr=pQualifiedAvpList->begin(); qualifiedItr!=pQualifiedAvpList->end(); qualifiedItr++)
    {
        if ((*qualifiedItr)->avp->avpType == AAA_AVP_GROUPED_TYPE)
        {
            // 从AAAGroupedAvpList中找到对应groupAvp
            AAAGroupedAvpList *pGroupedAvpList = AAAGroupedAvpList::instance();
            AAAGroupedAvpList::iterator groupedItr = pGroupedAvpList->begin();
            while (groupedItr != pGroupedAvpList->end())
            {
                if ((*groupedItr)->code==(*qualifiedItr)->avp->avpCode
                        && (*groupedItr)->vendorId==(*qualifiedItr)->avp->vendorId
                        && (*groupedItr)->scenarioId==(*qualifiedItr)->avp->scenarioId)
                    break;
                groupedItr++;
            }
            if (groupedItr == pGroupedAvpList->end())
                continue;
            if (avpName != "")
            {
                avpName.append(".");
                avpCode.append(".");
            }
            avpName.append((*qualifiedItr)->avp->avpName);
            sprintf(sTemp, "%d", (*qualifiedItr)->avp->avpCode);
            avpCode.append(sTemp);
            initGroupedAvp(*groupedItr, avpName.c_str(), avpCode.c_str(), pDictCommand);
        }
        else
        {
            if (avpName != "")
            {
                avpName.append(".");
                avpCode.append(".");
            }
            avpName.append((*qualifiedItr)->avp->avpName);
            sprintf(sTemp, "%d", (*qualifiedItr)->avp->avpCode);
            avpCode.append(sTemp);
            AvpField *pAvpField = new AvpField(avpName, avpCode, (*qualifiedItr)->avp->avpType);
            char sKey[256] = {0};
            strcpy(sKey, avpName.c_str());
            pDictCommand->m_pAvpFieldNameIndex->add(sKey, pAvpField);
            memset(sKey, 0, sizeof(sKey));
            strcpy(sKey, avpCode.c_str());
            pDictCommand->m_pAvpFieldCodeIndex->add(sKey, pAvpField);
#if DEBUG_TAVPPARSER
            Log::log(0, "\t\t%s: %s, %d", avpName.c_str(), avpCode.c_str(), (*qualifiedItr)->avp->avpType);
#endif
        }
        avpName = avpNamePrefix;
        avpCode = avpCodePrefix;
    }
}

void TAvpParser::initFixedAvp(AAADictionary* pAAADictionary, const char* prefix, DictCommand *pDictCommand)
{
    string avpCode = prefix;
    char sTemp[10];
    AAAQualifiedAvpList::iterator qualifiedItr;
    AAAQualifiedAvpList* pQualifiedAvpList = pAAADictionary->avp_f;
    for (qualifiedItr=pQualifiedAvpList->begin(); qualifiedItr!=pQualifiedAvpList->end(); qualifiedItr++)
    {
        if ((*qualifiedItr)->avp->avpType == AAA_AVP_GROUPED_TYPE)
        {
            // 从AAAGroupedAvpList中找到对应groupAvp
            AAAGroupedAvpList *pGroupedAvpList = AAAGroupedAvpList::instance();
            AAAGroupedAvpList::iterator groupedItr = pGroupedAvpList->begin();
            while (groupedItr != pGroupedAvpList->end())
            {
                if ((*groupedItr)->code==(*qualifiedItr)->avp->avpCode
                        && (*groupedItr)->vendorId==(*qualifiedItr)->avp->vendorId
                        && (*groupedItr)->scenarioId==(*qualifiedItr)->avp->scenarioId)
                    break;
                groupedItr++;
            }
            if (groupedItr == pGroupedAvpList->end())
                continue;
            if (avpCode != "")
                avpCode.append(".");
            sprintf(sTemp, "%d", (*qualifiedItr)->avp->avpCode);
            avpCode.append(sTemp);
            initFixedAvp(*groupedItr, avpCode.c_str(), pDictCommand);
        }
        else
        {
            if (avpCode != "")
                avpCode.append(".");
            sprintf(sTemp, "%d", (*qualifiedItr)->avp->avpCode);
            avpCode.append(sTemp);
            pDictCommand->m_vecFixedAvp.push_back(avpCode);
#if DEBUG_TAVPPARSER
            Log::log(0, "\t\t%s", avpCode.c_str());
#endif
        }
        avpCode = prefix;
    }
}

void TAvpParser::setServiceType(const string strServiceType)
{
    m_strServiceType = strServiceType;
}

string& TAvpParser::getServiceType()
{
    string::size_type pos1;
    string serviceContextId;

    if (_getAvpValue("Service-Context-Id", serviceContextId) <= 0)
    {
        return m_strServiceType;
    }

    pos1 = serviceContextId.find("@", 0);
    if (pos1 != string::npos)
    {
        m_strServiceType = serviceContextId.substr(0, pos1);
    }

    return m_strServiceType;
}

// 获取解析后的TAVPObject
void TAvpParser::getAVPObject(TAVPObject *pAVPObject)
{
    memcpy(&(pAVPObject->avpHead), &m_avpHead, sizeof(TAvpHead));
    vector<string> sAvpName;
    pAVPObject->clear();
    getAllAvp(&(m_oMsg->acl), sAvpName, pAVPObject);
}


void TAvpParser::recordHeadLog()
{
    char sBuff[512] = {0};

    sprintf(sBuff,
            "version = %d\n"
            "length = %ld\n"
            "code = %d\n"
            "flags(r,p,e,t,r,r,r,r) = (%d,%d,%d,%d,%d,%d,%d,%d)\n"
            "appId = %d\n"
            "hh = %ld\n"
            "ee = %ld\n",
            m_avpHead.Version, m_avpHead.Length, m_avpHead.CommandCode,
            (int)((m_avpHead.CommandFlag>>7)&0x1), (int)((m_avpHead.CommandFlag>>6)&0x1),
            (int)((m_avpHead.CommandFlag>>5)&0x1), (int)((m_avpHead.CommandFlag>>4)&0x1),
            (int)((m_avpHead.CommandFlag>>3)&0x1), (int)((m_avpHead.CommandFlag>>2)&0x1),
            (int)((m_avpHead.CommandFlag>>1)&0x1), (int)((m_avpHead.CommandFlag>>0)&0x1),
            m_avpHead.AppId, m_avpHead.HopId, m_avpHead.EndId);

    m_strDccLog = string(sBuff);

    if (m_bDispFlag)
    {
        Log::log(0, "%s", m_strDccLog.c_str());
    }
}

void TAvpParser::getAllAvp(AAAAvpContainerList *orig_acl, vector<string> &sAvpName, TAVPObject *pAVPObject)
{
    AAAAvpContainer *c;
    AAAAvpContainerList *acl = orig_acl,  *acl1;
    AAAAvpContainerList::iterator itor;
    unsigned int i;
    std::string tmp;
    char tmp_buf[20];

    int m_iGroupNum = 0;
    int mi_AvpNum = 0;

    for (itor = acl->begin(); itor != acl->end(); itor++)
    {
        c =  *itor;
        for (i = 0; i < c->size(); i++)
        {
            if (((*c)[i]->dataType()) == AAA_AVP_GROUPED_TYPE)
            {
                m_iGroupNum++;
                if (c->size() > 1)
                {
#if DEBUG_TAVPPARSER
                    string strTemp = c->getAvpName();
                    Log::log(0, "grouped push_back %s", strTemp.c_str());
#endif
                    sprintf(tmp_buf, "%d", i);
                    tmp = c->getAvpName() + std::string("[") + std::string(tmp_buf) + std::string("]");
                    sAvpName.push_back(tmp);
                }
                else
                {
                    sAvpName.push_back(c->getAvpName());
                }
                acl1 = ((*c)[i])->dataPtr(Type2Type < AAAAvpContainerList > ());
                getAllAvp(acl1, sAvpName, pAVPObject);
                sAvpName.pop_back();
            }
            else
            {
                mi_AvpNum++;
                sAvpName.push_back(c->getAvpName());
#if DEBUG_TAVPPARSER
                string strTemp = c->getAvpName();
                Log::log(0, "sAvpName.push_back %s", strTemp.c_str());
#endif
                getAvpData((*c)[i], i, c->size(), sAvpName, pAVPObject);
                sAvpName.pop_back();
            }
        }
    }
}

void TAvpParser::getAvpName(vector<string>& sAvpName, string& avp_name)
{
    vector<string>::iterator itor;

    for (itor = sAvpName.begin(); itor != sAvpName.end(); itor++)
    {
        if (itor == sAvpName.begin())
            avp_name = (*itor);
        else
            avp_name += "." + (*itor);
    }
}

void TAvpParser::getAvpData(AAAAvpContainerEntry *e, int pos, int size, vector<string> &sAvpName, TAVPObject *pAVPObject)
{
    string tmp;
    string avp_name = "";
    AvpField avpField;
    char sTmpBuf[5120], sFormatStr[80];
    int i;

    if (size > 1)
        sprintf(sFormatStr, "[%d]", pos);
    else
        strcpy(sFormatStr, "");
    getAvpName(sAvpName, avp_name);
#if DEBUG_TAVPPARSER
    Log::log(0, "avp_name %s", avp_name.c_str());
#endif
    getAvpField(m_avpHead.CommandCode, (m_avpHead.CommandFlag>>7)&0x1, m_strServiceType,
                avp_name, 1, avpField);

    switch (e->dataType())
    {
    case AAA_AVP_INTEGER32_TYPE:
    {
        sprintf(sTmpBuf, "%ld", e->dataRef(Type2Type<diameter_integer32_t>()));
        if (m_bDispFlag)
        {
            Log::log(0, "[type=diameter_integer32_t]%s%s%s", avp_name.c_str(), sFormatStr, sTmpBuf);
        }
        m_strDccLog += "[type=diameter_integer32_t]" + avp_name + sFormatStr + ":" + sTmpBuf + "\n";
        pAVPObject->insert(make_pair(avpField.m_avpCode+sFormatStr, string(sTmpBuf)));
    }
    break;
    case AAA_AVP_TIME_TYPE:  //modified by 2011.03.15
    {
        sprintf(sTmpBuf, "%lu", e->dataRef(Type2Type<diameter_time_t>()));
        //getAvpName(sAvpName, avp_name);
        if (m_bDispFlag)
        {
            Log::log(0, "[type=diameter_time_t]%s%s%s", avp_name.c_str(), sFormatStr, sTmpBuf);
        }
        m_strDccLog += "[type=diameter_time_t]" + avp_name + sFormatStr + ":" + sTmpBuf + "\n";
        pAVPObject->insert(make_pair(avpField.m_avpCode+sFormatStr, string(sTmpBuf)));
    }
    break;    //modified by 2011.03.15
    case AAA_AVP_UINTEGER32_TYPE:
    {
        sprintf(sTmpBuf, "%lu", e->dataRef(Type2Type<diameter_unsigned32_t>()));
        //getAvpName(sAvpName, avp_name);
        if (m_bDispFlag)
        {
            Log::log(0, "[type=diameter_unsigned32_t]%s%s%s", avp_name.c_str(), sFormatStr, sTmpBuf);
        }
        m_strDccLog += "[type=diameter_unsigned32_t]" + avp_name + sFormatStr + ":" + sTmpBuf + "\n";
        pAVPObject->insert(make_pair(avpField.m_avpCode+sFormatStr, string(sTmpBuf)));
    }
    break;

    case AAA_AVP_INTEGER64_TYPE:
    {
        sprintf(sTmpBuf, "%ld", e->dataRef(Type2Type<diameter_integer64_t>()));
        //getAvpName(sAvpName, avp_name);
        if (m_bDispFlag)
        {
            Log::log(0, "[type=diameter_integer64_t]%s%s%s", avp_name.c_str(), sFormatStr, sTmpBuf);
        }
        m_strDccLog += "[type=diameter_integer64_t]" + avp_name + sFormatStr + ":" + sTmpBuf + "\n";
        pAVPObject->insert(make_pair(avpField.m_avpCode+sFormatStr, string(sTmpBuf)));
    }
    break;
    case AAA_AVP_UINTEGER64_TYPE:
    {
        sprintf(sTmpBuf, "%lu", e->dataRef(Type2Type<diameter_unsigned64_t>()));
        //getAvpName(sAvpName, avp_name);
        if (m_bDispFlag)
        {
            Log::log(0, "[type=diameter_unsigned64_t]%s%s%s", avp_name.c_str(), sFormatStr, sTmpBuf);
        }
        m_strDccLog += "[type=diameter_unsigned64_t]" + avp_name + sFormatStr + ":" + sTmpBuf + "\n";
        pAVPObject->insert(make_pair(avpField.m_avpCode+sFormatStr, string(sTmpBuf)));
    }
    break;
    case AAA_AVP_ENUM_TYPE:
    {
        sprintf(sTmpBuf, "%d", e->dataRef(Type2Type<diameter_enumerated_t>()));
        //getAvpName(sAvpName, avp_name);
        if (m_bDispFlag)
        {
            Log::log(0, "[type=diameter_enumerated_t]%s%s%s", avp_name.c_str(), sFormatStr, sTmpBuf);
        }
        m_strDccLog += "[type=diameter_enumerated_t]" + avp_name + sFormatStr + ":" + sTmpBuf + "\n";
        pAVPObject->insert(make_pair(avpField.m_avpCode+sFormatStr, string(sTmpBuf)));
    }
    break;
    case AAA_AVP_STRING_TYPE:
    {
        sprintf(sTmpBuf, "%s", e->dataRef(Type2Type<diameter_octetstring_t>()).c_str());
        //getAvpName(sAvpName, avp_name);
        if (m_bDispFlag)
        {
            Log::log(0, "[type=diameter_octetstring_t]%s%slength:[%d]%s",
                     avp_name.c_str(), sFormatStr, strlen(sTmpBuf), sTmpBuf);
        }
        m_strDccLog += "[type=diameter_octetstring_t]" + avp_name + sFormatStr + ":" + sTmpBuf + "\n";
        pAVPObject->insert(make_pair(avpField.m_avpCode+sFormatStr, string(sTmpBuf)));
    }
    break;

    case AAA_AVP_ADDRESS_TYPE:
    {
        //getAvpName(sAvpName, avp_name);
        diameter_address_t &addr = e->dataRef(Type2Type<diameter_address_t>());
        sprintf(sTmpBuf, "%d:", addr.type);
        int len = addr.value.length();
        unsigned char tmp[100];
        memcpy(tmp, addr.value.c_str(), len);
        for (i = 0; i < len; i++)
            sprintf(sTmpBuf + strlen(sTmpBuf), "%d.", tmp[i]);

        if (sTmpBuf[strlen(sTmpBuf) - 1] == '.')
            sTmpBuf[strlen(sTmpBuf) - 1] = 0;

        if (m_bDispFlag)
        {
            Log::log(0, "[type=diameter_address_t]%s%s%s", avp_name.c_str(), sFormatStr, sTmpBuf);
        }
        m_strDccLog += "[type=diameter_address_t]" + avp_name + sFormatStr + ":" + sTmpBuf + "\n";
        pAVPObject->insert(make_pair(avpField.m_avpCode+sFormatStr, string(sTmpBuf)));
    }
    break;
    case AAA_AVP_UTF8_STRING_TYPE:
    {
        //getAvpName(sAvpName, avp_name);
        sprintf(sTmpBuf, "%s", e->dataRef(Type2Type<diameter_utf8string_t>()).c_str());
        if (m_bDispFlag)
        {
            Log::log(0, "[type=diameter_utf8string_t]%s%s%s", avp_name.c_str(), sFormatStr, sTmpBuf);
        }
        m_strDccLog += "[type=diameter_utf8string_t]" + avp_name + sFormatStr + ":" + sTmpBuf + "\n";
        pAVPObject->insert(make_pair(avpField.m_avpCode+sFormatStr, string(sTmpBuf)));
    }
    break;
    case AAA_AVP_DIAMID_TYPE:
    {
        //getAvpName(sAvpName, avp_name);
        sprintf(sTmpBuf, "%s", e->dataRef(Type2Type<diameter_identity_t>()).c_str());
        if (m_bDispFlag)
        {
            Log::log(0, "[type=diameter_identity_t]%s%s%s", avp_name.c_str(), sFormatStr, sTmpBuf);
        }
        m_strDccLog += "[type=diameter_identity_t]" + avp_name + sFormatStr + ":" + sTmpBuf + "\n";
        pAVPObject->insert(make_pair(avpField.m_avpCode+sFormatStr, string(sTmpBuf)));
    }
    break;
    case AAA_AVP_DATA_TYPE:
    case AAA_AVP_DIAMURI_TYPE:
        //case AAA_AVP_TIME_TYPE:    //modified by zhufengsheng 2011.03.15
    case AAA_AVP_GROUPED_TYPE:
    case AAA_AVP_IPFILTER_RULE_TYPE:
    case AAA_AVP_CUSTOM_TYPE:
        //getAvpName(sAvpName, avp_name);
        sprintf(sTmpBuf, "%s", e->dataRef(Type2Type<diameter_identity_t>()).c_str());
        if (m_bDispFlag)
        {
            Log::log(0, "WARNING TYPE [type=diameter_identity_t]%s%s%s", avp_name.c_str(), sFormatStr, sTmpBuf);
        }
        m_strDccLog += "[type=diameter_identity_t]" + avp_name + sFormatStr + ":" + sTmpBuf + "\n";

        break;
    }
}

template<class T>
int TAvpParser::_getAvpValue(const char *avp_name1, T &data)
{
    AAAAvpContainer *c;
    int quit_flag = 0;
    std::string::size_type pos, start_pos;
    std::string avpName, avp_name = std::string(avp_name1);
    std::string::size_type pos1, pos2, pos3;
    AAAAvpContainerList *acl = &m_oMsg->acl;
    for (pos = 0, start_pos = 0; !quit_flag;)
    {
        pos1 = avp_name.find(".", start_pos);
        if (pos1 != std::string::npos)
        {
            avpName = avp_name.substr(start_pos, pos1 - start_pos);
            pos2 = avpName.find("[", 0);
            if (pos2 != std::string::npos)
            {
                pos3 = avpName.find("]", pos2);
                if (pos3 != std::string::npos)
                    pos = atoi(avpName.substr(pos2 + 1, pos3 - pos2 - 1).c_str());
                avpName.erase(pos2, 10);
            }
            start_pos = pos1 + 1;
        }
        else
        {
            pos2 = avp_name.find("[", start_pos);
            if (pos2 != std::string::npos)
            {
                pos3 = avp_name.find("]", pos2);
                if (pos3 != std::string::npos)
                    pos = atoi(avp_name.substr(pos2+1, pos3-pos2-1).c_str());
                avpName = avp_name.substr(start_pos, pos2-start_pos);;
            }
            else
            {
                avpName = avp_name.substr(start_pos, 500);
            }
            quit_flag = 1;
        }

        if ((c = acl->search(avpName.c_str())))
        {
            if (c->size() > pos)
            {
                if (((*c)[pos]->dataType()) == AAA_AVP_GROUPED_TYPE)
                {
                    acl = ((*c)[pos])->dataPtr(Type2Type<AAAAvpContainerList>());
                    pos = 0;
                }
                else
                {
                    GET_DATA_REF(T, tmp, (*c)[pos]);
                    data = tmp;
                    pos = 0;
                    return 1;
                }
            }
            else
            {
                Log::log(0, "pos[%d] is large size[%d]", pos, c->size());
                return  - 1;
            }
        }
        else
        {
            printf("avpName[%s] not found", avpName.c_str());
            //Log::log(0, "avpName[%s] not found", avpName.c_str());
            return  - 1;
        }
    }
    return 0;

}

template<class T>
int  TAvpParser::_setAvpValue(const char *avp_name1, T &data, AAA_AVPDataType t)
{
    AAAAvpContainerManager cm;
    AAAAvpContainerEntryManager em;
    AAAAvpContainerEntry *e;
    int quit_flag = 0, ret = 0, flag, set_flag;
    std::string::size_type pos, start_pos;
    std::string avpName, avp_name = std::string(avp_name1);
    std::string::size_type pos1, pos2, pos3;
    AAAAvpContainerList *acl = &m_oMsg->acl;
    AAAAvpContainerList *tmp_list = NULL;
    AAAAvpContainer *c;
    for (pos = 0, start_pos = 0, flag = 0, set_flag = 0; !quit_flag;)
    {
        pos1 = avp_name.find(".", start_pos);
        if (pos1 != std::string::npos)
        {
            avpName = avp_name.substr(start_pos, pos1 - start_pos);
            pos2 = avpName.find("[", 0);
            if (pos2 != std::string::npos)
            {
                pos3 = avpName.find("]", pos2);
                if (pos3 != std::string::npos)
                    pos = atoi(avpName.substr(pos2 + 1, pos3 - pos2 - 1).c_str());
                avpName.erase(pos2, 10);
            }
            start_pos = pos1 + 1;
        }
        else
        {
            pos2 = avp_name.find("[", start_pos);
            if (pos2 != std::string::npos)
            {
                pos3 = avp_name.find("]", pos2);
                if (pos3 != std::string::npos)
                    pos = atoi(avp_name.substr(pos2+1, pos3-pos2-1).c_str());
                avpName = avp_name.substr(start_pos, pos2-start_pos);;
            }
            else
            {
                avpName = avp_name.substr(start_pos, 500);
            }
            quit_flag = 1;
        }

        if (!flag && (c = acl->search(avpName.c_str())))
        {
            if (c->size() > pos)
            {
                if (((*c)[pos]->dataType()) == AAA_AVP_GROUPED_TYPE)
                {
                    acl = ((*c)[pos])->dataPtr(Type2Type<AAAAvpContainerList>());
                    pos = 0;
                    set_flag = 1;
                }
                else
                {
                    //if (t != AAA_AVP_DATA_TYPE)
                    {
                        GET_DATA_REF(T, tmp, (*c)[pos]);
                        pos = 0;
                        tmp = data;
                    }
                    return 1;
                }
            }
            else
            {
                if (!quit_flag)
                {
                    e = em.acquire(AAA_AVP_GROUPED_TYPE);
                    acl = e->dataPtr(Type2Type < diameter_grouped_t > ());
                    c->add(e);
                }
                else
                {
                    //if (t != AAA_AVP_DATA_TYPE)
                    {
                        e = em.acquire(t);
                        GET_DATA_REF(T, tmp, e);
                        c->add(e);
                        tmp = data;
                    }
                    ret = 1;
                }
                flag = 1;
            }
        }
        else
        {
            c = cm.acquire(avpName.c_str());
            if (!quit_flag)
            {
                e = em.acquire(AAA_AVP_GROUPED_TYPE);
                tmp_list = e->dataPtr(Type2Type < diameter_grouped_t > ());
                c->add(e);
            }
            else
            {
                //if (t != AAA_AVP_DATA_TYPE)
                {
                    e = em.acquire(t);
                    GET_DATA_REF(T, tmp, e);
                    c->add(e);
                    tmp = data;
                }
                ret = 1;
            }

            if (acl)
            {
                acl->add(c);
            }
            acl = tmp_list;
            flag = 1;
        }

    }

    return ret;
}

int TAvpParser::setMsgAvp(int commandCode, int request, const string &scenario, TAVPObject *pAVPObject)
{
    int    iTemp;
    long   lTemp;
    string strTemp;
    AvpField avpField;
    TAVPObject::iterator itr = pAVPObject->begin();
    for (; itr!=pAVPObject->end(); itr++)
    {
        getAvpField(commandCode, request, scenario, itr->first, 2, avpField);
        if (avpField.m_avpCode == "")
        {
            Log::log(0, "setMsgAvp failed! \n avpName(%s) is not found in dict",
                     itr->first.c_str());
            return -1;
        }
        switch (avpField.m_avpType)
        {
        case AAA_AVP_INTEGER32_TYPE:
            iTemp = atoi(itr->second.c_str());
            _setAvpValue(avpField.m_avpName.c_str(), iTemp, AAA_AVP_INTEGER32_TYPE);
            break;
        case AAA_AVP_UINTEGER32_TYPE:
            iTemp = atoi(itr->second.c_str());
            _setAvpValue(avpField.m_avpName.c_str(), iTemp, AAA_AVP_UINTEGER32_TYPE);
            break;
        case AAA_AVP_INTEGER64_TYPE:
            lTemp = atol(itr->second.c_str());
            _setAvpValue(avpField.m_avpName.c_str(), lTemp, AAA_AVP_INTEGER64_TYPE);
            break;
        case AAA_AVP_UINTEGER64_TYPE:
            lTemp = atol(itr->second.c_str());
            _setAvpValue(avpField.m_avpName.c_str(), lTemp, AAA_AVP_UINTEGER64_TYPE);
            break;
        case AAA_AVP_ENUM_TYPE:
            iTemp = atoi(itr->second.c_str());
            _setAvpValue(avpField.m_avpName.c_str(), iTemp, AAA_AVP_ENUM_TYPE);
            break;
        case AAA_AVP_STRING_TYPE:
            strTemp = itr->second;
            _setAvpValue(avpField.m_avpName.c_str(), strTemp, AAA_AVP_STRING_TYPE);
            break;
        case AAA_AVP_ADDRESS_TYPE:
        {
            diameter_address_t address;
            int buf[5] = {0};
            if (itr->second.find(":") != string::npos)
            {
                sscanf(itr->second.c_str(), "%d:%d.%d.%d.%d", &buf[4], &buf[0], &buf[1], &buf[2], &buf[3]);
                address.type = buf[4];
            }
            else
            {
                sscanf(itr->second.c_str(), "%d.%d.%d.%d", &buf[0], &buf[1], &buf[2], &buf[3]);
                address.type = 1;
            }
            char tmp[4] = {0};
            tmp[0] = buf[0];
            tmp[1] = buf[1];
            tmp[2] = buf[2];
            tmp[3] = buf[3];
            address.value.assign(tmp, 4);
            _setAvpValue(avpField.m_avpName.c_str(), address, AAA_AVP_ADDRESS_TYPE);
        }
        break;
        case AAA_AVP_UTF8_STRING_TYPE:
            strTemp = itr->second;
            _setAvpValue(avpField.m_avpName.c_str(), strTemp, AAA_AVP_UTF8_STRING_TYPE);
            break;
        case AAA_AVP_DIAMID_TYPE:
            strTemp = itr->second;
            _setAvpValue(avpField.m_avpName.c_str(), strTemp, AAA_AVP_DIAMID_TYPE);
            break;
        case AAA_AVP_DATA_TYPE:
            strTemp = itr->second;
            _setAvpValue(avpField.m_avpName.c_str(), strTemp, AAA_AVP_DATA_TYPE);
            break;

        case AAA_AVP_TIME_TYPE: //add by zhufengsheng 2011.03.15
            iTemp = atoi(itr->second.c_str());
            _setAvpValue(avpField.m_avpName.c_str(), iTemp, AAA_AVP_TIME_TYPE);
            break;
        case AAA_AVP_DIAMURI_TYPE:
            //case AAA_AVP_TIME_TYPE:
        case AAA_AVP_GROUPED_TYPE:
        case AAA_AVP_IPFILTER_RULE_TYPE:
        case AAA_AVP_CUSTOM_TYPE:
        default:
            Log::log(0, "setMsgAvp failed! \n avpName(%s) avpDataType(%d) is not surpported",
                     avpField.m_avpName.c_str(), avpField.m_avpType);
            return -1;
        }
    }

    return 0;
}

// valueType=1时，strValue为avpName，valueType=2时，strValue为avpCode
// 返回的AvpField中m_avpName和m_avpCode带有下标
void TAvpParser::getAvpField(int commandCode, int request, const string &scenario,
                             string strValue, int valueType, AvpField &avpField)
{
    char sKey[256];
    switch (commandCode)
    {
    case COMMAND_CODE_CC:
        sprintf(sKey, "%s", COMMAND_NAME_CC);
        break;
    case COMMAND_CODE_RA:
        sprintf(sKey, "%s", COMMAND_NAME_RA);
        break;
    case COMMAND_CODE_AS:
        sprintf(sKey, "%s", COMMAND_NAME_AS);
        break;
    case COMMAND_CODE_DW:
        sprintf(sKey, "%s", COMMAND_NAME_DW);
        break;
    case COMMAND_CODE_DP:
        sprintf(sKey, "%s", COMMAND_NAME_DP);
        break;
    case COMMAND_CODE_ST:
        sprintf(sKey, "%s", COMMAND_NAME_ST);
        break;
    case COMMAND_CODE_CE:
        sprintf(sKey, "%s", COMMAND_NAME_CE);
        break;
    default:
        Log::log(0, "CommandCode of  %d is not found", commandCode);
        return ;
    }
    if (request == 1)
        strcat(sKey, "-Request");
    else
        strcat(sKey, "-Answer");

    if (scenario != "")
    {
        strcat(sKey, "_");
        strcat(sKey, scenario.c_str());
    }

    DictCommand *pDictCommand;
    if (!(m_pDictCommandIndex->get(sKey, &pDictCommand)))
    {
        Log::log(0, "Command(name_scenario): %s is not found", sKey);
        return ;
    }

    string str_Value = strValue;
    string::size_type loc1, loc2=0;
    while ((loc1=str_Value.find("[")) != string::npos)
    {
        loc2 = str_Value.find("]");
        str_Value = str_Value.erase(loc1, loc2-loc1+1);
    }

    char sKey1[256] = {0};
    strcpy(sKey1, str_Value.c_str());
    AvpField *pAvpField;
    if (valueType==1)
    {
        if (!(pDictCommand->m_pAvpFieldNameIndex->get(sKey1, &pAvpField)))
        {
            Log::log(0, "AvpName: %s is not found in Command(name_scenario) %s", sKey1, sKey);
            return ;
        }
    }
    else if (valueType==2)
    {
        if (!(pDictCommand->m_pAvpFieldCodeIndex->get(sKey1, &pAvpField)))
        {
            Log::log(0, "AvpCode: %s is not found in Command(name_scenario) %s", sKey1, sKey);
            return ;
        }
    }
    else if (valueType!=1 || valueType!=2)
    {
        Log::log(0, "valueType %d is not know", valueType);
        return ;
    }
#if DEBUG_TAVPPARSER
    Log::log(0, "AvpValue: %s  is found in Command(name_scenario) %s",
             sKey1, sKey);
#endif

    avpField.m_avpName = pAvpField->m_avpName;
    avpField.m_avpCode = pAvpField->m_avpCode;
    string &strAvpName = avpField.m_avpName;
    string &strAvpCode = avpField.m_avpCode;
    string::size_type loc3, loc4 = 0;
    loc1 = 0;
    while ((loc1=strValue.find(".", loc1)) != string::npos)
    {
        if ((loc3=strValue.find_last_of("]", loc1)) != string::npos)
        {
            loc2 = strValue.find_last_of("[", loc1);
            if (valueType == 1)
            {
                loc4 = strAvpCode.find(".", loc4);
                strAvpCode.insert(loc4, strValue, loc2, loc3-loc2+1);
            }
            else
            {
                loc4 = strAvpName.find(".", loc4);
                strAvpName.insert(loc4, strValue, loc2, loc3-loc2+1);
            }
            strValue.erase(loc2, loc3-loc2+1);
            loc1 = loc1-(loc3-loc2);
            loc4 = loc4+(loc3-loc2+1)+1;
        }
        else
        {
            if (valueType == 1)
            {
                loc4 = strAvpCode.find(".", loc4);
            }
            else
            {
                loc4 = strAvpName.find(".", loc4);
            }
            loc1++;
            loc4++;
        }
    }
    if ((loc3=strValue.find_last_of("]")) != string::npos)
    {
        loc2 = strValue.find_last_of("[");
        if (valueType == 1)
            strAvpCode.append(strValue, loc2, loc3-loc2+1);
        else
            strAvpName.append(strValue, loc2, loc3-loc2+1);
        strValue.erase(loc2, loc3-loc2+1);
    }

    avpField.m_avpType = pAvpField->m_avpType;

}




