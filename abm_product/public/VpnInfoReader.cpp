/*VER: 6*/ 
#include "VpnInfoReader.h"
#include "Date.h"
#include "StdEvent.h"
#include <vector>
#include "CommonMacro.h"
#include "Log.h"
#include "ParamDefineMgr.h"
#include "Process.h"
#include "IDParser.h"

using namespace std;

const int  ACC_LEN  = 11;

VpnInfoReader::VpnInfoReader() : VpnInfoBase ()
{
}

//根据vpn_code 查找vpn名称
const char * VpnInfoInterface::getVpnName(char * sVpnCode,char *sDate)
{
	unsigned int i;
	if(NULL==sVpnCode) return NULL;
	VpnInfo * pVpnInfo;
	if(m_poVpnIndex->get(sVpnCode,&i))
	{
		pVpnInfo = m_poVpnInfo + i;
		if(strcmp(sDate,pVpnInfo->m_sEffDate)>=0 
			&& strcmp(sDate,pVpnInfo->m_sExpDate)<0)
		{
			return pVpnInfo->m_sVpnName;
		}
	}
	return NULL;
}

VpnMemberInfo const * VpnInfoInterface::getVpnInfoByMember(char * sAccNbr,char *sDate,int &iFlag)
{
	unsigned int i;
	VpnMemberInfo  * oTemp;
	if(!sAccNbr) return 0;
	if(m_poVpnMemberIndex->get(sAccNbr,&i)){
		while(i){
			oTemp = m_poVpnMemberInfo + i;
			if(strcmp(sDate,oTemp->m_sEffDate)>=0 
				&& strcmp(sDate,oTemp->m_sExpDate)<0)
			{
				if(oTemp->m_sVpnType == '1'){
					iFlag = 2;
				}else{
					iFlag = 3;
				}
				return oTemp;
			}else{
				i = oTemp->m_iMemberNextVpnOffset;
			}
		}
	}
	if(m_poVpnMember999Index->getMax(sAccNbr,&i)){
		while(i){
			oTemp = m_poVpnMemberInfo + i;
			if(oTemp->m_sVpnType == '1'
				&& strcmp(sDate,oTemp->m_sEffDate)>=0 
				&& strcmp(sDate,oTemp->m_sExpDate)<0){
					iFlag = 1;
				return oTemp;
			}else{
				i = oTemp->m_iMemberNextVpnOffset;
			}
		}
	}

	if(m_poVpnFriendNumIndex->get(sAccNbr,&i)){
		iFlag = 4;
		return NULL;
	}
	iFlag = 5;
	return NULL;
}

//根据号码找VPN信息
VpnMemberInfo const * VpnInfoInterface::getVpnInfo(char * sAccNbr,char *sDate)
{
	unsigned int i;
	VpnMemberInfo  * oTemp;
	if(!sAccNbr) return 0;
	if(m_poVpnMemberIndex->get(sAccNbr,&i)){
		while(i){
			oTemp = m_poVpnMemberInfo + i;
			if(oTemp->m_sVpnType == '1'
				&& strcmp(sDate,oTemp->m_sEffDate)>=0 
				&& strcmp(sDate,oTemp->m_sExpDate)<0){
						return oTemp;
				}else{
				i = oTemp->m_iMemberNextVpnOffset;
			}
		}
	}
	if(m_poVpnMember999Index->getMax(sAccNbr,&i)){
		while(i){
			oTemp = m_poVpnMemberInfo + i;
			if(oTemp->m_sVpnType == '1'
				&& strcmp(sDate,oTemp->m_sEffDate)>=0 
				&& strcmp(sDate,oTemp->m_sExpDate)<0){
						return oTemp;
				}else{
				i = oTemp->m_iMemberNextVpnOffset;
			}
		}
	}
	return NULL;
}

VpnMemberInfo const * VpnInfoInterface::getVpnInfoEx(char * sAccNbr,char *sDate,int &iFlag)
{
	unsigned int i;
	VpnMemberInfo  * oTemp;
	if(!sAccNbr) return 0;
	if(m_poVpnMemberIndex->get(sAccNbr,&i)){
		while(i){
			oTemp = m_poVpnMemberInfo + i;
			if(oTemp->m_sVpnType == '1'
				&& strcmp(sDate,oTemp->m_sEffDate)>=0 
				&& strcmp(sDate,oTemp->m_sExpDate)<0){
				iFlag = 2;
				return oTemp;
			}else{
				i = oTemp->m_iMemberNextVpnOffset;
			}
		}
	}
	if(m_poVpnMember999Index->getMax(sAccNbr,&i)){
		while(i){
			oTemp = m_poVpnMemberInfo + i;
			if(oTemp->m_sVpnType == '1'
				&& strcmp(sDate,oTemp->m_sEffDate)>=0 
				&& strcmp(sDate,oTemp->m_sExpDate)<0){
					iFlag = 1;
				return NULL;
			}else{
				i = oTemp->m_iMemberNextVpnOffset;
			}
		}
	}
	iFlag = 3;
	return NULL;
}

