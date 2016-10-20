#include "CHssParamMgrTask.h"
#include "BaseRuleMgr.h"
#include "Log.h"
//##ModelId=4BFE313C006E
//extern const char* g_parrayDBSection[7];
//extern const char* g_parrayDBParamName[7];
//extern int			m_giDBInfoCnt;
//extern DBInfo m_gDbInfo[7];

CHbParamMgrTask::CHbParamMgrTask()
{	
	m_pParamInfo=0;
	m_pParamInfoVector=0;
	m_pIniFileMgr=0;
	m_pRuleMgr=0;
	//m_pDBAccessIF=0;
	m_pMsgFactory = new CmdMsgFactory();
}


//##ModelId=4BFE313C00C8
CHbParamMgrTask::~CHbParamMgrTask()
{
	Clear();
}

void CHbParamMgrTask::Clear(void)
{
	if(m_pParamInfo!=0)
		delete m_pParamInfo;
	m_pParamInfo=0;
	if(0!= m_pParamInfoVector)
		delete[] m_pParamInfoVector;
	m_pParamInfoVector=0;
	if(m_pIniFileMgr!=0)
		delete m_pIniFileMgr;
	m_pIniFileMgr=0;
}

long CHbParamMgrTask::AddKey(stIniCompKey& oIniComKey)
{
	if(0==m_pIniFileMgr)
		m_pIniFileMgr =new CIniFileMgr();

	long lRet=INIT_OP_OK;
	//检查
	if(0==m_pRuleMgr)
		m_pRuleMgr = new BaseRuleMgr();
	char sKeyValue[256]={0};
	if(oIniComKey.sKeyValue[1][0]!=0)
		strcpy(sKeyValue,oIniComKey.sKeyValue[1]);
	else
		strcpy(sKeyValue,oIniComKey.sKeyValue[0]);
	if(oIniComKey.flag ==OP_DEL)
	{
		if(0==m_pRuleMgr->checkParamNeed(oIniComKey.sSectionName,oIniComKey.sKeyName))
		{
			Log::log(0,"%s","核心参数检查不符合规则");
			return ERROR_Param_Rule_Noncomp;
		}else
		{
			string strTmp;
			if(0<m_pRuleMgr->check(oIniComKey.sSectionName,oIniComKey.sKeyName,sKeyValue,oIniComKey.sEffDate,0,false))
			{
				Log::log(0,"%s","核心参数检查不符合规则");
				return ERROR_Param_Rule_Noncomp;
			}
		}
		}
	if((oIniComKey.flag == OP_NEW)||(oIniComKey.flag== OP_FIX))
	{
		string strTmp;
		stIniCompKey sTempComKey;
		bool bGet=false;
		if(m_pIniFileMgr->GetIniKey(oIniComKey.sSectionName,oIniComKey.sKeyName,sTempComKey))
		{
			bGet=true;
		}	
		if(0<m_pRuleMgr->check(oIniComKey.sSectionName,oIniComKey.sKeyName,sKeyValue,oIniComKey.sEffDate,sTempComKey.sKeyValue[0],false))
		{
			Log::log(0,"%s","核心参数检查不符合规则");
			return ERROR_Param_Rule_Noncomp;
		}
	}
	if((oIniComKey.sKeyValue[0][0]!=0))
	{
	//if(0== m_pDBAccessIF)
	//{
	//	m_pDBAccessIF = CDBAccessIF::GetInstance();
	//}
	//if(0!= m_pDBAccessIF->DealDBConnect(oIniComKey))
	//{
	//	Log::log(0,"%s", "数据库连接失败");
	//	lRet=ERROR_Param_Connection_Failed;
	//	return lRet;
	//}
	//if(0 !=m_pDBAccessIF->DealInDbData(oIniComKey))
	//{
	//	Log::log(0,"%s", "数据库连接失败");
	//	lRet=ERROR_Param_RefreshDB_Failed;
	//	return lRet;
	//}
	}
	if(m_pIniFileMgr->AddKey(oIniComKey))
		lRet = INIT_OP_OK;
	else
	{
		if(oIniComKey.flag=OP_DEL)
			lRet=ERROR_Param_RefreshMem_Failed;
		else	if(oIniComKey.flag=OP_NEW)
			lRet =ERROR_Param_RefreshMem_Failed;
		else if(oIniComKey.flag=OP_FIX)
			lRet =ERROR_Param_RefreshMem_Failed;
	}
	return lRet;
}

