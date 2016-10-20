// Local_Memory_Pool.cpp,v 4.4 2005/10/28 16:14:52 ossama Exp

// Local_Memory_Pool.cpp
#include "Local_Memory_Pool.h"
#include "Auto_Ptr.h"
#include "ACE.h"


//typedef template class ACE_Auto_Basic_Array_Ptr<char>;
ACE_ALLOC_HOOK_DEFINE(ACE_Local_Memory_Pool)

void
ACE_Local_Memory_Pool::dump (void) const
{
	
}

ACE_Local_Memory_Pool::ACE_Local_Memory_Pool (const char  *,
                                              const OPTIONS *)
{
  
}

ACE_Local_Memory_Pool::~ACE_Local_Memory_Pool (void)
{
  // Free up all memory allocated by this pool.
  this->release ();
}

// Ask system for initial chunk of local memory.
void *
ACE_Local_Memory_Pool::init_acquire (size_t nbytes,
                                     size_t &rounded_bytes,
                                     int &first_time)
{
  // Note that we assume that when ACE_Local_Memory_Pool is used,
  // ACE_Malloc's constructor will only get called once.  If this
  // assumption doesn't hold, we are in deep trouble!

  first_time = 1;
  return this->acquire (nbytes, rounded_bytes);
}

void *
ACE_Local_Memory_Pool::acquire (size_t nbytes,
                                size_t &rounded_bytes)
{
  rounded_bytes = this->round_up (nbytes);

  char *temp = 0;
  ACE_NEW_RETURN (temp,
                  char[rounded_bytes],
                  0);

  ACE_Char_Auto_Basic_Array_Ptr cp(temp);
  
  if (this->allocated_chunks_.insert (cp.get ()) != 0)
    return 0;
  return cp.release ();
}

int
ACE_Local_Memory_Pool::release (int)
{

  // Zap the memory we allocated.
  for (ACE_Unbounded_Set<char *>::iterator i = this->allocated_chunks_.begin ();
//  for (ACE_Char_Unbounded_Set::iterator i = this->allocated_chunks_.begin ();
       i != this->allocated_chunks_.end ();
       ++i)
    delete [] *i;
  this->allocated_chunks_.reset ();
  return 0;
}

int
ACE_Local_Memory_Pool::sync (ssize_t, int)
{
  return 0;
}

int
ACE_Local_Memory_Pool::sync (void *, size_t, int)
{
  return 0;
}

int
ACE_Local_Memory_Pool::protect (ssize_t, int)
{
  return 0;
}

int
ACE_Local_Memory_Pool::protect (void *, size_t, int)
{
  return 0;
}

#if defined (ACE_WIN32)
int
ACE_Local_Memory_Pool::seh_selector (void *)
{
  return 0;
  // Continue propagate the structural exception up.
}
#endif /* ACE_WIN32 */

int
ACE_Local_Memory_Pool::remap (void *)
{
  return 0;
  // Not much can be done.
}

void *
ACE_Local_Memory_Pool::base_addr (void) const
{
  return 0;
}

// Let the underlying new operator figure out the alignment...
size_t
ACE_Local_Memory_Pool::round_up (size_t nbytes)
{
  return ACE::round_to_pagesize (static_cast<off_t> (nbytes));
}

