/* 电力远程抄表文件接口
   xianggs 2010-11-01
   实现业务：1、电力批量查询用户信息(文件名称：STATEGRID01_xxxxxxxx)
             2、电力批量查询用户本月流量(文件名称：STATEGRID02_xxxxxxxx)
             3、电力批量查询用户上月流量(文件名称：STATEGRID03_xxxxxxxx)

*/
#include "PowerFile.h"

using namespace std;

//主要处理流程
int PowerFile::PowerMainProcess(int m_iOperation)
{
	vector<string> v_Phone;
	v_Phone.clear();
	ReadFile(v_Phone);
	
	char sContent[1024];char sRet[10];
	char sSumAmount[18]; char sSendAmount[18];char sRecvAmount[18];
	int  iCnt = 0;
	
	//提供根据卡号码业务状态
	if (m_iOperation == 1) {
	  FILE  * fp;
	  char sWholeFileName[256];
	  sprintf(sWholeFileName,"%sRESP_%s",m_lFileDirResp,file->getFileName());
	  if ( (fp = fopen(sWholeFileName,"r+"))== NULL){
	    if( (fp = fopen(sWholeFileName,"wb") ) == NULL){
	      Log::log(0,"open file error<%s>",sWholeFileName);
	      return -1;
	    }
	  }
	  for(vector<string>::iterator iter=v_Phone.begin();iter !=v_Phone.end();iter++) {
	    char m_sState[256];
	    char sBusiName[256];
	    char sPackName[256];
	    memset(m_sState,0,sizeof(m_sState));
	    int iRet = m_pQuery->queryState((*iter).c_str(),sBusiName,sPackName,m_sState,pRespMsg);
	    //sprintf(sRet,"%d",iRet);
	    if ( iRet == 0 )//电力以1 成功 0失败
	       strcpy(sRet,"1");
	    else   
	       strcpy(sRet,"0");
	    memset(sContent,0,sizeof(sContent));
//手机号码|反馈结果|业务名称|套餐名称|号码状态
			sprintf("%s|%s|%s|%s|%s\n",(*iter).c_str(),sRet,sBusiName,sPackName,m_sState);	    
	    fprintf(fp,"%s",sContent);
	    
	    iCnt++;
	    if(iCnt%1000==0)
	      printf("完成卡号码业务状态查询 %d 条.\n",iCnt);
	  }
	  fclose(fp);
	  m_pQuery->insertFileList(file->getFileName(),iCnt,m_iOperation,sWholeFileName);	
	  Log::log(0,"完成卡号码业务状态查询 %d 条.\n",iCnt);
	  Log::log(0,"文件<%s>中的卡号码业务状态查询完毕,生成文件 %s.\n",m_sFileName,sWholeFileName);
	  /*将查询的号码写入数据库保存，预留*/
	}
	
	//提供根据卡号码当月实时流量当日实时流量 流量信息上行和下行分别统计
	else if (m_iOperation == 2) {
	  FILE  * fp;
	  char sWholeFileName[256];
	  sprintf(sWholeFileName,"%sRESP_%s",m_lFileDirResp,file->getFileName());
	  if ( (fp = fopen(sWholeFileName,"r+"))== NULL){
	    if( (fp = fopen(sWholeFileName,"wb") ) == NULL){
	      Log::log(0,"open file error<%s>",sWholeFileName);
	      return -1;
	    }
	  }
	  for(vector<string>::iterator iter=v_Phone.begin();iter !=v_Phone.end();iter++) {
	    vector<GprsAmount> v_Vec;
	    int iRet = m_pQuery->queryGprs((*iter).c_str(),v_Vec,pRespMsg);
	    //sprintf(sRet,"%d",iRet);
	    if ( iRet == 0 )//电力以1 成功 0失败
	       strcpy(sRet,"1");
	    else   
	       strcpy(sRet,"0");
	    
	    
	    //原协议传送只有一个号码，0928修改协议，增加EndDate字段，为方便，在此拆分
	    char m_sPhoneT[30];memset(m_sPhoneT,0,sizeof(m_sPhoneT));
	    for (int i = 0; i < strlen((*iter).c_str()) ; i++) {
	      if ((*iter)[i] == '|')  {
	      strncpy(m_sPhoneT,(*iter).c_str(),i);
	      m_sPhoneT[strlen(m_sPhoneT)] = '\0';
	      break;
	     }
	    }
	    
	    for (vector<GprsAmount>::iterator iter1=v_Vec.begin();iter1 !=v_Vec.end();iter1++) {
	      memset(sContent,0,sizeof(sContent));
//手机号码|反馈结果|日期|当月按日实时总流量信息|当月按日实时上行流量信息|当日实时下行流量信息	   
				sprintf(sContent,"%s|%s|%s|%ld|%ld%ld\n",m_sPhoneT
																								,sRet
																								,(*iter1).sStartDate
																								,(*iter1).SumAmount
																								,(*iter1).SendAmount
																								,(*iter1).RecvAmount
																								);
	      fprintf(fp,"%s",sContent);
	    }
	    
	    iCnt++;
	    if(iCnt%1000==0)
	      printf("完成卡流量信息查询 %d 条.\n",iCnt);
	  }
	  fclose(fp);
	  m_pQuery->insertFileList(file->getFileName(),iCnt,m_iOperation,sWholeFileName);	
	  Log::log(0,"完成卡流量信息查询 %d 条.\n",iCnt);
	  Log::log(0,"文件<%s>中的卡号码流量信息查询完毕,生成文件 %s.\n",m_sFileName,sWholeFileName);
	  /*将查询的号码写入数据库保存，预留*/
	}
	
	//提供根据行业卡号码，查询该号码提交时间所属月份的上月GPRS流量总量信息
	else if(m_iOperation == 3) {
	  FILE  * fp;
	  char sWholeFileName[256];
	  sprintf(sWholeFileName,"%sRESP_%s",m_lFileDirResp,file->getFileName());
	  if ( (fp = fopen(sWholeFileName,"r+"))== NULL){
	    if( (fp = fopen(sWholeFileName,"wb") ) == NULL){
	      Log::log(0,"open file error<%s>",sWholeFileName);
	      return -1;
	    }
	  }
	  for(vector<string>::iterator iter=v_Phone.begin();iter !=v_Phone.end();iter++) {
	    long SumAmount = 0;
	    int iRet = m_pQuery->queryLastGprs((*iter).c_str(),SumAmount,pRespMsg);
	    if ( iRet == 0 )//电力以1 成功 0失败
	       strcpy(sRet,"1");
	    else   
	       strcpy(sRet,"0");
//手机号码|反馈结果|上月总流量信息	    
	    memset(sContent,0,sizeof(sContent));
	    sprintf(sContent,"%s|%s|%ld\n",(*iter).c_str(),sRet,SumAmount);
	    fprintf(fp,"%s",sContent);
	    
	    iCnt++;
	    if(iCnt%1000==0)
	      printf("完成卡号码业上月GPRS流量总量信息查询 %d 条.\n",iCnt);
	  }
	  fclose(fp);
	  m_pQuery->insertFileList(file->getFileName(),iCnt,m_iOperation,sWholeFileName);	
	  Log::log(0,"完成卡号码业上月GPRS流量总量信息查询 %d 条.\n",iCnt);
	  Log::log(0,"文件<%s>中的卡号码业上月GPRS流量总量信息查询完毕,生成文件 %s.\n",m_sFileName,sWholeFileName);
	  /*将查询的号码写入数据库保存，预留*/
	}
	
	else {
	  printf("未知的交易流水");
	  return -1;
	}
	
	return 0;
	
}

