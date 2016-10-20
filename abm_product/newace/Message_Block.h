                                 // -*- C++ -*-

//==========================================================================
/**
 *  @file    Message_Block.h
 *
 *  Message_Block.h,v 4.111 2006/02/24 15:02:52 jwillemsen Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//==========================================================================

#ifndef ACE_MESSAGE_BLOCK_H
#define ACE_MESSAGE_BLOCK_H



#include "Default_Constants.h"
#include "macros.h"
#include "Time_Value.h"
#include "Log_Msg.h"
#include "Trace.h"


// Forward declaration.
class ACE_Allocator;
class ACE_Data_Block;
class ACE_Lock;


typedef int ACE_Message_Type;
typedef unsigned long Message_Flags;


/**
 * @class ACE_Data_Block
 *
 * @brief Stores the data payload that is accessed via one or more
 * ACE_Message_Block's.
 *
 * This data structure is reference counted to maximize
 * sharing.  It also contains the <locking_strategy_> (which
 * protects the reference count from race conditions in
 * concurrent programs) and the <allocation_strategy_> (which
 * determines what memory pool is used to allocate the memory).
 */
class  ACE_Data_Block
{
  enum
  {
    /// Don't delete the data on exit since we don't own it.
    DONT_DELETE = 01,
    /// user defined flags start here
    USER_FLAGS = 0x1000
  };

public:
  // = Initialization and termination methods.
  /// Default "do-nothing" constructor.
  ACE_Data_Block (void);

  /// Initialize.
  ACE_Data_Block (size_t size,
                  ACE_Message_Type msg_type,
                  const char *msg_data,
                  ACE_Allocator *allocator_strategy,
                  ACE_Lock *locking_strategy,
                  Message_Flags flags,
                  ACE_Allocator *data_block_allocator);

  /// Delete all the resources held in the message.
  virtual ~ACE_Data_Block (void);

  inline int
  reference_count_i (void) const
  {
    return reference_count_;
  }
  
  inline ACE_Message_Type msg_type (void) const
  {
    ACE_TRACE ("msg_type");
    return this->type_;
  }
  
  inline char *
  mark (void) const
  {
    ACE_TRACE ("mark");
    return this->base_ + this->cur_size_;
  }
 
  inline char *
  base (void) const
  {
    ACE_TRACE ("base");
    return this->base_;
  }
  
  inline size_t
  size (void) const
  {
    ACE_TRACE ("size");
    return this->cur_size_;
  }
  
  inline size_t
  capacity (void) const
  {
    ACE_TRACE ("capacity");
    return this->max_size_;
  }

  inline ACE_Allocator *
  allocator_strategy (void) const
  {
    ACE_TRACE ("allocator_strategy");
    return this->allocator_strategy_;
  }
  
  
  inline char *
  end (void) const
  {
    ACE_TRACE ("end");
    return this->base_ + this->max_size_;
  }
  
  inline Message_Flags
  set_flags (Message_Flags more_flags)
  {
    ACE_TRACE ("set_flags");
    // Later we might mask more_glags so that user can't change internal
    // ones: more_flags &= ~(USER_FLAGS -1).
    return ACE_SET_BITS (this->flags_, more_flags);
  }
  
  inline Message_Flags
  clr_flags (Message_Flags less_flags)
  {
    ACE_TRACE ("clr_flags");
    // Later we might mask more_flags so that user can't change internal
    // ones: less_flags &= ~(USER_FLAGS -1).
    return ACE_CLR_BITS (this->flags_, less_flags);
  }

  inline Message_Flags
  flags (void) const
  {
    ACE_TRACE ("flags");
     return this->flags_;
  }
  
  inline ACE_Allocator*
  data_block_allocator (void) const
  {
    ACE_TRACE ("data_block_allocator");
    return this->data_block_allocator_;
  }

  inline ACE_Lock *
  locking_strategy (void)
  {
    ACE_TRACE ("locking_strategy");
    return this->locking_strategy_;
  }
  
  inline ACE_Lock *
  locking_strategy (ACE_Lock *nls)
  {
    ACE_TRACE ("locking_strategy");
    ACE_Lock *ols = this->locking_strategy_;
  
    this->locking_strategy_ = nls;
    return ols;
  }


  /// Set type of the message.
#ifndef DEF_LINUX
  void msg_type (ACE_Message_Type type);
#else
  void msg_type (ACE_Message_Type type)
  {
        this->type_ = type;
  };
#endif


  /// Set message data pointer (doesn't reallocate).
  void base (char *data,
             size_t size,
             Message_Flags mflags = DONT_DELETE);


  /**
   * Return a pointer to 1 past the end of the allotted data in a message.
   * The allotted data may be less than allocated data if <size()> is passed
   * an argument less than <capacity()>.
   */

  // = Message size is the total amount of space alloted.

  /// Get the total amount of allotted space in the message.  The amount of
  /// allotted space may be less than allocated space.
  /// Set the total amount of space in the message.  Returns 0 if
  /// successful, else -1.
  int size (size_t length);


  /**
   * Return an exact "deep copy" of the message, i.e., create fresh
   * new copies of all the Data_Blocks and continuations.
   * Notice that Data_Blocks can act as "Prototypes", i.e. derived
   * classes can override this method and create instances of
   * themselves.
   */
  virtual ACE_Data_Block *clone (Message_Flags mask = 0) const;

  /**
   * As clone above, but it does not copy the contents of the buffer,
   * i.e., create a new Data_Block of the same dynamic type, with the
   * same allocator, locking_strategy, and with the same amount of
   * storage available but the buffer is unitialized.
   */
  virtual ACE_Data_Block *clone_nocopy (Message_Flags mask = 0) const;

