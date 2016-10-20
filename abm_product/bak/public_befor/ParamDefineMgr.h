/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef __PARAM_DEFINE_MGR_H_
#define __PARAM_DEFINE_MGR_H_

#include "TOCIQuery.h"

class ParamDefineMgr {
 public:
	static bool getParam(char const *_sSegment, char const *_sCode,
 char *_sValue, int _iMaxLen=-1);
	static bool getParam(char const *_sSegment, char const *_sCode,
 char *_sValue, TOCIQuery &qry,int _iMaxLen=-1);
    static long getLongParam(char const *sSegment, char const *sCode);
    static long getLongParam(int iBillFlowID,char const *sSegment, char const *sCode);
    static bool setParam(char const *_sSegment, char const *_sCode, char const *_sValue);
    static bool setParam(char const *_sSegment, char const *_sCode,char const *_sValue, char const *_sName);
    static bool delParam(char const *_sSegment, char const *_sCode);
    
};

#endif

