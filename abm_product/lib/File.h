/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef FILE_H_HEADER_INCLUDED_C02619C0
#define FILE_H_HEADER_INCLUDED_C02619C0

//##ModelId=41DCAC7D03DD
//##Documentation
//## 文件类
class File
{
  public:
    //##ModelId=41DCAC7E0081
    //##Documentation
    //## 构造函数(带路径文件名)
    File(char *pFileName = "");

    //##ModelId=41DCAC7E0083
    //##Documentation
    //## 构造函数(路径, 文件名)
    File(char *pFilePath, char *pFileName);

    //##ModelId=41DCAC7E008E
    //##Documentation
    //## 析构函数
    ~File();

    //##ModelId=41DCAC7E008F
    //##Documentation
    //## 移动文件,且当前对象的文件名也改为新文件
    bool moveFile(
	//##Documentation
	//## 符合要求的目标串: 
	//## example:
	//## 文件名:        aaa.txt
	//## 全路径文件名   /home/bill/aaa.txt
	//## 相对路径文件名 dir1/dir2/aaa.txt
	//## 绝对路径       /home/bill/
	//## 相对路径       dir1/dir2/
	char *newName = "");

    //##ModelId=41DCAC7E0097
    //##Documentation
    //## 删除本文件
    bool rm();

    //##ModelId=41DCAC7E0098
    //##Documentation
    //## 取得当前文件对象的文件名
    char *getFileName();

    //##ModelId=41DCAC7E0099
    //##Documentation
    //## 取得当前文件对象的文件路径
    char *getFilePath();

    //##ModelId=41DCAC7E00A0
    //##Documentation
    //## 取得当前文件对象的全文件名, 带路径
    char *getFullFileName();

    //##ModelId=41DCAC7E00A1
    //##Documentation
    //## 取得当前文件对象的文件大小
    int getSize();

    //##ModelId=41DCAC7E00A2
    //##Documentation
    //## set当前文件对象的文件大小
    int setSize(int size);

    //##ModelId=41DCAC7E00AA
    //##Documentation
    //## 设置或修改文件名
    void setFileName(
	//##Documentation
	//## 新文件名
	char *pFileName = "");

    //##ModelId=41DCAC7E00AC
    //##Documentation
    //## 拷贝文件.
    bool copyFile(
	//##Documentation
	//## 目标文件名. 参考moveFile的参数
	char *newFile = "");

    //##ModelId=41DCAC7E00B5
    //##Documentation
    //## 得到错误串
    char *getError();

    //##ModelId=41DCAC7E00B6
    //##Documentation
    //## 取得文件状态
    char *getState();

    //##ModelId=41DCAC7E00B7
    //##Documentation
    //## 设置文件状态
    bool setState(
	//##Documentation
	//## 状态
	char *pState = "");

  protected:
    //##ModelId=41DCDA41029A
    //##Documentation
    //## 文件路径
    char m_sFilePath[100];

    //##ModelId=41DCDA410312
    //##Documentation
    //## 文件名(不带路径)
    char m_sFileName[100];

    //##ModelId=41DCDA410362
    //##Documentation
    //## 带路径文件名
    char m_sFullFileName[200];

    //##ModelId=41DCDA4103BC
    //##Documentation
    //## 文件大小(字节)
    int m_iFileSize;

  private:
    //##ModelId=41DCAC7E00B9
    //##Documentation
    //## 分离带路径文件名中的路径和文件名(内部调用)
    bool getPathAndFile();

    //##ModelId=41DCDA420024
    //##Documentation
    //## 存放对象操作中的错误串
    char m_sErrorStr[200];

    //##ModelId=41DCDA420074
    //##Documentation
    //## 文件状态
    char m_sState[10];

};



#endif /* FILE_H_HEADER_INCLUDED_C02619C0 */