  /// Return a "shallow" copy that increments our reference count by 1.
  ACE_Data_Block *duplicate (void);

  /**
   * Decrease the shared reference count by 1.  If the reference count
   * is > 0 then return this; else if reference count == 0 then delete
   * <this> and <mb> and return 0.  Behavior is undefined if reference
   * count < 0.
   */
  ACE_Data_Block *release (ACE_Lock *lock = 0);



  /// Dump the state of an object.
  void dump (void) const;

  /// Get the current reference count.
  int reference_count (void) const;


protected:
  /// Internal release implementation
  ACE_Data_Block *release_i (void);


  /**
   * Decrease the reference count, but don't delete the object.
   * Returns 0 if the object should be removed.
   * If <lock> is equal to the locking strategy then we assume that
   * the lock is beign held by the current thread; this is used to
   * release all the data blocks in a chain while holding a single
   * lock.
   */
  friend class ACE_Message_Block;
  ACE_Data_Block *release_no_delete (ACE_Lock *lock);

  /// Type of message.
  ACE_Message_Type type_;

  /// Current size of message block.
  size_t cur_size_;

  /// Total size of buffer.
  size_t max_size_;

  /// Misc flags (e.g., DONT_DELETE and USER_FLAGS).
  Message_Flags flags_;

  /// Pointer To beginning of message payload.
  char *base_;

  // = Strategies.
  /**
   * Pointer to the allocator defined for this ACE_Data_Block.  Note
   * that this pointer is shared by all owners of this
   * ACE_Data_Block.
   */
  ACE_Allocator *allocator_strategy_;

  /**
   * Pointer to the locking strategy defined for this
   * ACE_Data_Block.  This is used to protect regions of code that
   * access shared ACE_Data_Block state.  Note that this lock is
   * shared by all owners of the ACE_Data_Block's data.
   */
  ACE_Lock *locking_strategy_;

  /**
   * Reference count for this ACE_Data_Block, which is used to avoid
   * deep copies (i.e., <clone>).  Note that this pointer value is
   * shared by all owners of the <Data_Block>'s data, i.e., all the
   * ACE_Message_Blocks.
   */
  int reference_count_;

  /// The allocator use to destroy ourselves.
  ACE_Allocator *data_block_allocator_;

private:
  // = Disallow these operations.
  ACE_Data_Block &operator= (const ACE_Data_Block &);
  ACE_Data_Block (const ACE_Data_Block &);
};

/**
 * @class ACE_Message_Block
 *
 * @brief Stores messages for use throughout ACE (particularly
 * in an ACE_Message_Queue).
 *
 * An ACE_Message_Block is modeled after the message data
 * structures used in System V STREAMS.  Its purpose is to
 * enable efficient manipulation of arbitrarily large messages
 * without incurring much memory copying overhead.  Here are the
 * main characteristics of an ACE_Message_Block:
 * - Contains a pointer to a reference-counted
 *   ACE_Data_Block, which in turn points to the actual data
 *   buffer.  This allows very flexible and efficient sharing of
 *   data by multiple ACE_Message_Block objects.
 * - One or more ACE_Message_Blocks can be linked to form a
 *    ``fragment chain.''
 * - ACE_Message_Blocks can be linked together in a doubly linked fashion
 *   to form a queue of messages (this is how ACE_Message_Queue works).
 *
 * @see C++NPv1, section 4.2; APG, section 12.3.2.
 */
class  ACE_Message_Block
{

public:

  friend class ACE_Data_Block;

  typedef int ACE_Message_Type;
  typedef unsigned long Message_Flags;

  enum
  {
    /// Don't delete the data on exit since we don't own it.
    DONT_DELETE = 01,
    /// user defined flags start here
    USER_FLAGS = 0x1000
  };

  enum
  {
    // = Data and proto
    /// Undifferentiated data message
    MB_DATA     = 0x01,
    /// Undifferentiated protocol control
    MB_PROTO    = 0x02,

    // = Control messages
    /// Line break (regular and priority)
    MB_BREAK    = 0x03,
    /// Pass file pointer
    MB_PASSFP   = 0x04,
    /// Post an event to an event queue
    MB_EVENT    = 0x05,
    /// Generate process signal
    MB_SIG      = 0x06,
    /// ioctl; set/get params
    MB_IOCTL    = 0x07,
    /// Set various stream head options
    MB_SETOPTS  = 0x08,

    // = Control messages
    /// Acknowledge ioctl (high priority; go to head of queue)
    MB_IOCACK   = 0x81,
    /// Negative ioctl acknowledge
    MB_IOCNAK   = 0x82,
    /// Priority proto message
    MB_PCPROTO  = 0x83,
    /// Generate process signal
    MB_PCSIG    = 0x84,
    /// Generate read notification
    MB_READ     = 0x85,
    /// Flush your queues
    MB_FLUSH    = 0x86,
    /// Stop transmission immediately
    MB_STOP     = 0x87,
    /// Restart transmission after stop
    MB_START    = 0x88,
    /// Line disconnect
    MB_HANGUP   = 0x89,
    /// Fatal error used to set u.u_error
    MB_ERROR    = 0x8a,
    /// Post an event to an event queue
    MB_PCEVENT  = 0x8b,

    // = Message class masks
    /// Normal priority message mask
    MB_NORMAL   = 0x00,
    /// High priority control message mask
    MB_PRIORITY = 0x80,
    /// User-defined message mask
    MB_USER     = 0x200
  };



