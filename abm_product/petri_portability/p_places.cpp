#include "petri.h"

void 
PETRI::places::loadInPlcs()
{
	/* Data Model Changed
	string sql= " \
		SELECT A.PLACE_ID,DECODE(E.ARC_TYPE,'P',1,0) PLACE_TYPE,C.PLACE_CUR_STATE,C.TOKEN_CNT	\
		FROM P_TRANSITION_PLACE_RELA A,P_PLACE_ATTR C,P_ARC_ATTR D,P_ARC_CHART_ATTR E	\
		WHERE A.TRANSITION_ID = :TRANSID	\
		AND A.PLACE_ID=C.PLACE_ID	\
		AND A.PLACE_TYPE='INT'	\
		AND D.ARC_START_TYPE='P'	\
		AND D.ARC_END_TYPE='T'	\
		AND D.ARC_START_ID=A.PLACE_ID	\
		AND D.ARC_ID=E.ARC_ID \
		AND D.ARC_END_ID=A.TRANSITION_ID	ORDER BY PLACE_TYPE \
	";
	*/


	try {
		string sql = " \
			SELECT DISTINCT A.PLACE_ID,DECODE(E.ARC_TYPE,'P',1,0) PLACE_TYPE,NVL(C.PLACE_CUR_STATE,'0'),C.TOKEN_CNT ,C.DRAW_ID,C.PLACE_NAME \
			FROM P_TRANSITION_PLACE_RELA A,P_PLACE_ATTR C,P_ARC_CHART_ATTR E  \
			WHERE A.TRANSITION_ID = :TRANSID \
			AND A.PLACE_ID=C.PLACE_ID  \
			AND A.PLACE_TYPE='INT'   \
			AND A.ARC_ID=E.ARC_ID \
			ORDER BY PLACE_TYPE  \
			";
		DEFINE_PETRI_QUERY(query);
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("TRANSID",l_transID);
		query.open();
		while(query.next()) {
			/*
			inPlcs.v_plcID.push_back(query.field(0).asInteger());
			inPlcs.v_plcType.push_back(query.field(1).asInteger());
			inPlcs.v_plcState.push_back(query.field(2).asString());
			inPlcs.v_plcToknum.push_back(query.field(3).asInteger());
			*/
	
			a_plcID[i_size] = query.field(0).asInteger();
			a_plcType[i_size] = query.field(1).asInteger();
			a_plcState[i_size] = query.field(2).asString();
			a_plcToknum[i_size] = query.field(3).asInteger();
			a_plcDrawID[i_size] = query.field(4).asLong();
			strncpy(a_plcName[i_size],query.field(5).asString(),NAME_LENGTH);
			i_size++;

		}
		query.close();


	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());

	} catch(...) {
		return;

	}
	return;

};

void
PETRI::places::loadOutPlcs(int i_subnet)
{
	string	sql2;
	if (i_subnet){
		sql2 = "SELECT C.PLACE_ID,C.TOKEN_CNT,C.DRAW_ID,C.PLACE_NAME \
    	FROM P_TRANSITION_ATTR A,P_DRAW B,P_PLACE_ATTR C \
    	WHERE A.SUBNET_DRAW_ID = B.DRAW_ID \
    	AND B.BEG_PLACE_ID = C.PLACE_ID \
    	AND A.TRANSITION_ID = :TRANSID";
	}else{
		sql2 = "SELECT DISTINCT A.PLACE_ID,B.TOKEN_CNT,B.DRAW_ID,B.PLACE_NAME \
		FROM P_TRANSITION_PLACE_RELA A,P_PLACE_ATTR B \
		WHERE PLACE_TYPE='OUT' AND TRANSITION_ID=:TRANSID \
		AND A.PLACE_ID=B.PLACE_ID";
	}

	DEFINE_PETRI_QUERY(query);
	try {
		query.close();
		query.setSQL(sql2.c_str());
		query.setParameter("TRANSID",l_transID);
		query.open();
		while(query.next()) {
			a_plcID[i_size] = query.field(0).asInteger();
			a_plcToknum[i_size] = query.field(1).asInteger();
			a_plcDrawID[i_size] = query.field(2).asLong();
			strncpy(a_plcName[i_size],query.field(3).asString(),NAME_LENGTH);
			a_plcType[i_size] = 0;
			//a_plcsState[i+size] = query.field(2).asString();
			i_size++;
		};
		if ( i_size> 0 ) {
			sql2.clear();
			sql2 = "SELECT STATE FROM P_PLACE_STATE_ENUM WHERE PLACE_ID = :ID	";
			query.close();
			query.setSQL(sql2.c_str());
			query.setParameter("ID",a_plcID[0]);
			query.open();
			v_plcStatelist.clear();
			while(query.next()) {
				v_plcStatelist.push_back(query.field(0).asString());
			};
			query.close();
		}


	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());

	} catch(...) {

	}
	return;

};




