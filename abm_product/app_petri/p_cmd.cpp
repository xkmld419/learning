#include "p_cmd.h"

/*
PETRI::cmd::cmd()
{

}
*/
void PETRI::cmd::exeCmd()
{


	pid_t pid= fork();
	pid_t childPid;
	int iStatus;
	switch(pid) {
		case 0:
			sleep(1);
			childPid = fork();
			if (childPid == 0 ) {
				P_DB.getDB(true);//重连数据库
				doCmd();
				exit(0);
			} else if (childPid == -1) {
				if ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2)) { //需要回复
					P_RECVBUFF.addMsg("Create Exec Process Fail ");
					P_RECVBUFF.setResult(1);
					P_RECVBUFF.getBuiltMsg();
					P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
					P_ACKMSG->mtype = P_CMDMSG->mtype;
					if ( kill(P_ACKMSG->mtype,0) != 0 )
						return;
					P_MQ.ackMQ.send();
				}
				exit(-1);
			} else {
				exit(0);
			}
			break;
		case -1:
			if ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2)) { //需要回复
				P_RECVBUFF.addMsg("Create Son Process Fail");
				P_RECVBUFF.setResult(1);
				P_RECVBUFF.getBuiltMsg();
				P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if ( kill(P_ACKMSG->mtype,0) != 0 )
					return;
				P_MQ.ackMQ.send();
			}
			break;
		default:
			wait(&iStatus);
	}

	return;


};

/*

void PETRI::cmd_fire::exeTrans(cmd_msg* cmsg)
{
	transition ts(cmsg->transID);

	bool bret;
	bret = ts.load();
	if (! bret ) goto FIRE_END;

	if ( cmsg->needAck || !ts.getExecType()) {
		if (!checkExec)//判断变迁是否在执行
			bret = ts.fire();//变迁点火
		else
			bret = false;
	} else {
		bret = false;
	}
FIRE_END:
	if (cmsg->needAck) {
		sendmq->cpyMsg(recvmq->getMsg());
		sendmq->msgbuf.ret = bret?0:1;// 0 成功 1 失败
		sendmq->send();//返回消息
	};
	if ( !bret ) {
		return;
	};

	ts.execute();//变迁执行
	vector<long> vt ;
	ts.getNextTrans(vt);//遍历下游变迁
	delete ts;
	for (int idx = 0 ;idx<vt.size();idx++ ) {
		cmd_msg mycmd;
		mycmd.transID=vt[idx];
		mycmd.needAck=false;
		exeTrans(&mycmd);
	};

	return;
};
*/

/*
void PETRI::cmd_fire::exeTrans(cmd_msg* cmsg)
{
	transition ts(cmsg->transID);
	ts.l_staffID = cmsg->staff_id;

	bool bret;
	bret = ts.load();
	if (! bret ) goto FIRE_END;
	places inP;
	inP.setTransID(ts.transID);
	inP.setIn();
	inP.load();
	while (inP.lock() == 1) {
		inP.erase();
		inP.load();
	}

	if ( cmsg->needAck || !ts.getExecType()) {
		if (!execState && bret = inP.checkFire()) {
			bret = inP.tokenMove();
		};

	} else {
		bret = false;
	}
FIRE_END:
	if (cmsg->needAck) {
		sendmq->cpyMsg(recvmq->getMsg());
		sendmq->msgbuf.ret = bret?0:1;// 0 成功 1 失败
		sendmq->send();//返回消息
	};
	if ( !bret ) {
		return;
	};

	ts.execute();//变迁执行
	vector<long> vt ;
	ts.getNextTrans(vt);//遍历下游变迁
	delete ts;
	for (int idx = 0 ;idx<vt.size();idx++ ) {
		cmd_msg mycmd;
		mycmd.transID=vt[idx];
		mycmd.needAck=false;
		exeTrans(&mycmd);
	};

	return;
};
*/

