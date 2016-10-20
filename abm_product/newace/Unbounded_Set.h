// -*- C++ -*-

//=============================================================================
/**
 *  @file Unbounded_Set.h
 *
 *  Unbounded_Set.h,v 4.17 2006/02/10 10:33:16 jwillemsen Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//=============================================================================

#ifndef ACE_UNBOUNDED_SET_H
#define ACE_UNBOUNDED_SET_H

#include "Node.h"
#include "Trace.h"
#include "Log_Msg.h"
#include "Malloc_Base.h"
#include <stddef.h>

//class ACE_Allocator;

/**
 * @class ACE_Unbounded_Set_Iterator
 *
 * @brief Implement an iterator over an unbounded set.
 */
template <class T>
class ACE_Unbounded_Set_Iterator
{
public:
  // = Initialization method.
//  ACE_Unbounded_Set_Iterator (ACE_Unbounded_Set<T> &s, int end = 0);


  ACE_Unbounded_Set_Iterator (ACE_Unbounded_Set<T> &s, int end = 0)
    : current_ (end == 0 ? s.head_->next_ : s.head_ ),
     set_ (&s)
  {
    ACE_TRACE ("ACE_Unbounded_Set_Iterator<T>::ACE_Unbounded_Set_Iterator");
  }

  inline int advance (void)
  {
    ACE_TRACE ("advance");
    this->current_ = this->current_->next_;
    return this->current_ != this->set_->head_;
  }
  
  inline int  first (void)
  {
    // ACE_TRACE ("first");
    this->current_ = this->set_->head_->next_;
    return this->current_ != this->set_->head_;
  }
  
  inline int done (void) const
  {
    ACE_TRACE ("done");
  
    return this->current_ == this->set_->head_;
  }
  
  inline int next (T *&item)
  {
    // ACE_TRACE ("next");
    if (this->current_ == this->set_->head_)
      return 0;
    else
      {
        item = &this->current_->item_;
        return 1;
      }
  }

  void dump (void) const
  {
    #if defined (ACE_HAS_DUMP)
     ACE_TRACE ("ACE_Unbounded_Set_Iterator<T>::dump");
    #endif /* ACE_HAS_DUMP */
  }


  inline ACE_Unbounded_Set_Iterator<T>&  operator++ (void)
  {
    // ACE_TRACE ("operator++ (void)");
  
    // prefix operator
  
    this->advance ();
    return *this;
  }
  
  inline T& operator* (void)
  {
    //ACE_TRACE ("operator*");
    T *retv = 0;
  
    int result = this->next (retv);
    ACE_ASSERT (result != 0);
    ACE_UNUSED_ARG (result);
  
    return *retv;
  }

  inline ACE_Unbounded_Set_Iterator<T> operator++ (int)
  {
  //ACE_TRACE ("operator++ (int)");
    ACE_Unbounded_Set_Iterator<T> retv (*this);

  // postfix operator

    this->advance ();
    return retv;
  } 

  inline  bool operator== (const ACE_Unbounded_Set_Iterator<T> &rhs) const
  {
    //ACE_TRACE ("operator==");
    return (this->set_ == rhs.set_ && this->current_ == rhs.current_);
  }
  
  inline  bool operator!= (const ACE_Unbounded_Set_Iterator<T> &rhs) const
  {
    //ACE_TRACE ("operator!=");
    return (this->set_ != rhs.set_ || this->current_ != rhs.current_);
  }
  
  /// Declare the dynamic allocation hooks.
  void *operator new (size_t bytes) { return ::new char[bytes]; } 
  void operator delete (void *ptr) { delete [] ((char *) ptr); }


private:

  /// Pointer to the current node in the iteration.
  ACE_Node<T> *current_;

  /// Pointer to the set we're iterating over.
  ACE_Unbounded_Set<T> *set_;
};

/**
 * @class ACE_Unbounded_Set_Const_Iterator
 *
 * @brief Implement an const iterator over an unbounded set.
 */
