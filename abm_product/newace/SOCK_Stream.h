// -*- C++ -*-

//=============================================================================
/**
 *  @file    SOCK_Stream.h
 *
 *  SOCK_Stream.h,v 4.33 2005/10/28 16:14:55 ossama Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//=============================================================================

#ifndef ACE_SOCK_STREAM_H
#define ACE_SOCK_STREAM_H

#include "SOCK_IO.h"


#include "INET_Addr.h"

// Forward declarations.
class ACE_Message_Block;

/**
 * @class ACE_SOCK_Stream
 *
 * @brief Defines the methods in the ACE_SOCK_Stream abstraction.
 *
 * This adds additional wrapper methods atop the ACE_SOCK_IO
 * class.
 *
 * <buf> is the buffer to write from or receive into.
 * <len> is the number of bytes to transfer.
 * The <timeout> parameter in the following methods indicates how
 * long to blocking trying to transfer data.  If <timeout> == 0,
 * then the call behaves as a normal send/recv call, i.e., for
 * blocking sockets, the call will block until action is possible;
 * for non-blocking sockets, EWOULDBLOCK will be returned if no
 * action is immediately possible.
 * If <timeout> != 0, the call will wait for data to arrive no longer
 * than the relative time specified in *<timeout>.
 * The "_n()" I/O methods keep looping until all the data has been
 * transferred.  These methods also work for sockets in non-blocking
 * mode i.e., they keep looping on EWOULDBLOCK.  <timeout> is used
 * to make sure we keep making progress, i.e., the same timeout
 * value is used for every I/O operation in the loop and the timeout
 * is not counted down.
 * The return values for the "*_n()" methods match the return values
 * from the non "_n()" methods and are specified as follows:
 * - On complete transfer, the number of bytes transferred is returned.
 * - On timeout, -1 is returned, errno == ETIME.
 * - On error, -1 is returned, errno is set to appropriate error.
 * - On EOF, 0 is returned, errno is irrelevant.
 *
 * On partial transfers, i.e., if any data is transferred before
 * timeout/error/EOF, <bytes_transferred> will contain the number of
 * bytes transferred.
 * Methods with <iovec> parameter are I/O vector variants of the I/O
 * operations.
 * Methods with the extra <flags> argument will always result in
 * <send> getting called. Methods without the extra <flags> argument
 * will result in <send> getting called on Win32 platforms, and
 * <write> getting called on non-Win32 platforms.
 */
class ACE_SOCK_Stream : public ACE_SOCK_IO
{
public:
  // Initialization and termination methods.
  /// Constructor.
  
  inline
  ACE_SOCK_Stream (void)
  {
    // ACE_TRACE ("ACE_SOCK_Stream");
  }
  
  inline
  ACE_SOCK_Stream (ACE_HANDLE h)
  {
    // ACE_TRACE ("ACE_SOCK_Stream");
    this->set_handle (h);
  }
  
  inline
  ~ACE_SOCK_Stream (void)
  {
    // ACE_TRACE ("~ACE_SOCK_Stream");
  }
  
  inline int
  close_reader (void)
  {
    ACE_TRACE ("close_reader");
    if (this->get_handle () != ACE_INVALID_HANDLE)
      return ACE_OS::shutdown (this->get_handle (), ACE_SHUTDOWN_READ);
    else
      return 0;
  }
  
  // Shut down just the writing end of a ACE_SOCK.
  
  inline int
  close_writer (void)
  {
    ACE_TRACE ("close_writer");
    if (this->get_handle () != ACE_INVALID_HANDLE)
      return ACE_OS::shutdown (this->get_handle (), ACE_SHUTDOWN_WRITE);
    else
      return 0;
  }
  
  inline ssize_t
  recv_n (void *buf,
                           size_t len,
                           int flags,
                           const ACE_Time_Value *timeout=0,
                           size_t *bytes_transferred=0) const
  {
    ACE_TRACE ("recv_n");
    return ACE::recv_n (this->get_handle (),
                        buf,
                        len,
                        flags,
                        timeout,
                        bytes_transferred);
  }
  
  inline ssize_t
  recv_n (void *buf,
                           size_t len,
                           const ACE_Time_Value *timeout=0,
                           size_t *bytes_transferred=0) const
  {
    ACE_TRACE ("recv_n");
    return ACE::recv_n (this->get_handle (),
                        buf,
                        len,
                        timeout,
                        bytes_transferred);
  }
  
  
  inline ssize_t
  send_n (const void *buf,
                           size_t len,
                           int flags,
                           const ACE_Time_Value *timeout=0,
                           size_t *bytes_transferred=0) const
  {
    ACE_TRACE ("send_n");
    return ACE::send_n (this->get_handle (),
                        buf,
                        len,
                        flags,
                        timeout,
                        bytes_transferred);
  }
  
  inline ssize_t
  send_n (const void *buf,
                           size_t len,
                           const ACE_Time_Value *timeout=0,
                           size_t *bytes_transferred=0) const
  {
    ACE_TRACE ("send_n");
    return ACE::send_n (this->get_handle (),
                        buf,
                        len,
                        timeout,
                        bytes_transferred);
  }
  
  
  inline ssize_t
  send_urg (const void *ptr,
                             size_t len,
                             const ACE_Time_Value *timeout=0) const
  {
    ACE_TRACE ("send_urg");
    return ACE::send (this->get_handle (),
                      ptr,
                      len,
                      MSG_OOB,
                      timeout);
  }
  
  inline ssize_t
  recv_urg (void *ptr,
                             size_t len,
                             const ACE_Time_Value *timeout=0) const
  {
    ACE_TRACE ("recv_urg");
    return ACE::recv (this->get_handle (),
                      ptr,
                      len,
                      MSG_OOB,
                      timeout);
  }

  /**
   * Close down the socket (we need this to make things work correctly
   * on Win32, which requires use to do a close_writer() before doing
   * the close to avoid losing data).
   */
  int close (void);

  // = Meta-type info
  typedef ACE_INET_Addr PEER_ADDR;

  /// Dump the state of an object.
  void dump (void) const;

  /// Declare the dynamic allocation hooks.
  ACE_ALLOC_HOOK_DECLARE;
};

#endif /* ACE_SOCK_STREAM_H */
