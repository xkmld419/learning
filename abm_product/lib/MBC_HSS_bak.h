#ifndef __MBC_HSS_H__
#define __MBC_HSS_H__
//#
//# 基本编码规则 2位目录编号+3位模块编号+4位信息编号
//#AVP编码规则为 2位目录编号+7位信息编号

#define	MBC_HSS_LIB	0
//@@	0	成功		成功
#define MBC_MEM_MALLOC_FAIL						1
//@@	1	内存分配失败		请检查是否有足够的内存
#define MBC_MEM_SPACE_SMALL						2
//@@	2	内存空间不足		请检查是否有足够的内存
#define MBC_MEM_INTERFACE_ERROR					3
//@@	3	内存查表接口错误	请检查是否有足够的内存
#define MBC_MEM_LINK_FAIL							4
//@@	4	共享内存连接失败	请检查是否有足够的内存
#define MBC_THREAD_CREAT_FAIL						5
//@@	5	派生进程或线程失败	请检查是否有足够的内存
#define MBC_SEMP_OPERA_FAIL						6
//@@	6	信号处理失败		请检查是否有足够的内存
#define MBC_LOCK_GET_FAIL							7
//@@	7	获取锁资源失败		请检查是否有足够的内存
#define MBC_SOCKET_LISTEN_FAIL					8
//@@	8	Socket--监听出错	请检查Socket的连接是否正常
#define MBC_SOCKET_LINK_FAIL						9
//@@	9	连接远端Socket错	请检查端口是否被占用
#define MBC_DISC_SPACE_SMALL						10
//@@	10	磁盘存储空间不足	请整理磁盘空间，并检查是否有程序文件占用大量磁盘空间
#define MBC_CPU_OVERLOAD							11
//@@	11	CPU超负荷			请检查是否系统资源是否被进程占用
#define MBC_DB_LINK_FAIL							12
//@@	12	连接数据库失败		请检查用户名密码是否正确
#define MBC_DB_TABLESPACE_SMALL					13
//@@	13	表空间不足			请检查是否有足够的表空间
#define MBC_ENV_NOT_SET							14
//@@	14	环境变量未设置		请设置环境变量
#define MBC_SYS_BUSY								15
//@@	15	系统繁忙	      请检查主机资源

#define MBC_FILE_OPEN_FAIL						101
//@@	101	打开文件失败		请检查文件是否存在
#define MBC_FILE_WRITE_FAIL						102
//@@	102	写文件失败			请检查文件是否存在
#define MBC_DIR_OPEN_FAIL							103
//@@	103	打开目录出错		请检查目录是否存在，路径是否正确
#define MBC_FILE_RENAME_FAIL						104
//@@	104	文件改名出错		请检查文件是否存在，是否被占用
#define MBC_FILE_SEQ_GET_FAIL						105
//@@	105	序列获取失败		请检查序列是否存在，是否被占用
#define MBC_FILE_NOT_EXIST						106
//@@	106	指定文件不存在		请检查文件名，访问目录是否正确
#define MBC_FILE_COPY_FAIL						107
//@@	107	拷贝文件失败		请检查文件名，访问目录是否正确
#define MBC_FILE_MOVE_FAIL						108
//@@	108	移动文件失败		请检查文件名，访问目录是否正确


#define MBC_MEM_UPDATE_FAIL						116
//@@	116	共享内存更新失败	请检查是否有足够的内存
#define MBC_MEM_IMPORT_FAIL						117
//@@	117	上载异常			请检查是否有足够的内存
#define MBC_MEM_EXPORT_FAIL						118
//@@	118	内存数据导出异常	请检查是否有足够的空间
#define MBC_MEM_CREATE_FAIL						119
//@@	119	创建共享内存失败	请检查是否有足够的内存
#define MBC_MEM_DELETE_FAIL						120
//@@	120	删除共享内存失败	请检查共享内存地址是否正确
#define MBC_MEM_DESTROY_FAIL						121
//@@	121	销毁共享内存失败	请检查共享内存是否正被使用
#define MBC_SEMP_CREATE_FAIL						122
//@@	122	创建信号量失败		请检查信号量是否已被使用


#define MBC_DB_SERV_PWD_ERROR						141
//@@	141	用户名密码错误		请检查用户名及密码
#define MBC_DB_LINK_CLOSE_FAIL					142
//@@	142	关闭数据库连接失败	请检查数据库连接是否正常
#define MBC_DB_TABLE_NOT_EXIT						143
//@@	143	表名不存在			请检查表名称
#define MBC_DB_ROLLBACK_FAIL						144
//@@	144	数据库事务回滚错误	请检查数据库事务的完整
#define MBC_DB_EXECUTE_FAIL						145
//@@	145	发生sql执行异常		请检查SQL语句是否正确

