// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.
#ifndef DIRECTORY_H_HEADER_INCLUDED_C02647E2
#define DIRECTORY_H_HEADER_INCLUDED_C02647E2
class File;
#include "File.h"
#include <dirent.h>


//##ModelId=41DCAC7D03BF
//##Documentation
//## 目录类
class Directory
{
  public:
    //##ModelId=41DCAC7E0028
    //##Documentation
    //## 对象构造(路径)
    Directory(char *sPath);

    //##ModelId=41DCAC7E0031
    //##Documentation
    //## 对象构造函数(不带路径参数,取当前路径)
    Directory();

    //##ModelId=41DCAC7E0032
    //##Documentation
    //## 析构
    ~Directory();

    //##ModelId=41DCAC7E0033
    //##Documentation
    //## 创建目录(支持多级目录)
    bool makeDir(char *subDir = "");

    //##ModelId=41DCAC7E003B
    //##Documentation
    //## 取得当前目录的文件列表
    int getFileList();

    //##ModelId=41DCAC7E003C
    //##Documentation
    //## 释放目录对象的文件列表
    int freeFileList();

    //##ModelId=41DCAC7E003D
    //##Documentation
    //## 取得当前目录的文件数量
    int getFileNum();

    //##ModelId=41DCAC7E003E
    //##Documentation
    //## 取得第i个文件
    File *getFile(
	//##Documentation
	//## 下标
	int index = 0);

    //##ModelId=41DCAC7E0046
    //##Documentation
    //## 取得目录名
    char *getPathName();

    //##ModelId=41DCAC7E0047
    //##Documentation
    //## 遍历目录中的文件,取下一个.
    File *nextFile();



  private:
    //##ModelId=41DCDA490010
    //##Documentation
    //## 目录中的文件个数
    int m_iFileNum;

    //##ModelId=41DCDA49009C
    //##Documentation
    //## 路径名
    char m_sPathName[100];

    //##ModelId=41DCDA4900D9
    //##Documentation
    //## 目录中的文件列表
    File *m_poFileList;

    //##ModelId=41DCDA4900E3
    //##Documentation
    //## 遍历时当前文件
    File m_oCurFile;

    //##ModelId=41DCDA8001FA
    //##Documentation
    //## 目录指针
    DIR *m_pDir;



};



#endif /* DIRECTORY_H_HEADER_INCLUDED_C02647E2 */



