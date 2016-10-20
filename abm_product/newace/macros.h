#ifndef _MACROS_H
#define _MACROS_H

#include "config.h"


#ifdef DEF_AIX
#include<sys/select.h>
#endif

#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<errno.h>

#include <sys/param.h>

#define ACE_TRACE_IMPL(X) ACE_Trace ____ (ACE_LIB_TEXT (X), __LINE__, ACE_LIB_TEXT (__FILE__))

// By default tracing is turned off.
#if !defined (ACE_NTRACE)
#define ACE_NTRACE 1
#endif /* ACE_NTRACE */

#if (ACE_NTRACE == 1)
#define ACE_TRACE(X)
#else
#if !defined (ACE_HAS_TRACE)
#define ACE_HAS_TRACE
#endif /* ACE_HAS_TRACE */
#define ACE_TRACE(X) ACE_TRACE_IMPL(X)
#include "Trace.h"
#endif /* ACE_NTRACE */


#define ACE_MEM_POOL_OPTIONS _ACE_MEM_POOL_OPTIONS

#if defined (ghs) || defined (__GNUC__) || defined (__hpux) || defined (__sgi) || defined (__DECCXX) || defined (__KCC) || defined (__rational__) || defined (__USLC__) || defined (ACE_RM544) || defined (__DCC__) || defined (__PGI) || defined (__TANDEM)
// Some compilers complain about "statement with no effect" with (a).
// This eliminates the warnings, and no code is generated for the null
// conditional statement.  @note that may only be true if -O is enabled,
// such as with GreenHills (ghs) 1.8.8.
# define ACE_UNUSED_ARG(a) do {/* null */} while (&a == 0)
#else /* ghs || __GNUC__ || ..... */
# define ACE_UNUSED_ARG(a) (a)
#endif /* ghs || __GNUC__ || ..... */


// ============================================================================
// ACE_ALLOC_HOOK* macros
//
// Macros to declare and define class-specific allocation operators.
// ============================================================================

# if defined (ACE_HAS_ALLOC_HOOKS)
#   define ACE_ALLOC_HOOK_DECLARE \
  void *operator new (size_t bytes); \
  void operator delete (void *ptr);

  // Note that these are just place holders for now.  Some day they
  // may be be replaced by <ACE_Malloc>.
#   define ACE_ALLOC_HOOK_DEFINE(CLASS) \
  void *CLASS::operator new (size_t bytes) { return ::new char[bytes]; } \
  void CLASS::operator delete (void *ptr) { delete [] ((char *) ptr); }
# else
#   define ACE_ALLOC_HOOK_DECLARE struct __Ace {} /* Just need a dummy... */
#   define ACE_ALLOC_HOOK_DEFINE(CLASS)
# endif /* ACE_HAS_ALLOC_HOOKS */

# define ACE_POW(X) (((X) == 0)?1:(X-=1,X|=X>>1,X|=X>>2,X|=X>>4,X|=X>>8,X|=X>>16,(++X)))
# define ACE_EVEN(NUM) (((NUM) & 1) == 0)
# define ACE_ODD(NUM) (((NUM) & 1) == 1)
# define ACE_BIT_ENABLED(WORD, BIT) (((WORD) & (BIT)) != 0)
# define ACE_BIT_DISABLED(WORD, BIT) (((WORD) & (BIT)) == 0)
# define ACE_BIT_CMP_MASK(WORD, BIT, MASK) (((WORD) & (BIT)) == MASK)
# define ACE_SET_BITS(WORD, BITS) (WORD |= (BITS))
# define ACE_CLR_BITS(WORD, BITS) (WORD &= ~(BITS))

// ============================================================================
/**
 * ACE_OSCALL* macros
 *
 * @deprecated ACE_OSCALL_RETURN and ACE_OSCALL should not be used.
 *             Please restart system calls in your application code.
 *             See the @c sigaction(2) man page for documentation
 *             regarding enabling restartable system calls across
 *             signals via the @c SA_RESTART flag.
 *
 * The following two macros used ensure that system calls are properly
 * restarted (if necessary) when interrupts occur.  However, that
 * capability was never enabled by any of our supported platforms.
 * In fact, some parts of ACE would not function properly when that
 * ability was enabled.  Furthermore, they assumed that ability to
 * restart system calls was determined statically.  That assumption
 * does not hold for modern platforms, where that ability is
 * determined dynamically at run-time.
 */
// ============================================================================

#define ACE_OSCALL_RETURN(X,TYPE,FAILVALUE) \
  do \
    return (TYPE) (X); \
  while (0)
