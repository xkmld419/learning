
#include "ParserXml.h"
#include "Log.h"
#include "common.h"

using namespace std;


/* begin 以下函数主要是将汉字进行转换，还原出正确的结果*/
StrTransformer::StrTransformer(char * encoding)
{
	target = new StrFormatTarget();
	m_dom = new DOMString(encoding);
	format = new XMLFormatter(m_dom->rawBuffer(), target,XMLFormatter::NoEscapes, 
		         XMLFormatter::UnRep_CharRef);

}

StrTransformer::~StrTransformer()
{
	delete target;
	delete m_dom;
	delete format;

}
char * StrTransformer::ChangeStr(const DOMString &s,char *retbuf){
	unsigned int lent = s.length();
	if (lent <= 0)
		return NULL;

	XMLCh*  buf = new XMLCh[lent + 1];
	XMLString::copyNString(buf, s.rawBuffer(), lent);
	buf[lent] = 0;
	*format<< buf;
	delete [] buf;
	target->GetResult(retbuf);
	return NULL;
}
char * StrTransformer::ChangeStr(const XMLCh * str,char *retbuf)
{
	DOMString dom(str);
	ChangeStr(dom,retbuf);
	return NULL;
}

void StrFormatTarget::writeChars(const   XMLByte* const  toWrite,
                    const   unsigned int    count,
                    XMLFormatter * const formatter)
{
	buffer = (char *)malloc(count + 1);
	memset(buffer,0,count+1);
	memcpy(buffer, (char *) toWrite, count);
};

char * StrFormatTarget::GetResult(char *retbuf)
{
	strcpy(retbuf, buffer);
	retbuf[strlen(buffer)] = 0;
	free(buffer);
	return NULL;
}
/*end*/


ParserXml::ParserXml()
{
	try{
		XMLPlatformUtils::Initialize();
	}catch(const XMLException& xe){
	  exit(0);
	} catch(...){
	  exit(0);
	}
}

ParserXml::~ParserXml()
{

}


int ParserXml::ProcessMessage(char* xmlbuf,int iDataSource)
{
	int errorCount ;
	int iRet;
	static const char* bufId = "tempbuf";
	iRet = 0;
	
	initall();
printf("%s\n",xmlbuf);		
	//初始化变量，以防干扰
/*
	if ( iDataSource == DATA_SOURCE_POWER )
		memset(&m_pin,0,sizeof(OperationIn));	
	if ( iDataSource == DATA_SOURCE_HLR )
		memset(&m_Hlr,0,sizeof(HLRMSG));	*/

Log::log(0,"%s",xmlbuf);
	try{
	   MemBufInputSource* memBufIS = 
	   	     new MemBufInputSource( (const XMLByte*)xmlbuf, strlen(xmlbuf), bufId, false);
	   
	   DOMParser *parser = new DOMParser();
	   parser->parse(*memBufIS);
	   if ((errorCount = parser->getErrorCount()) == 0)
	   {
	      DOM_Document document = parser->getDocument();
	      DOM_Element topLevel = document.getDocumentElement();
	        TopTree( topLevel,iDataSource );
	      //处理节点的元素
	      	iRet=0;
	   } 
	   else{
	       	iRet = 5;
	   }
	   iRet=1;
	   delete memBufIS;
	   delete parser;
	}
	catch(const XMLException& xe){
		iRet = 5;
	}
	catch(...){
	   iRet = 5;
	}
	return iRet;
}

int ParserXml::TopTree( DOM_Node node,int iDataSource)
{
	int iRet=0;
	if (node.getNodeType() == DOM_Node::ELEMENT_NODE)
	{
	   GetNode(node,iDataSource);		
	   DOM_NodeList children = node.getChildNodes();
	   for (unsigned int i=0; i<children.getLength(); i++) 
	     TopTree(children.item(i),iDataSource); 
	}
	return iRet;
}

