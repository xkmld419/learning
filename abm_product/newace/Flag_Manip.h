// -*- C++ -*-

//=============================================================================
/**
 *  @file   Flag_Manip.h
 *
 *  Flag_Manip.h,v 1.7 2003/07/19 19:04:11 dhinton Exp
 *
 *  This class includes the functions used for the Flag Manipulation.
 *
 *  @author Priyanka Gontla <pgontla@doc.ece.uci.edu>
 */
//=============================================================================

#ifndef ACE_FLAG_MANIP_H
#define ACE_FLAG_MANIP_H

#include <fcntl.h>

class ACE_Flag_Manip
{
 public:

  // = Set/get/clear various flags related to I/O HANDLE.
  /// Set flags associated with <handle>.
  static int set_flags (int  handle,
                        int flags);

  /// Clear flags associated with <handle>.
  static int clr_flags (int  handle,
                        int flags);

  inline static int
  get_flags (int  handle)
  {
    return fcntl (handle, F_GETFL, 0);
  };
}

#endif  /* ACE_FLAG_MANIP_H */
