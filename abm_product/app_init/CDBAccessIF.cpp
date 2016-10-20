#include "encode.h"
#include "ReadIni.h"
#include "CDBAccessIF.h"
#include <iostream>
#include "TOCIQuery.h"
#include "IpcKey.h"
#include "CommandCom.h"
using namespace std;

const char g_parrayParamNameSwitch[][64]={
"object.ticket_query_cycles",//详单可查询周期
"object.bill_query_cycles", //帐单可查询周期
"object.history_cycles", //在用与历史数据分离的周期
"locked_acct_nums", //批量销帐时每次锁定的帐户数
"locked_acct_retry_times",//帐户锁定冲突重试次数
"locked_acct_retry_interval",//帐户锁定冲突重试间隔时长
"state",			//ONLINE,OFFLINE
"run_state", // 运行状态
"max_pay_amount",//单笔缴费不允许超过金额
"max_parallel_task",//最大平均并发服务响应数
"vc.deals",				//VC受理量告警阀值
"bank.deals",			//银行受理量告警阀值
"operating_terminal_connects",	//营业终端连接数
"vc.max_access_connections",		//vc接口最大连接数	
"bank.max_access_connections",	//银行接口最大连接数	
"cashbill.deals",						//前台缴费受理量告警阀值
"max_due_flag",
"autobilling",
"ticket_query_limit",
"must_pay_change",
"big_acct_serv_num",
"allow_overdraft",
"adjust_flag",
"fee_to_next_cycle",
"adust_billed_serv",
"generate_dead_rec",
"cycle_limit",
"revserse_random",
"trasfer_balance_on_change_acct",
"remind_betweenness",
"reverse_crm_payment",
"max_adjust_charge",
"dead_account_cycle",
"open_service_cond",
"balance_refund_audit_type",
"due_audit_type",
"remind_backout_serv",
"invoice_merge_flag",
"choose_bill",
"bill_order",
"payed_change",
"change_limit",
"default_print_method",
"invoice_print_type",
"bill_more_state",
"reverse_put_passwd",
"check_agency",
"transfer_owe_flag"
};	//各业务受理量告警阀值

const char g_parrayParamSectionSwitch[][2]={
"*","*","*","*","*","*",
"*","*","*","*","*","*",
"*","*","*","*","*","*",
"*","*","*","*","*","*",
"*","*","*","*","*","*",
"*","*","*","*","*","*",
"*","*","*","*","*","*",
"*","*","*","*","*","*"
};

int g_parrayParamSwitchID[]={
1211,1241,57251305,1864003,57251301,57251302,
57251307,57251308,57251303,57251304,57251309,57251310,
1240,57251312,57251311,57251306,1006,6205,
1329,1075,30009,5011,5010,5005,
5021,5653004,5653002,1202,1063,1206,
1059,2100,2230,2000,1205,1208,
1010,1005,1008,1004,1001,1003,
1018,1015,1011,1200,5050,5060};
const	unsigned int g_iParamNumSwitch=sizeof(g_parrayParamNameSwitch)/sizeof(char[64]);

const char* g_parrayParamNameDefine[]={"products",//最大产品数
"product_offers", //最大商品数
"price_plans", //最大定价计划数	
"users", //最大用户数
"LRU_QUEUE_MAX_LEN_COPYCARD",
"LRU_QUEUE_MAX_LEN_HIGHFEE",
"max_products",
"max_product_offers",
"max_price_plans",
"max_users",
"AcctItemCount",
"ACC_DATA_COUNT",
"ACC_CYCLE_COUNT",
"TOTALDATA",
"DETAILDATA"
};	//各业务受理量告警阀值


const char* g_parrayParamNameDefineDB[]={"products",
"product_offer",
"price_plans",
"users",
"COPYCARDDATACOUNT",
"HIGHFEEDATACOUNT",
"products",
"product_offer",
"price_plans",
"users",
"AcctItemCount",
"ACC_DATA_COUNT",
"ACC_CYCLE_COUNT",
"TOTALDATA",
"DETAILDATA"
};
const char g_pMemoryIpcKey[][64]={"acct","serv","acctitem"};
const unsigned int g_iMemoryIpcKey=sizeof(g_pMemoryIpcKey)/sizeof(char[64]);
const char g_pDBIpcKey[][64]={"SHM_ACCT_INFO_DATA","SHM_SERV_INFO_DATA","SHM_AcctItemData"};

