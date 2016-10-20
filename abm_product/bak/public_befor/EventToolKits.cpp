/*VER: 5*/ 
#include "EventToolKits.h"
#include <algorithm>

EventToolKits::EventToolKits()
{
}

EventToolKits::~EventToolKits()
{
}
// 函数说明：按照Event_Attr获取StdEvent结构数据以及数据类型
// 参数说明：
//         const StdEvent *pStd    输入事件
//        int iEventAttrId        属性ID
//        int &iType                类型返回值 0-null,1-int,2-long,3-char
// 返回值说明：
//        返回属性数据
string EventToolKits::get_param( const StdEvent *pStd, int iEventAttrId, int &iType )
{
     switch ( iEventAttrId )
     {
     case 1 : // EventID - 事件标识
          iType = 2;
          return toString( pStd->m_lEventID );

     case 2 : // BillingCycleID - 帐期标识
          iType = 1;
          return toString( pStd->m_iBillingCycleID );

     case 3 : // EventTypeID - 事件类型标识
          iType = 1;
          return toString( pStd->m_iEventTypeID );

     case 4 : // BillingAreaCode - 计费方区号
          iType = 3;
          return string( pStd->m_sBillingAreaCode );

     case 5 : // BillingNBR - 计费方号码
          iType = 3;
          return string( pStd->m_sBillingNBR );

     case 6 : // BillingVisitAreaCode - 计费方到访地区号
          iType = 3;
          return string( pStd->m_sBillingVisitAreaCode );

     case 7 : // CallingAreaCode - 主叫方区号
          iType = 3;
          return string( pStd->m_sCallingAreaCode );

     case 8 : // CallingNBR - 主叫方号码
          iType = 3;
          return string( pStd->m_sCallingNBR );

     case 9 : // OrgCallingNBR - 原始的主叫方号码
          iType = 3;
          return string( pStd->m_sOrgCallingNBR );

     case 10 : // CallingSPTypeID - 主叫运营商类型标识
          iType = 1;
          return toString( pStd->m_iCallingSPTypeID );

     case 11 : // CallingServiceTypeID - 主叫业务类型标识
          iType = 1;
          return toString( pStd->m_iCallingServiceTypeID );

     case 13 : // CalledAreaCode - 被叫区号
          iType = 3;
          return string( pStd->m_sCalledAreaCode );

     case 14 : // CalledNBR - 被叫号码
          iType = 3;
          return string( pStd->m_sCalledNBR );

     case 15 : // OrgCalledNBR - 原始的被叫方号码
          iType = 3;
          return string( pStd->m_sOrgCalledNBR );

     case 16 : // CalledSPTypeID - 被叫运营商类型标识
          iType = 1;
          return toString( pStd->m_iCalledSPTypeID );

     case 17 : // CalledServiceTypeID - 被叫业务类型标识
          iType = 1;
          return toString( pStd->m_iCalledServiceTypeID );

     case 19 : // ThirdPartyAreaCode - 第三方号码区号
          iType = 3;
          return string( pStd->m_sThirdPartyAreaCode );

     case 20 : // ThirdPartyNBR - 第三方号码
          iType = 3;
          return string( pStd->m_sThirdPartyNBR );

     case 21 : // StartDate - 事件起始时间
          iType = 3;
          return string( pStd->m_sStartDate );

     case 22 : // EndDate - 事件结束时间
          iType = 3;
          return string( pStd->m_sEndDate );

     case 23 : // Duration - 事件时长
          iType = 1;
          return toString( pStd->m_lDuration );

     case 24 : // SwitchID - 交换机标识
          iType = 1;
          return toString( pStd->m_iSwitchID );

     case 25 : // TrunkIn - 入中继号
          iType = 3;
          return string( pStd->m_sTrunkIn );

     case 26 : // TrunkInTypeID - 入中继类型标识
          iType = 1;
          return toString( pStd->m_iTrunkInTypeID );

     case 27 : // TrunkOut - 出中继号
          iType = 3;
          return string( pStd->m_sTrunkOut );

     case 28 : // TrunkOutTypeID - 出中继类型
          iType = 1;
          return toString( pStd->m_iTrunkOutTypeID );

     case 29 : // FileID - 文件标识
          iType = 1;
          return toString( pStd->m_iFileID );

     case 30 : // EventState - 事件状态
          iType = 3;
          return string( pStd->m_sEventState );

     case 31 : // CreatedDate - 事件到达计费系统时间
          iType = 3;
          return string( pStd->m_sCreatedDate );

     case 32 : // StateDate - 事件确定状态的时间
          iType = 3;
          return string( pStd->m_sStateDate );

     case 33 : // CardID - 卡标识
          iType = 3;
          return string( pStd->m_sCardID );

     case 34 : // BillingTypeID - 计费类型标识
          iType = 1;
          return toString( pStd->m_iBillingTypeID );

     case 35 : // ParterID - 合作方标识
          iType = 1;
          return toString( pStd->m_iParterID );

     case 36 : // ServID - 用户标识
          iType = 1;
          return toString( pStd->m_iServID );

     case 37 : // CustID - 客户标识
          iType = 1;
          return toString( pStd->m_iCustID );

     case 38 : // ProductID - 产品标识
          iType = 1;
          return toString( pStd->m_iProdID );

     case 39 : // ServProductID - 附属产品标识
          iType = 1;
          return toString( pStd->m_iServProdID );

//      case 40 : // ProductBundleID - 产品包标识
//           iType = 1;
//           return toString( pStd->m_iProdBundleID );

     case 41 : // Offers[0] - 商品标识1
          iType = 1;
          return toString( pStd->m_lOffers[0] );

     case 42 : // Offers[1] - 商品标识2
          iType = 1;
          return toString( pStd->m_lOffers[1] );

     case 43 : // Offers[2] - 商品标识3
          iType = 1;
          return toString( pStd->m_lOffers[2] );

     case 44 : // Offers[3] - 商品标识4
          iType = 1;
          return toString( pStd->m_lOffers[3] );

     case 45 : // Offers[4] - 商品标识5
          iType = 1;
          return toString( pStd->m_lOffers[4] );

     case 46 : // BillingRecord - 算费过程记录
          iType = 3;
          return string( pStd->m_sBillingRecord );

     case 47 : // ErrorID - 错误号标识
          iType = 1;
          return toString( pStd->m_iErrorID );

     case 48 : // AccountAreaCode - 数据业务帐号所在地区区号
          iType = 3;
          return string( pStd->m_sAccountAreaCode );

     case 49 : // AccountNBR - 数据业务帐号
          iType = 3;
          return string( pStd->m_sAccountNBR );

     case 50 : // NASIp - 网络接入IP
          iType = 3;
          return string( pStd->m_sNASIp );

     case 51 : // NASPortID - 网络接入端口标识
          iType = 1;
          return toString( pStd->m_iNASPortID );

     case 52 : // RecvBytes - 接收字节数
          iType = 1;
          return toString( pStd->m_lRecvAmount );

     case 53 : // SendBytes - 发送字节数
          iType = 1;
          return toString( pStd->m_lSendAmount );

     //case 54 : // RecvPackets - 接收包数
     //     iType = 1;
     //     return toString( pStd->m_iRecvPackets );
     //
     //case 55 : // SendPackets - 发送包数
     //     iType = 1;
     //     return toString( pStd->m_iSendPackets );
     
     case 56 : // NegotiatedQoS - 服务质量
          iType = 1;
          return toString( pStd->m_iNegotiatedQoS );

     //case 57 : // CycleBeginDate - 帐期起始时间
     //     iType = 3;
     //     return string( pStd->m_sCycleBeginDate );
     //
     //case 58 : // CycleEndDate - 帐期结束时间
     //     iType = 3;
     //     return string( pStd->m_sCycleEndDate );

     case 59 : // ExtUsedNum - 扩展属性字段使用个数
          iType = 1;
          return toString( pStd->m_iExtUsedNum );

     case 60 : // BusinessKey - 业务键
          iType = 1;
          return toString( pStd->m_iBusinessKey );

//      case 61 : // SerProdAttrUsedNum -  附属产品属性实际使用的数目
//           iType = 1;
//           return toString( pStd->m_iSerProdAttrUsedNum );

     case 62 : // ChargeInfo[0].m_lOrgCharge - 第1个费用域的原始费用
          iType = 1;
          return toString( pStd->m_lOrgCharge[0] );

     case 63 : // StdChargeInfo[0].m_iStdCharge - 第1个费用域的标准费用
          iType = 1;
          return toString( pStd->m_oStdChargeInfo[0].m_lStdCharge );

     case 64 : // ChargeInfo[0].m_iCharge - 第1个费用域的最终费用
          iType = 1;
          return toString( pStd->m_oChargeInfo[0].m_lCharge );

     case 65 : // ChargeInfo[0].m_iAcctItemTypeID - 第1个费用域的帐目类型标识
          iType = 1;
          return toString( pStd->m_oChargeInfo[0].m_iAcctItemTypeID );

     case 66 : // ChargeInfo[0].m_iAcctItemTypeID - 第1个费用域的优惠帐目类型标识
          iType = 1;
          return toString( pStd->m_oChargeInfo[0].m_iAcctItemTypeID );

     case 67 : // ChargeInfo[1].m_lOrgCharge - 第2个费用域的原始费用
          iType = 1;
          return toString( pStd->m_lOrgCharge[1] );

     case 68 : // StdChargeInfo[1].m_iStdCharge - 第2个费用域的标准费用
          iType = 1;
          return toString( pStd->m_oStdChargeInfo[1].m_lStdCharge );

     case 69 : // ChargeInfo[1].m_iCharge - 第2个费用域的最终费用
          iType = 1;
          return toString( pStd->m_oChargeInfo[1].m_lCharge );

     case 70 : // ChargeInfo[1].m_iAcctItemTypeID - 第2个费用域的帐目类型标识
          iType = 1;
          return toString( pStd->m_oChargeInfo[1].m_iAcctItemTypeID );

     case 71 : // ChargeInfo[1].m_iAcctItemTypeID - 第2个费用域的优惠帐目类型标识
          iType = 1;
          return toString( pStd->m_oChargeInfo[1].m_iAcctItemTypeID );

//      case 72 : // ServProdAttr[0].m_iAttrID - 第1个附属产品信息的属性标识
//           iType = 1;
//           return toString( pStd->m_iServProdAttr[0].m_iAttrID );

//      case 73 : // ServProdAttr[0].m_iAttrVal - 第1个附属产品信息的属性取值
//           iType = 3;
//           return string( pStd->m_sServProdAttr[0].m_iAttrVal );

//      case 74 : // ServProdAttr[0].m_iAttrEffDate - 第1个附属产品信息的生效时间
//           iType = 3;
//           return string( pStd->m_sServProdAttr[0].m_iAttrEffDate );

//      case 75 : // ServProdAttr[0].m_iAttrExpDate - 第1个附属产品信息的失效时间
//           iType = 3;
//           return string( pStd->m_sServProdAttr[0].m_iAttrExpDate );

//      case 76 : // ServProdAttr[1].m_iAttrID - 第2个附属产品信息的属性标识
//           iType = 1;
//           return toString( pStd->m_iServProdAttr[1].m_iAttrID );

//      case 77 : // ServProdAttr[1].m_iAttrVal - 第2个附属产品信息的属性取值
//           iType = 3;
//           return string( pStd->m_sServProdAttr[1].m_iAttrVal );

//      case 78 : // ServProdAttr[1].m_iAttrEffDate - 第2个附属产品信息的生效时间
//           iType = 3;
//           return string( pStd->m_sServProdAttr[1].m_iAttrEffDate );

//      case 79 : // ServProdAttr[1].m_iAttrExpDate - 第2个附属产品信息的失效时间
//           iType = 3;
//           return string( pStd->m_sServProdAttr[1].m_iAttrExpDate );

     case 80 : // CallTypeID - 呼叫类型标识
          iType = 1;
          return toString( pStd->m_iCallTypeID );

     case 81 : // BillingOrgID - 计费方运营组织标识
          iType = 1;
          return toString( pStd->m_iBillingOrgID );

//      case 82 : // BillFlowID - 计费流程标识
//           iType = 1;
//           return toString( pStd->m_iBillFlowID );

     case 83 : // ChargeInfo[0].m_lBillMeasure - 第1个费用域的计费量
          iType = 1;
          return toString( pStd->m_oChargeInfo[0].m_lBillMeasure );

     case 84 : // ChargeInfo[1].m_lBillMeasure - 第2个费用域的计费量
          iType = 1;
          return toString( pStd->m_oChargeInfo[1].m_lBillMeasure );

     case 201 : // OrgSequence - 事件原始序列号
          iType = 3;
          return string( pStd->m_oEventExt.m_sOrgSequence );

     case 202 : // PartialIndicator - 事件部分指示
          iType = 1;
          return toString( pStd->m_oEventExt.m_iPartialIndicator );

     case 203 : // MiddleSequnce - 中间记录序列号
          iType = 1;
          return toString( pStd->m_oEventExt.m_iMiddleSequnce );

//      case 204 : // RecordIndex - 话单记录原始主索引
//           iType = 1;
//           return toString( pStd->m_oEventExt.m_iRecordIndex );

     case 205 : // HotBillingTag - 热计费标识
          iType = 1;
          return toString( pStd->m_oEventExt.m_iHotBillingTag );

     case 206 : // Causefortermination - 挂断标识
          iType = 1;
          return toString( pStd->m_oEventExt.m_iCausefortermination );

     case 207 : // MSC - 移动交换机标识
          iType = 3;
          return string( pStd->m_sMSC );

     case 208 : // LAC - 小区号标识
          iType = 3;
          return string( pStd->m_sLAC );

     case 209 : // CellID - 基站号标识
          iType = 1;
          return string( pStd->m_sCellID );

     case 210 : // RomingNBR - 动态漫游号标识
          iType = 1;
          return string( pStd->m_sRomingNBR );

     case 211 : // NoBillFlag - 免费标志标识
          iType = 1;
          return toString( pStd->m_iNoBillFlag );

     case 212 : // CallingVisitAreaCode - 主叫到访地区号标识
          iType = 3;
          return string( pStd->m_oEventExt.m_sCallingVisitAreaCode );

     case 213 : // CalledVisitAreaCode - 被叫到访地区号标识
          iType = 3;
          return string( pStd->m_oEventExt.m_sCalledVisitAreaCode );

     case 214 : // IMSI - IMSI
          iType = 1;
          return string( pStd->m_oEventExt.m_sIMSI );

     case 215 : // IMEI - IMEI
          iType = 1;
          return string( pStd->m_oEventExt.m_sIMEI );

     case 216 : // ExtendID[0] - 扩展属性1标识
          iType = 3;
          return toString( pStd->m_aiExtFieldID[0] );

     case 217 : // ExtendVaule[0] - 扩展属性1取值
          iType = 3;
          return string( pStd->m_asExtValue[0] );

     case 218 : // ExtendID[1] - 扩展属性2标识
          iType = 3;
          return toString( pStd->m_aiExtFieldID[1] );

     case 219 : // ExtendValue[1] - 扩展属性2取值
          iType = 3;
          return string( pStd->m_asExtValue[1] );

     case 220 : // ExtendID[2] - 扩展属性3标识
          iType = 3;
          return toString( pStd->m_aiExtFieldID[2] );

     case 221 : // ExtendValue[2] - 扩展属性3取值
          iType = 3;
          return string( pStd->m_asExtValue[2] );

     case 222 : // ExtendID[3] - 扩展属性4标识
          iType = 3;
          return toString( pStd->m_aiExtFieldID[3] );

     case 223 : // ExtendValue[3] - 扩展属性4取值
          iType = 3;
          return string( pStd->m_asExtValue[3] );

     case 224 : // ExtendID[4] - 扩展属性5标识
          iType = 3;
          return toString( pStd->m_aiExtFieldID[4] );

     case 225 : // ExtendValue[4] - 扩展属性5取值
          iType = 3;
          return string( pStd->m_asExtValue[4] );

     case 226 : // ExtendID[5] - 扩展属性6标识
          iType = 3;
          return toString( pStd->m_aiExtFieldID[5] );

     case 227 : // ExtendValue[5] - 扩展属性6取值
          iType = 3;
          return string( pStd->m_asExtValue[5] );

     case 228 : // ExtendID[6] - 扩展属性7标识
          iType = 3;
          return toString( pStd->m_aiExtFieldID[6] );

     case 229 : // ExtendValue[6] - 扩展属性7取值
          iType = 3;
          return string( pStd->m_asExtValue[6] );

     case 230 : // ExtendID[7] - 扩展属性8标识
          iType = 3;
          return toString( pStd->m_aiExtFieldID[7] );

     case 231 : // ExtendValue[7] - 扩展属性8取值
          iType = 3;
          return string( pStd->m_asExtValue[7] );

     case 232 : // ExtendID[8] - 扩展属性9标识
          iType = 3;
          return toString( pStd->m_aiExtFieldID[8] );

     case 233 : // ExtendValue[8] - 扩展属性9取值
          iType = 3;
          return string( pStd->m_asExtValue[8] );

     case 234 : // ExtendID[9] - 扩展属性10标识
          iType = 3;
          return toString( pStd->m_aiExtFieldID[9] );

     case 235 : // ExtendValue[9] - 扩展属性10取值
          iType = 3;
          return string( pStd->m_asExtValue[9] );
//   m_sReservedField 取保留字段的值      
     case 236 :
	  iType =3 ;
	 return string(pStd->m_sReservedField[0]);
	  case 237:
    	 iType = 3;
    	 return string(pStd->m_sReservedField[1]);
    case 238:
         iType = 3;
         return string(pStd->m_sReservedField[2]);
     case 239:
         iType = 3;
         return string(pStd->m_sReservedField[3]);
     case 240:
         iType = 3;
         return string(pStd->m_sReservedField[4]);
     case 243:
 	  iType = 1;
	  return toString(pStd->m_lSumAmount); 
     case 252:
	  iType = 3;
	  return string( pStd->m_sSessionID);	     
     case 253:
     	  iType  =3;
     	  return string(pStd->m_sOrgThirdPartyNBR);
     case 256:
     	  iType=3;
     	  return string(pStd->m_sDomainName);
     case 257:
		  iType=1;
		  return toString(pStd->m_iGroupID);
	   case 259 :
			 iType = 3;
			 return string( pStd->m_sBindAttr  );     	
     case 260:
        iType =1;
        return toString(pStd->m_iServiceType);
//add by yuanp 20081029 for sms
     case 261:
     	iType =1;
		return toString(pStd->m_iUserType);
	 case 262:
		 iType=3;
		 return string(pStd->m_sAreaNo);
	 case 263:
		 iType = 1;
		 return toString(pStd->m_lSepRecID);

//      case 301 : // CR_ServiceCode[15] - 彩铃服务编码
//           iType = 3;
//           return string( pStd->m_sCR_ServiceCode[15] );
//返回stdevent.m_oEventExt.m_lEventValue的值
    case 280:
     	  iType=1;
     	  return toString(pStd->m_oEventExt.m_lExtendValue[0]);
     case 281:
     	  iType=1;
     	  return toString(pStd->m_oEventExt.m_lExtendValue[1]);
     case 282:
     	  iType=1;
     	  return toString(pStd->m_oEventExt.m_lExtendValue[2]);
     case 283:
     	  iType=1;
     	  return toString(pStd->m_oEventExt.m_lExtendValue[3]);
     case 284:
     	  iType=1;
     	  return toString(pStd->m_oEventExt.m_lExtendValue[4]);
     case 285:
     	  iType=1;
     	  return toString(pStd->m_oEventExt.m_lExtendValue[5]);
     case 286:
     	  iType=1;
     	  return toString(pStd->m_oEventExt.m_lExtendValue[6]);
     case 287:
     	  iType=1;
     	  return toString(pStd->m_oEventExt.m_lExtendValue[7]);
     case 288:
     	  iType=1;
     	  return toString(pStd->m_oEventExt.m_lExtendValue[8]);
     case 289:
     	  iType=1;
     	  return toString(pStd->m_oEventExt.m_lExtendValue[9]);
//返回stdevent.m_oEventExt.m_sEventValue的值     	  
     case 290:
     	  iType=3;
     	  return string(pStd->m_oEventExt.m_sExtendValue[0]);
     case 291:
     	  iType=3;
     	  return string(pStd->m_oEventExt.m_sExtendValue[1]);	  
     case 292:
     	  iType=3;
     	  return string(pStd->m_oEventExt.m_sExtendValue[2]);
     case 293:
     	  iType=3;
     	  return string(pStd->m_oEventExt.m_sExtendValue[3]);
     case 294:
     	  iType=3;
     	  return string(pStd->m_oEventExt.m_sExtendValue[4]);
     case 295:
     	  iType=3;
     	  return string(pStd->m_oEventExt.m_sExtendValue[5]);
     case 296:
     	  iType=3;
     	  return string(pStd->m_oEventExt.m_sExtendValue[6]);
     case 297:
     	  iType=3;
     	  return string(pStd->m_oEventExt.m_sExtendValue[7]);
     case 298:
     	  iType=3;
     	  return string(pStd->m_oEventExt.m_sExtendValue[8]);
     case 299:
     	  iType=3;
     	  return string(pStd->m_oEventExt.m_sExtendValue[9]);

      case 302 : // SPCode[10] - SP代码
           iType = 3;
           return string( pStd->m_sSPFLAG );

//      case 303 : // spservicekey[15] - SP短信业务代码
//           iType = 3;
//           return string( pStd->m_sspservicekey[15] );

//      case 304 : // SPsmstype[15] - SP短信类型
//           iType = 3;
//           return string( pStd->m_sSPsmstype[15] );

      case 400 : // FileName - 文件名
          iType = 1;
          return string(pStd->m_oEventExt.m_sFileName);
          
//      case 401 : // TimeStruct - 时间信息结构<div></div>
//           iType = 3;
//           return string( pStd->m_sTimeStruct );

     case 402 : // StartDate - 起始日期(YYYYMMDD)
          iType = 3;
          return string( pStd->m_sStartDate ).substr( 0, 8 );

     case 403 : // StartTime - 起始时间(HH24MISS)
          iType = 3;
          return string( pStd->m_sStartDate ).substr( 9 );

     case 404 : // EndDate - 结束日期(YYYYMMDD)
          iType = 3;
          return string( pStd->m_sEndDate ).substr( 0, 8 );

     case 405 : // EndTime - 结束时间(HH24MISS)
          iType = 3;
          return string( pStd->m_sEndDate ).substr( 9 );
     case 435:
     	   iType = 3;
     	   return string(pStd->m_oEventExt.m_sAccessPointName);

//      case 406 : // Up1 - 上行流量1
//           iType = 1;
//           return toString( pStd->m_iUp1 );

//      case 407 : // Down1 - 下行流量1
//           iType = 1;
//           return toString( pStd->m_iDown1 );

//      case 408 : // Up2 - 上行流量2
//           iType = 1;
//           return toString( pStd->m_iUp2 );

//      case 409 : // Down2 - 下行流量2
//           iType = 1;
//           return toString( pStd->m_iDown2 );

//      case 410 : // Up3 - 上行流量3
//           iType = 1;
//           return toString( pStd->m_iUp3 );

//      case 411 : // Down3 - 下行流量3
//           iType = 1;
//           return toString( pStd->m_iDown3 );

//      case 412 : // Up4 - 上行流量4
//           iType = 1;
//           return toString( pStd->m_iUp4 );

//      case 413 : // Down4 - 下行流量4
//           iType = 1;
//           return toString( pStd->m_iDown4 );

//      case 414 : // Up5 - 上行流量5
//           iType = 1;
//           return toString( pStd->m_iUp5 );

//      case 415 : // Down5 - 下行流量5
//           iType = 1;
//           return toString( pStd->m_iDown5 );

//      case 416 : // Up6 - 上行流量6
//           iType = 1;
//           return toString( pStd->m_iUp6 );

//      case 417 : // Down6 - 下行流量6
//           iType = 1;
//           return toString( pStd->m_iDown6 );

//      case 418 : // Up7 - 上行流量7
//           iType = 1;
//           return toString( pStd->m_iUp7 );

//      case 419 : // Down7 - 下行流量7
//           iType = 1;
//           return toString( pStd->m_iDown7 );

//      case 420 : // Up8 - 上行流量8
//           iType = 1;
//           return toString( pStd->m_iUp8 );

//      case 421 : // Down8 - 下行流量8
//           iType = 1;
//           return toString( pStd->m_iDown8 );

//      case 422 : // Up9 - 上行流量9
//           iType = 1;
//           return toString( pStd->m_iUp9 );

//      case 423 : // Down9 - 下行流量9
//           iType = 1;
//           return toString( pStd->m_iDown9 );

//      case 424 : // Up10 - 上行流量10
//           iType = 1;
//           return toString( pStd->m_iUp10 );

//      case 425 : // Down10 - 下行流量10
//           iType = 1;
//           return toString( pStd->m_iDown10 );

     default :
          iType = 0;
          return string( "" );
     }
     return string( "" );
}
// 函数说明：按照Event_Attr获取StdEvent结构数据
// 参数说明：
//         const StdEvent *pStd    输入事件
//        int iEventAttrId        属性ID
// 返回值说明：
//        返回属性数据
string EventToolKits::get_param( const StdEvent *pStd, int iEventAttrId )
{
int iType;
return get_param( pStd, iEventAttrId , iType );
}

