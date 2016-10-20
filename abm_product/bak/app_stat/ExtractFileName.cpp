#include "ExtractFileName.h"
#include <string.h>

ExtractFileName::ExtractFileName()
{
	;
}

char * ExtractFileName::GetSepData(char * pData_,int Num,char pSep_)
{
	if (NULL == pData_)
		return NULL;
		
	if(0 == strlen(pData_))
		return NULL;
		
	memset(FILESEP,0,sizeof(FILESEP));
		
	int   i=0; 
	int  icpCount=0;
	int  iNum_=0;
	int j =0;
	i=strlen(pData_);
	char *fn = pData_;
	for(j=0;j <= i;j++)
	{ 
    if(*pData_ == pSep_) 
  	{ 
  		++iNum_;
  		if(iNum_ == Num)
   			break;
   		else
   		{
   			fn=fn+j+1;
   			icpCount=-1;
   		}
   	} 
    ++pData_; 
    ++icpCount;
	}
	
	strncpy(FILESEP,fn,icpCount);
	
	return FILESEP;
}

char * ExtractFileName::GetFileName(char * pData,char pSep)
{
	if (NULL == pData)
		return NULL;
		
	if(0 == strlen(pData))
		return NULL;
		
	memset(FILENAME,0,sizeof(FILENAME));
		
	int   i=0; 
	int j =0;
	i=strlen(pData);
	char *fn = pData;
	char*   fn1=fn+i; 
	for(j=0;j <= i && fn1 >= fn;j++)
	{ 
    if(*fn1 == pSep) 
  	{ 
   		break;
   	} 
    fn1--; 
	}
	
	strncpy(FILENAME,fn1+1,j);
	
	return FILENAME;
}

ExtractFileName::~ExtractFileName()
{
	;
}

char ExtractFileName::FILENAME[1024] = {0};
char ExtractFileName::FILESEP[1024] = {0};

