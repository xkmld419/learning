// -*- C++ -*-

//==========================================================================
/**
 *  @file    SOCK_IO.h
 *
 *  SOCK_IO.h,v 4.38 2006/02/25 23:01:14 shuston Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//==========================================================================

#ifndef ACE_SOCK_IO_H
#define ACE_SOCK_IO_H

#include "SOCK.h"
#include "OS_NS_sys_select.h"
#include "OS_NS_sys_socket.h"

#include "macros.h"

/**
 * @class ACE_SOCK_IO
 *
 * @brief Defines the methods for the ACE socket wrapper I/O routines
 * (e.g., send/recv).
 *
 *
 * If <timeout> == 0, then the call behaves as a normal
 * send/recv call, i.e., for blocking sockets, the call will
 * block until action is possible; for non-blocking sockets,
 * EWOULDBLOCK will be returned if no action is immediately
 * possible.
 * If <timeout> != 0, the call will wait until the relative time
 * specified in *<timeout> elapses.
 * Errors are reported by -1 and 0 return values.  If the
 * operation times out, -1 is returned with <errno == ETIME>.
 * If it succeeds the number of bytes transferred is returned.
 * Methods with the extra <flags> argument will always result in
 * <send> getting called. Methods without the extra <flags>
 * argument will result in <send> getting called on Win32
 * platforms, and <write> getting called on non-Win32 platforms.
 */

class ACE_SOCK_IO : public ACE_SOCK
{
public:
  // = Initialization and termination methods.

  inline
  ACE_SOCK_IO (void)
  {
    // ACE_TRACE ("ACE_SOCK_IO");
  }
  
  inline
  ~ACE_SOCK_IO (void)
  {
    // ACE_TRACE ("~ACE_SOCK_IO");
  }
  
  inline ssize_t recv (void *buf,
                     size_t len,
                     int flags,
                     const ACE_Time_Value *timeout = 0) const
  {
    ACE_TRACE ("recv");
    return ACE::recv (this->get_handle (),
                      buf,
                      len,
                      flags,
                      timeout);
  }
  
  inline ssize_t recv (void *buf,
                     size_t len,
                     const ACE_Time_Value *timeout = 0) const
  {
    ACE_TRACE ("recv");
    return ACE::recv (this->get_handle (),
                      buf,
                      len,
                      timeout);
  }
  
 
  inline ssize_t
  send (const void *buf,
                     size_t len,
                     int flags,
                     const ACE_Time_Value *timeout =0 ) const
  {
    ACE_TRACE ("send");
    return ACE::send (this->get_handle (),
                      buf,
                      len,
                      flags,
                      timeout);
  }
  
  inline ssize_t
  send (const void *buf,
                     size_t len,
                     const ACE_Time_Value *timeout = 0) const
  {
    ACE_TRACE ("send");
    return ACE::send (this->get_handle (),
                      buf,
                      len,
                      timeout);
  }
  


  /// Dump the state of an object.
  void dump (void) const;

  /// Declare the dynamic allocation hooks.
  ACE_ALLOC_HOOK_DECLARE;
};

#endif /* ACE_SOCK_IO_H */
