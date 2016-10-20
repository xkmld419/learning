/*VER: 3*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef ENVIRONMENT_H_HEADER_INCLUDED_BDBDC8E4
#define ENVIRONMENT_H_HEADER_INCLUDED_BDBDC8E4
#include "TOCIQuery.h"


class AggrMgr;
class TransactionMgr;
class Serv;

class SHMFileInfo;
#ifndef _JUST_DBLINK_
#include "UserInfoReader.h"
//xueqt #include "EventSectionUnioner.h"
#include "EventSection.h"
//xueqt #include "ReferenceObjectMgr.h"
//#include "EventTypeMgr.h"
#include <iostream>
#include "ReadIni.h"
#include "encode.h"
//#include "PricingMeasure.h"
//#include "PricingCombine.h"
//#include "OfferMgr.h"
//#include "CombDisctAssist.h"
//#include "../app_rent/GridRentDataMgr.h"
//#include "../app_rent/GridSetDataMgr.h"

class ProdOfferDisctAggr;
class ProdOfferTotalData;
class UserInfoInterface;


#ifdef HAVE_ABM
class TempAccuMgr;
#endif

#endif
class ParamInfoInterface;
class ParamInfoMgr;

#ifdef DEF_LINUX
#include <getopt.h>
#endif

using namespace std;

#define DEFINE_QUERY(x) TOCIQuery x (Environment::getDBConn())
#define DB_CONNECTED (Environment::m_bConnect)
/*
#ifndef _JUST_DBLINK_
#define G_CURMEASURE (Environment::m_iCurMeasure)
#define G_CURATTRID (Environment::m_iCurAttrID)
#define G_BABM (Environment::m_bAbm)
#define G_PUSERINFO (Environment::m_poUserInfoInterface)
#define G_SERV (*(Environment::m_poServ))
#define G_PSERV (Environment::m_poServ)
#define G_PESU (Environment::m_poEventSectionUnioner)
#define G_PTRANSMGR (Environment::m_poTransMgr)
#define G_PAGGRMGR (Environment::m_poAggrMgr)
//#define G_PROBJECTMGR (Environment::m_poReferenceObjectMgr)
#define G_PACCTITEMMGR (Environment::m_poAcctItemMgr)
#define G_PEVENTTYPEMGR (Environment::m_poEventTypeMgr)
#define G_PORGMGR (Environment::m_poOrgMgr)

#define G_POFFERINSQO (Environment::m_poOfferInsQO)
#define G_PCYCLEACCTBUF (Environment::m_poCycleAcctBuf)
#define G_CYCLEACCTBUF (*(Environment::m_poCycleAcctBuf))

//一次事件的累积量
#define G_PEVENTACCUBUF (Environment::m_poEventAccuBuf)
#define G_EVENTACCUBUF (*(Environment::m_poEventAccuBuf))

//账目优惠累积量全局变量   
#define G_POPRODOFFFERDISCT  (Environment::m_ProdOfferAggr)
    
#define G_CURPRICINGCOMBING (Environment::m_poCurPricingCombine)
//定价度量全局变量
#define G_PRICINGMEASURE (Environment::m_poCurPricingMeasure)
//批价中使用的事件的全局变量
#define G_PEVENT (Environment::m_poStdEvent)
#define G_POFFERMGR (Environment::m_poOfferMgr)

#define G_PCOMBDISCT (Environment::m_poCombDisctAssist)

//参数共享内存访问宏
//#define G_PPARAMINFO (Environment::m_poParamInfoInterface->m_pParamDataBuf[Environment::m_poParamInfoInterface->m_pParamAccessPort->m_iShmIdx])
#define G_PPARAMINFO (Environment::getSHM())
#define G_PPARAMREAL (Environment::linkSHM())

#define G_PPARAM_DETACH (Environment::SHMDetach())
#define G_PPARAMINFO_FLASH (Environment::flashSHM())//在获取参数之前,要刷新一下对内存参数的绑定,返回BOOL类型数据,true刷新了,false无需刷新
//#define G_SHMSTATE(Environment::getState()) //true 执行了P处理 ;false 无P-V处理
#define G_PPARAMSHM_P (Environment::shm_P())
#define G_PPARAMSHM_V (Environment::shm_V())

#define G_PPARAMSHM (Environment::m_poParamSHM)
#ifdef HAVE_ABM
#define G_BABM (Environment::m_bAbm)
#define G_PTEMPACCUMGR (Environment::m_poTempAccuMgr)
#endif
//网格化租费、套餐费共享内存管理
//#define G_PGRID_RENT_DATA_MGR (Environment::m_poGridRentDataMgr)
//#define G_PGRID_SET_DATA_MGR (Environment::m_poGridSetDataMgr)

extern EventSection * g_poEventSection;

#define G_SHMFILE (Environment::refleshSHMFile())
#define G_SHMFILEPOINT (Environment::m_pSHMFileInterface->m_pSHMFileExternal)
#endif
*/



