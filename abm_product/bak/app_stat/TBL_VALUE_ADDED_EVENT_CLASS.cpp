/*VER: 1*/ 
#include	"TBL_VALUE_ADDED_EVENT_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_VALUE_ADDED_EVENT_CLASS::TBL_VALUE_ADDED_EVENT_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="VALUE_ADDED_EVENT";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_VALUE_ADDED_EVENT_CLASS::~TBL_VALUE_ADDED_EVENT_CLASS()
{
		FreeBuf();
}

void	TBL_VALUE_ADDED_EVENT_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Open()
{
		string strSql;
		strSql="SELECT BILLING_ORG_ID,NVL(BILLING_VISIT_AREA_CODE,-1) BILLING_VISIT_AREA_CODE,NVL(CALLING_AREA_CODE,-1) CALLING_AREA_CODE,NVL(CALLING_NBR,-1) CALLING_NBR,NVL(ORG_CALLING_NBR,-1) ORG_CALLING_NBR,NVL(CALLING_SP_TYPE_ID,-1) CALLING_SP_TYPE_ID,NVL(CALLING_SERVICE_TYPE_ID,-1) CALLING_SERVICE_TYPE_ID,NVL(CALLED_AREA_CODE,-1) CALLED_AREA_CODE,NVL(CALLED_NBR,-1) CALLED_NBR,NVL(ORG_CALLED_NBR,-1) ORG_CALLED_NBR,NVL(CALLED_SP_TYPE_ID,-1) CALLED_SP_TYPE_ID,NVL(CALLED_SERVICE_TYPE_ID,-1) CALLED_SERVICE_TYPE_ID,NVL(THIRD_PARTY_AREA_CODE,-1) THIRD_PARTY_AREA_CODE,NVL(THIRD_PARTY_NBR,-1) THIRD_PARTY_NBR,NVL(TO_CHAR(START_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') START_DATE,NVL(DURATION,-1) DURATION,NVL(SWITCH_ID,-1) SWITCH_ID,NVL(FILE_ID,-1) FILE_ID,NVL(TO_CHAR(CREATED_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') CREATED_DATE,NVL(EVENT_STATE,-1) EVENT_STATE,NVL(TO_CHAR(STATE_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') STATE_DATE,NVL(CARD_NO,-1) CARD_NO,NVL(BILLING_TYPE,-1) BILLING_TYPE,NVL(PARTER_ID,-1) PARTER_ID,SERV_ID,NVL(OFFER_ID1,-1) OFFER_ID1,NVL(OFFER_ID2,-1) OFFER_ID2,NVL(OFFER_ID3,-1) OFFER_ID3,NVL(OFFER_ID4,-1) OFFER_ID4,NVL(OFFER_ID5,-1) OFFER_ID5,NVL(ORG_CHARGE1,-1) ORG_CHARGE1,NVL(STD_CHARGE1,-1) STD_CHARGE1,NVL(CHARGE1,-1) CHARGE1,NVL(ACCT_ITEM_TYPE_ID1,-1) ACCT_ITEM_TYPE_ID1,NVL(DISCOUNT_ACCT_ITEM_TYPE_ID1,-1) DISCOUNT_ACCT_ITEM_TYPE_ID1,NVL(BILL_MEASURE1,-1) BILL_MEASURE1,NVL(ORG_CHARGE2,-1) ORG_CHARGE2,NVL(STD_CHARGE2,-1) STD_CHARGE2,NVL(CHARGE2,-1) CHARGE2,NVL(ACCT_ITEM_TYPE_ID2,-1) ACCT_ITEM_TYPE_ID2,NVL(BILL_MEASURE2,-1) BILL_MEASURE2,NVL(DISCOUNT_ACCT_ITEM_TYPE_ID2,-1) DISCOUNT_ACCT_ITEM_TYPE_ID2,NVL(PRICING_COMBINE_ID,-1) PRICING_COMBINE_ID,EVENT_ID,BILL_FLOW_ID,BILLING_CYCLE_ID,EVENT_TYPE_ID,NVL(BILLING_AREA_CODE,-1) BILLING_AREA_CODE,NVL(BILLING_NBR,-1) BILLING_NBR from ";
		if	(sTableName.size()==0)
			strSql+="VALUE_ADDED_EVENT";
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
			cout<<"Error occured ... in TBL_VALUE_ADDED_EVENT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_VALUE_ADDED_EVENT_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry not open!");
		try{
			if ( !m_qry->next())
				return false;
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
			strcpy(item.THIRD_PARTY_AREA_CODE,m_qry->field("THIRD_PARTY_AREA_CODE").asString());
			strcpy(item.THIRD_PARTY_NBR,m_qry->field("THIRD_PARTY_NBR").asString());
			strcpy(item.START_DATE,m_qry->field("START_DATE").asString());
			item.DURATION=m_qry->field("DURATION").asLong();
			strcpy(item.SWITCH_ID,m_qry->field("SWITCH_ID").asString());
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
			item.BILL_MEASURE2=m_qry->field("BILL_MEASURE2").asLong();
			item.DISCOUNT_ACCT_ITEM_TYPE_ID2=m_qry->field("DISCOUNT_ACCT_ITEM_TYPE_ID2").asLong();
			item.PRICING_COMBINE_ID=m_qry->field("PRICING_COMBINE_ID").asLong();
			item.EVENT_ID=m_qry->field("EVENT_ID").asLong();
			item.BILL_FLOW_ID=m_qry->field("BILL_FLOW_ID").asLong();
			item.BILLING_CYCLE_ID=m_qry->field("BILLING_CYCLE_ID").asLong();
			item.EVENT_TYPE_ID=m_qry->field("EVENT_TYPE_ID").asLong();
			strcpy(item.BILLING_AREA_CODE,m_qry->field("BILLING_AREA_CODE").asString());
			strcpy(item.BILLING_NBR,m_qry->field("BILLING_NBR").asString());
		return true;
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_VALUE_ADDED_EVENT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_BILLING_ORG_ID(void)
{
		return	item.BILLING_ORG_ID;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_BILLING_VISIT_AREA_CODE(void)
{
		return	item.BILLING_VISIT_AREA_CODE;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_CALLING_AREA_CODE(void)
{
		return	item.CALLING_AREA_CODE;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_CALLING_NBR(void)
{
		return	item.CALLING_NBR;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_ORG_CALLING_NBR(void)
{
		return	item.ORG_CALLING_NBR;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_CALLING_SP_TYPE_ID(void)
{
		return	item.CALLING_SP_TYPE_ID;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_CALLING_SERVICE_TYPE_ID(void)
{
		return	item.CALLING_SERVICE_TYPE_ID;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_CALLED_AREA_CODE(void)
{
		return	item.CALLED_AREA_CODE;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_CALLED_NBR(void)
{
		return	item.CALLED_NBR;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_ORG_CALLED_NBR(void)
{
		return	item.ORG_CALLED_NBR;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_CALLED_SP_TYPE_ID(void)
{
		return	item.CALLED_SP_TYPE_ID;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_CALLED_SERVICE_TYPE_ID(void)
{
		return	item.CALLED_SERVICE_TYPE_ID;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_THIRD_PARTY_AREA_CODE(void)
{
		return	item.THIRD_PARTY_AREA_CODE;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_THIRD_PARTY_NBR(void)
{
		return	item.THIRD_PARTY_NBR;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_START_DATE(void)
{
		return	item.START_DATE;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_DURATION(void)
{
		return	item.DURATION;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_SWITCH_ID(void)
{
		return	item.SWITCH_ID;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_FILE_ID(void)
{
		return	item.FILE_ID;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_CREATED_DATE(void)
{
		return	item.CREATED_DATE;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_EVENT_STATE(void)
{
		return	item.EVENT_STATE;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_STATE_DATE(void)
{
		return	item.STATE_DATE;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_CARD_NO(void)
{
		return	item.CARD_NO;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_BILLING_TYPE(void)
{
		return	item.BILLING_TYPE;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_PARTER_ID(void)
{
		return	item.PARTER_ID;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_SERV_ID(void)
{
		return	item.SERV_ID;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_OFFER_ID1(void)
{
		return	item.OFFER_ID1;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_OFFER_ID2(void)
{
		return	item.OFFER_ID2;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_OFFER_ID3(void)
{
		return	item.OFFER_ID3;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_OFFER_ID4(void)
{
		return	item.OFFER_ID4;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_OFFER_ID5(void)
{
		return	item.OFFER_ID5;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_ORG_CHARGE1(void)
{
		return	item.ORG_CHARGE1;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_STD_CHARGE1(void)
{
		return	item.STD_CHARGE1;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_CHARGE1(void)
{
		return	item.CHARGE1;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_ACCT_ITEM_TYPE_ID1(void)
{
		return	item.ACCT_ITEM_TYPE_ID1;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_DISCOUNT_ACCT_ITEM_TYPE_ID1(void)
{
		return	item.DISCOUNT_ACCT_ITEM_TYPE_ID1;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_BILL_MEASURE1(void)
{
		return	item.BILL_MEASURE1;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_ORG_CHARGE2(void)
{
		return	item.ORG_CHARGE2;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_STD_CHARGE2(void)
{
		return	item.STD_CHARGE2;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_CHARGE2(void)
{
		return	item.CHARGE2;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_ACCT_ITEM_TYPE_ID2(void)
{
		return	item.ACCT_ITEM_TYPE_ID2;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_BILL_MEASURE2(void)
{
		return	item.BILL_MEASURE2;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_DISCOUNT_ACCT_ITEM_TYPE_ID2(void)
{
		return	item.DISCOUNT_ACCT_ITEM_TYPE_ID2;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_PRICING_COMBINE_ID(void)
{
		return	item.PRICING_COMBINE_ID;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_EVENT_ID(void)
{
		return	item.EVENT_ID;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_BILL_FLOW_ID(void)
{
		return	item.BILL_FLOW_ID;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_BILLING_CYCLE_ID(void)
{
		return	item.BILLING_CYCLE_ID;
}

long	TBL_VALUE_ADDED_EVENT_CLASS::Get_EVENT_TYPE_ID(void)
{
		return	item.EVENT_TYPE_ID;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_BILLING_AREA_CODE(void)
{
		return	item.BILLING_AREA_CODE;
}

char*	TBL_VALUE_ADDED_EVENT_CLASS::Get_BILLING_NBR(void)
{
		return	item.BILLING_NBR;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_BILLING_ORG_ID(long	lParam)
{
		item.BILLING_ORG_ID=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_BILLING_VISIT_AREA_CODE(char	* sParam)
{
		if ( strlen(sParam)>10 ) 
			 throw TException( " Set_BILLING_VISIT_AREA_CODE  string over 10!");
		strcpy(item.BILLING_VISIT_AREA_CODE,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_CALLING_AREA_CODE(char	* sParam)
{
		if ( strlen(sParam)>10 ) 
			 throw TException( " Set_CALLING_AREA_CODE  string over 10!");
		strcpy(item.CALLING_AREA_CODE,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_CALLING_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_CALLING_NBR  string over 32!");
		strcpy(item.CALLING_NBR,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_ORG_CALLING_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_ORG_CALLING_NBR  string over 32!");
		strcpy(item.ORG_CALLING_NBR,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_CALLING_SP_TYPE_ID(long	lParam)
{
		item.CALLING_SP_TYPE_ID=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_CALLING_SERVICE_TYPE_ID(long	lParam)
{
		item.CALLING_SERVICE_TYPE_ID=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_CALLED_AREA_CODE(char	* sParam)
{
		if ( strlen(sParam)>10 ) 
			 throw TException( " Set_CALLED_AREA_CODE  string over 10!");
		strcpy(item.CALLED_AREA_CODE,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_CALLED_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_CALLED_NBR  string over 32!");
		strcpy(item.CALLED_NBR,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_ORG_CALLED_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_ORG_CALLED_NBR  string over 32!");
		strcpy(item.ORG_CALLED_NBR,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_CALLED_SP_TYPE_ID(long	lParam)
{
		item.CALLED_SP_TYPE_ID=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_CALLED_SERVICE_TYPE_ID(long	lParam)
{
		item.CALLED_SERVICE_TYPE_ID=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_THIRD_PARTY_AREA_CODE(char	* sParam)
{
		if ( strlen(sParam)>10 ) 
			 throw TException( " Set_THIRD_PARTY_AREA_CODE  string over 10!");
		strcpy(item.THIRD_PARTY_AREA_CODE,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_THIRD_PARTY_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_THIRD_PARTY_NBR  string over 32!");
		strcpy(item.THIRD_PARTY_NBR,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_START_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_START_DATE  string over 14!");
		strcpy(item.START_DATE,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_DURATION(long	lParam)
{
		item.DURATION=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_SWITCH_ID(char	* sParam)
{
		if ( strlen(sParam)>10 ) 
			 throw TException( " Set_SWITCH_ID  string over 10!");
		strcpy(item.SWITCH_ID,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_FILE_ID(long	lParam)
{
		item.FILE_ID=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_CREATED_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_CREATED_DATE  string over 14!");
		strcpy(item.CREATED_DATE,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_EVENT_STATE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_EVENT_STATE  string over 3!");
		strcpy(item.EVENT_STATE,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_STATE_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_STATE_DATE  string over 14!");
		strcpy(item.STATE_DATE,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_CARD_NO(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_CARD_NO  string over 32!");
		strcpy(item.CARD_NO,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_BILLING_TYPE(char	* sParam)
{
		if ( strlen(sParam)>1 ) 
			 throw TException( " Set_BILLING_TYPE  string over 1!");
		strcpy(item.BILLING_TYPE,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_PARTER_ID(long	lParam)
{
		item.PARTER_ID=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_SERV_ID(long	lParam)
{
		item.SERV_ID=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_OFFER_ID1(long	lParam)
{
		item.OFFER_ID1=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_OFFER_ID2(long	lParam)
{
		item.OFFER_ID2=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_OFFER_ID3(long	lParam)
{
		item.OFFER_ID3=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_OFFER_ID4(long	lParam)
{
		item.OFFER_ID4=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_OFFER_ID5(long	lParam)
{
		item.OFFER_ID5=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_ORG_CHARGE1(long	lParam)
{
		item.ORG_CHARGE1=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_STD_CHARGE1(long	lParam)
{
		item.STD_CHARGE1=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_CHARGE1(long	lParam)
{
		item.CHARGE1=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_ACCT_ITEM_TYPE_ID1(long	lParam)
{
		item.ACCT_ITEM_TYPE_ID1=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_DISCOUNT_ACCT_ITEM_TYPE_ID1(long	lParam)
{
		item.DISCOUNT_ACCT_ITEM_TYPE_ID1=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_BILL_MEASURE1(long	lParam)
{
		item.BILL_MEASURE1=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_ORG_CHARGE2(long	lParam)
{
		item.ORG_CHARGE2=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_STD_CHARGE2(long	lParam)
{
		item.STD_CHARGE2=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_CHARGE2(long	lParam)
{
		item.CHARGE2=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_ACCT_ITEM_TYPE_ID2(long	lParam)
{
		item.ACCT_ITEM_TYPE_ID2=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_BILL_MEASURE2(long	lParam)
{
		item.BILL_MEASURE2=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_DISCOUNT_ACCT_ITEM_TYPE_ID2(long	lParam)
{
		item.DISCOUNT_ACCT_ITEM_TYPE_ID2=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_PRICING_COMBINE_ID(long	lParam)
{
		item.PRICING_COMBINE_ID=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_EVENT_ID(long	lParam)
{
		item.EVENT_ID=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_BILL_FLOW_ID(long	lParam)
{
		item.BILL_FLOW_ID=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_BILLING_CYCLE_ID(long	lParam)
{
		item.BILLING_CYCLE_ID=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_EVENT_TYPE_ID(long	lParam)
{
		item.EVENT_TYPE_ID=lParam;
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_BILLING_AREA_CODE(char	* sParam)
{
		if ( strlen(sParam)>10 ) 
			 throw TException( " Set_BILLING_AREA_CODE  string over 10!");
		strcpy(item.BILLING_AREA_CODE,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::Set_BILLING_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_BILLING_NBR  string over 32!");
		strcpy(item.BILLING_NBR,sParam);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_VALUE_ADDED_EVENT_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_VALUE_ADDED_EVENT_CLASS::SetBuf(int iBufLen)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_VALUE_ADDED_EVENT_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_VALUE_ADDED_EVENT_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_VALUE_ADDED_EVENT_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="BILLING_ORG_ID,BILLING_VISIT_AREA_CODE,CALLING_AREA_CODE,CALLING_NBR,ORG_CALLING_NBR,CALLING_SP_TYPE_ID,CALLING_SERVICE_TYPE_ID,CALLED_AREA_CODE,CALLED_NBR,ORG_CALLED_NBR,CALLED_SP_TYPE_ID,CALLED_SERVICE_TYPE_ID,THIRD_PARTY_AREA_CODE,THIRD_PARTY_NBR,START_DATE,DURATION,SWITCH_ID,FILE_ID,CREATED_DATE,EVENT_STATE,STATE_DATE,CARD_NO,BILLING_TYPE,PARTER_ID,SERV_ID,OFFER_ID1,OFFER_ID2,OFFER_ID3,OFFER_ID4,OFFER_ID5,ORG_CHARGE1,STD_CHARGE1,CHARGE1,ACCT_ITEM_TYPE_ID1,DISCOUNT_ACCT_ITEM_TYPE_ID1,BILL_MEASURE1,ORG_CHARGE2,STD_CHARGE2,CHARGE2,ACCT_ITEM_TYPE_ID2,BILL_MEASURE2,DISCOUNT_ACCT_ITEM_TYPE_ID2,PRICING_COMBINE_ID,EVENT_ID,BILL_FLOW_ID,BILLING_CYCLE_ID,EVENT_TYPE_ID,BILLING_AREA_CODE,BILLING_NBR";
		ParamList="";
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
		ParamList+=":THIRD_PARTY_AREA_CODE ";
		ParamList+=",";
		ParamList+=":THIRD_PARTY_NBR ";
		ParamList+=",";
		if (a_item[0].START_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:START_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":DURATION ";
		ParamList+=",";
		ParamList+=":SWITCH_ID ";
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
		ParamList+=":BILL_MEASURE2 ";
		ParamList+=",";
		ParamList+=":DISCOUNT_ACCT_ITEM_TYPE_ID2 ";
		ParamList+=",";
		ParamList+=":PRICING_COMBINE_ID ";
		ParamList+=",";
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
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
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
			qry.setParamArray("THIRD_PARTY_AREA_CODE",(char **)&(a_item[0].THIRD_PARTY_AREA_CODE),sizeof(a_item[0]),sizeof(a_item[0].THIRD_PARTY_AREA_CODE));
			qry.setParamArray("THIRD_PARTY_NBR",(char **)&(a_item[0].THIRD_PARTY_NBR),sizeof(a_item[0]),sizeof(a_item[0].THIRD_PARTY_NBR));
			if (a_item[0].START_DATE[0]!='\0')
				qry.setParamArray("START_DATE",(char **)&(a_item[0].START_DATE),sizeof(a_item[0]),sizeof(a_item[0].START_DATE));
			qry.setParamArray("DURATION",&(a_item[0].DURATION),sizeof(a_item[0]));
			qry.setParamArray("SWITCH_ID",(char **)&(a_item[0].SWITCH_ID),sizeof(a_item[0]),sizeof(a_item[0].SWITCH_ID));
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
			qry.setParamArray("BILL_MEASURE2",&(a_item[0].BILL_MEASURE2),sizeof(a_item[0]));
			qry.setParamArray("DISCOUNT_ACCT_ITEM_TYPE_ID2",&(a_item[0].DISCOUNT_ACCT_ITEM_TYPE_ID2),sizeof(a_item[0]));
			qry.setParamArray("PRICING_COMBINE_ID",&(a_item[0].PRICING_COMBINE_ID),sizeof(a_item[0]));
			qry.setParamArray("EVENT_ID",&(a_item[0].EVENT_ID),sizeof(a_item[0]));
			qry.setParamArray("BILL_FLOW_ID",&(a_item[0].BILL_FLOW_ID),sizeof(a_item[0]));
			qry.setParamArray("BILLING_CYCLE_ID",&(a_item[0].BILLING_CYCLE_ID),sizeof(a_item[0]));
			qry.setParamArray("EVENT_TYPE_ID",&(a_item[0].EVENT_TYPE_ID),sizeof(a_item[0]));
			qry.setParamArray("BILLING_AREA_CODE",(char **)&(a_item[0].BILLING_AREA_CODE),sizeof(a_item[0]),sizeof(a_item[0].BILLING_AREA_CODE));
			qry.setParamArray("BILLING_NBR",(char **)&(a_item[0].BILLING_NBR),sizeof(a_item[0]),sizeof(a_item[0].BILLING_NBR));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_VALUE_ADDED_EVENT_STR_>	MEM_VALUE_ADDED_EVENT_CLASS::a_item;
hash_map<const long ,_TBL_VALUE_ADDED_EVENT_STR_* , hash<long>, MEM_VALUE_ADDED_EVENT_CLASS::eqlong>	MEM_VALUE_ADDED_EVENT_CLASS::hashmap;
bool	MEM_VALUE_ADDED_EVENT_CLASS::bLoadFlag=false;
MEM_VALUE_ADDED_EVENT_CLASS::MEM_VALUE_ADDED_EVENT_CLASS(char *sTableName)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_VALUE_ADDED_EVENT_CLASS::~MEM_VALUE_ADDED_EVENT_CLASS()
{
		/*UnLoad();*/
}

void	MEM_VALUE_ADDED_EVENT_CLASS::Load(char *sTableName)
{
		if (bLoadFlag)	return ;
		TBL_VALUE_ADDED_EVENT_CLASS	 tempread;
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

void	MEM_VALUE_ADDED_EVENT_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
		bLoadFlag=false;
}

_TBL_VALUE_ADDED_EVENT_STR_ *	MEM_VALUE_ADDED_EVENT_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