void PETRI::cmd_fire::doCmd()
{
	long transID = (long)P_RECVBUFF.getTransitionID();
//	printf("%s,%d,transID=%ld\n",__FILE__,__LINE__,transID);
	transition ts(transID);
	ts.l_staffID = P_RECVBUFF.getStaffID();
	ts.l_transactionID  = P_RECVBUFF.getTransactionID();

	bool bret;
	bret = ts.load();
	if (! bret ) {
//		ts.updateTransExecLog("EXEC_PROC_DESC","Init Fail");
//		ts.updateTransExecLog("END_TIME");
		if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
				P_RECVBUFF.addMsg("Init Fail");
				P_RECVBUFF.setResult(1);
				P_RECVBUFF.getBuiltMsg();
				P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if (kill (P_ACKMSG->mtype,0) != 0) 
					return;
				P_MQ.ackMQ.send();
		}
		return;
	}

//	if  (P_RECVBUFF.getNeedAck() == 0 && ts.getExecAttr() != 0 )  return; //Manual Transition


	places inP;
	places outP;
	places parentOutP;

	inP.setTransID(transID);
	inP.setIn();
	inP.load();
	vector<string> stateList;
	ts.getInStateList(stateList);

	if (ts.checkExec()) {
//		ts.updateTransExecLog("EXEC_PROC_DESC","Start Fail,Already Running");
//		ts.updateTransExecLog("END_TIME");
		if	((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
				P_RECVBUFF.addMsg("Start Fail,Already Running");
				P_RECVBUFF.setResult(1);
				P_RECVBUFF.getBuiltMsg();
				P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if (kill (P_ACKMSG->mtype,0) != 0) 
					return;
				P_MQ.ackMQ.send();
		}
		return;
	};

	if (ts.checkEnd()) {
//		ts.updateTransExecLog("EXEC_PROC_DESC","Start Fail,Already Ending");
//		ts.updateTransExecLog("END_TIME");
		if	((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
				P_RECVBUFF.addMsg("Start Fail,Already Ending");
				P_RECVBUFF.setResult(1);
				P_RECVBUFF.getBuiltMsg();
				P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if (kill (P_ACKMSG->mtype,0) != 0) 
					return;
				P_MQ.ackMQ.send();
		}
		return;
		};



	if ( !inP.checkFire(stateList)) {
//		ts.updateTransExecLog("EXEC_PROC_DESC","Not Ready to Fire");
//		ts.updateTransExecLog("END_TIME");
		if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
				P_RECVBUFF.addMsg("Not Ready to Fire");
				P_RECVBUFF.setResult(1);
				P_RECVBUFF.getBuiltMsg();
				P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if (kill (P_ACKMSG->mtype,0) != 0) 
					return;
				P_MQ.ackMQ.send();
		}
		return;
	} ;

	if (! ts.checkTime()) {
//		ts.updateTransExecLog("EXEC_PROC_DESC","Not The Time");
//		ts.updateTransExecLog("END_TIME");
		if  (P_RECVBUFF.getNeedAck() == 1 ) {
				P_RECVBUFF.addMsg("定时时间未到达");
				P_RECVBUFF.setResult(1);
				P_RECVBUFF.getBuiltMsg();
				P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if (kill (P_ACKMSG->mtype,0) != 0) 
					return;
				P_MQ.ackMQ.send();
		}
		return;
	}


//	ts.updateTransExecLog("EXEC_PROC_DESC","Transition Fired");
//	ts.updateTransExecLog("END_TIME");
	if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
			P_RECVBUFF.addMsg("Transition Fired");
			P_RECVBUFF.setResult(0);
			P_RECVBUFF.getBuiltMsg();
			P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
			P_ACKMSG->mtype = P_CMDMSG->mtype;
			if (kill (P_ACKMSG->mtype,0) == 0) 
				P_MQ.ackMQ.send();
	};

	vector<long> vt;//保存后续变迁
	unsigned int idx;
	vt.clear();
	
	outP.setTransID(transID);
	outP.setOut();
	//		outP.getFirstPlace(ts.getSubDrawID());
	//		outP.getNextTrans(vt);
	//		vtSize = vt.size();
	outP.load(ts.getSubnetType());
	if(!ts.getSubnetType()){
		if (!ts.execute()) {
			ts.updateTransExecLog("EXEC_PROC_DESC","Exec Fail");
			ts.updateTransExecLog("END_TIME");
			if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
					P_RECVBUFF.addMsg("Exec Fail");
					P_RECVBUFF.setResult(1);
					P_RECVBUFF.getBuiltMsg();
					P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
					P_ACKMSG->mtype = P_CMDMSG->mtype;
					if (kill (P_ACKMSG->mtype,0) != 0) 
						return;
					P_MQ.ackMQ.send();
			}
			return;
		}
		if ( !outP.checkState(ts.getExecResult() ) ) {
			ts.updateTransExecLog("EXEC_PROC_DESC","Not in Outplaces StateList");
			ts.updateTransExecLog("END_TIME");
			if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
					P_RECVBUFF.addMsg("Not in Outplaces StateList");
					P_RECVBUFF.setResult(1);
					P_RECVBUFF.getBuiltMsg();
					P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
					P_ACKMSG->mtype = P_CMDMSG->mtype;
					if (kill (P_ACKMSG->mtype,0) != 0) 
						return;
					P_MQ.ackMQ.send();
			}
			return;
		}
	}else{
//		Date now;
//		char execdate[15];
//		now.getTimeString(execdate);
//		execdate[14]=0;
//		string sexecMsg(execdate);
//		sexecMsg = "start at " + sexecMsg;
		ts.switchState("R","");
		ts.createLog();
		ts.updateTransExecLog("START_TIME");
	}

	//transaction begin

	do {
		P_DB.getDB()->rollback();
		usleep(1000);
/*
		inP.erase();
		inP.setTransID(transID);
		inP.setIn();
		inP.load();
		outP.erase();
		outP.setTransID(transID);
		outP.setOut();
		outP.load(ts.getSubnetType());
*/
	}
	while(inP.lock() == 1 || outP.lock() == 1);
	//reload for Toknum charged
	inP.erase();
	inP.setTransID(transID);
	inP.setIn();
	inP.load();
	outP.erase();
	outP.setTransID(transID);
	outP.setOut();
	outP.load(ts.getSubnetType());

	if (!inP.tokenMove()) {
		P_DB.getDB()->rollback();
		ts.updateTransExecLog("EXEC_PROC_DESC","Inpalce tokenMove Fail");
		ts.updateTransExecLog("END_TIME");
		if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
				P_RECVBUFF.addMsg("Inpalce tokenMove Fail");
				P_RECVBUFF.setResult(1);
				P_RECVBUFF.getBuiltMsg();
				P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if (kill (P_ACKMSG->mtype,0) != 0) 
					return;
				P_MQ.ackMQ.send();
		}
		return;
	}
	bool tokenMoveResule ;
	if(ts.getSubnetType()){
		tokenMoveResule = outP.tokenMove("0");
	}else{
		tokenMoveResule = outP.tokenMove(ts.getExecResult());
	}
	if (!tokenMoveResule) {
		P_DB.getDB()->rollback();
		ts.updateTransExecLog("EXEC_PROC_DESC","Outpalce tokenMove Fail");
		ts.updateTransExecLog("END_TIME");
		if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
				P_RECVBUFF.addMsg("Outpalce tokenMove Fail");
				P_RECVBUFF.setResult(1);
				P_RECVBUFF.getBuiltMsg();
				P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if (kill (P_ACKMSG->mtype,0) != 0) 
					return;
				P_MQ.ackMQ.send();
		}
		return;
	}
	
	P_DB.getDB()->commit();  //transaction end

	PETRI::updateTransStateExecd(transID);

	//if subnet end place,token move to parent place
	if ( outP.CheckSubnetEndPlcs() ) {
		do{
			string endPlcsState = outP.getEndPlcsState();
			long parentTransID = outP.getParentTransID();
		
			if(-1 == parentTransID){
				ts.updateTransExecLog("EXEC_PROC_DESC","getParentOutPlcs Fail");
				ts.updateTransExecLog("END_TIME");
				if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
					P_RECVBUFF.addMsg("getParentOutPlcs Fail");
					P_RECVBUFF.setResult(1);
					P_RECVBUFF.getBuiltMsg();
					P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
					P_ACKMSG->mtype = P_CMDMSG->mtype;
					if (kill (P_ACKMSG->mtype,0) != 0) 
						return;
					P_MQ.ackMQ.send();
				}
				return;
			}		
			parentOutP.erase();
			parentOutP.setTransID(parentTransID);
			parentOutP.setOut();
			parentOutP.load();
		
			//transaction begin
			do {
				P_DB.getDB()->rollback();
				usleep(1000);
			}
			while(outP.lock() == 1 || parentOutP.lock() == 1);
			//reload for Toknum charged
			outP.erase();
			outP.setTransID(transID);
			outP.setOut();
			outP.load(); //transition can not be a subnet transition
			outP.setIn();
			parentOutP.erase();
			parentOutP.setTransID(parentTransID);
			parentOutP.setOut();
			parentOutP.load();			

			if (!outP.tokenMove()) {
				P_DB.getDB()->rollback();
				ts.updateTransExecLog("EXEC_PROC_DESC","Subnet End tokenMove Fail");
				ts.updateTransExecLog("END_TIME");
				if	((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
						P_RECVBUFF.addMsg("Subnet End tokenMove Fail");
						P_RECVBUFF.setResult(1);
						P_RECVBUFF.getBuiltMsg();
						P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
						P_ACKMSG->mtype = P_CMDMSG->mtype;
						if (kill (P_ACKMSG->mtype,0) != 0) 
							return;
						P_MQ.ackMQ.send();
				}
				return;
			}

			if (!parentOutP.tokenMove(endPlcsState)) {
				P_DB.getDB()->rollback();
				ts.updateTransExecLog("EXEC_PROC_DESC","Parentnet Begin tokenMove Fail");
				ts.updateTransExecLog("END_TIME");
				if	((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
						P_RECVBUFF.addMsg("Parentnet Begin tokenMove Fail");
						P_RECVBUFF.setResult(1);
						P_RECVBUFF.getBuiltMsg();
						P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
						P_ACKMSG->mtype = P_CMDMSG->mtype;
						if (kill (P_ACKMSG->mtype,0) != 0) 
							return;
						P_MQ.ackMQ.send();
				}
				return;
			}
			P_DB.getDB()->commit();  //transaction end
		
			Date now;
			char execdate[15];
			now.getTimeString(execdate);
			execdate[14]=0;
			string sexecMsg(execdate);
			sexecMsg = "End at " + sexecMsg;

			transition parentTS(parentTransID);
			bret = parentTS.load();
			if (! bret ) {
				ts.updateTransExecLog("EXEC_PROC_DESC","ParentTrans LOAD Fail");
				ts.updateTransExecLog("END_TIME");
				if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
					P_RECVBUFF.addMsg("ParentTrans LOAD Fail");
					P_RECVBUFF.setResult(1);
					P_RECVBUFF.getBuiltMsg();
					P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
					P_ACKMSG->mtype = P_CMDMSG->mtype;
					if (kill (P_ACKMSG->mtype,0) != 0) 
						return;
					P_MQ.ackMQ.send();
				}
				return;
			}
			parentTS.switchState("E","");
			parentTS.updateTransExecLog("END_TIME");
			
			transID = parentTransID;
			outP.erase();
			outP.setTransID(transID);
			outP.setOut();
			outP.load();
		}while(outP.CheckSubnetEndPlcs());
	}
	
	outP.getNextTrans(vt);
	//Switch The exec_state of transitions after tokens move successfully
	for (idx = 0 ;idx < vt.size();idx++ ) {
		PETRI::updateTransState(vt[idx]);
	}
	if	((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
		P_RECVBUFF.setResult(0);
		P_RECVBUFF.getBuiltMsg();
		P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
		P_ACKMSG->mtype = P_CMDMSG->mtype;
		if (kill (P_ACKMSG->mtype,0) != 0) 
			return;
		P_MQ.ackMQ.send();
	}
};



