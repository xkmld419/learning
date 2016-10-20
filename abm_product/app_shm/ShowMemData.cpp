#include "ShowMemData.h"
#include "ThreeLogGroup.h"
#include "CSemaphore.h"
#include "ThreeLogGroup.h"
#include "SHMParamStruct.h"
#include "IniFileMgr.h"
#include <stdio.h>
#include <string.h>

/*
 *	函 数 名:   printUsage
 *	函数功能:   帮助函数
 *	时    间:   2011年3月15日
 *	返 回 值:   void
 *	参数说明:  
*/


void ShmMgr::printUsage()
{
    cout << endl << endl;
    cout << "*********************************************************************************" << endl;
    cout << "                                  展现类命令:                                    " <<endl;  
    cout << "abmshm" << " "  << "-stat   :                          显示所有共享内存的状态信息"<<endl;
    
    cout << "*********************************************************************************" << endl;
    /*
    cout << "abmshm" << " "  << "-show tablename  [column=value]:   显示table_name[列值为value]的详细记录"<<endl;
    cout << "abmshm" << " "  << "-show log  :                       显示内存日志区的内容"<<endl;
    cout << "abmshm" << " "  << "-count tablename [column=value]:   显示table_name[列值为value]的记录数"<<endl;
    cout << "abmshm" << " "  << "-lockstat  [tablename] :           显示共享内存所有锁[tablename]的实体及锁类型"<<endl;
    cout << "abmshm" << " "  << "-lockreset [tablename] :           将所有/[tablename]的锁复位"<<endl;
    cout << "abmshm" << " "  << "-lockset   [tablename] :           将所有/[tablename]的锁锁住"<<endl;
    cout << "abmshm" << " "  << "-create    [tablename] :           创建所有/[tablename]的信号锁"<<endl;        
    */
    cout << endl;
    return ;
}

bool ShmMgr::show()
{
		if (g_argc<2|| g_argv[1][0]!= '-' ) 
		{			
            printUsage();
			return 0;
		}
        if(!getIpcKey()) 
    	    return 0;
    	    
		if(!strcmp(g_argv[1],"-stat"))
		{			
			showStatMem();
			showLRUMem();
			return 0 ;	
		}
		if(!strcmp(g_argv[1],"-tt"))
		{			
			showTTMem();
			return 0 ;	
		}
		/*		
		if(!strcmp(g_argv[1],"-show"))
		{
			if(g_argc==3 && !strcmp(g_argv[2],"-log"))
				{
					showLogMem();
					return 0;
				}
       
            if(g_argc==3) { showTaleMem(g_argv[2]); 			     return  0; }
            if(g_argc==4) { showTaleMem(g_argv[2],g_argv[3]);  return  0; }

		}
		
		if(!strcmp(g_argv[1],"-count"))
		{			
            if(g_argc==3) { showCountMem(g_argv[2]); 			      return  0; }
            if(g_argc==4) { showCountMem(g_argv[2],g_argv[3]);  return  0; }
		}
		
		if(!strcmp(g_argv[1],"-lockstat"))
		{
            if(g_argc==2) { showSemStat();   return  0; }			
            if(g_argc==3) { showSemStat(g_argv[2]);  return  0; }
		}	
		
		if(!strcmp(g_argv[1],"-lockreset"))
		{
            if(g_argc==2) { showSemReset();  return  0; }			
            if(g_argc==3) { showSemReset(g_argv[2]); 	return  0; }

		}	
		
		if(!strcmp(g_argv[1],"-lockset"))
		{
            if(g_argc==2) { lockSemStat();   return  0; }			
            if(g_argc==3) { lockSemStat(g_argv[2]);  return  0; }
		}		
		
		if(!strcmp(g_argv[1],"-creat"))
		{
            if(g_argc==2) { creatSem();   return  0; }			
            if(g_argc==3) { creatSem(g_argv[2]);  return  0; }
		}

		if(!strcmp(g_argv[1],"-ckey"))
		{
            if(g_argc==2) {printf("请输入key值\n"); return 0; }			
            if(g_argc==3) { creatOne(g_argv[2]);  return  0; }
		}
		*/											
        printUsage();
		return 0;		
}

