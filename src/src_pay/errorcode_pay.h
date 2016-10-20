#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

/********************************************************
*  本文件定义了《中国电信计费域全网互联错误码（V1.01）》
*  的公共部分和HSS部分，如有添加请按照各个模块自行处理
*								2011.5.30   liqf
*********************************************************/


/********************************************************
*                   公用严重错误
*					（1--100）
*********************************************************/
#define ECODE_MEM_MALLOC_FAIL						1		//内存分配失败
#define ECODE_MEM_SPACE_SMALL						2		//内存空间不足
#define ECODE_MEM_INTERFACE_ERROR					3		//内存查表接口错误
#define ECODE_MEM_LINK_FAIL							4		//共享内存连接失败
#define ECODE_THREAD_CREAT_FAIL						5		//派生进程或线程失败
#define ECODE_SEMP_OPERA_FAIL						6		//信号处理失败
#define ECODE_LOCK_GET_FAIL							7		//获取锁资源失败
#define ECODE_SOCKET_LISTEN_FAIL					8		//Socket--监听出错
#define ECODE_SOCKET_LINK_FAIL						9		//连接远端Socket错
#define ECODE_DISC_SPACE_SMALL						10		//磁盘存储空间不足
#define ECODE_CPU_OVERLOAD							11		//CPU超负荷
#define ECODE_DB_LINK_FAIL							12		//连接数据库失败
#define ECODE_DB_TABLESPACE_SMALL					13		//表空间不足
#define ECODE_ENV_NOT_SET							14		//环境变量未设置
#define ECODE_SYS_BUSY								15		//系统繁忙
/********************************************************
*                   系统运行错误
*					（101--300）
*********************************************************/
#define ECODE_FILE_OPEN_FAIL						101		//打开文件失败
#define ECODE_FILE_WRITE_FAIL						102		//写文件失败
#define ECODE_DIR_OPEN_FAIL							103		//打开目录出错
#define ECODE_FILE_RENAME_FAIL						104		//文件改名出错
#define ECODE_FILE_SEQ_GET_FAIL						105		//序列获取失败
#define ECODE_FILE_NOT_EXIST						106		//指定文件不存在
#define ECODE_FILE_COPY_FAIL						107		//拷贝文件失败
#define ECODE_FILE_MOVE_FAIL						108		//移动文件失败

#define ECODE_MEM_UPDATE_FAIL						116		//共享内存更新失败
#define ECODE_MEM_IMPORT_FAIL						117		//上载异常   hbimport命令
#define ECODE_MEM_EXPORT_FAIL						118		//内存数据导出异常 hbexport
#define ECODE_MEM_CREATE_FAIL						119		//创建共享内存失败
#define ECODE_MEM_DELETE_FAIL						120		//删除共享内存失败
#define ECODE_MEM_DESTROY_FAIL						121		//销毁共享内存失败
#define ECODE_SEMP_CREATE_FAIL						122		//创建信号量失败

#define ECODE_DB_SERV_PWD_ERROR						141		//用户名密码错误
#define ECODE_DB_LINK_CLOSE_FAIL					142		//关闭数据库连接失败
#define ECODE_DB_TABLE_NOT_EXIT						143		//表名不存在
#define ECODE_DB_ROLLBACK_FAIL						144		//数据库事务回滚错误
#define ECODE_DB_EXECUTE_FAIL						145		//发生sql执行异常
#define ECODE_DB_TICKET_IMPORT_FAIL					146		//话单入数据库失败


#define ECODE_NET_PROTOCOL_PACKAGE_FAIL				156		//协议封装失败，协议名称%s，错误原因%s.
#define ECODE_NET_PROTOCOL_PARSE_FAIL				157		//协议解析失败，协议名称%s，错误原因%s.
#define ECODE_NET_TUXEDO_CALL_FAIL					158		//tuxedo调用失败，失败原因%s

#define ECODE_PARAM_FILE_FORMAT_ERROR				166		//配置文件格式错误
#define ECODE_PARAM_THREAD_LOST_ERROR				167		//应用进程参数不正确（缺少必选项）
#define ECODE_PARAM_THREAD_NONSUPP_ERROR			168		//应用进程参数不正确（不支持的选项）
#define ECODE_PARAM_THREAD_VALUE_ERROR				169		//应用进程参数不正确（错误的取值）