#define ACE_OSCALL(X,TYPE,FAILVALUE,RESULT) \
  do \
    RESULT = (TYPE) (X); \
  while (0)

# define ACE_ALLOCATOR_RETURN(POINTER,ALLOCATOR,RET_VAL) \
   do { POINTER = ALLOCATOR; \
     if (POINTER == 0) { errno = ENOMEM; return RET_VAL; } \
   } while (0)
# define ACE_ALLOCATOR(POINTER,ALLOCATOR) \
   do { POINTER = ALLOCATOR; \
     if (POINTER == 0) { errno = ENOMEM; return; } \
   } while (0)
# define ACE_ALLOCATOR_NORETURN(POINTER,ALLOCATOR) \
   do { POINTER = ALLOCATOR; \
     if (POINTER == 0) { errno = ENOMEM; } \
   } while (0)

# define ACE_NEW_MALLOC_RETURN(POINTER,ALLOCATOR,CONSTRUCTOR,RET_VAL) \
   do { POINTER = ALLOCATOR; \
     if (POINTER == 0) { errno = ENOMEM; return RET_VAL;} \
     else { new (POINTER) CONSTRUCTOR; } \
   } while (0)
# define ACE_NEW_MALLOC(POINTER,ALLOCATOR,CONSTRUCTOR) \
   do { POINTER = ALLOCATOR; \
     if (POINTER == 0) { errno = ENOMEM; return;} \
     else { new (POINTER) CONSTRUCTOR; } \
   } while (0)
# define ACE_NEW_MALLOC_NORETURN(POINTER,ALLOCATOR,CONSTRUCTOR) \
   do { POINTER = ALLOCATOR; \
     if (POINTER == 0) { errno = ENOMEM;} \
     else { new (POINTER) CONSTRUCTOR; } \
   } while (0)


// ----------------------------------------------------------------

# define ACE_DES_NOFREE(POINTER,CLASS) \
   do { \
        if (POINTER) \
          { \
            (POINTER)->~CLASS (); \
          } \
      } \
   while (0)

# define ACE_DES_ARRAY_NOFREE(POINTER,SIZE,CLASS) \
   do { \
        if (POINTER) \
          { \
            for (size_t i = 0; \
                 i < SIZE; \
                 ++i) \
            { \
              (&(POINTER)[i])->~CLASS (); \
            } \
          } \
      } \
   while (0)

# define ACE_DES_FREE(POINTER,DEALLOCATOR,CLASS) \
   do { \
        if (POINTER) \
          { \
            (POINTER)->~CLASS (); \
            DEALLOCATOR (POINTER); \
          } \
      } \
   while (0)

# define ACE_DES_ARRAY_FREE(POINTER,SIZE,DEALLOCATOR,CLASS) \
   do { \
        if (POINTER) \
          { \
            for (size_t i = 0; \
                 i < SIZE; \
                 ++i) \
            { \
              (&(POINTER)[i])->~CLASS (); \
            } \
            DEALLOCATOR (POINTER); \
          } \
      } \
   while (0)

# if defined (ACE_HAS_WORKING_EXPLICIT_TEMPLATE_DESTRUCTOR)
#   define ACE_DES_NOFREE_TEMPLATE(POINTER,T_CLASS,T_PARAMETER) \
     do { \
          if (POINTER) \
            { \
              (POINTER)->~T_CLASS (); \
            } \
        } \
     while (0)
#   define ACE_DES_ARRAY_NOFREE_TEMPLATE(POINTER,SIZE,T_CLASS,T_PARAMETER) \
     do { \
          if (POINTER) \
            { \
              for (size_t i = 0; \
                   i < SIZE; \
                   ++i) \
              { \
                (&(POINTER)[i])->~T_CLASS (); \
              } \
            } \
        } \
     while (0)

#if defined (ACE_EXPLICIT_TEMPLATE_DESTRUCTOR_TAKES_ARGS)
#   define ACE_DES_FREE_TEMPLATE(POINTER,DEALLOCATOR,T_CLASS,T_PARAMETER) \
     do { \
          if (POINTER) \
            { \
              (POINTER)->~T_CLASS T_PARAMETER (); \
              DEALLOCATOR (POINTER); \
            } \
        } \
     while (0)
#else
#   define ACE_DES_FREE_TEMPLATE(POINTER,DEALLOCATOR,T_CLASS,T_PARAMETER) \
     do { \
          if (POINTER) \
            { \
              (POINTER)->~T_CLASS (); \
              DEALLOCATOR (POINTER); \
            } \
        } \
     while (0)
