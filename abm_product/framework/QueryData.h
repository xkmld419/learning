/*VER: 1*/ 
#ifndef QUERYDATA_H_HEADER_INCLUDED_BDBDC8E4
#define QUERYDATA_H_HEADER_INCLUDED_BDBDC8E4

#include "TOCIQuery.h"

#define DEFINE_QUERYDATA(x) TQUERY_DATA x;

class TQUERY_DATA
{
	
public:
	void setDBLogin(char *name, char *pwd, char *str);
	
  void getDBConn(bool bReconnect = false);
	
	static void commit();

  static void rollback();
  
  TOCIQuery *getQuery();
  
  static bool m_bConnect;

private:

  static char m_sDBUser[24];

  static char m_sDBPwd[24];

  static char m_sDBStr[32];

  static TOCIDatabase m_oDefineConn;
	
	static TOCIQuery * query_acct;
	
	static bool m_bDefaultConn;
};

#endif /*QUERYDATA_H_HEADER_INCLUDED_BDBDC8E4*/

