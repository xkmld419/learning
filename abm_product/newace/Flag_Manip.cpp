// Flag_Manip.cpp,v 1.4 2004/03/29 09:05:20 jwillemsen Exp

#include "Flag_Manip.h"
#include "Log_Msg.h"


// Flags are file status flags to turn on.

int
ACE_Flag_Manip::set_flags (ACE_HANDLE handle, int flags)
{
  ACE_TRACE ("ACE_Flag_Manip::set_flags");
#if defined (ACE_WIN32) || defined (VXWORKS) || defined (ACE_LACKS_FCNTL)
  switch (flags)
    {
    case ACE_NONBLOCK:
      // nonblocking argument (1)
      // blocking:            (0)
      {
        u_long nonblock = 1;
        return ACE_OS::ioctl (handle, FIONBIO, &nonblock);
      }
    default:
      ACE_NOTSUP_RETURN (-1);
    }
#else
  int val = ACE_OS::fcntl (handle, F_GETFL, 0);

  if (val == -1)
    return -1;

  // Turn on flags.
  ACE_SET_BITS (val, flags);

  if (ACE_OS::fcntl (handle, F_SETFL, val) == -1)
    return -1;
  else
    return 0;
#endif /* ACE_WIN32 || ACE_LACKS_FCNTL */
}

// Flags are the file status flags to turn off.

int
ACE_Flag_Manip::clr_flags (ACE_HANDLE handle, int flags)
{
  ACE_TRACE ("ACE_Flag_Manip::clr_flags");

#if defined (ACE_WIN32) || defined (VXWORKS) || defined (ACE_LACKS_FCNTL)
  switch (flags)
    {
    case ACE_NONBLOCK:
      // nonblocking argument (1)
      // blocking:            (0)
      {
        u_long nonblock = 0;
        return ACE_OS::ioctl (handle, FIONBIO, &nonblock);
      }
    default:
      ACE_NOTSUP_RETURN (-1);
    }
#else
  int val = ACE_OS::fcntl (handle, F_GETFL, 0);

  if (val == -1)
    return -1;

  // Turn flags off.
  ACE_CLR_BITS (val, flags);

  if (ACE_OS::fcntl (handle, F_SETFL, val) == -1)
    return -1;
  else
    return 0;
#endif /* ACE_WIN32 || ACE_LACKS_FCNTL */
}
