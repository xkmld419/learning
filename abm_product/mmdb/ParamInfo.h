#ifndef SHMPARAMINFO_H_HEADER_INCLUDED
#define SHMPARAMINFO_H_HEADER_INCLUDED

#include "SHMStructType.h"
#include "SHMData.h"
#include "SHMIntHashIndex.h"
#include "SHMStringTreeIndexEx.h"
#include "SHMStringTreeIndex.h"
#include "IpcKey.h"

#include "LogicStatement.h"
#include "AssignStatement.h"
#include "Formula.h"
#include "AttrTransMgr.h"
#include "FormatMethod.h"
#include "ConvertCityCodeMgr.h"

#include "OrgMgr.h"
#include "OrgMgrEx.h"
#include "OrgLongTypeMgr.h"
#include "Head2OrgIDMgr.h"
#include "LocalHead.h"

#include "EventType.h"
#include "EventTypeMgr.h"
#include "LongEventTypeRule.h"
#include "LongEventTypeRuleEx.h"

#include "PlanType.h"
#include "OfferMgr.h"
#include "StateConvertMgr.h"
#include "ValueMapMgr.h"
#include "NbrMapMgr.h"

#include "AcctItemMgr.h"
#include "ZoneMgr.h"
#include "SplitTicketCfg.h"
#include "DistanceMgr.h"
#include "EdgeRoamMgr.h"

#include "PricingInfo.h"
#include "Tariff.h"
#include "Discount.h"
#include "DisctApplyer.h"
#include "Percent.h"
#include "PricingMeasure.h"
#include "PricingCombine.h"
#include "UsageTypeMgr.h"
#include "AccuTypeCfg.h"
#include "SplitMgr.h"
#include "Directory.h"
#include "GatherTaskInfo.h"
#include "SpecialNbrStrategyMgr.h"
#include "HostInfo.h"
#include "SpecialTariffMgr.h"
#include "SpInfoSeekMgr.h"
#include "SegmentMgr.h"
//#include "LongTypeMgr.h"
#include "RecordCheck.h"
#include "ConvertFunc.h"
#include "InstTableListMgr.h"
#include "NetworkMgr.h"
#include "../app_prep/LocalFormatMethod.h"
#include "../app_dupchecker/CheckDup.h"
#include "TrunkSegBilling.h"
#include "FilterRule.h"

//2010.11.17
#include "../app_scheduler/SchedulerFilePoolMgr.h"
#include "../app_scheduler/SchedulerFileTypeMgr.h"
//#include "../app_scheduler/FileStructMgr.h"
#include "BillingNodeInfoMgr.h"
#include "../app_scheduler/SchedulerSwitchCodeMgr.h"
#include "../app_scheduler/SchedulerParseRuleMgr.h"
#include "../app_scheduler/TLVStructMgr.h"
#include "RoamTariff.h"

#include "../app_prep/PrepStructs.h"
#include "../app_prep/ParamMgr.h"
#include "../app_prep/Config.h"
#include "../app_prep/Field.h"
#include "../app_prep/Tap3FileInfo.h"
#include "../app_prep/SepFileInfo.h"
#include "../app_merge/MergeRule.h"
#include "../app_merge/MergeProcess.h"
#include "../app_merge/MergeStruct.h"


#include "../app_trans/TransStructs.h"

#define PARAM_SHM_INDEX_ALL -1                 //
#define PARAM_SHM_INDEX_A    0                 //共享内存块的编号A
#define PARAM_SHM_INDEX_B    1                 //共享内存块的编号B

#define PARAM_SHM_NUMBER              600       //需要导入的参数的种类数序号根据下面添加序号增加

#define LOGICSTATEMENT_INFO           0        //条件参数的key的序号
#define ASSIGNSTATEMENT_INFO          1        //运算参数的key的序号
#define FORMULA_INFO                  2        //公式参数的key的序号
#define ATTR_FORMAT_GROUP_INFO        3
#define ATTR_FORMAT_STEP_INFO         4
#define ATTR_FORMAT_RULE_INFO         5
#define ATTR_FORMAT_COMB_INFO         6
#define TRUNK_INFO                    7
#define AREACODE_INFO                 8
#define HLR_INFO                      9
#define CARRIER_INFO                  10
#define SERVICE_INFO                  11
#define SWITCH_INFO                   12
#define SWITCH_OPC_INFO               13
#define TRUNKBILLING_INFO             14
#define TIMESTYPE_INFO                15
#define CITY_INFO                     16
#define CITY_AREA_INFO                17
#define ORG_INFO                      18
#define ORGEX_INFO                    19
#define ORGLONGTYPE_INFO              20
#define HEADEX_INFO                   21
#define LOCALHEAD_INFO                22
#define EVENTTYPE_INFO                23
#define EVENTTYPEGROUP_INFO           24
#define OFFER_INFO                    25
#define OFFER_SPECATTR_INFO           26
#define OFFER_NUMATTR_INFO            27
#define OFFER_FREE_INFO               28
#define NBR_MAP_INFO                  29
#define NBR_MAP_INDEX1_INFO           30
#define NBR_MAP_INDEX2_INFO           31
#define NBR_MAP_INDEX4_INFO           32
#define NBR_MAP_INDEX8_INFO           33
#define ACCTITEM_INFO                 34
#define ACCTITEM_GROUP_INFO           35
#define ACCTITEM_GROUPB_INFO          36
#define ACCTITEM_BASICGROUPB_INFO     37
#define ACCTITEM_DISCTGROUPB_INFO     38
#define ACCTITEM_INCLUDEDISCT_INFO    39
#define LONGEVENTTYPERULE_INFO        40
#define LONGEVENTTYPERULEEX_INFO      41
#define PRICINGPLANTYPE_INFO          42
#define STATECONVERT_INFO             43
#define VALUEMAP_INFO                 44
#define ZONE_ITEM_INFO                45
#define SPLITACCTITEMGROUP_INFO       46
#define SPLITACCTGROUPEVENT_INFO      47
#define DISTANCETYPE_INFO             48
#define EDGEROAMRULEEX_INFO           49
#define EDGEROAMRULEEX_TYPEA_INFO     50
#define EDGEROAMRULEEX_TYPEB_INFO     51
#define EDGEROAMRULEEX_MSC_INFO       52

//2010.11.17
#define FILEPOOL_INFO                 53
#define FILEPOOL_COND_INFO            54
#define FILETYPE_INFO                 55  
#define TLVENCODE_INFO                56
#define TLVDECODE_INFO                57
#define BILLINGNODE_INFO              58
#define FILEPARSERULE_INFO            59
#define SWITCHCODE_INFO               60
//2010.12.20
#define ROAMTARIFF_INFO               61
#define ROAMCARRIER_INFO              62
#define TARIFFSECTOR_INFO             63
#define COUNTRY_INFO_A                64
#define COUNTRY_INFO_B                65
#define ROAMSPONSOR_INFO              66
#define SPECIALGROUPMEMBER_INFO       67
//2011.01.04
#define VISIT_AREACODE_RULE_INFO	  68
#define EDGEROAM_RULE_INFO	  		  69
#define PNSEGBILLING_INFO			  70
#define PNSEGBILLING_EX_INFO		  71
#define PNNOTBILLING_INFO		  	  72
#define TRUNKTREENODE_INFO		  	  73
#define TSERVICEINFO_INFO			  74
#define TSERVICEINFO2_INFO			  75
#define SPEC_TRUNK_PRODUCT_INFO		  76
#define SWITCHITEMTYPE_INFO			  77
#define STR_LOCALAREACODE_INFO		  78

#define PRICINGSECTION_INFO           201				 
#define SECTIONSTRATEGY_INFO          202				
#define TARIFF_INFO                   203        
#define DISCOUNT_INFO                 204       
#define DISCTAPPLYER_INFO             205        
#define DISCTAPPLYEROBJ_INFO          206 

#define PARAM_PERCENTINFO             207        // 
#define PARAM_LIFECYCLEINFO			  208	     // LIFECYCLE     
#define PARAM_MEASUREINFO             209        // MEASUREINFO 
#define PARAM_STRATEGYMEASUREINFO     210        // STRATEGYMEASUREINFO 
#define PARAM_COMBINETINFO            211        //COMBINE 
#define PARAM_PLAN_COMBINETINFO       212        //PLAN_COMBINE
#define PARAM_STRATEGY_COMBINETINFO   213        //STRATEGY_COMBINE
#define PARAM_FILTERCONDINFO          214        //FILTERCONDITION
#define PARAM_CONDITIONPOINTINFO      215        //ConditionPoint
#define PARAM_TIMECONDINFO            216        //TIMECOND
#define PARAM_EVENTTYPECONDINFO       217        //EVENTTYPECOND
#define PARAM_CYCLETYPEINFO           218        //EVENTTYPECOND_key
#define PARAM_ACCTTYPEINFO            219        //
#define PARAM_OFFERACCTRELATIONINFO   220
#define PARAM_OFFERACCTEXISTINFO   	  221
#define PARAM_CYCLETYPEIDINFO         222
#define PARAM_OFFERCOMBINEINFO        223       //OFFERCOMBINE
#define PARAM_PRODUCTCOMBINEINFO      224
#define PARAM_SPLITCFGINFO            225       //SPLITCFG
#define PARAM_SPECIALNBRSTRATEGYINFO  226       //SPECIALNBRSTRATEGY
#define PARAM_SPECIALNBROFFERINFO     227       //SPECIALNBROFFER

#define PARAM_VIRTUAL_STRATEGY_COMBINETINFO   228       //VIRTUAL_STRATEGY_COMBINE
#define PARAM_WHOLEDISCOUNTINFO       229       //WHOLEDISCOUNT
#define PARAM_CGCOMBINEINFO           230       //CGCOMBINE
#define PARAM_COMBINERELATIONINFO     231       
#define PARAM_USAGETYPEINFO           232        
#define PARAM_GATHERTESKINFO          233        
#define PARAM_ORGGATHERTESKINFO       234        
//#define PARAM_DIRECTORYINFO      	  235   
#define PARAM_HOSTINFO      	  	  235   
#define PARAM_PRODUCTIDINFO      	  236  
#define PARAM_PRODUCTACCITEMINFO      237  
#define PARAM_SPECIALTRIFFINFO        238 
#define PARAM_SPECIALTRIFFCHECKINFO   239 
#define PARAM_SPINFODATAINFO   		  240 
#define PARAM_GATHERTASKLISTINFO   	  241 
#define PARAM_SEGMENTINFO   	  	  242 
#define PARAM_SEGMENTTYPEINFO   	  243 
#define PARAM_LONGTYPEINFO   	  	  244 
#define PARAM_CHECKRULE_RULE_INFO	  245
#define PARAM_CHECKRULE_SUBRULE_INFO  246
#define PARAM_CHECKARGUMENT_INFO      247
#define PARAM_CHECKRELATION_INFO      248
#define PARAM_TRANSFILE_INFO      	  249
#define	PARAM_PARSERULE_INFO		  250
#define PARAM_PARSECOMB_INFO		  251
#define PARAM_INSTTABLELIST_INFO	  252
#define PARAM_TABLETYPE_INFO	  	  253
#define PARAM_RENT_CONFIG_INFO		  254
#define PARAM_NETWORKPRIORITY_INFO	  	  255
#define PARAM_TRUNKTOBENBR_INFO	          256
#define PARAM_OFFERDISCTINFO          257       //loadOffer
#define PARAM_CHECKDUPCFG_INFO		  258
#define PARAM_CHECKDUPTYPE_INFO		  259
#define PARAM_CHECKDUPSTEP_INFO		  260
#define PARAM_CHECKSHOTTICKET_INFO	  261
#define PARAM_UNDOPROCESSDUP_INFO	  262
#define PARAM_UNDOPROCESSSHOT_INFO	  263
#define PARAM_UNDOSERVDUP_INFO		  264
#define PARAM_UNDOPRODUCTDUP_INFO	  265
#define PARAM_PRODUCTCFG_INFO   	  266
#define PARAM_ORGPATH_INFO			  267
#define PARAM_LOCALEVENTTYPERULE_INFO	268
#define PARAM_ZONEITEM_STR_INFO			269

//std
#define PARAM_STD_STRUCT_INFO   	  	  270
#define PARAM_STRUCT_INFO_DESC   	  271

//prep
#define PARAM_TEMPLATEMAPSWITCH_INFO  272
#define PARAM_OBJECTDEFINE_INFO  	  273
#define PARAM_EVENTPARSERGROUP_INFO  	  274
#define PARAM_TEMPLATE_RELATION_INFO  	  275
#define PARAM_FIELD_SHM_INFO  	  		  276
#define PARAM_SWITCH_SHM_INFO  	  		  277
#define PARAM_SWITCH_TYPE_INFO  	  	  278
#define PARAM_EPGGROUP_INFO  	  		  279
#define PARAM_FIELD_CHECK_METHOD_INFO  	  		  280
#define PARAM_FIELD_VALUE_MAP_INFO  	  		  281
#define PARAM_FIELD_MERGE_INFO  	  		  282
#define PARAM_EVENT_HEAD_COLUMNS_INFO  	  		  283
#define PARAM_SWITCHID_MAP_SWITCH_INFO  	  	  284

#define PARAM_TAP3FILE_INFO				285
#define PARAM_TAP3BLOCK_INFO			286
#define PARAM_SEPFILE_INFO				287
#define PARAM_SEPBLOCK_INFO				288
#define PARAM_EVENT_ATTR_INFO			289
//#define PARAM_FILE_INFO_INFO			290
//合并功能
#define PARAM_MERGE_ASSOCIATE_RULE_INFO	 290
#define PARAM_MERGE_RULE_INFO			291
#define PARAM_MERGE_OUT_INFO			292
#define PARAM_MERGE_KEY_INFO			293

#define PARAM_TRANS_PEER_INFO			294
#define PARAM_TRANS_PEER_TYPE_INFO			295
#define PARAM_TRANS_PEER_COND_INFO			296
//
#define PARAM_FIELD_UNIONINDEX_INFO  	297
#define PNSEGBILLINGEX_INFO			  	298

//
#define FILTER_RULE_RESULT_INFO  			500
#define FILTER_RULE_RESULT_CALLING_INFO  	501
#define FILTER_RULE_RESULT_CALLED_INFO  	502
#define FILTER_RULE_RESULT_AREACODE_INFO  	503
#define FILTER_RULE_RESULT_GATHERPOT_INFO  	504
#define FILTER_RULE_RESULT_TRUNCKIN_INFO  	505
#define FILTER_RULE_RESULT_TRUNCKOUT_INFO  	506
#define FILTER_RULE_RESULT_SWITCHID_INFO  	507
//
//改进账目组的效率
#define PARAM_ACCT_GROUP_STR_INFO  			510
#define PARAM_ACCT_GROUPB_STR_INFO  		511
#define PARAM_ACCT_STR_INFO  				512
#define PARAM_M_SERVACCT_INFO  				513
#define M_SERVACCT_INFO_DATA_A         IpcKeyMgr::getIpcKey (-1,"M_SERVACCT_INFO_DATA_A")
#define M_SERVACCT_INFO_INDEX_A        IpcKeyMgr::getIpcKey (-1,"M_SERVACCT_INFO_INDEX_A")
#define M_SERVACCT_INFO_DATA_B         IpcKeyMgr::getIpcKey (-1,"M_SERVACCT_INFO_DATA_B")
#define M_SERVACCT_INFO_INDEX_B        IpcKeyMgr::getIpcKey (-1,"M_SERVACCT_INFO_INDEX_B")

#define LOGICSTATE_INFO_DATA_A         IpcKeyMgr::getIpcKey (-1,"LOGICSTATE_INFO_DATA_A")
#define LOGICSTATE_INFO_INDEX_A        IpcKeyMgr::getIpcKey (-1,"LOGICSTATE_INFO_INDEX_A")
#define LOGICSTATE_INFO_DATA_B         IpcKeyMgr::getIpcKey (-1,"LOGICSTATE_INFO_DATA_B")
#define LOGICSTATE_INFO_INDEX_B        IpcKeyMgr::getIpcKey (-1,"LOGICSTATE_INFO_INDEX_B")

#define ASSIGNSTATE_INFO_DATA_A        IpcKeyMgr::getIpcKey (-1,"ASSIGNSTATE_INFO_DATA_A")
#define ASSIGNSTATE_INFO_INDEX_A       IpcKeyMgr::getIpcKey (-1,"ASSIGNSTATE_INFO_INDEX_A")
#define ASSIGNSTATE_INFO_DATA_B        IpcKeyMgr::getIpcKey (-1,"ASSIGNSTATE_INFO_DATA_B")
#define ASSIGNSTATE_INFO_INDEX_B       IpcKeyMgr::getIpcKey (-1,"ASSIGNSTATE_INFO_INDEX_B")

#define FORMULA_INFO_DATA_A            IpcKeyMgr::getIpcKey (-1,"FORMULA_INFO_DATA_A")
#define FORMULA_INFO_INDEX_A           IpcKeyMgr::getIpcKey (-1,"FORMULA_INFO_INDEX_A")
#define FORMULA_INFO_DATA_B            IpcKeyMgr::getIpcKey (-1,"FORMULA_INFO_DATA_B")
#define FORMULA_INFO_INDEX_B           IpcKeyMgr::getIpcKey (-1,"FORMULA_INFO_INDEX_B")

#define FORMAT_GROUP_INFO_DATA_A       IpcKeyMgr::getIpcKey (-1,"FORMAT_GROUP_INFO_DATA_A")
#define FORMAT_GROUP_INFO_DATA_B       IpcKeyMgr::getIpcKey (-1,"FORMAT_GROUP_INFO_DATA_B")
#define FORMAT_STEP_INFO_DATA_A        IpcKeyMgr::getIpcKey (-1,"FORMAT_STEP_INFO_DATA_A")
#define FORMAT_STEP_INFO_DATA_B        IpcKeyMgr::getIpcKey (-1,"FORMAT_STEP_INFO_DATA_B")
#define FORMAT_RULE_INFO_DATA_A        IpcKeyMgr::getIpcKey (-1,"FORMAT_RULE_INFO_DATA_A")
#define FORMAT_RULE_INFO_DATA_B        IpcKeyMgr::getIpcKey (-1,"FORMAT_RULE_INFO_DATA_B")
#define FORMAT_COMB_INFO_DATA_A        IpcKeyMgr::getIpcKey (-1,"FORMAT_COMB_INFO_DATA_A")
#define FORMAT_COMB_INFO_DATA_B        IpcKeyMgr::getIpcKey (-1,"FORMAT_COMB_INFO_DATA_B")

#define TRUNK_INFO_DATA_A              IpcKeyMgr::getIpcKey (-1,"TRUNK_INFO_DATA_A")
#define TRUNK_INFO_INDEX_A             IpcKeyMgr::getIpcKey (-1,"TRUNK_INFO_INDEX_A")
#define TRUNK_INFO_DATA_B              IpcKeyMgr::getIpcKey (-1,"TRUNK_INFO_DATA_B")
#define TRUNK_INFO_INDEX_B             IpcKeyMgr::getIpcKey (-1,"TRUNK_INFO_INDEX_B")
#define AREACODE_INFO_INDEX_A             IpcKeyMgr::getIpcKey (-1,"AREACODE_INFO_INDEX_A")
#define AREACODE_INFO_INDEX_B             IpcKeyMgr::getIpcKey (-1,"AREACODE_INFO_INDEX_B")
#define HLR_INFO_DATA_A              IpcKeyMgr::getIpcKey (-1,"HLR_INFO_DATA_A")
#define HLR_INFO_INDEX_A             IpcKeyMgr::getIpcKey (-1,"HLR_INFO_INDEX_A")
#define HLR_INFO_DATA_B              IpcKeyMgr::getIpcKey (-1,"HLR_INFO_DATA_B")
#define HLR_INFO_INDEX_B             IpcKeyMgr::getIpcKey (-1,"HLR_INFO_INDEX_B")
//补充数据
#define VISITAREACODERULE_INFO_DATA_A              IpcKeyMgr::getIpcKey (-1,"VISITAREACODERULE_INFO_DATA_A")
#define VISITAREACODERULE_INFO_INDEX_A             IpcKeyMgr::getIpcKey (-1,"VISITAREACODERULE_INFO_INDEX_A")
#define VISITAREACODERULE_INFO_DATA_B              IpcKeyMgr::getIpcKey (-1,"VISITAREACODERULE_INFO_DATA_B")
#define VISITAREACODERULE_INFO_INDEX_B             IpcKeyMgr::getIpcKey (-1,"VISITAREACODERULE_INFO_INDEX_B")
//EdgeRoamRule
#define EDGEROAMRULE_INFO_DATA_A              IpcKeyMgr::getIpcKey (-1,"EDGEROAMRULE_INFO_DATA_A")
#define EDGEROAMRULE_INFO_INDEX_A             IpcKeyMgr::getIpcKey (-1,"EDGEROAMRULE_INFO_INDEX_A")
#define EDGEROAMRULE_INFO_DATA_B              IpcKeyMgr::getIpcKey (-1,"EDGEROAMRULE_INFO_DATA_B")
#define EDGEROAMRULE_INFO_INDEX_B             IpcKeyMgr::getIpcKey (-1,"EDGEROAMRULE_INFO_INDEX_B")
//
#define PNSEGBILLINGEX_INFO_DATA_A              IpcKeyMgr::getIpcKey (-1,"PNSEGBILLINGEX_INFO_DATA_A")
#define PNSEGBILLINGEX_INFO_DATA_B              IpcKeyMgr::getIpcKey (-1,"PNSEGBILLINGEX_INFO_DATA_B")
//
#define PNSEGBILLING_INFO_DATA_A              IpcKeyMgr::getIpcKey (-1,"PNSEGBILLING_INFO_DATA_A")
#define PNSEGBILLING_INFO_DATA_B              IpcKeyMgr::getIpcKey (-1,"PNSEGBILLING_INFO_DATA_B")

