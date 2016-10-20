#ifndef TYPEDEF_H_
#define TYPEDEF_H_
#include <vector>
#include <map>
#include <string>
#include <stdlib.h>

//const char* TYPE_MSG_CCA = "CCA";
//const char* TYPE_MSG_CCR = "CCR";
//const char* TYPE_DILIMITER_A = "^^^";
//const char* TYPE_DILIMITER_B= "|";
#define TYPE_DILIMITER_A "^^^"
#define TYPE_DILIMITER_B "|"

#define TYPE_MSG_CCA "CCA"
#define TYPE_MSG_CCR "CCR"

const int MAX_STRING_LENGTH = 2048;
const int MAX_SQL_LENGTH = 4096;
const int MAX_EVENT_NUMBER = 3;
//DCC部分定义
#define CC_Request_Type  "4"
#define CC_Request_Number  "0"
const int Application_id = 0;

#define FREE_X(x) {if(NULL != x) delete[] x; x = NULL;}
#define FREE(x)  {if(NULL != x) delete x; x = NULL;}
#define LOG_DEBUG(szLog) CLogManage::WriteLog(1, __FILE__, __LINE__, szLog);
#endif

