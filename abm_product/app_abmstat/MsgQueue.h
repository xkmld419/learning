#ifndef _MSG_QUEUE_H
#define _MSG_QUEUE_H

#include "StatFormBase.h"

class MsgQueue:public StatFormBase
{
    public:
        MsgQueue();
        ~MsgQueue();
    public:
        void Rebuild();
};
#endif

