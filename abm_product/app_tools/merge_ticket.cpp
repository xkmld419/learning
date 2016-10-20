/*VER: 1*/ 
/*
 *   连续话单合并
 *   
 *   ruanyj create  2004/04/01
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "ticket.h"
#include "billcommon.h"
#include "ascdate.h"
#include "TicketCal.h"
#include "MakeBill.h"

#define SPAN_BUFF_LEN 10000
#define MAX_CFG_COUNTS 100

#include "TOCIQuery.h"
#include "ConnectDB.h"
#include "CheckDup.h"
#include "TicketRead.h"
#include "INCallTicketRead.h"
#include "DTCallTicketRead.h"
#include "billing_cycle.h"
#include "mntapi.h"
#include "../cma/monitor.h"
#include "CallTicketRead.h"
#include "CallTicketSave.h"

#define TAB_TS	1
#define TAB_LS	2
#define TAB_IN	3
#define TAB_DT	4
#define TAB_MS	5

typedef struct TMergeTicketCfg {
	int iExchID;
	int iMinInterval;
	int iMaxInterval;
}TMergeTicketCfg;

typedef struct TTicketMainField {
	long lTicketID;
	char sCallingNbr[21];
	char sCalledNbr[21];
	char sStartTime[15];
	char sEndTime[15];
	int iDuration;
	int iCharge;
	int iAddCharge;
	int iExchID;
}TTicketMainField;

class TimeOrderSpan {

public:
	TimeOrderSpan ();
	~TimeOrderSpan ();
	
	/*
	 * 区间清空
	 */
	void clear ();

	/*
	 * 区间插入, 更新区间右边界sEndTime
	 * 返回: 1--区间发生交叉 0--区间没有交叉 -1--插入失败
	 */
	int insert (TicketRead *ticket_read, bool bEndFlag); 

	/* 
	 * 将区间内的话单进行合并
	 */
	int mergeTicket ();
	
private:
	char sCallingNbr[22];
	char sCalledNbr[22];
	int iExchID;

	char sEndTime[20];	// 区间终点
	int TicketCount;	// 连续区间包含的话单量
	TTicketMainField cTicket[SPAN_BUFF_LEN]; // 连续区间包含的话单

	CallTicketStruct m_ticket; // 长市话需要重新算法, 需要该结构的所有算法相关字段 
	
};

CheckDup *dup_checker;
TimeOrderSpan time_span;
TOCIDatabase gpDBLink;
static int iCounter = 0;
static int iMergeCounter = 0;
static TMergeTicketCfg gCfg[MAX_CFG_COUNTS];
static int iCfgCounts;
static char gsTableName[200];
static int giTableType;
MakeBillController * make_bill_controller;

void SaveMergeTicket(TTicketMainField *f_ticket, TicketRead *ticket_read)
{
	f_ticket->lTicketID = ticket_read->lTicketID;
	f_ticket->iDuration = ticket_read->iDuration;
	f_ticket->iExchID   = ticket_read->iExchID;
	f_ticket->iCharge   = ticket_read->iCharge;
	if (giTableType==TAB_TS || giTableType==TAB_LS) {
	    f_ticket->iAddCharge   = ticket_read->iAddCharge;
	}

	strcpy (f_ticket->sCallingNbr, ticket_read->sCallingNbr);
	strcpy (f_ticket->sCalledNbr,  ticket_read->sCalledNbr);
	strcpy (f_ticket->sStartTime,  ticket_read->sStartTime);
	strcpy (f_ticket->sEndTime,    ticket_read->sEndTime);
}

TimeOrderSpan::TimeOrderSpan()
{
	TicketCount = 0;
	sEndTime[0] = 0;
}

TimeOrderSpan::~TimeOrderSpan()
{

}

