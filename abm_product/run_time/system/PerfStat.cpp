#include <string.h>
#include <sys/time.h>
#include "PerfStat.h"

int StatDataMgr::getOfflineData(StatCmdData &oData, int &iLinkID)
{
    int ret=-1;

    struct timeval tm;

    gettimeofday (&tm, 0);
    int tmin = tm.tv_sec / 60 ;

    for (int i=0; i<LINK_NUM_MAX && m_poData->m_oData[i].m_iLinkID; i++) {
        for (int j=0; j<TIME_SLICE_MAX; j++) {
            StatCmdData *p = &(m_poData->m_oData[i].m_oCmdData[j]);
            if (!(p->m_lTime)) continue;

            if ((tmin-p->m_lTime>=DFT_INVAL_DUR) ||
                    (tmin>p->m_lTime && p->m_iSendNum==p->m_iRecvNum)) {
                memcpy (&oData, p, sizeof(StatCmdData));
                iLinkID = m_poData->m_oData[i].m_iLinkID;
                memset (p, 0, sizeof(StatCmdData));
                return 0;
            }
        }
    }

    return -1;
}

int PerfStat::init(StatData *pMem, int iLinkID)
{
    if (!pMem) {
    	return -1;	
    }
    
    if (iLinkID>=LINK_NUM_MAX || iLinkID<1) {
        return -2;
    }

    m_poData = &(pMem->m_oData[iLinkID-1]);
    m_poData->m_iLinkID = iLinkID;

	return 0;	
}

int PerfStat::getDurDataPoint(long dur)
{
	long iRet;
    dur += 9; dur/=10; //转成毫秒 向上取整

    if ((dur>8)) { //>=8 ms
        iRet = 8 + (dur-8)/4;
        if (iRet>DURATION_NUM-1) {
            iRet = DURATION_NUM - 1;
        }
    } else {
        iRet = dur-1;
        if (iRet<0) iRet=0; //万一的情况下，会小于0.1ms
    }
    
    return iRet;
}

int PerfStat::updSendClock()
{
     struct timeval tm;

     gettimeofday (&tm, 0);
     m_lSClockTime = tm.tv_sec*1000000 + tm.tv_usec;

     return 0;
}

int PerfStat::updRecvClock()
{
     struct timeval tm;

     gettimeofday (&tm, 0);
     m_lRClockTime = tm.tv_sec*1000000 + tm.tv_usec;

     return 0;
}

int PerfStat::statSendPack(int packnum, int sendnum)
{
    long tmmin = (m_lSClockTime/(1000000*60));
    long minnum = (tmmin % 60) & (TIME_SLICE_MAX-1);

    StatCmdData *pCmdData = &(m_poData->m_oCmdData[minnum]);

    if (pCmdData->m_lTime != tmmin) {   //查看该分片时间是否和消息时间相等
        if (pCmdData->m_lTime != 0) {   //不等的话 看是否第一次写入
            m_poData->m_oCmdData[TIME_SLICE_MAX].m_iSendPack += packnum;
            m_poData->m_oCmdData[TIME_SLICE_MAX].m_iSendPackContNum += sendnum;
            return 0;
        } else {
            pCmdData->m_lTime = tmmin;
        }
    }

    pCmdData->m_iSendPack += packnum;
    pCmdData->m_iSendPackContNum += sendnum;

    return 0;
}

int PerfStat::statRecvPack(int packnum, int recvnum)
{
    long tmmin = (m_lRClockTime/(1000000*60));
    long minnum = (tmmin % 60) & (TIME_SLICE_MAX-1);

    StatCmdData *pCmdData = &(m_poData->m_oCmdData[minnum]);

    if (pCmdData->m_lTime != tmmin) {   //查看该分片时间是否和消息时间相等
        if (pCmdData->m_lTime != 0) {   //不等的话 看是否第一次写入
            m_poData->m_oCmdData[TIME_SLICE_MAX].m_iRecvPack += packnum;
            m_poData->m_oCmdData[TIME_SLICE_MAX].m_iRecvPackContNum += recvnum;
            return 0;
        } else {
            pCmdData->m_lTime = tmmin;
        }
    }

    pCmdData->m_iRecvPack += packnum;
    pCmdData->m_iRecvPackContNum += recvnum;

    return 0;
}

int PerfStat::statClntSend(ABMObject * pObject)
{
    long tmmin = (m_lSClockTime/(1000000*60));
    long minnum = (tmmin % 60) & (TIME_SLICE_MAX-1);
    int  cmd = pObject->m_iCmdID & (ABM_CMD_MAX-1);

    pObject->m_lInTm = m_lSClockTime;

    StatCmdData *pCmdData = &(m_poData->m_oCmdData[minnum]);
    if (pCmdData->m_lTime != tmmin) {   //查看该分片时间是否和消息时间相等
        if (pCmdData->m_lTime != 0) {   //不等的话 看是否第一次写入
            m_poData->m_oCmdData[TIME_SLICE_MAX].m_oUnitData[cmd].m_iSendNum++;
            m_poData->m_oCmdData[TIME_SLICE_MAX].m_iSendNum++;
            return 0;
        }
        else {
            pCmdData->m_lTime = tmmin;
        }    
    } 

    pCmdData->m_oUnitData[cmd].m_iSendNum++;
    pCmdData->m_iSendNum++;
    return 0;
}

