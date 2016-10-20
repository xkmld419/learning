/*VER: 2*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

/*

	2008-05-22 注释掉不要用的
		class ServProdAttr

*/

#ifndef STDEVENT_H_HEADER_INCLUDED_BDB42691
#define STDEVENT_H_HEADER_INCLUDED_BDB42691

#include "CommonMacro.h"

class ChargeInfo;
class ServProductAttr;

//##ModelId=423E82E30286
//##Documentation
//## 针对每个费用域求取对应的批价信息。
struct StdChargeInfo
{
	long m_lStdCharge;
	int m_iStdAcctItemTypeID;
};
struct ChargeInfo
{
	long m_lCharge;
	int  m_iAcctItemTypeID;
	long m_lBillMeasure;
	int  m_iMeasureType;
	long m_lAcctID;
	long m_lOfferInstID;
    int  m_iItemSourceID;
};

#ifdef EVENT_CHARGE_ACCU_EXCEEDED
struct AccuItemInfo
{
    long m_lEventAggrID;
    long m_lOfferInstID;
    long m_lValue;
};
#endif

/*

//##ModelId=423F70E901A2
//##Documentation
//## 附属产品属性数据结构
class ServProdAttr
{
public:
    //##ModelId=423F718E027C
    //##Documentation
    //## 属性标识
    int m_iAttrID;

    //##ModelId=423F71B00022
    //##Documentation
    //## 属性取值
    char m_sAttrVal[MAX_ATTR_VAL_LEN];

    //##ModelId=423F71CD010A
    //##Documentation
    //## 属性生效时间
    char m_sAttrEffDate[MAX_ATTR_EFF_DATE_LEN];

    //##ModelId=423F71DA028F
    //##Documentation
    //## 属性失效时间
    char m_sAttrExpDate[MAX_ATTR_EXP_DATE_LEN];

};

*/


//##ModelId=4257605C01D2
//##Documentation
//## 事件结构扩展
//## 目的: 存放应用模块内的事件临时属性
class StdEventExt
{
  public:
    
    long m_lExtendValue[10];
    char m_sExtendValue[10][MAX_EXT_STRING_LEN];
    
    //##ModelId=425762460140
    //##Documentation
    //## 漫游类型
    int m_iRoamTypeID;
	// 边界漫游-1/同城漫游-2/非漫游-0/漫游3(需要细分)
	// 漫游细分: 省内-30/国内-31/国际-32



    int m_iStdRoamTypeID;	
    int m_iStdLongTypeID;



    //##ModelId=4273190B009C
    //##Documentation
    //## 长途类型
    int m_iLongTypeID;
    // 长途-1(需要细分)/非长途(市话)-2
    // 长途细分: 省内长途-10/国内长途-11/普通国际-12/十三国-13/美加-14/港澳台-15
    
    //##ModelId=42732006013C
    //##Documentation
    //## 预处理正在处理的文件名
    char m_sFileName[MAX_FILE_NAME_LEN];

    //##ModelId=4273252403B7
    //##Documentation
    //## 存放源事件类型
    int m_iSourceEventType;
    
    //##ModelId=42732B8603DE
    //##Documentation
    //## 网络承载类型
    int m_iCarrierTypeID;

    //##ModelId=427BA09B039E
    int m_iCallingLongGroupTypeID;

    //##ModelId=427BA0DD01EB
    int m_iCalledLongGroupTypeID;

    //##ModelId=4281779C02E9
    //##Documentation
    //## 主叫运维机构ID
    int m_iCallingOrgID;

    //##ModelId=428177BB03B6
    //##Documentation
    //## 被叫运维机构ID
    int m_iCalledOrgID;

    //##ModelId=4289582C01E3
    //##Documentation
    //## 事件原始序列号
    char m_sOrgSequence[MAX_ORG_SEQ_LEN];

    //##ModelId=42895853015D
    //##Documentation
    //## 事件部分指示 
    int m_iPartialIndicator;

	
	//中间记录序列号，从1开始计数
	int m_iMiddleSequnce;
	
	//## 交换机长途类型
	int m_iSwitchLongType;
	
