
#ifndef   PARAMSHMMGR_H_HEADER_INCLUDED
#define   PARAMSHMMGR_H_HEADER_INCLUDED

#include "CSemaphore.h"
#include "ParamInfoMgr.h"


#define PARAM_SHM_NUM 2                        //共享内存的数目.即A,B两块,这里的块是虚拟的

#define PARAM_SHM_INDEX_ALL -1                 //
#define PARAM_SHM_INDEX_A    0                 //共享内存块的编号A
#define PARAM_SHM_INDEX_B    1                 //共享内存块的编号B

//访问入扣得共享内存结构,存放目前可以连接使用的虚拟共享内存块的编号
struct ParamAccessPort
{
    int m_iShmIdx;                             //在工作的共享内存的编号
	int m_iFlashSeqIdx;
	int m_iWait2DelSec;//delete date need second() ., default is 0 no deleting other block    
	int m_iAlteration ;//
	int m_iAlteratAmend ;//
    long m_lExVal;//扩展数值,留给应用进程之间的处理
    char cState;//状态字段,可以用于全局数据控制    
};


class ParamShmMgr
{
public:
    ParamShmMgr();
    ~ParamShmMgr();

    void loadShm();                          //将参数导入共享内存
    void flashShm();                         //刷新共享内存    
    void unloadAllShm(bool bForce = false);  //卸载所有共享内存
		void getStat();													 //统计共享内存
    void unloadShmByKey(const long lShmKey, bool bForce = false);
    void setState(char cSate);


    bool linkDataAccessShm();                //连接参数访问入库的共享内存

    //数据结构体数组(虚拟的共享内存块结构,即参数信息块)
    ParamInfoMgr* m_pParamDataBuf[PARAM_SHM_NUM];
    
    //数据访问入口结构
    struct ParamAccessPort* m_pParamAccessPort;    
	int showCtlInfo();

	void unloadShmByFlag(int iFlag);
private:
    
    void allocDataAccessShm();
    void detachShmByData(void * pData);
    void deleteShmByKey(const long lShmKey);
    

    long m_lShmStateKey;                     //入口共享内存key
    long m_lShmStateLockKey;                 //入口共享内存的信号量的key
public:
    CSemaphore* m_pShmStateLockSem;          //入口共享内存的信号量


};


#endif    //PARAMSHMMGR_H_HEADER_INCLUDED

