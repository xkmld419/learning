#include "DccMsgParser.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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
	struct timeval start_timeval , end_timeval;
	long diff_time;
	DccMsgParser *pDccMsgParser = new DccMsgParser("./telecom/dictionary.xml");
	pDccMsgParser->releaseMsgAcl();
	int fd = open("./telecom/HexEdit.log",O_RDONLY);	
	char buf[8192];
	int len = 0, iolen = 0;
	do{
	  iolen = read(fd,&buf[len],sizeof(buf)-len);
	  len += iolen;
	}while (iolen > 0);
	
	gettimeofday(&start_timeval, NULL);		
	pDccMsgParser->parseRawToApp(buf, len);	
	gettimeofday(&end_timeval, NULL);		
	diff_time = (end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ;
	printf("parseRawToApp time=%ld\n",diff_time);
	
	//pDccMsgParser->printHeader();      
	//pDccMsgParser->getAllAvp(true);

	ACE_UINT64 int64 = 0;
	gettimeofday(&start_timeval, NULL);
	pDccMsgParser->getAvpValue("Requested-Service-Unit.CC-Money.Unit-Value.Value-Digits",int64);
	gettimeofday(&end_timeval, NULL);
	diff_time = (end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ;
	printf("getAvpValue time=%ld\n",diff_time);		
	
	int64 = 50;
	gettimeofday(&start_timeval, NULL);		
	pDccMsgParser->setAvpValue("Requested-Service-Unit.CC-Money.Unit-Value.Value-Digits",int64);
	gettimeofday(&end_timeval, NULL);
	diff_time = (end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ;
	printf("setAvpValue time=%ld\n",diff_time);
	
	char outbuf[8192];
	memset(outbuf,0,sizeof(outbuf));
	
	gettimeofday(&start_timeval, NULL);
	iolen = pDccMsgParser->parseAppToRaw(outbuf,sizeof(outbuf));
	gettimeofday(&end_timeval, NULL);		
	diff_time = (end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ;
	printf("parseAppToRaw time=%ld\n",diff_time);
		
		
	//disp_buf(outbuf,iolen);
	pDccMsgParser->releaseMsgAcl();
	pDccMsgParser->parseRawToApp(outbuf, len);
	//pDccMsgParser->printHeader();      
	//pDccMsgParser->getAllAvp(true);
	delete pDccMsgParser;
	return 0;
}

