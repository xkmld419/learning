/* 电力远程抄表实时接口
   xianggs 2010-11-01
   实现业务：1、电力远程查询用户信息(交易代码：cc_qryuserinfo)
             2、电力查询用户本月流量(交易代码：cc_qryfluxinfo)
             3、电力查询用户上月流量(交易代码：cc_qrylastmonthflux)
             4、电力查询用户在HLR的状态及NOC的IP，按两步进行
                 电力提交查询信息，计费立即返回查询流水(交易代码：cc_qryHLRformnum)
                 电力再次异步提交查询信息，计费按查询流水返回上次查询信息(交易代码：cc_qryHLRinfo)
   计费在查询HLR及NOC时，用到gsoap来调用Webservice提供的方法              
*/
#include "PowerXml.h"

using namespace std;

PowerXml::PowerXml()
{
	m_pXmlLogID = new SeqMgrExt("SEQ_XML_LOG_ID",50);
	memset(m_sFilePath,0,sizeof(m_sFilePath));
	if( !ParamDefineMgr::getParam("POWER_MAIN", "FILE_SAVE_PATH", m_sFilePath) )
		 strcpy(m_sFilePath,"./log");
		
	if( access(m_sFilePath, F_OK) != 0 )
	{
		 char sTemp[256] = "mkdir -p ";
		 strcat(sTemp, m_sFilePath);
		 system(sTemp);
	}
//	pParserXml=new ParserXml();

}

PowerXml::~PowerXml()
{

}

//每次完成后都将变量清空一下，以免影响下次的处理
void PowerXml::init()
{	
	char sRespSeq[20]={0};
	memset(&m_Resp,0,sizeof(RespMsg));
	//初始化
	m_Resp.m_iRespType = 0 ;
	strcpy(m_Resp.m_sRespDesc,"成功");
	strcpy(m_Resp.m_sRespCode,"0000");
	m_qry.getRespSeq(sRespSeq);
	sprintf(m_Resp.m_sRespSeq,"%s",sRespSeq);
	Date d1;
	strcpy(m_Resp.m_sRespTime,d1.toString("yyyymmddhhmiss"));

	
		memset(&pParserXml.m_pin,0,sizeof(OperationIn));	

		memset(&pParserXml.m_Hlr,0,sizeof(HLRMSG));	

}


//主要处理流程
int PowerXml::PowerXmlProcess(char* xmlbuf,char *retbuf)
{
	//处理前先初始化变量
	init();
	//解析XML串
	int iRet = pParserXml.ProcessMessage(xmlbuf,DATA_SOURCE_POWER);
	m_qry.insertOpertion(pParserXml.m_pin,m_Resp);
	//目前先存储在文件系统中
	SaveXMLToFile(xmlbuf,pParserXml.m_pin.m_sReqSeq);	
	strcpy(m_Resp.m_sReqSeq,pParserXml.m_pin.m_sReqSeq);//记录电力的请求序列
	//提供根据卡号码业务状态，流量信息查询接口。可查询内容：通信卡开通的业务和套餐信息查询，当前号码状态（正常、停机、空号)查询
	if (strcmp(pParserXml.m_pin.m_sProcessCode,"cc_qryuserinfo") == 0) {
		char m_sState[128];
		char sBusiName[128];
		char sPackName[128];
		
		int nRet = m_qry.queryState(pParserXml.m_pin.m_sMsisdn,sBusiName,sPackName,m_sState,m_Resp);
		memset(retbuf,0,sizeof(retbuf));
		pParserXml.PackXmlinfo(sBusiName,sPackName,m_sState,m_Resp,retbuf);
printf("%s\n",retbuf);
	 }

	//提供根据卡号码当月实时流量信息和当日实时流量 流量信息上行和下行分别统计
	else if(strcmp(pParserXml.m_pin.m_sProcessCode,"cc_qryfluxinfo") == 0) {
		vector<GprsAmount> v_Vec;
		char m_sPhone[60] = {0};
		strcpy(m_sPhone,pParserXml.m_pin.m_sMsisdn);
		strcat(m_sPhone,"|");
		strcat(m_sPhone,pParserXml.m_pin.m_sEndDate);

		int nRet = m_qry.queryGprs(m_sPhone,v_Vec,m_Resp);
		memset(retbuf,0,sizeof(retbuf));
		pParserXml.PackXmlFluxinfo(v_Vec,m_Resp,retbuf);
printf("%s\n",retbuf);			
	}

	//提供根据行业卡号码，查询该号码提交时间所属月份的上月GPRS流量总量信息
	else if(strcmp(pParserXml.m_pin.m_sProcessCode,"cc_qrylastmonthflux") == 0) {
		long sumAmount = 0;
		int nRet = m_qry.queryLastGprs(pParserXml.m_pin.m_sMsisdn,sumAmount,m_Resp);
		memset(retbuf,0,sizeof(retbuf));
		pParserXml.PackXmlLastTotalinfo(sumAmount,m_Resp,retbuf);
printf("%s\n",retbuf);	
	}

	//提供根据行业卡号码，查询该号码HLR网络侧通讯状态接口
	//处理流程：电力发送查询请求，计费收到消息后获取交易流水写表并立即返回给电力；
	else if(strcmp(pParserXml.m_pin.m_sProcessCode,"cc_qryHLRformnum") == 0) {
		//发送请求
			//生成返回电力的XML串
			char formnum[30] = {0};
			memset(retbuf,0,sizeof(retbuf));
			m_qry.getFormNum(formnum);//获取一个交易流水，返回给电力
			m_qry.insertFormNumLog(pParserXml.m_pin.m_sMsisdn,formnum,"","","");
			pParserXml.PackXmlHLRformnum(formnum,m_Resp,retbuf);
printf("%s\n",retbuf);			
	}
	//          电力异步发送查询结果请求，计费根据交易流水，查询HLR返回的结果反馈给电力
	else if(strcmp(pParserXml.m_pin.m_sProcessCode,"cc_qryHLRinfo") == 0) 
	{
		HlrFormnum hlr;
		memset(&hlr,0,sizeof(HlrFormnum));
		m_qry.getHLRinfo(pParserXml.m_pin.m_sFormnum,hlr,m_Resp);
		strcpy(pParserXml.m_pin.m_sMsisdn,hlr.m_sMsisdn);//这时电力不送号码，需要我们从表中取出
		memset(retbuf,0,sizeof(retbuf));
		pParserXml.PackXmlHLRInfo(hlr,m_Resp,retbuf);
printf("%s\n",retbuf);
	}	
	else {
		Log::log(0,"未知的交易流水");
		return -1;
	}
	
	return 0;
}

