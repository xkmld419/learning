#ifndef __ERROR_CODE_H_INCLUDED_
#define __ERROR_CODE_H_INCLUDED_

#define ECODE_NOMSG    4001
#define ECODE_UNPACK_FAIL 4002
#define ECODE_PACK_FAIL   4003
#define ECODE_TT_FAIL     4004
#define QUERY_FAIL_OTHER  13005         //全国中心余额查询无记录或者多条记录
#define TT_NO_RECORD      13013
#define OVERDUE_FLG       238
#define QUERY_BILL_FAIL   188
/*
查询业务部分错误码-包括 余额查询，充退记录查询，交易记录查询，电子账单查询，
余额划拨查询
*/
#define  QRY_ECODE_NOMSG_ERR 13700       //请求消息包为空
#define  QRY_ECODE_UNPACK_FAIL 13701     //请求消息解析错误
#define  QRY_USER_TYPE_ERR     13702     //用户类型为1，目前只支持2
#define  QRY_BALANCE_NODATA_ERR 13703    //查询全国ABM余额没有数据
#define  QRY_MUL_RECORD_ERR     13704    //查询全国ABM余额有多条记录，目前只支持1条
#define  QRY_GET_SEQUENCE_FAIL_ERR 13705 //获取保存DCC头的序列号失败
#define  QRY_INSERT_DCC_FAIL_ERR 13706   //插入DCC头信息失败
#define  QRY_SAVEABMDATA_FAIL_ERR 13707  //保存全国中心ABM数据失败
#define  QRY_PACKMSG_TO_PROC_ERR  13708  //打包发送到省里的请求信息失败
#define  QRY_GET_DCC_FAIL_ERR 13709      //查询DCC消息头失败,数据库操作失败
#define  QRY_GET_DCC_NOMSG_ERR 13710     //查询DCC消息没记录，这个要命，后续不知道怎么处理
#define  QRY_GET_ABMDATA_FAIL_ERR 13711  //查询保存的全国ABM数据失败，数据库操作失败
#define  QRY_GET_ABMDATA_NODATA_ERR 13712 //查询保存的全国ABM数据无记录
#define  QRY_UNPACK_PROC_FAIL_ERR 13713  //解析省里返回的消息失败
#define  QRY_PACK_SERVICE_FAIL_ERR 13714 //打包到自服务平台的返回消息失败
//余额查询
#define  QRY_PACK_FAIL_ERR  13715        //打包余额返回结果码消息失败
#define  QRY_PACK_DATA_ERR 13716         //打包余额查询数据部分失败

//充退记录查询
#define  QRY_PAY_PACK_FAILE 13717        //充退记录查询解包失败
#define  QRY_PAY_NORECORD_ERR 13718      //充退记录查询无记录
#define  QRY_PAY_PACKRLT_FAIL 13719      //充退记录打包返回结果码失败
#define  QRY_PAY_PACKDATA_FAIL 13720     //打包充退记录数据失败

//交易记录查询
#define  QRY_REC_UNPACK_FAIL 13721       //交易请求消息记录解包失败
#define  QRY_REC_NORECORD_ERR 13722      //交易记录查询无结果
#define  QRY_REC_PACK_FAIL   13723       //交易记录打包返回结果码失败
#define  QRY_REC_PACKDATA_FAIL  13724    //交易记录打包数据失败

//电子账单查询
#define  QRY_BIL_UNPACK_FAIL 13725       //电子账单请求消息记录解包失败
#define  QRY_BIL_NORECORD_ERR 13726      //电子账单记录查询无结果
#define  QRY_BIL_PACK_FAIL   13727       //电子账单记录打包返回结果码失败
#define  QRY_BIL_PACKDATA_FAIL  13728    //电子账单记录打包数据失败
#define  QRY_BIL_PREPARE_FAIL  13729     //预处理sql失败

//余额查询
#define  QRY_BAL_UNKNOWNTYPE   13730    //未识别的用户类型
#define  QRY_BAL_SERV_NOTDIGIT 13731    //余额查询按用户ID查询，用户ID非数字
#define  QRY_BAL_NORECORD      13732    //余额查询无记录
#define  QRY_BAL_MULRECORD     13733    //余额查询多条记录


#endif/*__ERROR_CODE_H_INCLUDED_*/
