#ifndef _PUBLIC_H_
#define _PUBLIC_H_
#include <stdio.h>
#include <strings.h>

#define DB_KEY "12345678"

#define COMMIT		1 << 0
#define ROLLBACK	1 << 1
#define ORA_DB		1 << 2
#define MM_DB			1 << 3

extern "C"
{
extern void  writeErrMsg( int line, char *filename,int iOraCode = 0);
extern long  cslreturn(int rval, long rcode, char *data, long len, int iDBControl);
extern long  cslopen(char *sServerName, int iDBControl);
extern long  cslclose(char *sServerName);
extern long  GetConnectInfo(char *sServerName,char *sConnectStr,char *sSecondUser,char *sSecondPasswd);
extern long  ConnectDatabase(char *sServerName);
extern long ConnectTimesTen(char *sServerName);

extern void sql_error( int line , char *filename);

//extern long  wwreturn(int rval,long rcode,char *data,long len);
//extern long  wwopen(char *sServerName);
//extern long  wwclose(char *sServerName);
}

#endif /* _PUBLIC_H_ */

