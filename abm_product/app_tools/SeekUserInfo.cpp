/*VER: 5*/ 
#include "Process.h"
#include <iostream>
#include "UserInfoReader.h"
#include "Log.h"
#include "TransactionMgr.h"
#include "StdEvent.h"
#ifdef GROUP_CONTROL
#include "FormatMethod.h"
#include "UserInfo.h"
#endif
using namespace std;

class SeekUserInfo 
{
  public:
	int run();

  public:
	SeekUserInfo();
	//add by xn 2009.11.13 for np
	int seekNpInfo(char* sOut, char* accNbr, char* sDate);

};

int main(int argc, char *argv[])
{
	g_argc = argc;
	g_argv = argv;

	SeekUserInfo x;

	return x.run ();
}

SeekUserInfo::SeekUserInfo()
{
//	Environment::useUserInfo(USER_INFO_BASE);
    Environment::useUserInfo(USRE_INFO_NOITEMGROUP);

	if (!G_PTRANSMGR) {
		G_PTRANSMGR = new TransactionMgr ();
	}

	if (!G_PTRANSMGR) {
		THROW (10);
	}
}
int SeekUserInfo::seekNpInfo(char* sOut,char* accNbr, char* sDate)
{
	NpInfo npInfo;
	char sTemp[128];
	if ( G_PUSERINFO->getNpInfo(npInfo,accNbr,sDate) )
	{
		strcat (sOut, "\n携号转网信息:\n");
		sprintf(sTemp,
				"\t\t号码:%s\n"
				"\t\t原始网络:%d\n"
				"\t\t现在网络:%d\n"
				"\t\t携号状态:%d\n"
				"\t\t生效时间:%s\n"
				"\t\t失效时间:%s\n",
				npInfo.m_sAccNbr,
				npInfo.m_iOrgNetwork,
				npInfo.m_iNetwork,
				npInfo.m_iNpState,
				npInfo.m_sEffDate,
				npInfo.m_sExpDate);
			strcat (sOut, sTemp);
		return 1;
	}
	return 0;
}
int SeekUserInfo::run()
{
	if (g_argc < 2) {
		Log::log (0, "[ERROR] seekuser SERV_ID|ACC_NBR [DATE]");
#ifdef GROUP_CONTROL
		Log::log (0, "[ERROR] seekuser -c|-g ACC_NBR [DATE]");
		Log::log (0, "[ERROR] seekuser -e ACC_NBR [DATE]");
#endif
#ifdef	IMSI_MDN_RELATION
		Log::log (0, "[ERROR] seekuser -i IMSI [DATE]");
#endif

		return 0;
	}
	char sDate[16];
	char sOut[10240];
	char sTemp[128];
	memset (sOut, 0, sizeof (sOut));
	memset (sTemp, 0, sizeof (sTemp));

#ifdef GROUP_CONTROL

	if(strcmp(g_argv[1],"-e")==0 ||
		strcmp(g_argv[1],"-E")==0)
	{

		int iRet;
		bool bRet;
		if(g_argc < 3)
		{
			Log::log (0, "[ERROR] seekuser -e ACC_NBR [DATE]");
			return 0;
		}
		if(g_argc > 3)
		{
			Date d(g_argv[3]);
			strcpy (sDate, d.toString ());
		}else{
			Date d;
			strcpy (sDate, d.toString ());
		}
		char * pTemp1 = g_argv[2];
		AccNbrOfferType oAccNbrOfferType;
		if(!G_PUSERINFO->getAccNbrOfferType(pTemp1,sDate,oAccNbrOfferType))
		{
			sprintf (sOut, "\n=========查询失败,很失败,^@^=========\n");
		}
		else
		{
			sprintf (sOut, "\n=========查询号码的信息如下=========\n");
			sprintf (sTemp,
				"号    码:%s\n"
				"生效时间:%s\n"
				"生效时间:%s\n"
				"商品类型:%d\n",
				oAccNbrOfferType.m_sAccNbr,oAccNbrOfferType.m_sEffDate,
				oAccNbrOfferType.m_sExpDate,oAccNbrOfferType.m_iOfferType);
			strcat (sOut, sTemp);
		}
		cout << sOut << endl << endl;
		return 0;
	}
	if(strcmp(g_argv[1],"-c")==0 ||
		strcmp(g_argv[1],"-C")==0 ||
		strcmp(g_argv[1],"-g")==0 ||
		strcmp(g_argv[1],"-G")==0)
	{
		int iRet;
		bool bRet;
		if(g_argc < 3)
		{
			Log::log (0, "[ERROR] seekuser -c|-g ACC_NBR [DATE]");
			return 0;
		}
		if(g_argc > 3)
		{
			Date d(g_argv[3]);
			strcpy (sDate, d.toString ());
		}else{
			Date d;
			strcpy (sDate, d.toString ());
		}
		char * pTemp1 = g_argv[2];
		GroupInfo groupInfo;
		NumberGroup numberInfo[GROUP_NUM];
		GroupMember memberInfo[GROUP_NUM];
		switch (g_argv[1][1]|0x20)
		{
		case 'c':
			iRet = G_PUSERINFO->getNumberGroupByAccNbr(numberInfo,pTemp1,sDate);
			if(iRet>0)
			{
				sprintf (sOut, "\n=========查询号码的信息如下=========\n");
				for(int i=0;i<iRet ;i++)
				{
					bRet = G_PUSERINFO->getGroupInfoByID(groupInfo,numberInfo[i].m_iGroupID,sDate);
					sprintf (sTemp,
					"关系  ID:%ld\n"
					"号    码:%s\n"
					"群组  ID:%d\n"
					"生效日期:%s\n"
					"失效日期:%s\n"
					"商品实例:%ld\n",
					numberInfo[i].m_lRelationID,numberInfo[i].m_sAccNbr,
					numberInfo[i].m_iGroupID,numberInfo[i].m_sEffDate,
					numberInfo[i].m_sExpDate,numberInfo[i].m_lOfferInstID);
					strcat (sOut, sTemp);
					if(bRet)
					{
						sprintf (sTemp,
						"\t群组名称:%s\n"
						"\t群组描述:%s\n"
						"\t条件  ID:%d\n"
						"\t策略  ID:%d\n"
						"\t生效日期:%s\n"
						"\t失效日期:%s\n"
						"\t优  先级:%d\n"
						"\t归属 ORG:%d\n",
						groupInfo.m_sGroupName,groupInfo.m_sGroupDesc,
						groupInfo.m_iConditionID,groupInfo.m_iStrategyID,
						groupInfo.m_sEffDate,groupInfo.m_sExpDate,
						groupInfo.m_iPriority,groupInfo.m_iOrgID);
						strcat (sOut, sTemp);
					}
				}
			}else
			{
				Log::log (0, "[SORRY] 没有找到号码[%s]的订购群信息", pTemp1);
				return 0;
			}
			break;
		case 'g':
			//把号码拆分成区号和号码
			char sAccNbr[33];
			char sAreaCode[33];
			char sFullAccNbr[66];
			strncpy(sAccNbr,pTemp1,sizeof(sAccNbr)-1);
			strcpy(sFullAccNbr,sAccNbr);
			int iLongGroup;
			if(FormatMethod::interceptAreaCodeCycle(sAccNbr, sAreaCode,iLongGroup))
			{
				//进行专网计费号码查找
				PNSegBilling oPNSegBilling;
				if(FormatMethod::getPNSegBilling(
						sAccNbr,
						sAreaCode,
						sDate,
						oPNSegBilling))
				{
					sprintf(sFullAccNbr,"%s%s",sAreaCode,oPNSegBilling.m_sAccNBR);
				}
			}
			iRet = G_PUSERINFO->getGroupMemberByAccNbr(memberInfo,sFullAccNbr,sDate);
			if(iRet>0)
			{
				sprintf (sOut, "\n=========查询号码的信息如下=========\n");
				for(int i=0;i<iRet ;i++)
				{
					bRet = G_PUSERINFO->getGroupInfoByID(groupInfo,memberInfo[i].m_iGroupID,sDate);
					sprintf (sTemp,
					"关系  ID:%ld\n"
					"号    码:%s\n"
					"群组  ID:%d\n"
					"生效日期:%s\n"
					"失效日期:%s\n",
					memberInfo[i].m_lMemberID,memberInfo[i].m_sAccNbr,
					memberInfo[i].m_iGroupID,memberInfo[i].m_sEffDate,
					memberInfo[i].m_sExpDate);
					strcat (sOut, sTemp);
					if(bRet)
					{
						sprintf (sTemp,
						"\t群组名称:%s\n"
						"\t群组描述:%s\n"
						"\t条件  ID:%d\n"
						"\t策略  ID:%d\n"
						"\t生效日期:%s\n"
						"\t失效日期:%s\n"
						"\t优  先级:%d\n"
						"\t归属 ORG:%d\n",
						groupInfo.m_sGroupName,groupInfo.m_sGroupDesc,
						groupInfo.m_iConditionID,groupInfo.m_iStrategyID,
						groupInfo.m_sEffDate,groupInfo.m_sExpDate,
						groupInfo.m_iPriority,groupInfo.m_iOrgID);
						strcat (sOut, sTemp);
					}
				}
			}else
			{
				Log::log (0, "[SORRY] 没有找到号码[%s]的归属群信息", pTemp1);
				return 0;
			}
			break;
		default:
			break;
			return 0;
		}
		cout << sOut << endl << endl;
		return 0;
	}
#endif

#ifdef	IMSI_MDN_RELATION
	if ((g_argv[1][1] | 0x20)=='i'){
		if (g_argc<3){
			Log::log (0, "[ERROR] seekuser -i IMSI [DATE]");
			return 0;
		}else if (g_argc==3){	
			G_PUSERINFO->showImsiMdnRelationByImsi(g_argv[2]);
		}else{
			G_PUSERINFO->showImsiMdnRelationByImsi(g_argv[2], g_argv[3]);
		}
#ifdef DEBUG
		G_PUSERINFO->showAllImsiMdnRelation();
#endif
		return 1;
	}
#endif

	char * pTemp1 = g_argv[1];

	if (g_argc > 2) {
		Date d(g_argv[2]);
		strcpy (sDate, d.toString ());
	} else {
		Date d;
		strcpy (sDate, d.toString ());
	}

	if (pTemp1[0] == '0') {
		if (!G_PUSERINFO->getServ (G_SERV, pTemp1, sDate)) {
			Log::log (0, "[SORRY] 没有找到号码%s在%s对应的用户资料", pTemp1, sDate);
			if(seekNpInfo(sOut,pTemp1,sDate))//add by xn 2009.11.13 for np
				cout << sOut << endl;
			return 0;
		}

	} else {
		if (!G_PUSERINFO->getServ (G_SERV, strtol (pTemp1, 0, 10), sDate)) {
			Log::log (0, "[SORRY] 没有找到主产品标识%s的用户资料", pTemp1);

			if (!G_PUSERINFO->getServ (G_SERV, pTemp1, sDate)) {
				Log::log (0, "[SORRY] 没有找到号码%s在%s对应的用户资料", pTemp1, sDate);
				if(seekNpInfo(sOut,pTemp1,sDate))//add by xn 2009.11.13 for np
					cout << sOut << endl;
				return 0;
			}
		} 
	}	

	sprintf (sOut, "\n=========查询用户的资料信息如下=========\n"
			"主产品标识:%ld\n"
			"归属局向  :%d\n"
			"创建日期  :%s\n"
			"起租日期  :%s\n"
			"竣工日期  :%s\n" 
			"帐期类型  :%d\n"
			"所属客户  :%ld[类型:%s]\n"
			"状    态  :%s\n"
			"状态日期  :%s\n"
			"产品标识  :%d\n"
			"计费模式  :%d\n"
			"订购商品  :\n",
			G_PSERV->getServID (), G_PSERV->getOrgID (), 
            G_PSERV->getCreateDate (), G_PSERV->getRentDate (), 
            G_PSERV->getNscDate (), G_PSERV->getBillingCycleType (),
			G_PSERV->getCustID (), G_PSERV->getCustType (), 
            G_PSERV->getState (), G_PSERV->getStateDate (),
			G_PSERV->getProductID (), G_PSERV->getServInfo ()->m_iBillingMode);

	int iTemp, jTemp;
    OfferInsQO          poOfferInstBuf[MAX_OFFER_INSTANCE_NUMBER];
	ProdOfferInsAttr	ret[MAX_OFFER_ATTRS];
    iTemp = G_PSERV->getOfferIns (poOfferInstBuf);
	
	for (jTemp=0; jTemp<iTemp; jTemp++) {
        //if (jTemp == 0) {
            sprintf (sTemp,
				"\t商品ID:%d\n"
				"\t实例ID:%ld\n"
				"\t生效时间:%s\n"
				"\t失效时间:%s\n",
            poOfferInstBuf[jTemp].m_poOfferIns->m_iOfferID,
			poOfferInstBuf[jTemp].m_poOfferIns->m_lProdOfferInsID,
			poOfferInstBuf[jTemp].m_poOfferIns->m_sEffDate,
			poOfferInstBuf[jTemp].m_poOfferIns->m_sExpDate);
        /*} else {
            sprintf (sTemp, ",%d[%ld]", 
                        poOfferInstBuf[jTemp].m_poOfferIns->m_iOfferID,
			poOfferInstBuf[jTemp].m_poOfferIns->m_lProdOfferInsID);
        }*/
		strcat (sOut, sTemp);
		int iResult = poOfferInstBuf[jTemp].getOfferInsAttrAndDate(ret,sDate);
		if(iResult){
			strcat(sOut,"\t商品的属性:\n");
		}
		for( int m=0;m<iResult;m++)
		{
			sprintf(sTemp,
				"\t\t属性ID:%d\n"
				"\t\t属性值:%s\n"
				"\t\t生效时间:%s\n"
				"\t\t失效时间:%s\n",
				ret[m].m_iAttrID,
				ret[m].m_sAttrValue,
				ret[m].m_sEffDate,
				ret[m].m_sExpDate);
			strcat (sOut, sTemp);
		}
	}

	vector<UserInfoAttr> attrs;
	vector<UserInfoAttr>::iterator iter;

	attrs.clear ();
    G_PSERV->getAttrs(attrs);
    if (attrs.size ()) {
        strcat (sOut, "\n产品的属性:\n");
        for (iter=attrs.begin (); iter!=attrs.end (); iter++) {
            sprintf (sTemp, "\t属性标识:%d\n"
                            "\t属性取值:%s\n"
                            "\t生效时间:%s\n"
                            "\t失效时间:%s\n",
                     (*iter).getAttrID (), (*iter).getAttrValue (),
                     (*iter).getEffDate (), (*iter).getExpDate ());

            strcat (sOut, sTemp);
        }

        //strcat (sOut, "\n");
    }

	ServProductIteration productIter;
	ServProduct product;

	if (G_PSERV->getServProductIteration (productIter)) {
		strcat (sOut, "\n附属产品信息:\n");

		while (productIter.next(product)) {
			sprintf (sTemp, "\t产品标识:%d\n"
					"\t生效时间:%s\n"
					"\t失效时间:%s\n",
					 product.getProductID (), product.getEffDate (),
					 product.getExpDate () );
			strcat (sOut, sTemp);

			attrs.clear ();
			product.getAttrs(attrs);
			if (attrs.size ()) {
				strcat (sOut, "\t附属产品的属性:\n");
				for (iter=attrs.begin (); iter!=attrs.end (); iter++) {
					sprintf (sTemp, "\t\t属性标识:%d\n"
							"\t\t属性取值:%s\n"
							"\t\t生效时间:%s\n"
							"\t\t失效时间:%s\n",
						 (*iter).getAttrID (), (*iter).getAttrValue (), 
						 (*iter).getEffDate (), (*iter).getExpDate ());

					strcat (sOut, sTemp);
				}

				//strcat (sOut, "\n");
			}

		}

	}
	const char *pAccNBR = G_PSERV->getAccNBR(25,sDate);
	char accNbr[33];
	strcpy(accNbr,pAccNBR);//类型不同，不得已为之
	seekNpInfo(sOut,accNbr,sDate);
	long lCharge;
	
	lCharge = G_PTRANSMGR->selectAcctItemAggr (G_PSERV->getServID (), 
					G_PSERV->getBillingCycle()->getBillingCycleID (), 0);

	sprintf (sTemp, "\n帐期[%d]的当前费用 %ld 分", 
                G_PSERV->getBillingCycle()->getBillingCycleID (), lCharge);
	strcat (sOut, sTemp);

	sprintf (sTemp, "\n\n该用户的帐户ID:");

    strcat (sOut, sTemp);

    vector<long> vAcct;
    G_PSERV->getAcctID (vAcct,sDate);

    for (lCharge=0; lCharge < vAcct.size (); lCharge++) {
		if(lCharge>0){
			strcat(sOut,",");
		}
        sprintf (sTemp, "%ld ", vAcct[lCharge]);
        strcat (sOut, sTemp);
    }

	cout << sOut << endl << endl;
	
	return 0;
}
