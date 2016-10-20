/*VER: 1*/ 
#include	"TBL_PAYMENT_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_PAYMENT_CLASS::TBL_PAYMENT_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="PAYMENT";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_PAYMENT_CLASS::~TBL_PAYMENT_CLASS()
{
		FreeBuf();
}

void	TBL_PAYMENT_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_PAYMENT_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_PAYMENT_CLASS::Open()
{
		string strSql;
		strSql="SELECT PAYMENT_ID,PAYMENT_METHOD,NVL(PAYED_METHOD,-1) PAYED_METHOD,OPERATION_TYPE,NVL(OPERATED_PAYMENT_SERIAL_NBR,-1) OPERATED_PAYMENT_SERIAL_NBR,AMOUNT,TO_CHAR(PAYMENT_DATE,'YYYYMMDDHH24MISS') PAYMENT_DATE,STATE,TO_CHAR(STATE_DATE,'YYYYMMDDHH24MISS') STATE_DATE,TO_CHAR(CREATED_DATE,'YYYYMMDDHH24MISS') CREATED_DATE,STAFF_ID from ";
		if	(sTableName.size()==0)
			strSql+="PAYMENT";
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
			cout<<"Error occured ... in TBL_PAYMENT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_PAYMENT_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry not open!");
		try{
			if ( !m_qry->next())
				return false;
			item.PAYMENT_ID=m_qry->field("PAYMENT_ID").asLong();
			item.PAYMENT_METHOD=m_qry->field("PAYMENT_METHOD").asLong();
			item.PAYED_METHOD=m_qry->field("PAYED_METHOD").asLong();
			strcpy(item.OPERATION_TYPE,m_qry->field("OPERATION_TYPE").asString());
			item.OPERATED_PAYMENT_SERIAL_NBR=m_qry->field("OPERATED_PAYMENT_SERIAL_NBR").asLong();
			item.AMOUNT=m_qry->field("AMOUNT").asLong();
			strcpy(item.PAYMENT_DATE,m_qry->field("PAYMENT_DATE").asString());
			strcpy(item.STATE,m_qry->field("STATE").asString());
			strcpy(item.STATE_DATE,m_qry->field("STATE_DATE").asString());
			strcpy(item.CREATED_DATE,m_qry->field("CREATED_DATE").asString());
			item.STAFF_ID=m_qry->field("STAFF_ID").asLong();
		return true;
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_PAYMENT_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_PAYMENT_CLASS::Get_PAYMENT_ID(void)
{
		return	item.PAYMENT_ID;
}

long	TBL_PAYMENT_CLASS::Get_PAYMENT_METHOD(void)
{
		return	item.PAYMENT_METHOD;
}

long	TBL_PAYMENT_CLASS::Get_PAYED_METHOD(void)
{
		return	item.PAYED_METHOD;
}

char*	TBL_PAYMENT_CLASS::Get_OPERATION_TYPE(void)
{
		return	item.OPERATION_TYPE;
}

long	TBL_PAYMENT_CLASS::Get_OPERATED_PAYMENT_SERIAL_NBR(void)
{
		return	item.OPERATED_PAYMENT_SERIAL_NBR;
}

long	TBL_PAYMENT_CLASS::Get_AMOUNT(void)
{
		return	item.AMOUNT;
}

char*	TBL_PAYMENT_CLASS::Get_PAYMENT_DATE(void)
{
		return	item.PAYMENT_DATE;
}

char*	TBL_PAYMENT_CLASS::Get_STATE(void)
{
		return	item.STATE;
}

char*	TBL_PAYMENT_CLASS::Get_STATE_DATE(void)
{
		return	item.STATE_DATE;
}

char*	TBL_PAYMENT_CLASS::Get_CREATED_DATE(void)
{
		return	item.CREATED_DATE;
}

long	TBL_PAYMENT_CLASS::Get_STAFF_ID(void)
{
		return	item.STAFF_ID;
}

void	TBL_PAYMENT_CLASS::Set_PAYMENT_ID(long	lParam)
{
		item.PAYMENT_ID=lParam;
}

void	TBL_PAYMENT_CLASS::Set_PAYMENT_METHOD(long	lParam)
{
		item.PAYMENT_METHOD=lParam;
}

void	TBL_PAYMENT_CLASS::Set_PAYED_METHOD(long	lParam)
{
		item.PAYED_METHOD=lParam;
}

void	TBL_PAYMENT_CLASS::Set_OPERATION_TYPE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_OPERATION_TYPE  string over 3!");
		strcpy(item.OPERATION_TYPE,sParam);
}

void	TBL_PAYMENT_CLASS::Set_OPERATED_PAYMENT_SERIAL_NBR(long	lParam)
{
		item.OPERATED_PAYMENT_SERIAL_NBR=lParam;
}

void	TBL_PAYMENT_CLASS::Set_AMOUNT(long	lParam)
{
		item.AMOUNT=lParam;
}

