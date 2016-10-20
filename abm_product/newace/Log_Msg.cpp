// Log_Msg.cpp,v 4.291 2004/01/05 21:18:46 schmidt Exp

// We need this to get the status of ACE_NTRACE...

// Turn off tracing for the duration of this file.
#if defined (ACE_NTRACE)
# undef ACE_NTRACE
#endif /* ACE_NTRACE */
#define ACE_NTRACE 1

#include "macros.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <stdarg.h>
#include <iostream>



#include "Log_Msg.h"
#include "Log_Msg_Callback.h"
#include "Log_Msg_IPC.h"
#include "Log_Msg_NT_Event_Log.h"
#include "Log_Msg_UNIX_Syslog.h"
#include "Log_Record.h"

ACE_ALLOC_HOOK_DEFINE(ACE_Log_Msg)

// only used here...  dhinton
#if defined (ACE_HAS_SYS_SIGLIST)
# if !defined (_sys_siglist)
#   define _sys_siglist sys_siglist
# endif /* !defined (sys_siglist) */
//extern char **_sys_siglist;
#endif /* ACE_HAS_SYS_SIGLIST */


#if defined (ACE_WIN32) && !defined (ACE_HAS_WINCE) && !defined (ACE_HAS_PHARLAP)
#  define ACE_LOG_MSG_SYSLOG_BACKEND ACE_Log_Msg_NT_Event_Log
#elif !defined (ACE_LACKS_UNIX_SYSLOG) && !defined (ACE_HAS_WINCE)
#  define ACE_LOG_MSG_SYSLOG_BACKEND ACE_Log_Msg_UNIX_Syslog
#else
#  define ACE_LOG_MSG_SYSLOG_BACKEND ACE_Log_Msg_IPC
#endif /* ! ACE_WIN32 */

// When doing ACE_OS::s[n]printf() calls in log(), we need to update
// the space remaining in the output buffer based on what's returned from
// the output function. If we could rely on more modern compilers, this
// would be in an unnamed namespace, but it's a macro instead.
// count is a size_t, len is an int and assumed to be non-negative.
#define ACE_UPDATE_COUNT(COUNT, LEN) \
   do { if (ACE_static_cast (size_t, LEN) > COUNT) COUNT = 0; \
     else COUNT -= ACE_static_cast (size_t, LEN); \
   } while (0)

/// Instance count for Log_Msg - used to know when dynamically
/// allocated storage (program name and host name) can be safely
/// deleted.
int ACE_Log_Msg::instance_count_ = 0;

/**
 * @class ACE_Log_Msg_Manager
 *
 * @brief Synchronize output operations.
 *
 * Provides global point of contact for all ACE_Log_Msg instances
 * in a process.
 *
 * For internal use by ACE, only!
 */
class ACE_Log_Msg_Manager
{
public:
  static ACE_Log_Msg_Backend *log_backend_;
  static ACE_Log_Msg_Backend *custom_backend_;

  static u_long log_backend_flags_;

  static int init_backend (const u_long *flags = 0);

};

ACE_Log_Msg_Backend *ACE_Log_Msg_Manager::log_backend_ = 0;
ACE_Log_Msg_Backend *ACE_Log_Msg_Manager::custom_backend_ = 0;

u_long ACE_Log_Msg_Manager::log_backend_flags_ = 0;

int ACE_Log_Msg_Manager::init_backend (const u_long *flags)
{
  // If flags have been supplied, and they are different from the flags
  // we had last time, then we may have to re-create the backend as a
  // different type.
  if (flags)
    {
      // Sanity check for custom backend.
      if (ACE_BIT_ENABLED (*flags, ACE_Log_Msg::CUSTOM) &&
          ACE_Log_Msg_Manager::custom_backend_ == 0)
        {
          return -1;
        }

      if ((ACE_BIT_ENABLED (*flags, ACE_Log_Msg::SYSLOG)
            && ACE_BIT_DISABLED (ACE_Log_Msg_Manager::log_backend_flags_, ACE_Log_Msg::SYSLOG))
          || (ACE_BIT_DISABLED (*flags, ACE_Log_Msg::SYSLOG)
            && ACE_BIT_ENABLED (ACE_Log_Msg_Manager::log_backend_flags_, ACE_Log_Msg::SYSLOG)))
        {
          delete ACE_Log_Msg_Manager::log_backend_;
          ACE_Log_Msg_Manager::log_backend_ = 0;
        }

      ACE_Log_Msg_Manager::log_backend_flags_ = *flags;
    }

  if (ACE_Log_Msg_Manager::log_backend_ == 0)
    {
      ACE_NO_HEAP_CHECK;

#if (defined (WIN32) || !defined (ACE_LACKS_UNIX_SYSLOG)) && !defined (ACE_HAS_WINCE) && !defined (ACE_HAS_PHARLAP)
      // Allocate the ACE_Log_Msg_Backend instance.
      if (ACE_BIT_ENABLED (ACE_Log_Msg_Manager::log_backend_flags_, ACE_Log_Msg::SYSLOG))
        ACE_NEW_RETURN (ACE_Log_Msg_Manager::log_backend_,
                        ACE_LOG_MSG_SYSLOG_BACKEND,
                        -1);
      else
#endif /* defined (WIN32) && !defined (ACE_HAS_WINCE) && !defined (ACE_HAS_PHARLAP) */
        ACE_NEW_RETURN (ACE_Log_Msg_Manager::log_backend_,
                        ACE_Log_Msg_IPC,
                        -1);
    }

  return 0;
}


/* static */
int
ACE_Log_Msg::exists (void)
{
  return 1;
}

ACE_Log_Msg *
ACE_Log_Msg::instance (void)
{
  // We don't have threads, we cannot call
  // ACE_Log_Msg_Manager::get_lock () to initialize the logger
  // callback, so instead we do it here.
  if (ACE_Log_Msg_Manager::init_backend () == -1)
    return 0;

  // Singleton implementation.
  static ACE_Log_Msg *log_msg = 0;
  if (log_msg == 0)
  {
      ACE_NEW_RETURN (log_msg, ACE_Log_Msg, 0);
      // Register the instance for destruction at program termination.
      return log_msg;
  }

  return log_msg;
}

// Not inlined to help prevent having to include OS.h just to
// get ACE_DEBUG, et al, macros.
int
ACE_Log_Msg::last_error_adapter (void)
{
  return errno;
}

// Sets the flag in the default priority mask used to initialize
// ACE_Log_Msg instances, as well as the current per-thread instance.

void
ACE_Log_Msg::enable_debug_messages (ACE_Log_Priority priority)
{
  ACE_SET_BITS (ACE_Log_Msg::default_priority_mask_, priority);
  ACE_Log_Msg *i = ACE_Log_Msg::instance ();
  i->priority_mask (i->priority_mask () | priority);
}

// Clears the flag in the default priority mask used to initialize
// ACE_Log_Msg instances, as well as the current per-thread instance.

void
ACE_Log_Msg::disable_debug_messages (ACE_Log_Priority priority)
{
  ACE_CLR_BITS (ACE_Log_Msg::default_priority_mask_, priority);
  ACE_Log_Msg *i = ACE_Log_Msg::instance ();
  i->priority_mask (i->priority_mask () & ~priority);
}

const ACE_TCHAR *
ACE_Log_Msg::program_name (void)
{
  return ACE_Log_Msg::program_name_;
}

/// Name of the local host.
const ACE_TCHAR *ACE_Log_Msg::local_host_ = 0;

/// Records the program name.
const ACE_TCHAR *ACE_Log_Msg::program_name_ = 0;

/// Default is to use stderr.
u_long ACE_Log_Msg::flags_ = ACE_Log_Msg::STDERR;

/// Process id of the current process.
pid_t ACE_Log_Msg::pid_ = -1;

/// Current offset of msg_[].
int ACE_Log_Msg::msg_off_ = 0;

/// Default per-thread priority mask
/// By default, no priorities are enabled.
u_long ACE_Log_Msg::default_priority_mask_ = 0;

/// Default per-process priority mask
/// By default, all priorities are enabled.
u_long ACE_Log_Msg::process_priority_mask_ = LM_SHUTDOWN
                                           | LM_TRACE
                                           | LM_DEBUG
                                           | LM_INFO
                                           | LM_NOTICE
                                           | LM_WARNING
                                           | LM_STARTUP
                                           | LM_ERROR
                                           | LM_CRITICAL
                                           | LM_ALERT
                                           | LM_EMERGENCY;

void
ACE_Log_Msg::close (void)
{
  // This call needs to go here to avoid memory leaks.
  

  // Please note that this will be called by a statement that is
  // harded coded into the ACE_Object_Manager's shutdown sequence, in
  // its destructor.

}

void
ACE_Log_Msg::sync_hook (const ACE_TCHAR *prg_name)
{
  ACE_LOG_MSG->sync (prg_name);
}


