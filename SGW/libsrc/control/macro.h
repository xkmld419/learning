/*VER: 1*/ 
#ifndef _MACRO_H
#define _MACRO_H

/* 与控件属性有关的宏定义 */
#define AMOUNT 		1   		  /* EDIT控件输入为小数点和数字 */
#define IPADDR          1  		  /* EDIT控件输入为小数点和数字 */
#define NUMERIC		2   		  /* 纯数字  */
//#define NUMBER          2   		  /* 纯数字  */
#define CHAR    	3   		  /* 字符    */
#define PASSWD          4   		  /* 为口令输入框 */
#define STATIC          5   		  /* 为静态编辑框 ==> 不可编辑 */
#define sCaption        sData             /* */  
#define pItems          pData             /* */
#define iItemCount      iAdditional       /* */
#define iInputType      iAdditional       /* */
#define MAX_INPUT_LEN	100 		  /*  EDIT 最大输入长度 */
#define MAX_ITEMS       6300 		  /*  LIST 最大items数目 */
#define SELECTED        1		  /*  LIST 的 item 被选中 */ 
#define UNSELECTED      0		  /*  LIST 的 item 未被选中 */ 
#define ALL_ITEMS       9999              /*  表示所有的 items ,    */

/* 与FORM有关的消息  */
#define FORM_KILL       999               /*  即发出该消息, 触发FormKill()      */ 
#define FORM_KILL_OK    998               /*  即发出该消息，可以触发父Form的重画*/ 
#define FORM_CLEAR      997		  /*  即发出该消息，可以触发对当前Form的控件内容清除 */

/* 参数 */
#define HOT_KEY         888               /*  当有键盘热键事件时的参数 */ 

#define BASE_ID		10000		  /*  Base ID Value */
#define ID(_n_)		(BASE_ID+_n_)     /*  ID Value */
#endif