string EventToolKits::get_param2(const StdEvent * pStd, int iEventAttrId, int & iType)
{
	void *pAttr;
	int iValueSize;
	return get_param2(pStd,iEventAttrId, iType, &pAttr, iValueSize);
}
string EventToolKits::get_param2(const StdEvent * pStd, int iEventAttrId, int & iType, void ** pEventAttr,int &iValueSize)
{
	switch ( iEventAttrId )
     {
     case 1 : // EventID - 事件标识
          iType = 2;
	  *pEventAttr = (void *)&(pStd->m_lEventID);
	  iValueSize = sizeof(pStd->m_lEventID);
          return toString( pStd->m_lEventID );

     case 2 : // BillingCycleID - 帐期标识
          iType = 1;
	  *pEventAttr =(void *) &(pStd->m_iBillingCycleID);
	  iValueSize = sizeof(pStd->m_iBillingCycleID);
          return toString( pStd->m_iBillingCycleID );

     case 3 : // EventTypeID - 事件类型标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iEventTypeID);
	  iValueSize = sizeof(pStd->m_iEventTypeID);
          return toString( pStd->m_iEventTypeID );

     case 4 : // BillingAreaCode - 计费方区号
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sBillingAreaCode);
	  iValueSize = sizeof(pStd->m_sBillingAreaCode);
          return string( pStd->m_sBillingAreaCode );

     case 5 : // BillingNBR - 计费方号码
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sBillingNBR);
	  iValueSize = sizeof(pStd->m_sBillingNBR);
          return string( pStd->m_sBillingNBR );

     case 6 : // BillingVisitAreaCode - 计费方到访地区号
          iType = 3;
          *pEventAttr = (void *)(pStd->m_sBillingVisitAreaCode);
	  iValueSize = sizeof(pStd->m_sBillingVisitAreaCode);
          return string( pStd->m_sBillingVisitAreaCode );

     case 7 : // CallingAreaCode - 主叫方区号
          iType = 3;
	  *pEventAttr =(void *) (pStd->m_sCallingAreaCode);
	  iValueSize = sizeof(pStd->m_sCallingAreaCode);
          return string( pStd->m_sCallingAreaCode );

     case 8 : // CallingNBR - 主叫方号码
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sCallingAreaCode);
	  iValueSize = sizeof(pStd->m_sCallingAreaCode);
          return string( pStd->m_sCallingNBR );

     case 9 : // OrgCallingNBR - 原始的主叫方号码
          iType = 3;	 
	 *pEventAttr = (void *)(pStd->m_sOrgCallingNBR);
	  iValueSize = sizeof(pStd->m_sOrgCallingNBR);
          return string( pStd->m_sOrgCallingNBR );

     case 10 : // CallingSPTypeID - 主叫运营商类型标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iCallingSPTypeID);
	  iValueSize = sizeof(pStd->m_iCallingSPTypeID);
          return toString( pStd->m_iCallingSPTypeID);

     case 11 : // CallingServiceTypeID - 主叫业务类型标识
          iType = 1;
	 *pEventAttr = (void *)&(pStd->m_iCallingServiceTypeID);
	 iValueSize = sizeof(pStd->m_iCallingServiceTypeID );
          return toString( pStd->m_iCallingServiceTypeID );

     case 13 : // CalledAreaCode - 被叫区号
          iType = 3;
	 *pEventAttr = (void *)(pStd->m_sCalledAreaCode);
	 iValueSize = sizeof(pStd->m_sCalledAreaCode);
          return string( pStd->m_sCalledAreaCode );

     case 14 : // CalledNBR - 被叫号码
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sCalledNBR);
	  iValueSize = sizeof(pStd->m_sCalledNBR);
          return string( pStd->m_sCalledNBR);	  
     case 15 : // OrgCalledNBR - 原始的被叫方号码
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sOrgCalledNBR);
	  iValueSize = sizeof(pStd->m_sOrgCalledNBR);
          return string( pStd->m_sOrgCalledNBR );

     case 16 : // CalledSPTypeID - 被叫运营商类型标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iCalledSPTypeID);
	  iValueSize = sizeof(pStd->m_iCalledSPTypeID);
          return toString( pStd->m_iCalledSPTypeID );

     case 17 : // CalledServiceTypeID - 被叫业务类型标识
          iType = 1;
	 *pEventAttr = (void *)&(pStd->m_iCalledServiceTypeID);
	 iValueSize = sizeof(pStd->m_iCalledServiceTypeID);
          return toString( pStd->m_iCalledServiceTypeID );

     case 19 : // ThirdPartyAreaCode - 第三方号码区号
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sThirdPartyAreaCode);
	  iValueSize = sizeof(pStd->m_sThirdPartyAreaCode);
          return string( pStd->m_sThirdPartyAreaCode );

     case 20 : // ThirdPartyNBR - 第三方号码
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sThirdPartyNBR);
	  iValueSize = sizeof(pStd->m_sThirdPartyNBR);
          return string( pStd->m_sThirdPartyNBR );

     case 21 : // StartDate - 事件起始时间
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sStartDate);
	  iValueSize = sizeof(pStd->m_sStartDate);
          return string( pStd->m_sStartDate );

     case 22 : // EndDate - 事件结束时间
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sEndDate);
	  iValueSize = sizeof(pStd->m_sEndDate);
          return string( pStd->m_sEndDate );

     case 23 : // Duration - 事件时长
          iType = 2;
	  *pEventAttr = (void *)&(pStd->m_lDuration);
	  iValueSize = sizeof(pStd->m_lDuration);
          return toString( pStd->m_lDuration );

     case 24 : // SwitchID - 交换机标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iSwitchID);
	  iValueSize = sizeof(pStd->m_iSwitchID);
          return toString( pStd->m_iSwitchID );

     case 25 : // TrunkIn - 入中继号
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sTrunkIn);
	  iValueSize = sizeof(pStd->m_sTrunkIn);
          return string( pStd->m_sTrunkIn );

     case 26 : // TrunkInTypeID - 入中继类型标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iTrunkInTypeID);
	  iValueSize = sizeof(pStd->m_iTrunkInTypeID);
          return toString( pStd->m_iTrunkInTypeID );

     case 27 : // TrunkOut - 出中继号
          iType = 3;
	 *pEventAttr = (void *)(pStd->m_sTrunkOut);
	 iValueSize = sizeof(pStd->m_sTrunkOut);
          return string( pStd->m_sTrunkOut );

     case 28 : // TrunkOutTypeID - 出中继类型
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iTrunkOutTypeID);
	  iValueSize = sizeof(pStd->m_iTrunkOutTypeID);
          return toString( pStd->m_iTrunkOutTypeID );

     case 29 : // FileID - 文件标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iFileID);
	  iValueSize = sizeof(pStd->m_iFileID);
          return toString( pStd->m_iFileID );

     case 30 : // EventState - 事件状态
          iType = 3;
	 *pEventAttr = (void *)(pStd->m_sEventState);
	 iValueSize = sizeof(pStd->m_sEventState);
          return string( pStd->m_sEventState );

     case 31 : // CreatedDate - 事件到达计费系统时间
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sCreatedDate);
	  iValueSize = sizeof(pStd->m_sCreatedDate);
          return string( pStd->m_sCreatedDate );

     case 32 : // StateDate - 事件确定状态的时间
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sStateDate);
	  iValueSize = sizeof(pStd->m_sStateDate);
          return string( pStd->m_sStateDate );

     case 33 : // CardID - 卡标识
          iType = 3;
	 *pEventAttr = (void *)(pStd->m_sCardID);
	 iValueSize = sizeof(pStd->m_sCardID);
          return string( pStd->m_sCardID );

     case 34 : // BillingTypeID - 计费类型标识
          iType = 1;
	 *pEventAttr = (void *)&(pStd->m_iBillingTypeID);
	 iValueSize = sizeof(pStd->m_iBillingTypeID);
          return toString( pStd->m_iBillingTypeID );

     case 35 : // ParterID - 合作方标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iParterID);
	  iValueSize = sizeof(pStd->m_iParterID);
          return toString( pStd->m_iParterID );

     case 36 : // ServID - 用户标识
          iType = 2;
	 *pEventAttr = (void *)&(pStd->m_iServID);
	 iValueSize = sizeof(pStd->m_iServID);
          return toString( pStd->m_iServID );

     case 37 : // CustID - 客户标识
          iType = 2;
	 *pEventAttr = (void *)&(pStd->m_iCustID);
	 iValueSize = sizeof(pStd->m_iCustID);
          return toString( pStd->m_iCustID );

     case 38 : // ProductID - 产品标识
          iType = 1;
	 *pEventAttr = (void *)&(pStd->m_iProdID);
	 iValueSize = sizeof(pStd->m_iProdID);
          return toString( pStd->m_iProdID );

     case 39 : // ServProductID - 附属产品标识
          iType = 2;
	  *pEventAttr = (void *)&(pStd->m_iServProdID);
	  iValueSize = sizeof(pStd->m_iServProdID);
          return toString( pStd->m_iServProdID );

