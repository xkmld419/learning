#include "BaseRuleMgr.h"
#include "Exception.h"
#include "MBC.h"
#include "MBC_ABM.h"
#include  <stdio.h>
#include  <stdlib.h>
#include "MsgDefineServer.h"
#include "Log.h"
#include "Env.h"
//CIniFileMgr *BaseRuleMgr::gpCIniFileMgr = 0;
//BaseSysSectRule *BaseRuleMgr::gpBaseSysSectRule = 0;

#define EFFDATE			"effdate" 

#define LINK_FLAG			"link"
#define EFF_FLAG			"eff"
#define DESC_FLAG			"desc"
#define DEFAULT_FLAG		"default"
#define MIN_FLAG			"min"
#define MAX_FLAG			"max"
#define ENUM_FLAG			"enum"
#define SUGGEST_FLAG		"suggest"
#define KEYWORD_FLAG		"keyword"
#define VALUE_FLAG			"value"
#define CHANGE_FLAGE		"change"
#define NEED_FLAGE			"need"
#define ARBI_FLAGE			"arbi"
#define TIME24_START_FLAGE		"time24start"
#define TIME24_END_FLAGE		"time24end"
#define CHANGE_VAL_FLAGE    "changeval"
#define CHAR_OR_NUMBER_FLAGE    "char_or_num"
#define DISOBEY_FLAGE    "disobey"
#define GROUP    		"group"

BaseRuleMgr::BaseRuleMgr()
{
//	if(!gpCIniFileMgr)
		gpCIniFileMgr = new CIniFileMgr();
	m_bLoadRule = false;
	m_bMonDebug = false;
	//m_poParamName.clear();
	//m_poSectName.clear();
	m_poParamRule = 0;
	gpBaseSysSectRule = 0;
	m_poIndex = 0;
	m_poGroupIndex = 0;
	for(int i=0;i<MAX_GROUP_NUM;i++)
	{
		m_iCurGroupParam[i] = 0;
	}
	//memset(m_iCurGroupParam,0,sizeof(int)*MAX_GROUP_NUM);
	m_iCurGroup = 0;
	loadRule();
}

BaseRuleMgr::~BaseRuleMgr()
{
    if(gpCIniFileMgr)
	{
		delete gpCIniFileMgr;
    	gpCIniFileMgr = 0;
	}
	freeALLMalloc();
	if(m_poIndex)
	{
		delete m_poIndex;
		m_poIndex = 0;
	}
	if(m_poGroupIndex)
	{
		delete m_poGroupIndex;
		m_poGroupIndex = 0;
	}
	if(m_poGroupNoIndex)
	{
		delete m_poGroupNoIndex;
		m_poGroupNoIndex = 0;
	}
}
void BaseRuleMgr::freeALLMalloc()
{
	 BaseSysSectRule *pSectRule = 0;
	 BaseSysSectRule *pSectRuleTmp = 0;
	 pSectRule = gpBaseSysSectRule;
	 while(pSectRule)
	 {
	 	pSectRuleTmp = pSectRule->next;
		freeALLParamMalloc(pSectRule);
		free(pSectRule);
		pSectRule = pSectRuleTmp;
	 }
}

void BaseRuleMgr::freeALLParamMalloc(BaseSysSectRule *pSectRule)
{
	 if(pSectRule)
	 {
	 	BaseSysParamRule *pParamRule = 0;
	 	BaseSysParamRule *pParamRuleTmp = 0;
	 	pParamRule = pSectRule->pFirst;
	 	while(pParamRule)
	 	{
	 		pParamRuleTmp = pParamRule->pNext;
			free(pParamRule);
			pParamRule = pParamRuleTmp;
	 	}
	 }
}

//测试用函数
void BaseRuleMgr::show()
{
	BaseSysSectRule *pSect = gpBaseSysSectRule;
	while(pSect)
	{
		printf("%s\n",pSect->sSectName);
		BaseSysParamRule *pParam = pSect->pFirst;
		while(pParam)
		{
			printf("%s%s\n","参数名: ",pParam->sParamName);
			printf("%s%s\n","最大值: ",pParam->sMaxVal);
			printf("%s%s\n","最小值: ",pParam->sMinVal);
			printf("%s%s\n","建议值: ",pParam->sSuggestVal);
			printf("%s%s\n","默认值: ",pParam->sDefaultVal);
			printf("%s%s\n","描述: ",pParam->sDesc);
			printf("%s%s\n","当前值: ",pParam->sVal);
			for(int i =0;i<pParam->iEnumPos;i++)
			{
				printf("%s%s\n","枚举值: ",pParam->sEnumVal[i]);
			}
			pParam = pParam->pNext;
		}
		pSect = pSect->next;
	}
}

void BaseRuleMgr::setDebugState(bool _Debug)
{
	 m_bMonDebug = _Debug;
}


BaseVarInfo *BaseRuleMgr::mallocBaseVar(int iGroup)
{
	if(m_iCurGroupParam[iGroup]<MAX_GROUP_PARAM_NUM)
	{ //这个组的使用量小于最大限制值
		BaseVarInfo *p = &m_oBaseVarInfo[iGroup][ m_iCurGroupParam[iGroup]];
		memset(p,0,sizeof(BaseVarInfo));
		p->next = NULL;
		return  &m_oBaseVarInfo[iGroup][ m_iCurGroupParam[iGroup]++ ];
	}
	return NULL;
}

//根据段名,参数名获取参数规则(public函数)
//如段名存在,参数名不存在返回段下第一个参数;段名不存在返回NULL
BaseSysParamRule *BaseRuleMgr::getParamRule(char *sSectNameSrc,char *sParamNameSrc)
{
	if(sSectNameSrc[0] == '\0' || sParamNameSrc[0] == '\0')
		return NULL;
	char sSectName[MAX_SECTION_LEN+1] = {0};
	char sParamName[MAX_KEY_NAME_LEN+1] = {0};
	strncpy(sSectName,sSectNameSrc,sizeof(sSectName));
	strncpy(sParamName,sParamNameSrc,sizeof(sParamName));
	//if(!Chang2Upper(sSectName) || !Chang2Lower(sParamName))
	//	return NULL;
	BaseSysSectRule *pSect = gpBaseSysSectRule;
	while(pSect)
	{
		if(strcmp(pSect->sSectName,sSectName)==0)
		{
			BaseSysParamRule *pParam = pSect->pFirst;
			//if(sParamName[0] == '\0')
			while(pParam)
			{
				if(strcmp(pParam->sParamName,sParamName)==0)
				{
					return pParam;
				}
				pParam = pParam->pNext;
			}
			return NULL;
		} else {
			pSect = pSect->next;		
		}
	}
	return NULL;
}

// 反向获得分隔符偏移量
int BaseRuleMgr::anayParam(char *sParam,char sep)
{
	 string *p = new string(sParam);
	 int iPos = p->rfind(sep);
	 delete p;
	 p = 0;
	 return iPos;
}

// 获取参数名(参数sDscParamName要自己保证足够记录源于sSrcName的数据)
int BaseRuleMgr::getParamName(char *sSrcName,char *sDscParamName,char sep)
{
	 if(sSrcName[0] == '\0'){
	 	sDscParamName[0] = '\0';
		return 0;
	 }
	 int resPos = anayParam(sSrcName,sep);
	 if(resPos>0) {
	 	strncpy(sDscParamName,sSrcName,resPos);
	 } else
	 { 
	 	sDscParamName[0] = '\0';
		return -1;
	 }
	 return resPos;
}

// 根据段名malloc出段结构(同段名直接返回已经malloc出的段指针)
BaseSysSectRule *BaseRuleMgr::MallocSectRule(char *sSectName)
{
	BaseSysSectRule *pSRule = gpBaseSysSectRule;
	BaseSysParamRule *pPRule = 0;
	if(NULL == pSRule)
	{
		pSRule = (BaseSysSectRule*)malloc(sizeof(struct BaseSysSectRule));
		while(NULL == pSRule)
		{
			sleep(3);
			pSRule = (BaseSysSectRule*)malloc(sizeof(struct BaseSysSectRule));
		}
		memset(pSRule,'\0',sizeof(struct BaseSysSectRule));				
		strncpy(pSRule->sSectName,sSectName,sizeof(pSRule->sSectName));
		pSRule->pFirst = NULL;
		pSRule->next = NULL;
		gpBaseSysSectRule = pSRule;
		return pSRule;
	} else {
		while(pSRule)
		{
			if(strcmp(pSRule->sSectName,sSectName) == 0)
			{
				return pSRule;
			} else if(NULL == pSRule->next)
			{
				pSRule->next = (BaseSysSectRule*)malloc(sizeof(struct BaseSysSectRule));	
				while(NULL == pSRule->next)
				{
					sleep(3);
					pSRule->next = (BaseSysSectRule*)malloc(sizeof(struct BaseSysSectRule));
				}			
				pSRule = pSRule->next;			
				memset(pSRule,'\0',sizeof(struct BaseSysSectRule));
				strncpy(pSRule->sSectName,sSectName,sizeof(pSRule->sSectName));
				pSRule->pFirst = NULL;
				pSRule->next = NULL;				
				return pSRule;
			} else {
				pSRule = pSRule->next;
			}
		} // end...while(pSRule)
	}

return pSRule;

}