#endif /* defined(ACE_EXPLICIT_TEMPLATE_DESTRUCTOR_TAKES_ARGS) */
#   define ACE_DES_ARRAY_FREE_TEMPLATE(POINTER,SIZE,DEALLOCATOR,T_CLASS,T_PARAMETER) \
     do { \
          if (POINTER) \
            { \
              for (size_t i = 0; \
                   i < SIZE; \
                   ++i) \
              { \
                (&(POINTER)[i])->~T_CLASS (); \
              } \
              DEALLOCATOR (POINTER); \
            } \
        } \
     while (0)
#if defined(ACE_EXPLICIT_TEMPLATE_DESTRUCTOR_TAKES_ARGS)
#   define ACE_DES_FREE_TEMPLATE2(POINTER,DEALLOCATOR,T_CLASS,T_PARAM1,T_PARAM2) \
     do { \
          if (POINTER) \
            { \
              (POINTER)->~T_CLASS <T_PARAM1, T_PARAM2> (); \
              DEALLOCATOR (POINTER); \
            } \
        } \
     while (0)
#else
#   define ACE_DES_FREE_TEMPLATE2(POINTER,DEALLOCATOR,T_CLASS,T_PARAM1,T_PARAM2) \
     do { \
          if (POINTER) \
            { \
              (POINTER)->~T_CLASS (); \
              DEALLOCATOR (POINTER); \
            } \
        } \
     while (0)
#endif /* defined(ACE_EXPLICIT_TEMPLATE_DESTRUCTOR_TAKES_ARGS) */
#   define ACE_DES_FREE_TEMPLATE3(POINTER,DEALLOCATOR,T_CLASS,T_PARAM1,T_PARAM2,T_PARAM3) \
     do { \
          if (POINTER) \
            { \
              (POINTER)->~T_CLASS (); \
              DEALLOCATOR (POINTER); \
            } \
        } \
     while (0)
#   define ACE_DES_FREE_TEMPLATE4(POINTER,DEALLOCATOR,T_CLASS,T_PARAM1,T_PARAM2,T_PARAM3, T_PARAM4) \
     do { \
          if (POINTER) \
            { \
              (POINTER)->~T_CLASS (); \
              DEALLOCATOR (POINTER); \
            } \
        } \
     while (0)
#   define ACE_DES_ARRAY_FREE_TEMPLATE2(POINTER,SIZE,DEALLOCATOR,T_CLASS,T_PARAM1,T_PARAM2) \
     do { \
          if (POINTER) \
            { \
              for (size_t i = 0; \
                   i < SIZE; \
                   ++i) \
              { \
                (&(POINTER)[i])->~T_CLASS (); \
              } \
              DEALLOCATOR (POINTER); \
            } \
        } \
     while (0)
# else /* ! ACE_HAS_WORKING_EXPLICIT_TEMPLATE_DESTRUCTOR */
#   define ACE_DES_NOFREE_TEMPLATE(POINTER,T_CLASS,T_PARAMETER) \
     do { \
          if (POINTER) \
            { \
              (POINTER)->T_CLASS T_PARAMETER::~T_CLASS (); \
            } \
        } \
     while (0)
#   define ACE_DES_ARRAY_NOFREE_TEMPLATE(POINTER,SIZE,T_CLASS,T_PARAMETER) \
     do { \
          if (POINTER) \
            { \
              for (size_t i = 0; \
                   i < SIZE; \
                   ++i) \
              { \
                (POINTER)[i].T_CLASS T_PARAMETER::~T_CLASS (); \
              } \
            } \
        } \
     while (0)
#   if defined (__Lynx__) && __LYNXOS_SDK_VERSION == 199701L
  // LynxOS 3.0.0's g++ has trouble with the real versions of these.
#     define ACE_DES_FREE_TEMPLATE(POINTER,DEALLOCATOR,T_CLASS,T_PARAMETER)
#     define ACE_DES_ARRAY_FREE_TEMPLATE(POINTER,DEALLOCATOR,T_CLASS,T_PARAMETER)
#     define ACE_DES_FREE_TEMPLATE2(POINTER,DEALLOCATOR,T_CLASS,T_PARAM1,T_PARAM2)
#     define ACE_DES_FREE_TEMPLATE3(POINTER,DEALLOCATOR,T_CLASS,T_PARAM1,T_PARAM2)
#     define ACE_DES_FREE_TEMPLATE4(POINTER,DEALLOCATOR,T_CLASS,T_PARAM1,T_PARAM2)
#     define ACE_DES_ARRAY_FREE_TEMPLATE2(POINTER,DEALLOCATOR,T_CLASS,T_PARAM1,T_PARAM2)
#   else
#     define ACE_DES_FREE_TEMPLATE(POINTER,DEALLOCATOR,T_CLASS,T_PARAMETER) \
       do { \
            if (POINTER) \
              { \
                POINTER->T_CLASS T_PARAMETER::~T_CLASS (); \
                DEALLOCATOR (POINTER); \
              } \
          } \
       while (0)
