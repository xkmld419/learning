// Copyright (c) 2010,联创科技股份有限公司电信事业部
// All rights reserved.

#include "DccCheckRecord.h"
#include "Exception.h"
#include "Log.h"
#include "ParamDefineMgr.h"
#include "Environment.h"
#include "RecordCheck.h"


using namespace std;
DEFINE_MAIN (DccCheckRecord)

DccCheckRecord::DccCheckRecord()
{
	vecEVENTATTRID.clear();
	m_totalNum=0;
	m_iBillTypeID=-1;
	m_struPackageHead.m_sSessionId.clear(); 	  
	m_struPackageHead.m_sHopByHop.clear();	  
	m_struPackageHead.m_sEndToEnd.clear();	  
	m_struPackageHead.m_sAuthApplicationId.clear();
	m_struPackageHead.m_sCCRequestType.clear();  
	m_struPackageHead.m_sCCRequestNumber.clear(); 
	m_struPackageHead.m_sServiceContextId.clear();	  
	memset (&m_oStdEvent,0,sizeof(StdEvent));
	getBuffSize();
	getSign();  
	m_seviceInformationToCCA="";
	m_strToCCA="";
	m_iGroup_Id=0;
	
	headStdEvent=new StdEvent;
  	if (!headStdEvent) 
      THROW (MBC_EventFormat+1);
	
	m_poFormatMgr = new AttrTransMgr(m_iProcID);
  	if (!m_poFormatMgr) 
      THROW (MBC_EventFormat+3);


}

DccCheckRecord::~DccCheckRecord()
{
	delete headStdEvent;
	if(headStdEvent)
		headStdEvent=NULL;

	delete m_poFormatMgr;
	if(m_poFormatMgr)
		m_poFormatMgr=NULL;
}

int DccCheckRecord::run()
{
	DenyInterrupt();
	Log::log(0, "dccCheckRecord run ......");
	loadTransFileGroupid();
	
	char *cBuffer=new char[m_iBuffSize];
	memset(cBuffer, 0, m_iBuffSize);
	
	while (!GetInterruptFlag())
	{
		if(!m_poReceiver->receive(cBuffer,false))//int receive(char *buffer, bool ifBlock = true); 从消息队列接收buffer，buffer大小为StdEvent的整数size倍
		{
			sleep(2);
			continue;
		}
		m_strToCCA="";
		m_seviceInformationToCCA="";
		int iresult=analysisStr(cBuffer);
	  	if(iresult==-1)
	  	{
	      	Log::log(0,"分析字符串失败！");
	      	cout<<"分析字符串失败！"<<endl;
			AddTicketStat (TICKET_ERROR);
	      	continue;
	  	}
		AddTicketStat(TICKET_NORMAL);

	}
	delete[] cBuffer;
	return true;
}

