// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_sys_socket.h
 *
 *  OS_NS_sys_socket.h,v 1.5 2003/11/06 00:57:56 bala Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef ACE_OS_NS_SYS_SOCKET_H
#define ACE_OS_NS_SYS_SOCKET_H

#if defined (ACE_HAS_VOIDPTR_SOCKOPT)
typedef void *ACE_SOCKOPT_TYPE1;
#elif defined (ACE_HAS_CHARPTR_SOCKOPT)
typedef char *ACE_SOCKOPT_TYPE1;
#else
typedef const char *ACE_SOCKOPT_TYPE1;
#endif /* ACE_HAS_VOIDPTR_SOCKOPT */

#include "macros.h"
#include "config.h"
#include "Log_Msg.h"
#include "Trace.h"
#include "Time_Value.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>


namespace ACE_OS {

# if defined (ACE_WIN32)
  /// Keeps track of whether we've already initialized WinSock...
  extern  int socket_initialized_;
# endif /* ACE_WIN32 */


  /// Initialize WinSock before first use (e.g., when a DLL is first
  /// loaded or the first use of a socket() call.
  extern 
  int socket_init (int version_high = 1,
                   int version_low = 1);

  /// Finalize WinSock after last use (e.g., when a DLL is unloaded).
  extern 
  int socket_fini (void);


  
  inline ACE_HANDLE
  accept (ACE_HANDLE handle,
                  struct sockaddr *addr,
                  int *addrlen)
  {
  
  #if defined (ACE_WIN32)
    ACE_SOCKCALL_RETURN (::accept ((ACE_SOCKET) handle,
                                   addr,
                                   (ACE_SOCKET_LEN *) addrlen),
                         ACE_HANDLE,
                         ACE_INVALID_HANDLE);
  #else
  #if defined (ACE_HAS_BROKEN_ACCEPT_ADDR)
    // Apparently some platforms like VxWorks can't correctly deal with
    // a NULL addr.
  
     sockaddr_in fake_addr;
     int fake_addrlen;
  
     if (addrlen == 0)
       addrlen = &fake_addrlen;
  
     if (addr == 0)
       {
         addr = (sockaddr *) &fake_addr;
         *addrlen = sizeof fake_addr;
       }
  #endif /* VXWORKS */
    ACE_HANDLE ace_result = ::accept ((ACE_SOCKET) handle,
                                      addr,
                                      (ACE_SOCKET_LEN *) addrlen) ;
    if (ace_result == ACE_INVALID_HANDLE && errno == EAGAIN)
      errno = EWOULDBLOCK;
    return ace_result;
  
  #endif /* defined (ACE_WIN32) */
  }
  
  inline int bind (ACE_HANDLE handle, struct sockaddr *addr, int addrlen)
  {
    ACE_TRACE ("bind");
  #if defined (ACE_PSOS) && !defined (ACE_PSOS_DIAB_PPC)
    ACE_SOCKCALL_RETURN (::bind ((ACE_SOCKET) handle,
                                 (struct sockaddr_in *) addr,
                                 (ACE_SOCKET_LEN) addrlen),
                         int, -1);
  #else /* !defined (ACE_PSOS) || defined (ACE_PSOS_DIAB_PPC) */
    ACE_SOCKCALL_RETURN (::bind ((ACE_SOCKET) handle,
                                 addr,
                                 (ACE_SOCKET_LEN) addrlen), int, -1);
  #endif /* defined (ACE_PSOS) && !defined (ACE_PSOS_DIAB_PPC) */
  }
  
  inline int closesocket (ACE_HANDLE handle)
  {
    ACE_TRACE ("closesocket");
  #if defined (ACE_WIN32)
    shutdown (handle, SD_SEND);
    ACE_SOCKCALL_RETURN (::closesocket ((SOCKET) handle), int, -1);
  #else
  #if defined (HPUX)
    // HP-UX, at least with current BSD socket semantics, will not abort
    // an in-progress operation on a socket that is close()ed. But it will
    // for one that is shutdown()ed.
    shutdown (handle, SHUT_RDWR);
  #endif /* HPUX */
    ACE_OSCALL_RETURN (::close (handle), int, -1);
  #endif /* ACE_WIN32 */
  }
  
  inline int connect (ACE_HANDLE handle,
                   struct sockaddr *addr,
                   int addrlen)
  {
    ACE_TRACE ("connect");
    ACE_SOCKCALL_RETURN (::connect ((ACE_SOCKET) handle,
                                    addr,
                                    (ACE_SOCKET_LEN) addrlen), int, -1);
  }
  
  
  inline int
  getpeername (ACE_HANDLE handle, struct sockaddr *addr,
                       int *addrlen)
  {
    ACE_TRACE ("getpeername");
    ACE_SOCKCALL_RETURN (::getpeername ((ACE_SOCKET) handle,
                                        addr,
                                        (ACE_SOCKET_LEN *) addrlen),
                         int, -1);
  }
  
