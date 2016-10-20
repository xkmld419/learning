/*VER: 1*/ 
/* 主要功能：
		4008，800，学子e行等根据完整号码取区号，与现有档案查找不同，在号头表中影响效率，且不容易刷新，
		所以采用共享内存方式。

		上载，卸载，查找等功能；

        更新内存不采用实时更新的方式，因为没有接口程序生成可信的数据，需要人工整理数据，所以需要检查的过程。
		检查确认后，执行更新命令即可，会同时更新表和内存。
*/

//#include "Process.h"
#include "WorkFlow.h"
#include "Log.h"
#include "ParamDefineMgr.h"
#include "Environment.h"

#include "ext_serv.h"

long GetNextSeqID(char *sSeqName)
{
	char sSql[201];
	sprintf(sSql,"select %s.nextval new_id from dual ",sSeqName);
	DEFINE_QUERY(qry);
	qry.setSQL(sSql);
	qry.open();
	if (qry.next()) 	
		return qry.field(0).asLong();
	else
		return -1;
}


ExtServCtl::ExtServCtl(int iGroup):DataEventMemCtl(iGroup)
{
	
}

ExtServCtl::~ExtServCtl()
{

}

void ExtServCtl::create()
{
	int iCount1 = 0,iCount2 = 0;
	char sCountBuf[32]={0};

	if (!ParamDefineMgr::getParam(USERINFO_SEGMENT,EXT_SERV_DATA_NUM,sCountBuf))
	{
		Log::log(0,"请在b_param_define中配置ext_serv需要的数据内存大小。");
		THROW(MBC_ExtServCtl+50);		
	}

	iCount1 = atoi(sCountBuf);

/*	if (!ParamDefineMgr::getParam(USERINFO_SEGMENT,EXT_SERV_INDEX_NUM,sCountBuf))
	{
		Log::log(0,"请在b_param_define中配置ext_serv需要的索引内存大小。");
		THROW(MBC_ExtServCtl+50);
	}
	iCount2 = atoi(sCountBuf);
*/
	m_poExtServData->create(iCount1);
	m_poExtServIndex->create(iCount1,MAX_ACCOUNT_NBR_LEN,3);
		
}

void ExtServCtl::unloadExtServ()
{
#define UNLOAD(X) \
				if(X) \
                if (X->exist()) { \
				if(X->connnum() > 1) \
				{ \
				THROW(MBC_E8UserCtl+3); \
} \
				X->remove (); \
                }

	UNLOAD(m_poExtServData);
	UNLOAD(m_poExtServIndex);

	m_bAttached = false;

	Log::log(0,"卸载结束.");
	return;
}

void ExtServCtl::linkExtNbr(ExtServ &poExtServ,unsigned int iPos)
{
	unsigned int k;
	ExtServ *p;

	p = (ExtServ *)(*m_poExtServData);

	if (!m_poExtServIndex->get(poExtServ.sExtNbr,&k))
	{
		m_poExtServIndex->add(poExtServ.sExtNbr,iPos);
	}
	else
	{
		unsigned int iCur = k;
		unsigned int iPre = k;
		unsigned int iHead = k;

		while (iCur) 
		{
			//同一类型的号码仅存在一份，直接覆盖信息即可
			if (strcmp(p[iCur].sExtType,poExtServ.sExtType) == 0) 
			{
				strcpy(p[iCur].sAreaCode,poExtServ.sAreaCode);
				strcpy(p[iCur].sState,poExtServ.sState);
				strcpy(p[iCur].sEffDate,poExtServ.sEffDate);
				strcpy(p[iCur].sExpDate,poExtServ.sExpDate);
			}
			
			iCur = p[iCur].m_iNextOffset;
		}
	}
}