int DccCheckRecord::analysisStr(char *cgetBuffStr)
{
	m_struPackageHead.m_sSessionId.clear(); 	  
	m_struPackageHead.m_sHopByHop.clear();	  
	m_struPackageHead.m_sEndToEnd.clear();	  
	m_struPackageHead.m_sAuthApplicationId.clear();
	m_struPackageHead.m_sCCRequestType.clear();  
	m_struPackageHead.m_sCCRequestNumber.clear(); 
	m_struPackageHead.m_sServiceContextId.clear();	  
	
	string strBuff(cgetBuffStr,m_iBuffSize);
	if(strBuff.length()==0)
	{
		cout<<"strBuff is empty!"<<endl;
		Log::log(0,"strBuff is empty!");
		return -1;
	}

	string strTemp = strBuff.substr(0,sizeof(StdEvent)); //取最前面的stdEvent,放到strTemp里面
  	
    strBuff.erase(0,sizeof(StdEvent));       //删除最前面的stdEvent
	
  	vecEVENTATTRID.clear();
  	m_totalNum=0;
  	 Log::log(0,"接收到的字符串=%s",strBuff.c_str());
	cout<<strBuff<<endl;
  	int iresult=getEventAttr(vecEVENTATTRID,m_totalNum,strTemp,strBuff);  //取配置信息
	if(iresult==-2)
  	{
      	Log::log(0,"接收到的不是CCR数据!");
      	return -2;
  	}
	if(iresult==-1)
  	{
      	Log::log(0,"取vecEVENTATTRID配置信息失败!");
      	return -1;
  	}

	char *testToCCA=new char[m_iBuffSize];
	memset (testToCCA,0,m_iBuffSize);
	memcpy(testToCCA,(char*)headStdEvent,sizeof(StdEvent));

    strBuff=m_sSignTotal+strBuff;                     // 加"|",方便后面的处理    
	//cout<<strBuff<<endl;
	
	int pos1=0;
	int pos2=0;
	int recordPos1=strBuff.find(m_sSignRecord,0);    //^^^开始的位置
	if(recordPos1==-1) //如果没有话单记录
	{
		formSeviceInformationToCCA(0);
	}
	
	int recordPos2=0;                        //^^^一条话单结束的位置
	int i=0;
	string strGet;                           //截取到的字符串
	
	while( ( (pos2=strBuff.find(m_sSignTotal,pos1+1))!=-1)&&( (pos2<recordPos1)||(recordPos1==-1) )  )
	{
		strGet=strBuff.substr(pos1+1,pos2-pos1-1);
		//cout<<strGet<<endl;
		pos1=pos2;
		i++;
		switch(i)
		{
			case 1:
				m_struPackageHead.m_sSessionId=strGet;
				break;
			case 2:
				m_struPackageHead.m_sHopByHop=strGet;
				break;
			case 3:
				m_struPackageHead.m_sEndToEnd=strGet;
				break;
			case 4:
				m_struPackageHead.m_sAuthApplicationId=strGet;
				break;
			case 5:
				m_struPackageHead.m_sCCRequestType=strGet;
				break;
			case 6:
				m_struPackageHead.m_sCCRequestNumber=strGet;
				break;
			default:
				m_struPackageHead.m_sServiceContextId=strGet;
				break; 
		}
	}
	if(recordPos1!=-1)  //确保有话单记录
	{
		strGet=strBuff.substr(pos1+1,recordPos1-pos1-1);
		m_struPackageHead.m_sServiceContextId=strGet;   //存放到结构里
		
		//cout<<m_struPackageHead.m_sServiceContextId<<endl;
		
		while( ( recordPos2=strBuff.find(m_sSignRecord,recordPos1+3))!=-1  )
		{
			strGet=strBuff.substr(recordPos1+3,recordPos2-recordPos1-3);   //取出一条话单记录
			recordPos1=recordPos2;
			if( strGet.length()!=0 ) //如果有话单记录
				int result=DealOneRecord(strGet);
			else
			{
				delete[] testToCCA;
				return true;			 //没有话单记录没有内容
			}
		}
		//取最后一条话单记录
		strGet=strBuff.substr(recordPos1+3);   
		if( strGet.length()!=0 )     //如果有话单记录
		{
			int result=DealOneRecord(strGet);
			if(result==-1)
			{
				delete[] testToCCA;
				return -1;
			}
		}
		else
		{
			delete[] testToCCA;
			return true;             //没有话单记录没有内容
		}
		
	}
	formStrToCCA();                //构造CCA串
	
	memcpy(testToCCA+sizeof(StdEvent),m_strToCCA.c_str(),m_strToCCA.length());
	cout<<"testToCCA="<<testToCCA+sizeof(StdEvent)<<endl;
	m_poSender->send(testToCCA);
	delete[] testToCCA;
	return true;
}

//业务逻辑处理后发送给打包：（需要打的CCA包）
//StdEvent的billingNbr中存放包类型，CCR/CCA  StdEvent的m_iBillingTypeID中存放业务类型，与TransFileTypeID做对应，需要在校验的时候加上这个信息
//<Session-Id>|{hop_by_hop}|{end_to_end}|{Auth-Application-Id}|{CC-Request-Type}|{CC-Request-Number}^^^Service-Result-Code|Para-Field-Result^^^...^^^Service-Result-Code|Para-Field-Result
void DccCheckRecord::formStrToCCA()    //构造CCA串
{        
	
	m_strToCCA=m_struPackageHead.m_sSessionId+m_sSignTotal+m_struPackageHead.m_sHopByHop+m_sSignTotal+m_struPackageHead.m_sEndToEnd+m_sSignTotal+m_struPackageHead.m_sAuthApplicationId+m_sSignTotal+m_struPackageHead.m_sCCRequestType+m_sSignTotal+m_struPackageHead.m_sCCRequestNumber+m_seviceInformationToCCA;
  Log::log(0,"发送的CCA串=%s",m_strToCCA.c_str());
	//cout<<"m_strToCCA="<<m_strToCCA<<endl;

}

