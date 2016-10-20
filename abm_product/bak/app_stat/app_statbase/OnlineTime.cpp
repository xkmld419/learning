#include "StatFormBase.h"
#include "OnlineTime.h"

OnlineTime::OnlineTime()
{
}

OnlineTime::~OnlineTime()
{
	if(!m_pCmd)
		delete m_pCmd;
}

void OnlineTime::Rebuild()
{
	//此变量用于自上而下绘制菜单
	int lv_iTotalRow = 0;
	int lv_iCurrentRow = 0;
	static char Buffer[COLS_LENS];

	LoadEmptyMenu();
	DisplayTime();

	memset(Buffer,0,COLS_LENS);
	char sOnlineTime[20] = {0};
	char sSysParamIni[1024];
	char sTemp[16] = {0};
	char sTime[16] = {0};
	memset(sSysParamIni,0,sizeof(sSysParamIni));

	sprintf(Buffer,"%s系统情况展示",SYSNAME);
	mvaddstr(4,0,Buffer);
	memset(Buffer,0,sizeof(Buffer));

	char *p = NULL;
	if ((p=getenv (ENV_HOME)) == NULL){
		printf("错误：获取环境变量$%s失败\n",ENV_HOME);
		return;
	}
	if (p[strlen(p)-1] == '/') {
		sprintf(sSysParamIni,"%setc/%s",p,CONFIG_FILE);
	} else {
		sprintf(sSysParamIni,"%s/etc/%s",p,CONFIG_FILE);
	}

	m_ReadFile.readIniString(sSysParamIni,"system","billing_online_time",sOnlineTime,"未设置");
	//Date date(pInfoHead->bootTime);

	Date date(sOnlineTime);

	int iYear = date.getYear();
	int iMonth = date.getMonth();
	int iDay = date.getDay();
	int iHour = date.getHour();
	int iMin = date.getMin();
	int iSec = date.getSec();
	sprintf(Buffer,"启动时间：%04d年%02d月%02d日 %02d时%02d分%02d秒",iYear,iMonth,iDay,iHour,iMin,iSec);
	mvaddstr(6,0,Buffer);
	time_t Now;
	struct tm  *Time;
	time(&Now);
	Time = localtime(&Now);	

	sprintf(Buffer,"当前时间：%04d年%02d月%02d日 %02d时%02d分%02d秒",Time->tm_year+1900,Time->tm_mon+1,Time->tm_mday,Time->tm_hour,Time->tm_min,Time->tm_sec);
	mvaddstr(8,0,Buffer);
	memset(sTemp,0,sizeof(sTemp));
	memset(Buffer,0,sizeof(Buffer));
	if(sSysParamIni[0] != '\0'){
		m_ReadFile.readIniString(sSysParamIni,"system","state",sTemp,NULL);
		sprintf(Buffer,"系统业务状态：%s",sTemp);
		mvaddstr(12,0,Buffer);
		if(!strcmp(sTemp,"online")){
			memset(Buffer,0,sizeof(Buffer));

			//sprintf(Buffer,"系统BILLING-ONLINE时间：%d年%d月%d日 %d时%d分%d秒",Time->tm_year+1900-iYear,Time->tm_mon+1-iMonth, \
			//Time->tm_mday-iDay,Time->tm_hour-iHour,Time->tm_min-iMin,Time->tm_sec-iSec);
			if(strlen(sOnlineTime) != 14)
				sprintf(Buffer,"系统SGW-ONLINE时间: 0年0月0日 0时0分0秒");
			else{
				Date sOnlineDate(sOnlineTime);
				static Date sCurDate;
				sCurDate.getCurDate();
				int iDiffSec = sCurDate.diffSec(sOnlineDate);
				int iDiffYear = iDiffSec/(365*24*60*60);
				int iDiffMonth = iDiffSec/(30*24*60*60) - (iDiffYear*12);
				int iDiffDay = iDiffSec/(24*60*60) - iDiffYear*12*30 - iDiffMonth*30;
				int iDiffHour = iDiffSec/(60*60) - iDiffYear*12*30*24 - iDiffMonth*30*24 - iDiffDay*24;
				int iDiffMin = iDiffSec/(60) - iDiffYear*12*30*24*60 - iDiffMonth*30*24*60 - iDiffDay*24*60 - iDiffHour*60;
				iDiffSec = iDiffSec - iDiffYear*12*30*24*60*60 - iDiffMonth*30*24*60*60 - iDiffDay*24*60*60 - iDiffHour*60*60 - iDiffMin*60;
				sprintf(Buffer,"系统BILLING-ONLINE时间：%d年%d月%d日 %d时%d分%d秒",iDiffYear,iDiffMonth,iDiffDay,iDiffHour,iDiffMin,iDiffSec);
				mvaddstr(10,0,Buffer);
			}
			mvaddstr(10,0,Buffer);
		}
		else{
			mvaddstr(10,0,"系统BILLING-ONLINE时间: 0年0月0日 0时0分0秒");
		}
		memset(sTemp,0,sizeof(sTemp));
		m_ReadFile.readIniString(sSysParamIni,"system","run_state",sTemp,NULL);
		memset(Buffer,0,sizeof(Buffer));
		sprintf(Buffer,"系统运行状态: %s",sTemp);
		mvaddstr(14,0,Buffer);

		memset(sTemp,0,sizeof(sTemp));
		m_ReadFile.readIniString(sSysParamIni,"system","net_state",sTemp,NULL);
		sprintf(Buffer,"系统网络状态: %s",sTemp);
		mvaddstr(16,0,Buffer);
	}
	refresh();
}
