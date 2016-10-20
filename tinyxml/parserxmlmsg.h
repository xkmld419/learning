#ifndef _PARSER_XML_MSG_H_
#define _PARSER_XML_MSG_H_

#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include "abmobject.h"
#include "abmcmd.h"
#include "Application.h"
#include "MessageQueue.h"
#include "LogV2.h"
#include "tinyxml.h"
#include "ReadXmlConf.h"

class ParserXMLMsg : public Application {

public:
	
	ParserXMLMsg(): m_poXmlDoc(0)/*,m_fp(0)*/ {}
	
	virtual ~ParserXMLMsg() 
	{
	    if (m_poXmlDoc != NULL) delete m_poXmlDoc;
	    //if (m_fp != NULL) fclose(m_fp);
    }
    
	virtual int init(ABMException &oExp);
	
	virtual int run();
	
	virtual int destroy();

private:
    int selfDccHead( ABMCCR *pCCR);
    //解包
    int toStruct();
	
	//心跳
    int parseReady(TiXmlHandle &SrvInfoHandle);
    
    //3.4.6.1 登录鉴权
    int parseLoginR(TiXmlHandle &SrvInfoHandle);
    
    //3.4.6.2 支付
    int parseChargeR(TiXmlHandle &SrvInfoHandle);
    
    //3.4.6.3 绑定手机号码查询
    int parseBindNbrR(TiXmlHandle &SrvInfoHandle);
    
    //3.4.6.4 余额划拨查询
    int parseQryBalR(TiXmlHandle &SrvInfoHandle);
    
    //3.4.6.5 余额划拨
    int parseTransBalR(TiXmlHandle &SrvInfoHandle);

    //3.4.6.6 余额划拨规则查询
    int parseQryTransRuleR(TiXmlHandle &SrvInfoHandle);

    //3.4.6.7 余额划拨规则设置
    int parseResetTransRuleR(TiXmlHandle &SrvInfoHandle);

    //3.4.6.8 支付密码修改
    int parseResetPassWdR(TiXmlHandle &SrvInfoHandle);
    
    //3.4.6.9 支付能力状态查询
    int parseQryPayStatusR(TiXmlHandle &SrvInfoHandle);
    
    //3.4.6.10 支付状态变更
    int parseResetPayStatusR(TiXmlHandle &SrvInfoHandle);
    
    //3.4.6.11 支付规则查询
    int parseQryPayRulesR(TiXmlHandle &SrvInfoHandle);
    
    //3.4.6.12 支付规则设置
    int parseResetPayRulesR(TiXmlHandle &SrvInfoHandle);
    
    //3.4.6.13 余额查询
    int parseQueryR(TiXmlHandle &SrvInfoHandle);
    
    //3.4.6.14 充退记录查询
    int parseRechargeRecR(TiXmlHandle &SrvInfoHandle);
    
    //3.4.6.15 交易记录查询
    int parseQryPayLogR(TiXmlHandle &SrvInfoHandle);
    
    //3.4.6.16 电子帐单查询
    int parseQryRechargeBillR(TiXmlHandle &SrvInfoHandle);
	
private:
    
    TiXmlDocument *m_poXmlDoc;
    
    //FILE *m_fp;
    
    char m_sXmlFile[64];
	
};

DEFINE_MAIN(ParserXMLMsg)

#endif