extern int g_argc;
extern char ** g_argv;

#ifdef DEF_AIX
int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
#else
#endif

//class OfferInsQO;



//##ModelId=424BBCEA011D
//##Documentation
//## 运行环境
class Environment
{
  public:
    //##ModelId=424BBCEA01A8
    //##Documentation
    //## 由Process调用
    static void setDBLogin(char *name, char *pwd, char *str);

    //##ModelId=424BBCEA01C6
    //##Documentation
    //## 由各模块调用
    static TOCIDatabase *getDBConn(bool bReconnect = false);
    static void commit();
    //##ModelId=42957FF802D6
    static void rollback();
    static void disconnect();    
    static void connectDB(TOCIDatabase * pDbConn, char const * sIniHeader);
    static void getConnectInfo(char const * sIniHeader,char *sUserName,char *sPasswd,char *sConnStr);

    
    //##ModelId=427DD69F0253
/*    
    static void useUserInfo();

static void useParamShm();

	//绑定用户资料，指定模式来绑定
	static void useUserInfo(int iMode);
	
	static ParamInfoMgr *getSHM();//获取数据管理指针
	static ParamInfoMgr *linkSHM();//获取数据管理指针
	static bool SHMDetach();//释放对共享内存参数的连接
	static bool shm_P();//加锁
    static bool shm_V();//解锁
    static bool getState();//获取当前锁状态
    static bool flashSHM();//刷新跟内存的连接
	//锁标识
	static bool SHMLock;//P-V处理
    static int iLinkSHM;//P-V处理数据记录
    //刷新次数
	static int iFlashSeqIdx;
	//当前PROCESS_ID
	static int gpShmParamLink_ProcessID;
	//PID
	static int gpShmParamLink_SysPID;
	
    //##ModelId=42957FEE0390


#ifndef _JUST_DBLINK_
    //##ModelId=427DD6A703E6
   // static void useEventSectionUnion();
    //##ModelId=427DD6C400EE
    static UserInfoInterface *m_poUserInfoInterface;
    //##ModelId=427DD6D403E0
    static Serv *m_poServ;
    //##ModelId=427DD6E601B5
    //static EventSectionUnioner *m_poEventSectionUnioner;
    //##ModelId=4285B7990261
    static TransactionMgr *m_poTransMgr;
    static AggrMgr *m_poAggrMgr;
    //##ModelId=4285C05E0093
//    static ReferenceObjectMgr *m_poReferenceObjectMgr;

    static AcctItemMgr * m_poAcctItemMgr;

    static OrgMgr * m_poOrgMgr;

    static EventTypeMgr * m_poEventTypeMgr;

    static OfferInsQO * m_poOfferInsQO;
    //账目优惠累积量
    static ProdOfferDisctAggr *m_ProdOfferAggr;
    //商品实例全局变量
    static PricingCombine  *m_poCurPricingCombine;
    //当前正在处理的度量全局变量
    static PricingMeasure  *m_poCurPricingMeasure;
	//当前正在处理的事件
	static StdEvent  * m_poStdEvent;
	static OfferMgr  * m_poOfferMgr;

    static vector<StdAcctItem> * m_poCycleAcctBuf;
    static int m_iCurMeasure;
    
    static int  m_iNewCycleTypeId;  

    static multimap<int, long> * m_poEventAccuBuf;

	//组合保底优惠协助处理类
	static CombDisctAssist * m_poCombDisctAssist;


    

#ifdef HAVE_ABM
	//带ABM业务的标志
	static bool m_bAbm;

	//临时累积量存放
	static TempAccuMgr * m_poTempAccuMgr;
#endif

	//当前正在执行的属性ID
	static int m_iCurAttrID;

	//网格租费、网格套餐费共享内存管理
    //static GridRentDataMgr *m_poGridRentDataMgr;
    //static GridSetDataMgr *m_poGridSetDataMgr;

#endif
	//参数共享内存访问接口
    static ParamInfoInterface *m_poParamInfoInterface;

	static ParamInfoMgr *m_poParamSHM;
	
    //##ModelId=424BBCEA019E


    static SHMFileInfo * m_pSHMFileInterface;
	static void refleshSHMFile();
	*/
    static bool m_bConnect;	
  private:
    //##ModelId=424BBCEA014E
    static char m_sDBUser[24];

    //##ModelId=424BBCEA0176
    static char m_sDBPwd[24];

    //##ModelId=424BBCEA0180
    static char m_sDBStr[32];

    //##ModelId=424BBCEA0194
    static TOCIDatabase m_oDBConn;

    //##ModelId=427DC28A006F
    static bool m_bDefaultConn;

    static void getDefaultLogin();

};



#endif /* ENVIRONMENT_H_HEADER_INCLUDED_BDBDC8E4 */

