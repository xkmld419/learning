#ifndef T_EXCEPTION_H__
#define T_EXCEPTION_H__

class TException
{
public:
  TException();
  TException(const char* err);
  virtual ~TException();
  
  virtual char* GetErrMsg() const;
private:
  char errMsg[1024+1];
};
#if !defined(_WIN32)
#include <strings.h>
#endif

#endif // EXCEPTION_H__