// 根据段结构malloc出参数规则
BaseSysParamRule *BaseRuleMgr::MallocParamRule(BaseSysSectRule *pSectRule)
{
	BaseSysParamRule *pPRule = 0;
	if(pSectRule)
	{
		pPRule = pSectRule->pFirst;
		if(!pPRule){
			pPRule = (BaseSysParamRule*)malloc(sizeof(struct BaseSysParamRule));
			while(!pPRule)
			{
				sleep(3);
				pPRule = (BaseSysParamRule*)malloc(sizeof(struct BaseSysParamRule));
			}
			memset(pPRule,'\0',sizeof(struct BaseSysParamRule));
			pPRule->pNext = NULL;
			pPRule->iEnumPos = 0;		
			pPRule->iLinkNum = 0;
			pSectRule->pFirst = pPRule;
			return pPRule;
		}	
		while(pPRule)
		{			
			if(!pPRule->pNext){
				pPRule->pNext = (BaseSysParamRule*)malloc(sizeof(struct BaseSysParamRule));	
				while(!pPRule->pNext)
				{
					sleep(3);
					pPRule->pNext = (BaseSysParamRule*)malloc(sizeof(struct BaseSysParamRule));
				}						
				pPRule = pPRule->pNext;				
				memset(pPRule,'\0',sizeof(struct BaseSysParamRule));	
				pPRule->iEnumPos = 0;
				pPRule->iLinkNum = 0;
				pPRule->pNext = NULL;				
				return pPRule;
			}
			pPRule = pPRule->pNext;
		}
	} else {
		return NULL;
	}
return pPRule;
}


//找不到新建一个结构体返回,直接使用这个结构体就可以了
BaseSysParamRule *BaseRuleMgr::SearchParamRule(char *sSectName,char *sParamName)
{
	if(sSectName[0] == '\0')
		return NULL;
	BaseSysSectRule *pSectRule = 0;
	BaseSysParamRule *pParamRule = 0;
	pSectRule = gpBaseSysSectRule;
	if(!pSectRule) //无段
	{
		gpBaseSysSectRule = MallocSectRule(sSectName);
		pParamRule = MallocParamRule(gpBaseSysSectRule);
		strncpy(pParamRule->sParamName,sParamName,sizeof(pParamRule->sParamName));
		return pParamRule;
	} else { //有段
		while(pSectRule)
		{
			if(strcmp(pSectRule->sSectName,sSectName) == 0) //同段
			{
				pParamRule = pSectRule->pFirst;
				if(!pParamRule){
					pParamRule =  MallocParamRule(pSectRule);
					strncpy(pParamRule->sParamName,sParamName,sizeof(pParamRule->sParamName));
					return pParamRule;
				}
				while(pParamRule)
				{
					if(strcmp(pParamRule->sParamName,sParamName)==0)
							return pParamRule;					

					if(!pParamRule->pNext)
					{
						pParamRule->pNext = MallocParamRule(pSectRule);
						pParamRule = pParamRule->pNext;
						strncpy(pParamRule->sParamName,sParamName,sizeof(pParamRule->sParamName));
						return pParamRule;
					} else {
						pParamRule = pParamRule->pNext;
					}
				}
			} else if(!pSectRule->next)
			{
				pSectRule->next = MallocSectRule(sSectName);
				pParamRule = MallocParamRule(pSectRule->next);
				strncpy(pParamRule->sParamName,sParamName,sizeof(pParamRule->sParamName));
				return pParamRule;
			} else {
				pSectRule = pSectRule->next;
			}
		}
	}
return pParamRule;
}

// 
BaseVarInfo *BaseRuleMgr::MallocBaseVar(char *sGroupName)
{
		BaseVarInfo *pBaseVarInfoGroup = 0;
		int iOffset = 0;
		if(m_poGroupNoIndex->get(sGroupName,&iOffset))//m_poGroupNoIndex加载用的临时变量
		{
			return mallocBaseVar(iOffset);
		} else if(m_iCurGroup<MAX_GROUP_NUM)
		{
			pBaseVarInfoGroup =  mallocBaseVar(m_iCurGroup);
			m_poGroupNoIndex->add(sGroupName,m_iCurGroup++);
			return pBaseVarInfoGroup;
		} else {
			return 0;//退出GROUP的加载
		}	
}

