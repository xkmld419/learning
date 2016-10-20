/*VER: 2*/ 
#ifndef TIBS_CHECK_FUNCTION_H_
#define TIBS_CHECK_FUNCTION_H_
/*-----------------------------------------
* 定义一套全局的宏，用于标识FUNC_ID
* 这些宏以前缀CK_开始，以防止重名
-------------------------------------------*/
#define     CK_IS_DEC           1   //检测是否为十进制数，eg:测试主叫是否为15位全数字。
#define     CK_BEGIN_WITH       2   //检测字符串，是否以某个号头开头。
#define     CK_GET_ORG          3   //检测能否获取ORG_ID
#define     CK_AREA_CODE        4   //校验是区号是否合法
#define     CK_PROVIN_CODE      5   //检测文件名中的省代码是否与属性值的省归属局相同
#define     CK_DATE_FORMAT      6   //检测时间格式是否合法，默认为"YYYYMMDDHH24MISS"
#define     CK_BEYOND_DATE      7   //比较时间差值
#define     CK_LARGER_THEN      8   //大于某个数
#define     CK_LESS_THEN        9   //小于某个数
#define     CK_GET_ORG_2        10  //根据MSC、LAC、CELL_ID获取ORG_ID
#define     CK_IS_OCT           11  //检测是否为八进制数
#define     CK_IS_HEX           12  //检测是否为十八进制数
#define     CK_EQUAL            13  //等于某个数
#define     CK_LENGTH           14  //检测长度是否在指定范围
#define     CK_DO_FUNC          15  //执行其他函数
#define     CK_IS_AREACODE      16  //检测区号是否合法（方法二）
#define     CK_GET_ATTR         17  //获取属性值
#define     CK_CMP_TWO_FUNC     18  //比较两个函数的执行结果是否相同。
#define     CK_GET_CARRIER      19  //获取承载类型
#define     CK_IS_MSC           20  //检测MSC是否合法
#define     CK_PIPE             21  //管道　前一个函数的执行结果，将作为后一个函数的输入。
#define     CK_FORMAT_CARRIER   22  //规整承载类型
#define     CK_FORMAT_AREACODE  23  //规整被叫区号
#define     CK_GET_PARTNER      24  //获取运营商ID
#define     CK_IS_INNER         25  //判断主、被叫是否为网内用户
#define     CK_FORMAT_0086      26  //规整掉被叫可能出现的0086国家代码，使其成为标准区号
#define     CK_GET_LONG_TYPE    27  //规整长途类型
#define     CK_GET_ROAM_TYPE    28  //规整漫游类型
#define     CK_MUX              29  //乘法
#define     CK_DIV              30  //除法
#define     CK_CMP_CHAROFSTRING		31  //判断字符串中字符是否在规定范围内
#define	  CK_SUBSTR					32 //求子字符串，与给定的字符串比较
#define	CK_ZONE				33 //判断取值是否在给定区表中，完全匹配；
#define	CK_ZONE_MAX		34 //判断取值是否在给定区表中，最长匹配；

/*--------------CheckFunction Class----------------------*/
#include "RecordCheck.h"

using namespace std;

class CheckFunction
{
public:
	CheckFunction();
	~CheckFunction();

	//执行与iFuncID对应的函数，这些函数的作用只是对strVal里的值进行判断，
	//返回true或false，不对数据做任何改变。这样的函数，又叫谓词-pridicate
	static bool doFunction(int iFuncID, const EventSection *poEventSection, 
		int iTAG, const CheckArgument *poFirstArgument, string *psResult=NULL,
		string *psInput=NULL);
		
    static bool isNumber(const char *str, const CheckArgument *poTable,
        const char *pFormat=NULL);
        
	static bool beginWith(const char *str, const CheckArgument *poTable, 
	    char *pHead=NULL);
	    
	static bool getOrgID(const char *sNumber, int &iOrgID, 
        const char* pTime, const CheckArgument *poTable, char *pHead=NULL,
        int *piShift=NULL);
        
    static bool getOrgID(const EventSection *poEventSection, int &iOrgID, 
       const CheckArgument *poTable, char *pHead=NULL);
        
    static bool getAreaCode(const char *sNumber, char *pAreaCode, int iSize, 
        const char *pTime, const CheckArgument *poTable);
        
    static bool isBeyondDate(const char *pTime, const CheckArgument *poTable,char *sFileName);
    
    static bool isAreaCode(const char *sNumber, char *pAreaCode, int iSize, 
        const char *pTime, const CheckArgument *poTable);
        
    static bool cmpTwoFunc(const EventSection *poEventSection, 
        const CheckArgument *poTable);
    static bool formatCarrier(const EventSection *poEventSection, 
        int &iCarrierType, char *psResult=NULL);
    static bool isMSC(const char *sMSC, const CheckArgument *poTable, 
        char *pHead);
    static bool pipe(const EventSection *poEventSection, 
        const CheckArgument *poTable, char *pResult=NULL);
    static bool formatAreaCode(const EventSection *poEventSection,
        char *psResult=NULL);
    static bool getPartnerID(const char *sAccNBR, const char *pTime, 
        int &iPartnerID);
    static bool isInner(const EventSection *poEventSection, const char *pTime);
    static bool getLongTypeAndRoamID(const EventSection *poEventSection, 
    int &iLongType, int &iRoamID);
    //yuanp 增加判断字符串包含的字符是否合法
    static bool cmpCharofString(const char *str,const CheckArgument *poTable,char &cNotFind);
    static bool cmpSubStr(const char *str,const CheckArgument *poTable);
    static bool check_zone(const char *str,const CheckArgument *poTable,int iMaxFlag);    
private:
    static bool format0086(const char *fromNBR, char *toNBR, int *trimLen=NULL, 
        int *piShift=NULL);
    static void to_Lower(char *sStr);
    static void to_Upper(char *sStr);
    static bool getFileTime(char *sTag,char *sFileTime,char *sFileName);
};

#endif /* End of _CHECK_FUNCTION_H */
