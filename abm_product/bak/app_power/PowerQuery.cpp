#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif
#include "PowerQuery.h"

using namespace std;

PowerQuery::PowerQuery()
{
//	m_pTableName = new HashList<string >(99);
	init();
	Environment::useParamShm();
}

PowerQuery::~PowerQuery()
{
#ifdef PRIVATE_MEMORY	
	if(m_pLocalHeadMgr)
	    delete m_pLocalHeadMgr;
	m_pLocalHeadMgr = 0;
#endif	

}

void PowerQuery::init()
{
	m_pLocalHeadMgr = new Head2OrgIDMgr();
	m_pLocalHead = new HeadEx();
	m_OrgMgr     = new OrgMgr();
	m_pTableName = new InstTableListMgr();
	/*
	char sSql[256];char sTableName[52];
	Date d;
	m_iBillingCycleID = atoi(d.toString((char *)"yyyymm"));
	
	sprintf(sSql,"select org_id,replace(table_name, %d, '%%d') table_name "
	             "  from b_inst_table_list_cfg "
	             " where billing_cycle_id = %d "
	             "   and table_type = 22 "
	             "   and org_id is not null",m_iBillingCycleID,m_iBillingCycleID);
	
	DEFINE_QUERY(qry);
	qry.setSQL(sSql);qry.open();
	while(qry.next()) {
		strcpy(sTableName,qry.field(1).asString());
		m_pTableName->add(qry.field(0).asInteger(),sTableName);
	}
	*/
	strcpy(m_sOrgType,"52Z");

	char * p;
	if ((p=getenv ("TIBS_HOME")) == NULL)
	  sprintf (sHome, "/bill/rtbill/TIBS_HOME/");
	else
	  sprintf (sHome, "%s", p);

	char sFile[254];char sTemp[254];
	if (sHome[strlen(sHome)-1] == '/') {
	  sprintf (sFile, "%setc/data_access_new.ini", sHome);
	} 
	else {
	  sprintf (sFile, "%s/etc/data_access_new.ini", sHome);
	}

	ReadIni reader;	
	reader.readIniString (sFile, (char *)"SSYT", (char *)"username", m_sDBUser, (char *)"");
	reader.readIniString (sFile, (char *)"SSYT", (char *)"password", sTemp,(char *)"");
	decode(sTemp,m_sDBPwd);
	reader.readIniString (sFile, (char *)"SSYT", (char *)"connstr", m_sDBStr, (char *)"");

	reader.readIniString (sFile, (char *)"SZXBILL", (char *)"username", m_sDBUser1, (char *)"");	
	reader.readIniString (sFile, (char *)"SZXBILL", (char *)"password", sTemp,(char *)"");
	decode(sTemp,m_sDBPwd1);
	reader.readIniString (sFile, (char *)"SZXBILL", (char *)"connstr", m_sDBStr1, (char *)"");

	reader.readIniString (sFile, (char *)"HLR_URL", (char *)"url", m_sHlrUrl, (char *)"");
	reader.readIniString (sFile, (char *)"NOC_URL", (char *)"url", m_sNocUrl, (char *)"");
	
	tSsyt.connect(m_sDBUser,m_sDBPwd,m_sDBStr);
	tSzx.connect(m_sDBUser1,m_sDBPwd1,m_sDBStr1);

	if (!ParamDefineMgr::getParam("POWER_MAIN","QUERY_OFFER",m_sOfferID,128))	{
	  Log::log(0,"b_param_define未中配置 POWER_MAIN QUERY_OFFER,请配置默认offer_id");
	  exit;
	}

	if (!ParamDefineMgr::getParam("POWER_MAIN","QUERY_PRODUCT_ID",m_sProductID,128))	{
	  Log::log(0,"b_param_define未中配置 POWER_MAIN QUERY_PRODUCT_ID,请配置默认product_id");
	  exit;
	}


}

