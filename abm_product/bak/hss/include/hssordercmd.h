/*
*   version js at 2011-5-18
*/


#ifndef    __HSSCMDORDER_H_INCLUDED_
#define    __HSSCMDORDER_H_INCLUDED_

#include "hssobject.h"
#include "StdAfx.h"

using namespace std;



struct DccServiceInfo
{
	int m_iSize;
	int m_iSendTimes;
	long m_lOrderID;
	long m_lActionType;
    char m_sAcceptDate[16];
    char m_sCompleteDate[16];
	long m_lMsgID;
	long m_lNum;
};


struct DccDataInfo
{
	int m_iSize;
	char m_sActionID[4];
	long m_lMsgID;
	long m_lNum;
};

struct DccTableXml
{	
	int m_iSize;
	int m_iDomain;//1规格数据，2参与人域；3产品域，4账务域，5关键局数据 
	char *m_pXml;
};





/**************************************************************************
*   类名：HSSOrder
*   功能：定义了订单Dcc的结构类
***************************************************************************/

class HSSOrder : public HSSObject
{
    
public:
    
    HSSOrder(int CMD) : HSSObject(CMD) {
    	clear();
    }
    
    bool empty() {
    	return !(*((long *)&m_uBody));	
    }
    
    
    
public:
    
    void clear() {
        m_iSize = CMD_OFFSET + sizeof(m_uBody.m_tNum);
        memset(&m_uBody.m_tNum, 0x00, sizeof(m_uBody.m_tNum));
        memset(&m_uBody.m_sBuf, 0x00, sizeof(m_uBody.m_sBuf));
    }
    
    bool addDccHead(DccHeadInfo &data){
    	memcpy((void *)m_sDccBuf, (const void *)&data, sizeof(DccHeadInfo));
    	return true;
    }
    
    bool addServiceInfo(DccServiceInfo &data) {
        if((m_iSize+sizeof(DccServiceInfo)) > CMD_BUF_MAX) {
            return false;
        }
        m_iFlag=1;
		m_iFlag2=0;
        m_uBody.m_tNum.m_iFirstNum ++;      
        memcpy((void *)&m_uBody.m_sBuf[m_iSize-CMD_OFFSET], (const void *)&data, sizeof(DccServiceInfo));
        m_pSrvSize = (int *)&m_uBody.m_sBuf[m_iSize-CMD_OFFSET];
        m_pDataNum = (long *)&m_uBody.m_sBuf[m_iSize-CMD_OFFSET+sizeof(DccServiceInfo)-8];
        *m_pDataNum = 0;
        *m_pSrvSize=0;
        m_iSize += sizeof(DccServiceInfo);
		*m_pSrvSize += sizeof(DccServiceInfo);
        return true;
    }
    
    bool addDataInfo(DccDataInfo &data) {
    	if(!m_iFlag) return false;
        if((m_iSize+sizeof(DccDataInfo)) > CMD_BUF_MAX) {
        		m_uBody.m_tNum.m_iFirstNum--;
        		m_iFlag=0;
            return false;
        }
		m_iFlag2=1;
        (*m_pDataNum) ++;
        memcpy((void *)&m_uBody.m_sBuf[m_iSize-CMD_OFFSET], (const void *)&data, sizeof(DccDataInfo));
        m_pDataSize = (int *)&m_uBody.m_sBuf[m_iSize-CMD_OFFSET];
		//m_pTableNum = (long *)&m_uBody.m_sBuf[m_iSize-CMD_OFFSET+sizeof(DccServiceInfo)-8];
		m_pTableNum = (long *)&m_uBody.m_sBuf[m_iSize-CMD_OFFSET+sizeof(DccDataInfo)-8];
        *m_pDataSize=0;
        *m_pTableNum = 0;
        m_iSize += sizeof(DccDataInfo);
		*m_pSrvSize += sizeof(DccDataInfo);
		*m_pDataSize +=sizeof(DccDataInfo);
        return true;
    }
      
    
    bool addTableInfo(DccTableXml &data) {
    	if(!m_iFlag2) return false;
        if((m_iSize+data.m_iSize) > CMD_BUF_MAX) {
        	m_uBody.m_tNum.m_iFirstNum--;
        	m_iFlag2=0;
          return false;
        }
        (*m_pTableNum) ++;
        memcpy((void *)&m_uBody.m_sBuf[m_iSize-CMD_OFFSET], (const void *)&data,sizeof(DccTableXml));
        //memcpy((void *)&m_uBody.m_sBuf[m_iSize-CMD_OFFSET]+sizeof(DccTableXml), (const void *)data.m_pXml, data.m_iSize-sizeof(DccTableXml));
        memcpy((void *)&m_uBody.m_sBuf[m_iSize-CMD_OFFSET+sizeof(DccTableXml)], (const void *)data.m_pXml, data.m_iSize-sizeof(DccTableXml));
        m_iSize += data.m_iSize;
		*m_pSrvSize += data.m_iSize;
		*m_pDataSize +=data.m_iSize;
        return true;
    }
    
    int getServiceNum() {
        return m_uBody.m_tNum.m_iFirstNum;                  
    }
    
    
    DccHeadInfo * getDccInfo() {
        return (DccHeadInfo *)m_sDccBuf;
    }

    int getServiceInfo(vector<DccServiceInfo *> &vec) {
    	  int m_iPost=CMD_OFFSET + sizeof(m_uBody.m_tNum);
        for(int i=0;i< m_uBody.m_tNum.m_iFirstNum;i++){
        	DccServiceInfo * tmp=(DccServiceInfo *)&m_uBody.m_sBuf[m_iPost-CMD_OFFSET];
        	vec.push_back(tmp);
        	m_iPost +=tmp->m_iSize;        	
        }
        return vec.size();      
    }
    
    int getDataInfo(DccServiceInfo * m_tmpInfo,vector<DccDataInfo *> &vec) {
    	  long m_lNum=m_tmpInfo->m_lNum;
    	  char * m_pPost= (char *)m_tmpInfo+sizeof(DccServiceInfo);
        for(int i=0;i< m_lNum;i++){
        	DccDataInfo * tmp=(DccDataInfo *)(m_pPost);
        	vec.push_back(tmp);
        	m_pPost =m_pPost+tmp->m_iSize;
        }
        return vec.size();      
    }
    
    int getTableInfo(DccDataInfo * m_tmpInfo,vector<DccTableXml *> &vec) {
    	  long m_lNum=m_tmpInfo->m_lNum;
    	  char * m_pPost= (char *)m_tmpInfo+sizeof(DccDataInfo);
        for(int i=0;i< m_lNum;i++){
        	DccTableXml * tmp=(DccTableXml *)(m_pPost); 
            //tmp.m_pXml=m_pPost+sizeof(DccTableXml);
            tmp->m_pXml=m_pPost+sizeof(DccTableXml);
        	vec.push_back(tmp);
        	m_pPost =m_pPost+tmp->m_iSize;
        }
        return vec.size();
    } 

private:
/*
	    
    int m_iFlag;
	int m_iFlag2;
    int *m_pSrvSize;
    int *m_pDataSize;
    long *m_pTableNum;
    long *m_pDataNum;*/
};


#endif/*__HSSCMDORDER_H_INCLUDED_*/