bool ShmMgr::getIpcKey()
{
	 char sHome[500],sFile[500];
   char temp[20],sKeyName[200];	    
   memset(sHome,'\0',sizeof(sHome));
   memset(sFile,'\0',sizeof(sFile));
   SHMINFO oSHMInfo;
   m_vSHMInfo.clear();
   
   SEMINFO oSEMInfo;
   m_vSEMInfo.clear();
   

   //m_vLRUInfo.clear();
   char *p=0;
   if ((p=getenv ("BILLDIR")) == NULL)
         sprintf (sHome, "/opt/opthb/etc");
     else
         sprintf (sHome, "%s/etc", p);
     if(sHome[strlen(sHome)-1] == '/') 
         sprintf (sFile, "%abmconfig", sHome);
     else
         sprintf (sFile, "%s/abmconfig", sHome);
 
    memset(temp,'\0',sizeof(temp));
    memset(sKeyName,'\0',sizeof(sKeyName));
    ReadIni *m_RD = new ReadIni();
    if(m_RD==NULL)   
    {
       // Log::log(0,"获取核心参数失败");
        return -false;
    }  
    //共享内存
     //0号进程
    oSHMInfo.m_lIpcKey = m_RD->readIniLong(sFile,"MEMORY","param.sem_key_value",0);
    m_RD->readIniString(sFile,"MEMORY","param.ipc_key_name",sKeyName,NULL);
    oSHMInfo.m_strIpcName = sKeyName;
    oSHMInfo.m_iIpcType=1;
    oSHMInfo.m_iTotal = m_RD->readIniLong(sFile,"MEMORY","param.init_data_size",0);
    m_vSHMInfo.push_back(oSHMInfo);

    //日志组  
    ThreeLogGroup Logg;                                             
    int log_size= m_RD->readIniLong(sFile,"MEMORY","log_data_size",0);
    oSHMInfo.m_lIpcKey = m_RD->readIniLong(sFile,"MEMORY","loggroupdata_one.ipc_key_value",0);
    m_RD->readIniString(sFile,"MEMORY","loggroupdata_one.ipc_key_name",sKeyName,NULL);
    oSHMInfo.m_strIpcName = sKeyName;
    oSHMInfo.m_iTotal=log_size ;
    oSHMInfo.m_iIpcType=2;
    //oSHMInfo.m_iState=Logg.GetGroupState(1);
    m_vSHMInfo.push_back(oSHMInfo);

    
    oSHMInfo.m_lIpcKey = m_RD->readIniLong(sFile,"MEMORY","loggroupdata_two.ipc_key_value",0);
    m_RD->readIniString(sFile,"MEMORY","loggroupdata_two.ipc_key_name",sKeyName,NULL);
    oSHMInfo.m_strIpcName = sKeyName;
    oSHMInfo.m_iIpcType=2;
    oSHMInfo.m_iTotal=log_size ;  
    //oSHMInfo.m_iState=Logg.GetGroupState(2)  ;    
    m_vSHMInfo.push_back(oSHMInfo);

    
    oSHMInfo.m_lIpcKey = m_RD->readIniLong(sFile,"MEMORY","loggroupdata_thr.ipc_key_value",0);
    m_RD->readIniString(sFile,"MEMORY","loggroupdata_thr.ipc_key_name",sKeyName,NULL);
    oSHMInfo.m_strIpcName = sKeyName;
    oSHMInfo.m_iIpcType=2;
    oSHMInfo.m_iTotal=log_size ;
    //oSHMInfo.m_iState=Logg.GetGroupState(3)  ;  
    m_vSHMInfo.push_back(oSHMInfo);
    
    
    //hcodedata
    oSHMInfo.m_lIpcKey = m_RD->readIniLong(sFile,"MEMORY","hcodedata.ipc_key_value",0);
    m_RD->readIniString(sFile,"MEMORY","hcodedata.ipc_key_name",sKeyName,NULL);
    oSHMInfo.m_strIpcName = sKeyName;
    oSHMInfo.m_iTotal=m_RD->readIniLong(sFile,"MEMORY","hcodedata.max_shm_size",0);
    oSHMInfo.m_iIpcType=3;
    m_vSHMInfo.push_back(oSHMInfo);
        
    //mindata
    oSHMInfo.m_lIpcKey = m_RD->readIniLong(sFile,"MEMORY","mindata.ipc_key_value",0);
    m_RD->readIniString(sFile,"MEMORY","mindata.ipc_key_name",sKeyName,NULL);
    oSHMInfo.m_strIpcName = sKeyName;
    oSHMInfo.m_iTotal=m_RD->readIniLong(sFile,"MEMORY","mindata.max_shm_size",0);
    oSHMInfo.m_iIpcType=4;
    m_vSHMInfo.push_back(oSHMInfo);
    
    //imsidata
    oSHMInfo.m_lIpcKey = m_RD->readIniLong(sFile,"MEMORY","imsidata.ipc_key_value",0);
    m_RD->readIniString(sFile,"MEMORY","imsidata.ipc_key_name",sKeyName,NULL);
    oSHMInfo.m_strIpcName = sKeyName;
    oSHMInfo.m_iTotal=m_RD->readIniLong(sFile,"MEMORY","imsidata.max_shm_size",0);
    oSHMInfo.m_iIpcType=5;
    m_vSHMInfo.push_back(oSHMInfo);    
 

    //信号量

    oSEMInfo.m_lIpcKey = m_RD->readIniLong(sFile,"MEMORY","loggroupdata_thr.ipc_key_value",0);
    m_RD->readIniString(sFile,"MEMORY","param.sem_key_name",sKeyName,NULL);
    oSEMInfo.m_strIpcName = sKeyName;
    strcpy(oSEMInfo.m_sTableName,"system");
    m_vSEMInfo.push_back(oSEMInfo);    
    
    oSEMInfo.m_lIpcKey = m_RD->readIniLong(sFile,"MEMORY","loggroupinfolock.ipc_key_value",0);
    m_RD->readIniString(sFile,"MEMORY","loggroupinfolock.ipc_key_name",sKeyName,NULL);
    oSEMInfo.m_strIpcName = sKeyName;
    strcpy(oSEMInfo.m_sTableName,"log");    
    m_vSEMInfo.push_back(oSEMInfo);  
    
    //LRU
    LRUINFO oLRUInfo ;
    oLRUInfo.m_lIpcKey=m_RD->readIniLong(sFile,"MEMORY","testlru.ipc_key_value",0);  
    m_RD->readIniString(sFile,"MEMORY","testlru.ipc_key_name",sKeyName,NULL);
    oLRUInfo.m_strIpcName = sKeyName;  
    m_RD->readIniString(sFile,"MEMORY","testlru.ext_file_name",sKeyName,NULL);
    oLRUInfo.m_strIpcName = sKeyName;     
    oLRUInfo.m_iExtSize=m_RD->readIniInteger(sFile,"MEMORY","testlru.ext_data_size",0);  
    oLRUInfo.m_iExtNum=m_RD->readIniInteger(sFile,"MEMORY","testlru.max_extents",0); 
    oLRUInfo.m_iLRUType =1 ;          
    m_vLRUInfo.push_back(oLRUInfo);  
    
    delete m_RD;    
    return true;
}

