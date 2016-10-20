#ifndef _USER_LOGIN_H
#define _USER_LOGIN_H

#include "StatFormBase.h"

class UserLogin:public StatFormBase
{
    public:
        UserLogin();
        ~UserLogin();
    public:
        void Rebuild();
};
#endif

