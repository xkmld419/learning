#include "Malloc_T.h"
#include "ACE.h"
#include "Log_Msg.h"
#include <string.h>


ACE_ALLOC_HOOK_DEFINE (ACE_Malloc_T)


void
ACE_Malloc_T::dump (void) const
{
#if defined (ACE_HAS_DUMP)
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::dump");

  ACE_DEBUG ((LM_DEBUG, ACE_BEGIN_DUMP, this));
  this->memory_pool_.dump ();
  ACE_DEBUG ((LM_DEBUG, ACE_LIB_TEXT ("cb_ptr_ = %@\n"), this->cb_ptr_));
  this->cb_ptr_->dump ();
#if defined (ACE_HAS_MALLOC_STATS)
  if (this->cb_ptr_ != 0)
    this->cb_ptr_->malloc_stats_.dump ();
#endif /* ACE_HAS_MALLOC_STATS */
  ACE_DEBUG ((LM_DEBUG, ACE_END_DUMP));
#endif /* ACE_HAS_DUMP */
}

void ACE_Malloc_T::print_stats (void) const
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::print_stats");

  if (this->cb_ptr_ == 0)
    return;

  ACE_TRACE ("contents of freelist:\n");

  for (MALLOC_HEADER *currp = this->cb_ptr_->freep_->next_block_;
       ;
       currp = currp->next_block_)
    {
      ACE_TRACE ((LM_DEBUG,
                  ACE_LIB_TEXT ("(%P|%t) ptr = %@, MALLOC_HEADER units = %d, byte units = %d\n"),
                  currp,
                  currp->size_,
                  currp->size_ * sizeof (ACE_Control_Block::ACE_Malloc_Header)));
      if (currp == this->cb_ptr_->freep_)
        break;
    }
} 
// Put <ptr> in the free list (locked version).

void
ACE_Malloc_T::free (void *ptr)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::free");

  this->shared_free (ptr);
}

// This function is called by the ACE_Malloc_T constructor to initialize
// the memory pool.  The first time in it allocates room for the
// control block (as well as a chunk of memory, depending on
// rounding...).  Depending on the type of <MEM_POOL> (i.e., shared
// vs. local) subsequent calls from other processes will only
// initialize the control block pointer.

int
ACE_Malloc_T::open (void)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::open");

  size_t rounded_bytes = 0;
  int first_time = 0;

  this->cb_ptr_ = (ACE_Control_Block *)
    this->memory_pool_.init_acquire (sizeof *this->cb_ptr_,
                                     rounded_bytes,
                                     first_time);
  if (this->cb_ptr_ == 0)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("(%P|%t) %p\n"),
                       ACE_LIB_TEXT ("init_acquire failed")),
                      -1);
  else if (first_time)
    {
      // ACE_TRACE ((LM_DEBUG, ACE_LIB_TEXT ("(%P|%t) first time in, control block = %@\n"), this->cb_ptr_));

      MALLOC_HEADER::init_ptr (&this->cb_ptr_->freep_,
                               &this->cb_ptr_->base_,
                               this->cb_ptr_);

      MALLOC_HEADER::init_ptr (&this->cb_ptr_->freep_->next_block_,
                               this->cb_ptr_->freep_,
                               this->cb_ptr_);

      NAME_NODE::init_ptr (&this->cb_ptr_->name_head_,
                           0,
                           this->cb_ptr_);

      this->cb_ptr_->freep_->size_ = 0;
      this->cb_ptr_->ref_counter_ = 1;

      if (rounded_bytes > (sizeof *this->cb_ptr_ + sizeof (MALLOC_HEADER)))
        {
          // If we've got any extra space at the end of the control
          // block, then skip past the dummy <MALLOC_HEADER> to
          // point at the first free block.
          MALLOC_HEADER *p = ((MALLOC_HEADER *) (this->cb_ptr_->freep_)) + 1;

          MALLOC_HEADER::init_ptr (&p->next_block_,
                                   0,
                                   this->cb_ptr_);

          // Why aC++ in 64-bit mode can't grok this, I have no
          // idea... but it ends up with an extra bit set which makes
          // size_ really big without this hack.
#if defined (__hpux) && defined (__LP64__)
          size_t hpux11_hack = (rounded_bytes - sizeof *this->cb_ptr_)
                               / sizeof (MALLOC_HEADER);
          p->size_ = hpux11_hack;
#else
          p->size_ = (rounded_bytes - sizeof *this->cb_ptr_)
            / sizeof (MALLOC_HEADER);
#endif /* (__hpux) && defined (__LP64__) */

/*          ACE_MALLOC_STATS (++this->cb_ptr_->malloc_stats_.nchunks_);
          ACE_MALLOC_STATS (++this->cb_ptr_->malloc_stats_.nblocks_);
          ACE_MALLOC_STATS (++this->cb_ptr_->malloc_stats_.ninuse_);
*/
          // Insert the newly allocated chunk of memory into the free
          // list.  Add "1" to skip over the <MALLOC_HEADER> when
          // freeing the pointer.
          this->shared_free (p + 1);
        }
    }
  else
    ++this->cb_ptr_->ref_counter_;
  return 0;
}

