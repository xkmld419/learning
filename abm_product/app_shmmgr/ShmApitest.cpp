#include <vector>
#include <iostream>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "SGWSHMParamApi.h"
#include "SHMDataMgr.h"
#include "SgwDefine.h"
#include "SHMStringTreeIndex.h"
#include "SHMSGWInfoBase.h"


using namespace std;
#define SEQ_PETRI_ACTION_ID GETVALUE(".action_id_seq")
struct SEQTEMP {
	long NEXTVAL;
};

inline SEQTEMP GETVALUE(const char *_sFile)
{
	static SEQTEMP _oSeq;
	_oSeq.NEXTVAL = -1;
	static char _sValue[32] ={0};
	struct stat st;
	int fd = 0;
	if (( fd = open(_sFile, O_RDWR|O_CREAT,0777) ) < 0) {
		return _oSeq;
	} else {
		fstat(fd, &st);
		if (st.st_size == 0) {
			char _sTemp[32] = "0";
			int iRet = write(fd,_sTemp,sizeof(_sTemp)-1);

			if (iRet== -1) {
				close(fd);
				return _oSeq;
			}
		}
		fstat(fd, &st);
		char *mmap_addr = NULL;
		mmap_addr = (char*)mmap(0, st.st_size, PROT_READ|PROT_WRITE , MAP_SHARED, fd, 0);
		if (mmap_addr == MAP_FAILED) {
			close(fd);
			return _oSeq;
		} else if (strlen(mmap_addr) >0) {
			if (mmap_addr[1] == '\n') {
				mmap_addr[1] = '\0';
			}
			snprintf(_sValue,sizeof(_sValue)-1,"%s",mmap_addr);
			_oSeq.NEXTVAL = atol(_sValue);
			++_oSeq.NEXTVAL;

			if (_oSeq.NEXTVAL>9999999999999) {
				_oSeq.NEXTVAL = 1;
			}
			sprintf(mmap_addr,"%ld",_oSeq.NEXTVAL);

		}
		if ( munmap(mmap_addr, st.st_size)==-1 ) {
			close(fd);
			_oSeq.NEXTVAL = -1;
			return _oSeq;
		}
		close(fd);
	}
	return _oSeq;
}


void test(char *p )
{
	cout<<p<<endl;
}

void testaddsession(const char *sessionID)
{
	//SHMSGWInfoBase::session_init();
	SessionData tSessionData;
	memset(&tSessionData,0,sizeof(tSessionData));
	strcpy(tSessionData.m_sSessionId,sessionID);
	int iRet = SHMSessionData::AddSessionData(tSessionData);
	if (iRet) {
		cout<<"add error:session_id="<<sessionID<<"["<<iRet<<"]"<<endl;
	}
}

void testdelsession(const char *sessionID)
{
	//SHMSGWInfoBase::session_init();
	SessionData tSessionData;
	memset(&tSessionData,0,sizeof(tSessionData));
	strcpy(tSessionData.m_sSessionId,sessionID);
	int iRet = SHMSessionData::DelSessionData(tSessionData);
	if (iRet) {
		cout<<"del error:session_id="<<sessionID<<"["<<iRet<<"]"<<endl;
	}
}

void testquerysession(const char *sessionID)
{
	//SHMSGWInfoBase::session_init();
	SessionData tSessionData;
	memset(&tSessionData,0,sizeof(tSessionData));
	strcpy(tSessionData.m_sSessionId,sessionID);
	int iRet = SHMSessionData::GetSessionData(tSessionData);
	if (iRet) {
		/*cout<<"tSessionData.m_iRequestType="<<tSessionData.m_iRequestType<<endl;
		cout<<"tSessionData.m_iRequestNumber="<<tSessionData.m_iRequestNumber<<endl;
		cout<<"tSessionData.m_iAuthApplicationId="<<tSessionData.m_iAuthApplicationId<<endl;
		cout<<"tSessionData.m_iRequestedAction="<<tSessionData.m_iRequestedAction<<endl;
		cout<<"tSessionData.m_iHh="<<tSessionData.m_iHh<<endl;
		cout<<"tSessionData.m_iEe="<<tSessionData.m_iEe<<endl;
		cout<<"tSessionData.m_lHostId="<<tSessionData.m_lHostId<<endl;
		cout<<"tSessionData.m_sEventTimestamp="<<tSessionData.m_sEventTimestamp<<endl;
		cout<<"tSessionData.m_iProtoType="<<tSessionData.m_iProtoType<<endl;
		cout<<"tSessionData.m_iREPTAG="<<tSessionData.m_iREPTAG<<endl;
		cout<<"tSessionData.m_iSocket="<<tSessionData.m_iSocket<<endl;
		cout<<"tSessionData.m_ulSocketPtr="<<tSessionData.m_ulSocketPtr<<endl;
		cout<<"tSessionData.m_sSessionId="<<tSessionData.m_sSessionId<<endl;
		cout<<"tSessionData.m_sOriginHost="<<tSessionData.m_sOriginHost<<endl;
		cout<<"tSessionData.m_strUserName="<<tSessionData.m_strUserName<<endl;
		cout<<"tSessionData.m_sOriginRealm="<<tSessionData.m_sOriginRealm<<endl;
		cout<<"tSessionData.m_sDestinationHost="<<tSessionData.m_sDestinationHost<<endl;
		cout<<"tSessionData.m_sDestinationRealm="<<tSessionData.m_sDestinationRealm<<endl;
		cout<<"tSessionData.m_sServiceContextId="<<tSessionData.m_sServiceContextId<<endl;
		cout<<"tSessionData.m_DateTime="<<tSessionData.m_DateTime<<endl;
		cout<<"tSessionData.m_sUID="<<tSessionData.m_sUID<<endl;
		cout<<"tSessionData.m_sServiceFlowID="<<tSessionData.m_sServiceFlowID<<endl;
		cout<<"tSessionData.m_sOutPlat="<<tSessionData.m_sOutPlat<<endl;
		cout<<"tSessionData.m_sNeCode="<<tSessionData.m_sNeCode<<endl;
		cout<<"tSessionData.m_sUserCode="<<tSessionData.m_sUserCode<<endl;
		cout<<"tSessionData.m_iNext="<<tSessionData.m_iNext<<endl;*/
		cout<<"query error:session_id="<<sessionID<<"["<<iRet<<"]"<<endl;
	}
}

void testsession()
{
	SHMSGWInfoBase::session_init();
	SessionData tSessionData;
	vector<SessionData> vSessionData;
	memset(&tSessionData,0,sizeof(tSessionData));
	SHMSessionData::GetSessionDataALL(vSessionData);
	for (int i =0; i<vSessionData.size(); i++)
	{
		cout<<"["<<i+1<<"]:"<<endl;
		cout<<"vSessionData["<<i<<"].m_iRequestType="<<vSessionData[i].m_iRequestType<<endl;
		cout<<"vSessionData["<<i<<"].m_iRequestNumber="<<vSessionData[i].m_iRequestNumber<<endl;
		cout<<"vSessionData["<<i<<"].m_iAuthApplicationId="<<vSessionData[i].m_iAuthApplicationId<<endl;
		cout<<"vSessionData["<<i<<"].m_iRequestedAction="<<vSessionData[i].m_iRequestedAction<<endl;
		cout<<"vSessionData["<<i<<"].m_iHh="<<vSessionData[i].m_iHh<<endl;
		cout<<"vSessionData["<<i<<"].m_iEe="<<vSessionData[i].m_iEe<<endl;
		cout<<"vSessionData["<<i<<"].m_lHostId="<<vSessionData[i].m_lHostId<<endl;
		cout<<"vSessionData["<<i<<"].m_sEventTimestamp="<<vSessionData[i].m_sEventTimestamp<<endl;
		cout<<"vSessionData["<<i<<"].m_iProtoType="<<vSessionData[i].m_iProtoType<<endl;
		cout<<"vSessionData["<<i<<"].m_iREPTAG="<<vSessionData[i].m_iREPTAG<<endl;
		cout<<"vSessionData["<<i<<"].m_iSocket="<<vSessionData[i].m_iSocket<<endl;
		cout<<"vSessionData["<<i<<"].m_ulSocketPtr="<<vSessionData[i].m_ulSocketPtr<<endl;
		cout<<"vSessionData["<<i<<"].m_sSessionId="<<vSessionData[i].m_sSessionId<<endl;
		cout<<"vSessionData["<<i<<"].m_sOriginHost="<<vSessionData[i].m_sOriginHost<<endl;
		cout<<"vSessionData["<<i<<"].m_strUserName="<<vSessionData[i].m_strUserName<<endl;
		cout<<"vSessionData["<<i<<"].m_sOriginRealm="<<vSessionData[i].m_sOriginRealm<<endl;
		cout<<"vSessionData["<<i<<"].m_sDestinationHost="<<vSessionData[i].m_sDestinationHost<<endl;
		cout<<"vSessionData["<<i<<"].m_sDestinationRealm="<<vSessionData[i].m_sDestinationRealm<<endl;
		cout<<"vSessionData["<<i<<"].m_sServiceContextId="<<vSessionData[i].m_sServiceContextId<<endl;
		cout<<"vSessionData["<<i<<"].m_DateTime="<<vSessionData[i].m_DateTime<<endl;
		cout<<"vSessionData["<<i<<"].m_sUID="<<vSessionData[i].m_sUID<<endl;
		cout<<"vSessionData["<<i<<"].m_sServiceFlowID="<<vSessionData[i].m_sServiceFlowID<<endl;
		cout<<"vSessionData["<<i<<"].m_sOutPlat="<<vSessionData[i].m_sOutPlat<<endl;
		cout<<"vSessionData["<<i<<"].m_sNeCode="<<vSessionData[i].m_sNeCode<<endl;
		cout<<"vSessionData["<<i<<"].m_sUserCode="<<vSessionData[i].m_sUserCode<<endl;
		cout<<"vSessionData["<<i<<"].m_iNext="<<vSessionData[i].m_iNext<<endl;
		cout<<"-----------------------------"<<endl;
	}
	return ;
	cout<<"-----------------------------"<<endl;
	while (true){
		for (int i =1; i<=2000; i++){
			memset(&tSessionData,0,sizeof(SessionData));
			sprintf(tSessionData.m_sSessionId,"scsp03@03.ctnbc-bon.net;2011%d1107103;0001",i);
			tSessionData.m_lHostId = i;
			SHMSessionData::AddSessionData(tSessionData);
		}

		for (int i =1; i<=2000; i++){
			memset(&tSessionData,0,sizeof(SessionData));
			sprintf(tSessionData.m_sSessionId,"scsp03@03.ctnbc-bon.net;2011%d1107103;0001",i);
			tSessionData.m_lHostId = i;
			SHMSessionData::DelSessionData(tSessionData);
		}

		vector<SessionData> vSessionData;
		SHMSessionData::GetSessionDataALL(vSessionData);
		if (vSessionData.size()>0) {
			return;
		}
		//cout<<vSessionData.size()<<endl;
		sleep(1);
	}
}

void testuserinfo()
{
	SPacketSession sPackageSession;
	SHMSGWInfoBase::userinfo_init();
	UserInfoData  tUserInfoData;
	vector<UserInfoData>  vsUserInfoData;
	while (true) {
		for (int i =1; i<=50; i++) {
			memset(&tUserInfoData,0,sizeof(UserInfoData));
			sprintf(tUserInfoData.m_strUserName,"%dace",i);
			tUserInfoData.m_iUserSeq = i;
			if (SHMUserInfoData::AddUserInfoData(tUserInfoData)) {
				cout<<"AddUserInfoData failed!"<<endl;
				sleep(2);
			}
		}
		SHMUserInfoData::GetUserInfoDataAll(vsUserInfoData);
		cout<<vsUserInfoData.size()<<endl<<endl;

		for (int i =1; i<=50;i++) {
				memset(&tUserInfoData,0,sizeof(tUserInfoData));

				if (!SHMUserInfoData::GetUserInfoData(i,tUserInfoData)) {
					cout<<endl;
					cout<<"tUserInfoData.m_strUserName="<<tUserInfoData.m_strUserName<<endl;
					cout<<"tUserInfoData.m_iUserSeq="<<tUserInfoData.m_iUserSeq<<endl;
					cout<<"tUserInfoData.m_iStatus="<<tUserInfoData.m_iStatus<<endl;
					cout<<"tUserInfoData.m_iAdaptiveInfoId="<<tUserInfoData.m_iAdaptiveInfoId<<endl;
				} else {
					cout<<"查询不到数据"<<endl;
					sleep(2);
				}
				cout<<"--------------------------------------------------"<<endl;
				sprintf(tUserInfoData.m_strUserName,"%dace",i);
				if (!SHMUserInfoData::GetUserInfoData(tUserInfoData.m_strUserName,tUserInfoData)) {
					cout<<endl;
					cout<<"tUserInfoData.m_strUserName="<<tUserInfoData.m_strUserName<<endl;
					cout<<"tUserInfoData.m_iUserSeq="<<tUserInfoData.m_iUserSeq<<endl;
					cout<<"tUserInfoData.m_iStatus="<<tUserInfoData.m_iStatus<<endl;
					cout<<"tUserInfoData.m_iAdaptiveInfoId="<<tUserInfoData.m_iAdaptiveInfoId<<endl;
				} else {
					cout<<"查询不到数据"<<endl;
					sleep(2);
				}
				cout<<"--------------------------------------------------"<<endl;
				tUserInfoData.m_iStatus = i;
				tUserInfoData.m_iAdaptiveInfoId = i;

				if (!SHMUserInfoData::UpdateUserInfoData(tUserInfoData)) {
					if (!SHMUserInfoData::GetUserInfoData(tUserInfoData.m_strUserName,tUserInfoData)) {
						//cout<<endl;
						//cout<<"tUserInfoData.m_strUserName="<<tUserInfoData.m_strUserName<<endl;
						cout<<"tUserInfoData.m_iUserSeq="<<tUserInfoData.m_iUserSeq<<endl;
						cout<<"tUserInfoData.m_iStatus="<<tUserInfoData.m_iStatus<<endl;
						cout<<"tUserInfoData.m_iAdaptiveInfoId="<<tUserInfoData.m_iAdaptiveInfoId<<endl;
					} else {
						cout<<"查询不到数据"<<endl;
						sleep(2);
					}
					cout<<"#==============================================#"<<endl;
				}
				if (SHMUserInfoData::DelUserInfoData(tUserInfoData)) {
					cout<<"DelUserInfoData failed"<<endl;
					sleep(2);
				}
		}

		SHMUserInfoData::GetUserInfoDataAll(vsUserInfoData);
		cout<<vsUserInfoData.size()<<endl;
		sleep(5);
	}

}


void testspacksession(const char *sHead)
{
	SHMSGWInfoBase::packetsession_init();
	SPacketSession sPackageSession;
	vector<SPacketSession> vPackageSession;
	vector<SPacketSession> vAddData;
	map<string,string> mKey;

	while (true) {
		vAddData.clear();
		for (int i =1 ; i<=5000; i++) {
			memset(&sPackageSession,0,sizeof(sPackageSession));
			sprintf(sPackageSession.m_szSessionID,"SR4@02.std-ctbn-net;%s7980%d%d;43%d",sHead,rand()%100,rand()%100,rand()%50);
			sprintf(sPackageSession.m_szSubSessionID,"SR3@02.std-ctbn-net;%s686%d868;%d",sHead,rand()%20000,rand()%100000);

			if (!SHMPacketSession::addPacketSession(sPackageSession)) {
				vAddData.push_back(sPackageSession);


				if (!SHMPacketSession::getPacketSession(sPackageSession.m_szSubSessionID,sPackageSession)) {
					//cout<<"sPackageSession.m_szSessionID="<<sPackageSession.m_szSessionID<<endl;
					//cout<<"sPackageSession.m_szSubSessionID="<<sPackageSession.m_szSubSessionID<<endl<<endl;
					sPackageSession.m_iBufSize = rand()%1000;
					sPackageSession.m_iRouteSeq=i;
					if (SHMPacketSession::updatePacketSession(sPackageSession)) {
							cout<<__FILE__<<__LINE__<<";"<<"更新失败"<<endl;
					}
						//cout<<"欲更新的数据是："<<endl;
						//cout<<"sPackageSession.m_iBufSize="<<sPackageSession.m_iBufSize<<endl;
						//cout<<"sPackageSession.m_iRouteSeq="<<sPackageSession.m_iRouteSeq<<endl<<endl;
						//cout<<"更新后节点信息如下："<<endl;

					if (!SHMPacketSession::getPacketSession(sPackageSession.m_szSubSessionID,sPackageSession)) {

							//cout<<"sPackageSession.m_szSessionID="<<sPackageSession.m_szSessionID<<endl;
							//cout<<"sPackageSession.m_szSubSessionID="<<sPackageSession.m_szSubSessionID<<endl;
							//cout<<"sPackageSession.m_iBufSize="<<sPackageSession.m_iBufSize<<endl;
							//cout<<"sPackageSession.m_iRouteSeq="<<sPackageSession.m_iRouteSeq<<endl<<endl;
					} else {
						cout<<__FILE__<<__LINE__<<";"<<"查询失败"<<endl;
					}
				} else {
					cout<<__FILE__<<__LINE__<<";"<<"查询失败"<<endl;
				}
			}
			//sleep(1);
			//cout<<"--------------------------------------------------"<<endl;
		}
		int sum = 0;
		//cout<<"开始查询会话ID"<<endl;
		mKey.clear();

		for (int j = 0; j<vAddData.size(); j++) {
			if (mKey.find(vAddData[j].m_szSessionID)==mKey.end()) {
				if (SHMPacketSession::getPacketSessionAll(vAddData[j].m_szSessionID,vPackageSession)) {
					cout<<__FILE__<<__LINE__<<";"<<"getPacketSessionAll查询失败"<<endl;
				}
				/*for (int i =0 ;i<vPackageSession.size(); i++){
					cout<<vPackageSession[i].m_szSessionID<<endl;
					cout<<vPackageSession[i].m_szSubSessionID<<endl;
					cout<<vPackageSession[i].m_iNext<<endl;
					cout<<vPackageSession[i].m_iSubNext<<endl<<endl;
				}*/
				sum +=vPackageSession.size();
				//cout<<"--------------------------------------------------"<<endl;
				//sleep(1);
				//cout<<sum<<endl;
				mKey.insert(pair<string,string>(vAddData[j].m_szSessionID,vAddData[j].m_szSessionID));
			}
		}
		if (sum != vAddData.size() ){
				cout<<__FILE__<<__LINE__<<";"<<"sum="<<sum<<endl;
				cout<<__FILE__<<__LINE__<<";"<<"vAddData.size()="<<vAddData.size()<<endl;
		}
		//cout<<"sum="<<sum<<endl;
		//cout<<"开始全量删除数据"<<endl;
		for (int i =0 ; i<vAddData.size(); i++) {
			SHMPacketSession::deletePacketSession(vAddData[i]);
		}
		//cout<<"数据删除成功"<<endl;

		SHMPacketSession::getPacketSessionAll(sPackageSession.m_szSessionID,vPackageSession);
		//cout<<"vPackageSession.size()="<<vPackageSession.size()<<endl;
		if(vPackageSession.size()!=0) {
				cout<<__FILE__<<__LINE__<<";"<<vPackageSession.size()<<endl;
		}
		sleep(5);
	}
}

