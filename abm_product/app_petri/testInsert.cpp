#include "petri.h"

namespace PETRI {



bool testTokenIn(string s_state)
{

	DEFINE_PETRI_QUERY(query);
	int i ,i_size;
	int a_plcID[PLACENUM],a_plcToknum[PLACENUM];
	string a_plcState[PLACENUM];
	i_size = 10;
	for (i = 0;i< i_size;i++ ) {
		a_plcState[i] = s_state;
		a_plcToknum[i] =i ;
		a_plcID[i]=i;
	};

	char stateArray[PLACENUM][STATE_LENGTH+1];
	for (i = 0;i <i_size;i++ ) {
		memset(stateArray[i],0,STATE_LENGTH+1);
		strncpy(stateArray[i],a_plcState[i].c_str(),STATE_LENGTH);
	};

	try {
	
		long i;
		string sql =  "SELECT TRANSITION_NAME FROM P_TRANSITION_ATTR WHERE TRANSITION_ID = :ID";
		for (i =200;i<300;i++) {
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("ID",i);
			query.open();
			while(query.next()) {
				printf("Transition_ID=%ld,Transition_name = %s\n",i,query.field(0).asString());
			}
		}
		query.close();

			/*
		string sql = "INSERT INTO P_ACTION(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE) VALUES(SEQ_PETRI_ACTION_ID.NEXTVAL,:ELMID,'P','PLACE_STATE',:STATE,'U',SYSDATE)";
		query.close();
		query.setSQL(sql.c_str());
		query.setParamArray("ELMID",a_plcID,sizeof(long));
		query.setParamArray("STATE",(char**)stateArray,STATE_LENGTH+1,STATE_LENGTH);
		query.execute(i_size);

		sql.clear();
		sql = "INSERT INTO P_ACTION(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE) VALUES(SEQ_PETRI_ACTION_ID.NEXTVAL,:ELMID,'P','TOKEN_CNT',TO_CHAR(:TNUM),'D',SYSDATE)";
		query.close();
		query.setSQL(sql.c_str());
		query.setParamArray("ELMID",a_plcID,sizeof(int));
		query.setParamArray("TNUM",a_plcToknum,sizeof(int));
		query.execute(i_size);

		P_DB.getDB()->commit();

		sql.clear();
		sql = "SELECT ATTR_VALUE FROM P_ACTION WHERE ELEMENT_ID = :ELMID";
		query.setSQL(sql.c_str());
		query.setParamArray("ELMID",a_plcID,sizeof(long));
		query.open(i_size);
		while (query.next()) {
			printf("%s\n",query.field(0).asString());
		};
		*/



	} catch(TOCIException & toe) {
		printf("SQL err:%d\n",toe.getErrCode());
		return false;
	} catch(...) {
		printf("SQL unknown err\n");
		return false;
	}

	return true;

};








};



int main()
{


	string state="XXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
	if ( PETRI::testTokenIn(state)) {
		printf ("insert success\n");
	} else {
		printf ("insert fail\n");
	}

	return 0;
}
