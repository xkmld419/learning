#ifndef __POWERXML_H__
#define __POWERXML_H__

#include "Process.h"
#include "Directory.h"
#include "Log.h"
#include "common.h"
#include "ParamDefineMgr.h"

#include "SeqMgrExt.h"


#include "PowerQuery.h"

#include "ParserXml.h"

#include "power_comm.h"

using namespace std;



class PowerXml
{
	public:
			int  PowerXmlProcess(char* xmlbuf,char *retbuf);
			void SaveXMLToFile(const char *xmlbuf,char * req_seq);
			void SaveXML(const char *xmlbuf,char * req_seq);
		//	long  getHLRStatus(char *sAccNbr,char *sStatus);
		//	long getVpdnIPByImsi(char *sImsi,char *sIP,char *sOnlineIP);
			PowerXml();
			~PowerXml();
			void  init();
			
			RespMsg m_Resp;

	private:
			int iCheckState;
			SeqMgrExt *m_pXmlLogID;

			char m_sFilePath[256];

			PowerQuery m_qry;
			ParserXml pParserXml;

};

#endif //__POWERXML_H__