void testportinf()
{
	//	 PortInfo oPortInfo;

	//	SHMSGWInfoBase::portInfo_init();
	// 	oPortInfo.m_iPortId = 1;
	// 	SHMPortInfo::addPortinfo(oPortInfo);
	// 	oPortInfo.m_iPortId = 1;
	// 	SHMPortInfo::addPortinfo(oPortInfo);

}

void testSHMStreamCtrlInfo()
{
	SHMSGWInfoBase::streamctrl_init();
	StreamControlInfo tStreamCtrlInfo;
	vector<StreamControlInfo> vStreamCtrlInfo;

	while (true) {
		for (int i = 1; i<5000; i++){
			memset(&tStreamCtrlInfo,0,sizeof(StreamControlInfo));
			sprintf(tStreamCtrlInfo.m_sUserName,"%d",i);
			sprintf(tStreamCtrlInfo.m_sSerConId,"%d",5000-i);
			SHMStreamCtrlInfo::AddStreamCtrlInfo(tStreamCtrlInfo);
		}
		for (int i =1; i<=5; i++ ) {
			memset(&tStreamCtrlInfo,0,sizeof(StreamControlInfo));
			int key = rand()%4999+1;
			sprintf(tStreamCtrlInfo.m_sUserName,"%d",key);
			sprintf(tStreamCtrlInfo.m_sSerConId,"%d",5000-key);
			tStreamCtrlInfo.m_iCtrlCycle = i;
			tStreamCtrlInfo.m_iWarningNumber=i+1;
			SHMStreamCtrlInfo::UpdateStreamCtrlInfo(tStreamCtrlInfo);
			if (!SHMStreamCtrlInfo::GetStreamCtrlInfo(tStreamCtrlInfo)) {
				cout<<"tStreamCtrlInfo.m_sUserName="<<tStreamCtrlInfo.m_sUserName<<endl;
				cout<<"tStreamCtrlInfo.m_sSerConId="<<tStreamCtrlInfo.m_sSerConId<<endl;
				cout<<"tStreamCtrlInfo.m_iCtrlCycle="<<tStreamCtrlInfo.m_iCtrlCycle<<endl;
				cout<<"tStreamCtrlInfo.m_iWarningNumber="<<tStreamCtrlInfo.m_iWarningNumber<<endl<<endl;
			}
		}

		for (int i = 1; i<5000; i++){
			memset(&tStreamCtrlInfo,0,sizeof(StreamControlInfo));
			sprintf(tStreamCtrlInfo.m_sUserName,"%d",i);
			sprintf(tStreamCtrlInfo.m_sSerConId,"%d",5000-i);
			SHMStreamCtrlInfo::DelStreamCtrlInfo(tStreamCtrlInfo);
		}

		SHMStreamCtrlInfo::GetStreamCtrlInfoAll(vStreamCtrlInfo);
		cout<<"vStreamCtrlInfo.size()="<<vStreamCtrlInfo.size()<<endl;
		cout<<"-------------------------------------------------------------"<<endl;
		sleep(3);
	}

}

void testservicetxtlist(int value)
{
	SHMSGWInfoBase::servicetxtlist_init();
	ServiceContextListData sServiceContextListData;
	memset(&sServiceContextListData,0,sizeof(sServiceContextListData));

	if (!SHMServiceTxtList::GetServiceContextList(value,sServiceContextListData)) {
		cout<<sServiceContextListData.m_iServiceContextListId<<endl;
		cout<<sServiceContextListData.m_strServiceContextId<<endl;
		cout<<sServiceContextListData.m_iActive<<endl;
		cout<<sServiceContextListData.m_iUserSeq<<endl;
		cout<<sServiceContextListData.m_iSign<<endl;
		cout<<sServiceContextListData.m_strFlowId<<endl<<endl;
	}
	/*while (true){
		for (int i =1000001; i<=1052000; i++) {
			memset(&sServiceContextListData,0,sizeof(sServiceContextListData));
			sServiceContextListData.m_iUserSeq = i;
			sprintf(sServiceContextListData.m_strServiceContextId,"%ld",sServiceContextListData.m_iUserSeq);
			SHMServiceTxtList::AddServiceContextData(sServiceContextListData);
		}

		for (int i =1000001; i<=1052000; i++) {
			memset(&sServiceContextListData,0,sizeof(sServiceContextListData));
			sServiceContextListData.m_iUserSeq = i;
			sprintf(sServiceContextListData.m_strServiceContextId,"%ld",sServiceContextListData.m_iUserSeq);
			SHMServiceTxtList::DelServiceContextData(sServiceContextListData);
		}

		vector<ServiceContextListData> vServiceContextListData;
		SHMServiceTxtList::GetServiceContextListAll(vServiceContextListData);

		for (int i =0; i<vServiceContextListData.size(); i++) {
			cout<<vServiceContextListData[i].m_iUserSeq<<endl;
			cout<<vServiceContextListData[i].m_strServiceContextId<<endl<<endl;
		}
		return 1;
		usleep(500000);
	}*/
}

void testContentFiltering()
{
	ContextData _test;
	vector<ContextData>  vsContextInfo;
	SHMContentFiltering::contentfiltering_init();
	while (true){
		for (int i =1; i<100; i ++){
			memset(&_test,0,sizeof(ContextData));
			_test.m_iFilterSeq = i;
			SHMContentFiltering::AddContextFilterInfo(_test);
			sprintf(_test.m_strIllegalKeyword,"%d", i+100);
			SHMContentFiltering::UpdateContextFilterInfo(_test);
			SHMContentFiltering::GetContextFilterInfo(i,vsContextInfo);

			for (int j =0; j<vsContextInfo.size(); j++) {
				cout<<vsContextInfo[j].m_iFilterSeq<<endl;
				cout<<vsContextInfo[j].m_strIllegalKeyword<<endl;
			}
			SHMContentFiltering::DelContextFilterInfo(_test);
		}
		sleep(1);
	}
}

void testSHMStreamCtrlData()
{
	SHMStreamCtrlData::streamctrldata_init();
	StreamControlData tStreamCtrlData;
	memset(&tStreamCtrlData,0,sizeof(tStreamCtrlData));
	strcpy(tStreamCtrlData.m_sUserName,"dcc1@021.ctnbc-bon.net");
	strcpy(tStreamCtrlData.m_sSerConId,"CardQuery.VC@021.ctnbc-bon.net");
	if (!SHMStreamCtrlData::GetStreamCtrlData(tStreamCtrlData)) {
		for (int i =0; i<12; i++)
		{
			cout<<tStreamCtrlData.m_dMonthData[i][0]<<"; "<<tStreamCtrlData.m_dMonthData[i][1]<<endl;
		}
	}
	return -1;

	while (true){
		for (int i = 1; i<=2000;i++){
			memset(&tStreamCtrlData,0,sizeof(StreamControlData));
			sprintf(tStreamCtrlData.m_sUserName,"%d",i);
			sprintf(tStreamCtrlData.m_sSerConId,"%d",2001-i);
			SHMStreamCtrlData::AddStreamCtrlData(tStreamCtrlData);
		}
		for (int i =1; i<=5; i++ ) {
			memset(&tStreamCtrlData,0,sizeof(tStreamCtrlData));
			int key = rand()%1999+1;
			sprintf(tStreamCtrlData.m_sUserName,"%d",key);
			sprintf(tStreamCtrlData.m_sSerConId,"%d",2001-key);
			tStreamCtrlData.m_iSate = i;
			tStreamCtrlData.m_iWarned=i+1;
			SHMStreamCtrlData::UpdateStreamCtrlData(tStreamCtrlData);
			if (!SHMStreamCtrlData::GetStreamCtrlData(tStreamCtrlData)) {
				cout<<"tStreamCtrlData.m_sUserName="<<tStreamCtrlData.m_sUserName<<endl;
				cout<<"tStreamCtrlData.m_sSerConId="<<tStreamCtrlData.m_sSerConId<<endl;
				cout<<"tStreamCtrlData.m_iWarned="<<tStreamCtrlData.m_iWarned<<endl;
				cout<<"tStreamCtrlData.m_iSate="<<tStreamCtrlData.m_iSate<<endl<<endl;
			}
		}
		for (int i = 1; i<=2000;i++){
			memset(&tStreamCtrlData,0,sizeof(StreamControlData));
			sprintf(tStreamCtrlData.m_sUserName,"%d",i);
			sprintf(tStreamCtrlData.m_sSerConId,"%d",2001-i);
			SHMStreamCtrlData::DelStreamCtrlData(tStreamCtrlData);
		}
		cout<<"---------------------------------------------------"<<endl;
		sleep(5);
	}

}

void testSHMSgwSlaqueRel()
{
	SgwSlaQueue sSgwSlaQueue;
	SHMSgwSlaqueRel::sgwslaquerel_init();
	vector<SgwSlaQueue>  vSgwSlaQueue;

	while (true) {
		for (int i = 1; i<=2000; i++) {
			memset(&sSgwSlaQueue,0,sizeof(sSgwSlaQueue));
			sSgwSlaQueue.m_iQueuePriority = 1000+i;
			SHMSgwSlaqueRel::AddSlaQueueData(sSgwSlaQueue);
		}
		for (int i = 1; i<=5; i++) {
			memset(&sSgwSlaQueue,0,sizeof(sSgwSlaQueue));
			sSgwSlaQueue.m_iQueuePriority = 1000+rand()%1999+1;
			sSgwSlaQueue.m_iServiceLevel=rand()%10000;
			SHMSgwSlaqueRel::UpdateSlaQueueData(sSgwSlaQueue);
			if (!SHMSgwSlaqueRel::GetSlaQueueData(sSgwSlaQueue)) {
				cout<<"sSgwSlaQueue.m_iQueuePriority="<<sSgwSlaQueue.m_iQueuePriority<<endl;
				cout<<"sSgwSlaQueue.m_iServiceLevel="<<sSgwSlaQueue.m_iServiceLevel<<endl<<endl;
			}
		}
		for (int i = 1; i<=2000; i++) {
			memset(&sSgwSlaQueue,0,sizeof(sSgwSlaQueue));
			sSgwSlaQueue.m_iQueuePriority = 1000+i;
			SHMSgwSlaqueRel::DelSlaQueueData(sSgwSlaQueue);
		}
		SHMSgwSlaqueRel::GetSlaQueueAllData(vSgwSlaQueue);

		cout<<"vSgwSlaQueue.size() ="<<vSgwSlaQueue.size()<<endl;
		cout<<"------------------------------------------------------------"<<endl;
		sleep(5);
	}

}

void testSHMNetInfoData(int value)
{
	SHMNetInfoData::netinfo_init();
	NetInfoData sNetInfoData;
	vector<NetInfoData>  vNetInfoData;
	sNetInfoData.m_iNetinfoSeq = value;
	if (!SHMNetInfoData::GetNetInfoData(sNetInfoData)) {
		cout<<"sNetInfoData.m_iNetinfoSeq="<<sNetInfoData.m_iNetinfoSeq<<endl;
		cout<<"sNetInfoData.m_strNetinfoName="<<sNetInfoData.m_strNetinfoName<<endl;
		cout<<"sNetInfoData.m_iNodeId =" <<sNetInfoData.m_iNodeId<<endl<<endl;
	} else {
		cout<<"查询不到数据"<<endl;
	}
	cout<<SHMNetInfoData::DelNetInfoData(sNetInfoData)<<endl;

	if (!SHMNetInfoData::GetNetInfoData(sNetInfoData)) {
		cout<<"sNetInfoData.m_iNetinfoSeq="<<sNetInfoData.m_iNetinfoSeq<<endl;
		cout<<"sNetInfoData.m_strNetinfoName="<<sNetInfoData.m_strNetinfoName<<endl;
		cout<<"sNetInfoData.m_iNodeId =" <<sNetInfoData.m_iNodeId<<endl<<endl;
	} else {
		cout<<"查询不到数据"<<endl;
	}
	/*for (int i =1; i<=20000; i++){
		memset(&sNetInfoData,0,sizeof(sNetInfoData));
		sprintf(sNetInfoData.m_strNetinfoName,"NetinfoName_%4.4d",i);
		sNetInfoData.m_iNetinfoSeq = 1000+i;
		sNetInfoData.m_iNodeId=1;
		SHMNetInfoData::AddNetInfoData(sNetInfoData);
		if (!SHMNetInfoData::GetNetInfoData(sNetInfoData)) {
			cout<<"sNetInfoData.m_iNetinfoSeq="<<sNetInfoData.m_iNetinfoSeq<<endl;
			cout<<"sNetInfoData.m_strNetinfoName="<<sNetInfoData.m_strNetinfoName<<endl;
			cout<<"sNetInfoData.m_iNodeId =" <<sNetInfoData.m_iNodeId<<endl;
		}
		sNetInfoData.m_iNodeId = 2000;
		SHMNetInfoData::UpdateNetInfoData(sNetInfoData);

		if (!SHMNetInfoData::GetNetInfoData(sNetInfoData)) {
			cout<<"sNetInfoData.m_iNetinfoSeq="<<sNetInfoData.m_iNetinfoSeq<<endl;
			cout<<"sNetInfoData.m_strNetinfoName="<<sNetInfoData.m_strNetinfoName<<endl;
			cout<<"sNetInfoData.m_iNodeId =" <<sNetInfoData.m_iNodeId<<endl;
		}

		if (!SHMNetInfoData::GetNetInfoData(sNetInfoData.m_strNetinfoName,sNetInfoData)) {
			cout<<"sNetInfoData.m_iNetinfoSeq="<<sNetInfoData.m_iNetinfoSeq<<endl;
			cout<<"sNetInfoData.m_strNetinfoName="<<sNetInfoData.m_strNetinfoName<<endl;
			cout<<"sNetInfoData.m_iNodeId =" <<sNetInfoData.m_iNodeId<<endl;
		}
		cout<<"--------------------------------------------------------------------"<<endl;
		sleep(3);
		//break;
	}
	SHMNetInfoData::GetNetInfoAllData(vNetInfoData);
	cout<<"vNetInfoData.size()="<<vNetInfoData.size()<<endl;
	sleep(1);*/

}


