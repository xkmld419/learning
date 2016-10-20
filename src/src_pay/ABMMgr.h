/*
*	ABM中心处理类
*/

#ifndef __ABMMGR_H_INCLUDED_
#define __ABMMGR_H_INCLUDED_


#include "Application.h"

class ABMMgr : public Application
{
public:
	
	ABMMgr() {};
	
	virtual ~ABMMgr() {};
    
	virtual int init(ABMException &oExp);
	
	virtual int run();
	
	virtual int destroy();
	
};

DEFINE_MAIN(ABMMgr)

#endif /*__ABMMGR_H_INCLUDED_*/