// Call after a fork to resynchronize the PID and PROGRAM_NAME
// variables.
void
ACE_Log_Msg::sync (const ACE_TCHAR *prog_name)
{
  ACE_TRACE ("ACE_Log_Msg::sync");

  if (prog_name)
    {
      // Must free if already allocated!!!
      free ((void *) ACE_Log_Msg::program_name_);

      // Stop heap checking, block will be freed by the destructor when
      // the last ACE_Log_Msg instance is deleted.
      // Heap checking state will be restored when the block is left.
      {
        ACE_NO_HEAP_CHECK;

        ACE_Log_Msg::program_name_ = strdup (prog_name);
      }
    }

  ACE_Log_Msg::pid_ = ::getpid ();
  ACE_Log_Msg::msg_off_ = 0;
}

u_long
ACE_Log_Msg::flags (void)
{
  ACE_TRACE ("ACE_Log_Msg::flags");
  u_long result;
  
  result = ACE_Log_Msg::flags_;
  return result;
}

void
ACE_Log_Msg::set_flags (u_long flgs)
{
  ACE_TRACE ("ACE_Log_Msg::set_flags");

  ACE_SET_BITS (ACE_Log_Msg::flags_, flgs);
}

void
ACE_Log_Msg::clr_flags (u_long flgs)
{
  ACE_TRACE ("ACE_Log_Msg::clr_flags");

  ACE_CLR_BITS (ACE_Log_Msg::flags_, flgs);
}

int
ACE_Log_Msg::acquire (void)
{
  ACE_TRACE ("ACE_Log_Msg::acquire");
  return 0;
}

u_long
ACE_Log_Msg::priority_mask (u_long n_mask, MASK_TYPE mask_type)
{
  u_long o_mask;

  if (mask_type == THREAD) {
    o_mask = this->priority_mask_;
    this->priority_mask_ = n_mask;
  }
  else {
    o_mask = ACE_Log_Msg::process_priority_mask_;
        ACE_Log_Msg::process_priority_mask_ = n_mask;
  }

  return o_mask;
}

u_long
ACE_Log_Msg::priority_mask (MASK_TYPE mask_type)
{
  return mask_type == THREAD  ?  this->priority_mask_
                              :  ACE_Log_Msg::process_priority_mask_;
}

int
ACE_Log_Msg::log_priority_enabled (ACE_Log_Priority log_priority)
{
  return ACE_BIT_ENABLED (this->priority_mask_ |
                            ACE_Log_Msg::process_priority_mask_,
                          log_priority);
}

int
ACE_Log_Msg::release (void)
{
  ACE_TRACE ("ACE_Log_Msg::release");

  return 0;
}

ACE_Log_Msg::ACE_Log_Msg (void)
  : status_ (0),
    errnum_ (0),
    linenum_ (0),
    restart_ (1),  // Restart by default...
    ostream_ (0),
    msg_callback_ (0),
    trace_depth_ (0),
    trace_active_ (0),
    tracing_enabled_ (1), // On by default?
    delete_ostream_(0),
    priority_mask_ (default_priority_mask_),
    timestamp_ (0)
{
  // ACE_TRACE ("ACE_Log_Msg::ACE_Log_Msg");

  ++instance_count_;

  this->conditional_values_.is_set_ = 0;

  char *timestamp = getenv ("ACE_LOG_TIMESTAMP");
  if (timestamp != 0)
    {
      // If variable is set or is set to date tag so we print date and time.
      if (strcmp (timestamp, "TIME") == 0)
        {
          this->timestamp_ = 1;
        }
      else if (strcmp (timestamp, "DATE") == 0)
        {
          this->timestamp_ = 2;
        }
    }
}

ACE_Log_Msg::~ACE_Log_Msg (void)
{
  int instance_count = --instance_count_;

  // If this is the last instance then cleanup.  Only the last
  // thread to destroy its ACE_Log_Msg instance should execute
  // this block.
  if (instance_count == 0)
    {
      // Destroy the message queue instance.
      if (ACE_Log_Msg_Manager::log_backend_ != 0)
        ACE_Log_Msg_Manager::log_backend_->close ();

      // Close down custom backend
      if (ACE_Log_Msg_Manager::custom_backend_ != 0)
        ACE_Log_Msg_Manager::custom_backend_->close ();


      if (ACE_Log_Msg::program_name_)
        {
          free ((void *) ACE_Log_Msg::program_name_);
          ACE_Log_Msg::program_name_ = 0;
        }

      if (ACE_Log_Msg::local_host_)
        {
          free ((void *) ACE_Log_Msg::local_host_);
          ACE_Log_Msg::local_host_ = 0;
        }
    }

  //
  // do we need to close and clean up?
  //
  if (this->delete_ostream_ == 1)
#if defined (ACE_LACKS_IOSTREAM_TOTALLY)
    {
      fclose (this->ostream_);
    }
#else
    {
      delete ostream_;
    }
#endif
}

// Open the sender-side of the message queue.

int
ACE_Log_Msg::open (const ACE_TCHAR *prog_name,
                   u_long flags,
                   const ACE_TCHAR *logger_key)
{
  ACE_TRACE ("ACE_Log_Msg::open");

  if (prog_name)
    {
      free ((void *) ACE_Log_Msg::program_name_);

      // Stop heap checking, block will be freed by the destructor.
      {
        ACE_NO_HEAP_CHECK;

        ACE_ALLOCATOR_RETURN (ACE_Log_Msg::program_name_,
                              strdup (prog_name),
                              -1);
      }
    }
  else if (ACE_Log_Msg::program_name_ == 0)
    {
      // Stop heap checking, block will be freed by the destructor.
      ACE_NO_HEAP_CHECK;
      ACE_ALLOCATOR_RETURN (ACE_Log_Msg::program_name_,
                            strdup (ACE_LIB_TEXT ("<unknown>")),
                            -1);
    }

  int status = 0;

  // Be sure that there is a message_queue_, with multiple threads.

  // Always close the current handle before doing anything else.
  if (ACE_Log_Msg_Manager::log_backend_ != 0)
    ACE_Log_Msg_Manager::log_backend_->reset ();

  if (ACE_Log_Msg_Manager::custom_backend_ != 0)
    ACE_Log_Msg_Manager::custom_backend_->reset ();

  // Note that if we fail to open the message queue the default action
  // is to use stderr (set via static initialization in the
  // Log_Msg.cpp file).

  if (ACE_BIT_ENABLED (flags, ACE_Log_Msg::LOGGER)
      || ACE_BIT_ENABLED (flags, ACE_Log_Msg::SYSLOG))
    {
      // The SYSLOG backends (both NT and UNIX) can get along fine
      // without the logger_key.
      if (logger_key == 0 && ACE_BIT_ENABLED (flags, ACE_Log_Msg::LOGGER))
        status = -1;
      else
        {
          status =
            ACE_Log_Msg_Manager::log_backend_->open (logger_key);
        }

      if (status == -1)
        ACE_SET_BITS (ACE_Log_Msg::flags_, ACE_Log_Msg::STDERR);
      else
        {
          if (ACE_BIT_ENABLED (flags, ACE_Log_Msg::LOGGER))
            ACE_SET_BITS (ACE_Log_Msg::flags_, ACE_Log_Msg::LOGGER);
          if (ACE_BIT_ENABLED (flags, ACE_Log_Msg::SYSLOG))
            ACE_SET_BITS (ACE_Log_Msg::flags_, ACE_Log_Msg::SYSLOG);
        }
    }
  else if (ACE_BIT_ENABLED (ACE_Log_Msg::flags_, ACE_Log_Msg::LOGGER) ||
           ACE_BIT_ENABLED (ACE_Log_Msg::flags_, ACE_Log_Msg::SYSLOG))
    {
      // If we are closing down logger, redirect logging to stderr.
      ACE_CLR_BITS (ACE_Log_Msg::flags_, ACE_Log_Msg::LOGGER);
      ACE_CLR_BITS (ACE_Log_Msg::flags_, ACE_Log_Msg::SYSLOG);
      ACE_SET_BITS (ACE_Log_Msg::flags_, ACE_Log_Msg::STDERR);
    }

  if (ACE_BIT_ENABLED (flags, ACE_Log_Msg::CUSTOM))
    {
      status =
        ACE_Log_Msg_Manager::custom_backend_->open (logger_key);

      if (status != -1)
        ACE_SET_BITS (ACE_Log_Msg::flags_, ACE_Log_Msg::CUSTOM);
    }

  // Remember, ACE_Log_Msg::STDERR bit is on by default...
  if (status != -1
      && ACE_BIT_ENABLED (flags,
                          ACE_Log_Msg::STDERR) == 0)
    ACE_CLR_BITS (ACE_Log_Msg::flags_,
                  ACE_Log_Msg::STDERR);

  // VERBOSE takes precedence over VERBOSE_LITE...
  if (ACE_BIT_ENABLED (flags,
                       ACE_Log_Msg::VERBOSE_LITE))
    ACE_SET_BITS (ACE_Log_Msg::flags_,
                  ACE_Log_Msg::VERBOSE_LITE);
  else if (ACE_BIT_ENABLED (flags,
                            ACE_Log_Msg::VERBOSE))
    ACE_SET_BITS (ACE_Log_Msg::flags_,
                  ACE_Log_Msg::VERBOSE);

  if (ACE_BIT_ENABLED (flags,
                       ACE_Log_Msg::OSTREAM))
    {
      ACE_SET_BITS (ACE_Log_Msg::flags_,
                    ACE_Log_Msg::OSTREAM);
      // Only set this to cerr if it hasn't already been set.
      if (this->msg_ostream () == 0)
        this->msg_ostream (ACE_DEFAULT_LOG_STREAM);
    }

  if (ACE_BIT_ENABLED (flags,
                       ACE_Log_Msg::MSG_CALLBACK))
    ACE_SET_BITS (ACE_Log_Msg::flags_,
                  ACE_Log_Msg::MSG_CALLBACK);

  if (ACE_BIT_ENABLED (flags,
                       ACE_Log_Msg::SILENT))
    ACE_SET_BITS (ACE_Log_Msg::flags_,
                  ACE_Log_Msg::SILENT);

  return status;
}

