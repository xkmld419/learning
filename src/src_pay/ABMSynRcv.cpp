#include "ABMSynRcv.h"
#include "errorcode.h"
#include "ABMException.h"
#include "LogV2.h"
#include "PublicConndb.h"

static vector<QryServSynData *> g_vData;					//资料同步结果应答
static vector<QryServSynData *>::iterator g_its;	
	
static char cPreSessionId[100]={0};

ABMSynRcv::ABMSynRcv()
{
		m_poUnpack = NULL;
		m_poSql = NULL;
		m_dccOperation = new DccOperation;	
}

ABMSynRcv::~ABMSynRcv()
{
	if (m_dccOperation != NULL)
		delete m_dccOperation;
	if (m_poSql != NULL)
        delete m_poSql;
}

int ABMSynRcv::init(ABMException &oExp)
{
	 if ((m_poSql=new QryServSynSql) == NULL) {
        ADD_EXCEPT0(oExp, "ABMSynRcv::init malloc failed!");
        return -1;
    }
    return 0;
}

int ABMSynRcv::deal(ABMCCR *pRecivePack, ABMCCA *pSendPack)
{
	__DEBUG_LOG0_(0, "=========>ABMSynRcv::deal 响应服务开始");
	__DEBUG_LOG0_(0, "=========>1：接收全国HSS返回的CCA...");
	
	int iRet=0;
	int iSize=0;
	
	// 获取pRecivePack的公共CCA消息头
	char sSessionId[64];
	struDccHead Head;

	char dccType[1];	// DCC消息类型
	strcpy(dccType,"A"); 
	long lDccRecordSeq=0L; // 流水号，主键
	
		// 取消息包的消息头信息
		memcpy((void *)&Head,(void *)(pRecivePack->m_sDccBuf),sizeof(Head));
		// 收到CCA包Session-Id
		__DEBUG_LOG1_(0, "=======>收到全国HSS返回的CCA的消息头，SESSION_ID:[%s]",Head.m_sSessionId);
		strcpy(cPreSessionId,Head.m_sSessionId);
		
		//解包
		m_poUnpack = (QryServSynCCA *)pRecivePack;
		
		 if((iRet = m_poUnpack->getServSyn(g_vData)) == 0){
		         __DEBUG_LOG0_(0, "ABMSynRcv:: deal 消息包为NULL!");
		         return INFO_SYN_UNPACK_ERR;
		}
		else if (iRet < 0)
		{
			__DEBUG_LOG0_(0, "ABMSynRcv:: deal 消息包解包出错!");	
			return INFO_SYN_UNPACK_ERR;
		}
		
	try
	{
		//CCA解包处理
		for(int i = 0; i <g_vData.size(); ++i)
		{
			if(g_vData[i]->m_iRltCode == 0){
			iRet = _deal(g_vData[i],cPreSessionId);
			__DEBUG_LOG1_(0, "ABMSynRcv::deal iRet=%d", iRet);
			}
			else{
			iRet = _dealErr(g_vData[i],cPreSessionId);
			__DEBUG_LOG1_(0, "ABMSynRcv::dealErr iRet=%d", iRet);
			}
				
			m_poUnpack->clear();
		}
	}
	catch(TTStatus oSt)
	{
		__DEBUG_LOG1_(0, "ABMSynRcv::deal oSt.err_msg=%s", oSt.err_msg);
	}
	return iRet;
}

int ABMSynRcv::_deal(QryServSynData *pData,char *m_oSessionId)
{
	__DEBUG_LOG0_(0, "enter ABMSynRcv::_deal" );
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
		iRet = INFOSYN_TT_ERR;
		__DEBUG_LOG1_(0, "开户资料同步失败，错误信息=%s", oSt.err_msg);
	}
	catch(int &value)
	{
		iRet = value;
		__DEBUG_LOG1_(0, "开户资料同步失败，错误信息=%d",value);
	}
   return iRet;
}

int ABMSynRcv::_dealErr(QryServSynData *pData,char *m_oSessionId)
{
	__DEBUG_LOG0_(0, "enter ABMSynRcv::_dealErr" );
	ABMException oExp;
    int iRet=0;
    
   //业务处理
   try
   {
   		//HSS端返回错误号入TT
   		iRet = m_poSql->insertUserAddErr(m_oSessionId,pData->m_iRltCode,oExp);
   		if(iRet != 0)
		{
			__DEBUG_LOG0_(0, "=====>insertUserAddErr error");
			throw iRet;
		}
   }
   catch(TTStatus oSt)
	{
		iRet = INFOSYN_TT_ERR;
		__DEBUG_LOG1_(0, "开户资料同步入错误信息失败，错误信息=%s", oSt.err_msg);
	}
	catch(int &value)
	{
		iRet = value;
		__DEBUG_LOG1_(0, "开户资料同步入错误信息失败，错误信息=%d",value);
	}
   return iRet;
}
