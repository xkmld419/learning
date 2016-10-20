#ifndef __POWERFILE_H__
#define __POWERFILE_H__

#include "Process.h"
#include "Directory.h"
#include "Log.h"
#include "common.h"
#include "ParamDefineMgr.h"
#include "SeqMgrExt.h"


//#include "ParserXml.h"
#include "PowerQuery.h"
//#include "PowerXml.h"
#include "power_comm.h"

#include "soapH.h" // 包含生成的存根例程定义
//#include "AsigServiceSoapBinding.nsmap" // 包含命名空间表
#include "soapStub.h"

#define SPITER "|"

class PowerFile
{
	public:
		int PowerMainProcess(int m_iOperation);
		int CheckFile( char* m_sFileName );
		int MainProcess();
void setUserInfo();	   
		int ReadFile(vector<string> &v_Phone);
		int WriteFile(char * m_sFileName,char *m_sString);
		void sendFile();
		void getFile();			
		void init();
		PowerFile();
		~PowerFile();
	private:
		File*       file;
		char m_lFileDirRecv[128];
		char m_lFileDirRecvBak[128];
		char m_lFileDirResp[128];
		char m_lFileDirRespBak[128];

		char m_rRecvPath[128];
		char m_rRecvBak[128];
		char m_rRespPath[128];

		int iCheckState;
		void listFile(char *sTempFile);
		vector<string> m_listFile;	   
		Directory oDir;
		Directory rDir;		
		char m_sFileName[256];
		PowerQuery *m_pQuery;
		char m_sIP[32];
		char m_sUser[32];
		char m_sPass[32];
		RespMsg pRespMsg;
};


#endif