int PowerFile::CheckFile( char* m_sFileName )
{
	int iTemp;
	///如果不在服务器上操作已经采集的文件.则先在本地判断是否已经采集过
	if ( m_pQuery->checkFileExist( m_sFileName ) )
	{
	    return -2;
	}
	
	/*行业卡号码业务状态查询接口 */
	if ( strncmp( &m_sFileName[0], "STATEGRID01", 11) == 0 ){
	    return 1;
	}
	
	/*行业卡号码流量信息查询接口*/
	else if ( strncmp( &m_sFileName[0], "STATEGRID02",11 ) == 0 )	{
	    return 2;
	}
	
	/*行业卡号码上月流量信息查询接口*/
	else if ( strncmp( &m_sFileName[0], "STATEGRID03",11 ) == 0 )	{
	    return 3;
	}

	/*文件名不可识别*/
	else {		
		return -1;
	}

	return 0;
}


int PowerFile::MainProcess()
{
//

	while (( file = oDir.nextFileRecursive() ) != NULL)
	{
	   if( file->getFileName() && ( iCheckState = CheckFile( file->getFileName() ) ))  {
	      if ( iCheckState == -2 )  {   ////重复文件
	          Log::log(0,"存在重复文件[%s],此文件重命名为[%s%s.err]",file->getFullFileName(),m_lFileDirRecvBak,
	                                                              file->getFileName() );
	          char sTmp[256];
	          sprintf(sTmp,"%s%s.err", m_lFileDirRecvBak,file->getFileName() );
	          file->moveFile(sTmp);
	          continue;
	      } 
	      else if ( iCheckState == -1  || iCheckState == 0) {
	      	   Log::log(0,"不能识别的文件名<%s>",file->getFileName());
	      	   //doWaring Send SMS;
	      }  
	      else {
	        memset(m_sFileName,0,sizeof(m_sFileName));
	        strcpy(m_sFileName,m_lFileDirRecv);
	        strcat(m_sFileName,file->getFileName());
	        
	        int nRet = PowerMainProcess(iCheckState);
	        if (nRet == 0 ) {
	          char sBackFile[256];
	          strcpy(sBackFile,m_lFileDirRecvBak);
	          strcat(sBackFile,file->getFileName());
	          file->moveFile(sBackFile);
	          Log::log(0,"文件<%s>查询完毕，备份成 %s.\n",file->getFileName(),sBackFile);
	        } else {
	          Log::log (0,"文件类查询出错，文件名称 %s \n",file->getFullFileName());
	        }
	      }
	   }
	}
	
	return 0;
}