//加载参数规则
bool BaseRuleMgr::loadRule()
{
	char sHome[254];
        char * p;

        if ((p=getenv (ENV_HOME)) == NULL){
         	sprintf (sHome, "/home/SGW_HOME/etc");
		 }
        else {
         sprintf (sHome, "%s/etc", p);
		}
	string strFileName=sHome;
        strFileName+= "/SysParamRule";
	if(!gpCIniFileMgr->LoadIniFromFile(strFileName.c_str())){
		printf("%s","加载文件到内存失败");
		return false;
	}
	BaseSysSectRule *pSectRule = 0;
	BaseSysParamRule *pParamRule = 0;
	for(int j = 1;j<gpCIniFileMgr->m_iUseNum+1;j++)
	{	
		if(gpCIniFileMgr->m_pIniKey[j].sSectionName[0] != '\0' && gpCIniFileMgr->m_pIniKey[j].sKeyName[0] != '\0')
		{
				char sNameTmp[512] = {0};
				//整出一个段与参数名
				BaseVarInfo mBaseVarInfo;
				memset(&mBaseVarInfo,'\0',sizeof(BaseVarInfo));
				strcpy(mBaseVarInfo.sSectName,gpCIniFileMgr->m_pIniKey[j].sSectionName);
				//Chang2Upper(mBaseVarInfo.sSectName);//只能分辨出段名称				
				
				if(strcmp(mBaseVarInfo.sSectName,GROUP)==0) //如果是特殊段就这个处理
				{
					
					strcpy(mBaseVarInfo.sParamName,gpCIniFileMgr->m_pIniKey[j].sKeyName);
					//Chang2Lower(mBaseVarInfo.sParamName);
					
					BaseVarInfo *pBaseVarInfoGroup = 0;
					
					if(!m_poGroupIndex)
						m_poGroupIndex = new HashList<BaseVarInfo*>(90000);
					if(!m_poGroupNoIndex)
						m_poGroupNoIndex = new HashList<int>(90000);
					if(!m_poIndex)
						m_poIndex = new HashList<BaseSysParamRule*>(90000);
					char *pValTmp = gpCIniFileMgr->m_pIniKey[j].sKeyValue[0];//这个特殊段的值其实就是参数全称形式用,隔开
					char *pVal = new char[strlen(pValTmp)+2];
					if(!pVal)
						THROW(100);
					memset(pVal,'\0',sizeof(pVal));
					strcpy(pVal,pValTmp);
					char *pRec = gpCIniFileMgr->m_pIniKey[j].sKeyValue[0];//
					int iOffset = 0;
					int iTime = 0;
					vector <string> vec_tag;
					getTagStringN(pVal,vec_tag,',');
					iTime = 0;
					
					while(iTime<vec_tag.size())
					{											
						BaseVarInfo *pBaseVarInfoGroup = 0;
						int iOffset = 0;
						if(m_poGroupNoIndex->get(mBaseVarInfo.sParamName,&iOffset))//m_poGroupNoIndex加载用的临时变量
						{
							pBaseVarInfoGroup = mallocBaseVar(iOffset);
							if(!pBaseVarInfoGroup)
								break;
						} else if(m_iCurGroup<MAX_GROUP_NUM)
						{
							pBaseVarInfoGroup = mallocBaseVar(m_iCurGroup);
							if(!pBaseVarInfoGroup)
								break;
							m_poGroupNoIndex->add(mBaseVarInfo.sParamName,m_iCurGroup++);
						} else {
							break;//退出GROUP的加载
						}	
						
						memset(pBaseVarInfoGroup,'\0',sizeof(BaseVarInfo));
						char sKey[MAX_SECTION_LEN+MAX_KEY_NAME_LEN+1] = {0};
						strcpy(sKey,(char*)vec_tag[iTime].c_str());
						char sGroupParamName[MAX_KEY_NAME_LEN+1] = {0};
						string strGroup(sKey);
						trimN(strGroup);
						sKey[0] = 0;
						strcpy(sKey,(char*)strGroup.c_str());
						int iGroupPos = strGroup.find('.');//是按照标准的段.参数这样的格式就处理
						if(iGroupPos)
						{
							strncpy(pBaseVarInfoGroup->sSectName,sKey,iGroupPos);
							strncpy(pBaseVarInfoGroup->sParamName,&sKey[iGroupPos+1],sizeof(sKey));
							//Chang2Upper(pBaseVarInfoGroup->sSectName);
							//Chang2Lower(pBaseVarInfoGroup->sParamName);
							BaseVarInfo *pBaseVarInfoGroupGet = 0;
							if(!m_poGroupIndex->get(mBaseVarInfo.sParamName,&pBaseVarInfoGroupGet))//GROUP名为主键
							{
								m_poGroupIndex->add(mBaseVarInfo.sParamName,pBaseVarInfoGroup);
							} else {
								pBaseVarInfoGroup->next = pBaseVarInfoGroupGet;
								m_poGroupIndex->add(mBaseVarInfo.sParamName,pBaseVarInfoGroup);//貌似还要加上组标识							
							}
							#ifndef DEF_UNLIMIT_GROUP_
							//连接到规则上
							BaseSysParamRule *pBR = 0;
							if(m_poIndex->get(sKey,&pBR))
							{
								strcpy(pBR->sGroupParamName,mBaseVarInfo.sParamName);
							} else {
								BaseSysSectRule *pSect = MallocSectRule(pBaseVarInfoGroup->sSectName);
								if(!pSect)
									continue;
								BaseSysParamRule *pParam = MallocParamRule(pSect);
								if(!pParam)
									continue;
								strcpy(pParam->sGroupParamName,mBaseVarInfo.sParamName);
								m_poIndex->add(sKey,pParam);
							}
							#else//多个组功能
							
							#endif
						}
						iTime++;
					}															
					if(pVal)
					{
						delete []pVal;
						pVal = 0;
					}
				} else { //原来的处理
					int resPos = 0 ;
					int iRes = getParamName(gpCIniFileMgr->m_pIniKey[j].sKeyName,sNameTmp,'.');	
					if(iRes<0){
						//m_bLoadRule = false;//THROW(MBC_PARAM_RULE+1);
						Log::log(0,"根据规则参数[%s]获取规则空间失败!",gpCIniFileMgr->m_pIniKey[j].sKeyName);
						continue;//return false;
					}																						
					resPos = anayParam(gpCIniFileMgr->m_pIniKey[j].sKeyName,'.');
					strcpy(mBaseVarInfo.sParamName,sNameTmp);
					//Chang2Lower(mBaseVarInfo.sParamName);
					pParamRule = SearchParamRule(mBaseVarInfo.sSectName,mBaseVarInfo.sParamName);
					if(!pParamRule){
						//m_bLoadRule = false;//THROW(MBC_PARAM_RULE+1);
						Log::log(0,"规则参数[%s.%s]获取规则空间失败!",mBaseVarInfo.sSectName,mBaseVarInfo.sParamName);
						continue;//return false;
					}
					int iResType = 0;
					
					iResType = getType(gpCIniFileMgr->m_pIniKey[j].sKeyName,gpCIniFileMgr->m_pIniKey[j].sKeyValue[0],pParamRule);
					if(!m_poIndex)
						m_poIndex = new HashList<BaseSysParamRule*>(90000);
					
					BaseSysParamRule *pParamRuleGet = 0;
					char sKey[MAX_SECTION_LEN+MAX_KEY_NAME_LEN+1] = {0};
					sprintf(sKey,"%s.%s",mBaseVarInfo.sSectName,mBaseVarInfo.sParamName);
					if(!m_poIndex->get(sKey,&pParamRuleGet))
					{
						m_poIndex->add(sKey,pParamRule);
					}
					if(iResType == 0)
					{
						//m_bLoadRule = false;
						Log::log(0,"解析规则参数[%s]失败!",sKey);
						continue;	//THROW(MBC_PARAM_RULE+1);
					}
				}
						
		} 
	}
	m_bLoadRule = true;
	return m_bLoadRule;
}

// 获取参数名(参数sDscParamName要自己保证足够记录源于sSrcName的数据)
int BaseRuleMgr::getParam(char *sSrcName,char *sDscParamName,char sep)
{
	 if(sSrcName[0] == '\0')
		return 0;
	 sDscParamName[0] = '\0';
	 string str(sSrcName);
	 int resPos = str.find(sep);
	 if(resPos!=0) 
	 {
	 	string strTmp;
		if(resPos>0)
		strTmp.assign(str,0,resPos);
	 	else
			strTmp = str;
	 	trimN(strTmp);
	 	strcpy(sDscParamName,(char*)strTmp.c_str());
		if(resPos>0)
		return resPos;
		else
			return 0;
	 } else//这个是字符在第一位
	 {
	 	return 0;
	 }
}

int BaseRuleMgr::addGroupType(BaseVarInfo *pBaseVarInfo,char *sVal,BaseSysParamRule *pSPRule)
{
	/*if(sVal[0] == '\0')
		return 0;
	//
	string str(sVal);
	while(str.size()>0)
	{
		int iPos = str.find(',');
		string strT;
		strT.assign(str,0,iPos);
		str.assign(str,iPos+1,str.size());
		strcpy(pSPRule->sGroupParamName[pSPRule->iGroupParamNum++],(char*)str.c_str());
	}
	return pSPRule->iGroupParamNum;*/
	return 0;
}

// 实际参数规则字段赋值函数
// 参数说明:sStr-带参数名以及suggest等参数的参数名;rPos-分隔符反向获得偏移量;sVal-参数值;pSPRule-要赋值的参数结构
// 返回值是标识给那个字段赋值的
int BaseRuleMgr::getType(char *sStr,char *sVal,BaseSysParamRule *pSPRule)
{
	if(!sStr || sStr[0] == '\0')
		return 99;
	string source = string(sStr);
	int pos = source.rfind('.');
	if(pos<=0){
		printf("%s%s%s\n","参数[",sStr,"]规则配置格式有误,请检查!");
		return 99;
	}
	char sDsc[1024] = {0};
	strncpy(sDsc,&sStr[pos+1],strlen(sStr)-pos);
	if(sDsc[0] == '\0'){
		if(!m_bLoadRule)
			printf("%s%s%s\n","参数[",sStr,"]规则配置格式有误,请检查!");
		return 99;
	}
	//Chang2Lower(sDsc);
	if(strcmp(sDsc,SUGGEST_FLAG)==0)//建议值
	{
		strncpy(pSPRule->sSuggestVal,sVal,sizeof(pSPRule->sSuggestVal));
		return 1;
	} /*else if(strcmp(sDsc,VALUE_FLAG)==0)//标准值
	{
		strncpy(pSPRule->sVal,sVal,sizeof(pSPRule->sVal));
		return 2;
	} */else if(strcmp(sDsc,DEFAULT_FLAG)==0)//默认值
	{
		strncpy(pSPRule->sDefaultVal,sVal,sizeof(pSPRule->sDefaultVal));
		return 3;
	}else if(strcmp(sDsc,MIN_FLAG)==0)//最小值
	{
		strncpy(pSPRule->sMinVal,sVal,sizeof(pSPRule->sMinVal));
		return 4;
	}else if(strcmp(sDsc,MAX_FLAG)==0)//最大值
	{
		strncpy(pSPRule->sMaxVal,sVal,sizeof(pSPRule->sMaxVal));
		return 5;
	}else if(strcmp(sDsc,ENUM_FLAG)==0)//枚举值
	{
			vector<string> vec_tag;
			vec_tag.clear();
			getTagStringN(sVal,vec_tag,',');
			vector<string>::iterator itr = vec_tag.begin();
			for(;itr != vec_tag.end();itr++)
			{
				strncpy(pSPRule->sEnumVal[pSPRule->iEnumPos++],(*itr).c_str(),sizeof(pSPRule->sEnumVal[pSPRule->iEnumPos++]));
			}
			return 6;
	}else if(strcmp(sDsc,DESC_FLAG)==0)//描述
	{
		strncpy(pSPRule->sDesc,sVal,sizeof(pSPRule->sDesc));
		return 7;
	}else if(strcmp(sDsc,EFF_FLAG)==0)//描述
	{
		pSPRule->iEffDateFlag = atoi(sVal);
		return 8;
	}/*else if(strcmp(sDsc,KEYWORD_FLAG)==0)//描述
	{
		pSPRule->iKeyWordFlag = atoi(sVal);
		return 9;
	}*/else if(strcmp(sDsc,CHANGE_FLAGE)==0)//描述
	{
		pSPRule->iValueChange = atoi(sVal);
		return 9;//CHANGE_FLAGE
	}else if(strcmp(sDsc,CHANGE_VAL_FLAGE)==0)//描述
	{
		strcpy(pSPRule->sValueChange,sVal);
		return 15;//CHANGE_FLAGE
	}else if(strcmp(sDsc,NEED_FLAGE)==0)//描述
	{
		pSPRule->iParamNeed = atoi(sVal);
		return 10;//NEED_FLAGE
	}else if(strcmp(sDsc,ARBI_FLAGE)==0)
	{
		pSPRule->cArbi = 'Y';
		return 11;//ARBITIONE_FLAGE
	}else if(strcmp(sDsc,TIME24_START_FLAGE)==0)
	{
		strcpy(pSPRule->sTime24Start,sVal);
		return 12;
	}else if(strcmp(sDsc,TIME24_END_FLAGE)==0)
	{
		strcpy(pSPRule->sTime24End,sVal);
		return 13;
	}else if(strcmp(sDsc,LINK_FLAG)==0)//链接参数
	{
		vector<string> vec_tag;
		vec_tag.clear();
		getTagStringN(sVal,vec_tag,',');
		vector<string>::iterator itr = vec_tag.begin();
		for(;itr != vec_tag.end();itr++)
		{
			strncpy(pSPRule->sLinkParamName[pSPRule->iLinkNum++],(*itr).c_str(),sizeof(pSPRule->sLinkParamName[pSPRule->iLinkNum]));
		}
		return 14;
	}else if(strcmp(sDsc,CHAR_OR_NUMBER_FLAGE)==0)//是否是NUM参数
	{
		pSPRule->iCharOrNum = atoi(sVal);
		return 15;
	}else if(strcmp(sDsc,DISOBEY_FLAGE)==0)//是否是NUM参数
	{
		strncpy(pSPRule->sDisobey,sVal,sizeof(pSPRule->sDisobey));
		return 16;
	}
	else {
		if(!m_bLoadRule)
			printf("%s%s%s\n","参数[",sStr,"]规则配置格式有误,请检查!");
		return 99;//现在返回一个有效值 新增支持不带后缀的参数
	}
	return 0;
}

