#include "TAvpParser.h"
#undef DEBUG_TAVPPARSER

#include <string>
#include <iostream>
using namespace std;
#include <string.h>

void getPack(char *buf, char *sPack, int iPackLen)
{
	if (iPackLen < 0)
		return;

	unsigned char buf1[4] = {0};
	buf1[1] = buf[1];
	buf1[2] = buf[2];
	buf1[3] = buf[3];

	int uiLen = buf1[1]*65536+buf1[2]*256+buf1[3];//此为网络序转为字节序修改灵活一些
	char temp[8] = {0};
	for (unsigned int i = 0; i < uiLen; i++)
	{
		memset(temp, 0, 8);
		unsigned char c = (*(buf + i));
		sprintf(temp,"%x%x ", ((c >> 4) & 0xf), (c & 0xf)); 
		strcat(sPack, temp);
	}
}

int main()
{
	char dccStr[1000] = {0};
	TAvpParser avpParser;
	TAvpHead avpHead;
	TAVPObject avpObj;
	string avpLog;
	unsigned int iAvpLen;

	unsigned char dccBuf[20000] = "1|0|(1,0,0,0,0,0,0,0)|272|4|3332503|3332503^^^258:=4^^^263:=HB@025.ChinaTelecom.com;1287559848;3332503^^^283:=025.ChinaTelecom.com^^^264:=HB@025.ChinaTelecom.com^^^296:=001.ChinaTelecom.com^^^415:=0^^^416:=4^^^461:=BillQuery.Payment@010.chinatelecom.com^^^873.80100.80102:=0^^^873.80100.80166:=1^^^873.80100.20608:=22^^^873.80100.80411:=02^^^873.80100.80101:=1";

	avpParser.initDictionary("../../etc/dictionary.xml");


	avpObj.setObjectValue((const char*)dccBuf);

	memset(dccBuf, 0, sizeof(dccBuf));
	string serviceType = "BillQuery.Payment";
	avpParser.setServiceType(serviceType);
	avpParser.BuildBody(&avpObj,dccBuf+20, &iAvpLen);
	avpParser.BuildHead(&(avpObj.avpHead),dccBuf, &iAvpLen);

    char buff[61441] = {0};
    getPack((char*)dccBuf, buff, 61441);
    cout << buff << endl;

	memset(&avpObj.avpHead, 0, sizeof(TAvpHead));
	avpObj.clear();
	avpParser.ParseHead(dccBuf, 20, NULL);
	avpParser.ParseBody(dccBuf+20, 20000, &avpObj);

	avpObj.toCString(dccStr);
	cout << dccStr << endl;

	//avpLog = avpParser.getDccLog();
	//cout << avpLog << endl;
}

