#ifndef	_HBSHUTDOWN_H_
#define	_HBSHUTDOWN_H_

#include "ProcessEx.h"

class CHbShutDown
	:public ProcessEx
{
public:
	CHbShutDown(void);
	~CHbShutDown(void);
	int run();
};

#endif	

