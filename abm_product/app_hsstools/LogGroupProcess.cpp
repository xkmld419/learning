#include "LogGroup.h"
#include "hbcfg.h"

int main(int argc,char** argv)
{
    char sString[4096];
	int bIsLogin;
//	bIsLogin=-1;
	//HbAdminMgr * cHbLogin = new HbAdminMgr();
	//bIsLogin =HbAdminMgr::getAuthenticate();

//	if(bIsLogin==-1)
//	{
//		Log::log(0, "请先登录认证!");
//		return 0;
//	}
	
//	if(bIsLogin==0)
//	{
//		Log::log(0, "未登录认证,请先运行登录认证程序!");
//		return 0;
//	}


    try
    {
        LogGroup *pGroup = new LogGroup();
        if(!pGroup)
            return -1;

        char sArgv[ARG_NUM][ARG_VAL_LEN];
        for(int i=0;i!=argc;i++)
    	{
		    int cnt = strlen(argv[i]);
		    if(cnt ==0)
			    break;
    		for(int j=0;j!=cnt;j++)
    		{
    		    sArgv[i][j] = argv[i][j];
    		}
    	}
        if(!pGroup->prepare(sArgv, argc))
        {
            delete pGroup;
            return -1;
        }
        pGroup->Deal();
        if(pGroup)
            delete pGroup;
        return 0;
    }
    catch (Exception &e)
    {
        snprintf (sString,4095, "收到Exception退出\n[EXCEPTION]: %s", e.descript());
        Log::log (0, sString);
        exit(-1);
    }
}
