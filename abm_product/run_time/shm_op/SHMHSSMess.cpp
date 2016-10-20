#include "SHMHSSMess.h"
#include "CSemaphore.h"
#include <sys/time.h>
#include <memory.h>

#define RCV_LOCK_KEY "20110718"
#define LOAD_LOCK_KEY "20110719"
#define INDB_LOCK_KEY "20110720"
#define RLS_LOCK_KEY "20110721"
#define UPDATE_LOCK_KEY "20110722"
#define QRY_LOCK_KEY    "20110723"
#define KPI_LOCK_KEY    "20110724"
#define STRESS_LOCK_KEY "20110725" 

#define NE_TO_INDEX(ne) (g_iNeToIndex[ne-3000])
#define INDEX_TO_NE(index) (g_iIndexToNe[index])

static int g_iLocToIndex[1024];
static int g_iIndexToLoc[64];
static int g_iNeToIndex[64];
static int g_iIndexToNe[64];
static bool g_bInitFlag = false;

static CSemaphore g_oRcvLock;
static CSemaphore g_oLoadLock;
static CSemaphore g_oInDBLock;
static CSemaphore g_oRlsLock;
static CSemaphore g_oUpdateLock;
static CSemaphore g_oQryLock;
static CSemaphore g_oKPILock;
static CSemaphore g_oStressLock;

void init_mess()
{
    
    if (g_bInitFlag)
        return;
    if (!g_oRcvLock.getSem(RCV_LOCK_KEY) || !g_oLoadLock.getSem(LOAD_LOCK_KEY) || !g_oInDBLock.getSem(INDB_LOCK_KEY) \
        || !g_oRlsLock.getSem(RLS_LOCK_KEY) || !g_oUpdateLock.getSem(UPDATE_LOCK_KEY) || !g_oQryLock.getSem(QRY_LOCK_KEY) \
        || !g_oKPILock.getSem(KPI_LOCK_KEY) ||!g_oStressLock.getSem(STRESS_LOCK_KEY))
        throw;     
    // 号头 下标 初始化
    memset((void *)&g_iLocToIndex[0], -1, sizeof(g_iLocToIndex));
    g_iLocToIndex[1] = 0;
    g_iLocToIndex[2] = 1;
    g_iLocToIndex[10] = 2;
    g_iLocToIndex[20] = 3;  
    g_iLocToIndex[21] = 4;
    g_iLocToIndex[22] = 5;
    g_iLocToIndex[23] = 6;
    g_iLocToIndex[24] = 7;
    g_iLocToIndex[25] = 8;
    g_iLocToIndex[27] = 9;
    g_iLocToIndex[28] = 10;
    g_iLocToIndex[29] = 11;
    g_iLocToIndex[311] = 12;
    g_iLocToIndex[351] = 13;
    g_iLocToIndex[371] = 14;
    g_iLocToIndex[431] = 15;
    g_iLocToIndex[451] = 16;
    g_iLocToIndex[471] = 17;
    g_iLocToIndex[531] = 18;
    g_iLocToIndex[551] = 19;
    g_iLocToIndex[571] = 20;
    g_iLocToIndex[591] = 21;
    g_iLocToIndex[731] = 22;
    g_iLocToIndex[771] = 23;
    g_iLocToIndex[791] = 24;
    g_iLocToIndex[851] = 25;
    g_iLocToIndex[871] = 26;
    g_iLocToIndex[891] = 27;
    g_iLocToIndex[898] = 28;
    g_iLocToIndex[931] = 29;
    g_iLocToIndex[951] = 30;
    g_iLocToIndex[971] = 31;
    g_iLocToIndex[991] = 32;
    
    memset((void *)&g_iIndexToLoc[0], -1, sizeof(g_iIndexToLoc));
    for (int i=0; i<=32; ++i) {
        for (int j=0; j<1024; ++j) {
            if (i == g_iLocToIndex[j]) {
                g_iIndexToLoc[i] = j;
                break;
            }        
        }    
    }
    
    //网元 下标初始化
    memset((void *)&g_iNeToIndex[0], -1, sizeof(g_iNeToIndex));
    g_iNeToIndex[1] = 0;
    g_iNeToIndex[2] = 1;
    g_iNeToIndex[3] = 2;
    g_iNeToIndex[4] = 3;
    g_iNeToIndex[5] = 4;
    g_iNeToIndex[6] = 5;
    g_iNeToIndex[7] = 6;
    g_iNeToIndex[8] = 7;
    g_iNeToIndex[9] = 8;
    g_iNeToIndex[10] = 9;
    g_iNeToIndex[11] = 10;
    g_iNeToIndex[12] = 11;
    g_iNeToIndex[31] = 12;
    g_iNeToIndex[61] = 13;
    
    memset((void *)&g_iIndexToNe[0], -1, sizeof(g_iIndexToNe));
    for (int i=0; i<=13; ++i) {
        for (int j=0; j<64; ++j) {
            if (i == g_iNeToIndex[j]) {
                g_iIndexToNe[i] = j+3000;
                break;
            }        
        }    
    }
    g_bInitFlag = true;
}