#define PNNOTBILLING_INFO_DATA_A			  IpcKeyMgr::getIpcKey (-1,"PNNOTBILLING_INFO_DATA_A")
#define PNNOTBILLING_INFO_INDEX_A			  IpcKeyMgr::getIpcKey (-1,"PNNOTBILLING_INFO_INDEX_A")
#define PNNOTBILLING_INFO_DATA_B			  IpcKeyMgr::getIpcKey (-1,"PNNOTBILLING_INFO_DATA_B")
#define PNNOTBILLING_INFO_INDEX_B			  IpcKeyMgr::getIpcKey (-1,"PNNOTBILLING_INFO_INDEX_B")

#define TRUNKTREENODE_INFO_DATA_A			  IpcKeyMgr::getIpcKey (-1,"TRUNKTREENODE_INFO_DATA_A")
#define TRUNKTREENODE_INFO_INDEX_A			  IpcKeyMgr::getIpcKey (-1,"TRUNKTREENODE_INFO_INDEX_A")
#define TRUNKTREENODE_INFO_DATA_B			  IpcKeyMgr::getIpcKey (-1,"TRUNKTREENODE_INFO_DATA_B")
#define TRUNKTREENODE_INFO_INDEX_B			  IpcKeyMgr::getIpcKey (-1,"TRUNKTREENODE_INFO_INDEX_B")


#define TSERVICEINFO2_INFO_DATA_A			  IpcKeyMgr::getIpcKey (-1,"TSERVICEINFO2_INFO_DATA_A")
#define TSERVICEINFO2_INFO_INDEX_A			  IpcKeyMgr::getIpcKey (-1,"TSERVICEINFO2_INFO_INDEX_A")
#define TSERVICEINFO2_INFO_DATA_B			  IpcKeyMgr::getIpcKey (-1,"TSERVICEINFO2_INFO_DATA_B")
#define TSERVICEINFO2_INFO_INDEX_B			  IpcKeyMgr::getIpcKey (-1,"TSERVICEINFO2_INFO_INDEX_B")

#define SPEC_TRUNK_PRODUCT_INFO_INDEX_A			  IpcKeyMgr::getIpcKey (-1,"SPEC_TRUNK_PRODUCT_INFO_INDEX_A")
#define SPEC_TRUNK_PRODUCT_INFO_INDEX_B			  IpcKeyMgr::getIpcKey (-1,"SPEC_TRUNK_PRODUCT_INFO_INDEX_B")

#define SWITCHITEMTYPE_DATA_A					IpcKeyMgr::getIpcKey (-1,"SWITCHITEMTYPE_DATA_A")
#define SWITCHITEMTYPE_INDEX_A					IpcKeyMgr::getIpcKey (-1,"SWITCHITEMTYPE_INDEX_A")
#define SWITCHITEMTYPE_DATA_B					IpcKeyMgr::getIpcKey (-1,"SWITCHITEMTYPE_DATA_B")
#define SWITCHITEMTYPE_INDEX_B					IpcKeyMgr::getIpcKey (-1,"SWITCHITEMTYPE_INDEX_B")

#define STR_LOCALAREACODE_INFO_DATA_A					IpcKeyMgr::getIpcKey (-1,"STR_LOCALAREACODE_INFO_DATA_A")
#define STR_LOCALAREACODE_INFO_DATA_B					IpcKeyMgr::getIpcKey (-1,"STR_LOCALAREACODE_INFO_DATA_B")
//
#define CARRIER_INFO_INDEX_A         IpcKeyMgr::getIpcKey (-1,"CARRIER_INFO_INDEX_A")
#define CARRIER_INFO_INDEX_B         IpcKeyMgr::getIpcKey (-1,"CARRIER_INFO_INDEX_B")
#define SERVICE_INFO_DATA_A          IpcKeyMgr::getIpcKey (-1,"SERVICE_INFO_DATA_A")
#define SERVICE_INFO_INDEX_A         IpcKeyMgr::getIpcKey (-1,"SERVICE_INFO_INDEX_A")
#define SERVICE_INFO_DATA_B          IpcKeyMgr::getIpcKey (-1,"SERVICE_INFO_DATA_B")
#define SERVICE_INFO_INDEX_B         IpcKeyMgr::getIpcKey (-1,"SERVICE_INFO_INDEX_B")
#define SWITCH_INFO_DATA_A           IpcKeyMgr::getIpcKey (-1,"SWITCH_INFO_DATA_A")
#define SWITCH_INFO_INDEX_A          IpcKeyMgr::getIpcKey (-1,"SWITCH_INFO_INDEX_A")
#define SWITCH_INFO_OPC_INDEX_A      IpcKeyMgr::getIpcKey (-1,"SWITCH_INFO_OPC_INDEX_A")
#define SWITCH_INFO_DATA_B           IpcKeyMgr::getIpcKey (-1,"SWITCH_INFO_DATA_B")
#define SWITCH_INFO_INDEX_B          IpcKeyMgr::getIpcKey (-1,"SWITCH_INFO_INDEX_B")
#define SWITCH_INFO_OPC_INDEX_B      IpcKeyMgr::getIpcKey (-1,"SWITCH_INFO_OPC_INDEX_B")
#define TRUNKBILLING_INFO_DATA_A     IpcKeyMgr::getIpcKey (-1,"TRUNKBILLING_INFO_DATA_A")
#define TRUNKBILLING_INFO_INDEX_A    IpcKeyMgr::getIpcKey (-1,"TRUNKBILLING_INFO_INDEX_A")
#define TRUNKBILLING_INFO_DATA_B     IpcKeyMgr::getIpcKey (-1,"TRUNKBILLING_INFO_DATA_B")
#define TRUNKBILLING_INFO_INDEX_B    IpcKeyMgr::getIpcKey (-1,"TRUNKBILLING_INFO_INDEX_B")
#define TIMESTYPE_INFO_INDEX_A       IpcKeyMgr::getIpcKey (-1,"TIMESTYPE_INFO_INDEX_A")
#define TIMESTYPE_INFO_INDEX_B       IpcKeyMgr::getIpcKey (-1,"TIMESTYPE_INFO_INDEX_B")

#define FILTER_RULE_RESULT_INFO_DATA_A     IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_INFO_DATA_A")
#define FILTER_RULE_RESULT_INFO_DATA_B     IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_INFO_DATA_B")

#define FILTER_RULE_RESULT_CALLING_INFO_INDEX_A    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_CALLING_INFO_INDEX_A")
#define FILTER_RULE_RESULT_CALLING_INFO_INDEX_B    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_CALLING_INFO_INDEX_B")

#define FILTER_RULE_RESULT_CALLED_INFO_INDEX_A    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_CALLED_INFO_INDEX_A")
#define FILTER_RULE_RESULT_CALLED_INFO_INDEX_B    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_CALLED_INFO_INDEX_B")

//
#define FILTER_RULE_RESULT_AREACODE_INFO_INDEX_A    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_AREACODE_INFO_INDEX_A")
#define FILTER_RULE_RESULT_AREACODE_INFO_INDEX_B    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_AREACODE_INFO_INDEX_B")

#define FILTER_RULE_RESULT_GATHERPOT_INFO_INDEX_A    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_GATHERPOT_INFO_INDEX_A")
#define FILTER_RULE_RESULT_GATHERPOT_INFO_INDEX_B    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_GATHERPOT_INFO_INDEX_B")

#define FILTER_RULE_RESULT_TRUNCKIN_INFO_INDEX_A    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_TRUNCKIN_INFO_INDEX_A")
#define FILTER_RULE_RESULT_TRUNCKIN_INFO_INDEX_B    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_TRUNCKIN_INFO_INDEX_B")

#define FILTER_RULE_RESULT_TRUNCKOUT_INFO_INDEX_A    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_TRUNCKOUT_INFO_INDEX_A")
#define FILTER_RULE_RESULT_TRUNCKOUT_INFO_INDEX_B    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_TRUNCKOUT_INFO_INDEX_B")

#define FILTER_RULE_RESULT_SWITCHID_INFO_INDEX_A    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_SWITCHID_INFO_INDEX_A")
#define FILTER_RULE_RESULT_SWITCHID_INFO_INDEX_B    IpcKeyMgr::getIpcKey (-1,"FILTER_RULE_RESULT_SWITCHID_INFO_INDEX_B")

#define CITY_INFO_DATA_A             IpcKeyMgr::getIpcKey (-1,"CITY_INFO_DATA_A")
#define CITY_INFO_INDEX_A            IpcKeyMgr::getIpcKey (-1,"CITY_INFO_INDEX_A")
#define CITY_AREA_INFO_INDEX_A       IpcKeyMgr::getIpcKey (-1,"CITY_AREA_INFO_INDEX_A")
#define CITY_INFO_DATA_B             IpcKeyMgr::getIpcKey (-1,"CITY_INFO_DATA_B")
#define CITY_INFO_INDEX_B            IpcKeyMgr::getIpcKey (-1,"CITY_INFO_INDEX_B")
#define CITY_AREA_INFO_INDEX_B       IpcKeyMgr::getIpcKey (-1,"CITY_AREA_INFO_INDEX_B")

#define ORG_INFO_DATA_A              IpcKeyMgr::getIpcKey (-1,"ORG_INFO_DATA_A")
#define ORG_INFO_INDEX_A             IpcKeyMgr::getIpcKey (-1,"ORG_INFO_INDEX_A")
#define ORG_INFO_DATA_B              IpcKeyMgr::getIpcKey (-1,"ORG_INFO_DATA_B")
#define ORG_INFO_INDEX_B             IpcKeyMgr::getIpcKey (-1,"ORG_INFO_INDEX_B")
#define ORGEX_INFO_DATA_A            IpcKeyMgr::getIpcKey (-1,"ORGEX_INFO_DATA_A")
#define ORGEX_INFO_INDEX_A           IpcKeyMgr::getIpcKey (-1,"ORGEX_INFO_INDEX_A")
#define ORGEX_INFO_DATA_B            IpcKeyMgr::getIpcKey (-1,"ORGEX_INFO_DATA_B")
#define ORGEX_INFO_INDEX_B           IpcKeyMgr::getIpcKey (-1,"ORGEX_INFO_INDEX_B")
#define ORGLONGTYPE_INFO_DATA_A      IpcKeyMgr::getIpcKey (-1,"ORGLONGTYPE_INFO_DATA_A")
#define ORGLONGTYPE_INFO_INDEX_A     IpcKeyMgr::getIpcKey (-1,"ORGLONGTYPE_INFO_INDEX_A")
#define ORGLONGTYPE_INFO_DATA_B      IpcKeyMgr::getIpcKey (-1,"ORGLONGTYPE_INFO_DATA_B")
#define ORGLONGTYPE_INFO_INDEX_B     IpcKeyMgr::getIpcKey (-1,"ORGLONGTYPE_INFO_INDEX_B")
#define HEADEX_INFO_DATA_A           IpcKeyMgr::getIpcKey (-1,"HEADEX_INFO_DATA_A")
#define HEADEX_INFO_INDEX_A          IpcKeyMgr::getIpcKey (-1,"HEADEX_INFO_INDEX_A")
#define HEADEX_INFO_DATA_B           IpcKeyMgr::getIpcKey (-1,"HEADEX_INFO_DATA_B")
#define HEADEX_INFO_INDEX_B          IpcKeyMgr::getIpcKey (-1,"HEADEX_INFO_INDEX_B")
#define LOCALHEAD_INFO_DATA_A        IpcKeyMgr::getIpcKey (-1,"LOCALHEAD_INFO_DATA_A")
#define LOCALHEAD_INFO_INDEX_A       IpcKeyMgr::getIpcKey (-1,"LOCALHEAD_INFO_INDEX_A")
#define LOCALHEAD_INFO_DATA_B        IpcKeyMgr::getIpcKey (-1,"LOCALHEAD_INFO_DATA_B")
#define LOCALHEAD_INFO_INDEX_B       IpcKeyMgr::getIpcKey (-1,"LOCALHEAD_INFO_INDEX_B")

#define EVENTTYPE_INFO_DATA_A          IpcKeyMgr::getIpcKey (-1,"EVENTTYPE_INFO_DATA_A")
#define EVENTTYPE_INFO_INDEX_A         IpcKeyMgr::getIpcKey (-1,"EVENTTYPE_INFO_INDEX_A")
#define EVENTTYPE_INFO_DATA_B          IpcKeyMgr::getIpcKey (-1,"EVENTTYPE_INFO_DATA_B")
#define EVENTTYPE_INFO_INDEX_B         IpcKeyMgr::getIpcKey (-1,"EVENTTYPE_INFO_INDEX_B")
#define EVENTTYPEGROUP_INFO_DATA_A     IpcKeyMgr::getIpcKey (-1,"EVENTTYPEGROUP_INFO_DATA_A")
#define EVENTTYPEGROUP_INFO_INDEX_A    IpcKeyMgr::getIpcKey (-1,"EVENTTYPEGROUP_INFO_INDEX_A")
#define EVENTTYPEGROUP_INFO_DATA_B     IpcKeyMgr::getIpcKey (-1,"EVENTTYPEGROUP_INFO_DATA_B")
#define EVENTTYPEGROUP_INFO_INDEX_B    IpcKeyMgr::getIpcKey (-1,"EVENTTYPEGROUP_INFO_INDEX_B")

#define OFFER_INFO_DATA_A              IpcKeyMgr::getIpcKey (-1,"OFFER_INFO_DATA_A")
#define OFFER_INFO_INDEX_A             IpcKeyMgr::getIpcKey (-1,"OFFER_INFO_INDEX_A")
#define OFFER_INFO_DATA_B              IpcKeyMgr::getIpcKey (-1,"OFFER_INFO_DATA_B")
#define OFFER_INFO_INDEX_B             IpcKeyMgr::getIpcKey (-1,"OFFER_INFO_INDEX_B")
#define OFFER_INFO_SPECATTR_INDEX_A    IpcKeyMgr::getIpcKey (-1,"OFFER_INFO_SPECATTR_INDEX_A")
#define OFFER_INFO_SPECATTR_INDEX_B    IpcKeyMgr::getIpcKey (-1,"OFFER_INFO_SPECATTR_INDEX_B")
#define OFFER_INFO_NUMATTR_INDEX_A     IpcKeyMgr::getIpcKey (-1,"OFFER_INFO_NUMATTR_INDEX_A")
#define OFFER_INFO_NUMATTR_INDEX_B     IpcKeyMgr::getIpcKey (-1,"OFFER_INFO_NUMATTR_INDEX_B")
#define OFFER_INFO_FREE_INDEX_A        IpcKeyMgr::getIpcKey (-1,"OFFER_INFO_FREE_INDEX_A")
#define OFFER_INFO_FREE_INDEX_B        IpcKeyMgr::getIpcKey (-1,"OFFER_INFO_FREE_INDEX_B")

#define NBR_MAP_INFO_DATA_A            IpcKeyMgr::getIpcKey (-1,"NBR_MAP_INFO_DATA_A")
#define NBR_MAP_INDEX1_INDEX_A         IpcKeyMgr::getIpcKey (-1,"NBR_MAP_INDEX1_INDEX_A")
#define NBR_MAP_INDEX2_INDEX_A         IpcKeyMgr::getIpcKey (-1,"NBR_MAP_INDEX2_INDEX_A")
#define NBR_MAP_INDEX4_INDEX_A         IpcKeyMgr::getIpcKey (-1,"NBR_MAP_INDEX4_INDEX_A")
#define NBR_MAP_INDEX8_INDEX_A         IpcKeyMgr::getIpcKey (-1,"NBR_MAP_INDEX8_INDEX_A")
#define NBR_MAP_INFO_DATA_B            IpcKeyMgr::getIpcKey (-1,"NBR_MAP_INFO_DATA_B")
#define NBR_MAP_INDEX1_INDEX_B         IpcKeyMgr::getIpcKey (-1,"NBR_MAP_INDEX1_INDEX_B")
#define NBR_MAP_INDEX2_INDEX_B         IpcKeyMgr::getIpcKey (-1,"NBR_MAP_INDEX2_INDEX_B")
#define NBR_MAP_INDEX4_INDEX_B         IpcKeyMgr::getIpcKey (-1,"NBR_MAP_INDEX4_INDEX_B")
#define NBR_MAP_INDEX8_INDEX_B         IpcKeyMgr::getIpcKey (-1,"NBR_MAP_INDEX8_INDEX_B")

#define ACCTITEM_INFO_DATA_A           IpcKeyMgr::getIpcKey (-1,"ACCTITEM_INFO_DATA_A")
#define ACCTITEM_INFO_INDEX_A          IpcKeyMgr::getIpcKey (-1,"ACCTITEM_INFO_INDEX_A")
#define ACCTITEM_INFO_DATA_B           IpcKeyMgr::getIpcKey (-1,"ACCTITEM_INFO_DATA_B")
#define ACCTITEM_INFO_INDEX_B          IpcKeyMgr::getIpcKey (-1,"ACCTITEM_INFO_INDEX_B")
#define ACCTITEM_GROUP_INFO_DATA_A           IpcKeyMgr::getIpcKey (-1,"ACCTITEM_GROUP_INFO_DATA_A")
#define ACCTITEM_GROUP_INFO_INDEX_A          IpcKeyMgr::getIpcKey (-1,"ACCTITEM_GROUP_INFO_INDEX_A")
#define ACCTITEM_GROUP_INFO_DATA_B           IpcKeyMgr::getIpcKey (-1,"ACCTITEM_GROUP_INFO_DATA_B")
#define ACCTITEM_GROUP_INFO_INDEX_B          IpcKeyMgr::getIpcKey (-1,"ACCTITEM_GROUP_INFO_INDEX_B")
#define ACCTITEM_GROUPB_INFO_DATA_A           IpcKeyMgr::getIpcKey (-1,"ACCTITEM_GROUPB_INFO_DATA_A")
#define ACCTITEM_GROUPB_INFO_INDEX_A          IpcKeyMgr::getIpcKey (-1,"ACCTITEM_GROUPB_INFO_INDEX_A")
#define ACCTITEM_GROUPB_INFO_DATA_B           IpcKeyMgr::getIpcKey (-1,"ACCTITEM_GROUPB_INFO_DATA_B")
#define ACCTITEM_GROUPB_INFO_INDEX_B          IpcKeyMgr::getIpcKey (-1,"ACCTITEM_GROUPB_INFO_INDEX_B")
#define ACCTITEM_BASICGROUPB_INFO_DATA_A      IpcKeyMgr::getIpcKey (-1,"ACCTITEM_BASICGROUPB_INFO_DATA_A")
#define ACCTITEM_BASICGROUPB_INFO_INDEX_A     IpcKeyMgr::getIpcKey (-1,"ACCTITEM_BASICGROUPB_INFO_INDEX_A")
#define ACCTITEM_BASICGROUPB_INFO_DATA_B      IpcKeyMgr::getIpcKey (-1,"ACCTITEM_BASICGROUPB_INFO_DATA_B")
#define ACCTITEM_BASICGROUPB_INFO_INDEX_B     IpcKeyMgr::getIpcKey (-1,"ACCTITEM_BASICGROUPB_INFO_INDEX_B")
#define ACCTITEM_DISCTGROUPB_INFO_DATA_A      IpcKeyMgr::getIpcKey (-1,"ACCTITEM_DISCTGROUPB_INFO_DATA_A")
#define ACCTITEM_DISCTGROUPB_INFO_INDEX_A     IpcKeyMgr::getIpcKey (-1,"ACCTITEM_DISCTGROUPB_INFO_INDEX_A")
#define ACCTITEM_DISCTGROUPB_INFO_DATA_B      IpcKeyMgr::getIpcKey (-1,"ACCTITEM_DISCTGROUPB_INFO_DATA_B")
#define ACCTITEM_DISCTGROUPB_INFO_INDEX_B     IpcKeyMgr::getIpcKey (-1,"ACCTITEM_DISCTGROUPB_INFO_INDEX_B")
#define ACCTITEM_INCLUDEDISCT_INFO_INDEX_A    IpcKeyMgr::getIpcKey (-1,"ACCTITEM_INCLUDEDISCT_INFO_INDEX_A")
#define ACCTITEM_INCLUDEDISCT_INFO_INDEX_B    IpcKeyMgr::getIpcKey (-1,"ACCTITEM_INCLUDEDISCT_INFO_INDEX_B")

