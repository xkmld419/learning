// SOCK_IO.cpp,v 4.35 2006/02/26 19:21:00 jwillemsen Exp

#include "SOCK_IO.h"

#include "Handle_Set.h"
//#include <sys/select.h>
#include <net/if.h>
#include "OS_Memory.h"
//#include "Truncate.h"


ACE_ALLOC_HOOK_DEFINE(ACE_SOCK_IO)

void
ACE_SOCK_IO::dump (void) const
{
#if defined (ACE_HAS_DUMP)
  ACE_TRACE ("ACE_SOCK_IO::dump");
#endif /* ACE_HAS_DUMP */
}

// Allows a client to read from a socket without having to provide
// a buffer to read.  This method determines how much data is in the
// socket, allocates a buffer of this size, reads in the data, and
// returns the number of bytes read.


// Send N char *ptrs and int lengths.  Note that the char *'s precede
// the ints (basically, an varargs version of writev).  The count N is
// the *total* number of trailing arguments, *not* a couple of the
// number of tuple pairs!

