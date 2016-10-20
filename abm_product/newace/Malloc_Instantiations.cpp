// Malloc_Instantiations.cpp,v 4.4 2005/11/15 16:33:24 shuston Exp

#include "Local_Memory_Pool.h"
#include "Malloc_T.h"


#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Allocator_Adapter<ACE_Malloc>;

#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */

