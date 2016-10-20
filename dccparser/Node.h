#ifndef ACE_NODE_H
#define ACE_NODE_H

#include "macros.h"

// Forward declarations.
// Forward declarations.
template <class T> class ACE_Unbounded_Set;
template <class T> class ACE_Unbounded_Set_Iterator;
template <class T> class ACE_Unbounded_Set_Const_Iterator;
template <class T> class ACE_Unbounded_Queue;
template <class T> class ACE_Unbounded_Queue_Iterator;
template <class T> class ACE_Unbounded_Queue_Const_Iterator;
template <class T> class ACE_Unbounded_Stack;
template <class T> class ACE_Unbounded_Stack_Iterator;

/**
 * @class ACE_Node
 *
 * @brief Implementation element in a Queue, Set, and Stack.
 */

template<class T>
class ACE_Node
{
public:
  friend class ACE_Unbounded_Queue<T>;
  friend class ACE_Unbounded_Queue_Iterator<T>;
  friend class ACE_Unbounded_Queue_Const_Iterator<T>;
  friend class ACE_Unbounded_Set<T>;
  friend class ACE_Unbounded_Set_Iterator<T>;
  friend class ACE_Unbounded_Set_Const_Iterator<T>;
  friend class ACE_Unbounded_Stack<T>;
  friend class ACE_Unbounded_Stack_Iterator<T>;

# if ! defined (ACE_HAS_BROKEN_NOOP_DTORS)
  ~ACE_Node (void)
  {
  }
# endif 
//  void * operator new (size_t bytes) { return ::new char[bytes]; } 
//  void operator delete (void *ptr) { delete [] ((char *) ptr); }


private:

  // = Initialization methods
  ACE_Node (const T &i, ACE_Node<T> *n)
    : next_ (n),
      item_ (i)
  {
  }
  
  ACE_Node (ACE_Node<T> *n=0, int = 0)
    : next_ (n)
  {
  }
  
  ACE_Node (const ACE_Node<T> &s)
    : next_ (s.next_),
      item_ (s.item_)
  {
  }
private:
  /// Not possible
  void operator= (const ACE_Node<T> &);

private:
  /// Pointer to next element in the list of <ACE_Node>s.
  ACE_Node<T> *next_;

  /// Current value of the item in this node.
  T item_;
};

/* class ACE_Node
{
public:
  friend class ACE_Unbounded_Queue<char *>;
  friend class ACE_Unbounded_Queue_Iterator<char *>;
  friend class ACE_Unbounded_Queue_Const_Iterator<char *>;
  friend class ACE_Unbounded_Set<char *>;
  friend class ACE_Unbounded_Set_Iterator<char *>;
  friend class ACE_Unbounded_Set_Const_Iterator<char *>;
  friend class ACE_Unbounded_Stack<char *>;
  friend class ACE_Unbounded_Stack_Iterator<char *>;

# if ! defined (ACE_HAS_BROKEN_NOOP_DTORS)
  ~ACE_Node (void)
  {
  	ACE_TRACE ("~ACE_Node");
  }
# endif 
  void * operator new (size_t bytes) { return ::new char[bytes]; } 
  void operator delete (void *ptr) { delete [] ((char *) ptr); }


private:

  // = Initialization methods
  ACE_Node (char* i, ACE_Node *n)
    : next_ (n),
      item_ (i)
  {
     ACE_TRACE ("ACE_Node");
  }
  
  ACE_Node (ACE_Node *n=0, int = 0)
    : next_ (n)
  {
     ACE_TRACE ("ACE_Node");
  }
  
  ACE_Node (const ACE_Node &s)
    : next_ (s.next_),
      item_ (s.item_)
  {
    ACE_TRACE ("ACE_Node");
  }
private:
  /// Not possible
  void operator= (const ACE_Node &);

private:
  /// Pointer to next element in the list of <ACE_Node>s.
  ACE_Node *next_;

  /// Current value of the item in this node.
  char *item_;
};*/


#endif /* ACE_NODE_H */
