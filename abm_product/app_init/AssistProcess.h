#ifndef	_ASSISTPROCESS_H_
#define	_ASSISTPROCESS_H_
#include "ProcessEx.h"
#include <vector>
using namespace std;
class CAssistProcess :
	public ProcessEx
{
public:
	CAssistProcess(void);
	~CAssistProcess(void);
	int run();

private:
	void GenerateParamClock(void);

	vector<SysParamHis>m_lstSysParamHis;

	bool ChangeParam(const char* pSection,const char* pParam,const char* pValue,const char* pDate=0);
	bool AddParam(SysParamHis& oSysParam);
};

#endif	

