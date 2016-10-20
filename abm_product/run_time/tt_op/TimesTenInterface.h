#ifndef _TIMESTEN_INTERFACE_INCLUDE_HEADER
#define _TIMESTEN_INTERFACE_INCLUDE_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <ttclasses/TTInclude.h>

#include <string>
#include <iostream>
using namespace std;

#define DEFINE_TTCMD(x) 								TimesTenCMD x (TimesTenEnv::getDBConn())
#define DEFINE_TTCMDPOINT(x) 		                    x = new TimesTenCMD (TimesTenEnv::getDBConn());
#define DEFINE_TTCMD_EXTSTAT(x,pttStatus) 				TimesTenCMD x (TimesTenEnv::getDBConn(),pttStatus)

#define DEFINE_TTCMD_BYCONN(x,pttConn) 					TimesTenCMD x (pttConn)

#define DEFINE_TTCMD_BYTTCONN(x,TTCONN)					        TimesTenCMD x (TTCONN.getDBConn())
#define DEFINE_TTCMD_BYTTCONN_EXTSTAT(x,TTCONN,pttStatus)		TimesTenCMD x (TTCONN.getDBConn(),pttStatus)
#define DEFINE_TTCMDPOINT_BYTTCONN(x,TTCONN)     x = new TimesTenCMD (TTCONN->getDBConn());

class TimesTenEnv
{
  public:
  	static 	TTStatus			ttStatus;
 	static  void setDBLogin(string sConnectstr);
  	static  TTConnection *getDBConn(bool bReconnect = false);
  	static void commit();
	static void rollback();
	static void disconnect();
  private:
  	static 	TTConnection	ttConn;
  	static 	string			sConnStr;
  	static	void			getMMDBInfo();
};//È«¾Ö¾²Ì¬Àà

class TimesTenConn
{
  public:
  	TTStatus			ttStatus;
 	void setDBLogin(string sConnectstr);
  	TTConnection *getDBConn(bool bReconnect = false);
  	void commit();
	void rollback();
	void disconnect();
  private:
  	TTConnection	ttConn;
  	string			sConnStr;
  	void			getMMDBInfo();
};


class TimesTenCMD : public TTCmd
{
	public:
		TTStatus	ttStatus;
		TimesTenCMD(TTConnection * pttConnect);
		TimesTenCMD(TTConnection * pttConnect,TTStatus *pttStatus);
		~TimesTenCMD();
		void Prepare (const char* sqlP, const char*explanationP);
		void Prepare (const char* sqlP);
		void Execute ();
		int  ExecuteImmediate (const char * sqlP);
		int  FetchNext ();
		void setMaxRows (const int nRows);
		int  getMaxRows ();
		void Close ();
		void Drop ();
		void Commit();   //zhangcheng  add
		void setQueryTimeout (const int nSecs);
		void PrepareBatch(const char * sqlP,TTCmd::TTCMD_USER_BIND_LEVEL level, unsigned short batchSize);
	
		int ExecuteBatch(unsigned short numRows);
	private:
		TTConnection 	*pttConnect;
		TTStatus		*pttStatus;
};

#endif

