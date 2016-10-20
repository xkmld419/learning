/*VER: 1*/ 
#include	"TBL_BILL_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_BILL_CLASS::TBL_BILL_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="BILL";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_BILL_CLASS::~TBL_BILL_CLASS()
{
		FreeBuf();
}

void	TBL_BILL_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_BILL_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_BILL_CLASS::Open()
{
		string strSql;
		strSql="SELECT BILL_ID,NVL(PAYMENT_ID,-1) PAYMENT_ID,NVL(PAYMENT_METHOD,-1) PAYMENT_METHOD,BILLING_CYCLE_ID,ACCT_ID,NVL(SERV_ID,-1) SERV_ID,NVL(ACC_NBR,-1) ACC_NBR,BILL_AMOUNT,LATE_FEE,DERATE_LATE_FEE,BALANCE,LAST_CHANGE,CUR_CHANGE,TO_CHAR(CREATED_DATE,'YYYYMMDDHH24MISS') CREATED_DATE,TO_CHAR(PAYMENT_DATE,'YYYYMMDDHH24MISS') PAYMENT_DATE,USE_DERATE_BLANCE,NVL(INVOICE_ID,-1) INVOICE_ID,STATE,TO_CHAR(STATE_DATE,'YYYYMMDDHH24MISS') STATE_DATE,STAFF_ID from ";
		if	(sTableName.size()==0)
			strSql+="BILL";
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
			cout<<"Error occured ... in TBL_BILL_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_BILL_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry not open!");
		try{
			if ( !m_qry->next())
				return false;
			item.BILL_ID=m_qry->field("BILL_ID").asLong();
			item.PAYMENT_ID=m_qry->field("PAYMENT_ID").asLong();
			item.PAYMENT_METHOD=m_qry->field("PAYMENT_METHOD").asLong();
			item.BILLING_CYCLE_ID=m_qry->field("BILLING_CYCLE_ID").asLong();
			item.ACCT_ID=m_qry->field("ACCT_ID").asLong();
			item.SERV_ID=m_qry->field("SERV_ID").asLong();
			strcpy(item.ACC_NBR,m_qry->field("ACC_NBR").asString());
			item.BILL_AMOUNT=m_qry->field("BILL_AMOUNT").asLong();
			item.LATE_FEE=m_qry->field("LATE_FEE").asLong();
			item.DERATE_LATE_FEE=m_qry->field("DERATE_LATE_FEE").asLong();
			item.BALANCE=m_qry->field("BALANCE").asLong();
			item.LAST_CHANGE=m_qry->field("LAST_CHANGE").asLong();
			item.CUR_CHANGE=m_qry->field("CUR_CHANGE").asLong();
			strcpy(item.CREATED_DATE,m_qry->field("CREATED_DATE").asString());
			strcpy(item.PAYMENT_DATE,m_qry->field("PAYMENT_DATE").asString());
			item.USE_DERATE_BLANCE=m_qry->field("USE_DERATE_BLANCE").asLong();
			item.INVOICE_ID=m_qry->field("INVOICE_ID").asLong();
			strcpy(item.STATE,m_qry->field("STATE").asString());
			strcpy(item.STATE_DATE,m_qry->field("STATE_DATE").asString());
			item.STAFF_ID=m_qry->field("STAFF_ID").asLong();
		return true;
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_BILL_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_BILL_CLASS::Get_BILL_ID(void)
{
		return	item.BILL_ID;
}

long	TBL_BILL_CLASS::Get_PAYMENT_ID(void)
{
		return	item.PAYMENT_ID;
}

long	TBL_BILL_CLASS::Get_PAYMENT_METHOD(void)
{
		return	item.PAYMENT_METHOD;
}

long	TBL_BILL_CLASS::Get_BILLING_CYCLE_ID(void)
{
		return	item.BILLING_CYCLE_ID;
}

long	TBL_BILL_CLASS::Get_ACCT_ID(void)
{
		return	item.ACCT_ID;
}

long	TBL_BILL_CLASS::Get_SERV_ID(void)
{
		return	item.SERV_ID;
}

char*	TBL_BILL_CLASS::Get_ACC_NBR(void)
{
		return	item.ACC_NBR;
}

long	TBL_BILL_CLASS::Get_BILL_AMOUNT(void)
{
		return	item.BILL_AMOUNT;
}

long	TBL_BILL_CLASS::Get_LATE_FEE(void)
{
		return	item.LATE_FEE;
}

long	TBL_BILL_CLASS::Get_DERATE_LATE_FEE(void)
{
		return	item.DERATE_LATE_FEE;
}

long	TBL_BILL_CLASS::Get_BALANCE(void)
{
		return	item.BALANCE;
}

long	TBL_BILL_CLASS::Get_LAST_CHANGE(void)
{
		return	item.LAST_CHANGE;
}

long	TBL_BILL_CLASS::Get_CUR_CHANGE(void)
{
		return	item.CUR_CHANGE;
}

char*	TBL_BILL_CLASS::Get_CREATED_DATE(void)
{
		return	item.CREATED_DATE;
}

char*	TBL_BILL_CLASS::Get_PAYMENT_DATE(void)
{
		return	item.PAYMENT_DATE;
}

long	TBL_BILL_CLASS::Get_USE_DERATE_BLANCE(void)
{
		return	item.USE_DERATE_BLANCE;
}

long	TBL_BILL_CLASS::Get_INVOICE_ID(void)
{
		return	item.INVOICE_ID;
}

char*	TBL_BILL_CLASS::Get_STATE(void)
{
		return	item.STATE;
}

char*	TBL_BILL_CLASS::Get_STATE_DATE(void)
{
		return	item.STATE_DATE;
}

long	TBL_BILL_CLASS::Get_STAFF_ID(void)
{
		return	item.STAFF_ID;
}

void	TBL_BILL_CLASS::Set_BILL_ID(long	lParam)
{
		item.BILL_ID=lParam;
}

void	TBL_BILL_CLASS::Set_PAYMENT_ID(long	lParam)
{
		item.PAYMENT_ID=lParam;
}

void	TBL_BILL_CLASS::Set_PAYMENT_METHOD(long	lParam)
{
		item.PAYMENT_METHOD=lParam;
}

void	TBL_BILL_CLASS::Set_BILLING_CYCLE_ID(long	lParam)
{
		item.BILLING_CYCLE_ID=lParam;
}

void	TBL_BILL_CLASS::Set_ACCT_ID(long	lParam)
{
		item.ACCT_ID=lParam;
}

void	TBL_BILL_CLASS::Set_SERV_ID(long	lParam)
{
		item.SERV_ID=lParam;
}

void	TBL_BILL_CLASS::Set_ACC_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_ACC_NBR  string over 32!");
		strcpy(item.ACC_NBR,sParam);
}

