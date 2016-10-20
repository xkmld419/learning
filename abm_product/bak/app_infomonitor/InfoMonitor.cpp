#include "InfoMonitor.h"
#include "Environment.h"
#include "TOCIQuery.h"
#include "Log.h"
#include "IpcKey.h"
#include "SimpleSHM.h"
#include "SHMData.h"
#include "SHMIntHashIndex.h"
#include "SHMStringTreeIndex.h"
#include "SHMIntHashIndex_A.h"
#include "SHMData_A.h"
#include "Log.h"
#include "interrupt.h"
#include "MessageQueue.h"
#include <unistd.h>
//#include "mntapi.h"


//THeadInfo *pInfoHead = NULL;
//TProcessInfo *pProcInfoHead = NULL;

int main(int iArgc,char **sArgv)
{
	InfoMonitor o;
	//Log::init(MBC_APP_infomonitor);
	o.init();
	o.run();
}

InfoMonitor::InfoMonitor(void)
{
	m_iPid = getpid();
	m_iSleep = 10;
	m_pCmdCom = NULL;	
}

InfoMonitor::~InfoMonitor(void)
{
}

bool InfoMonitor::init()
{
	bool bRet = false;
	m_icpu_warning_value			= 0;
	m_imem_warning_value			= 0;
	m_iphysical_memory				= 0;
	m_itable_space_warning_value	= 0;
	m_idisk_space_warning_value		= 0;
	m_iAutoStartProc0 = 0;
	m_icheck_process_time = 0;
	//C_LogAlertApi::m_iProcessID=0;
	//C_LogAlertApi::m_iAppID=0;
  m_bFirstCheck = true;
  
  	m_pCmdCom = new CommandCom();
	
	if(m_pCmdCom)
	{
	  m_pCmdCom->InitClient();
  }
  else
  {
     cout << "向0号进程注册失败" << endl;  
     exit(0);    
  }
		
	 memset(m_sConfigName,0,sizeof(m_sConfigName));	
   char sHome[256]={0};
   char *p;
   if ((p=getenv ("HSSDIR")) == NULL)
       sprintf (sHome, "/opt/opthss");
    else
       sprintf (sHome, "%s/etc", p);	
   
   sprintf(m_sConfigName,"%s/hssconfig", sHome);	
   		
  DEFINE_QUERY(qry);
  
  char cSql[1024] = {0};
  ProcessInfo obj;
  
  try
  {
    sprintf(cSql,"select app_name,app_param,NVL(aotoboot,0) aotoboot,NVL(shiftSystemState,0) shiftSystemState from cfg_infomonitor \
    order by checkorder");
  
    qry.setSQL(cSql);
  
    qry.open();
    
    m_vProcessInfo.clear();
  
    while(qry.next())
    {
    	memset(&obj,0,sizeof(ProcessInfo));
  	  strcpy(obj.cAppName,qry.field("app_name").asString());
  	  strcpy(obj.cAppParam,qry.field("app_param").asString());
  	  obj.iAotoBoot = qry.field("aotoboot").asInteger();
  	  obj.bNeedShiftSysState = qry.field("shiftSystemState").asInteger();
  	  m_vProcessInfo.push_back(obj);
  	  
    }
    qry.close();
  }
  catch (TOCIException &oe) 
  {      
  	    printf(oe.getErrMsg());  
        printf(oe.getErrSrc());
        throw oe;
  }
	return bRet;

}

int InfoMonitor::refashParam(char *strFile)
{
	int iRet = 0;
	char tmp[64] = {0};
	//m_iobject_shm_hwm_warming = m_oIni.readIniInteger(strFile,"MEMORY","object.shm_hwm_warming",-1);
  //if(m_iobject_shm_hwm_warming == -1)
  //{
  //	m_iobject_shm_hwm_warming= 99 ;
  //}
  
 // refashSHMParam(strFile,m_vIpcInfo);
	/*
	for (it = m_vIpcInfo.begin();it != m_vIpcInfo.end(); it++ )
	{
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp,"%s.shm_hwm_warming",it->m_lIpcKey);
		it->m_imemory_shm_hwm_warming = m_oIni.readIniInteger(strFile,"MEMORY",tmp,-1);
		if (it->m_imemory_shm_hwm_warming)
		{
			it->m_imemory_shm_hwm_warming = m_iobject_shm_hwm_warming;
		}
	}
	*/
	//日志等级
	//C_LogAlertApi::m_iAlarmLevel=m_oIni.readIniInteger(strFile,"LOG","log_info_level",-1);
	//if(C_LogAlertApi::m_iAlarmLevel == -1)
	//{
	  //add log info      
	//}
	/*
	//cpu告警阀值
	m_icpu_warning_value			= m_oIni.readIniInteger(strFile,"MONITOR","cpu_warning_value",-1);
	if( m_icpu_warning_value == -1 )
	{
	  //add log info      		
		iRet = -1;
	}
	//内存告警阀值
	m_imem_warning_value			= m_oIni.readIniInteger(strFile,"MONITOR","mem_warning_value",-1);
	if( m_imem_warning_value == -1 )
	{
    //add log info      		
		iRet = -1;
	}

	//表空间告警阀值
	m_itable_space_warning_value	= m_oIni.readIniInteger(strFile,"MONITOR","table_space_warning_value",-1);
	if( m_itable_space_warning_value == -1 )
	{
    //add log info       		
		iRet = -1;
	}
	//硬盘空间告警阀值
	m_idisk_space_warning_value		= m_oIni.readIniInteger(strFile,"MONITOR","disk_space_warning_value",-1);
	if( m_idisk_space_warning_value == -1 )
	{
		 //add log info      		
		iRet = -1;
	}
	*/
	//常驻进程检查时间
	m_icheck_process_time = m_oIni.readIniInteger(strFile,"MONITOR","check_process_time",-1);
	if( m_icheck_process_time == -1 )
	{
    //add log info     		
		iRet = -1;
	}
	
	//DCC积压消息数
	m_imax_dcc_request_cnt = m_oIni.readIniInteger(strFile,"MONITOR","max_dcc_request_cnt",-1);
	if( m_imax_dcc_request_cnt == -1 )
	{
    //add log info     		
		iRet = -1;
	}
	
		//DCC监控消息队列
	m_imqID = m_oIni.readIniInteger(strFile,"MONITOR","max_dcc_request_mq",-1);
	if( m_imqID == -1 )
	{
    //add log info     		
		iRet = -1;
	}
	

	m_strState = m_oIni.readIniString(strFile,"SYSTEM","state",tmp,"-1");
	if( m_strState == "offline" )
		m_iAutoStartProc0 = 0;
	else if(m_strState == "-1")
		iRet = -1;
	else
		m_iAutoStartProc0 = 1;
	return iRet;
}

