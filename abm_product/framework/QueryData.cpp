/*VER: 1*/ 
#include <string.h>
#include <iostream>
#include "QueryData.h"

using namespace std;

void TQUERY_DATA::setDBLogin(char *name, char *pwd, char *str)
{
	strncpy (m_sDBUser, name, sizeof(m_sDBUser));
	strncpy (m_sDBPwd, pwd, sizeof(m_sDBPwd));
	strncpy (m_sDBStr, str, sizeof(m_sDBStr));	

	m_bDefaultConn = false;
}

void TQUERY_DATA::getDBConn(bool bReconnect)
{
	if (bReconnect) {
		if (m_bConnect)
		try {
			m_oDefineConn.disconnect ();
		} catch (...) {
		}	

		m_bConnect = false;
	}

	if (!m_bConnect) {
		if (m_bDefaultConn) {
			strcpy (m_sDBUser, "acct");
			strcpy (m_sDBPwd, "acct");
			strcpy (m_sDBStr, "acct135");
		}
		m_oDefineConn.connect (m_sDBUser, m_sDBPwd, m_sDBStr);
	}
	
	query_acct = new TOCIQuery(&m_oDefineConn);
	
	m_bConnect = true;
}

TOCIQuery *TQUERY_DATA::getQuery()
{
	if (m_bConnect)
		return query_acct;
}

char TQUERY_DATA::m_sDBUser[24] = {0};

char TQUERY_DATA::m_sDBPwd[24] = {0};

char TQUERY_DATA::m_sDBStr[32] = {0};

TOCIDatabase TQUERY_DATA::m_oDefineConn;

TOCIQuery *TQUERY_DATA::query_acct;

bool TQUERY_DATA::m_bConnect = false;

bool TQUERY_DATA::m_bDefaultConn = true;

void TQUERY_DATA::commit()
{
	if (m_bConnect) m_oDefineConn.commit ();
}

void TQUERY_DATA::rollback()
{
	if (m_bConnect) m_oDefineConn.rollback ();
}

