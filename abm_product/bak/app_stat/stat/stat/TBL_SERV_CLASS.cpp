/*VER: 1*/ 
#include	"TBL_SERV_CLASS.h"
extern TOCIDatabase gpDBLink;
TBL_SERV_CLASS::TBL_SERV_CLASS()
{
		m_qry=NULL;
		sConditionSql="";
		sTableName="SERV";
		iCurPos=0;
		a_item=NULL;
		iBufferSize=ARRAY_BUFLEN;
}

TBL_SERV_CLASS::~TBL_SERV_CLASS()
{
		FreeBuf();
}

void	TBL_SERV_CLASS::AddCondition(string sql)
{
		this->sConditionSql=sql;
}

void	TBL_SERV_CLASS::SetTableName(string sTableName)
{
		this->sTableName=sTableName;
}

void	TBL_SERV_CLASS::Open()
{
		string strSql;
		strSql="SELECT SERV_ID,AGREEMENT_ID,CUST_ID,PRODUCT_ID,TO_CHAR(CREATE_DATE,'YYYYMMDDHH24MISS') CREATE_DATE,STATE,TO_CHAR(STATE_DATE,'YYYYMMDDHH24MISS') STATE_DATE,NVL(TO_CHAR(RENT_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') RENT_DATE,NVL(TO_CHAR(NSC_DATE,'YYYYMMDDHH24MISS') ,'00000000000000') NSC_DATE,NVL(CYCLE_TYPE_ID,-1) CYCLE_TYPE_ID,NVL(BILLING_MODE_ID,-1) BILLING_MODE_ID,NVL(ACC_NBR,-1) ACC_NBR,NVL(FREE_TYPE_ID,-1) FREE_TYPE_ID from ";
		if	(sTableName.size()==0)
			strSql+="SERV";
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
			cout<<"Error occured ... in TBL_SERV_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

bool	TBL_SERV_CLASS::Next()
{
		if ( m_qry == NULL)
			throw TException("mqry 还没open!");
		try{
			if ( !m_qry->next())
				return false;
			item.SERV_ID=m_qry->field("SERV_ID").asFloat();
			item.AGREEMENT_ID=m_qry->field("AGREEMENT_ID").asFloat();
			item.CUST_ID=m_qry->field("CUST_ID").asFloat();
			item.PRODUCT_ID=m_qry->field("PRODUCT_ID").asFloat();
			strcpy(item.CREATE_DATE,m_qry->field("CREATE_DATE").asString());
			strcpy(item.STATE,m_qry->field("STATE").asString());
			strcpy(item.STATE_DATE,m_qry->field("STATE_DATE").asString());
			strcpy(item.RENT_DATE,m_qry->field("RENT_DATE").asString());
			strcpy(item.NSC_DATE,m_qry->field("NSC_DATE").asString());
			item.CYCLE_TYPE_ID=m_qry->field("CYCLE_TYPE_ID").asFloat();
			item.BILLING_MODE_ID=m_qry->field("BILLING_MODE_ID").asFloat();
			strcpy(item.ACC_NBR,m_qry->field("ACC_NBR").asString());
			item.FREE_TYPE_ID=m_qry->field("FREE_TYPE_ID").asFloat();
		}
		catch (TOCIException &oe) {
			cout<<"Error occured ... in TBL_SERV_CLASS.cpp"<<endl;
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

long	TBL_SERV_CLASS::Get_SERV_ID(void)
{
		return	item.SERV_ID;
}

long	TBL_SERV_CLASS::Get_AGREEMENT_ID(void)
{
		return	item.AGREEMENT_ID;
}

long	TBL_SERV_CLASS::Get_CUST_ID(void)
{
		return	item.CUST_ID;
}

long	TBL_SERV_CLASS::Get_PRODUCT_ID(void)
{
		return	item.PRODUCT_ID;
}

char*	TBL_SERV_CLASS::Get_CREATE_DATE(void)
{
		return	item.CREATE_DATE;
}

char*	TBL_SERV_CLASS::Get_STATE(void)
{
		return	item.STATE;
}

char*	TBL_SERV_CLASS::Get_STATE_DATE(void)
{
		return	item.STATE_DATE;
}

char*	TBL_SERV_CLASS::Get_RENT_DATE(void)
{
		return	item.RENT_DATE;
}

char*	TBL_SERV_CLASS::Get_NSC_DATE(void)
{
		return	item.NSC_DATE;
}

long	TBL_SERV_CLASS::Get_CYCLE_TYPE_ID(void)
{
		return	item.CYCLE_TYPE_ID;
}

long	TBL_SERV_CLASS::Get_BILLING_MODE_ID(void)
{
		return	item.BILLING_MODE_ID;
}

char*	TBL_SERV_CLASS::Get_ACC_NBR(void)
{
		return	item.ACC_NBR;
}

long	TBL_SERV_CLASS::Get_FREE_TYPE_ID(void)
{
		return	item.FREE_TYPE_ID;
}

void	TBL_SERV_CLASS::Set_SERV_ID(long	lParam)
{
		item.SERV_ID=lParam;
}

void	TBL_SERV_CLASS::Set_AGREEMENT_ID(long	lParam)
{
		item.AGREEMENT_ID=lParam;
}

void	TBL_SERV_CLASS::Set_CUST_ID(long	lParam)
{
		item.CUST_ID=lParam;
}

void	TBL_SERV_CLASS::Set_PRODUCT_ID(long	lParam)
{
		item.PRODUCT_ID=lParam;
}

void	TBL_SERV_CLASS::Set_CREATE_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_CREATE_DATE 字符参数的长度超过14!");
		strcpy(item.CREATE_DATE,sParam);
}

void	TBL_SERV_CLASS::Set_STATE(char	* sParam)
{
		if ( strlen(sParam)>3 ) 
			 throw TException( " Set_STATE 字符参数的长度超过3!");
		strcpy(item.STATE,sParam);
}

void	TBL_SERV_CLASS::Set_STATE_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_STATE_DATE 字符参数的长度超过14!");
		strcpy(item.STATE_DATE,sParam);
}

void	TBL_SERV_CLASS::Set_RENT_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_RENT_DATE 字符参数的长度超过14!");
		strcpy(item.RENT_DATE,sParam);
}

