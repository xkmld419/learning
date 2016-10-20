#ifndef SHM_STRUCT_TYPE_INCLUDE_H
#define SHM_STRUCT_TYPE_INCLUDE_H

#define PARAM_STRUCT_NUM		1023//最大支持结构体类型ID数目,类型从1开始递增,这个值为2^n -1
//按次增加结构体类型ID
#define TEMPLATE_MAP_SWITCH_SHMTYPE		1 
#define OBJECT_DEFINE_SHMTYPE		2
#define MAP_PARSER_GROUP_SHMTYPE		3


// 记录唯一的 <m_lSwitchID,m_sTrunkIn>对
struct SwitchIDTrunkInRec
{
	   long m_lSwitchID;
	   char m_sTrunkIn[20];
};

struct strLocalAreaCode
{
	char m_sAreaCode[32];
};

struct CycleTypeID
{
	int  m_iCycleTypeID;
	char m_sCycleBeginDateType[9];
};

class MServAcctInfo
{

 public:
 	
 	//关系标识
	long m_lServAcctID;

    //## 账户标识
    long m_lAcctID;

    //## 主产品标识
    long m_lServID;

    //## 账目组
    int m_iAcctItemGroup;

    //## 状态，分为活动、非活动
    //## 1为活动，0为非活动
    int m_iState;
	//
	int m_iBillingModeID;
	int m_iShareFlag;
	//
    char m_sEffDate[16];

    char m_sExpDate[16];

    //## 同一主产品标识下一个定制关系
    unsigned int m_iNextOffset;
};

/*--std信息定义--*/
//标准8字节数据结构体数据字长
#define STD_CHAR8_LEN	8

// 'STD数据结构体'
struct StdChar8
{
    char sData[STD_CHAR8_LEN];
};

//描述此结构体对应信息的
struct StdChar8DataDesc
{
	long lStructID;//编号
	char sStructName[32];//名称
	int iStructSize;//结构体大小
	unsigned int iMaxNum;//主要record MaxNum[这都是按照自己结构体算出来的值]
	unsigned int iUsedPos;//已经使用到的指向,从0到iMaxNum-1
	int iIndexNum;//索引量
	int iFlashTimes;//扩展信息[]
};

#endif