void TimeOrderSpan::clear()
{
	sCallingNbr[0] = 0;
	sCalledNbr[0] = 0;
	iExchID = -1;

	TicketCount = 0;
	sEndTime[0] = 0;
}
int TimeOrderSpan::mergeTicket ()
{

	int i;

	char sSqlcode[500];
	TOCIQuery qry(&gpDBLink);

	/* 区间内少于2张话单不用合并 */
	if (TicketCount < 2) {
		return 0;
	}

	for (i=1; i<TicketCount; i++) {

		/* 第一张话单累计时长和费用 */
		cTicket[0].iDuration += cTicket[i].iDuration;
		cTicket[0].iCharge += cTicket[i].iCharge;
		if (giTableType==TAB_TS || giTableType==TAB_LS) {
		    cTicket[0].iAddCharge += cTicket[i].iAddCharge;
	    }
		sprintf (sSqlcode, "update %s set state='MRG', state_date=sysdate where ticket_id=%d", gsTableName, cTicket[i].lTicketID);
		qry.Close ();
		qry.SetSQL (sSqlcode);
		qry.Execute ();

	}

	/* 长市话需要把费用重算 */
	if (giTableType==TAB_TS || giTableType==TAB_LS) {

		/* 记录旧时长费用(总) */
		int iOldCharge=0, iOldAddCharge=0;
		iOldCharge   = cTicket[0].iCharge;
		iOldAddCharge = cTicket[0].iAddCharge;

		/* 赋新时长, 重新算费 */
		m_ticket.iDuration = cTicket[0].iDuration;
		PreRating ((CallTicketStruct *) &m_ticket);
		   Rating ((CallTicketStruct *) &m_ticket);

		/* 记录新费用(记到第一张新话单上) */
		cTicket[0].iCharge = m_ticket.iCharge;
		cTicket[0].iAddCharge = m_ticket.iAddCharge;

		/* 赋费用差, 重新合帐,修正账目费用 */
		m_ticket.iCharge -= iOldCharge;
		m_ticket.iAddCharge -= iOldAddCharge;
		MakeBill * make_bill = make_bill_controller->Get(m_ticket.iBillingCycleID);
		if ( make_bill){
			make_bill->AddChargeItem((CallTicketStruct *) &m_ticket);
		}
	}

	/* 对第一条话单进行更新 */
	sprintf (sSqlcode, "update %s set duration=%d, charge=%d, add_charge=%d where ticket_id=%d", gsTableName, cTicket[0].iDuration, cTicket[0].iCharge, cTicket[0].iAddCharge, cTicket[0].lTicketID);
	qry.Close ();
	qry.SetSQL (sSqlcode);
	qry.Execute ();

	gpDBLink.Commit ();

	return TicketCount;

}

int TimeOrderSpan::insert(TicketRead *ticket_read, bool bEndFlag)
{
	int i, ret;
	char sTicketEndTime[20];

	/* 如果终止标志,将区间内的话单合并,刷新入库 */
	if (bEndFlag == true) {
		ret = mergeTicket ();
		return ret;
	}

	if (TicketCount >= SPAN_BUFF_LEN) {
		pprintf (0,0,"[ERROR]: merge ticket buffer full. buffer size:%d\n", SPAN_BUFF_LEN);
		return -1;
	}

	/* 不同的被叫,exch_id,主叫, 将原区间中话单合并, 区间清空,写入该话单 */
	if (TicketCount==0 || strcmp (ticket_read->sCalledNbr, sCalledNbr)!=0 || ticket_read->iExchID!=iExchID || strcmp (ticket_read->sCallingNbr, sCallingNbr)!=0 ) {

		ret = mergeTicket ();

		strcpy (sCalledNbr, ticket_read->sCalledNbr);
		strcpy (sCallingNbr, ticket_read->sCallingNbr);
		iExchID = ticket_read->iExchID;
		
		strcpy (sEndTime, ticket_read->sStartTime);
		AddSeconds (sEndTime, ticket_read->iDuration);

		SaveMergeTicket (&cTicket[0], ticket_read);
		if (giTableType==TAB_TS || giTableType==TAB_LS) {
			m_ticket = *((CallTicketStruct *)ticket_read->getTicketAddr());
		}
		TicketCount = 1;
		return ret;
	}

	/* 查找当前 exch_id 的配置项 */
	for (i=0; i<iCfgCounts; i++) {
		if (gCfg[i].iExchID == ticket_read->iExchID)
			break;
	}
	
	if (i < iCfgCounts) {

		/* 取得当前话单和区间的终止时间的时间差 */
		int iDiffTime = iGetTime(ticket_read->sStartTime) - iGetTime(sEndTime);

		/* 时间差符合配置 */
		if (iDiffTime>=gCfg[i].iMinInterval && iDiffTime<=gCfg[i].iMaxInterval) {

			/* 更新区间的终止时间为本话单的终止时间 */
			strcpy (sTicketEndTime, ticket_read->sStartTime);
			AddSeconds (sTicketEndTime, ticket_read->iDuration);
			if (strcmp (sTicketEndTime, sEndTime) > 0) {
				strcpy (sEndTime, sTicketEndTime);
			}

			/* 保存本话单到区间 */
			SaveMergeTicket (&cTicket[TicketCount], ticket_read);
			if (giTableType==TAB_TS || giTableType==TAB_LS) {
				m_ticket = *((CallTicketStruct *)ticket_read->getTicketAddr());
			}
			TicketCount++;
			return 0;

		/* 时间差不符合配置 */
		} else {

			/* 合并区间内原来的话单 */
			ret = mergeTicket ();

			/* 更新区间的终止时间为本话单的终止时间 */
			strcpy (sTicketEndTime, ticket_read->sStartTime);
			AddSeconds (sTicketEndTime, ticket_read->iDuration);
			strcpy (sEndTime, sTicketEndTime);

			/* 清空区间, 保存本话单 */
			SaveMergeTicket (&cTicket[0], ticket_read);
			if (giTableType==TAB_TS || giTableType==TAB_LS) {
				m_ticket = *((CallTicketStruct *)ticket_read->getTicketAddr());
			}
			TicketCount = 1;

			return ret;
		}
	}

	return 0;
}

