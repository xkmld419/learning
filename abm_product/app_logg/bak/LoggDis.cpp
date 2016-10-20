#include "LoggDis.h"
#include "ThreeLogGroupDefine.h"

vector<string> DisplayLogg::m_vAppInfo;
vector<string> DisplayLogg::m_vProInfo;
vector<string> DisplayLogg::m_vAlarmInfo;
vector<string> DisplayLogg::m_vParamInfo;
vector<string> DisplayLogg::m_vSystemInfo;
vector<string> DisplayLogg::m_vSysOperInfo;
	
    //Levle 
vector<string> DisplayLogg::m_vLevFatal;
vector<string> DisplayLogg::m_vLevError;
vector<string> DisplayLogg::m_vLevWarn;
vector<string> DisplayLogg::m_vLevInfo;
vector<string> DisplayLogg::m_vLevDebug;     
    
    //Class
vector<string> DisplayLogg::m_vClaError;
vector<string> DisplayLogg::m_vClaWarn;      
vector<string> DisplayLogg::m_vClaInfo; 
    
    //Type 
vector<string> DisplayLogg::m_vTypeBusi;
vector<string> DisplayLogg::m_vTypeParam;
vector<string> DisplayLogg::m_vTypeSystem ;
vector<string> DisplayLogg::m_vTypeStart;
vector<string> DisplayLogg::m_vTypeOracle;   

//Code
vector<string> DisplayLogg::m_vCodeInfo;


void DisplayLogg::DisInit()
{
   //触发报警的时间间隔(单位：秒)
    int INTERVAL = 1;
    //菜单单页显示的行数
    int iLoggPageSize = 30;
    //全局变量：纪录在菜单之间传递的参数
    int Logg_ProblemProcSelectedRow = 1;
    int Logg_ProblemProcStartRow = 1;
    int NOW_USED=0;

    int LogFlagAll=0; 
}

void DisplayLogg::Loggstartwin()
{
    //忽略中止信号
    signal(SIGINT,SIG_IGN);
    initscr();
    noecho();
    intrflush(stdscr,true); 
    savetty();
    keypad(stdscr,TRUE);
}
void DisplayLogg::LoggLoadGeneralInfo()
{
    //----------------------------加载菜单的通用信息--------------------------------
    //加载总标题
    attron(A_BOLD); 
    mvaddstr(0,(COLS_LENS - strlen((char *)"日志查询"))/2,(char *)"日志查询");   	   	
    attroff(A_BOLD);
    
    /*加载菜单界限*/
    mvhline(3,0,'=',COLS_LENS);
    mvhline(5 + iLoggPageSize + 1,0,'=',COLS_LENS);
}
void DisplayLogg::LoggLoadEmptyMenu()
{
    mvhline(2,0,' ',112);//清标题
    mvhline(4,0,' ',COLS_LENS);//清标题
    for(int i=6;i<iLoggPageSize+6;i++)
    {
        mvhline(i,0,' ',COLS_LENS);	
    }
    mvhline(5 + iLoggPageSize + 3,0,' ',COLS_LENS);
    mvaddstr(5 + iLoggPageSize + 3,13,(char *)"Up Arrow = 向上   Down Arrow = 向下   ESC = 退出");

    return;
}

void DisplayLogg::LoggloadAll(vector<string> &m_vInfo)
{
    m_vConverInfo.clear();
    NOW_USED = m_vInfo.size();

    if(NOW_USED==0)
    {
        m_vConverInfo.push_back("**************没有查询到符合的信息**************");
        NOW_USED = m_vConverInfo.size();
        return ;
    }
    vector<string>::iterator iter;

    for(iter=m_vInfo.begin();iter!=m_vInfo.end();++iter)
    {
        int size = (*iter).size();
        if(size>COLS_LENS)
        {
        	//oh,my  god 
            int index=0;
            char temp_1[512],temp_2[512];
			int iTrunc = 0,iNoAsciiCnt = 0;

			while(size>COLS_LENS)
            {
               iTrunc = 0;
               memset(temp_1,0,sizeof(temp_1));
               (*iter).copy(temp_1,COLS_LENS,index);

			   iNoAsciiCnt = 0;
			   for(int k = COLS_LENS -1; k != -1; --k)
			   {
			   		if(!isascii(temp_1[k])) 
			   		{
			   			++iNoAsciiCnt;
			   		} else {
						break;
					}
			   }

			   if(iNoAsciiCnt % 2)
			   	{
			   		temp_1[COLS_LENS-1] = '\0';
					iTrunc = 1;
			   	}
               m_vConverInfo.push_back(temp_1);
               //size -= COLS_LENS;
               size = size - COLS_LENS + iTrunc;
               //index += COLS_LENS;
               index = index + COLS_LENS -iTrunc;
            }
            memset(temp_2,0,sizeof(temp_2));
            (*iter).copy(temp_2,COLS_LENS,index);
            m_vConverInfo.push_back(temp_2);
        }
        else
            m_vConverInfo.push_back(*iter);
    }

    NOW_USED = m_vConverInfo.size();
}

