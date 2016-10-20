
#include "DBManager.h"
#include "encode.h"
#include "TOCIQuery.h"

#include "ParamDefineMgr.h"

#include <fstream>

#include <unistd.h>
#include <time.h>

DBManager::DBManager(void)
:m_pDBInfo(0)
{
	char tmp[128];
	memset( tmp,0,sizeof(tmp) );
	ParamDefineMgr::getParam("DBMANAGER", "TEMP_PATH", tmp);
	m_temp_path = tmp;
}

DBManager::~DBManager(void)
{
}

//密码编码
char* DBManager::encode(char* sSrcStr,char * sDecStr)
{
	::encode( sSrcStr,sDecStr );
	return sDecStr;
}

//密码解码
char* DBManager::decode(char* sSrcStr,char * sDecStr)
{
	::decode(sSrcStr,sDecStr);
	return sDecStr;
}

void DBManager::ClearVecDBInfo()
{
	VEC_DBINFO::iterator it;
	for( it = m_vecDBInfo.begin();it != m_vecDBInfo.end();it++ )
	{
		if( *it != NULL )
			delete *it;
	}
	m_vecDBInfo.clear();
}

void DBManager::ClearFileContent()
{
	MAP_FILECONTENT::iterator it;
	VEC_FILECONTENT::iterator itvec;
	VEC_FILECONTENT *pVecFileContent;

	for( it = m_mapFileContent.begin();it != m_mapFileContent.end();it++ )
	{
		pVecFileContent = it->second;
		if( pVecFileContent != NULL )
		{
			for( itvec = pVecFileContent->begin();itvec != pVecFileContent->end();itvec ++ )
			{
				if( *itvec != NULL )
					delete *itvec;
			}
			delete pVecFileContent;
		}

	}
	m_mapFileContent.clear();
}

//读取数据库中密码信息
int DBManager::GetPassInfo()
{
	TOCIQuery qry(Environment::getDBConn());
	DBInfo *pDBInfo = NULL;

	ClearVecDBInfo();

	std::string sqltxt = "select database_str,username,old_passwd,passwd from B_SERVICE_PASSWD_CFG where action = 'C'";
	
	qry.close();
	qry.setSQL(sqltxt.c_str());
	qry.open();

	while( qry.next() ) 
	{ 
		pDBInfo = new DBInfo();
		strcpy( pDBInfo->sDBStr,		qry.field(0).asString());
		strcpy( pDBInfo->sUser,			qry.field(1).asString());
		strcpy( pDBInfo->sOldPasswd,	qry.field(2).asString());
		strcpy( pDBInfo->sPasswd,		qry.field(3).asString());

		m_vecDBInfo.push_back(pDBInfo);
	}
	qry.close();
	return m_vecDBInfo.size();
}

int DBManager::GetFileContent()
{
	TOCIQuery qry(Environment::getDBConn());
	FileContent *pFileContent = NULL;
	std::string strFileName;

	MAP_FILECONTENT::iterator it;
	VEC_FILECONTENT *pVecFileContent;

	ClearFileContent();
	int iCount = 0; 

	std::string sqltxt = "select b.file_path||b.filename,a.section,a.username,a.passwd,a.database_str from  \
		b_passwd_file_content a,b_service_passwd_file_cfg b where a.file_id = b.file_id;";

	qry.close();
	qry.setSQL(sqltxt.c_str());
	qry.open();

	while( qry.next() ) 
	{ 
		pFileContent = new FileContent();
		strcpy( pFileContent->sFileName,	qry.field(0).asString());
		strcpy( pFileContent->sSection,		qry.field(1).asString());
		strcpy( pFileContent->sUser,		qry.field(2).asString());
		strcpy( pFileContent->sPasswd,		qry.field(3).asString());
		strcpy( pFileContent->sDBStr,		qry.field(4).asString());

		strFileName = pFileContent->sFileName;

		it = m_mapFileContent.find( strFileName );

		if(it == m_mapFileContent.end() )
		{
			pVecFileContent = new VEC_FILECONTENT();
			m_mapFileContent[strFileName] = pVecFileContent;
		}

		pVecFileContent->push_back(pFileContent);
		
		iCount ++ ;
	}
	qry.close();
	return iCount;
}

int DBManager::doProcess()
{
	int iTmp = 0;

	VEC_DBINFO::iterator itDB;
	MAP_FILECONTENT::iterator itmapFile;
	VEC_FILECONTENT::iterator itvecFile;
	VEC_FILECONTENT *pVecFileContent;
	FileContent *pFileContent;
	DBInfo *pDBInfo = NULL;

	while(1)
	{
		sleep(10000);
		//取密码信息
		iTmp = GetPassInfo();
		if( iTmp )
			continue;
		//取文件内容
		iTmp = GetFileContent();
		if( iTmp )
			continue;

		for( itDB = m_vecDBInfo.begin();itDB != m_vecDBInfo.end();itDB++ )
		{
			pDBInfo = *itDB;
			if( pDBInfo !=NULL )
			{
				//修改密码
				iTmp = ChangeDBPass(pDBInfo->sDBStr,pDBInfo->sUser,pDBInfo->sOldPasswd,pDBInfo->sPasswd);
				if( iTmp == 1 )
				{
					//修改文件中的密码
					for( itmapFile = m_mapFileContent.begin();itmapFile != m_mapFileContent.end();itmapFile++ )
					{
						pVecFileContent = itmapFile->second;
						if( pVecFileContent != NULL )
						{
							for( itvecFile = pVecFileContent->begin();itvecFile != pVecFileContent->end();itvecFile ++ )
							{
								pFileContent = *itvecFile;
								if( pFileContent != NULL )
								{
									if( (0 == strcmp(pFileContent->sDBStr,pDBInfo->sDBStr))
										&& (0 == strcmp(pFileContent->sUser,pDBInfo->sUser))
									)
									{
										encode( pDBInfo->sPasswd,pFileContent->sPasswd );
									}
								}

							}
						}
					}
				}
				else
					cout<<"密码修改错误"<<endl;

			}
		}
		UpdateCfgFile();		
	}

}

//修改数据库密码
int DBManager::ChangeDBPass(char* sDBStr,char* sUser,char* sOldPass,char* sNewPass)
{
	return 1;
}

//修改文件
int DBManager::UpdateCfgFile()
{
	CreateCfgFile();
	return 1;
}

//生成文件
int DBManager::CreateCfgFile()
{
	ofstream file;

	MAP_FILECONTENT::iterator it;
	VEC_FILECONTENT::iterator itvec;
	VEC_FILECONTENT *pVecFileContent;
	
	FileContent *pFileContent;

	char str[2048];
	

	for( it = m_mapFileContent.begin();it != m_mapFileContent.end();it ++ )
	{
		if(file.is_open())
			file.close();
		file.open(it->first.c_str());
		pVecFileContent = it->second;
		for( itvec = pVecFileContent->begin();itvec != pVecFileContent->end();itvec ++ )
		{
			pFileContent = *itvec;
			memset( str,0,sizeof(str) );
			sprintf(str,"[%s]\n\
				username=%s\n\
				password=%s\n\
				connstr=%s\n\n\0",
				pFileContent->sSection,
				pFileContent->sUser,
				pFileContent->sPasswd,
				pFileContent->sDBStr);
			
			file<<str<<flush;
		}
		file.close();
	}
	return 1;
}

int main (int argc, char **argv)
{
	DBManager m_DBManager;
	m_DBManager.doProcess();
	
	return 0;
}