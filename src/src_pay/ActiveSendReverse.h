// ActiveSendReverse.h 对应省里未返回CCA的情况主动发送冲正请求

#ifndef __ACTIVE_SEND_REVERSE_H_INCLUDED_
#define __ACTIVE_SEND_REVERSE_H_INCLUDED_

#include "ReadAppConf.h"
#include "LogV2.h"
#include "abmcmd.h"
#include "Application.h"

class ActiveSendReverse : public Application
{
public:
	ActiveSendReverse() {};
	
	virtual ~ActiveSendReverse() {};
		
	virtual int init(ABMException &oExp);
	
	virtual int run();		
	
	virtual int destroy();
	
};

DEFINE_MAIN(ActiveSendReverse)

#endif /*__ACTIVE_SEND_REVERSE_H_INCLUDED_*/