//改进效率用
#define ACCTITEM_STR_INFO_INDEX_A          IpcKeyMgr::getIpcKey (-1,"ACCTITEM_STR_INFO_INDEX_A")
#define ACCTITEM_STR_INFO_INDEX_B          IpcKeyMgr::getIpcKey (-1,"ACCTITEM_STR_INFO_INDEX_B")

#define ACCTITEM_GROUP_STR_INFO_INDEX_A          IpcKeyMgr::getIpcKey (-1,"ACCTITEM_GROUP_STR_INFO_INDEX_A")
#define ACCTITEM_GROUP_STR_INFO_INDEX_B          IpcKeyMgr::getIpcKey (-1,"ACCTITEM_GROUP_STR_INFO_INDEX_B")

#define ACCTITEM_GROUPB_STR_INFO_INDEX_A          IpcKeyMgr::getIpcKey (-1,"ACCTITEM_GROUPB_STR_INFO_INDEX_A")
#define ACCTITEM_GROUPB_STR_INFO_INDEX_B          IpcKeyMgr::getIpcKey (-1,"ACCTITEM_GROUPB_STR_INFO_INDEX_B")
//
#define LONGEVENTTYPERULE_INFO_DATA_A         IpcKeyMgr::getIpcKey (-1,"LONGEVENTTYPERULE_INFO_DATA_A")
#define LONGEVENTTYPERULE_INFO_DATA_B         IpcKeyMgr::getIpcKey (-1,"LONGEVENTTYPERULE_INFO_DATA_B")
#define LONGEVENTTYPERULEEX_INFO_DATA_A       IpcKeyMgr::getIpcKey (-1,"LONGEVENTTYPERULEEX_INFO_DATA_A")
#define LONGEVENTTYPERULEEX_INFO_DATA_B       IpcKeyMgr::getIpcKey (-1,"LONGEVENTTYPERULEEX_INFO_DATA_B")

#define PRICINGPLANTYPE_INFO_INDEX_A          IpcKeyMgr::getIpcKey (-1,"PRICINGPLANTYPE_INFO_INDEX_A")
#define PRICINGPLANTYPE_INFO_INDEX_B          IpcKeyMgr::getIpcKey (-1,"PRICINGPLANTYPE_INFO_INDEX_B")

#define STATECONVERT_INFO_DATA_A              IpcKeyMgr::getIpcKey (-1,"STATECONVERT_INFO_DATA_A")
#define STATECONVERT_INFO_INDEX_A             IpcKeyMgr::getIpcKey (-1,"STATECONVERT_INFO_INDEX_A")
#define STATECONVERT_INFO_DATA_B              IpcKeyMgr::getIpcKey (-1,"STATECONVERT_INFO_DATA_B")
#define STATECONVERT_INFO_INDEX_B             IpcKeyMgr::getIpcKey (-1,"STATECONVERT_INFO_INDEX_B")

#define VALUEMAP_INFO_DATA_A                  IpcKeyMgr::getIpcKey (-1,"VALUEMAP_INFO_DATA_A")
#define VALUEMAP_INFO_INDEX_A                 IpcKeyMgr::getIpcKey (-1,"VALUEMAP_INFO_INDEX_A")
#define VALUEMAP_INFO_DATA_B                  IpcKeyMgr::getIpcKey (-1,"VALUEMAP_INFO_DATA_B")
#define VALUEMAP_INFO_INDEX_B                 IpcKeyMgr::getIpcKey (-1,"VALUEMAP_INFO_INDEX_B")

#define ZONE_ITEM_INFO_DATA_A                 IpcKeyMgr::getIpcKey (-1,"ZONE_ITEM_INFO_DATA_A")
#define ZONE_ITEM_INFO_INDEX_A                IpcKeyMgr::getIpcKey (-1,"ZONE_ITEM_INFO_INDEX_A")
#define ZONE_ITEM_INFO_DATA_B                 IpcKeyMgr::getIpcKey (-1,"ZONE_ITEM_INFO_DATA_B")
#define ZONE_ITEM_INFO_INDEX_B                IpcKeyMgr::getIpcKey (-1,"ZONE_ITEM_INFO_INDEX_B")
#define ZONE_ITEM_STR_INDEX_A                 IpcKeyMgr::getIpcKey (-1,"ZONE_ITEM_STR_INDEX_A")
#define ZONE_ITEM_STR_INDEX_B                 IpcKeyMgr::getIpcKey (-1,"ZONE_ITEM_STR_INDEX_B")

#define SPLITACCTITEMGROUP_DATA_A             IpcKeyMgr::getIpcKey (-1,"SPLITACCTITEMGROUP_DATA_A")
#define SPLITACCTITEMGROUP_INDEX_A            IpcKeyMgr::getIpcKey (-1,"SPLITACCTITEMGROUP_INDEX_A")
#define SPLITACCTITEMGROUP_DATA_B             IpcKeyMgr::getIpcKey (-1,"SPLITACCTITEMGROUP_DATA_B")
#define SPLITACCTITEMGROUP_INDEX_B            IpcKeyMgr::getIpcKey (-1,"SPLITACCTITEMGROUP_INDEX_B")
#define SPLITACCTGROUPEVENT_DATA_A            IpcKeyMgr::getIpcKey (-1,"SPLITACCTGROUPEVENT_DATA_A")
#define SPLITACCTGROUPEVENT_INDEX_A           IpcKeyMgr::getIpcKey (-1,"SPLITACCTGROUPEVENT_INDEX_A")
#define SPLITACCTGROUPEVENT_DATA_B            IpcKeyMgr::getIpcKey (-1,"SPLITACCTGROUPEVENT_DATA_B")
#define SPLITACCTGROUPEVENT_INDEX_B           IpcKeyMgr::getIpcKey (-1,"SPLITACCTGROUPEVENT_INDEX_B")

#define DISTANCETYPE_DATA_A                   IpcKeyMgr::getIpcKey (-1,"DISTANCETYPE_DATA_A")
#define DISTANCETYPE_INDEX_A                  IpcKeyMgr::getIpcKey (-1,"DISTANCETYPE_INDEX_A")
#define DISTANCETYPE_DATA_B                   IpcKeyMgr::getIpcKey (-1,"DISTANCETYPE_DATA_B")
#define DISTANCETYPE_INDEX_B                  IpcKeyMgr::getIpcKey (-1,"DISTANCETYPE_INDEX_B")

#define EDGEROAMRULEEX_DATA_A                 IpcKeyMgr::getIpcKey (-1,"EDGEROAMRULEEX_DATA_A")
#define EDGEROAMRULEEX_DATA_B                 IpcKeyMgr::getIpcKey (-1,"EDGEROAMRULEEX_DATA_B")
#define EDGEROAMRULEEX_TYPEA_INDEX_A          IpcKeyMgr::getIpcKey (-1,"EDGEROAMRULEEX_TYPEA_INDEX_A")
#define EDGEROAMRULEEX_TYPEA_INDEX_B          IpcKeyMgr::getIpcKey (-1,"EDGEROAMRULEEX_TYPEA_INDEX_B")
#define EDGEROAMRULEEX_TYPEB_INDEX_A          IpcKeyMgr::getIpcKey (-1,"EDGEROAMRULEEX_TYPEB_INDEX_A")
#define EDGEROAMRULEEX_TYPEB_INDEX_B          IpcKeyMgr::getIpcKey (-1,"EDGEROAMRULEEX_TYPEB_INDEX_B")
#define EDGEROAMRULEEX_MSC_INDEX_A            IpcKeyMgr::getIpcKey (-1,"EDGEROAMRULEEX_MSC_INDEX_A")
#define EDGEROAMRULEEX_MSC_INDEX_B            IpcKeyMgr::getIpcKey (-1,"EDGEROAMRULEEX_MSC_INDEX_B")

//2010.11.17
#define FILEPOOL_DATA_A                       IpcKeyMgr::getIpcKey (-1,"FILEPOOL_DATA_A")
#define FILEPOOL_DATA_B                       IpcKeyMgr::getIpcKey (-1,"FILEPOOL_DATA_B")
#define FILEPOOL_INDEX_A                      IpcKeyMgr::getIpcKey (-1,"FILEPOOL_INDEX_A")
#define FILEPOOL_INDEX_B                      IpcKeyMgr::getIpcKey (-1,"FILEPOOL_INDEX_B")
#define FILEPOOL_COND_DATA_A                  IpcKeyMgr::getIpcKey (-1,"FILEPOOL_COND_DATA_A")
#define FILEPOOL_COND_DATA_B                  IpcKeyMgr::getIpcKey (-1,"FILEPOOL_COND_DATA_B")
#define FILEPOOL_COND_INDEX_A                 IpcKeyMgr::getIpcKey (-1,"FILEPOOL_COND_INDEX_A")
#define FILEPOOL_COND_INDEX_B                 IpcKeyMgr::getIpcKey (-1,"FILEPOOL_COND_INDEX_B")
#define FILETYPE_INDEX_A                      IpcKeyMgr::getIpcKey (-1,"FILETYPE_INDEX_A")
#define FILETYPE_INDEX_B                      IpcKeyMgr::getIpcKey (-1,"FILETYPE_INDEX_B")
#define TLVENCODE_DATA_A                      IpcKeyMgr::getIpcKey (-1,"TLVENCODE_DATA_A")
#define TLVENCODE_DATA_B                      IpcKeyMgr::getIpcKey (-1,"TLVENCODE_DATA_B")
#define TLVENCODE_INDEX_A                     IpcKeyMgr::getIpcKey (-1,"TLVENCODE_INDEX_A")
#define TLVENCODE_INDEX_B                     IpcKeyMgr::getIpcKey (-1,"TLVENCODE_INDEX_B")
#define TLVDECODE_DATA_A                      IpcKeyMgr::getIpcKey (-1,"TLVDECODE_DATA_A")
#define TLVDECODE_DATA_B                      IpcKeyMgr::getIpcKey (-1,"TLVDECODE_DATA_B")
#define TLVDECODE_INDEX_A                     IpcKeyMgr::getIpcKey (-1,"TLVDECODE_INDEX_A")
#define TLVDECODE_INDEX_B                     IpcKeyMgr::getIpcKey (-1,"TLVDECODE_INDEX_B")
#define BILLINGNODE_DATA_A                    IpcKeyMgr::getIpcKey (-1,"BILLINGNODE_DATA_A")
#define BILLINGNODE_DATA_B                    IpcKeyMgr::getIpcKey (-1,"BILLINGNODE_DATA_B")
#define BILLINGNODE_INDEX_A                   IpcKeyMgr::getIpcKey (-1,"BILLINGNODE_INDEX_A")
#define BILLINGNODE_INDEX_B                   IpcKeyMgr::getIpcKey (-1,"BILLINGNODE_INDEX_B")
#define FILEPARSERULE_INDEX_A                 IpcKeyMgr::getIpcKey (-1,"FILEPARSERULE_INDEX_A")
#define FILEPARSERULE_INDEX_B                 IpcKeyMgr::getIpcKey (-1,"FILEPARSERULE_INDEX_B")
#define SWITCHCODE_DATA_A                     IpcKeyMgr::getIpcKey (-1,"SWITCHCODE_DATA_A")
#define SWITCHCODE_DATA_B                     IpcKeyMgr::getIpcKey (-1,"SWITCHCODE_DATA_B")
#define SWITCHCODE_INDEX_A                    IpcKeyMgr::getIpcKey (-1,"SWITCHCODE_INDEX_A")
#define SWITCHCODE_INDEX_B                    IpcKeyMgr::getIpcKey (-1,"SWITCHCODE_INDEX_B")
//2010.12.20    
#define ROAMTARIFF_DATA_A                     IpcKeyMgr::getIpcKey (-1,"ROAMTARIFF_DATA_A")
#define ROAMTARIFF_DATA_B                     IpcKeyMgr::getIpcKey (-1,"ROAMTARIFF_DATA_B")
#define ROAMTARIFF_INDEX_A                    IpcKeyMgr::getIpcKey (-1,"ROAMTARIFF_INDEX_A")
#define ROAMTARIFF_INDEX_B                    IpcKeyMgr::getIpcKey (-1,"ROAMTARIFF_INDEX_B")
#define ROAMCARRIER_DATA_A                    IpcKeyMgr::getIpcKey (-1,"ROAMCARRIER_DATA_A")
#define ROAMCARRIER_DATA_B                    IpcKeyMgr::getIpcKey (-1,"ROAMCARRIER_DATA_B")
#define ROAMCARRIER_INDEX_A                   IpcKeyMgr::getIpcKey (-1,"ROAMCARRIER_INDEX_A")
#define ROAMCARRIER_INDEX_B                   IpcKeyMgr::getIpcKey (-1,"ROAMCARRIER_INDEX_B")
#define TARIFFSECTOR_INDEX_A                  IpcKeyMgr::getIpcKey (-1,"TARIFFSECTOR_INDEX_A")
#define TARIFFSECTOR_INDEX_B                  IpcKeyMgr::getIpcKey (-1,"TARIFFSECTOR_INDEX_B")
#define COUNTRY_DATA_A                        IpcKeyMgr::getIpcKey (-1,"COUNTRY_DATA_A")
#define COUNTRY_DATA_B                        IpcKeyMgr::getIpcKey (-1,"COUNTRY_DATA_B")
#define COUNTRY_CODE_INDEX_A                  IpcKeyMgr::getIpcKey (-1,"COUNTRY_CODE_INDEX_A")
#define COUNTRY_CODE_INDEX_B                  IpcKeyMgr::getIpcKey (-1,"COUNTRY_CODE_INDEX_B")
#define COUNTRY_AREA_INDEX_A                  IpcKeyMgr::getIpcKey (-1,"COUNTRY_AREA_INDEX_A")
#define COUNTRY_AREA_INDEX_B                  IpcKeyMgr::getIpcKey (-1,"COUNTRY_AREA_INDEX_B")
#define ROAMSPONSOR_DATA_A                    IpcKeyMgr::getIpcKey (-1,"ROAMSPONSOR_DATA_A")
#define ROAMSPONSOR_DATA_B                    IpcKeyMgr::getIpcKey (-1,"ROAMSPONSOR_DATA_B")
#define ROAMSPONSOR_INDEX_A                   IpcKeyMgr::getIpcKey (-1,"ROAMSPONSOR_INDEX_A")
#define ROAMSPONSOR_INDEX_B                   IpcKeyMgr::getIpcKey (-1,"ROAMSPONSOR_INDEX_B")
#define SPECIALGROUPMEMBER_DATA_A             IpcKeyMgr::getIpcKey (-1,"SPECIALGROUPMEMBER_DATA_A")
#define SPECIALGROUPMEMBER_DATA_B             IpcKeyMgr::getIpcKey (-1,"SPECIALGROUPMEMBER_DATA_B")
#define SPECIALGROUPMEMBER_INDEX_A            IpcKeyMgr::getIpcKey (-1,"SPECIALGROUPMEMBER_INDEX_A")
#define SPECIALGROUPMEMBER_INDEX_B            IpcKeyMgr::getIpcKey (-1,"SPECIALGROUPMEMBER_INDEX_B")

#define PRICINGSECTION_INFO_DATA_A        IpcKeyMgr::getIpcKey (-1,"PRICINGSECTION_INFO_DATA_A")
#define PRICINGSECTION_INFO_INDEX_A       IpcKeyMgr::getIpcKey (-1,"PRICINGSECTION_INFO_INDEX_A")
#define PRICINGSECTION_INFO_DATA_B        IpcKeyMgr::getIpcKey (-1,"PRICINGSECTION_INFO_DATA_B")
#define PRICINGSECTION_INFO_INDEX_B       IpcKeyMgr::getIpcKey (-1,"PRICINGSECTION_INFO_INDEX_B")
#define SECTIONSTRATEGY_INFO_INDEX_A            IpcKeyMgr::getIpcKey (-1,"SECTIONSTRATEGY_INFO_INDEX_A")
#define SECTIONSTRATEGY_INFO_INDEX_B            IpcKeyMgr::getIpcKey (-1,"SECTIONSTRATEGY_INFO_INDEX_B")

#define TARIFF_INFO_DATA_A          IpcKeyMgr::getIpcKey (-1,"TARIFF_INFO_DATA_A")
#define TARIFF_INFO_INDEX_A         IpcKeyMgr::getIpcKey (-1,"TARIFF_INFO_INDEX_A")
#define TARIFF_INFO_DATA_B          IpcKeyMgr::getIpcKey (-1,"TARIFF_INFO_DATA_B")
#define TARIFF_INFO_INDEX_B         IpcKeyMgr::getIpcKey (-1,"TARIFF_INFO_INDEX_B")

#define DISCOUNT_INFO_DATA_A        IpcKeyMgr::getIpcKey (-1,"DISCOUNT_INFO_DATA_A")
#define DISCOUNT_INFO_INDEX_A       IpcKeyMgr::getIpcKey (-1,"DISCOUNT_INFO_INDEX_A")
#define DISCOUNT_INFO_DATA_B        IpcKeyMgr::getIpcKey (-1,"DISCOUNT_INFO_DATA_B")
#define DISCOUNT_INFO_INDEX_B       IpcKeyMgr::getIpcKey (-1,"DISCOUNT_INFO_INDEX_B")

#define DISCTAPPLYER_INFO_DATA_A        IpcKeyMgr::getIpcKey (-1,"DISCTAPPLYER_INFO_DATA_A")
#define DISCTAPPLYER_INFO_INDEX_A       IpcKeyMgr::getIpcKey (-1,"DISCTAPPLYER_INFO_INDEX_A")
#define DISCTAPPLYER_INFO_DATA_B        IpcKeyMgr::getIpcKey (-1,"DISCTAPPLYER_INFO_DATA_B")
#define DISCTAPPLYER_INFO_INDEX_B       IpcKeyMgr::getIpcKey (-1,"DISCTAPPLYER_INFO_INDEX_B")

#define DISCTAPPLYEROBJ_INFO_DATA_A        IpcKeyMgr::getIpcKey (-1,"DISCTAPPLYEROBJ_INFO_DATA_A")
#define DISCTAPPLYEROBJ_INFO_INDEX_A       IpcKeyMgr::getIpcKey (-1,"DISCTAPPLYEROBJ_INFO_INDEX_A")
#define DISCTAPPLYEROBJ_INFO_DATA_B        IpcKeyMgr::getIpcKey (-1,"DISCTAPPLYEROBJ_INFO_DATA_B")
#define DISCTAPPLYEROBJ_INFO_INDEX_B       IpcKeyMgr::getIpcKey (-1,"DISCTAPPLYEROBJ_INFO_INDEX_B")

#define PERCENT_INFO_DATA_A          IpcKeyMgr::getIpcKey (-1,"PERCENT_INFO_DATA_A")
#define PERCENT_INFO_INDEX_A         IpcKeyMgr::getIpcKey (-1,"PERCENT_INFO_INDEX_A")
#define PERCENT_INFO_DATA_B          IpcKeyMgr::getIpcKey (-1,"PERCENT_INFO_DATA_B")
#define PERCENT_INFO_INDEX_B       	 IpcKeyMgr::getIpcKey (-1,"PERCENT_INFO_INDEX_B")
#define LIFECYCLE_INFO_DATA_A        IpcKeyMgr::getIpcKey (-1,"LIFECYCLE_INFO_DATA_A")
#define LIFECYCLE_INFO_INDEX_A			 	   IpcKeyMgr::getIpcKey (-1,"LIFECYCLE_INFO_INDEX_A")
#define LIFECYCLE_INFO_DATA_B				   IpcKeyMgr::getIpcKey (-1,"LIFECYCLE_INFO_DATA_B")
#define LIFECYCLE_INFO_INDEX_B			 	   IpcKeyMgr::getIpcKey (-1,"LIFECYCLE_INFO_INDEX_B")

