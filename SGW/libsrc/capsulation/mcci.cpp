/*这是对TUXEDO和TONGEASY共有API的封装程序,适宜于TUXEDO环境使用*/
/*************************************
 *  Author: Linkage                  *
 *  Create Date:     2005/04/04      * 
 *  The last Modify: 2005/04/04      *
**************************************/
/*#define TUXEDO*/

#include "./mcci.h"
#include <stdlib.h>

/* 定义错误列表 */
MCCI_TUXEDO_ERROR TuxedoErrNo[27] = {
	                              {0,     "no error"      },
                                      {10205, "TPEABORT"      },
                                      {10012, "TPEBADDESC"    },
                                      {10010, "TPEBLOCK"      },
                                      {10013, "TPEINVAL"      },
                                      {10310, "TPELIMIT"      },
                                      {10007, "TPENOENT"      },
                                      {10003, "TPEOS"         },
                                      {10106, "TPEPERM"       },
                                      {10001, "TPEPROTO"      },
                                      {10306, "TPESVCERR"     },
                                      {10305, "TPESVCFAIL"    },
                                      {10002, "TPESYSTEM"     },
                                      {10006, "TPETIME"       },
                                      {10003, "TPETRAN"       },
                                      {10011, "TPGOTSIG"      },
                                      {10319, "TPERMERR"      },
                                      {10008, "TPEITYPE"      },
                                      {10009, "TPEOTYPE"      },
                                      {10320, "TPERELEASE"    },
                                      {10207, "TPEHAZARD"     },
                                      {10206, "TPEHEURISTIC"  },
                                      {10314, "TPEEVENT" 	  },
                                      {10315, "TPEMATCH"	  },
                                      {10316, "TPEDIAGNOSTIC" },
                                      {10317, "TPEMIB"  	  },
                                      {10318, "TPMAXVAL"  	  }
                                    };
