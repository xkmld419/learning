#include "SpecialTariffMgr.h"
#include "Environment.h"
#include "Log.h"
#include "ParamDefineMgr.h"
#include "ID.h"
#include "Value.h"
#include "IDParser.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

HashList<SpecialTariff *>  *SpecialTariffMgr::m_poSpecialTariffIndex = 0;
HashList<SpecialTariff *>  *SpecialTariffMgr::m_poSpecialTariffCheckIndex = 0;
SpecialTariff * SpecialTariffMgr::m_poSpecialTariffData = 0;
SpecialTariff * SpecialTariffMgr::m_poSpecialTariffCheckData = 0;
int SpecialTariffMgr::m_iaAttrID[4] = {0};
int SpecialTariffMgr::m_iType = 0;
bool SpecialTariffMgr::bInit = false;
//long SpecialTariffMgr::m_lCharge = 0;
//char SpecialTariffMgr::m_sChargeAttrID[32] = {0};

SpecialTariffMgr::SpecialTariffMgr()
{
#ifdef PRIVATE_MEMORY
	if(!bInit){
		initParam();
		load();	
		bInit = true;
	}
#else
	if(!bInit){
		initParam();
		bInit = true;
	}
#endif
}

void SpecialTariffMgr::initParam()
{
	char countbuf[32];	
	/*memset(countbuf,'\0',sizeof(countbuf));
	if(!ParamDefineMgr::getParam("SP_SPECIAL_TARIFF", "CHARGE_ATTR_ID", countbuf))
	{
		THROW(MBC_SpecialTariffMgr+4);
	}
	sprintf(m_sChargeAttrID,"%s_0_0",countbuf);*/

	memset(countbuf,'\0',sizeof(countbuf));
	if(!ParamDefineMgr::getParam("SP_SPECIAL_TARIFF", "ATTR_ID_LIST", countbuf))
	{
		THROW(MBC_SpecialTariffMgr+2);
	}
	int iTemp,m = 0,i = 0;
	while(countbuf[i]!='\0')
    {
        iTemp= atoi(countbuf+i);
		m_iaAttrID[m] = iTemp;
		for( ; countbuf[i]!='\0'; i++){
            if(countbuf[i]==','){
				i++;
                break;
            }
		}
		m++;
		if(m>3){//只取四个属性
			break;
		}
    }
	if(m!=4){
		THROW(MBC_SpecialTariffMgr+3);
	}
	m_iType = 2;//计费类型直接设置为2

}

void SpecialTariffMgr::getAttrValue(EventSection * oEvent,SpecialTariff &oTariff)
{
#ifndef PRIVATE_MEMORY
	if(!bInit){
		initParam();
		bInit = true;
	}
#endif
	ID * pID = new ID("0_0_0");
	Value oVal;
	
	//上下行标识
	memset(&oVal,0,sizeof(Value));
	pID->m_iAttrID = m_iaAttrID[0];
	IDParser::getValue(oEvent,pID,&oVal);
	if(oVal.m_iType == NUMBER){
		oTariff.m_iUpDownFlag = oVal.m_lValue;
	}else{
		oTariff.m_iUpDownFlag = atoi(oVal.m_sValue);
	}
	//SP代码
	memset(&oVal,0,sizeof(Value));
	pID->m_iAttrID = m_iaAttrID[1];
	IDParser::getValue(oEvent,pID,&oVal);
	if(oVal.m_iType == NUMBER){
		sprintf(oTariff.m_sSPFLAG,"%ld",oVal.m_lValue);
	}else{
		strcpy(oTariff.m_sSPFLAG, oVal.m_sValue);
	}
	//接入码
	memset(&oVal,0,sizeof(Value));
	pID->m_iAttrID = m_iaAttrID[2];
	IDParser::getValue(oEvent,pID,&oVal);
	if(oVal.m_iType == NUMBER){
		sprintf(oTariff.m_sNBR,"%ld",oVal.m_lValue);
	}else{
		strcpy(oTariff.m_sNBR, oVal.m_sValue);
	}
	//业务码
	memset(&oVal,0,sizeof(Value));
	pID->m_iAttrID = m_iaAttrID[3];
	IDParser::getValue(oEvent,pID,&oVal);
	if(oVal.m_iType == NUMBER){
		sprintf(oTariff.m_sServiceNBR,"%ld",oVal.m_lValue);
	}else{
		strcpy(oTariff.m_sServiceNBR, oVal.m_sValue);
	}
	delete pID;
	pID = NULL;
	return;
}

