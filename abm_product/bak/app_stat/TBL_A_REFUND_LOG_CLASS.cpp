/*VER: 1*/ 
#include	"TBL_A_REFUND_LOG_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_A_REFUND_LOG_CLASS::TBL_A_REFUND_LOG_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="A_REFUND_LOG";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_A_REFUND_LOG_CLASS::~TBL_A_REFUND_LOG_CLASS()
{
		FreeBuf();
}

void	TBL_A_REFUND_LOG_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_A_REFUND_LOG_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_A_REFUND_LOG_CLASS::Open()
{
		string strSql;
		strSql="SELECT REFUND_LOG_ID,NVL(STAFF_ID,-1) STAFF_ID,NVL(REFUND_REASON,-1) REFUND_REASON,NVL(CHARGE,-1) CHARGE,NVL(TO_CHAR(CREATED_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') CREATED_DATE,NVL(ACCT_ID,-1) ACCT_ID,NVL(SERV_ID,-1) SERV_ID,NVL(OPER_PAYOUT_ID,-1) OPER_PAYOUT_ID,NVL(STATE,-1) STATE,NVL(OLD_REFUND_LOG_ID,-1) OLD_REFUND_LOG_ID,NVL(A_REFUNDS_APPLY_ID,-1) A_REFUNDS_APPLY_ID from ";
		if	(sTableName.size()==0)
			strSql+="A_REFUND_LOG";
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
			cout<<"Error occured ... in TBL_A_REFUND_LOG_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_A_REFUND_LOG_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry not open!");
		try{
			if ( !m_qry->next())
				return false;
			item.REFUND_LOG_ID=m_qry->field("REFUND_LOG_ID").asLong();
			item.STAFF_ID=m_qry->field("STAFF_ID").asLong();
			strcpy(item.REFUND_REASON,m_qry->field("REFUND_REASON").asString());
			item.CHARGE=m_qry->field("CHARGE").asLong();
			strcpy(item.CREATED_DATE,m_qry->field("CREATED_DATE").asString());
			item.ACCT_ID=m_qry->field("ACCT_ID").asLong();
			item.SERV_ID=m_qry->field("SERV_ID").asLong();
			item.OPER_PAYOUT_ID=m_qry->field("OPER_PAYOUT_ID").asLong();
			strcpy(item.STATE,m_qry->field("STATE").asString());
			item.OLD_REFUND_LOG_ID=m_qry->field("OLD_REFUND_LOG_ID").asLong();
			item.A_REFUNDS_APPLY_ID=m_qry->field("A_REFUNDS_APPLY_ID").asLong();
		return true;
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_A_REFUND_LOG_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_A_REFUND_LOG_CLASS::Get_REFUND_LOG_ID(void)
{
		return	item.REFUND_LOG_ID;
}

long	TBL_A_REFUND_LOG_CLASS::Get_STAFF_ID(void)
{
		return	item.STAFF_ID;
}

char*	TBL_A_REFUND_LOG_CLASS::Get_REFUND_REASON(void)
{
		return	item.REFUND_REASON;
}

long	TBL_A_REFUND_LOG_CLASS::Get_CHARGE(void)
{
		return	item.CHARGE;
}

char*	TBL_A_REFUND_LOG_CLASS::Get_CREATED_DATE(void)
{
		return	item.CREATED_DATE;
}

long	TBL_A_REFUND_LOG_CLASS::Get_ACCT_ID(void)
{
		return	item.ACCT_ID;
}

long	TBL_A_REFUND_LOG_CLASS::Get_SERV_ID(void)
{
		return	item.SERV_ID;
}

long	TBL_A_REFUND_LOG_CLASS::Get_OPER_PAYOUT_ID(void)
{
		return	item.OPER_PAYOUT_ID;
}

char*	TBL_A_REFUND_LOG_CLASS::Get_STATE(void)
{
		return	item.STATE;
}

long	TBL_A_REFUND_LOG_CLASS::Get_OLD_REFUND_LOG_ID(void)
{
		return	item.OLD_REFUND_LOG_ID;
}

long	TBL_A_REFUND_LOG_CLASS::Get_A_REFUNDS_APPLY_ID(void)
{
		return	item.A_REFUNDS_APPLY_ID;
}

void	TBL_A_REFUND_LOG_CLASS::Set_REFUND_LOG_ID(long	lParam)
{
		item.REFUND_LOG_ID=lParam;
}

void	TBL_A_REFUND_LOG_CLASS::Set_STAFF_ID(long	lParam)
{
		item.STAFF_ID=lParam;
}

void	TBL_A_REFUND_LOG_CLASS::Set_REFUND_REASON(char	* sParam)
{
		if ( strlen(sParam)>250 ) 
			 throw TException( " Set_REFUND_REASON  string over 250!");
		strcpy(item.REFUND_REASON,sParam);
}

void	TBL_A_REFUND_LOG_CLASS::Set_CHARGE(long	lParam)
{
		item.CHARGE=lParam;
}

void	TBL_A_REFUND_LOG_CLASS::Set_CREATED_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_CREATED_DATE  string over 14!");
		strcpy(item.CREATED_DATE,sParam);
}