#define MEASURE_INFO_DATA_A        	 		 IpcKeyMgr::getIpcKey (-1,"MEASURE_INFO_DATA_A")
#define MEASURE_INFO_INDEX_A			 	 IpcKeyMgr::getIpcKey (-1,"MEASURE_INFO_INDEX_A")
#define MEASURE_INFO_DATA_B				 	 IpcKeyMgr::getIpcKey (-1,"MEASURE_INFO_DATA_B")
#define MEASURE_INFO_INDEX_B			 	 IpcKeyMgr::getIpcKey (-1,"MEASURE_INFO_INDEX_B")
#define STRATEGYMEASURE_INFO_INDEX_A    	 IpcKeyMgr::getIpcKey (-1,"STRATEGYMEASURE_INFO_INDEX_A")
#define STRATEGYMEASURE_INFO_INDEX_B    	 IpcKeyMgr::getIpcKey (-1,"STRATEGYMEASURE_INFO_INDEX_B")


#define COMBINE_INFO_DATA_A        	 			IpcKeyMgr::getIpcKey (-1,"COMBINE_INFO_DATA_A")
#define COMBINE_INFO_INDEX_A			 	 	IpcKeyMgr::getIpcKey (-1,"COMBINE_INFO_INDEX_A")	
#define PLAN_COMBINE_INFO_INDEX_A     			IpcKeyMgr::getIpcKey (-1,"PLAN_COMBINE_INFO_INDEX_A")
#define STRATEGY_COMBINE_INFO_INDEX_A     		IpcKeyMgr::getIpcKey (-1,"STRATEGY_COMBINE_INFO_INDEX_A")

#define COMBINE_INFO_DATA_B				 	  		IpcKeyMgr::getIpcKey (-1,"COMBINE_INFO_DATA_B")
#define COMBINE_INFO_INDEX_B			 	  		IpcKeyMgr::getIpcKey (-1,"COMBINE_INFO_INDEX_B")
#define PLAN_COMBINE_INFO_INDEX_B     				IpcKeyMgr::getIpcKey (-1,"PLAN_COMBINE_INFO_INDEX_B")
#define STRATEGY_COMBINE_INFO_INDEX_B     			IpcKeyMgr::getIpcKey (-1,"STRATEGY_COMBINE_INFO_INDEX_B")


#define FILTERCOND_INFO_DATA_A        	IpcKeyMgr::getIpcKey (-1,"FILTERCOND_INFO_DATA_A")
#define FILTERCOND_INFO_INDEX_A			IpcKeyMgr::getIpcKey (-1,"FILTERCOND_INFO_INDEX_A")
#define FILTERCOND_INFO_DATA_B			IpcKeyMgr::getIpcKey (-1,"FILTERCOND_INFO_DATA_B")
#define FILTERCOND_INFO_INDEX_B			IpcKeyMgr::getIpcKey (-1,"FILTERCOND_INFO_INDEX_B")

#define CONDITIONPOINT_INFO_DATA_A     IpcKeyMgr::getIpcKey (-1,"CONDITIONPOINT_INFO_DATA_A")
#define CONDITIONPOINT_INFO_DATA_B     IpcKeyMgr::getIpcKey (-1,"CONDITIONPOINT_INFO_DATA_B")

#define USAGETYPE_INFO_DATA_A        IpcKeyMgr::getIpcKey (-1,"USAGETYPE_INFO_DATA_A")
#define USAGETYPE_INFO_INDEX_A       IpcKeyMgr::getIpcKey (-1,"USAGETYPE_INFO_INDEX_A")
#define USAGETYPE_INFO_DATA_B        IpcKeyMgr::getIpcKey (-1,"USAGETYPE_INFO_DATA_B")
#define USAGETYPE_INFO_INDEX_B       IpcKeyMgr::getIpcKey (-1,"USAGETYPE_INFO_INDEX_B")

#define ACCTTYPECFG_TIMECOND_INFO_DATA_A		IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_TIMECOND_INFO_DATA_A")
#define ACCTTYPECFG_TIMECOND_INFO_INDEX_A     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_TIMECOND_INFO_INDEX_A")
#define ACCTTYPECFG_TIMECOND_INFO_DATA_B		IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_TIMECOND_INFO_DATA_B")
#define ACCTTYPECFG_TIMECOND_INFO_INDEX_B     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_TIMECOND_INFO_INDEX_B")

#define ACCTTYPECFG_EVENTTYPECOND_INFO_DATA_A		IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_EVENTTYPECOND_INFO_DATA_A")
#define ACCTTYPECFG_EVENTTYPECOND_INFO_INDEX_A     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_EVENTTYPECOND_INFO_INDEX_A")
#define ACCTTYPECFG_EVENTTYPECOND_INFO_DATA_B		IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_EVENTTYPECOND_INFO_DATA_B")
#define ACCTTYPECFG_EVENTTYPECOND_INFO_INDEX_B     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_EVENTTYPECOND_INFO_INDEX_B")

#define ACCTTYPECFG_CYCLETYPE_INFO_DATA_A		IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_CYCLETYPE_INFO_DATA_A")
#define ACCTTYPECFG_CYCLETYPE_INFO_INDEX_A     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_CYCLETYPE_INFO_INDEX_A")
#define ACCTTYPECFG_CYCLETYPE_INFO_DATA_B		IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_CYCLETYPE_INFO_DATA_B")
#define ACCTTYPECFG_CYCLETYPE_INFO_INDEX_B     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_CYCLETYPE_INFO_INDEX_B")

#define ACCTTYPECFG_ACCTTYPE_INFO_DATA_A		IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_ACCTTYPE_INFO_DATA_A")
#define ACCTTYPECFG_ACCTTYPE_INFO_INDEX_A     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_ACCTTYPE_INFO_INDEX_A")
#define ACCTTYPECFG_ACCTTYPE_INFO_DATA_B		IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_ACCTTYPE_INFO_DATA_B")
#define ACCTTYPECFG_ACCTTYPE_INFO_INDEX_B     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_ACCTTYPE_INFO_INDEX_B")

#define ACCTTYPECFG_OFFERACCTRELATION_INFO_DATA_A	    IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_OFFERACCTRELATION_INFO_DATA_A")
#define ACCTTYPECFG_OFFERACCTRELATION_INFO_INDEX_A     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_OFFERACCTRELATION_INFO_INDEX_A")
#define ACCTTYPECFG_OFFERACCTRELATION_INFO_DATA_B		IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_OFFERACCTRELATION_INFO_DATA_B")
#define ACCTTYPECFG_OFFERACCTRELATION_INFO_INDEX_B     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_OFFERACCTRELATION_INFO_INDEX_B")

#define ACCTTYPECFG_OFFERACCTEXIST_INFO_INDEX_A     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_OFFERACCTEXIST_INFO_INDEX_A")
#define ACCTTYPECFG_OFFERACCTEXIST_INFO_INDEX_B     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_OFFERACCTEXIST_INFO_INDEX_B")

//loadSpecialCycleType
#ifdef EVENT_AGGR_RECYCLE_USE
    #define ACCTTYPECFG_CYCLETYPEID_INFO_INDEX_A     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_CYCLETYPEID_INFO_INDEX_A")
    #define ACCTTYPECFG_CYCLETYPEID_INFO_INDEX_B     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_CYCLETYPEID_INFO_INDEX_B")
	#define ACCTTYPECFG_CYCLETYPEID_INFO_DATA_A     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_CYCLETYPEID_INFO_DATA_A")
    #define ACCTTYPECFG_CYCLETYPEID_INFO_DATA_B     	IpcKeyMgr::getIpcKey (-1,"ACCTTYPECFG_CYCLETYPEID_INFO_DATA_B")
#endif

#define OFFERCOMBINE_INFO_INDEX_A     		IpcKeyMgr::getIpcKey (-1,"OFFERCOMBINE_INFO_INDEX_A")
#define OFFERCOMBINE_INFO_INDEX_B     		IpcKeyMgr::getIpcKey (-1,"OFFERCOMBINE_INFO_INDEX_B")

#define OFFERDISCT_INFO_INDEX_A     		IpcKeyMgr::getIpcKey (-1,"OFFERDISCT_INFO_INDEX_A")
#define OFFERDISCT_INFO_INDEX_B     		IpcKeyMgr::getIpcKey (-1,"OFFERDISCT_INFO_INDEX_B")

#define PRODUCTCOMBINE_INFO_INDEX_A    			IpcKeyMgr::getIpcKey (-1,"PRODUCTCOMBINE_INFO_INDEX_A")
#define PRODUCTCOMBINE_INFO_INDEX_B    			IpcKeyMgr::getIpcKey (-1,"PRODUCTCOMBINE_INFO_INDEX_B")

#define SPLITCFG_INFO_DATA_A        	    IpcKeyMgr::getIpcKey (-1,"SPLITCFG_INFO_DATA_A")
#define SPLITCFG_INFO_INDEX_A			    IpcKeyMgr::getIpcKey (-1,"SPLITCFG_INFO_INDEX_A")
#define SPLITCFG_INFO_DATA_B			    IpcKeyMgr::getIpcKey (-1,"SPLITCFG_INFO_DATA_B")
#define SPLITCFG_INFO_INDEX_B		        IpcKeyMgr::getIpcKey (-1,"SPLITCFG_INFO_INDEX_B")

#define VIRTUAL_STRATEGY_COMBINE_INFO_DATA_A		IpcKeyMgr::getIpcKey (-1,"VIRTUAL_STRATEGY_COMBINE_INFO_DATA_A")
#define VIRTUAL_STRATEGY_COMBINE_INFO_INDEX_A     	IpcKeyMgr::getIpcKey (-1,"VIRTUAL_STRATEGY_COMBINE_INFO_INDEX_A")
#define VIRTUAL_STRATEGY_COMBINE_INFO_DATA_B	    IpcKeyMgr::getIpcKey (-1,"VIRTUAL_STRATEGY_COMBINE_INFO_DATA_B")
#define VIRTUAL_STRATEGY_COMBINE_INFO_INDEX_B     	IpcKeyMgr::getIpcKey (-1,"VIRTUAL_STRATEGY_COMBINE_INFO_INDEX_B")

#ifdef  WHOLE_DISCOUNT
    #define WHOLEDISCOUNT_INFO_DATA_A					IpcKeyMgr::getIpcKey (-1,"WHOLEDISCOUNT_INFO_DATA_A")
    #define WHOLEDISCOUNT_INFO_DATA_B				 	IpcKeyMgr::getIpcKey (-1,"WHOLEDISCOUNT_INFO_DATA_B")
#endif

#ifdef  GROUP_CONTROL
    #define CGCOMBINE_INFO_DATA_A        	 	 			 IpcKeyMgr::getIpcKey (-1,"CGCOMBINE_INFO_DATA_A")
    #define CGCOMBINE_INFO_INDEX_A			 	   			 IpcKeyMgr::getIpcKey (-1,"CGCOMBINE_INFO_INDEX_A")
    #define CGCOMBINE_INFO_DATA_B				 	 	 	 IpcKeyMgr::getIpcKey (-1,"CGCOMBINE_INFO_DATA_B")
    #define CGCOMBINE_INFO_INDEX_B			 	   			 IpcKeyMgr::getIpcKey (-1,"CGCOMBINE_INFO_INDEX_B")
#endif

#define COMBINERELATION_INFO_DATA_A        	 	 IpcKeyMgr::getIpcKey (-1,"COMBINERELATION_INFO_DATA_A")
#define COMBINERELATION_INFO_INDEX_A			 IpcKeyMgr::getIpcKey (-1,"COMBINERELATION_INFO_INDEX_A")
#define COMBINERELATION_INFO_DATA_B				 IpcKeyMgr::getIpcKey (-1,"COMBINERELATION_INFO_DATA_B")
#define COMBINERELATION_INFO_INDEX_B			 IpcKeyMgr::getIpcKey (-1,"COMBINERELATION_INFO_INDEX_B")

#define SPECIALNBRSTRATEGY_INFO_DATA_A				IpcKeyMgr::getIpcKey (-1,"SPECIALNBRSTRATEGY_INFO_DATA_A")
#define SPECIALNBRSTRATEGY_INFO_INDEX_A     		IpcKeyMgr::getIpcKey (-1,"SPECIALNBRSTRATEGY_INFO_INDEX_A")
#define SPECIALNBROFFER_INFO_INDEX_A     			IpcKeyMgr::getIpcKey (-1,"SPECIALNBROFFER_INFO_INDEX_A")	
#define SPECIALNBRSTRATEGY_INFO_DATA_B				IpcKeyMgr::getIpcKey (-1,"SPECIALNBRSTRATEGY_INFO_DATA_B")
#define SPECIALNBRSTRATEGY_INFO_INDEX_B     		IpcKeyMgr::getIpcKey (-1,"SPECIALNBRSTRATEGY_INFO_INDEX_B")
#define SPECIALNBROFFER_INFO_INDEX_B     			IpcKeyMgr::getIpcKey (-1,"SPECIALNBROFFER_INFO_INDEX_B")

#define HOSTINFO_INFO_DATA_A        	    IpcKeyMgr::getIpcKey (-1,"HOSTINFO_INFO_DATA_A")
#define HOSTINFO_INFO_INDEX_A			    IpcKeyMgr::getIpcKey (-1,"HOSTINFO_INFO_INDEX_A")
#define HOSTINFO_INFO_DATA_B			    IpcKeyMgr::getIpcKey (-1,"HOSTINFO_INFO_DATA_B")
#define HOSTINFO_INFO_INDEX_B		        IpcKeyMgr::getIpcKey (-1,"HOSTINFO_INFO_INDEX_B")

//重了#define OFFERCOMBINE_INFO_INDEX_A     		IpcKeyMgr::getIpcKey (-1,"OFFERCOMBINE_INFO_INDEX_A")
//重了#define OFFERCOMBINE_INFO_INDEX_B     		IpcKeyMgr::getIpcKey (-1,"OFFERCOMBINE_INFO_INDEX_B")

//重了#define PRODUCTCOMBINE_INFO_INDEX_A    			IpcKeyMgr::getIpcKey (-1,"PRODUCTCOMBINE_INFO_INDEX_A")
//重了#define PRODUCTCOMBINE_INFO_INDEX_B    			IpcKeyMgr::getIpcKey (-1,"PRODUCTCOMBINE_INFO_INDEX_B")

#define GATHERTESK_INFO_DATA_A				IpcKeyMgr::getIpcKey (-1,"GATHERTESK_INFO_DATA_A")
#define GATHERTESK_INFO_INDEX_A     		IpcKeyMgr::getIpcKey (-1,"GATHERTESK_INFO_INDEX_A")
#define ORGGATHERTESK_INFO_INDEX_A     		IpcKeyMgr::getIpcKey (-1,"ORGGATHERTESK_INFO_INDEX_A")	
#define GATHERTESK_INFO_DATA_B				IpcKeyMgr::getIpcKey (-1,"GATHERTESK_INFO_DATA_B")
#define GATHERTESK_INFO_INDEX_B     		IpcKeyMgr::getIpcKey (-1,"GATHERTESK_INFO_INDEX_B")
#define ORGGATHERTESK_INFO_INDEX_B     		IpcKeyMgr::getIpcKey (-1,"ORGGATHERTESK_INFO_INDEX_B")

#define PRODUCTID_INFO_INDEX_A     			IpcKeyMgr::getIpcKey (-1,"PRODUCTID_INFO_INDEX_A")
#define PRODUCTID_INFO_INDEX_B     			IpcKeyMgr::getIpcKey (-1,"PRODUCTID_INFO_INDEX_B")
#define PRODUCTACCTITEMS_INFO_INDEX_A     	IpcKeyMgr::getIpcKey (-1,"PRODUCTACCTITEMS_INFO_INDEX_A")
#define PRODUCTACCTITEMS_INFO_INDEX_B     	IpcKeyMgr::getIpcKey (-1,"PRODUCTACCTITEMS_INFO_INDEX_B")

#define SPECIALTRIFF_INFO_DATA_A        	    IpcKeyMgr::getIpcKey (-1,"SPECIALTRIFF_INFO_DATA_A")
#define SPECIALTRIFF_INFO_INDEX_A			    IpcKeyMgr::getIpcKey (-1,"SPECIALTRIFF_INFO_INDEX_A")
#define SPECIALTRIFF_INFO_DATA_B			    IpcKeyMgr::getIpcKey (-1,"SPECIALTRIFF_INFO_DATA_B")
#define SPECIALTRIFF_INFO_INDEX_B		        IpcKeyMgr::getIpcKey (-1,"SPECIALTRIFF_INFO_INDEX_B")

#define SPECIALTRIFFCHECK_INFO_DATA_A        	    IpcKeyMgr::getIpcKey (-1,"SPECIALTRIFFCHECK_INFO_DATA_A")
#define SPECIALTRIFFCHECK_INFO_INDEX_A			    IpcKeyMgr::getIpcKey (-1,"SPECIALTRIFFCHECK_INFO_INDEX_A")
#define SPECIALTRIFFCHECK_INFO_DATA_B			    IpcKeyMgr::getIpcKey (-1,"SPECIALTRIFFCHECK_INFO_DATA_B")
#define SPECIALTRIFFCHECK_INFO_INDEX_B		        IpcKeyMgr::getIpcKey (-1,"SPECIALTRIFFCHECK_INFO_INDEX_B")

#define SPINFODATA_INFO_DATA_A        	    IpcKeyMgr::getIpcKey (-1,"SPINFODATA_INFO_DATA_A")
#define SPINFODATA_INFO_INDEX_A			    IpcKeyMgr::getIpcKey (-1,"SPINFODATA_INFO_INDEX_A")
#define SPINFODATA_INFO_DATA_B			    IpcKeyMgr::getIpcKey (-1,"SPINFODATA_INFO_DATA_B")
#define SPINFODATA_INFO_INDEX_B		        IpcKeyMgr::getIpcKey (-1,"SPINFODATA_INFO_INDEX_B")

#define GATHERTASKLISTINFO_INFO_DATA_A        	    IpcKeyMgr::getIpcKey (-1,"GATHERTASKLISTINFO_INFO_DATA_A")
#define GATHERTASKLISTINFO_INFO_INDEX_A			    IpcKeyMgr::getIpcKey (-1,"GATHERTASKLISTINFO_INFO_INDEX_A")
#define GATHERTASKLISTINFO_INFO_DATA_B			    IpcKeyMgr::getIpcKey (-1,"GATHERTASKLISTINFO_INFO_DATA_B")
#define GATHERTASKLISTINFO_INFO_INDEX_B		        IpcKeyMgr::getIpcKey (-1,"GATHERTASKLISTINFO_INFO_INDEX_B")

#define SEGMENTINFO_INFO_DATA_A        	    	IpcKeyMgr::getIpcKey (-1,"SEGMENTINFO_INFO_DATA_A")
#define SEGMENTINFO_INFO_INDEX_A			        IpcKeyMgr::getIpcKey (-1,"SEGMENTINFO_INFO_INDEX_A")
#define SEGMENTTYPEINFO_INFO_INDEX_A			    IpcKeyMgr::getIpcKey (-1,"SEGMENTTYPEINFO_INFO_INDEX_A")
#define SEGMENTINFO_INFO_DATA_B			    	    IpcKeyMgr::getIpcKey (-1,"SEGMENTINFO_INFO_DATA_B")
#define SEGMENTINFO_INFO_INDEX_B		          IpcKeyMgr::getIpcKey (-1,"SEGMENTINFO_INFO_INDEX_B")
#define SEGMENTTYPEINFO_INFO_INDEX_B			    IpcKeyMgr::getIpcKey (-1,"SEGMENTTYPEINFO_INFO_INDEX_B")

#define LONGTYPE_INFO_INDEX_A     		IpcKeyMgr::getIpcKey (-1,"LONGTYPE_INFO_INDEX_A")
#define LONGTYPE_INFO_INDEX_B     		IpcKeyMgr::getIpcKey (-1,"LONGTYPE_INFO_INDEX_B")

#define CHECKRULE_RULE_INFO_DATA_A        	    IpcKeyMgr::getIpcKey (-1,"CHECKRULE_RULE_INFO_DATA_A")
#define CHECKRULE_RULE_INFO_INDEX_A				IpcKeyMgr::getIpcKey (-1,"CHECKRULE_RULE_INFO_INDEX_A")
#define CHECKRULE_RULE_INFO_DATA_B			    IpcKeyMgr::getIpcKey (-1,"CHECKRULE_RULE_INFO_DATA_B")
#define CHECKRULE_RULE_INFO_INDEX_B		    	IpcKeyMgr::getIpcKey (-1,"CHECKRULE_RULE_INFO_INDEX_B")

#define CHECKRULE_SUBRULE_INFO_DATA_A        	IpcKeyMgr::getIpcKey (-1,"CHECKRULE_SUBRULE_INFO_DATA_A")
#define CHECKRULE_SUBRULE_INFO_INDEX_A			IpcKeyMgr::getIpcKey (-1,"CHECKRULE_SUBRULE_INFO_INDEX_A")
#define CHECKRULE_SUBRULE_INFO_DATA_B			IpcKeyMgr::getIpcKey (-1,"CHECKRULE_SUBRULE_INFO_DATA_B")
#define CHECKRULE_SUBRULE_INFO_INDEX_B		    IpcKeyMgr::getIpcKey (-1,"CHECKRULE_SUBRULE_INFO_INDEX_B")

