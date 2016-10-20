// -*- C++ -*-

//=============================================================================
/**
 *  @file    Addr.h
 *
 *  Addr.h,v 4.21 2003/07/19 19:04:10 dhinton Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//=============================================================================

#ifndef ACE_ADDR_H
#define ACE_ADDR_H

#include "macros.h"
/**
 * @class ACE_Addr
 *
 * @brief Defines the base class for the "address family independent"
 * address format.
 */
class ACE_Addr
{
public:
  // = Initialization and termination methods.
  /// Initializes instance variables.
  ACE_Addr (int type = -1,
            int size = -1);

  /// Destructor.
  virtual ~ACE_Addr (void);

  inline virtual void *get_addr (void) const
  {
    return 0;
  }
  
  inline virtual void set_addr (void *, int)
  {
  }
  
  inline int 
  operator == (const ACE_Addr &sap) const
  {
    return (sap.addr_type_ == this->addr_type_ &&
  	  sap.addr_size_ == this->addr_size_   );
  }
  
  inline int
  operator != (const ACE_Addr &sap) const
  {
    return (sap.addr_type_ != this->addr_type_ ||
  	  sap.addr_size_ != this->addr_size_   );
  }
  
  // Return the size of the address. 
  
  inline int get_size (void) const
  {
    return this->addr_size_;
  }
  
  // Sets the size of the address. 
  
  inline void set_size (int size)
  {
    this->addr_size_ = size;
  }
  
  // Return the type of the address. 
  
  inline int get_type (void) const
  {
    return this->addr_type_;
  }
  
  // Set the type of the address. 
  
  inline void set_type (int type)
  {
    this->addr_type_ = type;
  }
  
  inline unsigned long  hash (void) const
  {
    return 0;
  }

  /// Initializes instance variables.
  void base_set (int type, int size);

  /// Wild-card address.
  static const ACE_Addr sap_any;


  /// Dump the state of an object.
  void dump (void) const;

  /// Declare the dynamic allocation hooks.
  ACE_ALLOC_HOOK_DECLARE;

protected:
  /// e.g., AF_UNIX, AF_INET, AF_SPIPE, etc.
  int addr_type_;

  /// Number of bytes in the address.
  int addr_size_;
};

#endif /* ACE_ADDR_H */