	//为1话单不经过合并，立即进入计费系统，要求2分钟之内计费完毕
	int  m_iHotBillingTag;
	
	//挂断原因，用于判断部分话单的结束标识（为0为正常结束）
	int m_iCausefortermination;
	
	//## 交换机区号
	char m_sSwitchAreaCode[MAX_CALLING_AREA_CODE_LEN];
	
	//主叫到访地区号  
	char   m_sCallingVisitAreaCode[MAX_CALLING_VISIT_AREA_CODE_LEN];

	//被叫到访地区号  
	char   m_sCalledVisitAreaCode[MAX_CALLED_VISIT_AREA_CODE_LEN];
    
    //第三方号码到访地区号
    char m_sThirdPartyVisitAreaCode[MAX_CALLED_VISIT_AREA_CODE_LEN];
    
	//IMSI    
	char m_sIMSI[MAX_IMSI_LEN];
	
	//IMEI    
	char m_sIMEI[MAX_IMEI_LEN];

	//接入点名称
	char m_sAccessPointName[MAX_ACCESS_POINT_NAME_LEN];
	
	//计费节点ID
	int m_iNodeID;
};


//##ModelId=41CF742F03B3
//##Documentation
//## 此结构采用系统所涉及的所有事件属性的全集，另外再加一部分保留字段。
class StdEvent
{
  public:
    
    //##ModelId=4236525C0222
    //##Documentation
    //## 事件标识
    long m_lEventID;
	//wuan 2010-07-14
    #ifdef ORG_BILLINGCYCLE
    int m_iOrgBillingCycleID;
    #endif
    //##ModelId=423652AB03C0
    //##Documentation
    //## 帐期标识
    int m_iBillingCycleID;

    //##ModelId=423652000356
    //##Documentation
    //## 事件类型标识
    int m_iEventTypeID;

    //##ModelId=423652ED002C
    //##Documentation
    //## 计费方区号
    char m_sBillingAreaCode[MAX_BILLING_AREA_CODE_LEN];
	
    //##ModelId=4236530B006B
    //##Documentation
    //## 计费方号码
    char m_sBillingNBR[MAX_BILLING_NBR_LEN];

    //##ModelId=42366C2F032C
    //##Documentation
    //## 计费方到访地区号
    char m_sBillingVisitAreaCode[MAX_BILLING_VISIT_AREA_CODE];

    //##ModelId=423653260286
    //##Documentation
    //## 主叫方区号
    char m_sCallingAreaCode[MAX_CALLING_AREA_CODE_LEN];

    //##ModelId=423653350133
    //##Documentation
    //## 主叫方号码
    char m_sCallingNBR[MAX_CALLING_NBR_LEN];

    //##ModelId=423653350133
    //##Documentation
    //## 主叫方物理号码
    char m_sCallingPhysicalNBR[MAX_CALLING_NBR_LEN];

    //##ModelId=4236537F02CA
    //##Documentation
    //## 原始的主叫方号码
    char m_sOrgCallingNBR[MAX_ORG_CALLING_NBR_LEN];
    
     //##ModelId=4236537F02CA
    //##Documentation
    //## 原始的主叫物理号码
    char m_sOrgCallingPhysicalNBR[MAX_ORG_CALLING_NBR_LEN];

    //##ModelId=4236534D037D
    //##Documentation
    //## 主叫运营商类型标识
    int m_iCallingSPTypeID;

    //##ModelId=42365370017E
    //##Documentation
    //## 主叫业务类型标识
    int m_iCallingServiceTypeID;

    //##ModelId=4236546502FD
    //##Documentation
    //## 被叫区号
    char m_sCalledAreaCode[MAX_CALLED_AREA_CODE_LEN];

    //##ModelId=423654730127
    //##Documentation
    //## 被叫号码
    char m_sCalledNBR[MAX_CALLED_NBR_LEN];
    
    //##ModelId=423654730127
    //##Documentation
    //## 被叫物理号码
    char m_sCalledPhysicalNBR[MAX_CALLED_NBR_LEN];

