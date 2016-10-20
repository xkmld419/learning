/*VER: 7*/ 
#ifndef __MBC_H__
#define __MBC_H__

#define MBC_FileNameRule  1000
#define MBC_HeadTailRule  1001
#define MBC_AccuCfg       1002
#define MBC_AccuRule      1003
//#include "Log.h"
#include "MBC_28.h"
#define MBC				999990000
#define MBC_MEM_IMPORT_FAIL                                             117
//@@    117     上载异常                        请检查是否有足够的内存
#define MBC_PARAM_THREAD_LOST_ERROR                             167
//@@    167     参数不存在      在核心参数配置新增此参数
#define MBC_PARAM_THREAD_NONSUPP_ERROR                  168
//@@    168     应用进程参数不正确（不支持的选项）      请检查选项参数
#define MBC_MEM_EXPORT_FAIL                                             118
//@@    118     内存数据导出异常        请检查是否有足够的空间
#define MBC_PARAM_THREAD_VALUE_ERROR                            169
//@@    169     应用进程参数不正确（错误的取值）        请修改参数值
#define MBC_DB_SERV_PWD_ERROR                                           141
//@@    141     用户名密码错误          请检查用户名及密码
#define MBC_DB_LINK_CLOSE_FAIL                                  142
//@@    142     关闭数据库连接失败      请检查数据库连接是否正常
#define MBC_DB_TABLE_NOT_EXIT                                           143
//@@    143     表名不存在                      请检查表名称
#define MBC_DB_ROLLBACK_FAIL                                            144
//@@    144     数据库事务回滚错误      请检查数据库事务的完整
#define MBC_DB_EXECUTE_FAIL                                             145
//@@    145     发生sql执行异常         请检查SQL语句是否正确
//#
//# 基本编码规则: 2位目录编号+3位模块编号+4位信息编号
//#
#define MBC_MEM_MALLOC_FAIL                                             1
//@@    1       内存分配失败            请检查是否有足够的内存
#define MBC_MEM_SPACE_SMALL                                             2
//@@    2       内存空间不足            请检查是否有足够的内存
#define MBC_MEM_INTERFACE_ERROR                                 3
//@@    3       内存查表接口错误        请检查是否有足够的内存
#define MBC_MEM_LINK_FAIL                                               4
//@@    4       共享内存连接失败        请检查是否有足够的内存
#define MBC_THREAD_CREAT_FAIL                                   5
//@@    5       派生进程或线程失败      请检查是否有足够的内存
#define MBC_SEMP_OPERA_FAIL                                             6
//@@    6       信号处理失败            请检查是否有足够的内存
#define MBC_LOCK_GET_FAIL                                                       7
//@@    7       获取锁资源失败          请检查是否有足够的内存
#define MBC_SOCKET_LISTEN_FAIL                                  8
//@@    8       Socket--监听出错        请检查Socket的连接是否正常
#define MBC_SOCKET_LINK_FAIL                                            9
//@@    9       连接远端Socket错        请检查端口是否被占用
#define MBC_DISC_SPACE_SMALL                                            10
//@@    10      磁盘存储空间不足        请整理磁盘空间，并检查是否有程序文件占用大量磁盘空间
#define MBC_CPU_OVERLOAD                                                        11
//@@    11      CPU超负荷                       请检查是否系统资源是否被进程占用
#define MBC_DB_LINK_FAIL                                                        12
//@@    12      连接数据库失败          请检查用户名密码是否正确
#define MBC_DB_TABLESPACE_SMALL                                 13
//@@    13      表空间不足                      请检查是否有足够的表空间
#define MBC_ENV_NOT_SET                                                 14
//@@    14      环境变量未设置          请设置环境变量
#define MBC_SYS_BUSY                                                            15
//@@    15      系统繁忙              请检查主机资源
#define MBC_MAX_DCC_REQUEST_CNT                                                 16
//@@    16      DCC消息请求达到最大值                   请检查是否系统进程运行是否正常
#define MBC_SHM_ERRNO_DATA_NOT_EXIST                    50
//@@    50      共享内存数据区不存在    请检查共享内存数据区是否创建
#define MBC_SHM_ERRNO_INDEX_NOT_EXIST                   51
//@@    51      共享内存索引区不存在    请检查共享内存索引是否创建
#define MBC_SHM_ERRNO_DATA_NOTCONNECT                   52
//@@    52      未连接共享内存数据区    请重新连接共享内存数据区
#define MBC_SHM_ERRNO_INDEX_NOTCONNECT                  53
//@@    53      未连接共享内存索引区    请重新连接共享内存索引区
#define MBC_SHM_ERRNO_ADD_DATA_EXIST                    54
//@@    54      共享内存数据区已经存在  无
#define MBC_SHM_ERRNO_UPDATEINFO_NOTEXIST               55
//@@    55      共享内存更新的数据不存在                请改用插入数据
#define MBC_SHM_ERRNO_DEL_DATA_NOTEXIST                 56
//@@    56      删除的数据共享内存不存在        无
#define MBC_SHM_ERRNO_DEL_INDEX_FAILED                  57
//@@    57      删除数据的索引失败      无
#define MBC_SHM_ADD_DATA_EXIST                                  58
//@@    58      添加的数据已经存在      无
#define MBC_SHM_QUERY_NO_DATE_FOUND                             59
//@@    59      查询数据不存在  无
#define MBC_SHM_ULOAD_DATA_NOT_EXIST                    60
//@@    60      卸载的数据区不存在      无
#define MBC_SHM_LOAD_NO_DATA                                    61
//@@    61      加载的表数据不存在      无
#define MBC_SHM_OPT_NOT_SUPPORT                                 62
//@@    62      共享内存不支持此种操作  无
#define MBC_SHM_NOT_CONNECT_DATA                                63
//@@    63      未连接数据区    重新连接共享内存
#define MBC_CFG_GETPARAM_FAILED                                 64
//@@    64      获取核心参数失败        查看配置文件是否配置
#define MBC_SHM_INIT_FAILED                                             65
//@@    65      初始化共享内存失败      查看错误日志信息
//# MBC_Example
#define MBC_Example			990010000
//@@ 990010001	Example测试Exception1 	----
//@@ 990010002	Example测试Exception2   ----

//#
//# MBC_LIB
#define MBC_APP_AccuStd								101
#define MBC_APP_adjustfee             102
#define MBC_APP_apportion             103
#define MBC_APP_archive               104
#define MBC_APP_asn                   105
#define MBC_APP_backup                106
#define MBC_APP_billfee               107
#define MBC_APP_billstat              108
#define MBC_APP_CfgExp                109
#define MBC_APP_checkaudit            110
#define MBC_APP_checkback             111
#define MBC_APP_checkdup              112
#define MBC_APP_CheckFile             113
#define MBC_APP_CheckRecord           114
#define MBC_APP_CheckRoamDup          115
#define MBC_APP_CmdParser             116
#define MBC_APP_Collect               117
#define MBC_APP_CommExp               118
#define MBC_APP_count                 119
#define MBC_APP_CreateErrFile         120
#define MBC_APP_Credit                121
#define MBC_APP_creditcal             122
#define MBC_APP_ctlacctitem           123
#define MBC_APP_ctlaccu               124
#define MBC_APP_ctlinfopoint          125
#define MBC_APP_ctlofferaggr          126
#define MBC_APP_DailyAcctItem         127
#define MBC_APP_DataEventDivMain      128
#define MBC_APP_DataEventMemAdm       129
#define MBC_APP_DataExport            130
#define MBC_APP_Datafilterstop        131
#define MBC_APP_Datastopbuf           132
#define MBC_APP_dccCheckRecord        133
#define MBC_APP_dccPack               134
#define MBC_APP_dccTran               135
#define MBC_APP_dccUnPack             136
#define MBC_APP_disctmaker            137
#define MBC_APP_distribute            138
#define MBC_APP_E8UserMain            139
#define MBC_APP_EdgeStationInfo       140
#define MBC_APP_errfileInDb           141
#define MBC_APP_EventGet              142
#define MBC_APP_eventmaker            143
#define MBC_APP_eventoutput           144
#define MBC_APP_exportuserinfo        145
#define MBC_APP_fileinstore           146
#define MBC_APP_fileinstore_stat      147
#define MBC_APP_filterstop            148
#define MBC_APP_format                149
#define MBC_APP_gather                150
#define MBC_APP_gendmfile             151
#define MBC_APP_hbctl                 152
#define MBC_APP_highFee               153
#define MBC_APP_incept                154
#define MBC_APP_Init                  155
#define MBC_APP_ivpn                  156
#define MBC_APP_load                  157
#define MBC_APP_loadaggrctl           158
#define MBC_APP_LoadBalance           159
#define MBC_APP_LoadIvpnInfo          160
#define MBC_APP_loadparam             161
#define MBC_APP_loadparaminfo         162
#define MBC_APP_loadstatlog           163
#define MBC_APP_loaduserctl           164
#define MBC_APP_loaduserinfo          165
#define MBC_APP_loadvpninfo           166
#define MBC_APP_loggroup              167
#define MBC_APP_mergeevent            168
#define MBC_APP_modifyacct            169
#define MBC_APP_msgQueTest            170
#define MBC_APP_msgSend               171
#define MBC_APP_offerremind           172
#define MBC_APP_offersimu             173
#define MBC_APP_output                174
#define MBC_APP_overtime              175
#define MBC_APP_owe                   176
#define MBC_APP_Points                177
#define MBC_APP_prep                  178
#define MBC_APP_pricing               179
#define MBC_APP_procerrfile           180
#define MBC_APP_putinfile             181
#define MBC_APP_rebillex              182
#define MBC_APP_recvHbCharge          183
#define MBC_APP_RecycleEvent          184
#define MBC_APP_redoevents            185
#define MBC_APP_remindbuf             186
#define MBC_APP_remindmaker           187
#define MBC_APP_remindtemplate        188
#define MBC_APP_rentex                189
#define MBC_APP_RoamCheckDup          190
#define MBC_APP_RTDataStop            191
#define MBC_APP_rt_make_stop          192
#define MBC_APP_schedule              193
#define MBC_APP_Score                 194
#define MBC_APP_sendFile              195
#define MBC_APP_setex                 196
#define MBC_APP_show                  197
#define MBC_APP_sockComm              198
#define MBC_APP_sp_bill_percent       199
#define MBC_APP_srSimulator           200
#define MBC_APP_stat                  201
#define MBC_APP_stattree              202
#define MBC_APP_stopbuf               203
#define MBC_APP_tdtrans               204
#define MBC_APP_to_tdtrans            205
#define MBC_APP_trans                 206
#define MBC_APP_TransTermInfo         207
#define MBC_APP_trustbank             208
#define MBC_APP_tygroup               209
#define MBC_APP_UpdateE8Info          210
#define MBC_APP_updatenpinfo          211
#define MBC_APP_updateuserinfo        212
#define MBC_APP_updatevpninfo         213
#define MBC_APP_UserInterface         214
#define MBC_APP_virement              215
#define MBC_APP_vpn                   216
#define MBC_APP_writefiledb           217
#define MBC_APP_writelocal            218
#define MBC_APP_memmgr                219
#define MBC_APP_loggrum               220
#define MBC_APP_seekuser              221
//add by jx 20100917
#define MBC_APP_infomonitor           222
//add by xn 20100927
#define MBC_APP_scheduleserver        224
#define MBC_APP_statserver            225
#define	MBC_APP_testinfo	      226

// add by zhangch 20101001
#define MBC_APP_dccSrv                		226
#define MBC_APP_dccSend                   227
#define MBC_APP_tuxClient                 228
#define MBC_APP_contextBill               229
#define MBC_APP_CheckPointCharge          230
#define MBC_APP_ArrearageCheck            231
#define MBC_APP_QueryBalance              232
#define MBC_APP_WriteOff				          233
#define MBC_APP_ReversePaymen             234
#define MBC_APP_QueryProdHSS              235
#define MBC_APP_QueryVisitCountry			    236
#define MBC_APP_BalanceTrans      				237
#define MBC_APP_RealtimeFeeQuery      		238
#define MBC_APP_FeeBillQuery          		239
#define MBC_APP_NmpSysParam               240
#define MBC_APP_hblogg										520
#define MBC_APP_hbparam										982
#define MBC_APP_checkpoint								984
#define MBC_APP_hbshm											987
#define MBC_APP_DRQHighUsage		241
#define MBC_APP_IRQHighUsage		242
#define MBC_APP_IFQHighUsage		243
#define	MBC_APP_PointQuery		244
#define MBC_APP_GetUserHssInfo		245
#define MBC_APP_GetUserData		246
#define MBC_APP_GetCustData		247
#define	MBC_APP_GetAcctData		248
#define MBC_APP_GetServData		249
#define MBC_APP_GetServAcctData		250
#define	MBC_APP_GetServStateData		251
#define MBC_APP_GetServProdData		252
#define MBC_APP_HQBillQueryBill		253
#define MBC_APP_HQBillTest		254
#define MBC_APP_UpdUserAndRecord	255
//Command Group      
//----------------------------------------
#define MBC_COMMAND_hbsyscheck				501
#define MBC_COMMAND_hbinstalltest     502
#define MBC_COMMAND_hbswinstall       503
#define MBC_COMMAND_hbswuninstall     504
#define MBC_COMMAND_Hbcheckpoint      505
#define MBC_COMMAND_hbcleantest       506
#define MBC_COMMAND_hbadmin           507
#define MBC_COMMAND_hbparam           508
#define MBC_COMMAND_hbinit            509
#define MBC_COMMAND_hbprocess         510
#define MBC_COMMAND_hbshm             511
#define MBC_COMMAND_hbmonitor         512
#define MBC_COMMAND_hbmode            513
#define MBC_COMMAND_hbstat            514
#define MBC_COMMAND_hbimport          515
#define MBC_COMMAND_hbexport          516
#define MBC_COMMAND_hbaccess          517
#define MBC_COMMAND_hbhelp            518
#define MBC_COMMAND_hbfinderr         519
#define MBC_COMMAND_hblogg            520
//******************************************************
//Class_ID
//******************************************************
#define MBC_CLASS_Fatal										0				//严重错误                      
#define MBC_CLASS_File                    1        //文件类0                       
#define MBC_CLASS_Memory                  2        //内存类1                       
#define MBC_CLASS_Database                3        //数据库类2                     
#define MBC_CLASS_Network                 4        //网络类                        
#define MBC_CLASS_Parameter               5        //参数类                        
#define MBC_CLASS_Information             6        //信息类                        
#define MBC_CLASS_Process                 7        //进程类                        
#define MBC_CLASS_Business                8        //业务类                        
#define MBC_CLASS_LNP                     9        //携号转网                      
#define MBC_CLASS_Carrier                 10        //C网国际漫游运营商代码         
#define MBC_CLASS_DiffStandCarrier        11        //不同制式国际漫游运营商代码文件
#define MBC_CLASS_MIN                     12        //MIN码表                       
#define MBC_CLASS_IMSI                    13        //不同制式IMSI码表              
#define MBC_CLASS_State                   14        //国家信息表                    
#define MBC_CLASS_HCode                   15        //H码表                         
#define MBC_CLASS_SP                      16        //SP码表                        
#define MBC_CLASS_Roaming                 17        //边界漫游数据                  
#define MBC_CLASS_Prepaid                 18        //充值卡充值类                  
#define MBC_CLASS_RemotePayment           19        //异地缴费类                    
#define MBC_CLASS_HighFee                 20        //高额类                        
#define MBC_CLASS_StaticParam             21        //静态参数类                    
#define MBC_CLASS_DataTrans               22        //资料传输类                    
#define MBC_CLASS_HSS                     23        //HSS查询类
//******************************************************
//Error_ID      
//******************************************************
//0001					//内存分配失败                            //检查核心文件中内存配置信息，检查该内存是否已被分配。                                                                                                                      
//0002          //内存空间不足                            //检查核心文件中内存配置信息，检查该系统内存是否足够。                                                                  
//0003          //内存查表接口错误                        //检查内存中数据结构。                                                                                                
//0004          //共享内存连接失败                        //检查共享内存是否存在，检查核心文件中内存的Key。                                                                                                                            
//0005          //派生进程或线程失败                      //检查派生的进程是否存在。检查是否达到系统限制。                                                                                                                             
//0006          //信号处理失败                            //检查是否存在该信号配置，检查该信号是否被锁定。                                                                                                                             
//0007          //获取锁资源失败                          //检查是否存在该锁资源配置，检查该锁资源是否被锁定。                                                                                                                         
//0008          //Socket--监听出错                        //检查监听端口是否已被占用，检查是否达到系统限制。                                                                                                                           
//0009          //连接远端Socket错                        //检查远端监听端口是否已被存在，检查是否达到系统限制。                                                                                                                       
//0010          //磁盘存储空间不足                        //检查系统磁盘空间。                                                                                                                                                         
//0011          //CPU超负荷                               //检查系统是否运行过多的进程，检查CPU超负荷阀值配置。                                                                                                                        
//0012          //连接数据库失败                          //检查数据库配置，检查数据库服务器状态。                                                                                                                                     
//0013          //表空间不足                              //检查数据库表空间配置，检查数据库服务器状态。                                                                                                                               
//0014          //环境变量未设置                          //检查系统环境变量。                                                                                                                                                         
//0015          //内存超负荷															//检查可用
//0016          //销账数据库异常                          //检查数据库连接                                                                                                                                          
      
