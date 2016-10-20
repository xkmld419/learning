#include "interfaceowe.h"   //命令头文件

#ifdef ABM_BALANCE
/*
*   ABM 同步API synExecute扣费
*/
AbmInterfaceManage::AbmInterfaceManage() 
{
  //ABM接口初始化
  pInst = new ABM;
  m_iInitRet = pInst->init();
  if (m_iInitRet != 0) 
  	 {
       m_iInitRet = m_iInitRet>0 ? m_iInitRet*-1:m_iInitRet;
       Log::log(0, "ABM接口初始化失败,错误代码:%d", m_iInitRet);
     }
}

AbmInterfaceManage::~AbmInterfaceManage() 
{
  if (pInst != NULL) 
  	 {
       delete pInst;
       pInst = NULL;
     }
}

/***************************************************************************
*函数名：int getInitResult()
*功能：获取初始化返回值,如果该值不等于0,那么初始化失败
***************************************************************************/
int AbmInterfaceManage::getInitResult() 
{
  return m_iInitRet;
}

/***************************************************************************
*函数名：int resetBatCharge()
*功能：初始化批量口费的请求
***************************************************************************/
int AbmInterfaceManage::resetBatCharge()
{
  brdCmd.reset();
  return 0;
}

/***************************************************************************
*函数名：int pachBatCharge()
*功能：批量扣费打包
***************************************************************************/
int AbmInterfaceManage::pachBatCharge(DeductBalanceR &oData, vector<DeductAcctItem*> &vecBal)
{
  if (brdCmd.add(oData, vecBal) != 0) 
  	 {
       //Log::log(0, "批量实时扣费命令打包失败!");
       return -1;
     }
  else 
     return 0;
}

/***************************************************************************
*函数名：bool pachCharge()
*功能：对于扣费记录打包
***************************************************************************/
int AbmInterfaceManage::pachCharge(DeductBalanceR &oData, vector<DeductAcctItem*> &vecBal)
{
  //实时扣费记录命令打包
  if (qryCmd.set(oData, vecBal) != 0) 
  	 {
       Log::log(0, "实时扣费命令打包失败!");
       return -1;
     }
  else 
     return 0;
}

/***************************************************************************
*函数名：bool transBatCharge()
*功能：批量上传扣费
***************************************************************************/
int AbmInterfaceManage::transBatCharge()
{
  pInst->beginTrans();
  int ret = pInst->synExecute(&brdCmd, &brdResult);
  if (ret != 0) 
  	 {
       Log::log(0, "ABM扣费异常,错误代码:%d ", ret);
       ret = ret>0 ? ret*-1:ret;
       return ret;
     }

  ret = brdResult.getResultCode();
  if (ret != 0) 
  	 {
       Log::log(0, "ABM扣费异常,错误代码:%d ", ret);
       ret = ret>0 ? ret*-1:ret;
       return ret;
     }
  return 0;
}

/***************************************************************************
*函数名：bool transCharge()
*功能：上传扣费
***************************************************************************/
int AbmInterfaceManage::transCharge()
{
  /* SYS PID
  int m_iSysPID;
  m_iSysPID = getpid();
  */
  pInst->beginTrans();
  int ret = pInst->synExecute(&qryCmd, &qryRlt);
  if (ret != 0) 
  	 {
        Log::log(0, "ABM扣费异常,错误代码:%d ", ret);
        ret = ret>0 ? ret*-1:ret;
        return ret;
     }

  ret = qryRlt.getResultCode();
  if (ret != 0) 
  	 {
        Log::log(0, "ABM扣费异常,错误代码:%d ", ret);
        ret = ret>0 ? ret*-1:ret;
        return ret;
     }
  return 0;
}

/***************************************************************************
*函数名：bool AbmInterfaceManage::queryBatBalance(RealDeductA &queryreal, int ioffset)
*功能：查询余额信息
***************************************************************************/
int AbmInterfaceManage::queryBatBalance(RealDeductA &queryreal, int ioffset)
{
  //获取账户和用户级余额
  queryreal.m_lUniversalBalance = brdResult.getUniversalBalance(ioffset); 
  //获取账户专款专用余额
  queryreal.m_lAcctSpecialBalance  = brdResult.getAcctSpecialBalance(ioffset);
  //获取用户专款专用余额
  queryreal.m_lServSpecialBalance = brdResult.getServSpecialBalance(ioffset);
  //获取欠费
  queryreal.m_lServOweCharge = brdResult.getServOweCharge(ioffset);
  return 0;
}

