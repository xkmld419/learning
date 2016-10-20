#ifndef _LOGG_INFO_H
#define _LOGG_INFO_H

#include "StatFormBase.h"

class LoggInfo:public StatFormBase
{
    public:
        LoggInfo();
        ~LoggInfo();
    public:
        void Rebuild();
};
#endif

