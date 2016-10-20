/*VER: 1*/ 
#include	"TBL_ACCT_ITEM_AGGR_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_ACCT_ITEM_AGGR_CLASS::TBL_ACCT_ITEM_AGGR_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="ACCT_ITEM_AGGR";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_ACCT_ITEM_AGGR_CLASS::~TBL_ACCT_ITEM_AGGR_CLASS()
{
		FreeBuf();
}

void	TBL_ACCT_ITEM_AGGR_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_ACCT_ITEM_AGGR_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_ACCT_ITEM_AGGR_CLASS::Open()
{
		string strSql;
		strSql="SELECT ACCT_ITEM_ID,SERV_ID,BILLING_CYCLE_ID,ACCT_ITEM_TYPE_ID,CHARGE,ACCT_ID from ";
		if	(sTableName.size()==0)
			strSql+="ACCT_ITEM_AGGR";
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
			cout<<"Error occured ... in TBL_ACCT_ITEM_AGGR_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_ACCT_ITEM_AGGR_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry not open!");
		try{
			if ( !m_qry->next())
				return false;
			item.ACCT_ITEM_ID=m_qry->field("ACCT_ITEM_ID").asLong();
			item.SERV_ID=m_qry->field("SERV_ID").asLong();
			item.BILLING_CYCLE_ID=m_qry->field("BILLING_CYCLE_ID").asLong();
			item.ACCT_ITEM_TYPE_ID=m_qry->field("ACCT_ITEM_TYPE_ID").asLong();
			item.CHARGE=m_qry->field("CHARGE").asLong();
			item.ACCT_ID=m_qry->field("ACCT_ID").asLong();
		return true;
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_ACCT_ITEM_AGGR_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_ACCT_ITEM_AGGR_CLASS::Get_ACCT_ITEM_ID(void)
{
		return	item.ACCT_ITEM_ID;
}

long	TBL_ACCT_ITEM_AGGR_CLASS::Get_SERV_ID(void)
{
		return	item.SERV_ID;
}

long	TBL_ACCT_ITEM_AGGR_CLASS::Get_BILLING_CYCLE_ID(void)
{
		return	item.BILLING_CYCLE_ID;
}

long	TBL_ACCT_ITEM_AGGR_CLASS::Get_ACCT_ITEM_TYPE_ID(void)
{
		return	item.ACCT_ITEM_TYPE_ID;
}

long	TBL_ACCT_ITEM_AGGR_CLASS::Get_CHARGE(void)
{
		return	item.CHARGE;
}

long	TBL_ACCT_ITEM_AGGR_CLASS::Get_ACCT_ID(void)
{
		return	item.ACCT_ID;
}

void	TBL_ACCT_ITEM_AGGR_CLASS::Set_ACCT_ITEM_ID(long	lParam)
{
		item.ACCT_ITEM_ID=lParam;
}

void	TBL_ACCT_ITEM_AGGR_CLASS::Set_SERV_ID(long	lParam)
{
		item.SERV_ID=lParam;
}

void	TBL_ACCT_ITEM_AGGR_CLASS::Set_BILLING_CYCLE_ID(long	lParam)
{
		item.BILLING_CYCLE_ID=lParam;
}

void	TBL_ACCT_ITEM_AGGR_CLASS::Set_ACCT_ITEM_TYPE_ID(long	lParam)
{
		item.ACCT_ITEM_TYPE_ID=lParam;
}

void	TBL_ACCT_ITEM_AGGR_CLASS::Set_CHARGE(long	lParam)
{
		item.CHARGE=lParam;
}

void	TBL_ACCT_ITEM_AGGR_CLASS::Set_ACCT_ID(long	lParam)
{
		item.ACCT_ID=lParam;
}

void	TBL_ACCT_ITEM_AGGR_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_ACCT_ITEM_AGGR_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_ACCT_ITEM_AGGR_CLASS::SetBuf(int iBufLen)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_ACCT_ITEM_AGGR_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_ACCT_ITEM_AGGR_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_ACCT_ITEM_AGGR_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="ACCT_ITEM_ID,SERV_ID,BILLING_CYCLE_ID,ACCT_ITEM_TYPE_ID,CHARGE,ACCT_ID";
		ParamList="";
		ParamList+=":ACCT_ITEM_ID ";
		ParamList+=",";
		ParamList+=":SERV_ID ";
		ParamList+=",";
		ParamList+=":BILLING_CYCLE_ID ";
		ParamList+=",";
		ParamList+=":ACCT_ITEM_TYPE_ID ";
		ParamList+=",";
		ParamList+=":CHARGE ";
		ParamList+=",";
		ParamList+=":ACCT_ID ";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("ACCT_ITEM_ID",&(a_item[0].ACCT_ITEM_ID),sizeof(a_item[0]));
			qry.setParamArray("SERV_ID",&(a_item[0].SERV_ID),sizeof(a_item[0]));
			qry.setParamArray("BILLING_CYCLE_ID",&(a_item[0].BILLING_CYCLE_ID),sizeof(a_item[0]));
			qry.setParamArray("ACCT_ITEM_TYPE_ID",&(a_item[0].ACCT_ITEM_TYPE_ID),sizeof(a_item[0]));
			qry.setParamArray("CHARGE",&(a_item[0].CHARGE),sizeof(a_item[0]));
			qry.setParamArray("ACCT_ID",&(a_item[0].ACCT_ID),sizeof(a_item[0]));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_ACCT_ITEM_AGGR_STR_>	MEM_ACCT_ITEM_AGGR_CLASS::a_item;
hash_map<const long ,_TBL_ACCT_ITEM_AGGR_STR_* , hash<long>, MEM_ACCT_ITEM_AGGR_CLASS::eqlong>	MEM_ACCT_ITEM_AGGR_CLASS::hashmap;
bool	MEM_ACCT_ITEM_AGGR_CLASS::bLoadFlag=false;
MEM_ACCT_ITEM_AGGR_CLASS::MEM_ACCT_ITEM_AGGR_CLASS(char *sTableName)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_ACCT_ITEM_AGGR_CLASS::~MEM_ACCT_ITEM_AGGR_CLASS()
{
		/*UnLoad();*/
}

void	MEM_ACCT_ITEM_AGGR_CLASS::Load(char *sTableName)
{
		if (bLoadFlag)	return ;
		TBL_ACCT_ITEM_AGGR_CLASS	 tempread;
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
		bLoadFlag=true;
}

void	MEM_ACCT_ITEM_AGGR_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
		bLoadFlag=false;
}

_TBL_ACCT_ITEM_AGGR_STR_ *	MEM_ACCT_ITEM_AGGR_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