//获取cpu，内存信息

int InfoMonitor::getSysInfo()
{
	 int iRtn=0;
	 if(PrepareCpuPercent())
	 {
	  if( m_icpu_warning_value != -1 && m_iCpuPercent > m_icpu_warning_value )
	  {
		  iRtn = -1;
      m_oLog.m_iLogProid = -1;

   	  m_oLog.m_iLogAppid = -1;
	
      m_oLog.log(MBC_CPU_OVERLOAD,LOG_LEVEL_WARNING,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"CPU超负荷,使用率[%d%%]",m_iCpuPercent);	  	
	  }	 	
	 }
   if(PrepareMemPercent())
   {
    if (m_iMemPercent > m_imem_warning_value )
	   {
		  iRtn = -1;
		  m_oLog.m_iLogProid = -1;

   	  m_oLog.m_iLogAppid = -1;
	
      m_oLog.log(MBC_MEM_SPACE_SMALL,LOG_LEVEL_WARNING,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"内存超负荷,可用[%sK]使用率[%d%%]",m_sFreeMem,m_iMemPercent);
	   }   	
   }
   
	//if( iRtn == 0 )
	//	printf("CPU内存状态正常,CPU:%d%%内存%d%%",m_iCpuPercent,m_iMemPercent);   
		
	return iRtn ;
}
//获取表空间
int InfoMonitor::getTableSpace()
{
	int iRtn = 0;
	/*
	资源类	table_space_warning_value	监控服务	表空间告警阈值	生效规则：立即 
	取值范围：75～90 
	建议取值：厂家自行确定	当系统的表空间使用率超过预先阀值时告警。
	*/

	char sSql[]="select a.a1 as name, "//--表空间名称
		" c.c2 as type, "//--类型
		" c.c3 as extent, "//--区管理
		" b.b2 / 1024 / 1024 as size_, "//--表空间大小M
		" (b.b2 - a.a2) / 1024 / 1024 as used, "//--已使用M
		" to_number(substr((b.b2 - a.a2) / b.b2 * 100, 1, 5)) as Utilization "//-- 利用率
		" from (select tablespace_name a1, sum(nvl(bytes, 0)) a2"
		" from dba_free_space"
		" group by tablespace_name) a,"
		" (select tablespace_name b1, sum(bytes) b2"
		" from dba_data_files"
		" group by tablespace_name) b,"
		" (select tablespace_name c1, contents c2, extent_management c3"
		" from dba_tablespaces) c"
		" where a.a1 = b.b1"
		" and c.c1 = b.b1";

	DEFINE_QUERY( qry );
	//TOCIQuery qry;
	qry.close( );
	qry.setSQL( sSql );
	qry.open( );
	TabSpac_ oTabSpac;
	//printf("-----------TABLESPACE---------------\n");
	while( qry.next( ) )
	{
		oTabSpac.m_strName	= qry.field(0).asString();
		oTabSpac.m_strType	= qry.field(1).asString();
		oTabSpac.m_strExtent= qry.field(2).asString();
		oTabSpac.m_iSize	= qry.field(3).asInteger();
		oTabSpac.m_iUsed	= qry.field(4).asInteger();
		oTabSpac.m_iUtili	= qry.field(5).asInteger();

		if( oTabSpac.m_iUtili > m_itable_space_warning_value )
		{
			iRtn = -1;
			
			m_oLog.m_iLogProid = -1;

   	  m_oLog.m_iLogAppid = -1;
	
      m_oLog.log(MBC_DB_TABLESPACE_SMALL,LOG_LEVEL_WARNING,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"[%s]表空间不足,已使用[%dM],使用率[%d%%]",
				oTabSpac.m_strName.c_str(),oTabSpac.m_iSize,oTabSpac.m_iUtili);	
		}
	}
	//if( iRtn == 0 )
	//	printf("%s","表空间状态正常!");

	qry.close();
	return 0;
}