  // = Initialization and termination.
  /// Create an empty message.
  ACE_Message_Block (ACE_Allocator *message_block_allocator = 0);

  inline int
  reference_count (void) const
  {
    return data_block () ? data_block ()->reference_count () : 0;
  }
  
  inline Message_Flags
  set_flags (Message_Flags more_flags)
  {
    ACE_TRACE ("set_flags");
    return this->data_block ()->set_flags (more_flags);
  }
  
  inline Message_Flags
  clr_flags (Message_Flags less_flags)
  {
    ACE_TRACE ("clr_flags");
    return this->data_block ()->clr_flags (less_flags);
  }
  
  inline Message_Flags
  flags (void) const
  {
    ACE_TRACE ("flags");
     return this->data_block ()->flags ();
  }
  
  // Return the length of the "active" portion of the message.
  
  inline size_t
  length (void) const
  {
    ACE_TRACE ("length");
    return this->wr_ptr_ - this->rd_ptr_;
  }
  
  // Sets the length of the "active" portion of the message.  This is
  // defined as the offset from RD_PTR to WR_PTR.
  
  inline void
  length (size_t len)
  {
    ACE_TRACE ("length");
    this->wr_ptr_ = this->rd_ptr_ + len;
  }
  
  // Return the length of the potential size of the message.
  
  inline size_t
  size (void) const
  {
    ACE_TRACE ("size");
    return this->data_block ()->size ();
  }
  
  inline size_t
  capacity (void) const
  {
    ACE_TRACE ("capacity");
    return this->data_block ()->capacity ();
  }
  
  
  inline ACE_Message_Type
  msg_type (void) const
  {
    ACE_TRACE ("msg_type");
    return this->data_block ()->msg_type ();
  }
  
  inline void
  msg_type (ACE_Message_Type t)
  {
    ACE_TRACE ("msg_type");
    this->data_block ()->msg_type (t);
  }
  
  inline ACE_Message_Type
  msg_class (void) const
  {
    ACE_TRACE ("msg_class");
  
    if (this->msg_type () < MB_PRIORITY)
      return MB_NORMAL;
    else if (this->msg_type () < MB_USER)
      return MB_PRIORITY;
    else
      return MB_USER;
  }
  
  inline int
  is_data_msg (void) const
  {
    ACE_TRACE ("is_data_msg");
    ACE_Message_Type mt = this->msg_type ();
    return
      mt == MB_DATA
      || mt == MB_PROTO
      || mt == MB_PCPROTO;
  }
  
  inline unsigned long
  msg_priority (void) const
  {
    ACE_TRACE ("msg_priority");
    return this->priority_;
  }
  
  inline void
  msg_priority (unsigned long pri)
  {
    ACE_TRACE ("msg_priority");
    this->priority_ = pri;
  }
  
  inline const ACE_Time_Value &
  msg_execution_time (void) const
  {
    ACE_TRACE ("msg_execution_time (void)");
  #if defined (ACE_HAS_TIMED_MESSAGE_BLOCKS)
    return this->execution_time_;
  #else
    return ACE_Time_Value::zero;
  #endif /* ACE_HAS_TIMED_MESSAGE_BLOCKS */
  }
  
  inline void
  msg_execution_time (const ACE_Time_Value &et)
  {
    ACE_TRACE ("msg_execution_time (const ACE_Time_Value & et)");
  #if defined (ACE_HAS_TIMED_MESSAGE_BLOCKS)
    this->execution_time_ = et;
  #else
    ACE_UNUSED_ARG (et);
  #endif /* ACE_HAS_TIMED_MESSAGE_BLOCKS */
  }
  
  inline const ACE_Time_Value &
  msg_deadline_time (void) const
  {
    ACE_TRACE ("msg_deadline_time (void)");
  
  #if defined (ACE_HAS_TIMED_MESSAGE_BLOCKS)
    return this->deadline_time_;
  #else
    return ACE_Time_Value::max_time; // absolute time of deadline
  #endif /* ACE_HAS_TIMED_MESSAGE_BLOCKS */
  }
  
  inline void
  msg_deadline_time (const ACE_Time_Value &dt)
  {
    ACE_TRACE ("msg_deadline_time (const ACE_Time_Value & et)");
  #if defined (ACE_HAS_TIMED_MESSAGE_BLOCKS)
    this->deadline_time_ = dt;
  #else
    ACE_UNUSED_ARG (dt);
  #endif /* ACE_HAS_TIMED_MESSAGE_BLOCKS */
  }
  
  inline void
  access_allocators (ACE_Allocator *& allocator_strategy,
                                        ACE_Allocator *& data_block_allocator,
                                        ACE_Allocator *& message_block_allocator)
  {
    allocator_strategy =
      this->data_block_->allocator_strategy_;
    data_block_allocator =
      this->data_block_->data_block_allocator_;
    message_block_allocator =
      this->message_block_allocator_;
  }
  
  inline char *
  base (void) const
  {
    ACE_TRACE ("base");
    return this->data_block ()->base ();
  }
  
  inline void
  base (char *msg_data,
                           size_t msg_length,
                           Message_Flags msg_flags)
  {
    ACE_TRACE ("base");
    this->rd_ptr_ = 0;
    this->wr_ptr_ = 0;
    this->data_block ()->base (msg_data, msg_length, msg_flags);
  }
  
  inline char *
  rd_ptr (void) const
  {
    ACE_TRACE ("rd_ptr");
    return this->base () + this->rd_ptr_;
  }
  
