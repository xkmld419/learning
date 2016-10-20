#include <stdio.h>
#include <string.h>
//#include "../app_dataio/FileMgr.h"
#include "SHMCmdSet.h"
#include "SGWSHMParamApi.h"

enum TABLE_SIGN {context_filter=0,\
					tab_msg_map,\
					service_context_list,\
					stream_ctrl_info,\
					stream_ctrl_data,\
					userinfo,\
					sgw_sla_que_rel,\
					netInfodata,\
					sessiondata,\
					shmcongestlevel,\
					service_context_list_base,\
					loadbalancedata,\
					base_method,\
					service_package_route,\
					s_packet_session,\
					service_package_node,\
					service_package_route_ccr,\
					service_package_judge,\
					service_package_cca,\
					service_package_func,\
					service_package_variable,\
					statisticscongestmsg,\
					wf_process_mq,\
					portinfo, \
					session_clear, \
					sgw_org_route,\
					sgw_org_head,\
					channelInfo, \
					sgw_global_map,\
					sgw_user_staff, \
					sgw_route_rule, \
					sgw_area_org, \
					sgw_tux_rela_in,\
					sgw_tux_rela_out, \
					sgw_svr_list, \
					sgw_svrs_param_rela, \
					sgw_svrs_param_list, \
					sgw_svrs_register, \
					sgw_param_ins_list, \
					sgw_enum_area
					};

char TABLEPARAM[_SHM_FILE_NUMBER_][BUFF_MIN_SIZE] = {"context_filter",\
														"tab_msg_map",\
														"service_context_list",\
														"stream_ctrl_info",\
														"stream_ctrl_data",\
														"userinfo",\
														"sgw_sla_que_rel",\
														"netInfodata",\
														"sessiondata",\
														"shmcongestlevel",\
														"service_context_list_base",\
														"loadbalancedata",\
														"base_method",\
														"service_package_route",\
														"s_packet_session",\
														"service_package_node",\
														"service_package_route_ccr",\
														"service_package_judge",\
														"service_package_cca",\
														"service_package_func",\
														"service_package_variable",\
														"statisticscongestmsg",\
														"wf_process_mq",\
														"portinfo", \
														"session_clear", \
														"sgw_org_route",\
														"sgw_org_head",\
														"channelInfo", \
														"sgw_global_map",\
														"sgw_user_staff", \
														"sgw_route_rule", \
														"sgw_area_org", \
														"sgw_tux_rela_in", \
														"sgw_tux_rela_out", \
														"sgw_svr_list", \
														"sgw_svrs_param_rela", \
														"sgw_svrs_param_list", \
														"sgw_svrs_register", \
														"sgw_param_ins_list", \
														"sgw_enum_area"};

int checkargv(const char * param)
{
	int i = 0;
	int len = strlen(param);

	if (len == 0) {
		return -1;
	}

	while (TABLEPARAM[i]!=NULL&&i<_SHM_FILE_NUMBER_) {
		if (strncasecmp(TABLEPARAM[i],param,len) == 0) {
			return (i);
		}
		i++;
	}
	return -1;
}

