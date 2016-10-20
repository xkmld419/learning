
#ifndef PARAMINFOINTERFACE_H_HEADER_INCLUDED
#define PARAMINFOINTERFACE_H_HEADER_INCLUDED

#include "ParamShmMgr.h"


class ParamInfoInterface : public ParamShmMgr
{
public:
    ParamInfoInterface();
    ~ParamInfoInterface();
	bool reBindData();
	bool updateInfo(int iPID,long lDate,int iFlashTimes,char *sDesc,int iProcessID=0);
	bool checkTable();
private:




};





#endif  //PARAMINFOINTERFACE_H_HEADER_INCLUDED


