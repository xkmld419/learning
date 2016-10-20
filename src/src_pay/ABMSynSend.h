/*
*ABM端开户资料同步请求短信下发类send端
*/

#ifndef __ABMSYN_SEND_H_INCLUDED_
#define __ABMSYN_SEND_H_INCLUDED_

//#include "Application.h"
#include "QryServSynSend.h"

class ABMSynSend :public Application
{
	public:
		ABMSynSend() {};
		
	virtual int init(ABMException &oExp);
	
	virtual int run();		
	
	virtual int destroy();
	
	private:
		PublicConndb *m_pPublicConndb;//公用数据库连接
};

DEFINE_MAIN(ABMSynSend)

#endif /*__ABMSYN_SEND_H_INCLUDE_*/