//获取共享内存使用情况
int InfoMonitor::getSHMInfo()
{
	/*
	内存类	memory.[object].shm_hwm_warming	DSC	共享内存高水位告警阀值	生效规则：立即 
	取值范围：75～99 
	建议取值：80	object代表数据对象，object可选。
	*/
	vector<IpcInfo_>::iterator it;
	int iRtn = 0;

	//printf("--------------SHMINFO---------------\n");
	for (it = m_vIpcInfo.begin();it != m_vIpcInfo.end();it ++)
	{
		unsigned int iTotal	= 0;
		unsigned int iUsed	= 0;
		switch (it->m_iIpcType)
		{
		case 1: //SHMData
			{
				SHMData<int> sdata( it->m_lIpcKey );
				if( sdata.exist() )
				{
					iTotal     = sdata.getTotal();
					iUsed      = sdata.getCount();
				}
				else
				{
					iTotal     = 0;
					iUsed      = 0;                    
				}
			}
			break;
		case 2: //SHMIntHashIndex
			{
				SHMIntHashIndex siidx( it->m_lIpcKey );
				if( siidx.exist() )
				{
					iTotal     = siidx.getTotal();
					iUsed      = siidx.getCount();                    
				}
				else
				{
					iTotal     = 0;
					iUsed      = 0;    
				}
			}
			break;
		case 3: //SHMStringTreeIndex
			{
				SHMStringTreeIndex ssidx( it->m_lIpcKey );
				if( ssidx.exist() )
				{
					iTotal     = ssidx.getTotal();
					iUsed      = ssidx.getCount();                    
				}
				else
				{
					iTotal     = 0;
					iUsed      = 0;
				}
			}
			break;
		case 5: //SHMIntHashIndex_A
			{
				SHMIntHashIndex_A siidx_a( it->m_lIpcKey );
				if( siidx_a.exist() )
				{
					iTotal     = siidx_a.getTotal();
					iUsed      = siidx_a.getCount();                    
				}
				else
				{
					iTotal     = 0;
					iUsed      = 0;
				}
			}
			break;
			/*
		case 6: //SHMIntHashIndex_AX
			{
				SHMIntHashIndex_AX siidx_ax( it->m_lIpcKey );
				if( siidx_ax.exist() )
				{
					iTotal     = siidx_ax.getTotal();
					iUsed      = siidx_ax.getCount();                    
				}
				else
				{
					iTotal     = 0;
					iUsed      = 0;
				}
			}
			break;
		case 7: //SHMIntHashIndex_KR
			{
				SHMIntHashIndex_KR siidx_kr( it->m_lIpcKey );
				if( siidx_kr.exist() )
				{
					iTotal     = siidx_kr.getTotal();
					iUsed      = siidx_kr.getCount();                    
				}
				else
				{
					iTotal     = 0;
					iUsed      = 0;
				}
			}
			break;
		case 8: //SHMLruData
			{
				SHMLruData<int> sLruDate( it->m_lIpcKey,"",0,0,0 );
				if( sLruDate.exist() )
				{
					iTotal     = sLruDate.getTotal();
					iUsed      = sLruDate.getCount();                    
				}
				else
				{
					iTotal     = 0;
					iUsed      = 0;
				}
			}
			break;
		case 9:	//SHMStringTreeIndex_A
			{
				SHMStringTreeIndex_A ssidx_a( it->m_lIpcKey );
				if( ssidx_a.exist() )
				{
					iTotal     = ssidx_a.getTotal();
					iUsed      = ssidx_a.getCount();                    
				}
				else
				{
					iTotal     = 0;
					iUsed      = 0;
				}
			}
			break;
		case 10:	//SHMStringTreeIndex_AX
			{
				SHMStringTreeIndex_AX ssidx_ax( it->m_lIpcKey );
				if( ssidx_ax.exist() )
				{
					iTotal     = ssidx_ax.getTotal();
					iUsed      = ssidx_ax.getCount();                    
				}
				else
				{
					iTotal     = 0;
					iUsed      = 0;
				}
			}

			break;
		case 11:	//SHMData_A
			{
				SHMData_A<int> sdata_a( it->m_lIpcKey );
				if( sdata_a.exist() )
				{
					iTotal     = sdata_a.getTotal();
					iUsed      = sdata_a.getCount();
				}
				else
				{
					iTotal     = 0;
					iUsed      = 0;                    
				}
			}
			break;
		case 12:	//SHMData_B
			{
				SHMData_B<int> sdata_b( it->m_lIpcKey );
				if( sdata_b.exist() )
				{
					iTotal     = sdata_b.getTotal();
					iUsed      = sdata_b.getCount();
				}
				else
				{
					iTotal     = 0;
					iUsed      = 0;                    
				}
			}
			break;
			*/
		default:
			break;
		}
		//判断是否达到阀值
		if(iTotal>0&& it->m_iIpcType != 0&&it->m_iIpcType != 4 && it->m_iIpcType != 13 )
		{
/*			
			cout<<"IPC_KEY["<<it->m_lIpcKey<<"]";
			cout<<"IPC_NAME["<<it->m_strIpcName<<"]";
			cout<< iTotal <<" ";
			cout<< iUsed <<" ";
			cout<<endl;
*/		

			if( (iUsed * 100) /iTotal > it->m_imemory_shm_hwm_warming )
			{
				iRtn = -1;
		   // C_LogAlertApi::WriteInfo2mq(LOG_LEVEL_1,LOG_TYPE_ALARM,LOG_SUBTYPE_SYSALARM,
        //  MBC_SR_SYS+1,__LINE__, __FILE__,NULL,0,"[%s]共享内存到达阀值。使用率[%d%%]。",it->m_strIpcName,(iUsed * 100) /iTotal);				
			//	ALARMLOG28(0,MBC_CLASS_Memory,123,"[%s]共享内存到达阀值。使用率[%d%%]。",it->m_strIpcName,(iUsed * 100) /iTotal);
			}
	  }
	}
	//printf("------------------------------------\n");		
	if( iRtn == 0 )
		printf("%s","共享内存状态正常!");
	return 0;
}
//#define DEF_AIX
//#define DEF_HP

