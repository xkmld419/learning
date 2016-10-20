#ifndef __SHM_HSS_KPI_H_INCLUDED_
#define __SHM_HSS_KPI_H_INCLUDED_

#include <vector>
#include <memory.h>
/*
*   每15分钟更新一块内存  
*/
#ifndef __UN_32
typedef /*unsigned*/ int _ui32;
#else
typedef unsigned int _ui32;
#endif

//以下宏定义不可以随便修改 跟结构体里  add函数绑定

//平衡性消息类型成功 拒绝 失败
#define RECV_MSG 0
#define SUCC_MSG 1   
#define RJCT_MSG 2
#define FAIL_MSG 3

//实例数变更统计类型
#define UDT_CUST      10
#define UDT_USER      11
#define UDT_ACCT      12
#define UDT_PRODOFFER 13

//查询接口性能统计类型
#define QRY_USER       1001
#define QRY_CUST       1002
#define QRY_PRODOFFER  1003
#define QRY_ACCT       1004
#define QRY_MEMBER     1005
#define QRY_INST       1006
#define QRY_SPEC_HEAD  1007
#define QRY_LOC_NBR    1008
#define QRY_LOC_IMSI   1009
#define QRY_H_CODE     1010
#define QRY_LOC_HEAD   1011
#define QRY_EDGE_RMT   1012
#define QRY_C_INTL_RMT 1013
#define QRY_DIFF_IMSI  1014
#define QRY_MIN        1015
#define QRY_NBR_PTB    1016
#define QRY_SP         1017


//2011-07-14 新增统计 每分钟 查询 和 同步按15分钟统计入，处理正常 处理失败  拒收
//这个跟第一个统计重了，减少代码修改 增加内存 以后记的修改
#define QRY_KPI 0    //查询统计
#define SYN_KPI 1    //同步统计


//2011-07-20 定义了 发布命令ID 和查询命令ID范围 方便信息点封装
#define DFT_LOC 25
#define RLECMD_BEGIN 40
#define RLECMD_END   61
#define QRYCMD_BEGIN 101
#define QRYCMD_END   120

//信息点平衡统计结构体
struct BalanceMess
{    
    int add(long lMin, int iType) {
        if ((iType<0) || (iType>3))
            return -1;
        if (lMin > m_lTime)
            clear(lMin);    
        _ui32 *pData = &m_uiTotalNum;
        pData += iType;   
        (*pData)++;     
        return 0;   
    }
    
    void clear(long lTm = 0) {
        m_uiTotalNum = 0;
        m_uiSuccNum = 0;       
        m_uiRjctNum = 0;
        m_uiFailNum = 0;
        m_lTime = lTm;
    }
    
    _ui32 m_uiTotalNum;
    _ui32 m_uiSuccNum;
    _ui32 m_uiRjctNum;
    _ui32 m_uiFailNum;
    long m_lTime;
};

struct BalanceMessStruct : public BalanceMess
{
    _ui32 m_uiLoc;
    _ui32 m_uiNeID;
    
    void assign(BalanceMess *pMess) {
        m_uiTotalNum = pMess->m_uiTotalNum;
        m_uiSuccNum = pMess->m_uiSuccNum;
        m_uiRjctNum = pMess->m_uiRjctNum;
        m_uiFailNum = pMess->m_uiFailNum;
        m_lTime = pMess->m_lTime;    
    }        
};

//实例数变更
struct InstUpdateMess
{
    int add(long lMin, int iType, int iCnt) {
        if ((iType<10) || (iType>13))
            return -1;
        else
            iType -= 10;
        if (lMin > m_lTime)
            clear(lMin);        
        _ui32 *pData = &m_uiCustNum;
        pData += iType;    
        (*pData) = *pData + iCnt;     
        return 0;   
    }
   
    void clear(long lTm=0) {
        m_uiCustNum = 0;
        m_uiUserNum = 0; 
        m_uiAcctNum = 0;
        m_uiProdOfferNum = 0;
        m_lTime = lTm;
    } 
    _ui32 m_uiCustNum;
    _ui32 m_uiUserNum;
    _ui32 m_uiAcctNum;
    _ui32 m_uiProdOfferNum;
    long m_lTime;    
};

struct InstUpdateMessStruct : public InstUpdateMess
{
    _ui32 m_uiLoc;
    _ui32 m_uiNeID;
    
    void assign(InstUpdateMess *pMess) {
        m_uiCustNum = pMess->m_uiCustNum;
        m_uiUserNum = pMess->m_uiUserNum;
        m_uiAcctNum = pMess->m_uiAcctNum;
        m_uiProdOfferNum = pMess->m_uiProdOfferNum;
        m_lTime = pMess->m_lTime;    
    }    
};