#define MBC_NET_PROTOCOL_PACKAGE_FAIL				156
//@@	156	协议封装失败					请检查封装协议
#define MBC_NET_PROTOCOL_PARSE_FAIL				157
//@@	157	协议解析失败	        请检查解析协议


#define MBC_PARAM_LOAD_ERROR				165
//@@	165	参数加载失败					检查参数文件
#define MBC_PARAM_FILE_FORMAT_ERROR				166
//@@	166	配置文件格式错误					检查配置文件格式
#define MBC_PARAM_THREAD_LOST_ERROR				167
//@@	167	应用进程参数不正确（缺少必选项）	请添加必选项参数
#define MBC_PARAM_THREAD_NONSUPP_ERROR			168
//@@	168	应用进程参数不正确（不支持的选项）	请检查选项参数
#define MBC_PARAM_THREAD_VALUE_ERROR				169
//@@	169	应用进程参数不正确（错误的取值）	请修改参数值
#define MBC_PARAM_UPDATE_FAIL						170
//@@	170	参数更新失败	            检查参数文件状态，检查参数格式。
#define MBC_PARAM_FILE_ERROR						171
//@@	171	参数文件不存在						检查参数文件路径
#define MBC_PARAM_LOST_ERROR						172
//@@	172	参数配置信息缺失					检查参数配置信息完整性
#define MBC_PARAM_VALUE_NULL_ERROR				173
//@@	173	非空检查（字段不能为空)	  检查字段，是否存在非空字段为空值
#define MBC_PARAM_LEN_MIN_ERROR					174
//@@	174	长度检查（小于最小长度	  检查字段，是否存在字段长度小于设定的最小长度
#define MBC_PARAM_LEN_MAX_ERROR					175
//@@	175	长度检查（大于最大长度)	  检查字段，是否存在字段长度大于设定的最大长度
#define MBC_PARAM_LEN_FIXED_ERROR					176
//@@	176	长度检查（非固定长度)	    检查参数配置信息完整性
#define MBC_PARAM_VALUE_ENUM_ERROR				177
//@@	177	枚举检查(枚举值不正确)	  检查参数值是否在参数的枚举范围内
#define MBC_PARAM_VALUE_MIN_ERROR					178
//@@	178	数值范围检查（小于最小值)	检查参数数值范围是否小于设定的最小值
#define MBC_PARAM_VALUE_MAX_ERROR					179
//@@	179	数值范围检查（大于最大值)	检查参数数值范围是否大于设定的最大值
#define MBC_PARAM_ERROR							180
//@@	180	参数错误	                检查不在已定义的参数格式错误中，但属于参数类的错误

#define MBC_INFO_CUST_GET_FAIL                  181
//@@	181	取客户信息失败	          检查客户信息是否存在
#define MBC_INFO_ACCT_GET_FAIL                  182
//@@	182	取账户信息失败	          检查账户信息是否存在
#define MBC_INFO_SERV_GET_FAIL                  183
//@@	183	取用户信息失败	          检查用户信息是否存在

#define MBC_SYSTEM_START_ERROR         195
//@@	195	系统启动失败					检查系统状态
#define MBC_HREAD_EXIST_ERROR						196
//@@	196	应用进程已经存在					检查该应用进程是否已经存在
#define MBC_HREAD_ERROR							197
//@@	197	预处理进程异常						检查预处理相关配置
#define MBC_HREAD_STATUS_ERROR					198
//@@	198	进程状态非法						请检查进程是否正常运行
#define MBC_HREAD_EXIT_ERROR						199
//@@	199	应用程序收到退出信号异常退出		检查该应用进程状态，检查系统状态

#define MBC_SRVC_SEQ_CONTINUE_FAIL				213
//@@	213	序号连续性校验失败				检查序号的连续性
#define MBC_SRVC_RECEIVE_FAIL						232
//@@	232	系统接收异常数据					检查系统状态，检查系统数据传输
#define MBC_SRVC_SEQ_REPEAT_FAIL					240
//@@	240	流水号重复				        检查是否存在重复流水号