const char g_parrayParamSectionDefine[][64]={"BUSINESS",
"BUSINESS",
"BUSINESS",
"BUSINESS",
"MEMORY_DB",
"MEMORY_DB",
"BUSINESS",
"BUSINESS",
"BUSINESS",
"BUSINESS",
"MEMORY_DB",
"MEMORY_DB",
"MEMORY_DB",
"PRODOFFERAGGR",
"PRODOFFERAGGR"
};
const	unsigned int g_iParamNumDefine=sizeof(g_parrayParamSectionDefine)/sizeof(char[64]);

//测试数据库连接是否正常


//增加这个记得增加
const char g_parrayDBSection[][64]={"BILLDB",
"BILLDB","BILLDB","BILLDB","BILLDB",
"BILLDB","BILLDB"};
const char g_parrayDBParamName[][64]={"bill",
"comm","stat","ticket","virement_src",
"rt_virement_src","rt_virement_obj"};

DBInfo m_gDbInfo[7];
const unsigned int m_giDBInfoCnt =sizeof(g_parrayDBParamName)/sizeof(char[64]);






CDBAccessIF* CDBAccessIF::m_gpDBAccessIF=0;


CDBAccessIF::CDBAccessIF()
{
	m_pQuery=0;
	m_pDatabase =0;
	m_iHostID=-1;
	memset(m_sUserName,0,sizeof(m_sUserName));
	char* pUserName =  getenv("LOGNAME");
	m_iHostID=IpcKeyMgr::getHostInfo(m_sUserName);
	if(0!=pUserName)
		strcpy(m_sUserName,pUserName);  
	else
		strcpy(m_sUserName,"bill");
}

CDBAccessIF::~CDBAccessIF()
{
}

CDBAccessIF* CDBAccessIF::GetInstance()
{
	if(m_gpDBAccessIF==0)
	{
		m_gpDBAccessIF = new CDBAccessIF();
		if(!m_gpDBAccessIF->Init()){
			delete m_gpDBAccessIF;
			m_gpDBAccessIF=0;
		}
	}
	return m_gpDBAccessIF;
}
bool CDBAccessIF::Init()
{
	if(0==m_pDatabase)
			m_pDatabase = new TOCIDatabase();

	char sHome[254];
	char * p;
	if ((p=getenv ("ABM_PRO_DIR")) == NULL)
		sprintf (sHome, "/home/bill/SGW_HOME");
	else
		sprintf (sHome, "%s", p);

	ReadIni reader;
	char sFile[254];
	char sDBUser[64]={0};
	char sDBstr[64]={0};
	char sDBPwd[64]={0};

	if (sHome[strlen(sHome)-1] == '/') {
		sprintf (sFile, "%setc/sgwconfig", sHome);
	} else {
		sprintf (sFile, "%s/etc/sgwconfig", sHome);
	}

	reader.readIniString (sFile, "BILLDB", "bill.username", sDBUser, "");

	char sTemp[254];		
	reader.readIniString (sFile, "BILLDB", "bill.password", sTemp,"");
	decode(sTemp,sDBPwd);

	reader.readIniString (sFile, "BILLDB", "bill.db_server_name", sDBstr, "");


	try
	{
		if(!m_pDatabase->connect(sDBUser,sDBPwd,sDBstr))
		{
			delete m_pDatabase;m_pDatabase=0;
			return false;
		}
	}catch(	TOCIException& exp)
	{
		//数据库出错
		Log::log(0,"数据库操作错误:%d", exp.getErrCode());
		return false;
	}
													
	if(0==m_pQuery)
		m_pQuery  =new TOCIQuery(m_pDatabase);
	memset(m_gDbInfo,0,sizeof(m_gDbInfo));
	return true;
}

