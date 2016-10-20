#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

/********************************************************
*
*                    系统错误
*说明:  
*       系统错误码分布范围（0~1999）
*
*********************************************************/

#define ECODE_ABM_SUCCESS       0           //成功 

#define ECODE_ABM_TIMEOUT       1001        //系统超时

#define ECODE_ABM_FAILURE       1002        //系统错误

#define ECODE_ENV_LOST          1003        //环境变量丢失

#define ECODE_RDCFG_FAIL        1100        //客户端读取配置文件错误

#define ECODE_PARAM_INVALID     1101        //客户端参数失效

#define ECODE_MQ_OP_FAIL        1102        //客户端消息队列操作失败

#define ECODE_MQ_NOMSG          1103        //客户端消息队列为空

#define ECODE_SIGREG_FAIL       1104        //客户端信号注册异常

#define ECODE_FTP_SEND_ERR      1105        //客户端FTP发送异常

#define ECODE_FTP_RECV_ERR      1106        //客户端FTP接受异常

#define ECODE_DISPATCH_FAIL     1107        //客户端分发异常

#define ECODE_MALLOC_FAIL       1108        //客户端内存分配失败

#define ECODE_SHARE_MEM_FAIL    1109        //客户端共享内存操作失败

#define ECODE_SOCKET_EXCEPTION  1110        //SOCKET连接异常

#define ECODE_RCVMQ_BLOCK       1111        //接收消息队列溢出到缓存

#define ECODE_GETFTP_INFOR_FAIL 1112        //获取内存中FTP信息失败

#define ECODE_FTP_LOGIN_FAIL    1113        //FTP登陆失败

#define ECODE_FTP_LOAD_FAIL     1114        //FTP传输失败 

#define ECODE_ACCU_DEALFILE_FAIL 1115      //FTP文件处理失败

#define ECODE_SRV_MQ_NOMEM       1116     //服务端消息队列溢出

#define ECODE_CLNT_MQ_NOMEM      1117     //非阻塞发送消息到客户端队列失败

#define ECODE_CLNT_RCVMQ_FAIL    1118     //客户端接收消息队列失败

#define ECODE_CLNT_SNDMQ_FAIL    1119     //客户端发送消息队列失败

#define ECODE_CLNTSND_TIMEOUT    1120     //客户端发送超时
 

#define ECODE_MEM_DB_ERROR      1900        //内存数据库错误

#define ECODE_TIMEOUT           ECODE_ABM_TIMEOUT


//FTP操作返回码
#define ECODE_INIT_FTP_FAIL  1801
#define ECODE_GET_FTP_INFO   ECODE_INIT_FTP_FAIL + 1
#define ECODE_FTP_CHECK_LOCAL_DIR ECODE_GET_FTP_INFO + 1
#define ECODE_FTP_LOGIN_IN ECODE_FTP_CHECK_LOCAL_DIR + 1   //登录FTP 出错
#define ECODE_FTP_SET_ATTR ECODE_FTP_LOGIN_IN + 1          //设置FTP属性出错
#define ECODE_FTP_REMOTE_DIR ECODE_FTP_SET_ATTR + 1        //查找服务器目录出错
#define ECODE_FTP_GET_FILE_ATTR ECODE_FTP_REMOTE_DIR + 1   //获取远程文件信息失败
#define ECODE_FTP_GET_FILE ECODE_FTP_GET_FILE_ATTR + 1
#define ECODE_FTP_DEL_ROMOTE_FILE ECODE_FTP_GET_FILE + 1
#define ECODE_FTP_PUT_FILE ECODE_FTP_DEL_ROMOTE_FILE + 1
#define ECODE_FTP_DEL_LOCAL_FILE ECODE_FTP_PUT_FILE + 1

#define ECODE_SRV_GET_FTP_FAIL ECODE_FTP_DEL_LOCAL_FILE + 1



// 新增返回码 用于客户端询问ABM服务端是否执行过某条命令, 服务端m_iRltCode=ECODE_ABM_CMD_REPEAT
//表示该命令执行过(目前仅用于查询累积量稽核更新命令)
#define ECODE_ABM_CMD_REPEAT    1901         

/********************************************************
*
*                   消息错误
*说明:  
*      该错误码为集团规定，参考集团余额技术平台V1.0文档,
*      分布范围（2000~2999）
*********************************************************/

#define ECODE_MSG_FORMAT_ERROR        2000        //消息格式错误

#define ECODE_MSG_TYPE_ERROR          2001        //消息类型错误

#define ECODE_MSG_PARA_LOST           2002        //参数缺失

#define ECODE_MSG_PARA_NULL           2003        //参数为空

