/*VER: 3*/ 
// Copyright (c) 2008,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef VPNINFO_H_HEADER_INCLUDED_
#define VPNINFO_H_HEADER_INCLUDED_

#include "SHMData.h"
#include "SHMData_A.h"
#include "SHMStringTreeIndex.h"
#include "SHMIntHashIndex.h"
#include "SHMStringTreeIndex_A.h"
#include "SHMIntHashIndex_A.h"
#include "IpcKey.h"

#ifdef  USERINFO_REUSE_MODE
///////打开 USERINFO_REUSE_MODE
    #define USERINFO_SHM_DATA_TYPE  SHMData_A
    #define USERINFO_SHM_INT_INDEX_TYPE  SHMIntHashIndex_A
    #define USERINFO_SHM_STR_INDEX_TYPE  SHMStringTreeIndex_A
	#define USERINFO_ATTACH_DATA(X, Y, Z) \
		X = new SHMData_A<Y> (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}

	#define USERINFO_ATTACH_INT_INDEX(X, Z) \
		X = new SHMIntHashIndex_A (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}

	#define USERINFO_ATTACH_STRING_INDEX(X, Z) \
		X = new SHMStringTreeIndex_A (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}


#else
///////关闭 USERINFO_REUSE_MODE 
    #define USERINFO_SHM_DATA_TYPE  SHMData
    #define USERINFO_SHM_INT_INDEX_TYPE  SHMIntHashIndex
    #define USERINFO_SHM_STR_INDEX_TYPE  SHMStringTreeIndex
	#define USERINFO_ATTACH_DATA(X, Y, Z) \
		X = new SHMData<Y> (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}

	#define USERINFO_ATTACH_INT_INDEX(X, Z) \
		X = new SHMIntHashIndex (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}

	#define USERINFO_ATTACH_STRING_INDEX(X, Z) \
		X = new SHMStringTreeIndex (Z); \
		if (!(X)) { \
			freeAll (); \
			THROW(MBC_UserInfo+1); \
		} \
		if (!(X->exist())) { \
			m_bAttached = false; \
			printf("%s:%d: ATTACE_DATA failed!\n",__FILE__, __LINE__); \
		}
#endif

#define VPN_INFO_DATA                     IpcKeyMgr::getIpcKey (-1,"SHM_VPN_INFO_DATA")
#define VPN_MEMBER_DATA                   IpcKeyMgr::getIpcKey (-1,"SHM_VPN_MEMBER_DATA")
#define VPN_ANDVPN_DATA                   IpcKeyMgr::getIpcKey (-1,"SHM_VPN_ANDVPN_DATA")
#define VPN_FRIENDNUM_DATA                IpcKeyMgr::getIpcKey (-1,"SHM_VPN_FRIENDNUM_DATA")

#define VPN_INFO_INDEX                    IpcKeyMgr::getIpcKey (-1,"SHM_VPN_INFO_INDEX")
#define VPN_MEMBER_INDEX                  IpcKeyMgr::getIpcKey (-1,"SHM_VPN_MEMBER_INDEX")
#define VPN_MEMBER_INDEX2                 IpcKeyMgr::getIpcKey (-1,"SHM_VPN_VPNHAVEMEMBER_INDEX")
#define VPN_MEMBER_999_INDEX              IpcKeyMgr::getIpcKey (-1,"SHM_VPN_VPNHAVEMEMBER999_INDEX")
#define VPN_ANDVPN_INDEX                  IpcKeyMgr::getIpcKey (-1,"SHM_VPN_ANDVPN_INDEX")
#define VPN_FRIENDNUM_INDEX               IpcKeyMgr::getIpcKey (-1,"SHM_VPN_FRIENDNUM_INDEX")
#define VPN_FRIENDNUM_INDEX2              IpcKeyMgr::getIpcKey (-1,"SHM_VPN_VPNHAVEFRIENDNUM_INDEX")



//VPN信息
class VpnInfo
{
public:
    //VPN编号
    char m_sVpnCode[21];

    //VPN名称
    char m_sVpnName[242];