    //##ModelId=423654A20020
    //##Documentation
    //## 原始的被叫号码
    char m_sOrgCalledNBR[MAX_ORG_CALLED_NBR_LEN];
    
    //##ModelId=423654A20020
    //##Documentation
    //## 原始的被叫物理号码
    char m_sOrgCalledPhysicalNBR[MAX_ORG_CALLED_NBR_LEN];

    //##ModelId=42366B6502E6
    //##Documentation
    //## 被叫运营商类型标识
    int m_iCalledSPTypeID;

    //##ModelId=42366B8A03BB
    //##Documentation
    //## 被叫业务类型标识
    int m_iCalledServiceTypeID;

    //##ModelId=42366B2302F5
    //##Documentation
    //## 第三方号码区号
    char m_sThirdPartyAreaCode[MAX_THIRD_PARTY_AREA_CODE_LEN];

    //##ModelId=42366BC70066
    //##Documentation
    //## 第三方号码
    char m_sThirdPartyNBR[MAX_THIRD_PARTY_NBR_LEN];
    
     //##ModelId=42366BC70066
    //##Documentation
    //## 第三方物理号码
    char m_sThirdPartyPhysicalNBR[MAX_THIRD_PARTY_NBR_LEN];

    //##ModelId=42366BE90263
    //##Documentation
    //## 事件起始时间
    char  m_sStartDate[MAX_START_DATE_LEN];


    //##ModelId=42366BFC0329
    //##Documentation
    //## 事件结束时间
    char  m_sEndDate[MAX_END_DATE_LEN];

    //##ModelId=42366C05014B
    //##Documentation
    //## 事件时长
    //int m_iDuration;
    long  m_lDuration;
    
    //IP用户类型
    int m_iIPType;
    
    // 下行流量
    long  m_lRecvAmount;
    
    // 上行流量
    long  m_lSendAmount;
    
    //IPv6下行流量
    long  m_lRecvAmountV6;
    
    //IPv6上行流量
    long  m_lSendAmountV6;
    
    //IPv4总流量
    long  m_lSumAmountV4;
    
    //IPv6总流量
    long  m_lSumAmountV6;
    
    //## 总流量
    long  m_lSumAmount;

    //##ModelId=42366C680270
    //##Documentation
    //## 入中继号
    char m_sTrunkIn[MAX_TRUNK_IN_LEN];

    //##ModelId=42366C740368
    //##Documentation
    //## 入中继类型标识
    int m_iTrunkInTypeID;

    //##ModelId=42366CAE01B2
    //##Documentation
    //## 出中继号
    char m_sTrunkOut[MAX_TRUNK_OUT_LEN];

    //##ModelId=42366CBD02FE
    //##Documentation
    //## 出中继类型
    int m_iTrunkOutTypeID;

    //##ModelId=42366CF703D4
    //##Documentation
    //## 文件标识
    int m_iFileID;

    //##ModelId=42366D070341
    //##Documentation
    //## 事件状态: 表示事件来源, 回收时用.  如 黑户回收/错单回收
    char m_sEventState[MAX_EVENT_STATE_LEN];

    //##ModelId=42366D2D0378
    //##Documentation
    //## 事件到达计费系统时间
    char  m_sCreatedDate[MAX_CREATED_DATE_LEN];

    //##ModelId=42366D350301
    //##Documentation
    //## 事件确定状态的时间
    char   m_sStateDate[MAX_STATE_DATE_LEN];

    //##ModelId=42366D4E01DB
    //##Documentation
    //## 卡标识
    char m_sCardID[MAX_CARD_ID_LEN];

    //##ModelId=42366D56024A
    //##Documentation
    //## 计费类型标识，用来标识预付费和后付费
    int m_iBillingTypeID;

    //##ModelId=42366D7701CF
    //##Documentation
    //## 合作方标识
    int m_iParterID;

    //##ModelId=42366DCA030F
    //##Documentation
    //## 用户标识
    long m_iServID;

    //##ModelId=42366DD30109
    //##Documentation
    //## 客户标识
    long m_iCustID;

