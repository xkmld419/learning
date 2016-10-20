// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "ParamDefineMgr.h"
#include "string.h"
#include "Environment.h"
#include "HashList.h"
#include <stdio.h>
//#include "Environment.h"

bool ParamDefineMgr::getParam(char const *_sSegment, char const *_sCode, 
char *_sValue)
{
	char sSql[1000];
	memset(sSql,0,1000);
	
	if (!_sValue)
		return false;
	
	sprintf(sSql,
		" select trim(param_value) param_value  from b_param_define "
		" where param_segment = '%s' and param_code = '%s' ",
		_sSegment, _sCode
	);
	
	DEFINE_QUERY(qry);
	qry.close();
	qry.setSQL(sSql);
	qry.open();
	
	if (!qry.next())
		return false;
	
	strcpy(_sValue, qry.field(0).asString());
	
	return true;
}

long ParamDefineMgr::getLongParam(char const *sSegment, char const *sCode)
{
	char sSql[1000];
	memset(sSql,0,1000);

    char sTemp[32];
	
	sprintf(sSql,
		" select trim(param_value) param_value  from b_param_define "
		" where param_segment = '%s' and param_code = '%s' ",
		sSegment, sCode
	);
	
	DEFINE_QUERY(qry); qry.close();
	qry.setSQL(sSql); qry.open();
	
	if (!qry.next()) {
        THROW (999);
    }
	
    
	strcpy(sTemp, qry.field(0).asString());

    
	return strtol (sTemp, 0, 10);
}

