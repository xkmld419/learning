/*VER: 1*/ 
#define SQLCA_STORAGE_CLASS extern
#define ORACA_STORAGE_CLASS extern

#include "WriteIni.h"
#include <unistd.h>

char* WriteIni::trim(char *sp)
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

char *WriteIni::TruncStr( char *sp , char *sDst , int len)
{
	memset(sDst , 0 ,  32);
	strncpy(sDst , 	sp , len) ;
	sDst[len]=0;
	return sDst;
}

/************************************************
 Check if the Section and Ident exists in the File 
 Section Exists: return 1,
 Section and Ident Exists: return 2,
 else: return 0;
 length1:section到文件头的大小 ;length2:Ident到文件头的大小;
 length3:Ident所在行的长度;length4:Section所在行的长度
************************************************/
int WriteIni::SectionAndIdentCheck(char const *IniFile,char const *Section,char const *Ident,
	int &length1,int &length2,int &length3,int &length4)
{
	char sSect[LINEMAXCHARS];
	char sBuffer[LINEMAXCHARS];
	char sBuffer1[LINEMAXCHARS];        //sBuffer下面会被trim，所以sBuffer备份，进行定位
	FILE *fd;
	int i=0;
	char *p , *pTmp,*pTmp1;;
	char sTmp1[LINEMAXCHARS];

	if(!(fd=fopen(IniFile,"r")))
		return 0;

	sprintf(sSect,"[%s]",Section);
	while(fgets(sBuffer,LINEMAXCHARS,fd)){
		strcpy(sBuffer1,sBuffer);
		trim(sBuffer);
		pTmp = (char *) sBuffer;
		pTmp1 = (char *) sBuffer1;
		/* note or empty line */
		if((sBuffer[0]=='#')||(sBuffer[0]=='\n')){
			continue;
		}
		if(strstr(sBuffer,sSect)){
			i=1;
			length1=ftell(fd);
			p=(char *)strchr(sBuffer1,'\n');
			length4=p-pTmp1+1;
			if(Ident){
			continue;
			}else{
				fclose(fd);
				return i;
			}
		}
		if(i&&(strchr(sBuffer,'['))&&(strchr(sBuffer,']'))){
			fclose(fd);
			return i;
		}
		if ( i&&( p = (char *) strchr( sBuffer, '=' ))  
			&&(strcmp( trim(TruncStr( sBuffer , sTmp1 , p - pTmp))  ,  Ident ) == 0) ){
			i=2;
			length2=ftell(fd);
			p=(char *) strchr( sBuffer1, '\n' );
		    length3=p-pTmp1+1;
		    fclose(fd);
			return i;
			}
		
	}

	fclose(fd);
	return i;
}
/************************************************
 add or update  the Section and Item in the File 
 write success: return 0,
 else  return -1;
************************************************/

int WriteIni::WriteIniString(char const *IniFile, char const *Section, 
char const *Ident, const char  *sItem)
{
	FILE *fp=NULL;
	int i,length;
	char sBuffer[LINEMAXCHARS];
  int len1=0,len2=0,len3=0,len4=0;
	char *pTmp,*p,*q;
	char sTmp1[LINEMAXCHARS];
	char sEnd[FILEMAXSIZE];
	long pos1,pos2,pos3;
  
	i=SectionAndIdentCheck(IniFile,Section,Ident,len1,len2,len3,len4);

	if(!(Section&&Ident))
		return -1;

    if(fp=fopen(IniFile,"r+b"))
	{
		if(!i){
			fseek(fp,0L,SEEK_END);
			length=ftell(fp);
			fseek(fp,length-1,SEEK_SET);
			if(getc(fp)=='\n'){
				fputc('\n', fp);}else{
				fputc('\n', fp);
				fputc('\n', fp);	
			}
			sprintf(sTmp1,"[%s]\n",Section);
			fputs(sTmp1,fp);
			if(Ident){
				sprintf(sBuffer,"%s = %s\n",Ident,sItem);
			}
			fputs(sBuffer,fp);
			fclose(fp);
			return 0;}
		
		if(i==1&&Ident){
			fseek(fp,0L,SEEK_END);
			pos1=ftell(fp);
			fseek(fp,len1,SEEK_SET);
			length=fread(sEnd,1,pos1-len1,fp);
			//printf("%d,%d,%d\n",pos1,length,pos1-len1);
			fseek(fp,len1,SEEK_SET);
			sprintf(sTmp1,"%s = %s\n",Ident,sItem);
			fputs(sTmp1,fp);
			pos2=ftell(fp);
			//printf("%d,%s,%d\n",ftell(fp),sEnd,sizeof(sEnd));
			//fseek(fp,len1+sizeof(sTmp1),SEEK_SET);
			fwrite(sEnd,1,length,fp);
			rewind(fp);
			ftruncate(fileno(fp),length+pos2);
			fclose(fp);
			return 0;
			}
		if(i==2){
				fseek(fp,0L,SEEK_END);
				pos1=ftell(fp);
				fseek(fp,len2,SEEK_SET);
				pos2=ftell(fp);
				if(pos1-pos2){
				length=fread(sEnd,1,pos1-pos2,fp);
				//*(sEnd+ftell(fp)-len2-1)='\0';
				fseek(fp,len2-len3,SEEK_SET);
				sprintf(sTmp1,"%s = %s\n",Ident,sItem);
				fputs(sTmp1,fp);
				pos3=ftell(fp);
				fwrite(sEnd,1,length,fp);
				//fseek(fp,len1+sizeof(sTmp1)+length1,SEEK_SET);
				rewind(fp);
				ftruncate(fileno(fp),length+pos3);				
				}else{
				fseek(fp,len1,SEEK_SET);
				sprintf(sTmp1,"%s = %s\n",Ident,sItem);
				fputs(sTmp1,fp);
				fseek(fp,0,SEEK_SET);
				ftruncate(fileno(fp),len1+strlen(sTmp1));
				//fputc(EOF,fp);
				}
				fclose(fp);
				return 0;
				}
	}
	else{
		fp=fopen(IniFile,"w");
		sprintf(sTmp1,"[%s]\n",Section);
		fputs(sTmp1,fp);
		sprintf(sTmp1,"%s = %s\n",Ident,sItem);
		fputs(sTmp1,fp);
	    fclose(fp);
		return 0;
	}
	return -1;
}

