/*VER: 1*/ 
#include	"TBL_CYCLE_FEE_EVENT_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_CYCLE_FEE_EVENT_CLASS::TBL_CYCLE_FEE_EVENT_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="CYCLE_FEE_EVENT";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_CYCLE_FEE_EVENT_CLASS::~TBL_CYCLE_FEE_EVENT_CLASS()
{
		FreeBuf();
}

void	TBL_CYCLE_FEE_EVENT_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Open()
{
		string strSql;
		strSql="SELECT EVENT_ID,BILL_FLOW_ID,BILLING_CYCLE_ID,EVENT_TYPE_ID,BILLING_ORG_ID,NVL(FILE_ID,-1) FILE_ID,NVL(TO_CHAR(CREATED_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') CREATED_DATE,NVL(EVENT_STATE,-1) EVENT_STATE,NVL(TO_CHAR(STATE_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') STATE_DATE,NVL(PARTER_ID,-1) PARTER_ID,SERV_ID,NVL(SERV_PRODUCT_ID,-1) SERV_PRODUCT_ID,NVL(TO_CHAR(CYCLE_BEGIN_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') CYCLE_BEGIN_DATE,NVL(TO_CHAR(CYCLE_END_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') CYCLE_END_DATE,NVL(SERV_PROD_ATTR_USED_NUM,-1) SERV_PROD_ATTR_USED_NUM,NVL(ATTR_ID1,-1) ATTR_ID1,NVL(ATTR_VALUE1,-1) ATTR_VALUE1,NVL(TO_CHAR(ATTR_EFF_DATE1,'YYYYMMDDHH24MISS') ,'00000000000000') ATTR_EFF_DATE1,NVL(TO_CHAR(ATTR_EXP_DATE1,'YYYYMMDDHH24MISS') ,'00000000000000') ATTR_EXP_DATE1,NVL(ATTR_ID2,-1) ATTR_ID2,NVL(ATTR_VALUE2,-1) ATTR_VALUE2,NVL(TO_CHAR(ATTR_EFF_DATE2,'YYYYMMDDHH24MISS') ,'00000000000000') ATTR_EFF_DATE2,NVL(TO_CHAR(ATTR_EXP_DATE2,'YYYYMMDDHH24MISS') ,'00000000000000') ATTR_EXP_DATE2,NVL(OFFER_ID1,-1) OFFER_ID1,NVL(OFFER_ID2,-1) OFFER_ID2,NVL(OFFER_ID3,-1) OFFER_ID3,NVL(OFFER_ID4,-1) OFFER_ID4,NVL(OFFER_ID5,-1) OFFER_ID5,NVL(STD_CHARGE1,-1) STD_CHARGE1,NVL(CHARGE1,-1) CHARGE1,NVL(ACCT_ITEM_TYPE_ID1,-1) ACCT_ITEM_TYPE_ID1,NVL(DISCOUNT_ACCT_ITEM_TYPE_ID1,-1) DISCOUNT_ACCT_ITEM_TYPE_ID1,NVL(BILL_MEASURE1,-1) BILL_MEASURE1,NVL(BILLING_RECORD,-1) BILLING_RECORD,NVL(PRICING_COMBINE_ID,-1) PRICING_COMBINE_ID,NVL(TO_CHAR(START_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') START_DATE,NVL(TO_CHAR(END_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') END_DATE from ";
		if	(sTableName.size()==0)
			strSql+="CYCLE_FEE_EVENT";
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
			cout<<"Error occured ... in TBL_CYCLE_FEE_EVENT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_CYCLE_FEE_EVENT_CLASS::Next()
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
			item.BILLING_ORG_ID=m_qry->field("BILLING_ORG_ID").asLong();
			item.FILE_ID=m_qry->field("FILE_ID").asLong();
			strcpy(item.CREATED_DATE,m_qry->field("CREATED_DATE").asString());
			strcpy(item.EVENT_STATE,m_qry->field("EVENT_STATE").asString());
			strcpy(item.STATE_DATE,m_qry->field("STATE_DATE").asString());
			item.PARTER_ID=m_qry->field("PARTER_ID").asLong();
			item.SERV_ID=m_qry->field("SERV_ID").asLong();
			item.SERV_PRODUCT_ID=m_qry->field("SERV_PRODUCT_ID").asLong();
			strcpy(item.CYCLE_BEGIN_DATE,m_qry->field("CYCLE_BEGIN_DATE").asString());
			strcpy(item.CYCLE_END_DATE,m_qry->field("CYCLE_END_DATE").asString());
			item.SERV_PROD_ATTR_USED_NUM=m_qry->field("SERV_PROD_ATTR_USED_NUM").asLong();
			item.ATTR_ID1=m_qry->field("ATTR_ID1").asLong();
			strcpy(item.ATTR_VALUE1,m_qry->field("ATTR_VALUE1").asString());
			strcpy(item.ATTR_EFF_DATE1,m_qry->field("ATTR_EFF_DATE1").asString());
			strcpy(item.ATTR_EXP_DATE1,m_qry->field("ATTR_EXP_DATE1").asString());
			item.ATTR_ID2=m_qry->field("ATTR_ID2").asLong();
			strcpy(item.ATTR_VALUE2,m_qry->field("ATTR_VALUE2").asString());
			strcpy(item.ATTR_EFF_DATE2,m_qry->field("ATTR_EFF_DATE2").asString());
			strcpy(item.ATTR_EXP_DATE2,m_qry->field("ATTR_EXP_DATE2").asString());
			item.OFFER_ID1=m_qry->field("OFFER_ID1").asLong();
			item.OFFER_ID2=m_qry->field("OFFER_ID2").asLong();
			item.OFFER_ID3=m_qry->field("OFFER_ID3").asLong();
			item.OFFER_ID4=m_qry->field("OFFER_ID4").asLong();
			item.OFFER_ID5=m_qry->field("OFFER_ID5").asLong();
			item.STD_CHARGE1=m_qry->field("STD_CHARGE1").asLong();
			item.CHARGE1=m_qry->field("CHARGE1").asLong();
			item.ACCT_ITEM_TYPE_ID1=m_qry->field("ACCT_ITEM_TYPE_ID1").asLong();
			item.DISCOUNT_ACCT_ITEM_TYPE_ID1=m_qry->field("DISCOUNT_ACCT_ITEM_TYPE_ID1").asLong();
			item.BILL_MEASURE1=m_qry->field("BILL_MEASURE1").asLong();
			strcpy(item.BILLING_RECORD,m_qry->field("BILLING_RECORD").asString());
			item.PRICING_COMBINE_ID=m_qry->field("PRICING_COMBINE_ID").asLong();
			strcpy(item.START_DATE,m_qry->field("START_DATE").asString());
			strcpy(item.END_DATE,m_qry->field("END_DATE").asString());
		return true;
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_CYCLE_FEE_EVENT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_EVENT_ID(void)
{
		return	item.EVENT_ID;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_BILL_FLOW_ID(void)
{
		return	item.BILL_FLOW_ID;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_BILLING_CYCLE_ID(void)
{
		return	item.BILLING_CYCLE_ID;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_EVENT_TYPE_ID(void)
{
		return	item.EVENT_TYPE_ID;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_BILLING_ORG_ID(void)
{
		return	item.BILLING_ORG_ID;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_FILE_ID(void)
{
		return	item.FILE_ID;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_CREATED_DATE(void)
{
		return	item.CREATED_DATE;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_EVENT_STATE(void)
{
		return	item.EVENT_STATE;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_STATE_DATE(void)
{
		return	item.STATE_DATE;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_PARTER_ID(void)
{
		return	item.PARTER_ID;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_SERV_ID(void)
{
		return	item.SERV_ID;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_SERV_PRODUCT_ID(void)
{
		return	item.SERV_PRODUCT_ID;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_CYCLE_BEGIN_DATE(void)
{
		return	item.CYCLE_BEGIN_DATE;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_CYCLE_END_DATE(void)
{
		return	item.CYCLE_END_DATE;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_SERV_PROD_ATTR_USED_NUM(void)
{
		return	item.SERV_PROD_ATTR_USED_NUM;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_ATTR_ID1(void)
{
		return	item.ATTR_ID1;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_ATTR_VALUE1(void)
{
		return	item.ATTR_VALUE1;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_ATTR_EFF_DATE1(void)
{
		return	item.ATTR_EFF_DATE1;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_ATTR_EXP_DATE1(void)
{
		return	item.ATTR_EXP_DATE1;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_ATTR_ID2(void)
{
		return	item.ATTR_ID2;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_ATTR_VALUE2(void)
{
		return	item.ATTR_VALUE2;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_ATTR_EFF_DATE2(void)
{
		return	item.ATTR_EFF_DATE2;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_ATTR_EXP_DATE2(void)
{
		return	item.ATTR_EXP_DATE2;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_OFFER_ID1(void)
{
		return	item.OFFER_ID1;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_OFFER_ID2(void)
{
		return	item.OFFER_ID2;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_OFFER_ID3(void)
{
		return	item.OFFER_ID3;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_OFFER_ID4(void)
{
		return	item.OFFER_ID4;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_OFFER_ID5(void)
{
		return	item.OFFER_ID5;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_STD_CHARGE1(void)
{
		return	item.STD_CHARGE1;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_CHARGE1(void)
{
		return	item.CHARGE1;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_ACCT_ITEM_TYPE_ID1(void)
{
		return	item.ACCT_ITEM_TYPE_ID1;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_DISCOUNT_ACCT_ITEM_TYPE_ID1(void)
{
		return	item.DISCOUNT_ACCT_ITEM_TYPE_ID1;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_BILL_MEASURE1(void)
{
		return	item.BILL_MEASURE1;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_BILLING_RECORD(void)
{
		return	item.BILLING_RECORD;
}

long	TBL_CYCLE_FEE_EVENT_CLASS::Get_PRICING_COMBINE_ID(void)
{
		return	item.PRICING_COMBINE_ID;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_START_DATE(void)
{
		return	item.START_DATE;
}

char*	TBL_CYCLE_FEE_EVENT_CLASS::Get_END_DATE(void)
{
		return	item.END_DATE;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_EVENT_ID(long	lParam)
{
		item.EVENT_ID=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_BILL_FLOW_ID(long	lParam)
{
		item.BILL_FLOW_ID=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_BILLING_CYCLE_ID(long	lParam)
{
		item.BILLING_CYCLE_ID=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_EVENT_TYPE_ID(long	lParam)
{
		item.EVENT_TYPE_ID=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_BILLING_ORG_ID(long	lParam)
{
		item.BILLING_ORG_ID=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_FILE_ID(long	lParam)
{
		item.FILE_ID=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_CREATED_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_CREATED_DATE  string over 14!");
		strcpy(item.CREATED_DATE,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_EVENT_STATE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_EVENT_STATE  string over 3!");
		strcpy(item.EVENT_STATE,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_STATE_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_STATE_DATE  string over 14!");
		strcpy(item.STATE_DATE,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_PARTER_ID(long	lParam)
{
		item.PARTER_ID=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_SERV_ID(long	lParam)
{
		item.SERV_ID=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_SERV_PRODUCT_ID(long	lParam)
{
		item.SERV_PRODUCT_ID=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_CYCLE_BEGIN_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_CYCLE_BEGIN_DATE  string over 14!");
		strcpy(item.CYCLE_BEGIN_DATE,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_CYCLE_END_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_CYCLE_END_DATE  string over 14!");
		strcpy(item.CYCLE_END_DATE,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_SERV_PROD_ATTR_USED_NUM(long	lParam)
{
		item.SERV_PROD_ATTR_USED_NUM=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_ATTR_ID1(long	lParam)
{
		item.ATTR_ID1=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_ATTR_VALUE1(char	* sParam)
{
		if ( strlen(sParam)>30 ) 
			 throw TException( " Set_ATTR_VALUE1  string over 30!");
		strcpy(item.ATTR_VALUE1,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_ATTR_EFF_DATE1(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_ATTR_EFF_DATE1  string over 14!");
		strcpy(item.ATTR_EFF_DATE1,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_ATTR_EXP_DATE1(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_ATTR_EXP_DATE1  string over 14!");
		strcpy(item.ATTR_EXP_DATE1,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_ATTR_ID2(long	lParam)
{
		item.ATTR_ID2=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_ATTR_VALUE2(char	* sParam)
{
		if ( strlen(sParam)>30 ) 
			 throw TException( " Set_ATTR_VALUE2  string over 30!");
		strcpy(item.ATTR_VALUE2,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_ATTR_EFF_DATE2(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_ATTR_EFF_DATE2  string over 14!");
		strcpy(item.ATTR_EFF_DATE2,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_ATTR_EXP_DATE2(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_ATTR_EXP_DATE2  string over 14!");
		strcpy(item.ATTR_EXP_DATE2,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_OFFER_ID1(long	lParam)
{
		item.OFFER_ID1=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_OFFER_ID2(long	lParam)
{
		item.OFFER_ID2=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_OFFER_ID3(long	lParam)
{
		item.OFFER_ID3=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_OFFER_ID4(long	lParam)
{
		item.OFFER_ID4=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_OFFER_ID5(long	lParam)
{
		item.OFFER_ID5=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_STD_CHARGE1(long	lParam)
{
		item.STD_CHARGE1=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_CHARGE1(long	lParam)
{
		item.CHARGE1=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_ACCT_ITEM_TYPE_ID1(long	lParam)
{
		item.ACCT_ITEM_TYPE_ID1=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_DISCOUNT_ACCT_ITEM_TYPE_ID1(long	lParam)
{
		item.DISCOUNT_ACCT_ITEM_TYPE_ID1=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_BILL_MEASURE1(long	lParam)
{
		item.BILL_MEASURE1=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_BILLING_RECORD(char	* sParam)
{
		if ( strlen(sParam)>100 ) 
			 throw TException( " Set_BILLING_RECORD  string over 100!");
		strcpy(item.BILLING_RECORD,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_PRICING_COMBINE_ID(long	lParam)
{
		item.PRICING_COMBINE_ID=lParam;
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_START_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_START_DATE  string over 14!");
		strcpy(item.START_DATE,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::Set_END_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_END_DATE  string over 14!");
		strcpy(item.END_DATE,sParam);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_CYCLE_FEE_EVENT_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_CYCLE_FEE_EVENT_CLASS::SetBuf(int iBufLen)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_CYCLE_FEE_EVENT_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_CYCLE_FEE_EVENT_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_CYCLE_FEE_EVENT_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="EVENT_ID,BILL_FLOW_ID,BILLING_CYCLE_ID,EVENT_TYPE_ID,BILLING_ORG_ID,FILE_ID,CREATED_DATE,EVENT_STATE,STATE_DATE,PARTER_ID,SERV_ID,SERV_PRODUCT_ID,CYCLE_BEGIN_DATE,CYCLE_END_DATE,SERV_PROD_ATTR_USED_NUM,ATTR_ID1,ATTR_VALUE1,ATTR_EFF_DATE1,ATTR_EXP_DATE1,ATTR_ID2,ATTR_VALUE2,ATTR_EFF_DATE2,ATTR_EXP_DATE2,OFFER_ID1,OFFER_ID2,OFFER_ID3,OFFER_ID4,OFFER_ID5,STD_CHARGE1,CHARGE1,ACCT_ITEM_TYPE_ID1,DISCOUNT_ACCT_ITEM_TYPE_ID1,BILL_MEASURE1,BILLING_RECORD,PRICING_COMBINE_ID,START_DATE,END_DATE";
		ParamList="";
		ParamList+=":EVENT_ID ";
		ParamList+=",";
		ParamList+=":BILL_FLOW_ID ";
		ParamList+=",";
		ParamList+=":BILLING_CYCLE_ID ";
		ParamList+=",";
		ParamList+=":EVENT_TYPE_ID ";
		ParamList+=",";
		ParamList+=":BILLING_ORG_ID ";
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
		ParamList+=":PARTER_ID ";
		ParamList+=",";
		ParamList+=":SERV_ID ";
		ParamList+=",";
		ParamList+=":SERV_PRODUCT_ID ";
		ParamList+=",";
		if (a_item[0].CYCLE_BEGIN_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:CYCLE_BEGIN_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		if (a_item[0].CYCLE_END_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:CYCLE_END_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":SERV_PROD_ATTR_USED_NUM ";
		ParamList+=",";
		ParamList+=":ATTR_ID1 ";
		ParamList+=",";
		ParamList+=":ATTR_VALUE1 ";
		ParamList+=",";
		if (a_item[0].ATTR_EFF_DATE1[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:ATTR_EFF_DATE1,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		if (a_item[0].ATTR_EXP_DATE1[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:ATTR_EXP_DATE1,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":ATTR_ID2 ";
		ParamList+=",";
		ParamList+=":ATTR_VALUE2 ";
		ParamList+=",";
		if (a_item[0].ATTR_EFF_DATE2[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:ATTR_EFF_DATE2,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		if (a_item[0].ATTR_EXP_DATE2[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:ATTR_EXP_DATE2,'YYYYMMDDHH24MISS')";
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
		ParamList+=":BILLING_RECORD ";
		ParamList+=",";
		ParamList+=":PRICING_COMBINE_ID ";
		ParamList+=",";
		if (a_item[0].START_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:START_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		if (a_item[0].END_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:END_DATE,'YYYYMMDDHH24MISS')";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("EVENT_ID",&(a_item[0].EVENT_ID),sizeof(a_item[0]));
			qry.setParamArray("BILL_FLOW_ID",&(a_item[0].BILL_FLOW_ID),sizeof(a_item[0]));
			qry.setParamArray("BILLING_CYCLE_ID",&(a_item[0].BILLING_CYCLE_ID),sizeof(a_item[0]));
			qry.setParamArray("EVENT_TYPE_ID",&(a_item[0].EVENT_TYPE_ID),sizeof(a_item[0]));
			qry.setParamArray("BILLING_ORG_ID",&(a_item[0].BILLING_ORG_ID),sizeof(a_item[0]));
			qry.setParamArray("FILE_ID",&(a_item[0].FILE_ID),sizeof(a_item[0]));
			if (a_item[0].CREATED_DATE[0]!='\0')
				qry.setParamArray("CREATED_DATE",(char **)&(a_item[0].CREATED_DATE),sizeof(a_item[0]),sizeof(a_item[0].CREATED_DATE));
			qry.setParamArray("EVENT_STATE",(char **)&(a_item[0].EVENT_STATE),sizeof(a_item[0]),sizeof(a_item[0].EVENT_STATE));
			if (a_item[0].STATE_DATE[0]!='\0')
				qry.setParamArray("STATE_DATE",(char **)&(a_item[0].STATE_DATE),sizeof(a_item[0]),sizeof(a_item[0].STATE_DATE));
			qry.setParamArray("PARTER_ID",&(a_item[0].PARTER_ID),sizeof(a_item[0]));
			qry.setParamArray("SERV_ID",&(a_item[0].SERV_ID),sizeof(a_item[0]));
			qry.setParamArray("SERV_PRODUCT_ID",&(a_item[0].SERV_PRODUCT_ID),sizeof(a_item[0]));
			if (a_item[0].CYCLE_BEGIN_DATE[0]!='\0')
				qry.setParamArray("CYCLE_BEGIN_DATE",(char **)&(a_item[0].CYCLE_BEGIN_DATE),sizeof(a_item[0]),sizeof(a_item[0].CYCLE_BEGIN_DATE));
			if (a_item[0].CYCLE_END_DATE[0]!='\0')
				qry.setParamArray("CYCLE_END_DATE",(char **)&(a_item[0].CYCLE_END_DATE),sizeof(a_item[0]),sizeof(a_item[0].CYCLE_END_DATE));
			qry.setParamArray("SERV_PROD_ATTR_USED_NUM",&(a_item[0].SERV_PROD_ATTR_USED_NUM),sizeof(a_item[0]));
			qry.setParamArray("ATTR_ID1",&(a_item[0].ATTR_ID1),sizeof(a_item[0]));
			qry.setParamArray("ATTR_VALUE1",(char **)&(a_item[0].ATTR_VALUE1),sizeof(a_item[0]),sizeof(a_item[0].ATTR_VALUE1));
			if (a_item[0].ATTR_EFF_DATE1[0]!='\0')
				qry.setParamArray("ATTR_EFF_DATE1",(char **)&(a_item[0].ATTR_EFF_DATE1),sizeof(a_item[0]),sizeof(a_item[0].ATTR_EFF_DATE1));
			if (a_item[0].ATTR_EXP_DATE1[0]!='\0')
				qry.setParamArray("ATTR_EXP_DATE1",(char **)&(a_item[0].ATTR_EXP_DATE1),sizeof(a_item[0]),sizeof(a_item[0].ATTR_EXP_DATE1));
			qry.setParamArray("ATTR_ID2",&(a_item[0].ATTR_ID2),sizeof(a_item[0]));
			qry.setParamArray("ATTR_VALUE2",(char **)&(a_item[0].ATTR_VALUE2),sizeof(a_item[0]),sizeof(a_item[0].ATTR_VALUE2));
			if (a_item[0].ATTR_EFF_DATE2[0]!='\0')
				qry.setParamArray("ATTR_EFF_DATE2",(char **)&(a_item[0].ATTR_EFF_DATE2),sizeof(a_item[0]),sizeof(a_item[0].ATTR_EFF_DATE2));
			if (a_item[0].ATTR_EXP_DATE2[0]!='\0')
				qry.setParamArray("ATTR_EXP_DATE2",(char **)&(a_item[0].ATTR_EXP_DATE2),sizeof(a_item[0]),sizeof(a_item[0].ATTR_EXP_DATE2));
			qry.setParamArray("OFFER_ID1",&(a_item[0].OFFER_ID1),sizeof(a_item[0]));
			qry.setParamArray("OFFER_ID2",&(a_item[0].OFFER_ID2),sizeof(a_item[0]));
			qry.setParamArray("OFFER_ID3",&(a_item[0].OFFER_ID3),sizeof(a_item[0]));
			qry.setParamArray("OFFER_ID4",&(a_item[0].OFFER_ID4),sizeof(a_item[0]));
			qry.setParamArray("OFFER_ID5",&(a_item[0].OFFER_ID5),sizeof(a_item[0]));
			qry.setParamArray("STD_CHARGE1",&(a_item[0].STD_CHARGE1),sizeof(a_item[0]));
			qry.setParamArray("CHARGE1",&(a_item[0].CHARGE1),sizeof(a_item[0]));
			qry.setParamArray("ACCT_ITEM_TYPE_ID1",&(a_item[0].ACCT_ITEM_TYPE_ID1),sizeof(a_item[0]));
			qry.setParamArray("DISCOUNT_ACCT_ITEM_TYPE_ID1",&(a_item[0].DISCOUNT_ACCT_ITEM_TYPE_ID1),sizeof(a_item[0]));
			qry.setParamArray("BILL_MEASURE1",&(a_item[0].BILL_MEASURE1),sizeof(a_item[0]));
			qry.setParamArray("BILLING_RECORD",(char **)&(a_item[0].BILLING_RECORD),sizeof(a_item[0]),sizeof(a_item[0].BILLING_RECORD));
			qry.setParamArray("PRICING_COMBINE_ID",&(a_item[0].PRICING_COMBINE_ID),sizeof(a_item[0]));
			if (a_item[0].START_DATE[0]!='\0')
				qry.setParamArray("START_DATE",(char **)&(a_item[0].START_DATE),sizeof(a_item[0]),sizeof(a_item[0].START_DATE));
			if (a_item[0].END_DATE[0]!='\0')
				qry.setParamArray("END_DATE",(char **)&(a_item[0].END_DATE),sizeof(a_item[0]),sizeof(a_item[0].END_DATE));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_CYCLE_FEE_EVENT_STR_>	MEM_CYCLE_FEE_EVENT_CLASS::a_item;
hash_map<const long ,_TBL_CYCLE_FEE_EVENT_STR_* , hash<long>, MEM_CYCLE_FEE_EVENT_CLASS::eqlong>	MEM_CYCLE_FEE_EVENT_CLASS::hashmap;
MEM_CYCLE_FEE_EVENT_CLASS::MEM_CYCLE_FEE_EVENT_CLASS(char *sTableName)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_CYCLE_FEE_EVENT_CLASS::~MEM_CYCLE_FEE_EVENT_CLASS()
{
		/*UnLoad();*/
}

void	MEM_CYCLE_FEE_EVENT_CLASS::Load(char *sTableName)
{
		if (a_item.size()>0)	return ;
		TBL_CYCLE_FEE_EVENT_CLASS	 tempread;
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
}

void	MEM_CYCLE_FEE_EVENT_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
}

_TBL_CYCLE_FEE_EVENT_STR_ *	MEM_CYCLE_FEE_EVENT_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


