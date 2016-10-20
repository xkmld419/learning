#ifndef TOCI_H__
#define TOCI_H__

#include <stdio.h>
#include <oci.h>
#include <oci8dp.h>
#include "TOCIQuery.h"

#define 	DEF_TRANS_BUF_SIZE		1024*1024*10
#define		INDATA_SAVE_GUARD		1024
#define		LINE_MAXi		256

//classes declared in this file
class TOCIDirPath;
class TOCIDirPathArray;

//class TOCIDirPathFile;

class TOCIDirPath
{
public:
	//TOCIDirPath(TOCIDatabase *pdbConnect);
	TOCIDirPath(TOCIDatabase *pdbConnect,char *strTableName=NULL);
	~TOCIDirPath();
	
	void prepare();
	void append();
	int	 load(unsigned uRows = -1);
	void	 save();
	void	 abort();
	void	 finish();
	
	void setTableName(char *strTableName);
	void setSchameName(char *strSchameName);
	void setBufferSize(int );
	
	long getSaveCount();
	
	/*bool setBufSize(int iBufSize);
	bool setDateFormat(char *strDateFormat);
	bool setCharsetID(char *strCharsetID);
	void noLog(bool bNoLog=TRUE);
	*/
	void setField(char *strFieldName,char *fieldValue);
	void setField(char *strFieldName,int	fieldValue);
	void setField(char *strFieldName,double	fieldValue);
	void setField(char *strFieldName,long	fieldValue);
	void setField(unsigned uPos,char *fieldValue);
	void setField(unsigned uPos,int	fieldValue);
	void setField(unsigned uPos,double	fieldValue);
	void setField(unsigned uPos,long	fieldValue);
	
private:
	OCIDirPathCtx		*m_hDirPathCtx;
	OCIError			*m_hError;
	OCIDirPathColArray	*m_hDirPathCA;
	OCIDirPathStream	*m_hDirPathStr;
	TOCIQuery			*m_Query;
	bool				*m_FieldAssigned;
	
	TOCIDatabase		*m_pDB;
	TOCISession			*m_pSession;
	
	char		m_strTableName[LINE_MAXi];
	char		m_strSchameName[LINE_MAXi];
	
	bool		m_bPrepared;
	unsigned	m_uMaxRows;
	unsigned	m_uMaxCols;
	int			m_iRowOff;
	unsigned	m_uRowSize;
	unsigned	m_uBufSize;
	unsigned	m_uBufCurPos;	
	void 		*m_inData;	
	
	int			m_iErrorNo;
	int         m_iBufferSize;
	long        m_lSaveCount;
	
	void setFieldNull(unsigned uIndex);
	void checkError();
	OCIDate  strToOCIDate(char * str);
};

/*
class TOCIDirPathFile: public TOCIDirPath
{
};
*/
#endif	//TOCIDIRPATH_H__



