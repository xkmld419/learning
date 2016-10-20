/*VER: 1*/ 
#include	"TBL_ACCT_ITEM_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_ACCT_ITEM_CLASS::TBL_ACCT_ITEM_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="ACCT_ITEM";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_ACCT_ITEM_CLASS::~TBL_ACCT_ITEM_CLASS()
{
		FreeBuf();
}

void	TBL_ACCT_ITEM_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_ACCT_ITEM_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_ACCT_ITEM_CLASS::Open()
{
		string strSql;
		strSql="SELECT NVL(ACCT_ITEM_ID,-1) ACCT_ITEM_ID,NVL(ITEM_SOURCE_ID,-1) ITEM_SOURCE_ID,NVL(BILL_ID,-1) BILL_ID,NVL(ACCT_ITEM_TYPE_ID,-1) ACCT_ITEM_TYPE_ID,NVL(BILLING_CYCLE_ID,-1) BILLING_CYCLE_ID,NVL(ACCT_ID,-1) ACCT_ID,NVL(SERV_ID,-1) SERV_ID,NVL(AMOUNT,-1) AMOUNT,NVL(TO_CHAR(CREATED_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') CREATED_DATE,NVL(FEE_CYCLE_ID,-1) FEE_CYCLE_ID,NVL(PAYMENT_METHOD,-1) PAYMENT_METHOD,NVL(STATE,-1) STATE,NVL(TO_CHAR(STATE_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') STATE_DATE,NVL(INV_OFFER,-1) INV_OFFER from ";
		if	(sTableName.size()==0)
			strSql+="ACCT_ITEM";
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
			cout<<"Error occured ... in TBL_ACCT_ITEM_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_ACCT_ITEM_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry 还没open!");
		try{
			if ( !m_qry->next())
				return false;
			item.ACCT_ITEM_ID=m_qry->field("ACCT_ITEM_ID").asFloat();
			item.ITEM_SOURCE_ID=m_qry->field("ITEM_SOURCE_ID").asFloat();
			item.BILL_ID=m_qry->field("BILL_ID").asFloat();
			item.ACCT_ITEM_TYPE_ID=m_qry->field("ACCT_ITEM_TYPE_ID").asFloat();
			item.BILLING_CYCLE_ID=m_qry->field("BILLING_CYCLE_ID").asFloat();
			item.ACCT_ID=m_qry->field("ACCT_ID").asFloat();
			item.SERV_ID=m_qry->field("SERV_ID").asFloat();
			item.AMOUNT=m_qry->field("AMOUNT").asFloat();
			strcpy(item.CREATED_DATE,m_qry->field("CREATED_DATE").asString());
			item.FEE_CYCLE_ID=m_qry->field("FEE_CYCLE_ID").asFloat();
			item.PAYMENT_METHOD=m_qry->field("PAYMENT_METHOD").asFloat();
			strcpy(item.STATE,m_qry->field("STATE").asString());
			strcpy(item.STATE_DATE,m_qry->field("STATE_DATE").asString());
			strcpy(item.INV_OFFER,m_qry->field("INV_OFFER").asString());
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_ACCT_ITEM_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_ACCT_ITEM_CLASS::Get_ACCT_ITEM_ID(void)
{
		return	item.ACCT_ITEM_ID;
}

long	TBL_ACCT_ITEM_CLASS::Get_ITEM_SOURCE_ID(void)
{
		return	item.ITEM_SOURCE_ID;
}

long	TBL_ACCT_ITEM_CLASS::Get_BILL_ID(void)
{
		return	item.BILL_ID;
}

long	TBL_ACCT_ITEM_CLASS::Get_ACCT_ITEM_TYPE_ID(void)
{
		return	item.ACCT_ITEM_TYPE_ID;
}

long	TBL_ACCT_ITEM_CLASS::Get_BILLING_CYCLE_ID(void)
{
		return	item.BILLING_CYCLE_ID;
}

long	TBL_ACCT_ITEM_CLASS::Get_ACCT_ID(void)
{
		return	item.ACCT_ID;
}

long	TBL_ACCT_ITEM_CLASS::Get_SERV_ID(void)
{
		return	item.SERV_ID;
}

long	TBL_ACCT_ITEM_CLASS::Get_AMOUNT(void)
{
		return	item.AMOUNT;
}

char*	TBL_ACCT_ITEM_CLASS::Get_CREATED_DATE(void)
{
		return	item.CREATED_DATE;
}

long	TBL_ACCT_ITEM_CLASS::Get_FEE_CYCLE_ID(void)
{
		return	item.FEE_CYCLE_ID;
}

long	TBL_ACCT_ITEM_CLASS::Get_PAYMENT_METHOD(void)
{
		return	item.PAYMENT_METHOD;
}

char*	TBL_ACCT_ITEM_CLASS::Get_STATE(void)
{
		return	item.STATE;
}

char*	TBL_ACCT_ITEM_CLASS::Get_STATE_DATE(void)
{
		return	item.STATE_DATE;
}

char*	TBL_ACCT_ITEM_CLASS::Get_INV_OFFER(void)
{
		return	item.INV_OFFER;
}

void	TBL_ACCT_ITEM_CLASS::Set_ACCT_ITEM_ID(long	lParam)
{
		item.ACCT_ITEM_ID=lParam;
}

void	TBL_ACCT_ITEM_CLASS::Set_ITEM_SOURCE_ID(long	lParam)
{
		item.ITEM_SOURCE_ID=lParam;
}

void	TBL_ACCT_ITEM_CLASS::Set_BILL_ID(long	lParam)
{
		item.BILL_ID=lParam;
}

void	TBL_ACCT_ITEM_CLASS::Set_ACCT_ITEM_TYPE_ID(long	lParam)
{
		item.ACCT_ITEM_TYPE_ID=lParam;
}

void	TBL_ACCT_ITEM_CLASS::Set_BILLING_CYCLE_ID(long	lParam)
{
		item.BILLING_CYCLE_ID=lParam;
}

void	TBL_ACCT_ITEM_CLASS::Set_ACCT_ID(long	lParam)
{
		item.ACCT_ID=lParam;
}

void	TBL_ACCT_ITEM_CLASS::Set_SERV_ID(long	lParam)
{
		item.SERV_ID=lParam;
}

void	TBL_ACCT_ITEM_CLASS::Set_AMOUNT(long	lParam)
{
		item.AMOUNT=lParam;
}

void	TBL_ACCT_ITEM_CLASS::Set_CREATED_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_CREATED_DATE 字符参数的长度超过14!");
		strcpy(item.CREATED_DATE,sParam);
}