/*      case 40 : // ProductBundleID - 产品包标识          
	  iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iProdBundleID);
	  iValueSize = sizeof(pStd->m_iProdBundleID);
          return toString( pStd->m_iProdBundleID);
*/          

     case 41 : // Offers[0] - 商品标识1
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_lOffers[0]);
	  iValueSize = sizeof(pStd->m_lOffers[0]);
          return toString( pStd->m_lOffers[0] );

     case 42 : // Offers[1] - 商品标识2
          iType = 1;
	 *pEventAttr = (void *)&(pStd->m_lOffers[1]);
	 iValueSize = sizeof(pStd->m_lOffers[1] );
          return toString( pStd->m_lOffers[1] );

     case 43 : // Offers[2] - 商品标识3
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_lOffers[2]);
	  iValueSize = sizeof(pStd->m_lOffers[2]);
          return toString( pStd->m_lOffers[2] );

     case 44 : // Offers[3] - 商品标识4
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_lOffers[3]);
	  iValueSize = sizeof(pStd->m_lOffers[3]);
          return toString( pStd->m_lOffers[3] );

     case 45 : // Offers[4] - 商品标识5
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_lOffers[4]);
	  iValueSize = sizeof(pStd->m_lOffers[4]);
          return toString( pStd->m_lOffers[4] );

     case 46 : // BillingRecord - 算费过程记录
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sBillingRecord);
	  iValueSize = sizeof(pStd->m_sBillingRecord);
          return string( pStd->m_sBillingRecord );

     case 47 : // ErrorID - 错误号标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iErrorID);
	  iValueSize = sizeof(pStd->m_iErrorID);
          return toString( pStd->m_iErrorID );

     case 48 : // AccountAreaCode - 数据业务帐号所在地区区号
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sAccountAreaCode);
	  iValueSize = sizeof(pStd->m_sAccountAreaCode);
          return string( pStd->m_sAccountAreaCode );

     case 49 : // AccountNBR - 数据业务帐号
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sAccountNBR);
	  iValueSize = sizeof(pStd->m_sAccountNBR);
          return string( pStd->m_sAccountNBR );

     case 50 : // NASIp - 网络接入IP
          iType = 3;
 	 *pEventAttr = (void *)(pStd->m_sNASIp);
	 iValueSize = sizeof(pStd->m_sNASIp);
          return string( pStd->m_sNASIp );

     case 51 : // NASPortID - 网络接入端口标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iNASPortID);
	  iValueSize = sizeof(pStd->m_iNASPortID);
          return toString( pStd->m_iNASPortID );

     case 52 : // RecvBytes - 接收字节数
          iType = 2;
	  *pEventAttr = (void *)&(pStd->m_lRecvAmount);
	  iValueSize = sizeof(pStd->m_lRecvAmount);
          return toString( pStd->m_lRecvAmount );

     case 53 : // SendBytes - 发送字节数
          iType = 2;
	  *pEventAttr = (void *)&(pStd->m_lSendAmount);
	  iValueSize = sizeof(pStd->m_lSendAmount);
          return toString( pStd->m_lSendAmount );

     case 54 : // RecvPackets - 接收包数
	  iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iNegotiatedQoS );
	  iValueSize = sizeof(pStd->m_iNegotiatedQoS );
          return toString( pStd->m_iNegotiatedQoS );
     
/*     case 55 : // SendPackets - 发送包数
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iSendPackets );
	  iValueSize = sizeof(pStd->m_iSendPackets );
          return toString( pStd->m_iSendPackets  );
*/
     
     case 56 : // NegotiatedQoS - 服务质量
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iNegotiatedQoS);
	  iValueSize = sizeof(pStd->m_iNegotiatedQoS);
          return toString( pStd->m_iNegotiatedQoS );

/*     case 57 : // CycleBeginDate - 帐期起始时间
	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sCycleBeginDate);
	  iValueSize = sizeof(pStd->m_sCycleBeginDate);
          return string( pStd->m_sCycleBeginDate);
     
     case 58 : // CycleEndDate - 帐期结束时间
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sCycleEndDate);
	  iValueSize = sizeof(pStd->m_sCycleEndDate);
          return string( pStd->m_sCycleEndDate);
*/          

     case 59 : // ExtUsedNum - 扩展属性字段使用个数
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iExtUsedNum);
	  iValueSize = sizeof(pStd->m_iExtUsedNum);
          return toString( pStd->m_iExtUsedNum );

     case 60 : // BusinessKey - 业务键
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iBusinessKey);
	  iValueSize = sizeof(pStd->m_iBusinessKey);
          return toString( pStd->m_iBusinessKey );

//      case 61 : // SerProdAttrUsedNum -  附属产品属性实际使用的数目
//           iType = 1;
//           return toString( pStd->m_iSerProdAttrUsedNum );

     case 62 : // ChargeInfo[0].m_lOrgCharge - 第1个费用域的原始费用
          iType = 2;
	  *pEventAttr = (void *)&(pStd->m_lOrgCharge[0]);
	  iValueSize = sizeof(pStd->m_lOrgCharge[0] );
          return toString( pStd->m_lOrgCharge[0] );

     case 63 : // StdChargeInfo[0].m_iStdCharge - 第1个费用域的标准费用
          iType = 1;
	 *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[0].m_lStdCharge);
	 iValueSize = sizeof(pStd->m_oStdChargeInfo[0].m_lStdCharge);
          return toString( pStd->m_oStdChargeInfo[0].m_lStdCharge );

     case 64 : // ChargeInfo[0].m_iCharge - 第1个费用域的最终费用
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[0].m_lCharge);
	  iValueSize = sizeof(pStd->m_oChargeInfo[0].m_lCharge);
          return toString( pStd->m_oChargeInfo[0].m_lCharge );

     case 65 : // ChargeInfo[0].m_iAcctItemTypeID - 第1个费用域的帐目类型标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[0].m_iAcctItemTypeID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[0].m_iAcctItemTypeID);
          return toString( pStd->m_oChargeInfo[0].m_iAcctItemTypeID );

     case 66 : // ChargeInfo[0].m_iAcctItemTypeID - 第1个费用域的优惠帐目类型标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[0].m_iAcctItemTypeID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[0].m_iAcctItemTypeID);
          return toString( pStd->m_oChargeInfo[0].m_iAcctItemTypeID );

     case 67 : // ChargeInfo[1].m_lOrgCharge - 第2个费用域的原始费用
          iType = 2;
	  *pEventAttr = (void *)&(pStd->m_lOrgCharge[1]);
	  iValueSize = sizeof(pStd->m_lOrgCharge[1] );
          return toString( pStd->m_lOrgCharge[1] );

     case 68 : // StdChargeInfo[1].m_iStdCharge - 第2个费用域的标准费用
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[1].m_lStdCharge);
	  iValueSize = sizeof(pStd->m_oStdChargeInfo[1].m_lStdCharge);
          return toString( pStd->m_oStdChargeInfo[1].m_lStdCharge );

     case 69 : // ChargeInfo[1].m_iCharge - 第2个费用域的最终费用
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[1].m_lCharge);
	  iValueSize = sizeof(pStd->m_oChargeInfo[1].m_lCharge);
          return toString( pStd->m_oChargeInfo[1].m_lCharge );

     case 70 : // ChargeInfo[1].m_iAcctItemTypeID - 第2个费用域的帐目类型标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[1].m_iAcctItemTypeID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[1].m_iAcctItemTypeID);
          return toString( pStd->m_oChargeInfo[1].m_iAcctItemTypeID );

     case 71 : // ChargeInfo[1].m_iAcctItemTypeID - 第2个费用域的优惠帐目类型标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[1].m_iAcctItemTypeID );
	  iValueSize = sizeof(pStd->m_oChargeInfo[1].m_iAcctItemTypeID);
          return toString( pStd->m_oChargeInfo[1].m_iAcctItemTypeID );

//      case 72 : // ServProdAttr[0].m_iAttrID - 第1个附属产品信息的属性标识
//           iType = 1;
//           return toString( pStd->m_iServProdAttr[0].m_iAttrID );

//      case 73 : // ServProdAttr[0].m_iAttrVal - 第1个附属产品信息的属性取值
//           iType = 3;
//           return string( pStd->m_sServProdAttr[0].m_iAttrVal );

//      case 74 : // ServProdAttr[0].m_iAttrEffDate - 第1个附属产品信息的生效时间
//           iType = 3;
//           return string( pStd->m_sServProdAttr[0].m_iAttrEffDate );

//      case 75 : // ServProdAttr[0].m_iAttrExpDate - 第1个附属产品信息的失效时间
//           iType = 3;
//           return string( pStd->m_sServProdAttr[0].m_iAttrExpDate );

//      case 76 : // ServProdAttr[1].m_iAttrID - 第2个附属产品信息的属性标识
//           iType = 1;
//           return toString( pStd->m_iServProdAttr[1].m_iAttrID );

//      case 77 : // ServProdAttr[1].m_iAttrVal - 第2个附属产品信息的属性取值
//           iType = 3;
//           return string( pStd->m_sServProdAttr[1].m_iAttrVal );

//      case 78 : // ServProdAttr[1].m_iAttrEffDate - 第2个附属产品信息的生效时间
//           iType = 3;
//           return string( pStd->m_sServProdAttr[1].m_iAttrEffDate );

//      case 79 : // ServProdAttr[1].m_iAttrExpDate - 第2个附属产品信息的失效时间
//           iType = 3;
//           return string( pStd->m_sServProdAttr[1].m_iAttrExpDate );

     case 80 : // CallTypeID - 呼叫类型标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iCallTypeID );
	  iValueSize = sizeof(pStd->m_iCallTypeID);
          return toString( pStd->m_iCallTypeID );

     case 81 : // BillingOrgID - 计费方运营组织标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iBillingOrgID );
	  iValueSize = sizeof(pStd->m_iBillingOrgID);
          return toString( pStd->m_iBillingOrgID );

