#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include "Date.h"
#include "Environment.h"

#include "HssNmpInfoCollect.h"
using namespace std;

void init_HSS_SC_pairs()
{
	memset(HSS_SC_pairs, 0 ,sizeof(HSS_SC_pairs));
	

	strcpy(HSS_SC_pairs[SCALAR_NEID].oid_value,NeId_oid);
	strcpy(HSS_SC_pairs[SCALAR_SRID].oid_value,SrId_oid);
	strcpy(HSS_SC_pairs[SCALAR_NENAME].oid_value,NeName_oid);
	strcpy(HSS_SC_pairs[SCALAR_DEPLOYADDR].oid_value,DeployAddr_oid);
	strcpy(HSS_SC_pairs[SCALAR_EVENDORNAME].oid_value,EVendorName_oid);
	strcpy(HSS_SC_pairs[SCALAR_NEVERSION].oid_value,NeVersion_oid);
	strcpy(HSS_SC_pairs[SCALAR_NODEID].oid_value,NodeId_oid);
	strcpy(HSS_SC_pairs[SCALAR_MAX_SERV_NUM].oid_value,max_serv_num_oid);
	strcpy(HSS_SC_pairs[SCALAR_MAX_PRODUCT_OFFER_INST_NUM].oid_value,max_product_offer_inst_num_oid);
	strcpy(HSS_SC_pairs[SCALAR_LOG_DATA_SIZE].oid_value,log_data_size_oid);
	strcpy(HSS_SC_pairs[SCALAR_CPU_WARNING_VALUE].oid_value,cpu_warning_value_oid);
	strcpy(HSS_SC_pairs[SCALAR_MEM_WARNING_VALUE].oid_value,mem_warning_value_oid);
	strcpy(HSS_SC_pairs[SCALAR_MAX_DCC_REQUEST_CNT].oid_value,max_dcc_request_cnt_oid);
	strcpy(HSS_SC_pairs[SCALAR_ARCHIVE_PATH].oid_value,archive_path_oid);
	strcpy(HSS_SC_pairs[SCALAR_SIZE].oid_value,size_oid);
	strcpy(HSS_SC_pairs[SCALAR_CHECK_PROCESS_TIME].oid_value,check_process_time_oid);
	strcpy(HSS_SC_pairs[SCALAR_PARAM_PATH].oid_value,param_path_oid);
	strcpy(HSS_SC_pairs[SCALAR_HSS_BSN_STATE].oid_value,HSS_bsn_state_oid);
	strcpy(HSS_SC_pairs[SCALAR_HSS_NE_STATE].oid_value,HSS_ne_state_oid);
	strcpy(HSS_SC_pairs[SCALAR_HSS_SYSUPTIME].oid_value,HSS_sysUptime_oid);
	strcpy(HSS_SC_pairs[SCALAR_NOT_BILLING_MSG_NUM].oid_value,not_billing_msg_num_oid);
	strcpy(HSS_SC_pairs[SCALAR_HQ_HSS_MSG_NUM].oid_value,hq_hss_msg_num_oid);
	strcpy(HSS_SC_pairs[SCALAR_OFCS_MSG_NUM ].oid_value,ofcs_msg_num_oid);
	strcpy(HSS_SC_pairs[SCALAR_OCS_MSG_NUM ].oid_value,ocs_msg_num_oid);
	strcpy(HSS_SC_pairs[SCALAR_VC_MSG_NUM ].oid_value,vc_msg_num_oid);
	strcpy(HSS_SC_pairs[SCALAR_UPDATE_TIME].oid_value,update_time_oid);
	strcpy(HSS_SC_pairs[SCALAR_OFCS_STOCK_COUNT ].oid_value,ofcs_stock_count_oid);
	strcpy(HSS_SC_pairs[SCALAR_OFCS_STOCK_SYSUPTIME].oid_value,ofcs_stock_sysUptime_oid);
	strcpy(HSS_SC_pairs[SCALAR_OCS_STOCK_COUNT ].oid_value,ocs_stock_count_oid);
	strcpy(HSS_SC_pairs[SCALAR_OCS_STOCK_SYSUPTIME].oid_value,ocs_stock_sysUptime_oid);
	strcpy(HSS_SC_pairs[SCALAR_VC_STOCK_COUNT ].oid_value,vc_stock_count_oid);
	strcpy(HSS_SC_pairs[SCALAR_VC_STOCK_SYSUPTIME ].oid_value,vc_stock_sysUptime_oid);	
	
}

void init_Table_pairs()
{
	
	//3.1.3.2.1	稽核类参数
	int i = 0;
	memset(audit_classTable,0,sizeof(audit_classTable));
	while(i < AUDIT_CLASSTABLE_COUNT) {
		strcpy(audit_classTable[i++].oid_value,audit_classTable_oid);
	}

	//3.1.3.2.2	业务处理参数   :   超时和重发次数列表
	i = 0;
	memset(time_out_numTable,0,sizeof(time_out_numTable));
	while(i < TIME_OUT_NUMTABLE_COUNT) {
		strcpy(time_out_numTable[i++].oid_value,time_out_numTable_oid);
	}


	//3.1.3.3	     内存类参数:  数据对象共享内存配置
	i = 0;
	memset(data_objTable,0,sizeof(data_objTable));
	while(i < DATA_OBJTABLE_COUNT) {
		strcpy(data_objTable[i++].oid_value,data_objTable_oid);
	}



	//3.1.3.4   资源类参数:  网元资源配置列表
	i = 0;
	memset(ne_resourceTable,0,sizeof(ne_resourceTable));
	while(i < NE_RESOURCETABLE_COUNT) {
		strcpy(ne_resourceTable[i++].oid_value,ne_resourceTable_oid);
	}


	//3.1.3.4   资源类参数:  主机资源配置列表
	i = 0;
	memset(host_resourceTable,0,sizeof(host_resourceTable));
	while(i < HOST_RESOURCETABLE_COUNT) {
		strcpy(host_resourceTable[i++].oid_value,host_resourceTable_oid);
	}


	//3.1.3.4   资源类参数:  模块资源配置列表
	i = 0;
	memset(module_resourceTable,0,sizeof(module_resourceTable));
	while(i < MODULE_RESOURCETABLE_COUNT) {
		strcpy(module_resourceTable[i++].oid_value,module_resourceTable_oid);
	}


	//3.1.3.5.1	数据库资源相关参数
	i = 0;
	memset(databaseTable,0,sizeof(databaseTable));
	while(i < DATABASETABLE_COUNT) {
		strcpy(databaseTable[i++].oid_value,databaseTable_oid);
	}


	//3.1.3.5.2	主机资源相关参数
	i = 0;
	memset(hss_hostTable,0,sizeof(hss_hostTable));
	while(i < HSS_HOSTTABLE_COUNT) {
		strcpy(hss_hostTable[i++].oid_value,hss_hostTable_oid);
	}



	//3.1.3.5.3	连接HSS网元的相关参数
	i = 0;
	memset(hss_ne_Table,0,sizeof(hss_ne_Table));
	while(i < HSS_NE_TABLE_COUNT) {
		strcpy(hss_ne_Table[i++].oid_value,hss_ne_Table_oid);
	}


	//3.1.3.5.4	日志相关参数:  模块日志配置
	i = 0;
	memset(module_log_confTable,0,sizeof(module_log_confTable));
	while(i < MODULE_LOG_CONFTABLE_COUNT) {
		strcpy(module_log_confTable[i++].oid_value,module_log_confTable_oid);
	}
	
}


