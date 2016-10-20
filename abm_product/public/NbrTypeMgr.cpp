/*VER: 1*/ 
#include "NbrTypeMgr.h"
#include <stdlib.h>
#include <unistd.h>



vector<NbrType> NbrTypeMgr::vecMobileNbr;
bool NbrTypeMgr::bInited = false;

NbrTypeMgr::NbrTypeMgr()
{
  init();

}

void NbrTypeMgr::init()
{
    vecMobileNbr.clear();
    bInited = true;
    
    char sTemp[200]={0};
    ParamDefineMgr::getParam("ATTR_FORMAT" , "MOBILE_NBR_HEAD",  sTemp, 199);
    char * pTemp = NULL;
    char * pEnd  = NULL;
    char * pType = NULL;
    
    NbrType oNbrType;
    
    if(sTemp[0]){
        pTemp = sTemp;
    }
    
    
    while( pTemp ){
        pEnd = strstr( pTemp, "," );
        if( pEnd ){
            *pEnd = '\0';
            pEnd++;
        }
        if( pType = strstr( pTemp, ":" ) ){
            *pType = '\0';
            pType++;
            oNbrType.iTypeID = atoi( pType );
        }
        strncpy(oNbrType.sNbr, pTemp, 32);
        vecMobileNbr.push_back( oNbrType );
        
        if( !pEnd )
            break;
        if( *pEnd == '\0')
            break;
            
        pTemp = pEnd;

    }
     
}


bool NbrTypeMgr::ifMobileNbr(char * sNbr)
{
    if(!bInited) 
        init();
        
    for(int i=0; i<vecMobileNbr.size(); i++){
        if(strncmp(sNbr,vecMobileNbr[i].sNbr, strlen(vecMobileNbr[i].sNbr))==0 )
            return true;
    }

    return false;
    
}


int NbrTypeMgr::getNbrType(char * sNbr)
{
    if(!bInited) 
        init();
        
    for(int i=0; i<vecMobileNbr.size(); i++){
        if(strncmp(sNbr,vecMobileNbr[i].sNbr, strlen(vecMobileNbr[i].sNbr))==0 )
            return vecMobileNbr[i].iTypeID;
    }

    return -1;
}

