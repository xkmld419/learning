/*VER: 1*/ 
#define MAX_ITEMS  300
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define  MAX_LEN   500

typedef struct TControl{
    int      iId;
    int      iTop,iLeft,iWidth,iHeight;   /* 控件的位置与大小 */  
    char     sType[20];                       /* 控件的类别 */
    int      bFrame;                      /* 0:不要边框 1: 要边框 */
    char     sHotKey[10];			  /* 控件的热键  2001.2.5 增加 */
    char     sAdditional[20];                 /* 控件的其它特点 */
                                          /*  在EDIT: 表示输入类型 
						      可选：AMOUNT,NUMERIC,NUMBER,PASSWD,STATIC,STRING
					      在LIST: 添入list中的 ITEMS 数目  
    					  */     
    int      bSingle;                     /* LIST控件使用 1:单选  其他:复选  */
    char     sData[MAX_ITEMS];            /* 控件的串空间 */
    char     **pData;                     /* 控件的指针型指针 */
    char     sIndex[30];
    char     sTab[30];
    char     sUp[30];
    char     sDown[30];
    char     sLeft[30];
    char     sRight[30];
    struct TControl *pIndex;
    char     sKeyPress[30];                   /* 控件的按键触发事件 添加方式 。如返回等于0则不运行控件提供的键盘事件*/
    char     sDesc[50];
} Control;

FILE *FileOpen(char *File,int ii,char flag);
void PreCreate( Control *pHandle );
void BuildJiemian(FILE *fp,Control *pCtl,char *bb);
void BuildFunction(FILE *fp,Control *pCtl,char *bb);
void BuildHeadFile(FILE *fp,Control *pCtl,char *bb);
void ReadToControl( Control *tmp1, char WorkBuf[]);
int ReadField(int ii,char WorkBuf[MAX_LEN],char buff[]);
int CheckFile(char *str);
int g_date(int day[3]);


