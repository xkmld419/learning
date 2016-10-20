/*VER: 1*/ 
# include "ReloadUserInfo.h"

# include <string.h>
#include "Process.h"
#include "ReadIni.h"

ReloadUserInfo::ReloadUserInfo()
{
	ReadIni reader;
	char sUser[32];
	char sPwd[32];
	char sStr[32];
	char sFile[254];

	if (Process::m_sTIBS_HOME[strlen(Process::m_sTIBS_HOME)-1] == '/') {
		sprintf (sFile, "%setc/data_access.ini", Process::m_sTIBS_HOME);
	} else {
		sprintf (sFile, "%s/etc/data_access.ini", Process::m_sTIBS_HOME);
	}

	reader.readIniString (sFile, "USER_INFO", "username", sUser, "");
	reader.readIniString (sFile, "USER_INFO", "password", sPwd, "");
	reader.readIniString (sFile, "USER_INFO", "connstr", sStr, "");

	try {
		DB_LINK->disconnect ();
		Environment::setDBLogin (sUser, sPwd, sStr);
		Environment::getDBConn (true);
	} catch (...) {
	}
}

void ReloadUserInfo::reloadServ()
{
	m_poServData->remove ();
	m_poServIndex->remove ();

	loadServInfo ();
	bindData ();

	reloadServLocation ();
	reloadServState ();
	reloadServAttr ();
	reloadServIdent ();
	reloadServGroup ();
	reloadServProduct ();
	reloadServAcct ();
	reloadOfferInstance ();
}

void ReloadUserInfo::reloadServProduct()
{
	int iTemp, jTemp;

	jTemp = m_poServData->getCount ();
	ServInfo * pServ = *m_poServData;

	for (iTemp=1; iTemp<=jTemp; iTemp++) {
		pServ[iTemp].m_iServProductOffset = 0;
	}

	m_poServProductData->remove ();
	m_poServProductIndex->remove ();
	m_poServProductAttrData->remove ();

	loadServProductInfo ();
	loadServProductAttrInfo ();
}

void ReloadUserInfo::reloadServAcct()
{
	m_poServAcctData->remove ();
	m_poServAcctIndex->remove ();

	loadServAcctInfo ();

	bindData ();
}

void ReloadUserInfo::reloadServLocation()
{
	m_poServLocationData->remove ();
	loadServLocationInfo ();

	bindData ();
}

void ReloadUserInfo::reloadServIdent()
{
	m_poServIdentData->remove ();
	m_poServIdentIndex->remove ();

	loadServIdentInfo ();

	bindData ();
}

void ReloadUserInfo::reloadServAttr()
{
	m_poServAttrData->remove ();
	loadServAttrInfo ();

	bindData ();
}

void ReloadUserInfo::reloadOfferInstance()
{
	m_poOfferInstanceData->remove ();
	m_poOfferInstanceIndex->remove ();

	loadOfferInstanceInfo ();

	bindData ();
}

void ReloadUserInfo::reloadServGroup()
{
	m_poServGroupBaseData->remove ();
	m_poServGroupBaseIndex->remove ();

	m_poServGroupData->remove ();
	loadServGroupBaseInfo ();
	loadServGroupInfo ();

	bindData ();
}

void ReloadUserInfo::reloadCustPlan()
{
	m_poCustPricingPlanData->remove ();
	m_poCustPricingPlanIndex->remove ();
	m_poCustPricingParamData->remove ();
	m_poCustPricingObjectData->remove ();

	ServInfo * pServ = (ServInfo *)(*m_poServData);
	CustInfo * pCust = (CustInfo *)(*m_poCustData);

	unsigned int iTemp;
	unsigned int jTemp = m_poServData->getCount ();
	unsigned int mTemp = m_poCustData->getCount ();
	unsigned int kTemp;

	for (iTemp=1; iTemp<=mTemp; iTemp++) {
		pCust[iTemp].m_iCustPlanOffset = 0;
	}

	loadCustPricingPlanInfo ();
	loadCustPricingParamInfo ();
	loadCustPricingObjectInfo ();
/*

	for (iTemp=1; iTemp<=jTemp; iTemp++) {
		if (m_poCustIndex->get (pServ[iTemp].m_lCustID, &kTemp)) {
			pServ[iTemp].m_iCustPlanOffset = pCust[kTemp].m_iCustPlanOffset;
		} else {
			cout << "NO CUSTID:" << pServ[iTemp].m_lServID << endl;
		}
	}
*/


	bindData ();
}

