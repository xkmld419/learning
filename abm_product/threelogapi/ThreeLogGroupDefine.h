#ifndef _LIB_THREE_LOG_DEFINE_H_
#define _LIB_THREE_LOG_DEFINE_H_

#define  WRITEFILE          0                   //写文件
#define  WRITESHARE         1                   //写共享

//展示类
//#define  DISPLAYMAXCNT      3000                //界面展示维数
#define  DISPLAYMAXCNT     1000000        //界面展示维数


//日志等级标识	1-fatal、2-error、3-warning、4-info、5-debug
enum
{
	LOG_LEVEL_FATAL       =1,
	LOG_LEVEL_ERROR       =2,
	LOG_LEVEL_WARNING     =3,
	LOG_LEVEL_INFO        =4,
	LOG_LEVEL_DEBUG       =5
};


//日志类别标识	1-错误（error/fatal）、2-告警（warning）、3-普通信息（debug/info）
enum
{
	LOG_CLASS_ERROR     =1,
	LOG_CLASS_WARNING   =2,
	LOG_CLASS_INFO      =3
};


//日志分类标识	1：业务日志、2、hbparam参数管理日志、3 系统管理日志、4 进程调度日志、5 hbaccess数据库关键内容修改日志

//日志类别
enum
{
    LOG_TYPE_BUSI        =1,
    LOG_TYPE_PARAM       =2,
    LOG_TYPE_SYSTEM      =3,
    LOG_TYPE_PROC        =4,
    LOG_TYPE_DATEDB      =5
};


//日志组状态
enum
{
    INACTIVE=1,                                 //空闲
    CURRENT,                                    //在用
    ACTIVE                                      //待归档
};

//清仓状态
enum
{
    UNDOWN=1,                                   //未归档
    ALLDOWN,                                    //已归档
    NEEDDOWN,                                   //待归档
    FILEDOWN                                    //已落地
};

enum
{
		LOG_ACTION_INS    =1,
	  LOG_ACTION_UPD    =2,
	  LOG_ACTION_DEL    =3
};
//展示类

enum
{
    GROUPCREAT              =1,                 //创建共享内存
    GROUPFREE               =2,                 //释放共享内存
    GROUPFILEADD            =3,                 //添加日志组成员文件
    GROUPDELETE             =4,                 //删除日志组成员文件
    GROUPPRINTF             =5,                 //显示日志组信息
    GROUPQUIT               =6,                 //切换日志组
    GROUPSET                =7,                 //设置归档
    GROUPBAK                =8,                 //手动备份文件
    GROUPOUT                =9,                 //输出文件信息
    FILETEST                =10,                //测试命令
    PIGEONHOLE              =11,                //手动归档
    GROUPHELP               =12,                //命令帮助手册
    GROUPADD                =13,                //添加日志组
    GROUPSTAT               =14,                //查看内存状态
    GROUPPARAM              =15,                //展现核心参数修改日志
    GROUPMODULE             =16,                //按模块名称查询日志
    GROUPPROCESS            =17,                //按进程名称查询日志
    GROUPINFOLV             =18,                //按日志等级查询日志
    GROUPCLASS              =19,                //按类别名称查询日志
    GROUPTYPE               =20,                //按分类名称查询日志
    GROUPCODE               =21,                //按日志编码查询日志
    GROUPRESET              =23,                //初始化清空共享内存
    GROUPTEST               =99                 //测试
};

#endif  //_LIB_THREE_LOG_DEFINE_H_
