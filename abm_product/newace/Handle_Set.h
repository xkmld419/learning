/* -*- C++ -*- */

//=============================================================================
/**
 *  @file    Handle_Set.h
 *
 *  Handle_Set.h,v 4.33 2003/07/19 19:04:11 dhinton Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//=============================================================================

#ifndef ACE_HANDLE_SET_H
#define ACE_HANDLE_SET_H
//#include <sys/select.h>
#include <limits.h>
#include "Log_Msg.h"
#include "Trace.h"
#if defined(DEF_SOLARIS)
	#include <string.h>
#endif

// Default size of the ACE Reactor.
#if defined (FD_SETSIZE)
   int const ACE_FD_SETSIZE = FD_SETSIZE;
#else /* !FD_SETSIZE */
#  define ACE_FD_SETSIZE FD_SETSIZE
#endif /* ACE_FD_SETSIZE */

#if !defined (ACE_DEFAULT_SELECT_REACTOR_SIZE)
#  define ACE_DEFAULT_SELECT_REACTOR_SIZE ACE_FD_SETSIZE
#endif /* ACE_DEFAULT_SELECT_REACTOR_SIZE */

/**
 * @class ACE_Handle_Set
 *
 * @brief C++ wrapper facade for the socket <fd_set> abstraction.
 *
 * This abstraction is a very efficient wrapper facade over
 * <fd_set>.  In particular, no range checking is performed, so
 * it's important not to set or clear bits that are outside the
 * <ACE_DEFAULT_SELECT_REACTOR_SIZE>.
 */
class ACE_Handle_Set
{
public:
  friend class ACE_Handle_Set_Iterator;

  // = Initialization and termination.

  enum
  {
    MAXSIZE = ACE_DEFAULT_SELECT_REACTOR_SIZE
  };

// Initialize the bitmask to all 0s and reset the associated fields.

  inline void reset (void)
  {
    ACE_TRACE ("reset");
    this->max_handle_ =
      ACE_INVALID_HANDLE;
  #if defined (ACE_HAS_BIG_FD_SET)
    this->min_handle_ =
      NUM_WORDS * WORDSIZE;
  #endif /* ACE_HAS_BIG_FD_SET */
    this->size_ = 0;
    // #if !defined (ACE_HAS_BIG_FD_SET)      Why is this here?  -Steve Huston
    FD_ZERO (&this->mask_);
    // #endif /* ACE_HAS_BIG_FD_SET */
  }
  
  #if defined (ACE_HAS_BIG_FD_SET)
  inline ACE_Handle_Set &  operator = (const ACE_Handle_Set &rhs)
  {
    ACE_TRACE ("operator =");
  
    if (rhs.size_ > 0)
      {
        this->size_ =
          rhs.size_;
        this->max_handle_ =
          rhs.max_handle_;
        this->min_handle_ =
          rhs.min_handle_;
        this->mask_ =
          rhs.mask_;
      }
    else
      this->reset ();
  
    return *this;
  }
  #endif /* ACE_HAS_BIG_FD_SET */
  
  // Returns the number of the large bit.
  
  inline ACE_HANDLE max_set (void) const
  {
    ACE_TRACE ("max_set");
    return this->max_handle_;
  }
  
  // Checks whether handle is enabled.
  
  inline int is_set (ACE_HANDLE handle) const
  {
    ACE_TRACE ("is_set");
  #if defined (ACE_HAS_BIG_FD_SET)
    return FD_ISSET (handle,
                     &this->mask_)
      && this->size_ > 0;
  #else
    return FD_ISSET (handle,
                     &this->mask_);
  #endif /* ACE_HAS_BIG_FD_SET */
  }
  
  // Enables the handle.
  
  inline void set_bit (ACE_HANDLE handle)
  {
    ACE_TRACE ("set_bit");
    if ((handle != ACE_INVALID_HANDLE)
        && (!this->is_set (handle)))
      {
  #if defined (ACE_WIN32)
        FD_SET ((SOCKET) handle,
                &this->mask_);
        this->size_++;
  #else /* ACE_WIN32 */
  #if defined (ACE_HAS_BIG_FD_SET)
        if (this->size_ == 0)
          FD_ZERO (&this->mask_);
  
        if (handle < this->min_handle_)
          this->min_handle_ = handle;
  #endif /* ACE_HAS_BIG_FD_SET */
  
        FD_SET (handle,
                &this->mask_);
        this->size_++;
  
        if (handle > this->max_handle_)
          this->max_handle_ = handle;
  #endif /* ACE_WIN32 */
      }
  }
  
  // Disables the handle.
  
  inline void clr_bit (ACE_HANDLE handle)
  {
    ACE_TRACE ("clr_bit");
  
    if ((handle != ACE_INVALID_HANDLE) &&
        (this->is_set (handle)))
      {
        FD_CLR ((ACE_SOCKET) handle,
                &this->mask_);
        this->size_--;
  
  #if !defined (ACE_WIN32)
        if (handle == this->max_handle_)
          this->set_max (this->max_handle_);
  #endif /* !ACE_WIN32 */
      }
  }
  
  // Returns a count of the number of enabled bits.
  