struct QryKPI
{
    _ui32 m_uiUser[2];
    _ui32 m_uiCust[2];
    _ui32 m_uiProdOffer[2];
    _ui32 m_uiAcct[2];
    _ui32 m_uiInstToMember[2];
    _ui32 m_uiMemberToInst[2];
    _ui32 m_uiSpecialHead[2];
    _ui32 m_uiLocNbr[2];
    _ui32 m_uiLocIMSI[2];
    _ui32 m_uiHCode[2];
    _ui32 m_uiLocHead[2];
    _ui32 m_uiEdgeRemote[2];
    _ui32 m_uiCIntlRemote[2];
    _ui32 m_uiDiffIMSI[2];
    _ui32 m_uiMin[2];
    _ui32 m_uiNbrPortability[2];
    _ui32 m_uiSP[2];
    long m_lTime;
    
    int add(long lMin, int iType, int iAct) {
        if ((iType<1001) || (iType>1017))
            return -1;
        else
            iType -= 1001;
        if (lMin > m_lTime)
            clear(lMin);                    
        _ui32 *pData = &m_uiUser[0];
        pData += (2*iType+iAct);
        (*pData)++;
        return 0;            
    }  
    
    void clear(long lTm=0) {
        memset((void *)&m_uiUser[0], 0x00, sizeof(QryKPI));
        m_lTime = lTm;    
    }  
};

struct StressDistribute
{
    _ui32 m_ui100;
    _ui32 m_ui200;
    _ui32 m_ui300;
    _ui32 m_ui500;
    _ui32 m_ui1000;
    _ui32 m_ui1500;
    _ui32 m_ui5000;
    _ui32 m_uiTmOut;
    long  m_lTime;
    
    int add(long lMin, _ui32 uiUs) {
        if (lMin != m_lTime)
            clear(lMin);
        
        if (uiUs < 0)
            return -1;     
        else if (uiUs <= 100000)
            m_ui100++;
        else if (uiUs <= 200000)
            m_ui200++;
        else if (uiUs <= 300000)
            m_ui300++;
        else if (uiUs <= 500000)
            m_ui500++; 
        else if (uiUs <= 1000000)
            m_ui1000++; 
        else if (uiUs <= 1500000)
            m_ui1500++; 
        else if (uiUs <= 5000000)
            m_ui5000++; 
        else 
            m_uiTmOut++; 
        return 0;                                         
    }
    
    void clear(long lMin=0) {
        memset((void *)&m_ui100, 0x00, sizeof(StressDistribute));
        m_lTime = lMin;    
    }  
 
};

struct StressDistriStruct : public StressDistribute
{
    int m_iNeID;
    int m_iCmdID;
    
    void assign(StressDistribute *pStress, int iNeID, int iCmdID) {
        memcpy((void *)&m_ui100, (const void *)pStress, sizeof(StressDistribute));
        m_iNeID = iNeID;
        m_iCmdID = iCmdID;    
    }    
};    


//信息点内存统计块 提供写内存的API 由业务程序调用
class MessBlock
{

public:
    
    #define TM_CNT 4
    #define LOC_CNT 40
    #define NE_CNT 16
    #define CMD_CNT 256
    
    //信息点统计类型
    #define MESS_RCV    0
    #define MESS_LOAD   1
    #define MESS_INDB   2
    #define MESS_RLS    3
    #define MESS_UPDATE 4

    MessBlock();
    
    ~MessBlock();
    
    //初始化 必须调用
    void init();
    
    //封装给DCC程序调用
    void rcvAddMess(long lTm, int iNeID, int iRltCode, int iCmdID, int iQryType=0);
    //封装给DCC程序调用
    void sndAddMess(long lTm, int iNeID, int iRltCode, int iCmdID, int iQryType=0);
    
    /*
    *  函数说明： 信息点数据接收平衡统计
    *  参数说明： lTm：  入系统时间精确到微妙 直接取HSSOBJECT 字段m_lInTm
    *             iLoc： 地区编码 取0吧 这个应该放到外部去做
    *             iNeID： 接入网元编码
    *             iType:   #define RECV_MSG 0
                           #define SUCC_MSG 1   
                           #define RJCT_MSG 2
                           #define FAIL_MSG 3        
    */
    int addRcv(long lTm, int iLoc, int iNeID, int iType) {
        return add(lTm, iLoc, iNeID, MESS_RCV, iType);     
    }
    void getRcv(std::vector<BalanceMessStruct> &vRcv, long lInTm=0);
    //获得当前15分钟之内的    
    void getCurRcv(std::vector<BalanceMessStruct> &vRcv, long lInTm=0);    
        
