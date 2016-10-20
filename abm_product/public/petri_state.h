#ifndef PETRI_RUN_UPDATE_STATE
#define PETRI_RUN_UPDATE_STATE

void UpdateExecStat(char *sAppName,int iStat = 0)
{
	DEFINE_QUERY(qry);
	char sql[1024] = {0};
	if( iStat == 0 )
		sprintf(sql,"update P_TRANSITION_SHELLPROC_RESULT set state='END',state_date=sysdate where app_name='%s'",sAppName);
	else
		sprintf(sql,"update P_TRANSITION_SHELLPROC_RESULT set state='ERR',state_date=sysdate where app_name='%s'",sAppName);
	qry.setSQL(sql);
	qry.execute();
	qry.commit();
	qry.close();
	
}

#endif