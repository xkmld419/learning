#include "BaseFunc.h"
//#include "ReadIni.h"
#include <ctype.h>

// 这个修改一下，改为权限控制
/*int FilterChoiceOrder(char *SecStr,char *OptionStr)
{
    int res = -1;
	ReadIni *pRead = new ReadIni();
	char sCodeTmp[12] = {0};
	pRead->readIniString("Droit.ini", SecStr, OptionStr,sCodeTmp,NULL);
	if(strcmp(sCodeTmp,"")!=0){
			res = atoi(sCodeTmp);		
	}
	delete pRead;
	pRead = 0;
    return res;
}*/

void AllTrimN(char sBuf[])
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

char *trimString( char *sp )
{
	char sDest[1024] = {0};
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


void AllTrimNew(char sBuf[])
{	
}


//去掉字符串左右空格
void trimN( string& str )
{
	if(str.size() <= 0)
		return;
	string::iterator pos = str.end()-1;
	for( ; pos >= str.begin() ; )
	{
		if( *pos == ' ' || *pos == '\t' || *pos == '\n' || *pos == '\r' )
		{
			str.erase(pos);
			pos = str.end()-1;
		}
		else
			break;
	}
	
	pos = str.begin();
	for( ; pos < str.end() ; )
	{
		if( *pos == ' '|| *pos == '\t'|| *pos == '\n' || *pos == '\r' )
		{
			str.erase(pos);
			pos = str.begin();
		}
		else
			break;	
	}
}


bool getTagStringN(char *sTagList,vector <string> &vec_tag,char sep)
{
     vec_tag.clear();
     char *p = strchr(sTagList,sep);
     char *p1 = sTagList;
     string *pStr = 0;
     while(p){
        char sTmp2[2048] = {0};
        strncpy(sTmp2,p1,p-p1); 
        pStr = new string(sTmp2);
		//trim(*pStr);
		if(strcmp(sTmp2,"") != 0){
			trimN(*pStr);
			if(pStr->size())
				vec_tag.push_back(*pStr);
		}
		delete pStr;
		pStr = 0;
		p++;
        p1=p;
        p = strchr(p1,sep);
     }
     if(p1 && (strcmp(p1,"")!= 0)) 
     {
        pStr = 0;
        pStr = new string(p1);
		trimN(*pStr);
		if(pStr->size())
        	vec_tag.push_back(*pStr);
        delete pStr;
		pStr = 0;
     }
     return  true;
}
// 小写转大写
void simple_toupper(char *s) 
{
        int i;
        for(i=0;i<strlen(s);i++)
        {
          s[i] = toupper(s[i]);
        }
}

// 0==
int compare(char *s1,char *s2)
{
	if(strlen(s1) != strlen(s2))
		return -1;
	char s1Up[1024] = {0};
	char s2Up[1024] = {0};
	strcpy(s1Up,s1);
	strcpy(s2Up,s2);
	simple_toupper(s1Up);
	simple_toupper(s2Up);
	int res = strcmp(s1Up,s2Up);
	return res;
}

char toUpper(const char ch)
{
	// a - z
  	if (ch >= 97 && ch <= 122)  return ch & 0xDF; // 1101 1111
  	return ch;
}

char toLower(const char ch)
{
	// A - Z
 	if (ch >= 65 && ch <= 90)  return ch | 0x20; // 0010 0000
 	return ch;
}

bool Chang2Upper(char *sSrc)
{
	 int iLen = strlen(sSrc);
	 char sBuf[4096] = {0};
     if(iLen>4096 || sSrc[0] == '\0')
        return false;
 	 char* p = sSrc;
 	 int iPos = 0;	 
 	 while (*p != '\0')
 	 {
	 	  sBuf[iPos++]=toUpper(*p);
	 	  sBuf[iPos] = '\0';
	 	  ++p;
	 }    
     sSrc[0] = '\0';
     strcpy(sSrc,sBuf);	 
	 return true;
}

bool Chang2Lower(char *sSrc)
{
	 int iLen = strlen(sSrc);
	 char sBuf[4096] = {0};
     if(iLen>4096 || sSrc[0] == '\0')
        return false;
 	 char* p = sSrc;
 	 int iPos = 0; 	 
 	 while (*p != '\0')
 	 {
         sBuf[iPos++]=toLower(*p);
	     sBuf[iPos] = '\0';
	     ++p;
	 }    
     sSrc[0] = '\0';
     strcpy(sSrc,sBuf);	 
	 return true;
}

//匹配通配符,做字符匹配操作,匹配成功返回true,不成功返回false
//只认 * ? 为通配符
bool MaskString ( char*   i_sName,char*   i_sMask )
{

    char    sTempFile[300];
    int     i, j, first[300], second[300];

    strncpy( sTempFile, i_sName, 300 );

    int     iNameLen = strlen( sTempFile );

    int     iWildcardLen = strlen( i_sMask );
    char    sTemp[100];

    strncpy( sTemp, i_sMask, 100 );

    for ( i = 0; i <= iWildcardLen; i++ )
    {
        second[i] = 0;
    }

    second[0] = 1;
    j = 1;
    while ( j <= iWildcardLen && sTemp[j - 1] == '*' )
    {
        second[j++] = 1;

    }

    for ( i = 1; i <= iNameLen; i++ )
    {

        for ( j = 0; j <= iWildcardLen; j++ )
        {
            first[j] = second[j];   /* 将后行内容移至前一行，准备计算新的后行 */
        }

        for ( j = 0; j <= iWildcardLen; j++ )
        {
            second[j] = 0;          /* 后行新值置初值 */
        }

        for ( j = 1; j <= iWildcardLen; j++ )
        {   /* 计算新的后行 */
            if (
                (
                    (sTemp[j - 1] == '*')
                &&  (first[j] == 1 || second[j - 1] == 1)
                )
            ||  (
                    (
                        (sTemp[j - 1] == '*')
                    ||  (sTemp[j - 1] == '?')
                    ||  (sTemp[j - 1] == sTempFile[i - 1])
                    ) && (first[j - 1] == 1)
                ) )
            {
                second[j] = 1;
            }
            else
            {
                second[j] = 0;
            }
        }
    }

    /*根据second  的iFixLen 字段来进行判断，如果是1，那么一定可以匹配，如果是0，就是匹配错误。*/
    if ( second[iWildcardLen] == 0 )
    {
        return false;
    }else{
        return true;
    }
}
static long gpdigChAntiDeal[256] = {0};
void InitDigChAntiTable()
{
    long c;
    for(c=0;c<=255;c++)
    {
    	if(isdigit(c)){
    		gpdigChAntiDeal[c]=c-'0';
    	}
    	else 	{
    		gpdigChAntiDeal[c]=c%10;
    	}
    }
}

void HashCharsToNum(char srcKey[18],char desKey[18])
{
    
    InitDigChAntiTable();
    long iStrLen=strlen(srcKey); 
    for(long i=0;i<iStrLen;i++)
    {      
      sprintf(desKey+i,"%ld",gpdigChAntiDeal[(unsigned char )srcKey[i]]);
    }
}