void ExtServCtl::loadExtServ()
{
	ExtServ *p;
	unsigned int i,k;
	int	iRecCnt = 0;

	unloadExtServ();
	
	this->create();

	DEFINE_QUERY(qry);

	p = (ExtServ *)(*m_poExtServData);
	
	qry.setSQL("select EXT_SERV_ID,AREA_CODE,EXT_NBR,EXT_TYPE,STATE,EFF_DATE,EXP_DATE from b_ext_serv ");
	qry.open();
	while (qry.next()) 
	{
		i = m_poExtServData->malloc();
		if (!i)
		{
			Log::log(0,"ext_serv档案数据空间无法继续分配，可能空间已满。");
			THROW(MBC_ExtServCtl+1);	
		}
		
		p[i].lExtServID = qry.field(0).asLong();
		strcpy(p[i].sAreaCode,qry.field(1).asString());
		strcpy(p[i].sExtNbr,qry.field(2).asString());
		strcpy(p[i].sExtType,qry.field(3).asString());
		strcpy(p[i].sState,qry.field(4).asString());
		strcpy(p[i].sEffDate,qry.field(5).asString());
		strcpy(p[i].sExpDate,qry.field(6).asString());

		linkExtNbr(p[i],i);
		iRecCnt++;

		if (iRecCnt % 10000 == 0) 
			Log::log(0,"已载入%d条.\n",iRecCnt);
	}

	Log::log(0,"上载结束，共载入%d条.",iRecCnt);
}

bool ExtServCtl::chk_upRec(ExtServChange &pServChange,TOCIQuery &qryIns,TOCIQuery &qryUpd)
{
	unsigned int i,k;
	long lNewID;
	ExtServ *p;
	int iFindRec = 0;
	char sSql[501]={0};
	
	p = (ExtServ *)(*m_poExtServData);

	try
	{
		DEFINE_QUERY(qrySel);
		qrySel.close();
		
		qrySel.setSQL(" select count(1) cnt from b_ext_serv "
			" where ext_nbr=:EXTNBR and ext_type=:EXTTYPE");
		qrySel.setParameter("EXTNBR",pServChange.sExtNbr);
		qrySel.setParameter("EXTTYPE",pServChange.sExtType);
		qrySel.open();
		qrySel.next();
		iFindRec = qrySel.field(0).asInteger();
		if (iFindRec) 
		{
			qryUpd.close();
			sprintf(sSql,"update b_ext_serv set state=:STATE,state_date=sysdate,eff_date=to_date(:EFFDATE,'yyyymmddhh24miss'),"
						"					exp_date=to_date(:EXPDATE,'yyyymmddhh24miss'),area_code=:AREACODE ");
			if (strlen(pServChange.sAreaName) >0 )
				strcat(sSql," ,area_name=:AREA_NAME ");
			if (strlen(pServChange.sRemark1) >0 ) 
				strcat(sSql," ,remark1=:REMARK1 ");
			if (strlen(pServChange.sRemark2) >0 ) 
				strcat(sSql," ,remark2=:REMARK2 ");

			strcat(sSql," where ext_nbr=:EXTNBR and ext_type=:EXTTYPE ");
			
			/*qryUpd.setSQL(" update b_ext_serv set state=:STATE,state_date=sysdate,eff_date=to_date(:EFFDATE,'yyyymmddhh24miss'),"
				"					exp_date=to_date(:EXPDATE,'yyyymmddhh24miss'),area_code=:AREACODE,"
				"					area_name=:AREA_NAME,remark1=:REMARK1,remark2=:REMARK2 "
				" where ext_nbr=:EXTNBR and ext_type=:EXTTYPE");*/
			qryUpd.setSQL(sSql);
			
			qryUpd.setParameter("STATE",pServChange.sState);
			qryUpd.setParameter("EFFDATE",pServChange.sEffDate);		
			qryUpd.setParameter("EXPDATE",pServChange.sExpDate);
			qryUpd.setParameter("AREACODE",pServChange.sAreaCode);
			if (strlen(pServChange.sAreaName) >0 )
				qryUpd.setParameter("AREA_NAME",pServChange.sAreaName);
			if (strlen(pServChange.sRemark1) >0 ) 
				qryUpd.setParameter("REMARK1",pServChange.sRemark1);
			if (strlen(pServChange.sRemark2) >0 ) 
				qryUpd.setParameter("REMARK2",pServChange.sRemark2);
			
			qryUpd.setParameter("EXTNBR",pServChange.sExtNbr);
			qryUpd.setParameter("EXTTYPE",pServChange.sExtType);
			qryUpd.execute();			
		}
		else
		{
			qryIns.close();
			qryIns.setSQL("insert into b_ext_serv(ext_serv_id,AREA_CODE,EXT_NBR,EXT_TYPE,STATE,STATE_DATE,EFF_DATE," 
				"						EXP_DATE,AREA_NAME,REMARK1,REMARK2)"
				"values (:EXTSERVID,:AREACODE,:EXTNBR,:EXTTYPE,:STATE,SYSDATE,to_date(:EFFDATE,'yyyymmddhh24miss'),"
				"		to_date(:EXPDATE,'yyyymmddhh24miss'),:AREANAME,:REMARK1,:REMARK2)");
			
			lNewID = GetNextSeqID("seq_ext_serv_id");
			qryIns.setParameter("EXTSERVID",lNewID);
			qryIns.setParameter("AREACODE",pServChange.sAreaCode);
			qryIns.setParameter("EXTNBR",pServChange.sExtNbr);
			qryIns.setParameter("EXTTYPE",pServChange.sExtType);
			qryIns.setParameter("STATE",pServChange.sState);
			qryIns.setParameter("EFFDATE",pServChange.sEffDate);		
			qryIns.setParameter("EXPDATE",pServChange.sExpDate);			
			qryIns.setParameter("AREANAME",pServChange.sAreaName);
			qryIns.setParameter("REMARK1",pServChange.sRemark1);
			qryIns.setParameter("REMARK2",pServChange.sRemark2);
			qryIns.execute();
		}
		qrySel.close();
		
		if (!m_poExtServIndex->get(pServChange.sExtNbr,&k)) 
		{
			i = m_poExtServData->malloc();
			p[i].lExtServID = lNewID;	
			strcpy(p[i].sExtNbr,pServChange.sExtNbr);
			strcpy(p[i].sExtType,pServChange.sExtType);
		}
		else
			i = k;
		
		strcpy(p[i].sAreaCode,pServChange.sAreaCode);
		strcpy(p[i].sState,pServChange.sState);
		strcpy(p[i].sEffDate,pServChange.sEffDate);
		strcpy(p[i].sExpDate,pServChange.sExpDate);
		linkExtNbr(p[i],i);

		qrySel.setSQL(" update b_ext_serv_change set up_flag=1,up_date=sysdate "
					" where rowid=:ROW_ID");
		qrySel.setParameter("ROW_ID",pServChange.sRowID);
		qrySel.execute();	
		
		return true;
	}
	
	catch (TOCIException e)
	{
		Log::log(0,"发生sql错误，错误描述:%s,\n,sql:%s",e.getErrMsg(),e.getErrSrc());
		return false;
	}
	catch (Exception e) 
	{
		Log::log(0,"发生错误，错误描述:%s",e.descript());
		return false;
	}
	catch (...) 
	{
		Log::log(0,"发生未知错误.");
		return false;
	}
}

