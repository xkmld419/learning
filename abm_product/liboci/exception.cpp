/*VER: 1*/ 
#include <string.h>
#include <stdio.h>
#include "exception.h"

TException::TException(const char* err)
{
  int nLen;
  nLen = strlen( err );
  if( nLen >= 1024 )
    nLen = 1024;
  strncpy(errMsg,err,nLen);
  errMsg[nLen] = '\0';
};

TException::TException()
{
  char *err = "fatal error£ºsystem undefine exception error";
  strcpy(errMsg,err);
  errMsg[strlen(err)] = '\0';
};

TException::~TException()
{
};
char* TException::GetErrMsg() const
{
  return( (char*)errMsg );
};