/*      case 82 : // BillFlowID - 计费流程标识
           iType = 1;           
	  *pEventAttr = (void *)&(pStd->m_iBillFlowID );
	  iValueSize = sizeof(pStd->m_iBillFlowID);
          return toString( pStd->m_iBillFlowID );*/

     case 83 : // ChargeInfo[0].m_lBillMeasure - 第1个费用域的计费量
          iType = 2;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[0].m_lBillMeasure );
	  iValueSize = sizeof(pStd->m_oChargeInfo[0].m_lBillMeasure);
          return toString( pStd->m_oChargeInfo[0].m_lBillMeasure );

     case 84 : // ChargeInfo[1].m_lBillMeasure - 第2个费用域的计费量
          iType = 2;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[1].m_lBillMeasure );
	  iValueSize = sizeof(pStd->m_oChargeInfo[1].m_lBillMeasure);
          return toString( pStd->m_oChargeInfo[1].m_lBillMeasure );

     case 201 : // OrgSequence - 事件原始序列号
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sOrgSequence);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sOrgSequence);
          return string( pStd->m_oEventExt.m_sOrgSequence );

     case 202 : // PartialIndicator - 事件部分指示
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_oEventExt.m_iPartialIndicator );
	  iValueSize = sizeof(pStd->m_oEventExt.m_iPartialIndicator);
          return toString( pStd->m_oEventExt.m_iPartialIndicator );

     case 203 : // MiddleSequnce - 中间记录序列号
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_oEventExt.m_iMiddleSequnce );
	  iValueSize = sizeof(pStd->m_oEventExt.m_iMiddleSequnce);
          return toString( pStd->m_oEventExt.m_iMiddleSequnce );

//      case 204 : // RecordIndex - 话单记录原始主索引
//           iType = 1;
//           return toString( pStd->m_oEventExt.m_iRecordIndex );

     case 205 : // HotBillingTag - 热计费标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_oEventExt.m_iHotBillingTag  );
	  iValueSize = sizeof(pStd->m_oEventExt.m_iHotBillingTag);
          return toString( pStd->m_oEventExt.m_iHotBillingTag );

     case 206 : // Causefortermination - 挂断标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_oEventExt.m_iCausefortermination );
	  iValueSize = sizeof(pStd->m_oEventExt.m_iCausefortermination);
          return toString( pStd->m_oEventExt.m_iCausefortermination );

     case 207 : // MSC - 移动交换机标识
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sMSC);
	  iValueSize = sizeof(pStd->m_sMSC);
          return string( pStd->m_sMSC );

     case 208 : // LAC - 小区号标识
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sLAC);
	  iValueSize = sizeof(pStd->m_sLAC);
          return string( pStd->m_sLAC );

     case 209 : // CellID - 基站号标识
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sCellID);
	  iValueSize = sizeof(pStd->m_sCellID);
          return string( pStd->m_sCellID );

     case 210 : // RomingNBR - 动态漫游号标识
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_sRomingNBR);
	  iValueSize = sizeof(pStd->m_sRomingNBR);
          return string( pStd->m_sRomingNBR );

     case 211 : // NoBillFlag - 免费标志标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_iNoBillFlag);
	  iValueSize = sizeof(pStd->m_iNoBillFlag);
          return toString( pStd->m_iNoBillFlag );

     case 212 : // CallingVisitAreaCode - 主叫到访地区号标识
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sCallingVisitAreaCode);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sCallingVisitAreaCode );
          return string( pStd->m_oEventExt.m_sCallingVisitAreaCode );

     case 213 : // CalledVisitAreaCode - 被叫到访地区号标识
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sCalledVisitAreaCode);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sCalledVisitAreaCode);
          return string( pStd->m_oEventExt.m_sCalledVisitAreaCode );

     case 214 : // IMSI - IMSI
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sIMSI );
	  iValueSize = sizeof(pStd->m_oEventExt.m_sIMSI);
          return string( pStd->m_oEventExt.m_sIMSI );

     case 215 : // IMEI - IMEI
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sIMEI );
	  iValueSize = sizeof(pStd->m_oEventExt.m_sIMEI);
          return string( pStd->m_oEventExt.m_sIMEI );

     case 216 : // ExtendID[0] - 扩展属性1标识
          iType = 1;
	 *pEventAttr = (void *)&(pStd->m_aiExtFieldID[0]);
	 iValueSize = sizeof(pStd->m_aiExtFieldID[0]);
          return toString( pStd->m_aiExtFieldID[0]);

     case 217 : // ExtendVaule[0] - 扩展属性1取值
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_asExtValue[0]);
	  iValueSize = sizeof(pStd->m_asExtValue[0]);
          return string( pStd->m_asExtValue[0] );

     case 218 : // ExtendID[1] - 扩展属性2标识
          iType = 1;
	 *pEventAttr = (void *)&(pStd->m_aiExtFieldID[1]);
	 iValueSize = sizeof(pStd->m_aiExtFieldID[1]);
          return toString( pStd->m_aiExtFieldID[1] );

     case 219 : // ExtendValue[1] - 扩展属性2取值
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_asExtValue[1]);
	  iValueSize = sizeof(pStd->m_asExtValue[1]);
          return string( pStd->m_asExtValue[1]);

     case 220 : // ExtendID[2] - 扩展属性3标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_aiExtFieldID[2]);
	  iValueSize = sizeof(pStd->m_aiExtFieldID[2]);
          return toString( pStd->m_aiExtFieldID[2] );

     case 221 : // ExtendValue[2] - 扩展属性3取值
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_asExtValue[2] );
	  iValueSize = sizeof(pStd->m_asExtValue[2]);
          return string( pStd->m_asExtValue[2] );

     case 222 : // ExtendID[3] - 扩展属性4标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_aiExtFieldID[3] );
	  iValueSize = sizeof(pStd->m_aiExtFieldID[3]);
          return toString( pStd->m_aiExtFieldID[3] );

     case 223 : // ExtendValue[3] - 扩展属性4取值
          iType = 3;
	 *pEventAttr = (void *)(pStd->m_asExtValue[3] );
	 iValueSize = sizeof(pStd->m_asExtValue[3]);
          return string( pStd->m_asExtValue[3] );

     case 224 : // ExtendID[4] - 扩展属性5标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_aiExtFieldID[4]);
	  iValueSize = sizeof(pStd->m_aiExtFieldID[4]);
          return toString( pStd->m_aiExtFieldID[4] );

     case 225 : // ExtendValue[4] - 扩展属性5取值
          iType = 3;
	 *pEventAttr = (void *)(pStd->m_asExtValue[4]);
	 iValueSize = sizeof(pStd->m_asExtValue[4]);
          return string( pStd->m_asExtValue[4] );

     case 226 : // ExtendID[5] - 扩展属性6标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_aiExtFieldID[5]);
	  iValueSize = sizeof(pStd->m_aiExtFieldID[5]);
          return toString( pStd->m_aiExtFieldID[5] );

     case 227 : // ExtendValue[5] - 扩展属性6取值
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_asExtValue[5]);
	  iValueSize = sizeof(pStd->m_asExtValue[5]);
          return string( pStd->m_asExtValue[5] );

     case 228 : // ExtendID[6] - 扩展属性7标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_aiExtFieldID[6]);
	  iValueSize = sizeof(pStd->m_aiExtFieldID[6]);
          return toString( pStd->m_aiExtFieldID[6] );

     case 229 : // ExtendValue[6] - 扩展属性7取值
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_asExtValue[6] );
	  iValueSize = sizeof(pStd->m_asExtValue[6]);
          return string( pStd->m_asExtValue[6] );

     case 230 : // ExtendID[7] - 扩展属性8标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_aiExtFieldID[7] );
	  iValueSize = sizeof(pStd->m_aiExtFieldID[7]);
          return toString( pStd->m_aiExtFieldID[7] );

     case 231 : // ExtendValue[7] - 扩展属性8取值
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_asExtValue[7] );
	  iValueSize = sizeof(pStd->m_asExtValue[7]);
          return string( pStd->m_asExtValue[7] );

     case 232 : // ExtendID[8] - 扩展属性9标识
          iType = 1;
 	  *pEventAttr = (void *)&(pStd->m_aiExtFieldID[8]);
	  iValueSize = sizeof(pStd->m_aiExtFieldID[8]);
          return toString( pStd->m_aiExtFieldID[8] );

     case 233 : // ExtendValue[8] - 扩展属性9取值
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_asExtValue[8]);
	  iValueSize = sizeof(pStd->m_asExtValue[8]);
          return string( pStd->m_asExtValue[8] );

     case 234 : // ExtendID[9] - 扩展属性10标识
          iType = 1;
	  *pEventAttr = (void *)&(pStd->m_aiExtFieldID[9]);
	  iValueSize = sizeof(pStd->m_aiExtFieldID[9]);
          return toString( pStd->m_aiExtFieldID[9] );

     case 235 : // ExtendValue[9] - 扩展属性10取值
          iType = 3;
	  *pEventAttr = (void *)(pStd->m_asExtValue[9]);
	  iValueSize = sizeof(pStd->m_asExtValue[9]);
          return string( pStd->m_asExtValue[9] );
//   m_sReservedField 取保留字段的值      
     case 236 :
	  iType =3 ;
	  *pEventAttr = (void *)(pStd->m_sReservedField[0]);
	  iValueSize = sizeof(pStd->m_sReservedField[0]);
	 return string(pStd->m_sReservedField[0]);
	 
     case 237:
    	 iType = 3;
	 *pEventAttr = (void *)(pStd->m_sReservedField[1]);
	 iValueSize = sizeof(pStd->m_sReservedField[1]);
    	 return string(pStd->m_sReservedField[1]);
    case 238:
         iType = 3;
	 *pEventAttr = (void *)(pStd->m_sReservedField[2]);
	 iValueSize = sizeof(pStd->m_sReservedField[2]);
         return string(pStd->m_sReservedField[2]);
     case 239:
         iType = 3;
	 *pEventAttr = (void *)(pStd->m_sReservedField[3]);
	 iValueSize = sizeof(pStd->m_sReservedField[3]);
         return string(pStd->m_sReservedField[3]);
     case 240:
         iType = 3;
	*pEventAttr = (void *)(pStd->m_sReservedField[4]);
	iValueSize = sizeof(pStd->m_sReservedField[4]);
         return string(pStd->m_sReservedField[4]);
     case 242:
		iType = 1;
	  	*pEventAttr = (void *)&(pStd->m_iEventSourceType);
		iValueSize = sizeof(pStd->m_iEventSourceType);
          	return toString( pStd->m_iEventSourceType);
     case 243:
 	  iType = 2;
	  *pEventAttr = (void *)&(pStd->m_lSumAmount);
	  iValueSize = sizeof(pStd->m_lSumAmount);
	  return toString(pStd->m_lSumAmount); 
	  
	case 244:		
		iType = 1;
	  	*pEventAttr = (void *)&(pStd->m_iFlowID);
		iValueSize = sizeof(pStd->m_iFlowID);
          	return toString( pStd->m_iFlowID);
	case 245:
		iType = 3;
		*pEventAttr = (void *)(pStd->m_sGateWay);
		iValueSize = sizeof(pStd->m_sGateWay);
	         return string(pStd->m_sGateWay);
	case 246:
		iType = 1;
	  	*pEventAttr = (void *)&(pStd->m_iCallingPA);
		iValueSize = sizeof(pStd->m_iCallingPA);
          	return toString( pStd->m_iCallingPA);
	case 247:
		iType = 1;
	  	*pEventAttr = (void *)&(pStd->m_iCalledPA);
		iValueSize = sizeof(pStd->m_iCalledPA);
          	return toString( pStd->m_iCalledPA);
	case 248:
		iType = 3;
		*pEventAttr = (void *)(pStd->m_sCallingRP);
		iValueSize = sizeof(pStd->m_sCallingRP);
	         return string(pStd->m_sCallingRP);
	case 249:
		iType = 3;
		*pEventAttr = (void *)(pStd->m_sCalledRP);
		iValueSize = sizeof(pStd->m_sCalledRP);
	         return string(pStd->m_sCalledRP);	
	case 250:
		iType = 1;
	  	*pEventAttr = (void *)&(pStd->m_iCallingRPArea);
		iValueSize = sizeof(pStd->m_iCallingRPArea);
          	return toString( pStd->m_iCallingRPArea);
	case 251:
		iType = 1;
	  	*pEventAttr = (void *)&(pStd->m_iCalledRPArea);
		iValueSize = sizeof(pStd->m_iCalledRPArea);
          	return toString( pStd->m_iCalledRPArea);
     case 252:
	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sSessionID);
	  iValueSize = sizeof(pStd->m_sSessionID);
	  return string( pStd->m_sSessionID);	     
     case 253:
     	  iType  =3;
	  *pEventAttr = (void *)(pStd->m_sOrgThirdPartyNBR);
	  iValueSize = sizeof(pStd->m_sOrgThirdPartyNBR);
     	  return string(pStd->m_sOrgThirdPartyNBR);
     case 254:
	  iType  =3;
	  *pEventAttr = (void *)(pStd->m_sOrgThirdPartyPhysicalNBR);
	  iValueSize = sizeof(pStd->m_sOrgThirdPartyPhysicalNBR);
     	  return string(pStd->m_sOrgThirdPartyPhysicalNBR);					
     case 255:
	  iType  =1;
	  *pEventAttr = (void *)&(pStd->m_iEventSeq);
	  iValueSize = sizeof(pStd->m_iEventSeq);
     	  return toString(pStd->m_iEventSeq);
     case 256:
     	  iType=3;
	  *pEventAttr = (void *)(pStd->m_sDomainName);
	  iValueSize = sizeof(pStd->m_sDomainName);
     	  return string(pStd->m_sDomainName);
     case 257:
	  iType  =1;
	  *pEventAttr = (void *)&(pStd->m_iGroupID);
	  iValueSize = sizeof(pStd->m_iGroupID);
     	  return toString(pStd->m_iGroupID);			    	
     case 258:
	  iType  =1;
	  *pEventAttr = (void *)&(pStd->m_iRoamFlag);
	  iValueSize = sizeof(pStd->m_iRoamFlag);
     	  return toString(pStd->m_iRoamFlag);			    		
     case 259:
	  iType=3;
	  *pEventAttr = (void *)(pStd->m_sBindAttr);
	  iValueSize = sizeof(pStd->m_sBindAttr);
     	  return string(pStd->m_sBindAttr);					
     case 260:
        iType =1;
	*pEventAttr = (void *)&(pStd->m_iServiceType);
	iValueSize = sizeof(pStd->m_iServiceType);
        return toString(pStd->m_iServiceType);
//add by yuanp 20081029 for sms
     case 261:
     	 iType =1;
	*pEventAttr = (void *)&(pStd->m_iUserType);
	iValueSize = sizeof(pStd->m_iUserType);
     	return toString(pStd->m_iUserType);
     case 262:
     	 iType=3;
	*pEventAttr = (void *)(pStd->m_sAreaNo);
	iValueSize = sizeof(pStd->m_sAreaNo);
     	return string(pStd->m_sAreaNo);
     case 263:
	 iType =2;
	*pEventAttr = (void *)&(pStd->m_lSepRecID);
	iValueSize = sizeof(pStd->m_lSepRecID);
     	return toString(pStd->m_lSepRecID);		
     case 264:
	  iType =1;
	*pEventAttr = (void *)&(pStd->m_iSepRecSeq);
	iValueSize = sizeof(pStd->m_iSepRecSeq);
     	return toString(pStd->m_iSepRecSeq);				
    case 265:
	 iType =2;
	*pEventAttr = (void *)&(pStd->m_lResourceID);
	iValueSize = sizeof(pStd->m_lResourceID);
     	return toString(pStd->m_lResourceID);				
    case 266:
	  iType =1;
	*pEventAttr = (void *)&(pStd->m_iQueryFlag);
	iValueSize = sizeof(pStd->m_iQueryFlag);
     	return toString(pStd->m_iQueryFlag);				

