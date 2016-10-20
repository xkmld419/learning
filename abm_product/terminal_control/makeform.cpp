/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)makeform.c   1.5     2001/02/09      ruanyongjiang" */

/******************************************************************/
/* 说明： 本程序是用于生成符合libcontrols.a 库调用标准的界面程序  */
/* 使用： makeform  example.rc	        			  */
/* 结果： 生成   example_j.c --界面生成程序                 	  */
/*               example_f.c --控件键盘事件处理对用户的接口 	  */
/*        用户只需在example_f.c相应控件事件中添入事件处理代码 	  */
/******************************************************************/

/* .rc 文件的格式： 详见readme 					  */

#include "makeform.h"

int	 iStaticFlag = 0;

int main(int argc,char *argv[])
{
    FILE *fpr,*fpw_f,*fpw_j,*fpw_h;
    Control *head,*tmp1,*tmp2;
    int firstflag=1;
    char WorkBuf[MAX_LEN],sRcName[30];

    if( argc < 2 ){
	printf("Example: makeform <*.rc> [static]\n");
	exit(1);
    }

    if (argc>=3 && !strcasecmp (argv[2], "STATIC"))
	iStaticFlag = 1;

    strcpy((char *)sRcName, (char *)argv[1]);
    CheckFile( sRcName );
    fpr   = (FILE *) FileOpen ( sRcName, 0, 0);
    fpw_j = (FILE *) FileOpen ( sRcName, 1, 'j');
    fpw_f = (FILE *) FileOpen ( sRcName, 1, 'f');
    fpw_h = (FILE *) FileOpen ( sRcName, 2, 0);
    
    /* 将 .rc 文件内容读到控件的 Index 链中 */
    while( fgets(WorkBuf,MAX_LEN,fpr)!=NULL ) {
	if(WorkBuf[0]=='#')
		continue;
        tmp1 = (Control *)malloc(sizeof(Control));
	tmp1->pIndex = NULL;
        if(firstflag){ 
	    head = tmp1; tmp2 = tmp1;  
	    firstflag=0;
	}
	ReadToControl( tmp1, WorkBuf);
	memset((void *)WorkBuf,0,MAX_LEN);
	if(!firstflag)
	     tmp2->pIndex = tmp1;
	tmp2=tmp1;
    }

    PreCreate(head); /* 预处理链表，设置控件的控制关系 */

    if(fpw_j!=NULL){
	BuildJiemian (fpw_j, head, sRcName);  /* 生成 XXX_j.cpp */
        printf("Makeform: Create %s_j.cpp OK!\n",sRcName);
    }
    if(fpw_f!=NULL){
	BuildFunction (fpw_f, head, sRcName); /* 生成 XXX_f.cpp */ 
        printf("Makeform: Create %s_f.cpp OK!\n",sRcName);
    }
    if(fpw_h!=NULL){
	BuildHeadFile (fpw_h, head, sRcName); /* 生成 XXX.h */ 
        printf("Makeform: Create %s.h OK!\n",sRcName);
    }
    tmp1 = head; tmp2 = head->pIndex;
    while( tmp2!=NULL ){ free(tmp1); tmp1=tmp2; tmp2=tmp2->pIndex;}
    free(tmp1);
    fclose(fpr);
    if (fpw_j != NULL) fclose(fpw_j);
    if (fpw_f != NULL) fclose(fpw_f);

}
FILE *FileOpen(char *File,int ii,char flag)
{   FILE *fp;
    char buff[50]; char ch;
    fflush(stdin);
    if( ii==0 ){
	if ((fp=fopen( File,"r" ))==NULL) {
	    printf("Cannot open %s\n",File);
	    exit(1);
    	}
    }else if (ii==1) {
	sprintf(buff,"%s_%c.cpp",strtok (File, ".") ,flag);
	if ((fp=fopen( buff,"r" ))==NULL) {
	    fp=fopen( buff,"w" );
	} else {
	    fclose( fp );
	    printf("Warning : Exist %s!!! overwrite it(Y/N)?",buff);
	    while( !(ch=='y'||ch=='n'||ch=='Y'||ch=='N') ){
		scanf("%c",&ch);
	        if( ch=='Y'||ch=='y' )
	            { fp=fopen( buff,"w" ); ch=99; break; }
	        if( ch=='N'||ch=='n' )
		    { ch=88; break; }
	    }
	    if( ch == 99 )return (FILE *)fp;
	    if( ch == 88 )return NULL;
	}
    } else {
	sprintf(buff,"%s.h",strtok (File, "."));
	if ((fp=fopen( buff,"r" ))==NULL) {
	    fp=fopen( buff,"w" );
	} else {
	    fclose( fp );
	    printf("Warning : Exist %s!!! overwrite it(Y/N)?",buff);
	    while( !(ch=='y'||ch=='n'||ch=='Y'||ch=='N') ){
		scanf("%c",&ch);
	        if( ch=='Y'||ch=='y' )
	            { fp=fopen( buff,"w" ); ch=99; break; }
	        if( ch=='N'||ch=='n' )
		    { ch=88; break; }
	    }
	    if( ch == 99 )return (FILE *)fp;
	    if( ch == 88 )return NULL;
	}

    }
    return (FILE *)fp;
}
void PreCreate( Control *pHandle )
{
    Control *tmp1;
    tmp1 = pHandle;
    while( tmp1!=NULL ){
	if(atoi(tmp1->sUp)<0)	strcpy(tmp1->sUp,"NULL");
	else 			sprintf(tmp1->sUp,"Ctl%d",atoi(tmp1->sUp));
	if(atoi(tmp1->sDown)<0)	strcpy(tmp1->sDown,"NULL");
	else 			sprintf(tmp1->sDown,"Ctl%d",atoi(tmp1->sDown));
	if(atoi(tmp1->sLeft)<0)	strcpy(tmp1->sLeft,"NULL");
	else 			sprintf(tmp1->sLeft,"Ctl%d",atoi(tmp1->sLeft));
	if(atoi(tmp1->sRight)<0)strcpy(tmp1->sRight,"NULL");
	else 			sprintf(tmp1->sRight,"Ctl%d",atoi(tmp1->sRight));
	if(atoi(tmp1->sTab)<0)	strcpy(tmp1->sTab,"NULL");
	else 			sprintf(tmp1->sTab,"Ctl%d",atoi(tmp1->sTab));
	if(tmp1->pIndex!=NULL)	sprintf(tmp1->sIndex,"Ctl%d",tmp1->pIndex->iId);
	else 			sprintf(tmp1->sIndex,"NULL");

        tmp1 = tmp1->pIndex;
    }

}
void BuildJiemian(FILE *fp,Control *pCtl,char *rc)
{
    int ymd[3],iii;
    Control *tmp1; int block_flag=0;
    g_date(ymd);
    fprintf(fp,"/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */\n");
    fprintf(fp,"/* #ident \"@(#)%s_j.c	1.0	%04d/%02d/%02d	<AutoCreate>\" */\n",rc,ymd[2],ymd[0],ymd[1]);
    fprintf(fp,"#include \"%s.h\"\n\n",rc);
/* MDF 2005.3.4 ruanyj
    tmp1 = pCtl;
    while( tmp1!=NULL ){
	if( strcmp(tmp1->sType,"BOX")&&strcmp(tmp1->sType,"LABEL") )
        fprintf(fp,"extern KEYPRESS %sCtl%dPress(Control *pCtl,int ch,long lParam);\n",rc,tmp1->iId);
        tmp1 = tmp1->pIndex;
    }
    fprintf(fp,"extern KEYPRESS %s_SysHotKeyPress(Control *pCtl,int ch,long lParam);\n",rc);
*/

    fprintf(fp,"\nControl *%s_init()\n{\n",rc);
    fprintf(fp,"    Control *Ctl%d",pCtl->iId);
    tmp1 = pCtl->pIndex;
    while( tmp1!=NULL ){
       fprintf(fp,",*Ctl%d",tmp1->iId);
       tmp1 = tmp1->pIndex;
    }

    if (!iStaticFlag) {
	    fprintf(fp,";\n    RECT rt; TFILE *pHead;\n"); 
	    fprintf(fp,"    ReadFile(\"%s.rc\", (TFILE **)&pHead);\n\n",rc); 
	    tmp1 = pCtl;
	    iii = 0;
	    while( tmp1!=NULL ){
		fprintf(fp,"    GetSiteInfo( pHead,%2d, &rt);\n",iii++);
		fprintf(fp,"    Ctl%-3d = CtlInit(%-8s,rt.iId,rt.iTop,rt.iLeft,rt.iWidth,rt.iHeight);\n",tmp1->iId,tmp1->sType);
		tmp1 = tmp1->pIndex;
	    }
    } else {

	    tmp1 = pCtl;
	    iii = 0;
	    fprintf (fp, ";\n\n");
	    while( tmp1!=NULL ){
		fprintf(fp,"    Ctl%-3d = CtlInit(%-8s, %5d,  %5d,  %5d,  %5d,  %5d);\n", tmp1->iId,tmp1->sType,tmp1->iId, tmp1->iTop, tmp1->iLeft, tmp1->iWidth, tmp1->iHeight);
		tmp1 = tmp1->pIndex;
	    }

    }
    fprintf(fp,"\n");
    tmp1 = pCtl;
    while( tmp1!=NULL ){
        if(strcmp(tmp1->sData,"")) {
            fprintf(fp,"    strcpy( Ctl%-3d->sData , \"%s\" );\n",tmp1->iId,tmp1->sData );
	    block_flag=1;
	}
        tmp1 = tmp1->pIndex;
    }
    if(block_flag){fprintf(fp,"\n");block_flag=0;}
    tmp1 = pCtl;
    while( tmp1!=NULL ){
        if(strcmp(tmp1->sAdditional,"")){
            fprintf(fp,"    Ctl%-3d->iAdditional = %s ;\n",tmp1->iId,tmp1->sAdditional );
	    block_flag = 1;
	}
        tmp1 = tmp1->pIndex;
    }
    if(block_flag){fprintf(fp,"\n");block_flag=0;}

    tmp1 = pCtl;
    while( tmp1!=NULL ){
	if( strcmp(tmp1->sType,"BOX")&&strcmp(tmp1->sType,"LABEL")&&strcmp(tmp1->sAdditional,"STATIC") )
        fprintf(fp,"    Ctl%-3d->pKeyPress = (KEYPRESS)&%sCtl%dPress;\n",tmp1->iId,rc,tmp1->iId);
        tmp1 = tmp1->pIndex;
    }
    fprintf(fp,"\n");

    tmp1 = pCtl;
    while( tmp1!=NULL ){
	if( strcmp(tmp1->sType,"BOX")&&strcmp(tmp1->sType,"LABEL")&&strcmp(tmp1->sAdditional,"STATIC") )
        fprintf(fp,"    Ctl%-3d->pHotKeyPress = (KEYPRESS)&%s_SysHotKeyPress;\n",tmp1->iId,rc);
        tmp1 = tmp1->pIndex;
    }
    fprintf(fp,"\n");

    tmp1 = pCtl;
    while( tmp1!=NULL ){
        if(strcmp(tmp1->sHotKey,"")){
            fprintf(fp,"    Ctl%-3d->iHotKey = %s ;\n",tmp1->iId,tmp1->sHotKey );
	    block_flag = 1;
	}
        tmp1 = tmp1->pIndex;
    }
    if(block_flag){fprintf(fp,"\n");block_flag=0;}
    tmp1 = pCtl;
    while( tmp1!=NULL ){
        if(tmp1->bFrame==1){
            fprintf(fp,"    Ctl%-3d->bFrame  = %d ;\n",tmp1->iId,tmp1->bFrame );
	    block_flag = 1;
	}
        tmp1 = tmp1->pIndex;
    }
    if(block_flag){fprintf(fp,"\n");block_flag=0;}
    tmp1 = pCtl;
    while( tmp1!=NULL ){
        if(!strcmp(tmp1->sType,"LIST")){
            fprintf(fp,"    Ctl%-3d->bSingle = %d ;\n",tmp1->iId,tmp1->bSingle );
	    block_flag = 1;
	}
        tmp1 = tmp1->pIndex;
    }
    if(block_flag){fprintf(fp,"\n");block_flag=0;}
    tmp1 = pCtl;
    while( tmp1!=NULL ){
        fprintf(fp,"    CtlSetDir(Ctl%-5d,%8s,%8s,%8s,%8s,%8s,%8s);\n",tmp1->iId,tmp1->sUp,tmp1->sDown,tmp1->sLeft,tmp1->sRight,tmp1->sTab,tmp1->sIndex);
        tmp1 = tmp1->pIndex;
    }
    fprintf(fp,"\n");
    if (!iStaticFlag)
	fprintf(fp,"    FreeFileMem( pHead );\n\n");

    fprintf(fp,"    %s_entry((Control *)Ctl%d);\n\n",rc,pCtl->iId);
    fprintf(fp,"    return ((Control *)Ctl%d) ;\n}\n\n",pCtl->iId);

    fprintf (fp, "%s::%s()\n{\n\tm_pForm = (Control *) %s_init ();\n\n}\n", rc, rc, rc);
    fprintf (fp, "%s::~%s()\n{\n\tFormKill (m_pForm);\n\tm_pForm = NULL;\n}\n\n", rc, rc);
    fprintf (fp, "int %s::run()\n{\n\tif (FormRun (m_pForm) == FORM_KILL_OK)\n\t\tm_pForm = NULL;\n\n\treturn FORM_KILL_OK;\n}\n\n", rc);

}
void BuildFunction(FILE *fp,Control *pCtl,char *rc)
{
    int ymd[3];
    Control *tmp1;
    g_date (ymd);
    fprintf(fp,"/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */\n");
    fprintf(fp,"/* #ident \"@(#)%s_f.c	1.0	%04d/%02d/%02d	<AutoCreate>\" */\n",rc,ymd[2],ymd[0],ymd[1]);
    fprintf(fp,"#include \"%s.h\"\n\n",rc);
    fprintf(fp,"Control *%s_handle;\n\n",rc);

    fprintf(fp,"int %s_SysHotKeyPress(Control *pCtl, int ch, long lParam)\n",rc);
    fprintf(fp,"{   \n");
    fprintf(fp,"    /* Add your self code here: */\n\n    return ch;\n}\n\n");

    fprintf(fp,"void %s_entry(Control *pHandle) /* %s 创建时触发调用 */\n",rc,rc);
    fprintf(fp,"{   \n");
    fprintf(fp,"    %s_handle = pHandle;\n",rc);
    fprintf(fp,"    /* Add your self code here: */\n\n\n}\n\n");

    tmp1 = pCtl;
    while( tmp1!=NULL ){
	if( strcmp(tmp1->sType,"BOX")&&strcmp(tmp1->sType,"LABEL")&&strcmp(tmp1->sAdditional,"STATIC") ) {
        fprintf(fp,"int %sCtl%dPress(Control *pCtl,int ch,long lParam)\n{",rc,tmp1->iId);
	if( strcmp(tmp1->sDesc,"") )
	    fprintf(fp,"   /* %s的按键消息处理 */",tmp1->sDesc);
	fprintf(fp,"\n");
	fprintf(fp,"    /* Add your self code here: */\n");
	fprintf(fp,"\n\n    return ch;\n}\n");
	}
        tmp1 = tmp1->pIndex;
    }

    fprintf (fp, "\n");



}

