/*VER: 1*/

#include <stdlib.h>
#include "Environment.h"
#include "DataEventMem.h"

bool DataEventMemBase::m_bAttached = false;
//宽带e8及商务领航数据及索引
USERINFO_SHM_DATA_TYPE<DataE8AccountInfo> * DataEventMemBase::m_poE8AccountData = 0 ;
DataE8AccountInfo * DataEventMemBase::m_poE8AccountIns = 0;
USERINFO_SHM_STR_INDEX_TYPE * DataEventMemBase::m_poE8AccountIndex = 0;	

//宽带话单拆分合并
//第一组
USERINFO_SHM_DATA_TYPE<DataEventDiv> * DataEventMemBase::m_poDataEventDivData11 = 0;
DataEventDiv * DataEventMemBase::m_poDataEventDivIns11 = 0;
USERINFO_SHM_INT_INDEX_TYPE * DataEventMemBase::m_poDataEventDivIndex11 = 0;
USERINFO_SHM_DATA_TYPE<DataEventDiv> * DataEventMemBase::m_poDataEventDivData12 = 0;
DataEventDiv * DataEventMemBase::m_poDataEventDivIns12 = 0 ;
USERINFO_SHM_INT_INDEX_TYPE * DataEventMemBase::m_poDataEventDivIndex12 = 0;

//第二组
USERINFO_SHM_DATA_TYPE<DataEventDiv> * DataEventMemBase::m_poDataEventDivData21 = 0;
DataEventDiv * DataEventMemBase::m_poDataEventDivIns21 = 0;
USERINFO_SHM_INT_INDEX_TYPE * DataEventMemBase::m_poDataEventDivIndex21 = 0;
USERINFO_SHM_DATA_TYPE<DataEventDiv> * DataEventMemBase::m_poDataEventDivData22 = 0;
DataEventDiv * DataEventMemBase::m_poDataEventDivIns22 = 0 ;
USERINFO_SHM_INT_INDEX_TYPE * DataEventMemBase::m_poDataEventDivIndex22 = 0;

//第三组
USERINFO_SHM_DATA_TYPE<DataEventDiv> * DataEventMemBase::m_poDataEventDivData31 = 0;
DataEventDiv * DataEventMemBase::m_poDataEventDivIns31 = 0;
USERINFO_SHM_INT_INDEX_TYPE * DataEventMemBase::m_poDataEventDivIndex31 = 0;
USERINFO_SHM_DATA_TYPE<DataEventDiv> * DataEventMemBase::m_poDataEventDivData32 = 0;
DataEventDiv * DataEventMemBase::m_poDataEventDivIns32 = 0 ;
USERINFO_SHM_INT_INDEX_TYPE * DataEventMemBase::m_poDataEventDivIndex32 = 0;

//4008/800等ext_serv的信息
USERINFO_SHM_DATA_TYPE<ExtServ> *DataEventMemBase::m_poExtServData = 0;
ExtServ * DataEventMemBase::m_poExtServIns = 0;
USERINFO_SHM_STR_INDEX_TYPE * DataEventMemBase::m_poExtServIndex = 0;

DataEventMemBase::DataEventMemBase()
{
	m_bAttached = true;
}