// srcStr的格式: 段.参数=值,....
bool BaseRuleMgr::check(const char *srcVal,bool Show)
{
	 if(srcVal[0] == '\0')
	 	return false;
	 return check((char*)srcVal,Show);
}

// srcStr的格式: 段.参数=值,....
bool BaseRuleMgr::check(char *srcVal,bool Show)
{
	 if(srcVal[0] == '\0')
	 	return true;//无参数不检测
	 if(!m_bLoadRule)
	 {
	 	if(Show)
		{
			printf("参数规则加载失败,此参数不做检查,请检查参数规则配置!\n");
		}
		return true;//规则加载失败,不检测
	 }
	 bool res = false;
	 m_poParamRule = 0;
	 /*char sSect[MAX_KEY_VALUE_LEN] = {0};
	 char sParam[MAX_KEY_VALUE_LEN] = {0};
	 char sVal[MAX_KEY_VALUE_LEN] = {0};
	 
	 vector<string> vec_tag;
	 vec_tag.clear();
	 getTagStringN(srcVal,vec_tag,',');
	 if(vec_tag.size()>2 || vec_tag.size()<1)
	 {
	 	if(Show)
	 		printf("%s\n","输入参数过多,这种参数格式不支持,请检查!");
	 	return false;
	 }*/
	 res = paramParse(srcVal,Show);
	 return res;
}
// 生效日期的检测,strWholeEffdate格式effdate=2010....
bool BaseRuleMgr::checkEffDate(string strWholeEffdate)
{	 
	 int iEffDatePos = 0;
	 if(m_poParamRule)
	 {
	 	switch(m_poParamRule->iEffDateFlag)
		{
			case 3:
					iEffDatePos = strWholeEffdate.find('=');
	 				if(iEffDatePos)
	 				{
	 					string sPureEffdate(strWholeEffdate,0,iEffDatePos);
						string sPureEffdateVal(strWholeEffdate,iEffDatePos+1,strWholeEffdate.size());
						trimN(sPureEffdate);
						trimN(sPureEffdateVal);
						if(sPureEffdate.size()<7 || sPureEffdateVal.size()<4)//effdate,以及日期最少精确到年
							return false;
						char sEffFlag[32] = {0};
						strncpy(sEffFlag,sPureEffdate.c_str(),sizeof(sEffFlag));
						//Chang2Lower(sEffFlag);
						if(strcmp(sEffFlag,EFFDATE)!=0)
							return false;
						if(checkEffdate(m_poParamRule,(char*)sPureEffdateVal.c_str())==0)
							return true;
						else	
							return false;
	 				} else {
						if(!m_bMonDebug)
								printf("%s\n","请设置正确生效日期值,请检查!");
						return false;
	 				}				
					break;
			case 1:
			case 2:
					if(strWholeEffdate.size()>0)
					{
						if(!m_bMonDebug)
							printf("%s\n","此参数无须带生效日期值等");
						return false;
					}
					break;
			default:
					break;
		}
	 }
	 return true;
}

bool BaseRuleMgr::paramParse(const char* str,bool Show)
{
	 if(str[0] == '\0')
	 	return false;

	 string strEffdate;
	 string sPureEff;
	 string sEffFlag;//标识effdate
	 string sParam;
	 string strAllParam(str);
	 int iParamEffPos = strAllParam.find(",");
	 if(iParamEffPos == 0)
	    return false;
	 if(iParamEffPos>0)
	 {
	    strEffdate.assign(strAllParam,iParamEffPos+1,strAllParam.size());
	    sParam.assign(strAllParam,0,iParamEffPos);
	 } else {
	    sParam = strAllParam;
	 }
	 int iValPos = sParam.rfind('=');
	 int iParamPos = sParam.find('.');
	 if(iValPos<0 || iParamPos<0){
		if(Show) 
	 		printf("%s\n","参数格式不正确,或者设置参数值时参数名与值之间的=两边包含有空格等特殊字符.正确参数格式例如: Section.param=value");
		return false;
	 }
	 string sPureSect(sParam,0,iParamPos);
	 string sPureParam(sParam,iParamPos+1,iValPos-iParamPos-1);
	 string sPureVal(sParam,iValPos+1,sParam.size());
	 trimN(sPureSect);
	 trimN(sPureParam);
	 trimN(sPureVal);
	 trimN(sPureEff);
	 //m_poSectName = sPureSect;
	 //m_poParamName = sPureParam;
	 //
	 m_sParamName[0] = '\0';
	 m_sSectName[0] = '\0';
	 strcpy(m_sParamName,(char*)sPureParam.c_str());
	 strcpy(m_sSectName,(char*)sPureSect.c_str());
	 //
	 // 增加对生效日期标识的判断
	 if(strEffdate.size()>0)
	 {
	 	int iEffPos = strEffdate.find("=");
	 	if(iEffPos>0)
	 	{
			sPureEff.assign(strEffdate,iEffPos+1,strEffdate.size());
			sEffFlag.assign(strEffdate,0,iEffPos);
			trimN(sEffFlag);
			trimN(sPureEff);
			char sEffDateFlag[32] = {0};
			strncpy(sEffDateFlag,(char*)sEffFlag.c_str(),sizeof(sEffDateFlag));
			//Chang2Lower(sEffDateFlag); 
			if( strcmp(sEffDateFlag,EFFDATE) != 0 )
			{
				if(Show) 
	 				printf("%s\n","参数格式不正确,生效日期标识不正确");
				m_poParamRule = 0;//getParamRule((char*)sPureSect.c_str(),(char*)sParam.c_str());
				return false;
			}
	 	}
	 }
	 int iRes = check((char*)sPureSect.c_str(),(char*)sPureParam.c_str(),(char*)sPureVal.c_str(),(char*)sPureEff.c_str(),NULL,Show);
	 if(iRes == 0)
	 	return true;
	 else
	 {
	 	if(Show)
		{
			int iResEnd = ERR_RULE_PARAM+iRes;
			Log::log(0,"%s","参数不匹配规则!");
		}
		return false;	
	 }
}