int PowerQuery::queryState(char * m_sPhone,char *sBusiName,char *sPackName,char *mState,RespMsg &pResp)
{
	//集团套餐名称，目前不知道如何查找
	strcpy(sBusiName,"");
	strcpy(sPackName,"");
	strcpy(mState,"");
	if (checkPhone(m_sPhone) ){
		pResp.m_iRespType=1;
		strcpy(pResp.m_sRespCode,"5001");//业务错误类，非电信C网号码
		strcpy(pResp.m_sRespDesc,"非电信C网号码");
		return -1;
	}	

	long m_lServID = getServID(m_sPhone);
	if ( m_lServID == -1 ){
		pResp.m_iRespType=1;
		strcpy(pResp.m_sRespCode,"5002");//业务错误类，非电信C网号码
		strcpy(pResp.m_sRespDesc,"用户在电信的资料中不存在");
		return -1;
	}
	TOCIQuery qry(&tSzx);
	sprintf(m_sSql," select c.offer_name from product_offer_instance_detail a ,product_offer_instance b,product_offer c "
                 "  where a.instance_id = :v_serv_id "
                 "    and a.product_offer_instance_id = b.product_offer_instance_id "
                 "    and b.product_offer_id = c.offer_id "
                 "    and c.offer_id in(%s) "
                 "    and nvl(a.exp_date,sysdate+1) >= sysdate "
                 "    and nvl(a.eff_date,sysdate-1) < sysdate ",m_sOfferID);
	qry.setSQL(m_sSql);
	qry.setParameter("v_serv_id",m_lServID);

	try{
		qry.open();qry.next();
		strcpy(sPackName,qry.field(0).asString());
	}catch( TOCIException & e ){
		Log::log(0,"根据号码<%s>查询订购套餐名称出错,错误原因 %s %s",m_sPhone,e.getErrMsg(),e.getErrSrc());
	  qry.rollback();
		pResp.m_iRespType=-1;
		sprintf(pResp.m_sRespCode,"%d",e.getErrCode());//数据库错误，错误代码由sqlcode指定
		strncpy(pResp.m_sRespDesc,e.getErrMsg(),110);
	  return e.getErrCode();
	}
  strcpy(sBusiName,"busi_name");//xgs,业务名称取啥？
	qry.close();
	sprintf(m_sSql," select a.state from serv_state_attr a "
                 "  where a.serv_id = :v_serv_id "
                 "    and nvl(a.exp_date,sysdate+1) >= sysdate "
                 "    and nvl(a.eff_date,sysdate-1) < sysdate ");
	qry.setSQL(m_sSql);
	qry.setParameter("v_serv_id",m_lServID);

	try{
		qry.open();qry.next();
		strcpy(mState,qry.field(0).asString());
	}catch( TOCIException & e ){
		Log::log(0,"根据号码<%s>查询用户状态出错,错误原因 %s %s",m_sPhone,e.getErrMsg(),e.getErrSrc());
	  qry.rollback();
		pResp.m_iRespType=-1;
		sprintf(pResp.m_sRespCode,"%d",e.getErrCode());//数据库错误，错误代码由sqlcode指定
		strncpy(pResp.m_sRespDesc,e.getErrMsg(),110);
	  return e.getErrCode();
	}

	return 0;
}