void PETRI::cmd_stop::doCmd()
{
	long transID = (long)P_RECVBUFF.getTransitionID();
	transition ts(transID);
	ts.l_staffID = P_RECVBUFF.getStaffID();
	ts.l_transactionID  = P_RECVBUFF.getTransactionID();

	bool bret;
	bret = ts.load();
	if (! bret ) {
		if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
				P_RECVBUFF.addMsg("transition load fail");
				P_RECVBUFF.setResult(1);
				P_RECVBUFF.getBuiltMsg();
				P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if (kill (P_ACKMSG->mtype,0) != 0) 
					return;
				P_MQ.ackMQ.send();
		}
		return;
	};

	if (ts.checkExec()) {
		ts.getCurrentLogID();
		ts.kill();
	}
	
	if	((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
		P_RECVBUFF.setResult(0);
		P_RECVBUFF.getBuiltMsg();
		P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
		P_ACKMSG->mtype = P_CMDMSG->mtype;
		if (kill (P_ACKMSG->mtype,0) != 0) 
			return;
		P_MQ.ackMQ.send();
	}
};


void PETRI::cmd_query::doCmd()
{
	long transID = (long)P_RECVBUFF.getTransitionID();
	transition ts(transID);
	ts.l_staffID = P_RECVBUFF.getStaffID();
	ts.l_transactionID  = P_RECVBUFF.getTransactionID();
	bool bret;
	if (!ts.load()) 
	{
		sprintf(P_ACKMSG->mbody,"<iResult=1><lTransitionID=%ld><sMsg=transition load fail>",transID);
		P_ACKMSG->mtype = P_CMDMSG->mtype;
		if (kill (P_ACKMSG->mtype,0) != 0) 
			return;
		P_MQ.ackMQ.send();
	};
	string s_querytsMSG = ts.query(); 
	
	sprintf(P_ACKMSG->mbody,"<iResult=0><lTransitionID=%ld>%s",transID,s_querytsMSG.c_str());
	P_ACKMSG->mtype = P_CMDMSG->mtype;
	if (kill (P_ACKMSG->mtype,0) != 0) 
		return;
	P_MQ.ackMQ.send();
}


