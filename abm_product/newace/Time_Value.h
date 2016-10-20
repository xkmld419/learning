// -*- C++ -*-

//=============================================================================
/**
 *  @file    Time_Value.h
 *
 *  Time_Value.h,v 4.28 2004/03/31 08:06:06 jwillemsen Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//=============================================================================

#ifndef ACE_TIME_VALUE_H
#define ACE_TIME_VALUE_H


#include "config.h"
#include "macros.h"

//#define __need_timespec
#include <sys/time.h>

// Define some helpful constants.
// Not type-safe, and signed.  For backward compatibility.
#define ACE_ONE_SECOND_IN_MSECS 1000L
#define ACE_ONE_SECOND_IN_USECS 1000000L
#define ACE_ONE_SECOND_IN_NSECS 1000000000L

// -------------------------------------------------------------------
// These forward declarations are only used to circumvent a bug in
// MSVC 6.0 compiler.  They shouldn't cause any problem for other
// compilers and they can be removed once MS release a SP that contains
// the fix.

// This forward declaration is needed by the set() and FILETIME() functions
#if defined (ACE_LACKS_LONGLONG_T)
class  ACE_U_LongLong;
#endif  /* ACE_LACKS_LONGLONG_T */
// -------------------------------------------------------------------

// -------------------------------------------------------------------

/**
 * @class ACE_Time_Value
 *
 * @brief Operations on "timeval" structures, which express time in
 * seconds (secs) and microseconds (usecs).
 *
 * This class centralizes all the time related processing in
 * ACE.  These time values are typically used in conjunction with OS
 * mechanisms like <select>, <poll>, or <cond_timedwait>.
 */
class  ACE_Time_Value
{
public:

  /// Constant "0".
  static const ACE_Time_Value zero;

  /**
   * Constant for maximum time representable.  Note that this time is
   * not intended for use with <select> or other calls that may have
   * *their own* implementation-specific maximum time representations.
   * Its primary use is in time computations such as those used by the
   * dynamic subpriority strategies in the ACE_Dynamic_Message_Queue
   * class.
   */
  static const ACE_Time_Value max_time;

  ACE_ALLOC_HOOK_DECLARE;
  // = Initialization methods.


  // = Methods for converting to/from various time formats.



  
  inline ACE_Time_Value (const struct timeval &tv)
    // : tv_ ()
  {
    // ACE_OS_TRACE ("ACE_Time_Value");
    this->set (tv);
  }
  
  inline operator timeval () const
  {
    // ACE_OS_TRACE ("operator timeval");
    return this->tv_;
  }

  inline
  operator const timeval * () const
  {
    // ACE_OS_TRACE ("operator const timeval *");
    return (const timeval *) &this->tv_;
  }
  
  inline void
  set (long sec, long usec)
  {
    // ACE_OS_TRACE ("set");
    this->tv_.tv_sec = sec;
    this->tv_.tv_usec = usec;
    this->normalize ();
  }
  
  inline void
  set (double d)
  {
    // ACE_OS_TRACE ("set");
    long l = (long) d;
    this->tv_.tv_sec = l;
    this->tv_.tv_usec = (long) ((d - (double) l) * ACE_ONE_SECOND_IN_USECS + .5);
    this->normalize ();
  }
  
  // Initializes a timespec.  Note that this approach loses precision
  // since it converts the nano-seconds into micro-seconds.  But then
  // again, do any real systems have nano-second timer precision?!
  
  inline void
  set (const timespec &tv)
  {
    // ACE_OS_TRACE ("set");
  #if ! defined(ACE_HAS_BROKEN_TIMESPEC_MEMBERS)
    this->tv_.tv_sec = ACE_static_cast (long, tv.tv_sec);
    // Convert nanoseconds into microseconds.
    this->tv_.tv_usec = tv.tv_nsec / 1000;
  #else
    this->tv_.tv_sec = tv.ts_sec;
    // Convert nanoseconds into microseconds.
    this->tv_.tv_usec = tv.ts_nsec / 1000;
  #endif /* ACE_HAS_BROKEN_TIMESPEC_MEMBERS */
  
    this->normalize ();
  }
  
  inline void
  set (const timeval &tv)
  {
    // ACE_OS_TRACE ("set");
    this->tv_.tv_sec = tv.tv_sec;
    this->tv_.tv_usec = tv.tv_usec;
  
    this->normalize ();
  }
  
  inline
  ACE_Time_Value (void)
    // : tv_ ()
  {
    // ACE_OS_TRACE ("ACE_Time_Value");
    this->set (0, 0);
  }
  
  inline
  ACE_Time_Value (long sec, long usec=0)
  {
    // ACE_OS_TRACE ("ACE_Time_Value");
    this->set (sec, usec);
  }
  
  // Returns number of seconds.
  
  inline long
  sec (void) const
  {
    // ACE_OS_TRACE ("sec");
    return this->tv_.tv_sec;
  }
  