void DisplayLogg::LoggRebuildProblemProc()
{
    //此变量用于自上而下绘制菜单
    int lv_iTotalRow = NOW_USED;
    int lv_iCurrentRow = 0;
    static char Buffer[COLS_LENS+1];
    
    LoggLoadEmptyMenu();
    //以下四个判断主要用于纠正一些非法的
    //如果全局选中行数<=0，则回指到gv_iMag_ProcSelectedRowinTotalRow 行
    if (Logg_ProblemProcSelectedRow  <= 0)
    {
       Logg_ProblemProcSelectedRow  = lv_iTotalRow ;
    }
    //如果全局选中行数> lv_iTotalRow ，则回指到第一行
    if (Logg_ProblemProcSelectedRow  > lv_iTotalRow)
    {
       Logg_ProblemProcSelectedRow  = 1;
    }
    //如果g_iStartRow <= 0，人为置为第一行	
    if (Logg_ProblemProcStartRow <= 0)
    {
       Logg_ProblemProcStartRow = 1;
    }
    if (Logg_ProblemProcStartRow > Logg_ProblemProcSelectedRow )
    {
       Logg_ProblemProcStartRow = Logg_ProblemProcSelectedRow ;
    }
    if (Logg_ProblemProcSelectedRow  - Logg_ProblemProcStartRow >= iLoggPageSize)
    {
       Logg_ProblemProcStartRow = Logg_ProblemProcSelectedRow  - iLoggPageSize + 1;
    }
    // --确定lv_iCurrentRow初始值
    while(1)
    {
        if (lv_iCurrentRow >= Logg_ProblemProcStartRow )
        {
            break;
        }
        else
        {
            lv_iCurrentRow++;
        }
    }	
    for(int i=0; i<iLoggPageSize; i++)
    {
        if ((lv_iCurrentRow > lv_iTotalRow)||(lv_iCurrentRow >= Logg_ProblemProcStartRow + iLoggPageSize))
        {
            break;
        }
        if (lv_iCurrentRow == Logg_ProblemProcSelectedRow)
        {
            standout();
        }
        mvhline(lv_iCurrentRow - Logg_ProblemProcStartRow + 5,0,' ',COLS_LENS);
        memset(Buffer,0,COLS_LENS);
        if(Logg_ProblemProcStartRow+i<=NOW_USED)
            sprintf(Buffer,"%s",(m_vConverInfo[Logg_ProblemProcStartRow+i-1]).c_str());
		//modify by liyr
		//mvaddstr(4,0,"时间  编码  编码名称  PRO_ID  APP_ID  级别  类别  分类  工号  操作类型  操作字段  本地网标识  内容");
		mvaddstr(4,0,"时 间    级 别   类 别   编 码   编码名称   进程ID   线程ID   模块ID   对端网元   流 向  工 号   操作类型   操作字段   日志内容");
		mvaddstr(lv_iCurrentRow - Logg_ProblemProcStartRow + 5,0,Buffer);
        if (lv_iCurrentRow == Logg_ProblemProcSelectedRow)
        {
            standend();
        }
        lv_iCurrentRow++;
    }
    //未满的行填入空白		
    while (lv_iCurrentRow < Logg_ProblemProcStartRow + iLoggPageSize)
    {
        mvhline(lv_iCurrentRow - Logg_ProblemProcStartRow + 5,0,' ',COLS_LENS);
        lv_iCurrentRow ++;
    }
    wmove(stdscr,Logg_ProblemProcSelectedRow - Logg_ProblemProcStartRow + 5,0);//将指针移到选定行首
    refresh();
}
void DisplayLogg::LoggTimeBreak_ProblemProc(int signo)
{
    LoggRebuildProblemProc();
    return;
}

