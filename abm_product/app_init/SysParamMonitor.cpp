#include "SysParamMonitor.h"
#include "MsgDefineClient.h"
#include <ctype.h>
#include "Log.h"
#include "MBC.h"


SysParamMonitor::SysParamMonitor()
{
	m_poCmdCom = 0;
	m_iWaitTime = 10;
	pIndexList  = 0;
	m_poSysParamHis = 0;
	m_poBaseRuleMgr = new BaseRuleMgr();
}

SysParamMonitor::~SysParamMonitor()
{
	if(m_poSysParamHis)
		delete []m_poSysParamHis;
	m_poSysParamHis = 0;
	if(m_poBaseRuleMgr)
		delete m_poBaseRuleMgr;
	m_poBaseRuleMgr = 0;
}



// 初始化基本参数,调用处理函数之前必须要初始化这些参数
void SysParamMonitor::init(int iWaitTime,CommandCom *pCmdCom)
{
     m_poCmdCom = pCmdCom;
     m_iWaitTime = iWaitTime*10+1;
}

// 获取参数在monitor显示的时候的行数值
int SysParamMonitor::getRows()
{
	return m_iRowNum;
}

int SysParamMonitor::dealParamOpera(char *sSectName,char *sParamVal, long lCMDID)
{
	int res = dealWithMem(sSectName , sParamVal , lCMDID);
	switch(lCMDID)
	{
		case MSG_PARAM_CHANGE:
		case MSG_PARAM_S_C:
		case MSG_PARAM_S_D:	
		{					
			m_iRowNum = 0;
			return res;
		}
		default:
		{
			res = GetParamInfoAll();
			return res;
		}
	}
}

SysParamHis* SysParamMonitor::getParamHisByOffset(int iOffset)
{
	if(m_poSysParamHis && iOffset>=0 && iOffset< MAX_ITEMS && iOffset<m_iCnt)
		return &m_poSysParamHis[iOffset];
	else 
		return NULL;
}

/*
函数:dealWithMem
参数:sSectName段, sSysParamValue参数内容, lCMDID命令ID
返回值: 0-失败 1-成功(创建,修改,删除是这样的) list的是返回获取的参数数目
*/
int SysParamMonitor::dealWithMem(char *sSectName, char *sSysParamValue, long lCMDID)
{
	if(!m_poCmdCom){
		return 0;
	}
	m_iCnt = 0;
	if(m_poSysParamHis)
		delete []m_poSysParamHis;
	m_poSysParamHis = 0;
	
	switch(lCMDID)
	{
		case MSG_PARAM_CHANGE:
		{
			if(!sSysParamValue || sSysParamValue[0] == '\0')
				return 0;
			string strParamVal(sSysParamValue);
			if(!m_poCmdCom->changeSectionKey(strParamVal))
			{
				return 0;
			}
			break;
		}
		case MSG_PARAM_S_ALL:
		{
			if(!m_poCmdCom->getSection(NULL,&m_poSysParamHis,m_iCnt))
			{
					return 0;
			}
			break;
		}
		case MSG_PARAM_ACTIVE_MOUDLE_S:
		{
			if(!m_poCmdCom->FlashMemory(&m_poSysParamHis,m_iCnt))
			{
				return 0;
			}
			break;
		}
		case MSG_PARAM_S_C:
		{
			if(!sSysParamValue || sSysParamValue[0] == '\0')
				return 0;
			string strParamVal(sSysParamValue);
			string createstr;
			createstr = "#create ";
			createstr += strParamVal;
			if(!m_poCmdCom->changeSectionKey(createstr));
				return 0;
			break;
		}
		case MSG_PARAM_S_D:
		{
			if(!sSysParamValue || sSysParamValue[0] == '\0')
				return 0;
			string strParamVal(sSysParamValue);
			string delstr;
			delstr = "#delete  ";
			delstr += strParamVal;
			if(!m_poCmdCom->changeSectionKey(delstr));
				return 0;
			break;
		}
		case MSG_LIST_SECTION:
		{
			//if(!m_poCmdCom->getAllSections(&m_poSysParamHis,m_iCnt))
			//{
			//	return 0;
			//}
			break;
		}
		case MSG_PARAM_S_N:
		{
			if(!sSysParamValue || sSysParamValue[0] == '\0')
				return 0;
			string strParamVal(sSysParamValue);
			int iRes = strParamVal.find('.');
			if(iRes>0)
			{
				string strTmp =strParamVal;
				strTmp.erase(0,strTmp.find_first_of('.')+1);
				trimN(strTmp);
				//-list SYSTEM.*等同 -list SYSTEM
				if(strTmp.size()>0 && strTmp.compare("*")==0)
				{
					if(!m_poCmdCom->getSection(sSectName,&m_poSysParamHis,m_iCnt))
					{
						return 0;
					}
				} else {
					m_poSysParamHis = new SysParamHis[1];
					if(!m_poCmdCom->getSectionParam(sSectName,strTmp.c_str(),*m_poSysParamHis))
					{
						return 0;
					}else {
						if(m_poSysParamHis->sSectionName[0] == '\0'){
							return 0;
						}
						m_iCnt=1;
					}
				}
			} else {				
				if(!m_poCmdCom->getSection(sSectName,&m_poSysParamHis,m_iCnt))
				{
					return 0;
				}
			}
			break;
		}
		default:
		{
			return 0;
		}
	}
	return 1;
}