void ReloadUserInfo::reloadServState()
{
	m_poServStateData->remove ();
	loadServStateInfo ();

	DEFINE_QUERY (qry);
	qry.setSQL ("select serv_id, state, to_char(state_date, 'yyyymmddhh24miss') from serv");
	qry.open ();

	long lServID;
	unsigned int k;
	ServInfo * pServ = (ServInfo *)(*m_poServData);

	while (qry.next ()) {
		lServID = qry.field(0).asLong ();

		if (m_poServIndex->get (lServID, &k)) {
			strcpy (pServ[k].m_sState, qry.field(1).asString ());
			strcpy (pServ[k].m_sStateDate, qry.field(2).asString ());
		}
	}

	qry.close ();

	bindData ();
}

void ReloadUserInfoMgr::prepare()
{
	int iTemp;
	char *sTemp;

	if (g_argc==1) {
		cout << "reloaduserinfo [servstate|ss] [custplan|cp]" << endl;
	}
	
	for (iTemp=1; iTemp<g_argc; iTemp++) {
		sTemp = g_argv[iTemp];

		if (!strcmp (sTemp, "servstate") || !strcmp (sTemp, "ss")) {
			m_bServState = true;
		} else if (!strcmp (sTemp, "custplan") || !strcmp (sTemp, "cp")) {
			m_bCustPlan = true;
		} else if (!strcmp (sTemp, "servgroup") || !strcmp (sTemp, "sg")) {
			m_bServGroup = true;
		} else if (!strcmp (sTemp, "offerinstance") || !strcmp (sTemp, "oi")) {
			m_bOfferInstance = true;
		} else if (!strcmp (sTemp, "servattr") || !strcmp (sTemp, "sar")) {
			m_bServAttr = true;
		} else if (!strcmp (sTemp, "servident") || !strcmp (sTemp, "si")) {
			m_bServIdent = true;
		} else if (!strcmp (sTemp, "servlocation") || !strcmp (sTemp, "sl")) {
			m_bServLocation = true;
		} else if (!strcmp (sTemp, "servacct") || !strcmp (sTemp, "sat")) {
			m_bServAcct = true;
		} else if (!strcmp (sTemp, "serv") ) {
			m_bServ = true;
		} else if (!strcmp (sTemp, "servproduct") || !strcmp (sTemp, "sp")) {
			m_bServProduct = true;
		} else {
			cout << "未知的参数:" << sTemp << endl;
		}

	}


	if (m_bServ) {
		m_bServGroup = false;
		m_bOfferInstance = false;
		m_bServAttr = false;
		m_bServLocation = false;
		m_bServIdent = false;
		m_bServAcct = false;
		m_bServProduct = false;
		m_bServState = false;
	}
}

ReloadUserInfoMgr::ReloadUserInfoMgr()
{
	m_bServState = false;
	m_bCustPlan = false;
	m_bServGroup = false;
	m_bOfferInstance = false;
	m_bServAttr = false;
	m_bServLocation = false;
	m_bServIdent = false;
	m_bServAcct = false;
	m_bServProduct = false;
	m_bServ = false;

}

int ReloadUserInfoMgr::run()
{
	ReloadUserInfo * pReload = new ReloadUserInfo ();

	prepare ();

	if (m_bServState) {
		cout << "刷新主产品实例状态数据..." << endl;
		pReload->reloadServState ();
	}

	if (m_bCustPlan) {
		cout << "刷新客户个性化定价信息数据..." << endl;
		pReload->reloadCustPlan ();
	}

	if (m_bServGroup) {
		cout << "刷新用户群数据..." << endl;
		pReload->reloadServGroup ();
	}

	if (m_bOfferInstance) {
		cout << "刷新商品明细实例数据..." << endl;
		pReload->reloadOfferInstance ();
	}

	if (m_bServAttr) {
		cout << "刷新主产品实例属性数据..." << endl;
		pReload->reloadServAttr ();
	}

	if (m_bServLocation) {
		cout << "刷新主产品实例地域数据..." << endl;
		pReload->reloadServLocation ();
	}

	if (m_bServIdent) {
		cout << "刷新主产品实例标识数据..." << endl;
		pReload->reloadServIdent ();
	}

	if (m_bServAcct) {
		cout << "刷新支付关系数据..." << endl;
		pReload->reloadServAcct ();
	}

	if (m_bServProduct) {
		cout << "刷新附属产品数据..." << endl;
		pReload->reloadServProduct ();
	}

	if (m_bServ) {
		cout << "刷新用户数据..." << endl;
		pReload->reloadServ ();
	}

	delete pReload;

	return 0;
}

DEFINE_MAIN (ReloadUserInfoMgr)