ACE_Malloc_T::ACE_Malloc_T (const char *pool_name)
  : memory_pool_ (pool_name),
    bad_flag_ (0)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::ACE_Malloc_T");

  this->bad_flag_ = this->open ();

  if (this->bad_flag_ == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("%p\n"),
                ACE_LIB_TEXT ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::ACE_Malloc_T")));
}

ACE_Malloc_T::ACE_Malloc_T (const char *pool_name,
                                                              const char *lock_name,
                                                              const ACE_Local_Memory_Pool_Options *options)
  : memory_pool_ (pool_name, options),
    bad_flag_ (0)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::ACE_Malloc_T");
  // Use pool_name for lock_name if lock_name not passed.
//  const char *name = lock_name ? lock_name : pool_name;
  this->bad_flag_ = this->open ();
  if (this->bad_flag_ == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("%p\n"),
                ACE_LIB_TEXT ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::ACE_Malloc_T")));
}


ACE_Malloc_T::ACE_Malloc_T (const char *pool_name,
                            const ACE_Local_Memory_Pool_Options *options)
  : memory_pool_ (pool_name, options),
    bad_flag_ (0)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::ACE_Malloc_T");


  this->bad_flag_ = this->open ();
  if (this->bad_flag_ == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("%p\n"),
                ACE_LIB_TEXT ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::ACE_Malloc_T")));
}


ACE_Malloc_T::~ACE_Malloc_T (void)
{
  ACE_TRACE ("ACE_Malloc_T<MEM_POOL>::~ACE_Malloc_T<MEM_POOL>");
}

// Clean up the resources allocated by ACE_Malloc_T.

int
ACE_Malloc_T::remove (void)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::remove");
  // ACE_TRACE ((LM_DEBUG, ACE_LIB_TEXT ("(%P|%t) destroying ACE_Malloc_T\n")));
  int result = 0;

#if defined (ACE_HAS_MALLOC_STATS)
  this->print_stats ();
#endif /* ACE_HAS_MALLOC_STATS */


  // Give the memory pool a chance to release its resources.
  result = this->memory_pool_.release ();

  // Reset this->cb_ptr_ as it is no longer valid.
  // There's also no need to keep the reference counter as the
  // underlying memory pool has been destroyed.
  // Also notice that we are leaving the decision of removing
  // the pool to users so they can map to the same mmap file
  // again.
  this->cb_ptr_ = 0;

  return result;
}

// General-purpose memory allocator.  Assumes caller holds the locks.

