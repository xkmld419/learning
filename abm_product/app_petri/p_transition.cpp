#include "petri.h"

namespace PETRI {

//判断变迁是否在执行
bool transition::checkExec()
{
	if (s_execState == "R") { // 判断条件待定
		return true;
	} else {
		return false;
	}
};

bool transition::checkEnd()
{
	if (s_execState == "E") { 
		return true;
	} else {
		return false;
	}
};




bool
transition::load()
{
	bool ret=false;
	DEFINE_PETRI_QUERY(query);
	try {

		string sql = " \
			SELECT A.DRAW_ID,A.TRANSITION_NAME,  	\
			DECODE(CON_TYPE,'AND',0,'XOR',1,0) CONTYPE,	\
			DECODE (TRANSITION_CLASS,'PROCESS',0,'SQL',1,'SHELL',3,-1) EXETYPE,	\
			TRANSITION_OBJ_ID ,	\
			DECODE(SUBNET_TYPE,'T',0,'N',1,0) SUBNET_TYPE,	\
			SUBNET_DRAW_ID ,	\
			DECODE(A.TRANSITION_EXEC_ATTR,'A',0,'M',1,0) EXECATTR,	\
			DECODE(A.TIME_TYPE,'Immediate',0,'Timed',1,0) TIME_TYPE,	\
			EXE_STATE \
			FROM P_TRANSITION_ATTR  A WHERE TRANSITION_ID=:TRANSID	\
			";

		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("TRANSID",l_transID);
		query.open();
		if (query.next()){
			l_drawID = query.field("DRAW_ID").asLong();
			i_conType = query.field("CONTYPE").asInteger();
			//s_instate = query.field("in_state").asString();
			i_execType = query.field("EXETYPE").asInteger();
			l_execObjID = query.field("TRANSITION_OBJ_ID").asInteger();
			i_subnetType = query.field("SUBNET_TYPE").asInteger();
			l_subDrawID = query.field("SUBNET_DRAW_ID").asLong();
			i_execAttr = query.field("EXECATTR").asInteger();
			i_timeType = query.field("TIME_TYPE").asInteger();
			s_execState = query.field("EXE_STATE").asString();
			strcpy(s_transName,query.field("TRANSITION_NAME").asString());
			ret = true;
		};

		//load state list
		sql.clear();
		sql = "SELECT STATE,DECODE(STATE_TYPE,'I',0,'O',1,0) TYPE \
					 FROM P_TRANSITION_STATE_ENUM WHERE TRANSITION_ID=:TS ORDER BY TYPE";


		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("TS",l_transID);
		query.open();
		v_inStateList.clear();
		v_outStateList.clear();
		while(query.next()) {
			if (query.field(1).asInteger() ) {
				v_outStateList.push_back(query.field(0).asString());
			}
			else {
				if (i_conType) { //异或变迁才保存输入状态列表
					v_inStateList.push_back(query.field(0).asString());
				}
			}
		}
		if (v_outStateList.size() == 0 ) {
			v_outStateList.push_back("0");// default in state
		}

		// load timeplan
		if (i_timeType == 1 ) {
			sql.clear();
			/*
			sql = " \
				SELECT B.PLAN_ID, \
				LPAD(YEAR,4,'0')||LPAD(MONTH,2,'0')||LPAD(DAY,2,'0')||LPAD(HOUR,2,'0')||LPAD(MINUTE,2,'0')||'00' BEGIN_TIME, \
				PLAN_TYPE ,TIME_INTERVAL,INTERVAL_UNIT,TO_CHAR(PLAN_END_TIME,'YYYYMMDDHH24MISS') END_TIME ,A.TIME_OFFSET,\
				NVL(TO_number(B.WEEK_DAY),0) WEEK_DAY\
				FROM P_TRANSITION_TIME_PLAN A,WF_TIME_PLAN B  \
				WHERE A.PLAN_ID = B.PLAN_ID \
				AND A.TRANSITION_ID=:TS \
				";
				*/
				
			sql= "SELECT B.PLAN_ID, \
		        LPAD(decode(YEAR,'*',to_char(sysdate,'YYYY'), YEAR),4,'0')|| \
		        LPAD(decode(MONTH,'*',to_char(sysdate,'MM'), MONTH),2,'0')|| \
		        LPAD(decode(DAY,'*',to_char(sysdate,'DD'),DAY),2,'0')|| \
		        LPAD(HOUR,2,'0')||LPAD(MINUTE,2,'0')||'00' BEGIN_TIME,  \
		        PLAN_TYPE ,TIME_INTERVAL,INTERVAL_UNIT,TO_CHAR(PLAN_END_TIME,'YYYYMMDDHH24MISS') END_TIME ,A.TIME_OFFSET, \
       			NVL(TO_number(decode(B.WEEK_DAY,'*',0,B.WEEK_DAY)), 0) WEEK_DAY \
		        FROM P_TRANSITION_TIME_PLAN A,WF_TIME_PLAN B  \
		        WHERE A.PLAN_ID = B.PLAN_ID \
		        AND A.TRANSITION_ID= :TS";
				
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("TS",l_transID);
			query.open();
			timePlan ** pptp=&tPlan;
			char * p_str = 0;
			while(query.next()) {
				*pptp = new timePlan;
				memset(*pptp,0,sizeof(timePlan));
				(*pptp)->lPlanID = query.field(0).asLong();
				strncpy((*pptp)->sBeginTime,query.field(1).asString(),14);
				strncpy((*pptp)->sEndTime,query.field(5).asString(),14);
				p_str = query.field(2).asString();
				(*pptp)->cPlanType = p_str[0];
				(*pptp)->lTimeInterval = query.field(3).asLong();
				p_str= query.field(4).asString();
				(*pptp)->cIntervalUnit = p_str[0];
				(*pptp)->lTimeOffset = query.field(6).asLong();
				(*pptp)->iWeekDay = query.field(7).asInteger();
				pptp = &((*pptp)->nextPlan);
			}
			query.close();
		};

		if (3 == i_execType){
			sql.clear();
		/*
			sql = "SELECT PROCESS_ID FROM P_TRANSITION_SH_HBPROCESS WHERE TRANSITION_ID = :TS ";
			*/
			sql = "SELECT APP_OBJECT_ID,APP_TYPE FROM P_TRANSITION_SH_APP_OBJECT WHERE TRANSITION_ID = :TS ";

		
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("TS",l_transID);
			query.open();
			if(query.next()){
				l_shProcessID = query.field(0).asLong();
				l_appTypeID = query.field(1).asLong();

			}

			query.close();
		}	
		
	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
		return false;
	}catch (...) {
		return false;
	}

	getTransactionID();

	return ret;

};

bool transition::initExec()
{
	switch (i_execType) { //初始化执行类
		case 0:
			tExec = new execProcess(this);
			break;
		case 1:
			tExec = new execSQL(this);
			break;
		case 3:
			tExec = new execShell(this);
			break;
		default:
			tExec = NULL;
			break;
	};
	if (tExec) {
		tExec->init();
		return true;
	} else {
		return false;
	}
}


void	transition::getParamList(vector<string>& vParamList)
{
	vParamList.clear();
	vector<long> vParamID;
	vector<string> vParamValue;
	vector<int> vGetMethod;
	string sParamValue;

	DEFINE_PETRI_QUERY(query);
	try {
		string sql = " \
			SELECT NVL(A.PARAM_ID,-1) PARAM_ID,A.PARAM_VALUES,DECODE(NVL(B.PARAM_GET_METHOD,'M'),'M',1,0) METHOD \
			FROM P_TRANSITION_PARAM A,P_COMM_PARAM B \
			WHERE A.PARAM_ID = B.PARAM_ID(+) \
			AND A.TRANSITION_ID = :ID \
			ORDER BY PARAM_ORDER \
			";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("ID",l_transID);
		query.open();

		while(query.next()) {
			vParamID.push_back(query.field(0).asLong());
			sParamValue = (query.field(1).asString());
			vParamValue.push_back(sParamValue);
			vGetMethod.push_back(query.field(2).asInteger());
		}
		query.close();

	} catch (TOCIException & toe) {
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
	} catch (...) {

	}
	unsigned int iParamNum = vParamID.size();
	vParamList.resize(iParamNum);

	for (unsigned int  i = 0;i <iParamNum;i++) {

		if (vGetMethod[i] == 0) {/*Auto Param*/
			vParamList[i] = parseCommParam(vParamID[i]);
		} else { /* Manual Param*/
			vParamList[i] = vParamValue[i];
		}

	}

	return;
}

string transition::parseCommParam(long  lParamID)
{
	char sParamValue[128];
	memset(sParamValue,0,sizeof(sParamValue));
	Date now;
	DEFINE_PETRI_QUERY(query);
	switch (lParamID) {
		case 1://transaction_id
			sprintf(sParamValue,"%ld",l_transactionID);
			break;
			
		case 2://transition_id
			sprintf(sParamValue,"%ld",l_transID);
			break;

		case 3://D billing_cycle_id
		case 4://D billing_cycle.cycle_begin_date
		case 5://D billing_cycle.cycle_end_date
			try {
				query.close();
				string sql = "SELECT TO_CHAR(BILLING_CYCLE_ID) BILLING_CYCLE_ID, \
					TO_CHAR(CYCLE_BEGIN_DATE, 'YYYYMMDD') BEGIN_DATE, \
					TO_CHAR(CYCLE_END_DATE, 'YYYYMMDD') END_DATE \
					FROM BILLING_CYCLE \
					WHERE BILLING_CYCLE_ID = \
					(SELECT MIN(BILLING_CYCLE_ID) \
					FROM BILLING_CYCLE \
					WHERE STATE IN ('10R','10B', '10A', '10D') \
					AND BILLING_CYCLE_TYPE_ID = 1)";
				query.setSQL(sql.c_str());
				query.open();
				if (query.next()) {
					strcpy(sParamValue,query.field(lParamID-3).asString());
				}
			}catch (TOCIException & toe){
				LOG(toe.getErrMsg());
				LOG(toe.getErrSrc());
			}catch (...) {
			}
			query.close();
			break;

		case 7://yesterday "yyyymmdd"
		case 8://today "yyyymmdd"
		case 9://tomorrow "yyyymmdd"
				now.addDay(lParamID-8);
				now.getTimeString(sParamValue,"yyyymmdd");
				break;

		case 6://New billing_cycle.cycle_begin_date
			try {
				query.close();
				string sql = "SELECT TO_CHAR(CYCLE_BEGIN_DATE, 'YYYYMMDD') BEGIN_DATE \
					FROM BILLING_CYCLE \
					WHERE BILLING_CYCLE_ID = \
					(SELECT MAX(BILLING_CYCLE_ID) \
					FROM BILLING_CYCLE \
					WHERE STATE = '10R' \
					AND BILLING_CYCLE_TYPE_ID = 1)";
				query.setSQL(sql.c_str());
				query.open();
				if (query.next()) {
					strcpy(sParamValue,query.field(0).asString());
				}
			}catch (TOCIException & toe){
				LOG(toe.getErrMsg());
				LOG(toe.getErrSrc());
			}catch (...) {
			}
			query.close();
			break;

		case 11://New billing_cycle_id
			try {
				int ilastbillid =0;
				query.close();
				string sql = "SELECT BILLING_CYCLE_ID \
					FROM BILLING_CYCLE \
					WHERE BILLING_CYCLE_ID = \
					(SELECT MIN(BILLING_CYCLE_ID) \
					FROM BILLING_CYCLE \
					WHERE STATE IN ('10R','10B', '10A', '10D') \
					AND BILLING_CYCLE_TYPE_ID = 1)";
				query.setSQL(sql.c_str());
				query.open();
				if (query.next()) {
					ilastbillid = query.field(0).asInteger();
				}
				
				query.close();
				string sql2 = "SELECT TO_CHAR(BILLING_CYCLE_ID) BILLING_CYCLE_ID \
					FROM BILLING_CYCLE \
					WHERE LAST_BILLING_CYCLE_ID = :LB";
				query.setSQL(sql2.c_str());
				query.setParameter("LB",ilastbillid);
				query.open();
				if (query.next()) {
					strcpy(sParamValue,query.field(0).asString());
				}
			}catch (TOCIException & toe){
				LOG(toe.getErrMsg());
				LOG(toe.getErrSrc());
			}catch (...) {
			}
			query.close();
			break;

		case 14://staffID
					sprintf(sParamValue,"%ld\0",l_staffID);
							
		break;



	}
	return string(sParamValue);

}

bool transition::checkTime()
{
	if (i_timeType != 1 || !tPlan )
		return true;

	DEFINE_PETRI_QUERY (query1);
	char nowtTime[30];
	timePlan *tmptp = tPlan;
	string sql2 = "select to_char(sysdate,'yyyymmddhh24miss') from dual	";
	query1.close();
	query1.setSQL(sql2.c_str());
	query1.open();
	while (query1.next())
	{
		strcpy(nowtTime,query1.field(0).asString());
	}
	
	Date now(nowtTime);
	for (;tmptp ;tmptp = tmptp->nextPlan) {
		try {
			if (strlen(tmptp->sEndTime) == 14 ){
				Date end(tmptp->sEndTime,"YYYYMMDDHHMISS");
				if (now > end)  continue;
			}
			Date begin(tmptp->sBeginTime,"YYYYMMDDHHMISS");
			Date offset(tmptp->sBeginTime,"YYYYMMDDHHMISS");
			offset.addMin(tmptp->lTimeOffset);


			if (tmptp->cPlanType == 'S') {
					if ((strcmp(now.toString(),offset.toString())>=0) && (0 == tmptp->iWeekDay ||now.getWeek() == tmptp->iWeekDay)) 
						return true;
					else
						continue;
			}

			while(offset <= now) {
				if ((0 == tmptp->iWeekDay ||now.getWeek() == tmptp->iWeekDay)) return true;

				switch(tmptp->cIntervalUnit) {
					case 'M':
						offset.addMonthAsOracle(tmptp->lTimeInterval);
						begin.addMonthAsOracle(tmptp->lTimeInterval);
						break;
					case 'D':
						offset.addDay(tmptp->lTimeInterval);
						begin.addDay(tmptp->lTimeInterval);
						break;
					case 'H':
						offset.addHour(tmptp->lTimeInterval);
						begin.addHour(tmptp->lTimeInterval);
						break;
					case 'T':
						offset.addMin(tmptp->lTimeInterval);
						begin.addMin(tmptp->lTimeInterval);
						break;
					default:
						goto WHILE_END;
				}
			};
			WHILE_END:
			;

		} catch (...) {
			continue;

		}

	}

	return false;

};

bool transition::saveExecTime()
{
#ifdef HB_28
		char sMsg[1024];
		memset(sMsg,0,sizeof(sMsg));
		sprintf(sMsg,"[TransName]%s [StaffID]%ld   Start!!",s_transName,l_staffID);
	  Log::m_iAlarmLevel = 3;
	  Log::m_iAlarmDbLevel = -1;
	  LOGG(SYSTEMLOG,sMsg);
#endif

	DEFINE_PETRI_QUERY(query);	
	try {
		string sql = "INSERT INTO P_TRANSITION_TIME_LOG VALUES(:TS,SYSDATE)";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("TS",l_transID);
		query.execute();
		query.close();

	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
		query.close();
		return false;
	} catch(...) {
		query.close();
		return false;
	}
	return true;

};

bool transition::execute()
{
	/*
	if (i_subnetType && getFirstPlace()) { //子网
		return;
	};
	*/
	vector<string> paramList;
	string strParam;

	bool bRet = false;
	createLog();
//	Date now;
//	char execdate[15];
//	now.getTimeString(execdate);
//	execdate[14]=0;
//	string sexecMsg(execdate);
//	sexecMsg = "start at " + sexecMsg;
	switchState("R","");
//	if (i_timeType == 1 ) 
//		saveExecTime();

//	P_DB.getDB()->commit();

	if (initExec()) {
		getParamList(paramList);

		strParam.clear();
		strParam = " ";
		for(int j=0;j<paramList.size();j++){
			strParam = strParam + paramList[j] + " ";
		}
		updateTransExecLog("PARAMETER",strParam);

		s_execResult = tExec->doExec(paramList);
		P_DB.getDB(true);
	} else {
//		sleep(5);//sleep if Null Transition 
		s_execResult = "0";
	}


	//更改变迁状态
	if (s_execResult == "-999") { //前台中止 
		switchState("K","Killed");
//		if (tExec) {
//			string sErrMsg = "Killed";
//			tExec->saveErrMsg(sErrMsg.c_str());
//		}
	}else if(s_execResult == "-666"){

			switchState("K","ERR当前进程不存在");

	} else if (s_execResult == "999"){
		s_execResult = "0"; // Permanent process(like pricing) return
		bRet = true;
	} 
	else if(l_appTypeID == 1)
	{
		if (0!=s_execResult.compare("1")) 
		{
			string sEndMsg = "End! Result="+s_execResult;
			switchState("E","");
			bRet = true;
		} 
		else 
		{
			string sErrMsg = "The State:"+s_execResult+" Not In The StateList \n";
			switchState("X",sErrMsg);

		}

	} else {
		bool ifIn= false;
		unsigned int i;
		for (i = 0;i<v_outStateList.size();i++ ) {
			if (s_execResult == v_outStateList[i] ) {
				ifIn = true;
				break;
			}
		}
		if (ifIn) {
			string sEndMsg = "End! Result="+s_execResult;
			switchState("E","");
			bRet = true;
//			if (tExec) {
//				string sErrMsg = "End Success! Result="+s_execResult;
//				tExec->saveErrMsg(sErrMsg.c_str());
//			}
		} else {
			string sErrMsg = "The State:"+s_execResult+" Not In The StateList \n";
			switchState("X",sErrMsg);
//			if (tExec) {
//				string sErrMsg = "The State:"+s_execResult+" Not In The StateList \n";
//				tExec->saveErrMsg(sErrMsg.c_str());
//			}
		}
	}
	updateTransExecLog("EXEC_RESULT",s_execResult);
	updateTransExecLog("END_TIME");
	P_DB.getDB()->commit();


	return bRet;
};


void transition::kill()
{
	if (initExec()) {
		s_execResult = tExec->doKill();
	}
	if (s_execResult == "0"){
	DEFINE_PETRI_QUERY(query);	
		
		string sql;
		sql.clear();
		sql= "UPDATE P_TRANSITION_ATTR SET EXE_STATE='K' WHERE TRANSITION_ID=:TS";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("TS",l_transID);
		query.execute();
		string inMsg;
		sql.clear();
		sql = "INSERT INTO P_ACTION(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,\
					CREATE_DATE,DRAW_ID,ACTION_TYPE,ELEMENT_NAME,ERR_MSG) \
					VALUES(SEQ_PETRI_ACTION_ID.NEXTVAL,:TS,'T','EXEC_STATE','K','U',SYSDATE,:DRAWID,'A',:ELMNAME,:ERR_MSG)";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("TS",l_transID);
		query.setParameter("DRAWID",l_drawID);
		query.setParameter("ELMNAME",s_transName);
		query.setParameter("ERR_MSG",inMsg.c_str());
		query.execute();
		query.commit();
		query.close();


	} 
	return;

};

string transition::query()
{
	string s_queryMSG;
	if (initExec()) {
		getLastLogID();
		s_queryMSG = tExec->doQuery();
		return s_queryMSG;
	}else {
		s_queryMSG ="";
		return s_queryMSG;// return TRUE if NULL Transition
	}
}

bool transition::switchState(const string& inState,const string& inMsg)
{
	bool b_srcStateX = false;
	bool b_destStateX = false;
	int sunbnetX = 0;
	string tmpMsg;
	string sql;
	vector<long> v_transID;
	DEFINE_PETRI_QUERY(query);	

	if(s_execState.compare("X") == 0){
		b_srcStateX = true;
	}

	if(inState.compare("X") == 0){
		b_destStateX = true;
	}


	if (s_execState == inState)
		return true;
	else s_execState = inState;

	try {
		sql.clear();
		sql= "UPDATE P_TRANSITION_ATTR SET EXE_STATE=:ST WHERE TRANSITION_ID=:TS";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("ST",inState.c_str());
		query.setParameter("TS",l_transID);
		query.execute();

		sql.clear();
		sql = "INSERT INTO P_ACTION(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE,DRAW_ID,ACTION_TYPE,ELEMENT_NAME,ERR_MSG) VALUES(SEQ_PETRI_ACTION_ID.NEXTVAL,:TS,'T','EXEC_STATE',:ST,'U',SYSDATE,:DRAWID,'A',:ELMNAME,:ERR_MSG)";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("ST",inState.c_str());
		query.setParameter("TS",l_transID);
		query.setParameter("DRAWID",l_drawID);
		query.setParameter("ELMNAME",s_transName);
		if((inState.compare("X") == 0)||(inState.compare("K") == 0))
		{
			query.setParameter("ERR_MSG",inMsg.c_str());
		}
		else
		{
			query.setParameter("ERR_MSG",tmpMsg.c_str());
		}
		AutoPlaceSendMessage(l_transID,inState,s_transName);

		query.execute();

	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
		query.close();
		return false;
	} catch(...) {
		query.close();
		return false;
	}
		
	if(b_srcStateX || b_destStateX){
		try{
			sql.clear();
			sql= " SELECT TRANSITION_ID \
				FROM (SELECT TRANSITION_ID \
				FROM P_TRANSITION_ATTR \
				START WITH TRANSITION_ID = :TS1 \
				CONNECT BY PRIOR DRAW_ID = SUBNET_DRAW_ID) \
				WHERE TRANSITION_ID <> :TS2 ";
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("TS1",l_transID);
			query.setParameter("TS2",l_transID);
			query.open();
			while(query.next()) {
				v_transID.push_back(query.field(0).asLong());
			}
		}catch (TOCIException & toe){
			LOG(toe.getErrMsg());
			LOG(toe.getErrSrc());
			query.close();
			return false;
		} catch(...) {
			query.close();
			return false;
		}
	}



	if (0 == v_transID.size()){
		query.commit();
		query.close();
		return true;
	}

	if(b_destStateX){
		for (int i = 0;i< v_transID.size();i++ ) {
			try {
				sql.clear();
				sql = "UPDATE P_TRANSITION_ATTR SET EXE_STATE=:ST WHERE TRANSITION_ID=:TS";
				query.close();
				query.setSQL(sql.c_str());
				query.setParameter("ST",inState.c_str());
				query.setParameter("TS",v_transID[i]);
				query.execute();

				sql.clear();
				sql = "INSERT INTO P_ACTION \
					(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE,DRAW_ID,ACTION_TYPE,ELEMENT_NAME) \
					SELECT SEQ_PETRI_ACTION_ID.NEXTVAL,TRANSITION_ID,'T','EXEC_STATE',:ST,'U',SYSDATE,DRAW_ID,'A',TRANSITION_NAME \
					FROM P_TRANSITION_ATTR where transition_id= :TS ";
				query.close();
				query.setSQL(sql.c_str());
				query.setParameter("ST",inState.c_str());
				query.setParameter("TS",v_transID[i]);
				query.execute();
			}catch (TOCIException & toe){
				LOG(toe.getErrMsg());
				LOG(toe.getErrSrc());
				query.close();
				return false;
			} catch(...) {
				query.close();
				return false;
			}
		}
	}
	
	if(b_srcStateX){
	for (int i = 0;i< v_transID.size();i++ ) {
		try {
			sql.clear();
			sql = " SELECT COUNT(*) \
				FROM P_TRANSITION_ATTR \
				WHERE DRAW_ID = (SELECT SUBNET_DRAW_ID \
				FROM P_TRANSITION_ATTR \
				WHERE TRANSITION_ID = :TS) \
				AND EXE_STATE = 'X'";
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("TS",v_transID[i]);
			query.open();
			query.next();
			sunbnetX = query.field(0).asInteger();
		
			if(sunbnetX > 0){break;}

			sql.clear();
			sql = "UPDATE P_TRANSITION_ATTR SET EXE_STATE=:ST WHERE TRANSITION_ID=:TS";
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("ST","R");
			query.setParameter("TS",v_transID[i]);
			query.execute();

			sql.clear();
			sql = "INSERT INTO P_ACTION \
				(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE,DRAW_ID,ACTION_TYPE,ELEMENT_NAME) \
				SELECT SEQ_PETRI_ACTION_ID.NEXTVAL,TRANSITION_ID,'T','EXEC_STATE',:ST,'U',SYSDATE,DRAW_ID,'A',TRANSITION_NAME \
				FROM P_TRANSITION_ATTR where transition_id= :TS ";
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("ST","R");
			query.setParameter("TS",v_transID[i]);
			query.execute();
		}catch (TOCIException & toe){
			LOG(toe.getErrMsg());
			LOG(toe.getErrSrc());
			query.close();
			return false;
		} catch(...) {
			query.close();
			return false;
		}
	}
	}
	if(getSubnetType())
	{
		string tmp_sql;
		string tmp_buff;
		string tmpString;
		char alarmDesc[200];
		DEFINE_PETRI_QUERY(query1);
		tmp_sql = "INSERT INTO P_ALARM_BSN(BSN_ALARM_ID,TRANSACTION_ID,DRAW_ID,TRANSITION_ID,EXEC_STATE,STATE_DATE,ALARM_TYPE,ALARM_DESC,ALARM_FLAG,REMARK) \
			VALUES(seq_p_alarm_bsn_id.NEXTVAL,:TA,:DRAWID,:TS,:STATE,SYSDATE,:ALARM,:ELMNAME,'N',:MARK)";
		query1.close();
		query1.setSQL(tmp_sql.c_str());
		getTransactionID();
		tmp_buff = "switchState to " + inState;
		strcpy(alarmDesc,s_transName);
		strcat(alarmDesc,tmp_buff.c_str());
		query1.setParameter("TS",l_transID);
		query1.setParameter("TA",l_transactionID);
		query1.setParameter("DRAWID",l_drawID);
		query1.setParameter("STATE",s_execState.c_str());
		query1.setParameter("ELMNAME",alarmDesc);
		query1.setParameter("MARK",tmp_buff.c_str());
		if(s_execState.compare("X") == 0)
		{
			tmpString = "E";
			query1.setParameter("ALARM",tmpString.c_str());
		}
		else
		{
			tmpString = "S";
			query1.setParameter("ALARM",tmpString.c_str());
		}
		query1.execute();
		query1.commit();
		query1.close();
		
	}
	query.commit();
	query.close();

	return true;
}

bool transition::checkTimeforMessage(long BEGIN_DEFER)
{
	Date now;
	Date tempTime;

	if (i_timeType == 1 && !tPlan)
	{	
		tempTime =oldTime;
		tempTime.addMin(BEGIN_DEFER);
		if(now > tempTime)
		return true;	
	}
	if (i_timeType != 1 &&!tPlan )
		return true;


	timePlan *tmptp = tPlan;
	for (;tmptp ;tmptp = tmptp->nextPlan) {
		try {
			if (strlen(tmptp->sEndTime) == 14 ){
				Date end(tmptp->sEndTime,"YYYYMMDDHHMISS");
				if (now > end)  continue;
			}
			Date begin(tmptp->sBeginTime,"YYYYMMDDHHMISS");
			Date offset(tmptp->sBeginTime,"YYYYMMDDHHMISS");
			offset.addMin(tmptp->lTimeOffset);
			offset.addMin(BEGIN_DEFER);


			if (tmptp->cPlanType == 'S') {
					if (now >= begin && now <= offset && (0 == tmptp->iWeekDay ||now.getWeek() == tmptp->iWeekDay)) 
						return true;
					else
						continue;
			}

			while(begin <= now) {
				if (offset >= now && (0 == tmptp->iWeekDay ||now.getWeek() == tmptp->iWeekDay)) return true;

				switch(tmptp->cIntervalUnit) {
					case 'M':
						offset.addMonthAsOracle(tmptp->lTimeInterval);
						begin.addMonthAsOracle(tmptp->lTimeInterval);
						break;
					case 'D':
						offset.addDay(tmptp->lTimeInterval);
						begin.addDay(tmptp->lTimeInterval);
						break;
					case 'H':
						offset.addHour(tmptp->lTimeInterval);
						begin.addHour(tmptp->lTimeInterval);
						break;
					case 'T':
						offset.addMin(tmptp->lTimeInterval);
						begin.addMin(tmptp->lTimeInterval);
						break;
					default:
						goto WHILE_END;
				}
			};
			WHILE_END:
			;

		} catch (...) {
			continue;

		}

	}

	return false;

}

void  transition::loadForMessage(message &ts,long transID)
{

	char sql[1024];
	ts.m_size =0;
	DEFINE_PETRI_QUERY (query);

	try {
			strcpy(sql, "	SELECT RULE_ID,TRANSITION_ID,STAFF_GROUP_ID,BEGIN_DEFER,TIME_OFFSET, ALARM_COUNT,ALARM_INTERVAL,ALARM_TYPE_TIME,ALARM_TYPE_BEGIN,ALARM_TYPE_END,ALARM_TYPE_ERR	\
				FROM P_ALARM_RULE WHERE TRANSITION_ID = :ID");
			query.close();
			query.setSQL(sql);
			query.setParameter("ID",transID);
			query.open();
			while(query.next()) {

				ts.m_RULE_ID[ts.m_size]			  	= query.field(0).asInteger();
				ts.m_TRANSITION_ID[ts.m_size]     	= query.field(1).asInteger();
				ts.m_STAFF_GROUP_ID[ts.m_size]    	= query.field(2).asInteger();
				ts.m_BEGIN_DEFER[ts.m_size]		  	= query.field(3).asInteger();
				ts.m_TIME_OFFSET[ts.m_size]		  	= query.field(4).asInteger();
				ts.m_COUNT[ts.m_size]			  	= query.field(5).asInteger();
				ts.m_INTERVAL[ts.m_size]		  	= query.field(6).asInteger();
				ts.m_TYPE_TIME[ts.m_size] 		  	= query.field(7).asInteger();
				ts.m_TYPE_BEGIN[ts.m_size]		    = query.field(8).asInteger();
				ts.m_TYPE_END[ts.m_size]			= query.field(9).asInteger();
				ts.m_TYPE_ERR[ts.m_size]			= query.field(10).asInteger();
				ts.m_size++;

			}


	}catch(...) {
		return;

	}
	return;

} 


int transition::AutoPlaceSendMessage(long transID,const string& inState,string transName)
{

	int i = 0;
	char Trans_STATE[20];
	int i_count = 0;
	long staff_id[100];
	string call_nbr[100];
	long ser_id[100];
	char tempChar[100];
	char sql[1024];
	string sql2;
	message ts;
	loadForMessage(ts,transID);
	DEFINE_PETRI_QUERY (qry1);
	DEFINE_PETRI_QUERY (qry);

	/*
	DEFINE_QUERY (qry);
	sprintf(sql, "select EXE_STATE from P_TRANSITION_ATTR where TRANSITION_ID = '%ld'",transID);
	qry.setSQL (sql);
	qry.open ();
	while (qry.next())
	{
  		strcpy(Trans_STATE, qry.field(EXE_STATE).asString()); 	
	
	}		
	qry.close();
	*/
	for (i=0;i<ts.m_size;i++)	
	{
		if(getExecAttr() == 0)
		{
			if((inState.compare("R") == 0)&& ts.m_TYPE_BEGIN[i] == 1)
			{

				Date now;
				sprintf(tempChar,"%s对应的变迁ID为%d,当前状态为正在运行，运行时间为%s",transName,transID,now.toString());

				sprintf(sql, "select STAFF_ID,ACC_NBR,SERV_ID from P_GROUP_STAFF where STAFF_GROUP_ID = '%d'",ts.m_STAFF_GROUP_ID[i]);
				qry.setSQL (sql);
				qry.open ();
				while (qry.next())
				{
			  		staff_id[i_count] = qry.field(0).asLong(); 
					call_nbr[i_count] = qry.field(1).asString();
					ser_id[i_count] = qry.field(2).asLong();
					  i_count++;
				
				}		
				qry.close();
				for (int i =0;i<i_count;i++)
				{
					sprintf(sql, "insert into P_ALARM_MSG(MSG_ALARM_ID,STAFF_ID,MSG_TYPE,MSG_NOTE,DEAL_FLAG,DEAL_DATE,CREATE_DATE,REMARK)  \
									values(SEQ_P_ALARM_MSG_ID.NEXTVAL,'%d','R','%s','N',SYSDATE,SYSDATE,'')",staff_id[i],tempChar);
					qry.close();
					qry.setSQL(sql);
					qry.execute();	
					qry.commit();
				}
			}
			else if((inState.compare("E") == 0)&&ts.m_TYPE_END[i] == 1)
			{

				Date now;
				sprintf(tempChar,"%s对应的变迁ID为%d,当前状态为已经结束，结束时间为%s",transName,transID,now.toString());

				sprintf(sql, "select STAFF_ID,ACC_NBR,SERV_ID from P_GROUP_STAFF where STAFF_GROUP_ID = '%d'",ts.m_STAFF_GROUP_ID[i]);
				qry.setSQL (sql);
				qry.open ();
				while (qry.next())
				{
			  		staff_id[i_count] = qry.field(0).asLong(); 
					call_nbr[i_count] = qry.field(1).asString();
					ser_id[i_count] = qry.field(2).asLong();

					  i_count++;
				
				}		
				qry.close();
				for (int i =0;i<i_count;i++)
				{
					sprintf(sql, "insert into P_ALARM_MSG(MSG_ALARM_ID,STAFF_ID,MSG_TYPE,MSG_NOTE,DEAL_FLAG,DEAL_DATE,CREATE_DATE,REMARK)  \
									values(SEQ_P_ALARM_MSG_ID.NEXTVAL,'%d','O','%s','N',SYSDATE,SYSDATE,'')",staff_id[i],tempChar);
					qry.close();
					qry.setSQL(sql);
					qry.execute();	
					qry.commit();
				}
			}
			else if((inState.compare("X") == 0)&&ts.m_TYPE_ERR[i] == 1)
			{

				Date now;
				sprintf(tempChar,"%s对应的变迁ID为%d,当前状态为已经出错，出错时间为%s",transName,transID,now.toString());

				sprintf(sql, "select STAFF_ID,ACC_NBR,SERV_ID from P_GROUP_STAFF where STAFF_GROUP_ID = '%d'",ts.m_STAFF_GROUP_ID[i]);
				qry.setSQL (sql);
				qry.open ();
				while (qry.next())
				{
			  	staff_id[i_count] = qry.field(0).asLong(); 
					call_nbr[i_count] = qry.field(1).asString();
					ser_id[i_count] = qry.field(2).asLong();

					  i_count++;
				
				}		
				qry.close();
				for (int i =0;i<i_count;i++)
				{
					sprintf(sql, "insert into P_ALARM_MSG(MSG_ALARM_ID,STAFF_ID,MSG_TYPE,MSG_NOTE,DEAL_FLAG,DEAL_DATE,CREATE_DATE,REMARK)  \
									values(SEQ_P_ALARM_MSG_ID.NEXTVAL,'%d','E','%s','N',SYSDATE,SYSDATE,'')",staff_id[i],tempChar);
					qry.close();
					qry.setSQL(sql);
					qry.execute();	
					qry.commit();
				}
			}	

			qry.close();
			for (int i =0;i<i_count;i++)
			{
							sql2= "insert into A_SMS_SEND \
							(sms_send_id,serv_id,acc_nbr,balance,amount ,exp_date ,msg_type_id,created_date ,staff_id ,state,state_date ,send_success_count ,send_fail_count,send_success_date,send_fail_date ,msg_content,content,reserved ,billing_mode_id,network_id ,source_type,acct_credit,generate_flag)	\
						    values(SMS_SEND_ID_SEQ.NEXTVAL,:SER_ID,:CALL_NBR,'','','','53',SYSDATE,:STRAFF_ID,'0','','','','','',:TEMPCHAR,'','','','','','','')";
						  //qry.close();
							qry.setSQL(sql2.c_str());
							qry.setParameter("STRAFF_ID",staff_id[i]);
							qry.setParameter("TEMPCHAR",tempChar);
							qry.setParameter("CALL_NBR",call_nbr[i].c_str());
							qry.setParameter("SER_ID",ser_id[i]);
							qry.execute();	
							qry.commit();
							
							qry.close();
			}
			
		}
	}

	return;
}

bool transition::switchState(const string& inState,const string& inMsg, const string& inAttr)
{
	string sql;
	string tmpMsg;
	DEFINE_PETRI_QUERY(query);	

	if (s_execState == inState)
		return true;
	else s_execState = inState;

	try {
		sql.clear();
		sql= "UPDATE P_TRANSITION_ATTR SET EXE_STATE=:ST WHERE TRANSITION_ID=:TS";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("ST",inState.c_str());
		query.setParameter("TS",l_transID);
		query.execute();

		sql.clear();
		sql = "INSERT INTO P_ACTION(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE,DRAW_ID,ACTION_TYPE,ELEMENT_NAME,ERR_MSG) VALUES(SEQ_PETRI_ACTION_ID.NEXTVAL,:TS,'T','EXEC_STATE',:ST,:AT,SYSDATE,:DRAWID,'A',:ELMNAME,:ERR_MSG)";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("ST",inState.c_str());
		query.setParameter("AT",inAttr.c_str());
		query.setParameter("TS",l_transID);
		query.setParameter("DRAWID",l_drawID);
		query.setParameter("ELMNAME",s_transName);
		if((inState.compare("X") == 0)||(inState.compare("K") == 0))
		{
			query.setParameter("ERR_MSG",inMsg.c_str());
		}
		else
		{
			query.setParameter("ERR_MSG",tmpMsg.c_str());
		}
	//query.setParameter("ERR_MSG",inMsg.c_str());
		query.execute();

	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
		query.close();
		return false;
	} catch(...) {
		query.close();
		return false;
	}

	query.commit();
	query.close();
	AutoPlaceSendMessage(l_transID,inState,s_transName);

	return true;
}

void transition::getTransactionID()
{
	if (l_transactionID != 0 ) {
		return;
	}
	DEFINE_PETRI_QUERY(query);
	try {
		string sql = "SELECT TRANSACTION_ID FROM PETRI_TRANSACTION WHERE DRAW_ID IN \
			(SELECT DRAW_ID FROM P_DRAW WHERE DRAW_LEVEL=1 START WITH DRAW_ID IN \
			(SELECT DRAW_ID FROM P_TRANSITION_ATTR WHERE TRANSITION_ID=:TS ) \
			CONNECT BY DRAW_ID = PRIOR PAN_DRAW_ID ) AND STATE='B'";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("TS",getTransID());
		query.open();
		if (query.next()) {
			l_transactionID = query.field(0).asInteger();
		} 
		query.close();
	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
	}
	catch(...) {

	};

};


void transition::nextLogID()
{
	DEFINE_PETRI_QUERY(query);
	l_logID = -1;
	try {
		string sql = "SELECT SEQ_P_TRANS_EXEC_LOG_ID.NEXTVAL FROM DUAL";
		query.close();
		query.setSQL(sql.c_str());
		query.open();
		if (query.next()) {
			l_logID = query.field(0).asInteger();
		};



	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
	} catch (...) {

	};

};

long transition::getLastLogID()
{
	DEFINE_PETRI_QUERY(query);
	l_logID = -1;
	try {
		string sql = "SELECT MAX(LOG_ID) FROM P_TRANSITION_EXEC_LOG \
									WHERE TRANSITION_ID=:ID ";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("ID",l_transID);
		query.open();
		if (query.next()) {
			l_logID = query.field(0).asInteger();
		};


	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
	} catch (...) {

	};

	return l_logID;

};

long transition::getCurrentLogID()
{
	DEFINE_PETRI_QUERY(query);
	l_logID = -1;
	try {
		string sql = "SELECT MAX(LOG_ID) FROM P_TRANSITION_EXEC_LOG \
									WHERE TRANSITION_ID=:ID AND END_TIME IS NULL ";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("ID",l_transID);
		query.open();
		if (query.next()) {
			l_logID = query.field(0).asInteger();
		};


	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
	} catch (...) {

	};

	return l_logID;

};

long transition::getLogID()
{
	return l_logID;
};

void transition::createLog()
{

	nextLogID();


	DEFINE_PETRI_QUERY(query);
	try {
		string sql = 
			"INSERT INTO P_TRANSITION_EXEC_LOG (LOG_ID,TRANSITION_ID,TRANSACTION_ID, \
			 EXEC_TYPE,STAFF_ID,START_TIME) VALUES(:LOGID,:TRANSITIONID,:TRANSACTIONID, \
				DECODE(:EXECTYPE,0,'A',1,'M','A'),:STAFFID,SYSDATE)";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("LOGID",l_logID);
		query.setParameter("TRANSITIONID",l_transID);
		query.setParameter("TRANSACTIONID",l_transactionID);
		query.setParameter("EXECTYPE",i_execType);
		query.setParameter("STAFFID",l_staffID);
		query.execute();


	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
		query.close();
		return;
	} catch (...) {
		query.close();
		return;
	}
	
	query.commit();

};

void 
transition::updateTransExecLog(const string& sColumnName,const string& sColumnValue)
{
#ifdef HB_28
	if (sColumnName == "REMARK" || sColumnName == "EXEC_RESULT" ) { //执行结果展示
		char sMsg[1024];
		memset(sMsg,0,sizeof(sMsg));
		if (sColumnName == "REMARK")
			sprintf(sMsg,"[TransName]%s [StaffID]%ld [ExecInfo]%s ",s_transName,l_staffID,sColumnValue.c_str());
		else 
			sprintf(sMsg,"[TransName]%s [StaffID]%ld [ExecResult]%s ",s_transName,l_staffID,sColumnValue.c_str());
	  Log::m_iAlarmLevel = 3;
	  Log::m_iAlarmDbLevel = -1;
	
	  //ALARMLOG28(0,MBC_CLASS_Business,0,"%s",in_msg);
	  LOGG(SYSTEMLOG,sMsg);
	}
#endif
	DEFINE_PETRI_QUERY(query);
	try {

		string sql ;
		sql.clear();
		query.close();

		if (sColumnName == "START_TIME" || sColumnName == "END_TIME") {
			sql = "UPDATE P_TRANSITION_EXEC_LOG SET "+sColumnName+ " =SYSDATE  WHERE LOG_ID= :ID";
			query.setSQL(sql.c_str());
			query.setParameter("ID",l_logID);
			query.execute();
		}
		else {
			sql = "UPDATE P_TRANSITION_EXEC_LOG SET "+sColumnName+ " = :CN WHERE LOG_ID= :ID";
			query.setSQL(sql.c_str());
			query.setParameter("ID",l_logID);
			query.setParameter("CN",sColumnValue.c_str());
			query.execute();
		};



	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
		query.close();
		return;
	} catch (...) {
		query.close();
		return;

	};
	
	query.commit();
	query.close();


};

void 
transition::updateTransExecLog(const string& sColumnName,const int iColumnValue)
{
	DEFINE_PETRI_QUERY(query);
	try {
		string sql = "UPDATE P_TRANSITION_EXEC_LOG SET "+sColumnName+ " = :CN WHERE LOG_ID= :ID";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("ID",l_logID);
		query.setParameter("CN",iColumnValue);
		query.execute();


	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
		return;
	}catch (...) {
		return;
	};
	
	query.commit();
	query.close();

};

void 
transition::updateTransExecLog(const string& sColumnName,const long lColumnValue)
{
	DEFINE_PETRI_QUERY(query);
	try {
		string sql = "UPDATE P_TRANSITION_EXEC_LOG SET "+sColumnName+ " = :CN WHERE LOG_ID= :ID";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("ID",l_logID);
		query.setParameter("CN",lColumnValue);
		query.execute();

	}catch (TOCIException & toe){
		LOG(toe.getErrMsg());
		LOG(toe.getErrSrc());
		return;
	}catch (...) {
		return;
	};
	
	query.commit();
	query.close();
	
};

string transition::getExecResult()
{
	return s_execResult;
};



void transition::getInStateList(vector<string>& outList)
{
	//copy(v_inStateList.begin(),v_inStateList.end(),outList);
	unsigned int i =0;
	for (;i<v_inStateList.size();i++) {
		outList.push_back(v_inStateList[i]);
	};
	return;
};

bool transition::doChangeState(const string& state)
{
	if (state != "X" || state != "A" || state !="E" || state !="R" ) {
		return false;
	} else {
		switchState(state,"");
		return true;
	}

};



int transition::getSubnetType()
{
	return i_subnetType;
};

long transition::getSubDrawID()
{
	return l_subDrawID;
};

long transition::getDrawID()
{
	return l_drawID;
};

int transition::getExecType()
{
	return i_execType;
};

long transition::getExecObjID()
{
	return l_execObjID;
};


};
