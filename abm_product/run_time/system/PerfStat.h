/*
* version 1.5 huff at 2010-05-04
*
*/
#ifndef _PERFORMANCE_STATIC_H_INCLUDE_EFFFCD
#define _PERFORMANCE_STATIC_H_INCLUDE_EFFFCD

#include "abmobject.h"

//1,2,3,4,5,6,7,8,12,16......................500,overflow
#define DURATION_NUM        132 
#define ABM_CMD_MAX         128     //ABM系统命令总数 
#define TIME_SLICE_MAX      4       //时间片(分钟) 4分钟 2的 power,必须大于3
#define DFT_INVAL_DUR       2       //默认失效时间 分钟

//#ifdef _ABM_SERVER_
#define LINK_NUM_MAX        128     //服务端最大连接数
//#endif

#ifdef _ABM_CLIENT_
#define LINK_NUM_MAX        32      //客户端最大的连接数
#endif


/*
*  内存单元 时间片内存布局
*/
struct StatUnitData
{
    long m_lTotalDur;                   // 时长  单位0.1毫秒
	long m_lNetDur;                     //客户端用于计算网络延迟 单位微妙
    int  m_iMaxWDur;                    //包括来回的最大时长
    int  m_iMinWDur;                    //包括来回的最小时长
    int  m_iMaxNDur;                    //最大的网络延时时长
    int  m_iMinNDur;                    //最小的网络延时时长
    int  m_iSendNum;                    // 发送 计数
    int  m_iRecvNum;                    // 接收 计数
    int  m_iDurData[DURATION_NUM];      //处理响应时间段分布 20微妙
    int  m_iNDurData[DURATION_NUM];		//网络处理响应时间段分布 20微妙
    int  m_iODurData[DURATION_NUM];		//纯业务处理响应时间段分布 20微妙
    int  m_iNext;                       //对于有数值的形成链表，方便读取
    //1,2,3,4,5,6,7,8,12,16......................500,overflow
};

//内存中的所有命令的一起的统计
struct StatCmdData
{
    long m_lTime;                       // yyyymmddhh24mi 转换成 秒 ,表示时间
    int  m_iSendNum;                    //发送的所有命令总数
    int  m_iRecvNum;                    //接收的对应的总数
    int  m_iSendPack;                   //发送的网络包数
    int  m_iSendPackContNum;            //发送的网络包数包含的命令/结果数
    int  m_iRecvPack;                   //含义为：某时间收到网络包数
    int  m_iRecvPackContNum;            //含义为该时间收到的网络包数包含的数；
    StatUnitData m_oUnitData[ABM_CMD_MAX];
    int  m_iHead;                       //指向第一个有值的命令
};

//一个连接的统计数据
struct StatBlockData
{
    int m_iLinkID;                              //以link_id作为统计单元
    StatCmdData m_oCmdData[TIME_SLICE_MAX+1];   //每个节点的内存布局 包含256个命令
};

//所有的统计数据
struct StatData
{
    StatBlockData m_oData[LINK_NUM_MAX];
};

struct OnlineStatData {
    int m_iLinkID;
    long m_lTime;
    int m_iCmdID;

};

//性能统计和计数数据主要为了后续的展现和分析
//帮助发现可能存在的潜在问题
class StatDataMgr {
public:
    //初始化
    int init(StatData *pMem) { m_poData = pMem; return 0; }

    //在没有前台界入的情况下，不需要实时监控的时候，
    //通过调用如下的接口；如下的接口约每分钟产生返回结果；
    //调用方法：外部sleep；尝试调用
    //返回0 ： 有一条数据返回 ; 当有数据返回时，不要sleep，直接获取下一次
    //其他-1： 无数据返回
    //-2: 参数错误
    int getOfflineData(StatCmdData &oData, int &iLinkID);

    //在实时监控的场景下；需要不断地获取新的数据
    //
    // 实时的思路，是按界面的需要进行数据获取；
    // 20号后实现
    // 前台需要什么样的数据，外界传相应的获取要求，返回相应的当时的数据
    //int getOnlineData(OnlineStatdata & data);
private:
    StatData * m_poData;
};

class PerfStat {
public:

    PerfStat() : m_poData(0), m_lRClockTime(0), m_lSClockTime(0) {}
    
    //根据通讯link_id, 内存由外部分配,
    //传入整个结构体的首指针
    int init( StatData * pMem, int iLinkID);
    
    //取处理响应时间段分布点位置
    int getDurDataPoint(long dur);

    //瞬间可能有多个消息并发处理，没有必要多次getdayoftime，所以外部进行显示调用
    //所以，方法内部放置一个时钟，由外部显式更新
    int updSendClock(); 
    int updRecvClock(); 

    //客户端的统计调用
    
    //客户端向服务端发送的时候，进行调用
    int statClntSend(ABMObject *pObject);
    //客户端从服务端获取的时候的调用
    int statClntRecv(ABMObject *pObject);

    //服务端的统计调用

    //服务端接收到客户端的命令时，进行调用
    int statSrvRecv(ABMObject *pObject);
    //服务端返回客户端命令时，进行调用
    int statSrvSend(ABMObject *pObject);

    //
    int statSendPack(int packnum, int sendnum);
    int statRecvPack(int packnum, int recvnum);
    
private:
	
    long m_lSClockTime;
    long m_lRClockTime;
    StatBlockData *m_poData;    //写指针
};

#endif

