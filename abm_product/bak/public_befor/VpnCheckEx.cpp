/*VER: 1*/ 
#include "VpnCheckEx.h"


VpnCheckEx* VpnCheckEx::m_gpVpnCheckEx =0; 


VpnCheckEx* VpnCheckEx::GetInstance(void)
{
	if(0 == VpnCheckEx::m_gpVpnCheckEx)
	{
		VpnCheckEx::m_gpVpnCheckEx = new VpnCheckEx();
		VpnCheckEx::m_gpVpnCheckEx->bindVpnInfo();
	}
		return VpnCheckEx::m_gpVpnCheckEx;
}

void VpnCheckEx::bindVpnInfo(void)
{
	if(!interface)
		interface = new VpnInfoInterface();
	if (!interface) THROW(MBC_Environment+1);
}
VpnCheckEx::VpnCheckEx(void)
{
	m_poVpnInfoA=0;
	m_poVpnInfoB=0;
	interface=0;
	memset(m_sTmpAccNbrA,0,sizeof(m_sTmpAccNbrA));
	memset(m_sTmpAccNbrB,0,sizeof(m_sTmpAccNbrB));
	memset(m_sTmpStartTime,0,sizeof(m_sTmpStartTime));	
	m_iVpn_type_no=ParamDefineMgr::getLongParam(VPNINFO_SEGMENT,VPN_TYPE_NO);
	m_iVpn_type_local=ParamDefineMgr::getLongParam(VPNINFO_SEGMENT,VPN_TYPE_LOCAL);
	m_iVpn_type_province=ParamDefineMgr::getLongParam(VPNINFO_SEGMENT,VPN_TYPE_PROVINCE);
	m_iVpn_type_country=ParamDefineMgr::getLongParam(VPNINFO_SEGMENT,VPN_TYPE_COUNTRY);
	m_iVpn_type_relation=ParamDefineMgr::getLongParam(VPNINFO_SEGMENT,VPN_TYPE_RELATION);
	m_iVpn_type_friend=ParamDefineMgr::getLongParam(VPNINFO_SEGMENT,VPN_TYPE_FRIEND);  
	m_iVpn_type_local_nomber=ParamDefineMgr::getLongParam(VPNINFO_SEGMENT,VPN_TYPE_LOCAL_NOMBER);
	m_iVpn_type_local_calling_center=ParamDefineMgr::getLongParam(VPNINFO_SEGMENT,VPN_TYPE_LOCAL_CALLING_CENTER);
	m_iVpn_type_local_called_center=ParamDefineMgr::getLongParam(VPNINFO_SEGMENT,VPN_TYPE_LOCAL_CALLED_CENTER);	
}

VpnCheckEx::~VpnCheckEx(void)
{
	if(interface)
		delete interface;
	interface =0;
}

int VpnCheckEx::checkAccNBR(const char* sAccNBR,const char* sStartDate)
{
	int iIvpnType =0;

	memset(m_sTmpAccNbrA,0,sizeof(m_sTmpAccNbrA));
	memset(m_sTmpStartTime,0,sizeof(m_sTmpStartTime));
	sprintf(m_sTmpAccNbrA,"%s",sAccNBR);
	sprintf(m_sTmpStartTime,sStartDate);

	//江苏vpn 主机999 可存在多个vpn 修改 lqf--20080919
#ifdef __VPN_JIANGSHU_
	int iFlag_A=0;
	int iFlag_B=0;
	m_poVpnInfoA = (VpnMemberInfo *)interface->getVpnInfoEx(m_sTmpAccNbrA,m_sTmpStartTime,iFlag_A);	
	if(iFlag_A==1)
	{
		iIvpnType =VPN_ACC_NBR_999;
	}else if( 2 == iFlag_A)
	{
		iIvpnType =VPN_ACC_NBR_VPN;
	}
	
#else
	m_poVpnInfoA = (VpnMemberInfo *)interface->getVpnInfo(m_sTmpAccNbrA,m_sTmpStartTime);
	if(m_poVpnInfoA!=0)
	{
		iIvpnType =VPN_ACC_NBR_VPN;
	}

#endif
	return iIvpnType;
}
bool VpnCheckEx::checkMobile(char * mobile){
		return nbrMgr.ifMobileNbr(mobile);
}