void PowerXml::SaveXMLToFile(const char *xmlbuf,char * req_seq)
{
	char sTemp[9] = {0},sPath[256] = {0},sFileName[256]={0};
	Date cur;
	cur.getTimeString(sTemp,"yyyymmdd");
  sprintf(sPath,"%s/%s",m_sFilePath,sTemp);
  
	if(access(sPath,F_OK) != 0)
	{
		if(mkdir(sPath, 0777) < 0 )
		{
			Log::log(0,"SaveXMLToFile failed! mkdir %s error!\n",sPath);
			return ;
		};
		chmod(sPath,0777);
	}

	sprintf(sFileName,"%s/%s.xml",sPath,req_seq);
	FILE *fp;
	if( (fp=fopen(sFileName,"a+")) == NULL)
	{
		Log::log(0,"SaveXMLToFile failed! fopen file error!\n");
		return;
	}
	
	fprintf(fp,"%s",xmlbuf);
	fclose(fp);

}

//存储到数据库中，需要对liboci/TOCIQuery.cpp增加对lob字段操作，如果部署在新主机可以考虑升级OCIQuery.cpp
//如果是目前生产机器上部署，有待商量是否。。。。
void  PowerXml::SaveXML(const char *xmlbuf,char * req_seq)
{
//	//向日志表中添加一条记录
//	char sSQL[512];      
//	long XML_LogID = m_pXmlLogID->getNextVal();
//	DEFINE_QUERY(qry);
//	sprintf(sSQL,"INSERT INTO Power_Xml_Log(XML_LOG_ID,REQ_SEQ_ID,XML_TEXT,CREATE_DATE)"
//	             "VALUES(%ld,%s,empty_clob(),sysdate)",XML_LogID,req_seq);
//	qry.setSQL(sSQL);
//	qry.execute();
//	qry.close();
//	
//	//把XML串保存到表的CLOB中
//	if( *xmlbuf )
//	{
//	  sprintf(sSQL,"SELECT XML_TEXT FROM xmlbuf WHERE XML_LOG_ID = %ld for update",XML_LogID);
//	  qry.setSQL(sSQL);
//	  qry.open();
//	  qry.next();
//	  qry.field(0).loadFromBufferOnce((unsigned char*)xmlbuf);
//	  qry.commit();
//	  qry.close();
//	} catch(TOCIException &ex) 
//  {
//  	Log::log(0,"SaveXML failed! req_seq = %s! "
//  		         "TOCIException catched! \n"                      
//  		         "\t *Err: %s\n"                                  
//               "\t *SQL: %s\n"
//               ,req_seq, ex.getErrMsg(), ex.getErrSrc()
//             );
//             
//    qry.rollback();
//    SaveXMLToFile(xmlbuf,lOrderListId,sAreaCode);
//  }
//  catch(...)
//  {
//  	Log::log(0,"SaveXML failed! Unknown Exception! req_seq = %s!\n",req_seq);
//  
//    qry.rollback(); 
//    		
//  	SaveXMLToFile(xmlbuf,lOrderListId,sAreaCode);
//  }
//  qry.close();
//
}