/***************************************************************************
*函数名：bool AbmInterfaceManage::queryBalance(RealDeductA & queryreal)
*功能：查询余额信息
***************************************************************************/
int AbmInterfaceManage::queryBalance(RealDeductA &queryreal)
{
  //获得ABM返回的查询信息
  prlt = (RealDeductResult *)&qryRlt;
  //获取账户和用户级余额
  queryreal.m_lUniversalBalance = prlt->getUniversalBalance(); 
  //获取账户专款专用余额
  queryreal.m_lAcctSpecialBalance  = prlt->getAcctSpecialBalance();
  //获取用户专款专用余额
  queryreal.m_lServSpecialBalance = prlt->getServSpecialBalance();
  //获取欠费
  queryreal.m_lServOweCharge = prlt->getServOweCharge();
  return 0;
}

/***************************************************************************
*函数名：bool AbmInterfaceManage::getBatCmdNum()
*功能：查询批量请求的数量
***************************************************************************/
long AbmInterfaceManage::getBatCmdNum()
{
  return brdCmd.getNum();
}

/***************************************************************************
*函数名：bool AbmInterfaceManage::getBatResultNum()
*功能：查询批量扣费结果的数量
***************************************************************************/
long AbmInterfaceManage::getBatResultNum()
{
  return brdResult.getNum();
}

/***************************************************************************
*函数名：int AbmInterfaceManage::getBreakPoint(long lFileID,long acct_id)
*功能：从ABM处获取文件的处理断点
***************************************************************************/
int AbmInterfaceManage::getBreakPoint(long lFileID, long &loffset)
{
  if (qryCmd1.set(lFileID) != 0) 
  	 {
       Log::log(0, "获取文件断点命令打包失败!");
       return -1;
     }

  pInst->beginTrans();
  int ret = pInst->synExecute(&qryCmd1, &qryRlt1);
  if (ret != 0) 
  	 {
       Log::log(0, "ABM获取文件断点异常,错误代码:%d", ret);
       ret = ret>0 ? ret*-1:ret;
       return ret;
     }
  
  ret = qryRlt1.getResultCode(); 
  if (ret != 0) 
  	 {
       Log::log(0, "ABM获取文件断点异常,错误代码:%d", ret);
       ret = ret>0 ? ret*-1:ret;
       return ret;
     }
  else 
  	 {
       prlt1 = (RealQueryResult *)&qryRlt1;
       loffset = prlt1->getOffset();
       return 0;
     }
}

/***************************************************************************
*函数名:bool querypayresult();
*功能：从ABM处获取扣费实时查询结果
***************************************************************************/
int AbmInterfaceManage::querypayresult(long lAcctID, long lServID, PayResultQryA &payresult)
{
  if (qryCmd2.set(lAcctID, lServID) != 0) 
  	 {
       Log::log(0,"ABM获取实时查询打包失败!");
       return -1;
     }
    
  pInst->beginTrans();
  int ret = pInst->synExecute(&qryCmd2, &qryRlt2); 
  if (ret != 0) 
  	 {
       Log::log(0,"ABM获取实时查询结果异常,错误代码:%d", ret);
       ret = ret>0 ? ret*-1:ret;
       return ret;
     }
  
  ret = qryRlt2.getResultCode();    
  if (ret != 0) 
  	 {
       Log::log(0,"ABM获取实时查询结果异常,错误代码:%d", ret);
       ret = ret>0 ? ret*-1:ret;
       return ret;
     }
  else 
  	 {
       //获得ABM返回的扣费实时信息
       prlt2 = (PayResultQryResult *)&qryRlt2;
       //获取账户和用户级余额
       payresult.m_lUniversalBalance = prlt2->getUniversalBalance(); 
       //获取账户专款专用余额
       payresult.m_lAcctSpecialBalance  = prlt2->getAcctSpecialBalance();
       //获取用户专款专用余额
       payresult.m_lServSpecialBalance = prlt2->getServSpecialBalance();
       //获取欠费
       payresult.m_lServOweCharge = prlt2->getServOweCharge();
       return 0;
     }
} 