//void EventToolKits::set_param( StdEvent *pStd, int iEventAttrId, string strAttr )
//     StdEvent *pStd        待设置的事件
//     int iEventAttrId      属性ID
//     string strAttr        属性值
int	DccCheckRecord::DealOneRecord(string strOneRecord)
{
	int pos1=0;
	int pos2=0;
	int i=1;
	string strField,resultstrField;             //截取到的字符串
	memset (&m_oStdEvent,0,sizeof(StdEvent));
	
	pos2=strOneRecord.find(m_sSignTotal,0);
	if(pos2==-1)                //该记录只有一个字段
	{
 	Log::log(0,"该记录只有一个字段");
		return -1;
	}else
	{
		strField=strOneRecord.substr(0,pos2);// 取第一个字段
		pos1=pos2;
		resultstrField.clear();
		replaceSign(strField,resultstrField);
		m_evnetToolKits.set_param(&m_oStdEvent,vecEVENTATTRID.at(0),resultstrField);
		//cout<<"i="<<1<<endl;
		//cout<<"strField="<<strField<<endl;
	}
	
	while( (( pos2=strOneRecord.find(m_sSignTotal,pos1+1))!=-1)&&(i<m_totalNum) )
	{
		strField=strOneRecord.substr(pos1+1,pos2-pos1-1);  //取每个字段
		resultstrField.clear();
		replaceSign(strField,resultstrField);
		m_evnetToolKits.set_param(&m_oStdEvent,vecEVENTATTRID.at(i),resultstrField);

		//cout<<"i="<<i<<endl;
		//cout<<"strField="<<strField<<endl;
		pos1=pos2;
		i++;
	}
	if( (pos2==-1)&&(i<m_totalNum) )
	{
		strField=strOneRecord.substr(pos1+1);  //取最后一个字段
		resultstrField.clear();
		replaceSign(strField,resultstrField);
		m_evnetToolKits.set_param(&m_oStdEvent,vecEVENTATTRID.at(i),resultstrField);
		//cout<<"i="<<i<<endl;
		//cout<<"strField="<<strField<<endl;
	}else	
	{
		Log::log(0,"b_asn1_field及b_asn1_tag_seq表的配置字段数 小于 接受到记录的字段数");
		return -1;
	}	

	checkRecord();                //记录级校验
  	formSeviceInformationToCCA(m_oStdEvent.m_iErrorID); //构造CCA串中的{Service-Information}	业务信息  部分的内容

	return true;
}

//记录级校验
int DccCheckRecord::checkRecord()
{
	m_oStdEvent.m_oEventExt.m_iSourceEventType=1;
	m_oStdEvent.m_iGroupID=m_iGroup_Id;

	m_oStdEvent.m_iFileID=getNextFileID();
	strcpy(m_oStdEvent.m_oEventExt.m_sFileName,(m_struPackageHead.m_sServiceContextId).c_str());

	setSessionIdToStdEvent(m_struPackageHead.m_sSessionId);  //存放session_id
    m_oStdEvent.m_lEventID = getNextEventID();
	
    m_poFormatMgr->trans(&m_oStdEvent); //执行事件规整

	m_poSender->send(&m_oStdEvent);
	SendFileEndEvt(m_oStdEvent.m_iFileID);  //发送文件结束信号和强制提交事件
	return true;

}

