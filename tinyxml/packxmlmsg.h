#ifndef _PACK_XML_MSG_H_
#define _PACK_XML_MSG_H_

#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include "abmobject.h"
#include "abmcmd.h"
#include "Application.h"
#include "MessageQueue.h"
#include "abmtime.h"
#include "LogV2.h"
#include "tinyxml.h"
#include "ReadXmlConf.h"

class PackXMLMsg : public Application {

public:
	
	PackXMLMsg(): m_poXmlDoc(0)/*, m_fp(0)*/ {}
	
	virtual ~PackXMLMsg()
	{
	    if (m_poXmlDoc != NULL) delete m_poXmlDoc;
	    //if (m_fp != NULL) fclose(m_fp);
    }
    
	virtual int init(ABMException &oExp);
	
	virtual int run();
	
	virtual int destroy();
	
private:
    //打包
    int toXml();
    
    //3.4.6.1 登录鉴权
    int packLoginA();
    
    //3.4.6.2 支付
    int packChargeA();
    
    //3.4.6.3 绑定手机号码查询
    int packBindNbrA();
    
    //3.4.6.4 余额划拨查询
    int packQryBalA();
    
    //3.4.6.5 余额划拨
    int packTransBalA();

    //3.4.6.6 余额划拨规则查询
    int packQryTransRuleA();

    //3.4.6.7 余额划拨规则设置
    int packResetTransRuleA();

    //3.4.6.8 支付密码修改
    int packResetPassWdA();
    
    //3.4.6.9 支付能力状态查询
    int packQryPayStatusA();
    
    //3.4.6.10 支付状态变更
    int packResetPayStatusA();
    
    //3.4.6.11 支付规则查询
    int packQryPayRulesA();
    
    //3.4.6.12 支付规则设置
    int packResetPayRulesA();
    
    //3.4.6.13 余额查询
    int packQueryA();
    
    //3.4.6.14 充退记录查询
    int packRechargeRecA();
    
    //3.4.6.15 交易记录查询
    int packQryPayLogA();
    
    //3.4.6.16 电子帐单查询
    int packQryRechargeBillA();
    
private:
    
    TiXmlDocument *m_poXmlDoc;
    
    //FILE *m_fp;
    
    //char m_sXmlFile[64];
	
};

DEFINE_MAIN(PackXMLMsg)


#endif