/*
对于sAccNbr超过12位后，getMax只能处理12位，会一直返回同一个号码导致while中的死循环，如果号码长度大于11位时
需要按号码长度来每次缩一位可以解决死循环
*/
int  VpnInfoInterface::isSameCodeAccNbr(char * sVpnCode,char * sAccNbr,char *sDate,int &iRoleA,int &iRoleB)
{
	int iRet = 0;
	unsigned int i,iLen;
	vector<char *> v;
	VpnMemberInfo  * oTempA;
	VpnMemberInfo  * oTempB;
	char sTempAccNbr[21];
	if(!sVpnCode ||!sAccNbr) return false;
	int iAccLen = strlen(sAccNbr);
	int iTemplen = iAccLen;
	strcpy(sTempAccNbr,sAccNbr);
	while(m_poVpnMember999Index->getMax(sTempAccNbr,&i))
	{//1
		while(i)
		{//2
			oTempB = m_poVpnMemberInfo + i;
			if(oTempB->m_sVpnType == '1'
				&& strcmp(sDate,oTempB->m_sEffDate)>=0 
				&& strcmp(sDate,oTempB->m_sExpDate)<0)
			{//3
				if(strcmp(sVpnCode,oTempB->m_sVpnCode)==0)
				{//4
					iRet = oTempB->m_sVpnType - '0';
					iRoleB = atoi(oTempB->m_sVpnMemberRole);
					return iRet;
				}//4
				v.push_back(oTempB->m_sVpnCode);
			}//3
			i = oTempB->m_iMemberNextVpnOffset;
		}//2
		if(iAccLen>ACC_LEN)
		{
			strncpy(sTempAccNbr,sAccNbr,--iTemplen);
			if(!iTemplen) break;
			sTempAccNbr[iTemplen]='\0';
		}
		else
		{
			iLen = strlen(oTempB->m_sVpnMember)-1;
			if(!iLen) break;
			strncpy(sTempAccNbr,oTempB->m_sVpnMember,iLen);
			sTempAccNbr[iLen]='\0';
		}
	}//1
	for (int m=0;m<v.size();m++)
	{//1
		if(m_poVpnMemberIndex->get((char *)(v[m]),&i))
		{//2
			while(i)
			{//3
				oTempB = m_poVpnMemberInfo + i;
				if(oTempB->m_sVpnType == '2'
					&& strcmp(sDate,oTempB->m_sEffDate)>=0 
					&& strcmp(sDate,oTempB->m_sExpDate)<0)
				{//4
					if(m_poVpnMemberIndex->get(sVpnCode,&i))
					{//5
						while(i)
						{//6
							oTempA = m_poVpnMemberInfo + i;
							if(oTempA->m_sVpnType == '2'
							&& strcmp(sDate,oTempA->m_sEffDate)>=0 
							&& strcmp(sDate,oTempA->m_sExpDate)<0)
							{//7
								if(strcmp(oTempB->m_sVpnCode,oTempA->m_sVpnCode)==0)
								{//8
									iRoleA = atoi(oTempA->m_sVpnMemberRole);
									iRoleB = atoi(oTempB->m_sVpnMemberRole);
									iRet = oTempA->m_sVpnType - '0';
									return iRet;
								}//8
							}//7
							i = oTempA->m_iMemberNextVpnOffset;
						}//6
					}
					else
					{//5
						return iRet;
					}//5
				}//4
				i = oTempB->m_iMemberNextVpnOffset;
			}//3
		}//2
	}//1
	return iRet;
}