void DccCheckRecord::setSessionIdToStdEvent(string strSessionId)
{
	char tmp[32];
	int ieventID=0;
	string resultstrSessionId;
	memset( tmp,0,sizeof(tmp) );
	
	if( ParamDefineMgr::getParam("DCC", "SESSION_ID", tmp,32) )
	{
	    ieventID=atoi(tmp);
 	}else
  	{
  		ieventID=252;
   		Log::log(0,"b_param_define表的DCC/SESSION_ID参数没有配置,取默认值。");
  	}	
  resultstrSessionId.clear();
	replaceSign(strSessionId,resultstrSessionId);
	m_evnetToolKits.set_param(&m_oStdEvent,ieventID,resultstrSessionId);

}

//构造CCA串中的{Service-Information}	业务信息  部分的内容
//^^^Service-Result-Code|Para-Field-Result^^^...^^^Service-Result-Code|Para-Field-Result
void DccCheckRecord::formSeviceInformationToCCA(int errID)
{
	char cerrorID1[64];
	memset (cerrorID1,0,64);
	sprintf(cerrorID1,"%d",errID);

	char cerrorID2[64];
	memset (cerrorID2,0,64);
	sprintf(cerrorID2,"%03d",errID);   //定宽

	m_seviceInformationToCCA=m_seviceInformationToCCA+m_sSignRecord;
	if(errID==0)
	{
		m_seviceInformationToCCA=m_seviceInformationToCCA+"0";
		m_seviceInformationToCCA=m_seviceInformationToCCA+m_sSignTotal;
		m_seviceInformationToCCA=m_seviceInformationToCCA+"0";
	}else
	{
		m_seviceInformationToCCA=m_seviceInformationToCCA+cerrorID1;
		m_seviceInformationToCCA=m_seviceInformationToCCA+m_sSignTotal;
		m_seviceInformationToCCA=m_seviceInformationToCCA+ "F"+cerrorID2;
	}
}

//根据最前面的stdEvent,找到iEventAttrId返回
//trans_file_groupid 表,参数：vector<int> vecEventAttrId  返回event_attr_id,  icount是vecEventAttrId中的记录条数
int DccCheckRecord::getEventAttr(vector<int>& vecEventAttrId,int& icount,string headString,string strBuff)
{
	//memset(headStdEvent,0,sizeof(StdEvent));
	memset(str,0,sizeof(StdEvent));
	memcpy(str,headString.c_str(),sizeof(StdEvent));

	headStdEvent=(StdEvent*)str;         //最前面的字符串强制转换成StdEvent
	m_iBillTypeID=headStdEvent->m_iBillingTypeID;
	if( strcmp(headStdEvent->m_sBillingNBR,"CCR")!=0 )
	{
	
		writeDccLog("A",headStdEvent->m_sBillingNBR,m_iBillTypeID,(char*)strBuff.c_str());
		return -2;                                                  //传过来的不是CCR数据
	}
	
	int iGroupId=getGroupIdByTransFileTypeId(m_iBillTypeID);        //根据TRANS_FILE_TYPE_ID获取GROUP_ID,TRANS_FILE_SERVICE_CONTEXT表
	
	if(iGroupId==-1)
	{
	  	Log::log(0,"没有配置TRANS_FILE_TYPE_ID=%d与GROUP_ID的关系,请检查trans_file_groupid表的配置",m_iBillTypeID);
		writeDccLog("B",headStdEvent->m_sBillingNBR,m_iBillTypeID,(char*)strBuff.c_str());
		return -1;
	}else{
		icount=getEventAttrByGroupId(vecEventAttrId,iGroupId);      //根据iGroupId获取EventAttrId 表 b_asn1_field,b_asn1_tag_seq
		strcpy(headStdEvent->m_sBillingNBR,"CCA");
		if(icount==0)
		{
			Log::log(0,"根据GroupId=%d获取EventAttrId失败,请检查模板配置",iGroupId);
			return -1;
		}else
		   	return icount;
	}
}

int DccCheckRecord::getGroupIdByTransFileTypeId(int iTransFileTypeId)  //根据TRANS_FILE_TYPE_ID获取GROUP_ID,即查TRANS_FILE_SERVICE_CONTEXT表,返回iGroupId
{
	TRANSFILEMAP::iterator pos=m_mapTransFileGroupid.find(iTransFileTypeId);
	if(pos==m_mapTransFileGroupid.end())
		return -1;
	else
	{
		m_iGroup_Id=(*pos).second;
		Log::log(0,"本条记录:Billfiletype=%d,GroupId=%d",iTransFileTypeId,m_iGroup_Id);
		return m_iGroup_Id;
	}
}

