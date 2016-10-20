// -*- C++ -*-

//==========================================================================
/**
 *  @file    Malloc_T.h
 *
 *  Malloc_T.h,v 4.86 2005/12/22 01:17:51 shuston Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu> and
 *          Irfan Pyarali <irfan@cs.wustl.edu>
 */
//==========================================================================

#ifndef ACE_MALLOC_T_H
#define ACE_MALLOC_T_H

#include "macros.h"               /* Need ACE_Control_Block */
#include "Malloc.h"               /* Need ACE_Control_Block */
#include "Malloc_Base.h"          /* Need ACE_Allocator */
#include "Malloc_Allocator.h"
#include "Free_List.h"
#include "Local_Memory_Pool.h"


/**
 * @class ACE_Allocator_Adapter
 *
 * @brief This class is an Adapter that allows the ACE_Allocator to
 * use the ACE_Malloc class below.
 */
template <class MALLOC>
class ACE_Allocator_Adapter : public ACE_Allocator
{
	
public:
  // Trait.
  typedef MALLOC ALLOCATOR;

  typedef const void *MEMORY_POOL_OPTIONS;



  ACE_Allocator_Adapter (const char *pool_name=0)
    : allocator_ (pool_name)
  {
    ACE_TRACE ("ACE_Allocator_Adapter");
  }


   
  ~ACE_Allocator_Adapter (void)
  {
    ACE_TRACE ("~ACE_Allocator_Adapter");
  }


  /**
   * Note that @a pool_name should be located in
   * a directory with the appropriate visibility and protection so
   * that all processes that need to access it can do so.
   * This constructor must be inline to avoid bugs with some C++
   * compilers. */
  ACE_Allocator_Adapter (const char *pool_name,
                         const char *lock_name,
                         MEMORY_POOL_OPTIONS options = 0)
      : allocator_ (pool_name,
                    lock_name,
                    options)
    {
      ACE_TRACE ("ACE_Allocator_Adapter");
    }


  inline virtual void *  malloc (size_t nbytes)
  {
    ACE_TRACE ("malloc");
    return this->allocator_.malloc (nbytes);
  }
  
  inline virtual void *  calloc (size_t nbytes,
                                         char initial_value)
  {
    ACE_TRACE ("calloc");
    return this->allocator_.calloc (nbytes, initial_value);
  }
  
  inline virtual void * calloc (size_t n_elem,
                                         size_t elem_size,
                                         char initial_value)
  {
    ACE_TRACE ("calloc");
    return this->allocator_.calloc (n_elem, elem_size, initial_value);
  }
  
  inline virtual MALLOC &alloc (void)
  {
    ACE_TRACE ("allocator");
    return this->allocator_;
  }
  
  inline virtual void free (void *ptr)
  {
    ACE_TRACE ("free");
    this->allocator_.free (ptr);
  }
  
  inline virtual int remove (void)
  {
    ACE_TRACE ("remove");
    return this->allocator_.remove ();
  }
  
  inline virtual int trybind (const char *name,
                                          void *&pointer)
  {
    ACE_TRACE ("trybind");
    return this->allocator_.trybind (name, pointer);
  }
  
  inline virtual int bind (const char *name,
                                       void *pointer,
                                       int duplicates)
  {
    ACE_TRACE ("bind");
    return this->allocator_.bind (name, pointer, duplicates);
  }
  
  inline virtual int find (const char *name,
                                       void *&pointer)
  {
    ACE_TRACE ("find");
    return this->allocator_.find (name, pointer);
  }
  
  inline virtual int find (const char *name)
  {
    ACE_TRACE ("find");
    return this->allocator_.find (name);
  }
  
  inline virtual int unbind (const char *name, void *&pointer)
  {
    ACE_TRACE ("unbind");
    return this->allocator_.unbind (name, pointer);
  }
  
  inline virtual int unbind (const char *name)
  {
    ACE_TRACE ("unbind");
    return this->allocator_.unbind (name);
  }
  
  inline virtual int sync (ssize_t len, int flags)
  {
    ACE_TRACE ("sync");
    return this->allocator_.sync (len, flags);
  }

