/*VER: 1*/ 
#ifndef NBR_TYPE_MGR_H
#define NBR_TYPE_MGR_H

//2008-8-6
//apin


#include "ParamDefineMgr.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

using namespace std;

class NbrType
{
    public:
        char sNbr[32];
        int  iTypeID; //ÄÄÀàºÅÂë
        
    NbrType(){ sNbr[0]='\0'; iTypeID = 0; };
        
};

class NbrTypeMgr
{
  public:
	NbrTypeMgr();

  public:
	static bool ifMobileNbr(char * sNbr);
    static int  getNbrType( char * sNbr );
    static void init();
    
    ///¼òµ¥ÅÐ¶Ï
    static vector<NbrType> vecMobileNbr;
    static bool bInited;
        
};

#endif
