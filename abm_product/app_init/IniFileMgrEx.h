#ifndef	_INIT_FILE_MGR_EX_H_
#define	_INIT_FILE_MGR_EX_H_
#include <unistd.h>
#include <iostream>
#include <fstream>
#include "Log.h"
#include <vector>
#include "SHMData.h"
#include "WriteIni.h"
#include <string>
#include <iostream>

using namespace std;
#define		MAX_SECTION_LEN_INIT				64
#define		MAX_KEY_NAME_LEN		512	
#define		MAX_KEY_VALUE_LEN		512	
#define		MAX_REMARK_LEN				256

#define		MAX_INI_KEY_NUM				4096
#define		MAX_KEY_NUM					64
#define		MAX_SECTION_NUM			128
#define		MAX_STACK_LEN					3

#define		MAX_DATE_LEN					16
#define		MAX_TMP_STRING_LEN		MAX_KEY_NAME_LEN+MAX_DATE_LEN


#define   MAX_SECTION_LEN  32
//2.8商用使用的参数保存结构增加生失效时间
#define		MAX_SECTION_NAME_LEN		256
#define		MAX_PARAM_NAME_LEN			256
#define		MAX_INI_VALUE_LEN			128
#define		MAX_DATE_LEN				16
typedef	struct	_SYS_PARAM_INFO_EX_
{
	char	sSectionName[MAX_SECTION_NAME_LEN];
	char	sParamName[MAX_PARAM_NAME_LEN];
	char	sParamValue[MAX_INI_VALUE_LEN];
	char	sEffDate[MAX_DATE_LEN];
	char	sExpDate[MAX_DATE_LEN];
	unsigned	int	uiNext;
}SysParamInfoEx;


#define		EFF_DATE_NAME				"effdate"			//判断参数名字是否是eff_date
#define		NEW_VALUE						"new_value"
#define		OLD_VALUE						"value"


//1,新增.2,修改.3,删除
typedef enum { OP_NEW=1,OP_FIX,OP_DEL} OP_FLAG;


//ini文件结构操作类型
typedef	struct	_SYS_PARAM_INFO_EX_OP_
{
	SysParamInfoEx	stParamInfoEx;
	OP_FLAG			enOp;
}SysParamInfoExOp;

class CIniFileMgrEx
{
public:
	CIniFileMgrEx(long lShmKeyID,unsigned int uiSize);
	~CIniFileMgrEx(void);	

	//从文件中装载Ini配置
	bool LoadIniFromFile(const char* cpsFileName);

	//重新装载Ini文件
	//只是重新调用下LoadIniFromFile函数
	bool ReLoadIniFromFile(void);

	//清理共享内存数据
	void ClearShmData(void);

	//清理共享内存
	void DestoryShm(void);

	//读取所有的Section字段值
	//当使用这个接口和下面的接口返回时不能使用结构体里面的pNext指针查找
	//下一个结构体
	bool GetAllIniKey(vector<SysParamInfoEx>& arraySysParamInfoEx,
		const char* sSectionName =0,
		const char* sParamName =0);

	//读取当前时间使用的Section字段值
	bool GetCurrentIniKey(vector<SysParamInfoEx>& arraySysParamInfoEx,
		const char* sSectionName =0,
		const char* sParamName =0);

	//读取当前Section字段的值
	bool GetCurrentIniKeyValue(const char* sSectionName,
		const char* sParamName,
		string& strValue);

	//读取下一个时间生效的字段
	bool GetNextIniKey(vector<SysParamInfoEx>& arraySysParamInfoEx,
		const char* sSectionName =0,
		const char* sParamName =0,const char* sDate=0);

	//读取下一个Section字段的值
	bool GetNextIniKeyValue(const char* sSectionName,
		const char* sParamName,
		string& strValue,const char* sDate=0);

	//修改参数
	bool ModifyKey(SysParamInfoExOp& stParamInfoOp);

	//将数据同步到文件
	bool SyncToFile(const char* strFileName);

	////链接共享内存
	//bool AttachShm(const char* cpsKey);
	
	//从字符串流中读取
	unsigned int LoadFromStream(istream& inputStream);

private:
	//初始化操作
	bool Init(void);
	bool InitShm(void);
	
	//清理数据
	void Clear(void);
	bool GetIniKey(vector<SysParamInfoEx>& arraySysParamInfoEx,
		const char* sSectionName,const char* sParamName,bool bCurrent,
		const char* sDate);
	//修改结果
	bool AnalysisLine(string& strLine,string& strSection,
			string& strKey,string& strValue);

	bool SaveKey(const char* cpsFileName,SysParamInfoExOp& stParamInfoOp);
	void freeAll(void);
private:
	SHMData<SysParamInfoEx>* m_poParamInfoEx;	//共享内存结构
	unsigned int m_iNum;						//SysParamInfoEx结构个数
	long m_lShmKey;								//共享内存键值
	unsigned int m_uiSize;						//共享内存大小
	string m_strMemLockKey;						//共享内存锁键值
	string m_strFileName;						//ini文件名	
	bool m_bAttached;
};

#endif