bool CDBAccessIF::SetDefaultConnect(char* pName,char* pPass,char* pTns)
{
	if((0==pName)||(strlen(pName)==0))
		return false;
	if((0==pPass)||(strlen(pPass)==0))
		return false;
	if((0==pTns)||(strlen(pTns)==0))
		return false;
	if(0==m_pDatabase)
		m_pDatabase = new TOCIDatabase();
	else
		m_pDatabase->disconnect();
	try
	{
		if(!m_pDatabase->connect(pName,pPass,pTns))
		{
			delete m_pDatabase;m_pDatabase=0;
			return false;
		}
	}catch(	TOCIException& exp)
	{
		//数据库出错
		Log::log(0,"数据库操作错误:%d", exp.getErrCode());
		return false;
	}

	if(0==m_pQuery)
		m_pQuery  =new TOCIQuery(m_pDatabase);
	return true;
}
bool CDBAccessIF::TestConnect(char* pName, char* pPass,char* pTns)
{
	if((0==pName)||(strlen(pName)==0))
		return false;
	if((0==pPass)||(strlen(pPass)==0))
		return false;
	if((0==pTns)||(strlen(pTns)==0))
		return false;

	TOCIDatabase* pDataBase = new TOCIDatabase();
	try
	{
		if(!pDataBase->connect(pName,pPass,pTns))
		{
			delete pDataBase;pDataBase=0;
			return false;
		}else
		{
			pDataBase->disconnect();
			delete pDataBase;pDataBase=0;
			return true;
		}
	}catch(	TOCIException& exp)
		{
			//数据库出错
			Log::log(0,"数据库连接出错:%d", exp.getErrCode());
			return false;
		}
		return true;
}

long CDBAccessIF:: ChangeParamDefineDB(const char* pSegment,const char* pCode,
		const char* pValue)
{
	if((0==pSegment)||(strlen(pSegment)==0))
		return -1;
	if((0==pCode)||(strlen(pCode)==0))
		return -1;
	if((0==pValue)||(strlen(pValue)==0))
		return -1;
	char *pUserName;
    char sHostName[200];
    pUserName = getenv("LOGNAME");
    int iHostID = IpcKeyMgr::getHostInfo(sHostName);

	if(0 !=m_pQuery)
	{
		m_pQuery->close();
		char sSql[1024]={0};
		sprintf(sSql," update b_param_define a set (a.PARAM_VALUE)=%s where "
			" lower(a.PARAM_SEGMENT)=lower('%s')and lower(a.param_code)=lower('%s') and host_id=%d "
" and lower(sys_username)=lower('%s')  ",pValue,pSegment,pCode,iHostID,pUserName);
		try
		{
			m_pQuery->setSQL(sSql);
			if(m_pQuery->execute())
			{
				m_pQuery->commit();
				m_pQuery->close();
			}
		}catch (TOCIException& exp)
		{
			//数据库出错
			Log::log(0,"数据库操作错误:%d", exp.getErrCode());
			m_pQuery->rollback();
			m_pQuery->close();	
			return exp.getErrCode();
		}

	}else
		return -1;
	return 0;

}
long CDBAccessIF::ChangeSwitchDB(long lSwitchID,const char *pValue)
{
	if((0==pValue)||(strlen(pValue)==0))
		return -1;

	if(0 !=m_pQuery)
	{
		m_pQuery->close();
		char sSql[1024]={0};
		sprintf(sSql," update a_bss_org_switch a set a.switch_value='%s' where "
			" lower(a.switch_id)=%ld",pValue,lSwitchID);
		try
		{
			m_pQuery->setSQL(sSql);
			if(m_pQuery->execute())
			{
				m_pQuery->commit();
			}
		}catch (TOCIException& exp)
		{
			//数据库出错
			Log::log(0,"数据库操作错误:%d", exp.getErrCode());
			m_pQuery->rollback();
			m_pQuery->close();
			return exp.getErrCode();
		}
		m_pQuery->close();

	}else
		return -1;
	return 0;
}

long CDBAccessIF::ChangeDBConnectInfo(const char* pSection,char* pName,char* pPass,char* pTns)
{
	if(0==pSection)
		return -1;
	char sTemp[256]={0};
	decode(pPass,sTemp);
	if(TestConnect(pName,sTemp,pTns))
	{
		for(int i=0; i<m_giDBInfoCnt;i++)
		{
			if((strcasecmp(g_parrayDBSection[i],pSection)==0)||
				(strcasecmp(g_parrayDBSection[i],"*")==0))
			{
				strcpy(m_gDbInfo[i].sName,pName);
				strcpy(m_gDbInfo[i].sPassWord,pPass);
				strcpy(m_gDbInfo[i].sTnsName,pTns);
				return 0;
			}
		}
		return -1;
	}else
		return -1;
}

