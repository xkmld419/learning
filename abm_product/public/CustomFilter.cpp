/*VER: 1*/ 
#include "CustomFilter.h"
#include "Environment.h"
#include "Exception.h"
//#include "IDParser.h"
#include "CommonMacro.h"

struct CustomConfig
{
  int iValueSeq;
  int iMatchMethod;
  int iAttrID;
  
};

char  g_asValues[20][50];
struct CustomConfig  g_aoConfig[20];
int g_iFieldTotalCnt;

int g_iFieldCnt;   // 从0 开始的数字
CustomCheckResult* g_pCustomCheckResult;
StdEvent * g_pEvent;


CustomCheckResult :: CustomCheckResult()
{
	m_iEventTypeID = 0;
	memset (m_sEffDate, 0, sizeof (m_sEffDate));
	memset (m_sExpDate, 0, sizeof (m_sExpDate));
}

CustomCheckResult :: CustomCheckResult(int iEventTypeID, char * sEffDate, char * sExpDate)
{
	m_iEventTypeID = iEventTypeID;
	strncpy (m_sEffDate, sEffDate, 16);
	strncpy (m_sExpDate, sExpDate, 16);
}

CustomCheck :: CustomCheck()
  {
      m_pResult=NULL;
  }
  
 int CustomCheck::check( )
  {
  CustomCheck * pCustomCheck=NULL;
  int iRet;
  bool bRet;

    if(g_iFieldCnt >=g_iFieldTotalCnt  )
        return -1;
    
   g_iFieldCnt++;
   
   if( g_aoConfig[g_iFieldCnt-1].iMatchMethod==1 )
       bRet=m_oCustomCheck.getMax(g_asValues[g_iFieldCnt-1], &pCustomCheck) ;
   else
       bRet=m_oCustomCheck.get(g_asValues[g_iFieldCnt-1],  &pCustomCheck) ;
   
    if (bRet==true)
        {

          if (  g_iFieldCnt ==  g_iFieldTotalCnt   )
            {
            
            if(strcmp (g_pEvent->m_sStartDate, pCustomCheck->m_pResult->m_sEffDate)>=0 && 
                strcmp (g_pEvent->m_sStartDate, pCustomCheck->m_pResult->m_sExpDate)<0 )
                {
                return pCustomCheck->m_pResult->m_iEventTypeID;
                }
            else
                return -1;
            
            } 
          
          iRet =pCustomCheck->check() ;
           
          if(iRet !=-1 )
            return  iRet;

        }

  if(m_oCustomCheck.get("*", &pCustomCheck) ==true)
    {
          if (  g_iFieldCnt ==  g_iFieldTotalCnt  )
            {
            
            if(strcmp (g_pEvent->m_sStartDate, pCustomCheck->m_pResult->m_sEffDate)>=0 && 
                strcmp (g_pEvent->m_sStartDate, pCustomCheck->m_pResult->m_sExpDate)<0 )
                {
                return pCustomCheck->m_pResult->m_iEventTypeID;
                }
            else
                return -1;
            
            } 
          
          return pCustomCheck->check() ;

    }


   return -1;

    
  }
  
  
void CustomCheck::insert(  )
  {

  CustomCheck * pCustomCheck=NULL;

 
    if (m_oCustomCheck.get( g_asValues[g_iFieldCnt], &pCustomCheck) !=true)
        {
        pCustomCheck=new CustomCheck();
        m_oCustomCheck.add( g_asValues[g_iFieldCnt],  pCustomCheck);

         if (  g_iFieldCnt == ( g_iFieldTotalCnt-1)   )
            {
            pCustomCheck->m_pResult=g_pCustomCheckResult;
            } 
        
        }
    
        g_iFieldCnt++;
      if ( g_iFieldCnt >=g_iFieldTotalCnt )
        return ;
      
      if(pCustomCheck!=NULL)
            pCustomCheck->insert();

  
  }


