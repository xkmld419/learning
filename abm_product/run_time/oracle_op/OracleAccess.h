/*
*	ll数据库登陆API
*/

#ifndef __ORACLE_ACCESS_H_INCLUDED_
#define __ORACLE_ACCESS_H_INCLUDED_

#include "TOCIQuery.h"
#include "ABMException.h"

/*
*	函数功能: 申请一个获得操作ORA数据库的游标
*	参数:
*        TOCIQuery *&cursor: 操作数据库的游标, 指针必须初始化为NULL
*        TOCIDatabase *&oci: 登陆的数据库实例, 如果oci为NULL, 内部会调用LoginOra读取
*        [label]下的配置, 生成一个登陆ORA库的会话, 在该会话上申请操作游标, 如果oci非NULL,
*        直接在该会话上申请操作游标
*        const char *label: 配置文件中的读取标签
*	返回: 函数成功返回0, 失败返回错误号
*   注意: GetOraHdl和 RelOraHdl要成对出现, 防止有内存泄漏,TOCIDatabase的释放需要调用LogoutOra
*/
extern int GetOraCursor(TOCIQuery *&cursor, TOCIDatabase *&oci, const char *pconf, ABMException &oExp);

/*
*	函数功能: 释放一个ORA数据库的游标
*	参数:
*        TOCIQuery *&hdl: 操作数据库的游标
*	返回: 
*/
extern void RelOraCursor(TOCIQuery *&cursor);


/*   登陆ORA数据库
* pconf:  读取配置文件的标志
* database: ORA库实例, 初始值必须为NULL
* 成功返回0, 失败返回错误号
*/
extern int ConnectOra(const char *pconf, TOCIDatabase *&database, ABMException &oExp);

/* 注销
* refOciHandle: ORA库实例
* 成功返回0 , 失败返回错误号
*/
extern int DisconnectOra(TOCIDatabase *&database, ABMException &oExp);


/* 读取配置信息*/
extern int GetOraConnectInfo (const char *pconf, char *usr, char *pwd, char *dsn, ABMException &oExp);

#endif/*__ORACLE_ACCESS_H_INCLUDED_*/
