/***********************************************************************
 * Module:  QueueInDb.h
 * Author:  Yuan Peng
 * Modified: 2009年4月20日 10:22:41
 * Version: 1.0
***********************************************************************/
#ifndef _QUEUE_IN_DB_H_
#define _QUEUE_IN_DB_H_

#include <vector>
#include <map>
#include "StdEvent.h"
#include "Environment.h"

using namespace std;

/***********************************************************************
 * Modified: 2009年4月20日 10:31:59
 * Purpose: Declaration of the class EventFieldDef
 * Comment: 定义event_attr_id对应的默认字段
 ***********************************************************************/
 /*
 typedef struct Event_Attr_Field
{
	int m_iEventAttrID;
   	char m_sFieldName[31];
   	char m_sDataType[4];   	
} EventAttrField;

 
class EventAttrFieldMgr
{
public:
	EventAttrFieldMgr()
	{
		memset(m_sInsSql,0,4001);
		memset(m_sInsValue,0,4001);
	}
	~EventAttrFieldMgr()
	{
		delete[] m_vEventFieldIndex;
		delete[] m_vEventFieldList;
		m_vEventFieldIndex = NULL;
		m_vEventFieldList = NULL;
	};   	
	
	static bool load();
	static bool getEventAttr(int iAttrID,char *sFieldName,char *sDataType);		
protected:
private:
   	static HashList<EventAttrField *> *m_vEventFieldIndex;
	static EventAttrField *m_vEventFieldList;
	static bool m_bLoadEvent;
	static int m_iAttrCnt;

	char m_sInsSql[4001];
	char m_sInsValue[4001];
};
*/

/***********************************************************************
 * Modified: 2009年4月20日 10:31:59
 * Purpose: Declaration of the class FileInDBField
 * Comment: 为每类文件类型定义特定的入库字段，未定义的字段采用event_attr_id默认对应的字段
 ***********************************************************************/

class FileInDBField
{
public:
	FileInDBField(int iErrFileType,int iAttrID,char *sFieldName,char *sDatatype);
	~FileInDBField(){};   	

	int m_iErrFileTypeID;
   	int m_iEventAttrID;
   	char m_sFieldName[31];
   	char m_sDataType[4];       //D0A:字符串，D0B:数值，D0C:日期
   	FileInDBField *m_oNext;
protected:
private:
  

};

/***********************************************************************
 * Modified: 2009年4月20日 10:31:59
 * Purpose: Declaration of the class FileTypeDef
 * Comment: 根据filerule找不到文件类型的设文件类型为－1，对应入库字段采用默认的event_attr_id对应的定义字段
 ***********************************************************************/
class FileTypeDef
{
	friend class FileDef;
	friend class QueueInDbMgr;
public:
	FileTypeDef(int iErrFileType,char *sFileRule,int iFileType,char *sTabName,int iGrpID,char *sIndbErrPath,char *sIndBErrFile)
	{
		m_iErrFileTypeID = iErrFileType;
		memset(m_sFileRule,0,33);
		memset(m_sINDbTable,0,31);
		memset(m_sIndbErrPath,0,33);
		memset(m_sIndbErrFileName,0,33);
		
		strncpy(m_sFileRule,sFileRule,32);
		m_iFileTypeID = iFileType;
		strncpy(m_sINDbTable,sTabName,30);
		m_iGroupID = iGrpID;
		strncpy(m_sIndbErrPath,sIndbErrPath,32);		
		strncpy(m_sIndbErrFileName,sIndBErrFile,32);		
		m_oInDBField = NULL;
		memset(m_sInsSql,0,2001);
		memset(m_sInsValue,0,2001);
		m_iAttrNum = 0;
		m_vEventList.clear();
	}
	~FileTypeDef(){};
   	bool load();	
   	bool unload();   	
	bool checkName(char *sLongName);
	//void saveToDB(StdEvent &poEventSection);
	//void commitDB();
	
protected:
private:
   	int m_iErrFileTypeID;
   	char m_sFileRule[33];
   	int m_iFileTypeID;
   	char m_sINDbTable[31];
	int m_iGroupID;
	char m_sIndbErrPath[33];
	char m_sIndbErrFileName[33];
   	char m_sInsSql[4001];
	char m_sInsValue[4001];
	int m_iAttrNum;  //统计每种文件类型中属性的个数，超过150个拼sql
	vector<StdEvent> m_vEventList;
			
   	FileInDBField *m_oInDBField;
	bool maskStr ( char*   i_sName,char*   i_sMask );
	int formatInsSql();
	//int formatSqlValue(StdEvent & poEventSection);
   	int createTable();
   	int execSql();	
};

/*********************************************************************** 
 * Modified: 2009年7月8日 9:58:30
 * Purpose: Declaration of the class FileDef
 * Comment: 文件级，需要每个文件结束时提交
 ***********************************************************************/

class FileDef
{
	friend class QueueInDbMgr;
	public:
		FileDef(int iFileID,FileTypeDef *pFileType)
		{
			m_iFileID = iFileID;
			m_oFileType = pFileType;
			m_iSaveEventCnt = 0;
			m_vEventList.clear();
			m_vInstSqlList.clear();
			m_oNext = NULL;			
		}
		~FileDef()
		{
			m_vEventList.clear();
		}
   		int saveEvent(StdEvent &poEventSection);
   		int commitEvent();
		
	protected:
	private:   		
		vector<StdEvent> m_vEventList;
		vector<string>m_vInstSqlList;
		int m_iFileID;
		FileTypeDef *m_oFileType;
		FileDef *m_oNext;
		int m_iSaveEventCnt;

		int commitSingleEvent();
		void getInstSql(StdEvent & poEventSection,string &sSql);
		int commitEvent_2();
};


/***********************************************************************
 * Modified: 2009年4月20日 10:27:58
 * Purpose: Declaration of the class QueueInDbMgr
 * Comment: 队列入库
 ***********************************************************************/

class QueueInDbMgr
{
public:
	QueueInDbMgr()
	{			
			m_oFileTypeList.clear();
			m_oFileHead = NULL;			
			//m_vEventList.clear();
	}
	~QueueInDbMgr(){unload();}
   	int saveEvent(StdEvent &poEventSection,int iFileID);
	int commitEvent(int iFileID);

protected:
private:
   	int getAbstractFileName(char *m_sFileName)
   	{
   		if (strlen(m_sFileName) == 0) 
   		{
   			memset(m_sAbstractFileName,65,0);
			return 0;			
   		}

		char *p;
		p=strrchr(m_sFileName,'/');
		if (p==NULL)
			strcpy(m_sAbstractFileName,m_sFileName);
		else
			strcpy(m_sAbstractFileName,p+1);

		return 1;
   	}
   	
   	char m_sAbstractFileName[65];
   	//FileTypeDef m_oFileType;
	
	map<int,FileTypeDef *> m_oFileTypeList;

	static bool m_bLoad;
	static FileDef *m_oFileHead;

	void load();
	void unload();
	
	FileTypeDef * getFileTypeByName(char *m_sFileName);
	FileTypeDef * getFileTypeByID(int iFileTypeID);
	FileDef *searchFileDefList(int iFileID);
	void addFileDefList(FileDef *poFileDef);
	

};

#endif