/**
 * Valid Options (prefixed by '%', as in printf format strings) include:
 *   'A': print an ACE_timer_t value
 *   'a': exit the program at this point (var-argument is the exit status!)
 *   'c': print a character
 *   'C': print a character string
 *   'i', 'd': print a decimal number
 *   'I', indent according to nesting depth
 *   'e', 'E', 'f', 'F', 'g', 'G': print a double
 *   'l', print line number where an error occurred.
 *   'M': print the name of the priority of the message.
 *   'm': Return the message corresponding to errno value, e.g., as done by <strerror>
 *   'N': print file name where the error occurred.
 *   'n': print the name of the program (or "<unknown>" if not set)
 *   'o': print as an octal number
 *   'P': format the current process id
 *   'p': format the appropriate errno message from sys_errlist, e.g., as done by <perror>
 *   'Q': print out the uint64 number
 *   '@': print a void* pointer (in hexadecimal)
 *   'r': call the function pointed to by the corresponding argument
 *   'R': print return status
 *   'S': format the appropriate _sys_siglist entry corresponding to var-argument.
 *   's': format a character string
 *   'T': print timestamp in hour:minute:sec:usec format.
 *   'D': print timestamp in month/day/year hour:minute:sec:usec format.
 *   't': print thread id (1 if single-threaded)
 *   'u': print as unsigned int
 *   'x': print as a hex number
 *   'X': print as a hex number
 *   'w': print a wide character
 *   'W': print out a wide character string.
 *   'z': print an ACE_OS::WChar character
 *   'Z': print an ACE_OS::WChar character string
 *   '%': format a single percent sign, '%'
 */
ssize_t
ACE_Log_Msg::log (ACE_Log_Priority log_priority,
                  const ACE_TCHAR *format_str, ...)
{
  ACE_TRACE ("ACE_Log_Msg::log");

  // Start of variable args section.
  va_list argp;

  va_start (argp, format_str);

//  int result = vprintf(format_str, argp);
  int result = this->log (format_str,
                          log_priority,
                          argp);
  va_end (argp);

  return result;
}

#if defined (ACE_HAS_WCHAR)
/**
 * Since this is the ANTI_TCHAR version, we need to convert
 * the format string over.
 */
ssize_t
ACE_Log_Msg::log (ACE_Log_Priority log_priority,
                  const ACE_ANTI_TCHAR *format_str, ...)
{
  ACE_TRACE ("ACE_Log_Msg::log");

  // Start of variable args section.
  va_list argp;

  va_start (argp, format_str);

  int result = this->log (ACE_TEXT_ANTI_TO_TCHAR (format_str),
                          log_priority,
                          argp);
  va_end (argp);

  return result;
}
#endif /* ACE_HAS_WCHAR */

