/*VER: 1*/ 
#include "OfferAccuMgr.h"
#include "Environment.h"
#include "TOCIQuery.h"
#include <string.h>
#include "Stack.h"
#include <iostream>
#include "Log.h"
#include "WorkFlow.h"
#include "AccuTypeCfg.h"
#include "PricingEngine.h"

using namespace std;


int main(int argc,char * argv[])
{
    char sOfferInstID[30];

    OfferAccuMgr ctl(true);

    if (argc == 1)
    {
    while(1){

        printf("input OfferInstID :\n");
        gets(sOfferInstID);

        if (strlen(sOfferInstID)==0)
            break;

        ctl.printAccByInst(atol(sOfferInstID));

    }
    }

    strcpy(sOfferInstID,argv[1]);

    ctl.printAccByInst(atol(sOfferInstID));

    return 0;
}




