#include "PublicConndb.h"
TimesTenCMD *m_poPublicConn;//查询
TimesTenCMD *m_pInsertPayout;//支出
TimesTenCMD *m_pInsertPayment;//记录业务操作
TimesTenCMD *m_pInsertRela;//记录支出来源关系
TimesTenCMD *m_pUpdateBalance;//更新账本
TimesTenCMD *m_pUpdateSource;//更新来源
TimesTenCMD *m_pInsertServInfo;//serv
TimesTenCMD *m_pInsertAcctInfo;//acct
TimesTenCMD *m_pInsertServAcctInfo;//serv_acct
TimesTenCMD *m_pInsertAcctBalance;//acct_balance
TimesTenCMD *m_pInsertBalanceSource;//balance_source
TimesTenCMD *m_pInsertUserInfoADD;//user_info_add
TimesTenCMD *m_pInsertAttUserInfo;//user_info_add
TimesTenCMD *m_pInsertAppUserInfo;//user_info_add

TimesTenCMD *m_pBalanceTransReset;//更新划拨规则
TimesTenCMD *m_pPasswordResetUpdate;//支付密码修改
TimesTenCMD *m_pPasswordResetDelete;//支付密码修改
TimesTenCMD *m_pPasswordResetInsert;//支付密码修改
TimesTenCMD *m_pPayStatusResetUpdate;//支付密码修改
TimesTenCMD *m_pPayRulesReset;//支付规则修改
TimesTenCMD *m_pTransferRulesInsert;//划拨支付规则入库


TimesTenCMD *m_pUpdatePayoutState; // 更新支出记录状态
TimesTenCMD *m_pUpdateRealState;   // 更新支出来源关系状态
TimesTenCMD *m_pUpdateOperState;   // 更新业务流水记录状态
TimesTenCMD *m_pRUpdateBalance;// 更新余额帐本-余额划拨冲正

TimesTenCMD *m_pInsertServHis;//记录SERV历史信息表
TimesTenCMD *m_pInsertAcctHis;//记录ACCT历史信息表
TimesTenCMD *m_pInsertServAcctHis;//记录SERV_ACCT历史信息表
TimesTenCMD *m_pDeleteAcctBalance;//删除余额账单余额为0数据
TimesTenCMD *m_pDeleteServInfo;//删除SERV表信息
TimesTenCMD *m_pDeleteAcctInfo;//删除ACCT表信息
TimesTenCMD *m_pDeleteServAcctInfo;//删除SERV_ACCT表信息
TimesTenCMD *m_pChangeInservHis;//换号用记录SERV历史信息表
TimesTenCMD *m_pChangeNbr;//换号
TimesTenCMD *m_pChangeAcct;//过户创建ACCT_ID
TimesTenCMD *m_pUpdateServAcct;//过户更改SERV_ACCT

TimesTenCMD *m_pUpdateServAddState;	//用户信息增量表同步状态更新
TimesTenCMD *m_pInsertSendSql;	//写短信工单表

TimesTenCMD *m_pSavePlatformCCR; // 保存自服务平台CCR信息
TimesTenCMD *m_pSavePlatformCCRBiz; // 保存自服务平台CCR业务信息
TimesTenCMD *m_pSaveDcc;// 保存DCC消息
TimesTenCMD *m_pUpdateDcc;// 更新DCC消息

TimesTenCMD *m_pLoginSaveDcc;//登陆密码鉴权
TimesTenCMD *m_pPasswdChangeDcc; //登陆密码修改

TimesTenCMD *m_pGetSeq;//取循环序列
TimesTenCMD *m_pInserUserAddErr;	//资料同步增量表记录错误码

TimesTenCMD *m_pSaveActiveCCR;
TimesTenCMD *m_pUpdateCCR;

TTConnection *TTConn;
using namespace std;

PublicConndb::PublicConndb()
{
	m_poPublicConn = NULL;    
	m_pInsertPayout=NULL;//支出
	m_pInsertPayment=NULL;//记录业务操作
	m_pInsertRela=NULL;//记录支出来源关系
	m_pUpdateBalance=NULL;//更新账本
	m_pUpdateSource=NULL;//更新来源
	
	m_pBalanceTransReset=NULL;//更新划拨规则
	m_pPasswordResetUpdate=NULL;//支付密码修改
	m_pPasswordResetDelete=NULL;//支付密码修改
	m_pPasswordResetInsert=NULL;//支付密码修改
	m_pPayStatusResetUpdate=NULL;//支付密码修改
	m_pPayRulesReset=NULL;//支付规则修改
	m_pTransferRulesInsert=NULL;//划拨支付规则入库
	
	m_pUpdatePayoutState=NULL;// 更新支出记录状态
	m_pUpdateRealState=NULL;// 更新支出来源关系状态
	m_pInsertServInfo=NULL;//serv
	m_pInsertAcctInfo=NULL;//acct
	m_pInsertServAcctInfo=NULL;//serv_acct
	m_pInsertAcctBalance=NULL;//acct_balance
	m_pInsertBalanceSource=NULL;//balance_source
	m_pInsertAttUserInfo=NULL;//att_user_info
	m_pUpdateOperState=NULL;//业务流水记录状态更新
	m_pRUpdateBalance=NULL;
	m_pInsertAppUserInfo=NULL;//att_user_info
	
	m_pInsertServHis=NULL;
	m_pInsertAcctHis=NULL;
	m_pInsertServAcctHis=NULL;
	m_pDeleteAcctBalance=NULL;
	m_pDeleteServInfo=NULL;
	m_pDeleteAcctInfo=NULL;
	m_pDeleteServAcctInfo=NULL;
	m_pChangeInservHis=NULL;
	m_pChangeNbr=NULL;
	m_pChangeAcct=NULL;//过户
	m_pUpdateServAcct=NULL;//过户
	
	m_pUpdateServAddState=NULL;
	m_pInsertSendSql=NULL;
	
	m_pSavePlatformCCR = NULL; // 保存自服务平台请求CCR信息
	m_pSavePlatformCCRBiz = NULL; // 保存自服务平台请求CCR业务信息
	m_pSaveDcc = NULL;// 保存DCC消息
	
	m_pLoginSaveDcc=NULL;
	m_pPasswdChangeDcc=NULL;
	m_pGetSeq=NULL;
	m_pInserUserAddErr=NULL;
	
	m_pUpdateDcc=NULL;// 更新DCC消息
	m_pSaveActiveCCR=NULL;
	m_pUpdateCCR=NULL;
}