void	TBL_SERV_CLASS::Set_NSC_DATE(char	* sParam)
{
		if ( strlen(sParam)>14 ) 
			 throw TException( " Set_NSC_DATE 字符参数的长度超过14!");
		strcpy(item.NSC_DATE,sParam);
}

void	TBL_SERV_CLASS::Set_CYCLE_TYPE_ID(long	lParam)
{
		item.CYCLE_TYPE_ID=lParam;
}

void	TBL_SERV_CLASS::Set_BILLING_MODE_ID(long	lParam)
{
		item.BILLING_MODE_ID=lParam;
}

void	TBL_SERV_CLASS::Set_ACC_NBR(char	* sParam)
{
		if ( strlen(sParam)>32 ) 
			 throw TException( " Set_ACC_NBR 字符参数的长度超过32!");
		strcpy(item.ACC_NBR,sParam);
}

void	TBL_SERV_CLASS::Set_FREE_TYPE_ID(long	lParam)
{
		item.FREE_TYPE_ID=lParam;
}

void	TBL_SERV_CLASS::AddItem(void)
{
		if (a_item==NULL)
			SetBuf();
		a_item[iCurPos++]=item;
		if (iCurPos==iBufferSize )
			SaveFlush();
}

void	TBL_SERV_CLASS::ClearBuf(void)
{
		item.ClearBuf();
		iCurPos=0;
		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");
		memset(a_item,0, sizeof(item)*iBufferSize);
}

void	TBL_SERV_CLASS::SetBuf(int iBufLen=ARRAY_BUFLEN)
{
		iBufferSize=iBufLen;
		a_item=new _TBL_SERV_STR_[iBufferSize];
		ClearBuf();
}

void	TBL_SERV_CLASS::FreeBuf(void)
{
		if (a_item!=NULL){
			delete []a_item;
			a_item=NULL;
			delete m_qry;
			m_qry=NULL;
		}
}