int PowerQuery::queryGprs(char * m_sPhone,vector<GprsAmount> &v_Vec,RespMsg &pResp)
{
	//原协议传送只有一个号码，0928修改协议，增加EndDate字段，为方便，在此拆分
	char m_sPhoneT[30];memset(m_sPhoneT,0,sizeof(m_sPhoneT));
	char m_sEndDate[30];memset(m_sEndDate,0,sizeof(m_sEndDate));

	for (int i = 0; i < strlen(m_sPhone) ; i++) {
		if (m_sPhone[i] == '|')  {
			strncpy(m_sPhoneT,m_sPhone,i);
			m_sPhoneT[strlen(m_sPhoneT)] = '\0';

			strncpy(m_sEndDate,&m_sPhone[i+1],strlen(m_sPhone)-i+1);
			m_sEndDate[strlen(m_sEndDate)] = '\0';

			break;
		}
	}
	if (checkPhone(m_sPhoneT) ){
		pResp.m_iRespType=1;
		strcpy(pResp.m_sRespCode,"5001");//业务错误类，非电信C网号码
		strcpy(pResp.m_sRespDesc,"非电信C网号码");
		return -1;
	}	

	long m_lSumAmount = 0;
	long m_lRecvAmount = 0;
	long m_lSendAmount = 0;
	char m_sStartDate[18];
	bool bHaveData=false;

	char imsi[33];
	if ( getIMSIByServID(m_sPhoneT,imsi) !=0 ){
		pResp.m_iRespType=1;
		strcpy(pResp.m_sRespCode,"5003");//业务错误类，根据号码找用户的IMSI错误
		strcpy(pResp.m_sRespDesc,"根据号码找用户的IMSI错误");
		return -1;
	}	

	if (!getTableName(m_sPhoneT)) {
		pResp.m_iRespType=1;
		strcpy(pResp.m_sRespCode,"5004");//业务错误类，不能找到存放用户流量数据的表
		strcpy(pResp.m_sRespDesc,"不能找到存放用户流量数据的表");
		return -1;
	}
	char tempTable[30];
	strcpy(tempTable,m_sTableName);
	memset(m_sTableName,0,sizeof(m_sTableName));
	sprintf(m_sTableName,tempTable,m_iBillingCycleID);

	TOCIQuery qry(&tSzx);
	sprintf(m_sSql," select to_char(trunc(start_date),'yyyymmdd') start_date,"
	               "        sum(floor((recv_amount+send_amount+1023)/1024)) sum_amount,"
	               "        sum(floor((recv_amount+1023)/1024)) recv_amount "
	               "   from %s "
	               "  where imsi = :v_imsi "
	               "    and trunc(start_date) <= to_date ('%s','yyyymmdd') "
	               "  group by trunc(start_date)",m_sTableName,m_sEndDate);
	qry.setSQL(m_sSql);
	qry.setParameter("v_imsi",imsi);

	try{
		qry.open();
	  GprsAmount gprsamount;
		while(qry.next()) {
			bHaveData=true;
			memset(&gprsamount,0,sizeof(GprsAmount));
		  strcpy(gprsamount.sStartDate,qry.field(0).asString());
		  gprsamount.SumAmount = qry.field(1).asLong();
		  gprsamount.RecvAmount = qry.field(2).asLong();
		  gprsamount.SendAmount = gprsamount.SumAmount - gprsamount.RecvAmount;
		  v_Vec.push_back(gprsamount);
	  }
	  if (! bHaveData ){
			memset(&gprsamount,0,sizeof(GprsAmount));
		  strcpy(gprsamount.sStartDate,m_sEndDate);
		  gprsamount.SumAmount = 0;
		  gprsamount.RecvAmount = 0;
		  gprsamount.SendAmount = 0;
		  v_Vec.push_back(gprsamount);	
		  return 1;//无数据可查  	
	  }

	}catch( TOCIException & e ){
		Log::log(0,"根据号码<%s>查询本月总流量,错误原因 %s %s",m_sPhone,e.getErrMsg(),e.getErrSrc());
	  qry.rollback();
	  v_Vec.clear();
		pResp.m_iRespType=-1;
		sprintf(pResp.m_sRespCode,"%d",e.getErrCode());//数据库错误，错误代码由sqlcode指定
		strncpy(pResp.m_sRespDesc,e.getErrMsg(),110);
	  return e.getErrCode();
	}
	return 0;

}

int PowerQuery::queryLastGprs(char * m_sPhone,long &SumAmount,RespMsg &pResp)
{
	if (checkPhone(m_sPhone) ){
		pResp.m_iRespType=1;
		strcpy(pResp.m_sRespCode,"5001");//业务错误类，非电信C网号码
		strcpy(pResp.m_sRespDesc,"非电信C网号码");
		return -1;
	}	

	
	long m_lSumAmount = 0;
	char imsi[33];
	if ( getIMSIByServID(m_sPhone,imsi) !=0 ){
		pResp.m_iRespType=1;
		strcpy(pResp.m_sRespCode,"5003");//业务错误类，根据号码找用户的IMSI错误
		strcpy(pResp.m_sRespDesc,"根据号码找用户的IMSI错误");
		return -1;
	}	

	if (!getTableName(m_sPhone)) {
		pResp.m_iRespType=1;
		strcpy(pResp.m_sRespCode,"5004");//业务错误类，不能找到存放用户流量数据的表
		strcpy(pResp.m_sRespDesc,"不能找到存放用户流量数据的表");
		return -1;
	}

	char tempTable[30];
	strcpy(tempTable,m_sTableName);
	memset(m_sTableName,0,sizeof(m_sTableName));
	sprintf(m_sTableName,tempTable,m_iBillingCycleID-1);
	
	TOCIQuery qry(&tSzx);
	sprintf(m_sSql,"select sum(floor((recv_amount+send_amount+1023)/1024)) from %s where imsi = :v_imsi ",m_sTableName);
	
	try{
	  qry.setSQL(m_sSql);
	  qry.setParameter("v_imsi",imsi);
		qry.open();
		if ( qry.next() )
			m_lSumAmount = qry.field(0).asLong();
	
	}catch( TOCIException & e ){
		Log::log(0,"根据号码<%s>查询上个月总流量,错误原因 %s %s",m_sPhone,e.getErrMsg(),e.getErrSrc());
	  qry.rollback();
	  SumAmount = -1;
		pResp.m_iRespType=-1;
		sprintf(pResp.m_sRespCode,"%d",e.getErrCode());//数据库错误，错误代码由sqlcode指定
		strncpy(pResp.m_sRespDesc,e.getErrMsg(),110);
	  return e.getErrCode();
	}
	
	SumAmount = m_lSumAmount;
	return 0;

}

