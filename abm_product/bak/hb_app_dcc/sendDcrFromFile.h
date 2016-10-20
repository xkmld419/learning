/*VER:1*/
/*目的:从tdtrans形成的文件中读取记录形成dcc消息的内容部分发送给编码模块*/

#ifndef __SEND_DCR_FROM_FILE_H__
#define __SEND_DCR_FROM_FILE_H__

#include "Process.h"
#include "Environment.h"
#include "dcc_FileMgr.h"

class FileToDccMain:public Process
{
	public:
		FileToDccMain()	;	
		~FileToDccMain();
		int run();

		static char m_sFileTypes[80];
		static int m_iWaitCCA;
		static char m_sFileBakDir[128];
	private:
		int printManual();
		bool prepare();						
		FileToDccMgr *m_pDccMgr;
};

#endif

