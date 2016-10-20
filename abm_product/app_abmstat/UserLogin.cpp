#include "UserLogin.h"
#include "Date.h"

UserLogin::UserLogin()
{
	
}

UserLogin::~UserLogin()
{
	
}


void UserLogin::Rebuild()
{
	//此变量用于自上而下绘制菜单
	int lv_iTotalRow = 0;
	int lv_iCurrentRow = 0;
	static char Buffer[COLS_LENS];

	LoadEmptyMenu();
	DisplayTime();

	int iTaskNum = 0;
	int i = 0;
	
	//char sUserLogin[10240]={0};

	char sBuff[1000][128];

	memset(Buffer,0,COLS_LENS);
	sprintf(Buffer,"用户今日登录信息    登录用户：登录方式：IP：日期：开始时间：结束时间：持续时间");
	mvaddstr(4,0,Buffer);
	Date dToday;

	//调用系统命令
	//---------------------------------------------------------
	char *sDay[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	char *sMonth[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	char sCommand[256] = {0};
	char sParam[16] = {0};
	FILE *fp = 0;
	int ret = 0;
	//sprintf (sParam,"%s %s %2d",sDay[dToday.getWeek()],sMonth[dToday.getMonth() - 1],dToday.getDay());
	sprintf (sParam,"%s %02d",sMonth[dToday.getMonth() - 1],dToday.getDay());
	sprintf (sCommand, "last |grep '%s' ",sParam);
	fp = popen (sCommand, "r");
	if (fp == NULL)
	{
		mvaddstr(8,(COLS_LENS - strlen((char *)"调用系统命令失败"))/2,(char *)"调用系统命令失败");
		return ;
	}
	//---------------------------------------------------------

  //sprintf(sBuff[0],"%s","登录用户：登录方式：IP：日期：开始时间：结束时间：持续时间"); 
	int n = 0;
	
	while(!feof(fp))
	{
		if( !fgets(sBuff[n],128,fp) )
			break;
		n++;
		lv_iTotalRow ++;
	}
	fclose(fp);

	

	if(lv_iTotalRow==0){
		mvaddstr(8,(COLS_LENS - strlen((char *)"无符合条件的选项"))/2,(char *)"无符合条件的选项");
		return;
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

	for(; i<n ; i++)
	{
		if ((lv_iCurrentRow > lv_iTotalRow)||(lv_iCurrentRow >= m_iStartRow + LINE_SIZE))
		{
			break;
		}

		if (lv_iCurrentRow == m_iSelectedRow)
		{
			standout();
		}

		//输出序号  			
		memset(Buffer,0,COLS_LENS);
		sprintf(Buffer,"%-10d",lv_iCurrentRow);
		mvaddstr(lv_iCurrentRow - m_iStartRow + 6,0,Buffer);

		memset(Buffer,0,COLS_LENS);
		sprintf(Buffer,"%s",sBuff[i]);
		mvaddstr(lv_iCurrentRow - m_iStartRow + 6,10,Buffer);

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

