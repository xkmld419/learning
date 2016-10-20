#ifndef SGW_PARAM_H
#define SGW_PARAM_H

#include "SysParamClient.h"
#include "MBC.h"
#include "CommandCom.h"
#include "GroupDroitMgr.h"
#include "Log.h"
#include "ThreeLogGroup.h"
#include "MBC_ABM.h"
#include "BaseProcess.h"
#define	HBPARAM_WAIT_TIME_NEED	100

class SgwParam : public BaseProcess 
{
public:
	SgwParam();
	~SgwParam();	
	int run();
};

#endif