time_t stringTotime_t(char* s)
{
	tm t;
    sscanf(s, "%4d%02d%02d%02d%02d%02d",&t.tm_year,
			&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec);
    t.tm_year = t.tm_year - 1900;
    t.tm_mon  = t.tm_mon - 1;

  	t.tm_isdst = 0;
	return mktime(&t);
} 


HssNmpInfoCollect::HssNmpInfoCollect()
{
	m_poCmdCom = NULL;
	m_poCmdCom = new CommandCom();
	if(!m_poCmdCom) {
		printf("new CommandCom error in HssNmpInfoCollect()\n");
		exit(-1);
	}

	if(!m_poCmdCom->InitClient() ) {
		printf("m_poCmdCom->InitClient() error in HssNmpInfoCollect()!\n");	
		exit(-1);
	}

	init_HSS_SC_pairs ();
	init_Table_pairs ();

	m_poQuery = NULL;
	//static TOCIDatabase oDBConn;
	//oDBConn.connect("hss","hss","bill");
	//m_poQuery = new TOCIQuery(&oDBConn);
	m_poQuery = new TOCIQuery(Environment::getDBConn());
	if(!m_poQuery) {
		printf("new TOCIQuery error! \n");
		exit(-1);
	}

	m_poTimesTenCMD = NULL;
	connectTT();
	
	sprintf(m_sSQL, "insert into STAT_INTERFACE (STAT_ID, KPI_CODE, VALUE, STAT_BEGIN_DATE, STAT_END_DATE, STATE, AREA_CODE, CREATE_DATE, DEAL_FLAG, DEAL_DATE, MODULE_ID, DIMENSIONS, DIMENSION_FLAG, STAT_DATE_FLAG, DB_SOURCE_ID, MIN_STAT_DATE, MAX_STAT_DATE)"
						"values ("
						"SEQ_MNT_STAT_ID.Nextval,"    /* STAT_ID */
						":KPI_CODE," 
 						"1,"		/* VALUE */
 						"sysdate,"  /* STAT_BEGIN_DATE */
						"sysdate," /* STAT_END_DATE */
						"1,"	/* STATE */
						"'%s',"    /* AREA_CODE */
						"sysdate," /*CREATE_DATE*/
						"1," /*DEAL_FLAG*/
						"sysdate," /*DEAL_DATE*/
						"0,"   /*MODULE_ID*/
						":DIMENSIONS,"   /*DIMENSIONS*/
						"1,"   /*DIMENSION_FLAG*/
						"2,"  /*STAT_DATE_FLAG*/ 
						"1,"  /*DB_SOURCE_ID*/
						"sysdate,"  /*MIN_STAT_DATE*/
						"sysdate)",  /*MAX_STAT_DATE*/
						"025");		
}