void ExtServCtl::updateExtServ()
{	
	ExtServChange pTmp;
	int	iSuccCnt=0,iFailCnt = 0;
	
	DEFINE_QUERY(qry);
	DEFINE_QUERY(qry_ins);
	DEFINE_QUERY(qry_upd);
	
	qry.setSQL(" select AREA_CODE,EXT_NBR,EXT_TYPE,STATE,EFF_DATE,"
		" EXP_DATE,AREA_NAME,REMARK1,REMARK2,rowid from b_ext_serv_change "
		" where (up_flag = 0 or up_flag is null) and rownum<1001 ");
	qry.open();
	while (qry.next()) 
	{		
		memset(&pTmp,0,sizeof(ExtServChange));
		
		strcpy(pTmp.sAreaCode,qry.field(0).asString());
		strcpy(pTmp.sExtNbr,qry.field(1).asString());
		strcpy(pTmp.sExtType,qry.field(2).asString());
		strcpy(pTmp.sState,qry.field(3).asString());
		strcpy(pTmp.sEffDate,qry.field(4).asString());
		strcpy(pTmp.sExpDate,qry.field(5).asString());
		strcpy(pTmp.sAreaName,qry.field(6).asString());
		strcpy(pTmp.sRemark1,qry.field(7).asString());
		strcpy(pTmp.sRemark2,qry.field(8).asString());
		strcpy(pTmp.sRowID,qry.field(9).asString());
		
		if (chk_upRec(pTmp,qry_ins,qry_upd)) 
		{
			iSuccCnt++;
			qry_ins.commit();  //任意一个qry提交即可
			AddTicketStat (TICKET_NORMAL);
		}
		else
		{
			iFailCnt++;
			qry_ins.rollback();
			AddTicketStat (TICKET_ERROR);
		}
		
		if ((iSuccCnt + iFailCnt) % COMMIT_LEN ==0)
			Log::log(0,"共处理%d条，其中成功%d，失败%d",iSuccCnt+iFailCnt,iSuccCnt,iFailCnt);		
	}
	
	if (iSuccCnt + iFailCnt>0)
		Log::log(0,"共处理%d条，其中成功%d，失败%d",iSuccCnt+iFailCnt,iSuccCnt,iFailCnt);
	//qry_ins.commit();  //任意一个qry提交即可。
	
	qry.close();
	qry_ins.close();
	qry_upd.close();
}


