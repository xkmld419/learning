/*VER: 1*/ 
#define SQLCA_STORAGE_CLASS extern
#define ORACA_STORAGE_CLASS extern

#define _STRING_MAX 128

#include "ReadIni.h"

char * ReadIni::trim( char *sp )
{
	char sDest[LINEMAXCHARS];
	char *pStr;
	int i = 0;

	if ( sp == NULL )
		return NULL;

	pStr = sp;
	while ( *pStr == ' ' || *pStr == '\t' )  pStr ++;
	strcpy( sDest, pStr );

	i = strlen( sDest ) - 1;
	while((i>=0)&&(sDest[i] == ' ' || sDest[i] == '\t' || sDest[i] == '\r' ) )
	{
		sDest[i] = '\0';
		i -- ;
	}

	strcpy( sp, sDest );

	return ( sp );
}

char *ReadIni::TruncStr( char *sp , char *sDst , int len)
{
	memset(sDst , 0 ,  32);
	strncpy(sDst , 	sp , len) ;
	return sDst;
}
	
char *ReadIni::SubStr( char *sp , char *sDst , int len)
{
	memset(sDst , 0 , 32);
	strncpy(sDst , 	sp , len) ;
	return sDst;
}

/************************************************
 Check if the Section exists in the File 
 Exists: return 1,
 else: return 0;
************************************************/
int ReadIni::SectionCheck(char *IniFile,char *Section)
{
	char sSect[LINEMAXCHARS];
	char sBuffer[LINEMAXCHARS];
	FILE *fd;

	if(!(fd=fopen( IniFile, "r" )))
		return 0;
	sprintf(sSect,"[%s]",Section);
	while ( fgets(sBuffer,LINEMAXCHARS,fd)){
		trim( sBuffer );
		/* note or empty line */
		if ( ( sBuffer[0] == '#' ) || ( sBuffer[0] == '\n' ) ){
			continue;
		}
		/* into the section */
		if(strstr(sBuffer,sSect)){
			fclose(fd);
			return 1;
		}
	}
	fclose(fd);
	return 0;
}

/* Read from system ini file parameter */
char *ReadIni::readIniString( char const *IniFile, char const *Section, 
char const *Ident, char  *sItem, char *defstr )
{
	FILE *fd;
	char sBuffer[LINEMAXCHARS], sSect[LINEMAXCHARS], *p , sTmpBuffer[32];
	int  nAct, nLen;
	char *pTmp;
	char sTmp1[LINEMAXCHARS];


	if ( sItem == NULL )
		return( NULL );

	if ( !( fd = fopen( IniFile, "r" ) ) )
		return( NULL );

	nAct = 0;
	if ( defstr == NULL )
		strcpy( sItem, "" );
	else
		strncpy( sItem, defstr, sizeof(sItem)-1);

	strcpy( sSect, "[" );
	strcat( sSect, Section );
	strcat( sSect, "]" );

	memset(sTmpBuffer , 0 , 32);

	while ( fgets( sBuffer, LINEMAXCHARS, fd ) )	{
		trim( sBuffer );
		pTmp = (char *) sBuffer;
		/* note or empty line */
		if ( ( sBuffer[0] == '#' ) || ( sBuffer[0] == '\n' ) )
			continue;

		/* into back section of the section */
		if ( nAct && ( strchr( sBuffer, '[' ) ) && ( strchr( sBuffer, ']' ) ) )
			break;

		/* into the section */
		if ( !nAct && strstr( sBuffer, sSect ) ){
			nAct = 1;
			continue;
		}

		/* front of the section */
		if ( !nAct )
			continue;

		if ( ( p = (char *) strchr( sBuffer, '=' ))  &&  (strcmp( trim(TruncStr( sBuffer , sTmp1 , p - pTmp))  ,  Ident ) == 0) )
		{
			nLen = 0;
			if ( p = ( char * )strchr( sBuffer, '#' ) )
				nLen = p - sBuffer + 1;
			nLen = ( nLen? nLen : strlen( sBuffer ) );
			sBuffer[nLen-1] = '\0';
			p = ( char * )strchr( sBuffer, '=' ) ;
			nAct = 2 ;
			break;
		}
	}
	fclose( fd );

	if ( nAct == 2 ){
		if (strlen(p+1) >= _STRING_MAX) {
			return NULL;	
		}
		strncpy( sItem, trim( p + 1 ), _STRING_MAX-1); //这里最多COPY 127个字节!!!
	}
	else if( defstr == NULL )
		return( NULL );

	return( sItem );
}



int ReadIni::readIniInteger( char const *IniFile, char const *Section, 
char const *Ident, int defint )
{
	char 	defstr[LINEMAXCHARS], sItem[LINEMAXCHARS], *ptr;

	sprintf( defstr, "%d", defint );
	ptr = readIniString( IniFile, Section, Ident, sItem, defstr );
	if ( ptr == NULL )
		return( defint );
	else
		return( atoi( sItem ) );
}