//0101          //打开文件失败                            //检查文件是否存在，检查文件权限设置、所有人。                                                                                                                     
//0102          //写文件失败                              //检查文件是否存在，检查文件权限设置、所有人。                                                                                                                     
//0103          //打开目录出错                            //                                                                                                                     
//0104          //文件改名出错                            //                                                                                                                     
//0105          //序列获取失败                            //                                                                                                                       
//0106          //指定文件不存在                          //检查文件是否存在，检查文件权限设置、所有人。                                                                                                                        
//0107          //拷贝文件失败                            //检查文件是否存在，检查目的目录是否存在。                                                                                                                           
//0108          //移动文件失败                            //检查文件是否存在，检查文件权限设置、所有人。                                                                                                                       
//0109          //                                                                                                                                                     
//0110          //                                                                                                                                                     
//0111          //                                                                                                                                                     
//0112          //                                                                                                                                                     
//0113          //                                                                                                                                                     
//0114          //                                                                                                                                                     
//0115          //                                                                                                                                                     
      
//0116          //共享内存更新失败                         //检查共享内存是否存在，检查共享内存结构。                                                                                                                     
//0117          //上载异常hbimport命令                  //检查共享内存是否存在，检查共享内存结构。                                                                                                                     
//0118          //内存数据导出异常hbexport                //检查共享内存是否存在，检查共享内存结构。                                                                                                                     
//0119          //创建共享内存失败                         //检查共享内存是否已经存在了。检查内存容量。                                                                                                                      
//0120          //删除共享内存失败                         //检查共享内存是否存在，检查该内存所有者。                                                                                                                      
//0121          //销毁共享内存失败                         //检查共享内存是否存在，检查共享内存结构。                                                                                                                      
//0122          //创建信号量失败                           //检查信号量配置，检查是否达到系统限制。                                                                                                                      
//0123          //共享内存到达阀值                         //                                                                                                                            
//0124          //                                                                                                                                                     
//0125          //                                                                                                                                                     
//0126          //                                                                                                                                                     
//0127          //                                                                                                                                                     
//0128          //                                                                                                                                                     
//0129          //                                                                                                                                                     
//0130          //                                                                                                                                                     
//0131          //消息队列创建失败												//检查消息队列是否有满或者没有权限                                                                                                                                                     
//0132          //                                                                                                                                                     
//0133          //                                                                                                                                                     
//0134          //                                                                                                                                                     
//0135          //                                                                                                                                                     
//0136          //                                                                                                                                                     
//0137          //                                                                                                                                                     
//0138          //                                                                                                                                                     
//0139          //                                                                                                                                                     
//0140          //                                                                                                                                                     
      
//0141          //用户名密码错误                            //检查数据库配置，检查数据库服务器状态。                                                                                                                     
//0142          //关闭数据库连接失败                        //检查数据库服务器状态，检查网络状态。                                                                                                                       
//0143          //表名不存在                                //                                                                                                                       
//0144          //数据库事务回滚错误                        //                                                                                                                     
//0145          //发生sql执行异常                           //检查Sql语句语法，检查数据库连接状态                                                                                                                    
//0146          //话单入数据库失败                          //                                                                                                                     
//0147          //话单成功入库                              //成功                                                                                                                      
//0148          //                                                                                                                                                     
//0149          //                                                                                                                                                     
//0150          //                                                                                                                                                     
//0151          //                                                                                                                                                     
//0152          //                                                                                                                                                     
//0153          //                                                                                                                                                     
//0154          //                                                                                                                                                     
//0155          //                                                                                                                                                     
      
//0156          //协议封装失败，协议名称%s，错误原因%s.      //检查数据库配置，检查话单表结构配置。                                                                                                             
//0157          //协议解析失败，协议名称%s，错误原因%s.      //检查该协议封装相关配置。   
//0158          //tuxedo调用失败，失败原因%s                 //检查tuxdo服务状态。                                                                                                                    
//0159          //系统并发达到最大连接数                     //系统并发达到最大连接数,请稍后再试                                                                                                                                           
//0160          //                                                                                                                                                     
//0161          //                                                                                                                                                     
//0162          //                                                                                                                                                     
//0163          //                                                                                                                                                     
//0164          //                                                                                                                                                     
//0165          //                                                                                                                                                     
      
//0166          //配置文件格式错误                            //检查配置文件格式。                                                                                                                              
//0167          //应用进程参数不正确（缺少必选项）            //检查应用进程执行参数。                                                                                                                          
//0168          //应用进程参数不正确（不支持的选项）          //检查应用进程执行参数。                                                                                                                          
//0169          //应用进程参数不正确（错误的取值）            //检查应用进程执行参数。                                                                                                                          
//0170          //参数更新失败                                //检查参数文件状态，检查参数格式。                                                                                                                
//0171          //参数文件缺失                                //检查参数文件路径。                                                                                                                              
//0172          //参数配置信息缺失                            //检查参数配置信息完整性。                                                                                                                        
//0173          //                                                                                                                                                     
//0174          //                                                                                                                                                     
//0175          //                                                                                                                                                     
//0176          //                                                                                                                                                     
//0177          //                                                                                                                                                     
//0178          //                                                                                                                                                     
//0179          //                                                                                                                                                     
//0180          //                                                                                                                                                     
      
//0181          //取客户信息失败                               //检查传入信息是否正确，检查该共享内存状态。                                                                                                                 
//0182          //取帐户信息失败                               //检查传入信息是否正确，检查该共享内存状态。                                                                                                                 
//0183          //取用户信息失败                               //检查传入信息是否正确，检查该共享内存状态。                                                                                                                 
//0184          //取余额失败                                   //检查传入信息是否正确，检查该共享内存状态。                                                                                                                 
//0185          //取实时费用失败                               //检查传入信息是否正确，检查该共享内存状态。                                                                                                                 
//0186          //取历史欠费失败                               //检查传入信息是否正确，检查该共享内存状态。                                                                                                                 
//0187          //取信用度失败                                 //检查传入信息是否正确，检查该共享内存状态。                                                                                                                 
//0188          //查账单失败                                   //检查传入信息是否正确，检查该共享内存状态。                                                                                                                 
//0189          //查清单失败                                   //检查传入信息是否正确，检查该共享内存状态。                                                                                                                 
//0190          //查发票失败                                   //检查传入信息是否正确，检查该共享内存状态。                                                                                                                  
//0191          //                                                                                                                                                     
//0192          //                                                                                                                                                     
//0193          //                                                                                                                                                     
//0194          //                                                                                                                                                     
//0195          //                                                                                                                                                     
      
//0196          //应用进程已经存在                               //检查该应用进程是否已经存在。                                                                                                                       
//0197          //预处理进程异常                                 //检查预处理相关配置。                                                                                                                               
//0198          //进程状态非法                                   //检查该应用进程状态，检查系统状态 。
//0199          //应用程序收到退出信号异常退出                   //                                                                                                                                 
//0200          //                                                                                                                                                     
//0201          //                                                                                                                                                     
//0202          //                                                                                                                                                     
//0203          //                                                                                                                                                     
//0204          //                                                                                                                                                     
//0205          //                                                                                                                                                     
//0206          //                                                                                                                                                     
//0207          //                                                                                                                                                     
//0208          //                                                                                                                                                     
//0209          //                                                                                                                                                     
//0210          //                                                                                                                                                     
      
//0211          //获取对应账期失败                                						 //检查账期配置信息。                                                                                                                                                 
//0212          //CRM档案接口更新内存失败                                      //检查档案接口配置，检查共享内存状态。                                                                                                                 
//0213          //序号连续性校验失败                                           //检查交换机配置。                                                                                                                                     
//0214          //源事件类型不合法                                             //检查该源事件相关配置。                                                                                                                               
//0215          //话单格式转化失败                                             //检查话单模板配置。                                                                                                                                   
//0216          //计费应用初始化失败                                           //检查该应用配置，检查该应用传入参数。                                                                                                                 
//0217          //获取定价计划信息出错                                         //检查定价计划信息配置。                                                                                                                               
//0218          //定价计划处理出错                                             //检查定价计划信息配置。                                                                                                                               
//0219          //计费处理回退失败                                             //检查回退相关配置。                                            																															                                                                                       
//0220          //处理回收失败                                                 //检查回收相关配置。                                            																															                                                                                       
//0221          //话单分发失败                                                 //检查话单分发配置。                                            																															                                                                                       
//0222          //积量错误                                                     //检查累积量配置，检查该内存是否存在。                          																															                                                                                       
//0223          //更新余额失败                                                 //检查余额相关配置，检查该内存或数据库表是否存在。              																															                                                                                       
//0224          //更新实时费用失败（如：当月实时费用特别缴费做结清处理。）     //检查实时费用相关配置，检查该内存或数据库表是否存在。          																															                                                                                       
//0225          //更新欠费失败                                                 //检查欠费相关配置，检查该内存或数据库表是否存在。              																															                                                                                       
//0226          //生成停机数据失败                                             //检查停复机配置，检查相关接口状态。                            																															                                                                                       
//0227          //生成复机数据失败                                             //检查停复机配置，检查相关接口状态。                            																															                                                                                       
//0228          //生成催缴数据失败                                             //检查催缴配置，检查相关接口状态。                              																															                                                                                       
//0229          //取发票格式错误                                               //检查发票格式配置。                                            																															                                                                                       
//0230          //发票打印失败                                                 //检查发票配置信息，检查相关设备信息。                          																															                                                                                       
//0231          //外围接口故障(如超时等)                                       //检查外围接口状态，检查超时等配置信息。                        																															                                                                                       
//0232          //系统接收异常数据                                                         																															                                                                                       
//0233          //缴费金额超过最大金额数                                       //检查该缴费信息正确性，检查最大金额阀值。                      																															                                                                                       
//0234          //银行代扣销帐失败                                             //检查银行代扣相关配置，检查相关接口状态。                      																															                                                                                       
//0235          //批量销帐失败                                                 //检查批销配置，检查数据正确性。                                																															                                                                                       
//0236          //批量复机错误                                                 //检查批量停复机配置，检查相关接口状态。                        																															                                                                                       
//0237          //充值金额过大，系统不支持                                     //检查充值配置，检查最大金额阀值。                              																															                                                                                       
//0238          //余额账本过期                                                 //检查余额账本有效期。                                          																															                                                                                       
//0239          //已经反销 不能重复反销                                        //检查该返销数据正确性。                                                                                                                              
//0240          //其他错误                                                     //请查看系统日志                                                                                              
//0241          //超出营业终端连接数阀值                                       // 请联系管理员修改阀值或稍后再试                                                                                                                                                    
//0242          //计费号码异常																								// 检查规整规则或者原始话单                                                                                                                                                    
//0243          //生成停机工单																									//用户停机																														                                                                                                                                                     
//0244          //生成重单                                                      //检查重单规则                                                                                               
//0245          //                                                                                                                                                     
      
//0301          //携号转网数据不存在                                            //检查携号转网配置信息，检查相关接口状态。                                                                                                
//0302          //携号转网数据已失效                                            //检查携号转网配置信息，检查相关接口状态。                                                                                                
//0303          //携号转网数据未生效                                            //检查携号转网配置信息，检查相关接口状态。                                                                                                 
//0304          //用户已拆机                                                    //检查携号转网配置信息，检查相关接口状态。                                                                                                 
//0305          //                                                                                                                                                     
//0306          //                                                                                                                                                     
//0307          //                                                                                                                                                     
//0308          //                                                                                                                                                     
//0309          //                                                                                                                                                     
//0310          //                                                                                                                                                     
      
//0311          //被访运营商不存在                                               //检查运营商配置信息，检查相关接口状态。                                                                                                      
//0312          //被访运营商已失效                                               //检查运营商配置信息，检查相关接口状态。                                                                                                      
//0313          //被访运营商尚未生效                                             //检查运营商配置信息，检查相关接口状态。                                                                                                      
//0314          //被访运营商所在国家不存在                                       //检查运营商以及国家配置信息，检查相关接口状态。                                                                                              
//0315          //被访运营商所属资费区域不存在                                   //检查运营商以及资费区域配置信息，检查相关接口状态。                                                                                              
//0316          //                                                                                                                                                     
//0317          //                                                                                                                                                     
//0318          //                                                                                                                                                     
//0319          //                                                                                                                                                     
//0320          //                                                                                                                                                     

//0321          //被访运营商不存在                                                //检查运营商配置信息，检查相关接口状态。                                                                                                      
//0322          //被访运营商已失效                                                //检查运营商配置信息，检查相关接口状态。                                                                                                      
//0323          //被访运营商尚未生效                                              //检查运营商配置信息，检查相关接口状态。                                                                                                      
//0324          //被访运营商所在国家不存在                                        //检查运营商以及国家配置信息，检查相关接口状态。                                                                                              
//0325          //CtoG代理运营商不存在                                            //                                                                                              
//0326          //被访运营商与所属资费区域不存在                                  //检查运营商以及资费区域配置信息，检查相关接口状态。                                                                                              
//0327          //                                                                                                                                                     
//0328          //                                                                                                                                                     
//0329          //                                                                                                                                                     
//0330          //                                                                                                                                                     

//0331          //MIN码不存在                                                      //检查MIN码相关配置，检查相关接口状态。                                                                                                     
//0332          //MIN码已失效                                                      //检查MIN码相关配置，检查相关接口状态。                                                                                                     
//0333          //MIN码尚未生效                                                    //检查MIN码相关配置，检查相关接口状态。                                                                                                     
//0334          //MIN码归属国家代码异常（不存在）                                  //检查MIN以及国家配置信息，检查相关接口状态。                                                                                               
//0335          //MIN码归属国家代码异常（失效）                                    //检查MIN以及国家配置信息，检查相关接口状态。                                                                                               
//0336          //MIN码归属国家代码异常（多个）                                    //检查MIN以及国家配置信息，检查相关接口状态。                                                                                               
//0337          //MIN码归属运营商代码异常（不存在）                                //检查MIN以及运营商配置信息，检查相关接口状态。                                                                                             
//0338          //MIN码归属运营商代码异常（失效）                                  //检查MIN以及运营商配置信息，检查相关接口状态。                                                                                             
//0339          //MIN码归属运营商代码异常（多个）                                  //检查MIN以及运营商配置信息，检查相关接口状态。                                                                                          
//0340          //                                                                                                                                                     
//0341          //                                                                                                                                                     
//0342          //                                                                                                                                                     
//0343          //                                                                                                                                                     
//0344          //                                                                                                                                                     
//0345          //                                                                                                                                                     

//0346          //IMSI码不存在                                                      //检查IMSI码相关配置，检查相关接口状态。                                                                                                   
//0347          //IMSI码已失效                                                      //检查IMSI码相关配置，检查相关接口状态。                                                                                                   
//0348          //IMSI码尚未生效                                                    //检查IMSI码相关配置，检查相关接口状态。                                                                                                   
//0349          //IMSI对应CtoG代理运营商代码异常                                    //                                                                                         
//0350          //IMSI对应多个本地网                                                //检查IMSI码以及区域相关配置，检查相关接口状态。                                                                                            
//0351          //                                                                                                                                                     
//0352          //                                                                                                                                                     
//0353          //                                                                                                                                                     
//0354          //                                                                                                                                                     
//0355          //                                                                                                                                                     

//0356          //国家信息不存在                                    //                                                                                                             
//0357          //国家信息已失效                                    //                                                                                                             
//0358          //国家信息尚未生效                                  //                                                                                                             
//0359          //国家代码重复                                      //                                                                                                             
//0360          //国家名称异常                                      //                                                                                                             
//0361          //                                                                                                                                                     
//0362          //                                                                                                                                                     
//0363          //                                                                                                                                                     
//0364          //                                                                                                                                                     
//0365          //                                                                                                                                                     

//0366          //H码不存在                                          //检查H码相关配置，检查相关接口状态。                                                                                                     
//0367          //H码已失效                                          //检查H码相关配置，检查相关接口状态。                                                                                                     
//0368          //H码尚未生效                                        //检查H码相关配置，检查相关接口状态。                                                                                                     
//0369          //H码归属本地网异常（多个）                          //检查H码以及区域相关配置，检查相关接口状态。                                                                                             
//0370          //H码归属运营商异常（多个）                          //检查H码以及运营商相关配置，检查相关接口状态。                                                                                           
//0371          //H码归属运营商异常（不存在）                        //检查H码以及运营商相关配置，检查相关接口状态。                                                                                           
//0372          //                                                                                                                                                     
//0373          //                                                                                                                                                     
//0374          //                                                                                                                                                     
//0375          //                                                                                                                                                     