  // Sets the number of seconds.
  
  inline void
  sec (long sec)
  {
    // ACE_OS_TRACE ("sec");
    this->tv_.tv_sec = sec;
  }
  
  // Converts from Time_Value format into milli-seconds format.
  
  inline unsigned long
  msec (void) const
  {
    // ACE_OS_TRACE ("msec");
    return this->tv_.tv_sec * 1000 + this->tv_.tv_usec / 1000;
  }
  
  // Converts from milli-seconds format into Time_Value format.
  
  inline void
  msec (long milliseconds)
  {
    // ACE_OS_TRACE ("msec");
    // Convert millisecond units to seconds;
    this->tv_.tv_sec = milliseconds / 1000;
    // Convert remainder to microseconds;
    this->tv_.tv_usec = (milliseconds - (this->tv_.tv_sec * 1000)) * 1000;
  }
  
  // Returns number of micro-seconds.
  
  inline long
  usec (void) const
  {
    // ACE_OS_TRACE ("usec");
    return this->tv_.tv_usec;
  }
  
  // Sets the number of micro-seconds.
  
  inline void
  usec (long usec)
  {
    // ACE_OS_TRACE ("usec");
    this->tv_.tv_usec = usec;
  }
  
  inline ACE_Time_Value &
  operator *= (double d)
  {
    double time =
      ((double) this->sec ()) * ACE_ONE_SECOND_IN_USECS + this->usec ();
    time *= d;
    this->sec ((long)(time / ACE_ONE_SECOND_IN_USECS));
    this->usec (((long)time) % ACE_ONE_SECOND_IN_USECS);
    this->normalize ();
    return *this;
  }
  
  inline friend ACE_Time_Value
  operator * (double d, const ACE_Time_Value &tv)
  {
    return ACE_Time_Value (tv) *= d;
  }
  
  inline friend ACE_Time_Value
  operator * (const ACE_Time_Value &tv, double d)
  {
    return ACE_Time_Value (tv) *= d;
  }
  
  // True if tv1 > tv2.
  
  inline friend  int
  operator > (const ACE_Time_Value &tv1,
              const ACE_Time_Value &tv2)
  {
    // ACE_OS_TRACE ("operator >");
    if (tv1.sec () > tv2.sec ())
      return 1;
    else if (tv1.sec () == tv2.sec ()
             && tv1.usec () > tv2.usec ())
      return 1;
    else
      return 0;
  }
  
  // True if tv1 >= tv2.
  
  inline friend int
  operator >= (const ACE_Time_Value &tv1,
               const ACE_Time_Value &tv2)
  {
    // ACE_OS_TRACE ("operator >=");
    if (tv1.sec () > tv2.sec ())
      return 1;
    else if (tv1.sec () == tv2.sec ()
             && tv1.usec () >= tv2.usec ())
      return 1;
    else
      return 0;
  }
  
  // Returns the value of the object as a timespec.
  
  inline
  operator timespec () const
  {
    // ACE_OS_TRACE ("operator timespec");
    timespec tv;
  #if ! defined(ACE_HAS_BROKEN_TIMESPEC_MEMBERS)
    tv.tv_sec = this->sec ();
    // Convert microseconds into nanoseconds.
    tv.tv_nsec = this->tv_.tv_usec * 1000;
  #else
    tv.ts_sec = this->sec ();
    // Convert microseconds into nanoseconds.
    tv.ts_nsec = this->tv_.tv_usec * 1000;
  #endif /* ACE_HAS_BROKEN_TIMESPEC_MEMBERS */
    return tv;
  }
  
  // Initializes the ACE_Time_Value object from a timespec.
  
  inline
  ACE_Time_Value (const timespec &tv)
    // : tv_ ()
  {
    // ACE_OS_TRACE ("ACE_Time_Value");
    this->set (tv);
  }
  
  // True if tv1 < tv2.
  
  inline friend  int
  operator < (const ACE_Time_Value &tv1,
              const ACE_Time_Value &tv2)
  {
    // ACE_OS_TRACE ("operator <");
    return tv2 > tv1;
  }
  
  // True if tv1 >= tv2.
  
  inline friend int
  operator <= (const ACE_Time_Value &tv1,
               const ACE_Time_Value &tv2)
  {
    // ACE_OS_TRACE ("operator <=");
    return tv2 >= tv1;
  }
  
  // True if tv1 == tv2.
  
  inline friend int
  operator == (const ACE_Time_Value &tv1,
               const ACE_Time_Value &tv2)
  {
    // ACE_OS_TRACE ("operator ==");
    return tv1.sec () == tv2.sec ()
      && tv1.usec () == tv2.usec ();
  }
  
  // True if tv1 != tv2.
  
  inline friend  int
  operator != (const ACE_Time_Value &tv1,
               const ACE_Time_Value &tv2)
  {
    // ACE_OS_TRACE ("operator !=");
    return !(tv1 == tv2);
  }
  
  // Add TV to this.
  