MessBlock::MessBlock()
{  
}    

MessBlock::~MessBlock()
{
    
}

void MessBlock::init() 
{
    init_mess();    
}
        
int MessBlock::add(long lTm, int iLoc, int iNeID, int iAct, int iType, int iCnt)
{
    if ((lTm<0) || (iLoc>1024) || (iLoc<0) || (iNeID>3064) || (iNeID<3000))
        return -1;
    if ((g_iLocToIndex[iLoc]==-1) || (NE_TO_INDEX(iNeID)==-1))
        return -1;   
    long lMin, lHH;
    unsigned int uiTmIdx, uiLocIdx, uiNeIdx;
    lMin = lTm/(1000000*60);
    lHH = lMin/60;
    uiTmIdx = (lMin-lHH*60)/15;
    lMin = lHH * 60 + 15 * uiTmIdx;
    
    switch (iAct) {
    case MESS_RCV:
        g_oRcvLock.P();
        m_oRcv[g_iLocToIndex[iLoc]][NE_TO_INDEX(iNeID)][uiTmIdx].add(lMin, iType);
        g_oRcvLock.V();
        break;
    case MESS_LOAD:
        g_oLoadLock.P();
        m_oLoad[g_iLocToIndex[iLoc]][NE_TO_INDEX(iNeID)][uiTmIdx].add(lMin, iType);
        g_oLoadLock.V();
        break;
    case MESS_INDB:
        g_oInDBLock.P();
        m_oInDB[g_iLocToIndex[iLoc]][NE_TO_INDEX(iNeID)][uiTmIdx].add(lMin, iType);
        g_oInDBLock.V();
        break;
    case MESS_RLS:
        g_oRlsLock.P();
        m_oRls[g_iLocToIndex[iLoc]][NE_TO_INDEX(iNeID)][uiTmIdx].add(lMin, iType);
        g_oRlsLock.V();
        break;
    case MESS_UPDATE:
        g_oUpdateLock.P();
        m_oUpdate[g_iLocToIndex[iLoc]][NE_TO_INDEX(iNeID)][uiTmIdx].add(lMin, iType, iCnt);
        g_oUpdateLock.V();
        break;               
    default:
        return -1;
    }
    return 0;         
}

void MessBlock::getRcv(std::vector<BalanceMessStruct> &vRcv, long lInTm)
{
    BalanceMessStruct oTmp;
    struct timeval tvstart;
    if (!lInTm) {
        gettimeofday(&tvstart, NULL);
        lInTm = tvstart.tv_sec/60 - 15;
    }    
    for (int i=0; i<LOC_CNT; ++i) {
        for (int j=0; j<NE_CNT; ++j) {
            for (int k=0; k<TM_CNT; ++k) {
                if (m_oRcv[i][j][k].m_lTime) {
                    if (m_oRcv[i][j][k].m_lTime < lInTm) {
                        oTmp.assign(&m_oRcv[i][j][k]);
                        oTmp.m_uiLoc = g_iIndexToLoc[i];
                        oTmp.m_uiNeID = g_iIndexToNe[j];
                        vRcv.push_back(oTmp);
                        m_oRcv[i][j][k].clear();
                    }        
                }    
            }    
        }    
    }        
}

void MessBlock::getCurRcv(std::vector<BalanceMessStruct> &vRcv, long lInTm)
{
    BalanceMessStruct oTmp;
    long lMin, lHH;
    unsigned int uiTmIdx, uiLocIdx, uiNeIdx;
    struct timeval tvstart;
    if (!lInTm) {
        gettimeofday(&tvstart, NULL);
        lInTm = tvstart.tv_sec;
    } 
    
    lMin = lInTm/60;
    lHH = lMin/60;
    uiTmIdx = (lMin-lHH*60)/15;
    lMin = lHH * 60 + 15 * uiTmIdx;
    
    for (int i=0; i<LOC_CNT; ++i) {
        for (int j=0; j<NE_CNT; ++j) {
            if (m_oRcv[i][j][uiTmIdx].m_lTime == lMin) {
                oTmp.assign(&m_oRcv[i][j][uiTmIdx]);
                oTmp.m_uiLoc = g_iIndexToLoc[i];
                oTmp.m_uiNeID = g_iIndexToNe[j];
                vRcv.push_back(oTmp);    
            }    
        }
    }    
           
}