int SHMCmdSet::checkParam()
{
	try {
		//共享内存全局变量包含告警阈值、可扩展次数、每次扩展的空间
		_SHM_PARAM_DATA_INITSIZE_   	  ;
		_SHM_PARAM_EXTENTS_SIZE_          ;
		_SHM_PARAM_EXTENTS_MAX_NUM_  	  ;
		_SHM_PARAM_EXTENTS_MAX_HWM_   	  ;
		_SHM_PARAM_EXTENTS_MAX_WARM_   	  ;
		_SHM_PARAM_DATA_MAXSIZE_          ;

		//局部变量（精确到各张文件的配置(包含告警阈值、可扩展次数、每次扩展的空间)）
		_SHM_CONTEXTDATA_DATA_KEY_;
		_SHM_CONTEXTDATA_INDEX_KEY_;
		_SHM_CONTEXTDATA_SPACE_INITSIZE_;
		_SHM_CONTEXTDATA_SPACE_MAXSIZE_;
		_SHM_CONTEXTDATA_EXTENTS_NUM_;
		_SHM_CONTEXTDATA_EXTENTS_MAX_NUM_;
		_SHM_CONTEXTDATA_EXTENTS_SIZE_;
		_SHM_CONTEXTDATA_INDEX_KEYNAME_(0);
		_SHM_CONTEXTDATA_DATA_KEYNAME_(0);
		_SHM_CONTEXTDATA_KEY_MAXSIZE_;
		_PV_CONTEXTDATA_KEY_;

		_SHM_MSGMAPINFO_DATA_KEY_;
		_SHM_MSGMAPINFO_EXTENTS_NUM_;
		_SHM_MSGMAPINFO_EXTENTS_MAX_NUM_;
		_SHM_MSGMAPINFO_EXTENTS_SIZE_;
		_SHM_MSGMAPINFO_INDEX_KEY_;
		_SHM_MSGMAPINFO_SPACE_INITSIZE_;
		_SHM_MSGMAPINFO_SPACE_MAXSIZE_;
		_SHM_MSGMAPINFO_DATA_KEYNAME_(0);
		_SHM_MSGMAPINFO_INDEX_KEYNAME_(0);
		_PV_MSGMAPINFO_KEY_;

		_SHM_SERVICETXTLIST_DATA_KEY_;
		_SHM_SERVICETXTLIST_INDEX_KEY_;
		_SHM_SERVICETXTLIST_INDEX_KEY_A_;
		_SHM_SERVICETXTLIST_SPACE_INITSIZE_;
		_SHM_SERVICETXTLIST_SPACE_MAXSIZE_;
		_SHM_SERVICETXTLIST_EXTENTS_NUM_;
		_SHM_SERVICETXTLIST_EXTENTS_MAX_NUM_;
		_SHM_SERVICETXTLIST_EXTENTS_SIZE_;
		_SHM_SERVICETXTLIST_DATA_KEYNAME_(0);
		_SHM_SERVICETXTLIST_INDEX_KEYNAME_(0);
		_SHM_SERVICETXTLIST_KEY_MAXSIZE_;
		_PV_SERVICETXTLIST_KEY_;

		_SHM_STREAMCTRLINFO_DATA_KEY_;
		_SHM_STREAMCTRLINFO_INDEX_KEY_;
		_SHM_STREAMCTRLINFO_SPACE_INITSIZE_;
		_SHM_STREAMCTRLINFO_SPACE_MAXSIZE_;
		_SHM_STREAMCTRLINFO_EXTENTS_NUM_;
		_SHM_STREAMCTRLINFO_EXTENTS_MAX_NUM_;
		_SHM_STREAMCTRLINFO_EXTENTS_SIZE_;
		_SHM_STREAMCTRLINFO_INDEX_KEYNAME_(0);
		_SHM_STREAMCTRLINFO_DATA_KEYNAME_(0);
		_SHM_STREAMCTRLINFO_KEY_MAXSIZE_;
		_PV_STREAMCTRLINFO_KEY_;

		_SHM_STREAMCTRLDATA_DATA_KEY_;
		_SHM_STREAMCTRLDATA_INDEX_KEY_;
		_SHM_STREAMCTRLDATA_SPACE_INITSIZE_;
		_SHM_STREAMCTRLDATA_SPACE_MAXSIZE_;
		_SHM_STREAMCTRLDATA_EXTENTS_NUM_;
		_SHM_STREAMCTRLDATA_EXTENTS_MAX_NUM_;
		_SHM_STREAMCTRLDATA_EXTENTS_SIZE_;
		_SHM_STREAMCTRLDATA_DATA_KEYNAME_(0);
		_SHM_STREAMCTRLDATA_INDEX_KEYNAME_(0);
		_SHM_STREAMCTRLDATA_KEY_MAXSIZE_;
		_PV_STREAMCTRLDATA_KEY_;

		_SHM_USERINFODATA_DATA_KEY_;
		_SHM_USERINFODATA_INDEX_KEY_;
		_SHM_USERINFODATA_INDEX_KEY_A_;
		_SHM_USERINFODATA_SPACE_INITSIZE_;
		_SHM_USERINFODATA_SPACE_MAXSIZE_;
		_SHM_USERINFODATA_EXTENTS_NUM_;
		_SHM_USERINFODATA_EXTENTS_MAX_NUM_;
		_SHM_USERINFODATA_EXTENTS_SIZE_;
		_SHM_USERINFODATA_KEY_MAXSIZE_;
		_PV_USERINFODATA_KEY_;
		_SHM_USERINFODATA_DATA_KEYNAME_(0);
		_SHM_USERINFODATA_INDEX_KEYNAME_(0);
		_SHM_USERINFODATA_INDEX_KEYNAME_A_(0);

		_SHM_SGWSLAQUEUE_DATA_KEY_;
		_SHM_SGWSLAQUEUE_INDEX_KEY_;
		_SHM_SGWSLAQUEUE_SPACE_INITSIZE_;
		_SHM_SGWSLAQUEUE_SPACE_MAXSIZE_;
		_SHM_SGWSLAQUEUE_EXTENTS_NUM_;
		_SHM_SGWSLAQUEUE_EXTENTS_MAX_NUM_;
		_SHM_SGWSLAQUEUE_EXTENTS_SIZE_;
		_SHM_SGWSLAQUEUE_INDEX_KEYNAME_(0);
		_SHM_SGWSLAQUEUE_DATA_KEYNAME_(0);
		_SHM_SGWSLAQUEUE_KEY_MAXSIZE_;
		_PV_SGWSLAQUEUE_KEY_;

		_SHM_NETINFO_DATA_KEY_;
		_SHM_NETINFO_INDEX_KEY_;
		_SHM_NETINFO_INDEX_KEY_S_;
		_SHM_NETINFO_SPACE_INITSIZE_;
		_SHM_NETINFO_SPACE_MAXSIZE_;
		_SHM_NETINFO_EXTENTS_NUM_;
		_SHM_NETINFO_EXTENTS_MAX_NUM_;
		_SHM_NETINFO_EXTENTS_SIZE_;
		_SHM_NETINFO_DATA_KEYNAME_(0);
		_SHM_NETINFO_INDEX_KEYNAME_(0);
		_SHM_NETINFO_INDEX_KEYNAME_A_(0);
		_SHM_NETINFO_KEY_MAXSIZE_;
		_PV_NETINFO_KEY_;

		_SHM_SESSION_DATA_KEY_;
		_SHM_SESSION_INDEX_KEY_;
		_SHM_SESSION_SPACE_INITSIZE_;
		_SHM_SESSION_SPACE_MAXSIZE_;
		_SHM_SESSION_EXTENTS_NUM_;
		_SHM_SESSION_EXTENTS_MAX_NUM_;
		_SHM_SESSION_EXTENTS_SIZE_;
		_SHM_SESSION_DATA_KEYNAME_(0);
		_SHM_SESSION_INDEX_KEYNAME_(0);
		_SHM_SESSION_KEY_MAXSIZE_;
		_PV_SESSION_KEY_;

		_SHM_SESSIONCLEAR_DATA_KEY_;
		_SHM_SESSIONCLEAR_INDEX_KEY_;
		_SHM_SESSIONCLEAR_SPACE_INITSIZE_;
		_SHM_SESSIONCLEAR_KEY_MAXSIZE_;
		_PV_SESSIONCLEAR_KEY_;

		_SHM_CONGESTLEVEL_DATA_KEY_;
		_SHM_CONGESTLEVEL_SPACE_INITSIZE_;
		_SHM_CONGESTLEVEL_SPACE_MAXSIZE_;
		_SHM_CONGESTLEVEL_EXTENTS_NUM_;
		_SHM_CONGESTLEVEL_EXTENTS_MAX_NUM_;
		_SHM_CONGESTLEVEL_EXTENTS_SIZE_;
		_SHM_CONGESTLEVEL_DATA_KEYNAME_(0);
		_SHM_CONGESTLEVEL_INDEX_KEYNAME_(0);
		_PV_CONGESTLEVEL_KEY_;

		_SHM_SERVICETXTLISTBASE_DATA_KEY_;
		_SHM_SERVICETXTLISTBASE_INDEX_KEY_;
		_SHM_SERVICETXTLISTBASE_SPACE_INITSIZE_;
		_SHM_SERVICETXTLISTBASE_SPACE_MAXSIZE_;
		_SHM_SERVICETXTLISTBASE_EXTENTS_NUM_;
		_SHM_SERVICETXTLISTBASE_EXTENTS_MAX_NUM_;
		_SHM_SERVICETXTLISTBASE_EXTENTS_SIZE_;
		_SHM_SERVICETXTLISTBASE_DATA_KEYNAME_(0);
		_SHM_SERVICETXTLISTBASE_INDEX_KEYNAME_(0);
		_SHM_SERVICETXTLISTBASE_KEY_MAXSIZE_;
		_PV_SERVICETXTLISTBASE_KEY_;

		_SHM_LOADBALANCE_DATA_KEY_;
		_SHM_LOADBALANCE_INDEX_KEY_;
		_SHM_LOADBALANCE_SPACE_INITSIZE_;
		_SHM_LOADBALANCE_SPACE_MAXSIZE_;
		_SHM_LOADBALANCE_EXTENTS_NUM_;
		_SHM_LOADBALANCE_EXTENTS_MAX_NUM_;
		_SHM_LOADBALANCE_EXTENTS_SIZE_;
		_SHM_LOADBALANCE_DATE_KEYNAME_(0);
		_SHM_LOADBALANCE_INDEX_KEYNAME_(0);
		_SHM_LOADBALANCE_KEY_MAXSIZE_;
		_PV_LOADBALANCE_KEY_;

		_SHM_BASEMETHOD_DATA_KEY_;
		_SHM_BASEMETHOD_INDEX_KEY_;
		_SHM_BASEMETHOD_SPACE_INITSIZE_;
		_SHM_BASEMETHOD_SPACE_MAXSIZE_;
		_SHM_BASEMETHOD_EXTENTS_NUM_;
		_SHM_BASEMETHOD_EXTENTS_SIZE_;
		_SHM_BASEMETHOD_EXTENTS_MAX_NUM_;
		_SHM_BASEMETHOD_DATA_KEYNAME_(0);
		_SHM_BASEMETHOD_INDEX_KEYNAME_(0);
		_SHM_BASEMETHOD_KEY_MAXSIZE_;
		_PV_BASEMETHOD_KEY_;

		_SHM_PACKETSESSION_DATA_KEY_;
		_SHM_PACKETSESSION_INDEX_KEY_;
		_SHM_PACKETSUBSESSION_INDEX_KEY_;
		_SHM_PACKETSESSION_SPACE_INITSIZE_;
		_SHM_PACKETSESSION_SPACE_MAXSIZE_;
		_SHM_PACKETSESSION_EXTENTS_NUM_;
		_SHM_PACKETSESSION_EXTENTS_MAX_NUM_;
		_SHM_PACKETSESSION_EXTENTS_SIZE_;
		_SHM_PACKETSESSION_DATA_KEYNAME_(0);
		_SHM_PACKETSESSION_INDEX_KEYNAME_(0);
		_SHM_PACKETSESSION_INDEX_KEYNAME_A_(0);
		_SHM_PACKETSESSION_KEY_MAXSIZE_;
		_PV_PACKETSESSION_KEY_;

		_SHM_SERVICEPACKAGE_DATA_KEY_;
		_SHM_SERVICEPACKAGE_INDEX_KEY_;
		_SHM_SERVICEPACKAGE_INDEX_KEY_A_;
		_SHM_SERVICEPACKAGE_SPACE_INITSIZE_;
		_SHM_SERVICEPACKAGE_SPACE_MAXSIZE_;
		_SHM_SERVICEPACKAGE_EXTENTS_NUM_;
		_SHM_SERVICEPACKAGE_EXTENTS_MAX_NUM_;
		_SHM_SERVICEPACKAGE_EXTENTS_SIZE_;
		_SHM_SERVICEPACKAGE_DATA_KEYNAME_(0);
		_SHM_SERVICEPACKAGE_INDEX_KEYNAME_(0);
		_SHM_SERVICEPACKAGE_INDEX_KEYNAME_A_(0);
		_SHM_SERVICEPACKAGE_KEY_MAXSIZE_;
		_PV_SERVICEPACKAGE_KEY_;

		_SHM_PACKAGEROUTE_DATA_KEY_;
		_SHM_PACKAGEROUTE_INDEX_KEY_;
		_SHM_PACKAGEROUTE_INDEX_KEY_A_;
		_SHM_PACKAGEROUTE_SPACE_INITSIZE_;
		_SHM_PACKAGEROUTE_SPACE_MAXSIZE_;
		_SHM_PACKAGEROUTE_EXTENTS_NUM_;
		_SHM_PACKAGEROUTE_EXTENTS_MAX_NUM_;
		_SHM_PACKAGEROUTE_EXTENTS_SIZE_;
		_SHM_PACKAGEROUTE_DATA_KEYNAME_(0);
		_SHM_PACKAGEROUTE_INDEX_KEYNAME_(0);
		_SHM_PACKAGEROUTE_INDEX_KEYNAME_A_(0);
		_SHM_PACKAGEROUTE_KEY_MAXSIZE_;
		_PV_PACKAGEROUTE_KEY_;

		_SHM_PACKAGEROUTECCR_DATA_KEY_;
		_SHM_PACKAGEROUTECCR_INDEX_KEY_;
		_SHM_PACKAGEROUTECCR_INDEX_KEY_A_;
		_SHM_PACKAGEROUTECCR_SPACE_INITSIZE_;
		_SHM_PACKAGEROUTECCR_SPACE_MAXSIZE_;
		_SHM_PACKAGEROUTECCR_EXTENTS_NUM_;
		_SHM_PACKAGEROUTECCR_EXTENTS_MAX_NUM_;
		_SHM_PACKAGEROUTECCR_EXTENTS_SIZE_;
		_SHM_PACKAGEROUTECCR_DATA_KEYNAME_(0);
		_SHM_PACKAGEROUTECCR_INDEX_KEYNAME_(0);
		_SHM_PACKAGEROUTECCR_INDEX_KEYNAME_A_(0);
		_SHM_PACKAGEROUTECCR_KEY_MAXSIZE_;
		_PV_PACKAGEROUTECCR_KEY_;

		_SHM_SERVICEPACKAGEJUDGE_DATA_KEY_;
		_SHM_SERVICEPACKAGEJUDGE_INDEX_KEY_;
		_SHM_SERVICEPACKAGEJUDGE_INDEX_KEY_A_;
		_SHM_SERVICEPACKAGEJUDGE_SPACE_INITSIZE_;
		_SHM_SERVICEPACKAGEJUDGE_SPACE_MAXSIZE_;
		_SHM_SERVICEPACKAGEJUDGE_EXTENTS_NUM_;
		_SHM_SERVICEPACKAGEJUDGE_EXTENTS_MAX_NUM_;
		_SHM_SERVICEPACKAGEJUDGE_EXTENTS_SIZE_;
		_SHM_SERVICEPACKAGEJUDGE_DATA_KEYNAME_(0);
		_SHM_SERVICEPACKAGEJUDGE_INDEX_KEYNAME_(0);
		_SHM_SERVICEPACKAGEJUDGE_INDEX_KEYNAME_A_(0);
		_SHM_SERVICEPACKAGEJUDGE_KEY_MAXSIZE_;
		_PV_SERVICEPACKAGEJUDGE_KEY_;

		_SHM_SERVICEPACKAGECCA_DATA_KEY_;
		_SHM_SERVICEPACKAGECCA_INDEX_KEY_;
		_SHM_SERVICEPACKAGECCA_INDEX_KEY_A_;
		_SHM_SERVICEPACKAGECCA_SPACE_INITSIZE_;
		_SHM_SERVICEPACKAGECCA_SPACE_MAXSIZE_;
		_SHM_SERVICEPACKAGECCA_EXTENTS_NUM_;
		_SHM_SERVICEPACKAGECCA_EXTENTS_MAX_NUM_;
		_SHM_SERVICEPACKAGECCA_EXTENTS_SIZE_;
		_SHM_SERVICEPACKAGECCA_DATA_KEYNAME_(0);
		_SHM_SERVICEPACKAGECCA_INDEX_KEYNAME_(0);
		_SHM_SERVICEPACKAGECCA_INDEX_KEYNAME_A_(0);
		_SHM_SERVICEPACKAGECCA_KEY_MAXSIZE_;
		_PV_SERVICEPACKAGECCA_KEY_;

		_SHM_SERVICEPACKAGEFUNC_DATA_KEY_;
		_SHM_SERVICEPACKAGEFUNC_INDEX_KEY_;
		_SHM_SERVICEPACKAGEFUNC_SPACE_INITSIZE_;
		_SHM_SERVICEPACKAGEFUNC_SPACE_MAXSIZE_;
		_SHM_SERVICEPACKAGEFUNC_EXTENTS_NUM_;
		_SHM_SERVICEPACKAGEFUNC_EXTENTS_MAX_NUM_;
		_SHM_SERVICEPACKAGEFUNC_EXTENTS_SIZE_;
		_SHM_SERVICEPACKAGEFUNC_DATA_KEYNAME_(0);
		_SHM_SERVICEPACKAGEFUNC_INDEX_KEYNAME_(0);
		_PV_SERVICEPACKAGEFUNC_KEY_;

		_SHM_SERVICEPACKAGEVLE_DATA_KEY_;
		_SHM_SERVICEPACKAGEVLE_INDEX_KEY_;
		_SHM_SERVICEPACKAGEVLE_SPACE_INITSIZE_;
		_SHM_SERVICEPACKAGEVLE_SPACE_MAXSIZE_;
		_SHM_SERVICEPACKAGEVLE_EXTENTS_NUM_;
		_SHM_SERVICEPACKAGEVLE_EXTENTS_MAX_NUM_;
		_SHM_SERVICEPACKAGEVLE_EXTENTS_SIZE_;
		_SHM_SERVICEPACKAGEVLE_DATA_KEYNAME_(0);
		_SHM_SERVICEPACKAGEVLE_INDEX_KEYNAME_(0);
		_PV_SERVICEPACKAGEVLE_KEY_;

		_SHM_SCONGESTMSG_DATA_KEY_;
		_SHM_SCONGESTMSG_INDEX_KEY_;
		_SHM_SCONGESTMSG_SPACE_INITSIZE_;
		_SHM_SCONGESTMSG_SPACE_MAXSIZE_;
		_SHM_SCONGESTMSG_EXTENTS_NUM_;
		_SHM_SCONGESTMSG_EXTENTS_MAX_NUM_;
		_SHM_SCONGESTMSG_EXTENTS_SIZE_;
		_SHM_SCONGESTMSG_DATA_KEYNAME_(0);
		_SHM_SCONGESTMSG_INDEX_KEYNAME_(0);
		_SHM_SCONGESTMSG_KEY_MAXSIZE_;
		_PV_SCONGESTMSG_KEY_;

		_SHM_WFPROCESSMQ_DATA_KEY_;
		_SHM_WFPROCESSMQ_INDEX_KEY_;
		_SHM_WFPROCESSMQ_SPACE_INITSIZE_;
		_SHM_WFPROCESSMQ_SPACE_MAXSIZE_;
		_SHM_WFPROCESSMQ_EXTENTS_NUM_;
		_SHM_WFPROCESSMQ_EXTENTS_MAX_NUM_;
		_SHM_WFPROCESSMQ_EXTENTS_SIZE_;
		_SHM_WFPROCESSMQ_DATA_KEYNAME_(0);
		_SHM_WFPROCESSMQ_INDEX_KEYNAME_(0);
		_PV_WFPROCESSMQ_KEY_;

		_SHM_PORTINFO_DATA_KEY_;
		_SHM_PORTINFO_INDEX_KEY_;
		_SHM_PORTINFO_SPACE_INITSIZE_;
		_SHM_PORTINFO_SPACE_MAXSIZE_;
		_SHM_PORTINFO_EXTENTS_NUM_;
		_SHM_PORTINFO_EXTENTS_MAX_NUM_;
		_SHM_PORTINFO_EXTENTS_SIZE_;
		_SHM_PORTINFO_DATA_KEYNAME_(0);
		_SHM_PORTINFO_INDEX_KEYNAME_(0);
		_PV_PORTINFO_KEY_;

		_SHM_ORGROUTE_DATA_KEY_;
		_SHM_ORGROUTE_INDEX_KEY_;
		_SHM_ORGROUTE_INDEX_KEY_A_;
		_SHM_ORGROUTE_SPACE_INITSIZE_;
		_SHM_ORGROUTE_SPACE_MAXSIZE_;
		_SHM_ORGROUTE_EXTENTS_NUM_;
		_SHM_ORGROUTE_EXTENTS_MAX_NUM_;
		_SHM_ORGROUTE_EXTENTS_SIZE_;
		_SHM_ORGROUTE_DATA_KEYNAME_(0);
		_SHM_ORGROUTE_INDEX_KEYNAME_(0);
		_PV_ORGROUTE_KEY_;

		_SHM_GLOBALMAP_DATA_KEY_;
		_SHM_GLOBALMAP_INDEX_KEY_;
		_SHM_GLOBALMAP_SPACE_INITSIZE_;
		_SHM_GLOBALMAP_SPACE_MAXSIZE_;
		_SHM_GLOBALMAP_DATA_KEYNAME_(0);
		_SHM_GLOBALMAP_INDEX_KEYNAME_(0);
		_PV_GLOBALMAP_KEY_;

		_SHM_ORGHEAD_DATA_KEY_;
		_SHM_ORGHEAD_INDEX_KEY_;
		_SHM_ORGHEAD_SPACE_INITSIZE_;
		_SHM_ORGHEAD_SPACE_MAXSIZE_;
		_SHM_ORGHEAD_EXTENTS_NUM_;
		_SHM_ORGHEAD_EXTENTS_MAX_NUM_;
		_SHM_ORGHEAD_EXTENTS_SIZE_;
		_SHM_ORGHEAD_DATA_KEYNAME_(0);
		_SHM_ORGHEAD_INDEX_KEYNAME_(0);
		_PV_ORGHEAD_KEY_;

		_SHM_CHANNEL_DATA_KEY_;
		_SHM_CHANNEL_INDEX_KEY_;
		_SHM_CHANNEL_SPACE_INITSIZE_;
		_SHM_CHANNEL_SPACE_MAXSIZE_;
		_SHM_CHANNEL_EXTENTS_NUM_;
		_SHM_CHANNEL_EXTENTS_MAX_NUM_;
		_SHM_CHANNEL_EXTENTS_SIZE_;
		_SHM_CHANNEL_DATA_KEYNAME_(0);
		_SHM_CHANNEL_INDEX_KEYNAME_(0);
		_PV_CHANNEL_KEY_;

		_SHM_USERSTAFF_DATA_KEY_;
		_SHM_USERSTAFF_INDEX_KEY_;
		_SHM_USERSTAFF_INDEX_KEY_A_;
		_SHM_USERSTAFF_INDEX_KEY_B_;
		_SHM_USERSTAFF_SPACE_INITSIZE_;
		_SHM_USERSTAFF_SPACE_MAXSIZE_;
		_SHM_USERSTAFF_EXTENTS_NUM_;
		_SHM_USERSTAFF_EXTENTS_MAX_NUM_;
		_SHM_USERSTAFF_EXTENTS_SIZE_;
		_SHM_USERSTAFF_DATA_KEYNAME_(0);
		_SHM_USERSTAFF_INDEX_KEYNAME_(0);
		_PV_USERSTAFF_KEY_;

		_SHM_ORGROUTERULE_DATA_KEY_;
		_SHM_ORGROUTERULE_INDEX_KEY_;
		_SHM_ORGROUTERULE_INDEX_KEY_A_;
		_SHM_ORGROUTERULE_SPACE_INITSIZE_;
		_SHM_ORGROUTERULE_SPACE_MAXSIZE_;
		_SHM_ORGROUTERULE_EXTENTS_NUM_;
		_SHM_ORGROUTERULE_EXTENTS_MAX_NUM_;
		_SHM_ORGROUTERULE_EXTENTS_SIZE_;
		_SHM_ORGROUTERULE_DATA_KEYNAME_(0);
		_SHM_ORGROUTERULE_INDEX_KEYNAME_(0);
		_PV_ORGROUTERULE_KEY_;

		_SHM_AREAORG_DATA_KEY_;
		_SHM_AREAORG_INDEX_KEY_;
		_SHM_AREAORG_SPACE_INITSIZE_;
		_SHM_AREAORG_SPACE_MAXSIZE_;
		_SHM_AREAORG_EXTENTS_NUM_;
		_SHM_AREAORG_EXTENTS_MAX_NUM_;
		_SHM_AREAORG_EXTENTS_SIZE_;
		_SHM_AREAORG_DATA_KEYNAME_(0);
		_SHM_AREAORG_INDEX_KEYNAME_(0);
		_PV_AREAORG_KEY_;

		_SHM_TUXRELAIN_DATA_KEY_;
		_SHM_TUXRELAIN_INDEX_KEY_;
		_SHM_TUXRELAIN_SPACE_INITSIZE_;
		_SHM_TUXRELAIN_SPACE_MAXSIZE_;
		_SHM_TUXRELAIN_EXTENTS_NUM_;
		_SHM_TUXRELAIN_EXTENTS_MAX_NUM_;
		_SHM_TUXRELAIN_EXTENTS_SIZE_;
		_SHM_TUXRELAIN_DATA_KEYNAME_(0);
		_SHM_TUXRELAIN_INDEX_KEYNAME_(0);
		_PV_TUXRELAIN_KEY_;

		_SHM_TUXRELAOUT_DATA_KEY_;
		_SHM_TUXRELAOUT_INDEX_KEY_;
		_SHM_TUXRELAOUT_INDEX_KEY_A_;
		_SHM_TUXRELAOUT_SPACE_INITSIZE_;
		_SHM_TUXRELAOUT_SPACE_MAXSIZE_;
		_SHM_TUXRELAOUT_EXTENTS_NUM_;
		_SHM_TUXRELAOUT_EXTENTS_MAX_NUM_;
		_SHM_TUXRELAOUT_EXTENTS_SIZE_;
		_SHM_TUXRELAOUT_DATA_KEYNAME_(0);
		_SHM_TUXRELAOUT_INDEX_KEYNAME_(0);
		_PV_TUXRELAOUT_KEY_;

		_SHM_SVRLIST_DATA_KEY_;
		_SHM_SVRLIST_INDEX_KEY_;
		_SHM_SVRLIST_INDEX_KEY_A_;
		_SHM_SVRLIST_SPACE_INITSIZE_;
		_SHM_SVRLIST_SPACE_MAXSIZE_;
		_SHM_SVRLIST_EXTENTS_NUM_;
		_SHM_SVRLIST_EXTENTS_MAX_NUM_;
		_SHM_SVRLIST_EXTENTS_SIZE_;
		_SHM_SVRLIST_DATA_KEYNAME_(0);
		_SHM_SVRLIST_INDEX_KEYNAME_(0);
		_PV_SVRLIST_KEY_;

		_SHM_SVRSPARAMRELA_DATA_KEY_;
		_SHM_SVRSPARAMRELA_INDEX_KEY_;
		_SHM_SVRSPARAMRELA_INDEX_KEY_A_;
		_SHM_SVRSPARAMRELA_SPACE_INITSIZE_;
		_SHM_SVRSPARAMRELA_SPACE_MAXSIZE_;
		_SHM_SVRSPARAMRELA_EXTENTS_NUM_;
		_SHM_SVRSPARAMRELA_EXTENTS_MAX_NUM_;
		_SHM_SVRSPARAMRELA_EXTENTS_SIZE_;
		_SHM_SVRSPARAMRELA_DATA_KEYNAME_(0);
		_SHM_SVRSPARAMRELA_INDEX_KEYNAME_(0);
		_PV_SVRSPARAMRELA_KEY_;

		_SHM_SVRSPARAMLIST_DATA_KEY_;
		_SHM_SVRSPARAMLIST_INDEX_KEY_;
		_SHM_SVRSPARAMLIST_SPACE_INITSIZE_;
		_SHM_SVRSPARAMLIST_SPACE_MAXSIZE_;
		_SHM_SVRSPARAMLIST_EXTENTS_NUM_;
		_SHM_SVRSPARAMLIST_EXTENTS_MAX_NUM_;
		_SHM_SVRSPARAMLIST_EXTENTS_SIZE_;
		_SHM_SVRSPARAMLIST_DATA_KEYNAME_(0);
		_SHM_SVRSPARAMLIST_INDEX_KEYNAME_(0);
		_PV_SVRSPARAMLIST_KEY_;

		_SHM_SVRSREGISTER_DATA_KEY_;
		_SHM_SVRSREGISTER_INDEX_KEY_;
		_SHM_SVRSREGISTER_INDEX_KEY_A_;
		_SHM_SVRSREGISTER_SPACE_INITSIZE_;
		_SHM_SVRSREGISTER_SPACE_MAXSIZE_;
		_SHM_SVRSREGISTER_EXTENTS_NUM_;
		_SHM_SVRSREGISTER_EXTENTS_MAX_NUM_;
		_SHM_SVRSREGISTER_EXTENTS_SIZE_;
		_SHM_SVRSREGISTER_DATA_KEYNAME_(0);
		_SHM_SVRSREGISTER_INDEX_KEYNAME_(0);
		_PV_SVRSREGISTER_KEY_;

		_SHM_PARAMINSLIST_DATA_KEY_;
		_SHM_PARAMINSLIST_INDEX_KEY_;
		_SHM_PARAMINSLIST_INDEX_KEY_A_;
		_SHM_PARAMINSLIST_SPACE_INITSIZE_;
		_SHM_PARAMINSLIST_SPACE_MAXSIZE_;
		_SHM_PARAMINSLIST_EXTENTS_NUM_;
		_SHM_PARAMINSLIST_EXTENTS_MAX_NUM_;
		_SHM_PARAMINSLIST_EXTENTS_SIZE_;
		_SHM_PARAMINSLIST_DATA_KEYNAME_(0);
		_SHM_PARAMINSLIST_INDEX_KEYNAME_(0);
		_PV_PARAMINSLIST_KEY_;

		_SHM_SGWENUMAREA_DATA_KEY_;
		_SHM_SGWENUMAREA_INDEX_KEY_;
		_SHM_SGWENUMAREA_INDEX_KEY_A_;
		_SHM_SGWENUMAREA_SPACE_INITSIZE_;
		_SHM_SGWENUMAREA_SPACE_MAXSIZE_;
		_SHM_SGWENUMAREA_EXTENTS_NUM_;
		_SHM_SGWENUMAREA_EXTENTS_MAX_NUM_;
		_SHM_SGWENUMAREA_EXTENTS_SIZE_;
		_SHM_SGWENUMAREA_DATA_KEYNAME_(0);
		_SHM_SGWENUMAREA_INDEX_KEYNAME_(0);
		_PV_SGWENUMAREA_KEY_;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		exit(-1);
	}
}

int SHMCmdSet::clearSHMALL()
{
	int i = 0;
	int iRet = 0;

	while (TABLEPARAM[i]!=NULL&&i<_SHM_FILE_NUMBER_) {
		iRet = clearoneSHM(TABLEPARAM[i]);
		if (iRet == -1) {
			return 0;
		} else if (iRet == MBC_SHM_INIT_FAILED) {
			return iRet;
		}
		i++;
	}
	return iRet;
}

//创建共享内存
int SHMCmdSet::createSHMALL()
{
	int i = 0;
	int iRet = 0;

	while (TABLEPARAM[i]!=NULL&&i<_SHM_FILE_NUMBER_) {
		iRet = createoneSHM(TABLEPARAM[i]);
		if (iRet == -1) {
			return 0;
		} else if (iRet == MBC_SHM_INIT_FAILED ) {
			return iRet;
		}
		i++;
	}
	return iRet;
}