// 规则说明
void BaseRuleMgr::paramExplain(char*sSect,char *sParam,bool bAfterCheck)
{
	 if(m_bMonDebug)
	 	return;
	 if(!m_bLoadRule){
	 	printf("%s\n","规则加载失败,请检查参数规则配置数据!");
		return ;
	 }
	 if(sSect[0] == '\0' || sParam[0] == '\0')
	 {
	 	printf("%s\n","参数数据不全，请检查!");
		return;
	 }
	 BaseSysParamRule *pParamRule = 0;
	 if(!bAfterCheck)
	 	pParamRule = getParamRule(sSect,sParam);
	 else
	 	pParamRule = m_poParamRule;
	 if(pParamRule)
	 {
	 	if(pParamRule->sParamName[0] != '\0')
	 		printf("%s%s\n","参数名: ",pParamRule->sParamName);
		if(pParamRule->sMaxVal[0] != '\0')
			printf("%s%s\n","最大值: ",pParamRule->sMaxVal);
		if(pParamRule->sMinVal[0] != '\0')
			printf("%s%s\n","最小值: ",pParamRule->sMinVal);
		if(pParamRule->sSuggestVal[0] != '\0')
			printf("%s%s\n","建议值: ",pParamRule->sSuggestVal);
		if(pParamRule->sDefaultVal[0] != '\0')
			printf("%s%s\n","默认值: ",pParamRule->sDefaultVal);
		if(pParamRule->sDesc[0] != '\0')
			printf("%s%s\n","参数描述: ",pParamRule->sDesc);
		if(pParamRule->sEnumVal[0][0] != '\0')
			printf("%s","枚举值: ");
		for(int i =0;i<pParamRule->iEnumPos;i++)
		{
			if(pParamRule->sEnumVal[i][0] != '\0')
				printf("%s",pParamRule->sEnumVal[i]);
			if(pParamRule->sEnumVal[i+1][0] == '\0')
				printf("\n");
			else 
				printf("%s",", ");
		}
		switch(pParamRule->iEffDateFlag)
		{
			case 1:
					printf("%s\n","此参数是立即生效方式");
					break;
			case 2:
					printf("%s\n","此参数是重启生效方式");
					break;
			case 3:
					printf("%s\n","此参数是带生效日期方式生效");
					break;
			default:
					printf("%s\n","重启生效");
					break;
		}
		switch(pParamRule->iValueChange)
		{
			case 1:
					printf("%s\n","此参数只能增加");
					break;
			case 2:
					printf("%s\n","此参数不能修改");
					break;
			case 3:
					printf("%s\n","此参数需要匹配规则");
					if(pParamRule->sValueChange[0] != '\0')
						printf("%s%s\n","此参数匹配的规则:", pParamRule->sValueChange);
					break;
			case 4:
					printf("%s\n","此参数只能减小");
					break;
			case 5:
					printf("%s\n","此参数只能是ip");
					break;
		}
		if(pParamRule->sDisobey[0] != '\0')
		{
			printf("%s%s\n","此参数数值不为空且需要规避以下字符:",pParamRule->sDisobey);
		}
		if(pParamRule->cArbi == 'Y')
		{
			printf("%s\n","此参数需要仲裁");
		}
		if(pParamRule->sTime24Start[0] != '\0' && pParamRule->sTime24End[0] != '\0')
		{
			printf("%s%s%s%s\n","此参数设置的时间最小值: ",pParamRule->sTime24Start ,"最大值: ",pParamRule->sTime24End);
		}
		if(pParamRule->iLinkNum>0)
		{
			printf("%s","关联参数: ");
			for(int i =0;i<pParamRule->iLinkNum;i++)
			{
				if(pParamRule->sLinkParamName[i][0] != '\0')
					printf("%s",pParamRule->sLinkParamName[i]);
				if(pParamRule->sLinkParamName[i+1][0] == '\0')
					printf("\n");
				else 
					printf("%s",", ");
			}
		}
		if(pParamRule->iParamNeed>0)
		{
			printf("%s\n","此参数必须存在");
		}
		if(pParamRule->iCharOrNum==1)
		{
			printf("%s\n","此参数必须是数值");
		}
		if(pParamRule->sGroupParamName[0] != '\0')
		{
			if(!m_poGroupIndex){
				printf("%s%s\n","此参数属于参数组:",pParamRule->sGroupParamName);
				return ;//没规则就不继续显示了
			}
			BaseVarInfo *pVar = 0;
			int i = 1;
			if(m_poGroupIndex->get(pParamRule->sGroupParamName,&pVar))
			{
				printf("此参数属于参数组[%s]\n",pParamRule->sGroupParamName);
				while(pVar)
				{
					printf("参数组[%s]第[%d]个参数是[ %s.%s ]\n",pParamRule->sGroupParamName,i++,pVar->sSectName,pVar->sParamName);
					pVar = pVar->next;		
				}
			}
		}
	 } else {
	    if(!bAfterCheck)
	 		printf("%s\n","对应参数未配置规则,请检查!");
	 }
}

// 显示规则说明(只显示)
void BaseRuleMgr::paramParseExplain(const char* str)
{
	 if(m_bMonDebug)
	 	return;
	 if(!m_bLoadRule){
	 	printf("%s\n","规则加载失败,请检查参数规则配置数据!");
		return ;
	 }
	 if(str[0] == '\0')
	 {
	 	printf("%s\n","无参数，请检查!");
		return;
	 }
	 string sParam = str;
	 int iValPos = sParam.rfind('=');
	 int iParamPos = sParam.find('.');
	 if(iParamPos<0)
	 {
	 	printf("%s\n","参数名未输全或格式有误,导致无法提供对应参数的规则信息!");
		return;
	 }
	 string sPureSect;
	 string sPureParam;
	 string sPureVal;
	 sPureSect.clear();
	 sPureParam.clear();
	 sPureVal.clear();
	 if(iValPos<0)
	 {
	 	sPureSect.assign(sParam,0,iParamPos);
		sPureParam.assign(sParam,iParamPos+1,sParam.size());
	 } else {
	 	sPureSect.assign(sParam,0,iParamPos);
	 	sPureParam.assign(sParam,iParamPos+1,iValPos-iParamPos-1);
	 	sPureVal.assign(sParam,iValPos+1,sParam.size());	 		 	
	 }
	 trimN(sPureSect);
	 trimN(sPureParam);
	 trimN(sPureVal);
	 paramExplain((char*)sPureSect.c_str(),(char*)sPureParam.c_str());// 实际处理函数
}

// bAfterCheck=true处理这种check之后的参数规则查询
void BaseRuleMgr::paramParseExplain(bool bAfterCheck)
{
	 //if(m_poSectName.size()>0 && m_poParamName.size())
	 if(m_sSectName[0] != '\0' && m_sParamName[0] != '\0')
	 {
	 	//paramExplain((char*)m_poSectName.c_str(),(char*)m_poParamName.c_str(),true);
		paramExplain(m_sSectName,m_sParamName,true);
	 } else {
	 	if(!m_bMonDebug)
			printf("%s\n","参数解析失败,请检查参数格式!");
	 }
}

/*********************************/
// 
void BaseRuleMgr::addParamRule(CIniSection *pCIniSection)
{
	BaseSysSectRule *pSectRule = 0;
	BaseSysSectRule *pLastSectRule = 0;
	BaseSysParamRule *pParamRule = 0;
	BaseSysParamRule *pLastParamRule = 0;	
	char sNameTmp[MAX_KEY_NAME_LEN+1] = {0};
	getParamName(pCIniSection->sKeyName,sNameTmp,'.');
	if(sNameTmp[0] == '\0')
	{
		return;
	}
	pParamRule = SearchParamRule(pCIniSection->sSectionName,sNameTmp);
	int resPos = anayParam(pCIniSection->sKeyName,'.');
	if(pParamRule->sParamName[0] == '\0')
		strncpy(pParamRule->sParamName,pCIniSection->sKeyName,sizeof(char)*resPos);
	int iValType = getType(pCIniSection->sKeyName,pCIniSection->sKeyValue[0],pParamRule);
	if(iValType == 0){
		THROW(34030000+1);
	}
}

bool BaseRuleMgr::checkCfgParam()
{
	 return true;
}

bool BaseRuleMgr::getCurDate(char *sDate,int iSize)
{
	 if(sizeof(sDate)<=iSize || iSize<=0)
	 	return false;
	 Date mDate;
	 sDate[0] = '\0';
	 strncpy(sDate,mDate.toString(),sizeof(char)*iSize);
	 return true;
}
/***************************************/

void BaseRuleMgr::showErrInfo(int iErrID)
{
	switch(iErrID)
	{
		case MAX_ERR:
			printf("%s\n","参数新值超过参数规则设定最大值,请检查!");
			break;
		case MIN_ERR:
			printf("%s\n","参数新值小于参数规则设定最小值,请检查!");
			break;
		case ENUM_ERR:
			printf("%s\n","参数新值不在参数规则设定的枚举值范围,请检查!");
			break;
		case EFF_ERR_LESS:
			printf("%s\n","生效日期值必须有,且是未来日期,请检查!");
			break;
		case EFF_ERR_NONEED:
			printf("%s\n","参数无需生效日期,请检查!");
			break;
		case MAX_MIN_SET_ERR:
			printf("%s\n","参数最大最小值配置错误,请检查!");
			break;
		case TIME_ERR:
			printf("%s\n","参数不符合配置的时间数据,请检查!");
			break;
		case ISNOTNUM_ERR:
			printf("%s\n","参数不符合配置数据类型(为数值且不为空或不在规则数值范围之内),请检查!");
			break;
		case DISOBEY_ERR:
			printf("%s\n","参数不符合配置,某些字符不能配置,请检查!");
			break;
		default:
			break;
	}
}