void BuildHeadFile(FILE *fp,Control *pCtl,char *rc)
{
    int ymd[3];
    Control *tmp1;
    g_date (ymd);
    fprintf(fp,"/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */\n");
    fprintf(fp,"/* #ident \"@(#)%s.h	1.0	%04d/%02d/%02d	<AutoCreate>\" */\n",rc,ymd[2],ymd[0],ymd[1]);
    fprintf(fp,"#ifndef __%s_h__\n#define __%s_h__\n\n",rc, rc);
    fprintf(fp,"#include \"controls.h\"\n\n");

    fprintf(fp,"class %s {\npublic:\n\t%s ();\n\t~%s ();\n\tint run ();\nprivate:\n\tControl *m_pForm;\n};\n\n", rc, rc, rc);

    fprintf(fp,"Control *%s_init();\n", rc);
    fprintf(fp,"int %s_SysHotKeyPress(Control *pCtl, int ch, long lParam);\n",rc);
    fprintf(fp,"void %s_entry(Control *pHandle);\n",rc);

    tmp1 = pCtl;
    while( tmp1!=NULL ){
	if( strcmp(tmp1->sType,"BOX")&&strcmp(tmp1->sType,"LABEL")&&strcmp(tmp1->sAdditional,"STATIC") ) {
	    fprintf(fp,"int %sCtl%dPress(Control *pCtl,int ch,long lParam);\n",rc,tmp1->iId);
	}
        tmp1 = tmp1->pIndex;
    }

    fprintf (fp, "\n#endif\n\n");
}