int PowerQuery::checkPhone(char * m_sPhone)
{
//	Alltrim(m_sPhone);
//	
//	if (m_sPhone[0] == '0')
//		sprintf(m_sPhone,"%s",&m_sPhone[1]);
//	
	if ( (strncmp(m_sPhone,"133",3)) != 0 
				&& (strncmp(m_sPhone,"153",3)) != 0
				&& (strncmp(m_sPhone,"189",3)) != 0 )
		return 1;
	return 0;
}

int PowerQuery::getIMSIByServID(char * m_sPhone,char *imsi)
{
	Alltrim(m_sPhone);
	strcpy(imsi,"");
	long m_lServID = getServID(m_sPhone);
	if (m_lServID <= 0) {
		return -1;
	}

	TOCIQuery qry(&tSsyt);
	qry.setSQL("select acc_nbr from a_serv_ex where serv_id =  :v_serv_id and acc_nbr_type = '52F' "
	           " and nvl(eff_date,sysdate-1) < sysdate and nvl(exp_date,sysdate+1) > sysdate");
	qry.setParameter("v_serv_id",m_lServID);

	try{
	  qry.open();qry.next();
	  strcpy(imsi,qry.field(0).asString());
	}catch( TOCIException & e ){
		Log::log(0,"根据Serv_ID=<%ld>查询IMSI出错,错误原因 %s %s",m_lServID,e.getErrMsg(),e.getErrSrc());
	  return -1;
	}
	return 0;

}


long PowerQuery::getServID(char * m_sPhone)
{
	Alltrim(m_sPhone);

	long m_lServID;

	TOCIQuery qry(&tSsyt);
	char m_sSql[1024];
	char m_sSql1[1024];
	sprintf(m_sSql,"select a.serv_id from serv_identification a,serv b "
	           " where a.acc_nbr = '%s' and a.serv_id = b.serv_id "
	           "   and nvl(a.eff_date,sysdate-1) < sysdate"
	           "   and nvl(a.exp_date,sysdate+1) > sysdate"
	           "   and b.product_id in(%s)",m_sPhone,m_sProductID);
	try{
	  qry.setSQL(m_sSql);
	  qry.open();qry.next();
	  m_lServID = qry.field(0).asLong();
	}catch( TOCIException & e ){
		Log::log(0,"根据号码<%s>查询ServID出错,错误原因 %s %s",m_sPhone,e.getErrMsg(),e.getErrSrc());
	  return -1;
	}
	return m_lServID;
}


bool PowerQuery::getTableName(char * m_sPhone)
{
	int i_iBillingCycleID;
	if(0==m_sPhone)
	    return false;
	Date d;
	if(!m_pLocalHeadMgr->searchLocalHead(m_sPhone,d.toString((char *)"yyyymmdd"),m_pLocalHead,m_sOrgType))
	{
	   //没有找到OrgID
	   //Log::log(0,"号码<%s>没找到相应的OrgID!",m_sPhone);
	   return false;
	}

	i_iBillingCycleID = atoi(d.toString((char *)"yyyymm"));
	//xgs   	i_iBillingCycleID=201009;
	m_iOrgID = m_pLocalHead->getOrgID();
	
	if (! (m_pTableName->getTableName(i_iBillingCycleID,-1,22,m_iOrgID,m_sTableName))){
		return false;
	}
/*
	string tempTable;
	while(!m_pTableName->get(m_iOrgID,&tempTable) && m_iOrgID != 1) {
		m_iOrgID = m_OrgMgr->getParentID(m_iOrgID);
	}
	
	if (tempTable[0] == 0) {
		return false;
	}
	
	strcpy(m_sTableName,tempTable.c_str());*/
	return true;
}