void testSHMServiceTxtListBase()
{
	vector<ServiceContextListBaseData> vTxtBaseData;
	SHMServiceTxtListBase::servicetxtlistbase_init();
	SHMServiceTxtListBase::getServiceTxtListBaseAll(vTxtBaseData);
	cout<<vTxtBaseData.size()<<endl;
	/*for (int i =0; i<vTxtBaseData.size(); i++) {
		if (!SHMServiceTxtListBase::getServiceTxtListBase(vTxtBaseData[i])) {
			cout<<"vTxtBaseData[i].m_strServiceContextId="<<vTxtBaseData[i].m_strServiceContextId<<endl;
			cout<<"vTxtBaseData[i].m_strServiceContextIdName="<<vTxtBaseData[i].m_strServiceContextIdName<<endl;
			cout<<"vTxtBaseData[i].m_strServiceContextIdCode="<<vTxtBaseData[i].m_strServiceContextIdCode<<endl;
			cout<<"vTxtBaseData[i].m_strCreateDate="<<vTxtBaseData[i].m_strCreateDate<<endl;
			cout<<"vTxtBaseData[i].m_iServicePackageType="<<vTxtBaseData[i].m_iServicePackageType<<endl;
			cout<<"vTxtBaseData[i].m_iLogicMode="<<vTxtBaseData[i].m_iLogicMode<<endl;
			cout<<"vTxtBaseData[i].m_strServiceContextIdDesc="<<vTxtBaseData[i].m_strServiceContextIdDesc<<endl;
			cout<<"vTxtBaseData[i].m_iPriorityLevel="<<vTxtBaseData[i].m_iPriorityLevel<<endl;
			cout<<"vTxtBaseData[i].m_strMajorAvp="<<vTxtBaseData[i].m_strMajorAvp<<endl;
			cout<<"vTxtBaseData[i].m_iNext="<<vTxtBaseData[i].m_iNext<<endl<<endl;
			vTxtBaseData[i].m_iServicePackageType = 20000+i;
			vTxtBaseData[i].m_iLogicMode=i+10000;
			SHMServiceTxtListBase::updateServiceTxtListBase(vTxtBaseData[i]);
			cout<<"更新后的值："<<endl;
			if (!SHMServiceTxtListBase::getServiceTxtListBase(vTxtBaseData[i])) {
					cout<<"vTxtBaseData[i].m_strServiceContextId="<<vTxtBaseData[i].m_strServiceContextId<<endl;
					cout<<"vTxtBaseData[i].m_strServiceContextIdName="<<vTxtBaseData[i].m_strServiceContextIdName<<endl;
					cout<<"vTxtBaseData[i].m_strServiceContextIdCode="<<vTxtBaseData[i].m_strServiceContextIdCode<<endl;
					cout<<"vTxtBaseData[i].m_strCreateDate="<<vTxtBaseData[i].m_strCreateDate<<endl;
					cout<<"vTxtBaseData[i].m_iServicePackageType="<<vTxtBaseData[i].m_iServicePackageType<<endl;
					cout<<"vTxtBaseData[i].m_iLogicMode="<<vTxtBaseData[i].m_iLogicMode<<endl;
					cout<<"vTxtBaseData[i].m_strServiceContextIdDesc="<<vTxtBaseData[i].m_strServiceContextIdDesc<<endl;
					cout<<"vTxtBaseData[i].m_iPriorityLevel="<<vTxtBaseData[i].m_iPriorityLevel<<endl;
					cout<<"vTxtBaseData[i].m_strMajorAvp="<<vTxtBaseData[i].m_strMajorAvp<<endl;
					cout<<"vTxtBaseData[i].m_iNext="<<vTxtBaseData[i].m_iNext<<endl<<endl;
			}
			cout<<"-------------------------------------------------------------"<<endl;
		}
		sleep(3);
	}*/
	ServiceContextListBaseData sTxtBaseData;
	memset(&sTxtBaseData,0,sizeof(ServiceContextListBaseData));
	strcpy(sTxtBaseData.m_strServiceContextId,"RatableResourceQuery1.Balance");
	sTxtBaseData.m_iLogicMode = 1;
	sTxtBaseData.m_iPriorityLevel = 1;
	cout<<SHMServiceTxtListBase::AddServiceTxtListBase(sTxtBaseData)<<endl;
	if (!SHMServiceTxtListBase::getServiceTxtListBase(sTxtBaseData)) {
		cout<<"sTxtBaseData.m_strServiceContextId="<<sTxtBaseData.m_strServiceContextId<<endl;
		cout<<"sTxtBaseData.m_strServiceContextIdName="<<sTxtBaseData.m_strServiceContextIdName<<endl;
		cout<<"sTxtBaseData.m_strServiceContextIdCode="<<sTxtBaseData.m_strServiceContextIdCode<<endl;
		cout<<"sTxtBaseData.m_strCreateDate="<<sTxtBaseData.m_strCreateDate<<endl;
		cout<<"sTxtBaseData.m_iServicePackageType="<<sTxtBaseData.m_iServicePackageType<<endl;
		cout<<"sTxtBaseData.m_iLogicMode="<<sTxtBaseData.m_iLogicMode<<endl;
		cout<<"sTxtBaseData.m_strServiceContextIdDesc="<<sTxtBaseData.m_strServiceContextIdDesc<<endl;
		cout<<"sTxtBaseData.m_iPriorityLevel="<<sTxtBaseData.m_iPriorityLevel<<endl;
		cout<<"sTxtBaseData.m_strMajorAvp="<<sTxtBaseData.m_strMajorAvp<<endl;
		cout<<"sTxtBaseData.m_iNext="<<sTxtBaseData.m_iNext<<endl<<endl;
	}
	cout<<SHMServiceTxtListBase::DelServiceTxtListBase(sTxtBaseData)<<endl;
	if (!SHMServiceTxtListBase::getServiceTxtListBase(sTxtBaseData)) {
		cout<<"sTxtBaseData.m_strServiceContextId="<<sTxtBaseData.m_strServiceContextId<<endl;
		cout<<"sTxtBaseData.m_strServiceContextIdName="<<sTxtBaseData.m_strServiceContextIdName<<endl;
		cout<<"sTxtBaseData.m_strServiceContextIdCode="<<sTxtBaseData.m_strServiceContextIdCode<<endl;
		cout<<"sTxtBaseData.m_strCreateDate="<<sTxtBaseData.m_strCreateDate<<endl;
		cout<<"sTxtBaseData.m_iServicePackageType="<<sTxtBaseData.m_iServicePackageType<<endl;
		cout<<"sTxtBaseData.m_iLogicMode="<<sTxtBaseData.m_iLogicMode<<endl;
		cout<<"sTxtBaseData.m_strServiceContextIdDesc="<<sTxtBaseData.m_strServiceContextIdDesc<<endl;
		cout<<"sTxtBaseData.m_iPriorityLevel="<<sTxtBaseData.m_iPriorityLevel<<endl;
		cout<<"sTxtBaseData.m_strMajorAvp="<<sTxtBaseData.m_strMajorAvp<<endl;
		cout<<"sTxtBaseData.m_iNext="<<sTxtBaseData.m_iNext<<endl<<endl;
	}

}

long changekey(const char *_skey,int _iModValue)
{
	if (_skey == NULL||strlen(_skey) == 0){
		return -1;
	}
	//char _sKeyTemp[12] = {0};

	//int len = (strlen(_skey) <= 11) ? strlen(_skey) : 11;
	//snprintf(_sKeyTemp,len,"%s",_skey);//截取11个字节

	//int ikey = 0;
	unsigned long ulKey = 0;
	int len = strlen(_skey);
	static long sys[11] ={1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000};

	for (int i =0; i<len&&i<11 ;i++){
		//ikey = _skey[i]%10;
		//sprintf(_sKeyTemp+i,"%d",_skey[i]%10);
		//ulKey += pow(10,i)*(_skey[i]%10);
		ulKey += sys[i]*(_skey[i]%10);
	}
	//return (atol(_sKeyTemp)%(_iModValue-1));
	return (ulKey%(_iModValue-1));
}

unsigned long changekey1(const char *_sStr,unsigned int iBegin,unsigned int iEnd)
{
	static unsigned long ulValue = 0;
	static long sys[13]={1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000,100000000000};
	if (iBegin == iEnd) {
		ulValue+=sys[iBegin%12]*(_sStr[iBegin]%10);
		return ulValue;
	}
	int iMid= (iBegin+iEnd)/2;
	changekey1(_sStr,iBegin,iMid);
	changekey1(_sStr,iMid+1,iEnd);
}

unsigned long changekey2(const char *_sStr,unsigned int iBegin,unsigned int iEnd)
{
	static unsigned long ulValue = 0;
	static long sys[13]={1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000,100000000000};
	for (int i=iBegin; i<iEnd; i++) {
		ulValue+=sys[iBegin%12]*(_sStr[iBegin]%10);
	}
	return ulValue;
}

void testSHMBaseMethod()
{
	SHMBaseMethod::basemethod_init();
	CBaseMethod  sBaseMethod;
	vector<CBaseMethod>  vBaseMethod;

	while (true) {
		for (int i = 1; i<=200;i++) {
			memset(&sBaseMethod,0,sizeof(sBaseMethod));
			sBaseMethod.m_iBaseMethodID =i;
			SHMBaseMethod::addBaseMethod(sBaseMethod);
			sprintf(sBaseMethod.m_strBaseMethodName,"test_%d",i);
			SHMBaseMethod::updateBaseMethod(sBaseMethod);
			memset(&sBaseMethod,0,sizeof(sBaseMethod));
			if (!SHMBaseMethod::getBaseMethod(i,sBaseMethod)) {
				cout<<"sBaseMethod.m_iBaseMethodID="<<sBaseMethod.m_iBaseMethodID<<endl;
				cout<<"sBaseMethod.m_strBaseMethodName="<<sBaseMethod.m_strBaseMethodName<<endl<<endl;
			}
			sleep(1);
		}

		for (int i = 1; i<=200;i++) {
			memset(&sBaseMethod,0,sizeof(sBaseMethod));
			sBaseMethod.m_iBaseMethodID =i;
			SHMBaseMethod::deleteBaseMethod(sBaseMethod);
		}

		SHMBaseMethod::getBaseMethodAll(vBaseMethod);
		cout<<vBaseMethod.size()<<endl;
		sleep(1);
	}

}

void testSHMCongestLevel()
{
	SHMCongestLevel::congestleve_init();
	CongestLevel  sShmCongestLevel;

	while (true) {

		memset(&sShmCongestLevel,0,sizeof(sShmCongestLevel));
		sShmCongestLevel.m_iConfirmFlag = rand()%30;
		sShmCongestLevel.m_iGeneCpu = rand()%30;
		SHMCongestLevel::AddShmCongestLevel(sShmCongestLevel);

		sShmCongestLevel.m_iConfirmFlag = rand()%30+1;
		sShmCongestLevel.m_iGeneCpu = rand()%30+1;
		SHMCongestLevel::UpdateShmCongestLevel(sShmCongestLevel);

		if (!SHMCongestLevel::GetShmCongestLevel(sShmCongestLevel)) {
			cout<<sShmCongestLevel.m_iConfirmFlag<<endl;
			cout<<sShmCongestLevel.m_iGeneCpu<<endl;
		}
		sleep(1);
	}
}

void testSHMServicePackage(int value)
{
	SHMServicePackage::servicepackage_init();
	ServicePackageNode  sServicePackageNode;
	if (!SHMServicePackage::getServicePackageNode(value,sServicePackageNode)) {
		cout<<"sServicePackageNode.m_iFlowId="<<sServicePackageNode.m_iFlowId<<endl;
		cout<<"sServicePackageNode.m_iNodeSeq="<<sServicePackageNode.m_iNodeSeq<<endl;
		cout<<"sServicePackageNode.m_iNodeType="<<sServicePackageNode.m_iNodeType<<endl;
		cout<<"sServicePackageNode.m_strNodeName="<<sServicePackageNode.m_strNodeName<<endl;
		cout<<"sServicePackageNode.m_strNodeSourceID="<<sServicePackageNode.m_strNodeSourceID<<endl;
		cout<<"sServicePackageNode.m_strServiceContextId="<<sServicePackageNode.m_strServiceContextId<<endl<<endl;
	}else {
		cout<<"1.查询不到数据"<<endl;
	}
	SHMServicePackage::deleteServicePackageNode(sServicePackageNode);
	if (!SHMServicePackage::getServicePackageNode(value,sServicePackageNode)) {
		cout<<"sServicePackageNode.m_iFlowId="<<sServicePackageNode.m_iFlowId<<endl;
		cout<<"sServicePackageNode.m_iNodeSeq="<<sServicePackageNode.m_iNodeSeq<<endl;
		cout<<"sServicePackageNode.m_iNodeType="<<sServicePackageNode.m_iNodeType<<endl;
		cout<<"sServicePackageNode.m_strNodeName="<<sServicePackageNode.m_strNodeName<<endl;
		cout<<"sServicePackageNode.m_strNodeSourceID="<<sServicePackageNode.m_strNodeSourceID<<endl;
		cout<<"sServicePackageNode.m_strServiceContextId="<<sServicePackageNode.m_strServiceContextId<<endl<<endl;
	}else {
		cout<<"2.查询不到数据"<<endl;
	}

	SHMServicePackage::addServicePackageNode(sServicePackageNode);
	if (!SHMServicePackage::getServicePackageNode(value,sServicePackageNode)) {
		cout<<"sServicePackageNode.m_iFlowId="<<sServicePackageNode.m_iFlowId<<endl;
		cout<<"sServicePackageNode.m_iNodeSeq="<<sServicePackageNode.m_iNodeSeq<<endl;
		cout<<"sServicePackageNode.m_iNodeType="<<sServicePackageNode.m_iNodeType<<endl;
		cout<<"sServicePackageNode.m_strNodeName="<<sServicePackageNode.m_strNodeName<<endl;
		cout<<"sServicePackageNode.m_strNodeSourceID="<<sServicePackageNode.m_strNodeSourceID<<endl;
		cout<<"sServicePackageNode.m_strServiceContextId="<<sServicePackageNode.m_strServiceContextId<<endl<<endl;
	}else {
		cout<<"3.查询不到数据"<<endl;
	}
	sServicePackageNode.m_iFlowId = 1000;
	sServicePackageNode.m_iNodeType = 1000;
	SHMServicePackage::updateServicePackageNode(sServicePackageNode);
	if (!SHMServicePackage::getServicePackageNode(value,sServicePackageNode)) {
		cout<<"sServicePackageNode.m_iFlowId="<<sServicePackageNode.m_iFlowId<<endl;
		cout<<"sServicePackageNode.m_iNodeSeq="<<sServicePackageNode.m_iNodeSeq<<endl;
		cout<<"sServicePackageNode.m_iNodeType="<<sServicePackageNode.m_iNodeType<<endl;
		cout<<"sServicePackageNode.m_strNodeName="<<sServicePackageNode.m_strNodeName<<endl;
		cout<<"sServicePackageNode.m_strNodeSourceID="<<sServicePackageNode.m_strNodeSourceID<<endl;
		cout<<"sServicePackageNode.m_strServiceContextId="<<sServicePackageNode.m_strServiceContextId<<endl<<endl;
	}else {
		cout<<"4.查询不到数据"<<endl;
	}
	/*memset(&sServicePackageNode,0,sizeof(sServicePackageNode));
	if (!SHMServicePackage::getServicePackageNode(1,sServicePackageNode)) {
		cout<<"sServicePackageNode.m_iFlowId="<<sServicePackageNode.m_iFlowId<<endl;
		cout<<"sServicePackageNode.m_iNodeSeq="<<sServicePackageNode.m_iNodeSeq<<endl;
		cout<<"sServicePackageNode.m_iNodeType="<<sServicePackageNode.m_iNodeType<<endl;
		cout<<"sServicePackageNode.m_strNodeName="<<sServicePackageNode.m_strNodeName<<endl;
		cout<<"sServicePackageNode.m_strNodeSourceID="<<sServicePackageNode.m_strNodeSourceID<<endl;
		cout<<"sServicePackageNode.m_strServiceContextId="<<sServicePackageNode.m_strServiceContextId<<endl;
	}

	cout<<endl;
	vector<ServicePackageNode>  vServicePackageNode;
	SHMServicePackage::getServicePackageNodeALL("RatableResourceQuery.Balance@028.ctnbc-bon.net",vServicePackageNode);
	for(int i =0; i<vServicePackageNode.size(); i++) {
		cout<<"vServicePackageNode["<<i<<"].m_iFlowId="<<vServicePackageNode[i].m_iFlowId<<endl;
		cout<<"vServicePackageNode["<<i<<"].m_iNodeSeq="<<vServicePackageNode[i].m_iNodeSeq<<endl;
		cout<<"vServicePackageNode["<<i<<"].m_iNodeType="<<vServicePackageNode[i].m_iNodeType<<endl;
		cout<<"vServicePackageNode["<<i<<"].m_strNodeName="<<vServicePackageNode[i].m_strNodeName<<endl;
		cout<<"vServicePackageNode["<<i<<"].m_strNodeSourceID="<<vServicePackageNode[i].m_strNodeSourceID<<endl;
		cout<<"vServicePackageNode["<<i<<"].m_strServiceContextId="<<vServicePackageNode[i].m_strServiceContextId<<endl;
	}*/
}