void	TBL_PAYMENT_CLASS::Set_PAYMENT_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_PAYMENT_DATE  string over 14!");
		strcpy(item.PAYMENT_DATE,sParam);
}

void	TBL_PAYMENT_CLASS::Set_STATE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_STATE  string over 3!");
		strcpy(item.STATE,sParam);
}

void	TBL_PAYMENT_CLASS::Set_STATE_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_STATE_DATE  string over 14!");
		strcpy(item.STATE_DATE,sParam);
}

void	TBL_PAYMENT_CLASS::Set_CREATED_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_CREATED_DATE  string over 14!");
		strcpy(item.CREATED_DATE,sParam);
}

void	TBL_PAYMENT_CLASS::Set_STAFF_ID(long	lParam)
{
		item.STAFF_ID=lParam;
}

void	TBL_PAYMENT_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_PAYMENT_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_PAYMENT_CLASS::SetBuf(int iBufLen)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_PAYMENT_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_PAYMENT_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_PAYMENT_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="PAYMENT_ID,PAYMENT_METHOD,PAYED_METHOD,OPERATION_TYPE,OPERATED_PAYMENT_SERIAL_NBR,AMOUNT,PAYMENT_DATE,STATE,STATE_DATE,CREATED_DATE,STAFF_ID";
		ParamList="";
		ParamList+=":PAYMENT_ID ";
		ParamList+=",";
		ParamList+=":PAYMENT_METHOD ";
		ParamList+=",";
		ParamList+=":PAYED_METHOD ";
		ParamList+=",";
		ParamList+=":OPERATION_TYPE ";
		ParamList+=",";
		ParamList+=":OPERATED_PAYMENT_SERIAL_NBR ";
		ParamList+=",";
		ParamList+=":AMOUNT ";
		ParamList+=",";
		if (a_item[0].PAYMENT_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:PAYMENT_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":STATE ";
		ParamList+=",";
		if (a_item[0].STATE_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:STATE_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		if (a_item[0].CREATED_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:CREATED_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":STAFF_ID ";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("PAYMENT_ID",&(a_item[0].PAYMENT_ID),sizeof(a_item[0]));
			qry.setParamArray("PAYMENT_METHOD",&(a_item[0].PAYMENT_METHOD),sizeof(a_item[0]));
			qry.setParamArray("PAYED_METHOD",&(a_item[0].PAYED_METHOD),sizeof(a_item[0]));
			qry.setParamArray("OPERATION_TYPE",(char **)&(a_item[0].OPERATION_TYPE),sizeof(a_item[0]),sizeof(a_item[0].OPERATION_TYPE));
			qry.setParamArray("OPERATED_PAYMENT_SERIAL_NBR",&(a_item[0].OPERATED_PAYMENT_SERIAL_NBR),sizeof(a_item[0]));
			qry.setParamArray("AMOUNT",&(a_item[0].AMOUNT),sizeof(a_item[0]));
			if (a_item[0].PAYMENT_DATE[0]!='\0')
				qry.setParamArray("PAYMENT_DATE",(char **)&(a_item[0].PAYMENT_DATE),sizeof(a_item[0]),sizeof(a_item[0].PAYMENT_DATE));
			qry.setParamArray("STATE",(char **)&(a_item[0].STATE),sizeof(a_item[0]),sizeof(a_item[0].STATE));
			if (a_item[0].STATE_DATE[0]!='\0')
				qry.setParamArray("STATE_DATE",(char **)&(a_item[0].STATE_DATE),sizeof(a_item[0]),sizeof(a_item[0].STATE_DATE));
			if (a_item[0].CREATED_DATE[0]!='\0')
				qry.setParamArray("CREATED_DATE",(char **)&(a_item[0].CREATED_DATE),sizeof(a_item[0]),sizeof(a_item[0].CREATED_DATE));
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

vector<_TBL_PAYMENT_STR_>	MEM_PAYMENT_CLASS::a_item;
hash_map<const long ,_TBL_PAYMENT_STR_* , hash<long>, MEM_PAYMENT_CLASS::eqlong>	MEM_PAYMENT_CLASS::hashmap;
MEM_PAYMENT_CLASS::MEM_PAYMENT_CLASS(char *sTableName)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_PAYMENT_CLASS::~MEM_PAYMENT_CLASS()
{
		/*UnLoad();*/
}

void	MEM_PAYMENT_CLASS::Load(char *sTableName)
{
		if (a_item.size()>0)	return ;
		TBL_PAYMENT_CLASS	 tempread;
		int		i;
		if (sTableName!=NULL)
				tempread.SetTableName(sTableName);
		tempread.Open();
		for(;tempread.Next();){
			a_item.push_back(tempread.item);
		}
		for(i=0;i<a_item.size();i++){
			hashmap[a_item[i].PAYMENT_ID]=&a_item[i];
		}
}

void	MEM_PAYMENT_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
}

_TBL_PAYMENT_STR_ *	MEM_PAYMENT_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


