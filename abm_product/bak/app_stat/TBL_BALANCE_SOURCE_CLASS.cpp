/*VER: 1*/ 
#include	"TBL_BALANCE_SOURCE_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_BALANCE_SOURCE_CLASS::TBL_BALANCE_SOURCE_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="BALANCE_SOURCE";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_BALANCE_SOURCE_CLASS::~TBL_BALANCE_SOURCE_CLASS()
{
		FreeBuf();
}

void	TBL_BALANCE_SOURCE_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_BALANCE_SOURCE_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_BALANCE_SOURCE_CLASS::Open()
{
		string strSql;
		strSql="SELECT OPER_INCOME_ID,ACCT_BALANCE_ID,OPER_TYPE,STAFF_ID,TO_CHAR(OPER_DATE,'YYYYMMDDHH24MISS') OPER_DATE,AMOUNT,SOURCE_TYPE,SOURCE_DESC,STATE,TO_CHAR(STATE_DATE,'YYYYMMDDHH24MISS') STATE_DATE,NVL(ACCT_BALANCE_OBJ_ID,-1) ACCT_BALANCE_OBJ_ID,NVL(INV_OFFER,-1) INV_OFFER,NVL(ALLOW_DRAW,-1) ALLOW_DRAW,NVL(CURR_STATE,-1) CURR_STATE,NVL(TO_CHAR(CURR_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') CURR_DATE,PAYMENT_ID,NVL(BALANCE,-1) BALANCE from ";
		if	(sTableName.size()==0)
			strSql+="BALANCE_SOURCE";
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
			cout<<"Error occured ... in TBL_BALANCE_SOURCE_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_BALANCE_SOURCE_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry not open!");
		try{
			if ( !m_qry->next())
				return false;
			item.OPER_INCOME_ID=m_qry->field("OPER_INCOME_ID").asLong();
			item.ACCT_BALANCE_ID=m_qry->field("ACCT_BALANCE_ID").asLong();
			strcpy(item.OPER_TYPE,m_qry->field("OPER_TYPE").asString());
			item.STAFF_ID=m_qry->field("STAFF_ID").asLong();
			strcpy(item.OPER_DATE,m_qry->field("OPER_DATE").asString());
			item.AMOUNT=m_qry->field("AMOUNT").asLong();
			strcpy(item.SOURCE_TYPE,m_qry->field("SOURCE_TYPE").asString());
			strcpy(item.SOURCE_DESC,m_qry->field("SOURCE_DESC").asString());
			strcpy(item.STATE,m_qry->field("STATE").asString());
			strcpy(item.STATE_DATE,m_qry->field("STATE_DATE").asString());
			item.ACCT_BALANCE_OBJ_ID=m_qry->field("ACCT_BALANCE_OBJ_ID").asLong();
			strcpy(item.INV_OFFER,m_qry->field("INV_OFFER").asString());
			strcpy(item.ALLOW_DRAW,m_qry->field("ALLOW_DRAW").asString());
			strcpy(item.CURR_STATE,m_qry->field("CURR_STATE").asString());
			strcpy(item.CURR_DATE,m_qry->field("CURR_DATE").asString());
			item.PAYMENT_ID=m_qry->field("PAYMENT_ID").asLong();
			item.BALANCE=m_qry->field("BALANCE").asLong();
		return true;
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_BALANCE_SOURCE_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_BALANCE_SOURCE_CLASS::Get_OPER_INCOME_ID(void)
{
		return	item.OPER_INCOME_ID;
}

long	TBL_BALANCE_SOURCE_CLASS::Get_ACCT_BALANCE_ID(void)
{
		return	item.ACCT_BALANCE_ID;
}

char*	TBL_BALANCE_SOURCE_CLASS::Get_OPER_TYPE(void)
{
		return	item.OPER_TYPE;
}

long	TBL_BALANCE_SOURCE_CLASS::Get_STAFF_ID(void)
{
		return	item.STAFF_ID;
}

char*	TBL_BALANCE_SOURCE_CLASS::Get_OPER_DATE(void)
{
		return	item.OPER_DATE;
}

long	TBL_BALANCE_SOURCE_CLASS::Get_AMOUNT(void)
{
		return	item.AMOUNT;
}

char*	TBL_BALANCE_SOURCE_CLASS::Get_SOURCE_TYPE(void)
{
		return	item.SOURCE_TYPE;
}

char*	TBL_BALANCE_SOURCE_CLASS::Get_SOURCE_DESC(void)
{
		return	item.SOURCE_DESC;
}

char*	TBL_BALANCE_SOURCE_CLASS::Get_STATE(void)
{
		return	item.STATE;
}

char*	TBL_BALANCE_SOURCE_CLASS::Get_STATE_DATE(void)
{
		return	item.STATE_DATE;
}

long	TBL_BALANCE_SOURCE_CLASS::Get_ACCT_BALANCE_OBJ_ID(void)
{
		return	item.ACCT_BALANCE_OBJ_ID;
}

char*	TBL_BALANCE_SOURCE_CLASS::Get_INV_OFFER(void)
{
		return	item.INV_OFFER;
}

char*	TBL_BALANCE_SOURCE_CLASS::Get_ALLOW_DRAW(void)
{
		return	item.ALLOW_DRAW;
}

char*	TBL_BALANCE_SOURCE_CLASS::Get_CURR_STATE(void)
{
		return	item.CURR_STATE;
}

char*	TBL_BALANCE_SOURCE_CLASS::Get_CURR_DATE(void)
{
		return	item.CURR_DATE;
}

long	TBL_BALANCE_SOURCE_CLASS::Get_PAYMENT_ID(void)
{
		return	item.PAYMENT_ID;
}

long	TBL_BALANCE_SOURCE_CLASS::Get_BALANCE(void)
{
		return	item.BALANCE;
}

void	TBL_BALANCE_SOURCE_CLASS::Set_OPER_INCOME_ID(long	lParam)
{
		item.OPER_INCOME_ID=lParam;
}

void	TBL_BALANCE_SOURCE_CLASS::Set_ACCT_BALANCE_ID(long	lParam)
{
		item.ACCT_BALANCE_ID=lParam;
}

void	TBL_BALANCE_SOURCE_CLASS::Set_OPER_TYPE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_OPER_TYPE  string over 3!");
		strcpy(item.OPER_TYPE,sParam);
}

