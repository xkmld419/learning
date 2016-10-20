/*VER: 1*/ 
#include "TBatchInsert.h"
#include <stdlib.h>
template <class  TDataStruct > 
TBatchInsert<TDataStruct>::TBatchInsert(TDataStruct & d)
{
	AllocBuffer();
}


template <class  TDataStruct > 
TBatchInsert<TDataStruct>::~TBatchInsert()
{
	FreeBuffer();
}

template <class  TDataStruct > 
void TBatchInsert<TDataStruct>::SetTableName(char * sName)
{
	strcpy ( sTableName, sName);
}

template <class  TDataStruct > 
void TBatchInsert<TDataStruct>::SetConnection(TOCIDatabase * db)
{
	db_connection = db;
}

template <class  TDataStruct > 
void TBatchInsert<TDataStruct>::AllocBuffer()
{
    pBuffer = (TDataStruct *)malloc( sizeof(TDataStruct) * LOAD_BUFFER_SIZE);
    if ( pBuffer ==NULL){
        throw TException("malloc failed: SettCallTicketLoad::SettCallTicketLoad(TOCIDatabase * db)");
    }
    
    iBufferSize = LOAD_BUFFER_SIZE;
    iRecordCount = 0;
}

template <class  TDataStruct > 
void TBatchInsert<TDataStruct>::FreeBuffer()
{
	free(pBuffer);
}


