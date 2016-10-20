#ifndef __AAA_MEMORY_MANAGER_H__
#define __AAA_MEMORY_MANAGER_H__

#include "diameter_parser_api.h"

/*!
 * AAA Message Manager
 *
 * This class provides very efficient memory management
 * for fixed sized message chunks. Internal implementations
 * MUST use this manager to allocate raw message buffers
 * to store AAA formatted packets during reception and
 * transmission. An instances of this object manages n 
 * number of pre-allocated chunks that it lends it's users.
 * The number of chunks is configurable at compile-time 
 * with the macro AAA_MIN_MESSAGE_COUNT. Care MUST be 
 * taken in choosing a number large enough so as not to 
 * starve the application requiring a large number of 
 * chuncks but small enough so as not to over allocate 
 * and have chunks that may never be used.
 *
 * It is important to note that a call to alloc() on this
 * object will return a fixed size chunks and that the
 * formatted AAA message MUST not exceed that size. The
 * size of these chunks is configurable at compile-time 
 * only. Care MUST be taken in choosing a size that will 
 * be larger than all possible packet size that AAA will 
 * send or recieve. If adjustments need to be made, adjust 
 * the AAA_MAX_MESSAGE_SIZE macro in defs.h
 */
class AAAMessageManager
{
    public:
        /*!
         * Definitions for default block count
         */
        enum {
	   AAA_MIN_MESSAGE_COUNT = 512
        };

    public:
        /*!
         * constructor
         *
         * \param n_blocks Number of blocks to manage
         */
        AAAMessageManager(int n_blocks = AAA_MIN_MESSAGE_COUNT);

	/*!
	 * destructor
	 */
	~AAAMessageManager();

        /*!
         * Access function to message singleton
         */
        static AAAMessageManager *instance() { return &AAAMessageManager::allocator_; }

        /*!
         * Allocates an un-used message buffer 
         */
        AAAMessageBlock *malloc();

        /*!
         * Returns a message buffer to the free list
         */
        void free(AAAMessageBlock *buffer);

    private:
        static AAAMessageManager allocator_; /*<< Singleton instance of the allocator */

        AAAMessageBlock **pool_; /*<< Message pool */

        int num_blocks_; /*<< number of message buffers in the pool */
};

#endif /* __AAA_MEMORY_MANAGER_H__ */
