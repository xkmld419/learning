#ifndef _BASERULE_MGR_H_
#define _BASERULE_MGR_H_

#include "BaseFunc.h"
#include "IniFileMgr.h"
#include "Log.h"
#include "HashList.h"

#define MAX_ENUM_NUM	  30  //这个值任意调整,枚举数目限制值
#define MAX_LINK_NUM	  2  //这个值任意调整,参数链接数目限制值
#define MAX_LONG_NUM	  18  //这个值就不能改大了,可以改小
#define MAX_GROUP_NUM	  100  //参数组的数目
#define MAX_GROUP_PARAM_NUM	  30  //参数组下属参数的数目
#define MAX_DISOBYE_NUM	  64  //不能出现的字符

//成功返回0
//错误信息标识返回
#define MAX_ERR	1
#define MIN_ERR	2
#define ENUM_ERR	3
#define EFF_ERR_LESS	4
#define EFF_ERR_NONEED	5
#define CHANGE_ERR	6
#define PARAM_INFO_NONEED	7
#define IN_OR_DECREASE_ERR	8
#define IN_CREASE_ERR           11
#define DE_CREASE_ERR           12 
#define MAX_MIN_SET_ERR			13
#define TIME_ERR				14
#define PARAM_INFO_NEED			15
#define PARAM_FORMAT_ERR	9
#define RULE_UNLOAD_ERR		10
#define CHAR_OR_NUM_ERR		16
#define ISNOTNUM_ERR		17
#define DISOBEY_ERR			18

// 参数规则(优先级是最大最小值,枚举,暂时不处理默认值)
struct BaseSysParamRule
{
	char sParamName[MAX_KEY_NAME_LEN];  //参数名
	char sMaxVal[MAX_LONG_NUM];	//最大值 //.min
	char sMinVal[MAX_LONG_NUM];	//最低值 //.max
	char sEnumVal[MAX_ENUM_NUM][MAX_KEY_VALUE_LEN];	//枚举值 //.enum
	int  iEnumPos; 
	char sSuggestVal[MAX_KEY_VALUE_LEN];//推荐值 //.suggest
	char sDefaultVal[MAX_KEY_VALUE_LEN];//默认值 //.default
	char sDesc[MAX_KEY_VALUE_LEN];//说明	//.desc
	char sVal[MAX_KEY_VALUE_LEN]; //.value
	int  iEffDateFlag; // 生效日期模式;1-立即生效 ;2-重启生效 ;3-参数修改要带生效日期(不配置这个字段或配置其它值暂不检测参数生效日期字段)
	int  iKeyWordFlag; // 密码字段标识;1-这个字段是密码字段 //.keyword
	int  iValueChange; // 1-只能增加 //.change; 2-不能修改 ; 3-规则匹配	;//4-只能减小
	char sValueChange[MAX_KEY_VALUE_LEN];//规则匹配
	int  iParamNeed;// >=1 必须存在的参数 
	char cArbi; // 必须仲裁的参数 --'Y'
	char sTime24Start[14]; //参数设置的开始时间值
	char sTime24End[14];   //参数设置的结束时间值 都是24小时格式
	char sLinkParamName[MAX_LINK_NUM][MAX_KEY_NAME_LEN+MAX_KEY_VALUE_LEN+1];//连接的参数:参数是 段.参数名的形式
	char sGroupParamName[MAX_SECTION_LEN+MAX_KEY_NAME_LEN];//参数所属的参数组 组名
	int  iLinkNum;//暂不使用
	int  iCharOrNum; //1-number 其余-不处理这个字段 默认都是对的
	char sDisobey[MAX_DISOBYE_NUM];   //不能出现的字符
	BaseSysParamRule *pNext;	
};

/* 
	参数段1------|-->参数1-->参数2-->参数3
	|     		
	|
	参数段2------|-->参数1-->参数2
*/
struct BaseSysSectRule
{
	char sSectName[MAX_SECTION_LEN];//参数段
	BaseSysParamRule *pFirst;
	BaseSysSectRule *next;
};


