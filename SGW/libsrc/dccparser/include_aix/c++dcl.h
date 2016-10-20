//# ##########################################################
//# 
//# c++dcl.h  --  the c++ libary declaration 
//# Language:    C++
//# 
//# Authors:     J.Rui
//# 
//# Copyright (C) 2001 by J.Rui, Linkage. All Rights Reserved.
//# 
//# Revision History: 
//#  
//# ##########################################################

#ifndef  CPLUSPLUSDECL_H__ 
#define  CPLUSPLUSDECL_H__

#if defined(hpux) || defined(__hpux__) || defined(__hpux)
#define _HPUX
#else
#undef	_HPUX
#endif

//# IOSTREAM DECLARATION BLOCK
#ifdef _HPUX
#include "platform.h"
#include "platform.h"
#include "platform.h"
#else
#include "platform.h"
#include "platform.h"
#include "platform.h"
#endif

//# 
#include <string>

//# 
#include <vector>

//#
#include <list>

//#
#include <set>

//#
#include <map>

//#
#include <stack>

//#
#include <algorithm>

//#
#include <functional>

//#
#include <stdexcept>

//# NAMESPACE DECLARATION BLOCK  
#define NAMESPACE_DISABLE
#ifdef _HPUX
////#define std
#else
#include "platform.h"
#endif

#endif