int BaseRuleMgr::checkNowValue(char *section, char *param, char *value, char *effdate)
{
	int res = 0;
	if( (section != NULL) && (param != NULL))
	{
		//没配置规则 直接算正确的
		BaseSysParamRule *rule = getParamRule(section,param);
		if(!rule)
			return 0;
		//最大最小值
		if(rule->sMaxVal[0] != '\0' || rule->sMinVal[0] != '\0')
		{
			if(!value || value[0] == '\0')
				res = MAX_MIN_SET_ERR;
			else
				res = checkMinMax(rule,value);
			if(res != 0){
				sprintf(sErrInfo,"参数[%s.%s]值不在最大最小值规则范围内",section,param);
				return res;
			}
		}
		//枚举值
		if(rule->iEnumPos>0)
		{
			if(!value || value[0] == '\0')
				res = ENUM_ERR;
			else
				res = checkEnum(rule,value);
			if(res != 0){
				sprintf(sErrInfo,"参数[%s.%s]值不在枚举规则范围内",section,param);
				return res;
			}
		}
		//生效时间值
		switch(rule->iEffDateFlag)
		{
			case 1:
			case 2:
			{
				if(effdate && effdate[0] != '\0'){
					sprintf(sErrInfo,"参数[%s.%s]值无需生效时间",section,param);
					return EFF_ERR_NONEED;
				}
				break;
			}
			case 3:
			{
				if(!effdate || effdate[0] == '\0'){
					sprintf(sErrInfo,"参数[%s.%s]值需要生效时间",section,param);
					return EFF_ERR_LESS;
				}
				break;
			}
		}
		//参数数据匹配
		if(rule->sValueChange[0] != '\0')
		{
			if(!value || value[0] == '\0')
				res = ENUM_ERR;
			else {
				res = MaskString(value,rule->sValueChange);
			}
			if(!res)
			{
				//值不符合设置的匹配数据
				sprintf(sErrInfo,"参数[%s.%s]值不符合设置的匹配数据[%s]",section,param,rule->sValueChange);
				return CHANGE_ERR;
			} else {
				if(rule->iValueChange == 5)
				{
					vector<string> vec_tagChange;
					vec_tagChange.clear();
					getTagStringN(value,vec_tagChange,'.');
					vector<string>::iterator itr = vec_tagChange.begin();
					for(;itr != vec_tagChange.end();itr++)
					{
						char *p = (char*)((*itr).c_str());
						for(int i = 0;i<strlen(p);i++)
						{
							if(!isdigit(p[i]))
							{
								return CHANGE_ERR;
							}
						}
						if(atoi(p) >=0 && atoi(p)<256)
						{
						} else {
							return CHANGE_ERR;
						}
					}	
				}
			}
		}
		//参数必须存在
		if(rule->iParamNeed>0)
		{
			if(!value || value[0] == '\0')
			{
				sprintf(sErrInfo,"参数[%s.%s]值必须存在",section,param);
				return PARAM_INFO_NEED;
			}
		}
		if(rule->sDisobey[0] != '\0')
		{
			if(!value || value[0] == '\0')
				res = DISOBEY_ERR;
			else
				res = checkDisobey(rule,value);
			if(res != 0){
				sprintf(sErrInfo,"参数[%s.%s]值设置了不能设置的字符",section,param);
				return res;
			}
		}
	}
	return 0;
}

int BaseRuleMgr::check(char *sSect,char *sParam,char *sValue,char *sEffVal,char *OldVal,bool ShowErr,int iFlag)
{
	int iRes = check(sSect,sParam,sValue,sEffVal,OldVal,iFlag);
	if(iRes != 0){
		if(ShowErr){
			showErrInfo(iRes);
		}
	}
	return iRes;	
}

/*
功能:检测参数以及参数值是否符合规范
参数:sSect:段名;sParam参数名;sValue参数值
返回值:检测通过或不需要检测为true；否则false
举例: sSect是SYSTEM  sParam是location  sValue是beijing 
或者  sSect是SYSTEM  sParam是location  sValue是beijing,sEffVal为20101010//这种是带生效日期的
*/
int BaseRuleMgr::check(char *sSect,char *sParam,char *sValue,char *sEffVal,char *OldVal,int iFlag)
{
	 BaseSysParamRule* pParamRule = getParamRule(sSect,sParam);
	 m_poParamRule = pParamRule;
	 int iRes = 0;
	 if(pParamRule)
	 {
		if(pParamRule->iValueChange>0)
		{
			iRes = checkValueChange(pParamRule,sValue,OldVal);
			if(iRes!=0)
				return iRes;	
		}	
		if(pParamRule->iCharOrNum==1)
		{
			iRes = CheckDigitString(pParamRule,sValue);
			if(iRes!=0)
				return iRes;	
		}	
		//最大最小值优先枚举值匹配
	 	if(pParamRule->sMaxVal[0]!='\0' || pParamRule->sMinVal[0]!='\0')
		{
			 iRes = checkMinMax(pParamRule,sValue);
			 if(iRes!=0)
			 	return iRes;
		} else if(pParamRule->sEnumVal[0][0]!='\0')
		{
			iRes = checkEnum(pParamRule,sValue);
			if(iRes!=0)
			 	return iRes;
		}
		
		if(pParamRule->iEffDateFlag>0)
		{
			iRes = checkEffdate(pParamRule,sEffVal,iFlag);
			if(iRes!=0)
				return iRes;
		} else {
			 if(sEffVal[0] != '\0')
				return EFF_ERR_NONEED;
		}
		
		//不能取的值
		if(pParamRule->sDisobey[0] != '\0')
		{
			if(!sValue || sValue[0] == '\0')
				iRes = DISOBEY_ERR;
			else
				iRes = checkDisobey(pParamRule,sValue);
			if(iRes != 0)
				return iRes;
		}
	
		iRes = checkTime(pParamRule,sValue);
		if(iRes)
			return iRes;
		else
			return 0;//配置了规则,没找到对应项,算规则匹配成功
	
	 } else { //return 0[没配置规则],直接成功,配置了规则的按照规则判断
	 	return 0;	 
	 }
}

int BaseRuleMgr::checkDisobey(char *sSect,char *sParam,char *sVal)
{
	 BaseSysParamRule* pParamRule = getParamRule(sSect,sParam);
	 if(pParamRule)
	 {
	 	return checkDisobey(pParamRule,sVal);
	 }
	 return 0;
}


int BaseRuleMgr::checkDisobey(BaseSysParamRule *pParamRule,char *sVal)
{
	 if(pParamRule)
	 {
	 	if(pParamRule->sDisobey[0] != '\0')
		{
			if(!sVal || sVal[0]=='\0')
				return DISOBEY_ERR;
			for(int i=0;i<strlen(sVal);i++)
			{
				for(int j=0;j<strlen(pParamRule->sDisobey);j++)
				{
					if(sVal[i] == pParamRule->sDisobey[j])
					{
						return DISOBEY_ERR;
					}
				}
			}
		}
	 }
	 return 0;
}

int BaseRuleMgr::checkEffdate(char *sSect,char *sParam,char *sEffDate,int iFlag)
{
	 BaseSysParamRule* pParamRule = getParamRule(sSect,sParam);
	 if(pParamRule)
	 {
	 	return checkEffdate(pParamRule,sEffDate,iFlag);
	 }
	 return 0;
}

int BaseRuleMgr::checkEffdate(BaseSysParamRule *pParamRule,char *sEffDateSrc,int iFlag)
{
	 if(iFlag == 1)//不检测生效日期方式
	 	return 0;
	 int iMaxSize = strlen(sEffDateSrc);
	 char sEffDate[14] = {0};
	 strncpy(sEffDate,sEffDateSrc,sizeof(sEffDate));
	 for(int i=0;i<(14-iMaxSize);i++)
	 {
	 	strcat(sEffDate,"0");
	 }
	 Date srcDate(sEffDate);
	 if(pParamRule && pParamRule->iEffDateFlag)
	 {
			Date mDate;		
	 		switch(pParamRule->iEffDateFlag)
			{
				case 3:	//带生效日期				
						if(srcDate>mDate)
							return 0;
						else
							return EFF_ERR_LESS;
					    break;
				case 1: //重启立即生效
				case 2:
						if(sEffDateSrc && sEffDateSrc[0] != '\0')
						{
							return EFF_ERR_NONEED;
						}
						break;
				default:
						break;
				}
	 }
	 return 0;
}

