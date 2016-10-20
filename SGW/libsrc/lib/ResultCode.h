#ifndef _INCLUDE_RESULTCODE_H_
#define _INCLUDE_RESULTCODE_H_

const int MSG_RECEIVE_FAIL = -1001;				//消息接收失败
const int STRMSG_RECEIVE_PARSER_FAIL = -1002;	//string消息接收失败
const int AVPMSG_RECEIVE_PARSER_FAIL = -1003;	//avp消息接收失败

const int MSG_SEND_FAIL = -1011;			//消息发送失败
const int STRMSG_SEND_PACK_FAIL = -1012;	//string消息发送失败
const int AVPMSG_SEND_PACK_FAIL = -1012;	//avp消息发送失败

//-----------------ABM Result Code --------------------
const int SUCCEED = 0; 					//成功
const int SYSTEM_BUSY_ERROR = 1001;		//系统忙，请稍后
const int FAIL = 1002; 					//失败
const int EVENT_TYPE_ERROR = 1501;  	//事件类型错误
const int EVENT_SOURCE_ERROR = 1502;	//事件来源源错误
const int MSG_TYPE_ERROR = 1503;		//消息类型错误string buffer或AVP buffer

//-----------------ABM Result Code --------------------

const int PARSER_FAIL = 2510;  			//消息解析失败

const int PARSER_STRBUFF_FAIL = 2511;	//string buffer消息解析失败

const int PARSER_AVPBUFF_FAIL = 2512;	//AVP buffer消息解析失败

const int PACK_FAIL = 2520;  			//消息打包失败

const int PACK_STRBUFF_FAIL = 2521;		//string buffer消息打包失败

const int PACK_AVPBUFF_FAIL = 2522;		//AVP buffer消息打包失败

#endif



