#ifndef __MMAP_FILE_H_INCLUDED_
#define __MMAP_FILE_H_INCLUDED_

class ABMException;

class MmapFile
{

public:

    //sFile: 要映射文件的路径
    //iSize: 共享内存大小
    MmapFile(char *sFile, int iSize=0);

    MmapFile(); 

    ~MmapFile();

    int openMap(ABMException &oExp);  //打开文件映射到私有空间

    int openMap(ABMException &oExp, const char *sFile, int iSize=0);

    //openMap后 调用该函数获得共享内存首地址
    void *getMap() {
        return m_pvAddr;
    }
    
    int getSize() {
        return m_iSize;    
    }

    int synFile(void *pAddr, int iSize);    //共享内存的修改刷新到文件

    int closeMap();                        //释放共享内存 

private:

    char m_sFile[512];

    int m_iSize;

    void *m_pvAddr;
};

#endif/*__MMAP_FILE_H_INCLUDED_*/

