/*ver:1*/

#include "dcc_FileMgr.h"
#include "Date.h"
#include "sendDcrFromFile.h"
#include "ParamDefineMgr.h"
#include "Log.h"

FileToDccMgr *FileToDccMgr::m_gpDccNgr = 0;

FileToDccMgr::FileToDccMgr()
{
	m_sSendString=string("");
	m_oSelfReceiver = NULL;
	m_oSelfSender = NULL;
	m_sSendBuffer = NULL;
	m_sReceiveBuffer = NULL;
	m_mapFileStore.clear();
	m_mapFileTypeInfo.clear();
	m_mapServiceContext.clear();
}

FileToDccMgr::~FileToDccMgr()
{
	freeAllFileType();
}

void FileToDccMgr::freeAllFileType()
{
	map<int,DccFileInfo *>::iterator iter;
	for (iter=m_mapFileTypeInfo.begin();iter!=m_mapFileTypeInfo.end();iter++)
	{
		DccFileInfo *pTmp = iter->second;
		if (pTmp)
		{
			delete pTmp;
			pTmp = NULL;
		}
	}

	m_mapFileTypeInfo.clear();
}

FileToDccMgr *FileToDccMgr::getInstance()
{
	if (m_gpDccNgr)
		return m_gpDccNgr;

	FileToDccMgr::m_gpDccNgr = new FileToDccMgr();	
	return m_gpDccNgr;
}

int FileToDccMgr::init(char *sFileTypes)
{
	char sSql[1024]={0};
	char sWhere[128]={0},sSelFileType[60]={0};
	char sFileBakDir[128]={0};

	if (sFileTypes)
		strcpy(sSelFileType,sFileTypes);
	else
		strcpy(sSelFileType,"NULL");

	if (!ParamDefineMgr::getParam("DCC", "FILE_BAK_DIR", sFileBakDir))
	{
		char sInfo[100]={0};
		strcpy(sInfo,"请在b_param_define表中配置文件备份目录参数!");
		Log::log(0,sInfo);
		throw(sInfo);
	}
	
	TOCIQuery  qry(Environment::getDBConn());
	qry.close();
	sprintf(sSql,"select trans_file_type_id,file_path from trans_file_store_config where 1=1 OR trans_file_type_id in (%s) ",sSelFileType);
	qry.setSQL(sSql);
	qry.open();
	while(qry.next())
	{
		m_mapFileStore[qry.field(0).asInteger()] = qry.field(1).asString();
	}
	qry.close();

	sprintf(sSql," SELECT a.trans_file_type_id,a.service_context,to_char(NVL(b.break_file_date,SYSDATE),'YYYYMMDDHH24MISS') BREAK_FILE_DATE,"
			"              nvl(b.break_file_name,'000'),nvl(b.break_file_line,0),nvl(b.state,'ENA'),FILE_PATERN "
			" FROM  TRANS_FILE_SERVICE_CONTEXT a,TRANS_DCC_BREAK_REC b "
			" WHERE a.trans_file_type_id=b.trans_file_type_id(+) "
			"       AND (1=1 OR a.trans_file_type_id IN (%s) ) order by a.trans_file_type_id ",sSelFileType);
	qry.setSQL(sSql);
	qry.open();
	while(qry.next())
	{
		DccFileInfo *poFileInfo = new DccFileInfo();
		poFileInfo->m_iFileTypeID = qry.field(0).asInteger();
		strcpy(poFileInfo->m_sServiceContext,qry.field(1).asString());
		strcpy(poFileInfo->m_sBreakFileDate,qry.field(2).asString());
		strcpy(poFileInfo->m_sBreakFile,qry.field(3).asString());
		poFileInfo->m_iBreakLine = qry.field(4).asInteger();
		strcpy(poFileInfo->m_sFileTypeState,qry.field(5).asString());
		strcpy(poFileInfo->m_sFilePatern,qry.field(6).asString());
		m_mapFileTypeInfo[poFileInfo->m_iFileTypeID] = poFileInfo;
		m_mapServiceContext[poFileInfo->m_sServiceContext] = poFileInfo->m_iFileTypeID;
		char sStoreDir[128]={0};
		strcpy(sStoreDir,m_mapFileStore[poFileInfo->m_iFileTypeID].c_str());
		poFileInfo->m_pStoreDir = new Directory(sStoreDir);
		poFileInfo->m_pBakDir = new Directory(sFileBakDir);
	}
	qry.close();

	//获取最大的发送和接收buffer大小

	sprintf(sSql,"SELECT min(a.msg_event_num) FROM wf_mq_attr a,wf_process_mq b,"
			"       (SELECT out_process_id FROM wf_distribute_rule ) c "
			"WHERE  a.mq_id=b.mq_id "
			"       AND b.process_id=c.out_process_id "
			"       AND b.process_id=%d ",Process::m_iProcID);
	qry.setSQL(sSql);
	qry.open();
	if (qry.next())
	{
		int iEventNum=qry.field(0).asInteger(); 
		m_sSendBuffer = new char[sizeof(StdEvent)*iEventNum+1];
		m_sReceiveBuffer = new char[sizeof(StdEvent)*iEventNum+1];
	}
	else
	{
		//默认大小为20个stdevent结构大小
		m_sSendBuffer = new char[sizeof(StdEvent)*20+1];
		m_sReceiveBuffer = new char[sizeof(StdEvent)*20+1];
	}
	qry.close();
	return 1;
}

void FileToDccMgr::setQueueTool(EventSender * poSender, EventReceiver * poReceiver)
{
	m_oSelfReceiver = poReceiver;
	m_oSelfSender = poSender;
}

string FileToDccMgr::getFilePath(int iFileType)
{
	return m_mapFileStore[iFileType];
}

string FileToDccMgr::getServiceContextByFileType(int iFileType)
{
	DccFileInfo *pFileInfo  = m_mapFileTypeInfo[iFileType];

	return string(pFileInfo->m_sServiceContext);
}

int FileToDccMgr::getFileTypeByServiceContext(char * sServiceContext)
{
	if (!sServiceContext)
		return 0;

	string sService_Context=string(sServiceContext);

	map<string,int>::iterator itr = m_mapServiceContext.find(sService_Context);
	if (itr==m_mapServiceContext.end())
		return 0;
	else
		return itr->second;
	
}


/*从各类文件中读取信息*/
int FileToDccMgr::formatDcrInfo()
{
	DccFileFieldInfo *pFieldInfo;
	
	map<int,DccFileInfo*>::iterator iter;
	for (iter=m_mapFileTypeInfo.begin();iter!=m_mapFileTypeInfo.end();iter++)
	{
		int iFileTypeID=(*iter).first;
		DccFileInfo *poFileInfo = (*iter).second;

		File *pSrcFile = 0;
		if(poFileInfo->getFile(pSrcFile))
			continue;
		if ((pFieldInfo=poFileInfo->getNextRecord()) == NULL)  //未取到数据
			continue;

		if (pFieldInfo->m_iFieldType!=FILE_BODY_TYPE)  //头尾不处理
			continue;

		string sCCRString=string("");
		pFieldInfo->genSendCCRString(sCCRString,poFileInfo->getServiceContext());
		//send ccr的消息字符串
		m_oSelfSender->send(m_sSendBuffer);
		//recieve cca的消息字符串，如果收到为错或者超时未收到cca消息
		//	设置该文件类型的状态为DIA，且当前记录序列－1
	}
	return 1;
}


