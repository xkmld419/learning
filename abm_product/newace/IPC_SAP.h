/* -*- C++ -*- */

//=============================================================================
/**
 *  @file    IPC_SAP.h
 *
 *  IPC_SAP.h,v 4.18 2003/07/19 19:04:11 dhinton Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//=============================================================================

#ifndef ACE_IPC_SAP_H
#define ACE_IPC_SAP_H
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include "ACE.h"

/**
 * @class ACE_IPC_SAP
 *
 * @brief Defines the member functions for the base class of the
 * ACE_IPC_SAP abstraction.
 */
class ACE_IPC_SAP
{
public:
  /// Default dtor.
//  ~ACE_IPC_SAP (void);
  inline ~ACE_IPC_SAP (void)
  {
    // ACE_TRACE ("~ACE_IPC_SAP");
  }
  
  inline ACE_HANDLE get_handle (void) const  {
    ACE_TRACE ("get_handle");
    return this->handle_;
  }
  
  // Used to set the underlying handle_. 
  
  inline void set_handle (ACE_HANDLE handle)
  {
    ACE_TRACE ("set_handle");
    this->handle_ = handle;
  }
  
  // Provides access to the ioctl system call. 
  
  inline int control (int cmd, void *arg) const
  {
    ACE_TRACE ("control");
    return ioctl (handle_, cmd, arg);
  }


  // = Common I/O handle options related to sockets.

  /**
   * Enable asynchronous I/O (ACE_SIGIO), urgent data (ACE_SIGURG),
   * non-blocking I/O (ACE_NONBLOCK), or close-on-exec (ACE_CLOEXEC),
   * which is passed as the <value>.
   */
  int enable (int value) const;

  /**
   * Disable asynchronous I/O (ACE_SIGIO), urgent data (ACE_SIGURG),
   * non-blocking I/O (ACE_NONBLOCK), or close-on-exec (ACE_CLOEXEC),
   * which is passed as the <value>.
   */
  int disable (int value) const;

  /// Dump the state of an object.
  void dump (void) const;

  /// Declare the dynamic allocation hooks.
  ACE_ALLOC_HOOK_DECLARE;

protected:
  // = Ensure that ACE_IPC_SAP is an abstract base class.
  /// Default constructor.
  ACE_IPC_SAP (void);

private:
  /// Underlying I/O handle.
  ACE_HANDLE handle_;

  /// Cache the process ID.
  static pid_t pid_;
};

#endif /* ACE_IPC_SAP_H */