DataEventMemBase::DataEventMemBase(int iGroup)
//DataEventMemBase::DataEventMemBase()
{
	bool bNowAttached=false;

	m_iGroupID = iGroup;

	/*if (m_bAttached) 
		return;
*/
	m_bAttached = true;

	//数据业务e8及商务领航帐号	
	if (m_iGroupID == E8_USER_MEM_GRP)
	{		

/*		if (!m_poE8AccountData)
			{
		m_poE8AccountData = new USERINFO_SHM_DATA_TYPE<DataE8AccountInfo>(120101);
		if (!m_poE8AccountData)
			{freeAll();
			THROW(MBC_UserInfo+1);
			}
		if (!m_poE8AccountData->exist())
			{
				m_bAttached = false;
			}
			}


		if (!m_poE8AccountIndex)
			{
		m_poE8AccountIndex = new USERINFO_SHM_STR_INDEX_TYPE(120102);
		if (!m_poE8AccountIndex)
			{
			freeAll();
			THROW(MBC_UserInfo+1);
			}

		if (!m_poE8AccountIndex->exist())
			{
				m_bAttached = false;
			}
			}
	*/
		

		if (!m_poE8AccountData)
			USERINFO_ATTACH_DATA(m_poE8AccountData,DataE8AccountInfo,DATA_E8_ACCOUNT_DATA);	
		if (!m_poE8AccountIndex)
			USERINFO_ATTACH_STRING_INDEX(m_poE8AccountIndex,DATA_E8_ACCOUNT_INDEX);
		
	}
	
	//宽带话单拆分
	//第一组
	if (m_iGroupID == E8_DIV_MEM_GRP_HEAD+0)
	{

		if (!m_poDataEventDivData11)
			USERINFO_ATTACH_DATA(m_poDataEventDivData11,DataEventDiv,DATA_EVENT_DIV_DATA11);	
		if (!m_poDataEventDivIndex11)
			USERINFO_ATTACH_INT_INDEX(m_poDataEventDivIndex11,DATA_EVENT_DIV_INDEX11);		
		if (!m_poDataEventDivData12)
			USERINFO_ATTACH_DATA(m_poDataEventDivData12,DataEventDiv,DATA_EVENT_DIV_DATA12);
		if (!m_poDataEventDivIndex12)
			USERINFO_ATTACH_INT_INDEX(m_poDataEventDivIndex12,DATA_EVENT_DIV_INDEX12);		
	}
	
	if (m_iGroupID == E8_DIV_MEM_GRP_HEAD+1)
	{
		if (!m_poDataEventDivData21)
			USERINFO_ATTACH_DATA(m_poDataEventDivData21,DataEventDiv,DATA_EVENT_DIV_DATA21);	
		if (!m_poDataEventDivIndex21)
			USERINFO_ATTACH_INT_INDEX(m_poDataEventDivIndex21,DATA_EVENT_DIV_INDEX21);	
		if (!m_poDataEventDivData22)
			USERINFO_ATTACH_DATA(m_poDataEventDivData22,DataEventDiv,DATA_EVENT_DIV_DATA22);
		if (!m_poDataEventDivIndex22)
			USERINFO_ATTACH_INT_INDEX(m_poDataEventDivIndex22,DATA_EVENT_DIV_INDEX22);		
	}

	if (m_iGroupID == E8_DIV_MEM_GRP_HEAD+2)
	{
		if (!m_poDataEventDivData31)
			USERINFO_ATTACH_DATA(m_poDataEventDivData31,DataEventDiv,DATA_EVENT_DIV_DATA31);
		if (!m_poDataEventDivIndex31)
			USERINFO_ATTACH_INT_INDEX(m_poDataEventDivIndex31,DATA_EVENT_DIV_INDEX31);	
		if (!m_poDataEventDivData32)
			USERINFO_ATTACH_DATA(m_poDataEventDivData32,DataEventDiv,DATA_EVENT_DIV_DATA32);	
		if (!m_poDataEventDivIndex32)
			USERINFO_ATTACH_INT_INDEX(m_poDataEventDivIndex32,DATA_EVENT_DIV_INDEX32);		
	}

	//4008、800等ext_serv
	if (m_iGroupID == EXT_SERV_MEM_GRP) 
	{
		if (!m_poExtServData)
			USERINFO_ATTACH_DATA(m_poExtServData,ExtServ,EXT_SERV_DATA);
		if (!m_poExtServIndex)
			USERINFO_ATTACH_STRING_INDEX(m_poExtServIndex,EXT_SERV_INDEX);
	}
}

DataEventMemBase::~DataEventMemBase()
{

}

void DataEventMemBase::getGroupIndex()
{		
	//第一组
	if(m_iGroupID == E8_DIV_MEM_GRP_HEAD+0)
	{
		m_poData1 = m_poDataEventDivData11;
		m_poData2 = m_poDataEventDivData12;
		m_poIndex1 = m_poDataEventDivIndex11;
		m_poIndex2 = m_poDataEventDivIndex12;
		m_poDataEventDivIns11 = (DataEventDiv *)(*m_poDataEventDivData11);
		m_poDataEventDivIns12 = (DataEventDiv *)(*m_poDataEventDivData12);
		m_poIns1 = m_poDataEventDivIns11;
		m_poIns2 = m_poDataEventDivIns12;
	}
	
	//第一组
	if(m_iGroupID == E8_DIV_MEM_GRP_HEAD+1)
	{
		m_poData1 = m_poDataEventDivData21;
		m_poData2 = m_poDataEventDivData22;
		m_poIndex1 = m_poDataEventDivIndex21;
		m_poIndex2 = m_poDataEventDivIndex22;
		m_poDataEventDivIns21 = (DataEventDiv *)(*m_poDataEventDivData21);
		m_poDataEventDivIns22 = (DataEventDiv *)(*m_poDataEventDivData22);
		m_poIns1 = m_poDataEventDivIns21;
		m_poIns2 = m_poDataEventDivIns22;
	}
	
	//第一组
	if(m_iGroupID == E8_DIV_MEM_GRP_HEAD+2)
	{
		m_poData1 = m_poDataEventDivData31;
		m_poData2 = m_poDataEventDivData32;
		m_poIndex1 = m_poDataEventDivIndex31;
		m_poIndex2 = m_poDataEventDivIndex32;
		m_poDataEventDivIns31 = (DataEventDiv *)(*m_poDataEventDivData31);
		m_poDataEventDivIns32 = (DataEventDiv *)(*m_poDataEventDivData32);
		m_poIns1 = m_poDataEventDivIns31;
		m_poIns2 = m_poDataEventDivIns32;
	}	
}