int ShmMgr::getStatMem()
{
	vector<SHMINFO>::iterator it;   		
	int iRtn = 0;
	int ii=0;
	for (it = m_vSHMInfo.begin();it != m_vSHMInfo.end();it ++)
	{
		unsigned int iTotal	= 0;
		unsigned int iUsed	= 0;
		unsigned long lAddr = 0;
		unsigned long lDate = 0;
		long lSize = 0;
     long *pAddr= NULL;	
		if(it->m_lIpcKey==0)
		    continue;
		ii++;
		switch (it->m_iIpcType)
		{
		case 1: //SHMDATA<Y>
			{
				
				SHMData<CIniSection> sdata( it->m_lIpcKey );
				if( sdata.exist() )
				{
					iTotal     = sdata.getTotal();
					iUsed      = sdata.getCount();
					//lSize 	   = sdata.getRealSize()/(1024*0124);
					lSize      = it->m_iTotal*sizeof(CIniSection)/(1024*1024)	;				
					lDate 	   = sdata.getLoadTime();
					lAddr 	   = sdata.getMemAdress();

				}else{
					iTotal     = 0;
					iUsed      = 0;
					lSize 	   = 0;
					lDate 	   = 0;
					lAddr 	   = 0;					
				}					
			}
			break;
		case 2: //SHMDATA<Y>
			{
				
				SHMData_A<int> sdata( it->m_lIpcKey );
				if( sdata.exist() )
				{
					iTotal     = sdata.getTotal();
					iUsed      = sdata.getCount();
					//lSize 	   = sdata.getRealSize()/(1024*0124);
					lSize      = it->m_iTotal*sizeof(LogInfoData)/(1024*1024);
					lDate 	   = sdata.getLoadTime();
					lAddr 	   = sdata.getMemAdress();
					switch(it->m_iState){
						case INACTIVE:
							strcpy(it->m_sState,"可用");	
							break;
						case CURRENT:
							strcpy(it->m_sState,"在用");	
							break;
						case ACTIVE:
              strcpy(it->m_sState,"清仓");								
							break;
             default:
             	break;
            }																					
							
				}else{
					iTotal     = 0;
					iUsed      = 0;
					lSize 	   = 0;
					lDate 	   = 0;
					lAddr 	   = 0;					
				}		
			}
			break;
		case 3: //SHMDATA<Y>
			{
				
				SHMData<int> sdata( it->m_lIpcKey );
				if( sdata.exist() )
				{
					iTotal     = sdata.getTotal();
					iUsed      = sdata.getCount();
					//lSize 	   = sdata.getRealSize()/(1024*0124);
					lSize      = it->m_iTotal*sizeof(HCodeInfoStruct)/(1024*1024);
					lDate 	   = sdata.getLoadTime();
					lAddr 	   = sdata.getMemAdress();
				}else{
					iTotal     = 0;
					iUsed      = 0;
					lSize 	   = 0;
					lDate 	   = 0;
					lAddr 	   = 0;					
				}		
			}
			break;
		 case 4: //SHMDATA<Y>
			{
				
				SHMData<int> sdata( it->m_lIpcKey );
				if( sdata.exist() )
				{
					iTotal     = sdata.getTotal();
					iUsed      = sdata.getCount();
					//lSize 	   = sdata.getRealSize()/(1024*0124);
					lSize      = it->m_iTotal*sizeof(MinInfoStruct)/(1024*1024);
					lDate 	   = sdata.getLoadTime();
					lAddr 	   = sdata.getMemAdress();
				}else{
					iTotal     = 0;
					iUsed      = 0;
					lSize 	   = 0;
					lDate 	   = 0;
					lAddr 	   = 0;					
				}		
			}
			break;
		case 5: //SHMDATA<Y>
			{
				
				SHMData<int> sdata( it->m_lIpcKey );
				if( sdata.exist() )
				{
					iTotal     = sdata.getTotal();
					iUsed      = sdata.getCount();
					//lSize 	   = sdata.getRealSize()/(1024*0124);
					lSize      = it->m_iTotal*sizeof(IMSIInfoStruct)/(1024*1024);
					lDate 	   = sdata.getLoadTime();
					lAddr 	   = sdata.getMemAdress();
				}else{
					iTotal     = 0;
					iUsed      = 0;
					lSize 	   = 0;
					lDate 	   = 0;
					lAddr 	   = 0;					
				}		
			}
			break;			
		}		
		it->m_iTotal = iTotal;
		it->m_iUsed = iUsed;
		it->m_lMemAddr = lAddr;
		it->m_lSize = lSize;
		iTotal=it->m_iTotal ;
		float fRes = 0;
		if(iTotal)
			fRes = (float)(iUsed*100.000/iTotal);
		it->m_fUsedRate = (float)fRes;
		it->m_fUnUsed = (float)(100.000-fRes);	
		if(	it->m_fUsedRate>100.000)
			{
				it->m_fUsedRate=100.000 ;
				it->m_fUnUsed=0.000;
			}
		if(it->m_iIpcType!=2){
	    if(	iUsed==0) strcpy(it->m_sState,"可用");	
	    else strcpy(it->m_sState,"在用");
	  }
	}
	
	return m_vSHMInfo.size();
}	