  inline int num_set (void) const
  {
    ACE_TRACE ("num_set");
  #if defined (ACE_WIN32)
    return this->mask_.fd_count;
  #else /* !ACE_WIN32 */
    return this->size_;
  #endif /* ACE_WIN32 */
  }
  
  // Returns a pointer to the underlying fd_set.
  
  inline
  operator fd_set *()
  {
    ACE_TRACE ("operator fd_set *");
  
    if (this->size_ > 0)
      return (fd_set *) &this->mask_;
    else
      return (fd_set *) 0;
  }
  
  // Returns a pointer to the underlying fd_set.
  
  inline fd_set *
  fdset (void)
  {
    ACE_TRACE ("fdset");
  
    if (this->size_ > 0)
      return (fd_set *) &this->mask_;
    else
      return (fd_set *) 0;
  }
    // = Initialization methods.
  /// Constructor, initializes the bitmask to all 0s.
  ACE_Handle_Set (void);

  /**
   * Constructor, initializes the handle set from a given mask.
   * <ACE_FD_SET_TYPE> is a <typedef> based on the platform's native
   * type used for masks passed to <select>.
   */
  ACE_Handle_Set (const ACE_FD_SET_TYPE &mask);


  /**
   * Rescan the underlying <fd_set> up to handle <max> to find the new
   * <max_handle> (highest bit set) and <size> (how many bits set) values.
   * This is useful for evaluating the changes after the handle set has
   * been manipulated in some way other than member functions; for example,
   * after <select> modifies the <fd_set>.
   */
  void sync (ACE_HANDLE max);



  /// Dump the state of an object.
  void dump (void) const;

  /// Declare the dynamic allocation hooks.
  ACE_ALLOC_HOOK_DECLARE;

private:
  /// Size of the set, i.e., a count of the number of enabled bits.
  int size_;

  /// Current max handle.
  ACE_HANDLE max_handle_;

#if defined (ACE_HAS_BIG_FD_SET)
  /// Current min handle.
  ACE_HANDLE min_handle_;
#endif /* ACE_HAS_BIG_FD_SET */

  /// Bitmask.
  fd_set mask_;

  enum
  {
    WORDSIZE = NFDBITS,
#if !defined (ACE_WIN32)
    NUM_WORDS = howmany (MAXSIZE, NFDBITS),
#endif /* ACE_WIN32 */
    NBITS = 256
  };

  /// Counts the number of bits enabled in N.  Uses a table lookup to
  /// speed up the count.
  static int count_bits (u_long n);

#if defined (ACE_HAS_BIG_FD_SET)
  /// Find the position of the bit counting from right to left.
  static int bitpos (u_long bit);
#endif /* ACE_HAS_BIG_FD_SET */

  /// Resets the <max_handle_> after a clear of the original
  /// <max_handle_>.
  void set_max (ACE_HANDLE max);

  /// Table that maps bytes to counts of the enabled bits in each value
  /// from 0 to 255.
  static const char nbits_[NBITS];
};

/**
 * @class ACE_Handle_Set_Iterator
 *
 * @brief Iterator for the <ACE_Handle_Set> abstraction.
 */
class ACE_Handle_Set_Iterator
{
public:
  /// Constructor.
  ACE_Handle_Set_Iterator (const ACE_Handle_Set &hs);

  inline 
  ~ACE_Handle_Set_Iterator (void)
  {
  }

  /// Reset the state of the iterator by reinitializing the state
  /// that we maintain.
  void reset_state (void);

  /**
   * "Next" operator.  Returns the next unseen <ACE_HANDLE> in the
   * <Handle_Set> up to <handle_set_.max_handle_>).  When all the
   * handles have been seen returns <ACE_INVALID_HANDLE>.  Advances
   * the iterator automatically, so you need not call <operator++>
   * (which is now obsolete).
   */
  ACE_HANDLE operator () (void);

  /// This is a no-op and no longer does anything.  It's only here for
  /// backwards compatibility.
  void operator++ (void);

  /// Dump the state of an object.
  void dump (void) const;

  /// Declare the dynamic allocation hooks.
  ACE_ALLOC_HOOK_DECLARE;

private:
  /// The <Handle_Set> we are iterating through.
  const ACE_Handle_Set &handles_;

  /// Index of the bit we're examining in the current <word_num_> word.
#if defined (ACE_WIN32)
  u_int handle_index_;
#elif !defined (ACE_HAS_BIG_FD_SET)
  int handle_index_;
#elif defined (ACE_HAS_BIG_FD_SET)
  int handle_index_;
  u_long oldlsb_;
#endif /* ACE_WIN32 */

  /// Number of the word we're iterating over (typically between 0..7).
  int word_num_;

#if defined (ACE_HAS_BIG_FD_SET)
  /// Number max of the words with a possible bit on.
  int word_max_;
#endif /* ACE_HAS_BIG_FD_SET */

#if !defined (ACE_WIN32) && !defined (ACE_HAS_BIG_FD_SET)
  /// Value of the bits in the word we're iterating on.
  fd_mask word_val_;
#elif !defined (ACE_WIN32) && defined (ACE_HAS_BIG_FD_SET)
  /// Value of the bits in the word we're iterating on.
  u_long word_val_;
#endif /* !ACE_WIN32 && !ACE_HAS_BIG_FD_SET */
};

#endif /* ACE_HANDLE_SET */