ssize_t
ACE_Log_Msg::log (const ACE_TCHAR *format_str,
                  ACE_Log_Priority log_priority,
                  va_list argp)
{
  ACE_TRACE ("ACE_Log_Msg::log");
  // External decls.

#if ! (defined(__BORLANDC__) && __BORLANDC__ >= 0x0530) \
    && !defined(__MINGW32__)
#if defined (__FreeBSD__) || defined(__QNX__) || defined(__APPLE__)
//   extern const int sys_nerr;
#else
//   extern int sys_nerr;
#endif /* !__FreeBSD__ && !__QNX__ && !__APPLE__ */
#endif /* ! (defined(__BORLANDC__) && __BORLANDC__ >= 0x0530) */
  typedef void (*PTF)(...);

  // Check if there were any conditional values set.
  int conditional_values = this->conditional_values_.is_set_;

  // Reset conditional values.
  this->conditional_values_.is_set_ = 0;

  // Only print the message if <priority_mask_> hasn't been reset to
  // exclude this logging priority.
  if (this->log_priority_enabled (log_priority) == 0)
    return 0;

  // If conditional values were set and the log priority is correct,
  // then the values are actually set.
  if (conditional_values)
    this->set (this->conditional_values_.file_,
               this->conditional_values_.line_,
               this->conditional_values_.op_status_,
               this->conditional_values_.errnum_,
               this->restart (),
               this->msg_ostream (),
               this->msg_callback ());


  ACE_Log_Record log_record (log_priority,
                             ACE_OS::gettimeofday (),
                             this->getpid ());
  // bp is pointer to where to put next part of logged message.
  // bspace is the number of characters remaining in msg_.
  ACE_TCHAR *bp = ACE_const_cast (ACE_TCHAR *, this->msg ());
  size_t bspace = ACE_Log_Record::MAXLOGMSGLEN;  // Leave room for Nul term.
  if (this->msg_off_ <= ACE_Log_Record::MAXLOGMSGLEN)
    bspace -= ACE_static_cast (size_t, this->msg_off_);

  // If this platform has snprintf() capability to prevent overrunning the
  // output buffer, use it. To avoid adding a maintenance-hassle compile-
  // time couple between here and OS.cpp, don't try to figure this out at
  // compile time. Instead, do a quick check now; if we get a -1 return,
  // the platform doesn't support the length-limiting capability.
  ACE_TCHAR test[2];
#ifndef OS_AIX
  int can_check = snprintf (test, 1, ACE_LIB_TEXT ("x")) != -1;
#else
  int can_check = sprintf (test, "%s", ACE_LIB_TEXT ("x")) != -1;
#endif
  int abort_prog = 0;
  int exit_value = 0;

  if (ACE_BIT_ENABLED (ACE_Log_Msg::flags_, ACE_Log_Msg::VERBOSE))
    {
      // Prepend the program name onto this message

      if (ACE_Log_Msg::program_name_ != 0)
        {
          for (const ACE_TCHAR *s = ACE_Log_Msg::program_name_;
               bspace > 1 && (*bp = *s) != '\0';
               s++, bspace--)
            bp++;

          *bp++ = '|';
          bspace--;
        }
    }

  if (timestamp_ > 0)
  {
     ACE_TCHAR day_and_time[35];
     const ACE_TCHAR *s;
     if (timestamp_ == 1)
     {
        // Print just the time
        s = ACE::timestamp (day_and_time, sizeof day_and_time, 1);
     }
     else
     {
        // Print time and date
        ACE::timestamp (day_and_time, sizeof day_and_time);
        s = day_and_time;
     }

     for (; bspace > 1 && (*bp = *s) != '\0'; s++, bspace--)
        bp++;

     *bp++ = '|';
     bspace--;
  }

  while (*format_str != '\0' && bspace > 0)
    {
      // Copy input to output until we encounter a %, however a
      // % followed by another % is not a format specification.

      if (*format_str != '%')
        {
          *bp++ = *format_str++;
          bspace--;
        }
      else if (format_str[1] == '%') // An "escaped" '%' (just print one '%').
        {
          *bp++ = *format_str++;    // Store first %
          format_str++;             // but skip second %
          bspace--;
        }
      else
        {
          // This is most likely a format specification that ends with
          // one of the valid options described previously. To enable full
          // use of all sprintf capabilities, save the format specifier
          // from the '%' up to the format letter in a new char array.
          // This allows the full sprintf capability for padding, field
          // widths, alignment, etc.  Any width/precision requiring a
          // caller-supplied argument is extracted and placed as text
          // into the format array. Lastly, we convert the caller-supplied
          // format specifier from the ACE_Log_Msg-supported list to the
          // equivalent sprintf specifier, and run the new format spec
          // through sprintf, adding it to the bp string.

          const ACE_TCHAR *abort_str = ACE_LIB_TEXT ("Aborting...");
          const ACE_TCHAR *start_format = format_str;
          ACE_TCHAR format[128]; // Converted format string
          ACE_TCHAR *fp;         // Current format pointer
          int       wp = 0;      // Width/precision extracted from args
          int       done = 0;
          int       skip_nul_locate = 0;
          int       this_len = 0;    // How many chars s[n]printf wrote

          fp = format;
          *fp++ = *format_str++;   // Copy in the %

          // Initialization to satisfy VC6
          int tmp_indent = 0;
          // Work through the format string to copy in the format
          // from the caller. While it's going across, extract ints
          // for '*' width/precision values from the argument list.
          // When the real format specifier is located, change it to
          // one recognized by sprintf, if needed, and do the sprintf
          // call.

          while (!done)
            {
              done = 1;               // Unless a conversion spec changes it

              switch (*format_str)
                {
                // The initial set of cases are the conversion
                // specifiers. Copy them in to the format array.
                // Note we don't use 'l', a normal conversion spec,
                // as a conversion because it is a ACE_Log_Msg format
                // specifier.
                case '-':
                case '+':
                case '0':
                case ' ':
                case '#':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '.':
                case 'L':
                case 'h':
                  *fp++ = *format_str;
                  done = 0;
                  break;

                case '*':
                  wp = va_arg (argp, int);
                  sprintf (fp, ACE_LIB_TEXT ("%d"), wp);
                  fp += strlen (fp);
                  done = 0;
                  break;

                case 'A':             // ACE_timer_t
                  {
#if defined (ACE_LACKS_FLOATING_POINT)
                    strcpy (fp, ACE_LIB_TEXT ("ld"));
                    ACE_UINT32 value = va_arg (argp, ACE_UINT32);
#else
                    strcpy (fp, ACE_LIB_TEXT ("f"));
                    double value = va_arg (argp, double);
#endif /* ACE_LACKS_FLOATING_POINT */
                    if (can_check)
#ifndef OS_AIX
                      this_len = snprintf (bp, bspace, format, value);
#else
                      this_len = sprintf (bp, format, value);
#endif
                    else
                      this_len = sprintf (bp, format, value);
                    ACE_UPDATE_COUNT (bspace, this_len);
                  }
                  break;

                case 'a': // Abort program after handling all of format string.
                  abort_prog = 1;
                  exit_value = va_arg (argp, int);
                  strncpy (bp, abort_str, bspace);
                  if (bspace > strlen (abort_str))
                    bspace -= strlen (abort_str);
                  else
                    bspace = 0;
                  break;

                case 'l':             // Source file line number
                  strcpy (fp, ACE_LIB_TEXT ("d"));
                  if (can_check)
#ifndef OS_AIX
                    this_len = snprintf (bp,
                                                 bspace,
                                                 format,
                                                 this->linenum ());
#else
                    this_len = sprintf (bp, format,
                                                 this->linenum ());
#endif
                  else
                    this_len = sprintf (bp, format, this->linenum ());
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case 'N':             // Source file name
                  // @@ UNICODE
                  strcpy (fp, ACE_LIB_TEXT ("s"));
                  if (can_check)
#ifndef OS_AIX
                    this_len = snprintf (bp, bspace, format,
                                                 this->file () ?
                                                 (this->file ())
                                                 : ACE_LIB_TEXT ("<unknown file>"));
#else
                    this_len = sprintf (bp, format,
                                                 this->file () ?
                                                 (this->file ())
                                                 : ACE_LIB_TEXT ("<unknown file>"));
#endif

                  else
                    this_len = sprintf (bp, format,
                                                this->file () ?
                                                 (this->file ())
                                                : ACE_LIB_TEXT ("<unknown file>"));
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case 'n':             // Program name
                  // @@ UNICODE
                  strcpy (fp, ACE_LIB_TEXT ("s"));
                  if (can_check)
#ifndef OS_AIX
                    this_len = snprintf (bp, bspace, format,
                                                 ACE_Log_Msg::program_name_ ?
                                                 ACE_Log_Msg::program_name_ :
                                                 ACE_LIB_TEXT ("<unknown>"));
#else
                    this_len = sprintf (bp, format,
                                                 ACE_Log_Msg::program_name_ ?
                                                 ACE_Log_Msg::program_name_ :
                                                 ACE_LIB_TEXT ("<unknown>"));
#endif
                  else
                    this_len = sprintf (bp, format,
                                                ACE_Log_Msg::program_name_ ?
                                                ACE_Log_Msg::program_name_ :
                                                ACE_LIB_TEXT ("<unknown>"));
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case 'P':             // Process ID
                  strcpy (fp, ACE_LIB_TEXT ("d"));
                  if (can_check)
#ifndef OS_AIX
                    this_len = snprintf
                      (bp, bspace, format,
                       ACE_static_cast (int, this->getpid ()));
#else
                    this_len = sprintf
                      (bp, format,
                       ACE_static_cast (int, this->getpid ()));

#endif
                  else
                    this_len = sprintf
                      (bp, format, ACE_static_cast (int, this->getpid ()));
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case 'p':             // <errno> string, ala perror()
                  {
                    errno = ACE::map_errno (this->errnum ());
#if !defined (ACE_HAS_WINCE)     /* CE doesn't do strerror() */
//                    if (errno >= 0 && errno < sys_nerr)
                    if (errno >= 0 )
                      {
                        strcpy (fp, ACE_LIB_TEXT ("s: %s"));
                        if (can_check)
#ifndef OS_AIX
                          this_len = snprintf
                            (bp, bspace, format, va_arg (argp, ACE_TCHAR *),
                             (strerror (errno)));
#else
                          this_len = sprintf
                            (bp, format, va_arg (argp, ACE_TCHAR *),
                             (strerror (errno)));
#endif
                        else
                          this_len = sprintf
                            (bp, format, va_arg (argp, ACE_TCHAR *),
                              (strerror (errno)));
                      }
                    else
#endif /* !ACE_HAS_WINCE */
                      {
#if defined (ACE_WIN32)
                        ACE_TCHAR *lpMsgBuf = 0;

     // PharLap can't do FormatMessage, so try for socket
     // error.
# if !defined (ACE_HAS_PHARLAP)
                        ACE_TEXT_FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER
                                                  | FORMAT_MESSAGE_MAX_WIDTH_MASK
                                                  | FORMAT_MESSAGE_FROM_SYSTEM,
                                                  0,
                                                  errno,
                                                  MAKELANGID (LANG_NEUTRAL,
                                                              SUBLANG_DEFAULT),
                                                              // Default language
                                                  (ACE_TCHAR *) &lpMsgBuf,
                                                  0,
                                                  0);
# endif /* ACE_HAS_PHARLAP */

                        // If we don't get a valid response from
                        // <FormatMessage>, we'll assume this is a
                        // WinSock error and so we'll try to convert
                        // it into a string.  If this doesn't work it
                        // returns "unknown error" which is fine for
                        // our purposes.
                        if (lpMsgBuf == 0)
                          {
                            const ACE_TCHAR *message =
                              ACE::sock_error (errno);
                            strcpy (fp, ACE_LIB_TEXT ("s: %s"));
                            if (can_check)
#ifndef OS_AIX
                              this_len = snprintf
                                (bp, bspace, format,
                                 va_arg (argp, const ACE_TCHAR *),
                                 message);
#else
                              this_len = snprintf
                                (bp, bspace, format,
                                 va_arg (argp, const ACE_TCHAR *),
                                 message);
#endif
                            else
                              this_len = sprintf
                                (bp, format,
                                 va_arg (argp, const ACE_TCHAR *),
                                 message);
                          }
                        else
                          {
                            strcpy (fp, ACE_LIB_TEXT ("s: %s"));
                            if (can_check)
#ifndef OS_AIX
                              this_len = snprintf
                                (bp, bspace, format,
                                 va_arg (argp, ACE_TCHAR *),
                                 lpMsgBuf);
#else
                              this_len = sprintf
                                (bp, format,
                                 va_arg (argp, ACE_TCHAR *),
                                 lpMsgBuf);
#endif
                            else
                              this_len = sprintf
                                (bp, format,
                                 va_arg (argp, ACE_TCHAR *),
                                 lpMsgBuf);
                            // Free the buffer.
                            ::LocalFree (lpMsgBuf);
                          }
#elif !defined (ACE_HAS_WINCE)
                        strcpy (fp,
                                        ACE_LIB_TEXT (
                                                  "s: <unknown error> = %d"));
                        if (can_check)
#ifndef OS_AIX
                          this_len = snprintf
                            (bp, bspace,
                             format,
                             va_arg (argp, ACE_TCHAR *),
                             errno);
#else
                          this_len = sprintf
                            (bp, 
                             format,
                             va_arg (argp, ACE_TCHAR *),
                             errno);
#endif
                        else
                          this_len = sprintf
                            (bp,
                             format,
                             va_arg (argp, ACE_TCHAR *),
                             errno);
#endif /* ACE_WIN32 */
                      }
                    ACE_UPDATE_COUNT (bspace, this_len);
                    break;
                  }

                case 'M': // Print the name of the priority of the message.
                  strcpy (fp, ACE_LIB_TEXT ("s"));
                  if (can_check)
#ifndef OS_AIX
                    this_len = snprintf
                      (bp, bspace, format,
                       ACE_Log_Record::priority_name (log_priority));
#else
                    this_len = sprintf
                      (bp, format,
                       ACE_Log_Record::priority_name (log_priority));
#endif
                  else
                    this_len = sprintf
                      (bp, format,
                       ACE_Log_Record::priority_name (log_priority));
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case 'm': // Format the string assocated with the errno value.
                  {
                    errno = ACE::map_errno (this->errnum ());
//                    if (errno >= 0 && errno < sys_nerr)
                    if (errno >= 0 )
                      {
                        strcpy (fp, ACE_LIB_TEXT ("s"));
                        if (can_check)
#ifndef OS_AIX
                          this_len = snprintf
                            (bp, bspace, format,
                              (strerror (errno)));
#else
                          this_len = sprintf
                            (bp, format,
                              (strerror (errno)));
#endif
                        else
                          this_len = sprintf
                            (bp, format,
                              (strerror (errno)));
                      }
                    else
                      {
#if defined (ACE_WIN32)
                        ACE_TCHAR *lpMsgBuf = 0;

     // PharLap can't do FormatMessage, so try for socket
     // error.
# if !defined (ACE_HAS_PHARLAP)
                        ACE_TEXT_FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER
                                                  | FORMAT_MESSAGE_MAX_WIDTH_MASK
                                                  | FORMAT_MESSAGE_FROM_SYSTEM,
                                                  0,
                                                  errno,
                                                  MAKELANGID (LANG_NEUTRAL,
                                                              SUBLANG_DEFAULT),
                                                              // Default language
                                                  (ACE_TCHAR *) &lpMsgBuf,
                                                  0,
                                                  0);
# endif /* ACE_HAS_PHARLAP */

                        // If we don't get a valid response from
                        // <FormatMessage>, we'll assume this is a
                        // WinSock error and so we'll try to convert
                        // it into a string.  If this doesn't work it
                        // returns "unknown error" which is fine for
                        // our purposes.
                        if (lpMsgBuf == 0)
                          {
                            const ACE_TCHAR *message =
                              ACE::sock_error (errno);
                            strcpy (fp, ACE_LIB_TEXT ("s"));
                            if (can_check)
                              this_len = sprintf
                                (bp, format, message);
                            else
                              this_len = sprintf (bp, format, message);
                          }
                        else
                          {
                            strcpy (fp, ACE_LIB_TEXT ("s"));
                            if (can_check)
                              this_len = sprintf
                                (bp, format, lpMsgBuf);
                            else
                              this_len = sprintf
                                (bp, format, lpMsgBuf);
                            // Free the buffer.
                            ::LocalFree (lpMsgBuf);
                          }
#elif !defined (ACE_HAS_WINCE)
                        // Ignore the built format... if this is a problem,
                        // this part can be changed to build another string
                        // and pass that with the complete conversion specs.
                        if (can_check)
                          this_len = sprintf
                            (bp, 
                             ACE_LIB_TEXT ("<unknown error> = %d"), errno);
                        else
                          this_len = sprintf
                            (bp, ACE_LIB_TEXT ("<unknown error> = %d"), errno);
#endif /* ACE_WIN32 */
                      }
                    ACE_UPDATE_COUNT (bspace, this_len);
                    break;
                  }

                case 'R': // Format the return status of the operation.
                  this->op_status (va_arg (argp, int));
                  strcpy (fp, ACE_LIB_TEXT ("d"));
                  if (can_check)
                    this_len = sprintf
                      (bp, format, this->op_status ());
                  else
                    this_len = sprintf
                      (bp, format, this->op_status ());
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case '{': // Increment the trace_depth, then indent
                  skip_nul_locate = 1;
                  (void) this->inc ();
                  break;

                case '}': // indent, then decrement trace_depth
                  skip_nul_locate = 1;
                  (void) this->dec ();
                  break;

                case '$': // insert a newline, then indent the next line
                          // according to %I
                  *bp++ = '\n';
                  bspace--;
                  /* fallthrough */

                case 'I': // Indent with nesting_depth*width spaces
                  // Caller can do %*I to override nesting indent, and
                  // if %*I was done, wp has the extracted width.
#if defined (ACE_HAS_TRACE)
                  if (0 == wp)
                    wp = ACE_Trace::get_nesting_indent ();
#else
                  if (0 == wp)
                    wp = 4;
#endif /* ACE_HAS_TRACE */
                  wp *= this->trace_depth_;
                  if (ACE_static_cast (size_t, wp) > bspace)
                    wp = ACE_static_cast (int, bspace);

                  for (tmp_indent = wp;
                       tmp_indent;
                       tmp_indent--)
                    *bp++ = ' ';

                  *bp = '\0';
                  bspace -= ACE_static_cast (size_t, wp);
                  skip_nul_locate = 1;
                  break;

                case 'r': // Run (invoke) this subroutine.
                  {
                    int osave = ACE_Log_Msg::msg_off_;

                    if (ACE_BIT_ENABLED (ACE_Log_Msg::flags_,
                                         ACE_Log_Msg::SILENT) &&
                        bspace > 1)
                      {
                        *bp++ = '{';
                        bspace--;
                      }
                    ACE_Log_Msg::msg_off_ =  bp - this->msg_;

                    (*va_arg (argp, PTF))();

                    if (ACE_BIT_ENABLED (ACE_Log_Msg::flags_,
                                         ACE_Log_Msg::SILENT) &&
                        bspace > (1 + strlen (bp)))
                      {
                        bspace -= (strlen (bp) + 1);
                        bp += strlen (bp);
                        *bp++ =  '}';
                      }
                    *bp = '\0';
                    skip_nul_locate = 1;
                    ACE_Log_Msg::msg_off_ = osave;
                    break;
                  }

                case 'S': // format the string for with this signal number.
                  {
                    int sig = va_arg (argp, int);
#if defined (ACE_HAS_SYS_SIGLIST)
                    if (sig >= 0 && sig < ACE_NSIG)
                      {
                        strcpy (fp, ACE_LIB_TEXT ("s"));
                        if (can_check)
                          this_len = sprintf
                            (bp, format, _sys_siglist[sig]);
                        else
                          this_len = sprintf
                            (bp, format, _sys_siglist[sig]);
                      }
                    else
                      {
                        if (can_check)
                          this_len = sprintf
                            (bp, 
                             ACE_LIB_TEXT("<unknown signal> %d"), sig);
                        else
                          this_len = sprintf
                            (bp, ACE_LIB_TEXT ("<unknown signal> %d"), sig);
                      }
#else
                    if (can_check)
                      this_len = sprintf
                        (bp, ACE_LIB_TEXT ("signal %d"), sig);
                    else
                      this_len = sprintf
                        (bp, ACE_LIB_TEXT ("signal %d"), sig);
#endif /* ACE_HAS_SYS_SIGLIST */
                    ACE_UPDATE_COUNT (bspace, this_len);
                    break;
                  }

                case 'D': // Format the timestamp in month/day/year
                          // hour:minute:sec:usec format.
                  {
                    ACE_TCHAR day_and_time[35];
                    ACE::timestamp (day_and_time,
                                    sizeof day_and_time);
                    strcpy (fp, ACE_LIB_TEXT ("s"));
                    if (can_check)
                      this_len = sprintf
                        (bp, format, day_and_time);
                    else
                      this_len = sprintf (bp, format, day_and_time);
                    ACE_UPDATE_COUNT (bspace, this_len);
                    break;
                  }

                case 'T': // Format the timestamp in
                          // hour:minute:sec:usec format.
                  {
                    ACE_TCHAR day_and_time[35];
                    strcpy (fp, ACE_LIB_TEXT ("s"));
                    if (can_check)
                      this_len = sprintf
                        (bp, format,
                         ACE::timestamp (day_and_time, sizeof day_and_time));
                    else
                      this_len = sprintf
                        (bp, format, ACE::timestamp (day_and_time,
                                                     sizeof day_and_time));
                    ACE_UPDATE_COUNT (bspace, this_len);
                    break;
                  }

                case 't': // Format thread id.
                  break;

                case 's':                       // String
                  strcpy (fp, ACE_LIB_TEXT ("s"));
                  if (can_check)
                    this_len = sprintf
                      (bp, format, va_arg (argp, ACE_TCHAR *));
                  else
                    this_len = sprintf
                      (bp, format, va_arg (argp, ACE_TCHAR *));
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case 'C':         // Char string, Unicode for Win32/WCHAR
#if defined (ACE_WIN32) && defined (ACE_USES_WCHAR)
                  strcpy (fp, ACE_LIB_TEXT ("S"));
#else /* ACE_WIN32 && ACE_USES_WCHAR */
                  strcpy (fp, ACE_LIB_TEXT ("s"));
#endif /* ACE_WIN32 && ACE_USES_WCHAR */
                  if (can_check)
                    this_len = sprintf
                      (bp, format, va_arg (argp, ACE_TCHAR *));
                  else
                    this_len = sprintf
                      (bp, format, va_arg (argp, ACE_TCHAR *));
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case 'W':
#if defined (ACE_WIN32)
# if defined (ACE_USES_WCHAR)
                  strcpy (fp, ACE_LIB_TEXT ("s"));
# else /* ACE_USES_WCHAR */
                  strcpy (fp, ACE_LIB_TEXT ("S"));
# endif /* ACE_USES_WCHAR */
                  if (can_check)
                    this_len = sprintf
                      (bp, format, va_arg (argp, ACE_TCHAR *));
                  else
                    this_len = sprintf
                      (bp, format, va_arg (argp, ACE_TCHAR *));
#elif defined (ACE_HAS_WCHAR)
# if defined (HPUX)
                  strcpy (fp, ACE_LIB_TEXT ("S"));
# else
                  strcpy (fp, ACE_LIB_TEXT ("ls"));
# endif /* HPUX */
                  if (can_check)
                    this_len = sprintf
                      (bp, format, va_arg (argp, wchar_t *));
                  else
                    this_len = sprintf
                      (bp, format, va_arg (argp, wchar_t *));
#endif /* ACE_WIN32 / ACE_HAS_WCHAR */
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case 'w':              // Wide character
#if defined (ACE_WIN32)
# if defined (ACE_USES_WCHAR)
                  strcpy (fp, ACE_LIB_TEXT ("c"));
# else /* ACE_USES_WCHAR */
                  strcpy (fp, ACE_LIB_TEXT ("C"));
# endif /* ACE_USES_WCHAR */
                  if (can_check)
                    this_len = sprintf
                      (bp, format, va_arg (argp, int));
                  else
                    this_len = sprintf
                      (bp, format, va_arg (argp, int));
#elif defined (ACE_USES_WCHAR)
# if defined (HPUX)
                  strcpy (fp, ACE_LIB_TEXT ("C"));
# else
                  strcpy (fp, ACE_LIB_TEXT ("lc"));
# endif /* HPUX */
                  if (can_check)
                    this_len = sprintf
                      (bp, format, va_arg (argp, wint_t));
                  else
                    this_len = sprintf
                      (bp, format, va_arg (argp, wint_t));
#else /* ACE_WIN32 */
                  strcpy (fp, ACE_LIB_TEXT ("u"));
                  if (can_check)
                    this_len = sprintf
                      (bp, format, va_arg (argp, int));
                  else
                    this_len = sprintf
                      (bp, format, va_arg (argp, int));
#endif /* ACE_WIN32 */
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case 'z':              // ACE_OS::WChar character
                  {
                    // On some platforms sizeof (wchar_t) can be 2
                    // on the others 4 ...
                    wchar_t wtchar =
                      ACE_static_cast(wchar_t,
                                      va_arg (argp, int));
#if defined (ACE_WIN32)
# if defined (ACE_USES_WCHAR)
                    strcpy (fp, ACE_LIB_TEXT ("c"));
# else /* ACE_USES_WCHAR */
                    strcpy (fp, ACE_LIB_TEXT ("C"));
# endif /* ACE_USES_WCHAR */
#elif defined (ACE_USES_WCHAR)
# if defined (HPUX)
                    strcpy (fp, ACE_LIB_TEXT ("C"));
# else
                    strcpy (fp, ACE_LIB_TEXT ("lc"));
# endif /* HPUX */
#else /* ACE_WIN32 */
                    strcpy (fp, ACE_LIB_TEXT ("u"));
#endif /* ACE_WIN32 */
                    if (can_check)
                      this_len = sprintf (bp, format, wtchar);
                    else
                      this_len = sprintf (bp, format, wtchar);
                    ACE_UPDATE_COUNT (bspace, this_len);
                    break;
                  }

                 case 'Z':              // ACE_OS::WChar character string
                  {
/*                    ACE_OS::WChar *wchar_str = va_arg (argp, ACE_OS::WChar*);
                    if (wchar_str == 0)
                      break;

                    wchar_t *wchar_t_str = 0;
                    if (sizeof (ACE_OS::WChar) != sizeof (wchar_t))
                      {
                        size_t len = ACE_OS::wslen (wchar_str) + 1;
                        //@@ Bad, but there is no such ugly thing as
                        // ACE_NEW_BREAK and ACE_NEW has a return
                        // statement inside.
                        ACE_NEW_RETURN(wchar_t_str, wchar_t[len], 0);
                        if (wchar_t_str == 0)
                          break;

                        for (size_t i = 0; i < len; i++)
                          {
                            wchar_t_str[i] = wchar_str[i];
                          }
                      }

                    if (wchar_t_str == 0)
                      {
                        wchar_t_str = ACE_reinterpret_cast(wchar_t*,
                                                           wchar_str);
                      }
*/                      
#if defined (ACE_WIN32)
# if defined (ACE_USES_WCHAR)
                  strcpy (fp, ACE_LIB_TEXT ("s"));
# else /* ACE_USES_WCHAR */
                  strcpy (fp, ACE_LIB_TEXT ("S"));
# endif /* ACE_USES_WCHAR */
#elif defined (ACE_HAS_WCHAR)
# if defined (HPUX)
                  strcpy (fp, ACE_LIB_TEXT ("S"));
# else
                  strcpy (fp, ACE_LIB_TEXT ("ls"));
# endif /* HPUX */
#endif /* ACE_WIN32 / ACE_HAS_WCHAR */
/*                  if (can_check)
                    this_len = sprintf
                      (bp, format, wchar_t_str);
                  else
                    this_len = sprintf (bp, format, wchar_t_str);
                  if(sizeof(ACE_OS::WChar) != sizeof(wchar_t))
                    {
                      delete wchar_t_str;
                    }
*/
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;
                  }

                case 'c':
#if defined (ACE_WIN32) && defined (ACE_USES_WCHAR)
                  strcpy (fp, ACE_LIB_TEXT ("C"));
#else
                  strcpy (fp, ACE_LIB_TEXT ("c"));
#endif /* ACE_WIN32 && ACE_USES_WCHAR */
                  if (can_check)
                    this_len = sprintf
                      (bp, format, va_arg (argp, int));
                  else
                    this_len = sprintf
                      (bp, format, va_arg (argp, int));
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case 'd': case 'i': case 'o':
                case 'u': case 'x': case 'X':
                  fp[0] = *format_str;
                  fp[1] = '\0';
                  if (can_check)
                    this_len = sprintf
                      (bp, format, va_arg (argp, int));
                  else
                    this_len = sprintf
                      (bp, format, va_arg (argp, int));
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case 'F': case 'f': case 'e': case 'E':
                case 'g': case 'G':
                  fp[0] = *format_str;
                  fp[1] = '\0';
                  if (can_check)
                    this_len = sprintf
                      (bp, format, va_arg (argp, double));
                  else
                    this_len = sprintf
                      (bp, format, va_arg (argp, double));
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case 'Q':
#if defined (ACE_LACKS_LONGLONG_T)
                  {
                    // This relies on the ACE_U_LongLong storage layout.
                    ACE_UINT32 hi = va_arg (argp, ACE_UINT32);
                    ACE_UINT32 lo = va_arg (argp, ACE_UINT32);
                    if (hi > 0)
                      this_len = sprintf (bp,
                                                  "0x%lx%0*lx",
                                                  hi,
                                                  2 * sizeof lo,
                                                  lo);
                    else
                      this_len = sprintf (bp, "0x%lx", lo);
                  }
#else  /* ! ACE_LACKS_LONGLONG_T */
                  {
                    /* const char *fmt = ACE_UINT64_FORMAT_SPECIFIER;
                    strcpy (fp, &fmt[1]);    // Skip leading %
                    if (can_check)
                      this_len = sprintf (bp, 
                                                   format,
                                                   va_arg (argp, ACE_UINT64));
                    else
                      this_len = sprintf (bp,
                                                  format,
                                                  va_arg (argp, ACE_UINT64));
*/
                  }
#endif /* ! ACE_LACKS_LONGLONG_T */
                  ACE_UPDATE_COUNT (bspace, this_len);
                  break;

                case '@':
                    strcpy (fp, ACE_LIB_TEXT ("p"));
                    if (can_check)
                      this_len = sprintf
                        (bp, format, va_arg (argp, void*));
                    else
                      this_len = sprintf
                        (bp, format, va_arg (argp, void*));
                    ACE_UPDATE_COUNT (bspace, this_len);
                    break;

                default:
                  // So, it's not a legit format specifier after all...
                  // Copy from the original % to where we are now, then
                  // continue with whatever comes next.
                  while (start_format != format_str && bspace > 0)
                    {
                      *bp++ = *start_format++;
                      bspace--;
                    }
                  if (bspace > 0)
                    {
                      *bp++ = *format_str;
                      bspace--;
                    }
                  break;
                }

              // Bump to the next char in the caller's format_str
              format_str++;
            }

          if (!skip_nul_locate)
            while (*bp != '\0') // Locate end of bp.
              bp++;
        }
    }

  *bp = '\0'; // Terminate bp, but don't auto-increment this!

  // Check that memory was not corrupted.
  if (bp >= this->msg_ + sizeof this->msg_)
    {
      abort_prog = 1;
      fprintf (stderr,
                       "The following logged message is too long!\n");
    }

  // Copy the message from thread-specific storage into the transfer
  // buffer (this can be optimized away by changing other code...).
  log_record.msg_data (this->msg ());

  // Write the <log_record> to the appropriate location.
  ssize_t result = this->log (log_record,
                              abort_prog);

  if (abort_prog)
    {
      // Since we are now calling abort instead of exit, this value is
      // not used.
      ACE_UNUSED_ARG (exit_value);

      // *Always* print a message to stderr if we're aborting.  We
      // don't use verbose, however, to avoid recursive aborts if
      // something is hosed.
      log_record.print (ACE_Log_Msg::local_host_, 0, stderr);
      abort ();
    }

   return result;
}