int PowerFile::ReadFile(vector<string> &v_Phone)
{
	FILE *rFile; 
	int i = 0;
	
	if  (v_Phone.size() != 0)
	  v_Phone.clear();	
	char 	sInPhone[201];
	if ((rFile = fopen(m_sFileName, "r"))== NULL) 	{
	  return -1;
	}
	
	while(!feof(rFile))
	{       
	  fgets(sInPhone,201,rFile);
	  if (feof(rFile)) break;
	  Alltrim(sInPhone);
	  if (sInPhone[strlen(sInPhone)-1] == '\n')
	  	sInPhone[strlen(sInPhone)-1] = '\0';
	  v_Phone.push_back(sInPhone);
	}
}

void PowerFile::init()
{
	if (!ParamDefineMgr::getParam("POWER_MAIN","FILE_RECV",m_lFileDirRecv,128)) {
	  printf("b_param_define未中配置 电力抄表文件接收目录");
	  exit;
	}
	if (m_lFileDirRecv[strlen(m_lFileDirRecv)-1] != '/')
	  m_lFileDirRecv[strlen(m_lFileDirRecv)] = '/';
	oDir = Directory(m_lFileDirRecv);
	
	if( access(m_lFileDirRecv, F_OK) != 0 )	{
	  char sTemp[256] = "mkdir -p ";
	  strcat(sTemp, m_lFileDirRecv);
	  system(sTemp);
	}
	
	if (!ParamDefineMgr::getParam("POWER_MAIN","FILE_RECV_BAK",m_lFileDirRecvBak,128))	{
	  printf("b_param_define未中配置 电力抄表文件接收备份目录");
	  exit;
	}
	if (m_lFileDirRecvBak[strlen(m_lFileDirRecvBak)-1] != '/')
	  m_lFileDirRecvBak[strlen(m_lFileDirRecvBak)] = '/';

	if( access(m_lFileDirRecvBak, F_OK) != 0 )	{
	  char sTemp[256] = "mkdir -p ";
	  strcat(sTemp, m_lFileDirRecvBak);
	  system(sTemp);
	}

	if (!ParamDefineMgr::getParam("POWER_MAIN","FILE_RESP",m_lFileDirResp,128)) {
	  printf("b_param_define未中配置 电力抄表文件反馈文件接口目录");
	  exit;
	}
	if (m_lFileDirResp[strlen(m_lFileDirResp)-1] != '/')
	  m_lFileDirResp[strlen(m_lFileDirResp)] = '/';
	rDir = Directory(m_lFileDirResp);	
	
	if( access(m_lFileDirResp, F_OK) != 0 )	{
	  char sTemp[256] = "mkdir -p ";
	  strcat(sTemp, m_lFileDirResp);
	  system(sTemp);
	}
	
	if (!ParamDefineMgr::getParam("POWER_MAIN","FILE_RESP_BAK",m_lFileDirRespBak,128))	{
	  printf("b_param_define未中配置 POWER_MAIN TEMP_FILE_DIR");
	  exit;
	}
	if (m_lFileDirRespBak[strlen(m_lFileDirRespBak)-1] != '/')
	  m_lFileDirRespBak[strlen(m_lFileDirRespBak)] = '/';

	if( access(m_lFileDirRespBak, F_OK) != 0 )	{
	  char sTemp[256] = "mkdir -p ";
	  strcat(sTemp, m_lFileDirRespBak);
	  system(sTemp);
	}

	char sFile[254];char sTemp[254],sHome[256];
	char * p;
	if ((p=getenv ("TIBS_HOME")) == NULL)
	  sprintf (sHome, "/bill/rtbill/TIBS_HOME/");
	else
	  sprintf (sHome, "%s", p);

	if (sHome[strlen(sHome)-1] == '/') {
	  sprintf (sFile, "%setc/data_access_new.ini", sHome);
	} 
	else {
	  sprintf (sFile, "%s/etc/data_access_new.ini", sHome);
	}	
	ReadIni reader;	
	reader.readIniString (sFile, (char *)"POWER_FTP", (char *)"ip", m_sIP, (char *)"");
	reader.readIniString (sFile, (char *)"POWER_FTP", (char *)"username", m_sUser,(char *)"");
	reader.readIniString (sFile, (char *)"POWER_FTP", (char *)"password", sTemp, (char *)"");
	decode(sTemp,m_sPass);
	Alltrim(m_sPass);
	reader.readIniString (sFile, (char *)"POWER_FTP", (char *)"recv_path", m_rRecvPath, (char *)"");	
	reader.readIniString (sFile, (char *)"POWER_FTP", (char *)"recv_bak" , m_rRecvBak,(char *)"");
	reader.readIniString (sFile, (char *)"POWER_FTP", (char *)"resp_path", m_rRespPath,(char *)"");


	m_pQuery = new PowerQuery();
	
}