//获取磁盘空间
int InfoMonitor::getDiskSpace()
{
	/*
	资源类	disk_space_warning_value	监控服务	磁盘空间告警阈值	生效规则：立即 
	取值范围：75～90  
	建议取值：厂家自行确定	当系统的磁盘空间使用率超过预先阀值时告警。
	*/

	int iRtn = 0;
	char *sTok="	 \r\n";//tab," ",,"\r","\n";
	char sLine[1024];
	char sTemp[20] = {0};
	char sFileSys[50] = {0};
	char sFree[20] = {0};
	char sUsed[20] = {0};
	int iDisk = 0;
	//float fAvail;
	//printf("------------DISKSPACE---------------\n");
#if defined DEF_AIX 
{
	FILE* fp = popen("df -k","r");
	if(!fp)
	{
		//布告警信息点
		return -1;
	}
	fgets(sLine,1024,fp);//第一行丢弃

	while(fgets(sLine,1024,fp))
	{
		//strncpy(sFileSys,strtok(sLine," "),50);//Filesystem
		memset(sFileSys,0,50);
		memset(sFree,0,20);
		memset(sUsed,0,20);
		strtok(sLine,sTok);//Filesystem
		strtok(NULL,sTok);//KB blocks
		strncpy(sFree,strtok(NULL,sTok),20);//Free
		strncpy(sUsed,strtok(NULL,sTok),20);//%Used
		strtok(NULL,sTok);//Iused
		strtok(NULL,sTok);//%Iused
		strncpy(sFileSys,strtok(NULL,sTok),15);//Mounted on
		sFileSys[14] = 0;
		sFree[5] = 0;
		sUsed[4] = 0;
		//sprintf(m_sDiskArray[iDisk++],"%-15s   %6s   %5s",sFileSys,sFree,sUsed);
		//printf("%-15s   %6s   %5s\n",sFileSys,sFree,sUsed);
		if( atoi(sUsed) > m_idisk_space_warning_value )
		{
			iRtn = -1;
			m_oLog.m_iLogProid = -1;

   	  m_oLog.m_iLogAppid = -1;
	
      m_oLog.log(MBC_DISC_SPACE_SMALL,LOG_LEVEL_WARNING,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"[%s]磁盘空间不足,剩余[%sK],使用率[%s%]",
			  	sFileSys,sFree,sUsed); 
		}

	}
	pclose(fp);
}
#endif

#if defined DEF_LINUX
{
	FILE* fp = popen("df -k","r");
	if(!fp)
	{
		//布告警信息点
		return -1;
	}
	fgets(sLine,1024,fp);//第一行丢弃

	while(fgets(sLine,1024,fp))
	{
		//strncpy(sFileSys,strtok(sLine," "),50);//Filesystem
		memset(sFileSys,0,50);
		memset(sFree,0,20);
		memset(sUsed,0,20);
		strtok(sLine,sTok);//Filesystem
		strtok(NULL,sTok);//KB blocks
		strtok(NULL,sTok);//used size
		strncpy(sFree,strtok(NULL,sTok),20);//Free
		strncpy(sUsed,strtok(NULL,sTok),20);//%Used
		//strtok(NULL,sTok);//Iused
		//strtok(NULL,sTok);//%Iused
		strncpy(sFileSys,strtok(NULL,sTok),15);//Mounted on
		sFileSys[14] = 0;
		sFree[5] = 0;
		sUsed[4] = 0;
		//sprintf(m_sDiskArray[iDisk++],"%-15s   %6s   %5s",sFileSys,sFree,sUsed);
		//printf("%-15s   %6s   %5s\n",sFileSys,sFree,sUsed);
		if( atoi(sUsed) > m_idisk_space_warning_value )
		{
			iRtn = -1;
    // C_LogAlertApi::WriteInfo2mq(LOG_LEVEL_1,LOG_TYPE_ALARM,LOG_SUBTYPE_SYSALARM,
         // MBC_SR_SYS+9,__LINE__, __FILE__,NULL,0,	"[%s]磁盘空间不足,剩余[%sK],使用率[%s%]",
			  //	sFileSys,sFree,sUsed);		
			//ALARMLOG28(0,MBC_CLASS_Fatal,10,"[%s]磁盘空间不足,剩余[%sK],使用率[%s%]",
			//s	sFileSys,sFree,sUsed);
		}

	}
	pclose(fp);
}
#endif
#ifdef DEF_HP
{
	FILE* fp = popen("bdf","r");
	if(!fp)
	{
		return -1;
	}
	fgets(sLine,1024,fp);//
	while(fgets(sLine,MAX_STRING_LEN,fp))
	{
		memset(sFileSys,0,50);
		memset(sTemp,0,20);
		memset(sFree,0,20);
		memset(sUsed,0,20);
		//strncpy(sFileSys,strtok(sLine,sTok),50);//Filesystem
		strtok(sLine,sTok);
		strncpy(sTemp,strtok(NULL,sTok),20);//kbytes
		if(sTemp[0] == '\0')
		{
			fgets(sLine,MAX_STRING_LEN,fp);
			strtok(sLine,sTok);
		}
		strtok(NULL,sTok);//used
		strncpy(sFree,strtok(NULL,sTok),20);//avail
		strncpy(sUsed,strtok(NULL,sTok),20);//%Used
		strncpy(sFileSys,strtok(NULL,sTok),50);//Mounted on
		//fAvail = atof(sFree)/1024/1024;
		//sprintf(sFree,"%f",fAvail);
		sFileSys[14] = 0;
		sFree[5] = 0;
		sUsed[4] = 0;
		//sprintf(m_sDiskArray[iDisk++],"%-15s   %6s   %5s",sFileSys,sFree,sUsed);
		//printf("%-15s   %6s   %5s\n",sFileSys,sFree,sUsed);
		if( atoi(sUsed) > m_idisk_space_warning_value )
		{
			iRtn = -1;
     //C_LogAlertApi::WriteInfo2mq(LOG_LEVEL_1,LOG_TYPE_ALARM,LOG_SUBTYPE_SYSALARM,
         // MBC_SR_SYS+9,__LINE__, __FILE__,NULL,0,	"[%s]磁盘空间不足,剩余[%sK],使用率[%s%]",
			  //sFileSys,sFree,sUsed);				
		}

	}
	pclose(fp);
}
#endif

	if( iRtn == 0 )
		printf("%s%s","磁盘空间状态正常!使用率为",sUsed);	
//printf("------------------------------------\n");
	return 0;
}

