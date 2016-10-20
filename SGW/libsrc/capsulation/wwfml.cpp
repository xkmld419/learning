/*这是对TUXEDO的FML的封装程序,适宜于TUXEDO环境使用*/
/*************************************
 *  Author: Many                     *
 *  Create Date:     2000/11/26      * 
 *  The last Modify: 2000/11/26      *
**************************************/
/*
#define TUXEDO
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "./wwfml.h"
#include "userlog.h"

/* 定义错误列表 */
MCCI_TUXEDO_FML_ERROR FMLErrNo[27] = {
   	                               {10500,   "FMINVAL"  },
                                   {10501,   "FALIGNERR"},
                                   {10502,   "FNOTFLD"  },
                                   {10503,   "FNOSPACE" },
                                   {10504,   "FNOTPRES" },
                                   {10505,   "FBADFLD"  },
                                   {10506,   "FTYPERR"  },
                                   {10507,   "FEUNIX"   },
                                   {10508,   "FBADNAME" },
                                   {10509,   "FMALLOC"  },
                                   {10510,   "FSYNTAX"  },
                                   {10511,   "FFTOPEN"  },
                                   {10512,   "FFTSYNTAX"},
                                   {10513,   "FEINVAL"  },
                                   {10514,   "FBADTBL"  },
                                   {10515,   "FBADVIEW" },
                                   {10516,   "FVFSYNTAX"},
                                   {10517,   "FVFOPEN"  },
                                   {10518,   "FBADACM"  },
                                   {10519,   "FNOCNAME" },
                                   {10520,   "FMAXVAL"  },
                                   {10521,   "INVALID_EX_METHOD"},
                                   {10522,   "INVALID_EX_VALUES"},
                                   {10523,   "EX_ERROR"         },
                                   {10524,   "数值型的数据取出空值"},
                                   {10525,	 "open file error"},
                                   {10526,	 "error type"}
                       };
                       
/*定义空间扩展策略*/
struct _ex_policy_ {
    int ex_method;
    int ex_values;
}sg_ex_policy={PERCENT,0};

/*************************************
 *  功能: 设置空间扩展策略           *
 *  参数: ex_method： 扩展方式       * 
 *        ex_values： 扩展值         *
 *        value1：    保留           *
 *        value2：    保留           *
 *        value3：    保留           *
 *  返回值：                         *
 *          >0   成功                *
 *          <0   失败                *       
**************************************/
int wwfexpolicy(int ex_method,int ex_values,int value1,int value2,int value3)
{
    if((ex_method!=0)&&(ex_method!=1)) return (0-FMLErrNo[21].ierrno);
    if(ex_values<0) return (0-FMLErrNo[22].ierrno);
    sg_ex_policy.ex_method=ex_method;
    sg_ex_policy.ex_values=ex_values;
    return 0;	
}

/*************************************
 *  功能: 查找指定域的记录数         *
 *  参数: fbfr：    缓冲区           * 
 *        fieldid： 域标识           *
 *  返回值：                         *
 *          >0   记录数              *
 *          <0   失败                *       
**************************************/
/*查找指定域的记录数*/
int wwfnum(WWFBFR *fbfr,WWFLDID fieldid)
{
    int i;
    if((i=(int)Foccur32(fbfr,fieldid))==-1)
       return (0-FMLErrNo[Ferror32].ierrno);
    else
       return i;
}