//后来新增的参数组功能使用的 记录参数段参数名这两个基本信息
struct BaseVarInfo
{
	char sSectName[MAX_SECTION_LEN];
	char sParamName[MAX_KEY_NAME_LEN]; 
	BaseVarInfo *next;
};

//add by hsir
class BaseRuleMgr
{
    public:
             		 BaseRuleMgr();
             		 ~BaseRuleMgr();
            
             		 //根据段名,参数名获取参数规则(public函数);如段名存在,参数名不存在返回段下第一个参数;段名不存在返回NULL
             		 BaseSysParamRule *getParamRule(char *sSectNameSrc,char *sParamNameSrc); 
					 bool check(const char *srcVal,bool Show=true);
					 // srcStr的格式: 段.参数=值,....
             		 bool check(char *srcVal,bool Show=true);
					 //_Debug为true不显示错误提示			 
					 void setDebugState(bool _Debug);
	public:
			 // 对参数是否合乎规则的判断,str格式为Section.param=value
			 bool paramParse(const char* str,bool Show=true);
			 bool paramParse(string str,string strEffDate);

			 // 规则说明,sSect段,sParam参数名
			 void paramExplain(char*sSect,char *sParam,bool bAfterCheck=false);
			 // 获得参数规则的说明,str格式为 段.参数
			 void paramParseExplain(const char* str);
			 // bAfterCheck=true处理这种check之后的参数规则查询
			 void paramParseExplain(bool bAfterCheck);
			 // 检测生效日期的
			 bool checkEffDate(string strWholeEffdate);
			 // 反向获得分隔符偏移量
			 int anayParam(char *sParam,char sep);//-list参数不能带'='
	public: //hss
			int checkNowValue(char *section, char *param, char *value, char *effdate);
			int CheckDigitString(char *section, char *param, char *value);//sgw
			int CheckDigitString(BaseSysParamRule* pParamRule, char *value);//
			// 不能存在的字符 
			int checkDisobey(char *sSect,char *sParam,char *sVal);
			int checkDisobey(BaseSysParamRule *pParamRule,char *sVal);
	public:	 /* 提供单独功能的函数 */
			void show();
			bool checkCfgParam();
			char sErrInfo[512];
	   		/* 以下函数都是基于这样的原则[不配置规则,或规则没配置这一项,默认是不检查的,算通过] */
			//
			void showErrInfo(int iErrID);
			//
			int check(char *sSect,char *sParam,char *sValue,char *sEffVal,char *OldVal,bool ShowErr,int iFlag=0);		
			//总的检测参数是否规范的调用函数
			int check(char *sSect,char *sParam,char *sValue,char *sEffVal,char *OldVal,int iFlag=0);
			//检测参数是否是必须存在参数[返回true]
			int checkParamNeed(char *sSect,char *sParam);
			//检测参数是否符合最大最小值的配置
			int checkMinMax(char *sSect,char *sParam);		
			//检测参数生效日期是否大于当前值
			int checkEffdate(char *sSect,char *sParam,char *sEffDate,int iFlag=0);
			//检测参数新值是否符合设置
			int checkValueChange(char *sSect,char *sParam,char *sValue,char *sOldVal);
			//检测参数是否是只能增加或者只能减少
			int checkInOrDecrease(const char* str);
			//检测参数新值是否符合枚举值设置[true符合规则]
			int checkEnum(char *sSect,char *sParam,char *sValue);
			//新增GROUP
			bool checkGroupParam(vector<string> str, bool bShow=false);
			bool IsGroupParam(char *section,char *param);
			bool searchGroupParam(vector<string> str, string strcmp,int iNoCmpPos);
			int  getParam(char *sSrcName,char *sDscParamName,char sep);
			