#define ECODE_PARAM_UPDATE_FAIL						170		//参数更新失败
#define ECODE_PARAM_FILE_ERROR						171		//参数文件缺失
#define ECODE_PARAM_LOST_ERROR						172		//参数配置信息缺失
#define ECODE_PARAM_VALUE_NULL_ERROR				173		//字段不能为空
#define ECODE_PARAM_LEN_MIN_ERROR					174		//小于最小长度
#define ECODE_PARAM_LEN_MAX_ERROR					175		//大于最大长度
#define ECODE_PARAM_LEN_FIXED_ERROR					176		//非固定长度
#define ECODE_PARAM_VALUE_ENUM_ERROR				177		//枚举值不正确
#define ECODE_PARAM_VALUE_MIN_ERROR					178		//小于最小值
#define ECODE_PARAM_VALUE_MAX_ERROR					179		//大于最大值
#define ECODE_PARAM_ERROR							180		//参数错误


#define ECODE_INFO_CUST_GET_FAIL					181		//取客户信息失败
#define ECODE_INFO_ACCT_GET_FAIL					182		//取帐户信息失败
#define ECODE_INFO_SERV_GET_FAIL					183		//取用户信息失败
#define ECODE_INFO_BALANCE_GET_FAIL					184		//取余额失败
#define ECODE_INFO_CURR_BALANCE_GET_FAIL			185		//取实时费用失败
#define ECODE_INFO_HIS_OWE_GET_FAIL					186		//取历史欠费失败
#define ECODE_INFO_CREDIT_GET_FAIL					187		//取信用度失败
#define ECODE_INFO_BILL_GET_FAIL					188		//查账单失败
#define ECODE_INFO_TICKET_GET_FAIL					189		//查清单失败
#define ECODE_INFO_INVOICE_GET_FAIL					190		//查发票失败


#define ECODE_HREAD_EXIST_ERROR						196		//应用进程已经存在
#define ECODE_HREAD_ERROR							197		//预处理进程异常
#define ECODE_HREAD_STATUS_ERROR					198		//进程状态非法
#define ECODE_HREAD_EXIT_ERROR						199		//应用程序收到退出信号异常退出

#define ECODE_SRVC_BILLINGCYCLE_GET_FAIL			211		//获取对应账期失败
#define ECODE_SRVC_ARCHIVES_MEM_UPDATE_FAIL			212		//CRM档案接口更新内存失败
#define ECODE_SRVC_SEQ_CONTINUE_FAIL				213		//序号连续性校验失败
#define ECODE_SRVC_SOURCE_EVENT_ERROR				214		//源事件类型不合法
#define ECODE_SRVC_TICKET_FROMAT_TRANS_FAIL			215		//话单格式转化失败
#define ECODE_SRVC_BILL_INIT_FAIL					216		//计费应用初始化失败
#define ECODE_SRVC_PRICING_GET_FAIL					217		//获取定价计划信息出错
#define ECODE_SRVC_PRICING_EXECUTE_FAIL				218		//定价计划处理出错
#define ECODE_SRVC_BILL_ROLLBACK_FAIL				219		//计费处理回退失败
#define ECODE_SRVC_RECOVE_FAIL						220		//处理回收失败
#define ECODE_SRVC_TICKET_DISTRIBUTE_FAIL			221		//话单分发失败
#define ECODE_SRVC_ACCU_FAIL						222		//积量错误
#define ECODE_SRVC_BALANCE_UPDATE_FAIL				223		//更新余额失败
#define ECODE_SRVC_CURR_BALANCE_UPDATE_FAIL			224		//更新实时费用失败（如：当月实时费用特别缴费做结清处理。）
#define ECODE_SRVC_OWE_UPDATE_FAIL					225		//更新欠费失败
#define ECODE_SRVC_STOPDATA_CREATE_FAIL				226		//生成停机数据失败
#define ECODE_SRVC_REGAINDATA_CREATE_FAIL			227		//生成复机数据失败
#define ECODE_SRVC_REMIND_PAYMENT_CREATE_FAIL		228		//生成催缴数据失败
#define ECODE_SRVC_INVOICE_FORMAT_FAIL				229		//取发票格式错误
#define ECODE_SRVC_INVOICE_PRINT_FAIL				230		//发票打印失败
#define ECODE_SRVC_INTERFACE_FAIL					231		//外围接口故障(如超时等)
#define ECODE_SRVC_RECEIVE_FAIL						232		//系统接收异常数据
#define ECODE_SRVC_PAYMENT_MONEY_MAX_ERROR			233		//缴费金额超过最大金额数
#define ECODE_SRVC_BANK_BILL_FAIL					234		//银行代扣销帐失败
#define ECODE_SRVC_BATCH_BILL_FAIL					235		//批量销帐失败
#define ECODE_SRVC_BATCH_REGAIN_FAIL				236		//批量复机错误
#define ECODE_SRVC_VC_MONEY_LARGE_ERROR				237		//充值金额过大，系统不支持
#define ECODE_SRVC_BALANCE_DATE_FAIL				238		//余额账本过期
#define ECODE_SRVC_BILLBACK_REPEAT_FAIL				239		//已经反销 不能重复反销
#define ECODE_SRVC_SEQ_REPEAT_FAIL					240		//流水号重复

