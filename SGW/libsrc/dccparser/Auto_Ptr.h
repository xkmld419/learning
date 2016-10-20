/* -*- C++ -*- */

//=============================================================================
/**
 *  @file    Auto_Ptr.h
 *
 *  Auto_Ptr.h,v 4.41 2006/01/11 19:54:41 ossama Exp
 *
 *  @author Doug Schmidt <schmidt@uci.edu>
 *  @author Irfan Pyarali <irfan@cs.wustl.edu>
 *  @author Jack Reeves <jack@fx.com>
 *  @author Dr. Harald M. Mueller <mueller@garwein.hai.siemens.co.at>
 */
//=============================================================================

#ifndef ACE_AUTO_PTR_H
#define ACE_AUTO_PTR_H

#include "macros.h"
/**
 * @class ACE_Auto_Basic_Ptr
 *
 * @brief Implements the draft C++ standard auto_ptr abstraction.
 * This class allows one to work on non-object (basic) types
 */
template <class X>
class ACE_Auto_Basic_Ptr
{
public:
  // = Initialization and termination methods
/*  explicit ACE_Auto_Basic_Ptr (X *p = 0) : p_ (p) {}

  ACE_Auto_Basic_Ptr (ACE_Auto_Basic_Ptr<X> &ap);
  ACE_Auto_Basic_Ptr<X> &operator= (ACE_Auto_Basic_Ptr<X> &rhs);
  ~ACE_Auto_Basic_Ptr (void);

  // = Accessor methods.
  X &operator *() const;
  X *get (void) const;
  X *release (void);
  void reset (X *p = 0);
*/
  /// Dump the state of an object.

  ACE_ALLOC_HOOK_DECLARE; /// Declare the dynamic allocation hooks.
//  void *operator new (size_t bytes); 
//  void operator delete (void *ptr);


  inline void dump (void) const
  {
  }

   inline ACE_Auto_Basic_Ptr (ACE_Auto_Basic_Ptr<X> &rhs)
     : p_ (rhs.release ())
   {
   }

   inline X * get (void) const
   {
    return this->p_;
   }

  inline X *
  release (void)
  {
    X *old = this->p_;
    this->p_ = 0;
    return old;
  }

  inline void
  reset (X *p)
  {
    if (this->get () != p)
      delete this->get ();
    this->p_ = p;
  }

  inline ACE_Auto_Basic_Ptr<X> &
  operator= (ACE_Auto_Basic_Ptr<X> &rhs)
  {
    if (this != &rhs)
    {
      this->reset (rhs.release ());
    }
    return *this;
  }

  inline ~ACE_Auto_Basic_Ptr (void)
  {
    delete this->get ();
  }

  inline X &
    operator *() const
  {
    return *this->get ();
  }

  protected:
    X *p_;
};



/**
 * @class auto_ptr
 *
 * @brief Implements the draft C++ standard auto_ptr abstraction.
 */
template <class X>
class auto_ptr : public ACE_Auto_Basic_Ptr <X>
{
public:
  // = Initialization and termination methods
  explicit auto_ptr (X *p = 0) : ACE_Auto_Basic_Ptr<X> (p) {}
  auto_ptr (auto_ptr<X> &ap) : ACE_Auto_Basic_Ptr<X> (ap.release()) {}

#if !defined(OS_HPUX) && !defined(DEF_HPUX) && !defined(__BORLANDC__) 
  X *operator-> () const;
#endif
  inline  X *operator-> () const
  {
    return this->get ();
  }

};



/**
 * @brief Implements the draft C++ standard auto_ptr abstraction.
 * This version can be used instead of auto_ptr<T>, and obviates
 * the need for the ACE_AUTO_PTR_RESET macro on platforms like
 * VC6 where the auto_ptr<T> is broken.
 */
template <class X>
class ACE_Auto_Ptr : public ACE_Auto_Basic_Ptr <X>
{
public:
  // = Initialization and termination methods
  explicit ACE_Auto_Ptr (X *p = 0) : ACE_Auto_Basic_Ptr<X> (p) {}
  inline X *operator-> () const
  {
    return this->get ();
  }

#if !defined(OS_HPUX) && !defined(DEF_HPUX) && !defined(__BORLANDC__) 
  X *operator-> () const;
#endif

};