  inline void
  wr_ptr (char *new_ptr)
  {
    ACE_TRACE ("wr_ptr");
    this->wr_ptr_ = new_ptr - this->base ();
  }
  
  // Return a pointer to 1 past the end of the data buffer.
  
  
  inline char *
  mark (void) const
  {
    ACE_TRACE ("mark");
    return this->data_block ()->mark ();
  }
  
  
  inline char *
  end (void) const
  {
    ACE_TRACE ("end");
    return this->data_block ()->end ();
  }
  
  
  inline void
  rd_ptr (char *new_ptr)
  {
    ACE_TRACE ("rd_ptr");
    this->rd_ptr_ = new_ptr - this->base ();
  }
  
  inline void
  rd_ptr (size_t n)
  {
    ACE_TRACE ("rd_ptr");
    this->rd_ptr_ += n;
  }
  
  inline char *
  wr_ptr (void) const
  {
    ACE_TRACE ("wr_ptr");
    return this->base () + this->wr_ptr_;
  }
  
  inline void
  wr_ptr (size_t n)
  {
    ACE_TRACE ("wr_ptr");
    this->wr_ptr_ += n;
  }
  
  inline void
  reset (void)
  {
    ACE_TRACE ("reset");
    this->rd_ptr_ = 0;
    this->wr_ptr_ = 0;
  }
  
  inline size_t
  space (void) const
  {
    ACE_TRACE ("space");
    return this->mark () - this->wr_ptr ();
  }
  
  inline ACE_Data_Block *
  replace_data_block (ACE_Data_Block *db)
  {
    ACE_TRACE ("replace_data_block");
    ACE_Data_Block *old = this->data_block_;
    this->data_block_ = db;
  
    if (db != 0)
      {
        // Set the read and write pointers in the <Message_Block> to point
        // to the buffer in the <ACE_Data_Block>.
        this->rd_ptr (this->data_block ()->base ());
        this->wr_ptr (this->data_block ()->base ());
      }
  
    return old;
  }
  
  inline void
  cont (ACE_Message_Block *cont_msg)
  {
    ACE_TRACE ("cont");
    this->cont_ = cont_msg;
  }
  
  inline ACE_Message_Block *
  cont (void) const
  {
    ACE_TRACE ("cont");
    return this->cont_;
  }
  
  inline void
  reset_allocators (ACE_Allocator *allocator_strategy,
                                       ACE_Allocator *data_block_allocator,
                                       ACE_Allocator *message_block_allocator)
  {
    this->data_block_->allocator_strategy_ =
      allocator_strategy;
    this->data_block_->data_block_allocator_ =
      data_block_allocator;
    this->message_block_allocator_ =
      message_block_allocator;
  
    if (this->cont () != 0)
      this->cont ()->reset_allocators (allocator_strategy,
                                       data_block_allocator,
                                       message_block_allocator);
  }
  
  inline void
  next (ACE_Message_Block *next_msg)
  {
    ACE_TRACE ("next");
    this->next_ = next_msg;
  }
  
  inline ACE_Message_Block *
  next (void) const
  {
    ACE_TRACE ("next");
    return this->next_;
  }
  
  inline void
  prev (ACE_Message_Block *next_msg)
  {
    ACE_TRACE ("prev");
    this->prev_ = next_msg;
  }
  
  inline ACE_Message_Block *
  prev (void) const
  {
    ACE_TRACE ("prev");
    return this->prev_;
  }
  
  
  inline ACE_Lock *
  locking_strategy (void)
  {
    ACE_TRACE ("locking_strategy");
    return this->data_block ()->locking_strategy ();
  }
  
  inline ACE_Lock *
  locking_strategy (ACE_Lock *nls)
  {
    ACE_TRACE ("locking_strategy");
    ACE_Lock *ols = this->data_block ()->locking_strategy ();
    this->data_block ()->locking_strategy (nls);
    return ols;
  }

  /**
   * Create an ACE_Message_Block that owns the specified ACE_Data_Block
   * without copying it. If the @a flags is set to @c DONT_DELETE we
   * don't delete the ACE_Data_Block. It is left to the client's
   * responsibility to take care of the memory allocated for the
   * data_block
   */
  ACE_Message_Block (ACE_Data_Block *,
                     Message_Flags flags = 0,
                     ACE_Allocator *message_block_allocator = 0);

  inline ACE_Data_Block *data_block (void) const
  {
    ACE_TRACE ("data_block");
    return this->data_block_;
  }

  inline Message_Flags
  set_self_flags (Message_Flags more_flags)
  {
    ACE_TRACE ("set_self_flags");
  // Later we might mask more_glags so that user can't change internal
  // ones: more_flags &= ~(USER_FLAGS -1).
    return ACE_SET_BITS (this->flags_, more_flags);
  }

  inline Message_Flags
  clr_self_flags (Message_Flags less_flags)
  {
    ACE_TRACE ("clr_self_flags");
  // Later we might mask more_flags so that user can't change internal
  // ones: less_flags &= ~(USER_FLAGS -1).
   return ACE_CLR_BITS (this->flags_, less_flags);
  }
  
  inline Message_Flags
  self_flags (void) const
  {
    ACE_TRACE ("self_flags");
    return this->flags_;
  }

  /**
   * Create an ACE_Message_Block that refers to @a data without
   * copying it. The @a data memory will not be freed when this block is
   * destroyed; memory management of @a data is left to the caller.
   * Note that the @c size of the new ACE_Message_Block will be @a size, but
   * the @c length will be 0 until the write pointer is changed.
   */
  ACE_Message_Block (const char *data,
                     size_t size = 0,
                     unsigned long priority = ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY);

