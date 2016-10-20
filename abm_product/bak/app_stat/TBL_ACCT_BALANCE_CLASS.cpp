/*VER: 1*/ 
#include	"TBL_ACCT_BALANCE_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_ACCT_BALANCE_CLASS::TBL_ACCT_BALANCE_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="ACCT_BALANCE";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_ACCT_BALANCE_CLASS::~TBL_ACCT_BALANCE_CLASS()
{
		FreeBuf();
}

void	TBL_ACCT_BALANCE_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_ACCT_BALANCE_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_ACCT_BALANCE_CLASS::Open()
{
		string strSql;
		strSql="SELECT ACCT_BALANCE_ID,BALANCE_TYPE_ID,NVL(TO_CHAR(EFF_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') EFF_DATE,NVL(TO_CHAR(EXP_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') EXP_DATE,BALANCE,NVL(CYCLE_UPPER,-1) CYCLE_UPPER,NVL(CYCLE_LOWER,-1) CYCLE_LOWER,NVL(CYCLE_UPPER_TYPE,-1) CYCLE_UPPER_TYPE,NVL(CYCLE_LOWER_TYPE,-1) CYCLE_LOWER_TYPE,NVL(BANK_ACCT,-1) BANK_ACCT,STATE,TO_CHAR(STATE_DATE,'YYYYMMDDHH24MISS') STATE_DATE,ACCT_ID,NVL(SERV_ID,-1) SERV_ID,NVL(ITEM_GROUP_ID,-1) ITEM_GROUP_ID from ";
		if	(sTableName.size()==0)
			strSql+="ACCT_BALANCE";
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
			cout<<"Error occured ... in TBL_ACCT_BALANCE_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_ACCT_BALANCE_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry not open!");
		try{
			if ( !m_qry->next())
				return false;
			item.ACCT_BALANCE_ID=m_qry->field("ACCT_BALANCE_ID").asLong();
			item.BALANCE_TYPE_ID=m_qry->field("BALANCE_TYPE_ID").asLong();
			strcpy(item.EFF_DATE,m_qry->field("EFF_DATE").asString());
			strcpy(item.EXP_DATE,m_qry->field("EXP_DATE").asString());
			item.BALANCE=m_qry->field("BALANCE").asLong();
			item.CYCLE_UPPER=m_qry->field("CYCLE_UPPER").asLong();
			item.CYCLE_LOWER=m_qry->field("CYCLE_LOWER").asLong();
			strcpy(item.CYCLE_UPPER_TYPE,m_qry->field("CYCLE_UPPER_TYPE").asString());
			strcpy(item.CYCLE_LOWER_TYPE,m_qry->field("CYCLE_LOWER_TYPE").asString());
			strcpy(item.BANK_ACCT,m_qry->field("BANK_ACCT").asString());
			strcpy(item.STATE,m_qry->field("STATE").asString());
			strcpy(item.STATE_DATE,m_qry->field("STATE_DATE").asString());
			item.ACCT_ID=m_qry->field("ACCT_ID").asLong();
			item.SERV_ID=m_qry->field("SERV_ID").asLong();
			item.ITEM_GROUP_ID=m_qry->field("ITEM_GROUP_ID").asLong();
		return true;
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_ACCT_BALANCE_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_ACCT_BALANCE_CLASS::Get_ACCT_BALANCE_ID(void)
{
		return	item.ACCT_BALANCE_ID;
}

long	TBL_ACCT_BALANCE_CLASS::Get_BALANCE_TYPE_ID(void)
{
		return	item.BALANCE_TYPE_ID;
}

char*	TBL_ACCT_BALANCE_CLASS::Get_EFF_DATE(void)
{
		return	item.EFF_DATE;
}

char*	TBL_ACCT_BALANCE_CLASS::Get_EXP_DATE(void)
{
		return	item.EXP_DATE;
}

long	TBL_ACCT_BALANCE_CLASS::Get_BALANCE(void)
{
		return	item.BALANCE;
}

long	TBL_ACCT_BALANCE_CLASS::Get_CYCLE_UPPER(void)
{
		return	item.CYCLE_UPPER;
}

long	TBL_ACCT_BALANCE_CLASS::Get_CYCLE_LOWER(void)
{
		return	item.CYCLE_LOWER;
}

char*	TBL_ACCT_BALANCE_CLASS::Get_CYCLE_UPPER_TYPE(void)
{
		return	item.CYCLE_UPPER_TYPE;
}

char*	TBL_ACCT_BALANCE_CLASS::Get_CYCLE_LOWER_TYPE(void)
{
		return	item.CYCLE_LOWER_TYPE;
}

char*	TBL_ACCT_BALANCE_CLASS::Get_BANK_ACCT(void)
{
		return	item.BANK_ACCT;
}

char*	TBL_ACCT_BALANCE_CLASS::Get_STATE(void)
{
		return	item.STATE;
}

char*	TBL_ACCT_BALANCE_CLASS::Get_STATE_DATE(void)
{
		return	item.STATE_DATE;
}

long	TBL_ACCT_BALANCE_CLASS::Get_ACCT_ID(void)
{
		return	item.ACCT_ID;
}

long	TBL_ACCT_BALANCE_CLASS::Get_SERV_ID(void)
{
		return	item.SERV_ID;
}

long	TBL_ACCT_BALANCE_CLASS::Get_ITEM_GROUP_ID(void)
{
		return	item.ITEM_GROUP_ID;
}

void	TBL_ACCT_BALANCE_CLASS::Set_ACCT_BALANCE_ID(long	lParam)
{
		item.ACCT_BALANCE_ID=lParam;
}

void	TBL_ACCT_BALANCE_CLASS::Set_BALANCE_TYPE_ID(long	lParam)
{
		item.BALANCE_TYPE_ID=lParam;
}

void	TBL_ACCT_BALANCE_CLASS::Set_EFF_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_EFF_DATE  string over 14!");
		strcpy(item.EFF_DATE,sParam);
}

