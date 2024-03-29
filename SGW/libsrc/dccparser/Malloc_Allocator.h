                                 // -*- C++ -*-

//==========================================================================
/**
 *  @file   Malloc_Allocator.h
 *
 *  Malloc_Allocator.h,v 4.12 2005/11/24 09:48:54 
 *
 *  @author 
 */
//==========================================================================

#ifndef ACE_MALLOC_ALLOCATOR_H
#define ACE_MALLOC_ALLOCATOR_H

#include "macros.h"
#include "Malloc_Base.h"

/**
 * @class ACE_New_Allocator
 *
 * @brief Defines a class that provided a simple implementation of
 * memory allocation.
 *
 * This class uses the new/delete operators to allocate and free up
 * memory.  Please note that the only methods that are supported are
 * <malloc>, <calloc>, and <free>. All other methods are no-ops that
 * return -1 and set <errno> to <ENOTSUP>.  If you require this
 * functionality, please use: ACE_Allocator_Adapter <ACE_Malloc
 * <ACE_LOCAL_MEMORY_POOL, MUTEX> >, which will allow you to use the
 * added functionality of bind/find/etc. while using the new/delete
 * operators.
 */
class ACE_New_Allocator : public ACE_Allocator
{
public:
  /// These methods are defined.
  virtual void *malloc (size_t nbytes);
  virtual void *calloc (size_t nbytes, char initial_value = '\0');
  virtual void *calloc (size_t n_elem, size_t elem_size, char initial_value = '\0');
  virtual void free (void *ptr);

  /// These methods are no-ops.
  virtual int remove (void);
  virtual int bind (const char *name, void *pointer, int duplicates = 0);
  virtual int trybind (const char *name, void *&pointer);
  virtual int find (const char *name, void *&pointer);
  virtual int find (const char *name);
  virtual int unbind (const char *name);
  virtual int unbind (const char *name, void *&pointer);
  virtual int sync (ssize_t len = -1, int flags = MS_SYNC);
  virtual int sync (void *addr, size_t len, int flags = MS_SYNC);
  virtual int protect (ssize_t len = -1, int prot = PROT_RDWR);
  virtual int protect (void *addr, size_t len, int prot = PROT_RDWR);
#if defined (ACE_HAS_MALLOC_STATS)
  virtual void print_stats (void) const;
#endif /* ACE_HAS_MALLOC_STATS */
  virtual void dump (void) const;

private:
  // DO NOT ADD ANY STATE (DATA MEMBERS) TO THIS CLASS!!!!  See the
  // <ACE_Allocator::instance> implementation for explanation.
};

/**
 * @class ACE_Static_Allocator_Base
 *
 * @brief Defines a class that provided a highly optimized memory
 * management scheme for allocating memory statically.
 *
 * This class manages a fixed-size <POOL_SIZE> of memory.  Every
 * time <malloc>/<calloc> is called, it simply moves an internal
 * index forward and returns a pointer to the requested chunk.
 * All memory is allocated statically (typically via the
 * <ACE_Static_Allocator> template) and <free> is a no-op.  This
 * behavior is useful for use-cases where all the memory
 * allocation needs are known in advance and no deletions ever
 * occur.
 */
class ACE_Static_Allocator_Base : public ACE_Allocator
{
public:
  ACE_Static_Allocator_Base (char *buffer, size_t size);
  virtual void *malloc (size_t nbytes);
  virtual void *calloc (size_t nbytes, char initial_value = '\0');
  virtual void *calloc (size_t n_elem, size_t elem_size, char initial_value = '\0');
  virtual void free (void *ptr);
  virtual int remove (void);
  virtual int bind (const char *name, void *pointer, int duplicates = 0);
  virtual int trybind (const char *name, void *&pointer);
  virtual int find (const char *name, void *&pointer);
  virtual int find (const char *name);
  virtual int unbind (const char *name);
  virtual int unbind (const char *name, void *&pointer);
  virtual int sync (ssize_t len = -1, int flags = MS_SYNC);
  virtual int sync (void *addr, size_t len, int flags = MS_SYNC);
  virtual int protect (ssize_t len = -1, int prot = PROT_RDWR);
  virtual int protect (void *addr, size_t len, int prot = PROT_RDWR);
#if defined (ACE_HAS_MALLOC_STATS)
  virtual void print_stats (void) const;
#endif /* ACE_HAS_MALLOC_STATS */
  virtual void dump (void) const;

protected:
  /// Don't allow direct instantiations of this class.
  ACE_Static_Allocator_Base (void);

  /// Pointer to the buffer.
  char *buffer_;

  /// Size of the buffer.
  size_t size_;

  /// Pointer to the current offset in the <buffer_>.
  size_t offset_;
};

#endif /* MALLOC_ALLOCATOR_H */