void PETRI::places::erase()  // 需要重写
{

	l_transID = -1;
	i_plcKind = -1;
	i_size = 0;
	v_plcStatelist.clear();
	for (int i =0 ;i<PLACENUM;i++ ) {
		a_plcID[i]=0;
		a_plcType[i] = -1;
		a_plcToknum [i] = 0;
		a_plcState[i].clear();
	};

};


int PETRI::places::lock()
{
	int iRet = 0;
	string sql = "SELECT 1 FROM P_PLACE_ATTR WHERE PLACE_ID = :PLCID FOR UPDATE NOWAIT";

	try {
		DEFINE_PETRI_QUERY(query);

		query.commit();
		int idx;
		for (idx = 0 ;idx <i_size;idx++) {
			try {
				query.close();
				query.setSQL(sql.c_str());
				query.setParameter("PLCID",a_plcID[idx]);
				query.open();
			} catch(TOCIException & toe){
				if ( toe.getErrCode () == 54) {
					iRet = 1;
				} else {
					iRet = -1;
				}
				query.rollback();
				break;
			};
		}
		query.close();
 
	} catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
	} catch(...) {
		iRet = -1;
	}
	return iRet;

};



/*
int PETRI::places::lock()
{
	DEFINE_PETRI_QUERY(query);
	int iRet = 0;
	string sql = "SELECT 1 FROM P_PALCE_ATTR WHERE PLACE_ID = :PLCID FOR UPDATE NOWAIT";
	try {
		query.close();
		query.commit();
		query.setSQL(sql.c_str());
		query.setParamArray("PLCID",a_plcID,sizeof(int));
		query.open();
	} catch(TOCIException & toe){
		if (toe.getErrCode() == 54) 
			iRet = 1;
		else 
			iRet = -1;
	} catch(...) {
		iRet = -1;
	}
	return iRet;

};
*/

void PETRI::places::setIn()
{
	i_plcKind = 0;
};

void PETRI::places::setOut()
{
	i_plcKind = 1;
};

void PETRI::places::load(int i_subnet)
{
	if (i_plcKind == 0 ) {
		loadInPlcs();
	} else if (i_plcKind == 1) {
		loadOutPlcs(i_subnet);
	} else {

	}
};

bool PETRI::places::tokenOut()
{
	int i ;

	/*
	for (i=0;i<inPlcs.v_plcsID.size();i++) 
		if (inPlcs.v_plcsType) break; 
	if (!i) return false;
	v_plcs.clear();
	copy(inPlcs.v_plcID.begin(),inPlcs.v_plcID+i,v_plcs);
	*/

	for ( i =0 ;i<i_size; i++ ) {
		if (a_plcType[i]){
			continue;
		}
		if (a_plcToknum[i] ){
			a_plcToknum[i]--;
		}
		else {
			printf("%s,%d__TokenMoveOutErr:placeID = %ld,Lefttoken_cnt = %d,transID = %ld\n",
						__FILE__,__LINE__,a_plcID[i],a_plcToknum[i],l_transID);
			return false;
		}
		
	}

	if (!i) return false;

	try {

		DEFINE_PETRI_QUERY(query);
		string sql="UPDATE P_PLACE_ATTR SET TOKEN_CNT=:TNUM WHERE PLACE_ID= :PLCID ";
		query.close();
		query.setSQL(sql.c_str());
		query.setParamArray("PLCID",a_plcID,sizeof(long));
		query.setParamArray("TNUM",a_plcToknum,sizeof(int));
		query.execute(i);

		sql.clear();
		sql = "INSERT INTO P_ACTION(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE,DRAW_ID,ACTION_TYPE,ELEMENT_NAME) VALUES(SEQ_PETRI_ACTION_ID.NEXTVAL,:ELMID,'P','TOKEN_CNT',TO_CHAR(:TNUM),'D',SYSDATE,:DRAWID,'A',:ELMNAME)";
		query.close();
		query.setSQL(sql.c_str());
		query.setParamArray("ELMID",a_plcID,sizeof(long));
		query.setParamArray("TNUM",a_plcToknum,sizeof(int));
		query.setParamArray("DRAWID",a_plcDrawID,sizeof(long));
		query.setParamArray("ELMNAME",(char**)a_plcName,NAME_LENGTH+1,NAME_LENGTH);
		query.execute(i);


	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
		return false;

	} catch(...) {
		return false;
	}
	
	return true;
};

