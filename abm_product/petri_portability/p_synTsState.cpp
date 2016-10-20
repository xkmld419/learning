#include "petri.h"

namespace PETRI {

	long PETRI::synTsState::getTsLatestID()
	{
	
		long lID = -1;
		DEFINE_PETRI_QUERY(query);
		try {
			string sql = "SELECT NVL(MAX(ACTION_ID),-1) ID FROM P_ACTION \
										WHERE ELEMENT_TYPE = 'T' ";
			query.close();
			query.setSQL(sql.c_str());
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
	
	}
	
	void PETRI::synTsState::dosynNetState()
	{
		long m_lsubDrawID =-1;
		long m_ltransID =-1;
		long m_ldrawID = -1;
		string m_sname;
		string m_sexeState;
		vector<string> v_exeState;
		string sql;
		DEFINE_PETRI_QUERY(query);
		DEFINE_PETRI_QUERY(query1);
		do {
			try {
				query.close();
				sql.clear();
				sql = " SELECT A.SUBNET_DRAW_ID,A.TRANSITION_ID,A.DRAW_ID,A.TRANSITION_NAME,A.EXE_STATE \
					FROM P_TRANSITION_ATTR A,P_DRAW_RELA B \
					WHERE A.SUBNET_DRAW_ID = B.DRAW_ID \
					AND B.DRAW_GRANULARITY = 1 \
					AND B.SYN_TYPE = 'SYN' \
					AND A.SUBNET_DRAW_ID IS NOT NULL ";
				query.setSQL(sql.c_str());
				query.open();
				while(query.next()) {
					m_lsubDrawID = query.field("SUBNET_DRAW_ID").asLong();
					m_ltransID = query.field("TRANSITION_ID").asLong();
					m_ldrawID = query.field("DRAW_ID").asLong();
					m_sname = query.field("TRANSITION_NAME").asString();
					m_sexeState = query.field("EXE_STATE").asString();
					
					v_exeState.clear();
					
					query1.close();
					sql.clear();
					sql = "SELECT DISTINCT EXE_STATE EXE_STATE FROM P_TRANSITION_ATTR WHERE DRAW_ID = :DID";
					query1.setSQL(sql.c_str());
					query1.setParameter("DID",m_lsubDrawID);
					query1.open();
					while(query1.next()) {
						string s_tmp = query1.field("EXE_STATE").asString();
						v_exeState.push_back(s_tmp);
					}
					query1.close();
					
					if (0 == v_exeState.size()){
						continue;
					}

					if (1 == v_exeState.size() && (0 == v_exeState[0].compare("E")) ) {
						if (0 != m_sexeState.compare("E")) {
							sql.clear();
							sql= "UPDATE P_TRANSITION_ATTR SET EXE_STATE='E' WHERE TRANSITION_ID=:TS";
							query1.close();
							query1.setSQL(sql.c_str());
							query1.setParameter("TS",m_ltransID);
							query1.execute();
							query1.close();

							sql.clear();
							sql = "INSERT INTO P_ACTION(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE,DRAW_ID,ACTION_TYPE,ELEMENT_NAME,ERR_MSG) VALUES(SEQ_PETRI_ACTION_ID.NEXTVAL,:TS,'T','EXEC_STATE','E','S',SYSDATE,:DRAWID,'A',:ELMNAME,NULL)";
							query1.close();
							query1.setSQL(sql.c_str());
							query1.setParameter("TS",m_ltransID);
							query1.setParameter("DRAWID",m_ldrawID);
							query1.setParameter("ELMNAME",m_sname.c_str());
							query1.execute();
							query1.close();
						}
						goto _ONC_TS_END;
					}
					
					if (1 == v_exeState.size() && (0 == v_exeState[0].compare("D")) ) {
						if (0 != m_sexeState.compare("D")){
							sql.clear();
							sql= "UPDATE P_TRANSITION_ATTR SET EXE_STATE='D' WHERE TRANSITION_ID=:TS";
							query1.close();
							query1.setSQL(sql.c_str());
							query1.setParameter("TS",m_ltransID);
							query1.execute();
							query1.close();

							sql.clear();
							sql = "INSERT INTO P_ACTION(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE,DRAW_ID,ACTION_TYPE,ELEMENT_NAME,ERR_MSG) VALUES(SEQ_PETRI_ACTION_ID.NEXTVAL,:TS,'T','EXEC_STATE','D','S',SYSDATE,:DRAWID,'A',:ELMNAME,NULL)";
							query1.close();
							query1.setSQL(sql.c_str());
							query1.setParameter("TS",m_ltransID);
							query1.setParameter("DRAWID",m_ldrawID);
							query1.setParameter("ELMNAME",m_sname.c_str());
							query1.execute();
							query1.close();
						}
						goto _ONC_TS_END;
					}

					for (int i = 0;i< v_exeState.size();i++ ) {
						if ("X" == v_exeState[i] ){
							if (0 != m_sexeState.compare("X")){
								sql.clear();
								sql= "UPDATE P_TRANSITION_ATTR SET EXE_STATE='X' WHERE TRANSITION_ID=:TS";
								query1.close();
								query1.setSQL(sql.c_str());
								query1.setParameter("TS",m_ltransID);
								query1.execute();
								query1.close();

								sql.clear();
								sql = "INSERT INTO P_ACTION(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE,DRAW_ID,ACTION_TYPE,ELEMENT_NAME,ERR_MSG) VALUES(SEQ_PETRI_ACTION_ID.NEXTVAL,:TS,'T','EXEC_STATE','X','S',SYSDATE,:DRAWID,'A',:ELMNAME,NULL)";
								query1.close();
								query1.setSQL(sql.c_str());
								query1.setParameter("TS",m_ltransID);
								query1.setParameter("DRAWID",m_ldrawID);
								query1.setParameter("ELMNAME",m_sname.c_str());
								query1.execute();
								query1.close();
							}
							goto _ONC_TS_END;
						}
					}
					
					if (0 != m_sexeState.compare("R")){ 
						sql.clear();
						sql= "UPDATE P_TRANSITION_ATTR SET EXE_STATE='R' WHERE TRANSITION_ID=:TS";
						query1.close();
						query1.setSQL(sql.c_str());
						query1.setParameter("TS",m_ltransID);
						query1.execute();
						query1.close();
					
						sql.clear();
						sql = "INSERT INTO P_ACTION(ACTION_ID,ELEMENT_ID,ELEMENT_TYPE,ATTR,ATTR_VALUE,ACTION,CREATE_DATE,DRAW_ID,ACTION_TYPE,ELEMENT_NAME,ERR_MSG) VALUES(SEQ_PETRI_ACTION_ID.NEXTVAL,:TS,'T','EXEC_STATE','R','S',SYSDATE,:DRAWID,'A',:ELMNAME,NULL)";
						query1.close();
						query1.setSQL(sql.c_str());
						query1.setParameter("TS",m_ltransID);
						query1.setParameter("DRAWID",m_ldrawID);
						query1.setParameter("ELMNAME",m_sname.c_str());
						query1.execute();
						query1.close();
					}

					_ONC_TS_END:
						
					query1.commit();
				}
			} catch (TOCIException &toe) {
				LOG(toe.getErrMsg());
				query.close();
			} catch (...) {
				query.close();
			}

		sleep(3);

		} while(1);
	}

	void PETRI::synTsState::dosynTsState()
	{
		long lLatestID = -1;
		DEFINE_PETRI_QUERY(query);
		DEFINE_PETRI_QUERY(query1);
		string sql;
		long m_ltransID = -1;
		long m_lrelaTransID = -1;
		string m_sattrValue;
		string m_serrMsg;
		
		m_lActionID = getTsLatestID();
		do {
			lLatestID = getTsLatestID();
			if (lLatestID <= m_lActionID) {
				continue;
			}
			m_lActionID ++;
			try {
				query.close();
				sql.clear();
				sql = "select ELEMENT_ID,ATTR_VALUE,ERR_MSG from P_ACTION \
					WHERE ACTION = 'U' \
					AND ELEMENT_TYPE = 'T' \
					AND ATTR = 'EXEC_STATE' \
					AND ACTION_ID BETWEEN :ID1 AND :ID2 ";
				query.setSQL(sql.c_str());
				query.setParameter("ID1",m_lActionID);
				query.setParameter("ID2",lLatestID);
				query.open();
				while(query.next()) {
					m_ltransID = query.field("ELEMENT_ID").asLong();
					m_sattrValue.clear();
					m_sattrValue = query.field("ATTR_VALUE").asString();
					m_serrMsg.clear();
					m_serrMsg = query.field("ERR_MSG").asString();

					query1.close();
					sql.clear();
					sql = "select RELA_TRANSITION_ID FROM P_TRANSITION_SYNCH_RELA WHERE TRANSITION_ID = :TS ";
					query1.setSQL(sql.c_str());
					query1.setParameter("TS",m_ltransID);
					query1.open();
					if(query1.next()){
						m_lrelaTransID = query1.field("RELA_TRANSITION_ID").asLong();
					}else{
						continue;
					}
					transition tmp_trans(m_lrelaTransID);
					bool bret = tmp_trans.load();
					if (!bret){
						char loaderr[50];
						sprintf(loaderr,"transition:%d load faill\0",m_lrelaTransID);
						LOG(loaderr);
					}
					tmp_trans.switchState(m_sattrValue,m_serrMsg,"S");
				}			
			} catch (TOCIException &toe) {
					LOG(toe.getErrMsg());
					query.close();
			} catch (...) {
					query.close();
			}
			m_lActionID = lLatestID;
			sleep(3);
		} while(1);
	}

}


int main () 
{
	PETRI::synTsState m_synTsState;

	int iStatus = 0;
	pid_t pid;
	if( (pid = fork())<0 ){
		PETRI::LOG("fork error \n");
	}else if ( 0 == pid){
		m_synTsState.dosynNetState();
	}else{
		m_synTsState.dosynTsState();
	}
	return 0;
}




