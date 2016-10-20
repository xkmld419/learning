#ifndef _SHM_INFO_SHOW_H
#define _SHM_INFO_SHOW_H

#include "StatFormBase.h"

class SHMInfoShow:public StatFormBase
{
    public:
        SHMInfoShow();
        ~SHMInfoShow();
    public:
        void Rebuild();
};
#endif