void DisplayLogg::LoggStartProblemProc()
{
    while(1)
    {
        LoggRebuildProblemProc();
        signal(SIGALRM, LoggTimeBreak_ProblemProc);
        alarm(INTERVAL); 
        int Key = getch();
        alarm(0);
        //如果键入Esc，则退出菜单
        if (Key == 27)
        {
          Logg_ProblemProcSelectedRow=1;
          Logg_ProblemProcStartRow=1;
          break;
        }
        switch (Key)
        {
            //键入回车
            case '\n':
            case '\r':
              break;
            case KEY_UP:
                Logg_ProblemProcSelectedRow--;
              break;
            case KEY_DOWN:
                Logg_ProblemProcSelectedRow++;
              break;
            case KEY_LEFT:
                Logg_ProblemProcStartRow -= iLoggPageSize;
                Logg_ProblemProcSelectedRow -= iLoggPageSize;
              break;
            case KEY_RIGHT:
                Logg_ProblemProcStartRow += iLoggPageSize;
                Logg_ProblemProcSelectedRow += iLoggPageSize;
              break;
            default	:
              break;
        }
    }
}

void DisplayLogg::LoggRebuildProcess()
{
    //此变量用于自上而下绘制菜单
    int lv_iTotalRow = 0;
    int lv_iCurrentRow = 0;
    static char Buffer[COLS_LENS];
    
    LoggLoadEmptyMenu();
    lv_iTotalRow = iDisParamCnt;
    lv_iCurrentRow =1;
    //以下四个判断主要用于纠正一些非法的
    //如果全局选中行数<=0，则回指到gv_iMainTotalRow 行
    if (Logg_Main2SelectedRow  <= 0)
    {
       Logg_Main2SelectedRow  = lv_iTotalRow ;
    }
    //如果全局选中行数> lv_iTotalRow ，则回指到第一行
    if (Logg_Main2SelectedRow  > lv_iTotalRow)
    {
       Logg_Main2SelectedRow  = 1;
    }
    //如果g_iStartRow <= 0，人为置为第一行	
    if (Logg_Main2StartRow <= 0)
    {
       Logg_Main2StartRow = 1;
    }
    if (Logg_Main2StartRow > Logg_Main2SelectedRow )
    {
       Logg_Main2StartRow = Logg_Main2SelectedRow ;
    }
    if (Logg_Main2SelectedRow  - Logg_Main2StartRow >= iLoggPageSize)
    {
       Logg_Main2StartRow = Logg_Main2SelectedRow  - iLoggPageSize + 1;
    }
    while(1)
    {
        if (lv_iCurrentRow >= Logg_Main2StartRow )
        {
           break;
        }
        else
        {
            lv_iCurrentRow++;
        }
    }
    while(lv_iCurrentRow <= lv_iTotalRow)
    {
        if (lv_iCurrentRow >= Logg_Main2StartRow + iLoggPageSize)
        {
            break;
        }
        if (lv_iCurrentRow == Logg_Main2SelectedRow)
        {
           standout();
        }
        mvhline(lv_iCurrentRow - Logg_Main2StartRow + 5,0,' ',COLS_LENS);
        memset(Buffer,0,COLS_LENS);
        
        //输出序号
        sprintf(Buffer,"%d",lv_iCurrentRow);
        mvaddstr(lv_iCurrentRow - Logg_Main2StartRow + 5,0,Buffer);
        //输出类别
//        mvaddstr(lv_iCurrentRow - Logg_Main2StartRow + 5,10,(char*)"日志信息查询");
        //输出名字
        memset(Buffer,0,COLS_LENS);
        sprintf(Buffer,"%s",Logg_pMenuInfo[lv_iCurrentRow-1].ChName);
        mvaddstr(lv_iCurrentRow - Logg_Main2StartRow + 5,10,Buffer);
        if (lv_iCurrentRow == Logg_Main2SelectedRow)
        {
            standend();
        }
        lv_iCurrentRow++;
    }
      //未满的行填入空白		
    while (lv_iCurrentRow < Logg_Main2StartRow + iLoggPageSize)
    {
        mvhline(lv_iCurrentRow - Logg_Main2StartRow + 5,0,' ',COLS_LENS);
        lv_iCurrentRow ++;
    }
    wmove(stdscr,Logg_Main2SelectedRow - Logg_Main2StartRow + 5,0);//将指针移到选定行首
    refresh();
}

void DisplayLogg::LoggTimeBreak_Process(int signo)
{
    LoggRebuildProcess();
    return;
}

