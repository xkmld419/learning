#ifndef DCC_CHECK_RECORD_H_HEADER_INCLUDED
#define DCC_CHECK_RECORD_H_HEADER_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <iostream>

#include "StdEvent.h"
#include "Process.h"
#include "EventToolKits.h"
#include "interrupt.h"
#include "AttrTransMgr.h"

using namespace std;
typedef std::map<int,int> TRANSFILEMAP;
typedef std::map<int,std::vector<int> > EVENTATTRIDMAP;

/*
解包后发送业务逻辑：(CCR解包后)
<Session-Id>|{hop_by_hop}|{end_to_end}|{Auth-Application-Id}|{CC-Request-Type}|{CC-Request-Number}|{Service-Context-Id}^^^话单1^^^话单2^^^...^^^话单n
<Session-Id>|{hop_by_hop}|{end_to_end}|{Auth-Application-Id}|{CC-Request-Type}|{CC-Request-Number}|{Service-Context-Id}^^^02|2|818233|818233|USA|USA02|0|0|20030101|20350101|20000101^^^03|2|818233|818233|USA|USA02|0|0|20030101|20350101|20000101^^^02|4|818233|818233|USA|USA02|0|0|20030101|20350101|20000101
业务逻辑处理后发送给打包：（需要打的CCA包）
<Session-Id>|{hop_by_hop}|{end_to_end}|{Auth-Application-Id}|{CC-Request-Type}|{CC-Request-Number}^^^Service-Result-Code|Para-Field-Result^^^...^^^Service-Result-Code|Para-Field-Result
<Session-Id>|{hop_by_hop}|{end_to_end}|{Auth-Application-Id}|{CC-Request-Type}|{CC-Request-Number}^^^Service-Result-Code|F50^^^Service-Result-Code|F50^^^Service-Result-Code|F20

发送给打包的ccr包字符串结构需要
<Session-Id>|{application_id}|{hop_by_hop}|{end_to_end}|{Destination-Realm}|{Auth-Application-Id}|{Service-Context-Id}|{CC-Request-Type}|{CC-Request-Number}^^^话单1^^^...^^^话单n 

*/

// 结构：存放 包头信息 
struct PackageHeadInfo
{
	string m_sSessionId;         //必选且第一个出现。会话ID，唯一标识一次DCC 会话过程
	string m_sHopByHop;          //Hop-by-Hop标识符为一个无符号32比特整数字段（按网络字节顺序），用来帮助匹配请求和响应。 发送者必须保证请求中的Hop-by-Hop标识符在特定的连接上在任何特定的时间是唯一的，并且保证该数字在经过重启动后仍然唯一。应答消息的发送者必须确保Hop-by-Hop标识符字段维持与相应的请求相同的值。 Hop-by-Hop标识符通常是单调升序的数字， 其开始值是随机生成的。 一个带有未知Hop-by-Hop标识符的应答消息必须被丢弃
	string m_sEndToEnd;          //端到端标识符是一个无符号32比特整数字段（按网络字节顺序），用来检测重复消息。 重启动实施可以设置高位12比特为包含当前时间的低位12比特， 低位20比特为随机值。请求消息的发送者必须为每一个消息插入一个唯一的标识符。 该标识符必须维持本地唯一至少4分钟，即使经过重启动。应答消息的生成者必须确保该端到端标识符字段包含与相应的请求相同的值。端到端标识符不可以被Diameter代理以任何原因修改。源主机AVP（Origin-Host）和该字段的结合可以用于检测重复。重复请求会造成相同应答的传输，并且不可以影响任何状态的设置，当处理原始请求时。应当在本地被消除的重复的应答消息将会被忽略
	string m_sAuthApplicationId; //用于重认证/授权的应用唯一标识  DCCA 4
	string m_sCCRequestType;     //发送CCR 消息的原因。会话类的消息（一个会话ID会有多个CCR的消息）：INITIAL_REQUEST 1 UPDATE_REQUEST             2      TERMINATION_REQUEST   3   事件类的消息（一个会话ID只有一个CCR）：EVENT_REQUEST 4
	string m_sCCRequestNumber;   //设置为0。如果在一个会话有多次CCR请求，按照会话的交互次数来递增	
	string m_sServiceContextId;  //一个业务能力的唯一标识，作为服务能力激活、去激活和寻址的关键字。命名规则为：<ServiceName>.<CatagoryName>@<DomainName>.ChinaTelecom.com  例如：Recharge.VC@021.ChinaTelecom.com
};



class DccCheckRecord:public Process
{
public:
	int m_iBuffSize;
	int m_iBillTypeID;                  //对应StdEvent中的m_iBillingTypeID
	string m_sSignTotal,m_sSignRecord;  //分隔符
	PackageHeadInfo m_struPackageHead;
	EventToolKits   m_evnetToolKits;
  	AttrTransMgr* m_poFormatMgr;

	char str[sizeof(StdEvent)];
	StdEvent* headStdEvent;
	string   m_seviceInformationToCCA;  //CCA串中的{Service-Information}	业务信息  部分的内容
	string   m_strToCCA;	            //打包好的CCA串
	StdEvent m_oStdEvent;
	int m_iGroup_Id;
	long m_lCurEventID;
	
	
	vector<int> vecEVENTATTRID;
	vector<int> m_vAttrId;
	TRANSFILEMAP m_mapTransFileGroupid;
	EVENTATTRIDMAP m_mapEventAttrId;
	int m_totalNum;
	
public:
	DccCheckRecord();
	~DccCheckRecord();
	
	int run();

	int analysisStr(char *cgetBuffStr);                               //功能：解析字符串
	int	DealOneRecord(string strOneRecord);
	int getEventAttr(vector<int>& vecEventAttrId,int& icount,string headString,string strBuff);
	int getGroupIdByTransFileTypeId(int iTransFileTypeId);            //根据TRANS_FILE_TYPE_ID获取GROUP_ID;查询TRANS_FILE_SERVICE_CONTEXT表,返回iGroupId
	int getEventAttrByGroupId(vector<int> &vecEventid,int iGroupId);  //根据iGroupId获取vecEventid, 表 b_asn1_field,b_asn1_tag_seq,返回获得的字段长度
	int getBuffSize();
	void getSign();                                                   //获得b_param_define表中配置的分隔符
	int checkRecord();                                                //记录级校验
	void setSessionIdToStdEvent(string strSessionId);             	  //存放session_id
	void formSeviceInformationToCCA(int errID);                       //构造CCA串中的{Service-Information}	业务信息  部分的内容
	void formStrToCCA();                                              //构造CCA串
	long getNextFileID();                                             //获取FILEID序列
	long getNextDccLogID();
	void writeDccLog(char* cLogType,char* cMessageType,int iTransFileTypeId,char* cRemark);

	void SendFileEndEvt(int iFileID);                                 //发送文件结束信号和强制提交事件
	void replaceSign(string strfield,string& resultStr );
	long getNextEventID();
	long getNextEventSeq();
	int loadTransFileGroupid();	//装载trans_file_groupid表的信息
	int loadEventAttrIdByGroupid(int igroupid);	//装载event_attr_id的信息

};


#endif /* DCC_CHECK_RECORD_H_HEADER_INCLUDED */
