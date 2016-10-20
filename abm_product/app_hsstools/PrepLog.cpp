
#include "PrepLog.h"
#include <TOCIQuery.h>
#include<unistd.h>


#include "ParamDefineMgr.h"


#include "interrupt.h"

#define BEATH_UPDATE 30

PrepLog::PrepLog(void)
{
}

PrepLog::~PrepLog(void)
{
}

void PrepLog::clearFileMap()
{
	m_FileMap.clear();
}

FileInfo* PrepLog::getFileInfo(string sAreaCode,int iFlag )
{
	FileInfo* pFileInfo = NULL;
	FILE_MAP::iterator it = m_FileMap.find(sAreaCode);
	if( it != m_FileMap.end() )
		pFileInfo = it->second;
	else
	{
		if( iFlag == 1 )
		{
			pFileInfo = new FileInfo();
			m_FileMap[sAreaCode] = pFileInfo;
		}

	}
	return pFileInfo;
}

int PrepLog::writeFile(string sTmpPath,string sPath,string sAreaCode)
{
	FileInfo* pFileInfo = getFileInfo(sAreaCode);
	Date dNow;

	char cmd[1024] = {0};

	char tmp[128] = {0};
	if( pFileInfo == NULL )
		return 0;

	dNow.getCurDate();
	sprintf(tmp,"pp%s.%s.log",sAreaCode.c_str(),dNow.toString("YYYYMMDD.HHMISS"));
	pFileInfo->m_sFileName = tmp;
	pFileInfo->m_sFileName = sTmpPath + pFileInfo->m_sFileName;

	pFileInfo->m_File.open(pFileInfo->m_sFileName.c_str());

	pFileInfo->m_File << pFileInfo->m_osContent.str().c_str();

	pFileInfo->m_File.close();

	sprintf(cmd,"mv %s %s",pFileInfo->m_sFileName.c_str(),sPath.c_str());
	system(cmd);

	return pFileInfo->m_iCount;

	//pFileInfo->m_sFileName = getFileName()
}


string PrepLog::GetUpdateString(const char* sFileId,char* sState)
{

	string sCondition = sFileId;

	sCondition = sCondition.substr(0,sCondition.length() - 1);

	string sql = "update b_event_file_list set log_state='";
	sql += sState;
	sql += "' where file_id in(";
	sql += sCondition;
	sql += ")";

	return sql;
}

int PrepLog::updateState(string sAreaCode)
{
	ostringstream osFileID;
	FileInfo* pFileInfo = getFileInfo(sAreaCode);
	vector<long>::iterator it;
	string sql_txt;

	FILE_MAP::iterator itmap;

	int iCount = 0;

	TOCIQuery qry(Environment::getDBConn());
	qry.close();

	int i = 0;
	for( it = pFileInfo->m_vecFileID.begin();it != pFileInfo->m_vecFileID.end();it ++ )
	{
		osFileID << *it << ',';
		iCount ++;
		i++;
		if( i== BEATH_UPDATE )
		{
			//生成sql语句提交
			sql_txt = GetUpdateString( osFileID.str().c_str(),"END" );

			
			qry.setSQL(sql_txt.c_str());
			qry.execute();

			i = 0;
			osFileID.clear();
		}
	}

	//剩余的file_id
	if( osFileID.str().length() )
	{
		//生成sql语句提交
		sql_txt = GetUpdateString( osFileID.str().c_str(),"END" );

		qry.setSQL(sql_txt.c_str());
		qry.execute();

	}

	qry.commit();
	qry.close();
	//删除m_FileMap下的该节点
	itmap = m_FileMap.find(sAreaCode);
	if( itmap != m_FileMap.end() )
		m_FileMap.erase( itmap );

	return iCount;
}

int PrepLog::forceCommit(string sTmpPath,string sPath,int iSecond)
{
	if( iSecond == 0 )
		return 0;
	
	Date dNow;
	dNow.getCurDate();
	FILE_MAP::iterator it;
	FileInfo* pFileInfo = NULL;
	
	int iDiffSec = 0;
	for ( it = m_FileMap.begin();it != m_FileMap.end();it ++ )
	{
		pFileInfo = it->second;
		if( pFileInfo )
		{
			iDiffSec = dNow.diffSec(pFileInfo->m_dStateDate);
			if( iDiffSec >= iSecond )
			{
				writeFile( sTmpPath,sPath,it->first );
				updateState( it->first );
			}
		}
	}
	return 1;
}