long CDBAccessIF::DealDBConnect(stIniCompKey& oIniCompKey)
{
	cout<<oIniCompKey.sSectionName<<":"<<oIniCompKey.sKeyName<<":"<<oIniCompKey.sKeyValue[0]<<endl;
	int i=0;
	char sTemp[256]={0};
	for(i=0; i<m_giDBInfoCnt;i++)
	{
		if((strcasecmp(g_parrayDBSection[i],oIniCompKey.sSectionName)==0)||
			(strcasecmp(g_parrayDBSection[i],"*")==0))
		{
			//section相同
			char sTmpName[255]={0};
			sprintf(sTmpName,"%s.%s",g_parrayDBParamName[i],"username");
			char sTmpPass[255]={0};
			sprintf(sTmpPass,"%s.%s",g_parrayDBParamName[i],"password");
			char sTmpTns[255]={0};
			sprintf(sTmpTns,"%s.%s",g_parrayDBParamName[i],"db_server_name");

			if(strcasecmp(sTmpName,oIniCompKey.sKeyName)==0)
			{
				//改了登录用户名
				decode(m_gDbInfo[i].sPassWord,sTemp);
				if(!TestConnect(oIniCompKey.sKeyValue[0],sTemp,m_gDbInfo[i].sTnsName))
					return 1;
				else{
					strcpy(m_gDbInfo[i].sName,oIniCompKey.sKeyValue[0]);
					break;
				}
			}
			if(strcasecmp(sTmpPass,oIniCompKey.sKeyName)==0)
			{
				//改了密码
				decode(oIniCompKey.sKeyValue[0],sTemp);
				if(!TestConnect( m_gDbInfo[i].sName,sTemp,m_gDbInfo[i].sTnsName))
					return 1;
				else{
					strcpy(m_gDbInfo[i].sPassWord,oIniCompKey.sKeyValue[0]);
					break;
				}
			}
			if(strcasecmp(sTmpTns,oIniCompKey.sKeyName)==0)
			{
				decode(m_gDbInfo[i].sPassWord,sTemp);
				if(m_gDbInfo[i].sName[0]==0){
					strcpy(m_gDbInfo[i].sTnsName,oIniCompKey.sKeyValue[0]);
					break;
				}	
				if(!TestConnect( m_gDbInfo[i].sName,m_gDbInfo[i].sPassWord,oIniCompKey.sKeyValue[0]))
					return 1;
				else{
					strcpy(m_gDbInfo[i].sTnsName,oIniCompKey.sKeyValue[0]);
					break;
				}
			}
		}
	}
	if(0==i)
	{
		decode(m_gDbInfo[i].sPassWord,sTemp);
		this->SetDefaultConnect(m_gDbInfo[0].sName,sTemp,m_gDbInfo[0].sTnsName);
	}
	return 0;
}
long CDBAccessIF::ChangeIPCKEY(int iBillFlowID,char* pValue,const char* pKeyName)
{
	if((0==pValue)||(0==pKeyName))
		return -1;
	long lKey = atol(pValue);
	if(0!= m_pQuery)
	{
	m_pQuery->close();
                char sSql[1024]={0};
                sprintf(sSql," update b_ipc_cfg a set a.IPC_KEY=%ld where "
                        " lower(a.ipc_name)=lower('%s') and host_id=%d and lower(sys_username)=lower('%s') and billflow_id=%d ",lKey,pKeyName,m_iHostID,m_sUserName,iBillFlowID);
                try
                {
                        m_pQuery->setSQL(sSql);
                        if(m_pQuery->execute())
                        {
                                m_pQuery->commit();
                        }
                }catch (TOCIException& exp)
                {
                        m_pQuery->rollback();
                        m_pQuery->close();
			return 1;
                        return exp.getErrCode();
                }
                m_pQuery->close();
	return 0;
	}else
		return -1;
	
}
long CDBAccessIF::DealInDbData(stIniCompKey& oIniCompKey)
{
	if((strcasecmp(oIniCompKey.sSectionName,"IPC_KEY_CONFIG")==0))
	{
		string strTemp = oIniCompKey.sKeyName;
		int iPos = strTemp.find_last_of('_');
		if(iPos<0)
			return 0;
		int iBillFlowID = atoi(&(oIniCompKey.sKeyName[iPos+1]));
		strTemp.clear();
		strTemp.assign(oIniCompKey.sKeyName,iPos);
		return ChangeIPCKEY(iBillFlowID,oIniCompKey.sKeyValue[0],strTemp.c_str()); 
	}
	if((strcasecmp(oIniCompKey.sSectionName,"memory")==0))
	{
		string strTemp = oIniCompKey.sKeyName;
		for(int i=0;i<g_iMemoryIpcKey;i++){
			if(strcasecmp(oIniCompKey.sKeyName,g_pMemoryIpcKey[i])==0){
				 if(ChangeIPCKEY(-1,oIniCompKey.sKeyValue[0],g_pDBIpcKey[i])) 
					return 1;
			}	
		}
	}
	for(int i =0; i<g_iParamNumSwitch;i++)
	{
		if((strcasecmp(g_parrayParamSectionSwitch[i],oIniCompKey.sSectionName)==0)||
			(strcasecmp(g_parrayParamSectionSwitch[i],"*")==0))
		{
			//section相同
			if((strcasecmp(g_parrayParamNameSwitch[i],oIniCompKey.sKeyName)==0))
			{
				//keyname相同
				return ChangeSwitchDB(g_parrayParamSwitchID[i],oIniCompKey.sKeyValue[0]);
			}
		}
	}

	for(int i =0;i<g_iParamNumDefine;i++)
	{
		if((strcasecmp(g_parrayParamSectionDefine[i],oIniCompKey.sSectionName)==0)||(strcasecmp(g_parrayParamSectionDefine[i],"*")==0))
		{
			//section相同
			if((strcasecmp(g_parrayParamNameDefine[i],oIniCompKey.sKeyName)==0))
			{
				//keyname相同
				return ChangeParamDefineDB(g_parrayParamSectionDefine[i],
					g_parrayParamNameDefineDB[i],oIniCompKey.sKeyValue[0]);
			}
		}
	}
	return 0;
}

