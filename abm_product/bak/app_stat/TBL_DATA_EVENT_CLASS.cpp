/*VER: 1*/ 
#include	"TBL_DATA_EVENT_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_DATA_EVENT_CLASS::TBL_DATA_EVENT_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="DATA_EVENT";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_DATA_EVENT_CLASS::~TBL_DATA_EVENT_CLASS()
{
		FreeBuf();
}

void	TBL_DATA_EVENT_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_DATA_EVENT_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_DATA_EVENT_CLASS::Open()
{
		string strSql;
		strSql="SELECT EVENT_ID,BILL_FLOW_ID,BILLING_CYCLE_ID,EVENT_TYPE_ID,NVL(BILLING_AREA_CODE,-1) BILLING_AREA_CODE,NVL(BILLING_NBR,-1) BILLING_NBR,BILLING_ORG_ID,NVL(BILLING_VISIT_AREA_CODE,-1) BILLING_VISIT_AREA_CODE,NVL(CALLING_AREA_CODE,-1) CALLING_AREA_CODE,NVL(CALLING_NBR,-1) CALLING_NBR,NVL(ORG_CALLING_NBR,-1) ORG_CALLING_NBR,NVL(CALLING_SP_TYPE_ID,-1) CALLING_SP_TYPE_ID,NVL(CALLING_SERVICE_TYPE_ID,-1) CALLING_SERVICE_TYPE_ID,NVL(CALLED_AREA_CODE,-1) CALLED_AREA_CODE,NVL(CALLED_NBR,-1) CALLED_NBR,NVL(ORG_CALLED_NBR,-1) ORG_CALLED_NBR,NVL(CALLED_SP_TYPE_ID,-1) CALLED_SP_TYPE_ID,NVL(CALLED_SERVICE_TYPE_ID,-1) CALLED_SERVICE_TYPE_ID,NVL(SWITCH_ID,-1) SWITCH_ID,NVL(ACCOUNT_AREA_CODE,-1) ACCOUNT_AREA_CODE,NVL(ACCOUNT_NBR,-1) ACCOUNT_NBR,NVL(NAS_IP,-1) NAS_IP,NVL(NAS_PORT_ID,-1) NAS_PORT_ID,NVL(TO_CHAR(START_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') START_DATE,NVL(DURATION,-1) DURATION,NVL(RECV_BYTES,-1) RECV_BYTES,NVL(SEND_BYTES,-1) SEND_BYTES,NVL(RECV_PACKETS,-1) RECV_PACKETS,NVL(SEND_PACKETS,-1) SEND_PACKETS,NVL(FILE_ID,-1) FILE_ID,NVL(TO_CHAR(CREATED_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') CREATED_DATE,NVL(EVENT_STATE,-1) EVENT_STATE,NVL(TO_CHAR(STATE_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') STATE_DATE,NVL(CARD_NO,-1) CARD_NO,NVL(BILLING_TYPE,-1) BILLING_TYPE,NVL(PARTER_ID,-1) PARTER_ID,SERV_ID,NVL(OFFER_ID1,-1) OFFER_ID1,NVL(OFFER_ID2,-1) OFFER_ID2,NVL(OFFER_ID3,-1) OFFER_ID3,NVL(OFFER_ID4,-1) OFFER_ID4,NVL(OFFER_ID5,-1) OFFER_ID5,NVL(ORG_CHARGE1,-1) ORG_CHARGE1,NVL(STD_CHARGE1,-1) STD_CHARGE1,NVL(CHARGE1,-1) CHARGE1,NVL(ACCT_ITEM_TYPE_ID1,-1) ACCT_ITEM_TYPE_ID1,NVL(DISCOUNT_ACCT_ITEM_TYPE_ID1,-1) DISCOUNT_ACCT_ITEM_TYPE_ID1,NVL(BILL_MEASURE1,-1) BILL_MEASURE1,NVL(ORG_CHARGE2,-1) ORG_CHARGE2,NVL(STD_CHARGE2,-1) STD_CHARGE2,NVL(CHARGE2,-1) CHARGE2,NVL(ACCT_ITEM_TYPE_ID2,-1) ACCT_ITEM_TYPE_ID2,NVL(DISCOUNT_ACCT_ITEM_TYPE_ID2,-1) DISCOUNT_ACCT_ITEM_TYPE_ID2,NVL(BILL_MEASURE2,-1) BILL_MEASURE2,NVL(BILLING_RECORD,-1) BILLING_RECORD,NVL(PRICING_COMBINE_ID,-1) PRICING_COMBINE_ID from ";
		if	(sTableName.size()==0)
			strSql+="DATA_EVENT";
		else
			strSql+=sTableName;
		if	(sConditionSql.size()!=0)
			strSql+=" where "+sConditionSql;
		strSql[strSql.size()]='\0';
		try{
			if ( m_qry == NULL) 
				m_qry = new TOCIQuery ( &gpDBLink);
			m_qry->close();
			m_qry->setSQL(strSql.c_str());
			m_qry->open();
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_DATA_EVENT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_DATA_EVENT_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry not open!");
		try{
			if ( !m_qry->next())
				return false;
			item.EVENT_ID=m_qry->field("EVENT_ID").asLong();
			item.BILL_FLOW_ID=m_qry->field("BILL_FLOW_ID").asLong();
			item.BILLING_CYCLE_ID=m_qry->field("BILLING_CYCLE_ID").asLong();
			item.EVENT_TYPE_ID=m_qry->field("EVENT_TYPE_ID").asLong();
			strcpy(item.BILLING_AREA_CODE,m_qry->field("BILLING_AREA_CODE").asString());
			strcpy(item.BILLING_NBR,m_qry->field("BILLING_NBR").asString());
			item.BILLING_ORG_ID=m_qry->field("BILLING_ORG_ID").asLong();
			strcpy(item.BILLING_VISIT_AREA_CODE,m_qry->field("BILLING_VISIT_AREA_CODE").asString());
			strcpy(item.CALLING_AREA_CODE,m_qry->field("CALLING_AREA_CODE").asString());
			strcpy(item.CALLING_NBR,m_qry->field("CALLING_NBR").asString());
			strcpy(item.ORG_CALLING_NBR,m_qry->field("ORG_CALLING_NBR").asString());
			item.CALLING_SP_TYPE_ID=m_qry->field("CALLING_SP_TYPE_ID").asLong();
			item.CALLING_SERVICE_TYPE_ID=m_qry->field("CALLING_SERVICE_TYPE_ID").asLong();
			strcpy(item.CALLED_AREA_CODE,m_qry->field("CALLED_AREA_CODE").asString());
			strcpy(item.CALLED_NBR,m_qry->field("CALLED_NBR").asString());
			strcpy(item.ORG_CALLED_NBR,m_qry->field("ORG_CALLED_NBR").asString());
			item.CALLED_SP_TYPE_ID=m_qry->field("CALLED_SP_TYPE_ID").asLong();
			item.CALLED_SERVICE_TYPE_ID=m_qry->field("CALLED_SERVICE_TYPE_ID").asLong();
			item.SWITCH_ID=m_qry->field("SWITCH_ID").asLong();
			strcpy(item.ACCOUNT_AREA_CODE,m_qry->field("ACCOUNT_AREA_CODE").asString());
			strcpy(item.ACCOUNT_NBR,m_qry->field("ACCOUNT_NBR").asString());
			strcpy(item.NAS_IP,m_qry->field("NAS_IP").asString());
			item.NAS_PORT_ID=m_qry->field("NAS_PORT_ID").asLong();
			strcpy(item.START_DATE,m_qry->field("START_DATE").asString());
			item.DURATION=m_qry->field("DURATION").asLong();
			item.RECV_BYTES=m_qry->field("RECV_BYTES").asLong();
			item.SEND_BYTES=m_qry->field("SEND_BYTES").asLong();
			item.RECV_PACKETS=m_qry->field("RECV_PACKETS").asLong();
			item.SEND_PACKETS=m_qry->field("SEND_PACKETS").asLong();
			item.FILE_ID=m_qry->field("FILE_ID").asLong();
			strcpy(item.CREATED_DATE,m_qry->field("CREATED_DATE").asString());
			strcpy(item.EVENT_STATE,m_qry->field("EVENT_STATE").asString());
			strcpy(item.STATE_DATE,m_qry->field("STATE_DATE").asString());
			strcpy(item.CARD_NO,m_qry->field("CARD_NO").asString());
			strcpy(item.BILLING_TYPE,m_qry->field("BILLING_TYPE").asString());
			item.PARTER_ID=m_qry->field("PARTER_ID").asLong();
			item.SERV_ID=m_qry->field("SERV_ID").asLong();
			item.OFFER_ID1=m_qry->field("OFFER_ID1").asLong();
			item.OFFER_ID2=m_qry->field("OFFER_ID2").asLong();
			item.OFFER_ID3=m_qry->field("OFFER_ID3").asLong();
			item.OFFER_ID4=m_qry->field("OFFER_ID4").asLong();
			item.OFFER_ID5=m_qry->field("OFFER_ID5").asLong();
			item.ORG_CHARGE1=m_qry->field("ORG_CHARGE1").asLong();
			item.STD_CHARGE1=m_qry->field("STD_CHARGE1").asLong();
			item.CHARGE1=m_qry->field("CHARGE1").asLong();
			item.ACCT_ITEM_TYPE_ID1=m_qry->field("ACCT_ITEM_TYPE_ID1").asLong();
			item.DISCOUNT_ACCT_ITEM_TYPE_ID1=m_qry->field("DISCOUNT_ACCT_ITEM_TYPE_ID1").asLong();
			item.BILL_MEASURE1=m_qry->field("BILL_MEASURE1").asLong();
			item.ORG_CHARGE2=m_qry->field("ORG_CHARGE2").asLong();
			item.STD_CHARGE2=m_qry->field("STD_CHARGE2").asLong();
			item.CHARGE2=m_qry->field("CHARGE2").asLong();
			item.ACCT_ITEM_TYPE_ID2=m_qry->field("ACCT_ITEM_TYPE_ID2").asLong();
			item.DISCOUNT_ACCT_ITEM_TYPE_ID2=m_qry->field("DISCOUNT_ACCT_ITEM_TYPE_ID2").asLong();
			item.BILL_MEASURE2=m_qry->field("BILL_MEASURE2").asLong();
			strcpy(item.BILLING_RECORD,m_qry->field("BILLING_RECORD").asString());
			item.PRICING_COMBINE_ID=m_qry->field("PRICING_COMBINE_ID").asLong();
		return true;
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_DATA_EVENT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_DATA_EVENT_CLASS::Get_EVENT_ID(void)
{
		return	item.EVENT_ID;
}

long	TBL_DATA_EVENT_CLASS::Get_BILL_FLOW_ID(void)
{
		return	item.BILL_FLOW_ID;
}

long	TBL_DATA_EVENT_CLASS::Get_BILLING_CYCLE_ID(void)
{
		return	item.BILLING_CYCLE_ID;
}

long	TBL_DATA_EVENT_CLASS::Get_EVENT_TYPE_ID(void)
{
		return	item.EVENT_TYPE_ID;
}

char*	TBL_DATA_EVENT_CLASS::Get_BILLING_AREA_CODE(void)
{
		return	item.BILLING_AREA_CODE;
}

char*	TBL_DATA_EVENT_CLASS::Get_BILLING_NBR(void)
{
		return	item.BILLING_NBR;
}

long	TBL_DATA_EVENT_CLASS::Get_BILLING_ORG_ID(void)
{
		return	item.BILLING_ORG_ID;
}

char*	TBL_DATA_EVENT_CLASS::Get_BILLING_VISIT_AREA_CODE(void)
{
		return	item.BILLING_VISIT_AREA_CODE;
}

char*	TBL_DATA_EVENT_CLASS::Get_CALLING_AREA_CODE(void)
{
		return	item.CALLING_AREA_CODE;
}

char*	TBL_DATA_EVENT_CLASS::Get_CALLING_NBR(void)
{
		return	item.CALLING_NBR;
}

char*	TBL_DATA_EVENT_CLASS::Get_ORG_CALLING_NBR(void)
{
		return	item.ORG_CALLING_NBR;
}

long	TBL_DATA_EVENT_CLASS::Get_CALLING_SP_TYPE_ID(void)
{
		return	item.CALLING_SP_TYPE_ID;
}

long	TBL_DATA_EVENT_CLASS::Get_CALLING_SERVICE_TYPE_ID(void)
{
		return	item.CALLING_SERVICE_TYPE_ID;
}

char*	TBL_DATA_EVENT_CLASS::Get_CALLED_AREA_CODE(void)
{
		return	item.CALLED_AREA_CODE;
}

char*	TBL_DATA_EVENT_CLASS::Get_CALLED_NBR(void)
{
		return	item.CALLED_NBR;
}

char*	TBL_DATA_EVENT_CLASS::Get_ORG_CALLED_NBR(void)
{
		return	item.ORG_CALLED_NBR;
}

long	TBL_DATA_EVENT_CLASS::Get_CALLED_SP_TYPE_ID(void)
{
		return	item.CALLED_SP_TYPE_ID;
}

long	TBL_DATA_EVENT_CLASS::Get_CALLED_SERVICE_TYPE_ID(void)
{
		return	item.CALLED_SERVICE_TYPE_ID;
}

long	TBL_DATA_EVENT_CLASS::Get_SWITCH_ID(void)
{
		return	item.SWITCH_ID;
}

char*	TBL_DATA_EVENT_CLASS::Get_ACCOUNT_AREA_CODE(void)
{
		return	item.ACCOUNT_AREA_CODE;
}

char*	TBL_DATA_EVENT_CLASS::Get_ACCOUNT_NBR(void)
{
		return	item.ACCOUNT_NBR;
}

char*	TBL_DATA_EVENT_CLASS::Get_NAS_IP(void)
{
		return	item.NAS_IP;
}

long	TBL_DATA_EVENT_CLASS::Get_NAS_PORT_ID(void)
{
		return	item.NAS_PORT_ID;
}

char*	TBL_DATA_EVENT_CLASS::Get_START_DATE(void)
{
		return	item.START_DATE;
}

long	TBL_DATA_EVENT_CLASS::Get_DURATION(void)
{
		return	item.DURATION;
}

long	TBL_DATA_EVENT_CLASS::Get_RECV_BYTES(void)
{
		return	item.RECV_BYTES;
}

long	TBL_DATA_EVENT_CLASS::Get_SEND_BYTES(void)
{
		return	item.SEND_BYTES;
}

long	TBL_DATA_EVENT_CLASS::Get_RECV_PACKETS(void)
{
		return	item.RECV_PACKETS;
}

long	TBL_DATA_EVENT_CLASS::Get_SEND_PACKETS(void)
{
		return	item.SEND_PACKETS;
}

long	TBL_DATA_EVENT_CLASS::Get_FILE_ID(void)
{
		return	item.FILE_ID;
}

char*	TBL_DATA_EVENT_CLASS::Get_CREATED_DATE(void)
{
		return	item.CREATED_DATE;
}

char*	TBL_DATA_EVENT_CLASS::Get_EVENT_STATE(void)
{
		return	item.EVENT_STATE;
}

char*	TBL_DATA_EVENT_CLASS::Get_STATE_DATE(void)
{
		return	item.STATE_DATE;
}

char*	TBL_DATA_EVENT_CLASS::Get_CARD_NO(void)
{
		return	item.CARD_NO;
}

char*	TBL_DATA_EVENT_CLASS::Get_BILLING_TYPE(void)
{
		return	item.BILLING_TYPE;
}

long	TBL_DATA_EVENT_CLASS::Get_PARTER_ID(void)
{
		return	item.PARTER_ID;
}

long	TBL_DATA_EVENT_CLASS::Get_SERV_ID(void)
{
		return	item.SERV_ID;
}

long	TBL_DATA_EVENT_CLASS::Get_OFFER_ID1(void)
{
		return	item.OFFER_ID1;
}

long	TBL_DATA_EVENT_CLASS::Get_OFFER_ID2(void)
{
		return	item.OFFER_ID2;
}

long	TBL_DATA_EVENT_CLASS::Get_OFFER_ID3(void)
{
		return	item.OFFER_ID3;
}

long	TBL_DATA_EVENT_CLASS::Get_OFFER_ID4(void)
{
		return	item.OFFER_ID4;
}

long	TBL_DATA_EVENT_CLASS::Get_OFFER_ID5(void)
{
		return	item.OFFER_ID5;
}

long	TBL_DATA_EVENT_CLASS::Get_ORG_CHARGE1(void)
{
		return	item.ORG_CHARGE1;
}

long	TBL_DATA_EVENT_CLASS::Get_STD_CHARGE1(void)
{
		return	item.STD_CHARGE1;
}

long	TBL_DATA_EVENT_CLASS::Get_CHARGE1(void)
{
		return	item.CHARGE1;
}

long	TBL_DATA_EVENT_CLASS::Get_ACCT_ITEM_TYPE_ID1(void)
{
		return	item.ACCT_ITEM_TYPE_ID1;
}

long	TBL_DATA_EVENT_CLASS::Get_DISCOUNT_ACCT_ITEM_TYPE_ID1(void)
{
		return	item.DISCOUNT_ACCT_ITEM_TYPE_ID1;
}

long	TBL_DATA_EVENT_CLASS::Get_BILL_MEASURE1(void)
{
		return	item.BILL_MEASURE1;
}

long	TBL_DATA_EVENT_CLASS::Get_ORG_CHARGE2(void)
{
		return	item.ORG_CHARGE2;
}

long	TBL_DATA_EVENT_CLASS::Get_STD_CHARGE2(void)
{
		return	item.STD_CHARGE2;
}

long	TBL_DATA_EVENT_CLASS::Get_CHARGE2(void)
{
		return	item.CHARGE2;
}

long	TBL_DATA_EVENT_CLASS::Get_ACCT_ITEM_TYPE_ID2(void)
{
		return	item.ACCT_ITEM_TYPE_ID2;
}

long	TBL_DATA_EVENT_CLASS::Get_DISCOUNT_ACCT_ITEM_TYPE_ID2(void)
{
		return	item.DISCOUNT_ACCT_ITEM_TYPE_ID2;
}

long	TBL_DATA_EVENT_CLASS::Get_BILL_MEASURE2(void)
{
		return	item.BILL_MEASURE2;
}

char*	TBL_DATA_EVENT_CLASS::Get_BILLING_RECORD(void)
{
		return	item.BILLING_RECORD;
}

long	TBL_DATA_EVENT_CLASS::Get_PRICING_COMBINE_ID(void)
{
		return	item.PRICING_COMBINE_ID;
}

void	TBL_DATA_EVENT_CLASS::Set_EVENT_ID(long	lParam)
{
		item.EVENT_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_BILL_FLOW_ID(long	lParam)
{
		item.BILL_FLOW_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_BILLING_CYCLE_ID(long	lParam)
{
		item.BILLING_CYCLE_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_EVENT_TYPE_ID(long	lParam)
{
		item.EVENT_TYPE_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_BILLING_AREA_CODE(char	* sParam)
{
		if ( strlen(sParam)>10 ) 
			 throw TException( " Set_BILLING_AREA_CODE  string over 10!");
		strcpy(item.BILLING_AREA_CODE,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_BILLING_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_BILLING_NBR  string over 32!");
		strcpy(item.BILLING_NBR,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_BILLING_ORG_ID(long	lParam)
{
		item.BILLING_ORG_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_BILLING_VISIT_AREA_CODE(char	* sParam)
{
		if ( strlen(sParam)>10 ) 
			 throw TException( " Set_BILLING_VISIT_AREA_CODE  string over 10!");
		strcpy(item.BILLING_VISIT_AREA_CODE,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_CALLING_AREA_CODE(char	* sParam)
{
		if ( strlen(sParam)>10 ) 
			 throw TException( " Set_CALLING_AREA_CODE  string over 10!");
		strcpy(item.CALLING_AREA_CODE,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_CALLING_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_CALLING_NBR  string over 32!");
		strcpy(item.CALLING_NBR,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_ORG_CALLING_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_ORG_CALLING_NBR  string over 32!");
		strcpy(item.ORG_CALLING_NBR,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_CALLING_SP_TYPE_ID(long	lParam)
{
		item.CALLING_SP_TYPE_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_CALLING_SERVICE_TYPE_ID(long	lParam)
{
		item.CALLING_SERVICE_TYPE_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_CALLED_AREA_CODE(char	* sParam)
{
		if ( strlen(sParam)>10 ) 
			 throw TException( " Set_CALLED_AREA_CODE  string over 10!");
		strcpy(item.CALLED_AREA_CODE,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_CALLED_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_CALLED_NBR  string over 32!");
		strcpy(item.CALLED_NBR,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_ORG_CALLED_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_ORG_CALLED_NBR  string over 32!");
		strcpy(item.ORG_CALLED_NBR,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_CALLED_SP_TYPE_ID(long	lParam)
{
		item.CALLED_SP_TYPE_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_CALLED_SERVICE_TYPE_ID(long	lParam)
{
		item.CALLED_SERVICE_TYPE_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_SWITCH_ID(long	lParam)
{
		item.SWITCH_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_ACCOUNT_AREA_CODE(char	* sParam)
{
		if ( strlen(sParam)>10 ) 
			 throw TException( " Set_ACCOUNT_AREA_CODE  string over 10!");
		strcpy(item.ACCOUNT_AREA_CODE,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_ACCOUNT_NBR(char	* sParam)
{
		if ( strlen(sParam)>30 ) 
			 throw TException( " Set_ACCOUNT_NBR  string over 30!");
		strcpy(item.ACCOUNT_NBR,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_NAS_IP(char	* sParam)
{
		if ( strlen(sParam)>20 ) 
			 throw TException( " Set_NAS_IP  string over 20!");
		strcpy(item.NAS_IP,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_NAS_PORT_ID(long	lParam)
{
		item.NAS_PORT_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_START_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_START_DATE  string over 14!");
		strcpy(item.START_DATE,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_DURATION(long	lParam)
{
		item.DURATION=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_RECV_BYTES(long	lParam)
{
		item.RECV_BYTES=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_SEND_BYTES(long	lParam)
{
		item.SEND_BYTES=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_RECV_PACKETS(long	lParam)
{
		item.RECV_PACKETS=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_SEND_PACKETS(long	lParam)
{
		item.SEND_PACKETS=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_FILE_ID(long	lParam)
{
		item.FILE_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_CREATED_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_CREATED_DATE  string over 14!");
		strcpy(item.CREATED_DATE,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_EVENT_STATE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_EVENT_STATE  string over 3!");
		strcpy(item.EVENT_STATE,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_STATE_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_STATE_DATE  string over 14!");
		strcpy(item.STATE_DATE,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_CARD_NO(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_CARD_NO  string over 32!");
		strcpy(item.CARD_NO,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_BILLING_TYPE(char	* sParam)
{
		if ( strlen(sParam)>1 ) 
			 throw TException( " Set_BILLING_TYPE  string over 1!");
		strcpy(item.BILLING_TYPE,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_PARTER_ID(long	lParam)
{
		item.PARTER_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_SERV_ID(long	lParam)
{
		item.SERV_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_OFFER_ID1(long	lParam)
{
		item.OFFER_ID1=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_OFFER_ID2(long	lParam)
{
		item.OFFER_ID2=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_OFFER_ID3(long	lParam)
{
		item.OFFER_ID3=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_OFFER_ID4(long	lParam)
{
		item.OFFER_ID4=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_OFFER_ID5(long	lParam)
{
		item.OFFER_ID5=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_ORG_CHARGE1(long	lParam)
{
		item.ORG_CHARGE1=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_STD_CHARGE1(long	lParam)
{
		item.STD_CHARGE1=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_CHARGE1(long	lParam)
{
		item.CHARGE1=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_ACCT_ITEM_TYPE_ID1(long	lParam)
{
		item.ACCT_ITEM_TYPE_ID1=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_DISCOUNT_ACCT_ITEM_TYPE_ID1(long	lParam)
{
		item.DISCOUNT_ACCT_ITEM_TYPE_ID1=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_BILL_MEASURE1(long	lParam)
{
		item.BILL_MEASURE1=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_ORG_CHARGE2(long	lParam)
{
		item.ORG_CHARGE2=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_STD_CHARGE2(long	lParam)
{
		item.STD_CHARGE2=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_CHARGE2(long	lParam)
{
		item.CHARGE2=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_ACCT_ITEM_TYPE_ID2(long	lParam)
{
		item.ACCT_ITEM_TYPE_ID2=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_DISCOUNT_ACCT_ITEM_TYPE_ID2(long	lParam)
{
		item.DISCOUNT_ACCT_ITEM_TYPE_ID2=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_BILL_MEASURE2(long	lParam)
{
		item.BILL_MEASURE2=lParam;
}

void	TBL_DATA_EVENT_CLASS::Set_BILLING_RECORD(char	* sParam)
{
		if ( strlen(sParam)>100 ) 
			 throw TException( " Set_BILLING_RECORD  string over 100!");
		strcpy(item.BILLING_RECORD,sParam);
}

void	TBL_DATA_EVENT_CLASS::Set_PRICING_COMBINE_ID(long	lParam)
{
		item.PRICING_COMBINE_ID=lParam;
}

void	TBL_DATA_EVENT_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_DATA_EVENT_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_DATA_EVENT_CLASS::SetBuf(int iBufLen)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_DATA_EVENT_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_DATA_EVENT_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_DATA_EVENT_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="EVENT_ID,BILL_FLOW_ID,BILLING_CYCLE_ID,EVENT_TYPE_ID,BILLING_AREA_CODE,BILLING_NBR,BILLING_ORG_ID,BILLING_VISIT_AREA_CODE,CALLING_AREA_CODE,CALLING_NBR,ORG_CALLING_NBR,CALLING_SP_TYPE_ID,CALLING_SERVICE_TYPE_ID,CALLED_AREA_CODE,CALLED_NBR,ORG_CALLED_NBR,CALLED_SP_TYPE_ID,CALLED_SERVICE_TYPE_ID,SWITCH_ID,ACCOUNT_AREA_CODE,ACCOUNT_NBR,NAS_IP,NAS_PORT_ID,START_DATE,DURATION,RECV_BYTES,SEND_BYTES,RECV_PACKETS,SEND_PACKETS,FILE_ID,CREATED_DATE,EVENT_STATE,STATE_DATE,CARD_NO,BILLING_TYPE,PARTER_ID,SERV_ID,OFFER_ID1,OFFER_ID2,OFFER_ID3,OFFER_ID4,OFFER_ID5,ORG_CHARGE1,STD_CHARGE1,CHARGE1,ACCT_ITEM_TYPE_ID1,DISCOUNT_ACCT_ITEM_TYPE_ID1,BILL_MEASURE1,ORG_CHARGE2,STD_CHARGE2,CHARGE2,ACCT_ITEM_TYPE_ID2,DISCOUNT_ACCT_ITEM_TYPE_ID2,BILL_MEASURE2,BILLING_RECORD,PRICING_COMBINE_ID";
		ParamList="";
		ParamList+=":EVENT_ID ";
		ParamList+=",";
		ParamList+=":BILL_FLOW_ID ";
		ParamList+=",";
		ParamList+=":BILLING_CYCLE_ID ";
		ParamList+=",";
		ParamList+=":EVENT_TYPE_ID ";
		ParamList+=",";
		ParamList+=":BILLING_AREA_CODE ";
		ParamList+=",";
		ParamList+=":BILLING_NBR ";
		ParamList+=",";
		ParamList+=":BILLING_ORG_ID ";
		ParamList+=",";
		ParamList+=":BILLING_VISIT_AREA_CODE ";
		ParamList+=",";
		ParamList+=":CALLING_AREA_CODE ";
		ParamList+=",";
		ParamList+=":CALLING_NBR ";
		ParamList+=",";
		ParamList+=":ORG_CALLING_NBR ";
		ParamList+=",";
		ParamList+=":CALLING_SP_TYPE_ID ";
		ParamList+=",";
		ParamList+=":CALLING_SERVICE_TYPE_ID ";
		ParamList+=",";
		ParamList+=":CALLED_AREA_CODE ";
		ParamList+=",";
		ParamList+=":CALLED_NBR ";
		ParamList+=",";
		ParamList+=":ORG_CALLED_NBR ";
		ParamList+=",";
		ParamList+=":CALLED_SP_TYPE_ID ";
		ParamList+=",";
		ParamList+=":CALLED_SERVICE_TYPE_ID ";
		ParamList+=",";
		ParamList+=":SWITCH_ID ";
		ParamList+=",";
		ParamList+=":ACCOUNT_AREA_CODE ";
		ParamList+=",";
		ParamList+=":ACCOUNT_NBR ";
		ParamList+=",";
		ParamList+=":NAS_IP ";
		ParamList+=",";
		ParamList+=":NAS_PORT_ID ";
		ParamList+=",";
		if (a_item[0].START_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:START_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":DURATION ";
		ParamList+=",";
		ParamList+=":RECV_BYTES ";
		ParamList+=",";
		ParamList+=":SEND_BYTES ";
		ParamList+=",";
		ParamList+=":RECV_PACKETS ";
		ParamList+=",";
		ParamList+=":SEND_PACKETS ";
		ParamList+=",";
		ParamList+=":FILE_ID ";
		ParamList+=",";
		if (a_item[0].CREATED_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:CREATED_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":EVENT_STATE ";
		ParamList+=",";
		if (a_item[0].STATE_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:STATE_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":CARD_NO ";
		ParamList+=",";
		ParamList+=":BILLING_TYPE ";
		ParamList+=",";
		ParamList+=":PARTER_ID ";
		ParamList+=",";
		ParamList+=":SERV_ID ";
		ParamList+=",";
		ParamList+=":OFFER_ID1 ";
		ParamList+=",";
		ParamList+=":OFFER_ID2 ";
		ParamList+=",";
		ParamList+=":OFFER_ID3 ";
		ParamList+=",";
		ParamList+=":OFFER_ID4 ";
		ParamList+=",";
		ParamList+=":OFFER_ID5 ";
		ParamList+=",";
		ParamList+=":ORG_CHARGE1 ";
		ParamList+=",";
		ParamList+=":STD_CHARGE1 ";
		ParamList+=",";
		ParamList+=":CHARGE1 ";
		ParamList+=",";
		ParamList+=":ACCT_ITEM_TYPE_ID1 ";
		ParamList+=",";
		ParamList+=":DISCOUNT_ACCT_ITEM_TYPE_ID1 ";
		ParamList+=",";
		ParamList+=":BILL_MEASURE1 ";
		ParamList+=",";
		ParamList+=":ORG_CHARGE2 ";
		ParamList+=",";
		ParamList+=":STD_CHARGE2 ";
		ParamList+=",";
		ParamList+=":CHARGE2 ";
		ParamList+=",";
		ParamList+=":ACCT_ITEM_TYPE_ID2 ";
		ParamList+=",";
		ParamList+=":DISCOUNT_ACCT_ITEM_TYPE_ID2 ";
		ParamList+=",";
		ParamList+=":BILL_MEASURE2 ";
		ParamList+=",";
		ParamList+=":BILLING_RECORD ";
		ParamList+=",";
		ParamList+=":PRICING_COMBINE_ID ";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("EVENT_ID",&(a_item[0].EVENT_ID),sizeof(a_item[0]));
			qry.setParamArray("BILL_FLOW_ID",&(a_item[0].BILL_FLOW_ID),sizeof(a_item[0]));
			qry.setParamArray("BILLING_CYCLE_ID",&(a_item[0].BILLING_CYCLE_ID),sizeof(a_item[0]));
			qry.setParamArray("EVENT_TYPE_ID",&(a_item[0].EVENT_TYPE_ID),sizeof(a_item[0]));
			qry.setParamArray("BILLING_AREA_CODE",(char **)&(a_item[0].BILLING_AREA_CODE),sizeof(a_item[0]),sizeof(a_item[0].BILLING_AREA_CODE));
			qry.setParamArray("BILLING_NBR",(char **)&(a_item[0].BILLING_NBR),sizeof(a_item[0]),sizeof(a_item[0].BILLING_NBR));
			qry.setParamArray("BILLING_ORG_ID",&(a_item[0].BILLING_ORG_ID),sizeof(a_item[0]));
			qry.setParamArray("BILLING_VISIT_AREA_CODE",(char **)&(a_item[0].BILLING_VISIT_AREA_CODE),sizeof(a_item[0]),sizeof(a_item[0].BILLING_VISIT_AREA_CODE));
			qry.setParamArray("CALLING_AREA_CODE",(char **)&(a_item[0].CALLING_AREA_CODE),sizeof(a_item[0]),sizeof(a_item[0].CALLING_AREA_CODE));
			qry.setParamArray("CALLING_NBR",(char **)&(a_item[0].CALLING_NBR),sizeof(a_item[0]),sizeof(a_item[0].CALLING_NBR));
			qry.setParamArray("ORG_CALLING_NBR",(char **)&(a_item[0].ORG_CALLING_NBR),sizeof(a_item[0]),sizeof(a_item[0].ORG_CALLING_NBR));
			qry.setParamArray("CALLING_SP_TYPE_ID",&(a_item[0].CALLING_SP_TYPE_ID),sizeof(a_item[0]));
			qry.setParamArray("CALLING_SERVICE_TYPE_ID",&(a_item[0].CALLING_SERVICE_TYPE_ID),sizeof(a_item[0]));
			qry.setParamArray("CALLED_AREA_CODE",(char **)&(a_item[0].CALLED_AREA_CODE),sizeof(a_item[0]),sizeof(a_item[0].CALLED_AREA_CODE));
			qry.setParamArray("CALLED_NBR",(char **)&(a_item[0].CALLED_NBR),sizeof(a_item[0]),sizeof(a_item[0].CALLED_NBR));
			qry.setParamArray("ORG_CALLED_NBR",(char **)&(a_item[0].ORG_CALLED_NBR),sizeof(a_item[0]),sizeof(a_item[0].ORG_CALLED_NBR));
			qry.setParamArray("CALLED_SP_TYPE_ID",&(a_item[0].CALLED_SP_TYPE_ID),sizeof(a_item[0]));
			qry.setParamArray("CALLED_SERVICE_TYPE_ID",&(a_item[0].CALLED_SERVICE_TYPE_ID),sizeof(a_item[0]));
			qry.setParamArray("SWITCH_ID",&(a_item[0].SWITCH_ID),sizeof(a_item[0]));
			qry.setParamArray("ACCOUNT_AREA_CODE",(char **)&(a_item[0].ACCOUNT_AREA_CODE),sizeof(a_item[0]),sizeof(a_item[0].ACCOUNT_AREA_CODE));
			qry.setParamArray("ACCOUNT_NBR",(char **)&(a_item[0].ACCOUNT_NBR),sizeof(a_item[0]),sizeof(a_item[0].ACCOUNT_NBR));
			qry.setParamArray("NAS_IP",(char **)&(a_item[0].NAS_IP),sizeof(a_item[0]),sizeof(a_item[0].NAS_IP));
			qry.setParamArray("NAS_PORT_ID",&(a_item[0].NAS_PORT_ID),sizeof(a_item[0]));
			if (a_item[0].START_DATE[0]!='\0')
				qry.setParamArray("START_DATE",(char **)&(a_item[0].START_DATE),sizeof(a_item[0]),sizeof(a_item[0].START_DATE));
			qry.setParamArray("DURATION",&(a_item[0].DURATION),sizeof(a_item[0]));
			qry.setParamArray("RECV_BYTES",&(a_item[0].RECV_BYTES),sizeof(a_item[0]));
			qry.setParamArray("SEND_BYTES",&(a_item[0].SEND_BYTES),sizeof(a_item[0]));
			qry.setParamArray("RECV_PACKETS",&(a_item[0].RECV_PACKETS),sizeof(a_item[0]));
			qry.setParamArray("SEND_PACKETS",&(a_item[0].SEND_PACKETS),sizeof(a_item[0]));
			qry.setParamArray("FILE_ID",&(a_item[0].FILE_ID),sizeof(a_item[0]));
			if (a_item[0].CREATED_DATE[0]!='\0')
				qry.setParamArray("CREATED_DATE",(char **)&(a_item[0].CREATED_DATE),sizeof(a_item[0]),sizeof(a_item[0].CREATED_DATE));
			qry.setParamArray("EVENT_STATE",(char **)&(a_item[0].EVENT_STATE),sizeof(a_item[0]),sizeof(a_item[0].EVENT_STATE));
			if (a_item[0].STATE_DATE[0]!='\0')
				qry.setParamArray("STATE_DATE",(char **)&(a_item[0].STATE_DATE),sizeof(a_item[0]),sizeof(a_item[0].STATE_DATE));
			qry.setParamArray("CARD_NO",(char **)&(a_item[0].CARD_NO),sizeof(a_item[0]),sizeof(a_item[0].CARD_NO));
			qry.setParamArray("BILLING_TYPE",(char **)&(a_item[0].BILLING_TYPE),sizeof(a_item[0]),sizeof(a_item[0].BILLING_TYPE));
			qry.setParamArray("PARTER_ID",&(a_item[0].PARTER_ID),sizeof(a_item[0]));
			qry.setParamArray("SERV_ID",&(a_item[0].SERV_ID),sizeof(a_item[0]));
			qry.setParamArray("OFFER_ID1",&(a_item[0].OFFER_ID1),sizeof(a_item[0]));
			qry.setParamArray("OFFER_ID2",&(a_item[0].OFFER_ID2),sizeof(a_item[0]));
			qry.setParamArray("OFFER_ID3",&(a_item[0].OFFER_ID3),sizeof(a_item[0]));
			qry.setParamArray("OFFER_ID4",&(a_item[0].OFFER_ID4),sizeof(a_item[0]));
			qry.setParamArray("OFFER_ID5",&(a_item[0].OFFER_ID5),sizeof(a_item[0]));
			qry.setParamArray("ORG_CHARGE1",&(a_item[0].ORG_CHARGE1),sizeof(a_item[0]));
			qry.setParamArray("STD_CHARGE1",&(a_item[0].STD_CHARGE1),sizeof(a_item[0]));
			qry.setParamArray("CHARGE1",&(a_item[0].CHARGE1),sizeof(a_item[0]));
			qry.setParamArray("ACCT_ITEM_TYPE_ID1",&(a_item[0].ACCT_ITEM_TYPE_ID1),sizeof(a_item[0]));
			qry.setParamArray("DISCOUNT_ACCT_ITEM_TYPE_ID1",&(a_item[0].DISCOUNT_ACCT_ITEM_TYPE_ID1),sizeof(a_item[0]));
			qry.setParamArray("BILL_MEASURE1",&(a_item[0].BILL_MEASURE1),sizeof(a_item[0]));
			qry.setParamArray("ORG_CHARGE2",&(a_item[0].ORG_CHARGE2),sizeof(a_item[0]));
			qry.setParamArray("STD_CHARGE2",&(a_item[0].STD_CHARGE2),sizeof(a_item[0]));
			qry.setParamArray("CHARGE2",&(a_item[0].CHARGE2),sizeof(a_item[0]));
			qry.setParamArray("ACCT_ITEM_TYPE_ID2",&(a_item[0].ACCT_ITEM_TYPE_ID2),sizeof(a_item[0]));
			qry.setParamArray("DISCOUNT_ACCT_ITEM_TYPE_ID2",&(a_item[0].DISCOUNT_ACCT_ITEM_TYPE_ID2),sizeof(a_item[0]));
			qry.setParamArray("BILL_MEASURE2",&(a_item[0].BILL_MEASURE2),sizeof(a_item[0]));
			qry.setParamArray("BILLING_RECORD",(char **)&(a_item[0].BILLING_RECORD),sizeof(a_item[0]),sizeof(a_item[0].BILLING_RECORD));
			qry.setParamArray("PRICING_COMBINE_ID",&(a_item[0].PRICING_COMBINE_ID),sizeof(a_item[0]));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_DATA_EVENT_STR_>	MEM_DATA_EVENT_CLASS::a_item;
hash_map<const long ,_TBL_DATA_EVENT_STR_* , hash<long>, MEM_DATA_EVENT_CLASS::eqlong>	MEM_DATA_EVENT_CLASS::hashmap;
bool	MEM_DATA_EVENT_CLASS::bLoadFlag=false;
MEM_DATA_EVENT_CLASS::MEM_DATA_EVENT_CLASS(char *sTableName)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_DATA_EVENT_CLASS::~MEM_DATA_EVENT_CLASS()
{
		/*UnLoad();*/
}

void	MEM_DATA_EVENT_CLASS::Load(char *sTableName)
{
		if (bLoadFlag)	return ;
		TBL_DATA_EVENT_CLASS	 tempread;
		int		i;
		if (sTableName!=NULL)
				tempread.SetTableName(sTableName);
		tempread.Open();
		for(;tempread.Next();){
			a_item.push_back(tempread.item);
		}
		for(i=0;i<a_item.size();i++){
			hashmap[a_item[i].EVENT_ID]=&a_item[i];
		}
		bLoadFlag=true;
}

void	MEM_DATA_EVENT_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
		bLoadFlag=false;
}

_TBL_DATA_EVENT_STR_ *	MEM_DATA_EVENT_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


