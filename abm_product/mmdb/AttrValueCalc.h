/*VER: 2*/ 
#include "TScriptParser.h"
#include "UserInfoReader.h"

#ifndef ATTR_VALUE_CALC_H_INCLUDED
#define ATTR_VALUE_CALC_H_INCLUDED

class AttrValueCalc : public ScriptParser
{
  public:
    int run(char * pFormula,OfferInsQO * pQO,char * sDate);

    bool parse();

    virtual ~AttrValueCalc();
    double Sysinput(const char *name);
    char *SysinputS(const char *name);

  private:
    char m_sFormula[2000];    
	OfferInsQO * m_poOfferInsQO;
	char m_sDate[16];
	bool m_bParsed;
};

#endif
