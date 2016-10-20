#ifndef _HSSCONSTDEF_H_
#define _HSSCONSTDEF_H_

#include "StdAfx.h"

#define HSS_ENUM_NUM 30
#define HSS_FUNCTION_NUM 20

#define HSS_DATA_FORMAT_14 "YYYYMMDDHH24MISS"
#define HSS_DATA_FORMAT_8 "YYYYMMDD"

#define HSS_FIELD_TYPE_NUM 1
#define HSS_FIELD_TYPE_STRING 2
#define HSS_FIELD_TYPE_DATE 3
#define HSS_FIELD_TYPE_MOBILE 4
#define HSS_FIELD_TYPE_IDENTITY 5
#define HSS_FIELD_TYPE_POSTCODE 8
#define HSS_FIELD_TYPE_IP 6
#define HSS_FIELD_TYPE_EFFDATE 10
#define HSS_FIELD_TYPE_EXPDATE 11
#define HSS_FIELD_TYPE_PROVINCECODE 7
#define HSS_FIELD_TYPE_LATCODE 9

#define HSS_FUN_TYPE_DOMAIN 3
#define HSS_FUN_TYPE_TABLE 4
#define HSS_FUN_TYPE_FIELDTYPE 5
#define HSS_FUN_TYPE_FIELD 1
#define HSS_FUN_TYPE_AVP 2


#define HSS_FUN_CHECK_ISNULL 10
#define HSS_FUN_CHECK_ENUM 11
#define HSS_FUN_CHECK_FIELDTYPE 12
#define HSS_FUN_CHECK_NUMBER 28
#define HSS_FUN_CHECK_CHAR 29

#define HSS_FUN_CHECK_REFCUST 13
#define HSS_FUN_CHECK_REFSERV 14
#define HSS_FUN_CHECK_REFACCT 15
#define HSS_FUN_CHECK_BANK 16
#define HSS_FUN_CHECK_SERVMAX 17
#define HSS_FUN_CHECK_OFFERINSMAX 18

#define HSS_FUN_CHECK_OFFEROBJ_DATE 19
#define HSS_FUN_CHECK_OFFERINS_DATE 20
#define HSS_FUN_CHECK_OFFEROBJ_NUM 21
#define HSS_FUN_CHECK_OFFEROBJ_TYPE 22
#define HSS_FUN_CHECK_OFFERINS_ATTR 23
#define HSS_FUN_CHECK_OFFERINS_PARAM 24

#define HSS_FUN_CHECK_SERV_EXIST 81
#define HSS_FUN_CHECK_ACCT_EXIST 82

#define HSS_FUN_CHECK_GROUP_MEMBER_IN_SERV 25






#define HSS_FUN_CHECK_EFF_EXPDATE 30

#define HSS_FUN_CHECK_GROUP_MEMBER_EXIST 40
#define HSS_FUN_CHECK_OFFERPARAM_INS_DATE 50
#define HSS_FUN_CHECK_OFFER_PARAM_ENUM_EXIST 60
#define HSS_FUN_CHECK_OFFER_PARAM_EXIST 70
#define HSS_FUN_CHECK_PRODUCT_OFFER_EXIST 80

#define HSS_FUN_CHECK_SERV_EFF_DATE_CHECK 90
#define HSS_FUN_CHECK_PRODOBJINS_EFF_DATE_CHECK 91
#define HSS_FUN_CHECK_PRODPARAMINS_EFF_DATE_CHECK 92
#define HSS_FUN_CHECK_PRODOFFERINS_I_EXP_DATE_CHECK 93
#define HSS_FUN_CHECK_PRODOFFERINS_U_EXP_DATE_CHECK 94
#define HSS_FUN_CHECK_PRODOFFERINS_INNET_DATE_CHECK 95
#define HSS_FUN_CHECK_PRODOFFE_BILLMODE_CHECK 96
#define HSS_FUN_CHECK_PRODUCT_OFFER_RELA_A 97
#define HSS_FUN_CHECK_PRODUCT_OFFER_RELA_B 98