template <class T>
class ACE_Unbounded_Set_Const_Iterator
{
public:
  // = Initialization method.
  void dump (void) const
  {
  #if defined (ACE_HAS_DUMP)
    // ACE_TRACE ("dump");
  #endif /* ACE_HAS_DUMP */
  }
  
  ACE_Unbounded_Set_Const_Iterator (const ACE_Unbounded_Set<T> &s, int end = 0)
    : current_ (end == 0 ? s.head_->next_ : s.head_ ),
      set_ (&s)
  {
    // ACE_TRACE ("ACE_Unbounded_Set_Const_Iterator");
  }
  
  int advance (void)
  {
    // ACE_TRACE ("advance");
    this->current_ = this->current_->next_;
    return this->current_ != this->set_->head_;
  }
  
  int first (void)
  {
    // ACE_TRACE ("first");
    this->current_ = this->set_->head_->next_;
    return this->current_ != this->set_->head_;
  }
  
  int done (void) const
  {
    ACE_TRACE ("done");
  
    return this->current_ == this->set_->head_;
  }
  
  int next (T *&item)
  {
    // ACE_TRACE ("next");
    if (this->current_ == this->set_->head_)
      return 0;
    else
      {
        item = &this->current_->item_;
        return 1;
      }
  }
  
  ACE_Unbounded_Set_Const_Iterator<T>
  operator++ (int)
  {
    //ACE_TRACE ("operator++ (int)");
    ACE_Unbounded_Set_Const_Iterator<T> retv (*this);
  
    // postfix operator
  
    this->advance ();
    return retv;
  }
  
  ACE_Unbounded_Set_Const_Iterator<T>&
  operator++ (void)
  {
    // ACE_TRACE ("operator++ (void)");
  
    // prefix operator
  
    this->advance ();
    return *this;
  }
  
  T& operator* (void)
  {
    //ACE_TRACE ("operator*");
    T *retv = 0;
  
    int const result = this->next (retv);
    ACE_ASSERT (result != 0);
    ACE_UNUSED_ARG (result);
  
    return *retv;
  }

  void *operator new (size_t bytes) { return ::new char[bytes]; } 
  void operator delete (void *ptr) { delete [] ((char *) ptr); }

private:

  /// Pointer to the current node in the iteration.
  ACE_Node<T> *current_;

  /// Pointer to the set we're iterating over.
  const ACE_Unbounded_Set<T> *set_;
};

/**
 * @class ACE_Unbounded_Set
 *
 * @brief Implement a simple unordered set of <T> of unbounded size.
 *
 * This implementation of an unordered set uses a circular
 * linked list with a dummy node.  This implementation does not
 * allow duplicates, but it maintains FIFO ordering of insertions.
 *
 * <b> Requirements and Performance Characteristics</b>
 *   - Internal Structure
 *       Circular linked list
 *   - Duplicates allowed?
 *       No
 *   - Random access allowed?
 *       No
 *   - Search speed
 *       Linear
 *   - Insert/replace speed
 *       Linear
 *   - Iterator still valid after change to container?
 *       Yes
 *   - Frees memory for removed elements?
 *       Yes
 *   - Items inserted by
 *       Value
 *   - Requirements for contained type
 *       -# Default constructor
 *       -# Copy constructor
 *       -# operator=
 *       -# operator==
 *
 */
template <class T>
class ACE_Unbounded_Set
{
public:
  friend class ACE_Unbounded_Set_Iterator<T>;
  friend class ACE_Unbounded_Set_Const_Iterator<T>;

  // Trait definition.
  typedef ACE_Unbounded_Set_Iterator<T> ITERATOR;
  typedef ACE_Unbounded_Set_Iterator<T> iterator;
  typedef ACE_Unbounded_Set_Const_Iterator<T> CONST_ITERATOR;
  typedef ACE_Unbounded_Set_Const_Iterator<T> const_iterator;

  size_t size (void) const
  {
    // ACE_TRACE ("size");
    return this->cur_size_;
  }

