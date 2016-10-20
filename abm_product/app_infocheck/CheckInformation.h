#ifndef __CHECK_INFORMATION_H_HEADER
#define __CHECK_INFORMATION_H_HEADER

#include <string>
#include <vector>
#include <map>
#include <iostream>
using namespace std;
#include "CheckStatInterface.h"

const int MODULE_GATHER  =1;
const int MODULE_PREP    =2;
const int MODULE_PRICING =3;
const int MODULE_INSTORE =4;

#define CAPABILITY_TITLE "S-KPIID|S-KPIName|S-KBP|S-KBPName|F-Min|F-Avg|F-Max"
#define CONFIG_TITLE "S-KPIID|S-KPIName|S-KBP|S-KBPName|I-Action|S-Value"
#define ALARM_TITLE "S-KPIID|S-KPIName|S-AlarmID|S-KBP|S-KBPName|D-FirstTime|D-LastTime|I-EventLevel|I-EventState|D-AckTime|S-AckUser|D-ClearTime|S-ClearUser|I-Count"

struct CDR_Info 
{
	long recvAllFileCount;
	long recvDelayFileCount;
	long recvAllRecordCount;
	long recvDelayRecordCount;
	long recvErrFileCount;
	
	//话单处理-预处理
	long prepAllRecordCount;
	long prepErrRecordCount;
	long prepDupRecordCount;
	
	//话单处理-批价
	long pricingAllRecordCount;
	long pricingUseLessRecordCount;
	long pricingErrRecordCount;
	long pricingDupRecordCount;
	long pricingBlackRecordCount;
	long pricingOkRecordCount;
	
	//话单处理-入库
	long indbInAllRecordCount;
	long indbOutAllRecordCount;
	long indbErrRecordCount;
	
	void init();
	void parse();
};


class Information
{
private :	
	map<string,string> m_pCodeList;
	map<string,string> m_pKpiList;
	int m_iDelayTimeSecs; //延时时间
	int m_mode;
	int m_iFlag;
	
private :
	vector<string> m_vGatherInfo;
	vector<string> m_vPrepInfo;
	vector<string> m_vPricingInfo;
	vector<string> m_vInDbInfo;
	
	string strValue;
	float ratio;
	char buf[32+1];
	char m_source[255];
	char m_busiclass[255];
	
	//告警判断
	bool m_bPrepInOut;
	bool m_bRecvOutPrepIn;
	bool m_bPricingInOut;
	bool m_bPrepOutPricingIn;
	bool m_bPricingOutIndbIn;
	
private :
	bool gatherInfo();
	bool prepInfo();
	bool pricingInfo();
	bool indbInfo();
	bool init(int iFlag);
	bool checkPerformance(int mode);
	void print(vector<string> vec);
	
	bool insertAlarm(string kpiId,string kpiName,string alarmId,string kbpId,string kbpName,Date occurTime,int modeType);
	long getAppInfo(int appId,bool in);
	bool getAppAlarm(int mode,vector<string> &vec);
	
	//add 2010-11-22
  long getAppInfo(const char *appId,bool in);
public :
	Information();
	~Information();
	//性能信息点
  bool checkGather();
	bool checkPerp();
	bool checkPricing();
	bool checkIndb();
	
	//配置信息点-全量
	bool checkAllProductInfo();
	bool checkAllProductOfferInfo();
	bool checkAllPricingPlanInfo();
	
	//配置信息点-增量
	bool checkChangedProductInfo();
	bool checkChangedProductOfferInfo();
	
	//告警信息点中间数据
	bool checkBalance(int mode);  //产生中间告警信息，参数mode取值2：预处理，3：批价，4：入库
	
	//告警信息点
	bool checkPrepAlarm();
	bool checkPricingAlarm();
	bool checkIndbAlarm();
	bool writePrepAlarm();
	bool writePricingAlarm();
	bool writeIndbAlarm();
	
public :
	map<int ,CDR_Info> m_mapCDRInfo;
	map<int ,CDR_Info>::iterator m_mapIter;
		

};

#endif
