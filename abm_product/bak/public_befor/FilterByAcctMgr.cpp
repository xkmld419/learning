/*VER: 1*/ 
#include "FilterByAcctMgr.h"


 HashList<int> FilterByAcctMgr::m_hiAcctEventType(123);
 bool FilterByAcctMgr::bInit=false;
 vector<long> FilterByAcctMgr::m_vlAcctID;



FilterByAcctMgr::FilterByAcctMgr()
{
    bInit=false;
}



 void FilterByAcctMgr::filterByAcctID (EventSection *_poEventSection)
{      
    if(bInit==false)
      {
        load();
        bInit=true;
      }

if(_poEventSection->m_iServID <=0 )
  _poEventSection->bindUserInfo();
  
if(_poEventSection->m_iServID <=0 )
  return ;  

int iEventType=0;

m_vlAcctID.clear();  
G_PSERV->getAcctID(m_vlAcctID,_poEventSection);

for(int iTmp=0; iTmp<m_vlAcctID.size(); iTmp++)
{
  if(m_hiAcctEventType.get( m_vlAcctID[iTmp], &iEventType )==true)
    {
      _poEventSection->m_iEventTypeID=iEventType;
      return ;
     
    }
}

if(m_hiAcctEventType.get(-1L,  &iEventType)==true)
  {
  _poEventSection->m_iEventTypeID=iEventType;  
    
  }

return;


}


 void  FilterByAcctMgr::load ()
  {
   m_hiAcctEventType.clearAll();
   TOCIQuery Qry(DB_LINK);
   Qry.setSQL("select distinct acct_id, event_type_id from b_acct_filter");
   Qry.open();

   while(Qry.next())
   {

    m_hiAcctEventType.add(Qry.field("acct_id").asLong(), Qry.field("event_type_id").asInteger() );
    
    
   } 
    
   Qry.close() ;
    
  }