void *
ACE_Malloc_T::shared_malloc (size_t nbytes)
{

  if (this->cb_ptr_ == 0)
    return 0;

  // Round up request to a multiple of the MALLOC_HEADER size.
  size_t nunits =
    (nbytes + sizeof (MALLOC_HEADER) - 1) / sizeof (MALLOC_HEADER)
    + 1; // Add one for the <MALLOC_HEADER> itself.

  MALLOC_HEADER *prevp = 0;
  MALLOC_HEADER *currp = 0;

  ACE_SEH_TRY
    {
      // Begin the search starting at the place in the freelist where the
      // last block was found.
      prevp = this->cb_ptr_->freep_;
      currp = prevp->next_block_;
    }
#if defined (ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS)
  ACE_SEH_EXCEPT (this->memory_pool_.seh_selector (GetExceptionInformation ()))
    {
      currp = prevp->next_block_;
    }
#endif /* ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS */

  // Search the freelist to locate a block of the appropriate size.

  while (1)

    // *Warning* Do not use "continue" within this while-loop.

    {
      ACE_SEH_TRY
        {
          if (currp->size_ >= nunits) // Big enough
            {
//              ACE_MALLOC_STATS (++this->cb_ptr_->malloc_stats_.ninuse_);
              if (currp->size_ == nunits)
                // Exact size, just update the pointers.
                prevp->next_block_ = currp->next_block_;
              else
                {
                  // Remaining chunk is larger than requested block, so
                  // allocate at tail end.
//                  ACE_MALLOC_STATS (++this->cb_ptr_->malloc_stats_.nblocks_);
                  currp->size_ -= nunits;
                  currp += currp->size_;
                  MALLOC_HEADER::init_ptr (&currp->next_block_,
                                           0,
                                           this->cb_ptr_);
                  currp->size_ = nunits;
                }
              this->cb_ptr_->freep_ = prevp;

              // Skip over the MALLOC_HEADER when returning pointer.
              return currp + 1;
            }
          else if (currp == this->cb_ptr_->freep_)
            {
              // We've wrapped around freelist without finding a
              // block.  Therefore, we need to ask the memory pool for
              // a new chunk of bytes.

              size_t chunk_bytes = 0;

              currp = (MALLOC_HEADER *)
                this->memory_pool_.acquire (nunits * sizeof (MALLOC_HEADER),
                                            chunk_bytes);
              void *remap_addr = this->memory_pool_.base_addr ();
              if (remap_addr != 0)
                this->cb_ptr_ = (ACE_Control_Block *) remap_addr;

              if (currp != 0)
                {
//                  ACE_MALLOC_STATS (++this->cb_ptr_->malloc_stats_.nblocks_);
//                  ACE_MALLOC_STATS (++this->cb_ptr_->malloc_stats_.nchunks_);
//                  ACE_MALLOC_STATS (++this->cb_ptr_->malloc_stats_.ninuse_);

                  ACE_Control_Block::ACE_Malloc_Header::init_ptr (&currp->next_block_,
                                           0,
                                           this->cb_ptr_);
                  // Compute the chunk size in MALLOC_HEADER units.
                  currp->size_ = chunk_bytes / sizeof (MALLOC_HEADER);

                  // Insert the newly allocated chunk of memory into the
                  // free list.  Add "1" to skip over the
                  // <MALLOC_HEADER> when freeing the pointer since
                  // the first thing <free> does is decrement by this
                  // amount.
                  this->shared_free (currp + 1);
                  currp = this->cb_ptr_->freep_;
                }
              else
                return 0;
                // Shouldn't do this here because of errors with the wchar ver
                // This is because ACE_ERROR_RETURN converts the __FILE__ to
                // wchar before printing out.  The compiler will complain
                // about this since a destructor would present in a SEH block
                //ACE_ERROR_RETURN ((LM_ERROR,
                //                   ACE_LIB_TEXT ("(%P|%t) %p\n"),
                //                   ACE_LIB_TEXT ("malloc")),
                //                  0);
            }
          prevp = currp;
          currp = currp->next_block_;
        }
      ACE_SEH_EXCEPT (this->memory_pool_.seh_selector (GetExceptionInformation ()))
        {
        }
    }
  return 0;
}