void	TBL_ACCT_ITEM_CLASS::Set_FEE_CYCLE_ID(long	lParam)
{
		item.FEE_CYCLE_ID=lParam;
}

void	TBL_ACCT_ITEM_CLASS::Set_PAYMENT_METHOD(long	lParam)
{
		item.PAYMENT_METHOD=lParam;
}

void	TBL_ACCT_ITEM_CLASS::Set_STATE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_STATE 字符参数的长度超过3!");
		strcpy(item.STATE,sParam);
}

void	TBL_ACCT_ITEM_CLASS::Set_STATE_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_STATE_DATE 字符参数的长度超过14!");
		strcpy(item.STATE_DATE,sParam);
}

void	TBL_ACCT_ITEM_CLASS::Set_INV_OFFER(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_INV_OFFER 字符参数的长度超过3!");
		strcpy(item.INV_OFFER,sParam);
}

void	TBL_ACCT_ITEM_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_ACCT_ITEM_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_ACCT_ITEM_CLASS::SetBuf(int iBufLen=ARRAY_BUFLEN)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_ACCT_ITEM_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_ACCT_ITEM_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_ACCT_ITEM_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="ACCT_ITEM_ID,ITEM_SOURCE_ID,BILL_ID,ACCT_ITEM_TYPE_ID,BILLING_CYCLE_ID,ACCT_ID,SERV_ID,AMOUNT,CREATED_DATE,FEE_CYCLE_ID,PAYMENT_METHOD,STATE,STATE_DATE,INV_OFFER";
		ParamList="";
		ParamList+=":ACCT_ITEM_ID ";
		ParamList+=",";
		ParamList+=":ITEM_SOURCE_ID ";
		ParamList+=",";
		ParamList+=":BILL_ID ";
		ParamList+=",";
		ParamList+=":ACCT_ITEM_TYPE_ID ";
		ParamList+=",";
		ParamList+=":BILLING_CYCLE_ID ";
		ParamList+=",";
		ParamList+=":ACCT_ID ";
		ParamList+=",";
		ParamList+=":SERV_ID ";
		ParamList+=",";
		ParamList+=":AMOUNT ";
		ParamList+=",";
		if (a_item[0].CREATED_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:CREATED_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":FEE_CYCLE_ID ";
		ParamList+=",";
		ParamList+=":PAYMENT_METHOD ";
		ParamList+=",";
		ParamList+=":STATE ";
		ParamList+=",";
		if (a_item[0].STATE_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:STATE_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":INV_OFFER ";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("ACCT_ITEM_ID",&(a_item[0].ACCT_ITEM_ID),sizeof(a_item[0]));
			qry.setParamArray("ITEM_SOURCE_ID",&(a_item[0].ITEM_SOURCE_ID),sizeof(a_item[0]));
			qry.setParamArray("BILL_ID",&(a_item[0].BILL_ID),sizeof(a_item[0]));
			qry.setParamArray("ACCT_ITEM_TYPE_ID",&(a_item[0].ACCT_ITEM_TYPE_ID),sizeof(a_item[0]));
			qry.setParamArray("BILLING_CYCLE_ID",&(a_item[0].BILLING_CYCLE_ID),sizeof(a_item[0]));
			qry.setParamArray("ACCT_ID",&(a_item[0].ACCT_ID),sizeof(a_item[0]));
			qry.setParamArray("SERV_ID",&(a_item[0].SERV_ID),sizeof(a_item[0]));
			qry.setParamArray("AMOUNT",&(a_item[0].AMOUNT),sizeof(a_item[0]));
			if (a_item[0].CREATED_DATE[0]!='\0')
				qry.setParamArray("CREATED_DATE",(char **)&(a_item[0].CREATED_DATE),sizeof(a_item[0]),sizeof(a_item[0].CREATED_DATE));
			qry.setParamArray("FEE_CYCLE_ID",&(a_item[0].FEE_CYCLE_ID),sizeof(a_item[0]));
			qry.setParamArray("PAYMENT_METHOD",&(a_item[0].PAYMENT_METHOD),sizeof(a_item[0]));
			qry.setParamArray("STATE",(char **)&(a_item[0].STATE),sizeof(a_item[0]),sizeof(a_item[0].STATE));
			if (a_item[0].STATE_DATE[0]!='\0')
				qry.setParamArray("STATE_DATE",(char **)&(a_item[0].STATE_DATE),sizeof(a_item[0]),sizeof(a_item[0].STATE_DATE));
			qry.setParamArray("INV_OFFER",(char **)&(a_item[0].INV_OFFER),sizeof(a_item[0]),sizeof(a_item[0].INV_OFFER));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_ACCT_ITEM_STR_>	MEM_ACCT_ITEM_CLASS::a_item;
hash_map<const long ,_TBL_ACCT_ITEM_STR_* , hash<long>, MEM_ACCT_ITEM_CLASS::eqlong>	MEM_ACCT_ITEM_CLASS::hashmap;
MEM_ACCT_ITEM_CLASS::MEM_ACCT_ITEM_CLASS(char *sTableName=NULL)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_ACCT_ITEM_CLASS::~MEM_ACCT_ITEM_CLASS()
{
		/*UnLoad();*/
}

void	MEM_ACCT_ITEM_CLASS::Load(char *sTableName=NULL)
{
		if (a_item.size()>0)	return ;
		TBL_ACCT_ITEM_CLASS	 tempread;
		int		i;
		if (sTableName!=NULL)
				tempread.SetTableName(sTableName);
		tempread.Open();
		for(;tempread.Next();){
			a_item.push_back(tempread.item);
		}
		for(i=0;i<a_item.size();i++){
			hashmap[a_item[i].ACCT_ITEM_ID]=&a_item[i];
		}
}

void	MEM_ACCT_ITEM_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
}

_TBL_ACCT_ITEM_STR_ *	MEM_ACCT_ITEM_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


