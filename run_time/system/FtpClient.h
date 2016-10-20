// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//##ModelId=41DB84030237
class FtpClient {

private:
    //##ModelId=41DCEAC80095
	char m_sIP[30];
    //##ModelId=41DCEAC800F9
	char m_sUser[50];
    //##ModelId=41DCEAC8012B
	char m_sPassword[50];
    //##ModelId=41DCEAC8015D
	char m_sCommand[1024];

public:
    //##ModelId=41DCEA510148
	FtpClient ();
    //##ModelId=41DCEA51015C
	FtpClient (char *ip);
    //##ModelId=41DCEA5101A2
	FtpClient (char *ip, char *user, char *password);
    //##ModelId=41DCEA510242
	~FtpClient ();
    //##ModelId=41DCEA510256
	void setRemoteIP (char *ip);
    //##ModelId=41DCEA51029C
	void setLogin (char *user, char *password);
    //##ModelId=41DCE9A800D7
	void clearCommand ();
    //##ModelId=41DCE9A101BD
	void addCommand (char *command);
    //##ModelId=41DCE9B10288
	void execCommand ();

};

#endif