//从核心参数获取
void HssNmpInfoCollect::collectOidInfo()
{	
	//NeId
	m_poCmdCom->readIniString("INFO","hss.ne_id",HSS_SC_pairs[SCALAR_NEID].attr_value,NULL);

	//SrId ok
	strcpy(HSS_SC_pairs[SCALAR_SRID].attr_value,"1");

	//NeName 网元名称
	m_poCmdCom->readIniString("SYSTEM","host_server_name",HSS_SC_pairs[SCALAR_NENAME].attr_value,NULL);

	//DeployAddr
	m_poCmdCom->readIniString("SYSTEM","location",HSS_SC_pairs[SCALAR_DEPLOYADDR].attr_value,NULL);

	//EvendorName
	m_poCmdCom->readIniString("INFO","server_provider",HSS_SC_pairs[SCALAR_EVENDORNAME].attr_value,NULL);

	//NeVersion
	m_poCmdCom->readIniString("INFO","version",HSS_SC_pairs[SCALAR_NEVERSION].attr_value,NULL);

	//NodeId
	m_poCmdCom->readIniString("SYSTEM","node_flag",HSS_SC_pairs[SCALAR_NODEID].attr_value,NULL);

	//max_serv_num
	m_poCmdCom->readIniString("BUSINESS","max_serv_num",HSS_SC_pairs[SCALAR_MAX_SERV_NUM].attr_value,NULL);

	//max_product_offer_inst_num	
	m_poCmdCom->readIniString("BUSINESS","max_product_offer_inst_num",HSS_SC_pairs[SCALAR_MAX_PRODUCT_OFFER_INST_NUM].attr_value,NULL);
	
	//log_data_size
	m_poCmdCom->readIniString("MEMORY","log_data_size",HSS_SC_pairs[SCALAR_LOG_DATA_SIZE].attr_value,NULL);
		
	//cpu_warning_value
	m_poCmdCom->readIniString("MONITOR","cpu_warning_value",HSS_SC_pairs[SCALAR_CPU_WARNING_VALUE].attr_value,NULL);
	
	//mem_warning_value
	m_poCmdCom->readIniString("MONITOR","mem_warning_value",HSS_SC_pairs[SCALAR_MEM_WARNING_VALUE].attr_value,NULL);

	//max_dcc_request_cnt
	m_poCmdCom->readIniString("MONITOR","max_dcc_request_cnt",HSS_SC_pairs[SCALAR_MAX_DCC_REQUEST_CNT].attr_value,NULL);
	
	// archive_path
	m_poCmdCom->readIniString("LOG","log_archive_path",HSS_SC_pairs[SCALAR_ARCHIVE_PATH].attr_value,NULL);
	//size
	m_poCmdCom->readIniString("LOG","log_size",HSS_SC_pairs[SCALAR_SIZE].attr_value,NULL);

	//check_process_time 
	m_poCmdCom->readIniString("MONITOR","check_process_time",HSS_SC_pairs[SCALAR_CHECK_PROCESS_TIME].attr_value,NULL);
	
	//param_path
	m_poCmdCom->readIniString("SYSTEM","param_path",HSS_SC_pairs[SCALAR_PARAM_PATH].attr_value,NULL);
	
	//HSS_bsn_state
	m_poCmdCom->readIniString("SYSTEM","net_state",HSS_SC_pairs[SCALAR_HSS_BSN_STATE].attr_value,NULL);
	if(strcmp(HSS_SC_pairs[SCALAR_HSS_BSN_STATE].attr_value,"innet") == 0) {
		strcpy(HSS_SC_pairs[SCALAR_HSS_BSN_STATE].attr_value,"0");
	} else if(strcmp(HSS_SC_pairs[SCALAR_HSS_BSN_STATE].attr_value,"outnet") == 0) {
		strcpy(HSS_SC_pairs[SCALAR_HSS_BSN_STATE].attr_value,"1");
	}
	

	//HSS_ne_state	
	char sStrTmp1[16] = {0};
	char sStrTmp2[16] = {0};
	m_poCmdCom->readIniString("SYSTEM","state",sStrTmp1,NULL);
	m_poCmdCom->readIniString("SYSTEM","run_state",sStrTmp2,NULL);
	if(strcmp(sStrTmp1,"maintenance") == 0) {
		strcpy(HSS_SC_pairs[SCALAR_HSS_NE_STATE].attr_value,"3");
	} else if((strcmp(sStrTmp1,"online") == 0) && (strcmp(sStrTmp2,"normal") == 0)) {
		strcpy(HSS_SC_pairs[SCALAR_HSS_NE_STATE].attr_value,"0");
	} else if((strcmp(sStrTmp1,"online") == 0) && (strcmp(sStrTmp2,"busy") == 0)) {
		strcpy(HSS_SC_pairs[SCALAR_HSS_NE_STATE].attr_value,"1");
	} else if((strcmp(sStrTmp1,"online") == 0) && (strcmp(sStrTmp2,"fault") == 0)) {
		strcpy(HSS_SC_pairs[SCALAR_HSS_NE_STATE].attr_value,"2");
	} else if((strcmp(sStrTmp1,"offline") == 0) && (strcmp(sStrTmp2,"normal") == 0)) {
		strcpy(HSS_SC_pairs[SCALAR_HSS_NE_STATE].attr_value,"4");
	} else if((strcmp(sStrTmp1,"offline") == 0) && (strcmp(sStrTmp2,"fault") == 0)) {
		strcpy(HSS_SC_pairs[SCALAR_HSS_NE_STATE].attr_value,"5");
	}	
	
	//HSS_sysUptime
	m_poCmdCom->readIniString("SYSTEM","state_date",HSS_SC_pairs[SCALAR_HSS_SYSUPTIME].attr_value,NULL);
	time_t t = stringTotime_t(HSS_SC_pairs[SCALAR_HSS_SYSUPTIME].attr_value);
	sprintf(HSS_SC_pairs[SCALAR_HSS_SYSUPTIME].attr_value,"%ld",t);
		
	//not_billing_msg_num
	static char *not_billing_msg_num = "select count(*) from HSS_SND_WORK_ORDER a, HSS_SERVICE_NODE_INFO b "
				"where a.state_date >= sysdate-NUMTODSINTERVAL(300,'SECOND') and a.node_id = b.node_id and "
				"b.node_type not in (1,4,7) and a.state != '10P'";
	m_poTimesTenCMD->Prepare(not_billing_msg_num);
	m_poTimesTenCMD->Commit();	
	m_poTimesTenCMD->Execute();
	if(!m_poTimesTenCMD->FetchNext()){
		int iCount = 0;
		m_poTimesTenCMD->getColumn(1,&iCount);
		sprintf(HSS_SC_pairs[SCALAR_NOT_BILLING_MSG_NUM].attr_value,"%d",int(iCount/5.0+0.5));
	}
	m_poTimesTenCMD->Close();
	
	//hq_hss_msg_num
	static char *hq_hss_msg_num = "select count(*) from HSS_SND_WORK_ORDER a, HSS_SERVICE_NODE_INFO b "
				"where a.state_date >= sysdate-NUMTODSINTERVAL(300,'SECOND') and a.node_id = b.node_id and "
				"b.node_type = 4 and a.state != '10P'";
	m_poTimesTenCMD->Prepare(hq_hss_msg_num);
	m_poTimesTenCMD->Commit();	
	m_poTimesTenCMD->Execute();
	if(!m_poTimesTenCMD->FetchNext()){
		int iCount = 0;
		m_poTimesTenCMD->getColumn(1,&iCount);
		sprintf(HSS_SC_pairs[SCALAR_HQ_HSS_MSG_NUM].attr_value,"%d",int(iCount/5.0+0.5));
	}
	m_poTimesTenCMD->Close();	
	
	//ofcs_msg_num
	static char *ofcs_msg_num = "select count(*) from HSS_SND_WORK_ORDER a, HSS_SERVICE_NODE_INFO b "
				"where a.state_date >= sysdate-NUMTODSINTERVAL(300,'SECOND') and a.node_id = b.node_id and "
				"b.node_type = 1 and a.state != '10P'";
	m_poTimesTenCMD->Prepare(ofcs_msg_num);
	m_poTimesTenCMD->Commit();	
	m_poTimesTenCMD->Execute();
	if(!m_poTimesTenCMD->FetchNext()){
		int iCount = 0;
		m_poTimesTenCMD->getColumn(1,&iCount);
		sprintf(HSS_SC_pairs[SCALAR_OFCS_MSG_NUM].attr_value,"%d",int(iCount/5.0+0.5));
	}
	m_poTimesTenCMD->Close();	

	//ocs_msg_num
	static char *ocs_msg_num = "select count(*) from HSS_SND_WORK_ORDER a, HSS_SERVICE_NODE_INFO b "
				"where a.state_date >= sysdate-NUMTODSINTERVAL(300,'SECOND') and a.node_id = b.node_id and "
				"b.node_type = 7 and a.state != '10P'";
	m_poTimesTenCMD->Prepare(ocs_msg_num);
	m_poTimesTenCMD->Commit();	
	m_poTimesTenCMD->Execute();
	if(!m_poTimesTenCMD->FetchNext()){
		int iCount = 0;
		m_poTimesTenCMD->getColumn(1,&iCount);
		sprintf(HSS_SC_pairs[SCALAR_OCS_MSG_NUM].attr_value,"%d",int(iCount/5.0+0.5));
	}
	m_poTimesTenCMD->Close();
	
	//vc_msg_num
	static char *vc_msg_num = "select count(*) from HSS_SND_WORK_ORDER a, HSS_SERVICE_NODE_INFO b "
				"where a.state_date >= sysdate-NUMTODSINTERVAL(300,'SECOND') and a.node_id = b.node_id and "
				"b.node_type = 2 and a.state != '10P'";
	m_poTimesTenCMD->Prepare(vc_msg_num);
	m_poTimesTenCMD->Commit();	
	m_poTimesTenCMD->Execute();
	if(!m_poTimesTenCMD->FetchNext()){
		int iCount = 0;
		m_poTimesTenCMD->getColumn(1,&iCount);
		sprintf(HSS_SC_pairs[SCALAR_VC_MSG_NUM].attr_value,"%d",int(iCount/5.0+0.5));
	}
	m_poTimesTenCMD->Close();	

	//update_time
	t = time((time_t*)NULL);
	sprintf(HSS_SC_pairs[SCALAR_UPDATE_TIME].attr_value,"%ld",t);
	
	//ofcs_stock_count
	static char *sSQL1 = "select count(*) from HSS_SND_WORK_ORDER a,HSS_SERVICE_NODE_INFO b " 
						 "where a.state = '10P' and a.node_id = b.node_id and b.node_type = 1";
	m_poTimesTenCMD->Prepare(sSQL1);
    m_poTimesTenCMD->Commit();	
   	m_poTimesTenCMD->Execute();
	if(!m_poTimesTenCMD->FetchNext()){
		int iCount = 0;
		m_poTimesTenCMD->getColumn(1,&iCount);
		sprintf(HSS_SC_pairs[SCALAR_OFCS_STOCK_COUNT].attr_value,"%d",iCount);
	}
	m_poTimesTenCMD->Close();

	//Trap  侧需要解析这个值
	strcat(HSS_SC_pairs[SCALAR_OFCS_STOCK_COUNT].attr_value,",");
	char sStr[16] = {0};
	if(m_poCmdCom->readIniString("MONITOR","ofcs_stock_count_alarm_value",sStr,NULL))
	{
		strcat(HSS_SC_pairs[SCALAR_OFCS_STOCK_COUNT].attr_value,sStr);
	} else  {
		strcat(HSS_SC_pairs[SCALAR_OFCS_STOCK_COUNT].attr_value,"10");
	}
	
	//ofcs_stock_sysUptime	
	/*static char *sSQL2 = "select a.snd_message_seq,TO_CHAR(a.state_date,'yyyymmddhh24miss') from HSS_SND_WORK_ORDER a,HSS_SERVICE_NODE_INFO b " 
                         "where a.state = '10P' and a.node_id = b.node_id and b.node_type = 1 and rownum < 2 "
                         "order by a.snd_message_seq desc"; */
    static char *sSQL2 = "select TO_CHAR(state_date,'yyyymmddhh24miss') from HSS_SND_WORK_ORDER where snd_message_seq = "
    					 "(select max(a.snd_message_seq) from HSS_SND_WORK_ORDER a,HSS_SERVICE_NODE_INFO b where a.state = '10P' and a.node_id = b.node_id and b.node_type = 1)";
	m_poTimesTenCMD->Prepare(sSQL2);
    m_poTimesTenCMD->Commit();	
   	m_poTimesTenCMD->Execute();
	if(!m_poTimesTenCMD->FetchNext()){
		m_poTimesTenCMD->getColumn(1,HSS_SC_pairs[SCALAR_OFCS_STOCK_SYSUPTIME].attr_value);
		t = stringTotime_t(HSS_SC_pairs[SCALAR_OFCS_STOCK_SYSUPTIME].attr_value);
		sprintf(HSS_SC_pairs[SCALAR_OFCS_STOCK_SYSUPTIME].attr_value,"%ld",t);
	} 
	m_poTimesTenCMD->Close();
	
	//ocs_stock_count
	static char *sSQL3 = "select count(*) from HSS_SND_WORK_ORDER a,HSS_SERVICE_NODE_INFO b " 
						 "where a.state = '10P' and a.node_id = b.node_id and b.node_type = 7";
	m_poTimesTenCMD->Prepare(sSQL3);
    m_poTimesTenCMD->Commit();	
   	m_poTimesTenCMD->Execute();
	if(!m_poTimesTenCMD->FetchNext()){
		int iCount = 0;
		m_poTimesTenCMD->getColumn(1,&iCount);
		sprintf(HSS_SC_pairs[SCALAR_OCS_STOCK_COUNT].attr_value,"%d",iCount);
	}
	m_poTimesTenCMD->Close();

	//Trap  侧需要解析这个值
	strcat(HSS_SC_pairs[SCALAR_OCS_STOCK_COUNT].attr_value,",");
	if(m_poCmdCom->readIniString("MONITOR","ocs_stock_count_alarm_value",sStr,NULL))
	{
		strcat(HSS_SC_pairs[SCALAR_OCS_STOCK_COUNT].attr_value,sStr);
	} else  {
		strcat(HSS_SC_pairs[SCALAR_OCS_STOCK_COUNT].attr_value,"10");
	}

		
	//ocs_stock_sysUptime
	/*static char *sSQL4 = "select a.snd_message_seq,TO_CHAR(a.state_date,'yyyymmddhh24miss') from HSS_SND_WORK_ORDER a,HSS_SERVICE_NODE_INFO b " 
                         "where a.state = '10P' and a.node_id = b.node_id and b.node_type = 7 and rownum < 2 "
                         "order by a.snd_message_seq desc ";*/
    static char *sSQL4 = "select TO_CHAR(state_date,'yyyymmddhh24miss') from HSS_SND_WORK_ORDER where snd_message_seq = "
    					 "(select max(a.snd_message_seq) from HSS_SND_WORK_ORDER a,HSS_SERVICE_NODE_INFO b where a.state = '10P' and a.node_id = b.node_id and b.node_type = 7)";	
	m_poTimesTenCMD->Prepare(sSQL4);
    m_poTimesTenCMD->Commit();	
   	m_poTimesTenCMD->Execute();
	if(!m_poTimesTenCMD->FetchNext()){
		m_poTimesTenCMD->getColumn(1,HSS_SC_pairs[SCALAR_OCS_STOCK_SYSUPTIME].attr_value);
		t = stringTotime_t(HSS_SC_pairs[SCALAR_OCS_STOCK_SYSUPTIME].attr_value);
		sprintf(HSS_SC_pairs[SCALAR_OCS_STOCK_SYSUPTIME].attr_value,"%ld",t);
	}
	m_poTimesTenCMD->Close();
	
	//vc_stock_count
	static char *sSQL5 = "select count(*) from HSS_SND_WORK_ORDER a,HSS_SERVICE_NODE_INFO b " 
						 "where a.state = '10P' and a.node_id = b.node_id and b.node_type = 2";
	m_poTimesTenCMD->Prepare(sSQL5);
    m_poTimesTenCMD->Commit();	
   	m_poTimesTenCMD->Execute();
	if(!m_poTimesTenCMD->FetchNext()){
		int iCount = 0;
		m_poTimesTenCMD->getColumn(1, &iCount);
		sprintf(HSS_SC_pairs[SCALAR_VC_STOCK_COUNT].attr_value,"%d",iCount);
	}
	m_poTimesTenCMD->Close();

	//Trap  侧需要解析这个值
	strcat(HSS_SC_pairs[SCALAR_VC_STOCK_COUNT].attr_value,",");
	if(m_poCmdCom->readIniString("MONITOR","vc_stock_count_alarm_value",sStr,NULL))
	{
		strcat(HSS_SC_pairs[SCALAR_VC_STOCK_COUNT].attr_value,sStr);
	} else  {
		strcat(HSS_SC_pairs[SCALAR_VC_STOCK_COUNT].attr_value,"10");
	}

	//vc_stock_sysUptime
	/*static char *sSQL6 = "select a.snd_message_seq,TO_CHAR(a.state_date,'yyyymmddhh24miss') from HSS_SND_WORK_ORDER a,HSS_SERVICE_NODE_INFO b " 
                         "where a.state = '10P' and a.node_id = b.node_id and b.node_type = 2 and rownum < 2 "
                         "order by a.snd_message_seq  desc";*/
    static char *sSQL6 = "select TO_CHAR(state_date,'yyyymmddhh24miss') from HSS_SND_WORK_ORDER where snd_message_seq = "
    					 "(select max(a.snd_message_seq) from HSS_SND_WORK_ORDER a,HSS_SERVICE_NODE_INFO b where a.state = '10P' and a.node_id = b.node_id and b.node_type = 2)";
	m_poTimesTenCMD->Prepare(sSQL6);
    m_poTimesTenCMD->Commit();	
   	m_poTimesTenCMD->Execute();
	if(!m_poTimesTenCMD->FetchNext()){
		m_poTimesTenCMD->getColumn(1,HSS_SC_pairs[SCALAR_VC_STOCK_SYSUPTIME].attr_value);
		t = stringTotime_t(HSS_SC_pairs[SCALAR_VC_STOCK_SYSUPTIME].attr_value);
		sprintf(HSS_SC_pairs[SCALAR_VC_STOCK_SYSUPTIME].attr_value,"%ld",t);
	}
	m_poTimesTenCMD->Close();


	//数据收集完提交到stat_interface 接口表	
	insertIntoStatInterface(HSS_SC_pairs,SCALAR_COUNT);
	/*
	m_poQuery->setSQL(m_sSQL);
	for (int i = 0; i < SCALAR_COUNT; ++i)
	{
		//如果两次值不一样，发送数据接口
		if( strcmp(HSS_SC_pairs[i].attr_value,HSS_SC_pairs[i].last_attr_value) )
		{
			m_poQuery->setParameter("KPI_CODE", HSS_SC_pairs[i].oid_value);
			m_poQuery->setParameter("DIMENSIONS", HSS_SC_pairs[i].attr_value);
			m_poQuery->execute();
			
			
			printf("【 mib_id = %s 】 变更: 新值为: %s, 上次值为: %s \n",
				   HSS_SC_pairs[i].oid_value,HSS_SC_pairs[i].attr_value,HSS_SC_pairs[i].last_attr_value);

			strcpy(HSS_SC_pairs[i].last_attr_value,HSS_SC_pairs[i].attr_value);

		}
	}
	m_poQuery->commit();
	m_poQuery->close();
	*/
}

