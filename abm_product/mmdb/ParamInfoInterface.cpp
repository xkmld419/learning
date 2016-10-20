#ifndef PRIVATE_MEMORY

#include "ParamInfoInterface.h"
#include "ParamInfoMgr.h"
#include "Log.h"
#include "MBC.h"


ParamInfoInterface::ParamInfoInterface()
{
    if(linkDataAccessShm() == false)
    {
        Log::log(0, "参数的共享内存不存在,请先创建共享内存!");
        THROW(MBC_ParamInfoInterface+1);
    }

    //根据参数信息块的编号连接对应的参数块内的参数共享内存访问链表
    //这里绑定链表只要有一个失败,会导致所有的链表都无法访问
    m_pParamDataBuf[PARAM_SHM_INDEX_A]->bindData();
    m_pParamDataBuf[PARAM_SHM_INDEX_B]->bindData();
}

ParamInfoInterface::~ParamInfoInterface()
{
    
}

bool ParamInfoInterface::reBindData()
{
	 if(m_pParamDataBuf[PARAM_SHM_INDEX_A])
	 {
		bool res = m_pParamDataBuf[PARAM_SHM_INDEX_A]->detachAll();
		if(!res)
		{
		     Log::log(0,"this process detach block_0  err!"); 
		}
		delete m_pParamDataBuf[PARAM_SHM_INDEX_A];
		m_pParamDataBuf[PARAM_SHM_INDEX_A] = 0;
	 }
	 m_pParamDataBuf[PARAM_SHM_INDEX_A] = new ParamInfoMgr(PARAM_SHM_INDEX_A);
	 m_pParamDataBuf[PARAM_SHM_INDEX_A]->bindKey(PARAM_SHM_INDEX_A);
	 m_pParamDataBuf[PARAM_SHM_INDEX_A]->attachAll();
	 m_pParamDataBuf[PARAM_SHM_INDEX_A]->bindData();
									 
	 if(m_pParamDataBuf[PARAM_SHM_INDEX_B])
	 {
		bool res = m_pParamDataBuf[PARAM_SHM_INDEX_B]->detachAll();
		if(!res)
		{
		    Log::log(0,"this process detach block_1 err!"); 
		}
		delete m_pParamDataBuf[PARAM_SHM_INDEX_B];
	    m_pParamDataBuf[PARAM_SHM_INDEX_B]= 0;
	 }
	 m_pParamDataBuf[PARAM_SHM_INDEX_B] = new ParamInfoMgr(PARAM_SHM_INDEX_B);
	 m_pParamDataBuf[PARAM_SHM_INDEX_B]->bindKey(PARAM_SHM_INDEX_B);
	 m_pParamDataBuf[PARAM_SHM_INDEX_B]->attachAll();
	 m_pParamDataBuf[PARAM_SHM_INDEX_B]->bindData();
	 return true;
}

// 
bool ParamInfoInterface::updateInfo(int iPID,long lDate,int iFlashTimes,char *sDesc,int iProcessID)
{
     bool bRes = true;
     bool updateInfo = false;	
     char sql[2048] = {0};
     DEFINE_QUERY(qry);
     qry.close();
	 if(iProcessID>0)
     	sprintf(sql,"%s%d%s%ld","select count(*) from b_param_flash_info where process_id=",iProcessID);
	 else
	 	sprintf(sql,"%s%d%s%ld","select count(*) from b_param_flash_info where pid=",iPID," and date_flag=",lDate);
     qry.setSQL(sql);
     qry.open();
     if(qry.next())
     {
		int iCount = qry.field(0).asInteger();
		if(iCount>0)
			updateInfo = true;
     }
     qry.close();
     sql[0] = '\0';
     if(updateInfo)
     {
	 	if(iProcessID>0)
			sprintf(sql,"update b_param_flash_info set flash_flag=%d,pid=%d,date_flag=%ld where process_id=%d",iFlashTimes,iPID,lDate,iProcessID); 
		else
	sprintf(sql,"update b_param_flash_info set flash_flag=%d,info_desc='%s' where pid=%d and date_flag=%ld",iFlashTimes,"update",iPID,lDate); 
     } else {
	sprintf(sql,"insert into b_param_flash_info (pid,date_flag,process_id,flash_flag,info_desc) values (%d,%ld,%d,%d,'%s')",iPID,lDate,iProcessID,iFlashTimes,sDesc);
     }
     qry.setSQL(sql); 
     try{       
         if(qry.execute())
	 {
		qry.commit();
	 } else {
		qry.rollback();
		bRes = false;
	 }
	 qry.close();
     }catch(TOCIException& e) 
     {
	 Log::log(0,"commit data err ,table name [b_param_flash_info]"); 
	 qry.close(); 
	 return false;
     }
	 
     return bRes;
}

bool ParamInfoInterface::checkTable()
{
	 int pid,processID,dateflag,flash=0;
     char sql[2048] = {0};
     DEFINE_QUERY(qry);
     qry.close();
     sprintf(sql,"%s","select pid,date_flag,nvl(process_id,0),flash_flag from b_param_flash_info order by date_flag desc");
     qry.setSQL(sql);
     qry.open();
     if(qry.next())
     {
		pid = qry.field(0).asInteger();
		dateflag = qry.field(1).asInteger();
		if(pid>0)
		{
			//获取数据
			processID = qry.field(2).asInteger();
			
	 	}
		
     }
     qry.close();
}

#endif