#if !defined (ACE_WIN32)
/**
 * @class ACE_Log_Msg_Sig_Guard
 *
 * @brief For use only by ACE_Log_Msg.
 *
 * Doesn't require the use of global variables or global
 * functions in an application).
 */
class ACE_Log_Msg_Sig_Guard
{
private:
  ACE_Log_Msg_Sig_Guard (void);
  ~ACE_Log_Msg_Sig_Guard (void);

  /// Original signal mask.
  sigset_t omask_;

  friend ssize_t ACE_Log_Msg::log (ACE_Log_Record &log_record,
                                   int suppress_stderr);
};

ACE_Log_Msg_Sig_Guard::ACE_Log_Msg_Sig_Guard (void)
{
#if !defined (ACE_LACKS_UNIX_SIGNALS)
  sigemptyset (&this->omask_);

#  if defined (ACE_LACKS_PTHREAD_THR_SIGSETMASK)
  sigprocmask (SIG_BLOCK,
                       ACE_OS_Object_Manager::default_mask (),
                       &this->omask_);
#  else
#  endif /* ACE_LACKS_PTHREAD_THR_SIGSETMASK */
#endif /* ACE_LACKS_UNIX_SIGNALS */
}

ACE_Log_Msg_Sig_Guard::~ACE_Log_Msg_Sig_Guard (void)
{
  sigprocmask (SIG_SETMASK,
                       &this->omask_,
                       0);
}
#endif /* ! ACE_WIN32 */

