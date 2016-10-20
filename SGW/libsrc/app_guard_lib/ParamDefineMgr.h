// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef __PARAM_DEFINE_MGR_H_
#define __PARAM_DEFINE_MGR_H_

class ParamDefineMgr {
 public:
	static bool getParam(char const *_sSegment, char const *_sCode,
 char *_sValue);	
    static long getLongParam(char const *sSegment, char const *sCode);
    
};

#endif