#define ECODE_PARAM_DATE_FORMAT_ERROR				246		//时间格式不正确
#define ECODE_PARAM_DATE_MIN_ERROR					247		//小于最小时间
#define ECODE_PARAM_DATE_MAX_ERROR					248		//大于最大时间
#define ECODE_PARAM_STRING_ERROR					249		//出现非法字符
#define ECODE_PARAM_IP_FROMAT_ERROR					250		//IP地址格式不正确
#define ECODE_PARAM_ACCNBR_FROMAT_ERROR				251		//号码格式不符合要求
#define ECODE_PARAM_PROVINCE_FROMAT_ERROR			252		//省份编码不正确
#define ECODE_PARAM_LOCAL_FROMAT_ERROR				253		//本地网编码不正确
#define ECODE_PARAM_POST_FROMAT_ERROR				254		//邮政不符合要求
#define ECODE_PARAM_NUMBER_FROMAT_ERROR				255		//数字型字段出现字符
#define ECODE_PARAM_CHARS_ERROR						256		//字符型字段出现非法字符
#define ECODE_ARCHIVING_PROCESS_ERROR				261		//数据归档失败
#define ECODE_OPERATE_ERROR							262		//操作失败



/********************************************************
*                   静态参数错误码
*					（301--500）
*********************************************************/

#define ECODE_NP_NOT_EXIST							301		//携号转网数据不存在
#define ECODE_NP_EXP								302		//携号转网数据已失效
#define ECODE_NP_INVALID							303		//携号转网数据未生效
#define ECODE_NP_SERV_REMOVED						304		//用户已拆机

#define ECODE_IBRST_CARRIER_NOT_EXIST				311		//C网国漫被访运营商不存在
#define ECODE_IBRST_CARRIER_EXP						312		//C网国漫被访运营商已失效
#define ECODE_IBRST_CARRIER_VALID					313		//C网国漫被访运营商尚未生效
#define ECODE_IBRST_COUNTRY_NOT_EXIST				314		//C网国漫被访运营商所在国家不存在
#define ECODE_IBRST_REGION_NOT_EXIST				315		//C网国漫被访运营商所属资费区域不存在

#define ECODE_CTOG_CARRIER_NOT_EXIST				321		//不同制式国漫被访运营商不存在
#define ECODE_CTOG_CARRIER_EXP						322		//不同制式国漫被访运营商已失效
#define ECODE_CTOG_CARRIER_INVALID					323		//不同制式国漫被访运营商尚未生效
#define ECODE_CTOG_COUNTRY_NOT_EXIST				324		//不同制式国漫被访运营商所在国家不存在
#define ECODE_CTOG_NOT_EXIST						325		//不同制式国漫CtoG代理运营商不存在
#define ECODE_CTOG_REGION_NOT_EXIST					326		//不同制式国漫被访运营商与所属资费区域不存在


