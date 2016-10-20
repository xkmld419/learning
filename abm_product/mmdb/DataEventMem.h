/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef  _LOCAL_MEM_BASE_
#define  _LOCAL_MEM_BASE_

#include "SHMData.h"
#include "SHMData_A.h"
#include "SHMData_B.h"
#include "SHMIntHashIndex.h"
#include "SHMIntHashIndex_A.h"
#include "SHMStringTreeIndex.h"
#include "SHMStringTreeIndex_A.h"

#include "IpcKey.h"
#include "ParamDefineMgr.h"
#include "CSemaphore.h"
#include "UserInfo.h"

using namespace std;

//用户档案数量定义常量，取自b_param_define
//1、对应于param_segment字段
#define USERINFO_SEGMENT		"USERINFO_UPLOAD"
#define USERINFO_LIMIT			"USERINFO_MEM_LIMIT"
#define E8_DIV_GROUP_SEGMENT	"E8_DIV_PROC_GROUP"
#define E8_USER              "DATA_USER"
#define EXT_SERV              "B_EXT_SERV"


//2。各类共享内存对应的groupid
#define E8_USER_MEM_GRP			0
#define E8_DIV_MEM_GRP_HEAD		1
#define EXT_SERV_MEM_GRP		101

//宽带e8帐号
#define DATA_E8_ACCOUNT_DATA					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_E8_ACCOUNT_DATA")
#define DATA_E8_ACCOUNT_INDEX					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_E8_ACCOUNT_INDEX")


//宽带话单拆分
//第一组
#define DATA_EVENT_DIV_DATA11					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_EVENT_DIV_DATA11")
#define DATA_EVENT_DIV_INDEX11					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_EVENT_DIV_INDEX11")
#define DATA_EVENT_DIV_DATA12					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_EVENT_DIV_DATA12")
#define DATA_EVENT_DIV_INDEX12					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_EVENT_DIV_INDEX12")

//第二组
#define DATA_EVENT_DIV_DATA21					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_EVENT_DIV_DATA21")
#define DATA_EVENT_DIV_INDEX21					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_EVENT_DIV_INDEX21")
#define DATA_EVENT_DIV_DATA22					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_EVENT_DIV_DATA22")
#define DATA_EVENT_DIV_INDEX22					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_EVENT_DIV_INDEX22")

//第三组
#define DATA_EVENT_DIV_DATA31					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_EVENT_DIV_DATA31")
#define DATA_EVENT_DIV_INDEX31					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_EVENT_DIV_INDEX31")
#define DATA_EVENT_DIV_DATA32					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_EVENT_DIV_DATA32")
#define DATA_EVENT_DIV_INDEX32					IpcKeyMgr::getIpcKey (-1,"SHM_DATA_EVENT_DIV_INDEX32")

//### 4008、800等Ext_serv
#define EXT_SERV_DATA							IpcKeyMgr::getIpcKey(-1,"EXT_SERV_DATA")
#define EXT_SERV_INDEX							IpcKeyMgr::getIpcKey(-1,"EXT_SERV_INDEX")


//ADD 20080508 YUANP E8帐号
class DataE8AccountInfo
{
public:
	//e8帐号标识
	long lUserID;
	//e8帐号变更序列
    int  iUserSeq;              
	//e8帐号
    char sUserName[41];
	//帐号归属地区号
    char sAreaCode[10];
	//记录修改时间,20080716为实现按天导出帐号与表中做比对修改，将创建时间改为修改时间
    char sStateDate[15];
	//生效时间
    char sEffDate[15];
	//失效时间
    char sExpDate[15];
    //状态
    char sState[4];    
    //分别指向相同帐号的下一结点，按时间排序
    unsigned int m_iNextOffset; 
};

//add yuanp 20080513 宽带话单拆分合并
class DataEventDiv
{
public:
	long lUserID;
	char sStartDate[15];
	char sEndDate[15];
	long lSepRecID;/*拆分记录标识,基于前面一条话单的*/
	long lResourceID;/*拆分来源　记录和原始话单的联系*/
	int  iSepRecSeq;
	int  iDuration;
	int  iState;		
	unsigned int m_iNextOffset;
};