bool CDBAccessIF::TestMySelf(void)
{
	bool bRet= true;
	char sTemp[256]={0};
	for(int i=0;i<m_giDBInfoCnt;i++)
	{
		if((m_gDbInfo[i].sSection[0]==0)||(m_gDbInfo[i].sName[0]==0)||
			(m_gDbInfo[i].sPassWord[0]==0))
		{
			continue;
		}
		decode(m_gDbInfo[i].sPassWord,sTemp);
		if(!TestConnect(m_gDbInfo[i].sName,sTemp,
			m_gDbInfo[i].sTnsName))
			bRet =false;
	}
	return bRet;
}
void CDBAccessIF::Clear()
{
	memset(m_gDbInfo,0,sizeof(m_gDbInfo));
}
long CDBAccessIF::InitDBConnect(const char *pSysParamName)
{

	string strFileName=DEFAULT_CONFIG_FILE_NAME;
        char *p = getenv ("ABM_PRO_DIR");
        if(p)
                strFileName.insert(0,p);
        if (-1 ==access( strFileName.c_str(), R_OK| W_OK| F_OK ) ){
                Log::log(0,"文件不存在或者由于权限问题无法访问:%s",
                        strFileName.c_str());
                return false;
        }
	ReadIni reader;
	for(int i=0;i<m_giDBInfoCnt;i++)
	{
		char sTemp[254]  ={0};
		char sTmpVal[254]={0};
		memset(sTemp, 0, sizeof(sTemp));

		strcpy(m_gDbInfo[i].sSection,g_parrayDBSection[i]);

		memset(sTmpVal, 0, sizeof(sTmpVal));
		sprintf (sTmpVal,"%s.username",g_parrayDBParamName[i]);
		reader.readIniString (strFileName.c_str(), g_parrayDBSection[i], sTmpVal, m_gDbInfo[0].sName, "");

		memset(sTmpVal, 0, sizeof(sTmpVal));
		sprintf (sTmpVal,"%s.password",g_parrayDBParamName[i]);
		reader.readIniString (strFileName.c_str(), g_parrayDBSection[i], sTmpVal, m_gDbInfo[i].sPassWord,"");
		//decode(sTemp,m_gDbInfo[i].sPassWord);

		memset(sTmpVal, 0, sizeof(sTmpVal));
		sprintf (sTmpVal,"%s.db_server_name",g_parrayDBParamName[i]);
		reader.readIniString (strFileName.c_str(), g_parrayDBSection[i], sTmpVal, m_gDbInfo[i].sTnsName, "");

	}
	return true;
}



