/*ver:1*/



#ifndef _DCC_PARAMS_MGR_H__

#define _DCC_PARAMS_MGR_H__



#include "Environment.h"

#include "TOCIQuery.h"

#include "dcc_ConstDef.h"

#include "Process.h"

class dccParamsMgr

{

public:

    dccParamsMgr()

    {

        memset(m_sSessionID,0,sizeof(m_sSessionID));

        m_iEndToEnd = 0;

        m_iHopByHop = 0;

        m_iNextVal = 0;

    }

    ~dccParamsMgr()

    {};



    void getDccHeadValue(char *sServiceContext);



    char m_sSessionID[SESSION_ID_LENGTH];

    unsigned int m_iHopByHop;

    unsigned int m_iEndToEnd;

private:

    unsigned int getNextSeq(char *sSeqName);

    unsigned int getNextValue();

    unsigned int m_iNextVal;

};



#endif



