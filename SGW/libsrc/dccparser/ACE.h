/* -*- C++ -*- */

//=============================================================================
/**
 *  @file    ACE.h
 *
 *  ACE.h,v 4.128 2004/01/28 06:46:18 jwillemsen Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//=============================================================================

#ifndef ACE_ACE_H
#define ACE_ACE_H

#include <sys/types.h>
#include "macros.h"

class  ACE
{
	
public:
  // = Miscelleous functions.
  /// Rounds the request to a multiple of the page size.
  static size_t round_to_pagesize (off_t len);
private:

  //
  /// Size of a VM page.
  //static size_t pagesize_;

  
};


#endif  /* ACE_ACE_H */
