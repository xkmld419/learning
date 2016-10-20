#ifndef GROUP_DROIT_MGR_H
#define GROUP_DROIT_MGR_H

#include <unistd.h>
#include <sys/types.h>
#include  <stdlib.h>
#include <string.h>
#include <stdio.h>

class GroupDroitMgr
{
      public:
              GroupDroitMgr();
              ~GroupDroitMgr();
			  //bool IsBillOnlineUsr();
              bool IsRootUsr();
			  bool getRootGID();
			  gid_t getGroupID();
			  char *getUsrName();
      private:
	          char m_sUsrName[512];
              gid_t m_iGroupID;
              gid_t m_iRootGroupID;
			  bool bGetGroupID;
			  //ªÒ»°
			  int getNeedGroupID(char *sUsr);
};

#endif