			bool neddArbi(char *sSect,char *sParam);
			int checkTime(char *sSect,char *sParam,char *sValue);
			/* -------参数解析的函数------- */
			//查看这个参数是不是只能增加不能减小的参数
			//[-1是没配置规则或参数有问题0-配置了参数规则,但没对参数做这个限制;1-只能增加;2-不能修改;3-规则匹配;4-只能减小]
			int showInOrDecrease(char* sSect,char *sParam);
			//解析获取段 参数 值,返回值:-1-失败;1-成功
			int getEveryInfo(const char* str,string &sPureSect,string &sPureParam,string &sPureVal);
	public:	 /* 特殊提供的单独功能的函数 */
			//-list 的参数不能带'=';sVal格式是段名.参数名 举例:SYSTEM.location=某个实际值或者为空值
			bool checkListParam(const char *sVal);
			//检测参数新值是否符合值设置
			bool IfNeedLinkParam(char *sSect,char *sParam);
			char *getLinkParam(char *sSect,char *sParam,int iPos);		
			char *getLinkParam(BaseSysParamRule *pParamRule,int iPos);
			int CheckDigitString(char *value);//sgw
    private:
			//  sPtr格式是: 参数数值
			int checkMinMax(BaseSysParamRule* pParamRule,char *sPtr);
			//  sPtr格式是: 参数数值
			int checkValueChange(BaseSysParamRule* pParamRule,char *sPtr,char *sOldVal);
			//  返回值是规则设定值
			int checkParamNeed(BaseSysParamRule* pParamRule);
			//  
			int checkEnum(BaseSysParamRule* pParamRule,char *sPtr);
			//  
			int checkEffdate(BaseSysParamRule *pParamRule,char *sEffDateSrc,int iFlag=0);
			int checkTime(BaseSysParamRule* pParamRule,char *sPtr);
			
			BaseSysParamRule *m_pParamRuleLast;//临时记录上次用的参数规则
			char sSectLast[MAX_SECTION_LEN];//临时记录上次用的参数段
			char sParamLast[MAX_KEY_NAME_LEN];//临时记录上次用的参数名
			//<段.参数,数据指针>
			HashList<BaseSysParamRule*> *m_poIndex;
			//<GROUP段的参数名[无段名],数据指针>
			HashList<BaseVarInfo*> *m_poGroupIndex;
			BaseVarInfo m_oBaseVarInfo[MAX_GROUP_NUM][MAX_GROUP_PARAM_NUM];//最多支持配置2000个组参数
			int m_iCurGroupParam[MAX_GROUP_NUM];//分配到哪个下标了?
			int m_iCurGroup;
			BaseVarInfo *mallocBaseVar(int iGroup);
			BaseVarInfo *MallocBaseVar(char *sGroupName);
			HashList<int> *m_poGroupNoIndex;//组名与组号的对应关系
    private:                       
             /* 文件解析用,参数规则 */
			 CIniFileMgr *gpCIniFileMgr; //文件读取用
			 BaseSysSectRule *gpBaseSysSectRule; //首段地址			 			 			 
			 // 获取参数名
			 int getParamName(char *sSrcName,char *sDscParamName,char sep);
			 
			 //找不到就新new一个结构体返回,直接使用这个结构体就可以了
			 BaseSysParamRule *SearchParamRule(char *sSectName,char *sParamName);
			 // 根据段名malloc出段结构
			 BaseSysSectRule *MallocSectRule(char *sSectName);
			 // malloc出参数结构
			 BaseSysParamRule *MallocParamRule(BaseSysSectRule *pSectRule);
			 
			 void addParamRule(CIniSection *pCIniSection);
			 //bool paramParse(const char* str);
			 int getType(char *sStr,char *sVal,BaseSysParamRule *pSPRule);
			 //
			 int addGroupType(BaseVarInfo *pBaseVarInfo,char *sVal,BaseSysParamRule *pSPRule);
			 //参数规则加载
             bool loadRule();
			 bool m_bLoadRule;
			 bool m_bMonDebug;
			 bool isRuleNumeric(char* pString);
	private: //日期计算
			 bool getCurDate(char *sDate,int iSize);
	protected://临时记录每个参数重要信息
			 //string m_poParamName;
			 //string m_poSectName;
			 //用char[]代替string类型 有的编译有问题
			 char m_sParamName[MAX_KEY_NAME_LEN];
			 char m_sSectName[MAX_SECTION_LEN];
			 BaseSysParamRule *m_poParamRule;
			 
			 void freeALLMalloc();
			 void freeALLParamMalloc(BaseSysSectRule *pSectRule);
};

#endif
