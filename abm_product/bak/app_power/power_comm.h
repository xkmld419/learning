#ifndef __POWER_STRUCT_H__
#define __POWER_STRUCT_H__

#define DATA_SOURCE_POWER 1 //电力的XML解析 
#define DATA_SOURCE_HLR   2 //HLR 的XML解析 

//struct OperationIn{
struct OperationIn{
   char m_sProcessCode[21];
   char m_sVerifyCode[33];
   char m_sReqType[5];
   char m_sOperId[31];
   char m_sOrgId[13];
   char m_sCityId[21];
   char m_sAcceptSeq[31];
   char m_sReqSeq[31];
   char m_sReqChannel[31];
   char m_sReqSource[6];
   char m_sReqTime[16];
   char m_sMsisdn[32];
   char m_sEndDate[20];
	 char m_sFormnum[32];
	 //char m_sRespTime[16];
}  ;

//struct HlrFormnum {
struct HlrFormnum {
	char m_sMsisdn[30];
	char m_sGprsStatus[30];
	char m_sApn[30];
	char m_sIp[30];
};

struct RespMsg {
	int  m_iRespType;
	char m_sRespCode[30];
	char m_sRespDesc[128];
	char m_sRespSeq[31];
	char m_sRespTime[20];
	char m_sReqSeq[31];
};


//struct GprsAmount{
struct GprsAmount{
	char sStartDate[16];
	long SumAmount;
	long RecvAmount;
	long SendAmount;
} ;

struct HLRMSG{
	int  returnCode;
	char vendor[20];
	char Status[20];
	char errorMessage[512];
} ;

#endif //__POWER_STRUCT_H__