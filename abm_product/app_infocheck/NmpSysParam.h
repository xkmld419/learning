/*VER:1*/

#ifndef __NMP_SYSPARAM_H__
#define __NMP_SYSPARAM_H__

#include <vector>
#include <map>
#include <algorithm>
#include "TOCIQuery.h"
//#include "../app_init/CommandCom.h"
#include "CommandCom.h"
#include "Process.h"

#define 	_OID_NAME_LEN		51
#define 	_OID_TYPE_LEN		4
#define	_OID_MACRO_LEN	51
#define	_OID_SQL_LEN		1024
//#define	_OID_SECTION_LEN	31
//#define	_OID_PARAM_LEN		31


using namespace std;

class Nmp_OID_VALUE
{
	friend class NmpOIDMgr;
public:
	Nmp_OID_VALUE(int iTargetID,char *sOidName)
	{
//		m_iTargetID = iTargetID;
		strcpy(m_sOIDName,sOidName);
	}

	Nmp_OID_VALUE();
	~Nmp_OID_VALUE()
	{		
	}
	//int load();	

	int getValuesFrmSql(const char *sMacroName,const char *sMacroValue,char *sParamValue);	
	int getValuesFrmSql(char *sParamValue);	
	
private:
	//int m_iTargetID;
	char m_sOIDName[_OID_NAME_LEN];
	char m_sOIDValue[_OID_TYPE_LEN]; //'V0T' TABLE型,'V0A' SINGLE VALUE
	int m_iValueSeq;   //table型oid取值的顺序
	char m_sParamSection[MAX_SECTION_NAME_LEN];
	char m_sParamName[MAX_PARAM_NAME_LEN];
	int m_iIfIndex;
	int m_iValueID;
	int m_iVisible;
	char m_sValueSql[_OID_SQL_LEN];
	//vector<string> m_vecValue;
	//Nmp_SQL_value *m_oSqlValue;	
};

typedef  vector<Nmp_OID_VALUE *>  VEC_NMPOIDVALUE;

/*class Nmp_SQL_value
{
public:
	Nmp_value(int iValueID)
		{
			memset(m_sValueSql,0,sizeof(m_sValueSql));
			m_iValueID = iValueID;
			m_vecValue.clear();
		}
	~Nmp_value(){m_vecValue.clear();}
	int getValuesFrmSql();
private:
	char m_sValueSql[1024];
	int    m_iValueID;
	vector<string> m_vecValue;
};*/

class NmpParamOID
{
	friend class NmpOIDMgr;
	friend class findParamOID;
	friend class NmpSysParamsGet;
public:
	NmpParamOID()
	{
		memset(m_sOIDName,0,sizeof(m_sOIDName));
		m_sMacroValue = string("");
	}
	~NmpParamOID(){};
	/*bool operator==(const NmpParamOID& c) const
	 {
		bool bFlag;
		bFlag = strcmp(c.m_sOIDName,m_sOIDName) == 0 && strcmp(c.m_sMacroValue.c_str(),m_sMacroValue.c_str());
		return bFlag;
	  //return c.m_str == m_str && c.m_i == m_i;//这里可以自定匹配个数
	 }*/
private:
	char m_sOIDName[_OID_NAME_LEN];
	string   m_sMacroValue;
};

class findParamOID
	{
	public:
		findParamOID(char *sName,string strValue) {
			strcpy(m_sName,sName);
			m_sValue = strValue;
		}
		bool operator() (NmpParamOID &S)
		{
			bool bFlag = (strcmp(S.m_sOIDName,m_sName) == 0) && (strcmp(S.m_sMacroValue.c_str(),m_sValue.c_str())==0);
			return bFlag;
		};
	private:
		char m_sName[_OID_NAME_LEN];
		string   m_sValue;
	};

class NmpOIDInfo
{
	friend class NmpOIDMgr;
public:
	NmpOIDInfo()
	{
		memset(m_sOIDName,0,sizeof(m_sOIDName));
		memset(m_sOIDType,0,sizeof(m_sOIDType));
		memset(m_sOIDMacro,0,sizeof(m_sOIDMacro));
		memset(m_sValueSql,0,sizeof(m_sValueSql));
		m_vecValue.clear();
		//m_oSqlValue = NULL;
	}
	~NmpOIDInfo()
	{
/*		if (m_oSqlValue)
		{
			delete m_oSqlValue;
			m_oSqlValue = 	NULL;
		}*/
	}

	int getValuesFrmSql();	

private:
	char m_sOIDName[_OID_NAME_LEN];
	char m_sOIDType[_OID_TYPE_LEN];   //'V0T'	TABLE,'V0A'	SINGLE VALUE;
	char m_sOIDMacro[_OID_MACRO_LEN];
	int m_iValueID;
	char m_sValueSql[_OID_SQL_LEN];
	vector<string> m_vecValue;
	//Nmp_SQL_value *m_oSqlValue;
};



class NmpOIDMgr
{
	//friend class Nmp_OID_VALUE;
	//friend class NmpOIDInfo;
	//friend class Nmp_SQL_value;
	//friend class NmpParamOID;
public:
	NmpOIDMgr();
	~NmpOIDMgr();
	int load();
	int unload();
	
	int instantiateParamName();
//校验参数名的配置，如不能存在oid中未定义的宏
	bool checkParamName(char *sOIDName,char *sParamName);
	bool ifParamNeedRenew(SysParamHis & oSysParam,NmpParamOID &oParamMacroInstance,bool bIfFirstRun);
	int  refreshParamToBsn(SysParamHis &oSysParam,NmpParamOID &oParamMacro);
	int setCommandComm(CommandCom *pComm);
private:	
	bool replaceMacro(string &sSrc,string sSubStr,string sReplaceStr);
	int instToBsnInterface(TOCIQuery &qry,string sOID, string strBsnValue);
	int getMapValue(char const *sSection,char const *sParamName,char *sOriValue,char *sMapedValue);
	
	//int m_iTargetID;
	char m_sOIDName[_OID_NAME_LEN];
	char m_sAreaCode[9];
	CommandCom *m_pCommandCom;
	
	Nmp_OID_VALUE *m_oOIDValue;
	map<string,NmpOIDInfo *>m_mapOIDInfo;
	map<string,VEC_NMPOIDVALUE *> m_mapOIDValueList;
	map<string,NmpParamOID *> m_mapParamNameToOID;   //参数和oid的对应关系，以参数为索引
};


class NmpSysParamsGet:public Process
{
public:
	NmpSysParamsGet()
	{
		m_vecSysParamHis = NULL;
		m_vecParamMacroValue.clear();
	};
	~NmpSysParamsGet(){
		if (m_vecSysParamHis)
		{
			m_vecSysParamHis->clear();
			delete m_vecSysParamHis;
			m_vecSysParamHis = NULL;			
		}
			
	};
	int run();
	int SubscribeSections();
	int freeVecParamMacro();
private:	
	NmpOIDMgr m_oOIDMgr();
	vector<SysParamHis> *m_vecSysParamHis;
	vector<NmpParamOID> m_vecParamMacroValue;
	static bool m_bFirstRun;
	public:
	CommandCom* m_pCommandCom;
};



#endif