//  获取参数(参数转化,直接按照 段 参数=值 生效日期的格式显示)[改进中]
int SysParamMonitor::GetParamInfoAll()
{
	if(!m_poSysParamHis)
	{
		m_iRowNum = 0;
		return m_iRowNum;
	}
	m_iRowNum = 0;
	if(pIndexList)
		pIndexList->clearAll();
	 else 
		pIndexList = new HashList<int>(MAX_ITEMS*4+16);
	
	//  初始行数据下标
	int iListPos = 0;
	char sWholeParamName[MAX_SECTION_NAME_LEN + MAX_PARAM_NAME_LEN + 1] = {0};
	for(int iKey = 0;iKey<m_iCnt; iKey++) 
	{
	  if(m_poSysParamHis[iKey].sSectionName[0] == '\0')
	  	continue;
	  sprintf(sWholeParamName,"%s.%s",m_poSysParamHis[iKey].sSectionName,m_poSysParamHis[iKey].sParamName);
	  if(m_poSysParamHis[iKey].sParamValue[0] != '\0' && m_poSysParamHis[iKey].sParamOrValue[0]!='\0' && strcmp(m_poSysParamHis[iKey].sParamOrValue,m_poSysParamHis[iKey].sParamValue)!=0)
	  {
		 analyseParamInfo(iListPos,sWholeParamName,m_poSysParamHis[iKey].sParamOrValue,"原值=",iKey);
		 analyseParamInfo(iListPos,sWholeParamName,m_poSysParamHis[iKey].sParamValue," 新值=",iKey);	 
	  } else {
	  	analyseParamInfo(iListPos,sWholeParamName,m_poSysParamHis[iKey].sParamValue,"=",iKey);		
	  }
	  if(m_poSysParamHis[iKey].sDate[0] != '\0')
	  {
	  	analyseParamInfo(iListPos,sWholeParamName,m_poSysParamHis[iKey].sDate,"生效时间: ",iKey);
	  }
	  try
	  {
	  	if(!m_poBaseRuleMgr)
			m_poBaseRuleMgr = new BaseRuleMgr();
	  } catch (...)
	  {	
	  	m_poBaseRuleMgr = 0;		 	
	  }
	  if(m_poBaseRuleMgr)
	  {
	  		char sEffType[64] = {0};
			BaseSysParamRule *pParamRule = m_poBaseRuleMgr->getParamRule(m_poSysParamHis[iKey].sSectionName,m_poSysParamHis[iKey].sParamName);
			if(pParamRule)
			{
				switch(pParamRule->iEffDateFlag)
				{
					case 1:
						strcpy(sEffType,"立即生效");
						break;
					case 2:
						strcpy(sEffType,"重启生效");
						break;
					case 3:
						strcpy(sEffType,"按照生效日期生效");
						break;
					default:
						strcpy(sEffType,"重启生效");
						break;
			 	}
		  	} else {
		  		 strcpy(sEffType,"重启生效");
		  	}
		  	analyseParamInfo(iListPos,sWholeParamName,sEffType,"生效方式: ",iKey);
	  }	
	}
	m_iRowNum = iListPos;	
    return m_iRowNum;
}

