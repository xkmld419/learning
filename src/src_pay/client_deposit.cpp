#include "abmpaycmd.h"
#include "MessageQueue.h"
#include "ABMException.h"
#include <iostream>
using namespace std;
int Deposit();
int ReverseDepositClient();

int main(int argc, char *argv[])
{

	int iRet=0;	
	string sFlag;
	sFlag.clear();
    	cout<<"请输入操作类型，1：VC充值，2：充值冲正:"<<endl;
	cin>>sFlag;
	if(!sFlag.compare("1"))
	{
		cout<<"开始充值!"<<endl;
		iRet=Deposit();
	}
	else if(!sFlag.compare("2"))
	{
		cout<<"开始充值冲正!"<<endl;
		iRet=ReverseDepositClient();
	}
	else
	{
		cout<<"重新输入操作类型!"<<endl;
	}
	return 0;
    
}
int Deposit()
{
    string sAccNbr,sFlag;
    cout<<"请输入充值号码,其他信息默认:"<<endl;
    cin>>sAccNbr;
    cout<<"是否测试优惠Y/N:"<<endl;
    cin>>sFlag;
    ABMException oExp;
    
    DepositServCondOne oCmd;
    strcpy(oCmd.m_sReqSerial, "98765432");
    strcpy(oCmd.m_sBalanceType, "2");
    oCmd.m_iDestinationBalanceUnit = 0;
    oCmd.m_iVoucherValue = 80;
    oCmd.m_iProlongDays = 30;
    strcpy(oCmd.m_sDestinationType , "5BC");
    strcpy(oCmd.m_sDestinationAccount ,sAccNbr.c_str());
    strcpy(oCmd.m_sAreaCode,"0514");
    oCmd.m_iDestinationAttr = 8;
    oCmd.m_iBonusStartTime = 20110520;
    oCmd.m_iBonusExpirationTime = 20110520;

    DepositServCondTwo oCmdTwo;
    oCmdTwo.m_iBonusUnit=0;
    oCmdTwo.m_iBonusAmount=3;
    strcpy(oCmdTwo.m_sBalanceType,"3");
    oCmdTwo.m_iBonusStartTime=20110101;
    oCmdTwo.m_iBonusExpirationTime=20110601;

    DepositInfoCCR oCCR;
    DepositInfoCCA oCCA;
    
    oCCR.clear();
    if (!oCCR.addDepositOneCCR(oCmd)) {
        cout<<"pack 普通充值 failed!"<<endl;
        return -1;
    }
    if(!(sFlag.compare("Y"))||(!sFlag.compare("y")))
    {
	    cout<<"增加优惠数据"<<endl;
	    if (!oCCR.addDepositTwoCCR(oCmdTwo)) {
		cout<<"pack  优惠 failed!"<<endl;
		return -1;
	    }
    }
    MessageQueue *pMQ = new MessageQueue(11005);
    if (pMQ->open(oExp, true, true) != 0) {
        cout<<"pMQ->open failed!"<<endl;
        return -1;
    }
    
    MessageQueue *pMQ2 = new MessageQueue(11006);
    if (pMQ2->open(oExp, true, true) != 0) {
        cout<<"pMQ->open failed!"<<endl;
        return -1;
    }
    
    if (pMQ->Send((void *)&oCCR, oCCR.m_iSize) <= 0) {
        cout<<"pMQ->Send failed!"<<endl;
        return -1;
    }

    if (pMQ2->Receive((void *)&oCCA, sizeof(oCCA), 0) <= 0) {
        cout<<"pMQ->Receive failed!"<<endl;
        return -1;
    }

    vector<ResponsDepositOne*> vRes;
    vector<ResponsDepositTwo*> vData;
    if (oCCA.getRespDepositOneCCA(vRes) <= 0) {
        cout<<"oCCA->getRespons failed!"<<endl;
        return -1;
    }
    for (int i=0; i<vRes.size(); ++i) {
        cout<<"业务处理码"<<vRes[i]->m_iRltCode<<endl;
        cout<<"请求流水"<<vRes[i]->m_sReqSerial<<endl;
        vData.clear();
	if(vRes[i]->m_iRltCode==0)
	{
		if (oCCA.getRespDepositTwoCCA(vRes[i], vData) <= 0) {
		    cout<<"getServData failed!"<<endl;
		    return -1;
		} 

		for (int j=0; j<vData.size(); ++j) {
		    cout<<"m_iDestinationBalance="<<vData[j]->m_iDestinationBalance<<endl;
		    cout<<"m_sBalanceType="<<vData[j]->m_sBalanceType<<endl;
		    cout<<"m_iDestinationBalanceUnit="<<vData[j]->m_iDestinationBalanceUnit<<endl;
		    cout<<"m_iDestinationEffectiveTime="<<vData[j]->m_iDestinationEffectiveTime<<endl;
		    cout<<"m_iDestinationExpirationTime="<<vData[j]->m_iDestinationExpirationTime<<endl;
		   
		}
	}
    }
    return 0;
}
int ReverseDepositClient()
{
	string sAccNbr;
    cout<<"请输入原充值请求流水号,其他信息默认:"<<endl;
    cin>>sAccNbr;
    ABMException oExp;
    
    ReverseDeposit oCmd;
    strcpy(oCmd.m_sReqSerial, "98765432");
    strcpy(oCmd.m_sPaymentID, sAccNbr.c_str());
    strcpy(oCmd.m_sDestinationType , "5BC");
    strcpy(oCmd.m_sDestinationAccount ,"18951765356");
    oCmd.m_iDestinationAttr = 8;

  
    ReverseDepositInfoCCR oCCR;
    ReverseDepositInfoCCA oCCA;
    
    oCCR.clear();
    if (!oCCR.addReverseDepositInfo(oCmd)) {
        cout<<"pack 充值冲正 failed!"<<endl;
        return -1;
    }
   
    MessageQueue *pMQ = new MessageQueue(11005);
    if (pMQ->open(oExp, true, true) != 0) {
        cout<<"pMQ->open failed!"<<endl;
        return -1;
    }
    
    MessageQueue *pMQ2 = new MessageQueue(11006);
    if (pMQ2->open(oExp, true, true) != 0) {
        cout<<"pMQ->open failed!"<<endl;
        return -1;
    }
    
    if (pMQ->Send((void *)&oCCR, oCCR.m_iSize) <= 0) {
        cout<<"pMQ->Send failed!"<<endl;
        return -1;
    }

    if (pMQ2->Receive((void *)&oCCA, sizeof(oCCA), 0) <= 0) {
        cout<<"pMQ->Receive failed!"<<endl;
        return -1;
    }

    vector<ResponsReverseDeposit*> vRes;
    vRes.clear();
    if (oCCA.getRespReverseDepositCCA(vRes) <= 0) {
        cout<<"oCCA->getRespons failed!"<<endl;
        return -1;
    }
    for (int i=0; i<vRes.size(); ++i) {
        cout<<"业务处理码"<<vRes[i]->m_iRltCode<<endl;
        cout<<"请求流水"<<vRes[i]->m_sReqSerial<<endl;
    }
    return 0;
}



