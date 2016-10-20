#ifndef			_SYS_PARAM_DEFINE_H_
#define			_SYS_PARAM_DEFINE_H_

//定义系统参数的信息结构
#define     MAX_SECTION_NAME_LEN 64
#define     MAX_PARAM_NAME_LEN 64 
#define     MAX_INI_VALUE_LEN  64
#define     MAX_SECTION_LEN_INIT                64
#define     MAX_KEY_NAME_LEN        512 
#define     MAX_KEY_VALUE_LEN       512 
#define     MAX_REMARK_LEN              256
#define     MAX_INI_KEY_NUM             4096
#define     MAX_INI_KEY_SIZE            20480   //默认参数共享内存的大小
#define     MAX_KEY_NUM                 64
#define     MAX_SECTION_NUM         128
#define     MAX_STACK_LEN                   3
 
#define     MAX_DATE_LEN                    16
#define     MAX_TMP_STRING_LEN      MAX_KEY_NAME_LEN+MAX_DATE_LEN

#define     EFF_DATE_NAME               "effdate"           //判断参数名字是否是eff_date
#define     NEW_VALUE                       "new_value"
#define     OLD_VALUE                       "value"
 
#define   MAX_SECTION_LEN  32
#define     DEFAULT_LOCK_KEY            ftok("initserver",'A')

#include <string>
using namespace std;
#define            MAX_SECTION_LEN_INIT                            64
typedef enum { OP_NEW=1,OP_FIX,OP_DEL} OP_FLAG;
typedef struct
{
     char sSectionName[MAX_SECTION_LEN_INIT];
     char sKeyName[MAX_KEY_NAME_LEN];
     char sKeyValue[MAX_STACK_LEN][MAX_KEY_VALUE_LEN];
    char sEffDate[MAX_DATE_LEN];
     OP_FLAG flag;
}stIniCompKey;
typedef	struct _SYS_PARAM_INFO_
{
	char sSectionName[32];
	char sParamName[32];				//参数名字
	char sParamValue[32];					//参数修改后值
	char sParamOrValue[32];			//参数修改前的值
	char sDate[16];								//参数修改时间
}SysParamInfo;

class SysParamHis
{
public:
	SysParamHis();
	SysParamHis(const SysParamHis& param);
	SysParamHis& operator = (const SysParamHis& param);
	char sSectionName[MAX_SECTION_NAME_LEN];
	char sParamName[MAX_PARAM_NAME_LEN];				//参数名字
	char sParamValue[MAX_INI_VALUE_LEN];					//参数修改后值
	char sParamOrValue[MAX_INI_VALUE_LEN];			//参数修改前的值
	char sDate[16];								//参数修改时间
};
	bool splitParamInfo(string& strContent,string& strSection,
					string& strParam,string& strValue);
void TransIniKeyToSysParamHis(SysParamHis& oSysParamHis,
							  stIniCompKey& oIniCompKey);

void badalloc(bad_alloc &ba);

#endif