    //##ModelId=42366E080282
    //##Documentation
    //## 产品标识
    int m_iProdID;

    //##ModelId=423F733A0318
    //##Documentation
    //## 附属产品标识
    long m_iServProdID;

    //##ModelId=423E788002F0
    long  m_lOffers[MAX_OFFER_NUM];

    //##ModelId=42367F7302B6
    //##Documentation
    //## 算费过程记载
    char m_sBillingRecord[MAX_BILLING_RECORD_LEN];

    //##ModelId=42367F7F00FB
    //##Documentation
    //## 错误号标识
    int m_iErrorID;

    //##ModelId=4236805800CB
    //##Documentation
    //## 数据业务帐号所在地区区号
    char m_sAccountAreaCode[MAX_ACCOUNT_AREA_CODE_LEN];

    //##ModelId=4236807100A9
    //##Documentation
    //## 数据业务帐号
    char m_sAccountNBR[MAX_ACCOUNT_NBR_LEN];

    //##ModelId=42368089016B
    //##Documentation
    //## 网络接入IP
    char m_sNASIp[MAX_NAS_IP_LEN];

    //##ModelId=42368233039F
    //##Documentation
    //## 网络接入端口标识
    int m_iNASPortID;

    //##ModelId=423921860255
    //##Documentation
    //## 得到的服务质量
    int m_iNegotiatedQoS;

    //##ModelId=425793160080
    //##Documentation
    //## 当前事件的扩展字段中被使用的属性个数
    int m_iExtUsedNum;

    //##ModelId=423E93DE0236
    //##Documentation
    //## 扩展字段的属性ID标志
    int    m_aiExtFieldID[MAX_EXT_FIELD_NUM];

    //##ModelId=423E93C201A0
    //##Documentation
    //## 扩展字段值数组
    char   m_asExtValue[MAX_EXT_FIELD_NUM][MAX_BYTES_PER_STRING];

    //##ModelId=424D0FFE0152
    //##Documentation
    //## 交换机标识
    int m_iSwitchID;
   
    //##ModelId=4255EDD40362
    //##Documentation
    //## 业务键(内部定义):
    int m_iBusinessKey;

    //##ModelId=423E8C540028
    //##Documentation
    //## 算费信息
    //int m_iOrgCharge[MAX_ORG_CHARGE_NUM]; 
    long m_lOrgCharge[MAX_ORG_CHARGE_NUM];

    ChargeInfo m_oChargeInfo[MAX_CHARGE_NUM];
    
    StdChargeInfo m_oStdChargeInfo[MAX_CHARGE_NUM];
    
    //##ModelId=426481FE004B
    //##Documentation
    //## 计费方所在运维组织机构
    int m_iBillingOrgID;

    //##ModelId=4282D21C006E
    //##Documentation
    //## 呼叫类型:  1-主叫话单  2-被叫话单
    int m_iCallTypeID;

    //##ModelId=428D37320285
    int m_iPricingCombineID;
    
  
    // 黄晓颖6月16日增加,由于无法连上VSS,先增加
    // m_iEventSourceType字段只是预处理和排重使用,由预处理赋值,作为是否进行排重处理的判断条件
    // 取值定义:
    // 原始文件:1
    // 无主回收:2
    // 预处理参数错误回收:3
    // 批价参数错误回收:4
    // 重单回收:5
    // 回退:6
    int m_iEventSourceType;
    
    //免费标志
    int m_iNoBillFlag;

	//移动交换机标识  
	char  m_sMSC[MAX_MSC_LEN];
	
	//小区号标识  
	char  m_sLAC[MAX_LAC_LEN];
	char  m_sCalledMSC[MAX_MSC_LEN];

	char  m_sCalledLAC[MAX_LAC_LEN];
	char  m_sCalledCellID[MAX_CELLID_LEN];
	//基站号   
	char  m_sCellID[MAX_CELLID_LEN];

	//动态漫游号   
	char m_sRomingNBR[MAX_ROMING_NBR_LEN];
    
    
    //##ModelId=428956460086
    StdEventExt m_oEventExt;

