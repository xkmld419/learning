#include "DccMsgParser.h"
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

void disp_buf(char *buf, int size)
{
  int i;
  printf("buf [%d] start:\n", size);
  for( i = 0 ; i < size; i ++ )
  {
  	if( i%16 == 0 )
  		printf("\n");
  	printf("%02X ", buf[i]&0xff);
  }
  printf("\nbuf end\n");
}

int main(int argc,char **argv)
{
	DccMsgParser *pDccMsgParser = new DccMsgParser("./telecom/dictionary.xml");
	pDccMsgParser->releaseMsgAcl();
	int fd = open("./telecom/HexEdit.log",O_RDONLY);	
	char buf[8192];
	int len = 0, iolen = 0;
	do{
	  iolen = read(fd,&buf[len],sizeof(buf)-len);
	  len += iolen;
	}while (iolen > 0);
	
	pDccMsgParser->parseRawToApp(buf, len);
	pDccMsgParser->printHeader();      
	pDccMsgParser->getAllAvp(true);
	ACE_UINT64 int64 = 50;
	pDccMsgParser->setAvpValue("Requested-Service-Unit.CC-Money.Unit-Value.Value-Digits",int64);
	char outbuf[8192];
	memset(outbuf,0,sizeof(outbuf));
	iolen = pDccMsgParser->parseAppToRaw(outbuf,sizeof(outbuf));
	disp_buf(outbuf,iolen);
	int num=1;
	if (argc > 1)
		num = atoi(argv[1]);
	struct timeval start_timeval , end_timeval;
	gettimeofday(&start_timeval, NULL);
	int cnt;
	for(cnt = 0 ; cnt < num ; cnt ++ )
	{
		pDccMsgParser->releaseMsgAcl();
		pDccMsgParser->parseRawToApp(outbuf, len);
		//pDccMsgParser->printHeader();
		//pDccMsgParser->getAllAvp(true);
	}
	pDccMsgParser->printHeader();
	pDccMsgParser->getAllAvp(true);
  gettimeofday(&end_timeval, NULL);
  cout << "########PID:[" << getpid() << "],cnt:" << cnt << ",diff_time:[" << ((end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000
  << " ms]-[" << ((end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us],speed:[" <<  (((end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 )/cnt << "ms],[" << (((end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) )/1000 << " us]" << endl;
	delete pDccMsgParser;
	return 0;
}