//      case 301 : // CR_ServiceCode[15] - 彩铃服务编码
//           iType = 3;
//           return string( pStd->m_sCR_ServiceCode[15] );
//返回stdevent.m_oEventExt.m_lEventValue的值
    case 280:
     	  iType=2;
	  *pEventAttr = (void *)&(pStd->m_oEventExt.m_lExtendValue[0]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_lExtendValue[0]);
     	  return toString(pStd->m_oEventExt.m_lExtendValue[0]);
     case 281:
     	  iType=2;
	  *pEventAttr = (void *)&(pStd->m_oEventExt.m_lExtendValue[1]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_lExtendValue[1]);
     	  return toString(pStd->m_oEventExt.m_lExtendValue[1]);
     case 282:
     	  iType=2;
	  *pEventAttr = (void *)&(pStd->m_oEventExt.m_lExtendValue[2]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_lExtendValue[2]);
     	  return toString(pStd->m_oEventExt.m_lExtendValue[2]);
     case 283:
     	  iType=2;
	  *pEventAttr = (void *)&(pStd->m_oEventExt.m_lExtendValue[3]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_lExtendValue[3]);
     	  return toString(pStd->m_oEventExt.m_lExtendValue[3]);
     case 284:
     	  iType=2;
	  *pEventAttr = (void *)&(pStd->m_oEventExt.m_lExtendValue[4]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_lExtendValue[4]);
     	  return toString(pStd->m_oEventExt.m_lExtendValue[4]);
     case 285:
     	  iType=2;
	  *pEventAttr = (void *)&(pStd->m_oEventExt.m_lExtendValue[5]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_lExtendValue[5]);
     	  return toString(pStd->m_oEventExt.m_lExtendValue[5]);
     case 286:
     	  iType=2;
 	 *pEventAttr = (void *)&(pStd->m_oEventExt.m_lExtendValue[6]);
	 iValueSize = sizeof(pStd->m_oEventExt.m_lExtendValue[6]);
     	  return toString(pStd->m_oEventExt.m_lExtendValue[6]);
     case 287:
     	  iType=2;
	 *pEventAttr = (void *)&(pStd->m_oEventExt.m_lExtendValue[7]);
	 iValueSize = sizeof(pStd->m_oEventExt.m_lExtendValue[7]);
     	  return toString(pStd->m_oEventExt.m_lExtendValue[7]);
     case 288:
     	  iType=2;
	  *pEventAttr = (void *)&(pStd->m_oEventExt.m_lExtendValue[8]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_lExtendValue[8]);
     	  return toString(pStd->m_oEventExt.m_lExtendValue[8]);
     case 289:
     	  iType=2;
	  *pEventAttr = (void *)&(pStd->m_oEventExt.m_lExtendValue[9]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_lExtendValue[9]);
     	  return toString(pStd->m_oEventExt.m_lExtendValue[9]);		  