void PowerQuery::getFormNum(char *sFormNum)
{
	TOCIQuery qry(&tSsyt);	
	char sSql[1024];	
	strcpy(sFormNum,"");
	sprintf(sSql,"select to_char(Sysdate,'yyyymmdd')||substr(to_char(1000000000000+ seq_power_formnum.Nextval),2,12) from dual");
	try{
		qry.close();
		qry.setSQL(sSql);
		qry.open();
		if ( qry.next() )
			strcpy(sFormNum,qry.field(0).asString());
		qry.close();	
	}catch(TOCIException & e){
		Log::log(0,"getFormNum获取交易流水失败%s,%s",e.getErrMsg(),e.getErrSrc());
	}
}

void PowerQuery::getRespSeq(char *sRespSeq)
{
	TOCIQuery qry(&tSsyt);	
	char sSql[1024];	
	strcpy(sRespSeq,"");
	sprintf(sSql,"select to_char(seq_power_respseq.Nextval) from dual");
	try{
		qry.close();
		qry.setSQL(sSql);
		qry.open();
		if ( qry.next() )
			strcpy(sRespSeq, qry.field(0).asString() );
		qry.close();	
	}catch(TOCIException & e){
		Log::log(0,"getRespSeq获取交易流水失败%s,%s",e.getErrMsg(),e.getErrSrc());
	}
}
int PowerQuery::getAreaID(char * m_sPhone)
{
	int iAreaId;
	Date d;
	if(!m_pLocalHeadMgr->searchLocalHead(m_sPhone,d.toString((char *)"yyyymmdd"),m_pLocalHead,m_sOrgType))
	{
	   return 99;
	}

	m_iOrgID = m_pLocalHead->getOrgID();
	while ( m_OrgMgr->getLevel(m_iOrgID) > 200 )
		m_iOrgID=m_OrgMgr->getParentID(m_iOrgID);
	if (	m_iOrgID == 250000002 )//	 江苏南京	
		iAreaId = 3;
	else if (	m_iOrgID == 510000002 )//		江苏无锡
		iAreaId = 15;
	else if (	m_iOrgID == 511000002 )//		江苏镇江	
		iAreaId = 4;
	else if (	m_iOrgID == 512000002 )//		江苏苏州
		iAreaId = 20;
	else if (	m_iOrgID == 513000002 )//	江苏南通
		iAreaId = 26;
	else if (	m_iOrgID == 514000002 )//		江苏扬州
		iAreaId = 33;
	else if (	m_iOrgID == 515000002 )//		江苏盐城
		iAreaId = 39;
	else if (	m_iOrgID == 516000002 )//		江苏徐州
		iAreaId = 48;
	else if (	m_iOrgID == 517000002 )//		江苏淮安
		iAreaId = 60;
	else if (	m_iOrgID == 518000002 )//		江苏连云港
		iAreaId = 63;
	else if (	m_iOrgID == 519000002 )//	江苏常州
		iAreaId = 69;
	else if (	m_iOrgID == 523000002 )//		江苏泰州
		iAreaId = 79;
	else if (	m_iOrgID == 527000002 )//		江苏宿迁
		iAreaId = 84;
	else
		iAreaId = 99;
	return iAreaId;	
		
}	

int PowerQuery::insertFormNumLog( char *sAccNbr,char *sFormNum,char *sStatus,char *sAPN,char *sIP)
{
	TOCIQuery qry(&tSsyt);
	char sSql[1024];	
	sprintf(sSql,"insert into b_power_formnum(formnum,acc_nbr,status,APN,IP,create_date,state,state_date) \
				values(:formnum,:acc_nbr,:status,:apn,:ip,sysdate,'10C',sysdate)");
	try{
		qry.close();
		qry.setSQL(sSql);
		qry.setParameter("formnum",sFormNum);
		qry.setParameter("acc_nbr",sAccNbr);
		qry.setParameter("status" ,sStatus);
		qry.setParameter("apn"    ,sAPN);
		qry.setParameter("ip"     ,sIP);
		qry.execute();
		qry.commit();
	}catch(TOCIException & e){
		Log::log(0,"insert b_power_formnum 失败%s,%s",e.getErrMsg(),e.getErrSrc());
	}
	return 0;
}	

int PowerQuery::insertFileList( char *sFileName,int iRecNum,int iFileType,char *sRespFullFile)
{
	TOCIQuery qry(&tSsyt);
	char sSql[1024];	
	sprintf(sSql,"insert into b_power_filelist(file_name,rec_num,file_type,resp_file_name,state,state_date) \
				values(:file_name,:rec_num,:file_type,:resp_file_name,0,sysdate)");
	try{
		qry.close();
		qry.setSQL(sSql);
		qry.setParameter("file_name" ,sFileName);
		qry.setParameter("rec_num"   ,iRecNum);
		qry.setParameter("file_type" ,iFileType);
		qry.setParameter("resp_file_name" ,sRespFullFile);
		qry.execute();
		qry.commit();
	}catch(TOCIException & e){
		Log::log(0,"insert b_power_filelist 失败%s,%s",e.getErrMsg(),e.getErrSrc());
	}
	return 0;	
}
/*
检查文件是否处理，1 已处理 ，0 未处理
*/

