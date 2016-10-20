#include "json/json.h"
#include <vector>
#include <string>
#include <stdio.h>
#include<sys/types.h>
#include<sys/stat.h>

using namespace std;

class JsonMgr
{
public:
    JsonMgr();
    ~JsonMgr();
    
    long writeJsonFile(char * sFileName, vector<string> & vecStr, char sDiv);
    bool writeKpiJson(char *sFileName,char* sKPI, 
        vector<string> &vecFileName , char *sOut, int iLen, int & iCnt );    
};




