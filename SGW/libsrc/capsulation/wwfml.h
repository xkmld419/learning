/***************************/
/* Create 18/11/2000.      */
/* Modify 23/11/2000 9:30  */
/***************************/

#ifndef MID_COMPONENT_COMMON_FML_INTERFACE_H
#define MID_COMPONENT_COMMON_FML_INTERFACE_H

#ifndef NULL
#define NULL 			0
#endif /* NULL */


#ifdef TUXEDO
/* Include 文件 */
#   include <fml32.h>
#   include <atmi.h>
typedef struct _mcci_tuxedo_fml_ {
    int    ierrno;	       /* 错误码 */
   char   *errstr;	       /* 错误信息 */
} MCCI_TUXEDO_FML_ERROR;
#endif /*TUXEDO*/

/* 声明应用程序信息结构 */
typedef  FBFR32    WWFBFR;
typedef  FLDID32   WWFLDID;
typedef  FLDOCC32  WWFLDOCC;
   
/* 空间扩展策略 */
#define   FIXED     0      /*固定值方式*/
#define   PERCENT   1      /*百分比方式*/
   
/*************************************
*    FML MANAGER FUNCTIONS DECLARE   *
**************************************/

extern int wwfexpolicy(int ex_method,int ex_values,int value1,int value2,int value3);
/*ex_method：扩展方式，两种取值：FIXED PERCENT; ex_values：扩展值
  ex_values=0时使用默认方式，即：扩展要存放的当前记录的大小的10倍*/

extern int wwfnum(WWFBFR *fbfr,WWFLDID fieldid);
/*fbfr:指定缓冲区;fieldid:域标识*/

extern int wwfput(WWFBFR **fbfr,WWFLDID fieldid,WWFLDOCC oc,void *value);
/*fbfr:指定缓冲区;fieldid:域标识;oc:下标;len:值长度
 value是个指针，它的类型不一定是(char *)，应该与你定义的fieldid类型相匹配*/

extern int wwfgets(WWFBFR *fbfr,WWFLDID fieldid,WWFLDOCC oc,char *buf);
/*fbfr:指定缓冲区;fieldid:域标识;oc:下标*/

extern int wwfgetl(WWFBFR *fbfr,WWFLDID fieldid,WWFLDOCC oc,long *buf);
/*fbfr:指定缓冲区;fieldid:域标识;oc:下标*/

extern char * wwferrstr(int ierrno);
/*ierrno:错误号*/

#endif /*MID_COMPONENT_COMMON_FML_INTERFACE_H*/

