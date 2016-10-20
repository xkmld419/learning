/*VER: 1*/ 



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "Environment.h"
#include "Process.h"
#include "Log.h"
#include "MBC.h"
#include "HashList.h"
#include "DbTableMgr.h"
#include "DataIncrementMgr.h"

class PrintLastEvent: public Process
{
public:
   
    int run();

private:

    void printUsage( void );
};


int PrintLastEvent::run()
{
    int iTableType=999;
    int iTemp, jTemp;

    jTemp = g_argc;

    bool bShort=false;
    bool bForce=false;

    for ( iTemp = 1; iTemp < jTemp; iTemp ++ )
    {
        if ( g_argv[iTemp][0] != '-' )
        {
            printUsage();
            return 0;
        }

        switch ( g_argv[iTemp][1] | 0x20 )
        {
            case 's':
                bShort = true;
                break;
                
            case 'f':
                bForce = true;
                break;

            case 't':
                iTemp++;
                iTableType=atoi( g_argv[iTemp] );
                break;

            default:
                printUsage();
                return false;
                break;
        }

    }
    

    BaseEventData baseEvt[1];
     
    
    if( GetLastEvent(  & baseEvt[0].strStdEvent )==TRUE )
    {
        switch(baseEvt[0].strStdEvent.m_iEventTypeID )
        {
          case FILE_END_EVENT_TYPE:
             printf("\n    file end event,file_id:%d\n\n",
               baseEvt[0].strStdEvent.m_iFileID);
             if(bForce==true)
             {
              DbTableMgr tablemgr(iTableType);
              printf("%s\n", tablemgr.getDbFieldStr() );
              printf("\n%s\n\n", tablemgr.getMemToDbValues((char *)baseEvt) );                
             }  
             break;
             
          case FORCE_COMMIT_EVENT_TYPE:
             printf("\n    force commit event..\n\n" );
             if(bForce==true)
             {
              DbTableMgr tablemgr(iTableType);
              printf("%s\n", tablemgr.getDbFieldStr() );
              printf("\n%s\n\n", tablemgr.getMemToDbValues((char *)baseEvt) );                
             }                         
            break;
            
          default :
            if(bShort==false)
            {
              DbTableMgr tablemgr(iTableType);
              printf("%s\n", tablemgr.getDbFieldStr() );
              printf("\n%s\n\n", tablemgr.getMemToDbValues((char *)baseEvt) ); 
            }
            else
            {
              printf("serv_id   :%ld\n", baseEvt[0].strStdEvent.m_iServID);
              printf("calling   :%s\n", baseEvt[0].strStdEvent.m_sCallingNBR);
              printf("called    :%s\n", baseEvt[0].strStdEvent.m_sCalledNBR);
              printf("start_date:%s\n", baseEvt[0].strStdEvent.m_sStartDate);
              printf("duration  :%ld\n", baseEvt[0].strStdEvent.m_lDuration);
              printf("event_type:%d\n", baseEvt[0].strStdEvent.m_iEventTypeID);
              printf("file_id   :%d\n\n\n", baseEvt[0].strStdEvent.m_iFileID);
              
            }               
            break;
               
        }

        
    }
    else
    {
        printf("no data to print..");
    }  

    fflush(stdout);
    return 0;

    
}



void PrintLastEvent:: printUsage(void)
  {
    printf("\n使用说明:\n");
    printf("%s -t table_type   以table_type这种表类型来打印\n", g_argv[0]);
    printf("%s -s   以简短方式(short)来打印\n", g_argv[0]);
    printf("%s -f   强制打印end_file事件或者force_commit事件的内存信息\n\n",
       g_argv[0]);
  }

DEFINE_MAIN (PrintLastEvent);