#     define ACE_DES_ARRAY_FREE_TEMPLATE(POINTER,SIZE,DEALLOCATOR,T_CLASS,T_PARAMETER) \
       do { \
            if (POINTER) \
              { \
                for (size_t i = 0; \
                     i < SIZE; \
                     ++i) \
                { \
                  POINTER[i].T_CLASS T_PARAMETER::~T_CLASS (); \
                } \
                DEALLOCATOR (POINTER); \
              } \
          } \
       while (0)
#     define ACE_DES_FREE_TEMPLATE2(POINTER,DEALLOCATOR,T_CLASS,T_PARAM1,T_PARAM2) \
       do { \
            if (POINTER) \
              { \
                POINTER->T_CLASS <T_PARAM1, T_PARAM2>::~T_CLASS (); \
                DEALLOCATOR (POINTER); \
              } \
          } \
       while (0)
#     define ACE_DES_FREE_TEMPLATE3(POINTER,DEALLOCATOR,T_CLASS,T_PARAM1,T_PARAM2,T_PARAM3) \
       do { \
            if (POINTER) \
              { \
                POINTER->T_CLASS <T_PARAM1, T_PARAM2, T_PARAM3>::~T_CLASS (); \
                DEALLOCATOR (POINTER); \
              } \
          } \
       while (0)
#     define ACE_DES_FREE_TEMPLATE4(POINTER,DEALLOCATOR,T_CLASS,T_PARAM1,T_PARAM2,T_PARAM3,T_PARAM4) \
       do { \
            if (POINTER) \
              { \
                POINTER->T_CLASS <T_PARAM1, T_PARAM2, T_PARAM3, T_PARAM4>::~T_CLASS (); \
                DEALLOCATOR (POINTER); \
              } \
          } \
       while (0)
#     define ACE_DES_ARRAY_FREE_TEMPLATE2(POINTER,SIZE,DEALLOCATOR,T_CLASS,T_PARAM1,T_PARAM2) \
       do { \
            if (POINTER) \
              { \
                for (size_t i = 0; \
                     i < SIZE; \
                     ++i) \
                { \
                  POINTER[i].T_CLASS <T_PARAM1, T_PARAM2>::~T_CLASS (); \
                } \
                DEALLOCATOR (POINTER); \
              } \
          } \
       while (0)
#   endif /* defined (__Lynx__) && __LYNXOS_SDK_VERSION == 199701L */
# endif /* defined ! ACE_HAS_WORKING_EXPLICIT_TEMPLATE_DESTRUCTOR */

typedef int ACE_HANDLE;
#define LPSECURITY_ATTRIBUTES int
#define ACE_INVALID_HANDLE -1
#define ACE_LIB_TEXT
#define ACE_CLASS_IS_NAMESPACE(CLASSNAME) \
private: \
CLASSNAME (void); \
CLASSNAME (const CLASSNAME&); \
friend class ace_dewarn_gplusplus


#if defined (ACE_SELECT_USES_INT) 
   typedef int ACE_FD_SET_TYPE;
#else
   typedef fd_set ACE_FD_SET_TYPE;
#endif

#if ! defined (howmany)
#  define howmany(x, y)   (((x)+((y)-1))/(y))
#endif /* howmany */

typedef ACE_HANDLE ACE_SOCKET;
typedef unsigned long ACE_SOCK_GROUP;
#define ACE_SOCKCALL_RETURN(OP,TYPE,FAILVALUE) ACE_OSCALL_RETURN(OP,TYPE,FAILVALUE)

struct ACE_OVERLAPPED
{
    unsigned long Internal;
    unsigned long InternalHigh;
    unsigned long Offset;
    unsigned long OffsetHigh;
    ACE_HANDLE hEvent;
};