//返回获得的字段长度
int DccCheckRecord::getEventAttrByGroupId(vector<int> &vecEventid,int iGroupId)  //根据iGroupId获取EventAttrId 表 b_asn1_field,b_asn1_tag_seq
{
	vecEventid.clear();
	EVENTATTRIDMAP::iterator pos=m_mapEventAttrId.find(iGroupId);
	if(pos==m_mapEventAttrId.end())
		return -1;
	else
	{
		vecEventid=(*pos).second;
		Log::log(0,"本条记录对应的eventattrid有%d条",vecEventid.size());
		return vecEventid.size();
	}
}

int DccCheckRecord::loadTransFileGroupid()  //装载trans_file_groupid表的信息
{
	int igroupid,itransFileTypeId;
	int icount=0;
	char sSql[1000];
	memset(sSql,0,1000);
	m_mapTransFileGroupid.clear();
	m_mapEventAttrId.clear();

	sprintf(sSql,
		" select trans_file_type_id,group_id from trans_file_groupid order by trans_file_type_id");
	DEFINE_QUERY(qry);
	qry.setSQL(sSql);
	qry.open();
	while(qry.next ()) {
		itransFileTypeId=qry.field("trans_file_type_id").asInteger();
		igroupid=qry.field("group_id").asInteger();
		m_mapTransFileGroupid.insert(TRANSFILEMAP::value_type(itransFileTypeId,igroupid));
		loadEventAttrIdByGroupid(igroupid);
		m_mapEventAttrId.insert(EVENTATTRIDMAP::value_type(igroupid,m_vAttrId));
		icount++;
	}	
	qry.close();

	Log::log(0,"数据库的配置信息如下:");
	Log::log(0,"trans_file_groupid表:");
	map<int,int>::iterator iter1;
	for(iter1=m_mapTransFileGroupid.begin();iter1!=m_mapTransFileGroupid.end();iter1++)
	{
		Log::log(0,"%d->%d",(*iter1).first,(*iter1).second);
		cout<<(*iter1).first<<"->";
		cout<<(*iter1).second<<std::endl;
	}

	Log::log(0,"b_asn1_field b_asn1_tag_seq表:");
	EVENTATTRIDMAP::iterator iter2;
	for(iter2=m_mapEventAttrId.begin();iter2!=m_mapEventAttrId.end();iter2++)
	{
		Log::log(0,"%d->",(*iter2).first);

		cout<<(*iter2).first<<"->";
		vector<int>::iterator iter3;
		for(iter3=((*iter2).second).begin();iter3!=((*iter2).second).end();iter3++)
		{
			Log::log(0,"%d",*iter3);
			cout<<*iter3<<endl;
		}
	}
	
	return icount;	
}

int DccCheckRecord::loadEventAttrIdByGroupid(int igroupid)  //装载event_attr_id的信息
{
	int m_iCount=0;
	m_vAttrId.clear();

	DEFINE_QUERY (qry) ;

	//获得SQL语句
	char m_sSql[1024];
	sprintf(m_sSql,"select b.event_attr_id from b_asn1_tag_seq a,b_asn1_field b where a.group_id=b.group_id and a.tag=b.tag and a.group_id=%d order by a.seq",igroupid);

	qry.setSQL (m_sSql);

	qry.open ();

	while(qry.next ())
	{
		m_vAttrId.push_back(qry.field(0).asInteger());
		m_iCount++;
	}
	qry.close ();
	return m_iCount;

}

void DccCheckRecord::SendFileEndEvt(int iFileID)
{

/*	发送文件结束信号	*/
	Date d; 
	StdEvent oFileEndEvent; 
	oFileEndEvent.m_iEventTypeID = FILE_END_EVENT_TYPE;
	oFileEndEvent.m_iFileID = iFileID; 
	strcpy (oFileEndEvent.m_sStartDate, d.toString ());
	m_poSender->send (&oFileEndEvent); 
		
/*	发送强制提交事件	*/
	StdEvent oCommitEvent; 
	oCommitEvent.m_iFileID = iFileID; 
	oCommitEvent.m_iEventTypeID = FORCE_COMMIT_EVENT_TYPE; 
	m_poSender->send (&oCommitEvent);	

}