PowerFile::PowerFile()
{
	init();	
}

PowerFile::~PowerFile()
{
	if ( m_pQuery ) delete m_pQuery;
}

void PowerFile::sendFile()
{
	char sFile[124]={0};
	char sBakFile[256]={0};
	char sLocalFile[256]={0};
	char sRemoteFile[256]={0};
	int  iPct;
	File *pFile;
	int iDeal=0;
	CFTPClient ftpc;
	if ( ftpc.FTPLogin(m_sIP,m_sUser,m_sPass) != 0 ) 
		return;
	ftpc.FTPOption(DATATYPE,BINARY);
	while (( pFile = rDir.nextFileRecursive() ) != NULL)	{
		strcpy(sFile,pFile->getFileName());
		sprintf(sRemoteFile,"%s/%s",m_rRespPath,sFile);
		sprintf(sLocalFile,"%s%s",m_lFileDirResp,sFile);
		sprintf(sBakFile,"%s%s",m_lFileDirRespBak,sFile);
		ftpc.PutFile(sLocalFile,sRemoteFile,&iPct);
		pFile->moveFile(sBakFile);
		iDeal++;		
	}
	Log::log(0,"传送了 <%d> 个文件到电力FTP服务器!",iDeal);
}

void PowerFile::getFile()
{

	char sRet[2048]={0};
	char sFile[124]={0};
	char sBakFile[256]={0};
	char sLocalFile[256]={0};
	char sTempFile[256];
	int iDeal=0;
	CFTPClient ftpc;
	if ( ftpc.FTPLogin(m_sIP,m_sUser,m_sPass) != 0 ) 
		return;
	ftpc.FTPOption(DATATYPE,BINARY);
	ftpc.FTPCmd(CHDIR,m_rRecvPath);
	strcpy(sTempFile,"recv.txt");
	ftpc.FTPCmd(LS,NULL,sTempFile);
	listFile(sTempFile);

	for(vector<string>::iterator iter=m_listFile.begin();iter !=m_listFile.end();iter++) {
		strcpy(sFile,(*iter).c_str());
		sprintf(sLocalFile,"%s%s",m_lFileDirRecv,sFile);//本地文件名
		sprintf(sBakFile,"%s/%s",m_rRecvBak,sFile);//ftp服务器备份文件名
		//printf("file: <%s>\n",sFile);
		ftpc.GetFile(sFile,sLocalFile);//从FTP获取文件到本地
		ftpc.FTPCmd(RENAME,sFile,sBakFile);		//FTP文件备份
		iDeal++;
	}
	ftpc.FTPLogout();
	Log::log(0,"从电力FTP服务器上获取了 <%d> 个文件!",iDeal);
}