  inline virtual int sync (void *addr, size_t len, int flags)
  {
    ACE_TRACE ("sync");
    return this->allocator_.sync (addr, len, flags);
  }

  inline virtual int protect (ssize_t len=-1, int flags = PROT_RDWR)
  {
    ACE_TRACE ("protect");
    return this->allocator_.protect (len, flags);
  }

  inline virtual int protect (void *addr, size_t len, int flags = PROT_RDWR)
  {
    ACE_TRACE ("protect");
    return this->allocator_.protect (addr, len, flags);
  }

#if defined (ACE_HAS_MALLOC_STATS)
  virtual void print_stats (void) const
  {
    ACE_TRACE ("print_stats");
    this->allocator_.print_stats ();
  }
#endif /* ACE_HAS_MALLOC_STATS */

  virtual void dump (void) const
  {
#if defined (ACE_HAS_DUMP)
    ACE_TRACE ("dump");
    this->allocator_.dump ();
#endif /* ACE_HAS_DUMP */
  }

private:
  /// ALLOCATOR instance, which is owned by the adapter.
  ALLOCATOR allocator_;
};

/**
 * @class ACE_Malloc_T
 *
 * @brief Define a C++ class that uses parameterized types to provide
 * an extensible mechanism for encapsulating various of dynamic
 * memory management strategies.
 *
 * This class can be configured flexibly with different
 * MEMORY_POOL strategies and different types of ACE_LOCK
 * strategies that support the @a ACE_Thread_Mutex and @a
 * ACE_Process_Mutex constructor API.
 *
 * Note that the @a bind() and @a find() methods use linear search, so
 * it's not a good idea to use them for managing a large number of
 * entities.  If you need to manage a large number of entities, it's
 * recommended that you @a bind() an @ ACE_Hash_Map_Manager that
 * resides in shared memory, use @a find() to locate it, and then
 * store/retrieve the entities in the hash map.
 * */
class ACE_Malloc_T
{
public:
  friend class ACE_Malloc_LIFO_Iterator_T;
  friend class ACE_Malloc_FIFO_Iterator_T;
  //typedef ACE_Local_Memory_Pool ACE_MEM_POOL;
//  typedef ACE_Local_Memory_Pool_Options ACE_Local_Memory_Pool_Options;
  typedef ACE_Control_Block::ACE_Name_Node NAME_NODE;
  typedef ACE_Control_Block::ACE_Malloc_Header MALLOC_HEADER;

  // = Initialization and termination methods.
  /**
   * Initialize ACE_Malloc.  This constructor passes @a pool_name to
   * initialize the memory pool, and uses ACE::basename() to
   * automatically extract out the name used for the underlying lock
   * name (if necessary).
   *
   * Note that @a pool_name should be located in
   * a directory with the appropriate visibility and protection so
   * that all processes that need to access it can do so.
   */
  ACE_Malloc_T (const char  *pool_name = 0);

  /**
   * Initialize ACE_Malloc.  This constructor passes @a pool_name to
   * initialize the memory pool, and uses @a lock_name to automatically
   * extract out the name used for the underlying lock name (if
   * necessary).  In addition, @a options is passed through to
   * initialize the underlying memory pool.
   *
   * Note that @a pool_name should be located in
   * a directory with the appropriate visibility and protection so
   * that all processes that need to access it can do so.
   */
  ACE_Malloc_T (const char  *pool_name,
                const char  *lock_name,
                const ACE_Local_Memory_Pool_Options *options = 0);

  /**
   * Initialize an ACE_Malloc with an external ACE_LOCK.
   * This constructor passes @a pool_name and @a options to initialize
   * the memory pool. @a lock is used as the pool lock, and must be
   * properly set up and ready for use before being passed to this method.
   */
  ACE_Malloc_T (const char  *pool_name,
                const ACE_Local_Memory_Pool_Options *options
                );

#if !defined (ACE_HAS_TEMPLATE_TYPEDEFS)
  /// This is necessary to work around template bugs with certain C++
  /// compilers.
  ACE_Malloc_T (const char  *pool_name,
                const char  *lock_name,
                const void *options = 0);
#endif /* ACE_HAS_TEMPLATE_TYPEDEFS */

