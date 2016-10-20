// Log_Record.cpp,v 4.68 2003/11/10 01:48:02 dhinton Exp

#include "Log_Record.h"


#include "Log_Msg.h"
#include "ACE.h"
#include <stdio.h>
#include <math.h>

#include <sys/time.h>
#if defined(OS_HPUX) || defined(DEF_HPUX)
#include <time.h>
#endif

#if !defined (ACE_LACKS_IOSTREAM_TOTALLY)
// FUZZ: disable check_for_streams_include
# include <iostream>
#endif /* ! ACE_LACKS_IOSTREAM_TOTALLY */


ACE_ALLOC_HOOK_DEFINE(ACE_Log_Record)

const char *ACE_Log_Record::priority_names_[] =
{
  ACE_LIB_TEXT ("<none>"),
  ACE_LIB_TEXT ("LM_SHUTDOWN"),
  ACE_LIB_TEXT ("LM_TRACE"),
  ACE_LIB_TEXT ("LM_DEBUG"),
  ACE_LIB_TEXT ("LM_INFO"),
  ACE_LIB_TEXT ("LM_NOTICE"),
  ACE_LIB_TEXT ("LM_WARNING"),
  ACE_LIB_TEXT ("LM_STARTUP"),
  ACE_LIB_TEXT ("LM_ERROR"),
  ACE_LIB_TEXT ("LM_CRITICAL"),
  ACE_LIB_TEXT ("LM_ALERT"),
  ACE_LIB_TEXT ("LM_EMERGENCY"),
  ACE_LIB_TEXT ("LM_UNK(04000)"),
  ACE_LIB_TEXT ("LM_UNK(010000)"),
  ACE_LIB_TEXT ("LM_UNK(020000)"),
  ACE_LIB_TEXT ("LM_UNK(040000)"),
  ACE_LIB_TEXT ("LM_UNK(0100000)"),
  ACE_LIB_TEXT ("LM_UNK(0200000)"),
  ACE_LIB_TEXT ("LM_UNK(0400000)"),
  ACE_LIB_TEXT ("LM_UNK(01000000)"),
  ACE_LIB_TEXT ("LM_UNK(02000000)"),
  ACE_LIB_TEXT ("LM_UNK(04000000)"),
  ACE_LIB_TEXT ("LM_UNK(010000000)"),
  ACE_LIB_TEXT ("LM_UNK(020000000)"),
  ACE_LIB_TEXT ("LM_UNK(040000000)"),
  ACE_LIB_TEXT ("LM_UNK(0100000000)"),
  ACE_LIB_TEXT ("LM_UNK(0200000000)"),
  ACE_LIB_TEXT ("LM_UNK(0400000000)"),
  ACE_LIB_TEXT ("LM_UNK(01000000000)"),
  ACE_LIB_TEXT ("LM_UNK(02000000000)"),
  ACE_LIB_TEXT ("LM_UNK(04000000000)"),
  ACE_LIB_TEXT ("LM_UNK(010000000000)"),
  ACE_LIB_TEXT ("LM_UNK(020000000000)")
};

const char *
ACE_Log_Record::priority_name (ACE_Log_Priority p)
{
  return ACE_Log_Record::priority_names_[ACE::log5 (p)];
}

void
ACE_Log_Record::priority_name (ACE_Log_Priority p,
                               const char *name)
{
  // Name must be a statically allocated string
  ACE_Log_Record::priority_names_[ACE::log5 (p)] = name;
}

u_long
ACE_Log_Record::priority (void) const
{
  ACE_TRACE ("ACE_Log_Record::priority");

  // Get the priority of the <Log_Record> <type_>.  This is computed
  // as the base 2 logarithm of <type_> (which must be a power of 2,
  // as defined by the enums in <ACE_Log_Priority>).
  return ACE::log5 ((u_long) this->type_);
}

void
ACE_Log_Record::priority (u_long p)
{
  ACE_TRACE ("ACE_Log_Record::priority");

  // Set the priority of the <Log_Record> <type_> (which must be a
  // power of 2, as defined by the enums in <ACE_Log_Priority>).
  this->type_ = (ACE_UINT32) p;
}

void
ACE_Log_Record::dump (void) const
{
#if defined (ACE_HAS_DUMP)
  // ACE_TRACE ("ACE_Log_Record::dump");

  ACE_DEBUG ((LM_DEBUG, ACE_BEGIN_DUMP, this));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("length_ = %d\n"), this->length_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\ntype_ = %u\n"), this->type_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\ntime_stamp_ = (%d, %d)\n"), this->secs_, this->usecs_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\npid_ = %u\n"), this->pid_));
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("\nmsg_data_ = %s\n"), this->msg_data_));
  ACE_DEBUG ((LM_DEBUG, ACE_END_DUMP));
#endif /* ACE_HAS_DUMP */
}

void
ACE_Log_Record::msg_data (const char *data)
{
  // ACE_TRACE ("ACE_Log_Record::msg_data");
  strncpy (this->msg_data_, data,
                    (sizeof this->msg_data_ / sizeof (char)));
  this->round_up ();
}

ACE_Log_Record::ACE_Log_Record (ACE_Log_Priority lp,
                                long ts_sec,
                                long p)
  : length_ (0),
    type_ (ACE_UINT32 (lp)),
    secs_ (ts_sec),
    usecs_ (0),
    pid_ (ACE_UINT32 (p))
{
  // ACE_TRACE ("ACE_Log_Record::ACE_Log_Record");
}