#define ECODE_MSG_PARA_INVALID        2004        //参数无效

#define ECODE_MSG_OVERSIZED           2005        //消息过大溢出

/********************************************************
*
*                   业务通用错误
*说明:  
*      该错误码为集团规定，参考集团余额技术平台V1.0文档,
*      分布范围（3000~3999）
*********************************************************/

#define ECODE_TRANS_ERROR          3001          //交易标识错误

/********************************************************
*                   业务错误
*说明:  
*    1 余额累积量共用错误码,分布范围（4000 ~ 4099），该
*      错误码为集团规定，参考集团余额技术平台V1.0文档;
*    2 余额返回码分布范围为（4100 ~ 4599）,该错误码为商
*      家规定；
*    3 累积量返回码分布范围为（4600 ~ 4999）,该错误码为商
*      家规定；
*
*********************************************************/

#define ECODE_SERV_NOT_EXIST          4001      //用户不存在

#define ECODE_SERV_INFO_ERROR         4002      //用户信息异常      

#define ECODE_CUST_NOT_EXIST          4003      //客户不存在      

#define ECODE_CUST_INFO_ERROR         4004      //客户信息异常     

#define ECODE_ACCT_NOT_EXIST          4005      //账户不存在       

#define ECODE_ACCT_INFO_ERROR         4006      //账户信息异常       

#define ECODE_ACCT_SERV_CUST_RELA_ERROR  4007      //用户账户客户关联错误       

#define ECODE_ADJUST_TYPE_FLAG_ERROR  4008      //重扣标识错误       
/*********************************************************
*               余额返回码
***********************************************************/

#define ECODE_NO_BALANCE           4100         //没有余额

#define ECODE_BAL_NOT_ENOUGH       4101         //预留不足成功的情况

#define ECODE_BAL_NOT_ENOUGH_FAIL  4102         //不足余额失败的情况

#define ECODE_DATA_NOTBALANCE      4103         //余额账本与来源不一致

#define ECODE_ACCT_LOCK_FAIL       4104        //ACCT共享内存操作失败

#define ECODE_BUSINESS_OVRERLOADED  4105        //业务负载过大，处理失败

#define ECODE_DATA_DIFF             4106        //数据差异

#define ECODE_FIFE_OPER_FAIL        4107        //文件操作失败

#define ECODE_FRESH_REGISTER_ACCT   4108        //ABM新注册帐户；

#define ECODE_CHECK_EM_DED          4109       //稽核月末优惠返回结果

/*********************************************************
*               累积量返回码
***********************************************************/

#define ECODE_ACCU_AUDIT_FAIL     4700          //累积量稽核失败

#define ECODE_ACCU_KEY_DISABLED   4701          //累积量主键在ABM不存在

#define ECODE_ACCU_VERSION_EXCEED 4702          //累积量版本越界

#define ECODE_ACCU_UPLOAD_FAIL    4703          //累积量上载失败

#define ECODE_ACCU_LOAD_FAIL      4704          //累积量下载失败

#define ECODE_ADTQRY_ZERO_ERROR   4705          //补稽核版本为0的参数，未查到。

#define ECODE_ADTQRY_EMPTY_ERROR   4706          //补稽核未查到任何数据

#define ECODE_ACCU_PACK_FAIL      4900          //累积量打包失败

#define ECODE_DETAIL_UPDATE_FAIL  4901          //优惠累积量明细和汇总不一致

#define ECODE_ADTQRY_PARAM_ERROR  4902          //稽核更新才查询的数据包参数错误

#define ECODE_ACCU_QRY_NULL       4903          //查询返回包为NULL 置此错误码

#define ECODE_ACCU_CLNT_UPACK_FAIL 4904         //客户端解包失败


#define ECODE_ACCU_LOAD_FREQUENT  4905          //累积量下载已执行

#define ECODE_ACCU_NO_RESOURCE    4906          //累积量服务程序资源不足 

//#define ECODE_ACCU_LOAD_FAIL      4907          //累积量下载失败

#define ECODE_ACCULOAD_UNVALID_NODE  4908         //累积量下载 无效的节点请求

#define ECODE_ACCU_BACKUP_FAIL      4909          //累积量备份失败
#define ECODE_ACCU_BACKUP_WORKING   4910          //累积量开始备份
#define ECODE_ACCUBACKUP_UNVALID_NODE 4911        // 配置文件无该节点及命令配置
#define ECODE_ACCU_BACKUP_FREQUENT  4912          //累积量备份已执行

/********************************************************
*                   内存数据库错误
*
*********************************************************/

#define MEM_DB_ERROR              8000          //内存数据库错误
#endif
