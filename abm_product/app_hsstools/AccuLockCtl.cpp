#include "AccuLock.h"
#include "Log.h"

#include <stdlib.h>
#include <stdio.h>


void printUsage(const char *cmd );

int main(int argc, char **argv){

    if( argc < 2){
        printUsage(argv[0]);
        return 0;
    }
    AccuLockMgr *pLockMgr=new AccuLockMgr();
    if ( !pLockMgr ) {
        Log::log(0, "ÉêÇë×ÊÔ´Ê§°Ü!");
        exit(0);
    }


    for ( int iTemp = 1; iTemp < argc; ++iTemp ) {
        switch ( argv[1][1] | 0x20) {
        case 'c':
            {
                pLockMgr->createAll();
                break;
            }
        case 'd':
            {
                pLockMgr->unloadAll();
                break;
            }
        case 'e':
            {
                pLockMgr->exportLockInfo( argv[++iTemp] );
                break;
            }
        case 'v':
            {
                pLockMgr->printAll();
                break;
            }
        case 'q':
            {
                long lKey = atol(argv[++iTemp]);
                pLockMgr->printLock(&lKey, 1 );
                break;
            }
        case 'p':
            {
                long lKey = atol(argv[++iTemp]);
                pLockMgr->printLock(&lKey, 1 );
                break;
            }

        case 'l':
            {
                long lKey = atol(argv[++iTemp]);
                int iValue = atoi(argv[++iTemp]);
                pLockMgr->lockAccu(&lKey, 1, iValue);
                break;
            }
        case 'u':
            {
                long lKey = atol(argv[++iTemp]);
                pLockMgr->unLockAccu(&lKey, 1);
                break;
            }
        case 'k':
            {
                pLockMgr->printLockAll();
                break;
            }

        default:
            printUsage(argv[0]);
            break;
        }
    }
    return 0;
}

void printUsage(const char * cmd ){

    printf( " Usage : %s\n" 
            "\t-c : create shared memory\n"
            "\t-d : destroy shared memory\n"
            "\t-e table : export lock info to table\n"
            "\t-v : print all lock info\n"
            //"\t-q key : query key's lock info\n"
            "\t-p key : print key's lock info\n"
            "\t-l key lockvalue : set key's lock as lockvalue\n"
            "\t-u key : unlock key\n", cmd);
}