void testSHMServicePackageRoute(int value)
{
	SHMServicePackageRoute::servicepackageroute_init();
	ServicePackageRoute  SPackageRoute;

	memset(&SPackageRoute,0,sizeof(SPackageRoute));
	if (!SHMServicePackageRoute::getServicePackageRoute(value,SPackageRoute)) {
		cout<<"ServicePackageRoute.m_iFlowId="<<SPackageRoute.m_iFlowId<<endl;
		cout<<"ServicePackageRoute.m_iFlowId="<<SPackageRoute.m_iFlowId<<endl;
		cout<<"ServicePackageRoute.m_iRouteSeq="<<SPackageRoute.m_iRouteSeq<<endl;
		cout<<"ServicePackageRoute.m_iSourceNodeSeq="<<SPackageRoute.m_iSourceNodeSeq<<endl;
		cout<<"ServicePackageRoute.m_iTargetNodeSeq="<<SPackageRoute.m_iTargetNodeSeq<<endl;
		cout<<"ServicePackageRoute.m_strServicePackageId="<<SPackageRoute.m_strServicePackageId<<endl<<endl;
	} else {
		cout<<"1.查询不到数据"<<endl;
	}
	SHMServicePackageRoute::deleteServicePackageRoute(SPackageRoute);

	if (!SHMServicePackageRoute::getServicePackageRoute(value,SPackageRoute)) {
		cout<<"ServicePackageRoute.m_iFlowId="<<SPackageRoute.m_iFlowId<<endl;
		cout<<"ServicePackageRoute.m_iFlowId="<<SPackageRoute.m_iFlowId<<endl;
		cout<<"ServicePackageRoute.m_iRouteSeq="<<SPackageRoute.m_iRouteSeq<<endl;
		cout<<"ServicePackageRoute.m_iSourceNodeSeq="<<SPackageRoute.m_iSourceNodeSeq<<endl;
		cout<<"ServicePackageRoute.m_iTargetNodeSeq="<<SPackageRoute.m_iTargetNodeSeq<<endl;
		cout<<"ServicePackageRoute.m_strServicePackageId="<<SPackageRoute.m_strServicePackageId<<endl<<endl;
	} else {
		cout<<"2.查询不到数据"<<endl;
	}
	SHMServicePackageRoute::addServicePackageRoute(SPackageRoute);

	if (!SHMServicePackageRoute::getServicePackageRoute(value,SPackageRoute)) {
		cout<<"ServicePackageRoute.m_iFlowId="<<SPackageRoute.m_iFlowId<<endl;
		cout<<"ServicePackageRoute.m_iFlowId="<<SPackageRoute.m_iFlowId<<endl;
		cout<<"ServicePackageRoute.m_iRouteSeq="<<SPackageRoute.m_iRouteSeq<<endl;
		cout<<"ServicePackageRoute.m_iSourceNodeSeq="<<SPackageRoute.m_iSourceNodeSeq<<endl;
		cout<<"ServicePackageRoute.m_iTargetNodeSeq="<<SPackageRoute.m_iTargetNodeSeq<<endl;
		cout<<"ServicePackageRoute.m_strServicePackageId="<<SPackageRoute.m_strServicePackageId<<endl<<endl;
	} else {
		cout<<"3.查询不到数据"<<endl;
	}

	SPackageRoute.m_iSourceNodeSeq=1000;
	SPackageRoute.m_iTargetNodeSeq = 100;

	SHMServicePackageRoute::updateServicePackageRoute(SPackageRoute);
	if (!SHMServicePackageRoute::getServicePackageRoute(value,SPackageRoute)) {
		cout<<"ServicePackageRoute.m_iFlowId="<<SPackageRoute.m_iFlowId<<endl;
		cout<<"ServicePackageRoute.m_iFlowId="<<SPackageRoute.m_iFlowId<<endl;
		cout<<"ServicePackageRoute.m_iRouteSeq="<<SPackageRoute.m_iRouteSeq<<endl;
		cout<<"ServicePackageRoute.m_iSourceNodeSeq="<<SPackageRoute.m_iSourceNodeSeq<<endl;
		cout<<"ServicePackageRoute.m_iTargetNodeSeq="<<SPackageRoute.m_iTargetNodeSeq<<endl;
		cout<<"ServicePackageRoute.m_strServicePackageId="<<SPackageRoute.m_strServicePackageId<<endl<<endl;
	} else {
		cout<<"4.查询不到数据"<<endl;
	}
	/*if (!SHMServicePackageRoute::getServicePackageRoute(1,SPackageRoute)) {
		cout<<"ServicePackageRoute.m_iFlowId="<<SPackageRoute.m_iFlowId<<endl;
		cout<<"ServicePackageRoute.m_iFlowId="<<SPackageRoute.m_iFlowId<<endl;
		cout<<"ServicePackageRoute.m_iRouteSeq="<<SPackageRoute.m_iRouteSeq<<endl;
		cout<<"ServicePackageRoute.m_iSourceNodeSeq="<<SPackageRoute.m_iSourceNodeSeq<<endl;
		cout<<"ServicePackageRoute.m_iTargetNodeSeq="<<SPackageRoute.m_iTargetNodeSeq<<endl;
		cout<<"ServicePackageRoute.m_strServicePackageId="<<SPackageRoute.m_strServicePackageId<<endl;
	}
	cout<<endl;

	vector<CServicePackageRoute> vServicePackageRoute;
	SHMServicePackageRoute::getServicePackageRouteALL("RatableResourceQuery.Balance@028.ctnbc-bon.net",vServicePackageRoute);

	for (int i =0; i<vServicePackageRoute.size(); i++) {
		cout<<"vServicePackageRoute["<<i<<"].m_iFlowId="<<vServicePackageRoute[i].m_iFlowId<<endl;
		cout<<"vServicePackageRoute["<<i<<"].m_iFlowId="<<vServicePackageRoute[i].m_iFlowId<<endl;
		cout<<"vServicePackageRoute["<<i<<"].m_iRouteSeq="<<vServicePackageRoute[i].m_iRouteSeq<<endl;
		cout<<"vServicePackageRoute["<<i<<"].m_iSourceNodeSeq="<<vServicePackageRoute[i].m_iSourceNodeSeq<<endl;
		cout<<"vServicePackageRoute["<<i<<"].m_iTargetNodeSeq="<<vServicePackageRoute[i].m_iTargetNodeSeq<<endl;
		cout<<"vServicePackageRoute["<<i<<"].m_strServicePackageId="<<vServicePackageRoute[i].m_strServicePackageId<<endl<<endl;

	}*/
}


void testSHMServicePackageRouteCCR(int value )
{
	SHMServicePackageRouteCCR::servicepackagerouteCCR_init();
	vector<CServicePackageRouteCCR>  vServicePackageRouteCCR;
	CServicePackageRouteCCR oServicePackageRouteCCR;
	//SHMServicePackageRouteCCR::getServicePackageRouteCCR(value,oServicePackageRouteCCR);
	if (!SHMServicePackageRouteCCR::getServicePackageRouteCCR(value,oServicePackageRouteCCR)) {
		cout<<oServicePackageRouteCCR.m_iRouteCCRSeq<<endl;
		cout<<oServicePackageRouteCCR.m_iRouteSeq<<endl;
		cout<<oServicePackageRouteCCR.m_strAvpName<<endl;
		cout<<oServicePackageRouteCCR.m_iAvpOption<<endl;
		cout<<oServicePackageRouteCCR.m_iFuncID<<endl;
	} else {
		cout<<"1.查询不到数据"<<endl;
	}
	SHMServicePackageRouteCCR::deleteServicePackageRouteCCR(oServicePackageRouteCCR);
	if (!SHMServicePackageRouteCCR::getServicePackageRouteCCR(value,oServicePackageRouteCCR)) {
		cout<<oServicePackageRouteCCR.m_iRouteCCRSeq<<endl;
		cout<<oServicePackageRouteCCR.m_iRouteSeq<<endl;
		cout<<oServicePackageRouteCCR.m_strAvpName<<endl;
		cout<<oServicePackageRouteCCR.m_iAvpOption<<endl;
		cout<<oServicePackageRouteCCR.m_iFuncID<<endl;
	} else {
		cout<<"2.查询不到数据"<<endl;
	}
	SHMServicePackageRouteCCR::addServicePackageRouteCCR(oServicePackageRouteCCR);
	if (!SHMServicePackageRouteCCR::getServicePackageRouteCCR(value,oServicePackageRouteCCR)) {
		cout<<oServicePackageRouteCCR.m_iRouteCCRSeq<<endl;
		cout<<oServicePackageRouteCCR.m_iRouteSeq<<endl;
		cout<<oServicePackageRouteCCR.m_strAvpName<<endl;
		cout<<oServicePackageRouteCCR.m_iAvpOption<<endl;
		cout<<oServicePackageRouteCCR.m_iFuncID<<endl;
	} else {
		cout<<"3.查询不到数据"<<endl;
	}
	oServicePackageRouteCCR.m_iAvpOption = 1000;
	oServicePackageRouteCCR.m_iFuncID = 1000;
	SHMServicePackageRouteCCR::updateServicePackageRouteCCR(oServicePackageRouteCCR);
	if (!SHMServicePackageRouteCCR::getServicePackageRouteCCR(value,oServicePackageRouteCCR)) {
		cout<<oServicePackageRouteCCR.m_iRouteCCRSeq<<endl;
		cout<<oServicePackageRouteCCR.m_iRouteSeq<<endl;
		cout<<oServicePackageRouteCCR.m_strAvpName<<endl;
		cout<<oServicePackageRouteCCR.m_iAvpOption<<endl;
		cout<<oServicePackageRouteCCR.m_iFuncID<<endl;
	} else {
		cout<<"4.查询不到数据"<<endl;
	}
	/*for (int i =0; i<vServicePackageRouteCCR.size(); i++) {
		cout<<vServicePackageRouteCCR[i].m_iRouteCCRSeq<<endl;
		cout<<vServicePackageRouteCCR[i].m_strAvpName<<endl;
		cout<<vServicePackageRouteCCR[i].m_iRouteSeq<<endl;
	}*/

	/*cout<<"******************"<<endl;
	CServicePackageRouteCCR sServicePackageRouteCCR;
	memset(&sServicePackageRouteCCR,0,sizeof(sServicePackageRouteCCR));
	if (!SHMServicePackageRouteCCR::getServicePackageRouteCCR(1,sServicePackageRouteCCR)) {
		cout<<sServicePackageRouteCCR.m_iRouteCCRSeq<<endl;
		cout<<sServicePackageRouteCCR.m_iRouteSeq<<endl;
		cout<<sServicePackageRouteCCR.m_strAvpName<<endl;
	}*/

}

void testSHMServicePackageJudge(int value)
{
	CServicePackageJudge  sServicePackageJudge;
	SHMServicePackageJudge::servicepackagejudge_init();

	if(!SHMServicePackageJudge::getServicePackageJudge(value,sServicePackageJudge)) {
		cout<<sServicePackageJudge.m_iJudgeConditionSeq<<endl;
		cout<<sServicePackageJudge.m_iNodeSeq<<endl;
		cout<<sServicePackageJudge.m_iJudgeConditionGroup<<endl;
		cout<<sServicePackageJudge.m_strConditionName<<endl;
		cout<<sServicePackageJudge.m_iOperaterID<<endl;
		cout<<sServicePackageJudge.m_iCompareLeft<<endl;
		cout<<sServicePackageJudge.m_iCompareRight<<endl<<endl;
	} else {
		cout<<"1.查询不到数据"<<endl;
	}
	SHMServicePackageJudge::deleteServicePackageJudge(sServicePackageJudge);
	if(!SHMServicePackageJudge::getServicePackageJudge(value,sServicePackageJudge)) {
		cout<<sServicePackageJudge.m_iJudgeConditionSeq<<endl;
		cout<<sServicePackageJudge.m_iNodeSeq<<endl;
		cout<<sServicePackageJudge.m_iJudgeConditionGroup<<endl;
		cout<<sServicePackageJudge.m_strConditionName<<endl;
		cout<<sServicePackageJudge.m_iOperaterID<<endl;
		cout<<sServicePackageJudge.m_iCompareLeft<<endl;
		cout<<sServicePackageJudge.m_iCompareRight<<endl<<endl;
	} else {
		cout<<"2.查询不到数据"<<endl;
	}

	SHMServicePackageJudge::addServicePackageJudge(sServicePackageJudge);
	if(!SHMServicePackageJudge::getServicePackageJudge(value,sServicePackageJudge)) {
		cout<<sServicePackageJudge.m_iJudgeConditionSeq<<endl;
		cout<<sServicePackageJudge.m_iNodeSeq<<endl;
		cout<<sServicePackageJudge.m_iJudgeConditionGroup<<endl;
		cout<<sServicePackageJudge.m_strConditionName<<endl;
		cout<<sServicePackageJudge.m_iOperaterID<<endl;
		cout<<sServicePackageJudge.m_iCompareLeft<<endl;
		cout<<sServicePackageJudge.m_iCompareRight<<endl<<endl;
	} else {
		cout<<"3.查询不到数据"<<endl;
	}

	sServicePackageJudge.m_iCompareLeft = 100;
	sServicePackageJudge.m_iJudgeConditionGroup = 1000;
	sServicePackageJudge.m_iCompareRight = 100;
	SHMServicePackageJudge::updateServicePackageJudge(sServicePackageJudge);
	if(!SHMServicePackageJudge::getServicePackageJudge(value,sServicePackageJudge)) {
		cout<<sServicePackageJudge.m_iJudgeConditionSeq<<endl;
		cout<<sServicePackageJudge.m_iNodeSeq<<endl;
		cout<<sServicePackageJudge.m_iJudgeConditionGroup<<endl;
		cout<<sServicePackageJudge.m_strConditionName<<endl;
		cout<<sServicePackageJudge.m_iOperaterID<<endl;
		cout<<sServicePackageJudge.m_iCompareLeft<<endl;
		cout<<sServicePackageJudge.m_iCompareRight<<endl<<endl;
	} else {
		cout<<"4.查询不到数据"<<endl;
	}

	/*cout<<"----------------------------"<<endl;
	vector<CServicePackageJudge>  vServicePackageJudge;

	SHMServicePackageJudge::getServicePackageJudgeAll(1,vServicePackageJudge);

	for (int i = 0; i<vServicePackageJudge.size(); i++) {
		cout<<vServicePackageJudge[i].m_iJudgeConditionSeq<<endl;
		cout<<vServicePackageJudge[i].m_iNodeSeq<<endl;
		cout<<vServicePackageJudge[i].m_iJudgeConditionGroup<<endl;
		cout<<vServicePackageJudge[i].m_strConditionName<<endl;
		cout<<vServicePackageJudge[i].m_iOperaterID<<endl;
		cout<<vServicePackageJudge[i].m_iCompareLeft<<endl;
		cout<<vServicePackageJudge[i].m_iCompareRight<<endl<<endl;
	}*/

}

void testSHMServicePackageCCA(int value)
{
	vector<CServicePackageCCA>  vServicePackageCCA;
	CServicePackageCCA oServicePackageCCA;
	SHMServicePackageCCA::servicepackagecca_init();

	if (!SHMServicePackageCCA::getServicePackageCCA(value,oServicePackageCCA)) {
		cout<<oServicePackageCCA.m_iServicePackageCCASeq<<endl;
		cout<<oServicePackageCCA.m_strServiceContextId<<endl;
		cout<<oServicePackageCCA.m_strAvpName<<endl;
		cout<<oServicePackageCCA.m_iAvpType<<endl;
		cout<<oServicePackageCCA.m_iAvpOption<<endl;
		cout<<oServicePackageCCA.m_iFuncID<<endl<<endl;
	} else {
		cout<<"1.查询不到数据"<<endl;
	}
	SHMServicePackageCCA::DelServicePackageCCA(oServicePackageCCA);
	if (!SHMServicePackageCCA::getServicePackageCCA(value,oServicePackageCCA)) {
		cout<<oServicePackageCCA.m_iServicePackageCCASeq<<endl;
		cout<<oServicePackageCCA.m_strServiceContextId<<endl;
		cout<<oServicePackageCCA.m_strAvpName<<endl;
		cout<<oServicePackageCCA.m_iAvpType<<endl;
		cout<<oServicePackageCCA.m_iAvpOption<<endl;
		cout<<oServicePackageCCA.m_iFuncID<<endl<<endl;
	} else {
		cout<<"2.查询不到数据"<<endl;
	}
	SHMServicePackageCCA::AddServicePackageCCA(oServicePackageCCA);
	if (!SHMServicePackageCCA::getServicePackageCCA(value,oServicePackageCCA)) {
		cout<<oServicePackageCCA.m_iServicePackageCCASeq<<endl;
		cout<<oServicePackageCCA.m_strServiceContextId<<endl;
		cout<<oServicePackageCCA.m_strAvpName<<endl;
		cout<<oServicePackageCCA.m_iAvpType<<endl;
		cout<<oServicePackageCCA.m_iAvpOption<<endl;
		cout<<oServicePackageCCA.m_iFuncID<<endl<<endl;
	} else {
		cout<<"3.查询不到数据"<<endl;
	}

	oServicePackageCCA.m_iAvpType = 100;
	oServicePackageCCA.m_iFuncID = 1000;
	SHMServicePackageCCA::UpdateServicePackageCCA(oServicePackageCCA);
	if (!SHMServicePackageCCA::getServicePackageCCA(value,oServicePackageCCA)) {
		cout<<oServicePackageCCA.m_iServicePackageCCASeq<<endl;
		cout<<oServicePackageCCA.m_strServiceContextId<<endl;
		cout<<oServicePackageCCA.m_strAvpName<<endl;
		cout<<oServicePackageCCA.m_iAvpType<<endl;
		cout<<oServicePackageCCA.m_iAvpOption<<endl;
		cout<<oServicePackageCCA.m_iFuncID<<endl<<endl;
	} else {
		cout<<"3.查询不到数据"<<endl;
	}
	/*SHMServicePackageCCA::getServicePackageCCA("abc",vServicePackageCCA);
	for (int i =0; i<vServicePackageCCA.size(); i++) {
		cout<<vServicePackageCCA[i].m_iServicePackageCCASeq<<endl;
		cout<<vServicePackageCCA[i].m_strServiceContextId<<endl;
		cout<<vServicePackageCCA[i].m_strAvpName<<endl;
		cout<<vServicePackageCCA[i].m_iAvpType<<endl;
		cout<<vServicePackageCCA[i].m_iAvpOption<<endl;
		cout<<vServicePackageCCA[i].m_iFuncID<<endl<<endl;
	}
	cout<<"-------------------"<<endl;
	CServicePackageCCA  sServicePackageCCA;
	if (!SHMServicePackageCCA::getServicePackageCCA(1,sServicePackageCCA)) {
		cout<<sServicePackageCCA.m_iServicePackageCCASeq<<endl;
		cout<<sServicePackageCCA.m_strServiceContextId<<endl;
		cout<<sServicePackageCCA.m_strAvpName<<endl;
		cout<<sServicePackageCCA.m_iAvpType<<endl;
		cout<<sServicePackageCCA.m_iAvpOption<<endl;
		cout<<sServicePackageCCA.m_iFuncID<<endl;
	}*/
}