#define ECODE_MIN_NOT_EXIST							331		//MIN码不存在
#define ECODE_MIN_EXP								332		//MIN码已失效
#define ECODE_MIN_INVALID							333		//MIN码尚未生效
#define ECODE_MIN_COUNTRY_NOT_EXIST					334		//MIN码归属国家代码异常（不存在）
#define ECODE_MIN_COUNTRY_EXP						335		//MINMIN码归属国家代码异常（失效）
#define ECODE_MIN_COUNTRY_MUL						336		//MINMIN码归属国家代码异常（多个）
#define ECODE_MIN_CARRIER_NOT_EXIT					337		//MINMIN码归属运营商代码异常（不存在）
#define ECODE_MIN_CARRIER_EXP						338		//MIN码归属运营商代码异常（失效）
#define ECODE_MIN_CARRIER_MUL						339		//MIN码归属运营商代码异常（多个）

#define ECODE_IMSI_NOT_EXIST						346		//IMSI码不存在
#define ECODE_IMSI_EXP								347		//IMSI码已失效
#define ECODE_IMSI_INVALID							348		//IMSI码尚未生效
#define ECODE_IMSI_CTOG_ERROR						349		//IMSI对应CtoG代理运营商代码异常
#define ECODE_IMSI_LOCAL_MUL						350		//IMSI对应多个本地网

#define ECODE_COUNTRY_NOT_EXIST						356		//国家信息不存在
#define ECODE_COUNTRY_EXP							357		//国家信息已失效
#define ECODE_COUNTRY_INVALID						358		//国家信息尚未生效
#define ECODE_COUNTRY_MUL							359		//国家代码重复
#define ECODE_COUNTRY_ERROR							360		//国家名称异常

#define ECODE_HCODE_NOT_EXIST						366		//H码不存在
#define ECODE_HCODE_EXP								367		//H码已失效
#define ECODE_HCODE_INVALID							368		//H码尚未生效
#define ECODE_HCODE_LOCAL_MUL						369		//H码归属本地网异常（多个）
#define ECODE_HCODE_CARRIER_MUL						370		//H码归属运营商异常（多个）
#define ECODE_HCODE_CARRIER_NOT_EXIT				371		//H码归属运营商异常（不存在）

#define ECODE_SP_NOT_EXIST							376		//SP代码不存在
#define ECODE_SP_EXP								377		//SP代码已失效
#define ECODE_SP_INVALID							378		//SP代码尚未生效

#define ECODE_BORDER_ROAM_PROVINCE_MUL				386		//边漫话单归属多个省



/********************************************************
*                   HSS逻辑判断错误码
*					（12000--12999）
*********************************************************/

#define ECODE_HSS_UPDATE_NOT_EXIST					12000	//更新记录不存在
#define ECODE_HSS_LAST_CONTINUE_NOT_EXIT			12001	//找不到上一个连续消息包
#define ECODE_HSS_AUTH_PAYPWD_FAIL					12002	 //支付密码鉴权失败
#define ECODE_HSS_CUSTID_ERROR						12003	//大客户标识不符合要求
#define ECODE_HSS_PRODUCTID_ERROR					12004	//产品编码不符合要求

#define ECODE_HSS_SERV_CUST_ERROR					12100	//有用户资料没有客户资料
#define ECODE_HSS_SERV_ACCT_ERROR					12101	//有用户资料没有帐户资料
#define ECODE_HSS_ACCT_CUST_ERROR					12102	//有帐户资料没有客户资料
#define ECODE_HSS_REFCUST_NOT_EXIT					12103	//依赖的大客户不存在
#define ECODE_HSS_REFSERV_NOT_EXIT					12104	//依赖的用户信息不存在
#define ECODE_HSS_REFACCT_NOT_EXIT					12105	//依赖的账户信息不存在
#define ECODE_HSS_BANK_NOT_EXIST					12106	//银行托收无银行帐号
#define ECODE_HSS_SERV_MAX_ERROR					12107	//超过最大用户数