#define HSS_FUN_CHECK_TWO_PARAM_EQUAL 200
#define HSS_FUN_CHECK_KEY_IN_TABLE_EXIST 201
#define HSS_FUN_CHECK_KEY_IN_TABLE_NOTEXIST 202
#define HSS_FUN_CHECK_XML_FIELD_EXIST 210
#define HSS_FUN_GET_XML_FIELD_INFO 220

#define HSS_FUN_SUBSCRIBE_OFCS 100
#define HSS_FUN_SUBSCRIBE_OCS 110
#define HSS_FUN_SUBSCRIBE_ALL 120
#define HSS_FUN_SUBSCRIBE_MOBILE 130
#define HSS_FUN_SUBSCRIBE_PRODUCT_ATTR 140

const int CONVERT_UPPER = 1;
const int CONVERT_LOWER = 2;
const int POINT_IS_NULL =0;
const string STRING_IS_NULL = "";
const int INT_INIT = 0;
const char FILE_FILL_LEFT = 'L';
const char FILE_FILL_RIGHT = 'R';
/********************************************************
*                   订单命令编码
*					（1--100）
*********************************************************/


#define HSSCMD_ORDER_SYNCHRO          	1   //定单数据接收
#define HSSCMD_REGULATION_SYNCHRO      	2   //规格数据接收
#define HSSCMD_CUSTDOMAIN_SYNCHRO       3   //参与人域数据接收
#define HSSCMD_ACCTDOMAIN_SYNCHRO       4   //账务域数据接收
#define HSSCMD_PRODUCTDOMAIN_SYNCHRO    5   //产品域数据接收
//关键局数据接受命令码

#define HSSCMD_CTOC_MIN_SYNCHRO         20  //接收MIN码数据文件
#define HSSCMD_HCODE_SYNCHRO            21  //接收H码发数据文件
#define HSSCMD_CTOG_IMSI_SYNCHRO        22  //接收不同制式IMSI码表数据文件
#define HSSCMD_CTRY_SYNCHRO             23  //接收国家信息表数据文件
#define HSSCMD_CTOC_CARRIER_SYNCHRO     24  //接收C网国际漫游运营商数据文件
#define HSSCMD_CTOG_CARRIER_SYNCHRO     25  //接收不同制式国际漫游运营商表数据文件
#define HSSCMD_SPCODE_SYNCHRO           26  //接收SP码表数据文件
#define HSSCMD_OBRST_SYNCHRO            27  //接收省际边界基站信息表数据文件
#define HSSCMD_BUSI_TYPE_SYNCHRO        28  //接收增值业务类型表数据文件
#define HSSCMD_IISRATE_SYNCHRO          29  //接收国际漫游来访结算费率表数据文件
#define HSSCMD_NPCODE_SYNCHRO           30  //接收携号转网数据文件
#define HSSCMD_CELLINFO_SYNCHRO         31   //基站数据接收
#define HSSCMD_BORDER_SYNCHRO      			32   //边漫数据接收



#define HSSCMD_ORDER_RELEASE          	61   //定单数据发布
#define HSSCMD_REGULATION_RELEASE      	62   //规格数据发布
#define HSSCMD_CUSTDOMAIN_RELEASE       63   //参与人域数据发布
#define HSSCMD_ACCTDOMAIN_RELEASE       64   //账务域数据发布
#define HSSCMD_PRODUCTDOMAIN_RELEASE    65   //产品域数据发布