    /*
    *  函数说明： 信息点数据加载平衡统计
    *  参数说明： lTm：  入系统时间精确到微妙 直接取HSSOBJECT 字段m_lInTm
    *             iLoc： 地区编码 取0吧 这个应该放到外部去做
    *             iNeID： 接入网元编码
    *             iType:   #define RECV_MSG 0
                           #define SUCC_MSG 1   
                           #define RJCT_MSG 2
                           #define FAIL_MSG 3        
    */
    int addLoad(long lTm, int iLoc, int iNeID, int iType) {
        return add(lTm, iLoc, iNeID, MESS_LOAD, iType);    
    }
    void getLoad(std::vector<BalanceMessStruct> &vRcv, long lInTm=0);
    void getCurLoad(std::vector<BalanceMessStruct> &vRcv, long lInTm=0);
    /*
    *  函数说明： 信息点数据入库平衡统计
    *  参数说明： lTm：  入系统时间精确到微妙 直接取HSSOBJECT 字段m_lInTm
    *             iLoc： 地区编码 取0吧 这个应该放到外部去做
    *             iNeID： 接入网元编码
    *             iType:   #define RECV_MSG 0
                           #define SUCC_MSG 1   
                           #define RJCT_MSG 2
                           #define FAIL_MSG 3        
    */
    int addInDB(long lTm, int iLoc, int iNeID, int iType) {
        return add(lTm, iLoc, iNeID, MESS_INDB, iType);    
    }
    void getInDB(std::vector<BalanceMessStruct> &vRcv, long lInTm=0);
        
    /*
    *  函数说明： 信息点数据发布平衡统计
    *  参数说明： lTm：  入系统时间精确到微妙 直接取系统当前时间
    *             iLoc： 地区编码 取0吧 这个应该放到外部去做
    *             iNeID： 接入网元编码
    *             iType:   #define RECV_MSG 0
                           #define SUCC_MSG 1   
                           #define RJCT_MSG 2
                           #define FAIL_MSG 3        
    */
    int addRls(long lTm, int iLoc, int iNeID, int iType) {
        return add(lTm, iLoc, iNeID, MESS_RLS, iType);    
    }
    void getRls(std::vector<BalanceMessStruct> &vRcv, long lInTm=0);
    void getCurRls(std::vector<BalanceMessStruct> &vRcv, long lInTm=0);    
    //实例变更统计
    /*
    *  函数说明： 信息点实例变更统计
    *  参数说明： lTm：  入系统时间精确到微妙 直接取系统当前时间
    *             iLoc： 地区编码 取0吧 这个应该放到外部去做
    *             iNeID： 接入网元编码
    *             iType:   #define UDT_CUST      10
                           #define UDT_USER      11
                           #define UDT_ACCT      12
                           #define UDT_PRODOFFER 13       
    */
    int addUpdate(long lTm, int iLoc, int iNeID, int iType,int iNum=1) {
        return add(lTm, iLoc, iNeID, MESS_UPDATE, iType, iNum);    
    }
    void getUpdate(std::vector<InstUpdateMessStruct> &vRcv, long lInTm=0);

    //查询总量统计
    //lTm：  入系统时间精确到微妙 直接取系统当前时间
    //iType  见上面 查询接口性能统计类型 宏定义
    int addQry(long lTm, int iType);
    //查询异常统计
    //lTm：  入系统时间精确到微妙 直接取系统当前时间
    //iType  见上面 查询接口性能统计类型 宏定义
    int addQryExcp(long lTm, int iType);
    
    void getQry(std::vector<QryKPI> &vQry, long lInTm=0);
    
    //2011-07-14 新增统计 每分钟 查询 和 同步按分钟统计入，处理正常 处理失败  拒收
    void addQryKPI(long lTm, int iItem);
    
    void addSynKPI(long lTm, int iItem);
    
    void getQS(std::vector<BalanceMess> &vQry, std::vector<BalanceMess> &vSyn, long lInTm=0);
    
    //lInTm 入系统时间 从hssobject.m_lInTime取值精确到微妙 在入系统的时候赋值的
    //iNeID 网元ID
    //iOpType 0为查询 1为更新  后续可以修改成CMD_ID    
    int addStress(long lInTm, int iNeID, int iCmdID); 
    void getStress(std::vector<StressDistriStruct> &vStress, long lInTm=0);  
    
private:

    int add(long lTm, int iLoc, int iNeID, int iAct, int iType, int iCnt=0);
    
private:    

    BalanceMess m_oRcv[LOC_CNT][NE_CNT][TM_CNT];    //数据接收平衡
    BalanceMess m_oLoad[LOC_CNT][NE_CNT][TM_CNT];   //数据加载平衡
    BalanceMess m_oInDB[LOC_CNT][NE_CNT][TM_CNT];   //数据入库平衡
    BalanceMess m_oRls[LOC_CNT][NE_CNT][TM_CNT];    //数据发布平衡
    
    InstUpdateMess m_oUpdate[LOC_CNT][NE_CNT][TM_CNT];    //实例数变更平衡
    
    QryKPI m_oQry[TM_CNT];                               //查询接口统计
    
    BalanceMess m_oQS[2][TM_CNT];                         //查询 同步 按15分钟统计 跟 m_oRcv重复了 暂时这样做吧
    
    StressDistribute m_oStress[CMD_CNT][NE_CNT][TM_CNT];       //压力分布
};

#endif/*__SHM_HSS_KPI_H_INCLUEDE_*/