void testSHMSCongestMsg()
{
	SHMSCongestMsg::scongestmsg_init();
	StatisticsCongestMsg sStatisticsCongestMsg;

	for (int i= 1; i<=500;i++) {
		memset(&sStatisticsCongestMsg,0,sizeof(sStatisticsCongestMsg));
		sprintf(sStatisticsCongestMsg.m_sServiceContextId,"test_%d",i);
		sStatisticsCongestMsg.m_iDiscardNum = 1;
		sStatisticsCongestMsg.m_iInMsgNum =1;
		sStatisticsCongestMsg.m_iOutMsgNum = 1;
		SHMSCongestMsg::InsertStatisticsCongestMsg(sStatisticsCongestMsg);
	}
	char key[18] = {0};
	int m_iDiscardNum = 0;
	int m_iInMsgNum = 0;
	int m_iOutMsgNum = 0;

	while (true) {
		memset(key,0,18);
		sprintf(key,"test_%d",rand()%500+1);
		if (!SHMSCongestMsg::GetStatisticsCongestMsg(key,sStatisticsCongestMsg)) {
			//cout<<sStatisticsCongestMsg.m_sServiceContextId<<endl;
			//cout<<sStatisticsCongestMsg.m_iDiscardNum<<endl;
			//cout<<sStatisticsCongestMsg.m_iInMsgNum<<endl;
			//cout<<sStatisticsCongestMsg.m_iOutMsgNum<<endl<<endl;
			m_iDiscardNum = sStatisticsCongestMsg.m_iDiscardNum;
			m_iInMsgNum = sStatisticsCongestMsg.m_iInMsgNum;
			m_iOutMsgNum = sStatisticsCongestMsg.m_iOutMsgNum;
		} else {
			cout<<__FILE__<<__LINE__<<";"<<"GetStatisticsCongestMsg FAILED"<<endl;
		}
		//cout<<"开始更新"<<endl;
		SHMSCongestMsg::UpdateStatisticsCongestDiscardMsg(key);
		SHMSCongestMsg::UpdateStatisticsCongestOutMsg(key,10);
		SHMSCongestMsg::UpdateStatisticsCongestInMsg(key,11);
		if (!SHMSCongestMsg::GetStatisticsCongestMsg(key,sStatisticsCongestMsg)) {
			//cout<<sStatisticsCongestMsg.m_sServiceContextId<<endl;
			//cout<<sStatisticsCongestMsg.m_iDiscardNum<<endl;
			//cout<<sStatisticsCongestMsg.m_iInMsgNum<<endl;
			//cout<<sStatisticsCongestMsg.m_iOutMsgNum<<endl<<endl;
			if ((sStatisticsCongestMsg.m_iDiscardNum-m_iDiscardNum)!=1) {
				cout<<__FILE__<<__LINE__<<";"<<"UpdateStatisticsCongestDiscardMsg FAILED"<<endl;
			}
			if ((sStatisticsCongestMsg.m_iInMsgNum-m_iInMsgNum)!=11) {
				cout<<__FILE__<<__LINE__<<";"<<"UpdateStatisticsCongestOutMsg FAILED"<<endl;
			}
			if ((sStatisticsCongestMsg.m_iOutMsgNum-m_iOutMsgNum)!=10) {
				cout<<__FILE__<<__LINE__<<";"<<"UpdateStatisticsCongestInMsg FAILED"<<endl;
			}
		} else {
			cout<<__FILE__<<__LINE__<<";"<<"GetStatisticsCongestMsg FAILED"<<endl;
		}

		vector<StatisticsCongestMsg> vStatisticsCongestMsg;
		SHMSCongestMsg::GetAllStatisticsCongestMsg(vStatisticsCongestMsg);
		if (vStatisticsCongestMsg.size()!=500) {
			cout<<__FILE__<<__LINE__<<";"<<"GetStatisticsCongestMsg FAILED"<<vStatisticsCongestMsg.size()<<endl;
		}
		sleep(3);
	}
}

void testSHMWfProcessMq()
{
	SHMWfProcessMq::wfprocessmq_init();
	ST_WF_PROCESS_MQ sStWfProcessMq;
	vector<ST_WF_PROCESS_MQ>  vStWfProcessMq;
	for (int i = 1; i<20000; i++)
	{
		sStWfProcessMq.m_iMqID = i;
		sStWfProcessMq.m_iProcessID = i;
		SHMWfProcessMq::addStWfProcessMq(sStWfProcessMq);
	}
	SHMWfProcessMq::getAllStWfProcessMq(vStWfProcessMq);
	cout<<vStWfProcessMq.size()<<endl<<endl;

	for (int i =1; i<20000; i++) {
		memset(&sStWfProcessMq,0,sizeof(sStWfProcessMq));
		sStWfProcessMq.m_iProcessID = i;
		if(!SHMWfProcessMq::getStWfProcessMq(sStWfProcessMq)) {
			cout<<sStWfProcessMq.m_iMqID<<endl;
			cout<<sStWfProcessMq.m_iProcessID<<endl<<endl;
		}
	}
}

void testSHMLoadBalance()
{
	vector<LoadBalanceData> vLodbalance;
	SHMLoadBalance::loadbalancedata_init();

   while (true) {
   		SHMLoadBalance::getLoadbalanceAll(vLodbalance);
		for (int i =0;i<vLodbalance.size();i++) {
			cout<<vLodbalance[i].SR_Host<<endl;
			cout<<vLodbalance[i].Load_Balance_Rate<<endl;
			vLodbalance[i].Load_Balance_Rate =rand()%99+1;
			SHMLoadBalance::updateLoadbalance(vLodbalance[i]);
		}
		cout<<"------------------------------------------------------------------"<<endl;
		SHMLoadBalance::getLoadbalanceAll(vLodbalance);
		for (int i =0;i<vLodbalance.size();i++){

			cout<<vLodbalance[i].SR_Host<<endl;
			cout<<vLodbalance[i].Load_Balance_Rate<<endl;
		}
		cout<<"------------------------------------------------------------------"<<endl;
		sleep(2);
	}
}

bool checkSessionClearData(const SessionClearData &oSessionClear,const SessionClearData &oQueryClear)
{
	if (strcmp(oSessionClear.m_sSessionID,oQueryClear.m_sSessionID) !=0 ) {
		return false;
	} else if (strcmp(oSessionClear.m_sServiceContextId,oQueryClear.m_sServiceContextId)!=0 ) {
		return false;
	} else if (oSessionClear.m_iMsgType!=oQueryClear.m_iMsgType) {
		return false;
	} else if (oSessionClear.m_lTime!=oQueryClear.m_lTime) {
		return false;
	} else if (oSessionClear.m_iIsFlag!=oQueryClear.m_iIsFlag) {
		return false;
	}
	return true;
}

void testSessionClear(const char *sHead)
{
	SHMSessionClear::sessionclear_init();
	SessionClearData oSessionClear;
	vector<SessionClearData> vClear;
	SessionClearData oQueryClear;
	int count = 0;
	struct timeval tstart;

	while (true) {
		gettimeofday(&tstart,NULL);
		memset(&oSessionClear,0,sizeof(oSessionClear));
		memset(&oQueryClear,0,sizeof(oQueryClear));
		sprintf(oSessionClear.m_sSessionID,"scsp03@03.ctnbc-bon.net;%s%d%d%d;43%d",sHead,tstart.tv_sec,tstart.tv_usec,rand()%1000,rand()%100);
		sprintf(oSessionClear.m_sServiceContextId,"scsp03@%02d.ctnbc-bon.net",rand()%10);
		oSessionClear.m_iMsgType= rand()%2;
		oSessionClear.m_lTime = rand()%1000000;
		oSessionClear.m_iIsFlag = rand()%2;
		SHMSessionClear::addSessionClear(oSessionClear);
		cout<<"----------------------------------------------------------"<<endl;
		cout<<"添加到内存的数据如下："<<endl;
		cout<<oSessionClear.m_sSessionID<<endl;
		cout<<oSessionClear.m_sServiceContextId<<endl;
		cout<<oSessionClear.m_iMsgType<<endl;
		cout<<oSessionClear.m_lTime<<endl;
		cout<<oSessionClear.m_iIsFlag<<endl<<endl;
		cout<<"查询的数据如下："<<endl;
		memcpy(&oQueryClear,&oSessionClear,sizeof(SessionClearData));
		if (!SHMSessionClear::getSessionClear(oQueryClear)) {
			cout<<oQueryClear.m_sSessionID<<endl;
			cout<<oQueryClear.m_sServiceContextId<<endl;
			cout<<oQueryClear.m_iMsgType<<endl;
			cout<<oQueryClear.m_lTime<<endl;
			cout<<oQueryClear.m_iIsFlag<<endl<<endl;
		} else {
			cout<<"getSessionClear failed "<<endl<<endl;
		}
		if (!checkSessionClearData(oSessionClear,oQueryClear)) {
			cout<<"查询的数据和添加的数据不一致，程序退出"<<endl;
			cout<<"查询的数据如下："<<endl;
			cout<<oQueryClear.m_sSessionID<<endl;
			cout<<oQueryClear.m_sServiceContextId<<endl;
			cout<<oQueryClear.m_iMsgType<<endl;
			cout<<oQueryClear.m_lTime<<endl;
			cout<<oQueryClear.m_iIsFlag<<endl<<endl;
			cout<<"添加到内存的数据如下："<<endl;
			cout<<oSessionClear.m_sSessionID<<endl;
			cout<<oSessionClear.m_sServiceContextId<<endl;
			cout<<oSessionClear.m_iMsgType<<endl;
			cout<<oSessionClear.m_lTime<<endl;
			cout<<oSessionClear.m_iIsFlag<<endl<<endl;
			return ;
		}
		cout<<"开始删除"<<endl;
		SHMSessionClear::delSessionClear(oSessionClear);
		cout<<"删除完成"<<endl<<endl;
		cout<<"查询删除的数据"<<endl;
		if (!SHMSessionClear::getSessionClear(oSessionClear)) {
			cout<<"errno"<<endl;
			cout<<oSessionClear.m_sSessionID<<endl;
			cout<<oSessionClear.m_sServiceContextId<<endl;
			cout<<oSessionClear.m_iMsgType<<endl;
			cout<<oSessionClear.m_lTime<<endl;
			cout<<oSessionClear.m_iIsFlag<<endl<<endl;
		}  else {
			cout<<"删除成功，进程查询不到已经删除的数据"<<endl<<endl;
		}
		SHMSessionClear::getSessionClearAll(vClear,100000);
		sleep(1);
	}
}
void testchangekey1()
{
	char skey[128]={0};
	char sValue[128] = {0};
	struct timeval tstart;
	gettimeofday(&tstart,NULL);
	unsigned long lvalue = 0;
	unsigned int count = 0;
	printf("1(精确到微妙):%ld.%ld\n",tstart.tv_sec,tstart.tv_usec);
	for (int i = 1; i<3000000;i++) {
		sprintf(skey,"%deree24qr2423",i);
		int temp = changekey1(skey,0,sizeof(skey));
		if (lvalue == temp) {
			cout<<count++<<endl;
			cout<<"skey="<<skey<<endl;
			cout<<lvalue<<endl;
		} else {
			lvalue = temp;
		}
	}
	gettimeofday(&tstart,NULL);
	printf("2(精确到微妙):%ld.%ld\n",tstart.tv_sec,tstart.tv_usec);
}

void testchangekey2()
{
	char skey[128]={0};
	char sValue[128] = {0};
	struct timeval tstart;
	gettimeofday(&tstart,NULL);
	unsigned long lvalue = 0;
	unsigned int count = 0;
	printf("1(精确到微妙):%ld.%ld\n",tstart.tv_sec,tstart.tv_usec);
	for (int i = 1; i<3000000;i++) {
		sprintf(skey,"%deree24qr2423",i);
		int temp = changekey2(skey,0,sizeof(skey));
		if (lvalue == temp) {
			cout<<count++<<endl;
			cout<<"skey="<<skey<<endl;
			cout<<lvalue<<endl;
		} else {
			lvalue = temp;
		}
	}
	gettimeofday(&tstart,NULL);
	printf("2(精确到微妙):%ld.%ld\n",tstart.tv_sec,tstart.tv_usec);
}
unsigned long lvalue = 0;
unsigned int count = 0;

void testOrgRoute(int sign,const char *svalue1,const char *svalue2,
				  const char *svalue3,const char *svalue4)
{
	int value3 = 0;
	SHMSGWInfoBase::sgworgroute_init();
	if (sign == 1) {
		SHMOrgRoute::GetRouteOrgID(atoi(svalue1),atoi(svalue2),value3);
	} else if (sign == 2) {
		SHMOrgRoute::GetRouteOrgIDUserCode(svalue1,atoi(svalue2),value3);
	} else if (sign == 3) {
		SHMOrgRoute::UpdateRouteOrg(atoi(svalue1),atoi(svalue2),atoi(svalue3),svalue4);
	} else if (sign == 4){
		value3 = SHMOrgRoute::ExpRouteOrg(svalue1);
	}
	cout<<value3<<endl;
}

void testOrgHead(int sign,const char *svalue1,const char *svalue2,
				 const char *svalue3,const char *svalue4,const char *svalue5)
{
	int value3 = 0;
	SHMSGWInfoBase::sgworghead_init();
	if (sign ==1) {
		SHMOrgHead::GetOrgIDbyNbr(svalue1,value3);
		cout<<value3<<endl;
	}else if (sign == 2) {
		SHMOrgHead::UpdateAccNbrHead(atoi(svalue1),svalue2,svalue3,svalue4,svalue5);
	}

}

int testchannel(char * pszChannelID)
{
	SgwChannelInfo sSgwChannelInfo;
	SHMSGWInfoBase::channelinfo_init();
	memset(&sSgwChannelInfo,0,sizeof(SgwChannelInfo));
	if (!SHMChannelInfo::GetChannelInfo(pszChannelID,sSgwChannelInfo)) {
		cout<<"m_szChannelID:"<<sSgwChannelInfo.m_szChannelID<<endl;
		cout<<"m_szChannelName:"<<sSgwChannelInfo.m_szChannelName<<endl;
		cout<<"m_szCheckFlag:"<<sSgwChannelInfo.m_szCheckFlag<<endl;
		cout<<"m_szEnCode:"<<sSgwChannelInfo.m_szEnCode<<endl;
		cout<<"m_szEffDate:"<<sSgwChannelInfo.m_szEffDate<<endl;
		cout<<"m_szExpDate:"<<sSgwChannelInfo.m_szExpDate<<endl;
		cout<<"m_szNetInfoCode:"<<sSgwChannelInfo.m_szNetInfoCode<<endl;
		cout<<"m_iUserSeq:"<<sSgwChannelInfo.m_iUserSeq<<endl;
	}
	memset(&sSgwChannelInfo,0,sizeof(SgwChannelInfo));
	strcpy(sSgwChannelInfo.m_szChannelID,pszChannelID);
	strcpy(sSgwChannelInfo.m_szChannelName,"test");
	strcpy(sSgwChannelInfo.m_szCheckFlag,"vc");
	SHMChannelInfo::UpdateChannelInfo(pszChannelID,sSgwChannelInfo);
	if (!SHMChannelInfo::GetChannelInfo(pszChannelID,sSgwChannelInfo)) {
		cout<<"修改后："<<endl;
		cout<<"m_szChannelID:"<<sSgwChannelInfo.m_szChannelID<<endl;
		cout<<"m_szChannelName:"<<sSgwChannelInfo.m_szChannelName<<endl;
		cout<<"m_szCheckFlag:"<<sSgwChannelInfo.m_szCheckFlag<<endl;
		cout<<"m_szEnCode:"<<sSgwChannelInfo.m_szEnCode<<endl;
		cout<<"m_szEffDate:"<<sSgwChannelInfo.m_szEffDate<<endl;
		cout<<"m_szExpDate:"<<sSgwChannelInfo.m_szExpDate<<endl;
		cout<<"m_szNetInfoCode:"<<sSgwChannelInfo.m_szNetInfoCode<<endl;
		cout<<"m_iUserSeq:"<<sSgwChannelInfo.m_iUserSeq<<endl;
	}
	SHMChannelInfo::DeleteChannelInfo(pszChannelID);
	if (!SHMChannelInfo::GetChannelInfo(pszChannelID,sSgwChannelInfo)) {
		cout<<"m_szChannelID:"<<sSgwChannelInfo.m_szChannelID<<endl;
		cout<<"m_szChannelName:"<<sSgwChannelInfo.m_szChannelName<<endl;
		cout<<"m_szCheckFlag:"<<sSgwChannelInfo.m_szCheckFlag<<endl;
		cout<<"m_szEnCode:"<<sSgwChannelInfo.m_szEnCode<<endl;
		cout<<"m_szEffDate:"<<sSgwChannelInfo.m_szEffDate<<endl;
		cout<<"m_szExpDate:"<<sSgwChannelInfo.m_szExpDate<<endl;
		cout<<"m_szNetInfoCode:"<<sSgwChannelInfo.m_szNetInfoCode<<endl;
		cout<<"m_iUserSeq:"<<sSgwChannelInfo.m_iUserSeq<<endl;
	} else {
		cout<<"删除后，查询不到数据！"<<endl;
	}

	SHMChannelInfo::AddChannelInfo(sSgwChannelInfo);
	if (!SHMChannelInfo::GetChannelInfo(pszChannelID,sSgwChannelInfo)) {
		cout<<"新增后："<<endl;
		cout<<"m_szChannelID:"<<sSgwChannelInfo.m_szChannelID<<endl;
		cout<<"m_szChannelName:"<<sSgwChannelInfo.m_szChannelName<<endl;
		cout<<"m_szCheckFlag:"<<sSgwChannelInfo.m_szCheckFlag<<endl;
		cout<<"m_szEnCode:"<<sSgwChannelInfo.m_szEnCode<<endl;
		cout<<"m_szEffDate:"<<sSgwChannelInfo.m_szEffDate<<endl;
		cout<<"m_szExpDate:"<<sSgwChannelInfo.m_szExpDate<<endl;
		cout<<"m_szNetInfoCode:"<<sSgwChannelInfo.m_szNetInfoCode<<endl;
		cout<<"m_iUserSeq:"<<sSgwChannelInfo.m_iUserSeq<<endl;
	}
}