ssize_t
ACE_Log_Msg::log (ACE_Log_Record &log_record,
                  int suppress_stderr)
{
  ssize_t result = 0;

  // Format the message and print it to stderr and/or ship it off to
  // the log_client daemon, and/or print it to the ostream.  Of
  // course, only print the message if "SILENT" mode is disabled.
  if (ACE_BIT_DISABLED (ACE_Log_Msg::flags_,
                        ACE_Log_Msg::SILENT))
    {
      int tracing = this->tracing_enabled ();
      this->stop_tracing ();

#if !defined (ACE_WIN32)
      // Make this block signal-safe.
      ACE_Log_Msg_Sig_Guard sb;
#endif /* !ACE_WIN32 && !ACE_PSOS */

      // Do the callback, if needed, before acquiring the lock
      // to avoid holding the lock during the callback so we don't
      // have deadlock if the callback uses the logger.
      if (ACE_BIT_ENABLED (ACE_Log_Msg::flags_,
                           ACE_Log_Msg::MSG_CALLBACK)
          && this->msg_callback () != 0)
        this->msg_callback ()->log (log_record);

      // Make sure that the lock is held during all this.

      if (ACE_BIT_ENABLED (ACE_Log_Msg::flags_,
                           ACE_Log_Msg::STDERR)
          && !suppress_stderr) // This is taken care of by our caller.
        log_record.print (ACE_Log_Msg::local_host_,
                          ACE_Log_Msg::flags_,
                          stderr);

      if (ACE_BIT_ENABLED (ACE_Log_Msg::flags_, ACE_Log_Msg::CUSTOM) ||
          ACE_BIT_ENABLED (ACE_Log_Msg::flags_, ACE_Log_Msg::SYSLOG) ||
          ACE_BIT_ENABLED (ACE_Log_Msg::flags_, ACE_Log_Msg::LOGGER))
        {
          // Be sure that there is a message_queue_, with multiple threads.
        }


      if (ACE_BIT_ENABLED (ACE_Log_Msg::flags_, ACE_Log_Msg::LOGGER) ||
          ACE_BIT_ENABLED (ACE_Log_Msg::flags_, ACE_Log_Msg::SYSLOG))
        {
          result =
            ACE_Log_Msg_Manager::log_backend_->log (log_record);
        }

      if (ACE_BIT_ENABLED (ACE_Log_Msg::flags_, ACE_Log_Msg::CUSTOM) &&
          ACE_Log_Msg_Manager::custom_backend_ != 0)
        {
          result =
            ACE_Log_Msg_Manager::custom_backend_->log (log_record);
        }


      // This must come last, after the other two print operations
      // (see the <ACE_Log_Record::print> method for details).
      if (ACE_BIT_ENABLED (ACE_Log_Msg::flags_,
                           ACE_Log_Msg::OSTREAM)
          && this->msg_ostream () != 0)
        log_record.print (ACE_Log_Msg::local_host_,
                          ACE_Log_Msg::flags_,
#if defined (ACE_LACKS_IOSTREAM_TOTALLY)
                          ACE_static_cast (FILE *,
                                           this->msg_ostream ())
#else  /* ! ACE_LACKS_IOSTREAM_TOTALLY */
                          *this->msg_ostream ()
#endif /* ! ACE_LACKS_IOSTREAM_TOTALLY */
                          );

      if (tracing)
        this->start_tracing ();
   }

  return result;
}