void	TBL_BILL_CLASS::Set_BILL_AMOUNT(long	lParam)
{
		item.BILL_AMOUNT=lParam;
}

void	TBL_BILL_CLASS::Set_LATE_FEE(long	lParam)
{
		item.LATE_FEE=lParam;
}

void	TBL_BILL_CLASS::Set_DERATE_LATE_FEE(long	lParam)
{
		item.DERATE_LATE_FEE=lParam;
}

void	TBL_BILL_CLASS::Set_BALANCE(long	lParam)
{
		item.BALANCE=lParam;
}

void	TBL_BILL_CLASS::Set_LAST_CHANGE(long	lParam)
{
		item.LAST_CHANGE=lParam;
}

void	TBL_BILL_CLASS::Set_CUR_CHANGE(long	lParam)
{
		item.CUR_CHANGE=lParam;
}

void	TBL_BILL_CLASS::Set_CREATED_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_CREATED_DATE  string over 14!");
		strcpy(item.CREATED_DATE,sParam);
}

void	TBL_BILL_CLASS::Set_PAYMENT_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_PAYMENT_DATE  string over 14!");
		strcpy(item.PAYMENT_DATE,sParam);
}

void	TBL_BILL_CLASS::Set_USE_DERATE_BLANCE(long	lParam)
{
		item.USE_DERATE_BLANCE=lParam;
}

void	TBL_BILL_CLASS::Set_INVOICE_ID(long	lParam)
{
		item.INVOICE_ID=lParam;
}

void	TBL_BILL_CLASS::Set_STATE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_STATE  string over 3!");
		strcpy(item.STATE,sParam);
}

void	TBL_BILL_CLASS::Set_STATE_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_STATE_DATE  string over 14!");
		strcpy(item.STATE_DATE,sParam);
}

void	TBL_BILL_CLASS::Set_STAFF_ID(long	lParam)
{
		item.STAFF_ID=lParam;
}

