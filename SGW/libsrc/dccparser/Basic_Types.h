#ifndef __BASIC_TYPES_H__
#define __BASIC_TYPES_H__
   
typedef short ACE_INT16;
typedef unsigned short ACE_UINT16;
typedef int ACE_INT32;
typedef unsigned int ACE_UINT32;
#if defined(WIN32) || defined(BIT32)
typedef long long ACE_INT64;
typedef unsigned long long ACE_UINT64;
#else
typedef long ACE_INT64;
typedef unsigned long ACE_UINT64;
#endif

typedef int ACE_Message_Type;
typedef unsigned long Message_Flags;

enum ParseOption {
  PARSE_LOOSE = 0,
  PARSE_STRICT = 1
};

enum Log_Priority
{
  // = Note, this first argument *must* start at 1!
  /// Shutdown the logger (decimal 1).
  LM_SHUTDOWN = 01,

  /// Messages indicating function-calling sequence (decimal 2).
  LM_TRACE = 02,

  /// Messages that contain information normally of use only when
  /// debugging a program (decimal 4).
  LM_DEBUG = 04,

  /// Informational messages (decimal 8).
  LM_INFO = 010,

  /// Conditions that are not error conditions, but that may require
  /// special handling (decimal 16).
  LM_NOTICE = 020,

  /// Warning messages (decimal 32).
  LM_WARNING = 040,

  /// Initialize the logger (decimal 64).
  LM_STARTUP = 0100,

  /// Error messages (decimal 128).
  LM_ERROR = 0200,

  /// Critical conditions, such as hard device errors (decimal 256).
  LM_CRITICAL = 0400,

  /// A condition that should be corrected immediately, such as a
  /// corrupted system database (decimal 512).
  LM_ALERT = 01000,

  /// A panic condition.  This is normally broadcast to all users
  /// (decimal 1024).
  LM_EMERGENCY = 02000,

  /// The maximum logging priority.
  LM_MAX = LM_EMERGENCY,

  /// Do not use!!  This enum value ensures that the underlying
  /// integral type for this enum is at least 32 bits.
  LM_ENSURE_32_BITS = 0x7FFFFFFF
};
 
#endif