//0376          //SP代码不存在                                        //检查SP代码相关配置，检查相关接口状态。                                                                                                            
//0377          //SP代码已失效                                        //检查SP代码相关配置，检查相关接口状态。                                                                                                            
//0378          //SP代码尚未生效                                      //检查SP代码相关配置，检查相关接口状态。                                                                                                           
//0379          //                                                                                                                                                     
//0380          //                                                                                                                                                     
//0381          //                                                                                                                                                     
//0382          //                                                                                                                                                     
//0383          //                                                                                                                                                     
//0384          //                                                                                                                                                     
//0385          //                                                                                                                                                     
//0386          //边漫话单归属多个省                                    //                                                                                                         
//0387          //                                                                                                                                                     
//0388          //                                                                                                                                                     
//0389          //                                                                                                                                                     
//0390          //                                                                                                                                                     

//0501          // 用户不存在（用户输入错误，没有相关资料匹配）                                              //    检查配置                                                                
//0502          // 用户处于冷冻期                                                                            //   检查配置                                                                 
//0503          // 用户未使用                                                                                //   检查配置                                                                 
//0504          // 用户处于挂失状态                                                                          //   检查配置                                                                 
//0505          // 流水号错误导致帐户冲正失败                                                                //   检查配置                                                                 
//0506          // 跨省后付费用户暂时不提供余额查询                                                          //    检查配置                                                                
//0507          // 非中国电信用户（eg：被充值用户非中国电信用户）                                            //    检查配置                                                                
//0508          // 用户资料错误（用户已拆机等）                                                              //    检查配置                                                                
//0509          // 充值用户受限，不允许充值（如公话，网厅给外省电话充值等）                                  //    检查配置                                                                
//0510          // 代理商帐号不存在                                                                          //     检查配置                                                               
//0511          // 代理商余额不足                                                                            //    检查配置                                                                
//0512          // 代理商状态：注销                                                                          //     检查配置                                                               
//0513          // 代理商状态：锁定（即进入黑名单）                                                          //     检查配置                                                               
//0514          // 代理商状态：暂停（暂时不可使用）                                                          //     检查配置                                                               
//0515          // 计费系统数据库错误                                                                        //     检查配置                                                               
//0516          // 计费系统内部错误（內存分配失敗等）                                                        //     检查配置                                                               
//0517          // 充值失败（不易归类的错误导致的充值失败）                                                  //     检查配置                                                               
//0518          // 充值失败，不支持客户级充值                                                                //    检查配置                                                                
//0519          // 冲正失败（用户的余额已被使用，导致金额不足，或者有其它限制）                              //    检查配置                                                                
//0520          // 查帐户信息失败（余额账本不存在等）                                                        //    检查配置                                                                
//0521          // 该帐户信息被锁定，可能正执行某些操作，请重试（锁帐户失败）                                //     检查配置                                                               
//0522          // 无法新增账本（首次充值可能需要新增账本对象）                                              //    检查配置                                                                
//0523          // 充值失败，对应的余额类型不存在（余额账本类型不支持balanceType，余额类型无效）             //    检查配置                                                                
//0524          // 计费系统处于出帐或维护期，暂时不能充值  （禁充时间）                                      //     检查配置                                                               
//0525          // 账户下付费用户过多，不能充值（如：一个合同号下有1000个用户）                              //     检查配置                                                               
//0526          // 找不到资费策略																																						//  检查配置                                                                                                                                                   
//0527          //                                                                                                                                                     
//0528          //                                                                                                                                                     
//0529          //                                                                                                                                                     
//0530          //                                                                                                                                                     
//0531          //没有欠费记录                                                          //检查接口状态，检查用户归属地信息是否正确。                                                                                         
//0532          //异地缴费归属地获取销帐费用数据出错                                    //检查接口状态，等待其归属地HB系统恢复正常。                                                                                         
//0533          //没有销账流水                                                          //检查接口状态，通知归属地检查数据库状态。                                                                                           
//0534          //异地返销获取销帐记录出错                                              //检查接口状态，通知归属地检查HB系统状态。                                                                                           
//0535          //                                                                                                                                                     
//0536          //                                                                                                                                                     
//0537          //                                                                                                                                                     
//0538          //                                                                                                                                                     
//0539          //                                                                                                                                                     
//0540          //                                                                                                                                                     
//0541          //非约定查询高额类型                                                    //检查高额查询类型配置。
//0542          //系统不提供该日期的查询                                                //检查高额查询类型配置。
//0543          //没有查询到信息                                                        //                                                                                         
//0544          //                                                                                                                                                     
//0545          //                                                                                                                                                     
//0546          //                                                                                                                                                     
//0547          //                                                                                                                                                     
//0548          //                                                                                                                                                     
//0549          //                                                                                                                                                     
//0550          //                                                                                                                                                     
//0551          //删除记录失败                                                          //检查静态参数配置，检查相关接口状态。                                                                                          
//0552          //增加记录失败                                                          //检查静态参数配置，检查相关接口状态。                                                                                          
//0553          //更新记录失败                                                          //检查静态参数配置，检查相关接口状态。                                                                                         
//0554          //                                                                                                                                                     
//0555          //                                                                                                                                                     
//0556          //访问资料不存在                                                        //检查数据正确性，检查相关接口状态。                                                                                         
//0557          //系统不提供相应查询                                                    //检查数据正确性，检查相关接口状态。                                                                                         
//0558          //访问失败                                                              //检查数据正确性，检查相关接口状态。                                                                                         
//0559          //                                                                                                                                                     
//0560          //                                                                                                                                                     
//0561          //                                                                                                                                                     
//0562          //                                                                                                                                                     
//0563          //                                                                                                                                                     
//0564          //                                                                                                                                                     
//0565          //                                                                                                                                                     
//0566          //非约定查询类型                                                         //检查数据正确性，检查相关接口状态。   
//0567          //查询资料不存在                                                         //检查数据正确性，检查相关接口状态。   
//0568          //访问失败                                                               //检查数据正确性，检查相关接口状态。   
//0569          //                                                                                                                                                     
//0570          //                                                                                                                                                     
//0571          //                                                                                                                                                     
//0572          //                                                                                                                                                     
//0573          //                                                                                                                                                     
//0574          //                                                                                                                                                     
//0575          // 
//0576          //配置数据错误																													//
//0577          //向initserver注册失败																									//
//0578          //参数格式错误																													//		
//0579          //获取intserver返回结果失败																							//
//0580          //跟initserver通信获取返回空数据

//1901					//超过最大值                            //                                                                                                                      
//1902          //小于最小值                            //                                                                  
//1903          //不在枚举值范围                        //                                                                                              
//1904          //生效日期应是未来日期                        //                                                                                                                            
//1905          //无需生效日期                      //                                                                                                                             
//1906          //参数值不可更改                            //                                                                                                                            
//1907          //不是必需存在的参数                          //                                                                                                                         
//1908          //只能增加或减小的参数                        //                                                                                                                           
//1911          //参数只能增加                        //                                                                                                                       
//1912          //参数只能减小                        //                                                                                                                                                      
//1913          //最大最小值配置错误                               //                                                                                                                        
//1914          //时间数值不符合规则配置范围                          //                                                                                                                                   
																				//

//----------------------------------------
//#endif //__28_TEST_

//5004					//请求连接失败																													//PPP拨号用户已经存在,试图创建第二个PPP链接失败					
#define MBC_MEM_INTERFACE_ERROR                                 3

//@@    3       内存查表接口错误        请检查是否有足够的内存
#define MBC_SEMP_CREATE_FAIL                                            122
//@@    122     创建信号量失败          请检查信号量是否已被使用
#define	ERROR_Exceed_Max_User			1000
//1001		超过系统限制的最大用户数		//修改系统核心参数配置，重启系统
#define ERR_RULE_PARAM				1900
	

#define	ERROR_Param_Not_Exist			172

#define	ERROR_Param_Rule_Noncomp	166			
//1202		核心参数检查不符合规则			//检查参数规则
#define	ERROR_Param_Connection_Failed	141

#define	ERROR_Param_RefreshDB_Failed		145
#define	ERROR_Param_RefreshMem_Failed	172
//1204		参数刷新内存失败						//检查内存是否有该参数
#define	ERROR_Param_RefreshFile_Failed		171
#define	ERROR_Init_Cmd_Unrec						1206
//1206		命令无法识别								//版本不匹配


#define	ERROR_Init_File_Access						106

#define	ERROR_Init_File_Analysis					166

#define	ERROR_Init_Boot_Failed						1301
//1301	服务器启动失败								//服务器启动失败检查配置

#define	ERROR_Init_Msg_Failed						131

#define	ERROR_Init_Proc_Fork							5

#define	ERROR_Param_Not_Set						14

#define	ERROR_Param_Max_Users					1401
//1401	超过最大用户数								//检查核心参数的配置

#define	ERROR_Param_Max_Products			1402
//1402	超过最大产品数								///检查核心参数配置

#define	ERROR_Param_Max_Plans					1403
//1403	超过最大定价计划数						//检查核心参数配置

#define	ERROR_Param_Max_Offers					1404
//1404	超过最大商品数								//检查核心参数配置

#define ERROR_Process_Max_Times					1405
//1405	进程启动失败									//进程启动次数超过阀值

#define ERROR_RejectFile                                        1406
//1406	产生拒收文件                                                           // 请检查集团下发文件


//#
#define MBC_LIB				100000000
#define MBC_AvlTree			100010000
#define MBC_Compress			100020000
#define MBC_Date			100030000
//@@ 100030001	解析日期失败	请检查日期串和格式串(sDateStr, sFormat)
//@@ 100030002	取系统时钟失败	检查gettimeofday函数以及参数
#define MBC_Directory			100040000
#define MBC_Encrypt			100050000
#define MBC_Exception			100060000
#define MBC_File			100070000
#define MBC_FtpClient			100080000
//@@ 100080001	ftp登录失败	请检查ftp主机是否能连通以及用户名密码是否正确
//@@ 100080002	执行命令失败	请检查ftp主机是否能连通
//@@ 100080003	获取文件列表失败	请检查ftp是否可以访问
//@@ 100080004	ftp获取文件失败	请检查ftp服务器是否可以访问
//@@ 100080005	备份文件失败	请检查ftp服务器是否可以访问
//@@ 100080006	删除服务器上的文件失败	请检查ftp服务器是否可以访问
//@@ 100080007	访问ftp远程备份目录失败	请检查ftp服务器远程备份目录是否可以访问


#define MBC_HashList			100090000
//@@ 100090001	创建对象失败		请检查是否有足够的内存
//@@ 100090002	创建对象失败		HashListValue参数错误，不能为0
#define MBC_KeyList			100100000
//@@ 100100001	创建对象失败		请检查是否有足够的内存
#define MBC_KeyTree			100110000
//@@ 100110001	创建对象失败		请检查是否有足够的内存
#define MBC_MessageQueue		100120000
//@@ 100120012	创建对象失败		请检查是否有足够的内存
//@@ 100120101	打开消息队列失败	请检查权限设置
//@@ 100120102	打开消息队列失败	消息队列不存在，请检查消息队列是否被删除
//@@ 100120103	打开消息队列失败	请检查打开队列的参数的是否有效
//@@ 100120104	打开消息队列失败	不能创建已经存在的消息对立
//@@ 100120105	打开消息队列失败	请检查是否有足够的内存创建队列
//@@ 100120106	打开消息队列失败	系统级错误
//@@ 100120107	打开消息队列失败	没有可读取的消息
//@@ 100120108	打开消息队列失败	收到中断信号
//@@ 100120109	打开消息队列失败	请检查消息队列是否已经被关闭
//@@ 100120110	打开消息队列失败	消息队列已满
//@@ 100120111	打开消息队列失败	未知的错误
//@@ 100120201	发送消息失败		请检查权限设置
//@@ 100120202	发送消息失败		消息队列不存在，请检查消息队列是否被删除
//@@ 100120203	发送消息失败		请检查打开队列的参数的是否有效
//@@ 100120204	发送消息失败		不能创建已经存在的消息对立
//@@ 100120205	发送消息失败		请检查是否有足够的内存创建队列
//@@ 100120206	发送消息失败		系统级错误
//@@ 100120207	发送消息失败		没有可读取的消息
//@@ 100120208	发送消息失败		收到中断信号
//@@ 100120209	发送消息失败		请检查消息队列是否已经被关闭
//@@ 100120210	发送消息失败		消息队列已满
//@@ 100120211	发送消息失败		未知的错误
//@@ 100120301	接收消息失败		请检查权限设置
//@@ 100120302	接收消息失败		消息队列不存在，请检查消息队列是否被删除
//@@ 100120303	接收消息失败		请检查打开队列的参数的是否有效
//@@ 100120304	接收消息失败		不能创建已经存在的消息对立
//@@ 100120305	接收消息失败		请检查是否有足够的内存创建队列
//@@ 100120306	接收消息失败		系统级错误
//@@ 100120307	接收消息失败		没有可读取的消息
//@@ 100120308	接收消息失败		收到中断信号
//@@ 100120309	接收消息失败		请检查消息队列是否已经被关闭
//@@ 100120310	接收消息失败		消息队列已满
//@@ 100120311	接收消息失败		未知的错误
//@@ 100120401	关闭消息队列失败	请检查权限设置
//@@ 100120402	关闭消息队列失败	消息队列不存在，请检查消息队列是否被删除
//@@ 100120403	关闭消息队列失败	请检查打开队列的参数的是否有效
//@@ 100120404	关闭消息队列失败	不能创建已经存在的消息对立
//@@ 100120405	关闭消息队列失败	请检查是否有足够的内存创建队列
//@@ 100120406	关闭消息队列失败	系统级错误
//@@ 100120407	关闭消息队列失败	没有可读取的消息
//@@ 100120408	关闭消息队列失败	收到中断信号
//@@ 100120409	关闭消息队列失败	请检查消息队列是否已经被关闭
//@@ 100120410	关闭消息队列失败	消息队列已满
//@@ 100120411	关闭消息队列失败	未知的错误
//@@ 100120501	删除消息队列失败	请检查权限设置
//@@ 100120502	删除消息队列失败	消息队列不存在，请检查消息队列是否被删除
//@@ 100120503	删除消息队列失败	请检查打开队列的参数的是否有效
//@@ 100120504	删除消息队列失败	不能创建已经存在的消息对立
//@@ 100120505	删除消息队列失败	请检查是否有足够的内存创建队列
//@@ 100120506	删除消息队列失败	系统级错误
//@@ 100120507	删除消息队列失败	没有可读取的消息
//@@ 100120508	删除消息队列失败	收到中断信号
//@@ 100120509	删除消息队列失败	请检查消息队列是否已经被关闭
//@@ 100120510	删除消息队列失败	消息队列已满
//@@ 100120511	删除消息队列失败	未知的错误
//@@ 100120601	创建消息队列失败	请检查权限设置,或者队列大小设置
//@@ 100120602	创建消息队列失败	消息队列不存在，请检查消息队列是否被删除
//@@ 100120603	创建消息队列失败	请检查打开队列的参数的是否有效
//@@ 100120604	创建消息队列失败	不能创建已经存在的消息对立
//@@ 100120605	创建消息队列失败	请检查是否有足够的内存创建队列
//@@ 100120606	创建消息队列失败	系统级错误
//@@ 100120607	创建消息队列失败	没有可读取的消息
//@@ 100120608	创建消息队列失败	收到中断信号
//@@ 100120609	创建消息队列失败	请检查消息队列是否已经被关闭
//@@ 100120610	创建消息队列失败	消息队列已满
//@@ 100120611	创建消息队列失败	未知的错误

#define MBC_Precision			100130000
#define MBC_Queue			100140000
//@@ 100140001	创建对象失败		请检查是否有足够的内存
#define MBC_Semaphore			100150000
#define MBC_SimpleMatrix		100160000
//@@ 100160001	创建对象失败		请检查是否有足够的内存
#define MBC_SimpleSHM			100170000
//@@ 100170001	打开共享内存失败	系统调用失败，请参考子编码(shmget)
//@@ 100170002	删除共享内存失败	系统调用失败，请参考子编码(shmctl)
//@@ 100170003	创建对象失败		系统调用失败，请参考子编码(ftok)
//@@ 100170004	打开共享内存失败	共享内存不存在，请检查是否被删除
//@@ 100170005	创建共享内存失败	系统调用失败，请参考子编码(shmat)
//@@ 100170006	获取共享内存信息失败	系统调用失败，请参考子编码(shmctl)
#define MBC_SimpleFile          100171000
//@@ 100171001	打开文件映射失败	    系统调用失败
//@@ 100171002	删除文件失败	        系统调用失败
//@@ 100171003	创建对象失败		    系统调用失败
//@@ 100171004	打开文件失败	        文件不存在，请检查是否被删除
//@@ 100171005	创建文件映射失败	    系统调用失败
//@@ 100171006	获取映射文件信息失败	系统调用失败
#define MBC_Socket			100180000
#define MBC_Log				100190000
//@@ 100190000	打开日志文件失败	请检查log目录是否存在,目录是否可写
#define MBC_Stack			100200000
//@@ 100200001	创建对象失败		请检查是否有足够的内存

