/*VER: 1*/ 
#include	"TBL_STAFF_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_STAFF_CLASS::TBL_STAFF_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="STAFF";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_STAFF_CLASS::~TBL_STAFF_CLASS()
{
		FreeBuf();
}

void	TBL_STAFF_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_STAFF_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_STAFF_CLASS::Open()
{
		string strSql;
		strSql="SELECT STAFF_ID,STAFF_CODE,NVL(PASSWD,-1) PASSWD,STAFF_DESC,ORGAN_ID,NVL(PARENT_PARTYROLEID,-1) PARENT_PARTYROLEID,BSS_ORG_ID,NVL(WORK_GROUP_ID,-1) WORK_GROUP_ID from ";
		if	(sTableName.size()==0)
			strSql+="STAFF";
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
			cout<<"Error occured ... in TBL_STAFF_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_STAFF_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry 还没open!");
		try{
			if ( !m_qry->next())
				return false;
			item.STAFF_ID=m_qry->field("STAFF_ID").asFloat();
			strcpy(item.STAFF_CODE,m_qry->field("STAFF_CODE").asString());
			strcpy(item.PASSWD,m_qry->field("PASSWD").asString());
			strcpy(item.STAFF_DESC,m_qry->field("STAFF_DESC").asString());
			item.ORGAN_ID=m_qry->field("ORGAN_ID").asFloat();
			item.PARENT_PARTYROLEID=m_qry->field("PARENT_PARTYROLEID").asFloat();
			item.BSS_ORG_ID=m_qry->field("BSS_ORG_ID").asFloat();
			item.WORK_GROUP_ID=m_qry->field("WORK_GROUP_ID").asFloat();
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_STAFF_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_STAFF_CLASS::Get_STAFF_ID(void)
{
		return	item.STAFF_ID;
}

char*	TBL_STAFF_CLASS::Get_STAFF_CODE(void)
{
		return	item.STAFF_CODE;
}

char*	TBL_STAFF_CLASS::Get_PASSWD(void)
{
		return	item.PASSWD;
}

char*	TBL_STAFF_CLASS::Get_STAFF_DESC(void)
{
		return	item.STAFF_DESC;
}

long	TBL_STAFF_CLASS::Get_ORGAN_ID(void)
{
		return	item.ORGAN_ID;
}

long	TBL_STAFF_CLASS::Get_PARENT_PARTYROLEID(void)
{
		return	item.PARENT_PARTYROLEID;
}

long	TBL_STAFF_CLASS::Get_BSS_ORG_ID(void)
{
		return	item.BSS_ORG_ID;
}

long	TBL_STAFF_CLASS::Get_WORK_GROUP_ID(void)
{
		return	item.WORK_GROUP_ID;
}

void	TBL_STAFF_CLASS::Set_STAFF_ID(long	lParam)
{
		item.STAFF_ID=lParam;
}

void	TBL_STAFF_CLASS::Set_STAFF_CODE(char	* sParam)
{
		if ( strlen(sParam)>15 ) 
			 throw TException( " Set_STAFF_CODE 字符参数的长度超过15!");
		strcpy(item.STAFF_CODE,sParam);
}

void	TBL_STAFF_CLASS::Set_PASSWD(char	* sParam)
{
		if ( strlen(sParam)>30 ) 
			 throw TException( " Set_PASSWD 字符参数的长度超过30!");
		strcpy(item.PASSWD,sParam);
}

void	TBL_STAFF_CLASS::Set_STAFF_DESC(char	* sParam)
{
		if ( strlen(sParam)>250 ) 
			 throw TException( " Set_STAFF_DESC 字符参数的长度超过250!");
		strcpy(item.STAFF_DESC,sParam);
}

void	TBL_STAFF_CLASS::Set_ORGAN_ID(long	lParam)
{
		item.ORGAN_ID=lParam;
}

void	TBL_STAFF_CLASS::Set_PARENT_PARTYROLEID(long	lParam)
{
		item.PARENT_PARTYROLEID=lParam;
}

void	TBL_STAFF_CLASS::Set_BSS_ORG_ID(long	lParam)
{
		item.BSS_ORG_ID=lParam;
}

void	TBL_STAFF_CLASS::Set_WORK_GROUP_ID(long	lParam)
{
		item.WORK_GROUP_ID=lParam;
}

void	TBL_STAFF_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_STAFF_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_STAFF_CLASS::SetBuf(int iBufLen=ARRAY_BUFLEN)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_STAFF_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_STAFF_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_STAFF_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="STAFF_ID,STAFF_CODE,PASSWD,STAFF_DESC,ORGAN_ID,PARENT_PARTYROLEID,BSS_ORG_ID,WORK_GROUP_ID";
		ParamList="";
		ParamList+=":STAFF_ID ";
		ParamList+=",";
		ParamList+=":STAFF_CODE ";
		ParamList+=",";
		ParamList+=":PASSWD ";
		ParamList+=",";
		ParamList+=":STAFF_DESC ";
		ParamList+=",";
		ParamList+=":ORGAN_ID ";
		ParamList+=",";
		ParamList+=":PARENT_PARTYROLEID ";
		ParamList+=",";
		ParamList+=":BSS_ORG_ID ";
		ParamList+=",";
		ParamList+=":WORK_GROUP_ID ";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("STAFF_ID",&(a_item[0].STAFF_ID),sizeof(a_item[0]));
			qry.setParamArray("STAFF_CODE",(char **)&(a_item[0].STAFF_CODE),sizeof(a_item[0]),sizeof(a_item[0].STAFF_CODE));
			qry.setParamArray("PASSWD",(char **)&(a_item[0].PASSWD),sizeof(a_item[0]),sizeof(a_item[0].PASSWD));
			qry.setParamArray("STAFF_DESC",(char **)&(a_item[0].STAFF_DESC),sizeof(a_item[0]),sizeof(a_item[0].STAFF_DESC));
			qry.setParamArray("ORGAN_ID",&(a_item[0].ORGAN_ID),sizeof(a_item[0]));
			qry.setParamArray("PARENT_PARTYROLEID",&(a_item[0].PARENT_PARTYROLEID),sizeof(a_item[0]));
			qry.setParamArray("BSS_ORG_ID",&(a_item[0].BSS_ORG_ID),sizeof(a_item[0]));
			qry.setParamArray("WORK_GROUP_ID",&(a_item[0].WORK_GROUP_ID),sizeof(a_item[0]));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_STAFF_STR_>	MEM_STAFF_CLASS::a_item;
hash_map<const long ,_TBL_STAFF_STR_* , hash<long>, MEM_STAFF_CLASS::eqlong>	MEM_STAFF_CLASS::hashmap;
MEM_STAFF_CLASS::MEM_STAFF_CLASS(char *sTableName=NULL)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_STAFF_CLASS::~MEM_STAFF_CLASS()
{
		/*UnLoad();*/
}

void	MEM_STAFF_CLASS::Load(char *sTableName=NULL)
{
		if (a_item.size()>0)	return ;
		TBL_STAFF_CLASS	 tempread;
		int		i;
		if (sTableName!=NULL)
				tempread.SetTableName(sTableName);
		tempread.Open();
		for(;tempread.Next();){
			a_item.push_back(tempread.item);
		}
		for(i=0;i<a_item.size();i++){
			hashmap[a_item[i].STAFF_ID]=&a_item[i];
		}
}

void	MEM_STAFF_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
}

_TBL_STAFF_STR_ *	MEM_STAFF_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


