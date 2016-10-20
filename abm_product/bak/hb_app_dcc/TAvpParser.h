#ifndef _T_AVP_PARSER_H
#define _T_AVP_PARSER_H

#include "diameter_parser_api.h"
#include "avplist.h"
#include "g_avplist.h"
#include "comlist.h"

#include "HashList.h"
#include "Log.h"
#include "dcc_ParamsMgr.h"
#include <string>
#include <map>
#include <utility>
using namespace std;
#include <stdlib.h>
#include <string.h>

#define COMMAND_CODE_CC 272
#define COMMAND_CODE_RA 258
#define COMMAND_CODE_AS 274
#define COMMAND_CODE_DW 280
#define COMMAND_CODE_DP 282
#define COMMAND_CODE_ST 275
#define COMMAND_CODE_CE 257

#define COMMAND_NAME_CC "Credit-Control"
#define COMMAND_NAME_RA "Re-Auth"
#define COMMAND_NAME_AS "Abort-Session"
#define COMMAND_NAME_DW "Device-Watchdog"
#define COMMAND_NAME_DP "Disconnect-Peer"
#define COMMAND_NAME_ST "Session-Termination"
#define COMMAND_NAME_CE "Capabilities-Exchange"

#define BODY_LEN 8192

#define DEBUG_TAVPPARSER 0

enum eCommandCode
{
COMMAND_CC = 272,
COMMAND_DW= 280,
COMMAND_DP = 282,
COMMAND_CE = 257
};

class TAvpHead 
{ 
public:
    int setHeadValue(const char *sAvpHead);
    void toCString(char *sAvpHead);

    int  Version;
    unsigned int  Length;
    int  CommandCode;
    char CommandFlag;
    int  AppId;
    unsigned int  HopId;
    unsigned int  EndId;
    int initHead(eCommandCode,bool);

private:
    char* subStr2Int(char *pos1, const char *delim, unsigned int &num);
};

class TAVPObject : public map<string, string>
{
public:
    int setObjectValue(const char *sAvpObject);
    void toCString(char *sAvpObject);
public:
    TAvpHead avpHead;
};


class DictCommand;
class AvpField;

class TAvpParser 
{
public:
    TAvpParser():
        m_oMsg(auto_ptr<AAAMessage>(new AAAMessage)),
        m_oDm(auto_ptr<AAADictionaryManager>(new AAADictionaryManager)),
        m_bDispFlag(false), m_bBuiledBody(false), m_option(PARSE_LOOSE)
    { }

    TAvpParser(char *dict_file) :
        m_oMsg(auto_ptr<AAAMessage>(new AAAMessage)),
        m_oDm(auto_ptr<AAADictionaryManager>(new AAADictionaryManager)),
        m_bDispFlag(false), m_bBuiledBody(false), m_option(PARSE_LOOSE)
    {
        m_oDm->init(dict_file);
        initDictCommand();
    }

    virtual ~TAvpParser() 
    { }

    void initDictionary(char *dict_file) 
    {
        ::RemoveDictionary();
        m_oDm->init(dict_file);
        initDictCommand();
    }
    
    // 设置全局合法检测标识head_check和avp_check，0：不检测，非0：检测
    void setCheckOption(int head_check, int avp_check) 
    {
        if (head_check == 0)
            m_option = PARSE_LOOSE;
        else
            m_option = PARSE_STRICT;

        ::setAvpCheck(avp_check);
    }
    
    //获取DCC解析log
    string& getDccLog() 
    {
        return m_strDccLog;
    }

    //获取失败的Avp字符串
    string& getFailedAvp() 
    {
        return m_strFailedAvp;
    }

    // 设置是否打印log标识
    void setDispFlag(bool bDispFlag)
    {
        m_bDispFlag = bDispFlag;
    }

public:
    // 解析消息包的头信息
    int ParseHead(unsigned char *pDataBuffer, unsigned int iDataSize, TAvpHead *pAvpHead);

    // 解析出某一个DCC消息体内容
    int ParseBody(unsigned char *pDataBuffer, unsigned int iDataSize, TAVPObject *pAVPObject);