// Calls log to do the actual print, but formats first.

int
ACE_Log_Msg::log_hexdump (ACE_Log_Priority log_priority,
                          const char *buffer,
                          size_t size,
                          const ACE_TCHAR *text)
{
  ACE_TCHAR buf[ACE_Log_Record::MAXLOGMSGLEN -
    ACE_Log_Record::VERBOSE_LEN - 58];
  // 58 for the HEXDUMP header;

  ACE_TCHAR *msg_buf;
  const size_t text_sz = text ? strlen(text) : 0;
  ACE_NEW_RETURN (msg_buf,
                  ACE_TCHAR[text_sz + 58],
                 -1);

  buf[0] = 0; // in case size = 0

  const size_t len = ACE::format_hexdump
    (buffer, size, buf, sizeof (buf) / sizeof (ACE_TCHAR) - text_sz);

  int sz = 0;

  if (text)
    sz = sprintf (msg_buf,
                          ACE_LIB_TEXT ("%s - "),
                          text);

  sz += sprintf (msg_buf + sz,
                         "HEXDUMP %u bytes", 
                         size);

  if (len < size)
    sprintf (msg_buf + sz,
                     " (showing first %u bytes)", 
                     len);

  // Now print out the formatted buffer.
  this->log (log_priority,
             ACE_LIB_TEXT ("%s\n%s"),
             msg_buf,
             buf);

  delete [] msg_buf;
  return 0;
}

void
ACE_Log_Msg::set (const char *filename,
                  int line,
                  int status,
                  int err,
                  int rs,
                  ACE_OSTREAM_TYPE *os,
                  ACE_Log_Msg_Callback *c)
{
  ACE_TRACE ("ACE_Log_Msg::set");
  this->file (filename);
  this->linenum (line);
  this->op_status (status);
  this->errnum (err);
  this->restart (rs);
  this->msg_ostream (os);
  this->msg_callback (c);
}

void
ACE_Log_Msg::conditional_set (const char *filename,
                              int line,
                              int status,
                              int err)
{
  this->conditional_values_.is_set_ = 1;
  this->conditional_values_.file_ = filename;
  this->conditional_values_.line_ = line;
  this->conditional_values_.op_status_ = status;
  this->conditional_values_.errnum_ = err;
}

