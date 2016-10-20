#include "Application.h"
#include "LogV2.h"
#include "HSSLogV3.h"
#include <iostream>

Application *GetApplication ();
Application *g_poProc = 0;

int main(int argc, char *argv[])
{
	int oc, iRet;
	ABMException oExp;
	char *p = strrchr(argv[0], '/');
    if (p == 0) {
    	strncpy(g_sAppName, argv[0], sizeof(g_sAppName)-1);	
    }
    else {
    	strncpy(g_sAppName, p+1, sizeof(g_sAppName)-1);	
    }

    char * paraOpt = "p:P:n:N:m:M";
    while ((oc=getopt(argc, argv, paraOpt)) != -1) {
        switch (oc) {
        case 'p':
        case 'P':
            g_iProcID = atoi(optarg);
            break;
        case 'n':
        case 'N':
            g_iNodeID = atoi(optarg);
            break;
        case 'm':
        case 'M':
            g_iModuID = atoi(optarg);
            break;    
        default:
            printf("程序发现未知参数 %c\n", oc);
            DIRECT_LOG(V3_INFO, V3_SCHEDUL,  MBC_PARAM_THREAD_NONSUPP_ERROR, "应用程序 %s 发现未知启动参数%c", argv[0], oc);
            break;
        }
    }
    
    if ((g_iProcID==-1) || (g_iModuID==-1)) {
        std::cout<<"启动命令必须加 -p x -m x 参数!"<<endl;
        DIRECT_LOG(V3_FATAL, V3_SCHEDUL, MBC_PARAM_THREAD_LOST_ERROR, "应用程序 %s 启动命令必须包含 -p x -m x 参数!" , argv[0]);    
        return -1;         
    }
    
    if ((g_iProcID<0) || (g_iModuID<0) ) {
        DIRECT_LOG(V3_FATAL, V3_SCHEDUL,  MBC_PARAM_THREAD_VALUE_ERROR, "应用程序 %s 启动命令参数值 必须非负!" , argv[0]);
        return -1;        
    }
    
    if ((g_poProc=GetApplication()) == NULL) {
        std::cout<<"GetApplication 失败, 程序退出!"<<endl;
        return -1;	
    }

	try {
    
    if (g_poProc->appInit(oExp) != 0) {
        oExp.printErr();
        return -1;	
    }
    
    
    if (g_poProc->init(oExp) != 0) {
		__INIT_LOG_(oExp);
        return -1;
    }
	
    __DEBUG_LOG3_(0, "应用程序 %s -p %d -m %d 启动...", g_sAppName, g_iProcID, g_iModuID);
    DIRECT_LOG(V3_INFO, V3_SCHEDUL, MBC_ACTION_PROCESS_START, "应用程序 %s -p %d -m %d 启动...", g_sAppName, g_iProcID, g_iModuID);
    
    iRet = g_poProc->run();

	}
	catch (...) {
		std::cout<<"catch sigbort signal"<<endl;
	}
    
   //  g_poProc->destory();
    
    delete g_poProc;
    
    __DEBUG_LOG3_(0, "应用程序　%s -p %d -m %d 停止...", g_sAppName, g_iProcID, g_iModuID);
    
    if (RcvExcepSig()) {
        DIRECT_LOG(V3_FATAL, V3_SCHEDUL, MBC_THREAD_EXIT_ERROR, "应用程序　%s -p %d -m %d receive unexcept signal!", g_sAppName, g_iProcID, g_iModuID);        
    }
    if (!iRet) {
        DIRECT_LOG(V3_INFO, V3_SCHEDUL, MBC_ACTION_PROCESS_END, "应用程序　%s -p %d -m %d 停止...", g_sAppName, g_iProcID, g_iModuID);    
    }
    else {
        DIRECT_LOG(V3_FATAL, V3_SCHEDUL, MBC_THREAD_EXIT_ERROR, "应用程序　%s -p %d -m %d 停止...", g_sAppName, g_iProcID, g_iModuID);
    }
	return 0;	

}