int ShmMgr::getStatMem(vector<SHMINFO> &vRet)
{
	vector<SHMINFO>::iterator it;   		
	int iRtn = 0;
	int ii=0;
	for (it = m_vSHMInfo.begin();it != m_vSHMInfo.end();it ++)
	{
		unsigned int iTotal	= 0;
		unsigned int iUsed	= 0;
		unsigned long lAddr = 0;
		unsigned long lDate = 0;
		long lSize = 0;
     long *pAddr= NULL;	
		if(it->m_lIpcKey==0)
		    continue;
		ii++;
		switch (it->m_iIpcType)
		{
		case 1: //SHMDATA<Y>
			{
				
				SHMData<int> sdata( it->m_lIpcKey );
				if( sdata.exist() )
				{
					iTotal     = sdata.getTotal();
					iUsed      = sdata.getCount();
					lSize 	   = sdata.getRealSize();
					lDate 	   = sdata.getLoadTime();
					lAddr 	   = sdata.getMemAdress();

				}else{
					iTotal     = 0;
					iUsed      = 0;
					lSize 	   = 0;
					lDate 	   = 0;
					lAddr 	   = 0;					
				}					
			}
			break;
		case 2: //SHMDATA<Y>
			{
				
				SHMData_A<int> sdata( it->m_lIpcKey );
				if( sdata.exist() )
				{
					iTotal     = sdata.getTotal();
					iUsed      = sdata.getCount();
					lSize 	   = sdata.getRealSize();
					lDate 	   = sdata.getLoadTime();
					lAddr 	   = sdata.getMemAdress();
				}else{
					iTotal     = 0;
					iUsed      = 0;
					lSize 	   = 0;
					lDate 	   = 0;
					lAddr 	   = 0;					
				}		
			}
			break;
		}		
		it->m_iTotal = iTotal;
		it->m_iUsed = iUsed;
		it->m_lMemAddr = lAddr;
		it->m_lSize = lSize;
		float fRes = 0;
		if(iTotal)
			fRes = (float)(iUsed*100/iTotal);
		it->m_fUsedRate = (float)fRes;
		it->m_fUnUsed = (float)(100-fRes);		
		
		vRet.push_back(*it);
	}
	
	return m_vSHMInfo.size();
}	

int ShmMgr::getStatSem()
{
	vector<SEMINFO>::iterator it;   		
	int iRtn = 0;
	int ii=0;
	CSemaphore *poInfoDataLock = new CSemaphore();
	char sTemp[32]={0};

	for (it = m_vSEMInfo.begin();it != m_vSEMInfo.end();it ++)
	{
		memset(sTemp,0,sizeof(sTemp));
		sprintf (sTemp, "%ld", it->m_lIpcKey);
    poInfoDataLock->getSem (sTemp, 1, 1);
    if(poInfoDataLock->P(false))
    	{
    		poInfoDataLock->V();
    		it->m_iState=0;
    	}
    else{
    	  it->m_iState=1;
    	}
	}
	delete poInfoDataLock ;
	return m_vSEMInfo.size();
}	


int ShmMgr::showLRUMem()
{
	vector<LRUINFO>::iterator it;   		
	int iRtn = 0;
	int ii=0;
	for (it = m_vLRUInfo.begin();it != m_vLRUInfo.end();it ++)
	{

		switch (it->m_iLRUType)
		{
		  case 1: //SHMDATA<Y>
		  {
				
       SHMLruData<LruAggrData> *  pLruAggr 
         = new SHMLruData<LruAggrData>(it->m_lIpcKey, 
          it->m_strFileName.c_str() , 1,
          it->m_iExtSize, it->m_iExtNum) ;
          
       if(!pLruAggr->exist() ){
          printf("内存没有准备好\n" );
          return 0;
       }
       pLruAggr->showDetail();
       if(pLruAggr)
			  delete pLruAggr ;	
		  }
			break;		
		}	

	}	
	return m_vSHMInfo.size();
}	

void ShmMgr::showTTMem()
{
    char buffer[512];
    char *pLastIndx;
    FILE *fp ;
    char *tp;
    try {
	    if((fp = popen("ttStatus" ,"r"))!=NULL){    
          memset(buffer,0,512);
          bool bFlag=true;
	      while(fgets(buffer,512,fp)) {
	          tp=buffer;
	          if(strstr(buffer,"accu5")&&bFlag) {
	            printf("------------------------------------------------------------------------\n");
	            printf("%s",tp);
	            bFlag=false;
	            continue;
	          }
	          if(!bFlag) {
	            printf("%s",tp);
	          }
	          if(strstr(buffer,"---")&&(!bFlag)) {
	            break;
	          } 
	      }
	      pclose(fp);
 	    }
 	    else  {
 	        printf("获取TT内存信息异常");
 	    	//sprintf(list_comm[n],"%s","命令执行错误！") ;
 	    }
    } catch (...) {
        
    }    
}


void ShmMgr::showStatMem()
{
	 if(getStatMem())
	 {
	 	int iPos = 0;
    printf("%-50s%-12s%-10s%-10s%-10s%-5s\n",
           "IPC_NAME","IPC_KEY","大小(M)","已用百分比"," 空闲百分比","状态");
	 	vector<SHMINFO>::iterator it;
	 	int icount=0;
	 	char jj;
	 	for (it = m_vSHMInfo.begin();it != m_vSHMInfo.end();it ++)
		{ 

			printf("%-50s%-12ld%-10ld%-10.3f%-10.3f%-5s\n",
			     it->m_strIpcName.c_str(),it->m_lIpcKey,
			     it->m_lSize,it->m_fUsedRate,it->m_fUnUsed,it->m_sState);
			icount++;
		  if((icount+1)%10==0&&jj!='a'){
   	  cout<<"ALL/BREAK/CONTINUE[a/b/c]:";
   	  cin>>jj;
   	  if(jj=='a'||jj=='c'){ continue;}
   	  else if(jj=='b'){ return ;}
   	  return ; 			 
		}
	  }	
	 }
	 return  ;
}