//返回stdevent.m_oEventExt.m_sEventValue的值     	  
     case 290:
     	  iType=3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sExtendValue[0]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sExtendValue[0]);
     	  return string(pStd->m_oEventExt.m_sExtendValue[0]);
     case 291:
     	  iType=3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sExtendValue[1]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sExtendValue[1]);
     	  return string(pStd->m_oEventExt.m_sExtendValue[1]);	  
     case 292:
     	  iType=3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sExtendValue[2]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sExtendValue[2]);
     	  return string(pStd->m_oEventExt.m_sExtendValue[2]);
     case 293:
     	  iType=3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sExtendValue[3]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sExtendValue[3]);
     	  return string(pStd->m_oEventExt.m_sExtendValue[3]);
     case 294:
     	  iType=3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sExtendValue[4]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sExtendValue[4]);
     	  return string(pStd->m_oEventExt.m_sExtendValue[4]);
     case 295:
     	  iType=3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sExtendValue[5]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sExtendValue[5]);
     	  return string(pStd->m_oEventExt.m_sExtendValue[5]);
     case 296:
     	  iType=3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sExtendValue[6]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sExtendValue[6]);
     	  return string(pStd->m_oEventExt.m_sExtendValue[6]);
     case 297:
     	  iType=3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sExtendValue[7]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sExtendValue[7]);
     	  return string(pStd->m_oEventExt.m_sExtendValue[7]);
     case 298:
     	  iType=3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sExtendValue[8]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sExtendValue[8]);
     	  return string(pStd->m_oEventExt.m_sExtendValue[8]);
     case 299:
     	  iType=3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sExtendValue[9]);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sExtendValue[9]);
     	  return string(pStd->m_oEventExt.m_sExtendValue[9]);
     case 426:
	  iType=3;
	  *pEventAttr = (void *)(pStd->m_sSPFLAG);
	  iValueSize = sizeof(pStd->m_sSPFLAG);
     	  return string(pStd->m_sSPFLAG);				
     case 427:
	  iType =1;
	 *pEventAttr = (void *)&(pStd->m_iSuppleServiceCode);
	 iValueSize = sizeof(pStd->m_iSuppleServiceCode);
     	 return toString(pStd->m_iSuppleServiceCode);
     case 428:
	  iType =1;
	 *pEventAttr = (void *)&(pStd->m_iCUGCode);
	 iValueSize = sizeof(pStd->m_iCUGCode);
     	 return toString(pStd->m_iCUGCode);
     case 435:
     	   iType = 3;
	  *pEventAttr = (void *)(pStd->m_oEventExt.m_sAccessPointName);
	  iValueSize = sizeof(pStd->m_oEventExt.m_sAccessPointName);
     	   return string(pStd->m_oEventExt.m_sAccessPointName);
    case 911:
	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sCalledCellID);
	  iValueSize = sizeof(pStd->m_sCalledCellID);
     	   return string(pStd->m_sCalledCellID);
    case 912:
	   iType = 3;
	  *pEventAttr = (void *)(pStd->m_sCalledLAC);
	  iValueSize = sizeof(pStd->m_sCalledLAC);
     	   return string(pStd->m_sCalledLAC);
    case 500://计费号码
    	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sBillingNBR);
	  iValueSize = sizeof(pStd->m_sBillingNBR);
     	   return string(pStd->m_sBillingNBR);
    case 501://计费号码到访地
    	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sBillingVisitAreaCode);
	  iValueSize = sizeof(pStd->m_sBillingVisitAreaCode);
     	   return string(pStd->m_sBillingVisitAreaCode);
    case 502://主叫号码
    	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sCallingNBR);
	  iValueSize = sizeof(pStd->m_sCallingNBR);
     	   return string(pStd->m_sCallingNBR);
    case 503://主叫方物理号码
    	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sCallingPhysicalNBR);
	  iValueSize = sizeof(pStd->m_sCallingPhysicalNBR);
     	   return string(pStd->m_sCallingPhysicalNBR);    
    case 504://主叫运营商类型标识
         iType =1;
	 *pEventAttr = (void *)&(pStd->m_iCallingSPTypeID);
	 iValueSize = sizeof(pStd->m_iCallingSPTypeID);
     	 return toString(pStd->m_iCallingSPTypeID);
    case 505://主叫业务类型标识
         iType =1;
	 *pEventAttr = (void *)&(pStd->m_iCallingServiceTypeID);
	 iValueSize = sizeof(pStd->m_iCallingServiceTypeID);
     	 return toString(pStd->m_iCallingServiceTypeID);    
    case 506://被叫号码
    	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sCalledNBR);
	  iValueSize = sizeof(pStd->m_sCalledNBR);
     	   return string(pStd->m_sCalledNBR);
    case 507://被叫物理号码
    	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sCalledPhysicalNBR);
	  iValueSize = sizeof(pStd->m_sCalledPhysicalNBR);
     	   return string(pStd->m_sCalledPhysicalNBR);    
    case 508://被叫运营商类型标识
          iType =1;
	 *pEventAttr = (void *)&(pStd->m_iCalledSPTypeID);
	 iValueSize = sizeof(pStd->m_iCalledSPTypeID);
     	 return toString(pStd->m_iCalledSPTypeID);
    case 509://被叫业务类型标识
          iType =1;
	 *pEventAttr = (void *)&(pStd->m_iCalledServiceTypeID);
	 iValueSize = sizeof(pStd->m_iCalledServiceTypeID);
     	 return toString(pStd->m_iCalledServiceTypeID);    
     case 510://第三方号码区号
     	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sThirdPartyAreaCode);
	  iValueSize = sizeof(pStd->m_sThirdPartyAreaCode);
     	   return string(pStd->m_sThirdPartyAreaCode);
     case 511://入中继类型标识
	 iType =1;
	 *pEventAttr = (void *)&(pStd->m_iTrunkInTypeID);
	 iValueSize = sizeof(pStd->m_iTrunkInTypeID);
     	 return toString(pStd->m_iTrunkInTypeID);
     case 512://出中继类型
	 iType =1;
	 *pEventAttr = (void *)&(pStd->m_iTrunkOutTypeID);
	 iValueSize = sizeof(pStd->m_iTrunkOutTypeID);
     	 return toString(pStd->m_iTrunkOutTypeID);
     case 513://文件标识
	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_iFileID );
	  iValueSize = sizeof(pStd->m_iFileID);
     	  return toString(pStd->m_iFileID);
     case 514://事件状态
     	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sEventState);
	  iValueSize = sizeof(pStd->m_sEventState);
     	   return string(pStd->m_sEventState);
     case 515://事件到达计费系统时间
     	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sCreatedDate);
	  iValueSize = sizeof(pStd->m_sCreatedDate);
     	   return string(pStd->m_sCreatedDate);
     case 516://事件确定状态的时间
     	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sStateDate);
	  iValueSize = sizeof(pStd->m_sStateDate);
     	   return string(pStd->m_sStateDate);
     case 517://算费过程记载
     	  iType = 3;
	  *pEventAttr = (void *)(pStd->m_sBillingRecord);
	  iValueSize = sizeof(pStd->m_sBillingRecord);
     	   return string(pStd->m_sBillingRecord);     
     case 518://费用0
	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[0].m_lCharge);
	  iValueSize = sizeof(pStd->m_oChargeInfo[0].m_lCharge);
     	  return toString(pStd->m_oChargeInfo[0].m_lCharge);
     case 519://帐目类型0
	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[0].m_iAcctItemTypeID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[0].m_iAcctItemTypeID);
     	  return toString(pStd->m_oChargeInfo[0].m_iAcctItemTypeID);
     case 520://度量0
	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[0].m_lBillMeasure);
	  iValueSize = sizeof(pStd->m_oChargeInfo[0].m_lBillMeasure);
     	  return toString(pStd->m_oChargeInfo[0].m_lBillMeasure);
     case 521://度量类型0
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[0].m_iMeasureType);
	  iValueSize = sizeof(pStd->m_oChargeInfo[0].m_iMeasureType);
     	  return toString(pStd->m_oChargeInfo[0].m_iMeasureType);
     case 522://帐号0
     	  iType =2;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[0].m_lAcctID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[0].m_lAcctID);
     	  return toString(pStd->m_oChargeInfo[0].m_lAcctID);     
     case 523://商品实例0
     	  iType =2;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[0].m_lOfferInstID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[0].m_lOfferInstID);
     	  return toString(pStd->m_oChargeInfo[0].m_lOfferInstID);     
     case 524://帐目来源0
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[0].m_iItemSourceID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[0].m_iItemSourceID);
     	  return toString(pStd->m_oChargeInfo[0].m_iItemSourceID);
		  
     case 525://费用1
	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[1].m_lCharge);
	  iValueSize = sizeof(pStd->m_oChargeInfo[0].m_lCharge);
     	  return toString(pStd->m_oChargeInfo[1].m_lCharge);
     case 526://帐目类型1
	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[1].m_iAcctItemTypeID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[0].m_iAcctItemTypeID);
     	  return toString(pStd->m_oChargeInfo[1].m_iAcctItemTypeID);
     case 527://度量1
	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[1].m_lBillMeasure);
	  iValueSize = sizeof(pStd->m_oChargeInfo[1].m_lBillMeasure);
     	  return toString(pStd->m_oChargeInfo[1].m_lBillMeasure);
     case 528://度量类型1
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[1].m_iMeasureType);
	  iValueSize = sizeof(pStd->m_oChargeInfo[1].m_iMeasureType);
     	  return toString(pStd->m_oChargeInfo[1].m_iMeasureType);
     case 529://帐号1
     	  iType =2;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[1].m_lAcctID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[1].m_lAcctID);
     	  return toString(pStd->m_oChargeInfo[1].m_lAcctID);     
     case 530://商品实例1
     	  iType =2;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[1].m_lOfferInstID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[1].m_lOfferInstID);
     	  return toString(pStd->m_oChargeInfo[1].m_lOfferInstID);     
     case 531://帐目来源1
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[1].m_iItemSourceID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[1].m_iItemSourceID);
     	  return toString(pStd->m_oChargeInfo[1].m_iItemSourceID);

     case 532://费用2
	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[2].m_lCharge);
	  iValueSize = sizeof(pStd->m_oChargeInfo[2].m_lCharge);
     	  return toString(pStd->m_oChargeInfo[2].m_lCharge);
     case 533://帐目类型2
	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[2].m_iAcctItemTypeID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[2].m_iAcctItemTypeID);
     	  return toString(pStd->m_oChargeInfo[2].m_iAcctItemTypeID);
     case 534://度量2
	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[2].m_lBillMeasure);
	  iValueSize = sizeof(pStd->m_oChargeInfo[2].m_lBillMeasure);
     	  return toString(pStd->m_oChargeInfo[2].m_lBillMeasure);
     case 535://度量类型2
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[2].m_iMeasureType);
	  iValueSize = sizeof(pStd->m_oChargeInfo[2].m_iMeasureType);
     	  return toString(pStd->m_oChargeInfo[2].m_iMeasureType);
     case 536://帐号2
     	  iType =2;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[2].m_lAcctID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[2].m_lAcctID);
     	  return toString(pStd->m_oChargeInfo[2].m_lAcctID);     
     case 537://商品实例2
     	  iType =2;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[2].m_lOfferInstID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[2].m_lOfferInstID);
     	  return toString(pStd->m_oChargeInfo[2].m_lOfferInstID);     
     case 538://帐目来源2
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[2].m_iItemSourceID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[2].m_iItemSourceID);
     	  return toString(pStd->m_oChargeInfo[2].m_iItemSourceID);

     case 539://费用3
	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[3].m_lCharge);
	  iValueSize = sizeof(pStd->m_oChargeInfo[3].m_lCharge);
     	  return toString(pStd->m_oChargeInfo[3].m_lCharge);
     case 540://帐目类型3
	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[3].m_iAcctItemTypeID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[3].m_iAcctItemTypeID);
     	  return toString(pStd->m_oChargeInfo[3].m_iAcctItemTypeID);
     case 541://度量3
	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[3].m_lBillMeasure);
	  iValueSize = sizeof(pStd->m_oChargeInfo[3].m_lBillMeasure);
     	  return toString(pStd->m_oChargeInfo[3].m_lBillMeasure);
     case 542://度量类型3
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[3].m_iMeasureType);
	  iValueSize = sizeof(pStd->m_oChargeInfo[3].m_iMeasureType);
     	  return toString(pStd->m_oChargeInfo[3].m_iMeasureType);
     case 543://帐号3
     	  iType =2;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[3].m_lAcctID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[3].m_lAcctID);
     	  return toString(pStd->m_oChargeInfo[3].m_lAcctID);     
     case 544://商品实例3
     	  iType =2;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[3].m_lOfferInstID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[3].m_lOfferInstID);
     	  return toString(pStd->m_oChargeInfo[3].m_lOfferInstID);
     case 545://帐目来源3
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oChargeInfo[3].m_iItemSourceID);
	  iValueSize = sizeof(pStd->m_oChargeInfo[3].m_iItemSourceID);
     	  return toString(pStd->m_oChargeInfo[3].m_iItemSourceID);

     case 546://std费用0
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[0].m_lStdCharge);
	  iValueSize = sizeof(pStd->m_oStdChargeInfo[0].m_lStdCharge);
     	  return toString(pStd->m_oStdChargeInfo[0].m_lStdCharge);
     case 547://std帐目类型0
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[0].m_iStdAcctItemTypeID);
	  iValueSize = sizeof(pStd->m_oStdChargeInfo[0].m_iStdAcctItemTypeID);
     	  return toString(pStd->m_oStdChargeInfo[0].m_iStdAcctItemTypeID);
     case 548://std费用1
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[1].m_lStdCharge);
	  iValueSize = sizeof(pStd->m_oStdChargeInfo[1].m_lStdCharge);
     	  return toString(pStd->m_oStdChargeInfo[1].m_lStdCharge);
     case 549://std帐目类型1
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[1].m_iStdAcctItemTypeID);
	  iValueSize = sizeof(pStd->m_oStdChargeInfo[1].m_iStdAcctItemTypeID);
     	  return toString(pStd->m_oStdChargeInfo[1].m_iStdAcctItemTypeID);
     case 550://std费用2
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[2].m_lStdCharge);
	  iValueSize = sizeof(pStd->m_oStdChargeInfo[2].m_lStdCharge);
     	  return toString(pStd->m_oStdChargeInfo[2].m_lStdCharge);
     case 551://std帐目类型2
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[2].m_iStdAcctItemTypeID);
	  iValueSize = sizeof(pStd->m_oStdChargeInfo[2].m_iStdAcctItemTypeID);
     	  return toString(pStd->m_oStdChargeInfo[2].m_iStdAcctItemTypeID);
     case 552://std费用3
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[3].m_lStdCharge);
	  iValueSize = sizeof(pStd->m_oStdChargeInfo[3].m_lStdCharge);
     	  return toString(pStd->m_oStdChargeInfo[3].m_lStdCharge);
     case 553://std帐目类型3
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[3].m_iStdAcctItemTypeID);
	  iValueSize = sizeof(pStd->m_oStdChargeInfo[3].m_iStdAcctItemTypeID);
     	  return toString(pStd->m_oStdChargeInfo[3].m_iStdAcctItemTypeID);
     case 554://定价组合标识
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_iPricingCombineID);
	  iValueSize = sizeof(pStd->m_iPricingCombineID);
     	  return toString(pStd->m_iPricingCombineID);
     case 555://SP标识
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_iSPID);
	  iValueSize = sizeof(pStd->m_iSPID);
     	  return toString(pStd->m_iSPID);     
     case 556://市话次数
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_iTimes);
	  iValueSize = sizeof(pStd->m_iTimes);
     	  return toString(pStd->m_iTimes);
     case 557://网络标识
     	  iType =1;
	  *pEventAttr = (void *)&(pStd->m_iNetworkID);
	  iValueSize = sizeof(pStd->m_iNetworkID);
     	  return toString(pStd->m_iNetworkID);
	case 558://进程号
		iType =1;
	  	*pEventAttr = (void *)&(pStd->m_iProcessID);
		iValueSize = sizeof(pStd->m_iProcessID);
     	  	return toString(pStd->m_iProcessID);
	//扩展
	case 559://漫游类型
		iType =1;
	  	*pEventAttr = (void *)&(pStd->m_oEventExt.m_iRoamTypeID);
		iValueSize = sizeof(pStd->m_oEventExt.m_iRoamTypeID);
     	  	return toString(pStd->m_oEventExt.m_iRoamTypeID);
	case 560://std漫游类型
		iType =1;
	  	*pEventAttr = (void *)&(pStd->m_oEventExt.m_iStdRoamTypeID);
		iValueSize = sizeof(pStd->m_oEventExt.m_iStdRoamTypeID);
     	  	return toString(pStd->m_oEventExt.m_iStdRoamTypeID);
	case 561://std长途类型
		iType =1;
	  	*pEventAttr = (void *)&(pStd->m_oEventExt.m_iStdLongTypeID);
		iValueSize = sizeof(pStd->m_oEventExt.m_iStdLongTypeID);
     	  	return toString(pStd->m_oEventExt.m_iStdLongTypeID);
	case 562://长途类型
		iType =1;
	  	*pEventAttr = (void *)&(pStd->m_oEventExt.m_iLongTypeID);
		iValueSize = sizeof(pStd->m_oEventExt.m_iLongTypeID);
     	  	return toString(pStd->m_oEventExt.m_iLongTypeID);
	case 563://文件名
		iType =3;
	  	*pEventAttr = (void *)(pStd->m_oEventExt.m_sFileName);
		iValueSize = sizeof(pStd->m_oEventExt.m_sFileName);
     	  	return string(pStd->m_oEventExt.m_sFileName);
	case 564://源事件类型
		iType =1;
	  	*pEventAttr = (void *)&(pStd->m_oEventExt.m_iSourceEventType);
		iValueSize = sizeof(pStd->m_oEventExt.m_iSourceEventType);
     	  	return toString(pStd->m_oEventExt.m_iSourceEventType);
	case 565://网络承载类型
		iType =1;
	  	*pEventAttr = (void *)&(pStd->m_oEventExt.m_iCarrierTypeID);
		iValueSize = sizeof(pStd->m_oEventExt.m_iCarrierTypeID);
     	  	return toString(pStd->m_oEventExt.m_iCarrierTypeID);
	case 567://主叫长途组类型标识
		iType =1;
	  	*pEventAttr = (void *)&(pStd->m_oEventExt.m_iCallingLongGroupTypeID);
		iValueSize = sizeof(pStd->m_oEventExt.m_iCallingLongGroupTypeID);
     	  	return toString(pStd->m_oEventExt.m_iCallingLongGroupTypeID);
	case 568://被叫长途组类型标识
		iType =1;
	  	*pEventAttr = (void *)&(pStd->m_oEventExt.m_iCalledLongGroupTypeID);
		iValueSize = sizeof(pStd->m_oEventExt.m_iCalledLongGroupTypeID);
     	  	return toString(pStd->m_oEventExt.m_iCalledLongGroupTypeID);	
	case 569://主叫运维机构ID
		iType =1;
	  	*pEventAttr = (void *)&(pStd->m_oEventExt.m_iCallingOrgID );
		iValueSize = sizeof(pStd->m_oEventExt.m_iCallingOrgID);
     	  	return toString(pStd->m_oEventExt.m_iCallingOrgID );
	case 570://交换机长途类型
		iType =1;
	  	*pEventAttr = (void *)&(pStd->m_oEventExt.m_iSwitchLongType );
		iValueSize = sizeof(pStd->m_oEventExt.m_iSwitchLongType);
     	  	return toString(pStd->m_oEventExt.m_iSwitchLongType );
	case 571://交换机区号
		iType =3;
	  	*pEventAttr = (void *)(pStd->m_oEventExt.m_sSwitchAreaCode);
		iValueSize = sizeof(pStd->m_oEventExt.m_sSwitchAreaCode);
     	  	return string(pStd->m_oEventExt.m_sSwitchAreaCode);
	case 572://第三方号码到访地区号
		iType =3;
	  	*pEventAttr = (void *)(pStd->m_oEventExt.m_sThirdPartyVisitAreaCode);
		iValueSize = sizeof(pStd->m_oEventExt.m_sThirdPartyVisitAreaCode);
     	  	return string(pStd->m_oEventExt.m_sThirdPartyVisitAreaCode);	
			
	if( MAX_CHARGE_NUM > 4 )
	{

		     case 580://费用4
			  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[4].m_lCharge);
			  iValueSize = sizeof(pStd->m_oChargeInfo[4].m_lCharge);
		     	  return toString(pStd->m_oChargeInfo[4].m_lCharge);
		     case 581://帐目类型4
			  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[4].m_iAcctItemTypeID);
			  iValueSize = sizeof(pStd->m_oChargeInfo[4].m_iAcctItemTypeID);
		     	  return toString(pStd->m_oChargeInfo[4].m_iAcctItemTypeID);
		     case 582://度量4
			  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[4].m_lBillMeasure);
			  iValueSize = sizeof(pStd->m_oChargeInfo[4].m_lBillMeasure);
		     	  return toString(pStd->m_oChargeInfo[4].m_lBillMeasure);
		     case 583://度量类型4
		     	  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[4].m_iMeasureType);
			  iValueSize = sizeof(pStd->m_oChargeInfo[4].m_iMeasureType);
		     	  return toString(pStd->m_oChargeInfo[4].m_iMeasureType);
		     case 584://帐号4
		     	  iType =2;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[4].m_lAcctID);
			  iValueSize = sizeof(pStd->m_oChargeInfo[4].m_lAcctID);
		     	  return toString(pStd->m_oChargeInfo[4].m_lAcctID);     
		     case 585://商品实例4
		     	  iType =2;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[4].m_lOfferInstID);
			  iValueSize = sizeof(pStd->m_oChargeInfo[4].m_lOfferInstID);
		     	  return toString(pStd->m_oChargeInfo[4].m_lOfferInstID);
		     case 586://帐目来源4
		     	  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[4].m_iItemSourceID);
			  iValueSize = sizeof(pStd->m_oChargeInfo[4].m_iItemSourceID);
		     	  return toString(pStd->m_oChargeInfo[4].m_iItemSourceID);
		     case 587://std费用4
		     	  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[4].m_lStdCharge);
			  iValueSize = sizeof(pStd->m_oStdChargeInfo[4].m_lStdCharge);
		     	  return toString(pStd->m_oStdChargeInfo[4].m_lStdCharge);
		     case 588://std帐目类型4
		     	  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[4].m_iStdAcctItemTypeID);
			  iValueSize = sizeof(pStd->m_oStdChargeInfo[4].m_iStdAcctItemTypeID);
		     	  return toString(pStd->m_oStdChargeInfo[4].m_iStdAcctItemTypeID);
	}

	if( MAX_CHARGE_NUM > 5 )
	{
		     case 589://费用5
			  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[5].m_lCharge);
			  iValueSize = sizeof(pStd->m_oChargeInfo[5].m_lCharge);
		     	  return toString(pStd->m_oChargeInfo[5].m_lCharge);
		     case 590://帐目类型5
			  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[5].m_iAcctItemTypeID);
			  iValueSize = sizeof(pStd->m_oChargeInfo[5].m_iAcctItemTypeID);
		     	  return toString(pStd->m_oChargeInfo[5].m_iAcctItemTypeID);
		     case 591://度量5
			  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[5].m_lBillMeasure);
			  iValueSize = sizeof(pStd->m_oChargeInfo[5].m_lBillMeasure);
		     	  return toString(pStd->m_oChargeInfo[5].m_lBillMeasure);
		     case 592://度量类型5
		     	  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[5].m_iMeasureType);
			  iValueSize = sizeof(pStd->m_oChargeInfo[5].m_iMeasureType);
		     	  return toString(pStd->m_oChargeInfo[5].m_iMeasureType);
		     case 593://帐号5
		     	  iType =2;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[5].m_lAcctID);
			  iValueSize = sizeof(pStd->m_oChargeInfo[5].m_lAcctID);
		     	  return toString(pStd->m_oChargeInfo[5].m_lAcctID);     
		     case 594://商品实例5
		     	  iType =2;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[5].m_lOfferInstID);
			  iValueSize = sizeof(pStd->m_oChargeInfo[5].m_lOfferInstID);
		     	  return toString(pStd->m_oChargeInfo[5].m_lOfferInstID);
		     case 595://帐目来源5
		     	  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oChargeInfo[5].m_iItemSourceID);
			  iValueSize = sizeof(pStd->m_oChargeInfo[5].m_iItemSourceID);
		     	  return toString(pStd->m_oChargeInfo[5].m_iItemSourceID);
		     case 596://std费用5
		     	  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[5].m_lStdCharge);
			  iValueSize = sizeof(pStd->m_oStdChargeInfo[5].m_lStdCharge);
		     	  return toString(pStd->m_oStdChargeInfo[5].m_lStdCharge);
		     case 597://std帐目类型5
		     	  iType =1;
			  *pEventAttr = (void *)&(pStd->m_oStdChargeInfo[5].m_iStdAcctItemTypeID);
			  iValueSize = sizeof(pStd->m_oStdChargeInfo[5].m_iStdAcctItemTypeID);
		     	  return toString(pStd->m_oStdChargeInfo[5].m_iStdAcctItemTypeID);
	}
     default :
          iType = 0;
          return string( "" );
    }
}

