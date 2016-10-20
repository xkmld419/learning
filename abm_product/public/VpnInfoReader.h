/*VER: 4*/ 
#ifndef VPNINFOREADER_H_HEADER_INCLUDED_
#define VPNINFOREADER_H_HEADER_INCLUDED_

#include "../../etc/LocalDefine.h"

#include "CommonMacro.h"
#include "VpnInfo.h"
#include "StdEvent.h"
#include "BillingCycle.h"
#include <vector>
#include "BillingCycleMgr.h"

using namespace std;

class BillingCycleMgr;


class VpnInfoReader : public VpnInfoBase
{
  public:
    //构造
    VpnInfoReader();    
};

//## VPN资料读取接口
class VpnInfoInterface : public VpnInfoReader
{
  public:
    //获取是否可用
    operator bool();

	//检查那些静态变量如果没有被初始化，进行初始化
    //VpnInfoInterface();

	//根据vpn_code 查找vpn名称
	static const char * getVpnName(char * sVpnCode,char *sDate);

	/*根据Member_code 查找vpn信息
		iFlag 取值：
		1：sVpnMember 是999 返回VpnMember对象
		2：sVpnMember 不是999，是正常的VPN_NUMBER，返回VpnMember对象
		3：sVpnMember 不是999，是正常的VPN_CODE，返回VpnMember对象
		4: sVpnMember 是网外号码，返回空
		5：sVpnMember 没有信息，返回空
	*/
	static VpnMemberInfo const * getVpnInfoByMember(char * sVpnMember,char *sDate,int &iFlag);

	//根据号码找VPN信息
	static VpnMemberInfo const * getVpnInfo(char * sAccNbr,char *sDate);

	/*根据号码找VPN信息扩展
	  iFlag 取值：
		1：sAccNbr 是999 返回对象是空
		2：sAccNbr 不是是999，是正常的VPN，返回对象是具体的VpnMember
		3：sAccNbr 即不是999也不是正常的VPN，返回对象是空
	*/
	static VpnMemberInfo const * getVpnInfoEx(char * sAccNbr,char *sDate,int &iFlag);

	//根据非999的vpn_code和号码（999类型的号码）返回是否在同一个VPN内
	static int  isSameCodeAccNbr(char * sVpnCode,char * sAccNbr,char *sDate,int &iRoleA,int &iRoleB);
	
	//根据两个号码（都是999类型的）返回是否在一个VPN内
	static int  isSame2AccNbr(char * sAccNbrA,char * sAccNbrB,char *sDate,int &iRoleA,int &iRoleB);

	//根据VPN信息找父节点信息，没有返回 NULL
	static VpnMemberInfo const * getFatherVpnInfo(VpnMemberInfo * oVpnInfo,char *sDate);

	//根据号码查找对应的VPN信息
	static const VpnFriendNumInfo * getVpnFriend(char * sAccNbr,char * sDate);

	//根据VPN编码和号码验证是否存在对应关系
	static bool IsVpnFriend(char * sVpnCode,char * sAccNbr,char * sDate);

	//根据两个VPN编码验证是否存在有关系
	static bool isVpnRelation(char * sVpnCodeA,char * sVpnCodeB,char * sDate);
};

#endif /* VPNINFOREADER_H_HEADER_INCLUDED_ */