unsigned int ReadIni::readIniUInteger( char const *IniFile, 
char const *Section, char const *Ident, int defint )
{
	char 	defstr[LINEMAXCHARS], sItem[LINEMAXCHARS], *ptr;
	unsigned int	i;

	sprintf( defstr, "%d", defint );
	ptr = readIniString( IniFile, Section, Ident, sItem, defstr );
	if ( ptr == NULL )
		return( defint );
	else
		sscanf(sItem,"%u",&i);
	return( i );
}


/* Read from system ini file parameter */
char *ReadIni::readIniStr( FILE *fd, char *Section, char *Ident, char *sItem, char *defstr )
{
	char sBuffer[LINEMAXCHARS], sSect[50], *p;
	int  nAct, nLen;
	long oldpos;

	if ( sItem == NULL || fd == NULL )
		return( NULL );

	nAct = 0;
	if ( defstr == NULL )
		strcpy( sItem, "" );
	else
		strcpy( sItem, defstr );

	strcpy( sSect, "[" );
	strcat( sSect, Section );
	strcat( sSect, "]" );

	oldpos = ftell( fd );
	fseek( fd, 0L, SEEK_SET );
	while ( fgets( sBuffer, LINEMAXCHARS, fd ) ){
		trim( sBuffer );
		/* note or empty line */
		if ( ( sBuffer[0] == '#' ) || ( sBuffer[0] == '\n' ) )
			continue;

		/* into back section of the section */
		/* Modify by LIU Peng[2000-9-22] add judgement to # and = before [] */
		if ( nAct && ( strchr( sBuffer, '[' ) ) && ( strchr( sBuffer, ']' ) ) ){
			if( ( strchr(sBuffer,'[') < strchr(sBuffer,'#') )
				||( strchr(sBuffer,'[') < strchr(sBuffer,'=') )  ){
				break;
			}
		}

		/* into the section */
		if ( !nAct && strstr( sBuffer, sSect ) ){
			nAct = 1;
			continue;
		}

		/* front of the section */
		if ( !nAct )
			continue;

		if ( p=strstr(sBuffer, Ident) ){
			if (  (*(p+strlen(Ident))!=' ')
				&&(*(p+strlen(Ident))!='\t')
				&&(*(p+strlen(Ident))!='=') )
			  continue;
			nLen = 0;
			if ( p = ( char * )strchr( sBuffer, '#' ) )
				nLen = p - sBuffer + 1;

			nLen = ( nLen? nLen : strlen( sBuffer ) );
			sBuffer[nLen-1] = '\0';
			if ( p = ( char * )strchr( sBuffer, '=' ) )
			{
				nAct = 2;
				break;
			}
		}
	}

	fseek( fd, oldpos, SEEK_SET );
	if ( nAct == 2 )
		strcpy( sItem, trim( p + 1 ) );
	else{
		if ( defstr == NULL ) return( NULL );
	}

	return( sItem );
}

int ReadIni::readIniInt( FILE *fd, char *Section, char *Ident, int defint )
{
	char 	defstr[LINEMAXCHARS], sItem[LINEMAXCHARS], *ptr;

	sprintf( defstr, "%d", defint );
	ptr = readIniStr( fd, Section, Ident, sItem, defstr );
	if ( ptr == NULL )
		return( defint );
	else
		return( atoi( sItem ) );
}

int ReadIni::readIniUInt( FILE *fd, char *Section, char *Ident, int defint )
{
	char 	defstr[LINEMAXCHARS], sItem[LINEMAXCHARS], *ptr;
	unsigned int	uint;

	sprintf( defstr, "%d", defint );
	ptr = readIniStr( fd, Section, Ident, sItem, defstr );
	if ( ptr == NULL )
		return( defint );
	else{
		sscanf(ptr,"%d",&uint);
		return( uint );
	}
}

/*
main( )
{
	char sItem[LINEMAXCHARS], *ptr;
	int  nRet;

	strcpy( sItem, "" );
	ptr = readIniString( "asdf", "Message", "MsgInKey", sItem, "asdf" );
	if ( ptr == NULL )
		printf( "return null\n " );
	else
		printf( "return string: %s\n", sItem );

	nRet = readIniInteger( "sdfas", "Message", "MsgInKey", 0 );
	printf( "return integer: %d\n", nRet );

	ptr = readIniString( "sys.ini", "DATABASE", "UserID", sItem, "" );
	if ( ptr == NULL )
		printf( "return null . \n" );
	else
		printf( "return string: %s\n", sItem );

	nRet = readIniInteger( "sys.ini", "MESSAGE", "MsgInKey", 0 );
	printf( "return : %d\n", nRet );

	return 0;
}
*/

ReadIni::ReadIni()
{

}

ReadIni::~ReadIni()
{

}