//获取进程状态
int InfoMonitor::getProcessStat(const char* sAppName)
{
	char sCommand[256] = {0};
	FILE *fp	= 0;
	//int ret		= 0;
	char sProcess[256] = {0};
	char sPID[20] = {0};
	int iRtn = 0;
	//char *pUser = getenv("LOGNAME");

#ifdef DEF_HP
#define _guard_def_unix95__
#endif
#ifdef DEF_SOLARIS
#define _guard_def_unix95__
#endif

#ifdef DEF_UNIX95
#define _guard_def_unix95__
#endif

	if( m_iPid > 0 )
	{
#ifdef _guard_def_unix95__
		sprintf (sCommand, "UNIX95= ps -e |grep %s |grep -v grep | grep -v %d|awk '{print $1,$4}'\0",sAppName,m_iPid);
#else
		sprintf (sCommand, "ps -e |grep  %s |grep -v grep| grep -v %d|awk '{print $1,$4}'\0",sAppName,m_iPid);
#endif
	}
	else
	{
#ifdef _guard_def_unix95__
		sprintf (sCommand, "UNIX95= ps -e |grep %s |grep -v grep|awk '{print $1,$4}'\0",sAppName);
#else
		sprintf (sCommand, "ps -e |grep  %s |grep -v grep|awk '{print $1,$4}'\0",sAppName);
#endif
	}
 
	fp = popen (sCommand, "r");
	if(fp == NULL)
	{
    if( m_iPid > 0 )
	  {
      #ifdef _guard_def_unix95__
	    	sprintf (sCommand, "UNIX95= ps -e |grep %s |grep -v grep | grep -v %d|awk '{print $1,$4}'\0",sAppName,m_iPid);
      #else
		    sprintf (sCommand, "ps -e |grep  %s |grep -v grep| grep -v %d|awk '{print $1,$4}'\0",sAppName,m_iPid);
      #endif
	  }
	  else
	  {
       #ifdef _guard_def_unix95__
		     sprintf (sCommand, "UNIX95= ps -e |grep %s |grep -v grep|awk '{print $1,$4}'\0",sAppName);
       #else
		     sprintf (sCommand, "ps -e |grep  %s |grep -v grep|awk '{print $1,$4}'\0",sAppName);
       #endif
	  }
		fp = popen (sCommand, "r");
		iRtn = 2;
	}

	if (fp == NULL)
	{
		m_iPid = 0;
		return -1;
	}
	if (fscanf (fp, "%s %s\n",sPID, sProcess) == EOF)
		iRtn = -1;
	else
		iRtn = 0;
	/*
	printf("----------NO.0 PROCESS--------------\n");
	cout<<sPID<<" ";
	cout<<sProcess<<endl;
	printf("------------------------------------\n");
	*/

	pclose (fp);
	return iRtn;
}

//启动进程
int InfoMonitor::runProcess(const char* sBinName,char* argv[])
{
	char *psPath = getenv("HSSDIR");
	string sBinPath = psPath + string("/bin/");
	sBinPath = sBinPath + sBinName;
	
	pid_t pid = fork();
	switch(pid){
		case 0:
			if((pid = fork()) < 0)
			{
				perror("fork");
			  cout<<"start_Command: fork err"<<endl;
			}
			else if(pid == 0)
			{
				Environment::getDBConn(true);
			  if(-1 ==execve(sBinPath.c_str(),argv,environ))
			  {
			   exit(0);
			  }
			}
			else if(pid > 0)
			{
			  exit(0);
	   	}
			
			break;

		case -1:
			perror("fork");
			//TWfLog::log(WF_FILE,"start_Command: fork err");
			cout<<"start_Command: fork err"<<endl;
			break;

		default:
			if(waitpid(pid,NULL,0) != pid)
			{
				perror("wait pid error");
				exit(0);
			}
			Environment::getDBConn(true);
			//TWfLog::log(0,"child process create ,pid:%d",iForkRet);
			//cout<<"\n\n\n\n====>> Child process create ,pid:"<<pid<<endl;
			break;
	}
	return 0;
}

int InfoMonitor::run()
{
	//Environment::getDBConn();
	
	if(getProcessStat("infomonitor") == 0)
	{
		  m_oLog.m_iLogProid = -1;
   	  m_oLog.m_iLogAppid = -1;
      m_oLog.log(MBC_THREAD_EXIST_ERROR,LOG_LEVEL_WARNING,LOG_CLASS_WARNING,LOG_TYPE_BUSI,-1,NULL,"应用进程已经存在");
		exit(0);
	}
	
	StartProcess();
	// 接管中断
	DenyInterrupt();
	
	while (!GetInterruptFlag ())
	{
	 
		//m_oComm.FlushParam();
		if (refashParam(m_sConfigName) < 0)
		{
			return 0;
		}
		
				//监控DCC消息
		getMessageQueue();
		
		//获取cpu，内存信息
		//getSysInfo(); 由于要修改系统状态，放到schedule里面去监控

		//获取共享内存使用情况
		//getSHMInfo();
		//获取表空间
		//getTableSpace();
		//获取磁盘空间
		//getDiskSpace();
		
		//检查常驻进程
		CheckResident();
		
		sleep(m_iSleep);
	
	}
}