	//VPN类型 1 本地网 2 跨本地网
	char m_sVpnType;

	//生效时间
	char m_sEffDate[16];

	//失效时间
	char m_sExpDate[16];
};

//VPN与成员关系
class VpnMemberInfo
{
public:
	//VPN成员关系seq
	long m_lVpnMemberID;

    //VPN编号
    char m_sVpnCode[21];

    //VPN成员
    char m_sVpnMember[21];

	//VPN成员类型 1、号码 2 VPN
    char m_sVpnType;

	//VPN成员角色
	char m_sVpnMemberRole[10];

	//生效时间
	char m_sEffDate[16];

	//失效时间
	char m_sExpDate[16];

	//VPN编号的下一个成员
    unsigned int m_iVpnNextOffset;

	//VPN成员对应的下一条记录
    unsigned int m_iMemberNextVpnOffset;
};

//VPN与VPN关系
class VpnAndVpnInfo
{
public:
    //VPN编号
    char m_sVpnCodeA[21];

    //VPN编号
    char m_sVpnCodeB[21];

	//生效时间
	char m_sEffDate[16];

	//失效时间
	char m_sExpDate[16];

	//关系的下一条记录
	unsigned int m_iNextOffset;
};

//VPN与外号码关系
class VpnFriendNumInfo
{
public:
    //VPN编号
    char m_sVpnCode[21];

    //VPN成员
    char m_sFriendNum[21];

	//生效时间
	char m_sEffDate[16];

	//失效时间
	char m_sExpDate[16];

	//VPN编号对应的下一个外号码
    unsigned int m_iVpnNextOffset;
	//一个外号码对应的下一条记录
    unsigned int m_iNumNextOffset;
};

class VpnInfoBase
{
  public:
    VpnInfoBase();
    
    operator bool();

  protected:
    
    void bindData();

    //Vpn信息数据区
    static USERINFO_SHM_DATA_TYPE<VpnInfo> *m_poVpnData;

    //Vpn与成员关系数据区
    static USERINFO_SHM_DATA_TYPE<VpnMemberInfo> *m_poVpnMemberData;

    //Vpn与VPn关系数据区
    static USERINFO_SHM_DATA_TYPE<VpnAndVpnInfo> *m_poVpnAndVpnData;

    //Vpn与外号码关系数据区
    static USERINFO_SHM_DATA_TYPE<VpnFriendNumInfo> *m_poVpnFriendNumData;

    //Vpn信息索引区，建立基于VPN编号的索引
    static USERINFO_SHM_STR_INDEX_TYPE *m_poVpnIndex;

    //Vpn信息索引区，建立基于VPN成员的索引
    static USERINFO_SHM_STR_INDEX_TYPE *m_poVpnMemberIndex;
	//Vpn信息索引区，建立基于VPN下各个成员的索引
	static USERINFO_SHM_STR_INDEX_TYPE *m_poVpnMemberIndex2;
	//Vpn信息索引区，建立角色999成员的索引
	static USERINFO_SHM_STR_INDEX_TYPE *m_poVpnMember999Index;

    //Vpn与Vpn关系信息索引区，建立基于两个VPN编号关系的索引
    static USERINFO_SHM_STR_INDEX_TYPE *m_poVpnAndVpnIndex;

    //Vpn与外号码关系信息索引区，建立基于外号码的索引
    static USERINFO_SHM_STR_INDEX_TYPE *m_poVpnFriendNumIndex;
    //Vpn与外号码关系信息索引区，建立VPN下各个外号码的索引
    static USERINFO_SHM_STR_INDEX_TYPE *m_poVpnFriendNumIndex2;

	//绑定变量
    static VpnInfo *m_poVpnInfo;

    static VpnMemberInfo *m_poVpnMemberInfo;

    static VpnAndVpnInfo *m_poVpnAndVpnInfo;

    static VpnFriendNumInfo *m_poVpnFriendNumInfo;

  private:
    //加载共享内存信息
    void loadSHMInfo();
    //释放
    void freeAll();

public:
	static bool m_bAttached;

};

#endif /* VPNINFO_H_HEADER_INCLUDED_ */