void testSHMOrgRouteRule(char *value )
{
	SHMSGWInfoBase::orgrouterule_init();
	SgwOrgRouteRule sOrgRouteRule;
	memset(&sOrgRouteRule,0,sizeof(SgwOrgRouteRule));
	if (!SHMOrgRouteRule::GetOrgRouteRuleByOrg(atoi(value),sOrgRouteRule)) {
		cout<<sOrgRouteRule.m_iRouteID<<endl;
		cout<<sOrgRouteRule.m_iUserSeq<<endl;
		cout<<sOrgRouteRule.m_szNetInfoCode<<endl;
		cout<<sOrgRouteRule.m_iOrgID<<endl;
	} else {
		cout<<"查询不到数据！"<<endl;
	}

	SHMOrgRouteRule::DeleteOrgRouteRule(sOrgRouteRule);
	if (!SHMOrgRouteRule::GetOrgRouteRuleByOrg(atoi(value),sOrgRouteRule)) {
		cout<<sOrgRouteRule.m_iRouteID<<endl;
		cout<<sOrgRouteRule.m_iUserSeq<<endl;
		cout<<sOrgRouteRule.m_szNetInfoCode<<endl;
		cout<<sOrgRouteRule.m_iOrgID<<endl;
	} else {
		cout<<"删除后:"<<endl;
		cout<<"查询不到数据！"<<endl;
	}

	SHMOrgRouteRule::AddOrgRouteRule(sOrgRouteRule);
	if (!SHMOrgRouteRule::GetOrgRouteRuleByOrg(atoi(value),sOrgRouteRule)) {
		cout<<"增加后:"<<endl;
		cout<<sOrgRouteRule.m_iRouteID<<endl;
		cout<<sOrgRouteRule.m_iUserSeq<<endl;
		cout<<sOrgRouteRule.m_szNetInfoCode<<endl;
		cout<<sOrgRouteRule.m_iOrgID<<endl;
	} else {
		cout<<"查询不到数据！"<<endl;
	}

	sOrgRouteRule.m_iUserSeq = 100;
	SHMOrgRouteRule::UpdateOrgRouteRule(sOrgRouteRule.m_iRouteID,sOrgRouteRule);
	if (!SHMOrgRouteRule::GetOrgRouteRuleByOrg(atoi(value),sOrgRouteRule)) {
		cout<<"修改后"<<endl;
		cout<<sOrgRouteRule.m_iRouteID<<endl;
		cout<<sOrgRouteRule.m_iUserSeq<<endl;
		cout<<sOrgRouteRule.m_szNetInfoCode<<endl;
		cout<<sOrgRouteRule.m_iOrgID<<endl;
	} else {
		cout<<"查询不到数据！"<<endl;
	}

}

void testSHMAreaOrg()
{
	SHMAreaOrg::sgwareaorg_init();
	vector<SgwAreaOrg>vSgwAreaOrg;
	SHMAreaOrg::GetAllAreaOrg(vSgwAreaOrg);
	for (int i =0; i<vSgwAreaOrg.size(); i++) {
		cout<<"m_iAreaSeq:"<<vSgwAreaOrg[i].m_iAreaSeq<<endl;
		cout<<"m_iUserSeq:"<<vSgwAreaOrg[i].m_iUserSeq<<endl;
		cout<<"m_iOrgID:"<<vSgwAreaOrg[i].m_iOrgID<<endl;
		vSgwAreaOrg[i].m_iUserSeq = i;
		vSgwAreaOrg[i].m_iOrgID = i;
		SHMAreaOrg::UpdateAreaOrg(vSgwAreaOrg[i].m_iAreaSeq,vSgwAreaOrg[i]);
	}
	SHMAreaOrg::GetAllAreaOrg(vSgwAreaOrg);
	for (int i =0; i<vSgwAreaOrg.size(); i++) {
		cout<<"m_iAreaSeq:"<<vSgwAreaOrg[i].m_iAreaSeq<<endl;
		cout<<"m_iUserSeq:"<<vSgwAreaOrg[i].m_iUserSeq<<endl;
		cout<<"m_iOrgID:"<<vSgwAreaOrg[i].m_iOrgID<<endl;
	}
}

void testSHMTuxRelaIn(char *value)
{

	SHMTuxRelaIn::sgwtuxrelain_init();
	SgwTuxedoRelationIn sTuxedoRelation;

	if (!SHMTuxRelaIn::GetServicesContextID(atoi(value),sTuxedoRelation)) {
		cout<<sTuxedoRelation.m_iRelationID<<endl;
		cout<<sTuxedoRelation.m_iServicesID<<endl;
		cout<<sTuxedoRelation.m_szServiceContextID<<endl;
	}
}

void testSHMTuxRelaOut(char *value)
{
	SHMTuxRelaOut::sgwtuxrelaout_init();
	SgwTuxedoRelationOut sTuxedoRelation;
	memset(&sTuxedoRelation,0,sizeof(sTuxedoRelation));
	if (!SHMTuxRelaOut::GetServicesID(value,sTuxedoRelation)) {
		cout<<sTuxedoRelation.m_iRelationID<<endl;
		cout<<sTuxedoRelation.m_iServicesID<<endl;
		cout<<sTuxedoRelation.m_szServiceContextID<<endl;
	}
}

void testSHMSvrList(char *value )
{
	SHMSvrList::sgwsvrlist_init();
	SgwServicesList sServicesList;
	memset(&sServicesList,0,sizeof(sServicesList));

	if (!SHMSvrList::GetServicesInfo(value,sServicesList)) {
		cout<<"m_iServicesID:"<<sServicesList.m_iServicesID<<endl;
		cout<<"m_szEffDate:"<<sServicesList.m_szEffDate<<endl;
		cout<<"m_szExpDate:"<<sServicesList.m_szExpDate<<endl;
		cout<<"m_szServicesDesc:"<<sServicesList.m_szServicesDesc<<endl;
		cout<<"m_szServicesName:"<<sServicesList.m_szServicesName<<endl;
		cout<<"m_iNext:"<<sServicesList.m_iNext<<endl;
	} else {
		cout<<"查询不到数据"<<endl;
	}

//	strcpy(sServicesList.m_szExpDate,"20110101150000");
//	cout<<sServicesList.m_szExpDate<<endl;
//	SHMSvrList::UpdateServicesInfo(sServicesList);
//	if(!SHMSvrList::GetServicesInfo(value,sServicesList))
//	{
//		cout<<"修改后："<<endl;
//		cout<<"m_iServicesID:"<<sServicesList.m_iServicesID<<endl;
//		cout<<"m_szEffDate:"<<sServicesList.m_szEffDate<<endl;
//		cout<<"m_szExpDate:"<<sServicesList.m_szExpDate<<endl;
//		cout<<"m_szServicesDesc:"<<sServicesList.m_szServicesDesc<<endl;
//		cout<<"m_szServicesName:"<<sServicesList.m_szServicesName<<endl;
//		cout<<"m_iNext:"<<sServicesList.m_iNext<<endl;
//	}else{
//		cout<<"查询不到数据"<<endl;
//	}

//	SHMSvrList::DeleteServicesInfo(sServicesList);
//	if(!SHMSvrList::GetServicesInfo(value,sServicesList))
//	{
//		cout<<"m_iServicesID:"<<sServicesList.m_iServicesID<<endl;
//		cout<<"m_szEffDate:"<<sServicesList.m_szEffDate<<endl;
//		cout<<"m_szExpDate:"<<sServicesList.m_szExpDate<<endl;
//		cout<<"m_szServicesDesc:"<<sServicesList.m_szServicesDesc<<endl;
//		cout<<"m_szServicesName:"<<sServicesList.m_szServicesName<<endl;
//		cout<<"m_iNext:"<<sServicesList.m_iNext<<endl;
//	}else{
//		cout<<"删除后："<<endl;
//		cout<<"查询不到数据"<<endl;
//	}


	sServicesList.m_iServicesID = 1018;
	strcpy(sServicesList.m_szEffDate,"20110101140000");
	strcpy(sServicesList.m_szExpDate,"30000101");
	strcpy(sServicesList.m_szServicesDesc,"缴费批量查询");
	strcpy(sServicesList.m_szServicesName,"GET_OWE_BY_FILE");
	SHMSvrList::AddServicesInfo(sServicesList);
	if(!SHMSvrList::GetServicesInfo(value,sServicesList))
	{
		cout<<"添加后："<<endl;
		cout<<"m_iServicesID:"<<sServicesList.m_iServicesID<<endl;
		cout<<"m_szEffDate:"<<sServicesList.m_szEffDate<<endl;
		cout<<"m_szExpDate:"<<sServicesList.m_szExpDate<<endl;
		cout<<"m_szServicesDesc:"<<sServicesList.m_szServicesDesc<<endl;
		cout<<"m_szServicesName:"<<sServicesList.m_szServicesName<<endl;
		cout<<"m_iNext:"<<sServicesList.m_iNext<<endl;
	}else{
		cout<<"查询不到数据"<<endl;
	}
}

void testSHMSrvsParamRela(char *value)
{
	SHMSrvsParamRela::sgwsrvsparamrela_init();
	SgwServicesParamRelation sServicesParamRelation;

//	vector<SgwServicesParamRelation> vServicesParam;
//	SHMSrvsParamRela::GetServicesParam(atoi(value),vServicesParam);
//	for (int i =0; i<vServicesParam.size(); i++)
//	{
//		cout<<"SERVICES_ID:"<<vServicesParam[i].m_iServicesID<<endl;
//		cout<<"PARAM_ID:"<<vServicesParam[i].m_iParamID<<endl;
//		cout<<"PARAM_FLAG:"<<vServicesParam[i].m_iParamFlag<<endl;
//		cout<<"MAPPING_ID:"<<vServicesParam[i].m_iMappingID<<endl;
//		cout<<"iNext:"<<vServicesParam[i].m_iNext<<endl<<endl;
//	}
	if(!SHMSrvsParamRela::GetServicesParam(atoi(value),sServicesParamRelation))
	{
		cout<<"SERVICES_ID="<<sServicesParamRelation.m_iServicesID<<endl;
		cout<<"PARAM_ID="<<sServicesParamRelation.m_iParamID<<endl;
		cout<<"PARAM_FLAG="<<sServicesParamRelation.m_iParamFlag<<endl;
		cout<<"MAPPING_ID="<<sServicesParamRelation.m_iMappingID<<endl;
		cout<<"iNext="<<sServicesParamRelation.m_iNext<<endl<<endl;
	}else{
		cout<<"查询不到数据"<<endl;
	}

//	sServicesParamRelation.m_iParamFlag = 3;
//	SHMSrvsParamRela::UpdateServicesParam(sServicesParamRelation);
//	if(!SHMSrvsParamRela::GetServicesParam(atoi(value),sServicesParamRelation))
//	{
//		cout<<"修改后："<<endl;
//		cout<<"SERVICES_ID="<<sServicesParamRelation.m_iServicesID<<endl;
//		cout<<"PARAM_ID="<<sServicesParamRelation.m_iParamID<<endl;
//		cout<<"PARAM_FLAG="<<sServicesParamRelation.m_iParamFlag<<endl;
//		cout<<"MAPPING_ID="<<sServicesParamRelation.m_iMappingID<<endl;
//		cout<<"iNext="<<sServicesParamRelation.m_iNext<<endl<<endl;
//	}else{
//		cout<<"查询不到数据"<<endl;
//	}

	SHMSrvsParamRela::DeleteServicesParam(sServicesParamRelation);
	if(!SHMSrvsParamRela::GetServicesParam(atoi(value),sServicesParamRelation))
	{
		cout<<"SERVICES_ID="<<sServicesParamRelation.m_iServicesID<<endl;
		cout<<"PARAM_ID="<<sServicesParamRelation.m_iParamID<<endl;
		cout<<"PARAM_FLAG="<<sServicesParamRelation.m_iParamFlag<<endl;
		cout<<"MAPPING_ID="<<sServicesParamRelation.m_iMappingID<<endl;
		cout<<"iNext="<<sServicesParamRelation.m_iNext<<endl<<endl;
	}else{
		cout<<"删除后："<<endl;
		cout<<"查询不到数据"<<endl;
	}

//	sServicesParamRelation.m_iMappingID = 656;
//	SHMSrvsParamRela::AddServicesParam(sServicesParamRelation);
//	if(!SHMSrvsParamRela::GetServicesParam(656,sServicesParamRelation))
//	{
//		cout<<"添加后："<<endl;
//		cout<<"SERVICES_ID="<<sServicesParamRelation.m_iServicesID<<endl;
//		cout<<"PARAM_ID="<<sServicesParamRelation.m_iParamID<<endl;
//		cout<<"PARAM_FLAG="<<sServicesParamRelation.m_iParamFlag<<endl;
//		cout<<"MAPPING_ID="<<sServicesParamRelation.m_iMappingID<<endl;
//		cout<<"iNext="<<sServicesParamRelation.m_iNext<<endl<<endl;
//	}else{
//		cout<<"查询不到数据"<<endl;
//	}
}

void testSHMSrvsParamList(char *value)
{
	SHMSrvsParamList::sgwsrvsparamlist_init();
	SgwServicesParamList sServicesparamList;
	memset(&sServicesparamList,0,sizeof(sServicesparamList));
	if (!SHMSrvsParamList::GetServiceParamList(atoi(value),sServicesparamList)) {
		cout<<"m_iParamID:"<<sServicesparamList.m_iParamID<<endl;
		cout<<"m_szParamName:"<<sServicesparamList.m_szParamName<<endl;
		cout<<"m_iParamVal:"<<sServicesparamList.m_iParamVal<<endl;
		cout<<"m_iParamType:"<<sServicesparamList.m_iParamType<<endl;
		cout<<"m_szParamDesc:"<<sServicesparamList.m_szParamDesc<<endl;
	}
}

void testSHMSrvsRegister(char *value,char *value1,char *value2)
{
	SgwServiceRegister sServiceRegister;
	memset(&sServiceRegister,0,sizeof(sServiceRegister));
	SHMSrvsRegister::sgwsrvsregister_init();
	if (atoi(value) == 1) {
		if (!SHMSrvsRegister::GetServiceRegisterBySrc(value1,atoi(value2),sServiceRegister)) {
			cout<<"m_iServicesInstanceID:"<<sServiceRegister.m_iServicesInstanceID<<endl;
			cout<<"m_szChannelID:"<<sServiceRegister.m_szChannelID<<endl;
			cout<<"m_szStateDate:"<<sServiceRegister.m_szStateDate<<endl;
			cout<<"m_iSrcServicesID:"<<sServiceRegister.m_iSrcServicesID<<endl;
			cout<<"m_szSrcServicesName:"<<sServiceRegister.m_szSrcServicesName<<endl;
			cout<<"m_iMapServicesID:"<<sServiceRegister.m_iMapServicesID<<endl;
			cout<<"m_szMapServicesName:"<<sServiceRegister.m_szMapServicesName<<endl;
			cout<<"m_iAreaUser:"<<sServiceRegister.m_iAreaUser<<endl;
			cout<<"m_szState:"<<sServiceRegister.m_szState<<endl;
		}
	} else {
		if (!SHMSrvsRegister::GetServiceRegisterByMap(value1,atoi(value2),sServiceRegister)) {
			cout<<"m_iServicesInstanceID:"<<sServiceRegister.m_iServicesInstanceID<<endl;
			cout<<"m_szChannelID:"<<sServiceRegister.m_szChannelID<<endl;
			cout<<"m_szStateDate:"<<sServiceRegister.m_szStateDate<<endl;
			cout<<"m_iSrcServicesID:"<<sServiceRegister.m_iSrcServicesID<<endl;
			cout<<"m_szSrcServicesName:"<<sServiceRegister.m_szSrcServicesName<<endl;
			cout<<"m_iMapServicesID:"<<sServiceRegister.m_iMapServicesID<<endl;
			cout<<"m_szMapServicesName:"<<sServiceRegister.m_szMapServicesName<<endl;
			cout<<"m_iAreaUser:"<<sServiceRegister.m_iAreaUser<<endl;
			cout<<"m_szState:"<<sServiceRegister.m_szState<<endl;
		}
	}
}