// 函数说明：设置StdEvent数据结构
// 参数说明：
//         StdEvent *pStd        待设置的事件
//        int iEventAttrId    属性ID
//        string strAttr        属性值
void EventToolKits::set_param( StdEvent *pStd, int iEventAttrId, string strAttr )
{
     switch ( iEventAttrId )
     {

     case 1 : // EventID - 事件标识
          pStd->m_lEventID = toLong( strAttr );
          break;

     case 2 : // BillingCycleID - 帐期标识
          pStd->m_iBillingCycleID = toInt( strAttr );
          break;

     case 3 : // EventTypeID - 事件类型标识
          pStd->m_iEventTypeID = toInt( strAttr );
          break;

     case 4 : // BillingAreaCode - 计费方区号
          strcpy( pStd->m_sBillingAreaCode, strAttr.c_str() );
          break;

     case 5 : // BillingNBR - 计费方号码
          strcpy( pStd->m_sBillingNBR, strAttr.c_str() );
          break;

     case 6 : // BillingVisitAreaCode - 计费方到访地区号
          strcpy( pStd->m_sBillingVisitAreaCode, strAttr.c_str() );
          break;

     case 7 : // CallingAreaCode - 主叫方区号
          strcpy( pStd->m_sCallingAreaCode, strAttr.c_str() );
          break;

     case 8 : // CallingNBR - 主叫方号码
          strcpy( pStd->m_sCallingNBR, strAttr.c_str() );
          break;

     case 9 : // OrgCallingNBR - 原始的主叫方号码
          strcpy( pStd->m_sOrgCallingNBR, strAttr.c_str() );
          break;

     case 10 : // CallingSPTypeID - 主叫运营商类型标识
          pStd->m_iCallingSPTypeID = toInt( strAttr );
          break;

     case 11 : // CallingServiceTypeID - 主叫业务类型标识
          pStd->m_iCallingServiceTypeID = toInt( strAttr );
          break;

     case 13 : // CalledAreaCode - 被叫区号
          strcpy( pStd->m_sCalledAreaCode, strAttr.c_str() );
          break;

     case 14 : // CalledNBR - 被叫号码
          strcpy( pStd->m_sCalledNBR, strAttr.c_str() );
          break;

     case 15 : // OrgCalledNBR - 原始的被叫方号码
          strcpy( pStd->m_sOrgCalledNBR, strAttr.c_str() );
          break;

     case 16 : // CalledSPTypeID - 被叫运营商类型标识
          pStd->m_iCalledSPTypeID = toInt( strAttr );
          break;

     case 17 : // CalledServiceTypeID - 被叫业务类型标识
          pStd->m_iCalledServiceTypeID = toInt( strAttr );
          break;

     case 19 : // ThirdPartyAreaCode - 第三方号码区号
          strcpy( pStd->m_sThirdPartyAreaCode, strAttr.c_str() );
          break;

     case 20 : // ThirdPartyNBR - 第三方号码
          strcpy( pStd->m_sThirdPartyNBR, strAttr.c_str() );
          break;

     case 21 : // StartDate - 事件起始时间
          strcpy( pStd->m_sStartDate, strAttr.c_str() );
          break;

     case 22 : // EndDate - 事件结束时间
          strcpy( pStd->m_sEndDate, strAttr.c_str() );
          break;

     case 23 : // Duration - 事件时长
          pStd->m_lDuration = toLong( strAttr );
          break;

     case 24 : // SwitchID - 交换机标识
          pStd->m_iSwitchID = toInt( strAttr );
          break;

     case 25 : // TrunkIn - 入中继号
          strcpy( pStd->m_sTrunkIn, strAttr.c_str() );
          break;

     case 26 : // TrunkInTypeID - 入中继类型标识
          pStd->m_iTrunkInTypeID = toInt( strAttr );
          break;

     case 27 : // TrunkOut - 出中继号
          strcpy( pStd->m_sTrunkOut, strAttr.c_str() );
          break;

     case 28 : // TrunkOutTypeID - 出中继类型
          pStd->m_iTrunkOutTypeID = toInt( strAttr );
          break;

     case 29 : // FileID - 文件标识
          pStd->m_iFileID = toInt( strAttr );
          break;

     case 30 : // EventState - 事件状态
          strcpy( pStd->m_sEventState, strAttr.c_str() );
          break;

     case 31 : // CreatedDate - 事件到达计费系统时间
          strcpy( pStd->m_sCreatedDate, strAttr.c_str() );
          break;

     case 32 : // StateDate - 事件确定状态的时间
          strcpy( pStd->m_sStateDate, strAttr.c_str() );
          break;

     case 33 : // CardID - 卡标识
          strcpy( pStd->m_sCardID, strAttr.c_str() );
          break;

     case 34 : // BillingTypeID - 计费类型标识
          pStd->m_iBillingTypeID = toInt( strAttr );
          break;

     case 35 : // ParterID - 合作方标识
          pStd->m_iParterID = toInt( strAttr );
          break;

     case 36 : // ServID - 用户标识
          pStd->m_iServID = toLong( strAttr );
          break;

     case 37 : // CustID - 客户标识
          pStd->m_iCustID = toLong( strAttr );
          break;

     case 38 : // ProdID - 产品标识
          pStd->m_iProdID = toInt( strAttr );
          break;

     case 39 : // ServProdID - 附属产品标识
          pStd->m_iServProdID = toInt( strAttr );
          break;

//      case 40 : // ProdBundleID - 产品包标识
//           pStd->m_iProdBundleID = toInt( strAttr );
//           break;

     case 41 : // Offers[0] - 商品标识1
          pStd->m_lOffers[0] = toLong( strAttr );
          break;

     case 42 : // Offers[1] - 商品标识2
          pStd->m_lOffers[1] = toLong( strAttr );
          break;

     case 43 : // Offers[2] - 商品标识3
          pStd->m_lOffers[2] = toLong( strAttr );
          break;

     case 44 : // Offers[3] - 商品标识4
          pStd->m_lOffers[3] = toLong( strAttr );
          break;

     case 45 : // Offers[4] - 商品标识5
          pStd->m_lOffers[4] = toLong( strAttr );
          break;

     case 46 : // BillingRecord - 算费过程记录
          strcpy( pStd->m_sBillingRecord, strAttr.c_str() );
          break;

     case 47 : // ErrorID - 错误号标识
          pStd->m_iErrorID = toInt( strAttr );
          break;

     case 48 : // AccountAreaCode - 数据业务帐号所在地区区号
          strcpy( pStd->m_sAccountAreaCode, strAttr.c_str() );
          break;

     case 49 : // AccountNBR - 数据业务帐号
          strcpy( pStd->m_sAccountNBR, strAttr.c_str() );
          break;

     case 50 : // NASIp - 网络接入IP
          strcpy( pStd->m_sNASIp, strAttr.c_str() );
          break;

     case 51 : // NASPortID - 网络接入端口标识
          pStd->m_iNASPortID = toInt( strAttr );
          break;

     case 52 : // RecvBytes - 接收字节数
          pStd->m_lRecvAmount = toLong( strAttr );
          break;

     case 53 : // SendBytes - 发送字节数
          pStd->m_lSendAmount = toLong( strAttr );
          break;

     //case 54 : // RecvPackets - 接收包数
     //     pStd->m_iRecvPackets = toInt( strAttr );
     //     break;

     //case 55 : // SendPackets - 发送包数
     //     pStd->m_iSendPackets = toInt( strAttr );
     //     break;

     case 56 : // NegotiatedQoS - 服务质量
          pStd->m_iNegotiatedQoS = toInt( strAttr );
          break;

     //case 57 : // CycleBeginDate - 帐期起始时间
     //     strcpy( pStd->m_sCycleBeginDate, strAttr.c_str() );
     //     break;
     //
     //case 58 : // CycleEndDate - 帐期结束时间
     //     strcpy( pStd->m_sCycleEndDate, strAttr.c_str() );
     //     break;

     case 59 : // ExtUsedNum - 扩展属性字段使用个数
          pStd->m_iExtUsedNum = toInt( strAttr );
          break;

     case 60 : // BusinessKey - 业务键
          pStd->m_iBusinessKey = toInt( strAttr );
          break;

//      case 61 : // SerProdAttrUsedNum -  附属产品属性实际使用的数目
//           pStd->m_iSerProdAttrUsedNum = toInt( strAttr );
//           break;

     case 62 : // ChargeInfo[0].m_lOrgCharge - 第1个费用域的原始费用
          pStd->m_lOrgCharge[0] = toInt( strAttr );
          break;

     case 63 : // StdChargeInfo[0].m_iStdCharge - 第1个费用域的标准费用
          pStd->m_oStdChargeInfo[0].m_lStdCharge = toLong( strAttr );
          break;

     case 64 : // ChargeInfo[0].m_iCharge - 第1个费用域的最终费用
          pStd->m_oChargeInfo[0].m_lCharge = toLong( strAttr );
          break;

     case 65 : // ChargeInfo[0].m_iAcctItemTypeID - 第1个费用域的帐目类型标识
          pStd->m_oChargeInfo[0].m_iAcctItemTypeID = toInt( strAttr );
          break;

     case 66 : // ChargeInfo[0].m_iAcctItemTypeID - 第1个费用域的优惠帐目类型标识
          pStd->m_oChargeInfo[0].m_iAcctItemTypeID = toInt( strAttr );
          break;

     case 67 : // ChargeInfo[1].m_lOrgCharge - 第2个费用域的原始费用
          pStd->m_lOrgCharge[1] = toInt( strAttr );
          break;

     case 68 : // StdChargeInfo[1].m_iStdCharge - 第2个费用域的标准费用
          pStd->m_oStdChargeInfo[1].m_lStdCharge = toLong( strAttr );
          break;

     case 69 : // ChargeInfo[1].m_iCharge - 第2个费用域的最终费用
          pStd->m_oChargeInfo[1].m_lCharge = toLong( strAttr );
          break;

     case 70 : // ChargeInfo[1].m_iAcctItemTypeID - 第2个费用域的帐目类型标识
          pStd->m_oChargeInfo[1].m_iAcctItemTypeID = toInt( strAttr );
          break;

     case 71 : // ChargeInfo[1].m_iAcctItemTypeID - 第2个费用域的优惠帐目类型标识
          pStd->m_oChargeInfo[1].m_iAcctItemTypeID = toInt( strAttr );
          break;

//      case 72 : // ServProdAttr[0].m_iAttrID - 第1个附属产品信息的属性标识
//           pStd->m_iServProdAttr[0].m_iAttrID = toInt( strAttr );
//           break;

//      case 73 : // ServProdAttr[0].m_iAttrVal - 第1个附属产品信息的属性取值
//           strcpy( pStd->m_sServProdAttr[0].m_iAttrVal, strAttr.c_str() );
//           break;

//      case 74 : // ServProdAttr[0].m_iAttrEffDate - 第1个附属产品信息的生效时间
//           strcpy( pStd->m_sServProdAttr[0].m_iAttrEffDate, strAttr.c_str() );
//           break;

//      case 75 : // ServProdAttr[0].m_iAttrExpDate - 第1个附属产品信息的失效时间
//           strcpy( pStd->m_sServProdAttr[0].m_iAttrExpDate, strAttr.c_str() );
//           break;

//      case 76 : // ServProdAttr[1].m_iAttrID - 第2个附属产品信息的属性标识
//           pStd->m_iServProdAttr[1].m_iAttrID = toInt( strAttr );
//           break;

//      case 77 : // ServProdAttr[1].m_iAttrVal - 第2个附属产品信息的属性取值
//           strcpy( pStd->m_sServProdAttr[1].m_iAttrVal, strAttr.c_str() );
//           break;

//      case 78 : // ServProdAttr[1].m_iAttrEffDate - 第2个附属产品信息的生效时间
//           strcpy( pStd->m_sServProdAttr[1].m_iAttrEffDate, strAttr.c_str() );
//           break;

//      case 79 : // ServProdAttr[1].m_iAttrExpDate - 第2个附属产品信息的失效时间
//           strcpy( pStd->m_sServProdAttr[1].m_iAttrExpDate, strAttr.c_str() );
//           break;

     case 80 : // CallTypeID - 呼叫类型标识
          pStd->m_iCallTypeID = toInt( strAttr );
          break;

     case 81 : // BillingOrgID - 计费方运营组织标识
          pStd->m_iBillingOrgID = toInt( strAttr );
          break;

//      case 82 : // BillFlowID - 计费流程标识
//           pStd->m_iBillFlowID = toInt( strAttr );
//           break;

     case 83 : // ChargeInfo[0].m_lBillMeasure - 第1个费用域的计费量
          pStd->m_oChargeInfo[0].m_lBillMeasure = toInt( strAttr );
          break;

     case 84 : // ChargeInfo[1].m_lBillMeasure - 第2个费用域的计费量
          pStd->m_oChargeInfo[1].m_lBillMeasure = toInt( strAttr );
          break;

     case 201 : // OrgSequence - 事件原始序列号
          strcpy( pStd->m_oEventExt.m_sOrgSequence, strAttr.c_str() );
          break;

     case 202 : // PartialIndicator - 事件部分指示
          pStd->m_oEventExt.m_iPartialIndicator = toInt( strAttr );
          break;

     case 203 : // MiddleSequnce - 中间记录序列号
          pStd->m_oEventExt.m_iMiddleSequnce = toInt( strAttr );
          break;

//      case 204 : // RecordIndex - 话单记录原始主索引
//           pStd->m_iRecordIndex = toInt( strAttr );
//           break;

     case 205 : // HotBillingTag - 热计费标识
          pStd->m_oEventExt.m_iHotBillingTag = toInt( strAttr );
          break;

     case 206 : // Causefortermination - 挂断标识
          pStd->m_oEventExt.m_iCausefortermination = toInt( strAttr );
          break;

     case 207 : // MSC - 移动交换机标识
          strcpy( pStd->m_sMSC, strAttr.c_str() );
          break;

     case 208 : // LAC - 小区号标识
          strcpy( pStd->m_sLAC, strAttr.c_str() );
          break;

     case 209 : // CellID - 基站号标识
          strcpy( pStd->m_sCellID, strAttr.c_str() );
          break;

     case 210 : // RomingNBR - 动态漫游号标识
          strcpy( pStd->m_sRomingNBR, strAttr.c_str() );
          break;

     case 211 : // NoBillFlag - 免费标志标识

          pStd->m_iNoBillFlag = toInt( strAttr );
          break;

     case 212 : // CallingVisitAreaCode - 主叫到访地区号标识
          strcpy( pStd->m_oEventExt.m_sCallingVisitAreaCode, strAttr.c_str() );
          break;

     case 213 : // CalledVisitAreaCode - 被叫到访地区号标识
          strcpy( pStd->m_oEventExt.m_sCalledVisitAreaCode, strAttr.c_str() );
          break;

     case 214 : // IMSI - IMSI
          strcpy( pStd->m_oEventExt.m_sIMSI, strAttr.c_str() );
          break;

     case 215 : // IMEI - IMEI
          strcpy( pStd->m_oEventExt.m_sIMEI, strAttr.c_str() );
          break;

     case 216 : // ExtendID[0] - 扩展属性1标识
          pStd->m_aiExtFieldID[0] = toInt( strAttr );
          break;

     case 217 : // ExtendValue[0] - 扩展属性1取值
          strcpy( pStd->m_asExtValue[0], strAttr.c_str() );
          break;

     case 218 : // ExtendID[1] - 扩展属性2标识
          pStd->m_aiExtFieldID[1] = toInt( strAttr );
          break;

     case 219 : // ExtendValue[1] - 扩展属性2取值
          strcpy( pStd->m_asExtValue[1], strAttr.c_str() );
          break;

     case 220 : // ExtendID[2] - 扩展属性3标识
          pStd->m_aiExtFieldID[2] = toInt( strAttr );
          break;

     case 221 : // ExtendValue[2] - 扩展属性3取值
          strcpy( pStd->m_asExtValue[2], strAttr.c_str() );
          break;

     case 222 : // ExtendID[3] - 扩展属性4标识
          pStd->m_aiExtFieldID[3] = toInt( strAttr );
          break;

     case 223 : // ExtendValue[3] - 扩展属性4取值
          strcpy( pStd->m_asExtValue[3], strAttr.c_str() );
          break;

     case 224 : // ExtendID[4] - 扩展属性5标识
          pStd->m_aiExtFieldID[4] = toInt( strAttr );
          break;

     case 225 : // ExtendValue[4] - 扩展属性5取值
          strcpy( pStd->m_asExtValue[4], strAttr.c_str() );
          break;

     case 226 : // ExtendID[5] - 扩展属性6标识
          pStd->m_aiExtFieldID[5] = toInt( strAttr );
          break;

     case 227 : // ExtendValue[5] - 扩展属性6取值
          strcpy( pStd->m_asExtValue[5], strAttr.c_str() );
          break;

     case 228 : // ExtendID[6] - 扩展属性7标识
          pStd->m_aiExtFieldID[6] = toInt( strAttr );
          break;

     case 229 : // ExtendValue[6] - 扩展属性7取值
          strcpy( pStd->m_asExtValue[6], strAttr.c_str() );
          break;

     case 230 : // ExtendID[7] - 扩展属性8标识
          pStd->m_aiExtFieldID[7] = toInt( strAttr );
          break;

     case 231 : // ExtendValue[7] - 扩展属性8取值
          strcpy( pStd->m_asExtValue[7], strAttr.c_str() );
          break;

     case 232 : // ExtendID[8] - 扩展属性9标识
          pStd->m_aiExtFieldID[8] = toInt( strAttr );
          break;

     case 233 : // ExtendValue[8] - 扩展属性9取值
          strcpy( pStd->m_asExtValue[8], strAttr.c_str() );
          break;

     case 234 : // ExtendID[9] - 扩展属性10标识
          pStd->m_aiExtFieldID[9] = toInt( strAttr );
          break;

     case 235 : // ExtendValue[9] - 扩展属性10取值
          strcpy( pStd->m_asExtValue[9], strAttr.c_str() );
          break;
     case 236 :
	  strcpy( pStd->m_sReservedField[0], strAttr.c_str() );
	  break;
	      case 237:    	 
    	 strcpy(pStd->m_sReservedField[1],strAttr.c_str());
	break;
    case 238:
         strcpy(pStd->m_sReservedField[2],strAttr.c_str());
	break;
     case 239:
         strcpy(pStd->m_sReservedField[3],strAttr.c_str());
	break;
     case 240:
        strcpy(pStd->m_sReservedField[4],strAttr.c_str());
	break;
     case 243:
	  pStd->m_lSumAmount=toLong( strAttr );
	  break;
     case 252 :
	  strcpy(pStd->m_sSessionID,strAttr.c_str());
	  break;     
     case 253:
     	  strcpy(pStd->m_sOrgThirdPartyNBR,strAttr.c_str());
     	  break;
     case 256:
     	  strcpy(pStd->m_sDomainName,strAttr.c_str());
     	  break;
     case 257:
	  	pStd->m_iGroupID=toInt(strAttr);
	  	break;
	   case 259 : 
			 strcpy( pStd->m_sBindAttr, strAttr.c_str() );
			 break;
     case 260:
     	  pStd->m_iServiceType = toInt(strAttr);
     	  break;
     case 261:
     	  pStd->m_iUserType = toInt(strAttr);
	   		break;
	 case 262:
		 strcpy(pStd->m_sAreaNo,strAttr.c_str());
		 break;
	 case 263:
		 pStd->m_lSepRecID=toLong(strAttr);
		 break;

//      case 301 : // CR_ServiceCode[15] - 彩铃服务编码
//           strcpy( pStd->m_sCR_ServiceCode[15], strAttr.c_str() );
//           break;

     case 280:     	  
     	  pStd->m_oEventExt.m_lExtendValue[0]= toInt(strAttr);
	break;
     case 281:
     	  pStd->m_oEventExt.m_lExtendValue[1]= toInt(strAttr);
	break;
     case 282:
     	  pStd->m_oEventExt.m_lExtendValue[2]= toInt(strAttr);
	break;
     case 283:
     	  pStd->m_oEventExt.m_lExtendValue[3]= toInt(strAttr);
	break;
     case 284:
     	  pStd->m_oEventExt.m_lExtendValue[4]= toInt(strAttr);
	break;
     case 285:
     	  pStd->m_oEventExt.m_lExtendValue[5]= toInt(strAttr);
	break;
     case 286:
     	  pStd->m_oEventExt.m_lExtendValue[6]= toInt(strAttr);
	break;
     case 287:
     	  pStd->m_oEventExt.m_lExtendValue[7]= toInt(strAttr);
	break;
     case 288:
     	  pStd->m_oEventExt.m_lExtendValue[8]= toInt(strAttr);
	break;
     case 289:
     	 pStd->m_oEventExt.m_lExtendValue[9]= toInt(strAttr);
	break;
//返回stdevent.m_oEventExt.m_sEventValue的值     	  
     case 290:
     	 strcpy(pStd->m_oEventExt.m_sExtendValue[0],strAttr.c_str());
	break;
     case 291:
     	  strcpy(pStd->m_oEventExt.m_sExtendValue[1],strAttr.c_str());
	break;
     case 292:
     	 strcpy(pStd->m_oEventExt.m_sExtendValue[2],strAttr.c_str());
	break;
     case 293:
     	  strcpy(pStd->m_oEventExt.m_sExtendValue[3],strAttr.c_str());
	break;
     case 294:
     	  strcpy(pStd->m_oEventExt.m_sExtendValue[4],strAttr.c_str());
	break;
     case 295:
     	  strcpy(pStd->m_oEventExt.m_sExtendValue[5],strAttr.c_str());
	break;
     case 296:
     	  strcpy(pStd->m_oEventExt.m_sExtendValue[6],strAttr.c_str());
	break;
     case 297:
     	  strcpy(pStd->m_oEventExt.m_sExtendValue[7],strAttr.c_str());
	break;
     case 298:
     	  strcpy(pStd->m_oEventExt.m_sExtendValue[8],strAttr.c_str());
	break;
     case 299:
     	  strcpy(pStd->m_oEventExt.m_sExtendValue[9],strAttr.c_str());
	break;

      case 302 : // SPCode[10] - SP代码
           strcpy( pStd->m_sSPFLAG, strAttr.c_str() );
           break;
      case 435:
      	    strcpy(pStd->m_oEventExt.m_sAccessPointName,strAttr.c_str());
      	    break;

//      case 303 : // spservicekey[15] - SP短信业务代码
//           strcpy( pStd->m_sspservicekey[15], strAttr.c_str() );
//           break;

//      case 304 : // SPsmstype[15] - SP短信类型
//           strcpy( pStd->m_sSPsmstype[15], strAttr.c_str() );
//           break;

//      case 401 : // TimeStruct - 时间信息结构
//           strcpy( pStd->m_sTimeStruct, strAttr.c_str() );
//           break;

//      case 402 : // StartDate - 起始日期(YYYYMMDD)
//           strcpy( pStd->m_sStartDate, strAttr.c_str() );
//           break;

//      case 403 : // StartTime - 起始时间(HH24MISS)
//           strcpy( pStd->m_sStartTime, strAttr.c_str() );
//           break;

//      case 404 : // EndDate - 结束日期(YYYYMMDD)
//           strcpy( pStd->m_sEndDate, strAttr.c_str() );
//           break;

//      case 405 : // EndTime - 结束时间(HH24MISS)
//           strcpy( pStd->m_sEndTime, strAttr.c_str() );
//           break;

//      case 406 : // Up1 - 上行流量1
//           pStd->m_iUp1 = toInt( strAttr );
//           break;

//      case 407 : // Down1 - 下行流量1
//           pStd->m_iDown1 = toInt( strAttr );
//           break;

//      case 408 : // Up2 - 上行流量2
//           pStd->m_iUp2 = toInt( strAttr );
//           break;

//      case 409 : // Down2 - 下行流量2
//           pStd->m_iDown2 = toInt( strAttr );
//           break;

//      case 410 : // Up3 - 上行流量3
//           pStd->m_iUp3 = toInt( strAttr );
//           break;

//      case 411 : // Down3 - 下行流量3
//           pStd->m_iDown3 = toInt( strAttr );
//           break;

//      case 412 : // Up4 - 上行流量4
//           pStd->m_iUp4 = toInt( strAttr );
//           break;

//      case 413 : // Down4 - 下行流量4
//           pStd->m_iDown4 = toInt( strAttr );
//           break;

//      case 414 : // Up5 - 上行流量5
//           pStd->m_iUp5 = toInt( strAttr );
//           break;

//      case 415 : // Down5 - 下行流量5
//           pStd->m_iDown5 = toInt( strAttr );
//           break;

//      case 416 : // Up6 - 上行流量6
//           pStd->m_iUp6 = toInt( strAttr );
//           break;

//      case 417 : // Down6 - 下行流量6
//           pStd->m_iDown6 = toInt( strAttr );
//           break;

//      case 418 : // Up7 - 上行流量7
//           pStd->m_iUp7 = toInt( strAttr );
//           break;

//      case 419 : // Down7 - 下行流量7
//           pStd->m_iDown7 = toInt( strAttr );
//           break;

//      case 420 : // Up8 - 上行流量8
//           pStd->m_iUp8 = toInt( strAttr );
//           break;

//      case 421 : // Down8 - 下行流量8
//           pStd->m_iDown8 = toInt( strAttr );
//           break;

//      case 422 : // Up9 - 上行流量9
//           pStd->m_iUp9 = toInt( strAttr );
//           break;

//      case 423 : // Down9 - 下行流量9
//           pStd->m_iDown9 = toInt( strAttr );
//           break;

//      case 424 : // Up10 - 上行流量10
//           pStd->m_iUp10 = toInt( strAttr );
//           break;

//      case 425 : // Down10 - 下行流量10
//           pStd->m_iDown10 = toInt( strAttr );
//           break;

     default :
          break;
     }
}