//#
//# MBC_PUBLIC
//#
#define MBC_PUBLIC			110000000
#define MBC_AcctItemMgr			110010000
//@@ 110010001	申请内存失败	请检查是否有足够的内存
//@@ 110010002	申请内存失败	请重新启动程序
#define MBC_AssignStatement		110020000
#define MBC_BillConditionMgr		110030000
#define MBC_BillOperationMgr		110040000
#define MBC_BillingCycle		110050000
#define MBC_BillingCycleMgr		110060000
//@@ 110060001	申请内存失败	请检查是否有足够的内存
#define MBC_EventSection		110070000
//@@ 110070001	申请内存失败	请调整CommonMacro.h中的MAX_EVENT_SECTION_NUM参数
//@@ 110070002	申请内存失败	请检查是否有足够的内存
//@@ 110070003	查找帐务周期失败	请检查BILLING_CYCLE配置是否正确
#define MBC_EventType			110080000
#define MBC_EventTypeMgr		110090000
//@@ 110090001	申请内存失败	请检查是否有足够的内存
//@@ 110090002	申请内存失败	有增加数据，请重新启动程序
#define MBC_ExtendFunctions		110100000
#define MBC_IDParser			110110000
//@@ 110110010	未知的ID	请增加IDParser::getValue中的ID
#define MBC_LifeCycle			110120000
#define MBC_LifeCycleMgr		110130000
//@@ 110130001	申请内存失败	请检查是否有足够的内存
//@@ 110130002	申请内存失败	有增加数据，请重新启动程序
#define MBC_LogicStatement		110140000
#define MBC_OrgMgr			110150000
//@@ 110150001	申请内存失败	请检查是否有足够的内存
//@@ 110150002	申请内存失败	有增加数据，请重新启动程序
#define MBC_SHMAccess			110160000
//@@ 110160001	申请内存失败	请检查是否有足够的内存
//@@ 110160002	申请内存失败	请检查是否有足够的内存
#define MBC_SHMData			110170000
//@@ 110170001	共享内存结构不符	请检查创建程序和连接使用的程序是否同样版本
//@@ 110170002	按条重用共享内存	内存单条结构体太小，不适合按条重用
//@@ 110170003	按块重用共享内存	内存单条结构体太小，不适合按块重用
//@@ 110170004	重用共享内存	内存不够用
#define MBC_SHMIntHashIndex		110180000
//@@ 110180001	申请内存失败	申请的共享内存已经使用完，请重新上载资料
#define MBC_SHMStringTreeIndex		110190000
//@@ 110190001	申请内存失败	申请的共享内存已经使用完，如果是实时更新，请重新上载资料，如果是全量加载，请调整SHMStringTreeIndex::create调用时的参数
//@@ 110190002	申请内存失败	申请的共享内存已经使用完，请重新上载资料
#define MBC_SHMLruData  110191000
//@@ 110191001	共享内存结构不符	请检查创建程序和连接使用的程序是否同样版本
//@@ 110191002	访问下标越界	请检查数据访问是否正确
//@@ 110191003	试图修改其他session正在修改的数据	请重新从头操作
//@@ 110191004	回滚空间不足	请修改后数据尽快commit，检查lrutofiles是否罢工
//@@ 110191005	所有内存都在进行修改，没有可供换出的空间	请修改后数据尽快commit，检查lrutofiles是否罢工
//@@ 110191006	访问的下标已经被删除	请不要对已经删除的数据项再操作
//@@ 110191007	对应的磁盘文件不能打开	请检查磁盘文件权限设置
//@@ 110191008	对应的磁盘文件数据损坏	请检查磁盘驱动器
//@@ 110191009	对应的磁盘文件写入数据出错	请检查磁盘文件权限设置和磁盘是否有足够的空间
//@@ 110191010	不能操作其他session	请重新从头操作
//@@ 110191011	session错误	重新从头操作
//@@ 110191012	内存不足	请停止部分占内存巨大的程序
#define MBC_SHMMixIndex		110192000
//@@ 110192001	申请内存失败	申请的共享内存已经使用完，如果是实时更新，请重新上载资料，如果是全量加载，请调整SHMMixIndex::create调用时的参数
//@@ 110192002	申请内存失败	申请的共享内存已经使用完，请重新上载资料
//@@ 110192003	主键超长失败	请调整SHMMixIndex::create

#define MBC_Statement			110200000
//@@ 110200001  申请内存失败    请检查主机是否有足够的内存

#define MBC_StdEvent			110210000
#define MBC_TransactionMgr		110220000
#define MBC_ProdOfferAggr       110221000
//@@ 110220001	申请共享内存(事件增量累积)失败	请检查是否有足够的内存
//@@ 110220002	申请共享内存(帐目增量累积)失败	请检查是否有足够的内存
//@@ 110220003	配置的事件结果域标识在程序中未定义	请检查相关配置或联系相关程序员
//@@ 110220004	共享内存(事件总量累积)不存在	请先运行事件总量累积上载程序
//@@ 110220005	共享内存(帐目总量累积)不存在	请先运行帐目总量累积上载程序
//@@ 110220006	共享内存(事件增量累积)不存在	请重启进程
//@@ 110220007	共享内存(帐目增量累积)不存在	请重启进程
//@@ 110220008	事件总量累积索引段溢出	请将EVENT_AGGR_INDEX_COUNT的值改大
//@@ 110220009	事件增量累积索引段溢出	请将EVENT_ADD_AGGR_INDEX_COUNT的值改大
//@@ 110220010	帐目总量累积索引段溢出	请将ACCTITEM_AGGR_INDEX_COUNT的值改大
//@@ 110220011	帐目增量累积索引段溢出	请将ACCTITEM_ADD_AGGR_INDEX_COUNT的值改大
//@@ 110220012	事件总量累积数据段溢出	请将EVENT_AGGR_DATA_COUNT的值改大
//@@ 110220013	帐目总量累积数据段溢出	请将ACCTITEM_AGGR_DATA_COUNT的值改大
//@@ 110220014	事件增量累积缓冲区溢出	请将ADD_AGGR_MAX_TABLE_NUM的值改大
//@@ 110220015	帐目增量累积缓冲区溢出	请将ADD_AGGR_MAX_TABLE_NUM的值改大
//@@ 110220016	事件增量累积数据段溢出	请将EVENT_ADD_AGGR_DATA_COUNT的值改大
//@@ 110220017	帐目增量累积数据段溢出	请将ACCTITEM_ADD_AGGR_DATA_COUNT的值改大
//@@ 110220018	一条事件记录包含太多的事件累积记录	请将MAX_EVENT_AGGR_TYPE_NUM_OfPerEvent的值改大
//@@ 110220019	申请共享内存(结束文件表)失败	请检查是否有足够的内存
//@@ 110220020	申请共享内存(事件入库缓冲)失败	请检查是否有足够的内存
//@@ 110220021	事件扩展属性表缓冲区溢出	请将EVENT_MAX_EXTEND_ATTR_NUM的值改大
//@@ 110220022	事件累积回退记录缓冲区溢出	请将MAX_EVENT_AGGR_TYPE_NUM_OfPerEvent的值改大
//@@ 110220023	帐目累积回退记录缓冲区溢出	请将MAX_ACCT_ITEM_PER_EVENT的值改大
//@@ 110220024	查找不到事件累积表的表名	具体信息请查看日志文件
//@@ 110220025	查找不到帐目累积表的表名	具体信息请查看日志文件
//@@ 110220026	扩展事件表名缓冲溢出		请将MAX_EXTEVENT_TABLE_NUM或MAX_END_FILE的值改大
//@@ 110220027	正常事件表名缓冲溢出		请将MAX_STDEVENT_TABLE_NUM或MAX_END_FILE的值改大
//@@ 110220028	提交记录数已达到		请先提交事务再插入事件至事务区
//@@ 110220030	查找不到事件表的类型		具体信息请查看日志文件
//@@ 110220031	查找不到事件表的表名		具体信息请查看日志文件
//@@ 110220032	回收事件表缓冲溢出		请将MAX_REDOEVENT_TABLE_NUM的值改大
//@@ 110220033	无主事件表缓冲溢出		请将MAX_NOOWNEREVENT_TABLE_NUM的值改大
//@@ 110220034	正常事件表缓冲溢出		请将MAX_STDEVENT_TABLE_NUM的值改大
//@@ 110220035	事件扩展属性表缓冲溢出		请将MAX_EXTEVENT_TABLE_NUM的值改大
//@@ 110220036	处理结束文件缓冲溢出		请将MAX_END_FILE的值改大


#define MBC_UserInfo			110230000
//@@ 110230001	申请内存失败		请检查是否有足够的内存
#define MBC_UserInfoCtl			110240000
//@@ 110240001	申请内存失败		客户数据区空间不够，请重新上载资料
//@@ 110240002	申请内存失败		客户群数据区空间不够，请重新上载资料
//@@ 110240003 	申请内存失败		客户定价信息数据区空间不够，请重新上载资料
//@@ 110240004	申请内存失败		产品信息数据区空间不够，请重新上载资料
//@@ 110240005	申请内存失败		商品信息数据区空间不够，请重新上载资料
//@@ 110240006	申请内存失败		主产品实例数据区空间不够，请重新上载资料
//@@ 110240007	申请内存失败		主产品实例地域信息数据区空间不够，请重新上载资料
//@@ 110240008	申请内存失败		主产品实例状态信息数据区空间不够，请重新上载资料
//@@ 110240009	申请内存失败		主产品实例标识信息数据区空间不够，请重新上载资料
//@@ 110240010	申请内存失败		用户群信息数据区空间不够，请重新上载资料
//@@ 110240011	申请内存失败		附属产品实例信息数据区空间不够，请重新上载资料
//@@ 110240012	申请内存失败		帐务定制信息数据区空间不够，请重新上载资料
//@@ 110240013	申请内存失败		商品明细实例信息数据区空间不够，请重新上载资料
//@@ 110240014	申请内存失败		客户数据区空间不够，请重新上载资料
//@@ 110240015  更新客户信息失败	没有找到对应的客户数据，请检查数据的正确性
//@@ 110240016  更新客户群信息失败	没有找到对应的客户数据，请检查数据的正确性
//@@ 110240017	申请内存失败		客户群数据区空间不够，请重新上载资料
//@@ 110240018  更新客户定价信息失败	没有找到对应的客户数据，请检查数据的正确性
//@@ 110240019 	申请内存失败		客户定价信息数据区空间不够，请重新上载资料
//@@ 110240020  更新客户定价信息失败	没有找到对应的客户定价信息数据，请检查数据的正确性
//@@ 110240021	申请内存失败		产品信息数据区空间不够，请重新上载资料
//@@ 110240022  更新产品信息失败	没有找到对应的产品数据，请检查数据的正确性
//@@ 110240023	申请内存失败		商品信息数据区空间不够，请重新上载资料
//@@ 110240024  更新商品信息失败	没有找到对应商品数据，请检查数据的正确性
//@@ 110240025	申请内存失败		主产品实例数据区空间不够，请重新上载资料
//@@ 110240026  更新主产品实例信息失败	没有找到对应主产品实例数据，请检查数据的正确性
//@@ 110240027  更新主产品实例地域信息失败	没有找到对应主产品实例数据，请检查数据的正确性
//@@ 110240028	申请内存失败		主产品实例地域信息数据区空间不够，请重新上载资料
//@@ 110240029  更新主产品实例状态信息失败	没有找到对应主产品实例数据，请检查数据的正确性
//@@ 110240030	申请内存失败		主产品实例状态信息数据区空间不够，请重新上载资料
//@@ 110240031	申请内存失败		主产品实例标识信息数据区空间不够，请重新上载资料
//@@ 110240032  更新用户群信息失败	没有找到对应主产品实例数据，请检查数据的正确性
//@@ 110240033	申请内存失败		用户群信息数据区空间不够，请重新上载资料
//@@ 110240034  更新用户群信息失败	没有找到对应用户群数据，请检查数据的正确性
//@@ 110240035  更新附属产品实例信息失败	没有找到对应主产品实例数据，请检查数据的正确性
//@@ 110240036	申请内存失败		附属产品实例信息数据区空间不够，请重新上载资料
//@@ 110240037  更新附属产品实例信息失败	没有找到对应附属产品实例数据，请检查数据的正确性
//@@ 110240038  更新帐务定制信息失败	没有找到对应附属产品实例数据，请检查数据的正确性
//@@ 110240039	申请内存失败		帐务定制信息数据区空间不够，请重新上载资料
//@@ 110240040  更新帐务定制信息失败	没有找到对应定制关系数据，请检查数据的正确性
//@@ 110240041	申请内存失败		商品明细实例信息数据区空间不够，请重新上载资料
//@@ 110240042  更新商品明细实例状态信息失败	没有找到对应主产品实例数据，请检查数据的正确性
//@@ 110240043  更新商品明细实例状态信息失败	没有找到对应商品实例数据，请检查数据的正确性
//@@ 110240044  更新商品明细实例状态信息失败	没有找到对应商品明细实例数据，请检查数据的正确性
//@@ 110240045  申请内存失败		附属产品实例属性信息数据区空间不够，请重新上载资料
//@@ 110240046  更新附属产品实例属性失败 没有找到对应的附属产品实例的数据，请检查数据的争取性
//@@ 110240047  申请内存失败		附属产品实例属性信息数据区空间不够，请重新上载资料
//@@ 110240048  申请内存失败		商品实例信息数据区空间不够，请修改参数后重新上载资料
//@@ 110240049  申请内存失败		商品实例明细信息数据区空间不够，请修改参数后重新上载资料
//@@ 110240050  上载商品实例失败	商品实例信息中的商品标示在商品表中未定义，请修改后重新上载资料
//@@ 110240051  申请内存失败		商品实例属性信息数据区空间不够，请修改参数后重新上载资料
//@@ 110240053  申请内存失败		更新商品实例信息时数据区空间不够，请修改参数后重新上载资料
//@@ 110240054	删除内存档案失败	确认已经停止相关的所有进程
//@@ 110240100  获取参数失败		系统参数中未配置用户档案的内存大小
#ifdef GROUP_CONTROL
//@@ 110240104	申请内存失败		群信息定义数据区空间不够，请重新上载资料
//@@ 110240105	申请内存失败		群成员信息数据区空间不够，请重新上载资料
//@@ 110240106	申请内存失败		号码与群关系信息数据区空间不够，请重新上载资料
//@@ 110240107	更新群成员信息失败	没有找到对应的群号码信息，请检查数据的正确性
//@@ 110240108	更新群成员信息失败	找到了对应的群号码信息但是不匹配，请检查数据的正确性
//@@ 110240109	更新号码群关系信息失败	没有找到对应的号码群关系信息，请检查数据的正确性
//@@ 110240110	更新号码群关系信息失败	找到了对应的号码群关系信息但是不匹配，请检查数据的正确性
#endif
#ifdef IMMEDIATE_VALID
//@@ 110240111	申请内存失败		用户订购套餐实时生效的数据区空间不够，请重新上载资料
#endif
//@@ 110240120	获取参数失败		b_param_define表未配置param_code:IMSI_MDN_RELATION_NUM
//@@ 110240121	申请内存失败		IMSI与MDN映射关系数据区空间不够，请重新上载资料
//@@ 110240122	更新IMSI与MDN的映射关系失败		没有找到对应relation_id的数据，请检查数据正确性	
//@@ 110240123	更新IMSI与MDN的映射关系失败		没有找到对应IMSI的数据，请检查数据正确性	
//@@ 110240124  插入商品实例明细错误    商品实例明细数据已经存在
//@@ 110240126  新增主产品实例信息失败	主产品实例信息已存在，请检查数据的正确性
//@@ 110240127  新增或更新主产品实例主产品信息失败	主产品实例信息不存在，请检查数据的正确性
//@@ 110240128  更新附属产品实例属性信息失败		要更新的附属产品实例属性信息不存在，请检查数据正确性
//@@ 110240129  新增帐户信息失败		帐户信息在内存中已存在，请检查数据正确性

#define MBC_HighFeeAggr     980000000
//@@ 980000001	申请内存失败	请检查是否有足够的内
#define MBC_VpnInfoCtl			110245000
//@@ 110245001	申请内存失败			VPN数据区空间不够，请重新上载资料
//@@ 110245002	申请内存失败			VPN成员数据区空间不够，请重新上载资料
//@@ 110245003 	申请内存失败			VPN与VPN关系数据区空间不够，请重新上载资料
//@@ 110245004	申请内存失败			VPN与外号码数据区空间不够，请重新上载资料
//@@ 110245005	更新中申请内存失败		VPN数据区空间不够，请重新上载资料
//@@ 110245006	更新中申请内存失败		VPN成员数据区空间不够，请重新上载资料
//@@ 110245007 	更新中申请内存失败		VPN与VPN关系数据区空间不够，请重新上载资料
//@@ 110245008	更新中申请内存失败		VPN与外号码数据区空间不够，请重新上载资料
//@@ 110245009	更新失败				VPN信息不存在，请检查数据完整性
//@@ 110245010	更新失败				VPN成员信息不存在，请检查数据完整性
//@@ 110245011	更新失败				VPN与VPN关系数据不存在，请检查数据完整性
//@@ 110245012	更新失败				VPN外号码信息不存在，请检查数据完整性
//@@ 110245013	删除VPN内存档案失败	    确认已经停止相关的所有进程


