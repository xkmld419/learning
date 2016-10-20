/*
*	TT数据库登陆API
*/

#ifndef __TIMESTEN_ACCESS_H_INCLUDED_
#define __TIMESTEN_ACCESS_H_INCLUDED_

#include "TimesTenInterface.h"
#include "ABMException.h"

#define GET_CURSOR(cursor, conn, conf, oExp) GetTTCursor(cursor, conn, conf, oExp)
#define REL_CURSOR(cursor) RelTTCursor(cursor)


/*
*	函数功能: 申请一个获得操作TT数据库的游标
*	参数:
*        TimesTenCMD *&cursor: 操作数据库的游标, 指针必须初始化为NULL
*        TimesTenConn *&conn: 登陆的数据库游标, 如果instance为NULL, 内部会调用LoginTT读取
*        [label]下的配置, 生成一个登陆TT库实例的会话, 在该会话上申请操作游标, 如果instance非NULL,
*        直接在该会话上申请操作游标
*        const char *pconf: 配置文件中的读取标签
*	返回: 函数成功返回0, 失败返回错误号
*   注意: GetTTHdl和 RelTTHdl要成对出现, 防止有内存泄漏,TimesTenConn的释放需要调用LogoutTT
*/
extern int GetTTCursor(TimesTenCMD *&cursor, TimesTenConn *&conn, const char *pconf, ABMException &oExp);

/*
*	函数功能: 释放一个TT数据库的句柄
*	参数:
*        TimesTenCMD *&hdl: 操作数据库的句柄
*	返回: 
*/
extern void RelTTCursor(TimesTenCMD *&cursor);


/*   登陆TT数据库
* pconf:  读取配置文件的标志
* conn: TT库实例,初始值必须为NULL
* 成功返回0, 失败返回错误号
*/
extern int ConnectTT(const char *pconf, TimesTenConn *&conn, ABMException &oExp);

/* 注销
* refTTHandle: TT库实例
* 成功返回0 , 失败返回错误号
*/
extern int DisconnectTT(TimesTenConn *&conn, ABMException &oExp);

/* 读取配置信息*/
extern int GetConnectInfo (const char *pconf, char *usr, char *pwd, char *dsn, ABMException &oExp);

#endif/*__TIMESTEN_ACCESS_H_INCLUDED_*/