/*
bool PETRI::places::tokenOut(int iPos)
{
	int i ;

//	for (i=0;i<inPlcs.v_plcsID.size();i++) 
//		if (inPlcs.v_plcsType) break; 
//	if (!i) return false;
//	v_plcs.clear();
//	copy(inPlcs.v_plcID.begin(),inPlcs.v_plcID+i,v_plcs);

	if (a_plcsType[iPos] )return false;

	if (!i) return false;

	try {

		DEFINE_PETRI_QUERY(query);
		string sql="UPDATE P_PLACE_ATTR SET TOKEN_NUM=TOKEN_NUM-1 WHERE PLACE_ID= :PLCID AND TOKEN_NUM>0";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("PLCID",a_plcID.[iPos]);
		query.open();


	}catch (TOCIException & toe){
		printf("%s\n",toe.getErrMsg());
		printf("%s\n",toe.getErrSrc());
		return false;
	} catch(...) {
		return false;
	}

	return true;
};
*/



bool PETRI::places::tokenIn(string s_state)
{
	DEFINE_PETRI_QUERY(query);
	int i ;
	/*
	for (i = 0;i< i_size;i++ ) {
		a_plcState[i] = s_state;
		a_plcToknum[i] ++ ;
	};*/
		//check in state
	char sSql[1024] = {0};
	int n = 0;

	for (i = 0;i< i_size;i++ ) {
		//add by jx for invalid state
		memset(sSql,0,1024);
		sprintf(sSql,"select count(1) from P_PLACE_STATE_ENUM a where a.PLACE_ID=%ld and a.state='%s'",a_plcID[i],s_state.c_str());
		query.close();
		query.setSQL(sSql);
		query.open();
		while(query.next())
		{
			n = query.field(0).asInteger();
		}
		query.close();
		if ( n == 0)
		{
			continue;
		}
		
		
		a_plcState[i] = s_state;
		a_plcToknum[i] ++ ;
	};
	char stateArray[PLACENUM][STATE_LENGTH+1];
	for (i = 0;i <i_size;i++ ) {
		memset(stateArray[i],0,STATE_LENGTH+1);
		strncpy(stateArray[i],a_plcState[i].c_str(),STATE_LENGTH);
	};

	try {
		string sql = "UPDATE P_PLACE_ATTR SET TOKEN_CNT = :TNUM \
									,PLACE_CUR_STATE = :STATE WHERE PLACE_ID= :PLCID ";
		query.close();
		query.setSQL(sql.c_str());
		query.setParamArray("TNUM",a_plcToknum,sizeof(int));
		query.setParamArray("STATE",(char**)stateArray,STATE_LENGTH+1,STATE_LENGTH);
		query.setParamArray("PLCID",a_plcID,sizeof(long));
		query.execute(i_size);

		sql.clear();
		sql = "INSERT INTO P_ACTION(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE,DRAW_ID,ACTION_TYPE,ELEMENT_NAME) VALUES(SEQ_PETRI_ACTION_ID.NEXTVAL,:ELMID,'P','PLACE_STATE',:STATE,'U',SYSDATE,:DRAWID,'A',:ELMNAME)";
		query.close();
		query.setSQL(sql.c_str());
		query.setParamArray("ELMID",a_plcID,sizeof(long));
		query.setParamArray("STATE",(char**)stateArray,STATE_LENGTH+1,STATE_LENGTH);
		query.setParamArray("DRAWID",a_plcDrawID,sizeof(long));
		query.setParamArray("ELMNAME",(char**)a_plcName,NAME_LENGTH+1,NAME_LENGTH);

		query.execute(i_size);

		sql.clear();
		sql = "INSERT INTO P_ACTION(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE,DRAW_ID,ACTION_TYPE,ELEMENT_NAME) VALUES(SEQ_PETRI_ACTION_ID.NEXTVAL,:ELMID,'P','TOKEN_CNT',TO_CHAR(:TNUM),'A',SYSDATE,:DRAWID,'A',:ELMNAME)";
		query.close();
		query.setSQL(sql.c_str());
		query.setParamArray("ELMID",a_plcID,sizeof(long));
		query.setParamArray("TNUM",a_plcToknum,sizeof(int));
		query.setParamArray("DRAWID",a_plcDrawID,sizeof(long));
		query.setParamArray("ELMNAME",(char**)a_plcName,NAME_LENGTH+1,NAME_LENGTH);
		query.execute(i_size);


	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
		return false;

	} catch(...) {
		return false;
	}
	
	return true;

};


