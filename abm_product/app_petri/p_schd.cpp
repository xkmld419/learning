#include "p_schd.h"

long	PETRI::schd::getLatestID()
{

	long lID = -1;
	DEFINE_PETRI_QUERY(query);
	try {
		string sql = "SELECT NVL(MAX(ACTION_ID),-1) ID FROM P_ACTION \
									WHERE ACTION_TYPE = 'M' ";
/*		if (l_drawID!= -1) {
			sql += "AND DRAW_ID IN ( \
								SELECT DRAW_ID FROM P_DRAW \
								START WITH DRAW_ID =:DRAWID \
								CONNECT BY PRIOR DRAW_ID = PAN_DRAW_ID )";
		}
*/
		query.close();
		query.setSQL(sql.c_str());
/*		if (l_drawID != -1) 
		{
			query.setParameter("DRAWID",l_drawID);
		}
*/
		query.open();
		if (query.next()) 
		{
			lID = query.field(0).asLong();
		}

	} catch (TOCIException &toe) {
		LOG(toe.getErrMsg());
		query.close();
	} catch (...) {

	}
	query.close();

	return lID;

};

void
PETRI::schd::collect()
{

	transList.clear();
	DEFINE_PETRI_QUERY(query);
	try {
		string sql = "\
			SELECT TS.TRANSITION_ID	\
			FROM P_TRANSITION_ATTR TS	\
			WHERE  NVL(TS.TRANSITION_EXEC_ATTR,'A')='A' \
			AND ( TS.EXE_STATE='A' OR TS.EXE_STATE='X' ) ";	

/*		if (l_drawID!= -1) {
			sql += "AND DRAW_ID IN ( \
								SELECT DRAW_ID FROM P_DRAW \
								START WITH DRAW_ID =:DRAWID \
								CONNECT BY PRIOR DRAW_ID = PAN_DRAW_ID )";
		}
*/
		if (l_wsnID!= -1) {
			sql += "AND TS.TRANSITION_ID IN ( \
								SELECT TRANSITION_ID FROM P_TRANSITION_HOST_INFO \
								WHERE WSNADDR_ID = :WID )";
		}	
		sql += " \
			AND NOT EXISTS (	\
						SELECT 1 FROM P_TRANSITION_PLACE_RELA A,P_ARC_CHART_ATTR B,P_PLACE_ATTR C	\
						WHERE A.TRANSITION_ID=TS.TRANSITION_ID	\
						AND A.PLACE_TYPE='INT'	\
						AND A.PLACE_ID=C.PLACE_ID	\
						AND A.ARC_ID = B.ARC_ID	\
						AND ( 	\
							(  B.ARC_TYPE = 'P' AND C.TOKEN_CNT >0 )	\
							OR (B.ARC_TYPE = 'O' AND C.TOKEN_CNT = 0)	\
							OR (	\
									NVL(TS.CON_TYPE,'AND') = 'XOR'  	\
									AND C.PLACE_CUR_STATE NOT IN (	\
										SELECT STATE FROM P_TRANSITION_STATE_ENUM 	\
										WHERE TRANSITION_ID =TS.TRANSITION_ID	\
										AND STATE_TYPE = 'I' 	\
									) 	\
							)	\
						)	\
			) ORDER BY NVL(TS.EXE_PRIORITY,0) " ;
		//printf("sql = %s\n",sql.c_str());
		query.setSQL(sql.c_str());
		if (l_wsnID != -1) {
			query.setParameter("WID",l_wsnID);
		}
		query.open();
		while(query.next()) {
			transList.push_back(query.field(0).asLong());
		}
		query.close();

	} catch (TOCIException &toe) {
		LOG(toe.getErrMsg());
		query.close();
	} catch (...) {
		query.close();
	};

	return;


};


