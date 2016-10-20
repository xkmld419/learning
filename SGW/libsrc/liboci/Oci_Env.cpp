
#include "Oci_Env.h"

/*
otl_connect		OciEnv::otl_Conn;
string				OciEnv::userName;
string				OciEnv::passWord;
string				OciEnv::tnsStr;
stringstream	OciEnv::stream_last_error;
*/

OciEnv* OciEnv::getInstance()
{
	static OciEnv s_instance;
	return &s_instance;
}

void OciEnv::setUser(const string& user)
{
	m_userName = user;
}

void OciEnv::setPass(const string& pass)
{
	m_passWord = pass;
}

void OciEnv::setConnStr(const string& connStr)
{
	m_tnsStr = connStr;
}

bool OciEnv::connectDb(const string &user, const string &pass, const string &tnsStr)
{
	if(!user.empty()&&(!pass.empty())&&(!tnsStr.empty()))
	{
		m_userName	= user;
		m_passWord	= pass;
		m_tnsStr		= tnsStr;
	}
	
	if(m_userName.empty())
	{
		stream_last_error.clear();
		stream_last_error<<"error:database username can't be null";
		return false;
	}
	
	
	if(m_passWord.empty())
	{
		stream_last_error.clear();
		stream_last_error<<"warning:database pass can't be null";
		fprintf(stderr, stream_last_error.str().c_str());
	}
	
	if(m_tnsStr.empty())
	{
		stream_last_error.clear();
		stream_last_error<<"database tns can't be null";
		return false;
	}
	
	char tem_user[128];
	char tem_pass[128];
	char tem_tns[128];
	
	strcpy(tem_user, m_userName.c_str());
	strcpy(tem_pass, m_passWord.c_str());
	strcpy(tem_tns, m_tnsStr.c_str());
	
	try
	{
		oci_database.connect(tem_user, tem_pass, tem_tns);
		Connected = false;
	}
	catch(TOCIException &oe)
	{
		stream_last_error.clear();
		stream_last_error<<"in function:"<<"connectDb"<<endl;
		stream_last_error<<oe.getErrMsg()<<endl; 
		stream_last_error<<oe.getErrSrc()<<endl; 
		return false;
	}
	
	return true;
}

void OciEnv::commit()
{
	oci_database.commit();
}

TOCIDatabase* OciEnv::getOCIDatabase()
{
	return &oci_database;
}

void OciEnv::auto_commit_off()
{
	//otl_Conn.auto_commit_off();
}
void OciEnv::auto_commit_on()
{
	//otl_Conn.auto_commit_off();
}

void OciEnv::rollback()
{
	oci_database.rollback();
}

void OciEnv::disconnect()
{
    commit();
	oci_database.disconnect();
	Connected = false;
}

bool OciEnv::isConnected()
{
	return Connected;
}



