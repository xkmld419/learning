#include "GroupDroitMgr.h"
#include "BaseFunc.h"
#include <string>
using namespace std;
GroupDroitMgr::GroupDroitMgr()
{
    m_iGroupID = getgid();
	bGetGroupID = false;//getRootGID();
	m_iRootGroupID = -1;
}

GroupDroitMgr::~GroupDroitMgr()
{
    
}

//当前用户是否是billongonline 用户
bool GroupDroitMgr::IsRootUsr()
{
	 char *pLogName = getenv("LOGNAME");
     if(!bGetGroupID)
	 {
	 	m_iRootGroupID = getNeedGroupID(pLogName);
		if(m_iRootGroupID != -1)
			bGetGroupID = true;
	  	if(bGetGroupID && m_iGroupID == m_iRootGroupID)
		{
        	return true;
		} else {
			return false;
		}
	 }
     else
        return false;
}

gid_t GroupDroitMgr::getGroupID()
{
	  if(bGetGroupID)
	  	return m_iGroupID;
	  else
	  	return -1;
}

char *GroupDroitMgr::getUsrName()
{
	 memset(m_sUsrName,'\0',strlen(m_sUsrName));
	 FILE *pp; 
     if( (pp = popen("whoami", "r")) == NULL ) 
     { 
        return NULL;
     } 
      
     if(!fgets(m_sUsrName, sizeof(m_sUsrName), pp))
     { 
        return NULL;
     }  
     pclose(pp);
	 if(m_sUsrName[0] != '\0')
	 {
	 	string str(m_sUsrName);
	 	trimN(str);
		memset(m_sUsrName,'\0',strlen(m_sUsrName));
		strcpy(m_sUsrName,(char*)str.c_str());
	 } 
	 if(m_sUsrName[0] != '\0')
	 		return m_sUsrName; 
	 else
	 	return NULL;
	 
}

int GroupDroitMgr::getNeedGroupID(char *sUsr)
{
	 if(sUsr[0] == '\0')
	 	return -1;
	 int iNeedGroupID = -1;
	 char sUsrName[128] = {0};
	 sprintf(sUsrName,"%s%s\0","id ",sUsr);
     string useStr;
     string useValue;
     FILE *pp; 
     char buf[40] = {0};
     if( (pp = popen(sUsrName, "r")) == NULL ) 
     { 
        return -1;
     } 
         
     if(!fgets(buf, sizeof(buf), pp))
     { 
        return -1;
     }  
     pclose(pp);
     string strBuf(buf);
     int iPos = strBuf.find("gid");
     string strFirst;
     string strGroup;
     if(iPos>0)
     {      
        strFirst.assign(strBuf,iPos,16);
        int iGroupPos = strFirst.find(' ');
        if(iGroupPos>0)
        {
            strGroup.assign(strFirst,0,iGroupPos);
        } else {
            strGroup = strFirst;
        }
        int iGroupValPos = strGroup.find('=');
        if(iGroupValPos>0)
            useStr.assign(strGroup,iGroupValPos+1,strGroup.size());
        else
            return -1; 
        int iGroupEndPos = useStr.find('(');
        if(iGroupEndPos>0)
            useValue.assign(useStr,0,iGroupEndPos);
        else
            useValue = useStr;
        
        iNeedGroupID = (gid_t)atoi((char*)useValue.c_str());
        //bGetGroupID = true;
        return iNeedGroupID;
     } else {
        return -1;
     }
}

/*
bool GroupDroitMgr::IsBillOnlineUsr()
{
	 if(bGetBillOnlineID && m_iBillUsrID == m_iRootGroupID)
        return true;
     else
        return false;
}
*/

