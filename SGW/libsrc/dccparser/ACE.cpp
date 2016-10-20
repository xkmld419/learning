// ACE.cpp,v 4.357 2003/11/05 23:30:46 shuston Exp

#include "ACE.h"

size_t ACE::round_to_pagesize (off_t len)
{
	static size_t pagesize_ = 0;
  if (pagesize_ == 0)
    pagesize_ = 4096;

  return (len + (pagesize_ - 1)) & ~(pagesize_ - 1);
}