/*long SpecialTariffMgr::getChargeValue(EventSection * oEvent)
{
	Value oVal;	
	//费用
	memset(&oVal,0,sizeof(Value));
	IDParser::getValue(oEvent,m_sChargeAttrID,&oVal);
	if(oVal.m_iType == NUMBER){
		m_lCharge = oVal.m_lValue;
	}else{
		m_lCharge = atol(oVal.m_sValue);
	}
	return m_lCharge;
}
*/
SpecialTariffMgr::~SpecialTariffMgr()
{
#ifdef PRIVATE_MEMORY
	if(m_poSpecialTariffIndex){
		delete m_poSpecialTariffIndex;
		m_poSpecialTariffIndex = NULL;
	}
	if(m_poSpecialTariffCheckIndex){
		delete m_poSpecialTariffCheckIndex;
		m_poSpecialTariffCheckIndex = NULL;
	}
	if(m_poSpecialTariffData){
		delete [] m_poSpecialTariffData;
		m_poSpecialTariffData = NULL;
	}
	if(m_poSpecialTariffCheckData){
		delete [] m_poSpecialTariffCheckData;
		m_poSpecialTariffCheckData = NULL;
	}
#endif
}

//取得费率
#ifdef PRIVATE_MEMORY
bool SpecialTariffMgr::getFeeByInfo (EventSection * oEvent,int &iCharge)
{
	if(!bInit){
		initParam();
		load();	
		bInit = true;
	}
	char sTemp[40];
	SpecialTariff * pSpecialTariff;
	SpecialTariff oTemp;
	getAttrValue(oEvent,oTemp);
		
	//int iUpDownFlag = oTemp.m_iUpDownFlag % 2;//对上下行进行用2取模
	sprintf(sTemp,"%d_%s_%d",oTemp.m_iUpDownFlag,oTemp.m_sSPFLAG,m_iType);
	if(m_poSpecialTariffIndex->get(sTemp,&pSpecialTariff)){
		while(pSpecialTariff){
			if((pSpecialTariff->m_sServiceNBR[0]=='*' 
				|| strcmp(pSpecialTariff->m_sServiceNBR,oTemp.m_sServiceNBR)==0) 
				&& strcmp(oEvent->m_sStartDate,pSpecialTariff->m_sEffDate)>=0 
				&& strcmp(oEvent->m_sStartDate,pSpecialTariff->m_sExpDate)<0 ){
					if(pSpecialTariff->m_sNBR[0]=='*' 
						|| strncmp(oTemp.m_sNBR,pSpecialTariff->m_sNBR,strlen(pSpecialTariff->m_sNBR))==0){
						iCharge = pSpecialTariff->m_iCharge;
						return true;
					}
				}
				pSpecialTariff = pSpecialTariff->m_poNext;
		}
	}
	return false;
}
#else
bool SpecialTariffMgr::getFeeByInfo (EventSection * oEvent,int &iCharge)
{
	if(!bInit){
		initParam();
		bInit = true;
	}
	char sTemp[40];
	SpecialTariff * pSpecialTariff = 0;
	SpecialTariff oTemp;
	getAttrValue(oEvent,oTemp);
	
	sprintf(sTemp,"%d_%s_%d",oTemp.m_iUpDownFlag,oTemp.m_sSPFLAG,m_iType);
	unsigned int i = 0;
	if(G_PPARAMINFO->m_poSpecialTariffIndex->get(sTemp,&i)){
		if(i) pSpecialTariff = &(G_PPARAMINFO->m_poSpecialTariffList[i]);
		while(pSpecialTariff){
			if((pSpecialTariff->m_sServiceNBR[0]=='*' 
				|| strcmp(pSpecialTariff->m_sServiceNBR,oTemp.m_sServiceNBR)==0) 
				&& strcmp(oEvent->m_sStartDate,pSpecialTariff->m_sEffDate)>=0 
				&& strcmp(oEvent->m_sStartDate,pSpecialTariff->m_sExpDate)<0 ){
					if(pSpecialTariff->m_sNBR[0]=='*' 
						|| strncmp(oTemp.m_sNBR,pSpecialTariff->m_sNBR,strlen(pSpecialTariff->m_sNBR))==0){
						iCharge = pSpecialTariff->m_iCharge;
						return true;
					}
				}
				if(pSpecialTariff->m_iNext)
					 pSpecialTariff = &(G_PPARAMINFO->m_poSpecialTariffList[pSpecialTariff->m_iNext]);
				else pSpecialTariff = 0;
		}
	}
	return false;
}
#endif
//检查在通信费中是否匹配到
#ifdef PRIVATE_MEMORY
bool SpecialTariffMgr::checkFeeExist(SpecialTariff * poTariff)
{
	char sTemp[40];
	SpecialTariff * pSpecialTariff;

	sprintf(sTemp,"%d_%s_%d",poTariff->m_iUpDownFlag,poTariff->m_sSPFLAG,m_iType);
	if(m_poSpecialTariffIndex->get(sTemp,&pSpecialTariff)){
		while(pSpecialTariff){
			if((pSpecialTariff->m_sServiceNBR[0]=='*' 
				|| strcmp(pSpecialTariff->m_sServiceNBR,poTariff->m_sServiceNBR)==0) 
				&& strcmp(poTariff->m_sEffDate,pSpecialTariff->m_sEffDate)>=0 
				&& strcmp(poTariff->m_sEffDate,pSpecialTariff->m_sExpDate)<0 ){
					if(pSpecialTariff->m_sNBR[0]=='*' 
						|| strncmp(poTariff->m_sNBR,pSpecialTariff->m_sNBR,strlen(pSpecialTariff->m_sNBR))==0){
						return true;
					}
				}
				pSpecialTariff = pSpecialTariff->m_poNext;
		}
	}
	return false;	
}
#else
bool SpecialTariffMgr::checkFeeExist(SpecialTariff * poTariff)
{
	char sTemp[40];
	SpecialTariff * pSpecialTariff;

	sprintf(sTemp,"%d_%s_%d",poTariff->m_iUpDownFlag,poTariff->m_sSPFLAG,m_iType);
	unsigned int i = 0;
	if(G_PPARAMINFO->m_poSpecialTariffIndex->get(sTemp,&i)){
		pSpecialTariff = &(G_PPARAMINFO->m_poSpecialTariffList[i]);
		while(pSpecialTariff){
			if((pSpecialTariff->m_sServiceNBR[0]=='*' 
				|| strcmp(pSpecialTariff->m_sServiceNBR,poTariff->m_sServiceNBR)==0) 
				&& strcmp(poTariff->m_sEffDate,pSpecialTariff->m_sEffDate)>=0 
				&& strcmp(poTariff->m_sEffDate,pSpecialTariff->m_sExpDate)<0 ){
					if(pSpecialTariff->m_sNBR[0]=='*' 
						|| strncmp(poTariff->m_sNBR,pSpecialTariff->m_sNBR,strlen(pSpecialTariff->m_sNBR))==0){
						return true;
					}
				}
				if(pSpecialTariff->m_iNext)
					 pSpecialTariff = &(G_PPARAMINFO->m_poSpecialTariffList[pSpecialTariff->m_iNext]);
				else pSpecialTariff = 0;
		}
	}
	return false;	
}
#endif