int InfoMonitor::refashSHMParam()
{
	int iRet = 0;
	//vector<IpcInfo_> m_vIpcInfo;
	vector<IpcInfo_>::iterator it;
	IpcInfo_ oIpcInfo;
	//读b_ipc_cfg表获取共享内存
	char sSql[1024]={0};
	char *pUser = getenv("LOGNAME");
	char sHostName[200] = {0};
	int iHost = IpcKeyMgr::getHostInfo(sHostName);

	unsigned int itotal = 0;
	char tmp[64] = {0};

	//IpcInfo_ oIpcInfo;
/*	sprintf(sSql,"select ipc_key,ipc_name,nvl(ipc_type,0),nvl(table_name,0),nvl(ini_time,0) from b_ipc_cfg "
		" where sys_username='%s' and host_id=%d "
		" and ipc_type not in (4,13) ",pUser,iHost);
*/
		sprintf(sSql,"select ipc_key,ipc_name,nvl(ipc_type,0) from b_ipc_cfg "
		" where sys_username='%s' and host_id=%d and ipc_type is not null"
		//" and remark like '%%SR%%' "
		,pUser,iHost);
		
	DEFINE_QUERY(qry);
	qry.close();
	qry.setSQL( sSql );
	qry.open( );
	m_vIpcInfo.clear();

	while( qry.next( ) )
	{
		oIpcInfo.m_lIpcKey			= qry.field(0).asLong();
		oIpcInfo.m_strIpcName		= qry.field(1).asString();
		oIpcInfo.m_iIpcType			= qry.field(2).asInteger();
		oIpcInfo.m_imemory_shm_hwm_warming = m_iobject_shm_hwm_warming;
		m_vIpcInfo.push_back(oIpcInfo);
	}
	qry.close();
	return m_vIpcInfo.size();

}

int InfoMonitor::refashSHMParam(char * sFilePath,vector<IpcInfo_> &vIpcInfo)
{
    char temp[20],sKeyName[200];

    memset(temp,'\0',sizeof(temp));
    memset(sKeyName,'\0',sizeof(sKeyName));
    ReadIni *m_RD = new ReadIni();
    if(m_RD==NULL)
    {
        Log::log(0,"获取核心参数失败");
        return -1;
    }
    vIpcInfo.clear();
	  IpcInfo_ oIpcInfo;
    oIpcInfo.m_imemory_shm_hwm_warming = 	m_iobject_shm_hwm_warming ;
    //能力池
    m_oIni.readIniString(sFilePath,"MEMORY","capdata.ipc_key_value",temp,NULL);
    oIpcInfo.m_lIpcKey = atol(temp);
    m_oIni.readIniString(sFilePath,"MEMORY","capdata.ipc_key_name",sKeyName,NULL);
    oIpcInfo.m_strIpcName = sKeyName;
    oIpcInfo.m_iIpcType=1;
    vIpcInfo.push_back(oIpcInfo);
    m_oIni.readIniString(sFilePath,"MEMORY","capindex.ipc_key_value",temp,NULL);
    oIpcInfo.m_lIpcKey = atol(temp);
    m_oIni.readIniString(sFilePath,"MEMORY","capindex.ipc_key_name",sKeyName,NULL);
    oIpcInfo.m_strIpcName = sKeyName;
    oIpcInfo.m_iIpcType=3;
    vIpcInfo.push_back(oIpcInfo);

    //路由池
    m_oIni.readIniString(sFilePath,"MEMORY","routedata.ipc_key_value",temp,NULL);
    oIpcInfo.m_lIpcKey = atol(temp);
    m_oIni.readIniString(sFilePath,"MEMORY","routedata.ipc_key_name",sKeyName,NULL);
    oIpcInfo.m_strIpcName = sKeyName;
    oIpcInfo.m_iIpcType=1;
    vIpcInfo.push_back(oIpcInfo);
    m_oIni.readIniString(sFilePath,"MEMORY","routeindex.ipc_key_value",temp,NULL);
    oIpcInfo.m_lIpcKey = atol(temp);
    m_oIni.readIniString(sFilePath,"MEMORY","routeindex.ipc_key_name",sKeyName,NULL);
    oIpcInfo.m_strIpcName = sKeyName;
    oIpcInfo.m_iIpcType=3;
    vIpcInfo.push_back(oIpcInfo);
    m_oIni.readIniString(sFilePath,"MEMORY","servicedata.ipc_key_value",temp,NULL);
    oIpcInfo.m_lIpcKey = atol(temp);
    m_oIni.readIniString(sFilePath,"MEMORY","servicedata.ipc_key_name",sKeyName,NULL);
    oIpcInfo.m_strIpcName = sKeyName;
    oIpcInfo.m_iIpcType=1;
    vIpcInfo.push_back(oIpcInfo);
    m_oIni.readIniString(sFilePath,"MEMORY","serviceindex.ipc_key_value",temp,NULL);
    oIpcInfo.m_lIpcKey = atol(temp);
    m_oIni.readIniString(sFilePath,"MEMORY","serviceindex.ipc_key_name",sKeyName,NULL);
    oIpcInfo.m_strIpcName = sKeyName;
    oIpcInfo.m_iIpcType=3;
    vIpcInfo.push_back(oIpcInfo);

    //消息池
    m_oIni.readIniString(sFilePath,"MEMORY","proxydata.ipc_key_value",temp,NULL);
    oIpcInfo.m_lIpcKey = atol(temp);
    m_oIni.readIniString(sFilePath,"MEMORY","proxydata.ipc_key_name",sKeyName,NULL);
    oIpcInfo.m_strIpcName = sKeyName;
    oIpcInfo.m_iIpcType=1;
    vIpcInfo.push_back(oIpcInfo);

    //会话池
    m_oIni.readIniString(sFilePath,"MEMORY","sessiondata.ipc_key_value",temp,NULL);
    oIpcInfo.m_lIpcKey = atol(temp);
    m_oIni.readIniString(sFilePath,"MEMORY","sessiondata.ipc_key_name",sKeyName,NULL);
    oIpcInfo.m_strIpcName = sKeyName;
    oIpcInfo.m_iIpcType=1;
    vIpcInfo.push_back(oIpcInfo);

    //连接池
    m_oIni.readIniString(sFilePath,"MEMORY","conndata.ipc_key_value",temp,NULL);
    oIpcInfo.m_lIpcKey = atol(temp);
    m_oIni.readIniString(sFilePath,"MEMORY","conndata.ipc_key_name",sKeyName,NULL);
    oIpcInfo.m_strIpcName = sKeyName;
    oIpcInfo.m_iIpcType=1;
    vIpcInfo.push_back(oIpcInfo);    
    m_oIni.readIniString(sFilePath,"MEMORY","connindex.ipc_key_value",temp,NULL);
    oIpcInfo.m_lIpcKey = atol(temp);
    m_oIni.readIniString(sFilePath,"MEMORY","connindex.ipc_key_name",sKeyName,NULL);
    oIpcInfo.m_strIpcName = sKeyName;
    oIpcInfo.m_iIpcType=3;
    vIpcInfo.push_back(oIpcInfo);   
     
    return 0;
}

