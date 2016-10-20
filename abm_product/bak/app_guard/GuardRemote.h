/*VER: 1*/ 
#ifndef WF_GUARD_REMOTE_H__
#define WF_GUARD_REMOTE_H__

#include "Guard.h"

bool InitRmtShm ();

bool LogMainGuardServer(int _iHostID, char *sLogName);

bool LogOutMainGuardSever();

void ProcessServerCommand();

void SendLocalAppInfo();

void CheckGuardRmtExist();

#endif