void
PETRI::schd::collect1()
{
	long lLatestID = -1;
	transList1.clear();
	lLatestID = getLatestID();
	if (lLatestID <= m_lActionID) {
		return;
	};
	m_lActionID ++;

	char tmpLOG[2048]={0};

	DEFINE_PETRI_QUERY(query);
	try {
		string sql = "\
			SELECT DISTINCT B.TRANSITION_ID FROM P_ACTION A,P_TRANSITION_PLACE_RELA B  \
			WHERE A.ACTION_ID BETWEEN :ID1 AND :ID2 \
			AND A.ACTION_TYPE = 'M' \
			AND A.ELEMENT_TYPE = 'P' \
			AND A.ACTION IN ('A','D') \
			AND A.ELEMENT_ID = B.PLACE_ID \
			AND B.PLACE_TYPE = 'INT' ";
/*		if (l_drawID!= -1) {
			sql += "AND A.DRAW_ID IN ( \
								SELECT DRAW_ID FROM P_DRAW \
								START WITH DRAW_ID =:DRAWID \
								CONNECT BY PRIOR DRAW_ID = PAN_DRAW_ID )";
		}
*/
		if (l_wsnID!= -1) {
			sql += "AND B.TRANSITION_ID IN ( \
								SELECT TRANSITION_ID FROM P_TRANSITION_HOST_INFO \
								WHERE WSNADDR_ID = :WID )";
		}
		query.setSQL(sql.c_str());
		query.setParameter("ID1",m_lActionID);
		query.setParameter("ID2",lLatestID);
		if (l_wsnID != -1) {
			query.setParameter("WID",l_wsnID);
		}
		query.open();
		while(query.next()) {
			transList1.push_back(query.field(0).asLong());
			//for test
			sprintf(tmpLOG,"get sql[%s]\n\0",sql.c_str());
			LOG(tmpLOG);
			sprintf(tmpLOG,"SELECT TRANS_ID[%ld],BETWEEN (m_lActionID:%ld,lLatestID:%ld)\n\0",query.field(0).asLong(),m_lActionID,lLatestID);
			LOG(tmpLOG);
		}
		query.close();

	} catch (TOCIException &toe) {
		LOG(toe.getErrMsg());
		query.close();
	} catch (...) {
		query.close();
	};

	m_lActionID = lLatestID;

	return;


};



void PETRI::schd::sendout()
{
	unsigned int i;
	int itransLoopattr;
	int itransTriCnt;
	long ltransactionID = 0;
	DEFINE_PETRI_QUERY(query);
	string sql;
	for (i = 0;i<transList.size();i++) {

//		long tmp_transID = (long)transList[i];
//		transition ts(tmp_transID);
		transition ts((long)transList[i]);
		if( !ts.load()){
			continue;
		}
		
		if (! ts.checkTime()) {
			continue;
		}
		
		try {
			
			sql.clear();
			sql = " select nvl(LOOP_EXEC_CNT,1) from P_TRANSITION_ATTR WHERE TRANSITION_ID = :transiID ";
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("transiID",(long)transList[i]);
			query.open();
			if (query.next() ) {
				itransLoopattr = query.field(0).asInteger();
			} else {
				query.close();
				continue;
			}

			sql.clear();
			sql = "SELECT TRANSACTION_ID FROM PETRI_TRANSACTION WHERE DRAW_ID IN \
				(SELECT DRAW_ID FROM P_DRAW WHERE DRAW_LEVEL=1 START WITH DRAW_ID IN \
				(SELECT DRAW_ID FROM P_TRANSITION_ATTR WHERE TRANSITION_ID=:TS ) \
				CONNECT BY DRAW_ID = PRIOR PAN_DRAW_ID ) AND STATE='B'";
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("TS",(long)transList[i]);
			query.open();
			if (query.next() ) {
				ltransactionID = query.field(0).asLong();
			}

			sql.clear();
			sql = "SELECT nvl(MAX(EXEC_CNT),0) FROM P_TRANSITION_TRIGGER_CNT WHERE TRANSITION_ID = :TS AND TRANSACTION_ID = :TA ";
			query.close();
			query.setSQL(sql.c_str());
			query.setParameter("TS",(long)transList[i]);
			query.setParameter("TA",ltransactionID);
			query.open();
			if (query.next() ) {			
				itransTriCnt = query.field(0).asInteger();
			} else {
				itransTriCnt = 0;
			}
			query.close();
		} catch (TOCIException &toe) {
			LOG(toe.getErrMsg());
			query.close();
			continue;
		} catch (...) {
			query.close();
			continue;
		}

		if(itransTriCnt >= itransLoopattr){continue;}
		
		//send msg
		sprintf(P_CMDMSG->mbody,
					"<iNeedAck=2><iOperation=1><lTransitionID=%ld><iStaffID=1>\0",
					transList[i]
					);
		P_CMDMSG->mtype = (long) mypid;
		LOG(P_CMDMSG->mbody);
		if (P_MQ.cmdMQ.send()) {
			LOG( "send Success\n");
			if(0 == itransTriCnt){
				try {
					sql.clear();
					sql = " INSERT INTO P_TRANSITION_TRIGGER_CNT (TRANSITION_ID,TRANSACTION_ID,EXEC_CNT) VALUES (:TS , :TA , 1) ";
					query.close();
					query.setSQL(sql.c_str());
					query.setParameter("TS",(long)transList[i]);
					query.setParameter("TA",ltransactionID);
					query.execute();
				} catch (TOCIException &toe) {
					LOG(toe.getErrMsg());
					query.close();
				} catch (...) {
					query.close();
				}
			}else{
				try {
					sql.clear();
					sql = " UPDATE P_TRANSITION_TRIGGER_CNT SET EXEC_CNT = EXEC_CNT + 1 \
						WHERE TRANSITION_ID = :TS AND TRANSACTION_ID = :TA ";
					query.close();
					query.setSQL(sql.c_str());
					query.setParameter("TS",(long)transList[i]);
					query.setParameter("TA",ltransactionID);
					query.execute();
				} catch (TOCIException &toe) {
					LOG(toe.getErrMsg());
					query.close();
				} catch (...) {
					query.close();
				}
			}
		}
		else {
			LOG( "send Fail\n");
		}

		P_DB.getDB()->commit(); 
//		sleep(30);
	};
	query.close();
	return;
};

