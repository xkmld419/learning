#ifndef __READ_PARAM_CONF_H_INCLUDED_
#define __READ_PARAM_CONF_H_INCLUDED_

class ABMException;
class ReadCfg;

struct SHMParamConf
{
    unsigned long m_ulKey;
    unsigned int  m_uiNum;
    unsigned int  m_uiStrLen;
    unsigned int  m_uiMulti;
};

class ReadParamConf
{
public:
    static ReadCfg *m_poConf;
    
    static int getSHMParamConf(ABMException &oExp, const char* pKey, SHMParamConf& oConf);
    
    static int getSemConf(const char* pKey, char *pValue, int iSize);
};

#endif/*__READ_PARAM_CONF_H_INCLUDED_*/