//@@ 110245100  获取参数失败			系统参数中未配置VPN档案的内存大小

#define MBC_IvpnInfoCtl			110245200
//@@ 110245201	申请内存失败			ServAcctEx数据区空间不够，请重新上载资料
//@@ 110245202	申请内存失败			AcctAttr数据区空间不够，请重新上载资料
//@@ 110245213	删除IVPN内存失败	    确认已经停止相关的所有进程
//@@ 110245221	更新代付关系失败	    代付关系表中的SERV_ID在用户表中未定义
//@@ 110245222	更新代付关系失败	    代付关系表中的ACCT_ID在帐户表中未定义
//@@ 110245223	更新代付关系失败	    代付关系表中的主建在共享内存中未定义,无法更新
//@@ 110245224	插入代付关系失败	    代付关系表中的主建在共享内存中已定义,无法插入
//@@ 110245231	更新帐户扩展属性失败	A_ACCT_ATTR表中的ACCT_ID在帐户表中未定义
//@@ 110245232	更新帐户扩展属性失败	A_ACCT_ATTR表中的主建在共享内存中未定义,无法更新
//@@ 110245233	插入帐户扩展属性失败	A_ACCT_ATTR表中的主建在共享内存中已定义,无法插入
//@@ 110245300  获取参数失败			系统参数中未配置IVPN集团代付的内存大小

#define MBC_ScaleInfoCtl			110245400
//@@ 110245401	申请内存失败		N_Route数据区空间不够，请重新上载资料
//@@ 110245402	申请内存失败		NetScaleInfo数据区空间不够，请重新上载资料
//@@ 110245410  获取参数失败		系统参数中未配置摊分运营商内存大小
//@@ 110245411  获取参数失败		系统参数中未配置专线摊分规则内存大小
//@@ 110245420	删除SCALE内存失败	确认已经停止相关的所有进程
//@@ 110245430  参数配置有误		本地运营商的区域编号没有配置请检查
//@@ 110245431  参数配置有误		境外运营商的区域编号没有配置请检查
//@@ 110245432  获取参数失败		本地运营商参数没有配置请检查
//@@ 110245433  参数配置有误		本地运营商参数配置有误
//@@ 110245440  参数配置有误		专线一次费的目录没有配置请检查
//@@ 110245451  获取seq序列失败     请检查数据库序列scale_item_seq
//@@ 110245452  获取seq序列失败     请检查数据库序列acct_item_id_big_seq
#define MBC_SpecialTariffMgr			110246000
//@@ 110246001	申请内存失败			SP特殊资费加载内存空间不够，请重新加载
//@@ 110246002	参数未配置				SP特殊资费需要的属性ID列表未配置，请配置后重新启动
//@@ 110246003	参数配置不完整			SP特殊资费需要的属性ID未配置完整，请配置后重新启动
//@@ 110246004	参数配置不完整			SP特殊检查的存放值的属性ID未配置，请配置后重新启动

#define MBC_UserInfoReader		110250000
//@@ 110250001	连接共享内存失败	请检查共享内存是否被删除且重新上载资料
//@@ 110250002  申请内存失败		请检查是否有足够的内存创建对象
#define MBC_Value			110260000
#define MBC_ZoneMgr			110270000
//@@ 110270001	申请内存失败		请检查是否有足够的内存创建对象
//@@ 110270002	申请内存失败		有增加数据，请重新启动程序
//@@ 110270003	整理数据出错		请检查数据的正确性
#define MBC_EventSectionUnioner		110280000
//@@ 100280001	创建对象失败		请调整CommonMacro.h中的MAX_CHARGE_NUM参数
#define MBC_InstTableListMgr		110290000
//@@ 110290001	申请内存失败	请检查是否有足够的内存
#define MBC_BalanceMgr			110300000
//@@ 110300001	无法连接共享内存(余额)	请先运行余额上载程序
//@@ 110300002	共享内存(余额)不存在	请先运行余额上载程序
#define MBC_ReferenceObjectMgr		110310000

#define MBC_BillingCycleAdmin		110320000
#define MBC_BillingCycleBase		110330000
#define MBC_E8UserCtl				110340000
//@@ 110340001	申请内存失败				e8档案上载数据空间不够
//@@ 110340002	申请内存失败				e8档案上载索引空间不够
//@@ 110340003	删除内存失败				确认已停止所有连接内存的相关进程
//@@ 110340004	连接内存失败				确认相关内存已经建立
//@@ 110340050	获取参数失败				系统参数中未配置e8档案的内存大小

#define MBC_DataEventDiv			110350000
//@@ 110350001	申请内存失败				数据话单内存数据空间不够
//@@ 110350002	申请内存失败				e8档案上载索引空间不够
//@@ 110350003	删除内存失败				确认已停止所有连接内存的相关进程
//@@ 110350004	连接内存失败				确认相关内存已经建立
//@@ 110350040	获取环境变量失败			请将相关环境变量export
//@@ 110350041	获取组别失败				请在b_param_define中配置进程对应的组别（1，2，3）
//@@ 110350050	获取参数失败				系统参数中未配置e8档案的内存大小

#define MBC_initserver  340010000
//@@ 340010001	initserver注册失败			initserver注册失败
//@@ 340010002	initserver获取初始参数值失败	initserver获取初始参数值失败
//@@ 340010003	initserver预约失败			initserver预约失败
//
#define MBC_Log_Group		100191000
//@@ 100191001	日志组申请内存失败				请检查系统资源
//@@ 100191002	日志组共享内存创建失败			系统调用失败，请参考子编码(shmat)
//@@ 100191003	日志组共享内存删除失败			系统调用失败，请参考子编码(shmat)	
//@@ 100191004	日志组获取共享内存信息失败		请检查是否已创建
//@@ 100191005	日志组初始化失败				请检查日志组配置
//@@ 100191006	日志组切换失败					请检查当前日志组状态及信息
//@@ 100191007	日志组打开日志文件失败			请检查文件目录是否存在，目录是否可写
//@@ 100191008	日志组获取文件状态出错			请检查文件状态
//@@ 100191009	日志组操作命令参数错误			请参考输入说明
//@@ 100191010	日志组操作命令删除成员文件失败	请查看文件所在日志组状态信息
//@@ 100191011	日志组切换标识超出范围			请检查该无法识别的日志组标识
//@@ 100191012	日志组获取当前使用组失败		请检查当前日志组状态信
#define	MBC_Server							320030000
//@@ 320030001	申请内存失败	命令没有指定共享内存的键
#define MBC_ExtServCtl				110360000
//@@ 110360001	申请内存失败				ext_serv上载数据空间不够
//@@ 110360002	申请内存失败				ext_serv上载索引空间不够
//@@ 110360003	删除内存失败				确认已停止所有连接内存的相关进程
//@@ 110360004	连接内存失败				确认相关内存已经建立
//@@ 110360050	获取参数失败				系统参数中未配置ext_serv的内存大小

//#
//# MBC_LIBOCI
//#
#define MBC_LIBOCI			120000000
#define MBC_TOCIDirPath			120010000
//#define MBC_exception			120020000
#define MBC_TOCIQuery			120030000
#define MBC_TBatchInsert		120040000

//#
//# MBC_CPARSER
//#
#define MBC_CPARSER			130000000
#define MBC_TScriptParser		130010000
#define MBC_wwInterpreter		130020000
#define MBC_wwLex			130030000
#define MBC_wwParser			130040000
#define MBC_exception			130050000

//#
//# MBC_FRAMEWORK
//#
#define MBC_FRAMEWORK			140000000
#define MBC_DistributeRule		140010000
#define MBC_Environment			140020000
//@@ 140020001	创建对象失败		请检查是否有足够内存
#define MBC_EventReceiver		140030000
//@@ 140030001	创建对象失败		请检查是否有足够内存
#define MBC_EventSender			140040000
//@@ 140040001	创建对象失败		请检查是否有足够内存
#define MBC_Process			140050000
//@@ 140050000	应用程序收到退出信号异常退出		检查SIGQUIT,SIGTERM,SIGINT信号的来源
//@@ 140050001	应用程序收到前台调度指令正常退出	---
//@@ 140050002	取不到环境变量PROCESS_ID		请设置环境变量PROCESS_ID,例:export PROCESS_ID=123
//@@ 140050003	取不到APP_ID,BILLFLOW_ID		请检查wf_process表,是否存在该process_id
//@@ 140050004	捕获SIGQUIT信号失败			---
//@@ 140050005	捕获SIGTERM信号失败			---
//@@ 140050006	捕获SIGINT信号失败			---
//@@ 140050007	捕获SIGUSR1信号失败			---
//@@ 140050008	日志文件打开失败			检查TIBS_HOME/log/ 目录是否存在,是否可写

//#
//# MBC_MIDWARE_ACCESS
//#
#define MBC_MIDWARE_ACCESS		150000000

//#
//# MBC_MEMORY_MANAGER
//#
#define MBC_MEMORY_MANAGER		160000000

//#
//# MBC_TERMINAL_CONTROL
//#
#define MBC_TERMINAL_CONTROL		170000000
#define MBC_control			170010000
#define MBC_edit			170020000
#define MBC_button			170030000
#define MBC_listbox			170040000
#define MBC_readinfo			170050000

//#
//# MBC_APP_PRICING
//#
#define MBC_APP_PRICING			180000000
#ifdef GROUP_CONTROL
//@@ 180000001  参数未配置	党政手机包营销使用的事件属性ID未配置，请配置
//@@ 180000002  参数未配置	党政手机包营销使用的事件属性值未配置，请配置
#endif
#define MBC_PricingMeasure		180010000
#define MBC_Tariff			180020000
#define MBC_Discount			180030000
//@@ 180030010  账目类型组没有成员	检查账目类型组的配置
#define MBC_PricingInfoMgr		180040000
//@@ 140050001	内存申请失败 			执行 ulimit -a 看单进程的内存限制
//@@ 180040003	定价策略没有找到  		检查表约束,或设置LOG_LEVEL>100, 查看策略ID
//@@ 180040004	取定价组合失败 			检查 HashList 模板
//@@ 180040017	取定价段落失败 			检查 HashList 模板
#define MBC_PricingInfo			180050000
#define MBC_BillObject			180060000
#define MBC_PricingCombine		180070000
#define MBC_Pricing			180080000
//@@ 180080002	内存申请失败			执行 ulimit -a 看单进程的内存限制

//#
//# MBC_APP_MONITOR
//#
#define MBC_APP_MONITOR			190000000
#define MBC_monitor			190010000
#define MBC_ipc_op			190020000
#define MBC_login_f			190030000
#define MBC_login_j			190040000
#define MBC_password_f			190050000
#define MBC_password_j			190060000
#define MBC_mainmenu_j			190070000
#define MBC_mainmenu_f			190080000
#define MBC_process_f			190090000
#define MBC_process_j			190100000
#define MBC_ticket_f			190110000
#define MBC_ticket_j			190120000
#define MBC_mnt				190130000
#define MBC_mnt_interface		190140000
#define MBC_socketapi			190150000

//#
//# MBC_APP_CYCLEFEE
//#
#define MBC_APP_CYCLEFEE		200000000
//@@ 200000001	参数错误,必须是1和2	请检查输入参数
//@@ 200000002	输入参数错误	cyclefee 2 yyyymmdd yyyymmdd,起始日期是闭区间，结束日期是开区间
//@@ 200000003	处理异常	检查用户资料
//@@ 200000004	日期非法	检查日期的正确性和输入格式（yyyymmdd）
#define MBC_CycleFeeEvent		200010000
#define MBC_CycleFeeEventCreaterMgr	200020000
//@@ 200020001	生成本进程对应的帐期类型和帐期标识之间的映射关系报错	请检查b_creator_process,检查此进程是否配置了对应的帐期类型
//@@ 200020002	生成产品和周期性费用事件类型之间的映射关系报错	请检查b_cycle_fee_event_type,检查此进程是否配置了对应的帐期类型
//@@ 200020003	输出周期性费用事件到文件有问题	请检查文件路径是否存在,文件是否生成
//@@ 200020004	创建周期性费用事件失败	请检查用户资料
//@@ 200020005	提交周期性费用文件报错	请检查文件是否存在
#define MBC_CycleFeeEventMgr		200030000
#define MBC_CycleInfo			200040000
#define MBC_FileInfo			200050000

//#
//# MBC_APP_PREPROCESS
//#
#define MBC_PreProcess             210000000
//@@ 210000001	初始化失败	请检查格式标准化参数是否配置完整
//@@ 210000002  将文件更新为运行状态失败  请检查数据库以及b_event_file_list表
//@@ 210000003  将文件更新为错误状态失败  请检查数据库以及b_event_file_list表
//@@ 210000004  将文件更新为结束状态失败  请检查数据库以及b_event_file_list表
//@@ 210000005	错误文件改名出错	请检查是否存在此文件
//@@ 210000005	输出文件改名出错	请检查是否存在此文件
//@@ 210000007	查找对应模板出错或者打开文件发生异常	请检查是否存在此文件,是否配置了该种交换机对应的解析模板

#define MBC_EventExtractor         210010000
//@@ 210010001	获取交换机信息出错	请检查b_switch_info这张表,是否配置了该种交换机信息
//@@ 210010002	获取该交换机对应的模板出错	请检查是否配置了该种交换机对应的模板
//@@ 210010003	打开文件出错	请检查文件列表中的此文件是否真是存在
//@@ 210010004	获取文件中的一条事件记录出错	请检查b_object_define表中对象对应是否正确
//@@ 210010005	解析一条事件记录中的字段出错	请检查字段信息表是否对字段的信息配置正确
//@@ 210010006	字段转换出错	请检查每张字段信息表中字段转换信息
//@@ 210010007	取字段结果出错	请检查LocalConvert.cpp对字段的个性化转换是否有问题
#define MBC_FileEventOwner         210020000
//@@ 210020001	从数据库中取对象定义出错	请检查数据库是否可以连接,检查b_object_define表,确认每个映射(交换机类型跟模板之间的映射)都已经正确配置对象
//@@ 210020002	从数据库中取每种类型事件记录的解析组出错	请检查数据库是否可以连接,检查b_map_parser_group表,确认此映射对应的各种事件记录都已经正确配置
//@@ 210020003	从数据库中取每种记录类型的字段解析信息出错	请检查数据库是否可以连接,检查此映射对应的字段表,确认各个字段的解析信息已经配置正确
//@@ 210020004	打开文件出错	请检查此文件是否真实存在
#define MBC_Block                  210030000
//@@ 210030001	说明没有一个标志匹配上,有问题	请检查配置是否正确,该种对象是通过标志位来确定其起始位置的,但是系统匹配不上任何标志
#define MBC_EventBlock             210040000
#define MBC_ChildEventBlock        210050000
#define MBC_OneEvent               210060000
#define MBC_Field                  210070000
#define MBC_FieldCheck             210080000
#define MBC_ParamMgr			   210090000
#define MBC_PpConvertUtility       210100000
#define MBC_LocalConvert           210110000
#define MBC_AttrTransMgr           210120000
//@@ 210120001	申请内存出错(AttrTransMgr::reloadRule() -- *ppFormat = new FormatGroup;)
//@@ 210120002	申请内存出错(AttrTransMgr::reloadRule() -- *ppStep = new FormatStep();)
//@@ 210120003	申请内存出错(AttrTransMgr::reloadRule() -- *ppRule = new FormatRule();)


#define MBC_EventFormat            210130000

#define MBC_FormatMethod           210140000
//@@ 210140001  申请内存出错(new KeyTree<Trunk>)
//@@ 210140002	申请内存出错(new KeyTree<TServiceInfo2>)
//@@ 210140003	申请内存出错(new KeyTree<int>)
//@@ 210140004	申请内存出错(new KeyTree<HLR>)
//@@ 210140005	申请内存出错(loadCarrier()-new KeyTree<int>)
//@@ 210140006	申请内存出错: Err when new TSwitchInfo[m_iSwitchInfoCnt] - FormatMethod::loadSwitchInfo()
//@@ 210140007	申请内存出错: err when m_pSwitchInfoPosTree = new KeyTree<int>; - FormatMethod::loadSwitchInfo()
//@@ 210140008	申请内存出错: err when new PNSegBilling[m_iPNSegBillingCnt];
//@@ 210140009	申请内存出错: err when new TServiceInfo[m_iServiceInfoCnt];
//@@ 210140010	申请内存出错: ERR when m_poOrgMgr = new OrgMgr; -- FormatMethod::getServiceInfo2
//@@ 210140011	申请内存出错: err when m_poLocalHeadMgr = new LocalHeadMgr;
//@@ 210140012	
//@@ 210140013
//@@ 210140014
//@@ 210140015
//@@ 210140016
//@@ 210140017
//@@ 210140018
//@@ 210140019

