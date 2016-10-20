#include "ReadIni.h"
#include "encode.h"
#include "TimesTenAccess.h"


#define ECODE_INVALID_PARA 1001   //参数无效
#define ECODE_LOST_ENV     1002   //缺失环境变量
#define ECODE_READCFG_FAIL 1003   //读取配置失败
#define ECODE_READCFG_EXP  1004   //读取配置异常
#define ECODE_CONN_TT_FAIL  1006  //连接TT数据库失败
#define ECODE_DISCONN_TT_FAIL 1008   //断开TT数据库连接失败    

#define DATA_ACCESS_INI "data_access.ini"

int GetConnectInfo(const char *pconf, char *usr, char *pwd, char *dsn, ABMException &oExp)
{
    if ((pconf==NULL) || (usr==NULL) || (pwd==NULL) || (dsn==NULL)) {
        ADD_EXCEPT0(oExp, "GetConnectInfo 参数为NULL");
        return ECODE_INVALID_PARA;	
    }
    
    ReadIni reader;
    char *penv = NULL;
    char HomeEnv[128] = {0};
    char sfile[256] = {0};
    char Pwd[128] = {0};
    if ((penv=getenv("ABMAPP_DEPLOY")) == NULL) {
        ADD_EXCEPT0(oExp, "环境变量ABMAPP_DEPLOY没有设置, GetConnectInfo 返回错误");				
        return ECODE_LOST_ENV;
    }
    strncpy(HomeEnv, penv, sizeof(HomeEnv)-2);
    if (HomeEnv[strlen(HomeEnv)-1] != '/') {
        HomeEnv[strlen(HomeEnv)] = '/';				
    }
    snprintf(sfile, sizeof(sfile), "%sconfig/%s", HomeEnv, DATA_ACCESS_INI);
    
    try {
        if (reader.readIniString (sfile, pconf, "username", usr, 0) == NULL) {
            return ECODE_READCFG_FAIL;	
        }
        if (reader.readIniString (sfile, pconf, "password", Pwd, 0) == NULL) {
            return ECODE_READCFG_FAIL;
        }
        decode(Pwd, pwd);
        if (reader.readIniString (sfile, pconf, "connstr", dsn, 0) == NULL) {
            return ECODE_READCFG_FAIL;	
        } 
        if ((usr[0]=='\0') || (pwd[0]=='\0') || (dsn[0]=='\0')) {
            ADD_EXCEPT0(oExp, "登陆信息获取不全, 请检查用户名,密码,连接串配置!");
            return ECODE_READCFG_FAIL;	
        }
    }
    catch(...) {
        ADD_EXCEPT1(oExp, "获取配置文件%s失败, GetConnectInfo 返回错误\n", sfile);	
        return ECODE_READCFG_EXP;	
    }
    return 0;
}

int ConnectTT(const char *pconf, TimesTenConn *&conn, ABMException &oExp)
{
    if (pconf == NULL) {
        ADD_EXCEPT0(oExp, "ConnectTT function pconf传入参数不可为NULL, ConnectTT 返回错误");
        return ECODE_INVALID_PARA;	
    }
    if (conn != NULL) {
        ADD_EXCEPT0(oExp, "ConnectTT function conn 传入参数必须为NULL, ConnectTT 返回错误");
        return ECODE_INVALID_PARA;	
    }
    char dbusr[128], dbpwd[128], dbdsn[256];
    memset(dbusr, 0x00, sizeof(dbusr));
    memset(dbpwd, 0x00, sizeof(dbpwd));
    memset(dbdsn, 0x00, sizeof(dbdsn));
    
    if (GetConnectInfo(pconf, dbusr, dbpwd, dbdsn, oExp) != 0) {
        ADD_EXCEPT1(oExp, "获取配置文件中标签为%s的内容失败, ConnectTT 返回错误\n", pconf);
        return ECODE_READCFG_FAIL;	
    } 
    string ttdsn(dbdsn);	
    try {
        conn = new TimesTenConn();
        conn->setDBLogin(ttdsn);
        conn->getDBConn(true);

    }
    catch (TTStatus st) {
        ADD_EXCEPT0(oExp, "登陆TT数据库失败,请检查配置和数据库服务器状态!!!, ConnectTT 返回错误");
        return ECODE_CONN_TT_FAIL;
    }
    return 0;	
}

int DisconnectTT(TimesTenConn *&conn, ABMException &oExp)
{
    if (conn == NULL) {
        //log
        return ECODE_DISCONN_TT_FAIL;	
    }
    try {	
        conn->disconnect();
        delete conn;
    }
    catch(...) {
        ADD_EXCEPT0(oExp, "TT注销时 发生异常!");
        return ECODE_DISCONN_TT_FAIL;
    }
    conn = NULL;
    return 0;
}

int GetTTCursor(TimesTenCMD *&cursor, TimesTenConn *&conn, const char *pconf, ABMException &oExp)
{
    if (cursor != NULL) {
        return ECODE_INVALID_PARA;		
    }	
    
    int ret;
    if (conn == NULL) {
        if (pconf == NULL) {
            return ECODE_INVALID_PARA;
        }		
        ret = ConnectTT(pconf, conn, oExp);
        if (ret != 0) {
            DisconnectTT(conn, oExp);	
            return ret;
        }
    }
    
    cursor = new TimesTenCMD(conn->getDBConn());
    
    return 0;
}

void RelTTCursor(TimesTenCMD *&cursor)
{
    if (cursor != NULL) {
        delete cursor;
        cursor = NULL;
    }
    return;		
}