  /**
   * Create an initialized message of type @a type containing @a size
   * bytes.  The @a cont argument initializes the continuation field in
   * the ACE_Message_Block.  If @a data == 0 then this block allocates and
   * owns the block's memory, using @a allocator to get the data if it's
   * non-0.  If @a data != 0 then this block refers to that memory until
   * this this block ceases to exist; this object will not free @a data on
   * destruction.  If @a locking_strategy is non-0 then this is used
   * to protect regions of code that access shared state (e.g.,
   * reference counting) from  race conditions.  Note that the @c size
   * of the ACE_Message_Block will be @a size, but the @c length will be 0
   * until the write pointer is set. The @a data_block_allocator is used to
   * allocate the data blocks while the @a allocator_strategy is used
   * to allocate the buffers contained by those. The
   * @a message_block_allocator is used to allocate new ACE_Message_Block
   * objects when the duplicate() method is called. If a
   * @a message_block_allocator is given, this ACE_Message_Block and
   * future ACE_Message_Block objects created by duplicate() will be
   * freed using this allocator when they are released.
   * @note If you use this allocator, the ACE_Message_Block you created
   * should have been created using this allocator because it will be
   * released to the same allocator.
   */
  ACE_Message_Block (size_t size,
                     ACE_Message_Type type = MB_DATA,
                     ACE_Message_Block *cont = 0,
                     const char *data = 0,
                     ACE_Allocator *allocator_strategy = 0,
                     ACE_Lock *locking_strategy = 0,
                     unsigned long priority = ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY,
                     const ACE_Time_Value &execution_time = ACE_Time_Value::zero,
                     const ACE_Time_Value &deadline_time = ACE_Time_Value::max_time,
                     ACE_Allocator *data_block_allocator = 0,
                     ACE_Allocator *message_block_allocator = 0);

  /**
   * A copy constructor. This constructor is a bit different. If the
   * incoming Message Block has a data block from the stack this
   * constructor does a deep copy ie. allocates a new data block on
   * the heap and does a copy of the data from the incoming message
   * block. As a final note, the alignment information is used to
   * align the data block if it is created afresh. If the incoming
   * <mb> has a data block has a data block allocated from the heap,
   * then this constructor just duplicates (ie. a shallow copy) the
   * data block of the incoming <mb>.
   */
  ACE_Message_Block (const ACE_Message_Block &mb,
                     size_t align);

  /**
   * Create a Message Block that assumes it has ownership of <data>,
   * but in reality it doesnt (i.e., cannot delete it since it didn't
   * malloc it!).  Note that the  <size> of the <Message_Block> will
   * be <size>, but the <length>  will be 0 until <wr_ptr> is set.
   */
  int init (const char *data,
            size_t size = 0);

  /**
   * Create an initialized message of type <type> containing <size>
   * bytes.  The <cont> argument initializes the continuation field in
   * the <Message_Block>.  If <data> == 0 then we create and own the
   * <data>, using <allocator> to get the data if it's non-0.  If
   * <data> != 0 we assume that we have ownership of the <data> till
   * this object ceases to exist  (and don't delete it during
   * destruction).  If <locking_strategy> is non-0 then this is used
   * to protect regions of code that access shared state (e.g.,
   * reference counting) from race conditions.  Note that the <size>
   * of the <Message_Block> will be <size>, but the <length> will be 0
   * until  <wr_ptr> is set. The <data_block_allocator> is use to
   * allocate the data blocks while the <allocator_strategy> is used
   * to allocate the buffers  contained by those.
   */
  int init (size_t size,
            ACE_Message_Type type = MB_DATA,
            ACE_Message_Block *cont = 0,
            const char *data = 0,
            ACE_Allocator *allocator_strategy = 0,
            ACE_Lock *locking_strategy = 0,
            unsigned long priority = ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY,
            const ACE_Time_Value &execution_time = ACE_Time_Value::zero,
            const ACE_Time_Value &deadline_time = ACE_Time_Value::max_time,
            ACE_Allocator *data_block_allocator = 0,
            ACE_Allocator *message_block_allocator = 0);

  /**
   * Delete all the resources held in the message.
   *
   * Note that <release()> is designed to release the continuation
   * chain; the destructor is not. See <release()> for details.
   */
  virtual ~ACE_Message_Block (void);

  // = Message Type accessors and mutators.


  // = Data Block flag accessors and mutators.
  /// Bitwise-or the <more_flags> into the existing message flags and
  /// return the new value.
  /* @todo: I think the following set of methods could not be used at
   *  all. May be they are useless. Let us have it so that we dont
   *  mess up memory management of the Message_Block. Somebody correct
   *  me if I am totally totally wrong..
   */
  // = Deep copy and shallow copy methods.

  /// Return an exact "deep copy" of the message, i.e., create fresh
  /// new copies of all the Data_Blocks and continuations.
  virtual ACE_Message_Block *clone (Message_Flags mask = 0) const;

  /// Return a "shallow" copy that increments our reference count by 1.
  virtual ACE_Message_Block *duplicate (void) const;

  /**
   * Return a "shallow" copy that increments our reference count by 1.
   * This is similar to CORBA's <_duplicate> method, which is useful
   * if you want to eliminate lots of checks for NULL <mb> pointers
   * before calling <_duplicate> on them.
   */
  static ACE_Message_Block *duplicate (const ACE_Message_Block *mb);