#define MBC_PARAM_DATE_FORMAT_ERROR				246
//@@	246	时间检查（时间格式不正确）									检查时间格式是否正确
#define MBC_PARAM_DATE_MIN_ERROR					247
//@@	247	时间检查（小于最小时间）									检查时间是否小于最小时间
#define MBC_PARAM_DATE_MAX_ERROR					248
//@@	248	时间检查（大于最大时间）										检查时间是否大于最大时间
#define MBC_PARAM_STRING_ERROR					249
//@@	249	字符串检查（出现非法字符）								检查字符串里是否出现非法字符
#define MBC_PARAM_IP_FROMAT_ERROR					250
//@@	250	IP地址检查（IP地址格式不正确）							检查IP地址的格式
#define MBC_PARAM_ACCNBR_FROMAT_ERROR				251
//@@	251	号码格式不符合要求	                        检查号码格式是否正确
#define MBC_PARAM_ACCNBR_FROMAT_ERROR				252
//@@	252	省份编码检查（省份编码不正确）	            检查省份编码是否正确
#define MBC_PARAM_LOCAL_FROMAT_ERROR				253
//@@	253	本地网编码检查（本地网编码不正确）					检查本地网编码是否正确
#define MBC_PARAM_POST_FROMAT_ERROR				254
//@@	254	邮政编码检查（邮政不符合要求）							检查邮政编码是否符合要求
#define MBC_PARAM_NUMBER_FROMAT_ERROR				255
//@@	255	数据类型检查（数字型字段出现字符）					检查数字型字段是否出现字符
#define MBC_PARAM_CHARS_ERROR						256
//@@	256	数据类型检查（字符型字段出现非法字符）			检查字符型字段是否出现非法字符


#define MBC_ARCHIVING_PROCESS_ERROR				261
//@@	261	数据归档失败	      检查数据归档配置，执行权限，磁盘空间
#define MBC_OPERATE_ERROR							262
//@@	262	操作失败	          检查无法归入已有错误的操作失败


#define MBC_NP_NOT_EXIST							301
//@@	301	携号转网数据不存在	                               检查是否存在携号转网数据
#define MBC_NP_EXP								302
//@@	302	携号转网数据已失效	                               检查携号转网数据状态，是否已失效
#define MBC_NP_INVALID							303
//@@	303	携号转网数据未生效	                               检查携号转网数据状态，是否未生效
#define MBC_NP_SERV_REMOVED						304
//@@	304	用户已拆机        	                               检查携号转网用户状态，是否已拆机


#define MBC_IBRST_CARRIER_NOT_EXIST				311	
//@@	311	C网国漫被访运营商不存在            	               检查C网国际漫游被访运营商是否存在
#define MBC_IBRST_CARRIER_EXP						312
//@@	312	C网国漫被访运营商已失效            	               检查C网国际漫游被访运营商资料状态，是否已失效
#define MBC_IBRST_CARRIER_VALID					313
//@@	313	C网国漫被访运营商尚未生效          	               检查C网国际漫游被访运营商资料状态，是否未生效
#define MBC_IBRST_COUNTRY_NOT_EXIST				314
//@@	314	C网国漫被访运营商所在国家不存在    	               检查C网国际漫游被访运营商所在国家是否不存在
#define MBC_IBRST_REGION_NOT_EXIST				315
//@@	315	C网国漫被访运营商所属资费区域不存在	               检查C网国际漫游被访运营商所属资费区域是否不存在



#define MBC_CTOG_CARRIER_NOT_EXIST				321
//@@	321	不同制式国漫被访运营商不存在            	         检查不同制式国际漫游被访运营商是否存在
#define MBC_CTOG_CARRIER_EXP						322
//@@	322	不同制式国漫被访运营商已失效            	         检查不同制式国际漫游被访运营商资料状态，是否已失效
#define MBC_CTOG_CARRIER_INVALID					323
//@@	323	不同制式国漫被访运营商尚未生效          	         检查不同制式国际漫游被访运营商资料状态，是否未生效
#define MBC_CTOG_COUNTRY_NOT_EXIST				324
//@@	324	不同制式国漫被访运营商所在国家不存在    	         检查不同制式国际漫游检查被访运营商所在国家是否不存在
#define MBC_CTOG_NOT_EXIST						325
//@@	325	不同制式国漫CtoG代理运营商不存在        	         检查不同制式国际漫游CtoG代理运营商是否不存在
#define MBC_CTOG_REGION_NOT_EXIST					326
//@@	326	不同制式国漫被访运营商所属资费区域不存在	         检查不同制式国际漫游检查被访运营商所属资费区域是否不存在