#define MBC_LongEvtRule           210150000
//@@ 210150001	申请内存出错(LongEventTypeRuleMgr::loadRule()-- pt = new LongEventTypeRule;)

#define MBC_LocalEvtRule          210160000
//@@ 210160001  申请内存出错(LocalEventTypeRuleMgr::loadRule() -- m_poOrgMgr = new OrgMgr;)
//@@ 210160002	申请内存出错(LocalEventTypeRuleMgr::loadRule() -- pt = new LocalEventTypeRule;)



//#
//# MBC_APP_PUTINDB
//#
#define MBC_APP_PUTINDB			220000000

//#
//# MBC_APP_WORKFLOW
//#
#define MBC_APP_WORKFLOW		230000000
#define MBC_guard			230010000
#define MBC_guard_main			230020000
#define MBC_guard_remote		230030000
#define MBC_public			230040000
#define MBC_server			230050000
#define MBC_server_client		230060000
#define MBC_server_monitor		230070000
#define MBC_workflow			230080000

//#
//# MBC_APP_STAT
//#
#define MBC_APP_STAT			240000000
#define MBC_BssOrgMgr			240010000 

//#
//# MBC_APP_DUPCHECKER
//#
#define MBC_APP_DUPCHECKER 240020000
//@@ 240020001		CheckDup.ini中FILE_SECS没有配置		请检查对应PROCESS_ID的FILE_SECS项配置
//@@ 240020002		CheckDup.ini中BLOCK_SIZE没有配置	请检查对应PROCESS_ID的BLOCK_SIZE项配置
//@@ 240020003		CheckDup.ini中BUFFER_SIZE没有配置	请检查对应PROCESS_ID的BUFFER_SIZE项配置
//@@ 240020004		OCI异常错误												再运行一次
//@@ 240020005		配置错误													请检查配置B_DUP_DATA_PATH或B_DUP_CHECK_CFG有没有配置记录
//@@ 240020006		配置错误													请检查重单类型判断规则B_DUP_TYPE_RULE,存在某个重单类型的CHILD_STEP_ID1或CHILD_STEP_ID2不是该重单类型的STEP_ID
//@@ 240020007		配置错误													请检查重单类型判断规则B_DUP_TYPE_RULE,&&运算的操作符不能为字符形
//@@ 240020008		配置错误													请检查重单类型判断规则B_DUP_TYPE_RULE,||运算的操作符不能为字符形
//@@ 240020009		配置错误													请检查重单类型判断规则B_DUP_TYPE_RULE,!运算的操作符不能为字符形
//@@ 240020010		配置错误													请检查重单类型判断规则B_DUP_TYPE_RULE,配置的运算的操作符程序不支持
//@@ 240020011		内存错误													内存不足
//@@ 240020012		配置错误													配置的重单稽核字段定义域程序不支持,请检查配置重单类型判断规则B_DUP_TYPE_RULE
//@@ 240020013		配置错误													参数1与参数2类型不一致,请检查配置重单类型判断规则B_DUP_TYPE_RULE
//@@ 240020014		查重目录													请检查查重目录及其权限设置


//#
//# MBC_APP_STOP
//#
#define MBC_APP_STOP 			240030000

//#
//# MBC_APP_UNIONACCT
//#
#define MBC_LoadAggr			250000000
//@@ 250000001	分配共享内存(事件总量累积)时出错	请检查是否有足够的内存
//@@ 250000002	连接共享内存(事件总量累积)时出错	请重启进程
//@@ 250000003	分配共享内存(帐目总量累积)时出错	请检查是否有足够的内存
//@@ 250000004	连接共享内存(帐目总量累积)时出错	请重启进程
//@@ 250000005	共享内存(事件总量累积)不存在		请重启进程
//@@ 250000006	共享内存(帐目总量累积)不存在		请重启进程
//@@ 250000007	事件总量累积索引段溢出			请将EVENT_AGGR_INDEX_COUNT的值改大
//@@ 250000008	帐目总量累积索引段溢出			请将ACCTITEM_AGGR_INDEX_COUNT的值改大
//@@ 250000009	事件总量累积数据段溢出			请将EVENT_AGGR_DATA_COUNT的值改大
//@@ 250000010	帐目总量累积数据段溢出			请将ACCTITEM_AGGR_DATA_COUNT的值改大
//@@ 250000011	参数不正确				请修改参数后重启进程
//@@ 250000012	事件累积共享内存中写锁信号量初始化错误	请检查KEY值
//@@ 250000013	帐目累积共享内存中写锁信号量初始化错误	请检查KEY值

#define MBC_LoadBalance			250010000
//@@ 250010001	分配共享内存(余额)时出错		请检查是否有足够的内存
//@@ 250010002	共享内存(余额)不存在			请重启进程
//@@ 250010003	余额索引段溢出				请将BALANCEBUF_INDEX_COUNT的值改大
//@@ 250010004	余额数据段溢出				请将BALANCEBUF_DATA_COUNT的值改大
//@@ 250010005	参数不正确				请修改参数后重启进程

#define MBC_FiltrateStopUser		250011000
//@@ 250011001	取不到过滤条件		请先配置过滤条件b_filtrate_Stop_User_cond

//#
//# MBC_APP_VIREMENT
//#
#define MBC_BankMgr			250020000
//@@ 250020001	该账户所对应的银行帐号不存在			请检查表BANK_BRANCH
//@@ 250020002	分配托收单节点空间失败

#define MBC_PaymentPlanMgr			250021000
//@@ 250021001	支付方案初始化----分配qry空间失败
//@@ 250021002	该帐户没有配置支付方案
//@@ 250021003	该帐户没有配置托收支付方案

#define MBC_TrustBankMgr			250022000
//@@ 250022001	托收单类初始化----分配qry空间失败
//@@ 250022002	托收单类初始化----分配pBankMgr空间失败
//@@ 250022003	托收单类初始化----分配pPaymentPlanMgr空间失败
//@@ 250022004	查询托收数据----未指定所要进行托收的帐期
//@@ 250022005	参数初始化出错----Example:: trustbank -c
//@@ 250022006	参数初始化出错----分配TrustBankSeqMgr空间失败
//@@ 250022007	参数初始化出错----所指定帐期类型不存在
//@@ 250022008	参数初始化出错----所指定帐期类型没有需要送托的帐期
//@@ 250022009	该帐户优先级最高的付款方式不是托收方式

#define MBC_SystemSwitchMgr			250023000
//@@ 250023001	系统参数类初始化----分配qry空间失败
//@@ 250023001	系统参数类初始化----该参数所对应的所对应的值未配置

#define MBC_AcctItemVireMgr			250024000
//@@ 250024001	账目管理类初始化----分配qry空间失败

#define MBC_BillMgr					250025000
//@@ 250025001	自动扣款类初始化----分配qry空间失败
//@@ 250025002	自动扣款类初始化----分配pSystemSwitchMgr空间失败
//@@ 250025003	该账户存在多条零头类型的余额账本
//@@ 250025004	该账户存在多条自动扣款类型的余额账本

#define MBC_VirementMgr				250026000
//@@ 250026001	下帐类初始化----分配qry空间失败
//@@ 250026002	下帐类初始化----分配pCfgMgr空间失败
//@@ 250026003	下帐类初始化----分配pAcctItemMgr空间失败
//@@ 250026004	下帐类初始化----分配pPaymentPlanMgr空间失败
//@@ 250026005	下帐类初始化----分配pBillMgr空间失败
//@@ 250026006	参数初始化出错----Example:: virement -c
//@@ 250026007	参数初始化出错----所指定帐期类型不存在
//@@ 250026008	参数初始化出错----所指定帐期类型没有需要下帐的帐期
//@@ 250026009	参数初始化出错----未指定所要进行下帐的帐期
//@@ 250026010	run():当前合同号帐目太多
//@@ 250026011	用户资料共享内存中查找用户资料失败
//@@ 250026012	所的资料档案信息为空
//@@ 250026013	下帐类初始化----分配pRedoLogMgr空间失败
//@@ 250026014	帐末下帐需先断点----检查(B_REDO_LOG表)
//@@ 250026015	帐末下帐进程正处于运行状态----运行前请检查PROCESS_ID环境变量
#define	MBC_BalItemGroupMgr			250026500
//@@ 250026501	专款专用帐目成员入HASH树出错			检查是否有足够的内存

#define	MBC_RT_QRY				250027000
//@@ 250027001	中间件错误,取SERV_ID个数出错			检查传入的BUFF值
//@@ 250027002	分配空间出错							检查是否有足够的内存
//@@ 250027003	中间件错误,取SERV_ID出错				检查传入的BUFF值
//@@ 250027004	中间件错误,取BILLING_CYCLE_ID出错		检查传入的BUFF值

#define	MBC_RT_VIREMENT			250027500
//@@ 250027501	中间件错误,取ACCT_ID个数出错			检查传入的BUFF值
//@@ 250027502	分配空间出错							检查是否有足够的内存
//@@ 250027503	中间件错误,取ACCT_ID出错				检查传入的BUFF值

#define	MBC_RT_VIRE_ROLL		250027800
//@@ 250027801	分配空间出错							检查是否有足够的内存
//@@ 250027802	立即结帐表中不存在此帐目				检查ACCT_ITEM_OWE表实时数据是否与RT_VIRE_ACCT_ITEM表数据匹配
//@@ 250027803	立即结帐表数据和欠费表数据不等			检查ACCT_ITEM_OWE表实时数据是否与RT_VIRE_ACCT_ITEM表数据匹配

#define MBC_ProcessAgentServer				250028000
//@@ 250028000	读取配置信息错		检查etc/process_agent.ini中,[process_id]下LOCAL_PORT的配置
//@@ 250028001	Socket--listen错	检查etc/process_agent.ini中,[process_id]下LOCAL_PORT的配置
//@@ 250028002	读取客户端数据错	可能客户端进程已退出
#define MBC_ProcessAgentClient				250029000
//@@ 250029000	读取配置信息错		检查etc/process_agent.ini中,[process_id]下REMOTE_IP,REMOTE_PORT的配置
//@@ 250029001	连接远端Socket错	服务端是否启动,检查etc/process_agent.ini,[process_id]下REMOTE_IP,REMOTE_PORT的配置

#define MBC_FormulaMgr				250030000
//@@ 250030000	公式执行错误  		请查看log 
//@@ 250030001	公式执行错误  		请查看log 

#define	MBC_PutInFile				250031000
//@@ 250031000	找不到配置数据	检查b_event_to_file_cfg表有没有当前process_id的配置数据
//@@ 250031001	找不到需要入库的字段的配置数据	检查b_event_to_file_field_cfg表, 有没有当前process_id的配置
//@@ 250031001	打开文件失败	检查b_event_to_file_cfg表的文件名, 测试该文件是否可写

#define MBC_LoadOwe					250032000
//@@ 250032001	分配共享内存(往月欠费)时出错		请检查是否有足够的内存
//@@ 250032002	共享内存(往月欠费)不存在			请重启进程
//@@ 250032003	往月欠费索引段溢出				请将ACCTITEMOWEBUF_INDEX_COUNT的值改大
//@@ 250032004	往月欠费数据段溢出				请将ACCTITEMOWEBUF_DATA_COUNT的值改大
//@@ 250032005	参数不正确				请修改参数后重启进程

#define MBC_OweMgr			250033000
//@@ 250033001	无法连接共享内存(往月欠费)	请先运行往月欠费上载程序
//@@ 250033002	共享内存(往月欠费)不存在	请先运行往月欠费上载程序
//@@ 250033003	分配共享内存(往月欠费)时出错		请检查是否有足够的内存
//@@ 250033004	共享内存(往月欠费)不存在			请重启进程

#define MBC_StateConvertMgr		250034000

#define MBC_PlanType			250035000

#define MBC_OfferMgr            250036000
//@@ 250036001  分配空间出错            检查是否有足够的内存

#define MBC_Rebill            260010000
//@@ 260010001	任务状态非法		检查表B_REBILL_TASK中TASK_STEP
//@@ 260010002	进程状态非法		检查进程框架:进程没有分配框架共享内存,居然在运行中...
//@@ 260010003	参数没有配置		检查表B_PARAM_DEFINE中配置回退的表类型


#define MBC_FileDB            270010000
//@@ 270010001		找不到合适的org		话单org找不到合适的org level为200的org_id
//@@ 270010002  	写磁盘文件失败..	请检查磁盘是否有足够的空间
//@@ 270010003  	写文件PackageID获取失败 	异常失败,请重启程序试试
//@@ 270010004  	连接入库共享内存失败 	检查数据库b_ipc_cfg配置是否正确
//@@ 270010005  	找不到表类型 	请在ratable_event_type表里面配置记录


#define MBC_Format            280010000
//@@ 280010001		按用户积压设置复位标志失败		SetServBlockFlag(0)返回false，按用户积压设置复位标志失败


#define MBC_CheckdupDel            290010000
//@@ 290010001		CheckDup::CheckDupDel 申请 TicketFileBackStruct内存出错		CheckDup::CheckDupDel 申请 TicketFileBackStruct内存出错

#define MBC_RedoEvents            290030000
//@@ 290030001		RedoEvents::redoMain 申请 m_ohSwitchInfo内存出错		RedoEvents::redoMain 申请 m_ohSwitchInfo内存出错


#define MBC_AcctItemAccu       290020000
//@@ 290020001  无法连接总帐共享内存    请执行总帐内存管理程序ctlacctitem
//@@ 290020002  无法申请内存            请扩大内存:)
//@@ 290020003  无法分配总帐内存        请调整AcctItemCount(b_param_define)

#define MBC_IpcKey             290040000
//@@ 290040001  无法获取HOST_ID         请检查wf_hostinfo中的信息是否配置正确
//@@ 290040002  无法获取IPC_KEY         请检查B_IPC_CFG表的配置是否完整
//@@ 290040003	无法设置IPC_KEY			请检查IPC_NAME不可以为空

#define MBC_PricingInfoLoad    290050000
//@@ 290050001  申请内存失败            请检查主机是否有足够的内存
//@@ 290050002  预申请空间不够          B_WHOLE_DISCOUNT变化中，重新启动程序

#define MBC_OfferAccuMgr       290060000
//@@ 290060001  初始化AccuMgr失败       检查代码
//@@ 290060002  获取seq_accu_inst失败   请检查数据库序列seq_accu_inst
//@@ 290060003  获取seq_accu_cycle失败  请检查数据库序列seq_accu_cycle
//@@ 290060004  申请累积内存失败        请检查主机是否有足够的内存
//@@ 290060005  申请周期内存失败        请检查主机是否有足够的内存
//@@ 290060006  累积回收临时缓存失败	 请检查主机是否有足够的内存

#define MBC_ParamDefineMgr     290070000
//@@ 290070001  获取HOST_ID失败         请检查wf_hostinfo中配置是否正确
//@@ 290070002  获取参数失败            请检查b_param_define表的配置是否完整
//@@ 290070003	删除参数失败			请检查需要删除参数配置的PARAM_SEGMENT或PARAM_CODE不可以为空
//@@ 290070004	配置参数失败			请检查需要配置参数配置的PARAM_SEGMENT或PARAM_CODE或PARAM_VALUE不可以为空

#define MBC_FilterRule         290080000
//@@ 290080001  申请内存失败            请检查是否有足够的内存
//@@ 290080002  加载参数失败            请调整MAX_LONG_TYPE

#define MBC_AccuTypeCfg        290090000
//@@ 290090001  周期类型配置错误        此种开始周期类型不应该配置此类型的偏移量

#define MBC_CheckRule          290090000
//@@ 290090001	申请内存失败	请检查是否有足够的内存
//@@ 290090002	申请内存失败	有增加数据，请重新启动程序
#define MBC_CheckSubRule       290091000
//@@ 290091001	申请内存失败	请检查是否有足够的内存
//@@ 290091002	申请内存失败	有增加数据，请重新启动程序
#define MBC_CheckArgument      290092000
//@@ 290092001	申请内存失败	请检查是否有足够的内存
//@@ 290092002	申请内存失败	有增加数据，请重新启动程序
#define MBC_CheckRelation      290093000
//@@ 290093001	申请内存失败	请检查是否有足够的内存
//@@ 290093002	申请内存失败	有增加数据，请重新启动程序
#define MBC_TransFileType      290094000
//@@ 290094001	申请内存失败	请检查是否有足够的内存
//@@ 290094002	申请内存失败	有增加数据，请重新启动程序
#define MBC_TagSeq             290094100
//@@ 290094101	申请内存失败	请检查是否有足够的内存
//@@ 290094102	申请内存失败	有增加数据，请重新启动程序
#define MBC_SegmentMgr			290094200
//@@ 290094201	申请内存失败	请检查是否有足够的内存
//@@ 290094202	申请内存失败	有增加数据，请重新启动程序

#define MBC_SpecailNbrStrategyMgr  310010000
//@@ 310010001	审请建树空间失败	请检查主机内存是否足够

#define MBC_TransTermFile	310020000
//@@ 310020001  获取当月补贴文件个数超过一个
//@@ 310020002  获取对应账期失败
//@@ 310020003  文件校验出错，包括大小，日期，省代码的校验
//@@ 310020004  读取文件号码信息出错
//@@ 310020004  读取参数配置出错，缺少参数配置

