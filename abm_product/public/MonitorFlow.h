#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>

#include "Environment.h"


struct DBInfo
{
   char sDBName[10];
   char sDBPsw[30];
   char sDBStr[30];
};


static TOCIDatabase gpDBLinkMF;
//struct DBInfo strDBInfo[1];


class MonitorFlow{
   public:
    MonitorFlow();
    
    int MonitorBillingCycle(int iBillingCycleID,char *sState);
    int MonitorDiskMake(int iBillingCycleID,int mStart);
    int MonitorVirement(int iBillingCycleID,int mStart);
     
    void getDefaultLogin();
    
    long getNextStatSeq();

    ~MonitorFlow();
   
    char m_sDBUser[24];
    char m_sDBPwd[24];
    char m_sDBStr[32];
    char m_sAreaCode[32];
    
    static bool m_bConnect;
    
    char m_DIMENSIONS[1024];
    char sLockPath[1024];
    char sHome[254];
           
    long m_lCurStatID;

};