void	TBL_ACCT_BALANCE_CLASS::Set_EXP_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_EXP_DATE  string over 14!");
		strcpy(item.EXP_DATE,sParam);
}

void	TBL_ACCT_BALANCE_CLASS::Set_BALANCE(long	lParam)
{
		item.BALANCE=lParam;
}

void	TBL_ACCT_BALANCE_CLASS::Set_CYCLE_UPPER(long	lParam)
{
		item.CYCLE_UPPER=lParam;
}

void	TBL_ACCT_BALANCE_CLASS::Set_CYCLE_LOWER(long	lParam)
{
		item.CYCLE_LOWER=lParam;
}

void	TBL_ACCT_BALANCE_CLASS::Set_CYCLE_UPPER_TYPE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_CYCLE_UPPER_TYPE  string over 3!");
		strcpy(item.CYCLE_UPPER_TYPE,sParam);
}

void	TBL_ACCT_BALANCE_CLASS::Set_CYCLE_LOWER_TYPE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_CYCLE_LOWER_TYPE  string over 3!");
		strcpy(item.CYCLE_LOWER_TYPE,sParam);
}

void	TBL_ACCT_BALANCE_CLASS::Set_BANK_ACCT(char	* sParam)
{
		if ( strlen(sParam)>30 ) 
			 throw TException( " Set_BANK_ACCT  string over 30!");
		strcpy(item.BANK_ACCT,sParam);
}

void	TBL_ACCT_BALANCE_CLASS::Set_STATE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_STATE  string over 3!");
		strcpy(item.STATE,sParam);
}

void	TBL_ACCT_BALANCE_CLASS::Set_STATE_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_STATE_DATE  string over 14!");
		strcpy(item.STATE_DATE,sParam);
}

void	TBL_ACCT_BALANCE_CLASS::Set_ACCT_ID(long	lParam)
{
		item.ACCT_ID=lParam;
}

void	TBL_ACCT_BALANCE_CLASS::Set_SERV_ID(long	lParam)
{
		item.SERV_ID=lParam;
}

void	TBL_ACCT_BALANCE_CLASS::Set_ITEM_GROUP_ID(long	lParam)
{
		item.ITEM_GROUP_ID=lParam;
}