// General-purpose memory allocator.

void *
ACE_Malloc_T::malloc (size_t nbytes)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::malloc");

  return this->shared_malloc (nbytes);
}

// General-purpose memory allocator.

void *
ACE_Malloc_T::calloc (size_t nbytes,
                                                        char initial_value)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::calloc");
  void *ptr = this->malloc (nbytes);

  if (ptr != 0)
    memset (ptr, initial_value, nbytes);

  return ptr;
}

void *
ACE_Malloc_T::calloc (size_t n_elem,
                                                        size_t elem_size,
                                                        char initial_value)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::calloc");

  return this->calloc (n_elem * elem_size, initial_value);
}

// Put block AP in the free list (must be called with locks held!)

void
ACE_Malloc_T::shared_free (void *ap)
{
#if !defined (ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS)
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::shared_free");
#endif /* ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS */

  if (ap == 0 || this->cb_ptr_ == 0)
    return;


  // Adjust AP to point to the block MALLOC_HEADER
  MALLOC_HEADER *blockp = ((MALLOC_HEADER *) ap) - 1;
  MALLOC_HEADER *currp = this->cb_ptr_->freep_;

  // Search until we find the location where the blocks belongs.  Note
  // that addresses are kept in sorted order.

  ACE_SEH_TRY
    {
      for (;
           blockp <= currp
             || blockp >= (MALLOC_HEADER *) currp->next_block_;
           currp = currp->next_block_)
        {
          if (currp >= (MALLOC_HEADER *) currp->next_block_
              && (blockp > currp
                  || blockp < (MALLOC_HEADER *) currp->next_block_))
            // Freed block at the start or the end of the memory pool.
            break;
        }

      // Join to upper neighbor.
      if ((blockp + blockp->size_) == currp->next_block_)
        {
//          ACE_MALLOC_STATS (--this->cb_ptr_->malloc_stats_.nblocks_);
          blockp->size_ += currp->next_block_->size_;
          blockp->next_block_ = currp->next_block_->next_block_;
        }
      else
        blockp->next_block_ = currp->next_block_;

      // Join to lower neighbor.
      if ((currp + currp->size_) == blockp)
        {
//          ACE_MALLOC_STATS (--this->cb_ptr_->malloc_stats_.nblocks_);
          currp->size_ += blockp->size_;
          currp->next_block_ = blockp->next_block_;
        }
      else
        currp->next_block_ = blockp;

//      ACE_MALLOC_STATS (--this->cb_ptr_->malloc_stats_.ninuse_);
      this->cb_ptr_->freep_ = currp;
    }
  ACE_SEH_EXCEPT (this->memory_pool_.seh_selector (GetExceptionInformation ()))
    {
    }
}

// No locks held here, caller must acquire/release lock.

void*
ACE_Malloc_T::shared_find (const char *name)
{
#if !defined (ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS)
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::shared_find");
#endif /* !ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS */

  if (this->cb_ptr_ == 0)
    return 0;

  ACE_SEH_TRY
    {
      for (NAME_NODE *node = this->cb_ptr_->name_head_;
           node != 0;
           node = node->next_)
        if (strcmp (node->name (),
                            name) == 0)
          return node;
    }
  ACE_SEH_EXCEPT (this->memory_pool_.seh_selector (GetExceptionInformation ()))
    {
    }
  return 0;
}