	//## SP原始标识串
	char m_sSPFLAG[MAX_SPFLAG_LEN];
	
    //SP标识
    int m_iSPID;
    
    //## 3G CS域补充业务标识
    int m_iSuppleServiceCode;
    
    //## 3G CS域CUG业务标识;
    int m_iCUGCode;
    
    char m_sReservedField[MAX_RESERVED_FIELD_NUM][MAX_RESERVED_FIELD_LEN];
    
    //add by yangks 2006.12.31  增加跳次计算功能(河北需求)
    //市话次数
    int m_iTimes;
    
    int m_iNetworkID;
    
    int m_iFlowID;
    
    //add by yangks 2007.03.09 for JS
    char m_sGateWay[MAX_GATE_WAY_LEN];      /*网关信息*/
    int  m_iCallingPA;                    /*主叫pa区*/
    int  m_iCalledPA;                     /*被叫pa区*/
    char m_sCallingRP[MAX_RP_STR_LEN];        /*主叫基站*/
    char m_sCalledRP[MAX_RP_STR_LEN];         /*被叫基站*/
    int  m_iCallingRPArea;                //主叫基站区域(根据网关信息、主叫pa区、基站规整)
    int  m_iCalledRPArea;                 //被叫基站区域
    char m_sSessionID[MAX_SESSION_ID_LEN];              /*数据业务的会话标识*/
    char m_sOrgThirdPartyNBR[MAX_THIRD_PARTY_NBR_LEN];  /*原始的第三方号码*/
    char m_sOrgThirdPartyPhysicalNBR[MAX_THIRD_PARTY_NBR_LEN];
    
    //江苏联机采集提供. 要求字段入库,供稽核话单使用
    //文件内序号. 即原始话单文件中,从1开始计算的本笔话单的序号.
    int  m_iEventSeq;
    
    //数据计费(原始属性,需要入库)
    char m_sDomainName[MAX_DOMAIN_NAME_LEN]; //DOMAIN_NAME:   域名    
    int  m_iGroupID;                         //GROUP_ID:      计费组别
    int  m_iRoamFlag;                        //ROAM_FLAG :    漫游标志
    char m_sBindAttr[MAX_BIND_ATTR_LEN];     //BIND_ATTR :    绑定属性
    int  m_iServiceType;    //SERVICE_TYPE:  服务类型
    int  m_iUserType;       //USER_TYPE :    用户类型
    char m_sAreaNo[MAX_AREA_NO_LEN];         //AREANO:        用户属地
    long m_lSepRecID;       //SEP_REC_ID:    拆分话单标识,第一条记录本身的ticket_id,后面的拆分话单记录参考话单的TICKET_ID
    int  m_iSepRecSeq;      //SEP_REC_SEQ:   拆分记录序列,与rec_id一同作为拆分话单的主健
    long m_lResourceID;     //RESOURCEID :   拆分来源,分割后的话单记录本条原始话单的ticket_id；
    int  m_iQueryFlag;      //QUERY_FLAG :   查询标识,
                            //// 0－未经拆分的原始话单,开始所有话单默认为0,未参加套餐的都需要计费;
                            //// 1－经过拆分后原始话单,不计费; 2－拆分
    
    int  m_iProcessID;

#ifdef EVENT_CHARGE_ACCU_EXCEEDED
	AccuItemInfo m_oAccuInfo[MAX_ACCU_NUM];
#endif
    char m_sAccuRecord[MAX_ACCU_RECORD_LEN];    //累积量轨迹 wuan 2010-09-08
};



//模块编码时使用,联调后修改为外部引用
struct StdAcctItem {
	int iFileID; //文件回退时该帐目需要回退。
	long lEventID;//由该事件产生的帐目,事件回退时该帐目也需要回退。
	long iServID;
	long iAcctID;
	int iOrgID;
	int iBillingCycleID;
	int iAcctItemTypeID;
	long lCharge;
	int iCombineID; //定价组合ID
	char sStateDate[16]; //State Date
	int iItemGroupID;//增加账目组的返回值
};

#endif /* STDEVENT_H_HEADER_INCLUDED_BDB42691 */