void MessBlock::getCurLoad(std::vector<BalanceMessStruct> &vRcv, long lInTm)
{
    BalanceMessStruct oTmp;
    long lMin, lHH;
    unsigned int uiTmIdx, uiLocIdx, uiNeIdx;
    struct timeval tvstart;
    if (!lInTm) {
        gettimeofday(&tvstart, NULL);
        lInTm = tvstart.tv_sec;
    } 
    
    lMin = lInTm/60;
    lHH = lMin/60;
    uiTmIdx = (lMin-lHH*60)/15;
    lMin = lHH * 60 + 15 * uiTmIdx;
    
    for (int i=0; i<LOC_CNT; ++i) {
        for (int j=0; j<NE_CNT; ++j) {
            if (m_oLoad[i][j][uiTmIdx].m_lTime == lMin) {
                oTmp.assign(&m_oLoad[i][j][uiTmIdx]);
                oTmp.m_uiLoc = g_iIndexToLoc[i];
                oTmp.m_uiNeID = g_iIndexToNe[j];
                vRcv.push_back(oTmp);    
            }    
        }
    }              
}

void MessBlock::getCurRls(std::vector<BalanceMessStruct> &vRcv, long lInTm)
{
    BalanceMessStruct oTmp;
    long lMin, lHH;
    unsigned int uiTmIdx, uiLocIdx, uiNeIdx;
    struct timeval tvstart;
    if (!lInTm) {
        gettimeofday(&tvstart, NULL);
        lInTm = tvstart.tv_sec;
    } 
    
    lMin = lInTm/60;
    lHH = lMin/60;
    uiTmIdx = (lMin-lHH*60)/15;
    lMin = lHH * 60 + 15 * uiTmIdx;
    
    for (int i=0; i<LOC_CNT; ++i) {
        for (int j=0; j<NE_CNT; ++j) {
            if (m_oRls[i][j][uiTmIdx].m_lTime == lMin) {
                oTmp.assign(&m_oRls[i][j][uiTmIdx]);
                oTmp.m_uiLoc = g_iIndexToLoc[i];
                oTmp.m_uiNeID = g_iIndexToNe[j];
                vRcv.push_back(oTmp);    
            }    
        }
    }    
           
}         

void MessBlock::getLoad(std::vector<BalanceMessStruct> &vRcv, long lInTm)
{
    BalanceMessStruct oTmp;
    struct timeval tvstart;
    if (!lInTm) {
        gettimeofday(&tvstart, NULL);
        lInTm = tvstart.tv_sec/60 - 15;
    }  
    for (int i=0; i<LOC_CNT; ++i) {
        for (int j=0; j<NE_CNT; ++j) {
            for (int k=0; k<TM_CNT; ++k) {
                if (m_oLoad[i][j][k].m_lTime) {
                    if (m_oLoad[i][j][k].m_lTime < lInTm) {
                        oTmp.assign(&m_oLoad[i][j][k]);
                        oTmp.m_uiLoc = g_iIndexToLoc[i];
                        oTmp.m_uiNeID = g_iIndexToNe[j];
                        vRcv.push_back(oTmp);
                        m_oLoad[i][j][k].clear();
                    }        
                }    
            }    
        }    
    }        
}

void MessBlock::getInDB(std::vector<BalanceMessStruct> &vRcv, long lInTm)
{
    BalanceMessStruct oTmp;
    struct timeval tvstart;
    if (!lInTm) {
        gettimeofday(&tvstart, NULL);
        lInTm = tvstart.tv_sec/60 - 15;
    } 
    for (int i=0; i<LOC_CNT; ++i) {
        for (int j=0; j<NE_CNT; ++j) {
            for (int k=0; k<TM_CNT; ++k) {
                if (m_oInDB[i][j][k].m_lTime) {
                    if (m_oInDB[i][j][k].m_lTime < lInTm) {
                        oTmp.assign(&m_oInDB[i][j][k]);
                        oTmp.m_uiLoc = g_iIndexToLoc[i];
                        oTmp.m_uiNeID = g_iIndexToNe[j];
                        vRcv.push_back(oTmp);
                        m_oInDB[i][j][k].clear();
                    }        
                }    
            }    
        }    
    }        
}