/*
功能: 检测参数是否符合最大最小值设定
参数说明:sSect段名,sParam参数名
返回值:true是参数必定要存在的,false是不一定要存在
举例:sSect为SYSTEM sParam为location
配置方法举例: 在规则文件配置以下的数据
[SYSTEM]
location.need = 1	# 必须存在的参数
*/
int BaseRuleMgr::checkParamNeed(char *sSect,char *sParam)
{
	BaseSysParamRule* pParamRule = getParamRule(sSect,sParam);
	return checkParamNeed(pParamRule);
}

int BaseRuleMgr::checkParamNeed(BaseSysParamRule* pParamRule)
{
	if(!pParamRule)
		return PARAM_INFO_NONEED;
	if(pParamRule->iParamNeed)
	{
		   return 0;
	}
	return PARAM_INFO_NONEED;
}

/***************************************/
int BaseRuleMgr::checkMinMax(BaseSysParamRule* pParamRule,char *sPtr)
{
	if(pParamRule->sMaxVal[0]!='\0' || pParamRule->sMinVal[0]!='\0')
	{
			if(sPtr==NULL || sPtr[0] == '\0')
				return ISNOTNUM_ERR;
			if(!isRuleNumeric(sPtr))
			{
				return ISNOTNUM_ERR;
			}
			long lMax = 0;
			long lMin = 0;
			long lVal = 0;
			/*if(sPtr && sPtr[0]!='\0')
			{
				int i=0;
				if(sPtr[0] == '-')
				{
					i++;
				}
				while((sPtr[i]))
				{
					if(!isdigit(sPtr[i]))
						break;
					i++;
				}
				if(strlen(sPtr)>i)
					return MAX_ERR;
			}*/
			lVal = atol(sPtr);
			if(pParamRule->sMaxVal[0] == '\0' && pParamRule->sMinVal[0]!='\0')
			{
				lMin = atol(pParamRule->sMinVal);
				if(lVal<lMin){
					return MIN_ERR;
				}
				else 
					return 0;
			}
			else if(pParamRule->sMaxVal[0] != '\0' && pParamRule->sMinVal[0] == '\0')
			{
				lMax = atol(pParamRule->sMaxVal);
				if(lVal>lMax){
					return MAX_ERR;
				}
				else
					return 0;
			} else {//最大最小值都设置的
				lMax = atol(pParamRule->sMaxVal);
				lMin = atol(pParamRule->sMinVal);
				if(lMax>lMin)
				{
					if(lVal<=lMax && lVal>=lMin){
						return 0;
					} else {
						if(lVal>lMax)
							return MAX_ERR;
						else
							return MIN_ERR;
					}
				} else {
					return MAX_MIN_SET_ERR;
				}
			}
	} 
	return 0;
	
}

/*
功能: 检测参数是否符合参数值修改设定
参数说明:sSect段名,sParam参数名,sValue新值
返回值:true是参数可以修改
举例:sSect为SYSTEM sParam为num
配置方法举例: 在规则文件配置以下的数据
[SYSTEM]
num.change = 2	# 1-只能增加 //.change; 2-不能修改 ; 3-规则匹配	;//4-只能减小
*/
int BaseRuleMgr::checkValueChange(char *sSect,char *sParam,char *sValue,char *sOldVal)
{
	 BaseSysParamRule* pParamRule = getParamRule(sSect,sParam);
	 return checkValueChange(pParamRule,sValue,sOldVal);
}

int BaseRuleMgr::checkValueChange(BaseSysParamRule* pParamRule,char *sPtr,char *sOldVal)
{
 	if(pParamRule->iValueChange)
	{
		switch(pParamRule->iValueChange)
		{
				case 1:	
						if(!sPtr || sPtr[0] == '\0')
							return IN_CREASE_ERR;
						if(!sOldVal || sOldVal[0] == '\0'){
							return 0;
						}else{
							if(atol(sOldVal)>=atol(sPtr))
								return IN_CREASE_ERR;
							else 
								return 0;
						}
						break;
				case 4:				
						if(!sPtr || sPtr[0] == '\0')
							return DE_CREASE_ERR;	
						if(!sOldVal || sOldVal[0] == '\0'){
							return 0;
						}else{
							if(atol(sOldVal)<=atol(sPtr))
								return DE_CREASE_ERR;
							else
								return 0;
						}
						break;
				case 2:
						if(sPtr && sPtr[0] != '\0')
							return CHANGE_ERR;
				case 3:
				case 5:
						if(pParamRule->sValueChange[0] == '\0'){
							//未设置的匹配数据
							return 0;
						}
						if(!sPtr || sPtr[0] == '\0')
							return IN_CREASE_ERR;
						if(!MaskString(sPtr,pParamRule->sValueChange))
						{
							//设置值不符合设置的匹配数据
							return CHANGE_ERR;
						}
						if(pParamRule->iValueChange == 5)
						{
							vector<string> vec_tagChange;
							vec_tagChange.clear();
							getTagStringN(sPtr,vec_tagChange,'.');
							vector<string>::iterator itr = vec_tagChange.begin();
							for(;itr != vec_tagChange.end();itr++)
							{
								char *p = (char*)((*itr).c_str());
								for(int i = 0;i<strlen(p);i++)
								{
									if(!isdigit(p[i]))
									{
										return CHANGE_ERR;
									}
								}
								if(atoi(p) >=0 && atoi(p)<256)
								{
								} else {
									return CHANGE_ERR;
								}
							}	
						}
						break;
				default:
						break;
		 }
	}
	return 0;
}

int BaseRuleMgr::checkEnum(char *sSect,char *sParam,char *sValue)
{
	 BaseSysParamRule* pParamRule = getParamRule(sSect,sParam);
	 return checkEnum(pParamRule,sValue);
}

/*
功能: 检测参数是否符合枚举值设定
参数说明:sPtr是具体值
返回值:true是参数符合枚举要求
举例:sPtr为
配置方法举例: 在规则文件配置以下的数据
[SYSTEM]
location.need = 1	# 必须存在的参数
*/
int BaseRuleMgr::checkEnum(BaseSysParamRule* pParamRule,char *sPtr)
{
	if(pParamRule->sEnumVal[0][0]!='\0')
	{
		for(int i=0;i<pParamRule->iEnumPos;i++)
		{
			if(strcmp(pParamRule->sEnumVal[i],sPtr)==0)
				return 0;
		}
		//if(!m_bMonDebug)
		//	printf("%s\n","设置值不在规则允许范围!");
		return ENUM_ERR;
	}
	return 0;
}

int BaseRuleMgr::checkTime(char *sSect,char *sParam,char *sValue)
{
	 BaseSysParamRule* pParamRule = getParamRule(sSect,sParam);
	 return checkTime(pParamRule,sValue);
}


int BaseRuleMgr::checkTime(BaseSysParamRule* pParamRule,char *sPtr)
{
	if(pParamRule->sTime24Start[0]!='\0' && pParamRule->sTime24End[0]!='\0')
	{
			if(!sPtr || sPtr[0] == '\0')
				return TIME_ERR;
			if(strlen(pParamRule->sTime24Start) != strlen(pParamRule->sTime24End))
				return 0;//规则错了,不比较
			if(strlen(pParamRule->sTime24Start) != strlen(sPtr))
				return TIME_ERR;
			if(strcmp(pParamRule->sTime24Start,sPtr)<=0 && strcmp(pParamRule->sTime24End,sPtr)>=0)
			{
				return 0;
			} else {
				return TIME_ERR;
			}	
	}
	return 0;
}

//  检测参数是否是是只能增加或者只能减少
int BaseRuleMgr::checkInOrDecrease(const char* str)
{
	 if(!m_bLoadRule){
		return RULE_UNLOAD_ERR;
	 }
	 if(str[0] == '\0')
	 {
		return PARAM_FORMAT_ERR;
	 }
	 string sPureSect;
	 string sPureParam;
	 string sPureVal;
	 BaseSysParamRule* pParamRule = 0;
	 if(getEveryInfo(str,sPureSect,sPureParam,sPureVal))
	 {
	 	pParamRule = getParamRule((char*)sPureSect.c_str(),(char*)sPureParam.c_str());
	 } else {
	 	return -1;
	 }
	 if(pParamRule)
	 {
		return pParamRule->iValueChange;
	 }
	 return 0;
}