  /**
   * Decrease the shared ACE_Data_Block's reference count by 1.  If the
   * ACE_Data_Block's reference count goes to 0, it is deleted.
   * In all cases, this ACE_Message_Block is deleted - it must have come
   * from the heap, or there will be trouble.
   *
   * release() is designed to release the continuation chain; the
   * destructor is not.  If we make the destructor release the
   * continuation chain by calling release() or delete on the message
   * blocks in the continuation chain, the following code will not
   * work since the message block in the continuation chain is not off
   * the heap:
   *
   *  ACE_Message_Block mb1 (1024);
   *  ACE_Message_Block mb2 (1024);
   *
   *  mb1.cont (&mb2);
   *
   * And hence, call release() on a dynamically allocated message
   * block. This will release all the message blocks in the
   * continuation chain.  If you call delete or let the message block
   * fall off the stack, cleanup of the message blocks in the
   * continuation chain becomes the responsibility of the user.
   *
   * @retval 0, always, and the object this method was invoked on is no
   *            longer valid.
   */
  virtual ACE_Message_Block *release (void);

  /**
   * This behaves like the non-static method <release>, except that it
   * checks if <mb> is 0.  This is similar to <CORBA::release>, which
   * is useful if you want to eliminate lots of checks for NULL
   * pointers before calling <release> on them.  Returns <mb>.
   */
  static ACE_Message_Block *release (ACE_Message_Block *mb);

  // = Operations on Message data

  /**
   * Copies data into this ACE_Message_Block. Data is copied into the
   * block starting at the current write pointer.
   *
   * @param buf  Pointer to the buffer to copy from.
   * @param n    The number of bytes to copy.
   *
   * @retval 0  on success; the write pointer is advanced by @arg n.
   * @retval -1 if the amount of free space following the write pointer
   *            in the block is less than @arg n. Free space can be checked
   *            by calling space().
   */
  int copy (const char *buf, size_t n);

  /**
   * Copies a 0-terminated character string into this ACE_Message_Block.
   * The string is copied into the block starting at the current write
   * pointer. The 0-terminator is included in the copied data.
   *
   * @param buf  Pointer to the character string to copy from.
   *
   * @retval 0  on success; the write pointer is advanced by the string's
   *            length, including the 0 terminator.
   * @retval -1 if the amount of free space following the write pointer
   *            in the block is less than required to hold the entire string.
   *            Free space can be checked by calling space().
   */
  int copy (const char *buf);

  /// Normalizes data in the top-level <Message_Block> to align with the base,
  /// i.e., it "shifts" the data pointed to by <rd_ptr> down to the <base> and
  /// then readjusts <rt_ptr> to point to <base> and <wr_ptr> to point
  /// to <base> + the length of the moved data.  Returns -1 and does
  /// nothing if the <rd_ptr> is > <wr_ptr>, else 0 on success.
  int crunch (void);

  /// Resets the Message Block data to contain nothing, i.e., sets the
  /// read and write pointers to align with the base.

  /// Access all the allocators in the message block.
  /// @@todo: Not sure whether we would need finer control while
  /// trying to access allocators ie. a method for every allocator.
  /**
   * This method returns the allocators only from the first message
   * block in the chain.
   *
   * @param allocator_strategy Strategy used to allocate the
   *                           underlying buffer
   *
   * @param data_block_allocator Strategy used to allocate the
   *                             underlying data block
   *
   * @param message_block_allocator Strategy used to allocate the
   *                                message block
   */

  /// Reset all the allocators in the message block.
  /// @@todo: Not sure whether we would need finer control while
  /// trying to reset allocators ie. a method for every allocator.
  /**
   * This method resets the allocators in all the message blocks in
   * the chain.
   */

  /// Get the length of the <Message_Block>s, including chained
  /// <Message_Block>s.
  size_t total_length (void) const;

  /// Get the total number of bytes in all <Message_Block>s, including
  /// chained <Message_Block>s.
  size_t total_size (void) const;

  /// Get the total number of bytes and total length in all
  /// <Message_Block>s, including chained <Message_Block>s.
  void total_size_and_length (size_t &mb_size,
                              size_t &mb_length) const;

  /**
   * Set the number of bytes in the top-level <Message_Block>,
   * reallocating space if necessary.  However, the <rd_ptr_> and
   * <wr_ptr_> remain at the original offsets into the buffer, even if
   * it is reallocated.  Returns 0 if successful, else -1.
   */
  int size (size_t length);

  /// Get the number of allocated bytes in all <Message_Block>, including
  /// chained <Message_Block>s.
  size_t total_capacity (void) const;

  //@}

  // = ACE_Data_Block methods.


  /**
   * Set a new data block pointer. The original ACE_Data_Block is released
   * as a result of this call. If you need to keep the original block, call
   * <replace_data_block> instead. Upon return, this ACE_Message_Block
   * holds a pointer to the new ACE_Data_Block, taking over the reference
   * you held on it prior to the call.
   */
  void data_block (ACE_Data_Block *);



  /// Dump the state of an object.
  void dump (void) const;


  /// Declare the dynamic allocation hooks.
//  void *operator new (size_t bytes); 
//  void operator delete (void *ptr);

protected:
  // = Internal initialization methods.
  /// Perform the actual initialization.
  ACE_Message_Block (size_t size,
                     ACE_Message_Type type,
                     ACE_Message_Block *cont,
                     const char *data,
                     ACE_Allocator *allocator_strategy,
                     ACE_Lock *locking_strategy,
                     Message_Flags flags,
                     unsigned long priority,
                     const ACE_Time_Value &execution_time,
                     const ACE_Time_Value &deadline_time,
                     ACE_Data_Block *db,
                     ACE_Allocator *data_block_allocator,
                     ACE_Allocator *message_block_allocator);

