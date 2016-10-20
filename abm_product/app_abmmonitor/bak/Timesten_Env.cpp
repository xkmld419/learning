#include "Timesten_Env.h"


TTStatus		TimesTenEnv::ttStatus;
TTConnection	TimesTenEnv::ttConn;
string			TimesTenEnv::sConnStr;

void TimesTenEnv::getMMDBInfo()
{
	/*char sSql[2000];
	DEFINE_QUERY(qry);

	if (sConnStr.size()!=0)
		return ;

	sprintf(sSql,"SELECT TTCONSTR FROM mmdb_info WHERE serve_id=%d ",Process::m_iServeID);
	qry.close();
	qry.setSQL(sSql);
	qry.open();
	qry.next();
	sConnStr=sConnStr+qry.field("TTCONSTR").asString();
	sConnStr[sConnStr.size()]='\0';
	*/
}

void TimesTenEnv::setDBLogin(string sConnectstr)
{
	sConnStr=sConnectstr;
	sConnStr[sConnStr.size()]='\0';
}

TTConnection * TimesTenEnv::getDBConn(bool bReconnect)
{
	if (bReconnect) {
		if (ttConn.isConnected())

		try {
			ttConn.Disconnect(ttStatus);
		} catch (...) {
		}

	}

	if (!ttConn.isConnected()) {
		getMMDBInfo();
		ttConn.Connect ((const char *)sConnStr.c_str(),ttStatus);
	}

	return &ttConn;
}

void TimesTenEnv::commit()
{
	if (ttConn.isConnected())
	{
		ttConn.Commit(ttStatus);
	}
}

void TimesTenEnv::rollback()
{
	if (ttConn.isConnected()){
		ttConn.Rollback(ttStatus);
	}
}

void TimesTenEnv::disconnect()
{
	if (ttConn.isConnected()){
		ttConn.Disconnect(ttStatus);
	}
}


TimesTenCMD::TimesTenCMD(TTConnection * pttConnect)
{
	this->pttConnect=pttConnect;
	this->pttStatus=&ttStatus;
}

TimesTenCMD::TimesTenCMD(TTConnection * pttConnect,TTStatus *pttStatus)
{
	this->pttConnect=pttConnect;
	this->pttStatus=pttStatus;
}

TimesTenCMD::~TimesTenCMD()
{
	try
	{
		TTCmd::Close (*pttStatus);
		TTCmd::Drop (*pttStatus);
	}
	catch (TTError err)
	{
		if (err.native_error != 0)
		{
			cerr << "Error encountered " << err.native_error << ": " << err.err_msg << endl;
			throw err;
		}
	}
	catch (...)
	{
		throw;
	}
}

void TimesTenCMD::Prepare(const char* sqlP, const char*explanationP)
{
	Close();
	TTCmd::Prepare(pttConnect,sqlP,explanationP,*pttStatus);
}

//zhangcheng  add
void TimesTenCMD::Commit()
{
	  if ((*pttConnect).isConnected()){
		  (*pttConnect).Commit(*pttStatus);
	  }
}

void TimesTenCMD::Prepare(const char* sqlP)
{
	Close();
	TTCmd::Prepare(pttConnect,sqlP,*pttStatus);
}

void TimesTenCMD::Execute ()
{
	try
	{
		TTCmd::Execute(*pttStatus);
	}
	catch (TTError err)
	{
		if (err.native_error == 6003)
		{
			cerr << "Error encountered " << err.native_error << ": " << err.err_msg << endl;
			sleep(1);
			try
			{
				TTCmd::Execute(*pttStatus);
			}
			catch (TTError err)
			{
				cerr << "Error encountered " << err.native_error << ": " << err.err_msg << endl;
				throw err;
			}
			catch (...)
			{
				throw;
			}
		}
		else
		{
			throw err;
		}
	}
}

int TimesTenCMD::ExecuteImmediate (const char * sqlP)
{
	return TTCmd::ExecuteImmediate (pttConnect,sqlP,*pttStatus);
}

int TimesTenCMD::FetchNext ()
{
	return TTCmd::FetchNext (*pttStatus);
}

void TimesTenCMD::setMaxRows (const int nRows)
{
	TTCmd::setMaxRows (nRows, *pttStatus);
}

int TimesTenCMD::getMaxRows ()
{
	return TTCmd::getMaxRows (*pttStatus);
}

void TimesTenCMD::Close ()
{
	try
	{
		TTCmd::Close (*pttStatus);
//		TTCmd::Drop (*pttStatus);
	}
	catch (TTError err)
	{
		if (err.native_error != 0)
		{
			cerr << "Error encountered " << err.native_error << ": " << err.err_msg << endl;
			throw err;
		}
	}
	catch (...)
	{
		throw;
	}
}

void TimesTenCMD::Drop ()
{
	try
	{
		TTCmd::Close (*pttStatus);
		TTCmd::Drop (*pttStatus);
	}
	catch (TTError err)
	{
		if (err.native_error != 0)
		{
			cerr << "Error encountered " << err.native_error << ": " << err.err_msg << endl;
			throw err;
		}
	}
	catch (...)
	{
		throw;
	}
}

void TimesTenCMD::setQueryTimeout (const int nSecs)
{
	TTCmd::setQueryTimeout (nSecs, *pttStatus);
}
void TimesTenCMD::PrepareBatch(const char * sqlP,TTCmd::TTCMD_USER_BIND_LEVEL level, unsigned short batchSize)
{
	Close();
	TTCmd::PrepareBatch(pttConnect, sqlP,level, batchSize, *pttStatus);
}

/*
void TimesTenCMD::BindParameter(int pno, unsigned short batchSize,<TYPE>* p, [SQLLEN*] q)
{
	TTCmd::BindParameter(pno,batchSize,p, q, *pttStatus);
}
*/

void TimesTenCMD::ExecuteBatch(unsigned short numRows)
{
	TTCmd::ExecuteBatch(numRows, *pttStatus);
}




void TimesTenConn::getMMDBInfo()
{
	/*char sSql[2000];
	DEFINE_QUERY(qry);

	if (sConnStr.size()!=0)
		return ;

	sprintf(sSql,"SELECT TTCONSTR FROM mmdb_info WHERE serve_id=%d ",Process::m_iServeID);
	qry.close();
	qry.setSQL(sSql);
	qry.open();
	qry.next();
	sConnStr=sConnStr+qry.field("TTCONSTR").asString();
	sConnStr[sConnStr.size()]='\0';
	*/
}

void TimesTenConn::setDBLogin(string sConnectstr)
{
	sConnStr=sConnectstr;
	sConnStr[sConnStr.size()]='\0';
}

TTConnection * TimesTenConn::getDBConn(bool bReconnect)
{
	if (bReconnect) {
		if (ttConn.isConnected())

		try {
			ttConn.Disconnect(ttStatus);
		} catch (...) {
		}

	}

	if (!ttConn.isConnected()) {
		getMMDBInfo();
		ttConn.Connect ((const char *)sConnStr.c_str(),ttStatus);
	}

	return &ttConn;
}

void TimesTenConn::commit()
{
	if (ttConn.isConnected()){
		ttConn.Commit(ttStatus);
	}
}

void TimesTenConn::rollback()
{
	if (ttConn.isConnected()){
		ttConn.Rollback(ttStatus);
	}
}

void TimesTenConn::disconnect()
{
	if (ttConn.isConnected()){
		ttConn.Disconnect(ttStatus);
	}
}