bool PETRI::places::tokenMove(string s_state)
{
	if (i_plcKind == 0 ) {
		return tokenOut();
	}
	else {
		return tokenIn(s_state);
	};

	

};

/*
bool PETRI::places::tokenMove(int iPos,string s_state)
{
	if (i_plcsKind == 0 ) {
		return tokenOut(iPos);
	}
	else {
		//return tokenIn(iPos ,placeID);
	};

	

};
*/

bool PETRI::places::in(const string& s1,const vector<string>& v1) 
{
	unsigned int i=0;
	for (i =0;i<v1.size();i++) {
		if (s1 == v1[i])
			return true;
	};

	return false;

};

bool PETRI::places::checkFire(const vector<string>& stateList) //检查库所令牌数是否符合点火条件
{
	bool bFire = true;
	int i,j;

	j=i_size;
	//printf("%s,%d__i_size = %d\n",__FILE__,__LINE__,j);

	//点火判断 
	if (!j) {
		bFire = false;
		return bFire;
	};


	if ( stateList.size() ) { //异或变迁

		for  ( i=0;i<j;i++) {
			if ( 
				!a_plcType[i] && ( a_plcToknum[i]<= 0 ||  !in(a_plcState[i],stateList) )
				) 
			{
				bFire = false;
				break;
			};
			if ( a_plcType[i] && a_plcToknum[i]>0) 
			{
				bFire = false;
				break;
			};

		};

	} else { //与变迁
		for  ( i=0;i<j;i++) {
			if ( !a_plcType[i] &&  a_plcToknum[i] <= 0) {
				bFire = false;
				break;
			};
			if ( a_plcType[i] && a_plcToknum[i] >0 ) {
				bFire = false;
				break;
			}

		};

	}


	return bFire;

};


//进入子网
bool PETRI::places::getFirstPlace(long drawID)  
{

	DEFINE_PETRI_QUERY(query);
	string sql = " SELECT NVL(BEG_PLACE_ID,-1) FROM P_DRAW WHERE DRAW_ID=:DRAWID";
	long beg_plc = -1;
	try {
		query.setSQL(sql.c_str());
		query.setParameter("DRAWID",drawID);
		query.open();
		if (query.next()) {
			beg_plc = query.field(0).asInteger();
		};
	} catch (...)  {
		return false;
	}

	if (beg_plc == -1 )
		return false;

	erase();
	a_plcID[i_size++] = beg_plc;
	return true;

};

//进入父网
bool PETRI::places::CheckSubnetEndPlcs()  
{
	if ((i_size!=1) || !i_plcKind )
		return false;
	DEFINE_PETRI_QUERY(query);
	long endPlcID = -1;
	try {
		string sql="SELECT END_PLACE_ID FROM P_DRAW WHERE DRAW_ID=:DRAWID";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("DRAWID",a_plcDrawID[0]);
		query.open();
		if (query.next() ) {
			endPlcID = query.field(0).asInteger();
		} else {
			query.close();
			return false;
		}
	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());	
		return false;
	}catch(...){
		query.close();
		return false;
	}
	
	if ( endPlcID != a_plcID[0] ){
		query.close();
		return false;
	}
	
	query.close();
	return true;
}

string  PETRI::places::getEndPlcsState()
{
	return a_plcState[0];
}