/***************************************************************************
*函数名:bool queryBalanceDetail();
*功能：查询abm帐户余额明细
***************************************************************************/
int AbmInterfaceManage::queryBalanceDetail(long lAcctID, vector<BalanceInfoS *> &vecBalanceInfoS)
{
	qb.m_lAcctID = lAcctID;
	qb.m_iQueryFlag = 1;
	qb.m_lQueryID = lAcctID;
	qb.m_iQueryState = 1;
	qbSCmd.set(qb);
	
	pInst->beginTrans();
	int iRet = pInst->synExecute(&qbSCmd, &qbSResult);
	if (iRet != 0)
		 {
       Log::log(0, "ABM查询帐户%ld余额明细异常,错误代码:%d", lAcctID, iRet);
       iRet = iRet>0 ? iRet*-1:iRet;
		   return iRet;
		 }

	iRet = qbSResult.getResultCode();
	if (iRet != 0)
		 {
       Log::log(0, "ABM查询帐户%ld余额明细异常,错误代码:%d", lAcctID, iRet);
       iRet = iRet>0 ? iRet*-1:iRet;
		   return iRet;
		 }
	else
		 {
		   qbSResult.getRecord(vecBalanceInfoS);
		   return 0;
		 }	   
}

/***************************************************************************
*函数名:bool queryCurAggrDetail();
*功能：查询abm帐户实时费用明细
***************************************************************************/
int AbmInterfaceManage::queryCurAggrDetail(long lAcctID, vector<AcctItemInfoHB *> &vecAcctItemInfoHB)
{
  qaiCmd.setAcctQry(lAcctID, 1);  	
  
	pInst->beginTrans();
	int iRet = pInst->synExecute(&qaiCmd, &qaiResult);
	if (iRet != 0)
		 {
       Log::log(0, "ABM查询帐户%ld实时费用明细异常,错误代码:%d", lAcctID, iRet);
       iRet = iRet>0 ? iRet*-1:iRet;
		   return iRet;
		 }
	
	iRet = qaiResult.getResultCode();
	if (iRet != 0)
		 {
       Log::log(0, "ABM查询帐户%ld实时费用明细异常,错误代码:%d", lAcctID, iRet);
       iRet = iRet>0 ? iRet*-1:iRet;
		   return iRet;
		 }
	else
		 {
		   qaiResult.getRecord(vecAcctItemInfoHB);
		   return 0;
		 }	   
}

/***************************************************************************
*函数名:bool transBillingMode();
*功能：用户预后付发生转变通知abm
***************************************************************************/
int AbmInterfaceManage::transBillingMode(long lAcctID, long lServID, long lTypeID)
{
  stCmd.set(lAcctID, lServID, lTypeID);
  
  pInst->beginTrans();
  int iRet = pInst->synExecute(&stCmd, &stResult);
  if (iRet != 0)
  	 {
       Log::log(0, "帐户%ld,用户%ld,付费方式%ld转变通知ABM发生异常,错误代码:%d", lAcctID, lServID, lTypeID, iRet);
		   iRet = iRet>0 ? iRet*-1:iRet;
		   return iRet;
  	 }
  
  iRet = stResult.getResultCode();
  if (iRet != 0)
  	 {
       Log::log(0, "帐户%ld,用户%ld,付费方式%ld转变通知ABM发生异常,错误代码:%d", lAcctID, lServID, lTypeID, iRet);
		   iRet = iRet>0 ? iRet*-1:iRet;
		   return iRet;
  	 }
  else
  	 {
  	   return 0;
  	 }	 
}

    //过户通知abm,0表示成功
int AbmInterfaceManage::modifyAcctOld(long lAcctID, long lServID, long lTransID, int iBillingCycleID, vector<AcctAggrHB01 *> &vecData)
{
  maoCmd.set(lAcctID, lServID, lTransID, iBillingCycleID);
  
  pInst->beginTrans();
  int iRet = pInst->synExecute(&maoCmd, &maoResult);
  if (iRet != 0)
  	 {
       Log::log(0, "帐户%ld,用户%ld,TransID%ld过户通知ABM发生异常,错误代码:%d", lAcctID, lServID, lTransID, iRet);
		   iRet = iRet>0 ? iRet*-1:iRet;
		   return iRet;
  	 }
  
  iRet = maoResult.getResultCode();
  if (iRet != 0)
  	 {
       Log::log(0, "帐户%ld,用户%ld,TransID%ld过户通知ABM发生异常,错误代码:%d", lAcctID, lServID, lTransID, iRet);
		   iRet = iRet>0 ? iRet*-1:iRet;
		   return iRet;
  	 }
  else
  	 {
  	   maoResult.getRecord(vecData);
  	   return 0;
  	 }	 
}