void ShmMgr::showLogMem()
{
	 vector<LogInfoData> vRet; 
	 ThreeLogGroup Logg;
   Logg.GetAllLoggInfo(vRet);
   vector<LogInfoData>::iterator it; 
   int icount=0;
   char jj;
	 printf("%-16s%-10s%-16s%-8s%-8s%-6s%-6s%-6s%-6s%-10s%-10s%-6s%6s\n",
	 	         "时间", "编码" ,"编码名称" ,"PRO_ID" ,"APP_ID" ,
	 	         "级别", "类别" , "分类" , "工号" , "操作类型" , "操作字段" , "本地网标识",  "内容");	   	 
   for (it = vRet.begin();it != vRet.end();it ++)
	 { 
      printf("%-16s%-10d%-16s%-8d%-8d%-6d%-6d%-6d%-8d%-10d%-10s%-6s%s\n", 
             it->m_sLogTime,it->m_illCode,it->m_sCodeName,
             it->m_illProId,it->m_illAppId,
             it->m_illLevelId,it->m_illClassId,it->m_illTypeId,
             it->m_illEmployee,it->m_illActionId,
             it->m_sActionName,it->m_sAreaCode,it->m_sInfo);
			icount++;
		  if((icount+1)%10==0&&jj!='a'){
   	  cout<<"ALL/BREAK/CONTINUE[a/b/c]:";
   	  cin>>jj;
   	  if(jj=='a'||jj=='c'){ continue;}
   	  else if(jj=='b'){ return ;}
   	  return ; 	                        
	   }
	 }
}	

