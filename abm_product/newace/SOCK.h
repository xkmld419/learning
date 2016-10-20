/* -*- C++ -*- */

//=============================================================================
/**
 *  @file    SOCK.h
 *
 *  SOCK.h,v 4.40 2005/10/28 16:14:55 ossama Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//=============================================================================

#ifndef ACE_SOCK_H
#define ACE_SOCK_H

#include "Addr.h"
#include "IPC_SAP.h"
#include "macros.h"
#include <sys/types.h>
#include <sys/socket.h>

/* struct ACE_Protocol_Info
{
  unsigned long dwServiceFlags1;
  int iAddressFamily;
  int iProtocol;
  char szProtocol[255+1];
};*/
/**
 * @class ACE_SOCK
 *
 * @brief An abstract class that forms the basis for more specific
 * classes, such as <ACE_SOCK_Acceptor> and <ACE_SOCK_Stream>.
 * Do not instantiate this class.
 *
 * This class provides functions that are common to all of the
 * <ACE_SOCK_*> classes. <ACE_SOCK> provides the ability to get
 * and set socket options, get the local and remote addresses,
 * and open and close a socket handle.
 */
class ACE_SOCK : public ACE_IPC_SAP
{
  public:
  
  
  inline int set_option (int level,
                        int option,
                        void *optval,
                        int optlen) const
  {
    ACE_TRACE ("set_option");
    return setsockopt (this->get_handle (), level,
                               option, (char *) optval, optlen);
  }
  
  // Provides access to the ACE_OS::getsockopt system call.
  
  inline int get_option (int level,
                        int option,
                        void *optval,
                        int *optlen) const
  {
    ACE_TRACE ("get_option");
#ifdef DEF_HP
    return getsockopt (this->get_handle (), level, option, (char *) optval, optlen);
#else
    return getsockopt (this->get_handle (), level, option, (char *) optval, (socklen_t *)optlen);
#endif
  }


  /**
   * Close the socket.
   * This method also sets the object's handle value to ACE_INVALID_HANDLE.
   *
   * @return The result of closing the socket; 0 if the handle value
   *         was already ACE_INVALID_HANDLE.
   */
  int close (void);

  /// Return the local endpoint address in the referenced <ACE_Addr>.
  /// Returns 0 if successful, else -1.
  int get_local_addr (ACE_Addr &) const;

  /**
   * Return the address of the remotely connected peer (if there is
   * one), in the referenced <ACE_Addr>. Returns 0 if successful, else
   * -1.
   */
  int get_remote_addr (ACE_Addr &) const;

  /// Dump the state of an object.
  void dump (void) const;

  /// Declare the dynamic allocation hooks.
  ACE_ALLOC_HOOK_DECLARE;

  /// Wrapper around the BSD-style <socket> system call (no QoS).
  int open (int type,
            int protocol_family,
            int protocol,
            int reuse_addr);

  /// Wrapper around the QoS-enabled <WSASocket> function.
  int open (int type,
            int protocol_family,
            int protocol,
            ACE_Protocol_Info *protocolinfo,
            ACE_SOCK_GROUP g,
            u_long flags,
            int reuse_addr);

protected:

  /// Constructor with arguments to call the BSD-style <socket> system
  /// call (no QoS).
  ACE_SOCK (int type,
            int protocol_family,
            int protocol = 0,
            int reuse_addr = 0);

  /// Constructor with arguments to call the QoS-enabled <WSASocket>
  /// function.
  ACE_SOCK (int type,
            int protocol_family,
            int protocol,
            ACE_Protocol_Info *protocolinfo,
            ACE_SOCK_GROUP g,
            u_long flags,
            int reuse_addr);

  /// Default constructor is protected to prevent instances of this class
  /// from being defined.
  ACE_SOCK (void);

  /// Protected destructor.
  /**
   * Not a virtual destructor.  Protected destructor to prevent
   * operator delete() from being called through a base class ACE_SOCK
   * pointer/reference.
   */
  inline
  ~ACE_SOCK (void)
  {
    // ACE_TRACE ("~ACE_SOCK");
  }

};


#endif /* ACE_SOCK_H */
