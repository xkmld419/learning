#include "P_messageSend.h"
#include "petri.h"



void  PETRI::message::loadInMessage()
{
	char sql[1024];
	m_size =0;
	
	try {
		strcpy(sql, "	SELECT RULE_ID,TRANSITION_ID,STAFF_GROUP_ID,BEGIN_DEFER,TIME_OFFSET, ALARM_COUNT,ALARM_INTERVAL,ALARM_TYPE_TIME,ALARM_TYPE_BEGIN,ALARM_TYPE_END,ALARM_TYPE_ERR	\
			FROM P_ALARM_RULE ");
		DEFINE_PETRI_QUERY (query);
		query.setSQL(sql);
		query.open();
		while(query.next()) {

			m_RULE_ID[m_size]			= query.field(0).asInteger();
			m_TRANSITION_ID[m_size]     = query.field(1).asInteger();
			m_STAFF_GROUP_ID[m_size]    = query.field(2).asInteger();
			m_BEGIN_DEFER[m_size]		= query.field(3).asInteger();
			m_TIME_OFFSET[m_size]		= query.field(4).asInteger();
			m_COUNT[m_size]				= query.field(5).asInteger();
			m_INTERVAL[m_size]			= query.field(6).asInteger();
			m_TYPE_TIME[m_size] 		= query.field(7).asInteger();
			m_TYPE_BEGIN[m_size]		= query.field(8).asInteger();
			m_TYPE_END[m_size]			= query.field(9).asInteger();
			m_TYPE_ERR[m_size]			= query.field(10).asInteger();
			m_size++;

		}
		query.close();


	}catch(...) {
		return;

	}
	return;

};







