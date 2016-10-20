// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_sys_select.h
 *
 *  OS_NS_sys_select.h,v 1.5 2003/12/04 19:56:40 jmzorko Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef ACE_OS_NS_SYS_SELECT_H
#define ACE_OS_NS_SYS_SELECT_H

#include "macros.h"

// The following is needed for Mac OSX 10.2 (Jaguar).  Mac OSX 10.3 (Panther)
// doesn't seem to have this issue.

#include <unistd.h>

//#include <sys/select.h>
#include "Time_Value.h"


class ACE_Time_Value;

namespace ACE_OS {

// It would be really cool to add another version of select that would
// function like the one we're defending against below!
   inline int select (int width,
                   fd_set *rfds, fd_set *wfds, fd_set *efds,
                   const ACE_Time_Value *timeout)
   {
     ACE_TRACE ("ACE_OS::select");
   #if defined (ACE_HAS_NONCONST_SELECT_TIMEVAL)
     // We must defend against non-conformity!
     timeval copy;
     timeval *timep;
   
     if (timeout != 0)
       {
         copy = *timeout;
         timep = &copy;
       }
     else
       timep = 0;
   #else
     const timeval *timep = (timeout == 0 ? (const timeval *)0 : *timeout);
   #endif /* ACE_HAS_NONCONST_SELECT_TIMEVAL */
     ACE_SOCKCALL_RETURN (::select (width,
                                    (ACE_FD_SET_TYPE *) rfds,
                                    (ACE_FD_SET_TYPE *) wfds,
                                    (ACE_FD_SET_TYPE *) efds,
                                    timep),
                          int, -1);
   }
   
   inline int select (int width,
                   fd_set *rfds, fd_set *wfds, fd_set *efds,
                   const ACE_Time_Value &timeout)
   {
     ACE_TRACE ("ACE_OS::select");
   #if defined (ACE_HAS_NONCONST_SELECT_TIMEVAL)
   # define ___ACE_TIMEOUT &copy
     timeval copy = timeout;
   #else
   # define ___ACE_TIMEOUT timep
     const timeval *timep = timeout;
   #endif /* ACE_HAS_NONCONST_SELECT_TIMEVAL */
     ACE_SOCKCALL_RETURN (::select (width,
                                 (ACE_FD_SET_TYPE *) rfds,
                                 (ACE_FD_SET_TYPE *) wfds,
                                 (ACE_FD_SET_TYPE *) efds,
                                 ___ACE_TIMEOUT),
                       int, -1);
   #undef ___ACE_TIMEOUT
   }

} /* namespace ACE_OS */
#endif /* ACE_OS_NS_SYS_SELECT_H */