int VpnInfoInterface::isSame2AccNbr(char * sAccNbrA,char * sAccNbrB,char *sDate,int &iRoleA,int &iRoleB)
{
	int iRet = 0;
	unsigned int iA,iB,i,iBTemp;
	unsigned int iLen;
	char sTempAccNbr[21];
	vector<int> viA;
	vector<int> viB;
	bool bNext = false;
	VpnMemberInfo  * oTempA;
	VpnMemberInfo  * oTempB;
	vector<char *> vA;
	vector<char *> vB;
	if(!sAccNbrA ||!sAccNbrB) return false;
	int iAccLenA = strlen(sAccNbrA);
	int iAccLenB = strlen(sAccNbrB);
	int iTemplenA = iAccLenA;
	int iTemplenB = iAccLenB;
	//查找A和B号码存在的小群的首地址，不验证时间
	if(m_poVpnMember999Index->getMax(sAccNbrA,&iA) 
		&& m_poVpnMember999Index->getMax(sAccNbrB,&iB))
	{//1
			viA.push_back(iA);
			viB.push_back(iB);
			oTempA = m_poVpnMemberInfo + iA;
			iLen = strlen(oTempA->m_sVpnMember) -1;
			if(iLen)
			{//2
				if(iAccLenA>ACC_LEN)
				{
					strncpy(sTempAccNbr,sAccNbrA,--iTemplenA);
					sTempAccNbr[iTemplenA]='\0';
				}
				else
				{
					strncpy(sTempAccNbr,oTempA->m_sVpnMember,iLen);
					sTempAccNbr[iLen]='\0';
				}
				if(iTemplenA>0)
				{//3
					while(m_poVpnMember999Index->getMax(sTempAccNbr,&iA))
					{//4
						viA.push_back(iA);
						if(iAccLenA>ACC_LEN)
						{
							strncpy(sTempAccNbr,sAccNbrA,--iTemplenA);
							if(!iTemplenA) break;
							sTempAccNbr[iTemplenA]='\0';
						}
						else
						{
							oTempA = m_poVpnMemberInfo + iA;
							iLen = strlen(oTempA->m_sVpnMember) -1;
							if(!iLen) break;
							strncpy(sTempAccNbr,oTempA->m_sVpnMember,iLen);
							sTempAccNbr[iLen]='\0';
						}
					}//4
				}//3
			}//2
			oTempB = m_poVpnMemberInfo + iB;
			iLen = strlen(oTempB->m_sVpnMember) -1;
			if(iLen)
			{//2
				if(iAccLenB>ACC_LEN){
					strncpy(sTempAccNbr,sAccNbrB,--iTemplenB);
					sTempAccNbr[iTemplenB]='\0';
				}
				else
				{
					strncpy(sTempAccNbr,oTempB->m_sVpnMember,iLen);
					sTempAccNbr[iLen]='\0';
				}
				if(iTemplenB>0)
				{//3
					while(m_poVpnMember999Index->getMax(sTempAccNbr,&iB))
					{//4
						viB.push_back(iB);
						if(iAccLenB>ACC_LEN){
							strncpy(sTempAccNbr,sAccNbrB,--iTemplenB);
							if(!iTemplenB) break;
							sTempAccNbr[iTemplenB]='\0';
						}
						else
						{
							oTempB = m_poVpnMemberInfo + iB;
							iLen = strlen(oTempB->m_sVpnMember) -1;
							if(!iLen) break;
							strncpy(sTempAccNbr,oTempB->m_sVpnMember,iLen);
							sTempAccNbr[iLen]='\0';
						}
					}//4
				}//3
			}//2
	}//1
	else
	{
		return iRet;
	}
	//查找小群关系
	for(int m=0;m<viA.size();m++)
	{//1
		iA = (int)viA[m];
		while(iA)
		{//2
			oTempA = m_poVpnMemberInfo + iA;
			if(oTempA->m_sVpnType == '1'
				&& strcmp(sDate,oTempA->m_sEffDate)>=0
				&& strcmp(sDate,oTempA->m_sExpDate)<0)
			{//3
				for(int n=0;n<viB.size();n++)
				{//4
					iB = (int)viB[n];
					while(iB)
					{//5
						oTempB = m_poVpnMemberInfo + iB;
						if(oTempB->m_sVpnType == '1'
							&& strcmp(sDate,oTempB->m_sEffDate)>=0 
							&& strcmp(sDate,oTempB->m_sExpDate)<0)
						{//6
							if(strcmp(oTempA->m_sVpnCode,oTempB->m_sVpnCode)== 0)
							{//7
								iRoleA = atoi(oTempA->m_sVpnMemberRole);
								iRoleB =  atoi(oTempB->m_sVpnMemberRole);
								iRet = oTempB->m_sVpnType - '0';
								return iRet;
							}//7
							if(!bNext){//7
								vB.push_back(oTempB->m_sVpnCode);
							}//7
						}//6
						iB = oTempB->m_iMemberNextVpnOffset;
					}//5
				}//4
				bNext = true;
			}//3
			vA.push_back(oTempA->m_sVpnCode);
			iA = oTempA->m_iMemberNextVpnOffset;
		}//2
	}//1
	//查找大群关系
	if(vA.size() && vB.size())
	{//1
		for (int m=0;m<vA.size();m++)
		{//2
			if(m_poVpnMemberIndex->get((char *)(vA[m]),&i))
			{//3
				while(i)
				{//4
					oTempA = m_poVpnMemberInfo + i;
					if(oTempA->m_sVpnType == '2'
						&& strcmp(sDate,oTempA->m_sEffDate)>=0 
						&& strcmp(sDate,oTempA->m_sExpDate)<0)
					{//5
						for (int n=0;n<vB.size();n++)
						{//6
							if(m_poVpnMemberIndex->get((char *)(vB[n]),&i))
							{//7
								while(i)
								{//8
									oTempB = m_poVpnMemberInfo + i;
									if(oTempB->m_sVpnType == '2'
										&& strcmp(sDate,oTempB->m_sEffDate)>=0 
										&& strcmp(sDate,oTempB->m_sExpDate)<0)
									{//9
										if(strcmp(oTempA->m_sVpnCode,oTempB->m_sVpnCode)== 0)
										{//10
											iRoleA = atoi(oTempA->m_sVpnMemberRole);
											iRoleB =  atoi(oTempB->m_sVpnMemberRole);
											iRet = oTempB->m_sVpnType - '0';
											return iRet;
										}//10
									}//9
									i = oTempB->m_iMemberNextVpnOffset;
								}//8
							}//7
						}//6
					}//5
					i = oTempA->m_iMemberNextVpnOffset;
				}//4
			}//3
		}//2
	}//1
	return iRet;	
}