/******************************************************************************************
*输入：	无
*输出：	无
*返回：	无
*调用：	成员函数
*描述：	准备CPU比例
*******************************************************************************************/
bool InfoMonitor::PrepareCpuPercent()
{
	FILE *fp=NULL;
	char ScanBuff[256]={'\0'};
	char sCpuPercent[256]={'\0'};
	m_iCpuPercent=0;
	
	memset(sCpuPercent,0x00,sizeof(sCpuPercent));
	//ps aux|sed -n '2,$ p' |awk 'BEGIN{total = 0} {total += $3} END{print total}'
	//hpunix vmstat 1 4 |sed -n '3,7p'|awk 'BEGIN{total = 0} {total +=$14} END{print total/4}'
	#ifdef DEF_AIX
	strcpy(ScanBuff," vmstat 1 4 |sed -n '7,10p'|awk 'BEGIN{total = 0} {total +=$16} END{print total/4}' ");
	#endif
	#ifdef DEF_HP
	strcpy(ScanBuff," vmstat 1 4 |sed -n '3,7p'|awk 'BEGIN{total = 0} {total +=$18} END{print total/4}' ");
	#endif
	#ifdef DEF_LINUX
	//strcpy(ScanBuff," vmstat 1 4 |sed -n '3,7p'|awk 'BEGIN{total = 0} {total +=$15} END{print total/4}' ");
  strcpy(ScanBuff," vmstat |sed -n '3,7p'|awk '{print $15}' ");	
	#endif
	if((fp = popen(ScanBuff ,"r")) == 0)
	{
		throw TException("DCCollectFactor::PrepareCpuPercent Error");
	}
	if (fgets(sCpuPercent,sizeof(sCpuPercent),fp))
	{
		pclose(fp);
		m_iCpuPercent=100-atoi(sCpuPercent);
		return true;

	}
	else
	{
		pclose(fp);
		return false;
	}
}



