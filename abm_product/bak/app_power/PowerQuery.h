#ifndef __POWERQUERY__
#define __POWERQUERY__

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "TOCIQuery.h"
#include "Environment.h"
#include "common.h"
#include "Head2OrgIDMgr.h"
#include "ParamDefineMgr.h"
#include "ParserXml.h"
//#include "PowerXml.h"
#include "power_comm.h"
#include "FTPClient.h"
#include "soapH.h" // 包含生成的存根例程定义
//#include "AsigServiceSoapBinding.nsmap" // 包含命名空间表
#include "soapStub.h"
using namespace std;

class PowerQuery
{
	public:
	  int  queryState(char * m_sPhone,char *sBusiName,char *sPackName,char *mState,RespMsg &pResp);
	  int  queryGprs(char *m_sPhone,vector<GprsAmount> &v_Vec,RespMsg &pResp);
	  int  queryLastGprs(char * m_sPhone,long &SumAmount,RespMsg &pResp);
		int getHLRinfo(char *sFormnum,HlrFormnum &pHlr,RespMsg &pResp);

	  int  checkPhone(char * m_sPhone);
	  long getServID(char * m_sPhone);
	  int  getIMSIByServID(char * m_sPhone,char *imsi);
		int setHLRinfo();
		void getCdmaStatus(HLRMSG &pHlr,char *sStatus)	  ;
		int getVpdnIPByImsi(char *sImsi,char *sIP,char *sOnlineIP);
		int getHLRStatus(char *sAccNbr,char *sStatus);
	  int  getLocal(char * m_sPhone,char *imsi);
	  bool getTableName(char * m_sPhone);
	  void init();
		void getFormNum(char *sFormNum);
		void getRespSeq(char *sRespSeq);
		int getAreaID(char * m_sPhone);
		int insertFormNumLog( char *sAccNbr,char *sFormNum,char *sStatus,char *sAPN,char *sIP);
		int insertFileList( char *sFileName,int iRecNum,int iFileType,char *sRespFullFile);
		int insertOpertion(OperationIn &pOper,RespMsg &pResp);
		bool checkFileExist( char *sFileName);
	  PowerQuery();
	 ~PowerQuery();
    char m_sHlrUrl[128];
    char m_sNocUrl[128];

	private:
	  //HashList<string> *m_pTableName;
	  InstTableListMgr *m_pTableName;
	  int m_iBillingCycleID;
	  int m_iOrgID;
		
	  char m_sSql[1024];
	  char m_sTableName[30];
	  char m_sOrgType[4];

	  Head2OrgIDMgr *m_pLocalHeadMgr;
	  HeadEx *m_pLocalHead;
	  OrgMgr *m_OrgMgr;
	  
	  TOCIDatabase tSsyt;
	  TOCIDatabase tSzx;

    char m_sDBUser[24];
    char m_sDBPwd[24];
    char m_sDBStr[32];

    char m_sDBUser1[24];
    char m_sDBPwd1[24];
    char m_sDBStr1[32];
    char m_sOfferID[128];
    char m_sProductID[256];
    
	  char sHome[254];
	  
	ParserXml pParserXml;		  
};


#endif 


