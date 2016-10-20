/*VER: 1*/ 
#include "Environment.h"
#include "Process.h"

class TracePricing:public Process
{
  public:
	TracePricing ();
	~TracePricing ();
	int run();
	int getBaseInfo ();
	void traceCustPricing ();
	void traceOfferPricing ();
 	void traceProductPricing ();
	void tracePricingSection ();
	char * getSectionName(int iID);
	int getParentSectionID(int iID);

  private:
	int m_iEventID;
	int m_iCombineID;
	int m_iStrategyID;
	int m_iPlanID;
	int m_iServID;
	char m_sTableName[256];
	char m_sBillingRecord[256];

};

TracePricing::TracePricing()
{
	Environment::useUserInfo ();
}

TracePricing::~TracePricing()
{

}

int TracePricing::run()
{
	if (g_argc != 3) {
		printf ("\n[Usage]: %s <table_name> <event_id>\n\n", g_argv[0]);
		return -1;
	}

	strcpy (m_sTableName, g_argv[1]);
	m_iEventID = atoi (g_argv[2]);

	printf ("开始跟踪定价过程...\n\n");

	if (getBaseInfo () < 0) 
		exit (-1);
	
	if (m_iServID <= 0)
		exit (-1);

	Date dt;
	G_PUSERINFO->getServ (G_SERV, m_iServID, dt.toString());

	traceCustPricing ();
	traceOfferPricing ();
 	traceProductPricing ();
	tracePricingSection ();

	printf ("\n跟踪结束.\n");

	return 0;
}

void TracePricing::traceCustPricing ()
{

    char *m_sGetCustPricingInfo=" "
    "  select a.cust_id,  "
    "   a.pricing_plan_id, "
    "   c.pricing_plan_name, "
    "   a.belong_object_type, "
    "   a.belong_object_id, "
    "   d.pricing_combine_id, "
    "   f.event_type_id, "
    "   e.eff_date, "
    "   e.exp_date "
    "  from cust_price_plan a, pricing_plan c, pricing_combine d, life_cycle e, event_pricing_strategy f "
    "  where a.cust_id=%d "
    "  and a.pricing_plan_id=c.pricing_plan_id "
    "  and a.pricing_plan_id=d.pricing_plan_id "
    "  and d.life_cycle_id = e.life_cycle_id "
    "  and d.EVENT_PRICING_STRATEGY_ID = f.EVENT_PRICING_STRATEGY_ID ";

	DEFINE_QUERY(qry);
	char sSqlcode[1024];

	sprintf (sSqlcode, m_sGetCustPricingInfo, G_PSERV->getCustID());
	qry.setSQL (sSqlcode);
	qry.open ();
	printf ("\n当前客户(cust_id:%d)定价信息列表:(*前缀的被执行)\n", G_PSERV->getCustID());
	printf ("---------------------------------------------------------\n");
	while (qry.next ()) {
		if (qry.field(5).asInteger() == m_iCombineID) 
			printf ("*");
		else
			printf (" ");
			
		printf ("计划(%d)[%s], 所属对象类型和ID:(%d-%d), 组合(%d)[事件(%d)] 生效失效:%s-%s\n",
		qry.field(1).asInteger (),
		qry.field(2).asString (),
		qry.field(3).asInteger (),
		qry.field(4).asInteger (),
		qry.field(5).asInteger (),
		qry.field(6).asInteger (),
		qry.field(7).asString (),
		qry.field(8).asString ()
		);


	}
	
}