void	TBL_SERV_CLASS::SaveFlush(void)
{
		string ColumnList,ParamList,StrSql;

		if (a_item==NULL)
			 throw TException("buffer is empty,please malloc buffer!");

		if (iCurPos==0)
			 return;

		ColumnList="SERV_ID,AGREEMENT_ID,CUST_ID,PRODUCT_ID,CREATE_DATE,STATE,STATE_DATE,RENT_DATE,NSC_DATE,CYCLE_TYPE_ID,BILLING_MODE_ID,ACC_NBR,FREE_TYPE_ID";
		ParamList="";
		ParamList+=":SERV_ID ";
		ParamList+=",";
		ParamList+=":AGREEMENT_ID ";
		ParamList+=",";
		ParamList+=":CUST_ID ";
		ParamList+=",";
		ParamList+=":PRODUCT_ID ";
		ParamList+=",";
		if (a_item[0].CREATE_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:CREATE_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":STATE ";
		ParamList+=",";
		if (a_item[0].STATE_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:STATE_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		if (a_item[0].RENT_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:RENT_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		if (a_item[0].NSC_DATE[0]=='\0')
			ParamList+="SYSDATE ";
		else
			ParamList+="TO_DATE(:NSC_DATE,'YYYYMMDDHH24MISS')";
		ParamList+=",";
		ParamList+=":CYCLE_TYPE_ID ";
		ParamList+=",";
		ParamList+=":BILLING_MODE_ID ";
		ParamList+=",";
		ParamList+=":ACC_NBR ";
		ParamList+=",";
		ParamList+=":FREE_TYPE_ID ";
		try{
			TOCIQuery qry(&gpDBLink);
			StrSql=" INSERT INTO "+ sTableName + "(" +ColumnList+ ") VALUES (" + ParamList + ")" ;
			StrSql[StrSql.size()]='\0';
			qry.setSQL(StrSql.c_str());
			qry.setParamArray("SERV_ID",&(a_item[0].SERV_ID),sizeof(a_item[0]));
			qry.setParamArray("AGREEMENT_ID",&(a_item[0].AGREEMENT_ID),sizeof(a_item[0]));
			qry.setParamArray("CUST_ID",&(a_item[0].CUST_ID),sizeof(a_item[0]));
			qry.setParamArray("PRODUCT_ID",&(a_item[0].PRODUCT_ID),sizeof(a_item[0]));
			if (a_item[0].CREATE_DATE[0]!='\0')
				qry.setParamArray("CREATE_DATE",(char **)&(a_item[0].CREATE_DATE),sizeof(a_item[0]),sizeof(a_item[0].CREATE_DATE));
			qry.setParamArray("STATE",(char **)&(a_item[0].STATE),sizeof(a_item[0]),sizeof(a_item[0].STATE));
			if (a_item[0].STATE_DATE[0]!='\0')
				qry.setParamArray("STATE_DATE",(char **)&(a_item[0].STATE_DATE),sizeof(a_item[0]),sizeof(a_item[0].STATE_DATE));
			if (a_item[0].RENT_DATE[0]!='\0')
				qry.setParamArray("RENT_DATE",(char **)&(a_item[0].RENT_DATE),sizeof(a_item[0]),sizeof(a_item[0].RENT_DATE));
			if (a_item[0].NSC_DATE[0]!='\0')
				qry.setParamArray("NSC_DATE",(char **)&(a_item[0].NSC_DATE),sizeof(a_item[0]),sizeof(a_item[0].NSC_DATE));
			qry.setParamArray("CYCLE_TYPE_ID",&(a_item[0].CYCLE_TYPE_ID),sizeof(a_item[0]));
			qry.setParamArray("BILLING_MODE_ID",&(a_item[0].BILLING_MODE_ID),sizeof(a_item[0]));
			qry.setParamArray("ACC_NBR",(char **)&(a_item[0].ACC_NBR),sizeof(a_item[0]),sizeof(a_item[0].ACC_NBR));
			qry.setParamArray("FREE_TYPE_ID",&(a_item[0].FREE_TYPE_ID),sizeof(a_item[0]));
			qry.execute(iCurPos);
			iCurPos=0;
		}
		catch(TOCIException &oe){
			cout<<oe.getErrMsg()<<endl;
			cout<<oe.getErrSrc()<<endl;
			throw oe;
		}
}

vector<_TBL_SERV_STR_>	MEM_SERV_CLASS::a_item;
hash_map<const long ,_TBL_SERV_STR_* , hash<long>, MEM_SERV_CLASS::eqlong>	MEM_SERV_CLASS::hashmap;
MEM_SERV_CLASS::MEM_SERV_CLASS(char *sTableName=NULL)
{
		if (sTableName==NULL)
			Load();
		else
			Load(sTableName);
}

MEM_SERV_CLASS::~MEM_SERV_CLASS()
{
		/*UnLoad();*/
}

void	MEM_SERV_CLASS::Load(char *sTableName=NULL)
{
		if (a_item.size()>0)	return ;
		TBL_SERV_CLASS	 tempread;
		int		i;
		if (sTableName!=NULL)
				tempread.SetTableName(sTableName);
		tempread.Open();
		for(;tempread.Next();){
			a_item.push_back(tempread.item);
		}
		for(i=0;i<a_item.size();i++){
			hashmap[a_item[i].SERV_ID]=&a_item[i];
		}
}

void	MEM_SERV_CLASS::UnLoad(void)
{
		a_item.clear();
		hashmap.clear();
}

_TBL_SERV_STR_ *	MEM_SERV_CLASS::operator [](long opt)
{
		return hashmap[opt];
}