#define MBC_ParamShmMgr     310030000
//@@ 310030001	共享内存配置错误		参数访问入口的共享内存KEY配置错误
//@@ 310030002	删除共享内存失败		该共享内存编号不存在
//@@ 310030003	分离共享内存失败		该共享内存编号不存在
//@@ 310030004	分配共享内存失败		请检查是否有足够的内存
//@@ 310030005	分配共享内存失败		该共享内存编号不存在
//@@ 310030006	删除共享内存失败		其他进程正在使用,请先停止其他进程
//@@ 310030007	获取共享内存的连接进程数失败		共享内存可能不存在

#define MBC_ParamInfoInterface     310040000
//@@ 310040001	参数共享内存不存在		请先创建共享内存

#define MBC_ParamInfo     310020000
//@@ 310040001	初始化的配置参数不全		无法初始化数据库中的配置参数

#define MBC_ParamInfoMgr     310050000
//@@ 310050001	获取参数失败			系统未配置条件参数的内存大小
//@@ 310050002	申请内存失败			条件参数数据区空间不够，请修改配置重新上载资料
//@@ 310050003	获取参数失败			系统未配置运算参数的内存大小
//@@ 310050004	申请内存失败			运算参数数据区空间不够，请修改配置重新上载资料

//@@ 310050005	获取参数失败			系统参数中未配置段落参数的内存大小
//@@ 310050006	申请内存失败			段落参数数据区空间不够，请修改配置重新上载资料
//@@ 310050007	查找参数失败			段落索引匹配失败，对应此ID的段落信息不存在

//@@ 310050008	获取参数失败			系统参数中未配置资费参数的内存大小
//@@ 310050009	申请内存失败			资费参数数据区空间不够，请修改配置重新上载资料
//@@ 310050010	查找参数失败			资费索引匹配失败，对应此ID的信息不存在

//@@ 310050011	获取参数失败			系统参数中未配置优惠参数的内存大小
//@@ 310050012	申请内存失败			优惠参数数据区空间不够，请修改配置重新上载资料
//@@ 310050013	查找参数失败			优惠索引匹配失败，对应此ID的信息不存在

//@@ 310050014	获取参数失败			系统参数中未配置优惠分配者参数的内存大小
//@@ 310050015	申请内存失败			优惠分配者参数数据区空间不够，请修改配置重新上载资料
//@@ 310050016	查找参数失败			优惠分配者索引匹配失败，对应此ID的信息不存在

//@@ 310050017	获取参数失败			系统参数中未配置优惠分配对象参数的内存大小
//@@ 310050018	申请内存失败			优惠分配对象参数数据区空间不够，请修改配置重新上载资料
//@@ 310050019	查找参数失败			优惠分配对象索引匹配失败，对应此数据的信息不存在

//@@ 310050020	获取参数失败			系统参数中未配置折扣参数的内存大小
//@@ 310050021	申请内存失败			折扣参数数据区空间不够，请修改配置重新上载资料
//@@ 310050022	查找参数失败			折扣索引匹配失败，对应此数据的信息不存在

//@@ 310050023	获取参数失败			系统参数中未配置生命周期参数的内存大小
//@@ 310050024	申请内存失败			生命周期参数数据区空间不够，请修改配置重新上载资料
//@@ 310050025	查找参数失败			生命周期索引匹配失败，对应此数据的信息不存在

//@@ 310050026	获取参数失败			系统参数中未配置度量参数的内存大小
//@@ 310050027	申请内存失败			度量参数数据区空间不够，请修改配置重新上载资料
//@@ 310050028	查找参数失败			度量索引匹配失败，对应此数据的信息不存在

//@@ 310050029	获取参数失败			系统参数中未配置ConditionPoint(条件点)参数的内存大小
//@@ 310050030	申请内存失败			ConditionPoint(条件点)参数数据区空间不够，请修改配置重新上载资料
//@@ 310050031	查找参数失败			ConditionPoint(条件点)索引匹配失败，对应此数据的信息不存在

//@@ 310050032	获取参数失败			系统参数中未配置FilterCond(过滤条件)参数的内存大小
//@@ 310050033	申请内存失败			FilterCond(过滤条件)参数数据区空间不够，请修改配置重新上载资料
//@@ 310050034	查找参数失败			FilterCond(过滤条件)索引匹配失败，对应此数据的信息不存在

//@@ 310050035	获取参数失败			系统参数中未配置组合参数的内存大小
//@@ 310050036	申请内存失败			通用组合参数数据区空间不够，请修改配置重新上载资料
//@@ 310050037	查找参数失败			通用组合索引匹配失败，对应此数据的信息不存在
//@@ 310050038	申请内存失败			CheckRule参数数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_CHECKRULE_RULE_INFO_NUM)
//@@ 310050039	查找参数失败			CheckRule组合索引匹配失败，对应此数据的信息不存在(LOADPARAMINFO, PARAM_CHECKRULE_RULE_INFO_NUM)
//@@ 310050040	申请内存失败			CheckSubRule参数数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_CHECKRULE_SUBRULE_INFO_NUM)
//@@ 310050041	查找参数失败			CheckSubRule组合索引匹配失败，对应此数据的信息不存在(LOADPARAMINFO, PARAM_CHECKRULE_SUBRULE_INFO_NUM)
//@@ 310050042	申请内存失败			Argument组合参数数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_CHECKARGUMENT_INFO_NUM)
//@@ 310050043	查找参数失败			Argument组合索引匹配失败，对应此数据的信息不存在(LOADPARAMINFO, PARAM_CHECKARGUMENT_INFO_NUM)


//@@ 310050044	获取参数失败			系统参数中未配置组合关系参数的内存大小
//@@ 310050045	申请内存失败			组合关系参数数据区空间不够，请修改配置重新上载资料
//@@ 310050046	查找参数失败			组合关系索引匹配失败，对应此数据的信息不存在

//@@ 310050047	获取参数失败			系统参数中未配置TimeCond(时间分段条件)参数的内存大小
//@@ 310050048	申请内存失败			通用TimeCond(时间分段条件)参数数据区空间不够，请修改配置重新上载资料
//@@ 310050049	查找参数失败			通用TimeCond(时间分段条件)索引匹配失败，对应此数据的信息不存在

//@@ 310050050	获取参数失败			系统参数中未配置EventTypeCond参数的内存大小
//@@ 310050051	申请内存失败			通用EventTypeCond参数数据区空间不够，请修改配置重新上载资料
//@@ 310050052	查找参数失败			通用EventTypeCond索引匹配失败，对应此数据的信息不存在

//@@ 310050053	获取参数失败			系统参数中未配置CycleType(周期类型)参数的内存大小
//@@ 310050054	申请内存失败			通用CycleType(周期类型)参数数据区空间不够，请修改配置重新上载资料
//@@ 310050055	检查参数失败			通用CycleType(周期类型)check失败，对应此数据的信息不存在

//@@ 310050056	获取参数失败			系统参数中未配置AccuType参数的内存大小
//@@ 310050057	申请内存失败			通用AccuType参数数据区空间不够，请修改配置重新上载资料
//@@ 310050058	查找参数失败			通用AccuType索引匹配失败，对应此数据的信息不存在

//@@ 310050059	获取参数失败			系统参数中未配置OfferAccuRelation(商品和累积类型对应关系)参数的内存大小
//@@ 310050060	申请内存失败			通用OfferAccuRelation(商品和累积类型对应关系)参数数据区空间不够，请修改配置重新上载资料
//@@ 310050061	查找参数失败			通用OfferAccuRelation(商品和累积类型对应关系)索引匹配失败，对应此数据的信息不存在

//@@ 310050062	获取参数失败			系统参数中未配置OfferAccuExist参数的内存大小
//@@ 310050063	查找参数失败			通用OfferAccuExist索引匹配失败，对应此数据的信息不存在

//@@ 310050064	获取参数失败			系统参数中未配置SpecialCycleTypee参数的内存大小
//@@ 310050065	查找参数失败			通用SpecialCycleType索引匹配失败，对应此数据的信息不存在
//@@ 310050066	获取参数失败			系统参数中未配商品<->组合参数的内存大小
//@@ 310050067	查找参数失败			商品<->组合索引匹配失败，对应此数据的信息不存在
//@@ 310050068	获取参数失败			系统参数中未配产品<->组合参数的内存大小
//@@ 310050069	查找参数失败			产品<->组合索引匹配失败，对应此数据的信息不存在
//@@ 310050070	获取参数失败			系统参数中未配事件参数的内存大小
//@@ 310050071	申请内存失败			通用事件参数数据区空间不够，请修改配置重新上载资料
//@@ 310050072	获取参数失败			系统参数中未配Split(分隔数据)参数的内存大小
//@@ 310050073	申请内存失败			Split(分隔数据)参数数据区空间不够，请修改配置重新上载资料
//@@ 310050074	获取参数失败			系统参数中未配特殊号码策略参数的内存大小
//@@ 310050075	申请内存失败			特殊号码策略参数数据区空间不够，请修改配置重新上载资料
//@@ 310050076	获取参数失败			系统参数中未配虚拟组合参数的内存大小
//@@ 310050077	申请内存失败			虚拟组合参数数据区空间不够，请修改配置重新上载资料
//@@ 310050078	获取参数失败			系统参数中未配CG组合参数的内存大小
//@@ 310050079	申请内存失败			CG组合参数数据区空间不够，请修改配置重新上载资料
//@@ 310050080	获取参数失败			系统参数中未配置特殊资费参数的内存大小
//@@ 310050081	申请内存失败			特殊资费参数数据区空间不够，请修改配置重新上载资料
//@@ 310050082	获取参数失败			系统参数中未配置特殊资费CHECK参数的内存大小
//@@ 310050083	申请内存失败			特殊资费CHECK参数数据区空间不够，请修改配置重新上载资料
//@@ 310050084	获取参数失败			系统参数中未配置SpInfo参数的内存大小(LOADPARAMINFO, PARAM_SPINFOSEEKINFO_NUM)
//@@ 310050085	申请内存失败			SpInfo参数数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_SPINFOSEEKINFO_NUM)
//@@ 310050086	获取参数失败			系统参数中未配置GatherTask参数的内存大小(LOADPARAMINFO, PARAM_GATHERDATAINFO_NUM)
//@@ 310050087	申请内存失败			GatherTask参数数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_GATHERDATAINFO_NUM)
//@@ 310050088	获取参数失败			系统参数中未配置GatherTaskList参数的内存大小(LOADPARAMINFO, PARAM_GATERTASKLISTINFO_NUM)
//@@ 310050089	申请内存失败			GatherTaskList参数数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_GATERTASKLISTINFO_NUM)
//@@ 310050090	获取参数失败			系统参数中未配置ProductAcctItem参数的内存大小
//@@ 310050091	获取参数失败			系统参数中未配置PROIDPRODUCTACCTITEM参数的内存大小
//@@ 310050092	获取参数失败			系统参数中未配置LongTypeRule参数的内存大小(LOADPARAMINFO, PARAM_LONGTYPE_INFO_NUM)
//@@ 310050093	获取参数失败			系统参数中未配置HOST参数的内存大小(LOADPARAMINFO,PARAM_HOSTINFOINFO_NUM)
//@@ 310050094	申请内存失败			HOST参数数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO,PARAM_HOSTINFOINFO_NUM)

//@@ 310050095	获取参数失败			系统参数中未配置CheckRelation参数的内存大小(LOADPARAMINFO, PARAM_CHECKRELATION_INFO_NUM)
//@@ 310050096	申请内存失败			CheckRelation参数数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_CHECKRELATION_INFO_NUM)
//@@ 310050097	获取参数失败			系统参数中未配置TransFileType参数的内存大小(LOADPARAMINFO, PARAM_TRANSFILE_INFO_NUM)
//@@ 310050098	申请内存失败			TransFileType参数数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_TRANSFILE_INFO_NUM)
//@@ 310050099	获取参数失败			系统参数中未配置xx参数的内存大小
//@@ 310050100	申请内存失败			xx参数数据区空间不够，请修改配置重新上载资料