void TracePricing::traceOfferPricing ()
{
    char *m_sGetOfferPricingInfo=" "
        " select a.offer_id, "
        "   a.offer_name, "
        "   a.pricing_plan_id, "
        "   c.pricing_plan_name,"
        "   d.pricing_combine_id, "
        "   f.event_type_id, "
        "   e.eff_date, "
        "   e.exp_date "
        " from product_offer a, pricing_plan c, pricing_combine d, life_cycle e, event_pricing_strategy f " 
        " where a.offer_id=%d "
        " and a.pricing_plan_id=c.pricing_plan_id "
        " and a.pricing_plan_id=d.pricing_plan_id "
        " and d.life_cycle_id = e.life_cycle_id "
        " and d.EVENT_PRICING_STRATEGY_ID = f.EVENT_PRICING_STRATEGY_ID ";

	DEFINE_QUERY(qry);
	char sSqlcode[1024];

	int iOffers[100];
	int iCount = G_PSERV->getOffers (iOffers);
	printf ("\n当前商品列表的定价信息列表:(*前缀的被执行)\n");
	printf ("---------------------------------------------------------\n");
	for (int i=0; i<iCount; i++) {

		sprintf (sSqlcode, m_sGetOfferPricingInfo, iOffers[i]);
		qry.setSQL (sSqlcode);
		qry.open ();
		while (qry.next ()) {
			if (qry.field(4).asInteger() == m_iCombineID)
				printf ("*");
			else
				printf (" ");
				
			printf ("商品(%d)[%s], 计划(%d)[%s], 组合(%d)[事件(%d)] 生效失效:%s-%s\n",
				qry.field(0).asInteger (),
				qry.field(1).asString (),
				qry.field(2).asInteger (),
				qry.field(3).asString (),
				qry.field(4).asInteger (),
				qry.field(5).asInteger (),
				qry.field(6).asString (),
				qry.field(7).asString ()
			);
		}
		qry.close ();
	}

}

void TracePricing::traceProductPricing ()
{
    char *m_sGetProductPricingInfo=" "
         "  select a.product_id, "
         "    a.product_name, "
         "    a.pricing_plan_id, "
         "    c.pricing_plan_name,"
         "    d.pricing_combine_id, "
         "    f.event_type_id, "
         "    e.eff_date, "
         "    e.exp_date "
         " from product a, pricing_plan c, pricing_combine d, life_cycle e, event_pricing_strategy f "
         " where a.product_id=%d "
         " and a.pricing_plan_id=c.pricing_plan_id "
         " and a.pricing_plan_id=d.pricing_plan_id "
         " and d.life_cycle_id = e.life_cycle_id "
         " and d.EVENT_PRICING_STRATEGY_ID = f.EVENT_PRICING_STRATEGY_ID ";
         
	DEFINE_QUERY(qry);
	char sSqlcode[1024];

	sprintf (sSqlcode, m_sGetProductPricingInfo, G_PSERV->getServInfo()->m_iProductID);
	qry.setSQL (sSqlcode);
	qry.open ();
	printf ("\n当前产品(product_id:%d)定价信息列表:(*前缀的被执行)\n", G_PSERV->getServInfo()->m_iProductID);
	printf ("---------------------------------------------------------\n");
	while (qry.next ()) {
		if (qry.field(4).asInteger() == m_iCombineID)
			printf ("*");
		else
			printf (" ");
		printf ("产品[%s], 计划(%d)[%s], 组合(%d)[事件(%d)] 生效失效:%s-%s\n",
			qry.field(1).asString (),
			qry.field(2).asInteger (),
			qry.field(3).asString (),
			qry.field(4).asInteger (),
			qry.field(5).asInteger (),
			qry.field(6).asString (),
			qry.field(7).asString ()
		);
		
	}
	qry.close ();
}