  /// Internal release implementation
  /// Returns 1 if the data block has to be destroyed.
  int release_i (ACE_Lock *lock);

  /// Perform the actual initialization.
  int init_i (size_t size,
              ACE_Message_Type type,
              ACE_Message_Block *cont,
              const char *data,
              ACE_Allocator *allocator_strategy,
              ACE_Lock *locking_strategy,
              Message_Flags flags,
              unsigned long priority,
              const ACE_Time_Value &execution_time,
              const ACE_Time_Value &deadline_time,
              ACE_Data_Block *db,
              ACE_Allocator *data_block_allocator,
              ACE_Allocator *message_block_allocator);

  /// Pointer to beginning of next read.
  size_t rd_ptr_;

  /// Pointer to beginning of next write.
  size_t wr_ptr_;

  /// Priority of message.
  unsigned long priority_;

#if defined (ACE_HAS_TIMED_MESSAGE_BLOCKS)
  /// Execution time associated with the message.
  ACE_Time_Value execution_time_;

  /// Absolute deadline time for message.
  ACE_Time_Value deadline_time_;
#endif /* ACE_HAS_TIMED_MESSAGE_BLOCKS */

  // = Links to other ACE_Message_Block *s.
  /// Pointer to next message block in the chain.
  ACE_Message_Block *cont_;

  /// Pointer to next message in the list.
  ACE_Message_Block *next_;

  /// Pointer to previous message in the list.
  ACE_Message_Block *prev_;

  /// Misc flags (e.g., DONT_DELETE and USER_FLAGS).
  Message_Flags flags_;

  /// Pointer to the reference counted data structure that contains the
  /// actual memory buffer.
  ACE_Data_Block *data_block_;

  /// The allocator used to destroy ourselves when release is called
  /// and create new message blocks on duplicate.
  ACE_Allocator *message_block_allocator_;

private:
  // = Disallow these operations for now (use <clone> instead).
  ACE_Message_Block &operator= (const ACE_Message_Block &);
  ACE_Message_Block (const ACE_Message_Block &);
};

/**
 * @class ACE_Dynamic_Message_Strategy
 *
 * @brief An abstract base class which provides dynamic priority
 * evaluation methods for use by the ACE_Dynamic_Message_Queue
 * class or any other class which needs to manage the priorities
 * of a collection of ACE_Message_Block's dynamically.
 *
 * Methods for deadline and laxity based priority evaluation are
 * provided.  These methods assume a specific partitioning of
 * the message priority number into a higher order dynamic bit
 * field and a lower order static priority bit field.  The
 * default partitioning assumes an unsigned dynamic message
 * priority field of 22 bits and an unsigned static message
 * priority field of 10 bits.  This corresponds to the initial
 * values of the static class members.  To provide a different
 * partitioning, assign a different set of values to the static
 * class memebers before using the static member functions.
 */
class  ACE_Dynamic_Message_Strategy
{
public:

  // = Message priority status

  // Values are defined as bit flags so that status combinations may
  // be specified easily.

  enum Priority_Status
  {
    /// Message can still make its deadline
    PENDING     = 0x01,
    /// Message cannot make its deadline
    LATE        = 0x02,
    /// Message is so late its priority is undefined
    BEYOND_LATE = 0x04,
    /// Mask to match any priority status
    ANY_STATUS  = 0x07
  };

  /// Constructor.
  ACE_Dynamic_Message_Strategy (unsigned long static_bit_field_mask,
                                unsigned long static_bit_field_shift,
                                unsigned long dynamic_priority_max,
                                unsigned long dynamic_priority_offset);

  /// Virtual destructor.
  virtual ~ACE_Dynamic_Message_Strategy (void);
  inline unsigned long
  static_bit_field_mask (void) const
  {
    return static_bit_field_mask_;
  }
    // get static bit field mask
  
  inline void
  static_bit_field_mask (unsigned long ul)
  {
    static_bit_field_mask_ = ul;
  }
    // set static bit field mask
  
  inline unsigned long
  static_bit_field_shift (void) const
  {
    return static_bit_field_shift_;
  }
    // get left shift value to make room for static bit field
  
  inline void
  static_bit_field_shift (unsigned long ul)
  {
    static_bit_field_shift_ = ul;
  }
    // set left shift value to make room for static bit field
  
  inline unsigned long
  dynamic_priority_max (void) const
  {
    return dynamic_priority_max_;
  }
    // get maximum supported priority value
  
  inline void
  dynamic_priority_max (unsigned long ul)
  {
    // pending_shift_ depends on dynamic_priority_max_: for performance
    // reasons, the value in pending_shift_ is (re)calculated only when
    // dynamic_priority_max_ is initialized or changes, and is stored
    // as a class member rather than being a derived value.
    dynamic_priority_max_ = ul;
    pending_shift_ = ACE_Time_Value (0, ul);
  }
    // set maximum supported priority value
  
  inline unsigned long
  dynamic_priority_offset (void) const
  {
    return dynamic_priority_offset_;
  }
    // get offset for boundary between signed range and unsigned range
  