//从数据库表中获取接受到的Buffstr的大小
int DccCheckRecord::getBuffSize()
{
	char sql[1024];
	memset (sql, 0, sizeof(sql));
	sprintf(sql, "SELECT MAX(A.MSG_EVENT_NUM) FROM WF_MQ_ATTR A");

	DEFINE_QUERY (qry);
	qry.setSQL(sql);
	qry.open();
	qry.next();
	int iMsgEventNum = qry.field(0).asInteger();
	m_iBuffSize=(sizeof(StdEvent))*iMsgEventNum;
	qry.close();
	
	return true;
}

//获得b_param_define表中配置的分隔符
void DccCheckRecord::getSign()
{
	char tmp[32];
	memset( tmp,0,sizeof(tmp) );
	m_sSignTotal="";
	m_sSignRecord="";
	
	if( ParamDefineMgr::getParam("DCC", "DELIMITER_CHAR", tmp,32) )
	{
	    m_sSignTotal.assign(tmp);
 	}else
  	{
  		m_sSignTotal="|";
   		Log::log(0,"b_param_define表的参数没有配置,取默认值。");
  	}	
	memset( tmp,0,sizeof(tmp) );
	if( ParamDefineMgr::getParam("DCC", "DELIMITER_RECORD", tmp,32) )
	{
	    m_sSignRecord.assign(tmp);
  	}else
  	{
  		m_sSignRecord="^^^";
   		Log::log(0,"b_param_define表的参数没有配置,取默认值。");
  	}	
	
}


long DccCheckRecord::getNextFileID()
{
    long    iRet = 0;
    DEFINE_QUERY( query );

    query.setSQL( "select seq_file_id.nextval file_id from dual" );
    query.open();
    query.next();

    iRet = query.field( "file_id" ).asInteger();

    query.close();

    return iRet;
}
long DccCheckRecord::getNextDccLogID()
{
    long    iRet = 0;
    DEFINE_QUERY( query );

    query.setSQL( "select seq_dcc_log.nextval dcc_log_id from dual" );
    query.open();
    query.next();

    iRet = query.field( "dcc_log_id" ).asInteger();

    query.close();

    return iRet;
}

void DccCheckRecord::writeDccLog(char* cLogType,char* cMessageType,int iTransFileTypeId,char* cRemark)
{
	DEFINE_QUERY (qry);
    char sSql[1024];
	long ldccLog=getNextDccLogID();	
    
    sprintf (sSql, "Insert into dcc_log "
        " (log_id, err_type, message_type, trans_file_type_id, remark,log_time ) "
        " values ('%d', '%s', '%s', '%d', '%s', sysdate)",
        ldccLog,cLogType, cMessageType, iTransFileTypeId,cRemark );
    qry.setSQL (sSql);
    qry.execute();
    qry.commit ();
    qry.close ();

}
void DccCheckRecord::replaceSign(string strfield,string& resultStr )
{
	int pos=0;
	while( (pos=strfield.find("^"))!=-1 )
		strfield.replace(pos,1,"");
	resultStr=strfield;
	
}

long DccCheckRecord::getNextEventID()
{
    static long alFlag = 0;
    
    if (!alFlag) 
        getNextEventSeq();
    else
        m_lCurEventID++;
        
    alFlag++;
    alFlag = alFlag%EVENT_ID_INCREMENT;
    
    return m_lCurEventID;
}

long DccCheckRecord::getNextEventSeq()
{
    static TOCIQuery qry(DB_LINK);
    
    qry.close();
    qry.setSQL("select seq_event_id.nextval event_id from dual");
    qry.open();
    
    if (!qry.next() ) THROW(EVENT_ID_INCREMENT);
    
    m_lCurEventID = qry.field("event_id").asLong();
    
    qry.close();
    
    return m_lCurEventID;
}