int  HssNmpInfoCollect::parserString(const char *pStr, vector<string> &sVec)
{
	char sStr[1024];
	strcpy(sStr,pStr);

	char *p = strtok(sStr,",");
	while(p) {
		sVec.push_back(string(p));
		p = strtok(NULL,",");
	}

	return sVec.size();
}
int HssNmpInfoCollect::getNeId(const char *sNeName)
{
	if(!strcasecmp(sNeName,"ocs")) {
		return 3003;
	} else if(!strcasecmp(sNeName,"hss")) {
		return 3012;
	} else if(!strcasecmp(sNeName,"ofcs") || !strcasecmp(sNeName,"hb")) {
		return 3002;
	} else if(!strcasecmp(sNeName,"crm")) {
		return 3004;
	} else if(!strcasecmp(sNeName,"hss001")) {
		return 3061;
	} else if(!strcasecmp(sNeName,"vc")) {
		return 3007;
	}

	return -1;
}

int HssNmpInfoCollect::connectTT()
{
	TimesTenConn* pTTConn = NULL;
	char sConnTns[8] = "hss";
	ABMException oExp;
	
	try {
    	if (GET_CURSOR(m_poTimesTenCMD, pTTConn, sConnTns, oExp) != 0) {
         	ADD_EXCEPT0(oExp, "connec to TT failed!");
            	return -1;
           }
        }
        catch(TTStatus st) {
              ADD_EXCEPT1(oExp, "connec to TT failed", st.err_msg);
            return -1;
        }
	return 0;
}