long PETRI::places::getParentTransID()
{

//	if (a_plcToknum[0] ){
//		a_plcToknum[0]--;
//	}
	long PtransID = -1;
	DEFINE_PETRI_QUERY(query);
	try {
/*			string sql= "UPDATE P_PLACE_ATTR SET TOKEN_CNT = TOKEN_CNT-1 \
					 WHERE PLACE_ID= :PLCID \
					 AND TOKEN_CNT >0 ";
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("PLCID",a_plcID[0]);
			query.execute();

			sql.clear();
			sql = "INSERT INTO P_ACTION(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE,DRAW_ID,ACTION_TYPE,ELEMENT_NAME) VALUES(SEQ_PETRI_ACTION_ID.NEXTVAL,:ELMID,'P','TOKEN_CNT',TO_CHAR(:TNUM),'D',SYSDATE,:DRAWID,'A',:ELMNAME)";
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("ELMID",a_plcID[0]);
			query.setParameter("TNUM",a_plcToknum[0]);
			query.setParameter("DRAWID",a_plcDrawID[0]);
			query.setParameter("ELMNAME",a_plcName[0]);
			query.execute();

			erase();

			sql.clear();
*/			string sql = "SELECT TRANSITION_ID FROM P_TRANSITION_ATTR WHERE SUBNET_DRAW_ID = :DRAWID";
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("DRAWID",a_plcDrawID[0]);
			query.open();
			if (query.next() ) {
				PtransID = query.field(0).asInteger();
			} else {
				query.close();
				return PtransID;
			}
		}catch (TOCIException & toe){
			LOG(toe.getErrMsg());
			LOG(toe.getErrSrc());	
			return PtransID;
		}catch(...){
			query.close();
			return PtransID;
		}

	query.close();
	return PtransID;
//	setOut();
//	load();
	
}



void PETRI::places::getNextTrans(vector <long> & vt)
{
	//if (!i_plcKind) return; 
	try {
		DEFINE_PETRI_QUERY(query);

		string sql = "SELECT DISTINCT TRANSITION_ID FROM P_TRANSITION_PLACE_RELA WHERE PLACE_ID=:PLCID AND PLACE_TYPE='INT'";

		int idx;
		vt.clear();
		for (idx=0;idx<i_size;idx++) {
			try {
				query.close();
				query.setSQL(sql.c_str());
				query.setParameter("PLCID",a_plcID[idx]);
				query.open();
				while(query.next()) {
					vt.push_back(query.field(0).asLong());
				}


			} catch (TOCIException & toe){
				LOG(toe.getErrMsg());
				LOG(toe.getErrSrc());
			} catch (...) {

			}
		};


	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
	} catch (...) {

	};

};


bool PETRI::places::checkState(const string& in_state)
{
	bool bRet = false;
	bRet = in(in_state,v_plcStatelist);
	if (!bRet) {
		string sStates;
		sStates.clear();
		unsigned int i;
		for (i = 0;i< v_plcStatelist.size();i++ ) {

			if (i == 0) {
				sStates += v_plcStatelist[i];
			} else {
				sStates += ",";
				sStates += v_plcStatelist[i];
			}

		};

		string sErrMsg = "TransState:"+in_state+" not in the stateList of outplaces:"+sStates;
		DEFINE_PETRI_QUERY(query);
		try {
			string sql =" \
				INSERT INTO P_ACTION \
				(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE,ERR_MSG,DRAW_ID,ACTION_TYPE) \
				VALUES \
				(SEQ_PETRI_ACTION_ID.NEXTVAL,:ELEMID,'T','EXEC_RESULT',  NULL,'U',SYSDATE, \
				 SUBSTR(:MSG,0,256),:DRAWID,'A') \
				";
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("ELEMID",l_transID);
			query.setParameter("MSG",sErrMsg.c_str());
			query.setParameter("DRAWID",a_plcDrawID[0]);
			//query.setParameter("ELMNAME",ts->getName());
	
			query.execute();

		} catch (TOCIException &toe) {
			LOG(toe.getErrMsg());
			query.close();
			return bRet;
		} catch (...) {
			query.close();
			return bRet;
		}

    	query.commit();
		query.close();

	};
    
	return bRet;
};

void	PETRI::places::setTransID(long transID)
{
	l_transID = transID;
};


