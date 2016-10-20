#include "regular.h"

int Regex::regexComp(const char* reg) 
{
	if (!reg) {
		compNo = -1;
		return compNo;
	}
	
	compNo = regcomp (&regex, reg, REG_EXTENDED);
	if (compNo) { 
	  int len = regerror (compNo, &regex, errbuf, sizeof (errbuf)); 
	} 

	return compNo;
}

int Regex::exec(const char* str) 
{
	if (!str) {
		execNo = -1;
		return execNo;
	}

	if (compNo) {
		return compNo;
	}

	
	execNo = regexec (&regex, str, 1, &pMatchs, 0); 

	if (!execNo) {
		if (0 != pMatchs.rm_so || pMatchs.rm_eo != strlen(str)) {
			execNo = -3;
		}
	}
	else if (execNo != REG_NOMATCH) { 
		int len = regerror (execNo, &regex, errbuf, sizeof (errbuf)); 
	} 

	return execNo;
}

