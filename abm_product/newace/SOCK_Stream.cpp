// SOCK_Stream.cpp,v 4.16 2005/10/28 16:14:55 ossama Exp

#include "SOCK_Stream.h"

ACE_ALLOC_HOOK_DEFINE(ACE_SOCK_Stream)

void
ACE_SOCK_Stream::dump (void) const
{
#if defined (ACE_HAS_DUMP)
  ACE_TRACE ("ACE_SOCK_Stream::dump");
#endif /* ACE_HAS_DUMP */
}

int
ACE_SOCK_Stream::close (void)
{
  // Close down the socket.
  return ACE_SOCK::close ();
}