//检查费率是否有效
#ifdef PRIVATE_MEMORY
bool SpecialTariffMgr::checkFeeValid (EventSection * oEvent,long lCharge)
{
	if(!bInit){
		initParam();
		load();	
		bInit = true;
	}
	char sTemp[40];
	bool bRet = false;
	SpecialTariff * pSpecialTariff;
	SpecialTariff oTemp;
	getAttrValue(oEvent,oTemp);

	strcpy(oTemp.m_sEffDate,oEvent->m_sStartDate);//置事件开始时间
	if(!checkFeeExist(&oTemp)){
		return bRet;
	}
	bRet = true;
	sprintf(sTemp,"%d_%s",oTemp.m_iUpDownFlag,oTemp.m_sSPFLAG);
	if(m_poSpecialTariffCheckIndex->get(sTemp,&pSpecialTariff)){
		while(pSpecialTariff){
			if((pSpecialTariff->m_sServiceNBR[0]=='*' 
				|| strcmp(pSpecialTariff->m_sServiceNBR,oTemp.m_sServiceNBR)==0) 
				&& strcmp(oTemp.m_sEffDate,pSpecialTariff->m_sEffDate)>=0 
				&& strcmp(oTemp.m_sEffDate,pSpecialTariff->m_sExpDate)<0 ){
					if(pSpecialTariff->m_sNBR[0]=='*' 
						|| strncmp(oTemp.m_sNBR,pSpecialTariff->m_sNBR,strlen(pSpecialTariff->m_sNBR))==0){
						//getChargeValue(oEvent);
						if(!(lCharge  <= pSpecialTariff->m_iCharge)){
							bRet = false;
						}
					}
				}
				pSpecialTariff = pSpecialTariff->m_poNext;
		}
	}
	return bRet;
}
#else
bool SpecialTariffMgr::checkFeeValid (EventSection * oEvent,long lCharge)
{
	if(!bInit){
		initParam();
		bInit = true;
	}
	char sTemp[40];
	bool bRet = false;
	SpecialTariff * pSpecialTariff;
	SpecialTariff oTemp;
	getAttrValue(oEvent,oTemp);

	strcpy(oTemp.m_sEffDate,oEvent->m_sStartDate);//置事件开始时间
	if(!checkFeeExist(&oTemp)){
		return bRet;
	}
	bRet = true;
	sprintf(sTemp,"%d_%s",oTemp.m_iUpDownFlag,oTemp.m_sSPFLAG);
	unsigned int i = 0;
	if(G_PPARAMINFO->m_poSpecialTariffCheckIndex->get(sTemp,&i)){
		pSpecialTariff = &(G_PPARAMINFO->m_poSpecialTariffCheckList[i]);
		while(pSpecialTariff){
			if((pSpecialTariff->m_sServiceNBR[0]=='*' 
				|| strcmp(pSpecialTariff->m_sServiceNBR,oTemp.m_sServiceNBR)==0) 
				&& strcmp(oTemp.m_sEffDate,pSpecialTariff->m_sEffDate)>=0 
				&& strcmp(oTemp.m_sEffDate,pSpecialTariff->m_sExpDate)<0 ){
					if(pSpecialTariff->m_sNBR[0]=='*' 
						|| strncmp(oTemp.m_sNBR,pSpecialTariff->m_sNBR,strlen(pSpecialTariff->m_sNBR))==0){
						//getChargeValue(oEvent);
						if(!(lCharge  <= pSpecialTariff->m_iCharge)){
							bRet = false;
						}
					}
				}
				if(pSpecialTariff->m_iNext)
					 pSpecialTariff = &(G_PPARAMINFO->m_poSpecialTariffCheckList[pSpecialTariff->m_iNext]);
				else pSpecialTariff = 0;
		}
	}
	return bRet;
}
#endif

