#include "LoggInfo.h"
#include "ThreeLogGroup.h"

LoggInfo::LoggInfo()
{
//	ConnectGuardMain();
}

LoggInfo::~LoggInfo()
{
}


void LoggInfo::Rebuild()
{
	//此变量用于自上而下绘制菜单
	int lv_iTotalRow = 0;
	int lv_iCurrentRow = 0;
	static char Buffer[COLS_LENS];

	LoadEmptyMenu();
	DisplayTime();

	int iTaskNum = 0;
	int i = 0;
	
	char sLoggInfo[4096]={0};

	ThreeLogGroup* pLogg = NULL;
	try
	{
		pLogg = new ThreeLogGroup();
	}
	catch (...)
	{
		pLogg = NULL;
	}
	
	memset(Buffer,0,COLS_LENS);
	sprintf(Buffer,"日志组信息");
	mvaddstr(4,0,Buffer);

	if( pLogg == 0 )
	{
		mvaddstr(8,(COLS_LENS - strlen((char *)"连接日志组内存失败"))/2,(char *)"连接日志组内存失败");
		return;
	}

	if( !pLogg->DisplayLogGroup(-1,sLoggInfo))
	{
		mvaddstr(8,(COLS_LENS - strlen((char *)"获取日志组信息失败"))/2,(char *)"获取日志组信息失败");
		return;
	}

	if(strlen(sLoggInfo)==0){
		mvaddstr(8,(COLS_LENS - strlen((char *)"无符合条件的选项"))/2,(char *)"无符合条件的选项");
		return;
	}

	char *p = sLoggInfo;

	while(*p)
	{
		if(*p == '\n')
			lv_iTotalRow ++;
		p++;
	}


	lv_iCurrentRow =1;

	//以下四个判断主要用于纠正一些非法的
	//如果全局选中行数<=0，则回指到 行
	if (m_iSelectedRow  <= 0)
	{
		m_iSelectedRow  = lv_iTotalRow ;
	}		
	//如果全局选中行数> lv_iTotalRow ，则回指到第一行
	if (m_iSelectedRow  > lv_iTotalRow)
	{
		m_iSelectedRow  = 1;
	}		
	//如果g_iStartRow <= 0，人为置为第一行	
	if (m_iStartRow <= 0)
	{
		m_iStartRow = 1;
	}				
	if (m_iStartRow > m_iSelectedRow )
	{
		m_iStartRow = m_iSelectedRow ;
	}				
	if (m_iSelectedRow  - m_iStartRow >= LINE_SIZE)
	{
		m_iStartRow = m_iSelectedRow  - LINE_SIZE + 1;
	}				

	// --确定lv_iCurrentRow初始值
	while(1)
	{
		if (lv_iCurrentRow >= m_iStartRow )
		{
			break;
		}
		else
		{
			lv_iCurrentRow++;
		}				
	}
	i = lv_iCurrentRow -1;

	char *sTok = "\n";
	p = strtok(sLoggInfo,sTok);
	while(p)
	{
		if ((lv_iCurrentRow > lv_iTotalRow)||(lv_iCurrentRow >= m_iStartRow + LINE_SIZE))
		{
			break;
		}
		if (lv_iCurrentRow == m_iSelectedRow)
		{
			standout();
		}

		memset(Buffer,0,COLS_LENS);
		sprintf(Buffer,"%s",p);
		mvaddstr(lv_iCurrentRow - m_iStartRow + 6,0,Buffer);
		p = strtok(NULL,sTok);

		if (lv_iCurrentRow == m_iSelectedRow)
		{
			standend();
		}
		lv_iCurrentRow ++;
	}


	//未满的行填入空白		
	while (lv_iCurrentRow < m_iStartRow + LINE_SIZE)
	{
		mvhline(lv_iCurrentRow - m_iStartRow + 6,0,' ',COLS_LENS);
		lv_iCurrentRow ++;
	}

	wmove(stdscr,m_iSelectedRow - m_iStartRow + 6,0);//将指针移到选定行首
	refresh();
}