void CustomCheck::del(  )
  {
/*
KeyTree 没实现遍历的方法
  CustomCheck * pCustomCheck=NULL;
  KeyTree <CustomCheck *> :: Iteration iterCustomCheck=m_oCustomCheck.getIteration();
 
    while (iterCustomCheck.next( pCustomCheck) ==true)
        {
        pCustomCheck->del();

         if (pCustomCheck->m_pResult  )
            {
            del pCustomCheck->m_pResult ;
            pCustomCheck->m_pResult =NULL;
            } 
        
        }
*/
    m_oCustomCheck.destroy();

  
  }







void CustomFilter::load()
  {
  
    memset(  g_asValues, 0, sizeof(g_asValues) );
    memset(  g_aoConfig, 0, sizeof(g_aoConfig) );

    m_iNotFilterEventType=0;
    
    g_iFieldTotalCnt=0;

    g_iFieldCnt=0;   // 从0 开始的数字
    g_pCustomCheckResult=0;
    g_pEvent=0;

	DEFINE_QUERY (qry);
        char sSql[500];

	qry.setSQL("select value_seq,attr_id,nvl(match_method,0) match_method"
            " from b_custom_filter_config where value_seq>=0 and rownum<=20"); 
	qry.open ();
	struct CustomConfig oConfigTmp;
	while(qry.next ())
	 {
	 oConfigTmp.iValueSeq=qry.field("value_seq").asInteger();
         oConfigTmp.iMatchMethod=qry.field("match_method").asInteger();
         oConfigTmp.iAttrID=qry.field("attr_id").asInteger();

         if(oConfigTmp.iValueSeq==0)
            {
            m_iNotFilterEventType=oConfigTmp.iAttrID;
            continue;
            }
	  g_aoConfig[g_iFieldTotalCnt]=oConfigTmp ;
          g_iFieldTotalCnt++;
	  
	  
	}
    
	qry.close ();
        char sTmp[100];
        strcpy(sSql, "select ");
        for (int iTmp=0; iTmp<g_iFieldTotalCnt; iTmp++)
            {
            snprintf(sTmp, 100, "nvl(value%d,'*'),",  g_aoConfig[iTmp].iValueSeq  );
            strncat(sSql, sTmp, 500);
            }
        
        strncat(sSql,  
            " to_number(event_type_id) event_type_id,"
            " nvl(eff_date, to_date('19800101', 'yyyymmdd')) eff, "
            " nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp from b_custom_filter",
            500);

        
	qry.setSQL (sSql);
	qry.open ();

	while (qry.next ()) {
		g_pCustomCheckResult = new CustomCheckResult (
                    qry.field ("event_type_id").asInteger (),
                    qry.field ("eff").asString (),
                    qry.field ("exp").asString () );
        
                for (int iTmp=0; iTmp<g_iFieldTotalCnt; iTmp++)
                    {
                    strncpy(g_asValues[iTmp],  qry.field ( iTmp ).asString () ,50);
                    }		
                
		g_iFieldCnt=0;
 		m_oCustomCheck.insert ();
	}

	qry.close ();	    
	    
	    
}
	  
	  
	  
	  
void CustomFilter::unload()
{

m_oCustomCheck.del();

}
	  
	  
	  
	  	  
	  	  
	int  CustomFilter::filter(StdEvent * pEvent)
	  {
        Value pValue[1];
        int iEentTypeID;
        char sTmp[50];

        g_pEvent=pEvent;
       
        for (int iTmp=0; iTmp<g_iFieldTotalCnt; iTmp++)
            {
            snprintf(sTmp, 50, "%d",  g_aoConfig[iTmp].iAttrID);
             IDParser::getValue (  (EventSection * )pEvent, sTmp, pValue );
             if (pValue->m_iType == NUMBER )
                snprintf(g_asValues[iTmp], 50, "%ld",  pValue->m_lValue );
             else
                strncpy(g_asValues[iTmp],  pValue->m_sValue, 50  );
             
            }
        g_iFieldCnt=0;
        iEentTypeID=m_oCustomCheck.check();

        
        if (iEentTypeID!=-1)
            {
            pEvent->m_iEventTypeID=iEentTypeID;
            return 1;
            }
        else 
            {
            if(m_iNotFilterEventType!=0)
                {
                pEvent->m_iEventTypeID=m_iNotFilterEventType;
                return 1;
                }
            }

        
        return 0;
        
        
	    
	  }	  