//检查是否是测试话单
#ifdef PRIVATE_MEMORY
bool SpecialTariffMgr::checkIsTestCdr (EventSection * oEvent)
{
	if(!bInit){
		initParam();
		load();	
		bInit = true;
	}
	char sTemp[40];
	bool bRet = true;
	SpecialTariff * pSpecialTariff;
	SpecialTariff oTemp;
	getAttrValue(oEvent,oTemp);

	strcpy(oTemp.m_sEffDate,oEvent->m_sStartDate);//置事件开始时间
	if(!checkFeeExist(&oTemp)){//费率不存在就算测试话单
		return bRet;
	}
	bRet = false;
	sprintf(sTemp,"%d_%s",oTemp.m_iUpDownFlag,oTemp.m_sSPFLAG);
	if(m_poSpecialTariffCheckIndex->get(sTemp,&pSpecialTariff)){
		while(pSpecialTariff){
			if(strcmp(oTemp.m_sEffDate,pSpecialTariff->m_sEffDate)>=0 
				&& strcmp(oTemp.m_sEffDate,pSpecialTariff->m_sExpDate)<0 
				&& (pSpecialTariff->m_sNBR[0]=='*' 
				|| strncmp(oTemp.m_sNBR,pSpecialTariff->m_sNBR,strlen(pSpecialTariff->m_sNBR))==0)){
					if(pSpecialTariff->m_sServiceNBR[0]=='*' 
						|| strcmp(pSpecialTariff->m_sServiceNBR,oTemp.m_sServiceNBR)==0){
						bRet = false;
						return bRet;
					}else{
						bRet =  true;
					}
				}
				pSpecialTariff = pSpecialTariff->m_poNext;
		}
	}
	return bRet;
}
#else
bool SpecialTariffMgr::checkIsTestCdr (EventSection * oEvent)
{
	if(!bInit){
		initParam();
		bInit = true;
	}
	char sTemp[40];
	bool bRet = true;
	SpecialTariff * pSpecialTariff;
	SpecialTariff oTemp;
	getAttrValue(oEvent,oTemp);

	strcpy(oTemp.m_sEffDate,oEvent->m_sStartDate);//置事件开始时间
	if(!checkFeeExist(&oTemp)){//费率不存在就算测试话单
		return bRet;
	}
	bRet = false;
	sprintf(sTemp,"%d_%s",oTemp.m_iUpDownFlag,oTemp.m_sSPFLAG);
	unsigned int i = 0;
	if(G_PPARAMINFO->m_poSpecialTariffCheckIndex->get(sTemp,&i)){
		if(i) pSpecialTariff = &(G_PPARAMINFO->m_poSpecialTariffCheckList[i]);
		while(pSpecialTariff){
			if(strcmp(oTemp.m_sEffDate,pSpecialTariff->m_sEffDate)>=0 
				&& strcmp(oTemp.m_sEffDate,pSpecialTariff->m_sExpDate)<0 
				&& (pSpecialTariff->m_sNBR[0]=='*' 
				|| strncmp(oTemp.m_sNBR,pSpecialTariff->m_sNBR,strlen(pSpecialTariff->m_sNBR))==0)){
					if(pSpecialTariff->m_sServiceNBR[0]=='*' 
						|| strcmp(pSpecialTariff->m_sServiceNBR,oTemp.m_sServiceNBR)==0){
						bRet = false;
						return bRet;
					}else{
						bRet =  true;
					}
				}
				pSpecialTariff = pSpecialTariff->m_poNext;
		}
	}
	return bRet;
}
#endif

