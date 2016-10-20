/*这是对TUXEDO和TONGEASY共有API的封装程序,适宜于TUXEDO环境下非客户端程序使用*/
/*************************************
 *  Author: Many                     *
 *  Create Date:     2000/11/29     * 
 *  The last Modify: 2000/11/29      *
**************************************/
/*
#define TUXEDO
#define COMMON
*/

#include "./mcci.h"

extern "C"
{

/*************************************
 *  功能: 数据转发                   *
 *  参数:   svc： 转发的服务名       * 
 *          data： 发送的数据包      *
 *          len：  发送的数据包长度  *
 *  返回值：                         *
 *          无                       *
**************************************/
int cslsvrfwd(char *svc,char *data,long len)
{
	tpforward(svc,data,len,(long)0);
	return 0;
}

/*************************************
 *  功能: 同步应答                   *
 *  参数:   rval： 处理结果标志      * 
 *          rcode：返回代码          *
 *          data： 返回的数据包      *
 *          len：  返回的数据包长度  *
 *  返回值：                         *
 *          无                       *
**************************************/
int cslreturn_common(int rval,long rcode,char *data,long len)
{
	tpreturn(rval,rcode,data,len,(long)0);
	return 0;	
}

int cslopen_common()
{
	if(tpopen()==-1)
	{
		return -1;
	}
	
	return 0;
}

int cslclose_common()
{
	if (tpclose()==-1)
	{
		return -1;
	}
	
	return 0;
}

}