int PETRI::message::noFireSendMessage()
{
	int i=0;
	int tmp_count[2000] ={0};	
	int oldBillID =0;
	int newBillID=0;	
	string sql1;
	string sql2;

	while(1)
	{
		DEFINE_PETRI_QUERY (query);
				query.close();
				string sql = "SELECT BILLING_CYCLE_ID \
				  FROM BILLING_CYCLE  \
				 WHERE BILLING_CYCLE_ID =  \
					   (SELECT MIN(BILLING_CYCLE_ID)  \
						  FROM BILLING_CYCLE   \
						 WHERE STATE IN ('10R', '10B', '10A', '10D') \
						   AND BILLING_CYCLE_TYPE_ID = 1)";
				
		query.setSQL(sql.c_str());
		query.open();
		if (query.next()) {
			newBillID = query.field(0).asInteger();
		}
		if(oldBillID ==0) 
		{
			oldBillID=newBillID;
		}
		else
		{
			if(oldBillID!=newBillID)
			{
				 memset(tmp_count,0,sizeof(tmp_count)); 
			}
		}
				
		loadInMessage();
		for (i=0;i<m_size;i++)	
		{

			if (m_TYPE_TIME[i] == 1)
			{
					long transID = m_TRANSITION_ID[i];
					PETRI::transition ts(transID);
					bool bret;
					bret = ts.load();
					
					if (ts.checkTimeforMessage(m_BEGIN_DEFER[i])&&(ts.getExecState() == "A")&&(ts.getExecAttr() == 1)) 
					{
						int i_count = 0;
						long staff_id[100];
						string call_nbr[100];
						long ser_id[100];
						char tempChar[100];
						char sql[1024];
						if(tmp_count[i] < m_COUNT[i])
						{
							tmp_count[i]++;
						}
						else
						{
							continue;
						}
						sprintf(tempChar,"%s对应的变迁ID为%d,当前状态为等待点火，已经点火超时，请点火",ts.getName(),transID);
						
						
						DEFINE_PETRI_QUERY (qry);
						qry.close();
						sprintf(sql, "select STAFF_ID,ACC_NBR,SERV_ID from P_GROUP_STAFF where STAFF_GROUP_ID = '%d'",m_STAFF_GROUP_ID[i]);
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
							string sql1= "insert into P_ALARM_MSG(MSG_ALARM_ID,STAFF_ID,MSG_TYPE,MSG_NOTE,DEAL_FLAG,DEAL_DATE,CREATE_DATE,REMARK)  \
											values(SEQ_P_ALARM_MSG_ID.NEXTVAL,:STRAFF_ID,'T',:TEMPCHAR,'N',SYSDATE,SYSDATE,'')";
							qry.close();
							qry.setSQL(sql1.c_str());
							qry.setParameter("STRAFF_ID",staff_id[i]);
							qry.setParameter("TEMPCHAR",tempChar);
							qry.execute();	
							qry.commit();
						}
						
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
						}
						
					}
					else if ((ts.getExecState() == "R")&&(ts.getExecAttr() == 1)) 
					{
						int i_count = 0;
						long staff_id[100];
						string call_nbr[100];
						long ser_id[100];
						char tempChar[100];
						char sql[1024];
						if(tmp_count[i] < m_COUNT[i])
						{
							tmp_count[i]++;
						}
						else
						{
							continue;
						}

						
						Date now;
						sprintf(tempChar,"%s对应的变迁ID为%d,当前状态为已经点火，点火时间为%s",ts.getName(),transID,now.toString());
						
						
						DEFINE_PETRI_QUERY (qry);
						qry.close();
						sprintf(sql, "select STAFF_ID,ACC_NBR,SERV_ID from P_GROUP_STAFF where STAFF_GROUP_ID = '%d'",m_STAFF_GROUP_ID[i]);
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
							string sql1= "insert into P_ALARM_MSG(MSG_ALARM_ID,STAFF_ID,MSG_TYPE,MSG_NOTE,DEAL_FLAG,DEAL_DATE,CREATE_DATE,REMARK)  \
											values(SEQ_P_ALARM_MSG_ID.NEXTVAL,:STRAFF_ID,'R',:TEMPCHAR,'N',SYSDATE,SYSDATE,'')";
							qry.close();
							qry.setSQL(sql1.c_str());
							qry.setParameter("STRAFF_ID",staff_id[i]);
							qry.setParameter("TEMPCHAR",tempChar);
							qry.execute();	
							qry.commit();
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
						}
						
					}
					else if ((ts.getExecState() == "X")&&(ts.getExecAttr() == 1)) 
					{
						int i_count = 0;
						long staff_id[100];
						string call_nbr[100];
						long ser_id[100];
						char tempChar[100];
						char sql[1024];
						if(tmp_count[i] < m_COUNT[i])
						{
							tmp_count[i]++;
						}
						else
						{
							continue;
						}

						
						Date now;
						sprintf(tempChar,"%s对应的变迁ID为%d,当前状态为已经出错，出粗时间为%s",ts.getName(),transID,now.toString());
						
						
						DEFINE_PETRI_QUERY (qry);
						qry.close();
						sprintf(sql, "select STAFF_ID,ACC_NBR,SERV_ID from P_GROUP_STAFF where STAFF_GROUP_ID = '%d'",m_STAFF_GROUP_ID[i]);
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
							string sql1= "insert into P_ALARM_MSG(MSG_ALARM_ID,STAFF_ID,MSG_TYPE,MSG_NOTE,DEAL_FLAG,DEAL_DATE,CREATE_DATE,REMARK)  \
											values(SEQ_P_ALARM_MSG_ID.NEXTVAL,:STRAFF_ID,'E',:TEMPCHAR,'N',SYSDATE,SYSDATE,'')";
							qry.close();
							qry.setSQL(sql1.c_str());
							qry.setParameter("STRAFF_ID",staff_id[i]);
							qry.setParameter("TEMPCHAR",tempChar);
							qry.execute();	
							qry.commit();

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
						}
						
					}
					else if ((ts.getExecState() == "E")&&(ts.getExecAttr() == 1)) 
					{
						int i_count = 0;
						long staff_id[100];
						string call_nbr[100];
						long ser_id[100];
						char tempChar[100];
						char sql[1024];
						if(tmp_count[i] < m_COUNT[i])
						{
							tmp_count[i]++;
						}
						else
						{
							continue;
						}
						Date now;
						sprintf(tempChar,"%s对应的变迁ID为%d,当前状态为已经正确结束，结束时间为%s",ts.getName(),transID,now.toString());
						
						
						DEFINE_PETRI_QUERY (qry);
						qry.close();
						sprintf(sql, "select STAFF_ID,ACC_NBR,SERV_ID from P_GROUP_STAFF where STAFF_GROUP_ID = '%d'",m_STAFF_GROUP_ID[i]);
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
							string sql1= "insert into P_ALARM_MSG(MSG_ALARM_ID,STAFF_ID,MSG_TYPE,MSG_NOTE,DEAL_FLAG,DEAL_DATE,CREATE_DATE,REMARK)  \
											values(SEQ_P_ALARM_MSG_ID.NEXTVAL,:STRAFF_ID,'O',:TEMPCHAR,'N',SYSDATE,SYSDATE,'')";
							qry.close();
							qry.setSQL(sql1.c_str());
							qry.setParameter("STRAFF_ID",staff_id[i]);
							qry.setParameter("TEMPCHAR",tempChar);
							qry.execute();	
							qry.commit();
						}
						qry.close();
						for (int i =0;i<i_count;i++)
						{
							sql2= "insert into A_SMS_SEND \
							  (sms_send_id,serv_id,acc_nbr,balance,amount ,exp_date ,msg_type_id,created_date ,staff_id ,state,state_date ,send_success_count ,send_fail_count,send_success_date,send_fail_date ,msg_content,content,reserved ,billing_mode_id,network_id ,source_type,acct_credit,generate_flag)	\
							  values(SMS_SEND_ID_SEQ.NEXTVAL,:SER_ID,:CALL_NBR,'','','','53',SYSDATE,:STRAFF_ID,'0','','','','','',:TEMPCHAR,'','','','','','','')";							
						  qry.close();
							qry.setSQL(sql2.c_str());
							qry.setParameter("STRAFF_ID",staff_id[i]);
							qry.setParameter("TEMPCHAR",tempChar);
							qry.setParameter("CALL_NBR",call_nbr[i].c_str());
							qry.setParameter("SER_ID",ser_id[i]);
							qry.execute();	
							qry.commit();
						}
						
					}


			}

		}
		sleep(10);

	}
	
	
	
	

}



