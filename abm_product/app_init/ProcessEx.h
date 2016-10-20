#ifndef PROCESSEX_H_HEADER_INCLUDED_B41FE682
#define PROCESSEX_H_HEADER_INCLUDED_B41FE682

#include "InDependProcess.h"
#include "CommandCom.h"

#define DEFINE_MAIN_EX(x) ProcessEx *GetProcess () { return (new x()); }

//##ModelId=4BE0109B0091
class ProcessEx : public InDependProcess,public CommandCom
{
public:
	ProcessEx();
	virtual ~ProcessEx();
};



#endif /* PROCESSEX_H_HEADER_INCLUDED_B41FE682 */