void testSHMParamInsList(char *value)
{
	vector<SgwParamInstanceList> vInstanceList;
	SHMParamInsList::sgwparaminslist_init();

//	SHMParamInsList::GetParamInstanceList(atoi(value),vInstanceList);
//	for (int  i =0; i<vInstanceList.size();i++){
//		cout<<"m_iServicesInstanceID:"<<vInstanceList[i].m_iServicesInstanceID<<endl;
//		cout<<"m_iSrcParamID:"<<vInstanceList[i].m_iSrcParamID<<endl;
//		cout<<"m_szSrcParamName:"<<vInstanceList[i].m_szSrcParamName<<endl;
//		cout<<"m_iSrcParamType:"<<vInstanceList[i].m_iSrcParamType<<endl;
//		cout<<"m_iSrcParamVal:"<<vInstanceList[i].m_iSrcParamVal<<endl;
//		cout<<"m_iDestParamID:"<<vInstanceList[i].m_iDestParamID<<endl;
//		cout<<"m_szDestParamName:"<<vInstanceList[i].m_szDestParamName<<endl;
//		cout<<"m_iDestParamType:"<<vInstanceList[i].m_iDestParamType<<endl;
//		cout<<"m_iDestParamVal:"<<vInstanceList[i].m_iDestParamVal<<endl;
//		cout<<"m_szSrcParamDesc:"<<vInstanceList[i].m_szSrcParamDesc<<endl;
//		cout<<"m_szDestParamDesc:"<<vInstanceList[i].m_szDestParamDesc<<endl;
//		cout<<"m_iParamFlag:"<<vInstanceList[i].m_iParamFlag<<endl;
//		cout<<"m_iParamLog:"<<vInstanceList[i].m_iParamLog<<endl;
//	}
	SgwParamInstanceList oInstanceList;
	if(!SHMParamInsList::GetParamInstanceList(atoi(value),oInstanceList)){
		cout<<"m_iServicesInstanceID:"<<oInstanceList.m_iServicesInstanceID<<endl;
		cout<<"m_iSrcParamID:"<<oInstanceList.m_iSrcParamID<<endl;
		cout<<"m_szSrcParamName:"<<oInstanceList.m_szSrcParamName<<endl;
		cout<<"m_iSrcParamType:"<<oInstanceList.m_iSrcParamType<<endl;
		cout<<"m_iSrcParamVal:"<<oInstanceList.m_iSrcParamVal<<endl;
		cout<<"m_iDestParamID:"<<oInstanceList.m_iDestParamID<<endl;
		cout<<"m_szDestParamName:"<<oInstanceList.m_szDestParamName<<endl;
		cout<<"m_iDestParamType:"<<oInstanceList.m_iDestParamType<<endl;
		cout<<"m_iDestParamVal:"<<oInstanceList.m_iDestParamVal<<endl;
		cout<<"m_szSrcParamDesc:"<<oInstanceList.m_szSrcParamDesc<<endl;
		cout<<"m_szDestParamDesc:"<<oInstanceList.m_szDestParamDesc<<endl;
		cout<<"m_iParamFlag:"<<oInstanceList.m_iParamFlag<<endl;
		cout<<"m_iParamLog:"<<oInstanceList.m_iParamLog<<endl;
	}
	SHMParamInsList::DeleteParamInstanceList(oInstanceList);
	if(!SHMParamInsList::GetParamInstanceList(atoi(value),oInstanceList)){
		cout<<"m_iServicesInstanceID:"<<oInstanceList.m_iServicesInstanceID<<endl;
		cout<<"m_iSrcParamID:"<<oInstanceList.m_iSrcParamID<<endl;
		cout<<"m_szSrcParamName:"<<oInstanceList.m_szSrcParamName<<endl;
		cout<<"m_iSrcParamType:"<<oInstanceList.m_iSrcParamType<<endl;
		cout<<"m_iSrcParamVal:"<<oInstanceList.m_iSrcParamVal<<endl;
		cout<<"m_iDestParamID:"<<oInstanceList.m_iDestParamID<<endl;
		cout<<"m_szDestParamName:"<<oInstanceList.m_szDestParamName<<endl;
		cout<<"m_iDestParamType:"<<oInstanceList.m_iDestParamType<<endl;
		cout<<"m_iDestParamVal:"<<oInstanceList.m_iDestParamVal<<endl;
		cout<<"m_szSrcParamDesc:"<<oInstanceList.m_szSrcParamDesc<<endl;
		cout<<"m_szDestParamDesc:"<<oInstanceList.m_szDestParamDesc<<endl;
		cout<<"m_iParamFlag:"<<oInstanceList.m_iParamFlag<<endl;
		cout<<"m_iParamLog:"<<oInstanceList.m_iParamLog<<endl;
	} else {
		cout<<"删除后查询不到数据！"<<endl;
	}
	SHMParamInsList::AddParamInstanceList(oInstanceList);
	if(!SHMParamInsList::GetParamInstanceList(atoi(value),oInstanceList)){
		cout<<"m_iServicesInstanceID:"<<oInstanceList.m_iServicesInstanceID<<endl;
		cout<<"m_iSrcParamID:"<<oInstanceList.m_iSrcParamID<<endl;
		cout<<"m_szSrcParamName:"<<oInstanceList.m_szSrcParamName<<endl;
		cout<<"m_iSrcParamType:"<<oInstanceList.m_iSrcParamType<<endl;
		cout<<"m_iSrcParamVal:"<<oInstanceList.m_iSrcParamVal<<endl;
		cout<<"m_iDestParamID:"<<oInstanceList.m_iDestParamID<<endl;
		cout<<"m_szDestParamName:"<<oInstanceList.m_szDestParamName<<endl;
		cout<<"m_iDestParamType:"<<oInstanceList.m_iDestParamType<<endl;
		cout<<"m_iDestParamVal:"<<oInstanceList.m_iDestParamVal<<endl;
		cout<<"m_szSrcParamDesc:"<<oInstanceList.m_szSrcParamDesc<<endl;
		cout<<"m_szDestParamDesc:"<<oInstanceList.m_szDestParamDesc<<endl;
		cout<<"m_iParamFlag:"<<oInstanceList.m_iParamFlag<<endl;
		cout<<"m_iParamLog:"<<oInstanceList.m_iParamLog<<endl;
	}
}

void testSHMUserStaff(int iSign,int value)
{
	SHMUserStaff::userstaff_init();
	SgwUserStaff sSgwUserStaff;
	memset(&sSgwUserStaff,0,sizeof(sSgwUserStaff));

	if (iSign == 1) {
		if (!SHMUserStaff::GetUserStaffByStarr(value,sSgwUserStaff)) {
			cout<<"m_iUserStaffID:"<<sSgwUserStaff.m_iUserStaffID<<endl;
			cout<<"m_iUserStaffID:"<<sSgwUserStaff.m_lStaffID<<endl;
			cout<<"m_szStaffName:"<<sSgwUserStaff.m_szStaffName<<endl;
			cout<<"m_szChannelID:"<<sSgwUserStaff.m_szChannelID<<endl;
		}
	} else {
		if (!SHMUserStaff::GetUserStaffByUserStarr(value,sSgwUserStaff)) {
			cout<<"m_iUserStaffID:"<<sSgwUserStaff.m_iUserStaffID<<endl;
			cout<<"m_iUserStaffID:"<<sSgwUserStaff.m_lStaffID<<endl;
			cout<<"m_szStaffName:"<<sSgwUserStaff.m_szStaffName<<endl;
			cout<<"m_szChannelID:"<<sSgwUserStaff.m_szChannelID<<endl;
		}
	}
}

void testSHMUserStaff1(int value)
{
	SHMUserStaff::userstaff_init();
	SgwUserStaff sSgwUserStaff;
	SgwUserStaff sSgwUserStaff1;
	memset(&sSgwUserStaff,0,sizeof(sSgwUserStaff));
	memset(&sSgwUserStaff1,0,sizeof(sSgwUserStaff1));


	if (!SHMUserStaff::GetUserStaffByStarr(value,sSgwUserStaff)) {
		cout<<"m_iUserStaffID:"<<sSgwUserStaff.m_iUserStaffID<<endl;
		cout<<"m_iUserStaffID:"<<sSgwUserStaff.m_lStaffID<<endl;
		cout<<"m_szStaffName:"<<sSgwUserStaff.m_szStaffName<<endl;
		cout<<"m_szChannelID:"<<sSgwUserStaff.m_szChannelID<<endl;
		cout<<"m_iNext:"<<sSgwUserStaff.m_iNext<<endl;
	}
//	SHMUserStaff::DeleteUserStaff(sSgwUserStaff);
	memcpy(&sSgwUserStaff1,&sSgwUserStaff,sizeof(sSgwUserStaff));
	sSgwUserStaff1.m_lStaffID = 11;
	SHMUserStaff::UpdateUserStaff(sSgwUserStaff.m_iUserStaffID,sSgwUserStaff1);
	if (!SHMUserStaff::GetUserStaffByStarr(sSgwUserStaff.m_lStaffID,sSgwUserStaff)) {
		cout<<"修改后："<<endl;
		cout<<"m_iUserStaffID:"<<sSgwUserStaff.m_iUserStaffID<<endl;
		cout<<"m_iUserStaffID:"<<sSgwUserStaff.m_lStaffID<<endl;
		cout<<"m_szStaffName:"<<sSgwUserStaff.m_szStaffName<<endl;
		cout<<"m_szChannelID:"<<sSgwUserStaff.m_szChannelID<<endl;
	}

	if (!SHMUserStaff::GetUserStaffByStarr(sSgwUserStaff1.m_lStaffID,sSgwUserStaff)) {
		cout<<"修改后："<<endl;
		cout<<"m_iUserStaffID:"<<sSgwUserStaff.m_iUserStaffID<<endl;
		cout<<"m_iUserStaffID:"<<sSgwUserStaff.m_lStaffID<<endl;
		cout<<"m_szStaffName:"<<sSgwUserStaff.m_szStaffName<<endl;
		cout<<"m_szChannelID:"<<sSgwUserStaff.m_szChannelID<<endl;
	}
	vector<SgwUserStaff> vSgwUserStaff;
	if (!SHMUserStaff::GetUserStaffByChannel(sSgwUserStaff1.m_szChannelID,vSgwUserStaff)) {
		for (int  i =0; i<vSgwUserStaff.size();i++) {
			cout<<"修改后vSgwUserStaff："<<endl;
			cout<<"m_iUserStaffID:"<<vSgwUserStaff[i].m_iUserStaffID<<endl;
			cout<<"m_iUserStaffID:"<<vSgwUserStaff[i].m_lStaffID<<endl;
			cout<<"m_szStaffName:"<<vSgwUserStaff[i].m_szStaffName<<endl;
			cout<<"m_szChannelID:"<<vSgwUserStaff[i].m_szChannelID<<endl;
		}
	}
}

void testSgwGlobalMap(int value )
{
	SHMGlobalMap::globalmap_init();
	SgwGlobalMap sGlobalMap;
	memset(&sGlobalMap,0,sizeof(sGlobalMap));
	//cout<<SHMGlobalMap::AddGlobalMap(sGlobalMap)<<endl;

	if (!SHMGlobalMap::GetGlobalMap(value,sGlobalMap)) {
		cout<<sGlobalMap.m_iGID<<endl;
		cout<<sGlobalMap.m_szServiceContextID<<endl;
		cout<<sGlobalMap.m_szSourceAvp<<endl;
		cout<<sGlobalMap.m_szDesAvp<<endl;
		cout<<sGlobalMap.m_szSelective<<endl;
		cout<<sGlobalMap.m_szFormat<<endl;
		cout<<sGlobalMap.m_iOperate<<endl;
		cout<<sGlobalMap.m_iMsgType<<endl;
		cout<<sGlobalMap.m_iSourceType<<endl;
		cout<<sGlobalMap.m_iDesType<<endl<<endl;
	} else {
		cout<<"查询不到数据"<<endl;
	}
	SHMGlobalMap::DelGlobalMap(sGlobalMap);
	if (!SHMGlobalMap::GetGlobalMap(value,sGlobalMap)) {
		cout<<sGlobalMap.m_iGID<<endl;
		cout<<sGlobalMap.m_szServiceContextID<<endl;
		cout<<sGlobalMap.m_szSourceAvp<<endl;
		cout<<sGlobalMap.m_szDesAvp<<endl;
		cout<<sGlobalMap.m_szSelective<<endl;
		cout<<sGlobalMap.m_szFormat<<endl;
		cout<<sGlobalMap.m_iOperate<<endl;
		cout<<sGlobalMap.m_iMsgType<<endl;
		cout<<sGlobalMap.m_iSourceType<<endl;
		cout<<sGlobalMap.m_iDesType<<endl<<endl;
	} else {
		cout<<"查询不到数据"<<endl;
	}
	SHMGlobalMap::AddGlobalMap(sGlobalMap);
	if (!SHMGlobalMap::GetGlobalMap(value,sGlobalMap)) {
		cout<<sGlobalMap.m_iGID<<endl;
		cout<<sGlobalMap.m_szServiceContextID<<endl;
		cout<<sGlobalMap.m_szSourceAvp<<endl;
		cout<<sGlobalMap.m_szDesAvp<<endl;
		cout<<sGlobalMap.m_szSelective<<endl;
		cout<<sGlobalMap.m_szFormat<<endl;
		cout<<sGlobalMap.m_iOperate<<endl;
		cout<<sGlobalMap.m_iMsgType<<endl;
		cout<<sGlobalMap.m_iSourceType<<endl;
		cout<<sGlobalMap.m_iDesType<<endl<<endl;
	} else {
		cout<<"查询不到数据"<<endl;
	}
	sGlobalMap.m_iDesType = 1;
	sGlobalMap.m_iMsgType = 2;
	sGlobalMap.m_iOperate = 0;
	sGlobalMap.m_iSourceType = 3;
	SHMGlobalMap::UpdateGlobalMap(sGlobalMap);

	if (!SHMGlobalMap::GetGlobalMap(value,sGlobalMap)) {
		cout<<sGlobalMap.m_iGID<<endl;
		cout<<sGlobalMap.m_szServiceContextID<<endl;
		cout<<sGlobalMap.m_szSourceAvp<<endl;
		cout<<sGlobalMap.m_szDesAvp<<endl;
		cout<<sGlobalMap.m_szSelective<<endl;
		cout<<sGlobalMap.m_szFormat<<endl;
		cout<<sGlobalMap.m_iOperate<<endl;
		cout<<sGlobalMap.m_iMsgType<<endl;
		cout<<sGlobalMap.m_iSourceType<<endl;
		cout<<sGlobalMap.m_iDesType<<endl<<endl;
	} else {
		cout<<"查询不到数据"<<endl;
	}

}

