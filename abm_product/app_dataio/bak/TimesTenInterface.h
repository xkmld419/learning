#ifndef _TIMESTEN_INTERFACE_INCLUDE_HEADER
#define _TIMESTEN_INTERFACE_INCLUDE_HEADER

#include <ttclasses/TTInclude.h>

#include <string>
using namespace std;

#define DEFINE_TTCMD_BYCONN(x,pttConn) 					        TimesTenCMD x (pttConn)

#define DEFINE_TTCMD_BYTTCONN(x,TTCONN)					        TimesTenCMD x (TTCONN.getDBConn())
#define DEFINE_TTCMD_BYTTCONN_EXTSTAT(x,TTCONN,pttStatus)		TimesTenCMD x (TTCONN.getDBConn(),pttStatus)
#define DEFINE_TTCMDPOINT_BYTTCONN(x,TTCONN)     x = new TimesTenCMD (TTCONN->getDBConn());

class TimesTenConn
{
public:
    TTStatus ttStatus;
    void setDBLogin(string sConnectstr);
    TTConnection *getDBConn(bool bReconnect = false);
    void commit();
    void rollback();
    void disconnect();
private:
    TTConnection ttConn;
    string sConnStr;
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