void	TBL_ACCT_BALANCE_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_ACCT_BALANCE_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_ACCT_BALANCE_CLASS::SetBuf(int iBufLen)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_ACCT_BALANCE_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_ACCT_BALANCE_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_ACCT_BALANCE_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="ACCT_BALANCE_ID,BALANCE_TYPE_ID,EFF_DATE,EXP_DATE,BALANCE,CYCLE_UPPER,CYCLE_LOWER,CYCLE_UPPER_TYPE,CYCLE_LOWER_TYPE,BANK_ACCT,STATE,STATE_DATE,ACCT_ID,SERV_ID,ITEM_GROUP_ID";
		ParamList="";
		ParamList+=":ACCT_BALANCE_ID ";
		ParamList+=",";
		ParamList+=":BALANCE_TYPE_ID ";
		ParamList+=",";
		if (a_item[0].EFF_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:EFF_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		if (a_item[0].EXP_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:EXP_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":BALANCE ";
		ParamList+=",";
		ParamList+=":CYCLE_UPPER ";
		ParamList+=",";
		ParamList+=":CYCLE_LOWER ";
		ParamList+=",";
		ParamList+=":CYCLE_UPPER_TYPE ";
		ParamList+=",";
		ParamList+=":CYCLE_LOWER_TYPE ";
		ParamList+=",";
		ParamList+=":BANK_ACCT ";
		ParamList+=",";
		ParamList+=":STATE ";
		ParamList+=",";
		if (a_item[0].STATE_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:STATE_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":ACCT_ID ";
		ParamList+=",";
		ParamList+=":SERV_ID ";
		ParamList+=",";
		ParamList+=":ITEM_GROUP_ID ";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("ACCT_BALANCE_ID",&(a_item[0].ACCT_BALANCE_ID),sizeof(a_item[0]));
			qry.setParamArray("BALANCE_TYPE_ID",&(a_item[0].BALANCE_TYPE_ID),sizeof(a_item[0]));
			if (a_item[0].EFF_DATE[0]!='\0')
				qry.setParamArray("EFF_DATE",(char **)&(a_item[0].EFF_DATE),sizeof(a_item[0]),sizeof(a_item[0].EFF_DATE));
			if (a_item[0].EXP_DATE[0]!='\0')
				qry.setParamArray("EXP_DATE",(char **)&(a_item[0].EXP_DATE),sizeof(a_item[0]),sizeof(a_item[0].EXP_DATE));
			qry.setParamArray("BALANCE",&(a_item[0].BALANCE),sizeof(a_item[0]));
			qry.setParamArray("CYCLE_UPPER",&(a_item[0].CYCLE_UPPER),sizeof(a_item[0]));
			qry.setParamArray("CYCLE_LOWER",&(a_item[0].CYCLE_LOWER),sizeof(a_item[0]));
			qry.setParamArray("CYCLE_UPPER_TYPE",(char **)&(a_item[0].CYCLE_UPPER_TYPE),sizeof(a_item[0]),sizeof(a_item[0].CYCLE_UPPER_TYPE));
			qry.setParamArray("CYCLE_LOWER_TYPE",(char **)&(a_item[0].CYCLE_LOWER_TYPE),sizeof(a_item[0]),sizeof(a_item[0].CYCLE_LOWER_TYPE));
			qry.setParamArray("BANK_ACCT",(char **)&(a_item[0].BANK_ACCT),sizeof(a_item[0]),sizeof(a_item[0].BANK_ACCT));
			qry.setParamArray("STATE",(char **)&(a_item[0].STATE),sizeof(a_item[0]),sizeof(a_item[0].STATE));
			if (a_item[0].STATE_DATE[0]!='\0')
				qry.setParamArray("STATE_DATE",(char **)&(a_item[0].STATE_DATE),sizeof(a_item[0]),sizeof(a_item[0].STATE_DATE));
			qry.setParamArray("ACCT_ID",&(a_item[0].ACCT_ID),sizeof(a_item[0]));
			qry.setParamArray("SERV_ID",&(a_item[0].SERV_ID),sizeof(a_item[0]));
			qry.setParamArray("ITEM_GROUP_ID",&(a_item[0].ITEM_GROUP_ID),sizeof(a_item[0]));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_ACCT_BALANCE_STR_>	MEM_ACCT_BALANCE_CLASS::a_item;
hash_map<const long ,_TBL_ACCT_BALANCE_STR_* , hash<long>, MEM_ACCT_BALANCE_CLASS::eqlong>	MEM_ACCT_BALANCE_CLASS::hashmap;
MEM_ACCT_BALANCE_CLASS::MEM_ACCT_BALANCE_CLASS(char *sTableName)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_ACCT_BALANCE_CLASS::~MEM_ACCT_BALANCE_CLASS()
{
		/*UnLoad();*/
}

void	MEM_ACCT_BALANCE_CLASS::Load(char *sTableName)
{
		if (a_item.size()>0)	return ;
		TBL_ACCT_BALANCE_CLASS	 tempread;
		int		i;
		if (sTableName!=NULL)
				tempread.SetTableName(sTableName);
		tempread.Open();
		for(;tempread.Next();){
			a_item.push_back(tempread.item);
		}
		for(i=0;i<a_item.size();i++){
			hashmap[a_item[i].ACCT_BALANCE_ID]=&a_item[i];
		}
}

void	MEM_ACCT_BALANCE_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
}

_TBL_ACCT_BALANCE_STR_ *	MEM_ACCT_BALANCE_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