// 函数说明：long型转string
// 参数说明：
//         const long lval        long型数据
// 返回值说明：
//        转换后的值
string EventToolKits::toString( const long lval )
{
     m_oSstream.str("");
     m_oSstream << lval;
     return m_oSstream.str();
}

// 函数说明：int型转string
// 参数说明：
//         const int ival        int型数据
// 返回值说明：
//        转换后的值
string EventToolKits::toString( const int ival )
{
     m_oSstream.str("");
     m_oSstream << ival;
     return m_oSstream.str();
}

int EventToolKits::toInt( string strval )
{
     return atoi( trim( strval ).c_str() );
}
// 函数说明：string型转int
// 参数说明：
//         const int ival        int型数据
// 返回值说明：
//        转换后的值
bool EventToolKits::toBool( string strval )
{
     lowercase( strval );
     if ( strval == "0" || strval == "false" )
     {
          return false;
     }
     return true;
}
// 函数说明：全部转换为小写
// 参数说明：
//         string &strval        待转换的值
// 返回值说明：
//        转换后的值
string &EventToolKits::lowercase( string &strval )
{
     for_each( strval.begin(), strval.end(), lowertrans() );
     return strval;
}

// 函数说明：全部转换为大写
// 参数说明：
//         string &strval        待转换的值
// 返回值说明：
//        转换后的值
string &EventToolKits::uppercase( string &strval )
{
     for_each( strval.begin(), strval.end(), uppertrans() );
     return strval;
}

// 函数说明：去除所有空格
// 参数说明：
//         string &strval        待转换的值
// 返回值说明：
//        转换后的值
string EventToolKits::trim( string &strval )
{
     string::size_type pos = 0;
     while ( ( pos = strval.find_first_of( ' ', pos ) ) != string::npos )
     {
          strval.erase( pos, 1 );
     }
     return strval;
}

// 函数说明：字符串转换为日期（YYYYMMDD）
// 参数说明：
//         const char *strDate        待转换的值
// 返回值说明：
//        转换后的值
time_t EventToolKits::toDate( const char *strDate )
{
     struct tm tm_time = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
     char sTime[5];
     memcpy( sTime, strDate, 4 );
     tm_time.tm_year = atoi( sTime ) - 1900;
     memcpy( sTime, strDate + 4, 2 );
     sTime[2] = '\0';
     tm_time.tm_mon = atoi( sTime ) - 1;
     memcpy( sTime, strDate + 6, 2 );
     sTime[2] = '\0';
     tm_time.tm_mday = atoi( sTime ) - 1;
     return mktime( &tm_time );
}
// 函数说明：字符串转换为时间（HH24MISS）
// 参数说明：
//         const char *strTime        待转换的值
// 返回值说明：
//        转换后的值
time_t EventToolKits::toTime( const char *strTime )
{
     struct tm tm_time = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
     char sTime[3];
     memcpy( sTime, strTime, 2 );
     sTime[2] = '\0';
     tm_time.tm_hour = atoi( sTime ) - 1;
     memcpy( sTime, strTime + 2, 2 );
     sTime[2] = '\0';
     tm_time.tm_min = atoi( sTime ) - 1;
     memcpy( sTime, strTime + 4, 2 );
     sTime[2] = '\0';
     tm_time.tm_sec = atoi( sTime ) - 1;
     return mktime( &tm_time );
}
// 函数说明：字符串转换为日期时间（YYYYMMDDHH24MISS）
// 参数说明：
//         const char *strDateTime        待转换的值
// 返回值说明：
//        转换后的值
time_t EventToolKits::toDateTime( const char *strDateTime )
{
     struct tm tm_time = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
     char sTime[5];
     memcpy( sTime, strDateTime, 4 );
     tm_time.tm_year = atoi( sTime ) - 1900;
     memcpy( sTime, strDateTime + 4, 2 );
     sTime[2] = '\0';
     tm_time.tm_mon = atoi( sTime ) - 1;
     memcpy( sTime, strDateTime + 6, 2 );
     sTime[2] = '\0';
     tm_time.tm_mday = atoi( sTime ) - 1;
     memcpy( sTime, strDateTime + 8, 2 );
     sTime[2] = '\0';
     tm_time.tm_hour = atoi( sTime ) - 1;
     memcpy( sTime, strDateTime + 10, 2 );
     sTime[2] = '\0';
     tm_time.tm_min = atoi( sTime ) - 1;
     memcpy( sTime, strDateTime + 12, 2 );
     sTime[2] = '\0';
     tm_time.tm_sec = atoi( sTime ) - 1;
     return mktime( &tm_time );
}

// 函数说明：计算时间差（YYYYMMDDHH24MISS）
// 参数说明：
//         const char *strTime1    时间1
//        const char *strTime2    时间2
// 返回值说明：
//        相差秒数
double EventToolKits::time_diff( const char *strTime1, const char *strTime2 )
{
     return difftime( toDateTime( strTime1 ), toDateTime( strTime2 ) );
}
// 函数说明：计算时间差（YYYYMMDDHH24MISS）
// 参数说明：
//         string const &strTime1    时间1
//        string const &strTime2    时间2
// 返回值说明：
//        相差秒数
double EventToolKits::time_diff( string const &strTime1, string const &strTime2 )
{
     return time_diff( strTime1.c_str(), strTime2.c_str() );
}
// 函数说明：string型转float
// 参数说明：
//         string &strval        待转换的值
// 返回值说明：
//        转换后的值
double EventToolKits::toFloat( string strval )
{
     return atof( strval.c_str() );
}

long EventToolKits::toLong(string strval)
{
    return atol(strval.c_str());
}