void ParserXml::GetNode(DOM_Node node,int iDataSource)
{
	int iRet;
	static int iii=0;
	char sParentName[40];
	char sChFieldVaue[4000]={0};
	try{
	   DOM_NodeList children = node.getChildNodes();
	   unsigned int j=children.getLength();
	   
	   for (unsigned int i=0; i<children.getLength(); i++) 
	   {
	   	  DOM_Node nod = children.item(i);
	      if(nod.getNodeType()==DOM_Node::ELEMENT_NODE)
	      {
	          StrTransformer *	st = new StrTransformer((char *)"gbk");
	          DOMString dom1 = nod.getNodeName();
	          st->ChangeStr(dom1,sParentName);	  
			      
			      if (nod.hasChildNodes() ) {
			         DOMString dom2 = nod.getFirstChild().getNodeValue();
			         st->ChangeStr(dom2,sChFieldVaue);
			      }

			      Alltrim(sChFieldVaue);
	          SetValue(sParentName,sChFieldVaue,iDataSource);
	          
	         delete st; 
	       }         
	    }
	}
	catch(DOM_DOMException &de){
	 ;	
	}
	catch(...){}
	return ;
}

/*
按不同的数据来源iDataSource，将数据写入不同的结构中
*/
void ParserXml::SetValue(char *parent_name,char *sChFieldVaue,int iDataSource)
{
	printf("parent_name = %s, sChFieldVaue = %s \n",parent_name,sChFieldVaue);	
if ( iDataSource == DATA_SOURCE_POWER ){
	if (strcmp(parent_name,(char *)"process_code") == 0 )
	{
	   strcpy(m_pin.m_sProcessCode,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"verify_code") == 0 )
	{
	   strcpy(m_pin.m_sVerifyCode,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"req_type") == 0 )
	{
	   strcpy(m_pin.m_sReqType,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"oper_id") == 0 )
	{
	   strcpy(m_pin.m_sOperId,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"org_id") == 0 )
	{
	   strcpy(m_pin.m_sOrgId,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"city_id") == 0 )
	{
	   strcpy(m_pin.m_sCityId,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"accept_seq") == 0 )
	{
	   strcpy(m_pin.m_sAcceptSeq,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"req_seq") == 0 )
	{
	   strcpy(m_pin.m_sReqSeq,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"req_channel") == 0 )
	{
	   strcpy(m_pin.m_sReqChannel,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"req_time") == 0 )
	{
	   strcpy(m_pin.m_sReqTime,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"req_source") == 0 )
	{
	   strcpy(m_pin.m_sReqSource,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"msisdn") == 0 )
	{
	   strcpy(m_pin.m_sMsisdn,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"enddate") == 0 )
	{
	   ; //父元素 无意义
	}
	else if (strcmp(parent_name,(char *)"formnum") == 0 )
	{
	   strcpy(m_pin.m_sFormnum,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"date") == 0 )
	{
	   strcpy(m_pin.m_sEndDate,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"content") == 0 ) {
		; //父元素 无意义
	} else {
		Log::log(0,"未知的协议字段 parent_name = %s",parent_name);
	}
}
else if (iDataSource == DATA_SOURCE_HLR )
{
	if (strcmp(parent_name,(char *)"resultDoc") == 0 ) {
		;//父元素 无意义
	}
	else if (strcmp(parent_name,(char *)"returnCode") == 0 ){
		m_Hlr.returnCode=atoi(sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"vendor") == 0 ){
		strcpy(m_Hlr.vendor,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"Status") == 0 ){
		strcpy(m_Hlr.Status,sChFieldVaue);
	}
	else if (strcmp(parent_name,(char *)"errorMessage") == 0 ){
		strcpy(m_Hlr.errorMessage,sChFieldVaue);
	} else {
		Log::log(0,"未知的协议字段 parent_name = %s",parent_name);
	}
}
else{
   Log::log(0,"未知的数据源，数据源类型:%d!",iDataSource);
}
   	
	return ;
}

void ParserXml::initall()
{
}

void ParserXml::GetRespCode(int nRet,char *sRet)
{
	if (nRet == 0 ) {
		strcpy(sRet,"0");
	} else if (nRet == -1) {
		strcpy(sRet,"Sqlcode");
	}else if (nRet == -2) {
		strcpy(sRet,"Errno");
	}else if (nRet == -3) {
		strcpy(sRet,"errcode");
	}else if (nRet == -4) {
		strcpy(sRet,"-20");
	}else if (nRet == -5) {
		strcpy(sRet,"-21");
	}
}

