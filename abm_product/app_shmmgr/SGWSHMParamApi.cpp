#include "SGWSHMParamApi.h"
#include "MBC.h"
#include "MBC_ABM.h"
#include <iostream>
using namespace std;

//#define _DF_LOCK_

//转换session_id
char *changeSessionID(const char *_sSessindID)
{
	if (strlen(_sSessindID) == 0 ){
		return NULL;
	}
	static char sKey[12] = {0};
	memset(sKey,0,12);
	const char *p1 = strstr(_sSessindID,";");
	if (p1 == NULL ) {
		return NULL;
	}
	const char *q1 =strstr(p1+1,";");
	if (q1 == NULL) {
		return NULL;
	}
	int _strLen1 = strlen(p1+1)-strlen(q1);
	int _len1 = (11>_strLen1)?_strLen1:11;
	snprintf(sKey,_len1,"%s",p1+1);
	return sKey;
}

//截取会话ID的字段
char *changeSessionID(const char *_sSessindID,char *sKey)
{
	if (strlen(_sSessindID) == 0 ){
		return NULL;
	}
	memset(sKey,0,12);
	const char *p1 = strstr(_sSessindID,";");
	if (p1 == NULL ) {
		return NULL;
	}
	const char *q1 =strstr(p1+1,";");
	if (q1 == NULL) {
		return NULL;
	}
	int _strLen1 = strlen(p1+1)-strlen(q1);
	int _len1 = (11>_strLen1)?_strLen1:11;
	//snprintf(sKey,_len1,"%s",p1);
	strncpy(sKey,p1+1,_len1);
	return sKey;
}

//查询接口
int SHMContentFiltering::GetContextFilterInfo(int _iFilterSeq, vector<ContextData> &vsContextInfo)
{
	vsContextInfo.clear();
	if (m_pSHMMgrContextData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrContextData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	vsContextInfo.clear();
	int pos =  1;

#ifdef _PV_STATE_
	m_pSHMMgrContextDataLock->P();

	for (int i =1; i<=m_pSHMMgrContextData->getCount();) {

		if (m_pSHMMgrContextData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrContextData->getTotal()) {
				break;
			}
			continue;
		}

		if (m_pMgrContextData[pos].m_iFilterSeq==_iFilterSeq ) { //表示节点已经使用
			vsContextInfo.push_back(m_pMgrContextData[pos]);
		}
		pos++;
		i++;
	}

	m_pSHMMgrContextDataLock->V();

	if (vsContextInfo.size() == 0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else

	for (int i =1; i<=m_pSHMMgrContextData->getCount();) {

		if (m_pSHMMgrContextData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrContextData->getTotal()) {
				break;
			}
			continue;
		}

		if (m_pMgrContextData[pos].m_iFilterSeq==_iFilterSeq ) { //表示节点已经使用
			vsContextInfo.push_back(m_pMgrContextData[pos]);
		}
		pos++;
		i++;
	}

	if (vsContextInfo.size() == 0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

//查询接口
int SHMContentFiltering::GetContextFilterInfoAll(vector<ContextData> &vsContextInfo)
{
	vsContextInfo.clear();
	if (m_pSHMMgrContextData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrContextData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	vsContextInfo.clear();
#ifdef _PV_STATE_
	m_pSHMMgrContextDataLock->P();
	int pos = 1;

	for (int i =1; i<=m_pSHMMgrContextData->getCount();) {

		if (m_pSHMMgrContextData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrContextData->getTotal()) {
				break;
			}
			continue;
		}
		vsContextInfo.push_back(m_pMgrContextData[pos]);
		pos++;
		i++;
	}
	m_pSHMMgrContextDataLock->V();

	if (vsContextInfo.size() == 0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else
	int pos = 1;

	for (int i =1; i<=m_pSHMMgrContextData->getCount();) {

		if (m_pSHMMgrContextData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrContextData->getTotal()) {
				break;
			}
			continue;
		}
		vsContextInfo.push_back(m_pMgrContextData[pos]);
		pos++;
		i++;
	}

	if (vsContextInfo.size() == 0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

int SHMContentFiltering::AddContextFilterInfo(const ContextData & osContextInfo)
{
	if (m_pSHMMgrContextData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrContextData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	int pos = 1;

	for (int i =1; i<=m_pSHMMgrContextData->getCount();) {

		if (m_pSHMMgrContextData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrContextData->getTotal()) {
				break;
			}
			continue;
		}

		if (m_pMgrContextData[pos].m_iFilterSeq==osContextInfo.m_iFilterSeq ) { //表示节点已经使用
			return MBC_SHM_ADD_DATA_EXIST;
		}
		pos++;
		i++;
	}
#ifdef _DF_LOCK_
	m_pSHMMgrContextDataLock->P();

	int i = m_pSHMMgrContextData->malloc();

	if (i == 0) {
		m_pSHMMgrContextDataLock->V();
		return MBC_MEM_SPACE_SMALL;
	}
	memcpy(&m_pMgrContextData[i],&osContextInfo,sizeof(ContextData));
	m_pSHMMgrContextDataLock->V();
	return 0;
#else
	int i = m_pSHMMgrContextData->malloc();

	if (i == 0) {
		return MBC_MEM_SPACE_SMALL;
	}
	memcpy(&m_pMgrContextData[i],&osContextInfo,sizeof(ContextData));
	return 0;
#endif
}

int SHMContentFiltering::DelContextFilterInfo(const ContextData & osContextInfo)
{
	if (m_pSHMMgrContextData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrContextData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	int pos = 1;
#ifdef _DF_LOCK_
	m_pSHMMgrContextDataLock->P();

	for (int i =1; i <=m_pSHMMgrContextData->getCount();){

		if (m_pSHMMgrContextData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrContextData->getTotal()) {
				break;
			}
			continue;
		}

		if (m_pMgrContextData[pos].m_iFilterSeq==osContextInfo.m_iFilterSeq ) { //表示节点已经使用
			m_pSHMMgrContextData->erase(pos);
			memset(&m_pMgrContextData[pos],0,sizeof(ContextData));
		}
		pos++;
		i++;
	}
	m_pSHMMgrContextDataLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else
	for (int i =1; i <=m_pSHMMgrContextData->getCount();){

		if (m_pSHMMgrContextData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrContextData->getTotal()) {
				break;
			}
			continue;
		}

		if (m_pMgrContextData[pos].m_iFilterSeq==osContextInfo.m_iFilterSeq ) { //表示节点已经使用
			m_pSHMMgrContextData->erase(pos);
			memset(&m_pMgrContextData[pos],0,sizeof(ContextData));
		}
		pos++;
		i++;
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif
}

int SHMContentFiltering::UpdateContextFilterInfo(const ContextData & osContextInfo)
{
	if (m_pSHMMgrContextData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrContextData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
#ifdef _DF_LOCK_

	m_pSHMMgrContextDataLock->P();
	int pos = 1;

	for (int i =1; i <=m_pSHMMgrContextData->getCount();){
		if (m_pSHMMgrContextData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrContextData->getTotal()) {
				break;
			}
			continue;
		}
		if (m_pMgrContextData[pos].m_iFilterSeq == osContextInfo.m_iFilterSeq) {
			memcpy(&m_pMgrContextData[pos],&osContextInfo,sizeof(ContextData));
			m_pSHMMgrContextDataLock->V();
			return 0;
		}
		pos++;
		i++;
	}
	m_pSHMMgrContextDataLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else
	int pos = 1;

	for (int i =1; i <=m_pSHMMgrContextData->getCount();){
		if (m_pSHMMgrContextData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrContextData->getTotal()) {
				break;
			}
			continue;
		}
		if (m_pMgrContextData[pos].m_iFilterSeq == osContextInfo.m_iFilterSeq) {
			memcpy(&m_pMgrContextData[pos],&osContextInfo,sizeof(ContextData));
			return 0;
		}
		pos++;
		i++;
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif
}

int SHMMsgMapInfo::GetMsgMapInfo(int _iID ,SMsgMapInfo & vsMsgMapInfo)
{
	unsigned int value = 0;
	if (m_pSHMMgrMsgMapInfoIndex->get(_iID,&value)) {
		memcpy(&vsMsgMapInfo,&m_pMgrMsgMapInfoData[value],sizeof(SMsgMapInfo));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMMsgMapInfo::AddMsgMapInfo(SMsgMapInfo & vsMsgMapInfo)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrMsgMapInfoLock->P();

	if (m_pSHMMgrMsgMapInfoIndex->get(vsMsgMapInfo.m_iID,&value)) {
		m_pSHMMgrMsgMapInfoLock->V();
		return MBC_SHM_ADD_DATA_EXIST;
	} else {
		int i = m_pSHMMgrMsgMapInfoData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息tab_msg_map共享内存空间不足");
			m_pSHMMgrMsgMapInfoLock->V();
			return MBC_MEM_SPACE_SMALL;
		} else {
			memcpy(&m_pMgrMsgMapInfoData[i],&vsMsgMapInfo,sizeof(SMsgMapInfo));
			m_pSHMMgrMsgMapInfoIndex->add(vsMsgMapInfo.m_iID,i);
			m_pSHMMgrMsgMapInfoLock->V();
			return 0;
		}
	}
#else

	if (m_pSHMMgrMsgMapInfoIndex->get(vsMsgMapInfo.m_iID,&value)) {
		return MBC_SHM_ADD_DATA_EXIST;
	} else {
		int i = m_pSHMMgrMsgMapInfoData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息tab_msg_map共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		} else {
			memcpy(&m_pMgrMsgMapInfoData[i],&vsMsgMapInfo,sizeof(SMsgMapInfo));
			m_pSHMMgrMsgMapInfoIndex->add(vsMsgMapInfo.m_iID,i);
			return 0;
		}
	}
#endif
}

int SHMMsgMapInfo::DelMsgMapInfo(SMsgMapInfo & vsMsgMapInfo)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrMsgMapInfoLock->P();

	if (!m_pSHMMgrMsgMapInfoIndex->get(vsMsgMapInfo.m_iID,&value)) {
		m_pSHMMgrMsgMapInfoLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		m_pSHMMgrMsgMapInfoIndex->revokeIdx(vsMsgMapInfo.m_iID,value);
		m_pSHMMgrMsgMapInfoData->erase(value);
		memset(&m_pMgrMsgMapInfoData[value],0,sizeof(SMsgMapInfo));
		m_pSHMMgrMsgMapInfoLock->V();
		return 0;
	}
#else
	if (!m_pSHMMgrMsgMapInfoIndex->get(vsMsgMapInfo.m_iID,&value)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		m_pSHMMgrMsgMapInfoIndex->revokeIdx(vsMsgMapInfo.m_iID,value);
		m_pSHMMgrMsgMapInfoData->erase(value);
		memset(&m_pMgrMsgMapInfoData[value],0,sizeof(SMsgMapInfo));
		return 0;
	}
#endif
}

int SHMMsgMapInfo::UpdateMsgMapInfo(SMsgMapInfo & vsMsgMapInfo)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrMsgMapInfoLock->P();

	if (!m_pSHMMgrMsgMapInfoIndex->get(vsMsgMapInfo.m_iID,&value)) {
		m_pSHMMgrMsgMapInfoLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		memcpy(&m_pMgrMsgMapInfoData[value],&vsMsgMapInfo,sizeof(SMsgMapInfo));
		m_pSHMMgrMsgMapInfoLock->V();
		return 0;
	}
#else
	if (!m_pSHMMgrMsgMapInfoIndex->get(vsMsgMapInfo.m_iID,&value)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		memcpy(&m_pMgrMsgMapInfoData[value],&vsMsgMapInfo,sizeof(SMsgMapInfo));
		return 0;
	}
#endif
}

int SHMMsgMapInfo::GetMsgMapInfo(vector<SMsgMapInfo> &vsMsgMapInfo)
{
	if (m_pSHMMgrMsgMapInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrMsgMapInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	vsMsgMapInfo.clear();
#ifdef _PV_STATE_
	m_pSHMMgrMsgMapInfoLock->P();

	for (int i =1; i<=m_pSHMMgrMsgMapInfoData->getCount();i++){
		vsMsgMapInfo.push_back(m_pMgrMsgMapInfoData[i]);
	}
	m_pSHMMgrMsgMapInfoLock->V();
	if (vsMsgMapInfo.size() == 0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else
	for (int i =1; i<=m_pSHMMgrMsgMapInfoData->getCount();i++){
		vsMsgMapInfo.push_back(m_pMgrMsgMapInfoData[i]);
	}
	if (vsMsgMapInfo.size() == 0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

int SHMGlobalMap::GetGlobalMap(int _iGID, SgwGlobalMap &sGlobalMap)
{
	unsigned int value = 0;

	if(!m_pSHMMgrGlobalMapindex->get(_iGID,&value)) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		memcpy(&sGlobalMap,&m_pMgrGlobalMapData[value],sizeof(SgwGlobalMap));
		return 0;
	}
	return 0;
}

int SHMGlobalMap::AddGlobalMap(SgwGlobalMap &sGlobalMap)
{
	unsigned int value = 0;

#ifndef _DF_LOCK_
	m_pSHMMgrGlobalMapLock->P();

	if(!m_pSHMMgrGlobalMapindex->get(sGlobalMap.m_iGID,&value)) {
		int i = m_pSHMMgrGlobalMapData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息sgw_global_map共享内存空间不足");
			m_pSHMMgrGlobalMapLock->V();
			return MBC_MEM_SPACE_SMALL;
		} else {
			memcpy(&m_pMgrGlobalMapData[i],&sGlobalMap,sizeof(SgwGlobalMap));
			m_pSHMMgrGlobalMapindex->add(sGlobalMap.m_iGID,i);
			m_pSHMMgrGlobalMapLock->V();
			return 0;
		}
	} else {
		m_pSHMMgrGlobalMapLock->V();
		return MBC_SHM_ERRNO_ADD_DATA_EXIST;
	}
#else

	if(!m_pSHMMgrGlobalMapindex->get(sGlobalMap.m_iGID,&value)) {
		int i = m_pSHMMgrGlobalMapData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息sgw_global_map共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		} else {
			memcpy(&m_pMgrGlobalMapData[i],&sGlobalMap,sizeof(SgwGlobalMap));
			m_pSHMMgrGlobalMapindex->add(sGlobalMap.m_iGID,i);
			return 0;
		}
	} else {
		return MBC_SHM_ERRNO_ADD_DATA_EXIST;
	}
#endif
}

int SHMGlobalMap::DelGlobalMap(SgwGlobalMap &sGlobalMap)
{
	unsigned int value = 0;

#ifndef _DF_LOCK_
	m_pSHMMgrGlobalMapLock->P();

	if(!m_pSHMMgrGlobalMapindex->get(sGlobalMap.m_iGID,&value)) {
		m_pSHMMgrGlobalMapLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		m_pSHMMgrGlobalMapindex->revokeIdx(sGlobalMap.m_iGID,value);
		m_pSHMMgrGlobalMapData->erase(value);
		memset(&m_pMgrGlobalMapData[value],0,sizeof(SgwGlobalMap));
		m_pSHMMgrGlobalMapLock->V();
		return 0;
	}
#else
	if(!m_pSHMMgrGlobalMapindex->get(sGlobalMap.m_iGID,&value)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		m_pSHMMgrGlobalMapindex->revokeIdx(sGlobalMap.m_iGID,value);
		m_pSHMMgrGlobalMapData->erase(value);
		memset(&m_pMgrGlobalMapData[value],0,sizeof(SgwGlobalMap));
		return 0;
	}
#endif
}

int SHMGlobalMap::UpdateGlobalMap(SgwGlobalMap &sGlobalMap)
{
	unsigned int value = 0;

#ifndef _DF_LOCK_
	m_pSHMMgrGlobalMapLock->P();

	if(!m_pSHMMgrGlobalMapindex->get(sGlobalMap.m_iGID,&value)) {
		m_pSHMMgrGlobalMapLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		memcpy(&m_pMgrGlobalMapData[value],&sGlobalMap,sizeof(SgwGlobalMap));
		m_pSHMMgrGlobalMapLock->V();
		return 0;
	}
#else
	if(!m_pSHMMgrGlobalMapindex->get(sGlobalMap.m_iGID,&value)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		memcpy(&m_pMgrGlobalMapData[value],&sGlobalMap,sizeof(SgwGlobalMap));
		return 0;
	}
#endif
}

int SHMGlobalMap::GetGlobalMapALL(vector<SgwGlobalMap>&vGlobalMap)
{
	if (m_pSHMMgrGlobalMapData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrGlobalMapData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	int sum = m_pSHMMgrGlobalMapData->getCount();
	vGlobalMap.clear();
	vGlobalMap.resize(sum);
	copy(m_pMgrGlobalMapData+1,m_pMgrGlobalMapData+sum+1,vGlobalMap.begin());

	if (sum == 1) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;

}

//获取全量数据
int SHMServiceTxtList::GetServiceContextListAll(vector<ServiceContextListData> & vServiceContextListData)
{
	vServiceContextListData.clear();
	if (m_pSHMMgrServiceTxtListData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrServiceTxtListData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	int pos = 1;
#ifdef _PV_STATE_
	m_pSHMMgrServiceTxtListLock->P();

	for (int i =1; i<=m_pSHMMgrServiceTxtListData->getCount();) {

		if (m_pSHMMgrServiceTxtListData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrServiceTxtListData->getTotal()) {
				break;
			}
			continue;
		}

		vServiceContextListData.push_back(m_pMgrServiceTxtListData[pos]);
		pos++;
		i++;
	}
	m_pSHMMgrServiceTxtListLock->V();
#else
	for (int i =1; i<=m_pSHMMgrServiceTxtListData->getCount();) {

		if (m_pSHMMgrServiceTxtListData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrServiceTxtListData->getTotal()) {
				break;
			}
			continue;
		}

		vServiceContextListData.push_back(m_pMgrServiceTxtListData[pos]);
		pos++;
		i++;
	}

#endif


	if (vServiceContextListData.size() == 0 ) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;;
}

int SHMServiceTxtList::GetServiceContextList(int _iServiceContextListId,ServiceContextListData &sServiceContextListData)
{
	unsigned int value = 0;

	if (m_pSHMMgrSrvTxtListIDIndex->get(_iServiceContextListId,&value)) {
		memcpy(&sServiceContextListData,&m_pMgrServiceTxtListData[value],sizeof(ServiceContextListData));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMServiceTxtList::GetServiceContextList(const char *_strServiceContextId, vector<ServiceContextListData>& vServiceContextListData)
{
	vServiceContextListData.clear();
	if (m_pSHMMgrServiceTxtListData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrServiceTxtListData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (m_pSHMMgrServiceTxtListIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrServiceTxtListIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int pValue = 0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(_strServiceContextId,m_pSHMMgrServiceTxtListData->getTotal());

#ifdef _PV_STATE_
	m_pSHMMgrServiceTxtListLock->P();

	if (m_pSHMMgrServiceTxtListIndex->get(lkey,&pValue)) {
		while (pValue) {
			if (strcmp(m_pMgrServiceTxtListData[pValue].m_strServiceContextId,_strServiceContextId)==0 ) {
				vServiceContextListData.push_back(m_pMgrServiceTxtListData[pValue]);
			}
			pValue = m_pMgrServiceTxtListData[pValue].m_iNext;
		}
	}
	m_pSHMMgrServiceTxtListLock->V();

#else
	if (m_pSHMMgrServiceTxtListIndex->get(lkey,&pValue)) {
		while (pValue) {
			if (strcmp(m_pMgrServiceTxtListData[pValue].m_strServiceContextId,_strServiceContextId)==0 ) {
				vServiceContextListData.push_back(m_pMgrServiceTxtListData[pValue]);
			}
			pValue = m_pMgrServiceTxtListData[pValue].m_iNext;
		}
	}

#endif

	if (vServiceContextListData.size() == 0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
}
//获取某一条数据
int SHMServiceTxtList::GetServiceContextID(int _iUserSeq,const char *_strServiceContextId,ServiceContextListData &sServiceContextListData)
{
	if (m_pSHMMgrServiceTxtListData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrServiceTxtListData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (m_pSHMMgrServiceTxtListIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrServiceTxtListIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int pValue = 0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(_strServiceContextId,m_pSHMMgrServiceTxtListData->getTotal());

#ifdef _PV_STATE_
	m_pSHMMgrServiceTxtListLock->P();

	if (m_pSHMMgrServiceTxtListIndex->get(lkey,&pValue)) {
		while (pValue) {
			if (strcmp(m_pMgrServiceTxtListData[pValue].m_strServiceContextId,_strServiceContextId)==0
				&&_iUserSeq ==m_pMgrServiceTxtListData[pValue].m_iUserSeq  ) {
					memcpy(&sServiceContextListData,&m_pMgrServiceTxtListData[pValue],sizeof(ServiceContextListData));
					m_pSHMMgrServiceTxtListLock->V();
					return 0;
			}
			pValue = m_pMgrServiceTxtListData[pValue].m_iNext;
		}
	}
	m_pSHMMgrServiceTxtListLock->V();

#else
	if (m_pSHMMgrServiceTxtListIndex->get(lkey,&pValue)) {
		while (pValue) {
			if (strcmp(m_pMgrServiceTxtListData[pValue].m_strServiceContextId,_strServiceContextId)==0
				&&_iUserSeq ==m_pMgrServiceTxtListData[pValue].m_iUserSeq  ) {
					memcpy(&sServiceContextListData,&m_pMgrServiceTxtListData[pValue],sizeof(ServiceContextListData));
					return 0;
			}
			pValue = m_pMgrServiceTxtListData[pValue].m_iNext;
		}
	}

#endif
	return MBC_SHM_QUERY_NO_DATE_FOUND;

}

int SHMServiceTxtList::GetServiceContextID(int _iUserSeq,vector<ServiceContextListData> &vServiceContextListData)
{
	if (m_pSHMMgrServiceTxtListData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrServiceTxtListData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
#ifdef _PV_STATE_
	vServiceContextListData.clear();
	m_pSHMMgrServiceTxtListLock->P();

	for (int i =1; i<=m_pSHMMgrServiceTxtListData->getCount(); i++) {

		if (m_pMgrServiceTxtListData[i].m_iUserSeq == _iUserSeq) {
			vServiceContextListData.push_back(m_pMgrServiceTxtListData[i]);
		}
	}
	m_pSHMMgrServiceTxtListLock->V();

	if(vServiceContextListData.size() == 0 ) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else
	vServiceContextListData.clear();

	for (int i =1; i<=m_pSHMMgrServiceTxtListData->getCount(); i++) {

		if (m_pMgrServiceTxtListData[i].m_iUserSeq == _iUserSeq) {
			vServiceContextListData.push_back(m_pMgrServiceTxtListData[i]);
		}
	}
	if(vServiceContextListData.size() == 0 ) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

//删除某条数据
int SHMServiceTxtList::DelServiceContextData(ServiceContextListData sServiceContextListData)
{
	if (m_pSHMMgrServiceTxtListData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrServiceTxtListData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (m_pSHMMgrServiceTxtListIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrServiceTxtListIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int pValue = 0;
	long lkey = SHMDataMgr::changekey(sServiceContextListData.m_strServiceContextId,m_pSHMMgrServiceTxtListData->getTotal());

	if (lkey == -1) {
		return 0;
	}

#ifndef _DF_LOCK_
	m_pSHMMgrServiceTxtListLock->P();

	if (!m_pSHMMgrServiceTxtListIndex->get(lkey,&pValue)) {
		m_pSHMMgrServiceTxtListLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (strcmp(m_pMgrServiceTxtListData[pValue].m_strServiceContextId,sServiceContextListData.m_strServiceContextId)==0
				&&m_pMgrServiceTxtListData[pValue].m_iUserSeq == sServiceContextListData.m_iUserSeq) {
					if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
						int pos = m_pMgrServiceTxtListData[pValue].m_iNext;
						if (pos !=0) {
							lkey = SHMDataMgr::changekey(m_pMgrServiceTxtListData[pos].m_strServiceContextId,m_pSHMMgrServiceTxtListData->getTotal());
							m_pSHMMgrServiceTxtListIndex->add(lkey,pos);
						} else {
							m_pSHMMgrServiceTxtListIndex->revokeIdx(lkey,pValue);
						}
						m_pSHMMgrServiceTxtListData->erase(pValue);
						memset(&m_pMgrServiceTxtListData[pValue],0,sizeof(ServiceContextListData));
						m_pSHMMgrServiceTxtListLock->V();
						return 0;
					} else {
						int pos = m_pMgrServiceTxtListData[pValue].m_iNext;

						m_pMgrServiceTxtListData[fVale].m_iNext = pos;
						m_pSHMMgrServiceTxtListData->erase(pValue);
						memset(&m_pMgrServiceTxtListData[pValue],0,sizeof(ServiceContextListData));
						m_pSHMMgrServiceTxtListLock->V();
						return 0;
					}
			}
			fVale = pValue;
			pValue = m_pMgrServiceTxtListData[pValue].m_iNext;
			_iCount++;
		}
	}
	m_pSHMMgrServiceTxtListLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else
	if (!m_pSHMMgrServiceTxtListIndex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (strcmp(m_pMgrServiceTxtListData[pValue].m_strServiceContextId,sServiceContextListData.m_strServiceContextId)==0
				&&m_pMgrServiceTxtListData[pValue].m_iUserSeq == sServiceContextListData.m_iUserSeq) {
					if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
						int pos = m_pMgrServiceTxtListData[pValue].m_iNext;
						if (pos !=0) {
							lkey = SHMDataMgr::changekey(m_pMgrServiceTxtListData[pos].m_strServiceContextId,m_pSHMMgrServiceTxtListData->getTotal());
							m_pSHMMgrServiceTxtListIndex->add(lkey,pos);
						} else {
							m_pSHMMgrServiceTxtListIndex->revokeIdx(lkey,pValue);
						}
						m_pSHMMgrServiceTxtListData->erase(pValue);
						memset(&m_pMgrServiceTxtListData[pValue],0,sizeof(ServiceContextListData));
						return 0;
					} else {
						int pos = m_pMgrServiceTxtListData[pValue].m_iNext;

						m_pMgrServiceTxtListData[fVale].m_iNext = pos;
						m_pSHMMgrServiceTxtListData->erase(pValue);
						memset(&m_pMgrServiceTxtListData[pValue],0,sizeof(ServiceContextListData));
						return 0;
					}
			}
			fVale = pValue;
			pValue = m_pMgrServiceTxtListData[pValue].m_iNext;
			_iCount++;
		}
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif


}

//更新数据
int SHMServiceTxtList::UpdateServiceContextdata(ServiceContextListData sServiceContextListData)
{
	if (m_pSHMMgrServiceTxtListData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrServiceTxtListData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (m_pSHMMgrServiceTxtListIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrServiceTxtListIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int pValue = 0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(sServiceContextListData.m_strServiceContextId,m_pSHMMgrServiceTxtListData->getTotal());
#ifndef _DF_LOCK_
	m_pSHMMgrServiceTxtListLock->P();

	if (!m_pSHMMgrServiceTxtListIndex->get(lkey,&pValue)) {
		m_pSHMMgrServiceTxtListLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {

		while (pValue) {
			if (strcmp(m_pMgrServiceTxtListData[pValue].m_strServiceContextId,sServiceContextListData.m_strServiceContextId)==0
				&&m_pMgrServiceTxtListData[pValue].m_iUserSeq == sServiceContextListData.m_iUserSeq) {
					sServiceContextListData.m_iNext = m_pMgrServiceTxtListData[pValue].m_iNext;
					memcpy(&m_pMgrServiceTxtListData[pValue],&sServiceContextListData,sizeof(ServiceContextListData));
					m_pSHMMgrServiceTxtListLock->V();
					return 0;
			}
			pValue = m_pMgrServiceTxtListData[pValue].m_iNext;
		}
	}
	m_pSHMMgrServiceTxtListLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else
	if (!m_pSHMMgrServiceTxtListIndex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {

		while (pValue) {
			if (strcmp(m_pMgrServiceTxtListData[pValue].m_strServiceContextId,sServiceContextListData.m_strServiceContextId)==0
				&&m_pMgrServiceTxtListData[pValue].m_iUserSeq == sServiceContextListData.m_iUserSeq) {
					sServiceContextListData.m_iNext = m_pMgrServiceTxtListData[pValue].m_iNext;
					memcpy(&m_pMgrServiceTxtListData[pValue],&sServiceContextListData,sizeof(ServiceContextListData));
					return 0;
			}
			pValue = m_pMgrServiceTxtListData[pValue].m_iNext;
		}
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif
}

//新增数据
int SHMServiceTxtList::AddServiceContextData(ServiceContextListData sServiceContextListData)
{
	if (m_pSHMMgrServiceTxtListData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrServiceTxtListData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (m_pSHMMgrServiceTxtListIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrServiceTxtListIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int pValue = 0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(sServiceContextListData.m_strServiceContextId,m_pSHMMgrServiceTxtListData->getTotal());
#ifndef _DF_LOCK_
	m_pSHMMgrServiceTxtListLock->P();

	if (!m_pSHMMgrServiceTxtListIndex->get(lkey,&pValue)) {
		int i = m_pSHMMgrServiceTxtListData->malloc();

		if (i == 0 ) {
			m_pSHMMgrServiceTxtListLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrServiceTxtListData[i],&sServiceContextListData,sizeof(ServiceContextListData));
		m_pSHMMgrServiceTxtListIndex->add(lkey,i);
	} else {
		int fValue = pValue;

		while (pValue) {
			if (strcmp(m_pMgrServiceTxtListData[pValue].m_strServiceContextId,sServiceContextListData.m_strServiceContextId)==0
				&&m_pMgrServiceTxtListData[pValue].m_iUserSeq ==sServiceContextListData.m_iUserSeq ) {
					m_pSHMMgrServiceTxtListLock->V();
					return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			pValue = m_pMgrServiceTxtListData[pValue].m_iNext;
		}
		int i = m_pSHMMgrServiceTxtListData->malloc();

		if (i == 0 ) {
			m_pSHMMgrServiceTxtListLock->V();
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrServiceTxtListData[i],&sServiceContextListData,sizeof(ServiceContextListData));
		int tmppos = m_pMgrServiceTxtListData[fValue].m_iNext;

		m_pMgrServiceTxtListData[fValue].m_iNext = i;
		m_pMgrServiceTxtListData[i].m_iNext = tmppos;

	}
	m_pSHMMgrServiceTxtListLock->V();
	return 0;
#else
	if (!m_pSHMMgrServiceTxtListIndex->get(lkey,&pValue)) {
		int i = m_pSHMMgrServiceTxtListData->malloc();

		if (i == 0 ) {
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrServiceTxtListData[i],&sServiceContextListData,sizeof(ServiceContextListData));
		m_pSHMMgrServiceTxtListIndex->add(lkey,i);
	} else {
		int fValue = pValue;

		while (pValue) {
			if (strcmp(m_pMgrServiceTxtListData[pValue].m_strServiceContextId,sServiceContextListData.m_strServiceContextId)==0
				&&m_pMgrServiceTxtListData[pValue].m_iUserSeq ==sServiceContextListData.m_iUserSeq ) {
					return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			pValue = m_pMgrServiceTxtListData[pValue].m_iNext;
		}
		int i = m_pSHMMgrServiceTxtListData->malloc();

		if (i == 0 ) {
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrServiceTxtListData[i],&sServiceContextListData,sizeof(ServiceContextListData));
		int tmppos = m_pMgrServiceTxtListData[fValue].m_iNext;
		m_pMgrServiceTxtListData[fValue].m_iNext = i;
		m_pMgrServiceTxtListData[i].m_iNext = tmppos;
	}
	return 0;
#endif


}

//往共享内存添加数据
int SHMStreamCtrlInfo::AddStreamCtrlInfo(StreamControlInfo &tStreamCtrlInfo)
{
	if (m_pSHMMgrStreamCtrlInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrStreamCtrlInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrStreamCtrlInfoIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrStreamCtrlInfoIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	char sTemp[1024] = {0};
	long lkey = 0;

	snprintf(sTemp,sizeof(sTemp)-1,"%s%s",tStreamCtrlInfo.m_sUserName,tStreamCtrlInfo.m_sSerConId);

	lkey = SHMDataMgr::changekey(sTemp,m_pSHMMgrStreamCtrlInfoData->getTotal());

	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrStreamCtrlInfoLock->P();

	if (!m_pSHMMgrStreamCtrlInfoIndex->get(lkey,&pValue)) {
		int i = m_pSHMMgrStreamCtrlInfoData->malloc();

		if (i == 0 ) {
			m_pSHMMgrStreamCtrlInfoLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrStreamCtrlInfoData[i],&tStreamCtrlInfo,sizeof(StreamControlInfo));
		m_pSHMMgrStreamCtrlInfoIndex->add(lkey,i);
	} else {
		int fValue = pValue;

		while (pValue) {
			if (strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sUserName,tStreamCtrlInfo.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sSerConId,tStreamCtrlInfo.m_sSerConId) == 0) {
					m_pSHMMgrStreamCtrlInfoLock->V();
					return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			pValue = m_pMgrStreamCtrlInfoData[pValue].m_iNext;
		}
		int i = m_pSHMMgrStreamCtrlInfoData->malloc();

		if (i == 0 ) {
			m_pSHMMgrStreamCtrlInfoLock->V();
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrStreamCtrlInfoData[i],&tStreamCtrlInfo,sizeof(StreamControlInfo));
		int tmppos = m_pMgrStreamCtrlInfoData[fValue].m_iNext;

		m_pMgrStreamCtrlInfoData[fValue].m_iNext = i;
		m_pMgrStreamCtrlInfoData[i].m_iNext = tmppos;
	}
	m_pSHMMgrStreamCtrlInfoLock->V();
	return 0;
#else

	if (!m_pSHMMgrStreamCtrlInfoIndex->get(lkey,&pValue)) {
		int i = m_pSHMMgrStreamCtrlInfoData->malloc();

		if (i == 0 ) {
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrStreamCtrlInfoData[i],&tStreamCtrlInfo,sizeof(StreamControlInfo));
		m_pSHMMgrStreamCtrlInfoIndex->add(lkey,i);
	} else {
		int fValue = pValue;

		while (pValue) {
			if (strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sUserName,tStreamCtrlInfo.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sSerConId,tStreamCtrlInfo.m_sSerConId) == 0) {
					return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			pValue = m_pMgrStreamCtrlInfoData[pValue].m_iNext;
		}
		int i = m_pSHMMgrStreamCtrlInfoData->malloc();

		if (i == 0 ) {
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrStreamCtrlInfoData[i],&tStreamCtrlInfo,sizeof(StreamControlInfo));
		int tmppos = m_pMgrStreamCtrlInfoData[fValue].m_iNext;
		m_pMgrStreamCtrlInfoData[fValue].m_iNext = i;
		m_pMgrStreamCtrlInfoData[i].m_iNext = tmppos;

	}
	return 0;
#endif

}

//根据m_sUserName和m_sSerConId为条件进行查找；返回值 0 成功 非0失败
int SHMStreamCtrlInfo::GetStreamCtrlInfo(StreamControlInfo &tStreamCtrlInfo)
{
	if (m_pSHMMgrStreamCtrlInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrStreamCtrlInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrStreamCtrlInfoIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrStreamCtrlInfoIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	char sTemp[1024] = {0};
	long lkey = 0;

	snprintf(sTemp,sizeof(sTemp)-1,"%s%s",tStreamCtrlInfo.m_sUserName,tStreamCtrlInfo.m_sSerConId);

	lkey = SHMDataMgr::changekey(sTemp,m_pSHMMgrStreamCtrlInfoData->getTotal());
	unsigned int pValue = 0;

#ifdef _PV_STATE_
	m_pSHMMgrStreamCtrlInfoLock->P();
	if (!m_pSHMMgrStreamCtrlInfoIndex->get(lkey,&pValue)) {
		m_pSHMMgrStreamCtrlInfoLock->V();
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		int fValue = pValue;
		while (pValue) {
			if (strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sUserName,tStreamCtrlInfo.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sSerConId,tStreamCtrlInfo.m_sSerConId) == 0) {
					memcpy(&tStreamCtrlInfo,&m_pMgrStreamCtrlInfoData[pValue],sizeof(StreamControlInfo));
					m_pSHMMgrStreamCtrlInfoLock->V();
					return 0;
			}
			pValue = m_pMgrStreamCtrlInfoData[pValue].m_iNext;
		}
	}
	m_pSHMMgrStreamCtrlInfoLock->V();
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#else
	if (!m_pSHMMgrStreamCtrlInfoIndex->get(lkey,&pValue)) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		int fValue = pValue;
		while (pValue) {
			if (strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sUserName,tStreamCtrlInfo.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sSerConId,tStreamCtrlInfo.m_sSerConId) == 0) {
					memcpy(&tStreamCtrlInfo,&m_pMgrStreamCtrlInfoData[pValue],sizeof(StreamControlInfo));
					return 0;
			}
			pValue = m_pMgrStreamCtrlInfoData[pValue].m_iNext;
		}
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#endif

}

//全量查询
int SHMStreamCtrlInfo::GetStreamCtrlInfoAll(vector<StreamControlInfo> &tStreamCtrlInfo)
{
	tStreamCtrlInfo.clear();
	if (m_pSHMMgrStreamCtrlInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrStreamCtrlInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	int pos = 1;
#ifdef _PV_STATE_
	m_pSHMMgrStreamCtrlInfoLock->P();

	for (int i =1 ;i <=m_pSHMMgrStreamCtrlInfoData->getCount();) {

		if (m_pSHMMgrStreamCtrlInfoData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrStreamCtrlInfoData->getTotal()) {
				break;
			}
			continue;
		}

		tStreamCtrlInfo.push_back(m_pMgrStreamCtrlInfoData[pos]);
		pos++;
		i++;
	}
	m_pSHMMgrStreamCtrlInfoLock->V();

	if (tStreamCtrlInfo.size() == 0 ) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else

	for (int i =1 ;i <=m_pSHMMgrStreamCtrlInfoData->getCount();) {

		if (m_pSHMMgrStreamCtrlInfoData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrStreamCtrlInfoData->getTotal()) {
				break;
			}
			continue;
		}

		tStreamCtrlInfo.push_back(m_pMgrStreamCtrlInfoData[pos]);
		pos++;
		i++;
	}
	if (tStreamCtrlInfo.size() == 0 ) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

int SHMStreamCtrlInfo::DelStreamCtrlInfo(StreamControlInfo &tStreamCtrlInfo)
{
	if (m_pSHMMgrStreamCtrlInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrStreamCtrlInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrStreamCtrlInfoIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrStreamCtrlInfoIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	char sTemp[1024] = {0};

	snprintf(sTemp,sizeof(sTemp)-1,"%s%s",tStreamCtrlInfo.m_sUserName,tStreamCtrlInfo.m_sSerConId);
	long lkey = SHMDataMgr::changekey(sTemp,m_pSHMMgrStreamCtrlInfoData->getTotal());
	if (lkey == -1) {
		return 0;
	}
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrStreamCtrlInfoLock->P();

	if (!m_pSHMMgrStreamCtrlInfoIndex->get(lkey,&pValue)) {
		m_pSHMMgrStreamCtrlInfoLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sUserName,tStreamCtrlInfo.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sSerConId,tStreamCtrlInfo.m_sSerConId) == 0) {
					if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
						int pos = m_pMgrStreamCtrlInfoData[pValue].m_iNext;
						if (pos !=0) {
							memset(sTemp,0,sizeof(sTemp));
							snprintf(sTemp,sizeof(sTemp)-1,"%s%s",m_pMgrStreamCtrlInfoData[pos].m_sUserName,m_pMgrStreamCtrlInfoData[pos].m_sSerConId);
							long lkey = SHMDataMgr::changekey(sTemp,m_pSHMMgrStreamCtrlInfoData->getTotal());
							m_pSHMMgrStreamCtrlInfoIndex->add(lkey,pos);
						} else {
							m_pSHMMgrStreamCtrlInfoIndex->revokeIdx(lkey,pValue);
						}
						m_pSHMMgrStreamCtrlInfoData->erase(pValue);
						memset(&m_pMgrStreamCtrlInfoData[pValue],0,sizeof(StreamControlInfo));
						m_pSHMMgrStreamCtrlInfoLock->V();
						return 0;
					} else {
						int pos = m_pMgrStreamCtrlInfoData[pValue].m_iNext;

						m_pMgrStreamCtrlInfoData[fVale].m_iNext = pos;
						m_pSHMMgrStreamCtrlInfoData->erase(pValue);
						memset(&m_pMgrStreamCtrlInfoData[pValue],0,sizeof(StreamControlInfo));
						m_pSHMMgrStreamCtrlInfoLock->V();
						return 0;
					}
			}
			fVale = pValue;
			pValue = m_pMgrStreamCtrlInfoData[pValue].m_iNext;
			_iCount++;
		}
	}
	m_pSHMMgrStreamCtrlInfoLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else
	if (!m_pSHMMgrStreamCtrlInfoIndex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sUserName,tStreamCtrlInfo.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sSerConId,tStreamCtrlInfo.m_sSerConId) == 0) {
					if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
						int pos = m_pMgrStreamCtrlInfoData[pValue].m_iNext;
						if (pos !=0) {
							memset(sTemp,0,sizeof(sTemp));
							snprintf(sTemp,sizeof(sTemp)-1,"%s%s",m_pMgrStreamCtrlInfoData[pos].m_sUserName,m_pMgrStreamCtrlInfoData[pos].m_sSerConId);
							long lkey = SHMDataMgr::changekey(sTemp,m_pSHMMgrStreamCtrlInfoData->getTotal());
							m_pSHMMgrStreamCtrlInfoIndex->add(lkey,pos);
						} else {
							m_pSHMMgrStreamCtrlInfoIndex->revokeIdx(lkey,pValue);
						}
						m_pSHMMgrStreamCtrlInfoData->erase(pValue);
						memset(&m_pMgrStreamCtrlInfoData[pValue],0,sizeof(StreamControlInfo));
						return 0;
					} else {
						int pos = m_pMgrStreamCtrlInfoData[pValue].m_iNext;

						m_pMgrStreamCtrlInfoData[fVale].m_iNext = pos;
						m_pSHMMgrStreamCtrlInfoData->erase(pValue);
						memset(&m_pMgrStreamCtrlInfoData[pValue],0,sizeof(StreamControlInfo));
						return 0;
					}
			}
			fVale = pValue;
			pValue = m_pMgrStreamCtrlInfoData[pValue].m_iNext;
			_iCount++;
		}
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif

}

//根据m_sUserName和m_sSerConId为条件进行修改；返回值 0 成功 非0失败
int SHMStreamCtrlInfo::UpdateStreamCtrlInfo(StreamControlInfo &tStreamCtrlInfo)
{
	if (m_pSHMMgrStreamCtrlInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrStreamCtrlInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrStreamCtrlInfoIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrStreamCtrlInfoIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	char sTemp[1024] = {0};
	long lkey = 0;
	snprintf(sTemp,sizeof(sTemp)-1,"%s%s",tStreamCtrlInfo.m_sUserName,tStreamCtrlInfo.m_sSerConId);
	lkey = SHMDataMgr::changekey(sTemp,m_pSHMMgrStreamCtrlInfoData->getTotal());
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrStreamCtrlInfoLock->P();

	if (!m_pSHMMgrStreamCtrlInfoIndex->get(lkey,&pValue)) {
		m_pSHMMgrStreamCtrlInfoLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sUserName,tStreamCtrlInfo.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sSerConId,tStreamCtrlInfo.m_sSerConId) == 0) {
					tStreamCtrlInfo.m_iNext = m_pMgrStreamCtrlInfoData[pValue].m_iNext;
					memcpy(&m_pMgrStreamCtrlInfoData[pValue],&tStreamCtrlInfo,sizeof(StreamControlInfo));
					m_pSHMMgrStreamCtrlInfoLock->V();
					return 0;
			}
			pValue = m_pMgrStreamCtrlInfoData[pValue].m_iNext;
		}

	}
	m_pSHMMgrStreamCtrlInfoLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else
	if (!m_pSHMMgrStreamCtrlInfoIndex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sUserName,tStreamCtrlInfo.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlInfoData[pValue].m_sSerConId,tStreamCtrlInfo.m_sSerConId) == 0) {
					tStreamCtrlInfo.m_iNext = m_pMgrStreamCtrlInfoData[pValue].m_iNext;
					memcpy(&m_pMgrStreamCtrlInfoData[pValue],&tStreamCtrlInfo,sizeof(StreamControlInfo));
					return 0;
			}
			pValue = m_pMgrStreamCtrlInfoData[pValue].m_iNext;
		}

	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif

}

int SHMStreamCtrlData::GetStreamCtrlData(const char *sUserName,const char * sSerConId , long lMonth, StreamControlData &tStreamCtrlData)
{
	return 0;
}

//往共享内存添加数据
int SHMStreamCtrlData::AddStreamCtrlData(StreamControlData &tStreamCtrlData)
{
	if (m_pSHMMgrStreamCtrlData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrStreamCtrlData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrStreamCtrlDataIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrStreamCtrlDataIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	char sTemp[1024] = {0};
	long lkey = 0;
	snprintf(sTemp,sizeof(sTemp)-1,"%s%s",tStreamCtrlData.m_sUserName,tStreamCtrlData.m_sSerConId);
	lkey = SHMDataMgr::changekey(sTemp,m_pSHMMgrStreamCtrlData->getTotal());
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrStreamCtrlDataLock->P();

	if (!m_pSHMMgrStreamCtrlDataIndex->get(lkey,&pValue)) {
		int i = m_pSHMMgrStreamCtrlData->malloc();

		if (i == 0 ) {
			m_pSHMMgrStreamCtrlDataLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrStreamCtrlData[i],&tStreamCtrlData,sizeof(StreamControlData));
		m_pSHMMgrStreamCtrlDataIndex->add(lkey,i);
	} else {
		int fValue = pValue;

		while (pValue) {
			if (strcmp(m_pMgrStreamCtrlData[pValue].m_sUserName,tStreamCtrlData.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlData[pValue].m_sSerConId,tStreamCtrlData.m_sSerConId) == 0) {
					m_pSHMMgrStreamCtrlDataLock->V();
					return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			pValue = m_pMgrStreamCtrlData[pValue].m_iNext;
		}
		int i = m_pSHMMgrStreamCtrlData->malloc();

		if (i == 0 ) {
			m_pSHMMgrStreamCtrlDataLock->V();
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrStreamCtrlData[i],&tStreamCtrlData,sizeof(StreamControlData));
		int tmppos = m_pMgrStreamCtrlData[fValue].m_iNext;
		m_pMgrStreamCtrlData[fValue].m_iNext = i;
		m_pMgrStreamCtrlData[i].m_iNext = tmppos;

	}
	m_pSHMMgrStreamCtrlDataLock->V();
	return 0;
#else
	if (!m_pSHMMgrStreamCtrlDataIndex->get(lkey,&pValue)) {
		int i = m_pSHMMgrStreamCtrlData->malloc();

		if (i == 0 ) {
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrStreamCtrlData[i],&tStreamCtrlData,sizeof(StreamControlData));
		m_pSHMMgrStreamCtrlDataIndex->add(lkey,i);
	} else {
		int fValue = pValue;

		while (pValue) {
			if (strcmp(m_pMgrStreamCtrlData[pValue].m_sUserName,tStreamCtrlData.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlData[pValue].m_sSerConId,tStreamCtrlData.m_sSerConId) == 0) {
					return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			pValue = m_pMgrStreamCtrlData[pValue].m_iNext;
		}
		int i = m_pSHMMgrStreamCtrlData->malloc();

		if (i == 0 ) {
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrStreamCtrlData[i],&tStreamCtrlData,sizeof(StreamControlData));
		int tmppos = m_pMgrStreamCtrlData[fValue].m_iNext;
		m_pMgrStreamCtrlData[fValue].m_iNext = i;
		m_pMgrStreamCtrlData[i].m_iNext = tmppos;

	}
	return 0;
#endif

}

int SHMStreamCtrlData::GetStreamCtrlDataAll(vector<StreamControlData> &vStreamCtrlData)
{
	vStreamCtrlData.clear();
	if (m_pSHMMgrStreamCtrlData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrStreamCtrlData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	int pos = 1;
#ifdef _PV_STATE_
	m_pSHMMgrStreamCtrlDataLock->P();

	for (int i =1 ;i <=m_pSHMMgrStreamCtrlData->getCount();) {

		if (m_pSHMMgrStreamCtrlData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrStreamCtrlData->getTotal()) {
				break;
			}
			continue;
		}

		vStreamCtrlData.push_back(m_pMgrStreamCtrlData[pos]);
		pos++;
		i++;
	}
	m_pSHMMgrStreamCtrlDataLock->V();

	if (vStreamCtrlData.size() == 0 ) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else

	for (int i =1 ;i <=m_pSHMMgrStreamCtrlData->getCount();) {

		if (m_pSHMMgrStreamCtrlData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrStreamCtrlData->getTotal()) {
				break;
			}
			continue;
		}

		vStreamCtrlData.push_back(m_pMgrStreamCtrlData[pos]);
		pos++;
		i++;
	}
	if (vStreamCtrlData.size() == 0 ) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

//根据m_sUserName和m_sSerConId为条件进行查找；返回值 0 成功 非0失败
int SHMStreamCtrlData::GetStreamCtrlData(StreamControlData &tStreamCtrlData)
{
	if (m_pSHMMgrStreamCtrlData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrStreamCtrlData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrStreamCtrlDataIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrStreamCtrlDataIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	char sTemp[1024] = {0};
	long lkey =0;
	snprintf(sTemp,sizeof(sTemp)-1,"%s%s",tStreamCtrlData.m_sUserName,tStreamCtrlData.m_sSerConId);
	lkey = SHMDataMgr::changekey(sTemp,m_pSHMMgrStreamCtrlData->getTotal());
	unsigned int pValue = 0;

#ifdef _PV_STATE_
	m_pSHMMgrStreamCtrlDataLock->P();

	if (!m_pSHMMgrStreamCtrlDataIndex->get(lkey,&pValue)) {
		m_pSHMMgrStreamCtrlDataLock->V();
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		while (pValue) {
			if (strcmp(m_pMgrStreamCtrlData[pValue].m_sUserName,tStreamCtrlData.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlData[pValue].m_sSerConId,tStreamCtrlData.m_sSerConId) == 0) {
					memcpy(&tStreamCtrlData,&m_pMgrStreamCtrlData[pValue],sizeof(tStreamCtrlData));
					m_pSHMMgrStreamCtrlDataLock->V();
					return 0;
			}
			pValue = m_pMgrStreamCtrlData[pValue].m_iNext;
		}
	}
	m_pSHMMgrStreamCtrlDataLock->V();
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#else
	if (!m_pSHMMgrStreamCtrlDataIndex->get(lkey,&pValue)) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		while (pValue) {
			if (strcmp(m_pMgrStreamCtrlData[pValue].m_sUserName,tStreamCtrlData.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlData[pValue].m_sSerConId,tStreamCtrlData.m_sSerConId) == 0) {
					memcpy(&tStreamCtrlData,&m_pMgrStreamCtrlData[pValue],sizeof(tStreamCtrlData));
					return 0;
			}
			pValue = m_pMgrStreamCtrlData[pValue].m_iNext;
		}
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#endif

}

int SHMStreamCtrlData::DelStreamCtrlData(StreamControlData &tStreamCtrlData)
{
	if (m_pSHMMgrStreamCtrlData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrStreamCtrlData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrStreamCtrlDataIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrStreamCtrlDataIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	char sTemp[1024] = {0};
	snprintf(sTemp,sizeof(sTemp)-1,"%s%s",tStreamCtrlData.m_sUserName,tStreamCtrlData.m_sSerConId);
	long lkey = SHMDataMgr::changekey(sTemp,m_pSHMMgrStreamCtrlData->getTotal());
	if (lkey == -1) return 0;
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrStreamCtrlDataLock->P();

	if (!m_pSHMMgrStreamCtrlDataIndex->get(lkey,&pValue)) {
		m_pSHMMgrStreamCtrlDataLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (strcmp(m_pMgrStreamCtrlData[pValue].m_sUserName,tStreamCtrlData.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlData[pValue].m_sSerConId,tStreamCtrlData.m_sSerConId) == 0) {

					if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
						int pos = m_pMgrStreamCtrlData[pValue].m_iNext;
						if (pos !=0) {
							memset(sTemp,0,sizeof(sTemp));
							snprintf(sTemp,sizeof(sTemp)-1,"%s%s",m_pMgrStreamCtrlData[pos].m_sUserName,m_pMgrStreamCtrlData[pos].m_sSerConId);
							long lkey = SHMDataMgr::changekey(sTemp,m_pSHMMgrStreamCtrlData->getTotal());
							m_pSHMMgrStreamCtrlDataIndex->add(lkey,pos);
						} else {
							m_pSHMMgrStreamCtrlDataIndex->revokeIdx(lkey,pValue);
						}
						m_pSHMMgrStreamCtrlData->erase(pValue);
						memset(&m_pMgrStreamCtrlData[pValue],0,sizeof(StreamControlData));
						m_pSHMMgrStreamCtrlDataLock->V();
						return 0;
					} else {
						int pos = m_pMgrStreamCtrlData[pValue].m_iNext;

						m_pMgrStreamCtrlData[fVale].m_iNext = pos;
						m_pSHMMgrStreamCtrlData->erase(pValue);
						memset(&m_pMgrStreamCtrlData[pValue],0,sizeof(StreamControlData));
						m_pSHMMgrStreamCtrlDataLock->V();
						return 0;
					}
			}
			fVale = pValue;
			pValue = m_pMgrStreamCtrlData[pValue].m_iNext;
			_iCount++;
		}
	}
	m_pSHMMgrStreamCtrlDataLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else
	if (!m_pSHMMgrStreamCtrlDataIndex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (strcmp(m_pMgrStreamCtrlData[pValue].m_sUserName,tStreamCtrlData.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlData[pValue].m_sSerConId,tStreamCtrlData.m_sSerConId) == 0) {

					if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
						int pos = m_pMgrStreamCtrlData[pValue].m_iNext;
						if (pos !=0) {
							memset(sTemp,0,sizeof(sTemp));
							snprintf(sTemp,sizeof(sTemp)-1,"%s%s",m_pMgrStreamCtrlData[pos].m_sUserName,m_pMgrStreamCtrlData[pos].m_sSerConId);
							long lkey = SHMDataMgr::changekey(sTemp,m_pSHMMgrStreamCtrlData->getTotal());
							m_pSHMMgrStreamCtrlDataIndex->add(lkey,pos);
						} else {
							m_pSHMMgrStreamCtrlDataIndex->revokeIdx(lkey,pValue);
						}
						m_pSHMMgrStreamCtrlData->erase(pValue);
						memset(&m_pMgrStreamCtrlData[pValue],0,sizeof(StreamControlData));
						return 0;
					} else {
						int pos = m_pMgrStreamCtrlData[pValue].m_iNext;

						m_pMgrStreamCtrlData[fVale].m_iNext = pos;
						m_pSHMMgrStreamCtrlData->erase(pValue);
						memset(&m_pMgrStreamCtrlData[pValue],0,sizeof(StreamControlData));
						return 0;
					}
			}
			fVale = pValue;
			pValue = m_pMgrStreamCtrlData[pValue].m_iNext;
			_iCount++;
		}
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif

}

//根据m_sUserName和m_sSerConId为条件进行修改；返回值 0 成功 非0失败
int SHMStreamCtrlData::UpdateStreamCtrlData(StreamControlData &tStreamCtrlData)
{
	if (m_pSHMMgrStreamCtrlData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrStreamCtrlData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrStreamCtrlDataIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrStreamCtrlDataIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	char sTemp[1024] = {0};
	long lkey = 0;
	snprintf(sTemp,sizeof(sTemp)-1,"%s%s",tStreamCtrlData.m_sUserName,tStreamCtrlData.m_sSerConId);
	lkey = SHMDataMgr::changekey(sTemp,m_pSHMMgrStreamCtrlData->getTotal());
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrStreamCtrlDataLock->P();

	if (!m_pSHMMgrStreamCtrlDataIndex->get(lkey,&pValue)) {
		m_pSHMMgrStreamCtrlDataLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {

		while (pValue) {

			if (strcmp(m_pMgrStreamCtrlData[pValue].m_sUserName,tStreamCtrlData.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlData[pValue].m_sSerConId,tStreamCtrlData.m_sSerConId) == 0) {
					tStreamCtrlData.m_iNext = m_pMgrStreamCtrlData[pValue].m_iNext;
					memcpy(&m_pMgrStreamCtrlData[pValue],&tStreamCtrlData,sizeof(StreamControlData));
					m_pSHMMgrStreamCtrlDataLock->V();
					return 0;
			}
			pValue = m_pMgrStreamCtrlData[pValue].m_iNext;
		}

	}
	m_pSHMMgrStreamCtrlDataLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else
	if (!m_pSHMMgrStreamCtrlDataIndex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {

		while (pValue) {

			if (strcmp(m_pMgrStreamCtrlData[pValue].m_sUserName,tStreamCtrlData.m_sUserName)==0
				&&strcmp(m_pMgrStreamCtrlData[pValue].m_sSerConId,tStreamCtrlData.m_sSerConId) == 0) {
					tStreamCtrlData.m_iNext = m_pMgrStreamCtrlData[pValue].m_iNext;
					memcpy(&m_pMgrStreamCtrlData[pValue],&tStreamCtrlData,sizeof(StreamControlData));
					return 0;
			}
			pValue = m_pMgrStreamCtrlData[pValue].m_iNext;
		}

	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif

}

int SHMUserInfoData::GetUserInfoData(int iUserSeq,UserInfoData & vsUserInfoData)
{
	if (m_pSHMMgrUserInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrUserInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrUserInfoIndex_A == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrUserInfoIndex_A->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int value = 0;
	//#ifdef _PV_STATE
#ifdef _PV_STATE_
	m_pSHMMgrUserInfoDataLock->P();
	if(m_pSHMMgrUserInfoIndex_A->get(iUserSeq,&value)) {
		memcpy(&vsUserInfoData,&m_pMgrUserInfoData[value],sizeof(UserInfoData));
	} else {
		m_pSHMMgrUserInfoDataLock->V();
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	m_pSHMMgrUserInfoDataLock->V();
	return 0;
#else
	if(m_pSHMMgrUserInfoIndex_A->get(iUserSeq,&value)) {
		memcpy(&vsUserInfoData,&m_pMgrUserInfoData[value],sizeof(UserInfoData));
	} else {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif


}

//根据m_sUserName和m_sSerConId为条件进行查找；返回值 0 成功 非0失败
int SHMUserInfoData::GetUserInfoData(const char *_sUserName,UserInfoData & vsUserInfoData)
{
	if (m_pSHMMgrUserInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrUserInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrUserInfoIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrUserInfoIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	long lkey = 0;
	lkey = SHMDataMgr::changekey(_sUserName,m_pSHMMgrUserInfoData->getTotal());
	unsigned int pValue = 0;
	//#ifdef _PV_STATE
#ifdef _PV_STATE_
	m_pSHMMgrUserInfoDataLock->P();

	if (!m_pSHMMgrUserInfoIndex->get(lkey,&pValue)) {
		m_pSHMMgrUserInfoDataLock->V();
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {

		while (pValue) {

			if (strcmp(m_pMgrUserInfoData[pValue].m_strUserName,_sUserName)==0 ) {
				memcpy(&vsUserInfoData,&m_pMgrUserInfoData[pValue],sizeof(UserInfoData));
				m_pSHMMgrUserInfoDataLock->V();
				return 0;
			}
			pValue = m_pMgrUserInfoData[pValue].m_iNext;
		}
	}
	m_pSHMMgrUserInfoDataLock->V();
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#else

	if (!m_pSHMMgrUserInfoIndex->get(lkey,&pValue)) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {

		while (pValue) {

			if (strcmp(m_pMgrUserInfoData[pValue].m_strUserName,_sUserName)==0 ) {
				memcpy(&vsUserInfoData,&m_pMgrUserInfoData[pValue],sizeof(UserInfoData));
				return 0;
			}
			pValue = m_pMgrUserInfoData[pValue].m_iNext;
		}
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#endif

}

int SHMUserInfoData::GetUserInfoDataAll(vector<UserInfoData> & vsUserInfoData)
{
	if (m_pSHMMgrUserInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrUserInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	int pos = 1;
	vsUserInfoData.clear();
#ifdef _PV_STATE_
	m_pSHMMgrUserInfoDataLock->P();
	for (int i =1 ;i<=m_pSHMMgrUserInfoData->getCount();) {
		if (m_pSHMMgrUserInfoData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrUserInfoData->getTotal()) {
				break;
			}
			continue;
		}

		vsUserInfoData.push_back(m_pMgrUserInfoData[pos]);
		pos++;
		i++;
	}
	m_pSHMMgrUserInfoDataLock->V();
	if (vsUserInfoData.size()==0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else
	for (int i =1 ;i<=m_pSHMMgrUserInfoData->getCount(); ) {
		if (m_pSHMMgrUserInfoData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrUserInfoData->getTotal()) {
				break;
			}
			continue;
		}

		vsUserInfoData.push_back(m_pMgrUserInfoData[pos]);
		pos++;
		i++;
	}
	if (vsUserInfoData.size()==0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

int SHMUserInfoData::DeleteUserInfoData(const UserInfoData & vsUserInfoData)
{
	if (m_pSHMMgrUserInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrUserInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrUserInfoIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrUserInfoIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	long lkey = 0;
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrUserInfoDataLock->P();

	lkey = SHMDataMgr::changekey(vsUserInfoData.m_strUserName,m_pSHMMgrUserInfoData->getTotal());

	if (!m_pSHMMgrUserInfoIndex->get(lkey,&pValue)) {
		m_pSHMMgrUserInfoDataLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (strcmp(m_pMgrUserInfoData[pValue].m_strUserName,vsUserInfoData.m_strUserName)==0 ) {

					if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
						int pos = m_pMgrUserInfoData[pValue].m_iNext;

						if (pos !=0) {
							lkey = SHMDataMgr::changekey(m_pMgrUserInfoData[pos].m_strUserName,m_pSHMMgrUserInfoData->getTotal());
							m_pSHMMgrUserInfoIndex->add(lkey,pos);
						} else {
							m_pSHMMgrUserInfoIndex->revokeIdx(lkey,pValue);
						}

						if (m_pSHMMgrUserInfoIndex_A->get(m_pMgrUserInfoData[pValue].m_iUserSeq,&pValue)) {
							m_pSHMMgrUserInfoIndex_A->revokeIdx(m_pMgrUserInfoData[pValue].m_iUserSeq,pValue);//回收索引
						}

						m_pSHMMgrUserInfoData->erase(pValue);
						memset(&m_pMgrUserInfoData[pValue],0,sizeof(UserInfoData));
						m_pSHMMgrUserInfoDataLock->V();
						return 0;
					} else {
						int pos = m_pMgrUserInfoData[pValue].m_iNext;
						if (m_pSHMMgrUserInfoIndex_A->get(m_pMgrUserInfoData[pValue].m_iUserSeq,&pValue)) {
							m_pSHMMgrUserInfoIndex_A->revokeIdx(m_pMgrUserInfoData[pValue].m_iUserSeq,pValue);//回收索引
						}
						m_pMgrUserInfoData[fVale].m_iNext = pos;

						m_pSHMMgrUserInfoData->erase(pValue);
						memset(&m_pMgrUserInfoData[pValue],0,sizeof(UserInfoData));
						m_pSHMMgrUserInfoDataLock->V();
						return 0;
					}
			}
			fVale = pValue;
			pValue = m_pMgrUserInfoData[pValue].m_iNext;
			_iCount++;
		}
	}
	m_pSHMMgrUserInfoDataLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else

	lkey = SHMDataMgr::changekey(vsUserInfoData.m_strUserName,m_pSHMMgrUserInfoData->getTotal());

	if (!m_pSHMMgrUserInfoIndex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (strcmp(m_pMgrUserInfoData[pValue].m_strUserName,vsUserInfoData.m_strUserName)==0 ) {

				if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
					int pos = m_pMgrUserInfoData[pValue].m_iNext;

					if (pos !=0) {
						lkey = SHMDataMgr::changekey(m_pMgrUserInfoData[pos].m_strUserName,m_pSHMMgrUserInfoData->getTotal());
						m_pSHMMgrUserInfoIndex->add(lkey,pos);
					} else {
						m_pSHMMgrUserInfoIndex->revokeIdx(lkey,pValue);
					}

					if (m_pSHMMgrUserInfoIndex_A->get(m_pMgrUserInfoData[pValue].m_iUserSeq,&pValue)) {
						m_pSHMMgrUserInfoIndex_A->revokeIdx(m_pMgrUserInfoData[pValue].m_iUserSeq,pValue);//回收索引
					}

					m_pSHMMgrUserInfoData->erase(pValue);
					memset(&m_pMgrUserInfoData[pValue],0,sizeof(UserInfoData));
					return 0;
				} else {
					int pos = m_pMgrUserInfoData[pValue].m_iNext;
					if (m_pSHMMgrUserInfoIndex_A->get(m_pMgrUserInfoData[pValue].m_iUserSeq,&pValue)) {
						m_pSHMMgrUserInfoIndex_A->revokeIdx(m_pMgrUserInfoData[pValue].m_iUserSeq,pValue);//回收索引
					}
					m_pMgrUserInfoData[fVale].m_iNext = pos;

					m_pSHMMgrUserInfoData->erase(pValue);
					memset(&m_pMgrUserInfoData[pValue],0,sizeof(UserInfoData));
					return 0;
				}
			}
			fVale = pValue;
			pValue = m_pMgrUserInfoData[pValue].m_iNext;
			_iCount++;
		}
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif

}

int SHMUserInfoData::DelUserInfoData(const UserInfoData & vsUserInfoData)
{
	if (m_pSHMMgrUserInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrUserInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrUserInfoIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrUserInfoIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	long lkey = 0;
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrUserInfoDataLock->P();

	if (m_pSHMMgrUserInfoIndex_A->get(vsUserInfoData.m_iUserSeq,&pValue)) {
		m_pSHMMgrUserInfoIndex_A->revokeIdx(vsUserInfoData.m_iUserSeq,pValue);//回收索引


		lkey = SHMDataMgr::changekey(m_pMgrUserInfoData[pValue].m_strUserName,m_pSHMMgrUserInfoData->getTotal());

		unsigned int temp = 0;

		if (!m_pSHMMgrUserInfoIndex->get(lkey,&temp)) {
			m_pSHMMgrUserInfoDataLock->V();
			return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
		} else {
			int _iCount = 0;
			int fVale = 0;

			while (pValue) {

				if (strcmp(m_pMgrUserInfoData[temp].m_strUserName,m_pMgrUserInfoData[pValue].m_strUserName)==0
					&&m_pMgrUserInfoData[temp].m_iUserSeq  == m_pMgrUserInfoData[pValue].m_iUserSeq) {

						if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
							int pos = m_pMgrUserInfoData[temp].m_iNext;

							if (pos !=0) {
								lkey = SHMDataMgr::changekey(m_pMgrUserInfoData[pos].m_strUserName,m_pSHMMgrUserInfoData->getTotal());
								m_pSHMMgrUserInfoIndex->add(lkey,pos);
							} else {
								m_pSHMMgrUserInfoIndex->revokeIdx(lkey,temp);
							}
							m_pSHMMgrUserInfoData->erase(temp);
							memset(&m_pMgrUserInfoData[temp],0,sizeof(UserInfoData));
							m_pSHMMgrUserInfoDataLock->V();
							return 0;
						} else {
							int pos = m_pMgrUserInfoData[temp].m_iNext;

							m_pMgrUserInfoData[fVale].m_iNext = pos;
							m_pSHMMgrUserInfoData->erase(temp);
							memset(&m_pMgrUserInfoData[temp],0,sizeof(UserInfoData));
							m_pSHMMgrUserInfoDataLock->V();
							return 0;
						}
				}
				fVale = temp;
				temp = m_pMgrUserInfoData[temp].m_iNext;
				_iCount++;
			}
		}
	}
	m_pSHMMgrUserInfoDataLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else
	if (m_pSHMMgrUserInfoIndex_A->get(vsUserInfoData.m_iUserSeq,&pValue)) {
		m_pSHMMgrUserInfoIndex_A->revokeIdx(vsUserInfoData.m_iUserSeq,pValue);//回收索引


		lkey = SHMDataMgr::changekey(m_pMgrUserInfoData[pValue].m_strUserName,m_pSHMMgrUserInfoData->getTotal());

		unsigned int temp = 0;

		if (!m_pSHMMgrUserInfoIndex->get(lkey,&temp)) {
			return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
		} else {
			int _iCount = 0;
			int fVale = 0;

			while (pValue) {

				if (strcmp(m_pMgrUserInfoData[temp].m_strUserName,m_pMgrUserInfoData[pValue].m_strUserName)==0
					&&m_pMgrUserInfoData[temp].m_iUserSeq  == m_pMgrUserInfoData[pValue].m_iUserSeq) {

						if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
							int pos = m_pMgrUserInfoData[temp].m_iNext;

							if (pos !=0) {
								lkey = SHMDataMgr::changekey(m_pMgrUserInfoData[pos].m_strUserName,m_pSHMMgrUserInfoData->getTotal());
								m_pSHMMgrUserInfoIndex->add(lkey,pos);
							} else {
								m_pSHMMgrUserInfoIndex->revokeIdx(lkey,temp);
							}
							m_pSHMMgrUserInfoData->erase(temp);
							memset(&m_pMgrUserInfoData[temp],0,sizeof(UserInfoData));
							return 0;
						} else {
							int pos = m_pMgrUserInfoData[temp].m_iNext;

							m_pMgrUserInfoData[fVale].m_iNext = pos;
							m_pSHMMgrUserInfoData->erase(temp);
							memset(&m_pMgrUserInfoData[temp],0,sizeof(UserInfoData));
							return 0;
						}
				}
				fVale = temp;
				temp = m_pMgrUserInfoData[temp].m_iNext;
				_iCount++;
			}
		}
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;

#endif

}

int SHMUserInfoData::AddUserInfoData(UserInfoData & tUserInfoData)
{
	if (m_pSHMMgrUserInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrUserInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrUserInfoIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrUserInfoIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}


	long lkey = 0;
	lkey = SHMDataMgr::changekey(tUserInfoData.m_strUserName,m_pSHMMgrUserInfoData->getTotal());
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrUserInfoDataLock->P();

	if(m_pSHMMgrUserInfoIndex_A->get(tUserInfoData.m_iUserSeq,&pValue)) {//数据已经存在
		m_pSHMMgrUserInfoDataLock->V();
		return MBC_SHM_ADD_DATA_EXIST;
	} else {
		int i = m_pSHMMgrUserInfoData->malloc();

		if (i == 0) {
			m_pSHMMgrUserInfoDataLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrUserInfoData[i],&tUserInfoData,sizeof(UserInfoData));
		m_pSHMMgrUserInfoIndex_A->add(tUserInfoData.m_iUserSeq,i);

		if (!m_pSHMMgrUserInfoIndex->get(lkey,&pValue)) {
			m_pSHMMgrUserInfoIndex->add (lkey,i);
			m_pMgrUserInfoData[i].m_iNext = 0;
		} else {
			int tmppos = m_pMgrUserInfoData[pValue].m_iNext;
			m_pMgrUserInfoData[pValue].m_iNext = i;
			m_pMgrUserInfoData[i].m_iNext = tmppos;

		}
	}
	m_pSHMMgrUserInfoDataLock->V();
	return 0;
#else

	if(m_pSHMMgrUserInfoIndex_A->get(tUserInfoData.m_iUserSeq,&pValue)) {//数据已经存在
		return MBC_SHM_ADD_DATA_EXIST;
	} else {
		int i = m_pSHMMgrUserInfoData->malloc();

		if (i == 0) {
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrUserInfoData[i],&tUserInfoData,sizeof(UserInfoData));
		m_pSHMMgrUserInfoIndex_A->add(tUserInfoData.m_iUserSeq,i);

		if (!m_pSHMMgrUserInfoIndex->get(lkey,&pValue)) {
			m_pSHMMgrUserInfoIndex->add (lkey,i);
			m_pMgrUserInfoData[i].m_iNext = 0;
		} else {
			int tmppos = m_pMgrUserInfoData[pValue].m_iNext;
			m_pMgrUserInfoData[pValue].m_iNext = i;
			m_pMgrUserInfoData[i].m_iNext = tmppos;
		}
	}
	return 0;
#endif

}

int SHMUserInfoData::UpdateUserInfoData(UserInfoData & tUserInfoData)
{
	if (m_pSHMMgrUserInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrUserInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrUserInfoIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrUserInfoIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	long lkey = 0;
	lkey = SHMDataMgr::changekey(tUserInfoData.m_strUserName,m_pSHMMgrUserInfoData->getTotal());
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrUserInfoDataLock->P();

	if (!m_pSHMMgrUserInfoIndex->get(lkey,&pValue)) {
		m_pSHMMgrUserInfoDataLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrUserInfoData[pValue].m_strUserName,tUserInfoData.m_strUserName)==0 ) {
				tUserInfoData.m_iNext = m_pMgrUserInfoData[pValue].m_iNext;
				memcpy(&m_pMgrUserInfoData[pValue],&tUserInfoData,sizeof(UserInfoData));
				m_pSHMMgrUserInfoDataLock->V();
				return 0;
			}
			pValue = m_pMgrUserInfoData[pValue].m_iNext;
		}
	}
	m_pSHMMgrUserInfoDataLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else

	if (!m_pSHMMgrUserInfoIndex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrUserInfoData[pValue].m_strUserName,tUserInfoData.m_strUserName)==0 ) {
				tUserInfoData.m_iNext = m_pMgrUserInfoData[pValue].m_iNext;
				memcpy(&m_pMgrUserInfoData[pValue],&tUserInfoData,sizeof(UserInfoData));
				return 0;
			}
			pValue = m_pMgrUserInfoData[pValue].m_iNext;
		}
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif

}

int SHMSgwSlaqueRel::GetSlaQueueData(SgwSlaQueue & sSgwSlaQueue)
{
	if (m_pSHMMgrSgwSlaqueRelData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrSgwSlaqueRelData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrSgwSlaqueRelIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrSgwSlaqueRelIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int pValue = 0;
#ifdef _PV_STATE_
	m_pSHMMgrSgwSlaqueRelLock->P();

	if (!m_pSHMMgrSgwSlaqueRelIndex->get(sSgwSlaQueue.m_iQueuePriority,&pValue)) {
		m_pSHMMgrSgwSlaqueRelLock->V();
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {

		while (pValue) {

			if (m_pMgrSgwSlaqueRelData[pValue].m_iQueuePriority == sSgwSlaQueue.m_iQueuePriority) {
				memcpy(&sSgwSlaQueue,&m_pMgrSgwSlaqueRelData[pValue],sizeof(SgwSlaQueue));
				m_pSHMMgrSgwSlaqueRelLock->V();
				return 0;
			}
			pValue = m_pMgrSgwSlaqueRelData[pValue].m_iNext;
		}
	}
	m_pSHMMgrSgwSlaqueRelLock->V();
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#else

	if (!m_pSHMMgrSgwSlaqueRelIndex->get(sSgwSlaQueue.m_iQueuePriority,&pValue)) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {

		while (pValue) {

			if (m_pMgrSgwSlaqueRelData[pValue].m_iQueuePriority == sSgwSlaQueue.m_iQueuePriority) {
				memcpy(&sSgwSlaQueue,&m_pMgrSgwSlaqueRelData[pValue],sizeof(SgwSlaQueue));
				return 0;
			}
			pValue = m_pMgrSgwSlaqueRelData[pValue].m_iNext;
		}
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#endif

}

int SHMSgwSlaqueRel::GetSlaQueueAllData(vector<SgwSlaQueue> & vSgwSlaQueue)
{
	if (m_pSHMMgrSgwSlaqueRelData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrSgwSlaqueRelData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	vSgwSlaQueue.clear();
#ifdef  _PV_STATE_
	m_pSHMMgrSgwSlaqueRelLock->P();
	int pos =1;

	for (int i =1 ;i<=m_pSHMMgrSgwSlaqueRelData->getCount();) {

		if (m_pSHMMgrSgwSlaqueRelData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrSgwSlaqueRelData->getTotal()) {
				break;
			}
			continue;
		}

		vSgwSlaQueue.push_back(m_pMgrSgwSlaqueRelData[pos]);
		pos++;
		i++;
	}
	m_pSHMMgrSgwSlaqueRelLock->V();
	if (vSgwSlaQueue.size()==0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else
	int pos =1;

	for (int i =1 ;i<=m_pSHMMgrSgwSlaqueRelData->getCount();) {

		if (m_pSHMMgrSgwSlaqueRelData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrSgwSlaqueRelData->getTotal()) {
				break;
			}
			continue;
		}

		vSgwSlaQueue.push_back(m_pMgrSgwSlaqueRelData[pos]);
		pos++;
		i++;
	}
	if (vSgwSlaQueue.size()==0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

int SHMSgwSlaqueRel::DelSlaQueueData(SgwSlaQueue sSgwSlaQueue)
{
	if (m_pSHMMgrSgwSlaqueRelData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrSgwSlaqueRelData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrSgwSlaqueRelIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrSgwSlaqueRelIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrSgwSlaqueRelLock->P();

	if (!m_pSHMMgrSgwSlaqueRelIndex->get(sSgwSlaQueue.m_iQueuePriority,&pValue)) {
		m_pSHMMgrSgwSlaqueRelLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (m_pMgrSgwSlaqueRelData[pValue].m_iQueuePriority == sSgwSlaQueue.m_iQueuePriority) {
				if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
					int pos = m_pMgrSgwSlaqueRelData[pValue].m_iNext;
					if (pos !=0) {
						m_pSHMMgrSgwSlaqueRelIndex->add(m_pMgrSgwSlaqueRelData[pos].m_iQueuePriority,pos);
					} else {
						m_pSHMMgrSgwSlaqueRelIndex->revokeIdx(m_pMgrSgwSlaqueRelData[pos].m_iQueuePriority,pValue);
					}
					m_pSHMMgrSgwSlaqueRelData->erase(pValue);
					memset(&m_pMgrSgwSlaqueRelData[pValue],0,sizeof(SgwSlaQueue));
					//m_pMgrSgwSlaqueRelData[pValue].m_iQueuePriority = -1;
					m_pSHMMgrSgwSlaqueRelLock->V();
					return 0;
				} else {
					int pos = m_pMgrSgwSlaqueRelData[pValue].m_iNext;
					m_pMgrSgwSlaqueRelData[fVale].m_iNext = pos;
					m_pSHMMgrSgwSlaqueRelData->erase(pValue);
					memset(&m_pMgrSgwSlaqueRelData[pValue],0,sizeof(SgwSlaQueue));
					//m_pMgrSgwSlaqueRelData[pValue].m_iQueuePriority = -1;
					m_pSHMMgrSgwSlaqueRelLock->V();
					return 0;
				}
			}
			fVale = pValue;
			pValue = m_pMgrSgwSlaqueRelData[pValue].m_iNext;
			_iCount++;
		}
	}
	m_pSHMMgrSgwSlaqueRelLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else

	if (!m_pSHMMgrSgwSlaqueRelIndex->get(sSgwSlaQueue.m_iQueuePriority,&pValue)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (m_pMgrSgwSlaqueRelData[pValue].m_iQueuePriority == sSgwSlaQueue.m_iQueuePriority) {
				if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
					int pos = m_pMgrSgwSlaqueRelData[pValue].m_iNext;
					if (pos !=0) {
						m_pSHMMgrSgwSlaqueRelIndex->add(m_pMgrSgwSlaqueRelData[pos].m_iQueuePriority,pos);
					} else {
						m_pSHMMgrSgwSlaqueRelIndex->revokeIdx(m_pMgrSgwSlaqueRelData[pos].m_iQueuePriority,pValue);
					}
					m_pSHMMgrSgwSlaqueRelData->erase(pValue);
					memset(&m_pMgrSgwSlaqueRelData[pValue],0,sizeof(SgwSlaQueue));
					//m_pMgrSgwSlaqueRelData[pValue].m_iQueuePriority = -1;
					return 0;
				} else {
					int pos = m_pMgrSgwSlaqueRelData[pValue].m_iNext;

					m_pMgrSgwSlaqueRelData[fVale].m_iNext = pos;
					m_pSHMMgrSgwSlaqueRelData->erase(pValue);
					memset(&m_pMgrSgwSlaqueRelData[pValue],0,sizeof(SgwSlaQueue));
					//m_pMgrSgwSlaqueRelData[pValue].m_iQueuePriority = -1;
					return 0;
				}
			}
			fVale = pValue;
			pValue = m_pMgrSgwSlaqueRelData[pValue].m_iNext;
			_iCount++;
		}
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif

}

int SHMSgwSlaqueRel::UpdateSlaQueueData(SgwSlaQueue sSgwSlaQueue)
{
	if (m_pSHMMgrSgwSlaqueRelData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrSgwSlaqueRelData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrSgwSlaqueRelIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrSgwSlaqueRelIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrSgwSlaqueRelLock->P();

	if (!m_pSHMMgrSgwSlaqueRelIndex->get(sSgwSlaQueue.m_iQueuePriority,&pValue)) {
		m_pSHMMgrSgwSlaqueRelLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (m_pMgrSgwSlaqueRelData[pValue].m_iQueuePriority == sSgwSlaQueue.m_iQueuePriority) {
				sSgwSlaQueue.m_iNext = m_pMgrSgwSlaqueRelData[pValue].m_iNext;
				memcpy(&m_pMgrSgwSlaqueRelData[pValue],&sSgwSlaQueue,sizeof(SgwSlaQueue));
				m_pSHMMgrSgwSlaqueRelLock->V();
				return 0;
			}
			pValue = m_pMgrSgwSlaqueRelData[pValue].m_iNext;
		}
	}
	m_pSHMMgrSgwSlaqueRelLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else
	if (!m_pSHMMgrSgwSlaqueRelIndex->get(sSgwSlaQueue.m_iQueuePriority,&pValue)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (m_pMgrSgwSlaqueRelData[pValue].m_iQueuePriority == sSgwSlaQueue.m_iQueuePriority) {
				sSgwSlaQueue.m_iNext = m_pMgrSgwSlaqueRelData[pValue].m_iNext;
				memcpy(&m_pMgrSgwSlaqueRelData[pValue],&sSgwSlaQueue,sizeof(SgwSlaQueue));
				return 0;
			}
			pValue = m_pMgrSgwSlaqueRelData[pValue].m_iNext;
		}
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif

}

int SHMSgwSlaqueRel::AddSlaQueueData(SgwSlaQueue sSgwSlaQueue)
{
	if (m_pSHMMgrSgwSlaqueRelData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrSgwSlaqueRelData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrSgwSlaqueRelIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrSgwSlaqueRelIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrSgwSlaqueRelLock->P();

	if (!m_pSHMMgrSgwSlaqueRelIndex->get(sSgwSlaQueue.m_iQueuePriority,&pValue)) {
		int i = m_pSHMMgrSgwSlaqueRelData->malloc();

		if (i == 0) {
			m_pSHMMgrSgwSlaqueRelLock->V();
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrSgwSlaqueRelData[i],&sSgwSlaQueue,sizeof(SgwSlaQueue));
		m_pSHMMgrSgwSlaqueRelIndex->add (sSgwSlaQueue.m_iQueuePriority,i);
		m_pMgrSgwSlaqueRelData[i].m_iNext = 0;
	} else {
		int fValue = pValue;

		while (pValue) {

			if (m_pMgrSgwSlaqueRelData[pValue].m_iQueuePriority == sSgwSlaQueue.m_iQueuePriority) {
				m_pSHMMgrSgwSlaqueRelLock->V();
				return MBC_SHM_ADD_DATA_EXIST;
			}

			pValue = m_pMgrSgwSlaqueRelData[pValue].m_iNext;
		}

		int i = m_pSHMMgrSgwSlaqueRelData->malloc();

		if (i == 0 ) {
			m_pSHMMgrSgwSlaqueRelLock->V();
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrSgwSlaqueRelData[i],&sSgwSlaQueue,sizeof(SgwSlaQueue));
		int tmppos = m_pMgrSgwSlaqueRelData[fValue].m_iNext;
		m_pMgrSgwSlaqueRelData[fValue].m_iNext = i;
		m_pMgrSgwSlaqueRelData[i].m_iNext = tmppos;
	}
	m_pSHMMgrSgwSlaqueRelLock->V();
	return 0;
#else

	if (!m_pSHMMgrSgwSlaqueRelIndex->get(sSgwSlaQueue.m_iQueuePriority,&pValue)) {
		int i = m_pSHMMgrSgwSlaqueRelData->malloc();

		if (i == 0) {
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrSgwSlaqueRelData[i],&sSgwSlaQueue,sizeof(SgwSlaQueue));
		m_pSHMMgrSgwSlaqueRelIndex->add (sSgwSlaQueue.m_iQueuePriority,i);
		m_pMgrSgwSlaqueRelData[i].m_iNext = 0;
	} else {
		int fValue = pValue;

		while (pValue) {

			if (m_pMgrSgwSlaqueRelData[pValue].m_iQueuePriority == sSgwSlaQueue.m_iQueuePriority) {
				return MBC_SHM_ADD_DATA_EXIST;
			}
			pValue = m_pMgrSgwSlaqueRelData[pValue].m_iNext;
		}

		int i = m_pSHMMgrSgwSlaqueRelData->malloc();

		if (i == 0 ) {
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrSgwSlaqueRelData[i],&sSgwSlaQueue,sizeof(SgwSlaQueue));
		int tmppos = m_pMgrSgwSlaqueRelData[fValue].m_iNext;
		m_pMgrSgwSlaqueRelData[fValue].m_iNext = i;
		m_pMgrSgwSlaqueRelData[i].m_iNext = tmppos;

	}
	return 0;
#endif
}

int SHMNetInfoData::GetNetInfoAllData(vector<NetInfoData> & vNetInfoData)
{
	if (m_pSHMMgrNetInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrNetInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	vNetInfoData.clear();
#ifdef _PV_STATE_
	m_pSHMMgrNetInfoLock->P();

	for (int i =1 ;i<=m_pSHMMgrNetInfoData->getCount(); i++) {
		vNetInfoData.push_back(m_pMgrNetInfoData[i]);
	}
	m_pSHMMgrNetInfoLock->V();

	if (vNetInfoData.size()==0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else
	for (int i =1 ;i<=m_pSHMMgrNetInfoData->getCount(); i++) {
		vNetInfoData.push_back(m_pMgrNetInfoData[i]);
	}

	if (vNetInfoData.size()==0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

int SHMNetInfoData::GetNetInfoData(NetInfoData &sNetInfoData)
{
	if (m_pSHMMgrNetInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrNetInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrNetInfoIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}
	if (!m_pSHMMgrNetInfoIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
#ifdef _PV_STATE_
	unsigned int pValue = 0;
	m_pSHMMgrNetInfoLock->P();

	if (!m_pSHMMgrNetInfoIndex->get(sNetInfoData.m_iNetinfoSeq,&pValue)) {
		m_pSHMMgrNetInfoLock->V();
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}

	memcpy(&sNetInfoData,&m_pMgrNetInfoData[pValue],sizeof(NetInfoData));
	m_pSHMMgrNetInfoLock->V();
	return 0;
#else
	unsigned int pValue = 0;

	if (!m_pSHMMgrNetInfoIndex->get(sNetInfoData.m_iNetinfoSeq,&pValue)) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}

	memcpy(&sNetInfoData,&m_pMgrNetInfoData[pValue],sizeof(NetInfoData));
	return 0;
#endif

}

int SHMNetInfoData::GetNetInfoData(const char *_strNetinfoName,NetInfoData &oNetInfoData)
{
	if (m_pSHMMgrNetInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrNetInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrNetInfoIndex_S == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}
	if (!m_pSHMMgrNetInfoIndex_S->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	long lkey = 0;
	lkey = SHMDataMgr::changekey(_strNetinfoName,m_pSHMMgrNetInfoData->getTotal());
	unsigned int pValue = 0;
#ifdef _PV_STATE_
	m_pSHMMgrNetInfoLock->P();

	if (!m_pSHMMgrNetInfoIndex_S->get(lkey,&pValue)) {
		m_pSHMMgrNetInfoLock->V();
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		while (pValue) {
			if (strcmp(_strNetinfoName,m_pMgrNetInfoData[pValue].m_strNetinfoName) == 0) {
				memcpy(&oNetInfoData,&m_pMgrNetInfoData[pValue],sizeof(NetInfoData));
				m_pSHMMgrNetInfoLock->V();
				return 0;
			}
			pValue = m_pMgrNetInfoData[pValue].m_iNext;
		}
	}
	m_pSHMMgrNetInfoLock->V();
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#else
	if (!m_pSHMMgrNetInfoIndex_S->get(lkey,&pValue)) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		while (pValue) {
			if (strcmp(_strNetinfoName,m_pMgrNetInfoData[pValue].m_strNetinfoName) == 0) {
				memcpy(&oNetInfoData,&m_pMgrNetInfoData[pValue],sizeof(NetInfoData));
				return 0;
			}
			pValue = m_pMgrNetInfoData[pValue].m_iNext;
		}
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#endif

}

int SHMNetInfoData::UpdateNetInfoData(const NetInfoData &sNetInfoData)
{
	if (m_pSHMMgrNetInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrNetInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrNetInfoIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrNetInfoIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrNetInfoLock->P();

	if (!m_pSHMMgrNetInfoIndex->get(sNetInfoData.m_iNetinfoSeq,&pValue)) {
		m_pSHMMgrNetInfoLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		memcpy(&m_pMgrNetInfoData[pValue],&sNetInfoData,sizeof(NetInfoData));
	}
	m_pSHMMgrNetInfoLock->V();
	return 0;
#else

	if (!m_pSHMMgrNetInfoIndex->get(sNetInfoData.m_iNetinfoSeq,&pValue)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		memcpy(&m_pMgrNetInfoData[pValue],&sNetInfoData,sizeof(NetInfoData));
	}
	return 0;
#endif

}

int SHMNetInfoData::DelNetInfoData(const NetInfoData &sNetInfoData)
{
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrNetInfoLock->P();

	if (!m_pSHMMgrNetInfoIndex->get(sNetInfoData.m_iNetinfoSeq,&pValue)) {
		m_pSHMMgrNetInfoLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		m_pSHMMgrNetInfoIndex->revokeIdx(sNetInfoData.m_iNetinfoSeq,pValue);
		m_pSHMMgrNetInfoData->erase(pValue);
		memset(&m_pMgrNetInfoData[pValue],0,sizeof(NetInfoData));
	}
	m_pSHMMgrNetInfoLock->V();
	return 0;
#else
	if (!m_pSHMMgrNetInfoIndex->get(sNetInfoData.m_iNetinfoSeq,&pValue)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		m_pSHMMgrNetInfoIndex->revokeIdx(sNetInfoData.m_iNetinfoSeq,pValue);
		m_pSHMMgrNetInfoData->erase(pValue);
		memset(&m_pMgrNetInfoData[pValue],0,sizeof(NetInfoData));
	}
	return 0;
#endif
}

int SHMNetInfoData::AddNetInfoData(const NetInfoData &sNetInfoData)
{
	if (m_pSHMMgrNetInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrNetInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrNetInfoIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrNetInfoIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrNetInfoLock->P();

	if (!m_pSHMMgrNetInfoIndex->get(sNetInfoData.m_iNetinfoSeq,&pValue)) {
		int i = m_pSHMMgrNetInfoData->malloc();

		if (i == 0) {
			m_pSHMMgrNetInfoLock->V();
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrNetInfoData[i],&sNetInfoData,sizeof(NetInfoData));
		m_pSHMMgrNetInfoIndex->add (sNetInfoData.m_iNetinfoSeq,i);

		long lkey = 0;
		lkey = SHMDataMgr::changekey(sNetInfoData.m_strNetinfoName,m_pSHMMgrNetInfoData->getTotal());

		if (m_pSHMMgrNetInfoIndex_S->get(lkey,&pValue)) {
			int pos = m_pMgrNetInfoData[pValue].m_iNext;
			m_pMgrNetInfoData[pValue].m_iNext = i;
			m_pMgrNetInfoData[i].m_iNext = pos;
		} else {
			m_pSHMMgrNetInfoIndex_S->add(lkey,i);
			m_pMgrNetInfoData[i].m_iNext = 0;
		}

	} else {
		m_pSHMMgrNetInfoLock->V();
		return MBC_SHM_ERRNO_ADD_DATA_EXIST;
	}
	m_pSHMMgrNetInfoLock->V();
	return 0;
#else

	if (!m_pSHMMgrNetInfoIndex->get(sNetInfoData.m_iNetinfoSeq,&pValue)) {
		int i = m_pSHMMgrNetInfoData->malloc();

		if (i == 0) {
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrNetInfoData[i],&sNetInfoData,sizeof(NetInfoData));
		m_pSHMMgrNetInfoIndex->add (sNetInfoData.m_iNetinfoSeq,i);

		long lkey = 0;
		lkey = SHMDataMgr::changekey(sNetInfoData.m_strNetinfoName,m_pSHMMgrNetInfoData->getTotal());

		if (m_pSHMMgrNetInfoIndex_S->get(lkey,&pValue)) {
			int pos = m_pMgrNetInfoData[pValue].m_iNext;
			m_pMgrNetInfoData[pValue].m_iNext = i;
			m_pMgrNetInfoData[i].m_iNext = pos;
		} else {
			m_pSHMMgrNetInfoIndex_S->add(lkey,i);
			m_pMgrNetInfoData[i].m_iNext = 0;
		}

	} else {
		return MBC_SHM_ERRNO_ADD_DATA_EXIST;
	}
	return 0;
#endif

}

int SHMSessionData::GetSessionData(SessionData &tSessionData)
{
	if (m_pSHMMgrSessionData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrSessionData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrSessionIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrSessionIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	char sKey[12] = {0};
	long lkey = SHMDataMgr::changekey(changeSessionID(tSessionData.m_sSessionId,sKey),m_pSHMMgrSessionData->getTotal());
	unsigned int pValue = 0;
#ifndef _PV_STATE_
	m_pSHMMgrSessionLock->P();

	if (!m_pSHMMgrSessionIndex->get(lkey,&pValue)) {
		m_pSHMMgrSessionLock->V();
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {

		while (pValue) {

			if (strcmp(m_pMgrSessionData[pValue].m_sSessionId,tSessionData.m_sSessionId)==0) {
				memcpy(&tSessionData,&m_pMgrSessionData[pValue],sizeof(SessionData));
				m_pSHMMgrSessionLock->V();
				return 0;
			}
			pValue = m_pMgrSessionData[pValue].m_iNext;
		}
	}
	m_pSHMMgrSessionLock->V();
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#else
	if (!m_pSHMMgrSessionIndex->get(lkey,&pValue)) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {

		while (pValue) {

			if (strcmp(m_pMgrSessionData[pValue].m_sSessionId,tSessionData.m_sSessionId)==0) {
				memcpy(&tSessionData,&m_pMgrSessionData[pValue],sizeof(SessionData));
				return 0;
			}
			pValue = m_pMgrSessionData[pValue].m_iNext;
		}
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#endif

}

int SHMSessionData::DelSessionData(SessionData &tSessionData,bool _bCopy)
{
	if (m_pSHMMgrSessionData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrSessionData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrSessionIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrSessionIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	char sKey[12] = {0};
	long lkey = SHMDataMgr::changekey(changeSessionID(tSessionData.m_sSessionId,sKey),m_pSHMMgrSessionData->getTotal());
	if (lkey == -1) return 0;
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrSessionLock->P();

	if (!m_pSHMMgrSessionIndex->get(lkey,&pValue)) {
		m_pSHMMgrSessionLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (strcmp(m_pMgrSessionData[pValue].m_sSessionId,tSessionData.m_sSessionId)==0) {

				if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
					int pos = m_pMgrSessionData[pValue].m_iNext;
					if (pos !=0) {
						//下面截取sessionid的特殊字段的11个字节
						lkey = SHMDataMgr::changekey(changeSessionID(m_pMgrSessionData[pos].m_sSessionId,sKey),m_pSHMMgrSessionData->getTotal());
						m_pSHMMgrSessionIndex->add(lkey,pos);
					} else {
						m_pSHMMgrSessionIndex->revokeIdx(lkey,pValue);
					}
					m_pSHMMgrSessionData->erase(pValue);
					if (_bCopy) {
						memcpy(&tSessionData,&m_pMgrSessionData[pValue],sizeof(SessionData));
					}
					memset(&m_pMgrSessionData[pValue],0,sizeof(SessionData));
					m_pSHMMgrSessionLock->V();
					return 0;
				} else {
					int pos = m_pMgrSessionData[pValue].m_iNext;
					if (pos !=0) {
						m_pMgrSessionData[fVale].m_iNext = pos;
						m_pSHMMgrSessionData->erase(pValue);
						if (_bCopy) {
							memcpy(&tSessionData,&m_pMgrSessionData[pValue],sizeof(SessionData));
						}
						memset(&m_pMgrSessionData[pValue],0,sizeof(SessionData));
						m_pSHMMgrSessionLock->V();
						return 0;
					}else {
						m_pMgrSessionData[fVale].m_iNext = 0;
						m_pSHMMgrSessionData->erase(pValue);
						if (_bCopy) {
							memcpy(&tSessionData,&m_pMgrSessionData[pValue],sizeof(SessionData));
						}
						memset(&m_pMgrSessionData[pValue],0,sizeof(SessionData));
						m_pSHMMgrSessionLock->V();
						return 0;
					}
				}
			}
			fVale = pValue;
			pValue = m_pMgrSessionData[pValue].m_iNext;
			_iCount++;
		}
	}
	m_pSHMMgrSessionLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else

	if (!m_pSHMMgrSessionIndex->get(sKey,&pValue)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (strcmp(m_pMgrSessionData[pValue].m_sSessionId,tSessionData.m_sSessionId)==0) {

				if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
					int pos = m_pMgrSessionData[pValue].m_iNext;
					if (pos !=0) {
						//下面截取sessionid的特殊字段的11个字节
						lkey = SHMDataMgr::changekey(changeSessionID(m_pMgrSessionData[pos].m_sSessionId,sKey),m_pSHMMgrSessionData->getTotal());
						m_pSHMMgrSessionIndex->add(lkey,pos);
					} else {
						m_pSHMMgrSessionIndex->revokeIdx(lkey,pValue);
					}
					m_pSHMMgrSessionData->erase(pValue);
					if (_bCopy) {
						memcpy(&tSessionData,&m_pMgrSessionData[pValue],sizeof(SessionData));
					}
					memset(&m_pMgrSessionData[pValue],0,sizeof(SessionData));
					return 0;
				} else {
					int pos = m_pMgrSessionData[pValue].m_iNext;

					m_pMgrSessionData[fVale].m_iNext = pos;
					m_pSHMMgrSessionData->erase(pValue);
					if (_bCopy) {
						memcpy(&tSessionData,&m_pMgrSessionData[pValue],sizeof(SessionData));
					}
					memset(&m_pMgrSessionData[pValue],0,sizeof(SessionData));
					return 0;
				}
			}
			fVale = pValue;
			pValue = m_pMgrSessionData[pValue].m_iNext;
			_iCount++;
		}
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif

}

int SHMSessionData::UpdateSessionData(SessionData &tSessionData)
{
	if (m_pSHMMgrSessionData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrSessionData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrSessionIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrSessionIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	char sKey[12] = {0};
	long lkey = SHMDataMgr::changekey(changeSessionID(tSessionData.m_sSessionId,sKey),m_pSHMMgrSessionData->getTotal());
	if (lkey == -1) return 0;
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrSessionLock->P();

	if (!m_pSHMMgrSessionIndex->get(lkey,&pValue)) {
		m_pSHMMgrSessionLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrSessionData[pValue].m_sSessionId,tSessionData.m_sSessionId)==0) {
				tSessionData.m_iNext = m_pMgrSessionData[pValue].m_iNext;
				memcpy(&m_pMgrSessionData[pValue],&tSessionData,sizeof(SessionData));
				m_pSHMMgrSessionLock->V();
				return 0;
			}
			pValue = m_pMgrSessionData[pValue].m_iNext;
		}
	}
	m_pSHMMgrSessionLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else

	if (!m_pSHMMgrSessionIndex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrSessionData[pValue].m_sSessionId,tSessionData.m_sSessionId)==0) {
				tSessionData.m_iNext = m_pMgrSessionData[pValue].m_iNext;
				memcpy(&m_pMgrSessionData[pValue],&tSessionData,sizeof(SessionData));
				return 0;
			}
			pValue = m_pMgrSessionData[pValue].m_iNext;
		}
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif


}
//按下标查询数据
int SHMSessionData::GetSessionDataALL(vector<SessionData> &vSessionData)
{
	if (m_pSHMMgrSessionData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrSessionData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	vSessionData.clear();
#ifndef  _PV_STATE_
	m_pSHMMgrSessionLock->P();
	int pos =1;

	for (int i =1 ;i<=m_pSHMMgrSessionData->getCount();) {

		if (m_pSHMMgrSessionData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrSessionData->getTotal()) {
				break;
			}
			continue;
		}

		vSessionData.push_back(m_pMgrSessionData[pos]);
		pos++;
		i++;
	}
	m_pSHMMgrSessionLock->V();
	if (vSessionData.size()==0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else
	int pos =1;

	for (int i =1 ;i<=m_pSHMMgrSessionData->getCount();) {

		if (m_pSHMMgrSessionData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrSessionData->getTotal()) {
				break;
			}
			continue;
		}

		vSessionData.push_back(m_pMgrSessionData[pos]);
		pos++;
		i++;
	}
	if (vSessionData.size()==0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

int SHMSessionData::AddSessionData(SessionData &tSessionData)
{
	if (m_pSHMMgrSessionData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrSessionData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrSessionIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrSessionIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	char skey[12] = {0};
	long lkey = SHMDataMgr::changekey(changeSessionID(tSessionData.m_sSessionId,skey),m_pSHMMgrSessionData->getTotal());
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrSessionLock->P();

	if (!m_pSHMMgrSessionIndex->get(lkey,&pValue)) {
		int i = m_pSHMMgrSessionData->malloc();

		if (i == 0) {
			m_pSHMMgrSessionLock->V();
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrSessionData[i],&tSessionData,sizeof(SessionData));
		m_pSHMMgrSessionIndex->add (lkey,i);
		m_pMgrSessionData[i].m_iNext = 0;
	} else {
		int fValue = pValue;

		while (pValue) {

			if (strcmp(m_pMgrSessionData[pValue].m_sSessionId, tSessionData.m_sSessionId)==0) {
				m_pSHMMgrSessionLock->V();
				return MBC_SHM_ADD_DATA_EXIST;
			}

			pValue = m_pMgrSessionData[pValue].m_iNext;
		}

		int i = m_pSHMMgrSessionData->malloc();

		if (i == 0 ) {
			m_pSHMMgrSessionLock->V();
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrSessionData[i],&tSessionData,sizeof(SessionData));
		int tmppos = m_pMgrSessionData[fValue].m_iNext;
		m_pMgrSessionData[fValue].m_iNext = i;
		m_pMgrSessionData[i].m_iNext = tmppos;

	}
	m_pSHMMgrSessionLock->V();
	return 0;
#else

	if (!m_pSHMMgrSessionIndex->get(lkey,&pValue)) {
		int i = m_pSHMMgrSessionData->malloc();

		if (i == 0) {
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrSessionData[i],&tSessionData,sizeof(SessionData));
		m_pSHMMgrSessionIndex->add (lkey,i);
		m_pMgrSessionData[i].m_iNext = 0;
	} else {
		int fValue = pValue;

		while (pValue) {

			if (strcmp(m_pMgrSessionData[pValue].m_sSessionId, tSessionData.m_sSessionId)==0) {
				return MBC_SHM_ADD_DATA_EXIST;
			}

			pValue = m_pMgrSessionData[pValue].m_iNext;
		}

		int i = m_pSHMMgrSessionData->malloc();

		if (i == 0 ) {
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrSessionData[i],&tSessionData,sizeof(SessionData));
		int tmppos = m_pMgrSessionData[fValue].m_iNext;
		m_pMgrSessionData[fValue].m_iNext = i;
		m_pMgrSessionData[i].m_iNext = tmppos;
	}
	return 0;
#endif
}

int SHMCongestLevel::GetShmCongestLevel(CongestLevel & sShmCongestLevel)
{
	if (m_pSHMMgrCongestLevelData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrCongestLevelData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
#ifdef _PV_STATE_
	m_pSHMMgrCongestLevelLock->P();
	memcpy(&sShmCongestLevel,&m_pMgrCongestLevel[1],sizeof(CongestLevel));
	m_pSHMMgrCongestLevelLock->V();
	return 0;
#else
	memcpy(&sShmCongestLevel,&m_pMgrCongestLevel[1],sizeof(CongestLevel));
	return 0;
#endif

}

int SHMCongestLevel::UpdateShmCongestLevel(CongestLevel sShmCongestLevel)
{
	if (m_pSHMMgrCongestLevelData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrCongestLevelData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
#ifdef _DF_LOCK_
	m_pSHMMgrCongestLevelLock->P();
	memcpy(&m_pMgrCongestLevel[1],&sShmCongestLevel,sizeof(CongestLevel));
	m_pSHMMgrCongestLevelLock->V();
	return 0;

#else
	memcpy(&m_pMgrCongestLevel[1],&sShmCongestLevel,sizeof(CongestLevel));
	return 0;

#endif

}

int SHMCongestLevel::AddShmCongestLevel(const CongestLevel & sShmCongestLevel)
{
	if (m_pSHMMgrCongestLevelData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrCongestLevelData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	int i = m_pSHMMgrCongestLevelData->malloc();
	if (i == 0) {
		return MBC_MEM_SPACE_SMALL;
	}
	memcpy(&m_pMgrCongestLevel[i],&sShmCongestLevel,sizeof(CongestLevel));
	return 0;
}

// 通过主键m_strServiceContextId 查询对应的信息
int SHMServiceTxtListBase::getServiceTxtListBase(ServiceContextListBaseData &sTxtBaseData)
{
	if (m_pSHMMgrServiceTxtListBaseData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrServiceTxtListBaseData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrServiceTxtListBaseIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrServiceTxtListBaseIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	long lKey = 0;
	lKey = SHMDataMgr::changekey(sTxtBaseData.m_strServiceContextId,m_pSHMMgrServiceTxtListBaseData->getTotal());
	unsigned int pValue = 0;
#ifdef _PV_STATE_
	m_pSHMMgrServiceTxtListBaseLock->P();

	if (!m_pSHMMgrServiceTxtListBaseIndex->get(lKey,&pValue)) {
		m_pSHMMgrServiceTxtListBaseLock->V();
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {

		while (pValue) {

			if (strcmp(m_pMgrServiceTxtListBaseData[pValue].m_strServiceContextId,sTxtBaseData.m_strServiceContextId)==0) {
				memcpy(&sTxtBaseData,&m_pMgrServiceTxtListBaseData[pValue],sizeof(ServiceContextListBaseData));
				m_pSHMMgrServiceTxtListBaseLock->V();
				return 0;
			}
			pValue = m_pMgrServiceTxtListBaseData[pValue].m_iNext;
		}
	}
	m_pSHMMgrServiceTxtListBaseLock->V();
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#else
	if (!m_pSHMMgrServiceTxtListBaseIndex->get(lKey,&pValue)) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {

		while (pValue) {

			if (strcmp(m_pMgrServiceTxtListBaseData[pValue].m_strServiceContextId,sTxtBaseData.m_strServiceContextId)==0) {
				memcpy(&sTxtBaseData,&m_pMgrServiceTxtListBaseData[pValue],sizeof(ServiceContextListBaseData));
				return 0;
			}
			pValue = m_pMgrServiceTxtListBaseData[pValue].m_iNext;
		}
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#endif

}

int SHMServiceTxtListBase::getServiceTxtListBaseAll(vector<ServiceContextListBaseData> &vTxtBaseData)
{
	vTxtBaseData.clear();
#ifdef _PV_STATE_
	m_pSHMMgrServiceTxtListBaseLock->P();
	for (int i=1; i<=m_pSHMMgrServiceTxtListBaseData->getCount();i++) {
		vTxtBaseData.push_back(m_pMgrServiceTxtListBaseData[i]);
	}
	m_pSHMMgrServiceTxtListBaseLock->V();
	if (vTxtBaseData.size() == 0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else
	for (int i=1; i<=m_pSHMMgrServiceTxtListBaseData->getCount();i++) {
		vTxtBaseData.push_back(m_pMgrServiceTxtListBaseData[i]);
	}
	if (vTxtBaseData.size() == 0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

int SHMServiceTxtListBase::DelServiceTxtListBase(ServiceContextListBaseData &sTxtBaseData)
{
	if (m_pSHMMgrServiceTxtListBaseData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrServiceTxtListBaseData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrServiceTxtListBaseIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrServiceTxtListBaseIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	long lKey = 0;
	lKey = SHMDataMgr::changekey(sTxtBaseData.m_strServiceContextId,m_pSHMMgrServiceTxtListBaseData->getTotal());
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrServiceTxtListBaseLock->P();

	if (!m_pSHMMgrServiceTxtListBaseIndex->get(lKey,&pValue)) {
		m_pSHMMgrServiceTxtListBaseLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int fvalue  = pValue;
		int num = 0;

		while (pValue) {

			if (strcmp(m_pMgrServiceTxtListBaseData[pValue].m_strServiceContextId,sTxtBaseData.m_strServiceContextId) == 0 ){

				if (num == 0 ) {
					int pos = m_pMgrServiceTxtListBaseData[pValue].m_iNext;

					if (pos == 0) {//该索引链表只有一个数据
						m_pSHMMgrServiceTxtListBaseIndex->revokeIdx(lKey,pValue);
					} else {
						m_pSHMMgrServiceTxtListBaseIndex->add(lKey,pos);
					}
					m_pSHMMgrServiceTxtListBaseData->erase(pValue);
					memset(&m_pMgrServiceTxtListBaseData[pValue],0,sizeof(ServiceContextListBaseData ));
					m_pSHMMgrServiceTxtListBaseLock->V();
					return 0;
				} else {
					int pos = m_pMgrServiceTxtListBaseData[pValue].m_iNext;
					m_pSHMMgrServiceTxtListBaseData->erase(pValue);
					m_pMgrServiceTxtListBaseData[fvalue].m_iNext = pos;
					m_pSHMMgrServiceTxtListBaseLock->V();
					return 0;
				}
			}
			num++;
			fvalue = pValue;
			pValue = m_pMgrServiceTxtListBaseData[pValue].m_iNext;
		}
		m_pSHMMgrServiceTxtListBaseLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	}
#else

	if (!m_pSHMMgrServiceTxtListBaseIndex->get(lKey,&pValue)) {
		m_pSHMMgrServiceTxtListBaseLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int fvalue  = pValue;
		int num = 0;

		while (pValue) {

			if (strcmp(m_pMgrServiceTxtListBaseData[pValue].m_strServiceContextId,sTxtBaseData.m_strServiceContextId) == 0 ){

				if (num == 0 ) {
					int pos = m_pMgrServiceTxtListBaseData[pValue].m_iNext;

					if (pos == 0) {//该索引链表只有一个数据
						m_pSHMMgrServiceTxtListBaseIndex->revokeIdx(lKey,pValue);
					} else {
						m_pSHMMgrServiceTxtListBaseIndex->add(lKey,pos);
					}
					m_pSHMMgrServiceTxtListBaseData->erase(pValue);
					memset(&m_pMgrServiceTxtListBaseData[pValue],0,sizeof(ServiceContextListBaseData ));
					return 0;
				} else {
					int pos = m_pMgrServiceTxtListBaseData[pValue].m_iNext;
					m_pSHMMgrServiceTxtListBaseData->erase(pValue);
					m_pMgrServiceTxtListBaseData[fvalue].m_iNext = pos;
					return 0;
				}
			}
			num++;
			fvalue = pValue;
			pValue = m_pMgrServiceTxtListBaseData[pValue].m_iNext;
		}
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	}
#endif

}

int SHMServiceTxtListBase::AddServiceTxtListBase(ServiceContextListBaseData &sTxtBaseData)
{
	if (m_pSHMMgrServiceTxtListBaseData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrServiceTxtListBaseData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrServiceTxtListBaseIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrServiceTxtListBaseIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	long lKey = 0;
	lKey = SHMDataMgr::changekey(sTxtBaseData.m_strServiceContextId,m_pSHMMgrServiceTxtListBaseData->getTotal());
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrServiceTxtListBaseLock->P();

	if (!m_pSHMMgrServiceTxtListBaseIndex->get(lKey,&pValue)) {
		int i = m_pSHMMgrServiceTxtListBaseData->malloc();

		if ( i == 0 ) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息 service_context_list_base共享内存空间不足");
			m_pSHMMgrServiceTxtListBaseLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrServiceTxtListBaseData[i],&sTxtBaseData,sizeof(ServiceContextListBaseData));
		m_pSHMMgrServiceTxtListBaseIndex->add(lKey,i);
		m_pMgrServiceTxtListBaseData[i].m_iNext = 0;
		m_pSHMMgrServiceTxtListBaseLock->V();
		return 0;
	} else {
		int fvalue = pValue;

		while (pValue) {
			if (strcmp(m_pMgrServiceTxtListBaseData[pValue].m_strServiceContextId,sTxtBaseData.m_strServiceContextId) == 0) {
				m_pSHMMgrServiceTxtListBaseLock->V();
				return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			pValue = m_pMgrServiceTxtListBaseData[pValue].m_iNext;
		}

		int i = m_pSHMMgrServiceTxtListBaseData->malloc();

		if ( i == 0 ) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息 service_context_list_base共享内存空间不足");
			m_pSHMMgrServiceTxtListBaseLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrServiceTxtListBaseData[i],&sTxtBaseData,sizeof(ServiceContextListBaseData));

		int pos = m_pMgrServiceTxtListBaseData[fvalue].m_iNext;
		m_pMgrServiceTxtListBaseData[fvalue].m_iNext = i;
		m_pMgrServiceTxtListBaseData[i].m_iNext = pos;
		m_pSHMMgrServiceTxtListBaseLock->V();
		return 0;
	}
#else

	if (!m_pSHMMgrServiceTxtListBaseIndex->get(lKey,&pValue)) {//新增数据
		int i = m_pSHMMgrServiceTxtListBaseData->malloc();

		if ( i == 0 ) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息 service_context_list_base共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrServiceTxtListBaseData[i],&sTxtBaseData,sizeof(ServiceContextListBaseData));
		m_pSHMMgrServiceTxtListBaseIndex->add(lKey,i);
		m_pMgrServiceTxtListBaseData[i].m_iNext = 0;
		m_pSHMMgrServiceTxtListBaseLock->V();
		return 0;
	} else {
		int fvalue = pValue;

		while (pValue) {
			if (strcmp(m_pMgrServiceTxtListBaseData[pValue].m_strServiceContextId,sTxtBaseData.m_strServiceContextId) == 0) {
				return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			pValue = m_pMgrServiceTxtListBaseData[pValue].m_iNext;
		}

		int i = m_pSHMMgrServiceTxtListBaseData->malloc();

		if ( i == 0 ) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息 service_context_list_base共享内存空间不足");
			m_pSHMMgrServiceTxtListBaseLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrServiceTxtListBaseData[i],&sTxtBaseData,sizeof(ServiceContextListBaseData));

		int pos = m_pMgrServiceTxtListBaseData[fvalue].m_iNext;
		m_pMgrServiceTxtListBaseData[fvalue].m_iNext = i;
		m_pMgrServiceTxtListBaseData[i].m_iNext = pos;
		return 0;
	}
#endif
}

int SHMServiceTxtListBase::updateServiceTxtListBase(ServiceContextListBaseData &sTxtBaseData)
{
	if (m_pSHMMgrServiceTxtListBaseData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrServiceTxtListBaseData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrServiceTxtListBaseIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrServiceTxtListBaseIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	long lKey = 0;
	lKey = SHMDataMgr::changekey(sTxtBaseData.m_strServiceContextId,m_pSHMMgrServiceTxtListBaseData->getTotal());
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrServiceTxtListBaseLock->P();

	if (!m_pSHMMgrServiceTxtListBaseIndex->get(lKey,&pValue)) {
		m_pSHMMgrServiceTxtListBaseLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrServiceTxtListBaseData[pValue].m_strServiceContextId,sTxtBaseData.m_strServiceContextId)==0) {
				sTxtBaseData.m_iNext = m_pMgrServiceTxtListBaseData[pValue].m_iNext;
				memcpy(&m_pMgrServiceTxtListBaseData[pValue],&sTxtBaseData,sizeof(ServiceContextListBaseData));
				m_pSHMMgrServiceTxtListBaseLock->V();
				return 0;
			}
			pValue = m_pMgrServiceTxtListBaseData[pValue].m_iNext;
		}
	}
	m_pSHMMgrServiceTxtListBaseLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else

	if (!m_pSHMMgrServiceTxtListBaseIndex->get(lKey,&pValue)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrServiceTxtListBaseData[pValue].m_strServiceContextId,sTxtBaseData.m_strServiceContextId)==0) {
				sTxtBaseData.m_iNext = m_pMgrServiceTxtListBaseData[pValue].m_iNext;
				memcpy(&m_pMgrServiceTxtListBaseData[pValue],&sTxtBaseData,sizeof(ServiceContextListBaseData));
				return 0;
			}
			pValue = m_pMgrServiceTxtListBaseData[pValue].m_iNext;
		}
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif

}

//根据主键SR_Host 查询对应的信息
int SHMLoadBalance::getLoadbalance(LoadBalanceData &sLodbalance)
{
	if (m_pSHMMgrLoadBalanceData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrLoadBalanceData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrLoadBalanceIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrLoadBalanceIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	long lKey = 0;
	lKey = SHMDataMgr::changekey(sLodbalance.SR_Host,m_pSHMMgrLoadBalanceData->getTotal());
	unsigned int pValue = 0;
#ifdef _PV_STATE_
	m_pSHMMgrLoadBalanceLock->P();

	if (!m_pSHMMgrLoadBalanceIndex->get(lKey,&pValue)) {
		m_pSHMMgrLoadBalanceLock->V();
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {

		while (pValue) {

			if (strcmp(m_pMgrLoadBalanceData[pValue].SR_Host,sLodbalance.SR_Host)==0) {
				memcpy(&sLodbalance,&m_pMgrLoadBalanceData[pValue],sizeof(LoadBalanceData));
				m_pSHMMgrLoadBalanceLock->V();
				return 0;
			}
			pValue = m_pMgrLoadBalanceData[pValue].m_iNext;
		}
	}
	m_pSHMMgrLoadBalanceLock->V();
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#else
	if (!m_pSHMMgrLoadBalanceIndex->get(lKey,&pValue)) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {

		while (pValue) {

			if (strcmp(m_pMgrLoadBalanceData[pValue].SR_Host,sLodbalance.SR_Host)==0) {
				memcpy(&sLodbalance,&m_pMgrLoadBalanceData[pValue],sizeof(LoadBalanceData));
				return 0;
			}
			pValue = m_pMgrLoadBalanceData[pValue].m_iNext;
		}
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
#endif

}
int SHMLoadBalance::getLoadbalanceAll(vector<LoadBalanceData> &vLodbalance)
{
	if (m_pSHMMgrLoadBalanceData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrLoadBalanceData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	vLodbalance.clear();
#ifdef _PV_STATE_
	m_pSHMMgrLoadBalanceLock->P();
	for (int i =1; i<=m_pSHMMgrLoadBalanceData->getCount(); i++) {
		vLodbalance.push_back(m_pMgrLoadBalanceData[i]);
	}
	m_pSHMMgrLoadBalanceLock->V();

	if (vLodbalance.size() == 0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else
	for (int i =1; i<=m_pSHMMgrLoadBalanceData->getCount(); i++) {
		vLodbalance.push_back(m_pMgrLoadBalanceData[i]);
	}

	if (vLodbalance.size() == 0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

int SHMLoadBalance::updateLoadbalance(LoadBalanceData &sLodbalance)
{
	if (m_pSHMMgrLoadBalanceData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrLoadBalanceData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrLoadBalanceIndex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}

	if (!m_pSHMMgrLoadBalanceIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	long lKey = SHMDataMgr::changekey(sLodbalance.SR_Host,m_pSHMMgrLoadBalanceData->getTotal());
	unsigned int pValue = 0;
#ifdef _DF_LOCK_
	m_pSHMMgrLoadBalanceLock->P();

	if (!m_pSHMMgrLoadBalanceIndex->get(lKey,&pValue)) {
		m_pSHMMgrLoadBalanceLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrLoadBalanceData[pValue].SR_Host,sLodbalance.SR_Host)==0) {
				sLodbalance.m_iNext = m_pMgrLoadBalanceData[pValue].m_iNext;
				memcpy(&m_pMgrLoadBalanceData[pValue],&sLodbalance,sizeof(LoadBalanceData));
				m_pSHMMgrLoadBalanceLock->V();
				return 0;
			}
			pValue = m_pMgrLoadBalanceData[pValue].m_iNext;
		}
	}
	m_pSHMMgrLoadBalanceLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else

	if (!m_pSHMMgrLoadBalanceIndex->get(lKey,&pValue)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrLoadBalanceData[pValue].SR_Host,sLodbalance.SR_Host)==0) {
				sLodbalance.m_iNext = m_pMgrLoadBalanceData[pValue].m_iNext;
				memcpy(&m_pMgrLoadBalanceData[pValue],&sLodbalance,sizeof(LoadBalanceData));
				return 0;
			}
			pValue = m_pMgrLoadBalanceData[pValue].m_iNext;
		}
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif

}

int SHMBaseMethod::getBaseMethod(int m_iBaseMethodID, CBaseMethod & sBaseMethod)
{
	if (m_pSHMMgrBaseMethodData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrBaseMethodData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrBaseMethodindex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}
	if (!m_pSHMMgrBaseMethodindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int value = 0 ;
#ifdef _PV_STATE_
	m_pSHMMgrBaseMethodLock->P();
	if (!m_pSHMMgrBaseMethodindex->get(m_iBaseMethodID,&value)) {
		m_pSHMMgrBaseMethodLock->V();
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		memcpy(&sBaseMethod,&m_pMgrBaseMethodData[value],sizeof(CBaseMethod));
	}
	m_pSHMMgrBaseMethodLock->V();
	return 0;
#else
	if (!m_pSHMMgrBaseMethodindex->get(m_iBaseMethodID,&value)) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		memcpy(&sBaseMethod,&m_pMgrBaseMethodData[value],sizeof(CBaseMethod));
	}
	return 0;
#endif

}

int SHMBaseMethod::getBaseMethodAll(vector<CBaseMethod> & vBaseMethod)
{
	if (m_pSHMMgrBaseMethodData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrBaseMethodData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrBaseMethodindex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}
	if (!m_pSHMMgrBaseMethodindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	vBaseMethod.clear();
	int pos = 0;
#ifdef _PV_STATE_
	m_pSHMMgrBaseMethodLock->P();
	for (int i = 1;i<=m_pSHMMgrBaseMethodData->getCount(); ) {

		if (m_pSHMMgrBaseMethodData->m_piIdleNext[pos]!=-1) {
			if ((pos++)>=m_pSHMMgrBaseMethodData->getTotal()) {
				break;
			}
			continue;
		}
		vBaseMethod.push_back(m_pMgrBaseMethodData[pos]);
		pos++;
		i++;
	}
	m_pSHMMgrBaseMethodLock->V();
	if (vBaseMethod.size() == 0 ) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else
	for (int i = 1;i<=m_pSHMMgrBaseMethodData->getCount(); ) {

		if (m_pSHMMgrBaseMethodData->m_piIdleNext[pos]!=-1) {
			if ((pos++)>=m_pSHMMgrBaseMethodData->getTotal()) {
				break;
			}
			continue;
		}
		vBaseMethod.push_back(m_pMgrBaseMethodData[pos]);
		pos++;
		i++;
	}
	if (vBaseMethod.size() == 0 ) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

int SHMBaseMethod::updateBaseMethod(const CBaseMethod & sBaseMethod)
{
	if (m_pSHMMgrBaseMethodData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrBaseMethodData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrBaseMethodindex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}
	if (!m_pSHMMgrBaseMethodindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int value = 0;
#ifdef _DF_LOCK_
	m_pSHMMgrBaseMethodLock->P();
	if (!m_pSHMMgrBaseMethodindex->get(sBaseMethod.m_iBaseMethodID,&value)) {
		m_pSHMMgrBaseMethodLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else{
		memcpy(&m_pMgrBaseMethodData[value],&sBaseMethod,sizeof(CBaseMethod));
	}
	m_pSHMMgrBaseMethodLock->V();
	return 0;
#else
	if (!m_pSHMMgrBaseMethodindex->get(sBaseMethod.m_iBaseMethodID,&value)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else{
		memcpy(&m_pMgrBaseMethodData[value],&sBaseMethod,sizeof(CBaseMethod));
	}
	return 0;
#endif

}

int SHMBaseMethod::deleteBaseMethod(const CBaseMethod & sBaseMethod)
{
	if (m_pSHMMgrBaseMethodData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrBaseMethodData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrBaseMethodindex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}
	if (!m_pSHMMgrBaseMethodindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int value = 0;
#ifdef _DF_LOCK_
	m_pSHMMgrBaseMethodLock->P();
	if (!m_pSHMMgrBaseMethodindex->get(sBaseMethod.m_iBaseMethodID,&value)) {
		m_pSHMMgrBaseMethodLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else{
		if (m_pSHMMgrBaseMethodindex->revokeIdx(sBaseMethod.m_iBaseMethodID,value)) {
			m_pSHMMgrBaseMethodData->erase(value);
			memset(&m_pMgrBaseMethodData[value],0,sizeof(CBaseMethod));
		} else {
			m_pSHMMgrBaseMethodLock->V();
			return MBC_SHM_ERRNO_DEL_INDEX_FAILED;
		}

	}
	m_pSHMMgrBaseMethodLock->V();
	return 0;
#else
	if (!m_pSHMMgrBaseMethodindex->get(sBaseMethod.m_iBaseMethodID,&value)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else{
		if (m_pSHMMgrBaseMethodindex->revokeIdx(sBaseMethod.m_iBaseMethodID,value)) {
			m_pSHMMgrBaseMethodData->erase(value);
			memset(&m_pMgrBaseMethodData[value],0,sizeof(CBaseMethod));
		} else {
			return MBC_SHM_ERRNO_DEL_INDEX_FAILED;
		}

	}
	return 0;
#endif

}

int SHMBaseMethod::addBaseMethod(const CBaseMethod & sBaseMethod)
{
	if (m_pSHMMgrBaseMethodData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrBaseMethodData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	if (m_pSHMMgrBaseMethodindex == NULL ) {
		return MBC_SHM_ERRNO_INDEX_NOTCONNECT;
	}
	if (!m_pSHMMgrBaseMethodindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int value = 0;
#ifdef _DF_LOCK_
	m_pSHMMgrBaseMethodLock->P();
	if (!m_pSHMMgrBaseMethodindex->get(sBaseMethod.m_iBaseMethodID,&value)) {
		int i = m_pSHMMgrBaseMethodData->malloc();

		if ( i == 0) {
			m_pSHMMgrBaseMethodLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		m_pSHMMgrBaseMethodindex->add(sBaseMethod.m_iBaseMethodID,i);
		memcpy(&m_pMgrBaseMethodData[i],&sBaseMethod,sizeof(CBaseMethod));
	} else{
		m_pSHMMgrBaseMethodLock->V();
		return MBC_SHM_ADD_DATA_EXIST;
	}
	m_pSHMMgrBaseMethodLock->V();
	return 0;
#else
	if (!m_pSHMMgrBaseMethodindex->get(sBaseMethod.m_iBaseMethodID,&value)) {
		int i = m_pSHMMgrBaseMethodData->malloc();

		if ( i == 0) {
			return MBC_MEM_SPACE_SMALL;
		}
		m_pSHMMgrBaseMethodindex->add(sBaseMethod.m_iBaseMethodID,i);
		memcpy(&m_pMgrBaseMethodData[i],&sBaseMethod,sizeof(CBaseMethod));
	} else{
		return MBC_SHM_ADD_DATA_EXIST;
	}
	return 0;
#endif

}

int SHMPacketSession::addPacketSession(const SPacketSession &sPackageSession)
{
	if (!m_pSHMMgrPacketSessionData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrPacketSessionindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	if (!m_pSHMMgrPacketSubSessionindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	long lkey = SHMDataMgr::changekey(changeSessionID(sPackageSession.m_szSubSessionID),m_pSHMMgrPacketSessionData->getTotal());
	if (lkey == -1) return 0;

	unsigned int pValue = 0;
	int i = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrPacketSessionLock->P();

	if (m_pSHMMgrPacketSubSessionindex->get(lkey,&pValue)) {
		int fValue = pValue;

		while (pValue) {
			if (strcmp(m_pMgrPacketSessionData[pValue].m_szSubSessionID,sPackageSession.m_szSubSessionID) == 0) {
				m_pSHMMgrPacketSessionLock->V();
				return MBC_SHM_ADD_DATA_EXIST;
			}
			pValue = m_pMgrPacketSessionData[pValue].m_iNext;
		}
		i = m_pSHMMgrPacketSessionData->malloc();

		if (i == 0 ) {
			m_pSHMMgrPacketSessionLock->V();
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrPacketSessionData[i],&sPackageSession,sizeof(SPacketSession));
		int tmppos = m_pMgrPacketSessionData[fValue].m_iNext;

		m_pMgrPacketSessionData[fValue].m_iNext = i;
		m_pMgrPacketSessionData[i].m_iNext = tmppos;
	} else {
		i = m_pSHMMgrPacketSessionData->malloc();

		if (i == 0) {
			m_pSHMMgrPacketSessionLock->V();
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrPacketSessionData[i],&sPackageSession,sizeof(SPacketSession));
		m_pSHMMgrPacketSubSessionindex->add (lkey,i);
		m_pMgrPacketSessionData[i].m_iNext = 0;
	}

	lkey = SHMDataMgr::changekey(changeSessionID(sPackageSession.m_szSessionID),m_pSHMMgrPacketSessionData->getTotal());
	if (lkey == -1) {
		m_pSHMMgrPacketSessionLock->V();
		return 0;
	}

	if (m_pSHMMgrPacketSessionindex->get(lkey,&pValue)) {
		if (strlen(m_pMgrPacketSessionData[pValue].m_szSessionID) == 0 ){
			m_pSHMMgrPacketSessionindex->add (lkey,i);
			m_pMgrPacketSessionData[i].m_iSubNext = 0;
		} else {
			int tmppos = m_pMgrPacketSessionData[pValue].m_iSubNext;
			m_pMgrPacketSessionData[pValue].m_iSubNext = i;
			m_pMgrPacketSessionData[i].m_iSubNext = tmppos;
		}
	} else {
		m_pSHMMgrPacketSessionindex->add (lkey,i);
		m_pMgrPacketSessionData[i].m_iSubNext = 0;
	}
	m_pSHMMgrPacketSessionLock->V();
	return 0;
#else
	if (m_pSHMMgrPacketSubSessionindex->get(lkey,&pValue)) {
		int fValue = pValue;

		while (pValue) {
			if (strcmp(m_pMgrPacketSessionData[pValue].m_szSubSessionID,sPackageSession.m_szSubSessionID) == 0) {
				return MBC_SHM_ADD_DATA_EXIST;
			}
			pValue = m_pMgrPacketSessionData[pValue].m_iNext;
		}
		i = m_pSHMMgrPacketSessionData->malloc();

		if (i == 0 ) {
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrPacketSessionData[i],&sPackageSession,sizeof(SPacketSession));
		int tmppos = m_pMgrPacketSessionData[fValue].m_iNext;

		m_pMgrPacketSessionData[fValue].m_iNext = i;
		m_pMgrPacketSessionData[i].m_iNext = tmppos;

	} else {
		i = m_pSHMMgrPacketSessionData->malloc();

		if (i == 0) {
			return MBC_MEM_SPACE_SMALL;
		}

		memcpy(&m_pMgrPacketSessionData[i],&sPackageSession,sizeof(SPacketSession));
		m_pSHMMgrPacketSubSessionindex->add (lkey,i);
		m_pMgrPacketSessionData[i].m_iNext = 0;
	}

	lkey = SHMDataMgr::changekey(changeSessionID(sPackageSession.m_szSessionID),m_pSHMMgrPacketSessionData->getTotal());
	if (lkey == -1) {
		return 0;
	}
	if (m_pSHMMgrPacketSessionindex->get(lkey,&pValue)) {
		if (strlen(m_pMgrPacketSessionData[pValue].m_szSessionID) == 0 ){
			m_pSHMMgrPacketSessionindex->add (lkey,i);
			m_pMgrPacketSessionData[i].m_iSubNext = 0;
		} else {
			int tmppos = m_pMgrPacketSessionData[pValue].m_iSubNext;
			m_pMgrPacketSessionData[pValue].m_iSubNext = i;
			m_pMgrPacketSessionData[i].m_iSubNext = tmppos;
		}
	} else {
		m_pSHMMgrPacketSessionindex->add (lkey,i);
		m_pMgrPacketSessionData[i].m_iSubNext = 0;
	}
	return 0;
#endif

}

int SHMPacketSession::getPacketSessionAll(const char * psSessionID, vector<SPacketSession> &sPackageSession)
{
	if (!m_pSHMMgrPacketSessionData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrPacketSessionindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	sPackageSession.clear();
	long lkey = SHMDataMgr::changekey(changeSessionID(psSessionID),m_pSHMMgrPacketSessionData->getTotal());
	if (lkey == -1) return 0;
	unsigned int pValue = 0;
#ifdef _PV_STATE_
	m_pSHMMgrPacketSessionLock->P();

	if (m_pSHMMgrPacketSessionindex->get(lkey,&pValue)) {
		while (pValue) {
			if (strcmp(m_pMgrPacketSessionData[pValue].m_szSessionID,psSessionID) == 0) {
				sPackageSession.push_back(m_pMgrPacketSessionData[pValue]);
			}
			pValue = m_pMgrPacketSessionData[pValue].m_iSubNext;
		}
	}
	m_pSHMMgrPacketSessionLock->V();
	if (sPackageSession.size() == 0 ) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else
	if (m_pSHMMgrPacketSessionindex->get(lkey,&pValue)) {
		while (pValue) {
			if (strcmp(m_pMgrPacketSessionData[pValue].m_szSessionID,psSessionID) == 0) {
				sPackageSession.push_back(m_pMgrPacketSessionData[pValue]);
			}
			pValue = m_pMgrPacketSessionData[pValue].m_iSubNext;
		}
	}
	if (sPackageSession.size() == 0 ) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

int SHMPacketSession::deletePacketSession(const SPacketSession &sPackageSession)
{
	if (!m_pSHMMgrPacketSessionData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrPacketSessionindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	if (!m_pSHMMgrPacketSubSessionindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	vector<SPacketSession> _vTemp;
	_vTemp.clear();
	long lkey = SHMDataMgr::changekey(changeSessionID(sPackageSession.m_szSessionID),m_pSHMMgrPacketSessionData->getTotal());
	if (lkey == -1) return 0;

	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrPacketSessionLock->P();

	if (!m_pSHMMgrPacketSessionindex->get(lkey,&pValue)) {
		m_pSHMMgrPacketSessionLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (strcmp(m_pMgrPacketSessionData[pValue].m_szSessionID, sPackageSession.m_szSessionID)==0 ) {
				if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
					int pos = m_pMgrPacketSessionData[pValue].m_iSubNext;
					if (pos !=0) {
						lkey = SHMDataMgr::changekey(changeSessionID(m_pMgrPacketSessionData[pos].m_szSessionID),m_pSHMMgrPacketSessionData->getTotal());
						m_pSHMMgrPacketSessionindex->add(lkey,pos);//重置索引
						m_pSHMMgrPacketSessionData->erase(pValue);
						_vTemp.push_back(m_pMgrPacketSessionData[pValue]);
						//memset(&m_pMgrPacketSessionData[pValue],0,sizeof(SPacketSession));
						pValue = pos;
						fVale = 0;
						continue;
					} else {
						m_pSHMMgrPacketSessionindex->revokeIdx(lkey,pValue);
						m_pSHMMgrPacketSessionData->erase(pValue);
						_vTemp.push_back(m_pMgrPacketSessionData[pValue]);
						//memset(&m_pMgrPacketSessionData[pValue],0,sizeof(SPacketSession));
					}
				} else {
					int pos = m_pMgrPacketSessionData[pValue].m_iSubNext;
					if (pos !=0) {
						m_pMgrPacketSessionData[fVale].m_iSubNext = pos;
						m_pSHMMgrPacketSessionData->erase(pValue);
						_vTemp.push_back(m_pMgrPacketSessionData[pValue]);
						//memset(&m_pMgrPacketSessionData[pValue],0,sizeof(SPacketSession));
						pValue = pos;
						continue;
					}else {
						m_pMgrPacketSessionData[fVale].m_iSubNext = 0;
						m_pSHMMgrPacketSessionData->erase(pValue);
						_vTemp.push_back(m_pMgrPacketSessionData[pValue]);
						//memset(&m_pMgrPacketSessionData[pValue],0,sizeof(SPacketSession));
					}
				}
			}
			fVale = pValue;
			pValue = m_pMgrPacketSessionData[pValue].m_iSubNext;
			_iCount++;
		}

		//删除子会话
		for (int i =0; i<_vTemp.size(); i++) {
			lkey = SHMDataMgr::changekey(changeSessionID(_vTemp[i].m_szSubSessionID),m_pSHMMgrPacketSessionData->getTotal());

			if (m_pSHMMgrPacketSubSessionindex->get(lkey,&pValue)) {
				int _iCount = 0;
				int fVale = 0;

				while (pValue) {
					if (strcmp(m_pMgrPacketSessionData[pValue].m_szSubSessionID, _vTemp[i].m_szSubSessionID)==0 ) {
						if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
							int pos = m_pMgrPacketSessionData[pValue].m_iNext;

							if (pos !=0) {
								lkey = SHMDataMgr::changekey(changeSessionID(m_pMgrPacketSessionData[pos].m_szSubSessionID),m_pSHMMgrPacketSessionData->getTotal());
								m_pSHMMgrPacketSubSessionindex->add(lkey,pos);
							} else {
								m_pSHMMgrPacketSubSessionindex->revokeIdx(lkey,pValue);
							}
						} else {
							int pos = m_pMgrPacketSessionData[pValue].m_iNext;
							m_pMgrPacketSessionData[fVale].m_iNext = pos;
						}
						memset(&m_pMgrPacketSessionData[pValue],0,sizeof(SPacketSession));
						break;
					}
					fVale = pValue;
					pValue = m_pMgrPacketSessionData[pValue].m_iNext;
					_iCount++;
				}
			}
		}
		m_pSHMMgrPacketSessionLock->V();
		return 0;
	}
	m_pSHMMgrPacketSessionLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else

	if (!m_pSHMMgrPacketSessionindex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		int _iCount = 0;
		int fVale = 0;

		while (pValue) {

			if (strcmp(m_pMgrPacketSessionData[pValue].m_szSessionID, sPackageSession.m_szSessionID)==0 ) {
				if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
					int pos = m_pMgrPacketSessionData[pValue].m_iSubNext;
					if (pos !=0) {
						lkey = SHMDataMgr::changekey(changeSessionID(m_pMgrPacketSessionData[pos].m_szSessionID),m_pSHMMgrPacketSessionData->getTotal());
						m_pSHMMgrPacketSessionindex->add(lkey,pos);//重置索引
						m_pSHMMgrPacketSessionData->erase(pValue);
						_vTemp.push_back(m_pMgrPacketSessionData[pValue]);
						//memset(&m_pMgrPacketSessionData[pValue],0,sizeof(SPacketSession));
						pValue = pos;
						fVale = 0;
						continue;
					} else {
						m_pSHMMgrPacketSessionindex->revokeIdx(lkey,pValue);
						m_pSHMMgrPacketSessionData->erase(pValue);
						_vTemp.push_back(m_pMgrPacketSessionData[pValue]);
						//memset(&m_pMgrPacketSessionData[pValue],0,sizeof(SPacketSession));
					}
				} else {
					int pos = m_pMgrPacketSessionData[pValue].m_iSubNext;
					if (pos !=0) {
						m_pMgrPacketSessionData[fVale].m_iSubNext = pos;
						m_pSHMMgrPacketSessionData->erase(pValue);
						_vTemp.push_back(m_pMgrPacketSessionData[pValue]);
						//memset(&m_pMgrPacketSessionData[pValue],0,sizeof(SPacketSession));
						pValue = pos;
						continue;
					}else {
						m_pMgrPacketSessionData[fVale].m_iSubNext = 0;
						m_pSHMMgrPacketSessionData->erase(pValue);
						_vTemp.push_back(m_pMgrPacketSessionData[pValue]);
						//memset(&m_pMgrPacketSessionData[pValue],0,sizeof(SPacketSession));
					}
				}
			}
			fVale = pValue;
			pValue = m_pMgrPacketSessionData[pValue].m_iSubNext;
			_iCount++;
		}

		//删除子会话
		for (int i =0; i<_vTemp.size(); i++) {
			lkey = SHMDataMgr::changekey(changeSessionID(_vTemp[i].m_szSubSessionID),m_pSHMMgrPacketSessionData->getTotal());

			if (m_pSHMMgrPacketSubSessionindex->get(lkey,&pValue)) {
				int _iCount = 0;
				int fVale = 0;

				while (pValue) {
					if (strcmp(m_pMgrPacketSessionData[pValue].m_szSubSessionID, _vTemp[i].m_szSubSessionID)==0 ) {
						if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
							int pos = m_pMgrPacketSessionData[pValue].m_iNext;

							if (pos !=0) {
								lkey = SHMDataMgr::changekey(changeSessionID(m_pMgrPacketSessionData[pos].m_szSubSessionID),m_pSHMMgrPacketSessionData->getTotal());
								m_pSHMMgrPacketSubSessionindex->add(lkey,pos);
							} else {
								m_pSHMMgrPacketSubSessionindex->revokeIdx(lkey,pValue);
							}
						} else {
							int pos = m_pMgrPacketSessionData[pValue].m_iNext;
							m_pMgrPacketSessionData[fVale].m_iNext = pos;
						}
						memset(&m_pMgrPacketSessionData[pValue],0,sizeof(SPacketSession));
						break;
					}
					fVale = pValue;
					pValue = m_pMgrPacketSessionData[pValue].m_iNext;
					_iCount++;
				}
			}
		}
		return 0;
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif

}

int SHMPacketSession::updatePacketSession(const SPacketSession &sPackageSession)
{
	if (!m_pSHMMgrPacketSessionData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrPacketSessionindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	if (!m_pSHMMgrPacketSubSessionindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	char sKey[8] = {0};
	long lkey = SHMDataMgr::changekey(changeSessionID(sPackageSession.m_szSubSessionID),m_pSHMMgrPacketSessionData->getTotal());
	if (lkey == -1) return 0;
	unsigned int pValue = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrPacketSessionLock->P();

	if (!m_pSHMMgrPacketSubSessionindex->get(lkey,&pValue)) {
		m_pSHMMgrPacketSessionLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrPacketSessionData[pValue].m_szSubSessionID, sPackageSession.m_szSubSessionID)==0 ) {
				int _iNext = m_pMgrPacketSessionData[pValue].m_iNext;
				int _iSubNext = m_pMgrPacketSessionData[pValue].m_iSubNext;
				memcpy(&m_pMgrPacketSessionData[pValue],&sPackageSession,sizeof(SPacketSession));
				m_pMgrPacketSessionData[pValue].m_iNext = _iNext;
				m_pMgrPacketSessionData[pValue].m_iSubNext = _iSubNext;
				m_pSHMMgrPacketSessionLock->V();
				return 0;
			}
			pValue = m_pMgrPacketSessionData[pValue].m_iNext;
		}
	}
	m_pSHMMgrPacketSessionLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else

	if (!m_pSHMMgrPacketSubSessionindex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrPacketSessionData[pValue].m_szSubSessionID, sPackageSession.m_szSubSessionID)==0 ) {
				int _iNext = m_pMgrPacketSessionData[pValue].m_iNext;
				int _iSubNext = m_pMgrPacketSessionData[pValue].m_iSubNext;
				memcpy(&m_pMgrPacketSessionData[pValue],&sPackageSession,sizeof(SPacketSession));
				m_pMgrPacketSessionData[pValue].m_iNext = _iNext;
				m_pMgrPacketSessionData[pValue].m_iSubNext = _iSubNext;
				return 0;
			}
			pValue = m_pMgrPacketSessionData[pValue].m_iNext;
		}
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif

}

int SHMPacketSession::getPacketSession(const char * psChildSessionID, SPacketSession &sPackageSession)
{
	if (!m_pSHMMgrPacketSessionData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrPacketSessionindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	if (!m_pSHMMgrPacketSubSessionindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	long lkey = SHMDataMgr::changekey(changeSessionID(psChildSessionID),m_pSHMMgrPacketSessionData->getTotal());
	unsigned int pValue = 0;
#ifdef _PV_STATE_
	m_pSHMMgrPacketSessionLock->P();

	if (!m_pSHMMgrPacketSubSessionindex->get(lkey,&pValue)) {
		m_pSHMMgrPacketSessionLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrPacketSessionData[pValue].m_szSubSessionID,psChildSessionID)==0 ) {
				memcpy(&sPackageSession,&m_pMgrPacketSessionData[pValue],sizeof(SPacketSession));
				m_pSHMMgrPacketSessionLock->V();
				return 0;
			}
			pValue = m_pMgrPacketSessionData[pValue].m_iNext;
		}
	}
	m_pSHMMgrPacketSessionLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else
	if (!m_pSHMMgrPacketSubSessionindex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else {
		while (pValue) {

			if (strcmp(m_pMgrPacketSessionData[pValue].m_szSubSessionID,psChildSessionID)==0 ) {
				memcpy(&sPackageSession,&m_pMgrPacketSessionData[pValue],sizeof(SPacketSession));
				return 0;
			}
			pValue = m_pMgrPacketSessionData[pValue].m_iNext;
		}
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif

}

int SHMServicePackage::deleteServicePackageNode(const ServicePackageNode & sServicePackageNode)
{
	int iRet = 0;
	unsigned int value = 0;

#ifndef _DF_LOCK_
	m_pSHMMgrServicePackageLock->P();

	if (m_pSHMMgrSrvPackageNodeSeqIndex->get(sServicePackageNode.m_iNodeSeq,&value)) {
		long lkey = SHMDataMgr::changekey(m_pMgrServicePackageData[value].m_strServiceContextId,m_pSHMMgrServicePackageData->getTotal());
		unsigned int temp = 0;

		if (m_pSHMMgrServicePackageindex->get(lkey,&temp)) {
			unsigned int fvalue =temp;
			int num = 0;

			while (temp) {
				if (strcmp(m_pMgrServicePackageData[value].m_strServiceContextId,
					m_pMgrServicePackageData[temp].m_strServiceContextId)==0) {

						if (num == 0) {
							int pos = m_pMgrServicePackageData[temp].m_iNext;

							if (pos == 0) {
								m_pSHMMgrServicePackageindex->revokeIdx(lkey,temp);
							} else {
								m_pSHMMgrServicePackageindex->add(lkey,pos);
							}
							break;
						} else {
							int pos = m_pMgrServicePackageData[temp].m_iNext;
							m_pMgrServicePackageData[fvalue].m_iNext = pos;
							break;
						}
				}
				num++;
				fvalue = temp;
				temp = m_pMgrServicePackageData[temp].m_iNext;
			}
		}
		m_pSHMMgrSrvPackageNodeSeqIndex->revokeIdx(sServicePackageNode.m_iNodeSeq,value);
		m_pSHMMgrServicePackageData->erase(value);
		memset(&m_pMgrServicePackageData[value],0,sizeof(ServicePackageNode));
		iRet = 0;
	} else {
		iRet = MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	}
	m_pSHMMgrServicePackageLock->V();
	return iRet;
#else

	if (m_pSHMMgrSrvPackageNodeSeqIndex->get(sServicePackageNode.m_iNodeSeq,&value)) {
		long lkey = SHMDataMgr::changekey(m_pMgrServicePackageData[value].m_strServiceContextId,m_pSHMMgrServicePackageData->getTotal());
		unsigned int temp = 0;

		if (m_pSHMMgrServicePackageindex->get(lkey,&temp)) {
			unsigned int fvalue =temp;
			int num = 0;

			while (temp) {
				if (strcmp(m_pMgrServicePackageData[value].m_strServiceContextId,
					m_pMgrServicePackageData[temp].m_strServiceContextId)==0) {

						if (num == 0) {
							int pos = m_pMgrServicePackageData[temp].m_iNext;

							if (pos == 0) {
								m_pSHMMgrServicePackageindex->revokeIdx(lkey,temp);
							} else {
								m_pSHMMgrServicePackageindex->add(lkey,pos);
							}
							break;
						} else {
							int pos = m_pMgrServicePackageData[temp].m_iNext;
							m_pMgrServicePackageData[fvalue].m_iNext = pos;
							break;
						}
				}
				num++;
				fvalue = temp;
				temp = m_pMgrServicePackageData[temp].m_iNext;
			}
		}
		m_pSHMMgrSrvPackageNodeSeqIndex->revokeIdx(sServicePackageNode.m_iNodeSeq,value);
		m_pSHMMgrServicePackageData->erase(value);
		memset(&m_pMgrServicePackageData[value],0,sizeof(ServicePackageNode));
		iRet = 0;
	} else {
		iRet = MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	}
	return iRet;
#endif
}

int SHMServicePackage::updateServicePackageNode(const ServicePackageNode & sServicePackageNode)
{
	int iRet = 0;
	unsigned int value  = 0;

	if (m_pSHMMgrSrvPackageNodeSeqIndex->get(sServicePackageNode.m_iNodeSeq,&value)) {
		iRet = deleteServicePackageNode(m_pMgrServicePackageData[value]);
		if(iRet!=0) {
			return iRet;
		}
		iRet = addServicePackageNode(sServicePackageNode);
	} else {
		iRet = MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	}
	return iRet;
}

int SHMServicePackage::addServicePackageNode(const ServicePackageNode & sServicePackageNode)
{
	int iRet = 0;
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrServicePackageLock->P();

	if (!m_pSHMMgrSrvPackageNodeSeqIndex->get(sServicePackageNode.m_iNodeSeq,&value)) {
		int i = m_pSHMMgrServicePackageData->malloc();
		if (i == 0){
			m_pSHMMgrServicePackageLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrServicePackageData[i],&sServicePackageNode,sizeof(ServicePackageNode));
		long lkey = SHMDataMgr::changekey(m_pMgrServicePackageData[i].m_strServiceContextId,m_pSHMMgrServicePackageData->getTotal());
		unsigned int temp = 0;

		if (m_pSHMMgrServicePackageindex->get(lkey,&temp)) {
			int pos = m_pMgrServicePackageData[temp].m_iNext;
			m_pMgrServicePackageData[i].m_iNext = pos;
			m_pMgrServicePackageData[temp].m_iNext = i;
		} else {
			m_pSHMMgrServicePackageindex->add(lkey,i);
			m_pMgrServicePackageData[i].m_iNext = 0;
		}
		m_pSHMMgrSrvPackageNodeSeqIndex->add(sServicePackageNode.m_iNodeSeq,i);
		iRet = 0;
	} else {
		iRet = MBC_SHM_ERRNO_ADD_DATA_EXIST;
	}
	m_pSHMMgrServicePackageLock->V();
	return iRet;
#else

	if (!m_pSHMMgrSrvPackageNodeSeqIndex->get(sServicePackageNode.m_iNodeSeq,&value)) {
		int i = m_pSHMMgrServicePackageData->malloc();

		if (i == 0){
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrServicePackageData[i],&sServicePackageNode,sizeof(ServicePackageNode));
		long lkey = SHMDataMgr::changekey(m_pMgrServicePackageData[i].m_strServiceContextId,m_pSHMMgrServicePackageData->getTotal());
		unsigned int temp = 0;

		if (m_pSHMMgrServicePackageindex->get(lkey,&temp)) {
			int pos = m_pMgrServicePackageData[temp].m_iNext;
			m_pMgrServicePackageData[i].m_iNext = pos;
			m_pMgrServicePackageData[temp].m_iNext = i;
		} else {
			m_pSHMMgrServicePackageindex->add(lkey,i);
			m_pMgrServicePackageData[i].m_iNext = 0;
		}
		m_pSHMMgrSrvPackageNodeSeqIndex->add(sServicePackageNode.m_iNodeSeq,i);
		iRet = 0;
	} else {
		iRet = MBC_SHM_ERRNO_ADD_DATA_EXIST;
	}
	return iRet;
#endif
}

int SHMServicePackage::getServicePackageNode(int iNodeSeq, ServicePackageNode & vServicePackageNode)
{
	if (!m_pSHMMgrServicePackageData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrSrvPackageNodeSeqIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int value = 0;
	int iRet = 0;
#ifdef _PV_STATE_
	m_pSHMMgrServicePackageLock->P();
	if (m_pSHMMgrSrvPackageNodeSeqIndex->get(iNodeSeq,&value)) {
		memcpy(&vServicePackageNode,&m_pMgrServicePackageData[value],sizeof(ServicePackageNode));
		iRet = 0;
	} else {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	m_pSHMMgrServicePackageLock->V();
	return iRet;
#else
	if (m_pSHMMgrSrvPackageNodeSeqIndex->get(iNodeSeq,&value)) {
		memcpy(&vServicePackageNode,&m_pMgrServicePackageData[value],sizeof(ServicePackageNode));
		iRet = 0;
	} else {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return iRet;
#endif

}

int SHMServicePackage::getServicePackageNodeALL(const char * psServiceContextId, vector<ServicePackageNode> & vServicePackageNode)
{
	if (!m_pSHMMgrServicePackageData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrServicePackageindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	vServicePackageNode.clear();
	long lkey= 0;
	int iRet = 0;
	lkey = SHMDataMgr::changekey(psServiceContextId,m_pSHMMgrServicePackageData->getTotal());
	unsigned int pValue = 0;
#ifdef _PV_STATE_
	m_pSHMMgrServicePackageLock->P();

	if (!m_pSHMMgrServicePackageindex->get(lkey,&pValue)) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		while (pValue) {
			if (strcmp(m_pMgrServicePackageData[pValue].m_strServiceContextId,psServiceContextId)==0) {
				vServicePackageNode.push_back(m_pMgrServicePackageData[pValue]);
			}
			pValue = m_pMgrServicePackageData[pValue].m_iNext;
		}
	}
	if (vServicePackageNode.size() == 0 ){
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	m_pSHMMgrServicePackageLock->V();
	return iRet;
#else
	if (!m_pSHMMgrServicePackageindex->get(lkey,&pValue)) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		while (pValue) {
			if (strcmp(m_pMgrServicePackageData[pValue].m_strServiceContextId,psServiceContextId)==0) {
				vServicePackageNode.push_back(m_pMgrServicePackageData[pValue]);
			}
			pValue = m_pMgrServicePackageData[pValue].m_iNext;
		}
	}
	if (vServicePackageNode.size() == 0 ){
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return iRet;
#endif

}

int SHMServicePackageRoute::updateServicePackageRoute(const ServicePackageRoute & sServicePackageRoute)
{
	unsigned int value = 0;
	int iRet = 0;

	if (m_pSHMMgrSrvPackageRouteSeqIndex->get(sServicePackageRoute.m_iRouteSeq,&value)) {
		iRet = deleteServicePackageRoute(m_pMgrServicePackageRouteData[value]);
		if (iRet!=0) {
			return iRet;
		}
		iRet = addServicePackageRoute(sServicePackageRoute);
	}
	return iRet;
}

int SHMServicePackageRoute::addServicePackageRoute(const ServicePackageRoute & sServicePackageRoute)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	if (!m_pSHMMgrSrvPackageRouteSeqIndex->get(sServicePackageRoute.m_iRouteSeq,&value)) {
		int i = m_pSHMMgrServicePackageRouteData->malloc();

		if (i == 0)  {
			m_pSHMMgrServicePackageRouteLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrServicePackageRouteData[i],&sServicePackageRoute,sizeof(ServicePackageRoute));
		long lkey = SHMDataMgr::changekey(m_pMgrServicePackageRouteData[value].m_strServicePackageId,m_pSHMMgrServicePackageRouteData->getTotal());
		unsigned int temp = 0;

		if (m_pSHMMgrServicePackageRouteindex->get(lkey,&temp)) {
			int pos = m_pMgrServicePackageRouteData[temp].m_iNext;
			m_pMgrServicePackageRouteData[i].m_iNext = pos;
			m_pMgrServicePackageRouteData[temp].m_iNext = i;
		} else {
			m_pSHMMgrServicePackageRouteindex->add(lkey,i);
			m_pMgrServicePackageRouteData[i].m_iNext = 0;
		}
		m_pSHMMgrSrvPackageRouteSeqIndex->add(sServicePackageRoute.m_iRouteSeq,i);
		m_pSHMMgrServicePackageRouteLock->V();
		return 0;
	}
	m_pSHMMgrServicePackageRouteLock->V();
	return MBC_SHM_ADD_DATA_EXIST;
#else

	if (!m_pSHMMgrSrvPackageRouteSeqIndex->get(sServicePackageRoute.m_iRouteSeq,&value)) {
		int i = m_pSHMMgrServicePackageRouteData->malloc();
		if (i == 0)  {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrServicePackageRouteData[i],&sServicePackageRoute,sizeof(ServicePackageRoute));
		long lkey = SHMDataMgr::changekey(m_pMgrServicePackageRouteData[value].m_strServicePackageId,m_pSHMMgrServicePackageRouteData->getTotal());
		unsigned int temp = 0;

		if (m_pSHMMgrServicePackageRouteindex->get(lkey,&temp)) {
			int pos = m_pMgrServicePackageRouteData[temp].m_iNext;
			m_pMgrServicePackageRouteData[i].m_iNext = pos;
			m_pMgrServicePackageRouteData[temp].m_iNext = i;
		} else {
			m_pSHMMgrServicePackageRouteindex->add(lkey,i);
			m_pMgrServicePackageRouteData[i].m_iNext = 0;
		}
		m_pSHMMgrSrvPackageRouteSeqIndex->add(sServicePackageRoute.m_iRouteSeq,i);
		return 0;
	}
	return MBC_SHM_ADD_DATA_EXIST;
#endif
}

int SHMServicePackageRoute::deleteServicePackageRoute(const ServicePackageRoute & sServicePackageRoute)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	if (!m_pSHMMgrSrvPackageRouteSeqIndex->get(sServicePackageRoute.m_iRouteSeq,&value)) {
		m_pSHMMgrServicePackageRouteLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		long lkey = SHMDataMgr::changekey(m_pMgrServicePackageRouteData[value].m_strServicePackageId,m_pSHMMgrServicePackageRouteData->getTotal());
		unsigned int temp =0;

		if (m_pSHMMgrServicePackageRouteindex->get(lkey,&temp)) {
			int favlue =temp;
			int num = 0;

			while (temp) {
				if (strcmp(m_pMgrServicePackageRouteData[temp].m_strServicePackageId,
					m_pMgrServicePackageRouteData[value].m_strServicePackageId) == 0) {
						if (num ==0 ){
							int pos = m_pMgrServicePackageRouteData[temp].m_iNext;

							if (pos == 0 ) {
								m_pSHMMgrServicePackageRouteindex->revokeIdx(lkey,temp);
							} else {
								m_pSHMMgrServicePackageRouteindex->add(lkey,pos);
							}
							break;
						} else {
							int pos =m_pMgrServicePackageRouteData[temp].m_iNext;
							m_pMgrServicePackageRouteData[favlue].m_iNext = pos;
							break;
						}
				}
				favlue = temp;
				num++;
				temp = m_pMgrServicePackageRouteData[temp].m_iNext;
			}
		}
		m_pSHMMgrSrvPackageRouteSeqIndex->revokeIdx(sServicePackageRoute.m_iRouteSeq,value);
		m_pSHMMgrServicePackageRouteData->erase(value);
		memset(&m_pMgrServicePackageRouteData[value],0,sizeof(ServicePackageRoute));
		m_pSHMMgrServicePackageRouteLock->V();
		return 0;
	}
#else
	if (!m_pSHMMgrSrvPackageRouteSeqIndex->get(sServicePackageRoute.m_iRouteSeq,&value)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		long lkey = SHMDataMgr::changekey(m_pMgrServicePackageRouteData[value].m_strServicePackageId,m_pSHMMgrServicePackageRouteData->getTotal());
		unsigned int temp =0;

		if (m_pSHMMgrServicePackageRouteindex->get(lkey,&temp)) {
			int favlue =temp;
			int num = 0;

			while (temp) {
				if (strcmp(m_pMgrServicePackageRouteData[temp].m_strServicePackageId,
					m_pMgrServicePackageRouteData[value].m_strServicePackageId) == 0) {
						if (num ==0 ){
							int pos = m_pMgrServicePackageRouteData[temp].m_iNext;

							if (pos == 0 ) {
								m_pSHMMgrServicePackageRouteindex->revokeIdx(lkey,temp);
							} else {
								m_pSHMMgrServicePackageRouteindex->add(lkey,pos);
							}
							break;
						} else {
							int pos =m_pMgrServicePackageRouteData[temp].m_iNext;
							m_pMgrServicePackageRouteData[favlue].m_iNext = pos;
							break;
						}
				}
				favlue = temp;
				num++;
				temp = m_pMgrServicePackageRouteData[temp].m_iNext;
			}
		}
		m_pSHMMgrSrvPackageRouteSeqIndex->revokeIdx(sServicePackageRoute.m_iRouteSeq,value);
		m_pSHMMgrServicePackageRouteData->erase(value);
		memset(&m_pMgrServicePackageRouteData[value],0,sizeof(ServicePackageRoute));
		return 0;
	}
#endif
}

int SHMServicePackageRoute::getServicePackageRoute(int iNodeSeq, ServicePackageRoute & SPackageRoute)
{
	if (!m_pSHMMgrServicePackageRouteData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrSrvPackageRouteSeqIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int value = 0;
	int iRet = 0;
#ifdef _PV_STATE_
	m_pSHMMgrServicePackageRouteLock->P();
	if (m_pSHMMgrSrvPackageRouteSeqIndex->get(iNodeSeq,&value)) {
		memcpy(&SPackageRoute,&m_pMgrServicePackageRouteData[value],sizeof(ServicePackageRoute));
		iRet = 0;
	} else {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	m_pSHMMgrServicePackageRouteLock->V();
	return iRet;
#else
	if (m_pSHMMgrSrvPackageRouteSeqIndex->get(iNodeSeq,&value)) {
		memcpy(&SPackageRoute,&m_pMgrServicePackageRouteData[value],sizeof(ServicePackageRoute));
		iRet = 0;
	} else {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return iRet;
#endif

}

int SHMServicePackageRoute::getServicePackageRouteALL(const char * psServicePackageId,vector<ServicePackageRoute> & vSPackageRoute)
{
	if (!m_pSHMMgrServicePackageRouteData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrServicePackageRouteindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	vSPackageRoute.clear();
	long lkey = 0;
	int iRet = 0;
	lkey = SHMDataMgr::changekey(psServicePackageId,m_pSHMMgrServicePackageRouteData->getTotal());
	unsigned int pValue = 0;
#ifdef _PV_STATE_
	m_pSHMMgrServicePackageRouteLock->P();

	if (!m_pSHMMgrServicePackageRouteindex->get(lkey,&pValue)) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		while (pValue) {
			if (strcmp(m_pMgrServicePackageRouteData[pValue].m_strServicePackageId,psServicePackageId)==0) {
				vSPackageRoute.push_back(m_pMgrServicePackageRouteData[pValue]);
			}
			pValue = m_pMgrServicePackageRouteData[pValue].m_iNext;
		}
	}
	m_pSHMMgrServicePackageRouteLock->V();
	if (vSPackageRoute.size() == 0 ){
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return iRet;
#else
	if (!m_pSHMMgrServicePackageRouteindex->get(lkey,&pValue)) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		while (pValue) {
			if (strcmp(m_pMgrServicePackageRouteData[pValue].m_strServicePackageId,psServicePackageId)==0) {
				vSPackageRoute.push_back(m_pMgrServicePackageRouteData[pValue]);
			}
			pValue = m_pMgrServicePackageRouteData[pValue].m_iNext;
		}
	}
	if (vSPackageRoute.size() == 0 ){
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return iRet;
#endif

}

int SHMServicePackageRouteCCR::deleteServicePackageRouteCCR(const CServicePackageRouteCCR & sServicePackageRouteCCR)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrServicePackageRouteCCRLock->P();

	if(m_pSHMMgrSrvPackageRouteCCRSeqindex->get(sServicePackageRouteCCR.m_iRouteCCRSeq,&value)) {
		unsigned int temp = 0;

		if (m_pSHMMgrSrvPackageRouteSeqindex->get(m_pMgrServicePackageRouteCCRData[value].m_iRouteSeq,&temp)) {
			int favlue = temp;
			int num = 0;

			while (temp) {
				if (m_pMgrServicePackageRouteCCRData[value].m_iRouteSeq
					==m_pMgrServicePackageRouteCCRData[temp].m_iRouteSeq) {
						if (num == 0) {
							int pos = m_pMgrServicePackageRouteCCRData[temp].m_iNext;
							if (pos == 0 ) {
								m_pSHMMgrSrvPackageRouteSeqindex->revokeIdx(m_pMgrServicePackageRouteCCRData[temp].m_iRouteSeq,temp);
							} else {
								m_pSHMMgrSrvPackageRouteSeqindex->add(m_pMgrServicePackageRouteCCRData[temp].m_iRouteSeq,pos);
							}
						} else {
							int pos = m_pMgrServicePackageRouteCCRData[temp].m_iNext;
							m_pMgrServicePackageRouteCCRData[favlue].m_iNext = pos;
						}
						break;
				}
				num++;
				favlue = temp;
				temp = m_pMgrServicePackageRouteCCRData[temp].m_iNext;
			}
		}
		m_pSHMMgrSrvPackageRouteCCRSeqindex->revokeIdx(sServicePackageRouteCCR.m_iRouteCCRSeq,value);
		m_pSHMMgrServicePackageRouteCCRData->erase(value);
		memset(&m_pMgrServicePackageRouteCCRData[value],0,sizeof(CServicePackageRouteCCR));
		m_pSHMMgrServicePackageRouteCCRLock->V();
		return 0;
	}
	m_pSHMMgrServicePackageRouteCCRLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;

#else

	if(m_pSHMMgrSrvPackageRouteCCRSeqindex->get(sServicePackageRouteCCR.m_iRouteCCRSeq,&value)) {
		unsigned int temp = 0;

		if (m_pSHMMgrSrvPackageRouteSeqindex->get(m_pMgrServicePackageRouteCCRData[value].m_iRouteSeq,&temp)) {
			int favlue = temp;
			int num = 0;

			while (temp) {
				if (m_pMgrServicePackageRouteCCRData[value].m_iRouteSeq
					==m_pMgrServicePackageRouteCCRData[temp].m_iRouteSeq) {
						if (num == 0) {
							int pos = m_pMgrServicePackageRouteCCRData[temp].m_iNext;
							if (pos == 0 ) {
								m_pSHMMgrSrvPackageRouteSeqindex->revokeIdx(m_pMgrServicePackageRouteCCRData[temp].m_iRouteSeq,temp);
							} else {
								m_pSHMMgrSrvPackageRouteSeqindex->add(m_pMgrServicePackageRouteCCRData[temp].m_iRouteSeq,pos);
							}
						} else {
							int pos = m_pMgrServicePackageRouteCCRData[temp].m_iNext;
							m_pMgrServicePackageRouteCCRData[favlue].m_iNext = pos;
						}
						break;
				}
				num++;
				favlue = temp;
				temp = m_pMgrServicePackageRouteCCRData[temp].m_iNext;
			}
		}
		m_pSHMMgrSrvPackageRouteCCRSeqindex->revokeIdx(sServicePackageRouteCCR.m_iRouteCCRSeq,value);
		m_pSHMMgrServicePackageRouteCCRData->erase(value);
		memset(&m_pMgrServicePackageRouteCCRData[value],0,sizeof(CServicePackageRouteCCR));
		return 0;
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif

}

int SHMServicePackageRouteCCR::updateServicePackageRouteCCR(const CServicePackageRouteCCR & sServicePackageRouteCCR)
{
	unsigned int value  = 0;
	int iRet = 0;

	if (m_pSHMMgrSrvPackageRouteCCRSeqindex->get(sServicePackageRouteCCR.m_iRouteCCRSeq,&value)) {
	    iRet = deleteServicePackageRouteCCR(m_pMgrServicePackageRouteCCRData[value]);

		if (iRet != 0) {
			return iRet;
		}
		iRet = addServicePackageRouteCCR(sServicePackageRouteCCR);
	}
	return iRet;
}

int SHMServicePackageRouteCCR::addServicePackageRouteCCR(const CServicePackageRouteCCR & sServicePackageRouteCCR)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrServicePackageRouteCCRLock->P();

	if(!m_pSHMMgrSrvPackageRouteCCRSeqindex->get(sServicePackageRouteCCR.m_iRouteCCRSeq,&value)) {
		int i = m_pSHMMgrServicePackageRouteCCRData->malloc();

		if ( i == 0) {
			m_pSHMMgrServicePackageRouteCCRLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrServicePackageRouteCCRData[i],&sServicePackageRouteCCR,sizeof(CServicePackageRouteCCR));

		unsigned int temp = 0;

		if (m_pSHMMgrSrvPackageRouteSeqindex->get(m_pMgrServicePackageRouteCCRData[i].m_iRouteSeq,&temp)) {
			int pos = m_pMgrServicePackageRouteCCRData[temp].m_iNext;
			m_pMgrServicePackageRouteCCRData[i].m_iNext = pos;
			m_pMgrServicePackageRouteCCRData[temp].m_iNext = i;
		} else {
			m_pSHMMgrSrvPackageRouteSeqindex->add(m_pMgrServicePackageRouteCCRData[i].m_iRouteSeq,i);
			m_pMgrServicePackageRouteCCRData[i].m_iNext = 0;
		}
		m_pSHMMgrSrvPackageRouteCCRSeqindex->add(sServicePackageRouteCCR.m_iRouteCCRSeq,i);
		m_pSHMMgrServicePackageRouteCCRLock->V();
		return 0;
	}
	m_pSHMMgrServicePackageRouteCCRLock->V();
	return MBC_SHM_ADD_DATA_EXIST;

#else

	if(!m_pSHMMgrSrvPackageRouteCCRSeqindex->get(sServicePackageRouteCCR.m_iRouteCCRSeq,&value)) {
		int i = m_pSHMMgrServicePackageRouteCCRData->malloc();

		if ( i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrServicePackageRouteCCRData[i],&sServicePackageRouteCCR,sizeof(CServicePackageRouteCCR));

		unsigned int temp = 0;

		if (m_pSHMMgrSrvPackageRouteSeqindex->get(m_pMgrServicePackageRouteCCRData[i].m_iRouteSeq,&temp)) {
			int pos = m_pMgrServicePackageRouteCCRData[temp].m_iNext;
			m_pMgrServicePackageRouteCCRData[i].m_iNext = pos;
			m_pMgrServicePackageRouteCCRData[temp].m_iNext = i;
		} else {
			m_pSHMMgrSrvPackageRouteSeqindex->add(m_pMgrServicePackageRouteCCRData[i].m_iRouteSeq,i);
			m_pMgrServicePackageRouteCCRData[i].m_iNext = 0;
		}
		m_pSHMMgrSrvPackageRouteCCRSeqindex->add(sServicePackageRouteCCR.m_iRouteCCRSeq,i);
		return 0;
	}
	return MBC_SHM_ADD_DATA_EXIST;
#endif

}

int SHMServicePackageRouteCCR::getServicePackageRouteCCR(int iRouteCCRSeq, CServicePackageRouteCCR & sServicePackageRouteCCR)
{
	if (!m_pSHMMgrServicePackageRouteCCRData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrSrvPackageRouteCCRSeqindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	unsigned int value = 0;
	int iRet = 0;
#ifdef _PV_STATE_
	m_pSHMMgrServicePackageRouteCCRLock->P();

	if (m_pSHMMgrSrvPackageRouteCCRSeqindex->get(iRouteCCRSeq,&value)) {
		memcpy(&sServicePackageRouteCCR,&m_pMgrServicePackageRouteCCRData[value],sizeof(CServicePackageRouteCCR));
		iRet = 0;
	} else {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	m_pSHMMgrServicePackageRouteCCRLock->V();
	return iRet;
#else
	if (m_pSHMMgrSrvPackageRouteCCRSeqindex->get(iRouteCCRSeq,&value)) {
		memcpy(&sServicePackageRouteCCR,&m_pMgrServicePackageRouteCCRData[value],sizeof(CServicePackageRouteCCR));
		iRet = 0;
	} else {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return iRet;
#endif

}

int SHMServicePackageRouteCCR::getServicePackageRouteCCR(int iRouteSeq, vector<CServicePackageRouteCCR> & vServicePackageRouteCCR)
{
	if (!m_pSHMMgrServicePackageRouteCCRData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrSrvPackageRouteSeqindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	int iRet = 0;
	vServicePackageRouteCCR.clear();
	unsigned int pValue = 0;
#ifdef _PV_STATE_
	m_pSHMMgrServicePackageRouteCCRLock->P();

	if (!m_pSHMMgrSrvPackageRouteSeqindex->get(iRouteSeq,&pValue)) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		while (pValue) {
			if (m_pMgrServicePackageRouteCCRData[pValue].m_iRouteSeq == iRouteSeq) {
				vServicePackageRouteCCR.push_back(m_pMgrServicePackageRouteCCRData[pValue]);
			}
			pValue = m_pMgrServicePackageRouteCCRData[pValue].m_iNext;
		}
	}
	if (vServicePackageRouteCCR.size() == 0 ){
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	m_pSHMMgrServicePackageRouteCCRLock->V();
	return iRet;
#else

	if (!m_pSHMMgrSrvPackageRouteSeqindex->get(iRouteSeq,&pValue)) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		while (pValue) {
			if (m_pMgrServicePackageRouteCCRData[pValue].m_iRouteSeq == iRouteSeq) {
				vServicePackageRouteCCR.push_back(m_pMgrServicePackageRouteCCRData[pValue]);
			}
			pValue = m_pMgrServicePackageRouteCCRData[pValue].m_iNext;
		}
	}
	if (vServicePackageRouteCCR.size() == 0 ){
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return iRet;
#endif

}

int SHMServicePackageJudge::updateServicePackageJudge(const CServicePackageJudge & sServicePackageJudge)
{
	unsigned int value = 0;
	int iRet = 0;
	if (m_pSHMMgrServicePackageJudgeindex->get(sServicePackageJudge.m_iJudgeConditionSeq,&value)) {
		iRet = deleteServicePackageJudge(m_pMgrServicePackageJudgeData[value]);
		if (iRet != 0 )  {
			return iRet;
		}
		iRet = addServicePackageJudge(sServicePackageJudge);
	}
	return iRet;
}

int SHMServicePackageJudge::deleteServicePackageJudge(const CServicePackageJudge & sServicePackageJudge)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	if (!m_pSHMMgrServicePackageJudgeindex->get(sServicePackageJudge.m_iJudgeConditionSeq,&value)) {
		m_pSHMMgrServicePackageJudgeLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		unsigned int temp =0;

		if (m_pSHMMgrSrvPackageJudgeNodeSeqIndex->get(m_pMgrServicePackageJudgeData[value].m_iNodeSeq,&temp)) {
			int favlue =temp;
			int num = 0;

			while (temp) {
				if (m_pMgrServicePackageJudgeData[temp].m_iNodeSeq==m_pMgrServicePackageJudgeData[value].m_iNodeSeq) {
						if (num ==0 ){
							int pos = m_pMgrServicePackageJudgeData[temp].m_iNext;

							if (pos == 0 ) {
								m_pSHMMgrSrvPackageJudgeNodeSeqIndex->revokeIdx(m_pMgrServicePackageJudgeData[temp].m_iNodeSeq,temp);
							} else {
								m_pSHMMgrSrvPackageJudgeNodeSeqIndex->add(m_pMgrServicePackageJudgeData[pos].m_iNodeSeq,pos);
							}
							break;
						} else {
							int pos =m_pMgrServicePackageJudgeData[temp].m_iNext;
							m_pMgrServicePackageJudgeData[favlue].m_iNext = pos;
							break;
						}
				}
				favlue = temp;
				num++;
				temp = m_pMgrServicePackageJudgeData[temp].m_iNext;
			}
		}
		m_pSHMMgrServicePackageJudgeindex->revokeIdx(sServicePackageJudge.m_iJudgeConditionSeq,value);
		m_pSHMMgrServicePackageJudgeData->erase(value);
		memset(&m_pMgrServicePackageJudgeData[value],0,sizeof(CServicePackageJudge));
		m_pSHMMgrServicePackageJudgeLock->V();
		return 0;
	}
#else
	if (!m_pSHMMgrServicePackageJudgeindex->get(sServicePackageJudge.m_iJudgeConditionSeq,&value)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		unsigned int temp =0;

		if (m_pSHMMgrSrvPackageJudgeNodeSeqIndex->get(m_pMgrServicePackageJudgeData[value].m_iNodeSeq,&temp)) {
			int favlue =temp;
			int num = 0;

			while (temp) {
				if (m_pMgrServicePackageJudgeData[temp].m_iNodeSeq==m_pMgrServicePackageJudgeData[value].m_iNodeSeq) {
					if (num ==0 ){
						int pos = m_pMgrServicePackageJudgeData[temp].m_iNext;

						if (pos == 0 ) {
							m_pSHMMgrSrvPackageJudgeNodeSeqIndex->revokeIdx(m_pMgrServicePackageJudgeData[temp].m_iNodeSeq,temp);
						} else {
							m_pSHMMgrSrvPackageJudgeNodeSeqIndex->add(m_pMgrServicePackageJudgeData[pos].m_iNodeSeq,pos);
						}
						break;
					} else {
						int pos =m_pMgrServicePackageJudgeData[temp].m_iNext;
						m_pMgrServicePackageJudgeData[favlue].m_iNext = pos;
						break;
					}
				}
				favlue = temp;
				num++;
				temp = m_pMgrServicePackageJudgeData[temp].m_iNext;
			}
		}
		m_pSHMMgrServicePackageJudgeindex->revokeIdx(sServicePackageJudge.m_iJudgeConditionSeq,value);
		m_pSHMMgrServicePackageJudgeData->erase(temp);
		memset(&m_pMgrServicePackageJudgeData[temp],0,sizeof(CServicePackageJudge));
		return 0;
	}
#endif
}

int SHMServicePackageJudge::addServicePackageJudge(const CServicePackageJudge & sServicePackageJudge)
{
	if (!m_pSHMMgrServicePackageJudgeData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrServicePackageJudgeindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	if (!m_pSHMMgrSrvPackageJudgeNodeSeqIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	int iRet = 0;
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrServicePackageJudgeLock->P();
	if (!m_pSHMMgrServicePackageJudgeindex->get(sServicePackageJudge.m_iJudgeConditionSeq,&value)) {
		int i = m_pSHMMgrServicePackageJudgeData->malloc();
		if (i == 0){
			m_pSHMMgrServicePackageJudgeLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrServicePackageJudgeData[i],&sServicePackageJudge,sizeof(CServicePackageJudge));
		m_pSHMMgrServicePackageJudgeindex->add(sServicePackageJudge.m_iJudgeConditionSeq,i);

		if (m_pSHMMgrSrvPackageJudgeNodeSeqIndex->get(sServicePackageJudge.m_iNodeSeq,&value)) {
			int pos = m_pMgrServicePackageJudgeData[value].m_iNext;
			m_pMgrServicePackageJudgeData[value].m_iNext  = i;
			m_pMgrServicePackageJudgeData[i].m_iNext = pos;
		} else {
			m_pSHMMgrSrvPackageJudgeNodeSeqIndex->add(sServicePackageJudge.m_iNodeSeq,i);
			m_pMgrServicePackageJudgeData[i].m_iNext = 0;
		}
	} else {
		iRet = MBC_SHM_ADD_DATA_EXIST;
	}
	m_pSHMMgrServicePackageJudgeLock->V();
	return iRet;
#else
	if (!m_pSHMMgrServicePackageJudgeindex->get(sServicePackageJudge.m_iJudgeConditionSeq,&value)) {
		int i = m_pSHMMgrServicePackageJudgeData->malloc();
		if (i == 0){
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrServicePackageJudgeData[i],&sServicePackageJudge,sizeof(CServicePackageJudge));
		m_pSHMMgrServicePackageJudgeindex->add(sServicePackageJudge.m_iJudgeConditionSeq,i);

		if (m_pSHMMgrSrvPackageJudgeNodeSeqIndex->get(sServicePackageJudge.m_iNodeSeq,&value)) {
			int pos = m_pMgrServicePackageJudgeData[value].m_iNext;
			m_pMgrServicePackageJudgeData[value].m_iNext  = i;
			m_pMgrServicePackageJudgeData[i].m_iNext = pos;
		} else {
			m_pSHMMgrSrvPackageJudgeNodeSeqIndex->add(sServicePackageJudge.m_iNodeSeq,i);
			m_pMgrServicePackageJudgeData[i].m_iNext = 0;
		}
	} else {
		iRet = MBC_SHM_ADD_DATA_EXIST;
	}
	return iRet;
#endif

}

int SHMServicePackageJudge::getServicePackageJudge(int iJudgeConditionSeq, CServicePackageJudge & sServicePackageJudge)
{
	if (!m_pSHMMgrServicePackageJudgeData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrServicePackageJudgeindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	int iRet = 0;
	unsigned int value = 0;
#ifdef _PV_STATE_
	m_pSHMMgrServicePackageJudgeLock->P();
	if (m_pSHMMgrServicePackageJudgeindex->get(iJudgeConditionSeq,&value)) {
		memcpy(&sServicePackageJudge,&m_pMgrServicePackageJudgeData[value],sizeof(CServicePackageJudge));
	} else {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	m_pSHMMgrServicePackageJudgeLock->V();
	return iRet;
#else
	if (m_pSHMMgrServicePackageJudgeindex->get(iJudgeConditionSeq,&value)) {
		memcpy(&sServicePackageJudge,&m_pMgrServicePackageJudgeData[value],sizeof(CServicePackageJudge));
	} else {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return iRet;
#endif

}

int SHMServicePackageJudge::getServicePackageJudgeAll(int iNodeSeq, vector<CServicePackageJudge> & vServicePackageJudge)
{
	if (!m_pSHMMgrServicePackageJudgeData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrSrvPackageJudgeNodeSeqIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	int iRet = 0;
	unsigned int value = 0;
#ifdef _PV_STATE_
	m_pSHMMgrServicePackageJudgeLock->P();
	if (m_pSHMMgrSrvPackageJudgeNodeSeqIndex->get(iNodeSeq,&value)) {

		while (value) {
			vServicePackageJudge.push_back(m_pMgrServicePackageJudgeData[value]);
			value = m_pMgrServicePackageJudgeData[value].m_iNext;
		}
	} else {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}

	if (vServicePackageJudge.size()==0) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	m_pSHMMgrServicePackageJudgeLock->V();
	return iRet;
#else
	if (m_pSHMMgrSrvPackageJudgeNodeSeqIndex->get(iNodeSeq,&value)) {

		while (value) {
			vServicePackageJudge.push_back(m_pMgrServicePackageJudgeData[value]);
			value = m_pMgrServicePackageJudgeData[value].m_iNext;
		}
	} else {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}

	if (vServicePackageJudge.size()==0) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return iRet;
#endif

}

int SHMServicePackageCCA::getServicePackageCCA(const char * psServiceContextId,vector<CServicePackageCCA> & vServicePackageCCA)
{
	if (!m_pSHMMgrServicePackageCCAData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrServicePackageCCAindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

	vServicePackageCCA.clear();
	int iRet = 0;
	unsigned int value =0;
	long  lkey = 0;
	lkey = SHMDataMgr::changekey(psServiceContextId,m_pSHMMgrServicePackageCCAData->getTotal());
#ifdef _PV_STATE_
	m_pSHMMgrServicePackageCCALock->P();

	if (!m_pSHMMgrServicePackageCCAindex->get(lkey,&value)) {
		iRet  =MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		while (value) {
			if (strcmp(psServiceContextId,m_pMgrServicePackageCCAData[value].m_strServiceContextId) == 0 ){
				vServicePackageCCA.push_back(m_pMgrServicePackageCCAData[value]);
			}
			value = m_pMgrServicePackageCCAData[value].m_iNext;
		}
	}
	if (vServicePackageCCA.size() == 0){
		iRet  =MBC_SHM_QUERY_NO_DATE_FOUND;

	}
	m_pSHMMgrServicePackageCCALock->V();
	return iRet;
#else

	if (!m_pSHMMgrServicePackageCCAindex->get(lkey,&value)) {
		iRet  =MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		while (value) {
			if (strcmp(psServiceContextId,m_pMgrServicePackageCCAData[value].m_strServiceContextId) == 0 ){
				vServicePackageCCA.push_back(m_pMgrServicePackageCCAData[value]);
			}
			value = m_pMgrServicePackageCCAData[value].m_iNext;
		}
	}
	if (vServicePackageCCA.size() == 0){
		iRet  =MBC_SHM_QUERY_NO_DATE_FOUND;

	}
	return iRet;
#endif

}

int SHMServicePackageCCA::DelServicePackageCCA(CServicePackageCCA & sServicePackageCCA)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	if (!m_pSHMMgrSrvPackageCCASeqIndex->get(sServicePackageCCA.m_iServicePackageCCASeq,&value)) {
		m_pSHMMgrServicePackageCCALock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		long lkey = SHMDataMgr::changekey(m_pMgrServicePackageCCAData[value].m_strServiceContextId,m_pSHMMgrServicePackageCCAData->getTotal());
		unsigned int temp =0;

		if (m_pSHMMgrServicePackageCCAindex->get(lkey,&temp)) {
			int favlue =temp;
			int num = 0;

			while (temp) {
				if (strcmp(m_pMgrServicePackageCCAData[temp].m_strServiceContextId,
					m_pMgrServicePackageCCAData[value].m_strServiceContextId) == 0) {
						if (num ==0 ){
							int pos = m_pMgrServicePackageCCAData[temp].m_iNext;

							if (pos == 0 ) {
								m_pSHMMgrServicePackageCCAindex->revokeIdx(lkey,temp);
							} else {
								m_pSHMMgrServicePackageCCAindex->add(lkey,pos);
							}
							break;
						} else {
							int pos =m_pMgrServicePackageCCAData[temp].m_iNext;
							m_pMgrServicePackageCCAData[favlue].m_iNext = pos;
							break;
						}
				}
				favlue = temp;
				num++;
				temp = m_pMgrServicePackageCCAData[temp].m_iNext;
			}
		}
		m_pSHMMgrSrvPackageCCASeqIndex->revokeIdx(sServicePackageCCA.m_iServicePackageCCASeq,value);
		m_pSHMMgrServicePackageCCAData->erase(temp);
		memset(&m_pMgrServicePackageCCAData[temp],0,sizeof(CServicePackageCCA));
		m_pSHMMgrServicePackageCCALock->V();
		return 0;
	}
#else
	if (!m_pSHMMgrSrvPackageCCASeqIndex->get(sServicePackageCCA.m_iServicePackageCCASeq,&value)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		long lkey = SHMDataMgr::changekey(m_pMgrServicePackageCCAData[value].m_strServiceContextId,m_pSHMMgrServicePackageCCAData->getTotal());
		unsigned int temp =0;

		if (m_pSHMMgrServicePackageCCAindex->get(lkey,&temp)) {
			int favlue =temp;
			int num = 0;

			while (temp) {
				if (strcmp(m_pMgrServicePackageCCAData[temp].m_strServiceContextId,
					m_pMgrServicePackageCCAData[value].m_strServiceContextId) == 0) {
						if (num ==0 ){
							int pos = m_pMgrServicePackageCCAData[temp].m_iNext;

							if (pos == 0 ) {
								m_pSHMMgrServicePackageCCAindex->revokeIdx(lkey,temp);
							} else {
								m_pSHMMgrServicePackageCCAindex->add(lkey,pos);
							}
							break;
						} else {
							int pos =m_pMgrServicePackageCCAData[temp].m_iNext;
							m_pMgrServicePackageCCAData[favlue].m_iNext = pos;
							break;
						}
				}
				favlue = temp;
				num++;
				temp = m_pMgrServicePackageCCAData[temp].m_iNext;
			}
		}
		m_pSHMMgrSrvPackageCCASeqIndex->revokeIdx(sServicePackageCCA.m_iServicePackageCCASeq,value);
		m_pSHMMgrServicePackageCCAData->erase(temp);
		memset(&m_pMgrServicePackageCCAData[temp],0,sizeof(CServicePackageCCA));
		return 0;
	}
#endif
}

int SHMServicePackageCCA::UpdateServicePackageCCA(CServicePackageCCA & sServicePackageCCA)
{
	unsigned int value = 0;
	int iRet = 0;

	if (m_pSHMMgrSrvPackageCCASeqIndex->get(sServicePackageCCA.m_iServicePackageCCASeq,&value)){
		iRet = DelServicePackageCCA(m_pMgrServicePackageCCAData[value]);
		if (iRet!=0) {
			return iRet;
		}
		iRet = AddServicePackageCCA(sServicePackageCCA);
	}
	return iRet ;
}

int SHMServicePackageCCA::AddServicePackageCCA(CServicePackageCCA & sServicePackageCCA)
{
	unsigned int value  = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrServicePackageCCALock->P();

	if (m_pSHMMgrSrvPackageCCASeqIndex->get(sServicePackageCCA.m_iServicePackageCCASeq,&value)) {
		m_pSHMMgrServicePackageCCALock->V();
		return MBC_SHM_ERRNO_ADD_DATA_EXIST;
	} else {
		int i = m_pSHMMgrServicePackageCCAData->malloc();

		if (i == 0) {
			m_pSHMMgrServicePackageCCALock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrServicePackageCCAData[i],&sServicePackageCCA,sizeof(CServicePackageCCA));
		long lkey = SHMDataMgr::changekey(sServicePackageCCA.m_strServiceContextId,m_pSHMMgrServicePackageCCAData->getTotal());

		if (m_pSHMMgrServicePackageCCAindex->get(lkey,&value)) {
			int pos = m_pMgrServicePackageCCAData[value].m_iNext;
			m_pMgrServicePackageCCAData[i].m_iNext = pos;
			m_pMgrServicePackageCCAData[value].m_iNext = i;
		} else {
			m_pSHMMgrServicePackageCCAindex->add(lkey,i);
			m_pMgrServicePackageCCAData[i].m_iNext = 0;
		}
		m_pSHMMgrSrvPackageCCASeqIndex->add(sServicePackageCCA.m_iServicePackageCCASeq,i);
		m_pSHMMgrServicePackageCCALock->V();
		return 0;
	}
#else
	if (m_pSHMMgrSrvPackageCCASeqIndex->get(sServicePackageCCA.m_iServicePackageCCASeq,&value)) {
		return MBC_SHM_ERRNO_ADD_DATA_EXIST;
	} else {
		int i = m_pSHMMgrServicePackageCCAData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrServicePackageCCAData[i],&sServicePackageCCA,sizeof(CServicePackageCCA));
		long lkey = SHMDataMgr::changekey(sServicePackageCCA.m_strServiceContextId,m_pSHMMgrServicePackageCCAData->getTotal());

		if (m_pSHMMgrServicePackageCCAindex->get(lkey,&value)) {
			int pos = m_pMgrServicePackageCCAData[value].m_iNext;
			m_pMgrServicePackageCCAData[i].m_iNext = pos;
			m_pMgrServicePackageCCAData[value].m_iNext = i;
		} else {
			m_pSHMMgrServicePackageCCAindex->add(lkey,i);
			m_pMgrServicePackageCCAData[i].m_iNext = 0;
		}
		m_pSHMMgrSrvPackageCCASeqIndex->add(sServicePackageCCA.m_iServicePackageCCASeq,i);
		return 0;
	}
#endif
}

int SHMServicePackageCCA::getServicePackageCCA(int iServicePackageCCASeq,CServicePackageCCA & sServicePackageCCA)
{
	if (!m_pSHMMgrServicePackageCCAData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrSrvPackageCCASeqIndex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	int iRet = 0;
	unsigned int value = 0;
#ifdef _PV_STATE_
	m_pSHMMgrServicePackageCCALock->P();
	if (!m_pSHMMgrSrvPackageCCASeqIndex->get(iServicePackageCCASeq,&value)) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		memcpy(&sServicePackageCCA,&m_pMgrServicePackageCCAData[value],sizeof(CServicePackageCCA));
	}
	m_pSHMMgrServicePackageCCALock->V();
	return iRet;
#else
	if (!m_pSHMMgrSrvPackageCCASeqIndex->get(iServicePackageCCASeq,&value)) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		memcpy(&sServicePackageCCA,&m_pMgrServicePackageCCAData[value],sizeof(CServicePackageCCA));
	}
	return iRet;
#endif

}
int SHMServicePackageFunc::AddServicePackageFunc(CServicePackageFunc & sServicePackageFunc)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrServicePackageFuncLock->P();

	if (!m_pSHMMgrServicePackageFuncindex->get(sServicePackageFunc.m_iCallID,&value)) {
		int i = m_pSHMMgrServicePackageFuncData->malloc();
		if (i == 0 ) {
			m_pSHMMgrServicePackageFuncLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrServicePackageFuncData[i],&sServicePackageFunc,sizeof(CServicePackageFunc));
		m_pSHMMgrServicePackageFuncindex->add(sServicePackageFunc.m_iCallID,i);
		m_pSHMMgrServicePackageFuncLock->V();
		return 0;
	}
	m_pSHMMgrServicePackageFuncLock->V();
	return  MBC_SHM_ERRNO_ADD_DATA_EXIST;
#else

	if (!m_pSHMMgrServicePackageFuncindex->get(sServicePackageFunc.m_iCallID,&value)) {
		int i = m_pSHMMgrServicePackageFuncData->malloc();
		if (i == 0 ) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrServicePackageFuncData[i],&sServicePackageFunc,sizeof(CServicePackageFunc));
		m_pSHMMgrServicePackageFuncindex->add(sServicePackageFunc.m_iCallID,i);
		return 0;
	}
	return  MBC_SHM_ERRNO_ADD_DATA_EXIST;
#endif
}

int SHMServicePackageFunc::DelServicePackageFunc(CServicePackageFunc & sServicePackageFunc)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrServicePackageFuncLock->P();

	if (m_pSHMMgrServicePackageFuncindex->get(sServicePackageFunc.m_iCallID,&value)) {
		m_pSHMMgrServicePackageFuncindex->revokeIdx(sServicePackageFunc.m_iCallID,value);
		m_pSHMMgrServicePackageFuncData->erase(value);
		memset(&m_pMgrServicePackageFuncData[value],0,sizeof(CServicePackageFunc));
		m_pSHMMgrServicePackageFuncLock->V();
		return 0;
	}
	m_pSHMMgrServicePackageFuncLock->V();
	return  MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else

	if (m_pSHMMgrServicePackageFuncindex->get(sServicePackageFunc.m_iCallID,&value)) {
		m_pSHMMgrServicePackageFuncindex->revokeIdx(sServicePackageFunc.m_iCallID,value);
		m_pSHMMgrServicePackageFuncData->erase(value);
		memset(&m_pMgrServicePackageFuncData[value],0,sizeof(CServicePackageFunc));
		return 0;
	}
	return  MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif
}

int SHMServicePackageFunc::UpdateServicePackageFunc(CServicePackageFunc & sServicePackageFunc)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrServicePackageFuncLock->P();

	if (m_pSHMMgrServicePackageFuncindex->get(sServicePackageFunc.m_iCallID,&value)) {
		memcpy(&m_pMgrServicePackageFuncData[value],&sServicePackageFunc,sizeof(CServicePackageFunc));
		m_pSHMMgrServicePackageFuncLock->V();
		return 0;
	}
	m_pSHMMgrServicePackageFuncLock->V();
	return  MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else
	if (m_pSHMMgrServicePackageFuncindex->get(sServicePackageFunc.m_iCallID,&value)) {
		memcpy(&m_pMgrServicePackageFuncData[value],&sServicePackageFunc,sizeof(CServicePackageFunc));
		return 0;
	}
	return  MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif
}

int SHMServicePackageFunc::GetServicePackageFunc(int iCallID,CServicePackageFunc & sServicePackageFunc)
{
	if (!m_pSHMMgrServicePackageFuncData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrServicePackageFuncindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	int iRet = 0;
	unsigned int value = 0;
#ifdef _PV_STATE_
	m_pSHMMgrServicePackageFuncLock->P();
	if (!m_pSHMMgrServicePackageFuncindex->get(iCallID,&value)) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		memcpy(&sServicePackageFunc,&m_pMgrServicePackageFuncData[value],sizeof(CServicePackageFunc));
	}
	m_pSHMMgrServicePackageFuncLock->V();
	return iRet;
#else
	if (!m_pSHMMgrServicePackageFuncindex->get(iCallID,&value)) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		memcpy(&sServicePackageFunc,&m_pMgrServicePackageFuncData[value],sizeof(CServicePackageFunc));
	}
	return iRet;
#endif

}

int SHMServicePackageVle::DelServicePackageVariable(CServicePackageVariable & sServicePackageVariable)
{
	unsigned int value = 0;

#ifndef _DF_LOCK_
	m_pSHMMgrServicePackageVariableLock->P();

	if (m_pSHMMgrServicePackageVariableindex->get(sServicePackageVariable.m_iVariableID,&value)) {
		m_pSHMMgrServicePackageVariableindex->revokeIdx(sServicePackageVariable.m_iVariableID,value);
		m_pSHMMgrServicePackageVariableData->erase(value);
		memset(&m_pMgrServicePackageVariableData[value],0,sizeof(CServicePackageVariable));
		m_pSHMMgrServicePackageVariableLock->V();
		return 0;
	}
	m_pSHMMgrServicePackageVariableLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else
	if (m_pSHMMgrServicePackageVariableindex->get(sServicePackageVariable.m_iVariableID,&value)) {
		m_pSHMMgrServicePackageVariableindex->revokeIdx(sServicePackageVariable.m_iVariableID,value);
		m_pSHMMgrServicePackageVariableData->erase(value);
		memset(&m_pMgrServicePackageVariableData[value],0,sizeof(CServicePackageVariable));
		return 0;
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif
}

int SHMServicePackageVle::AddServicePackageVariable(CServicePackageVariable & sServicePackageVariable)
{
	unsigned int value = 0;

#ifndef _DF_LOCK_
	m_pSHMMgrServicePackageVariableLock->P();

	if (!m_pSHMMgrServicePackageVariableindex->get(sServicePackageVariable.m_iVariableID,&value)) {
		int i = m_pSHMMgrServicePackageVariableData->malloc();

		if (i == 0) {
			m_pSHMMgrServicePackageVariableLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrServicePackageVariableData[i],&sServicePackageVariable,sizeof(CServicePackageVariable));
		m_pSHMMgrServicePackageVariableindex->add(sServicePackageVariable.m_iVariableID,i);
		m_pSHMMgrServicePackageVariableLock->V();
		return 0;
	}
	m_pSHMMgrServicePackageVariableLock->V();
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#else
	if (!m_pSHMMgrServicePackageVariableindex->get(sServicePackageVariable.m_iVariableID,&value)) {
		int i = m_pSHMMgrServicePackageVariableData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrServicePackageVariableData[i],&sServicePackageVariable,sizeof(CServicePackageVariable));
		m_pSHMMgrServicePackageVariableindex->add(sServicePackageVariable.m_iVariableID,i);
		return 0;
	}
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#endif

}

int SHMServicePackageVle::UpdateServicePackageVariable(CServicePackageVariable & sServicePackageVariable)
{
	unsigned int value = 0;

#ifndef _DF_LOCK_
	m_pSHMMgrServicePackageVariableLock->P();

	if (m_pSHMMgrServicePackageVariableindex->get(sServicePackageVariable.m_iVariableID,&value)) {
		memcpy(&m_pMgrServicePackageVariableData[value],&sServicePackageVariable,sizeof(CServicePackageVariable));
		m_pSHMMgrServicePackageVariableLock->V();
		return 0;
	}
	m_pSHMMgrServicePackageVariableLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else
	if (m_pSHMMgrServicePackageVariableindex->get(sServicePackageVariable.m_iVariableID,&value)) {
		memcpy(&m_pMgrServicePackageVariableData[value],&sServicePackageVariable,sizeof(CServicePackageVariable));
		return 0;
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif

}

int SHMServicePackageVle::GetServicePackageVariable(int iVariableID, CServicePackageVariable & sServicePackageVariable)
{
	if (!m_pSHMMgrServicePackageVariableData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrServicePackageVariableindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
	int iRet = 0;
	unsigned int value = 0;
#ifdef _PV_STATE_
	m_pSHMMgrServicePackageVariableLock->P();
	if (!m_pSHMMgrServicePackageVariableindex->get(iVariableID,&value)) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		memcpy(&sServicePackageVariable,&m_pMgrServicePackageVariableData[value],sizeof(CServicePackageVariable));
	}
	m_pSHMMgrServicePackageVariableLock->V();
	return iRet;
#else
	if (!m_pSHMMgrServicePackageVariableindex->get(iVariableID,&value)) {
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		memcpy(&sServicePackageVariable,&m_pMgrServicePackageVariableData[value],sizeof(CServicePackageVariable));
	}
	return iRet;
#endif


}

int SHMSCongestMsg::GetAllStatisticsCongestMsg(vector<StatisticsCongestMsg> &vStatisticsCongestMsg)
{
	vStatisticsCongestMsg.clear();
	if (!m_pSHMMgrSCongestMsgData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
#ifdef _PV_STATE_
	if (!m_pSHMMgrSCongestMsgLock->P(false)) {
		return MBC_PV_ERRNO_P_FAILED;
	}
	for (int i =1; i<=m_pSHMMgrSCongestMsgData->getCount(); i++){
		vStatisticsCongestMsg.push_back(m_pMgrSCongestMsgData[i]);
	}
	m_pSHMMgrSCongestMsgLock->V();
	return 0;
#else

	for (int i =1; i<=m_pSHMMgrSCongestMsgData->getCount(); i++){
		vStatisticsCongestMsg.push_back(m_pMgrSCongestMsgData[i]);
	}
	return 0;
#endif

}


int SHMSCongestMsg::GetStatisticsCongestMsg(const char* m_strServiceContextId,StatisticsCongestMsg &sStatisticsCongestMsg)
{
	if (!m_pSHMMgrSCongestMsgData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrSCongestMsgindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
#ifdef _PV_STATE_
	if (!m_pSHMMgrSCongestMsgLock->P(false)) {
		return MBC_PV_ERRNO_P_FAILED;
	}

	int iRet = 0;
	unsigned int value = 0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(m_strServiceContextId,m_pSHMMgrSCongestMsgData->getTotal());

	if (m_pSHMMgrSCongestMsgindex->get(lkey,&value)) {
		while (value) {
			if (strcmp(m_strServiceContextId,m_pMgrSCongestMsgData[value].m_sServiceContextId)==0 ) {
				memcpy(&sStatisticsCongestMsg,&m_pMgrSCongestMsgData[value],sizeof(StatisticsCongestMsg));
				m_pSHMMgrSCongestMsgLock->V();
				return 0;
			}
			value = m_pMgrSCongestMsgData[value].m_iNext;
		}
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else{
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	m_pSHMMgrSCongestMsgLock->V();
	return iRet;
#else
	int iRet = 0;
	unsigned int value = 0;
	long  lkey = 0;
	lkey = SHMDataMgr::changekey(m_strServiceContextId,m_pSHMMgrSCongestMsgData->getTotal());

	if (m_pSHMMgrSCongestMsgindex->get(lkey,&value)) {
		while (value) {
			if (strcmp(m_strServiceContextId,m_pMgrSCongestMsgData[value].m_sServiceContextId)==0 ) {
				memcpy(&sStatisticsCongestMsg,&m_pMgrSCongestMsgData[value],sizeof(StatisticsCongestMsg));
				return 0;
			}
			value = m_pMgrSCongestMsgData[value].m_iNext;
		}
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else{
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return iRet;
#endif

}

int SHMSCongestMsg::InsertStatisticsCongestMsg(StatisticsCongestMsg sStatisticsCongestMsg)
{
	if (!m_pSHMMgrSCongestMsgData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrSCongestMsgindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}

#ifndef _DF_LOCK_
	if (!m_pSHMMgrSCongestMsgLock->P(false)) {
		return MBC_PV_ERRNO_P_FAILED;
	}

	int iRet = 0;
	unsigned int value = 0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(sStatisticsCongestMsg.m_sServiceContextId,m_pSHMMgrSCongestMsgData->getTotal());

	if (m_pSHMMgrSCongestMsgindex->get(lkey,&value)) {
		int _tmmp = value;
		while (value) {
			if (strcmp(m_pMgrSCongestMsgData[value].m_sServiceContextId,sStatisticsCongestMsg.m_sServiceContextId) ==0 ) {
				iRet = MBC_SHM_ADD_DATA_EXIST;
				m_pSHMMgrSCongestMsgLock->V();
				return iRet;
			}
			value = m_pMgrSCongestMsgData[value].m_iNext;
		}

		int i = m_pSHMMgrSCongestMsgData->malloc();

		if (i == 0) {
			iRet = MBC_MEM_SPACE_SMALL;
			m_pSHMMgrSCongestMsgLock->V();
			return iRet;
		}
		memcpy(&m_pMgrSCongestMsgData[i],&sStatisticsCongestMsg,sizeof(StatisticsCongestMsg));
		int _fnext = m_pMgrSCongestMsgData[_tmmp].m_iNext ;
		m_pMgrSCongestMsgData[_tmmp].m_iNext = i;
		m_pMgrSCongestMsgData[i].m_iNext = _fnext;
	} else{
		int i = m_pSHMMgrSCongestMsgData->malloc();

		if (i == 0) {
			m_pSHMMgrSCongestMsgLock->V();
			iRet = MBC_MEM_SPACE_SMALL;
			return iRet;
		}
		memcpy(&m_pMgrSCongestMsgData[i],&sStatisticsCongestMsg,sizeof(StatisticsCongestMsg));
		m_pSHMMgrSCongestMsgindex->add(lkey,i);
		m_pMgrSCongestMsgData[i].m_iNext = 0;
	}
	m_pSHMMgrSCongestMsgLock->V();
	return iRet ;
#else
	int iRet = 0;
	unsigned int value = 0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(sStatisticsCongestMsg.m_sServiceContextId,m_pSHMMgrSCongestMsgData->getTotal());

	if (m_pSHMMgrSCongestMsgindex->get(lkey,&value)) {
		int _tmmp = value;
		while (value) {
			if (strcmp(m_pMgrSCongestMsgData[value].m_sServiceContextId,sStatisticsCongestMsg.m_sServiceContextId) ==0 ) {
				iRet = MBC_SHM_ADD_DATA_EXIST;
				return iRet;
			}
			value = m_pMgrSCongestMsgData[value].m_iNext;
		}

		int i = m_pSHMMgrSCongestMsgData->malloc();

		if (i == 0) {
			iRet = MBC_MEM_SPACE_SMALL;
			return iRet;
		}
		memcpy(&m_pMgrSCongestMsgData[i],&sStatisticsCongestMsg,sizeof(StatisticsCongestMsg));
		int _fnext = m_pMgrSCongestMsgData[_tmmp].m_iNext ;
		m_pMgrSCongestMsgData[_tmmp].m_iNext = i;
		m_pMgrSCongestMsgData[i].m_iNext = _fnext;
	} else{
		int i = m_pSHMMgrSCongestMsgData->malloc();

		if (i == 0) {
			iRet = MBC_MEM_SPACE_SMALL;
			return iRet;
		}
		memcpy(&m_pMgrSCongestMsgData[i],&sStatisticsCongestMsg,sizeof(StatisticsCongestMsg));
		m_pSHMMgrSCongestMsgindex->add(lkey,i);
		m_pMgrSCongestMsgData[i].m_iNext = 0;
	}
	return iRet ;
#endif

}

int SHMSCongestMsg::UpdateStatisticsCongestInMsg(const char* m_strServiceContextId,int num)
{
	if (!m_pSHMMgrSCongestMsgData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrSCongestMsgindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
#ifndef _DF_LOCK_
	if (!m_pSHMMgrSCongestMsgLock->P(false)) {
		return MBC_PV_ERRNO_P_FAILED;
	}

	int iRet = 0;
	unsigned int value = 0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(m_strServiceContextId,m_pSHMMgrSCongestMsgData->getTotal());

	if (m_pSHMMgrSCongestMsgindex->get(lkey,&value)) {
		while (value) {
			if (strcmp(m_strServiceContextId,m_pMgrSCongestMsgData[value].m_sServiceContextId)==0 ) {
				m_pMgrSCongestMsgData[value].m_iInMsgNum+=num;
				m_pSHMMgrSCongestMsgLock->V();
				return 0;
			}
			value = m_pMgrSCongestMsgData[value].m_iNext;
		}
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else{
		iRet = MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	}
	m_pSHMMgrSCongestMsgLock->V();
	return iRet ;
#else
	int iRet = 0;
	unsigned int value = 0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(m_strServiceContextId,m_pSHMMgrSCongestMsgData->getTotal());

	if (m_pSHMMgrSCongestMsgindex->get(lkey,&value)) {
		while (value) {
			if (strcmp(m_strServiceContextId,m_pMgrSCongestMsgData[value].m_sServiceContextId)==0 ) {
				m_pMgrSCongestMsgData[value].m_iInMsgNum+=num;
				return 0;
			}
			value = m_pMgrSCongestMsgData[value].m_iNext;
		}
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else{
		iRet = MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	}
	return iRet ;
#endif

}

int SHMSCongestMsg::UpdateStatisticsCongestOutMsg(const char* m_strServiceContextId,int num)
{
	if (!m_pSHMMgrSCongestMsgData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrSCongestMsgindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
#ifndef _DF_LOCK_
	if (!m_pSHMMgrSCongestMsgLock->P(false)) {
		return MBC_PV_ERRNO_P_FAILED;
	}

	int iRet = 0;
	unsigned int value = 0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(m_strServiceContextId,m_pSHMMgrSCongestMsgData->getTotal());

	if (m_pSHMMgrSCongestMsgindex->get(lkey,&value)) {
		while (value) {
			if (strcmp(m_strServiceContextId,m_pMgrSCongestMsgData[value].m_sServiceContextId)==0 ) {
				m_pMgrSCongestMsgData[value].m_iOutMsgNum+=num;
				m_pSHMMgrSCongestMsgLock->V();
				return 0;
			}
			value = m_pMgrSCongestMsgData[value].m_iNext;
		}
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else{
		iRet = MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	}
	m_pSHMMgrSCongestMsgLock->V();
	return iRet ;
#else
	int iRet = 0;
	unsigned int value = 0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(m_strServiceContextId,m_pSHMMgrSCongestMsgData->getTotal());

	if (m_pSHMMgrSCongestMsgindex->get(lkey,&value)) {
		while (value) {
			if (strcmp(m_strServiceContextId,m_pMgrSCongestMsgData[value].m_sServiceContextId)==0 ) {
				m_pMgrSCongestMsgData[value].m_iOutMsgNum+=num;
				return 0;
			}
			value = m_pMgrSCongestMsgData[value].m_iNext;
		}
		iRet = MBC_SHM_QUERY_NO_DATE_FOUND;
	} else{
		iRet = MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	}
	return iRet ;
#endif

}

int SHMSCongestMsg::UpdateStatisticsCongestDiscardMsg(const char* m_strServiceContextId,int num)
{
	if (!m_pSHMMgrSCongestMsgData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}

	if (!m_pSHMMgrSCongestMsgindex->exist()) {
		return MBC_SHM_ERRNO_INDEX_NOT_EXIST;
	}
#ifndef _DF_LOCK_
	if (!m_pSHMMgrSCongestMsgLock->P(false)) {
		return MBC_PV_ERRNO_P_FAILED;
	}

	int iRet = 0;
	unsigned int value = 0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(m_strServiceContextId,m_pSHMMgrSCongestMsgData->getTotal());

	if (m_pSHMMgrSCongestMsgindex->get(lkey,&value)) {
		while (value) {
			if (strcmp(m_strServiceContextId,m_pMgrSCongestMsgData[value].m_sServiceContextId)==0 ) {
				m_pMgrSCongestMsgData[value].m_iDiscardNum+=num;
				m_pSHMMgrSCongestMsgLock->V();
				return 0;
			}
			value = m_pMgrSCongestMsgData[value].m_iNext;
		}
		iRet = MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else{
		iRet = MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	}
	m_pSHMMgrSCongestMsgLock->V();
	return iRet ;
#else
	int iRet = 0;
	unsigned int value = 0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(m_strServiceContextId,m_pSHMMgrSCongestMsgData->getTotal());

	if (m_pSHMMgrSCongestMsgindex->get(lkey,&value)) {
		while (value) {
			if (strcmp(m_strServiceContextId,m_pMgrSCongestMsgData[value].m_sServiceContextId)==0 ) {
				m_pMgrSCongestMsgData[value].m_iDiscardNum+=num;
				return 0;
			}
			value = m_pMgrSCongestMsgData[value].m_iNext;
		}
		iRet = MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else{
		iRet = MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	}
	return iRet ;
#endif

}

int SHMWfProcessMq::getAllStWfProcessMq(vector<ST_WF_PROCESS_MQ> & vStWfProcessMq)
{
	vStWfProcessMq.clear();
#ifdef _PV_STATE_
	m_pSHMMgrWfprocessMqLock->P();
	for (int i =1; i<=m_pSHMMgrWfprocessMqData->getCount(); i++) {
		vStWfProcessMq.push_back(m_pMgrWfprocessMqData[i]);
	}
	m_pSHMMgrWfprocessMqLock->V();
	return 0;
#else
	for (int i =1; i<=m_pSHMMgrWfprocessMqData->getCount(); i++) {
		vStWfProcessMq.push_back(m_pMgrWfprocessMqData[i]);
	}
	return 0;
#endif

}

int SHMWfProcessMq::getStWfProcessMq(ST_WF_PROCESS_MQ &sStWfProcessMq)
{
	unsigned int value = 0;
#ifdef _PV_STATE_
	m_pSHMMgrWfprocessMqLock->P();

	if (!m_pSHMMgrWfprocessMqindex->get(sStWfProcessMq.m_iProcessID,&value)) {
		m_pSHMMgrWfprocessMqLock->V();
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		memcpy(&sStWfProcessMq,&m_pMgrWfprocessMqData[value],sizeof(ST_WF_PROCESS_MQ));
	}
	m_pSHMMgrWfprocessMqLock->V();
	return 0;
#else
	if (!m_pSHMMgrWfprocessMqindex->get(sStWfProcessMq.m_iProcessID,&value)) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		memcpy(&sStWfProcessMq,&m_pMgrWfprocessMqData[value],sizeof(ST_WF_PROCESS_MQ));
	}
	return 0;
#endif

}

int SHMWfProcessMq::addStWfProcessMq(const ST_WF_PROCESS_MQ &sStWfProcessMq)
{
	unsigned int value = 0;
#ifdef _DF_LOCK_
	m_pSHMMgrWfprocessMqLock->P();

	if (!m_pSHMMgrWfprocessMqindex->get(sStWfProcessMq.m_iProcessID,&value)) {
		int i = m_pSHMMgrWfprocessMqData->malloc();

		if (i == 0) {
			m_pSHMMgrWfprocessMqLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrWfprocessMqData[i],&sStWfProcessMq,sizeof(ST_WF_PROCESS_MQ));
		m_pSHMMgrWfprocessMqindex->add(sStWfProcessMq.m_iProcessID,i);
	} else {
		m_pSHMMgrWfprocessMqLock->V();
		return MBC_SHM_ADD_DATA_EXIST;
	}
	m_pSHMMgrWfprocessMqLock->V();
	return 0;
#else

	if (!m_pSHMMgrWfprocessMqindex->get(sStWfProcessMq.m_iProcessID,&value)) {
		int i = m_pSHMMgrWfprocessMqData->malloc();

		if (i == 0) {
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrWfprocessMqData[i],&sStWfProcessMq,sizeof(ST_WF_PROCESS_MQ));
		m_pSHMMgrWfprocessMqindex->add(sStWfProcessMq.m_iProcessID,i);
	} else {
		return MBC_SHM_ADD_DATA_EXIST;
	}
	return 0;
#endif

}

int SHMPortInfo::getPortInfoAll(vector<PortInfo> &vPortInfo)
{
	if (m_pSHMMgrPortInfoData == NULL) {
		return MBC_SHM_ERRNO_DATA_NOTCONNECT;
	}
	if (!m_pSHMMgrPortInfoData->exist()) {
		return MBC_SHM_ERRNO_DATA_NOT_EXIST;
	}
	vPortInfo.clear();

#ifdef _PV_STATE_
	m_pSHMMgrPortInfoLock->P();
	int pos = 1;

	for (int i =1; i<=m_pSHMMgrPortInfoData->getCount();) {

		if (m_pSHMMgrPortInfoData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrPortInfoData->getTotal()) {
				break;
			}
			continue;
		}
		vPortInfo.push_back(m_pMgrPortInfoData[pos]);
		pos++;
		i++;
	}
	m_pSHMMgrPortInfoLock->V();

	if (vPortInfo.size() == 0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#else
	int pos = 1;

	for (int i =1; i<=m_pSHMMgrPortInfoData->getCount();) {

		if (m_pSHMMgrPortInfoData->m_piIdleNext[pos] != -1) {
			if ((pos++)>=m_pSHMMgrPortInfoData->getTotal()) {
				break;
			}
			continue;
		}
		vPortInfo.push_back(m_pMgrPortInfoData[pos]);
		pos++;
		i++;
	}

	if (vPortInfo.size() == 0) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
#endif

}

int SHMPortInfo::getPortInfo(int m_iPortID,PortInfo &oPortInfo)
{
	unsigned int value = 0;
#ifdef _PV_STATE_
	m_pSHMMgrPortInfoLock->P();
	if (!m_pSHMMgrPortInfoindex->get(m_iPortID,&value)) {
		m_pSHMMgrPortInfoLock->V();
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}else {
		memcpy(&oPortInfo,&m_pMgrPortInfoData[value],sizeof(PortInfo));
	}
	m_pSHMMgrPortInfoLock->V();
	return 0;
#else
	if (!m_pSHMMgrPortInfoindex->get(m_iPortID,&value)) {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	} else {
		memcpy(&oPortInfo,&m_pMgrPortInfoData[value],sizeof(PortInfo));
	}
	return 0;
#endif
}

int SHMPortInfo::addPortinfo(const PortInfo &oPortInfo)
{
#ifdef _DF_LOCK_
	unsigned int value = 0;
	m_pSHMMgrPortInfoLock->P();
	if (!m_pSHMMgrPortInfoindex->get(oPortInfo.m_iPortId,&value)) {
		int i = m_pSHMMgrPortInfoData->malloc();

		if (i == 0) {
			m_pSHMMgrPortInfoLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrPortInfoData[i],&oPortInfo,sizeof(PortInfo));

	} else {
		m_pSHMMgrPortInfoLock->V();
		return MBC_SHM_ADD_DATA_EXIST;
	}
	m_pSHMMgrPortInfoLock->V();
	return 0;
#else
	unsigned int value = 0;
	if (!m_pSHMMgrPortInfoindex->get(oPortInfo.m_iPortId,&value)) {
		int i = m_pSHMMgrPortInfoData->malloc();

		if (i == 0) {
			return MBC_MEM_SPACE_SMALL;
		}
		memcpy(&m_pMgrPortInfoData[i],&oPortInfo,sizeof(PortInfo));
		m_pSHMMgrPortInfoindex->add(oPortInfo.m_iPortId,i);

	} else {
		return MBC_SHM_ADD_DATA_EXIST;
	}
	return 0;
#endif

}

int SHMPortInfo::delPortInfo(const PortInfo &oPortInfo)
{
#ifdef _DF_LOCK_
	unsigned int value = 0;
	m_pSHMMgrPortInfoLock->P();
	if (!m_pSHMMgrPortInfoindex->get(oPortInfo.m_iPortId,&value)) {
		m_pSHMMgrPortInfoLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else{
		if (m_pSHMMgrPortInfoindex->revokeIdx(oPortInfo.m_iPortId,value)) {
			m_pSHMMgrPortInfoData->erase(value);
			memset(&m_pMgrPortInfoData[value],0,sizeof(PortInfo));
		} else {
			m_pSHMMgrPortInfoLock->V();
			return MBC_SHM_ERRNO_DEL_INDEX_FAILED;
		}

	}
	m_pSHMMgrPortInfoLock->V();
	return 0;
#else
	unsigned int value = 0;
	if (!m_pSHMMgrPortInfoindex->get(oPortInfo.m_iPortId,&value)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else{
		if (m_pSHMMgrPortInfoindex->revokeIdx(oPortInfo.m_iPortId,value)) {
			m_pSHMMgrPortInfoData->erase(value);
			memset(&m_pMgrPortInfoData[value],0,sizeof(PortInfo));
		} else {
			return MBC_SHM_ERRNO_DEL_INDEX_FAILED;
		}

	}
	return 0;
#endif

}

int SHMPortInfo::updatePortinfo(const PortInfo &oPortInfo)
{
#ifdef _DF_LOCK_
	unsigned int value =0;
	m_pSHMMgrPortInfoLock->P();
	if (!m_pSHMMgrPortInfoindex->get(oPortInfo.m_iPortId,&value)) {
		m_pSHMMgrPortInfoLock->V();
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else{
		memcpy(&m_pMgrPortInfoData[value],&oPortInfo,sizeof(PortInfo));
	}
	m_pSHMMgrPortInfoLock->V();
	return 0;
#else
	unsigned int value =0;
	if (!m_pSHMMgrPortInfoindex->get(oPortInfo.m_iPortId,&value)) {
		return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
	} else{
		memcpy(&m_pMgrPortInfoData[value],&oPortInfo,sizeof(PortInfo));
	}
	return 0;
#endif
}

int SHMPortInfo::clearPortInfo()
{
#ifdef _DF_LOCK_
	m_pSHMMgrPortInfoLock->P();
	m_pSHMMgrPortInfoData->clear();
	m_pSHMMgrPortInfoindex->empty();
	m_pSHMMgrPortInfoLock->V();
	return 0;
#else
	m_pSHMMgrPortInfoData->clear();
	m_pSHMMgrPortInfoindex->empty();
	return 0;
#endif

}

void SHMSessionClear::addSessionClear(const SessionClearData &oSessionClear)
{
	char skey[12] = {0};
	long lkey = 0;
	unsigned int pValue = 0;
	lkey = SHMDataMgr::changekey(changeSessionID(oSessionClear.m_sSessionID,skey),m_pSHMMgrSessionClearData->getTotal());
#ifndef _DF_LOCK_
	m_pSHMMgrSessionClearLock->P();

	if (!m_pSHMMgrSessionClearIndex->get(lkey,&pValue)) {
		int pos = m_pSHMMgrSessionClearData->malloc();
		if (pos == 0) {
			m_pSHMMgrSessionClearData->reset();
			pos = m_pSHMMgrSessionClearData->malloc();
		}
		int _iNext  = m_pMgrSessionClearData[pos].m_iNext;
		memset(&m_pMgrSessionClearData[pos],0,sizeof(SessionClearData));
		memcpy(&m_pMgrSessionClearData[pos],&oSessionClear,sizeof(SessionClearData));
		m_pMgrSessionClearData[pos].m_iNext = _iNext;
		//setFreePos(pos);//设置下一次调用时候的空闲节点
		m_pMgrSessionClearData[pos].m_iIndexNext = 0;
		m_pSHMMgrSessionClearIndex->add(lkey,pos);
		m_pSHMMgrSessionClearLock->V();
	} else {
		unsigned int fValue = pValue;

		while (pValue) {
			if (strcmp(m_pMgrSessionClearData[pValue].m_sSessionID,oSessionClear.m_sSessionID) == 0) {
				m_pSHMMgrSessionClearLock->V();
				return MBC_SHM_ADD_DATA_EXIST;
			}
			pValue = m_pMgrSessionClearData[pValue].m_iIndexNext;
		}
		int pos = m_pSHMMgrSessionClearData->malloc();
		if (pos == 0) {
			m_pSHMMgrSessionClearData->reset();
			pos = m_pSHMMgrSessionClearData->malloc();
		}
		int _iNext  = m_pMgrSessionClearData[pos].m_iNext;
		memset(&m_pMgrSessionClearData[pos],0,sizeof(SessionClearData));
		memcpy(&m_pMgrSessionClearData[pos],&oSessionClear,sizeof(SessionClearData));
		m_pMgrSessionClearData[pos].m_iNext =_iNext;
		//setFreePos(pos);//设置下一次调用时候的空闲节点
		int temppos = m_pMgrSessionClearData[fValue].m_iIndexNext;
		m_pMgrSessionClearData[pos].m_iIndexNext = temppos;
		m_pMgrSessionClearData[fValue].m_iIndexNext = pos;
		m_pSHMMgrSessionClearLock->V();
	}
	return 0;
#else
	if (!m_pSHMMgrSessionClearIndex->get(lkey,&pValue)) {
		int pos = m_pSHMMgrSessionClearData->malloc();
		if (pos == 0) {
			m_pSHMMgrSessionClearData->reset();
			pos = m_pSHMMgrSessionClearData->malloc();
		}
		int _iNext  = m_pMgrSessionClearData[pos].m_iNext;
		memset(&m_pMgrSessionClearData[pos],0,sizeof(SessionClearData));
		memcpy(&m_pMgrSessionClearData[pos],&oSessionClear,sizeof(SessionClearData));
		m_pMgrSessionClearData[pos].m_iNext = _iNext;
		//setFreePos(pos);//设置下一次调用时候的空闲节点
		m_pMgrSessionClearData[pos].m_iIndexNext = 0;
		m_pSHMMgrSessionClearIndex->add(lkey,pos);
	} else {
		unsigned int fValue = pValue;

		while (pValue) {
			if (strcmp(m_pMgrSessionClearData[pValue].m_sSessionID,oSessionClear.m_sSessionID) == 0) {
				return MBC_SHM_ADD_DATA_EXIST;
			}
			pValue = m_pMgrSessionClearData[pValue].m_iIndexNext;
		}
		int pos = m_pSHMMgrSessionClearData->malloc();
		if (pos == 0) {
			m_pSHMMgrSessionClearData->reset();
			pos = m_pSHMMgrSessionClearData->malloc();
		}
		int _iNext  = m_pMgrSessionClearData[pos].m_iNext;
		memset(&m_pMgrSessionClearData[pos],0,sizeof(SessionClearData));
		memcpy(&m_pMgrSessionClearData[pos],&oSessionClear,sizeof(SessionClearData));
		m_pMgrSessionClearData[pos].m_iNext =_iNext;
		//setFreePos(pos);//设置下一次调用时候的空闲节点
		int temppos = m_pMgrSessionClearData[fValue].m_iIndexNext;
		m_pMgrSessionClearData[pos].m_iIndexNext = temppos;
		m_pMgrSessionClearData[fValue].m_iIndexNext = pos;
	}
	return 0;
#endif
}

int SHMSessionClear::delSessionClear(const SessionClearData &oSessionClear)
{
	char skey[12] = {0};
	long lkey = 0;
	unsigned int pValue = 0;
	lkey = SHMDataMgr::changekey(changeSessionID(oSessionClear.m_sSessionID,skey),m_pSHMMgrSessionClearData->getTotal());
#ifndef _DF_LOCK_
	m_pSHMMgrSessionClearLock->P();
	if (!m_pSHMMgrSessionClearIndex->get(lkey,&pValue)) {
		m_pSHMMgrSessionClearLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		unsigned int fValue = pValue;
		int indexpos = 0;

		while (pValue) {
			if (strcmp(m_pMgrSessionClearData[pValue].m_sSessionID,oSessionClear.m_sSessionID) == 0) {

				if (indexpos == 0) {//删除的数据为索引
					int pos = m_pMgrSessionClearData[pValue].m_iIndexNext;
					if (pos != 0)  {
						lkey = SHMDataMgr::changekey(changeSessionID(m_pMgrSessionClearData[pos].m_sSessionID,skey),m_pSHMMgrSessionClearData->getTotal());
						m_pSHMMgrSessionClearIndex->add(lkey,pos);
					} else {
						m_pSHMMgrSessionClearIndex->revokeIdx(lkey,pValue);
					}
					int iNext = m_pMgrSessionClearData[pValue].m_iNext;
					memset(m_pMgrSessionClearData[pValue].m_sSessionID,0,sizeof(m_pMgrSessionClearData[pValue].m_sSessionID));
					m_pMgrSessionClearData[pValue].m_iNext = iNext;
					m_pSHMMgrSessionClearLock->V();
					return 0;
				} else {
					int pos = m_pMgrSessionClearData[pValue].m_iIndexNext;
					m_pMgrSessionClearData[fValue].m_iIndexNext = pos;
					int iNext = m_pMgrSessionClearData[pValue].m_iNext;
					memset(m_pMgrSessionClearData[pValue].m_sSessionID,0,sizeof(m_pMgrSessionClearData[pValue].m_sSessionID));
					m_pMgrSessionClearData[pValue].m_iNext = iNext;
					m_pSHMMgrSessionClearLock->V();
					return 0;
				}
			}
			indexpos++;
			fValue = pValue;
			pValue = m_pMgrSessionClearData[pValue].m_iIndexNext;
		}
		m_pSHMMgrSessionClearLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	}
#else
	if (!m_pSHMMgrSessionClearIndex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		unsigned int fValue = pValue;
		int indexpos = 0;

		while (pValue) {
			if (strcmp(m_pMgrSessionClearData[pValue].m_sSessionID,oSessionClear.m_sSessionID) == 0) {
				if (indexpos == 0) {//删除的数据为索引
					int pos = m_pMgrSessionClearData[pValue].m_iIndexNext;
					if (pos != 0)  {
						lkey = SHMDataMgr::changekey(changeSessionID(m_pMgrSessionClearData[pos].m_sSessionID,skey),m_pSHMMgrSessionClearData->getTotal());
						m_pSHMMgrSessionClearIndex->add(lkey,pos);
					} else {
						m_pSHMMgrSessionClearIndex->revokeIdx(lkey,pValue);
					}
					int iNext = m_pMgrSessionClearData[pValue].m_iNext;
					memset(m_pMgrSessionClearData[pValue].m_sSessionID,0,sizeof(m_pMgrSessionClearData[pValue].m_sSessionID));
					m_pMgrSessionClearData[pValue].m_iNext = iNext;
					return 0;
				} else {
					int pos = m_pMgrSessionClearData[pValue].m_iIndexNext;
					m_pMgrSessionClearData[fValue].m_iIndexNext = pos;
					int iNext = m_pMgrSessionClearData[pValue].m_iNext;
					memset(m_pMgrSessionClearData[pValue].m_sSessionID,0,sizeof(m_pMgrSessionClearData[pValue].m_sSessionID));
					m_pMgrSessionClearData[pValue].m_iNext = iNext;
					return 0;
				}
			}
			indexpos++;
			fValue = pValue;
			pValue = m_pMgrSessionClearData[pValue].m_iIndexNext;
		}
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	}
#endif
}

int SHMSessionClear::getSessionClear(SessionClearData &oSessionClear)
{
	char skey[12] = {0};
	long lkey = 0;
	unsigned int pValue = 0;
	lkey = SHMDataMgr::changekey(changeSessionID(oSessionClear.m_sSessionID,skey),m_pSHMMgrSessionClearData->getTotal());

	if (!m_pSHMMgrSessionClearIndex->get(lkey,&pValue)) {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	} else {
		while (pValue) {
			if (strcmp(m_pMgrSessionClearData[pValue].m_sSessionID,oSessionClear.m_sSessionID) == 0) {
				memcpy(&oSessionClear,&m_pMgrSessionClearData[pValue],sizeof(SessionClearData));
				return 0;
			}
			pValue = m_pMgrSessionClearData[pValue].m_iIndexNext;
		}
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	}
}

int SHMSessionClear::getSessionClearAll(vector<SessionClearData> &vSessionClear,long _lTime)
{
	vSessionClear.clear();
	int i = getPreDeal();

	m_pSHMMgrSessionClearLock->P();
	for (; i<m_pSHMMgrSessionClearData->getTotal();i++) {

		if (m_pMgrSessionClearData[i].m_lTime<=_lTime) {
			if (strlen(m_pMgrSessionClearData[i].m_sSessionID) == 0 ) {
				continue;
			} else {
				vSessionClear.push_back(m_pMgrSessionClearData[i]);
			}
		} else {
			setPreDeal(i);
			m_pSHMMgrSessionClearLock->V();
			return vSessionClear.size();
		}
	}

	for (i = 1; i<getPreDeal();i++) {

		if (m_pMgrSessionClearData[i].m_lTime<=_lTime) {
			if (strlen(m_pMgrSessionClearData[i].m_sSessionID) == 0 ) {
				continue;
			} else {
				vSessionClear.push_back(m_pMgrSessionClearData[i]);
			}
		} else {
			setPreDeal(i);
			m_pSHMMgrSessionClearLock->V();
			return vSessionClear.size();
		}
	}
	m_pSHMMgrSessionClearLock->V();
	return vSessionClear.size();
}


int SHMOrgRoute::GetRouteOrgIDUserCode(char *org_area_code,int org_level_id,int &descOrgID )
{

	for (int i = 1; i<=m_pSHMMgrOrgRouteData->getCount(); i++) {
		if (strcmp(org_area_code,m_pMgrOrgRouteData[i].m_szOrgAreaCode) == 0 &&
			org_level_id == m_pMgrOrgRouteData[i].m_iOrgLevelID) {
				descOrgID =m_pMgrOrgRouteData[i].m_iOrgID ;
				break;
		}
	}
	return 0;
}

int SHMOrgRoute::GetRouteOrgID(int src_org_id,int org_level_id,int &descOrgID)
{
	unsigned int value = 0;
	if (!m_pSHMMgrOrgRouteindex->get(src_org_id,&value)) {
		return -1;
	}

	while((m_pMgrOrgRouteData[value].m_iOrgLevelID != org_level_id)
		&&(src_org_id!=-1)){
			src_org_id = m_pMgrOrgRouteData[value].m_iParentOrgID;

			if (!m_pSHMMgrOrgRouteindex->get(src_org_id,&value)) {
				return -1;
			}
			src_org_id = m_pMgrOrgRouteData[value].m_iParentOrgID;
	}
	descOrgID = m_pMgrOrgRouteData[value].m_iOrgID;
	return 0;
}

int SHMOrgRoute::UpdateRouteOrg(int org_id,int org_level_id,int parent_org_id,char *org_area_code)
{
	int iRet = 0;
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrOrgRouteLock->P();

	if (!m_pSHMMgrOrgRouteindex->get(org_id,&value)) {
		int i = m_pSHMMgrOrgRouteData->malloc();
		if (i == 0) {
			m_pSHMMgrOrgRouteLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		m_pMgrOrgRouteData[i].m_iOrgID = org_id;
		m_pMgrOrgRouteData[i].m_iOrgLevelID = org_level_id;
		m_pMgrOrgRouteData[i].m_iParentOrgID = parent_org_id;
		strncpy(m_pMgrOrgRouteData[i].m_szOrgAreaCode,org_area_code,sizeof(m_pMgrOrgRouteData[i].m_szOrgAreaCode));
	} else {
		m_pMgrOrgRouteData[value].m_iOrgID = org_id;
		m_pMgrOrgRouteData[value].m_iOrgLevelID = org_level_id;
		m_pMgrOrgRouteData[value].m_iParentOrgID = parent_org_id;
		strncpy(m_pMgrOrgRouteData[value].m_szOrgAreaCode,org_area_code,sizeof(m_pMgrOrgRouteData[value].m_szOrgAreaCode));
	}
	m_pSHMMgrOrgRouteLock->V();
	return 0;
#else

	if (!m_pSHMMgrOrgRouteindex->get(org_id,&value)) {
		int i = m_pSHMMgrOrgRouteData->malloc();
		if (i == 0) {
			return MBC_MEM_SPACE_SMALL;
		}
		m_pMgrOrgRouteData[i].m_iOrgID = org_id;
		m_pMgrOrgRouteData[i].m_iOrgLevelID = org_level_id;
		m_pMgrOrgRouteData[i].m_iParentOrgID = parent_org_id;
		strncpy(m_pMgrOrgRouteData[i].m_szOrgAreaCode,org_area_code,sizeof(m_pMgrOrgRouteData[i].m_szOrgAreaCode));
	} else {
		m_pMgrOrgRouteData[value].m_iOrgID = org_id;
		m_pMgrOrgRouteData[value].m_iOrgLevelID = org_level_id;
		m_pMgrOrgRouteData[value].m_iParentOrgID = parent_org_id;
		strncpy(m_pMgrOrgRouteData[value].m_szOrgAreaCode,org_area_code,sizeof(m_pMgrOrgRouteData[value].m_szOrgAreaCode));
	}
	return 0;
#endif
}

int SHMOrgRoute::ExpRouteOrg(char *sTable_name)
{
	DEFINE_SGW_QUERY(query);
	char sSQL[1024] = {0};
	try {
		int size = sizeof(Sgw_Org_Route);
#ifndef _DF_LOCK_
		m_pSHMMgrOrgRouteLock->P();


#ifndef __SQLLITE__
		snprintf(sSQL,sizeof(sSQL)-1,"insert into %s (ORG_ID,ORG_LEVEL_ID,NAME, \
									 PARENT_ORG_ID,ORG_CODE,ORG_AREA_CODE \
									 ) values (:p1,:p2,'未知',:p3,0,:p4)",sTable_name);
		query.setSQL(sSQL);
		query.setParamArray("p1",&m_pMgrOrgRouteData[1].m_iOrgID,size);
		query.setParamArray("p2",&m_pMgrOrgRouteData[1].m_iOrgLevelID,size);
		query.setParamArray("p3",&m_pMgrOrgRouteData[1].m_iParentOrgID,size);
		query.setParamArray("p4",(char **)&m_pMgrOrgRouteData[1].m_szOrgAreaCode,size,sizeof(m_pMgrOrgRouteData[1].m_szOrgAreaCode));
		query.execute(m_pSHMMgrOrgRouteData->getCount());
		query.commit();

#else
		for (int i =1; i<=m_pSHMMgrOrgRouteData->getCount(); i++) {
			memset(sSQL,0,sizeof(sSQL));
			snprintf(sSQL,sizeof(sSQL)-1,"insert into %s (ORG_ID,ORG_LEVEL_ID,NAME, \
										 PARENT_ORG_ID,ORG_CODE,ORG_AREA_CODE \
										 ) values (%d,%d,'未知',%d,0,'%s')",sTable_name,
										 m_pMgrOrgRouteData[i].m_iOrgID,
										 m_pMgrOrgRouteData[i].m_iOrgLevelID,
										 m_pMgrOrgRouteData[i].m_iParentOrgID,
										 m_pMgrOrgRouteData[i].m_szOrgAreaCode);
			query.setSQL(sSQL);
			query.execute();
			query.commit();
		}

		m_pSHMMgrOrgRouteLock->V();
#endif
#else
		DEFINE_SGW_QUERY(query);

#ifndef __SQLLITE__
		snprintf(sSQL,sizeof(sSQL)-1,"insert into %s (ORG_ID,ORG_LEVEL_ID,NAME, \
									 PARENT_ORG_ID,ORG_CODE,ORG_AREA_CODE \
									 ) values (:p1,:p2,'未知',:p4,0,:p6)",sTable_name);
		query.setSQL(sSQL);

		for (int i =1; i<=m_pSHMMgrOrgRouteData->getCount(); i++) {
			query.setParameter("p1",m_pMgrOrgRouteData[i].m_iOrgID);
			query.setParameter("p2",m_pMgrOrgRouteData[i].m_iOrgLevelID);
			query.setParameter("p4",m_pMgrOrgRouteData[i].m_iParentOrgID);
			query.setParameter("p6",m_pMgrOrgRouteData[i].m_szOrgAreaCode);
			query.execute();
			query.commit();
		}
#else
		for (int i =1; i<=m_pSHMMgrOrgRouteData->getCount(); i++) {
			memset(sSQL,0,sizeof(sSQL));
			snprintf(sSQL,sizeof(sSQL)-1,"insert into %s (ORG_ID,ORG_LEVEL_ID,NAME, \
										 PARENT_ORG_ID,ORG_CODE,ORG_AREA_CODE \
										 ) values (%d,%d,'未知',%d,0,'%s')",sTable_name,
										 m_pMgrOrgRouteData[i].m_iOrgID,
										 m_pMgrOrgRouteData[i].m_iOrgLevelID,
										 m_pMgrOrgRouteData[i].m_iParentOrgID,
										 m_pMgrOrgRouteData[i].m_szOrgAreaCode);
			query.setSQL(sSQL);
			query.execute();
			query.commit();
		}
#endif
#endif
	}catch (TOCIException & toe){
		cout<<toe.getErrCode()<<toe.getErrMsg()<<toe.getErrSrc()<<endl;
		query.rollback();
		return -1;
	}
	return 0;
}

int SHMOrgHead::GetOrgIDbyNbr(char *AccNbr,int &Org_Id)
{
	unsigned int value = 0;

	if (m_pSHMMgrOrgHeadindex->get(SHMDataMgr::trim(AccNbr),&value)) {
		Org_Id = m_pMgrOrgHeadData[value].m_iOrgID;
	} else {
		Org_Id = -1;
	}
	return Org_Id;
}

int SHMOrgHead::UpdateAccNbrHead(int org_id,char *head,char *eff_date,char *exp_date,char *table_name)
{
#ifndef _DF_LOCK_
	m_pSHMMgrOrgHeadLock->P();
	unsigned int value = 0;

	if (m_pSHMMgrOrgHeadindex->get(SHMDataMgr::trim(head),&value)) {
		m_pMgrOrgHeadData[value].m_iOrgID =org_id ;
		strncpy(m_pMgrOrgHeadData[value].m_szEffDate,eff_date,sizeof(m_pMgrOrgHeadData[value].m_szEffDate)-1);
		strncpy(m_pMgrOrgHeadData[value].m_szExpDate,exp_date,sizeof(m_pMgrOrgHeadData[value].m_szExpDate)-1);
		strncpy(m_pMgrOrgHeadData[value].m_szTableNmae,table_name,sizeof(m_pMgrOrgHeadData[value].m_szTableNmae)-1);
	} else {
		int i = m_pSHMMgrOrgHeadData->malloc();
		if (i == 0) {
			m_pSHMMgrOrgHeadLock->V();
			return MBC_MEM_SPACE_SMALL;
		}
		m_pMgrOrgHeadData[i].m_iOrgID =org_id ;
		strncpy(m_pMgrOrgHeadData[i].m_szEffDate,eff_date,sizeof(m_pMgrOrgHeadData[i].m_szEffDate)-1);
		strncpy(m_pMgrOrgHeadData[i].m_szExpDate,exp_date,sizeof(m_pMgrOrgHeadData[i].m_szExpDate)-1);
		strncpy(m_pMgrOrgHeadData[i].m_szTableNmae,table_name,sizeof(m_pMgrOrgHeadData[i].m_szTableNmae)-1);
	}
	m_pSHMMgrOrgHeadLock->V();
	return 0;
#else
	unsigned int value = 0;

	if (m_pSHMMgrOrgHeadindex->get(SHMDataMgr::trim(head),&value)) {
		m_pMgrOrgHeadData[value].m_iOrgID =org_id ;
		strncpy(m_pMgrOrgHeadData[value].m_szEffDate,eff_date,sizeof(m_pMgrOrgHeadData[value].m_szEffDate)-1);
		strncpy(m_pMgrOrgHeadData[value].m_szExpDate,exp_date,sizeof(m_pMgrOrgHeadData[value].m_szExpDate)-1);
		strncpy(m_pMgrOrgHeadData[value].m_szTableNmae,table_name,sizeof(m_pMgrOrgHeadData[value].m_szTableNmae)-1);
	} else {
		int i = m_pSHMMgrOrgHeadData->malloc();
		if (i == 0) {
			return MBC_MEM_SPACE_SMALL;
		}
		m_pMgrOrgHeadData[i].m_iOrgID =org_id ;
		strncpy(m_pMgrOrgHeadData[i].m_szEffDate,eff_date,sizeof(m_pMgrOrgHeadData[i].m_szEffDate)-1);
		strncpy(m_pMgrOrgHeadData[i].m_szExpDate,exp_date,sizeof(m_pMgrOrgHeadData[i].m_szExpDate)-1);
		strncpy(m_pMgrOrgHeadData[i].m_szTableNmae,table_name,sizeof(m_pMgrOrgHeadData[i].m_szTableNmae)-1);
	}
	return 0;
#endif
}

int SHMOrgHead::ExpAccNbrHead(char *sTable_name)
{
	DEFINE_SGW_QUERY(query);
	try{
		char sSQL[5000] = {0};
		int size = sizeof(Sgw_Org_Head);
#ifndef _DF_LOCK_
		m_pSHMMgrOrgHeadLock->P();
#ifndef __SQLLITE__
		snprintf(sSQL,sizeof(sSQL)-1,"insert into %s (HEAD_ID,HEAD,EFF_DATE,EXP_DATE,TABLE_NAME \
									 ) values (HEAD_ID_SEQ.nextval,:p1,to_date(:p2,'YYYYMMDD'), \
									 to_date(:p3,'YYYYMMDD'),:p4)",sTable_name);
		query.setSQL(sSQL);

		query.setParamArray("p1",(char **)&m_pMgrOrgHeadData[1].m_szHead,size,sizeof(m_pMgrOrgHeadData[1].m_szHead));
		query.setParamArray("p2",(char **)&m_pMgrOrgHeadData[1].m_szEffDate,size,sizeof(m_pMgrOrgHeadData[1].m_szEffDate));
		query.setParamArray("p3",(char **)&m_pMgrOrgHeadData[1].m_szExpDate,size,sizeof(m_pMgrOrgHeadData[1].m_szExpDate));
		query.setParamArray("p4",(char **)&m_pMgrOrgHeadData[1].m_szTableNmae,size,sizeof(m_pMgrOrgHeadData[1].m_szTableNmae));
		query.execute(m_pSHMMgrOrgHeadData->getCount());
		query.commit();

#else

		for (int i =1; i<=m_pSHMMgrOrgHeadData->getCount(); i++) {
			memset(sSQL,0,sizeof(sSQL));
			snprintf(sSQL,sizeof(sSQL)-1,"insert into %s (HEAD,EFF_DATE,EXP_DATE,TABLE_NAME \
										 ) values (%s,to_date('%s','YYYYMMDD'), \
										 to_date('%s','YYYYMMDD'),'%s')",
										 sTable_name,
										 m_pMgrOrgHeadData[i].m_szHead,
										 m_pMgrOrgHeadData[i].m_szEffDate,
										 m_pMgrOrgHeadData[i].m_szExpDate,
										 m_pMgrOrgHeadData[i].m_szTableNmae);

			query.setSQL(sSQL);
			query.execute();
			query.commit();
		}
#endif
		m_pSHMMgrOrgHeadLock->V();
#else
		DEFINE_SGW_QUERY(query);
#ifndef __SQLLITE__
		snprintf(sSQL,sizeof(sSQL)-1,"insert into %s (HEAD_ID,HEAD,EFF_DATE,EXP_DATE,TABLE_NAME \
									 ) values (HEAD_ID_SEQ.nextval,:p1,to_date(:p2,'YYYYMMDD'), \
									 to_date(:p3,'YYYYMMDD'),:p4)",sTable_name);
		query.setSQL(sSQL);

		query.setParamArray("p1",(char **)&m_pMgrOrgHeadData[1].m_szHead,size,sizeof(m_pMgrOrgHeadData[1].m_szHead));
		query.setParamArray("p2",(char **)&m_pMgrOrgHeadData[1].m_szEffDate,size,sizeof(m_pMgrOrgHeadData[1].m_szEffDate));
		query.setParamArray("p3",(char **)&m_pMgrOrgHeadData[1].m_szExpDate,size,sizeof(m_pMgrOrgHeadData[1].m_szExpDate));
		query.setParamArray("p4",(char **)&m_pMgrOrgHeadData[1].m_szTableNmae,size,sizeof(m_pMgrOrgHeadData[1].m_szTableNmae));
		query.execute(m_pSHMMgrOrgHeadData->getCount());
		query.commit();
#else

		for (int i =1; i<=m_pSHMMgrOrgHeadData->getCount(); i++) {
			memset(sSQL,0,sizeof(sSQL));
			snprintf(sSQL,sizeof(sSQL)-1,"insert into %s (HEAD,EFF_DATE,EXP_DATE,TABLE_NAME \
										 ) values (%s,to_date('%s','YYYYMMDD'), \
										 to_date('%s','YYYYMMDD'),'%s')",
										 sTable_name,
										 m_pMgrOrgHeadData[i].m_szHead,
										 m_pMgrOrgHeadData[i].m_szEffDate,
										 m_pMgrOrgHeadData[i].m_szExpDate,
										 m_pMgrOrgHeadData[i].m_szTableNmae);

			query.setSQL(sSQL);
			query.execute();
			query.commit();
		}
#endif
#endif
	}catch (TOCIException & toe){
		cout<<toe.getErrCode()<<toe.getErrMsg()<<toe.getErrSrc()<<endl;
		query.rollback();
		return -1;
	}
}

int SHMChannelInfo::GetChannelInfo(char * pszChannelID,SgwChannelInfo & sSgwChannelInfo)
{
	long lkey = 0;
	unsigned int value = 0;
	lkey = SHMDataMgr::changekey(pszChannelID,m_pSHMMgrChannelData->getTotal());

	if (!m_pSHMMgrChannelindex->get(lkey,&value)) {
		return -1;
	} else {
		while (value) {
			if (strcmp(m_pMgrChannelData[value].m_szChannelID,pszChannelID) == 0) {
				memcpy(&sSgwChannelInfo,&m_pMgrChannelData[value],sizeof(SgwChannelInfo));
			}
			value = m_pMgrChannelData[value].m_iNext;
		}
		return 0;
	}
}

int SHMChannelInfo::DeleteChannelInfo(char * pszChannelID)
{
	unsigned int value = 0;
	long lkey = SHMDataMgr::changekey(pszChannelID,m_pSHMMgrChannelData->getTotal());

#ifndef _DF_LOCK_
	m_pSHMMgrChannelLock->P();
	if (!m_pSHMMgrChannelindex->get(lkey,&value)) {
		m_pSHMMgrChannelLock->V();
		return -1;
	} else {
		int fvalue = value;
		int _iCount =0;

		while (value) {
			if (strcmp(m_pMgrChannelData[value].m_szChannelID,pszChannelID) == 0) {

				if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
					int pos = m_pMgrChannelData[value].m_iNext;
					if (pos !=0) {
						lkey = SHMDataMgr::changekey(m_pMgrChannelData[pos].m_szChannelID,m_pSHMMgrChannelData->getTotal());
						m_pSHMMgrChannelindex->add(lkey,pos);
					}  else {
						m_pSHMMgrChannelindex->revokeIdx(lkey,value);
					}
					m_pSHMMgrChannelData->erase(value);
					memset(&m_pMgrChannelData[value],0,sizeof(SgwChannelInfo ));
					m_pSHMMgrChannelLock->V();
					return 0;
				} else {
					int pos = m_pMgrChannelData[value].m_iNext;

					m_pMgrChannelData[fvalue].m_iNext = pos;
					m_pSHMMgrChannelData->erase(value);
					memset(&m_pMgrChannelData[value],0,sizeof(SgwChannelInfo ));
					m_pSHMMgrChannelLock->V();
					return 0;
				}
			}
			fvalue = value;
			value = m_pMgrChannelData[value].m_iNext;
			_iCount++;
		}
		m_pSHMMgrChannelLock->V();
		return 0;
	}
#else
	if (!m_pSHMMgrChannelindex->get(lkey,&value)) {
		return -1;
	} else {
		int fvalue = value;
		int _iCount =0;

		while (value) {
			if (strcmp(m_pMgrChannelData[value].m_szChannelID,pszChannelID) == 0) {

				if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
					int pos = m_pMgrChannelData[value].m_iNext;
					if (pos !=0) {
						lkey = SHMDataMgr::changekey(m_pMgrChannelData[pos].m_szChannelID,m_pSHMMgrChannelData->getTotal());
						m_pSHMMgrChannelindex->add(lkey,pos);
					}  else {
						m_pSHMMgrChannelindex->revokeIdx(lkey,value);
					}
					m_pSHMMgrChannelData->erase(value);
					memset(&m_pMgrChannelData[value],0,sizeof(SgwChannelInfo ));
					return 0;
				} else {
					int pos = m_pMgrChannelData[value].m_iNext;

					m_pMgrChannelData[fvalue].m_iNext = pos;
					m_pSHMMgrChannelData->erase(value);
					memset(&m_pMgrChannelData[value],0,sizeof(SgwChannelInfo ));
					return 0;
				}
			}
			fvalue = value;
			value = m_pMgrChannelData[value].m_iNext;
			_iCount++;
		}
		return 0;
	}
#endif

}

int SHMChannelInfo::AddChannelInfo(const SgwChannelInfo & sSgwChannelInfo)
{
	unsigned int value  = 0;
	long lkey = SHMDataMgr::changekey(sSgwChannelInfo.m_szChannelID,m_pSHMMgrChannelData->getTotal());

#ifndef _DF_LOCK_
	m_pSHMMgrChannelLock->P();
	if (m_pSHMMgrChannelindex->get(lkey,&value)) {
		int fvalue = value ;

		while (value) {
			if (strcmp(sSgwChannelInfo.m_szChannelID,m_pMgrChannelData[value].m_szChannelID) == 0) {
				m_pSHMMgrChannelLock->V();
				return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			value = m_pMgrChannelData[value].m_iNext;
		}
		int i = m_pSHMMgrChannelData->malloc();

		if (i == 0) {
			m_pSHMMgrChannelLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrChannelData[i],&sSgwChannelInfo,sizeof(SgwChannelInfo));
		int pos = m_pMgrChannelData[fvalue].m_iNext;
		m_pMgrChannelData[i].m_iNext = pos;
		m_pMgrChannelData[fvalue].m_iNext = i;
		m_pSHMMgrChannelLock->V();
		return 0;
	} else {
		int i = m_pSHMMgrChannelData->malloc();

		if (i == 0) {
			m_pSHMMgrChannelLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrChannelData[i],&sSgwChannelInfo,sizeof(SgwChannelInfo));
		m_pSHMMgrChannelindex->add(lkey,i);
		m_pMgrChannelData[i].m_iNext = 0;
		m_pSHMMgrChannelLock->V();
		return 0;
	}
#else

	if (m_pSHMMgrChannelindex->get(lkey,&value)) {
		int fvalue = value ;

		while (value) {
			if (strcmp(sSgwChannelInfo.m_szChannelID,m_pMgrChannelData[value].m_szChannelID) == 0) {
				return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			value = m_pMgrChannelData[value].m_iNext;
		}
		int i = m_pSHMMgrChannelData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrChannelData[i],&sSgwChannelInfo,sizeof(SgwChannelInfo));
		int pos = m_pMgrChannelData[fvalue].m_iNext;
		m_pMgrChannelData[i].m_iNext = pos;
		m_pMgrChannelData[fvalue].m_iNext = i;
		return 0;
	} else {
		int i = m_pSHMMgrChannelData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrChannelData[i],&sSgwChannelInfo,sizeof(SgwChannelInfo));
		m_pMgrChannelData[i].m_iNext = 0;
		return 0;
	}
#endif

}

int SHMChannelInfo::UpdateChannelInfo(char * pszChannelID,SgwChannelInfo & sSgwChannelInfo)
{
	unsigned int value  = 0;
	long lkey = SHMDataMgr::changekey(pszChannelID,m_pSHMMgrChannelData->getTotal());

#ifndef _DF_LOCK_
	m_pSHMMgrChannelLock->P();
	if (!m_pSHMMgrChannelindex->get(lkey,&value)) {
		m_pSHMMgrChannelLock->V();
		return -1;
	} else {
		int iNext = m_pMgrChannelData[value].m_iNext;
		memcpy(&m_pMgrChannelData[value],&sSgwChannelInfo,sizeof(SgwChannelInfo));
		m_pMgrChannelData[value].m_iNext = iNext;
		m_pSHMMgrChannelLock->V();
		return 0;
	}
#else

	if (!m_pSHMMgrChannelindex->get(lkey,&value)) {
		return -1;
	} else {
		int iNext = m_pMgrChannelData[value].m_iNext;
		memcpy(&m_pMgrChannelData[value],&sSgwChannelInfo,sizeof(SgwChannelInfo));
		m_pMgrChannelData[value].m_iNext = iNext;
		return 0;
	}
#endif
}

//int SHMStaffInfo::GetStaffInfo(long lStaffID,SgwStaffInfo& sSgwStaffInfo)
//{
//	unsigned int value = 0;
//
//	if (!m_pSHMMgrStaffindex->get(lStaffID,&value)) {
//		return -1;
//	} else {
//		memcpy(&sSgwStaffInfo,&m_pMgrStaffData[value],sizeof(SgwStaffInfo));
//		return 0;
//	}
//}
//
//int SHMStaffInfo::AddStaffInfo(const SgwStaffInfo& sSgwStaffInfo)
//{
//	unsigned int value = 0;
//#ifndef _DF_LOCK_
//	m_pSHMMgrStaffLock->P();
//
//	if (!m_pSHMMgrStaffindex->get(sSgwStaffInfo.m_lStaffID,&value)) {
//		int i = m_pSHMMgrStaffData->malloc();
//
//		if (i == 0) {
//			m_pSHMMgrStaffLock->V();
//			return MBC_MEM_MALLOC_FAIL;
//		}
//		memcpy(&m_pMgrStaffData[i],&sSgwStaffInfo,sizeof(SgwStaffInfo));
//		m_pSHMMgrStaffindex->add(m_pMgrStaffData[i].m_lStaffID,i);
//		m_pSHMMgrStaffLock->V();
//		return 0;
//	} else {
//		m_pSHMMgrStaffLock->V();
//		return MBC_SHM_ADD_DATA_EXIST;
//	}
//#else
//
//	if (!m_pSHMMgrStaffindex->get(sSgwStaffInfo.m_lStaffID,&value)) {
//		int i = m_pSHMMgrStaffData->malloc();
//
//		if (i == 0) {
//			return MBC_MEM_MALLOC_FAIL;
//		}
//		memcpy(&m_pMgrStaffData[i],&sSgwStaffInfo,sizeof(SgwStaffInfo));
//		m_pSHMMgrStaffindex->add(m_pMgrStaffData[i].m_lStaffID,i);
//		return 0;
//	} else {
//		return MBC_SHM_ADD_DATA_EXIST;
//	}
//#endif
//}
//
//int SHMStaffInfo::UpdateStaffInfo(long lStaffID,SgwStaffInfo& sSgwStaffInfo)
//{
//	unsigned int value = 0;
//#ifndef _DF_LOCK_
//	m_pSHMMgrStaffLock->P();
//
//	if (!m_pSHMMgrStaffindex->get(lStaffID,&value)) {
//		m_pSHMMgrStaffLock->V();
//		return -1;
//	} else {
//		memcpy(&m_pMgrStaffData[value],&sSgwStaffInfo,sizeof(SgwStaffInfo));
//		m_pSHMMgrStaffLock->V();
//		return 0;
//	}
//#else
//
//	if (!m_pSHMMgrStaffindex->get(lStaffID,&value)) {
//		return -1;
//	} else {
//		memcpy(&m_pMgrStaffData[value],&sSgwStaffInfo,sizeof(SgwStaffInfo));
//		return 0;
//	}
//#endif
//}
//
//int SHMStaffInfo::DeleteStaffInfo(long lStaffID)
//{
//	unsigned int value = 0;
//#ifndef _DF_LOCK_
//	m_pSHMMgrStaffLock->P();
//
//	if (!m_pSHMMgrStaffindex->get(lStaffID,&value)) {
//		m_pSHMMgrStaffLock->V();
//		return -1;
//	} else {
//		m_pSHMMgrStaffData->erase(value);
//		m_pSHMMgrStaffindex->revokeIdx(lStaffID,value);
//		memset(&m_pMgrStaffData[value],0,sizeof(SgwStaffInfo));
//		m_pSHMMgrStaffLock->V();
//		return 0;
//	}
//#else
//
//	if (!m_pSHMMgrStaffindex->get(lStaffID,&value)) {
//		return -1;
//	} else {
//		m_pSHMMgrStaffData->erase(value);
//		memcpy(&m_pMgrStaffData[value],0,sizeof(SgwStaffInfo));
//		return 0;
//	}
//#endif
//}

//int SHMNetChannelInfo::GetNetChannelByNet(char * pszNetInfoCode,SgwNetChannel& sSgwNetChannel)
//{
//	long lkey = SHMDataMgr::changekey(pszNetInfoCode,m_pSHMMgrNetChannelData->getTotal());
//	unsigned int value = 0;
//
//	if (m_pSHMMgrNetChannelCodeindex->get(lkey,&value)) {
//		while (value) {
//			if (strcmp(m_pMgrNetChannelData[value].m_szNetInfoCode,pszNetInfoCode) == 0) {
//				memcpy(&sSgwNetChannel,&m_pMgrNetChannelData[value],sizeof(SgwNetChannel));
//				return 0;
//			}
//			value = m_pMgrNetChannelData[value].m_iCodeNext;
//		}
//	}
//	return MBC_SHM_QUERY_NO_DATE_FOUND;
//}
//
//int SHMNetChannelInfo::GetNetChannelByChannel(char * pszChannel,SgwNetChannel& sSgwNetChannel)
//{
//	long lkey = SHMDataMgr::changekey(pszChannel,m_pSHMMgrNetChannelData->getTotal());
//	unsigned int value = 0;
//
//	if (m_pSHMMgrNetChannelindex->get(lkey,&value)) {
//		while (value) {
//			if (strcmp(m_pMgrNetChannelData[value].m_szChannelID,pszChannel) == 0) {
//				memcpy(&sSgwNetChannel,&m_pMgrNetChannelData[value],sizeof(SgwNetChannel));
//				return 0;
//			}
//			value = m_pMgrNetChannelData[value].m_iChannelNext;
//		}
//	}
//	return -1;
//}
//
//int SHMNetChannelInfo::AddNetChannel(const SgwNetChannel& sSgwNetChannel)
//{
//	unsigned int value = 0;
//	long lkey = 0;
//#ifndef _DF_LOCK_
//	m_pSHMMgrNetChannelLock->P();
//
//	if (!m_pSHMMgrNetChannelNetIDindex->get(sSgwNetChannel.m_iChannelNetID,&value)) {
//		int i = m_pSHMMgrNetChannelData->malloc();
//		if (i == 0) {
//			m_pSHMMgrNetChannelLock->V();
//			return MBC_MEM_MALLOC_FAIL;
//		}
//		memcpy(&m_pMgrNetChannelData[i],&sSgwNetChannel,sizeof(SgwNetChannel));
//
//		//更新之前先删除已原来数据建立的索引
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[i].m_szChannelID,m_pSHMMgrNetChannelData->getTotal());
//		unsigned int temp = 0;
//
//		if (m_pSHMMgrNetChannelindex->get(lkey,&temp)) {
//			int pos = m_pMgrNetChannelData[temp].m_iChannelNext;
//			m_pMgrNetChannelData[i].m_iChannelNext = pos;
//			m_pMgrNetChannelData[temp].m_iChannelNext = i;
//		} else {
//			m_pSHMMgrNetChannelindex->add(lkey,i);
//			m_pMgrNetChannelData[i].m_iChannelNext = 0;
//		}
//
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[i].m_szNetInfoCode,m_pSHMMgrNetChannelData->getTotal());
//
//		temp = 0;
//
//		if (m_pSHMMgrNetChannelCodeindex->get(lkey,&temp)) {
//			int pos = m_pMgrNetChannelData[temp].m_iCodeNext;
//			m_pMgrNetChannelData[i].m_iCodeNext = pos;
//			m_pMgrNetChannelData[temp].m_iCodeNext = i;
//		} else {
//			m_pSHMMgrNetChannelCodeindex->add(lkey,i);
//			m_pMgrNetChannelData[i].m_iCodeNext = 0;
//		}
//		m_pSHMMgrNetChannelNetIDindex->add(sSgwNetChannel.m_iChannelNetID,i);
//		m_pSHMMgrNetChannelLock->V();
//		return 0;
//	}
//	m_pSHMMgrNetChannelLock->V();
//	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
//#else
//
//
//	if (!m_pSHMMgrNetChannelNetIDindex->get(sSgwNetChannel.m_iChannelNetID,&value)) {
//		int i = m_pSHMMgrNetChannelData->malloc();
//		if (i == 0) {
//			return MBC_MEM_MALLOC_FAIL;
//		}
//		memcpy(&m_pMgrNetChannelData[i],&sSgwNetChannel,sizeof(SgwNetChannel));
//
//		//更新之前先删除已原来数据建立的索引
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[i].m_szChannelID,m_pSHMMgrNetChannelData->getTotal());
//		unsigned int temp = 0;
//
//		if (m_pSHMMgrNetChannelindex->get(lkey,&temp)) {
//			int pos = m_pMgrNetChannelData[temp].m_iChannelNext;
//			m_pMgrNetChannelData[i].m_iChannelNext = pos;
//			m_pMgrNetChannelData[temp].m_iChannelNext = i;
//		} else {
//			m_pSHMMgrNetChannelindex->add(lkey,i);
//			m_pMgrNetChannelData[i].m_iChannelNext = 0;
//		}
//
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[i].m_szNetInfoCode,m_pSHMMgrNetChannelData->getTotal());
//
//		temp = 0;
//
//		if (m_pSHMMgrNetChannelCodeindex->get(lkey,&temp)) {
//			int pos = m_pMgrNetChannelData[temp].m_iCodeNext;
//			m_pMgrNetChannelData[i].m_iCodeNext = pos;
//			m_pMgrNetChannelData[temp].m_iCodeNext = i;
//		} else {
//			m_pSHMMgrNetChannelCodeindex->add(lkey,i);
//			m_pMgrNetChannelData[i].m_iCodeNext = 0;
//		}
//		m_pSHMMgrNetChannelNetIDindex->add(sSgwNetChannel.m_iChannelNetID,i);
//		return 0;
//	}
//	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
//#endif
//
//}
//
//int SHMNetChannelInfo::GetNetChannel(int iChannelNetID,SgwNetChannel& sSgwNetChannel)
//{
//	unsigned int value = 0;
//
//	if (m_pSHMMgrNetChannelNetIDindex->get(iChannelNetID,&value)) {
//		memcpy(&sSgwNetChannel,&m_pMgrNetChannelData[value],sizeof(SgwNetChannel));
//		return 0;
//	}
//	return MBC_SHM_QUERY_NO_DATE_FOUND;
//}
//
//int SHMNetChannelInfo::UpdateNetChannel(int iChannelNetID,SgwNetChannel& sSgwNetChannel)
//{
//	unsigned int value = 0;
//	long lkey = 0;
//#ifndef _DF_LOCK_
//	m_pSHMMgrNetChannelLock->P();
//
//	if (m_pSHMMgrNetChannelNetIDindex->get(iChannelNetID,&value)) {
//		//更新之前先删除已原来数据建立的索引
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[value].m_szChannelID,m_pSHMMgrNetChannelData->getTotal());
//		unsigned int temp = 0;
//		int count = 0;
//
//		if (m_pSHMMgrNetChannelindex->get(lkey,&temp)) {
//			int fvalue = temp;
//
//			while (temp) {
//				if (strcmp(m_pMgrNetChannelData[value].m_szChannelID,m_pMgrNetChannelData[temp].m_szChannelID)==0) {
//
//					if (count == 0 ) {//如果删除的是索引的首节点，那么就重置索引
//						int pos = m_pMgrNetChannelData[temp].m_iChannelNext;
//						if (pos !=0) {
//							lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[pos].m_szChannelID,m_pSHMMgrNetChannelData->getTotal());
//							m_pSHMMgrNetChannelindex->add(lkey,pos);
//						}  else {
//							m_pSHMMgrNetChannelindex->revokeIdx(lkey,temp);
//						}
//						break;
//					} else {
//						int pos = m_pMgrNetChannelData[temp].m_iChannelNext;
//						m_pMgrNetChannelData[fvalue].m_iChannelNext = pos;
//						break;
//					}
//				}
//				fvalue = temp;
//				temp = m_pMgrNetChannelData[temp].m_iChannelNext;
//				count++;
//			}
//		}
//
//		count = 0;
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[value].m_szNetInfoCode,m_pSHMMgrNetChannelData->getTotal());
//
//		temp = 0;
//
//		if (m_pSHMMgrNetChannelCodeindex->get(lkey,&temp)) {
//			int fvalue = temp;
//
//			while (temp) {
//				if (strcmp(m_pMgrNetChannelData[temp].m_szNetInfoCode,m_pMgrNetChannelData[value].m_szNetInfoCode) == 0) {
//
//
//					if (count == 0 ) {//如果删除的是索引的首节点，那么就重置索引
//						int pos = m_pMgrNetChannelData[temp].m_iCodeNext;
//
//						if (pos !=0) {
//							lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[pos].m_szNetInfoCode,m_pSHMMgrNetChannelData->getTotal());
//							m_pSHMMgrNetChannelCodeindex->add(lkey,pos);
//						}  else {
//							m_pSHMMgrNetChannelCodeindex->revokeIdx(lkey,temp);
//						}
//						break;
//					} else {
//						int pos = m_pMgrNetChannelData[temp].m_iCodeNext;
//						m_pMgrNetChannelData[fvalue].m_iCodeNext = pos;
//						break;
//					}
//				}
//				fvalue = temp;
//				temp = m_pMgrNetChannelData[temp].m_iCodeNext;
//				count++;
//			}
//		}
//
//		memcpy(&m_pMgrNetChannelData[value],&sSgwNetChannel,sizeof(SgwNetChannel));
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[value].m_szChannelID,m_pSHMMgrNetChannelData->getTotal());
//		temp = 0;
//
//		if(m_pSHMMgrNetChannelindex->get(lkey,&temp)) {
//			int pos = m_pMgrNetChannelData[temp].m_iChannelNext;
//			m_pMgrNetChannelData[temp].m_iChannelNext = value;
//			m_pMgrNetChannelData[value].m_iChannelNext = pos;
//		} else {
//			m_pSHMMgrNetChannelindex->add(lkey,value);
//			m_pMgrNetChannelData[value].m_iChannelNext = 0;
//		}
//
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[value].m_szNetInfoCode,m_pSHMMgrNetChannelData->getTotal());
//		temp = 0;
//
//		if(m_pSHMMgrNetChannelCodeindex->get(lkey,&temp)) {
//			int pos = m_pMgrNetChannelData[temp].m_iCodeNext;
//			m_pMgrNetChannelData[temp].m_iCodeNext = value;
//			m_pMgrNetChannelData[value].m_iCodeNext = pos;
//		} else {
//			m_pSHMMgrNetChannelCodeindex->add(lkey,value);
//			m_pMgrNetChannelData[value].m_iCodeNext = 0;
//		}
//		m_pSHMMgrNetChannelLock->V();
//		return 0;
//	}
//	m_pSHMMgrNetChannelLock->V();
//	return -1;
//#else
//
//	if (m_pSHMMgrNetChannelNetIDindex->get(iChannelNetID,&value)) {
//		//更新之前先删除已原来数据建立的索引
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[value].m_szChannelID,m_pSHMMgrNetChannelData->getTotal());
//		unsigned int temp = 0;
//		int count = 0;
//
//		if (m_pSHMMgrNetChannelindex->get(lkey,&temp)) {
//			int fvalue = temp;
//
//			while (temp) {
//				if (strcmp(m_pMgrNetChannelData[value].m_szChannelID,m_pMgrNetChannelData[temp].m_szChannelID)==0) {
//
//					if (count == 0 ) {//如果删除的是索引的首节点，那么就重置索引
//						int pos = m_pMgrNetChannelData[temp].m_iChannelNext;
//						if (pos !=0) {
//							lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[pos].m_szChannelID,m_pSHMMgrNetChannelData->getTotal());
//							m_pSHMMgrNetChannelindex->add(lkey,pos);
//						}  else {
//							m_pSHMMgrNetChannelindex->revokeIdx(lkey,temp);
//						}
//						break;
//					} else {
//						int pos = m_pMgrNetChannelData[temp].m_iChannelNext;
//						m_pMgrNetChannelData[fvalue].m_iChannelNext = pos;
//						break;
//					}
//				}
//				fvalue = temp;
//				temp = m_pMgrNetChannelData[temp].m_iChannelNext;
//				count++;
//			}
//		}
//
//		count = 0;
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[value].m_szNetInfoCode,m_pSHMMgrNetChannelData->getTotal());
//
//		temp = 0;
//
//		if (m_pSHMMgrNetChannelCodeindex->get(lkey,&temp)) {
//			int fvalue = temp;
//
//			while (temp) {
//				if (strcmp(m_pMgrNetChannelData[temp].m_szNetInfoCode,m_pMgrNetChannelData[value].m_szNetInfoCode) == 0) {
//
//
//					if (count == 0 ) {//如果删除的是索引的首节点，那么就重置索引
//						int pos = m_pMgrNetChannelData[temp].m_iCodeNext;
//
//						if (pos !=0) {
//							lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[pos].m_szNetInfoCode,m_pSHMMgrNetChannelData->getTotal());
//							m_pSHMMgrNetChannelCodeindex->add(lkey,pos);
//						}  else {
//							m_pSHMMgrNetChannelCodeindex->revokeIdx(lkey,temp);
//						}
//						break;
//					} else {
//						int pos = m_pMgrNetChannelData[temp].m_iCodeNext;
//						m_pMgrNetChannelData[fvalue].m_iCodeNext = pos;
//						break;
//					}
//				}
//				fvalue = temp;
//				temp = m_pMgrNetChannelData[temp].m_iCodeNext;
//				count++;
//			}
//		}
//
//		memcpy(&m_pMgrNetChannelData[value],&sSgwNetChannel,sizeof(SgwNetChannel));
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[value].m_szChannelID,m_pSHMMgrNetChannelData->getTotal());
//		temp = 0;
//
//		if(m_pSHMMgrNetChannelindex->get(lkey,&temp)) {
//			int pos = m_pMgrNetChannelData[temp].m_iChannelNext;
//			m_pMgrNetChannelData[temp].m_iChannelNext = value;
//			m_pMgrNetChannelData[value].m_iChannelNext = pos;
//		} else {
//			m_pSHMMgrNetChannelindex->add(lkey,value);
//			m_pMgrNetChannelData[value].m_iChannelNext = 0;
//		}
//
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[value].m_szNetInfoCode,m_pSHMMgrNetChannelData->getTotal());
//		temp = 0;
//
//		if(m_pSHMMgrNetChannelCodeindex->get(lkey,&temp)) {
//			int pos = m_pMgrNetChannelData[temp].m_iCodeNext;
//			m_pMgrNetChannelData[temp].m_iCodeNext = value;
//			m_pMgrNetChannelData[value].m_iCodeNext = pos;
//		} else {
//			m_pSHMMgrNetChannelCodeindex->add(lkey,value);
//			m_pMgrNetChannelData[value].m_iCodeNext = 0;
//		}
//		return 0;
//	}
//	return -1;
//#endif
//}
//
//int SHMNetChannelInfo::DeleteNetChannel(int iChannelNetID)
//{
//#ifndef _DF_LOCK_
//	m_pSHMMgrNetChannelLock->P();
//	unsigned int value = 0;
//	long lkey = 0;
//	int _iCount = 0;
//
//	if (m_pSHMMgrNetChannelNetIDindex->get(iChannelNetID,&value)) {
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[value].m_szChannelID,m_pSHMMgrNetChannelData->getTotal());
//
//		unsigned int temp = 0;
//
//		if (m_pSHMMgrNetChannelindex->get(lkey,&temp)) {
//			int fvalue = temp;
//
//			while (temp) {
//				if (strcmp(m_pMgrNetChannelData[temp].m_szChannelID,m_pMgrNetChannelData[value].m_szChannelID) == 0) {
//
//					if (temp != value) {
//						m_pSHMMgrNetChannelLock->V();
//						return -1;
//					}
//
//					if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
//						int pos = m_pMgrNetChannelData[temp].m_iChannelNext;
//						if (pos !=0) {
//							lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[pos].m_szChannelID,m_pSHMMgrNetChannelData->getTotal());
//							m_pSHMMgrNetChannelindex->add(lkey,pos);
//						}  else {
//							m_pSHMMgrNetChannelindex->revokeIdx(lkey,temp);
//						}
//						break;
//					} else {
//						int pos = m_pMgrNetChannelData[temp].m_iChannelNext;
//						m_pMgrNetChannelData[fvalue].m_iChannelNext = pos;
//						break;
//					}
//				}
//				fvalue = temp;
//				temp = m_pMgrNetChannelData[temp].m_iChannelNext;
//				_iCount++;
//			}
//		}
//
//		_iCount = 0;
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[value].m_szNetInfoCode,m_pSHMMgrNetChannelData->getTotal());
//
//		temp = 0;
//
//		if (m_pSHMMgrNetChannelCodeindex->get(lkey,&temp)) {
//			int fvalue = temp;
//
//			while (temp) {
//				if (strcmp(m_pMgrNetChannelData[temp].m_szNetInfoCode,m_pMgrNetChannelData[value].m_szNetInfoCode) == 0) {
//
//					if (temp != value) {
//						m_pSHMMgrNetChannelLock->V();
//						return -1;
//					}
//
//					if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
//						int pos = m_pMgrNetChannelData[temp].m_iCodeNext;
//
//						if (pos !=0) {
//							lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[pos].m_szNetInfoCode,m_pSHMMgrNetChannelData->getTotal());
//							m_pSHMMgrNetChannelCodeindex->add(lkey,pos);
//						}  else {
//							m_pSHMMgrNetChannelCodeindex->revokeIdx(lkey,temp);
//						}
//						break;
//					} else {
//						int pos = m_pMgrNetChannelData[temp].m_iCodeNext;
//						m_pMgrNetChannelData[fvalue].m_iCodeNext = pos;
//						break;
//					}
//				}
//				fvalue = temp;
//				temp = m_pMgrNetChannelData[temp].m_iCodeNext;
//				_iCount++;
//			}
//		}
//
//		m_pSHMMgrNetChannelData->erase(value);
//		memset(&m_pMgrNetChannelData[value],0,sizeof(SgwNetChannel));
//		m_pSHMMgrNetChannelLock->V();
//		return 0;
//	}
//	m_pSHMMgrNetChannelLock->V();
//	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
//#else
//	unsigned int value = 0;
//	long lkey = 0;
//	int _iCount = 0;
//
//	if (m_pSHMMgrNetChannelNetIDindex->get(iChannelNetID,&value)) {
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[value].m_szChannelID,m_pSHMMgrNetChannelData->getTotal());
//
//		unsigned int temp = 0;
//
//		if (m_pSHMMgrNetChannelindex->get(lkey,&temp)) {
//			int fvalue = temp;
//
//			while (temp) {
//				if (strcmp(m_pMgrNetChannelData[temp].m_szChannelID,m_pMgrNetChannelData[value].m_szChannelID) == 0) {
//
//					if (temp != value) {
//						return -1;
//					}
//
//					if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
//						int pos = m_pMgrNetChannelData[temp].m_iChannelNext;
//						if (pos !=0) {
//							lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[pos].m_szChannelID,m_pSHMMgrNetChannelData->getTotal());
//							m_pSHMMgrNetChannelindex->add(lkey,pos);
//						}  else {
//							m_pSHMMgrNetChannelindex->revokeIdx(lkey,temp);
//						}
//						break;
//					} else {
//						int pos = m_pMgrNetChannelData[temp].m_iChannelNext;
//						m_pMgrNetChannelData[fvalue].m_iChannelNext = pos;
//						break;
//					}
//				}
//				fvalue = temp;
//				temp = m_pMgrNetChannelData[temp].m_iChannelNext;
//				_iCount++;
//			}
//		}
//
//		_iCount = 0;
//		lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[value].m_szNetInfoCode,m_pSHMMgrNetChannelData->getTotal());
//
//		temp = 0;
//
//		if (m_pSHMMgrNetChannelCodeindex->get(lkey,&temp)) {
//			int fvalue = temp;
//
//			while (temp) {
//				if (strcmp(m_pMgrNetChannelData[temp].m_szNetInfoCode,m_pMgrNetChannelData[value].m_szNetInfoCode) == 0) {
//
//					if (temp != value) {
//						return -1;
//					}
//
//					if (_iCount == 0 ) {//如果删除的是索引的首节点，那么就重置索引
//						int pos = m_pMgrNetChannelData[temp].m_iCodeNext;
//
//						if (pos !=0) {
//							lkey = SHMDataMgr::changekey(m_pMgrNetChannelData[pos].m_szNetInfoCode,m_pSHMMgrNetChannelData->getTotal());
//							m_pSHMMgrNetChannelCodeindex->add(lkey,pos);
//						}  else {
//							m_pSHMMgrNetChannelCodeindex->revokeIdx(lkey,temp);
//						}
//						break;
//					} else {
//						int pos = m_pMgrNetChannelData[temp].m_iCodeNext;
//						m_pMgrNetChannelData[fvalue].m_iCodeNext = pos;
//						break;
//					}
//				}
//				fvalue = temp;
//				temp = m_pMgrNetChannelData[temp].m_iCodeNext;
//				_iCount++;
//			}
//		}
//
//		m_pSHMMgrNetChannelData->erase(value);
//		memcpy(&m_pMgrNetChannelData[value],0,sizeof(SgwNetChannel));
//		return 0;
//	}
//	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
//#endif
//}

int SHMUserStaff::GetUserStaffByStarr(int iStaffID,SgwUserStaff& sSgwUserStaff)
{
	unsigned int value  = 0;
	if (m_pSHMMgrStaffIDindex->get(iStaffID,&value)) {
		memcpy(&sSgwUserStaff,&m_pMgrUserStaffData[value],sizeof(SgwUserStaff));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMUserStaff::GetUserStaffByUserStarr(int iUserStaffID,SgwUserStaff& sSgwUserStaff)
{
	unsigned int value  = 0;

	if (m_pSHMMgrUserStaffIDindex->get(iUserStaffID,&value)) {
		memcpy(&sSgwUserStaff,&m_pMgrUserStaffData[value],sizeof(SgwUserStaff));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMUserStaff::GetUserStaffByChannel(char * pszChannelID,vector<SgwUserStaff>& vSgwUserStaff)
{
	unsigned int value  = 0;
	long lkey = 0;
	vSgwUserStaff.clear();
	lkey = SHMDataMgr::changekey(pszChannelID,m_pSHMMgrUserStaffData->getTotal());
	if (m_pSHMMgrUserStaffChannelIDindex->get(lkey,&value)) {
		while(value) {
			vSgwUserStaff.push_back(m_pMgrUserStaffData[value]);
		      value = m_pMgrUserStaffData[value].m_iNext;
		}
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMUserStaff::AddUserStaff(SgwUserStaff& sSgwUserStaff)
{
	unsigned int value = 0;
	long lkey = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrUserStaffLock->P();

	if (!m_pSHMMgrUserStaffIDindex->get(sSgwUserStaff.m_iUserStaffID,&value)){

		int i = m_pSHMMgrUserStaffData->malloc();

		if (i == 0) {
			m_pSHMMgrUserStaffLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrUserStaffData[i],&sSgwUserStaff,sizeof(SgwUserStaff));
		m_pSHMMgrStaffIDindex->add(m_pMgrUserStaffData[i].m_lStaffID,i);
		m_pSHMMgrUserStaffIDindex->add(sSgwUserStaff.m_iUserStaffID,i);

		lkey = SHMDataMgr::changekey(m_pMgrUserStaffData[i].m_szChannelID,m_pSHMMgrUserStaffData->getTotal());
		unsigned int temp = 0;
		if (m_pSHMMgrUserStaffChannelIDindex->get(lkey,&temp)){
			int pos = m_pMgrUserStaffData[temp].m_iNext;
			m_pMgrUserStaffData[temp].m_iNext = i;
			m_pMgrUserStaffData[i].m_iNext = pos;
		} else {
			m_pSHMMgrUserStaffChannelIDindex->add(lkey,i);
		}
		m_pSHMMgrUserStaffLock->V();
		return 0;
	}
	m_pSHMMgrUserStaffLock->V();
	return MBC_SHM_ADD_DATA_EXIST;
#else

	if (!m_pSHMMgrUserStaffIDindex->get(sSgwUserStaff.m_iUserStaffID,&value)){

		int i = m_pSHMMgrUserStaffData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrUserStaffData[i],&sSgwUserStaff,sizeof(SgwUserStaff));
		m_pSHMMgrStaffIDindex->add(m_pMgrUserStaffData[i].m_lStaffID,i);
		m_pSHMMgrUserStaffIDindex->add(sSgwUserStaff.m_iUserStaffID,i);

		lkey = SHMDataMgr::changekey(m_pMgrUserStaffData[i].m_szChannelID,m_pSHMMgrUserStaffData->getTotal());
		unsigned int temp = 0;
		if (m_pSHMMgrUserStaffChannelIDindex->get(lkey,&temp)){
			int pos = m_pMgrUserStaffData[temp].m_iNext;
			m_pMgrUserStaffData[temp].m_iNext = i;
			m_pMgrUserStaffData[i].m_iNext = pos;
		} else {
			m_pSHMMgrUserStaffChannelIDindex->add(lkey,i);
		}
		return 0;
	}
	return MBC_SHM_ADD_DATA_EXIST;
#endif
}

int SHMUserStaff::DeleteUserStaff(SgwUserStaff& sSgwUserStaff)
{
	unsigned int value = 0;
	long lkey = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrUserStaffLock->P();

	if (m_pSHMMgrUserStaffIDindex->revokeIdx(sSgwUserStaff.m_iUserStaffID,value)){
		lkey = SHMDataMgr::changekey(sSgwUserStaff.m_szChannelID,m_pSHMMgrUserStaffData->getTotal());
		unsigned int temp = 0;

		if (m_pSHMMgrUserStaffChannelIDindex->get(lkey,&temp)){
			unsigned int falue = temp;
			int num = 0;

			while (temp) {
				if (strcmp(m_pMgrUserStaffData[value].m_szChannelID,m_pMgrUserStaffData[temp].m_szChannelID)==0) {
					if (num == 0) {
						int pos = m_pMgrUserStaffData[temp].m_iNext;
						if (pos == 0) {
							m_pSHMMgrUserStaffChannelIDindex->revokeIdx(lkey,temp);
						} else {
							m_pSHMMgrUserStaffChannelIDindex->add(lkey,pos);
						}
					} else {
						int pos = m_pMgrUserStaffData[temp].m_iNext;
						m_pMgrUserStaffData[falue].m_iNext = pos;
					}
					break;
				}
				num++;
				falue = temp;
				temp = m_pMgrUserStaffData[temp].m_iNext;
			}
		}

		m_pSHMMgrStaffIDindex->revokeIdx(m_pMgrUserStaffData[value].m_lStaffID,value);
		m_pSHMMgrUserStaffData->erase(value);
		memset(&m_pMgrUserStaffData[value],0,sizeof(SgwUserStaff));
		m_pSHMMgrUserStaffLock->V();
		return 0;
	}
	m_pSHMMgrUserStaffLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else

	if (m_pSHMMgrUserStaffIDindex->revokeIdx(sSgwUserStaff.m_iUserStaffID,value)){
		lkey = SHMDataMgr::changekey(sSgwUserStaff.m_szChannelID,m_pSHMMgrUserStaffData->getTotal());
		unsigned int temp = 0;

		if (m_pSHMMgrUserStaffChannelIDindex->get(lkey,&temp)){
			unsigned int falue = temp;
			int num = 0;

			while (temp) {
				if (strcmp(m_pMgrUserStaffData[value].m_szChannelID,m_pMgrUserStaffData[temp].m_szChannelID)==0) {
					if (num == 0) {
						int pos = m_pMgrUserStaffData[temp].m_iNext;
						if (pos == 0) {
							m_pSHMMgrUserStaffChannelIDindex->revokeIdx(lkey,temp);
						} else {
							m_pSHMMgrUserStaffChannelIDindex->add(lkey,pos);
						}
					} else {
						int pos = m_pMgrUserStaffData[temp].m_iNext;
						m_pMgrUserStaffData[falue].m_iNext = pos;
					}
					break;
				}
				num++;
				falue = temp;
				temp = m_pMgrUserStaffData[temp].m_iNext;
			}
		}

		m_pSHMMgrStaffIDindex->revokeIdx(m_pMgrUserStaffData[value].m_lStaffID,value);
		m_pSHMMgrUserStaffData->erase(value);
		memset(&m_pMgrUserStaffData[value],0,sizeof(SgwUserStaff));
		return 0;
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif

}

int SHMUserStaff::UpdateUserStaff(int iUserStaffID,SgwUserStaff& sSgwUserStaff)
{
	unsigned int value = 0;
	int iRet = 0;
	if (m_pSHMMgrUserStaffIDindex->get(iUserStaffID,&value)) {
		iRet = DeleteUserStaff(m_pMgrUserStaffData[value]);
		if (iRet!=0) {
			return iRet;
		}
		iRet = AddUserStaff(sSgwUserStaff);
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
}

int SHMOrgRouteRule::GetOrgRouteRule(int _iRouteID,SgwOrgRouteRule& sOrgRouteRule)
{
	unsigned int value = 0;

	if (m_pSHMMgrOrgRouteIDindex->get(_iRouteID,&value)) {
		memcpy(&sOrgRouteRule,&m_pMgrOrgRoutRuleData[value],sizeof(SgwOrgRouteRule));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMOrgRouteRule::GetOrgRouteRuleByOrg(int iOrgID,SgwOrgRouteRule& sOrgRouteRule)
{
	unsigned int value = 0;

	if (m_pSHMMgrOrgIDindex->get(iOrgID,&value)) {
		while (value) {
			if (m_pMgrOrgRoutRuleData[value].m_iOrgID == iOrgID ) {
				memcpy(&sOrgRouteRule,&m_pMgrOrgRoutRuleData[value],sizeof(SgwOrgRouteRule));
				return 0;
			}
			value = m_pMgrOrgRoutRuleData[value].m_iNext;
		}
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMOrgRouteRule::AddOrgRouteRule(const SgwOrgRouteRule& sOrgRouteRule)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrOrgRoutRuleLock->P();

	if (!m_pSHMMgrOrgRouteIDindex->get(sOrgRouteRule.m_iRouteID,&value)) {
		int i = m_pSHMMgrOrgRoutRuleData->malloc();

		if (i == 0) {
			m_pSHMMgrOrgRoutRuleLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrOrgRoutRuleData[i],&sOrgRouteRule,sizeof(SgwOrgRouteRule));

		unsigned int temp = 0;

		//先删除另外一个索引的数据
		if (m_pSHMMgrOrgIDindex->get(sOrgRouteRule.m_iOrgID,&temp)) {
			int pos = m_pMgrOrgRoutRuleData[temp].m_iNext;
			m_pMgrOrgRoutRuleData[i].m_iNext = pos;
			m_pMgrOrgRoutRuleData[temp].m_iNext = i;
		} else {
			m_pSHMMgrOrgIDindex->add(sOrgRouteRule.m_iOrgID,i);
			m_pMgrOrgRoutRuleData[i].m_iNext = 0;
		}
		m_pSHMMgrOrgRouteIDindex->add(sOrgRouteRule.m_iRouteID,i);;
		m_pSHMMgrOrgRoutRuleLock->V();
		return 0;
	}
	m_pSHMMgrOrgRoutRuleLock->V();
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#else

	if (!m_pSHMMgrOrgRouteIDindex->get(sOrgRouteRule.m_iRouteID,&value)) {
		int i = m_pSHMMgrOrgRoutRuleData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrOrgRoutRuleData[i],&sOrgRouteRule,sizeof(SgwOrgRouteRule));

		unsigned int temp = 0;

		//先删除另外一个索引的数据
		if (m_pSHMMgrOrgIDindex->get(sOrgRouteRule.m_iOrgID,&temp)) {
			int pos = m_pMgrOrgRoutRuleData[temp].m_iNext;
			m_pMgrOrgRoutRuleData[i].m_iNext = pos;
			m_pMgrOrgRoutRuleData[temp].m_iNext = i;
		} else {
			m_pSHMMgrOrgIDindex->add(sOrgRouteRule.m_iOrgID,i);
			m_pMgrOrgRoutRuleData[i].m_iNext = 0;
		}
		m_pSHMMgrOrgRouteIDindex->add(sOrgRouteRule.m_iRouteID,i);;
		return 0;
	}
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#endif
}

int SHMOrgRouteRule::DeleteOrgRouteRule(const SgwOrgRouteRule& sOrgRouteRule)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrOrgRoutRuleLock->P();

	if (m_pSHMMgrOrgRouteIDindex->get(sOrgRouteRule.m_iRouteID,&value)) {
		unsigned int temp = 0;

		//先删除另外一个索引的数据
		if (m_pSHMMgrOrgIDindex->get(sOrgRouteRule.m_iOrgID,&temp)) {
			int fvalue = temp;
			int num = 0;

			while (temp) {
				if (m_pMgrOrgRoutRuleData[value].m_iOrgID == m_pMgrOrgRoutRuleData[temp].m_iOrgID) {
					if (num == 0 ) {
						int pos = m_pMgrOrgRoutRuleData[temp].m_iNext;
						if (pos == 0) {
							m_pSHMMgrOrgIDindex->revokeIdx(sOrgRouteRule.m_iOrgID,temp);
						} else {
							m_pSHMMgrOrgIDindex->add(sOrgRouteRule.m_iOrgID,pos);
						}
						break;
					} else {
						int pos = m_pMgrOrgRoutRuleData[temp].m_iNext;
						m_pMgrOrgRoutRuleData[fvalue].m_iNext = pos;
					}
					break;
				}
				num++;
				fvalue = temp;
				temp = m_pMgrOrgRoutRuleData[temp].m_iNext;
			}
		}
		m_pSHMMgrOrgRouteIDindex->revokeIdx(sOrgRouteRule.m_iRouteID,value);
		m_pSHMMgrOrgRoutRuleData->erase(value);
		memset(&m_pMgrOrgRoutRuleData[value],0,sizeof(SgwOrgRouteRule));
		m_pSHMMgrOrgRoutRuleLock->V();
		return 0;
	}
	m_pSHMMgrOrgRoutRuleLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else

	if (m_pSHMMgrOrgRouteIDindex->get(sOrgRouteRule.m_iRouteID,&value)) {
		unsigned int temp = 0;

		//先删除另外一个索引的数据
		if (m_pSHMMgrOrgIDindex->get(sOrgRouteRule.m_iOrgID,&temp)) {
			int fvalue = temp;
			int num = 0;

			while (temp) {
				if (m_pMgrOrgRoutRuleData[value].m_iOrgID == m_pMgrOrgRoutRuleData[temp].m_iOrgID) {
					if (num == 0 ) {
						int pos = m_pMgrOrgRoutRuleData[temp].m_iNext;
						if (pos == 0) {
							m_pSHMMgrOrgIDindex->revokeIdx(sOrgRouteRule.m_iOrgID,temp);
						} else {
							m_pSHMMgrOrgIDindex->add(sOrgRouteRule.m_iOrgID,pos);
						}
						break;
					} else {
						int pos = m_pMgrOrgRoutRuleData[temp].m_iNext;
						m_pMgrOrgRoutRuleData[fvalue].m_iNext = pos;
					}
					break;
				}
				num++;
				fvalue = temp;
				temp = m_pMgrOrgRoutRuleData[temp].m_iNext;
			}
		}
		m_pSHMMgrOrgRouteIDindex->revokeIdx(sOrgRouteRule.m_iRouteID,value);
		m_pSHMMgrOrgRoutRuleData->erase(value);
		memset(&m_pMgrOrgRoutRuleData[value],0,sizeof(SgwOrgRouteRule));
		return 0;
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif

}

int SHMOrgRouteRule::UpdateOrgRouteRule(int iRouteID,SgwOrgRouteRule& sOrgRouteRule)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrOrgRoutRuleLock->P();
	if (m_pSHMMgrOrgRouteIDindex->get(iRouteID,&value)) {
		int iNext = m_pMgrOrgRoutRuleData[value].m_iNext;
		memcpy(&m_pMgrOrgRoutRuleData[value],&sOrgRouteRule,sizeof(SgwOrgRouteRule));
		m_pMgrOrgRoutRuleData[value].m_iNext = iNext;
		m_pSHMMgrOrgRoutRuleLock->V();
		return 0;
	}
	m_pSHMMgrOrgRoutRuleLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else
	if (m_pSHMMgrOrgRouteIDindex->get(iRouteID,&value)) {
		int iNext = m_pMgrOrgRoutRuleData[value].m_iNext;
		memcpy(&m_pMgrOrgRoutRuleData[value],&sOrgRouteRule,sizeof(SgwOrgRouteRule));
		m_pMgrOrgRoutRuleData[value].m_iNext = iNext;
		return 0;
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif
}

int SHMAreaOrg::GetAllAreaOrg(vector<SgwAreaOrg>&vSgwAreaOrg)
{
	vSgwAreaOrg.clear();

	for (int i = 1; i<=m_pSHMMgrAreaOrgData->getCount(); i++){
		vSgwAreaOrg.push_back(m_pMgrAreaOrgData[i]);
	}
	return vSgwAreaOrg.size();
}

int SHMAreaOrg::AddAreaOrg(const SgwAreaOrg&sSgwAreaOrg)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrAreaOrgLock->P();

	if (!m_pSHMMgrAreaOrgSeqindex->get(sSgwAreaOrg.m_iAreaSeq,&value)) {
		int i = m_pSHMMgrAreaOrgData->malloc();

		if (i == 0) {
			m_pSHMMgrAreaOrgLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrAreaOrgData[i],&sSgwAreaOrg,sizeof(SgwAreaOrg));
		m_pSHMMgrAreaOrgSeqindex->add(sSgwAreaOrg.m_iAreaSeq,i);
		m_pSHMMgrAreaOrgLock->V();
		return 0;
	}
	m_pSHMMgrAreaOrgLock->V();
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#else
	if (!m_pSHMMgrAreaOrgSeqindex->get(sSgwAreaOrg.m_iAreaSeq,&value)) {
		int i = m_pSHMMgrAreaOrgData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrAreaOrgData[i],&sSgwAreaOrg,sizeof(SgwAreaOrg));
		m_pSHMMgrAreaOrgSeqindex->add(sSgwAreaOrg.m_iAreaSeq,i);
		return 0;
	}
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#endif
}

int SHMAreaOrg::GetAreaOrg(int iAreaSeq,const SgwAreaOrg &sSgwAreaOrg)
{
	unsigned int value = 0;
	if (m_pSHMMgrAreaOrgSeqindex->get(iAreaSeq,&value)) {
		memcpy(&sSgwAreaOrg,&m_pMgrAreaOrgData[value],sizeof(SgwAreaOrg));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMAreaOrg::DeleteAreaOrg(const SgwAreaOrg&sSgwAreaOrg)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrAreaOrgLock->P();

	if (m_pSHMMgrAreaOrgSeqindex->revokeIdx(sSgwAreaOrg.m_iAreaSeq,value)) {
		m_pSHMMgrAreaOrgData->erase(value);
		memset(&m_pMgrAreaOrgData[value],0,sizeof(SgwAreaOrg));
		m_pSHMMgrAreaOrgLock->V();
		return 0;
	}
	m_pSHMMgrAreaOrgLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else
	if (m_pSHMMgrAreaOrgSeqindex->get(sSgwAreaOrg.m_iAreaSeq,&value)) {
		m_pSHMMgrAreaOrgData->erase(value);
		memset(&m_pMgrAreaOrgData[value],0,sizeof(SgwAreaOrg));
		return 0;
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif
}

int SHMAreaOrg::UpdateAreaOrg(int iAreaSeq,SgwAreaOrg&sSgwAreaOrg)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrAreaOrgLock->P();

	if (m_pSHMMgrAreaOrgSeqindex->get(iAreaSeq,&value)) {
		memcpy(&m_pMgrAreaOrgData[value],&sSgwAreaOrg,sizeof(SgwAreaOrg));
		m_pSHMMgrAreaOrgLock->V();
		return 0;
	}
	m_pSHMMgrAreaOrgLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else
	if (m_pSHMMgrAreaOrgSeqindex->get(iAreaSeq,&value)) {
		memcpy(&m_pMgrAreaOrgData[value],&sSgwAreaOrg,sizeof(SgwAreaOrg));
		return 0;
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif
}

int SHMTuxRelaIn::UpdateServicesContextID(SgwTuxedoRelationIn&sTuxedoRelation)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrTuxRelaInLock->P();

	if (m_pSHMMgrTuxInSvrsIDindex->get(sTuxedoRelation.m_iServicesID,&value)) {
		memcpy(&m_pMgrTuxRelaInData[value],&sTuxedoRelation,sizeof(SgwTuxedoRelationIn));
		m_pSHMMgrTuxRelaInLock->V();
		return 0;
	}
	m_pSHMMgrTuxRelaInLock->V();
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#else

	if (m_pSHMMgrTuxInSvrsIDindex->get(sTuxedoRelation.m_iServicesID,&value)) {
		memcpy(&m_pMgrTuxRelaInData[value],&sTuxedoRelation,sizeof(SgwTuxedoRelationIn));
		return 0;
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
#endif
}

int SHMTuxRelaIn::DeleteServicesContextID(SgwTuxedoRelationIn&sTuxedoRelation)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrTuxRelaInLock->P();

	if (m_pSHMMgrTuxInSvrsIDindex->revokeIdx(sTuxedoRelation.m_iServicesID,value)) {
		m_pSHMMgrTuxRelaInData->erase(value);
		memset(&m_pMgrTuxRelaInData[value],0,sizeof(SgwTuxedoRelationIn));
		m_pSHMMgrTuxRelaInLock->V();
		return 0;
	}
	m_pSHMMgrTuxRelaInLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else

	if (m_pSHMMgrTuxInSvrsIDindex->revokeIdx(sTuxedoRelation.m_iServicesID,value)) {
		m_pSHMMgrTuxRelaInData->erase(value);
		memset(&m_pMgrTuxRelaInData[value],0,sizeof(SgwTuxedoRelationIn));
		return 0;
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif

}

int SHMTuxRelaIn::AddServicesContextID(SgwTuxedoRelationIn&sTuxedoRelation)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrTuxRelaInLock->P();

	if (!m_pSHMMgrTuxInSvrsIDindex->get(sTuxedoRelation.m_iServicesID,&value)) {
		int i= m_pSHMMgrTuxRelaInData->malloc();

		if (i == 0) {
			m_pSHMMgrTuxRelaInLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrTuxRelaInData[i],&sTuxedoRelation,sizeof(SgwTuxedoRelationIn));
		m_pSHMMgrTuxInSvrsIDindex->add(sTuxedoRelation.m_iServicesID,i);
		m_pSHMMgrTuxRelaInLock->V();
		return 0;
	}
	m_pSHMMgrTuxRelaInLock->V();
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#else

	if (!m_pSHMMgrTuxInSvrsIDindex->get(sTuxedoRelation.m_iServicesID,&value)) {
		int i= m_pSHMMgrTuxRelaInData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrTuxRelaInData[i],&sTuxedoRelation,sizeof(SgwTuxedoRelationIn));
		m_pSHMMgrTuxInSvrsIDindex->add(sTuxedoRelation.m_iServicesID,i);
		return 0;
	}
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#endif
}

int SHMTuxRelaIn::GetServicesContextID(int iServicesID,SgwTuxedoRelationIn&sTuxedoRelation)
{
	unsigned int value = 0;

	if (m_pSHMMgrTuxInSvrsIDindex->get(iServicesID,&value)) {
		memcpy(&sTuxedoRelation,&m_pMgrTuxRelaInData[value],sizeof(SgwTuxedoRelationIn));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMTuxRelaOut::AddServicesID(const SgwTuxedoRelationOut&sTuxedoRelation)
{
	unsigned int value = 0;
	long lkey = 0;

#ifndef _DF_LOCK_
	m_pSHMMgrTuxRelaOutLock->P();

	if (!m_pSHMMgrTuxOutRelaIDindex->get(sTuxedoRelation.m_iRelationID,&value)) {
		int i = m_pSHMMgrTuxRelaOutData->malloc();

		if (i == 0) {
			m_pSHMMgrTuxRelaOutLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrTuxRelaOutData[i],&sTuxedoRelation,sizeof(SgwTuxedoRelationOut));
		lkey = SHMDataMgr::changekey(m_pMgrTuxRelaOutData[i].m_szServiceContextID,m_pSHMMgrTuxRelaOutData->getTotal());
		unsigned int temp = 0;

		if (m_pSHMMgrTuxOutSvrTxtIDindex->get(lkey,&temp)){
			int pos = m_pMgrTuxRelaOutData[temp].m_iNext;
			m_pMgrTuxRelaOutData[temp].m_iNext = i;
			m_pMgrTuxRelaOutData[i].m_iNext = pos;
		} else {
			m_pSHMMgrTuxOutSvrTxtIDindex->add(lkey,i);
		}
		m_pSHMMgrTuxOutRelaIDindex->add(m_pMgrTuxRelaOutData[i].m_iRelationID,i);
		m_pSHMMgrTuxRelaOutLock->V();
		return 0;
	}
	m_pSHMMgrTuxRelaOutLock->V();
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#else
	if (!m_pSHMMgrTuxOutRelaIDindex->get(sTuxedoRelation.m_iRelationID,&value)) {
		int i = m_pSHMMgrTuxRelaOutData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrTuxRelaOutData[i],&sTuxedoRelation,sizeof(SgwTuxedoRelationOut));
		lkey = SHMDataMgr::changekey(m_pMgrTuxRelaOutData[i].m_szServiceContextID,m_pSHMMgrTuxRelaOutData->getTotal());
		unsigned int temp = 0;

		if (m_pSHMMgrTuxOutSvrTxtIDindex->get(lkey,&temp)){
			int pos = m_pMgrTuxRelaOutData[temp].m_iNext;
			m_pMgrTuxRelaOutData[temp].m_iNext = i;
			m_pMgrTuxRelaOutData[i].m_iNext = pos;
		} else {
			m_pSHMMgrTuxOutSvrTxtIDindex->add(lkey,i);
		}
		m_pSHMMgrTuxOutRelaIDindex->add(m_pMgrTuxRelaOutData[i].m_iRelationID,i);
		return 0;
	}
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#endif
}

int SHMTuxRelaOut::DeleteServicesID(const SgwTuxedoRelationOut&sTuxedoRelation)
{
	unsigned int value = 0;
	long lkey = 0;

#ifndef _DF_LOCK_
	m_pSHMMgrTuxRelaOutLock->P();

	if (m_pSHMMgrTuxOutRelaIDindex->get(sTuxedoRelation.m_iRelationID,&value)) {

		lkey = SHMDataMgr::changekey(m_pMgrTuxRelaOutData[value].m_szServiceContextID,m_pSHMMgrTuxRelaOutData->getTotal());
		unsigned int temp = 0;

		if (m_pSHMMgrTuxOutSvrTxtIDindex->get(lkey,&temp)){
			unsigned int falue = temp;
			int num = 0;

			while (temp) {
				if (strcmp(m_pMgrTuxRelaOutData[value].m_szServiceContextID,m_pMgrTuxRelaOutData[temp].m_szServiceContextID)==0) {
					if (num == 0) {//如果要删除的数据时链表的头结点
						int pos = m_pMgrTuxRelaOutData[temp].m_iNext;
						if (pos == 0) {//若该索引对应的数据链表只有一条数据，则删除该索引
							m_pSHMMgrTuxOutSvrTxtIDindex->revokeIdx(lkey,temp);
						} else {//如果有多条数据，则以第二条数据建立索引，再把头结点删除
							m_pSHMMgrTuxOutSvrTxtIDindex->add(lkey,pos);
						}
					} else {
						int pos = m_pMgrTuxRelaOutData[temp].m_iNext;
						m_pMgrTuxRelaOutData[falue].m_iNext = pos;
					}
					break;
				}
				num++;
				falue = temp;
				temp = m_pMgrTuxRelaOutData[temp].m_iNext;
			}
		}

		m_pSHMMgrTuxOutRelaIDindex->revokeIdx(m_pMgrTuxRelaOutData[value].m_iRelationID,value);
		m_pSHMMgrTuxRelaOutData->erase(value);
		memset(&m_pMgrTuxRelaOutData[value],0,sizeof(SgwTuxedoRelationOut));
		m_pSHMMgrTuxRelaOutLock->V();
		return 0;
	}
	m_pSHMMgrTuxRelaOutLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else
	if (m_pSHMMgrTuxOutRelaIDindex->get(sTuxedoRelation.m_iRelationID,&value)) {

		lkey = SHMDataMgr::changekey(m_pMgrTuxRelaOutData[value].m_szServiceContextID,m_pSHMMgrTuxRelaOutData->getTotal());
		unsigned int temp = 0;

		if (m_pSHMMgrTuxOutSvrTxtIDindex->get(lkey,&temp)){
			unsigned int falue = temp;
			int num = 0;

			while (temp) {
				if (strcmp(m_pMgrTuxRelaOutData[value].m_szServiceContextID,m_pMgrTuxRelaOutData[temp].m_szServiceContextID)==0) {
					if (num == 0) {//如果要删除的数据时链表的头结点
						int pos = m_pMgrTuxRelaOutData[temp].m_iNext;
						if (pos == 0) {//若该索引对应的数据链表只有一条数据，则删除该索引
							m_pSHMMgrTuxOutSvrTxtIDindex->revokeIdx(lkey,temp);
						} else {//如果有多条数据，则以第二条数据建立索引，再把头结点删除
							m_pSHMMgrTuxOutSvrTxtIDindex->add(lkey,pos);
						}
					} else {
						int pos = m_pMgrTuxRelaOutData[temp].m_iNext;
						m_pMgrTuxRelaOutData[falue].m_iNext = pos;
					}
					break;
				}
				num++;
				falue = temp;
				temp = m_pMgrTuxRelaOutData[temp].m_iNext;
			}
		}

		m_pSHMMgrTuxOutRelaIDindex->revokeIdx(m_pMgrTuxRelaOutData[value].m_iRelationID,value);
		m_pSHMMgrTuxRelaOutData->erase(value);
		memset(&m_pMgrTuxRelaOutData[value],0,sizeof(SgwTuxedoRelationOut));
		return 0;
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif
}

int SHMTuxRelaOut::UpdateServicesID(const SgwTuxedoRelationOut&sTuxedoRelation)
{
	unsigned int value = 0;
	int iRet = 0;

	if (m_pSHMMgrTuxOutRelaIDindex->get(sTuxedoRelation.m_iRelationID,&value)) {
		iRet = DeleteServicesID(m_pMgrTuxRelaOutData[value]);
		if (iRet!=0) {
			return iRet;
		}
		iRet = AddServicesID(sTuxedoRelation);
	}
	return iRet;
}

int SHMTuxRelaOut::GetServicesID(char * pszServiceContextID,SgwTuxedoRelationOut&sTuxedoRelation)
{
	unsigned int value =0;
	long lkey = 0;
	lkey = SHMDataMgr::changekey(pszServiceContextID,m_pSHMMgrTuxRelaOutData->getTotal());

	if (m_pSHMMgrTuxOutSvrTxtIDindex->get(lkey,&value)) {
		while (value) {
			if (strcmp(pszServiceContextID,m_pMgrTuxRelaOutData[value].m_szServiceContextID) == 0) {
				memcpy(&sTuxedoRelation,&m_pMgrTuxRelaOutData[value],sizeof(SgwTuxedoRelationOut));
				return 0;
			}
			value  = m_pMgrTuxRelaOutData[value].m_iNext;
		}
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMSvrList::AddServicesInfo(const SgwServicesList&sServicesList)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrSrvsListLock->P();

	long lKey = SHMDataMgr::changekey(sServicesList.m_szServicesName,m_pSHMMgrSrvsListData->getTotal());
	if (m_pSHMMgrSrvsListServicesNameindex->get(lKey,&value)) {
		int fvalue = value;

		while (value) {

			if (m_pMgrSrvsListData[value].m_iServicesID == sServicesList.m_iServicesID) {
				m_pSHMMgrSrvsParamRelaLock->V();
				return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			value = m_pMgrSrvsListData[value].m_iNext;
		}
		int i = m_pSHMMgrSrvsListData->malloc();

		if (i == 0) {
			m_pSHMMgrSrvsListLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}

		memcpy(&m_pMgrSrvsListData[i],&sServicesList,sizeof(SgwServicesList));
		m_pSHMMgrSrvsListIDindex->add(sServicesList.m_iServicesID,i);

		int pos = m_pMgrSrvsListData[fvalue].m_iNext;
		m_pMgrSrvsListData[i].m_iNext = pos;
		m_pMgrSrvsListData[fvalue].m_iNext = i;
	} else {
		int i = m_pSHMMgrSrvsListData->malloc();

		if (i == 0) {
			m_pSHMMgrSrvsParamRelaLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrSrvsListData[i],&sServicesList,sizeof(SgwServicesList));
		m_pSHMMgrSrvsListIDindex->add(sServicesList.m_iServicesID,i);
		m_pSHMMgrSrvsListServicesNameindex->add(lKey,i);
		m_pMgrSrvsListData[i].m_iNext = 0;
		m_pSHMMgrSrvsListLock->V();
		return 0;
	}

	m_pSHMMgrSrvsListLock->V();
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#else
	long lKey = SHMDataMgr::changekey(sServicesList.m_szServicesName,m_pSHMMgrSrvsListData->getTotal());
	if (m_pSHMMgrSrvsListServicesNameindex->get(lKey,&value)) {
		int fvalue = value;

		while (value) {

			if (m_pMgrSrvsListData[value].m_iServicesID == sServicesList.m_iServicesID) {
				m_pSHMMgrSrvsParamRelaLock->V();
				return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			value = m_pMgrSrvsListData[value].m_iNext;
		}
		int i = m_pSHMMgrSrvsListData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}

		memcpy(&m_pMgrSrvsListData[i],&sServicesList,sizeof(SgwServicesList));
		m_pSHMMgrSrvsListIDindex->add(sServicesList.m_iServicesID,i);

		int pos = m_pMgrSrvsListData[fvalue].m_iNext;
		m_pMgrSrvsListData[i].m_iNext = pos;
		m_pMgrSrvsListData[fvalue].m_iNext = i;
	} else {
		int i = m_pSHMMgrSrvsListData->malloc();

		if (i == 0) {
			m_pSHMMgrSrvsParamRelaLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrSrvsListData[i],&sServicesList,sizeof(SgwServicesList));
		m_pSHMMgrSrvsListIDindex->add(sServicesList.m_iServicesID,i);
		m_pSHMMgrSrvsListServicesNameindex->add(lKey,i);
		m_pMgrSrvsListData[i].m_iNext = 0;
		m_pSHMMgrSrvsListLock->V();
		return 0;
	}
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#endif
}

int SHMSvrList::UpdateServicesInfo(const SgwServicesList&sServicesList)
{
	int iRet = 0;
	unsigned int value = 0;

	long lKey = SHMDataMgr::changekey(sServicesList.m_szServicesName,m_pSHMMgrSrvsListData->getTotal());
	if (m_pSHMMgrSrvsListServicesNameindex->get(lKey,&value)) {
		while(value) {
			if(strcmp(m_pMgrSrvsListData[value].m_szServicesName,sServicesList.m_szServicesName)==0) {
				iRet = DeleteServicesInfo(m_pMgrSrvsListData[value]);
				if (iRet != 0) {
					return iRet;
				}
				iRet = AddServicesInfo(sServicesList);
			}
			value = m_pMgrSrvsListData[value].m_iNext;
		}

	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;

}

int SHMSvrList::DeleteServicesInfo(const SgwServicesList&sServicesList)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrSrvsListLock->P();

	long lKey = SHMDataMgr::changekey(sServicesList.m_szServicesName,m_pSHMMgrSrvsListData->getTotal());
	if (m_pSHMMgrSrvsListServicesNameindex->get(lKey,&value)) {
		int fvalue = value;
		int num = 0;

		while (value) {
			if (m_pMgrSrvsListData[value].m_iServicesID == sServicesList.m_iServicesID) {

				if (num == 0 ) {
					int pos = m_pMgrSrvsListData[value].m_iNext;

					if (pos == 0) {
						m_pSHMMgrSrvsListServicesNameindex->revokeIdx(lKey,value);
					} else {
						m_pSHMMgrSrvsListServicesNameindex->add(lKey,pos);
					}
				} else {
					int pos = m_pMgrSrvsListData[value].m_iNext;
					m_pMgrSrvsListData[fvalue].m_iNext = pos;
				}
				m_pSHMMgrSrvsListIDindex->revokeIdx(m_pMgrSrvsListData[value].m_iServicesID,value);
				m_pSHMMgrSrvsListData->erase(value);
				memset(&m_pMgrSrvsListData[value],0,sizeof(SgwServicesList));
				m_pSHMMgrSrvsListLock->V();
				return 0;
			}
			num++;
			fvalue = value;
			value = m_pMgrSrvsListData[value].m_iNext;
		}
	}

	m_pSHMMgrSrvsListLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else
	long lKey = SHMDataMgr::changekey(sServicesList.m_szServicesName,m_pSHMMgrSrvsListData->getTotal());
	if (m_pSHMMgrSrvsListServicesNameindex->get(lKey,&value)) {
		int fvalue = value;
		int num = 0;

		while (value) {
			if (m_pMgrSrvsListData[value].m_iServicesID == sServicesList.m_iServicesID) {

				if (num == 0 ) {
					int pos = m_pMgrSrvsListData[value].m_iNext;

					if (pos == 0) {
						m_pSHMMgrSrvsListServicesNameindex->revokeIdx(lKey,value);
					} else {
						m_pSHMMgrSrvsListServicesNameindex->add(lKey,pos);
					}
				} else {
					int pos = m_pMgrSrvsListData[value].m_iNext;
					m_pMgrSrvsListData[fvalue].m_iNext = pos;
				}
				m_pSHMMgrSrvsListIDindex->revokeIdx(m_pMgrSrvsListData[value].m_iServicesID,value);
				m_pSHMMgrSrvsListData->erase(value);
				memset(&m_pMgrSrvsListData[value],0,sizeof(SgwServicesList));
				return 0;
			}
			num++;
			fvalue = value;
			value = m_pMgrSrvsListData[value].m_iNext;
		}
	}

	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif
}

//按服务名称（SERVICES_NAME）返回信息
int SHMSvrList::GetServicesInfo(char * iServicesName,SgwServicesList&sServicesList)
{
	unsigned int value = 0;
	long lKey = SHMDataMgr::changekey(iServicesName,m_pSHMMgrSrvsListData->getTotal());

	if (m_pSHMMgrSrvsListServicesNameindex->get(lKey,&value)) {
		while(value) {
			if(strcmp(m_pMgrSrvsListData[value].m_szServicesName,iServicesName)==0) {
				memcpy(&sServicesList,&m_pMgrSrvsListData[value],sizeof(SgwServicesList));
				return 0;
			}
			value = m_pMgrSrvsListData[value].m_iNext;
		}
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMSvrList::GetServicesInfo(int iServicesID,SgwServicesList&sServicesList)
{
	unsigned int value = 0;

	if (m_pSHMMgrSrvsListIDindex->get(iServicesID,&value)) {
		memcpy(&sServicesList,&m_pMgrSrvsListData[value],sizeof(SgwServicesList));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMSrvsParamRela::GetServicesParam(int iServicesID,vector<SgwServicesParamRelation>& vServicesParam)
{
	unsigned int value = 0;
	vServicesParam.clear();

	if (m_pSHMMgrSrvsParamRelaindex->get(iServicesID,&value)) {
		while(value) {
			vServicesParam.push_back(m_pMgrSrvsParamRelaData[value]);
			value = m_pMgrSrvsParamRelaData[value].m_iNext;
		}
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMSrvsParamRela::GetServicesParam(int iMappingID,SgwServicesParamRelation &oServicesParam)
{
	unsigned int value  = 0;

	if(m_pSHMMgrSrvsParamRelaMappingIDIndex->get(iMappingID,&value)) {
		memcpy(&oServicesParam,&m_pMgrSrvsParamRelaData[value],sizeof(SgwServicesParamRelation));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMSrvsParamRela::AddServicesParam(const SgwServicesParamRelation &oServicesParam)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_

	m_pSHMMgrSrvsParamRelaLock->P();

	if (m_pSHMMgrSrvsParamRelaindex->get(oServicesParam.m_iServicesID,&value)) {
		int fvalue = value;

		while (value) {

			if (m_pMgrSrvsParamRelaData[value].m_iMappingID == oServicesParam.m_iMappingID) {
				m_pSHMMgrSrvsParamRelaLock->V();
				return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			value = m_pMgrSrvsParamRelaData[value].m_iNext;
		}
		int i = m_pSHMMgrSrvsParamRelaData->malloc();

		if (i == 0) {
			m_pSHMMgrSrvsParamRelaLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}

		memcpy(&m_pMgrSrvsParamRelaData[i],&oServicesParam,sizeof(SgwServicesParamRelation));
		m_pSHMMgrSrvsParamRelaMappingIDIndex->add(oServicesParam.m_iMappingID,i);

		int pos = m_pMgrSrvsParamRelaData[fvalue].m_iNext;
		m_pMgrSrvsParamRelaData[i].m_iNext = pos;
		m_pMgrSrvsParamRelaData[fvalue].m_iNext = i;
	} else {
		int i = m_pSHMMgrSrvsParamRelaData->malloc();

		if (i == 0) {
			m_pSHMMgrSrvsParamRelaLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrSrvsParamRelaData[i],&oServicesParam,sizeof(SgwServicesParamRelation));
		m_pSHMMgrSrvsParamRelaMappingIDIndex->add(oServicesParam.m_iMappingID,i);
		m_pSHMMgrSrvsParamRelaindex->add(oServicesParam.m_iServicesID,i);
		m_pMgrSrvsParamRelaData[i].m_iNext = 0;
		m_pSHMMgrSrvsParamRelaLock->V();
		return 0;
	}
	m_pSHMMgrSrvsParamRelaLock->V();
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#else

	if (m_pSHMMgrSrvsParamRelaindex->get(oServicesParam.m_iServicesID,&value)) {
		int fvalue = value;

		while (value) {

			if (m_pMgrSrvsParamRelaData[value].m_iMappingID == oServicesParam.m_iMappingID) {
				return MBC_SHM_ERRNO_ADD_DATA_EXIST;
			}
			value = m_pMgrSrvsParamRelaData[value].m_iNext;
		}
		int i = m_pSHMMgrSrvsParamRelaData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}

		memcpy(&m_pMgrSrvsParamRelaData[i],&oServicesParam,sizeof(SgwServicesParamRelation));
		m_pSHMMgrSrvsParamRelaMappingIDIndex->add(oServicesParam.m_iMappingID,i);

		int pos = m_pMgrSrvsParamRelaData[fvalue].m_iNext;
		m_pMgrSrvsParamRelaData[i].m_iNext = pos;
		m_pMgrSrvsParamRelaData[fvalue].m_iNext = i;
	} else {
		int i = m_pSHMMgrSrvsParamRelaData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrSrvsParamRelaData[i],&oServicesParam,sizeof(SgwServicesParamRelation));
		m_pSHMMgrSrvsParamRelaMappingIDIndex->add(oServicesParam.m_iMappingID,i);
		m_pSHMMgrSrvsParamRelaindex->add(oServicesParam.m_iServicesID,i);
		m_pMgrSrvsParamRelaData[i].m_iNext = 0;
		return 0;
	}
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#endif
}

int SHMSrvsParamRela::UpdateServicesParam(const SgwServicesParamRelation &oServicesParam)
{
	int iRet = 0;
	unsigned int value = 0;

	if (m_pSHMMgrSrvsParamRelaMappingIDIndex->get(oServicesParam.m_iMappingID,&value)) {
		iRet = DeleteServicesParam(m_pMgrSrvsParamRelaData[value]);
		if (iRet != 0) {
			return iRet;
		}
		iRet = AddServicesParam(oServicesParam);
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
}

int SHMSrvsParamRela::DeleteServicesParam(const SgwServicesParamRelation &oServicesParam)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_

	m_pSHMMgrSrvsParamRelaLock->P();

	if (m_pSHMMgrSrvsParamRelaindex->get(oServicesParam.m_iServicesID,&value)) {
		int fvalue = value;
		int num = 0;

		while (value) {

			if (m_pMgrSrvsParamRelaData[value].m_iMappingID == oServicesParam.m_iMappingID) {

				if (num == 0 ) {
					int pos = m_pMgrSrvsParamRelaData[value].m_iNext;

					if (pos == 0) {
						m_pSHMMgrSrvsParamRelaindex->revokeIdx(oServicesParam.m_iServicesID,value);
					} else {
						m_pSHMMgrSrvsParamRelaindex->add(oServicesParam.m_iServicesID,pos);
					}
				} else {
					int pos = m_pMgrSrvsParamRelaData[value].m_iNext;
					m_pMgrSrvsParamRelaData[fvalue].m_iNext = pos;
				}
				m_pSHMMgrSrvsParamRelaMappingIDIndex->revokeIdx(m_pMgrSrvsParamRelaData[value].m_iMappingID,value);
				m_pSHMMgrSrvsParamRelaData->erase(value);
				memset(&m_pMgrSrvsParamRelaData[value],0,sizeof(SgwServicesParamRelation));
				m_pSHMMgrSrvsParamRelaLock->V();
				return 0;
			}
			num++;
			fvalue = value;
			value = m_pMgrSrvsParamRelaData[value].m_iNext;
		}
	}
	m_pSHMMgrSrvsParamRelaLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else
	if (m_pSHMMgrSrvsParamRelaindex->get(oServicesParam.m_iServicesID,&value)) {
		int fvalue = value;
		int num = 0;

		while (value) {

			if (m_pMgrSrvsParamRelaData[value].m_iMappingID == oServicesParam.m_iMappingID) {

				if (num == 0 ) {
					int pos = m_pMgrSrvsParamRelaData[value].m_iNext;

					if (pos == 0) {
						m_pSHMMgrSrvsParamRelaindex->revokeIdx(oServicesParam.m_iServicesID,value);
					} else {
						m_pSHMMgrSrvsParamRelaindex->add(oServicesParam.m_iServicesID,pos);
					}
				} else {
					int pos = m_pMgrSrvsParamRelaData[value].m_iNext;
					m_pMgrSrvsParamRelaData[fvalue].m_iNext = pos;
				}
				m_pSHMMgrSrvsParamRelaMappingIDIndex->revokeIdx(m_pMgrSrvsParamRelaData[value].m_iMappingID,value);
				m_pSHMMgrSrvsParamRelaData->erase(value);
				memset(&m_pMgrSrvsParamRelaData[value],0,sizeof(SgwServicesParamRelation));
				return 0;
			}
			num++;
			fvalue = value;
			value = m_pMgrSrvsParamRelaData[value].m_iNext;
		}
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif

}

int SHMSrvsParamList::GetServiceParamList(int iParamID,SgwServicesParamList& sServicesparamList)
{
	unsigned int value = 0;

	if (m_pSHMMgrSrvsParamListindex->get(iParamID,&value)) {
		memcpy(&sServicesparamList,&m_pMgrSrvsParamListData[value],sizeof(SgwServicesParamList));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMSrvsParamList::AddServiceParamList(const SgwServicesParamList& sServicesparamList)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_

	m_pSHMMgrSrvsParamListLock->P();

	if (!m_pSHMMgrSrvsParamListindex->get(sServicesparamList.m_iParamID,&value)) {
		int i = m_pSHMMgrSrvsParamListData->malloc();

		if (i == 0) {
			m_pSHMMgrSrvsParamListLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrSrvsParamListData[i],&sServicesparamList,sizeof(SgwServicesParamList));
		m_pSHMMgrSrvsParamListindex->add(sServicesparamList.m_iParamID,i);
		m_pSHMMgrSrvsParamListLock->V();
		return 0;
	} else {
		m_pSHMMgrSrvsParamListLock->V();
		return MBC_SHM_ERRNO_ADD_DATA_EXIST;
	}
#else

	if (!m_pSHMMgrSrvsParamListindex->get(sServicesparamList.m_iParamID,&value)) {
		int i = m_pSHMMgrSrvsParamListData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrSrvsParamListData[i],&sServicesparamList,sizeof(SgwServicesParamList));
		m_pSHMMgrSrvsParamListindex->add(sServicesparamList.m_iParamID,i);
		return 0;
	} else {
		return MBC_SHM_ERRNO_ADD_DATA_EXIST;
	}
#endif
}

int SHMSrvsParamList::DeleteServiceParamList(const SgwServicesParamList& sServicesparamList)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_

	m_pSHMMgrSrvsParamListLock->P();

	if (m_pSHMMgrSrvsParamListindex->revokeIdx(sServicesparamList.m_iParamID,value)) {
		m_pSHMMgrSrvsParamListData->erase(value);
		memset(&m_pMgrSrvsParamListData[value],0,sizeof(SgwServicesParamList));
		m_pSHMMgrSrvsParamListLock->V();
		return 0;
	} else {
		m_pSHMMgrSrvsParamListLock->V();
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	}
#else

	if (m_pSHMMgrSrvsParamListindex->revokeIdx(sServicesparamList.m_iParamID,value)) {
		m_pSHMMgrSrvsParamListData->erase(value);
		memset(&m_pMgrSrvsParamListData[value],0,sizeof(SgwServicesParamList));
		return 0;
	} else {
		return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
	}
#endif

}

int SHMSrvsParamList::UpdateServiceParamList(const SgwServicesParamList& sServicesparamList)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_

	m_pSHMMgrSrvsParamListLock->P();

	if (m_pSHMMgrSrvsParamListindex->get(sServicesparamList.m_iParamID,&value)) {
		memcpy(&m_pMgrSrvsParamListData[value],&sServicesparamList,sizeof(SgwServicesParamList));
		m_pSHMMgrSrvsParamListLock->V();
		return 0;
	} else {
		m_pSHMMgrSrvsParamListLock->V();
		return MBC_SHM_ERRNO_ADD_DATA_EXIST;
	}
#else

	if (m_pSHMMgrSrvsParamListindex->get(sServicesparamList.m_iParamID,&value)) {
		memcpy(&m_pMgrSrvsParamListData[value],&sServicesparamList,sizeof(SgwServicesParamList));
		return 0;
	} else {
		return MBC_SHM_ERRNO_ADD_DATA_EXIST;
	}
#endif

}

int SHMSrvsRegister::GetServiceRegister(int iServicesInstanceID,SgwServiceRegister& sServiceRegister)
{
	unsigned int value = 0;
	if (m_pSHMMgrSrvsRegisterSvrInsIDIndex->get(iServicesInstanceID,&value)) {
		memcpy(&sServiceRegister,&m_pMgrSrvsRegisterData[value],sizeof(SgwServiceRegister));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMSrvsRegister::DeleteServiceRegister(const SgwServiceRegister& sServiceRegister)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrSrvsRegisterLock->P();

	if (m_pSHMMgrSrvsRegisterSvrInsIDIndex->get(sServiceRegister.m_iServicesInstanceID,&value)) {

		long lkey = SHMDataMgr::changekey(sServiceRegister.m_szChannelID,m_pSHMMgrSrvsRegisterData->getTotal());
		unsigned int temp = 0;

		//删除B的索引
		if (m_pSHMMgrSrvsRegisterindex->get(lkey,&temp)) {
			int fvalue = temp;
			int num = 0;//判断是否删除索引头结点

			while (temp) {
				if (strcmp(sServiceRegister.m_szChannelID,m_pMgrSrvsRegisterData[temp].m_szChannelID) == 0 ) {
					if (num == 0) {//删除索引头结点
						int pos = m_pMgrSrvsRegisterData[temp].m_iNext;
						if (pos == 0) {//只有一条数据
							m_pSHMMgrSrvsRegisterindex->revokeIdx(lkey,temp);
						} else {
							m_pSHMMgrSrvsRegisterindex->add(lkey,pos);//重置索引
						}
					} else {
						int pos = m_pMgrSrvsRegisterData[temp].m_iNext;
						m_pMgrSrvsRegisterData[fvalue].m_iNext = pos;
					}
					break;
				}
				num++;
				fvalue = temp;//记录每次循环的上一个结点
				temp = m_pMgrSrvsRegisterData[temp].m_iNext;
			}
		}
		m_pSHMMgrSrvsRegisterSvrInsIDIndex->revokeIdx(sServiceRegister.m_iServicesInstanceID,value);
		m_pSHMMgrSrvsRegisterData->erase(value);
		memset(&m_pMgrSrvsRegisterData[value],0,sizeof(SgwServiceRegister));
		m_pSHMMgrSrvsRegisterLock->V();
		return 0;
	}
	m_pSHMMgrSrvsRegisterLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#else

	if (m_pSHMMgrSrvsRegisterSvrInsIDIndex->get(sServiceRegister.m_iServicesInstanceID,&value)) {

		long lkey = SHMDataMgr::changekey(sServiceRegister.m_szChannelID,m_pSHMMgrSrvsRegisterData->getTotal());
		unsigned int temp = 0;

		if (m_pSHMMgrSrvsRegisterindex->get(lkey,&temp)) {
			int fvalue = temp;
			int num = 0;

			while (temp) {
				if (strcmp(sServiceRegister.m_szChannelID,m_pMgrSrvsRegisterData[temp].m_szChannelID) == 0 ) {
					if (num == 0) {
						int pos = m_pMgrSrvsRegisterData[temp].m_iNext;
						if (pos == 0) {
							m_pSHMMgrSrvsRegisterindex->revokeIdx(lkey,temp);
						} else {
							m_pSHMMgrSrvsRegisterindex->add(lkey,pos);
						}
					} else {
						int pos = m_pMgrSrvsRegisterData[temp].m_iNext;
						m_pMgrSrvsRegisterData[fvalue].m_iNext = pos;
					}
					break;
				}
				num++;
				fvalue = temp;
				temp = m_pMgrSrvsRegisterData[temp].m_iNext;
			}
		}
		m_pSHMMgrSrvsRegisterSvrInsIDIndex->revokeIdx(sServiceRegister.m_iServicesInstanceID,value);
		m_pSHMMgrSrvsRegisterData->erase(value);
		memset(&m_pMgrSrvsRegisterData[value],0,sizeof(SgwServiceRegister));
		return 0;
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;
#endif

}
int SHMSrvsRegister::UpdateServiceRegister(const SgwServiceRegister& sServiceRegister)
{
	unsigned int value ;

	if (m_pSHMMgrSrvsRegisterSvrInsIDIndex->get(sServiceRegister.m_iServicesInstanceID,&value)) {
		int iRet = DeleteServiceRegister(m_pMgrSrvsRegisterData[value]);
		if (iRet !=0 ) {
			return iRet;
		}
		iRet = AddServiceRegister(sServiceRegister);
		return iRet;
	}
	return MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST;
}

int SHMSrvsRegister::AddServiceRegister(const SgwServiceRegister& sServiceRegister)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrSrvsRegisterLock->P();

	if (!m_pSHMMgrSrvsRegisterSvrInsIDIndex->get(sServiceRegister.m_iServicesInstanceID,&value)) {
		int  i = m_pSHMMgrSrvsRegisterData->malloc();
		if ( i == 0 ) {
			m_pSHMMgrSrvsRegisterLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrSrvsRegisterData[i],&sServiceRegister,sizeof(SgwServiceRegister));
		m_pSHMMgrSrvsRegisterSvrInsIDIndex->add(sServiceRegister.m_iServicesInstanceID,i);
		long lkey = SHMDataMgr::changekey(sServiceRegister.m_szChannelID,m_pSHMMgrSrvsRegisterData->getTotal());

		if (m_pSHMMgrSrvsRegisterindex->get(lkey,&value)) {
			int pos = m_pMgrSrvsRegisterData[value].m_iNext;
			m_pMgrSrvsRegisterData[i].m_iNext = pos;
			m_pMgrSrvsRegisterData[value].m_iNext = i;
		} else {
			m_pSHMMgrSrvsRegisterindex->add(lkey,i);
			m_pMgrSrvsRegisterData[i].m_iNext = 0;
		}
		m_pSHMMgrSrvsRegisterLock->V();
		return 0;
	}
	m_pSHMMgrSrvsRegisterLock->V();
	return MBC_SHM_ADD_DATA_EXIST;
#else

	if (!m_pSHMMgrSrvsRegisterSvrInsIDIndex->get(sServiceRegister.m_iServicesInstanceID,&value)) {
		int  i = m_pSHMMgrSrvsRegisterData->malloc();
		if ( i == 0 ) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrSrvsRegisterData[i],&sServiceRegister,sizeof(SgwServiceRegister));
		m_pSHMMgrSrvsRegisterSvrInsIDIndex->add(sServiceRegister.m_iServicesInstanceID,i);
		long lkey = SHMDataMgr::changekey(sServiceRegister.m_szChannelID,m_pSHMMgrSrvsRegisterData->getTotal());

		if (m_pSHMMgrSrvsRegisterindex->get(lkey,&value)) {
			int pos = m_pMgrSrvsRegisterData[value].m_iNext;
			m_pMgrSrvsRegisterData[i].m_iNext = pos;
			m_pMgrSrvsRegisterData[value].m_iNext = i;
		} else {
			m_pSHMMgrSrvsRegisterindex->add(lkey,i);
			m_pMgrSrvsRegisterData[i].m_iNext = 0;
		}
		return 0;
	}
	return MBC_SHM_ADD_DATA_EXIST;
#endif

}

int SHMSrvsRegister::GetServiceRegisterByMap(char * pszChannelID,int iMapID,vector<SgwServiceRegister>& vsServiceRegister)
{
	unsigned int value = 0;
	long lkey = SHMDataMgr::changekey(pszChannelID,m_pSHMMgrSrvsRegisterData->getTotal());
	vsServiceRegister.clear();
	if (m_pSHMMgrSrvsRegisterindex->get(lkey,&value)) {
		while (value) {
			if ((strcmp(pszChannelID,m_pMgrSrvsRegisterData[value].m_szChannelID) == 0)
				&&(iMapID == m_pMgrSrvsRegisterData[value].m_iMapServicesID))
			{
					vsServiceRegister.push_back(m_pMgrSrvsRegisterData[value]);
			}
			value = m_pMgrSrvsRegisterData[value].m_iNext;
		}
	}
	if (vsServiceRegister.size() > 0)
		return 0;
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMSrvsRegister::GetServiceRegisterBySrc(char * pszChannelID,int iSrcID,vector<SgwServiceRegister>& vsServiceRegister)
{
	unsigned int value = 0;
	long lkey = SHMDataMgr::changekey(pszChannelID,m_pSHMMgrSrvsRegisterData->getTotal());
	vsServiceRegister.clear();
	if (m_pSHMMgrSrvsRegisterindex->get(lkey,&value)) {
		while (value) {
			if ((strcmp(pszChannelID,m_pMgrSrvsRegisterData[value].m_szChannelID) == 0)
				&&(iSrcID == m_pMgrSrvsRegisterData[value].m_iSrcServicesID)) {
					vsServiceRegister.push_back(m_pMgrSrvsRegisterData[value]);
			}
			value = m_pMgrSrvsRegisterData[value].m_iNext;
		}
	}
	if (vsServiceRegister.size() > 0)
		return 0;
	return MBC_SHM_QUERY_NO_DATE_FOUND;

}

int SHMParamInsList::GetParamInstanceList(int iServicesInstanceID,vector<SgwParamInstanceList>&vInstanceList)
{
	unsigned int value ;
	vInstanceList.clear();

	if (m_pSHMMgrParamInsListindex->get(iServicesInstanceID,&value)) {

		while (value) {
			vInstanceList.push_back(m_pMgrParamInsListData[value]);
			value = m_pMgrParamInsListData[value].m_iNext;
		}
	} else {
		return MBC_SHM_QUERY_NO_DATE_FOUND;
	}
	return 0;
}

int SHMParamInsList::GetParamInstanceList(int iMappingID,SgwParamInstanceList &oInstanceList)
{
	unsigned int value = 0;
	if (m_pSHMMgrParamInsListMapIDindex->get(iMappingID,&value)) {
		memcpy(&oInstanceList,&m_pMgrParamInsListData[value],sizeof(SgwParamInstanceList));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMParamInsList::AddParamInstanceList(const SgwParamInstanceList &oInstanceList)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrParamInsListLock->P();

	if (!m_pSHMMgrParamInsListMapIDindex->get(oInstanceList.m_iMappingID,&value)) {
		int i = m_pSHMMgrParamInsListData->malloc();

		if (i == 0) {
			m_pSHMMgrParamInsListLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrParamInsListData[i],&oInstanceList,sizeof(SgwParamInstanceList));
		m_pSHMMgrParamInsListMapIDindex->add(oInstanceList.m_iMappingID,i);
		unsigned int temp = 0;

		if (!m_pSHMMgrParamInsListindex->get(oInstanceList.m_iServicesInstanceID,temp)) {
			m_pSHMMgrParamInsListindex->add(oInstanceList.m_iServicesInstanceID,i);
			m_pMgrParamInsListData[i].m_iNext = 0;
		} else {
			int pos = m_pMgrParamInsListData[temp].m_iNext;
			m_pMgrParamInsListData[i].m_iNext = pos;
			m_pMgrParamInsListData[temp].m_iNext = i;
		}
		m_pSHMMgrParamInsListLock->V();
		return 0;
	}
	m_pSHMMgrParamInsListLock->V();
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;

#else

	if (!m_pSHMMgrParamInsListMapIDindex->get(oInstanceList.m_iMappingID,&value)) {
		int i = m_pSHMMgrParamInsListData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrParamInsListData[i],&oInstanceList,sizeof(SgwParamInstanceList));
		m_pSHMMgrParamInsListMapIDindex->add(oInstanceList.m_iMappingID,i);
		unsigned int temp = 0;

		if (!m_pSHMMgrParamInsListindex->get(oInstanceList.m_iServicesInstanceID,temp)) {
			m_pSHMMgrParamInsListindex->add(oInstanceList.m_iServicesInstanceID,i);
			m_pMgrParamInsListData[i].m_iNext = 0;
		} else {
			int pos = m_pMgrParamInsListData[temp].m_iNext;
			m_pMgrParamInsListData[i].m_iNext = pos;
			m_pMgrParamInsListData[temp].m_iNext = i;
		}
		return 0;
	}
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#endif
}

int SHMParamInsList::DeleteParamInstanceList(const SgwParamInstanceList &oInstanceList)
{
	unsigned int value = 0;
#ifndef _DF_LOCL_
	m_pSHMMgrParamInsListLock->P();

	if (m_pSHMMgrParamInsListMapIDindex->get(oInstanceList.m_iMappingID,&value)) {

		unsigned int temp = 0;
		if (m_pSHMMgrParamInsListindex->get(oInstanceList.m_iServicesInstanceID,&temp)) {
			unsigned int fvalue = temp;
			int num = 0;

			while(temp) {
				if (oInstanceList.m_iServicesInstanceID==m_pMgrParamInsListData[temp].m_iServicesInstanceID) {
					if (num == 0 ) {
						int pos =m_pMgrParamInsListData[temp].m_iNext;
						if (pos == 0) {
							m_pSHMMgrParamInsListindex->revokeIdx(oInstanceList.m_iServicesInstanceID,temp);
						} else {
							m_pSHMMgrParamInsListindex->add(oInstanceList.m_iServicesInstanceID,pos);
						}
					} else {
						int pos = m_pMgrParamInsListData[temp].m_iNext;
						m_pMgrParamInsListData[fvalue].m_iNext = pos;
					}
					break;
				}
				num++;
				fvalue =temp;
				temp = m_pMgrParamInsListData[temp].m_iNext;
			}
		}
		m_pSHMMgrParamInsListMapIDindex->revokeIdx(oInstanceList.m_iMappingID,value);
		m_pSHMMgrParamInsListData->erase(value);
		memset(&m_pMgrParamInsListData[value],0,sizeof(SgwParamInstanceList));
		m_pSHMMgrParamInsListLock->V();
		return 0;
	}
	m_pSHMMgrParamInsListLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;

#else
	if (m_pSHMMgrParamInsListMapIDindex->get(oInstanceList.m_iMappingID,&value)) {

		unsigned int temp = 0;
		if (m_pSHMMgrParamInsListindex->get(oInstanceList.m_iServicesInstanceID,&temp)) {
			unsigned int fvalue = temp;
			int num = 0;

			while(temp) {
				if (oInstanceList.m_iServicesInstanceID==m_pMgrParamInsListData[temp].m_iServicesInstanceID) {
					if (num == 0 ) {
						int pos =m_pMgrParamInsListData[temp].m_iNext;
						if (pos == 0) {
							m_pSHMMgrParamInsListindex->revokeIdx(oInstanceList.m_iServicesInstanceID,temp);
						} else {
							m_pSHMMgrParamInsListindex->add(oInstanceList.m_iServicesInstanceID,pos);
						}
					} else {
						int pos = m_pMgrParamInsListData[temp].m_iNext;
						m_pMgrParamInsListData[fvalue].m_iNext = pos;
					}
					break;
				}
				num++;
				fvalue =temp;
				temp = m_pMgrParamInsListData[temp].m_iNext;
			}
		}
		m_pSHMMgrParamInsListMapIDindex->revokeIdx(oInstanceList.m_iMappingID,value);
		m_pSHMMgrParamInsListData->erase(value);
		memset(&m_pMgrParamInsListData[value],0,sizeof(SgwParamInstanceList));
		return 0;
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;

#endif

}

int SHMParamInsList::UpdateParamInstanceList(const SgwParamInstanceList &oInstanceList)
{
	int iRet = 0;
	unsigned int value  = 0;

	if (m_pSHMMgrParamInsListMapIDindex->get(oInstanceList.m_iMappingID,&value)) {
		iRet = DeleteParamInstanceList(m_pMgrParamInsListData[value]);
		if (iRet != 0) {
			return iRet;
		}
		iRet = AddParamInstanceList(oInstanceList);
	}
	return iRet;
}

int SHMEnumArea::GetSgwEnumArea(int iAreaSeq,SgwEnumArea &oSgwEnumArea)
{
	unsigned int value = 0;
	if (m_pSHMMgrSgwEnumAreaindex->get(iAreaSeq,&value)) {
		memcpy(&oSgwEnumArea,&m_pMgrSgwEnumAreaData[value],sizeof(SgwEnumArea));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMEnumArea::GetSgwEnumAreaByOrgId(int iOrgID,SgwEnumArea &oSgwEnumArea)
{
	unsigned int value = 0;
	if (m_pSHMMgrSgwEnumAreaOrgIDindex->get(iOrgID,&value)) {
		memcpy(&oSgwEnumArea,&m_pMgrSgwEnumAreaData[value],sizeof(SgwEnumArea));
		return 0;
	}
	return MBC_SHM_QUERY_NO_DATE_FOUND;
}

int SHMEnumArea::AddSgwEnumArea(const SgwEnumArea &oSgwEnumArea)
{
	unsigned int value = 0;
#ifndef _DF_LOCK_
	m_pSHMMgrSgwEnumAreaLock->P();

	if (!m_pSHMMgrSgwEnumAreaindex->get(oSgwEnumArea.m_iAreaSeq,&value)) {
		int i = m_pSHMMgrSgwEnumAreaData->malloc();

		if (i == 0) {
			m_pSHMMgrSgwEnumAreaLock->V();
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrSgwEnumAreaData[i],&oSgwEnumArea,sizeof(SgwEnumArea));
		m_pSHMMgrSgwEnumAreaindex->add(oSgwEnumArea.m_iAreaSeq,i);
		m_pSHMMgrSgwEnumAreaOrgIDindex->add(oSgwEnumArea.m_iOrgID,i);
		m_pSHMMgrSgwEnumAreaLock->V();
		return 0;
	}
	m_pSHMMgrSgwEnumAreaLock->V();
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;

#else

	if (!m_pSHMMgrSgwEnumAreaindex->get(oSgwEnumArea.m_iAreaSeq,&value)) {
		int i = m_pSHMMgrSgwEnumAreaData->malloc();

		if (i == 0) {
			return MBC_MEM_MALLOC_FAIL;
		}
		memcpy(&m_pMgrSgwEnumAreaData[i],&oSgwEnumArea,sizeof(SgwEnumArea));
		m_pSHMMgrSgwEnumAreaindex->add(oSgwEnumArea.m_iAreaSeq,i);
		m_pSHMMgrSgwEnumAreaOrgIDindex->add(oSgwEnumArea.m_iOrgID,i);
		return 0;
	}
	return MBC_SHM_ERRNO_ADD_DATA_EXIST;
#endif
}

int SHMEnumArea::DeleteSgwEnumArea(const SgwEnumArea &oSgwEnumArea)
{
	unsigned int value = 0;
#ifndef _DF_LOCL_
	m_pSHMMgrSgwEnumAreaLock->P();

	if (m_pSHMMgrSgwEnumAreaindex->get(oSgwEnumArea.m_iAreaSeq,&value)) {
		m_pSHMMgrSgwEnumAreaindex->revokeIdx(oSgwEnumArea.m_iAreaSeq,value);
		m_pSHMMgrSgwEnumAreaOrgIDindex->revokeIdx(oSgwEnumArea.m_iOrgID,value);
		m_pSHMMgrSgwEnumAreaData->erase(value);
		memset(&m_pMgrSgwEnumAreaData[value],0,sizeof(SgwEnumArea));
		m_pSHMMgrSgwEnumAreaLock->V();
		return 0;
	}
	m_pSHMMgrSgwEnumAreaLock->V();
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;

#else
	if (m_pSHMMgrSgwEnumAreaindex->get(oSgwEnumArea.m_iAreaSeq,&value)) {
		m_pSHMMgrSgwEnumAreaindex->revokeIdx(oSgwEnumArea.m_iAreaSeq,value);
		m_pSHMMgrSgwEnumAreaOrgIDindex->revokeIdx(oSgwEnumArea.m_iOrgID,value);
		m_pSHMMgrSgwEnumAreaData->erase(value);
		memset(&m_pMgrSgwEnumAreaData[value],0,sizeof(SgwEnumArea));
		return 0;
	}
	return MBC_SHM_ERRNO_DEL_DATA_NOTEXIST;

#endif

}

int SHMEnumArea::UpdateSgwEnumArea(const SgwEnumArea &oSgwEnumArea)
{
	int iRet = 0;
	unsigned int value  = 0;

	if (m_pSHMMgrSgwEnumAreaindex->get(oSgwEnumArea.m_iAreaSeq,&value)) {
		iRet = DeleteSgwEnumArea(m_pMgrSgwEnumAreaData[value]);
		if (iRet != 0) {
			return iRet;
		}
		iRet = AddSgwEnumArea(oSgwEnumArea);
	}
	return iRet;
}