int PerfStat::statClntRecv(ABMObject * pObject)
{
    StatCmdData * pCmdData;

    long dur = ((m_lRClockTime-pObject->m_lInTm)/10+9)/10; //0.1ms 向上取整

    if (dur > DFT_INVAL_DUR*60*10000) {
        pCmdData = &(m_poData->m_oCmdData[TIME_SLICE_MAX]);
    } else {
        long tmmin = (pObject->m_lInTm/(1000000*60));
        long minnum = (tmmin % 60) & (TIME_SLICE_MAX-1);

        pCmdData = &(m_poData->m_oCmdData[minnum]);
        if (pCmdData->m_lTime != tmmin) {
            pCmdData =  &(m_poData->m_oCmdData[TIME_SLICE_MAX]);
        }
    }

    int  cmd = pObject->m_iCmdID & (ABM_CMD_MAX-1);
    StatUnitData * pUnitData = &(pCmdData->m_oUnitData[cmd]);
    
    if (pObject->m_lOutTm<0) pObject->m_lOutTm=0;   //遇到业务端处理时间为副的情况，直接赋值为0

    int ndur = dur - pObject->m_lOutTm;
    pUnitData->m_iRecvNum++;
    pUnitData->m_lTotalDur += dur;
    pUnitData->m_lNetDur += ndur;
    pCmdData->m_iRecvNum++;

    if (pUnitData->m_iMaxWDur < dur) {
        pUnitData->m_iMaxWDur = dur;
    }
    
    if (pUnitData->m_iMinWDur > dur) {
        pUnitData->m_iMinWDur = dur;
    }

    if (pUnitData->m_iMaxNDur < ndur) {
        pUnitData->m_iMaxNDur = ndur;
    }
    
    if (pUnitData->m_iMinNDur > ndur) {
        pUnitData->m_iMinNDur = ndur;
    }

	pUnitData->m_iDurData[getDurDataPoint(dur)]++;
	pUnitData->m_iNDurData[getDurDataPoint(ndur)]++;
	pUnitData->m_iODurData[getDurDataPoint(pObject->m_lOutTm)]++;

    return 0;
}

int PerfStat::statSrvRecv(ABMObject *pObject)
{
    long tmmin = (m_lRClockTime/(1000000*60));
    long minnum = (tmmin % 60) & (TIME_SLICE_MAX-1);
    int  cmd = pObject->m_iCmdID & (ABM_CMD_MAX-1);

    pObject->m_lOutTm = m_lRClockTime;

    StatCmdData *pCmdData = &(m_poData->m_oCmdData[minnum]);
    if (pCmdData->m_lTime != tmmin) {   //查看该分片时间是否和消息时间相等
        if (pCmdData->m_lTime != 0) {   //不等的话 看是否第一次写入
            m_poData->m_oCmdData[TIME_SLICE_MAX].m_oUnitData[cmd].m_iRecvNum++;
            m_poData->m_oCmdData[TIME_SLICE_MAX].m_iRecvNum++;
            return 0;
        }
        else {
            pCmdData->m_lTime = tmmin;
        }    
    } 

    pCmdData->m_oUnitData[cmd].m_iRecvNum++;
    pCmdData->m_iRecvNum++;

    return 0;
}


int PerfStat::statSrvSend(ABMObject *pObject)
{
    StatCmdData * pCmdData;

    long dur = ((m_lSClockTime-pObject->m_lOutTm)/10+9)/10; //0.1ms 向上取整

    if (dur > DFT_INVAL_DUR*60*10000) {
        pCmdData = &(m_poData->m_oCmdData[TIME_SLICE_MAX]);
    } else {
        long tmmin = (pObject->m_lOutTm/(1000000*60));
        long minnum = (tmmin % 60) & (TIME_SLICE_MAX-1);

        pCmdData = &(m_poData->m_oCmdData[minnum]);
        if (pCmdData->m_lTime != tmmin) {
            pCmdData =  &(m_poData->m_oCmdData[TIME_SLICE_MAX]);
        }
    }

    int  cmd = pObject->m_iCmdID & (ABM_CMD_MAX-1);
    StatUnitData * pUnitData = &(pCmdData->m_oUnitData[cmd]);

    pObject->m_lOutTm = dur;

    pUnitData->m_iSendNum++;
    pUnitData->m_lTotalDur += dur;
    pCmdData->m_iSendNum++;

    if (pUnitData->m_iMaxWDur < dur) {
        pUnitData->m_iMaxWDur = dur;
    }
    
    if (pUnitData->m_iMinWDur > dur) {
        pUnitData->m_iMinWDur = dur;
    }

    pUnitData->m_iDurData[getDurDataPoint(dur)]++;

    return 0;
}

