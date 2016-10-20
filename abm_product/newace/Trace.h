// -*- C++ -*-

//=============================================================================
/**
 *  @file    Trace.h
 *
 *  Trace.h,v 4.19 2003/07/30 21:15:59 dhinton Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//=============================================================================

#ifndef ACE_TRACE_H
#define ACE_TRACE_H

#include "macros.h"
/**
 * @class ACE_Trace
 *
 * @brief A C++ trace facility that keeps track of which methods are
 * entered and exited.
 *
 * This class uses C++ constructors and destructors to automate
 * the ACE_Trace nesting.  In addition, thread-specific storage
 * is used to enable multiple threads to work correctly.
 */
class ACE_Trace
{
public:
  // = Initialization and termination methods.

  /// Perform the first part of the trace, which prints out the string
  /// N, the LINE, and the ACE_FILE as the function is entered.
  ACE_Trace (const char *n,
             int line = 0,
             const char *file = "");

  /// Perform the second part of the trace, which prints out the NAME
  /// as the function is exited.
  ~ACE_Trace (void);

  // = Control the tracing level.
  /// Determine if tracing is enabled (return == 1) or not (== 0)
  static int  is_tracing(void);

  /// Enable the tracing facility.
  static void start_tracing (void);

  /// Disable the tracing facility.
  static void stop_tracing (void);

  /// Change the nesting indentation level.
  static void set_nesting_indent (int indent);

  /// Get the nesting indentation level.
  static int get_nesting_indent (void);

  /// Dump the state of an object.
  void dump (void) const;

private:
  // Keeps track of how deeply the call stack is nested (this is
  // maintained in thread-specific storage to ensure correctness in
  // multiple threads of control.

  /// Name of the method we are in.
  const char *name_;

  /// Keeps track of how far to indent per trace call.
  static int nesting_indent_;

  /// Is tracing enabled?
  static int enable_tracing_;

  /// Default values.
  enum
  {
    DEFAULT_INDENT  = 3,
    DEFAULT_TRACING = 1
  };
};



#endif /* ACE_TRACE_H */
