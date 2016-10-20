/*VER: 2*/ 
#include "AttrValueCalc.h"
#include "../cparser/exception.h"

AttrValueCalc::~AttrValueCalc()
{
}

int AttrValueCalc::run(char * pFormula,OfferInsQO * pQO,char * sDate)
{
	if(!pFormula || !pQO ||!sDate) return 0;
	char name[200];
	memset(name,0,sizeof(name));
	memset(m_sFormula,0,sizeof(m_sFormula));
	strcpy(m_sFormula,pFormula);
	m_poOfferInsQO = pQO;
	memset(m_sDate,0,sizeof(m_sDate));
	strcpy(m_sDate,sDate);
	this->Clear();
	if (!m_bParsed)
	{
		if(!parse ())
		{
			cout<<"== formula ["<<m_sFormula<<"] parse result is false =="<<endl;
			return 0;
		}
	}
	//this->ClearResult();
	this->Run ();
	m_bParsed =  false;
	return this->Result("result")->asInteger();
}

bool AttrValueCalc::parse()
{
	if (m_bParsed)
		return true;
	try {
		this->SetSource (m_sFormula, strlen(m_sFormula));
		this->Parse ();
	} catch (TException) {
		return false;
	} catch (...) {
		return false;
	}
	m_bParsed = true;
	return true;
}

double AttrValueCalc::Sysinput(const char *name)
{
	if (name[0] == 'T')
	{
		int iAttr = atoi((char *)name+1);
		char * sVal = m_poOfferInsQO->getOfferAttr(iAttr,m_sDate);
		if(sVal){
			return atol(sVal);
		}
	}
	return 0;
}

char *AttrValueCalc::SysinputS(const char *name)
{
	return NULL;
}