void DisplayLogg::LoggInitProcessInfo(int flag,char* sParam)
{
    int iClassId=0;
    int iLevel=0;
    int iType=0;
    switch(flag)
    {
        case GROUPPARAM:
            iDisParamCnt = 1;
            Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
            Logg_pMenuInfo[0].Index  = 1;
            strcpy(Logg_pMenuInfo[0].ChName,"系统参数管理类日志信息");
            break;
        case GROUPMODULE:
        case GROUPPROCESS:
            iDisParamCnt = 3;
            Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
            Logg_pMenuInfo[0].Index  = 1;
            strcpy(Logg_pMenuInfo[0].ChName,"系统错误类(error/fatal)日志信息");

            Logg_pMenuInfo[1].Index  = 2;
            strcpy(Logg_pMenuInfo[1].ChName,"系统告警类(warning)日志信息");

            Logg_pMenuInfo[2].Index  = 3;
            strcpy(Logg_pMenuInfo[2].ChName,"系统普通类(debug/info)日志信息");
            break;                    	
        case GROUPINFOLV:
            if(strlen(sParam))
            {
                iLevel = atoi(sParam);
                iDisParamCnt = 1;
                Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
                Logg_pMenuInfo[0].Index  = 1;
                if(iLevel==LOG_LEVEL_FATAL)
                    strcpy(Logg_pMenuInfo[0].ChName,"系统等级一(灾难错误fatal)日志信息");
                else if(iLevel==LOG_LEVEL_ERROR)
                    strcpy(Logg_pMenuInfo[0].ChName,"系统等级二(错误error)日志信息");
                else if(iLevel==LOG_LEVEL_WARNING)
                    strcpy(Logg_pMenuInfo[0].ChName,"系统等级三(告警warning)日志信息");
                else if(iLevel==LOG_LEVEL_INFO)
                    strcpy(Logg_pMenuInfo[0].ChName,"系统等级四(普通info)日志信息"); 
                else if(iLevel==LOG_LEVEL_DEBUG)
                    strcpy(Logg_pMenuInfo[0].ChName,"系统等级五(调试debug)日志信息");                                           
                else
                    strcpy(Logg_pMenuInfo[0].ChName,"【错误的LEVEL_ID】请检查!");
            }
            else
            {
                iDisParamCnt = 5;
                Logg_pMenuInfo = new Log_Menu[iDisParamCnt];

                Logg_pMenuInfo[0].Index  = 1;
                strcpy(Logg_pMenuInfo[0].ChName,"系统等级一(灾难错误fatal)日志信息");

                Logg_pMenuInfo[1].Index  = 2;
                strcpy(Logg_pMenuInfo[1].ChName,"系统等级二(错误error)日志信息");
                
                Logg_pMenuInfo[2].Index  = 3;
                strcpy(Logg_pMenuInfo[2].ChName,"系统等级三(告警warning)日志信息");
                
                Logg_pMenuInfo[3].Index  = 4;
                strcpy(Logg_pMenuInfo[3].ChName,"系统等级四(普通info)日志信息");
                
                Logg_pMenuInfo[4].Index  = 5;
                strcpy(Logg_pMenuInfo[4].ChName,"系统等级五(调试debug)日志信息");                                
            }
            break;
        case GROUPCLASS:
            if(strlen(sParam))
            {
                iClassId = atoi(sParam);
                iDisParamCnt = 1;
                Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
                Logg_pMenuInfo[0].Index  = 1;
                if(iClassId==LOG_CLASS_ERROR)
                    strcpy(Logg_pMenuInfo[0].ChName,"系统错误类日志信息");
                else if(iClassId==LOG_CLASS_WARNING)
                    strcpy(Logg_pMenuInfo[0].ChName,"系统告警类日志信息");
                else if(iClassId==LOG_CLASS_INFO)
                    strcpy(Logg_pMenuInfo[0].ChName,"系统普通信息类日志信息");
                else
                    strcpy(Logg_pMenuInfo[0].ChName,"【错误的CLASS_ID】请检查!");
            }
            else
            {
                iDisParamCnt = 3;
                Logg_pMenuInfo = new Log_Menu[iDisParamCnt];

                Logg_pMenuInfo[0].Index  = 1;
                strcpy(Logg_pMenuInfo[0].ChName,"系统错误类日志信息");

                Logg_pMenuInfo[1].Index  = 2;
                strcpy(Logg_pMenuInfo[1].ChName,"系统告警类日志信息");
                
                Logg_pMenuInfo[2].Index  = 3;
                strcpy(Logg_pMenuInfo[2].ChName,"系统普通信息类日志信息");
            }
            break;
        case GROUPTYPE:
            if(strlen(sParam))
            {
                iType = atoi(sParam);
                iDisParamCnt = 1;
                Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
                Logg_pMenuInfo[0].Index  = 1;
                if(iType==LOG_TYPE_BUSI)
                    strcpy(Logg_pMenuInfo[0].ChName,"系统业务日志信息");
                else if(iType==LOG_TYPE_PARAM)
                    strcpy(Logg_pMenuInfo[0].ChName,"系统参数管理日志信息");
                else if(iType==LOG_TYPE_SYSTEM)
                    strcpy(Logg_pMenuInfo[0].ChName,"系统系统管理日志信息");
                else if(iType==LOG_TYPE_PROC)
                    strcpy(Logg_pMenuInfo[0].ChName,"系统进程调度日志信息");
                else if(iType==LOG_TYPE_DATEDB)
                    strcpy(Logg_pMenuInfo[0].ChName,"系统操作类日志信息");                                        
                else
                    strcpy(Logg_pMenuInfo[0].ChName,"【错误的CLASS_ID】请检查!");
            }
            else
            {
                iDisParamCnt = 5;
                Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
                
                Logg_pMenuInfo[0].Index  = 1;
                strcpy(Logg_pMenuInfo[0].ChName,"系统业务日志信息");
                Logg_pMenuInfo[1].Index  = 2;
                strcpy(Logg_pMenuInfo[1].ChName,"系统参数管理日志信息");
                Logg_pMenuInfo[2].Index  = 3;
                strcpy(Logg_pMenuInfo[2].ChName,"系统系统管理日志信息");
                Logg_pMenuInfo[3].Index  = 4;
                strcpy(Logg_pMenuInfo[3].ChName,"系统进程调度日志信息");
                Logg_pMenuInfo[4].Index  = 5;
                strcpy(Logg_pMenuInfo[4].ChName,"系统操作类日志信息");                   

            }
            break;   
        case GROUPCODE:
            iDisParamCnt = 1;
            Logg_pMenuInfo = new Log_Menu[iDisParamCnt];
            Logg_pMenuInfo[0].Index  = 1;
            strcpy(Logg_pMenuInfo[0].ChName,"系统错误码操作码日志信息");
            break;        	     	
        default:
            break;
    }
    return;
}