ACE_Log_Record::ACE_Log_Record (ACE_Log_Priority lp,
                                const ACE_Time_Value &ts,
                                long p)
  : length_ (0),
    type_ (ACE_UINT32 (lp)),
    secs_ ((ACE_UINT32) ts.sec ()),
    usecs_ ((ACE_UINT32) ts.usec ()),
    pid_ (ACE_UINT32 (p))
{
   ACE_TRACE ("ACE_Log_Record::ACE_Log_Record");
}

void
ACE_Log_Record::round_up (void)
{
  // ACE_TRACE ("ACE_Log_Record::round_up");
  // Determine the length of the payload.
  size_t len = (sizeof (*this) - sizeof (this->msg_data_))
    + (sizeof (char) * ((strlen (this->msg_data_) + 1)));

  // Round up to the alignment.
  len = ((len + ACE_Log_Record::ALIGN_WORDB - 1)
         & ~(ACE_Log_Record::ALIGN_WORDB - 1));
  this->length_ = ACE_static_cast (ACE_UINT32, len);
}

ACE_Log_Record::ACE_Log_Record (void)
  : length_ (0),
    type_ (0),
    secs_ (0),
    usecs_ (0),
    pid_ (0)
{
  // ACE_TRACE ("ACE_Log_Record::ACE_Log_Record");
}

int
ACE_Log_Record::format_msg (const char host_name[],
                            u_long verbose_flag,
                            char *verbose_msg)
{
  /* 0123456789012345678901234     */
  /* Oct 18 14:25:36.000 1989<nul> */
  char timestamp[26]; // Only used by VERBOSE and VERBOSE_LITE.

  if (ACE_BIT_ENABLED (verbose_flag,
                       ACE_Log_Msg::VERBOSE)
      || ACE_BIT_ENABLED (verbose_flag,
                          ACE_Log_Msg::VERBOSE_LITE))
    {
      time_t now = this->secs_;
      char ctp[26]; // 26 is a magic number...

#if defined(OS_HPUX) || defined(DEF_HPUX)
      char *p_ctp = NULL;
      if ((p_ctp = ctime (&now)) == 0)
        return -1;
      strcpy(ctp,p_ctp);
#elif defined(DEF_SOLARIS)
      if (ctime_r (&now, ctp, strlen(ctp)) == 0)
#else
	  if (ctime_r (&now, ctp) == 0)
        return -1;
#endif

      /* 01234567890123456789012345 */
      /* Wed Oct 18 14:25:36 1989n0 */

      ctp[19] = '\0'; // NUL-terminate after the time.
      ctp[24] = '\0'; // NUL-terminate after the date.

      sprintf (timestamp,
                       ACE_LIB_TEXT ("%s.%03ld %s"),
                       ctp + 4,
                       ((long) this->usecs_) / 1000,
                       ctp + 20);
    }

  if (ACE_BIT_ENABLED (verbose_flag,
                       ACE_Log_Msg::VERBOSE))
    {
# if defined (ACE_HAS_BROKEN_CONDITIONAL_STRING_CASTS)
      const char *lhost_name =  (const char *) ((host_name == 0)
                                                            ? ((char *) ACE_LIB_TEXT ("<local_host>"))
                                                            : ((char *) host_name));
# else /* ! defined (ACE_HAS_BROKEN_CONDITIONAL_STRING_CASTS) */
      const char *lhost_name = ((host_name == 0)
                                      ? ACE_LIB_TEXT ("<local_host>")
                                      : host_name);
# endif /* ! defined (ACE_HAS_BROKEN_CONDITIONAL_STRING_CASTS) */
      sprintf (verbose_msg,
                       ACE_LIB_TEXT ("%s@%s@%u@%s@%s"),
                       timestamp,
                       lhost_name,
                       this->pid_,
                       ACE_Log_Record::priority_name (ACE_Log_Priority (this->type_)),
                       this->msg_data_);
    }
  else if (ACE_BIT_ENABLED (verbose_flag, ACE_Log_Msg::VERBOSE_LITE))
    sprintf (verbose_msg,
                     ACE_LIB_TEXT ("%s@%s@%s"),
                     timestamp,
                     ACE_Log_Record::priority_name (ACE_Log_Priority (this->type_)),
                     this->msg_data_);
  else
    sprintf (verbose_msg,
                     ACE_LIB_TEXT ("%s"),
                     this->msg_data_);
  return 0;
}

int
ACE_Log_Record::print (const char host_name[],
                       u_long verbose_flag,
                       FILE *fp)
{
  char verbose_msg [MAXVERBOSELOGMSGLEN];
  int result = this->format_msg (host_name,
                                 verbose_flag,
                                 verbose_msg);

  if (result == 0)
    {
      if (fp != 0)
        {
          int verbose_msg_len = ACE_static_cast (int,
                                                 strlen (verbose_msg));
          int fwrite_result = fprintf (fp, ACE_LIB_TEXT ("%s"), verbose_msg);

          // We should have written everything
          if (fwrite_result != verbose_msg_len)
            result = -1;
          else
            fflush (fp);
        }
    }

  return result;
}

#if !defined (ACE_LACKS_IOSTREAM_TOTALLY)

int
ACE_Log_Record::print (const char host_name[],
                       u_long verbose_flag,
                       ACE_OSTREAM_TYPE &s)
{
  char verbose_msg [MAXVERBOSELOGMSGLEN];
  int result = this->format_msg (host_name, verbose_flag, verbose_msg);

  if (result == 0)
    {
      // Since ostream expects only chars, we cannot pass wchar_t's
      s << verbose_msg;
      s.flush ();
    }

  return result;
}

#endif /* ! ACE_LACKS_IOSTREAM_TOTALLY */