void	TBL_A_REFUND_LOG_CLASS::Set_ACCT_ID(long	lParam)
{
		item.ACCT_ID=lParam;
}

void	TBL_A_REFUND_LOG_CLASS::Set_SERV_ID(long	lParam)
{
		item.SERV_ID=lParam;
}

void	TBL_A_REFUND_LOG_CLASS::Set_OPER_PAYOUT_ID(long	lParam)
{
		item.OPER_PAYOUT_ID=lParam;
}

void	TBL_A_REFUND_LOG_CLASS::Set_STATE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_STATE  string over 3!");
		strcpy(item.STATE,sParam);
}

void	TBL_A_REFUND_LOG_CLASS::Set_OLD_REFUND_LOG_ID(long	lParam)
{
		item.OLD_REFUND_LOG_ID=lParam;
}

void	TBL_A_REFUND_LOG_CLASS::Set_A_REFUNDS_APPLY_ID(long	lParam)
{
		item.A_REFUNDS_APPLY_ID=lParam;
}

void	TBL_A_REFUND_LOG_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_A_REFUND_LOG_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_A_REFUND_LOG_CLASS::SetBuf(int iBufLen)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_A_REFUND_LOG_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_A_REFUND_LOG_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_A_REFUND_LOG_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="REFUND_LOG_ID,STAFF_ID,REFUND_REASON,CHARGE,CREATED_DATE,ACCT_ID,SERV_ID,OPER_PAYOUT_ID,STATE,OLD_REFUND_LOG_ID,A_REFUNDS_APPLY_ID";
		ParamList="";
		ParamList+=":REFUND_LOG_ID ";
		ParamList+=",";
		ParamList+=":STAFF_ID ";
		ParamList+=",";
		ParamList+=":REFUND_REASON ";
		ParamList+=",";
		ParamList+=":CHARGE ";
		ParamList+=",";
		if (a_item[0].CREATED_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:CREATED_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":ACCT_ID ";
		ParamList+=",";
		ParamList+=":SERV_ID ";
		ParamList+=",";
		ParamList+=":OPER_PAYOUT_ID ";
		ParamList+=",";
		ParamList+=":STATE ";
		ParamList+=",";
		ParamList+=":OLD_REFUND_LOG_ID ";
		ParamList+=",";
		ParamList+=":A_REFUNDS_APPLY_ID ";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("REFUND_LOG_ID",&(a_item[0].REFUND_LOG_ID),sizeof(a_item[0]));
			qry.setParamArray("STAFF_ID",&(a_item[0].STAFF_ID),sizeof(a_item[0]));
			qry.setParamArray("REFUND_REASON",(char **)&(a_item[0].REFUND_REASON),sizeof(a_item[0]),sizeof(a_item[0].REFUND_REASON));
			qry.setParamArray("CHARGE",&(a_item[0].CHARGE),sizeof(a_item[0]));
			if (a_item[0].CREATED_DATE[0]!='\0')
				qry.setParamArray("CREATED_DATE",(char **)&(a_item[0].CREATED_DATE),sizeof(a_item[0]),sizeof(a_item[0].CREATED_DATE));
			qry.setParamArray("ACCT_ID",&(a_item[0].ACCT_ID),sizeof(a_item[0]));
			qry.setParamArray("SERV_ID",&(a_item[0].SERV_ID),sizeof(a_item[0]));
			qry.setParamArray("OPER_PAYOUT_ID",&(a_item[0].OPER_PAYOUT_ID),sizeof(a_item[0]));
			qry.setParamArray("STATE",(char **)&(a_item[0].STATE),sizeof(a_item[0]),sizeof(a_item[0].STATE));
			qry.setParamArray("OLD_REFUND_LOG_ID",&(a_item[0].OLD_REFUND_LOG_ID),sizeof(a_item[0]));
			qry.setParamArray("A_REFUNDS_APPLY_ID",&(a_item[0].A_REFUNDS_APPLY_ID),sizeof(a_item[0]));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_A_REFUND_LOG_STR_>	MEM_A_REFUND_LOG_CLASS::a_item;
hash_map<const long ,_TBL_A_REFUND_LOG_STR_* , hash<long>, MEM_A_REFUND_LOG_CLASS::eqlong>	MEM_A_REFUND_LOG_CLASS::hashmap;
MEM_A_REFUND_LOG_CLASS::MEM_A_REFUND_LOG_CLASS(char *sTableName)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_A_REFUND_LOG_CLASS::~MEM_A_REFUND_LOG_CLASS()
{
		/*UnLoad();*/
}

void	MEM_A_REFUND_LOG_CLASS::Load(char *sTableName)
{
		if (a_item.size()>0)	return ;
		TBL_A_REFUND_LOG_CLASS	 tempread;
		int		i;
		if (sTableName!=NULL)
				tempread.SetTableName(sTableName);
		tempread.Open();
		for(;tempread.Next();){
			a_item.push_back(tempread.item);
		}
		for(i=0;i<a_item.size();i++){
			hashmap[a_item[i].REFUND_LOG_ID]=&a_item[i];
		}
}

void	MEM_A_REFUND_LOG_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
}

_TBL_A_REFUND_LOG_STR_ *	MEM_A_REFUND_LOG_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


