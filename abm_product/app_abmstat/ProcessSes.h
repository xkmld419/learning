#ifndef _PROCESS_SES_H
#define _PROCESS_SES_H

#include "StatFormBase.h"

class ProcessSes:public StatFormBase
{
    public:
        ProcessSes();
        ~ProcessSes();
    public:
        void Rebuild();
};
#endif

