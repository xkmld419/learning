/*VER: 1*/ 
/* Copyright (c) 2001-<2003> Wholewise, All rights Reserved */
/* #ident "@(#)password.h	1.0	2005/03/15	<AutoCreate>" */
#ifndef __ACCESS_CONFIG_H__
#define __ACCESS_CONFIG_H__

#include "iostream"
using namespace std;

FILE *fp=NULL;
string sFileName="/cbilling/app/petri/ori_bak/HSS/src/bill/src/app_access/test.config";
char *delim ="|";
char sLineContxt[1024]={0};

conFind(char *KEY,char *DSN,char *conSQL)
{
	bool flag=false;
	fp=fopen(m_sFileName,"r");
	while(1)
	    {
	        if(fgets(sLineContxt, 1024, fp)==NULL)
	        {
	            cout<<"文件处理结束"<<endl;
	            break;
	        }
		      int iNum =1;
		      char sCheckData[1024]={0};
		      cp = strtok(sLineContxt,delim);
		        //sDate
		      strcpy(sCheckData,cp);
		      
		      cp=strtok(NULL,delim);
		        //sDate
		      strcpy(sCheckData,cp);
		      if (strcmp(sCheckData,KEY)!=0)
		      {
		      		flag=false;
		      }else{
		      	 flag=true;
		      	}
		      cp=strtok(NULL,delim);
		        //sDate
		      strcpy(sCheckData,cp);
		      if (strcmp(sCheckData,DSN)=0&&flag=true)
		      {
		      		flag=true;
		      }else{
		      	 flag=false;
		      	}
		      cp=strtok(NULL,delim);
		      if(flag){
		      	strcpy(conSQL,cp);
		      	return 0;
		      }
	}
	if(!flag){
		      	strcpy(conSQL,NULL);
		      	return -1;
		      }
}


conFind(char *KEY,char *DSN,char *conSQL)
{
	bool flag=false;
	fp=fopen(m_sFileName,"a");
	while(1)
	    {
	        if(fgets(sLineContxt, 1024, fp)==NULL)
	        {
	            cout<<"文件处理结束"<<endl;
	            break;
	        }
		      int iNum =1;
		      char sCheckData[1024]={0};
		      cp = strtok(sLineContxt,delim);
		        //sDate
		      strcpy(sCheckData,cp);
		      
		      cp=strtok(NULL,delim);
		        //sDate
		      strcpy(sCheckData,cp);
		      if (strcmp(sCheckData,KEY)!=0)
		      {
		      		flag=false;
		      }else{
		      	 flag=true;
		      	}
		      cp=strtok(NULL,delim);
		        //sDate
		      strcpy(sCheckData,cp);
		      if (strcmp(sCheckData,DSN)=0&&flag=true)
		      {
		      		flag=true;
		      }else{
		      	 flag=false;
		      	}
		      cp=strtok(NULL,delim);
		      if(flag){
		      	strcpy(conSQL,cp);
		      	return 0;
		      }
	}
	if(!flag){
		      	strcpy(conSQL,NULL);
		      	return -1;
		      }
}


conFind(char *KEY,char *DSN,char *conSQL)
{
	bool flag=false;
	fp=fopen(m_sFileName,"r");
	while(1)
	    {
	        if(fgets(sLineContxt, 1024, fp)==NULL)
	        {
	            cout<<"文件处理结束"<<endl;
	            break;
	        }
		      int iNum =1;
		      char sCheckData[1024]={0};
		      cp = strtok(sLineContxt,delim);
		        //sDate
		      strcpy(sCheckData,cp);
		      
		      cp=strtok(NULL,delim);
		        //sDate
		      strcpy(sCheckData,cp);
		      if (strcmp(sCheckData,KEY)!=0)
		      {
		      		flag=false;
		      }else{
		      	 flag=true;
		      	}
		      cp=strtok(NULL,delim);
		        //sDate
		      strcpy(sCheckData,cp);
		      if (strcmp(sCheckData,DSN)=0&&flag=true)
		      {
		      		flag=true;
		      }else{
		      	 flag=false;
		      	}
		      cp=strtok(NULL,delim);
		      if(flag){
		      	strcpy(conSQL,cp);
		      	return 0;
		      }
	}
	if(!flag){
		      	strcpy(conSQL,NULL);
		      	return -1;
		      }
}

#endif

