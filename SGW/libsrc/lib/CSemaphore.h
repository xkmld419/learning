/*VER: 1*/ 
#ifndef __CSEMAPHORE_HEADER_INCLULDED__
#define __CSEMAPHORE_HEADER_INCLULDED__

/**********************************************************************************************\
* 信号量类
* 
*
\**********************************************************************************************/

class CSemaphore 
{
public:
    bool getSemEx(const char * sem_name, 
 /*在Windows上可以用字符串表示，在unix上请输入数字，表示key_t*/
                int init_count = 1,
                 /*初始资源数量，默认为 1 */
                int max_count = 64);
            /*可以服务的进程数量，默认为64*/
	
    bool getSemEx(int iKey, 
 /*在Windows上可以用字符串表示，在unix上请输入数字，表示key_t*/
                int init_count = 1,
                 /*初始资源数量，默认为 1 */
                int max_count = 64);
            /*可以服务的进程数量，默认为64*/
	bool getSem(const char * sem_name, 
 /*在Windows上可以用字符串表示，在unix上请输入数字，表示key_t*/
                int init_count = 1,
                 /*初始资源数量，默认为 1 */
                int max_count = 64);
            /*可以服务的进程数量，默认为64*/
    bool P(bool bWait = true);                           /*P操作，表示请求资源, bWait是否要同步等待*/
    bool V();                                               /*v操作，表示释放资源*/
    bool V_W();                                               /*v操作，表示释放资源*/
    bool P_W(bool bWait = true);                           /*P操作，表示请求资源, bWait是否要同步等待*/
    bool close();                                           /*关闭信号量*/
public:
    CSemaphore();                                           /*构造函数，并不完成semaphore的申请，需要getSem后使用*/
    virtual ~CSemaphore();                                  /*关闭句柄*/
private:
    void * _Handle;                                         /*信号量句柄，如果*/
	char m_sSemName[19];
};
#endif


