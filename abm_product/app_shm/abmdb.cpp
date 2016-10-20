#include "MBC_ABM.h"
#include "TOCIQuery.h"
#include "Environment.h"
#include "ThreeLogGroup.h"

int main(int argc,char **argv)
{  
try{
  int iNum=0 ;
  if(argc==1) iNum=10000;
  else iNum=atoi(argv[1]);
  DEFINE_QUERY(qry);
  char sSql[512]={0};
  sprintf(sSql,"insert into b_test_hss(test_id,test_value) values(:id,:value)");
  qry.setSQL(sSql);
  for(int i=1;i<=iNum;i++)
  {
	  qry.setParameter("id", i);
	  qry.setParameter("value", "HSS_TEST_INSERT");
	  qry.execute();
	 }
	  qry.commit();
	  
	  if(argc==3) sleep(atoi(argv[2]));
	  Environment::disconnect();
	}catch(TOCIException &e){
		ThreeLogGroup Log;
		if(e.getErrCode()==MBC_DB_LINK_FAIL){
			Log.log(MBC_DB_LINK_FAIL,LOG_LEVEL_FATAL,LOG_TYPE_SYSTEM,e.getErrMsg());
			return -1;}
		if(e.getErrCode()==MBC_DB_SERV_PWD_ERROR){
			Log.log(MBC_DB_SERV_PWD_ERROR,LOG_LEVEL_FATAL,LOG_TYPE_SYSTEM,e.getErrMsg());
			return -1;
			}
		if(e.getErrCode()==1653){
			Log.log(MBC_DB_TABLESPACE_SMALL,LOG_LEVEL_FATAL,LOG_TYPE_SYSTEM,e.getErrMsg()); 
			return -1;}
		if(e.getErrCode()==904||e.getErrCode()==1722){
			Log.log(MBC_DB_EXECUTE_FAIL,LOG_LEVEL_FATAL,LOG_TYPE_SYSTEM,e.getErrMsg()); 
			return -1;}
		else{
			Log.log(MBC_DB_LINK_CLOSE_FAIL,LOG_LEVEL_FATAL,LOG_TYPE_SYSTEM,e.getErrMsg()); 
			return -1;}			
			
	}
	return 0;
	
}