  inline int
  getsockname (ACE_HANDLE handle,
                       struct sockaddr *addr,
                       int *addrlen)
  {
    ACE_TRACE ("getsockname");
    ACE_SOCKCALL_RETURN (::getsockname ((ACE_SOCKET) handle,
                                        addr,
                                        (ACE_SOCKET_LEN *) addrlen),
                         int, -1);
  }
  
  inline int
  getsockopt (ACE_HANDLE handle,
                      int level,
                      int optname,
                      char *optval,
                      int *optlen)
  {
    ACE_TRACE ("getsockopt");
    ACE_SOCKCALL_RETURN (::getsockopt ((ACE_SOCKET) handle,
                                       level,
                                       optname,
                                       optval,
                                       (ACE_SOCKET_LEN *) optlen),
                         int,
                         -1);
  }
  
  inline int
  listen (ACE_HANDLE handle, int backlog)
  {
    ACE_TRACE ("listen");
    ACE_SOCKCALL_RETURN (::listen ((ACE_SOCKET) handle, backlog), int, -1);
  }
  
  inline int
  recv (ACE_HANDLE handle, char *buf, size_t len, int flags)
  {
    ACE_TRACE ("recv");
  
    // On UNIX, a non-blocking socket with no data to receive, this
    // system call will return EWOULDBLOCK or EAGAIN, depending on the
    // platform.  UNIX 98 allows either errno, and they may be the same
    // numeric value.  So to make life easier for upper ACE layers as
    // well as application programmers, always change EAGAIN to
    // EWOULDBLOCK.  Rather than hack the ACE_OSCALL_RETURN macro, it's
    // handled explicitly here.  If the ACE_OSCALL macro ever changes,
    // this function needs to be reviewed.  On Win32, the regular macros
    // can be used, as this is not an issue.
  #if defined (ACE_WIN32)
    ACE_SOCKCALL_RETURN (::recv ((ACE_SOCKET) handle, buf,
                                 ACE_static_cast (int, len), flags), int, -1);
  #else
    int ace_result_;
    ace_result_ = ::recv ((ACE_SOCKET) handle, buf, len, flags);
    if (ace_result_ == -1 && errno == EAGAIN)
      errno = EWOULDBLOCK;
    return ace_result_;
  #endif /* defined (ACE_WIN32) */
  }
  
  inline int recvfrom (ACE_HANDLE handle,
                    char *buf,
                    size_t len,
                    int flags,
                    struct sockaddr *addr,
                    int *addrlen)
  {
    ACE_TRACE ("recvfrom");
  #if defined (ACE_WIN32)
    int shortened_len = ACE_static_cast (int, len);
    int result = ::recvfrom ((ACE_SOCKET) handle,
                             buf,
                             shortened_len,
                             flags,
                             addr,
                             (ACE_SOCKET_LEN *) addrlen);
    if (result == SOCKET_ERROR)
      {
        set_errno_to_wsa_last_error ();
        if (errno == WSAEMSGSIZE &&
            ACE_BIT_ENABLED (flags, MSG_PEEK))
          return shortened_len;
        else
          return -1;
      }
    else
      return result;
  #else /* non Win32 and non PSOS */
    ACE_SOCKCALL_RETURN (::recvfrom ((ACE_SOCKET) handle, buf, len, flags,
                                     addr, (ACE_SOCKET_LEN *) addrlen),
                         int, -1);
  #endif /* defined (ACE_PSOS) */
  }
  
  
  
  inline ssize_t
  recvv (ACE_HANDLE handle,
                 iovec *buffers,
                 int n)
  {
  #if defined (ACE_HAS_WINSOCK2)
  
    DWORD bytes_received = 0;
    int result = 1;
  
    // Winsock 2 has WSARecv and can do this directly, but Winsock 1 needs
    // to do the recvs piece-by-piece.
  
  #if (ACE_HAS_WINSOCK2 != 0)
    DWORD flags = 0;
    result = ::WSARecv ((SOCKET) handle,
                        (WSABUF *) buffers,
                        n,
                        &bytes_received,
                        &flags,
                        0,
                        0);
  #else
    int i, chunklen;
    char *chunkp = 0;
  
    // Step through the buffers requested by caller; for each one, cycle
    // through reads until it's filled or an error occurs.
    for (i = 0; i < n && result > 0; i++)
      {
        chunkp = buffers[i].iov_base;     // Point to part of chunk being read
        chunklen = buffers[i].iov_len;    // Track how much to read to chunk
        while (chunklen > 0 && result > 0)
          {
            result = ::recv ((SOCKET) handle, chunkp, chunklen, 0);
            if (result > 0)
              {
                chunkp += result;
                chunklen -= result;
                bytes_received += result;
              }
          }
      }
  #endif /* ACE_HAS_WINSOCK2 != 0 */
  
    if (result == SOCKET_ERROR)
      {
        set_errno_to_wsa_last_error ();
        return -1;
      }
    else
      return (ssize_t) bytes_received;
  #else
    return readv (handle, buffers, n);
  #endif /* ACE_HAS_WINSOCK2 */
  }
  