bool PowerQuery::checkFileExist( char *sFileName)
{
	TOCIQuery qry(&tSsyt);
	char sSql[1024];	
	int iCnt=0;
	sprintf(sSql,"select nvl(Count(*),0)  from b_power_filelist \
				 where file_name=:file_name");
	try{
		qry.close();
		qry.setSQL(sSql);
		qry.setParameter("file_name" ,sFileName);
		qry.open();
		if ( qry.next() )
			iCnt=qry.field(0).asInteger();
	}catch(TOCIException & e){
		Log::log(0,"select b_power_filelist 失败%s,%s",e.getErrMsg(),e.getErrSrc());
		iCnt=0;	
	}
	if ( iCnt > 0 )
		return true;
	else
		return false;	
}

int PowerQuery::getHLRinfo(char *sFormnum,HlrFormnum &pHlr,RespMsg &pResp)
{
	int iRet=0;
	TOCIQuery qry(&tSsyt);
	char sSql[1024];	
	char sState[4];
	int iCnt=0;
	sprintf(sSql,"select STATUS,APN,IP,STATE,ACC_NBR  from b_power_formnum \
				 where formnum=:formnum");
	try{
		qry.close();
		qry.setSQL(sSql);
		qry.setParameter("formnum" ,sFormnum);
		qry.open();
		if ( qry.next() ){
			strcpy(sState,            qry.field(3).asString());
			strcpy(pHlr.m_sMsisdn,    qry.field(4).asString());
			if ( strncmp(sState,"10C",3)== 0 ){
				pResp.m_iRespType=1;
				strcpy(pResp.m_sRespCode,"5100");
				strcpy(pResp.m_sRespDesc,"数据已提交未处理");
			}
			else if ( strncmp(sState,"10F",3) == 0 ){
				pResp.m_iRespType=1;
				strcpy(pResp.m_sRespCode,"5101");
				strcpy(pResp.m_sRespDesc,"查询HLR出错");
			}
			else if ( strncmp(sState,"10A",3)== 0 ){
				strcpy(pHlr.m_sGprsStatus,qry.field(0).asString());
				strcpy(pHlr.m_sApn,       qry.field(1).asString());
				strcpy(pHlr.m_sIp,        qry.field(2).asString());
			}
			else {
				pResp.m_iRespType=1;
				strcpy(pResp.m_sRespCode,"5102");
				strcpy(pResp.m_sRespDesc,"查询HLR未知状态");
			}
		}
		else{
				pResp.m_iRespType=1;
				strcpy(pResp.m_sRespCode,"5103");
				sprintf(pResp.m_sRespDesc,"提交的查询流水不存在%s",sFormnum);
				return 1;
		}
	}catch(TOCIException & e){
		Log::log(0,"select b_power_formnum 失败%s,%s",e.getErrMsg(),e.getErrSrc());
		pResp.m_iRespType=-1;
		sprintf(pResp.m_sRespCode,"%d",e.getErrCode());//数据库错误，错误代码由sqlcode指定
		strncpy(pResp.m_sRespDesc,e.getErrMsg(),110);
	  return e.getErrCode();
	}
	return iRet;
}