int AbmInterfaceManage::modifyAcctNew(long lAcctID, long lServID, long lTransID, int iBillingCycleID, int iBillingMode, vector<AcctAggrHB01 *> &vecData)
{
  manCmd.set(lAcctID, lServID, lTransID, iBillingCycleID, iBillingMode, vecData);
  
  pInst->beginTrans();
  int iRet = pInst->synExecute(&manCmd, &manResult);
  if (iRet != 0)
  	 {
       Log::log(0, "帐户%ld,用户%ld,TransID%ld过户通知ABM发生异常,错误代码:%d", lAcctID, lServID, lTransID, iRet);
		   iRet = iRet>0 ? iRet*-1:iRet;
		   return iRet;
  	 }
  	 
  iRet = manResult.getResultCode();
  if (iRet != 0)
  	 {
       Log::log(0, "帐户%ld,用户%ld,TransID%ld过户通知ABM发生异常,错误代码:%d", lAcctID, lServID, lTransID, iRet);
		   iRet = iRet>0 ? iRet*-1:iRet;
		   return iRet;
  	 }
  else
  	 {
  	   return 0;
  	 }	 
}

/***************************************************************************
*函数名:bool dealacct_item_withoutabm();
*功能：处理账单
***************************************************************************/
bool AbmInterfaceManage::dealacct_item_withoutabm(char const *sTable)
{
  char sSQL[4096];
  char sDate[18]={0};
  Date d;
  strcpy(sDate, d.toString());

  sprintf(sSQL, "select acct_item_id, serv_id, acct_id, offer_inst_id,"
                "billing_cycle_id, charge, measure_type, acct_item_type_id,"
                "item_source_id, state_date, nvl(item_class_id, 0) "
                "from %s", sTable);
  DEFINE_QUERY(qry);

  unsigned int i;
  AcctItemData  p;
  int j(0);

  Log::log(0, "开始加载%s,当前时间 %s", sTable, sDate);

  qry.setSQL(sSQL); 
  qry.open();
  while(qry.next()) 
       {        
         p.m_lAcctItemID     = qry.field(0).asLong();
         p.m_lServID         = qry.field(1).asLong();
         p.m_lAcctID         = qry.field(2).asLong();
         p.m_lOfferInstID    = qry.field(3).asLong();
         p.m_iBillingCycleID = qry.field(4).asInteger();
         p.m_lValue          = qry.field(5).asLong();
         p.m_iPayMeasure     = qry.field(6).asInteger();
         p.m_iPayItemTypeID  = qry.field(7).asInteger();
         p.m_iItemSourceID   = qry.field(8).asInteger();
         strcpy (p.m_sStateDate, qry.field(9).asString());
         p.m_iItemClassID = qry.field(10).asInteger();
         j++;
         if (!(j%300000)) {
            Log::log(0, "\t已处理了%d\t条", j);
         }
       }
  qry.close();

  char sDate1[18]={0};
  Date d1;
  strcpy(sDate1, d1.toString());
  Log::log(0, "\t已处理了%d\t条,当前时间是%s", j, sDate1);

  return true;
}