    // 对于传入消息体进行编码
    int BuildBody(TAVPObject *pAvpObject, unsigned char *pDataBuffer, unsigned int *iDataSize);

    // 对消息头进行编码
    int BuildHead(TAvpHead *pAvpHead, unsigned char *pDataBuffer, unsigned int *iDataSize);

    // 判断消息头，根据基本格式测试的要求，跟业务相关的AVP不做判断
    int IsValidMsg(TAVPObject *pAVPObject);

    // 设置消息携带的业务类型
    void setServiceType(const string strServiceType);
    // 获取消息携带的业务类型
    string& getServiceType();

    // 获取解析后的TAVPObject
    void getAVPObject(TAVPObject *pAVPObject);

    
private:
    // 释放Msg acl
    void releaseMsgAcl()                                    
    {
        m_oMsg->acl.releaseContainers();
    }

    void initDictCommand();
    void initGroupedAvp(AAADictionary* pAAADictionary, const char* avpNamePrefix, const char* avpCodePrefix, DictCommand *pDictCommand);
    void initQualiedAvp(AAAQualifiedAvpList* pQualifiedAvpList, const char* avpNamePrefix, const char* avpCodePrefix, DictCommand *pDictCommand);


    void initFixedAvp(AAADictionary* pAAADictionary, const char* prefix, DictCommand *pDictCommand);

    template <class T> int _setAvpValue(const char *avp_name1, T &data, AAA_AVPDataType t);
    template <class T> int _getAvpValue(const char *avp_name1, T &data);

    void recordHeadLog();

    void getAllAvp(AAAAvpContainerList *orig_acl, vector<string> &sAvpName, TAVPObject *pAVPObject);
    void getAvpName(vector<string>& sAvpName, string& avp_name);
    void getAvpData(AAAAvpContainerEntry *e, int pos, int size, vector<string>  &sAvpName, TAVPObject *pAVPObject);

    void getAvpField(int commandCode, int request, const string &scenario, string strValue, int valueType, AvpField &avpField);
    int setMsgAvp(int commandCode, int request, const string &scenario, TAVPObject *pAVPObject);
    

private:
    auto_ptr<AAAMessage>           m_oMsg;      // DCC消息内部结构
    auto_ptr<AAADictionaryManager> m_oDm;       // DCC协议字典管理器

    bool          m_bDispFlag;          // 是否打印到屏幕
    bool          m_bBuiledBody;        // 是否对消息体已进行编码
    ParseOption   m_option;             // 消息头解包或打包时，是否对消息头属性进行校验

    TAvpHead m_avpHead;               // DCC消息头
    string  m_strDccLog;              // DCC消息log字符串
    string  m_strFailedAvp;           // 失败的Avp字符串
    string  m_strServiceType;         // 标识CCR或CCA携带的业务类型，取Service-Context-Id的前缀，静态数据都取Para 

    HashList<DictCommand *> *m_pDictCommandIndex;       // key 值为commandCode_scenario-id或commandCode
};

class AvpField
{
public:
    AvpField(string avpName, string avpCode, AAA_AVPDataType avpType) :
        m_avpName(avpName), m_avpCode(avpCode), m_avpType(avpType)
    { }

    AvpField() :
        m_avpName(""), m_avpCode(""), m_avpType(AAA_AVP_DATA_TYPE)
    { }

    string m_avpName;
    string m_avpCode;
    AAA_AVPDataType m_avpType;
};

class DictCommand
{
public:
    DictCommand()
    { }

    ~DictCommand()
    {
        delete []m_pAvpFieldNameIndex; m_pAvpFieldNameIndex = NULL;
        delete []m_pAvpFieldCodeIndex; m_pAvpFieldCodeIndex = NULL;
        m_vecFixedAvp.clear();
    }

    HashList<AvpField *> *m_pAvpFieldNameIndex;       // key 值为avp_name1.avp_name2.avp_name3
    HashList<AvpField *> *m_pAvpFieldCodeIndex;       // key 值为avp_code1.avp_code2.avp_code3
    vector<string> m_vecFixedAvp;
};


#endif