//关键局数据下发命令码
#define HSSCMD_CTOC_MIN_RELEASE         40  //MIN码下发数据文件
#define HSSCMD_HCODE_RELEASE            41  //H码下发数据文件
#define HSSCMD_CTOG_IMSI_RELEASE        42  //不同制式IMSI码表下发数据文件
#define HSSCMD_CTRY_RELEASE             43  //国家信息表下发数据文件
#define HSSCMD_CTOC_CARRIER_RELEASE     44  //C网国际漫游运营商下发数据文件
#define HSSCMD_CTOG_CARRIER_RELEASE     45  //不同制式国际漫游运营商表下发数据文件
#define HSSCMD_SPCODE_RELEASE           46  //SP码表下发数据文件
#define HSSCMD_OBRST_RELEASE            47  //省际边界基站信息表下发数据文件
#define HSSCMD_BUSI_TYPE_RELEASE        48  //增值业务类型表下发数据文件
#define HSSCMD_IISRATE_RELEASE          49  //国际漫游来访结算费率表下发数据文件
#define HSSCMD_NPCODE_RELEASE           50  //携号转网下发数据文件
#define HSSCMD_CELLINFO_RELEASE         51  //发布CELL_INFO

/********************************************************
*                   其他通用命令编码
*					（101--150）查询使用
*					（151--256）其他
*********************************************************/

#define HSSCMD_SERV_QUERY            		101   //用户信息查询
#define HSSCMD_SERVPRODUCT_QUERY        102   //附属产品信息查询
#define HSSCMD_ACCT_QUERY         			103   //帐户信息查询
#define HSSCMD_CUST_QUERY       				104   //客户信息查询
#define HSSCMD_ORDERRESOURCE_QUERY      105   //销售品信息查询
#define HSSCMD_GROUPMEMBER_QUERY        106   //通过群实例查找群成员
#define HSSCMD_GROUP_QUERY 							107  //通过群成员查找群信息
#define HSSCMD_EXISTHEAD_QUERY					108  //特殊号头查询
#define HSSCMD_HOMEBYACCNBR_QUERY       110   //归属查询（按号码）
#define HSSCMD_HOMEBYIMSI_QUERY     		111   //归属查询（按IMSI）
#define HSSCMD_HCODE_QUERY           		112   //H码查询
#define HSSCMD_LOCALHEAD_QUERY      		113   //本地号头表查询
#define HSSCMD_BORDERROAM_QUERY         114   //边漫判断
#define HSSCMD_IBRSTCODE_QUERY     			115   //	C网国际漫游运营商
#define HSSCMD_IMSICODE_QUERY           116   //不同制式IMSI码查询
#define HSSCMD_MINCODE_QUERY      			117   //MIN码查询
#define HSSCMD_NPCODE_QUERY           	118   //携号转网判断
#define HSSCMD_SPCODE_QUERY      				119   //SP判断
#define HSSCMD_PRODUCTOFFER_QUERY      	120   //增值业务查询
#define HSSCMD_DATAREPORT_QUERY         121   //业务网元提供汇总信息查询接口




#define HSSCMD_OFFERDOMAIN_REGISTER     153   //销售品注册
#define HSSCMD_OFFERDOMAIN_APPROVE      154   //销售品审核

#define PRODUCT_COMMAND_TYPE_EFF				1			//销售品体系生效时间检查
#define PRODUCT_ADD_TYPE_EXP 						2			//新装销售品失效时间检查
#define PRODUCT_MODIFY_TYPE_EXP 				3			//改装销售品失效时间检查
#define PRODUCT_ONLINE_TIME						4			//online time

#define PRODUCT_ACTION_NEW_SUB 					1			//新用户订购
#define PRODUCT_ACTION_OLD_SUB 					2			//老用户订购
#define PRODUCT_ACTION_UNSUB 						3			//退订
#define PRODUCT_ACTION_CHG_MEM 					4			//成员变更
#define PRODUCT_ACTION_CHG_PARAM 				5			//参数变更

#define TIME_UNIT_HOUR 									1			//小时
#define TIME_UNIT_DAY 									2			//天
#define TIME_UNIT_WEEK 									3			//周
#define TIME_UNIT_MONTH 								4			//月
#define TIME_UNIT_YEAR 									5			//年


#endif

