#include <stdlib.h>
#include "Environment.h"
#include "ParamDefineMgr.h"
#include "Log.h"
#include "LogDB.h"
#include <sys/types.h>
#include <signal.h>
int main(int argc ,char *argv[])
{
    int i = 1;
    int iSysProcID = 0;
    char sTemp[6];
    char sString[1024];
    
    try{
        ParamDefineMgr::getParam("LOGDB", "LOGDB_SWITCH", sTemp, 5);
        if(!atoi(sTemp)){
            Log::log(0, "LOG入库开关未打开\n");
            return -1;
        }
        
        LogDB::init();
        
        while (i < argc){
            if (argv[i][0] != '-'){
                i++;
                continue;
            }
            
            switch(argv[i][1] | 0x20){
                case 'k':
                    iSysProcID = LogDB::getSysProc();
                    if(iSysProcID > 0){
                        kill(iSysProcID, SIGKILL);
                        LogDB::freeSHMBuf();
                    }
                    Log::log(0, "删除共享内存和常驻进程完成\n");
                    i++;
                    break;
                    
                case 'c':
                    LogDB::logFileOper();
                    Log::log(0, "创建共享内存和常驻进程完成\n");
                    i++;
                    break;
                    
                default:
                    i++;
                    Log::log(0, "********************************************************************\n"
                                 "  LOG入库 logdbmgr Version 1.0.0\n"
                                 "  调用方法:\n"
                                 "  运行模式: logdbmgr -c (创建共享内存和常驻进程)\n"
                                 "            logdbmgr -k (删除共享内存和常驻进程)\n"
                                 "********************************************************************\n"
                              );
                    break;
            }
        }
        
        if(argc == 1)
            Log::log(0, "********************************************************************\n"
                                 "  LOG入库 logdbmgr Version 1.0.0\n"
                                 "  调用方法:\n"
                                 "  运行模式: logdbmgr -c (创建共享内存和常驻进程)\n"
                                 "            logdbmgr -k (删除共享内存和常驻进程)\n"
                                 "********************************************************************\n"
                              );
    }
    catch( TOCIException & e ){
        snprintf( sString, 1024, "LOG入库处理收到TOCIException\n[TOCIEXCEPTION]: TOCIException catched\n"
            "*Err: %s\n""  *SQL: %s\n********\n", e.getErrMsg(),e.getErrSrc());
        Log::log( 0, sString );
    }
    catch( TException & e ){
        snprintf( sString, 1024, "LOG入库处理收到TException\n[EXCEPTION]: %s\n********\n", e.GetErrMsg());
        Log::log( 0, sString );
    }
    catch( Exception & e ){
        snprintf( sString, 1024, "LOG入库处理收到Exception\n[EXCEPTION]: %s\n********\n", e.descript());
        Log::log( 0, sString );
    }
    
    return 0;
}
