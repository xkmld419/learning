/*VER: 1*/ 
#include	"TBL_SERV_ACCT_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_SERV_ACCT_CLASS::TBL_SERV_ACCT_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="SERV_ACCT";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_SERV_ACCT_CLASS::~TBL_SERV_ACCT_CLASS()
{
		FreeBuf();
}

void	TBL_SERV_ACCT_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_SERV_ACCT_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_SERV_ACCT_CLASS::Open()
{
		string strSql;
		strSql="SELECT ACCT_ID,SERV_ID,ITEM_GROUP_ID,BILL_REQUIRE_ID,INVOICE_REQUIRE_ID,PAYMENT_RULE_ID,NVL(STATE,-1) STATE,TO_CHAR(STATE_DATE,'YYYYMMDDHH24MISS') STATE_DATE from ";
		if	(sTableName.size()==0)
			strSql+="SERV_ACCT";
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
			cout<<"Error occured ... in TBL_SERV_ACCT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_SERV_ACCT_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry not open!");
		try{
			if ( !m_qry->next())
				return false;
			item.ACCT_ID=m_qry->field("ACCT_ID").asLong();
			item.SERV_ID=m_qry->field("SERV_ID").asLong();
			item.ITEM_GROUP_ID=m_qry->field("ITEM_GROUP_ID").asLong();
			item.BILL_REQUIRE_ID=m_qry->field("BILL_REQUIRE_ID").asLong();
			item.INVOICE_REQUIRE_ID=m_qry->field("INVOICE_REQUIRE_ID").asLong();
			item.PAYMENT_RULE_ID=m_qry->field("PAYMENT_RULE_ID").asLong();
			strcpy(item.STATE,m_qry->field("STATE").asString());
			strcpy(item.STATE_DATE,m_qry->field("STATE_DATE").asString());
		return true;
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_SERV_ACCT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_SERV_ACCT_CLASS::Get_ACCT_ID(void)
{
		return	item.ACCT_ID;
}

long	TBL_SERV_ACCT_CLASS::Get_SERV_ID(void)
{
		return	item.SERV_ID;
}

long	TBL_SERV_ACCT_CLASS::Get_ITEM_GROUP_ID(void)
{
		return	item.ITEM_GROUP_ID;
}

long	TBL_SERV_ACCT_CLASS::Get_BILL_REQUIRE_ID(void)
{
		return	item.BILL_REQUIRE_ID;
}

long	TBL_SERV_ACCT_CLASS::Get_INVOICE_REQUIRE_ID(void)
{
		return	item.INVOICE_REQUIRE_ID;
}

long	TBL_SERV_ACCT_CLASS::Get_PAYMENT_RULE_ID(void)
{
		return	item.PAYMENT_RULE_ID;
}

char*	TBL_SERV_ACCT_CLASS::Get_STATE(void)
{
		return	item.STATE;
}

char*	TBL_SERV_ACCT_CLASS::Get_STATE_DATE(void)
{
		return	item.STATE_DATE;
}

void	TBL_SERV_ACCT_CLASS::Set_ACCT_ID(long	lParam)
{
		item.ACCT_ID=lParam;
}

void	TBL_SERV_ACCT_CLASS::Set_SERV_ID(long	lParam)
{
		item.SERV_ID=lParam;
}

void	TBL_SERV_ACCT_CLASS::Set_ITEM_GROUP_ID(long	lParam)
{
		item.ITEM_GROUP_ID=lParam;
}

void	TBL_SERV_ACCT_CLASS::Set_BILL_REQUIRE_ID(long	lParam)
{
		item.BILL_REQUIRE_ID=lParam;
}

void	TBL_SERV_ACCT_CLASS::Set_INVOICE_REQUIRE_ID(long	lParam)
{
		item.INVOICE_REQUIRE_ID=lParam;
}

void	TBL_SERV_ACCT_CLASS::Set_PAYMENT_RULE_ID(long	lParam)
{
		item.PAYMENT_RULE_ID=lParam;
}

void	TBL_SERV_ACCT_CLASS::Set_STATE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_STATE  string over 3!");
		strcpy(item.STATE,sParam);
}

void	TBL_SERV_ACCT_CLASS::Set_STATE_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_STATE_DATE  string over 14!");
		strcpy(item.STATE_DATE,sParam);
}

void	TBL_SERV_ACCT_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_SERV_ACCT_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_SERV_ACCT_CLASS::SetBuf(int iBufLen)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_SERV_ACCT_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_SERV_ACCT_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_SERV_ACCT_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="ACCT_ID,SERV_ID,ITEM_GROUP_ID,BILL_REQUIRE_ID,INVOICE_REQUIRE_ID,PAYMENT_RULE_ID,STATE,STATE_DATE";
		ParamList="";
		ParamList+=":ACCT_ID ";
		ParamList+=",";
		ParamList+=":SERV_ID ";
		ParamList+=",";
		ParamList+=":ITEM_GROUP_ID ";
		ParamList+=",";
		ParamList+=":BILL_REQUIRE_ID ";
		ParamList+=",";
		ParamList+=":INVOICE_REQUIRE_ID ";
		ParamList+=",";
		ParamList+=":PAYMENT_RULE_ID ";
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
			qry.setParamArray("ACCT_ID",&(a_item[0].ACCT_ID),sizeof(a_item[0]));
			qry.setParamArray("SERV_ID",&(a_item[0].SERV_ID),sizeof(a_item[0]));
			qry.setParamArray("ITEM_GROUP_ID",&(a_item[0].ITEM_GROUP_ID),sizeof(a_item[0]));
			qry.setParamArray("BILL_REQUIRE_ID",&(a_item[0].BILL_REQUIRE_ID),sizeof(a_item[0]));
			qry.setParamArray("INVOICE_REQUIRE_ID",&(a_item[0].INVOICE_REQUIRE_ID),sizeof(a_item[0]));
			qry.setParamArray("PAYMENT_RULE_ID",&(a_item[0].PAYMENT_RULE_ID),sizeof(a_item[0]));
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

vector<_TBL_SERV_ACCT_STR_>	MEM_SERV_ACCT_CLASS::a_item;
hash_map<const long ,_TBL_SERV_ACCT_STR_* , hash<long>, MEM_SERV_ACCT_CLASS::eqlong>	MEM_SERV_ACCT_CLASS::hashmap;
MEM_SERV_ACCT_CLASS::MEM_SERV_ACCT_CLASS(char *sTableName)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_SERV_ACCT_CLASS::~MEM_SERV_ACCT_CLASS()
{
		/*UnLoad();*/
}

void	MEM_SERV_ACCT_CLASS::Load(char *sTableName)
{
		if (a_item.size()>0)	return ;
		TBL_SERV_ACCT_CLASS	 tempread;
		int		i;
		if (sTableName!=NULL)
				tempread.SetTableName(sTableName);
		tempread.Open();
		for(;tempread.Next();){
			a_item.push_back(tempread.item);
		}
		for(i=0;i<a_item.size();i++){
			hashmap[a_item[i].ACCT_ID]=&a_item[i];
		}
}

void	MEM_SERV_ACCT_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
}

_TBL_SERV_ACCT_STR_ *	MEM_SERV_ACCT_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