  inline void
  dynamic_priority_offset (unsigned long ul)
  {
    // max_late_ and min_pending_ depend on dynamic_priority_offset_:
    // for performance reasons, the values in max_late_ and min_pending_
    // are (re)calculated only when dynamic_priority_offset_ is
    // initialized or changes, and are stored as a class member rather
    // than being derived each time one of their values is needed.
    dynamic_priority_offset_ = ul;
    max_late_ = ACE_Time_Value (0, ul - 1);
    min_pending_ = ACE_Time_Value (0, ul);
  }
    // set offset for boundary between signed range and unsigned range
  
  
  inline Priority_Status
  priority_status (ACE_Message_Block & mb,
                                                 const ACE_Time_Value & tv)
  {
    // default the message to have pending priority status
    Priority_Status status = PENDING;
  
    // start with the passed absolute time as the message's priority, then
    // call the polymorphic hook method to (at least partially) convert
    // the absolute time and message attributes into the message's priority
    ACE_Time_Value priority (tv);
    convert_priority (priority, mb);
  
    // if the priority is negative, the message is pending
    if (priority < ACE_Time_Value::zero)
      {
        // priority for pending messages must be shifted
        // upward above the late priority range
        priority += pending_shift_;
        if (priority < min_pending_)
          priority = min_pending_;
      }
    // otherwise, if the priority is greater than the maximum late
    // priority value that can be represented, it is beyond late
    else if (priority > max_late_)
      {
        // all messages that are beyond late are assigned lowest priority (zero)
        mb.msg_priority (0);
        return BEYOND_LATE;
      }
    // otherwise, the message is late, but its priority is correct
    else
      status = LATE;
  
    // use (fast) bitwise operators to isolate and replace
    // the dynamic portion of the message's priority
    mb.msg_priority((mb.msg_priority() & static_bit_field_mask_) |
                    ((priority.usec () + ACE_ONE_SECOND_IN_USECS * priority.sec ()) <<
                     static_bit_field_shift_));
  
    return status;
  }




  /// Dump the state of the strategy.
  virtual void dump (void) const;

protected:
  /// Hook method for dynamic priority conversion.
  virtual void convert_priority (ACE_Time_Value &priority,
                                 const ACE_Message_Block &mb) = 0;

  /// This is a bit mask with all ones in the static bit field.
  unsigned long static_bit_field_mask_;

  /**
   * This is a left shift value to make room for static bit field:
   * this value should be the logarithm base 2 of
   * (static_bit_field_mask_ + 1).
   */
  unsigned long static_bit_field_shift_;

  /// Maximum supported priority value.
  unsigned long dynamic_priority_max_;

  /// Offset to boundary between signed range and unsigned range.
  unsigned long dynamic_priority_offset_;

  /// Maximum late time value that can be represented.
  ACE_Time_Value max_late_;

  /// Minimum pending time value that can be represented.
  ACE_Time_Value min_pending_;

  /// Time value by which to shift pending priority.
  ACE_Time_Value pending_shift_;
};

/**
 * @class ACE_Deadline_Message_Strategy
 *
 * @brief Deadline based message priority strategy.
 *
 * Assigns dynamic message priority according to time to deadline.  The
 * message priority is divided into high and low order bit fields.  The
 * high order bit field is used for dynamic message priority, which is
 * updated whenever the convert_priority() method is called.  The
 * low order bit field is used for static message priority and is left
 * unchanged.  The partitioning of the priority value into high and low
 * order bit fields is done according to the arguments passed to the
 * strategy object's constructor.
 */
class  ACE_Deadline_Message_Strategy : public ACE_Dynamic_Message_Strategy
{
public:
  /// Ctor, with all arguments defaulted.
  ACE_Deadline_Message_Strategy (unsigned long static_bit_field_mask = 0x3FFUL,       // 2^(10) - 1
                                 unsigned long static_bit_field_shift = 10,           // 10 low order bits
                                 unsigned long dynamic_priority_max = 0x3FFFFFUL,     // 2^(22)-1
                                 unsigned long dynamic_priority_offset = 0x200000UL); // 2^(22-1)

  /// Virtual dtor.
  virtual ~ACE_Deadline_Message_Strategy (void);

  /// Dynamic priority conversion function based on time to deadline.
  virtual void convert_priority (ACE_Time_Value &priority,
                                 const ACE_Message_Block &mb);

  /// Dump the state of the strategy.
  virtual void dump (void) const;
};

/**
 * @class ACE_Laxity_Message_Strategy
 *
 * @brief Laxity based message priority strategy.
 *
 * Assigns dynamic message priority according to laxity (time to
 * deadline minus worst case execution time).  The message priority is
 * divided into high and low order bit fields.  The high order
 * bit field is used for dynamic message priority, which is
 * updated whenever the convert_priority() method is called.  The
 * low order bit field is used for static message priority and is left
 * unchanged.  The partitioning of the priority value into high and low
 * order bit fields is done according to the arguments passed to the
 * strategy object's constructor.
 */
class  ACE_Laxity_Message_Strategy : public ACE_Dynamic_Message_Strategy
{
public:
  /// Ctor, with all arguments defaulted.
  ACE_Laxity_Message_Strategy (unsigned long static_bit_field_mask = 0x3FFUL,       // 2^(10) - 1
                               unsigned long static_bit_field_shift = 10,           // 10 low order bits
                               unsigned long dynamic_priority_max = 0x3FFFFFUL,     // 2^(22)-1
                               unsigned long dynamic_priority_offset = 0x200000UL); // 2^(22-1)

  /// virtual dtor.
  virtual ~ACE_Laxity_Message_Strategy (void);

  /// Dynamic priority conversion function based on laxity.
  virtual void convert_priority (ACE_Time_Value &priority,
                                 const ACE_Message_Block &mb);

  /// Dump the state of the strategy.
  virtual void dump (void) const;
};


#endif /* ACE_MESSAGE_BLOCK_H */
