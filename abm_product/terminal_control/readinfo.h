/*VER: 1*/ 
#ifndef READINFO_H
#define READINFO_H

#include "control.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct{
    int iId,iTop,iLeft,iWidth,iHeight;
}RECT;

typedef struct TTFile{
    char *pInfo;
    struct TTFile *pNext;
}TFILE;

typedef struct{
    Control *pUp,*pDown,*pLeft,*pRight,*pTab,*pIndex;
}DIR__L;

int GetDirInfo(TFILE *pHead,int iii,Control *pHandle,DIR__L *dir);
int GetSiteInfo(TFILE *pHead,int iii,RECT *rect);
int FreeFileMem(TFILE *pHead);
int myread(int ii,char WorkBuf[300],char buff[200],int flag);
Control *GetCtlAddr(Control *pHead,int ii);
/* 为版本统一而改 */
int ReadFile(char FileName[],TFILE **pHead);
int ReadFile2(char FileName[],TFILE **pHead);

 
#endif