void HssNmpInfoCollect::insertIntoStatInterface(struct scalar_pair *p, int iCount)
{
	try {
		m_poQuery->setSQL(m_sSQL);

		for (int i = 0; i < iCount; i++)
		{
			if(strcmp(p[i].attr_value,p[i].last_attr_value))
			{

				m_poQuery->setParameter("KPI_CODE", p[i].oid_value);
				m_poQuery->setParameter("DIMENSIONS", p[i].attr_value);
				m_poQuery->execute();
				m_poQuery->commit();
				
				printf("【 mib_id = %s 】 变更: 新值为: %s, 上次值为: %s \n",
						 p[i].oid_value,p[i].attr_value,p[i].last_attr_value);
			
				strcpy(p[i].last_attr_value,p[i].attr_value);
			}
		}
	
		m_poQuery->close();
	}
	catch(TOCIException & e)
	{
		printf("catch TOCIException：\nSQL->%s\nERROR->%s",e.getErrSrc(),e.getErrMsg());
	}
}


//3.1.3.2.1 稽核类参数ok
void HssNmpInfoCollect::collect_audit_classTable()
{
	char sStr[1024] = {0};
	char sStrTmp[1024] = {0};		
	if(!m_poCmdCom->readIniString("BUSINESS","hss.nelist",sStr,NULL))
	{
		printf("核心参数没有配置[BUSINESS].[hss.nelist]:\n");
		return;
	}

	vector<string> sVec;
	int iCount = parserString(sStr,sVec);

	if(!iCount) return;

	for(int i = 0; i < iCount; ++i) 
	{
		int iNeId = getNeId(sVec[i].c_str());
		sprintf(audit_classTable[i].attr_value,"%d;%d;%s;",i+1,iNeId,sVec[i].c_str());
		
		sprintf(sStr,"%s.inc_audit_time",sVec[i].c_str());
		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("BUSINESS",sStr,sStrTmp,NULL);
		//时间格式转换:例如将0800  转换成08:00
		strncat(audit_classTable[i].attr_value,sStrTmp,2);
		strcat(audit_classTable[i].attr_value,":");
		strncat(audit_classTable[i].attr_value,sStrTmp+2,2);
		strcat(audit_classTable[i].attr_value,";");

		sprintf(sStr,"%s.audit_path",sVec[i].c_str());
		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("BUSINESS",sStr,sStrTmp,NULL);
		strcat(audit_classTable[i].attr_value,sStrTmp);
		strcat(audit_classTable[i].attr_value,";");

		sprintf(sStr,"%s.audit_result_path",sVec[i].c_str());
		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("BUSINESS",sStr,sStrTmp,NULL);
		strcat(audit_classTable[i].attr_value,sStrTmp);
		strcat(audit_classTable[i].attr_value,";");

		sprintf(sStr,"%s.audit_file_clear",sVec[i].c_str());
		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("BUSINESS",sStr,sStrTmp,NULL);
		strcat(audit_classTable[i].attr_value,sStrTmp);		
	}

	insertIntoStatInterface(audit_classTable, iCount);
}