void ShmMgr::showTaleMem(char * sTableName ,char *sCondtion)
{
	if (connectTT() != 0) {
	    printf("connect shm failed!\n");
	    return;
	}    
	TIMESTAMP_STRUCT oEffTm, oExpTm;
	ThreeLogGroup Logg;
	try{
	   char sCond[128]={0};
	   char *pbegin=sCondtion;
	   char *p=0;		
		if(sCondtion !=0)
		{

	     p=strchr(pbegin,'=');
	     if(!p) 
	     	{ 
	     	  printf("输入的参数不准确，请检查并重新输入");
	     	  Logg.log(MBC_PARAM_THREAD_VALUE_ERROR,LOG_LEVEL_FATAL,
                LOG_TYPE_PROC,"应用进程参数不正确（错误的取值）[%s]!",sCondtion);
        }
        strncpy(sCond,pbegin,p-pbegin);	
        p++;  
      }  
	    if(strcmp(sTableName,"HSS_HCODE_INFO")==0 || strcmp(sTableName,"hss_hcode_info")==0 )
	    {
	        HCodeInfoStruct oData ;
	        char sSql[2048]={0};
	        char sSql2[2048]={0};
	        sprintf(sSql,"SELECT HEAD, TSP_ID, ZONE_CODE, EFF_DATE, EXP_DATE, HEAD_ID FROM HSS_HCODE_INFO ");
	        if(sCondtion !=0)
	        {     	
	        	sprintf(sSql2,"%s where %s='%s' ",sSql,sCond,p);
	        	m_poInfo->Prepare(sSql2);
	        }else{

              
	            m_poInfo->Prepare(sSql);
	        }
            m_poInfo->Commit();
            m_poInfo->Execute();
          printf("%10s%10s%10s%10s%12s%12s\n",
                "HEAD", "TSP_ID", "ZONE_CODE", "EFF_DATE", "EXP_DATE", "HEAD_ID");
            while (!m_poInfo->FetchNext()) {
               m_poInfo->getColumn(1, oData.m_sHead);
               m_poInfo->getColumn(2, oData.m_sTspID);
               m_poInfo->getColumn(3, oData.m_sZoneCode);
               m_poInfo->getColumn(4, &oEffTm);
               m_poInfo->getColumn(5, &oExpTm);
               m_poInfo->getColumn(6, (int *)&oData.m_uiHeadID);
               snprintf(oData.m_sEffDate, sizeof(oData.m_sEffDate), "%04d%02d%02d", oEffTm.year, oEffTm.month, oEffTm.day);
               snprintf(oData.m_sExpDate, sizeof(oData.m_sExpDate), "%04d%02d%02d", oExpTm.year, oExpTm.month, oExpTm.day);	
	         printf("%10s%10s%10s%10d%12s%12s\n",oData.m_sHead,oData.m_sTspID,oData.m_sZoneCode,
	                  oData.m_uiHeadID,oData.m_sEffDate,oData.m_sExpDate);
	        }
	        m_poInfo->Close();        	
	    }
	    else if(strcmp(sTableName,"HSS_MIN_INFO")==0 || strcmp(sTableName,"hss_min_info")==0 )
	    {
	        MinInfoStruct oData;
	        char sSql[2048]={0};
	        char sSql2[2048]={0};
	        sprintf(sSql,"SELECT BEGIN_MIN, END_MIN, USER_FLAG, COUNTRY_ID, "
	                     "HOME_CARRIER_CODE, ZONE_CODE, "
                       "USER_TYPE, EFF_DATE, EXP_DATE, MIN_INFO_ID FROM HSS_MIN_INFO");
	        if(sCondtion !=0)
	        {    	
	        	sprintf(sSql2,"%s where %s='%s' ",sSql,sCond,p);
	        	m_poInfo->Prepare(sSql2);
	        }else{
	          m_poInfo->Prepare(sSql);
	        }
            m_poInfo->Commit();
            m_poInfo->Execute();
         printf("%14s%14s%10s%10s%12s%14s%12s%14s%14s\n" ,
                "BEGIN_MIN", "END_MIN", "USER_FLAG", "COUNTRY_ID", 
                "HOME_CARRIER_CODE", "ZONE_CODE", 
                "USER_TYPE", "EFF_DATE", "EXP_DATE", "MIN_INFO_ID" );        
     	    while (!m_poInfo->FetchNext()) {
                m_poInfo->getColumn(1, oData.m_sBeginMin);
                m_poInfo->getColumn(2, oData.m_sEndMin);
                m_poInfo->getColumn(3, &oData.m_iUserFlag);
                m_poInfo->getColumn(4, &oData.m_iCountryID);
                m_poInfo->getColumn(5, oData.m_sHomeCarrCode);
                m_poInfo->getColumn(6, oData.m_sZoneCode);
                m_poInfo->getColumn(7, &oData.m_iUserType);
                m_poInfo->getColumn(8, &oEffTm);
                m_poInfo->getColumn(9, &oExpTm);
                m_poInfo->getColumn(10, (int *)&oData.m_uiMinInfoID);
                snprintf(oData.m_sEffDate, sizeof(oData.m_sEffDate), "%04d%02d%02d", oEffTm.year, oEffTm.month, oEffTm.day);
                snprintf(oData.m_sExpDate, sizeof(oData.m_sExpDate), "%04d%02d%02d", oExpTm.year, oExpTm.month, oExpTm.day);
          printf("%14s%14s%10d%10d%12s%14s%12d%14s%14s\n",oData.m_sBeginMin,
                   oData.m_sEndMin, oData.m_iUserFlag,oData.m_iCountryID,
                   oData.m_sHomeCarrCode,oData.m_sZoneCode,oData.m_uiMinInfoID,
                   oData.m_sEffDate,oData.m_sExpDate);
            }    
	        m_poInfo->Close();    	
	    }
	    else if(strcmp(sTableName,"HSS_CTOG_IMSI_INFO")==0 || strcmp(sTableName,"hss_ctog_imsi_info")==0 )
	    {
	        IMSIInfoStruct oData;
	        char sSql[2048]={0};
	        char sSql2[2048]={0};
	        sprintf(sSql,"SELECT IMSI_BEGIN, IMSI_END, IMSI_TYPE, SPONSOR_CODE,"
	                      "ZONE_CODE, EFF_DATE, "
                        "EXP_DATE, ISMI_ID FROM HSS_CTOG_IMSI_INFO");
	        if(sCondtion !=0)
	        {    	
	        	sprintf(sSql2,"%s where %s='%s' ",sSql,sCond,p);
	        	m_poInfo->Prepare(sSql2);
	        }else{
	          m_poInfo->Prepare(sSql);
	        }
            m_poInfo->Commit();
            m_poInfo->Execute();
          printf("%14s%14s%10s%14s%12s%14s%14s%14s\n",
                 "IMSI_BEGIN", "IMSI_END", "IMSI_TYPE", 
                 "SPONSOR_CODE","ZONE_CODE","ISMI_ID" ,"EFF_DATE", "EXP_DATE" );
     	    while (!m_poInfo->FetchNext()) {
                m_poInfo->getColumn(1, oData.m_sBeginIMSI);
                m_poInfo->getColumn(2, oData.m_sEndIMSI);
                m_poInfo->getColumn(3, &oData.m_iIMSIType);
                m_poInfo->getColumn(4, oData.m_sSponsorCode);
                m_poInfo->getColumn(5, oData.m_sZoneCode);
                m_poInfo->getColumn(6, &oEffTm);
                m_poInfo->getColumn(7, &oExpTm);
                m_poInfo->getColumn(8, (int *)&oData.m_uiIMSIID);
                snprintf(oData.m_sEffDate, sizeof(oData.m_sEffDate), "%04d%02d%02d", oEffTm.year, oEffTm.month, oEffTm.day);
                snprintf(oData.m_sExpDate, sizeof(oData.m_sExpDate), "%04d%02d%02d", oExpTm.year, oExpTm.month, oExpTm.day);
            printf("%14s%14s%10d%14s%12s%14d%14s%14s\n",oData.m_sBeginIMSI,
                   oData.m_sEndIMSI, oData.m_iIMSIType,oData.m_sSponsorCode,
                   oData.m_sZoneCode,oData.m_uiIMSIID,oData.m_sEffDate,oData.m_sExpDate);
            }
	        m_poInfo->Close();    	
	    }
	    else
	    {
	      printf("-tablename的表名不正确，不是内存的表\n");
	      Logg.log(MBC_MEM_INTERFACE_ERROR,LOG_LEVEL_FATAL,
                LOG_TYPE_SYSTEM,"内存查表接口错误,表[%s]不存在!",sTableName);
       }
	
 }catch(TTStatus st) {
        m_poInfo->Close();
        printf("checkOutputBalance failed! err_msg=%s", st.err_msg);
	     	 Logg.log(MBC_PARAM_THREAD_VALUE_ERROR,LOG_LEVEL_FATAL,
                LOG_TYPE_PROC,"应用进程参数不正确（错误的取值）");            
 }
}	