ExtServMgr::ExtServMgr(int iGroup):DataEventMemBase(iGroup)
{
	bindData();
}

ExtServMgr::~ExtServMgr()
{

}

bool ExtServMgr::getAreaCodeByExtNbr(ExtServ *poExtServ,char *sAccNbr,char *sNbrType,char *sStartDate)
{
	unsigned int i,k;
	char sTmpNbr[MAX_CALLED_NBR_LEN] = {0};
	char sTmpType[4] = {0};
	ExtServ *p;

	p = (ExtServ *)(*m_poExtServData);

	strcpy(sTmpNbr,sAccNbr);
	strcpy(sTmpType,sNbrType);

	if (!m_poExtServIndex->get(sTmpNbr,&k)) 		
		return false;

	while (k) 
	{
		if (strcmp(sTmpType,p[k].sExtType) == 0  ) 
		{
			if (strcmp(sStartDate,p[k].sEffDate) >= 0 && strcmp(sStartDate,p[k].sExpDate) <=0 )
			{
				memcpy(poExtServ,&p[k],sizeof(ExtServ));
				return true;
			}			
		}

		k = p[k].m_iNextOffset;
	}

	if (!k)
		return false;
}

int ExtServMgr::percentOfMemUsed(int iMemMode)
{
	unsigned int iPercent,iUsed,iTotal;
	
	
	if (iMemMode == DATA_BUFFER)
	{		
		iUsed = m_poExtServData->getCount();
		iTotal = m_poExtServData->getTotal();
		iPercent = iUsed*100/iTotal;
		//iPercent = (m_poE8AccountData->getCount()/m_poE8AccountData->getTotal()) * 100;
		printf("数据区－内存使用情况：总量[%d],已使用[%d],使用比例[%d]\n",iTotal,iUsed,iPercent);		
	}
	
	if (iMemMode == INDEX_BUFFER)
	{
		iUsed = m_poExtServIndex->getCount();
		iTotal = m_poExtServIndex->getTotal();
		iPercent = iUsed*100/iTotal;
		//iPercent = (m_poE8AccountIndex->getCount()/m_poE8AccountIndex->getTotal()) * 100;
		printf("索引区－内存使用情况：总量[%d],已使用[%d],使用比例[%d]\n",iTotal,iUsed,iPercent);
	}
	
	return iPercent;
	
}

void ExtServMgr::checkMemUse()
{
	percentOfMemUsed(DATA_BUFFER);
	percentOfMemUsed(INDEX_BUFFER);
}

void ExtServMgr::dispUserInfo(char *sExtNbr,char *sExtType)
{
	unsigned int i;
	bool bFind = false;
	
	if (!m_poExtServIndex->get(sExtNbr,&i)) 
	{
		Log::log(0,"未找到号码%s在内存中的信息。",sExtNbr);
		return;
	}

	while (i) 
	{
		if (strcmp(m_poExtServIns[i].sExtType,sExtType) == 0) 
		{
			printf("ExtServID:%ld,ExtNbr:%s,ExtType:%s,AreaCode:%s\n",
					m_poExtServIns[i].lExtServID,m_poExtServIns[i].sExtNbr,m_poExtServIns[i].sExtType,m_poExtServIns[i].sAreaCode);
			printf("State:%s,EffDate:%s,ExpDate:%s\n\n",m_poExtServIns[i].sState,m_poExtServIns[i].sEffDate,m_poExtServIns[i].sExpDate);
			bFind = true;
		}

		i = m_poExtServIns[i].m_iNextOffset;
	}

	if (!i && bFind == false) 
	{
		Log::log(0,"未找到%s类型号码%s在内存中的信息。",sExtType,sExtNbr);
		return;
	}
}