#define CHECKARGUMENT_INFO_DATA_A        	    IpcKeyMgr::getIpcKey (-1,"CHECKARGUMENT_INFO_DATA_A")
#define CHECKARGUMENT_INFO_DATA_B        	    IpcKeyMgr::getIpcKey (-1,"CHECKARGUMENT_INFO_DATA_B")

#define CHECKRELATION_INFO_DATA_A        	    IpcKeyMgr::getIpcKey (-1,"CHECKRELATION_INFO_DATA_A")
#define CHECKRELATION_INFO_DATA_B        	    IpcKeyMgr::getIpcKey (-1,"CHECKRELATION_INFO_DATA_B")

#define TRANSFILETYPE_INFO_DATA_A        	IpcKeyMgr::getIpcKey (-1,"TRANSFILETYPE_INFO_DATA_A")
#define TRANSFILETYPE_INFO_INDEX_A			IpcKeyMgr::getIpcKey (-1,"TRANSFILETYPE_INFO_INDEX_A")
#define TRANSFILETYPE_INFO_DATA_B			IpcKeyMgr::getIpcKey (-1,"TRANSFILETYPE_INFO_DATA_B")
#define TRANSFILETYPE_INFO_INDEX_B		    IpcKeyMgr::getIpcKey (-1,"TRANSFILETYPE_INFO_INDEX_B")
	
#define PARSERULE_INFO_DATA_A        	IpcKeyMgr::getIpcKey (-1,"PARSERULE_INFO_DATA_A")
#define PARSERULE_INFO_INDEX_A			IpcKeyMgr::getIpcKey (-1,"PARSERULE_INFO_INDEX_A")
#define PARSERULE_INFO_DATA_B			IpcKeyMgr::getIpcKey (-1,"PARSERULE_INFO_DATA_B")
#define PARSERULE_INFO_INDEX_B		    IpcKeyMgr::getIpcKey (-1,"PARSERULE_INFO_INDEX_B")
#define PARSECOMB_INFO_DATA_A        	IpcKeyMgr::getIpcKey (-1,"PARSECOMB_INFO_DATA_A")
#define PARSECOMB_INFO_DATA_B        	IpcKeyMgr::getIpcKey (-1,"PARSECOMB_INFO_DATA_B")
#define PARSECOMB_INFO_INDEX_A			IpcKeyMgr::getIpcKey (-1,"PARSECOMB_INFO_INDEX_A")
#define PARSECOMB_INFO_INDEX_B			IpcKeyMgr::getIpcKey (-1,"PARSECOMB_INFO_INDEX_B")
#define INSTTABLELIST_INFO_DATA_A		IpcKeyMgr::getIpcKey (-1,"INSTTABLELIST_INFO_DATA_A")
#define INSTTABLELIST_INFO_DATA_B		IpcKeyMgr::getIpcKey (-1,"INSTTABLELIST_INFO_DATA_B")
#define INSTTABLELIST_INFO_INDEX_A		IpcKeyMgr::getIpcKey (-1,"INSTTABLELIST_INFO_INDEX_A")
#define INSTTABLELIST_INFO_INDEX_B		IpcKeyMgr::getIpcKey (-1,"INSTTABLELIST_INFO_INDEX_B")
#define TABLETYPE_INFO_DATA_A			IpcKeyMgr::getIpcKey (-1,"TABLETYPE_INFO_DATA_A")
#define TABLETYPE_INFO_DATA_B			IpcKeyMgr::getIpcKey (-1,"TABLETYPE_INFO_DATA_B")
#define TABLETYPE_INFO_INDEX_A			IpcKeyMgr::getIpcKey (-1,"TABLETYPE_INFO_INDEX_A")
#define TABLETYPE_INFO_INDEX_B			IpcKeyMgr::getIpcKey (-1,"TABLETYPE_INFO_INDEX_B")
#define RENT_CONFIG_INFO_INDEX_A	        IpcKeyMgr::getIpcKey (-1,"RENT_CONFIG_INFO_INDEX_A")
#define RENT_CONFIG_INFO_INDEX_B		IpcKeyMgr::getIpcKey (-1,"RENT_CONFIG_INFO_INDEX_B")
#define NETWORKPRIORITY_INFO_DATA_A				IpcKeyMgr::getIpcKey (-1,"NETWORKPRIORITY_INFO_DATA_A")
#define NETWORKPRIORITY_INFO_DATA_B				IpcKeyMgr::getIpcKey (-1,"NETWORKPRIORITY_INFO_DATA_B")
#define NETWORKPRIORITY_INFO_INDEX_A			IpcKeyMgr::getIpcKey (-1,"NETWORKPRIORITY_INFO_INDEX_A")
#define NETWORKPRIORITY_INFO_INDEX_B			IpcKeyMgr::getIpcKey (-1,"NETWORKPRIORITY_INFO_INDEX_B")
#define TRUNKTOBENBR_INFO_DATA_A        	IpcKeyMgr::getIpcKey (-1,"TRUNKTOBENBR_INFO_DATA_A")
#define TRUNKTOBENBR_INFO_INDEX_A		IpcKeyMgr::getIpcKey (-1,"TRUNKTOBENBR_INFO_INDEX_A")
#define TRUNKTOBENBR_INFO_DATA_B		IpcKeyMgr::getIpcKey (-1,"TRUNKTOBENBR_INFO_DATA_B")
#define TRUNKTOBENBR_INFO_INDEX_B		IpcKeyMgr::getIpcKey (-1,"TRUNKTOBENBR_INFO_INDEX_B")
#define CHECKDUPCFG_INFO_DATA_A					IpcKeyMgr::getIpcKey (-1,"CHECKDUPCFG_INFO_DATA_A")
#define CHECKDUPCFG_INFO_DATA_B					IpcKeyMgr::getIpcKey (-1,"CHECKDUPCFG_INFO_DATA_B")
#define CHECKDUPCFG_INFO_INDEX_A					IpcKeyMgr::getIpcKey (-1,"CHECKDUPCFG_INFO_INDEX_A")
#define CHECKDUPCFG_INFO_INDEX_B					IpcKeyMgr::getIpcKey (-1,"CHECKDUPCFG_INFO_INDEX_B")
#define CHECKDUPTYPE_INFO_DATA_A					IpcKeyMgr::getIpcKey (-1,"CHECKDUPTYPE_INFO_DATA_A")
#define CHECKDUPTYPE_INFO_DATA_B					IpcKeyMgr::getIpcKey (-1,"CHECKDUPTYPE_INFO_DATA_B")
#define CHECKDUPTYPE_INFO_INDEX_A					IpcKeyMgr::getIpcKey (-1,"CHECKDUPTYPE_INFO_INDEX_A")
#define CHECKDUPTYPE_INFO_INDEX_B					IpcKeyMgr::getIpcKey (-1,"CHECKDUPTYPE_INFO_INDEX_B")
#define CHECKDUPSTEP_INFO_DATA_A					IpcKeyMgr::getIpcKey (-1,"CHECKDUPSTEP_INFO_DATA_A")
#define CHECKDUPSTEP_INFO_DATA_B					IpcKeyMgr::getIpcKey (-1,"CHECKDUPSTEP_INFO_DATA_B")
#define CHECKDUPSTEP_INFO_INDEX_A					IpcKeyMgr::getIpcKey (-1,"CHECKDUPSTEP_INFO_INDEX_A")
#define CHECKDUPSTEP_INFO_INDEX_B					IpcKeyMgr::getIpcKey (-1,"CHECKDUPSTEP_INFO_INDEX_B")
#define CHECKSHOTTICKET_INFO_DATA_A					IpcKeyMgr::getIpcKey (-1,"CHECKSHOTTICKET_INFO_DATA_A")
#define CHECKSHOTTICKET_INFO_DATA_B					IpcKeyMgr::getIpcKey (-1,"CHECKSHOTTICKET_INFO_DATA_B")
#define CHECKSHOTTICKET_INFO_INDEX_A					IpcKeyMgr::getIpcKey (-1,"CHECKSHOTTICKET_INFO_INDEX_A")
#define CHECKSHOTTICKET_INFO_INDEX_B					IpcKeyMgr::getIpcKey (-1,"CHECKSHOTTICKET_INFO_INDEX_B")
#define UNDOPROCESSDUP_INFO_DATA_A					IpcKeyMgr::getIpcKey (-1,"UNDOPROCESSDUP_INFO_DATA_A")
#define UNDOPROCESSDUP_INFO_DATA_B					IpcKeyMgr::getIpcKey (-1,"UNDOPROCESSDUP_INFO_DATA_B")
#define UNDOPROCESSDUP_INFO_INDEX_A					IpcKeyMgr::getIpcKey (-1,"UNDOPROCESSDUP_INFO_INDEX_A")
#define UNDOPROCESSDUP_INFO_INDEX_B					IpcKeyMgr::getIpcKey (-1,"UNDOPROCESSDUP_INFO_INDEX_B")
#define UNDOPROCESSSHOT_INFO_DATA_A					IpcKeyMgr::getIpcKey (-1,"UNDOPROCESSSHOT_INFO_DATA_A")
#define UNDOPROCESSSHOT_INFO_DATA_B					IpcKeyMgr::getIpcKey (-1,"UNDOPROCESSSHOT_INFO_DATA_B")
#define UNDOPROCESSSHOT_INFO_INDEX_A					IpcKeyMgr::getIpcKey (-1,"UNDOPROCESSSHOT_INFO_INDEX_A")
#define UNDOPROCESSSHOT_INFO_INDEX_B					IpcKeyMgr::getIpcKey (-1,"UNDOPROCESSSHOT_INFO_INDEX_B")
#define UNDOSERVDUP_INFO_DATA_A					IpcKeyMgr::getIpcKey (-1,"UNDOSERVDUP_INFO_DATA_A")
#define UNDOSERVDUP_INFO_DATA_B					IpcKeyMgr::getIpcKey (-1,"UNDOSERVDUP_INFO_DATA_B")
#define UNDOPRODUCTDUP_INFO_DATA_A					IpcKeyMgr::getIpcKey (-1,"UNDOPRODUCTDUP_INFO_DATA_A")
#define UNDOPRODUCTDUP_INFO_DATA_B					IpcKeyMgr::getIpcKey (-1,"UNDOPRODUCTDUP_INFO_DATA_B")
#define PRODUCT_CONFIG_INFO_INDEX_A					IpcKeyMgr::getIpcKey (-1,"PRODUCT_CONFIG_INFO_INDEX_A")
#define PRODUCT_CONFIG_INFO_INDEX_B					IpcKeyMgr::getIpcKey (-1,"PRODUCT_CONFIG_INFO_INDEX_B")
#define PARAM_ORGPATH_INFO_DATA_A					IpcKeyMgr::getIpcKey (-1,"PARAM_ORGPATH_INFO_DATA_A")
#define PARAM_ORGPATH_INFO_DATA_B					IpcKeyMgr::getIpcKey (-1,"PARAM_ORGPATH_INFO_DATA_B")
#define PARAM_ORGPATH_INFO_INDEX_A					IpcKeyMgr::getIpcKey (-1,"PARAM_ORGPATH_INFO_INDEX_A")
#define PARAM_ORGPATH_INFO_INDEX_B					IpcKeyMgr::getIpcKey (-1,"PARAM_ORGPATH_INFO_INDEX_B")
#define PARAM_LOCALEVENTTYPERULE_INFO_DATA_A					IpcKeyMgr::getIpcKey (-1,"PARAM_LOCALEVENTTYPERULE_INFO_DATA_A")
#define PARAM_LOCALEVENTTYPERULE_INFO_DATA_B					IpcKeyMgr::getIpcKey (-1,"PARAM_LOCALEVENTTYPERULE_INFO_DATA_B")
#define PARAM_LOCALEVENTTYPERULE_INFO_INDEX_A					IpcKeyMgr::getIpcKey (-1,"PARAM_LOCALEVENTTYPERULE_INFO_INDEX_A")
#define PARAM_LOCALEVENTTYPERULE_INFO_INDEX_B					IpcKeyMgr::getIpcKey (-1,"PARAM_LOCALEVENTTYPERULE_INFO_INDEX_B")

//prep
#define TEMPLATEMAPSWITCH_DATA_A			IpcKeyMgr::getIpcKey (-1,"TEMPLATEMAPSWITCH_DATA_A")
#define TEMPLATEMAPSWITCH_DATA_B			IpcKeyMgr::getIpcKey (-1,"TEMPLATEMAPSWITCH_DATA_B")
#define TEMPLATEMAPSWITCH_INDEX_A			IpcKeyMgr::getIpcKey (-1,"TEMPLATEMAPSWITCH_INDEX_A")
#define TEMPLATEMAPSWITCH_INDEX_B			IpcKeyMgr::getIpcKey (-1,"TEMPLATEMAPSWITCH_INDEX_B")

#define OBJECTDEFINE_DATA_A					IpcKeyMgr::getIpcKey (-1,"OBJECTDEFINE_DATA_A")
#define OBJECTDEFINE_DATA_B					IpcKeyMgr::getIpcKey (-1,"OBJECTDEFINE_DATA_B")
#define OBJECTDEFINE_INDEX_A				IpcKeyMgr::getIpcKey (-1,"OBJECTDEFINE_INDEX_A")
#define OBJECTDEFINE_INDEX_B				IpcKeyMgr::getIpcKey (-1,"OBJECTDEFINE_INDEX_B")

#define EVENTPARSERGROUP_DATA_A				IpcKeyMgr::getIpcKey (-1,"EVENTPARSERGROUP_DATA_A")
#define EVENTPARSERGROUP_DATA_B				IpcKeyMgr::getIpcKey (-1,"EVENTPARSERGROUP_DATA_B")
#define EVENTPARSERGROUP_INDEX_A			IpcKeyMgr::getIpcKey (-1,"EVENTPARSERGROUP_INDEX_A")
#define EVENTPARSERGROUP_INDEX_B			IpcKeyMgr::getIpcKey (-1,"EVENTPARSERGROUP_INDEX_B")
//FieldPtr
#define TEMPLATE_RELATION_DATA_A			IpcKeyMgr::getIpcKey (-1,"TEMPLATE_RELATION_DATA_A")
#define TEMPLATE_RELATION_DATA_B			IpcKeyMgr::getIpcKey (-1,"TEMPLATE_RELATION_DATA_B")
#define TEMPLATE_RELATION_INDEX_A			IpcKeyMgr::getIpcKey (-1,"TEMPLATE_RELATION_INDEX_A")
#define TEMPLATE_RELATION_INDEX_B			IpcKeyMgr::getIpcKey (-1,"TEMPLATE_RELATION_INDEX_B")
#define FIELD_SHM_DATA_A			IpcKeyMgr::getIpcKey (-1,"FIELD_SHM_DATA_A")
#define FIELD_SHM_DATA_B			IpcKeyMgr::getIpcKey (-1,"FIELD_SHM_DATA_B")
#define FIELD_TEMPLATE_INDEX_A			IpcKeyMgr::getIpcKey (-1,"FIELD_TEMPLATE_INDEX_A")
#define FIELD_TEMPLATE_INDEX_B			IpcKeyMgr::getIpcKey (-1,"FIELD_TEMPLATE_INDEX_B")
#define FIELD_UNION_INDEX_A			IpcKeyMgr::getIpcKey (-1,"FIELD_UNION_INDEX_A")
#define FIELD_UNION_INDEX_B			IpcKeyMgr::getIpcKey (-1,"FIELD_UNION_INDEX_B")
//mapid
#define SWITCH_TYPE_INFO_DATA_A			IpcKeyMgr::getIpcKey (-1,"SWITCH_TYPE_INFO_DATA_A")
#define SWITCH_TYPE_INFO_DATA_B			IpcKeyMgr::getIpcKey (-1,"SWITCH_TYPE_INFO_DATA_B")
#define SWITCH_TYPE_INFO_INDEX_A		IpcKeyMgr::getIpcKey (-1,"SWITCH_TYPE_INFO_INDEX_A")
#define SWITCH_TYPE_INFO_INDEX_B		IpcKeyMgr::getIpcKey (-1,"SWITCH_TYPE_INFO_INDEX_B")

//EVENT_ATTR
#define EVENT_ATTR_DATA_A				IpcKeyMgr::getIpcKey (-1,"EVENT_ATTR_DATA_A")
#define EVENT_ATTR_DATA_B				IpcKeyMgr::getIpcKey (-1,"EVENT_ATTR_DATA_B")
#define EVENT_ATTR_INDEX_A			IpcKeyMgr::getIpcKey (-1,"EVENT_ATTR_INDEX_A")
#define EVENT_ATTR_INDEX_B			IpcKeyMgr::getIpcKey (-1,"EVENT_ATTR_INDEX_B")


//
//b_map_parser_group loadEventHeadAndColunmCfg
#define EVENT_HEAD_COLUMNS_CFG_A				IpcKeyMgr::getIpcKey (-1,"EVENT_HEAD_COLUMNS_CFG_A")
#define EVENT_HEAD_COLUMNS_CFG_B				IpcKeyMgr::getIpcKey (-1,"EVENT_HEAD_COLUMNS_CFG_B")
#define EVENT_HEAD_COLUMNS_INDEX_A			IpcKeyMgr::getIpcKey (-1,"EVENT_HEAD_COLUMNS_INDEX_A")
#define EVENT_HEAD_COLUMNS_INDEX_B			IpcKeyMgr::getIpcKey (-1,"EVENT_HEAD_COLUMNS_INDEX_B")

// 
#define SWITCHID_MAP_SWITCH_DATA_A				IpcKeyMgr::getIpcKey (-1,"SWITCHID_MAP_SWITCH_DATA_A")
#define SWITCHID_MAP_SWITCH_DATA_B				IpcKeyMgr::getIpcKey (-1,"SWITCHID_MAP_SWITCH_DATA_B")
#define SWITCHID_MAP_SWITCH_INDEX_A			IpcKeyMgr::getIpcKey (-1,"SWITCHID_MAP_SWITCH_INDEX_A")
#define SWITCHID_MAP_SWITCH_INDEX_B			IpcKeyMgr::getIpcKey (-1,"SWITCHID_MAP_SWITCH_INDEX_B")


//FieldCheckMethod
#define FIELD_CHECK_METHOD_DATA_A				IpcKeyMgr::getIpcKey (-1,"FIELD_CHECK_METHOD_DATA_A")
#define FIELD_CHECK_METHOD_DATA_B				IpcKeyMgr::getIpcKey (-1,"FIELD_CHECK_METHOD_DATA_B")
#define FIELD_CHECK_METHOD_INDEX_A			IpcKeyMgr::getIpcKey (-1,"FIELD_CHECK_METHOD_INDEX_A")
#define FIELD_CHECK_METHOD_INDEX_B			IpcKeyMgr::getIpcKey (-1,"FIELD_CHECK_METHOD_INDEX_B")
//FieldMAP
#define FIELD_VALUE_MAP_DATA_A			IpcKeyMgr::getIpcKey (-1,"FIELD_VALUE_MAP_DATA_A")
#define FIELD_VALUE_MAP_DATA_B			IpcKeyMgr::getIpcKey (-1,"FIELD_VALUE_MAP_DATA_B")
#define FIELD_VALUE_MAP_INDEX_A			IpcKeyMgr::getIpcKey (-1,"FIELD_VALUE_MAP_INDEX_A")
#define FIELD_VALUE_MAP_INDEX_B			IpcKeyMgr::getIpcKey (-1,"FIELD_VALUE_MAP_INDEX_B")

//FieldMERGE
#define FIELD_MERGE_DATA_A			IpcKeyMgr::getIpcKey (-1,"FIELD_MERGE_DATA_A")
#define FIELD_MERGE_INDEX_A			IpcKeyMgr::getIpcKey (-1,"FIELD_MERGE_INDEX_A")
#define FIELD_MERGE_DATA_B			IpcKeyMgr::getIpcKey (-1,"FIELD_MERGE_DATA_B")
#define FIELD_MERGE_INDEX_B			IpcKeyMgr::getIpcKey (-1,"FIELD_MERGE_INDEX_B")


//TAP3
#define TAP3FILE_DATA_A				IpcKeyMgr::getIpcKey (-1,"TAP3FILE_DATA_A")
#define TAP3FILE_DATA_B				IpcKeyMgr::getIpcKey (-1,"TAP3FILE_DATA_B")
#define TAP3_BLOCK_DATA_A			IpcKeyMgr::getIpcKey (-1,"TAP3_BLOCK_DATA_A")
#define TAP3_BLOCK_DATA_B			IpcKeyMgr::getIpcKey (-1,"TAP3_BLOCK_DATA_B")
//#define TAP3_BLOCK_INDEX_A			IpcKeyMgr::getIpcKey (-1,"TAP3_BLOCK_INDEX_A")
//#define TAP3_BLOCK_INDEX_B			IpcKeyMgr::getIpcKey (-1,"TAP3_BLOCK_INDEX_B")