void	TBL_BALANCE_SOURCE_CLASS::Set_STAFF_ID(long	lParam)
{
		item.STAFF_ID=lParam;
}

void	TBL_BALANCE_SOURCE_CLASS::Set_OPER_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_OPER_DATE  string over 14!");
		strcpy(item.OPER_DATE,sParam);
}

void	TBL_BALANCE_SOURCE_CLASS::Set_AMOUNT(long	lParam)
{
		item.AMOUNT=lParam;
}

void	TBL_BALANCE_SOURCE_CLASS::Set_SOURCE_TYPE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_SOURCE_TYPE  string over 3!");
		strcpy(item.SOURCE_TYPE,sParam);
}

void	TBL_BALANCE_SOURCE_CLASS::Set_SOURCE_DESC(char	* sParam)
{
		if ( strlen(sParam)>250 ) 
			 throw TException( " Set_SOURCE_DESC  string over 250!");
		strcpy(item.SOURCE_DESC,sParam);
}

void	TBL_BALANCE_SOURCE_CLASS::Set_STATE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_STATE  string over 3!");
		strcpy(item.STATE,sParam);
}

void	TBL_BALANCE_SOURCE_CLASS::Set_STATE_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_STATE_DATE  string over 14!");
		strcpy(item.STATE_DATE,sParam);
}

void	TBL_BALANCE_SOURCE_CLASS::Set_ACCT_BALANCE_OBJ_ID(long	lParam)
{
		item.ACCT_BALANCE_OBJ_ID=lParam;
}

void	TBL_BALANCE_SOURCE_CLASS::Set_INV_OFFER(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_INV_OFFER  string over 3!");
		strcpy(item.INV_OFFER,sParam);
}

void	TBL_BALANCE_SOURCE_CLASS::Set_ALLOW_DRAW(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_ALLOW_DRAW  string over 3!");
		strcpy(item.ALLOW_DRAW,sParam);
}

void	TBL_BALANCE_SOURCE_CLASS::Set_CURR_STATE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_CURR_STATE  string over 3!");
		strcpy(item.CURR_STATE,sParam);
}

void	TBL_BALANCE_SOURCE_CLASS::Set_CURR_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_CURR_DATE  string over 14!");
		strcpy(item.CURR_DATE,sParam);
}

void	TBL_BALANCE_SOURCE_CLASS::Set_PAYMENT_ID(long	lParam)
{
		item.PAYMENT_ID=lParam;
}

void	TBL_BALANCE_SOURCE_CLASS::Set_BALANCE(long	lParam)
{
		item.BALANCE=lParam;
}