#define MBC_MIN_NOT_EXIST							331
//@@	331	MIN码不存在                      	                 检查MIN码是否存在
#define MBC_MIN_EXP								332
//@@	332	MIN码已失效                      	                 检查MIN码是否已失效
#define MBC_MIN_INVALID							333
//@@	333	MIN码尚未生效                    	                 检查MIN码是否未生效
#define MBC_MIN_COUNTRY_NOT_EXIST					334
//@@	334	MIN码归属国家代码异常（不存在）  	                 检查MIN码归属国家代码是否不存在
#define MBC_MIN_COUNTRY_EXP						335
//@@	335	MIN码归属国家代码异常（失效）    	                 检查MIN码归属国家代码是否已失效
#define MBC_MIN_COUNTRY_MUL						336
//@@	336	MIN码归属国家代码异常（多个）    	                 检查是否存在多个MIN码归属国家代码
#define MBC_MIN_CARRIER_NOT_EXIT					337
//@@	337	MIN码归属运营商代码异常（不存在）	                 检查MIN码归属运营商代码是否不存在
#define MBC_MIN_CARRIER_EXP						338
//@@	338	MMIN码归属运营商异常（失效）     	                 检查MIN码归属运营商代码是否已失效
#define MBC_MIN_CARRIER_MUL						339
//@@	339	MIN码归属运营商代码异常（多个）  	                 检查是否存在多个MIN码归属运营商代码



#define	 MBC_HSS_IMSI_CODE	 346
//@@	346	IMSI码不存在                  	                   检查IMSI码是否存在
#define MBC_IMSI_EXP								347
//@@	347	IMSI码已失效                  	                   检查IMSI码是否已失效
#define MBC_IMSI_INVALID							348
//@@	348	IMSI码尚未生效                	                   检查IMSI码是否未生效
#define MBC_IMSI_CTOG_ERROR						349
//@@	349	IMSI对应CtoG代理运营商代码异常	                   检查IMSI与CtoG代理运营商代码的对应关系
#define MBC_IMSI_LOCAL_MUL						350
//@@	350	IMSI对应多个本地网            	                   检查是否存在IMSI对应多个本地网的情况



#define	 MBC_COUNTRY_NOT_EXIST	 356
//@@	356	国家信息不存在  	                                 检查国家信息是否存在
#define	 MBC_COUNTRY_EXP	 357
//@@	357	国家信息已失效  	                                 检查国家信息是否已失效
#define MBC_COUNTRY_INVALID						358
//@@	358	国家信息尚未生效	                                 检查国家信息是否未生效
#define MBC_COUNTRY_MUL							359
//@@	359	国家代码重复    	                                 检查是否存在重复的国家代码
#define MBC_COUNTRY_ERROR							360
//@@	360	国家名称异常    	                                 检查是否存在异常国家名称


#define MBC_HCODE_NOT_EXIST						366
//@@	366	H码不存在                  	                       检查H码是否存在
#define MBC_HCODE_EXP								367
//@@	367	H码已失效                  	                       检查H码是否已失效
#define MBC_HCODE_INVALID							368
//@@	368	H码尚未生效                	                       检查H码是否未生效
#define MBC_HCODE_LOCAL_MUL						369
//@@	369	H码归属本地网异常（多个）  	                       检查是否存在H码归属多个本地网
#define MBC_HCODE_CARRIER_MUL						370
//@@	370	H码归属运营商异常（多个）  	                       检查是否存在H码归属多个运营商
#define MBC_HCODE_CARRIER_NOT_EXIT				371
//@@	371	H码归属运营商异常（不存在）	                       检查是否存在H码无归属运营商



#define MBC_SP_NOT_EXIST							376
//@@	376	SP代码不存在  	                                   检查SP代码是否存在
#define MBC_SP_EXP								377
//@@	377	SP代码已失效  	                                   检查SP代码是否已失效
#define MBC_SP_INVALID							378
//@@	378	SP代码尚未生效	                                   检查SP代码是否未生效


#define MBC_BORDER_ROAM_PROVINCE_MUL				386
//@@	386	边漫话单归属多个省	                               检查边漫话单，边漫归属配置



#define MBC_HSS_UPDATE_NOT_EXIST					12000
//@@	12000	更新记录不存在        	                         检查更新的记录是否存在
#define MBC_HSS_LAST_CONTINUE_NOT_EXIT			12001
//@@	12001	找不到上一个连续消息包	                         检查传递消息包的连续性
#define MBC_HSS_AUTH_PAYPWD_FAIL					12002
//@@	12002	支付密码鉴权失败      	                         检查支付密码鉴权
#define MBC_HSS_CUSTID_ERROR						12003
//@@	12003	大客户标识不符合要求  	                         检查产品编码是否符合规范
#define MBC_HSS_PRODUCTID_ERROR					12004
//@@	12004	产品编码不符合要求    	                         检查产品编码是否符合规范



