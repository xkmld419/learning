#include "SplitMgr.h"
#include "Environment.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

HashList<SplitCfg *>  *SplitMgr::m_poSplitIndex = 0;
bool SplitMgr::bInit=false;
SplitCfg * SplitMgr::m_poSplitData = 0;

SplitMgr::SplitMgr()
{
    bInit=false;
}

SplitMgr::~SplitMgr()
{
#ifdef PRIVATE_MEMORY	
	if(m_poSplitIndex){
		delete m_poSplitIndex;
		m_poSplitIndex = NULL;
	}
	if(m_poSplitData){
		delete [] m_poSplitData;
		m_poSplitData = NULL;
	}
	bInit = false;
#endif	
}

#ifdef PRIVATE_MEMORY
bool SplitMgr::queryBySplitID (int iSplitID,SplitCfg * pCfg)
{
	if(bInit==false){
		load();
		bInit=true;
	}
	SplitCfg * cfg = 0;
	if(m_poSplitIndex->get(iSplitID,&cfg)){
		memcpy(pCfg,cfg,sizeof(SplitCfg));
		return true;
	}
	return false;
}
#else
bool SplitMgr::queryBySplitID (int iSplitID,SplitCfg * pCfg)
{
	SplitCfg * cfg =0;
	unsigned int iCfg = 0;
	if(G_PPARAMINFO->m_poSplitIndex->get(iSplitID,&iCfg)){
	    cfg = &(G_PPARAMINFO->m_poSplitCfgList[iCfg]);
		memcpy(pCfg,cfg,sizeof(SplitCfg));
		return true;
	}
	return false;
}
#endif
//取pVal的后面部分以SplitCfg->m_sStartVal的长度为基准比较
#ifdef PRIVATE_MEMORY
bool SplitMgr::checkBySplitID (int iSplitID,char * pVal)
{
	if (NULL == pVal) return false;

	if(bInit==false){
		load();
		bInit=true;
	}
	SplitCfg * pCfg =0;
	if(m_poSplitIndex->get(iSplitID,&pCfg)){
		char * p = pVal;
		int len = strlen(pCfg->m_sStartVal);
		int iStrLen = strlen(pVal);
		if (iStrLen > len){
			p = pVal + (iStrLen-len);
		}
		if(strncmp(p,pCfg->m_sStartVal,len)>=0 && 
			strncmp(p,pCfg->m_sEndVal,len)<=0){
			return true;
		}
	}
	return false;
}
#else
bool SplitMgr::checkBySplitID (int iSplitID,char * pVal)
{
	SplitCfg *m_poSplitCfg = (SplitCfg*)(*G_PPARAMINFO->m_poSplitData);
	SplitCfg * pCfg =0;
	unsigned int iCfg = 0;
	if(G_PPARAMINFO->m_poSplitIndex->get(iSplitID,&iCfg)){
	    pCfg = &(G_PPARAMINFO->m_poSplitCfgList[iCfg]);
		int len = strlen(pCfg->m_sStartVal);
		if(strncmp(pVal,pCfg->m_sStartVal,len)>=0 && 
			strncmp(pVal,pCfg->m_sEndVal,len)<=0){
			return true;
		}
	}
	return false;
}
#endif
void  SplitMgr::load ()
{
	int iCount = 0,m=0;
	m_poSplitIndex->clearAll();
	SplitCfg * pSplitCfgTemp;

	TOCIQuery qry(Environment::getDBConn());
	qry.setSQL("select count(1) from cfg_split_attr");
	qry.open();
	if(qry.next()){
		iCount = qry.field(0).asInteger();
	}
	qry.close();
	qry.commit();

	iCount = iCount+1;
	m_poSplitData = new SplitCfg[iCount];
	m_poSplitIndex = new HashList<SplitCfg *>(iCount);
    
    qry.setSQL("select split_id,split_type,start_val,end_val from cfg_split_attr");
    qry.open();
	while(qry.next())
	{
		pSplitCfgTemp = m_poSplitData+m;
		pSplitCfgTemp->m_iSplitType=qry.field(1).asInteger();
		strcpy(pSplitCfgTemp->m_sStartVal,qry.field(2).asString());
		strcpy(pSplitCfgTemp->m_sEndVal,qry.field(3).asString());
		m_poSplitIndex->add(qry.field(0).asInteger(),pSplitCfgTemp);
		m++;
	}
	qry.close();
	qry.commit();
}
