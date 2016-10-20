// -*- C++ -*-
// OS_NS_sys_socket.cpp,v 1.2 2003/11/01 11:15:15 dhinton Exp

#include "OS_NS_sys_socket.h"

#if defined (ACE_WIN32)
int ACE_OS::socket_initialized_;
#endif /* ACE_WIN32 */


int
ACE_OS::socket_init (int version_high, int version_low)
{
# if defined (ACE_WIN32)
  if (ACE_OS::socket_initialized_ == 0)
    {
      WORD version_requested = MAKEWORD (version_high, version_low);
      WSADATA wsa_data;
      int error = WSAStartup (version_requested, &wsa_data);

      if (error != 0)
#   if defined (ACE_HAS_WINCE)
        {
          wchar_t fmt[] = ACE_LIB_TEXT ("%s failed, WSAGetLastError returned %d");
          wchar_t buf[80];  // @@ Eliminate magic number.
          sprintf (buf, fmt, ACE_LIB_TEXT ("WSAStartup"), error);
          ::MessageBox (0, buf, ACE_LIB_TEXT ("WSAStartup failed!"), MB_OK);
        }
#   else
      fprintf (stderr,
                       "ACE_OS::socket_init; WSAStartup failed, "
                         "WSAGetLastError returned %d\n",
                       error);
#   endif /* ACE_HAS_WINCE */

      ACE_OS::socket_initialized_ = 1;
    }
# else
  ACE_UNUSED_ARG (version_high);
  ACE_UNUSED_ARG (version_low);
# endif /* ACE_WIN32 */
  return 0;
}

int
ACE_OS::socket_fini (void)
{
# if defined (ACE_WIN32)
  if (ACE_OS::socket_initialized_ != 0)
    {
      if (WSACleanup () != 0)
        {
          int error = ::WSAGetLastError ();
#   if defined (ACE_HAS_WINCE)
          wchar_t fmt[] = ACE_LIB_TEXT ("%s failed, WSAGetLastError returned %d");
          wchar_t buf[80];  // @@ Eliminate magic number.
          sprintf (buf, fmt, ACE_LIB_TEXT ("WSACleanup"), error);
          ::MessageBox (0, buf , ACE_LIB_TEXT ("WSACleanup failed!"), MB_OK);
#   else
          fprintf (stderr,
                           "ACE_OS::socket_fini; WSACleanup failed, "
                             "WSAGetLastError returned %d\n",
                           error);
#   endif /* ACE_HAS_WINCE */
        }
      ACE_OS::socket_initialized_ = 0;
    }
# endif /* ACE_WIN32 */
  return 0;
}