//根据VPN信息找父节点信息，没有返回 NULL
VpnMemberInfo const * VpnInfoInterface::getFatherVpnInfo(VpnMemberInfo * oVpnInfo,char *sDate)
{
	unsigned int i;
	VpnMemberInfo  * oTemp;
	if(!oVpnInfo) return 0;
	if(m_poVpnMemberIndex->get(oVpnInfo->m_sVpnCode,&i)){
		while(i){
			oTemp = m_poVpnMemberInfo + i;
			if(oTemp->m_sVpnType == '2'
				&& strcmp(sDate,oTemp->m_sEffDate)>=0 
				&& strcmp(sDate,oTemp->m_sExpDate)<0){
					return oTemp;
				}else{
				i = oTemp->m_iMemberNextVpnOffset;
			}
		}
	}
	return NULL;
}

//根据VPN编码和外网号码验证是否存在对应关系
bool VpnInfoInterface::IsVpnFriend(char * sVpnCode,char * sAccNbr,char * sDate)
{
	unsigned int i;
	bool bRet = false;
	VpnFriendNumInfo * pTemp;
	if(!sVpnCode) return false;
	if(!sAccNbr) return false;
	if(m_poVpnFriendNumIndex->get(sAccNbr,&i)){
		while(i){
			pTemp = m_poVpnFriendNumInfo + i;
			if( strcmp(sVpnCode,pTemp->m_sVpnCode)==0
				&& strcmp(sDate,pTemp->m_sEffDate)>=0 
				&& strcmp(sDate,pTemp->m_sExpDate)<0){
						bRet = true;
						break;
			}else{
				i = pTemp->m_iNumNextOffset;
			}
		}
	}
	return bRet;
}

//根据号码查找对应的VPN信息
VpnFriendNumInfo const * VpnInfoInterface::getVpnFriend(char * sAccNbr,char * sDate)
{
	unsigned int i;
	VpnFriendNumInfo * pTemp;
	if(!sAccNbr) return NULL;
	if(m_poVpnFriendNumIndex->get(sAccNbr,&i)){
		while(i){
			pTemp = m_poVpnFriendNumInfo + i;
			if(strcmp(sDate,pTemp->m_sEffDate)>=0 
				&& strcmp(sDate,pTemp->m_sExpDate)<0)
			{
				return pTemp;
			}else{
				i = pTemp->m_iNumNextOffset;
			}
		}
	}
	return NULL;
}

//根据两个VPN编码验证是否存在有关系
bool VpnInfoInterface::isVpnRelation(char * sVpnCodeA,char * sVpnCodeB,char * sDate)
{
	bool bRet = false;
	char sTemp[42];
	unsigned int i;
	VpnAndVpnInfo * pTemp;
	strcpy(sTemp,sVpnCodeA);
	strcat(sTemp,sVpnCodeB);
	if(m_poVpnAndVpnIndex->get(sTemp,&i)){
		while(i){
			pTemp = m_poVpnAndVpnInfo + i;
			if(strcmp(sDate,pTemp->m_sEffDate)>=0 
				&& strcmp(sDate,pTemp->m_sExpDate)<0){
						bRet = true;
						break;
			}else{
				i = pTemp->m_iNextOffset;
			}
		}
	}
	if(bRet) return bRet;
	strcpy(sTemp,sVpnCodeB);
	strcat(sTemp,sVpnCodeA);
	if(m_poVpnAndVpnIndex->get(sTemp,&i)){
		while(i){
			pTemp = m_poVpnAndVpnInfo + i;
			if(strcmp(sDate,pTemp->m_sEffDate)>=0 
				&& strcmp(sDate,pTemp->m_sExpDate)<0){
						bRet = true;
						break;
			}else{
				i = pTemp->m_iNextOffset;
			}
		}
	}
	return bRet;
}