void testSHMMsgMapInfo(int value )
{
	SHMMsgMapInfo::msgmap_init();
	SMsgMapInfo  vsMsgMapInfo;
	if (!SHMMsgMapInfo::GetMsgMapInfo(value,vsMsgMapInfo)) {
		cout<<vsMsgMapInfo.m_iID<<endl;
		cout<<vsMsgMapInfo.m_iDataType<<endl;
		cout<<vsMsgMapInfo.m_iDesDataType<<endl;
		cout<<vsMsgMapInfo.m_iMsgType<<endl;
		cout<<vsMsgMapInfo.m_iContextFilter<<endl;
		cout<<vsMsgMapInfo.m_iDataLimited<<endl;
		cout<<vsMsgMapInfo.m_iXmlCData<<endl;
		cout<<vsMsgMapInfo.m_bMajorAvp<<endl;
		cout<<vsMsgMapInfo.m_bGlobCfg<<endl;
		cout<<vsMsgMapInfo.m_szSelective<<endl;
		cout<<vsMsgMapInfo.m_szServiceContextID<<endl;
		cout<<vsMsgMapInfo.m_szDccNode<<endl;
		cout<<vsMsgMapInfo.m_szDataNode<<endl;
		cout<<vsMsgMapInfo.m_szDccCode<<endl;
		cout<<vsMsgMapInfo.m_szDccVendor<<endl;
		cout<<vsMsgMapInfo.m_szFormat<<endl<<endl;
	} else {
		cout<<"1.查询不到数据"<<endl;
	}
	SHMMsgMapInfo::DelMsgMapInfo(vsMsgMapInfo);
	if (!SHMMsgMapInfo::GetMsgMapInfo(value,vsMsgMapInfo)) {
		cout<<vsMsgMapInfo.m_iID<<endl;
		cout<<vsMsgMapInfo.m_iDataType<<endl;
		cout<<vsMsgMapInfo.m_iDesDataType<<endl;
		cout<<vsMsgMapInfo.m_iMsgType<<endl;
		cout<<vsMsgMapInfo.m_iContextFilter<<endl;
		cout<<vsMsgMapInfo.m_iDataLimited<<endl;
		cout<<vsMsgMapInfo.m_iXmlCData<<endl;
		cout<<vsMsgMapInfo.m_bMajorAvp<<endl;
		cout<<vsMsgMapInfo.m_bGlobCfg<<endl;
		cout<<vsMsgMapInfo.m_szSelective<<endl;
		cout<<vsMsgMapInfo.m_szServiceContextID<<endl;
		cout<<vsMsgMapInfo.m_szDccNode<<endl;
		cout<<vsMsgMapInfo.m_szDataNode<<endl;
		cout<<vsMsgMapInfo.m_szDccCode<<endl;
		cout<<vsMsgMapInfo.m_szDccVendor<<endl;
		cout<<vsMsgMapInfo.m_szFormat<<endl<<endl;
	} else {
		cout<<"2.查询不到数据"<<endl;
	}
	SHMMsgMapInfo::AddMsgMapInfo(vsMsgMapInfo);
	if (!SHMMsgMapInfo::GetMsgMapInfo(value,vsMsgMapInfo)) {
		cout<<vsMsgMapInfo.m_iID<<endl;
		cout<<vsMsgMapInfo.m_iDataType<<endl;
		cout<<vsMsgMapInfo.m_iDesDataType<<endl;
		cout<<vsMsgMapInfo.m_iMsgType<<endl;
		cout<<vsMsgMapInfo.m_iContextFilter<<endl;
		cout<<vsMsgMapInfo.m_iDataLimited<<endl;
		cout<<vsMsgMapInfo.m_iXmlCData<<endl;
		cout<<vsMsgMapInfo.m_bMajorAvp<<endl;
		cout<<vsMsgMapInfo.m_bGlobCfg<<endl;
		cout<<vsMsgMapInfo.m_szSelective<<endl;
		cout<<vsMsgMapInfo.m_szServiceContextID<<endl;
		cout<<vsMsgMapInfo.m_szDccNode<<endl;
		cout<<vsMsgMapInfo.m_szDataNode<<endl;
		cout<<vsMsgMapInfo.m_szDccCode<<endl;
		cout<<vsMsgMapInfo.m_szDccVendor<<endl;
		cout<<vsMsgMapInfo.m_szFormat<<endl<<endl;
	} else {
		cout<<"3.查询不到数据"<<endl;
	}
	vsMsgMapInfo.m_bIsAcctID = 0;
	vsMsgMapInfo.m_iContextFilter =1000;
	vsMsgMapInfo.m_iDataLimited = 10000000;
	SHMMsgMapInfo::UpdateMsgMapInfo(vsMsgMapInfo);
	if (!SHMMsgMapInfo::GetMsgMapInfo(value,vsMsgMapInfo)) {
		cout<<vsMsgMapInfo.m_iID<<endl;
		cout<<vsMsgMapInfo.m_iDataType<<endl;
		cout<<vsMsgMapInfo.m_iDesDataType<<endl;
		cout<<vsMsgMapInfo.m_iMsgType<<endl;
		cout<<vsMsgMapInfo.m_iContextFilter<<endl;
		cout<<vsMsgMapInfo.m_iDataLimited<<endl;
		cout<<vsMsgMapInfo.m_iXmlCData<<endl;
		cout<<vsMsgMapInfo.m_bMajorAvp<<endl;
		cout<<vsMsgMapInfo.m_bGlobCfg<<endl;
		cout<<vsMsgMapInfo.m_szSelective<<endl;
		cout<<vsMsgMapInfo.m_szServiceContextID<<endl;
		cout<<vsMsgMapInfo.m_szDccNode<<endl;
		cout<<vsMsgMapInfo.m_szDataNode<<endl;
		cout<<vsMsgMapInfo.m_szDccCode<<endl;
		cout<<vsMsgMapInfo.m_szDccVendor<<endl;
		cout<<vsMsgMapInfo.m_szFormat<<endl<<endl;
	} else {
		cout<<"4.查询不到数据"<<endl;
	}

}

void testSHMServicePackageFunc(int value )
{
	CServicePackageFunc  sServicePackageFunc;
	SHMServicePackageFunc::servicepackagefunc_init();
	memset(&sServicePackageFunc,0,sizeof(CServicePackageFunc));
	if (!SHMServicePackageFunc::GetServicePackageFunc(value,sServicePackageFunc)) {
		cout<<sServicePackageFunc.m_iCallID<<endl;
		cout<<sServicePackageFunc.m_iCallFuncID<<endl;
		cout<<sServicePackageFunc.m_iFuncParam1<<endl;
		cout<<sServicePackageFunc.m_iFuncParam1Type<<endl;
		cout<<sServicePackageFunc.m_iFuncParam2<<endl;
		cout<<sServicePackageFunc.m_iFuncParam2Type<<endl<<endl;
	} else {
		cout<<"1.查询不到数据"<<endl;
	}
	SHMServicePackageFunc::DelServicePackageFunc(sServicePackageFunc);

	if (!SHMServicePackageFunc::GetServicePackageFunc(value,sServicePackageFunc)) {
		cout<<sServicePackageFunc.m_iCallID<<endl;
		cout<<sServicePackageFunc.m_iCallFuncID<<endl;
		cout<<sServicePackageFunc.m_iFuncParam1<<endl;
		cout<<sServicePackageFunc.m_iFuncParam1Type<<endl;
		cout<<sServicePackageFunc.m_iFuncParam2<<endl;
		cout<<sServicePackageFunc.m_iFuncParam2Type<<endl<<endl;
	} else {
		cout<<"2.查询不到数据"<<endl;
	}

	SHMServicePackageFunc::AddServicePackageFunc(sServicePackageFunc);

	if (!SHMServicePackageFunc::GetServicePackageFunc(value,sServicePackageFunc)) {
		cout<<sServicePackageFunc.m_iCallID<<endl;
		cout<<sServicePackageFunc.m_iCallFuncID<<endl;
		cout<<sServicePackageFunc.m_iFuncParam1<<endl;
		cout<<sServicePackageFunc.m_iFuncParam1Type<<endl;
		cout<<sServicePackageFunc.m_iFuncParam2<<endl;
		cout<<sServicePackageFunc.m_iFuncParam2Type<<endl<<endl;
	} else {
		cout<<"3.查询不到数据"<<endl;
	}

	sServicePackageFunc.m_iFuncParam1 = 1000;
	sServicePackageFunc.m_iFuncParam1Type = 10000;
	sServicePackageFunc.m_iFuncParam2 = 1;
	sServicePackageFunc.m_iFuncParam2Type = 1112;
	SHMServicePackageFunc::UpdateServicePackageFunc(sServicePackageFunc);

	if (!SHMServicePackageFunc::GetServicePackageFunc(value,sServicePackageFunc)) {
		cout<<sServicePackageFunc.m_iCallID<<endl;
		cout<<sServicePackageFunc.m_iCallFuncID<<endl;
		cout<<sServicePackageFunc.m_iFuncParam1<<endl;
		cout<<sServicePackageFunc.m_iFuncParam1Type<<endl;
		cout<<sServicePackageFunc.m_iFuncParam2<<endl;
		cout<<sServicePackageFunc.m_iFuncParam2Type<<endl<<endl;
	} else {
		cout<<"4.查询不到数据"<<endl;
	}

}

void testSHMServicePackageVle(int value )
{
	SHMServicePackageVle::servicepackagevariable_init();
	CServicePackageVariable  sServicePackageVariable;
	memset(&sServicePackageVariable,0,sizeof(sServicePackageVariable));

	if (!SHMServicePackageVle::GetServicePackageVariable(value,sServicePackageVariable)) {
		cout<<sServicePackageVariable.m_iVariableID<<endl;
		cout<<sServicePackageVariable.m_iVariableType<<endl;
		cout<<sServicePackageVariable.m_iGetValueMode<<endl;
		cout<<sServicePackageVariable.m_iSourceAVPType<<endl;
		cout<<sServicePackageVariable.m_szVariableName<<endl;
		cout<<sServicePackageVariable.m_szSourceAVP<<endl;
		cout<<sServicePackageVariable.m_szSourceData<<endl;
	} else {
		cout<<"1.查询不到数据"<<endl<<endl;
	}

	SHMServicePackageVle::DelServicePackageVariable(sServicePackageVariable);
	if (!SHMServicePackageVle::GetServicePackageVariable(value,sServicePackageVariable)) {
		cout<<sServicePackageVariable.m_iVariableID<<endl;
		cout<<sServicePackageVariable.m_iVariableType<<endl;
		cout<<sServicePackageVariable.m_iGetValueMode<<endl;
		cout<<sServicePackageVariable.m_iSourceAVPType<<endl;
		cout<<sServicePackageVariable.m_szVariableName<<endl;
		cout<<sServicePackageVariable.m_szSourceAVP<<endl;
		cout<<sServicePackageVariable.m_szSourceData<<endl;
	} else {
		cout<<"2.查询不到数据"<<endl<<endl;
	}

//	SHMServicePackageVle::AddServicePackageVariable(sServicePackageVariable);
//	if (!SHMServicePackageVle::GetServicePackageVariable(value,sServicePackageVariable)) {
//		cout<<sServicePackageVariable.m_iVariableID<<endl;
//		cout<<sServicePackageVariable.m_iVariableType<<endl;
//		cout<<sServicePackageVariable.m_iGetValueMode<<endl;
//		cout<<sServicePackageVariable.m_iSourceAVPType<<endl;
//		cout<<sServicePackageVariable.m_szVariableName<<endl;
//		cout<<sServicePackageVariable.m_szSourceAVP<<endl;
//		cout<<sServicePackageVariable.m_szSourceData<<endl;
//	} else {
//		cout<<"3.查询不到数据"<<endl<<endl;
//	}
//
//	sServicePackageVariable.m_iVariableType=11111;
//	sServicePackageVariable.m_iGetValueMode=11111;
//	sServicePackageVariable.m_iSourceAVPType=11111;
//
//	SHMServicePackageVle::UpdateServicePackageVariable(sServicePackageVariable);
//	if (!SHMServicePackageVle::GetServicePackageVariable(value,sServicePackageVariable)) {
//		cout<<sServicePackageVariable.m_iVariableID<<endl;
//		cout<<sServicePackageVariable.m_iVariableType<<endl;
//		cout<<sServicePackageVariable.m_iGetValueMode<<endl;
//		cout<<sServicePackageVariable.m_iSourceAVPType<<endl;
//		cout<<sServicePackageVariable.m_szVariableName<<endl;
//		cout<<sServicePackageVariable.m_szSourceAVP<<endl;
//		cout<<sServicePackageVariable.m_szSourceData<<endl;
//	} else {
//		cout<<"4.查询不到数据"<<endl<<endl;
//	}
}

void testSHMSgwEnumarea(int value )
{
	SHMEnumArea::sgwenumarea_init();
	SgwEnumArea sSgwEnumArea;
	memset(&sSgwEnumArea,0,sizeof(sSgwEnumArea));
//	if (!SHMEnumArea::GetSgwEnumArea(value,sSgwEnumArea)) {
//		cout<<"m_iAreaSeq:"<<sSgwEnumArea.m_iAreaSeq<<endl;
//		cout<<"m_iOrgID:"<<sSgwEnumArea.m_iOrgID<<endl;
//		cout<<"m_szAreaCode:"<<sSgwEnumArea.m_szAreaCode<<endl;
//		cout<<"m_szAreaName:"<<sSgwEnumArea.m_szAreaName<<endl;
//	} else {
//		cout<<"查询不到数据"<<endl;
//	}

	if (!SHMEnumArea::GetSgwEnumAreaByOrgId(value,sSgwEnumArea)) {
		cout<<"m_iAreaSeq:"<<sSgwEnumArea.m_iAreaSeq<<endl;
		cout<<"m_iOrgID:"<<sSgwEnumArea.m_iOrgID<<endl;
		cout<<"m_szAreaCode:"<<sSgwEnumArea.m_szAreaCode<<endl;
		cout<<"m_szAreaName:"<<sSgwEnumArea.m_szAreaName<<endl;
	} else {
		cout<<"查询不到数据"<<endl;
	}
//	strcpy(sSgwEnumArea.m_szAreaCode,"test");
//	strcpy(sSgwEnumArea.m_szAreaName,"test");
//	SHMEnumArea::UpdateSgwEnumArea(sSgwEnumArea);
//
//	if (!SHMEnumArea::GetSgwEnumAreaByOrgId(value,sSgwEnumArea)) {
//		cout<<"m_iAreaSeq:"<<sSgwEnumArea.m_iAreaSeq<<endl;
//		cout<<"m_iOrgID:"<<sSgwEnumArea.m_iOrgID<<endl;
//		cout<<"m_szAreaCode:"<<sSgwEnumArea.m_szAreaCode<<endl;
//		cout<<"m_szAreaName:"<<sSgwEnumArea.m_szAreaName<<endl;
//	} else {
//		cout<<"查询不到数据"<<endl;
//	}
	SHMEnumArea::DeleteSgwEnumArea(sSgwEnumArea);
	if (!SHMEnumArea::GetSgwEnumAreaByOrgId(value,sSgwEnumArea)) {
		cout<<"m_iAreaSeq:"<<sSgwEnumArea.m_iAreaSeq<<endl;
		cout<<"m_iOrgID:"<<sSgwEnumArea.m_iOrgID<<endl;
		cout<<"m_szAreaCode:"<<sSgwEnumArea.m_szAreaCode<<endl;
		cout<<"m_szAreaName:"<<sSgwEnumArea.m_szAreaName<<endl;
	} else {
		cout<<"删除后查询不到数据！"<<endl;
	}
	SHMEnumArea::AddSgwEnumArea(sSgwEnumArea);
	if (!SHMEnumArea::GetSgwEnumAreaByOrgId(value,sSgwEnumArea)) {
		cout<<"m_iAreaSeq:"<<sSgwEnumArea.m_iAreaSeq<<endl;
		cout<<"m_iOrgID:"<<sSgwEnumArea.m_iOrgID<<endl;
		cout<<"m_szAreaCode:"<<sSgwEnumArea.m_szAreaCode<<endl;
		cout<<"m_szAreaName:"<<sSgwEnumArea.m_szAreaName<<endl;
	} else {
		cout<<"查询不到数据！"<<endl;
	}
}

int main(int argc ,char *argv[])
{
//	testSHMServicePackageVle(atoi(argv[1]));
	//testSHMNetInfoData(atoi(argv[1]));
	//testservicetxtlist(atoi(argv[1]));
	//testSHMServiceTxtListBase();
//	testSHMUserStaff1(atoi(argv[1]));
	testSHMParamInsList(argv[1]);
//	testSHMSrvsRegister(argv[1],argv[2],argv[3]);
//	testSHMSrvsParamList(argv[1]);
//	testSHMSrvsParamRela(argv[1]);
//	testSHMSvrList(argv[1]);
	//testSHMTuxRelaOut(argv[1]);
	//testSHMTuxRelaIn(argv[1]);
//	testSHMAreaOrg();
//	testSHMOrgRouteRule(argv[1]);
	//testnetchannel(argv[1]);
	//teststaff(argv[1]);
	//cout<<SHMSGWInfoBase::attachALL()<<endl;
//	testchannel(argv[1]);
	//testContentFiltering();
	//testservicetxtlist();
	//testSHMStreamCtrlInfo();
	//testSHMStreamCtrlData();
	//testSHMSgwSlaqueRel();
	//testSHMNetInfoData();
	//testquerysession(argv[1]);
	//testsession();
	//testSHMServiceTxtListBase();
	//testSHMBaseMethod();
	//testspacksession(argv[1]);
	//testuserinfo();
	//testSHMCongestLevel();
	//testSHMServicePackageRouteCCR();
	//testSHMServicePackageJudge();
	//testSHMServicePackageCCA();
	//testSHMSCongestMsg();
	//testSHMWfProcessMq();
	//testSHMLoadBalance();
//	testSHMSgwEnumarea(atoi(argv[1]));
//	testchangekey1();
//	testchangekey2();
	/*char sbuf[128] = {0};
	int i = 0;
	SHMSGWInfoBase::session_init();
	struct timeval tstart;

	while (true) {
		gettimeofday(&tstart,NULL);
		memset(sbuf,0,128);
		sprintf(sbuf,"CRMDCC@20.cntbn-bon.net;%s%ld%ld;%d",argv[1],tstart.tv_usec,rand()%1000,rand()%10000);
		testaddsession(sbuf);
		testquerysession(sbuf);
		testdelsession(sbuf);
		i++;
		if (i==3000) 	{
			i =0;
			sleep(1);
		}else {
			usleep(5);
		}
	}*/
	//testSessionClear(argv[1]);
	//testOrgRoute(atoi(argv[1]),argv[2],argv[3],argv[4],argv[5]);
	//testOrgHead(atoi(argv[1]),argv[2],argv[3],argv[4],argv[5],argv[6]);
	//SHMSGWInfoBase::attachALL();
}