//  行字符数目,data的开始下标;同时建立index与data的联系,sStrAdd:要在参数前增加的字符,如'.OldValue='
int SysParamMonitor::analyseParamInfo(int &iListPos,char *sParamName,char *sParam,char *sStrAdd,int iSysParamKey,int type)
{
	if(sParam[0] == '\0' || sParamName[0] == '\0') 
		return 0;
	if(iListPos >= MAX_ITEMS)
		return iListPos;
	char sMaxParam[2048] = {0};
	bool line2 = false;
	if(strlen(sParamName)+strlen(sStrAdd)+strlen(sParam) <= SYS_PARAM_LINE_VAL){
		sprintf(sMaxParam,"%s%s%s\0",sParamName,sStrAdd,sParam);
	} else {
		sprintf(sMaxParam,"%s\0",sParamName);
		line2 = true;
	}
	int iMaxParamStrlen = strlen(sMaxParam); //获取字符长值
	int iNeedRow = 0; // 总需要的行数
	
	iNeedRow = iMaxParamStrlen/SYS_PARAM_LINE_VAL;
	int iMod = iMaxParamStrlen%SYS_PARAM_LINE_VAL;
	if(iMod)
		iNeedRow++;
	int iNL = 0;
	for(; iNL<iNeedRow; iNL++)
	{
		if(iListPos >= MAX_ITEMS)
			return iListPos;
		memset(param_list_data[iListPos],'\0',MAX_PARAM_VAL_LEN);
		strncpy(param_list_data[iListPos],&sMaxParam[iNL*SYS_PARAM_LINE_VAL],sizeof(char)*SYS_PARAM_LINE_VAL);
		//int len = strlen(param_list_data[iListPos]);
		//if(is_zh_ch(param_list_data[iListPos][len-1]) == 1)
		//	param_list_data[iListPos][len-1] = '\0';
		param_list_index[iListPos] = param_list_data[iListPos];
		pIndexList->add(iListPos,iSysParamKey);
		iListPos++;				
	}
	if(line2)
	{
		if(iListPos >= MAX_ITEMS)
			return iListPos;
		char sMaxParam2[SYS_PARAM_LINE_VAL+31] = {0};
		sprintf(sMaxParam2,"%s%s\0",sStrAdd,sParam);
		strncpy(param_list_data[iListPos],sMaxParam2,sizeof(char)*SYS_PARAM_LINE_VAL);
		param_list_index[iListPos] = param_list_data[iListPos];
		pIndexList->add(iListPos,iSysParamKey);
		iListPos++;
	}
	return iListPos;
}

/********
检查字符p是否是双字节汉字的一个字节
返回值： 0:  是汉字 1:  不是汉字
********/
int SysParamMonitor::is_zh_ch(char p)
{
        /*if(~(p >> 8) == 0)
        {
                return(0);        
        }
        return(1);*/
		if((p&0x80)!=0) 
		{
			return 0;
		} else {
			return 1;
		}
		
}


//
SysParamHis *SysParamMonitor::getParamRecordByLine(int iLine)
{
    int iOffset = -1;
	if(NULL == pIndexList)
		return NULL;
    if(pIndexList->get(iLine,&iOffset))
    {
		if(m_poSysParamHis && iOffset<m_iCnt)
        	return &m_poSysParamHis[iOffset];
    }
    return NULL;
}