//3.1.3.2.2 业务处理参数   :   超时和重发次数列表ok
void HssNmpInfoCollect::collect_time_out_numTable()
{
	char sStr[1024] = {0};
	char sStrTmp[1024] = {0};		
	if(!m_poCmdCom->readIniString("BUSINESS","hss.nelist",sStr,NULL))
	{
		printf("核心参数没有配置[BUSINESS].[hss.nelist]:\n");
		return;
	}

	vector<string> sVec;
	int iCount = parserString(sStr,sVec);

	if(!iCount) return;

	for(int i = 0; i < iCount; ++i) 
	{
	
		int iNeId = getNeId(sVec[i].c_str());
		sprintf(time_out_numTable[i].attr_value,"%d;%d;%s;",i+1,iNeId,sVec[i].c_str());
		
		sprintf(sStr,"%s.time_out",sVec[i].c_str());
		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("BUSINESS",sStr,sStrTmp,NULL);
		strcat(time_out_numTable[i].attr_value,sStrTmp);
		strcat(time_out_numTable[i].attr_value,";");	

		sprintf(sStr,"%s.repetition_times",sVec[i].c_str());
		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("BUSINESS",sStr,sStrTmp,NULL);
		strcat(time_out_numTable[i].attr_value,sStrTmp);	
	}

	insertIntoStatInterface(time_out_numTable, iCount);
}


//3.1.3.3		 内存类参数:  数据对象共享内存配置
void HssNmpInfoCollect::collect_data_objTable()
{	
	char sStr[1024] = {0};
	char sStrTmp[1024] = {0};
	
	vector<string> sVec;
	sVec.push_back("param");
		
	//0号进程共享内存
	//index && obj_name 
	int i = 0;
	sprintf(data_objTable[i].attr_value,"1;");

	sprintf(sStr,"%s.obj_name",sVec[0].c_str());
	sStrTmp[0] = '\0';
	m_poCmdCom->readIniString("MEMORY",sStr,sStrTmp,"");
	strcat(data_objTable[i].attr_value,sStrTmp);
	strcat(data_objTable[i].attr_value,";");	
	
	sprintf(sStr,"%s.max_shm_size",sVec[0].c_str());
	sStrTmp[0] = '\0';
	m_poCmdCom->readIniString("MEMORY",sStr,sStrTmp,"");
	strcat(data_objTable[i].attr_value,sStrTmp);
	strcat(data_objTable[i].attr_value,";");	

	sprintf(sStr,"%s.ipc_key_value",sVec[0].c_str());
	sStrTmp[0] = '\0';
	m_poCmdCom->readIniString("MEMORY",sStr,sStrTmp,"");
	strcat(data_objTable[i].attr_value,sStrTmp);	
	strcat(data_objTable[i].attr_value,";");	
		
	sprintf(sStr,"%s.ipc_key_name",sVec[0].c_str());
	sStrTmp[0] = '\0';
	m_poCmdCom->readIniString("MEMORY",sStr,sStrTmp,"");
	strcat(data_objTable[i].attr_value,sStrTmp);	
	strcat(data_objTable[i].attr_value,";");

	sprintf(sStr,"%s.init_data_size",sVec[0].c_str());
	sStrTmp[0] = '\0';
	m_poCmdCom->readIniString("MEMORY",sStr,sStrTmp,"");
	strcat(data_objTable[i].attr_value,sStrTmp);	
	strcat(data_objTable[i].attr_value,";");			

	sprintf(sStr,"%s.ext_hwm",sVec[0].c_str());
	sStrTmp[0] = '\0';
	m_poCmdCom->readIniString("MEMORY",sStr,sStrTmp,"");
	strcat(data_objTable[i].attr_value,sStrTmp);	
	strcat(data_objTable[i].attr_value,";");	

	sprintf(sStr,"%s.ext_data_size",sVec[0].c_str());
	sStrTmp[0] = '\0';
	m_poCmdCom->readIniString("MEMORY",sStr,sStrTmp,"");
	strcat(data_objTable[i].attr_value,sStrTmp);


	//日志组共享内存
	sVec.clear();
	sVec.push_back("loggroupdata_one");
	sVec.push_back("loggroupdata_two");
	sVec.push_back("loggroupdata_thr");

	for(int j = 0; j < 3; ++j) 
	{	
		++i;

		sprintf(data_objTable[i].attr_value,"%d;",j+2);

		//obj_name
		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("MEMORY","loggroupdata.obj_name",sStrTmp,"");
		strcat(data_objTable[i].attr_value,sStrTmp);
		strcat(data_objTable[i].attr_value,";");	

		//max_shm_size
		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("MEMORY","loggroupdata.max_shm_size",sStrTmp,"");
		strcat(data_objTable[i].attr_value,sStrTmp);
		strcat(data_objTable[i].attr_value,";");		


		sprintf(sStr,"%s.ipc_key_value",sVec[j].c_str());
		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("MEMORY",sStr,sStrTmp,"");
		strcat(data_objTable[i].attr_value,sStrTmp);	
		strcat(data_objTable[i].attr_value,";");	
		
		sprintf(sStr,"%s.ipc_key_name",sVec[j].c_str());
		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("MEMORY",sStr,sStrTmp,"");
		strcat(data_objTable[i].attr_value,sStrTmp);	
		strcat(data_objTable[i].attr_value,";");

		//init_data_size
		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("MEMORY","log_data_size",sStrTmp,"");
		strcat(data_objTable[i].attr_value,sStrTmp);
		strcat(data_objTable[i].attr_value,";");			

		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("MEMORY","loggroupdata.ext_hwm",sStrTmp,"");
		strcat(data_objTable[i].attr_value,sStrTmp);	
		strcat(data_objTable[i].attr_value,";");	

		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("MEMORY","loggroupdata.ext_data_size",sStrTmp,"");
		strcat(data_objTable[i].attr_value,sStrTmp);
		
	}

	++i;
	insertIntoStatInterface(data_objTable, i);	
}