  /// Destructor
  ~ACE_Malloc_T (void);

  
  /// Releases resources allocated by this object.
  int remove (void);

  // = Memory management

  /// Allocate @a nbytes, but don't give them any initial value.
  void *malloc (size_t nbytes);

  /// Allocate @a nbytes, giving them @a initial_value.
  void *calloc (size_t nbytes, char initial_value = '\0');

  /// Allocate @a n_elem each of size @a elem_size, giving them
  /// @a initial_value.
  void *calloc (size_t n_elem,
                size_t elem_size,
                char initial_value = '\0');

  /// Deallocate memory pointed to by @a ptr, which must have been
  /// allocated previously by malloc().
  void free (void *ptr);

  inline ACE_Local_Memory_Pool &memory_pool (void)
  {
    ACE_TRACE ("ACE_Malloc_T<MEMORY_POOL, ACE_LOCK, ACE_CB>::memory_pool");
    return this->memory_pool_;
  }

  inline int sync (ssize_t len, int flags)
  {
    ACE_TRACE ("ACE_Malloc_T<MEMORY_POOL, ACE_LOCK, ACE_CB>::sync");
    return this->memory_pool_.sync (len, flags);
  }

  inline int sync (void *addr, size_t len, int flags)
  {
    ACE_TRACE ("ACE_Malloc_T<MEMORY_POOL, ACE_LOCK, ACE_CB>::sync");
    return this->memory_pool_.sync (addr, len, flags);
  }

  inline int protect (ssize_t len, int flags)
  {
    ACE_TRACE ("ACE_Malloc_T<MEMORY_POOL, ACE_LOCK, ACE_CB>::protect");
    return this->memory_pool_.protect (len, flags);
  }

  inline int protect (void *addr, size_t len, int flags)
  {
    ACE_TRACE ("ACE_Malloc_T<MEMORY_POOL, ACE_LOCK, ACE_CB>::protect");
    return this->memory_pool_.protect (addr, len, flags);
  }

  inline void *base_addr (void)
  {
    return this->cb_ptr_;
  }

  inline int bad (void)
  {
    return this->bad_flag_;
  }

  inline int ref_counter (void)
  {
    if (this->cb_ptr_ != 0)
      return this->cb_ptr_->ref_counter_;

    return -1;
  }


  inline int release (int close)
  {
    if (this->cb_ptr_ != 0)
    {
      int retv = --this->cb_ptr_->ref_counter_;

      if (close)
        this->memory_pool_.release (0);

      if (retv == 0)
        this->remove ();
      return retv;
    }
    return -1;
  }


  
  /**
   * Associate @a name with @a pointer.  If @a duplicates == 0 then do
   * not allow duplicate name/pointer associations, else if
   * @a duplicates != 0 then allow duplicate name/pointer
   * assocations.  Returns 0 if successfully binds (1) a previously
   * unbound @a name or (2) @a duplicates != 0, returns 1 if trying to
   * bind a previously bound @a name and @a duplicates == 0, else
   * returns -1 if a resource failure occurs.
   */
  int bind (const char *name, void *pointer, int duplicates = 0);

  /**
   * Associate @a name with @a pointer.  Does not allow duplicate
   * name/pointer associations.  Returns 0 if successfully binds
   * (1) a previously unbound @a name, 1 if trying to bind a previously
   * bound @a name, or returns -1 if a resource failure occurs.  When
   * this call returns @a pointer's value will always reference the
   * void * that @a name is associated with.  Thus, if the caller needs
   * to use @a pointer (e.g., to free it) a copy must be maintained by
   * the caller.
   */
  int trybind (const char *name, void *&pointer);

  /// Locate @a name and pass out parameter via @a pointer.  If found,
  /// return 0, returns -1 if failure occurs.
  int find (const char *name, void *&pointer);

  /// Returns 0 if @a name is in the mapping. -1, otherwise.
  int find (const char *name);

  /**
   * Unbind (remove) the name from the map.  Don't return the pointer
   * to the caller.  If you want to remove all occurrences of @a name
   * you'll need to call this method multiple times until it fails...
   */
  int unbind (const char *name);