void MessBlock::getRls(std::vector<BalanceMessStruct> &vRcv, long lInTm)
{
    BalanceMessStruct oTmp;
    struct timeval tvstart;
    if (!lInTm) {
        gettimeofday(&tvstart, NULL);
        lInTm = tvstart.tv_sec/60 - 15;
    } 
    for (int i=0; i<LOC_CNT; ++i) {
        for (int j=0; j<NE_CNT; ++j) {
            for (int k=0; k<TM_CNT; ++k) {
                if (m_oRls[i][j][k].m_lTime) {
                    if (m_oRls[i][j][k].m_lTime < lInTm) {
                        oTmp.assign(&m_oRls[i][j][k]);
                        oTmp.m_uiLoc = g_iIndexToLoc[i];
                        oTmp.m_uiNeID = g_iIndexToNe[j];
                        vRcv.push_back(oTmp);
                        m_oRls[i][j][k].clear();
                    }        
                }    
            }    
        }    
    }        
}

void MessBlock::getUpdate(std::vector<InstUpdateMessStruct> &vRcv, long lInTm)
{
    InstUpdateMessStruct oTmp;
    struct timeval tvstart;
    if (!lInTm) {
        gettimeofday(&tvstart, NULL);
        lInTm = tvstart.tv_sec/60 - 15;
    } 
    for (int i=0; i<LOC_CNT; ++i) {
        for (int j=0; j<NE_CNT; ++j) {
            for (int k=0; k<TM_CNT; ++k) {
                if (m_oUpdate[i][j][k].m_lTime) {
                    if (m_oUpdate[i][j][k].m_lTime < lInTm) {
                        oTmp.assign(&m_oUpdate[i][j][k]);
                        oTmp.m_uiLoc = g_iIndexToLoc[i];
                        oTmp.m_uiNeID = g_iIndexToNe[j];
                        vRcv.push_back(oTmp);
                        m_oUpdate[i][j][k].clear();
                    }        
                }    
            }    
        }    
    }        
}

int MessBlock::addQry(long lTm, int iType)
{
    if (lTm < 0)
        return -1;    
    long lMin, lHH;
    unsigned int uiTmIdx;
    lMin = lTm/(1000000*60);
    lHH = lMin/60;
    uiTmIdx = (lMin-lHH*60)/15;
    lMin = lHH * 60 + 15 * uiTmIdx;
    g_oQryLock.P();
    int iRet = m_oQry[uiTmIdx].add(lMin, iType, 0);
    g_oQryLock.V();
    return iRet;          
}

int MessBlock::addQryExcp(long lTm, int iType)
{
    if (lTm < 0)
        return -1;
    long lMin, lHH;
    unsigned int uiTmIdx;
    lMin = lTm/(1000000*60);
    lHH = lMin/60;
    uiTmIdx = (lMin-lHH*60)/15;
    lMin = lHH * 60 + 15 * uiTmIdx;
    g_oQryLock.P();
    int iRet = m_oQry[uiTmIdx].add(lMin, iType, 1);
    g_oQryLock.V();
    return iRet;          
}

void MessBlock::getQry(std::vector<QryKPI> &vQry, long lInTm)
{
    struct timeval tvstart;
    BalanceMessStruct oTmp;
    if (!lInTm) {
        gettimeofday(&tvstart, NULL);
        lInTm = tvstart.tv_sec/60 - 15;
    }    
    for (int i=0; i<TM_CNT; ++i) {
        if (m_oQry[i].m_lTime) {
            if (m_oQry[i].m_lTime < lInTm) {
                vQry.push_back(m_oQry[i]);
                m_oQry[i].clear();    
            }        
        }        
    }    
}

void MessBlock::addQryKPI(long lTm, int iItem)
{
    long lMin, lHH;
    unsigned int uiTmIdx;
    lMin = lTm/(1000000*60);
    lHH = lMin/60;
    uiTmIdx = (lMin-lHH*60)/15;
    lMin = lHH * 60 + 15 * uiTmIdx;
    g_oKPILock.P();
    m_oQS[QRY_KPI][uiTmIdx].add(lMin, iItem);
    g_oKPILock.V();        
}

void MessBlock::addSynKPI(long lTm, int iItem)
{
    long lMin, lHH;
    unsigned int uiTmIdx;
    lMin = lTm/(1000000*60);
    lHH = lMin/60;
    uiTmIdx = (lMin-lHH*60)/15;
    lMin = lHH * 60 + 15 * uiTmIdx;
    g_oKPILock.P();
    m_oQS[SYN_KPI][uiTmIdx].add(lMin, iItem);
    g_oKPILock.V();        
}            