void ExtServMgr::dispAll()
{
	unsigned int i;

	for (i =1;i<=m_poExtServData->getCount();i++)
	{
		printf("第%d个：",i);
		printf("ExtServID:%ld,ExtNbr:%s,ExtType:%s,AreaCode:%s\n",
			m_poExtServIns[i].lExtServID,m_poExtServIns[i].sExtNbr,m_poExtServIns[i].sExtType,m_poExtServIns[i].sAreaCode);
		printf("State:%s,EffDate:%s,ExpDate:%s,Next:%d\n\n",m_poExtServIns[i].sState,m_poExtServIns[i].sEffDate,m_poExtServIns[i].sExpDate,m_poExtServIns[i].m_iNextOffset);
	}
}

void ExtServMgr::alarmMemUser(char *sRcvAlarmAccNbr)   //使用量告警
{
	int iPercent;
	char sAlarmInfo[101];
	
	iPercent = percentOfMemUsed(DATA_BUFFER);
	if (iPercent >97)
	{
		sprintf(sAlarmInfo,"ext_serv内存数据空间使用率已超过97%，请及时调整大小.");
		insMonitorInterface(sAlarmInfo,sRcvAlarmAccNbr);
	}

	iPercent = percentOfMemUsed(INDEX_BUFFER);
	if (iPercent >97)
	{
		sprintf(sAlarmInfo,"ext_serv内存索引空间使用率已超过97%，请及时调整大小.");
		insMonitorInterface(sAlarmInfo,sRcvAlarmAccNbr);
	}
	
}

#ifdef  USERINFO_REUSE_MODE
void  ExtServCtl::revokeExtServ()//将b_ext_serv已过期的资料占用内存释放
{
	const char * skey;
	unsigned int uiKeyOffset=0;
	unsigned int uiIndex=0;
	char countbuf[32];
	int 	iNum = 0;	

	int iKickCnt=0;
	char sLimitDate[16];//设置一个日期
	iKickCnt = 0;
	if(!ParamDefineMgr::getParam(USERINFO_LIMIT, EXT_SERV, countbuf))
	{
		Log::log(0,"请先在表b_param_define中配置 %s 档案的内存大小参数.",EXT_SERV);
		THROW(MBC_IvpnInfoCtl+60);
	}
	iNum = atoi(countbuf);	
	getBillingCycleOffsetTime(iNum,sLimitDate);
	SHMStringTreeIndex_A::Iteration iter = m_poExtServIndex->getIteration();
	ExtServ * pp = (ExtServ *)(*m_poExtServData);    

	while( iter.next(skey, uiKeyOffset) ){
		unsigned int uiTemp=0;
		bool bKickSelf=false;
                
		if( uiKeyOffset ){
			iKickCnt += m_poExtServData->kickAndRevoke(
								uiKeyOffset,
								offsetof(ExtServ, m_iNextOffset),
								offsetof(ExtServ, sExpDate),  sLimitDate,
								-1,
								-1,
								&uiTemp,
								&bKickSelf);
			if( bKickSelf ){
				if(0 == uiTemp)
					iter.removeMe();
				else
					iter.setValue(uiTemp);
			}

		}
		if ( ( iKickCnt % 1000 ) == 0 && iKickCnt)
			Log::log(0,"已处理过期内存 %d\n",iKickCnt);                
	}///end while iter.next
	cout<< "kick "<< "b_ext_serv" <<":"<< iKickCnt<<"."<<endl;

	return ;	
}

#endif	

bool ExtServCtl::getBillingCycleOffsetTime(int iOffset,char * strDate)
{
    Date d;
    char sql[1024];
    memset(sql,0,sizeof(sql));      
    DEFINE_QUERY (qry);
    iOffset = iOffset -1;
    sprintf(sql, "select to_char(add_months(cycle_begin_date,-%d),'yyyymmddhh24miss') begin_date FROM billing_cycle where "
            " billing_cycle_type_id=1 AND to_date(%s,'yyyymmddhh24miss')>=cycle_begin_date and "
            " to_date(%s,'yyyymmddhh24miss')<cycle_end_date ",iOffset,d.toString(),d.toString());
    qry.setSQL (sql);
    qry.open ();
    qry.next();
    strcpy(strDate,qry.field(0).asString());
    qry.commit();
    qry.close ();
    return true;
}