int PowerQuery::setHLRinfo()
{
	int iRet=0;

	TOCIQuery qry(&tSsyt);
	TOCIQuery qry2(&tSsyt);
	char sSql[1024];	
	char sSql2[2048];	
	char sAccNbr[32];
	char sFormNum[22];
	char sStatus[5];
	char imsi[22];
	char sIP[22];
	char sOnlineIP[22];
	int  iFailt;
	sprintf(sSql,"select ACC_NBR,FORMNUM  from b_power_formnum \
				 where state='10C'");
	try{
		qry.close();
		qry.setSQL(sSql);
		qry.open();
		while ( qry.next() ){
			strcpy(sStatus,"");
			strcpy(imsi,"");
			strcpy(sIP,"");
			strcpy(sOnlineIP,"");
			iFailt = 0;
			
			strcpy(sAccNbr ,qry.field(0).asString());
			strcpy(sFormNum,qry.field(1).asString());
			iFailt = iFailt + getHLRStatus(sAccNbr,sStatus);
			iFailt = iFailt + getIMSIByServID(sAccNbr,imsi);
			iFailt = iFailt + getVpdnIPByImsi(imsi,sIP,sOnlineIP);
			if ( iFailt != 0 ){
				sprintf(sSql2,"update b_power_formnum set state='10F',state_date=sysdate \
				  where formnum=:formnum");
			}
			else{	
				sprintf(sSql2,"update b_power_formnum set STATUS='%s',IP='%s',APN='%s', \
				         state='10A',state_date=sysdate where formnum=:formnum",sStatus,sIP,sOnlineIP);//APN暂时用sOnlineIP代替
			}
			qry2.close();
			qry2.setSQL(sSql2);			
			qry2.setParameter("formnum" ,sFormNum);
			qry2.execute();
			qry2.commit();
			qry2.close();
		}		
		qry.close();
	}catch(TOCIException & e){
		Log::log(0,"update b_power_formnum 失败%s,%s",e.getErrMsg(),e.getErrSrc());
	  return e.getErrCode();
	}	
	return iRet;
}

/*
返回CDMA在HLR的状态，1 开机，0 关机 
华为 POWERDOWN: 关机ACTIVE:开机
中兴 1：关机 0：开机

*/
void PowerQuery::getCdmaStatus(HLRMSG &pHlr,char *sStatus)
{
	strcpy(sStatus,"9");
	if (  pHlr.returnCode == 0 ){
		if ( strcmp(pHlr.vendor,"zx")  ){
			if ( strcmp(pHlr.Status,"1") )
				strcpy(sStatus,"0");
			else if ( strcmp(pHlr.Status,"0") )	
				strcpy(sStatus,"1");
		}
		else if (  strcmp(pHlr.vendor,"hw")  ){
			if (  strcmp(pHlr.Status,"POWERDOWN")  )
				strcpy(sStatus,"0");
			else	if (  strcmp(pHlr.Status,"ACTIVE")  )
				strcpy(sStatus,"1");
		}
	}
}

int PowerQuery::getHLRStatus(char *sAccNbr,char *sStatus)
{
			int iRet;
			struct soap soap;// gSOAP运行环境
			struct _ns2__call ns2__call;
			struct _ns2__callResponse ns2__callResponse;
			char sHlrBuff[2048]={0};
			char sHlrRet[2048]={0};
			char sAddr[128];
			int  iAreaId;
			//生成调用HLR的XML串
			iAreaId=getAreaID(sAccNbr);
			pParserXml.PackXmlForHLR(sAccNbr,iAreaId,"STAFF",sHlrBuff);
			strcpy(sAddr,m_sHlrUrl);
Log::log(0,"地址:<%s>",sAddr);
			//调用HLR前的准备	
			ns2__call.inXmlString=sHlrBuff;
	  	soap_init(&soap);
printf("%s\n",sHlrBuff);
			//调用HLR提供的接口，获取号码状态的XML串	  
 			iRet=soap_call___ns2__call(&soap, sAddr, "", &ns2__call, &ns2__callResponse);
			strcpy(sHlrRet,ns2__callResponse.callReturn);
			soap_destroy(&soap);  // 删除类实例
			soap_end(&soap);      // 清除运行环境变量
			soap_done(&soap);     // 卸载运行环境变量	  
			//解析获取的号码状态的XML串	
			pParserXml.ProcessMessage(sHlrRet,DATA_SOURCE_HLR);	
			HLRMSG pHlr=pParserXml.m_Hlr;			
			getCdmaStatus(pHlr,sStatus);
			return iRet;
	
}