/*************************************
 *  功能: 把域值存入缓冲区           *
 *  参数: fbfr：      缓冲区地址     * 
 *        fieldid：   域标识         *
 *        oc：        下标           *
 *        value：     数据包         *
 *  返回值：                         *
 *          =0   成功                *
 *          <0   失败                *       
**************************************/
int wwfput(WWFBFR **fbfr,WWFLDID fieldid,WWFLDOCC oc,void *value)
{
    int iRet;
    long lLen,lUnused,lSize;
    WWFBFR *TempBfr,*TempBuf;
    char sLogStr[1000];

    /* LOG */
/*	if(strcmp(Ftype32(fieldid),"string")==0)
    	sprintf(sLogStr,"PUT;WWFLDID:%X;VALUE:%s",fieldid,value);
	else if(strcmp(Ftype32(fieldid),"long")==0)
		sprintf(sLogStr,"PUT;WWFLDID:%X;VALUE:%ld",fieldid,*((long *)value));
	else
		return(-10526);
		
	iRet = wwlog(sLogStr);
	if(iRet!=0)
		return(iRet);*/
    
    TempBfr=*fbfr;
    lLen=Flen32(TempBfr,fieldid,oc-1)+50;
    lUnused=Funused32(TempBfr);
/*    sprintf(sLogStr,"unused:%ld",lUnused);
    wwlog(sLogStr); */
    if(lLen>(lUnused-10)){
       if(sg_ex_policy.ex_values==0)
       	  lSize = Fsizeof32(TempBfr)*20/100+Fsizeof32(TempBfr);
       else if((sg_ex_policy.ex_method == 0) && (sg_ex_policy.ex_values<=lLen))
          lSize = Fsizeof32(TempBfr)+1*lLen;
       else if((sg_ex_policy.ex_method==0) && (sg_ex_policy.ex_values>lLen))
          lSize = Fsizeof32(TempBfr)+sg_ex_policy.ex_values;
       else if(sg_ex_policy.ex_method==1)
       	  lSize = (Fsizeof32(TempBfr)*(sg_ex_policy.ex_values)/100)  +Fsizeof32(TempBfr);

/*       	sprintf(sLogStr,"size:%ld",lSize);
    	wwlog(sLogStr); */
       TempBuf = (WWFBFR *)tprealloc((char *)TempBfr,lSize);
       userlog("unused:%ld,size:%ld",lUnused,lSize);

       if(TempBuf==NULL){
          userlog("ext_error:tprealloc return NULL!");
          return (0-FMLErrNo[23].ierrno);
       }
          
       if(Fsizeof32(TempBuf)<lSize){
       		userlog("ext_error:lSize is:%ld,Fsizeof32 is:%ld",lSize,Fsizeof32(TempBuf));
       		return (0-FMLErrNo[23].ierrno);
       }
       
       *fbfr=TempBuf;
       TempBfr=*fbfr;
    }

    if(Fchg32(TempBfr,fieldid,oc,(char*)value,(FLDLEN32)10)==-1)
       return(0-FMLErrNo[Ferror32].ierrno);
    else
       return 0;	
}

/*************************************
 *  功能: 从缓冲区中取出字符串形数据 *
 *  参数: fbfr：      缓冲区地址     * 
 *        fieldid：   域标识         *
 *        oc：        下标           *
 *        value：     数据包地址     *
 *  返回值：                         *
 *          =0   成功                *
 *          <0   失败                *       
**************************************/
int wwfgets(WWFBFR *fbfr,WWFLDID fieldid,WWFLDOCC oc,char *buf)
{
    int iRet;
    char sLogStr[1000];

     if(Fgets32(fbfr,fieldid,oc,buf)==-1)
     	return (0-FMLErrNo[Ferror32].ierrno);
     else{
    	/* LOG */
 /*  		sprintf(sLogStr,"GET;WWFLDID:%X;VALUE:%s",fieldid,buf);

		iRet = wwlog(sLogStr);
		if(iRet!=0)
			return(iRet);     */
     
        return 0;
     }
}

/*************************************
 *  功能: 从缓冲区中取出整形数据     *
 *  参数: fbfr：      缓冲区地址     * 
 *        fieldid：   域标识         *
 *        oc：        下标           *
 *        buf：       数据包地址     *
 *  返回值：                         *
 *          =0   成功                *
 *          <0   失败                *       
**************************************/
int wwfgetl(WWFBFR *fbfr,WWFLDID fieldid,WWFLDOCC oc,long *buf)
{
    int iRet;
    char temp[20];
    char sLogStr[1000];

     if(Fgets32(fbfr,fieldid,oc,temp)==-1){
     	return(0-FMLErrNo[Ferror32].ierrno);
     }
     
     if(strcmp(temp,"")==0)
     	return(-10524);
     
     *(buf)= atol(temp);
 
    /* LOG */
/*	sprintf(sLogStr,"GET;WWFLDID:%X;VALUE:%ld",fieldid,*buf);
		
	iRet = wwlog(sLogStr);
	if(iRet!=0)
		return(iRet);*/
		
     return 0;
}

/*************************************
 *  功能: 取错误信息                 *
 *  参数: ierrno： 错误号            * 
 *  返回值：                         *
 *          <>NULL   错误信息        *
 *          =NULL    失败            *       
**************************************/
char * wwferrstr(int ierrno)
{
     int i;
     for(i=0;i<24;i++)
     	if(FMLErrNo[i].ierrno==ierrno)
     	   return (FMLErrNo[i].errstr);
     return NULL;
}

/*************************************
 *  功能: 记录日志					 *
 *  参数: 							 * 
 *        str	内容		         *
 *  返回值：                         *
 *          =0   成功                *
 *          <0   失败                *       
**************************************/
int wwlog(char *str)
{
   struct tm *time_now;
   time_t secs_now;
   static char nowstr[80];
   static char logfilename[101];

   FILE *manylog;

   tzset();
   time(&secs_now);
   time_now = localtime(&secs_now);
   strftime(nowstr, 80,"%Y-%m-%d %H:%M:%S",time_now);
   strftime(logfilename, 101, "PAY%Y%m%d%H.LOG", time_now);
   
   if ((manylog = fopen(logfilename, "at"))== NULL)
		return(-10525);

   fprintf(manylog, "PID:%d;TIME:%s;LOG:%s.\n",getpid(), nowstr, str);
   
   fclose(manylog);
   
   return 0;
}