void ParserXml::PackXmlForHLR(char *sAccNbr,int iAreaID,char *sStaffID,char *retbuf)
{
	sprintf(retbuf,"\
<?xml version=\"1.0\"  encoding=\"GBK\"?>\n\
<root>\n\
	<functionCode>ProvHLRService</functionCode>\n\
	<paramDoc>\n\
		<type>20</type>\n\
		<faultCode>86%s</faultCode >\n\
		<areaID>%d</areaID>\n\
		<opType>QRYCFALL</opType>\n\
		<operator>%s</operator >\n\
	</paramDoc>\n\
</root>",sAccNbr
				,iAreaID
				,sStaffID);
}

void ParserXml::PackXmlinfo(char *sBusiName,char *sPackName,char *sState,RespMsg &pResp,char *retbuf)
{
		sprintf(retbuf,"\
<?xml version=\"1.0\" encoding=\"GBK\"?>\n\
<operation_out>\n\
	<req_seq>%s</req_seq>\n\
	<resp_seq>%s</resp_seq>\n\
	<resp_time>%s</resp_time>\n\
	<response>\n\
		<resp_type>%d</resp_type>\n\
		<resp_code>%s</resp_code>\n\
		<resp_desc><![CDATA[%s]]></resp_desc>\n\
	</response>\n\
	<content>\n\
		<bunisess_name>%s</bunisess_name>\n\
		<package_name>%s</package_name>\n\
		<status>%s</status>\n\
	</content>\n\
</operation_out>",pResp.m_sReqSeq
								 ,pResp.m_sRespSeq
								 ,pResp.m_sRespTime
								 ,pResp.m_iRespType
								 ,pResp.m_sRespCode
								 ,pResp.m_sRespDesc
								,sBusiName
								,sPackName
								,sState);

}


void ParserXml::PackXmlLastTotalinfo(long mSumAmount,RespMsg &pResp,char *retbuf)
{
	sprintf(retbuf,"\
<?xml version=\"1.0\" encoding=\"GBK\"?>\n\
<operation_out>\n\
	<req_seq>%s</req_seq>\n\
	<resp_seq>%s</resp_seq>\n\
	<resp_time>%s</resp_time>\n\
	<response>\n\
		<resp_type>%d</resp_type>\n\
		<resp_code>%s</resp_code>\n\
		<resp_desc><![CDATA[%s]]></resp_desc>\n\
	</response>\n\
	<content>\n\
		<lastmonth_totalinfo>%ld</lastmonth_totalinfo>\n\
	</content>\n\
</operation_out>",pResp.m_sReqSeq
								 ,pResp.m_sRespSeq
								 ,pResp.m_sRespTime
								 ,pResp.m_iRespType
								 ,pResp.m_sRespCode
								 ,pResp.m_sRespDesc
								,mSumAmount);


}


