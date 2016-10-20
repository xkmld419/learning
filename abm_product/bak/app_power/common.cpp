#include <ctime>
#include <iostream>
#include <cstring>

#define MaxLineLen   250
void WriteErrLog(char *sSerialNbr,char *sErrMsg);
int GetProfileStr( const char* File, const char* Section, const char* Entry, char* Result );
long SearchSection( FILE*  pfile, const char* Section  );
long SearchEntry( FILE* pfile, const char* Entry, char* Result, int flag );
char* Rtrim( char *str );
char* Ltrim( char *str );
void Alltrim(char *sBuff);
int GetLocalTime(char *sSysTime);
void GetToBssXML(char *sSerialNbr,int iRetCode/*char *sRetCode*/,char *sRetMsg,char *sXMLBuff);

void Alltrim(char *sBuff)
{

	int iFirstChar=0;
	int iEndPos=0;
	int i;
	int iFirstPos=0;
	for(i=0;sBuff[i]!='\0';i++){
		if(sBuff[i]==' '){
			if(iFirstChar==0) iFirstPos++;
		}
		else{
			iEndPos=i;
			iFirstChar=1;
		}
	}
	for(i=iFirstPos;i<=iEndPos;i++)  sBuff[i-iFirstPos]=sBuff[i];
	sBuff[i-iFirstPos]='\0';
	return ;

}

void WriteErrLog(char *sSerialNbr,char *sErrMsg)
{

	char filename[200];
	time_t	clock;
	struct tm *timep;
	FILE	*fp;
	clock = time(NULL);

	timep = (struct tm *)localtime(&clock);
	sprintf(filename,"%s%04d%02d%02d.log","./log/",timep->tm_year+1900,timep->tm_mon+1,timep->tm_mday);
	fp = fopen(filename,"a");
	fprintf(fp,"[%02d:%02d:%02d] %s,%s\n",		\
		timep->tm_hour,timep->tm_min, \
		timep->tm_sec,sSerialNbr,sErrMsg);
	fclose(fp);
	return ;
}

int GetLocalTime(char *sSysTime)
{
        struct tm *tm1;
        time_t t;
        time(&t);
        tm1 = localtime(&t);
/*        printf("%04d年%02d月%02d日%02d时%02d分%02d秒\n",tm1->tm_year+1900,tm1->tm_mon+1,tm1->tm_mday,tm1->tm_hour,tm1->tm_min,tm1->tm_sec);*/
        sprintf(sSysTime,"%04d-%02d-%02d %02d:%02d:%02d",tm1->tm_year+1900,tm1->tm_mon+1,tm1->tm_mday,tm1->tm_hour,tm1->tm_min,tm1->tm_sec);
        return 0;
}

int GetProfileStr( const char* File, const char* Section, const char* Entry, char* Result )
{
  int ret = -1;
  FILE* pfile;

  if ( !File || !strlen( File ) )
    return ret;

  pfile = fopen( File, "a+" );
  if ( pfile != NULL )
  {
    if ( SearchSection( pfile, Section ) )
    {
      if ( SearchEntry( pfile, Entry, Result, 1 ) )
        ret = 1;
    }
  }
  fclose( pfile );
  
  Rtrim(Result);
  return ret;
}

long SearchSection( FILE* pfile, const char* Section  )
{
  long Found = 0;
  char* pcTmp;
  char* pcSec;

  if ( !Section || !strlen( Section ) )
    return Found;
  pcTmp = (char*)malloc( MaxLineLen );
  pcSec = (char*)malloc( strlen(Section)+3 );
  strcpy( pcTmp, Section );
  Ltrim( pcTmp );
  Rtrim( pcTmp );
  sprintf( pcSec, "[%s]", pcTmp );

  if( pfile != NULL )
  {
    while( ! feof( pfile ) && !Found )
    {
      fgets ( pcTmp, MaxLineLen-1, pfile );
      if ( strstr( pcTmp, pcSec ) == pcTmp )
        Found = ftell( pfile );
    }
  }

  free( pcTmp );
  free( pcSec );
  return Found;
}

long SearchEntry( FILE* pfile, const char* Entry, char* Result, int flag  )
{
  long Found,temp,len;
  int Out;
  char* pcTmp;
  char *pcTmpResult;
  
  Found = 0;
  if ( !Entry || !strlen( Entry ) )
    return Found;
  Out = -1;
  pcTmp = (char*)malloc( MaxLineLen );

  if( pfile != NULL )
  {
    while( !Found && (Out==-1) && ! feof( pfile ) )
    {
      temp = ftell( pfile );   /*定位*/
      
      pcTmp[0] = 0;
      fgets ( pcTmp, MaxLineLen-1, pfile );
      Ltrim( pcTmp );
      pcTmpResult = strstr( pcTmp, Entry );
      if ( pcTmpResult == pcTmp )
      {
        pcTmpResult += strlen( Entry );
        Ltrim( pcTmpResult );
        if ( *pcTmpResult == '=' )
        {
          pcTmpResult++;
          Ltrim( pcTmpResult );
          if ( flag == 1 )
          {
            strcpy( Result, pcTmpResult );
					  len = strlen( Result );
					  while ( len > 0 && ( Result[len-1] == '\n' || Result[len-1] == '\r' || Result[len-1] == ' ' ) )
					  {
						  Result[len-1] = 0;
						  len -= 1;
					  }
				  }
          Found = temp;
        }
      }
      else if ( pcTmp[0] == '[' )
        Out = 1;
    }
  }
  free( pcTmp );
  return Found;
}

char* Ltrim( char *str )
{
  char *pcS = str;
  char *pcD = str;
  int i = 0;
  while ( *pcS )
  {
    if ( *pcS == ' ' && i == 0 )
      pcS++;
    else
    {
      if ( *pcS != ' ' )
        i = 1;
      *pcD++ = *pcS++;
    }
  }
  *pcD = 0;
  return str;
}
char* Rtrim( char *str )
{
  int k,j;
  
  j = ( int )strlen( str ) - 1;
  for ( k = j ; k >= 0 ; k-- )
  {
    if ( str[k] == ' ' )
      str[ k ] = 0 ;
    else
      k = -1 ;
  }
  return str;
}
void GetToBssXML(char *sSerialNbr,int iRetCode/*char *sRetCode*/,char *sRetMsg,char *sXMLBuff)
{
	char sSysDate[30];
	int  iAutoFlag;
	GetLocalTime(sSysDate);
	Alltrim(sSysDate);
	/*
	if ( strcmp(sRetCode,"0")== 0 )
	    iAutoFlag=0;
	else    
	    iAutoFlag=1;*/
	sprintf(sXMLBuff,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<BssReplys>\n\
	<BssReplyInteOrder>\n\
		<InteOrderId>%s</InteOrderId>\n\
		<AutoFlag>1</AutoFlag>\n\
		<RC>%d</RC>\n\
		<Memo>%s</Memo>\n\
		<OrgId></OrgId>\n\
		<StaffId></StaffId>\n\
		<FinishDate>%s</FinishDate>\n\
	</BssReplyInteOrder>\n\
</BssReplys>\n",sSerialNbr,/*iAutoFlag,*/iRetCode/*sRetCode*/,sRetMsg,sSysDate);
   Alltrim(sXMLBuff);
	return;
}