//add yuanp 20080721 4008/800等信息
class ExtServ
{
public:
	long lExtServID;
	char sAreaCode[7];
	char sExtNbr[MAX_CALLED_NBR_LEN];
	char sExtType[4];
	char sState[4];
	char sEffDate[15];
	char sExpDate[15];	
    unsigned int m_iNextOffset; 
};

class DataEventMemBase 
{
public:
	DataEventMemBase();
	DataEventMemBase(int iGroup);
	~DataEventMemBase();
	void insMonitorInterface(char *sInfo,char *sRcvAlarmAccNbr);
//protected:
	void bindData();
	void getGroupIndex();
	
	static bool m_bAttached;
	int m_iGroupID;
	
    //宽带e8及商务领航数据及索引
	static	USERINFO_SHM_DATA_TYPE<DataE8AccountInfo> * m_poE8AccountData;
	static  DataE8AccountInfo * m_poE8AccountIns;
	static	USERINFO_SHM_STR_INDEX_TYPE * m_poE8AccountIndex;	

	//宽带话单拆分合并
	DataEventDiv *m_poIns1;   //当前组第一片内存的数据指针
	USERINFO_SHM_INT_INDEX_TYPE *m_poIndex1; //当前组第一片内存的索引内存指针
	USERINFO_SHM_DATA_TYPE<DataEventDiv> *m_poData1; //当前组第一片内存的数据内存指针
	
	DataEventDiv *m_poIns2;   //当前组第二片内存的数据指针
	USERINFO_SHM_INT_INDEX_TYPE *m_poIndex2; //当前组第二片内存的索引内存指针
	USERINFO_SHM_DATA_TYPE<DataEventDiv> *m_poData2; //当前组第二片内存的数据内存指针
	
    
	//第一组
	static USERINFO_SHM_DATA_TYPE<DataEventDiv> * m_poDataEventDivData11;
	static DataEventDiv * m_poDataEventDivIns11;
	static USERINFO_SHM_INT_INDEX_TYPE * m_poDataEventDivIndex11;
	static USERINFO_SHM_DATA_TYPE<DataEventDiv> * m_poDataEventDivData12;
	static DataEventDiv * m_poDataEventDivIns12;
	static USERINFO_SHM_INT_INDEX_TYPE * m_poDataEventDivIndex12;

	//第二组
	static USERINFO_SHM_DATA_TYPE<DataEventDiv> * m_poDataEventDivData21;
	static DataEventDiv * m_poDataEventDivIns21;
	static USERINFO_SHM_INT_INDEX_TYPE * m_poDataEventDivIndex21;
	static USERINFO_SHM_DATA_TYPE<DataEventDiv> * m_poDataEventDivData22;
	static DataEventDiv * m_poDataEventDivIns22;
	static USERINFO_SHM_INT_INDEX_TYPE * m_poDataEventDivIndex22;

	//第三组
	static USERINFO_SHM_DATA_TYPE<DataEventDiv> * m_poDataEventDivData31;
	static DataEventDiv * m_poDataEventDivIns31;
	static USERINFO_SHM_INT_INDEX_TYPE * m_poDataEventDivIndex31;
	static USERINFO_SHM_DATA_TYPE<DataEventDiv> * m_poDataEventDivData32;
	static DataEventDiv * m_poDataEventDivIns32;
	static USERINFO_SHM_INT_INDEX_TYPE * m_poDataEventDivIndex32;

	//4008、800等ext_serv信息
	static USERINFO_SHM_DATA_TYPE<ExtServ> * m_poExtServData;
	static ExtServ * m_poExtServIns;
	static USERINFO_SHM_STR_INDEX_TYPE * m_poExtServIndex;

	void freeAll();
};

class DataEventMemCtl:public DataEventMemBase
{
public:
	DataEventMemCtl();	
	DataEventMemCtl(int iGroup);	
	~DataEventMemCtl();	
	
	//DataEventMemCtl();
	bool exist();	
};

#endif
