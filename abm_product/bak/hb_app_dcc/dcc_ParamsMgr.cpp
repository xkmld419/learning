/*ver:1*/
#include "dcc_ParamsMgr.h"

/*dcc的seq需要设定最大值不超过unsigned int值4294967295，并且循环*/
/*使用seq是为了多个进程获取到的值不重复*/
unsigned int dccParamsMgr::getNextSeq(char * sSeqName)
{
	char sSql[100]={0};	
	char sValue[33] = {0};
	TOCIQuery qry(Environment::getDBConn());

	sprintf(sSql,"select to_char(%s.nextval) nextval from dual  ",sSeqName);
	qry.setSQL(sSql);
	qry.open();
	if (!qry.next())
	{
		char sInfo[100];
		sprintf(sInfo,"请检查seq[%s]创建是否正确.",sSeqName);
		throw(sInfo);
	}

	strcpy(sValue,qry.field(0).asString());
	m_iNextVal= strtoul(sValue,NULL,10);
	return m_iNextVal;
}

unsigned int dccParamsMgr::getNextValue()
{
	static long lFlag = 0;
	
	if (!lFlag) 
		getNextSeq("SEQ_DCC_HOPBYHOP");
	else
		m_iNextVal++;
        
    	lFlag++;
	lFlag = lFlag%DCC_SEQ_STEP;
    
    return m_iNextVal;
}

void dccParamsMgr::getDccHeadValue(char * sServiceContext)
{	
	m_iHopByHop = m_iEndToEnd = getNextValue();	
	if(0==sServiceContext)
	{
		sprintf(m_sSessionID,";%u;%u",(int)time(NULL),m_iHopByHop);
		return ;
	}	
sprintf(m_sSessionID,"%s;%u;%u",sServiceContext,(int)time(NULL),m_iHopByHop);
}