bool BaseRuleMgr::IfNeedLinkParam(char *sSect,char *sParam)
{
     if(sSect[0] != '\0'  && sParam[0] != '\0')
     {
        BaseSysParamRule* pParamRule = 0;
        pParamRule = getParamRule(sSect,sParam);
        if(pParamRule)
        {
            if(pParamRule->iLinkNum>0)
                return true;
            else
                return false;
        }
     }
     return false;
}

//新增GROUP
bool BaseRuleMgr::checkGroupParam(vector<string> str, bool bShow)
{
	bool res = false;
	//传首个参数的信息,然后根据首个参数
	BaseSysParamRule *pRule = 0;
	BaseVarInfo mBaseVarInfo;
	
	//先作转化算了 转成BaseVarInfo
	if(str.size()<=0)
		return false;
	if(!m_poIndex || !m_poGroupNoIndex ||!m_poGroupIndex)
		return true;//没规则就不判断了
	//
	for(int i=0;i<str.size();i++)
	{
		memset(&mBaseVarInfo,'\0',sizeof(BaseVarInfo));
		string strT = str[i];
		trimN(strT);
		int iPos = strT.find('.');
		string sPureSect;
		string sPureParam;
		if(iPos)//参数格式正确
		{
			sPureSect.assign(strT,0,iPos);
			strcpy(mBaseVarInfo.sSectName,(char*)sPureSect.c_str());
			//Chang2Upper(mBaseVarInfo.sSectName);
			sPureParam.assign(strT,iPos+1,strT.size());
			strcpy(mBaseVarInfo.sParamName,(char*)sPureParam.c_str());
			//Chang2Lower(mBaseVarInfo.sParamName);
			BaseSysParamRule *pTmp = 0;
			char sFullFName[MAX_SECTION_LEN+MAX_KEY_NAME_LEN+1] = {0};
			sprintf(sFullFName,"%s.%s",mBaseVarInfo.sSectName,mBaseVarInfo.sParamName);
			if(m_poIndex->get(sFullFName,&pTmp))
			{
				//有规则就继续
				if(pTmp->sGroupParamName[0] != '\0')
				{
					/*if(str.size() == 1)
						return false;*/
					char sKey[MAX_SECTION_LEN+MAX_KEY_NAME_LEN+1] = {0};
					BaseVarInfo *pVar = 0;
					if(m_poGroupIndex->get(pTmp->sGroupParamName,&pVar))
					{
						
						while(pVar)
						{
							sprintf(sKey,"%s.%s",pVar->sSectName,pVar->sParamName);
							if(strcmp(sKey,sFullFName)!=0 )
							{
								if(! searchGroupParam(str,sKey,i)){
									if(bShow)
										printf("%s%s%s%s%s\n","GROUP[",pTmp->sGroupParamName,"]中必要参数[",sKey,"]此次未赋值!");
									return false;
								}
								pVar = pVar->next;
							} else {
								pVar = pVar->next;
							}
						}
					}
				}
				return true;
			} else {
				return true;//没找到规则算成功
			}
		} else {
			if(bShow)
				printf("%s%s%s\n","参数[",(char*)strT.c_str(),"]格式检验失败!");
			return false;
		}
	}
	return false;//格式异常 错误
}

//捞一把数据看看有没有要找的
bool BaseRuleMgr::searchGroupParam(vector<string> str, string strcmp,int iNoCmpPos)
{
	//传首个参数的信息,然后根据首个参数
	BaseSysParamRule *pRule = 0;
	BaseVarInfo mBaseVarInfo;
	for(int i = 0;i<str.size();i++)
	{
		if( i!=iNoCmpPos && str[i] == strcmp)
			return true;
	}
	return false;//否则不判断 直接算成功
}
//iPos是从1开始
char *BaseRuleMgr::getLinkParam(char *sSect,char *sParam,int iPos)
{
     if(sSect[0] != '\0'  && sParam[0] != '\0' && iPos>0)
     {
        BaseSysParamRule* pParamRule = 0;
        pParamRule = getParamRule(sSect,sParam);
        if(pParamRule)
            return getLinkParam(pParamRule,iPos);
        else
            return NULL;
     }
     return NULL;
}
char *BaseRuleMgr::getLinkParam(BaseSysParamRule *pParamRule,int iPos)
{
     if(pParamRule)
     {
        if(pParamRule->iLinkNum>=iPos)
            return pParamRule->sLinkParamName[iPos-1];
        else
            return false;
     }
     return false;
}
//true就是需要仲裁的参数
bool BaseRuleMgr::neddArbi(char *sSect,char *sParam)
{
	 if(sParam[0] == '\0' || sSect[0] == '\0')
	 	return false;
	 BaseSysParamRule* pParamRule = getParamRule(sSect,sParam);
	 if(pParamRule)
	 {
	 	if(pParamRule->cArbi == 'Y')
		{
			return true;
		}
	 }
	 return false;
}

//str格式 段.参数=参数值  举例:SYSTEM.location=beijing这样的格式,获得段名SYSTEM,参数名location,参数值beijing
int BaseRuleMgr::getEveryInfo(const char* str,string &sPureSect,string &sPureParam,string &sPureVal)
{
	 string sParam = str;
	 trimN(sParam);
	 int iValPos = sParam.rfind('=');
	 int iParamPos = sParam.find('.');
	 if(iParamPos<0){
		return -1;
	 }
	 sPureSect.clear();
	 sPureParam.clear();
	 sPureVal.clear();
	 if(iValPos<0)
	 {
	 	sPureSect.assign(sParam,0,iParamPos);
		sPureParam.assign(sParam,iParamPos+1,sParam.size());
	 } else {
	 	sPureSect.assign(sParam,0,iParamPos);
	 	sPureParam.assign(sParam,iParamPos+1,iValPos-iParamPos-1);
	 	sPureVal.assign(sParam,iValPos+1,sParam.size());	 		 	
	 }
	 trimN(sPureSect);
	 trimN(sPureParam);
	 trimN(sPureVal);
	 return 1;
}

//  检测参数是否是是只能增加或者只能减少
int BaseRuleMgr::showInOrDecrease(char* sSect,char *sParam)
{
	 BaseSysParamRule* pParamRule = getParamRule(sSect,sParam);
	 if(pParamRule)
	 {
		return pParamRule->iValueChange;
	 }
	 return 0;
}

bool BaseRuleMgr::IsGroupParam(char *section,char *param)
{
	 BaseSysParamRule *pTmp = 0;
	 char sFullFName[MAX_SECTION_LEN+MAX_KEY_NAME_LEN+1] = {0};
	 sprintf(sFullFName,"%s.%s",section,param);
	 if(m_poIndex && m_poIndex->get(sFullFName,&pTmp))
	 {
	 	//有规则就继续
	 	if(pTmp->sGroupParamName[0] != '\0')
	 	{
	 		/*char sKey[MAX_SECTION_LEN+MAX_KEY_NAME_LEN+1] = {0};
			BaseVarInfo *pVar = 0;
			if(m_poGroupIndex->get(pTmp->sGroupParamName,&pVar))
			{
				
			}*/
			return true;
		}
	 }
	 return false;
}

int BaseRuleMgr::CheckDigitString(char *value)
{
	 if(!value || value[0] == '\0')
	 	return 0;//
	 for(int i = 0;i<strlen(value);i++)
	 {
	 	 if(i==0)
		 {
		 	if(isdigit(value[i])==0)
			{
		 		if(!(value[i] == '-'))
					return CHAR_OR_NUM_ERR;
			}
		 } else {
		 	if(isdigit(value[i])==0)
				return CHAR_OR_NUM_ERR;
		 }
	 }
	 return 0;
}

int BaseRuleMgr::CheckDigitString(BaseSysParamRule* pParamRule, char *value)
{
	if(pParamRule)
    {
		switch(pParamRule->iCharOrNum)
		{
			case 1: //num
			{
				return CheckDigitString(value);
				break;
			}
			default:
			{
				return 0;
			}
		}
	}
	return 0;
}

int BaseRuleMgr::CheckDigitString(char *section, char *param, char *value)
{
	if(section[0] != '\0'  && param[0] != '\0')
    {
        BaseSysParamRule* pParamRule = 0;
        pParamRule = getParamRule(section,param);
		if(!pParamRule)
			return 0;
		switch(pParamRule->iCharOrNum)
		{
			case 1:
			{
				return CheckDigitString(value);
				break;
			}
			default:
			{
				return 0;
			}
		}
	}
	return 0;
}
bool BaseRuleMgr::isRuleNumeric(char* pString)
{
	if(0== pString)
		return false;
	int i=0;
	for( ;i<strlen(pString);i++){
		if((i==0)&&(pString[0]=='-'))
			continue;
		else{
			if(isdigit(pString[i]))
				continue;
			else
				break;
		}
	}
	if(i<strlen(pString))
		return false;
	return true;
}
