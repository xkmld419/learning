#include "SHMDataMgr.h"
#include "MBC_ABM.h"
#include "ThreeLogGroup.h"




//获取共享内存核心参数，X是局部变量,Y是全局变量
int  GETVALUE(int X,int Y)
{
	if ( X<=0) {
		if (Y<=0){
			THROW(MBC_UserInfoCtl+100);
		} else {
			return Y;
		}
	} else {
		return X;
	}
}


unsigned long SHMDataMgr::trim(const char *sHead)
{
	if (sHead == NULL) return ;
	char sTemp[128] = {0};
	int i = 0;
	int len = strlen(sHead);

	while ((sHead[i]<'1'||sHead[i]>'9')&&(++i)<len);
	return atol(sHead+i);
}

long SHMDataMgr::changekey(const char *_skey,int _iModValue)
{
	if (_skey == NULL||strlen(_skey) == 0){
		return -1;
	}

	unsigned long ulKey = 0;
	int len = strlen(_skey);
	static long sys[11] ={1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000};

	for (int i =0; i<len&&i<11 ;i++){
		ulKey += sys[i]*(_skey[i]%10);
	}

	return (ulKey%(_iModValue-1));
}

//创建共享内存数据区和索引区
int SHMDataMgr::createContentFilteringSHM(int _iExtenSzie)
{
	long _lCount = 0;

	if (!m_pSHMMgrContextData->exist()) {
		_lCount = GETVALUE(_SHM_CONTEXTDATA_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)+\
			_iExtenSzie*GETVALUE(_SHM_CONTEXTDATA_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前欲分配共享内存空间大小
		long _lMaxCount = GETVALUE(_SHM_CONTEXTDATA_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//可分配最大值

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建内容过滤共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrContextData->create(_lMaxCount);
			m_pMgrContextData = (ContextData *)(*m_pSHMMgrContextData);

			return _lMaxCount;
		} else {
			m_pSHMMgrContextData->create(_lCount);
			m_pMgrContextData = (ContextData *)(*m_pSHMMgrContextData);

		}
	} else {
		cout<<"(context_filter)共享内存已存在"<<endl;
		return 0;
	}
	return _lCount;
}

//数据全量加载
int SHMDataMgr::loadContentFiltering()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select filter_keyword_seq, \
								  illegal_keyword  \
								  from context_filter_keyword order by filter_keyword_seq");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrContextData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载context_filter_keyword信息共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrContextData[i].m_iFilterSeq = GETVALUE_INT(slQry,0);
			strncpy(m_pMgrContextData[i].m_strIllegalKeyword,GETVALUE_STRING(slQry,1),sizeof(m_pMgrContextData[i].m_strIllegalKeyword)-1);
		}
	}
	slQry.close();
	if (m_pSHMMgrContextData->getCount() == 0) {
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadContentFilteringSHM()
{
	if (m_pSHMMgrContextData->exist()) {
		m_pSHMMgrContextData->remove();
	}
	m_pSHMMgrContextDataLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryContentFilteringData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrContextData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrContextData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrContextData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"context_filter(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrContextData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrContextData->getTotal()+1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrContextData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrContextData->getCount());
	return 0;
}


//创建共享内存数据区和索引区
int SHMDataMgr::createMsgMapSHM(int _iExtenSzie)
{
	//求出当前创建的内存大小=初始值+每次扩展大小*扩展次数
	long _lCount = GETVALUE(_SHM_MSGMAPINFO_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_MSGMAPINFO_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);
	long _lMaxCount = GETVALUE(_SHM_MSGMAPINFO_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrMsgMapInfoData->exist()) {
		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建消息映射共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrMsgMapInfoData->create(_lMaxCount);
			m_pMgrMsgMapInfoData = (SMsgMapInfo *)(*m_pSHMMgrMsgMapInfoData);

			if (!m_pSHMMgrMsgMapInfoIndex->exist()) {
				m_pSHMMgrMsgMapInfoIndex->create(_lMaxCount);
			} else{
				cout<<"(tab_msg_map)共享内存索引区已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrMsgMapInfoData->create(_lCount);
			m_pMgrMsgMapInfoData = (SMsgMapInfo *)(*m_pSHMMgrMsgMapInfoData);

			if (!m_pSHMMgrMsgMapInfoIndex->exist()) {
				m_pSHMMgrMsgMapInfoIndex->create(_lCount);
			} else{
				cout<<"(tab_msg_map)共享内存索引区已经存在"<<endl;
			}

		}
	} else {
		cout<<"(tab_msg_map)共享内存数据区已经存在"<<endl;
		if (!m_pSHMMgrMsgMapInfoIndex->exist()) {
			m_pSHMMgrMsgMapInfoIndex->create(_lCount);
		} else {
			cout<<"(tab_msg_map)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//数据全量加载
int SHMDataMgr::loadMsgMap()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select MAP_ID,Data_Type,SELECTIVE,Msg_Type, \
								  Dcc_Node,Data_Node,Service_Context_ID,Is_Filter, \
								  nvl(Data_Limited,0),  \
								  nvl(Dcc_Code,-1),nvl(Dcc_Vendor,-1),Xml_CData,IS_ACCTID \
								  from TAB_MSG_MAP order by MAP_ID");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrMsgMapInfoData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息TAB_MSG_MAP共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrMsgMapInfoData[i].m_iID = GETVALUE_INT(slQry,0);
			m_pMgrMsgMapInfoData[i].m_iDataType = GETVALUE_INT(slQry,1); ;
			strncpy(m_pMgrMsgMapInfoData[i].m_szSelective,GETVALUE_STRING(slQry,2),sizeof(m_pMgrMsgMapInfoData[i].m_szSelective)-1);
			m_pMgrMsgMapInfoData[i].m_iMsgType = GETVALUE_INT(slQry,3);
			strncpy(m_pMgrMsgMapInfoData[i].m_szDccNode,GETVALUE_STRING(slQry,4),sizeof(m_pMgrMsgMapInfoData[i].m_szDccNode)-1);
			strncpy(m_pMgrMsgMapInfoData[i].m_szDataNode,GETVALUE_STRING(slQry,5),sizeof(m_pMgrMsgMapInfoData[i].m_szDataNode)-1);
			strncpy(m_pMgrMsgMapInfoData[i].m_szServiceContextID,GETVALUE_STRING(slQry,6),sizeof(m_pMgrMsgMapInfoData[i].m_szServiceContextID)-1);
			m_pMgrMsgMapInfoData[i].m_iContextFilter = GETVALUE_INT(slQry,7);
			m_pMgrMsgMapInfoData[i].m_iDataLimited = GETVALUE_INT(slQry,8);
			strncpy(m_pMgrMsgMapInfoData[i].m_szDccCode,GETVALUE_STRING(slQry,9),sizeof(m_pMgrMsgMapInfoData[i].m_szDccCode)-1);
			strncpy(m_pMgrMsgMapInfoData[i].m_szDccVendor,GETVALUE_STRING(slQry,10),sizeof(m_pMgrMsgMapInfoData[i].m_szDccVendor)-1);
			m_pMgrMsgMapInfoData[i].m_iXmlCData = GETVALUE_INT(slQry,11);
#ifdef DEF_SICHUAN
			m_pMgrMsgMapInfoData[i].m_bIsAcctID = GETVALUE_INT(slQry,12);
#endif
			m_pSHMMgrMsgMapInfoIndex->add (m_pMgrMsgMapInfoData[i].m_iID,i);
		}
	}
	slQry.close();
	if (m_pSHMMgrMsgMapInfoData->getCount() == 0 ) {
		return MBC_SHM_LOAD_NO_DATA;
	}

	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadMsgMapSHM()
{

	if (m_pSHMMgrMsgMapInfoData->exist()) {
		m_pSHMMgrMsgMapInfoData->remove();
	}

	if (m_pSHMMgrMsgMapInfoIndex->exist()) {
		m_pSHMMgrMsgMapInfoIndex->remove();
	}
	m_pSHMMgrMsgMapInfoLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryMsgMapData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrMsgMapInfoData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrMsgMapInfoData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrMsgMapInfoData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"tab_msg_map(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrMsgMapInfoData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrMsgMapInfoData->getTotal()+1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrMsgMapInfoData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrMsgMapInfoData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryMsgMapIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrMsgMapInfoIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrMsgMapInfoIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrMsgMapInfoIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"tab_msg_map(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrMsgMapInfoIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrMsgMapInfoData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrMsgMapInfoIndex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createServiceTxtListSHM(int _iExtenSzie)
{
	long _lCount = GETVALUE(_SHM_SERVICETXTLIST_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)+_iExtenSzie*GETVALUE(_SHM_SERVICETXTLIST_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SERVICETXTLIST_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小


	if (!m_pSHMMgrServiceTxtListData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建能力列表共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrServiceTxtListData->create(_lMaxCount);
			m_pMgrServiceTxtListData = (ServiceContextListData *)(*m_pSHMMgrServiceTxtListData);
			if (!m_pSHMMgrServiceTxtListIndex->exist()) {
				m_pSHMMgrServiceTxtListIndex->create(_lMaxCount);
			} else {
				cout<<"(service_context_list)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvTxtListIDIndex->exist()) {
				m_pSHMMgrSrvTxtListIDIndex->create(_lMaxCount);
			} else {
				cout<<"(service_context_list)共享内存索引区B已经存在"<<endl;
			}


			return _lMaxCount;
		} else {
			m_pSHMMgrServiceTxtListData->create(_lCount);
			m_pMgrServiceTxtListData = (ServiceContextListData *)(*m_pSHMMgrServiceTxtListData);

			if (!m_pSHMMgrServiceTxtListIndex->exist()) {
				m_pSHMMgrServiceTxtListIndex->create(_lCount);
			} else {
				cout<<"(service_context_list)共享内存索引区已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvTxtListIDIndex->exist()) {
				m_pSHMMgrSrvTxtListIDIndex->create(_lMaxCount);
			} else {
				cout<<"(service_context_list)共享内存索引区B已经存在"<<endl;
			}

		}
	} else {
		cout<<"(service_context_list)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrServiceTxtListIndex->exist()) {
			m_pSHMMgrServiceTxtListIndex->create(_lCount);
		} else {
			cout<<"(service_context_list)共享内存索引区已经存在"<<endl;
		}
		if (!m_pSHMMgrSrvTxtListIDIndex->exist()) {
			m_pSHMMgrSrvTxtListIDIndex->create(_lMaxCount);
		} else {
			cout<<"(service_context_list)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//数据全量加载
int SHMDataMgr::loadServiceTxtList()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lKey = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select service_context_list_id, \
								  service_context_id,active,user_seq, \
								  context_issign,nvl(flow_id,-1) \
								  from service_context_list order by service_context_id");
	slQry.setSQL(sSQL);
	slQry.open();
	unsigned int value = 0;

	while (slQry.next()) {
		int i = m_pSHMMgrServiceTxtListData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息service_context_list共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrServiceTxtListData[i].m_iServiceContextListId = GETVALUE_INT(slQry,0);
			strncpy(m_pMgrServiceTxtListData[i].m_strServiceContextId,GETVALUE_STRING(slQry,1),sizeof(m_pMgrServiceTxtListData[i].m_strServiceContextId)-1);
			m_pMgrServiceTxtListData[i].m_iActive = GETVALUE_INT(slQry,2);
			m_pMgrServiceTxtListData[i].m_iUserSeq = GETVALUE_INT(slQry,3);
			m_pMgrServiceTxtListData[i].m_iSign = GETVALUE_INT(slQry,4);
			strncpy(m_pMgrServiceTxtListData[i].m_strFlowId,GETVALUE_STRING(slQry,5),sizeof(m_pMgrServiceTxtListData[i].m_strFlowId)-1);

			m_pSHMMgrSrvTxtListIDIndex->add(m_pMgrServiceTxtListData[i].m_iServiceContextListId,i);

			lKey = changekey(m_pMgrServiceTxtListData[i].m_strServiceContextId,m_pSHMMgrServiceTxtListData->getTotal());

			if (m_pSHMMgrServiceTxtListIndex->get(lKey,&value)) {
				int tmp =m_pMgrServiceTxtListData[value].m_iNext;

				if (tmp !=0 ) {
					m_pMgrServiceTxtListData[value].m_iNext = i;
					m_pMgrServiceTxtListData[i].m_iNext = tmp;
				} else {
					m_pMgrServiceTxtListData[value].m_iNext = i;
					m_pMgrServiceTxtListData[i].m_iNext = 0;
				}
			} else {
				m_pSHMMgrServiceTxtListIndex->add (lKey,i);
				m_pMgrServiceTxtListData[i].m_iNext = 0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrServiceTxtListData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}

	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadServiceTxtListSHM()
{
	if (m_pSHMMgrServiceTxtListData->exist()) {
		m_pSHMMgrServiceTxtListData->remove();
	}

	if (m_pSHMMgrServiceTxtListIndex->exist()) {
		m_pSHMMgrServiceTxtListIndex->remove();
	}

	if (m_pSHMMgrSrvTxtListIDIndex->exist()) {
		m_pSHMMgrSrvTxtListIDIndex->remove();
	}
	m_pSHMMgrServiceTxtListLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryServiceTxtListData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServiceTxtListData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServiceTxtListData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrServiceTxtListData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_context_list(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServiceTxtListData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrServiceTxtListData->getTotal()+1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServiceTxtListData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrServiceTxtListData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryServiceTxtListIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServiceTxtListIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServiceTxtListIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrServiceTxtListIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_context_list(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServiceTxtListIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServiceTxtListData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrServiceTxtListIndex->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querySrvTxtListIDIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvTxtListIDIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvTxtListIDIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvTxtListIDIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_context_list(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvTxtListIDIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSrvTxtListIDIndex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrSrvTxtListIDIndex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createStreamCtrlSHM(int _iExtenSzie)
{
	long _lCount = GETVALUE(_SHM_STREAMCTRLINFO_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)+_iExtenSzie*GETVALUE(_SHM_STREAMCTRLINFO_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_STREAMCTRLINFO_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小


	if (!m_pSHMMgrStreamCtrlInfoData->exist()) {
		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建流控制信息共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrStreamCtrlInfoData->create(_lMaxCount);
			m_pMgrStreamCtrlInfoData = (StreamControlInfo *)(*m_pSHMMgrStreamCtrlInfoData);
			if (!m_pSHMMgrStreamCtrlInfoIndex->exist()) {
				m_pSHMMgrStreamCtrlInfoIndex->create(_lMaxCount);
			}else {
				cout<<"(stream_ctrl_info)共享内存索引区已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrStreamCtrlInfoData->create(_lCount);
			m_pMgrStreamCtrlInfoData = (StreamControlInfo *)(*m_pSHMMgrStreamCtrlInfoData);
			if (!m_pSHMMgrStreamCtrlInfoIndex->exist()) {
				m_pSHMMgrStreamCtrlInfoIndex->create(_lCount);
			}else {
				cout<<"(stream_ctrl_info)共享内存索引区已经存在"<<endl;
			}

		}
	} else {
		cout<<"(stream_ctrl_info)共享内存数据区已经存在"<<endl;
		if (!m_pSHMMgrStreamCtrlInfoIndex->exist()) {
			m_pSHMMgrStreamCtrlInfoIndex->create(_lCount);
		} else {
			cout<<"(stream_ctrl_info)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//数据全量加载
int SHMDataMgr::loadStreamCtrl()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	char sTemp[1024] = {0};
	long lKey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select CTRL_CYCLE,USER_NAME,SERVICE_CONTEXT_ID,LEARN_SELF, \
								  WARNING_NUMBER,WARNING_FLOW,DISCONNECT_NUM,DISCONNECT_FLOW,RE_CONNECT \
								  from STREAM_CTRL_INFO order by CTRL_CYCLE");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrStreamCtrlInfoData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息STREAM_CTRL_INFO共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrStreamCtrlInfoData[i].m_iCtrlCycle = GETVALUE_INT(slQry,0);
			strncpy(m_pMgrStreamCtrlInfoData[i].m_sUserName,GETVALUE_STRING(slQry,1),sizeof(m_pMgrStreamCtrlInfoData[i].m_sUserName)-1);
			strncpy(m_pMgrStreamCtrlInfoData[i].m_sSerConId,GETVALUE_STRING(slQry,2),sizeof(m_pMgrStreamCtrlInfoData[i].m_sSerConId)-1);
			m_pMgrStreamCtrlInfoData[i].m_iLenarnSelf = GETVALUE_INT(slQry,3);
			m_pMgrStreamCtrlInfoData[i].m_iWarningNumber = GETVALUE_INT(slQry,4);
			m_pMgrStreamCtrlInfoData[i].m_iWarningFlow = GETVALUE_INT(slQry,5);
			m_pMgrStreamCtrlInfoData[i].m_iDisconnectNum = GETVALUE_INT(slQry,6);
			m_pMgrStreamCtrlInfoData[i].m_iDisconnectFlow = GETVALUE_INT(slQry,7);
			m_pMgrStreamCtrlInfoData[i].m_iReConnect = GETVALUE_INT(slQry,8);
			memset(sTemp,0,sizeof(sTemp));
			snprintf(sTemp,sizeof(sTemp)-1,"%s%s",m_pMgrStreamCtrlInfoData[i].m_sUserName,m_pMgrStreamCtrlInfoData[i].m_sSerConId);
			lKey = changekey(sTemp,m_pSHMMgrStreamCtrlInfoData->getTotal());

			if (m_pSHMMgrStreamCtrlInfoIndex->get(lKey,&value)) {
				int tmp =m_pMgrStreamCtrlInfoData[value].m_iNext;

				if (tmp !=0 ) {
					m_pMgrStreamCtrlInfoData[value].m_iNext = i;
					m_pMgrStreamCtrlInfoData[i].m_iNext = tmp;
				} else {
					m_pMgrStreamCtrlInfoData[value].m_iNext = i;
					m_pMgrStreamCtrlInfoData[i].m_iNext = 0;
				}
			} else {
				m_pSHMMgrStreamCtrlInfoIndex->add (lKey,i);
				m_pMgrStreamCtrlInfoData[i].m_iNext = 0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrStreamCtrlInfoData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}

	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadStreamCtrlSHM()
{
	if (m_pSHMMgrStreamCtrlInfoData->exist()) {
		m_pSHMMgrStreamCtrlInfoData->remove();
	}
	if (m_pSHMMgrStreamCtrlInfoIndex->exist()) {
		m_pSHMMgrStreamCtrlInfoIndex->remove();
	}
	m_pSHMMgrStreamCtrlInfoLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryStreamCtrlData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrStreamCtrlInfoData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrStreamCtrlInfoData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrStreamCtrlInfoData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"stream_ctrl_info(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrStreamCtrlInfoData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrStreamCtrlInfoData->getTotal()+1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrStreamCtrlInfoData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrStreamCtrlInfoData->getCount());

	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryStreamCtrlIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrStreamCtrlInfoIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrStreamCtrlInfoIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrStreamCtrlInfoIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"stream_ctrl_info(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrStreamCtrlInfoIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrStreamCtrlInfoData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrStreamCtrlInfoIndex->getCount());

	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createStreamCtrlDataSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_STREAMCTRLDATA_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_STREAMCTRLDATA_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_STREAMCTRLDATA_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrStreamCtrlData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建流控制数据共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrStreamCtrlData->create(_lMaxCount);
			m_pMgrStreamCtrlData = (StreamControlData *)(*m_pSHMMgrStreamCtrlData);
			if (!m_pSHMMgrStreamCtrlDataIndex->exist()) {
				m_pSHMMgrStreamCtrlDataIndex->create(_lMaxCount);
			}else {
				cout<<"(stream_ctrl_data)共享内存索引已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrStreamCtrlData->create(_lCount);
			m_pMgrStreamCtrlData = (StreamControlData *)(*m_pSHMMgrStreamCtrlData);
			if (!m_pSHMMgrStreamCtrlDataIndex->exist()) {
				m_pSHMMgrStreamCtrlDataIndex->create(_lCount);
			}else {
				cout<<"(stream_ctrl_data)共享内存索引已经存在"<<endl;
			}

		}
	} else {
		cout<<"(stream_ctrl_data)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrStreamCtrlDataIndex->exist()) {
			m_pSHMMgrStreamCtrlDataIndex->create(_lCount);
		} else {
			cout<<"(stream_ctrl_data)共享内存索引已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadStreamCtrlDataSHM()
{
	if (m_pSHMMgrStreamCtrlData->exist()) {
		m_pSHMMgrStreamCtrlData->remove();
	}

	if (m_pSHMMgrStreamCtrlDataIndex->exist()) {
		m_pSHMMgrStreamCtrlDataIndex->remove();
	}
	m_pSHMMgrStreamCtrlDataLock->close();
}

int SHMDataMgr::loadStreamCtrlData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	char sTemp[1024] = {0};
	long lKey = 0;
	unsigned int value = 0;
	int _iMon = 0;
	int _iMonNum =0 ;
	int _iMonFlow = 0 ;
	char _sUserName[MAX_USER_NAME_LEN + 1];
	char _sSerConId[MAX_SERVICE_CONTEXT_ID_SIZE + 1];
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select USER_NAME,SERVICE_CONTEXT_ID,month, \
								  month_num,month_flow from STREAM_MONTH_DATA");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		memset(_sUserName,0,sizeof(_sUserName));
		memset(_sSerConId,0,sizeof(_sSerConId));
		strncpy(_sUserName,GETVALUE_STRING(slQry,0),sizeof(_sUserName)-1);
		strncpy(_sSerConId,GETVALUE_STRING(slQry,1),sizeof(_sSerConId)-1);
		_iMon = GETVALUE_INT(slQry,2);
		_iMonNum = GETVALUE_INT(slQry,3);
		_iMonFlow = GETVALUE_INT(slQry,4);

		memset(sTemp,0,sizeof(sTemp));
		snprintf(sTemp,sizeof(sTemp)-1,"%s%s",_sUserName,_sSerConId);
		lKey = changekey(sTemp,m_pSHMMgrStreamCtrlData->getTotal());

		if (m_pSHMMgrStreamCtrlDataIndex->get(lKey,&value)) {
			int _iHead = value;

			while (value) {
				if (strcmp(_sUserName,m_pMgrStreamCtrlData[value].m_sUserName) == 0 &&
					strcmp(_sSerConId,m_pMgrStreamCtrlData[value].m_sSerConId) == 0 ) {
						m_pMgrStreamCtrlData[value].m_dMonthData[_iMon%100-1][0]=_iMonNum;
						m_pMgrStreamCtrlData[value].m_dMonthData[_iMon%100-1][1]=_iMonFlow;
						break;
				}
				value = m_pMgrStreamCtrlData[value].m_iNext;
			}

			if (value == 0) {
				int i = m_pSHMMgrStreamCtrlData->malloc();

				if (i == 0) {
					ThreeLogGroup _oLogObj;
					_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息STREAM_MONTH_DATA共享内存空间不足");
					return MBC_MEM_SPACE_SMALL;
				} else {
					strcpy(m_pMgrStreamCtrlData[i].m_sUserName,_sUserName);
					strcpy(m_pMgrStreamCtrlData[i].m_sSerConId,_sSerConId);
					m_pMgrStreamCtrlData[i].m_dMonthData[_iMon%100-1][0]=_iMonNum;
					m_pMgrStreamCtrlData[i].m_dMonthData[_iMon%100-1][1]=_iMonFlow;

					int tmp =m_pMgrStreamCtrlData[_iHead].m_iNext;
					m_pMgrStreamCtrlData[_iHead].m_iNext = i;
					m_pMgrStreamCtrlData[i].m_iNext = tmp;

				}
			}
		} else {
			int i = m_pSHMMgrStreamCtrlData->malloc();

			if (i == 0) {
				ThreeLogGroup _oLogObj;
				_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息STREAM_MONTH_DATA共享内存空间不足");
				return MBC_MEM_SPACE_SMALL;
			} else {
				strcpy(m_pMgrStreamCtrlData[i].m_sUserName,_sUserName);
				strcpy(m_pMgrStreamCtrlData[i].m_sSerConId,_sSerConId);
				m_pMgrStreamCtrlData[i].m_dMonthData[_iMon%100-1][0]=_iMonNum;
				m_pMgrStreamCtrlData[i].m_dMonthData[_iMon%100-1][1]=_iMonFlow;
				m_pSHMMgrStreamCtrlDataIndex->add (lKey,i);
				m_pMgrStreamCtrlData[i].m_iNext = 0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrStreamCtrlData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}

	return 0;
}

//查询共享内存数据区信息
int SHMDataMgr::queryStreamCtrlDataData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrStreamCtrlData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrStreamCtrlData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrStreamCtrlData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"stream_ctrl_data(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrStreamCtrlData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrStreamCtrlData->getTotal()+1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrStreamCtrlData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrStreamCtrlData->getCount());

	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryStreamCtrlDataIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrStreamCtrlDataIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrStreamCtrlDataIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrStreamCtrlDataIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"stream_ctrl_data(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrStreamCtrlDataIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrStreamCtrlData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrStreamCtrlDataIndex->getCount());

	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createUserInfoSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_USERINFODATA_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_USERINFODATA_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_USERINFODATA_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrUserInfoData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建用户信息共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrUserInfoData->create(_lMaxCount);
			m_pMgrUserInfoData = (UserInfoData *)(*m_pSHMMgrUserInfoData);
			if (!m_pSHMMgrUserInfoIndex->exist()) {
				m_pSHMMgrUserInfoIndex->create(_lMaxCount);

			} else {
				cout<<"(userinfo)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrUserInfoIndex_A->exist()){
				m_pSHMMgrUserInfoIndex_A->create(_lMaxCount);
			} else {
				cout<<"(userinfo)共享内存索引区B已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrUserInfoData->create(_lCount);
			m_pMgrUserInfoData = (UserInfoData *)(*m_pSHMMgrUserInfoData);

			if (!m_pSHMMgrUserInfoIndex->exist()) {
				m_pSHMMgrUserInfoIndex->create(_lCount);

			} else {
				cout<<"(userinfo)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrUserInfoIndex_A->exist()){
				m_pSHMMgrUserInfoIndex_A->create(_lCount);
			} else {
				cout<<"(userinfo)共享内存索引区B已经存在"<<endl;
			}

		}
	} else {
		cout<<"(userinfo)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrUserInfoIndex->exist()) {
			m_pSHMMgrUserInfoIndex->create(_lCount);

		} else {
			cout<<"(userinfo)共享内存索引区A已经存在"<<endl;
		}

		if (!m_pSHMMgrUserInfoIndex_A->exist()){
			m_pSHMMgrUserInfoIndex_A->create(_lCount);
		} else {
			cout<<"(userinfo)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadUserInfoSHM()
{
	if (m_pSHMMgrUserInfoData->exist()) {
		m_pSHMMgrUserInfoData->remove();
	}

	if (m_pSHMMgrUserInfoIndex->exist()) {
		m_pSHMMgrUserInfoIndex->remove();
	}

	if (m_pSHMMgrUserInfoIndex_A->exist()) {
		m_pSHMMgrUserInfoIndex_A->remove();
	}
	m_pSHMMgrUserInfoDataLock->close();
}
//表:UserInfo 方法：select
//数据全量加载
int SHMDataMgr::loadUserInfo()
{
	unsigned int value = 0;
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	DEFINE_SGW_QUERY(slQry);
#ifndef __SQLLITE__
	snprintf (sSQL,sizeof(sSQL)-1,"select user_seq,nvl(host,-1),nvl(realm,-1),nvl(ip_addr,-1), \
								  nvl(port,-1),nvl(adaptive_info_id,-1),nvl(status,-1), \
								  nvl(register_time,-1),nvl(update_time,-1),nvl(passwd,-1),\
								  nvl(is_encrypt,-1),nvl(user_code,-1),nvl(netflag,-1),nvl(active,-1), \
								  nvl(timeout,-1), nvl(ShortLinkFlag,-1),nvl(action,-1), \
								  nvl(ExternNetCertPath,-1),SGWCertPath,nvl(BusiPublicKey,-1),nvl(BusiPrivateKey,-1), \
								  to_char(BusiKeyExpDate,'YYYYMMDDHH24MISS') BusiKeyExpDate, nvl(netinfo_seq,-1),nvl(padding_type,3),nvl(priority_level,-1), \
								  nvl(padding_type,3), nvl(soap_url,-1),nvl(sgw_soap_port,-1),nvl(pri_update_time,-1) \
								  from UserInfo order by user_seq");
#else
	snprintf (sSQL,sizeof(sSQL)-1,"select user_seq,nvl(host,-1),nvl(realm,-1),nvl(ip_addr,-1), \
								  nvl(port,-1),nvl(adaptive_info_id,-1),nvl(status,-1), \
								  nvl(register_time,-1),nvl(update_time,-1),nvl(passwd,-1),\
								  nvl(is_encrypt,-1),nvl(user_code,-1),nvl(netflag,-1),nvl(active,-1), \
								  nvl(timeout,-1), nvl(ShortLinkFlag,-1),nvl(action,-1), \
								  nvl(ExternNetCertPath,-1),SGWCertPath,nvl(BusiPublicKey,-1),nvl(BusiPrivateKey,-1), \
								  nvl(BusiKeyExpDate,-1),nvl(netinfo_seq,-1),nvl(padding_type,3),nvl(priority_level,-1), \
								  nvl(padding_type,3), nvl(soap_url,-1),nvl(sgw_soap_port,-1),nvl(pri_update_time,-1) \
								  from UserInfo order by user_seq");
#endif
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrUserInfoData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息UserInfo共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrUserInfoData[i].m_iUserSeq = GETVALUE_INT(slQry,0);
			strncpy(m_pMgrUserInfoData[i].m_strUserName,GETVALUE_STRING(slQry,1),sizeof(m_pMgrUserInfoData[i].m_strUserName)-1);
			strncpy(m_pMgrUserInfoData[i].m_strRealm,GETVALUE_STRING(slQry,2),sizeof(m_pMgrUserInfoData[i].m_strRealm)-1);
			strncpy(m_pMgrUserInfoData[i].m_strIpAddr,GETVALUE_STRING(slQry,3),sizeof(m_pMgrUserInfoData[i].m_strIpAddr)-1);
			m_pMgrUserInfoData[i].m_iPort = GETVALUE_INT(slQry,4);
			m_pMgrUserInfoData[i].m_iAdaptiveInfoId = GETVALUE_INT(slQry,5);
			m_pMgrUserInfoData[i].m_iStatus = GETVALUE_INT(slQry,6);
			strncpy(m_pMgrUserInfoData[i].m_strRegisterTime,GETVALUE_STRING(slQry,7),sizeof(m_pMgrUserInfoData[i].m_strRegisterTime)-1);
			strncpy(m_pMgrUserInfoData[i].m_strUpdateTime,GETVALUE_STRING(slQry,8),sizeof(m_pMgrUserInfoData[i].m_strUpdateTime)-1);
			strncpy(m_pMgrUserInfoData[i].m_strPasswd,GETVALUE_STRING(slQry,9),sizeof(m_pMgrUserInfoData[i].m_strPasswd)-1);
			m_pMgrUserInfoData[i].m_bIsEncrypt = (bool)GETVALUE_INT(slQry,10);
			strncpy(m_pMgrUserInfoData[i].m_strUserCode,GETVALUE_STRING(slQry,11),sizeof(m_pMgrUserInfoData[i].m_strUserCode)-1);
			m_pMgrUserInfoData[i].m_iNetFlag = GETVALUE_INT(slQry,12);
			m_pMgrUserInfoData[i].m_iActive = GETVALUE_INT(slQry,13);
			m_pMgrUserInfoData[i].m_iTimeOut = GETVALUE_INT(slQry,14);
			m_pMgrUserInfoData[i].m_iShortLinkFlag = GETVALUE_INT(slQry,15);
			m_pMgrUserInfoData[i].m_iAction = GETVALUE_INT(slQry,16);
			strncpy(m_pMgrUserInfoData[i].m_strExternNetCertPath,GETVALUE_STRING(slQry,17),sizeof(m_pMgrUserInfoData[i].m_strExternNetCertPath)-1);
			strncpy(m_pMgrUserInfoData[i].m_strSGWCertPath,GETVALUE_STRING(slQry,18),sizeof(m_pMgrUserInfoData[i].m_strSGWCertPath)-1);
			strncpy(m_pMgrUserInfoData[i].m_sBusiPublicKey,GETVALUE_STRING(slQry,19),sizeof(m_pMgrUserInfoData[i].m_sBusiPublicKey)-1);
			strncpy(m_pMgrUserInfoData[i].m_sBusiPrivateKey,GETVALUE_STRING(slQry,20),sizeof(m_pMgrUserInfoData[i].m_sBusiPrivateKey)-1);
			strncpy(m_pMgrUserInfoData[i].m_sBusiKeyExpDate,GETVALUE_STRING(slQry,21),sizeof(m_pMgrUserInfoData[i].m_sBusiKeyExpDate)-1);
			m_pMgrUserInfoData[i].m_iNetinfoSeq = GETVALUE_INT(slQry,22);
			m_pMgrUserInfoData[i].m_iPaddingType = GETVALUE_INT(slQry,23);
			m_pMgrUserInfoData[i].m_iPriority_level = GETVALUE_INT(slQry,24);
			m_pMgrUserInfoData[i].m_iPaddingType = GETVALUE_INT(slQry,25);
			strncpy(m_pMgrUserInfoData[i].m_strSoapUrl,GETVALUE_STRING(slQry,26),sizeof(m_pMgrUserInfoData[i].m_strSoapUrl)-1);
			m_pMgrUserInfoData[i].m_iSgwSoapPort = GETVALUE_INT(slQry,27);
			strncpy(m_pMgrUserInfoData[i].m_strPriUpdateTime,GETVALUE_STRING(slQry,28),sizeof(m_pMgrUserInfoData[i].m_strPriUpdateTime)-1);

			m_pSHMMgrUserInfoIndex_A->add(m_pMgrUserInfoData[i].m_iUserSeq,i);

			lkey = changekey(m_pMgrUserInfoData[i].m_strUserName,m_pSHMMgrUserInfoData->getTotal());

			if (m_pSHMMgrUserInfoIndex->get(lkey,&value)) {
				int tmp =m_pMgrUserInfoData[value].m_iNext;

				if (tmp !=0 ) {
					m_pMgrUserInfoData[value].m_iNext = i;
					m_pMgrUserInfoData[i].m_iNext = tmp;
				} else {
					m_pMgrUserInfoData[value].m_iNext = i;
					m_pMgrUserInfoData[i].m_iNext = 0;
				}
			} else {
				m_pSHMMgrUserInfoIndex->add (lkey,i);
				m_pMgrUserInfoData[i].m_iNext = 0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrUserInfoData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}


//查询共享内存数据区信息
int SHMDataMgr::queryUserInfoData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrUserInfoData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrUserInfoData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrUserInfoData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"userInfo(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrUserInfoData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrUserInfoData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrUserInfoData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrUserInfoData->getCount());

	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryUserInfoIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrUserInfoIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrUserInfoIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrUserInfoIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"userInfo(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrUserInfoIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrUserInfoData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrUserInfoIndex->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryUserInfoIndex_A(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrUserInfoIndex_A->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrUserInfoIndex_A->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrUserInfoIndex_A->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"userInfo(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrUserInfoIndex_A->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrUserInfoData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrUserInfoIndex_A->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createSgwSlaqueRelSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SGWSLAQUEUE_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SGWSLAQUEUE_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SGWSLAQUEUE_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrSgwSlaqueRelData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建SgwSlaQueue共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrSgwSlaqueRelData->create(_lMaxCount);
			m_pMgrSgwSlaqueRelData = (SgwSlaQueue *)(*m_pSHMMgrSgwSlaqueRelData);

			if (!m_pSHMMgrSgwSlaqueRelIndex->exist()) {
				m_pSHMMgrSgwSlaqueRelIndex->create(_lMaxCount);
			} else {
				cout<<"(sgw_sla_que_rel)共享内存索引区已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrSgwSlaqueRelData->create(_lCount);
			m_pMgrSgwSlaqueRelData = (SgwSlaQueue *)(*m_pSHMMgrSgwSlaqueRelData);

			if (!m_pSHMMgrSgwSlaqueRelIndex->exist()) {
				m_pSHMMgrSgwSlaqueRelIndex->create(_lCount);
			}else {
				cout<<"(sgw_sla_que_rel)共享内存索引区已经存在"<<endl;
			}

		}
	} else {
		cout<<"(sgw_sla_que_rel)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrSgwSlaqueRelIndex->exist()) {
			m_pSHMMgrSgwSlaqueRelIndex->create(_lCount);

		} else {
			cout<<"(sgw_sla_que_rel)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadSgwSlaqueRelSHM()
{

	if (m_pSHMMgrSgwSlaqueRelData->exist()) {
		m_pSHMMgrSgwSlaqueRelData->remove();
	}

	if (m_pSHMMgrSgwSlaqueRelIndex->exist()) {
		m_pSHMMgrSgwSlaqueRelIndex->remove();
	}

	m_pSHMMgrSgwSlaqueRelLock->close();
}

//表:UserInfo 方法：select
//数据全量加载
int SHMDataMgr::loadSgwSlaqueRel()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	char sValue[32] = {0};
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select SERVICELEVEL,QUEUEPRIORITY,NETLEVEL, \
								  SERVICEPERCENT,NETPERCENT,MESSAGETOTALNUM   \
								  from SGW_SLA_QUE_REL order by SERVICELEVEL");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrSgwSlaqueRelData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息SGW_SLA_QUE_REL共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrSgwSlaqueRelData[i].m_iServiceLevel = GETVALUE_INT(slQry,0);
			m_pMgrSgwSlaqueRelData[i].m_iQueuePriority = GETVALUE_INT(slQry,1);
			m_pMgrSgwSlaqueRelData[i].m_iNetLevel = GETVALUE_INT(slQry,2);
			m_pMgrSgwSlaqueRelData[i].m_iServicePercent = GETVALUE_INT(slQry,3);
			m_pMgrSgwSlaqueRelData[i].m_iNetPercent = GETVALUE_INT(slQry,4);
			m_pMgrSgwSlaqueRelData[i].m_iMessageTotalNum = GETVALUE_INT(slQry,5);

			if (m_pSHMMgrSgwSlaqueRelIndex->get(m_pMgrSgwSlaqueRelData[i].m_iQueuePriority,&value)) {
				int tmp =m_pMgrSgwSlaqueRelData[value].m_iNext;

				if (tmp !=0 ) {
					m_pMgrSgwSlaqueRelData[value].m_iNext = i;
					m_pMgrSgwSlaqueRelData[i].m_iNext = tmp;
				} else {
					m_pMgrSgwSlaqueRelData[value].m_iNext = i;
					m_pMgrSgwSlaqueRelData[i].m_iNext=0;
				}
			} else {
				m_pSHMMgrSgwSlaqueRelIndex->add (m_pMgrSgwSlaqueRelData[i].m_iQueuePriority,i);
				m_pMgrSgwSlaqueRelData[i].m_iNext=0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrSgwSlaqueRelData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}


//查询共享内存数据区信息
int SHMDataMgr::querySgwSlaqueRelData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSgwSlaqueRelData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSgwSlaqueRelData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrSgwSlaqueRelData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_sla_que_rel(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSgwSlaqueRelData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrSgwSlaqueRelData->getTotal()+1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSgwSlaqueRelData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrSgwSlaqueRelData->getCount());

	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querySgwSlaqueRelIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSgwSlaqueRelIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSgwSlaqueRelIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSgwSlaqueRelIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_sla_que_rel(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSgwSlaqueRelIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSgwSlaqueRelData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrSgwSlaqueRelIndex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createNetSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_NETINFO_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_NETINFO_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_NETINFO_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrNetInfoData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建网元信息共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrNetInfoData->create(_lMaxCount);
			m_pMgrNetInfoData = (NetInfoData *)(*m_pSHMMgrNetInfoData);
			if (!m_pSHMMgrNetInfoIndex->exist()) {
				m_pSHMMgrNetInfoIndex->create(_lMaxCount);
			} else {
				cout<<"(netInfodata)共享内存索引区A已经存在"<<endl;
			}

			if (!m_pSHMMgrNetInfoIndex_S->exist()) {
				m_pSHMMgrNetInfoIndex_S->create(_lMaxCount);
			}else {
				cout<<"(netInfodata)共享内存索引区B已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrNetInfoData->create(_lCount);
			m_pMgrNetInfoData = (NetInfoData *)(*m_pSHMMgrNetInfoData);

			if (!m_pSHMMgrNetInfoIndex->exist()) {
				m_pSHMMgrNetInfoIndex->create(_lCount);
			} else {
				cout<<"(netInfodata)共享内存索引区A已经存在"<<endl;
			}

			if (!m_pSHMMgrNetInfoIndex_S->exist()) {
				m_pSHMMgrNetInfoIndex_S->create(_lCount);
			}else {
				cout<<"(netInfodata)共享内存索引区B已经存在"<<endl;
			}

		}
	} else {
		cout<<"(netInfodata)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrNetInfoIndex->exist()) {
			m_pSHMMgrNetInfoIndex->create(_lCount);
		} else {
			cout<<"(netInfodata)共享内存索引区A已经存在"<<endl;
		}
		if (!m_pSHMMgrNetInfoIndex_S->exist()) {
			m_pSHMMgrNetInfoIndex_S->create(_lCount);
		}else {
			cout<<"(netInfodata)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadNetSHM()
{
	if (m_pSHMMgrNetInfoData->exist()) {
		m_pSHMMgrNetInfoData->remove();
	}

	if (m_pSHMMgrNetInfoIndex->exist()) {
		m_pSHMMgrNetInfoIndex->remove();
	}

	if (m_pSHMMgrNetInfoIndex_S->exist()) {
		m_pSHMMgrNetInfoIndex_S->remove();
	}
	m_pSHMMgrNetInfoLock->close();
}

//表:UserInfo 方法：select
//数据全量加载
int SHMDataMgr::loadNet()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lKey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select netinfo_seq,nvl(netinfo_name,-1),nvl(register_time,-1), \
								  nvl(update_time,-1),nvl(netinfo_code,-1),nvl(priority_level,-1), \
								  nvl(depoly_addr,-1),nvl(Eventdor_Name,-1),nvl(Ne_Version,-1),nvl(node_id,-1) \
								  from NetInfo order by netinfo_seq");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrNetInfoData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息NetInfo共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrNetInfoData[i].m_iNetinfoSeq = GETVALUE_INT(slQry,0);
			strncpy(m_pMgrNetInfoData[i].m_strNetinfoName,GETVALUE_STRING(slQry,1),sizeof(m_pMgrNetInfoData[i].m_strNetinfoName)-1);
			strncpy(m_pMgrNetInfoData[i].m_strRegisterTime,GETVALUE_STRING(slQry,2),sizeof(m_pMgrNetInfoData[i].m_strRegisterTime)-1);
			strncpy(m_pMgrNetInfoData[i].m_strUpdateTime,GETVALUE_STRING(slQry,3),sizeof(m_pMgrNetInfoData[i].m_strUpdateTime)-1);
			strncpy(m_pMgrNetInfoData[i].m_strNetinfoCode,GETVALUE_STRING(slQry,4),sizeof(m_pMgrNetInfoData[i].m_strNetinfoCode)-1);
			m_pMgrNetInfoData[i].m_iPriorityLevel = GETVALUE_INT(slQry,5);
			strncpy(m_pMgrNetInfoData[i].m_strDepolyAddr,GETVALUE_STRING(slQry,6),sizeof(m_pMgrNetInfoData[i].m_strDepolyAddr)-1);
			strncpy(m_pMgrNetInfoData[i].m_strEventdorName,GETVALUE_STRING(slQry,7),sizeof(m_pMgrNetInfoData[i].m_strEventdorName)-1);
			strncpy(m_pMgrNetInfoData[i].m_strNeVersion,GETVALUE_STRING(slQry,8),sizeof(m_pMgrNetInfoData[i].m_strNeVersion)-1);
			m_pMgrNetInfoData[i].m_iNodeId = GETVALUE_INT(slQry,9);
			m_pSHMMgrNetInfoIndex->add (m_pMgrNetInfoData[i].m_iNetinfoSeq,i);
			lKey = changekey(m_pMgrNetInfoData[i].m_strNetinfoName,m_pSHMMgrNetInfoData->getTotal());

			if (m_pSHMMgrNetInfoIndex_S->get(lKey,&value)) {
				int tmp =m_pMgrNetInfoData[value].m_iNext;

				if (tmp !=0 ) {
					m_pMgrNetInfoData[value].m_iNext = i;
					m_pMgrNetInfoData[i].m_iNext = tmp;
				} else {
					m_pMgrNetInfoData[value].m_iNext = i;
					m_pMgrNetInfoData[i].m_iNext = 0;
				}
			} else {
				m_pSHMMgrNetInfoIndex_S->add (lKey,i);
				m_pMgrNetInfoData[i].m_iNext = 0;
			}

		}
	}
	slQry.close();
	if (m_pSHMMgrNetInfoData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}


//查询共享内存数据区信息
int SHMDataMgr::queryNetData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrNetInfoData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrNetInfoData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrNetInfoData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"netInfodata(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrNetInfoData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrNetInfoData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrNetInfoData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrNetInfoData->getCount());

	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryNetIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrNetInfoIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrNetInfoIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrNetInfoIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"netInfodata(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrNetInfoIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrNetInfoData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrNetInfoIndex->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryNetIndex_s(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrNetInfoIndex_S->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrNetInfoIndex_S->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrNetInfoIndex_S->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"netInfodata(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrNetInfoIndex_S->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrNetInfoData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrNetInfoIndex_S->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createSessionSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SESSION_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SESSION_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SESSION_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrSessionData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建会话信息共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrSessionData->create(_lMaxCount);
			m_pMgrSessionData = (SessionData *)(*m_pSHMMgrSessionData);
			if (!m_pSHMMgrSessionIndex->exist()) {
				m_pSHMMgrSessionIndex->create(_lMaxCount);
			} else {
				cout<<"(sessiondata)共享内存索引区已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrSessionData->create(_lCount);
			m_pMgrSessionData = (SessionData *)(*m_pSHMMgrSessionData);

			if (!m_pSHMMgrSessionIndex->exist()) {
				m_pSHMMgrSessionIndex->create(_lCount);
			} else {
				cout<<"(sessiondata)共享内存索引区已经存在"<<endl;
			}

		}
	} else {
		cout<<"(sessiondata)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrSessionIndex->exist()) {
			m_pSHMMgrSessionIndex->create(_lCount);
		}  else {
			cout<<"(sessiondata)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadSessionSHM()
{
	if (m_pSHMMgrSessionData->exist()) {
		m_pSHMMgrSessionData->remove();
	}

	if (m_pSHMMgrSessionIndex->exist()) {
		m_pSHMMgrSessionIndex->remove();
	}
	m_pSHMMgrSessionLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::querySessionData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSessionData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSessionData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrSessionData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sessiondata(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSessionData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrSessionData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSessionData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrSessionData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querySessionIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSessionIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSessionIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSessionIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sessiondata(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSessionIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSessionData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrSessionIndex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createSessionClearSHM(int _iExtenSzie)
{
	if (!m_pSHMMgrSessionClearData->exist()) {

		m_pSHMMgrSessionClearData->create(_SHM_SESSIONCLEAR_SPACE_INITSIZE_);
		m_pMgrSessionClearData = (SessionClearData *)(*m_pSHMMgrSessionClearData);
		m_pMgrSessionHeadSign = (SessionHead *)(char *)m_pMgrSessionClearData;
		m_pMgrSessionHeadSign[0].m_iFree = 1;//空闲头
		int iTotal = m_pSHMMgrSessionClearData->getTotal();
		for(int i=1; i<iTotal; i++){
			m_pMgrSessionClearData[i].m_iNext = i+1;
			m_pMgrSessionClearData[i].m_iIndexNext = 0;
		}

		//最后一个特殊处理
		m_pMgrSessionClearData[iTotal].m_iNext = 1;
		m_pMgrSessionClearData[iTotal].m_iIndexNext = 0;
		m_pMgrSessionHeadSign[0].m_iPreDeal =1;

		if (!m_pSHMMgrSessionClearIndex->exist()) {
			m_pSHMMgrSessionClearIndex->create(_SHM_SESSIONCLEAR_SPACE_INITSIZE_);
		} else {
			cout<<"(sessioncleardata)共享内存索引区已经存在"<<endl;
		}
		return _SHM_SESSIONCLEAR_SPACE_INITSIZE_;
	} else {
		cout<<"(sessioncleardata)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrSessionClearIndex->exist()) {
			m_pSHMMgrSessionClearIndex->create(_SHM_SESSIONCLEAR_SPACE_INITSIZE_);
		}  else {
			cout<<"(sessioncleardata)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return 1;
}

void SHMDataMgr::unloadSessionClearSHM()
{
	if (m_pSHMMgrSessionClearData->exist()) {
		m_pSHMMgrSessionClearData->remove();
	}

	if (m_pSHMMgrSessionClearIndex->exist()) {
		m_pSHMMgrSessionClearIndex->remove();
	}
	m_pSHMMgrSessionClearLock->close();

}

//创建共享内存数据区和索引区
int SHMDataMgr::createCongestLeveSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_CONGESTLEVEL_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_CONGESTLEVEL_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_CONGESTLEVEL_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrCongestLevelData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建ShmCongestLevel共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrCongestLevelData->create(_lMaxCount);
			m_pMgrCongestLevel = (CongestLevel *)(*m_pSHMMgrCongestLevelData);

			return _lMaxCount;
		} else {
			m_pSHMMgrCongestLevelData->create(_lCount);
			m_pMgrCongestLevel = (CongestLevel *)(*m_pSHMMgrCongestLevelData);


		}
	} else {
		cout<<"(shmcongestlevel)共享内存数据区已经存在"<<endl;

		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadCongestLeveSHM()
{
	if (m_pSHMMgrCongestLevelData->exist()) {
		m_pSHMMgrCongestLevelData->remove();
	}
	m_pSHMMgrCongestLevelLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryCongestLeveData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrCongestLevelData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrCongestLevelData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrCongestLevelData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"shmcongestlevel(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrCongestLevelData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrCongestLevelData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrCongestLevelData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrCongestLevelData->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createServiceTxtListBaseSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SERVICETXTLISTBASE_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SERVICETXTLISTBASE_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SERVICETXTLISTBASE_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrServiceTxtListBaseData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建能力基础列表共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrServiceTxtListBaseData->create(_lMaxCount);
			m_pMgrServiceTxtListBaseData = (ServiceContextListBaseData *)(*m_pSHMMgrServiceTxtListBaseData);
			if (!m_pSHMMgrServiceTxtListBaseIndex->exist()) {
				m_pSHMMgrServiceTxtListBaseIndex->create(_lMaxCount);
			}else {
				cout<<"(service_context_list_base)共享内存索引区已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrServiceTxtListBaseData->create(_lCount);
			m_pMgrServiceTxtListBaseData = (ServiceContextListBaseData *)(*m_pSHMMgrServiceTxtListBaseData);
			if (!m_pSHMMgrServiceTxtListBaseIndex->exist()) {
				m_pSHMMgrServiceTxtListBaseIndex->create(_lCount);
			}else {
				cout<<"(service_context_list_base)共享内存索引区已经存在"<<endl;
			}

		}
	} else {
		cout<<"(service_context_list_base)共享内存数据区已经存在"<<endl;
		if (!m_pSHMMgrServiceTxtListBaseIndex->exist()) {
			m_pSHMMgrServiceTxtListBaseIndex->create(_lCount);
		} else {
			cout<<"(service_context_list_base)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//数据全量加载
int SHMDataMgr::loadServiceTxtListBase()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lKey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select service_context_id,nvl(service_context_id_name,-1), \
								  nvl(service_context_id_code,-1), \
								  create_date,nvl(service_package_type,-1),nvl(logic_mode,-1), \
								  nvl(service_context_id_desc,-1),nvl(priority_level,-1),  \
								  nvl(major_avp_value,-1) \
								  from service_context_list_base");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrServiceTxtListBaseData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息service_context_list_base共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			strncpy(m_pMgrServiceTxtListBaseData[i].m_strServiceContextId,GETVALUE_STRING(slQry,0),sizeof(m_pMgrServiceTxtListBaseData[i].m_strServiceContextId)-1);
			strncpy(m_pMgrServiceTxtListBaseData[i].m_strServiceContextIdName,GETVALUE_STRING(slQry,1),sizeof(m_pMgrServiceTxtListBaseData[i].m_strServiceContextIdName)-1);
			strncpy(m_pMgrServiceTxtListBaseData[i].m_strServiceContextIdCode,GETVALUE_STRING(slQry,2),sizeof(m_pMgrServiceTxtListBaseData[i].m_strServiceContextIdCode)-1);
			strncpy(m_pMgrServiceTxtListBaseData[i].m_strCreateDate,GETVALUE_STRING(slQry,3),sizeof(m_pMgrServiceTxtListBaseData[i].m_strCreateDate)-1);
			m_pMgrServiceTxtListBaseData[i].m_iServicePackageType = GETVALUE_INT(slQry,4);
			m_pMgrServiceTxtListBaseData[i].m_iLogicMode = GETVALUE_INT(slQry,5);
			strncpy(m_pMgrServiceTxtListBaseData[i].m_strServiceContextIdDesc,GETVALUE_STRING(slQry,6),sizeof(m_pMgrServiceTxtListBaseData[i].m_strServiceContextIdDesc)-1);
			m_pMgrServiceTxtListBaseData[i].m_iPriorityLevel = GETVALUE_INT(slQry,7);
			strncpy(m_pMgrServiceTxtListBaseData[i].m_strMajorAvp,GETVALUE_STRING(slQry,8),sizeof(m_pMgrServiceTxtListBaseData[i].m_strMajorAvp)-1);

			lKey = changekey(m_pMgrServiceTxtListBaseData[i].m_strServiceContextId,m_pSHMMgrServiceTxtListBaseData->getTotal());

			if (m_pSHMMgrServiceTxtListBaseIndex->get(lKey,&value)) {
				int tmp =m_pMgrServiceTxtListBaseData[value].m_iNext;

				if (tmp !=0 ) {
					m_pMgrServiceTxtListBaseData[value].m_iNext = i;
					m_pMgrServiceTxtListBaseData[i].m_iNext = tmp;
				} else {
					m_pMgrServiceTxtListBaseData[value].m_iNext = i;
					m_pMgrServiceTxtListBaseData[i].m_iNext = 0;
				}
			} else {
				m_pSHMMgrServiceTxtListBaseIndex->add (lKey,i);
				m_pMgrServiceTxtListBaseData[i].m_iNext = 0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrServiceTxtListBaseData->getCount() == 0 ) {
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadServiceTxtListBaseSHM()
{
	if (m_pSHMMgrServiceTxtListBaseData->exist()) {
		m_pSHMMgrServiceTxtListBaseData->remove();
	}

	if (m_pSHMMgrServiceTxtListBaseIndex->exist()) {
		m_pSHMMgrServiceTxtListBaseIndex->remove();
	}
	m_pSHMMgrServiceTxtListBaseLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryServiceTxtListBaseData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServiceTxtListBaseData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServiceTxtListBaseData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrServiceTxtListBaseData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_context_list_base(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServiceTxtListBaseData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrServiceTxtListBaseData->getTotal()+1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServiceTxtListBaseData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrServiceTxtListBaseData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryServiceTxtListBaseIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServiceTxtListBaseIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServiceTxtListBaseIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrServiceTxtListBaseIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_context_list_base(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServiceTxtListBaseIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServiceTxtListBaseData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrServiceTxtListBaseIndex->getCount());

	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createloadbalanceSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_LOADBALANCE_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_LOADBALANCE_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_LOADBALANCE_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrLoadBalanceData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建loadbalancedata共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrLoadBalanceData->create(_lMaxCount);
			m_pMgrLoadBalanceData = (LoadBalanceData *)(*m_pSHMMgrLoadBalanceData);
			if (!m_pSHMMgrLoadBalanceIndex->exist()) {
				m_pSHMMgrLoadBalanceIndex->create(_lMaxCount);
			} else {
				cout<<"(LoadBalanceData)共享内存索引区已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrLoadBalanceData->create(_lCount);
			m_pMgrLoadBalanceData = (LoadBalanceData *)(*m_pSHMMgrLoadBalanceData);
			if (!m_pSHMMgrLoadBalanceIndex->exist()) {
				m_pSHMMgrLoadBalanceIndex->create(_lCount);
			} else {
				cout<<"(LoadBalanceData)共享内存索引区已经存在"<<endl;
			}

		}
	} else {
		cout<<"(LoadBalanceData)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrLoadBalanceIndex->exist()) {
			m_pSHMMgrLoadBalanceIndex->create(_lCount);
		} else {
			cout<<"(LoadBalanceData)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadloadbalanceSHM()
{
	if (m_pSHMMgrLoadBalanceData->exist()) {
		m_pSHMMgrLoadBalanceData->remove();
	}

	if (m_pSHMMgrLoadBalanceIndex->exist()) {
		m_pSHMMgrLoadBalanceIndex->remove();
	}
	m_pSHMMgrLoadBalanceLock->close();
}

int SHMDataMgr::loadBalance()
{
	unsigned int value = 0;

	int i = m_pSHMMgrLoadBalanceData->malloc();
	if (i == 0 ){
		ThreeLogGroup _oLogObj;
		_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息loadbalancedata共享内存空间不足");

		return MBC_MEM_SPACE_SMALL;
	}
	if (strlen(SHMParamCfg::getString("dcc","sr1_host",m_pMgrLoadBalanceData[i].SR_Host))==0) {
		return MBC_CFG_GETPARAM_FAILED;
	}
	m_pMgrLoadBalanceData[i].Load_Balance_Rate=50;
	long lkey = SHMDataMgr::changekey(m_pMgrLoadBalanceData[i].SR_Host,m_pSHMMgrLoadBalanceData->getTotal());

	if (m_pSHMMgrLoadBalanceIndex->get(lkey,&value)) {
		int tmp = m_pMgrLoadBalanceData[value].m_iNext;
		m_pMgrLoadBalanceData[value].m_iNext = i;
		m_pMgrLoadBalanceData[i].m_iNext = tmp;
	} else {
		m_pSHMMgrLoadBalanceIndex->add(lkey,i);
		m_pMgrLoadBalanceData[i].m_iNext = 0;
	}

	i = m_pSHMMgrLoadBalanceData->malloc();
	if (i == 0 ){
		ThreeLogGroup _oLogObj;
		_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息loadbalancedata共享内存空间不足");

		return MBC_MEM_SPACE_SMALL;
	}
	if (strlen(SHMParamCfg::getString("dcc","sr2_host",m_pMgrLoadBalanceData[i].SR_Host))==0) {
		return MBC_CFG_GETPARAM_FAILED;
	}
	m_pMgrLoadBalanceData[i].Load_Balance_Rate=50;
	lkey = SHMDataMgr::changekey(m_pMgrLoadBalanceData[i].SR_Host,m_pSHMMgrLoadBalanceData->getTotal());

	if (m_pSHMMgrLoadBalanceIndex->get(lkey,&value)) {
		int tmp = m_pMgrLoadBalanceData[value].m_iNext;
		m_pMgrLoadBalanceData[value].m_iNext = i;
		m_pMgrLoadBalanceData[i].m_iNext = tmp;
	} else {
		m_pSHMMgrLoadBalanceIndex->add(lkey,i);
		m_pMgrLoadBalanceData[i].m_iNext = 0;
	}
	return 0;
}

//查询共享内存数据区信息
int SHMDataMgr::queryloadbalanceData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrLoadBalanceData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrLoadBalanceData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrLoadBalanceData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"loadbalancedata(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrLoadBalanceData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrLoadBalanceData->getTotal()+1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrLoadBalanceData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrLoadBalanceData->getCount());

	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryloadbalanceIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrLoadBalanceIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrLoadBalanceIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrLoadBalanceIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"loadbalancedata(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrLoadBalanceIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrLoadBalanceData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrLoadBalanceIndex->getCount());

	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createbasemethodSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_BASEMETHOD_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_BASEMETHOD_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_BASEMETHOD_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小


	if (!m_pSHMMgrBaseMethodData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建basemethod共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrBaseMethodData->create(_lMaxCount);
			m_pMgrBaseMethodData = (CBaseMethod *)(*m_pSHMMgrBaseMethodData);
			if (!m_pSHMMgrBaseMethodindex->exist()) {
				m_pSHMMgrBaseMethodindex->create(_lMaxCount);
			} else {
				cout<<"(base_method_list)共享内存索引区已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrBaseMethodData->create(_lCount);
			m_pMgrBaseMethodData = (CBaseMethod *)(*m_pSHMMgrBaseMethodData);

			if (!m_pSHMMgrBaseMethodindex->exist()) {
				m_pSHMMgrBaseMethodindex->create(_lCount);
			} else {
				cout<<"(base_method_list)共享内存索引区已经存在"<<endl;
			}

		}
	} else {
		cout<<"(base_method_list)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrBaseMethodindex->exist()) {
			m_pSHMMgrBaseMethodindex->create(_lCount);
		} else {
			cout<<"(base_method_list)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadbasemethodSHM()
{
	if (m_pSHMMgrBaseMethodData->exist()) {
		m_pSHMMgrBaseMethodData->remove();
	}

	if (m_pSHMMgrBaseMethodindex->exist()) {
		m_pSHMMgrBaseMethodindex->remove();
	}
	m_pSHMMgrBaseMethodLock->close();
}

//数据全量加载
int SHMDataMgr::loadbasemethod()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select base_method_id,base_method_name, base_method_desc from base_method ");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrBaseMethodData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息base_method共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrBaseMethodData[i].m_iBaseMethodID = GETVALUE_INT(slQry,0);
			strncpy(m_pMgrBaseMethodData[i].m_strBaseMethodName,GETVALUE_STRING(slQry,1),sizeof(m_pMgrBaseMethodData[i].m_strBaseMethodName)-1);
			strncpy(m_pMgrBaseMethodData[i].m_strBaseMethodDesc,GETVALUE_STRING(slQry,2),sizeof(m_pMgrBaseMethodData[i].m_strBaseMethodDesc)-1);
			m_pSHMMgrBaseMethodindex->add (m_pMgrBaseMethodData[i].m_iBaseMethodID,i);
		}
	}
	slQry.close();
	if (m_pSHMMgrBaseMethodData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//查询共享内存数据区信息
int SHMDataMgr::querybasemethodData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrBaseMethodData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrBaseMethodData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrBaseMethodData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"base_method_list(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrBaseMethodData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrBaseMethodData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrBaseMethodData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrBaseMethodData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querybasemethodIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrBaseMethodindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrBaseMethodindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrBaseMethodindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"base_method_list(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrBaseMethodindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrBaseMethodData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrBaseMethodindex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createpacketsessionSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_PACKETSESSION_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_PACKETSESSION_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_PACKETSESSION_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrPacketSessionData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建packetsession共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrPacketSessionData->create(_lMaxCount);
			m_pMgrPacketSessionData = (SPacketSession *)(*m_pSHMMgrPacketSessionData);
			if (!m_pSHMMgrPacketSessionindex->exist()) {
				m_pSHMMgrPacketSessionindex->create(_lMaxCount);
			}else {
				cout<<"(CPacketSession)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrPacketSubSessionindex->exist()) {
				m_pSHMMgrPacketSubSessionindex->create(_lMaxCount);
			}else {
				cout<<"(CPacketSession)共享内存索引区B已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrPacketSessionData->create(_lCount);
			m_pMgrPacketSessionData = (SPacketSession *)(*m_pSHMMgrPacketSessionData);

			if (!m_pSHMMgrPacketSessionindex->exist()) {
				m_pSHMMgrPacketSessionindex->create(_lCount);
			}else {
				cout<<"(CPacketSession)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrPacketSubSessionindex->exist()) {
				m_pSHMMgrPacketSubSessionindex->create(_lCount);
			}else {
				cout<<"(CPacketSession)共享内存索引区B已经存在"<<endl;
			}

		}
	} else {
		cout<<"(CPacketSession)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrPacketSessionindex->exist()) {
			m_pSHMMgrPacketSessionindex->create(_lCount);
		}else {
			cout<<"(CPacketSession)共享内存索引区A已经存在"<<endl;
		}
		if (!m_pSHMMgrPacketSubSessionindex->exist()) {
			m_pSHMMgrPacketSubSessionindex->create(_lCount);
		}else {
			cout<<"(CPacketSession)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadpacketsessionSHM()
{
	if (m_pSHMMgrPacketSessionData->exist()) {
		m_pSHMMgrPacketSessionData->remove();
	}

	if (m_pSHMMgrPacketSessionindex->exist()) {
		m_pSHMMgrPacketSessionindex->remove();
	}

	if (m_pSHMMgrPacketSubSessionindex->exist()) {
		m_pSHMMgrPacketSubSessionindex->remove();
	}
	m_pSHMMgrPacketSessionLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::querypacketsessionData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrPacketSessionData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrPacketSessionData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrPacketSessionData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"s_packet_session(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrPacketSessionData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrPacketSessionData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrPacketSessionData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrPacketSessionData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querypacketsessionIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrPacketSessionindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrPacketSessionindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrPacketSessionindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"s_packet_session(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrPacketSessionindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrPacketSessionData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrPacketSessionindex->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querypacketsubsessionIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrPacketSubSessionindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrPacketSubSessionindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrPacketSubSessionindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"s_packet_session(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrPacketSubSessionindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrPacketSessionData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrPacketSubSessionindex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createservicepackageSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SERVICEPACKAGE_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SERVICEPACKAGE_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SERVICEPACKAGE_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrServicePackageData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建ServicePackageNode共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrServicePackageData->create(_lMaxCount);
			m_pMgrServicePackageData = (ServicePackageNode *)(*m_pSHMMgrServicePackageData);
			if (!m_pSHMMgrServicePackageindex->exist()) {
				m_pSHMMgrServicePackageindex->create(_lMaxCount);
			}else {
				cout<<"(service_package_node)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvPackageNodeSeqIndex->exist()) {
				m_pSHMMgrSrvPackageNodeSeqIndex->create(_lMaxCount);
			}else {
				cout<<"(service_package_node)共享内存索引区B已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrServicePackageData->create(_lCount);
			m_pMgrServicePackageData = (ServicePackageNode *)(*m_pSHMMgrServicePackageData);

			if (!m_pSHMMgrServicePackageindex->exist()) {
				m_pSHMMgrServicePackageindex->create(_lCount);
			}else {
				cout<<"(service_package_node)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvPackageNodeSeqIndex->exist()) {
				m_pSHMMgrSrvPackageNodeSeqIndex->create(_lCount);
			}else {
				cout<<"(service_package_node)共享内存索引区B已经存在"<<endl;
			}

		}
	} else {
		cout<<"(service_package_node)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrServicePackageindex->exist()) {
			m_pSHMMgrServicePackageindex->create(_lCount);
		}else {
			cout<<"(service_package_node)共享内存索引区A已经存在"<<endl;
		}
		if (!m_pSHMMgrSrvPackageNodeSeqIndex->exist()) {
			m_pSHMMgrSrvPackageNodeSeqIndex->create(_lCount);
		}else {
			cout<<"(service_package_node)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadservicepackageSHM()
{
	if (m_pSHMMgrServicePackageData->exist()) {
		m_pSHMMgrServicePackageData->remove();
	}

	if (m_pSHMMgrServicePackageindex->exist()) {
		m_pSHMMgrServicePackageindex->remove();
	}

	if (m_pSHMMgrSrvPackageNodeSeqIndex->exist()) {
		m_pSHMMgrSrvPackageNodeSeqIndex->remove();
	}
	m_pSHMMgrServicePackageLock->close();
}


//数据全量加载
int SHMDataMgr::loadservicepackage()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select node_seq,service_context_id,nvl(node_source_id,-1), \
								  node_type,nvl(node_name,-1),nvl(flow_id,-1) from service_package_node ");
	slQry.setSQL(sSQL);
	slQry.open();
	unsigned int value = 0;

	while (slQry.next()) {
		int i = m_pSHMMgrServicePackageData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息service_package_node共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrServicePackageData[i].m_iNodeSeq = GETVALUE_INT(slQry,0);
			strncpy(m_pMgrServicePackageData[i].m_strServiceContextId,GETVALUE_STRING(slQry,1),sizeof(m_pMgrServicePackageData[i].m_strServiceContextId)-1);
			strncpy(m_pMgrServicePackageData[i].m_strNodeSourceID,GETVALUE_STRING(slQry,2),sizeof(m_pMgrServicePackageData[i].m_strNodeSourceID)-1);
			m_pMgrServicePackageData[i].m_iNodeType = GETVALUE_INT(slQry,3);
			strncpy(m_pMgrServicePackageData[i].m_strNodeName,GETVALUE_STRING(slQry,4),sizeof(m_pMgrServicePackageData[i].m_strNodeName)-1);
			m_pMgrServicePackageData[i].m_iFlowId = GETVALUE_INT(slQry,5);
			m_pSHMMgrSrvPackageNodeSeqIndex->add (m_pMgrServicePackageData[i].m_iNodeSeq,i);
			lkey  = changekey(m_pMgrServicePackageData[i].m_strServiceContextId,m_pSHMMgrServicePackageData->getTotal());

			if (m_pSHMMgrServicePackageindex->get(lkey,&value)) {
				int tmp =m_pMgrServicePackageData[value].m_iNext;
				m_pMgrServicePackageData[value].m_iNext = i;
				m_pMgrServicePackageData[i].m_iNext = tmp;
			} else {
				m_pSHMMgrServicePackageindex->add (lkey,i);
				m_pMgrServicePackageData[i].m_iNext=0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrServicePackageData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//查询共享内存数据区信息
int SHMDataMgr::queryservicepackageData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServicePackageData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServicePackageData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrServicePackageData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_node(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServicePackageData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrServicePackageData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrServicePackageData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryservicepackageIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServicePackageindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServicePackageindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrServicePackageindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_node(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServicePackageindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrServicePackageindex->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryservicepackageIndex_a(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvPackageNodeSeqIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvPackageNodeSeqIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvPackageNodeSeqIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_node(tables)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvPackageNodeSeqIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSrvPackageNodeSeqIndex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createpackagerouteSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_PACKAGEROUTE_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_PACKAGEROUTE_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_PACKAGEROUTE_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrServicePackageRouteData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建service_package_route共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrServicePackageRouteData->create(_lMaxCount);
			m_pMgrServicePackageRouteData = (ServicePackageRoute *)(*m_pSHMMgrServicePackageRouteData);
			if (!m_pSHMMgrServicePackageRouteindex->exist()) {
				m_pSHMMgrServicePackageRouteindex->create(_lMaxCount);
			}else {
				cout<<"(service_package_route)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvPackageRouteSeqIndex->exist()) {
				m_pSHMMgrSrvPackageRouteSeqIndex->create(_lMaxCount);
			}else {
				cout<<"(service_package_route)共享内存索引区B已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrServicePackageRouteData->create(_lCount);
			m_pMgrServicePackageRouteData = (ServicePackageRoute *)(*m_pSHMMgrServicePackageRouteData);

			if (!m_pSHMMgrServicePackageRouteindex->exist()) {
				m_pSHMMgrServicePackageRouteindex->create(_lCount);
			}else {
				cout<<"(service_package_route)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvPackageRouteSeqIndex->exist()) {
				m_pSHMMgrSrvPackageRouteSeqIndex->create(_lCount);
			}else {
				cout<<"(service_package_route)共享内存索引区B已经存在"<<endl;
			}


		}
	} else {
		cout<<"(service_package_route)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrServicePackageRouteindex->exist()) {
			m_pSHMMgrServicePackageRouteindex->create(_lCount);
		}else {
			cout<<"(service_package_route)共享内存索引区A已经存在"<<endl;
		}
		if (!m_pSHMMgrSrvPackageRouteSeqIndex->exist()) {
			m_pSHMMgrSrvPackageRouteSeqIndex->create(_lCount);
		}else {
			cout<<"(service_package_route)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadpackagerouteSHM()
{
	if (m_pSHMMgrServicePackageRouteData->exist()) {
		m_pSHMMgrServicePackageRouteData->remove();
	}

	if (m_pSHMMgrServicePackageRouteindex->exist()) {
		m_pSHMMgrServicePackageRouteindex->remove();
	}

	if (m_pSHMMgrSrvPackageRouteSeqIndex->exist()) {
		m_pSHMMgrSrvPackageRouteSeqIndex->remove();
	}
	m_pSHMMgrServicePackageRouteLock->close();
}

//数据全量加载
int SHMDataMgr::loadpackageroute()
{

	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select route_seq,source_node_seq,target_node_seq,nvl(route_condition,-1), \
								  service_package_id,nvl(flow_id,-1) from service_package_route ");
	slQry.setSQL(sSQL);
	slQry.open();
	unsigned int value = 0;

	while (slQry.next()) {
		int i = m_pSHMMgrServicePackageRouteData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息service_package_route共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrServicePackageRouteData[i].m_iRouteSeq = GETVALUE_INT(slQry,0);
			m_pMgrServicePackageRouteData[i].m_iSourceNodeSeq = GETVALUE_INT(slQry,1);
			m_pMgrServicePackageRouteData[i].m_iTargetNodeSeq = GETVALUE_INT(slQry,2);
			m_pMgrServicePackageRouteData[i].m_iRouteCondition = GETVALUE_INT(slQry,3);
			strncpy(m_pMgrServicePackageRouteData[i].m_strServicePackageId,GETVALUE_STRING(slQry,4),sizeof(m_pMgrServicePackageRouteData[i].m_strServicePackageId)-1);
			m_pMgrServicePackageRouteData[i].m_iFlowId = GETVALUE_INT(slQry,5);

			m_pSHMMgrSrvPackageRouteSeqIndex->add (m_pMgrServicePackageRouteData[i].m_iRouteSeq,i);

			lkey = changekey(m_pMgrServicePackageRouteData[i].m_strServicePackageId,m_pSHMMgrServicePackageRouteData->getTotal());

			if (m_pSHMMgrServicePackageRouteindex->get(lkey,&value)) {
				int tmp =m_pMgrServicePackageRouteData[value].m_iNext;
				m_pMgrServicePackageRouteData[value].m_iNext = i;
				m_pMgrServicePackageRouteData[i].m_iNext = tmp;
			} else {
				m_pSHMMgrServicePackageRouteindex->add (lkey,i);
				m_pMgrServicePackageRouteData[i].m_iNext=0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrServicePackageRouteData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//查询共享内存数据区信息
int SHMDataMgr::querypackagerouteData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServicePackageRouteData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServicePackageRouteData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrServicePackageRouteData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_route(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServicePackageRouteData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrServicePackageRouteData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageRouteData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrServicePackageRouteData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querypackagerouteIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServicePackageRouteindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServicePackageRouteindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrServicePackageRouteindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_route(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServicePackageRouteindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageRouteData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrServicePackageRouteindex->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querypackagerouteIndex_a(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvPackageRouteSeqIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvPackageRouteSeqIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvPackageRouteSeqIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_route(tables)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvPackageRouteSeqIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageRouteData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSrvPackageRouteSeqIndex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createservicepackageCCRSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_PACKAGEROUTECCR_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_PACKAGEROUTECCR_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_PACKAGEROUTECCR_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrServicePackageRouteCCRData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建能力封装路径CCR共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrServicePackageRouteCCRData->create(_lMaxCount);
			m_pMgrServicePackageRouteCCRData = (CServicePackageRouteCCR *)(*m_pSHMMgrServicePackageRouteCCRData);
			if (!m_pSHMMgrSrvPackageRouteCCRSeqindex->exist()) {
				m_pSHMMgrSrvPackageRouteCCRSeqindex->create(_lMaxCount);
			}else {
				cout<<"(service_package_route_ccr)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvPackageRouteSeqindex->exist()) {
				m_pSHMMgrSrvPackageRouteSeqindex->create(_lMaxCount);
			}else {
				cout<<"(service_package_route_ccr)共享内存索引区B已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrServicePackageRouteCCRData->create(_lCount);
			m_pMgrServicePackageRouteCCRData = (CServicePackageRouteCCR *)(*m_pSHMMgrServicePackageRouteCCRData);

			if (!m_pSHMMgrSrvPackageRouteCCRSeqindex->exist()) {
				m_pSHMMgrSrvPackageRouteCCRSeqindex->create(_lCount);
			}else {
				cout<<"(service_package_route_ccr)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvPackageRouteSeqindex->exist()) {
				m_pSHMMgrSrvPackageRouteSeqindex->create(_lCount);
			}else {
				cout<<"(service_package_route_ccr)共享内存索引区B已经存在"<<endl;
			}

		}
	} else {
		cout<<"(service_package_route_ccr)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrSrvPackageRouteCCRSeqindex->exist()) {
			m_pSHMMgrSrvPackageRouteCCRSeqindex->create(_lCount);
		}else {
			cout<<"(service_package_route_ccr)共享内存索引区A已经存在"<<endl;
		}
		if (!m_pSHMMgrSrvPackageRouteSeqindex->exist()) {
			m_pSHMMgrSrvPackageRouteSeqindex->create(_lCount);
		}else {
			cout<<"(service_package_route_ccr)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadservicepackageCCRSHM()
{
	if (m_pSHMMgrServicePackageRouteCCRData->exist()) {
		m_pSHMMgrServicePackageRouteCCRData->remove();
	}

	if (m_pSHMMgrSrvPackageRouteCCRSeqindex->exist()) {
		m_pSHMMgrSrvPackageRouteCCRSeqindex->remove();
	}

	if (m_pSHMMgrSrvPackageRouteSeqindex->exist()) {
		m_pSHMMgrSrvPackageRouteSeqindex->remove();
	}
	m_pSHMMgrServicePackageRouteCCRLock->close();
}

//数据全量加载
int SHMDataMgr::loadservicepackageCCR()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	char skey[8] = {0};
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select route_ccr_seq,route_seq,avp_name,avp_type,avp_option, \
								  call_func_id from service_package_route_ccr ");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrServicePackageRouteCCRData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息service_package_route_ccr共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrServicePackageRouteCCRData[i].m_iRouteCCRSeq = GETVALUE_INT(slQry,0);
			m_pMgrServicePackageRouteCCRData[i].m_iRouteSeq = GETVALUE_INT(slQry,1);
			strncpy(m_pMgrServicePackageRouteCCRData[i].m_strAvpName,GETVALUE_STRING(slQry,2),sizeof(m_pMgrServicePackageRouteCCRData[i].m_strAvpName)-1);
			m_pMgrServicePackageRouteCCRData[i].m_iAvpType = GETVALUE_INT(slQry,3);
			m_pMgrServicePackageRouteCCRData[i].m_iAvpOption = GETVALUE_INT(slQry,4);
			m_pMgrServicePackageRouteCCRData[i].m_iFuncID = GETVALUE_INT(slQry,5);

			m_pSHMMgrSrvPackageRouteCCRSeqindex->add (m_pMgrServicePackageRouteCCRData[i].m_iRouteCCRSeq,i);

			if (m_pSHMMgrSrvPackageRouteSeqindex->get(m_pMgrServicePackageRouteCCRData[i].m_iRouteSeq,&value)) {
				int pos = m_pMgrServicePackageRouteCCRData[value].m_iNext;
				m_pMgrServicePackageRouteCCRData[value].m_iNext = i;
				m_pMgrServicePackageRouteCCRData[i].m_iNext = pos;
			} else {
				m_pSHMMgrSrvPackageRouteSeqindex->add(m_pMgrServicePackageRouteCCRData[i].m_iRouteSeq,i);
				m_pMgrServicePackageRouteCCRData[i].m_iNext = 0;
			}

		}
	}
	slQry.close();
	if (m_pSHMMgrServicePackageRouteCCRData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//查询共享内存数据区信息
int SHMDataMgr::queryservicepackageCCRData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServicePackageRouteCCRData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServicePackageRouteCCRData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrServicePackageRouteCCRData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_route_ccr(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServicePackageRouteCCRData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrServicePackageRouteCCRData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageRouteCCRData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrServicePackageRouteCCRData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryservicepackageCCRIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvPackageRouteCCRSeqindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvPackageRouteCCRSeqindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvPackageRouteCCRSeqindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_route_ccr(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvPackageRouteCCRSeqindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageRouteCCRData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSrvPackageRouteCCRSeqindex->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryservicepackageCCRIndex_a(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvPackageRouteSeqindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvPackageRouteSeqindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvPackageRouteSeqindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_route_ccr(tables)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvPackageRouteSeqindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageRouteCCRData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSrvPackageRouteSeqindex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createpackagejudgeSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SERVICEPACKAGEJUDGE_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SERVICEPACKAGEJUDGE_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SERVICEPACKAGEJUDGE_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrServicePackageJudgeData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建CServicePackageJudge共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrServicePackageJudgeData->create(_lMaxCount);
			m_pMgrServicePackageJudgeData = (CServicePackageJudge *)(*m_pSHMMgrServicePackageJudgeData);
			if (!m_pSHMMgrServicePackageJudgeindex->exist()) {
				m_pSHMMgrServicePackageJudgeindex->create(_lMaxCount);
			}else {
				cout<<"(service_package_judge)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvPackageJudgeNodeSeqIndex->exist()) {
				m_pSHMMgrSrvPackageJudgeNodeSeqIndex->create(_lMaxCount);
			}else {
				cout<<"(service_package_judge)共享内存索引区B已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrServicePackageJudgeData->create(_lCount);
			m_pMgrServicePackageJudgeData = (CServicePackageJudge *)(*m_pSHMMgrServicePackageJudgeData);

			if (!m_pSHMMgrServicePackageJudgeindex->exist()) {
				m_pSHMMgrServicePackageJudgeindex->create(_lCount);
			}else {
				cout<<"(service_package_judge)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvPackageJudgeNodeSeqIndex->exist()) {
				m_pSHMMgrSrvPackageJudgeNodeSeqIndex->create(_lCount);
			}else {
				cout<<"(service_package_judge)共享内存索引区B已经存在"<<endl;
			}


		}
	} else {
		cout<<"(service_package_judge)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrServicePackageJudgeindex->exist()) {
			m_pSHMMgrServicePackageJudgeindex->create(_lCount);
		}else {
			cout<<"(service_package_judge)共享内存索引区A已经存在"<<endl;
		}
		if (!m_pSHMMgrSrvPackageJudgeNodeSeqIndex->exist()) {
			m_pSHMMgrSrvPackageJudgeNodeSeqIndex->create(_lCount);
		}else {
			cout<<"(service_package_judge)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadpackagejudgeSHM()
{
	if (m_pSHMMgrServicePackageJudgeData->exist()) {
		m_pSHMMgrServicePackageJudgeData->remove();
	}

	if (m_pSHMMgrServicePackageJudgeindex->exist()) {
		m_pSHMMgrServicePackageJudgeindex->remove();
	}

	if (m_pSHMMgrSrvPackageJudgeNodeSeqIndex->exist()) {
		m_pSHMMgrSrvPackageJudgeNodeSeqIndex->remove();
	}
	m_pSHMMgrServicePackageJudgeLock->close();
}

//数据全量加载
int SHMDataMgr::loadpackagejudge()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	char skey[8] = {0};
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select judge_condition_seq,node_seq,judge_condition_group,condition_name, \
								  operater_id,Compare_Left,Compare_Right from service_package_judge ");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrServicePackageJudgeData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息service_package_judge共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrServicePackageJudgeData[i].m_iJudgeConditionSeq = GETVALUE_INT(slQry,0);
			m_pMgrServicePackageJudgeData[i].m_iNodeSeq = GETVALUE_INT(slQry,1);
			m_pMgrServicePackageJudgeData[i].m_iJudgeConditionGroup = GETVALUE_INT(slQry,2);
			strncpy(m_pMgrServicePackageJudgeData[i].m_strConditionName,GETVALUE_STRING(slQry,3),sizeof(m_pMgrServicePackageJudgeData[i].m_strConditionName)-1);
			m_pMgrServicePackageJudgeData[i].m_iOperaterID = GETVALUE_INT(slQry,4);
			m_pMgrServicePackageJudgeData[i].m_iCompareLeft = GETVALUE_INT(slQry,5);
			m_pMgrServicePackageJudgeData[i].m_iCompareRight = GETVALUE_INT(slQry,6);
			m_pSHMMgrServicePackageJudgeindex->add (m_pMgrServicePackageJudgeData[i].m_iJudgeConditionSeq,i);

			if (m_pSHMMgrSrvPackageJudgeNodeSeqIndex->get(m_pMgrServicePackageJudgeData[i].m_iNodeSeq,&value)) {
				int pos = m_pMgrServicePackageJudgeData[value].m_iNext;
				m_pMgrServicePackageJudgeData[value].m_iNext = i;
				m_pMgrServicePackageJudgeData[i].m_iNext = pos;
			} else {
				m_pSHMMgrSrvPackageJudgeNodeSeqIndex->add(m_pMgrServicePackageJudgeData[i].m_iNodeSeq,i);
				m_pMgrServicePackageJudgeData[i].m_iNext = 0;
			}

		}
	}
	slQry.close();
	if (m_pSHMMgrServicePackageJudgeData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//查询共享内存数据区信息
int SHMDataMgr::querypackagejudgeData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServicePackageJudgeData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServicePackageJudgeData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrServicePackageJudgeData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_judge(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServicePackageJudgeData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrServicePackageJudgeData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageJudgeData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrServicePackageJudgeData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querypackagejudgeIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServicePackageJudgeindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServicePackageJudgeindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrServicePackageJudgeindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_judge(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServicePackageJudgeindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageJudgeData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrServicePackageJudgeindex->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querypackagejudgeIndex_a(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvPackageJudgeNodeSeqIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvPackageJudgeNodeSeqIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvPackageJudgeNodeSeqIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_judge(tables)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvPackageJudgeNodeSeqIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageJudgeData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSrvPackageJudgeNodeSeqIndex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createpackageccaSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SERVICEPACKAGECCA_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SERVICEPACKAGECCA_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SERVICEPACKAGECCA_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrServicePackageCCAData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建CServicePackageCCA共享内存请求分配的共享内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrServicePackageCCAData->create(_lMaxCount);
			m_pMgrServicePackageCCAData = (CServicePackageCCA *)(*m_pSHMMgrServicePackageCCAData);
			if (!m_pSHMMgrServicePackageCCAindex->exist()) {
				m_pSHMMgrServicePackageCCAindex->create(_lMaxCount);
			}else {
				cout<<"(service_package_cca)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvPackageCCASeqIndex->exist()) {
				m_pSHMMgrSrvPackageCCASeqIndex->create(_lMaxCount);
			}else {
				cout<<"(service_package_cca)共享内存索引区B已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrServicePackageCCAData->create(_lCount);
			m_pMgrServicePackageCCAData = (CServicePackageCCA *)(*m_pSHMMgrServicePackageCCAData);

			if (!m_pSHMMgrServicePackageCCAindex->exist()) {
				m_pSHMMgrServicePackageCCAindex->create(_lCount);
			}else {
				cout<<"(service_package_cca)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvPackageCCASeqIndex->exist()) {
				m_pSHMMgrSrvPackageCCASeqIndex->create(_lCount);
			}else {
				cout<<"(service_package_cca)共享内存索引区B已经存在"<<endl;
			}

		}
	} else {
		cout<<"(service_package_cca)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrServicePackageCCAindex->exist()) {
			m_pSHMMgrServicePackageCCAindex->create(_lCount);
		}else {
			cout<<"(service_package_cca)共享内存索引区A已经存在"<<endl;
		}
		if (!m_pSHMMgrSrvPackageCCASeqIndex->exist()) {
			m_pSHMMgrSrvPackageCCASeqIndex->create(_lCount);
		}else {
			cout<<"(service_package_cca)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadpackageccaSHM()
{
	if (m_pSHMMgrServicePackageCCAData->exist()) {
		m_pSHMMgrServicePackageCCAData->remove();
	}

	if (m_pSHMMgrServicePackageCCAindex->exist()) {
		m_pSHMMgrServicePackageCCAindex->remove();
	}

	if (m_pSHMMgrSrvPackageCCASeqIndex->exist()) {
		m_pSHMMgrSrvPackageCCASeqIndex->remove();
	}
	m_pSHMMgrServicePackageCCALock->close();
}

//数据全量加载
int SHMDataMgr::loadpackagecca()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select service_package_cca_seq,service_context_id,avp_name,avp_type, \
								  avp_option,call_func_id from service_package_cca ");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrServicePackageCCAData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息service_package_cca共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrServicePackageCCAData[i].m_iServicePackageCCASeq = GETVALUE_INT(slQry,0);
			strncpy(m_pMgrServicePackageCCAData[i].m_strServiceContextId,GETVALUE_STRING(slQry,1),sizeof(m_pMgrServicePackageCCAData[i].m_strServiceContextId)-1);
			strncpy(m_pMgrServicePackageCCAData[i].m_strAvpName,GETVALUE_STRING(slQry,2),sizeof(m_pMgrServicePackageCCAData[i].m_strAvpName)-1);
			m_pMgrServicePackageCCAData[i].m_iAvpType = GETVALUE_INT(slQry,3);
			m_pMgrServicePackageCCAData[i].m_iAvpOption = GETVALUE_INT(slQry,4);
			m_pMgrServicePackageCCAData[i].m_iFuncID = GETVALUE_INT(slQry,5);
			m_pSHMMgrSrvPackageCCASeqIndex->add (m_pMgrServicePackageCCAData[i].m_iServicePackageCCASeq,i);

			lkey = SHMDataMgr::changekey(m_pMgrServicePackageCCAData[i].m_strServiceContextId,m_pSHMMgrServicePackageCCAData->getTotal());

			if (m_pSHMMgrServicePackageCCAindex->get(lkey,&value)) {
				int pos = m_pMgrServicePackageCCAData[value].m_iNext;
				m_pMgrServicePackageCCAData[value].m_iNext = i;
				m_pMgrServicePackageCCAData[i].m_iNext = pos;
			} else {
				m_pSHMMgrServicePackageCCAindex->add(lkey,i);
				m_pMgrServicePackageCCAData[i].m_iNext = 0;
			}

		}
	}
	slQry.close();
	if (m_pSHMMgrServicePackageCCAData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//查询共享内存数据区信息
int SHMDataMgr::querypackageccaData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServicePackageCCAData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServicePackageCCAData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrServicePackageCCAData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_cca(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServicePackageCCAData->connnum()-1;

	_oShmInfo.m_iTotal = m_pSHMMgrServicePackageCCAData->getTotal()+1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageCCAData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrServicePackageCCAData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querypackageccaIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServicePackageCCAindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServicePackageCCAindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrServicePackageCCAindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_cca(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServicePackageCCAindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageCCAData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrServicePackageCCAindex->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querypackageccaIndex_a(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvPackageCCASeqIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvPackageCCASeqIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvPackageCCASeqIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_cca(tables)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvPackageCCASeqIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageCCAData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSrvPackageCCASeqIndex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createpackagevleSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SERVICEPACKAGEVLE_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SERVICEPACKAGEVLE_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SERVICEPACKAGEVLE_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrServicePackageVariableData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建service_package_variable共享内存请求分配的内存空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrServicePackageVariableData->create(_lMaxCount);
			m_pMgrServicePackageVariableData = (CServicePackageVariable *)(*m_pSHMMgrServicePackageVariableData);

			if (!m_pSHMMgrServicePackageVariableindex->exist()) {
				m_pSHMMgrServicePackageVariableindex->create(_lMaxCount);
			}else {
				cout<<"(service_package_variable)共享内存索引区已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrServicePackageVariableData->create(_lCount);
			m_pMgrServicePackageVariableData = (CServicePackageVariable *)(*m_pSHMMgrServicePackageVariableData);

			if (!m_pSHMMgrServicePackageVariableindex->exist()) {
				m_pSHMMgrServicePackageVariableindex->create(_lCount);
			}else {
				cout<<"(service_package_variable)共享内存索引区已经存在"<<endl;
			}

		}
	} else {
		cout<<"(service_package_variable)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrServicePackageVariableindex->exist()) {
			m_pSHMMgrServicePackageVariableindex->create(_lCount);
		}else {
			cout<<"(service_package_variable)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadpackagevleSHM()
{
	if (m_pSHMMgrServicePackageVariableData->exist()) {
		m_pSHMMgrServicePackageVariableData->remove();
	}

	if (m_pSHMMgrServicePackageVariableindex->exist()) {
		m_pSHMMgrServicePackageVariableindex->remove();
	}
	m_pSHMMgrServicePackageVariableLock->close();
}

//数据全量加载
int SHMDataMgr::loadpackagevle()
{

	char sSQL[BUFF_MAX_SIZE] ={0};
	char skey[8] = {0};
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select Variable_ID,nvl(Variable_Name,-1),Variable_Type,Get_Value_Mode, \
								  nvl(Source_AVP,-1),Source_AVP_TYPE,nvl(Source_Data,-1) \
								  from service_package_variable ");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrServicePackageVariableData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息service_package_variable共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrServicePackageVariableData[i].m_iVariableID = GETVALUE_INT(slQry,0);
			strncpy(m_pMgrServicePackageVariableData[i].m_szVariableName,GETVALUE_STRING(slQry,1),sizeof(m_pMgrServicePackageVariableData[i].m_szVariableName)-1);
			m_pMgrServicePackageVariableData[i].m_iVariableType = GETVALUE_INT(slQry,2);
			m_pMgrServicePackageVariableData[i].m_iGetValueMode = GETVALUE_INT(slQry,3);
			strncpy(m_pMgrServicePackageVariableData[i].m_szSourceAVP,GETVALUE_STRING(slQry,4),sizeof(m_pMgrServicePackageVariableData[i].m_szSourceAVP)-1);
			m_pMgrServicePackageVariableData[i].m_iSourceAVPType = GETVALUE_INT(slQry,5);
			strncpy(m_pMgrServicePackageVariableData[i].m_szSourceData,GETVALUE_STRING(slQry,6),sizeof(m_pMgrServicePackageVariableData[i].m_szSourceData)-1);
			m_pSHMMgrServicePackageVariableindex->add (m_pMgrServicePackageVariableData[i].m_iVariableID,i);
		}
	}
	slQry.close();
	if (m_pSHMMgrServicePackageVariableData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//查询共享内存数据区信息
int SHMDataMgr::querypackagevleData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServicePackageVariableData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServicePackageVariableData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrServicePackageVariableData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_variable(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServicePackageVariableData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrServicePackageVariableData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageVariableData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrServicePackageVariableData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querypackagevleIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServicePackageVariableindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServicePackageVariableindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrServicePackageVariableindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_variable(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServicePackageVariableindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageVariableData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrServicePackageVariableindex->getCount());
	return 0;
}


//创建共享内存数据区和索引区
int SHMDataMgr::createpackagefuncSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SERVICEPACKAGEFUNC_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SERVICEPACKAGEFUNC_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SERVICEPACKAGEFUNC_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrServicePackageFuncData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存service_package_func请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);
			sleep(3);
			m_pSHMMgrServicePackageFuncData->create(_lMaxCount);
			m_pMgrServicePackageFuncData = (CServicePackageFunc *)(*m_pSHMMgrServicePackageFuncData);
			if (!m_pSHMMgrServicePackageFuncindex->exist()) {
				m_pSHMMgrServicePackageFuncindex->create(_lMaxCount);
			}else {
				cout<<"(service_package_func)共享内存索引区已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrServicePackageFuncData->create(_lCount);
			m_pMgrServicePackageFuncData = (CServicePackageFunc *)(*m_pSHMMgrServicePackageFuncData);

			if (!m_pSHMMgrServicePackageFuncindex->exist()) {
				m_pSHMMgrServicePackageFuncindex->create(_lCount);
			}else {
				cout<<"(service_package_func)共享内存索引区已经存在"<<endl;
			}

		}
	} else {
		cout<<"(service_package_func)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrServicePackageFuncindex->exist()) {
			m_pSHMMgrServicePackageFuncindex->create(_lCount);
		}else {
			cout<<"(service_package_func)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadpackagefuncSHM()
{

	if (m_pSHMMgrServicePackageFuncData->exist()) {
		m_pSHMMgrServicePackageFuncData->remove();
	}

	if (m_pSHMMgrServicePackageFuncindex->exist()) {
		m_pSHMMgrServicePackageFuncindex->remove();
	}
	m_pSHMMgrServicePackageFuncLock->close();
}

//数据全量加载
int SHMDataMgr::loadpackagefunc()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	char skey[8] = {0};
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select Call_ID,Call_Func_ID,nvl(Func_Param1,-1),nvl(Func_Param1_Type,-1),\
								  nvl(Func_Param2,-1),nvl(Func_Param2_Type,-1) from service_package_func ");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrServicePackageFuncData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息service_package_func共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrServicePackageFuncData[i].m_iCallID = GETVALUE_INT(slQry,0);
			m_pMgrServicePackageFuncData[i].m_iCallFuncID = GETVALUE_INT(slQry,1);
			m_pMgrServicePackageFuncData[i].m_iFuncParam1 = GETVALUE_INT(slQry,2);
			m_pMgrServicePackageFuncData[i].m_iFuncParam1Type = GETVALUE_INT(slQry,3);
			m_pMgrServicePackageFuncData[i].m_iFuncParam2 = GETVALUE_INT(slQry,4);
			m_pMgrServicePackageFuncData[i].m_iFuncParam2Type = GETVALUE_INT(slQry,5);
			m_pSHMMgrServicePackageFuncindex->add (m_pMgrServicePackageFuncData[i].m_iCallID,i);
		}
	}
	slQry.close();
	if (m_pSHMMgrServicePackageFuncData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//查询共享内存数据区信息
int SHMDataMgr::querypackagefuncData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServicePackageFuncData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServicePackageFuncData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrServicePackageFuncData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_func(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServicePackageFuncData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrServicePackageFuncData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageFuncData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrServicePackageFuncData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querypackagefuncIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrServicePackageFuncindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrServicePackageFuncindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrServicePackageFuncindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"service_package_func(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrServicePackageFuncindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrServicePackageFuncData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrServicePackageFuncindex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createscongestmsgSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SCONGESTMSG_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SCONGESTMSG_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SCONGESTMSG_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrSCongestMsgData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存StatisticsCongestMsg请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrSCongestMsgData->create(_lMaxCount);
			m_pMgrSCongestMsgData = (StatisticsCongestMsg *)(*m_pSHMMgrSCongestMsgData);
			if (!m_pSHMMgrSCongestMsgindex->exist()) {
				m_pSHMMgrSCongestMsgindex->create(_lMaxCount);
			}else {
				cout<<"(共享内存消息统计)共享内存索引区已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrSCongestMsgData->create(_lCount);
			m_pMgrSCongestMsgData = (StatisticsCongestMsg *)(*m_pSHMMgrSCongestMsgData);

			if (!m_pSHMMgrSCongestMsgindex->exist()) {
				m_pSHMMgrSCongestMsgindex->create(_lCount);
			}else {
				cout<<"(共享内存消息统计)共享内存索引区已经存在"<<endl;
			}

		}
	} else {
		cout<<"(共享内存消息统计)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrSCongestMsgindex->exist()) {
			m_pSHMMgrSCongestMsgindex->create(_lCount);
		}else {
			cout<<"(共享内存消息统计)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadscongestmsgSHM()
{

	if (m_pSHMMgrSCongestMsgData->exist()) {
		m_pSHMMgrSCongestMsgData->remove();
	}

	if (m_pSHMMgrSCongestMsgindex->exist()) {
		m_pSHMMgrSCongestMsgindex->remove();
	}

	m_pSHMMgrSCongestMsgLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryscongestmsgData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSCongestMsgData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSCongestMsgData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrSCongestMsgData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"statisticscongestmsg(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSCongestMsgData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrSCongestMsgData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSCongestMsgData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrSCongestMsgData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryscongestmsgIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSCongestMsgindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSCongestMsgindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSCongestMsgindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"statisticscongestmsg(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSCongestMsgindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSCongestMsgData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSCongestMsgindex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createwfprocessmqSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_WFPROCESSMQ_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_WFPROCESSMQ_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_WFPROCESSMQ_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrWfprocessMqData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存wf_process_mq请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrWfprocessMqData->create(_lMaxCount);
			m_pMgrWfprocessMqData = (ST_WF_PROCESS_MQ *)(*m_pSHMMgrWfprocessMqData);
			if (!m_pSHMMgrWfprocessMqindex->exist()) {
				m_pSHMMgrWfprocessMqindex->create(_lMaxCount);
			}else {
				cout<<"(wf_process_mq)共享内存索引区已经存在"<<endl;
			}

			return _lMaxCount;
		} else {
			m_pSHMMgrWfprocessMqData->create(_lCount);
			m_pMgrWfprocessMqData = (ST_WF_PROCESS_MQ *)(*m_pSHMMgrWfprocessMqData);

			if (!m_pSHMMgrWfprocessMqindex->exist()) {
				m_pSHMMgrWfprocessMqindex->create(_lCount);
			}else {
				cout<<"(wf_process_mq)共享内存索引区已经存在"<<endl;
			}

		}
	} else {
		cout<<"(wf_process_mq)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrWfprocessMqindex->exist()) {
			m_pSHMMgrWfprocessMqindex->create(_lCount);
		}else {
			cout<<"(wf_process_mq)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadwfprocessmqSHM()
{
	if (m_pSHMMgrWfprocessMqData->exist()) {
		m_pSHMMgrWfprocessMqData->remove();
	}

	if (m_pSHMMgrWfprocessMqindex->exist()) {
		m_pSHMMgrWfprocessMqindex->remove();
	}
	m_pSHMMgrWfprocessMqLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::querywfprocessmqData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrWfprocessMqData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrWfprocessMqData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrWfprocessMqData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"wf_process_mq(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrWfprocessMqData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrWfprocessMqData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrWfprocessMqData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrWfprocessMqData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querywfprocessmqIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrWfprocessMqindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrWfprocessMqindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrWfprocessMqindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"wf_process_mq(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrWfprocessMqindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrWfprocessMqData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrWfprocessMqindex->getCount());
	return 0;
}

int SHMDataMgr::loadwfprocessmq()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select PROCESS_ID,MQ_ID from WF_PROCESS_MQ ");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrWfprocessMqData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息WF_PROCESS_MQ共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrWfprocessMqData[i].m_iProcessID = GETVALUE_INT(slQry,0);
			m_pMgrWfprocessMqData[i].m_iMqID = GETVALUE_INT(slQry,1);

			m_pSHMMgrWfprocessMqindex->add (m_pMgrWfprocessMqData[i].m_iProcessID,i);
		}
	}
	slQry.close();
	if (m_pSHMMgrWfprocessMqData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createportinfoSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_PORTINFO_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_PORTINFO_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_PORTINFO_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrPortInfoData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存wf_process_mq请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrPortInfoData->create(_lMaxCount);
			m_pMgrPortInfoData = (PortInfo *)(*m_pSHMMgrPortInfoData);
			if (!m_pSHMMgrPortInfoindex->exist()) {
				m_pSHMMgrPortInfoindex->create(_lMaxCount);
			}else {
				cout<<"(portInfo)共享内存索引区已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrPortInfoData->create(_lCount);
			m_pMgrPortInfoData = (PortInfo *)(*m_pSHMMgrPortInfoData);

			if (!m_pSHMMgrPortInfoindex->exist()) {
				m_pSHMMgrPortInfoindex->create(_lCount);
			}else {
				cout<<"(portInfo)共享内存索引区已经存在"<<endl;
			}
		}
	} else {
		cout<<"(portInfo)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrPortInfoindex->exist()) {
			m_pSHMMgrPortInfoindex->create(_lCount);
		}else {
			cout<<"(portInfo)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadportinfoSHM()
{
	if (m_pSHMMgrPortInfoData->exist()) {
		m_pSHMMgrPortInfoData->remove();
	}

	if (m_pSHMMgrPortInfoindex->exist()) {
		m_pSHMMgrPortInfoindex->remove();
	}
	m_pSHMMgrPortInfoLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryportinfoData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrPortInfoData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrPortInfoData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrPortInfoData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"portinfo(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrPortInfoData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrPortInfoData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrPortInfoData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrPortInfoData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryportinfoIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrPortInfoindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrPortInfoindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrPortInfoindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"portinfo(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrPortInfoindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrPortInfoData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrPortInfoindex->getCount());
	return 0;
}

//查询共享内存数据区信息
int SHMDataMgr::querySessionClearData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSessionClearData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSessionClearData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrSessionClearData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"session_clear(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSessionClearData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrSessionClearData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSessionClearData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrSessionClearData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querySessionClearIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSessionClearIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSessionClearIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSessionClearIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"session_clear(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSessionClearIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSessionClearData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSessionClearIndex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createOrgRouteSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_ORGROUTE_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_ORGROUTE_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_ORGROUTE_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrOrgRouteData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_org_route请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrOrgRouteData->create(_lMaxCount);
			m_pMgrOrgRouteData = (Sgw_Org_Route *)(*m_pSHMMgrOrgRouteData);

			if (!m_pSHMMgrOrgRouteindex->exist()) {
				m_pSHMMgrOrgRouteindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_org_route)共享内存索引区已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrOrgRouteData->create(_lCount);
			m_pMgrOrgRouteData = (Sgw_Org_Route *)(*m_pSHMMgrOrgRouteData);

			if (!m_pSHMMgrOrgRouteindex->exist()) {
				m_pSHMMgrOrgRouteindex->create(_lCount);
			}else {
				cout<<"(sgw_org_route)共享内存索引区已经存在"<<endl;
			}

		}
	} else {
		cout<<"(sgw_org_route)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrOrgRouteindex->exist()) {
			m_pSHMMgrOrgRouteindex->create(_lCount);
		}else {
			cout<<"(sgw_org_route)共享内存索引区已经存在"<<endl;
		}

		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadOrgRouteData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	unsigned long lkey  = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select org_id,org_level_id,nvl(parent_org_id,-1), \
								  org_area_code from org");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrOrgRouteData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息WF_PROCESS_MQ共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrOrgRouteData[i].m_iOrgID = GETVALUE_INT(slQry,0);
			m_pMgrOrgRouteData[i].m_iOrgLevelID = GETVALUE_INT(slQry,1);
			m_pMgrOrgRouteData[i].m_iParentOrgID = GETVALUE_INT(slQry,2);
			strncpy(m_pMgrOrgRouteData[i].m_szOrgAreaCode,GETVALUE_STRING(slQry,3),sizeof(m_pMgrOrgRouteData[i].m_szOrgAreaCode));
			m_pSHMMgrOrgRouteindex->add (m_pMgrOrgRouteData[i].m_iOrgID,i);
		}
	}
	slQry.close();
	if (m_pSHMMgrOrgRouteData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadOrgRouteSHM()
{
	if (m_pSHMMgrOrgRouteData->exist()) {
		m_pSHMMgrOrgRouteData->remove();
	}

	if (m_pSHMMgrOrgRouteindex->exist()) {
		m_pSHMMgrOrgRouteindex->remove();
	}
	m_pSHMMgrOrgRouteLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryOrgRouteData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrOrgRouteData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrOrgRouteData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrOrgRouteData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_org_route(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrOrgRouteData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrOrgRouteData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrOrgRouteData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrOrgRouteData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryOrgRouteIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrOrgRouteindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrOrgRouteindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrOrgRouteindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_org_route(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrOrgRouteindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrOrgRouteindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrOrgRouteindex->getCount());
	return 0;
}

int SHMDataMgr::createGlobalMapSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_GLOBALMAP_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_);
	long _lMaxCount = GETVALUE(_SHM_GLOBALMAP_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrGlobalMapData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_global_map请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrGlobalMapData->create(_lMaxCount);
			m_pMgrGlobalMapData = (SgwGlobalMap *)(*m_pSHMMgrGlobalMapData);

			if (!m_pSHMMgrGlobalMapindex->exist()) {
				m_pSHMMgrGlobalMapindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_global_map)共享内存索引区已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrGlobalMapData->create(_lCount);
			m_pMgrGlobalMapData = (SgwGlobalMap *)(*m_pSHMMgrGlobalMapData);

			if (!m_pSHMMgrGlobalMapindex->exist()) {
				m_pSHMMgrGlobalMapindex->create(_lCount);
			}else {
				cout<<"(sgw_global_map)共享内存索引区已经存在"<<endl;
			}
		}
	} else {
		cout<<"(sgw_global_map)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrGlobalMapindex->exist()) {
			m_pSHMMgrGlobalMapindex->create(_lCount);
		}else {
			cout<<"(sgw_global_map)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadGlobalMapSHM()
{
	if (m_pSHMMgrGlobalMapData->exist()) {
		m_pSHMMgrGlobalMapData->remove();
	}

	if (m_pSHMMgrGlobalMapindex->exist()) {
		m_pSHMMgrGlobalMapindex->remove();
	}
	m_pSHMMgrGlobalMapLock->close();
}

int SHMDataMgr::loadGlobalMapData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select g_id,service_context_id,operate, \
								  msg_type,source_avp,source_type,des_avp, \
								  des_type,nvl(format,-1),nvl(Selective,-1) \
								  from sgw_global_map ");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrGlobalMapData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息sgw_global_map共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrGlobalMapData[i].m_iGID = GETVALUE_INT(slQry,0);
			strncpy(m_pMgrGlobalMapData[i].m_szServiceContextID,GETVALUE_STRING(slQry,1),sizeof(m_pMgrGlobalMapData[i].m_szServiceContextID));
			m_pMgrGlobalMapData[i].m_iOperate = GETVALUE_INT(slQry,2);
			m_pMgrGlobalMapData[i].m_iMsgType = GETVALUE_INT(slQry,3);
			strncpy(m_pMgrGlobalMapData[i].m_szSourceAvp,GETVALUE_STRING(slQry,4),sizeof(m_pMgrGlobalMapData[i].m_szSourceAvp));
			m_pMgrGlobalMapData[i].m_iSourceType = GETVALUE_INT(slQry,5);
			strncpy(m_pMgrGlobalMapData[i].m_szDesAvp,GETVALUE_STRING(slQry,6),sizeof(m_pMgrGlobalMapData[i].m_szDesAvp));
			m_pMgrGlobalMapData[i].m_iDesType = GETVALUE_INT(slQry,7);
			strncpy(m_pMgrGlobalMapData[i].m_szFormat,GETVALUE_STRING(slQry,8),sizeof(m_pMgrGlobalMapData[i].m_szFormat));
			strncpy(m_pMgrGlobalMapData[i].m_szSelective,GETVALUE_STRING(slQry,9),sizeof(m_pMgrGlobalMapData[i].m_szSelective));
			m_pSHMMgrGlobalMapindex->add(m_pMgrGlobalMapData[i].m_iGID ,i);
		}
	}
	slQry.close();
	if (m_pSHMMgrGlobalMapData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//查询共享内存数据区信息
int SHMDataMgr::queryGlobalMapData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrGlobalMapData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrGlobalMapData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrGlobalMapData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_global_map(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrGlobalMapData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrGlobalMapData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrGlobalMapData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrGlobalMapData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryGlobalMapIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrGlobalMapindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrGlobalMapindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrGlobalMapindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_global_map(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrGlobalMapindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrGlobalMapindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrGlobalMapindex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createOrgHeadSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_ORGHEAD_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_ORGHEAD_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_ORGHEAD_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrOrgHeadData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_org_head请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrOrgHeadData->create(_lMaxCount);
			m_pMgrOrgHeadData = (Sgw_Org_Head *)(*m_pSHMMgrOrgHeadData);

			if (!m_pSHMMgrOrgHeadindex->exist()) {
				m_pSHMMgrOrgHeadindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_org_head)共享内存索引区已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrOrgHeadData->create(_lCount);
			m_pMgrOrgHeadData = (Sgw_Org_Head *)(*m_pSHMMgrOrgHeadData);

			if (!m_pSHMMgrOrgHeadindex->exist()) {
				m_pSHMMgrOrgHeadindex->create(_lCount);
			}else {
				cout<<"(sgw_org_head)共享内存索引区已经存在"<<endl;
			}
		}
	} else {
		cout<<"(sgw_org_head)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrOrgHeadindex->exist()) {
			m_pSHMMgrOrgHeadindex->create(_lCount);
		}else {
			cout<<"(sgw_org_head)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadOrgHeadData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	DEFINE_SGW_QUERY(slQry);
	unsigned long _ulHead = 0;
	snprintf (sSQL,sizeof(sSQL)-1,"select org_id,area_code||head head,to_char(eff_date,'YYYYMMDD'), \
								  to_char(exp_date,'YYYYMMDD'), \
								  'local_head' table_name from local_head where eff_date<=trunc(sysdate,'DD') \
								  and exp_date>trunc(sysdate,'DD') \
								  union all \
								  select region_id org_id,head ,to_char(eff_date,'YYYYMMDD'), \
								  to_char(exp_date,'YYYYMMDD'), \
								  'b_head_region' table_name from b_head_region  \
								  where type_code='52Z' and eff_date <= trunc(sysdate,'DD') \
								  and exp_date>trunc(sysdate,'DD')");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrOrgHeadData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息org_head共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrOrgHeadData[i].m_iOrgID = GETVALUE_INT(slQry,0);
			strncpy(m_pMgrOrgHeadData[i].m_szHead,GETVALUE_STRING(slQry,1),sizeof(m_pMgrOrgHeadData[i].m_szHead));
			strncpy(m_pMgrOrgHeadData[i].m_szEffDate,GETVALUE_STRING(slQry,2),sizeof(m_pMgrOrgHeadData[i].m_szEffDate));
			strncpy(m_pMgrOrgHeadData[i].m_szExpDate,GETVALUE_STRING(slQry,3),sizeof(m_pMgrOrgHeadData[i].m_szExpDate));
			strncpy(m_pMgrOrgHeadData[i].m_szTableNmae,GETVALUE_STRING(slQry,4),sizeof(m_pMgrOrgHeadData[i].m_szTableNmae));
			m_pSHMMgrOrgHeadindex->add (SHMDataMgr::trim(m_pMgrOrgHeadData[i].m_szHead),i);
		}
	}
	slQry.close();
	if (m_pSHMMgrOrgHeadData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadOrgHeadSHM()
{
	if (m_pSHMMgrOrgHeadData->exist()) {
		m_pSHMMgrOrgHeadData->remove();
	}

	if (m_pSHMMgrOrgHeadindex->exist()) {
		m_pSHMMgrOrgHeadindex->remove();
	}
	m_pSHMMgrOrgHeadLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryOrgHeadData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrOrgHeadData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrOrgHeadData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrOrgHeadData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_org_head(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrOrgHeadData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrOrgHeadData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrOrgHeadData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrOrgHeadData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryOrgHeadIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrOrgHeadindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrOrgHeadindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrOrgHeadindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_org_head(others)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrOrgHeadindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrOrgHeadindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrOrgHeadindex->getCount());
	return 0;
}


//创建共享内存数据区和索引区
int SHMDataMgr::createChannelSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_CHANNEL_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_CHANNEL_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_CHANNEL_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrChannelData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存channel_info请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrChannelData->create(_lMaxCount);
			m_pMgrChannelData = (SgwChannelInfo *)(*m_pSHMMgrChannelData);

			if (!m_pSHMMgrChannelindex->exist()) {
				m_pSHMMgrChannelindex->create(_lMaxCount);
			}else {
				cout<<"(channel_info)共享内存索引区已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrChannelData->create(_lCount);
			m_pMgrChannelData = (SgwChannelInfo *)(*m_pSHMMgrChannelData);

			if (!m_pSHMMgrChannelindex->exist()) {
				m_pSHMMgrChannelindex->create(_lCount);
			}else {
				cout<<"(channel_info)共享内存索引区已经存在"<<endl;
			}
		}
	} else {
		cout<<"(channel_info)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrChannelindex->exist()) {
			m_pSHMMgrChannelindex->create(_lCount);
		}else {
			cout<<"(channel_info)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadChannelData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	DEFINE_SGW_QUERY(slQry);
	long lkey = 0;
	unsigned int value = 0;
	unsigned long _ulHead = 0;
	snprintf (sSQL,sizeof(sSQL)-1,"SELECT CHANNEL_ID,CHANNEL_NAME,CHECK_FLAG, \
								  ENCODE,to_char(EFF_DATE,'YYYYMMDD'), \
								  to_char(EXP_DATE,'YYYYMMDD'), \
								  NETINFO_CODE,USER_SEQ \
								  FROM SGW_CHANNEL ORDER BY CHANNEL_ID");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrChannelData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息channel_info共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			strncpy(m_pMgrChannelData[i].m_szChannelID,GETVALUE_STRING(slQry,0),sizeof(m_pMgrChannelData[i].m_szChannelID));
			strncpy(m_pMgrChannelData[i].m_szChannelName,GETVALUE_STRING(slQry,1),sizeof(m_pMgrChannelData[i].m_szChannelName));
			strncpy(m_pMgrChannelData[i].m_szCheckFlag,GETVALUE_STRING(slQry,2),sizeof(m_pMgrChannelData[i].m_szCheckFlag));
			strncpy(m_pMgrChannelData[i].m_szEnCode,GETVALUE_STRING(slQry,3),sizeof(m_pMgrChannelData[i].m_szEnCode));
			strncpy(m_pMgrChannelData[i].m_szEffDate,GETVALUE_STRING(slQry,4),sizeof(m_pMgrChannelData[i].m_szEffDate));
			strncpy(m_pMgrChannelData[i].m_szExpDate,GETVALUE_STRING(slQry,5),sizeof(m_pMgrChannelData[i].m_szExpDate));
			strncpy(m_pMgrChannelData[i].m_szNetInfoCode,GETVALUE_STRING(slQry,6),sizeof(m_pMgrChannelData[i].m_szNetInfoCode));
			m_pMgrChannelData[i].m_iUserSeq = GETVALUE_INT(slQry,7);

			lkey = SHMDataMgr::changekey(m_pMgrChannelData[i].m_szChannelID,m_pSHMMgrChannelData->getTotal());

			if (m_pSHMMgrChannelindex->get(lkey,&value)) {
				m_pMgrChannelData[i].m_iNext = m_pMgrChannelData[value].m_iNext;
				m_pMgrChannelData[value].m_iNext = i;
			} else {
				m_pSHMMgrChannelindex->add(lkey,i);
				m_pMgrChannelData[i].m_iNext = 0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrChannelData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadChannelSHM()
{
	if (m_pSHMMgrChannelData->exist()) {
		m_pSHMMgrChannelData->remove();
	}

	if (m_pSHMMgrChannelindex->exist()) {
		m_pSHMMgrChannelindex->remove();
	}
	m_pSHMMgrChannelLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryChannelData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrChannelData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrChannelData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrChannelData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"channel_info(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrChannelData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrChannelData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrChannelData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrChannelData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryChannelIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrChannelindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrChannelindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrChannelindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"channel_info(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrChannelindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrChannelindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrChannelindex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createUserStaffSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_USERSTAFF_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_USERSTAFF_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_USERSTAFF_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrUserStaffData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_user_staff请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrUserStaffData->create(_lMaxCount);
			m_pMgrUserStaffData = (SgwUserStaff *)(*m_pSHMMgrUserStaffData);

			if (!m_pSHMMgrUserStaffIDindex->exist()) {
				m_pSHMMgrUserStaffIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_user_staff)共享内存索引B区已经存在"<<endl;
			}
			if (!m_pSHMMgrStaffIDindex->exist()) {
				m_pSHMMgrStaffIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_user_staff)共享内存索引A区已经存在"<<endl;
			}
			if (!m_pSHMMgrUserStaffChannelIDindex->exist()) {
				m_pSHMMgrUserStaffChannelIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_user_staff)共享内存索引C区已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrUserStaffData->create(_lCount);
			m_pMgrUserStaffData = (SgwUserStaff *)(*m_pSHMMgrUserStaffData);

			if (!m_pSHMMgrUserStaffIDindex->exist()) {
				m_pSHMMgrUserStaffIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_user_staff)共享内存索引B区已经存在"<<endl;
			}
			if (!m_pSHMMgrStaffIDindex->exist()) {
				m_pSHMMgrStaffIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_user_staff)共享内存索引A区已经存在"<<endl;
			}
			if (!m_pSHMMgrUserStaffChannelIDindex->exist()) {
				m_pSHMMgrUserStaffChannelIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_user_staff)共享内存索引C区已经存在"<<endl;
			}
		}
	} else {
		cout<<"(sgw_user_staff)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrUserStaffIDindex->exist()) {
			m_pSHMMgrUserStaffIDindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_user_staff)共享内存索引B区已经存在"<<endl;
		}
		if (!m_pSHMMgrStaffIDindex->exist()) {
			m_pSHMMgrStaffIDindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_user_staff)共享内存索引A区已经存在"<<endl;
		}
		if (!m_pSHMMgrUserStaffChannelIDindex->exist()) {
			m_pSHMMgrUserStaffChannelIDindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_user_staff)共享内存索引C区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadUserStaffData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"SELECT USER_STAFF_ID,STAFF_ID,STAFF_NAME,SGW_CHANNEL_ID from SGW_USER_STAFF");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrUserStaffData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息sgw_user_staff共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrUserStaffData[i].m_iUserStaffID =GETVALUE_LONG(slQry,0);
			m_pMgrUserStaffData[i].m_lStaffID=GETVALUE_LONG(slQry,1);
			strncpy(m_pMgrUserStaffData[i].m_szStaffName,GETVALUE_STRING(slQry,2),sizeof(m_pMgrUserStaffData[i].m_szStaffName));
			strncpy(m_pMgrUserStaffData[i].m_szChannelID,GETVALUE_STRING(slQry,3),sizeof(m_pMgrUserStaffData[i].m_szChannelID));

			m_pSHMMgrUserStaffIDindex->add (m_pMgrUserStaffData[i].m_iUserStaffID,i);
			m_pSHMMgrStaffIDindex->add (m_pMgrUserStaffData[i].m_lStaffID,i);

			lkey = SHMDataMgr::changekey(m_pMgrUserStaffData[i].m_szChannelID,m_pSHMMgrUserStaffData->getTotal());
			if (m_pSHMMgrUserStaffChannelIDindex->get(lkey,&value)) {
				int pos = m_pMgrUserStaffData[value].m_iNext;
				m_pMgrUserStaffData[i].m_iNext = pos;
				m_pMgrUserStaffData[value].m_iNext = i;
			} else {
				m_pSHMMgrUserStaffChannelIDindex->add (lkey,i);
				m_pMgrUserStaffData[i].m_iNext = 0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrUserStaffData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadUserStaffSHM()
{
	if (m_pSHMMgrUserStaffData->exist()) {
		m_pSHMMgrUserStaffData->remove();
	}

	if (m_pSHMMgrUserStaffIDindex->exist()) {
		m_pSHMMgrUserStaffIDindex->remove();
	}

	if (m_pSHMMgrStaffIDindex->exist()) {
		m_pSHMMgrStaffIDindex->remove();
	}

	if (m_pSHMMgrUserStaffChannelIDindex->exist()) {
		m_pSHMMgrUserStaffChannelIDindex->remove();
	}
	m_pSHMMgrUserStaffLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryUserStaffData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrUserStaffData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrUserStaffData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrUserStaffData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_user_staff(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrUserStaffData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrUserStaffData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrUserStaffData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrUserStaffData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryUserStaffIndex_a(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrUserStaffIDindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrUserStaffIDindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrUserStaffIDindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_user_staff(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrUserStaffIDindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrUserStaffIDindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrUserStaffIDindex->getCount());
	return 0;
}
//查询共享内存索引区信息
int SHMDataMgr::queryUserStaffIndex_b(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrStaffIDindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrStaffIDindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrStaffIDindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_user_staff(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrStaffIDindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrStaffIDindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrStaffIDindex->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryUserStaffIndex_c(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrUserStaffChannelIDindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrUserStaffChannelIDindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrUserStaffChannelIDindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_user_staff(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrUserStaffChannelIDindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrUserStaffChannelIDindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrUserStaffChannelIDindex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createRouteRuleSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_ORGROUTERULE_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_ORGROUTERULE_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_ORGROUTERULE_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrOrgRoutRuleData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_route_rule请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrOrgRoutRuleData->create(_lMaxCount);
			m_pMgrOrgRoutRuleData = (SgwOrgRouteRule *)(*m_pSHMMgrOrgRoutRuleData);

			if (!m_pSHMMgrOrgRouteIDindex->exist()) {
				m_pSHMMgrOrgRouteIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_route_rule)共享内存索引B区已经存在"<<endl;
			}
			if (!m_pSHMMgrOrgIDindex->exist()) {
				m_pSHMMgrOrgIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_route_rule)共享内存索引A区已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrOrgRoutRuleData->create(_lCount);
			m_pMgrOrgRoutRuleData = (SgwOrgRouteRule *)(*m_pSHMMgrOrgRoutRuleData);

			if (!m_pSHMMgrOrgRouteIDindex->exist()) {
				m_pSHMMgrOrgRouteIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_route_rule)共享内存索引B区已经存在"<<endl;
			}
			if (!m_pSHMMgrOrgIDindex->exist()) {
				m_pSHMMgrOrgIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_route_rule)共享内存索引A区已经存在"<<endl;
			}
		}
	} else {
		cout<<"(sgw_route_rule)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrOrgRouteIDindex->exist()) {
			m_pSHMMgrOrgRouteIDindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_route_rule)共享内存索引B区已经存在"<<endl;
		}
		if (!m_pSHMMgrOrgIDindex->exist()) {
			m_pSHMMgrOrgIDindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_route_rule)共享内存索引A区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadRouteRuleData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"SELECT ROUTE_ID,USER_SEQ,NETINFO_CODE,ORG_ID FROM SGW_ROUTE_RULE");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrOrgRoutRuleData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息SGW_ROUTE_RULE共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrOrgRoutRuleData[i].m_iRouteID =GETVALUE_INT(slQry,0);
			m_pMgrOrgRoutRuleData[i].m_iUserSeq=GETVALUE_INT(slQry,1);
			strncpy(m_pMgrOrgRoutRuleData[i].m_szNetInfoCode,GETVALUE_STRING(slQry,2),sizeof(m_pMgrOrgRoutRuleData[i].m_szNetInfoCode)-1);
			m_pMgrOrgRoutRuleData[i].m_iOrgID = GETVALUE_INT(slQry,3);
			m_pSHMMgrOrgRouteIDindex->add (m_pMgrOrgRoutRuleData[i].m_iRouteID,i);

			if (m_pSHMMgrOrgIDindex->get(m_pMgrOrgRoutRuleData[i].m_iOrgID,&value)) {
				int tmp = m_pMgrOrgRoutRuleData[value].m_iNext;
				m_pMgrOrgRoutRuleData[value].m_iNext = i;
				m_pMgrOrgRoutRuleData[i].m_iNext = tmp;
			} else {
				m_pSHMMgrOrgIDindex->add(m_pMgrOrgRoutRuleData[i].m_iOrgID,i);
				m_pMgrOrgRoutRuleData[i].m_iNext = 0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrOrgRoutRuleData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadRouteRuleSHM()
{
	if (m_pSHMMgrOrgRoutRuleData->exist()) {
		m_pSHMMgrOrgRoutRuleData->remove();
	}

	if (m_pSHMMgrOrgRouteIDindex->exist()) {
		m_pSHMMgrOrgRouteIDindex->remove();
	}

	if (m_pSHMMgrOrgIDindex->exist()) {
		m_pSHMMgrOrgIDindex->remove();
	}

	m_pSHMMgrOrgRoutRuleLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryRouteRuleData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrOrgRoutRuleData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrOrgRoutRuleData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrOrgRoutRuleData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_route_rule(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrOrgRoutRuleData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrOrgRoutRuleData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrOrgRoutRuleData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrOrgRoutRuleData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryRouteRuleIndex_a(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrOrgRouteIDindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrOrgRouteIDindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrOrgRouteIDindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_route_rule(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrOrgRouteIDindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrOrgRouteIDindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrOrgRouteIDindex->getCount());
	return 0;
}
//查询共享内存索引区信息
int SHMDataMgr::queryRouteRuleIndex_b(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrOrgIDindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrOrgIDindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrOrgIDindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_route_rule(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrOrgIDindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrOrgIDindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrOrgIDindex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createAreaOrgSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_AREAORG_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_AREAORG_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_AREAORG_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrAreaOrgData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_area_org请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrAreaOrgData->create(_lMaxCount);
			m_pMgrAreaOrgData = (SgwAreaOrg *)(*m_pSHMMgrAreaOrgData);

			if (!m_pSHMMgrAreaOrgSeqindex->exist()) {
				m_pSHMMgrAreaOrgSeqindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_area_org)共享内存索引区已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrAreaOrgData->create(_lCount);
			m_pMgrAreaOrgData = (SgwAreaOrg *)(*m_pSHMMgrAreaOrgData);

			if (!m_pSHMMgrAreaOrgSeqindex->exist()) {
				m_pSHMMgrAreaOrgSeqindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_area_org)共享内存索引区已经存在"<<endl;
			}
		}
	} else {
		cout<<"(sgw_area_org)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrAreaOrgSeqindex->exist()) {
			m_pSHMMgrAreaOrgSeqindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_area_org)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadAreaOrgData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"SELECT AREA_SEQ,USER_SEQ,ORG_ID from SGW_AREA_ORG");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrAreaOrgData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息sgw_area_org共享内存空间不足");

			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrAreaOrgData[i].m_iAreaSeq = GETVALUE_INT(slQry,0);
			m_pMgrAreaOrgData[i].m_iUserSeq = GETVALUE_INT(slQry,1);
			m_pMgrAreaOrgData[i].m_iOrgID = GETVALUE_INT(slQry,2);

			m_pSHMMgrAreaOrgSeqindex->add (m_pMgrAreaOrgData[i].m_iAreaSeq,i);
		}
	}
	slQry.close();
	if (m_pSHMMgrAreaOrgData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadAreaOrgSHM()
{
	if (m_pSHMMgrAreaOrgData->exist()) {
		m_pSHMMgrAreaOrgData->remove();
	}

	if (m_pSHMMgrAreaOrgSeqindex->exist()) {
		m_pSHMMgrAreaOrgSeqindex->remove();
	}

	m_pSHMMgrAreaOrgLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryAreaOrgData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrAreaOrgData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrAreaOrgData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrAreaOrgData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_area_org(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrAreaOrgData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrAreaOrgData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrAreaOrgData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrAreaOrgData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryAreaOrgIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrAreaOrgData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrAreaOrgData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrAreaOrgData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_area_org(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrAreaOrgData->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrAreaOrgData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrAreaOrgData->getCount());
	return 0;
}


//创建共享内存数据区和索引区
int SHMDataMgr::createTuxRelaInSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_TUXRELAIN_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_TUXRELAIN_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_TUXRELAIN_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrTuxRelaInData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_tuxedo_relation_in请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrTuxRelaInData->create(_lMaxCount);
			m_pMgrTuxRelaInData = (SgwTuxedoRelationIn *)(*m_pSHMMgrTuxRelaInData);

			if (!m_pSHMMgrTuxInSvrsIDindex->exist()) {
				m_pSHMMgrTuxInSvrsIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_tuxedo_relation_in)共享内存索引区已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrTuxRelaInData->create(_lCount);
			m_pMgrTuxRelaInData = (SgwTuxedoRelationIn *)(*m_pSHMMgrTuxRelaInData);

			if (!m_pSHMMgrTuxInSvrsIDindex->exist()) {
				m_pSHMMgrTuxInSvrsIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_tuxedo_relation_in)共享内存索引区已经存在"<<endl;
			}
		}
	} else {
		cout<<"(sgw_tuxedo_relation_in)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrTuxInSvrsIDindex->exist()) {
			m_pSHMMgrTuxInSvrsIDindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_tuxedo_relation_in)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadTuxRelaInData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"select RELATION_ID,SERVICE_CONTEXT_ID,SERVICES_ID from SGW_TUXEDO_RELATION_IN");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrTuxRelaInData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息sgw_tuxedo_relation_in共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrTuxRelaInData[i].m_iRelationID =GETVALUE_INT(slQry,0);
			strncpy(m_pMgrTuxRelaInData[i].m_szServiceContextID,GETVALUE_STRING(slQry,1),sizeof(m_pMgrTuxRelaInData[i].m_szServiceContextID)-1);
			m_pMgrTuxRelaInData[i].m_iServicesID=GETVALUE_INT(slQry,2);
			m_pSHMMgrTuxInSvrsIDindex->add (m_pMgrTuxRelaInData[i].m_iServicesID,i);
		}
	}
	slQry.close();
	if (m_pSHMMgrTuxRelaInData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadTuxRelaInSHM()
{
	if (m_pSHMMgrTuxRelaInData->exist()) {
		m_pSHMMgrTuxRelaInData->remove();
	}

	if (m_pSHMMgrTuxInSvrsIDindex->exist()) {
		m_pSHMMgrTuxInSvrsIDindex->remove();
	}
	m_pSHMMgrTuxRelaInLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryTuxRelaInData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrTuxRelaInData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrTuxRelaInData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrTuxRelaInData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_tuxedo_relation_in(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrTuxRelaInData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrTuxRelaInData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrTuxRelaInData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrTuxRelaInData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryTuxRelaInIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrTuxInSvrsIDindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrTuxInSvrsIDindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrTuxInSvrsIDindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_tuxedo_relation_in(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrTuxInSvrsIDindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrTuxInSvrsIDindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrTuxInSvrsIDindex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createTuxRelaOutSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_TUXRELAOUT_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_TUXRELAOUT_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_TUXRELAOUT_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrTuxRelaOutData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_tuxedo_relation_out请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrTuxRelaOutData->create(_lMaxCount);
			m_pMgrTuxRelaOutData = (SgwTuxedoRelationOut *)(*m_pSHMMgrTuxRelaOutData);

			if (!m_pSHMMgrTuxOutSvrTxtIDindex->exist()) {
				m_pSHMMgrTuxOutSvrTxtIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_tuxedo_relation_out)共享内存索引区A已经存在"<<endl;
			}

			if (!m_pSHMMgrTuxOutRelaIDindex->exist()) {
				m_pSHMMgrTuxOutRelaIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_tuxedo_relation_out)共享内存索引区B已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrTuxRelaOutData->create(_lCount);
			m_pMgrTuxRelaOutData = (SgwTuxedoRelationOut *)(*m_pSHMMgrTuxRelaOutData);

			if (!m_pSHMMgrTuxOutSvrTxtIDindex->exist()) {
				m_pSHMMgrTuxOutSvrTxtIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_tuxedo_relation_out)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrTuxOutRelaIDindex->exist()) {
				m_pSHMMgrTuxOutRelaIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_tuxedo_relation_out)共享内存索引区B已经存在"<<endl;
			}
		}
	} else {
		cout<<"(sgw_tuxedo_relation_out)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrTuxOutSvrTxtIDindex->exist()) {
			m_pSHMMgrTuxOutSvrTxtIDindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_tuxedo_relation_out)共享内存索引区A已经存在"<<endl;
		}
		if (!m_pSHMMgrTuxOutRelaIDindex->exist()) {
			m_pSHMMgrTuxOutRelaIDindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_tuxedo_relation_out)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadTuxRelaOutData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
	snprintf (sSQL,sizeof(sSQL)-1,"SELECT RELATION_ID,SERVICE_CONTEXT_ID,SERVICES_ID from SGW_TUXEDO_RELATION_OUT");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrTuxRelaOutData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息sgw_tuxedo_relation_out共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrTuxRelaOutData[i].m_iRelationID =GETVALUE_INT(slQry,0);
			strncpy(m_pMgrTuxRelaOutData[i].m_szServiceContextID,GETVALUE_STRING(slQry,1),sizeof(m_pMgrTuxRelaOutData[i].m_szServiceContextID)-1);
			m_pMgrTuxRelaOutData[i].m_iServicesID=GETVALUE_INT(slQry,2);
			m_pSHMMgrTuxOutRelaIDindex->add(m_pMgrTuxRelaOutData[i].m_iRelationID,i);

			lkey = SHMDataMgr::changekey(m_pMgrTuxRelaOutData[i].m_szServiceContextID,m_pSHMMgrTuxRelaOutData->getTotal());

			if (m_pSHMMgrTuxOutSvrTxtIDindex->get(lkey,&value)) {
				int pos = m_pMgrTuxRelaOutData[value].m_iNext;
				m_pMgrTuxRelaOutData[i].m_iNext = pos;
				m_pMgrTuxRelaOutData[value].m_iNext = i;
			} else {
				m_pSHMMgrTuxOutSvrTxtIDindex->add (lkey,i);
				m_pMgrTuxRelaOutData[i].m_iNext = 0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrTuxRelaOutData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadTuxRelaOutSHM()
{
	if (m_pSHMMgrTuxRelaOutData->exist()) {
		m_pSHMMgrTuxRelaOutData->remove();
	}

	if (m_pSHMMgrTuxOutSvrTxtIDindex->exist()) {
		m_pSHMMgrTuxOutSvrTxtIDindex->remove();
	}

	if (m_pSHMMgrTuxOutRelaIDindex->exist()) {
		m_pSHMMgrTuxOutRelaIDindex->remove();
	}
	m_pSHMMgrTuxRelaOutLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryTuxRelaOutData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrTuxRelaOutData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrTuxRelaOutData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrTuxRelaOutData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_tuxedo_relation_out(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrTuxRelaOutData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrTuxRelaOutData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrTuxRelaOutData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrTuxRelaOutData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryTuxRelaOutIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrTuxOutSvrTxtIDindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrTuxOutSvrTxtIDindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrTuxOutSvrTxtIDindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_tuxedo_relation_out(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrTuxOutSvrTxtIDindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrTuxOutSvrTxtIDindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrTuxOutSvrTxtIDindex->getCount());
	return 0;
}

int SHMDataMgr::queryTuxRelaOutRelaIDIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrTuxOutRelaIDindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrTuxOutRelaIDindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrTuxOutRelaIDindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_tuxedo_relation_out(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrTuxOutRelaIDindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrTuxOutRelaIDindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrTuxOutRelaIDindex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createSvrListSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SVRLIST_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SVRLIST_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SVRLIST_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrSrvsListData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_services_list请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrSrvsListData->create(_lMaxCount);
			m_pMgrSrvsListData = (SgwServicesList *)(*m_pSHMMgrSrvsListData);

			if (!m_pSHMMgrSrvsListIDindex->exist()) {
				m_pSHMMgrSrvsListIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_services_list)共享内存索引区B已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvsListServicesNameindex->exist()) {
				m_pSHMMgrSrvsListServicesNameindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_services_list)共享内存索引区A已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrSrvsListData->create(_lCount);
			m_pMgrSrvsListData = (SgwServicesList *)(*m_pSHMMgrSrvsListData);

			if (!m_pSHMMgrSrvsListIDindex->exist()) {
				m_pSHMMgrSrvsListIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_services_list)共享内存索引区B已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvsListServicesNameindex->exist()) {
				m_pSHMMgrSrvsListServicesNameindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_services_list)共享内存索引区A已经存在"<<endl;
			}
		}
	} else {
		cout<<"(sgw_services_list)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrSrvsListIDindex->exist()) {
			m_pSHMMgrSrvsListIDindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_services_list)共享内存索引区B已经存在"<<endl;
		}
		if (!m_pSHMMgrSrvsListServicesNameindex->exist()) {
			m_pSHMMgrSrvsListServicesNameindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_services_list)共享内存索引区A已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadSvrListData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);
#ifndef __SQLITE__
	snprintf (sSQL,sizeof(sSQL)-1,"select SERVICES_ID,SERVICES_NAME,SERVICES_DESC, \
								  TO_CHAR(EFF_DATE,'YYYYMMDDHH24MISS'), \
								  TO_CHAR(EXP_DATE,'YYYYMMDD') from SGW_SERVICES_LIST");

#else
	snprintf (sSQL,sizeof(sSQL)-1,"select SERVICES_ID,SERVICES_NAME,SERVICES_DESC, \
								  EFF_DATE,EXP_DATE from SGW_SERVICES_LIST");

#endif
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrSrvsListData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息sgw_services_list共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrSrvsListData[i].m_iServicesID =GETVALUE_INT(slQry,0);
			strncpy(m_pMgrSrvsListData[i].m_szServicesName,GETVALUE_STRING(slQry,1),sizeof(m_pMgrSrvsListData[i].m_szServicesName)-1);
			strncpy(m_pMgrSrvsListData[i].m_szServicesDesc,GETVALUE_STRING(slQry,2),sizeof(m_pMgrSrvsListData[i].m_szServicesDesc)-1);
			strncpy(m_pMgrSrvsListData[i].m_szEffDate,GETVALUE_STRING(slQry,3),sizeof(m_pMgrSrvsListData[i].m_szEffDate)-1);
			strncpy(m_pMgrSrvsListData[i].m_szExpDate,GETVALUE_STRING(slQry,4),sizeof(m_pMgrSrvsListData[i].m_szExpDate)-1);
			m_pSHMMgrSrvsListIDindex->add (m_pMgrSrvsListData[i].m_iServicesID,i);

			long lKey = SHMDataMgr::changekey(m_pMgrSrvsListData[i].m_szServicesName,m_pSHMMgrSrvsListData->getTotal());
			if (m_pSHMMgrSrvsListServicesNameindex->get(lKey,&value)) {
				int pos = m_pMgrSrvsListData[value].m_iNext;
				m_pMgrSrvsListData[value].m_iNext = i;
				m_pMgrSrvsListData[i].m_iNext = pos;
			} else {
				m_pSHMMgrSrvsListServicesNameindex->add (lKey,i);
				m_pMgrSrvsListData[i].m_iNext = 0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrSrvsListData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadSvrListSHM()
{
	if (m_pSHMMgrSrvsListData->exist()) {
		m_pSHMMgrSrvsListData->remove();
	}

	if (m_pSHMMgrSrvsListIDindex->exist()) {
		m_pSHMMgrSrvsListIDindex->remove();
	}

	if (m_pSHMMgrSrvsListServicesNameindex->exist()) {
		m_pSHMMgrSrvsListServicesNameindex->remove();
	}
	m_pSHMMgrSrvsListLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::querySvrListData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvsListData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvsListData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvsListData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_services_list(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvsListData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrSrvsListData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSrvsListData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrSrvsListData->getCount());
	return 0;
}

//查询共享内存索引区B信息
int SHMDataMgr::querySvrListIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvsListIDindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvsListIDindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvsListIDindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_services_list(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvsListIDindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSrvsListIDindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSrvsListIDindex->getCount());
	return 0;
}

//查询共享内存索引区A信息
int SHMDataMgr::querySvrListServicesNameIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvsListServicesNameindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvsListServicesNameindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvsListServicesNameindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_services_list(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvsListServicesNameindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSrvsListServicesNameindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSrvsListServicesNameindex->getCount());
	return 0;
}



//创建共享内存数据区和索引区
int SHMDataMgr::createSvrsParamRelaSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SVRSPARAMRELA_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SVRSPARAMRELA_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SVRSPARAMRELA_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrSrvsParamRelaData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_services_param_relation请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrSrvsParamRelaData->create(_lMaxCount);
			m_pMgrSrvsParamRelaData = (SgwServicesParamRelation *)(*m_pSHMMgrSrvsParamRelaData);

			if (!m_pSHMMgrSrvsParamRelaindex->exist()) {
				m_pSHMMgrSrvsParamRelaindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_services_param_relation)共享内存索引区A已经存在"<<endl;
			}

			if (!m_pSHMMgrSrvsParamRelaMappingIDIndex->exist()) {
				m_pSHMMgrSrvsParamRelaMappingIDIndex->create(_lMaxCount);
			}else {
				cout<<"(sgw_services_param_relation)共享内存索引区B已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrSrvsParamRelaData->create(_lCount);
			m_pMgrSrvsParamRelaData = (SgwServicesParamRelation *)(*m_pSHMMgrSrvsParamRelaData);

			if (!m_pSHMMgrSrvsParamRelaindex->exist()) {
				m_pSHMMgrSrvsParamRelaindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_services_param_relation)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvsParamRelaMappingIDIndex->exist()) {
				m_pSHMMgrSrvsParamRelaMappingIDIndex->create(_lMaxCount);
			}else {
				cout<<"(sgw_services_param_relation)共享内存索引区B已经存在"<<endl;
			}
		}
	} else {
		cout<<"(sgw_services_param_relation)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrSrvsParamRelaindex->exist()) {
			m_pSHMMgrSrvsParamRelaindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_services_param_relation)共享内存索引区A已经存在"<<endl;
		}

		if (!m_pSHMMgrSrvsParamRelaMappingIDIndex->exist()) {
			m_pSHMMgrSrvsParamRelaMappingIDIndex->create(_lMaxCount);
		}else {
			cout<<"(sgw_services_param_relation)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadSvrsParamRelaData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);

	snprintf (sSQL,sizeof(sSQL)-1,"SELECT A.SERVICES_ID,A.PARAM_ID,A.PARAM_FLAG,A.MAPPING_ID, \
				B.PARAM_NAME,B.PARAM_VAL,B.PARAM_TYPE,B.PARAM_DESC,A.PARAM_LOG \
				FROM SGW_SERVICES_PARAM_RELATION A \
				LEFT JOIN SGW_SERVICES_PARAM_LIST B ON A.PARAM_ID = B.PARAM_ID");
	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrSrvsParamRelaData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息sgw_services_param_relation共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrSrvsParamRelaData[i].m_iServicesID =GETVALUE_INT(slQry,0);
			m_pMgrSrvsParamRelaData[i].m_iParamID =GETVALUE_INT(slQry,1);
			m_pMgrSrvsParamRelaData[i].m_iParamFlag =GETVALUE_INT(slQry,2);
			m_pMgrSrvsParamRelaData[i].m_iMappingID =GETVALUE_INT(slQry,3);
			strncpy(m_pMgrSrvsParamRelaData[i].m_szParamName,GETVALUE_STRING(slQry,4),sizeof(m_pMgrSrvsParamRelaData[i].m_szParamName)-1);
			m_pMgrSrvsParamRelaData[i].m_iParamVal =GETVALUE_INT(slQry,5);
			m_pMgrSrvsParamRelaData[i].m_iParamType =GETVALUE_INT(slQry,6);
			strncpy(m_pMgrSrvsParamRelaData[i].m_szParamDesc,GETVALUE_STRING(slQry,7),sizeof(m_pMgrSrvsParamRelaData[i].m_szParamDesc)-1);
			m_pMgrSrvsParamRelaData[i].m_iParamLog =GETVALUE_INT(slQry,8);
			m_pSHMMgrSrvsParamRelaMappingIDIndex->add(m_pMgrSrvsParamRelaData[i].m_iMappingID ,i);

			if (m_pSHMMgrSrvsParamRelaindex->get(m_pMgrSrvsParamRelaData[i].m_iServicesID,&value)) {
				int pos = m_pMgrSrvsParamRelaData[value].m_iNext;
				m_pMgrSrvsParamRelaData[value].m_iNext = i;
				m_pMgrSrvsParamRelaData[i].m_iNext = pos;
			} else {
				m_pSHMMgrSrvsParamRelaindex->add (m_pMgrSrvsParamRelaData[i].m_iServicesID,i);
				m_pMgrSrvsParamRelaData[i].m_iNext = 0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrSrvsParamRelaData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadSvrsParamRelaSHM()
{
	if (m_pSHMMgrSrvsParamRelaData->exist()) {
		m_pSHMMgrSrvsParamRelaData->remove();
	}

	if (m_pSHMMgrSrvsParamRelaindex->exist()) {
		m_pSHMMgrSrvsParamRelaindex->remove();
	}

	if (m_pSHMMgrSrvsParamRelaMappingIDIndex->exist()) {
		m_pSHMMgrSrvsParamRelaMappingIDIndex->remove();
	}
	m_pSHMMgrSrvsParamRelaLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::querySvrsParamRelaData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvsParamRelaData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvsParamRelaData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvsParamRelaData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_services_param_relation(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvsParamRelaData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrSrvsParamRelaData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSrvsParamRelaData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrSrvsParamRelaData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querySvrsParamRelaIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvsParamRelaindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvsParamRelaindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvsParamRelaindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_services_param_relation(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvsParamRelaindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSrvsParamRelaindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSrvsParamRelaindex->getCount());
	return 0;
}



int SHMDataMgr::querySvrsParamRelaMappingIDIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvsParamRelaMappingIDIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvsParamRelaMappingIDIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvsParamRelaMappingIDIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_services_param_relation(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvsParamRelaMappingIDIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSrvsParamRelaMappingIDIndex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSrvsParamRelaMappingIDIndex->getCount());
	return 0;
}



//创建共享内存数据区和索引区
int SHMDataMgr::createSvrsParamListSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SVRSPARAMLIST_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SVRSPARAMLIST_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SVRSPARAMLIST_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrSrvsParamListData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_services_param_list请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrSrvsParamListData->create(_lMaxCount);
			m_pMgrSrvsParamListData = (SgwServicesParamList *)(*m_pSHMMgrSrvsParamListData);

			if (!m_pSHMMgrSrvsParamListindex->exist()) {
				m_pSHMMgrSrvsParamListindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_services_param_list)共享内存索引区B已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrSrvsParamListData->create(_lCount);
			m_pMgrSrvsParamListData = (SgwServicesParamList *)(*m_pSHMMgrSrvsParamListData);

			if (!m_pSHMMgrSrvsParamListindex->exist()) {
				m_pSHMMgrSrvsParamListindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_services_param_list)共享内存索引区已经存在"<<endl;
			}
		}
	} else {
		cout<<"(sgw_services_param_list)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrSrvsParamListindex->exist()) {
			m_pSHMMgrSrvsParamListindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_services_param_list)共享内存索引区已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadSvrsParamListData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);

	snprintf (sSQL,sizeof(sSQL)-1,"SELECT PARAM_ID,PARAM_NAME,PARAM_VAL,PARAM_TYPE, \
								  PARAM_DESC FROM SGW_SERVICES_PARAM_LIST");


	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrSrvsParamListData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息sgw_services_param_list共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrSrvsParamListData[i].m_iParamID =GETVALUE_INT(slQry,0);
			strncpy(m_pMgrSrvsParamListData[i].m_szParamName,GETVALUE_STRING(slQry,1),sizeof(m_pMgrSrvsParamListData[i].m_szParamName)-1);
			m_pMgrSrvsParamListData[i].m_iParamVal =GETVALUE_INT(slQry,2);
			m_pMgrSrvsParamListData[i].m_iParamType =GETVALUE_INT(slQry,3);
			strncpy(m_pMgrSrvsParamListData[i].m_szParamDesc,GETVALUE_STRING(slQry,4),sizeof(m_pMgrSrvsParamListData[i].m_szParamDesc)-1);
			m_pSHMMgrSrvsParamListindex->add (m_pMgrSrvsParamListData[i].m_iParamID,i);
		}
	}
	slQry.close();
	if (m_pSHMMgrSrvsParamListData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadSvrsParamListSHM()
{
	if (m_pSHMMgrSrvsParamListData->exist()) {
		m_pSHMMgrSrvsParamListData->remove();
	}

	if (m_pSHMMgrSrvsParamListindex->exist()) {
		m_pSHMMgrSrvsParamListindex->remove();
	}
	m_pSHMMgrSrvsParamListLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::querySvrsParamListData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvsParamListData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvsParamListData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvsParamListData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_services_param_list(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvsParamListData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrSrvsParamListData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSrvsParamListData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrSrvsParamListData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querySvrsParamListIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvsParamListindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvsParamListindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvsParamListindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_services_param_list(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvsParamListindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSrvsParamListindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSrvsParamListindex->getCount());
	return 0;
}




//创建共享内存数据区和索引区
int SHMDataMgr::createSvrsRegisterSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SVRSREGISTER_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SVRSREGISTER_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SVRSREGISTER_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrSrvsRegisterData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_service_register请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrSrvsRegisterData->create(_lMaxCount);
			m_pMgrSrvsRegisterData = (SgwServiceRegister *)(*m_pSHMMgrSrvsRegisterData);

			if (!m_pSHMMgrSrvsRegisterindex->exist()) {
				m_pSHMMgrSrvsRegisterindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_service_register)共享内存索引区A已经存在"<<endl;
			}

			if (!m_pSHMMgrSrvsRegisterSvrInsIDIndex->exist()) {
				m_pSHMMgrSrvsRegisterSvrInsIDIndex->create(_lMaxCount);
			}else {
				cout<<"(sgw_service_register)共享内存索引区B已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrSrvsRegisterData->create(_lCount);
			m_pMgrSrvsRegisterData = (SgwServiceRegister *)(*m_pSHMMgrSrvsRegisterData);

			if (!m_pSHMMgrSrvsRegisterindex->exist()) {
				m_pSHMMgrSrvsRegisterindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_service_register)共享内存索引区A已经存在"<<endl;
			}
			if (!m_pSHMMgrSrvsRegisterSvrInsIDIndex->exist()) {
				m_pSHMMgrSrvsRegisterSvrInsIDIndex->create(_lMaxCount);
			}else {
				cout<<"(sgw_service_register)共享内存索引区B已经存在"<<endl;
			}
		}
	} else {
		cout<<"(sgw_service_register)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrSrvsRegisterindex->exist()) {
			m_pSHMMgrSrvsRegisterindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_service_register)共享内存索引区A已经存在"<<endl;
		}

		if (!m_pSHMMgrSrvsRegisterSvrInsIDIndex->exist()) {
			m_pSHMMgrSrvsRegisterSvrInsIDIndex->create(_lMaxCount);
		}else {
			cout<<"(sgw_service_register)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadSvrsRegisterData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);

	snprintf (sSQL,sizeof(sSQL)-1,"SELECT SERVICES_INSTANCE_ID,CHANNEL_ID, \
								  to_char(STATE_DATE,'YYYYMMDDHH24MISS'), \
								  SRC_SERVICES_ID,SRC_SERVICES_NAME, \
								  MAP_SERVICES_ID,MAP_SERVICES_NAME,\
								  AREA_USER,STATE FROM SGW_SERVICE_REGISTER");


	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrSrvsRegisterData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息sgw_service_register共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrSrvsRegisterData[i].m_iServicesInstanceID =GETVALUE_INT(slQry,0);
			strncpy(m_pMgrSrvsRegisterData[i].m_szChannelID,GETVALUE_STRING(slQry,1),sizeof(m_pMgrSrvsRegisterData[i].m_szChannelID)-1);
			strncpy(m_pMgrSrvsRegisterData[i].m_szStateDate,GETVALUE_STRING(slQry,2),sizeof(m_pMgrSrvsRegisterData[i].m_szStateDate)-1);
			m_pMgrSrvsRegisterData[i].m_iSrcServicesID =GETVALUE_INT(slQry,3);
			strncpy(m_pMgrSrvsRegisterData[i].m_szSrcServicesName,GETVALUE_STRING(slQry,4),sizeof(m_pMgrSrvsRegisterData[i].m_szSrcServicesName)-1);
			m_pMgrSrvsRegisterData[i].m_iMapServicesID = GETVALUE_INT(slQry,5);
			strncpy(m_pMgrSrvsRegisterData[i].m_szMapServicesName,GETVALUE_STRING(slQry,6),sizeof(m_pMgrSrvsRegisterData[i].m_szMapServicesName)-1);
			m_pMgrSrvsRegisterData[i].m_iAreaUser = GETVALUE_INT(slQry,7);
			strncpy(m_pMgrSrvsRegisterData[i].m_szState,GETVALUE_STRING(slQry,8),sizeof(m_pMgrSrvsRegisterData[i].m_szState)-1);

			m_pSHMMgrSrvsRegisterSvrInsIDIndex->add(m_pMgrSrvsRegisterData[i].m_iServicesInstanceID,i);

			lkey = SHMDataMgr::changekey(m_pMgrSrvsRegisterData[i].m_szChannelID,m_pSHMMgrSrvsRegisterData->getTotal());

			if (m_pSHMMgrSrvsRegisterindex->get(lkey,&value)) {
				int pos = m_pMgrSrvsRegisterData[value].m_iNext;
				m_pMgrSrvsRegisterData[value].m_iNext = i;
				m_pMgrSrvsRegisterData[i].m_iNext = pos;
			} else {
				m_pSHMMgrSrvsRegisterindex->add (lkey,i);
				m_pMgrSrvsRegisterData[i].m_iNext = 0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrSrvsRegisterData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadSvrsRegisterSHM()
{
	if (m_pSHMMgrSrvsRegisterData->exist()) {
		m_pSHMMgrSrvsRegisterData->remove();
	}

	if (m_pSHMMgrSrvsRegisterindex->exist()) {
		m_pSHMMgrSrvsRegisterindex->remove();
	}

	if (m_pSHMMgrSrvsRegisterSvrInsIDIndex->exist()) {
		m_pSHMMgrSrvsRegisterSvrInsIDIndex->remove();
	}
	m_pSHMMgrSrvsRegisterLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::querySvrsRegisterData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvsRegisterData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvsRegisterData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvsRegisterData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_service_register(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvsRegisterData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrSrvsRegisterData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSrvsRegisterData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrSrvsRegisterData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querySvrsRegisterIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvsRegisterindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvsRegisterindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvsRegisterindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_service_register(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvsRegisterindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSrvsRegisterindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSrvsRegisterindex->getCount());
	return 0;
}




int SHMDataMgr::querySvrsRegisterInsIDIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSrvsRegisterSvrInsIDIndex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSrvsRegisterSvrInsIDIndex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSrvsRegisterSvrInsIDIndex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_service_register(file)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSrvsRegisterSvrInsIDIndex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSrvsRegisterSvrInsIDIndex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSrvsRegisterSvrInsIDIndex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createParamInsListSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_PARAMINSLIST_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_PARAMINSLIST_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_PARAMINSLIST_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrParamInsListData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_param_instance_list请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrParamInsListData->create(_lMaxCount);
			m_pMgrParamInsListData = (SgwParamInstanceList *)(*m_pSHMMgrParamInsListData);

			if (!m_pSHMMgrParamInsListindex->exist()) {
				m_pSHMMgrParamInsListindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_param_instance_list)共享内存索引区A已经存在"<<endl;
			}

			if (!m_pSHMMgrParamInsListMapIDindex->exist()) {
				m_pSHMMgrParamInsListMapIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_param_instance_list)共享内存索引区B已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrParamInsListData->create(_lCount);
			m_pMgrParamInsListData = (SgwParamInstanceList *)(*m_pSHMMgrParamInsListData);

			if (!m_pSHMMgrParamInsListindex->exist()) {
				m_pSHMMgrParamInsListindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_param_instance_list)共享内存索引区A已经存在"<<endl;
			}

			if (!m_pSHMMgrParamInsListMapIDindex->exist()) {
				m_pSHMMgrParamInsListMapIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_param_instance_list)共享内存索引区B已经存在"<<endl;
			}
		}
	} else {
		cout<<"(sgw_param_instance_list)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrParamInsListindex->exist()) {
			m_pSHMMgrParamInsListindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_param_instance_list)共享内存索引区A已经存在"<<endl;
		}

		if (!m_pSHMMgrParamInsListMapIDindex->exist()) {
			m_pSHMMgrParamInsListMapIDindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_param_instance_list)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadParamInsListData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);

	snprintf (sSQL,sizeof(sSQL)-1,"SELECT SERVICES_INSTANCE_ID,SRC_PARAM_ID, \
								  SRC_PARAM_NAME, \
								  SRC_PARAM_TYPE,SRC_PARAM_VAL, \
								  DEST_PARAM_ID,DEST_PARAM_NAME,\
								  DEST_PARAM_TYPE,DEST_PARAM_VAL, \
								  SRC_PARAM_DESC,DEST_PARAM_DESC, CHANGE_ID, \
								  PARAM_FLAG,PARAM_LOG\
								  FROM SGW_PARAM_INSTANCE_LIST");


	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrParamInsListData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息sgw_service_register共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrParamInsListData[i].m_iMappingID = GETVALUE_INT(slQry,11);
			m_pMgrParamInsListData[i].m_iServicesInstanceID =GETVALUE_INT(slQry,0);
			m_pMgrParamInsListData[i].m_iSrcParamID =GETVALUE_INT(slQry,1);
			strncpy(m_pMgrParamInsListData[i].m_szSrcParamName,GETVALUE_STRING(slQry,2),sizeof(m_pMgrParamInsListData[i].m_szSrcParamName)-1);
			m_pMgrParamInsListData[i].m_iSrcParamType =GETVALUE_INT(slQry,3);
			m_pMgrParamInsListData[i].m_iSrcParamVal =GETVALUE_INT(slQry,4);
			m_pMgrParamInsListData[i].m_iDestParamID =GETVALUE_INT(slQry,5);
			strncpy(m_pMgrParamInsListData[i].m_szDestParamName,GETVALUE_STRING(slQry,6),sizeof(m_pMgrParamInsListData[i].m_szDestParamName)-1);
			m_pMgrParamInsListData[i].m_iDestParamType =GETVALUE_INT(slQry,7);
			m_pMgrParamInsListData[i].m_iDestParamVal =GETVALUE_INT(slQry,8);
			strncpy(m_pMgrParamInsListData[i].m_szSrcParamDesc,GETVALUE_STRING(slQry,9),sizeof(m_pMgrParamInsListData[i].m_szSrcParamDesc)-1);
			strncpy(m_pMgrParamInsListData[i].m_szDestParamDesc,GETVALUE_STRING(slQry,10),sizeof(m_pMgrParamInsListData[i].m_szDestParamDesc)-1);
			m_pMgrParamInsListData[i].m_iParamFlag =GETVALUE_INT(slQry,12);
			m_pMgrParamInsListData[i].m_iParamLog =GETVALUE_INT(slQry,13);
			m_pSHMMgrParamInsListMapIDindex->add(m_pMgrParamInsListData[i].m_iMappingID ,i);

			if (m_pSHMMgrParamInsListindex->get(m_pMgrParamInsListData[i].m_iServicesInstanceID,&value)) {
				int pos = m_pMgrParamInsListData[value].m_iNext;
				m_pMgrParamInsListData[value].m_iNext = i;
				m_pMgrParamInsListData[i].m_iNext = pos;
			} else {
				m_pSHMMgrParamInsListindex->add (m_pMgrParamInsListData[i].m_iServicesInstanceID,i);
				m_pMgrParamInsListData[i].m_iNext = 0;
			}
		}
	}
	slQry.close();
	if (m_pSHMMgrParamInsListData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadParamInsListSHM()
{
	if (m_pSHMMgrParamInsListData->exist()) {
		m_pSHMMgrParamInsListData->remove();
	}

	if (m_pSHMMgrParamInsListindex->exist()) {
		m_pSHMMgrParamInsListindex->remove();
	}

	if (m_pSHMMgrParamInsListMapIDindex->exist()) {
		m_pSHMMgrParamInsListMapIDindex->remove();
	}
	m_pSHMMgrParamInsListLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::queryParamInsListData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrParamInsListData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrParamInsListData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrParamInsListData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_param_instance_list(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrParamInsListData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrParamInsListData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrParamInsListData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrParamInsListData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::queryParamInsListIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrParamInsListindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrParamInsListindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrParamInsListindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_param_instance_list(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrParamInsListindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrParamInsListindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrParamInsListindex->getCount());
	return 0;
}

int SHMDataMgr::queryParamInsListMapIDIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrParamInsListMapIDindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrParamInsListMapIDindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrParamInsListMapIDindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_param_instance_list(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrParamInsListMapIDindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrParamInsListMapIDindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrParamInsListMapIDindex->getCount());
	return 0;
}

//创建共享内存数据区和索引区
int SHMDataMgr::createSgwEnumAreaSHM(int _iExtenSzie)
{
	long _lCount =  GETVALUE(_SHM_SGWENUMAREA_SPACE_INITSIZE_,_SHM_PARAM_DATA_INITSIZE_)
		+_iExtenSzie*GETVALUE(_SHM_SGWENUMAREA_EXTENTS_SIZE_,_SHM_PARAM_EXTENTS_SIZE_);//求出当前创建的内存大小
	long _lMaxCount = GETVALUE(_SHM_SGWENUMAREA_SPACE_MAXSIZE_,_SHM_PARAM_DATA_MAXSIZE_);//总得可以创建的大小

	if (!m_pSHMMgrSgwEnumAreaData->exist()) {

		if (_lCount>_lMaxCount) {
			cout<<"警告！欲分配的共享内存空间大小超过最大值(分配的空间取可分配最大值)……"<<endl;

			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,2,-1,NULL,"创建共享内存sgw_enum_area请求分配的共享空间是%d,超过了可分配空间大小%d",_lCount,_lMaxCount);

			sleep(3);
			m_pSHMMgrSgwEnumAreaData->create(_lMaxCount);
			m_pMgrSgwEnumAreaData = (SgwEnumArea *)(*m_pSHMMgrSgwEnumAreaData);

			if (!m_pSHMMgrSgwEnumAreaindex->exist()) {
				m_pSHMMgrSgwEnumAreaindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_enum_area)共享内存索引区A已经存在"<<endl;
			}

			if (!m_pSHMMgrSgwEnumAreaOrgIDindex->exist()) {
				m_pSHMMgrSgwEnumAreaOrgIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_enum_area)共享内存索引区B已经存在"<<endl;
			}
			return _lMaxCount;
		} else {
			m_pSHMMgrSgwEnumAreaData->create(_lCount);
			m_pMgrSgwEnumAreaData = (SgwEnumArea *)(*m_pSHMMgrSgwEnumAreaData);

			if (!m_pSHMMgrSgwEnumAreaindex->exist()) {
				m_pSHMMgrSgwEnumAreaindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_enum_area)共享内存索引区A已经存在"<<endl;
			}

			if (!m_pSHMMgrSgwEnumAreaOrgIDindex->exist()) {
				m_pSHMMgrSgwEnumAreaOrgIDindex->create(_lMaxCount);
			}else {
				cout<<"(sgw_enum_area)共享内存索引区B已经存在"<<endl;
			}
		}
	} else {
		cout<<"(sgw_enum_area)共享内存数据区已经存在"<<endl;

		if (!m_pSHMMgrSgwEnumAreaindex->exist()) {
			m_pSHMMgrSgwEnumAreaindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_enum_area)共享内存索引区A已经存在"<<endl;
		}

		if (!m_pSHMMgrSgwEnumAreaOrgIDindex->exist()) {
			m_pSHMMgrSgwEnumAreaOrgIDindex->create(_lMaxCount);
		}else {
			cout<<"(sgw_enum_area)共享内存索引区B已经存在"<<endl;
		}
		return 0;
	}
	return _lCount;
}

int SHMDataMgr::loadSgwEnumAreaData()
{
	char sSQL[BUFF_MAX_SIZE] ={0};
	long lkey = 0;
	unsigned int value = 0;
	DEFINE_SGW_QUERY(slQry);

	snprintf (sSQL,sizeof(sSQL)-1,"SELECT AREA_SEQ, ORG_ID, AREA_CODE, AREA_NAME FROM SGW_ENUM_AREA");


	slQry.setSQL(sSQL);
	slQry.open();

	while (slQry.next()) {
		int i = m_pSHMMgrSgwEnumAreaData->malloc();

		if (i == 0) {
			ThreeLogGroup _oLogObj;
			_oLogObj.sgwlog(MBC_MEM_SPACE_SMALL,-1,-1,-1,3,3,-1,NULL,"全量加载文件信息sgw_enum_area共享内存空间不足");
			return MBC_MEM_SPACE_SMALL;
		} else {
			m_pMgrSgwEnumAreaData[i].m_iAreaSeq = GETVALUE_INT(slQry,0);
			m_pMgrSgwEnumAreaData[i].m_iOrgID =GETVALUE_INT(slQry,1);
			strncpy(m_pMgrSgwEnumAreaData[i].m_szAreaCode,GETVALUE_STRING(slQry,2),sizeof(m_pMgrSgwEnumAreaData[i].m_szAreaCode)-1);
			strncpy(m_pMgrSgwEnumAreaData[i].m_szAreaName,GETVALUE_STRING(slQry,3),sizeof(m_pMgrSgwEnumAreaData[i].m_szAreaName)-1);

			m_pSHMMgrSgwEnumAreaindex->add(m_pMgrSgwEnumAreaData[i].m_iAreaSeq,i);
			m_pSHMMgrSgwEnumAreaOrgIDindex->add(m_pMgrSgwEnumAreaData[i].m_iOrgID,i);
		}
	}
	slQry.close();
	if (m_pSHMMgrSgwEnumAreaData->getCount() == 0 ) {
		//告警
		return MBC_SHM_LOAD_NO_DATA;
	}
	return 0;
}

//卸载共享内存数据区和索引区
void SHMDataMgr::unloadSgwEnumAreaSHM()
{
	if (m_pSHMMgrSgwEnumAreaData->exist()) {
		m_pSHMMgrSgwEnumAreaData->remove();
	}

	if (m_pSHMMgrSgwEnumAreaindex->exist()) {
		m_pSHMMgrSgwEnumAreaindex->remove();
	}

	if (m_pSHMMgrSgwEnumAreaOrgIDindex->exist()) {
		m_pSHMMgrSgwEnumAreaOrgIDindex->remove();
	}
	m_pSHMMgrSgwEnumAreaLock->close();
}

//查询共享内存数据区信息
int SHMDataMgr::querySgwEnumAreaData(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSgwEnumAreaData->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSgwEnumAreaData->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"数据区");
	_oShmInfo.m_iUsed = m_pSHMMgrSgwEnumAreaData->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_enum_area(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSgwEnumAreaData->connnum()-1;
	_oShmInfo.m_iTotal = m_pSHMMgrSgwEnumAreaData->getTotal()+1;

	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSgwEnumAreaData->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d", m_pSHMMgrSgwEnumAreaData->getCount());
	return 0;
}

//查询共享内存索引区信息
int SHMDataMgr::querySgwEnumAreaIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSgwEnumAreaindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSgwEnumAreaindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSgwEnumAreaindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_enum_area(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSgwEnumAreaindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSgwEnumAreaindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSgwEnumAreaindex->getCount());
	return 0;
}

int SHMDataMgr::querySgwEnumAreaOrgIDIndex(SHMInfo &_oShmInfo)
{
	memset(&_oShmInfo,0,sizeof(_oShmInfo));

	_oShmInfo.m_lKey = m_pSHMMgrSgwEnumAreaOrgIDindex->getKey();
	snprintf(_oShmInfo.m_sKey,sizeof(_oShmInfo.m_sKey)-1,"0x%8.8x",_oShmInfo.m_lKey);
	_oShmInfo.m_iAllSize = m_pSHMMgrSgwEnumAreaOrgIDindex->getMemSize();
	snprintf(_oShmInfo.m_sIPCType,sizeof(_oShmInfo.m_sIPCType)-1,"索引区");
	_oShmInfo.m_iUsed = m_pSHMMgrSgwEnumAreaOrgIDindex->getUsedSize();
	float _fUseRate = (float)(100)*((float)(_oShmInfo.m_iUsed)/(float)(_oShmInfo.m_iAllSize)) ;
	snprintf(_oShmInfo.m_sUseRate,sizeof(_oShmInfo.m_sUseRate)-1,"%2.2f%%",_fUseRate);
	snprintf(_oShmInfo.m_sDataFrom,sizeof(_oShmInfo.m_sDataFrom)-1,"sgw_enum_area(table)");
	snprintf(_oShmInfo.m_sOwner,sizeof(_oShmInfo.m_sOwner)-1,"%s",getenv("USER"));
	_oShmInfo.m_iNattch = m_pSHMMgrSgwEnumAreaOrgIDindex->connnum()-1;
	struct tm *p = NULL;
	time_t timep = m_pSHMMgrSgwEnumAreaOrgIDindex->getLoadTime();
	p =gmtime(&timep);
	sprintf(_oShmInfo.m_sLoadTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",(p->tm_year+1900),(p->tm_mon+1),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	snprintf(_oShmInfo.m_sDataNbr,sizeof(_oShmInfo.m_sDataNbr)-1,"%d",m_pSHMMgrSgwEnumAreaOrgIDindex->getCount());
	return 0;
}