  inline int insert_tail (const T &item)
  {
    // ACE_TRACE ("insert_tail");
    ACE_Node<T> *temp;
  
    // Insert <item> into the old dummy node location.
    this->head_->item_ = item;
  
    // Create a new dummy node.
    ACE_NEW_MALLOC_RETURN (temp,
                           static_cast<ACE_Node<T>*> (this->allocator_->malloc (sizeof (ACE_Node<T>))),
                           ACE_Node<T> (this->head_->next_), -1);
    // Link this pointer into the list.
    this->head_->next_ = temp;
  
    // Point the head to the new dummy node.
    this->head_ = temp;
  
    ++this->cur_size_;
    return 0;
  }

  void dump (void) const
  {
  #if defined (ACE_HAS_DUMP)
    ACE_TRACE ("dump");
  
    ACE_DEBUG ((LM_DEBUG, ACE_BEGIN_DUMP, this));
    ACE_DEBUG ((LM_DEBUG,  ACE_LIB_TEXT ("\nhead_ = %u"), this->head_));
    ACE_DEBUG ((LM_DEBUG,  ACE_LIB_TEXT ("\nhead_->next_ = %u"), this->head_->next_));
    ACE_DEBUG ((LM_DEBUG,  ACE_LIB_TEXT ("\ncur_size_ = %d\n"), this->cur_size_));
  
    T *item = 0;
  #if !defined (ACE_NLOGGING)
    size_t count = 1;
  #endif /* ! ACE_NLOGGING */
  
    for (ACE_Unbounded_Set_Iterator<T> iter (*(ACE_Unbounded_Set<T> *) this);
         iter.next (item) != 0;
         iter.advance ())
      ACE_DEBUG ((LM_DEBUG,  ACE_LIB_TEXT ("count = %d\n"), count++));
  
    ACE_DEBUG ((LM_DEBUG, ACE_END_DUMP));
  #endif /* ACE_HAS_DUMP */
  }
  
  
  ~ACE_Unbounded_Set (void)
  {
    // ACE_TRACE ("~ACE_Unbounded_Set");
  
    this->delete_nodes ();
  
    // Delete the dummy node.
    ACE_DES_FREE_TEMPLATE (head_,
                           this->allocator_->free,
                           ACE_Node,
                           <T>);
    this->head_ = 0;
  }
  
  ACE_Unbounded_Set (ACE_Allocator *alloc=0)
    : head_ (0),
      cur_size_ (0),
      allocator_ (alloc)
  {
    // ACE_TRACE ("ACE_Unbounded_Set");
  
    if (this->allocator_ == 0)
      this->allocator_ = ACE_Allocator::instance ();
  
    ACE_NEW_MALLOC (this->head_,
                    (ACE_Node<T>*) this->allocator_->malloc (sizeof (ACE_Node<T>)),
                    ACE_Node<T>);
    // Make the list circular by pointing it back to itself.
    this->head_->next_ = this->head_;
  }
  
  ACE_Unbounded_Set (const ACE_Unbounded_Set<T> &us)
    : head_ (0),
      cur_size_ (0),
      allocator_ (us.allocator_)
  {
    ACE_TRACE ("ACE_Unbounded_Set");
  
    if (this->allocator_ == 0)
      this->allocator_ = ACE_Allocator::instance ();
  
    ACE_NEW_MALLOC (this->head_,
                    (ACE_Node<T>*) this->allocator_->malloc (sizeof (ACE_Node<T>)),
                    ACE_Node<T>);
    this->head_->next_ = this->head_;
    this->copy_nodes (us);
  }
  
  ACE_Unbounded_Set<T> & operator= (const ACE_Unbounded_Set<T> &us)
  {
    ACE_TRACE ("operator=");
  
    if (this != &us)
      {
        this->delete_nodes ();
        this->copy_nodes (us);
      }
  
    return *this;
  }
  
  int find (const T &item) const
  {
    // ACE_TRACE ("find");
    // Set <item> into the dummy node.
    this->head_->item_ = item;
  
    ACE_Node<T> *temp = this->head_->next_;
  
    // Keep looping until we find the item.
    while (!(temp->item_ == item))
      temp = temp->next_;
  
    // If we found the dummy node then it's not really there, otherwise,
    // it is there.
    return temp == this->head_ ? -1 : 0;
  }