/************************************************
 delete  the Section in the File 
 success: return 0,
 else  return -1;
************************************************/

int WriteIni::DelIniBySection(const char *IniFile, const char *Section)
{
	FILE *fp;
	int i,len1=0,len2=0,len3=0,len4=0;
	char sBuffer[LINEMAXCHARS];
	char sEnd[FILEMAXSIZE];
	char *p,*pTmp;
	int length=0,length1=0,pos1,pos2;

	if(!(fp=fopen(IniFile,"r+b")))
		return -1;

	if(!Section){
		fclose(fp);
		return -1;
	}
  
	i=SectionAndIdentCheck(IniFile,Section,NULL,len1,len2,len3,len4);//len1和len4有值
	if(!i){
		fclose(fp);
		return -1;}
	else if(i==1){
		fseek(fp,len1,SEEK_SET);
		while(fgets(sBuffer,LINEMAXCHARS,fp)){
			pTmp = (char *) sBuffer;
			if((strchr( sBuffer, '[' ) ) && ( strchr( sBuffer, ']' ))){
				p=strchr(sBuffer,'\n');
				length=p-pTmp+1;
			  break;
			}
		}
		//length=0,后面没有段信息
		if(!length){
			rewind(fp);
			ftruncate(fileno(fp),len1-len4);
			fclose(fp);
			return 0;
		}else{
			pos1=ftell(fp);
			fseek(fp,0L,SEEK_END);
			pos2=ftell(fp);
			fseek(fp,pos1-length,SEEK_SET);
			length1=fread(sEnd,1,pos2-pos1+length,fp);
			fseek(fp,len1-len4,SEEK_SET);
			fwrite(sEnd,1,length1,fp);
			rewind(fp);
			ftruncate(fileno(fp),len1-len4+length1);
			fclose(fp);
			return 0;
		}
	}
	fclose(fp);
	return -1;
}


/************************************************
 delete  the Ident in the File 
 success: return 0,
 else  return -1;
************************************************/
int WriteIni::DelIniByIdent(const char *IniFile, const char *Section, const char *Ident)
{
	FILE *fp;
	int i,len1=0,len2=0,len3=0,len4=0;
	int pos1,length;
	char sEnd[FILEMAXSIZE];

	if(!(fp=fopen(IniFile,"r+b")))
		return -1;

	if(!Section||!Ident){
		fclose(fp);
		return -1;}

	i=SectionAndIdentCheck(IniFile,Section,Ident,len1,len2,len3,len4);//len1~len4都有值

	if(i!=2){
		fclose(fp);
		return -1;
	}else{
		fseek(fp,0L,SEEK_END);
		pos1=ftell(fp);
		if(len2==pos1){
			rewind(fp);
			ftruncate(fileno(fp),len2-len3);
			fclose(fp);
			return 0;
		}else{
			fseek(fp,len2,SEEK_SET);
			length = fread(sEnd,1,pos1-len2,fp);
			fseek(fp,len2-len3,SEEK_SET);
			fwrite(sEnd,1,length,fp);
			rewind(fp);
			ftruncate(fileno(fp),pos1-len3);
			fclose(fp);
			return 0;
		}
	}

	fclose(fp);
	return -1;
}
		

WriteIni::WriteIni()
{

}

WriteIni::~WriteIni()
{

}

//int main()
//{
	//WriteIni a;
	//int res;
	
	//res=a.WriteIniString("cc.ini","BILL","username","cdma");
	//res=a.WriteIniString("cc.ini","BILL","cdma11",NULL);
	//res=a.WriteIniString("bb.ini","STATRPT","password","    password    ");
	//res=a.WriteIniString("bb.ini","STATRPT","username","username");
	//res=a.DelIniBySection("bb.ini","STATRPT");
	//res=a.DelIniByIdent("bb.ini","STATRPT","password");
	//printf("%d\n",res);
	/*res=a.WriteIniString("bb.ini","BILL","username","cdma");
	printf("%d\n",res);*/

	//return 0;
//}