//3.1.3.4	资源类参数:  网元资源配置列表ok
void HssNmpInfoCollect::collect_ne_resourceTable()
{
	char sStr[1024] = {0};
	char sStrTmp[1024] = {0};		
	if(!m_poCmdCom->readIniString("SYNC","hss.nelist",sStr,NULL))
	{
		printf("核心参数没有配置[SYNC].[hss.nelist]:\n");
		return;
	}

	vector<string> sVec;
	int iCount = parserString(sStr,sVec);

	if(!iCount) return;

	for(int i = 0; i < iCount; ++i) 
	{
		int iNeId = getNeId(sVec[i].c_str());
		sprintf(ne_resourceTable[i].attr_value,"%d;%d;%s;",i+1,iNeId,sVec[i].c_str());
		
		sprintf(sStr,"%s.sync_path",sVec[i].c_str());
		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("SYNC",sStr,sStrTmp,"");
		strcat(ne_resourceTable[i].attr_value,sStrTmp);
		strcat(ne_resourceTable[i].attr_value,";");	


		sprintf(sStr,"%s.sync_backup_path",sVec[i].c_str());
		sStrTmp[0] = '\0';
		m_poCmdCom->readIniString("SYNC",sStr,sStrTmp,"");
		strcat(ne_resourceTable[i].attr_value,sStrTmp);	
	}

	insertIntoStatInterface(ne_resourceTable, iCount);		
}


//3.1.3.4	资源类参数:  主机资源配置列表ok
void HssNmpInfoCollect::collect_host_resourceTable()
{
	char sStr[1024] = {0};
	sprintf(host_resourceTable[0].attr_value,"1;");

	//host_ip
	if(m_poCmdCom->readIniString("SYSTEM","ip",sStr,NULL))
	{
		strcat(host_resourceTable[0].attr_value,sStr);
	} else {
		strcat(host_resourceTable[0].attr_value,"-1");
	}
	strcat(host_resourceTable[0].attr_value,";");

	//host_name
	sStr[0] = '\0';
	if(gethostname(sStr,1024) == 0)
	{
		strcat(host_resourceTable[0].attr_value,sStr);
	} else {
		strcat(host_resourceTable[0].attr_value,"-1");
	}
	strcat(host_resourceTable[0].attr_value,";");

	//max_process_num
	if(m_poCmdCom->readIniString("SCHEDULE","system_max_process_num",sStr,NULL))
	{
		strcat(host_resourceTable[0].attr_value,sStr);
	} 
	
	insertIntoStatInterface(host_resourceTable, 1);	
}

//3.1.3.4	资源类参数:  模块资源配置列表ok
void HssNmpInfoCollect::collect_module_resourceTable()
{
	char sStr[1024] = {0};
	char sStrTmp[1024] = {0};		
	if(!m_poCmdCom->readIniString("SCHEDULE","module_list",sStr,NULL))
	{
		printf("核心参数没有配置[SCHEDULE].[module_list]:\n");
		return;
	}

	vector<string> sVec;
	int iCount = parserString(sStr,sVec);

	if(!iCount) return;

	for(int i = 0; i < iCount; ++i)
	{
		//index && module_id
		sprintf(module_resourceTable[i].attr_value,"%d;%s000;",i+1,sVec[i].c_str());

		//module_name
		sprintf(sStrTmp,"vp%s_module_name",sVec[i]);
		sStr[0] = '\0';
		m_poCmdCom->readIniString("SCHEDULE",sStrTmp,sStr,NULL);
		strcat(module_resourceTable[i].attr_value,sStr);
		strcat(module_resourceTable[i].attr_value,";");

		//process_high_level
		sprintf(sStrTmp,"vp%s_process_high_level",sVec[i]);
		sStr[0] = '\0';
		m_poCmdCom->readIniString("SCHEDULE",sStrTmp,sStr,NULL);
		strcat(module_resourceTable[i].attr_value,sStr);
		strcat(module_resourceTable[i].attr_value,";");		
		
		//process_low_level
		sprintf(sStrTmp,"vp%s_process_low_level",sVec[i]);
		sStr[0] = '\0';
		m_poCmdCom->readIniString("SCHEDULE",sStrTmp,sStr,NULL);
		strcat(module_resourceTable[i].attr_value,sStr);
		strcat(module_resourceTable[i].attr_value,";");	

		//max_process
		sprintf(sStrTmp,"vp%s_max_process",sVec[i]);
		sStr[0] = '\0';
		m_poCmdCom->readIniString("SCHEDULE",sStrTmp,sStr,NULL);
		strcat(module_resourceTable[i].attr_value,sStr);
		strcat(module_resourceTable[i].attr_value,";");  

		//min_process
		sprintf(sStrTmp,"vp%s_min_process",sVec[i]);
		sStr[0] = '\0';
		m_poCmdCom->readIniString("SCHEDULE",sStrTmp,sStr,NULL);
		strcat(module_resourceTable[i].attr_value,sStr);
		strcat(module_resourceTable[i].attr_value,";"); 

		//queue_size
		sprintf(sStrTmp,"vp%s_queue_size",sVec[i]);
		sStr[0] = '\0';
		m_poCmdCom->readIniString("SCHEDULE",sStrTmp,sStr,NULL);
		strcat(module_resourceTable[i].attr_value,sStr);

	}

	insertIntoStatInterface(module_resourceTable,iCount);
	
}


