#include "HssProcessInit.h"
#include "Environment.h"
HssProInfo::HssProInfo()
{
		proid = 0;
		exename[0] = 0;
		execpath[0] = 0;
		parameter[0] = 0;
		cSate[0] = 'N';
}

HssProInfo::~HssProInfo()
{
}


HssProcessInit::HssProcessInit()
{
}

HssProcessInit::~HssProcessInit()
{
}

int HssProcessInit::startApp(char *appname)
{
	if(!appname || appname[0] == '\0')
		return 0;
	int cnt = 0;
	if(getProcess(appname))
	{
		for(vector<HssProInfo>::iterator itr = vec_process.begin();itr!=vec_process.end();itr++)
		{
			if(startProcess(*itr))
				cnt++;
		}
		return cnt;
	}
	return cnt;
}

int HssProcessInit::getProcess(char *appname)
{
	vec_process.clear();
	//
	char *sUserName = getenv("LOGNAME");
    char sHostInfo[100];
    int  iHostID = IpcKeyMgr::getHostInfo (sHostInfo);
	//
	char sSql[2048];
    memset(sSql, 0 , 2048);
    TOCIQuery qry(Environment::getDBConn());
    sprintf(sSql," select nvl(a.process_id,0), nvl(a.exec_path,'') ,nvl(a.state,'N'),nvl(a.parameter,''),nvl(a.billflow_id,0) From wf_process a,wf_application b where a.app_id=b.app_id and b.exec_name='%s' and host_id=%d and sys_username='%s' Order By billflow_id, process_id",appname,iHostID,sUserName);
    qry.setSQL(sSql);
    qry.open();
    int cnt = 0;
    while(qry.next())
    {
    		HssProInfo mPro;
    		mPro.proid = qry.field(0).asLong();
			if(mPro.proid<=0)
				continue;
    		strncpy(mPro.execpath,qry.field(1).asString(),MAX_EXEC_PATH_LEN);
    		strncpy(mPro.cSate , qry.field(2).asString(),1);
			if(mPro.cSate[0] == 'Q')
				continue;
    		strncpy(mPro.parameter , qry.field(3).asString(),MAX_PARAMTER_LEN);
    		mPro.flowid = qry.field(4).asLong();
    		strncpy(mPro.exename,appname,MAX_PRP_NAME_LEN);
    		vec_process.push_back(mPro);
    		cnt++;
	}
	return cnt;
}

bool HssProcessInit::startProcess(HssProInfo proinfo)
{
		 if(proinfo.proid <=0 )
		 	return false;
		 if(proinfo.cSate[0] != 'R')
		 {
		 	  char tmp[1024] = {0};
			  sprintf(tmp,"export PROCESS_ID=%ld",proinfo.proid);
			  if(system(tmp)!=0){
			  	printf("PROCESS_ID[%ld]ÉèÖÃÊ§°Ü\n",proinfo.proid);
			  	return false;
			  }
			  tmp[0] = 0;
			  if(proinfo.execpath[strlen(proinfo.execpath)-1] == '/')
			  	sprintf(tmp,"%s%s %s",proinfo.execpath,proinfo.exename,proinfo.parameter);
			  else
			  	sprintf(tmp,"%s/%s %s",proinfo.execpath,proinfo.exename,proinfo.parameter);
			  if(system(tmp)!=0){
			  	printf("PROCESS_ID[%ld]³ÌÐò[%s]Æô¶¯Ê§°Ü\n",proinfo.proid,proinfo.exename);
			  	return false;
			  }
		 }
		 return true;
}