#define ECODE_HSS_OFFERINS_DATE_NOT_CONST_ERROR		12200	//销售品实例与销售品包含对象实例生失效时间不一致
#define ECODE_HSS_OFFER_EXP_MIN_EFF_ERROR			12201	//销售品失效日期小于生效日期
#define ECODE_HSS_OFFER_MEMBER_NUM_ERROR			12202	//销售品包含对象成员数量异常（产品缺失或多）
#define ECODE_HSS_OFFER_MEMBER_TYPE_ERROR			12203	//销售品包含对象成员类型异常
#define ECODE_HSS_OFFERINS_ATTR_LOST_ERROR			12204	//缺少必填的销售品属性实例
#define ECODE_HSS_OFFERINS_PARAM_LOST_ERROR			12205	//缺少必填的销售品参数实例
#define ECODE_HSS_OFFERINS_NUM_MAX_ERROR			12206	//超过最大销售品实例数
#define ECODE_HSS_OFFERINS_NUM_MAX_ERROR			12250	//电信网内群成员资料在主产品实例表无数据
#define ECODE_HSS_OFFERINS_NUM_MAX_ERROR			12260	//开户失败
#define ECODE_HSS_OFFER_CATALOG_APPROVE_FAIL		12270	//销售品目录审批失败

/********************************************************
*                   HSS自定义错误码
*					（12900--12999）
*********************************************************/

#define ECODE_HSS_SUCCESS							0		//成功 
#define ECODE_HSS_TIMEOUT							12900	//系统超时
#define ECODE_HSS_FAILURE							12901	//系统错误
#define ECODE_HSS_RDCFG_FAIL						12902	//客户端读取配置文件错误
#define ECODE_HSS_PARAM_INVALID						12903	//客户端参数失效
#define ECODE_HSS_MQ_OP_FAIL						12904	//客户端消息队列操作失败
#define ECODE_HSS_MQ_NOMSG							12905	//客户端消息队列为空
#define ECODE_HSS_FTP_SEND_ERR						12906	//客户端FTP发送异常
#define ECODE_HSS_FTP_RECV_ERR						12907	//客户端FTP接受异常
#define ECODE_HSS_DISPATCH_FAIL						12908	//客户端分发异常
#define ECODE_HSS_MEM_DB_ERROR						12909	//内存数据库错误

// 新增返回码 用于客户端询问HSS服务端是否执行过某条命令, 服务端m_iRltCode=ECODE_HSS_CMD_REPEAT

#define ECODE_HSS_CMD_REPEAT						12910	//表示该命令执行过

#endif
/*****************************************************
 *	 集团ABM主动余额划拨，被动余额划拨，
 *                  划拨冲正，支付冲正错误码
 *
 *	 (13800~13899)
 *
 */
#define DEDUCT_UNPACK_NOMSG						13800	//被动余额划拨-消息包为空
#define DEDUCT_UNPACK_ERR						13801	//被动余额划拨-解包错
#define DEDUCT_SERV_NOT_EXIST						13802	//用户信息不存在
#define DEDUCT_SERV_QRY_ERR						13803	//用户查询失败
#define TRANSFER_RULUES_NO_EXIT						13804	//划拨规则不存在
#define TRANSFER_RULUES_QRY_ERR						13805	//划拨规则查询失败
#define OVER_DAY_TRANS_FREQ						13806	//当天已发生次数>每天划拨频度
#define OVER_DAY_TRANS_LIMIT						13807	//超过当天划拨限额
#define OVER_MONTH_TRANS_LIMIT						13808	//超过月划拨限额
#define BALANCE_NOT_DEDUCT						13809	//余额帐本金额不足划拨
#define DEDUCT_TT_ERR							13810	//余额划拨数据库出错
#define DEDUCT_PACK_ERR							13811	//余额划拨打包失败
#define UNPACK_PLATFORM_CCR_NOMSG					13812	//解自服务平台请求CCR-消息包为空
#define UNPACK_PLATFORM_CCR_ERR						13813	//解自服务平台请求CCR-解包错
#define ACTIVE_DEDUCT_TT_ERR						13814	//主动划拨ActiveAllocateBalance,数据库出错
#define CENTER_ABM_DEDUCT_IN_ERR					13815	//中心ABM划拨充值失败
#define CENTER_ABM_DEDUCT_OUT_ERR					13816	//中心ABM划拨充值失败
#define SAVE_PLATFOR_CCR_ERR						13817	//保存自服务平台请求CCR消息头失败
#define SAVE_PLATFOR_CCR_BIZ_ERR					13818	//保存自服务平台请求CCR业务信息失败
#define QRY_SESSION_ID_ERR						13819	//查询DCC会话标识Session-Id失败
#define QRY_SERV_ORGID_ERR						13820	// 查询用户号码所属机构失败
#define ACTIVE_PACK_CCR_ERR						13821	// 组省ABM被动划拨请求CCR-打包失败
#define ACTIVE_PACK_CCR_OUT_ERR						13822	// 组中心ABM主动划拨请求CCR-打包失败