//3.1.3.5.1 数据库资源相关参数ok
void HssNmpInfoCollect::collect_databaseTable()
{
	char sStr[1024] = {0};
	char sStrTmp[1024] = {0};

	//char sDBStr[][1024] = {"bill","hss"};
	char sDBStr[][1024] = {"hss"};
	int iCount = sizeof(sDBStr)/1024;

	for (int i = 0; i < iCount; ++i)
	{
		//index
		sprintf(databaseTable[i].attr_value,"%d;",i+1);
		
		//dbbase_type
		strcpy(sStr,"1");  //default:  oracle
		sprintf(sStrTmp,"%s.dbbase_type",sDBStr[i]);
		m_poCmdCom->readIniString("HSSDB",sStrTmp,sStr,"");
		strcat(databaseTable[i].attr_value,sStr);
		strcat(databaseTable[i].attr_value,";");

		//db_server_name
		sStr[0] = '\0';
		sprintf(sStrTmp,"%s.db_server_name",sDBStr[i]);
		m_poCmdCom->readIniString("HSSDB",sStrTmp,sStr,"");
		strcat(databaseTable[i].attr_value,sStr);	
		strcat(databaseTable[i].attr_value,";");
		
		//password
		sStr[0] = '\0';
		sprintf(sStrTmp,"%s.password",sDBStr[i]);
		m_poCmdCom->readIniString("HSSDB",sStrTmp,sStr,"");
		strcat(databaseTable[i].attr_value,sStr);	
		strcat(databaseTable[i].attr_value,";");

		//username
		sStr[0] = '\0';
		sprintf(sStrTmp,"%s.username",sDBStr[i]);
		m_poCmdCom->readIniString("HSSDB",sStrTmp,sStr,"");
		strcat(databaseTable[i].attr_value,sStr);	

	}

	insertIntoStatInterface(databaseTable,iCount);
	
}


//3.1.3.5.2 主机资源相关参数ok
void HssNmpInfoCollect::collect_hss_hostTable()
{
	char sStr[1024] = {0};
	sprintf(hss_hostTable[0].attr_value,"1;");

	//host_server_name
	if(!m_poCmdCom->readIniString("SYSTEM","host_server_name",sStr,NULL))
	{
		strcat(hss_hostTable[0].attr_value,sStr);
	} else {
		strcat(hss_hostTable[0].attr_value,"hss");
	}
	strcat(hss_hostTable[0].attr_value,";");

	//ip
	if(m_poCmdCom->readIniString("SYSTEM","ip",sStr,NULL))
	{
		strcat(hss_hostTable[0].attr_value,sStr);
	} 

	strcat(hss_hostTable[0].attr_value,";");

	//port
	if(m_poCmdCom->readIniString("SYSTEM","port",sStr,NULL))
	{
		strcat(hss_hostTable[0].attr_value,sStr);
	} 
	
	insertIntoStatInterface(hss_hostTable, 1);
	
}


//3.1.3.5.3 连接HSS网元的相关参数ok
void HssNmpInfoCollect::collect_hss_ne_Table()
{
	char sStr[1024] = {0};

	strcpy(hss_ne_Table[0].attr_value,"1;3009;sr;");

	if(m_poCmdCom->readIniString("NET","sr.ip",sStr,NULL))
	{
		strcat(hss_ne_Table[0].attr_value,sStr);
	} 
	strcat(hss_ne_Table[0].attr_value,";");

	sStr[0] = '\0';
	if(m_poCmdCom->readIniString("NET","sr.port",sStr,NULL))
	{
		strcat(hss_ne_Table[0].attr_value,sStr);
	} 

	insertIntoStatInterface(hss_ne_Table, 1);
}


//3.1.3.5.4 日志相关参数:  模块日志配置ok
void HssNmpInfoCollect::collect_module_log_confTable()
{
	char sStr[1024] = {0};
	char sStrTmp[24] = {0};
	
	int iLogLevel = m_poCmdCom->readIniInteger("LOG","log_level",0);

	int iCount = m_poCmdCom->readIniInteger("LOG","group_num",0);

	int j = 0;
	for (int i = 1; i <= iCount; ++i)
	{
		sprintf(module_log_confTable[j].attr_value,"%d;-1;-1;%d;",j+1,iLogLevel);
		sprintf(sStrTmp,"log(%d)(1).path",i);
		sStr[0] = '\0';
		m_poCmdCom->readIniString("LOG",sStrTmp,sStr,NULL);
		strcat(module_log_confTable[j].attr_value,sStr);
		++j;
		
		sprintf(module_log_confTable[j].attr_value,"%d;-1;-1;%d;",j+1,iLogLevel);
		sprintf(sStrTmp,"log(%d)(2).path",i);
		sStr[0] = '\0';
		m_poCmdCom->readIniString("LOG",sStrTmp,sStr,NULL);
		strcat(module_log_confTable[j].attr_value,sStr);	
		++j;
	}

	insertIntoStatInterface(module_log_confTable,j);
	
}


//从表中获取信息点数据
void HssNmpInfoCollect::collectTableInfo()
{
	collect_audit_classTable();
	collect_time_out_numTable();
	collect_data_objTable();
	collect_databaseTable();
	collect_ne_resourceTable();
	collect_host_resourceTable();
	collect_module_resourceTable();
	collect_hss_hostTable();
	collect_hss_ne_Table();
	collect_module_log_confTable();
}


HssNmpInfoCollect::~HssNmpInfoCollect()
{
	if(m_poQuery)
	{
		delete m_poQuery;
		m_poQuery = NULL;
	}

}


int main(int argc, char *argv[])
{
	if(argc < 3) {
		printf("Usage:\n\tHssNmpInfoCollect -t interval(seconds)\n");
		exit(-1);
	}

	HssNmpInfoCollect oCollect;

	int iInterval = atoi(argv[2]);

	static int i = 1;

	printf("**********开始提取HSS-NMP 信息点数据!**********\n");
	
	while (1) {

		printf("*******第 %d 次提取数据!************\n",i++);

		oCollect.collectOidInfo();
		
		oCollect.collectTableInfo();
		
		sleep(iInterval);
	}

	return 0;

}