void InitAll()
{
	char sSqlcode[500];

	ConnectOciDB(gpDBLink);

        pprintf (0,0,"[MESSAGE]: 初始化Merge_Ticket_cfg...\n");
	iCfgCounts = 0;
	TOCIQuery qry(&gpDBLink);
	sprintf (sSqlcode,"select exch_id, min_interval, max_interval from merge_ticket_cfg order by exch_id");
	qry.Close ();
	qry.SetSQL (sSqlcode);
	qry.Open ();
	while (qry.Next ()) {
		if (++iCfgCounts > MAX_CFG_COUNTS) {
			printf ("[ERROR]: merge_ticket_cfg 配置超过%d条\n", MAX_CFG_COUNTS);
			exit (-1);
		}
		gCfg[iCfgCounts-1].iExchID = qry.Field ("exch_id").asInteger ();
		gCfg[iCfgCounts-1].iMinInterval = qry.Field ("min_interval").asInteger ();
		gCfg[iCfgCounts-1].iMaxInterval = qry.Field ("max_interval").asInteger ();
	}
	if (iCfgCounts == 0) {
		printf ("[WARNING]: 没有配置 merge_ticket_cfg\n");
		exit (-2);
	}
	
	time_span.clear ();

        //初始化合帐处理
	WriteMsg(1, "[MESSAGE]: 初始化合帐处理...\n");
	make_bill_controller = new MakeBillController;
	
}

void UpdateTicketState(char *sTableName, int iTicketID, char *sState)
{
	char sSqlcode[500];
	TOCIQuery qry(&gpDBLink);

	sprintf (sSqlcode, "update %s set state='%s', state_date=sysdate where ticket_id=%d", sTableName, sState, iTicketID);
	qry.SetSQL (sSqlcode);
	qry.Execute ();
	qry.Commit ();
}

int getTableName(char *sTableName, int iBillingCycleID, char *sTableType)
{
	char sSqlcode[500];
	TOCIQuery qry(&gpDBLink);
	
	sprintf (sSqlcode, "select table_name from table_list_cycle where billing_cycle_id=%d and table_type='%s'", iBillingCycleID, sTableType);
	qry.SetSQL (sSqlcode);
	qry.Open ();

	if (qry.Next()) {
		strcpy (sTableName, qry.Field("table_name").asString());
		return 1;
	}

	return 0;
}

