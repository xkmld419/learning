/*ver:1*/

#include "sendDcrFromFile.h"
#include "ParamDefineMgr.h"

char FileToDccMain::m_sFileTypes[80]={0};
char FileToDccMain::m_sFileBakDir[128]={0};
int FileToDccMain::m_iWaitCCA = 0;

FileToDccMain::FileToDccMain()
{
	m_pDccMgr = FileToDccMgr::getInstance();
}

FileToDccMain::~FileToDccMain()
{
	if (m_pDccMgr)
		delete m_pDccMgr;
}

void FileToDccMain::printManual()
{
	Log::log(0,"sendCCRFromFile 程序输入参数:\n"
			"	-f  处理的trans_file_type_id组合，用','分隔\n"
			"	-w 发送ccr包后是否等候cca包的返回,默认为0，不等待");

	return;
}

bool FileToDccMain::prepare()
{
	int opt=0;

	while((opt=getopt(g_argc,g_argv,"F:f:W:w:?"))!=EOF)
	{
		switch(opt)
		{
			case 'F':
			case 'f':
				strcpy(m_sFileTypes,optarg);
				m_sFileTypes[79]=0;
				break;
			case 'W':
			case 'w':
				m_iWaitCCA = atoi(optarg);
				break;
			default:
				printManual();
				return false;
				break;
		}
	}

	return true;
}


int FileToDccMain::run()
{
	if (!prepare())
		return 0;

	if (!ParamDefineMgr::getParam("DCC", "FILE_BAK_DIR", m_sFileBakDir))
	{
		Log::log(0,"请在b_param_define表中配置文件备份目录参数!");
		return 0;
	}

	m_pDccMgr->setQueueTool(m_poSender, m_poReceiver);
	while(1)
	{
		try
		{
			m_pDccMgr->setReReadStateFlag();
			m_pDccMgr->formatDcrInfo();
		}
		catch (TOCIException &e)
		{
			Log::log(0,"发生sql错误:sqlcode[%d]\nmessage[%s]\nsql[%s]",e.getErrCode(),e.getErrMsg(),e.getErrSrc());
		}
		catch (Exception &e)
		{
			Log::log(0,"发生错误，错误信息[%s]",e.descript());
		}
		catch (...)
		{
			Log::log(0,"发生未知错误!");
		}

		return 0;
	}

}
