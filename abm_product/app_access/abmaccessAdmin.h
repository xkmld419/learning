/* add by  zhangyz */
/* date:2010 09 15 */

#ifndef INCLUDED_ABMACCESS_ADMIN_H
#define INCLUDED_ABMACCESS_ADMIN_H

#include <signal.h>
#include <iostream>
#include <map>

#include "Environment.h"
#include "TOCIQuery.h"
#include "exception.h"

#include "abmaccessLog.h"

#include "abmclearfile.h"
#include "Log.h"
#include "Date.h"
#include "ReadIni.h"

//#include "UsrInfoCheckMgr.h"

#define ARG_NUM   128
#define ARG_VAL_LEN  1024


//#define HBACCESS_DEBUG 1

/*
#define HBACCESS_OPT_ADD 1  //增加
#define HBACCESS_OPT_DEL 2  //删除
#define HBACCESS_OPT_QRY 3  //查询
#define HBACCESS_OPT_OTHER 4  //其他

#define HBACCESS_STATE_OK "OK"  //成功
#define HBACCESS_STATE_FALSE "FALSE"  //失败
*/

using namespace std;

class hbaccessAdmin
{
  public:
    hbaccessAdmin();
    ~hbaccessAdmin();
    int run();
    int cirPre();
    int deal();
    int usrInfoCheck(char *sInfo);
    int usrInfoCheck(char *sStaffID,char *sKeyWord);
    void trimEcho(char *s1,char *s2);
    void operateLog(int type,char *result,char *cmd,char *res);

  private:
  	int specialOrder( char * orderName,char *paramId1,char *paramId2);
  	int setSQL(char *name, char * line);
  	int delSQL(char *name);
  	int dealcomm(char *name,char *paramId1);
  	int showFieldAd(char *sql);
  	int showField(char *sql);
  	int executeOrder( char * orderName,char *paramId,char *paramId2);
  	void redirectDB(const char * sIniHeader);
  	void clearDataBase();
  	void printHbaccess();
  	void printHbaccessad();
  	

  private:
  	int getTagString();
  	int clearParam();

  	void printfManual()
		{
			  printf("\n ************************************************************************   \n") ;
			  printf("  1:help 打印帮助文件 \n");
			  printf("  2:abmaccess -u 设置一个参数对应的执行脚本 \n");
		    printf("     ex:abmaccess  -u getallproduct=SQL \"select * from PRODUCT\"\n");
		    printf("  3:abmaccess -d 删除一个参数对应的执行脚本\n");
		    printf("     ex:abmaccess -d getallproduct \n");
		    printf("  4:abmaccess -s 显示命令或执行命令 \n");
		    printf("     ex:abmaccess -s 显示所有自定义的命令\n");
		    printf("     ex:abmaccess -s getallproduct 执行getallproduct命令\n");
		    printf("     ex:abmaccess -s getallproduct 555 执行getallproduct命令\n");
		  /*printf("  5:abmaccess -l  调用数据库表空间数据清理功能\n");
		   
		    printf("  6:hbaccess 参数 + 时间(YYYYMMDD)  清理指定文件系统过期数据\n");
		    printf("     -g 清理原始采集文件\n") ;
				printf("     -c 清理校验错误文件 \n") ;
				printf("     -m 清理数据合并中间文件\n") ;
				printf("     -r 清理排重历史文件\n") ;
				printf("     -w 清理写文件历史文件\n") ;
				printf("     -t 清理tdtrans可能留下来的文件\n") ;
				printf("     -a 清理asn编码前的备份文件\n") ;
				printf("     -e 清理租费套餐费回退备份的回退文件\n") ;
				*/
				printf("  5:q or quit or exit 退出控制台 \n");
		    printf(" ************************************************************************   \n") ;
		  }

  	private:
  		char m_sStaffId[100];
  		char m_sPassed[100];
  		bool m_bUserFlag;

  		int m_iargc;
      char m_sOrder[ARG_VAL_LEN*ARG_NUM];
      char m_sArgv[ARG_NUM][ARG_VAL_LEN];

};

#endif

