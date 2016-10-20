#include "QryServSynRcv.h"

static vector<QryServSynData *> g_vData;					//资料同步结果应答
static vector<QryServSynData *>::iterator g_its;	

QryServSynRcv::QryServSynRcv()
{
    m_poSql = NULL;
    //oCCA = NULL;
    //oCCR = NULL;
}

QryServSynRcv:: ~QryServSynRcv()
{
    if (m_poSql != NULL)
        delete m_poSql;
}

int QryServSynRcv::init(ABMException &oExp)
{
	 if ((m_poSql=new QryServSynSql) == NULL) {
        ADD_EXCEPT0(oExp, "QryServInfor::init malloc failed!");
        return -1;
    }
/*
    if (m_poSql->init(oExp) != 0) {
        ADD_EXCEPT0(oExp, "QryServInfor::init m_poSql->init failed!");
        return -1;
    }
*/
    return 0;
}

int QryServSynRcv::deal(QryServSynCCA *pCCA)
{
	int iRet;
	if ((pCCA==NULL))
        return -1;
	oCCA = (QryServSynCCA *)pCCA;
	struDccHead *pHead;
	pHead = (struDccHead *)oCCA->m_sDccBuf;
	g_vData.clear();
	
	//解包
   if((iRet = oCCA->getServSyn(g_vData)) == 0){
		          __DEBUG_LOG0_(0, "QryServSyn:: _deal 消息包为NULL!");
		         return -1;
	}
	else if (iRet < 0)
	{
		__DEBUG_LOG0_(0, "QryServSyn:: _deal 消息包解包出错!");	
	}
	for(int i = 0; i <g_vData.size(); ++i)
	{
		try{
			if(g_vData[i]->m_iRltCode == 0){
			iRet = _deal(g_vData[i],pHead->m_sSessionId);
			__DEBUG_LOG1_(0, "QryServSynRcv::deal iRet=%d", iRet);
			}
			else
				continue;
				oCCA->clear();
		}
		catch(TTStatus oSt)
		{
			__DEBUG_LOG1_(0, "QryServSynRcv::deal oSt.err_msg=%s", oSt.err_msg);
			oCCA->clear();
		}	
	}
	return iRet;
}

int QryServSynRcv::_deal(QryServSynData *pData,char *m_oSessionId)
{
	ABMException oExp;
    int iRet=0;
   
   //业务处理:判断返回结果代码
   try{
   		//拼短信内容写短信工单表
	iRet = m_poSql->insertSendSql(m_oSessionId,oExp);
	if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "=====>insertSendSql error");
			throw iRet;
		}
   //用户增量表置同步完成状态  
	iRet = m_poSql->updateServAddState(m_oSessionId,oExp);
	if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "=====>deleteServAdd error");
			throw iRet;
		}
	}
	catch(TTStatus oSt)
	{
		iRet =-1;
		__DEBUG_LOG1_(0, "开户资料同步失败，错误信息=%s", oSt.err_msg);
	}
	catch(int &value)
	{
		iRet = value;
		__DEBUG_LOG1_(0, "开户资料同步失败，错误信息=%d",value);
	}
   //业务完成后
   oCCA->clear();
   
   return iRet;
}