void PrepLog::run()
{
	//遍历b_event_file_list
	TOCIQuery qry(Environment::getDBConn());



	ofstream file;
	ostringstream osFileId;
	string FileName;
	string TempPath;
	string AreaCode;

	string Path;
	

	int iSleep 		= 0;
	int iRows 		= 0;
	int iFileCount	= 0;
	int iSeq		= 0;
	int iForceCommit = 0;

	//读配置
	char tmp[128];
	memset( tmp,0,sizeof(tmp) );
	ParamDefineMgr::getParam("PREPLOG", "SLEEP", tmp);
	iSleep = atoi(tmp);
	memset( tmp,0,sizeof(tmp) );
	ParamDefineMgr::getParam("PREPLOG", "ROWS", tmp);
	iRows = atoi(tmp);
	memset( tmp,0,sizeof(tmp) );
	ParamDefineMgr::getParam("PREPLOG", "TEMPPATH", tmp);
	TempPath = tmp;
	memset( tmp,0,sizeof(tmp) );
	ParamDefineMgr::getParam("PREPLOG", "FILECOUNT", tmp);
	iFileCount = atoi(tmp);
	memset( tmp,0,sizeof(tmp) );
	ParamDefineMgr::getParam("PREPLOG", "FORCECOMMIT", tmp);
	iForceCommit = atoi(tmp);
	memset( tmp,0,sizeof(tmp) );
	ParamDefineMgr::getParam("PREPLOG", "PATH", tmp);
	Path = tmp;

	if( TempPath.c_str()[TempPath.length() - 1] != '/' )
		TempPath += '/';

	if( Path.c_str()[Path.length() - 1] != '/' )
		Path += '/';
		

	FileInfo* pFileInfo = NULL;
	
	//更新中间状态
	TOCIQuery qryExc(Environment::getDBConn());
	ostringstream osFileID;
	string sSqlExc;

	while(!GetInterruptFlag())
	{

		string sql = "select a.file_id, a.file_name, a.file_size, a.record_cnt,b.area_code \
						from b_event_file_list a,b_switch_info b \
						where a.state = 'END' and a.log_state = 'RDY' and a.switch_id = b.switch_id	and rownum<=";
		memset( tmp,0,sizeof(tmp) );
		sprintf(tmp,"%d",iRows);
		sql += tmp;
		//生成文件
		qry.close();
		qry.setSQL(sql.c_str());
		qry.open();

		int iCount = 0;

		while( qry.next() ) 
		{
			AreaCode = qry.field(4).asString();
			pFileInfo = getFileInfo( AreaCode,1 );
			if( !pFileInfo )
				return;

			osFileId << qry.field(0).asLong() << ',';
			pFileInfo->m_vecFileID.push_back( qry.field(0).asLong() );

			pFileInfo->m_osContent << qry.field(1).asString() <<','<< qry.field(2).asInteger() <<','<< qry.field(3).asInteger() << endl;
			pFileInfo->m_iCount ++;

			if( pFileInfo->m_iCount == iFileCount )
			{
				writeFile( TempPath,Path,AreaCode );
				updateState( AreaCode );


			}
			
			iCount ++;

			if( iCount%30 == 0 )
			{
				//更新中间状态
				sSqlExc = GetUpdateString(osFileId.str().c_str(),"RUN");
				qryExc.setSQL(sSqlExc.c_str());
				qryExc.execute();
				osFileId.clear();
			}
		}
		qry.close();

		if( osFileId.str().length() )
		{
			sSqlExc = GetUpdateString(osFileId.str().c_str(),"RUN");
			qryExc.setSQL(sSqlExc.c_str());
			qryExc.execute();
		}
		qryExc.commit();
		qryExc.close();
		
		osFileId.clear();
		//到达时间强制提交，未满足行数要求的文件
		forceCommit(TempPath,Path,iForceCommit);

		sleep(iSleep);
	}
}

int main(int argc, char **argv)
{
	PrepLog preplog;
	DenyInterrupt();
	preplog.run();
	return 0;
}