  inline ACE_Time_Value &
  operator+= (const ACE_Time_Value &tv)
  {
    // ACE_OS_TRACE ("operator+=");
    this->sec (this->sec () + tv.sec ());
    this->usec (this->usec () + tv.usec ());
    this->normalize ();
    return *this;
  }
  
  // Subtract TV to this.
  
  inline ACE_Time_Value &
  operator-= (const ACE_Time_Value &tv)
  {
    // ACE_OS_TRACE ("operator-=");
    this->sec (this->sec () - tv.sec ());
    this->usec (this->usec () - tv.usec ());
    this->normalize ();
    return *this;
  }
  
  // Adds two ACE_Time_Value objects together, returns the sum.
  
  inline friend ACE_Time_Value
  operator + (const ACE_Time_Value &tv1,
              const ACE_Time_Value &tv2)
  {
    // ACE_OS_TRACE ("operator +");
    ACE_Time_Value sum (tv1.sec () + tv2.sec (),
                        tv1.usec () + tv2.usec ());
  
    sum.normalize ();
    return sum;
  }

// Subtracts two ACE_Time_Value objects, returns the difference.

  inline friend ACE_Time_Value
  operator - (const ACE_Time_Value &tv1,
              const ACE_Time_Value &tv2)
  {
    // ACE_OS_TRACE ("operator -");
    ACE_Time_Value delta (tv1.sec () - tv2.sec (),
                          tv1.usec () - tv2.usec ());
    delta.normalize ();
    return delta;
  }
# if defined (ACE_WIN32)
  /// Construct the ACE_Time_Value object from a Win32 FILETIME
  ACE_Time_Value (const FILETIME &ft);
# endif /* ACE_WIN32 */


# if defined (ACE_WIN32)
  ///  Initializes the ACE_Time_Value object from a Win32 FILETIME.
  void set (const FILETIME &ft);
# endif /* ACE_WIN32 */

  /// Converts from ACE_Time_Value format into milli-seconds format.
  /// Increment microseconds as postfix.
  /**
   * @note The only reason this is here is to allow the use of ACE_Atomic_Op
   * with ACE_Time_Value.
   */
  ACE_Time_Value operator++ (int);

  /// Increment microseconds as prefix.
  /**
   * @note The only reason this is here is to allow the use of ACE_Atomic_Op
   * with ACE_Time_Value.
   */
  ACE_Time_Value &operator++ (void);

  /// Decrement microseconds as postfix.
  /**
   * @note The only reason this is here is to allow the use of ACE_Atomic_Op
   * with ACE_Time_Value.
   */
  ACE_Time_Value operator-- (int);

  /// Decrement microseconds as prefix.
  /**
   * @note The only reason this is here is to allow the use of ACE_Atomic_Op
   * with ACE_Time_Value.
   */
  ACE_Time_Value &operator-- (void);

  //@}

  /// Dump is a no-op.
  /**
   * The dump() method is a no-op.  It's here for backwards compatibility
   * only, but does not dump anything. Invoking logging methods here
   * violates layering restrictions in ACE because this class is part
   * of the OS layer and @c ACE_Log_Msg is at a higher level.
   */
  void dump (void) const;

# if defined (ACE_WIN32)
  /// Const time difference between FILETIME and POSIX time.
#  if defined (ACE_LACKS_LONGLONG_T)
  static const ACE_U_LongLong FILETIME_to_timval_skew;
#  else
  static const DWORDLONG FILETIME_to_timval_skew;
#  endif // ACE_LACKS_LONGLONG_T
# endif /* ACE_WIN32 */

private:
  /// Put the timevalue into a canonical form.
  void normalize (void);

  /// Store the values as a timeval.
  timeval tv_;
};

/**
 * @class ACE_Countdown_Time
 *
 * @brief Keeps track of the amount of elapsed time.
 *
 * This class has a side-effect on the <max_wait_time> -- every
 * time the <stop> method is called the <max_wait_time> is
 * updated.
 */
class  ACE_Countdown_Time
{
public:
  // = Initialization and termination methods.
  /// Cache the <max_wait_time> and call <start>.
  ACE_Countdown_Time (ACE_Time_Value *max_wait_time);

  /// Call <stop>.
  ~ACE_Countdown_Time (void);

  /// Cache the current time and enter a start state.
  int start (void);

  /// Subtract the elapsed time from max_wait_time_ and enter a stopped
  /// state.
  int stop (void);

  /// Calls stop and then start.  max_wait_time_ is modified by the
  /// call to stop.
  int update (void);

  /// Returns 1 if we've already been stopped, else 0.
  int stopped (void) const;

private:
  /// Maximum time we were willing to wait.
  ACE_Time_Value *max_wait_time_;

  /// Beginning of the start time.
  ACE_Time_Value start_time_;

  /// Keeps track of whether we've already been stopped.
  int stopped_;
};



#endif /* ACE_TIME_VALUE_H */
