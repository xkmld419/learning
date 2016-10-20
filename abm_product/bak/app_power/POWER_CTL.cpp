/*
提供给电力调用的服务
 */
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <time.h>
#include <setjmp.h>
#include <signal.h>

#include <atmi.h>
#include <fml32.h>
#include <userlog.h>
#include <vector>
//#include "appflds.h"
#include "tblflds.h"
#include "Exception.h"
#include "PowerXml.h"

#include <time.h>

#define MAX_XML_SIZE 1024*1024*10

PowerXml   *pPower = NULL;

extern "C"
{
	int tpsvrinit(int argc, char *argv[])
	{
		char sErrMsg[1000];
		try
		{
				userlog("welcome to server %s !", argv[0]);
				pPower = new PowerXml();
				if(pPower ==NULL)
				{
					userlog("new PowerXml error");
				}
		}
		catch(Exception &e)
		{		
			bzero(sErrMsg,1000);
			sprintf(sErrMsg, "收到Exception退出\n[EXCEPTION]: %s", e.descript());
			userlog(sErrMsg);
			exit(-1);
		}
		catch(...)
		{
			bzero(sErrMsg,1000);
			sprintf(sErrMsg, "收到其他Exception退出");
			userlog(sErrMsg);
			exit(-1);
		}	
		return(0);
	}

	void tpsvrdone()
	{
		if(pPower)
		{
			delete pPower;
			pPower= NULL;
		}	
	}

	/****************************************************************/

	/****************************************************************/

	void POWER_CTL_RUN(TPSVCINFO *rqst)
	{
		FBFR32 *rcvbuf = NULL;
		FBFR32 *sendbuf = NULL;
		{
		char sInXML[MAX_XML_SIZE];
		char sOutXML[MAX_XML_SIZE];
		memset(sInXML, 0, MAX_XML_SIZE);
		memset(sOutXML, 0, MAX_XML_SIZE);
		int iRet = -1;
		
		long lResult = -1L;
		char sErrMsg[1000]={0};

		try
		{
			rcvbuf = (FBFR32 *)rqst->data;

			if ((sendbuf = (FBFR32*)tpalloc("FML32", NULL, MAX_XML_SIZE))==NULL)
			{
				userlog("tpalloc error");
			}
			if (Fgets32(rcvbuf, IN_XML, 0, sInXML) == -1)
			{
				userlog("get inxml error");
			}
			
			if(pPower == NULL)
			{
				pPower = new PowerXml();
				userlog("new PowerXml again");
				if(pPower == NULL)
				{
					//THROW(INSTANTIATION_ERR);
					userlog("new PowerXml error");
				}
			}
	
			clock_t start, finish;	//测试run运行时间
			double total_time;
			start = clock();
			iRet = pPower->PowerXmlProcess(sInXML, sOutXML);
			finish = clock();
			total_time = (double)(finish - start)/CLOCKS_PER_SEC;
			userlog(" pPower->PowerXmlProcess() running time is %f seconds\n", total_time);
//			if (iRet != 0)
//			{
//				userlog("pPower->PowerXmlProcess() lorderListID=%ld error code=%d",lOrderListId,iRet);
//			}

			if (Fchg32(sendbuf, OUT_XML, 0, (char *)sOutXML, (FLDLEN32)0) == -1)
			{
				userlog("OUT_XML error");
			}
		}

		catch(Exception &e)
		{		
			bzero(sErrMsg,1000);
			sprintf(sErrMsg, "收到Exception退出\n[EXCEPTION]: %s", e.descript());
			iRet = -1;
		}
		catch(...)
		{
			bzero(sErrMsg,1000);
			sprintf(sErrMsg, "收到其他Exception退出");
			userlog(sErrMsg);
			iRet = -1;
		}
		
		if(iRet == 0)
		{//成功
			lResult = 0;
			strcpy(sErrMsg, "OK"); 
		}

		Fchg32(sendbuf, IRESULT, 0, (char *)&lResult, (FLDLEN32)0);
		Fchg32(sendbuf, SMSG, 0, (char *)sErrMsg, (FLDLEN32)0);
		
		}
		
		//Environment::commit();

		tpreturn(TPSUCCESS, 0, (char *)sendbuf, 0L, 0L);
	}
}
