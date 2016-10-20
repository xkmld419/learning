#include "memory_manager.h"

#define AAA_MSGBLOCK_USED   0x0001

AAAMessageManager AAAMessageManager::allocator_;

AAAMessageManager::AAAMessageManager(int n_blocks) :
    pool_(NULL),
    num_blocks_(n_blocks)
{    
    ACE_NEW_NORETURN(this->pool_, AAAMessageBlock*[this->num_blocks_]);
}

AAAMessageManager::~AAAMessageManager()
{
    delete this->pool_;
}

AAAMessageBlock *AAAMessageManager::malloc()
{
    if (pool_) {

        AAAMessageBlock *buffer;

        for (int i = 0; i < this->num_blocks_; i++ ) {

            buffer = this->pool_[i];

            if (! (buffer->flags() & AAA_MSGBLOCK_USED)) {
  	        buffer->set_flags(AAA_MSGBLOCK_USED);
                return (buffer);
            }
        }
    }
    return (NULL);
}

void AAAMessageManager::free(AAAMessageBlock *buffer)
{
    if (buffer->flags() & AAA_MSGBLOCK_USED) {
       buffer->clr_flags(AAA_MSGBLOCK_USED);
    }
}