void ParserXml::PackXmlFluxinfo(vector<GprsAmount> &v_Vec,RespMsg &pResp,char *retbuf)
{
	
	char tmpbuf[10000];
	memset(tmpbuf,0,10000); 

	strcpy(retbuf,"<?xml version=\"1.0\" encoding=\"GBK\"?>\n");
	strcat(retbuf,"<operation_out>\n");
	sprintf(tmpbuf,"\t<req_seq>%s</req_seq>\n",m_pin.m_sReqSeq);
	strcat(retbuf,tmpbuf);
	sprintf(tmpbuf,"\t<resp_seq>%s</resp_seq>\n",pResp.m_sReqSeq);//此处含义待定
	strcat(retbuf,tmpbuf);
	sprintf(tmpbuf,"\t<resp_time>%s</resp_time>\n",pResp.m_sRespTime);
	strcat(retbuf,tmpbuf);
	sprintf(tmpbuf,"\t<response>\n\t\t<req_type>%d</req_type>\n",pResp.m_iRespType);
	strcat(retbuf,tmpbuf);
	char sRet[10] = {0};
	GetRespCode(pResp.m_iRespType,sRet);
	sprintf(tmpbuf,"\t\t<resp_code>%s</resp_code>\n",sRet);
	strcat(retbuf,tmpbuf); 
	sprintf(tmpbuf,"\t\t<resp_desc>\n");
	strcat(retbuf,tmpbuf);
	if (pResp.m_iRespType == 0)
	   sprintf(tmpbuf,"\t\t\t<![CDATA[查询成功]]>\n\t\t</resp_desc>\n");
	else 	   
		 sprintf(tmpbuf,"\t\t\t<![CDATA[查询出错]]>\n\t\t</resp_desc>\n");
	strcat(retbuf,tmpbuf);
	sprintf(tmpbuf,"\t</response>\n");
	strcat(retbuf,tmpbuf);
	
	//begin content
	sprintf(tmpbuf,"\t<content>\n");
	strcat(retbuf,tmpbuf);
	for (vector<GprsAmount>::iterator iter=v_Vec.begin();iter !=v_Vec.end();iter++) {
	   sprintf(tmpbuf,"\t<fluxinfolist>\n\t\t<date>%s</date>\n",(*iter).sStartDate);
	   strcat(retbuf,tmpbuf);
	   sprintf(tmpbuf,"\t\t<total_fluxinfo>%ld</total_fluxinfo>\n",(*iter).SumAmount);
	   strcat(retbuf,tmpbuf);
	   sprintf(tmpbuf,"\t\t<up_fluxinfo>%ld</up_fluxinfo>\n",(*iter).SendAmount);
	   strcat(retbuf,tmpbuf);
	   sprintf(tmpbuf,"\t\t<down_fluxinfo>%ld</down_fluxinfo>\n\t</fluxinfolist>\n",(*iter).RecvAmount);
	   strcat(retbuf,tmpbuf);
	}
	sprintf(tmpbuf,"\t</content>\n</operation_out>");
	strcat(retbuf,tmpbuf);

}


void ParserXml::PackXmlHLRformnum(char * formnum,RespMsg &pResp,char *retbuf)
{
	sprintf(retbuf,
"<?xml version=\"1.0\" encoding=\"GBK\"?>\n\
<operation_out>\n\
	<req_seq>%s</req_seq>\n\
	<resp_seq>%s</resp_seq>\n\
	<resp_time>%s</resp_time>\n\
	<response>\n\
		<req_type>%d</req_type>\n\
		<resp_code>%s</resp_code>\n\
		<resp_desc><![CDATA[%s]]></resp_desc>\n\
	</response>\n\
	<content>\n\
		<formnum>%s</formnum>\n\
	</content>\n\
</operation_out>",pResp.m_sReqSeq
								 ,pResp.m_sRespSeq
								 ,pResp.m_sRespTime
								 ,pResp.m_iRespType
								 ,pResp.m_sRespCode
								 ,pResp.m_sRespDesc
								 ,formnum);

}


void ParserXml::PackXmlHLRInfo(HlrFormnum &hlr,RespMsg &pResp,char *retbuf)
{
	sprintf(retbuf,
"<?xml version=\"1.0\" encoding=\"GBK\" ?>\n\
<operation_out>\n\
	<req_seq>%s</req_seq>\n\
	<resp_seq>%s</resp_seq>\n\
	<resp_time>%s</resp_time>\n\
	<response>\n\
		<resp_type>%d</resp_type>\n\
		<resp_code>%s</resp_code>\n\
		<resp_desc><![CDATA[%s]]></resp_desc>\n\
	</response>\n\
	<content>\n\
		<msisdn>%s</msisdn>\n\
		<gprsstatus>%s</gprsstatus>\n\
		<APN>%s</APN>\n\
		<IP>%s</IP>\n\
	</content>\n\
</operation_out>",pResp.m_sReqSeq
								 ,pResp.m_sRespSeq
								 ,pResp.m_sRespTime
								 ,pResp.m_iRespType
								 ,pResp.m_sRespCode
								 ,pResp.m_sRespDesc
								 ,hlr.m_sMsisdn
								 ,hlr.m_sGprsStatus
								 ,hlr.m_sApn
								 ,hlr.m_sIp);
}
