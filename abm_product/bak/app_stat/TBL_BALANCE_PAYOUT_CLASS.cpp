/*VER: 1*/ 
#include	"TBL_BALANCE_PAYOUT_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_BALANCE_PAYOUT_CLASS::TBL_BALANCE_PAYOUT_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="BALANCE_PAYOUT";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_BALANCE_PAYOUT_CLASS::~TBL_BALANCE_PAYOUT_CLASS()
{
		FreeBuf();
}

void	TBL_BALANCE_PAYOUT_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_BALANCE_PAYOUT_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_BALANCE_PAYOUT_CLASS::Open()
{
		string strSql;
		strSql="SELECT OPER_PAYOUT_ID,ACCT_BALANCE_ID,NVL(BILLING_CYCLE_ID,-1) BILLING_CYCLE_ID,NVL(BILL_ID,-1) BILL_ID,OPER_TYPE,STAFF_ID,TO_CHAR(OPER_DATE,'YYYYMMDDHH24MISS') OPER_DATE,AMOUNT,BALANCE,NVL(PRN_COUNT,-1) PRN_COUNT,STATE,TO_CHAR(STATE_DATE,'YYYYMMDDHH24MISS') STATE_DATE from ";
		if	(sTableName.size()==0)
			strSql+="BALANCE_PAYOUT";
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
			cout<<"Error occured ... in TBL_BALANCE_PAYOUT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_BALANCE_PAYOUT_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry not open!");
		try{
			if ( !m_qry->next())
				return false;
			item.OPER_PAYOUT_ID=m_qry->field("OPER_PAYOUT_ID").asLong();
			item.ACCT_BALANCE_ID=m_qry->field("ACCT_BALANCE_ID").asLong();
			item.BILLING_CYCLE_ID=m_qry->field("BILLING_CYCLE_ID").asLong();
			item.BILL_ID=m_qry->field("BILL_ID").asLong();
			strcpy(item.OPER_TYPE,m_qry->field("OPER_TYPE").asString());
			item.STAFF_ID=m_qry->field("STAFF_ID").asLong();
			strcpy(item.OPER_DATE,m_qry->field("OPER_DATE").asString());
			item.AMOUNT=m_qry->field("AMOUNT").asLong();
			item.BALANCE=m_qry->field("BALANCE").asLong();
			item.PRN_COUNT=m_qry->field("PRN_COUNT").asLong();
			strcpy(item.STATE,m_qry->field("STATE").asString());
			strcpy(item.STATE_DATE,m_qry->field("STATE_DATE").asString());
		return true;
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_BALANCE_PAYOUT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_BALANCE_PAYOUT_CLASS::Get_OPER_PAYOUT_ID(void)
{
		return	item.OPER_PAYOUT_ID;
}

long	TBL_BALANCE_PAYOUT_CLASS::Get_ACCT_BALANCE_ID(void)
{
		return	item.ACCT_BALANCE_ID;
}

long	TBL_BALANCE_PAYOUT_CLASS::Get_BILLING_CYCLE_ID(void)
{
		return	item.BILLING_CYCLE_ID;
}

long	TBL_BALANCE_PAYOUT_CLASS::Get_BILL_ID(void)
{
		return	item.BILL_ID;
}

char*	TBL_BALANCE_PAYOUT_CLASS::Get_OPER_TYPE(void)
{
		return	item.OPER_TYPE;
}

long	TBL_BALANCE_PAYOUT_CLASS::Get_STAFF_ID(void)
{
		return	item.STAFF_ID;
}

char*	TBL_BALANCE_PAYOUT_CLASS::Get_OPER_DATE(void)
{
		return	item.OPER_DATE;
}

long	TBL_BALANCE_PAYOUT_CLASS::Get_AMOUNT(void)
{
		return	item.AMOUNT;
}

long	TBL_BALANCE_PAYOUT_CLASS::Get_BALANCE(void)
{
		return	item.BALANCE;
}

long	TBL_BALANCE_PAYOUT_CLASS::Get_PRN_COUNT(void)
{
		return	item.PRN_COUNT;
}

char*	TBL_BALANCE_PAYOUT_CLASS::Get_STATE(void)
{
		return	item.STATE;
}

char*	TBL_BALANCE_PAYOUT_CLASS::Get_STATE_DATE(void)
{
		return	item.STATE_DATE;
}

void	TBL_BALANCE_PAYOUT_CLASS::Set_OPER_PAYOUT_ID(long	lParam)
{
		item.OPER_PAYOUT_ID=lParam;
}

void	TBL_BALANCE_PAYOUT_CLASS::Set_ACCT_BALANCE_ID(long	lParam)
{
		item.ACCT_BALANCE_ID=lParam;
}

void	TBL_BALANCE_PAYOUT_CLASS::Set_BILLING_CYCLE_ID(long	lParam)
{
		item.BILLING_CYCLE_ID=lParam;
}

void	TBL_BALANCE_PAYOUT_CLASS::Set_BILL_ID(long	lParam)
{
		item.BILL_ID=lParam;
}

void	TBL_BALANCE_PAYOUT_CLASS::Set_OPER_TYPE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_OPER_TYPE  string over 3!");
		strcpy(item.OPER_TYPE,sParam);
}

void	TBL_BALANCE_PAYOUT_CLASS::Set_STAFF_ID(long	lParam)
{
		item.STAFF_ID=lParam;
}

void	TBL_BALANCE_PAYOUT_CLASS::Set_OPER_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_OPER_DATE  string over 14!");
		strcpy(item.OPER_DATE,sParam);
}