#define ACTIVE_DEDUCT_BIZ_TT_ERR					13823	//主动划拨收发CCA，数据库出错
#define DEDUCT_BIZ1_IN_ERR						13824	//划拨充值失败
#define DEDUCT_BIZ2_IN_ERR						13825	//主动划出失败

#define UNPACK_CCA1_ERR							13826	//主动划拨，CCA解包失败，省ABM被动请求CCA
#define UNPACK_CCA2_ERR							13827	//主动划拨，CCA解包失败，中心ABM主动请求CCA
#define UNPACK_REVERSE_DEDUCT_NOMSG					13828	//划拨冲正-请求CCR解包消息体为空
#define UNPACK_REVERSE_DEDUCT_ERR					13829	//划拨冲正-请求CCR解包失败
#define REVERSE_DEDUCT_TT_ERR						13830	//划拨冲正,数据库出错
#define REVERSE_DEDUCT_BIZ_ERR						13831	//划拨冲正,业务处理失败


#define SAVE_RECEIVE_DCC_CCA_ERR					13832	//保存接收的CCA消息头失败
#define SAVE_SEND_DCC_CCA_ERR						13833	//保存发出的CCA消息头失败

#define SAVE_RECEVIE_DCC_CCR_ERR					13834	//保存接收的CCR消息头失败
#define REVERSE_DEDUCT_PACK_ERR						13835	//划拨冲正-返回CCA打包失败
#define GET_DCC_INFO_RECORD_SEQ_ERR					13836	//获取DCC_INFO_RECORD_SEQ失败

#define RETURN_CCA_UNPACK_NOMSG						13837	//接收省ABM返回的CCA包消息为空
#define RETURN_CCA_UNPACK_ERR						13838	//接收省ABM返回的CCA包解包错
#define DEDUCT_IN_ERR							13839	//划拨充值失败
#define GET_SESSION_ID_SEQ_ERR						13840	//取SESSION_ID 序列失败
#define GET_ACCT_NBR_ORGID_ERR						13841	//取号码所属机构代码失败
#define DEDUCT_IN_PACK_ERR						13842	//划拨充值打包失败
#define GET_DCC_INFO_RECORD_SEQ_ERR					13843	//取DCC_INFO_RECORD_SEQ失败
#define DEDUCT_OUT_ERR							13844	//主动划出业务处理失败
#define QRY_PRE_SESSION_ID_ERR						13845	//查询源会话标识失败
#define RETURN_BIZ_CODE_ERR						13846	//省ABM返回CCA-业务级结果代码非0！
#define DEDUCT_AMOUNT_ERR						13847	//余额划拨金额小于等于0！


/*****************************************************
 *	 集团abm充值，充值冲正，支付错误码
 *
 *        （13900~13999）
 *
 * **************************************************/