  inline int
  send (ACE_HANDLE handle, const char *buf, size_t len, int flags)
  {
    ACE_TRACE ("send");
  
    // On UNIX, a non-blocking socket with no data to receive, this
    // system call will return EWOULDBLOCK or EAGAIN, depending on the
    // platform.  UNIX 98 allows either errno, and they may be the same
    // numeric value.  So to make life easier for upper ACE layers as
    // well as application programmers, always change EAGAIN to
    // EWOULDBLOCK.  Rather than hack the ACE_OSCALL_RETURN macro, it's
    // handled explicitly here.  If the ACE_OSCALL macro ever changes,
    // this function needs to be reviewed.  On Win32, the regular macros
    // can be used, as this is not an issue.
  #if defined (ACE_WIN32)
    ACE_SOCKCALL_RETURN (::send ((ACE_SOCKET) handle,
                                 buf,
                                 ACE_static_cast (int, len),
                                 flags), int, -1);
  #else
    int ace_result_;
  #if defined (VXWORKS) || defined (HPUX) || defined (ACE_PSOS)
    ace_result_ = ::send ((ACE_SOCKET) handle, (char *) buf, len, flags);
  #else
    ace_result_ = ::send ((ACE_SOCKET) handle, buf, len, flags);
  #endif /* VXWORKS */
    if (ace_result_ == -1 && errno == EAGAIN)
      errno = EWOULDBLOCK;
    return ace_result_;
  #endif /* defined (ACE_WIN32) */
  }
  
  
  inline int sendto (ACE_HANDLE handle,
                  const char *buf,
                  size_t len,
                  int flags,
                  const struct sockaddr *addr,
                  int addrlen)
  {
    ACE_TRACE ("sendto");
  #if defined (ACE_WIN32)
    ACE_SOCKCALL_RETURN (::sendto ((ACE_SOCKET) handle, buf,
                                   ACE_static_cast (int, len), flags,
                                   ACE_const_cast (struct sockaddr *, addr), addrlen),
                         int, -1);
  #else
    ACE_SOCKCALL_RETURN (::sendto ((ACE_SOCKET) handle, buf, len, flags,
                                   (struct sockaddr *) addr, addrlen),
                         int, -1);
  #endif /* ACE_WIN32 */
  }
  
  
  inline ssize_t
  sendv (ACE_HANDLE handle,
                 const iovec *buffers,
                 int n)
  {
  #if defined (ACE_HAS_WINSOCK2)
    DWORD bytes_sent = 0;
    int result = 0;
  
    // Winsock 2 has WSASend and can do this directly, but Winsock 1
    // needs to do the sends one-by-one.
  #if (ACE_HAS_WINSOCK2 != 0)
    result = ::WSASend ((SOCKET) handle,
                        (WSABUF *) buffers,
                        n,
                        &bytes_sent,
                        0,
                        0,
                        0);
  #else
    int i;
    for (i = 0; i < n && result != SOCKET_ERROR; i++)
      {
        result = ::send ((SOCKET) handle,
                         buffers[i].iov_base,
                         buffers[i].iov_len,
                         0);
        // Gets ignored on error anyway
        bytes_sent += buffers[i].iov_len;
  
        // If the transfer isnt complete just drop out of the loop.
        if (result < (int)buffers[i].iov_len)
          break;
      }
  #endif /* ACE_HAS_WINSOCK2 != 0 */
  
    if (result == SOCKET_ERROR)
      {
        set_errno_to_wsa_last_error ();
        return -1;
      }
    else
      return (ssize_t) bytes_sent;
  
  #else
    return writev (handle, buffers, n);
  #endif /* ACE_HAS_WINSOCK2 */
  }
  
