#ifndef _HSS_NMP_INFO_COLLECT_H_
#define _HSS_NMP_INFO_COLLECT_H_

#include "CommandCom.h"
#include "TOCIQuery.h"

#include "ABMException.h"
#include "TimesTenAccess.h"


//单点的oid
const char * NeId_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.1.1";
const char * SrId_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.1.2";
const char * NeName_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.1.3";
const char * DeployAddr_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.1.4";
const char * EVendorName_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.1.5";
const char * NeVersion_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.1.6";
const char * NodeId_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.1.7";
const char * max_serv_num_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.2.2.1";
const char * max_product_offer_inst_num_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.2.2.2";
const char * log_data_size_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.3.1";
const char * cpu_warning_value_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.4.1";
const char * mem_warning_value_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.4.2";
const char * max_dcc_request_cnt_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.4.3";
const char * archive_path_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.5.4.1";
const char * size_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.5.4.2";
const char * check_process_time_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.5.5.1";
const char * param_path_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.5.5.2";
const char * HSS_bsn_state_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.2.1.2";
const char * HSS_ne_state_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.2.1.3";
const char * HSS_sysUptime_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.2.1.4";
const char * not_billing_msg_num_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.2.2.1";
const char * hq_hss_msg_num_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.2.2.2";
const char * ofcs_msg_num_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.2.2.3";
const char * ocs_msg_num_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.2.2.4";
const char * vc_msg_num_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.2.2.5";
const char * update_time_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.2.2.6";
const char * ofcs_stock_count_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.3.1.2";
const char * ofcs_stock_sysUptime_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.3.1.3";
const char * ocs_stock_count_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.3.2.2";
const char * ocs_stock_sysUptime_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.3.2.3";
const char * vc_stock_count_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.3.3.2";
const char * vc_stock_sysUptime_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.3.3.3";

/* define scalar index */
#define SCALAR_NEID 0
#define SCALAR_SRID 1
#define SCALAR_NENAME 2
#define SCALAR_DEPLOYADDR 3
#define SCALAR_EVENDORNAME 4
#define SCALAR_NEVERSION 5
#define SCALAR_NODEID 6
#define SCALAR_MAX_SERV_NUM 7
#define SCALAR_MAX_PRODUCT_OFFER_INST_NUM 8
#define SCALAR_LOG_DATA_SIZE 9
#define SCALAR_CPU_WARNING_VALUE 10
#define SCALAR_MEM_WARNING_VALUE 11
#define SCALAR_MAX_DCC_REQUEST_CNT 12
#define SCALAR_ARCHIVE_PATH 13
#define SCALAR_SIZE 14
#define SCALAR_CHECK_PROCESS_TIME 15
#define SCALAR_PARAM_PATH 16
#define SCALAR_HSS_BSN_STATE 17
#define SCALAR_HSS_NE_STATE 18
#define SCALAR_HSS_SYSUPTIME 19
#define SCALAR_NOT_BILLING_MSG_NUM 20
#define SCALAR_HQ_HSS_MSG_NUM 21
#define SCALAR_OFCS_MSG_NUM 22
#define SCALAR_OCS_MSG_NUM 23
#define SCALAR_VC_MSG_NUM 24
#define SCALAR_UPDATE_TIME 25
#define SCALAR_OFCS_STOCK_COUNT 26
#define SCALAR_OFCS_STOCK_SYSUPTIME 27
#define SCALAR_OCS_STOCK_COUNT 28
#define SCALAR_OCS_STOCK_SYSUPTIME 29
#define SCALAR_VC_STOCK_COUNT 30
#define SCALAR_VC_STOCK_SYSUPTIME 31

#define SCALAR_COUNT 32


struct scalar_pair{
	char oid_value[1024];	
	char attr_value[1024];
	char last_attr_value[1024];
};

struct scalar_pair HSS_SC_pairs[32];

//Table 型的oid
//3.1.3.2.1	稽核类参数
const char *audit_classTable_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.2.1.1";
#define AUDIT_CLASSTABLE_COUNT  10
#define COLUMNS_AUDIT_CLASSTABLE	   7
struct scalar_pair audit_classTable[AUDIT_CLASSTABLE_COUNT];


//3.1.3.2.2	业务处理参数   :   超时和重发次数列表
const char *time_out_numTable_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.2.2.3";
#define TIME_OUT_NUMTABLE_COUNT  10
#define COLUMNS_TIME_OUT_NUMTABLE		 5
struct scalar_pair time_out_numTable[TIME_OUT_NUMTABLE_COUNT];