//SEP
#define SEP_FILE_DATA_A				IpcKeyMgr::getIpcKey (-1,"SEP_FILE_DATA_A")
#define SEP_FILE_DATA_B				IpcKeyMgr::getIpcKey (-1,"SEP_FILE_DATA_B")
#define SEP_BLOCK_DATA_A			IpcKeyMgr::getIpcKey (-1,"SEP_BLOCK_DATA_A")
#define SEP_BLOCK_DATA_B			IpcKeyMgr::getIpcKey (-1,"SEP_BLOCK_DATA_B")
//#define SEP_BLOCK_INDEX_A			IpcKeyMgr::getIpcKey (-1,"SEP_BLOCK_INDEX_A")
//#define SEP_BLOCK_INDEX_B			IpcKeyMgr::getIpcKey (-1,"SEP_BLOCK_INDEX_B")

//MERGE
#define MERGE_RULE_DATA_A				IpcKeyMgr::getIpcKey (-1,"MERGE_RULE_DATA_A")
#define MERGE_RULE_DATA_B				IpcKeyMgr::getIpcKey (-1,"MERGE_RULE_DATA_B")
#define MERGE_RULE_INDEX_A			IpcKeyMgr::getIpcKey (-1,"MERGE_RULE_INDEX_A")
#define MERGE_RULE_INDEX_B			IpcKeyMgr::getIpcKey (-1,"MERGE_RULE_INDEX_B")
#define MERGE_ASSCOCIATE_RULE_DATA_A				IpcKeyMgr::getIpcKey (-1,"MERGE_ASSCOCIATE_RULE_DATA_A")
#define MERGE_ASSCOCIATE_RULE_DATA_B				IpcKeyMgr::getIpcKey (-1,"MERGE_ASSCOCIATE_RULE_DATA_B")
#define MERGE_ASSCOCIATE_RULE_INDEX_A				IpcKeyMgr::getIpcKey (-1,"MERGE_ASSCOCIATE_RULE_INDEX_A")
#define MERGE_ASSCOCIATE_RULE_INDEX_B				IpcKeyMgr::getIpcKey (-1,"MERGE_ASSCOCIATE_RULE_INDEX_B")

#define MERGE_OUT_DATA_A				IpcKeyMgr::getIpcKey (-1,"MERGE_OUT_DATA_A")
#define MERGE_OUT_DATA_B				IpcKeyMgr::getIpcKey (-1,"MERGE_OUT_DATA_B")
#define MERGE_OUT_INDEX_A			IpcKeyMgr::getIpcKey (-1,"MERGE_OUT_INDEX_A")
#define MERGE_OUT_INDEX_B			IpcKeyMgr::getIpcKey (-1,"MERGE_OUT_INDEX_B")

#define MERGE_KEY_DATA_A				IpcKeyMgr::getIpcKey (-1,"MERGE_KEY_DATA_A")
#define MERGE_KEY_DATA_B				IpcKeyMgr::getIpcKey (-1,"MERGE_KEY_DATA_B")
#define MERGE_KEY_INDEX_A			IpcKeyMgr::getIpcKey (-1,"MERGE_KEY_INDEX_A")
#define MERGE_KEY_INDEX_B			IpcKeyMgr::getIpcKey (-1,"MERGE_KEY_INDEX_B")
#define LOADPARAMINFO                      "LOADPARAMINFO"
#define PARAM_LOGICSTATEMENT_NUM           "PARAM_LOGICSTATEMENT_NUM"
#define PARAM_ASSIGNSTATEMENT_NUM          "PARAM_ASSIGNSTATEMENT_NUM"
#define PARAM_FORMULA_NUM                  "PARAM_FORMULA_NUM"
#define PARAM_FORMATGROUP_NUM              "PARAM_FORMATGROUP_NUM"
#define PARAM_FORMATSTEP_NUM               "PARAM_FORMATSTEP_NUM"
#define PARAM_FORMATRULE_NUM               "PARAM_FORMATRULE_NUM"
#define PARAM_FORMATCOMB_NUM               "PARAM_FORMATCOMB_NUM"
#define PARAM_TRUNK_NUM                    "PARAM_TRUNK_NUM"
#define PARAM_AREACODE_NUM                 "PARAM_AREACODE_NUM"
#define PARAM_HLR_NUM                      "PARAM_HLR_NUM"
#define PARAM_CARRIER_NUM                  "PARAM_CARRIER_NUM"
#define PARAM_SERVICE_NUM                  "PARAM_SERVICE_NUM"
#define PARAM_SWITCH_NUM                   "PARAM_SWITCH_NUM"
#define PARAM_TRUNKBILLING_NUM             "PARAM_TRUNKBILLING_NUM"
#define PARAM_TIMESTYPE_NUM                "PARAM_TIMESTYPE_NUM"
#define PARAM_CITYINFO_NUM                 "PARAM_CITYINFO_NUM"
#define PARAM_ORG_NUM                      "PARAM_ORG_NUM"
#define PARAM_ORGEX_NUM                    "PARAM_ORGEX_NUM"
#define PARAM_ORGLONGTYPE_NUM              "PARAM_ORGLONGTYPE_NUM"
#define PARAM_HEADEX_NUM                   "PARAM_HEADEX_NUM"
#define PARAM_LOCALHEAD_NUM                "PARAM_LOCALHEAD_NUM"
#define PARAM_EVENTTYPE_NUM                "PARAM_EVENTTYPE_NUM"
#define PARAM_EVENTTYPEGROUP_NUM           "PARAM_EVENTTYPEGROUP_NUM"
#define PARAM_OFFER_NUM                    "PARAM_OFFER_NUM"
#define PARAM_OFFER_SPECATTR_NUM           "PARAM_OFFER_SPECATTR_NUM"
#define PARAM_OFFER_NUMATTR_NUM            "PARAM_OFFER_NUMATTR_NUM"
#define PARAM_OFFER_FREE_NUM               "PARAM_OFFER_FREE_NUM"
#define PARAM_NBR_MAP_NUM                  "PARAM_NBR_MAP_NUM"

#define PARAM_ACCTITEM_NUM                 "PARAM_ACCTITEM_NUM"
#define PARAM_ACCTITEMMEMBERS_NUM          "PARAM_ACCTITEMMEMBERS_NUM"
#define PARAM_ACCTITEMGROUPS_NUM           "PARAM_ACCTITEMGROUPS_NUM"
#define PARAM_ACCTITEMMEMBERSB_NUM         "PARAM_ACCTITEMMEMBERSB_NUM"
#define PARAM_ACCTITEMGROUPSB_NUM          "PARAM_ACCTITEMGROUPSB_NUM"

#define PARAM_LONGEVENTTYPERULE_NUM        "PARAM_LONGEVENTTYPERULE_NUM"
#define PARAM_LONGEVENTTYPERULEEX_NUM      "PARAM_LONGEVENTTYPERULEEX_NUM"

#define PARAM_PRICINGPLANTYPE_NUM          "PARAM_PRICINGPLANTYPE_NUM"
#define PARAM_STATECONVERT_NUM             "PARAM_STATECONVERT_NUM"
#define PARAM_VALUEMAP_NUM                 "PARAM_VALUEMAP_NUM"
#define PARAM_ZONE_ITEM_NUM                "PARAM_ZONE_ITEM_NUM"
#define PARAM_SPLITACCTGROUP_NUM           "PARAM_SPLITACCTGROUP_NUM"
#define PARAM_SPLITACCTEVENT_NUM           "PARAM_SPLITACCTEVENT_NUM"
#define PARAM_DISTANCETYPE_NUM             "PARAM_DISTANCETYPE_NUM"
#define PARAM_EDGEROAMRULEEX_NUM           "PARAM_EDGEROAMRULEEX_NUM"

#define PARAM_SECTIONINFO_NUM               "PARAM_SECTIONINFO_NUM"
#define PARAM_TARIFFINFO_NUM 		   	    "PARAM_TARIFFINFO_NUM"
#define PARAM_DISCOUNTINFO_NUM 		   		"PARAM_DISCOUNTINFO_NUM"
#define PARAM_DISCTAPPLYERINFO_NUM 		   	"PARAM_DISCTAPPLYERINFO_NUM"
#define PARAM_DISCTAPPLYEROBJINFO_NUM 		"PARAM_DISCTAPPLYEROBJINFO_NUM"
#define PARAM_PERCENTINFO_NUM 		  		"PARAM_PERCENTINFO_NUM"
#define PARAM_LIFECYCLEINFO_NUM 		  	"PARAM_LIFECYCLEINFO_NUM"
#define PARAM_MEASUREINFO_NUM 		  		"PARAM_MEASUREINFO_NUM"
#define PARAM_COMBINEINFO_NUM 		  		"PARAM_COMBINEINFO_NUM"
#define PARAM_FILTERINFO_NUM 		  		"PARAM_FILTERINFO_NUM"
#define PARAM_CONDITIONPOINTINFO_NUM 		"PARAM_CONDITIONPOINTINFO_NUM"
#define PARAM_TIMECONDINFO_NUM 		  	    "PARAM_TIMECONDINFO_NUM"  
#define PARAM_EVENTTYPECONDINFO_NUM 		"PARAM_EVENTTYPECONDINFO_NUM"  
#define PARAM_CYCLETYPEINFO_NUM 		  	"PARAM_CYCLETYPEINFO_NUM" 
#define PARAM_ACCTTYPEINFO_NUM 		  	    "PARAM_ACCTTYPEINFO_NUM" 
#define PARAM_OFFERACCYRELATIONINFO_NUM 	"PARAM_OFFERACCYRELATIONINFO_NUM" 
#define PARAM_OFFERACCTEXISTINFO_NUM 		"PARAM_OFFERACCTEXISTINFO_NUM" 
#define PARAM_CYCLETYPEIDINFO_NUM 		  	"PARAM_CYCLETYPEIDINFO_NUM"  
#define PARAM_OFFERCOMBINEINFO_NUM 			"PARAM_OFFERCOMBINEINFO_NUM" 
#define PARAM_OFFERDISCTINFO_NUM 			"PARAM_OFFERDISCTINFO_NUM" 
#define PARAM_PRODUCTCOMBINEINFO_NUM 		"PARAM_PRODUCTCOMBINEINFO_NUM"  

#define PARAM_USAGETYPEINFO_NUM 		  			"PARAM_USAGETYPEINFO_NUM"
#define PARAM_SPLITCFGINFO_NUM 		  				"PARAM_SPLITCFGINFO_NUM"
#define PARAM_WHOLEDISCOUNTINFO_NUM 				"PARAM_WHOLEDISCOUNTINFO_NUM"
#define PARAM_CGCOMBINEINFO_NUM                     "PARAM_CGCOMBINEINFO_NUM"		 
#define PARAM_VIRTUALCOMBINEINFO_NUM 		  	    "PARAM_VIRTUALCOMBINEINFO_NUM"   
#define PARAM_COMBINERELATIONINFO_NUM 		        "PARAM_COMBINERELATIONINFO_NUM"
#define PARAM_GATHERDATAINFO_NUM 		        	"PARAM_GATHERDATAINFO_NUM"
#define PARAM_SPECIALNBRSTRATEGYINFO_NUM 		    "PARAM_SPECIALNBRSTRATEGYINFO_NUM" 
#define PARAM_HOSTINFOINFO_NUM						"PARAM_HOSTINFOINFO_NUM"
//重了#define PARAM_GATHERDATAINFO_NUM					"PARAM_GATHERDATAINFO_NUM"
#define PARAM_PRODUCTACCTITEMINFO_NUM				"PARAM_PRODUCTACCTITEMINFO_NUM"
#define PARAM_PROID_PRODUCTACCTITEMINFO_NUM			"PARAM_PROID_PRODUCTACCTITEMINFO_NUM"
#define PARAM_SPECIALTRIFFINFO_NUM					"PARAM_SPECIALTRIFFINFO_NUM"
#define PARAM_SPECIALTRIFFCHECKINFO_NUM				"PARAM_SPECIALTRIFFCHECKINFO_NUM"
#define PARAM_SPINFOSEEKINFO_NUM					"PARAM_SPINFOSEEKINFO_NUM"
#define PARAM_GATERTASKLISTINFO_NUM					"PARAM_GATERTASKLISTINFO_NUM"
#define PARAM_SEGMENTINFO_INFO_NUM					"PARAM_SEGMENTINFO_INFO_NUM"
#define PARAM_LONGTYPE_INFO_NUM						"PARAM_LONGTYPE_INFO_NUM"
#define PARAM_CHECKRULE_RULE_INFO_NUM				"PARAM_CHECKRULE_RULE_INFO_NUM"
#define PARAM_CHECKRULE_SUBRULE_INFO_NUM			"PARAM_CHECKRULE_SUBRULE_INFO_NUM"
#define PARAM_CHECKARGUMENT_INFO_NUM				"PARAM_CHECKARGUMENT_INFO_NUM"
#define PARAM_CHECKRELATION_INFO_NUM				"PARAM_CHECKRELATION_INFO_NUM"
#define PARAM_TRANSFILE_INFO_NUM					"PARAM_TRANSFILE_INFO_NUM"
#define PARAM_SWITCHINFO_NUM                                    "PARAM_SWITCH_INFO_NUM"
#define PARAM_PARSERULE_INFO_NUM					"PARAM_PARSERULE_INFO_NUM"
#define PARAM_PARSECOMB_INFO_NUM					"PARAM_PARSECOMB_INFO_NUM"
#define PARAM_MSERVACCT_INFO_NUM					"PARAM_MSERVACCT_INFO_NUM"

//王帅 20101229 取消所有非参数表的操作

//#define IVPN_SERVACCT_NUM               "IVPN_SERVACCT_NUM"
//#define IVPN_ACCTATTR_NUM               "IVPN_ACCTATTR_NUM"
//#define VPNINFO_NUM_VPNINFO             "VPNINFO"
//#define VPNINFO_NUM_VPNMEMBER           "VPNMEMBER"
//#define VPNINFO_NUM_VPNMEMBER999        "VPNMEMBER999"
//#define VPNINFO_NUM_VPNANDVPN           "VPNANDVPN"
//#define VPNINFO_NUM_VPNFRIENDNUM        "VPNFRIENDNUM"
//#define E8_USER_INFO_NUM                "E8_USER"
//#define EXT_SERV_DATA_NUM               "EXT_SERV_DATA_NUM"
//#define IVPN_SEGMENT                    "IVPN_UPLOAD"
//#define VPNINFO_SEGMENT                 "VPNINFO_UPLOAD"
//#define USERINFO_SEGMENT                "USERINFO_UPLOAD"
//#define USERINFO_NUM_SERVPRODUCT                "SERVPRODUCT"
//#define USERINFO_NUM_SERVPRODUCTATTR            "SERVPRODUCTATTR"
//#define USERINFO_NUM_ASERVTYPECHANGE            "ASERVTYPECHANGE"
//#define USERINFO_NUM_SERVLOCATION               "SERVLOCATION"
//#define USERINFO_NUM_PRODOFFERINSATTR           "PRODOFFERINSATTR"
//#define USERINFO_NUM_PRODOFFERINS               "PRODOFFERINS"
//#define USERINFO_NUM_SERV                       "SERV"
//#define USERINFO_NUM_SERVSTATEATTR              "SERVSTATEATTR"
//#define USERINFO_NUM_SERVIDENT                  "SERVIDENT"
//#define USERINFO_NUM_SERVATTR                   "SERVATTR"
//#define USERINFO_NUM_CUST                       "CUST"
//#define USERINFO_NUM_SERVACCT                   "SERVACCT"
//#define USERINFO_NUM_ACCT                       "ACCT"
//#define EXT_SERV_INDEX_NUM                      "EXT_SERV_INDEX_NUM"
//#define SERV_IDEN_CENTER                        "SERV_IDEN_CENTER"
//#define	USERINFO_NUM_ACC_NBR_OFFER_TYPE	         "ACCNBROFFERTYPE"
//#define USERINFO_NUM_GROUPINFO                   "GROUPINFO"
//#define USERINFO_NUM_GROUPMEMBER                 "GROUPMEMBER"
//#define USERINFO_NUM_NUMBERGROUP                 "NUMBERGROUP"
//#define USERINFO_NUM_IMSI_MDN                    "IMSI_MDN_RELATION_NUM"

#define PARAM_INSTTABLELIST_INFO_NUM				"PARAM_INSTTABLELIST_INFO_NUM"
#define PARAM_TABLETYPE_INFO_NUM					"PARAM_TABLETYPE_INFO_NUM"
#define PARAM_RENT_CONFIG_INFO_NUM					"PARAM_RENT_CONFIG_INFO_NUM"
#define PARAM_NETWORKPRIORITY_INFO_NUM				"PARAM_NETWORKPRIORITY_INFO_NUM"
#define PARAM_TRUNKTOBENBR_NUM  		                "PARAM_TRUNKTOBENBR_NUM"

#define PARAM_CHECKDUPCFG_INFO_NUM					"PARAM_CHECKDUPCFG_INFO_NUM"
#define PARAM_CHECKDUPTYPE_INFO_NUM					"PARAM_CHECKDUPTYPE_INFO_NUM"
#define PARAM_CHECKDUPSTEP_INFO_NUM		  			"PARAM_CHECKDUPSTEP_INFO_NUM"
#define PARAM_CHECKSHOTTICKET_INFO_NUM	  			"PARAM_CHECKSHOTTICKET_INFO_NUM"
#define PARAM_UNDOPROCESSDUP_INFO_NUM	  			"PARAM_UNDOPROCESSDUP_INFO_NUM"
#define PARAM_UNDOPROCESSSHOT_INFO_NUM	  			"PARAM_UNDOPROCESSSHOT_INFO_NUM"
#define PARAM_UNDOSERVDUP_INFO_NUM		  			"PARAM_UNDOSERVDUP_INFO_NUM"
#define PARAM_UNDOPRODUCTDUP_INFO_NUM	  			"PARAM_UNDOPRODUCTDUP_INFO_NUM"
#define PARAM_PRODUCT_CONFIG_INFO_NUM	  			"PARAM_PRODUCT_CONFIG_INFO_NUM"
#define PARAM_ORGPATH_INFO_NUM	  					"PARAM_ORGPATH_INFO_NUM"
#define PARAM_LOCALEVENTTYPERULE_INFO_NUM	  		"PARAM_LOCALEVENTTYPERULE_INFO_NUM"
//2010.11.17
#define PARAM_FILEPOOLINFO_NUM              "PARAM_FILEPOOLINFO_NUM"
#define PARAM_FILEPOOLCOND_NUM              "PARAM_FILEPOOLCOND_NUM"
#define PARAM_FILETYPECFG_NUM               "PARAM_FILETYPECFG_NUM"
#define PARAM_TLVENCODE_NUM                 "PARAM_TLVENCODE_NUM"
#define PARAM_TLVDECODE_NUM                 "PARAM_TLVDECODE_NUM"
#define PARAM_BILLINGNODE_NUM               "PARAM_BILLINGNODE_NUM"
#define PARAM_FILEPARSERULE_NUM             "PARAM_FILEPARSERULE_NUM"
#define PARAM_SWITCHCODE_NUM                "PARAM_SWITCHCODE_NUM"
#define PARAM_ROAMTARIFF_NUM                "PARAM_ROAMTARIFF_NUM"
#define PARAM_ROAMCARRIER_NUM               "PARAM_ROAMCARRIER_NUM"
#define PARAM_TARIFFSECTOR_NUM              "PARAM_TARIFFSECTOR_NUM"
#define PARAM_COUNTRY_NUM                   "PARAM_COUNTRY_NUM"
#define PARAM_ROAMSPONSOR_NUM               "PARAM_ROAMSPONSOR_NUM"
#define PARAM_SPECIALGROUPMEMBER_NUM        "PARAM_SPECIALGROUPMEMBER_NUM"

//prep
#define PARAM_TEMPLATEMAPSWITCH_NUM                 "PARAM_TEMPLATEMAPSWITCH_NUM"
#define PARAM_OBJECTDEFINE_NUM                 		"PARAM_OBJECTDEFINE_NUM"
#define PARAM_EVENTPARSERGROUP_NUM                 	"PARAM_EVENTPARSERGROUP_NUM"
#define PARAM_FIELD_SHM_NUM                 		"PARAM_FIELD_SHM_NUM"
#define PARAM_TEMPLATE_RELATION_SHM_NUM              "PARAM_TEMPLATE_RELATION_SHM_NUM"
#define PARAM_SWITCH_TYPE_INFO_NUM                 	 "PARAM_SWITCH_TYPE_INFO_NUM"
#define PARAM_SEP_INFO_NUM                 			"PARAM_SEP_INFO_NUM"

