#ifndef __TBATCH_INSERT_H__
#define __TBATCH_INSERT_H__

#include "TOCIQuery.h"
#include "TOCIDirPath.h"
#include <string.h>

#define LOAD_BUFFER_SIZE  50000

//class TBatchInsert;

template <class  TDataStruct > 
class TBatchInsert 
{
protected:
    int   iBufferSize;                  //缓冲大小
    int   iRecordCount;                 //缓冲记录数
    TOCIDatabase * db_connection;       //数据库连接
    char sTableName[81];                //表名
    TDataStruct * pBuffer;
    void  AllocBuffer();       //分配缓冲
    void  FreeBuffer();        //释放缓冲
public:
	TBatchInsert( TDataStruct & );
	~TBatchInsert();
    void  SetTableName(char * sName);       //设置表名
    void  SetConnection(TOCIDatabase * db);  //设置数据库连接

    virtual void  Save();              //话单结构入缓冲
    virtual void  Flush();             //数据缓冲入库
};

#endif



