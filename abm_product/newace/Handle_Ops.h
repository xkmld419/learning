// -*- C++ -*-

//=============================================================================
/**
 *  @file   Handle_Ops.h
 *
 *  Handle_Ops.h,v 1.6 2003/07/19 19:04:11 dhinton Exp
 *
 * This class consolidates the operations on the Handles.
 *
 *
 *  @author Priyanka Gontla <pgontla@ece.uci.edu>
 */
//=============================================================================

#ifndef ACE_HANDLE_OPS_H
#define ACE_HANDLE_OPS_H


#include "macros.h"

//class ACE_Time_Value;

class ACE_Handle_Ops
{
public:
  // = Operations on HANDLEs.

  /**
   * Wait up to <timeout> amount of time to actively open a device.
   * This method doesn't perform the <connect>, it just does the timed
   * wait...
   */
  static ACE_HANDLE handle_timed_open (ACE_Time_Value *timeout,
                                       const ACE_TCHAR *name,
                                       int flags,
                                       int perms,
                                       LPSECURITY_ATTRIBUTES sa = 0);
};


#endif /* ACE_HANDLE_OPS_H */