inline void  PETRI::schd::state()
{
	unsigned idx;
	for (idx = 0 ;idx < transList1.size();idx++ ) 
		updateTransState(transList1[idx]);
	P_DB.getDB()->commit();

}

void PETRI::schd::doschd()
{
	int iStatus = 0;
	pid_t pid;
	if( (pid = fork())<0 ){
		LOG("fork error \n");
	}else if ( 0 == pid){
		string srevMsg;
		string sql;
		long ltransID;
		DEFINE_PETRI_QUERY(query);
		while(1) {
			if(P_MQ.ackMQ.receive(mypid)){
				srevMsg = P_ACKMSG->mbody;
				P_RECVBUFF.reset();
				P_RECVBUFF.parseBuff(srevMsg.c_str());
				ltransID = P_RECVBUFF.getTransitionID();
				try {
					sql = " INSERT INTO P_SCHD_LOG (TRANSITION_ID, CREATED_DATE, RESULTS) VALUES (:TS, SYSDATE, :RS) ";
					query.close();
					query.setSQL(sql.c_str());
					query.setParameter("TS",ltransID);
					query.setParameter("RS",srevMsg.c_str());
					query.execute();
				} catch (TOCIException &toe) {
					LOG(toe.getErrMsg());
					continue;
				} catch (...) {
					continue;
				}
				query.commit();	
			}

			sleep(10);
			
		}
		exit(0);
	}else{
		printf("pid=%d",pid);
		m_lActionID = getLatestID();
		do {
			collect1();
			state();
			collect();
			sendout();
//			LOG("sleep 3 \n");

			sleep(3);
		} while(1);
	}
	wait(&iStatus);
	return;

};


int main(int argc,char** argv)
{
	int i;
//	long lDrawID = -1;
	long lwsnID = -1;
	char sMsg[256];
	strcpy(sMsg,"Hello Petri!");
	/*
	i = getopt(argc,argv,"w:"); 
	if ( i == 'w'  && optarg) {
		lwsnID = atol(optarg);
	};
	*/
	
	ReadIni reader; //≤‚ ‘”√
	char sFile[254];
	strcpy(sFile,"./HostAddress.ini");
	
	FILE *fd = fopen(sFile,"r");

	lwsnID = reader.readIniInt (fd, "HostAddress", "AddressID", 0);
	fclose(fd);
	PETRI::schd schedule;
	/*
	Log::m_iAlarmLevel = 3;
	Log::m_iAlarmDbLevel = -1;

	ALARMLOG28(0,MBC_CLASS_Business,0,"%s",sMsg);
	LOGG(SYSTEMLOG,sMsg);
	*/
	if (lwsnID != -1 ) {
		schedule.setWSNID(lwsnID);
	}
	schedule.doschd();
	return 0;
};

