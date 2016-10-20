#ifndef _SHMCMD_SET_H_HEAD_
#define _SHMCMD_SET_H_HEAD_
#include <vector>
#include "SHMDataMgr.h"
using namespace std;

class SHMCmdSet
{
public:
	//创建所有共享内存
	static int createSHMALL();

	//卸载所有共享内存
	static int unloadSHMALL();

	//断开所有共享内存的连接
	static int detachSHMALL();

	//导出共享内存数据到指定的table中
	//static int exportSHMALL(const char * sTab);

	//清空所有共享内存
	static int clearSHMALL();

	//查询所有共享内存的使用清空
	static int querySHMALL(vector<SHMInfo> &_vShmInfo);

	//上载所有共享内存数据
	static int loadSHMALL();

	//导出共享内存数据到指定的table中
	static int exportOneSHMALL(const char *_sType,const char * sTab);

	//创建某一个块共享内存
	static int createoneSHM(const char *_sType);

	//扩充某一个块共享内存
	static int ralloconeSHM(const char *_sType);

	//展示一块共享内存信息
	static int queryoneSHM(const char *_sType,vector<SHMInfo> &_vShmInfo);

	//卸载某一块共享内存
	static int unloadoneSHM(const char *_sType);

	//清空某一块共享内存
	static int clearoneSHM(const char *_sType);

	//断开某一块共享内存的连接
	static int detachoneSHM(const char *_sType);

	//加载某一块共享内存的数据
	static int loadoneSHM(const char *_sType,const char * _sFileName = NULL);

	//往共享内存插入数据
	static int insertoneSHM(const char *_sType);

	//将某一块共享内存上锁
	static int lockoneSHM(const char *_sType);

	//验证核心参数是否正确
	static int checkParam();

	//查询共享内存的key值
	static int querySHMData(const char *_sType,char *_sKey=NULL,char *_sKey1 = NULL);
};
#endif