void MessBlock::getQS(std::vector<BalanceMess> &vQry, std::vector<BalanceMess> &vSyn, long lInTm)
{
    BalanceMess oTmp;
    struct timeval tvstart;
    if (!lInTm) {
        gettimeofday(&tvstart, NULL);
        lInTm = tvstart.tv_sec/60 - 15;
    }
    for (int i=0; i<TM_CNT; ++i) {
        if (m_oQS[0][i].m_lTime) {
            if (m_oQS[0][i].m_lTime < lInTm) {
                vQry.push_back(m_oQS[0][i]);
                m_oQS[0][i].clear();    
            }    
        }    
    } 
    
    for (int i=0; i<TM_CNT; ++i) {
        if (m_oQS[1][i].m_lTime) {
            if (m_oQS[1][i].m_lTime < lInTm) {
                vSyn.push_back(m_oQS[1][i]);
                m_oQS[1][i].clear();    
            }    
        }    
    }       
}

void MessBlock::rcvAddMess(long lTm, int iNeID, int iRltCode, int iCmdID, int iQryType)
{
    //首先判断是内部发布消息的应答包 还是外部请求包
    if ((iCmdID>=RLECMD_BEGIN) && (iCmdID<=RLECMD_END)) {
        //发布消息的应答包 根据返回码 == 统计类型 
        addRls(lTm, DFT_LOC, iNeID, iRltCode);
    }
    else {   //外部请求包
        addRcv(lTm, DFT_LOC, iNeID, RECV_MSG);
        
        //判断是查询 还是加载(同步)
        if ((iCmdID>=QRYCMD_BEGIN) && (iCmdID<=QRYCMD_END)) {
            addQry(lTm, iQryType);
            addQryKPI(lTm, RECV_MSG);
        }
        else {
            addLoad(lTm, DFT_LOC, iNeID, RECV_MSG);
            addSynKPI(lTm, RECV_MSG);
        } 
    } 
}

void MessBlock::sndAddMess(long lTm, int iNeID, int iRltCode, int iCmdID, int iQryType)
{
    //首先判断是HSS的发布请求包 还是回复给外部的应答包
    if ((iCmdID>=RLECMD_BEGIN) && (iCmdID<=RLECMD_END)) {
        addRls(lTm, DFT_LOC, iNeID, RECV_MSG);
    }
    else { //外部请求回包
        addRcv(lTm, DFT_LOC, iNeID, iRltCode);
        
        //判断是查询 还是加载(同步)
        if ((iCmdID>=QRYCMD_BEGIN) && (iCmdID<=QRYCMD_END)) {
            addQryKPI(lTm, iRltCode);
            
            //判断是否查询异常回包
            if (iRltCode != SUCC_MSG)
                addQryExcp(lTm, iQryType);    
        }
        else {
            addLoad(lTm, DFT_LOC, iNeID, iRltCode);
            addSynKPI(lTm, iRltCode);
        }
    }
}    

int MessBlock::addStress(long lInTm, int iNeID, int iCmdID)
{
    if ((lInTm<0) || (iNeID>3064) || (iNeID<3000) || (iCmdID<0) || (iCmdID>=256))
        return -1;
    if (NE_TO_INDEX(iNeID) == -1)
        return -1;   
    long lMin, lHH, lUs;
    unsigned int uiTmIdx, uiLocIdx, uiNeIdx;
    lMin = lInTm/(1000000*60);
    lHH = lMin/60;
    uiTmIdx = (lMin-lHH*60)/15;
    lMin = lHH * 60 + 15 * uiTmIdx;
    struct timeval tvstart;
    gettimeofday(&tvstart, NULL);
    lUs = tvstart.tv_sec*1000000 + tvstart.tv_usec - lInTm;

    g_oStressLock.P();
    m_oStress[iCmdID][NE_TO_INDEX(iNeID)][uiTmIdx].add(lMin, lUs);
    g_oStressLock.V();
    
    return 0;                 
}  

void MessBlock::getStress(std::vector<StressDistriStruct> &vStress, long lInTm)
{
    StressDistriStruct oTmp;
    struct timeval tvstart;
    if (!lInTm) {
        gettimeofday(&tvstart, NULL);
        lInTm = tvstart.tv_sec/60 - 15;
    }
    
    for (int i=0; i<CMD_CNT; ++i) {
        for (int j=0; j<NE_CNT; ++j) {
            for (int k=0; k<TM_CNT; ++k) {
                if (m_oStress[i][j][k].m_lTime) {
                    if (m_oStress[i][j][k].m_lTime < lInTm) {
                        oTmp.assign(&m_oStress[i][j][k], g_iIndexToNe[j], i);
                        vStress.push_back(oTmp);
                        m_oStress[i][j][k].clear();    
                    }    
                }    
            }    
        }    
    }    
}      