void TracePricing::tracePricingSection ()
{
	char *p, sSqlcode[1024], sSectionName[200];
	int iIDList[200], iCount=0, iIDType[200], iSectionList[200], iSectionCount=0;
	DEFINE_QUERY (qry);
	int iSectionID, iParentSectionID, iBlankCount=0;

	printf ("\n跟踪资费优惠查找过程(定价段落路径)\n", m_iStrategyID);
	printf ("---------------------------------------------------------\n");
	
	p=strtok (m_sBillingRecord, "|");
	while (p) {
		if (strlen (p) == 0)
			break;

		if (p[0] == 'T') {
			iIDList[iCount] = atoi (p+1);
			iIDType[iCount++] = 'T';
		} else if (p[0] == 'D') {
			iIDList[iCount] = atoi (p+1);
			iIDType[iCount++] = 'D';
		}

		for (int i=0; i<iCount; i++) {
			if (iIDType[i] == 'T') {
				sprintf (sSqlcode, 
					"select PRICING_SECTION_ID from tariff where tariff_id=%d",iIDList[i]);
				qry.setSQL (sSqlcode);
				qry.open ();
				qry.next ();
				iSectionID=qry.field(0).asInteger ();
				iSectionList[iSectionCount++] = iSectionID;
				while ((iParentSectionID=getParentSectionID(iSectionID))>0) {
					iSectionList[iSectionCount++] = iParentSectionID;
					iSectionID = iParentSectionID;
				}

				for (int j=iSectionCount-1; j>=0; j--) {
					printf ("Section(%d)[%s] ==> ", 
						iSectionList[j], getSectionName(iSectionList[j]));
				}
				printf ("Tariff(%d)\n\n", iIDList[i]);
				qry.close ();
	
				
			} else {
				sprintf (sSqlcode, "select PRICING_SECTION_ID from discount where discount_id=%d",iIDList[i]);
				qry.setSQL (sSqlcode);
				qry.open ();
				qry.next ();
				iSectionID=qry.field(0).asInteger ();
				iSectionList[iSectionCount++] = iSectionID;
				while ((iParentSectionID=getParentSectionID(iSectionID))>0) {
					iSectionList[iSectionCount++] = iParentSectionID;
					iSectionID = iParentSectionID;
				}

				for (int j=iSectionCount-1; j>=0; j--) {
					printf ("Section(%d)[%s] ==> ", 
						iSectionList[j], getSectionName(iSectionList[j]));
				}
				printf ("Discount(%d)\n", iIDList[i]);
				qry.close ();
			}
		}

		p = strtok (NULL, "|\n");
	}
}

char * TracePricing::getSectionName(int iID)
{
	char sSqlcode[1024], sSectionName[200];
	DEFINE_QUERY (qry);

	sprintf (sSqlcode, 
		"select nvl(PRICING_SECTION_NAME,'') from pricing_section where PRICING_SECTION_ID=%d", iID);
	qry.setSQL (sSqlcode);
	qry.open ();
	if (qry.next ()) {
		strncpy (sSectionName, qry.field(0).asString(), 200);
		return sSectionName;
	} else
		return NULL;
}

int TracePricing::getParentSectionID(int iID)
{
	char sSqlcode[1024];
	DEFINE_QUERY (qry);
	
	sprintf (sSqlcode, 
		"select nvl (PARENT_SECTION_ID,-1) from pricing_section where PRICING_SECTION_ID=%d", iID);
	qry.setSQL (sSqlcode);
	qry.open ();
	if (qry.next ())
		return qry.field(0).asInteger ();
	else
		return -1;
}

int TracePricing::getBaseInfo ()
{
	char sSqlcode[1024];
	char sStartDate[20];
	sprintf (sSqlcode, 
		"select nvl(serv_id,-1), start_date, nvl(pricing_combine_id,-1), nvl(billing_record,'') from %s where event_id=%d",
		m_sTableName, m_iEventID);

	DEFINE_QUERY (qry);
	qry.setSQL (sSqlcode);
	qry.open ();
	if (qry.next ()) {
		m_iServID = qry.field (0).asInteger ();
		strcpy (sStartDate, qry.field(1).asString());
		m_iCombineID  = qry.field (2).asInteger ();
		strcpy (m_sBillingRecord, qry.field(3).asString());
	} else {
		printf ("未找到该话单,table:[%s], event_id:[%d]\n", m_sTableName, m_iEventID);
		return -1;
	}
	qry.close ();

	if (m_iServID==-1) {
		printf ("话单字段:serv_id为空, 无法跟踪\n");
		return -2;
	}
	printf ("当前话单的serv_id(%d), 组合combine_id(%d)\n", m_iServID, m_iCombineID);

	sprintf (sSqlcode, "select nvl(event_pricing_strategy_id,-1) from pricing_combine where pricing_combine_id=%d", m_iCombineID);
	qry.setSQL (sSqlcode);
	qry.open ();
	if (qry.next ())
		m_iStrategyID = qry.field(0).asInteger ();
	else
		m_iStrategyID = -1;

	if (m_iServID>0 && G_PUSERINFO) {
		G_PUSERINFO->getServ (G_SERV, m_iServID, sStartDate);
	}
	
	return 0;
}


DEFINE_MAIN (TracePricing);

