#ifndef SYS_PARAMINFO_H
#define SYS_PARAMINFO_H

#define MAX_P_VAL_LEN  1024
#define MAX_PN_VAL_LEN  512
#define MAX_PARAM_SECT_NUM 1024

struct ParamHisRecord //缓冲
{
	char sSectName[MAX_PN_VAL_LEN];
	char sParamName[MAX_P_VAL_LEN];
	char sParamOrValue[MAX_P_VAL_LEN];
	char sParamValue[MAX_P_VAL_LEN];
	char sDate[16];	
	long  lResult; //只有与修改值返回的修改结果
	char sWholeParamName[MAX_P_VAL_LEN+MAX_PN_VAL_LEN];   // 参数名(带上了段名.参数名的全称)
};

// 显示用结构体参数
typedef struct TSysParamInfo 
{    
    /* 主要参数 */
	char sSrcSectName[MAX_PN_VAL_LEN];	//原始段名[]
	char sSrcParamName[MAX_P_VAL_LEN]; //原始参数名[这两个字段显示不同]
    char sParamName[MAX_P_VAL_LEN+MAX_PN_VAL_LEN];   // 参数名(带上了段名.参数名的全称)
    char sParamOldVal[MAX_P_VAL_LEN]; // 参数原值
    char sParamNewVal[MAX_P_VAL_LEN]; // 参数新值
    char sDate[16];      // 生效时间(格式是yyyy mm dd hh mm ss这样)
	int  iResult; //只有与修改值返回的修改结果
    //char sExpDate[16];      // 失效时间(格式同上)
	//char sNormalValue[512];
    TSysParamInfo *pNext; //这个可能用不上
}TSysParamInfo;

// 模块->参数
typedef struct TSysMoudleHead
{
    char sMoudle[128];  //参数所属模块名
	int iUsedNum;
	int iTotalNum;  // 这个指的是段下的模块有多少个参数
    TSysParamInfo *pSysParamInfo; //这个是一次申请最大数的参数空间
    TSysMoudleHead *pNext; //这个是链接
}TSysMoudleHead;

// 段->模块
typedef struct TSysSectHead
{
    char sSectName[MAX_PARAM_SECT_NUM];       //参数所属段的名称
    int iParamTotalNum;      //参数个数
	int iSectionNum;    //已经使用的段个数(全局用)
	bool Used;    		//此段是否已经使用
	int iMoudleNum;     //模块个数
    int iRefreshFlag;   //这个是标识信息更新的，看来要记录一个刷新记录值
    TSysMoudleHead *pSysMoudleHead;
	TSysSectHead *pNext;
    //TSysParamInfo SysParamInfo;
}TSysInfoHead;


#endif

