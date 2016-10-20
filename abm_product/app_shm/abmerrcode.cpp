#include "MBC_ABM.h"
#include "TOCIQuery.h"
#include "Environment.h"
#include "ThreeLogGroup.h"

int main(int argc,char **argv)
{  
try{
  if(argc==1) return 0;
  
  int iCode=atoi(argv[1]);
  int iLevel=LOG_LEVEL_FATAL;
  int iType=LOG_TYPE_SYSTEM;
  HBErrorCode *m_pErrorCode = new HBErrorCode();
  m_pErrorCode->init();
  ThreeLogGroup *pLog=new ThreeLogGroup();
  
  char sBuf[512]={0};
  sprintf(sBuf,"%s--%s",m_pErrorCode->getCauseInfo(iCode),m_pErrorCode->getAdviceInfo(iCode));
  
  if(iCode<=190 || iCode==200)
  {
    iType=LOG_TYPE_SYSTEM ;
   }
  if(iCode>190 && iCode<=199)
  {
     iType=LOG_TYPE_PROC ;
  }
  if(iCode>200 && iCode<100001 )
  {
     iType=LOG_TYPE_BUSI ;
	 iLevel=LOG_LEVEL_ERROR;
  }
  if(iCode>=100001)
  {
	iLevel=4;
	}
  pLog->log(iCode,iLevel,iType,sBuf);
  
  if(pLog) delete pLog;
  if(m_pErrorCode) delete m_pErrorCode ;
  }catch(...){
  
  }
 }
  