int PowerQuery::getVpdnIPByImsi(char *sImsi,char *sIP,char *sOnlineIP)
{
			int iRet;
			struct soap soap;// gSOAP运行环境
			struct _ns1__GetVpdnIPByImsi  ns1__GetVpdnIPByImsi;
			struct _ns1__GetVpdnIPByImsiResponse ns1__GetVpdnIPByImsiResponse;
			
			struct _ns1__GetVpdnOnlineIPByImsi ns1__GetVpdnOnlineIPByImsi;
			struct _ns1__GetVpdnOnlineIPByImsiResponse ns1__GetVpdnOnlineIPByImsiResponse;
			char sAddr[128];	
			
			struct ns1__VpdnIPRequest *_VpdnIPRequest=new ns1__VpdnIPRequest;
			_VpdnIPRequest->imsi=sImsi;

			struct ns1__VpdnOnlineIPRequest *_VpdnOnlineIPRequest=new ns1__VpdnOnlineIPRequest;
			_VpdnOnlineIPRequest->imsi=sImsi;
			
			strcpy(sAddr,m_sNocUrl);
			//生成调用HLR的XML串
			//调用NOC前的准备	

			ns1__GetVpdnIPByImsi.request=_VpdnIPRequest;
			ns1__GetVpdnOnlineIPByImsi.request=_VpdnOnlineIPRequest;

struct timeb tp1,tp2;
ftime(&tp1);


	  	soap_init(&soap);
printf("sImsi=%s\n",sImsi);
			//调用NOC提供的接口，获取号码的IP
			iRet=soap_call___ns1__GetVpdnIPByImsi(&soap, 
                                            sAddr, 
                                            "", 
                                            &ns1__GetVpdnIPByImsi, 
                                            &ns1__GetVpdnIPByImsiResponse);
       strcpy(sIP,ns1__GetVpdnIPByImsiResponse.GetVpdnIPByImsiReturn->ip);                                                       
       iRet = ns1__GetVpdnIPByImsiResponse.GetVpdnIPByImsiReturn->ret;
/*
			 iRet = soap_call___ns1__GetVpdnOnlineIPByImsi(&soap
																										,sAddr, 
																										"", 
																										&ns1__GetVpdnOnlineIPByImsi,
																										&ns1__GetVpdnOnlineIPByImsiResponse);
       strcpy(sOnlineIP,ns1__GetVpdnOnlineIPByImsiResponse.GetVpdnOnlineIPByImsiReturn->ip);                                                       
       iRet = ns1__GetVpdnOnlineIPByImsiResponse.GetVpdnOnlineIPByImsiReturn->ret;
*/
ftime(&tp2);
printf("control 不包含初始化执行1次耗时: %d ms\n", tp2.time*1000+tp2.millitm - tp1.time*1000 -tp1.millitm );
printf("GetVpdnIPByImsi:       imsi=%s,ip=%s,ret=%d\n",sImsi,sIP,iRet);


			soap_destroy(&soap);  // 删除类实例
			soap_end(&soap);      // 清除运行环境变量
			soap_done(&soap);     // 卸载运行环境变量	  
 			
 			if (_VpdnIPRequest) delete _VpdnIPRequest;
 			if (_VpdnOnlineIPRequest) delete _VpdnOnlineIPRequest;
			return iRet;
	
}

int PowerQuery::insertOpertion(OperationIn &pOper,RespMsg &pResp)
{
	TOCIQuery qry(&tSsyt);
	char sSql[4096];	
	int iCnt=0;
	sprintf(sSql,"insert into b_power_operation( \
	 req_seq, process_code, verify_code, req_source, req_channel, req_time, req_type, \
	 oper_id, org_id, city_id, accept_seq, msisdn, formnum, end_date, state_date ) \
	 values(	:req_seq, :process_code,:verify_code,:req_source, :req_channel, :req_time, :req_type, \
	 :oper_id, :org_id, :city_id, :accept_seq, :msisdn, :formnum, :end_date, sysdate ) ");
	try{
		qry.close();
		qry.setSQL(sSql);
		qry.setParameter("req_seq"      ,pOper.m_sReqSeq);
		qry.setParameter("process_code" ,pOper.m_sProcessCode);
		qry.setParameter("verify_code"  ,pOper.m_sVerifyCode);
		qry.setParameter("req_source"   ,pOper.m_sReqSource);
		qry.setParameter("req_channel"  ,pOper.m_sReqChannel);
		qry.setParameter("req_time"     ,pOper.m_sReqTime);
		qry.setParameter("req_type"     ,pOper.m_sReqType);
		qry.setParameter("oper_id"      ,pOper.m_sOperId);
		qry.setParameter("org_id"       ,pOper.m_sOrgId);
		qry.setParameter("city_id"      ,pOper.m_sCityId);
		qry.setParameter("accept_seq"   ,pOper.m_sAcceptSeq);
		qry.setParameter("msisdn"       ,pOper.m_sMsisdn);
		qry.setParameter("formnum"      ,pOper.m_sFormnum);
		qry.setParameter("end_date"     ,pOper.m_sEndDate);
		
		qry.execute();
		qry.commit();

	}catch(TOCIException & e){
		Log::log(0,"insert into b_power_operation 失败%s,%s",e.getErrMsg(),e.getErrSrc());;	
	}
	
	return 0;
}