//@@ 310050101	获取参数失败			系统未配置ORG的内存大小
//@@ 310050102	申请内存失败			ORG数据区空间不够，请修改配置重新上载资料
//@@ 310050103	获取参数失败			系统未配置OFFER的内存大小(LOADPARAMINFO,PARAM_OFFER_NUM)
//@@ 310050104	申请内存失败			OFFER数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO,PARAM_OFFER_NUM)
//@@ 310050105	获取参数失败			系统未配置OFFERSEPCIALATTR的内存大小(LOADPARAMINFO, PARAM_OFFER_SPECATTR_NUM)
//@@ 310050106	申请内存失败			OFFERSEPCIALATTR数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_OFFER_SPECATTR_NUM)
//@@ 310050107	获取参数失败			系统未配置OFFERNUMBERATTR的内存大小(LOADPARAMINFO, PARAM_OFFER_NUMATTR_NUM)
//@@ 310050108	申请内存失败			OFFERNUMBERATTR数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_OFFER_NUMATTR_NUM)
//@@ 310050109	获取参数失败			系统未配置OFFERFREE的内存大小(LOADPARAMINFO, PARAM_OFFER_FREE_NUM)
//@@ 310050110	申请内存失败			OFFERFREE数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_OFFER_FREE_NUM)
//@@ 310050111	获取参数失败			系统未配置PRODUCT的内存大小
//@@ 310050112	申请内存失败			PRODUCT数据区空间不够，请修改配置重新上载资料
//@@ 310050113	获取参数失败			系统未配置OFFERATTR的内存大小
//@@ 310050114	申请内存失败			OFFERATTR数据区空间不够，请修改配置重新上载资料
//@@ 310050115	获取参数失败			系统未配置ACCTITEM的内存大小(LOADPARAMINFO, PARAM_ACCTITEM_NUM)
//@@ 310050116	申请内存失败			ACCTITEM数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_ACCTITEM_NUM)
//@@ 310050117	获取参数失败			系统未配置ACCTITEMMEMBERS的内存大小(LOADPARAMINFO, PARAM_ACCTITEMMEMBERS_NUM)
//@@ 310050118	获取参数失败			系统未配置ACCTITEMGROUPS的内存大小(LOADPARAMINFO, PARAM_ACCTITEMGROUPS_NUM)
//@@ 310050119	申请内存失败			ACCTITEMMEMBERS数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_ACCTITEMGROUPS_NUM)
//@@ 310050120	获取参数失败			系统未配置ACCTITEMMEMBERSB的内存大小(LOADPARAMINFO, PARAM_ACCTITEMMEMBERSB_NUM)
//@@ 310050121	获取参数失败			系统未配置ACCTITEMGROUPSB的内存大小(LOADPARAMINFO, PARAM_ACCTITEMGROUPSB_NUM)
//@@ 310050122	申请内存失败			ACCTITEMMEMBERSB数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_ACCTITEMMEMBERSB_NUM)
//@@ 310050123	获取参数失败			系统未配置AREAORGRELATION的内存大小(LOADPARAMINFO, PARAM_ACCTITEMGROUPSB_NUM)
//@@ 310050124	申请内存失败			AREAORGRELATION数据区空间不够，请修改配置重新上载资料
//@@ 310050125	获取参数失败			系统未配置EVENTTYPERULE的内存大小
//@@ 310050126	申请内存失败			EVENTTYPERULE数据区空间不够，请修改配置重新上载资料
//@@ 310050127	获取参数失败			系统未配置FORMATGROUP的内存大小
//@@ 310050128	获取参数失败			系统未配置FORMATSTEP的内存大小
//@@ 310050129	获取参数失败			系统未配置FORMATRULE的内存大小
//@@ 310050130	获取参数失败			系统未配置FORMATCOMB的内存大小
//@@ 310050131	申请内存失败			FORMATGROUP数据区空间不够，请修改配置重新上载资料
//@@ 310050132	申请内存失败			FORMATSTEP数据区空间不够，请修改配置重新上载资料
//@@ 310050133	申请内存失败			FORMATRULE数据区空间不够，请修改配置重新上载资料
//@@ 310050134	申请内存失败			FORMATCOMB数据区空间不够，请修改配置重新上载资料
//@@ 310050135	获取参数失败			系统未配置ZONE_ITEM的内存大小(LOADPARAMINFO, PARAM_ZONE_ITEM_NUM)
//@@ 310050136	申请内存失败			ZONE_ITEM数据区空间不够，请修改配置重新上载资料(LOADPARAMINFO, PARAM_ZONE_ITEM_NUM)
//@@ 310050137	获取参数失败			系统未配置BAMBOTHPRODUCT的内存大小
//@@ 310050138	申请内存失败			BAMBOTHPRODUCT数据区空间不够，请修改配置重新上载资料
//@@ 310050139	获取参数失败			系统未配置BAMGETTARIFF的内存大小
//@@ 310050140	申请内存失败			BAMGETTARIFF数据区空间不够，请修改配置重新上载资料
//@@ 310050141	获取参数失败			系统未配置BAMPRODTARIFF的内存大小
//@@ 310050142	申请内存失败			BAMPRODTARIFF数据区空间不够，请修改配置重新上载资料
//@@ 310050143	申请内存失败			账目组参数读取失败
//@@ 310050144	获取参数失败			系统未配置SwitchInfo的内存大小
//@@ 310050145	申请内存失败			SwitchInfo数据区空间不够，请修改配置重新上载资料
//@@ 310050180  获取参数失败                    系统未配置FilePoolInfo的内存大小PARAM_FILEPOOLINFO_NUM
//@@ 310050181  申请内存失败                    FilePoolInfo数据空间不足，请修改PARAM_FILEPOOLINFO_NUM重新上载 
//@@ 310050182  获取参数失败                    系统未配置FilePoolCondition的内存大小PARAM_FILEPOOLCOND_NUM 
//@@ 310050183  申请内存失败                    FilePoolCondition数据空间不足，请修改PARAM_FILEPOOLCOND_NUM重新上载 
//@@ 310050184  获取参数失败                    系统未配置FileTypeConfig的索引内存大小PARAM_FILETYPECFG_NUM 
//@@ 310050185  获取参数失败                    系统未配置Encode(FileAttr)的内存大小PARAM_TLVENCODE_NUM 
//@@ 310050186  申请内存失败                    Encode(FileAttr)数据空间不足，请修改PARAM_TLVENCODE_NUM重新上载 
//@@ 310050187  获取参数失败                    系统未配置Decode(FileAttr)的内存大小PARAM_TLVDECODE_NUM 
//@@ 310050188  申请内存失败                    Decode(FileAttr)数据空间不足，请修改PARAM_TLVDECODE_NUM重新上载 
//@@ 310050189  获取参数失败                    系统未配置FileParseRule的内存大小PARAM_FILEPARSERULE_NUM 
//@@ 310050190  获取参数失败                    系统未配置SwitchCode的内存大小PARAM_SWITCHCODE_NUM 
//@@ 310050191  申请内存失败                    SwitchCode数据空间不足，请修改PARAM_SWITCHCODE_NUM重新上载 
//@@ 310050192  获取参数失败                    系统未配置BillingNodeInfo的内存大小PARAM_BILLINGNODEINFO_NUM 
//@@ 310050193  申请内存失败                    BillingNodeInfo数据空间不足，请修改PARAM_BILLINGNODEINFO_NUM重新上载 
//@@ 310050197	申请内存失败			标准数据区描述区空间不足,未能申请到空间,请修改配置重新上载资料
//@@ 310050198	获取参数失败			PARAM_TEMPLATEMAPSWITCH_NUM
//@@ 310050199	申请内存失败			标准数据区不足,请修改配置重新上载资料
//@@ 310050200	申请内存失败			SwitchInfo数据区空间不够，请修改配置重新上载资料
//@@ 310050201  申请内存失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_NBR_MAP_NUM)的大小
//@@ 310050202  获取参数失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_NBR_MAP_NUM)的大小
//@@ 310050203  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_LONGEVENTTYPERULE_NUM)的大小
//@@ 310050204  获取参数失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_LONGEVENTTYPERULE_NUM)的大小
//@@ 310050205  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_LONGEVENTTYPERULEEX_NUM)的大小
//@@ 310050206  获取参数失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_LONGEVENTTYPERULEEX_NUM)的大小
//@@ 310050207  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_PRICINGPLANTYPE_NUM)的大小
//@@ 310050208  获取参数失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_PRICINGPLANTYPE_NUM)的大小
//@@ 310050209  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_STATECONVERT_NUM)的大小
//@@ 310050210  获取参数失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_STATECONVERT_NUM)的大小
//@@ 310050211  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_SPLITACCTGROUP_NUM)的大小
//@@ 310050212  获取参数失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_SPLITACCTGROUP_NUM)的大小
//@@ 310050213  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_SPLITACCTEVENT_NUM)的大小
//@@ 310050214  获取参数失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_SPLITACCTEVENT_NUM)的大小
//@@ 310050215  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_DISTANCETYPE_NUM)的大小
//@@ 310050216  获取参数失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_DISTANCETYPE_NUM)的大小
//@@ 310050217  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_EDGEROAMRULEEX_NUM)的大小
//@@ 310050218  获取参数失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_EDGEROAMRULEEX_NUM)的大小
//@@ 310050219  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_HEADEX_NUM)的大小
//@@ 310050220  获取参数失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_HEADEX_NUM)的大小
//@@ 310050221  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_SEGMENTINFO_INFO_NUM)的大小
//@@ 310050222  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_SEGMENTINFO_INFO_NUM)的大小
//@@ 310050223  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_LOCALHEAD_NUM)的大小
//@@ 310050224  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_LOCALHEAD_NUM)的大小
//@@ 310050225  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_EVENTTYPE_NUM)的大小
//@@ 310050226  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_EVENTTYPE_NUM)的大小
//@@ 310050227  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_EVENTTYPEGROUP_NUM)的大小
//@@ 310050228  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_EVENTTYPEGROUP_NUM)的大小
//@@ 310050229  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_VALUEMAP_NUM)的大小
//@@ 310050230  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_VALUEMAP_NUM)的大小
//@@ 310050231  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO,PARAM_ROAMCARRIER_NUM)的大小
//@@ 310050232  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO,PARAM_ROAMCARRIER_NUM)的大小 
//@@ 310050233  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO,PARAM_ROAMTARIFF_NUM)的大小
//@@ 310050234  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO,PARAM_ROAMTARIFF_NUM)的大小 
//@@ 310050235  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO,PARAM_ROAMSPONSOR_NUM)的大小
//@@ 310050236  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO,PARAM_ROAMSPONSOR_NUM)的大小 
//@@ 310050237  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO,PARAM_SPECIALGROUPMEMBER_NUM)的大小
//@@ 310050238  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO,PARAM_SPECIALGROUPMEMBER_NUM)的大小 
//@@ 310050239  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO,PARAM_TARIFFSECTOR_NUM)的大小
//@@ 310050240  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO,PARAM_COUNTRY_NUM)的大小
//@@ 310050241  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO,PARAM_COUNTRY_NUM)的大小 

//@@ 310050300  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_TEMPLATEMAPSWITCH_NUM)的大小
//@@ 310050301  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_TEMPLATEMAPSWITCH_NUM)的大小

//@@ 310050302  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_OBJECTDEFINE_NUM)的大小
//@@ 310050303  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_OBJECTDEFINE_NUM)的大小

//@@ 310050304  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_EVENTPARSERGROUP_NUM)的大小
//@@ 310050305  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_EVENTPARSERGROUP_NUM)的大小

//@@ 310050306  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_FIELD_SHM_NUM)的大小
//@@ 310050307  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_TEMPLATE_RELATION_SHM_NUM)的大小
//@@ 310050308  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_TEMPLATE_RELATION_SHM_NUM)的大小
//@@ 310050309  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_FIELD_SHM_NUM)的大小

//@@ 310050310  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_SWITCH_TYPE_INFO_NUM)的大小
//@@ 310050311  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_SWITCH_TYPE_INFO_NUM)的大小

//@@ 310050312  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_EVENT_ATTR_INFO_NUM)的大小
//@@ 310050313  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_EVENT_ATTR_INFO_NUM)的大小

//@@ 310050314  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_EVENTHEAD_COLUMUN_INFO_NUM)的大小
//@@ 310050315  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_EVENTHEAD_COLUMUN_INFO_NUM)的大小

//@@ 310050316  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_SWITICHIDMAPSWITCHINFO_INFO_NUM)的大小
//@@ 310050317  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_SWITICHIDMAPSWITCHINFO_INFO_NUM)的大小

//@@ 310050318  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_FIELD_CHECK_INFO_NUM)的大小
//@@ 310050319  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_FIELD_CHECK_INFO_NUM)的大小

//@@ 310050320  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_FIELD_MAP_INFO_NUM)的大小
//@@ 310050321  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_FIELD_MAP_INFO_NUM)的大小

//@@ 310050322  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_FIELD_MERGE_INFO_NUM)的大小
//@@ 310050323  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_FIELD_MERGE_INFO_NUM)的大小

//@@ 310050324  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_TAP3_FILE_INFO_NUM)的大小
//@@ 310050326  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_TAP3_FILE_INFO_NUM)的大小

//@@ 310050325  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_TAP3_BLOCK_INFO_NUM)的大小
//@@ 310050327  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_TAP3_BLOCK_INFO_NUM)的大小

//@@ 310050328  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_SEP_FILE_INFO_NUM)的大小
//@@ 310050330  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_SEP_FILE_INFO_NUM)的大小

//@@ 310050329  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_SEP_BLOCK_INFO_NUM)的大小
//@@ 310050331  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_SEP_BLOCK_INFO_NUM)的大小

//@@ 310050339  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_MERGE_RULE_INFO_NUM)的大小
//@@ 310050340  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_MERGE_RULE_INFO_NUM)的大小
//@@ 310050401  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_VISIT_AREAVODE_INFO_NUM)的大小
//@@ 310050402  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_VISIT_AREAVODE_INFO_NUM)的大小
//@@ 310050403  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_EDGEROAMRULE_INFO_NUM)的大小
//@@ 310050404  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_EDGEROAMRULE_INFO_NUM)的大小
//@@ 310050405  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_PNSEGBILLING_INFO_NUM)的大小
//@@ 310050406  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_PNSEGBILLING_INFO_NUM)的大小
//@@ 310050407  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_TRUNKTREENODE_INFO_NUM)的大小
//@@ 310050408  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_TRUNKTREENODE_INFO_NUM)的大小
//@@ 310050409  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_PNSEGBILLING_INFO_NUM)的大小
//@@ 310050410  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_PNSEGBILLING_INFO_NUM)的大小
//@@ 310050411  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_PNNOTBILLING_INFO_NUM)的大小
//@@ 310050412  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_PNNOTBILLING_INFO_NUM)的大小
//@@ 310050413  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_CARRIER_NUM)的大小
//@@ 310050414  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_SERVICE_INFO2_NUM)的大小
//@@ 310050415  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_SERVICE_INFO2_NUM)的大小
//@@ 310050416  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_TRUNKBILLING_NUM)的大小
//@@ 310050417  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_TRUNKBILLING_NUM)的大小
//@@ 310050418  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_SPEC_TRUNK_PRODUCT_NUM)的大小
//@@ 310050419  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_SWITCHITEMTYPE_NUM)的大小
//@@ 310050420  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_SWITCHITEMTYPE_NUM)的大小
//@@ 310050421  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_STR_LOCALAREACODE_NUM)的大小
//@@ 310050423  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_TIMESTYPE_NUM)的大小
//@@ 310050424  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_CITYINFO_NUM)的大小
//@@ 310050425  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_CITYINFO_NUM)的大小
//@@ 310050426  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_FILTER_RESULT_NUM)的大小
//@@ 310050427  申请内存失败      请调整B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_FILTER_RESULT_NUM)的大小
//@@ 310050502  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_TRUNK_NUM)的大小
//@@ 310050503  申请内存失败      请调大B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_TRUNK_NUM)的大小
//@@ 310050504  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_AREACODE_NUM)的大小
//@@ 310050505  获取参数失败      B_PARAM_DEFINE表中没有配置(LOADPARAMINFO, PARAM_AREACODE_NUM)的大小
//@@ 310050506  申请内存失败      请调大B_PARAM_DEFINE表中(LOADPARAMINFO, PARAM_AREACODE_NUM)的大小

//# MBC_APP_BILLFEE
#define MBC_BillFeeFilterRuleMgr  320010000
//@@ 320010001	申请内存失败	请检查是否有足够的内存
//@@ 320010002	申请内存失败	TIF_FEE_BILL_FILTER_RULE表中增加了数据，请重新启动程序

#define MBC_CreditRuleMgr				350010000
//@@ 350010001	内存申请失败				(CreditRuleMgr::loadEvalIndex()--*ppEvalIndex = new EvalIndex)
//@@ 350010002	内存申请失败				(CreditRuleMgr::loadCreditFactor()--*ppCreditFactor = new CreditFactor)
//@@ 350010003	信用度因素计算方式配置有误	信用度因素计算方式必须指定为计算操作标识或计算存储过程！
//@@ 350010004	内存申请失败				(CreditRuleMgr::loadEvalPlan--*ppEvalPlan = new EvalPlan)
//@@ 350010005	内存申请失败				(CreditRuleMgr::loadEvalPlan--*ppEvalRule = new EvalRule)
//@@ 350010006	评估规则配置错误			请正确配置评估规则！
//@@ 350010007	内存申请失败				(CreditRuleMgr::loadEvalPlan--pEvalRuleStep = new EvalRuleStep)
//@@ 350010008	评估步骤配置错误			请正确配置评估步骤！
//@@ 350010009	计划执行方法配置错误		请配置合适的评估计划执行方法！
//@@ 350010010	找不到评估计划				请配置信用度/积分的评估计划！
//@@ 350010011	没有配置缺省信用度(未用)	请配置预付费用户的信用度缺省值！	
//@@ 350010012	没有配置计算信用值的帐目组	请配置计算信用值的帐目组(使用“,”分隔开)！
//@@ 350010013	未能正确获取因素值			请检查因素配置	

#define MBC_SHMMixData     350060000
//@@ 350060001	申请内存失败			数据区空间不够,请重新设置大小
//@@ 350060002	申请内存失败			此数据区空间已有一种类型数据使用了
//@@ 350060003	申请内存失败			数据类型超过设定的最大值

#define MBC_AcctItemHisOweMgr			350020000
//@@ 350020001	无法连接共享内存(前3个月欠费)				请先运行往月欠费上载程序
//@@ 350020002	共享内存(前3个月欠费)不存在 				请先运行往月欠费上载程序
//@@ 350020003	分配共享内存(前3个月欠费)时出错				请检查是否有足够的内存
//@@ 350020004	共享内存(前3个月欠费)不存在					请重启进程
//@@ 350020005  删除内存失败                                确认已停止所有连接内存的相关进程

#define MBC_AverageOweGetter			350030000
//@@ 350030001	没有选择适合的历史账期数	请选择合适的历史账期数(1~3)！
//@@ 350030002  内存帐务周期不存在          请正确加载帐务周期！

#define MBC_CreditStore					350040000
//@@ 350040001	内存申请失败				(CreditCalInfo内存分配失败)

#define MBC_CacheMgr					360010000
//@@ 360010001	内存申请失败			本地缓存区管理中的中间缓冲信息区太小，请调大后重新启动
//@@ 360010002	内存申请失败			本地缓存区管理中的中间缓冲数据区太小，请调大后重新启动
//@@ 360010003	内存总帐指针为空		本地缓存管理程序接受到的内存总帐指针为空，请检查程序和配置后重启
//@@ 360010004	内存累积量指针为空		本地缓存管理程序接受到的内存累积量指针为空，请检查程序和配置后重启
//@@ 360010005	入库进程指针为空		本地缓存管理程序接受到的入库进程为空，请检查程序和配置后重启
//@@ 360010006	锁管理进程指针为空		本地缓存管理程序接受到的锁管理进程为空，请检查程序和配置后重启
//@@ 360010007	发送消息队列指针为空	本地缓存管理程序接受到的消息队列发送指针为空，请检查程序和配置后重启
//@@ 360010008	进程ID取值错误			批价进程ID没有正常送本地缓存管理程序，请检查程序和配置后重启

#define MBC_IpcKeyMgr    370010000  
//@@ 370010001  获取配置失败       配置表中缺少配置,请检查
//@@ 370010002  KEY值超出范围      KEY值超出其在B_IPCKEY_LIMIT表中的配置,请修改

//app_rent
#define MBC_GridRentDataMgr 380010000
//@@ 380010002	租费网格共享内存删除失败	请退出其他相关进程，或者强制删除
//@@ 380010003	申请租费网格失败	租费网格共享内存耗尽，请重新分配共享内存

#define MBC_GridSetDataMgr 380020000
//@@ 380020002	套餐费网格共享内存删除失败	请退出其他相关进程，或者强制删除
//@@ 380020003	申请套餐费网格失败	套餐费网格共享内存耗尽，请重新分配共享内存

#define MBC_GridProcessMgr 380030000
//@@ 380030001	网格化租费没有配置该进程的信息	请在grid_process表里面配置该procss_id的信息

#define MBC_GridEventMgr   380040000
//@@ 380040001	网格租费或者套餐费生成类实例化超过系统限制个数	请修改宏GRID_MAKER_NUM的大小


#define MBC_SEQCHECKDUP  110192000
//  110192001	取模配置数据有误							配置取模数据
//  110192002	无序列排重基本配置数据						配置序列排重的配置数据
//  110192003	序列排重域以及序列排重ID域未配置规整规则 	规整出序列排重主键域以及序列ID域

#define MBC_NP 310060000
//@@ 310060001  获取新空间失败
//@@ 310060002  获取对象失败

#define         MBC_ERRORCODE              330010000
//@@    330010001       错误码或操作码不存在,请在MBC_SGW.h中配置
#endif
