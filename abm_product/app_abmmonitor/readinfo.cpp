/*VER: 1*/ 
#include "readinfo.h"

int GetDirInfo(TFILE *pHead,int iii,Control *pHandle,DIR__L *dir)
{
    int i=0,ii;TFILE *pTmp;
    char buff[200];   
    pTmp = pHead;
    for(i=0;i<iii;i++)if(pTmp!=NULL)pTmp = pTmp->pNext;
 
    ii=myread( 0,pTmp->pInfo,buff,0); 
    ii=myread(ii,pTmp->pInfo,buff,0); 
    ii=myread(ii,pTmp->pInfo,buff,0);
    ii=myread(ii,pTmp->pInfo,buff,0);
    ii=myread(ii,pTmp->pInfo,buff,0);
    ii=myread(ii,pTmp->pInfo,buff,0);
    ii=myread(ii,pTmp->pInfo,buff,1);dir->pUp=GetCtlAddr(pHandle,atoi(buff));
    ii=myread(ii,pTmp->pInfo,buff,1);dir->pDown=GetCtlAddr(pHandle,atoi(buff));
    ii=myread(ii,pTmp->pInfo,buff,1);dir->pLeft=GetCtlAddr(pHandle,atoi(buff));
    ii=myread(ii,pTmp->pInfo,buff,1);dir->pRight=GetCtlAddr(pHandle,atoi(buff));
    ii=myread(ii,pTmp->pInfo,buff,1);dir->pTab=GetCtlAddr(pHandle,atoi(buff));
    ii=myread(ii,pTmp->pInfo,buff,1);dir->pIndex=GetCtlAddr(pHandle,atoi(buff));

    return 0;

}

int GetSiteInfo(TFILE *pHead,int iii,RECT *rect)
{
    int i=0,ii;TFILE *pTmp;
    char buff[200];   
    pTmp = pHead;
    for(i=0;i<iii;i++)if(pTmp!=NULL)pTmp = pTmp->pNext;
 
    ii = myread( 0,pTmp->pInfo,buff,1); rect->iId     = atoi(buff);
    ii = myread(ii,pTmp->pInfo,buff,0); 
    ii = myread(ii,pTmp->pInfo,buff,1); rect->iTop    = atoi(buff);
    ii = myread(ii,pTmp->pInfo,buff,1); rect->iLeft   = atoi(buff);
    ii = myread(ii,pTmp->pInfo,buff,1); rect->iWidth  = atoi(buff);
    ii = myread(ii,pTmp->pInfo,buff,1); rect->iHeight = atoi(buff);

    return 0;
}
/*
TFILE * ReadFile(char FileName[])
{   FILE *fp;
    char WorkBuf[300];int firstflag=1;
    TFILE *pHead,*tmp1,*tmp2;
    if ((fp=fopen( FileName,"r" ))==NULL) {
	    printf("Cannot open %s\n",FileName);
	    exit(1);
    }

    while( fgets(WorkBuf,300,fp)!=NULL ) {
	if(WorkBuf[0]=='#')continue;
	tmp1 = (TFILE *)malloc(sizeof(TFILE));
	tmp1->pInfo = (char *)malloc(strlen(WorkBuf));
	memcpy(tmp1->pInfo,WorkBuf,strlen(WorkBuf));
	tmp1->pNext = NULL;
	if(firstflag){ pHead = tmp1 ; tmp2 = tmp1; firstflag = 0; }
	if(!firstflag)tmp2->pNext = tmp1;
	tmp2=tmp1;
    }

   fclose(fp);
   return (TFILE *)pHead;
}
*/
 
int FreeFileMem(TFILE *pHead)
{
    TFILE *tmp1,*tmp2;
    tmp1 = pHead; tmp2 = pHead->pNext;
    while( tmp2!=NULL ){ 
	free(tmp1->pInfo);
	free(tmp1);
	tmp1=tmp2; 
	tmp2=tmp2->pNext;
    }
    free(tmp1->pInfo);
    free(tmp1);

    return 0;

}
int myread(int ii,char WorkBuf[300],char buff[200],int flag)
{
    int i=0,j=0,k=0,m=199; /* 200-1 */
    while( WorkBuf[ii+i] != '|'&&WorkBuf[ii+i]!=0 )i++;
    if( !flag ) return ii+i+1;  /* 如果只是定位 */
    memset(buff,0,200);
    memcpy(buff,WorkBuf+ii,i);
    while( buff[j]==0x20 )j++;
    k=strlen( buff );
    if( j>0 ){
	for( m=0;m<k-j;m++) 
	    buff[m]=buff[j+m];
    } else {
        j = k-1;
        while( j>0&&buff[j]==0x20 ){
    	    buff[j] = 0;
    	    j--;
        }
    }
    for(;m<200;m++) buff[m]=0;
    return ii+i+1;
}
Control *GetCtlAddr(Control *pHead,int ii)
{
    Control *tmp;
    if( ii < 0 ) return NULL;
    tmp = pHead;
    while( tmp!=NULL ){
        if( tmp->iId == ii )return (Control *)tmp;
    }
    return NULL;
}
/* 为版本统一而改 */
int ReadFile(char FileName[],TFILE **pHead)
{  
    FILE *fp;
    char WorkBuf[300];int firstflag=1;
    TFILE *tmp1,*tmp2;
    if ((fp=fopen( FileName,"r" ))==NULL) 
    {
    	printf("Cannot open %s\n",FileName);
	exit(1);
    }
    while( fgets(WorkBuf,300,fp)!=NULL)
    {
	if(WorkBuf[0]=='#')continue;
	tmp1 = (TFILE *)malloc(sizeof(TFILE));
	tmp1->pInfo = (char *)malloc(strlen(WorkBuf));
	memcpy(tmp1->pInfo,WorkBuf,strlen(WorkBuf));
	tmp1->pNext = NULL;
	//tmp2 = NULL;
	//tmp2 ->pNext = NULL;
	if(firstflag)
	{ 
	   *pHead = tmp1 ;
	   firstflag = 0; 
	}
	else
	{
	   tmp2->pNext=tmp1;
	}
	tmp2=tmp1;
    }
    fclose(fp);
    return 1;
}

int ReadFile2(char FileName[],TFILE **pHead)
{  
    FILE *fp;
    char WorkBuf[300];int firstflag=1;
    TFILE *tmp1,*tmp2;
    if ((fp=fopen( FileName,"r" ))==NULL) 
    {
    	printf("Cannot open %s\n",FileName);
	exit(1);
    }
    while( fgets(WorkBuf,300,fp)!=NULL)
    {
	if(WorkBuf[0]=='#')continue;
	tmp1 = (TFILE *)malloc(sizeof(TFILE));
	tmp1->pInfo = (char *)malloc(strlen(WorkBuf));
	memcpy(tmp1->pInfo,WorkBuf,strlen(WorkBuf));
	tmp1->pNext = NULL;
	//tmp2 ->pNext = NULL;
	//tmp2 = NULL;
	if(firstflag)
	{ 
	   *pHead = tmp1 ;
	   firstflag = 0; 
	}
	else
	{
	   tmp2->pNext=tmp1;
	}
	tmp2=tmp1;
    }
    fclose(fp);
    return 1;
}