void DataEventMemBase::bindData()
{
	m_poData1 = NULL;
	m_poData2 = NULL;
	m_poIndex1 = NULL;	
	m_poIndex2 = NULL;
	m_poIns1 = NULL;
	m_poIns2 = NULL;
	
	//数据e8及商务领航帐号
	if (m_iGroupID == E8_USER_MEM_GRP)
	{
		m_poE8AccountIns = (DataE8AccountInfo *)(*m_poE8AccountData);
		return;
	}
	
	//4008、800等ext_serv
	if (m_iGroupID == EXT_SERV_MEM_GRP)
	{
		m_poExtServIns = (ExtServ *)(*m_poExtServData);
		return;
	}
	
	//宽带话单拆分
	getGroupIndex();
	m_poIns1 = (DataEventDiv *)(*m_poData1);
	m_poIns2 = (DataEventDiv *)(*m_poData2);

	

/*	//第一组
	if (m_iGroupID == 1)
	{
		m_poDataEventDivIns11 = (DataEventDiv *)(*m_poDataEventDivData11);
		m_poDataEventDivIns12 = (DataEventDiv *)(*m_poDataEventDivData12);
	}
	
	//第二组
	if (m_iGroupID == 2)
	{
		m_poDataEventDivIns21 = (DataEventDiv *)(*m_poDataEventDivData21);
		m_poDataEventDivIns22 = (DataEventDiv *)(*m_poDataEventDivData22);
	}

	//第三组
	if (m_iGroupID == 3)
	{
		m_poDataEventDivIns31 = (DataEventDiv *)(*m_poDataEventDivData31);
		m_poDataEventDivIns32 = (DataEventDiv *)(*m_poDataEventDivData32);
	}
*/
}

void DataEventMemBase::insMonitorInterface(char *sInfo,char *sRcvAlarmAccNbr)
{
	char sSql[501];
	
	try
	{
		DEFINE_QUERY(qry);
		
		sprintf(sSql,"INSERT INTO ISSU_NOTE_LISTING("
			"		NOTE_ID,"
			"		WORK_ORDER_NBR,"
			"		NOTE,"
			"		OBJ_NBR,"
			"		AREA_CODE,"
			"		STAFF_ID,"
			"		SEND_TIME,"
			"		DEAL_FLAG,"
			"		DEAL_DATE,"
			"		MSGFORMAT,"
			"		MSGLEVEL,"
			"		SERVICEID,"
			"		SMGP_SEND_ID) "
			"VALUES (ISSU_NOTE_LISTING_ID_SEQ.NEXTVAL,"
			"		NULL,"
			"		:TMPINFO,"
			"		:ALARM_ACCNBR,"
			"		'025',"
			"		2,"
			"		sysdate,"
			"		0,"
			"		sysdate,"
			"		15,"
			"		1,"
			"		'025',"
			"		1)");
		
		qry.setSQL(sSql);
		qry.setParameter("TMPINFO",sInfo);
		qry.setParameter("ALARM_ACCNBR",sRcvAlarmAccNbr);
		qry.execute();
		qry.commit();
		qry.close();
	}
	catch (TOCIException &e) {
		printf("e8内存告警插入告警表出错，错误描述:%s\n,sql:%s",e.getErrMsg(),e.getErrSrc());
		throw e;
	}
}


void DataEventMemBase::freeAll()
{
	#define FREE(X) \
		if (X) { \
		delete X; \
		X = 0; \
		}

	//数据e8及商务领航帐号
	FREE(m_poE8AccountData);
	FREE(m_poE8AccountIndex);
	
	//宽带话单分拆
	getGroupIndex();
	FREE(m_poData1);
	FREE(m_poData2);
	FREE(m_poIndex1);
	FREE(m_poIndex2);

	FREE(m_poExtServData);
	FREE(m_poExtServIndex);
/*	//第一组
	FREE(m_poDataEventDivData11);
	FREE(m_poDataEventDivIndex11);
	FREE(m_poDataEventDivData12);
	FREE(m_poDataEventDivIndex12);

	//第二组
	FREE(m_poDataEventDivData21);
	FREE(m_poDataEventDivIndex21);
	FREE(m_poDataEventDivData22);
	FREE(m_poDataEventDivIndex22);

	//第三组
	FREE(m_poDataEventDivData31);
	FREE(m_poDataEventDivIndex31);
	FREE(m_poDataEventDivData32);
	FREE(m_poDataEventDivIndex32);
*/	
}


DataEventMemCtl::DataEventMemCtl():DataEventMemBase()
{
	
}

DataEventMemCtl::DataEventMemCtl(int iGroup):DataEventMemBase(iGroup)
{
	
}

DataEventMemCtl::~DataEventMemCtl()
{
	
}

bool DataEventMemCtl::exist()
{
	return m_bAttached;
}

