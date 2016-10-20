/*VER: 1*/ 
#include	"TBL_ACCT_ITEM_SOURCE_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_ACCT_ITEM_SOURCE_CLASS::TBL_ACCT_ITEM_SOURCE_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="ACCT_ITEM_SOURCE";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_ACCT_ITEM_SOURCE_CLASS::~TBL_ACCT_ITEM_SOURCE_CLASS()
{
		FreeBuf();
}

void	TBL_ACCT_ITEM_SOURCE_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_ACCT_ITEM_SOURCE_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_ACCT_ITEM_SOURCE_CLASS::Open()
{
		string strSql;
		strSql="SELECT ITEM_SOURCE_ID,ACCT_ITEM_TYPE_ID,ITEM_SOURCE_TYPE,NAME,DESCRIPT from ";
		if	(sTableName.size()==0)
			strSql+="ACCT_ITEM_SOURCE";
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
			cout<<"Error occured ... in TBL_ACCT_ITEM_SOURCE_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_ACCT_ITEM_SOURCE_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry not open!");
		try{
			if ( !m_qry->next())
				return false;
			item.ITEM_SOURCE_ID=m_qry->field("ITEM_SOURCE_ID").asLong();
			item.ACCT_ITEM_TYPE_ID=m_qry->field("ACCT_ITEM_TYPE_ID").asLong();
			strcpy(item.ITEM_SOURCE_TYPE,m_qry->field("ITEM_SOURCE_TYPE").asString());
			strcpy(item.NAME,m_qry->field("NAME").asString());
			strcpy(item.DESCRIPT,m_qry->field("DESCRIPT").asString());
		return true;
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_ACCT_ITEM_SOURCE_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_ACCT_ITEM_SOURCE_CLASS::Get_ITEM_SOURCE_ID(void)
{
		return	item.ITEM_SOURCE_ID;
}

long	TBL_ACCT_ITEM_SOURCE_CLASS::Get_ACCT_ITEM_TYPE_ID(void)
{
		return	item.ACCT_ITEM_TYPE_ID;
}

char*	TBL_ACCT_ITEM_SOURCE_CLASS::Get_ITEM_SOURCE_TYPE(void)
{
		return	item.ITEM_SOURCE_TYPE;
}

char*	TBL_ACCT_ITEM_SOURCE_CLASS::Get_NAME(void)
{
		return	item.NAME;
}

char*	TBL_ACCT_ITEM_SOURCE_CLASS::Get_DESCRIPT(void)
{
		return	item.DESCRIPT;
}

void	TBL_ACCT_ITEM_SOURCE_CLASS::Set_ITEM_SOURCE_ID(long	lParam)
{
		item.ITEM_SOURCE_ID=lParam;
}

void	TBL_ACCT_ITEM_SOURCE_CLASS::Set_ACCT_ITEM_TYPE_ID(long	lParam)
{
		item.ACCT_ITEM_TYPE_ID=lParam;
}

void	TBL_ACCT_ITEM_SOURCE_CLASS::Set_ITEM_SOURCE_TYPE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_ITEM_SOURCE_TYPE  string over 3!");
		strcpy(item.ITEM_SOURCE_TYPE,sParam);
}

void	TBL_ACCT_ITEM_SOURCE_CLASS::Set_NAME(char	* sParam)
{
		if ( strlen(sParam)>50 ) 
			 throw TException( " Set_NAME  string over 50!");
		strcpy(item.NAME,sParam);
}

void	TBL_ACCT_ITEM_SOURCE_CLASS::Set_DESCRIPT(char	* sParam)
{
		if ( strlen(sParam)>250 ) 
			 throw TException( " Set_DESCRIPT  string over 250!");
		strcpy(item.DESCRIPT,sParam);
}

void	TBL_ACCT_ITEM_SOURCE_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_ACCT_ITEM_SOURCE_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_ACCT_ITEM_SOURCE_CLASS::SetBuf(int iBufLen)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_ACCT_ITEM_SOURCE_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_ACCT_ITEM_SOURCE_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_ACCT_ITEM_SOURCE_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="ITEM_SOURCE_ID,ACCT_ITEM_TYPE_ID,ITEM_SOURCE_TYPE,NAME,DESCRIPT";
		ParamList="";
		ParamList+=":ITEM_SOURCE_ID ";
		ParamList+=",";
		ParamList+=":ACCT_ITEM_TYPE_ID ";
		ParamList+=",";
		ParamList+=":ITEM_SOURCE_TYPE ";
		ParamList+=",";
		ParamList+=":NAME ";
		ParamList+=",";
		ParamList+=":DESCRIPT ";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("ITEM_SOURCE_ID",&(a_item[0].ITEM_SOURCE_ID),sizeof(a_item[0]));
			qry.setParamArray("ACCT_ITEM_TYPE_ID",&(a_item[0].ACCT_ITEM_TYPE_ID),sizeof(a_item[0]));
			qry.setParamArray("ITEM_SOURCE_TYPE",(char **)&(a_item[0].ITEM_SOURCE_TYPE),sizeof(a_item[0]),sizeof(a_item[0].ITEM_SOURCE_TYPE));
			qry.setParamArray("NAME",(char **)&(a_item[0].NAME),sizeof(a_item[0]),sizeof(a_item[0].NAME));
			qry.setParamArray("DESCRIPT",(char **)&(a_item[0].DESCRIPT),sizeof(a_item[0]),sizeof(a_item[0].DESCRIPT));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_ACCT_ITEM_SOURCE_STR_>	MEM_ACCT_ITEM_SOURCE_CLASS::a_item;
hash_map<const long ,_TBL_ACCT_ITEM_SOURCE_STR_* , hash<long>, MEM_ACCT_ITEM_SOURCE_CLASS::eqlong>	MEM_ACCT_ITEM_SOURCE_CLASS::hashmap;
bool MEM_ACCT_ITEM_SOURCE_CLASS::bLoadFlag=false;
MEM_ACCT_ITEM_SOURCE_CLASS::MEM_ACCT_ITEM_SOURCE_CLASS(char *sTableName)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_ACCT_ITEM_SOURCE_CLASS::~MEM_ACCT_ITEM_SOURCE_CLASS()
{
		/*UnLoad();*/
}

void	MEM_ACCT_ITEM_SOURCE_CLASS::Load(char *sTableName)
{
		if (bLoadFlag)	return ;
		TBL_ACCT_ITEM_SOURCE_CLASS	 tempread;
		int		i;
		if (sTableName!=NULL)
				tempread.SetTableName(sTableName);
		tempread.Open();
		for(;tempread.Next();){
			a_item.push_back(tempread.item);
		}
		for(i=0;i<a_item.size();i++){
			hashmap[a_item[i].ITEM_SOURCE_ID]=&a_item[i];
		}
		bLoadFlag=true;
}

void	MEM_ACCT_ITEM_SOURCE_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
		bLoadFlag=false;
}

_TBL_ACCT_ITEM_SOURCE_STR_ *	MEM_ACCT_ITEM_SOURCE_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


