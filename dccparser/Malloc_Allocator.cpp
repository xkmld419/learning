// Malloc_Allocator.cpp,v 4.13 2005/10/28 16:14:53 ossama Exp

#include "Malloc_Allocator.h"

#include <string.h>

ACE_Allocator *
ACE_Allocator::instance (void)
{
  //  ACE_TRACE ("ACE_Allocator::instance");

  if (ACE_Allocator::allocator_ == 0)
  {
      // Perform Double-Checked Locking Optimization.
          static void *allocator_instance = 0;

          // Check this critical assumption.  We put it in a variable
          // first to avoid stupid compiler warnings that the
          // condition may always be true/false.

         ACE_Allocator::allocator_ = new (&allocator_instance) ACE_New_Allocator;
  }

  return ACE_Allocator::allocator_;
}

ACE_Allocator *
ACE_Allocator::instance (ACE_Allocator *r)
{
  ACE_Allocator *t = ACE_Allocator::allocator_;

  // We can't safely delete it since we don't know who created it!
  ACE_Allocator::delete_allocator_ = 0;

  ACE_Allocator::allocator_ = r;
  return t;
}

void
ACE_Allocator::close_singleton (void)
{
  if (ACE_Allocator::delete_allocator_)
    {
      // This should never be executed....  See the
      // ACE_Allocator::instance (void) method for an explanation.
      delete ACE_Allocator::allocator_;
      ACE_Allocator::allocator_ = 0;
      ACE_Allocator::delete_allocator_ = 0;
    }
}

ACE_Allocator::~ACE_Allocator (void)
{
}

ACE_Allocator::ACE_Allocator (void)
{
}

/******************************************************************************/

void *
ACE_New_Allocator::malloc (size_t nbytes)
{
  char *ptr = 0;

  if (nbytes > 0)
    ACE_NEW_RETURN (ptr, char[nbytes], 0);
  return (void *) ptr;
}

void *
ACE_New_Allocator::calloc (size_t nbytes,
                           char initial_value)
{
  char *ptr = 0;

  ACE_NEW_RETURN (ptr, char[nbytes], 0);

  memset (ptr, initial_value, nbytes);
  return (void *) ptr;
}

void *
ACE_New_Allocator::calloc (size_t n_elem, size_t elem_size, char initial_value)
{
  return ACE_New_Allocator::calloc (n_elem * elem_size, initial_value);
}

void
ACE_New_Allocator::free (void *ptr)
{
  delete [] (char *) ptr;
}

int
ACE_New_Allocator::remove (void)
{
  return (-1);
}

int
ACE_New_Allocator::bind (const char *, void *, int)
{
  return (-1);
}

int
ACE_New_Allocator::trybind (const char *, void *&)
{
  return (-1);
}

int
ACE_New_Allocator::find (const char *, void *&)
{
  return (-1);
}

int
ACE_New_Allocator::find (const char *)
{
  return (-1);
}

int
ACE_New_Allocator::unbind (const char *)
{
  return (-1);
}

int
ACE_New_Allocator::unbind (const char *, void *&)
{
  return (-1);
}

int
ACE_New_Allocator::sync (ssize_t, int)
{
  return (-1);
}

int
ACE_New_Allocator::sync (void *, size_t, int)
{
  return (-1);
}

int
ACE_New_Allocator::protect (ssize_t, int)
{
  return (-1);
}

int
ACE_New_Allocator::protect (void *, size_t, int)
{
  return (-1);
}

#if defined (ACE_HAS_MALLOC_STATS)
void
ACE_New_Allocator::print_stats (void) const
{
}
#endif /* ACE_HAS_MALLOC_STATS */

void
ACE_New_Allocator::dump (void) const
{
#if defined (ACE_HAS_DUMP)
#endif /* ACE_HAS_DUMP */
}

/******************************************************************************/

void *
ACE_Static_Allocator_Base::malloc (size_t nbytes)
{
  if (this->offset_ + nbytes > this->size_)
    {
      errno = ENOMEM;
      return 0;
    }
  else
    {
      // Record the current offset, increment the offset by the number
      // of bytes requested, and return the original offset.
      char *ptr = &this->buffer_[this->offset_];
      this->offset_ += nbytes;
      return (void *) ptr;
    }
}

void *
ACE_Static_Allocator_Base::calloc (size_t nbytes,
                                   char initial_value)
{
  void *ptr = this->malloc (nbytes);

  memset (ptr, initial_value, nbytes);
  return (void *) ptr;
}

void *
ACE_Static_Allocator_Base::calloc (size_t n_elem,
                                   size_t elem_size,
                                   char initial_value)
{
  return this->calloc (n_elem * elem_size, initial_value);
}

void
ACE_Static_Allocator_Base::free (void *ptr)
{
  // Check to see if ptr is within our pool?!
  ACE_UNUSED_ARG (ptr);
}

int
ACE_Static_Allocator_Base::remove (void)
{
  return -1;
}

int
ACE_Static_Allocator_Base::bind (const char *, void *, int)
{
  return -1;
}

int
ACE_Static_Allocator_Base::trybind (const char *, void *&)
{
  return -1;
}

int
ACE_Static_Allocator_Base::find (const char *, void *&)
{
  return -1;
}

int
ACE_Static_Allocator_Base::find (const char *)
{
  return -1;
}

int
ACE_Static_Allocator_Base::unbind (const char *)
{
  return -1;
}

int
ACE_Static_Allocator_Base::unbind (const char *, void *&)
{
  return -1;
}

int
ACE_Static_Allocator_Base::sync (ssize_t, int)
{
  return -1;
}

int
ACE_Static_Allocator_Base::sync (void *, size_t, int)
{
  return -1;
}

int
ACE_Static_Allocator_Base::protect (ssize_t, int)
{
  return -1;
}

int
ACE_Static_Allocator_Base::protect (void *, size_t, int)
{
  return -1;
}

#if defined (ACE_HAS_MALLOC_STATS)
void
ACE_Static_Allocator_Base::print_stats (void) const
{
}
#endif /* ACE_HAS_MALLOC_STATS */

void
ACE_Static_Allocator_Base::dump (void) const
{

}