int PETRI::message::TimeOutWriteToBSN()
{
	int i_count=0;
	int tmp_count[2000] ={0};	
	int TRANSID[2000] ={0};
	int EX_VALUE[2000]={0};
	string m_sname;
	char tmpTime[30]={0};
	char tmpTime2[30]={0};
	char nowtTime[30]={0};

	long timelong;
	long transID;
	char tempChar[200]={0};
	DEFINE_PETRI_QUERY (query);
	DEFINE_PETRI_QUERY (query1);

	while(1)
	{
		query.close();
		string sql = "SELECT TRANSITION_ID,nvl(EXPERIENCE_VALUE,0) \
				  FROM P_TRANSITION_ATTR   \
				 WHERE EXE_STATE = 'R'";
				
		query.setSQL(sql.c_str());
		query.open();
		while (query.next())
		{
			TRANSID[i_count] = query.field(0).asLong();
			EX_VALUE[i_count] = query.field(1).asLong();
			i_count++;
		
		}
		for(int i=0;i<i_count;i++)
		{
			if(EX_VALUE[i] == 0)
			{
				continue;
			}
			string sql1 = "SELECT  max(to_char(START_TIME, 'yyyymmddhh24miss')) \
					  FROM P_TRANSITION_EXEC_LOG   \
					 WHERE TRANSITION_ID = :ID";
			query1.close();
			query1.setSQL(sql1.c_str());
			query1.setParameter("ID",TRANSID[i]);
			query1.open();
			while (query1.next())
			{
				strcpy(tmpTime,query1.field(0).asString());
			}


			string sql2 = "SELECT  to_char(SYSDATE,'yyyymmddhh24miss'),max(to_char(RUN_TIME,'yyyymmddhh24miss')) \
					  FROM P_TRANSITION_RUN_TIME   \
					 WHERE TRANSITION_ID = :ID";
			query1.close();
			query1.setSQL(sql2.c_str());
			query1.setParameter("ID",TRANSID[i]);
			query1.open();
			while (query1.next())
			{
				strcpy(nowtTime,query1.field(0).asString());
				strcpy(tmpTime2,query1.field(1).asString());
			}


			if(strcmp(tmpTime,tmpTime2)==0)
			{
				continue;
			}
			
			Date now(nowtTime);
			Date startTime(tmpTime);
			timelong = now.diffSec(startTime);
			if (timelong >(EX_VALUE[i]*60))
			{	

				string sql2 = "select TRANSITION_ID,TRANSITION_NAME from p_transition_attr where subnet_draw_id in \
					(select draw_id  from P_TRANSITION_ATTR where TRANSITION_ID = :TID) ";
				query.close();
				query.setSQL(sql2.c_str());
				query.setParameter("TID",TRANSID[i]);
				query.open();
				while (query.next())
				{
					transID = query.field(0).asLong();
					m_sname = query.field(1).asString();
				}
				char sql3[1024];
				//string sql3 = "INSERT INTO P_TRANSITION_RUN_TIME(TRANSITION_ID,RUN_TIME)VALUES(:ID,TO_DATE(:DATE,'YYYYMMDDHH24MISS'))";
				sprintf(sql3,"INSERT INTO P_TRANSITION_RUN_TIME(TRANSITION_ID,RUN_TIME)VALUES(%d,TO_DATE(%s,'YYYYMMDDHH24MISS'))",TRANSID[i],tmpTime);
				query1.close();
				query1.setSQL(sql3);
				//query1.setParameter("ID",TRANSID[i]);
				//query1.setParameter("DATE",tmpTime);
				query1.execute();
				query1.commit();



				sprintf(tempChar,"子网%d下面对应的变迁ID为%d的变迁,执行时间超过经验时长，请检查",transID,TRANSID[i]);
										
				transition ts(transID);
				ts.load();
				sql.clear();
				sql = "INSERT INTO P_ALARM_BSN(BSN_ALARM_ID,TRANSACTION_ID,DRAW_ID,TRANSITION_ID,EXEC_STATE,STATE_DATE,ALARM_TYPE,ALARM_DESC,ALARM_FLAG,REMARK) \
					VALUES(seq_p_alarm_bsn_id.NEXTVAL,:TA,:DRAWID,:TS,'R',SYSDATE,'T',:ELMNAME,'N',:REMARK)";
				query.close();
				query.setSQL(sql.c_str());
				ts.getTransactionID();
				m_sname = m_sname	+ "THE EXPERIENCE_VALUE TIMEOUT ";
				query.setParameter("TS",transID);
				query.setParameter("TA",ts.l_transactionID);
				query.setParameter("DRAWID",ts.getDrawID());
				query.setParameter("ELMNAME",m_sname.c_str());
				query.setParameter("REMARK",tempChar);
				query.execute();
				query.commit();

			}

		}

		sleep(20);

	}
}



main()
{
	PETRI::message Msg;
	int iStatus = 0;
	pid_t pid;
	if( (pid = fork())<0 ){
		PETRI::LOG("fork error \n");
	}else if ( 0 == pid){
		Msg.noFireSendMessage();
	}else{
				Msg.TimeOutWriteToBSN();

	}
}