#define DEPOSIT_UNPACK_ERR						13900	//充值解包错
#define DEPOSIT_PACK_ERR						13901	//充值打包错
#define DEPOSIT_TT_ERR							13902	//充值数据库错
#define PAYMENT_UNPACK_ERR						13903	//支付解包错
#define PAYMENT_PACK_ERR						13904	//支付打包错
#define PAYMENT_TT_ERR							13905	//支付数据库错
#define REVERSE_PAYMENT_TT_ERR						13906	//支付解包错
#define BALANCE_PAYOUT_TT_ERR						13907	//余额划拨数据库错
#define REVER_BALANCE_PAYOUT_TT_ERR					13908	//余额划拨冲正数据库错
#define BALANCE_NOT_PAYMENT_ERR						13909	//账本余额不足以支付
#define REVERSE_DEP_UNPACK_ERR						13910	//充值冲正解包错
#define REVERSE_DEP_PACK_ERR						13911	//充值冲正打包错
#define REVERSE_DEP_TT_ERR						13912	//充值冲正数据库错  
#define REVERSE_DEP_QRY_OPTID_ERR					13913	//充值冲正查询原业务记录失败
#define REVERSE_DEP_QRY_DATA_ERR					13914	//充值冲正查询原业务记录数据失败
#define REVERSE_DEP_BALANCE_CHANGE_ERR					13915	//充值冲正余额来源金额已发生变化
#define COMPARE_PASSWORD_ERR						13916	//支付密码错
#define QUERY_SEQ_ERR							13917	//查询序列错
#define INSERT_PAYMENT_ERR						13918	//记录业务操作表错
#define QRY_ACCT_BALANCE_ACC_NBR_ERR					13919	//按号码查询余额账本错
#define QRY_ACCT_BALANCE_ID_ERR						13920	//按账本序列查错
#define INSERT_BALANCE_PAYOUT_ERR					13921	//记录余额支出表错
#define INSERT_BALANCE_SOURCE_PAYOUT_ERR				13922	//记录来源支出关系表错
#define UPDATE_ACCT_BALANCE_ERR						13923	//更新余额账本表错
#define UPDATE_BALANCE_SOURCE_ERR					13924	//更新余额来源错
#define QRY_BALANCE_PAYOUT_ERR						13925	//查询余额支出数据错
#define QRY_BALANCE_SOURCE_ERR						13926	//查询余额来源出错
#define UPDATE_BALANCE_PAYOUT_ERR					13927	//更新余额支出错
#define UPDATE_BALANCE_SOURCE_PAYOUT_ERR				13928	//更新来源支出关系错
#define QRY_SERV_ACCT_SEQ_ERR						13929	//查询用户信息序列错
#define INSERT_SERV_ERR							13930	//记录用户信息表错
#define INSERT_ACCT_ERR							13931	//记录账户信息出错
#define INSERT_SERV_ACCT_ERR						13932	//记录用户账户对应关系错
#define INSERT_ACCT_BALANCE_ERR						13933	//记录余额账本错
#define INSERT_BALANCE_SOURCE_ERR					13934	//记录余额来源错
#define QRY_ACCT_BALANCE_SIMPLE_ERR					13935	//查询余额账本信息错
#define INSERT_USER_INFO_ADD_ERR					13936	//记录用户信息工单错
#define INSERT_ATT_USER_INFO_ERR					13937	//记录用户支付密码错
#define INSERT_APP_USER_INFO_ERR					13938	//记录用户所属区号错
#define UPDATE_PAYMENT_ERR						13939	//更新业务操作错
#define QRY_PAYMENT_ERR							13940	//查询业务操作错


/*****************************************************
 *	 集团abm资料同步错误码
 *
 *        （14000~14007）
 *
 * **************************************************/
 #define INFOSYN_TT_ERR			14000	//同步操作数据库错
 #define QRY_USER_INFO_ADD_ERR			14001	//查询用户信息增量表错
 #define QRY_SESSION_ID_ERR			14002	//查询开户用流水号错
 #define UPDATE_USER_INFO_ADD_SYNSTATE_ERR			14003	//更新用户信息增量表更新状态字段错
 #define INSERT_SMS_SEND_ERR			14004	//记录短信工单表错
 #define INFO_SYN_UNPACK_ERR			14005	//资料同步解包错
 #define INFO_SYN_PACK_ERR				14006	//资料同步打包错
 #define INFO_SYN_ERROR_ID_ERR		14007	//用户信息增量表返回结果ID错误码记录错
 
 
 /*****************************************************
 *	 集团abm自服务功能返回错误码
 *
 *        （14000~14007）
 *
 * **************************************************/
 #define 	PASSWD_AUTHEN_ERR					15001	// 支付密码鉴权失败
 #define 	PASSWD_FLAG_TYPE_ERR				15002	// 密码操作类型错误
 #define 	PAY_PASSWD_UPDATE_ERR				15003	// 支付密码更新失败
 #define 	PAY_PASSWD_DELETE_ERR				15004	// 支付密码删除失败
 #define 	PAY_PASSWD_INSERT_ERR				15005	// 密码操作插入错误
 #define 	PAY_PASSWD_SELECT_ERR				15006	// 密码操作查询错误
 #define 	PAY_PASSWD_AUTHEN_ERR				15007	// 密码操作鉴权错误
 #define 	PAY_RULE_NULL						15008	// 无划拨规则
 #define 	PAY_RULE_ERR						15009	// 划拨规则查询失败
 #define 	TRANSFERRULES_INSERT_ERR			15010	// 划拨支付规则插入失败