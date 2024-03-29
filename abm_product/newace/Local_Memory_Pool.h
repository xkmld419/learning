// -*- C++ -*-

//=============================================================================
/**
 *  @file     Local_Memory_Pool.h
 *
 *  Local_Memory_Pool.h,v 4.5 2005/10/28 16:14:52 ossama Exp
 *
 *  @author Dougls C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Prashant Jain <pjain@cs.wustl.edu>
 */
//=============================================================================

#ifndef ACE_LOCAL_MEMORY_POOL_H
#define ACE_LOCAL_MEMORY_POOL_H

#include "ACE.h"


#include "Unbounded_Set.h"

//typedef ACE_Unbounded_Set<char *> ACE_Char_Unbounded_Set;

/**
 * @class ACE_Local_Memory_Pool_Options
 *
 * @brief Helper class for Local Memory Pool constructor options.
 *
 * This should be a nested class, but that breaks too many
 * compilers.
 */
class  ACE_Local_Memory_Pool_Options
{
	
};

/**
 * @class ACE_Local_Memory_Pool
 *
 * @brief Make a memory pool that is based on C++ new/delete.  This is
 * useful for integrating existing components that use new/delete
 * into the ACE Malloc scheme...
 */
class ACE_Local_Memory_Pool
{
public:
  typedef ACE_Local_Memory_Pool_Options OPTIONS;

  /// Initialize the pool.
  ACE_Local_Memory_Pool (const char  *backing_store_name = 0,
                         const OPTIONS *options = 0);

  virtual ~ACE_Local_Memory_Pool (void);

  /// Ask system for initial chunk of local memory.
  virtual void *init_acquire (size_t nbytes,
                              size_t &rounded_bytes,
                              int &first_time);

  /// Acquire at least @a nbytes from the memory pool. @a rounded_bytes is
  /// the actual number of bytes allocated.
  virtual void *acquire (size_t nbytes,
                         size_t &rounded_bytes);

  /// Instruct the memory pool to release all of its resources.
  virtual int release (int destroy = 1);

  /**
   * Sync @a len bytes of the memory region to the backing store
   * starting at <this->base_addr_>.  If @a len == -1 then sync the
   * whole region.
   */
  virtual int sync (ssize_t len = -1, int flags = MS_SYNC);

  /// Sync @a len bytes of the memory region to the backing store
  /// starting at @a add_.
  virtual int sync (void *addr, size_t len, int flags = MS_SYNC);

  /**
   * Change the protection of the pages of the mapped region to <prot>
   * starting at <this->base_addr_> up to <len> bytes.  If <len> == -1
   * then change protection of all pages in the mapped region.
   */
  virtual int protect (ssize_t len = -1, int prot = PROT_RDWR);

  /// Change the protection of the pages of the mapped region to <prot>
  /// starting at <addr> up to <len> bytes.
  virtual int protect (void *addr, size_t len, int prot = PROT_RDWR);


  /**
   * Try to extend the virtual address space so that <addr> is now
   * covered by the address mapping.  Always returns 0 since we can't
   * remap a local memory pool.
   */
  virtual int remap (void *addr);

  /// Return the base address of this memory pool, 0 if base_addr
  /// never changes.
  virtual void *base_addr (void) const;

  /// Dump the state of an object.
  virtual void dump (void) const;

  ACE_ALLOC_HOOK_DECLARE;
protected:
  /// List of memory that we have allocated.
  ACE_Unbounded_Set<char *> allocated_chunks_;
//  ACE_Char_Unbounded_Set allocated_chunks_;
  /// Implement the algorithm for rounding up the request to an
  /// appropriate chunksize.
  virtual size_t round_up (size_t nbytes);

};

#endif /* ACE_LOCAL_MEMORY_POOL_H */