extern "C"
{
/*************************************
 *  功能: 程序注册                   *
 *  参数: AppID：应用程序类型        * 
 *        AppInfo：应用程序注册信息  *
 *  返回值：                         *
 *          =0   成功                *
 *          <0   失败                *       
**************************************/
int csllogin(int AppID,APPINIT *AppInfo)
{
	if (tpinit(AppInfo)==-1)
		return (0-TuxedoErrNo[tperrno].ierrno);
	else
		return 0;
}

/*************************************
 *  功能: 程序注销                   *
 *  参数: 无                         * 
 *  返回值：                         *
 *          =0   成功                *
 *          <0   失败                *       
**************************************/
int csllogout()
{
	if (tpterm()==-1)
		return(0-TuxedoErrNo[tperrno].ierrno);
	else
		return 0;
}

/*************************************
 *  功能: 交易开始                   *
 *  参数: 无                         * 
 *  返回值：                         *
 *          =0   成功                *
 *          <0   失败                *       
**************************************/
int cslbegin()
{
	int i;
	
	#ifdef TX
		i=tx_begin();
	#else
		i=tpbegin((long)0,(long)0);
	#endif /*TX*/
	
	if (i==-1)
		return(0-TuxedoErrNo[tperrno].ierrno);
	else
		return 0;
}

/*************************************
 *  功能: 交易提交                   *
 *  参数: 无                         * 
 *  返回值：                         *
 *          =0   成功                *
 *          <0   失败                *       
**************************************/
int cslcommit()
{
	int i;
	
	#ifdef TX
		i=tx_commit();     
	#else
		i=tpcommit((long)0);
	#endif /*TX*/
	
	if(i==-1)
		return(0-TuxedoErrNo[tperrno].ierrno);
	else
		return 0;
}

/*************************************
 *  功能: 交易失败                   *
 *  参数: 无                         * 
 *  返回值：                         *
 *          =0   成功                *
 *          <0   失败                *       
**************************************/
int cslrollback()
{
	int i;
	
	#ifdef TX
		i=tx_rollback();     
	#else
		i=tpabort((long)0);
	#endif /*TX*/
	
	if (i==-1)
		return(0-TuxedoErrNo[tperrno].ierrno);
	else
		return 0;
}

/*************************************
 *  功能: 同步发送                   *
 *  参数: svc：  服务名              * 
 *        idata：发送数据缓冲区      *
 *        ilen： 发送数据包长度      *
 *        odata：接收数据缓冲区地址  *
 *        olen： 接收数据长度的地址  *
 *  返回值：                         *
 *          =0   成功                *
 *          <0   失败                *       
**************************************/
int cslcall(char *svc,char *idata,long ilen,char **odata,long *olen)
{
	if (tpcall(svc,idata,ilen,odata,olen,(long)0)==-1)
		return(0-TuxedoErrNo[tperrno].ierrno);
	else
		return 0;	
}

/*************************************
 *  功能: 会话连接                   *
 *  参数: svc：  服务名              * 
 *        idata：发送数据缓冲区      *
 *        ilen： 发送数据包长度      *
 *        flags：控制标志            *
 *  返回值：                         *
 *          >0   连接描述符          *
 *          <0   失败                *       
**************************************/
int cslconnect(char *svc,char *data,long len,long flags)
{
	int i;
	
	if ((i=tpconnect(svc,data,len,flags))==-1)
		return(0-TuxedoErrNo[tperrno].ierrno);
	else
		return i;
}

/*************************************
 *  功能: 会话断开                   *
 *  参数: cd：  连接描述符           * 
 *  返回值：                         *
 *          >0   连接描述符          *
 *          <0   失败                *       
**************************************/
int csldiscon(int cd)
{
	if (tpdiscon(cd)==-1)
		return(0-TuxedoErrNo[tperrno].ierrno);
	else
		return 0;
}

/*************************************
 *  功能: 会话发送                   *
 *  参数: cd：   连接描述符          * 
 *        data： 发送数据缓冲区      *
 *        len：  发送数据包长度      *
 *        flags：控制标志            *
 *  返回值：                         *
 *          >0   成功                *
 *          <0   失败                *       
**************************************/
int cslsend(int cd,char *data,long len,long flags)
{
	int i;
	long test;
	
	if ((i=tpsend(cd,data,len,flags,&test))==-1)
	{
		if (tperrno==TPEEVENT)
		{
			switch ((int)test)
			{
				case TPEV_DISCONIMM: 
					return (0-TuxedoErrNo[23].ierrno);
				case TPEV_SVCERR :
					return (0-TuxedoErrNo[24].ierrno);
				case TPEV_SVCFAIL:
					return (0-TuxedoErrNo[25].ierrno); 
			}
		}
		else
			return(0-TuxedoErrNo[tperrno].ierrno);
	}
	else
		return i;
}

/*************************************
 *  功能: 会话接收                   *
 *  参数: cd：   连接描述符          * 
 *        data： 接收数据缓冲区地址  *
 *        len：  接收数据长度地址    *
 *        flags：控制标志            *
 *  返回值：                         *
 *          >0   成功                *
 *          <0   失败                *       
**************************************/
int cslrecv(int cd,char **data,long *len,long flags)
{
	int i;
	long test;
	
	if ((i=tprecv(cd,data,len,flags,&test))==-1)
	{
		if (tperrno==TPEEVENT)
		{
			switch((int)test)
			{
				case TPEV_DISCONIMM: 
					return (0-TuxedoErrNo[23].ierrno);
				case TPEV_SENDONLY:
					return (0-TuxedoErrNo[22].ierrno);
				case TPEV_SVCERR:
					return (0-TuxedoErrNo[24].ierrno);
				case TPEV_SVCFAIL:
					return (0-TuxedoErrNo[25].ierrno);
				case TPEV_SVCSUCC:
					return (0-TuxedoErrNo[26].ierrno);
			}
		}
		else
			return(0-TuxedoErrNo[tperrno].ierrno);
	}
	else
		return i;
}

/*************************************
 *  功能: 分配缓冲区                 *
 *  参数: type： 缓冲区类型          * 
 *        size： 缓冲区长度          *
 *  返回值：                         *
 *          <>NULL   缓冲区地址      *
 *          =NULL   失败             *       
**************************************/
char * cslalloc(char *type ,long size)
{
	return(tpalloc(type,NULL,size));	
}



/*************************************
 *  功能: 释放缓冲区                 *
 *  参数: ptr： 缓冲区地址           * 
 *  返回值：                         *
 *          无                       *
**************************************/
void  cslfree(char *ptr)
{
	tpfree(ptr);	
}


/*************************************
 *  功能: 取错误信息                 *
 *  参数: ierrno： 错误号             * 
 *  返回值：                         *
 *          <>NULL   错误信息        *
 *          =NULL   失败             *       
**************************************/
char * cslerrstr(int ierrno)
{
	int i;
	
	for(i=0;i<27;i++)
		if(TuxedoErrNo[i].ierrno==ierrno)
			return (TuxedoErrNo[i].errstr);
	
	return NULL;
}

}