/***************************************************************************
*函数名:bool dealacct_item_withabm();
*功能：处理账单调用abm服务
***************************************************************************/
bool AbmInterfaceManage::dealacct_item_withabm(char const *sTable)
{
  char sSQL[4096];
  char sDate[18]={0};
  Date d;
  int m_iSysPID;
  m_iSysPID = getpid();
  strcpy(sDate,d.toString());
  sprintf(sSQL, "select acct_item_id, serv_id, acct_id, offer_inst_id,"
                "billing_cycle_id, charge, measure_type, acct_item_type_id,"
                "item_source_id, state_date, nvl(item_class_id, 0) "
                "from %s", sTable);
  DEFINE_QUERY(qry);
  unsigned int i;
  AcctItemData  p;
  int j (0);
    
  Log::log(0, "deal with abm 开始%s,当前时间 %s pid %d", sTable, sDate, m_iSysPID);

  vector<AcctItemData> vBuf;
  vBuf.resize (20001);
    
  DeductBalanceR oData;
  vector<DeductAcctItem*> vecBal;

  DeductAcctItem ss ;
  vecBal.push_back(&ss);

  qry.setSQL(sSQL); 
  qry.open();
  while(qry.next()) 
       {
         p.m_lAcctItemID = qry.field (0).asLong ();
         p.m_lServID     = qry.field (1).asLong ();
         p.m_lAcctID     = qry.field (2).asLong ();
         p.m_lOfferInstID   = qry.field (3).asLong ();
         p.m_iBillingCycleID = qry.field (4).asInteger ();
         p.m_lValue      = qry.field (5).asLong ();
         p.m_iPayMeasure = qry.field (6).asInteger ();
         p.m_iPayItemTypeID  = qry.field (7).asInteger ();
         p.m_iItemSourceID = qry.field (8).asInteger ();
         strcpy (p.m_sStateDate, qry.field (9).asString ());
         p.m_iItemClassID = qry.field (10).asInteger ();
         j++;
         vBuf.push_back(p);
         if (j%20000 == 0) 
         	  {
              Log::log(0, "\t 已经加载 %d\t条 pid %d", j, m_iSysPID);
              for(int i=0; i<vBuf.size(); i++) 
                 {
                   oData.m_iBillingCycleID =p.m_iBillingCycleID;
                   oData.m_lAcctID= p.m_lAcctID;
                   oData.m_lServID = p.m_lServID;
                   oData.m_iFileID = 1000 +j;
                   oData.m_iBillingMode = 2;
                   oData.m_iOffset = i;

                   ss.m_iItemSourceID=p.m_iItemSourceID;
                   ss.m_iOfferID=8888;
                   ss.m_lCharge =p.m_lValue;
                   ss.m_lOfferInstID = p.m_lOfferInstID;
                   ss.m_uiAcctItemTypeID = p.m_iPayItemTypeID;

                   pachCharge(oData, vecBal);
                   transCharge();
                 }
              vBuf.clear();
              Log::log(0, "\t 已经处理 %d\t条 pid %d", j, m_iSysPID);
            }
       }
  qry.close ();

  Log::log(0, "\t 已经加载 %d\t条 pid %d", j, m_iSysPID);
  for(int i=0; i<vBuf.size(); i++) 
     {
       oData.m_iBillingCycleID =p.m_iBillingCycleID;
       oData.m_lAcctID= p.m_lAcctID;
       oData.m_lServID = p.m_lServID;
       oData.m_iFileID = 1000 +j;
       oData.m_iBillingMode = 2;
       oData.m_iOffset = i;

       ss.m_iItemSourceID=p.m_iItemSourceID;
       ss.m_iOfferID=8888;
       ss.m_lCharge =p.m_lValue;
       ss.m_lOfferInstID = p.m_lOfferInstID;
       ss.m_uiAcctItemTypeID = p.m_iPayItemTypeID;
 
       pachCharge(oData, vecBal);
       transCharge();
     }
  vBuf.clear();
  Log::log(0, "\t 已经处理 %d\t条 pid %d", j, m_iSysPID);
  char sDate1[18]={0};
  Date d1;
  strcpy(sDate1, d1.toString());
  Log::log(0, "\t已经通过abm处理了%d\t条,当前时间是%s pid %d", j,sDate1, m_iSysPID);
  return true;
}
#endif

//主函数
/*
int main(int argc, char **argv)
{
  AbmInterfaceManage tt;
  switch(argv[1][0] | 0x20) 
        {
          case 'a':
            tt.dealacct_item_withabm("acct_item_aggr_201009");
            break;
          case 'n':
            tt.dealacct_item_withoutabm("acct_item_aggr_201009");
            break;
          default:
            Log::log(0,    "\n********************************************************************\n"
                            " 调用方法错误\n"
                            "********************************************************************\n"
                     );
        }
}
*/