void
ACE_Log_Msg::dump (void) const
{
#if defined (ACE_HAS_DUMP)
  ACE_TRACE ("ACE_Log_Msg::dump");

  ACE_DEBUG ((LM_DEBUG, ACE_BEGIN_DUMP, this));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("status_ = %d\n"), this->status_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nerrnum_ = %d\n"), this->errnum_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nlinenum_ = %d\n"), this->linenum_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nfile_ = %s\n"), this->file_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nmsg_ = %s\n"), this->msg_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nrestart_ = %d\n"), this->restart_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nostream_ = %x\n"), this->ostream_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nmsg_callback_ = %x\n"),
              this->msg_callback_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nprogram_name_ = %s\n"),
              this->program_name_ ? this->program_name_
                                  : ACE_LIB_TEXT ("<unknown>")));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nlocal_host_ = %s\n"),
              this->local_host_ ? this->local_host_
                                : ACE_LIB_TEXT ("<unknown>")));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\npid_ = %d\n"), this->getpid ()));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nflags_ = %x\n"), this->flags_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\ntrace_depth_ = %d\n"),
              this->trace_depth_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\ntrace_active_ = %d\n"),
              this->trace_active_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\ntracing_enabled_ = %d\n"),
              this->tracing_enabled_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\npriority_mask_ = %x\n"),
              this->priority_mask_));
  if (this->thr_desc_ != 0 && this->thr_desc_->state () != 0)
    ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nthr_state_ = %d\n"),
                this->thr_desc_->state ()));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nmsg_off_ = %d\n"), this->msg_off_));

  // Be sure that there is a message_queue_, with multiple threads.
  ACE_Log_Msg_Manager::init_backend ();

  // Synchronize output operations.

  ACE_DEBUG ((LM_DEBUG, ACE_END_DUMP));
#endif /* ACE_HAS_DUMP */
}

void
ACE_Log_Msg::op_status (int status)
{
  this->status_ = status;
}

int
ACE_Log_Msg::op_status (void)
{
  return this->status_;
}

void
ACE_Log_Msg::restart (int r)
{
  this->restart_ = r;
}

int
ACE_Log_Msg::restart (void)
{
  return this->restart_;
}

int
ACE_Log_Msg::errnum (void)
{
  return this->errnum_;
}

void
ACE_Log_Msg::errnum (int e)
{
  this->errnum_ = e;
}

int
ACE_Log_Msg::linenum (void)
{
  return this->linenum_;
}

void
ACE_Log_Msg::linenum (int l)
{
  this->linenum_ = l;
}

int
ACE_Log_Msg::inc (void)
{
  return this->trace_depth_++;
}

int
ACE_Log_Msg::dec (void)
{
  return this->trace_depth_ == 0 ? 0 : --this->trace_depth_;
}

int
ACE_Log_Msg::trace_depth (void)
{
  return this->trace_depth_;
}

void
ACE_Log_Msg::trace_depth (int depth)
{
  this->trace_depth_ = depth;
}

int
ACE_Log_Msg::trace_active (void)
{
  return this->trace_active_;
}

void
ACE_Log_Msg::trace_active (int value)
{
  this->trace_active_ = value;
}


#if defined (ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS) && defined(ACE_LEGACY_MODE)
ACE_SEH_EXCEPT_HANDLER
ACE_Log_Msg::seh_except_selector (void)
{
  return ACE_OS_Object_Manager::seh_except_selector ();
}

ACE_SEH_EXCEPT_HANDLER
ACE_Log_Msg::seh_except_selector (ACE_SEH_EXCEPT_HANDLER n)
{
  return ACE_OS_Object_Manager::seh_except_selector (n);
}

ACE_SEH_EXCEPT_HANDLER
ACE_Log_Msg::seh_except_handler (void)
{
  return ACE_OS_Object_Manager::seh_except_handler ();
}

ACE_SEH_EXCEPT_HANDLER
ACE_Log_Msg::seh_except_handler (ACE_SEH_EXCEPT_HANDLER n)
{
  return ACE_OS_Object_Manager::seh_except_handler (n);
}
#endif /* ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS && ACE_LEGACY_MODE */

// Enable the tracing facility on a per-thread basis.

void
ACE_Log_Msg::start_tracing (void)
{
  this->tracing_enabled_ = 1;
}

// Disable the tracing facility on a per-thread basis.

void
ACE_Log_Msg::stop_tracing (void)
{
  this->tracing_enabled_ = 0;
}

int
ACE_Log_Msg::tracing_enabled (void)
{
  return this->tracing_enabled_;
}

const char *
ACE_Log_Msg::file (void)
{
  return this->file_;
}

void
ACE_Log_Msg::file (const char *s)
{
  strncpy (this->file_, s, sizeof this->file_);
}

const ACE_TCHAR *
ACE_Log_Msg::msg (void)
{
  return this->msg_ + ACE_Log_Msg::msg_off_;
}

void
ACE_Log_Msg::msg (const ACE_TCHAR *m)
{
  strncpy (this->msg_, m,
                    (sizeof this->msg_ / sizeof (ACE_TCHAR)));
}

ACE_Log_Msg_Callback *
ACE_Log_Msg::msg_callback (void) const
{
  return this->msg_callback_;
}

ACE_Log_Msg_Callback *
ACE_Log_Msg::msg_callback (ACE_Log_Msg_Callback *c)
{
  ACE_Log_Msg_Callback *old = this->msg_callback_;
  this->msg_callback_ = c;
  return old;
}

ACE_Log_Msg_Backend *
ACE_Log_Msg::msg_backend (ACE_Log_Msg_Backend *b)
{
  ACE_TRACE ("ACE_Log_Msg::msg_backend");

  ACE_Log_Msg_Backend *tmp  = ACE_Log_Msg_Manager::custom_backend_;
  ACE_Log_Msg_Manager::custom_backend_ = b;
  return tmp;
}

ACE_Log_Msg_Backend *
ACE_Log_Msg::msg_backend (void)
{
  ACE_TRACE ("ACE_Log_Msg::msg_backend");

  return ACE_Log_Msg_Manager::custom_backend_;
}

ACE_OSTREAM_TYPE *
ACE_Log_Msg::msg_ostream (void) const
{
  return this->ostream_;
}

void
ACE_Log_Msg::msg_ostream (ACE_OSTREAM_TYPE *m, int delete_ostream)
{
  this->delete_ostream_ = delete_ostream;
  this->ostream_ = m;
}

void
ACE_Log_Msg::msg_ostream (ACE_OSTREAM_TYPE *m)
{
  this->ostream_ = m;
}

void
ACE_Log_Msg::local_host (const ACE_TCHAR *s)
{
  if (s)
    {
      free ((void *) ACE_Log_Msg::local_host_);
      {
        ACE_NO_HEAP_CHECK;

        ACE_ALLOCATOR (ACE_Log_Msg::local_host_, strdup (s));
      }
    }
}

const ACE_TCHAR *
ACE_Log_Msg::local_host (void) const
{
  return ACE_Log_Msg::local_host_;
}

pid_t ACE_Log_Msg::getpid (void) const
{
  if (ACE_Log_Msg::pid_ == -1)
    ACE_Log_Msg::pid_ = ::getpid ();

  return ACE_Log_Msg::pid_;
}

int
ACE_Log_Msg::log_priority_enabled (ACE_Log_Priority log_priority,
                                   const char *,
                                   ...)
{
  return this->log_priority_enabled (log_priority);
}

#if defined (ACE_USES_WCHAR)
int
ACE_Log_Msg::log_priority_enabled (ACE_Log_Priority log_priority,
                                   const wchar_t *,
                                   ...)
{
  return this->log_priority_enabled (log_priority);
}
#endif /* ACE_USES_WCHAR */

// ****************************************************************

void
ACE_Log_Msg::init_hook (ACE_OS_Log_Msg_Attributes &attributes
# if defined (ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS)
                        , ACE_SEH_EXCEPT_HANDLER selector
                        , ACE_SEH_EXCEPT_HANDLER handler
# endif /* ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS */
                                   )
{
# if defined (ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS)
  attributes.seh_except_selector_ = selector;
  attributes.seh_except_handler_ = handler;
# endif /* ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS */
  if (ACE_Log_Msg::exists ())
    {
      ACE_Log_Msg *inherit_log = ACE_LOG_MSG;
      attributes.ostream_ = inherit_log->msg_ostream ();
      attributes.priority_mask_ = inherit_log->priority_mask ();
      attributes.tracing_enabled_ = inherit_log->tracing_enabled ();
      attributes.restart_ = inherit_log->restart ();
      attributes.trace_depth_ = inherit_log->trace_depth ();
    }
}

#if defined (ACE_THREADS_DONT_INHERIT_LOG_MSG)  || \
    defined (ACE_HAS_MINIMAL_ACE_OS)
# if defined (ACE_PSOS)
// Unique file identifier
static int ACE_PSOS_unique_file_id = 0;
# endif /* ACE_PSOS */
#endif /* ACE_THREADS_DONT_INHERIT_LOG_MSG) || ACE_HAS_MINIMAL_ACE_OS */

void
ACE_Log_Msg::inherit_hook (ACE_OS_Log_Msg_Attributes &attributes)
{

}

