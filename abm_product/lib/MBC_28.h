#ifndef __MBC_28_H__
#define __MBC_28_H__
//#ifdef __28_TEST_

//######################################################
//# 2.8错误编码
//# 基本编码规则: 3位模块编号+2位类别编号+4位信息编号
//# 1)模块号在进程或命令集初始化时赋值给Log类
//# 2)在ALARMLOG中同时传入类别编号以及4位信息码
//# 3)根据各自实际情况，选择是否需要THROW
//######################################################

//******************************************************
//Module_ID
//******************************************************
//----------------------------------------
//Application
//----------------------------------------
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
//#define MBC_APP_CfgExp	              223
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

//----------------------------------------
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
#endif //__MBC_28_H__