  /**
   * Unbind (remove) one association of @a name to @a pointer.  Returns
   * the value of pointer in case the caller needs to deallocate
   * memory.  If you want to remove all occurrences of @a name you'll
   * need to call this method multiple times until it fails...
   */
  int unbind (const char *name, void *&pointer);

  // = Protection and "sync" (i.e., flushing data to backing store).

  /**
   * Sync @a len bytes of the memory region to the backing store
   * starting at @c this->base_addr_.  If @a len == -1 then sync the
   * whole region.
   */

  /**
   * Returns a count of the number of available chunks that can hold
   * @a size byte allocations.  Function can be used to determine if you
   * have reached a water mark. This implies a fixed amount of allocated
   * memory.
   *
   * @param size  The chunk size of that you would like a count of
   * @return Function returns the number of chunks of the given size
   *          that would fit in the currently allocated memory.
   */
  ssize_t avail_chunks (size_t size) const;

#if defined (ACE_HAS_MALLOC_STATS)
  /// Dump statistics of how malloc is behaving.
  void print_stats (void) const;
#endif /* ACE_HAS_MALLOC_STATS */

  /// Returns a pointer to the lock used to provide mutual exclusion to
  /// an ACE_Malloc allocator.

  ACE_ALLOC_HOOK_DECLARE;
  /// Dump the state of an object.
  void dump (void) const;

  /// Declare the dynamic allocation hooks.
  /**
   * Bad flag.  This operation should be called immediately after the
   * construction of the Malloc object to query whether the object was
   * constructed successfully.  If not, the user should invoke @c
   * remove and release the object (it is not usable.)
   * @retval 0 if all is fine.  non-zero if this malloc object is
   *         unuable.
   */
   void print_stats (void) const;

private:
  /// Initialize the Malloc pool.
  int open (void);

  /// Associate @a name with @a pointer.  Assumes that locks are held by
  /// callers.
  int shared_bind (const char *name,
                   void *pointer);

  /**
   * Try to locate @a name.  If found, return the associated
   * ACE_Name_Node, else returns 0 if can't find the @a name.
   * Assumes that locks are held by callers.  Remember to cast the
   * return value to ACE_CB::ACE_Name_Node*.
   */
  void *shared_find (const char *name);

  /// Allocate memory.  Assumes that locks are held by callers.
  void *shared_malloc (size_t nbytes);

  /// Deallocate memory.  Assumes that locks are held by callers.
  void shared_free (void *ptr);

  /// Pointer to the control block that is stored in memory controlled
  /// by <MEMORY_POOL>.
  ACE_Control_Block *cb_ptr_;

  /// Pool of memory used by ACE_Malloc to manage its freestore.
  ACE_Local_Memory_Pool memory_pool_;

  /// Lock that ensures mutual exclusion for the memory pool.

  /// Keep track of failure in constructor.
  int bad_flag_;
};

/*****************************************************************************/

/**
 * @class ACE_Malloc_Lock_Adapter_T
 *
 * @brief Template functor adapter for lock strategies used with ACE_Malloc_T.
 *
 * This class acts as a factory for lock strategies that have various ctor
 * signatures.  If the lock strategy's ctor takes an ACE_TCHAR* as the first
 * and only required parameter, it will just work.  Otherwise use template
 * specialization to create a version that matches the lock strategy's ctor
 * signature.  See ACE_Process_Semaphore and ACE_Thread_Semaphore for
 * examples.
 *
 */
/*****************************************************************************/

/**
 * @class ACE_Malloc_LIFO_Iterator_T
 *
 * @brief LIFO iterator for names stored in Malloc'd memory.
 *
 * This class can be configured flexibly with different types of
 * ACE_LOCK strategies that support the @a ACE_Thread_Mutex and @a
 * ACE_Process_Mutex constructor API.
 *
 * Does not support deletions while iteration is occurring.
 */
class ACE_Malloc_LIFO_Iterator_T
{
public:
  typedef ACE_Control_Block::ACE_Name_Node NAME_NODE;
  typedef ACE_Control_Block::ACE_Malloc_Header MALLOC_HEADER;