void	TBL_BALANCE_PAYOUT_CLASS::Set_AMOUNT(long	lParam)
{
		item.AMOUNT=lParam;
}

void	TBL_BALANCE_PAYOUT_CLASS::Set_BALANCE(long	lParam)
{
		item.BALANCE=lParam;
}

void	TBL_BALANCE_PAYOUT_CLASS::Set_PRN_COUNT(long	lParam)
{
		item.PRN_COUNT=lParam;
}

void	TBL_BALANCE_PAYOUT_CLASS::Set_STATE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_STATE  string over 3!");
		strcpy(item.STATE,sParam);
}

void	TBL_BALANCE_PAYOUT_CLASS::Set_STATE_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_STATE_DATE  string over 14!");
		strcpy(item.STATE_DATE,sParam);
}

void	TBL_BALANCE_PAYOUT_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_BALANCE_PAYOUT_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_BALANCE_PAYOUT_CLASS::SetBuf(int iBufLen)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_BALANCE_PAYOUT_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_BALANCE_PAYOUT_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_BALANCE_PAYOUT_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="OPER_PAYOUT_ID,ACCT_BALANCE_ID,BILLING_CYCLE_ID,BILL_ID,OPER_TYPE,STAFF_ID,OPER_DATE,AMOUNT,BALANCE,PRN_COUNT,STATE,STATE_DATE";
		ParamList="";
		ParamList+=":OPER_PAYOUT_ID ";
		ParamList+=",";
		ParamList+=":ACCT_BALANCE_ID ";
		ParamList+=",";
		ParamList+=":BILLING_CYCLE_ID ";
		ParamList+=",";
		ParamList+=":BILL_ID ";
		ParamList+=",";
		ParamList+=":OPER_TYPE ";
		ParamList+=",";
		ParamList+=":STAFF_ID ";
		ParamList+=",";
		if (a_item[0].OPER_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:OPER_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":AMOUNT ";
		ParamList+=",";
		ParamList+=":BALANCE ";
		ParamList+=",";
		ParamList+=":PRN_COUNT ";
		ParamList+=",";
		ParamList+=":STATE ";
		ParamList+=",";
		if (a_item[0].STATE_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:STATE_DATE,'YYYYMMDDHH24MISS')";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("OPER_PAYOUT_ID",&(a_item[0].OPER_PAYOUT_ID),sizeof(a_item[0]));
			qry.setParamArray("ACCT_BALANCE_ID",&(a_item[0].ACCT_BALANCE_ID),sizeof(a_item[0]));
			qry.setParamArray("BILLING_CYCLE_ID",&(a_item[0].BILLING_CYCLE_ID),sizeof(a_item[0]));
			qry.setParamArray("BILL_ID",&(a_item[0].BILL_ID),sizeof(a_item[0]));
			qry.setParamArray("OPER_TYPE",(char **)&(a_item[0].OPER_TYPE),sizeof(a_item[0]),sizeof(a_item[0].OPER_TYPE));
			qry.setParamArray("STAFF_ID",&(a_item[0].STAFF_ID),sizeof(a_item[0]));
			if (a_item[0].OPER_DATE[0]!='\0')
				qry.setParamArray("OPER_DATE",(char **)&(a_item[0].OPER_DATE),sizeof(a_item[0]),sizeof(a_item[0].OPER_DATE));
			qry.setParamArray("AMOUNT",&(a_item[0].AMOUNT),sizeof(a_item[0]));
			qry.setParamArray("BALANCE",&(a_item[0].BALANCE),sizeof(a_item[0]));
			qry.setParamArray("PRN_COUNT",&(a_item[0].PRN_COUNT),sizeof(a_item[0]));
			qry.setParamArray("STATE",(char **)&(a_item[0].STATE),sizeof(a_item[0]),sizeof(a_item[0].STATE));
			if (a_item[0].STATE_DATE[0]!='\0')
				qry.setParamArray("STATE_DATE",(char **)&(a_item[0].STATE_DATE),sizeof(a_item[0]),sizeof(a_item[0].STATE_DATE));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_BALANCE_PAYOUT_STR_>	MEM_BALANCE_PAYOUT_CLASS::a_item;
hash_map<const long ,_TBL_BALANCE_PAYOUT_STR_* , hash<long>, MEM_BALANCE_PAYOUT_CLASS::eqlong>	MEM_BALANCE_PAYOUT_CLASS::hashmap;
MEM_BALANCE_PAYOUT_CLASS::MEM_BALANCE_PAYOUT_CLASS(char *sTableName)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_BALANCE_PAYOUT_CLASS::~MEM_BALANCE_PAYOUT_CLASS()
{
		/*UnLoad();*/
}

void	MEM_BALANCE_PAYOUT_CLASS::Load(char *sTableName)
{
		if (a_item.size()>0)	return ;
		TBL_BALANCE_PAYOUT_CLASS	 tempread;
		int		i;
		if (sTableName!=NULL)
				tempread.SetTableName(sTableName);
		tempread.Open();
		for(;tempread.Next();){
			a_item.push_back(tempread.item);
		}
		for(i=0;i<a_item.size();i++){
			hashmap[a_item[i].OPER_PAYOUT_ID]=&a_item[i];
		}
}

void	MEM_BALANCE_PAYOUT_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
}

_TBL_BALANCE_PAYOUT_STR_ *	MEM_BALANCE_PAYOUT_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


