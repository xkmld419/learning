#ifndef _MACROS_H
#define _MACROS_H

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include "Default_Constants.h"

#define ACE_HAS_TEMPLATE_TYPEDEFS 1

#define ACE_reinterpret_cast(TYPE, EXPR) reinterpret_cast<TYPE> (EXPR)

#define ACE_SEH_TRY if (1)
#define ACE_SEH_EXCEPT(X) while (0)
#define ACE_SEH_FINALLY if (1)

#define ACE_UNUSED_ARG(a) do {/* null */} while (&a == 0)

#define ACE_ALLOC_HOOK_DECLARE struct __Ace {}
#define ACE_ALLOC_HOOK_DEFINE(CLASS)
#define ACE_BIT_DISABLED(WORD, BIT) (((WORD) & (BIT)) == 0)
#define ACE_SET_BITS(WORD, BITS) (WORD |= (BITS))
#define ACE_CLR_BITS(WORD, BITS) (WORD &= ~(BITS))

#define ACE_UNIMPLEMENTED_FUNC(f) f;
#define ACE_CLASS_IS_NAMESPACE(CLASSNAME) \
private: \
CLASSNAME (void); \
CLASSNAME (const CLASSNAME&); \
friend class ace_dewarn_gplusplus


#define ACE_align_binary(ptr, alignment) \
    ((ptr + ((ptrdiff_t)((alignment)-1))) & (~((ptrdiff_t)((alignment)-1))))

#define ACE_ptr_align_binary(ptr, alignment) \
        ((char *) ACE_align_binary (((ptrdiff_t) (ptr)), (alignment)))

# define ACE_ALLOCATOR_RETURN(POINTER,ALLOCATOR,RET_VAL) \
   do { POINTER = ALLOCATOR; \
     if (POINTER == 0) { errno = ENOMEM; return RET_VAL; } \
   } while (0)
# define ACE_ALLOCATOR(POINTER,ALLOCATOR) \
   do { POINTER = ALLOCATOR; \
     if (POINTER == 0) { errno = ENOMEM; return; } \
   } while (0)

#define ACE_DES_FREE_TEMPLATE(POINTER,DEALLOCATOR,T_CLASS,T_PARAMETER) \
     do { \
          if (POINTER) \
            { \
              (POINTER)->~T_CLASS T_PARAMETER (); \
              DEALLOCATOR (POINTER); \
            } \
        } \
     while (0)

# define ACE_DES_FREE(POINTER,DEALLOCATOR,CLASS) \
   do { \
        if (POINTER) \
          { \
            (POINTER)->~CLASS (); \
            DEALLOCATOR (POINTER); \
          } \
      } \
   while (0)


# define ACE_NEW_MALLOC_RETURN(POINTER,ALLOCATOR,CONSTRUCTOR,RET_VAL) \
   do { POINTER = ALLOCATOR; \
     if (POINTER == 0) { errno = ENOMEM; return RET_VAL;} \
     else { new (POINTER) CONSTRUCTOR; } \
   } while (0)
# define ACE_NEW_MALLOC(POINTER,ALLOCATOR,CONSTRUCTOR) \
   do { POINTER = ALLOCATOR; \
     if (POINTER == 0) { errno = ENOMEM; return;} \
     else { new (POINTER) CONSTRUCTOR; } \
   } while (0)
# define ACE_NEW_MALLOC_NORETURN(POINTER,ALLOCATOR,CONSTRUCTOR) \
   do { POINTER = ALLOCATOR; \
     if (POINTER == 0) { errno = ENOMEM;} \
     else { new (POINTER) CONSTRUCTOR; } \
   } while (0)

#    include /**/ <new>
#    define ACE_bad_alloc std::bad_alloc
#    define ACE_throw_bad_alloc throw ACE_bad_alloc ()


#define ACE_NEW_RETURN(POINTER,CONSTRUCTOR,RET_VAL) \
   do { try { POINTER = new CONSTRUCTOR; } \
        catch (ACE_bad_alloc) { errno = ENOMEM; POINTER = 0; return RET_VAL; } \
   } while (0)

#define ACE_NEW(POINTER,CONSTRUCTOR) \
   do { try { POINTER = new CONSTRUCTOR; } \
        catch (ACE_bad_alloc) { errno = ENOMEM; POINTER = 0; return; } \
   } while (0)

#define ACE_NEW_NORETURN(POINTER,CONSTRUCTOR) \
   do { try { POINTER = new CONSTRUCTOR; } \
        catch (ACE_bad_alloc) { errno = ENOMEM; POINTER = 0; } \
   } while (0)

#endif

