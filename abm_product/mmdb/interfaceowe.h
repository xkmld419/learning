#ifndef interface_H_HEADER_INCLUDED
#define interface_H_HEADER_INCLUDED

#ifdef ABM_BALANCE
#include "abmclnt.h"
#include "abmaccess.h"   //命令头文件
#endif

#include "AcctItemAccu.h"
#include "Environment.h"
#include "TOCIQuery.h"
#include "Log.h"

#include <iostream>

using namespace std;

class AbmInterfaceManage
{
#ifdef ABM_BALANCE
public:
    AbmInterfaceManage();
    ~AbmInterfaceManage();
    
    //获取接口初始化返回值,0表示成功
    int getInitResult();
    
    //实时扣费,0表示成功
    int pachCharge(DeductBalanceR &oData, vector<DeductAcctItem*> &vecBal);
    int transCharge();
    int queryBalance(RealDeductA &queryreal);

    //批量实时扣费,0表示成功
    int resetBatCharge();
    int pachBatCharge(DeductBalanceR &oData, vector<DeductAcctItem*> &vecBal);
    int transBatCharge();
    int queryBatBalance(RealDeductA &queryreal, int ioffset);
    long getBatCmdNum();
    long getBatResultNum();
    
    //获取断点,0表示成功
    int getBreakPoint(long lFileID, long &loffset);

    //查询实时扣费结果,0表示成功
    int querypayresult(long lAcctID, long lServID, PayResultQryA &payresult);

    //查询帐户余额明细,0表示成功
    int queryBalanceDetail(long lAcctID, vector<BalanceInfoS *> &vecBalanceInfoS);
    
    //查询帐户实时欠费明细,0表示成功
    int queryCurAggrDetail(long lAcctID, vector<AcctItemInfoHB *> &vecAcctItemInfoHB);
    
    //过户通知abm,0表示成功
    int modifyAcctOld(long lAcctID, long lServID, long lTransID, int iBillingCycleID, vector<AcctAggrHB01 *> &vecData);
    int modifyAcctNew(long lAcctID, long lServID, long lTransID, int iBillingCycleID, int iBillingMode, vector<AcctAggrHB01 *> &vecData);

    //用户预后付发生转变通知abm,0表示成功
    int transBillingMode(long lAcctID, long lServID, long lTypeID);

    //测试用
    bool dealacct_item_withoutabm(char const *sTable);
    bool dealacct_item_withabm(char const *sTable);
    

private:
   ABM *pInst;   //ABM接口初始化
   
   int m_iInitRet;          //接口初始化返回值
 
   //实时扣费
   RealDeductCmd qryCmd;    // 实时欠费命令
   ABMResult qryRlt;        //查询结果 
   RealDeductResult *prlt;  //扣费查询信息
 
   //实时费用查询进度
   RealQueryCmd qryCmd1;   // 实时费用查询进度命令
   ABMResult qryRlt1;      //查询结果2
   RealQueryResult *prlt1; //实时费用查询进度结果
 
   //扣费结果实时查询
   PayResultQryCmd qryCmd2;    // 扣费结果实时查询命令
   ABMResult qryRlt2;          //查询结果 
   PayResultQryResult *prlt2;  //扣费结果实时查询信息
   
	 //查询帐户实时费用明细
	 QueryAcctItemCmd qaiCmd;        //查询请求
	 QueryAcctItemResult qaiResult;  //查询结果
   
   //查询帐户余额明细
   QueryBalance qb;
	 QryBalanceSCmd qbSCmd;         //查询请求
	 QryBalanceSResult qbSResult;   //查询结果

   //过户通知ABM
   ModifyAcctOldCmd maoCmd;       //查询请求
   ModifyAcctOldResult maoResult; //查询结果
   
   ModifyAcctNewCmd manCmd;       //查询请求
   ModifyAcctNewResult manResult; //查询结果
   
   //用户预后付变化通知abm
   StatusTransCmd stCmd;          //查询请求
   StatusTransResult stResult;    //查询结果

   //批量实时扣费
   BatRealDeductCmd      brdCmd;    //查询请求
   BatRealDeductResult   brdResult; //查询结果
#endif
};


#endif /* interface_H_HEADER_INCLUDED */