void ShmMgr::showCountMem(char * sTableName ,char *sCondtion)
{
	connectTT();
	TIMESTAMP_STRUCT oEffTm, oExpTm;
	int iCount=0;
	ThreeLogGroup Logg;
	try{
	   char sCond[128]={0};
	   char *pbegin=sCondtion;
	   char *p=0;		
		if(sCondtion !=0)
		{

	     p=strchr(pbegin,'=');
	     if(!p) 
	     	{ 
	     	  printf("输入的参数不准确，请检查并重新输入");
	     	  Logg.log(MBC_PARAM_THREAD_VALUE_ERROR,LOG_LEVEL_FATAL,
                LOG_TYPE_PROC,"应用进程参数不正确（错误的取值）[%s]!",sCondtion);
        }
        strncpy(sCond,pbegin,p-pbegin);	
        p++;  
      }  		
	    if(strcmp(sTableName,"HSS_HCODE_INFO")==0 || strcmp(sTableName,"hss_hcode_info")==0 )
	    {
	        HCodeInfoStruct oData ;
	        char sSql[2048]={0};
	        char sSql2[2048]={0};
	        sprintf(sSql,"SELECT COUNT(*) FROM HSS_HCODE_INFO ");
	        if(sCondtion !=0)
	        {     	
	        	sprintf(sSql2,"%s where %s='%s' ",sSql,sCond,p);
	        	m_poInfo->Prepare(sSql2);
	        }else{
	            m_poInfo->Prepare(sSql);
	        }
            m_poInfo->Commit();
            m_poInfo->Execute();
           if (!m_poInfo->FetchNext()) {
            m_poInfo->getColumn(1, (int *)&iCount);  
            printf("The count num is %d\n",iCount);               
	        }
	        m_poInfo->Close();        	
	    }
	    else if(strcmp(sTableName,"HSS_MIN_INFO")==0 || strcmp(sTableName,"hss_min_info")==0 )
	    {
	        MinInfoStruct oData;
	        char sSql[2048]={0};
	        char sSql2[2048]={0};
	        sprintf(sSql,"SELECT COUNT(*) FROM HSS_MIN_INFO ");
	        if(sCondtion !=0)
	        {
	        	sprintf(sSql2,"%s where %s='%s' ",sSql,sCond,p);
	        	m_poInfo->Prepare(sSql2);
	        }else{
	          m_poInfo->Prepare(sSql);
	        }
            m_poInfo->Commit();
            m_poInfo->Execute();       
     	    if(!m_poInfo->FetchNext()) {
            m_poInfo->getColumn(1, (int *)&iCount);  
            printf("The count num is %d\n",iCount);
            }    
	        m_poInfo->Close();    	
	    }
	    else if(strcmp(sTableName,"HSS_CTOG_IMSI_INFO")==0 || strcmp(sTableName,"hss_ctog_imsi_info")==0 )
	    {
	        IMSIInfoStruct oData;
	        char sSql[2048]={0};
	        char sSql2[2048]={0};
	        sprintf(sSql,"SELECT COUNT(*) FROM HSS_CTOG_IMSI_INFO");
	        if(sCondtion !=0)
	        {
	        	sprintf(sSql2,"%s where %s='%s' ",sSql,sCond,p);
	        	m_poInfo->Prepare(sSql2);
	        }else{
	          m_poInfo->Prepare(sSql);
	        }
            m_poInfo->Commit();
            m_poInfo->Execute();
     	    if(!m_poInfo->FetchNext()) {
            m_poInfo->getColumn(1, (int *)&iCount);  
            printf("The count num is %d\n",iCount);
            }
	        m_poInfo->Close();    	
	    }
	    else
	    {
	      printf("-tablename的表名不正确，不是内存的表");
	      Logg.log(MBC_MEM_INTERFACE_ERROR,LOG_LEVEL_FATAL,
                LOG_TYPE_SYSTEM,"内存查表接口错误,表[%s]不存在!",sTableName);
       }	        
	
 }catch(TTStatus st) {
        m_poInfo->Close();
        printf("checkOutputBalance failed! err_msg=%s", st.err_msg);
	     	 Logg.log(MBC_PARAM_THREAD_VALUE_ERROR,LOG_LEVEL_FATAL,
                LOG_TYPE_PROC,"应用进程参数不正确（错误的取值）");        
 }
}		

void ShmMgr::showSemStat(char * sTableName )
{
		getStatSem() ;
	 
	 	int iPos = 0;
	 	char sState[16]={0};
    printf("%-50s%-12s%-5s\n",
           "IPC_NAME","IPC_KEY","状态");
	 	vector<SEMINFO>::iterator it;
	 	int icount=0;
	 	char jj;	 	
	 	for (it = m_vSEMInfo.begin();it != m_vSEMInfo.end();it ++)
		{ 
      if(sTableName==NULL || strcmp(it->m_sTableName,sTableName)==0 )
      {
      	memset(sState,0,sizeof(sState));
      	if(it->m_iState==0) strcpy(sState,"可用");
      	else strcpy(sState,"已锁");
			  printf("%-50s%-12ld%-5s\n",
			     it->m_strIpcName.c_str(),it->m_lIpcKey,sState );
			  icount++;
		    if((icount+1)%10==0&&jj!='a'){
   	      cout<<"ALL/BREAK/CONTINUE[a/b/c]:";
   	      cin>>jj;
   	      if(jj=='a'||jj=='c'){ continue;}
   	      else if(jj=='b'){ return ;}
   	      return ; 			 
		    }
		  }
	  }	
	
	return  ;
}	