  int insert (const T &item)
  {
  // ACE_TRACE ("insert");
    if (this->find (item) == 0)
      return 1;
    else
      return this->insert_tail (item);
  }

  int remove (const T &item)
  {
  // ACE_TRACE ("remove");

  // Insert the item to be founded into the dummy node.
    this->head_->item_ = item;

    ACE_Node<T> *curr = this->head_;

    while (!(curr->next_->item_ == item))
      curr = curr->next_;

    if (curr->next_ == this->head_)
      return -1; // Item was not found.
    else
    {
      ACE_Node<T> *temp = curr->next_;
      // Skip over the node that we're deleting.
      curr->next_ = temp->next_;
      --this->cur_size_;
      ACE_DES_FREE_TEMPLATE (temp,
                             this->allocator_->free,
                             ACE_Node,
                             <T>);
      return 0;
    }
}

  
  
  /// Returns 1 if the container is empty, otherwise returns 0.
  /**
   * Constant time is_empty check.
   */
  inline  int is_empty (void) const
  {
    ACE_TRACE ("is_empty");
    return this->head_ == this->head_->next_;
  }

  inline int is_full (void) const
  {
    ACE_TRACE ("is_full");
    return 0; // We should implement a "node of last resort for this..."
  }

  // = Classic unordered set operations.

  /// Linear insertion of an item.
  /**
   * Insert @a new_item into the set (doesn't allow duplicates).
   * Returns -1 if failures occur, 1 if item is already present, else
   * 0.
   */


  /// Reset the <ACE_Unbounded_Set> to be empty.
  /**
   * Delete the nodes of the set.
   */
  inline void reset (void)
  {
    ACE_TRACE ("reset");
  
    this->delete_nodes ();
  }
//  void reset (void);

  // = STL-styled unidirectional iterator factory.
//  ACE_Unbounded_Set_Iterator<T> begin (void);
  inline ACE_Unbounded_Set_Iterator<T> begin (void)
  {
  // ACE_TRACE ("begin");
    return ACE_Unbounded_Set_Iterator<T> (*this);
  }
  
  inline ACE_Unbounded_Set_Iterator<T> end (void)
  {
  // ACE_TRACE ("end");
    return ACE_Unbounded_Set_Iterator<T> (*this, 1);
  }

//  ACE_Unbounded_Set_Iterator<T> end (void);

  /// Declare the dynamic allocation hooks.
  
  void *operator new (size_t bytes) { return ::new char[bytes]; } 
  void operator delete (void *ptr) { delete [] ((char *) ptr); }

private:
  /// Delete all the nodes in the Set.
  //void delete_nodes (void);
  void delete_nodes (void)
  {
    ACE_Node<T> *curr = this->head_->next_;
  
    // Keep looking until we've hit the dummy node.
  
    while (curr != this->head_)
      {
        ACE_Node<T> *temp = curr;
        curr = curr->next_;
        ACE_DES_FREE_TEMPLATE (temp,
                               this->allocator_->free,
                               ACE_Node, <T>);
        --this->cur_size_;
      }
  
    // Reset the list to be a circular list with just a dummy node.
    this->head_->next_ = this->head_;
  }

  void copy_nodes (const ACE_Unbounded_Set<T> &us)
  {
    for (ACE_Node<T> *curr = us.head_->next_;
         curr != us.head_;
         curr = curr->next_)
      this->insert_tail (curr->item_);
  }
  /// Copy nodes into this set.
//  void copy_nodes (const ACE_Unbounded_Set<T> &);

  /// Head of the linked list of Nodes.
  ACE_Node<T> *head_;

  /// Current size of the set.
  size_t cur_size_;

  /// Allocation strategy of the set.
  ACE_Allocator *allocator_;
};

//typedef ACE_Unbounded_Set<char *> ACE_Char_Unbounded_Set;
//typedef ACE_Unbounded_Set_Iterator<char *> ACE_Char_Unbounded_Set_Iterator;
//typedef ACE_Unbounded_Set_Const_Iterator<char *> ACE_Char_Unbounded_Set_Const_Iterator;

#endif /* ACE_UNBOUNDED_SET_H */