/******************************************************************************************
*输入：	无
*输出：	无
*返回：	无
*调用：	成员函数
*描述：	准备MEM使用率
*******************************************************************************************/
bool InfoMonitor::PrepareMemPercent()
{
	char ScanBuff[256]={'\0'};
	char sTmpBuff[128]={'\0'};
	char sTmp[128]={'\0'};
	char sTotalMem[128]={'\0'};
	char sFreeMem[128]={'\0'};
	char *p=NULL;
	m_iMemPercent=0;
	FILE * fp=NULL;

	#ifdef DEF_LINUX
	strcpy(ScanBuff," free -m|sed -n '2p'|awk '{print $2}' ");
	if((fp = popen(ScanBuff ,"r")) == 0)
	{
		throw TException("DCCollectFactor::PrepareMemPercent Error");
	}
	if (fgets(sTmpBuff,sizeof(sTmpBuff),fp))
	{
		pclose(fp);
		strcpy(sTotalMem,sTmpBuff);
		
	}
	else
	{
		pclose(fp);
		return false;
	}
	#else
	
	#ifdef __IBM_AIX__
	strcpy(ScanBuff," vmstat |grep \"System\"|grep -v \"grep\"|awk '{print $4}' ");//命令结果为mem=41984MB
	#endif
	#ifdef DEF_HPUX
	strcpy(ScanBuff," machinfo|grep \"Memory\"|awk '{print $2}' ");//Memory = 16353 MB (15.969727 GB)
	#endif
	if((fp = popen(ScanBuff ,"r")) == 0)
	{
		throw TException("DCCollectFactor::PrepareMemPercent Error");
	}
	if (fgets(sTmpBuff,sizeof(sTmpBuff),fp))
	{
		pclose(fp);
		//strcpy(sTmp,&sTmpBuff[4]);//去掉前四位mem=
		p=strstr(sTmpBuff,"=");
		strcpy(sTmp,p+1);
		p=NULL;
		p=index(sTmp,'M');
		strncpy(sTotalMem,sTmp,p-sTmp);//去掉后两位MB
		strcat(sTotalMem,'\0');		
	}
	else
	{
		pclose(fp);
		return false;
	}
	#endif

	memset(ScanBuff,0x00,sizeof(ScanBuff));

	#ifdef __IBM_AIX__
	//hpunix vmstat 1 4 |sed -n '3,7p'|awk 'BEGIN{total = 0} {total += $4} END{print total/4/1024}'
	strcpy(ScanBuff," vmstat 1 4 |sed -n '7,10p'|awk 'BEGIN{total = 0} {total += $4} END{print total/1024}' ");//获取FREE内存，已经转化为MB
	#endif
	#ifdef DEF_HPUX
	strcpy(ScanBuff," vmstat 1 4 |sed -n '3,7p'|awk 'BEGIN{total = 0} {total += $5} END{print total/1024}' ");//获取FREE内存，已经转化为MB
	#endif
	#ifdef DEF_LINUX
	strcpy(ScanBuff," vmstat 1 4 |sed -n '3,7p'|awk 'BEGIN{total = 0} {total += $4} END{print total/1024}' ");//获取FREE内存，已经转化为MB	
	#endif
	if((fp = popen(ScanBuff ,"r")) == 0)
	{
		throw TException("DCCollectFactor::PrepareMemPercent Error");
	}
	if (fgets(sFreeMem,sizeof(sFreeMem),fp))
	{
		pclose(fp);	
	}
	else
	{
		pclose(fp);
		return false;
	}
	// //## 返回使用百分比，精确到整数
	m_iMemPercent=((atoi(sTotalMem)-atoi(sFreeMem))*100/atoi(sTotalMem));
	memset(m_sFreeMem,0,sizeof(m_sFreeMem));
	strncpy(m_sFreeMem,sFreeMem,127);
	return true;


}
void InfoMonitor::StartProcess()
{
	char* argv[20] ={0};
	int iPos = 0;
		
  for(vector <ProcessInfo>::iterator itr = m_vProcessInfo.begin(); itr!=m_vProcessInfo.end();itr++)
  {
  	if(itr->iAotoBoot != 1)
  	{//不是启动系统启动一次的不处理
  		continue;
  	}
  	memset(argv,0,sizeof(argv));
  	iPos = 0;
  	
  		printf("启动%s进程....",itr->cAppName);			
			argv[iPos] = itr->cAppName;	
			iPos++;
			char tempargv[200]={0};
			strcpy(tempargv,itr->cAppParam);
			argv[iPos] = strtok(tempargv," \n");
			iPos ++;
			while((argv[iPos] = strtok(NULL," \n")) != NULL)
			{
				iPos++;
			}
			runProcess(itr->cAppName,argv);
  }
	
}

void InfoMonitor::CheckResident()
{
	int iRtn = 0;
	if(m_bFirstCheck)
	{
	  m_tLastCheckTime = time(NULL);
	  m_bFirstCheck = false;
	}
	else
	{
		time_t tNow = time(NULL);
		if( difftime(tNow,m_tLastCheckTime) < m_icheck_process_time )
		{
			return;
		}	
	}
	
	char* argv[20] ={0};
	int iPos = 0;
		
  for(vector <ProcessInfo>::iterator itr = m_vProcessInfo.begin(); itr!=m_vProcessInfo.end();itr++)
  {
  	if(itr->iAotoBoot == 1)
  	{//只启动一次不监控的不管
  		continue;
  	}
  	memset(argv,0,sizeof(argv));
  	iPos = 0;
  	iRtn = getProcessStat(itr->cAppName);
  	if(iRtn == -1)
  	{
  		m_oLog.m_iLogProid = -1;
   	  m_oLog.m_iLogAppid = -1;
      m_oLog.log(MBC_THREAD_EXIT_ERROR,LOG_LEVEL_FATAL,LOG_CLASS_ERROR,LOG_TYPE_SYSTEM,-1,NULL,"致命错误，%s进程不存在",itr->cAppName); 
  	
  	
  	if(itr->bNeedShiftSysState)
  	{
         char sCmd[100] = {0};
	       sprintf(sCmd,"SYSTEM.run_state=fault\0");
         m_pCmdCom->changeSectionKey(sCmd);

		 if(strcmp(itr->cAppName,"initserver") == 0)
		 {
		   char sCmd[100] = {0}; 
		   sprintf(sCmd,"SYSTEM.state=offline\0");
			m_pCmdCom->changeSectionKey(sCmd);

			memset(sCmd,0,100);
			 sprintf(sCmd,"SYSTEM.mid_state=offline\0");
			  m_pCmdCom->changeSectionKey(sCmd);
			   memset(sCmd,0,100);
				 sprintf(sCmd,"SYSTEM.net_state=outnet\0");
				  m_pCmdCom->changeSectionKey(sCmd);
		 }
  	}
  	
  	if(itr->iAotoBoot && m_iAutoStartProc0 == 1)
  	{   
	    
		iPos = 0;
  		printf("启动%s进程....",itr->cAppName);			
			argv[iPos] = itr->cAppName;	
			iPos++;
			char temp[200]={0};
			strcpy(temp,itr->cAppParam);
			argv[iPos] = strtok(temp," \n");
			iPos ++;
			while((argv[iPos] = strtok(NULL," \n")) != NULL)
			{
				iPos++;
			}
			runProcess(itr->cAppName,argv);
  	}
  }
  }
}

void InfoMonitor::getMessageQueue()
{
	MessageQueue mq(m_imqID);
	if(mq.exist())
	{
	  int iNum = mq.getMessageNumber();
	
	  if(iNum >= m_imax_dcc_request_cnt)
	  {
			m_oLog.m_iLogProid = -1;
   	  m_oLog.m_iLogAppid = -1;
      m_oLog.log(MBC_MAX_DCC_REQUEST_CNT,LOG_LEVEL_WARNING,LOG_CLASS_WARNING,LOG_TYPE_SYSTEM,-1,NULL,"DCC请求消息积压达到最大值");
	  }
	}
	
}