void ShmMgr::showSemReset(char * sTableName )
{
  vector<SEMINFO>::iterator it;   		
	int iRtn = 0;
	int ii=0;
	CSemaphore *poInfoDataLock = new CSemaphore();
	char sTemp[32]={0};
  int iCount=0;
	for (it = m_vSEMInfo.begin();it != m_vSEMInfo.end();it ++)
	{
		if(sTableName==NULL || strcmp(it->m_sTableName,sTableName)==0  )
		{
		  memset(sTemp,0,sizeof(sTemp));
		  sprintf (sTemp, "%ld", it->m_lIpcKey);
      poInfoDataLock->getSem (sTemp, 1, 1);
      if(!poInfoDataLock->P(false))
    	  {
    		  poInfoDataLock->V();
    		  printf("%s锁复位\n",it->m_sTableName);
    		  iCount++;
    	  }else{
    	  	poInfoDataLock->V();
    	  	printf("%s锁无需复位\n",it->m_sTableName);
    	  }
    }
	}
	if(iCount==0)
		printf("所有锁都可用，无需复位！\n");
	delete poInfoDataLock ;	
}	


void ShmMgr::lockSemStat(char * sTableName )
{
  vector<SEMINFO>::iterator it;   		
	int iRtn = 0;
	int ii=0;
	CSemaphore *poInfoDataLock = new CSemaphore();
	char sTemp[32]={0};
  int iCount=0;
	for (it = m_vSEMInfo.begin();it != m_vSEMInfo.end();it ++)
	{
	 if(sTableName==0 || strcmp(it->m_sTableName,sTableName)==0  )
	 {
	    memset(sTemp,0,sizeof(sTemp));
	    sprintf (sTemp, "%ld", it->m_lIpcKey);
      poInfoDataLock->getSem (sTemp, 1, 1);
      if(poInfoDataLock->P(false))
    	  {
    		  printf("[%s]信号量上锁成功\n",it->m_sTableName);
    		  iCount++;
    	  }else{
    	  	printf("[%s]信号量正在使用，无法上锁\n",it->m_sTableName);
    	  }  
    	}  
	} 
	if(iCount==0)
		printf("所有锁信号量正在使用，无法上锁！\n");
	else
		printf("供锁住信号量%d个\n",iCount);
	delete poInfoDataLock ;	
}	

void ShmMgr::creatSem(char * sTableName )
{
  vector<SEMINFO>::iterator it;   		
	int iRtn = 0;
	int ii=0;
	ThreeLogGroup Logg;
	CSemaphore *poInfoDataLock = new CSemaphore();
	char sTemp[32]={0};
  int iCount=0;
	for (it = m_vSEMInfo.begin();it != m_vSEMInfo.end();it ++)
	{
	 if(sTableName==0 || strcmp(it->m_sTableName,sTableName)==0  )
	 {
	 	  if(it->m_lIpcKey<=0)
	 	  	{
	 	      Logg.log(MBC_SEMP_CREATE_FAIL,LOG_LEVEL_FATAL,
                LOG_TYPE_SYSTEM,"创建信号量失败，请检查key值[%s]!\n",sTemp); 
          printf("创建信号量失败，请检查key值[%ld]!\n",it->m_lIpcKey);
          continue ; 		
	 	  	}
	    memset(sTemp,0,sizeof(sTemp));
	    sprintf (sTemp, "%ld", it->m_lIpcKey);
	    
      poInfoDataLock->getSem (sTemp, 1, 1);
      printf("[%s]信号量创建成功!\n",sTemp);
      iCount++;
    }  
	} 
	if(iCount==0)
		printf("没有创建任何信号量,请检查你输入的TableName!\n");
	else
		printf("共创建信号量%d个\n",iCount);
	delete poInfoDataLock ;	
}

void ShmMgr::creatOne(char * sKey )
{
	ThreeLogGroup Logg;
	CSemaphore *poInfoDataLock = new CSemaphore();
	char sTemp[32]={0};
  int iCount=0;
  //ThreeLogGroup Logg;  
  long iKey=atol(sKey);
	 if(iKey<=0)
	 	{
	 	   Logg.log(MBC_SEMP_CREATE_FAIL,LOG_LEVEL_FATAL,
               LOG_TYPE_SYSTEM,"创建信号量失败，请检查key值[%s]!\n",sKey); 
      printf("创建信号量失败，请检查key值[%s]!\n",sKey);
      return  ; 		
	 	}
    poInfoDataLock->getSem (sKey, 1, 1);
    printf("[%s]信号量创建成功\n",sKey);

	 delete poInfoDataLock ;	
   return;
}

/*连接TTT数据库*/
int  ShmMgr::connectTT()
{
	static TimesTenConn* pTTConn = NULL;
	if (m_poInfo != NULL) {
	    REL_CURSOR(m_poInfo);
	    m_poInfo = NULL;
	}    
	char sConnTns[8] = "hss";
	ABMException oExp;
	
	try {
    	if (GET_CURSOR(m_poInfo, pTTConn, sConnTns, oExp) != 0) {
    	    oExp.printErr();
         	ADD_EXCEPT0(oExp, "connec to TT failed!");
            	return -1;
           }
        }
        catch(TTStatus st) {
            oExp.printErr();
              ADD_EXCEPT1(oExp, "connec to TT failed", st.err_msg);
            return -1;
        }
	return 0;
}