#define PARAM_EVENT_ATTR_INFO_NUM                 	"PARAM_EVENT_ATTR_INFO_NUM"
//#define PARAM_FILE_LIST_INFO_NUM                 	"PARAM_FILE_LIST_INFO_NUM"
#define PARAM_EVENTHEAD_COLUMUN_INFO_NUM             "PARAM_EVENTHEAD_COLUMUN_INFO_NUM"
#define PARAM_SWITICHIDMAPSWITCHINFO_INFO_NUM        "PARAM_SWITICHIDMAPSWITCHINFO_INFO_NUM"
#define PARAM_FIELD_CHECK_INFO_NUM              "PARAM_FIELD_CHECK_INFO_NUM"
#define PARAM_FIELD_MAP_INFO_NUM                "PARAM_FIELD_MAP_INFO_NUM"
#define PARAM_FIELD_MERGE_INFO_NUM              "PARAM_FIELD_MERGE_INFO_NUM"

#define PARAM_TAP3_FILE_INFO_NUM              	"PARAM_TAP3_FILE_INFO_NUM"
#define PARAM_TAP3_BLOCK_INFO_NUM             	"PARAM_TAP3_BLOCK_INFO_NUM"

#define PARAM_SEP_FILE_INFO_NUM					"PARAM_SEP_FILE_INFO_NUM"
#define PARAM_SEP_BLOCK_INFO_NUM				"PARAM_SEP_BLOCK_INFO_NUM"
//合并功能
#define PARAM_MERGE_RULE_INFO_NUM               "PARAM_MERGE_RULE_INFO_NUM"
#define PARAM_MERGE_RULE_OUT_INFO_NUM           "PARAM_MERGE_RULE_OUT_INFO_NUM"
#define PARAM_MERGE_KEY_RULE_INFO_NUM           "PARAM_MERGE_KEY_RULE_INFO_NUM"
//
#define PARAM_TRANS_PEER_INFO_NUM               "PARAM_TRANS_PEER_INFO_NUM"
#define PARAM_TRANS_TYPE_DEFINE_INFO_NUM        "PARAM_TRANS_TYPE_DEFINE_INFO_NUM"
#define PARAM_TRANS_CONTPOINT_TYPEDEFINE_INFO_NUM        "PARAM_TRANS_CONTPOINT_TYPEDEFINE_INFO_NUM"

//规整补充
#define PARAM_VISIT_AREAVODE_INFO_NUM        "PARAM_VISIT_AREAVODE_INFO_NUM"
#define PARAM_EDGEROAMRULE_INFO_NUM        	 "PARAM_EDGEROAMRULE_INFO_NUM"
#define PARAM_PNSEGBILLING_INFO_NUM			 "PARAM_PNSEGBILLING_INFO_NUM"

#define PARAM_PNNOTBILLING_INFO_NUM			 "PARAM_PNNOTBILLING_INFO_NUM"
#define PARAM_TRUNKTREENODE_INFO_NUM		 "PARAM_TRUNKTREENODE_INFO_NUM"
#define PARAM_SERVICE_INFO2_NUM				 "PARAM_SERVICE_INFO2_NUM"
#define PARAM_SPEC_TRUNK_PRODUCT_NUM		 "PARAM_SPEC_TRUNK_PRODUCT_NUM"
#define PARAM_STR_LOCALAREACODE_NUM			 "PARAM_STR_LOCALAREACODE_NUM"
#define PARAM_SWITCHITEMTYPE_NUM			 "PARAM_SWITCHITEMTYPE_NUM"
#define PARAM_FILTER_RESULT_NUM				 "PARAM_FILTER_RESULT_NUM"
class LifeCycle;
//class SpecialNbrStrategy;
#ifdef GROUP_CONTROL
	struct WholeDiscount;
#endif

class ShmParamInfo
{
#ifndef PRIVATE_MEMORY
public:
    ShmParamInfo();
    ~ShmParamInfo();
void dprintf(const char *sfmt, ...);

    void bindKey(int iShmIdx);

    void bindData();
	bool SHMDetachShm(char * p);
    //连接所有指针对应的内存
    void attachAll();
	//释放对共享内存的连接
	bool detachAll();
    //释放所有指针对应的内存
    void freeAll();

    operator bool();

    //初始化共享内存的大小配置(初始化数据库中的配置信息)
    void initSizeCfg(const char * sSegment, const char * sCode, const char * sSqlcode);
    void initSizeCfg(const char* sSegment, const char* sCode, const char* sName, const char* sSqlcode);
    void initAllSizeCfg();

    void initAllIpcKey();

    void resetAllSizeCfg();
public:	//std	
	unsigned int getStdTotalNum();

    //条件参数
    SHMData<LogicStatement> *m_poLogicStatementData;
    SHMIntHashIndex         *m_poLogicStatementIndex;
    LogicStatement          *m_poLogicStatementList;
    //运算参数
    SHMData<AssignStatement> *m_poAssignStatementData;
    SHMIntHashIndex          *m_poAssignStatementIndex;
    AssignStatement          *m_poAssignStatementList;
    //公式参数
    SHMData<Formula>         *m_poFormulaData;
    SHMIntHashIndex          *m_poFormulaIndex;
    Formula                  *m_poFormulaList;

    //属性规整规整组等
    SHMData<FormatGroup>      *m_poFormatGroupData;
    FormatGroup               *m_poFormatGroupList;
    SHMData<FormatStep>       *m_poFormatStepData;
    FormatStep                *m_poFormatStepList;
    SHMData<FormatRule>       *m_poFormatRuleData;
    FormatRule                *m_poFormatRuleList;
    SHMData<FormatComb>       *m_poFormatCombData;
    FormatComb                *m_poFormatCombList;

    //B_TRUNK
    SHMData<Trunk>          *m_poTrunkData;
    SHMStringTreeIndex      *m_poTrunkIndex;
    Trunk                   *m_poTrunkList;
    //B_AREA_CODE
    SHMStringTreeIndex      *m_poAreaCodeIndex;
    //B_HLR
    SHMData<HLR>            *m_poHLRData;
    SHMStringTreeIndex      *m_poHLRIndex;
    HLR                     *m_poHLRList;
	
	//补充规整数据
	SHMData<VisitAreaCodeRule> *m_poVisitAreaCodeRuleData;
	SHMStringTreeIndex *m_pVisitAreaCodeIndex;
	VisitAreaCodeRule *m_poVisitAreaCodeRuleList;
	
	SHMData<EdgeRoamRule> *m_poEdgeRoamRuleData;
	SHMStringTreeIndex *m_pEdgeRoamIndex;
	EdgeRoamRule *m_poEdgeRoamRuleList;
	
	//
	SHMData<PNSegBilling> 	*m_aPNSegBillingData; 
	PNSegBilling *m_aPNSegBilling;
	
	//PNSegBilling
	SHMData<PNSegBilling> 	*m_poPNSegBillingData; 
	//<0,NULL_NULL> <1,EXIST_NULL> <2,EXIST,EXIST> <3,NULL,EXIST>
	SHMStringTreeIndex *m_pPNSegBillingIndex;
	PNSegBilling *m_poPNSegBillingList;
	
	//
	SHMData<TrunkTreeNode> *m_poTrunkTreeNodeData;
	SHMStringTreeIndex *m_pSwtichTrunkTree;
	TrunkTreeNode *m_poTrunkTreeNodeList;
	
	//
	SHMData<strPNNotBilling> *m_poPNNotBillingData;
	SHMStringTreeIndex *m_poPNNotBillingIndex; 
	strPNNotBilling *m_poPNNotBillingList;
	
	//TServiceInfo2
    SHMData<TServiceInfo2>  *m_poServiceInfo2Data;
	SHMStringTreeIndex      *m_poServiceInfo2Index;
    TServiceInfo2           *m_poServiceInfo2List;
	
	//SpecTrunkProductIndex
	//SHMData<SpecTrunkProduct> *m_poSpecTrunkProductData;
	SHMIntHashIndex           *m_poSpecTrunkProductIndex;
	
	//SwitchItemType
	SHMData<SwitchItemType>  *m_poSwitchItemTypeData;
    SwitchItemType           *m_poSwitchItemTypeList;
	SHMStringTreeIndex  	 *m_poSwitchItemTypeIndex;
	//SHMMixHashIndex			 *m_poSwitchItemTypeIndex;
	
	//
	SHMData<strLocalAreaCode>  *m_poStrLocalAreaCodeData;
    strLocalAreaCode           *m_poStrLocalAreaCodeList;
	
    //B_CARRIER
    SHMStringTreeIndex      *m_poCarrierIndex;
    //B_SP_SERVICE
    //SHMData<TServiceInfo2>  *m_poServiceData;
    //SHMStringTreeIndex      *m_poServiceIndex;
    //TServiceInfo2           *m_poServiceList;
    //B_SWITCH_INFO
    SHMData<TSwitchInfo>    *m_poSwitchData;
    SHMIntHashIndex         *m_poSwitchIndex;
    SHMStringTreeIndex      *m_poSwitchOPCIndex;
    TSwitchInfo             *m_poSwitchList;
    //B_TRUNK_BILLING
    SHMData<TrunkBilling>   *m_poTrunkBillingData;
    SHMStringTreeIndex      *m_poTrunkBillingIndex;
    TrunkBilling            *m_poTrunkBillingList;
    //B_TIMES_TYPE
    SHMIntHashIndex         *m_poTimesTypeIndex;
	//FilterRuleResult
	SHMData<FilterRuleResult>   *m_poFilterRuleResultData;
	FilterRuleResult 			*m_poFilterRuleResultList;
    //SHMStringTreeIndex      	*m_poFilterRuleResultIndex;
	//其它使用这个FilterRuleResult数据区的索引
	SHMStringTreeIndex      	*m_poRuleCheckCalledIndex;
	SHMStringTreeIndex      	*m_poRuleCheckCallingIndex;
	SHMStringTreeIndex      	*m_poRuleCheckAreaCodeIndex;
	SHMStringTreeIndex      	*m_poRuleCheckGatherPotIndex;
	SHMStringTreeIndex      	*m_poRuleCheckTrunkOutIndex;
	SHMStringTreeIndex      	*m_poRuleCheckTrunkInIndex;
	SHMStringTreeIndex      	*m_poRuleCheckSwitchIDIndex;
	
    //B_CITY_INFO
    SHMData<CityInfo>       *m_poCityInfoData;
    SHMStringTreeIndex      *m_poCityInfoIndex;
    SHMStringTreeIndex      *m_poCityAreaIndex;
    CityInfo                *m_poCityInfoList;
    //ORG
    SHMData<Org>             *m_poOrgData;
    SHMIntHashIndex          *m_poOrgIndex;
    Org                      *m_poOrgList;
    //ORGEX
    SHMData<OrgEx>           *m_poOrgExData;
    SHMStringTreeIndex       *m_poOrgExIndex;
    OrgEx                    *m_poOrgExList;
    //ORGLONGTYPE
    SHMData<OrgLongType>     *m_poOrgLongTypeData;
    SHMIntHashIndex          *m_poOrgLongTypeIndex;
    OrgLongType              *m_poOrgLongTypeList;
    //HEADEX
    SHMData<HeadEx>          *m_poHeadExData;
    SHMStringTreeIndex       *m_poHeadExIndex;
    HeadEx                   *m_poHeadExList;
    //LOCALHEAD
    SHMData<LocalHead>       *m_poLocalHeadData;
    SHMStringTreeIndex       *m_poLocalHeadIndex;
    LocalHead                *m_poLocalHeadList;
    //EventType
    SHMData<EventType>       *m_poEventTypeData;
    SHMIntHashIndex          *m_poEventTypeIndex;
    EventType                *m_poEventTypeList;
    SHMData<EventTypeGroup>  *m_poEventTypeGroupData;
    SHMIntHashIndex          *m_poEventTypeGroupIndex;
    EventTypeGroup           *m_poEventTypeGroupList;
    //商品类型
    SHMData<Offer>           *m_poOfferData;
    SHMIntHashIndex          *m_poOfferIndex;
    Offer                    *m_poOfferList;
    SHMIntHashIndex          *m_poOfferSpecAttrIndex;
    SHMIntHashIndex          *m_poOfferNumAttrIndex;
    SHMIntHashIndex          *m_poOfferFreeIndex;
    //B_NBR_MAP
    SHMData<NbrMapData>      *m_poNbrMapData;
    SHMStringTreeIndex       *m_poNbrMapType1Index;
    SHMStringTreeIndex       *m_poNbrMapType2Index;
    SHMStringTreeIndex       *m_poNbrMapType4Index;
    SHMStringTreeIndex       *m_poNbrMapType8Index;
    NbrMapData               *m_poNbrMapList;
    //账目组
    SHMData<AcctItem>        *m_poAcctItemData;
    SHMIntHashIndex          *m_poAcctItemIndex;
	SHMStringTreeIndex		 *m_poAcctItemStrIndex;
    AcctItem                 *m_poAcctItemList;

    SHMData<ItemGroupMember> *m_poAcctItemGroupData;
    SHMIntHashIndex          *m_poAcctItemGroupIndex;
	SHMStringTreeIndex       *m_poAcctItemGroupStrIndex;
    ItemGroupMember          *m_poAcctItemGroupList;

    SHMData<ItemGroupMember> *m_poAcctItemGroupBData;
    SHMIntHashIndex          *m_poAcctItemGroupBIndex;
	SHMStringTreeIndex       *m_poAcctItemGroupBStrIndex;
    ItemGroupMember          *m_poAcctItemGroupBList;

    SHMData<ItemGroupMember> *m_poAcctItemBasicGroupBData;
    SHMIntHashIndex          *m_poAcctItemBasicGroupBIndex;
    ItemGroupMember          *m_poAcctItemBasicGroupBList;

    SHMData<ItemGroupMember> *m_poAcctItemDisctGroupBData;
    SHMIntHashIndex          *m_poAcctItemDisctGroupBIndex;
    ItemGroupMember          *m_poAcctItemDisctGroupBList;

    SHMIntHashIndex          *m_poAcctItemIncludeDisctIndex;

    //B_LONG_EVENT_TYPE_RULE
    SHMData<LongEventTypeRule> *m_poLongEventTypeRuleData;
    LongEventTypeRule          *m_poLongEventTypeRuleList;
    //B_TONE_EVENT_TYPE_RULE
    SHMData<LongEventTypeRuleEx> *m_poLongEventTypeRuleExData;
    LongEventTypeRuleEx          *m_poLongEventTypeRuleExList;

    //PRICING_PLAN
    SHMIntHashIndex          *m_poPricingPlanTypeIndex;

    //B_SERV_STATE_CONVERT
    SHMData<StateConvert>    *m_poStateConvertData;
    SHMIntHashIndex          *m_poStateConvertIndex;
    StateConvert             *m_poStateConvertList;

    //B_VALUE_MAP
    SHMData<ValueMapData>    *m_poValueMapData;
    SHMStringTreeIndex       *m_poValueMapIndex;
    ValueMapData             *m_poValueMapList;

    //ZONE
    SHMData<ZoneItem>         *m_poZoneItemData;
    SHMIntHashIndex           *m_poZoneItemIndex;
    SHMStringTreeIndexEx      *m_poZoneItemStrIndex;
    ZoneItem                  *m_poZoneItemList;

    //B_SPLIT_ACCT_ITEM_GROUP
    SHMData<SplitAcctItemGroup>  *m_poSplitAcctItemGroupData;
    SHMIntHashIndex              *m_poSplitAcctItemGroupIndex;
    SplitAcctItemGroup           *m_poSplitAcctItemGroupList;
    //B_SPLIT_ACCT_GROUP2EVENT_TYPE
    SHMData<SplitAcctGroupEvent> *m_poSplitAcctGroupEventData;
    SHMIntHashIndex              *m_poSplitAcctGroupEventIndex;
    SplitAcctGroupEvent          *m_poSplitAcctGroupEventList;

    //B_DISTANCE_TYPE
    SHMData<DistanceType>        *m_poDistanceTypeData;
    SHMIntHashIndex              *m_poDistanceTypeIndex;
    DistanceType                 *m_poDistanceTypeList;

    //B_MOBILE_EQUIP_REGION
    SHMData<EdgeRoamRuleEx>      *m_poEdgeRoamRuleExData;
    SHMStringTreeIndexEx         *m_poEdgeRoamRuleExTypeAIndex;
    SHMStringTreeIndexEx         *m_poEdgeRoamRuleExTypeBIndex;
    SHMStringTreeIndex           *m_poEdgeRoamRuleExMSCIndex;
    EdgeRoamRuleEx               *m_poEdgeRoamRuleExList;
    
	//------------PricingSection----------------
    PricingSection *m_poSectionList ;
    SHMData<PricingSection> *m_poPricingSectionInfoData;    
    SHMIntHashIndex *m_poSectionIndex;      
    SHMIntHashIndex *m_poStrategyIndex;

    //----Tariff------
    Tariff *m_poTariffList;
    SHMData<Tariff> *m_poTariffInfoData;    
    SHMIntHashIndex *m_poTariffIndex;  
	
	//----------Discount-------------
    Discount *m_poDiscountList;
    SHMData<Discount> *m_poDiscountInfoData;    
    SHMIntHashIndex *m_poDiscountIndex; 
        
    //-------DisctApplyer---------
    DisctApplyer *m_poDisctApplyerList;
    SHMData<DisctApplyer> *m_poDisctApplyerInfoData;
    //<DiscountID,DisctApplyer offset> 
    SHMIntHashIndex *m_poDisctApplyerIndex;
        
    //-------DisctApplyObject---------
    DisctApplyer::DisctApplyObject *m_poDisctApplyObjectList;
    SHMData<DisctApplyer::DisctApplyObject> *m_poDisctApplyObjectInfoData;  
    //<DisctApplyoffset, DisctApplyObjectoffset>    
    SHMIntHashIndex *m_poDisctApplyObjectIndex;
	
	//-------UsageType---------
	UsageTypeMgr::_USAGE_TYPE_ *m_poUsageTypeList;
	SHMData<UsageTypeMgr::_USAGE_TYPE_> *m_poUsageTypeInfoData;	
	SHMIntHashIndex *m_poUsageTypeIndex;
	
	//------------Percent------------
    Percent *m_poPercentList;
    SHMData<Percent> *m_poPercentInfoData;
    //<PercentID,Percentoffset>   
    SHMIntHashIndex *m_poPercentIndex;
        
    //------------LifeCycle------------
    LifeCycle *m_poLifeCycleList;
    SHMData<LifeCycle> *m_poLifeCycleData;
    SHMIntHashIndex *m_poLifeCycleIndex ;
    
    //------------PricingMeasure------------
    PricingMeasure *m_poPricingMeasureList;
    SHMData<PricingMeasure> *m_poPricingMeasureData;
    //<PricingMeasureID,PricingMeasure>
    SHMIntHashIndex *m_poPricingMeasureIndex ;  
    //<StrategyID,PricingMeasure>
    SHMIntHashIndex *m_poStrategyIDMeasureIndex ;   
    
    //------------COMBINE------------
    PricingCombine *m_poPricingCombineList;     
    SHMData<PricingCombine> *m_poPricingCombineData;        
    //<planID, Combine> 
    SHMIntHashIndex *m_poPlanIDIndex ;          
    //<PricingCombineID,PricingCombine> 
    //<CombineID,Combine>
    SHMIntHashIndex *m_poPricingCombineIndex;                   
    // <StrategyID,Combine> 
    SHMIntHashIndex *m_poStrategyCombineIndex;  
            
    //------------FilterCond------------
    FilterCond *m_poFilterConditionList;
    SHMData<FilterCond> *m_poFilterConditionData;
    //< FilterCond, >
    SHMIntHashIndex *m_poFilterConditionIndex ;
    
    //------------PricingSection_ConditionPoint------------
    ConditionPoint *m_poConditionPointList;
    SHMData<ConditionPoint> *m_poConditionPointData;
    // SHMIntHashIndex *m_poConditionPointIndex ;
	
	//------------loadTimeCond------------
    TimeCond *m_poTimeCondList;     
    SHMData<TimeCond> *m_poTimeCondData;        
    //<AccuTypeID, TimeCond>
    SHMIntHashIndex *m_poTimeCondIndex;
    
    //------------loadEventTypeCond------------
    EventTypeCond *m_poEventTypeCondList;       
    SHMData<EventTypeCond> *m_poEventTypeCondData;      
    //<AccuTypeID,>
    SHMIntHashIndex *m_poEventTypeCondIndex;
    