int VpnCheckEx::check(EventSection *poEventSection)
{
	memset(m_sTmpAccNbrA,0,sizeof(m_sTmpAccNbrA));
	memset(m_sTmpAccNbrB,0,sizeof(m_sTmpAccNbrB));
	memset(m_sTmpStartTime,0,sizeof(m_sTmpStartTime));
	if(poEventSection->m_sCallingNBR[0]=='0' || checkMobile(poEventSection->m_sCallingNBR))
		sprintf(m_sTmpAccNbrA,"%s",poEventSection->m_sCallingNBR);
	else{		
		sprintf(m_sTmpAccNbrA,"%s%s",poEventSection->m_sCallingAreaCode,poEventSection->m_sCallingNBR);
	}
	
	if(poEventSection->m_sCalledNBR[0]=='0' || checkMobile(poEventSection->m_sCalledNBR))
		sprintf(m_sTmpAccNbrB,"%s",poEventSection->m_sCalledNBR);
	else
		sprintf(m_sTmpAccNbrB,"%s%s",poEventSection->m_sCalledAreaCode,poEventSection->m_sCalledNBR);
	
	sprintf(m_sTmpStartTime,poEventSection->m_sStartDate);



  	//江苏vpn 主机999 可存在多个vpn 修改 lqf--20080919
	#ifdef __VPN_JIANGSHU_
	int iFlag_A=0;
	int iFlag_B=0;
	int iRole_A=0;
	int iRole_B=0;
	int iFlag=0;
	m_poVpnInfoA = (VpnMemberInfo *)interface->getVpnInfoEx(m_sTmpAccNbrA,m_sTmpStartTime,iFlag_A);	
	m_poVpnInfoB = (VpnMemberInfo *)interface->getVpnInfoEx(m_sTmpAccNbrB,m_sTmpStartTime,iFlag_B);	
				if(iFlag_A==1){
				if(iFlag_B==1){
					/*
					if(iFlag=interface->isSame2AccNbr(m_sTmpAccNbrA,m_sTmpAccNbrB,m_sTmpStartTime,iRole_A,iRole_B)){				
						if(iFlag==1){
							poEventSection->m_aiExtFieldID[0]=999;
						poEventSection->m_aiExtFieldID[1]=999;
							return 	m_iVpn_type_local_calling_center;	
						}
						if(iFlag==2){
							poEventSection->m_aiExtFieldID[0]=iRole_A;
							poEventSection->m_aiExtFieldID[1]=iRole_B;
							return 	m_iVpn_type_province;	
						}
						if(iFlag==3){
							poEventSection->m_aiExtFieldID[0]=iRole_A;
							poEventSection->m_aiExtFieldID[1]=iRole_B;
							return 	m_iVpn_type_country;
						}
					}else{
						return m_iVpn_type_no;
					}		*/
						return m_iVpn_type_no;
				}
				if(iFlag_B==2){
					if(iFlag=interface->isSameCodeAccNbr(m_poVpnInfoB->m_sVpnCode,m_sTmpAccNbrA,m_sTmpStartTime,iRole_A,iRole_B)){							
						if(iFlag==1){
							poEventSection->m_aiExtFieldID[0]=999;
							poEventSection->m_aiExtFieldID[1]=atoi(m_poVpnInfoB->m_sVpnMemberRole);
							return 	m_iVpn_type_local_calling_center;	
						}
						if(iFlag==2){
							poEventSection->m_aiExtFieldID[0]=iRole_B;
							poEventSection->m_aiExtFieldID[1]=iRole_A;
							return 	m_iVpn_type_province;	
						}
						if(iFlag==3){
							poEventSection->m_aiExtFieldID[0]=iRole_B;
							poEventSection->m_aiExtFieldID[1]=iRole_A;
							return 	m_iVpn_type_country;
						}
					}
					else
						return m_iVpn_type_no;
				}
				return m_iVpn_type_no;
			}
			if(iFlag_A==2){
				if(iFlag_B==1){
					if(iFlag=interface->isSameCodeAccNbr(m_poVpnInfoA->m_sVpnCode,m_sTmpAccNbrB,m_sTmpStartTime,iRole_A,iRole_B)){
						if(iFlag=1){
							poEventSection->m_aiExtFieldID[1]=999;
							poEventSection->m_aiExtFieldID[0]=atoi(m_poVpnInfoB->m_sVpnMemberRole);		
							return m_iVpn_type_local_called_center;
						}
						if(iFlag==2){
							poEventSection->m_aiExtFieldID[0]=iRole_A;
							poEventSection->m_aiExtFieldID[1]=iRole_B;
							return 	m_iVpn_type_province;	
						}
						if(iFlag==3){
							poEventSection->m_aiExtFieldID[0]=iRole_A;
							poEventSection->m_aiExtFieldID[1]=iRole_B;
							return 	m_iVpn_type_country;
						}
					}
					else
						return m_iVpn_type_no;
				}
			}	
	#else
		//取主叫号码A的vpn号
	m_poVpnInfoA = (VpnMemberInfo *)interface->getVpnInfo(m_sTmpAccNbrA,m_sTmpStartTime);
	

	//取被叫号码vpn号
	m_poVpnInfoB = (VpnMemberInfo *)interface->getVpnInfo(m_sTmpAccNbrB,m_sTmpStartTime);
	

	#endif
	//没有返回0
	if(m_poVpnInfoA==0){
				if(m_poVpnInfoB==0)
					return m_iVpn_type_no;
			//没有判断是否是网外号码（需要判断上级）
			while(m_poVpnInfoB){
					if(interface->IsVpnFriend(m_poVpnInfoB->m_sVpnCode,m_sTmpAccNbrA,m_sTmpStartTime)){
						return m_iVpn_type_friend;
					}
					m_poVpnInfoB = (VpnMemberInfo *)interface->getFatherVpnInfo(m_poVpnInfoB,m_sTmpStartTime);
			}
			return m_iVpn_type_no;			
	}
	if(m_poVpnInfoB==0){
		//没有判断是否是网外号码（需要判断上级）
		while(m_poVpnInfoA){		
			if(interface->IsVpnFriend(m_poVpnInfoA->m_sVpnCode,m_sTmpAccNbrB,m_sTmpStartTime)){
				return m_iVpn_type_friend;
			}
			m_poVpnInfoA = (VpnMemberInfo *)interface->getFatherVpnInfo(m_poVpnInfoA,m_sTmpStartTime);
		}
		return m_iVpn_type_no;
	}else{
		//判断是否是本地vpn或跨省vpn
		while(m_poVpnInfoA){
				if(strcmp(m_poVpnInfoA->m_sVpnCode,m_poVpnInfoB->m_sVpnCode)==0){
						poEventSection->m_aiExtFieldID[0]=atoi(m_poVpnInfoA->m_sVpnMemberRole);
						poEventSection->m_aiExtFieldID[1]=atoi(m_poVpnInfoB->m_sVpnMemberRole);
						strcpy(poEventSection->m_asExtValue[2],m_poVpnInfoA->m_sVpnCode);
						strcpy(poEventSection->m_asExtValue[3],m_poVpnInfoB->m_sVpnCode);
						if(m_poVpnInfoA->m_sVpnType=='1'){
							#ifdef __VPN_JIANGSHU_
								if (strcmp(m_poVpnInfoA->m_sVpnMemberRole,"234")==0||strcmp(m_poVpnInfoA->m_sVpnMemberRole,"999")==0)
									return m_iVpn_type_local_calling_center;
								if (strcmp(m_poVpnInfoB->m_sVpnMemberRole,"234")==0||strcmp(m_poVpnInfoB->m_sVpnMemberRole,"999")==0)
									return m_iVpn_type_local_called_center;
								return m_iVpn_type_local_nomber;
							#endif
							return m_iVpn_type_local;
						}
						if(m_poVpnInfoA->m_sVpnType=='2'){
							return m_iVpn_type_province;
						}
						if(m_poVpnInfoA->m_sVpnType=='3'){		
							return m_iVpn_type_country;
						}
				}else{
					//判断是否是亲友团
					
					if(interface->isVpnRelation(m_poVpnInfoA->m_sVpnCode,m_poVpnInfoB->m_sVpnCode,m_sTmpStartTime)){
						strcpy(poEventSection->m_asExtValue[2],m_poVpnInfoA->m_sVpnCode);
						strcpy(poEventSection->m_asExtValue[3],m_poVpnInfoB->m_sVpnCode);
						return m_iVpn_type_relation;
					}
				}
			m_poVpnInfoB = (VpnMemberInfo *)interface->getFatherVpnInfo(m_poVpnInfoB,m_sTmpStartTime);
			m_poVpnInfoA = (VpnMemberInfo *)interface->getFatherVpnInfo(m_poVpnInfoA,m_sTmpStartTime);			
		}		
	}
	return m_iVpn_type_no;

}