bool  SpecialTariffMgr::load ()
{
	int iCount = 0,m=0;
	char sTemp[40];
	SpecialTariff * pSpecialTariffTemp;
	SpecialTariff * pTemp;

	TOCIQuery qry(Environment::getDBConn());
	qry.setSQL("select count(1) from b_js_sp_special_process where special_type = 1");
	qry.open();
	if(qry.next()){
		iCount = qry.field(0).asInteger();
	}
	qry.close();
	qry.commit();

	iCount = iCount+1;
	m_poSpecialTariffData = new SpecialTariff[iCount];
	m_poSpecialTariffIndex = new HashList<SpecialTariff *>(iCount);
	if(!m_poSpecialTariffData || !m_poSpecialTariffIndex){
		 THROW(MBC_SpecialTariffMgr+1);
	}
    qry.setSQL("select up_down_flag,sp_nbr,in_nbr,service_nbr,billing_type,"
		"nvl(eff_date,to_date('20000501','yyyymmdd')) eff_date,"
		"nvl(exp_date,to_date('30000501','yyyymmdd')) exp_date,"
		"tariff_rate from b_js_sp_special_process a,"
		"b_js_sp_tariff_cfg b where a.special_type = 1 and a.tariff_id=b.tariff_id "
		" ORDER BY up_down_flag,sp_nbr,billing_type,service_nbr,in_nbr asc");
    qry.open();
	while(qry.next())
	{
		pSpecialTariffTemp = m_poSpecialTariffData+m;
		pSpecialTariffTemp->m_iUpDownFlag = qry.field(0).asInteger();
		strcpy(pSpecialTariffTemp->m_sSPFLAG,qry.field(1).asString());
		strcpy(pSpecialTariffTemp->m_sNBR,qry.field(2).asString());
		strcpy(pSpecialTariffTemp->m_sServiceNBR,qry.field(3).asString());
		pSpecialTariffTemp->m_iBillingType = qry.field(4).asInteger();
		strcpy(pSpecialTariffTemp->m_sEffDate,qry.field(5).asString());
		strcpy(pSpecialTariffTemp->m_sExpDate,qry.field(6).asString());
		pSpecialTariffTemp->m_iCharge = qry.field(7).asInteger();
		pSpecialTariffTemp->m_poNext = 0;
		sprintf(sTemp,"%d_%s_%d",pSpecialTariffTemp->m_iUpDownFlag,
			pSpecialTariffTemp->m_sSPFLAG,pSpecialTariffTemp->m_iBillingType);
		if(m_poSpecialTariffIndex->get(sTemp,&pTemp)){
			pSpecialTariffTemp->m_poNext = pTemp;
		}
		m_poSpecialTariffIndex->add(sTemp,pSpecialTariffTemp);
		m++;
	}
	qry.close();
	qry.commit();

	iCount = 0;
	m = 0;
	qry.setSQL("select count(1) from b_js_sp_special_process where special_type = 2");
	qry.open();
	if(qry.next()){
		iCount = qry.field(0).asInteger();
	}
	qry.close();
	qry.commit();

	iCount = iCount+1;
	m_poSpecialTariffCheckData = new SpecialTariff[iCount];
	m_poSpecialTariffCheckIndex = new HashList<SpecialTariff *>(iCount);
    if(!m_poSpecialTariffCheckData || !m_poSpecialTariffCheckIndex){
		 THROW(MBC_SpecialTariffMgr+1);
	}
    qry.setSQL("select up_down_flag,sp_nbr,in_nbr,service_nbr,"
		"nvl(eff_date,to_date('20000501','yyyymmdd')) eff_date,"
		"nvl(exp_date,to_date('30000501','yyyymmdd')) exp_date,"
		"tariff_rate from b_js_sp_special_process a,b_js_sp_tariff_cfg b "
		" where a.special_type = 2 and a.tariff_id=b.tariff_id "
		" ORDER BY up_down_flag,sp_nbr,service_nbr,in_nbr ASC");
    qry.open();
	while(qry.next())
	{
		pSpecialTariffTemp = m_poSpecialTariffCheckData+m;
		pSpecialTariffTemp->m_iUpDownFlag = qry.field(0).asInteger();
		strcpy(pSpecialTariffTemp->m_sSPFLAG,qry.field(1).asString());
		strcpy(pSpecialTariffTemp->m_sNBR,qry.field(2).asString());
		strcpy(pSpecialTariffTemp->m_sServiceNBR,qry.field(3).asString());
		strcpy(pSpecialTariffTemp->m_sEffDate,qry.field(4).asString());
		strcpy(pSpecialTariffTemp->m_sExpDate,qry.field(5).asString());
		pSpecialTariffTemp->m_iCharge = qry.field(6).asInteger();
		pSpecialTariffTemp->m_poNext = 0;
		sprintf(sTemp,"%d_%s",pSpecialTariffTemp->m_iUpDownFlag,
			pSpecialTariffTemp->m_sSPFLAG);
		if(m_poSpecialTariffCheckIndex->get(sTemp,&pTemp)){
			pSpecialTariffTemp->m_poNext = pTemp;
		}
		m_poSpecialTariffCheckIndex->add(sTemp,pSpecialTariffTemp);
		m++;
	}
	qry.close();
	qry.commit();
	return true;
}