void	TBL_BALANCE_SOURCE_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_BALANCE_SOURCE_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_BALANCE_SOURCE_CLASS::SetBuf(int iBufLen)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_BALANCE_SOURCE_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_BALANCE_SOURCE_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_BALANCE_SOURCE_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="OPER_INCOME_ID,ACCT_BALANCE_ID,OPER_TYPE,STAFF_ID,OPER_DATE,AMOUNT,SOURCE_TYPE,SOURCE_DESC,STATE,STATE_DATE,ACCT_BALANCE_OBJ_ID,INV_OFFER,ALLOW_DRAW,CURR_STATE,CURR_DATE,PAYMENT_ID,BALANCE";
		ParamList="";
		ParamList+=":OPER_INCOME_ID ";
		ParamList+=",";
		ParamList+=":ACCT_BALANCE_ID ";
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
		ParamList+=":SOURCE_TYPE ";
		ParamList+=",";
		ParamList+=":SOURCE_DESC ";
		ParamList+=",";
		ParamList+=":STATE ";
		ParamList+=",";
		if (a_item[0].STATE_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:STATE_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":ACCT_BALANCE_OBJ_ID ";
		ParamList+=",";
		ParamList+=":INV_OFFER ";
		ParamList+=",";
		ParamList+=":ALLOW_DRAW ";
		ParamList+=",";
		ParamList+=":CURR_STATE ";
		ParamList+=",";
		if (a_item[0].CURR_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:CURR_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":PAYMENT_ID ";
		ParamList+=",";
		ParamList+=":BALANCE ";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("OPER_INCOME_ID",&(a_item[0].OPER_INCOME_ID),sizeof(a_item[0]));
			qry.setParamArray("ACCT_BALANCE_ID",&(a_item[0].ACCT_BALANCE_ID),sizeof(a_item[0]));
			qry.setParamArray("OPER_TYPE",(char **)&(a_item[0].OPER_TYPE),sizeof(a_item[0]),sizeof(a_item[0].OPER_TYPE));
			qry.setParamArray("STAFF_ID",&(a_item[0].STAFF_ID),sizeof(a_item[0]));
			if (a_item[0].OPER_DATE[0]!='\0')
				qry.setParamArray("OPER_DATE",(char **)&(a_item[0].OPER_DATE),sizeof(a_item[0]),sizeof(a_item[0].OPER_DATE));
			qry.setParamArray("AMOUNT",&(a_item[0].AMOUNT),sizeof(a_item[0]));
			qry.setParamArray("SOURCE_TYPE",(char **)&(a_item[0].SOURCE_TYPE),sizeof(a_item[0]),sizeof(a_item[0].SOURCE_TYPE));
			qry.setParamArray("SOURCE_DESC",(char **)&(a_item[0].SOURCE_DESC),sizeof(a_item[0]),sizeof(a_item[0].SOURCE_DESC));
			qry.setParamArray("STATE",(char **)&(a_item[0].STATE),sizeof(a_item[0]),sizeof(a_item[0].STATE));
			if (a_item[0].STATE_DATE[0]!='\0')
				qry.setParamArray("STATE_DATE",(char **)&(a_item[0].STATE_DATE),sizeof(a_item[0]),sizeof(a_item[0].STATE_DATE));
			qry.setParamArray("ACCT_BALANCE_OBJ_ID",&(a_item[0].ACCT_BALANCE_OBJ_ID),sizeof(a_item[0]));
			qry.setParamArray("INV_OFFER",(char **)&(a_item[0].INV_OFFER),sizeof(a_item[0]),sizeof(a_item[0].INV_OFFER));
			qry.setParamArray("ALLOW_DRAW",(char **)&(a_item[0].ALLOW_DRAW),sizeof(a_item[0]),sizeof(a_item[0].ALLOW_DRAW));
			qry.setParamArray("CURR_STATE",(char **)&(a_item[0].CURR_STATE),sizeof(a_item[0]),sizeof(a_item[0].CURR_STATE));
			if (a_item[0].CURR_DATE[0]!='\0')
				qry.setParamArray("CURR_DATE",(char **)&(a_item[0].CURR_DATE),sizeof(a_item[0]),sizeof(a_item[0].CURR_DATE));
			qry.setParamArray("PAYMENT_ID",&(a_item[0].PAYMENT_ID),sizeof(a_item[0]));
			qry.setParamArray("BALANCE",&(a_item[0].BALANCE),sizeof(a_item[0]));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_BALANCE_SOURCE_STR_>	MEM_BALANCE_SOURCE_CLASS::a_item;
hash_map<const long ,_TBL_BALANCE_SOURCE_STR_* , hash<long>, MEM_BALANCE_SOURCE_CLASS::eqlong>	MEM_BALANCE_SOURCE_CLASS::hashmap;
MEM_BALANCE_SOURCE_CLASS::MEM_BALANCE_SOURCE_CLASS(char *sTableName)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_BALANCE_SOURCE_CLASS::~MEM_BALANCE_SOURCE_CLASS()
{
		/*UnLoad();*/
}

void	MEM_BALANCE_SOURCE_CLASS::Load(char *sTableName)
{
		if (a_item.size()>0)	return ;
		TBL_BALANCE_SOURCE_CLASS	 tempread;
		int		i;
		if (sTableName!=NULL)
				tempread.SetTableName(sTableName);
		tempread.Open();
		for(;tempread.Next();){
			a_item.push_back(tempread.item);
		}
		for(i=0;i<a_item.size();i++){
			hashmap[a_item[i].OPER_INCOME_ID]=&a_item[i];
		}
}

void	MEM_BALANCE_SOURCE_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
}

_TBL_BALANCE_SOURCE_STR_ *	MEM_BALANCE_SOURCE_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


