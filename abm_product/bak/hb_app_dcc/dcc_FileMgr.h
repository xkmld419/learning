/*ver:1*/

#ifndef __DCC_FILE_MGR_H__
#define __DCC_FILE_MGR_H__

#include "Environment.h"
#include "Log.h"
#include "Directory.h"
#include "dcc_FileInfo.h"
#include "EventSender.h"
#include "EventReceiver.h"

class FileToDccMgr
{
	public:
		FileToDccMgr();
		~FileToDccMgr();
		int formatDcrInfo();
		static FileToDccMgr* getInstance();
		int init(char *sFileTypes = NULL);
		string getServiceContextByFileType(int iFileType);
		string getFilePath(int iFileType);
		int getFileTypeByServiceContext(char *sServiceContext);
		void setQueueTool(EventSender *poSender,EventReceiver *poReceiver);
	private:		
		void freeAllFileType();		
		EventReceiver *m_oSelfReceiver;
		EventSender *m_oSelfSender;
		map<int,DccFileInfo *>m_mapFileTypeInfo;
		map<int,string>m_mapFileStore;
		map<string,int>m_mapServiceContext;
		static FileToDccMgr *m_gpDccNgr;
		string m_sSendString;
		char *m_sSendBuffer;
		char *m_sReceiveBuffer;
};

#endif