#if defined (ACE_HAS_SOCKLEN_T)
#if defined (__hpux)
  /*
  ** HP-UX supplies the socklen_t type unless some feature set less than
  ** _XOPEN_SOURCE_EXTENDED is specifically requested. However, it only
  ** actually uses the socklen_t type in supplied socket functions if
  ** _XOPEN_SOURCE_EXTENDED is specifically requested. So, for example,
  ** the compile options ACE usually uses (includes -mt) cause _HPUX_SOURCE
  ** to be set, which sets _INCLUDE_XOPEN_SOURCE_EXTENDED (causing socklen_t
  ** to be defined) but _not_ _XOPEN_SOURCE_EXTENDED (causing socket functions
  ** to use int, not socklen_t). React to this situation here...
  */
#if defined (_XOPEN_SOURCE_EXTENDED)
typedef socklen_t ACE_SOCKET_LEN;
#else
typedef int ACE_SOCKET_LEN;
#endif /* _XOPEN_SOURCE_EXTENDED */
#else
typedef socklen_t ACE_SOCKET_LEN;
#endif /* __hpux */
#elif defined (ACE_HAS_SIZET_SOCKET_LEN)
typedef size_t ACE_SOCKET_LEN;
#else
typedef int ACE_SOCKET_LEN;
#endif /* ACE_HAS_SIZET_SOCKET_LEN */

typedef char ACE_TCHAR;

// Get the unicode (i.e. ACE_TCHAR) defines

// This is used to indicate that a platform doesn't support a
// particular feature.
#if defined ACE_HAS_VERBOSE_NOTSUP
  // Print a console message with the file and line number of the
  // unsupported function.
#if defined (ACE_HAS_STANDARD_CPP_LIBRARY) && (ACE_HAS_STANDARD_CPP_LIBRARY != 0)
#   include /**/ <cstdio>
#else
#include <stdio.h>
#endif
#define ACE_NOTSUP_RETURN(FAILVALUE) do { errno = ENOTSUP; fprintf (stderr, ACE_LIB_TEXT ("ACE_NOTSUP: %s, line %d\n"), __FILE__, __LINE__); return FAILVALUE; } while (0)
#define ACE_NOTSUP do { errno = ENOTSUP; fprintf (stderr, ACE_LIB_TEXT ("ACE_NOTSUP: %s, line %d\n"), __FILE__, __LINE__); return; } while (0)
#else /* ! ACE_HAS_VERBOSE_NOTSUP */
#define ACE_NOTSUP_RETURN(FAILVALUE) do { errno = ENOTSUP ; return FAILVALUE; } while (0)
#define ACE_NOTSUP do { errno = ENOTSUP; return; } while (0)
#endif /* ! ACE_HAS_VERBOSE_NOTSUP */

#if !defined (AF_ANY)
#define AF_ANY (-1)
#endif /* AF_ANY */

#if defined (ACE_HAS_STRUCT_NETDB_DATA)
   typedef char ACE_HOSTENT_DATA[sizeof(struct hostent_data)];
   typedef char ACE_SERVENT_DATA[sizeof(struct servent_data)];
   typedef char ACE_PROTOENT_DATA[sizeof(struct protoent_data)];
#else
#if !defined ACE_HOSTENT_DATA_SIZE
#define ACE_HOSTENT_DATA_SIZE (4*1024)
#endif /*ACE_HOSTENT_DATA_SIZE */
#if !defined ACE_SERVENT_DATA_SIZE
#define ACE_SERVENT_DATA_SIZE (4*1024)
#endif /*ACE_SERVENT_DATA_SIZE */
#if !defined ACE_PROTOENT_DATA_SIZE
#define ACE_PROTOENT_DATA_SIZE (2*1024)
#endif /*ACE_PROTOENT_DATA_SIZE */
#endif


#if defined (ACE_NEEDS_FUNC_DEFINITIONS)
    // It just evaporated ;-)  Not pleasant.
#define ACE_UNIMPLEMENTED_FUNC(f)
#else
#define ACE_UNIMPLEMENTED_FUNC(f) f;
#endif /* ACE_NEEDS_FUNC_DEFINITIONS */

