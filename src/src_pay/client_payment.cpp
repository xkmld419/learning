#include "abmpaycmd.h"
#include "MessageQueue.h"
#include "ABMException.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{

    string sAccNbr;
    cout<<"请输入支付号码,其他信息默认:"<<endl;
    cin>>sAccNbr;

    ABMException oExp;
    
    AbmPayCond oCmd;
    strcpy(oCmd.m_sOperatype, "hello world");
    strcpy(oCmd.m_sOperateSeq, "1234567");
    strcpy(oCmd.m_sSpID, "543");
    strcpy(oCmd.m_sServicePlatformID, "321");
    strcpy(oCmd.m_sPayPassword, "password");
    strcpy(oCmd.m_sRandomPassword, "password");
    strcpy(oCmd.m_sDestinationAccount, sAccNbr.c_str());
    strcpy(oCmd.m_sOrderID, "13579");
    strcpy(oCmd.m_sDescription, "支付测试");
    strcpy(oCmd.m_sSpID, "9876");
    oCmd.m_lPayAmount=7;
    oCmd.m_iOperateAction = 8;
    oCmd.m_iPayDate = 20110531193030;
    AbmPaymentCCR oCCR;
    AbmPaymentCCA oCCA;
    
    oCCR.clear();
    if (!oCCR.addPayInfo(oCmd)) {
        cout<<"pack failed!"<<endl;
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
    oCCA.clear();
    if (pMQ2->Receive((void *)&oCCA, sizeof(oCCA), 0) <= 0) {
        cout<<"pMQ->Receive failed!"<<endl;
        return -1;
    }

    vector<ResponsPayOne*> vRes;
    vector<ResponsPayTwo*> vData;
    vRes.clear();
    vData.clear();
    if (oCCA.getRespons(vRes) <= 0) {
        cout<<"oCCA->getRespons failed!"<<endl;
        return -1;
    }
    for (int i=0; i<vRes.size(); ++i) {
        cout<<"业务处理码="<<vRes[i]->m_iRltCode<<endl;
        vData.clear();
	if(vRes[i]->m_iRltCode==0)
	{
		if (oCCA.getPayData(vRes[i], vData) <= 0) {
		    cout<<"getServData failed!"<<endl;
		    return -1;
		} 

		for (int j=0; j<vData.size(); ++j) {
		    cout<<"m_sOperateSeq="<<vData[j]->m_sOperateSeq<<endl;
		    cout<<"m_sReqSerial="<<vData[j]->m_sReqSerial<<endl;
		   
		}
	}
    }
    return 0;
}

