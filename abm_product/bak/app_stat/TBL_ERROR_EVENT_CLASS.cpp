/*VER: 1*/ 
#include	"TBL_ERROR_EVENT_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_ERROR_EVENT_CLASS::TBL_ERROR_EVENT_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="ERROR_EVENT";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_ERROR_EVENT_CLASS::~TBL_ERROR_EVENT_CLASS()
{
		FreeBuf();
}

void	TBL_ERROR_EVENT_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_ERROR_EVENT_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_ERROR_EVENT_CLASS::Open()
{
		string strSql;
		strSql="SELECT EVENT_ID,BILL_FLOW_ID,BILLING_ORG_ID,NVL(ORG_CALLING_NBR,-1) ORG_CALLING_NBR,NVL(ORG_CALLED_NBR,-1) ORG_CALLED_NBR,NVL(THIRD_PARTY_AREA_CODE,-1) THIRD_PARTY_AREA_CODE,NVL(TO_CHAR(START_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') START_DATE,NVL(DURATION,-1) DURATION,NVL(TRUNK_IN,-1) TRUNK_IN,NVL(TRUNK_OUT,-1) TRUNK_OUT,NVL(ACCOUNT_AREA_CODE,-1) ACCOUNT_AREA_CODE,NVL(ACCOUNT_NBR,-1) ACCOUNT_NBR,NVL(NAS_IP,-1) NAS_IP,NVL(NAS_PORT_ID,-1) NAS_PORT_ID,NVL(RECV_BYTES,-1) RECV_BYTES,NVL(SEND_BYTES,-1) SEND_BYTES,NVL(RECV_PACKETS,-1) RECV_PACKETS,NVL(SEND_PACKETS,-1) SEND_PACKETS,NVL(SWITCH_ID,-1) SWITCH_ID,NVL(FILE_ID,-1) FILE_ID,NVL(TO_CHAR(CREATED_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') CREATED_DATE,NVL(EVENT_STATE,-1) EVENT_STATE,NVL(TO_CHAR(STATE_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') STATE_DATE,NVL(ERROR_ID,-1) ERROR_ID,NVL(EXTEND_USED_NUM,-1) EXTEND_USED_NUM,NVL(EXTEND_FIELD_ID1,-1) EXTEND_FIELD_ID1,NVL(EXTEND_VALUE1,-1) EXTEND_VALUE1,NVL(EXTEND_FIELD_ID2,-1) EXTEND_FIELD_ID2,NVL(EXTEND_VALUE2,-1) EXTEND_VALUE2,NVL(EXTEND_FIELD_ID3,-1) EXTEND_FIELD_ID3,NVL(EXTEND_VALUE3,-1) EXTEND_VALUE3,NVL(EXTEND_FIELD_ID4,-1) EXTEND_FIELD_ID4,NVL(EXTEND_VALUE4,-1) EXTEND_VALUE4,NVL(EXTEND_FIELD_ID5,-1) EXTEND_FIELD_ID5,NVL(EXTEND_VALUE5,-1) EXTEND_VALUE5,NVL(EXTEND_FIELD_ID6,-1) EXTEND_FIELD_ID6,NVL(EXTEND_VALUE6,-1) EXTEND_VALUE6,NVL(EXTEND_FIELD_ID7,-1) EXTEND_FIELD_ID7,NVL(EXTEND_VALUE7,-1) EXTEND_VALUE7,NVL(EXTEND_FIELD_ID8,-1) EXTEND_FIELD_ID8,NVL(EXTEND_VALUE8,-1) EXTEND_VALUE8,NVL(EXTEND_FIELD_ID9,-1) EXTEND_FIELD_ID9,NVL(EXTEND_VALUE9,-1) EXTEND_VALUE9,NVL(EXTEND_FIELD_ID10,-1) EXTEND_FIELD_ID10,NVL(EXTEND_VALUE10,-1) EXTEND_VALUE10 from ";
		if	(sTableName.size()==0)
			strSql+="ERROR_EVENT";
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
			cout<<"Error occured ... in TBL_ERROR_EVENT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_ERROR_EVENT_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry not open!");
		try{
			if ( !m_qry->next())
				return false;
			item.EVENT_ID=m_qry->field("EVENT_ID").asLong();
			item.BILL_FLOW_ID=m_qry->field("BILL_FLOW_ID").asLong();
			item.BILLING_ORG_ID=m_qry->field("BILLING_ORG_ID").asLong();
			strcpy(item.ORG_CALLING_NBR,m_qry->field("ORG_CALLING_NBR").asString());
			strcpy(item.ORG_CALLED_NBR,m_qry->field("ORG_CALLED_NBR").asString());
			strcpy(item.THIRD_PARTY_AREA_CODE,m_qry->field("THIRD_PARTY_AREA_CODE").asString());
			strcpy(item.START_DATE,m_qry->field("START_DATE").asString());
			item.DURATION=m_qry->field("DURATION").asLong();
			strcpy(item.TRUNK_IN,m_qry->field("TRUNK_IN").asString());
			strcpy(item.TRUNK_OUT,m_qry->field("TRUNK_OUT").asString());
			strcpy(item.ACCOUNT_AREA_CODE,m_qry->field("ACCOUNT_AREA_CODE").asString());
			strcpy(item.ACCOUNT_NBR,m_qry->field("ACCOUNT_NBR").asString());
			strcpy(item.NAS_IP,m_qry->field("NAS_IP").asString());
			item.NAS_PORT_ID=m_qry->field("NAS_PORT_ID").asLong();
			item.RECV_BYTES=m_qry->field("RECV_BYTES").asLong();
			item.SEND_BYTES=m_qry->field("SEND_BYTES").asLong();
			item.RECV_PACKETS=m_qry->field("RECV_PACKETS").asLong();
			item.SEND_PACKETS=m_qry->field("SEND_PACKETS").asLong();
			item.SWITCH_ID=m_qry->field("SWITCH_ID").asLong();
			item.FILE_ID=m_qry->field("FILE_ID").asLong();
			strcpy(item.CREATED_DATE,m_qry->field("CREATED_DATE").asString());
			item.EVENT_STATE=m_qry->field("EVENT_STATE").asLong();
			strcpy(item.STATE_DATE,m_qry->field("STATE_DATE").asString());
			item.ERROR_ID=m_qry->field("ERROR_ID").asLong();
			item.EXTEND_USED_NUM=m_qry->field("EXTEND_USED_NUM").asLong();
			item.EXTEND_FIELD_ID1=m_qry->field("EXTEND_FIELD_ID1").asLong();
			strcpy(item.EXTEND_VALUE1,m_qry->field("EXTEND_VALUE1").asString());
			item.EXTEND_FIELD_ID2=m_qry->field("EXTEND_FIELD_ID2").asLong();
			strcpy(item.EXTEND_VALUE2,m_qry->field("EXTEND_VALUE2").asString());
			item.EXTEND_FIELD_ID3=m_qry->field("EXTEND_FIELD_ID3").asLong();
			strcpy(item.EXTEND_VALUE3,m_qry->field("EXTEND_VALUE3").asString());
			item.EXTEND_FIELD_ID4=m_qry->field("EXTEND_FIELD_ID4").asLong();
			strcpy(item.EXTEND_VALUE4,m_qry->field("EXTEND_VALUE4").asString());
			item.EXTEND_FIELD_ID5=m_qry->field("EXTEND_FIELD_ID5").asLong();
			strcpy(item.EXTEND_VALUE5,m_qry->field("EXTEND_VALUE5").asString());
			item.EXTEND_FIELD_ID6=m_qry->field("EXTEND_FIELD_ID6").asLong();
			strcpy(item.EXTEND_VALUE6,m_qry->field("EXTEND_VALUE6").asString());
			item.EXTEND_FIELD_ID7=m_qry->field("EXTEND_FIELD_ID7").asLong();
			strcpy(item.EXTEND_VALUE7,m_qry->field("EXTEND_VALUE7").asString());
			item.EXTEND_FIELD_ID8=m_qry->field("EXTEND_FIELD_ID8").asLong();
			strcpy(item.EXTEND_VALUE8,m_qry->field("EXTEND_VALUE8").asString());
			item.EXTEND_FIELD_ID9=m_qry->field("EXTEND_FIELD_ID9").asLong();
			strcpy(item.EXTEND_VALUE9,m_qry->field("EXTEND_VALUE9").asString());
			item.EXTEND_FIELD_ID10=m_qry->field("EXTEND_FIELD_ID10").asLong();
			strcpy(item.EXTEND_VALUE10,m_qry->field("EXTEND_VALUE10").asString());
		return true;
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_ERROR_EVENT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_ERROR_EVENT_CLASS::Get_EVENT_ID(void)
{
		return	item.EVENT_ID;
}

long	TBL_ERROR_EVENT_CLASS::Get_BILL_FLOW_ID(void)
{
		return	item.BILL_FLOW_ID;
}

long	TBL_ERROR_EVENT_CLASS::Get_BILLING_ORG_ID(void)
{
		return	item.BILLING_ORG_ID;
}

char*	TBL_ERROR_EVENT_CLASS::Get_ORG_CALLING_NBR(void)
{
		return	item.ORG_CALLING_NBR;
}

char*	TBL_ERROR_EVENT_CLASS::Get_ORG_CALLED_NBR(void)
{
		return	item.ORG_CALLED_NBR;
}

char*	TBL_ERROR_EVENT_CLASS::Get_THIRD_PARTY_AREA_CODE(void)
{
		return	item.THIRD_PARTY_AREA_CODE;
}

char*	TBL_ERROR_EVENT_CLASS::Get_START_DATE(void)
{
		return	item.START_DATE;
}

long	TBL_ERROR_EVENT_CLASS::Get_DURATION(void)
{
		return	item.DURATION;
}

char*	TBL_ERROR_EVENT_CLASS::Get_TRUNK_IN(void)
{
		return	item.TRUNK_IN;
}

char*	TBL_ERROR_EVENT_CLASS::Get_TRUNK_OUT(void)
{
		return	item.TRUNK_OUT;
}

char*	TBL_ERROR_EVENT_CLASS::Get_ACCOUNT_AREA_CODE(void)
{
		return	item.ACCOUNT_AREA_CODE;
}

char*	TBL_ERROR_EVENT_CLASS::Get_ACCOUNT_NBR(void)
{
		return	item.ACCOUNT_NBR;
}

char*	TBL_ERROR_EVENT_CLASS::Get_NAS_IP(void)
{
		return	item.NAS_IP;
}

long	TBL_ERROR_EVENT_CLASS::Get_NAS_PORT_ID(void)
{
		return	item.NAS_PORT_ID;
}

long	TBL_ERROR_EVENT_CLASS::Get_RECV_BYTES(void)
{
		return	item.RECV_BYTES;
}

long	TBL_ERROR_EVENT_CLASS::Get_SEND_BYTES(void)
{
		return	item.SEND_BYTES;
}

long	TBL_ERROR_EVENT_CLASS::Get_RECV_PACKETS(void)
{
		return	item.RECV_PACKETS;
}

long	TBL_ERROR_EVENT_CLASS::Get_SEND_PACKETS(void)
{
		return	item.SEND_PACKETS;
}

long	TBL_ERROR_EVENT_CLASS::Get_SWITCH_ID(void)
{
		return	item.SWITCH_ID;
}

long	TBL_ERROR_EVENT_CLASS::Get_FILE_ID(void)
{
		return	item.FILE_ID;
}

char*	TBL_ERROR_EVENT_CLASS::Get_CREATED_DATE(void)
{
		return	item.CREATED_DATE;
}

long	TBL_ERROR_EVENT_CLASS::Get_EVENT_STATE(void)
{
		return	item.EVENT_STATE;
}

char*	TBL_ERROR_EVENT_CLASS::Get_STATE_DATE(void)
{
		return	item.STATE_DATE;
}

long	TBL_ERROR_EVENT_CLASS::Get_ERROR_ID(void)
{
		return	item.ERROR_ID;
}

long	TBL_ERROR_EVENT_CLASS::Get_EXTEND_USED_NUM(void)
{
		return	item.EXTEND_USED_NUM;
}

long	TBL_ERROR_EVENT_CLASS::Get_EXTEND_FIELD_ID1(void)
{
		return	item.EXTEND_FIELD_ID1;
}

char*	TBL_ERROR_EVENT_CLASS::Get_EXTEND_VALUE1(void)
{
		return	item.EXTEND_VALUE1;
}

long	TBL_ERROR_EVENT_CLASS::Get_EXTEND_FIELD_ID2(void)
{
		return	item.EXTEND_FIELD_ID2;
}

char*	TBL_ERROR_EVENT_CLASS::Get_EXTEND_VALUE2(void)
{
		return	item.EXTEND_VALUE2;
}

long	TBL_ERROR_EVENT_CLASS::Get_EXTEND_FIELD_ID3(void)
{
		return	item.EXTEND_FIELD_ID3;
}

char*	TBL_ERROR_EVENT_CLASS::Get_EXTEND_VALUE3(void)
{
		return	item.EXTEND_VALUE3;
}

long	TBL_ERROR_EVENT_CLASS::Get_EXTEND_FIELD_ID4(void)
{
		return	item.EXTEND_FIELD_ID4;
}

char*	TBL_ERROR_EVENT_CLASS::Get_EXTEND_VALUE4(void)
{
		return	item.EXTEND_VALUE4;
}

long	TBL_ERROR_EVENT_CLASS::Get_EXTEND_FIELD_ID5(void)
{
		return	item.EXTEND_FIELD_ID5;
}

char*	TBL_ERROR_EVENT_CLASS::Get_EXTEND_VALUE5(void)
{
		return	item.EXTEND_VALUE5;
}

long	TBL_ERROR_EVENT_CLASS::Get_EXTEND_FIELD_ID6(void)
{
		return	item.EXTEND_FIELD_ID6;
}

char*	TBL_ERROR_EVENT_CLASS::Get_EXTEND_VALUE6(void)
{
		return	item.EXTEND_VALUE6;
}

long	TBL_ERROR_EVENT_CLASS::Get_EXTEND_FIELD_ID7(void)
{
		return	item.EXTEND_FIELD_ID7;
}

char*	TBL_ERROR_EVENT_CLASS::Get_EXTEND_VALUE7(void)
{
		return	item.EXTEND_VALUE7;
}

long	TBL_ERROR_EVENT_CLASS::Get_EXTEND_FIELD_ID8(void)
{
		return	item.EXTEND_FIELD_ID8;
}

char*	TBL_ERROR_EVENT_CLASS::Get_EXTEND_VALUE8(void)
{
		return	item.EXTEND_VALUE8;
}

long	TBL_ERROR_EVENT_CLASS::Get_EXTEND_FIELD_ID9(void)
{
		return	item.EXTEND_FIELD_ID9;
}

char*	TBL_ERROR_EVENT_CLASS::Get_EXTEND_VALUE9(void)
{
		return	item.EXTEND_VALUE9;
}

long	TBL_ERROR_EVENT_CLASS::Get_EXTEND_FIELD_ID10(void)
{
		return	item.EXTEND_FIELD_ID10;
}

char*	TBL_ERROR_EVENT_CLASS::Get_EXTEND_VALUE10(void)
{
		return	item.EXTEND_VALUE10;
}

void	TBL_ERROR_EVENT_CLASS::Set_EVENT_ID(long	lParam)
{
		item.EVENT_ID=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_BILL_FLOW_ID(long	lParam)
{
		item.BILL_FLOW_ID=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_BILLING_ORG_ID(long	lParam)
{
		item.BILLING_ORG_ID=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_ORG_CALLING_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_ORG_CALLING_NBR  string over 32!");
		strcpy(item.ORG_CALLING_NBR,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_ORG_CALLED_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_ORG_CALLED_NBR  string over 32!");
		strcpy(item.ORG_CALLED_NBR,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_THIRD_PARTY_AREA_CODE(char	* sParam)
{
		if ( strlen(sParam)>10 ) 
			 throw TException( " Set_THIRD_PARTY_AREA_CODE  string over 10!");
		strcpy(item.THIRD_PARTY_AREA_CODE,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_START_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_START_DATE  string over 14!");
		strcpy(item.START_DATE,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_DURATION(long	lParam)
{
		item.DURATION=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_TRUNK_IN(char	* sParam)
{
		if ( strlen(sParam)>12 ) 
			 throw TException( " Set_TRUNK_IN  string over 12!");
		strcpy(item.TRUNK_IN,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_TRUNK_OUT(char	* sParam)
{
		if ( strlen(sParam)>12 ) 
			 throw TException( " Set_TRUNK_OUT  string over 12!");
		strcpy(item.TRUNK_OUT,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_ACCOUNT_AREA_CODE(char	* sParam)
{
		if ( strlen(sParam)>10 ) 
			 throw TException( " Set_ACCOUNT_AREA_CODE  string over 10!");
		strcpy(item.ACCOUNT_AREA_CODE,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_ACCOUNT_NBR(char	* sParam)
{
		if ( strlen(sParam)>30 ) 
			 throw TException( " Set_ACCOUNT_NBR  string over 30!");
		strcpy(item.ACCOUNT_NBR,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_NAS_IP(char	* sParam)
{
		if ( strlen(sParam)>20 ) 
			 throw TException( " Set_NAS_IP  string over 20!");
		strcpy(item.NAS_IP,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_NAS_PORT_ID(long	lParam)
{
		item.NAS_PORT_ID=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_RECV_BYTES(long	lParam)
{
		item.RECV_BYTES=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_SEND_BYTES(long	lParam)
{
		item.SEND_BYTES=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_RECV_PACKETS(long	lParam)
{
		item.RECV_PACKETS=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_SEND_PACKETS(long	lParam)
{
		item.SEND_PACKETS=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_SWITCH_ID(long	lParam)
{
		item.SWITCH_ID=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_FILE_ID(long	lParam)
{
		item.FILE_ID=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_CREATED_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_CREATED_DATE  string over 14!");
		strcpy(item.CREATED_DATE,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_EVENT_STATE(long	lParam)
{
		item.EVENT_STATE=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_STATE_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_STATE_DATE  string over 14!");
		strcpy(item.STATE_DATE,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_ERROR_ID(long	lParam)
{
		item.ERROR_ID=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_USED_NUM(long	lParam)
{
		item.EXTEND_USED_NUM=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_FIELD_ID1(long	lParam)
{
		item.EXTEND_FIELD_ID1=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_VALUE1(char	* sParam)
{
		if ( strlen(sParam)>100 ) 
			 throw TException( " Set_EXTEND_VALUE1  string over 100!");
		strcpy(item.EXTEND_VALUE1,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_FIELD_ID2(long	lParam)
{
		item.EXTEND_FIELD_ID2=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_VALUE2(char	* sParam)
{
		if ( strlen(sParam)>100 ) 
			 throw TException( " Set_EXTEND_VALUE2  string over 100!");
		strcpy(item.EXTEND_VALUE2,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_FIELD_ID3(long	lParam)
{
		item.EXTEND_FIELD_ID3=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_VALUE3(char	* sParam)
{
		if ( strlen(sParam)>100 ) 
			 throw TException( " Set_EXTEND_VALUE3  string over 100!");
		strcpy(item.EXTEND_VALUE3,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_FIELD_ID4(long	lParam)
{
		item.EXTEND_FIELD_ID4=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_VALUE4(char	* sParam)
{
		if ( strlen(sParam)>100 ) 
			 throw TException( " Set_EXTEND_VALUE4  string over 100!");
		strcpy(item.EXTEND_VALUE4,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_FIELD_ID5(long	lParam)
{
		item.EXTEND_FIELD_ID5=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_VALUE5(char	* sParam)
{
		if ( strlen(sParam)>100 ) 
			 throw TException( " Set_EXTEND_VALUE5  string over 100!");
		strcpy(item.EXTEND_VALUE5,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_FIELD_ID6(long	lParam)
{
		item.EXTEND_FIELD_ID6=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_VALUE6(char	* sParam)
{
		if ( strlen(sParam)>100 ) 
			 throw TException( " Set_EXTEND_VALUE6  string over 100!");
		strcpy(item.EXTEND_VALUE6,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_FIELD_ID7(long	lParam)
{
		item.EXTEND_FIELD_ID7=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_VALUE7(char	* sParam)
{
		if ( strlen(sParam)>100 ) 
			 throw TException( " Set_EXTEND_VALUE7  string over 100!");
		strcpy(item.EXTEND_VALUE7,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_FIELD_ID8(long	lParam)
{
		item.EXTEND_FIELD_ID8=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_VALUE8(char	* sParam)
{
		if ( strlen(sParam)>100 ) 
			 throw TException( " Set_EXTEND_VALUE8  string over 100!");
		strcpy(item.EXTEND_VALUE8,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_FIELD_ID9(long	lParam)
{
		item.EXTEND_FIELD_ID9=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_VALUE9(char	* sParam)
{
		if ( strlen(sParam)>100 ) 
			 throw TException( " Set_EXTEND_VALUE9  string over 100!");
		strcpy(item.EXTEND_VALUE9,sParam);
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_FIELD_ID10(long	lParam)
{
		item.EXTEND_FIELD_ID10=lParam;
}

void	TBL_ERROR_EVENT_CLASS::Set_EXTEND_VALUE10(char	* sParam)
{
		if ( strlen(sParam)>100 ) 
			 throw TException( " Set_EXTEND_VALUE10  string over 100!");
		strcpy(item.EXTEND_VALUE10,sParam);
}

void	TBL_ERROR_EVENT_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_ERROR_EVENT_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_ERROR_EVENT_CLASS::SetBuf(int iBufLen)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_ERROR_EVENT_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_ERROR_EVENT_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_ERROR_EVENT_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="EVENT_ID,BILL_FLOW_ID,BILLING_ORG_ID,ORG_CALLING_NBR,ORG_CALLED_NBR,THIRD_PARTY_AREA_CODE,START_DATE,DURATION,TRUNK_IN,TRUNK_OUT,ACCOUNT_AREA_CODE,ACCOUNT_NBR,NAS_IP,NAS_PORT_ID,RECV_BYTES,SEND_BYTES,RECV_PACKETS,SEND_PACKETS,SWITCH_ID,FILE_ID,CREATED_DATE,EVENT_STATE,STATE_DATE,ERROR_ID,EXTEND_USED_NUM,EXTEND_FIELD_ID1,EXTEND_VALUE1,EXTEND_FIELD_ID2,EXTEND_VALUE2,EXTEND_FIELD_ID3,EXTEND_VALUE3,EXTEND_FIELD_ID4,EXTEND_VALUE4,EXTEND_FIELD_ID5,EXTEND_VALUE5,EXTEND_FIELD_ID6,EXTEND_VALUE6,EXTEND_FIELD_ID7,EXTEND_VALUE7,EXTEND_FIELD_ID8,EXTEND_VALUE8,EXTEND_FIELD_ID9,EXTEND_VALUE9,EXTEND_FIELD_ID10,EXTEND_VALUE10";
		ParamList="";
		ParamList+=":EVENT_ID ";
		ParamList+=",";
		ParamList+=":BILL_FLOW_ID ";
		ParamList+=",";
		ParamList+=":BILLING_ORG_ID ";
		ParamList+=",";
		ParamList+=":ORG_CALLING_NBR ";
		ParamList+=",";
		ParamList+=":ORG_CALLED_NBR ";
		ParamList+=",";
		ParamList+=":THIRD_PARTY_AREA_CODE ";
		ParamList+=",";
		if (a_item[0].START_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:START_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":DURATION ";
		ParamList+=",";
		ParamList+=":TRUNK_IN ";
		ParamList+=",";
		ParamList+=":TRUNK_OUT ";
		ParamList+=",";
		ParamList+=":ACCOUNT_AREA_CODE ";
		ParamList+=",";
		ParamList+=":ACCOUNT_NBR ";
		ParamList+=",";
		ParamList+=":NAS_IP ";
		ParamList+=",";
		ParamList+=":NAS_PORT_ID ";
		ParamList+=",";
		ParamList+=":RECV_BYTES ";
		ParamList+=",";
		ParamList+=":SEND_BYTES ";
		ParamList+=",";
		ParamList+=":RECV_PACKETS ";
		ParamList+=",";
		ParamList+=":SEND_PACKETS ";
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
		ParamList+=":ERROR_ID ";
		ParamList+=",";
		ParamList+=":EXTEND_USED_NUM ";
		ParamList+=",";
		ParamList+=":EXTEND_FIELD_ID1 ";
		ParamList+=",";
		ParamList+=":EXTEND_VALUE1 ";
		ParamList+=",";
		ParamList+=":EXTEND_FIELD_ID2 ";
		ParamList+=",";
		ParamList+=":EXTEND_VALUE2 ";
		ParamList+=",";
		ParamList+=":EXTEND_FIELD_ID3 ";
		ParamList+=",";
		ParamList+=":EXTEND_VALUE3 ";
		ParamList+=",";
		ParamList+=":EXTEND_FIELD_ID4 ";
		ParamList+=",";
		ParamList+=":EXTEND_VALUE4 ";
		ParamList+=",";
		ParamList+=":EXTEND_FIELD_ID5 ";
		ParamList+=",";
		ParamList+=":EXTEND_VALUE5 ";
		ParamList+=",";
		ParamList+=":EXTEND_FIELD_ID6 ";
		ParamList+=",";
		ParamList+=":EXTEND_VALUE6 ";
		ParamList+=",";
		ParamList+=":EXTEND_FIELD_ID7 ";
		ParamList+=",";
		ParamList+=":EXTEND_VALUE7 ";
		ParamList+=",";
		ParamList+=":EXTEND_FIELD_ID8 ";
		ParamList+=",";
		ParamList+=":EXTEND_VALUE8 ";
		ParamList+=",";
		ParamList+=":EXTEND_FIELD_ID9 ";
		ParamList+=",";
		ParamList+=":EXTEND_VALUE9 ";
		ParamList+=",";
		ParamList+=":EXTEND_FIELD_ID10 ";
		ParamList+=",";
		ParamList+=":EXTEND_VALUE10 ";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("EVENT_ID",&(a_item[0].EVENT_ID),sizeof(a_item[0]));
			qry.setParamArray("BILL_FLOW_ID",&(a_item[0].BILL_FLOW_ID),sizeof(a_item[0]));
			qry.setParamArray("BILLING_ORG_ID",&(a_item[0].BILLING_ORG_ID),sizeof(a_item[0]));
			qry.setParamArray("ORG_CALLING_NBR",(char **)&(a_item[0].ORG_CALLING_NBR),sizeof(a_item[0]),sizeof(a_item[0].ORG_CALLING_NBR));
			qry.setParamArray("ORG_CALLED_NBR",(char **)&(a_item[0].ORG_CALLED_NBR),sizeof(a_item[0]),sizeof(a_item[0].ORG_CALLED_NBR));
			qry.setParamArray("THIRD_PARTY_AREA_CODE",(char **)&(a_item[0].THIRD_PARTY_AREA_CODE),sizeof(a_item[0]),sizeof(a_item[0].THIRD_PARTY_AREA_CODE));
			if (a_item[0].START_DATE[0]!='\0')
				qry.setParamArray("START_DATE",(char **)&(a_item[0].START_DATE),sizeof(a_item[0]),sizeof(a_item[0].START_DATE));
			qry.setParamArray("DURATION",&(a_item[0].DURATION),sizeof(a_item[0]));
			qry.setParamArray("TRUNK_IN",(char **)&(a_item[0].TRUNK_IN),sizeof(a_item[0]),sizeof(a_item[0].TRUNK_IN));
			qry.setParamArray("TRUNK_OUT",(char **)&(a_item[0].TRUNK_OUT),sizeof(a_item[0]),sizeof(a_item[0].TRUNK_OUT));
			qry.setParamArray("ACCOUNT_AREA_CODE",(char **)&(a_item[0].ACCOUNT_AREA_CODE),sizeof(a_item[0]),sizeof(a_item[0].ACCOUNT_AREA_CODE));
			qry.setParamArray("ACCOUNT_NBR",(char **)&(a_item[0].ACCOUNT_NBR),sizeof(a_item[0]),sizeof(a_item[0].ACCOUNT_NBR));
			qry.setParamArray("NAS_IP",(char **)&(a_item[0].NAS_IP),sizeof(a_item[0]),sizeof(a_item[0].NAS_IP));
			qry.setParamArray("NAS_PORT_ID",&(a_item[0].NAS_PORT_ID),sizeof(a_item[0]));
			qry.setParamArray("RECV_BYTES",&(a_item[0].RECV_BYTES),sizeof(a_item[0]));
			qry.setParamArray("SEND_BYTES",&(a_item[0].SEND_BYTES),sizeof(a_item[0]));
			qry.setParamArray("RECV_PACKETS",&(a_item[0].RECV_PACKETS),sizeof(a_item[0]));
			qry.setParamArray("SEND_PACKETS",&(a_item[0].SEND_PACKETS),sizeof(a_item[0]));
			qry.setParamArray("SWITCH_ID",&(a_item[0].SWITCH_ID),sizeof(a_item[0]));
			qry.setParamArray("FILE_ID",&(a_item[0].FILE_ID),sizeof(a_item[0]));
			if (a_item[0].CREATED_DATE[0]!='\0')
				qry.setParamArray("CREATED_DATE",(char **)&(a_item[0].CREATED_DATE),sizeof(a_item[0]),sizeof(a_item[0].CREATED_DATE));
			qry.setParamArray("EVENT_STATE",&(a_item[0].EVENT_STATE),sizeof(a_item[0]));
			if (a_item[0].STATE_DATE[0]!='\0')
				qry.setParamArray("STATE_DATE",(char **)&(a_item[0].STATE_DATE),sizeof(a_item[0]),sizeof(a_item[0].STATE_DATE));
			qry.setParamArray("ERROR_ID",&(a_item[0].ERROR_ID),sizeof(a_item[0]));
			qry.setParamArray("EXTEND_USED_NUM",&(a_item[0].EXTEND_USED_NUM),sizeof(a_item[0]));
			qry.setParamArray("EXTEND_FIELD_ID1",&(a_item[0].EXTEND_FIELD_ID1),sizeof(a_item[0]));
			qry.setParamArray("EXTEND_VALUE1",(char **)&(a_item[0].EXTEND_VALUE1),sizeof(a_item[0]),sizeof(a_item[0].EXTEND_VALUE1));
			qry.setParamArray("EXTEND_FIELD_ID2",&(a_item[0].EXTEND_FIELD_ID2),sizeof(a_item[0]));
			qry.setParamArray("EXTEND_VALUE2",(char **)&(a_item[0].EXTEND_VALUE2),sizeof(a_item[0]),sizeof(a_item[0].EXTEND_VALUE2));
			qry.setParamArray("EXTEND_FIELD_ID3",&(a_item[0].EXTEND_FIELD_ID3),sizeof(a_item[0]));
			qry.setParamArray("EXTEND_VALUE3",(char **)&(a_item[0].EXTEND_VALUE3),sizeof(a_item[0]),sizeof(a_item[0].EXTEND_VALUE3));
			qry.setParamArray("EXTEND_FIELD_ID4",&(a_item[0].EXTEND_FIELD_ID4),sizeof(a_item[0]));
			qry.setParamArray("EXTEND_VALUE4",(char **)&(a_item[0].EXTEND_VALUE4),sizeof(a_item[0]),sizeof(a_item[0].EXTEND_VALUE4));
			qry.setParamArray("EXTEND_FIELD_ID5",&(a_item[0].EXTEND_FIELD_ID5),sizeof(a_item[0]));
			qry.setParamArray("EXTEND_VALUE5",(char **)&(a_item[0].EXTEND_VALUE5),sizeof(a_item[0]),sizeof(a_item[0].EXTEND_VALUE5));
			qry.setParamArray("EXTEND_FIELD_ID6",&(a_item[0].EXTEND_FIELD_ID6),sizeof(a_item[0]));
			qry.setParamArray("EXTEND_VALUE6",(char **)&(a_item[0].EXTEND_VALUE6),sizeof(a_item[0]),sizeof(a_item[0].EXTEND_VALUE6));
			qry.setParamArray("EXTEND_FIELD_ID7",&(a_item[0].EXTEND_FIELD_ID7),sizeof(a_item[0]));
			qry.setParamArray("EXTEND_VALUE7",(char **)&(a_item[0].EXTEND_VALUE7),sizeof(a_item[0]),sizeof(a_item[0].EXTEND_VALUE7));
			qry.setParamArray("EXTEND_FIELD_ID8",&(a_item[0].EXTEND_FIELD_ID8),sizeof(a_item[0]));
			qry.setParamArray("EXTEND_VALUE8",(char **)&(a_item[0].EXTEND_VALUE8),sizeof(a_item[0]),sizeof(a_item[0].EXTEND_VALUE8));
			qry.setParamArray("EXTEND_FIELD_ID9",&(a_item[0].EXTEND_FIELD_ID9),sizeof(a_item[0]));
			qry.setParamArray("EXTEND_VALUE9",(char **)&(a_item[0].EXTEND_VALUE9),sizeof(a_item[0]),sizeof(a_item[0].EXTEND_VALUE9));
			qry.setParamArray("EXTEND_FIELD_ID10",&(a_item[0].EXTEND_FIELD_ID10),sizeof(a_item[0]));
			qry.setParamArray("EXTEND_VALUE10",(char **)&(a_item[0].EXTEND_VALUE10),sizeof(a_item[0]),sizeof(a_item[0].EXTEND_VALUE10));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_ERROR_EVENT_STR_>	MEM_ERROR_EVENT_CLASS::a_item;
hash_map<const long ,_TBL_ERROR_EVENT_STR_* , hash<long>, MEM_ERROR_EVENT_CLASS::eqlong>	MEM_ERROR_EVENT_CLASS::hashmap;
bool	MEM_ERROR_EVENT_CLASS::bLoadFlag=false;
MEM_ERROR_EVENT_CLASS::MEM_ERROR_EVENT_CLASS(char *sTableName)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_ERROR_EVENT_CLASS::~MEM_ERROR_EVENT_CLASS()
{
		/*UnLoad();*/
}

void	MEM_ERROR_EVENT_CLASS::Load(char *sTableName)
{
		if (bLoadFlag)	return ;
		TBL_ERROR_EVENT_CLASS	 tempread;
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

void	MEM_ERROR_EVENT_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
		bLoadFlag=false;
}

_TBL_ERROR_EVENT_STR_ *	MEM_ERROR_EVENT_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