#if defined (ACE_PSOS)
#   define ACE_SEH_TRY if (1)
#   define ACE_SEH_EXCEPT(X) while (0)
#   define ACE_SEH_FINALLY if (1)
#elif defined (ACE_WIN32)
#   if !defined (ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS)
#     define ACE_SEH_TRY if (1)
#     define ACE_SEH_EXCEPT(X) while (0)
#     define ACE_SEH_FINALLY if (1)
#   elif defined(__BORLANDC__)
#     if (__BORLANDC__ >= 0x0530) /* Borland C++ Builder 3.0 */
#       define ACE_SEH_TRY try
#       define ACE_SEH_EXCEPT(X) __except(X)
#       define ACE_SEH_FINALLY __finally
#     else
#       define ACE_SEH_TRY if (1)
#       define ACE_SEH_EXCEPT(X) while (0)
#       define ACE_SEH_FINALLY if (1)
#     endif
#   elif defined (__IBMCPP__) && (__IBMCPP__ >= 400)
#     define ACE_SEH_TRY if (1)
#     define ACE_SEH_EXCEPT(X) while (0)
#     define ACE_SEH_FINALLY if (1)
#   else
#     define ACE_SEH_TRY __try
#     define ACE_SEH_EXCEPT(X) __except(X)
#     define ACE_SEH_FINALLY __finally
#   endif /* ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS */
# else /* !ACE_WIN32 && ACE_PSOS */
#   define ACE_SEH_TRY if (1)
#   define ACE_SEH_EXCEPT(X) while (0)
#   define ACE_SEH_FINALLY if (1)
#endif /* ACE_WIN32 && ACE_PSOS */

# if defined (ACE_HAS_ALLOC_HOOKS)
#define ACE_ALLOC_HOOK_DECLARE \
  void *operator new (size_t bytes); \
  void operator delete (void *ptr);

  // Note that these are just place holders for now.  Some day they
  // may be be replaced by <ACE_Malloc>.
#define ACE_ALLOC_HOOK_DEFINE(CLASS) \
  void *CLASS::operator new (size_t bytes) { return ::new char[bytes]; } \
  void CLASS::operator delete (void *ptr) { delete [] ((char *) ptr); }
#else
#define ACE_ALLOC_HOOK_DECLARE struct __Ace {} /* Just need a dummy... */
#define ACE_ALLOC_HOOK_DEFINE(CLASS)
#endif /* ACE_HAS_ALLOC_HOOKS */

#ifdef MAXHOSTNAMELEN
#undef MAXHOSTNAMELEN
#endif
#define MAXHOSTNAMELEN 256

#if defined (ACE_HAS_POSIX_NONBLOCK)
#  define ACE_NONBLOCK O_NONBLOCK
#else
#  define ACE_NONBLOCK O_NDELAY
#endif /* ACE_HAS_POSIX_NONBLOCK */
//#define MAXPATHLEN 256
#define ACE_IOV_MAX 64
#define ACE_static_cast(TYPE, EXPR) static_cast<TYPE> (EXPR)
#define ACE_const_cast(TYPE, EXPR)  const_cast<TYPE> (EXPR)
#define ACE_reinterpret_cast(TYPE, EXPR) reinterpret_cast<TYPE> (EXPR)
#define ACE_CLOEXEC -3
#define ACE_SIGURGC -2

#define ACE_SHUTDOWN_READ  0
#define ACE_SHUTDOWN_WRITE 1


struct ACE_Protocol_Info
{
  unsigned long dwServiceFlags1;
  int iAddressFamily;
  int iProtocol;
  char szProtocol[255+1];
};

#if !defined (ACE_OSTREAM_TYPE)
#if defined (ACE_LACKS_IOSTREAM_TOTALLY)
#define ACE_OSTREAM_TYPE FILE
#else  
#include <iostream>
#define ACE_OSTREAM_TYPE std::ostream
#endif 
#endif /* ! ACE_OSTREAM_TYPE */

#define ACE_NO_HEAP_CHECK


#if !defined (ACE_DEFAULT_LOG_STREAM)
#if defined (ACE_LACKS_IOSTREAM_TOTALLY)
#define ACE_DEFAULT_LOG_STREAM 0
#else  /* ! ACE_LACKS_IOSTREAM_TOTALLY */
#define ACE_DEFAULT_LOG_STREAM (&std::cerr)
#endif /* ! ACE_LACKS_IOSTREAM_TOTALLY */
#endif /* ! ACE_DEFAULT_LOG_STREAM */

# if !defined (ACE_TEXT_WIDE)
#  if (ACE_USES_L_PREFIX == 1)
#    define ACE_TEXT_WIDE_I(STRING) L##STRING
#  else /* ACE_USES_L_PREFIX */
#    define ACE_TEXT_WIDE_I(STRING) STRING
#  endif /* ACE_USES_L_PREFIX */
#  define ACE_TEXT_WIDE(STRING) ACE_TEXT_WIDE_I (STRING)
# endif /* ACE_TEXT_WIDE */

#endif