PublicConndb::~PublicConndb()
{
    FreeObject(m_poPublicConn);
    FreeObject(m_pInsertPayout);
    FreeObject(m_pInsertPayment);
    FreeObject(m_pInsertRela);
    FreeObject(m_pUpdateBalance);
    FreeObject(m_pUpdateSource);
    FreeObject(m_pUpdatePayoutState);
    FreeObject(m_pUpdateRealState);
    
    FreeObject(m_pBalanceTransReset);
	FreeObject(m_pPasswordResetUpdate);
	FreeObject(m_pPasswordResetDelete);
	FreeObject(m_pPasswordResetInsert);
	FreeObject(m_pPayStatusResetUpdate);
	FreeObject(m_pPayRulesReset);
	FreeObject(m_pTransferRulesInsert);

    FreeObject(m_pInsertServInfo);
    FreeObject(m_pInsertAcctInfo);
    FreeObject(m_pInsertServAcctInfo);
    FreeObject(m_pInsertAcctBalance);
    FreeObject(m_pInsertBalanceSource);
    FreeObject(m_pInsertAttUserInfo);
    FreeObject(m_pUpdateOperState);
    FreeObject(m_pInsertAppUserInfo);

    FreeObject(m_pInsertServHis);
    FreeObject(m_pInsertAcctHis);
    FreeObject(m_pInsertServAcctHis);
    FreeObject(m_pDeleteAcctBalance);
    FreeObject(m_pDeleteServInfo);
    FreeObject(m_pDeleteAcctInfo);
    FreeObject(m_pDeleteServAcctInfo);
    FreeObject(m_pChangeInservHis);
    FreeObject(m_pChangeNbr);
    FreeObject(m_pChangeAcct);
    FreeObject(m_pUpdateServAcct);
    
    FreeObject(m_pUpdateServAddState);
    FreeObject(m_pInsertSendSql);
    
    FreeObject(m_pSavePlatformCCR);
    FreeObject(m_pSavePlatformCCRBiz);
    FreeObject(m_pSaveDcc);
    
    FreeObject(m_pLoginSaveDcc);
    FreeObject(m_pPasswdChangeDcc);
    FreeObject(m_pGetSeq);
    FreeObject(m_pInserUserAddErr);
    
    FreeObject(m_pUpdateDcc);
    FreeObject(m_pSaveActiveCCR);
    FreeObject(m_pUpdateCCR);
}
int PublicConndb::init(ABMException &oExp)
{
  string sql1,sql2,sql3,sql4,sql5,sql6,sql7,sql8,sql9; 
  string sql10,sql11,sql12,sql13,sql14,sql15,sql16;
  string sql24,sql25;
  string sql31,sql32,sql33,sql34,sql35,sql36,sql37,sql38,sql39;
  string sql40,sql41,sql42,sql43,sql44,sql45,sql46;
  string sql50,sql51;
  string sql52,sql53;
  string sql54,sql55,sql56,sql57;
  string sql60,sql61,sql62,sql63,sql64,sql65,sql66;
  try
  {
	if (GET_CURSOR(m_poPublicConn, m_poTTConn, m_sConnTns, oExp) != 0) 
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
        }
	TTConn = m_poTTConn->getDBConn(true);
	if (TTConn == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
 	
	m_pInsertPayout = new TimesTenCMD(TTConn);
	if (m_pInsertPayout == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	
	//划拨规则设置
	m_pBalanceTransReset = new TimesTenCMD(TTConn);
	if (m_pBalanceTransReset == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql60.clear();
	sql60 ="  update TRANSFER_RULES A set A.DATA_TRANS_FREQ=:p0, A.TRANS_LIMIT=:p1 ,A.MONTH_TRANS_LIMIT=:p2,A.AUTO_TRANS_FLAG=:p3,A.AUTO_TRANS_THRD=:p4,A.AUTO_TRANS_AMOUNT=:p5 where acct_id in(select C.acct_id from serv B,serv_acct C where B.acc_nbr=:p6 and B.serv_id=C.serv_id)";
       	m_pBalanceTransReset->Prepare(sql60.c_str());
	m_pBalanceTransReset->Commit();
	
	//支付密码修改
	m_pPasswordResetUpdate = new TimesTenCMD(TTConn);
	if (m_pPasswordResetUpdate == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql61.clear();
	sql61 ="  update ATT_USER_INFO a set a.ATTR_VALUES=:p0 where a.serv_id in(select b.serv_id from serv b where b.acc_nbr=:p1) and a.ATTR_TYPE='04'";
       	m_pPasswordResetUpdate->Prepare(sql61.c_str());
	m_pPasswordResetUpdate->Commit();
	
		//支付密码删除
	m_pPasswordResetDelete = new TimesTenCMD(TTConn);
	if (m_pPasswordResetDelete == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql62.clear();
	sql62 ="delete from ATT_USER_INFO a where a.serv_id in(select b.serv_id from serv b where b.acc_nbr=:p0) and a.ATTR_TYPE='04'";
       	m_pPasswordResetDelete->Prepare(sql62.c_str());
	m_pPasswordResetDelete->Commit();
	
		//支付密码新建
	m_pPasswordResetInsert = new TimesTenCMD(TTConn);
	if (m_pPasswordResetInsert == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql63.clear();
	sql63 ="  INSERT INTO ATT_USER_INFO (SERV_ID,ATTR_VALUES,ATTR_TYPE) VALUES (:p0,:p1, '04')";
       	m_pPasswordResetInsert->Prepare(sql63.c_str());
	m_pPasswordResetInsert->Commit();
	
		//支付状态变更
	m_pPayStatusResetUpdate = new TimesTenCMD(TTConn);
	if (m_pPayStatusResetUpdate == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql64.clear();
	sql64 =" update serv set PAY_STATUS=:p0 where acc_nbr=:p1";
       	m_pPayStatusResetUpdate->Prepare(sql64.c_str());
	m_pPayStatusResetUpdate->Commit();
	
	//支付规则设置
	m_pPayRulesReset = new TimesTenCMD(TTConn);
	if (m_pPayRulesReset == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql65.clear();
	sql65 ="  update TRANSFER_RULES A set A.DATA_PAY_FREQ=:p0, A.PAY_LIMIT=:p1 ,A.MONTH_PAY_LIMIT=:p2,A.MICRO_PAY_LIMIT=:p3 where acct_id in(select C.acct_id from serv B,serv_acct C where B.acc_nbr=:p4 and B.serv_id=C.serv_id)";
       	m_pPayRulesReset->Prepare(sql65.c_str());
	m_pPayRulesReset->Commit();
	
		//支付划拨规则入库
	m_pTransferRulesInsert = new TimesTenCMD(TTConn);
	if (m_pTransferRulesInsert == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql66.clear();
	sql66 ="  INSERT INTO TRANSFER_RULES (ACCT_ID,DATA_TRANS_FREQ,TRANS_LIMIT,MONTH_TRANS_LIMIT,AUTO_TRANS_FLAG,"
		"AUTO_TRANS_THRD,AUTO_TRANS_AMOUNT,DATA_PAY_FREQ,PAY_LIMIT,MONTH_PAY_LIMIT,MICRO_PAY_LIMIT ) "
		"VALUES (:p0,:p1,:p2,:p3,:p4,:p5,:p6,:p7,:p8,:p9,:p10)";
       	m_pTransferRulesInsert->Prepare(sql66.c_str());
	m_pTransferRulesInsert->Commit();
	
	//支付
	sql1.clear();
	sql1="  INSERT INTO BALANCE_PAYOUT ( OPER_PAYOUT_ID, ACCT_BALANCE_ID,BILLING_CYCLE_ID,OPT_ID, OPER_TYPE,OPER_DATE,AMOUNT, BALANCE,PRN_COUNT,STATE,STATE_DATE,STAFF_ID)VALUES(:OPER_PAYOUT_ID,:ACCT_BALANCE_ID,TO_NUMBER(TO_CHAR(SYSDATE,'YYYYMM')),:OPT_ID,:OPER_TYPE,SYSDATE,:AMOUNT,:BALANCE,:PRN_COUNT,'00A',SYSDATE,:STAFF_ID)";
	m_pInsertPayout->Prepare(sql1.c_str());
	m_pInsertPayout->Commit();

	m_pInsertPayment = new TimesTenCMD(TTConn);
	if (m_pInsertPayment == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql2.clear();
	sql2="INSERT INTO OPERATION(OPT_ID,FOREIGN_ID,CAPABILITY_CODE,SERV_ID,REC_TYPE,RBK_OPT_ID,OPT_DATE,SP_ID,SERVICE_PLATFORM_ID,ORDER_INFO,ORDER_ID,ORDER_DESC,ORDER_STATE,PAY_TYPE,AMOUNT,STATE_DATE) values(:p1,:p2,:p3,:p4,'1',:p5,sysdate,:p6,:p7,:p8,:p9,:p10,:p11,:p12,:p13,SYSDATE)";
	m_pInsertPayment->Prepare(sql2.c_str());
	m_pInsertPayment->Commit();

	m_pInsertRela = new TimesTenCMD(TTConn);
	if (m_pInsertRela == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql3.clear();
	sql3="INSERT INTO BALANCE_SOURCE_PAYOUT_RELA( BALANCE_SOURCE_PAYOUT_RELA_ID, OPER_INCOME_ID,OPER_PAYOUT_ID,ACCT_BALANCE_ID, PAYOUT_AMOUNT, BILLING_CYCLE_ID, STATE,STATE_DATE, OLD_SOURCE_AMOUNT)VALUES (:BALANCE_SOURCE_PAYOUT_RELA_ID, :OPER_INCOME_ID, :OPER_PAYOUT_ID, :ACCT_BALANCE_ID,:PAYOUT_AMOUNT, TO_NUMBER(TO_CHAR(SYSDATE,'YYYYMM')), '00A', SYSDATE,   :OLD_SOURCE_AMOUNT)";
	m_pInsertRela->Prepare(sql3.c_str());
	m_pInsertRela->Commit();

	m_pUpdateBalance = new TimesTenCMD(TTConn);
	if (m_pUpdateBalance == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql4.clear();
	sql4 ="  UPDATE ACCT_BALANCE A  SET A.BALANCE = :BALANCE, A.STATE_DATE = SYSDATE WHERE A.ACCT_BALANCE_ID = :ACCT_BALANCE_ID";
       	m_pUpdateBalance->Prepare(sql4.c_str());
	m_pUpdateBalance->Commit();

	/*
	m_pInsertPayout = new TimesTenCMD(TTConn);
	if (m_pInsertPayout == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	
	sql5.clear();
	sql5 ="  INSERT INTO BALANCE_PAYOUT   ( OPER_PAYOUT_ID,      ACCT_BALANCE_ID,      BILLING_CYCLE_ID,        OPT_ID,  OPER_TYPE,  OPER_DATE,   AMOUNT,   BALANCE,   PRN_COUNT, STATE,STATE_DATE,     STAFF_ID)    VALUES  (:OPER_PAYOUT_ID, :ACCT_BALANCE_ID, TO_NUMBER(TO_CHAR(SYSDATE,'YYYYMM')), :OPT_ID, '5UR',  SYSDATE, :AMOUNT,  :BALANCE, :PRN_COUNT, '00A',SYSDATE, :STAFF_ID)";
	m_pInsertPayout->Prepare(sql5.c_str());
	m_pInsertPayout->Commit();
	*/

	m_pUpdateSource = new TimesTenCMD(TTConn);
	if (m_pUpdateSource == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql6.clear();
	sql6 =" UPDATE BALANCE_SOURCE A SET A.BALANCE=:BALANCE,A.STATE_DATE=SYSDATE WHERE A.OPER_INCOME_ID=:OPER_INCOME_ID";
	m_pUpdateSource->Prepare(sql6.c_str());
	m_pUpdateSource->Commit();

	m_pUpdatePayoutState = new TimesTenCMD(TTConn);
	if (m_pUpdatePayoutState == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql7.clear();
	sql7 =" UPDATE BALANCE_PAYOUT A SET A.STATE='00X',A.STATE_DATE=sysdate WHERE A.OPER_PAYOUT_ID=:p0";
	m_pUpdatePayoutState->Prepare(sql7.c_str());
	m_pUpdatePayoutState->Commit();

	m_pUpdateRealState = new TimesTenCMD(TTConn);
	if (m_pUpdateRealState == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	
	sql8.clear();
	sql8 =" UPDATE BALANCE_SOURCE_PAYOUT_RELA A SET A.STATE='00X',A.STATE_DATE=sysdate WHERE A.OPER_PAYOUT_ID=:p0";
	m_pUpdateRealState->Prepare(sql8.c_str());
	m_pUpdateRealState->Commit();
	
	//充值开户
	m_pInsertServInfo= new TimesTenCMD(TTConn);
	if (m_pInsertServInfo == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql9.clear();
	sql9 =" INSERT INTO SERV (SERV_ID, AGREEMENT_ID, CUST_ID, PRODUCT_ID, CYCLE_TYPE_ID, CREATE_DATE, RENT_DATE, STATE, STATE_DATE, ACC_NBR, USER_TYPE, USER_ATTR, PAY_TYPE, PAY_STATUS, BILLING_MODE_ID, FREE_TYPE_ID, NSC_DATE) VALUES (:SERV_ID, 1, :CUST_ID, 1, TO_NUMBER(TO_CHAR(SYSDATE, 'YYYYMM')), SYSDATE, SYSDATE, '2HA', SYSDATE, :ACC_NBR, :USER_TYPE, :USER_ATTR, '0', '0', 1, 0, SYSDATE)";
	m_pInsertServInfo->Prepare(sql9.c_str());
	m_pInsertServInfo->Commit();

	m_pInsertAcctInfo= new TimesTenCMD(TTConn);
	if (m_pInsertAcctInfo == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql10.clear();
	sql10 =" INSERT INTO ACCT (ACCT_ID, CUST_ID, ACCT_NAME, ACCT_TYPE, ADDRESS_ID, STATE, STATE_DATE, ACCT_NBR, ABM_FLAG, BATCH_BILLING_CYCLE_ID) VALUES (:ACCT_ID, :CUST_ID, null, null, 0, '10A', SYSDATE, :ACCT_NBR, null, TO_NUMBER(TO_CHAR(SYSDATE, 'YYYYMM')))";
	m_pInsertAcctInfo->Prepare(sql10.c_str());
	m_pInsertAcctInfo->Commit();

	m_pInsertServAcctInfo= new TimesTenCMD(TTConn);
	if (m_pInsertServAcctInfo == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql11.clear();
	sql11 =" INSERT INTO SERV_ACCT (SERV_ACCT_ID, ACCT_ID, SERV_ID, ITEM_GROUP_ID, PAYMENT_RULE_ID, STATE, STATE_DATE, CREATED_DATE, BILL_REQUIRE_ID, INVOICE_REQUIRE_ID) VALUES (:SERV_ACCT_ID, :ACCT_ID, :SERV_ID, 1, 1, '10A', SYSDATE, SYSDATE, null, null)";
	m_pInsertServAcctInfo->Prepare(sql11.c_str());
	m_pInsertServAcctInfo->Commit();

	m_pInsertAcctBalance= new TimesTenCMD(TTConn);
	if (m_pInsertAcctBalance == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql12.clear();
	sql12 =" INSERT INTO ACCT_BALANCE (ACCT_BALANCE_ID, ACCT_ID, BALANCE_TYPE_ID, EFF_DATE, EXP_DATE, BALANCE, CYCLE_UPPER, CYCLE_LOWER, CYCLE_UPPER_TYPE, CYCLE_LOWER_TYPE, SERV_ID, ITEM_GROUP_ID, STATE, STATE_DATE, OBJECT_TYPE_ID, BANK_ACCT) VALUES ( :ACCT_BALANCE_ID, :ACCT_ID, :BALANCE_TYPE_ID, TO_DATE(:EFF_DATE,'YYYYMMDDHH24MISS'), TO_DATE(:EXP_DATE,'YYYYMMDDHH24MISS'), :BALANCE, null, null, '', '', null, null, '10A', SYSDATE, null, '')";
	m_pInsertAcctBalance->Prepare(sql12.c_str());
	m_pInsertAcctBalance->Commit();

	m_pInsertBalanceSource= new TimesTenCMD(TTConn);
	if (m_pInsertBalanceSource == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql13.clear();
	sql13 =" INSERT INTO BALANCE_SOURCE (OPER_INCOME_ID, ACCT_BALANCE_ID, OPER_TYPE, OPER_DATE, AMOUNT, SOURCE_TYPE, SOURCE_DESC, STATE, STATE_DATE, OPT_ID, BALANCE, ALLOW_DRAW, INV_OFFER, CURR_STATE, CURR_DATE, REMAIN_AMOUNT, STAFF_ID, ACCT_BALANCE_OBJ_ID, EFF_DATE_OFFSET, EXP_DATE_OFFSET) VALUES (:OPER_INCOME_ID, :ACCT_BALANCE_ID, :OPER_TYPE, sysdate, :AMOUNT, :SOURCE_TYPE, :SOURCE_DESC, :STATE, sysdate, :OPT_ID, :BALANCE, :ALLOW_DRAW, '', '', '', '', null, '', null, null)";
	m_pInsertBalanceSource->Prepare(sql13.c_str());
	m_pInsertBalanceSource->Commit();

	m_pInsertUserInfoADD= new TimesTenCMD(TTConn);
	if (m_pInsertUserInfoADD == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql14.clear();
	sql14 =" INSERT INTO USER_INFO_ADD (SERV_ID, AGREEMENT_ID, CUST_ID, PRODUCT_ID, BILLING_CYCLE_TYPE_ID, PRODUCT_FAMILY_ID, CREATE_DATE, RENT_DATE, COMPLETED_DATE, STATE, STATE_DATE, USER_TYPE, USER_ATTR, PAY_TYPE, ACCT_ID, USER_LOGIN_ID) VALUES (:SERV_ID, :AGREEMENT_ID, :CUST_ID, :PRODUCT_ID, :BILLING_CYCLE_TYPE_ID, :PRODUCT_FAMILY_ID, SYSDATE, SYSDATE, SYSDATE, '10A', SYSDATE, :USER_TYPE, :USER_ATTR, :PAY_TYPE, :ACCT_ID, :USER_LOGIN_ID)";
	m_pInsertUserInfoADD->Prepare(sql14.c_str());
	m_pInsertUserInfoADD->Commit();

	// sql15
	m_pUpdateOperState = new TimesTenCMD(TTConn);
	if (m_pUpdateOperState == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql15.clear();
	sql15 =" UPDATE OPERATION A SET A.ORDER_STATE='C0R',A.STATE_DATE=sysdate WHERE A.OPT_ID=:p0";
	m_pUpdateOperState->Prepare(sql15.c_str());
	m_pUpdateOperState->Commit();
	
	// sql16
	m_pRUpdateBalance = new TimesTenCMD(TTConn);
	if (m_pRUpdateBalance == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql16.clear();
	sql16 ="  UPDATE ACCT_BALANCE A  SET A.BALANCE = A.BALANCE+:BALANCE, A.STATE_DATE = SYSDATE WHERE A.ACCT_BALANCE_ID = :ACCT_BALANCE_ID";
       	m_pRUpdateBalance->Prepare(sql16.c_str());
	m_pRUpdateBalance->Commit();
	
	//资料同步sql31~sql39
	m_pInsertServHis = new TimesTenCMD(TTConn);
	if (m_pInsertServHis == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
  	}
  	sql31.clear();
	sql31="INSERT INTO SERV_HIS(SERV_ID,AGREEMENT_ID,CUST_ID,PRODUCT_ID,CYCLE_TYPE_ID,CREATE_DATE,RENT_DATE,STATE,STATE_DATE,ACC_NBR,USER_TYPE,USER_ATTR,PAY_TYPE,PAY_STATUS,BILLING_MODE_ID,FREE_TYPE_ID,NSC_DATE) SELECT SERV_ID,AGREEMENT_ID,CUST_ID,PRODUCT_ID,CYCLE_TYPE_ID,CREATE_DATE,RENT_DATE,:p0,STATE_DATE,ACC_NBR,USER_TYPE,USER_ATTR,PAY_TYPE,PAY_STATUS,BILLING_MODE_ID,FREE_TYPE_ID,NSC_DATE FROM SERV WHERE ACC_NBR=:p1";
	m_pInsertServHis->Prepare(sql31.c_str());
	m_pInsertServHis->Commit();
	  
	  m_pInsertAcctHis = new TimesTenCMD(TTConn);
	if (m_pInsertAcctHis == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
  	}
  	sql32.clear();
	sql32="INSERT INTO ACCT_HIS(ACCT_ID,CUST_ID,ACCT_NAME,ACCT_TYPE,ADDRESS_ID,STATE,STATE_DATE,ACCT_NBR,ABM_FLAG,BATCH_BILLING_CYCLE_ID) SELECT ACCT_ID,CUST_ID,ACCT_NAME,ACCT_TYPE,ADDRESS_ID,:p0,STATE_DATE,ACCT_NBR,ABM_FLAG,BATCH_BILLING_CYCLE_ID FROM ACCT WHERE ACCT_NBR=:p1";
	m_pInsertAcctHis->Prepare(sql32.c_str());
	m_pInsertAcctHis->Commit();
	  
	m_pInsertServAcctHis = new TimesTenCMD(TTConn);
	if (m_pInsertServAcctHis == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
  	}
  	sql33.clear();
  	sql33="INSERT INTO SERV_ACCT_HIS(SERV_ACCT_ID,ACCT_ID,SERV_ID,ITEM_GROUP_ID,PAYMENT_RULE_ID,STATE,STATE_DATE,CREATED_DATE,BILL_REQUIRE_ID,INVOICE_REQUIRE_ID) SELECT SERV_ACCT_ID,ACCT_ID,SERV_ID,ITEM_GROUP_ID,PAYMENT_RULE_ID,:p0,STATE_DATE,CREATED_DATE,BILL_REQUIRE_ID,INVOICE_REQUIRE_ID FROM SERV_ACCT WHERE SERV_ID=(SELECT SERV_ID FROM SERV WHERE ACC_NBR=:p1)";
  	m_pInsertServAcctHis->Prepare(sql33.c_str());
  	m_pInsertServAcctHis->Commit();
  
  	m_pDeleteAcctBalance = new TimesTenCMD(TTConn);
	if (m_pDeleteAcctBalance == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
  	}
  	sql34.clear();
  	sql34="DELETE FROM ACCT_BALANCE WHERE SERV_ID=(SELECT SERV_ID FROM SERV WHERE BALANCE=0 AND ACC_NBR=:p0)";
  	m_pDeleteAcctBalance->Prepare(sql34.c_str());
  	m_pDeleteAcctBalance->Commit();
  
  	m_pDeleteServInfo = new TimesTenCMD(TTConn);
	if (m_pDeleteServInfo == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
  	}
  	sql35.clear();
  	sql35="DELETE FROM SERV WHERE ACC_NBR=:p0";
  	m_pDeleteServInfo->Prepare(sql35.c_str());
  	m_pDeleteServInfo->Commit();
  
  	m_pDeleteAcctInfo = new TimesTenCMD(TTConn);
	if (m_pDeleteAcctInfo == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
  	}
  	sql36.clear();
  	sql36="DELETE FROM ACCT WHERE ACCT_NBR=:p0";
  	m_pDeleteAcctInfo->Prepare(sql36.c_str());
  	m_pDeleteAcctInfo->Commit();
  
  	m_pDeleteServAcctInfo = new TimesTenCMD(TTConn);
	if (m_pDeleteServAcctInfo == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
  	}
  	sql37.clear();
  	sql37="DELETE FROM SERV_ACCT WHERE SERV_ID=(SELECT SERV_ID FROM SERV WHERE ACC_NBR=:p0)";
 	m_pDeleteServAcctInfo->Prepare(sql37.c_str());
 	m_pDeleteServAcctInfo->Commit();
  
  	m_pChangeInservHis = new TimesTenCMD(TTConn);
	if (m_pChangeInservHis == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
  	}
  	sql38.clear();
  	sql38="INSERT INTO SERV_HIS(SERV_ID,AGREEMENT_ID,CUST_ID,PRODUCT_ID,CYCLE_TYPE_ID,CREATE_DATE,RENT_DATE,STATE,STATE_DATE,ACC_NBR,USER_TYPE,USER_ATTR,PAY_TYPE,PAY_STATUS,BILLING_MODE_ID,FREE_TYPE_ID,NSC_DATE) SELECT SERV_ID,AGREEMENT_ID,CUST_ID,PRODUCT_ID,CYCLE_TYPE_ID,CREATE_DATE,RENT_DATE,:p0,STATE_DATE,ACC_NBR,USER_TYPE,USER_ATTR,PAY_TYPE,PAY_STATUS,BILLING_MODE_ID,FREE_TYPE_ID,NSC_DATE FROM SERV WHERE SERV_ID=(SELECT SERV_ID FROM APP_USER_INFO WHERE NATIONAL_USER_ID=:p1)";
  	m_pChangeInservHis->Prepare(sql38.c_str());
  	m_pChangeInservHis->Commit();
  
  	m_pChangeNbr = new TimesTenCMD(TTConn);
	if (m_pChangeNbr == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
  	}
  	sql39.clear();
  	sql39="UPDATE SERV SET ACC_NBR=:p0 WHERE SERV_ID=(SELECT SERV_ID FROM APP_USER_INFO WHERE NATIONAL_USER_ID=:p1 )";
  	m_pChangeNbr->Prepare(sql39.c_str());
  	m_pChangeNbr->Commit();
	
	m_pInsertAttUserInfo= new TimesTenCMD(TTConn);
	if (m_pInsertAttUserInfo == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql24.clear();
	sql24 =" INSERT INTO ATT_USER_INFO (SERV_ID, ATTR_VALUES, ATTR_TYPE) VALUES (:SERV_ID, :ATTR_VALUES, :ATTR_TYPE)";
	m_pInsertAttUserInfo->Prepare(sql24.c_str());
	m_pInsertAttUserInfo->Commit();

	m_pInsertAppUserInfo= new TimesTenCMD(TTConn);
	if (m_pInsertAttUserInfo == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql25.clear();
	sql25 =" INSERT INTO APP_USER_INFO (SERV_ID, NATIONAL_USER_ID, NATIONAL_ACCT_ID, LOCAL_USER_ID, PRODUCT_ID, LOCAL_AREA_ID, SERVICE_PLATFORM_ID) VALUES (:SERV_ID, :NATIONAL_USER_ID, :NATIONAL_ACCT_ID, :LOCAL_USER_ID, :PRODUCT_ID, :LOCAL_AREA_ID, :SERVICE_PLATFORM_ID)";
	m_pInsertAppUserInfo->Prepare(sql25.c_str());
	m_pInsertAppUserInfo->Commit();
	
	// Save PLATFORM_CCR
	m_pSavePlatformCCR= new TimesTenCMD(TTConn);
	if (m_pSavePlatformCCR == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql40.clear();
	sql40 =" INSERT INTO PLATFORM_CCR (SESSION_ID, ORIGIN_HOST, ORIGIN_REALM, DEST_HOST, DEST_REALM, SVC_CONTEXT_ID, EVENT_TIMESTAMP) VALUES (:SESSION_ID, :ORIGIN_HOST, :ORIGIN_REALM, :DEST_HOST, :DEST_REALM, :SVC_CONTEXT_ID, sysdate)";
	m_pSavePlatformCCR->Prepare(sql40.c_str());
	m_pSavePlatformCCR->Commit();
	
	// Save PLATFORM_CCRBIZ
	m_pSavePlatformCCRBiz= new TimesTenCMD(TTConn);
	if (m_pSavePlatformCCRBiz == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql41.clear();
	sql41 =" INSERT INTO PLATFORM_CCRBIZ_INFO(SESSION_ID, REQUEST_ID, REQUEST_DATE, DEST_ACCOUNT, DEST_TYPE, DEST_ATTR, SVC_PLATFORM_ID,PAY_PASSWORD,BALANCE_TRANS_FLAG,UNIT_TYPE_ID,DEDUCT_AMOUNT) VALUES (:SESSION_ID, :REQUEST_ID, sysdate, :DEST_ACCOUNT, :DEST_TYPE, :DEST_ATTR, :SVC_PLATFORM_ID, :PAY_PASSWORD, :BALANCE_TRANS_FLAG, :UNIT_TYPE_ID,:DEDUCT_AMOUNT)";
	m_pSavePlatformCCRBiz->Prepare(sql41.c_str());
	m_pSavePlatformCCRBiz->Commit();
	
	// Save DCC_INFO
	m_pSaveDcc= new TimesTenCMD(TTConn);
	if (m_pSaveDcc == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql42.clear();
	sql42 =" INSERT INTO DCC_INFO_RECORD(RECORD_ID,SESSION_ID, ORIGIN_HOST, ORIGIN_REALM, DEST_REALM, SRV_CONTEXT_ID, SRV_FLOW_ID,RECORD_TIME,RECCORD_TYPE,STATE,STATE_DATE) VALUES (:RECORD_ID,:SESSION_ID, :ORIGIN_HOST, :ORIGIN_REALM, :DEST_REALM, :SVC_CONTEXT_ID, :SRV_FLOW_ID,sysdate,:RECORD_TYPE,'00P',sysdate)";
	m_pSaveDcc->Prepare(sql42.c_str());
	m_pSaveDcc->Commit();
	
	// Save Send DCC_INFO
	m_pSaveDcc= new TimesTenCMD(TTConn);
	if (m_pSaveDcc == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql43.clear();
	sql43 =" INSERT INTO DCC_INFO_RECORD(RECORD_ID,SESSION_ID, ORIGIN_HOST, ORIGIN_REALM, DEST_REALM, SRV_CONTEXT_ID, SRV_FLOW_ID,RECORD_TIME,RECCORD_TYPE,PRE_SESSION_ID,STATE,STATE_DATE) VALUES (:RECORD_ID,:SESSION_ID, :ORIGIN_HOST, :ORIGIN_REALM, :DEST_REALM, :SVC_CONTEXT_ID, :SRV_FLOW_ID,sysdate,:RECORD_TYPE,:PRE_SESSION_ID,'00X',sysdate)";
	m_pSaveDcc->Prepare(sql43.c_str());
	m_pSaveDcc->Commit();
	
	// Update DCC_INFO_RECORD
	m_pUpdateDcc= new TimesTenCMD(TTConn);
	if (m_pUpdateDcc == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql44.clear();
	sql44 =" UPDATE DCC_INFO_RECORD SET STATE='00A',STATE_DATE=sysdate WHERE SESSION_ID=:SESSION_ID ";
	m_pUpdateDcc->Prepare(sql44.c_str());
	m_pUpdateDcc->Commit();
	
	// Save Send DCC_INFO-CCR,请求流水
	m_pSaveActiveCCR= new TimesTenCMD(TTConn);
	if (m_pSaveActiveCCR == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql45.clear();
	sql45 =" INSERT INTO ACTIVE_CCR_LOG(SESSION_ID,REQUEST_ID,CREATE_DATE,REQUEST_TYPE,ORG_ID,STATE,STATE_DATE) VALUES (:SESSION_ID, :REQUEST_ID,sysdate,:REQUEST_TYPE,:ORG_ID,'00X',sysdate)";
	m_pSaveActiveCCR->Prepare(sql45.c_str());
	m_pSaveActiveCCR->Commit();
	
	// Update ACTIVE_CCR_LOG
	m_pUpdateCCR= new TimesTenCMD(TTConn);
	if (m_pUpdateCCR == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw ECODE_DB_LINK_FAIL;
	}
	sql46.clear();
	sql46 =" UPDATE ACTIVE_CCR_LOG SET STATE='00A',STATE_DATE=sysdate WHERE REQUEST_ID=:REQUEST_ID ";
	m_pUpdateCCR->Prepare(sql46.c_str());
	m_pUpdateCCR->Commit();
	
	 //用户信息增量表状态更新
  m_pUpdateServAddState = new TimesTenCMD(TTConn);
	if (m_pUpdateServAddState == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw -1;
  }
  sql50.clear();
  sql50="UPDATE USER_INFO_ADD SET SYN_STATE='END' WHERE SESSION_ID=:p0";
  m_pUpdateServAddState->Prepare(sql50.c_str());
  m_pUpdateServAddState->Commit();
  
  m_pInsertSendSql = new TimesTenCMD(TTConn);
	if (m_pInsertSendSql == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw -1;
  }
  sql51.clear();
  sql51="INSERT INTO SMS_SEND(SMS_SEND_ID,SERV_ID,ACC_BNR,MSG_TYPE_ID,CREATED_DATE,STATE, MSG_CONTENT)select SMS_SEND_SEQ.nextval,serv_id,user_login_id,:p0,sysdate,:p1,:p2 from user_info_add where session_id=:p3";
  m_pInsertSendSql->Prepare(sql51.c_str());
  m_pInsertSendSql->Commit();
  
  m_pLoginSaveDcc = new TimesTenCMD(TTConn);
	if (m_pLoginSaveDcc == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw -1;
  }
  sql52.clear();
  sql52="INSERT INTO PLATFORM_LOGINIDENTIFY_INFO(SESSION_ID, REQUEST_ID,REQUEST_DATE, DEST_ACCOUNT, DEST_TYPE, DEST_ATTR, SVC_PLATFORM_ID,LOGIN_PASSWORD) VALUES (:SESSION_ID, :REQUEST_ID, sysdate, :DEST_ACCOUNT, :DEST_TYPE, :DEST_ATTR, :SVC_PLATFORM_ID, :LOGIN_PASSWORD)";
  m_pLoginSaveDcc->Prepare(sql52.c_str());
  m_pLoginSaveDcc->Commit();
  
  m_pPasswdChangeDcc = new TimesTenCMD(TTConn);
	if (m_pPasswdChangeDcc == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw -1;
  }
  sql53.clear();
  sql53="INSERT INTO PLATFORM_PASSWDCHANGE_INFO(SESSION_ID, REQUEST_ID,REQUEST_DATE, DEST_ACCOUNT, DEST_TYPE, DEST_ATTR, SVC_PLATFORM_ID,OLD_PASSWORD,NEW_PASSWORD) VALUES (:SESSION_ID, :REQUEST_ID, sysdate, :DEST_ACCOUNT, :DEST_TYPE, :DEST_ATTR, :SVC_PLATFORM_ID, :OLD_PASSWORD,:NEW_PASSWORD)";
  m_pPasswdChangeDcc->Prepare(sql53.c_str());
  m_pPasswdChangeDcc->Commit();
  
  m_pChangeAcct = new TimesTenCMD(TTConn);
	if (m_pChangeAcct == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw -1;
  }
  sql54.clear();
  sql54="INSERT INTO ACCT(ACCT_ID,CUST_ID,STATE,STATE_DATE,ACCT_NBR,BATCH_BILLING_CYCLE_ID)VALUES(ACCT_ID_SEQ.nextval,CUST_ID_SEQ.nextval,'10A',SYSDATE,:ACCT_NBR,to_number(to_char(sysdate,'YYYYMM')))";
  m_pChangeAcct->Prepare(sql54.c_str());
  m_pChangeAcct->Commit();
  
  m_pUpdateServAcct = new TimesTenCMD(TTConn);
	if (m_pUpdateServAcct == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw -1;
  }
  sql55.clear();
  sql55="update serv_acct a set a.acct_id=(select b.acct_id from acct b where b.acct_nbr=:b_acct_nbr) where exists(select 1 from acct_his c where a.acct_id=c.acct_id and c.acct_nbr=:c_acct_nbr)";
  m_pUpdateServAcct->Prepare(sql55.c_str());
  m_pUpdateServAcct->Commit();
  
   m_pGetSeq = new TimesTenCMD(TTConn);
	if (m_pGetSeq == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw -1;
   }
	sql56.clear();
	sql56="select request_id_seq.nextval from dual";
	m_pGetSeq->Prepare(sql56.c_str());
   m_pGetSeq->Commit();
   
   m_pInserUserAddErr = new TimesTenCMD(TTConn);
	if (m_pInserUserAddErr == NULL)
	{
		ADD_EXCEPT0(oExp, "连接数据库失败!");
		throw -1;
   }
   sql57.clear();
	sql57="update user_info_add set syn_state='ERR',err_nbr=:ERR_NBR where session_id=:SESSION_ID";
	m_pInserUserAddErr->Prepare(sql57.c_str());
   m_pInserUserAddErr->Commit();
	
        return 0;
    }
    catch (TTStatus oSt) 
    {
        ADD_EXCEPT1(oExp, "PublicConndb::init oSt.err_msg=%s", oSt.err_msg);
    }
    catch(int &value)
    {
        ADD_EXCEPT1(oExp, "PublicConndb::init oSt.err_msg=%s", "MALLOC_MEM_FAIL");
    }
    return ECODE_DB_LINK_FAIL;

}
int PublicConndb::Commit(ABMException &oExp)
{
	  TTStatus ttStatus;
  try
  {
	if(!TTConn)
	{
		ADD_EXCEPT0(oExp, "提交失败!");
		return  ECODE_DB_LINK_FAIL;
	}
	//TTConn->commit();
	if (TTConn->isConnected()){
		__DEBUG_LOG0_(0, "ABMSvcSupt::commit ");
		TTConn->Commit(ttStatus);
	}
	
	return 0;
    }
    catch (TTStatus oSt) 
    {
        ADD_EXCEPT1(oExp, "TTConn->commit()=%s", oSt.err_msg);
    }
    return ECODE_DB_LINK_FAIL;
}
int PublicConndb::RollBack(ABMException &oExp)
{
	  TTStatus ttStatus;
  try
  {
	if(!TTConn)
	{
		ADD_EXCEPT0(oExp, "回滚失败!");
		return  ECODE_DB_LINK_FAIL;
	}
	//TTConn->rollback();
	if (TTConn->isConnected()){
		__DEBUG_LOG0_(0, "ABMSvcSupt::rollback ");
		TTConn->Rollback(ttStatus);
	}
	
	return 0;
    }
    catch (TTStatus oSt) 
    {
        ADD_EXCEPT1(oExp, "TTConn->rollback()=%s", oSt.err_msg);
    }
    return ECODE_DB_LINK_FAIL;
}