/**
 * @class ACE_Auto_Basic_Array_Ptr
 *
 * @brief Implements an extension to the draft C++ standard auto_ptr
 * abstraction.  This class allows one to work on non-object
 * (basic) types that must be treated as an array, e.g.,
 * deallocated via "delete [] foo".
 */
template<class X>
class ACE_Auto_Basic_Array_Ptr
{
public:
  // = Initialization and termination methods.
  explicit ACE_Auto_Basic_Array_Ptr (X *p = 0) : p_ (p) {}

//  ACE_Auto_Basic_Array_Ptr (ACE_Auto_Basic_Array_Ptr<X> &ap);
//  ACE_Auto_Basic_Array_Ptr<X> &operator= (ACE_Auto_Basic_Array_Ptr<X> &rhs);
//  ~ACE_Auto_Basic_Array_Ptr (void);

  // = Accessor methods.
//  X &operator* () const;
//  X &operator[] (int i) const;

  /// Dump the state of an object.
  void dump (void) const;

  /// Declare the dynamic allocation hooks.
  ACE_ALLOC_HOOK_DECLARE;
  
  inline X *
  get (void) const
  {
    return this->p_;
  }

  inline X *release (void)
  {
    X *old = this->p_;
    this->p_ = 0;
    return old;
  }

  inline void
  reset (X *p)
  {
    if (this->get () != p)
      delete [] this->get ();
    this->p_ = p;
  }

  inline ACE_Auto_Basic_Array_Ptr (ACE_Auto_Basic_Array_Ptr<X> &rhs)
    : p_ (rhs.release ())
  {
  }

  inline ACE_Auto_Basic_Array_Ptr<X> &
    operator= (ACE_Auto_Basic_Array_Ptr<X> &rhs)
  {
    if (this != &rhs)
    {
      this->reset (rhs.release ());
    }
    return *this;
  }

  inline
  ~ACE_Auto_Basic_Array_Ptr (void)
  {
    delete [] this->get ();
  }

  inline X &
  operator *() const
  {
    return *this->get ();
  }

  inline X &
  operator[](int i) const
  {
    X *array = this->get ();
    return array[i];
  }

protected:
  X *p_;
};

/**
 * @class ACE_Auto_Array_Ptr
 *
 * @brief Implements an extension to the draft C++ standard auto_ptr
 * abstraction.
 */
template<class X>
class ACE_Auto_Array_Ptr : public ACE_Auto_Basic_Array_Ptr<X>
{
public:
  // = Initialization and termination methods.
  explicit ACE_Auto_Array_Ptr (X *p = 0)
    : ACE_Auto_Basic_Array_Ptr<X> (p) {}

  inline X *operator->() const
  {
    return this->get ();
  }
};

typedef ACE_Auto_Basic_Array_Ptr<char> ACE_Char_Auto_Basic_Array_Ptr;

// Some platforms have an older version of auto_ptr
// support, which lacks reset, and cannot be disabled
// easily.  Portability to these platforms requires
// use of the following ACE_AUTO_PTR_RESET macro.
//
// Note that this macro correctly handles the case where NEWPTR may be
// a call to operator new(), e.g. "new foo", by making sure it is only
// evaluated once.
#if defined (ACE_AUTO_PTR_LACKS_RESET)
#define ACE_AUTO_PTR_RESET(AUTOPTR,NEWPTR,TYPE) \
      do { \
        TYPE * tmp_ptr = NEWPTR; \
        if (tmp_ptr != AUTOPTR.get ()) \
          { \
            delete AUTOPTR.release (); \
            AUTOPTR = auto_ptr<TYPE> (tmp_ptr); \
          } \
      } while (0)
#else /* ! ACE_AUTO_PTR_LACKS_RESET */
#define ACE_AUTO_PTR_RESET(AUTOPTR,NEWPTR,TYPE) \
      do { \
         AUTOPTR.reset (NEWPTR); \
      } while (0)
#endif /* ACE_AUTO_PTR_LACKS_RESET */


#endif /* ACE_AUTO_PTR_H */