void	TBL_BILL_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_BILL_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_BILL_CLASS::SetBuf(int iBufLen)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_BILL_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_BILL_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_BILL_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="BILL_ID,PAYMENT_ID,PAYMENT_METHOD,BILLING_CYCLE_ID,ACCT_ID,SERV_ID,ACC_NBR,BILL_AMOUNT,LATE_FEE,DERATE_LATE_FEE,BALANCE,LAST_CHANGE,CUR_CHANGE,CREATED_DATE,PAYMENT_DATE,USE_DERATE_BLANCE,INVOICE_ID,STATE,STATE_DATE,STAFF_ID";
		ParamList="";
		ParamList+=":BILL_ID ";
		ParamList+=",";
		ParamList+=":PAYMENT_ID ";
		ParamList+=",";
		ParamList+=":PAYMENT_METHOD ";
		ParamList+=",";
		ParamList+=":BILLING_CYCLE_ID ";
		ParamList+=",";
		ParamList+=":ACCT_ID ";
		ParamList+=",";
		ParamList+=":SERV_ID ";
		ParamList+=",";
		ParamList+=":ACC_NBR ";
		ParamList+=",";
		ParamList+=":BILL_AMOUNT ";
		ParamList+=",";
		ParamList+=":LATE_FEE ";
		ParamList+=",";
		ParamList+=":DERATE_LATE_FEE ";
		ParamList+=",";
		ParamList+=":BALANCE ";
		ParamList+=",";
		ParamList+=":LAST_CHANGE ";
		ParamList+=",";
		ParamList+=":CUR_CHANGE ";
		ParamList+=",";
		if (a_item[0].CREATED_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:CREATED_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		if (a_item[0].PAYMENT_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:PAYMENT_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":USE_DERATE_BLANCE ";
		ParamList+=",";
		ParamList+=":INVOICE_ID ";
		ParamList+=",";
		ParamList+=":STATE ";
		ParamList+=",";
		if (a_item[0].STATE_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:STATE_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":STAFF_ID ";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("BILL_ID",&(a_item[0].BILL_ID),sizeof(a_item[0]));
			qry.setParamArray("PAYMENT_ID",&(a_item[0].PAYMENT_ID),sizeof(a_item[0]));
			qry.setParamArray("PAYMENT_METHOD",&(a_item[0].PAYMENT_METHOD),sizeof(a_item[0]));
			qry.setParamArray("BILLING_CYCLE_ID",&(a_item[0].BILLING_CYCLE_ID),sizeof(a_item[0]));
			qry.setParamArray("ACCT_ID",&(a_item[0].ACCT_ID),sizeof(a_item[0]));
			qry.setParamArray("SERV_ID",&(a_item[0].SERV_ID),sizeof(a_item[0]));
			qry.setParamArray("ACC_NBR",(char **)&(a_item[0].ACC_NBR),sizeof(a_item[0]),sizeof(a_item[0].ACC_NBR));
			qry.setParamArray("BILL_AMOUNT",&(a_item[0].BILL_AMOUNT),sizeof(a_item[0]));
			qry.setParamArray("LATE_FEE",&(a_item[0].LATE_FEE),sizeof(a_item[0]));
			qry.setParamArray("DERATE_LATE_FEE",&(a_item[0].DERATE_LATE_FEE),sizeof(a_item[0]));
			qry.setParamArray("BALANCE",&(a_item[0].BALANCE),sizeof(a_item[0]));
			qry.setParamArray("LAST_CHANGE",&(a_item[0].LAST_CHANGE),sizeof(a_item[0]));
			qry.setParamArray("CUR_CHANGE",&(a_item[0].CUR_CHANGE),sizeof(a_item[0]));
			if (a_item[0].CREATED_DATE[0]!='\0')
				qry.setParamArray("CREATED_DATE",(char **)&(a_item[0].CREATED_DATE),sizeof(a_item[0]),sizeof(a_item[0].CREATED_DATE));
			if (a_item[0].PAYMENT_DATE[0]!='\0')
				qry.setParamArray("PAYMENT_DATE",(char **)&(a_item[0].PAYMENT_DATE),sizeof(a_item[0]),sizeof(a_item[0].PAYMENT_DATE));
			qry.setParamArray("USE_DERATE_BLANCE",&(a_item[0].USE_DERATE_BLANCE),sizeof(a_item[0]));
			qry.setParamArray("INVOICE_ID",&(a_item[0].INVOICE_ID),sizeof(a_item[0]));
			qry.setParamArray("STATE",(char **)&(a_item[0].STATE),sizeof(a_item[0]),sizeof(a_item[0].STATE));
			if (a_item[0].STATE_DATE[0]!='\0')
				qry.setParamArray("STATE_DATE",(char **)&(a_item[0].STATE_DATE),sizeof(a_item[0]),sizeof(a_item[0].STATE_DATE));
			qry.setParamArray("STAFF_ID",&(a_item[0].STAFF_ID),sizeof(a_item[0]));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_BILL_STR_>	MEM_BILL_CLASS::a_item;
hash_map<const long ,_TBL_BILL_STR_* , hash<long>, MEM_BILL_CLASS::eqlong>	MEM_BILL_CLASS::hashmap;
MEM_BILL_CLASS::MEM_BILL_CLASS(char *sTableName)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_BILL_CLASS::~MEM_BILL_CLASS()
{
		/*UnLoad();*/
}

void	MEM_BILL_CLASS::Load(char *sTableName)
{
		if (a_item.size()>0)	return ;
		TBL_BILL_CLASS	 tempread;
		int		i;
		if (sTableName!=NULL)
				tempread.SetTableName(sTableName);
		tempread.Open();
		for(;tempread.Next();){
			a_item.push_back(tempread.item);
		}
		for(i=0;i<a_item.size();i++){
			hashmap[a_item[i].BILL_ID]=&a_item[i];
		}
}

void	MEM_BILL_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
}

_TBL_BILL_STR_ *	MEM_BILL_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