/*
//the Query CMD should compose the message manually,not by parserbuff
//<iResult=0>$<lTransitionID=213>...$<lTransitionID=214>...
void PETRI::cmd_query::doCmd()
{
	long drawID = (long) P_RECVBUFF.getDrawID();
	int iResult = 0;
	vector<long> transList;
	transList.clear();
	DEFINE_PETRI_QUERY(query);
	try {
		string sql = "SELECT TRANSITION_ID FROM P_TRANSITION_ATTR WHERE DRAW_ID= :DRAWID AND EXE_STATE = 'R'";
		query.close();
		query.setSQL(sql.c_str());
		query.setParameter("DRAWID",drawID);
		query.open();
		while(query.next()) {
			transList.push_back(query.field(0).asLong());
		}
		query.close();


	} catch (TOCIException& toe) {
		LOG(toe.getErrMsg());
		query.close();
		iResult = 1;
	} catch (...) {
		query.close();
		iResult = 1;
	};

	if (iResult != 0) {
		if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
				P_RECVBUFF.addMsg("Get Transition of The Draw Error!\n");
				strcpy(P_ACKMSG->mbody,"<iResult=1><sMsg=Get Transition of The Draw Error!>");
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if (kill (P_ACKMSG->mtype,0) != 0) 
					return;

				P_MQ.ackMQ.send();
		}
		return;
	}

	unsigned int idx,iSize;
	iSize = transList.size();
	if (iSize  == 0) {
		if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
				P_RECVBUFF.addMsg("Get Transition of The Draw Error!\n");
				strcpy(P_ACKMSG->mbody,"<iResult=1><sMsg=No Executing Transition Exist!>");
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if (kill (P_ACKMSG->mtype,0) != 0) 
					return;

				P_MQ.ackMQ.send();
		}
	}


	char sQueryMsg[MAX_CMAMSG_LEN+1];
	memset(sQueryMsg,0,MAX_CMAMSG_LEN+1);


	for (idx = 0;idx <iSize;idx ++ ) {
		//get message from every transition
		//fetch message by ACKMSG's mbody
		memset(P_ACKMSG->mbody,0,MSG_LENGTH);
		sprintf(P_ACKMSG->mbody,"$<lTransitionID=%ld>\0",transList[idx]);
		long transID = (long)P_RECVBUFF.getTransitionID();
		transition ts(transList[idx]);

		if (!ts.load()) 
		{
			iResult = 1 ;
			continue;
		};

		if (!ts.query()) 
		{ 
			iResult =1;
			continue;
		};

		int iTotalLength = strlen(sQueryMsg);
		int	iCurLength = strlen(P_ACKMSG->mbody); 
		if (iTotalLength+iCurLength > MAX_CMAMSG_LEN) break;
		strncpy(sQueryMsg+iTotalLength,P_ACKMSG->mbody,iCurLength);
	};
	memset(P_ACKMSG->mbody,0,MSG_LENGTH);

	if ( iResult == 0 ) {
		sprintf(P_ACKMSG->mbody,"<iResult=0>%s",sQueryMsg);
	} else {
		sprintf(P_ACKMSG->mbody,"<iResult=1>%s",sQueryMsg);
	}

	if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
		P_ACKMSG->mtype = P_CMDMSG->mtype;
		if (kill (P_ACKMSG->mtype,0) != 0) 
			return;
		P_MQ.ackMQ.send();
	};

	return;


	long transID = (long)P_RECVBUFF.getTransitionID();
	transition ts(transID);
	ts.l_staffID = P_RECVBUFF.getStaffID();
	ts.l_transactionID  = P_RECVBUFF.getTransactionID();

	bool bret;
	bret = ts.load();
	if (! bret ) {
		if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
				P_RECVBUFF.addMsg("变迁初始化失败");
				P_RECVBUFF.setResult(1);
				P_RECVBUFF.getBuiltMsg();
				P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if (kill (P_ACKMSG->mtype,0) != 0) 
					return;

				P_MQ.ackMQ.send();
		}
		return;
	};

	ts.getLastLogID();
	string ret =ts.tExec->doQuery();
	if (ret == "0" ) {
		P_RECVBUFF.setResult(0);
	} else {
		P_RECVBUFF.setResult(1);
	}
	P_RECVBUFF.getBuiltMsg();
	P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
	P_ACKMSG->mtype = P_CMDMSG->mtype;
	if (kill (P_ACKMSG->mtype,0) != 0) 
		return;
	P_MQ.ackMQ.send();

};
*/
void PETRI::cmd_changeState::doCmd()
{
	char state[32];
	long transID = (long)P_RECVBUFF.getTransitionID();
	transition ts(transID);
	ts.l_staffID = P_RECVBUFF.getStaffID();
	ts.l_transactionID  = P_RECVBUFF.getTransactionID();

	bool bret;
	bret = ts.load();
	if (! bret ) {
		if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
				P_RECVBUFF.addMsg("transition init fail");
				P_RECVBUFF.setResult(1);
				P_RECVBUFF.getBuiltMsg();
				P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if (P_ACKMSG > 0) {
					if (kill (P_ACKMSG->mtype,0) != 0) 
						return;
				};
				P_MQ.ackMQ.send();
		}
		return;
	};
	P_RECVBUFF.getDescState(state);
	if (!state[0] ) {
		if  ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
				P_RECVBUFF.addMsg("Wrong DescState!");
				P_RECVBUFF.setResult(1);
				P_RECVBUFF.getBuiltMsg();
				P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
				P_ACKMSG->mtype = P_CMDMSG->mtype;
				if (P_ACKMSG > 0) {
					if (kill (P_ACKMSG->mtype,0) != 0) 
						return;
				};
				P_MQ.ackMQ.send();
		}
	}
	string sState = state;
	if (ts.switchState(sState,"") ) {
		P_DB.getDB()->commit();
		P_RECVBUFF.setResult(0);
	} else {
		P_DB.getDB()->rollback();
		P_RECVBUFF.addMsg("Exception When Changing State!");
		P_RECVBUFF.setResult(1);
	}

	if(strcmp(state,"E")==0){
		places inP;
		places outP;
		places parentOutP;
	
		inP.setTransID(transID);
		inP.setIn();
		inP.load();
	
		vector<long> vt;//保存后续变迁
		unsigned int idx;
		vt.clear();
		
		outP.setTransID(transID);
		outP.setOut();
		outP.load();
	
		//transaction begin
	
		do {
			P_DB.getDB()->rollback();
			usleep(1000);
		}
		while(inP.lock() == 1 || outP.lock() == 1);
		//reload for Toknum charged
		inP.erase();
		inP.setTransID(transID);
		inP.setIn();
		inP.load();
		outP.erase();
		outP.setTransID(transID);
		outP.setOut();
		outP.load();
		if (!inP.tokenMove()) {
			P_DB.getDB()->rollback();
			if	((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
					P_RECVBUFF.addMsg("Inplace tokenMove Fail");
					P_RECVBUFF.setResult(1);
					P_RECVBUFF.getBuiltMsg();
					P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
					P_ACKMSG->mtype = P_CMDMSG->mtype;
					if (kill (P_ACKMSG->mtype,0) != 0) 
						return;
					P_MQ.ackMQ.send();
			}
			return;
		}
		if (!outP.tokenMove("0")) {
			P_DB.getDB()->rollback();
			if	((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
					P_RECVBUFF.addMsg("Outplace tokenMove Fail");
					P_RECVBUFF.setResult(1);
					P_RECVBUFF.getBuiltMsg();
					P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
					P_ACKMSG->mtype = P_CMDMSG->mtype;
					if (kill (P_ACKMSG->mtype,0) != 0) 
						return;
					P_MQ.ackMQ.send();
			}
			return;
		}
		
		P_DB.getDB()->commit();  //transaction end
		
		PETRI::updateTransStateExecd(transID);
		
		//if subnet end place,token move to parent place
		if ( outP.CheckSubnetEndPlcs() ) {
			do{
				string endPlcsState = outP.getEndPlcsState();
				long parentTransID = outP.getParentTransID();
			
				if(-1 == parentTransID){
					if	((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
						P_RECVBUFF.addMsg("getParentOutPlcs Fail");
						P_RECVBUFF.setResult(1);
						P_RECVBUFF.getBuiltMsg();
						P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
						P_ACKMSG->mtype = P_CMDMSG->mtype;
						if (kill (P_ACKMSG->mtype,0) != 0) 
							return;
						P_MQ.ackMQ.send();
					}
					return;
				}
				
				parentOutP.erase();
				parentOutP.setTransID(parentTransID);
				parentOutP.setOut();
				parentOutP.load();
			
				//transaction begin
				do {
					P_DB.getDB()->rollback();
					usleep(1000);
				}
				while(outP.lock() == 1 || parentOutP.lock() == 1);
				
				//reload for Toknum charged
				outP.erase();
				outP.setTransID(transID);
				outP.setOut();
				outP.load(); //transition can not be a subnet transition
				outP.setIn();
				parentOutP.erase();
				parentOutP.setTransID(parentTransID);
				parentOutP.setOut();
				parentOutP.load();
			
				if (!outP.tokenMove()) {
					P_DB.getDB()->rollback();
					if	((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
							P_RECVBUFF.addMsg("Subnet End tokenMove Fail");
							P_RECVBUFF.setResult(1);
							P_RECVBUFF.getBuiltMsg();
							P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
							P_ACKMSG->mtype = P_CMDMSG->mtype;
							if (kill (P_ACKMSG->mtype,0) != 0) 
								return;
							P_MQ.ackMQ.send();
					}
					return;
				}
	
				if (!parentOutP.tokenMove(endPlcsState)) {
					P_DB.getDB()->rollback();
					if	((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
							P_RECVBUFF.addMsg("Parentnet Begin tokenMove Fail");
							P_RECVBUFF.setResult(1);
							P_RECVBUFF.getBuiltMsg();
							P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
							P_ACKMSG->mtype = P_CMDMSG->mtype;
							if (kill (P_ACKMSG->mtype,0) != 0) 
								return;
							P_MQ.ackMQ.send();
					}
					return;
				}
				P_DB.getDB()->commit();  //transaction end
			
				Date now;
				char execdate[15];
				now.getTimeString(execdate);
				execdate[14]=0;
				string sexecMsg(execdate);
				sexecMsg = "End at " + sexecMsg;
	
				transition parentTS(parentTransID);
				bret = parentTS.load();
				if (! bret ) {
					if	((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
						P_RECVBUFF.addMsg("ParentTrans LOAD Fail");
						P_RECVBUFF.setResult(1);
						P_RECVBUFF.getBuiltMsg();
						P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
						P_ACKMSG->mtype = P_CMDMSG->mtype;
						if (kill (P_ACKMSG->mtype,0) != 0) 
							return;
						P_MQ.ackMQ.send();
					}
					return;
				}
				parentTS.switchState("E","");

				transID = parentTransID;
				outP.erase();
				outP.setTransID(transID);
				outP.setOut();
				outP.load();
			}while(outP.CheckSubnetEndPlcs());
		}
		
		outP.getNextTrans(vt);
		//Switch The exec_state of transitions after tokens move successfully
		for (idx = 0 ;idx < vt.size();idx++ ) {
			PETRI::updateTransState(vt[idx]);
		}
	}

	if	((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2) ) {
		P_RECVBUFF.setResult(0);
		P_RECVBUFF.getBuiltMsg();
		P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
		P_ACKMSG->mtype = P_CMDMSG->mtype;
		if (kill (P_ACKMSG->mtype,0) != 0) 
			return;
		P_MQ.ackMQ.send();
	}
};


// ---------schedule------------
PETRI::commander::commander()
{
	initCmd();
	
};

PETRI::commander::~commander()
{
	delete []cmdBuf;
	
};

void PETRI::commander::applyCMD()
{

	while(1) {

		usleep(100000);

		if ( P_MQ.cmdMQ.receive() ) {
			P_RECVBUFF.reset();
			P_SENDBUFF.reset();

			LOG("RECVMSG");
			LOG(P_CMDMSG->mbody);
			P_RECVBUFF.parseBuff(P_CMDMSG->mbody);
			cmd* command = getCmd(P_RECVBUFF.getOperation());
			if ( command ) {

				command->exeCmd();
//				command->doCmd();  //test
			}
			else if ((P_RECVBUFF.getNeedAck() == 1)||(P_RECVBUFF.getNeedAck() == 2)) { //需要回复

				P_RECVBUFF.addMsg("unknown command");
				P_RECVBUFF.setResult(1);
				P_RECVBUFF.getBuiltMsg();
				P_RECVBUFF.getBuiltStr(P_ACKMSG->mbody);
				P_CMDMSG->mtype = P_ACKMSG->mtype;
				P_MQ.ackMQ.send();

			};
				
		};

	};
	
	return;
};

void PETRI::commander::initCmd()
{

	cmdBuf[0] = new cmd_fire;
	cmdBuf[1] = new cmd_stop;
	cmdBuf[2] = new cmd_changeState;
	cmdBuf[3] = new cmd_query;
	return;
};

namespace PETRI{
cmd* commander::getCmd(int req_id)
{
	switch(req_id) {
		case 1:
			return cmdBuf[0];
			break;
		case 2:
			return cmdBuf[1];
			break;
		case 3:
			return cmdBuf[2];
			break;
		case 4:
			return cmdBuf[3];
			break;
		default:
			return NULL;
			break;
	};
};
};


int main () 
{

	PETRI::commander cmder;
	cmder.applyCMD();


	return 0;
}