void DisplayLogg::LoggProcessMain(int flag,char* sParam)
{
    Loggstartwin();
    LoggLoadGeneralInfo();
    LoggInitProcessInfo(flag,sParam);
    while(1)
    {
        LoggRebuildProcess();
        signal(SIGALRM, LoggTimeBreak_Process);
        alarm(INTERVAL); 
        int Key = getch();
        alarm(0);
        
        //如果键入Esc，则退出菜单
        if (Key == 27)
        {
          break;
        }
        switch (Key)
        {
            //键入回车
            case '\n':
            case '\r':
                if(flag==GROUPPROCESS||flag==GROUPMODULE)
                {
                    if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 1)
                        LoggloadAll(m_vClaError);
                    else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 2)
                        LoggloadAll(m_vClaWarn);
                    else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 3)
                        LoggloadAll(m_vClaInfo);
                    LoggStartProblemProc();
                }
                else if(flag==GROUPINFOLV)
                {
                    if(strlen(sParam))
                    {
                    	  switch(atoi(sParam))
                    	  {
                    	  	case LOG_LEVEL_FATAL:
                    	  	  LoggloadAll(m_vLevFatal);
                    	  	  break;	
                    	  	case LOG_LEVEL_ERROR:
                    	  	  LoggloadAll(m_vLevError);
                    	  	  break;	
                    	  	case LOG_LEVEL_WARNING:
                    	  	  LoggloadAll(m_vLevWarn);
                    	  	  break;	
                    	  	case LOG_LEVEL_INFO:
                    	  	  LoggloadAll(m_vLevInfo);
                    	  	  break;	
                    	  	case LOG_LEVEL_DEBUG:
                    	  	  LoggloadAll(m_vLevDebug);
                    	  	  break;	                    	  	                      	  	                      	  	                      	  	  
                    	  	default:
                    	  		break;	
                    	  }
                        LoggStartProblemProc();
                    }
                    else
                    {
                        if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 1)
                            LoggloadAll(m_vLevFatal);
                        else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 2)
                            LoggloadAll(m_vLevError);
                        else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 3)
                            LoggloadAll(m_vLevWarn);
                        else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 4)
                            LoggloadAll(m_vLevInfo);
                        else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 5)
                            LoggloadAll(m_vLevDebug);                                                       
                        LoggStartProblemProc();
                    }
                }                  
                else if(flag==GROUPCLASS)
                {
                    if(strlen(sParam))
                    {
                    	  switch(atoi(sParam))
                    	  {
                    	  	case LOG_CLASS_ERROR:
                    	  	  LoggloadAll(m_vClaError);
                    	  	  break;	
                    	  	case LOG_CLASS_WARNING:
                    	  	  LoggloadAll(m_vClaWarn);
                    	  	  break;	
                    	  	case LOG_CLASS_INFO:
                    	  	  LoggloadAll(m_vClaInfo);
                    	  	  break;	                    	  	                      	  	                      	  	                      	  	  
                    	  	default:
                    	  		break;	
                    	  }
                        LoggStartProblemProc();
                    }
                    else
                    {
                      if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 1)
                          LoggloadAll(m_vClaError);
                      else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 2)
                           LoggloadAll(m_vClaWarn);
                      else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 3)
                          LoggloadAll(m_vClaInfo);
                      LoggStartProblemProc();
                    }
                }
                else if(flag==GROUPTYPE)
                {
                    if(strlen(sParam))
                    {
                    	  switch(atoi(sParam))
                    	  {
                    	  	case LOG_TYPE_BUSI:
                    	  	  LoggloadAll(m_vTypeBusi);
                    	  	  break;	
                    	  	case LOG_TYPE_PARAM:
                    	  	  LoggloadAll(m_vTypeParam);
                    	  	  break;	
                    	  	case LOG_TYPE_SYSTEM:
                    	  	  LoggloadAll(m_vTypeSystem);
                    	  	  break;	
                    	  	case LOG_TYPE_PROC:
                    	  	  LoggloadAll(m_vTypeStart);
                    	  	  break;	
                    	  	case LOG_LEVEL_DEBUG:
                    	  	  LoggloadAll(m_vTypeOracle);
                    	  	  break;	                    	  	                      	  	                      	  	                      	  	  
                    	  	default:
                    	  		break;	
                    	  }
                        LoggStartProblemProc();
                    }
                    else
                    {
                        if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 1)
                            LoggloadAll(m_vTypeBusi);
                        else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 2)
                            LoggloadAll(m_vTypeParam);
                        else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 3)
                            LoggloadAll(m_vTypeSystem);
                        else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 4)
                            LoggloadAll(m_vTypeStart);
                        else if(Logg_pMenuInfo[Logg_Main2SelectedRow-1].Index == 5)
                            LoggloadAll(m_vTypeOracle);                                                       
                        LoggStartProblemProc();
                    }
                }                   
                else if(flag==GROUPPARAM)
                {
                    LoggloadAll(m_vTypeParam);
                    LoggStartProblemProc(); 
                }
                else if(flag==GROUPCODE)
                {
                    LoggloadAll(m_vCodeInfo);
                    LoggStartProblemProc();              	
                }
               
              break;
            case KEY_UP:
                Logg_Main2SelectedRow--;
              break;
            case KEY_DOWN:
                Logg_Main2SelectedRow++;
              break;
            default :
              break;
        }
    }
    //退出到命令行模式
    resetty();
    endwin();
    system("clear");
    return ;
}

   //触发报警的时间间隔(单位：秒)
int DisplayLogg::INTERVAL = 1;
//菜单单页显示的行数
int DisplayLogg::iLoggPageSize = LINS_SIZE;
//全局变量：纪录在菜单之间传递的参数
int DisplayLogg::Logg_ProblemProcSelectedRow = 1;
int DisplayLogg::Logg_ProblemProcStartRow = 1;
int DisplayLogg::Logg_Main2SelectedRow = 1;
int DisplayLogg::Logg_Main2StartRow = 1;
int DisplayLogg::NOW_USED=0;
int DisplayLogg::iDisParamCnt=0;
Log_Menu* DisplayLogg::Logg_pMenuInfo = NULL;
Log_Menu* DisplayLogg::Logg_pMenuInfoAll = NULL;

vector<string> DisplayLogg::m_vConverInfo;