#define MBC_HSS_SERV_CUST_ERROR					12100
//@@	12100	有用户资料没有客户资料	                         检查用户的客户资料是否存在
#define MBC_HSS_SERV_ACCT_ERROR					12101
//@@	12101	有用户资料没有帐户资料	                         检查用户的帐户资料是否存在
#define MBC_HSS_ACCT_CUST_ERROR					12102
//@@	12102	有帐户资料没有客户资料	                         检查帐户的客户资料是否存在
#define MBC_HSS_REFCUST_NOT_EXIT					12103
//@@	12103	依赖的大客户不存在    	                         检查依赖的大客户资料是否存在
#define MBC_HSS_REFSERV_NOT_EXIT					12104
//@@	12104	依赖的用户信息不存在  	                         检查依赖的用户信息是否存在
#define MBC_HSS_REFACCT_NOT_EXIT					12105
//@@	12105	依赖的账户信息不存在  	                         检查依赖的账户信息是否存在
#define MBC_HSS_BANK_NOT_EXIST					12106
//@@	12106	银行托收无银行帐号    	                         检查银行托收的银行帐号
#define MBC_HSS_SERV_MAX_ERROR					12107
//@@	12107	超过最大用户数        	                         检查用户数是否超过设定的最大用户数



#define MBC_HSS_OFFERINS_DATE_NOT_CONST_ERROR		12200
//@@	12200	销售品实例与销售品包含对象实例生失效时间不一致	 检查销售品实例与销售品包含对象实例的生失效时间是否一致
#define MBC_HSS_OFFER_EXP_MIN_EFF_ERROR			12201
//@@	12201	销售品失效日期小于生效日期                    	 检查销售品配置的失效日期是否小于生效日期
#define MBC_HSS_OFFER_MEMBER_NUM_ERROR			12202
//@@	12202	销售品包含对象成员数量异常（产品缺失或多）    	 检查销售品包含对象成员数量
#define MBC_HSS_OFFER_MEMBER_TYPE_ERROR			12203
//@@	12203	销售品包含对象成员类型异常                    	 检查销售品包含对象成员类型
#define MBC_HSS_OFFERINS_ATTR_LOST_ERROR			12204
//@@	12204	缺少必填的销售品属性实例                      	 检查是否缺少必填的销售品属性实例
#define MBC_HSS_OFFERINS_PARAM_LOST_ERROR			12205
//@@	12205	缺少必填的销售品参数实例                      	 检查是否缺少必填的销售品参数实例
#define MBC_HSS_OFFERINS_NUM_MAX_ERROR			12206
//@@	12206	超过最大销售品实例数                          	 检查销售品实例数是否超过设定的最大销售品实例数


#define MBC_HSS_GROUPMEMBER_NODATA_ERROR			12250
//@@	12250	电信网内群成员资料在主产品实例表无数据	         检查电信网内群成员资料是否在主产品实例表无数据


#define MBC_HSS_CUST_CREATE_MAX_ERROR			    12260
//@@	12260	开户失败	                                       检查用户资料完整性


#define MBC_HSS_OFFER_CATALOG_APPROVE_FAIL		12270
//@@	12270	销售品目录审批失败	                             检查销售品目录规范



/*    操作码    */
#define  MBC_ACTION_SYSTEM_START               100001
//@@	100001	启动系统	      系统启动成功
#define  MBC_ACTION_SYSTEM_END                 100002
//@@	100002	退出系统	      系统退出成功
#define  MBC_ACTION_PROCESS_START              100003
//@@	100003	启动程序	      程序启动成功
#define  MBC_ACTION_PROCESS_END                100004
//@@	100004	退出程序	      程序退出成功
#define  MBC_ACTION_MODIFY_PARAM               100005
//@@	100005	更新参数	      参数更新成功
#define  MBC_ACTION_LOAD_PARAM                 100006
//@@	100006	加载参数	      参数加载成功
#define  MBC_ACTION_MODIFY_DB                  100007
//@@	100007	操作数据库	    数据库操作成功
#define  MBC_ACTION_CHECK_POINT                100008
//@@	100008	启动CheciPoint	CheciPoint成功
#define  MBC_ACTION_DEBUG                      200000
//@@	200000	调试操作	      调整操作成功
#endif
