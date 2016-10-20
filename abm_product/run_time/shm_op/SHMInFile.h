#ifndef __SHM_IN_FILE_H_INCLUDED_
#define __SHM_IN_FILE_H_INCLUDED_

class ABMException;

class SHMInFile
{

public:

    SHMInFile();

    ~SHMInFile();
    
    //初始化 成功返回0 失败返回-1
    int init(ABMException &oExp);
    
    //共享内存中HCODE信息写入文件 成功返回写入条数 失败返回-1
    long hcodeInFile(const char *pDir, long lBatchID);
    
    //共享内存中的MIN信息写入文件 成功返回写入条数 失败返回-1
    long minInFile(const char *pDir, long lBatchID);
};

#endif/*__SHM_IN_FILE_H_INCLUDED_*/