int SHMCmdSet::unloadSHMALL()
{
	int i = 0;
	int iRet = 0;

	while (TABLEPARAM[i]!=NULL&&i<_SHM_FILE_NUMBER_) {
		iRet = unloadoneSHM(TABLEPARAM[i]);
		if (iRet == -1) {
			return 0;
		} else if (iRet == MBC_SHM_INIT_FAILED) {
			return iRet;
		}
		i++;
	}
	return iRet;

}

int SHMCmdSet::exportOneSHMALL(const char *_sType,const char * sTab)
{
	int iRet = 0;
	switch(checkargv(_sType))
	{
	case sgw_org_route:
		{
			int iRet = 0;
			iRet = SHMDataMgr::sgworgroute_init();
			if (!iRet) {
				SHMOrgRoute::ExpRouteOrg(sTab);
				SHMDataMgr::sgworgroute_free();
			} else {
				//cout<<"[update errno]: "<<__FILE__<<":"<<__LINE__<<"  sgworgroute_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
			break;
		}
	case sgw_org_head:
		{
			iRet = SHMDataMgr::sgworghead_init();
			if (!iRet) {
				SHMOrgHead::ExpAccNbrHead(sTab);
				SHMDataMgr::sgworghead_free();
			} else {
				//cout<<"[update errno]: "<<__FILE__<<":"<<__LINE__<<"  sgworghead_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}

		}
		break;
	default:
		break;
	}
	return iRet;
}

int SHMCmdSet::detachSHMALL()
{
	int iRet = 0;
	if (!SHMDataMgr::contentfiltering_init()) {
		SHMDataMgr::detachContentFiltering();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  contentfiltering_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::msgmap_init()) {
		SHMDataMgr::detachMsgMap();//断开连接存储表TAB_MSG_MAP的数据的共享内存
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  msgmap_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::servicetxtlist_init()) {
		SHMDataMgr::detachServiceTxtList();//断开连接存储表service_context_list的数据的共享内存
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlist_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::streamctrl_init()) {
		SHMDataMgr::detachStreamCtrl();//断开连接存储表STREAM_CTRL_INFO的数据的共享内存
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrl_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::streamctrldata_init()) {
		SHMDataMgr::detachStreamCtrlData();//断开连接共享内存，提供业务进程实时数据存储
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrldata_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::userinfo_init()) {
		SHMDataMgr::detachUserInfo();//断开连接共享内存，提供业务进程实时数据存储
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  userinfo_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::sgwslaquerel_init()) {
		SHMDataMgr::detachSgwSlaqueRel();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwslaquerel_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::netinfo_init()) {
		SHMDataMgr::detachNet();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  netinfo_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::session_init()) {
		SHMDataMgr::detachSession();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  session_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::congestleve_init()) {
		SHMDataMgr::detachCongestLeve();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  congestleve_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::servicetxtlistbase_init()) {
		SHMDataMgr::detachServiceTxtListBase();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlistbase_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::loadbalancedata_init()) {
		SHMDataMgr::detachloadbalance();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  loadbalancedata_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::servicepackageroute_init()) {
		SHMDataMgr::detachpackageroute();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackageroute_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::basemethod_init()) {
		SHMDataMgr::detachbasemethod();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  basemethod_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::packetsession_init()) {
		SHMDataMgr::detachpacketsession();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  packetsession_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::servicepackage_init()) {
		SHMDataMgr::detachservicepackage();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackage_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::servicepackagerouteCCR_init()) {
		SHMDataMgr::detachservicepackageCCR();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagerouteCCR_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::servicepackagejudge_init()) {
		SHMDataMgr::detachpackagejudge();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagejudge_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::servicepackagecca_init()) {
		SHMDataMgr::detachpackagecca();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagecca_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::servicepackagefunc_init()) {
		SHMDataMgr::detachpackagefunc();
	} else {
		//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagefunc_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::servicepackagevariable_init()) {
		SHMDataMgr::detachpackagevle();
	} else {
		//cout<<"[update errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagevariable_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::scongestmsg_init()) {
		SHMDataMgr::detachscongestmsg();
	} else {
		//cout<<"[update errno]: "<<__FILE__<<":"<<__LINE__<<"  scongestmsg_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::wfprocessmq_init()) {
		SHMDataMgr::detachwfprocessmq();
	} else {
		//cout<<"[update errno]: "<<__FILE__<<":"<<__LINE__<<"  scongestmsg_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	if (!SHMDataMgr::portInfo_init()) {
		SHMDataMgr::detachportinfo();
	} else {
		//cout<<"[update errno]: "<<__FILE__<<":"<<__LINE__<<"  scongestmsg_init failed!"<<endl;
		iRet = MBC_SHM_INIT_FAILED;
	}

	SHMSGWInfoBase::freeAll();
	return iRet;
}

int SHMCmdSet::loadSHMALL()
{
	int i = 0;
	int iRet = 0;

	while (TABLEPARAM[i]!=NULL&&i<_SHM_FILE_NUMBER_) {
		iRet = loadoneSHM(TABLEPARAM[i]);
		if (iRet == -1) {
			return 0;
		} else if (iRet == MBC_SHM_INIT_FAILED ) {
			return iRet;
		}
		i++;
	}
	return iRet;
}

int SHMCmdSet::querySHMALL(vector<SHMInfo> &_vShmInfo)
{
	int i = 0;
	int iRet = 0;
	_vShmInfo.clear();

	while (TABLEPARAM[i]!=NULL&&i<_SHM_FILE_NUMBER_) {
		iRet = queryoneSHM(TABLEPARAM[i],_vShmInfo);
		if (iRet == -1) {
			return 0;
		}
		i++;
	}
	return iRet;
}

//创建某一个快共享内存
int SHMCmdSet::createoneSHM(const char *_sType)
{
	int iRet = 0;
	switch(checkargv(_sType))
	{
	case context_filter:
		{
			//初始化连接共享内存
			iRet = SHMDataMgr::contentfiltering_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createContentFilteringSHM();//创建存储表context_filter信息的共享内存
				SHMDataMgr::contentfiltering_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  contentfiltering_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case tab_msg_map:
		{
			iRet = SHMDataMgr::msgmap_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createMsgMapSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::msgmap_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  msgmap_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_context_list:
		{
			iRet = SHMDataMgr::servicetxtlist_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createServiceTxtListSHM();//创建存储表service_context_list的数据的共享内存
				SHMDataMgr::servicetxtlist_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case stream_ctrl_info:
		{
			iRet = SHMDataMgr::streamctrl_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createStreamCtrlSHM();//创建存储表STREAM_CTRL_INFO的数据的共享内存
				SHMDataMgr::streamctrl_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrl_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case stream_ctrl_data:
		{
			iRet = SHMDataMgr::streamctrldata_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createStreamCtrlDataSHM();
				SHMDataMgr::streamctrldata_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrldata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case userinfo:
		{
			iRet = SHMDataMgr::userinfo_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet =SHMDataMgr::createUserInfoSHM();
				SHMDataMgr::userinfo_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  userinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_sla_que_rel:
		{
			iRet = SHMDataMgr::sgwslaquerel_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet =SHMDataMgr::createSgwSlaqueRelSHM();
				SHMDataMgr::sgwslaquerel_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwslaquerel_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case netInfodata:
		{
			iRet = SHMDataMgr::netinfo_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createNetSHM();
				SHMDataMgr::netinfo_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  netinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sessiondata:
		{
			iRet = SHMDataMgr::session_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createSessionSHM();
				SHMDataMgr::session_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  session_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case shmcongestlevel:
		{
			iRet = SHMDataMgr::congestleve_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createCongestLeveSHM();
				SHMDataMgr::congestleve_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  congestleve_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_context_list_base:
		{
			iRet = SHMDataMgr::servicetxtlistbase_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createServiceTxtListBaseSHM();
				SHMDataMgr::servicetxtlistbase_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlistbase_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case loadbalancedata:
		{
			iRet = SHMDataMgr::loadbalancedata_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createloadbalanceSHM();
				SHMDataMgr::loadbalancedata_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  loadbalancedata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_route:
		{
			iRet = SHMDataMgr::servicepackageroute_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createpackagerouteSHM();
				SHMDataMgr::servicepackageroute_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackageroute_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case base_method:
		{
			iRet = SHMDataMgr::basemethod_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createbasemethodSHM();
				SHMDataMgr::basemethod_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  basemethod_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case s_packet_session:
		{
			iRet = SHMDataMgr::packetsession_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createpacketsessionSHM();
				SHMDataMgr::packetsession_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  packetsession_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_node:
		{
			iRet = SHMDataMgr::servicepackage_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createservicepackageSHM();
				SHMDataMgr::servicepackage_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackage_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_route_ccr:
		{
			iRet = SHMDataMgr::servicepackagerouteCCR_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createservicepackageCCRSHM();
				SHMDataMgr::servicepackagerouteCCR_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagerouteCCR_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_judge:
		{
			iRet = SHMDataMgr::servicepackagejudge_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createpackagejudgeSHM();
				SHMDataMgr::servicepackagejudge_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagejudge_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_cca:
		{
			iRet = SHMDataMgr::servicepackagecca_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createpackageccaSHM();
				SHMDataMgr::servicepackagecca_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagecca_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_func:
		{
			iRet = SHMDataMgr::servicepackagefunc_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createpackagefuncSHM();
				SHMDataMgr::servicepackagefunc_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagefunc_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_variable:
		{
			iRet = SHMDataMgr::servicepackagevariable_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createpackagevleSHM();
				SHMDataMgr::servicepackagevariable_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagevariable_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case statisticscongestmsg:
		{
			iRet = SHMDataMgr::scongestmsg_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createscongestmsgSHM();
				SHMDataMgr::scongestmsg_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  scongestmsg_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case wf_process_mq:
		{
			iRet = SHMDataMgr::wfprocessmq_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createwfprocessmqSHM();
				SHMDataMgr::wfprocessmq_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  wfprocessmq_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case portinfo:
		{
			iRet = SHMDataMgr::portInfo_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createportinfoSHM();
				SHMDataMgr::portInfo_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case session_clear:
		{
			iRet = SHMDataMgr::sessionclear_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createSessionClearSHM();
				SHMDataMgr::sessionclear_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sessionclear_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_org_route:
		{
			iRet = SHMDataMgr::sgworgroute_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createOrgRouteSHM();
				SHMDataMgr::sgworgroute_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sgworgroute_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_org_head:
		{
			iRet = SHMDataMgr::sgworghead_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createOrgHeadSHM();
				SHMDataMgr::sgworghead_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sgworghead_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case channelInfo:
		{
			iRet = SHMDataMgr::channelinfo_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet= SHMDataMgr::createChannelSHM();
				SHMDataMgr::channelinfo_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  channelinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_global_map:
		{
			iRet = SHMDataMgr::globalmap_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet= SHMDataMgr::createGlobalMapSHM();
				SHMDataMgr::globalmap_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  globalmap_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_user_staff:
		{
			iRet = SHMDataMgr::userstaff_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createUserStaffSHM();
				SHMDataMgr::userstaff_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  userstaff_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_route_rule:
		{
			iRet = SHMDataMgr::orgrouterule_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createRouteRuleSHM();
				SHMDataMgr::orgrouterule_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  orgrouterule_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_area_org:
		{
			iRet = SHMDataMgr::sgwareaorg_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createAreaOrgSHM();
				SHMDataMgr::sgwareaorg_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwareaorg_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_tux_rela_in:
		{
			iRet = SHMDataMgr::sgwtuxrelain_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createTuxRelaInSHM();
				SHMDataMgr::sgwtuxrelain_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwtuxrelain_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_tux_rela_out:
		{
			iRet = SHMDataMgr::sgwtuxrelaout_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createTuxRelaOutSHM();
				SHMDataMgr::sgwtuxrelaout_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwtuxrelaout_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_svr_list:
		{
			iRet = SHMDataMgr::sgwsvrlist_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createSvrListSHM();
				SHMDataMgr::sgwsvrlist_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsvrlist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_svrs_param_rela:
		{
			iRet = SHMDataMgr::sgwsrvsparamrela_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createSvrsParamRelaSHM();
				SHMDataMgr::sgwsrvsparamrela_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsrvsparamrela_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_svrs_param_list:
		{
			iRet = SHMDataMgr::sgwsrvsparamlist_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createSvrsParamListSHM();
				SHMDataMgr::sgwsrvsparamlist_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsrvsparamlist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_svrs_register:
		{
			iRet = SHMDataMgr::sgwsrvsregister_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createSvrsRegisterSHM();
				SHMDataMgr::sgwsrvsregister_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsrvsregister_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_param_ins_list:
		{
			iRet = SHMDataMgr::sgwparaminslist_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createParamInsListSHM();
				SHMDataMgr::sgwparaminslist_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwparaminslist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_enum_area:
		{
			iRet = SHMDataMgr::sgwenumarea_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				iRet = SHMDataMgr::createSgwEnumAreaSHM();
				SHMDataMgr::sgwenumarea_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwparaminslist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	default:
		return -1;
		break;
	}
	if (iRet != 0){
		cout<<_sType<<"共享内存创建结束,可通过sgwshm -stat 查询其状态……"<<endl<<endl;
	}
	return iRet;
}

//扩充某一个块共享内存
int SHMCmdSet::ralloconeSHM(const char *_sType)
{
	int iRet =0;

	switch(checkargv(_sType))
	{
	case context_filter:
		{
			iRet = SHMDataMgr::contentfiltering_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_CONTEXTDATA_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_CONTEXTDATA_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};
					SHMDataMgr::contentfiltering_init();
					iRet =  SHMDataMgr::createContentFilteringSHM(1);

					if (iRet>0) {//创建存储表context_filter信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.context_filter_data.memory_extents_num=%d",_SHM_CONTEXTDATA_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.context_filter_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::contentfiltering_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  contentfiltering_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}

		}
		break;
	case tab_msg_map:
		{
			iRet = SHMDataMgr::msgmap_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_MSGMAPINFO_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);
				if (_SHM_MSGMAPINFO_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};
					SHMDataMgr::msgmap_init();
					int iRet =  SHMDataMgr::createMsgMapSHM(1);

					if (iRet>0) {
						snprintf(sValue,sizeof(sValue)-1,"memory.tabmsgmap_data.memory_extents_num=%d",_SHM_MSGMAPINFO_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.tabmsgmap_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}

					SHMDataMgr::msgmap_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  msgmap_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_context_list:
		{
			iRet = SHMDataMgr::servicetxtlist_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_SERVICETXTLIST_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);
				if (_SHM_SERVICETXTLIST_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};
					SHMDataMgr::servicetxtlist_init();
					int iRet =  SHMDataMgr::createServiceTxtListSHM(1);

					if (iRet>0) {
						snprintf(sValue,sizeof(sValue)-1,"memory.servicetxtlist_data.memory_extents_num=%d",_SHM_SERVICETXTLIST_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.servicetxtlist_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}

					SHMDataMgr::servicetxtlist_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case stream_ctrl_info:
		{
			iRet = SHMDataMgr::streamctrl_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_STREAMCTRLINFO_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_STREAMCTRLINFO_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};
					SHMDataMgr::streamctrl_init();
					int iRet =  SHMDataMgr::createStreamCtrlSHM(1);

					if (iRet>0)  {
						snprintf(sValue,sizeof(sValue)-1,"memory.streamctrlinfo_data.memory_extents_num=%d",_SHM_STREAMCTRLINFO_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.streamctrlinfo_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::streamctrl_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrl_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case stream_ctrl_data:
		{
			iRet = SHMDataMgr::streamctrldata_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_STREAMCTRLDATA_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_STREAMCTRLDATA_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};
					SHMDataMgr::streamctrldata_init();
					int iRet =  SHMDataMgr::createStreamCtrlDataSHM(1);

					if (iRet>0) {
						snprintf(sValue,sizeof(sValue)-1,"memory.streamctrldata.memory_extents_num=%d",_SHM_STREAMCTRLDATA_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.streamctrldata.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::streamctrldata_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrldata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case userinfo:
		{
			iRet = SHMDataMgr::userinfo_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_USERINFODATA_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_USERINFODATA_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};
					SHMDataMgr::userinfo_init();
					int iRet =  SHMDataMgr::createUserInfoSHM(1);

					if (iRet>0) {
						snprintf(sValue,sizeof(sValue)-1,"memory.userinfodata.memory_extents_num=%d",_SHM_USERINFODATA_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.userinfodata.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}

					SHMDataMgr::userinfo_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  userinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}

		}
		break;
	case sgw_sla_que_rel:
		{
			iRet = SHMDataMgr::sgwslaquerel_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_SGWSLAQUEUE_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_SGWSLAQUEUE_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};
					SHMDataMgr::sgwslaquerel_init();
					int iRet =  SHMDataMgr::createSgwSlaqueRelSHM(1);

					if (iRet>0) {
						snprintf(sValue,sizeof(sValue)-1,"memory.sgeslaquereldata.memory_extents_num=%d",_SHM_SGWSLAQUEUE_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.sgeslaquereldata.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}

					SHMDataMgr::sgwslaquerel_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwslaquerel_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case netInfodata:
		{
			iRet = SHMDataMgr::netinfo_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_NETINFO_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_NETINFO_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::netinfo_init();
					int iRet =  SHMDataMgr::createNetSHM(1);

					if (iRet>0) {//创建存储表context_filter信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.netInfodata_data.memory_extents_num=%d",_SHM_NETINFO_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.netInfodata_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}

					SHMDataMgr::netinfo_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  netinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}

		}
		break;
	case sessiondata:
		{
			iRet = SHMDataMgr::session_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_SESSION_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_SESSION_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::session_init();
					int iRet =  SHMDataMgr::createSessionSHM(1);

					if (iRet>0) {
						snprintf(sValue,sizeof(sValue)-1,"memory.session_data.memory_extents_num=%d",_SHM_SESSION_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.session_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}

					SHMDataMgr::session_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  session_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case shmcongestlevel:
		{
			iRet = SHMDataMgr::congestleve_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_CONGESTLEVEL_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_CONGESTLEVEL_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::congestleve_init();
					int iRet =  SHMDataMgr::createCongestLeveSHM(1);

					if (iRet>0) {//创建存储表context_filter信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.shmcongestlevel_data.memory_extents_num=%d",_SHM_CONGESTLEVEL_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.shmcongestlevel_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}

					SHMDataMgr::congestleve_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}

			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  congestleve_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_context_list_base:
		{
			iRet = SHMDataMgr::servicetxtlistbase_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_SERVICETXTLISTBASE_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_SERVICETXTLISTBASE_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::servicetxtlistbase_init();
					int iRet =  SHMDataMgr::createServiceTxtListBaseSHM(1);

					if (iRet>0) {//创建存储表context_filter信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.service_txt_list_base_data.memory_extents_num=%d",_SHM_SERVICETXTLISTBASE_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.service_txt_list_base_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::servicetxtlistbase_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}

			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  contentfiltering_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case loadbalancedata:
		{
			iRet = SHMDataMgr::loadbalancedata_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_LOADBALANCE_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);
				if (_SHM_LOADBALANCE_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::loadbalancedata_init();
					int iRet =  SHMDataMgr::createloadbalanceSHM(1);

					if (iRet>0) {//创建存储表context_filter信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.loadbalancedata.memory_extents_num=%d",_SHM_LOADBALANCE_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.loadbalancedata.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}

					SHMDataMgr::loadbalancedata_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}

			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  loadbalancedata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_route:
		{
			iRet = SHMDataMgr::servicepackageroute_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_PACKAGEROUTE_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_PACKAGEROUTE_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::servicepackageroute_init();
					int iRet =  SHMDataMgr::createpackagerouteSHM(1);

					if (iRet>0) {//创建存储表context_filter信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.packageroute_data.memory_extents_num=%d",_SHM_PACKAGEROUTE_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.packageroute_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::servicepackageroute_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}

			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackageroute_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case base_method:
		{
			iRet = SHMDataMgr::basemethod_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_BASEMETHOD_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_BASEMETHOD_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::basemethod_init();
					int iRet =  SHMDataMgr::createbasemethodSHM(1);

					if (iRet>0) {//创建存储表context_filter信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.basemethod_data.memory_extents_num=%d",_SHM_BASEMETHOD_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.basemethod_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::basemethod_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  basemethod_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}

		}
		break;
	case s_packet_session:
		{
			iRet = SHMDataMgr::packetsession_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_PACKETSESSION_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_PACKETSESSION_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::packetsession_init();
					int iRet =  SHMDataMgr::createpacketsessionSHM(1);

					if (iRet>0) {//创建存储表context_filter信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.packetsession_data.memory_extents_num=%d",_SHM_PACKETSESSION_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.packetsession_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::packetsession_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  packetsession_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}

		}
		break;
	case service_package_node:
		{
			iRet = SHMDataMgr::servicepackage_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_SERVICEPACKAGE_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_SERVICEPACKAGE_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::servicepackage_init();
					int iRet =  SHMDataMgr::createservicepackageSHM(1);

					if (iRet>0) {//创建存储表context_filter信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.servicepackage_data.memory_extents_num=%d",_SHM_SERVICEPACKAGE_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.servicepackage_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}

					SHMDataMgr::servicepackage_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackage_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_route_ccr:
		{
			iRet = SHMDataMgr::servicepackagerouteCCR_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_PACKAGEROUTECCR_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_PACKAGEROUTECCR_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::servicepackagerouteCCR_init();
					int iRet =  SHMDataMgr::createservicepackageCCRSHM(1);

					if (iRet>0) {
						snprintf(sValue,sizeof(sValue)-1,"memory.packagerouteccr_data.memory_extents_num=%d",_SHM_PACKAGEROUTECCR_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.packagerouteccr_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::servicepackagerouteCCR_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagerouteCCR_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_judge:
		{
			iRet = SHMDataMgr::servicepackagejudge_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_SERVICEPACKAGEJUDGE_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_SERVICEPACKAGEJUDGE_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::servicepackagejudge_init();
					int iRet =  SHMDataMgr::createpackagejudgeSHM(1);

					if (iRet>0) {
						snprintf(sValue,sizeof(sValue)-1,"memory.packagejudge_data.memory_extents_num=%d",_SHM_SERVICEPACKAGEJUDGE_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.packagejudge_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::servicepackagejudge_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagejudge_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}

		}
		break;
	case service_package_cca:
		{
			iRet = SHMDataMgr::servicepackagecca_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_SERVICEPACKAGECCA_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_SERVICEPACKAGECCA_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};
					SHMDataMgr::servicepackagecca_init();
					int iRet =  SHMDataMgr::createpackageccaSHM(1);

					if (iRet>0) {//创建存储文件context_filter信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.service_package_cca_data.memory_extents_num=%d",_SHM_SERVICEPACKAGECCA_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.service_package_cca_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::servicepackagecca_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagecca_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}

		}
		break;
	case service_package_func:
		{
			iRet = SHMDataMgr::servicepackagefunc_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_SERVICEPACKAGEFUNC_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_SERVICEPACKAGEFUNC_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};
					SHMDataMgr::servicepackagefunc_init();
					int iRet =  SHMDataMgr::createpackagefuncSHM(1);

					if (iRet>0) {
						snprintf(sValue,sizeof(sValue)-1,"memory.service_package_func_data.memory_extents_num=%d",_SHM_SERVICEPACKAGEFUNC_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.service_package_func_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::servicepackagefunc_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  contentfiltering_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_variable:
		{
			iRet = SHMDataMgr::servicepackagevariable_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_SERVICEPACKAGEVLE_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_SERVICEPACKAGEVLE_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::servicepackagevariable_init();
					int iRet =  SHMDataMgr::createpackagevleSHM(1);

					if (iRet>0) {//创建存储文件context_filter信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.service_package_vle_data.memory_extents_num=%d",_SHM_SERVICEPACKAGEVLE_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.service_package_vle_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::servicepackagevariable_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagevariable_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}

		}
		break;
	case statisticscongestmsg:
		{
			iRet = SHMDataMgr::scongestmsg_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_SCONGESTMSG_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_SCONGESTMSG_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};
					SHMDataMgr::scongestmsg_init();
					int iRet =  SHMDataMgr::createscongestmsgSHM(1);

					if (iRet>0) {
						snprintf(sValue,sizeof(sValue)-1,"memory.scongestmsg_data.memory_extents_num=%d",_SHM_SCONGESTMSG_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.scongestmsg_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::scongestmsg_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  scongestmsg_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}

		}
		break;
	case wf_process_mq:
		{
			iRet = SHMDataMgr::wfprocessmq_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_WFPROCESSMQ_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_WFPROCESSMQ_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};
					SHMDataMgr::wfprocessmq_init();

					int iRet =  SHMDataMgr::createwfprocessmqSHM(1);

					if (iRet>0) {//创建存储表context_filter信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.wfprocessmq_data.memory_extents_num=%d",_SHM_WFPROCESSMQ_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.wfprocessmq_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::scongestmsg_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  scongestmsg_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case portinfo:
		{
			iRet = SHMDataMgr::portInfo_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_PORTINFO_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_PORTINFO_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::portInfo_init();
					int iRet =  SHMDataMgr::createportinfoSHM(1);

					if (iRet>0) {//创建存储表context_filter信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.portinfo_data.memory_extents_num=%d",_SHM_PORTINFO_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.portinfo_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::portInfo_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}

		}
		break;
	case sgw_org_route:
		{
			iRet = SHMDataMgr::sgworgroute_init();

			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_ORGROUTE_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_ORGROUTE_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::portInfo_init();
					int iRet =  SHMDataMgr::createOrgRouteSHM(1);

					if (iRet>0) {//创建存储表sgw_org_route信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.orgroute_data.memory_extents_num=%d",_SHM_ORGROUTE_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.orgroute_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::sgworgroute_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}

		}
		break;
	case sgw_org_head:
		{
			iRet = SHMDataMgr::sgworghead_init();

			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				int iMaxNum =GETVALUE(_SHM_ORGHEAD_EXTENTS_MAX_NUM_,_SHM_PARAM_EXTENTS_MAX_NUM_);

				if (_SHM_ORGHEAD_EXTENTS_NUM_<iMaxNum) {
					SHMCmdSet::unloadoneSHM(_sType);
					char sValue[128] = {0};

					SHMDataMgr::sgworghead_init();
					int iRet =  SHMDataMgr::createOrgHeadSHM(1);

					if (iRet>0) {//创建存储表sgw_org_head信息的共享内存
						snprintf(sValue,sizeof(sValue)-1,"memory.orghead_data.memory_extents_num=%d",_SHM_ORGHEAD_EXTENTS_NUM_+1);
						SHMParamCfg::setValue(sValue);
						memset(sValue,0,sizeof(sValue));
						snprintf(sValue,sizeof(sValue)-1,"memory.orghead_data.init_data_size=%d",iRet);
						SHMParamCfg::setValue(sValue);
					}
					SHMDataMgr::sgworghead_free();
				} else {
					cout<<"错误！共享内存扩充次数已经超过最大限制"<<endl;
					return -1;
				}
			} else {
				//cout<<"[rallocSHM errno]: "<<__FILE__<<":"<<__LINE__<<"  sgworghead_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}

		}
		break;
	default:
		break;
	}
	return iRet;
}

int SHMCmdSet::queryoneSHM(const char *_sType,vector<SHMInfo> &_vShmInfo)
{
	int iRet = 0;
	//_vShmInfo.clear();
	SHMInfo _oShmInfo;
	switch(checkargv(_sType))
	{
	case context_filter:
		{
			if (SHMDataMgr::contentfiltering_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  contentfiltering_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryContentFilteringData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				//SHMDataMgr::queryContentFilteringIndex(_oShmInfo);
				//_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::contentfiltering_free();
		}
		break;
	case tab_msg_map:
		{
			if (SHMDataMgr::msgmap_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  msgmap_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryMsgMapData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryMsgMapIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::msgmap_free();
		}
		break;
	case service_context_list:
		{
			if (SHMDataMgr::servicetxtlist_init()!=0 ) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryServiceTxtListData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryServiceTxtListIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::servicetxtlist_free();
		}
		break;
	case stream_ctrl_info:
		{
			if (SHMDataMgr::streamctrl_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrl_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryStreamCtrlData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryStreamCtrlIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::streamctrl_free();
		}
		break;
	case stream_ctrl_data:
		{
			if (SHMDataMgr::streamctrldata_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrldata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryStreamCtrlDataData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryStreamCtrlDataIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::streamctrldata_free();
		}
		break;
	case userinfo:
		{
			if (SHMDataMgr::userinfo_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  userinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryUserInfoData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryUserInfoIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryUserInfoIndex_A(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::userinfo_free();
		}
		break;
	case sgw_sla_que_rel:
		{
			if (SHMDataMgr::sgwslaquerel_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwslaquerel_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querySgwSlaqueRelData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querySgwSlaqueRelIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::sgwslaquerel_free();
		}
		break;
	case netInfodata:
		{
			if (SHMDataMgr::netinfo_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  netinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryNetData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryNetIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryNetIndex_s(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::netinfo_free();
		}
		break;
	case sessiondata:
		{
			if (SHMDataMgr::session_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  session_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querySessionData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querySessionIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::session_free();
		}
		break;
	case shmcongestlevel:
		{
			if (SHMDataMgr::congestleve_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  congestleve_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryCongestLeveData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::congestleve_free();
		}
		break;
	case service_context_list_base:
		{
			if (SHMDataMgr::servicetxtlistbase_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlistbase_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryServiceTxtListBaseData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryServiceTxtListBaseIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::servicetxtlistbase_free();
		}
		break;
	case loadbalancedata:
		{
			if (SHMDataMgr::loadbalancedata_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  loadbalancedata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryloadbalanceData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryloadbalanceIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::loadbalancedata_free();
		}
		break;
	case service_package_route:
		{
			if (SHMDataMgr::servicepackageroute_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackageroute_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querypackagerouteData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querypackagerouteIndex_a(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querypackagerouteIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::servicepackageroute_free();
		}
		break;
	case base_method:
		{
			if (SHMDataMgr::basemethod_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  basemethod_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querybasemethodData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querybasemethodIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::basemethod_free();
		}
		break;
	case s_packet_session:
		{
			if (SHMDataMgr::packetsession_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  packetsession_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querypacketsessionData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querypacketsessionIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querypacketsubsessionIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::packetsession_free();
		}
		break;
	case service_package_node:
		{
			if (SHMDataMgr::servicepackage_init()!=0) {
				//cout<<"初始化连接loadbalancedata共享内存失败，请检查共享内存是否已经创建！"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryservicepackageData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryservicepackageIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryservicepackageIndex_a(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::servicepackage_free();
		}
		break;
	case service_package_route_ccr:
		{
			if (SHMDataMgr::servicepackagerouteCCR_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagerouteCCR_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryservicepackageCCRData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryservicepackageCCRIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryservicepackageCCRIndex_a(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::servicepackagerouteCCR_free();
		}
		break;
	case service_package_judge:
		{
			if (SHMDataMgr::servicepackagejudge_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagejudge_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querypackagejudgeData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querypackagejudgeIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querypackagejudgeIndex_a(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::servicepackagejudge_free();
		}
		break;
	case service_package_cca:
		{
			if (SHMDataMgr::servicepackagecca_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagecca_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querypackageccaData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querypackageccaIndex_a(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querypackageccaIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::servicepackagecca_free();
		}
		break;
	case service_package_func:
		{
			if (SHMDataMgr::servicepackagefunc_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagefunc_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querypackagefuncData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querypackagefuncIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::servicepackagefunc_free();
		}
		break;
	case service_package_variable:
		{
			if (SHMDataMgr::servicepackagevariable_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagevariable_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querypackagevleData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querypackagevleIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::servicepackagevariable_free();
		}
		break;
	case statisticscongestmsg:
		{
			if (SHMDataMgr::scongestmsg_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  scongestmsg_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryscongestmsgData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryscongestmsgIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::scongestmsg_free();
		}
		break;
	case wf_process_mq:
		{
			if (SHMDataMgr::wfprocessmq_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  wfprocessmq_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querywfprocessmqData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querywfprocessmqIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::wfprocessmq_free();
		}
		break;
	case portinfo:
		{
			if (SHMDataMgr::portInfo_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryportinfoData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryportinfoIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::portInfo_free();
		}
		break;
	case session_clear:
		{
			if (SHMDataMgr::sessionclear_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querySessionClearData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querySessionClearIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::sessionclear_free();
		}
		break;
	case sgw_org_route:
		{
			if (SHMDataMgr::sgworgroute_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  sgworgroute_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryOrgRouteData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryOrgRouteIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::sgworgroute_free();
		}
		break;
	case sgw_org_head:
		{
			if (SHMDataMgr::sgworghead_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  sgworghead_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryOrgHeadData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryOrgHeadIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::sgworghead_free();
		}
		break;
	case channelInfo:
		{
			if (SHMDataMgr::channelinfo_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  channelinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryChannelData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryChannelIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::channelinfo_free();
		}
		break;
	case sgw_global_map:
		{
			if (SHMDataMgr::globalmap_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  globalmap_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryGlobalMapData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryGlobalMapIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::globalmap_free();
		}
		break;
	case sgw_user_staff:
		{
			if (SHMDataMgr::userstaff_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  userstaff_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryUserStaffData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryUserStaffIndex_a(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryUserStaffIndex_b(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryUserStaffIndex_c(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::userstaff_free();
		}
		break;
	case sgw_route_rule:
		{
			if (SHMDataMgr::orgrouterule_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  userstaff_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryRouteRuleData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryRouteRuleIndex_a(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryRouteRuleIndex_b(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::orgrouterule_free();
		}
		break;
	case sgw_area_org:
		{
			if (SHMDataMgr::sgwareaorg_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwareaorg_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryAreaOrgData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryAreaOrgIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::sgwareaorg_free();
		}
		break;
	case sgw_tux_rela_in:
		{
			if (SHMDataMgr::sgwtuxrelain_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwtuxrelain_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryTuxRelaInData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryTuxRelaInIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::sgwtuxrelain_free();
		}
		break;
	case sgw_tux_rela_out:
		{
			if (SHMDataMgr::sgwtuxrelaout_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwtuxrelaout_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryTuxRelaOutData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryTuxRelaOutRelaIDIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryTuxRelaOutIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::sgwtuxrelaout_free();
		}
		break;
	case sgw_svr_list:
		{
			if (SHMDataMgr::sgwsvrlist_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsvrlist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querySvrListData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querySvrListIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querySvrListServicesNameIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::sgwsvrlist_free();
		}
		break;
	case sgw_svrs_param_rela:
		{
			if (SHMDataMgr::sgwsrvsparamrela_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsrvsparamrela_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querySvrsParamRelaData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querySvrsParamRelaIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querySvrsParamRelaMappingIDIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::sgwsrvsparamrela_free();
		}
		break;
	case sgw_svrs_param_list:
		{
			if (SHMDataMgr::sgwsrvsparamlist_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsrvsparamlist_init sgwsrvsparamlsit_init!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querySvrsParamListData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querySvrsParamListIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::sgwsrvsparamlist_free();
		}
		break;
	case sgw_svrs_register:
		{
			if (SHMDataMgr::sgwsrvsregister_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsrvsregister_init sgwsrvsparamlsit_init!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querySvrsRegisterData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querySvrsRegisterIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::sgwsrvsregister_free();
		}
		break;
	case sgw_param_ins_list:
		{
			if (SHMDataMgr::sgwparaminslist_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwparaminslist_init sgwsrvsparamlsit_init!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::queryParamInsListData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryParamInsListIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::queryParamInsListMapIDIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::sgwparaminslist_free();
		}
		break;
	case sgw_enum_area:
		{
			if (SHMDataMgr::sgwenumarea_init()!=0) {
				//cout<<"[query errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwparaminslist_init sgwsrvsparamlsit_init!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::querySgwEnumAreaData(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querySgwEnumAreaIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
				SHMDataMgr::querySgwEnumAreaOrgIDIndex(_oShmInfo);
				_vShmInfo.push_back(_oShmInfo);
			}
			SHMDataMgr::sgwparaminslist_free();
		}
		break;
	default:
		return -1;
		break;
	}
	return iRet;
}

int SHMCmdSet::unloadoneSHM(const char *_sType)
{
	int iRet = 0;

	switch(checkargv(_sType))
	{
	case context_filter:
		{
			SHMDataMgr::contentfiltering_init();
			SHMDataMgr::unloadContentFilteringSHM();//卸载存储表context_filter信息的共享内存
			SHMDataMgr::contentfiltering_free();
		}
		break;
	case tab_msg_map:
		{
			SHMDataMgr::msgmap_init();
			SHMDataMgr::unloadMsgMapSHM();//卸载存储表TAB_MSG_MAP的数据的共享内存
			SHMDataMgr::msgmap_free();
		}
		break;
	case service_context_list:
		{
			SHMDataMgr::servicetxtlist_init();
			SHMDataMgr::unloadServiceTxtListSHM();//卸载存储表service_context_list的数据的共享内存
			SHMDataMgr::servicetxtlist_free();
		}
		break;
	case stream_ctrl_info:
		{
			SHMDataMgr::streamctrl_init();
			SHMDataMgr::unloadStreamCtrlSHM();//卸载存储表STREAM_CTRL_INFO的数据的共享内存
			SHMDataMgr::streamctrl_free();
		}
		break;
	case stream_ctrl_data:
		{
			SHMDataMgr::streamctrldata_init();
			SHMDataMgr::unloadStreamCtrlDataSHM();//卸载共享内存，提供业务进程实时数据存储
			SHMDataMgr::streamctrldata_free();
		}
		break;
	case userinfo:
		{
			SHMDataMgr::userinfo_init();
			SHMDataMgr::unloadUserInfoSHM();//卸载共享内存，提供业务进程实时数据存储
			SHMDataMgr::userinfo_free();
		}
		break;
	case sgw_sla_que_rel:
		{
			SHMDataMgr::sgwslaquerel_init();
			SHMDataMgr::unloadSgwSlaqueRelSHM();
			SHMDataMgr::sgwslaquerel_free();
		}
		break;
	case netInfodata:
		{
			SHMDataMgr::netinfo_init();
			SHMDataMgr::unloadNetSHM();
			SHMDataMgr::netinfo_free();
		}
		break;
	case sessiondata:
		{
			SHMDataMgr::session_init();
			SHMDataMgr::unloadSessionSHM();
			SHMDataMgr::session_free();
		}
		break;
	case shmcongestlevel:
		{
			SHMDataMgr::congestleve_init();
			SHMDataMgr::unloadCongestLeveSHM();
			SHMDataMgr::congestleve_free();
		}
		break;
	case service_context_list_base:
		{
			SHMDataMgr::servicetxtlistbase_init();
			SHMDataMgr::unloadServiceTxtListBaseSHM();
			SHMDataMgr::servicetxtlistbase_free();
		}
		break;
	case loadbalancedata:
		{
			SHMDataMgr::loadbalancedata_init();
			SHMDataMgr::unloadloadbalanceSHM();
			SHMDataMgr::loadbalancedata_free();
		}
		break;
	case service_package_route:
		{
			SHMDataMgr::servicepackageroute_init();
			SHMDataMgr::unloadpackagerouteSHM();
			SHMDataMgr::servicepackageroute_free();
		}
		break;
	case base_method:
		{
			SHMDataMgr::basemethod_init();
			SHMDataMgr::unloadbasemethodSHM();
			SHMDataMgr::basemethod_free();
		}
		break;
	case s_packet_session:
		{
			SHMDataMgr::packetsession_init();
			SHMDataMgr::unloadpacketsessionSHM();
			SHMDataMgr::packetsession_free();
		}
		break;
	case service_package_node:
		{
			SHMDataMgr::servicepackage_init();
			SHMDataMgr::unloadservicepackageSHM();
			SHMDataMgr::servicepackage_free();
		}
		break;
	case service_package_route_ccr:
		{
			SHMDataMgr::servicepackagerouteCCR_init();
			SHMDataMgr::unloadservicepackageCCRSHM();
			SHMDataMgr::servicepackagerouteCCR_free();
		}
		break;
	case service_package_judge:
		{
			SHMDataMgr::servicepackagejudge_init();
			SHMDataMgr::unloadpackagejudgeSHM();
			SHMDataMgr::servicepackagejudge_free();
		}
		break;
	case service_package_cca:
		{
			SHMDataMgr::servicepackagecca_init();
			SHMDataMgr::unloadpackageccaSHM();
			SHMDataMgr::servicepackagecca_free();
		}
		break;
	case service_package_func:
		{
			SHMDataMgr::servicepackagefunc_init();
			SHMDataMgr::unloadpackagefuncSHM();
			SHMDataMgr::servicepackagefunc_free();
		}
		break;
	case service_package_variable:
		{
			SHMDataMgr::servicepackagevariable_init();
			SHMDataMgr::unloadpackagevleSHM();
			SHMDataMgr::servicepackagevariable_free();
		}
		break;
	case statisticscongestmsg:
		{
			SHMDataMgr::scongestmsg_init();
			SHMDataMgr::unloadscongestmsgSHM();
			SHMDataMgr::scongestmsg_free();
		}
		break;
	case wf_process_mq:
		{
			SHMDataMgr::wfprocessmq_init();
			SHMDataMgr::unloadwfprocessmqSHM();
			SHMDataMgr::wfprocessmq_free();
		}
		break;
	case portinfo:
		{
			SHMDataMgr::portInfo_init();
			SHMDataMgr::unloadportinfoSHM();
			SHMDataMgr::portInfo_free();
		}
		break;
	case session_clear:
		{
			SHMDataMgr::sessionclear_init();
			SHMDataMgr::unloadSessionClearSHM();
			SHMDataMgr::sessionclear_free();
		}
		break;
	case sgw_org_route:
		{
			SHMDataMgr::sgworgroute_init();
			SHMDataMgr::unloadOrgRouteSHM();
			SHMDataMgr::sgworgroute_free();
		}
		break;
	case sgw_org_head:
		{
			SHMDataMgr::sgworghead_init();
			SHMDataMgr::unloadOrgHeadSHM();
			SHMDataMgr::sgworghead_free();
		}
		break;
	case channelInfo:
		{
			SHMDataMgr::channelinfo_init();
			SHMDataMgr::unloadChannelSHM();
			SHMDataMgr::channelinfo_free();
		}
		break;
	case sgw_global_map:
		{
			SHMDataMgr::globalmap_init();
			SHMDataMgr::unloadGlobalMapSHM();
			SHMDataMgr::globalmap_free();
		}
		break;
	case sgw_user_staff:
		{
			SHMDataMgr::userstaff_init();
			SHMDataMgr::unloadUserStaffSHM();
			SHMDataMgr::userstaff_free();
		}
		break;
	case sgw_route_rule:
		{
			SHMDataMgr::orgrouterule_init();
			SHMDataMgr::unloadRouteRuleSHM();
			SHMDataMgr::orgrouterule_free();
		}
		break;
	case sgw_area_org:
		{
			SHMDataMgr::sgwareaorg_init();
			SHMDataMgr::unloadAreaOrgSHM();
			SHMDataMgr::sgwareaorg_free();
		}
		break;
	case sgw_tux_rela_in:
		{
			SHMDataMgr::sgwtuxrelain_init();
			SHMDataMgr::unloadTuxRelaInSHM();
			SHMDataMgr::sgwtuxrelain_free();
		}
		break;
	case sgw_tux_rela_out:
		{
			SHMDataMgr::sgwtuxrelaout_init();
			SHMDataMgr::unloadTuxRelaOutSHM();
			SHMDataMgr::sgwtuxrelaout_free();
		}
		break;
	case sgw_svr_list:
		{
			SHMDataMgr::sgwsvrlist_init();
			SHMDataMgr::unloadSvrListSHM();
			SHMDataMgr::sgwsvrlist_free();
		}
		break;
	case sgw_svrs_param_rela:
		{
			SHMDataMgr::sgwsrvsparamrela_init();
			SHMDataMgr::unloadSvrsParamRelaSHM();
			SHMDataMgr::sgwsrvsparamrela_free();
		}
		break;
	case sgw_svrs_param_list:
		{
			SHMDataMgr::sgwsrvsparamlist_init();
			SHMDataMgr::unloadSvrsParamListSHM();
			SHMDataMgr::sgwsrvsparamlist_free();
		}
		break;
	case sgw_svrs_register:
		{
			SHMDataMgr::sgwsrvsregister_init();
			SHMDataMgr::unloadSvrsRegisterSHM();
			SHMDataMgr::sgwsrvsregister_free();
		}
		break;
	case sgw_param_ins_list:
		{
			SHMDataMgr::sgwparaminslist_init();
			SHMDataMgr::unloadParamInsListSHM();
			SHMDataMgr::sgwparaminslist_free();
		}
		break;
	case sgw_enum_area:
		{
			SHMDataMgr::sgwenumarea_init();
			SHMDataMgr::unloadSgwEnumAreaSHM();
			SHMDataMgr::sgwenumarea_free();
		}
		break;
	default:
		return -1;
		break;
	}
	cout<<_sType<<"共享内存卸载完成,可通过sgwshm -stat 查询其状态……"<<endl<<endl;
	return 0;
}

int SHMCmdSet::clearoneSHM(const char *_sType)
{
	int iRet = 0;

	switch(checkargv(_sType))
	{
	case context_filter:
		{
			iRet = SHMDataMgr::contentfiltering_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  contentfiltering_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearContentFilteringSHM();//创建存储表context_filter信息的共享内存
				SHMDataMgr::contentfiltering_free();
			}
		}
		break;
	case tab_msg_map:
		{
			iRet= SHMDataMgr::msgmap_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  msgmap_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearMsgMapSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::msgmap_free();
			}
		}
		break;
	case service_context_list:
		{
			iRet= SHMDataMgr::servicetxtlist_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearServiceTxtListSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::servicetxtlist_free();
			}
		}
		break;
	case stream_ctrl_info:
		{
			iRet= SHMDataMgr::streamctrl_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrl_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearStreamCtrlSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::streamctrl_free();
			}
		}
		break;
	case stream_ctrl_data:
		{
			iRet= SHMDataMgr::streamctrldata_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrldata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearStreamCtrlDataSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::streamctrldata_free();
			}
		}
		break;
	case userinfo:
		{
			iRet= SHMDataMgr::userinfo_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  userinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearUserInfoSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::userinfo_free();
			}
		}
		break;
	case sgw_sla_que_rel:
		{
			iRet= SHMDataMgr::sgwslaquerel_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwslaquerel_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearSgwSlaqueRelSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::sgwslaquerel_free();
			}
		}
		break;
	case netInfodata:
		{
			iRet= SHMDataMgr::netinfo_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  netinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearNetSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::netinfo_free();
			}
		}
		break;
	case sessiondata:
		{
			iRet= SHMDataMgr::session_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  session_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearSessionSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::session_free();
			}
		}
		break;
	case shmcongestlevel:
		{
			iRet= SHMDataMgr::congestleve_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  congestleve_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearCongestLeveSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::congestleve_free();
			}
		}
		break;
	case service_context_list_base:
		{
			iRet= SHMDataMgr::servicetxtlistbase_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlistbase_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearServiceTxtListBaseSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::servicetxtlistbase_free();
			}
		}
		break;
	case loadbalancedata:
		{
			iRet= SHMDataMgr::loadbalancedata_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  loadbalancedata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearloadbalanceSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::loadbalancedata_free();
			}
		}
		break;
	case service_package_route:
		{
			iRet= SHMDataMgr::servicepackageroute_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackageroute_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearpackagerouteSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::servicepackageroute_free();
			}
		}
		break;
	case base_method:
		{
			iRet= SHMDataMgr::basemethod_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  basemethod_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearbasemethodSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::basemethod_free();
			}

		}
		break;
	case s_packet_session:
		{
			iRet= SHMDataMgr::packetsession_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  packetsession_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearpacketsessionSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::packetsession_free();
			}
		}
		break;
	case service_package_node:
		{
			iRet= SHMDataMgr::servicepackage_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackage_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearservicepackageSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::servicepackage_free();
			}
		}
		break;
	case service_package_route_ccr:
		{
			iRet= SHMDataMgr::servicepackagerouteCCR_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagerouteCCR_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearservicepackageCCRSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::servicepackagerouteCCR_free();
			}
		}
		break;
	case service_package_judge:
		{
			iRet= SHMDataMgr::servicepackagejudge_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagejudge_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearpackagejudgeSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::servicepackagejudge_free();
			}
		}
		break;
	case service_package_cca:
		{
			iRet= SHMDataMgr::servicepackagecca_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagecca_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearpackageccaSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::servicepackagejudge_free();
			}
		}
		break;
	case service_package_func:
		{
			iRet= SHMDataMgr::servicepackagefunc_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagefunc_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearpackagefuncSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::servicepackagefunc_free();
			}
		}
		break;
	case service_package_variable:
		{
			iRet= SHMDataMgr::servicepackagevariable_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagevariable_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearpackagevleSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::servicepackagevariable_free();
			}
		}
		break;
	case statisticscongestmsg:
		{
			iRet= SHMDataMgr::scongestmsg_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  scongestmsg_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearscongestmsgSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::scongestmsg_free();
			}
		}
		break;
	case wf_process_mq:
		{
			iRet= SHMDataMgr::wfprocessmq_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  wfprocessmq_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearwfprocessmqSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::wfprocessmq_free();
			}
		}
		break;
	case portinfo:
		{
			iRet= SHMDataMgr::portInfo_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearportinfoSHM();//创建存储表TAB_MSG_MAP的数据的共享内存
				SHMDataMgr::portInfo_free();
			}
		}
		break;
	case session_clear:
		{
			iRet= SHMDataMgr::sessionclear_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearSessionClearSHM();
				SHMDataMgr::sessionclear_free();
			}
		}
		break;
	case sgw_org_route:
		{
			iRet= SHMDataMgr::sgworgroute_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  sgworgroute_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearOrgRouteSHM();
				SHMDataMgr::sgworgroute_free();
			}
		}
		break;
	case sgw_org_head:
		{
			iRet= SHMDataMgr::sgworghead_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  sgworghead_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearOrgHeadSHM();
				SHMDataMgr::sgworghead_free();
			}
		}
		break;
	case channelInfo:
		{
			iRet= SHMDataMgr::channelinfo_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  channelinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearChannelSHM();
				SHMDataMgr::channelinfo_free();
			}
		}
		break;
	case sgw_global_map:
		{
			iRet= SHMDataMgr::globalmap_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  globalmap_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearGlobalMapSHM();
				SHMDataMgr::globalmap_free();
			}
		}
		break;
	case sgw_user_staff:
		{
			iRet= SHMDataMgr::userstaff_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  userstaff_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearUserStaffSHM();
				SHMDataMgr::userstaff_free();
			}
		}
		break;
	case sgw_route_rule:
		{
			iRet= SHMDataMgr::orgrouterule_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  orgrouterule_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearRouteRuleSHM();
				SHMDataMgr::orgrouterule_free();
			}
		}
		break;
	case sgw_area_org:
		{
			iRet= SHMDataMgr::sgwareaorg_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwareaorg_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearAreaOrgSHM();
				SHMDataMgr::sgwareaorg_free();
			}
		}
		break;
	case sgw_tux_rela_in:
		{
			iRet= SHMDataMgr::sgwtuxrelain_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwtuxrelain_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearTuxRelaInSHM();
				SHMDataMgr::sgwtuxrelain_free();
			}
		}
		break;
	case sgw_tux_rela_out:
		{
			iRet= SHMDataMgr::sgwtuxrelaout_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwtuxrelaout_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearTuxRelaOutSHM();
				SHMDataMgr::sgwtuxrelaout_free();
			}
		}
		break;
	case sgw_svr_list:
		{
			iRet= SHMDataMgr::sgwsvrlist_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsvrlist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearSvrListSHM();
				SHMDataMgr::sgwsvrlist_free();
			}
		}
		break;
	case sgw_svrs_param_rela:
		{
			iRet= SHMDataMgr::sgwsrvsparamrela_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsrvsparamrela_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearSvrsParamRelaSHM();
				SHMDataMgr::sgwsrvsparamrela_init();
			}
		}
		break;
	case sgw_svrs_param_list:
		{
			iRet= SHMDataMgr::sgwsrvsparamlist_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsrvsparamlist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearSvrsParamListSHM();
				SHMDataMgr::sgwsrvsparamlist_free();
			}
		}
		break;
	case sgw_svrs_register:
		{
			iRet= SHMDataMgr::sgwsrvsregister_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsrvsregister_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearSvrsRegisterSHM();
				SHMDataMgr::sgwsrvsregister_free();
			}
		}
		break;
	case sgw_param_ins_list:
		{
			iRet= SHMDataMgr::sgwparaminslist_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwparaminslist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearParamInsListSHM();
				SHMDataMgr::sgwparaminslist_free();
			}
		}
		break;
	case sgw_enum_area:
		{
			iRet= SHMDataMgr::sgwenumarea_init();
			if (iRet) {
				//cout<<"[clear errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwparaminslist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearSgwEnumAreaSHM();
				SHMDataMgr::sgwenumarea_free();
			}
		}
		break;
	default:
		return -1;
		break;
	}
	cout<<_sType<<"共享内存清理成功,可通过sgwshm -stat 查询其状态……"<<endl<<endl;
	return iRet;
}

int SHMCmdSet::detachoneSHM(const char *_sType)
{
	int iRet = 0;

	switch(checkargv(_sType))
	{
	case context_filter:
		{
			iRet= SHMDataMgr::contentfiltering_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  contentfiltering_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachContentFiltering();
				SHMDataMgr::contentfiltering_free();
			}
		}
		break;
	case tab_msg_map:
		{
			iRet= SHMDataMgr::msgmap_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  msgmap_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachMsgMap();
				SHMDataMgr::msgmap_free();
			}
		}
		break;
	case service_context_list:
		{
			iRet= SHMDataMgr::servicetxtlist_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachServiceTxtList();
				SHMDataMgr::servicetxtlist_free();
			}
		}
		break;
	case stream_ctrl_info:
		{
			iRet= SHMDataMgr::streamctrl_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrl_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachStreamCtrl();
				SHMDataMgr::streamctrl_free();
			}
		}
		break;
	case stream_ctrl_data:
		{
			iRet= SHMDataMgr::streamctrldata_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrldata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachStreamCtrlData();
				SHMDataMgr::streamctrldata_free();
			}
		}
		break;
	case userinfo:
		{
			iRet= SHMDataMgr::userinfo_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  userinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachUserInfo();
				SHMDataMgr::userinfo_free();
			}
		}
		break;
	case sgw_sla_que_rel:
		{
			iRet= SHMDataMgr::sgwslaquerel_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwslaquerel_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachSgwSlaqueRel();
				SHMDataMgr::sgwslaquerel_free();
			}
		}
		break;
	case netInfodata:
		{
			iRet= SHMDataMgr::netinfo_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  netinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachNet();
				SHMDataMgr::netinfo_free();
			}
		}
		break;
	case sessiondata:
		{
			iRet= SHMDataMgr::session_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  session_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachSession();
				SHMDataMgr::session_free();
			}
		}
		break;
	case shmcongestlevel:
		{
			iRet= SHMDataMgr::congestleve_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  congestleve_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachCongestLeve();
				SHMDataMgr::congestleve_free();
			}
		}
		break;
	case service_context_list_base:
		{
			iRet= SHMDataMgr::servicetxtlistbase_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlistbase_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachServiceTxtListBase();
				SHMDataMgr::servicetxtlistbase_free();
			}
		}
		break;
	case loadbalancedata:
		{
			iRet= SHMDataMgr::loadbalancedata_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  loadbalancedata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachloadbalance();
				SHMDataMgr::loadbalancedata_free();
			}
		}
		break;
	case service_package_route:
		{
			iRet= SHMDataMgr::servicepackageroute_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackageroute_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachpackageroute();
				SHMDataMgr::servicepackageroute_free();
			}
		}
		break;
	case base_method:
		{
			iRet= SHMDataMgr::basemethod_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  basemethod_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachbasemethod();
				SHMDataMgr::basemethod_free();
			}
		}
		break;
	case s_packet_session:
		{
			iRet= SHMDataMgr::packetsession_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  packetsession_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachpacketsession();
				SHMDataMgr::packetsession_free();
			}
		}
		break;
	case service_package_node:
		{
			iRet= SHMDataMgr::servicepackage_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackage_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachservicepackage();
				SHMDataMgr::servicepackage_free();
			}
		}
		break;
	case service_package_route_ccr:
		{
			iRet= SHMDataMgr::servicepackagerouteCCR_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagerouteCCR_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachservicepackageCCR();
				SHMDataMgr::servicepackagerouteCCR_free();
			}
		}
		break;
	case service_package_judge:
		{
			iRet= SHMDataMgr::servicepackagejudge_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagejudge_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachpackagejudge();
				SHMDataMgr::servicepackagejudge_free();
			}
		}
		break;
	case service_package_cca:
		{
			iRet= SHMDataMgr::servicepackagecca_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagecca_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachpackagecca();
				SHMDataMgr::servicepackagecca_free();
			}
		}
		break;
	case service_package_func:
		{
			iRet= SHMDataMgr::servicepackagefunc_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagefunc_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachpackagefunc();
				SHMDataMgr::servicepackagefunc_free();
			}
		}
		break;
	case service_package_variable:
		{
			iRet= SHMDataMgr::servicepackagevariable_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagevariable_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachpackagevle();
				SHMDataMgr::servicepackagevariable_free();
			}
		}
		break;
	case statisticscongestmsg:
		{
			iRet= SHMDataMgr::scongestmsg_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  scongestmsg_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachscongestmsg();
				SHMDataMgr::scongestmsg_free();
			}
		}
		break;
	case wf_process_mq:
		{
			iRet= SHMDataMgr::wfprocessmq_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  wfprocessmq_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachwfprocessmq();
				SHMDataMgr::wfprocessmq_free();
			}
		}
		break;
	case portinfo:
		{
			iRet= SHMDataMgr::portInfo_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachportinfo();
				SHMDataMgr::portInfo_free();
			}
		}
		break;
	case sgw_org_route:
		{
			iRet= SHMDataMgr::sgworgroute_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachOrgRouteSHM();
				SHMDataMgr::sgworgroute_free();
			}
		}
		break;
	case sgw_org_head:
		{
			iRet= SHMDataMgr::sgworghead_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachOrgHeadSHM();
				SHMDataMgr::sgworghead_free();
			}
		}
		break;
	case channelInfo:
		{
			iRet= SHMDataMgr::channelinfo_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachChannelSHM();
				SHMDataMgr::channelinfo_free();
			}
		}
		break;
	case sgw_global_map:
		{
			iRet= SHMDataMgr::globalmap_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachGlobalMapSHM();
				SHMDataMgr::globalmap_free();
			}
		}
		break;
	case sgw_user_staff:
		{
			iRet= SHMDataMgr::orgrouterule_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachUserStaffSHM();
				SHMDataMgr::userstaff_free();
			}
		}
		break;
	case sgw_route_rule:
		{
			iRet= SHMDataMgr::orgrouterule_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachRouteRuleSHM();
				SHMDataMgr::orgrouterule_free();
			}
		}
		break;
	case sgw_area_org:
		{
			iRet= SHMDataMgr::sgwareaorg_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachAreaOrgSHM();
				SHMDataMgr::sgwareaorg_free();
			}
		}
		break;
	case sgw_tux_rela_in:
		{
			iRet= SHMDataMgr::sgwtuxrelain_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachTuxRelaInSHM();
				SHMDataMgr::sgwtuxrelain_free();
			}
		}
		break;
	case sgw_tux_rela_out:
		{
			iRet= SHMDataMgr::sgwtuxrelaout_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachTuxRelaOutSHM();
				SHMDataMgr::sgwtuxrelaout_free();
			}
		}
		break;
	case sgw_svr_list:
		{
			iRet= SHMDataMgr::sgwsvrlist_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachSvrListSHM();
				SHMDataMgr::sgwsvrlist_free();
			}
		}
		break;
	case sgw_svrs_param_rela:
		{
			iRet= SHMDataMgr::sgwsrvsparamrela_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachSvrsParamRelaSHM();
				SHMDataMgr::sgwsrvsparamrela_free();
			}
		}
		break;
	case sgw_svrs_param_list:
		{
			iRet= SHMDataMgr::sgwsrvsparamlist_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachSvrsParamListSHM();
				SHMDataMgr::sgwsrvsparamlist_free();
			}
		}
		break;
	case sgw_svrs_register:
		{
			iRet= SHMDataMgr::sgwsrvsregister_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachSvrsRegisterSHM();
				SHMDataMgr::sgwsrvsregister_free();
			}
		}
		break;
	case sgw_param_ins_list:
		{
			iRet= SHMDataMgr::sgwparaminslist_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachParamInsListSHM();
				SHMDataMgr::sgwparaminslist_free();
			}
		}
		break;
	case sgw_enum_area:
		{
			iRet= SHMDataMgr::sgwenumarea_init();
			if (iRet) {
				//cout<<"[detach errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::detachSgwEnumAreaSHM();
				SHMDataMgr::sgwenumarea_free();
			}
		}
		break;
	default:
		break;
	}
	return iRet;
}

int SHMCmdSet::loadoneSHM(const char * _sType,const char * _sFileName)
{
	int iRet= 0;

	switch(checkargv(_sType))
	{
	case context_filter:
		{
			iRet= SHMDataMgr::contentfiltering_init();
			if (iRet) {
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  contentfiltering_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearContentFilteringSHM();
				SHMDataMgr::loadContentFiltering();
				SHMDataMgr::contentfiltering_free();
			}
		}
		break;
	case tab_msg_map:
		{
			iRet= SHMDataMgr::msgmap_init();
			if (iRet) {
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  msgmap_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearMsgMapSHM();
				SHMDataMgr::loadMsgMap();
				SHMDataMgr::msgmap_free();
			}
		}
		break;
	case service_context_list:
		{
			iRet= SHMDataMgr::servicetxtlist_init();
			if (iRet) {
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearServiceTxtListSHM();
				SHMDataMgr::loadServiceTxtList();
				SHMDataMgr::servicetxtlist_free();
			}
		}
		break;
	case stream_ctrl_info:
		{
			iRet= SHMDataMgr::streamctrl_init();
			if (iRet) {
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrl_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearStreamCtrlSHM();
				SHMDataMgr::loadStreamCtrl();
				SHMDataMgr::streamctrl_free();
			}
		}
		break;
	case userinfo:
		{
			iRet= SHMDataMgr::userinfo_init();
			if (iRet) {
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  userinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearUserInfoSHM();
				SHMDataMgr::loadUserInfo();
				SHMDataMgr::userinfo_free();
			}
		}
		break;
	case sgw_sla_que_rel:
		{
			iRet= SHMDataMgr::sgwslaquerel_init();
			if (iRet) {
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwslaquerel_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearSgwSlaqueRelSHM();
				SHMDataMgr::loadSgwSlaqueRel();
				SHMDataMgr::sgwslaquerel_free();
			}
		}
		break;
	case netInfodata:
		{
			iRet= SHMDataMgr::netinfo_init();
			if (iRet) {
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  netinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearNetSHM();
				SHMDataMgr::loadNet();
				SHMDataMgr::netinfo_free();
			}
		}
		break;
	case loadbalancedata:
		{
			iRet= SHMDataMgr::loadbalancedata_init();
			if (iRet) {
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  loadbalancedata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearloadbalanceSHM();
				SHMDataMgr::loadBalance();
				SHMDataMgr::loadbalancedata_free();
			}
			break;
		}
	case stream_ctrl_data:
		{
			if (SHMDataMgr::streamctrldata_init()) {
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrldata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearStreamCtrlDataSHM();
				SHMDataMgr::loadStreamCtrlData();
				SHMDataMgr::streamctrldata_free();
			}
			break;

		}
	case sessiondata:
	case shmcongestlevel:
	case s_packet_session:
	case statisticscongestmsg:
	case portinfo:
	case session_clear:
		{
			break;
		}
	case service_context_list_base:
		{
			iRet= SHMDataMgr::servicetxtlistbase_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlistbase_init failed!"<<endl;
			} else {
				SHMDataMgr::clearServiceTxtListBaseSHM();
				SHMDataMgr::loadServiceTxtListBase();
				SHMDataMgr::servicetxtlistbase_free();
			}
		}
		break;
	case service_package_route:
		{
			iRet= SHMDataMgr::servicepackageroute_init();
			if (iRet) {
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackageroute_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				SHMDataMgr::clearpackagerouteSHM();
				SHMDataMgr::loadpackageroute();
				SHMDataMgr::servicepackageroute_free();
			}
		}
		break;
	case base_method:
		{
			iRet= SHMDataMgr::basemethod_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  basemethod_init failed!"<<endl;
			} else {
				SHMDataMgr::clearbasemethodSHM();
				SHMDataMgr::loadbasemethod();
				SHMDataMgr::basemethod_free();
			}
		}
		break;
	case service_package_node:
		{
			iRet= SHMDataMgr::servicepackage_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackage_init failed!"<<endl;
			} else {
				SHMDataMgr::clearservicepackageSHM();
				SHMDataMgr::loadservicepackage();
				SHMDataMgr::servicepackage_free();
			}
		}
		break;
	case service_package_route_ccr:
		{
			iRet= SHMDataMgr::servicepackagerouteCCR_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagerouteCCR_init failed!"<<endl;
			} else {
				SHMDataMgr::clearservicepackageCCRSHM();
				SHMDataMgr::loadservicepackageCCR();
				SHMDataMgr::servicepackagerouteCCR_free();
			}
		}
		break;
	case service_package_judge:
		{
			iRet= SHMDataMgr::servicepackagejudge_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagejudge_init failed!"<<endl;
			} else {
				SHMDataMgr::clearpackagejudgeSHM();
				SHMDataMgr::loadpackagejudge();
				SHMDataMgr::servicepackagejudge_free();
			}
		}
		break;
	case service_package_cca:
		{
			iRet= SHMDataMgr::servicepackagecca_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagecca_init failed!"<<endl;
			} else {
				SHMDataMgr::clearpackageccaSHM();
				SHMDataMgr::loadpackagecca();
				SHMDataMgr::servicepackagecca_free();
			}
		}
		break;
	case service_package_func:
		{
			iRet= SHMDataMgr::servicepackagefunc_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagefunc_init failed!"<<endl;
			} else {
				SHMDataMgr::clearpackagefuncSHM();
				SHMDataMgr::loadpackagefunc();
				SHMDataMgr::servicepackagefunc_free();
			}
		}
		break;
	case service_package_variable:
		{
			iRet= SHMDataMgr::servicepackagevariable_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagevariable_init failed!"<<endl;
			} else {
				SHMDataMgr::clearpackagevleSHM();
				SHMDataMgr::loadpackagevle();
				SHMDataMgr::servicepackagevariable_free();
			}
		}
		break;
	case wf_process_mq:
		{
			iRet= SHMDataMgr::wfprocessmq_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  wfprocessmq_init failed!"<<endl;
			} else {
				SHMDataMgr::clearwfprocessmqSHM();
				SHMDataMgr::loadwfprocessmq();
				SHMDataMgr::wfprocessmq_free();
			}
		}
		break;
	case sgw_org_route:
		{
			iRet= SHMDataMgr::sgworgroute_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  sgworgroute_init failed!"<<endl;
			} else {
				SHMDataMgr::clearOrgRouteSHM();
				SHMDataMgr::loadOrgRouteData();
				SHMDataMgr::sgworgroute_free();
			}
		}
		break;
	case sgw_org_head:
		{
			iRet= SHMDataMgr::sgworghead_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  sgworghead_init failed!"<<endl;
			} else {
				SHMDataMgr::clearOrgHeadSHM();
				SHMDataMgr::loadOrgHeadData();
				SHMDataMgr::sgworghead_free();
			}
		}
		break;
	case channelInfo:
		{
			iRet= SHMDataMgr::channelinfo_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  channelinfo_init failed!"<<endl;
			} else {
				SHMDataMgr::clearChannelSHM();
				SHMDataMgr::loadChannelData();
				SHMDataMgr::channelinfo_free();
			}
		}
		break;
	case sgw_global_map:
		{
			iRet= SHMDataMgr::globalmap_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  globalmap_init failed!"<<endl;
			} else {
				SHMDataMgr::clearGlobalMapSHM();
				SHMDataMgr::loadGlobalMapData();
				SHMDataMgr::globalmap_free();
			}
		}
		break;
	case sgw_user_staff:
		{
			iRet= SHMDataMgr::userstaff_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  userstaff_init failed!"<<endl;
			} else {
				SHMDataMgr::clearUserStaffSHM();
				SHMDataMgr::loadUserStaffData();
				SHMDataMgr::userstaff_free();
			}
		}
		break;
	case sgw_route_rule:
		{
			iRet= SHMDataMgr::orgrouterule_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  orgrouterule_init failed!"<<endl;
			} else {
				SHMDataMgr::clearRouteRuleSHM();
				SHMDataMgr::loadRouteRuleData();
				SHMDataMgr::orgrouterule_free();
			}
		}
		break;
	case sgw_area_org:
		{
			iRet= SHMDataMgr::sgwareaorg_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwareaorg_init failed!"<<endl;
			} else {
				SHMDataMgr::clearAreaOrgSHM();
				SHMDataMgr::loadAreaOrgData();
				SHMDataMgr::sgwareaorg_free();
			}
		}
		break;
	case sgw_tux_rela_in:
		{
			iRet= SHMDataMgr::sgwtuxrelain_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwtuxrelain_init failed!"<<endl;
			} else {
				SHMDataMgr::clearTuxRelaInSHM();
				SHMDataMgr::loadTuxRelaInData();
				SHMDataMgr::sgwtuxrelain_free();
			}
		}
		break;
	case sgw_tux_rela_out:
		{
			iRet= SHMDataMgr::sgwtuxrelaout_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwtuxrelaout_init failed!"<<endl;
			} else {
				SHMDataMgr::clearTuxRelaOutSHM();
				SHMDataMgr::loadTuxRelaOutData();
				SHMDataMgr::sgwtuxrelaout_free();
			}
		}
		break;
	case sgw_svr_list:
		{
			iRet= SHMDataMgr::sgwsvrlist_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsvrlist_init failed!"<<endl;
			} else {
				SHMDataMgr::clearSvrListSHM();
				SHMDataMgr::loadSvrListData();
				SHMDataMgr::sgwsvrlist_free();
			}
		}
		break;
	case sgw_svrs_param_rela:
		{
			iRet= SHMDataMgr::sgwsrvsparamrela_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsrvsparamrela_init failed!"<<endl;
			} else {
				SHMDataMgr::clearSvrsParamRelaSHM();
				SHMDataMgr::loadSvrsParamRelaData();
				SHMDataMgr::sgwsrvsparamrela_init();
			}
		}
		break;
	case sgw_svrs_param_list:
		{
			iRet= SHMDataMgr::sgwsrvsparamlist_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsrvsparamlist_init failed!"<<endl;
			} else {
				SHMDataMgr::clearSvrsParamListSHM();
				SHMDataMgr::loadSvrsParamListData();
				SHMDataMgr::sgwsrvsparamlist_free();
			}
		}
		break;
	case sgw_svrs_register:
		{
			iRet= SHMDataMgr::sgwsrvsregister_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwsrvsregister_init failed!"<<endl;
			} else {
				SHMDataMgr::clearSvrsRegisterSHM();
				SHMDataMgr::loadSvrsRegisterData();
				SHMDataMgr::sgwsrvsregister_free();
			}
		}
		break;
	case sgw_param_ins_list:
		{
			iRet= SHMDataMgr::sgwparaminslist_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwparaminslist_init failed!"<<endl;
			} else {
				SHMDataMgr::clearParamInsListSHM();
				SHMDataMgr::loadParamInsListData();
				SHMDataMgr::sgwparaminslist_free();
			}
		}
		break;
	case sgw_enum_area:
		{
			iRet= SHMDataMgr::sgwenumarea_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[load errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwparaminslist_init failed!"<<endl;
			} else {
				SHMDataMgr::clearSgwEnumAreaSHM();
				SHMDataMgr::loadSgwEnumAreaData();
				SHMDataMgr::sgwenumarea_free();
			}
		}
		break;
	default:
		return -1;
		break;
	}
	cout<<_sType<<"共享内存上载完成,可通过sgwshm -stat 查询其状态……"<<endl<<endl;
	return iRet;
}

int SHMCmdSet::insertoneSHM(const char *_sType)
{
	int iRet= 0;
	switch(checkargv(_sType))
	{
	case stream_ctrl_data:
		{
			iRet= SHMStreamCtrlData::streamctrldata_init();
			if (iRet) {
				//cout<<"[insert errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrldata_init failed!"<<endl;
			} else {
				StreamControlData tStreamCtrlData;
				memset(&tStreamCtrlData,0,sizeof(tStreamCtrlData));
				cout<<"请输入数据："<<endl;
				cout<<"tStreamCtrlData.m_sUserName=";
				cin>>tStreamCtrlData.m_sUserName;
				cout<<"tStreamCtrlData.m_sSerConId=";
				cin>>tStreamCtrlData.m_sSerConId;
				cout<<"tStreamCtrlData.m_lSunFlow=";
				cin>>tStreamCtrlData.m_lSunFlow;
				cout<<"tStreamCtrlData.m_iSumNumber=";
				cin>>tStreamCtrlData.m_iSumNumber;
				cout<<"tStreamCtrlData.m_sStartDate=";
				cin>>tStreamCtrlData.m_sStartDate;
				cout<<"tStreamCtrlData.m_sEndDate=";
				cin>>tStreamCtrlData.m_sEndDate;
				cout<<"tStreamCtrlData.m_iSate=";
				cin>>tStreamCtrlData.m_iSate;
				cout<<"tStreamCtrlData.m_iWarned=";
				cin>>tStreamCtrlData.m_iWarned;
				cout<<"tStreamCtrlData.m_iDisConnect=";
				cin>>tStreamCtrlData.m_iDisConnect;
				SHMStreamCtrlData::AddStreamCtrlData(tStreamCtrlData);
				SHMStreamCtrlData::streamctrldata_free();
			}
			break;
		}
	case sessiondata:
		{
			iRet= SHMSessionData::session_init();
			if (iRet) {
				//cout<<"[insert errno]: "<<__FILE__<<":"<<__LINE__<<"  session_init failed!"<<endl;
			} else {
				SessionData tSessionData;
				memset(&tSessionData,0,sizeof(SessionData));
				cout<<"请输入数据："<<endl;
				cout<<"tSessionData.m_iRequestType=";
				cin>>tSessionData.m_iRequestType;
				cout<<"tSessionData.m_iRequestNumber=";
				cin>>tSessionData.m_iRequestNumber;
				cout<<"tSessionData.m_iAuthApplicationId=";
				cin>>tSessionData.m_iAuthApplicationId;
				cout<<"tSessionData.m_iRequestedAction=";
				cin>>tSessionData.m_iRequestedAction;
				cout<<"tSessionData.m_sSessionId=";
				cin>>tSessionData.m_sSessionId;
				cout<<"tSessionData.m_sOriginHost=";
				cin>>tSessionData.m_sOriginHost;
				cout<<"tSessionData.m_sOriginRealm=";
				cin>>tSessionData.m_sOriginRealm;
				cout<<"tSessionData.m_sDestinationHost=";
				cin>>tSessionData.m_sDestinationHost;

				cout<<"tSessionData.m_sDestinationRealm=";
				cin>>tSessionData.m_sDestinationRealm;
				cout<<"tSessionData.m_sServiceContextId=";
				cin>>tSessionData.m_sServiceContextId;
				cout<<"tSessionData.m_DateTime=";
				cin>>tSessionData.m_DateTime;
				//cout<<"tSessionData.m_szUID=";
				//cin>>tSessionData.m_szUID;
				cout<<"tSessionData.m_iSocket=";
				cin>>tSessionData.m_iSocket;
				SHMSessionData::AddSessionData(tSessionData);
				SHMSessionData::session_free();
			}
			break;
		}
	case shmcongestlevel:
		{
			iRet= SHMCongestLevel::congestleve_init();
			if (iRet) {
				//cout<<"[insert errno]: "<<__FILE__<<":"<<__LINE__<<"  congestleve_init failed!"<<endl;
			} else {
				CongestLevel sShmCongestLevel;
				memset(&sShmCongestLevel,0,sizeof(CongestLevel));
				cout<<"请输入数据："<<endl;
				cout<<"sShmCongestLevel.m_iLevel=";
				cin>>sShmCongestLevel.m_iLevel;
				cout<<"sShmCongestLevel.m_irawLevel=";
				cin>>sShmCongestLevel.m_irawLevel;
				cout<<"sShmCongestLevel.m_iGeneMq=";
				cin>>sShmCongestLevel.m_iGeneMq;
				cout<<"sShmCongestLevel.m_iGeneCpu=";
				cin>>sShmCongestLevel.m_iGeneCpu;
				cout<<"sShmCongestLevel.m_iGeneMem=";
				cin>>sShmCongestLevel.m_iGeneMem;
				cout<<"sShmCongestLevel.m_iLimitSpeed=";
				cin>>sShmCongestLevel.m_iLimitSpeed;
				cout<<"sShmCongestLevel.m_sCreateTime=";
				cin>>sShmCongestLevel.m_sCreateTime;
				cout<<"sShmCongestLevel.m_iConfirmFlag=";
				cin>>sShmCongestLevel.m_iConfirmFlag;
				SHMCongestLevel::UpdateShmCongestLevel(sShmCongestLevel);
				SHMCongestLevel::congestleve_free();
			}
			break;
		}
	case s_packet_session:
		{
			break;
		}
	case statisticscongestmsg:
		{
			break;
		}
	default:
		break;
	}
	return iRet;
}

//将某一块共享内存上锁
int SHMCmdSet::lockoneSHM(const char *_sType)
{
	int iRet= 0;

	switch(checkargv(_sType))
	{
	case context_filter:
		{
			iRet= SHMContentFiltering::contentfiltering_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  contentfiltering_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMContentFiltering::contentFilter_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMContentFiltering::contentFilter_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case tab_msg_map:
		{
			iRet= SHMMsgMapInfo::msgmap_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  msgmap_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMMsgMapInfo::msgmap_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMMsgMapInfo::msgmap_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case service_context_list:
		{
			iRet= SHMServiceTxtList::servicetxtlist_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMServiceTxtList::servicetxtlist_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMServiceTxtList::servicetxtlist_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case stream_ctrl_info:
		{
			iRet= SHMStreamCtrlInfo::streamctrl_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrl_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMStreamCtrlInfo::streamctl_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMStreamCtrlInfo::streamctl_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case stream_ctrl_data:
		{
			iRet= SHMStreamCtrlData::streamctrldata_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrldata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMStreamCtrlData::streamctldata_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMStreamCtrlData::streamctldata_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
			break;
		}
	case userinfo:
		{
			iRet= SHMUserInfoData::userinfo_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  userinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMUserInfoData::userinfo_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						if(SHMUserInfoData::userinfo_P(false)) {
							exit(0);
						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case sgw_sla_que_rel:
		{
			iRet= SHMSgwSlaqueRel::sgwslaquerel_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwslaquerel_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMSgwSlaqueRel::sgwslaquerel_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMSgwSlaqueRel::sgwslaquerel_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case netInfodata:
		{
			iRet= SHMNetInfoData::netinfo_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  netinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMNetInfoData::netinfo_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMNetInfoData::netinfo_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;

	case sessiondata:
		{
			iRet= SHMSessionData::session_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  session_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMSessionData::session_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMSessionData::session_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
			break;
		}
	case shmcongestlevel:
		{
			iRet= SHMCongestLevel::congestleve_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  congestleve_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMCongestLevel::congestlevel_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMCongestLevel::congestlevel_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
			break;
		}
	case service_context_list_base:
		{
			iRet= SHMServiceTxtListBase::servicetxtlistbase_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlistbase_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMServiceTxtListBase::servicetxtlistbase_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMServiceTxtListBase::servicetxtlistbase_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case loadbalancedata:
		{
			iRet= SHMLoadBalance::loadbalancedata_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  loadbalancedata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMLoadBalance::loadbalance_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMLoadBalance::loadbalance_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
			break;
		}
	case base_method:
		{
			iRet= SHMBaseMethod::basemethod_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  basemethod_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMBaseMethod::basemethod_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMBaseMethod::basemethod_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case service_package_route:
		{
			iRet= SHMServicePackageRoute::servicepackageroute_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackageroute_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMServicePackageRoute::packageroute_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMServicePackageRoute::packageroute_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case s_packet_session:
		{
			iRet= SHMPacketSession::packetsession_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  packetsession_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMPacketSession::packetsession_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMPacketSession::packetsession_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
			break;
		}

	case service_package_node:
		{
			iRet= SHMServicePackage::servicepackage_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackage_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMServicePackage::servicepackage_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMServicePackage::servicepackage_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case service_package_route_ccr:
		{
			iRet= SHMServicePackageRouteCCR::servicepackagerouteCCR_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagerouteCCR_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMServicePackageRouteCCR::packagerouteccr_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMServicePackageRouteCCR::packagerouteccr_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case service_package_judge:
		{
			iRet= SHMServicePackageJudge::servicepackagejudge_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagejudge_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMServicePackage::servicepackage_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMServicePackage::servicepackage_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case service_package_cca:
		{
			iRet= SHMServicePackageCCA::servicepackagecca_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagecca_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMServicePackageCCA::servicepackageCCA_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMServicePackageCCA::servicepackageCCA_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case service_package_func:
		{
			iRet= SHMServicePackageFunc::servicepackagefunc_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagefunc_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMServicePackageFunc::servicepackagefunc_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMServicePackageFunc::servicepackagefunc_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}
			}
		}
		break;
	case service_package_variable:
		{
			iRet= SHMServicePackageVle::servicepackagevariable_init();
			if (iRet) {
				iRet = MBC_SHM_INIT_FAILED;
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagevariable_init failed!"<<endl;
			} else {
				if (fork() == 0 ) {
					SHMServicePackageVle::servicepackagevle_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMServicePackageVle::servicepackagevle_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}

			}
		}
		break;
	case statisticscongestmsg:
		{
			iRet= SHMSCongestMsg::scongestmsg_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  scongestmsg_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMSCongestMsg::scongestmsg_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMSCongestMsg::scongestmsg_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}

			}
			break;
		}
	case wf_process_mq:
		{
			iRet= SHMSGWInfoBase::wfprocessmq_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  wfprocessmq_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMWfProcessMq::wfprocessmq_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						// 						if(SHMWfProcessMq::wfprocessmq_P(false)) {
						// 							exit(0);
						// 						}
						sleep(3);
					}
				} else {
					exit(0);
				}

			}
		}
		break;
	case portinfo:
		{
			iRet= SHMSGWInfoBase::portInfo_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMPortInfo::portinfo_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						sleep(3);
					}
				} else {
					exit(0);
				}

			}
		}
		break;
	case channelInfo:
		{
			iRet= SHMSGWInfoBase::channelinfo_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  channelinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMChannelInfo::ChannelInfo_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						sleep(3);
					}
				} else {
					exit(0);
				}

			}
		}
		break;
	case sgw_enum_area:
		{
			iRet= SHMSGWInfoBase::sgwenumarea_init();
			if (iRet) {
				//cout<<"[lock errno]: "<<__FILE__<<":"<<__LINE__<<"  channelinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			} else {
				if (fork() == 0 ) {
					SHMEnumArea::SgwEnumArea_P();
					cout<<"共享内存锁定成功……"<<endl;
					while (true) {
						sleep(3);
					}
				} else {
					exit(0);
				}

			}
		}
		break;
	default:
		break;
	}
	return iRet;
}

int SHMCmdSet::querySHMData(const char *_sType,char *_sKey,char *_sKey1)
{
	int iRet = 0;
	switch(checkargv(_sType))
	{
	case context_filter:
		{
			iRet = SHMDataMgr::contentfiltering_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {

				if (_sKey==NULL||strlen(_sKey) == 0){
					vector<ContextData>  vsContextInfo;
					SHMContentFiltering::GetContextFilterInfoAll(vsContextInfo);

					cout<<"查找的内存数据信息如下:"<<endl;
					for (int i =0; i<vsContextInfo.size();i++) {
						cout<<"--------------------------"<<endl;
						cout<<"FilterSeq="<<vsContextInfo[i].m_iFilterSeq<<endl;
						cout<<"ServiceContextId="<<vsContextInfo[i].m_strServiceContextId<<endl;
						cout<<"IllegalKeyword="<<vsContextInfo[i].m_strIllegalKeyword<<endl;
						cout<<"--------------------------"<<endl;
						cout<<"继续请按c键，其它任意键退出！"<<endl;
						char cCin;
						cin>>cCin;
						if (cCin != 'c') break;
					}
				} else {
					vector<ContextData>  vsContextInfo;;
					int iRlst = SHMContentFiltering::GetContextFilterInfo(atoi(_sKey),vsContextInfo);

					if (iRlst!=0) {
						cout<<"查询不到内存数据！"<<endl;
					} else {
						cout<<"查找的内存数据信息如下:"<<endl;
						for (int i =0; i<vsContextInfo.size();i++) {
							cout<<"--------------------------"<<endl;
							cout<<"FilterSeq="<<vsContextInfo[i].m_iFilterSeq<<endl;
							cout<<"ServiceContextId="<<vsContextInfo[i].m_strServiceContextId<<endl;
							cout<<"IllegalKeyword="<<vsContextInfo[i].m_strIllegalKeyword<<endl;
							cout<<"--------------------------"<<endl;
							cout<<"继续请按c键，其它任意键退出！"<<endl;
							char cCin;
							cin>>cCin;
							if (cCin != 'c') break;
						}
					}
				}
				SHMDataMgr::contentfiltering_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  contentfiltering_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case tab_msg_map:
		{
			iRet = SHMDataMgr::msgmap_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				vector<SMsgMapInfo>  vsMsgMapInfo;
				SHMMsgMapInfo::GetMsgMapInfo(vsMsgMapInfo);//创建存储表TAB_MSG_MAP的数据的共享内存
				cout<<"查找的内存数据信息如下:"<<endl;
				for (int i =0; i<vsMsgMapInfo.size();i++) {
					cout<<"--------------------------"<<endl;
					cout<<"ID="<<vsMsgMapInfo[i].m_iID<<endl;
					cout<<"DataType="<<vsMsgMapInfo[i].m_iDataType<<endl;
					cout<<"MsgType="<<vsMsgMapInfo[i].m_iMsgType<<endl;
					cout<<"ContextFilter="<<vsMsgMapInfo[i].m_iContextFilter<<endl;
					cout<<"DataLimited="<<vsMsgMapInfo[i].m_iDataLimited<<endl;
					cout<<"MajorAvp="<<vsMsgMapInfo[i].m_bMajorAvp<<endl;
					cout<<"Selective="<<vsMsgMapInfo[i].m_szSelective<<endl;
					cout<<"ServiceContextID="<<vsMsgMapInfo[i].m_szServiceContextID<<endl;
					cout<<"DccNode="<<vsMsgMapInfo[i].m_szDccNode<<endl;
					cout<<"DataNode="<<vsMsgMapInfo[i].m_szDataNode<<endl;
					cout<<"DccCode="<<vsMsgMapInfo[i].m_szDccCode<<endl;
					cout<<"DccVendor="<<vsMsgMapInfo[i].m_szDccVendor<<endl;
					cout<<"--------------------------"<<endl;
					cout<<"继续请按c键，其它任意键退出！"<<endl;
					char cCin;
					cin>>cCin;
					if (cCin != 'c') break;
				}
				SHMDataMgr::msgmap_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  msgmap_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_context_list:
		{
			iRet = SHMDataMgr::servicetxtlist_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				if (_sKey == NULL||strlen(_sKey) == 0) {
					vector<ServiceContextListData>  vServiceContextListData;
					SHMServiceTxtList::GetServiceContextListAll(vServiceContextListData);//创建存储表service_context_list的数据的共享内存
					cout<<"查找的内存数据信息如下:"<<endl;
					for (int i =0; i<vServiceContextListData.size();i++) {
						cout<<"--------------------------"<<endl;
						cout<<"ServiceContextListId="<<vServiceContextListData[i].m_iServiceContextListId<<endl;
						cout<<"ServiceContextId="<<vServiceContextListData[i].m_strServiceContextId<<endl;
						cout<<"Active="<<vServiceContextListData[i].m_iActive<<endl;
						cout<<"UserSeq="<<vServiceContextListData[i].m_iUserSeq<<endl;
						cout<<"Sign="<<vServiceContextListData[i].m_iSign<<endl;
						cout<<"FlowId="<<vServiceContextListData[i].m_strFlowId<<endl;
						cout<<"Next="<<vServiceContextListData[i].m_iNext<<endl;
						cout<<"--------------------------"<<endl;
						cout<<"继续请按c键，其它任意键退出！"<<endl;
						char cCin;
						cin>>cCin;
						if (cCin != 'c') break;
					}
				} else {
					if (_sKey1 == NULL||strlen(_sKey1) == 0) {
						vector<ServiceContextListData>  vServiceContextListData;
						SHMServiceTxtList::GetServiceContextID(atoi(_sKey),vServiceContextListData);
						cout<<"查找的内存数据信息如下:"<<endl;
						for (int i =0; i<vServiceContextListData.size();i++) {
							cout<<"--------------------------"<<endl;
							cout<<"ServiceContextListId="<<vServiceContextListData[i].m_iServiceContextListId<<endl;
							cout<<"ServiceContextId="<<vServiceContextListData[i].m_strServiceContextId<<endl;
							cout<<"Active="<<vServiceContextListData[i].m_iActive<<endl;
							cout<<"UserSeq="<<vServiceContextListData[i].m_iUserSeq<<endl;
							cout<<"Sign="<<vServiceContextListData[i].m_iSign<<endl;
							cout<<"FlowId="<<vServiceContextListData[i].m_strFlowId<<endl;
							cout<<"Next="<<vServiceContextListData[i].m_iNext<<endl;
							cout<<"--------------------------"<<endl;
							cout<<"继续请按c键，其它任意键退出！"<<endl;
							char cCin;
							cin>>cCin;
							if (cCin != 'c') break;
						}
					} else {
						ServiceContextListData oServiceContextListData;
						int iRlst = SHMServiceTxtList::GetServiceContextID(atoi(_sKey),_sKey1,oServiceContextListData);
						if (iRlst == 0) {
							cout<<"查找的内存数据信息如下:"<<endl;
							cout<<"--------------------------"<<endl;
							cout<<"ServiceContextListId="<<oServiceContextListData.m_iServiceContextListId<<endl;
							cout<<"ServiceContextId="<<oServiceContextListData.m_strServiceContextId<<endl;
							cout<<"Active="<<oServiceContextListData.m_iActive<<endl;
							cout<<"UserSeq="<<oServiceContextListData.m_iUserSeq<<endl;
							cout<<"Sign="<<oServiceContextListData.m_iSign<<endl;
							cout<<"FlowId="<<oServiceContextListData.m_strFlowId<<endl;
							cout<<"Next="<<oServiceContextListData.m_iNext<<endl;
							cout<<"--------------------------"<<endl;
						} else {
							cout<<"查询不到内存数据！"<<endl;
						}
					}

				}
				SHMDataMgr::servicetxtlist_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlist_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case stream_ctrl_info:
		{
			iRet = SHMDataMgr::streamctrl_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createStreamCtrlSHM();//创建存储表STREAM_CTRL_INFO的数据的共享内存
				SHMDataMgr::streamctrl_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrl_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case stream_ctrl_data:
		{
			iRet = SHMDataMgr::streamctrldata_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createStreamCtrlDataSHM();
				SHMDataMgr::streamctrldata_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  streamctrldata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case userinfo:
		{
			iRet = SHMDataMgr::userinfo_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				if (_sKey == NULL||strlen(_sKey) == 0) {
					vector<UserInfoData>  vsUserInfoData;
					SHMUserInfoData::GetUserInfoDataAll(vsUserInfoData);
					cout<<"查询的数据如下："<<endl;
					for (int i =0; i<vsUserInfoData.size(); i++){
						cout<<"--------------------------"<<endl;
						cout<<"UserSeq="<<vsUserInfoData[i].m_iUserSeq<<endl;
						cout<<"UserName="<<vsUserInfoData[i].m_strUserName<<endl;
						cout<<"Realm="<<vsUserInfoData[i].m_strRealm<<endl;
						cout<<"IpAddr="<<vsUserInfoData[i].m_strIpAddr<<endl;
						cout<<"Port="<<vsUserInfoData[i].m_iPort<<endl;
						cout<<"SgwSoapPort="<<vsUserInfoData[i].m_iSgwSoapPort<<endl;
						cout<<"AdaptiveInfoId="<<vsUserInfoData[i].m_iAdaptiveInfoId<<endl;
						cout<<"RegisterTime="<<vsUserInfoData[i].m_strRegisterTime<<endl;
						cout<<"UpdateTime="<<vsUserInfoData[i].m_strUpdateTime<<endl;
						cout<<"Passwd="<<vsUserInfoData[i].m_strPasswd<<endl;
						cout<<"IsEncrypt="<<vsUserInfoData[i].m_bIsEncrypt<<endl;
						cout<<"ContextIssign="<<vsUserInfoData[i].m_bContextIssign<<endl;
						cout<<"NetFlag="<<vsUserInfoData[i].m_iNetFlag<<endl;
						cout<<"Active="<<vsUserInfoData[i].m_iActive<<endl;
						cout<<"Connect="<<vsUserInfoData[i].m_beConnect<<endl;
						cout<<"TimeOut="<<vsUserInfoData[i].m_iTimeOut<<endl;
						cout<<"Action="<<vsUserInfoData[i].m_iAction<<endl;
						cout<<"ShortLinkFlag="<<vsUserInfoData[i].m_iShortLinkFlag<<endl;
						cout<<"NetinfoSeq="<<vsUserInfoData[i].m_iNetinfoSeq<<endl;
						cout<<"ServiceContext数组如下:"<<endl;
						cout<<"......................."<<endl;
						for (int j =0; j<SERVICENUM; j++){
							if (strlen(vsUserInfoData[i].m_strServiceContext[j]) == 0 )continue;
							cout<<"ServiceContext["<<j<<"] ="<<vsUserInfoData[i].m_strServiceContext[j]<<endl;
						}
						cout<<"......................."<<endl;
						cout<<"BeServiceContextActive数组如下:"<<endl;
						cout<<"......................."<<endl;
						for (int j = 0; j<SERVICENUM;j++){
							cout<<vsUserInfoData[i].m_iBeServiceContextActive[j]<<";";
						}
						cout<<endl;
						cout<<"......................."<<endl;
						cout<<"ExternNetCertPath="<<vsUserInfoData[i].m_strExternNetCertPath<<endl;
						cout<<"BusiPublicKey="<<vsUserInfoData[i].m_sBusiPublicKey<<endl;
						cout<<"BusiPrivateKey="<<vsUserInfoData[i].m_sBusiPrivateKey<<endl;
						cout<<"BusiKeyExpDate="<<vsUserInfoData[i].m_sBusiKeyExpDate<<endl;
						cout<<"SGWCertPath="<<vsUserInfoData[i].m_strSGWCertPath<<endl;
						cout<<"RePid="<<vsUserInfoData[i].m_iRePid<<endl;
						cout<<"IsDisConn="<<vsUserInfoData[i].m_iIsDisConn<<endl;
						cout<<"ReConnTime="<<vsUserInfoData[i].m_lReConnTime<<endl;
						cout<<"链路进程信息如下:"<<endl;
						cout<<"......................."<<endl;
						cout<<"ConnInfo.RecvPid="<<vsUserInfoData[i].s_ConnInfo.m_iRecvPid<<";";
						cout<<"ConnInfo.m_iSendPid="<<vsUserInfoData[i].s_ConnInfo.m_iSendPid<<";";
						cout<<"ConnInfo.m_lRecvMsgNum="<<vsUserInfoData[i].s_ConnInfo.m_lRecvMsgNum<<";";
						cout<<"ConnInfo.m_lSendMsgNum="<<vsUserInfoData[i].s_ConnInfo.m_lSendMsgNum<<endl;
						cout<<"......................."<<endl;
						cout<<"PaddingType="<<vsUserInfoData[i].m_iPaddingType<<endl;
						cout<<"Priority_level="<<vsUserInfoData[i].m_iPriority_level<<endl;
						cout<<"SoapUrl="<<vsUserInfoData[i].m_strSoapUrl<<endl;
						cout<<"PasErrTime="<<vsUserInfoData[i].m_iPasErrTime<<endl;
						cout<<"SerErrTime="<<vsUserInfoData[i].m_iSerErrTime<<endl;
						cout<<"UnLockTime="<<vsUserInfoData[i].m_lUnLockTime<<endl;
						cout<<"SerConErr="<<vsUserInfoData[i].m_iSerConErr<<endl;
						cout<<"SerConNoActive="<<vsUserInfoData[i].m_iSerConNoActive<<endl;
						cout<<"PriUpdateTime="<<vsUserInfoData[i].m_strPriUpdateTime<<endl;
						cout<<"Next="<<vsUserInfoData[i].m_iNext<<endl;
						cout<<"--------------------------"<<endl;
						cout<<"继续请按c键，其它任意键退出！"<<endl;
						char cCin;
						cin>>cCin;
						if (cCin != 'c') break;
					}
				} else {
					int pos = 0;
					UserInfoData _oData;
					while (_sKey[pos]>='0'&&_sKey[pos++]<='9');
					memset(&_oData,0,sizeof(_oData));
					int _iRlst = 0;

					if (pos == strlen(_sKey)) {
						_iRlst =SHMUserInfoData::GetUserInfoData(atoi(_sKey),_oData);
					} else {
						_iRlst = SHMUserInfoData::GetUserInfoData(_sKey,_oData);
					}
					if (!_iRlst) {
						cout<<"UserSeq="<<_oData.m_iUserSeq<<endl;
						cout<<"UserName="<<_oData.m_strUserName<<endl;
						cout<<"Realm="<<_oData.m_strRealm<<endl;
						cout<<"IpAddr="<<_oData.m_strIpAddr<<endl;
						cout<<"Port="<<_oData.m_iPort<<endl;
						cout<<"SgwSoapPort="<<_oData.m_iSgwSoapPort<<endl;
						cout<<"AdaptiveInfoId="<<_oData.m_iAdaptiveInfoId<<endl;
						cout<<"RegisterTime="<<_oData.m_strRegisterTime<<endl;
						cout<<"UpdateTime="<<_oData.m_strUpdateTime<<endl;
						cout<<"Passwd="<<_oData.m_strPasswd<<endl;
						cout<<"IsEncrypt="<<_oData.m_bIsEncrypt<<endl;
						cout<<"ContextIssign="<<_oData.m_bContextIssign<<endl;
						cout<<"NetFlag="<<_oData.m_iNetFlag<<endl;
						cout<<"Active="<<_oData.m_iActive<<endl;
						cout<<"Connect="<<_oData.m_beConnect<<endl;
						cout<<"TimeOut="<<_oData.m_iTimeOut<<endl;
						cout<<"Action="<<_oData.m_iAction<<endl;
						cout<<"ShortLinkFlag="<<_oData.m_iShortLinkFlag<<endl;
						cout<<"NetinfoSeq="<<_oData.m_iNetinfoSeq<<endl;
						cout<<"ServiceContext数组如下:"<<endl;
						cout<<"......................."<<endl;
						for (int j =0; j<SERVICENUM; j++){
							if (strlen(_oData.m_strServiceContext[j]) == 0) continue;
							cout<<"ServiceContext["<<j<<"] ="<<_oData.m_strServiceContext[j]<<endl;
						}
						cout<<"......................."<<endl;
						cout<<"BeServiceContextActive数组如下:"<<endl;
						cout<<"......................."<<endl;
						for (int j = 0; j<SERVICENUM;j++){
							cout<<_oData.m_iBeServiceContextActive[j]<<"; ";
						}
						cout<<endl;
						cout<<"......................."<<endl;
						cout<<"ExternNetCertPath="<<_oData.m_strExternNetCertPath<<endl;
						cout<<"BusiPublicKey="<<_oData.m_sBusiPublicKey<<endl;
						cout<<"BusiPrivateKey="<<_oData.m_sBusiPrivateKey<<endl;
						cout<<"BusiKeyExpDate="<<_oData.m_sBusiKeyExpDate<<endl;
						cout<<"SGWCertPath="<<_oData.m_strSGWCertPath<<endl;
						cout<<"RePid="<<_oData.m_iRePid<<endl;
						cout<<"IsDisConn="<<_oData.m_iIsDisConn<<endl;
						cout<<"ReConnTime="<<_oData.m_lReConnTime<<endl;
						cout<<"链路进程信息如下:"<<endl;
						cout<<"......................."<<endl;
						cout<<"ConnInfo.RecvPid="<<_oData.s_ConnInfo.m_iRecvPid<<"; ";
						cout<<"ConnInfo.m_iSendPid="<<_oData.s_ConnInfo.m_iSendPid<<"; ";
						cout<<"ConnInfo.m_lRecvMsgNum="<<_oData.s_ConnInfo.m_lRecvMsgNum<<"; ";
						cout<<"ConnInfo.m_lSendMsgNum="<<_oData.s_ConnInfo.m_lSendMsgNum<<endl;
						cout<<"......................."<<endl;
						cout<<"PaddingType="<<_oData.m_iPaddingType<<endl;
						cout<<"Priority_level="<<_oData.m_iPriority_level<<endl;
						cout<<"SoapUrl="<<_oData.m_strSoapUrl<<endl;
						cout<<"PasErrTime="<<_oData.m_iPasErrTime<<endl;
						cout<<"SerErrTime="<<_oData.m_iSerErrTime<<endl;
						cout<<"UnLockTime="<<_oData.m_lUnLockTime<<endl;
						cout<<"SerConErr="<<_oData.m_iSerConErr<<endl;
						cout<<"SerConNoActive="<<_oData.m_iSerConNoActive<<endl;
						cout<<"PriUpdateTime="<<_oData.m_strPriUpdateTime<<endl;
					} else {
						cout<<"查询不到数据！"<<endl;
					}
				}
				SHMDataMgr::userinfo_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  userinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sgw_sla_que_rel:
		{
			iRet = SHMDataMgr::sgwslaquerel_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createSgwSlaqueRelSHM();
				SHMDataMgr::sgwslaquerel_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sgwslaquerel_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case netInfodata:
		{
			iRet = SHMDataMgr::netinfo_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createNetSHM();
				SHMDataMgr::netinfo_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  netinfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case sessiondata:
		{
			iRet = SHMDataMgr::session_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createSessionSHM();
				SHMDataMgr::session_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  session_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case shmcongestlevel:
		{
			iRet = SHMDataMgr::congestleve_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createCongestLeveSHM();
				SHMDataMgr::congestleve_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  congestleve_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_context_list_base:
		{
			iRet = SHMDataMgr::servicetxtlistbase_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createServiceTxtListBaseSHM();
				SHMDataMgr::servicetxtlistbase_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicetxtlistbase_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case loadbalancedata:
		{
			iRet = SHMDataMgr::loadbalancedata_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createloadbalanceSHM();
				SHMDataMgr::loadbalancedata_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  loadbalancedata_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_route:
		{
			iRet = SHMDataMgr::servicepackageroute_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createpackagerouteSHM();
				SHMDataMgr::servicepackageroute_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackageroute_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case base_method:
		{
			iRet = SHMDataMgr::basemethod_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createbasemethodSHM();
				SHMDataMgr::basemethod_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  basemethod_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case s_packet_session:
		{
			iRet = SHMDataMgr::packetsession_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createpacketsessionSHM();
				SHMDataMgr::packetsession_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  packetsession_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_node:
		{
			iRet = SHMDataMgr::servicepackage_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createservicepackageSHM();
				SHMDataMgr::servicepackage_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackage_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_route_ccr:
		{
			iRet = SHMDataMgr::servicepackagerouteCCR_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createservicepackageCCRSHM();
				SHMDataMgr::servicepackagerouteCCR_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagerouteCCR_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_judge:
		{
			iRet = SHMDataMgr::servicepackagejudge_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createpackagejudgeSHM();
				SHMDataMgr::servicepackagejudge_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagejudge_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_cca:
		{
			iRet = SHMDataMgr::servicepackagecca_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createpackageccaSHM();
				SHMDataMgr::servicepackagecca_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagecca_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_func:
		{
			iRet = SHMDataMgr::servicepackagefunc_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createpackagefuncSHM();
				SHMDataMgr::servicepackagefunc_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagefunc_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case service_package_variable:
		{
			iRet = SHMDataMgr::servicepackagevariable_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createpackagevleSHM();
				SHMDataMgr::servicepackagevariable_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  servicepackagevariable_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case statisticscongestmsg:
		{
			iRet = SHMDataMgr::scongestmsg_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createscongestmsgSHM();
				SHMDataMgr::scongestmsg_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  scongestmsg_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case wf_process_mq:
		{
			iRet = SHMDataMgr::wfprocessmq_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createwfprocessmqSHM();
				SHMDataMgr::wfprocessmq_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  wfprocessmq_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case portinfo:
		{
			iRet = SHMDataMgr::portInfo_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createportinfoSHM();
				SHMDataMgr::portInfo_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  portInfo_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	case session_clear:
		{
			iRet = SHMDataMgr::sessionclear_init();
			if (iRet!=MBC_PARAM_THREAD_LOST_ERROR&&iRet!=MBC_PARAM_THREAD_VALUE_ERROR) {
				//SHMDataMgr::createSessionClearSHM();
				SHMDataMgr::sessionclear_free();
			} else {
				//cout<<"[create errno]: "<<__FILE__<<":"<<__LINE__<<"  sessionclear_init failed!"<<endl;
				iRet = MBC_SHM_INIT_FAILED;
			}
		}
		break;
	default:
		break;
	}
	//cout<<"共享内存创建成功,可通过sgwshm -stat 查询其状态……"<<endl;
	return iRet;

}