int
ACE_Malloc_T::shared_bind (const char *name,
                                                             void *pointer)
{
  if (this->cb_ptr_ == 0)
    return -1;

  // Combine the two allocations into one to avoid overhead...
  NAME_NODE *new_node = 0;

  ACE_ALLOCATOR_RETURN (new_node,
                        (NAME_NODE *)
                        this->shared_malloc (sizeof (NAME_NODE) +
                                             strlen (name) + 1),
                        -1);
  char *name_ptr = (char *) (new_node + 1);

  // Use operator placement new to insert <new_node> at the head of
  // the linked list of <NAME_NODE>s.
  NAME_NODE *result =
    new (new_node) NAME_NODE (name,
                              name_ptr,
                              reinterpret_cast<char *> (pointer),
                              this->cb_ptr_->name_head_);
  this->cb_ptr_->name_head_ = result;
  return 0;
}

int
ACE_Malloc_T::trybind (const char *name,
                                                         void *&pointer)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::trybind");

  NAME_NODE *node = (NAME_NODE *) this->shared_find (name);

  if (node == 0)
    // Didn't find it, so insert it.
    return this->shared_bind (name, pointer);

  // Found it, so return a copy of the current entry.
  pointer = (char *) node->pointer_;
  return 1;
}

int
ACE_Malloc_T::bind (const char *name,
                                                      void *pointer,
                                                      int duplicates)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::bind");

  if (duplicates == 0 && this->shared_find (name) != 0)
    // If we're not allowing duplicates, then if the name is already
    // present, return 1.
    return 1;

  // If we get this far, either we're allowing duplicates or we didn't
  // find the name yet.
  return this->shared_bind (name, pointer);
}

int
ACE_Malloc_T::find (const char *name,
                                                    void *&pointer)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::find");


  NAME_NODE *node = (NAME_NODE *) this->shared_find (name);

  if (node == 0)
    return -1;

  pointer = (char *) node->pointer_;
  return 0;
}

// Returns a count of the number of available chunks that can hold
// <size> byte allocations.  Function can be used to determine if you
// have reached a water mark. This implies a fixed amount of allocated
// memory.
//
// @param size - the chunk size of that you would like a count of
// @return function returns the number of chunks of the given size
//          that would fit in the currently allocated memory

ssize_t
ACE_Malloc_T::avail_chunks (size_t size) const
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::avail_chunks");

  if (this->cb_ptr_ == 0)
    return -1;

  size_t count = 0;
  // Avoid dividing by 0...
  size = size == 0 ? 1 : size;
  MALLOC_HEADER *currp = this->cb_ptr_->freep_;

  // Calculate how many will fit in this block.
  do {
    size_t avail_size = currp->size_ == 0 ? 0 : currp->size_ - 1;
    if (avail_size * sizeof (MALLOC_HEADER) >= size)
      count += avail_size * sizeof (MALLOC_HEADER) / size;
    currp = currp->next_block_;
  }
  while (currp != this->cb_ptr_->freep_);

  return count;
}

int
ACE_Malloc_T::find (const char *name)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::find");

  return this->shared_find (name) == 0 ? -1 : 0;
}

int
ACE_Malloc_T::unbind (const char *name, void *&pointer)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::unbind");

  if (this->cb_ptr_ == 0)
    return -1;

  NAME_NODE *prev = 0;

  for (NAME_NODE *curr = this->cb_ptr_->name_head_;
       curr != 0;
       curr = curr->next_)
    {
      if (strcmp (curr->name (), name) == 0)
        {
          pointer = (char *) curr->pointer_;

          if (prev == 0)
            this->cb_ptr_->name_head_ = curr->next_;
          else
            prev->next_ = curr->next_;

          if (curr->next_)
            curr->next_->prev_ = prev;

          // This will free up both the node and the name due to our
          // clever trick in <bind>!
          this->shared_free (curr);
          return 0;
        }
      prev = curr;
    }

  // Didn't find it, so fail.
  return -1;
}

int
ACE_Malloc_T::unbind (const char *name)
{
  ACE_TRACE ("ACE_Malloc_T<ACE_MEM_POOL_2, ACE_LOCK, ACE_Control_Block>::unbind");
  void *temp = 0;
  return this->unbind (name, temp);
}

