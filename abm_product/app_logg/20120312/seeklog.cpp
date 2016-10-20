#include "ThreeLogGroup.h"

int main(int argc,char **argv)
{
   //ThreeLogGroup Log;
   //Log.m_iLogProid=1001;
   //Log.m_iLogAppid=10;
   vector<LogInfoData> vRet; 
   HBErrorCode m_pErrorCode ;
   m_pErrorCode.init();
   //Log.GetLoggInfo(vRet,GROUPTYPE,LOG_TYPE_PROC,"20110713160000");
   if(argc==1)
   {
   	printf("please input the param :flag,condition");
   	return 1;
   }
   if(argc==2)
   {
   	   int iCodeId =atoi(argv[1]);
   	 cout<<m_pErrorCode.getCauseInfo(iCodeId)<<endl;
   	 return 1;
   }
   /*
     switch(atoi(argv[1])){
     	case GROUPCODE:
     	case GROUPTYPE:
     	case GROUPINFOLV:
     	case GROUPCLASS:
    	  printf("please input the param :condition\n")  ;
    	  return 1;
    	  break ;
    	default:
    		break;
     }  		
   }

   if(argc==3)
      icond=atoi(argv[2]);
    
   Log.GetLoggInfo(vRet,iflag,icond,"20110713160000");
   cout<<"the vRet size is:"<<vRet.size()<<endl;
   char sBuf[2048]={0};
   for(int i=0;i<vRet.size();i++)
   {
      memset(sBuf,0,sizeof(sBuf));
      sprintf(sBuf,"%s|%d|%s|%d|%d|%d|%d|%d|%d|%d|%s|%s|%s",
          vRet[i].m_sLogTime,vRet[i].m_illCode,vRet[i].m_sCodeName,
          vRet[i].m_illProId,vRet[i].m_illAppId,
          vRet[i].m_illLevelId,vRet[i].m_illClassId,vRet[i].m_illTypeId,
          vRet[i].m_illEmployee,vRet[i].m_illActionId,
          vRet[i].m_sActionName,vRet[i].m_sAreaCode,vRet[i].m_sInfo);
       cout<<i<<"'s ret is :"<<sBuf<<endl;
   }
         
       */
   return 1;    
}