  inline int
  setsockopt (ACE_HANDLE handle,
                      int level,
                      int optname,
                      const char *optval,
                      int optlen)
  {
    ACE_TRACE ("setsockopt");
  
    #if defined (ACE_HAS_WINSOCK2) && (ACE_HAS_WINSOCK2 != 0) && defined(SO_REUSEPORT)
    // To work around an inconsistency with Microsofts implementation of
    // sockets, we will check for SO_REUSEADDR, and ignore it. Winsock
    // always behaves as if SO_REUSEADDR=1. Some implementations have the
    // same behaviour as Winsock, but use a new name for it. SO_REUSEPORT.
    // If you want the normal behaviour for SO_REUSEADDR=0, then NT 4 sp4 and later
    // supports SO_EXCLUSIVEADDRUSE. This also requires using an updated Platform SDK
    // so it was decided to ignore the option for now. (Especially since ACE always
    // sets SO_REUSEADDR=1, which we can mimic by doing nothing.)
    if (level == SOL_SOCKET) {
      if (optname == SO_REUSEADDR) {
        return 0; // Not supported by Winsock
      }
      if (optname == SO_REUSEPORT) {
        optname = SO_REUSEADDR;
      }
    }
    #endif /*ACE_HAS_WINSOCK2*/
  
    ACE_SOCKCALL_RETURN (::setsockopt ((ACE_SOCKET) handle,
                                       level,
                                       optname,
                                       (ACE_SOCKOPT_TYPE1) optval,
                                       optlen),
                         int,
                         -1);
  }
  
  inline int
  shutdown (ACE_HANDLE handle, int how)
  {
    ACE_TRACE ("shutdown");
    ACE_SOCKCALL_RETURN (::shutdown ((ACE_SOCKET) handle, how), int, -1);
  }
  
  inline ACE_HANDLE
  socket (int domain,
                  int type,
                  int proto)
  {
    ACE_TRACE ("socket");
    ACE_SOCKCALL_RETURN (::socket (domain,
                                   type,
                                   proto),
                         ACE_HANDLE,
                         ACE_INVALID_HANDLE);
  }
  

  inline ACE_HANDLE socket (int domain,
                int type,
                int proto,
                ACE_Protocol_Info *protocolinfo,
                ACE_SOCK_GROUP g,
                u_long flags)
  {
    ACE_TRACE ("ACE_OS::socket");
  
  #if defined (ACE_HAS_WINSOCK2) && (ACE_HAS_WINSOCK2 != 0)
    ACE_SOCKCALL_RETURN (::WSASocket (domain,
                                      type,
                                      proto,
                                      protocolinfo,
                                      g,
                                      flags),
                         ACE_HANDLE,
                         ACE_INVALID_HANDLE);
  #else
    ACE_UNUSED_ARG (protocolinfo);
    ACE_UNUSED_ARG (g);
    ACE_UNUSED_ARG (flags);
  
    return ACE_OS::socket (domain,
                           type,
                           proto);
  #endif /* ACE_HAS_WINSOCK2 */
  }


  
  
  inline int
  socketpair (int domain, int type,
                      int protocol, ACE_HANDLE sv[2])
  {
    ACE_TRACE ("socketpair");
  #if defined (ACE_WIN32) || defined (ACE_LACKS_SOCKETPAIR)
    ACE_UNUSED_ARG (domain);
    ACE_UNUSED_ARG (type);
    ACE_UNUSED_ARG (protocol);
    ACE_UNUSED_ARG (sv);
  
    ACE_NOTSUP_RETURN (-1);
  #else
    ACE_OSCALL_RETURN (::socketpair (domain, type, protocol, sv),
                       int, -1);
  #endif /* ACE_WIN32 */
  }
  
  #if defined (__linux__) && defined (ACE_HAS_IPV6)
  inline unsigned int
  if_nametoindex (const char *ifname)
  {
    ACE_TRACE ("if_nametoindex");
    ACE_OSCALL_RETURN (::if_nametoindex (ifname), int, 0);
  }
  
  inline char *
  if_indextoname (unsigned int ifindex, char *ifname)
  {
    ACE_TRACE ("if_indextoname");
    ACE_OSCALL_RETURN (::if_indextoname (ifindex, ifname), char *, 0);
  }
  
  inline struct if_nameindex *
  if_nameindex (void)
  {
    ACE_TRACE ("if_nameindex");
    ACE_OSCALL_RETURN (::if_nameindex (), struct if_nameindex *, 0);
  }
  
  inline void if_freenameindex (struct if_nameindex *ptr)
  {
    ACE_TRACE ("if_freenameindex");
    if (ptr != 0)
     ::if_freenameindex (ptr);
  }
  #endif

  inline ACE_Time_Value gettimeofday (void)
  {
    timeval tv;
    int result = 0;
    ACE_OSCALL (::gettimeofday (&tv, NULL), int, -1, result);
    if (result == -1)
      return ACE_Time_Value();
    else
    return ACE_Time_Value (tv);
  }

} /* namespace ACE_OS */


#endif /* ACE_OS_NS_SYS_SOCKET_H */
