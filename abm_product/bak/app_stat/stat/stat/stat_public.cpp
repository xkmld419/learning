/*VER: 1*/ 
#include "stat_public.h"

void AllTrim(char sBuf[])
{
	int iFirstChar=0;
	int iEndPos=0;
	int i;
	int iFirstPos=0;
	for(i=0;sBuf[i]!='\0';i++){
		if(sBuf[i]==' '){
			if(iFirstChar==0) iFirstPos++;
		}
		else{
			iEndPos=i;
			iFirstChar=1;
		}
	}
	for(i=iFirstPos;i<=iEndPos;i++)  sBuf[i-iFirstPos]=sBuf[i];
	sBuf[i-iFirstPos]='\0';
	return ;
}