void ReadToControl( Control *tmp1, char WorkBuf[MAX_LEN])
{
    int ii;
    char buff[200];    
    ii = ReadField( 0,WorkBuf,buff ); tmp1->iId     = atoi(buff);
    ii = ReadField(ii,WorkBuf,buff ); strcpy(tmp1->sType,buff);
    ii = ReadField(ii,WorkBuf,buff ); tmp1->iTop    = atoi(buff);
    ii = ReadField(ii,WorkBuf,buff ); tmp1->iLeft   = atoi(buff);
    ii = ReadField(ii,WorkBuf,buff ); tmp1->iWidth  = atoi(buff);
    ii = ReadField(ii,WorkBuf,buff ); tmp1->iHeight = atoi(buff);
    ii = ReadField(ii,WorkBuf,buff ); strcpy(tmp1->sUp,buff);
    ii = ReadField(ii,WorkBuf,buff ); strcpy(tmp1->sDown,buff);
    ii = ReadField(ii,WorkBuf,buff ); strcpy(tmp1->sLeft,buff);
    ii = ReadField(ii,WorkBuf,buff ); strcpy(tmp1->sRight,buff);
    ii = ReadField(ii,WorkBuf,buff ); strcpy(tmp1->sTab,buff);
    ii = ReadField(ii,WorkBuf,buff ); strcpy(tmp1->sHotKey,buff);
    ii = ReadField(ii,WorkBuf,buff ); tmp1->bFrame = atoi(buff);
    ii = ReadField(ii,WorkBuf,buff ); strcpy(tmp1->sAdditional,buff);
    ii = ReadField(ii,WorkBuf,buff ); tmp1->bSingle= atoi(buff);
    ii = ReadField(ii,WorkBuf,buff ); strcpy(tmp1->sData,buff);
    ii = ReadField(ii,WorkBuf,buff ); strcpy(tmp1->sDesc,buff);

}
int ReadField(int ii,char WorkBuf[MAX_LEN],char buff[200])
{
    int i=0,j=0,k=0,m=199; /* 200-1 */
    memset(buff,0,200);
    while( WorkBuf[ii+i] != '|'&&WorkBuf[ii+i]!=0&&WorkBuf[ii+i]!=0x0a)i++;
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
int CheckFile(char *str)
{
    char *pt;

    pt = strchr (str, '.') + 1;
    if (!strcmp (pt, "rc"))
	return 0;
    else {
	printf("Filename must be *.rc\n");
	exit(1);
    }
}

int g_date(int day[3])
{
 long clock;
 struct tm *tim;

 time((time_t *)&clock);
 tim=localtime((const time_t *)&clock);

 day[2]=tim->tm_year+1900;
 day[0]=tim->tm_mon+1;
 day[1]=tim->tm_mday;
 return(0);
}