//##ModelId=4BFE332E0220
bool CHbParamMgrTask::getTaskResult(CmdMsg  *pMsg,long& lMsgID,variant& vtRet,int* piRetSize)
{
	vtRet=false;
	if((0== pMsg)||(0==piRetSize))
		return false;

	long lCmd = 	pMsg->GetMsgCmd();
	string strUrl = pMsg->GetMsgSrc();


	variant vt;
	pMsg->GetMsgContent(vt);

	if(0==m_pIniFileMgr)
		m_pIniFileMgr =new CIniFileMgr();
	else
	{
		m_pIniFileMgr->FlashIt();
	}


	string strSection,strParam,strValue;
	string strContent = vt.asStringEx();
	char sKey[256]={0};
	int iRetSize=0;
	Log::log(0,"%s,%d",strContent.c_str(),pMsg->GetMsgSessionID());
	switch(lCmd)
	{
	case MSG_PARAM_LOG_PATH:
		{
			m_strLogPath=strUrl;
			 m_pMsgFactory->SetMsgLogPath(strUrl);
			m_pMsgFactory->RefreshLogFileName();
		}break;
	case MSG_PARAM_SAVE_IT:
		{
			//服务端传来的核心参数文件
			m_strIniFileName=strUrl;
		}break;
	case MSG_NEW_VALUE_EFFECT:
		{
			//启用新的参数
			memset(m_pParamInfo,0,sizeof(SysParamHis));
			stIniCompKey oIniKey;
			memset((void*)&oIniKey,0,sizeof(oIniKey));
			if(m_pIniFileMgr->GetIniKeyByString(strContent,oIniKey))
			{
				 oIniKey.flag=OP_FIX;	
				for(int i=1; i<=m_pIniFileMgr->m_iUseNum&&i<MAX_PARAM_SIZE;i++)
				{
					if((m_pIniFileMgr->m_pIniKey[i].sEffDate[0]!=0)&&
						(strcasecmp(m_pIniFileMgr->m_pIniKey[i].sSectionName,oIniKey.sSectionName)==0)&&
						(strcasecmp(m_pIniFileMgr->m_pIniKey[i].sKeyName,oIniKey.sKeyName)==0))
					{
						if(m_pIniFileMgr->m_pIniKey[i].sKeyValue[1][0]!=0)
						{
							strcpy(m_pIniFileMgr->m_pIniKey[i].sKeyValue[2],m_pIniFileMgr->m_pIniKey[i].sKeyValue[0]);
							strcpy(m_pIniFileMgr->m_pIniKey[i].sKeyValue[0],m_pIniFileMgr->m_pIniKey[i].sKeyValue[1]);
							memset(m_pIniFileMgr->m_pIniKey[i].sEffDate,0,sizeof(m_pIniFileMgr->m_pIniKey[i].sEffDate));	
							//strcpy(m_pIniFileMgr->m_pIniKey[i].sEffDate,oIniKey.sEffDate);
							strcpy(oIniKey.sKeyValue[0],m_pIniFileMgr->m_pIniKey[i].sKeyValue[0]);
							strcpy(oIniKey.sKeyValue[1],m_pIniFileMgr->m_pIniKey[i].sKeyValue[1]);
							strcpy(oIniKey.sKeyValue[2],m_pIniFileMgr->m_pIniKey[i].sKeyValue[2]);
							
        //if(0== m_pDBAccessIF)
        //{
        //        m_pDBAccessIF = CDBAccessIF::GetInstance();
        //}
        //if(0!= m_pDBAccessIF->DealDBConnect(oIniKey))
        //{
        //        Log::log(0,"%s", "数据库连接失败");
        //         lMsgID=ERROR_Param_Connection_Failed;
        //       	break; 
        //}
        //if(0 !=m_pDBAccessIF->DealInDbData(oIniKey))
        //{
        //        Log::log(0,"%s", "数据库连接失败");
        //        lMsgID=ERROR_Param_RefreshDB_Failed;
        //  	break; 
        //}	
							if(m_pIniFileMgr->SaveKey(m_strIniFileName.c_str(),oIniKey))
							{
								//存盘成功
								lMsgID=MSG_OP_RETURN_OK;
								m_pMsgFactory->RefreshLogFileName();	
								m_pMsgFactory->SaveMsg(pMsg,oIniKey);
								break;
							}else
							{
								Log::log(0,"刷新文件失败文件名:%s",m_strIniFileName.c_str());
								lMsgID = MSG_OP_RETURN_ERROR;
								TransIniKeyToSysParamHis(*m_pParamInfo,oIniKey);
								vtRet = m_pParamInfo;//文件不存在;
								break;
							}
						}
					}
				}
			}
		}break;
		case MSG_STATE_CHANGE:
		{
			memset(m_pParamInfo,0,sizeof(SysParamHis));
                        stIniCompKey oIniKey;
                        while(splitParamInfo(strContent,strSection,strParam,strValue))
			{
				oIniKey.flag=OP_FIX;
				strncpy(m_pParamInfo->sSectionName,strSection.c_str(),MAX_SECTION_LEN_INIT>strSection.length()?strSection.length():MAX_SECTION_LEN_INIT-1);
				strncpy(m_pParamInfo->sParamName,strParam.c_str(),MAX_KEY_NAME_LEN>strParam.length()?strParam.length():MAX_KEY_NAME_LEN-1);
				strncpy(m_pParamInfo->sParamValue,strValue.c_str(),MAX_KEY_VALUE_LEN>strValue.length()?strValue.length():MAX_KEY_VALUE_LEN-1);
			}
			lMsgID=MSG_OP_RETURN_OK;
			m_pIniFileMgr->SaveKey(m_strIniFileName.c_str(),oIniKey);
			vtRet.clear();
			vtRet.change_type(variant::vt_long);
			vtRet=0;
		}break;
	case MSG_PARAM_S_TIMING:
		{
			//读取实时生效参数
			int i =0,j=0;
			memset(m_pParamInfoVector,0,sizeof(SysParamHis)*MAX_PARAM_SIZE);
			for(; i<=m_pIniFileMgr->m_iUseNum&&i<MAX_PARAM_SIZE;i++)
			{
				if(m_pIniFileMgr->m_pIniKey[i].sEffDate[0]!=0)
				{
					strncpy(m_pParamInfoVector[j].sSectionName,m_pIniFileMgr->m_pIniKey[i].sSectionName,
						sizeof(m_pParamInfoVector[j].sSectionName));
					strncpy(m_pParamInfoVector[j].sParamName,
						m_pIniFileMgr->m_pIniKey[i].sKeyName,sizeof(m_pParamInfoVector[i].sParamName)-1);
					if(strlen(m_pIniFileMgr->m_pIniKey[i].sKeyValue[1])!=0)
					{
						strncpy(m_pParamInfoVector[j].sParamOrValue,m_pIniFileMgr->m_pIniKey[i].sKeyValue[1],sizeof(m_pParamInfo->sParamOrValue)-1);
					}
					strcpy(m_pParamInfoVector[j].sDate,m_pIniFileMgr->m_pIniKey[i].sEffDate);
					strncpy(m_pParamInfoVector[j++].sParamValue,m_pIniFileMgr->m_pIniKey[i].sKeyValue[0],sizeof(m_pParamInfo->sParamValue)-1);
				}
			}
			vtRet.clear();
			if(j!=0)
			{
				vtRet.change_type(variant::vt_ptr);
				vtRet=m_pParamInfoVector;
				*piRetSize= sizeof(SysParamHis)*j;
			}else
			{
				Log::log(0,"%s","读取实时生效参数失败");
				vtRet.change_type(variant::vt_long);
				vtRet=(long)ERROR_Param_Not_Exist;
			}
		}break;
	case MSG_PARAM_S_D:
		{
			//删除
			memset(m_pParamInfo,0,sizeof(SysParamHis));
			stIniCompKey oIniKey;
			if(splitParamInfo(strContent,strSection,strParam,strValue))
			{
				memset((void*)&oIniKey,0,sizeof(oIniKey));
				oIniKey.flag=OP_DEL;
				strncpy(oIniKey.sSectionName,strSection.c_str(),MAX_SECTION_LEN_INIT>strSection.length()?strSection.length():MAX_SECTION_LEN_INIT-1);
				strncpy(oIniKey.sKeyName,strParam.c_str(),MAX_KEY_NAME_LEN>strParam.length()?strParam.length():MAX_KEY_NAME_LEN-1);
				strncpy(oIniKey.sKeyValue[0],strValue.c_str(),MAX_KEY_VALUE_LEN>strValue.length()?strValue.length():MAX_KEY_VALUE_LEN-1);
				
				vtRet.clear();
				vtRet = AddKey(oIniKey);
				if( vtRet.asLong()==INIT_OP_OK){
					if(m_pIniFileMgr->SaveKey(m_strIniFileName.c_str(),oIniKey))
					{
						//存盘成功
						lMsgID=MSG_OP_RETURN_OK;
					}else{
						Log::log(0,"刷新文件失败文件名:%s",m_strIniFileName.c_str());
						lMsgID = MSG_OP_RETURN_ERROR;
						vtRet = ERROR_Param_RefreshFile_Failed;//文件不存在;
					}
				}else{
					lMsgID = MSG_OP_RETURN_ERROR;
					TransIniKeyToSysParamHis(*m_pParamInfo,oIniKey);
					vtRet = m_pParamInfo;
				}
			}
		}break;
	case MSG_PARAM_S_C:
		{
			//创建
			memset(m_pParamInfo,0,sizeof(SysParamHis));
			stIniCompKey oIniKey;
			if(splitParamInfo(strContent,strSection,strParam,strValue))
			{
				memset((void*)&oIniKey,0,sizeof(oIniKey));
				oIniKey.flag=OP_NEW;
				strncpy(oIniKey.sSectionName,strSection.c_str(),MAX_SECTION_LEN_INIT>strSection.length()?strSection.length():MAX_SECTION_LEN_INIT-1);
				strncpy(oIniKey.sKeyName,strParam.c_str(),MAX_KEY_NAME_LEN>strParam.length()?strParam.length():MAX_KEY_NAME_LEN-1);
				strncpy(oIniKey.sKeyValue[0],strValue.c_str(),MAX_KEY_VALUE_LEN>strValue.length()?strValue.length():MAX_KEY_VALUE_LEN-1);
				
				vtRet.clear();
				vtRet.change_type(variant::vt_long);
				vtRet = AddKey(oIniKey);
				if( vtRet.asLong()==INIT_OP_OK){
					if(m_pIniFileMgr->SaveKey(m_strIniFileName.c_str(),oIniKey))
					{
						//存盘成功
						lMsgID=MSG_OP_RETURN_OK;
					}else{
						lMsgID = MSG_OP_RETURN_ERROR;
						vtRet = ERROR_Param_RefreshFile_Failed;//文件不存在;
					}
				}else{
					lMsgID = MSG_OP_RETURN_ERROR;
					TransIniKeyToSysParamHis(*m_pParamInfo,oIniKey);
					vtRet = m_pParamInfo;
				}
			}
		}break;
	case MSG_PARAM_ACTIVE_MOUDLE_S:
		{
			//重载文件
			vtRet.clear();
			vtRet.change_type(variant::vt_long);
			vtRet=ERROR_Param_RefreshFile_Failed;
			cout<<m_strMemKey<<endl;
			cout<<m_strMemLockKey<<endl;

			CIniFileMgr* pIniFileMgr = new CIniFileMgr();
			pIniFileMgr->LoadIniFromFile(m_strIniFileName.c_str());
			vector<stIniCompKey> vtKey;
			vtRet=INIT_OP_OK;
			unsigned int j=0;
			if(m_pIniFileMgr->Compare(pIniFileMgr,vtKey))
			{
				for(vector<stIniCompKey> ::iterator it = vtKey.begin();
					it!=vtKey.end();it++)
				{
					stIniCompKey oIniCompKey;
					vtRet = AddKey(*it);
					if(vtRet.asLong()!=INIT_OP_OK)
					{
						if(m_pIniFileMgr->GetIniKey((*it).sSectionName,(*it).sKeyName,oIniCompKey))
						{
							TransIniKeyToSysParamHis(m_pParamInfoVector[j++],oIniCompKey);	
							strcpy(m_pParamInfoVector[j-1].sParamOrValue,(*it).sKeyValue[0]);
							m_pIniFileMgr->SaveKey(m_strIniFileName.c_str(),oIniCompKey);
						}
					}
				}
				if(j!=0)
				{
					lMsgID = MSG_OP_RETURN_ERROR;
					vtRet.clear();
					vtRet.change_type(variant::vt_ptr);
					vtRet=m_pParamInfoVector;
					*piRetSize= sizeof(SysParamHis)*j;
				}else{
					lMsgID = MSG_OP_RETURN_OK;
					vtRet=INIT_OP_OK;
				}
			}
			delete pIniFileMgr;pIniFileMgr=0;
			//vtRet=INIT_OP_OK;
			break;
		}break;
	case MSG_PARAM_SHM_ATTACH:
		{
			//Attach 共享内存
			vtRet.clear();
			vtRet.change_type(variant::vt_long);
			*piRetSize=0;
			if(!strContent.empty())	
			{
				m_strMemLockKey = strUrl;
				m_strMemKey = strContent;
				strcpy(sKey,strContent.c_str());
				m_pIniFileMgr->GetDataFromShm(sKey)    ;
				/*
				if(m_pIniFileMgr->GetDataFromShm(sKey))
				{
					//增加初始化数据交互接口
					//if(0 ==m_pDBAccessIF)
					//	m_pDBAccessIF = CDBAccessIF::GetInstance();
					for(int i=0;i<m_giDBInfoCnt;i++)
					{
						char sTmpName[255]={0};
						sprintf(sTmpName,"%s.%s",g_parrayDBParamName[i],"username");
						char sTmpPass[255]={0};
						sprintf(sTmpPass,"%s.%s",g_parrayDBParamName[i],"password");
						char sTmpTns[255]={0};
						sprintf(sTmpTns,"%s.%s",g_parrayDBParamName[i],"db_server_name");

						stIniCompKey oCompKeyUserName,oCompKeyPassWord,oCompKeyTns;
						if(m_pIniFileMgr->GetIniKey(g_parrayDBSection[i],sTmpName,oCompKeyUserName))
						{
							if(m_pIniFileMgr->GetIniKey(g_parrayDBSection[i],sTmpPass,oCompKeyPassWord))
							{
								if(m_pIniFileMgr->GetIniKey(g_parrayDBSection[i],sTmpTns,oCompKeyTns))
								{
									strcpy(m_gDbInfo[i].sSection,g_parrayDBSection[i]);
									//m_pDBAccessIF->ChangeDBConnectInfo(g_parrayDBSection[i],oCompKeyUserName.sKeyValue[0],
										oCompKeyPassWord.sKeyValue[0],oCompKeyTns.sKeyValue[0]);							
								}
							}
						}
					}
					char sTemp[256]={0};
					decode(m_gDbInfo[0].sPassWord,sTemp);
				//m_pDBAccessIF->SetDefaultConnect(m_gDbInfo[0].sName,sTemp,m_gDbInfo[0].sTnsName);		
					vtRet=INIT_OP_OK;
				}
				else
					vtRet=INIT_ERROR_PARAM_MEM_KEY;
				*/
			}else
			{
				vtRet=INIT_ERROR_PARAM_MEM_KEY;
			}
		}break;
	case MSG_STATE_CHANGE_RETURN:
	case MSG_PARAM_CHANGE:
		{
			//修改参数
			memset(m_pParamInfo,0,sizeof(SysParamHis));
			stIniCompKey oIniKey;
			memset((void*)&oIniKey,0,sizeof(oIniKey));
			int iPos = strContent.find(',');
			if((iPos>0)&&(iPos<strContent.length()))
			{
				string strContent1,strContent2;
				strContent1.assign(strContent,0,iPos);
				strContent2.assign(strContent,iPos+1,strContent.length());
				if(m_pIniFileMgr->GetIniKeyByString(strContent1,oIniKey))
			{	
				oIniKey.flag=OP_FIX;
				vtRet.clear();
				vtRet.change_type(variant::vt_long);
				vtRet =AddKey(oIniKey);
				if( vtRet.asLong()==INIT_OP_OK){
					if(m_pIniFileMgr->SaveKey(m_strIniFileName.c_str(),oIniKey))
					{
						//存盘成功
						lMsgID=MSG_OP_RETURN_OK;
						m_pMsgFactory->RefreshLogFileName();
						m_pMsgFactory->SaveMsg(pMsg,oIniKey);
					}else{
						Log::log(0,"刷新文件失败文件名:%s",m_strIniFileName.c_str());
						lMsgID = MSG_OP_RETURN_ERROR;
						vtRet = ERROR_Param_RefreshFile_Failed;//文件不存在;
					}
				}else{
					Log::log(0,"%s","刷新内存失败");
					lMsgID = MSG_OP_RETURN_ERROR;
					TransIniKeyToSysParamHis(*m_pParamInfo,oIniKey);
					vtRet = m_pParamInfo;
				}
			}
			if(m_pIniFileMgr->GetIniKeyByString(strContent2,oIniKey))
			{	
				oIniKey.flag=OP_FIX;
				vtRet.clear();
				vtRet.change_type(variant::vt_long);
				vtRet =AddKey(oIniKey);
				if( vtRet.asLong()==INIT_OP_OK){
					if(m_pIniFileMgr->SaveKey(m_strIniFileName.c_str(),oIniKey))
					{
						//存盘成功
						lMsgID=MSG_OP_RETURN_OK;
						m_pMsgFactory->RefreshLogFileName();
						m_pMsgFactory->SaveMsg(pMsg,oIniKey);
					}else{
						Log::log(0,"刷新文件失败文件名:%s",m_strIniFileName.c_str());
						lMsgID = MSG_OP_RETURN_ERROR;
						vtRet = ERROR_Param_RefreshFile_Failed;//文件不存在;
					}
				}else{
					Log::log(0,"%s","刷新内存失败");
					lMsgID = MSG_OP_RETURN_ERROR;
					TransIniKeyToSysParamHis(*m_pParamInfo,oIniKey);
					vtRet = m_pParamInfo;
				}
			}
			}
			if(m_pIniFileMgr->GetIniKeyByString(strContent,oIniKey))
			{	
				oIniKey.flag=OP_FIX;
				vtRet.clear();
				vtRet.change_type(variant::vt_long);
				vtRet =AddKey(oIniKey);
				if( vtRet.asLong()==INIT_OP_OK){
					if(m_pIniFileMgr->SaveKey(m_strIniFileName.c_str(),oIniKey))
					{
						//存盘成功
						lMsgID=MSG_OP_RETURN_OK;
						m_pMsgFactory->RefreshLogFileName();
						m_pMsgFactory->SaveMsg(pMsg,oIniKey);
					}else{
						Log::log(0,"刷新文件失败文件名:%s",m_strIniFileName.c_str());
						lMsgID = MSG_OP_RETURN_ERROR;
						vtRet = ERROR_Param_RefreshFile_Failed;//文件不存在;
					}
				}else{
					Log::log(0,"%s","刷新内存失败");
					lMsgID = MSG_OP_RETURN_ERROR;
					TransIniKeyToSysParamHis(*m_pParamInfo,oIniKey);
					vtRet = m_pParamInfo;
				}
			}
		}break;
	case MSG_PARAM_S_ALL:
		{
			memset(m_pParamInfoVector,0,sizeof(SysParamHis)*MAX_PARAM_SIZE);
			//读取整个Section
			unsigned int i=1;
			unsigned int j=0;
			for(; i<=m_pIniFileMgr->m_iUseNum&&i<MAX_PARAM_SIZE;i++)
			{
					strncpy(m_pParamInfoVector[j].sSectionName,m_pIniFileMgr->m_pIniKey[i].sSectionName,
						sizeof(m_pParamInfoVector[j].sSectionName));
					strncpy(m_pParamInfoVector[j].sParamName,
						m_pIniFileMgr->m_pIniKey[i].sKeyName,sizeof(m_pParamInfoVector[i].sParamName)-1);
					if(strlen(m_pIniFileMgr->m_pIniKey[i].sKeyValue[1])!=0)
					{	
					strncpy(m_pParamInfoVector[j].sParamOrValue,
					m_pIniFileMgr->m_pIniKey[i].sKeyValue[1],sizeof(m_pParamInfo->sParamOrValue)-1);
					}else
						strncpy(m_pParamInfoVector[j].sParamOrValue,m_pIniFileMgr->m_pIniKey[i].sKeyValue[0],sizeof(m_pParamInfo->sParamOrValue)-1);
					strncpy(m_pParamInfoVector[j++].sParamValue,
					m_pIniFileMgr->m_pIniKey[i].sKeyValue[0],sizeof(m_pParamInfo->sParamValue)-1);
			}
			vtRet.clear();
			if(j!=0)
			{
				vtRet.change_type(variant::vt_ptr);
				vtRet=m_pParamInfoVector;
				*piRetSize= sizeof(SysParamHis)*j;
			}else
			{
				Log::log(0,"%s","核心参数没有找到");
				vtRet.change_type(variant::vt_long);
				vtRet=INIT_ERROR_NO_PARAM;
			}
			//*piRetSize= sizeof(SysParamHis)*1;
		
		}break;
	case MSG_PARAM_S_N:
		{
			string strKeyName;
			string strKeyValue;
			memset(m_pParamInfoVector,0,sizeof(SysParamHis)*MAX_PARAM_SIZE);
			if(splitParamInfo(strContent,strSection,strParam,strValue))
			{
				if(strParam.empty())
				{
					//读取整个Section
					unsigned int i=1;
					unsigned int j=0;
					for(; i<=m_pIniFileMgr->m_iUseNum&&i<MAX_PARAM_SIZE;i++)
					{
						if(strcmp(m_pIniFileMgr->m_pIniKey[i].sSectionName,strSection.c_str())==0)
						{
							strncpy(m_pParamInfoVector[j].sSectionName,m_pIniFileMgr->m_pIniKey[i].sSectionName,
									sizeof(m_pParamInfoVector[j].sSectionName));
							strncpy(m_pParamInfoVector[j].sParamName,
								m_pIniFileMgr->m_pIniKey[i].sKeyName,sizeof(m_pParamInfoVector[i].sParamName)-1);
							if(strlen(m_pIniFileMgr->m_pIniKey[i].sKeyValue[2])!=0)
							{
								strncpy(m_pParamInfoVector[j].sParamOrValue,m_pIniFileMgr->m_pIniKey[i].sKeyValue[2],sizeof(m_pParamInfo->sParamOrValue)-1);
							}else
							{	
								strncpy(m_pParamInfoVector[j].sParamOrValue,m_pIniFileMgr->m_pIniKey[i].sKeyValue[0],sizeof(m_pParamInfo->sParamOrValue)-1);
							}
							if(m_pIniFileMgr->m_pIniKey[i].sEffDate[0]!=0)
							{
								strncpy(m_pParamInfoVector[j].sDate,m_pIniFileMgr->m_pIniKey[i].sEffDate,sizeof(m_pParamInfo->sDate)-1);
							}
							strncpy(m_pParamInfoVector[j++].sParamValue,m_pIniFileMgr->m_pIniKey[i].sKeyValue[0],sizeof(m_pParamInfo->sParamValue)-1);
						}
					}
					vtRet.clear();
					if(j!=0)
					{
						vtRet.change_type(variant::vt_ptr);
						vtRet=m_pParamInfoVector;
						*piRetSize= sizeof(SysParamHis)*j;
					}else
					{
						Log::log(0,"%s","核心参数没有找到");
						vtRet.change_type(variant::vt_long);
						vtRet=(long)ERROR_Param_Not_Exist;
					}
				}else
				{
					if(strValue.empty())
					{
						//读取单个值
						int iLen=0;
						bool bGet=false;
						for(unsigned int i=1; i<=m_pIniFileMgr->m_iUseNum&&i<MAX_PARAM_SIZE;i++)
						{
							if((strcmp(m_pIniFileMgr->m_pIniKey[i].sSectionName,strSection.c_str())==0)
								&&(strcmp(m_pIniFileMgr->m_pIniKey[i].sKeyName,strParam.c_str())==0))
							{
								bGet=true;
								strncpy(m_pParamInfo->sSectionName,m_pIniFileMgr->m_pIniKey[i].sSectionName,
										sizeof(m_pParamInfo->sSectionName)-1);
								strncpy(m_pParamInfo->sParamName,
										m_pIniFileMgr->m_pIniKey[i].sKeyName,
										sizeof(m_pParamInfo->sParamName)-1);
								if(strlen(m_pIniFileMgr->m_pIniKey[i].sKeyValue[2])!=0)
								{
									strncpy(m_pParamInfo->sParamOrValue,m_pIniFileMgr->m_pIniKey[i].sKeyValue[2],sizeof(m_pParamInfo->sParamOrValue)-1);
								}else
								{
									strncpy(m_pParamInfo->sParamOrValue,m_pIniFileMgr->m_pIniKey[i].sKeyValue[0],sizeof(m_pParamInfo->sParamOrValue)-1);
								}
								if(m_pIniFileMgr->m_pIniKey[i].sEffDate[0]!=0)
							{
								strncpy(m_pParamInfo->sDate,m_pIniFileMgr->m_pIniKey[i].sEffDate,sizeof(m_pParamInfo->sDate)-1);
								strncpy(m_pParamInfo->sParamOrValue,m_pIniFileMgr->m_pIniKey[i].sKeyValue[0],sizeof(m_pParamInfo->sParamOrValue)-1);
								strncpy(m_pParamInfo->sParamValue,m_pIniFileMgr->m_pIniKey[i].sKeyValue[1],sizeof(m_pParamInfo->sParamValue)-1);	
							}else {
								strncpy(m_pParamInfo->sParamValue,m_pIniFileMgr->m_pIniKey[i].sKeyValue[0],sizeof(m_pParamInfo->sParamValue)-1);
								}
								break;
							}
						}
						vtRet.clear();
						if(bGet)
						{
							vtRet.change_type(variant::vt_ptr);
							vtRet=m_pParamInfo;
							*piRetSize = sizeof(SysParamHis);
						}else
						{
							Log::log(0,"%s","核心参数没有找到");
							vtRet.change_type(variant::vt_long);
							vtRet=(long)ERROR_Param_Not_Exist;
							*piRetSize=0;	
							return false;
						}
					} 
				}
			}
			break;
		}
	default:
		{
			//ERROR_ID INIT_ERROR_UNRECOGNIZED
		Log::log(0,"无法识别的命令，或者目前尚不支持的命令,Cmd:%ld",lCmd);
			return false;
		}break;
	}
	return true;
}

//##ModelId=4BFE33360082
bool CHbParamMgrTask::Init()
{
	//访问共享内存配置;
	/*m_strIniFileName.assign(InDependProcess::m_sTIBS_HOME);
	m_strIniFileName.append("/etc/SysParam.ini");*/
	m_pParamInfo = new SysParamHis();
	m_pParamInfoVector = new SysParamHis[MAX_PARAM_SIZE];
	//memset(m_pParamInfo,0,sizeof(SysParamHis));
	//memset(m_pParamInfoVector,0,sizeof(SysParamHis)*MAX_PARAM_SIZE);
	return CHbTaskBase::Init();
}


void CHbParamMgrTask::OnExit(void)
{
	Clear();
	return ;
}