//3.1.3.3	     内存类参数:  数据对象共享内存配置
const char *data_objTable_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.3.2";
#define DATA_OBJTABLE_COUNT  10
#define COLUMNS_DATA_OBJTABLE		 8
struct scalar_pair data_objTable[DATA_OBJTABLE_COUNT];


//3.1.3.4   资源类参数:  网元资源配置列表
const char *ne_resourceTable_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.4.4";
#define NE_RESOURCETABLE_COUNT 10
#define COLUMNS_NE_RESOURCETABLE	 5
struct scalar_pair ne_resourceTable[NE_RESOURCETABLE_COUNT];


//3.1.3.4   资源类参数:  主机资源配置列表
const char *host_resourceTable_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.4.5";
#define HOST_RESOURCETABLE_COUNT 10
#define COLUMNS_HOST_RESOURCETABLE		 4
struct scalar_pair host_resourceTable[HOST_RESOURCETABLE_COUNT];


//3.1.3.4   资源类参数:  模块资源配置列表
const char *module_resourceTable_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.4.6";
#define MODULE_RESOURCETABLE_COUNT 10
#define COLUMNS_MODULE_RESOURCETABLE	 8
struct scalar_pair module_resourceTable[MODULE_RESOURCETABLE_COUNT];


//3.1.3.5.1	数据库资源相关参数
const char *databaseTable_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.5.1.1";
#define DATABASETABLE_COUNT 10
#define COLUMNS_DATABASETABLE		 5
struct scalar_pair databaseTable[DATABASETABLE_COUNT];


//3.1.3.5.2	主机资源相关参数
const char *hss_hostTable_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.5.2.1";
#define HSS_HOSTTABLE_COUNT 10
#define COLUMNS_HSS_HOSTTABLE		 4
struct scalar_pair hss_hostTable[HSS_HOSTTABLE_COUNT];


//3.1.3.5.3	连接HSS网元的相关参数
const char *hss_ne_Table_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.5.3.1";
#define HSS_NE_TABLE_COUNT 10
#define COLUMNS_HSS_NE_TABLE	 5
struct scalar_pair hss_ne_Table[HSS_NE_TABLE_COUNT];


//3.1.3.5.4	日志相关参数:  模块日志配置
const char *module_log_confTable_oid = "1.3.6.1.4.1.81000.2.103.2.2.4.1.5.4.3";
#define MODULE_LOG_CONFTABLE_COUNT 10
#define COLUMNS_MODULE_LOG_CONFTABLE	 5
struct scalar_pair module_log_confTable[MODULE_LOG_CONFTABLE_COUNT];

class HssNmpInfoCollect
{
	public:
		HssNmpInfoCollect();
		
		~ HssNmpInfoCollect();
		
		void collectOidInfo();
		
		void collectTableInfo();

		//3.1.3.2.1	稽核类参数
		void collect_audit_classTable();

		//3.1.3.2.2	业务处理参数   :   超时和重发次数列表
		void collect_time_out_numTable();

		//3.1.3.3		 内存类参数:  数据对象共享内存配置
		void collect_data_objTable();

		//3.1.3.4   资源类参数:  网元资源配置列表
		void collect_ne_resourceTable();


		//3.1.3.4	资源类参数:  主机资源配置列表
		void collect_host_resourceTable();
		
		
		//3.1.3.4	资源类参数:  模块资源配置列表
		void collect_module_resourceTable();
		
		
		//3.1.3.5.1 数据库资源相关参数
		void collect_databaseTable();
		
		
		//3.1.3.5.2 主机资源相关参数
		void collect_hss_hostTable();
		
		
		//3.1.3.5.3 连接HSS网元的相关参数
		void collect_hss_ne_Table();
		
		
		//3.1.3.5.4 日志相关参数:  模块日志配置
		void collect_module_log_confTable();

		
		int parserString(const char *pStr, vector<string> &sVec);

		int connectTT();

		void insertIntoStatInterface(struct scalar_pair *p, int iCount);

		int getNeId(const char *sNeName);
		

	private:
		CommandCom *m_poCmdCom;
		char m_sSQL[1024];

		//stat_interface
		TOCIQuery *m_poQuery;

		TimesTenCMD *m_poTimesTenCMD;
};


#endif