  // = Initialization method.
  /// If @a name = 0 it will iterate through everything else only
  /// through those entries whose @a name match.
  ACE_Malloc_LIFO_Iterator_T (ACE_Malloc_T &malloc,
                              const char *name = 0);

  /// Destructor.
  ~ACE_Malloc_LIFO_Iterator_T (void);

  // = Iteration methods.

  /// Returns 1 when all items have been seen, else 0.
  int done (void) const;

  /// Pass back the next entry in the set that hasn't yet been
  /// visited.  Returns 0 when all items have been seen, else 1.
  int next (void *&next_entry);

  /**
   * Pass back the next entry (and the name associated with it) in
   * the set that hasn't yet been visited.  Returns 0 when all items
   * have been seen, else 1.
   */
  int next (void *&next_entry,
            const char *&name);

  /// Move forward by one element in the set.  Returns 0 when all the
  /// items in the set have been seen, else 1.
  int advance (void);

  /// Dump the state of an object.
  void dump (void) const;

  ACE_ALLOC_HOOK_DECLARE;

private:
  /// Malloc we are iterating over.
  ACE_Malloc_T &malloc_;

  /// Keeps track of how far we've advanced...
  NAME_NODE *curr_;


  /// Name that we are searching for.
  const char *name_;
};

/**
 * @class ACE_Malloc_FIFO_Iterator_T
 *
 * @brief FIFO iterator for names stored in Malloc'd memory.
 *
 * This class can be configured flexibly with different types of
 * ACE_LOCK strategies that support the @a ACE_Thread_Mutex and @a
 * ACE_Process_Mutex constructor API.
 *
 * Does not support deletions while iteration is occurring.
 */
class ACE_Malloc_FIFO_Iterator_T
{
public:
  typedef ACE_Control_Block::ACE_Name_Node NAME_NODE;
  typedef ACE_Control_Block::ACE_Malloc_Header MALLOC_HEADER;

  // = Initialization method.
  /// If @a name = 0 it will iterate through everything else only
  /// through those entries whose @a name match.
  ACE_Malloc_FIFO_Iterator_T (ACE_Malloc_T &malloc,
                              const char *name = 0);

  /// Destructor.
  ~ACE_Malloc_FIFO_Iterator_T (void);

  // = Iteration methods.

  /// Returns 1 when all items have been seen, else 0.
  int done (void) const;

  /// Pass back the next entry in the set that hasn't yet been
  /// visited.  Returns 0 when all items have been seen, else 1.
  int next (void *&next_entry);

  /**
   * Pass back the next entry (and the name associated with it) in
   * the set that hasn't yet been visited.  Returns 0 when all items
   * have been seen, else 1.
   */
  int next (void *&next_entry, const char *&name);

  /// Move forward by one element in the set.  Returns 0 when all the
  /// items in the set have been seen, else 1.
  int advance (void);

  /// Go to the starting element that was inserted first. Returns 0
  /// when there is no item in the set, else 1.
  int start (void);

  /// Dump the state of an object.
  void dump (void) const;

  /// Declare the dynamic allocation hooks.
  void *operator new (size_t bytes); \
  void operator delete (void *ptr);


private:
  /// Malloc we are iterating over.
  ACE_Malloc_T &malloc_;

  /// Keeps track of how far we've advanced...
  NAME_NODE *curr_;


  /// Name that we are searching for.
  const char *name_;
};

class ACE_Malloc : public ACE_Malloc_T
{
public:


  inline ACE_Malloc (const char  *pool_name)
    : ACE_Malloc_T (pool_name)
  {
  }

  inline ACE_Malloc (const char  *pool_name,
                         const char  *lock_name,
                         const ACE_Local_Memory_Pool_Options *options)
  : ACE_Malloc_T (pool_name, lock_name, options)
  {

  }

#if !defined (ACE_HAS_TEMPLATE_TYPEDEFS)
  /// This is necessary to work around template bugs with certain C++
  /// compilers.
  ACE_Malloc (const char  *pool_name,
              const char  *lock_name,
              const void *options = 0);
#endif /* ACE_HAS_TEMPLATE_TYPEDEFS */
};



#endif /* ACE_MALLOC_H */
