#include "ThreeLogGroup.h"
#include "CommandCom.h"
#include "HSSLogV3.h"
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef OFFSET_HEAD
#define OFFSET_HEAD(c,m) ((long )&((c *)0)->m)
#endif

extern int g_iModuID;
extern int g_iProcID;

static ThreeLogGroup *g_po3Log = NULL;

static CommandCom *g_poKern = NULL;

void directLog(int iLevel, int iTypeID, int iErrNo, char *fmt, ...)
{
    return;
    char sTmp[4096];
    va_list ap;
	va_start(ap, fmt);
	vsnprintf(sTmp, sizeof(sTmp), fmt, ap);
	va_end(ap);
	    
    if (!g_po3Log) {
        if ((g_po3Log=new ThreeLogGroup) == NULL) {
            std::cout<<"g_po3Log new ThreeLogGroup Failed! throw!!!"<<endl; 
            throw;
        }   
    }
    int iNetID=0;
    char sFlow[10]={0}; 
    g_po3Log->Product_log(iErrNo, g_iProcID, g_iModuID, iLevel, iTypeID, iNetID,sFlow,sTmp);
}

void authLog(int iNeID, int iServiceID, char *pServiceName, char *pErrMsg)
{
    char sTmp[4096];
    snprintf(sTmp, sizeof(sTmp), "%d|%d|%s|%s", iNeID, iServiceID, pServiceName, pErrMsg);
    return directLog(V3_ERROR, V3_SERVICE, MBC_ABM_AUTH_LOG, sTmp);    
}

CommandCom* getKernel()
{
    if (!g_poKern) {
        if ((g_poKern=new CommandCom) == NULL)
            throw;
        if (!g_poKern->InitClient()) {
            throw;
        }             
    }
    return g_poKern;   
}    
           
