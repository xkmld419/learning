// Malloc.cpp,v 4.62 2005/11/29 07:39:36 jwillemsen Exp

#include "Malloc.h"
#include "Malloc_Base.h"
#include <string.h>




// Process-wide ACE_Allocator.
ACE_Allocator *ACE_Allocator::allocator_ = 0;

// Controls whether the Allocator is deleted when we shut down (we can
// only delete it safely if we created it!)  This is no longer used;
// see ACE_Allocator::instance (void).
int ACE_Allocator::delete_allocator_ = 0;

void
ACE_Control_Block::ACE_Malloc_Header::dump (void) const
{

}

void
ACE_Control_Block::print_alignment_info (void)
{

}

void
ACE_Control_Block::dump (void) const
{

}

ACE_Control_Block::ACE_Name_Node::ACE_Name_Node (void)
{

}

ACE_Control_Block::ACE_Name_Node::ACE_Name_Node (const char *name,
                                                 char *name_ptr,
                                                 char *pointer,
                                                 ACE_Name_Node *next)
  : name_ (name_ptr),
    pointer_ (pointer),
    next_ (next),
    prev_ (0)
{
  char *n = this->name_;
  strcpy (n, name);
  if (next != 0)
    next->prev_ = this;
}

const char *
ACE_Control_Block::ACE_Name_Node::name (void) const
{
  const char *c = this->name_;
  return c;
}

ACE_Control_Block::ACE_Malloc_Header::ACE_Malloc_Header (void)
  : next_block_ (0),
    size_ (0)
{
}

void
ACE_Control_Block::ACE_Name_Node::dump (void) const
{

}