    // ------------loadCycleType------------
    CycleType *m_poCycleTypeList;       
    SHMData<CycleType> *m_poCycleTypeData;      
    //<CycleType,>
    SHMIntHashIndex *m_poCycleTypeIndex;    
        
    //------------loadAccuType--------------
    AccuType *m_poAccuTypeList;     
    SHMData<AccuType> *m_poAccuTypeData;        
    //<AccuTypeID,>
    SHMIntHashIndex *m_poAccuTypeIndex;
    
    //------------loadOfferAccuRelation------------
    OfferAccuRelation *m_poOfferAccuRelationList;       
    SHMData<OfferAccuRelation> *m_poOfferAccuRelationData;      
    //<OfferID,>
    SHMIntHashIndex *m_poOfferAccuRelationIndex;
    
    //------------loadOfferAccuExist------------------
    //OfferAccuRelation *m_poOfferAccuRelationList;     
    //SHMData<OfferAccuRelation> *m_poOfferAccuRelationData;        
    //<iOfferID+iAccuTypeID,1>
    SHMStringTreeIndex *m_poOfferAccuExistIndex;
	
	//-------------loadSpecialCycleType----------        
    //<cycle_type_id,cycle_type_id>
    SHMStringTreeIndex *m_poCycleTypeIDIndex;
	SHMData<CycleTypeID> *m_poCycleTypeIDData;    
	CycleTypeID *m_poCycleTypeIDList;
	// ----------loadOfferCombine----------
    // <offer_id, CombineOffset>
    SHMIntHashIndex *m_poOfferCombineIndex;
	SHMIntHashIndex *m_poOfferDisctIndex;

    
    // ----------loadProductCombine----------
    //<product_id, CombineOffset > 
    SHMIntHashIndex *m_poProductCombineIndex;
    
    //---------- SplitMgr----------
	SplitCfg *m_poSplitCfgList;
	SHMData<SplitCfg> *m_poSplitData;
	//< split_id,  >
	SHMIntHashIndex *m_poSplitIndex ;
	
	//----------VirtualCOMBINE----------
	PricingCombine *m_poVirtualCombineList;
	SHMData<PricingCombine> *m_poVirtualPricingCombineData;
	SHMIntHashIndex *m_poVirtualStrategyCombineIndex;
    
    //----------CGCOMBINE----------
#ifdef GROUP_CONTROL
	PricingCombine *m_poCGPricingCombineList;		
	SHMData<PricingCombine> *m_poCGPricingCombineData;		
	//<ID, Combine>
	SHMIntHashIndex *m_poCGStrategyCombineIndex;	
#endif
    
    //----------WholeDiscount------
    #ifdef WHOLE_DISCOUNT
	WholeDiscount *m_poWholeDiscountList;
    SHMData<WholeDiscount> *m_poWholeDiscountData;
	#endif
    
    //-------CombineRelation-----
    CombineRelation *m_poCombineRelationList;
    SHMData<CombineRelation> *m_poCombineRelationData;
    // < a_combineID, offset > A_Z <-> Z_A
    SHMIntHashIndex *m_poCombineRelationIndex ;
	
	//----------Directory---------
    //Directory *m_poDirectoryList;
    //SHMData<Directory> *m_poDirectoryData;
	
	// ----------SpecialNbrStrategy----------
	SpecialNbrStrategy *m_poSpecialNbrStrategyList;
	SHMData<SpecialNbrStrategy> *m_poSpecialNbrStrategyData;
	// <acc_nbr[char *], offset> 
	SHMStringTreeIndex *m_poSpecialNbrStrategyIndex;		
	// <offer_id, >
	SHMIntHashIndex *m_poSpecialNbrOfferIndex;

	//----------HostInfo---------
    HostInfo *m_poHostInfoList;
    SHMData<HostInfo> *m_poHostInfoData;
	// <host_id, >
	SHMIntHashIndex *m_poHostInfoIndex;
	
	//----------GatherTaskInfo---------
    GatherTaskInfo *m_poGatherTaskInfoList;
    SHMData<GatherTaskInfo> *m_poGatherTaskData;
	// <Task_id, offset>
	SHMIntHashIndex *m_poGatherTaskIndex;
	// <org_id, offset>
	SHMIntHashIndex *m_poOrgGatherTaskIndex;
	
	//---------ProductAcctItem----------
	// <ProductID,  1>
	SHMIntHashIndex *m_poProductIDIndex ;
	// <iProductID_acct_item_type_id, 1 > 
    SHMStringTreeIndex *m_poProIDAcctItemStringIndex ;
	
	//----------SpecialTariff---------
    SpecialTariff *m_poSpecialTariffList;
    SHMData<SpecialTariff> *m_poSpecialTariffData;
	// <UpDownFlag_SPFLAG_BillingType, offset>
	SHMStringTreeIndex *m_poSpecialTariffIndex;
	//---SpecialTariffCheck--
	SpecialTariff *m_poSpecialTariffCheckList;
    SHMData<SpecialTariff> *m_poSpecialTariffCheckData;
	// <UpDownFlag_SPFLAG, offset>
	SHMStringTreeIndex *m_poSpecialTariffCheckIndex;
	
	//----------SpInfoSeekMgr---------
    SpInfoData *m_poSpInfoDataList;
    SHMData<SpInfoData> *m_poSpInfoData;
	// <m_sPrdOfferID, offset>
	SHMStringTreeIndex *m_pHSpInfoIndex;
	
	//----------GatherTaskListInfo---------
    GatherTaskListInfo *m_poGatherTaskListInfoList;
    SHMData<GatherTaskListInfo> *m_poGatherTaskListInfoData;
	// <Task_ID, offset>
	SHMIntHashIndex *m_poGatherTaskListInfoIndex;
	
	//----------SegmentInfo---------
    SegmentInfo *m_poSegmentInfoList;
    SHMData<SegmentInfo> *m_poSegmentInfoData;
	// <segment_id, Nodeoffset>
	SHMIntHashIndex *m_poSegmentIndex;
	// <segment_type, Nodeoffset>
	SHMIntHashIndex *m_poSegmentTypeIndex;
	
	//----------LongTypeMgr---------
    // <CALLED_LONG_GROUP_TYPE_ID, LONG_TYPE_ID>
	SHMIntHashIndex *m_poLongTypeIndex;
	
	//----------RecordCheck(loadRule)---------
    CheckRule *m_poCheckRuleList;
    SHMData<CheckRule> *m_poCheckRuleData;  //s_poRuleList
	// <m_iRuleID, offset>
	SHMIntHashIndex *m_poCheckRuleIndex; //s_poRuleIndex
	
	//----------RecordCheck(loadSubRule)---------
    CheckSubRule *m_poCheckSubRuleList;
    SHMData<CheckSubRule> *m_poCheckSubRuleData;  //s_poSubRuleList
	// <iRuleID, offset>
	SHMIntHashIndex *m_poCheckSubRuleIndex; //s_poSubRuleIndex
	
	//----------RecordCheck(loadArgument)---------
    CheckArgument *m_poCheckArgumentList;
    SHMData<CheckArgument> *m_poCheckArgumentData;  //s_poArgumentList
	//SHMIntHashIndex *m_poCheckSubRuleIndex; //s_poArgumentIndex
	
	//----------RecordCheck(loadCheckRelation)---///------
    CheckRelation *m_poCheckRelationList;
    SHMData<CheckRelation> *m_poCheckRelationData;  //s_poRelationList
	//SHMIntHashIndex *m_poCheckRelationIndex; 
	
	//----------RecordCheck(TransFileMgr)---///------ 
TransFileType *m_poTransFileTypeList;
    SHMData<TransFileType> *m_poTransFileTypeData;  //s_poFileTypeList
	// <m_iTypeID, offset>
	SHMIntHashIndex *m_poTransFileTypeIndex;  // s_poFileTypeIndex
	
	//-----------ParseRule(ParseRuleMgr)----//-----
	ParseRule *m_poParseRuleList;
	SHMData<ParseRule> *m_poParseRuleData;
	SHMStringTreeIndex *m_poParseRuleIndex;
	ParseComb *m_poParseCombList;
	SHMData<ParseComb> *m_poParseCombData;
	SHMIntHashIndex *m_poParseCombIndex;
	
	//-----------InstTableList(InstTableListMgr)---//----
	InstTableList *m_poInstTableList;
	SHMData<InstTableList> *m_poInstTableData;
	SHMStringTreeIndex *m_poInstTableIndex;
	EventTableType *m_poTableTypeList;
	SHMData<EventTableType> *m_poTableTypeData;
	SHMIntHashIndex *m_poTableTypeIndex;

        //----------Rent_config---///------
        SHMIntHashIndex *m_poRentIndex;
        
	//-----------InstTableList(InstTableListMgr)---//----
	NetworkPriority *m_poNetworkPriorList;
	SHMData<NetworkPriority> *m_poNetworkPriorData;
	SHMIntHashIndex *m_poNetworkPriorIndex;
 
	//TTrunkToNBR
	SHMData<TTrunkToNBR> *m_poTrunkToBeNBRData;
	SHMStringTreeIndex *m_poTrunkToBeNBRIndex;
	TTrunkToNBR *m_poTrunkToBeNBRList;
	
	//-----------InstCheckDup(InstCheckDup)---//----
	struct CheckDupCfg *m_poCheckDupCfgList;
	SHMData<struct CheckDupCfg> *m_poCheckDupCfgData;
	SHMIntHashIndex *m_poCheckDupCfgIndex;

	struct CheckDupType *m_poCheckDupTypeList;
	SHMData<struct CheckDupType> *m_poCheckDupTypeData;
	SHMIntHashIndex *m_poCheckDupTypeIndex;

	struct CheckDupStep *m_poCheckDupStepList;
	SHMData<struct CheckDupStep> *m_poCheckDupStepData;
	SHMIntHashIndex *m_poCheckDupStepIndex;

	struct CheckDupType *m_poShotTicketList;
	SHMData<struct CheckDupType> *m_poShotTicketData;
	SHMIntHashIndex *m_poShotTicketIndex;

	struct UndoProcess *m_poUndoProcessList;
	SHMData<struct UndoProcess> *m_poUndoProcessData;
	SHMIntHashIndex *m_poUndoProcessIndex;
	
	struct UndoProcess *m_poUndoProcShotList;
	SHMData<struct UndoProcess> *m_poUndoProcShotData;
	SHMIntHashIndex *m_poUndoProcShotIndex;

	struct UndoServ *m_poUndoServList;
	SHMData<struct UndoServ> *m_poUndoServData;

	struct UndoProductPackage *m_poUndoProductList;
	SHMData<struct UndoProductPackage> *m_poUndoProductData;

	//----------Product_config---///------
	SHMIntHashIndex *m_poProductIndex;
	
		OrgPath * m_poOrgPathList;
	SHMData<OrgPath> *m_poOrgPathData;
	SHMIntHashIndex  * m_poOrgPathIndex;
	
	LocalEventTypeRule *m_poLocalEventTypeRuleList;
	SHMData<LocalEventTypeRule> * m_poLocalEventTypeRuleData;
	SHMStringTreeIndex *m_poLocalEventTypeRuleIndex;
    
    //FilePool FilePoolCondition 2010.11.17
    FilePoolInfo *m_poFilePoolInfoList;
    SHMData<FilePoolInfo> *m_poFilePoolInfoData;
    SHMStringTreeIndex *m_poFilePoolInfoIndex;

    FilePoolCondition *m_poFilePoolCondList;
    SHMData<FilePoolCondition> *m_poFilePoolCondData;
    SHMStringTreeIndex *m_poFilePoolCondIndex;
    //FileType 
    SHMStringTreeIndex *m_poFileTypeIndex;
    //TLVEncode TLVDecode TLVConvert
    FileAttr *m_poTLVEncodeList;
    SHMData<FileAttr> *m_poTLVEncodeData;
    SHMIntHashIndex *m_poTLVEncodeIndex;

    FileAttr *m_poTLVDecodeList;
    SHMData<FileAttr> *m_poTLVDecodeData;
    SHMStringTreeIndex *m_poTLVDecodeIndex;


    BillingNodeInfo *m_poBillingNodeList;
    SHMData<BillingNodeInfo> *m_poBillingNodeData;
    SHMIntHashIndex *m_poBillingNodeIndex;
	
	SHMStringTreeIndex *m_poFileParseRuleIndex;

    SwitchCode *m_poSwitchCodeList;
    SHMData<SwitchCode> *m_poSwitchCodeData;
    SHMStringTreeIndex *m_poSwitchCodeIndex;
    //2010.12.20
    TariffInfo *m_poRoamTariffList;
    SHMData<TariffInfo> *m_poRoamTariffData;
    SHMIntHashIndex *m_poRoamTariffIndex;
    
    CarrierInfo *m_poRoamCarrierList;
    SHMData<CarrierInfo> *m_poRoamCarrierData;
    SHMStringTreeIndex *m_poRoamCarrierIndex;

    SHMStringTreeIndex *m_poTariffSectorIndex;

    CountryInfo *m_poCountryList;
    SHMData<CountryInfo> *m_poCountryData;
    SHMStringTreeIndex *m_poCountryCodeIndex;
    SHMStringTreeIndex *m_poCountryAreaIndex;

    SponsorInfo *m_poSponsorList;
    SHMData<SponsorInfo> *m_poSponsorData;
    SHMStringTreeIndex *m_poSponsorIndex;

    SpecialGroupMember *m_poMemberList;
    SHMData<SpecialGroupMember> *m_poMemberData;
    SHMIntHashIndex *m_poMemberIndex;
	//prep
	//b_template_map_switch
	//<template_id,>
	SHMData<TemplateMapSwitch> * m_poTemplateMapSwitchData;
	SHMIntHashIndex *m_poTemplateMapSwitchIndex;
	TemplateMapSwitch *m_poTemplateMapSwitchList;
	
	//b_Object_Define
	SHMData<ObjectDefine> * m_poObjectDefineData;
	SHMIntHashIndex *m_poObjectDefineIndex;
	ObjectDefine *m_poObjectDefineList;
	
	//<map_id,>
	SHMData<EventParserGroup> * m_poEventParserGroupData;
	SHMIntHashIndex *m_poEventParserGroupIndex;//<mapid,>
	EventParserGroup *m_poEventParserGroupList;
	//SHMData<EventParserGroup> * m_poEPGGroupData;	
	//SHMIntHashIndex *m_poEPGGroupIndex;//<obj_id,>
	//EventParserGroup *m_poEPGGroupList;
	
	//FieldPtr
	SHMData<TemplateRelation> *m_poTemplateRelationData;
	//<GroupId,TemplateID>
	SHMIntHashIndex *m_poTemplateRelationIndex;
	TemplateRelation *m_poTemplateRelationList;
	SHMData<Field> * m_poFieldData;
	//<GroupId,起始偏移>
	SHMIntHashIndex *m_poFieldTemplateIndex;
	SHMStringTreeIndex *m_poFieldUnionStrIndex;//<,>
	Field *m_poFieldSHMList;
	
	//MapID[getMapID]
	SHMData<TemplateMapSwitch> *m_poMapSwitchTypeData;	
	SHMIntHashIndex *m_poMapSwitchTypeIndex;//<switch_type_id,起始偏移>
	TemplateMapSwitch *m_poMapSwitchTypeList;
	
	//sep
	//SHMData<EventParserGroup> *m_poSepEPGData;
	//SHMIntHashIndex *m_poSepEPGIndex;//<group_id,起始偏移>
	//EventParserGroup *m_poSepEPGList;
	//SHMData<ObjectDefine> *m_poObjectType6Data;	
	//SHMIntHashIndex *m_poObjectType6Index;//<switch_type_id,起始偏移>
	//ObjectDefine *m_poObjectType6List;
	
	//EventAttr
	SHMData<EventAttr> *m_poEventAttrData;
	SHMIntHashIndex *m_poEventAttrIndex;//<EventAttr_id,起始偏移>
	EventAttr *m_poEventAttrList;
	
	//FileInfo
	//SHMData<prep_space::FileInfo> *m_poFileInfoData;
	//SHMIntHashIndex *m_poFileInfoIndex;//<PROCESS_id,起始偏移>
	//prep_space::FileInfo *m_poFileInfoList;
	
	//getEventHeadSize跟getColumns加载的是一类数据
	SHMData<EventParserGroup> *m_poEventHeadAndColumnsCfg;
	SHMIntHashIndex *m_poEventHeadAndColumnsIndex;//<group_id,起始偏移>
	EventParserGroup *m_poEventHeadAndColumnsList;
	//getEventHeadSize以及getColumns还要加载的数据已经在loadObjectDefine加载
	
	
	SHMData<TSwitchInfo> *m_poSwitchIDMapSwitchData;
	SHMIntHashIndex *m_poSwitchIDMapSwitchIndex;//<switch_id,起始偏移>
	TSwitchInfo *m_poSwitchIDMapSwitchList;
	
	//FieldCheckMethod[load]
	SHMData<FieldCheckMethod> *m_poFieldCheckMethodData;
	SHMIntHashIndex *m_poFieldCheckMethodIndex;//<group_id,起始偏移>
	FieldCheckMethod *m_poFieldCheckMethodList;
	
	//FieldMap[load]
	SHMData<ValueMap> *m_poFieldMapData;
	SHMIntHashIndex *m_poFieldMapIndex;//<group_id,起始偏移>
	ValueMap *m_poFieldMapList;
	
	//FieldMerge
	SHMData<Field> *m_poFieldMergeData;
	SHMIntHashIndex *m_poFieldMergeIndex;//<group_id,起始偏移>
	Field *m_poFieldMergeList;
	
	//Tap3File
	SHMData<Tap3File> *m_poTap3FileData;
	Tap3File *m_poTap3FileList;
	//Tap3Block
	SHMData<Tap3BlockInfo> *m_poTap3BlockInfoData;
	//SHMIntHashIndex *m_poTap3BlockInfoIndex;//<BlockInfo->iFileTypeID*10 + pBlockInfo->iBlockType,起始偏移>
	Tap3BlockInfo *m_poTap3BlockInfoList;
	
	//SepFile
	SHMData<SepFile> *m_poSepFileData;
	SepFile *m_poSepFileList;
	//sepblock
	SHMData<SepBlockInfo> *m_poSepBlockInfoData;
	//SHMIntHashIndex *m_poSepBlockInfoIndex;//<BlockInfo->iFileTypeID*10 + pBlockInfo->iBlockType,起始偏移>
	SepBlockInfo *m_poSepBlockInfoList;
	SHMData<MServAcctInfo> *m_poMServAcctInfoData;
	SHMIntHashIndex *m_poMServAcctIndex;//<SERV_id,起始偏移>
	MServAcctInfo *m_poMServAcctInfoList;
	
	//getOneEvent
	
	//合并功能
	SHMData<MergeRuleData> *m_poMergeRuleData;
	SHMStringTreeIndex *m_poMergeRuleStrIndex;
	MergeRuleData *m_poMergeRuleDataList;
	
	SHMData<MergeRuleData> *m_poMergeRuleAssociateData;//关联规则
	SHMStringTreeIndex *m_poMergeRuleAssociateStrIndex;
	MergeRuleData *m_poMergeRuleAssociateList;//关联规则
	
	SHMData<MergeRuleData::ruleOut> *m_poMergeRuleOutData;
	SHMIntHashIndex *m_poMergeRuleOutIndex;//<OutRuleID,>
	MergeRuleData::ruleOut *m_poRuleOutList;
	
	SHMData<MergeKeyRule> *m_poMergeKeyRuleData;
	SHMIntHashIndex *m_poMergeKeyRuleIndex;//<KeyRule,>
	MergeKeyRule *m_poMergeKeyRuleList;
	
	//上传下发
	SHMData<TransPeer> *m_poTransPeerData;
	SHMIntHashIndex *m_poTransPeerIndex;//<,>
	TransPeer *m_poTransPeerList;
	
	SHMData<ParamTypeDefine> *m_poParamTypeDefineData;
	SHMStringTreeIndex *m_poParamTypeDefineStrIndex;//<,>
	ParamTypeDefine *m_poParamTypeDefineList;
	
	SHMData<ContPointTypeDefine> *m_poContPointTypeDefineData;
	SHMStringTreeIndex *m_poContPointTypeDefineStrIndex;//<,>
	ContPointTypeDefine *m_poContPointTypeDefineList;
	
	void bindStdData();
	//计算配置数据生成的实际数据
	unsigned int calculateCodeNum(char const *_sSegment, char const *_sCode,int iStructSize);

	
    long m_lDataKey[PARAM_SHM_NUMBER];
    long m_lIndexKey[PARAM_SHM_NUMBER];
    
    //是否成功连接共享内存
    bool m_bAttached;
	bool m_bdetached;
private:
#endif
};


#endif  //SHMPARAMINFO_H_HEADER_INCLUDED