main (int argc, char *argv[])
{
try {

	char sTableType[20], sTemp[20];
	int opt, iNormalTicketID, iDupType, ret, iExchID=-1, iBillTaskID;
	TicketRead *ticket_read;
	
	while((opt=getopt(argc,argv,"t:"))!=EOF)
	{
		switch(opt)
		{
			case 't':
				gsTableName[0] = 0;
				/* 长话 */
			        if ( strstr( optarg,"TS") != NULL || strstr(optarg,"ts")!=NULL ) {
					giTableType = TAB_TS;
					strcpy (sTableType, "TS");
					ticket_read = new CallTicketRead();
				}
					
				/* 市话 */
			        if ( strstr( optarg,"LS") != NULL || strstr(optarg,"ls")!=NULL ) {
					giTableType = TAB_LS;
					strcpy (sTableType, "LS");
					ticket_read = new CallTicketRead();
				}

				/* 智能网 */
			        if ( strstr( optarg,"IN") != NULL || strstr(optarg,"in")!=NULL ) {
					giTableType = TAB_IN;
					strcpy (sTableType, "IN");
					ticket_read = new INCallTicketRead();
				}

				/* 数据计费 */
			        if ( strstr( optarg,"DT") != NULL || strstr(optarg,"dt")!=NULL ) {
					giTableType = TAB_DT;
					strcpy (sTableType, "DT");
					ticket_read = new DTCallTicketRead();
				}

				/* 信息台, 暂注释 
			        if ( strstr( optarg,"MS") != NULL || strstr(optarg,"ms")!=NULL ) {
					giTableType = TAB_MS;
					strcpy (sTableType, "MS");
					ticket_read = new MSCallTicketRead();
				} */
					
					break;
		}
	}

	ConnectMonitor(1);

	if (giTableType == 0) {
		pprintf (0,0,"[Usage]: %s -t [ts|ls|in|dt]\n", argv[0]);
		DisconnectMonitor(PS_ABORT);
		exit (0);
	}

	InitAll ();

	iBillTaskID = GetBillTaskID();
	pprintf (0,0,"[MESSAGE]: env: BILL_TASK_ID=%d\n", iBillTaskID);
	BillTask bt(iBillTaskID);
	bt.First ();

	while (bt.Next()) {

	/* 根据 table_list_cycle 获得表名,如果表不存在则跳过 */
	if (getTableName (gsTableName, bt.cycle->iBillingCycleID, sTableType) == 0)
		continue;

	iCounter = iMergeCounter = 0;

	pprintf (0,0,"[MESSAGE]: 开始打开话单表: %s\n", gsTableName);
	//CallTicketRead ticket_read;
        ticket_read->SetTableName (gsTableName);
	if (giTableType==TAB_TS || giTableType==TAB_LS) {

		ticket_read->AddCondition ("and state='T00' and switch_id in (select distinct exch_id from merge_ticket_cfg) order by calling_area_code asc, calling_nbr asc, switch_id asc, start_time asc");

	} else {

		ticket_read->AddCondition ("and state='T00' and exch_id in (select distinct exch_id from merge_ticket_cfg) order by calling_area_code asc, calling_nbr asc, exch_id asc, start_time asc");

	}
        ticket_read->Open ();

        pprintf (0,0,"[MESSAGE]: 开始话单合并处理...\n");
	time_span.clear ();
	while (ticket_read->Next ()) {
		
		ticket_read->getBaseInfo ();

		if (++iCounter%50000 == 0)
			pprintf (0,0,"[MESSAGE]: 已处理话单%d条, 合并%d条\n", iCounter, iMergeCounter);
		
		ret = time_span.insert (ticket_read, false);
		if (ret < 0) {
			DisconnectMonitor(PS_ABORT);
			exit (-1);
		} else {
			iMergeCounter += ret;
		}
		
	}
	
	ret = time_span.insert (NULL, true);
	iMergeCounter += ret;

	make_bill_controller->FlushAll();
	gpDBLink.Commit();
	pprintf (0,0,"[MESSAGE]: 共处理话单%d条, 合并%d条\n", iCounter, iMergeCounter);

	}

	pprintf (0,0,"[MESSAGE]: 处理结束\n");

	DisconnectMonitor(PS_NORMALDONE);

}catch (TException &e) {
	pprintf (0,0,"[Exception]: %s.\n", e.GetErrMsg());
	DisconnectMonitor(PS_NORMALDONE);
	exit (-1);
}

}