void PowerFile::listFile(char * sTempFile)
{
    struct stat statbuf;
    int         iFileSize = 0;
    int         iServerFileSize = 0;    /////服务器上文件的大小
    char        sTemp[500];
    
    m_listFile.clear();
    
    if ( stat( sTempFile, &statbuf ) == 0 )
    {
        iFileSize = statbuf.st_size;
    }

    ////////////////获取列表数据
    if ( iFileSize > 0 )
    {

        char*   sFileInfo = new char[iFileSize + 1];

        if ( sFileInfo == NULL )
        {
            return ;
        }

        FILE*   theFp = fopen( sTempFile, "rb" );
        if ( theFp == NULL || fread( sFileInfo, iFileSize, 1, theFp ) != 1 )
        {
            Log::log( 0, "读取文件列表失败！" );
            delete[] sFileInfo;
            return ;
        }

        if ( theFp )
        {
            fclose( theFp );
        }

        theFp = NULL;

        sFileInfo[iFileSize] = '\0';

        const char  sDiv[] = " ";                   ////域分隔符
        const int   iSizeOffCnt = 5;                /////文件大小是第5个域,从1开始计数,跟awk程序一致
        const int   iOff = sizeof( sDiv ) - 1;

        char*       pBegin = sFileInfo;             //////一行的开头
        char*       pEnd = sFileInfo;               //////一行的结尾
        char*       pSize = sFileInfo;              //////指向文件大小
        char*       pDate = sFileInfo;               //指向文件的时间
        char*       pName = sFileInfo;              //////指向 文件/目录 名字
        char*       pOver = &sFileInfo[iFileSize];  /////文件内容结束
        int         iCnt = 0;
        pEnd--;
        while ( pEnd < pOver )
        {
            pEnd++;
            pBegin = pEnd;                  //////上次的end给这次的begin
            for ( ; pEnd < pOver; pEnd++ )
            {
                if ( *pEnd == '\n' || *pEnd == '\0' || *pEnd == '\r' )
                {
                    break;
                }
            }

            /////找到一行结束了
            
                          ///////只关心一行的开始是d 或者 - 的
            if ( *pBegin != 'd' && *pBegin != '-' )
            {
                continue;

            }

            *pEnd = '\0';
            if ( *pBegin == '\0' )
            {
                continue;
            }

            ////找文件/目录名字
            for ( pName = pEnd - 1; pName > pBegin; pName-- )
            {
                if ( strstr( pName, sDiv ) != NULL )
                {
                    break;
                }
            }

            pName += iOff;

            /////找到名字域
            if ( *pBegin == 'd' )
            {                               /////是目录
                                /////以.开头的都不要
                if ( *pName != '.' )
                {
                    ;//目录就不管了，不作任何处理
                }
            }
            else if ( *pBegin == '-' )
            {                               /////是文件
                
                                ////找文件大小
                pSize = pBegin;

                char*   pTmp;
                for ( iCnt = 0; iCnt < iSizeOffCnt - 1; iCnt++ )
                {
                    pSize = strstr( pSize, sDiv );
                    if ( pSize == NULL )
                    {
                        break;
                    }

                    if ( iOff == 1 )
                    {                       ///////单字符重复属于一个分隔
                        while ( pSize < pEnd )
                        {                   ////
                            pSize++;
                            if ( ( *pSize ) != sDiv[0] )
                            {
                                break;
                            }
                        }
                    }
                }

                if ( pSize == NULL )
                {                           ////没有找到文件大小的那个域
                    continue;
                }

                if ( iOff != 1 )
                {
                    pSize += iOff;

                }

                ////找到文件大小的域
                iServerFileSize = atoi( pSize );
                if ( iServerFileSize < 0 )
                {
                    continue;
                }
                
                //## 取ftp文件创建时间                
                pDate = strstr(pSize, sDiv);
                if (pDate != NULL)
                {
                    pDate += iOff;
                    char sTempDate[13];
                    memset (sTempDate, 0, sizeof(sTempDate));
                    strncpy (sTempDate, pDate, 12);
                    sTempDate[12] = 0;
                    if (!CFTPClient::changeDateFormat(sTempDate, sTemp))
                    {
                        sTemp[0] = 0;
                    }
                }
                else
                    sTemp[0] = 0;
                                
//printf("%20s%20d%20s\n",pName,iServerFileSize,sTemp)        ;                        
                m_listFile.push_back( pName );

            }
        }

        delete[] sFileInfo;
    }
}

void PowerFile::setUserInfo(){
	m_pQuery->setHLRinfo();
}