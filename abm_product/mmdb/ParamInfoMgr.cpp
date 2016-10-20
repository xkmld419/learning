#ifndef PRIVATE_MEMORY
#include "ParamInfoMgr.h"
#include "ParamDefineMgr.h"
#include "TOCIQuery.h"
#include "Environment.h"
#include <string.h>
#include "AcctItemLoad.h"
#include "SHMIntHashIndex_AX.h"
#include "SHMStringTreeIndex_AX.h"
#include "SHMIntHashIndex_KR.h"
#include "CommonMacro.h"
#include "DataEventMem.h"
#include "../app_servload/LoadServInfoMgr.h"
#include "BillingCycleMgr.h"
#include "MBC.h"
#include "CommandCom.h"
//int g_iCurEventType;//
#define CAL_COUNT_E(count) \
        (\
        (int ) \
                ( \
                        (count)>1000000 ? \
                        ((count)*1.05) : \
                        ( \
                                (count)>100000? \
                                ((count)*1.1): \
                                (\
                                        (count)>10000? \
                                        ((count)*1.15) : \
                                        ( \
                                                (count)>1000? \
                                                10100 : \
                                                2000 \
                                        ) \
                                ) \
                        )\
                ) \
         ) 

//#define MAX_EULECHECK_KEY_LEN	56
#define LOADPARAMNEEDINFO	"LOADPARAMNEEDINFO"

ParamInfoMgr::ParamInfoMgr():m_fRatio(0)
{
	m_iTotalMem = 0;
	m_iCurMem = 0;
	initMem = false;
        TOCIQuery qry(Environment::getDBConn());
    qry.setSQL ("SELECT t.EVENT_TYPE_ID FROM RATABLE_EVENT_TYPE t WHERE t.NAME='帐期末费用转移事件'");
    qry.open();
    while (qry.next ()) {
                if (!qry.field(0).asInteger ()) continue;
                Environment::m_iNewCycleTypeId=qry.field(0).asInteger ();
        }
        qry.close ();    
		m_iNo = -1;
}

ParamInfoMgr::ParamInfoMgr(int iNo):m_fRatio(0)
{
	m_iTotalMem = 0;
	m_iCurMem = 0;
	initMem = false;
	TOCIQuery qry(Environment::getDBConn());
    qry.setSQL ("SELECT t.EVENT_TYPE_ID FROM RATABLE_EVENT_TYPE t WHERE t.NAME='帐期末费用转移事件'");
    qry.open();
    while (qry.next ()) {
                if (!qry.field(0).asInteger ()) continue;
                Environment::m_iNewCycleTypeId=qry.field(0).asInteger ();
        }
        qry.close (); 
	m_iNo = iNo;
}
ParamInfoMgr::~ParamInfoMgr()
{
}

bool ParamInfoMgr::InitMem()
{
	if(initMem)
		return true;
	CommandCom *pCommandCom = new CommandCom();
	if(!pCommandCom)
	{
			Log::log(0,"内存空间不足!");
			ALARMLOG28(0,MBC_CLASS_Fatal, 2,"%s","内存空间不足[ParamInfoMgr.InitMem]");
			THROW(1);
	}
	if(!pCommandCom->InitClient())
	{
		Log::log(0,"注册失败!");
		THROW(1);
	}
	char sNow[128] = {0};
	pCommandCom->readIniString("MEMORY","shm.max_shm_size",sNow,"");
	delete pCommandCom;
	m_iTotalMem = atol(sNow);
	initMem = true;
	return true;
}

bool ParamInfoMgr::checkMem(unsigned int iDataMem)
{
	InitMem();
	if(m_iTotalMem)
	{
		m_iCurMem += iDataMem;
		if(m_iCurMem>m_iTotalMem){
			Log::log(0,"内存现有值[%d]超过限制[%ld]",m_iCurMem,m_iTotalMem);
			ALARMLOG28(0,MBC_CLASS_Fatal, 2,"内存现有值[%d]超过限制[%ld]",m_iCurMem,m_iTotalMem);
			THROW(1);
			return false;
		}
	}
	return true;
}

bool ParamInfoMgr::Needload(char const *_sCode)
{
		return Needload(LOADPARAMNEEDINFO,_sCode);
}

bool ParamInfoMgr::Needload(char const *_sSegment, char const *_sCode)
{
		char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(_sSegment, _sCode, countbuf))
        {
        	return false;
        }
		return true;
}
bool ParamInfoMgr::load()
{
    empty();

    //获取参数共享内存大小的配置系数,当未配置或配置小于1时设置为默认值1.3
    char countbuf[32]= {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, "PARAM_SIZE_RATIO_NUM", countbuf))
    {
        m_fRatio = 1.3f;
    }
    else
    {
        m_fRatio = atof(countbuf);
        if(m_fRatio <= 1.0f)
            m_fRatio = 1.3f;
    }
	if(Needload("loadMServAcct"))
	{
		try{
    		loadMServAcct();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
	try{
    	loadLogicStatement();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadAssignStatement();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadFormula();
    } catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadAttrTrans();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadTrunk();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadAreaCode();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadHLR();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	if(Needload("loadVisitAreaCode"))
	{
		try{
			loadVisitAreaCode();
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
	
	try{
		loadEdgeRoamEx();
	} catch(TOCIException &e)
	{
		Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
		return false;			
	}
	
	try{
    	loadCarrier();
	} catch(TOCIException &e)
	{
		Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
		return false;			
	}
	
	//new1
	if(Needload("loadNotPNSegBilling")){
		try{
			loadNotPNSegBilling();
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
	
	try{
		loadPNSegBillingEx();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{	
		loadTrunkTreeNode();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
		loadServiceInfo2();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
		loadTrunkBilling();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
		loadSpecTrunkProduct();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	if(Needload("loadSwitchItemType")){
		try{
			loadSwitchItemType();
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
	
	try{
		loadLocalAreaCode();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
		loadTimesType();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadCityInfo();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
		loadFilterRuleResult();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadOrg();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadOrgEx();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadOrgLongType();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadHeadEx();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadLocalHead();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadEventType();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadEventTypeGroup();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadOffer();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadOfferSpecAttr();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadOfferNumAttr();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadOfferFree();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadNbrMap();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}

	try{
   		loadAcctItem();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}

	try{
    	loadLongEventTypeRule();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadLongEventTypeRuleEx();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadPricingPlanType();
    } catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadStateConvert();
	} catch(TOCIException &e)
	{
		Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
		return false;			
	}
	
	try{
    	loadValueMap();
	} catch(TOCIException &e)
	{
		Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
		return false;			
	}
	
	try{
   		loadZoneItem();
	} catch(TOCIException &e)
	{
		Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
		return false;			
	}
	
	try{
    	loadSplitAcctItemGroup();
	} catch(TOCIException &e)
	{
		Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
		return false;			
	}
	
	try{
    	loadSplitGroupEvent();
	} catch(TOCIException &e)
	{
		Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
		return false;			
	}
	
	try{
    	loadDistanceType();
	} catch(TOCIException &e)
	{
		Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
		return false;			
	}
	
	try{
                loadEdgeRoam();
   	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	
	try{
    	loadSection();
	} catch(TOCIException &e)
	{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
	}

		try{
    		loadTariff();
    		Log::log(0, "资费参数上载正常结束. 记录数: %d", m_poTariffInfoData->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        	loadSpecialTariff();
        	Log::log(0, "特殊资费参数上载正常结束. 记录数: %d", m_poSpecialTariffCheckData->getCount());
        	Log::log(0, "特殊资费check参数上载正常结束. 记录数: %d", m_poSpecialTariffCheckData->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	
		try{
        	loadProductAcctItem();
        	Log::log(0, "ProductAcctItem参数上载正常结束. 记录数: %d", m_poProductIDIndex->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        loadDiscount();
        Log::log(0, "优惠参数上载正常结束. 记录数: %d", m_poDiscountInfoData->getCount());
        Log::log(0, "DisctApplyer参数上载正常结束. 记录数: %d", m_poDisctApplyerInfoData->getCount());
        Log::log(0, "DisctApplyObject参数上载正常结束. 记录数: %d", m_poDisctApplyObjectInfoData->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        loadPercent();
        Log::log(0, "折扣参数上载正常结束. 记录数: %d", m_poPercentInfoData->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
        try{
        loadLifeCycle();
        Log::log(0, "生命周期参数上载正常结束. 记录数: %d", m_poLifeCycleData->getCount());   
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        loadPricingMeasure();   
        Log::log(0, "度量参数上载正常结束. 记录数: %d", m_poPricingMeasureData->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        loadUsageType();
        Log::log(0, "事件类型参数上载正常结束. 记录数: %d", m_poUsageTypeInfoData->getCount());   
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
    loadTimeCond();
        Log::log(0, "TimeCond参数上载正常结束. 记录数: %d", m_poTimeCondData->getCount());   
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        loadEventTypeCond();
        Log::log(0, "EventTypeCond参数上载正常结束. 记录数: %d", m_poEventTypeCondData->getCount());   
        } catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}   
		
		try{             
        loadCycleType();
        Log::log(0, "CycleType参数上载正常结束. 记录数: %d", m_poCycleTypeData->getCount());   
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        loadAccuType();  
        Log::log(0, "AccuType参数上载正常结束. 记录数: %d", m_poAccuTypeData->getCount());   
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        loadOfferAccuRelation();    
        Log::log(0, "OfferAccuRelation参数上载正常结束. 记录数: %d", m_poOfferAccuRelationData->getCount());   
		} catch(TOCIException &e)
		{
			Log::log(0,"数据库获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        loadOfferAccuExist();   
        Log::log(0, "OfferAccuExist参数上载正常结束. 记录数: %d", m_poOfferAccuExistIndex->getCount());   
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
#ifdef EVENT_AGGR_RECYCLE_USE           
		try{          
        loadSpecialCycleType();
        Log::log(0, "SpecialCycleType参数上载正常结束. 记录数: %d", m_poCycleTypeIDIndex->getCount());   
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}   
#endif
		if(Needload("loadSplit"))
		{
			try{
        		loadSplit();  
        		Log::log(0, "Split参数上载正常结束. 记录数: %d", m_poSplitData->getCount());        
			} catch(TOCIException &e)
			{
				Log::log(0,"Split参数获取数据异常: %s",e.getErrMsg());
				return false;			
			}
		} 
		
		try{     
        	loadSpecialNbrStrategy();
        	Log::log(0, "SpecialNbrStrategy参数上载正常结束. 记录数: %d", m_poSpecialNbrStrategyIndex->getCount());         
		} catch(TOCIException &e)
		{
				Log::log(0,"SpecialNbrStrategy获取数据异常: %s",e.getErrMsg());
				return false;			
		}
		
		
		try{   
        	loadCombine();
        	Log::log(0, "组合参数上载正常结束. 记录数: %d", m_poPricingCombineData->getCount());   
		} catch(TOCIException &e)
		{
				Log::log(0,"组合参数获取数据异常: %s",e.getErrMsg());
				return false;			
		}
		
		
		try{
        	loadVirtualCombine();
        	Log::log(0, "Virtual组合参数上载正常结束. 记录数: %d", m_poVirtualPricingCombineData->getCount());   
		} catch(TOCIException &e)
		{
				Log::log(0,"Virtual组合获取数据异常: %s",e.getErrMsg());
				return false;			
		}
	
#ifdef GROUP_CONTROL
		try{
        	loadCGCombine();
        	Log::log(0, "CG组合参数上载正常结束. 记录数: %d", m_poCGPricingCombineData->getCount());   
		} catch(TOCIException &e)
		{
				Log::log(0,"CG组合参数获取数据异常: %s",e.getErrMsg());
				return false;			
		}
	
#endif
		try{
        loadCombineRelation();
        Log::log(0, "组合关系参数上载正常结束. 记录数: %d", m_poCombineRelationData->getCount());   
		} catch(TOCIException &e)
		{
				Log::log(0,"组合关系获取数据异常: %s",e.getErrMsg());
				return false;			
		}
	
	
		try{ 
        loadOfferCombine();
        Log::log(0, "OfferCombine参数上载正常结束. 记录数: %d", m_poOfferCombineIndex->getCount());   
		} catch(TOCIException &e)
		{
				Log::log(0,"OfferCombine 获取数据异常: %s",e.getErrMsg());
				return false;			
		}
	
#ifdef WHOLE_DISCOUNT
		try{
        loadWholeDiscount();
        Log::log(0, "WholeDiscount参数上载正常结束. 记录数: %d", m_poWholeDiscountData->getCount());   
		} catch(TOCIException &e)
		{
				Log::log(0,"WholeDiscount 获取数据异常: %s",e.getErrMsg());
				return false;			
		} 
#endif
		try{
        loadProductCombine();
        Log::log(0, "ProductCombine参数上载正常结束. 记录数: %d", m_poProductCombineIndex->getCount());
        } catch(TOCIException &e)
		{
				Log::log(0,"ProductCombine 获取数据异常: %s",e.getErrMsg());
				return false;			
		}
	
		try{
        loadHost();
        Log::log(0, "Host参数上载正常结束. 记录数: %d", m_poHostInfoData->getCount());  
		} catch(TOCIException &e)
		{
				Log::log(0,"Host参数获取数据异常: %s",e.getErrMsg());
				return false;			
		}
	
		try{
        loadGatherTask();
        Log::log(0, "GatherTask参数上载正常结束. 记录数: %d", m_poGatherTaskData->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
        //Log::log(0, "DisctApplyer参数上载正常结束. 记录数: %d", m_poDisctApplyerInfoData->getCount());
        //Log::log(0, "DisctApplyObject参数上载正常结束. 记录数: %d", m_poDisctApplyObjectInfoData->getCount());
		if(Needload("loadSpInfo"))
		{
			try{
        loadSpInfo();
        Log::log(0, "SpInfoData参数上载正常结束. 记录数: %d", m_poSpInfoData->getCount());
			} catch(TOCIException &e)
			{
				Log::log(0,"获取数据异常: %s",e.getErrMsg());
				return false;			
			}
		}
	
		try{
        loadGatherTaskListInfo();
        Log::log(0, "GatherTaskListInfo参数上载正常结束. 记录数: %d", m_poGatherTaskListInfoData->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		if(Needload("loadSegment"))
		{
        	try{
        loadSegment();
        Log::log(0, "SegmentInfo参数上载正常结束. 记录数: %d", m_poSegmentInfoData->getCount());
			} catch(TOCIException &e)
			{
				Log::log(0,"获取数据异常: %s",e.getErrMsg());
				return false;			
			}
		}
	
		try{
        loadLongTypeRule();
        Log::log(0, "LongType参数上载正常结束. 记录数: %d", m_poLongTypeIndex->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
        //TestCombine2(19331);
		try{
        loadCheckRule();
        Log::log(0, "CheckRuleData参数上载正常结束. 记录数: %d", m_poCheckRuleData->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	
		try{
        loadCheckSubRule();
        Log::log(0, "CheckSubRuleData参数上载正常结束. 记录数: %d", m_poCheckSubRuleData->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	
		try{
        loadArgument();
        Log::log(0, "CheckArgumentData参数上载正常结束. 记录数: %d", m_poCheckArgumentData->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	
		try{
        loadCheckRelation();
        Log::log(0, "CheckRelationData参数上载正常结束. 记录数: %d", m_poCheckRelationData->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	
		try{
        loadTransFile();
        Log::log(0, "TransFile参数上载正常结束. 记录数: %d", m_poTransFileTypeData->getCount());
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	
		try{
        loadParseRule();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	
		try{
        loadInstTableList();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	
		try{
        	loadNetworkPriority(); 
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	
		try{
        loadRentCfg(); 
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	
		try{
  			loadTrunkToBeNBR(); 
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	
		try{
        	loadCheckDup();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		try{
        loadProductorIDCfg();
        } catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		try{
        	loadLocalEventTypeRule();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
    //2010.11.17
	if(Needload("loadFilePool"))
	{
		try{
    		loadFilePoolInfo();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		try{
    		loadFilePoolCondition();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
	try{
    	loadFileTypeConfig();
	} catch(TOCIException &e)
	{
		Log::log(0,"获取数据异常: %s",e.getErrMsg());
		return false;			
	}
	if(Needload("loadTLVEncodeStruct"))
	{
		try{
    		loadTLVEncodeStruct();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
    //loadTLVDecodeStruct();
	if(Needload("loadBillingNodeInfo"))
	{
		try{
    		loadBillingNodeInfo();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
	
	try{
    	loadTLVParseStruct();
	} catch(TOCIException &e)
	{
		Log::log(0,"获取数据异常: %s",e.getErrMsg());
		return false;			
	}
		
	if(Needload("loadFileParseRule"))
	{
		try{
    		loadFileParseRule();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
	if(Needload("loadSwitchCode"))
	{
		try{
    		loadSwitchCode();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
    //2010.12.20
	if(Needload("loadCarrierInfo")){
		try{
    		loadCarrierInfo( );
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
	if(Needload("loadTariffInfo")){
		try{
    		loadTariffInfo();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
	try{
    loadSectorInfo();
	} catch(TOCIException &e)
	{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
	}
	if(Needload("loadCountryInfo"))
	{
		try{
    		loadCountryInfo();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
	if(Needload("loadSpecialGroupMember"))
	{
		try{
    		loadSpecialGroupMember();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
	
	if(Needload("loadSponsorInfo"))
	{
		try{
    	loadSponsorInfo();    
        } catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
	}
        //std
        //createStdData();
        //prep
		try{
        	loadTemplateMapSwitch();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        	loadObjectDefine();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		try{
        	loadEventParserGroup();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        	loadField();
        } catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        	loadSwitchInfo();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        	loadSwitchTypeInfo();
        } catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        	loadEventAttr();
        } catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        	loadEventHeadAndColunmCfg();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        	loadSwitchIDMapSwitchInfo();
        } catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        	loadFieldCheckMethod();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        	loadFieldValueMap();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        	loadFieldMerge();
        } catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        	loadTap3FileInfo();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
		
		try{
        	loadSepFile();
		} catch(TOCIException &e)
		{
			Log::log(0,"获取数据异常: %s",e.getErrMsg());
			return false;			
		}
        //合并功能
        //loadMergeRule();
	return true;
        Log::log(0,"参数上载共享内存正常结束[没有这个提示显示的,应该是刷新不成功的]!\n");
}

void ParamInfoMgr::empty()
{
#define EMPTYDATA(X) \
    if(X) \
    { \
        if(X->exist()) \
            X->reset(); \
    }

#define EMPTYINDEX(X) \
    if(X) \
    { \
        if(X->exist()) \
            X->empty(); \
    }

#define EMPTYSTRINGINDEX(X) \
    if(X) \
    { \
        if(X->exist()) \
            X->empty(); \
    }
    EMPTYDATA(m_poMServAcctInfoData);
    EMPTYINDEX(m_poMServAcctIndex);
    
    EMPTYDATA(m_poLogicStatementData);
    EMPTYINDEX(m_poLogicStatementIndex);
    EMPTYDATA(m_poAssignStatementData);
    EMPTYINDEX(m_poAssignStatementIndex);
    EMPTYDATA(m_poFormulaData);
    EMPTYINDEX(m_poFormulaIndex);

    EMPTYDATA(m_poFormatGroupData);
    EMPTYDATA(m_poFormatStepData);
    EMPTYDATA(m_poFormatRuleData);
    EMPTYDATA(m_poFormatCombData);

    EMPTYDATA(m_poTrunkData);
    EMPTYSTRINGINDEX(m_poTrunkIndex);

    EMPTYSTRINGINDEX(m_poAreaCodeIndex);

    EMPTYDATA(m_poHLRData);
    EMPTYSTRINGINDEX(m_poHLRIndex);    
	EMPTYDATA(m_poVisitAreaCodeRuleData);
	EMPTYSTRINGINDEX(m_pVisitAreaCodeIndex);
	EMPTYDATA(m_poEdgeRoamRuleData);
	EMPTYSTRINGINDEX(m_pEdgeRoamIndex);
    EMPTYSTRINGINDEX(m_poCarrierIndex);

	EMPTYDATA(m_poPNNotBillingData);
	EMPTYSTRINGINDEX(m_poPNNotBillingIndex);

	EMPTYDATA(m_aPNSegBillingData);
	EMPTYDATA(m_poPNSegBillingData);
	EMPTYDATA(m_poTrunkTreeNodeData);
	EMPTYSTRINGINDEX(m_pSwtichTrunkTree);
    //EMPTYDATA(m_poServiceData);
	EMPTYDATA(m_poServiceInfo2Data);
	EMPTYSTRINGINDEX(m_poServiceInfo2Index);

    EMPTYDATA(m_poSwitchData);
    EMPTYINDEX(m_poSwitchIndex);
    EMPTYSTRINGINDEX(m_poSwitchOPCIndex); 

    EMPTYDATA(m_poTrunkBillingData);
    EMPTYSTRINGINDEX(m_poTrunkBillingIndex);
	EMPTYINDEX(m_poSpecTrunkProductIndex);
	EMPTYDATA(m_poSwitchItemTypeData);
	EMPTYSTRINGINDEX(m_poSwitchItemTypeIndex);
	EMPTYDATA(m_poStrLocalAreaCodeData);
    EMPTYINDEX(m_poTimesTypeIndex);
	//
	EMPTYDATA(m_poFilterRuleResultData);
	EMPTYSTRINGINDEX(m_poRuleCheckCallingIndex);
	EMPTYSTRINGINDEX(m_poRuleCheckCalledIndex);
	EMPTYSTRINGINDEX(m_poRuleCheckAreaCodeIndex);
	EMPTYSTRINGINDEX(m_poRuleCheckGatherPotIndex);
	EMPTYSTRINGINDEX(m_poRuleCheckTrunkOutIndex);
	EMPTYSTRINGINDEX(m_poRuleCheckTrunkInIndex);
	EMPTYSTRINGINDEX(m_poRuleCheckSwitchIDIndex);

    EMPTYDATA(m_poCityInfoData);
    EMPTYSTRINGINDEX(m_poCityInfoIndex);
    EMPTYSTRINGINDEX(m_poCityAreaIndex);

    EMPTYDATA(m_poOrgData);
    EMPTYINDEX(m_poOrgIndex);
    EMPTYDATA(m_poOrgExData);
    EMPTYSTRINGINDEX(m_poOrgExIndex);
    EMPTYDATA(m_poOrgLongTypeData);
    EMPTYINDEX(m_poOrgLongTypeIndex);
    EMPTYDATA(m_poHeadExData);
    EMPTYSTRINGINDEX(m_poHeadExIndex);
    EMPTYDATA(m_poLocalHeadData);
    EMPTYSTRINGINDEX(m_poLocalHeadIndex);

    EMPTYDATA(m_poEventTypeData);
    EMPTYINDEX(m_poEventTypeIndex);
    EMPTYDATA(m_poEventTypeGroupData);
    EMPTYINDEX(m_poEventTypeGroupIndex);

    EMPTYDATA(m_poOfferData);
    EMPTYINDEX(m_poOfferIndex);
    EMPTYINDEX(m_poOfferSpecAttrIndex);
    EMPTYINDEX(m_poOfferNumAttrIndex);
    EMPTYINDEX(m_poOfferFreeIndex);

    EMPTYDATA(m_poNbrMapData);
    EMPTYSTRINGINDEX(m_poNbrMapType1Index);
    EMPTYSTRINGINDEX(m_poNbrMapType2Index);
    EMPTYSTRINGINDEX(m_poNbrMapType4Index);
    EMPTYSTRINGINDEX(m_poNbrMapType8Index);

    EMPTYDATA(m_poAcctItemData);
    EMPTYINDEX(m_poAcctItemIndex);
    EMPTYDATA(m_poAcctItemGroupData);
    EMPTYINDEX(m_poAcctItemGroupIndex);
    EMPTYDATA(m_poAcctItemGroupBData);
    EMPTYINDEX(m_poAcctItemGroupBIndex);
    EMPTYDATA(m_poAcctItemBasicGroupBData);
    EMPTYINDEX(m_poAcctItemBasicGroupBIndex);
    EMPTYDATA(m_poAcctItemDisctGroupBData);
    EMPTYINDEX(m_poAcctItemDisctGroupBIndex);
    EMPTYINDEX(m_poAcctItemIncludeDisctIndex);
	//
	EMPTYSTRINGINDEX(m_poAcctItemStrIndex);
	EMPTYSTRINGINDEX(m_poAcctItemGroupStrIndex);
	EMPTYSTRINGINDEX(m_poAcctItemGroupBStrIndex);
	//
    EMPTYDATA(m_poLongEventTypeRuleData);
    EMPTYDATA(m_poLongEventTypeRuleExData);

    EMPTYINDEX(m_poPricingPlanTypeIndex);

    EMPTYDATA(m_poStateConvertData);
    EMPTYINDEX(m_poStateConvertIndex);

    EMPTYDATA(m_poValueMapData);
    EMPTYSTRINGINDEX(m_poValueMapIndex);

    EMPTYDATA(m_poZoneItemData);
    EMPTYINDEX(m_poZoneItemIndex);
        EMPTYSTRINGINDEX(m_poZoneItemStrIndex);

    EMPTYDATA(m_poSplitAcctItemGroupData);
    EMPTYINDEX(m_poSplitAcctItemGroupIndex);
    EMPTYDATA(m_poSplitAcctGroupEventData);
    EMPTYINDEX(m_poSplitAcctGroupEventIndex);

    EMPTYDATA(m_poDistanceTypeData);
    EMPTYINDEX(m_poDistanceTypeIndex);

    EMPTYDATA(m_poEdgeRoamRuleExData);
    EMPTYSTRINGINDEX(m_poEdgeRoamRuleExTypeAIndex);
    EMPTYSTRINGINDEX(m_poEdgeRoamRuleExTypeBIndex);
    EMPTYSTRINGINDEX(m_poEdgeRoamRuleExMSCIndex);
    
        EMPTYDATA(m_poPricingSectionInfoData);
    EMPTYINDEX(m_poSectionIndex);
    EMPTYINDEX(m_poStrategyIndex);
        
        EMPTYDATA(m_poTariffInfoData);
    EMPTYINDEX(m_poTariffIndex); 
        
        EMPTYDATA(m_poDiscountInfoData);
        EMPTYINDEX(m_poDiscountIndex);
        
        EMPTYDATA(m_poDisctApplyerInfoData);
        EMPTYINDEX(m_poDisctApplyerIndex);
        
        EMPTYDATA(m_poDisctApplyObjectInfoData);
        EMPTYINDEX(m_poDisctApplyObjectIndex);  
        
        EMPTYDATA(m_poPercentInfoData);
        EMPTYINDEX(m_poPercentIndex);

        EMPTYDATA(m_poLifeCycleData);
        EMPTYINDEX(m_poLifeCycleIndex); 
    
        EMPTYDATA(m_poPricingMeasureData);
        EMPTYINDEX(m_poPricingMeasureIndex);
        EMPTYINDEX(m_poStrategyIDMeasureIndex); 
        
        EMPTYDATA(m_poPricingCombineData);
        EMPTYINDEX(m_poPricingCombineIndex);
        EMPTYINDEX(m_poPlanIDIndex);    
        EMPTYINDEX(m_poStrategyCombineIndex);
        
        EMPTYDATA(m_poFilterConditionData);
        EMPTYINDEX(m_poFilterConditionIndex); 
        
        EMPTYDATA(m_poConditionPointData);
        EMPTYDATA(m_poTimeCondData);
        EMPTYINDEX(m_poTimeCondIndex);
        
        EMPTYDATA(m_poEventTypeCondData);
        EMPTYINDEX(m_poEventTypeCondIndex);
        
        EMPTYDATA(m_poCycleTypeData);
        EMPTYINDEX(m_poCycleTypeIndex);
        
        EMPTYDATA(m_poAccuTypeData);
        EMPTYINDEX(m_poAccuTypeIndex);
        
        EMPTYDATA(m_poOfferAccuRelationData);
        EMPTYINDEX(m_poOfferAccuRelationIndex);
        
        EMPTYSTRINGINDEX(m_poOfferAccuExistIndex);
        
        EMPTYDATA(m_poUsageTypeInfoData);
        EMPTYINDEX(m_poUsageTypeIndex);
        
        EMPTYDATA(m_poVirtualPricingCombineData);
        EMPTYINDEX(m_poVirtualStrategyCombineIndex);    
    
#ifdef GROUP_CONTROL
        EMPTYDATA(m_poCGPricingCombineData);
        EMPTYINDEX(m_poCGStrategyCombineIndex);
#endif
    
#ifdef WHOLE_DISCOUNT
        EMPTYDATA(m_poWholeDiscountData);
#endif
    
    EMPTYDATA(m_poCombineRelationData);
        EMPTYINDEX(m_poCombineRelationIndex);

#ifdef EVENT_AGGR_RECYCLE_USE
		EMPTYDATA(m_poCycleTypeIDData);
        EMPTYINDEX(m_poCycleTypeIDIndex);
#endif
        
        EMPTYDATA(m_poSplitData);
    EMPTYINDEX(m_poSplitIndex);
        
        EMPTYDATA(m_poSpecialNbrStrategyData);
        EMPTYINDEX(m_poSpecialNbrStrategyIndex);
        EMPTYSTRINGINDEX(m_poSpecialNbrOfferIndex);
        
        EMPTYINDEX(m_poOfferCombineIndex);
        EMPTYINDEX(m_poOfferDisctIndex);

        EMPTYINDEX(m_poProductCombineIndex);
        
        EMPTYDATA(m_poHostInfoData);
        EMPTYINDEX(m_poHostInfoIndex);
        
        EMPTYDATA(m_poGatherTaskData);
        EMPTYINDEX(m_poGatherTaskIndex);
        EMPTYINDEX(m_poOrgGatherTaskIndex);
        
        EMPTYINDEX(m_poProductIDIndex);
        EMPTYSTRINGINDEX(m_poProIDAcctItemStringIndex);
        
        EMPTYDATA(m_poSpecialTariffData);
        EMPTYSTRINGINDEX(m_poSpecialTariffIndex);
        
        EMPTYDATA(m_poSpecialTariffCheckData);
        EMPTYSTRINGINDEX(m_poSpecialTariffCheckIndex);
        
        EMPTYDATA(m_poSpInfoData);
        EMPTYSTRINGINDEX(m_pHSpInfoIndex);
        
        EMPTYDATA(m_poGatherTaskListInfoData);
    EMPTYINDEX(m_poGatherTaskListInfoIndex);
        
        EMPTYDATA(m_poSegmentInfoData);
        EMPTYINDEX(m_poSegmentIndex);
        EMPTYINDEX(m_poSegmentTypeIndex);

        EMPTYINDEX(m_poLongTypeIndex);
        
        EMPTYDATA(m_poCheckRuleData);
        EMPTYINDEX(m_poCheckRuleIndex);
        EMPTYDATA(m_poCheckSubRuleData);
        EMPTYINDEX(m_poCheckSubRuleIndex);
        EMPTYDATA(m_poCheckArgumentData);
        EMPTYDATA(m_poCheckRelationData);
        EMPTYDATA(m_poTransFileTypeData);
        EMPTYINDEX(m_poTransFileTypeIndex);
        EMPTYDATA(m_poParseRuleData);
        EMPTYSTRINGINDEX(m_poParseRuleIndex);
        EMPTYDATA(m_poParseCombData);
        EMPTYINDEX(m_poParseCombIndex);
        EMPTYDATA(m_poInstTableData);
        EMPTYSTRINGINDEX(m_poInstTableIndex);
        EMPTYDATA(m_poTableTypeData);
        EMPTYINDEX(m_poTableTypeIndex);
        EMPTYINDEX(m_poRentIndex);
        EMPTYDATA(m_poNetworkPriorData);
        EMPTYINDEX(m_poNetworkPriorIndex);
        EMPTYDATA(m_poTrunkToBeNBRData);
        EMPTYINDEX(m_poTrunkToBeNBRIndex);
        
        EMPTYDATA(m_poCheckDupCfgData);
        EMPTYINDEX(m_poCheckDupCfgIndex);
        EMPTYDATA(m_poCheckDupTypeData);
        EMPTYINDEX(m_poCheckDupTypeIndex);
        EMPTYDATA(m_poCheckDupStepData);
        EMPTYINDEX(m_poCheckDupStepIndex);
        EMPTYDATA(m_poShotTicketData);
        EMPTYINDEX(m_poShotTicketIndex);
        EMPTYDATA(m_poUndoProcessData);
        EMPTYINDEX(m_poUndoProcessIndex);
        EMPTYDATA(m_poUndoProcShotData);
        EMPTYINDEX(m_poUndoProcShotIndex);
        EMPTYDATA(m_poUndoServData);
        EMPTYDATA(m_poUndoProductData); 
        EMPTYINDEX(m_poProductIndex);
        EMPTYDATA(m_poOrgPathData);
        EMPTYINDEX(m_poOrgPathIndex);
        EMPTYDATA(m_poLocalEventTypeRuleData);
        EMPTYINDEX(m_poLocalEventTypeRuleIndex);

    EMPTYDATA(m_poFilePoolInfoData);
    EMPTYSTRINGINDEX(m_poFilePoolInfoIndex);
    EMPTYDATA(m_poFilePoolCondData);
    EMPTYSTRINGINDEX(m_poFilePoolCondIndex);
    EMPTYSTRINGINDEX(m_poFileTypeIndex);
    EMPTYDATA(m_poTLVEncodeData);
    EMPTYINDEX(m_poTLVEncodeIndex);
    EMPTYDATA(m_poTLVDecodeData);
    EMPTYSTRINGINDEX(m_poTLVDecodeIndex);
    EMPTYSTRINGINDEX(m_poFileParseRuleIndex);
    EMPTYDATA(m_poSwitchCodeData);
    EMPTYSTRINGINDEX(m_poSwitchCodeIndex);
    EMPTYDATA(m_poBillingNodeData);
    EMPTYSTRINGINDEX(m_poBillingNodeIndex);
    //2010.12.20 
    EMPTYDATA(m_poRoamTariffData);
    EMPTYINDEX(m_poRoamTariffIndex);
    EMPTYDATA(m_poRoamCarrierData);
    EMPTYSTRINGINDEX(m_poRoamCarrierIndex);
    EMPTYSTRINGINDEX(m_poTariffSectorIndex);
    EMPTYDATA(m_poCountryData);
    EMPTYSTRINGINDEX(m_poCountryCodeIndex);
    EMPTYSTRINGINDEX(m_poCountryAreaIndex);
    EMPTYDATA(m_poSponsorData);
    EMPTYSTRINGINDEX(m_poSponsorIndex);
    EMPTYDATA(m_poMemberData);
    EMPTYINDEX(m_poMemberIndex);
        //prep
        EMPTYDATA(m_poTemplateMapSwitchData); 
        EMPTYINDEX(m_poTemplateMapSwitchIndex);
        EMPTYDATA(m_poObjectDefineData); 
        EMPTYINDEX(m_poObjectDefineIndex);
        EMPTYDATA(m_poEventParserGroupData); 
        EMPTYINDEX(m_poEventParserGroupIndex);
        
        EMPTYDATA(m_poTemplateRelationData);
        EMPTYINDEX(m_poTemplateRelationIndex);
        EMPTYDATA(m_poFieldData);
        EMPTYINDEX(m_poFieldTemplateIndex);
        EMPTYSTRINGINDEX(m_poFieldUnionStrIndex);
        //EMPTYDATA(m_poSwitchInfoData);
        //EMPTYINDEX(m_poSwitchInfoIndex);
        EMPTYDATA(m_poMapSwitchTypeData);
        EMPTYINDEX(m_poMapSwitchTypeIndex);
        /*
        EMPTYDATA(m_poObjectType6Data);
        EMPTYINDEX(m_poObjectType6Index);
        EMPTYDATA(m_poSepEPGData);
        EMPTYINDEX(m_poSepEPGIndex);*/
        
        EMPTYDATA(m_poEventAttrData);
        EMPTYINDEX(m_poEventAttrIndex);
        
        //EMPTYDATA(m_poFileInfoData);
        //EMPTYINDEX(m_poFileInfoIndex);
        
        EMPTYDATA(m_poEventHeadAndColumnsCfg);
        EMPTYINDEX(m_poEventHeadAndColumnsIndex);
        EMPTYDATA(m_poSwitchIDMapSwitchData);
        EMPTYINDEX(m_poSwitchIDMapSwitchIndex);
        
        EMPTYDATA(m_poFieldCheckMethodData);
        EMPTYINDEX(m_poFieldCheckMethodIndex);
        EMPTYDATA(m_poFieldMapData);
        EMPTYINDEX(m_poFieldMapIndex);
        EMPTYDATA(m_poFieldMergeData);
        EMPTYINDEX(m_poFieldMergeIndex);
        EMPTYDATA(m_poTap3FileData);
        EMPTYDATA(m_poTap3BlockInfoData);
        
        EMPTYDATA(m_poSepFileData);
        EMPTYDATA(m_poSepBlockInfoData);
        //
        EMPTYDATA(m_poMergeRuleData);
		EMPTYDATA(m_poMergeRuleAssociateData);
        EMPTYSTRINGINDEX(m_poMergeRuleStrIndex);
		EMPTYSTRINGINDEX(m_poMergeRuleAssociateStrIndex);
        
        EMPTYDATA(m_poMergeRuleOutData);
        EMPTYINDEX(m_poMergeRuleOutIndex);
    
        EMPTYDATA(m_poMergeKeyRuleData);
        EMPTYINDEX(m_poMergeKeyRuleIndex);


}

int ParamInfoMgr::getConnnum()
{
	if(m_poLogicStatementData && m_poLogicStatementData->exist())
	{
		return m_poLogicStatementData->connnum();
	}
	return 0; 
}
//
void ParamInfoMgr::unload()
{
#define UNLOAD(X) \
    if(X) \
    if (X->exist()) { \
            if(X->connnum() > 1) \
                { \
                    THROW(MBC_ParamShmMgr+6); \
                } \
            X->remove (); \
    }

	UNLOAD(m_poMServAcctInfoData);
    UNLOAD(m_poMServAcctIndex);
    UNLOAD(m_poLogicStatementData);
    UNLOAD(m_poLogicStatementIndex);
    UNLOAD(m_poAssignStatementData);
    UNLOAD(m_poAssignStatementIndex);
    UNLOAD(m_poFormulaData);
    UNLOAD(m_poFormulaIndex);

    UNLOAD(m_poFormatGroupData);
    UNLOAD(m_poFormatStepData);
    UNLOAD(m_poFormatRuleData);
    UNLOAD(m_poFormatCombData);

    UNLOAD(m_poTrunkData);
    UNLOAD(m_poTrunkIndex);

    UNLOAD(m_poAreaCodeIndex);

    UNLOAD(m_poHLRData);
    UNLOAD(m_poHLRIndex);    

	UNLOAD(m_poVisitAreaCodeRuleData);
	UNLOAD(m_pVisitAreaCodeIndex);
	UNLOAD(m_poEdgeRoamRuleData);
	UNLOAD(m_pEdgeRoamIndex);
    UNLOAD(m_poCarrierIndex);
	//
	UNLOAD(m_poPNNotBillingData);
	UNLOAD(m_poPNNotBillingIndex);
	//
	UNLOAD(m_aPNSegBillingData);
	UNLOAD(m_poPNSegBillingData);
	UNLOAD(m_poTrunkTreeNodeData);
	UNLOAD(m_pSwtichTrunkTree);

    //UNLOAD(m_poServiceData);
	UNLOAD(m_poServiceInfo2Data);
	UNLOAD(m_poServiceInfo2Index);

    UNLOAD(m_poSwitchData);
    UNLOAD(m_poSwitchIndex);
    UNLOAD(m_poSwitchOPCIndex);
    UNLOAD(m_poTrunkBillingData);
    UNLOAD(m_poTrunkBillingIndex);
	UNLOAD(m_poSpecTrunkProductIndex);
	UNLOAD(m_poSwitchItemTypeData);
	UNLOAD(m_poSwitchItemTypeIndex);
	UNLOAD(m_poStrLocalAreaCodeData);
    UNLOAD(m_poTimesTypeIndex);
	//
	UNLOAD(m_poFilterRuleResultData);
	UNLOAD(m_poRuleCheckCallingIndex);
	UNLOAD(m_poRuleCheckCalledIndex);
	UNLOAD(m_poRuleCheckAreaCodeIndex);
	UNLOAD(m_poRuleCheckGatherPotIndex);
	UNLOAD(m_poRuleCheckTrunkOutIndex);
	UNLOAD(m_poRuleCheckTrunkInIndex);
	UNLOAD(m_poRuleCheckSwitchIDIndex);

    UNLOAD(m_poCityInfoData);
    UNLOAD(m_poCityInfoIndex);
    UNLOAD(m_poCityAreaIndex);

    UNLOAD(m_poOrgData);
    UNLOAD(m_poOrgIndex);
    UNLOAD(m_poOrgExData);
    UNLOAD(m_poOrgExIndex);
    UNLOAD(m_poOrgLongTypeData);
    UNLOAD(m_poOrgLongTypeIndex);
    UNLOAD(m_poHeadExData);
    UNLOAD(m_poHeadExIndex);
    UNLOAD(m_poLocalHeadData);
    UNLOAD(m_poLocalHeadIndex);

    UNLOAD(m_poEventTypeData);
    UNLOAD(m_poEventTypeIndex);
    UNLOAD(m_poEventTypeGroupData);
    UNLOAD(m_poEventTypeGroupIndex);

    UNLOAD(m_poOfferData);
    UNLOAD(m_poOfferIndex);
    UNLOAD(m_poOfferSpecAttrIndex);
    UNLOAD(m_poOfferNumAttrIndex);
    UNLOAD(m_poOfferFreeIndex);

    UNLOAD(m_poNbrMapData);
    UNLOAD(m_poNbrMapType1Index);
    UNLOAD(m_poNbrMapType2Index);
    UNLOAD(m_poNbrMapType4Index);
    UNLOAD(m_poNbrMapType8Index);

    UNLOAD(m_poAcctItemData);
    UNLOAD(m_poAcctItemIndex);
    UNLOAD(m_poAcctItemGroupData);
    UNLOAD(m_poAcctItemGroupIndex);
    UNLOAD(m_poAcctItemGroupBData);
    UNLOAD(m_poAcctItemGroupBIndex);
    UNLOAD(m_poAcctItemBasicGroupBData);
    UNLOAD(m_poAcctItemBasicGroupBIndex);
    UNLOAD(m_poAcctItemDisctGroupBData);
    UNLOAD(m_poAcctItemDisctGroupBIndex);
    UNLOAD(m_poAcctItemIncludeDisctIndex);
	//
	UNLOAD(m_poAcctItemStrIndex);
	UNLOAD(m_poAcctItemGroupStrIndex);
	UNLOAD(m_poAcctItemGroupBStrIndex);
	//
    UNLOAD(m_poLongEventTypeRuleData);
    UNLOAD(m_poLongEventTypeRuleExData);

    UNLOAD(m_poPricingPlanTypeIndex);

    UNLOAD(m_poStateConvertData);
    UNLOAD(m_poStateConvertIndex);

    UNLOAD(m_poValueMapData);
    UNLOAD(m_poValueMapIndex);

    UNLOAD(m_poZoneItemData);
    UNLOAD(m_poZoneItemIndex);
        UNLOAD(m_poZoneItemStrIndex);
        
    UNLOAD(m_poSplitAcctItemGroupData);
    UNLOAD(m_poSplitAcctItemGroupIndex);
    UNLOAD(m_poSplitAcctGroupEventData);
    UNLOAD(m_poSplitAcctGroupEventIndex);

    UNLOAD(m_poDistanceTypeData);
    UNLOAD(m_poDistanceTypeIndex);

    UNLOAD(m_poEdgeRoamRuleExData);
    UNLOAD(m_poEdgeRoamRuleExTypeAIndex);
    UNLOAD(m_poEdgeRoamRuleExTypeBIndex);
    UNLOAD(m_poEdgeRoamRuleExMSCIndex);
    
        UNLOAD(m_poPricingSectionInfoData);
    UNLOAD(m_poSectionIndex);
        UNLOAD(m_poStrategyIndex);
    
        UNLOAD(m_poTariffInfoData);
        UNLOAD(m_poTariffIndex);
        
        UNLOAD(m_poDiscountInfoData);
        UNLOAD(m_poDiscountIndex);
                
        UNLOAD(m_poDisctApplyerInfoData);
        UNLOAD(m_poDisctApplyerIndex);
                
        UNLOAD(m_poDisctApplyObjectInfoData);
        UNLOAD(m_poDisctApplyObjectIndex);
        
        UNLOAD(m_poPercentInfoData);
        UNLOAD(m_poPercentIndex);
        
        UNLOAD(m_poLifeCycleData);
        UNLOAD(m_poLifeCycleIndex);
        
        UNLOAD(m_poPricingMeasureData);
        UNLOAD(m_poPricingMeasureIndex);
        UNLOAD(m_poStrategyIDMeasureIndex);
        
        UNLOAD(m_poPricingCombineData);
    UNLOAD(m_poPlanIDIndex);
        UNLOAD(m_poPricingCombineIndex);
        UNLOAD(m_poStrategyCombineIndex);
        
        UNLOAD(m_poFilterConditionData);
        UNLOAD(m_poFilterConditionIndex);
        
        UNLOAD(m_poConditionPointData);
        UNLOAD(m_poTimeCondData);
        UNLOAD(m_poTimeCondIndex);
        
        UNLOAD(m_poEventTypeCondData);
        UNLOAD(m_poEventTypeCondIndex);
        
        UNLOAD(m_poCycleTypeData);
        UNLOAD(m_poCycleTypeIndex);
        
        UNLOAD(m_poAccuTypeData);
        UNLOAD(m_poAccuTypeIndex);
        
        UNLOAD(m_poOfferAccuRelationData);
        UNLOAD(m_poOfferAccuRelationIndex);
        
        UNLOAD(m_poOfferAccuExistIndex);
        
#ifdef EVENT_AGGR_RECYCLE_USE
		UNLOAD(m_poCycleTypeIDData);
        UNLOAD(m_poCycleTypeIDIndex);
#endif
        
        UNLOAD(m_poVirtualPricingCombineData);
        UNLOAD(m_poVirtualStrategyCombineIndex);
        
#ifdef GROUP_CONTROL
        UNLOAD(m_poCGPricingCombineData);
        UNLOAD(m_poCGStrategyCombineIndex);
#endif  
        UNLOAD(m_poOfferCombineIndex);  
        UNLOAD(m_poOfferDisctIndex);
#ifdef WHOLE_DISCOUNT
        UNLOAD(m_poWholeDiscountData);
#endif
        UNLOAD(m_poProductCombineIndex);        
        UNLOAD(m_poCombineRelationData);
        UNLOAD(m_poCombineRelationIndex);
        
        UNLOAD(m_poUsageTypeInfoData);
        UNLOAD(m_poUsageTypeIndex);
        
//#ifdef EVENT_AGGR_RECYCLE_USE
//      UNLOAD(m_poCycleTypeIDIndex);
//#endif
        
        UNLOAD(m_poSplitData);
        UNLOAD(m_poSplitIndex);//xgs
        
        UNLOAD(m_poTableTypeIndex);//xgs
        UNLOAD(m_poTableTypeData);//xgs
        
        UNLOAD(m_poSpecialNbrStrategyData);
        UNLOAD(m_poSpecialNbrStrategyIndex);
        UNLOAD(m_poSpecialNbrOfferIndex);
        
        UNLOAD(m_poHostInfoData);
        UNLOAD(m_poHostInfoIndex);
        
        UNLOAD(m_poGatherTaskData);
        UNLOAD(m_poGatherTaskIndex);
        UNLOAD(m_poOrgGatherTaskIndex);
        
        UNLOAD(m_poProductIDIndex);
        UNLOAD(m_poProIDAcctItemStringIndex);
        
        UNLOAD(m_poSpecialTariffData);
        UNLOAD(m_poSpecialTariffIndex);
        
        UNLOAD(m_poSpecialTariffCheckData);
        UNLOAD(m_poSpecialTariffCheckIndex);
        
        UNLOAD(m_poSpInfoData);
        UNLOAD(m_pHSpInfoIndex);
        
        UNLOAD(m_poGatherTaskListInfoData);
        UNLOAD(m_poGatherTaskListInfoIndex);
        
        UNLOAD(m_poSegmentInfoData);
        UNLOAD(m_poSegmentIndex);
        UNLOAD(m_poSegmentTypeIndex);
        
        UNLOAD(m_poLongTypeIndex);
        
        UNLOAD(m_poCheckRuleData);
        UNLOAD(m_poCheckRuleIndex);
        UNLOAD(m_poCheckSubRuleData);
        UNLOAD(m_poCheckSubRuleIndex);
        UNLOAD(m_poCheckArgumentData);
        UNLOAD(m_poCheckRelationData);
        UNLOAD(m_poTransFileTypeData);
        UNLOAD(m_poTransFileTypeIndex);
        UNLOAD(m_poParseRuleData);
        UNLOAD(m_poParseRuleIndex);
        UNLOAD(m_poParseCombData);
        UNLOAD(m_poParseCombIndex);
        UNLOAD(m_poInstTableData);
        UNLOAD(m_poInstTableIndex);
        UNLOAD(m_poNetworkPriorData);
        UNLOAD(m_poNetworkPriorIndex);
        UNLOAD(m_poRentIndex);
        UNLOAD(m_poTrunkToBeNBRData);
        UNLOAD(m_poTrunkToBeNBRIndex);
        
                UNLOAD(m_poCheckDupCfgData);
        UNLOAD(m_poCheckDupCfgIndex);
        UNLOAD(m_poCheckDupTypeData);
        UNLOAD(m_poCheckDupTypeIndex);
        UNLOAD(m_poCheckDupStepData);
        UNLOAD(m_poCheckDupStepIndex);
        UNLOAD(m_poShotTicketData);
        UNLOAD(m_poShotTicketIndex);
        UNLOAD(m_poUndoProcessData);
        UNLOAD(m_poUndoProcessIndex);
        UNLOAD(m_poUndoProcShotData);
        UNLOAD(m_poUndoProcShotIndex);
        UNLOAD(m_poUndoServData);
        UNLOAD(m_poUndoProductData); 
        UNLOAD(m_poProductIndex);
        
        UNLOAD(m_poOrgPathData);
        UNLOAD(m_poOrgPathIndex);
        UNLOAD(m_poLocalEventTypeRuleData);
        UNLOAD(m_poLocalEventTypeRuleIndex);

    //2010.11.17
    UNLOAD(m_poFilePoolInfoData);
    UNLOAD(m_poFilePoolInfoIndex);
    UNLOAD(m_poFilePoolCondData);
    UNLOAD(m_poFilePoolCondIndex);
    UNLOAD(m_poFileTypeIndex);
    UNLOAD(m_poTLVEncodeData);
    UNLOAD(m_poTLVEncodeIndex);
    UNLOAD(m_poTLVDecodeData);
    UNLOAD(m_poTLVDecodeIndex);
    UNLOAD(m_poFileParseRuleIndex);
    UNLOAD(m_poSwitchCodeData);
    UNLOAD(m_poSwitchCodeIndex);
    UNLOAD(m_poBillingNodeData);
    UNLOAD(m_poBillingNodeIndex);
    //2010.12.20 
    UNLOAD(m_poRoamTariffData);
    UNLOAD(m_poRoamTariffIndex);
    UNLOAD(m_poRoamCarrierData);
    UNLOAD(m_poRoamCarrierIndex);
    UNLOAD(m_poTariffSectorIndex);
    UNLOAD(m_poCountryData);
    UNLOAD(m_poCountryCodeIndex);
    UNLOAD(m_poCountryAreaIndex);
    UNLOAD(m_poSponsorData);
    UNLOAD(m_poSponsorIndex);
    UNLOAD(m_poMemberData);
    UNLOAD(m_poMemberIndex);
        //std
        /*UNLOAD(m_poStdChar8Data);
        UNLOAD(m_poStdChar8Index);
        UNLOAD(m_poStdChar8DataDesc);*/
        //prep
        UNLOAD(m_poTemplateMapSwitchData); 
        UNLOAD(m_poTemplateMapSwitchIndex);
        UNLOAD(m_poObjectDefineData); 
        UNLOAD(m_poObjectDefineIndex);
        UNLOAD(m_poEventParserGroupData); 
        UNLOAD(m_poEventParserGroupIndex);
        
        UNLOAD(m_poTemplateRelationData);
        UNLOAD(m_poTemplateRelationIndex);
        UNLOAD(m_poFieldData);
        UNLOAD(m_poFieldTemplateIndex);
        UNLOAD(m_poFieldUnionStrIndex);
        //UNLOAD(m_poSwitchInfoData);
        //UNLOAD(m_poSwitchInfoIndex);
        UNLOAD(m_poMapSwitchTypeData);
        UNLOAD(m_poMapSwitchTypeIndex);
        /*
        UNLOAD(m_poObjectType6Data);
        UNLOAD(m_poObjectType6Index);
        UNLOAD(m_poSepEPGData);
        UNLOAD(m_poSepEPGIndex);
        */
        UNLOAD(m_poEventAttrData);
        UNLOAD(m_poEventAttrIndex);
        
        //UNLOAD(m_poFileInfoData);
        //UNLOAD(m_poFileInfoIndex);

        
        UNLOAD(m_poEventHeadAndColumnsCfg);
        UNLOAD(m_poEventHeadAndColumnsIndex);
        UNLOAD(m_poSwitchIDMapSwitchData);
        UNLOAD(m_poSwitchIDMapSwitchIndex);
        UNLOAD(m_poFieldCheckMethodData);
        UNLOAD(m_poFieldCheckMethodIndex);
        UNLOAD(m_poFieldMapData);
        UNLOAD(m_poFieldMapIndex);
        UNLOAD(m_poFieldMergeData);
        UNLOAD(m_poFieldMergeIndex);
        
        UNLOAD(m_poTap3FileData);
        UNLOAD(m_poTap3BlockInfoData);

        
        UNLOAD(m_poSepFileData);
        UNLOAD(m_poSepBlockInfoData);
        
        UNLOAD(m_poMergeRuleData);
		UNLOAD(m_poMergeRuleAssociateData);
        UNLOAD(m_poMergeRuleStrIndex);
		UNLOAD(m_poMergeRuleAssociateStrIndex);
        
        UNLOAD(m_poMergeRuleOutData);
        UNLOAD(m_poMergeRuleOutIndex);
        
        UNLOAD(m_poMergeKeyRuleData);
        UNLOAD(m_poMergeKeyRuleIndex);

    m_bAttached = false;
}


void ParamInfoMgr::loadLogicStatement()
{
    int iCount = 0;
    LogicStatement *pLogicStatement = 0;
    char sSqlcode[512] = {0};
    TOCIQuery qry(Environment::getDBConn());
                Log::log(0,"开始条件参数上载...");      
    strcpy(sSqlcode, "SELECT A.CONDITION_ID CONDITION_ID,"
                " B.STATEMENT_SEQ STATEMENT_SEQ,"
                " NVL(B.GROUP_ID,0) GROUP_ID,"
                " NVL(B.PARAM_FUNCTION,-1) FUNCTION_ID,"
                " NVL(B.PARAM1_ID,'') PARAM1,"
                " NVL(B.PARAM2_ID,'') PARAM2,"
                " NVL(B.PARAM3_ID,'') PARAM3,"
                " B.OPERATOR OPERATOR,"
                " B.CONDITION_VAL_ID DES_PARAM"
                " FROM BILL_CONDITION A, LOGIC_STATEMENT B"
                " WHERE A.CONDITION_ID = B.CONDITION_ID"
                " ORDER BY B.CONDITION_ID, B.GROUP_ID, B.STATEMENT_SEQ");

    
    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_LOGICSTATEMENT_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+1);
    }
    iCount = atoi(countbuf);

    if(!(m_poLogicStatementData->exist()))
        m_poLogicStatementData->create(iCount * m_fRatio);
    if(!(m_poLogicStatementIndex->exist()))
        m_poLogicStatementIndex->create(iCount * m_fRatio);

    pLogicStatement = (LogicStatement *)(*m_poLogicStatementData);
	checkMem(m_poLogicStatementData->getCount() * sizeof(LogicStatement)); 
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poLogicStatementData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+2);
            
                        pLogicStatement[i].m_iStatementID = -1;
        pLogicStatement[i].m_iFunctionID = -1;
        pLogicStatement[i].m_iOperatorID = -1;

        pLogicStatement[i].m_bParsed = false;

        pLogicStatement[i].m_poFormula = NULL;
    
                static bool flag = true;
                if(flag) 
                {
                char str[32];
                memset(str,0,sizeof(str));
                if(ParamDefineMgr::getParam("PRICING","DISCT_PERCENT_ATTR_ID",str))
                 {
                        pLogicStatement[i].m_oDisctPercentAttrID = ID(str);
                 }
     
                 memset(str,0,sizeof(str));
                if(ParamDefineMgr::getParam("PRICING","DISCT_PERCENT_ATTR_VAL",str))
                {
                        pLogicStatement[i].m_iDisctPercentAttrValue = atoi(str);
                }
                flag = false;   
                } 


        pLogicStatement[i].m_iConditionID = qry.field (0).asInteger ();
        pLogicStatement[i].m_iStatementSeq = qry.field (1).asInteger ();
        pLogicStatement[i].m_iGroupID = qry.field (2).asInteger ();
        pLogicStatement[i].m_iStatementID = pLogicStatement[i].m_iConditionID;

        pLogicStatement[i].setFunction (qry.field (3).asInteger());
        pLogicStatement[i].setOperator (qry.field (7).asInteger());
        pLogicStatement[i].setParam1 (qry.field (4).asString());
        pLogicStatement[i].setParam2 (qry.field (5).asString());
        pLogicStatement[i].setParam3 (qry.field (6).asString());
        pLogicStatement[i].setDesParam (qry.field (8).asString());
        pLogicStatement[i].m_poNext = NULL;
        pLogicStatement[i].m_iNextOffset= 0;

        //设置相同ConditionID的链表
        if(i == 1)
        {
            m_poLogicStatementIndex->add((long)pLogicStatement[i].m_iConditionID, i);
        }
        else
        {
            if(pLogicStatement[i].m_iConditionID == pLogicStatement[i-1].m_iConditionID)
            {
                pLogicStatement[i-1].m_iNextOffset= i;
            }
            else
                m_poLogicStatementIndex->add((long)pLogicStatement[i].m_iConditionID, i);
        }
    }
    
    qry.close();
    Log::log(0, "条件参数上载正常结束. 记录数: %d", m_poLogicStatementData->getCount());
    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poLogicStatementData->getCount(),m_poLogicStatementData->getTotal());
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poLogicStatementIndex->getCount(),m_poLogicStatementIndex->getTotal());
	Date mDate;
	//updateSHMInfo(m_poLogicStatementData->getKey(),"BILL_CONDITION LOGIC_STATEMENT",m_poLogicStatementData->getCount(),m_poLogicStatementData->getTotal(),mDate.toString());
	//updateSHMInfo(m_poLogicStatementIndex->getKey(),"BILL_CONDITION LOGIC_STATEMENT",m_poLogicStatementIndex->getCount(),m_poLogicStatementIndex->getTotal(),mDate.toString());
}


void ParamInfoMgr::loadAssignStatement()
{
    int iCount = 0;
    AssignStatement *pAssignStatement = 0;
    char sSqlcode[512] = {0};
    TOCIQuery qry(Environment::getDBConn());
    Log::log(0, "开始运算参数上载");
    sprintf (sSqlcode, "SELECT A.OPERATION_ID OPERATION_ID,"
                " NVL(B.STATEMENT_SEQ,0) STATEMENT_SEQ,"
                " NVL(B.FUNCTION,-1) FUNCTION_ID,"
                " NVL(B.PARAM1_ID,'') PARAM1,"
                " NVL(B.PARAM2_ID,'') PARAM2,"
                " NVL(B.PARAM3_ID,'') PARAM3,"
                " B.RESULT_NAME DES_PARAM"
                " FROM BILL_OPERATION A, ASSIGN_STATEMENT B"
                " WHERE A.OPERATION_ID = B.OPERATION_ID"
                " ORDER BY B.OPERATION_ID, B.STATEMENT_SEQ, B.STATEMENT_SEQ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ASSIGNSTATEMENT_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+3);
    }
    iCount = atoi(countbuf);

    if(!(m_poAssignStatementData->exist()))
        m_poAssignStatementData->create(iCount * m_fRatio);
    if(!(m_poAssignStatementIndex->exist()))
        m_poAssignStatementIndex->create(iCount * m_fRatio);

    pAssignStatement= (AssignStatement *)(*m_poAssignStatementData);
	checkMem(m_poAssignStatementData->getCount() * sizeof(AssignStatement)); 
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poAssignStatementData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+4);
            
                        pAssignStatement[i].m_iStatementID = -1;
        pAssignStatement[i].m_iFunctionID = -1;
        pAssignStatement[i].m_iOperatorID = -1;

        pAssignStatement[i].m_bParsed = false;

        pAssignStatement[i].m_poFormula = NULL;
    
                static bool flag = true;
                if(flag) 
                {
                char str[32];
                memset(str,0,sizeof(str));
                if(ParamDefineMgr::getParam("PRICING","DISCT_PERCENT_ATTR_ID",str))
                 {
                        pAssignStatement[i].m_oDisctPercentAttrID = ID(str);
                 }
     
                 memset(str,0,sizeof(str));
                if(ParamDefineMgr::getParam("PRICING","DISCT_PERCENT_ATTR_VAL",str))
                {
                        pAssignStatement[i].m_iDisctPercentAttrValue = atoi(str);
                }
                flag = false;   
                } 
                

        pAssignStatement[i].m_iOperationID = qry.field (0).asInteger ();
        pAssignStatement[i].m_iStatementSeq = qry.field (1).asInteger ();
        pAssignStatement[i].m_iStatementID = pAssignStatement[i].m_iOperationID;

        pAssignStatement[i].setFunction (qry.field (2).asInteger());
        pAssignStatement[i].setOperator (OP_ASGN);
        pAssignStatement[i].setParam1 (qry.field (3).asString());
        pAssignStatement[i].setParam2 (qry.field (4).asString());
        pAssignStatement[i].setParam3 (qry.field (5).asString());
        pAssignStatement[i].setDesParam (qry.field (6).asString());
        pAssignStatement[i].m_poNext = NULL;
        pAssignStatement[i].m_iNextOffset= 0;

        //设置相同OperationID的链表
        if (i == 1)
        {
            m_poAssignStatementIndex->add((long)pAssignStatement[i].m_iOperationID, i);
        }
        else
        {
            if(pAssignStatement[i].m_iOperationID == pAssignStatement[i-1].m_iOperationID)
            {
                pAssignStatement[i-1].m_iNextOffset= i;
            }
            else
                m_poAssignStatementIndex->add((long)pAssignStatement[i].m_iOperationID, i);
        }
    }

    qry.close ();
    Log::log(0, "运算参数上载正常结束. 记录数: %d", m_poAssignStatementData->getCount());
    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poAssignStatementData->getCount(),m_poAssignStatementData->getTotal());
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poAssignStatementIndex->getCount(),m_poAssignStatementIndex->getTotal());

}

void ParamInfoMgr::loadFormula()
{
    int iCount;
    Formula *pFormula;
    char sSqlcode[512];
    TOCIQuery qry(Environment::getDBConn());
    Log::log(0, "开始公式参数上载");
    sprintf (sSqlcode, "SELECT FORMULA_ID, FORMULA "
                 " FROM FORMULA_LIST "
                 " WHERE STATE='F0A' ");

    char countbuf[32];
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FORMULA_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+3);
    }
    iCount = atoi(countbuf);

    if(!(m_poFormulaData->exist()))
        m_poFormulaData->create(iCount*m_fRatio);
    if(!(m_poFormulaIndex->exist()))
        m_poFormulaIndex->create(iCount*m_fRatio);
    pFormula = (Formula *)(*m_poFormulaData);
	checkMem(m_poFormulaData->getCount() * sizeof(Formula)); 
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL (sSqlcode);
    qry.open ();

    while(qry.next())
    {
        i = m_poFormulaData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+4);

        Formula *ppFormula = new(pFormula+i)Formula();   //这里要调用Formula()构造函数

        pFormula[i].m_iFormulaID = qry.field(0).asInteger();
        strncpy(pFormula[i].m_sFormula, qry.field(1).asString(), 2000);
        pFormula[i].m_bParsed = false;

        if(!pFormula[i].parse())
        {
            Log::log (0, "Formula(ID : %d) Error.", pFormula[i].m_iFormulaID);
            Log::log (0, "Formula(TXT: [%s])", pFormula[i].m_sFormula);
            THROW (MBC_FormulaMgr+1);
        }
        pFormula[i].m_bParsed = true;

        m_poFormulaIndex->add((long)pFormula[i].m_iFormulaID, i);
    }

    qry.close();
    Log::log(0, "公式参数上载正常结束. 记录数: %d", m_poFormulaData->getCount());
    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poFormulaData->getCount(),m_poFormulaData->getTotal());
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poFormulaIndex->getCount(),m_poFormulaIndex->getTotal());
    
}


void ParamInfoMgr::loadFilePoolInfo()
{
    TOCIQuery qry(Environment::getDBConn());
    int iCount = 0;
    char countbuf[32] ={0};
    char sKey[24]={0};

    Log::log(0, "开始加载文件池配置");

    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FILEPOOLINFO_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+180);
    }
    iCount = atoi(countbuf);
    if ( !(m_poFilePoolInfoData->exist()) ) {
        m_poFilePoolInfoData->create(iCount * m_fRatio);
    }
    if ( !(m_poFilePoolInfoIndex->exist()) ) {
        m_poFilePoolInfoIndex->create(iCount * m_fRatio, 24);
    }
    m_poFilePoolInfoList =(FilePoolInfo *) (*m_poFilePoolInfoData );

    qry.setSQL("select FILE_POOL_ID, SCHEDULER_FILE_ID_SEQ, node_id from B_FILE_POOL_CONFIG t "
               " ORDER BY node_id ");

    qry.open();
	checkMem(m_poFilePoolInfoData->getCount() * sizeof(FilePoolInfo)); 
    unsigned int i = 0;
    while (qry.next()) {
        i = m_poFilePoolInfoData->malloc();
        if (!i) {
            THROW(MBC_ParamInfoMgr+181);
        }
        m_poFilePoolInfoList[i].m_iNodeID = qry.field(2).asInteger();
        m_poFilePoolInfoList[i].m_iFilePoolID = qry.field(0).asInteger();
        strncpy(m_poFilePoolInfoList[i].m_sSchedulerFileIDSeq, qry.field(1).asString(), 60);
        snprintf(sKey, 24, "%d_%d", m_poFilePoolInfoList[i].m_iNodeID, 
                 m_poFilePoolInfoList[i].m_iFilePoolID );
        m_poFilePoolInfoIndex->add(sKey, i);
    }

    qry.close();
    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poFilePoolInfoData->getCount(),
             m_poFilePoolInfoData->getTotal());
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poFilePoolInfoIndex->getlCount(),
             m_poFilePoolInfoIndex->getlTotal());
}       

void ParamInfoMgr::loadFilePoolCondition()
{
    TOCIQuery qry(Environment::getDBConn());
    int iCount = 0;
    char countbuf[32] ={0};
    char sKey[24]={0};

    Log::log(0,"开始加载文件池条件配置");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FILEPOOLCOND_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+182);
    }
    iCount = atoi(countbuf);
    if ( !(m_poFilePoolCondData->exist() )) {
        m_poFilePoolCondData->create(iCount * m_fRatio);
    }
    if ( !(m_poFilePoolCondIndex->exist() )) {
        m_poFilePoolCondIndex->create(iCount * m_fRatio, 24);
    }
	checkMem(m_poFilePoolCondData->getCount() * sizeof(FilePoolCondition)); 
    m_poFilePoolCondList =(FilePoolCondition *) (*m_poFilePoolCondData );
        //有条件先
    qry.setSQL(" select t.file_type_id,t.file_pool_id,nvl(condition_id,0) condition_id, node_id,"
               " NVL(switch_opc_code, '-1') "
               " from B_FILE_POOL_TYPE_RELA t, B_FILE_POOL_CONFIG s where t.FILE_POOL_ID=s.FILE_POOL_ID "
               " order by node_id, file_type_id, NVL(condition_id, 0) ");

    qry.open();

    unsigned int i =0 ;
    unsigned int k = 0;
    FilePoolCondition *pTemp = NULL;
    while (qry.next()) {
        i = m_poFilePoolCondData->malloc();
        if (!i) {
            THROW(MBC_ParamInfoMgr+183);
        }

        snprintf(sKey, 24, "%d_%d", qry.field(3).asInteger(), qry.field(0).asInteger() );
        m_poFilePoolCondList[i].m_iFilePoolID = qry.field(1).asInteger();
        m_poFilePoolCondList[i].m_iConditionID = qry.field(2).asInteger();
        strncpy(m_poFilePoolCondList[i].m_sSwitchOpcCode, qry.field(4).asString(), 
                sizeof(m_poFilePoolCondList[i].m_sSwitchOpcCode));

        if (m_poFilePoolCondIndex->get(sKey, &k)) {
            m_poFilePoolCondList[i].m_pNext = &(m_poFilePoolCondList[k]);
        } else {
            m_poFilePoolCondList[i].m_pNext = 0;
        }
        m_poFilePoolCondIndex->add(sKey, i);
    }
    qry.close();
    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poFilePoolCondData->getCount(),
             m_poFilePoolCondData->getTotal());
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poFilePoolCondIndex->getlCount(),
             m_poFilePoolCondIndex->getlTotal());
}   

void ParamInfoMgr::loadFileTypeConfig(){
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char sKey[30] = {0};
    char countbuf[32]={0};

    Log::log(0, "开始加载文件类型配置");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FILETYPECFG_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+184);
    }
    iCount = atoi(countbuf);
    if ( !(m_poFileTypeIndex->exist() ) ) {
        m_poFileTypeIndex->create(iCount * m_fRatio, 30);
    }

    qry.setSQL(" select FILE_TYPE_ID, SOURCE_EVENT_TYPE_ID, "
               "nvl(SWITCH_ID, -1), nvl(BILLFLOW_ID, -1)  "
               " from B_FILE_TYPE_CONFIG ");

    qry.open();

    while (qry.next()) {
        snprintf(sKey, 30, "%d_%d_%d", qry.field(1).asInteger(),  
                 qry.field(2).asInteger(), qry.field(3).asInteger());
        m_poFileTypeIndex->add(sKey, qry.field(0).asInteger());
    }       
    qry.close();
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poFileTypeIndex->getlCount(),
             m_poFileTypeIndex->getlTotal());
}

void ParamInfoMgr::loadTLVEncodeStruct()
{
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char sKey[24] = {0};
    char countbuf[32]={0};

    Log::log(0, "开始加载TLV编码");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TLVENCODE_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+185);
    }
    iCount = atoi(countbuf);
    if ( !(m_poTLVEncodeData->exist()) )
        m_poTLVEncodeData->create(iCount * m_fRatio);
    if ( !(m_poTLVEncodeIndex->exist()) )
        m_poTLVEncodeIndex->create(iCount * m_fRatio);
	checkMem(m_poTLVEncodeData->getCount() * sizeof(FileAttr)); 
    m_poTLVEncodeList = (FileAttr *)(*m_poTLVEncodeData);

    qry.setSQL(" select DISTINCT file_type_id, nvl(attr_id, -1) attr_id from B_FILE_TYPE_ATTR_MAP "
               " order by file_type_id, attr_id DESC");

    int iEventAttrID = 0;
    unsigned int i = 0;
    unsigned int k = 0;
    qry.open();
    while (qry.next()) {
        iEventAttrID = qry.field(1).asInteger();
        if (iEventAttrID <= 0) {
            continue;
        }
        i = m_poTLVEncodeData->malloc();
        if ( !i ) {
            THROW(MBC_ParamInfoMgr + 186);
        }
        m_poTLVEncodeList[i].m_iAttrID = qry.field(1).asInteger();
        if ( !m_poTLVEncodeIndex->get(qry.field(0).asInteger() , &k) ) {
            m_poTLVEncodeList[i].m_iNext = 0;
        } else {
            m_poTLVEncodeList[i].m_iNext = k ;
        }
        m_poTLVEncodeIndex->add(qry.field(0).asInteger(), i);
    }
    qry.close();
    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poTLVEncodeData->getCount(),
             m_poTLVEncodeData->getTotal());
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poTLVEncodeIndex->getCount(),
             m_poTLVEncodeIndex->getTotal());
}
/*
void ParamInfoMgr::loadTLVDecodeStruct( )
{
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char sKey[24] = {0};
    char countbuf[32]={0};

    Log::log(0, "开始加载TLV解码");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TLVDECODE_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+3);
    }
    iCount = atoi(countbuf);
    if ( !(m_poTLVDecodeIndex->exist() ))
        m_poTLVDecodeIndex->create(iCount * m_fRatio, 24);

    qry.setSQL(" select file_type_id, nvl(attr_id, -1) attr_id from b_parser_attr_map "
               " order by file_type_id, attr_id ");

    qry.open();
    int iEventAttrID = 0;
    while (qry.next()) {
        iEventAttrID = qry.field(1).asInteger();
        if ( iEventAttrID <= 0 ) {
            continue;
        }
        snprintf(sKey, 24, "%d_%d", qry.field(0).asInteger(), iEventAttrID);
        m_poTLVDecodeIndex->add(sKey, 1);
    }
    qry.close();
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poTLVDecodeIndex->getlCount(),
             m_poTLVDecodeIndex->getlTotal());
}
*/
void ParamInfoMgr::loadTLVParseStruct( )
{
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char sKey[32] = {0};
    char countbuf[32]={0};

    Log::log(0, "开始加载TLV解析");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TLVDECODE_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+187);
    }
    iCount = atoi(countbuf);
    if ( !(m_poTLVDecodeIndex->exist() ))
        m_poTLVDecodeIndex->create(iCount * m_fRatio, 32);
    if ( !(m_poTLVDecodeData->exist() ) )
        m_poTLVDecodeData->create(iCount * m_fRatio);
	checkMem(m_poTLVDecodeData->getCount() * sizeof(FileAttr)); 
    m_poTLVDecodeList = (FileAttr*)(*m_poTLVDecodeData);

    qry.setSQL(" select DISTINCT file_type_id, nvl(attr_id, -1) attr_id , NVL(dest_attr_id, -1), "
               " rule_id "
               " from b_parser_attr_map "
               " order by rule_id, file_type_id, attr_id DESC");

    unsigned int i = 0, k = 0;
    int iEventAttrID = 0;
    qry.open();
    while (qry.next()) {
        iEventAttrID = qry.field(1).asInteger();
        if ( iEventAttrID <= 0 ) {
            continue;
        }

        i = m_poTLVDecodeData->malloc();
        if ( !i ) {
            THROW(MBC_ParamInfoMgr + 188);
        }
        m_poTLVDecodeList[i].m_iAttrID = qry.field(2).asInteger();

        snprintf(sKey, 32, "%d_%d_%d", qry.field(0).asInteger(), qry.field(3).asInteger(),iEventAttrID );
        if ( !m_poTLVDecodeIndex->get(sKey , &k) ) {
            m_poTLVDecodeList[i].m_iNext = 0;
        } else {
            m_poTLVDecodeList[i].m_iNext = k ;
        }
        m_poTLVDecodeIndex->add(sKey, i);
    }
    qry.close();
    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poTLVDecodeData->getCount(),
             m_poTLVDecodeData->getTotal());
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poTLVDecodeIndex->getlCount(),
             m_poTLVDecodeIndex->getlTotal());
}

void ParamInfoMgr::loadFileParseRule(){
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char sKey[24] = {0};
    char countbuf[32]={0};

    Log::log(0, "开始加载文件解析规则");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FILEPARSERULE_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+189);
    }
    iCount = atoi(countbuf);
    if ( !(m_poFileParseRuleIndex->exist() ))
        m_poFileParseRuleIndex->create(iCount * m_fRatio, 24);

    qry.setSQL (" select b.process_id, NVL(b.file_type_id,0), NVL(b.rule_id,0) "
                " from B_PROCESS_FILE_TYPE_RULE b "
                " order by process_id, file_type_id ");

    qry.open();
    unsigned int iRuleID = 0;
    while (qry.next()) {
        snprintf(sKey, sizeof(sKey), "%d_%d", qry.field(0).asInteger(), qry.field(1).asInteger() );
        if ( m_poFileParseRuleIndex->get(sKey, &iRuleID) ) {
            Log::log (0, "进程%d包含了FileTypeID:%d的多个解析规则,请检查B_PROCESS_FILE_TYPE_RULE表",
                      qry.field(0).asInteger(), qry.field(1).asInteger() );
            continue;
        }
        m_poFileParseRuleIndex->add(sKey, (unsigned int )qry.field(2).asInteger());
    }
    qry.close();
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poFileParseRuleIndex->getlCount(),
             m_poFileParseRuleIndex->getlTotal());
}

void ParamInfoMgr::loadSwitchCode(){
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char sKey[24] = {0};
    char countbuf[32]={0};

    Log::log(0, "开始加载TLV解码");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SWITCHCODE_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+190);
    }
    iCount = atoi(countbuf);
    if ( !(m_poSwitchCodeIndex->exist() ))
        m_poSwitchCodeIndex->create(iCount * m_fRatio, 24);
    if ( !(m_poSwitchCodeData->exist() ))
        m_poSwitchCodeData->create(iCount * m_fRatio);
	checkMem(m_poSwitchCodeData->getCount() * sizeof(SwitchCode)); 
    m_poSwitchCodeList = (SwitchCode *)(*m_poSwitchCodeData);

    qry.setSQL(" select FILE_TYPE_ID, FILE_POOL_ID, nvl(SWITCH_OPC_CODE, '-1')"
               " from B_FILE_POOL_TYPE_RELA ");

    unsigned int i = 0;
    qry.open();
    while (qry.next()) {
        i= m_poSwitchCodeData->malloc();
        if ( !i ) {
            THROW(MBC_ParamInfoMgr+191);
        }
        strncpy(m_poSwitchCodeList[i].m_sCode, qry.field(2).asString(), 
                sizeof(m_poSwitchCodeList[i].m_sCode)-1 );
        snprintf(sKey, sizeof(sKey), "%d_%d", qry.field(0).asInteger(), qry.field(1).asInteger() );

        m_poFileParseRuleIndex->add(sKey, i);
    }
    qry.close();
    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poSwitchCodeData->getCount(),
             m_poSwitchCodeData->getTotal());
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poSwitchCodeIndex->getlCount(),
             m_poSwitchCodeIndex->getlTotal());
}

void ParamInfoMgr::loadBillingNodeInfo()
{
    BillingNodeInfo *pTemp = NULL;
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char sKey[24] = {0};
    char countbuf[32]={0};

    Log::log(0, "开始加载计费节点信息");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_BILLINGNODE_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+192);
    }
    iCount = atoi(countbuf);
    if ( !(m_poBillingNodeData->exist()) )
        m_poBillingNodeData->create(iCount *m_fRatio);
    if ( !(m_poBillingNodeIndex->exist()) )
        m_poBillingNodeIndex->create(iCount * m_fRatio);
	checkMem(m_poBillingNodeData->getCount() * sizeof(BillingNodeInfo)); 
    m_poBillingNodeList = (BillingNodeInfo *)(*m_poBillingNodeData);
    //org_id都配置为200级别org
    qry.setSQL(" select node_id,parent_node_id,org_id,nvl(condtion_id,0) " 
               " from b_billing_node_define t where t.node_level=100 "
               " order by org_id,nvl(condtion_id,0) DESC ");
    //目前暂不支撑condtion配置                         
    unsigned int i = 0;
    unsigned int k = 0;
    int iOrgID = 0;
    qry.open();
    while (qry.next()) {
        i=m_poBillingNodeData->malloc();
        if ( !i ) {
            THROW(MBC_ParamInfoMgr + 193);
        }
        m_poBillingNodeList[i].m_iNodeID = qry.field(0).asInteger();
        m_poBillingNodeList[i].m_iParentNodeID = qry.field(1).asInteger();
        iOrgID = qry.field(2).asInteger();
        m_poBillingNodeList[i].m_iOrgID = iOrgID;
        m_poBillingNodeList[i].m_iConditionID = qry.field(3).asInteger();

        if ( m_poBillingNodeIndex->get(iOrgID, &k) ) {
            m_poBillingNodeList[i].m_poNext = &(m_poBillingNodeList[k]);
        } else {
            m_poBillingNodeList[i].m_poNext = 0;
        }
        m_poBillingNodeIndex->add(iOrgID, i);
    }
    qry.close();

    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poBillingNodeData->getCount(),
             m_poBillingNodeData->getTotal());
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poBillingNodeIndex->getCount(),
             m_poBillingNodeIndex->getTotal());

    return ;
}       

void ParamInfoMgr::loadCarrierInfo(){
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32]={0};

    Log::log(0, "开始加载国际漫出运营商信息");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ROAMCARRIER_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+231);
    }
    iCount = atoi(countbuf);

    if ( !(m_poRoamCarrierData->exist()) )
        m_poRoamCarrierData->create(iCount * m_fRatio);
    if ( !(m_poRoamCarrierIndex->exist()) )
        m_poRoamCarrierIndex->create(iCount *m_fRatio, 24);
	checkMem(m_poRoamCarrierData->getCount() * sizeof(CarrierInfo)); 
    m_poRoamCarrierList = (CarrierInfo *) (*m_poRoamCarrierData);

    qry.setSQL(" SELECT carrier_code, NVL(visited_country, ''), NVL(sponsor_code, 'CtoC'), "
               " NVL(imsi_type,'0'), sector_id, to_char(eff_date, 'yyyymmdd'), "
               " NVL(to_char(exp_date,'yyyymmdd'), '20500101')  "
               " FROM b_carrier_sector a  " );

    unsigned int i = 0;
    unsigned int k = 0;
    int *pTemp = 0;
    char key[20]={0};
    qry.open();
    while (qry.next() ) {
        
        i = m_poRoamCarrierData->malloc();
        if (!i){
            THROW(MBC_ParamInfoMgr + 232 );
        }
        memset(key, 0, sizeof(key) );
        memset( &m_poRoamCarrierList[i], 0, sizeof(CarrierInfo) );

        strcpy( m_poRoamCarrierList[i].m_sCarrierCode, qry.field(0).asString());
        strcpy( m_poRoamCarrierList[i].m_sVisitedCountry, qry.field(1).asString() );
        strcpy( m_poRoamCarrierList[i].m_sSponsorCode, qry.field(2).asString() );
        strcpy( m_poRoamCarrierList[i].m_sImsiType, qry.field(3).asString());
        strcpy( m_poRoamCarrierList[i].m_sSectorID, qry.field(4).asString()  );
        strcpy( m_poRoamCarrierList[i].m_sEffDate, qry.field(5).asString() );
        strcpy( m_poRoamCarrierList[i].m_sExpDate, qry.field(6).asString() );

        m_poRoamCarrierList[i].m_iNext = -1;

        sprintf( key, "%s_%s", 
                 m_poRoamCarrierList[i].m_sCarrierCode, 
                 m_poRoamCarrierList[i].m_sSponsorCode);

        if ( m_poRoamCarrierIndex->get(key, &k) ) {
            if ( strcmp(m_poRoamCarrierList[i].m_sEffDate, 
                        m_poRoamCarrierList[k].m_sEffDate) >= 0 ) {
                m_poRoamCarrierList[i].m_iNext = k;
                m_poRoamCarrierIndex->add(key, i);
            } else {
                pTemp = &(m_poRoamCarrierList[k].m_iNext);
                while (*pTemp && (-1 != *pTemp)) {
                    if ( strcmp(m_poRoamCarrierList[i].m_sEffDate, 
                                m_poRoamCarrierList[*pTemp].m_sEffDate) >= 0 ) {
                        break;
                    }

                    pTemp = &(m_poRoamCarrierList[*pTemp].m_iNext);
                }
                m_poRoamCarrierList[i].m_iNext = *pTemp;
                *pTemp = i;
            }
        } else {
            m_poRoamCarrierIndex->add( key , i );
        }
    }

    qry.close();
    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poRoamCarrierData->getCount(),
             m_poRoamCarrierData->getTotal());
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poRoamCarrierIndex->getlCount(),
             m_poRoamCarrierIndex->getlTotal());
    return;
}

void ParamInfoMgr::loadTariffInfo( ){

    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32]={0};

    Log::log(0, "开始加载国际漫出资费信息");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ROAMTARIFF_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+233);
    }
    iCount = atoi(countbuf);

    if ( !(m_poRoamTariffData->exist()) )
        m_poRoamTariffData->create(iCount * m_fRatio);
    if ( !(m_poRoamTariffIndex->exist()) )
        m_poRoamTariffIndex->create(iCount *m_fRatio);
	checkMem(m_poRoamTariffData->getCount() * sizeof(TariffInfo)); 
    m_poRoamTariffList = (TariffInfo *)(*m_poRoamTariffData);

    qry.setSQL("SELECT sector_no, NVL(special_group_id, 0), NVL(event_type_id, 0),"
               " NVL(value, 0), NVL(to_char(eff_date, 'yyyymmdd'), '20000101'), "
               " NVL(to_char(exp_date, 'yyyymmdd'),'20500101' )  "
               " FROM b_sector_tariff a "
               " ORDER BY sector_no, NVL(special_group_id, 0), "
               " NVL(to_char(eff_date, 'yyyymmdd'), '20000101') ");

    unsigned int i = 1;
    unsigned int k = 0;
    qry.open();
    bool bOrder = false;
    while (qry.next()) {
        i = m_poRoamTariffData->malloc();
        if (!i){
            THROW(MBC_ParamInfoMgr + 234);
        }
        memset (&m_poRoamTariffList[i], 0, sizeof(TariffInfo) );

        m_poRoamTariffList[i].m_iSectorNo = qry.field(0).asInteger();
        m_poRoamTariffList[i].m_iSpecialGroupID = qry.field(1).asInteger();
        m_poRoamTariffList[i].m_iEventTypeID = qry.field(2).asInteger();
        m_poRoamTariffList[i].m_lValue = qry.field(3).asLong();
        strcpy(m_poRoamTariffList[i].m_sEffDate, qry.field(4).asString() );
        strcpy(m_poRoamTariffList[i].m_sExpDate, qry.field(5).asString() );

        bOrder = false;

        if ( m_poRoamTariffIndex->get( m_poRoamTariffList[i].m_iSectorNo, &k ) ) {
            if ( (!m_poRoamTariffList[k].m_iSpecialGroupID &&
                  m_poRoamTariffList[i].m_iSpecialGroupID) 
                 ||
                 (m_poRoamTariffList[k].m_iSpecialGroupID &&
                  m_poRoamTariffList[i].m_iSpecialGroupID &&
                  (strcmp(m_poRoamTariffList[i].m_sEffDate, 
                          m_poRoamTariffList[k].m_sEffDate) >= 0) )
                 ||
                 (!m_poRoamTariffList[k].m_iSpecialGroupID &&
                  !m_poRoamTariffList[i].m_iSpecialGroupID &&
                  (strcmp(m_poRoamTariffList[i].m_sEffDate, 
                          m_poRoamTariffList[k].m_sEffDate) >= 0))
               ) {
                m_poRoamTariffList[i].m_iNext = k;
                m_poRoamTariffIndex->add(m_poRoamTariffList[i].m_iSectorNo, i);
            } else {
                unsigned int *pTemp = &(m_poRoamTariffList[k].m_iNext);

                if ( *pTemp ) {
                    if ( !m_poRoamTariffList[i].m_iSpecialGroupID && 
                         m_poRoamTariffList[*pTemp].m_iSpecialGroupID) {
                        while (*pTemp ) {
                            if ( !m_poRoamTariffList[*pTemp].m_iSpecialGroupID ) {
                                break;
                            }
                            pTemp = &(m_poRoamTariffList[*pTemp].m_iNext);
                        }
                    } else if ( m_poRoamTariffList[i].m_iSpecialGroupID ) {
                        while (*pTemp ) {
                            if ( !m_poRoamTariffList[*pTemp].m_iSpecialGroupID ||
                                 (m_poRoamTariffList[*pTemp].m_iSpecialGroupID &&
                                  (strcmp(m_poRoamTariffList[i].m_sEffDate, 
                                          m_poRoamTariffList[*pTemp].m_sEffDate) >= 0) ) ) {
                                break;
                            }
                            pTemp = &(m_poRoamTariffList[*pTemp].m_iNext);
                        }

                        m_poRoamTariffList[i].m_iNext = *pTemp;
                        *pTemp = i;
                        bOrder = true;
                    }
                }

                if (!bOrder) {
                    while ( *pTemp ) {
                        if ( strcmp(m_poRoamTariffList[i].m_sEffDate, 
                                    m_poRoamTariffList[*pTemp].m_sEffDate) >= 0 ) {
                            break;
                        }

                        pTemp = &(m_poRoamTariffList[*pTemp].m_iNext);
                    }

                    m_poRoamTariffList[i].m_iNext = *pTemp;
                    *pTemp = i;
                }
            }
        } else {
            m_poRoamTariffIndex->add(m_poRoamTariffList[i].m_iSectorNo, i);
        }

    }
    qry.close();

    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poRoamTariffData->getCount(),
             m_poRoamTariffData->getTotal());
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poRoamTariffIndex->getCount(),
             m_poRoamTariffIndex->getTotal());
}

void ParamInfoMgr::loadSponsorInfo(){
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32]={0};

    Log::log(0, "开始加载国际漫出代理运营商信息");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ROAMSPONSOR_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+235);
    }
    iCount = atoi(countbuf);

    if ( !(m_poSponsorData->exist()) )
        m_poSponsorData->create(iCount * m_fRatio);
    if ( !(m_poSponsorIndex->exist()) )
        m_poSponsorIndex->create(iCount *m_fRatio, 8);
	checkMem(m_poSponsorData->getCount() * sizeof(SponsorInfo)); 
    m_poSponsorList = (SponsorInfo *)(*m_poSponsorData);

    qry.setSQL("SELECT sponsor_id, NVL(sponsor_code, 'CtoC'), NVL(default_sector_no,0) FROM "
               " b_sponsor_carrier_info ");
    unsigned int i = 0;
    qry.open();
    while ( qry.next() ) {
        i = m_poSponsorData->malloc();
        if ( !i ){
            THROW(MBC_ParamInfoMgr + 236);
        }
        memset(&m_poSponsorList[i], 0, sizeof(SponsorInfo) );

        strcpy( m_poSponsorList[i].m_sSponsorID, qry.field(0).asString());
        strcpy( m_poSponsorList[i].m_sSponsorCode, qry.field(1).asString());
        m_poSponsorList[i].m_iDefaultSectorNo = qry.field(2).asInteger();

        m_poSponsorIndex->add( m_poSponsorList[i].m_sSponsorID, i);

    }
    qry.close();
    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poSponsorData->getCount(),
             m_poSponsorData->getTotal());
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poSponsorIndex->getlCount(),
             m_poSponsorIndex->getlTotal());
}

void ParamInfoMgr::loadSpecialGroupMember() {
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32]={0};

    Log::log(0, "开始加载国际漫出特殊资费组成员信息");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SPECIALGROUPMEMBER_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+237);
    }
    iCount = atoi(countbuf);

    if ( !(m_poMemberData->exist()) )
        m_poMemberData->create(iCount * m_fRatio);
    if ( !(m_poMemberIndex->exist()) )
        m_poMemberIndex->create(iCount *m_fRatio);
	checkMem(m_poMemberData->getCount() * sizeof(SpecialGroupMember)); 
    m_poMemberList = (SpecialGroupMember *)(*m_poMemberData);

    qry.setSQL("SELECT group_id, NVL(group_member, '') FROM b_special_tariff_group_member ");
    unsigned int i = 0;
    unsigned int k = 0;
    qry.open();
    while ( qry.next() ) {
        i = m_poMemberData->malloc();
        if (!i){
            THROW(MBC_ParamInfoMgr + 238);
        }
        memset(&m_poMemberList[i], 0, sizeof(SpecialGroupMember) );
        m_poMemberList[i].m_iGroupID = qry.field(0).asInteger();
        strcpy( m_poMemberList[i].m_sMember, qry.field(1).asString());
        m_poMemberList[i].m_iNext = -1;

        if (m_poMemberIndex->get(m_poMemberList[i].m_iGroupID, &k) ) {
            m_poMemberList[i].m_iNext = k;
        }
        m_poMemberIndex->add( m_poMemberList[i].m_iGroupID, i);

    }
    qry.close();

    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poMemberData->getCount(),
             m_poMemberData->getTotal());
    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poMemberIndex->getCount(),
             m_poMemberIndex->getTotal());
}

void ParamInfoMgr::loadSectorInfo( ){
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32]={0};

    Log::log(0, "开始加载国际漫出资费区信息");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TARIFFSECTOR_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+ 239);
    }
    iCount = atoi(countbuf);

    if ( !(m_poTariffSectorIndex->exist()) )
        m_poTariffSectorIndex->create(iCount *m_fRatio, 20);

    char key[20] = {0};
    qry.setSQL("SELECT sector_id, NVL(sponsor_code, 'CtoC'), sector_no FROM b_tariff_sector a ");
    qry.open();
    while (qry.next()) {
        memset(key, 0, sizeof(key) );
        sprintf(key, "%s_%s", qry.field(0).asString(), qry.field(1).asString() );
        m_poTariffSectorIndex->add(key, qry.field(2).asInteger() );
    }
    qry.close();

    Log::log(0, "\t索引区：记录数/总数: %d/%d", m_poTariffSectorIndex->getlCount(),
             m_poTariffSectorIndex->getlTotal());
}

void ParamInfoMgr::loadCountryInfo(){
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32]={0};

    Log::log(0, "开始加载国家信息");
    if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_COUNTRY_NUM, countbuf)) {
        THROW(MBC_ParamInfoMgr+240);
    }
    iCount = atoi(countbuf);

    if ( !(m_poCountryData->exist() ) )
        m_poCountryData->create(iCount * m_fRatio);
    if ( !(m_poCountryCodeIndex->exist()) )
        m_poCountryCodeIndex->create(iCount *m_fRatio, 4);
    if ( !(m_poCountryAreaIndex->exist()) )
        m_poCountryAreaIndex->create(iCount *m_fRatio, 10);
	checkMem(m_poCountryData->getCount() * sizeof(CountryInfo)); 
    m_poCountryList = (CountryInfo *)(*m_poCountryData);
    qry.setSQL("SELECT country_code, NVL(area_code,''), NVL(cn_name, ''), to_char(eff_date,'yyyymmdd'), "
               " NVL(to_char(exp_date,'yyyymmdd'), '20500101') "
               " FROM b_country_info ");
    qry.open();
    unsigned int i = 0;
    int *pTemp = NULL;
    unsigned int k = 0;
    while ( qry.next() ) {
        i = m_poCountryData->malloc();
        if (!i){
            THROW(MBC_ParamInfoMgr +241);
        }
        memset(&m_poCountryList[i], 0, sizeof(CountryInfo) );

        strcpy(m_poCountryList[i].m_sCountryCode ,qry.field(0).asString());
        strcpy(m_poCountryList[i].m_sAreaCode, qry.field(1).asString());
        strcpy(m_poCountryList[i].m_sCnName, qry.field(2).asString());
        strcpy(m_poCountryList[i].m_sEffDate, qry.field(3).asString());
        strcpy(m_poCountryList[i].m_sExpDate, qry.field(4).asString());

        m_poCountryList[i].m_iNext = -1;

        ////m_poCountryIndex->add(m_poCountryList[i].m_sCnName, i); 
        m_poCountryAreaIndex->add(m_poCountryList[i].m_sAreaCode, i);

        if ( m_poCountryCodeIndex->get(m_poCountryList[i].m_sCountryCode, &k) ) {
            if ( strcmp(m_poCountryList[i].m_sExpDate, 
                        m_poCountryList[k].m_sExpDate) >= 0 ) {
                m_poCountryList[i].m_iNext = k;
                m_poCountryCodeIndex->add(m_poCountryList[i].m_sCountryCode, i);
            } else {
                pTemp = &(m_poCountryList[k].m_iNext);
                while (*pTemp) {
                    if ( strcmp(m_poCountryList[i].m_sExpDate, 
                                m_poCountryList[*pTemp].m_sExpDate) >= 0 ) {
                        break;
                    }

                    pTemp = &(m_poCountryList[*pTemp].m_iNext);
                }
                m_poCountryList[i].m_iNext = *pTemp;
                *pTemp = i;
            }
        } else {
            m_poCountryCodeIndex->add(m_poCountryList[i].m_sCountryCode, i);
        }

    }
    qry.close();

    Log::log(0, "\t数据区：记录数/总数: %d/%d", m_poCountryData->getCount(),
             m_poCountryData->getTotal());
    Log::log(0, "\t索引区(国家码)：记录数/总数: %d/%d", m_poCountryCodeIndex->getlCount(),
             m_poCountryCodeIndex->getlTotal());
    Log::log(0, "\t索引区(区号)：记录数/总数: %d/%d", m_poCountryAreaIndex->getlCount(),
             m_poCountryAreaIndex->getlTotal());
}
void ParamInfoMgr::loadLocalEventTypeRule()
{
    int iOrgPathCnt,iLocalRuleCnt;
    char cOrgPathbuf[32],cLocalRulebuf[32];

    OrgPath *pOrgPath,*pOrgPathHead;
        LocalEventTypeRule *pLocalRule;
    char sSqlcode[2048];
        Org * pOrg;
        unsigned int indx1;

        
    TOCIQuery qry(Environment::getDBConn());
    Log::log(0, "开始LocalEventTypeRule上载");
        

    if(!ParamDefineMgr::getParam(LOADPARAMINFO,PARAM_ORGPATH_INFO_NUM, cOrgPathbuf))
    {
        THROW(MBC_ParamInfoMgr+3);
    }
    iOrgPathCnt = atoi(cOrgPathbuf);
    if(!ParamDefineMgr::getParam(LOADPARAMINFO,PARAM_LOCALEVENTTYPERULE_INFO_NUM, cLocalRulebuf))
    {
        THROW(MBC_ParamInfoMgr+3);
    }
    iLocalRuleCnt = atoi(cLocalRulebuf);
        

    if(!(m_poOrgPathData->exist()))
        m_poOrgPathData->create(iOrgPathCnt*m_fRatio);
    if(!(m_poOrgPathIndex->exist()))
        m_poOrgPathIndex->create(iOrgPathCnt*m_fRatio);
                
    if(!(m_poLocalEventTypeRuleData->exist()))
        m_poLocalEventTypeRuleData->create(iLocalRuleCnt*m_fRatio);
    if(!(m_poLocalEventTypeRuleIndex->exist()))
        m_poLocalEventTypeRuleIndex->create(iLocalRuleCnt*m_fRatio,20);
	checkMem(m_poOrgPathData->getCount() * sizeof(OrgPath)); 
	checkMem(m_poLocalEventTypeRuleData->getCount() * sizeof(LocalEventTypeRule)); 
    pOrgPath = (OrgPath *)(*m_poOrgPathData);
        pLocalRule = (LocalEventTypeRule *)(*m_poLocalEventTypeRuleData);
        pOrg = (Org *)(*m_poOrgData);
        
        memset(sSqlcode,0,2048);
    sprintf (sSqlcode, "SELECT ORG_ID,rownum row_id from (select org_id FROM ORG order by org_level_id)");
        unsigned int i = 0;
    qry.setSQL (sSqlcode);
    qry.open ();
        while(qry.next())
        {
                i = m_poOrgPathData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+4);
                pOrgPath[i].m_iOrgID = qry.field("ORG_ID").asInteger();
                pOrgPath[i].m_iRowID = qry.field("row_id").asInteger();
                pOrgPath[i].m_iParentOffset = 0;
                pOrgPath[i].m_poParent=0;
                pOrgPath[i].m_sOrgPath[0]=0;

                m_poOrgPathIndex->add((long)pOrgPath[i].m_iOrgID,i);
                
        }
        qry.close();
        
        i=1;
        int iOrgPathCount = m_poOrgPathData->getCount();
        
        //OrgPath的组织
        for(;i<=iOrgPathCount;i++)
        {
                char sTemp[200];
        int  iCurrentOrgID = pOrgPath[i].m_iOrgID;
                sprintf(pOrgPath[i].m_sOrgPath,"%d|",pOrgPath[i].m_iRowID);

                for (;;)
                {
                        int iParentID;
                        if(m_poOrgIndex->get(iCurrentOrgID,&indx1))
                                iParentID =pOrg[indx1].m_iParentID;
                        else
                                break;
                        
            if (iParentID < 0)
                break;
            else
                iCurrentOrgID = iParentID;
          
                        if(!m_poOrgPathIndex->get(iCurrentOrgID,&indx1))
                        {
                                THROW(MBC_LocalEvtRule+3);
                        }
                        
                        sprintf (sTemp, "%d|%s", pOrgPath[indx1].m_iRowID , pOrgPath[i].m_sOrgPath);
                        strcpy (pOrgPath[i].m_sOrgPath, sTemp);
                }
        }


        int iFirstOrgID = 0;
        qry.setSQL("select org_id from org where parent_org_id is null");
    qry.open();
    if (!qry.next())
          iFirstOrgID = 0;
    else  iFirstOrgID = qry.field(0).asInteger();
    qry.close();

        memset(sSqlcode,0,2048);
        
        sprintf (sSqlcode, " select nvl(b.org_level_id,-1) level_1, nvl(c.org_level_id,-1) level_2, " 
        " decode(in_event_type_id,null,1,0) s0,"
        " decode(calling_sp_type_id,null,1,0) s1, decode(called_sp_type_id,null,1,0) s2, "
        " decode(calling_service_type_id,null,1,0) s3, decode(called_service_type_id,null,1,0) s4, "
        " decode(carrier_type_id,null,1,0) s5,decode(business_key,null,1,0) s6,"
        " rule_id, source_event_type,"
        " nvl(in_event_type_id,-1) in_event_type_id, "
        " nvl(calling_org_id,:vCallingFirst) calling_org_id, "
        " nvl(called_org_id,:vCalledFirst) called_org_id, "
        " nvl(calling_sp_type_id,-1) calling_sp_type_id, "
        " nvl(called_sp_type_id,-1) called_sp_type_id, "
        " nvl(calling_service_type_id,-1) calling_service_type_id, "
        " nvl(called_service_type_id,-1) called_service_type_id, "
        " nvl(carrier_type_id,-1) carrier_type_id, "
        " nvl(business_key,-1) business_key, out_event_type_id "
        " from b_local_event_type_rule a, org b, org c "
        " where a.calling_org_id = b.org_id(+) and a.called_org_id = c.org_id(+) "
        " order by source_event_type, s0,"
        "         level_1 , calling_org_id desc, level_2 , called_org_id desc,"
        "        s1,s2,s3,s4,s5,s6 "
        );
    
    qry.setSQL (sSqlcode);
    qry.setParameter("vCallingFirst",iFirstOrgID);
    qry.setParameter("vCalledFirst",iFirstOrgID);
    qry.open ();
        while(qry.next())
        {
                i = m_poLocalEventTypeRuleData->malloc();
                if(!i)
                        THROW(MBC_ParamInfoMgr+152);
                pLocalRule[i].m_poNextSourceEvent = 0;
        pLocalRule[i].m_poNextInEventType = 0;
        pLocalRule[i].m_poNextCallingOrgID = 0;
        pLocalRule[i].m_poNextCalledOrgID = 0;
        pLocalRule[i].m_pCallingTree = 0;
        pLocalRule[i].m_pCalledTree = 0;
        pLocalRule[i].m_poNextRule = 0; 

                pLocalRule[i].m_iRuleID = qry.field("rule_id").asInteger();
        pLocalRule[i].m_iSourceEventType = qry.field("source_event_type").asInteger();
        pLocalRule[i].m_iInEventTypeID = qry.field("in_event_type_id").asInteger();
        pLocalRule[i].m_iCallingOrgID = qry.field("calling_org_id").asInteger();
        pLocalRule[i].m_iCalledOrgID = qry.field("called_org_id").asInteger();
        pLocalRule[i].m_iCallingSPTypeID = qry.field("calling_sp_type_id").asInteger();
        pLocalRule[i].m_iCalledSPTypeID = qry.field("called_sp_type_id").asInteger();
        pLocalRule[i].m_iCallingServiceTypeID = qry.field("calling_service_type_id").asInteger();
        pLocalRule[i].m_iCalledServiceTypeID = qry.field("called_service_type_id").asInteger();
        pLocalRule[i].m_iCarrierTypeID = qry.field("carrier_type_id").asInteger();
        pLocalRule[i].m_iBusinessKey = qry.field("business_key").asInteger();
        pLocalRule[i].m_iOutEventTypeID = qry.field("out_event_type_id").asInteger();
        pLocalRule[i].m_iNextOffset = 0;
                pLocalRule[i].m_sCallingOrgPath[0]=0;
                pLocalRule[i].m_sCalledOrgPath[0]=0;
                
                if(m_poOrgPathIndex->get( pLocalRule[i].m_iCallingOrgID,&indx1))
                        strcpy(pLocalRule[i].m_sCallingOrgPath,pOrgPath[indx1].m_sOrgPath);
                
                if(m_poOrgPathIndex->get( pLocalRule[i].m_iCalledOrgID,&indx1))
                        strcpy(pLocalRule[i].m_sCalledOrgPath,pOrgPath[indx1].m_sOrgPath);

                char key[20];
                sprintf(key,"%d_%d",pLocalRule[i].m_iSourceEventType,pLocalRule[i].m_iInEventTypeID);

                unsigned int iOffset = 0;
                if(m_poLocalEventTypeRuleIndex->get(key,&iOffset))
                {
                        pLocalRule[i].m_iNextOffset = iOffset;          
                        m_poLocalEventTypeRuleIndex->add(key,i);
                }       
                else
                        m_poLocalEventTypeRuleIndex->add(key,i);

        }
                
}



void ParamInfoMgr::loadCheckDup()
{       
        int iDupCfgCnt,iDupTypeCnt,iDupStepCnt,iShotTicketCnt;
        int iUndoProcessCnt,iUndoProcShotCnt,iUndoServCnt,iUndoProductCnt;
        char cDupCfgBuf[32],cDupTypeBuf[32],cDupStepBuf[32];
        char cShotTicketBuf[32],cUndoProcessBuf[32],cUndoProcShotBuf[32];
        char cUndoServBuf[32],cUndoProductBuf[32];
        struct CheckDupCfg *pDupCfg;
        struct CheckDupType *pDupType;
        struct CheckDupStep *pDupStep;
        struct CheckDupType *pShotTicket;
        struct UndoProcess *pUndoProcess;
        struct UndoProcess *pUndoProcShot;
        struct UndoServ *pUndoServ;
        struct UndoProductPackage *UndoProduct;
        unsigned int i,j,k;
    bool bExistNeedCheckTime = false;    //是否存在需要 判断时间关系的 重单类型
    bool bExistNotNeedCheckTime = false;


        if(!ParamDefineMgr::getParam(LOADPARAMINFO,PARAM_CHECKDUPCFG_INFO_NUM, cDupCfgBuf))
        {
                THROW(MBC_ParamInfoMgr+151);
        }
        iDupCfgCnt = atoi(cDupCfgBuf);
        if(!ParamDefineMgr::getParam(LOADPARAMINFO,PARAM_CHECKDUPTYPE_INFO_NUM, cDupTypeBuf))
        {
                THROW(MBC_ParamInfoMgr+152);
        }
        iDupTypeCnt = atoi(cDupTypeBuf);
        if(!ParamDefineMgr::getParam(LOADPARAMINFO,PARAM_CHECKDUPSTEP_INFO_NUM, cDupStepBuf))
        {
                THROW(MBC_ParamInfoMgr+153);
        }
        iDupStepCnt = atoi(cDupStepBuf);
        if(!ParamDefineMgr::getParam(LOADPARAMINFO,PARAM_CHECKSHOTTICKET_INFO_NUM, cShotTicketBuf))
        {
                THROW(MBC_ParamInfoMgr+154);
        }
        iShotTicketCnt = atoi(cShotTicketBuf);
        if(!ParamDefineMgr::getParam(LOADPARAMINFO,PARAM_UNDOPROCESSDUP_INFO_NUM, cUndoProcessBuf))
        {
                THROW(MBC_ParamInfoMgr+154);
        }
        iUndoProcessCnt = atoi(cUndoProcessBuf);
        if(!ParamDefineMgr::getParam(LOADPARAMINFO,PARAM_UNDOPROCESSSHOT_INFO_NUM, cUndoProcShotBuf))
        {
                THROW(MBC_ParamInfoMgr+154);
        }
        iUndoProcShotCnt = atoi(cUndoProcShotBuf);
        if(!ParamDefineMgr::getParam(LOADPARAMINFO,PARAM_UNDOSERVDUP_INFO_NUM, cUndoServBuf))
        {
                THROW(MBC_ParamInfoMgr+154);
        }
        iUndoServCnt = atoi(cUndoServBuf);
        if(!ParamDefineMgr::getParam(LOADPARAMINFO,PARAM_UNDOPRODUCTDUP_INFO_NUM, cUndoProductBuf))
        {
                THROW(MBC_ParamInfoMgr+154);
        }
        iUndoProductCnt = atoi(cUndoProductBuf);



        if(!(m_poCheckDupCfgData->exist()))
                m_poCheckDupCfgData->create(iDupCfgCnt * m_fRatio);
        if(!(m_poCheckDupCfgIndex->exist()))
                m_poCheckDupCfgIndex->create(iDupCfgCnt * m_fRatio);
        if(!(m_poCheckDupTypeData->exist()))
                m_poCheckDupTypeData->create(iDupTypeCnt * m_fRatio);
        if(!(m_poCheckDupTypeIndex->exist()))
                m_poCheckDupTypeIndex->create(iDupTypeCnt * m_fRatio);
        if(!(m_poCheckDupStepData->exist()))
                m_poCheckDupStepData->create(iDupStepCnt * m_fRatio);
        if(!(m_poCheckDupStepIndex->exist()))
                m_poCheckDupStepIndex->create(iDupStepCnt * m_fRatio);
        if(!(m_poShotTicketData->exist()))
                m_poShotTicketData->create(iShotTicketCnt * m_fRatio);
        if(!(m_poShotTicketIndex->exist()))
                m_poShotTicketIndex->create(iShotTicketCnt * m_fRatio);
        if(!(m_poUndoProcessData->exist()))
                m_poUndoProcessData->create(iUndoProcessCnt * m_fRatio);
        if(!(m_poUndoProcessIndex->exist()))
                m_poUndoProcessIndex->create(iUndoProcessCnt * m_fRatio);
        if(!(m_poUndoProcShotData->exist()))
                m_poUndoProcShotData->create(iUndoProcShotCnt * m_fRatio);
        if(!(m_poUndoProcShotIndex->exist()))
                m_poUndoProcShotIndex->create(iUndoProcShotCnt * m_fRatio);
        if(!(m_poUndoServData->exist()))
                m_poUndoServData->create(iUndoServCnt * m_fRatio);
        if(!(m_poUndoProductData->exist()))
                m_poUndoProductData->create(iUndoProductCnt * m_fRatio);


        pDupCfg = (struct CheckDupCfg *)(*m_poCheckDupCfgData);
        pDupType = (struct CheckDupType *)(*m_poCheckDupTypeData);
        pDupStep = (struct CheckDupStep *)(*m_poCheckDupStepData);
        pShotTicket = (struct CheckDupType *)(*m_poShotTicketData);
        pUndoProcess = (struct UndoProcess *)(*m_poUndoProcessData);
        pUndoProcShot = (struct UndoProcess *)(*m_poUndoProcShotData);
        pUndoServ =(struct UndoServ *)(*m_poUndoServData);
        UndoProduct = (struct UndoProductPackage *)(*m_poUndoProductData);
        
		checkMem(m_poCheckDupCfgData->getCount() * sizeof(CheckDupCfg)); 
		checkMem(m_poCheckDupTypeData->getCount() * sizeof(CheckDupType)); 
		checkMem(m_poCheckDupStepData->getCount() * sizeof(CheckDupStep)); 
		checkMem(m_poShotTicketData->getCount() * sizeof(CheckDupType)); 
		checkMem(m_poUndoProcessData->getCount() * sizeof(UndoProcess)); 
		checkMem(m_poUndoProcShotData->getCount() * sizeof(UndoProcess)); 
		checkMem(m_poUndoServData->getCount() * sizeof(UndoServ)); 
		checkMem(m_poUndoProductData->getCount() * sizeof(UndoProductPackage)); 
                
        char sSql[2048];
        memset(sSql, 0 , 2048);
        TOCIQuery qryDupCfg(Environment::getDBConn());

        sprintf(sSql," SELECT PROCESS_ID, DATA_PATH,IN_EVENT_TYPE,DUP_TYPE, "
                  " OUT_EVENT_TYPE FROM B_DUP_DATA_PATH a ,  "
                  " B_DUP_CHECK_CFG b WHERE a.PROCESS_TYPE_ID=b.PROCESS_TYPE_ID "
                  " ORDER BY PROCESS_ID");

        qryDupCfg.setSQL(sSql);
        qryDupCfg.open();
        while(qryDupCfg.next())
        {
                i = m_poCheckDupCfgData->malloc();
                if(!i)
                        THROW(MBC_ParamInfoMgr+161);
                pDupCfg[i].iProcessID = qryDupCfg.field("PROCESS_ID").asInteger();
                strcpy(pDupCfg[i].sCheckDataPath,qryDupCfg.field("DATA_PATH").asString());
                pDupCfg[i].iInEventType = qryDupCfg.field("IN_EVENT_TYPE").asInteger();
                pDupCfg[i].iDupType=qryDupCfg.field("DUP_TYPE").asInteger();
                pDupCfg[i].iOutEventType = qryDupCfg.field("OUT_EVENT_TYPE").asInteger();
                
                AllTrim ( pDupCfg[i].sCheckDataPath);
                pDupCfg[i].pNext = NULL;
                pDupCfg[i].m_iNextOffset = 0;
                
                ////apin 200611 增加目录存在性的校验
                //CheckFilePath( pDupCfg[i].sCheckDataPath);

                unsigned int iOffset = 0;
                if(m_poCheckDupCfgIndex->get(pDupCfg[i].iProcessID,&iOffset))
                {       
                        pDupCfg[i].m_iNextOffset = iOffset;
                        m_poCheckDupCfgIndex->add(pDupCfg[i].iProcessID,i);
                }
                else
                        m_poCheckDupCfgIndex->add(pDupCfg[i].iProcessID,i);
        }
        qryDupCfg.close();

        memset(sSql, 0 , 2048);
        TOCIQuery qryDupType(Environment::getDBConn());
        sprintf(sSql," select A.PROCESS_ID, C.DUP_TYPE from B_DUP_DATA_PATH A, B_DUP_TYPE_RELATION B , "
                    "   B_DUP_TICKET_TYPE C WHERE A.PROCESS_TYPE_ID=B.PROCESS_TYPE_ID and B.DUP_TYPE =C.DUP_TYPE  "
                      "   AND nvl(C.NEED_DO_OTHER,0)=0 ORDER BY A.PROCESS_ID ASC , PRIORITY_LEVEL DESC ");

        qryDupType.setSQL(sSql);
        qryDupType.open();
        while(qryDupType.next())
        {
                j = m_poCheckDupTypeData->malloc();
                if(!j)
                        THROW(MBC_ParamInfoMgr+162);
                pDupType[j].m_iProcessID = qryDupType.field("PROCESS_ID").asInteger();
                pDupType[j].m_iDupType = qryDupType.field("DUP_TYPE").asInteger();
                                pDupType[j].iDupType = pDupType[j].m_iDupType;//modify hsir 201 这其实应该是一个字段，但现在时间紧急就暂时留着,一个是共享化使用，一个是私有使用
                pDupType[j].m_iNextOffset = 0;
                pDupType[j].m_bNeedJudgeCross = FALSE;
                pDupType[j].m_bTimeNotCheckFlag = FALSE;
                pDupType[j].m_bTimeAllCheckFlag = FALSE;

                if (IsCrossed (pDupType[j].m_iDupType)) 
        { //当前重单类型 属于 包容交叉类重单
                pDupType[j].m_bNeedJudgeCross = true;
        }
                unsigned int iOffset = 0;
                if(m_poCheckDupTypeIndex->get((long)pDupType[j].m_iProcessID,&iOffset))
                {       
                        pDupType[j].m_iNextOffset = iOffset;
                        m_poCheckDupTypeIndex->add((long)pDupType[j].m_iProcessID,j);
                }
                else
                        m_poCheckDupTypeIndex->add((long)pDupType[j].m_iProcessID,j);
        }
        qryDupType.close();
                
        memset(sSql, 0 , 2048);
        TOCIQuery qryDupStep(Environment::getDBConn());
        sprintf(sSql," select DUP_TYPE, STEP_ID,NVL(DUP_FIELD_ID,-1) DUP_FIELD_ID ,CONDITION_TYPE, "









               " NVL(HIS_DUP_FIELD_ID,-1) HIS_DUP_FIELD_ID,NVL(CHILD_STEP_ID1,-1) CHILD_STEP_ID1, "
               "   NVL(CHILD_STEP_ID2,-1) CHILD_STEP_ID2 ,NVL(TIME_OFFSET,0) TIME_OFFSET  "
               "   from B_DUP_TYPE_RULE  "
                "  ORDER BY DUP_TYPE ASC,STEP_ID DESC ");
        qryDupStep.setSQL(sSql);
        qryDupStep.open();
        while(qryDupStep.next())
        {       
                k = m_poCheckDupStepData->malloc();
                if(!k)
                        THROW(MBC_ParamInfoMgr+163);
                pDupStep[k].iDupType=qryDupStep.field("DUP_TYPE").asInteger();
                pDupStep[k].iStepID=qryDupStep.field("STEP_ID").asInteger();
        pDupStep[k].iDupFieldID=qryDupStep.field("DUP_FIELD_ID").asInteger();
        pDupStep[k].iHisDupFieldID=qryDupStep.field("HIS_DUP_FIELD_ID").asInteger();
        strcpy( pDupStep[k].sConditionType,qryDupStep.field("CONDITION_TYPE").asString());
        AllTrim( pDupStep[k].sConditionType);
        pDupStep[k].iChildStepID1=qryDupStep.field("CHILD_STEP_ID1").asInteger();
        pDupStep[k].iChildStepID2=qryDupStep.field("CHILD_STEP_ID2").asInteger();
        pDupStep[k].iTimeOffset = qryDupStep.field("TIME_OFFSET").asInteger();
        pDupStep[k].iResult=0;
                pDupStep[k].m_iNextOffset = 0;
                pDupStep[k].bTimeChecked = false;
                
                unsigned int iOffset = 0;
                if(m_poCheckDupStepIndex->get(pDupStep[k].iDupType,&iOffset))
                {       
                        pDupStep[k].m_iNextOffset = iOffset;
                        m_poCheckDupStepIndex->add(pDupStep[k].iDupType,k);
                }
                else
                        m_poCheckDupStepIndex->add(pDupStep[k].iDupType,k);

                //add by yks 07.10 for QingHai SP
                if ((pDupStep[k].iDupFieldID==2 || pDupStep[k].iDupFieldID==7)
                        &&(pDupStep[k].iHisDupFieldID==2 || pDupStep[k].iHisDupFieldID==7)
                        //&&(!strcmp(tempdupstep.sConditionType,"==") ||
                        //       !strcmp(tempdupstep.sConditionType,">=") ||
                        //       !strcmp(tempdupstep.sConditionType,"<=")
                        //      )
                   ) {
                        pDupStep[k].bTimeChecked = true; //表示当前判断步骤需要判断时间关系
                }
                        
        }
        qryDupStep.close();


        //遍历整个process_dup ,和dup_step 置标记
        {
                bool bTimeChecked = false;
                struct CheckDupType *pType=0;
                struct  CheckDupType *pDupType_t=0;
                struct CheckDupStep *pStep=0;
                struct CheckDupStep *pDupStep_t=0;
                
                pType = (struct CheckDupType *)(* m_poCheckDupTypeData);
                pStep = (struct CheckDupStep *)(* m_poCheckDupStepData);

                i=1;
                while(i<=m_poCheckDupTypeData->getTotal())
                {
                        pDupType_t = pType + i;
                        m_poCheckDupStepIndex->get(pDupType_t->m_iDupType,&j);
                        pDupStep_t = pStep + j;
                        while(1)
                        {
                                if(pDupStep_t->bTimeChecked == true)
                                        bTimeChecked = true;
                                if(pDupStep_t->m_iNextOffset == 0)//一个dup_type 的dup_step 扫描结束
                                {
                                        bExistNeedCheckTime = bExistNeedCheckTime || bTimeChecked;
                                        bExistNotNeedCheckTime = bExistNotNeedCheckTime || !bTimeChecked;
                                        bTimeChecked = false;
                                        break;
                                }
                                else
                                        pDupStep_t = pStep + pDupStep_t->m_iNextOffset;
                        }
                        if(pDupType_t->m_iNextOffset == 0)//一个process 的duptype   扫描结束
                        {
                                if (bExistNotNeedCheckTime)
                                        pDupType_t->m_bTimeNotCheckFlag = true;
                                if (bExistNeedCheckTime && bExistNotNeedCheckTime) //如果共存
                                pDupType_t->m_bTimeAllCheckFlag = true;
                                bExistNeedCheckTime = false;
                                bExistNotNeedCheckTime = false;
                        }
                        i++;
                }
        }
        
        memset(sSql, 0 , 2048);
        TOCIQuery qryShotTicket(Environment::getDBConn());
        sprintf(sSql," select a.process_id, C.DUP_TYPE from B_DUP_DATA_PATH A ,B_DUP_TYPE_RELATION B ,B_DUP_TICKET_TYPE C "
                     "   WHERE  A.PROCESS_TYPE_ID=B.PROCESS_TYPE_ID  "
                    "   and B.DUP_TYPE=C.DUP_TYPE AND C.NEED_DO_OTHER=1");
        qryShotTicket.setSQL(sSql);
        qryShotTicket.open();
        while(qryShotTicket.next())
        {
                j = m_poShotTicketData->malloc();
                if(!j)
                        THROW(MBC_ParamInfoMgr+164);
                pShotTicket[j].m_iProcessID = qryShotTicket.field("PROCESS_ID").asInteger();
                pShotTicket[j].m_iDupType = qryShotTicket.field("DUP_TYPE").asInteger();
                pShotTicket[j].m_iNextOffset = 0;
                pShotTicket[j].m_bNeedJudgeCross = FALSE;
                pShotTicket[j].m_bTimeNotCheckFlag = FALSE;
                pShotTicket[j].m_bTimeAllCheckFlag = FALSE;

                unsigned int iOffset = 0;
                if(m_poShotTicketIndex->get(pShotTicket[j].m_iProcessID,&iOffset))
                {       
                        pShotTicket[j].m_iNextOffset = iOffset;
                        m_poShotTicketIndex->add(pShotTicket[j].m_iProcessID,j);
                }
                else
                        m_poShotTicketIndex->add(pShotTicket[j].m_iProcessID,j);
        }
        qryShotTicket.close();

        
        memset(sSql, 0 , 2048);
        TOCIQuery qryUndoProcess(Environment::getDBConn());
        sprintf(sSql,"select  * from b_undocheck_process where dup_type not in "
                " (select dup_type FROM b_dup_ticket_type where need_do_other is not null "
                  " and need_do_other <>0 ) ");
        //modified by luwz  :added  "and need_do_other <>0"
        qryUndoProcess.setSQL(sSql);
        qryUndoProcess.open();
        while(qryUndoProcess.next())
        {
                j = m_poUndoProcessData->malloc();
                if(!j)
                        THROW(MBC_ParamInfoMgr+165);
                pUndoProcess[j].process_id  = qryUndoProcess.field("PROCESS_ID").asInteger();
                pUndoProcess[j].dup_type = qryUndoProcess.field("DUP_TYPE").asInteger();
                pUndoProcess[j].condition_id = qryUndoProcess.field("condition_id").asInteger();
                pUndoProcess[j].m_iNextOffset = 0;

                unsigned int iOffset = 0;
                if(m_poUndoProcessIndex->get(pUndoProcess[j].process_id,&iOffset))
                {       
                        pUndoProcess[j].m_iNextOffset = iOffset;
                        m_poUndoProcessIndex->add(pUndoProcess[j].process_id,j);
                }
                else
                        m_poUndoProcessIndex->add(pUndoProcess[j].process_id,j);
        }
        qryUndoProcess.close();

        
        memset(sSql, 0 , 2048);
        TOCIQuery qryUndoProcShot(Environment::getDBConn());
        sprintf(sSql,"select  * from b_undocheck_process where dup_type  in "
                " (select dup_type FROM b_dup_ticket_type where need_do_other is not null "
                " and need_do_other <>0) ");
        qryUndoProcShot.setSQL(sSql);
        qryUndoProcShot.open();
        while(qryUndoProcShot.next())
        {
                j = m_poUndoProcShotData->malloc();
                if(!j)
                        THROW(MBC_ParamInfoMgr+166);
                pUndoProcShot[j].process_id  = qryUndoProcShot.field("PROCESS_ID").asInteger();
                pUndoProcShot[j].dup_type = qryUndoProcShot.field("DUP_TYPE").asInteger();
                pUndoProcShot[j].condition_id = qryUndoProcShot.field("condition_id").asInteger();
                pUndoProcShot[j].m_iNextOffset = 0;

                unsigned int iOffset = 0;
                if(m_poUndoProcShotIndex->get(pUndoProcShot[j].process_id,&iOffset))
                {       
                        pUndoProcShot[j].m_iNextOffset = iOffset;
                        m_poUndoProcShotIndex->add(pUndoProcShot[j].process_id,j);
                }
                else
                        m_poUndoProcShotIndex->add(pUndoProcShot[j].process_id,j);
        }
        qryUndoProcShot.close();

        memset(sSql, 0 , 2048);
        TOCIQuery qryUndoServ(Environment::getDBConn());
        sprintf(sSql,"select SERV_ID,nvl(DUP_TYPE,-1) DUP_TYPE ,nvl(EVENT_TYPE,-1) EVENT_TYPE "
   " from B_DUP_UNDOCHECK_SERV order by SERV_ID,DUP_TYPE,EVENT_TYPE ");
        qryUndoServ.setSQL(sSql);
        qryUndoServ.open();
        while(qryUndoServ.next())
        {
                j = m_poUndoServData->malloc();
                if(!j)
                        THROW(MBC_ParamInfoMgr+167);
                pUndoServ[j].lServID  = long(qryUndoServ.field("SERV_ID").asFloat());
                pUndoServ[j].iDupType = qryUndoServ.field("DUP_TYPE").asInteger();
                pUndoServ[j].iEventType = qryUndoServ.field("EVENT_TYPE").asInteger(); 
        }
        qryUndoServ.close();

        memset(sSql, 0 , 2048);
        TOCIQuery qryUndoProduct(Environment::getDBConn());
        sprintf(sSql," select nvl(OFFER_ID,-1) OFFER_ID,nvl(PROD_BUNDLE_ID,-1) PROD_BUNDLE_ID, "
              "  nvl(PRODUCT_ID,-1) PRODUCT_ID ,nvl(EVENT_TYPE,-1) EVENT_TYPE,  "
               "  nvl(TRUNK_IN,'NULL') TRUNK_IN,nvl(TRUNK_OUT,'NULL') TRUNK_OUT, "
               " nvl(DUP_TYPE,-1) DUP_TYPE from B_DUP_UNDOCHECK_PRODUCT  "
              " order by OFFER_ID,PROD_BUNDLE_ID,PRODUCT_ID,EVENT_TYPE,DUP_TYPE ");
        qryUndoProduct.setSQL(sSql);
        qryUndoProduct.open();
        while(qryUndoProduct.next())
        {
                j = m_poUndoProductData->malloc();
                if(!j)
                        THROW(MBC_ParamInfoMgr+168);
                UndoProduct[j].iOfferID  = qryUndoProduct.field("OFFER_ID").asInteger();
                UndoProduct[j].iProdBundleID = qryUndoProduct.field("PROD_BUNDLE_ID").asInteger();
                UndoProduct[j].iProductID = qryUndoProduct.field("PRODUCT_ID").asInteger();
                UndoProduct[j].iEventType = qryUndoProduct.field("EVENT_TYPE").asInteger();
                strcpy(UndoProduct[j].sTrunkIn, qryUndoProduct.field("TRUNK_IN").asString() );
                AllTrim(UndoProduct[j].sTrunkIn);
                strcpy(UndoProduct[j].sTrunkOut, qryUndoProduct.field("TRUNK_OUT").asString() );
                AllTrim(UndoProduct[j].sTrunkOut);
                UndoProduct[j].iDupType = qryUndoProduct.field("DUP_TYPE").asInteger();
        }
        qryUndoProduct.close();
        
            Log::log(0, "CheckDup参数上载正常结束. CheckDupCfg记录数: %d ", m_poCheckDupCfgData->getCount());
        
}

void ParamInfoMgr::loadNetworkPriority()
{
        int iNetworkCnt;
        char cCntNetworkBuf[32];

        NetworkPriority *pNetWork;
        unsigned int i;
        
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_NETWORKPRIORITY_INFO_NUM, cCntNetworkBuf))
        {
                THROW(MBC_ParamInfoMgr+149);
        }
        iNetworkCnt = atoi(cCntNetworkBuf);
        
        if(!(m_poNetworkPriorData->exist()))
                m_poNetworkPriorData->create(iNetworkCnt * m_fRatio);
        if(!(m_poNetworkPriorIndex->exist()))
                m_poNetworkPriorIndex->create(iNetworkCnt * m_fRatio);
		checkMem(m_poNetworkPriorData->getCount() * sizeof(NetworkPriority));
        pNetWork = (NetworkPriority *)(*m_poNetworkPriorData);

        char sSqlNetwork[1024];
        memset(sSqlNetwork, 0 , 1024);
        TOCIQuery qryNetwork(Environment::getDBConn());

        sprintf(sSqlNetwork,"SELECT A.NETWORK_SOURCE_ID, A.NETWORK_ID, A.PRIORITY "
                  "FROM b_network_priority A "
                  "ORDER BY NETWORK_SOURCE_ID, PRIORITY ");
        
        qryNetwork.setSQL(sSqlNetwork);
        qryNetwork.open();
        while(qryNetwork.next())
        {
                i=m_poNetworkPriorData->malloc();
                if(!i)
                  THROW(MBC_ParamInfoMgr+150);
                pNetWork[i].m_iNetworkSourceID = qryNetwork.field(0).asInteger() ;
                pNetWork[i].m_iNetworkID = qryNetwork.field(1).asInteger() ;
                pNetWork[i].m_iPriority = qryNetwork.field(2).asInteger() ;
                pNetWork[i].m_poNext = 0;
                pNetWork[i].m_iNextOffset = 0;

                unsigned int iOffset = 0;
                if(m_poNetworkPriorIndex->get(pNetWork[i].m_iNetworkSourceID,&iOffset))
                {       
                        pNetWork[i].m_iNextOffset = iOffset;
                        m_poNetworkPriorIndex->add(pNetWork[i].m_iNetworkSourceID,i);
                }
                else
                        m_poNetworkPriorIndex->add(pNetWork[i].m_iNetworkSourceID,i);
        }
        qryNetwork.close();
    Log::log(0, "NetworkPriority参数上载正常结束. 记录数: %d", m_poNetworkPriorData->getCount());

}

void ParamInfoMgr::loadInstTableList()
{
        int iInstCnt,iTypeCnt;
        char  cCntInstBuf[32],cCntTypeBuf[32];
        
        InstTableList *pInst = 0;
        EventTableType *pType = 0;
        unsigned int i,j;

        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_INSTTABLELIST_INFO_NUM, cCntInstBuf))
        {
                THROW(MBC_ParamInfoMgr+146);
        }
        iInstCnt= atoi(cCntInstBuf);
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TABLETYPE_INFO_NUM, cCntTypeBuf))
        {
                THROW(MBC_ParamInfoMgr+147);
        }
        iTypeCnt= atoi(cCntTypeBuf);


        if(!(m_poInstTableData->exist()))
        m_poInstTableData->create(iInstCnt * m_fRatio);
        if(!(m_poInstTableIndex->exist()))
                m_poInstTableIndex->create(iInstCnt * m_fRatio,20);
        if(!(m_poTableTypeData->exist()))
        m_poTableTypeData->create(iTypeCnt * m_fRatio);
        if(!(m_poTableTypeIndex->exist()))
                m_poTableTypeIndex->create(iTypeCnt * m_fRatio);

        pInst = (InstTableList *)(*m_poInstTableData);
        pType = (EventTableType *)(*m_poTableTypeData);
		checkMem(m_poInstTableData->getCount() * sizeof(InstTableList));
		checkMem(m_poTableTypeData->getCount() * sizeof(EventTableType));
        char sSql[4096];

    sprintf (sSql, "SELECT "
                  "NVL(BILLING_CYCLE_ID,-2) BILLING_CYCLE_ID, "
                  "NVL(BILLFLOW_ID,-2) BILLFLOW_ID, "
                  "TABLE_TYPE, "
                  "NVL(ORG_ID,-2) ORG_ID, "
                  "TABLE_NAME , NVL(OP_ROLE,'00') OP_ROLE "
                  "FROM B_INST_TABLE_LIST_CFG "
                  " ORDER BY NVL(BILLING_CYCLE_ID,-2), "
            "BILLFLOW_ID,TABLE_TYPE,NVL(ORG_ID,-2)");

        int iCnt;
        char key[20];

        DEFINE_QUERY(qry);
  DEFINE_QUERY(qry1);

        char sSql1[4096];

        sprintf (sSql1, "SELECT TABLE_NAME "
                                "FROM B_INST_TABLE_LIST_CFG A, "
                        "(SELECT nvl(RB_TABLE_TYPE_ID,0) RB_TABLE_TYPE_ID FROM "
                        "B_EVENT_TABLE_TYPE B WHERE B.EVENT_TABLE_TYPE_ID "
                        "=:EVENT_TABLE_TYPE_ID ) B "
                        "WHERE A.TABLE_TYPE = B.RB_TABLE_TYPE_ID "
                        "AND NVL(A.BILLFLOW_ID,-2)=:BILLFLOW_ID "
                        "AND A.ORG_ID=:ORG_ID "
                        "AND A.BILLING_CYCLE_ID=:BILLING_CYCLE_ID "
                        "AND NVL(A.OP_ROLE,'00')=:OP_ROLE "); 

        qry.setSQL(sSql);
        qry.open();

        iCnt = 0;

        while (qry.next())
                iCnt++; 

        if (iCnt == 0){
                printf("ParamInfoMgr::loadInstTableList[B_INST_TABLE_LIST_CFG表为空]\n");
                Log::log(0,"ParamInfoMgr::loadInstTableList[B_INST_TABLE_LIST_CFG表为空]\n");
                return;
        }

        qry.close();
        qry.setSQL( sSql);
        qry.open();

        while(qry.next()){

                i = m_poInstTableData->malloc();
                if(!i)
                        THROW(MBC_ParamInfoMgr+147);
        
                pInst[i].iBillingCycleID = qry.field("BILLING_CYCLE_ID").asInteger();
                pInst[i].iBillFlowID = qry.field("BILLFLOW_ID").asInteger();
                pInst[i].iTableType = qry.field("TABLE_TYPE").asInteger();
                pInst[i].iOrgID = qry.field("ORG_ID").asInteger();
                strcpy(pInst[i].sTableName,qry.field("TABLE_NAME").asString());
                strcpy(pInst[i].m_sRole,qry.field("OP_ROLE").asString());
                pInst[i].m_iNextOffset = 0;
                
        qry1.setSQL(sSql1);
        qry1.setParameter("EVENT_TABLE_TYPE_ID",pInst[i].iTableType);            
        qry1.setParameter("BILLFLOW_ID",pInst[i].iBillFlowID);
        qry1.setParameter("ORG_ID",pInst[i].iOrgID);
        qry1.setParameter("BILLING_CYCLE_ID",pInst[i].iBillingCycleID);
        qry1.setParameter("OP_ROLE",pInst[i].m_sRole);

        qry1.open();
        if(qry1.next())
        {
            strcpy(pInst[i].sTableNameRB,qry1.field(0).asString());
        }else
        {
            memset(pInst[i].sTableNameRB,0,sizeof(pInst[i].sTableNameRB));
        }
        qry1.close();

                sprintf (key, "%dB%dT%d", pInst[i].iBillingCycleID,pInst[i].iBillFlowID,pInst[i].iTableType);
                unsigned int iOffset = 0;
                if(m_poInstTableIndex->get(key,&iOffset))
                {
                        pInst[i].m_iNextOffset = iOffset;               
                        m_poInstTableIndex->add(key,i);
                }       
                else
                        m_poInstTableIndex->add(key,i);


        }
        
        qry.close();
    sprintf(sSql,"SELECT   EVENT_TABLE_TYPE_ID  ,  PREFIX_TABLE_NAME  , "
                " NVL(RB_TABLE_TYPE_ID,-2) RB_TABLE_TYPE_ID ,  NVL(EVENT_PRICE_TABLE_TYPE_ID,-2) EVENT_PRICE_TABLE_TYPE_ID,     "
                " NVL(PARENT_TABLE_TYPE_ID,-2) PARENT_TABLE_TYPE_ID ,  NVL(DB_FLAG,-2) DB_FLAG FROM "
              " B_EVENT_TABLE_TYPE  ");
    qry.setSQL(sSql);
    qry.open();
    while(qry.next())
    {
        j = m_poTableTypeData->malloc();
                if(!j)
                        THROW(MBC_ParamInfoMgr+148);
        pType[j].iTableTypeID = qry.field("EVENT_TABLE_TYPE_ID").asInteger();
        pType[j].iRbTableTypeID = qry.field("RB_TABLE_TYPE_ID").asInteger();
        pType[j].iPriceTableTypeID = qry.field("EVENT_PRICE_TABLE_TYPE_ID").asInteger();
        pType[j].iParentTableTypeID = qry.field("PARENT_TABLE_TYPE_ID").asInteger();     
        pType[j].iDBFlag = qry.field("DB_FLAG").asInteger();     
                strcpy(pType[j].sPrefixTableName,qry.field("PREFIX_TABLE_NAME").asString());
                
                m_poTableTypeIndex->add((long)pType[j].iTableTypeID,j);
    }
    qry.close();
    
        Log::log(0, "账期实例表相关参数参数上载正常结束. 记录数: insttable: %d  tabletype: %d", 
                m_poInstTableData->getCount(),m_poTableTypeData->getCount());

}

void ParamInfoMgr::loadParseRule()
{
        int iRuleCnt, iCombCnt;

    char  cCntRuleBuf[32], cCntCombBuf[32];
    ParseRule  *pRule  = 0;
    ParseComb  *pComb  = 0;
    unsigned int i,k;

        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_PARSERULE_INFO_NUM, cCntRuleBuf))
    {
        THROW(MBC_ParamInfoMgr+1);
    }
    iRuleCnt= atoi(cCntRuleBuf);
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_PARSECOMB_INFO_NUM, cCntCombBuf))
    {
        THROW(MBC_ParamInfoMgr+2);
    }
    iCombCnt= atoi(cCntCombBuf);


    if(!(m_poParseRuleData->exist()))
        m_poParseRuleData->create(iRuleCnt * m_fRatio);
        if(!(m_poParseRuleIndex->exist()))
                m_poParseRuleIndex->create(iRuleCnt * m_fRatio,30);
    if(!(m_poParseCombData->exist()))
        m_poParseCombData->create(iCombCnt * m_fRatio);
        if(!(m_poParseCombIndex->exist()))
                m_poParseCombIndex->create(iCombCnt * m_fRatio);
    checkMem(m_poParseRuleData->getCount() * sizeof(ParseRule));
	checkMem(m_poParseCombData->getCount() * sizeof(ParseComb));         
    pRule  = (ParseRule *)(*m_poParseRuleData);
    pComb  = (ParseComb *)(*m_poParseCombData);


        char sSqlRule[2048];
    memset(sSqlRule, 0 , 2048);
        TOCIQuery qryRule(Environment::getDBConn());
        sprintf(sSqlRule,"  Select tlv_tag_id, begin_pos, DATA_LEN, "
                " parse_func_id, attr_id, bit_position,parse_rule_group_id "
                " From b_parse_rule "
                " Where parse_rule_group_id in "
                "(select parse_rule_group_id  from b_process_parse_combine ) "
                " Order By parse_rule_group_id,tlv_tag_id, begin_pos ");

        qryRule.setSQL(sSqlRule);
        qryRule.open();
        i =0 ;
        while(qryRule.next())
        {
                i=m_poParseRuleData->malloc();
                if(!i)
                        THROW(MBC_ParamInfoMgr+3);

                pRule[i].m_iTag = qryRule.field(0).asInteger();
                pRule[i].m_iBeginPos = qryRule.field(1).asInteger();
                pRule[i].m_iDataLen = qryRule.field(2).asInteger();
                pRule[i].m_iFuncID = qryRule.field(3).asInteger();
                pRule[i].m_iAttrID = qryRule.field(4).asInteger();
                pRule[i].m_iBitPosition = qryRule.field(5).asInteger();
                pRule[i].m_iParseGroupID = qryRule.field(6).asInteger();
                pRule[i].m_pNext=0;
                pRule[i].m_iNextOffset=0;

                //# 当bit位有效时, 转换函数强行使用3, 数据长度强行置为1个字节
        if (pRule[i].m_iBitPosition>=1 && pRule[i].m_iBitPosition<=8) {
            pRule[i].m_iFuncID = 3;
            pRule[i].m_iDataLen = 1;
        }
        
        if (pRule[i].m_iTag<1 || pRule[i].m_iTag>MAX_TLV_FLAGS) {
            Log::log (0,"上载解析规则异常, 系统定义TAG位范围(1--%d) \n"
                "当前参数配置的TAG标识为: %d ",
                MAX_TLV_FLAGS, pRule[i].m_iTag
            );
            THROW (100100004);
        }
        
        
                char sKey[30]={0};
                sprintf(sKey,"%d_%d",pRule[i].m_iParseGroupID,pRule[i].m_iTag);
                unsigned int iOffset = 0;
                if(m_poParseRuleIndex->get(sKey,&iOffset))
                {
                        pRule[i].m_iNextOffset = iOffset;               
                        m_poParseRuleIndex->add(sKey,i);
                }       
                else
                        m_poParseRuleIndex->add(sKey,i);
                        
        }
        qryRule.close();

        char sSqlComb[2048];
    memset(sSqlComb, 0, 2048);
    TOCIQuery qryComb(Environment::getDBConn());
        sprintf(sSqlComb," SELECT PROCESS_ID,nvl(PARSE_RULE_GROUP_ID,0) PARSE_RULE_GROUP_ID "
                        " FROM B_PROCESS_PARSE_COMBINE "
                        " ORDER BY PROCESS_ID,PARSE_RULE_GROUP_ID DESC ");
        qryComb.setSQL(sSqlComb);
        qryComb.open();
        k = 0;
        while(qryComb.next())
        {
                k = m_poParseCombData->malloc();
                if(!k)
                THROW(MBC_ParamInfoMgr+4);
                pComb[k].m_iProcessID = qryComb.field(0).asInteger();
                pComb[k].m_iParseGroupID = qryComb.field(1).asInteger();
                pComb[k].m_iParseGroupOffset = 0;
                
                //检查是否有默认解析规则组0
        if (pComb[k].m_iParseGroupID == 0) 
                {       
                        TOCIQuery qry(Environment::getDBConn());
                qry.setSQL (" select count(*) from B_PARSE_RULE_GROUP "
            " where parse_rule_group_id = 0" );
                qry.open();
                qry.next();
                if (!qry.field(0).asInteger()) 
                        {
                Log::log (0, "当前进程(%d)未定义解析规则组,也未找到默认(即ID=0的)解析规则组",pComb[k].m_iProcessID);
                Log::log (0, "相关配置表有:B_PARSE_RULE_GROUP,B_PROCESS_PARSE_COMBINE,B_PARSE_RULE");
                THROW (100100003);
                }
                qry.close();
        }
                
                m_poParseCombIndex->add((long)pComb[k].m_iProcessID, k);                
        }

        qryComb.close();
        Log::log(0, "接收模块参数上载正常结束. 记录数: Combine: %d/%d  Rule: %d/%d", 
                m_poParseCombData->getCount(),iCombCnt,m_poParseRuleData->getCount(),iRuleCnt);
        
}


void ParamInfoMgr::loadAttrTrans()
{
    int iGroupCnt, iStepCnt, iRuleCnt, iCombCnt;
    char sSqlGroup[1024] = {0};
    TOCIQuery qryGroup(Environment::getDBConn());
    char cCntGroupbuf[32], cCntStepBuf[32], cCntRuleBuf[32], cCntCombBuf[32];
    
    FormatGroup *pGroup = 0;
    FormatStep  *pStep  = 0;
    FormatRule  *pRule  = 0;
    FormatComb  *pComb  = 0;
    unsigned int i, j, k, l;
    Log::log(0, "开始属性规整参数上载");
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FORMATGROUP_NUM, cCntGroupbuf))
    {
        THROW(MBC_ParamInfoMgr+127);
    }
    iGroupCnt = atoi(cCntGroupbuf);
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FORMATSTEP_NUM, cCntStepBuf))
    {
        THROW(MBC_ParamInfoMgr+128);
    }
    iStepCnt= atoi(cCntStepBuf);
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FORMATRULE_NUM, cCntRuleBuf))
    {
        THROW(MBC_ParamInfoMgr+129);
    }
    iRuleCnt= atoi(cCntRuleBuf);
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FORMATCOMB_NUM, cCntCombBuf))
    {
        THROW(MBC_ParamInfoMgr+130);
    }
    iCombCnt= atoi(cCntCombBuf);

    if(!(m_poFormatGroupData->exist()))
        m_poFormatGroupData->create(iGroupCnt * m_fRatio);
    if(!(m_poFormatStepData->exist()))
        m_poFormatStepData->create(iStepCnt * m_fRatio);
    if(!(m_poFormatRuleData->exist()))
        m_poFormatRuleData->create(iRuleCnt * m_fRatio);
    if(!(m_poFormatCombData->exist()))
        m_poFormatCombData->create(iCombCnt * m_fRatio);
    pGroup = (FormatGroup *)(*m_poFormatGroupData);
    pStep  = (FormatStep *)(*m_poFormatStepData);
    pRule  = (FormatRule *)(*m_poFormatRuleData);
    pComb  = (FormatComb *)(*m_poFormatCombData);

	checkMem(m_poFormatGroupData->getCount() * sizeof(FormatGroup));
	checkMem(m_poFormatStepData->getCount() * sizeof(FormatStep));
	checkMem(m_poFormatRuleData->getCount() * sizeof(FormatRule));
	checkMem(m_poFormatCombData->getCount() * sizeof(FormatComb));
    sprintf(sSqlGroup, " SELECT DISTINCT FORMAT_ID FORMAT_ID "
                " FROM B_ATTRIBUTE_FORMAT "
                " WHERE FORMAT_ID IN "
                " (SELECT FORMAT_ID FROM B_ATTRIBUTE_FORMAT_COMBIN) "
                " ORDER BY FORMAT_ID ");

    qryGroup.setSQL(sSqlGroup);
    qryGroup.open();
    while(qryGroup.next())  //所有规整规则组循环(不分进程,取的时候再自己判断)
    {
        i = m_poFormatGroupData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+131); 

        pGroup[i].m_iFormatID = qryGroup.field(0).asInteger();
        pGroup[i].m_iFormatStepOffset = 0;
        pGroup[i].m_iFormatStepCnt = 0;

        char sSqlStep[1024];
        memset(sSqlStep, 0 , 1024);
        TOCIQuery qryStep(Environment::getDBConn());
        sprintf(sSqlStep, " SELECT STEP_SEQ, FORMAT_ID, STEP_NAME "
            " FROM B_ATTRIBUTE_FORMAT_STEP "
            " WHERE FORMAT_ID = :FORMAT_ID "
            " ORDER BY STEP_ORDER ");

        qryStep.setSQL(sSqlStep);
        qryStep.setParameter("FORMAT_ID", pGroup[i].m_iFormatID);
        qryStep.open();
        while(qryStep.next())  //当前规整组的规整步骤循环
        {
            j = m_poFormatStepData->malloc();
            if(!j)
                THROW(MBC_ParamInfoMgr+132); 

            pGroup[i].m_iFormatStepOffset = j - pGroup[i].m_iFormatStepCnt;
            pGroup[i].m_iFormatStepCnt = pGroup[i].m_iFormatStepCnt + 1;

            pStep[j].m_iStepSeq = qryStep.field(0).asInteger();
            pStep[j].m_iFormatID = qryStep.field(1).asInteger();
            strcpy(pStep[j].m_sStepName, qryStep.field(2).asString());
            pStep[j].m_iFormatRuleOffset = 0;
            pStep[j].m_iFormatRuleCnt = 0;

            char sSqlRule[1024];
            memset(sSqlRule, 0, 1024);
            TOCIQuery qryRule(Environment::getDBConn());
            sprintf(sSqlRule, " SELECT RULE_SEQ, NVL(OPERATION_ID, -1) OPERATION_ID, "
                " RULE_NAME, NVL(CONDITION_ID, -1) CONDITION_ID, EXIT_FLAG, "
                " NVL(IN_EVENT_TYPE, -1) IN_EVENT_TYPE, NVL(OUT_EVENT_TYPE, -1) OUT_EVENT_TYPE, "
                " NVL(TO_CHAR(EFF_DATE, 'YYYYMMDDHH24MISS'), '0') EFF_DATE, "
                " NVL(TO_CHAR(EXP_DATE, 'YYYYMMDDHH24MISS'), '9') EXP_DATE, "
                " TRUNC(SYSDATE - NVL(EFF_DATE, SYSDATE - 1000)) EFFLEN, "
                " TRUNC(NVL(EXP_DATE, SYSDATE + 1000) - SYSDATE) EXPLEN "
                " FROM B_ATTRIBUTE_FORMAT_RULE "
                " WHERE FORMAT_ID = :FORMAT_ID AND STEP_SEQ = :STEP_SEQ "
                " ORDER BY RULE_SEQ ");

            qryRule.setSQL(sSqlRule);
            qryRule.setParameter("FORMAT_ID", pStep[j].m_iFormatID);
            qryRule.setParameter("STEP_SEQ", pStep[j].m_iStepSeq);
            qryRule.open();
            while(qryRule.next()) //属性规整规则循环
            {
                k = m_poFormatRuleData->malloc();
                if(!k)
                    THROW(MBC_ParamInfoMgr+133);

                pStep[j].m_iFormatRuleOffset = k - pStep[j].m_iFormatRuleCnt;
                pStep[j].m_iFormatRuleCnt = pStep[j].m_iFormatRuleCnt + 1;

                pRule[k].m_iRuleSeq = qryRule.field(0).asInteger();
                pRule[k].m_iOperationID = qryRule.field(1).asInteger();
                strcpy(pRule[k].m_sRuleName, qryRule.field(2).asString());
                pRule[k].m_iConditionID = qryRule.field(3).asInteger();
                pRule[k].m_iExitFlag = qryRule.field(4).asInteger();
                pRule[k].m_iInEventType = qryRule.field(5).asInteger();
                pRule[k].m_iOutEventType = qryRule.field(6).asInteger();
                strncpy(pRule[k].m_sEffDate, qryRule.field(7).asString(), 14);
                pRule[k].m_sEffDate[14] = '\0';
                strncpy(pRule[k].m_sExpDate, qryRule.field(8).asString(), 14);
                pRule[k].m_sExpDate[14] = '\0';
                if(qryRule.field(9).asInteger() > 365)
                    strcpy(pRule[k].m_sEffDate, "0");
                if(qryRule.field(10).asInteger() > 365)
                    strcpy(pRule[k].m_sExpDate, "9");
            }
            qryRule.close();
        }
        qryStep.close();
    }
    qryGroup.close();

    char sSqlComb[1024];
    memset(sSqlComb, 0, 1024);
    TOCIQuery qryComb(Environment::getDBConn());
    sprintf(sSqlComb, " SELECT SOURCE_EVENT_TYPE, FORMAT_ID, PROCESS_ID "
        " FROM B_ATTRIBUTE_FORMAT_COMBIN "
        " ORDER BY SOURCE_EVENT_TYPE ");
    qryComb.setSQL(sSqlComb);
    qryComb.open();
    while(qryComb.next())
    {
        l = m_poFormatCombData->malloc();
        if(!l)
            THROW(MBC_ParamInfoMgr+134);

        pComb[l].m_iSourceEventType = qryComb.field(0).asInteger();
        pComb[l].m_iFormatID = qryComb.field(1).asInteger();
        pComb[l].m_iProcessID = qryComb.field(2).asInteger();
        pComb[l].m_iFormatGroupOffset = 0;

        int iCnt = m_poFormatGroupData->getCount();
        for(int n = 1; n <= iCnt; n++)
        {
            if(pComb[l].m_iFormatID == pGroup[n].m_iFormatID)
            {
                pComb[l].m_iFormatGroupOffset = n;
                break;
            }
        }
    }
    qryComb.close();
    Log::log(0, "\t数据区：规整组   记录数/总数: %d/%d", m_poFormatGroupData->getCount(),m_poFormatGroupData->getTotal());
    Log::log(0, "\t        规整步骤 记录数/总数: %d/%d", m_poFormatStepData->getCount(),m_poFormatStepData->getTotal());
    Log::log(0, "\t        规整规则 记录数/总数: %d/%d", m_poFormatRuleData->getCount(),m_poFormatRuleData->getTotal());
    Log::log(0, "\t        规整组合 记录数/总数: %d/%d", m_poFormatCombData->getCount(),m_poFormatCombData->getTotal());

}


void ParamInfoMgr::loadTrunk()
{
    int iCount = 0;
    Trunk *pTrunk = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    sprintf(sSqlCode, " SELECT SWITCH_ID||'+'||TRUNK_CODE||'+'||'0' SKEY, "
                " TRUNK_TYPE_ID, TO_NUMBER(NEED_BILLING) NEED_BILLING "
                " FROM B_TRUNK  WHERE TRUNK_SIDE <> '1' "
                " UNION "
                " SELECT SWITCH_ID||'+'||TRUNK_CODE||'+'||'1' SKEY,  "
                " TRUNK_TYPE_ID, TO_NUMBER(NEED_BILLING) NEED_BILLING  "   
                " FROM B_TRUNK WHERE TRUNK_SIDE <> '0' ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TRUNK_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+502);
    }
    iCount = atoi(countbuf);

    if(!(m_poTrunkData->exist()))
        m_poTrunkData->create(iCount * m_fRatio);
    if(!(m_poTrunkIndex->exist()))
        m_poTrunkIndex->create(iCount * m_fRatio, 32); //索引字符串长度最长为32个字符
    pTrunk = (Trunk *)(*m_poTrunkData);
	checkMem(m_poTrunkData->getCount() * sizeof(Trunk));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlCode);
    qry.open();

    while(qry.next())
    {
        i = m_poTrunkData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+503);
		memset(&pTrunk[i],'\0',sizeof(Trunk));
        char sKey[30] = {0};
        strncpy(sKey, qry.field(0).asString(), 29);
        pTrunk[i].m_iTrunkTypeID = qry.field(1).asInteger();
        pTrunk[i].m_iNeedBilling = qry.field(2).asInteger();

        m_poTrunkIndex->add(sKey, i);
    }
    qry.close();
    Log::log(0, "Trunk参数上载正常结束. 记录数/总数: %d/%d", m_poTrunkData->getCount(),m_poTrunkData->getTotal());
    Log::log(0, "Trunk索引上载正常结束. 记录数/总数: %d/%d", m_poTrunkIndex->getCount(),m_poTrunkIndex->getTotal());
}

void ParamInfoMgr::loadAreaCode()
{
    int iCount = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    sprintf(sSqlCode, " SELECT AREA_CODE, nvl(LONG_GROUP_TYPE_ID,0) FROM B_AREA_CODE ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_AREACODE_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+504);
    }
    iCount = atoi(countbuf);

    if(!(m_poAreaCodeIndex->exist()))
        m_poAreaCodeIndex->create(iCount * m_fRatio + 5, 12); //借用这个索引存放一下int数据

    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlCode);
    qry.open();

    while(qry.next())
    {
        m_poAreaCodeIndex->add(qry.field(0).asString(), (unsigned int)qry.field(1).asInteger());
    }
    qry.close();
	//得出最大最小长度值
	int m_iMaxAreaCodeLen,m_iMinAreaCodeLen;
	qry.setSQL("select max(length(area_code)) max_len, min(length(area_code)) min_len  from b_area_code");
    qry.open();
    if (qry.next()) {
        m_iMaxAreaCodeLen = qry.field("max_len").asInteger();
        m_iMinAreaCodeLen = qry.field("min_len").asInteger();
    }
    else {
        m_iMaxAreaCodeLen = 0;
        m_iMinAreaCodeLen = 0;
    }
	char sKey[10] = {0};
	strcpy(sKey,"max");
	m_poAreaCodeIndex->add(sKey,(unsigned int )m_iMaxAreaCodeLen);
	sKey[0] = '\0';
	strcpy(sKey,"min");
	m_poAreaCodeIndex->add(sKey,(unsigned int )m_iMinAreaCodeLen);
	//
	//## 取得区号匹配规则信息(是否循环匹配、循环区号的选择)
    char sValue[1000];
    sValue[0] = 0;
    ParamDefineMgr::getParam("PRE_PROCESS","AREACODE_CYC_FIT",sValue);
    int m_bAreaCodeCycleFit = atoi(sValue);
    
    sValue[0] = 0;
    ParamDefineMgr::getParam("PRE_PROCESS","SEL_AREA_PART",sValue);
    int m_iSelAreaPart = atoi(sValue);
    
    sValue[0] = 0;
    ParamDefineMgr::getParam("FORMAT_METHOD","TRIM_MOBILE_HEAD_ZERO", sValue);
    int m_iTrimMobHeadZero = atoi(sValue); 
	char *pKey = "CycleFit";
	m_poAreaCodeIndex->add(pKey,(unsigned int )m_bAreaCodeCycleFit);
	pKey = "SelAreaPart";
	m_poAreaCodeIndex->add(pKey,(unsigned int )m_iSelAreaPart);
	pKey = "MobHeadZero";
	m_poAreaCodeIndex->add(pKey,(unsigned int )m_iTrimMobHeadZero);
	//
    Log::log(0, "B_AREA_CODE索引上载正常结束. 记录数/总数: %d/%d", m_poAreaCodeIndex->getlCount(),m_poAreaCodeIndex->getlTotal());
	Log::log(0, "最大长度/最小长度 %d/%d",m_iMaxAreaCodeLen,m_iMinAreaCodeLen);
}

void ParamInfoMgr::loadHLR()
{
    int iCount = 0;
    HLR *pHLR = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    sprintf(sSqlCode, " SELECT CODE_SEGMENT, AREA_CODE FROM B_HLR ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_HLR_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+505);
    }
    iCount = atoi(countbuf);

    if(!(m_poHLRData->exist()))
        m_poHLRData->create(iCount * m_fRatio); 
    if(!(m_poHLRIndex->exist()))
        m_poHLRIndex->create(iCount * m_fRatio+2, 10);//这个参数要借用来做点事
    pHLR = (HLR *)(*m_poHLRData);
	checkMem(m_poHLRData->getCount() * sizeof(HLR));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlCode);
    qry.open();

    while(qry.next())
    {
        i = m_poHLRData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+506);
		memset(&pHLR[i],'\0',sizeof(HLR));
        strcpy(pHLR[i].m_sAreaCode, qry.field(1).asString());

        m_poHLRIndex->add(qry.field(0).asString(), i);
    }
    qry.close();
	//
	int m_iMaxSegCodeLen,m_iMinSegCodeLen;
	qry.setSQL("select max(length(code_segment)) max_len,min(length(code_segment)) min_len from b_hlr");
    qry.open();
    if (qry.next()) {
        m_iMaxSegCodeLen = qry.field("max_len").asInteger();
        m_iMinSegCodeLen = qry.field("min_len").asInteger();
    }
    else {
        m_iMaxSegCodeLen = 0;
        m_iMinSegCodeLen = 0;
    }
	qry.close();
	char *p = "max";
	m_poHLRIndex->add(p,(unsigned int )m_iMaxSegCodeLen);
	p = "min";
	m_poHLRIndex->add(p,(unsigned int )m_iMinSegCodeLen);
	
    Log::log(0, "B_HLR参数上载正常结束. 记录数/总数: %d/%d", m_poHLRData->getCount(),m_poHLRData->getTotal());
	Log::log(0, "B_HLR索引上载正常结束. 记录数/总数: %d/%d", m_poHLRIndex->getCount(),m_poHLRIndex->getTotal());
}

//加载拜访低地区码[补充数据从400开始抛异常]
void ParamInfoMgr::loadVisitAreaCode()
{
    int iCount = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    sprintf(sSqlCode,"select trim(msc)||decode(lac,null,'',('-'||trim(lac)||decode(cell_id,null,'','-'||trim(cell_id)))) sKey, "
        " trim(area_code) area_code from b_visit_area_code");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_VISIT_AREAVODE_INFO_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+401);
    }
    iCount = atoi(countbuf);

    if(!(m_poVisitAreaCodeRuleData->exist()))
        m_poVisitAreaCodeRuleData->create(iCount * m_fRatio); 
    if(!(m_pVisitAreaCodeIndex->exist()))
        m_pVisitAreaCodeIndex->create(iCount * m_fRatio, 100);
    VisitAreaCodeRule *pVisitAreaCodeRule = (VisitAreaCodeRule *)(*m_poVisitAreaCodeRuleData);
	checkMem(m_poVisitAreaCodeRuleData->getCount() * sizeof(VisitAreaCodeRule));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlCode);
    qry.open();

    while(qry.next())
    {
        i = m_poVisitAreaCodeRuleData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+402);
		memset(&pVisitAreaCodeRule[i],'\0',sizeof(VisitAreaCodeRule));
        strcpy(pVisitAreaCodeRule[i].m_sAreaCode, qry.field("area_code").asString());
        m_pVisitAreaCodeIndex->add(qry.field("sKey").asString(), i);
    }
    qry.close();
    Log::log(0, "VisitAreaCodeRule 参数上载正常结束. 记录数/总数: %d/%d", m_poVisitAreaCodeRuleData->getCount(),m_poVisitAreaCodeRuleData->getTotal());
	Log::log(0, "VisitAreaCodeRule 索引上载正常结束. 记录数/总数: %d/%d", m_pVisitAreaCodeIndex->getlCount(),m_pVisitAreaCodeIndex->getlTotal());
}

void ParamInfoMgr::loadEdgeRoamEx()
{
	int iCount = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    sprintf(sSqlCode,"select trim(msc)||decode(lac,null,'',('-'||trim(lac)||decode(cell_id,null,'','-'||trim(cell_id)))) sKey, "
        " trim(area_code) area_code from b_edge_roam");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_EDGEROAMRULE_INFO_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+403);
    }
    iCount = atoi(countbuf);

    if(!(m_poEdgeRoamRuleData->exist()))
        m_poEdgeRoamRuleData->create(iCount * m_fRatio); 
    if(!(m_pEdgeRoamIndex->exist()))
        m_pEdgeRoamIndex->create(iCount * m_fRatio, 32);
    EdgeRoamRule *pEdgeRoamRule = (EdgeRoamRule *)(*m_poEdgeRoamRuleData);
	checkMem(m_poEdgeRoamRuleData->getCount() * sizeof(EdgeRoamRule));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlCode);
    qry.open();

    while(qry.next())
    {
        i = m_poEdgeRoamRuleData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+404);
		memset(&pEdgeRoamRule[i],'\0',sizeof(EdgeRoamRule));
        strcpy(pEdgeRoamRule[i].m_sAreaCode, qry.field("area_code").asString());
        m_pEdgeRoamIndex->add(qry.field("sKey").asString(), i);
    }
    qry.close();
    Log::log(0, "EdgeRoamRule[边界漫游规则] 参数上载正常结束. 记录数/总数: %d/%d", m_poEdgeRoamRuleData->getCount(),m_poEdgeRoamRuleData->getTotal());
	Log::log(0, "EdgeRoamRule[边界漫游规则] 索引上载正常结束. 记录数/总数: %d/%d", m_pEdgeRoamIndex->getlCount(),m_pEdgeRoamIndex->getlTotal()); 
}

//对应SegBillingMgr::loadTrunkSeg
unsigned int ParamInfoMgr::loadPNSegBilling(char *sWhere,int &iSegBillingCnt)
{
	int iCount1 = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_PNSEGBILLING_INFO_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+405);
    }
    iCount1 = atoi(countbuf);
    if(!(m_poPNSegBillingData->exist()))
        m_poPNSegBillingData->create(iCount1 * m_fRatio); 
	checkMem(m_poPNSegBillingData->getCount() * sizeof(PNSegBilling));
    PNSegBilling *pPNSegBilling = (PNSegBilling *)(*m_poPNSegBillingData);
	unsigned int iCount = m_poPNSegBillingData->getCount();
    // 数据区填值,并建立索引
    unsigned int i = 0;
	sprintf(sSqlCode," select  area_code, begin_acc_nbr, end_acc_nbr, acc_nbr, "
        " to_char(eff_date,'yyyymmddhh24miss') eff_date, "
        " to_char(exp_date,'yyyymmddhh24miss') exp_date, "
        " to_number(nvl(seg_flag,0)) seg_flag, "
		" nvl(switch_id,0) switch_id, "
		" nvl(trunk_in,'') trunk_in "
        " from b_pn_seg_billing "
		" %s "
        " order by begin_acc_nbr, end_acc_nbr ",sWhere);
    qry.setSQL(sSqlCode);
    qry.open();
	int iSeq = 1;
	unsigned int iRes = 0;
	char sMaxEndNBRAhead[MAX_CALLING_NBR_LEN] = {0};
    while(qry.next())
    {
		i = m_poPNSegBillingData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+406);
		if(iSeq == 1)
		{
			iRes = i; // 返回的首偏移
			iSeq++;
		}
		memset(&pPNSegBilling[i],'\0',sizeof(PNSegBilling));
        strcpy (pPNSegBilling[i].m_sAreaCode, qry.field("area_code").asString());
        strcpy (pPNSegBilling[i].m_sBeginNBR, qry.field("begin_acc_nbr").asString());
        strcpy (pPNSegBilling[i].m_sEndNBR, qry.field("end_acc_nbr").asString());       
        strcpy (pPNSegBilling[i].m_sAccNBR, qry.field("acc_nbr").asString());
        strcpy (pPNSegBilling[i].m_sEffDate, qry.field("eff_date").asString());
        strcpy (pPNSegBilling[i].m_sExpDate, qry.field("exp_date").asString());
        pPNSegBilling[i].m_iSegFlag = qry.field("seg_flag").asInteger();
		//
		pPNSegBilling[i].m_lSwitchID = qry.field("switch_id").asLong();
		strcpy (pPNSegBilling[i].m_sTrunkIn, qry.field("trunk_in").asString());
		//
        strcpy (pPNSegBilling[i].m_sMaxEndNBRAhead, sMaxEndNBRAhead);   
        if (strcmp (pPNSegBilling[i].m_sEndNBR, sMaxEndNBRAhead) >0 )
            strcpy (sMaxEndNBRAhead, pPNSegBilling[i].m_sEndNBR);
    }
    qry.close();
	iSegBillingCnt = (int)(m_poPNSegBillingData->getCount()-iCount);
	return iRes;
}

/*
函数说明: 获取不重复的SwitchID以及TrunkIn 保证数据不重复
参数说明: sWhere SQL语句中的条件语句 pSwitchIDTrunkInRec获取的不重复的SwitchID以及TrunkIn的结果保留
返回值: 实际查到的不重复的SwitchID以及TrunkIn的数目
举例:  SwitchIDTrunkInRec *pSwitchIDTrunkInRec=0;int iNum = getSwitchIDTrunkInRec(" where switch_id is not null and trunk_in is null ",pSwitchIDTrunkInRec);
*/
int ParamInfoMgr::getSwitchIDTrunkInRec(char *sWhere,SwitchIDTrunkInRec *&pSwitchIDTrunkIn)
{
	SwitchIDTrunkInRec *pSwitchIDTrunkInRec = 0;
	TOCIQuery qry(Environment::getDBConn());  
	qry.close();
	char sql[1024] = {0};
	sprintf (sql, "select count(*) from b_pn_seg_billing %s ", sWhere);
    qry.setSQL(sql);   
	qry.open ();
	int iRec = 0;
	if(qry.next())
	{
		iRec = qry.field(0).asInteger();
		if(iRec>0)
		{
			pSwitchIDTrunkInRec = new SwitchIDTrunkInRec[iRec];
			memset(pSwitchIDTrunkInRec,'\0',sizeof(SwitchIDTrunkInRec)*iRec);
		}
	}
	qry.close();
	if(!pSwitchIDTrunkInRec)
	{
		return 0; //不用加载了 没有数据 直接退出了 后面也没其余的加载了
	}
	sql[0] = '\0';
	sprintf (sql, "select distinct nvl(switch_id,-1), nvl(trunk_in,'') from b_pn_seg_billing %s ", sWhere);
	qry.setSQL (sql);
	qry.open();
	int iRecPos = 0;
	while(qry.next())
	{
		 pSwitchIDTrunkInRec[iRecPos].m_lSwitchID = qry.field(0).asLong();
		 strcpy(pSwitchIDTrunkInRec[iRecPos].m_sTrunkIn,qry.field(1).asString());
		 iRecPos++;
	}
	iRec = iRecPos; //准确值
	qry.close();
	pSwitchIDTrunkIn = pSwitchIDTrunkInRec;
	return iRec;
}

/*
函数说明: loadTrunkTreeNode 加载TrunkTreeNode需要
参数说明: 
返回值: 
举例: 
*/
void ParamInfoMgr::loadTrunkTreeNode()
{
	unsigned int iCountBefore = 0;
	if(m_poPNSegBillingData->exist())
		iCountBefore = m_poPNSegBillingData->getCount();
	
	int iCount = 0;
    char sSqlCode[1024] = {0};
    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TRUNKTREENODE_INFO_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+407);
    }
    iCount = atoi(countbuf);
	
	if(!(m_pSwtichTrunkTree->exist())) //
       m_pSwtichTrunkTree->create(iCount * m_fRatio + 30,65); 
	if(!(m_poTrunkTreeNodeData->exist()))
        m_poTrunkTreeNodeData->create(iCount * m_fRatio + 30); 
	checkMem(m_poTrunkTreeNodeData->getCount() * sizeof(TrunkTreeNode));
    TrunkTreeNode *pTrunkTreeNode = (TrunkTreeNode *)(*m_poTrunkTreeNodeData);
	PNSegBilling *pPNSegBilling = (PNSegBilling *)(*m_poPNSegBillingData);
	int iRec,iRec2,iRecPos = 0;
	char *pWhere = 0;
	char *p = 0;
	//先获取所有索引的最大值总和,再申请索引空间
	int iRecTmp = 0;
	//这是不全是NULL,where switch_id is not null and trunk_in is null
	pWhere = " where switch_id is not null and trunk_in is null ";
	//SwitchIDTrunkInRec *pSwitchRec = 0;
	iRec = getSwitchIDTrunkInRec(pWhere,pSwitchRec);
	//累积
	iRecTmp += iRec;
	//
	//先搜索出 SWITCH_ID 以及 TRUCK_IN信息
	pWhere = " where switch_id is not null and trunk_in is not null ";
	//SwitchIDTrunkInRec *pSwitchTrunkRec = 0;
	iRec2 = getSwitchIDTrunkInRec(pWhere,pSwitchTrunkRec);
	//
	iRecTmp += iRec2;
	
	
	unsigned int i = 0;
	//这是全是NULL的
	i = m_poTrunkTreeNodeData->malloc();
    if(!i)
    	THROW(MBC_ParamInfoMgr+408);
	memset(&pTrunkTreeNode[i],'\0',sizeof(TrunkTreeNode));
	p = " where switch_id is null and trunk_in is null ";
	pTrunkTreeNode[i].m_iSegArray = loadPNSegBilling(p,pTrunkTreeNode[i].m_iSegBillingCnt);
	p = "null_null"; //<switch_id,trunk_in>
	m_pSwtichTrunkTree->add(p,i);
	
	
	if(pSwitchRec)
	{
		//加载每个KEY对应的数据
		iRecPos = 0;
		for(;iRecPos<iRec;iRecPos++)
		{
			i = m_poTrunkTreeNodeData->malloc();
    		if(!i)
    			THROW(MBC_ParamInfoMgr+408);
			memset(&pTrunkTreeNode[i],'\0',sizeof(TrunkTreeNode));
			char sSQLWhere[1024] = {0};
			sprintf(sSQLWhere," where switch_id = %ld and trunk_in is null ",pSwitchRec[iRecPos].m_lSwitchID);
			pTrunkTreeNode[i].m_iSegArray = loadPNSegBilling(sSQLWhere,pTrunkTreeNode[i].m_iSegBillingCnt);
			char sKey[32] = {0};
			sprintf (sKey,"%ld_", pSwitchRec[iRecPos].m_lSwitchID);
			m_pSwtichTrunkTree->add(sKey,i);
		}
	}
	
	//这个EXIST_EXIST的处理稍微不一样
	//先搜索出 SWITCH_ID 以及 TRUCK_IN信息
	//SwitchIDTrunkInRec *pSwitchTrunkRec = 0;
	//pWhere = " switch_id is not null and trunk_in is not null ";
	//iRec2 = getSwitchIDTrunkInRec(pWhere,pSwitchTrunkRec);
	//加载每个KEY对应的数据
	if(pSwitchTrunkRec)
	{
		iRecPos = 0;
		for(;iRecPos<iRec2;iRecPos++)
		{
			//这是全不是NULL,where switch_id is not null and trunk_in is not null
			i = m_poTrunkTreeNodeData->malloc();
    		if(!i)
    			THROW(MBC_ParamInfoMgr+408);
			memset(&pTrunkTreeNode[i],'\0',sizeof(TrunkTreeNode));
			char sSQLDesc[1024] = {0};
			sprintf (sSQLDesc, " where switch_id = %ld and trunk_in = '%s'", pSwitchTrunkRec[iRecPos].m_lSwitchID, pSwitchTrunkRec[iRecPos].m_sTrunkIn);
			pTrunkTreeNode[i].m_iSegArray = loadPNSegBilling(sSQLDesc,pTrunkTreeNode[i].m_iSegBillingCnt);
		
			char sKey[65] = {0};
			sprintf (sKey,"%ld_%s", pSwitchTrunkRec[iRecPos].m_lSwitchID, pSwitchTrunkRec[iRecPos].m_sTrunkIn);
			unsigned int iOff = 0;
			if(!m_pSwtichTrunkTree->get(sKey,&iOff))
			{ 
				m_pSwtichTrunkTree->add(sKey,i); 			
			} else {
				//这种情况不符合原有程序逻辑,是不能存在的
				THROW(MBC_ParamInfoMgr+409);
			}
		}
	}
	//
	Log::log(0, "TrunkTreeNode_PNSegBilling 参数上载正常结束. PNSegBillin数据记录数/总数: %d/%d" ,m_poPNSegBillingData->getCount()-iCountBefore,m_poPNSegBillingData->getTotal());
	Log::log(0, "TrunkTreeNode 参数上载正常结束. TrunkTreeNode数据记录数/总数: %d/%d" ,m_poTrunkTreeNodeData->getCount(),m_poTrunkTreeNodeData->getTotal());
	Log::log(0, "TrunkTreeNode 参数上载正常结束. SwitchTrunk索引记录数/总数: %d/%d" ,m_pSwtichTrunkTree->getlCount(),m_pSwtichTrunkTree->getlTotal());
}

//  
void ParamInfoMgr::loadPNSegBillingEx()
{
	int iCount = 0;
	//loadPNSegBilling(p,iCount);
	int iCount1 = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_PNSEGBILLING_INFO_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+409);
    }
    iCount1 = atoi(countbuf);
    if(!(m_aPNSegBillingData->exist()))
        m_aPNSegBillingData->create(iCount1 * m_fRatio); 
	checkMem(m_aPNSegBillingData->getCount() * sizeof(PNSegBilling));
    PNSegBilling *pPNSegBilling = (PNSegBilling *)(*m_aPNSegBillingData);
	//unsigned int iCount = m_aPNSegBillingData->getCount();
    // 数据区填值,并建立索引
    unsigned int i = 0;
	sprintf(sSqlCode," select  area_code, begin_acc_nbr, end_acc_nbr, acc_nbr, "
        " to_char(eff_date,'yyyymmddhh24miss') eff_date, "
        " to_char(exp_date,'yyyymmddhh24miss') exp_date, "
        " to_number(nvl(seg_flag,0)) seg_flag, "
		" nvl(switch_id,0) switch_id, "
		" nvl(trunk_in,'') trunk_in "
        " from b_pn_seg_billing "
        " order by begin_acc_nbr, end_acc_nbr ");
    qry.setSQL(sSqlCode);
    qry.open();
	int iSeq = 1;
	unsigned int iRes = 0;
	char sMaxEndNBRAhead[MAX_CALLING_NBR_LEN] = {0};
    while(qry.next())
    {
		i = m_aPNSegBillingData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+410);
		memset(&pPNSegBilling[i],'\0',sizeof(PNSegBilling));
        strcpy (pPNSegBilling[i].m_sAreaCode, qry.field("area_code").asString());
        strcpy (pPNSegBilling[i].m_sBeginNBR, qry.field("begin_acc_nbr").asString());
        strcpy (pPNSegBilling[i].m_sEndNBR, qry.field("end_acc_nbr").asString());       
        strcpy (pPNSegBilling[i].m_sAccNBR, qry.field("acc_nbr").asString());
        strcpy (pPNSegBilling[i].m_sEffDate, qry.field("eff_date").asString());
        strcpy (pPNSegBilling[i].m_sExpDate, qry.field("exp_date").asString());
        pPNSegBilling[i].m_iSegFlag = qry.field("seg_flag").asInteger();
		//
		pPNSegBilling[i].m_lSwitchID = qry.field("switch_id").asLong();
		strcpy (pPNSegBilling[i].m_sTrunkIn, qry.field("trunk_in").asString());
		//
        strcpy (pPNSegBilling[i].m_sMaxEndNBRAhead, sMaxEndNBRAhead);   
        if (strcmp (pPNSegBilling[i].m_sEndNBR, sMaxEndNBRAhead) >0 )
            strcpy (sMaxEndNBRAhead, pPNSegBilling[i].m_sEndNBR);
    }
    qry.close();
	Log::log(0, "b_pn_seg_billing 不分条件 参数上载正常结束. 数据记录数/总数: %d/%d" ,m_aPNSegBillingData->getCount(),m_aPNSegBillingData->getTotal());
}

//
void ParamInfoMgr::loadNotPNSegBilling()
{
	int iCount = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_PNNOTBILLING_INFO_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+411);
    }
    iCount = atoi(countbuf);

    if(!(m_poPNNotBillingData->exist()))
        m_poPNNotBillingData->create(iCount * m_fRatio); 
	if(!(m_poPNNotBillingIndex->exist()))
        m_poPNNotBillingIndex->create(iCount * m_fRatio, 32); //这个字段会借用来存储一下最大以及最小字段长
	checkMem(m_poPNNotBillingData->getCount() * sizeof(strPNNotBilling));
	strPNNotBilling *pNotBilling = (strPNNotBilling*)(*m_poPNNotBillingData);
    // 数据区填值,并建立索引 
    unsigned int i = 0;
	sprintf(sSqlCode, " select AREA_CODE||ACC_NBR  NBR,"
          	" to_char( EFF_DATE,'yyyymmddhh24miss') EFF_DATE," 
    	    " to_char( EXP_DATE,'yyyymmddhh24miss') EXP_DATE "
    	    " from b_pn_seg_not_billing ");
    qry.setSQL(sSqlCode);
    qry.open();
    
    while(qry.next())
    {
		i = m_poPNNotBillingData->malloc();
    	if(!i)
    		THROW(MBC_ParamInfoMgr+412);
		memset(&pNotBilling[i],'\0',sizeof(strPNNotBilling));
		strncpy (pNotBilling[i].m_sEffDate, qry.field("EFF_DATE").asString(), 15);
        strncpy (pNotBilling[i].m_sExpDate, qry.field("EXP_DATE").asString(), 15 );
        m_poPNNotBillingIndex->add(qry.field("NBR").asString(), i);
    }
    qry.close();
	Log::log(0, "PNNotBilling 参数上载正常结束. 数据记录数/总数: %d/%d", m_poPNNotBillingData->getCount(),m_poPNNotBillingData->getTotal());
    Log::log(0, "PNNotBilling 参数上载正常结束. 索引记录数/总数: %d/%d", m_poPNNotBillingIndex->getlCount(),m_poPNNotBillingIndex->getlTotal());
}

void ParamInfoMgr::loadCarrier()
{
    int iCount = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    sprintf(sSqlCode, " SELECT PRECODE, CARRIER_TYPE_ID FROM B_CARRIER ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_CARRIER_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+413);
    }
    iCount = atoi(countbuf);

    if(!(m_poCarrierIndex->exist()))
        m_poCarrierIndex->create(iCount * m_fRatio + 2, 32); //这个字段会借用来存储一下最大以及最小字段长

    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlCode);
    qry.open();
    
    while(qry.next())
    {
        m_poCarrierIndex->add(qry.field(0).asString(), (unsigned int)qry.field(1).asInteger());
    }
    qry.close();
	unsigned int m_iMaxPreCodeLen,m_iMinPreCodeLen;
	qry.setSQL("select max(length(precode)) max_len, min(length(precode)) min_len  from b_carrier");
    qry.open();
    if (qry.next()) {
        m_iMaxPreCodeLen = (unsigned int)qry.field("max_len").asInteger();
        m_iMinPreCodeLen = (unsigned int)qry.field("min_len").asInteger();
    }
    else {
        m_iMaxPreCodeLen = 0;
        m_iMinPreCodeLen = 0;
    }
	qry.close();
	char *p = "max";
	m_poCarrierIndex->add(p,m_iMaxPreCodeLen);
	p = "min";
	m_poCarrierIndex->add(p,m_iMinPreCodeLen);
    Log::log(0, "CARRIER参数上载正常结束. 记录数/总数: %d/%d", m_poCarrierIndex->getCount(),m_poCarrierIndex->getTotal());

}



void ParamInfoMgr::loadServiceInfo2()
{
    int iCount = 0;
    TServiceInfo2 *pServiceInfo2= 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    sprintf(sSqlCode, " SELECT SERVICE_TYPE_ID, PARTY_ROLE_ID, ORG_ID, TO_CHAR(ORG_ID) S_ORG_ID "
                      " FROM B_SP_SERVICE ");
	
    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SERVICE_INFO2_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+414);
    }
    iCount = atoi(countbuf);
	
    if(!(m_poServiceInfo2Data->exist()))
        m_poServiceInfo2Data->create(iCount * m_fRatio); 
    if(!(m_poServiceInfo2Index->exist()))
        m_poServiceInfo2Index->create(iCount * m_fRatio + 3,32);
    pServiceInfo2 = (TServiceInfo2 *)(*m_poServiceInfo2Data);
	checkMem(m_poServiceInfo2Data->getCount() * sizeof(TServiceInfo2));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlCode);
    qry.open();

    while(qry.next())
    {
        i = m_poServiceInfo2Data->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+415);
        char sOrgIDKey[10] = {0};
        pServiceInfo2[i].m_iServiceTypeID = qry.field(0).asInteger();
        pServiceInfo2[i].m_iPartyRoleID   = qry.field(1).asInteger();
        pServiceInfo2[i].m_iOrgID         = qry.field(2).asInteger();
        strcpy(sOrgIDKey, qry.field(3).asString());
        m_poServiceInfo2Index->add(sOrgIDKey, i);
    }
    qry.close();
	
	char sTempValue[10] = {0};
    int m_iCallingOrgIDRule = 0;
    int m_iCalledOrgIDRule = 0;
    if (ParamDefineMgr::getParam ("FORMAT_METHOD","CALLING_ORG_ID_RULE", sTempValue) )
        m_iCallingOrgIDRule = atoi (sTempValue);
    
    memset (sTempValue, 0, 10);
    if (ParamDefineMgr::getParam ("FORMAT_METHOD","CALLED_ORG_ID_RULE", sTempValue) )
        m_iCalledOrgIDRule = atoi (sTempValue);
	
	char *pKey = "m_iCalledOrgIDRule";
	m_poServiceInfo2Index->add(pKey, (unsigned int)m_iCalledOrgIDRule);
	pKey = "m_iCallingOrgIDRule";
	m_poServiceInfo2Index->add(pKey, (unsigned int)m_iCallingOrgIDRule);
	
    Log::log(0, "TServiceInfo2(B_SP_SERVICE) 参数上载正常结束. 数据记录数/总数: %d/%d", m_poServiceInfo2Data->getCount(),m_poServiceInfo2Data->getTotal());
	Log::log(0, "TServiceInfo2(B_SP_SERVICE) 参数上载正常结束. 索引记录数/总数: %d/%d", m_poServiceInfo2Index->getCount(),m_poServiceInfo2Index->getTotal());
}

void ParamInfoMgr::loadTrunkBilling()
{
    int iCount = 0;
    TrunkBilling *pTrunkBilling = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TRUNKBILLING_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+416);
    }
    iCount = atoi(countbuf);

    if(!(m_poTrunkBillingData->exist()))
        m_poTrunkBillingData->create(iCount * m_fRatio); 
    if(!(m_poTrunkBillingIndex->exist()))
        m_poTrunkBillingIndex->create(iCount * m_fRatio, 32);
    pTrunkBilling = (TrunkBilling *)(*m_poTrunkBillingData);
	checkMem(m_poTrunkBillingData->getCount() * sizeof(TrunkBilling));
#ifdef  DEF_SICHUAN
    sprintf(sSqlCode, " SELECT SWITCH_ID, GATHER_POT,TRUNK_CODE, "
            " TO_CHAR(EFF_DATE, 'yyyymmddhh24miss') EFF_DATE, "
            " TO_CHAR(EXP_DATE, 'yyyymmddhh24miss') EXP_DATE, "
            " ACC_NBR, TO_NUMBER(TRUNK_SIDE) TRUNK_SIDE, "
            " NVL(SWITCH_ID, -1) || '+' || NVL(GATHER_POT, -1) || '+' || TRUNK_CODE SKEY "
            " FROM B_TRUNK_BILLING ORDER BY SWITCH_ID ");
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlCode);
    qry.open();

    while(qry.next())
    {
        i = m_poTrunkBillingData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+417);
		memset(&pTrunkBilling[i],'\0',sizeof(TrunkBilling));
        char sKey[30] = {0};
        pTrunkBilling[i].m_iSwitch_id = qry.field(0).asInteger();
        pTrunkBilling[i].m_iGatherPot = qry.field(1).asInteger();
        strcpy(pTrunkBilling[i].m_sTrunk_Code, qry.field(2).asString());
        strcpy(pTrunkBilling[i].m_sEffDate, qry.field(3).asString());
        strcpy(pTrunkBilling[i].m_sExpDate, qry.field(4).asString());
        strcpy(pTrunkBilling[i].m_sAccNBR, qry.field(5).asString());
        pTrunkBilling[i].m_iTrunkSide = qry.field(6).asInteger();
        strcpy(sKey, qry.field(7).asString());

        m_poTrunkBillingIndex->add(sKey, i);
    }
#else
    sprintf(sSqlCode, "  SELECT SWITCH_ID, TRUNK_CODE, "
        " TO_CHAR(EFF_DATE, 'yyyymmddhh24miss') EFF_DATE, "
        " TO_CHAR(EXP_DATE, 'yyyymmddhh24miss') EXP_DATE, "
        " ACC_NBR, TO_NUMBER(TRUNK_SIDE) TRUNK_SIDE, "
        " SWITCH_ID || '+' || TRUNK_CODE SKEY "
        "FROM B_TRUNK_BILLING ORDER BY SWITCH_ID ");
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlCode);
    qry.open();

    while(qry.next())
    {
        i = m_poTrunkBillingData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+417);
		memset(&pTrunkBilling[i],'\0',sizeof(TrunkBilling));
        char sKey[30] = {0};
        pTrunkBilling[i].m_iSwitch_id = qry.field(0).asInteger();
        strcpy(pTrunkBilling[i].m_sTrunk_Code, qry.field(1).asString());
        strcpy(pTrunkBilling[i].m_sEffDate, qry.field(2).asString());
        strcpy(pTrunkBilling[i].m_sExpDate, qry.field(3).asString());
        strcpy(pTrunkBilling[i].m_sAccNBR, qry.field(4).asString());
        pTrunkBilling[i].m_iTrunkSide = qry.field(5).asInteger();
        strcpy(sKey, qry.field(6).asString());
        m_poTrunkBillingIndex->add(sKey, i);
    }
#endif
    
    qry.close();
    Log::log(0, "B_TRUNK_BILLING参数上载正常结束. 数据记录数/总数: %d/%d", m_poTrunkBillingData->getCount(),m_poTrunkBillingData->getTotal());
	Log::log(0, "B_TRUNK_BILLING参数上载正常结束. 索引记录数/总数: %d/%d", m_poTrunkBillingIndex->getCount(),m_poTrunkBillingIndex->getTotal());
}

//  
void ParamInfoMgr::loadSpecTrunkProduct()
{
#ifdef DEF_SHANXI
	int iCount = 0;
    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SPEC_TRUNK_PRODUCT_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+418);
    }
    iCount = atoi(countbuf);
    if(!(m_poSpecTrunkProductIndex->exist()))
        m_poSpecTrunkProductIndex->create(iCount * m_fRatio);
    DEFINE_QUERY(qry);
    qry.close();
    qry.setSQL("select product_id, state from b_spec_trunk_product");
    qry.open();
    while(qry.next())
	{
    	if(qry.field(1).asString()[2] == 'A')
		{
    		m_poSpecTrunkProductIndex->add( qry.field(0).asInteger(), 1 );
    	}
    }
    qry.close();
	Log::log(0, "b_spec_trunk_product 参数上载正常结束. 索引记录数/总数: %d/%d", m_poSpecTrunkProductIndex->getCount(),m_poSpecTrunkProductIndex->getTotal());
#endif
}

void ParamInfoMgr::loadSwitchItemType()
{
	int iCount = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SWITCHITEMTYPE_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+419);
    }
    iCount = atoi(countbuf);
	if(!(m_poSwitchItemTypeData->exist()))
        m_poSwitchItemTypeData->create(iCount * m_fRatio);
    if(!(m_poSwitchItemTypeIndex->exist()))
        m_poSwitchItemTypeIndex->create(iCount * m_fRatio,32);
	checkMem(m_poSwitchItemTypeData->getCount() * sizeof(SwitchItemType));
	SwitchItemType *pSwitchItemType = (SwitchItemType*)(*m_poSwitchItemTypeData);
    qry.close();
    qry.setSQL("select switch_id||'+'||nvl(tariff_type,-1)||bill_mode skey, switch_id,  "
        " nvl(tariff_type,-1) tariff_type, event_type_id, acct_item_type_id, bill_mode "
        " from b_switch_item_type ");
    qry.open();
	unsigned int i = 0;
    while(qry.next())
	{
		i = m_poSwitchItemTypeData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+420);
		memset(&pSwitchItemType[i],'\0',sizeof(SwitchItemType));
    	char sKey[30] = {0};
        pSwitchItemType[i].m_iSwitchID = qry.field("switch_id").asInteger();
        pSwitchItemType[i].m_iTariffType = qry.field("tariff_type").asInteger();
        pSwitchItemType[i].m_iEventTypeID = qry.field("event_type_id").asInteger();
        pSwitchItemType[i].m_iAcctItemTypeID = qry.field("acct_item_type_id").asInteger();
        pSwitchItemType[i].m_iBillMode = qry.field("bill_mode").asInteger();       
        strcpy(sKey, qry.field("skey").asString());
        m_poSwitchItemTypeIndex->add(sKey, i);
    }
    qry.close();
	Log::log(0, "b_switch_item_type 参数上载正常结束. 参数记录数/总数: %d/%d", m_poSwitchItemTypeData->getCount(),m_poSwitchItemTypeData->getTotal());
	Log::log(0, "b_switch_item_type 参数上载正常结束. 索引记录数/总数: %d/%d", m_poSwitchItemTypeIndex->getCount(),m_poSwitchItemTypeIndex->getTotal());
}

void ParamInfoMgr::loadLocalAreaCode()
{
	int iCount = 0;
	unsigned int i = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_STR_LOCALAREACODE_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+421);
    }
    iCount = atoi(countbuf);
	if(!(m_poStrLocalAreaCodeData->exist()))
        m_poStrLocalAreaCodeData->create(iCount * m_fRatio);
	checkMem(m_poStrLocalAreaCodeData->getCount() * sizeof(strLocalAreaCode));
	strLocalAreaCode *pStrLocalAreaCode = (strLocalAreaCode*)(*m_poStrLocalAreaCodeData);
    qry.close();
    qry.setSQL("select area_code from b_area_code where long_group_type_id=10 order by area_code");
    qry.open();
    while(qry.next())
	{
		i = m_poStrLocalAreaCodeData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+422);
		memset(&pStrLocalAreaCode[i],'\0',sizeof(strLocalAreaCode));
		strcpy(pStrLocalAreaCode[i].m_sAreaCode,qry.field(0).asString());
    }
    qry.close();
	Log::log(0, "b_area_code[long_group_type_id=10] 参数上载正常结束. 参数记录数/总数: %d/%d", m_poStrLocalAreaCodeData->getCount(),m_poStrLocalAreaCodeData->getTotal());
}


void ParamInfoMgr::loadTimesType()
{
    int iCount = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TIMESTYPE_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+423);
    }
    iCount = atoi(countbuf);

    if(!(m_poTimesTypeIndex->exist()))
        m_poTimesTypeIndex->create(iCount * m_fRatio); 

    // 建立索引
	sprintf(sSqlCode, " select event_type_id,times_type from b_times_type ");
    qry.setSQL(sSqlCode);
    qry.open();
    while(qry.next())
    {
        m_poTimesTypeIndex->add(qry.field("event_type_id").asInteger(), (unsigned int)qry.field("times_type").asInteger());
    }
    qry.close();
    Log::log(0, "B_TIMES_TYPE参数上载正常结束. 索引记录数/总数: %d/%d",m_poTimesTypeIndex->getCount(),m_poTimesTypeIndex->getTotal());
	
}

void ParamInfoMgr::loadCityInfo()
{
    int iCount = 0;
    CityInfo *pCityInfo = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    sprintf(sSqlCode, " SELECT AREA_CODE, PROV_CODE, REGION_CODE FROM B_CITY_INFO ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_CITYINFO_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+424);
    }
    iCount = atoi(countbuf);

    if(!(m_poCityInfoData->exist()))
        m_poCityInfoData->create(iCount * m_fRatio); 
    if(!(m_poCityInfoIndex->exist()))
        m_poCityInfoIndex->create(iCount * m_fRatio, 32);
    if(!(m_poCityAreaIndex->exist()))
        m_poCityAreaIndex->create(iCount * m_fRatio, 32); 
    pCityInfo = (CityInfo *)(*m_poCityInfoData);
	checkMem(m_poCityInfoData->getCount() * sizeof(CityInfo));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlCode);
    qry.open();

    while(qry.next())
    {
        i = m_poCityInfoData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+425);
		memset(&pCityInfo[i],'\0',sizeof(CityInfo));
        strncpy(pCityInfo[i].m_sAreaCode, qry.field(0).asString(), 32);
        strncpy(pCityInfo[i].m_sProvCode, qry.field(1).asString(), 32);
        strncpy(pCityInfo[i].m_sRegionCode, qry.field(2).asString(), 32);

        m_poCityInfoIndex->add(pCityInfo[i].m_sRegionCode, i);
        m_poCityAreaIndex->add(pCityInfo[i].m_sAreaCode, i);
    }
    
    qry.close();
    Log::log(0, "B_CITY_INFO参数上载正常结束. 数据记录数: %d/%d", m_poCityInfoData->getCount(),m_poCityInfoData->getTotal());
    Log::log(0, "B_CITY_INFO参数上载正常结束. RegionCode索引记录数: %d/%d", m_poCityInfoIndex->getCount(),m_poCityInfoIndex->getTotal());
	Log::log(0, "B_CITY_INFO参数上载正常结束. AreaCode索引记录数: %d/%d", m_poCityAreaIndex->getCount(),m_poCityAreaIndex->getTotal());
}

void ParamInfoMgr::loadFilterRuleResult()
{
    int iCount = 0;
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FILTER_RESULT_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+426);
    }
    iCount = atoi(countbuf);

    if(!(m_poRuleCheckCalledIndex->exist()))
        m_poRuleCheckCalledIndex->create(iCount * m_fRatio,MAX_EULECHECK_KEY_LEN); 
		
	if(!(m_poRuleCheckCallingIndex->exist()))
        m_poRuleCheckCallingIndex->create(iCount * m_fRatio,MAX_EULECHECK_KEY_LEN); 
		
	if(!(m_poRuleCheckAreaCodeIndex->exist()))
        m_poRuleCheckAreaCodeIndex->create(iCount * m_fRatio,MAX_EULECHECK_KEY_LEN); 
		
	if(!(m_poRuleCheckGatherPotIndex->exist()))
        m_poRuleCheckGatherPotIndex->create(iCount * m_fRatio,MAX_EULECHECK_KEY_LEN); 
		
	if(!(m_poRuleCheckTrunkOutIndex->exist()))
        m_poRuleCheckTrunkOutIndex->create(iCount * m_fRatio,MAX_EULECHECK_KEY_LEN); 
	
	if(!(m_poRuleCheckTrunkInIndex->exist()))
        m_poRuleCheckTrunkInIndex->create(iCount * m_fRatio,MAX_EULECHECK_KEY_LEN); 
	
	if(!(m_poRuleCheckSwitchIDIndex->exist()))
        m_poRuleCheckSwitchIDIndex->create(iCount * m_fRatio,MAX_EULECHECK_KEY_LEN); 
	
	if(!(m_poFilterRuleResultData->exist()))
        m_poFilterRuleResultData->create(iCount * m_fRatio);
	 
	FilterRuleResult *pFilterRuleResult = (FilterRuleResult *)(*m_poFilterRuleResultData);
	checkMem(m_poFilterRuleResultData->getCount() * sizeof(FilterRuleResult));
    // 建立索引
	qry.setSQL ("select switch_long_type,nvl(switch_id,-1),nvl(trunk_in,'*'),nvl(trunk_out,'*'),nvl( gather_pot,-1),"
			"switch_area_code, org_calling_nbr, org_called_nbr, "
			"event_type_id, nvl(eff_date, to_date('19800101', 'yyyymmdd')), "
			"nvl(exp_date, to_date('20500101', 'yyyymmdd')) from b_filter_rule");
	qry.open();
	unsigned int i = 0;
    while(qry.next())
    {
		i = m_poFilterRuleResultData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+427);
		memset(&pFilterRuleResult[i],'\0',sizeof(FilterRuleResult));
		pFilterRuleResult[i].m_iEventTypeID = qry.field (8).asInteger ();
		strcpy(pFilterRuleResult[i].m_sEffDate , qry.field (9).asString());
		strcpy(pFilterRuleResult[i].m_sExpDate , qry.field (10).asString());
		//
		char sTrunkIn[MAX_EULECHECK_KEY_LEN] = {0};
		char sTrunkOut[MAX_EULECHECK_KEY_LEN] = {0};
		char sSwitchAreaCode[MAX_EULECHECK_KEY_LEN] = {0};
		char sOrgCallingNbr[MAX_EULECHECK_KEY_LEN] = {0};
		char sOrgCalledNbr[MAX_EULECHECK_KEY_LEN] = {0};
		
		int iLongType = qry.field (0).asInteger();
		int iSwitchID = qry.field (1).asInteger ();
		strcpy(sTrunkIn,qry.field (2).asString());
		strcpy(sTrunkOut,qry.field (3).asString());
		int iGatherPot = qry.field (4).asInteger ();
		strcpy(sSwitchAreaCode,qry.field (5).asString());
		strcpy(sOrgCallingNbr,qry.field(6).asString());
		strcpy(sOrgCalledNbr,qry.field(7).asString());
		RuleCheckLongTypeInsert(iLongType,iSwitchID,sTrunkIn,sTrunkOut,iGatherPot,sSwitchAreaCode,sOrgCallingNbr,sOrgCalledNbr,i);
		
    }
    qry.close();
    Log::log(0, "b_filter_rule 参数上载正常结束. 数据记录数/总数: %d/%d",m_poFilterRuleResultData->getCount(),m_poFilterRuleResultData->getTotal());
    Log::log(0, "b_filter_rule 参数上载正常结束. Called索引记录数/总数: %d/%d",m_poRuleCheckCalledIndex->getlCount(),m_poRuleCheckCalledIndex->getlTotal());
    Log::log(0, "b_filter_rule 参数上载正常结束. Calling索引记录数/总数: %d/%d",m_poRuleCheckCallingIndex->getlCount(),m_poRuleCheckCallingIndex->getlTotal());
    Log::log(0, "b_filter_rule 参数上载正常结束. AreaCode索引记录数/总数: %d/%d",m_poRuleCheckAreaCodeIndex->getlCount(),m_poRuleCheckAreaCodeIndex->getlTotal());
    Log::log(0, "b_filter_rule 参数上载正常结束. GatherPot索引记录数/总数: %d/%d",m_poRuleCheckGatherPotIndex->getlCount(),m_poRuleCheckGatherPotIndex->getlTotal());
    Log::log(0, "b_filter_rule 参数上载正常结束. TrunkOut索引记录数/总数: %d/%d",m_poRuleCheckTrunkOutIndex->getlCount(),m_poRuleCheckTrunkOutIndex->getlTotal());
    Log::log(0, "b_filter_rule 参数上载正常结束. TrunkIn索引记录数/总数: %d/%d",m_poRuleCheckTrunkInIndex->getlCount(),m_poRuleCheckTrunkInIndex->getlTotal());
    Log::log(0, "b_filter_rule 参数上载正常结束. SwitchID索引记录数/总数: %d/%d",m_poRuleCheckSwitchIDIndex->getlCount(),m_poRuleCheckSwitchIDIndex->getlTotal());
	
}

void ParamInfoMgr::RuleCheckCalledInsert(char * sOrgCalledNbr,unsigned int iFilterRuleResult,char * sOrgCallingNbr)
{
	 char sKey[MAX_EULECHECK_KEY_LEN] = {0};
	 if(sOrgCalledNbr[0] == '*') 
	 {
	 	sprintf(sKey,"%s_%s",sOrgCallingNbr,"*");
		m_poRuleCheckCalledIndex->add(sKey,iFilterRuleResult);
	 } else {
	 	sprintf(sKey,"%s_%s",sOrgCallingNbr,sOrgCalledNbr);
		m_poRuleCheckCalledIndex->add (sKey,iFilterRuleResult);
	 }
}

void ParamInfoMgr::RuleCheckCallingInsert(char * sOrgCallingNbr, char * sOrgCalledNbr,unsigned int iFilterRuleResult,char *sSwitchAreaCode)
{
	char sKey[MAX_EULECHECK_KEY_LEN] = {0};
	unsigned int iCalled = 0;
	if (sOrgCallingNbr[0] == '*') 
	{ 
		sOrgCallingNbr[1] = '\0';
	}
	sprintf(sKey,"%s_%s",sSwitchAreaCode,sOrgCallingNbr);
	if (!m_poRuleCheckCallingIndex->get (sKey, &iCalled)) 
	{
		m_poRuleCheckCallingIndex->add (sKey, 1);
	}
	
	RuleCheckCalledInsert(sOrgCalledNbr, iFilterRuleResult,sOrgCallingNbr);	
}

void ParamInfoMgr::RuleCheckAreaCodeInsert(char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr,unsigned int iFilterRuleResult,int iGatherPot)
{
	char sKey[MAX_EULECHECK_KEY_LEN] = {0};
	unsigned int iRes = 0;	
	if (sSwitchAreaCode[0] == '*') 
	{
		sSwitchAreaCode[1] = '\0';
	}
	
	sprintf(sKey,"%d_%s",iGatherPot,sSwitchAreaCode);
	if (!m_poRuleCheckAreaCodeIndex->get (sKey, &iRes)) 
	{
		m_poRuleCheckAreaCodeIndex->add (sKey, 1);
	}
	RuleCheckCallingInsert(sOrgCallingNbr, sOrgCalledNbr, iFilterRuleResult ,sSwitchAreaCode);
}

//  
void ParamInfoMgr::RuleCheckGatherPotInsert(int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr,unsigned int iFilterRuleResult,char *sTrunkOut)
{
	unsigned int iAreaCode = 0;
	char sKey[MAX_EULECHECK_KEY_LEN] = {0};
	sprintf(sKey,"%s_%d",sTrunkOut,iGatherPot);
	if (!m_poRuleCheckGatherPotIndex->get (sKey, &iAreaCode)) 
	{
		m_poRuleCheckGatherPotIndex->add (sKey, 1);
	}
	
	RuleCheckAreaCodeInsert(sSwitchAreaCode,sOrgCallingNbr, sOrgCalledNbr, iFilterRuleResult,iGatherPot);
}

void ParamInfoMgr::RuleCheckTrunkOutInsert(char * sTrunkOut, int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, unsigned int iFilterRuleResult,char * sTrunkIn)
{
	unsigned int iGatherPotOffset = 0;
	char sKey[MAX_EULECHECK_KEY_LEN] = {0};
	if (sTrunkOut[0] == '*') 
	{
		sTrunkOut[1] = '\0';
	}
	sprintf(sKey,"%s_%s",sTrunkIn,sTrunkOut);
	
	if(!m_poRuleCheckTrunkOutIndex->get (sKey, &iGatherPotOffset)) 
	{
		m_poRuleCheckTrunkOutIndex->add (sKey, 1);
	}
	
	RuleCheckGatherPotInsert(iGatherPot,sSwitchAreaCode,sOrgCallingNbr, sOrgCalledNbr, iFilterRuleResult,sTrunkOut);
}

void ParamInfoMgr::RuleCheckTrunkInInsert(char * sTrunkIn, char * sTrunkOut, int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr,unsigned int iFilterRuleResult,int iSwitchID)
{
	unsigned int iTrunkOut = 0;
	char sKey[MAX_EULECHECK_KEY_LEN] = {0};
	if (sTrunkIn[0] == '*') 
	{
		sTrunkIn[1] = '\0';
	}
	sprintf(sKey,"%d_%s",iSwitchID,sTrunkIn);
	if (!m_poRuleCheckTrunkInIndex->get (sKey, &iTrunkOut)) 
	{
		m_poRuleCheckTrunkInIndex->add (sKey, 1);
	}
	
	RuleCheckTrunkOutInsert(sTrunkOut,iGatherPot,sSwitchAreaCode,sOrgCallingNbr, sOrgCalledNbr, iFilterRuleResult,sTrunkIn);
}

void ParamInfoMgr::RuleCheckSwitchIDInsert(int iSwitchID, char * sTrunkIn, char * sTrunkOut,  int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, unsigned int iFilterRuleResult,int iLongType)
{
	unsigned int iRuleCheckSwitchID = 0;
	char sKey[MAX_EULECHECK_KEY_LEN] = {0};
	sprintf(sKey,"%d_%d",iLongType,iSwitchID);
	if (!m_poRuleCheckSwitchIDIndex->get (sKey, &iRuleCheckSwitchID)) 
	{
		m_poRuleCheckSwitchIDIndex->add (sKey, 1);
	}

	RuleCheckTrunkInInsert(sTrunkIn,sTrunkOut,iGatherPot,sSwitchAreaCode,sOrgCallingNbr, sOrgCalledNbr, iFilterRuleResult,iSwitchID);
}

void ParamInfoMgr::RuleCheckLongTypeInsert(int iLongType, int iSwitchID, char * sTrunkIn, char * sTrunkOut,  int iGatherPot,  char *sSwitchAreaCode, char * sOrgCallingNbr, char * sOrgCalledNbr, unsigned int iFilterRuleResult)
{
	 RuleCheckSwitchIDInsert(iSwitchID,sTrunkIn,sTrunkOut,iGatherPot,sSwitchAreaCode, sOrgCallingNbr, sOrgCalledNbr, iFilterRuleResult,iLongType);
}


void ParamInfoMgr::loadOrg()
{
    int iCount = 0;
    Org *pOrg = 0;
    char sSqlcode[512] = {0};
    TOCIQuery qry(Environment::getDBConn());

    sprintf (sSqlcode, "SELECT ORG_ID, nvl(PARENT_ORG_ID, -2), "
                " nvl(ORG_LEVEL_ID, -2) FROM ORG");
    
    char countbuf[32]= {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ORG_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+101);
    }
    iCount = atoi(countbuf);

    if(!(m_poOrgData->exist()))
        m_poOrgData->create(iCount * m_fRatio);
    if(!(m_poOrgIndex->exist()))
        m_poOrgIndex->create(iCount * m_fRatio);
    pOrg= (Org *)(*m_poOrgData);
	checkMem(m_poOrgData->getCount() * sizeof(Org));
    // 数据区填值,并建立索引
    unsigned int i;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poOrgData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+102);

        pOrg[i].m_iOrgID = qry.field(0).asInteger();
        pOrg[i].m_iParentID = qry.field(1).asInteger();
        pOrg[i].m_iLevel = qry.field(2).asInteger();
        pOrg[i].m_poParent = NULL;
        pOrg[i].m_iParentOffset = 0;
        
        m_poOrgIndex->add((long)pOrg[i].m_iOrgID, i);
    }

    int iUsedCnt = m_poOrgData->getCount();
    for(int j = 1; j <= iUsedCnt; j++)
    {
        if(pOrg[j].m_iParentID != -2)
        {
            unsigned int offset = 0;
            if(m_poOrgIndex->get(pOrg[j].m_iParentID, &offset))
            {
                pOrg[j].m_iParentOffset = offset;
            }
            else
                pOrg[j].m_iParentOffset = 0;
        }
    }

    qry.close();
    Log::log(0, "ORG参数上载正常结束. 记录数: %d", m_poOrgData->getCount());
}

void ParamInfoMgr::loadOrgEx()
{
    int iCount = 0;
    OrgEx *pOrgEx= 0;
    char sSqlcode[512] = {0};
    HashList<int> *pOrgExIndex = 0;
    TOCIQuery qry(Environment::getDBConn());

    sprintf (sSqlcode, "SELECT ORG_ID, NVL(PARENT_ORG_ID, -2) PARENT_ID, "
                " NVL(ORG_LEVEL_ID, -2) LEVEL_ID,NVL(ORG_AREA_CODE, '') AREA_CODE, "
                " '1_' || ORG_ID SKEY "
                " FROM ORG  UNION       ALL "
                " SELECT REGION_ID, NVL(PARENT_REGION_ID, -2), "
                " NVL(REGION_LEVEL, -2), NVL(ORG_AREA_CODE, ''), "
                " '2_'|| REGION_ID "
                " FROM STAT_REGION UNION ALL     "
                " SELECT REGION_ID, NVL(PARENT_REGION_ID, -2), "
                " NVL(REGION_LEVEL, -2), NVL(REGION_AREA_CODE, ''), "
                " NVL(REGION_TYPE_ID, 0) || '_' || REGION_ID "
                " FROM B_SELF_DEFINE_REGION ");
    
    char countbuf[32]= {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ORGEX_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+101);
    }
    iCount = atoi(countbuf);

    pOrgExIndex = new HashList<int>(iCount * m_fRatio);
    if(!pOrgExIndex)
        THROW(MBC_ParamInfoMgr+101);
    
    if(!(m_poOrgExData->exist()))
        m_poOrgExData->create(iCount * m_fRatio);
    if(!(m_poOrgExIndex->exist()))
        m_poOrgExIndex->create(iCount * m_fRatio, 22);
    pOrgEx= (OrgEx*)(*m_poOrgExData);
	checkMem(m_poOrgExData->getCount() * sizeof(OrgEx));
    // 数据区填值,并建立索引
    unsigned int i;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poOrgExData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+102);

        pOrgEx[i].m_iOrgID    = qry.field(0).asInteger();
        pOrgEx[i].m_iParentID = qry.field(1).asInteger();
        pOrgEx[i].m_iLevel    = qry.field(2).asInteger();
        strcpy(pOrgEx[i].sAreaCode, qry.field(3).asString());
        pOrgEx[i].m_poParent = NULL;
        pOrgEx[i].m_iParentOffset = 0;

        pOrgExIndex->add(pOrgEx[i].m_iOrgID, i);
        m_poOrgExIndex->add(qry.field(4).asString(), i);
    }

    int iUsedCnt = m_poOrgExData->getCount();
    for(int j = 1; j <= iUsedCnt; j++)
    {
        if(pOrgEx[j].m_iParentID != -2)
        {
            int offset = 0;
            if(pOrgExIndex->get(pOrgEx[j].m_iParentID, &offset))
            {
                pOrgEx[j].m_iParentOffset = offset;
            }
            else
                pOrgEx[j].m_iParentOffset = 0;
        }
    }

    qry.close();
    delete pOrgExIndex;
    pOrgExIndex = 0;
    Log::log(0, "ORGEX参数上载正常结束. 记录数: %d", m_poOrgExData->getCount());
}

void ParamInfoMgr::loadOrgLongType()
{
    int iCount = 0;
    OrgLongType *pOrgLongType = 0;
    char sSqlcode[512] = {0};
    TOCIQuery qry(Environment::getDBConn());

    sprintf (sSqlcode, "SELECT ORG_ID_A, ORG_ID_B, DISTANCE_TYPE_ID FROM B_REGION_DISTANCE "
                " UNION ALL "
                " SELECT ORG_ID_B, ORG_ID_A, DISTANCE_TYPE_ID FROM B_REGION_DISTANCE");
    
    char countbuf[32]= {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ORGLONGTYPE_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+101);
    }
    iCount = atoi(countbuf);
    
    if(!(m_poOrgLongTypeData->exist()))
        m_poOrgLongTypeData->create(iCount * m_fRatio);
    if(!(m_poOrgLongTypeIndex->exist()))
        m_poOrgLongTypeIndex->create(iCount * m_fRatio);
    pOrgLongType= (OrgLongType*)(*m_poOrgLongTypeData);
	checkMem(m_poOrgLongTypeData->getCount() * sizeof(OrgLongType));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poOrgLongTypeData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+102);

        int iOrgIDA = 0;
        int iOrgIDB = 0;
        int iLongTypeID = 0;
        int iLevelCur = 0;
        int iLevelPre = 0;

        iOrgIDA     = qry.field(0).asInteger();
        iOrgIDB     = qry.field(1).asInteger();
        iLongTypeID = qry.field(2).asInteger();

        pOrgLongType[i].m_iOrg = iOrgIDA;
        pOrgLongType[i].m_iLongType = iLongTypeID;
        pOrgLongType[i].m_pNext = 0;
        pOrgLongType[i].m_iNextOffset = 0;

        unsigned int iOffset = 0;
        OrgLongType *pCur = 0;
        OrgLongType *pNext = 0;
        OrgLongType *pPre = 0;
        if(m_poOrgLongTypeIndex->get(iOrgIDB, &iOffset))//orgb以前已经加入到索引了
        {
            pPre = pOrgLongType + iOffset;  //取得该索引orgb链表的链首
            if(iOrgIDA == pPre->m_iOrg)     //同样的orga, orgb已经发现过,这里就不编入链表和索引了
            {
                continue;
            }
            else
            {
                //按照orga的level插入到该链表中
                char sKey[22] = {0};
                unsigned int offset = 0;
                OrgEx *pOrgEx= 0;
                pOrgEx = (OrgEx*)(*m_poOrgExData);

                sprintf(sKey, "%d_%d", 1, iOrgIDA);
                if(m_poOrgExIndex->get(sKey, &offset))
                {
                    iLevelCur = pOrgEx[offset].m_iLevel;
                }

                memset(sKey, 0, sizeof(sKey));
                offset = 0;
                sprintf(sKey, "%d_%d", 1, pPre->m_iOrg);
                if(m_poOrgExIndex->get(sKey, &offset))
                {
                    iLevelPre= pOrgEx[offset].m_iLevel;
                }

                if(iLevelPre < iLevelCur)
                {
                    pOrgLongType[i].m_iNextOffset = iOffset;
                    m_poOrgLongTypeIndex->add(iOrgIDB, i);
                }
                else
                {
                    bool bGet = false;
                    pCur = pPre;
                    pNext = pOrgLongType + pCur->m_iNextOffset;
                    while(pNext != pOrgLongType)
                    {
                        memset(sKey, 0, sizeof(sKey));
                        offset = 0;
                        sprintf(sKey, "%d_%d", 1, pNext->m_iOrg);
                        if(m_poOrgExIndex->get(sKey, &offset))
                        {
                            iLevelPre= pOrgEx[offset].m_iLevel;
                        }

                        if(iLevelPre < iLevelCur)
                        {
                            pOrgLongType[i].m_iNextOffset = pCur->m_iNextOffset;
                            pCur->m_iNextOffset = i;
                            bGet = true;
                            break;
                        }
                        else
                        {
                            pCur = pNext;
                            pNext = pOrgLongType + pCur->m_iNextOffset;
                        }
                    }
                    if(bGet)
                        continue;
                    if(pNext == pOrgLongType)
                        pCur->m_iNextOffset = i;

                    continue;
                        
                }
            }
        }
        else
        {
            m_poOrgLongTypeIndex->add(iOrgIDB, i);
        }
    }

    qry.close();
    Log::log(0, "ORGLONGTYPE参数上载正常结束. 记录数: %d", m_poOrgLongTypeData->getCount());
}

void ParamInfoMgr::loadHeadEx()
{
    int iCount = 0;
    HeadEx *pHeadEx = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    sprintf(sSqlCode, " SELECT REGION_ID, "
        " TO_CHAR(NVL(EFF_DATE, TO_DATE('19000101010101', 'yyyymmddhh24miss')), "
                " 'yyyymmddhh24miss') EFF_DATE, "
        " TO_CHAR(NVL(EXP_DATE, TO_DATE('20500101010101', 'yyyymmddhh24miss')), "
                " 'yyyymmddhh24miss') EXP_DATE, "
        " NVL(EMULATORY_PARTNER_ID, -1) EMULATORY_PARTNER_ID, "
        " NVL(PARTY_ID, -1) PARTY_ID, "
        " NVL(TYPE_CODE, '52Z') TYPE_CODE, "
        " HEAD, "
        " ASCII(LOWER(SUBSTR(NVL(TYPE_CODE, '52Z'), -1, 1))) - 97 POSITION, "
        " B.REGION_TYPE_ID REGION_TYPE_ID "
        " FROM (SELECT * FROM B_HEAD_REGION) A, "
        " (SELECT * "
           " FROM (SELECT ORG_ID, "
                        " PARENT_ORG_ID, "
                        " ORG_LEVEL_ID, "
                        " NAME, "
                        " ORG_AREA_CODE, "
                        " ORG_CODE, "
                        " 1 REGION_TYPE_ID "
                   " FROM ORG A "
                 " UNION ALL "
                 " SELECT REGION_ID ORG_ID, "
                        " PARENT_REGION_ID PARENT_ORG_ID, "
                        " REGION_LEVEL ORG_LEVEL_ID, "
                        " NAME, "
                        " ORG_AREA_CODE, "
                        " ORG_CODE, "
                        " 2 REGION_TYPE_ID "
                   " FROM STAT_REGION B "
                 " UNION ALL "
                 " SELECT REGION_ID ORG_ID, "
                        " PARENT_REGION_ID PARENT_ORG_ID, "
                        " REGION_LEVEL ORG_LEVEL_ID, "
                        " NAME, "
                        " REGION_AREA_CODE ORG_CODE, "
                        " REGION_CODE ORG_CODE, "
                        " REGION_TYPE_ID "
                   " FROM B_SELF_DEFINE_REGION)) B "
            " WHERE A.REGION_ID = B.ORG_ID ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_HEADEX_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+219);
    }
    iCount = atoi(countbuf);

    if(!(m_poHeadExData->exist()))
        m_poHeadExData->create(iCount * m_fRatio);
    if(!(m_poHeadExIndex->exist()))
        m_poHeadExIndex->create(iCount * m_fRatio, 32); //索引字符串长度最长为32个字符
    pHeadEx = (HeadEx *)(*m_poHeadExData);
	checkMem(m_poHeadExData->getCount() * sizeof(HeadEx));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlCode);
    qry.open();

    while(qry.next())
    {
        i = m_poHeadExData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+220);

        int iPos = 0;
        int iTypeID=0;
        char sHead[32] = {0};
        char sKey[128]={0};

        pHeadEx[i].m_iOrgID = qry.field("REGION_ID").asInteger();
        strcpy(pHeadEx[i].m_sEffDate, qry.field("EFF_DATE").asString());
        strcpy(pHeadEx[i].m_sExpDate, qry.field("EXP_DATE").asString());
        pHeadEx[i].m_iPartnerID = qry.field("EMULATORY_PARTNER_ID").asInteger();
        pHeadEx[i].m_iPartyID = qry.field("PARTY_ID").asInteger();
        strcpy(pHeadEx[i].m_sTypeCode, qry.field("TYPE_CODE").asString());
        strcpy(pHeadEx[i].m_sHead, qry.field("HEAD").asString());
        pHeadEx[i].m_pNext = 0;
        pHeadEx[i].m_iNextOffset = 0;

        iPos = qry.field("POSITION").asInteger();
        iTypeID = qry.field("REGION_TYPE_ID").asInteger();
        strcpy(sHead, qry.field("HEAD").asString());
        sprintf(sKey, "%d_%d_%s", iPos, iTypeID, sHead);

        unsigned int iOffset = 0;
        if(m_poHeadExIndex->get(sKey, &iOffset))
        {
            pHeadEx[i].m_iNextOffset = iOffset;
            m_poHeadExIndex->add(sKey, i);
        }
        else
        {
            m_poHeadExIndex->add(sKey, i);
        }
    }
    qry.close();
    Log::log(0, "HeadEx参数上载正常结束. 记录数: %d", m_poHeadExData->getCount());
}

void ParamInfoMgr::loadLocalHead()
{
    int iCount = 0;
    LocalHead *pLocalHead = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    sprintf(sSqlCode, " SELECT ORG_ID, AREA_CODE, HEAD, NVL(LEN, 0) LEN, "
                " TO_CHAR(EFF_DATE, 'yyyymmddhh24miss') EFF_DATE, "
                " TO_CHAR(EXP_DATE, 'yyyymmddhh24miss') EXP_DATE "
                " FROM LOCAL_HEAD ORDER BY LEN ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_LOCALHEAD_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+223);
    }
    iCount = atoi(countbuf);

    if(!(m_poLocalHeadData->exist()))
        m_poLocalHeadData->create(iCount * m_fRatio);
    if(!(m_poLocalHeadIndex->exist()))
        m_poLocalHeadIndex->create(iCount * m_fRatio, 42); //索引字符串长度最长为42个字符
    pLocalHead = (LocalHead *)(*m_poLocalHeadData);
	checkMem(m_poLocalHeadData->getCount() * sizeof(LocalHead));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlCode);
    qry.open();

    while(qry.next())
    {
        i = m_poLocalHeadData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+224);

        pLocalHead[i].m_iOrgID = qry.field("ORG_ID").asInteger();
        strcpy(pLocalHead[i].m_sAreaCode, qry.field("AREA_CODE").asString());
        strcpy(pLocalHead[i].m_sHead, qry.field("HEAD").asString());
        pLocalHead[i].m_iLen   = qry.field("LEN").asInteger();
        strcpy(pLocalHead[i].m_sEffDate, qry.field("EFF_DATE").asString());
        strcpy(pLocalHead[i].m_sExpDate, qry.field("EXP_DATE").asString());
        pLocalHead[i].m_pNext = 0;
        pLocalHead[i].m_iNextOffset = 0;
        
        char sKey[42] = {0};
        sprintf(sKey, "%s_%s", pLocalHead[i].m_sAreaCode, pLocalHead[i].m_sHead);
        unsigned int iOffset = 0;
        if(m_poLocalHeadIndex->get(sKey, &iOffset))
        {
            pLocalHead[i].m_iNextOffset = iOffset;
            m_poLocalHeadIndex->add(sKey, i);
        }
        else
        {
            m_poLocalHeadIndex->add(sKey, i);
        }
    }
    qry.close();
    Log::log(0, "LocalHead参数上载正常结束. 记录数: %d", m_poLocalHeadData->getCount());
}

void ParamInfoMgr::loadEventType()
{
    int iCount = 0;
    EventType *pEventType = 0;
    char sSqlcode[512] = {0};
    TOCIQuery qry(Environment::getDBConn());

    sprintf(sSqlcode, "SELECT EVENT_TYPE_ID CHILD_TYPE, NVL(SUM_EVENT_TYPE, -1) "
                " PARENT_TYPE, NVL(EVENT_TABLE_TYPE_ID, -1) TABLE_TYPE "
                " FROM RATABLE_EVENT_TYPE "
                " WHERE STATE = '10A' "
                " ORDER BY EVENT_TYPE_ID ");


    char countbuf[32];
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_EVENTTYPE_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+225);
    }
    iCount = atoi(countbuf);         //RATABLE_EVENT_TYPE的记录条数

    if(!(m_poEventTypeData->exist()))
        m_poEventTypeData->create(iCount*m_fRatio);
    if(!(m_poEventTypeIndex->exist()))
        m_poEventTypeIndex->create(iCount*m_fRatio);
    pEventType = (EventType *)(*m_poEventTypeData);
	checkMem(m_poEventTypeData->getCount() * sizeof(EventType));

    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poEventTypeData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+226);

        pEventType[i].m_iEventType = qry.field(0).asInteger();
        pEventType[i].m_iParentType = qry.field(1).asInteger();
        pEventType[i].m_iTableType = qry.field(2).asInteger();
        pEventType[i].m_poParent = NULL;
        pEventType[i].m_iParentOffset = 0;
        pEventType[i].m_iIsMutex = 1;

        m_poEventTypeIndex->add((long)pEventType[i].m_iEventType, i);
    }

    int iUsedCnt = m_poEventTypeData->getCount();
    for(int j = 1; j <= iUsedCnt; j++)
    {
        if(pEventType[j].m_iParentType != -1)
        {
            unsigned int offset = 0;
            if(m_poEventTypeIndex->get(pEventType[j].m_iParentType, &offset))
            {
                pEventType[j].m_iParentOffset = offset;
            }
            else
                pEventType[j].m_iParentOffset = 0;
        }
    }

    qry.close();
    Log::log(0, "EventType参数上载正常结束. 记录数: %d", m_poEventTypeData->getCount());
}

void ParamInfoMgr::loadEventTypeGroup()
{
    int iCount = 0;
    EventTypeGroup *pEventTypeGroup = 0;
    char sSqlcode[512] = {0};
    TOCIQuery qry(Environment::getDBConn());

    sprintf(sSqlcode, "SELECT EVENT_TYPE_ID, TYPE_GROUP_ID "
                " FROM B_EVENT_TYPE_GROUP_MEMBER "
                " ORDER BY TYPE_GROUP_ID, EVENT_TYPE_ID");

    char countbuf[32];
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_EVENTTYPEGROUP_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+227);
    }
    iCount = atoi(countbuf);    //B_EVENT_TYPE_GROUP_MEMBER的记录条数

    if(!(m_poEventTypeGroupData->exist()))
        m_poEventTypeGroupData->create(iCount*m_fRatio);
    if(!(m_poEventTypeGroupIndex->exist()))
        m_poEventTypeGroupIndex->create(iCount*m_fRatio);
    pEventTypeGroup = (EventTypeGroup *)(*m_poEventTypeGroupData);
	checkMem(m_poEventTypeGroupData->getCount() * sizeof(EventTypeGroup));
    // 数据区填值,并建立索引
    unsigned int i;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poEventTypeGroupData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+228);

        
        pEventTypeGroup[i].m_iEventType = qry.field(0).asInteger();
        pEventTypeGroup[i].m_iGroup = qry.field(1).asInteger();
        pEventTypeGroup[i].m_iNextOffset = 0;

        //如果没有该事件类型 抛出异常.保证在ratable_event_type中存在该事件类型
        unsigned int n = 0;       
        if (!(m_poEventTypeIndex->get(pEventTypeGroup[i].m_iEventType, &n)))
        {
            THROW (MBC_EventTypeMgr+11);
        }

        
        if(i == 0)
        {
            m_poEventTypeGroupIndex->add((long)pEventTypeGroup[i].m_iGroup, i);
        }
        else
        {
            if(pEventTypeGroup[i].m_iGroup == pEventTypeGroup[i-1].m_iGroup)
            {
                pEventTypeGroup[i-1].m_iNextOffset = i;
            }
            else
                m_poEventTypeGroupIndex->add((long)pEventTypeGroup[i].m_iGroup, i);
        }

    }

    qry.close();
    Log::log(0, "EventTypeGroup参数上载正常结束. 记录数: %d", m_poEventTypeGroupData->getCount());
}

void ParamInfoMgr::loadOffer()
{
    int iCount = 0;
    Offer *pOffer = 0;
    char sSqlcode[512] = {0};
    TOCIQuery qry(Environment::getDBConn());

    sprintf(sSqlcode, " SELECT A.OFFER_ID, A.PRICING_PLAN_ID, A.OFFER_PRIORITY, "
                " DECODE(A.OFFER_TYPE, 1, 0, A.OFFER_TYPE), "
                " A.EFF_DATE, A.EXP_DATE "
                " FROM PRODUCT_OFFER A "
                " WHERE STATE = '10A' ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_OFFER_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+103);
    }
    iCount = atoi(countbuf);

    if(!(m_poOfferData->exist()))
        m_poOfferData->create(iCount * m_fRatio);
    if(!(m_poOfferIndex->exist()))
        m_poOfferIndex->create(iCount * m_fRatio);
    pOffer = (Offer *)(*m_poOfferData);
	checkMem(m_poOfferData->getCount() * sizeof(Offer));
    // 数据区填值,并建立索引
    unsigned int i;
    qry.setSQL (sSqlcode);
    qry.open ();

    while(qry.next())
    {
        i = m_poOfferData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+104);

        pOffer[i].m_iOfferID = qry.field(0).asInteger();
        pOffer[i].m_iPricingPlan = qry.field(1).asInteger();
        pOffer[i].m_iPriority = qry.field(2).asInteger();
        pOffer[i].m_iOfferType = (OfferType)qry.field(3).asInteger(); //类型强制转换一下
        strcpy(pOffer[i].m_sEffDate, qry.field(4).asString());
        strcpy(pOffer[i].m_sExpDate, qry.field(5).asString());
        m_poOfferIndex->add((long)pOffer[i].m_iOfferID, i);
    }

    qry.close();
    Log::log(0, "Offer参数上载正常结束. 记录数: %d", m_poOfferData->getCount());
}

void ParamInfoMgr::loadOfferSpecAttr()
{
    int iCount = 0;
    char sSqlcode[512] = {0};
    TOCIQuery qry(Environment::getDBConn());

    sprintf(sSqlcode, " SELECT ATTR_ID, NVL(TO_NUMBER(OFFER_ATTR_VALUE), 0) "
                " FROM PRODUCT_OFFER_ATTR "
                " WHERE ATTR_VALUE_TYPE_ID = 15 ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_OFFER_SPECATTR_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+105);
    }
    iCount = atoi(countbuf);

    if(!(m_poOfferSpecAttrIndex->exist()))
        m_poOfferSpecAttrIndex->create(iCount * m_fRatio);

    qry.setSQL (sSqlcode);
    qry.open ();
    
    while(qry.next())
    {
        m_poOfferSpecAttrIndex->add((long)qry.field(0).asInteger(), qry.field(1).asInteger());
    }

    qry.close();
    Log::log(0, "OfferSpecAttr参数上载正常结束. 记录数: %d", m_poOfferSpecAttrIndex->getCount());
}

void ParamInfoMgr::loadOfferNumAttr()
{
    int iCount = 0;
    char sSqlcode[512] = {0};
    TOCIQuery qry(Environment::getDBConn());

    sprintf(sSqlcode, " SELECT ATTR_ID FROM PRODUCT_OFFER_ATTR "
                " WHERE ATTR_VALUE_TYPE_ID IN (3, 5, 15) ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_OFFER_NUMATTR_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+107);
    }
    iCount = atoi(countbuf);

    if(!(m_poOfferNumAttrIndex->exist()))
        m_poOfferNumAttrIndex->create(iCount * m_fRatio);

    qry.setSQL (sSqlcode);
    qry.open ();
    
    while(qry.next())
    {
        m_poOfferNumAttrIndex->add((long)qry.field(0).asInteger(), 1);
    }

    qry.close();
    Log::log(0, "OfferNumAttr参数上载正常结束. 记录数: %d", m_poOfferNumAttrIndex->getCount());
}

void ParamInfoMgr::loadOfferFree()
{
    int iCount = 0;
    char sSqlcode[512] = {0};
    TOCIQuery qry(Environment::getDBConn());

    sprintf(sSqlcode, " SELECT OFFER_ID FROM B_OFFER_ITEM_GROUP_RELATION "
                " WHERE FREE_TYPE = 1 ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_OFFER_FREE_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+109);
    }
    iCount = atoi(countbuf);

    if(!(m_poOfferFreeIndex->exist()))
        m_poOfferFreeIndex->create(iCount * m_fRatio);

    qry.setSQL (sSqlcode);
    qry.open ();
    
    while(qry.next())
    {
        m_poOfferFreeIndex->add((long)qry.field(0).asInteger(), 1);
    }

    qry.close();
    Log::log(0, "OfferFree参数上载正常结束. 记录数: %d", m_poOfferFreeIndex->getCount());
}

void ParamInfoMgr::loadNbrMap()
{
    int iCount= 0;
    NbrMapData *pNbrMapData = 0;
    char sSqlcode[512] = {0};
    TOCIQuery qry(Environment::getDBConn());

    sprintf(sSqlcode, " SELECT ACC_NBR, MAP_TYPE_ID, NEED_SAME_AREA, REF_VALUE, "
                " OUT_AREA_CODE, OUT_ACC_NBR, "
                " TO_CHAR(EFF_DATE, 'yyyymmddhh24miss') EFF, "
                " TO_CHAR(EXP_DATE, 'yyyymmddhh24miss') EXP "
                " FROM B_NBR_MAP ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_NBR_MAP_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+201);
    }
    iCount = atoi(countbuf);

    if(!(m_poNbrMapData->exist()))
        m_poNbrMapData->create(iCount * m_fRatio);
    if(!(m_poNbrMapType1Index->exist()))
        m_poNbrMapType1Index->create(iCount * m_fRatio, 65);
    if(!(m_poNbrMapType2Index->exist()))
        m_poNbrMapType2Index->create(iCount * m_fRatio, 65);
    if(!(m_poNbrMapType4Index->exist()))
        m_poNbrMapType4Index->create(iCount * m_fRatio, 65);
    if(!(m_poNbrMapType8Index->exist()))
        m_poNbrMapType8Index->create(iCount * m_fRatio, 65);
    pNbrMapData = (NbrMapData *)(*m_poNbrMapData);
	checkMem(m_poNbrMapData->getCount() * sizeof(NbrMapData));
    // 数据区填值,并建立索引
    unsigned int i;
    qry.setSQL (sSqlcode);
    qry.open ();

    while(qry.next())
    {
        i = m_poNbrMapData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+202);

        strncpy(pNbrMapData[i].m_sAccNbr, qry.field(0).asString(), 32);
        pNbrMapData[i].m_iMapType = qry.field(1).asInteger();
        strncpy(pNbrMapData[i].m_sNeedSameArea, qry.field(2).asString(), 32);
        strncpy(pNbrMapData[i].m_sRefValue, qry.field(3).asString(), 32);
        strncpy(pNbrMapData[i].m_sOutAreaCode, qry.field(4).asString(), 16);
        strncpy(pNbrMapData[i].m_sOutAccNbr, qry.field(5).asString(), 32);
        strncpy(pNbrMapData[i].m_sEffDate, qry.field(6).asString(), 16);
        strncpy(pNbrMapData[i].m_sExpDate, qry.field(7).asString(), 16);
        pNbrMapData[i].m_iNextOffset = 0;

        char sKey[66] = {0};
        int  iType = 0;
        sprintf(sKey, "%s_%s", pNbrMapData[i].m_sAccNbr, pNbrMapData[i].m_sRefValue);
        iType = pNbrMapData[i].m_iMapType/2;
        if(iType >= 4)
            iType = 4;
        switch(iType)
        {
            case 1:
                {
                    m_poNbrMapType1Index->get(sKey, &pNbrMapData[i].m_iNextOffset);
                    m_poNbrMapType1Index->add(sKey, i);
                    break;
                }
            case 2:
                {
                    m_poNbrMapType2Index->get(sKey, &pNbrMapData[i].m_iNextOffset);
                    m_poNbrMapType2Index->add(sKey, i);
                    break;
                }
            case 3:
                {
                    m_poNbrMapType4Index->get(sKey, &pNbrMapData[i].m_iNextOffset);
                    m_poNbrMapType4Index->add(sKey, i);
                    break;
                }
            case 4:
                
            default:
                {
                    m_poNbrMapType8Index->get(sKey, &pNbrMapData[i].m_iNextOffset);
                    m_poNbrMapType8Index->add(sKey, i);
                    break;
                }
            
        }
    }

    qry.close();
    Log::log(0, "NbrMapData参数上载正常结束. 记录数: %d", m_poNbrMapData->getCount());
}

void ParamInfoMgr::loadAcctItem()
{
    AcctItemLoad *pAcctLoad = new AcctItemLoad();  //加载到私有内存
    if(!pAcctLoad)
        THROW(MBC_ParamInfoMgr+143);
    
    int iCount;                 //AcctItem总条数
    AcctItem *pAcctItem = 0;

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ACCTITEM_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+115);
    }
    iCount = atoi(countbuf);    //AcctItem总条数

    if(!(m_poAcctItemData->exist()))
        m_poAcctItemData->create(iCount * m_fRatio);
    if(!(m_poAcctItemIndex->exist()))
        m_poAcctItemIndex->create(iCount * m_fRatio);
	if(!(m_poAcctItemStrIndex->exist()))
        m_poAcctItemStrIndex->create(iCount * m_fRatio,26);
    pAcctItem = (AcctItem *)(*m_poAcctItemData);
	checkMem(m_poAcctItemData->getCount() * sizeof(AcctItem));
    int iAcctCnt = pAcctLoad->m_iCount;  //私有内存中AcctItem的总条数
    int iIdx;
    for(int i=0; i < iAcctCnt; i++)      //拷贝到共享内存
    {
        iIdx = m_poAcctItemData->malloc();
        if(!iIdx)
            THROW(MBC_ParamInfoMgr+116);

        pAcctItem[iIdx].m_iAcctItemID = pAcctLoad->m_poAcctItemList[i].m_iAcctItemID;
        pAcctItem[iIdx].m_iParentID = pAcctLoad->m_poAcctItemList[i].m_iParentID;
        pAcctItem[iIdx].m_iOffset = iIdx;
        strcpy(pAcctItem[iIdx].m_sAcctItemCode, pAcctLoad->m_poAcctItemList[i].m_sAcctItemCode);
        pAcctItem[iIdx].m_iPartyRoleID = pAcctLoad->m_poAcctItemList[i].m_iPartyRoleID;
        pAcctItem[iIdx].m_iParentOffset = 0;
        pAcctItem[iIdx].m_iOtherOffset = 0;

        m_poAcctItemIndex->add((long)pAcctItem[iIdx].m_iAcctItemID, iIdx);
    }
    
    for(int i=0; i < iAcctCnt; i++)
    {
        unsigned int j = 0;
        unsigned int k = 0;
        unsigned int l = 0;
        if(m_poAcctItemIndex->get(pAcctLoad->m_poAcctItemList[i].m_iAcctItemID, &j))
        {
            if((pAcctLoad->m_poAcctItemList[i].m_iParentID != -1) && 
                m_poAcctItemIndex->get(pAcctLoad->m_poAcctItemList[i].m_iParentID, &k))
            {
                pAcctItem[j].m_iParentOffset = k;
            }

            if(((pAcctLoad->m_poAcctItemList[i].m_sAcctItemCode[0] == 'A') 
               || (pAcctLoad->m_poAcctItemList[i].m_sAcctItemCode[0] == 'B'))
               && pAcctLoad->m_poAcctItemList[i].m_poOther != 0
               && m_poAcctItemIndex->get(pAcctLoad->m_poAcctItemList[i].m_poOther->m_iAcctItemID, &l))
            {
                pAcctItem[j].m_iOtherOffset = l;
            }
        }
    }
	//str索引
	unsigned int iOff = 0;
	for(unsigned int i=1; i <= m_poAcctItemData->getCount(); i++)
    {
		char sKey[32] = {0};
    	sprintf(sKey,"%d_%d",pAcctItem[i].m_iAcctItemID,pAcctItem[i].m_iParentID);
		if(!m_poAcctItemStrIndex->get(sKey,&iOff))
		{
			m_poAcctItemStrIndex->add(sKey,i);
		}
    }
    Log::log(0, "ACCT_ITEM_TYPE参数上载正常结束. 数据区记录数/总数: %d/%d; 索引区记录数/总数: %d/%d ; STR索引区记录数/总数: %d/%d", 
        m_poAcctItemData->getCount(), m_poAcctItemData->getTotal(),
        m_poAcctItemIndex->getCount(), m_poAcctItemIndex->getTotal(),
		m_poAcctItemStrIndex->getCount(), m_poAcctItemStrIndex->getTotal());
    
    //
    int iItemCnt, iGroupCnt;
    char countbuf2[32], countbuf3[32];
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ACCTITEMMEMBERS_NUM, countbuf2))
    {
        THROW(MBC_ParamInfoMgr+117);
    }
    iItemCnt = atoi(countbuf2);      //总条数

    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ACCTITEMGROUPS_NUM, countbuf3))
    {
        THROW(MBC_ParamInfoMgr+118);
    }
    iGroupCnt = atoi(countbuf3);    //总组数

    if(!(m_poAcctItemGroupData->exist()))
        m_poAcctItemGroupData->create(iItemCnt * m_fRatio);
    if(!(m_poAcctItemGroupIndex->exist()))
        m_poAcctItemGroupIndex->create(iGroupCnt * m_fRatio);
	if(!(m_poAcctItemGroupStrIndex->exist()))
        m_poAcctItemGroupStrIndex->create(iItemCnt * m_fRatio,26);
	checkMem(m_poAcctItemGroupData->getCount() * sizeof(ItemGroupMember));
    ItemGroupMember *pItemGroupMember = 0;
    pItemGroupMember = (ItemGroupMember *)(*m_poAcctItemGroupData);

    int iOffset= 0;
    unsigned int iItemIdx = 0;
    unsigned int iGroupIdx = 0;
    ItemGroupMember *pItem = 0;
    HashList<int>::Iteration iter = pAcctLoad->m_poGroupSerial->getIteration();
    while(iter.next(iOffset))
    {
        pItem = &(pAcctLoad->m_poGroupList[iOffset]);
        while(pItem)
        {
            iItemIdx = m_poAcctItemGroupData->malloc();
            if(!iItemIdx)
                THROW(MBC_ParamInfoMgr+119);

            pItemGroupMember[iItemIdx].m_iItemTypeID = pItem->m_iItemTypeID;
            pItemGroupMember[iItemIdx].m_iItemGroupID = pItem->m_iItemGroupID;
            pItemGroupMember[iItemIdx].m_poNext = 0;
            pItemGroupMember[iItemIdx].m_iNextOffset = 0;

            if(!m_poAcctItemGroupIndex->get(pItem->m_iItemGroupID, &iGroupIdx))
            {
                m_poAcctItemGroupIndex->add((long)pItemGroupMember[iItemIdx].m_iItemGroupID, iItemIdx);
            }
            else
            {
                pItemGroupMember[iItemIdx].m_iNextOffset = pItemGroupMember[iGroupIdx].m_iNextOffset;
                pItemGroupMember[iGroupIdx].m_iNextOffset = iItemIdx;
            }
			//扩展的索引
			char sKey[32] = {0};
			sprintf(sKey,"%d_%d",pItem->m_iItemGroupID,pItem->m_iItemTypeID);
			if(!m_poAcctItemGroupStrIndex->get(sKey, &iGroupIdx))
            {
                m_poAcctItemGroupStrIndex->add(sKey, iItemIdx);
            }
			
            pItem = pItem->m_poNext;

        }

    }
    Log::log(0, "ACCT_ITEM_GROUP_MEMBER参数上载正常结束. 数据区记录数/总数: %d/%d; 索引区记录数/总数: %d/%d,; STR索引区记录数/总数: %d/%d", 
        m_poAcctItemGroupData->getCount(), m_poAcctItemGroupData->getTotal(),
        m_poAcctItemGroupIndex->getCount(), m_poAcctItemGroupIndex->getTotal(),
		m_poAcctItemGroupStrIndex->getCount(), m_poAcctItemGroupStrIndex->getTotal());

    //    
    //
    int iItemCntB, iGroupCntB;
    char countbuf4[32], countbuf5[32];
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ACCTITEMMEMBERSB_NUM, countbuf4))
    {
        THROW(MBC_ParamInfoMgr+120);
    }
    iItemCntB = atoi(countbuf4);      //总条数

    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ACCTITEMGROUPSB_NUM, countbuf5))
    {
        THROW(MBC_ParamInfoMgr+121);
    }
    iGroupCntB = atoi(countbuf5);     //总组数

    if(!(m_poAcctItemGroupBData->exist()))
        m_poAcctItemGroupBData->create(iItemCntB * m_fRatio);
    if(!(m_poAcctItemGroupBIndex->exist()))
        m_poAcctItemGroupBIndex->create(iGroupCntB * m_fRatio);
    if(!(m_poAcctItemBasicGroupBData->exist()))
        m_poAcctItemBasicGroupBData->create(iItemCntB * m_fRatio);
    if(!(m_poAcctItemBasicGroupBIndex->exist()))
        m_poAcctItemBasicGroupBIndex->create(iGroupCntB * m_fRatio);
    if(!(m_poAcctItemDisctGroupBData->exist()))
        m_poAcctItemDisctGroupBData->create(iItemCntB * m_fRatio);
    if(!(m_poAcctItemDisctGroupBIndex->exist()))
        m_poAcctItemDisctGroupBIndex->create(iGroupCntB * m_fRatio);
    if(!(m_poAcctItemIncludeDisctIndex->exist()))
        m_poAcctItemIncludeDisctIndex->create(iGroupCntB * m_fRatio);
	//
	if(!(m_poAcctItemGroupBStrIndex->exist()))
        m_poAcctItemGroupBStrIndex->create(iItemCntB * m_fRatio,26);
	//

    ItemGroupMember *pItemGroupMemberB = 0;
    pItemGroupMemberB = (ItemGroupMember *)(*m_poAcctItemGroupBData);

    ItemGroupMember *pItemGroupBasicMemberB = 0;
    pItemGroupBasicMemberB = (ItemGroupMember *)(*m_poAcctItemBasicGroupBData);

    ItemGroupMember *pItemGroupDisctMemberB = 0;
    pItemGroupDisctMemberB = (ItemGroupMember *)(*m_poAcctItemDisctGroupBData);
	checkMem(m_poAcctItemGroupBData->getCount() * sizeof(ItemGroupMember));
	checkMem(m_poAcctItemBasicGroupBData->getCount() * sizeof(ItemGroupMember));
	checkMem(m_poAcctItemDisctGroupBData->getCount() * sizeof(ItemGroupMember));
    char sSqlcode[512];
    int iGroupID;
    unsigned int o;
    int iOffsetB= 0;
    int iItemIdxB = 0;
    unsigned int iGroupIdxB = 0;
    ItemGroupMember *pItemB = 0;
    int iOffsetBBasic= 0;
    int iItemIdxBBasic = 0;
    unsigned int iGroupIdxBBasic = 0;
    ItemGroupMember *pItemBBasic = 0;
    int iOffsetBDisct= 0;
    int iItemIdxBDisct = 0;
    unsigned int iGroupIdxBDisct = 0;
    ItemGroupMember *pItemBDisct = 0;
    TOCIQuery qry(Environment::getDBConn());

    strcpy(sSqlcode, "SELECT DISTINCT S.ITEM_GROUP_ID, NVL(S.INCLUDE_DISCOUNT_ITEM, 1) "
                " FROM B_ACCT_ITEM_GROUP S "
                " WHERE EXISTS "
                " (SELECT 1 FROM B_ACCT_ITEM_GROUP_MEMBER T "
                " WHERE T.ITEM_GROUP_ID = S.ITEM_GROUP_ID) ");

    qry.setSQL (sSqlcode);
    qry.open ();
    while(qry.next())
    {
        iGroupID = qry.field(0).asInteger();

        if(!m_poAcctItemIncludeDisctIndex->get(iGroupID,&o))
        {
            m_poAcctItemIncludeDisctIndex->add((long)iGroupID, qry.field(1).asInteger());
        }

        if(pAcctLoad->m_poGroupSerialB->get(iGroupID, &iOffsetB))
        {
            pItemB= &(pAcctLoad->m_poGroupListB[iOffsetB]);
            while(pItemB)
            {
                iItemIdxB= m_poAcctItemGroupBData->malloc();
                if(!iItemIdxB)
                    THROW(MBC_ParamInfoMgr+122);

                pItemGroupMemberB[iItemIdxB].m_iItemTypeID = pItemB->m_iItemTypeID;
                pItemGroupMemberB[iItemIdxB].m_iItemGroupID = pItemB->m_iItemGroupID;
                pItemGroupMemberB[iItemIdxB].m_poNext = 0;
                pItemGroupMemberB[iItemIdxB].m_iNextOffset = 0;

                if(!m_poAcctItemGroupBIndex->get(pItemB->m_iItemGroupID, &iGroupIdxB))
                {
                    m_poAcctItemGroupBIndex->add((long)pItemGroupMemberB[iItemIdxB].m_iItemGroupID, iItemIdxB);
                }
                else
                {
                    pItemGroupMemberB[iItemIdxB].m_iNextOffset = pItemGroupMemberB[iGroupIdxB].m_iNextOffset;
                    pItemGroupMemberB[iGroupIdxB].m_iNextOffset = iItemIdxB;
                }
				//扩展的索引
				char sKey[32] = {0};
				sprintf(sKey,"%d_%d",pItemB->m_iItemGroupID,pItemB->m_iItemTypeID);
				if(!m_poAcctItemGroupBStrIndex->get(sKey, &iGroupIdxB))
                {
                    m_poAcctItemGroupBStrIndex->add(sKey, iItemIdxB);
                }

                pItemB= pItemB->m_poNext;
            }
        }
        if(pAcctLoad->m_poGroupBasicSerialB->get(iGroupID, &iOffsetBBasic))
        {
            pItemBBasic = &(pAcctLoad->m_poGroupBasicMemberListB[iOffsetBBasic]);
            while(pItemBBasic)
            {
                iItemIdxBBasic = m_poAcctItemBasicGroupBData->malloc();
                if(!iItemIdxBBasic)
                    THROW(MBC_ParamInfoMgr+122);

                pItemGroupBasicMemberB[iItemIdxBBasic].m_iItemTypeID = pItemBBasic->m_iItemTypeID;
                pItemGroupBasicMemberB[iItemIdxBBasic].m_iItemGroupID = pItemBBasic->m_iItemGroupID;
                pItemGroupBasicMemberB[iItemIdxBBasic].m_poNext = 0;
                pItemGroupBasicMemberB[iItemIdxBBasic].m_iNextOffset = 0;

                if(!m_poAcctItemBasicGroupBIndex->get(pItemBBasic->m_iItemGroupID, &iGroupIdxBBasic))
                {
                    m_poAcctItemBasicGroupBIndex->add((long)pItemGroupBasicMemberB[iItemIdxBBasic].m_iItemGroupID, iItemIdxBBasic);
                }
                else
                {
                    pItemGroupBasicMemberB[iItemIdxBBasic].m_iNextOffset =
                        pItemGroupBasicMemberB[iGroupIdxBBasic].m_iNextOffset;
                    pItemGroupBasicMemberB[iGroupIdxBBasic].m_iNextOffset = iItemIdxBBasic;
                }
                pItemBBasic = pItemBBasic->m_poNext;
            }
        }
        if(pAcctLoad->m_poGroupDisctSerialB->get(iGroupID, &iOffsetBDisct))
        {
            pItemBDisct = &(pAcctLoad->m_poGroupDisctMemberListB[iOffsetBDisct]);
            while(pItemBDisct)
            {
                iItemIdxBDisct = m_poAcctItemDisctGroupBData->malloc();
                if(!iItemIdxBDisct)
                    THROW(MBC_ParamInfoMgr+122);

                pItemGroupDisctMemberB[iItemIdxBDisct].m_iItemTypeID = pItemBDisct->m_iItemTypeID;
                pItemGroupDisctMemberB[iItemIdxBDisct].m_iItemGroupID = pItemBDisct->m_iItemGroupID;
                pItemGroupDisctMemberB[iItemIdxBDisct].m_poNext = 0;
                pItemGroupDisctMemberB[iItemIdxBDisct].m_iNextOffset = 0;

                if(!m_poAcctItemDisctGroupBIndex->get(pItemBDisct->m_iItemGroupID, &iGroupIdxBDisct))
                {
                    m_poAcctItemDisctGroupBIndex->add((long)pItemGroupDisctMemberB[iItemIdxBDisct].m_iItemGroupID, iItemIdxBDisct);
                }
                else
                {
                    pItemGroupDisctMemberB[iItemIdxBDisct].m_iNextOffset =
                        pItemGroupDisctMemberB[iGroupIdxBDisct].m_iNextOffset;
                    pItemGroupDisctMemberB[iGroupIdxBDisct].m_iNextOffset = iItemIdxBDisct;
                }
                pItemBDisct = pItemBDisct->m_poNext;
            }
        }

    }

    qry.close();
    
    if(pAcctLoad)
    {
        delete pAcctLoad;
        pAcctLoad = 0;
    }

    Log::log(0, "B_ACCT_ITEM_GROUP参数上载正常结束. 数据区记录数/总数: %d/%d; 索引区记录数/总数: %d/%d,; STR索引区记录数/总数: %d/%d", 
        m_poAcctItemGroupBData->getCount(), m_poAcctItemGroupBData->getTotal(),
        m_poAcctItemGroupBIndex->getCount(), m_poAcctItemGroupBIndex->getTotal(),
		m_poAcctItemGroupBStrIndex->getCount(), m_poAcctItemGroupBStrIndex->getTotal());
    Log::log(0, "B_ACCT_ITEM_GROUP_BASIC参数上载正常结束. 数据区记录数/总数: %d/%d; 索引区记录数/总数: %d/%d", 
        m_poAcctItemBasicGroupBData->getCount(), m_poAcctItemBasicGroupBData->getTotal(),
        m_poAcctItemBasicGroupBIndex->getCount(), m_poAcctItemBasicGroupBIndex->getTotal());
    Log::log(0, "B_ACCT_ITEM_GROUP_DISCT参数上载正常结束. 数据区记录数/总数: %d/%d; 索引区记录数/总数: %d/%d", 
        m_poAcctItemDisctGroupBData->getCount(), m_poAcctItemDisctGroupBData->getTotal(),
        m_poAcctItemDisctGroupBIndex->getCount(), m_poAcctItemDisctGroupBIndex->getTotal());
    Log::log(0, "B_ACCT_ITEM_GROUP_INCLUDE参数上载正常结束. 索引区记录数/总数: %d/%d", 
        m_poAcctItemIncludeDisctIndex->getCount(), m_poAcctItemIncludeDisctIndex->getTotal());
    
}

void ParamInfoMgr::loadLongEventTypeRule()
{
    int iCount = 0;
    LongEventTypeRule *pRule = 0;
    char sSqlcode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
        
    strcpy(sSqlcode, " SELECT DECODE(IN_EVENT_TYPE_ID,NULL,1,0) S0, "
    " DECODE(LONG_TYPE_ID,NULL,1,0) S1, DECODE(ROAM_TYPE_ID,NULL,1,0) S2, "
    " DECODE(CALLING_SP_TYPE_ID,NULL,1,0) S3, DECODE(CALLED_SP_TYPE_ID,NULL,1,0) S4, "
    " DECODE(CALLING_SERVICE_TYPE_ID,NULL,1,0) S5, DECODE(CALLED_SERVICE_TYPE_ID,NULL,1,0) S6, "
    " DECODE(CARRIER_TYPE_ID,NULL,1,0) S7,DECODE(BUSINESS_KEY,NULL,1,0) S8, "
    " RULE_ID, SOURCE_EVENT_TYPE, "
    " NVL(IN_EVENT_TYPE_ID,-1) IN_EVENT_TYPE_ID , "
    " NVL(LONG_TYPE_ID,-1) LONG_TYPE_ID, "
    " NVL(ROAM_TYPE_ID,-1) ROAM_TYPE_ID, "
    " NVL(CALLING_SP_TYPE_ID,-1) CALLING_SP_TYPE_ID, "
    " NVL(CALLED_SP_TYPE_ID,-1) CALLED_SP_TYPE_ID, "
    " NVL(CALLING_SERVICE_TYPE_ID,-1) CALLING_SERVICE_TYPE_ID, "
    " NVL(CALLED_SERVICE_TYPE_ID,-1) CALLED_SERVICE_TYPE_ID, "
    " NVL(CARRIER_TYPE_ID,-1) CARRIER_TYPE_ID, "
    " NVL(BUSINESS_KEY,-1) BUSINESS_KEY, OUT_EVENT_TYPE_ID "
    " FROM B_LONG_EVENT_TYPE_RULE "
    " ORDER BY SOURCE_EVENT_TYPE, S0,S1,S2,S3,S4,S5,S6,S7,S8 ");

    
    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_LONGEVENTTYPERULE_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+203);
    }
    iCount = atoi(countbuf);

    if(!(m_poLongEventTypeRuleData->exist()))
        m_poLongEventTypeRuleData->create(iCount * m_fRatio);

    pRule = (LongEventTypeRule *)(*m_poLongEventTypeRuleData);

    // 数据区填值,并建立索引
    unsigned int i = 0;
    unsigned int iFirstEventOffset = 0;
    unsigned int iPreEventOffset = 0;
    int iLastSourceEventType = -1;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poLongEventTypeRuleData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+204);

        pRule[i].m_iRuleID = qry.field("RULE_ID").asInteger();
        pRule[i].m_iSourceEventType = qry.field("SOURCE_EVENT_TYPE").asInteger();
        pRule[i].m_iInEventTypeID = qry.field("IN_EVENT_TYPE_ID").asInteger();
        pRule[i].m_iLongTypeID = qry.field("LONG_TYPE_ID").asInteger();
        pRule[i].m_iRoamTypeID = qry.field("ROAM_TYPE_ID").asInteger();
        pRule[i].m_iCallingSPTypeID = qry.field("CALLING_SP_TYPE_ID").asInteger();
        pRule[i].m_iCalledSPTypeID = qry.field("CALLED_SP_TYPE_ID").asInteger();
        pRule[i].m_iCallingServiceTypeID = qry.field("CALLING_SERVICE_TYPE_ID").asInteger();
        pRule[i].m_iCalledServiceTypeID = qry.field("CALLED_SERVICE_TYPE_ID").asInteger();
        pRule[i].m_iCarrierTypeID = qry.field("CARRIER_TYPE_ID").asInteger();
        pRule[i].m_iBusinessKey   = qry.field("BUSINESS_KEY").asInteger();
        pRule[i].m_iOutEventTypeID = qry.field("OUT_EVENT_TYPE_ID").asInteger();
        pRule[i].m_poNextEvent = NULL;
        pRule[i].m_poNextRule= NULL;
        pRule[i].m_iNextEventOffset = 0;
        pRule[i].m_iNextRuleOffset = 0;

        if(i == 1)
        {
            iLastSourceEventType = pRule[1].m_iSourceEventType;
            iFirstEventOffset    = 1;
            iPreEventOffset      = 1;
        }
        else
        {
            if(pRule[i].m_iSourceEventType != iLastSourceEventType)
            {
                pRule[iFirstEventOffset].m_iNextEventOffset = i;
                iLastSourceEventType = pRule[i].m_iSourceEventType;
                iFirstEventOffset = i;
                iPreEventOffset = i;
            }
            else
            {
                pRule[iPreEventOffset].m_iNextRuleOffset = i;
                iPreEventOffset = i;
            }
        }
        
    }
    
    qry.close();
    Log::log(0, "LongEventTypeRule上载正常结束. 记录数: %d", m_poLongEventTypeRuleData->getCount());
}

void ParamInfoMgr::loadLongEventTypeRuleEx()
{
    int iCount = 0;
    LongEventTypeRuleEx *pRule = 0;
    char sSqlcode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
        
    strcpy(sSqlcode, " SELECT DECODE(IN_EVENT_TYPE_ID,NULL,1,0) S0, "
    "DECODE(LONG_TYPE_ID,NULL,1,0) S1, DECODE(ROAM_TYPE_ID,NULL,1,0) S2, "
    "DECODE(PARTY_ROLE_ID,NULL,1,0) S3, DECODE(PARTY_ID,NULL,1,0) S4, "
    "DECODE(CARRIER_TYPE_ID,NULL,1,0) S7,DECODE(BUSINESS_KEY,NULL,1,0) S8, " 
    "RULE_ID, SOURCE_EVENT_TYPE,  "
    "NVL(IN_EVENT_TYPE_ID,-1) IN_EVENT_TYPE_ID , " 
    "NVL(LONG_TYPE_ID,-1) LONG_TYPE_ID, "
    "NVL(ROAM_TYPE_ID,-1) ROAM_TYPE_ID,  "
    "NVL(   PARTY_ROLE_ID   ,-1) PARTY_ROLE_ID, "
    "NVL(   PARTY_ID   ,-1) PARTY_ID, "
    "NVL(CARRIER_TYPE_ID,-1) CARRIER_TYPE_ID, "
    "NVL(BUSINESS_KEY,-1) BUSINESS_KEY, OUT_EVENT_TYPE_ID, "
    "NVL(CALL_TYPE_ID,-1) CALLTYPEID "
    "FROM B_TONE_EVENT_TYPE_RULE "
    "ORDER BY SOURCE_EVENT_TYPE, S0,S1,S2,S4,S3,S7,S8  ");

    
    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_LONGEVENTTYPERULEEX_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+205);
    }
    iCount = atoi(countbuf);

    if(!(m_poLongEventTypeRuleExData->exist()))
        m_poLongEventTypeRuleExData->create(iCount * m_fRatio);

    pRule = (LongEventTypeRuleEx *)(*m_poLongEventTypeRuleExData);
	checkMem(m_poLongEventTypeRuleExData->getCount() * sizeof(LongEventTypeRuleEx));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    unsigned int iFirstEventOffset = 0;
    unsigned int iPreEventOffset = 0;
    int iLastSourceEventType = -1;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poLongEventTypeRuleExData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+206);

        pRule[i].m_iRuleID = qry.field("RULE_ID").asInteger();
        pRule[i].m_iSourceEventType = qry.field("SOURCE_EVENT_TYPE").asInteger();
        pRule[i].m_iInEventTypeID = qry.field("IN_EVENT_TYPE_ID").asInteger();
        pRule[i].m_iLongTypeID = qry.field("LONG_TYPE_ID").asInteger();
        pRule[i].m_iRoamTypeID = qry.field("ROAM_TYPE_ID").asInteger();
        pRule[i].m_iPartyRuleID = qry.field("PARTY_ROLE_ID").asInteger();
        pRule[i].m_iPartyID = qry.field("PARTY_ID").asInteger();
        pRule[i].m_iCarrierTypeID = qry.field("CARRIER_TYPE_ID").asInteger();
        pRule[i].m_iBusinessKey   = qry.field("BUSINESS_KEY").asInteger();
        pRule[i].m_iOutEventTypeID = qry.field("OUT_EVENT_TYPE_ID").asInteger();
        pRule[i].m_iCallTypeID = qry.field("CALLTYPEID").asInteger();
        pRule[i].m_poNextEvent = NULL;
        pRule[i].m_poNextRule= NULL;
        pRule[i].m_iNextEventOffset = 0;
        pRule[i].m_iNextRuleOffset = 0;

        if(i == 1)
        {
            iLastSourceEventType = pRule[1].m_iSourceEventType;
            iFirstEventOffset    = 1;
            iPreEventOffset      = 1;
        }
        else
        {
            if(pRule[i].m_iSourceEventType != iLastSourceEventType)
            {
                pRule[iFirstEventOffset].m_iNextEventOffset = i;
                iLastSourceEventType = pRule[i].m_iSourceEventType;
                iFirstEventOffset = i;
                iPreEventOffset = i;
            }
            else
            {
                pRule[iPreEventOffset].m_iNextRuleOffset = i;
                iPreEventOffset = i;
            }
        }
        
    }
    
    qry.close();
    Log::log(0, "LongEventTypeRuleEx上载正常结束. 记录数: %d", m_poLongEventTypeRuleExData->getCount());
}

void ParamInfoMgr::loadPricingPlanType()
{
    int iCount = 0;
    char sSqlCode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
    sprintf(sSqlCode, " SELECT PRICING_PLAN_ID, PRICING_TYPE FROM PRICING_PLAN ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_PRICINGPLANTYPE_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+207);
    }
    iCount = atoi(countbuf);

    if(!(m_poPricingPlanTypeIndex->exist()))
        m_poPricingPlanTypeIndex->create(iCount * m_fRatio);

    //数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlCode);
    qry.open();
    
    while(qry.next())
    {
        m_poPricingPlanTypeIndex->add(qry.field(0).asInteger(), qry.field(1).asInteger());
    }
    qry.close();
    Log::log(0, "PRICING_PLAN_TYPE参数上载正常结束. 记录数: %d", m_poPricingPlanTypeIndex->getCount());
}

void ParamInfoMgr::loadStateConvert()
{
    int iCount = 0;
    StateConvert *pStateConvert = 0;
    char sSqlcode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
        
    strcpy(sSqlcode, " SELECT NVL(PRODUCT_ID, 0), NVL(PRODUCT_OFFER_ID, 0), "
       " SERV_STATE, BILLING_STATE FROM B_SERV_STATE_CONVERT "
       " ORDER BY SERV_STATE, PRODUCT_ID, PRODUCT_OFFER_ID ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_STATECONVERT_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+209);
    }
    iCount = atoi(countbuf);

    if(!(m_poStateConvertData->exist()))
        m_poStateConvertData->create(iCount * m_fRatio);
    if(!(m_poStateConvertIndex->exist()))
        m_poStateConvertIndex->create(iCount * m_fRatio);

    pStateConvert = (StateConvert *)(*m_poStateConvertData);
	checkMem(m_poStateConvertData->getCount() * sizeof(StateConvert));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    unsigned int iIdx = 0;
    unsigned int iFirstEventOffset = 0;
    unsigned int iPreEventOffset = 0;
    int iLastServState = -1;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poStateConvertData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+210);

        pStateConvert[i].m_iProductID = qry.field(0).asInteger();
        pStateConvert[i].m_iOfferID   = qry.field(1).asInteger();
        strcpy(pStateConvert[i].m_sServState, qry.field(2).asString());
        strcpy(pStateConvert[i].m_sBillingState, qry.field(3).asString());
        pStateConvert[i].m_iNextProdOffset  = 0;
        pStateConvert[i].m_iNextOfferOffset = 0;

        if(m_poStateConvertIndex->get((int)pStateConvert[i].m_sServState[2], &iIdx))
        {
            if(pStateConvert[i].m_iProductID == pStateConvert[iIdx].m_iProductID)
            {
                pStateConvert[i].m_iNextOfferOffset = iIdx;
                pStateConvert[i].m_iNextProdOffset = pStateConvert[iIdx].m_iNextProdOffset;
            }
            else
            {
                pStateConvert[i].m_iNextOfferOffset = 0;
                pStateConvert[i].m_iNextProdOffset = iIdx;
            }
            m_poStateConvertIndex->add((int)pStateConvert[i].m_sServState[2], i);
        }
        else
        {
            m_poStateConvertIndex->add((int)pStateConvert[i].m_sServState[2], i);
        }
        
    }
    
    qry.close();
    Log::log(0, "B_SERV_STATE_CONVERT上载正常结束. 记录数: %d", m_poStateConvertData->getCount());
}

void ParamInfoMgr::loadValueMap()
{
    int iCount = 0;
    ValueMapData *pValueMapData = 0;
    char sSqlcode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
        
    strcpy(sSqlcode, " SELECT NVL(APP_ID, -1), NVL(BILLFLOW_ID, -1), NVL(PROCESS_ID, -1), "
        " NVL(VALUE_MAP_TYPE, -1), "
        " NVL(ORG_VALUE, -1), "
        " NVL(DEST_VALUE, -1), "
        " NVL(TO_CHAR(EFF_DATE, 'yyyymmddhh24miss'), '19000101000000') EFF, "
        " NVL(TO_CHAR(EXP_DATE, 'yyyymmddhh24miss'), '30000101000000') EXP "
        " FROM B_VALUE_MAP "
        " ORDER BY EFF_DATE ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_VALUEMAP_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+229);
    }
    iCount = atoi(countbuf);

    if(!(m_poValueMapData->exist()))
        m_poValueMapData->create(iCount * m_fRatio);
    if(!(m_poValueMapIndex->exist()))
        m_poValueMapIndex->create(iCount * m_fRatio, 40);

    pValueMapData = (ValueMapData *)(*m_poValueMapData);
	checkMem(m_poValueMapData->getCount() * sizeof(ValueMapData));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poValueMapData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+230);
        char sKey[40];
        memset(sKey, '\0', 40);
        pValueMapData[i].m_iAppID  = qry.field(0).asInteger();
        pValueMapData[i].m_iFlowID = qry.field(1).asInteger();
        pValueMapData[i].m_iProcID = qry.field(2).asInteger();
        pValueMapData[i].m_iMapType= qry.field(3).asInteger();
        strncpy(pValueMapData[i].m_sOrgValue,  qry.field(4).asString(), 31);
        strncpy(pValueMapData[i].m_sDestValue, qry.field(5).asString(), 31);
        strncpy(pValueMapData[i].m_sEffDate,   qry.field(6).asString(), 16);
        strncpy(pValueMapData[i].m_sExpDate,   qry.field(7).asString(), 16);
        pValueMapData[i].m_iNextOffset = 0;
        pValueMapData[i].m_sOrgValue[31]  = '\0';
        pValueMapData[i].m_sDestValue[31] = '\0';

        sprintf(sKey, "%d_%s", pValueMapData[i].m_iMapType, pValueMapData[i].m_sOrgValue);

        m_poValueMapIndex->get(sKey, &pValueMapData[i].m_iNextOffset);
        m_poValueMapIndex->add(sKey, i);
    }
    
    qry.close();
    Log::log(0, "B_VALUE_MAP上载正常结束. 记录数: %d", m_poValueMapData->getCount());
}

void ParamInfoMgr::loadZoneItem()
{
    int iCount = 0;
    ZoneItem *pZoneItem = 0;
    char sSqlcode[512];
    TOCIQuery qry(Environment::getDBConn());

    sprintf(sSqlcode, " SELECT A.ZONE_ID, B.ZONE_ITEM_ID, C.ITEM_REF_VALUE "
            " FROM ZONE A, ZONE_ITEM B, ZONE_ITEM_VALUE C "
            " WHERE A.ZONE_ID = B.ZONE_ID "
            " AND B.ZONE_ITEM_ID = C.ZONE_ITEM_ID "
            " ORDER BY A.ZONE_ID, B.ZONE_ITEM_ID, C.ITEM_REF_VALUE ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ZONE_ITEM_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+135);
    }
    iCount = atoi(countbuf);

    if(!(m_poZoneItemData->exist()))
        m_poZoneItemData->create(iCount * m_fRatio);
    if(!(m_poZoneItemIndex->exist()))
        m_poZoneItemIndex->create(iCount * m_fRatio);
        if(!(m_poZoneItemStrIndex->exist()))
        m_poZoneItemStrIndex->create(iCount * m_fRatio, 45);
    pZoneItem= (ZoneItem *)(*m_poZoneItemData);
	checkMem(m_poZoneItemData->getCount() * sizeof(ZoneItem));
    // 数据区填值,并建立索引
    unsigned int i;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poZoneItemData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+136);

        pZoneItem[i].m_iZoneID = qry.field(0).asInteger();
        pZoneItem[i].m_iZoneItemID = qry.field(1).asInteger();//1
        strncpy(pZoneItem[i].m_sZoneItemValue, qry.field(2).asString(), 32);//2
        pZoneItem[i].m_iNextDifOffset = 0;
        pZoneItem[i].m_iNextOffset = 0;
                char sTmp[64] = {0};
                //m_poZoneItemStrIndex->add(sTmp,i);
        if(i == 1)
        {
            m_poZoneItemIndex->add((long)pZoneItem[i].m_iZoneID, i);
                        sprintf(sTmp,"%d_%d_%s",i,pZoneItem[i].m_iZoneItemID,pZoneItem[i].m_sZoneItemValue);
                        m_poZoneItemStrIndex->add(sTmp,i);
        }
        else
        {
            if(pZoneItem[i].m_iZoneID == pZoneItem[i-1].m_iZoneID)
            {
                if(pZoneItem[i].m_iZoneItemID == pZoneItem[i-1].m_iZoneItemID)
                {
                    pZoneItem[i-1].m_iNextOffset = i;
                }
                else
                {
                    unsigned int iOffset = 0;
                    if(m_poZoneItemIndex->get(pZoneItem[i].m_iZoneID, &iOffset))
                    {
                        while(pZoneItem[iOffset].m_iNextDifOffset != 0)
                        {
                            iOffset = pZoneItem[iOffset].m_iNextDifOffset;
                        }
                    }
                    pZoneItem[iOffset].m_iNextDifOffset = i;
                }
            }
            else
                m_poZoneItemIndex->add((long)pZoneItem[i].m_iZoneID, i);
                        //新增 add by hsir
                        unsigned int iIdx = 0;
                        if(m_poZoneItemIndex->get(pZoneItem[i].m_iZoneID, &iIdx))
                        {
                                sprintf(sTmp,"%d_%d_%s",iIdx,pZoneItem[i].m_iZoneItemID,pZoneItem[i].m_sZoneItemValue);
                                m_poZoneItemStrIndex->add(sTmp,i);
                        }
        }

    }

    qry.close();
    Log::log(0, "区表参数上载正常结束. 数据区记录数/总数: %d/%d;  索引区记录数/总数: %d/%d", 
        m_poZoneItemData->getCount(), m_poZoneItemData->getTotal(),
        m_poZoneItemIndex->getCount(), m_poZoneItemIndex->getTotal());
}

void ParamInfoMgr::loadSplitAcctItemGroup()
{
    int iCount = 0;
    SplitAcctItemGroup *pSplitAcctItemGroup = 0;
    char sSqlcode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
        
    strcpy(sSqlcode, " SELECT ACCT_ITEM_TYPE_ID,SPLIT_GROUP_ID,nvl(EVENT_TYPE_ID,-1) "
        " FROM B_SPLIT_ACCT_ITEM_GROUP ORDER BY EVENT_TYPE_ID, ACCT_ITEM_TYPE_ID ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SPLITACCTGROUP_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+211);
    }
    iCount = atoi(countbuf);

    if(!(m_poSplitAcctItemGroupData->exist()))
        m_poSplitAcctItemGroupData->create(iCount * m_fRatio);
    if(!(m_poSplitAcctItemGroupIndex->exist()))
        m_poSplitAcctItemGroupIndex->create(iCount * m_fRatio);

    pSplitAcctItemGroup = (SplitAcctItemGroup *)(*m_poSplitAcctItemGroupData);
	checkMem(m_poSplitAcctItemGroupData->getCount() * sizeof(SplitAcctItemGroup));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    unsigned int iIdx = 0;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poSplitAcctItemGroupData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+212);

        pSplitAcctItemGroup[i].m_iAcctItemType = qry.field(0).asInteger();
        pSplitAcctItemGroup[i].m_iSplitGroupID = qry.field(1).asInteger();
        pSplitAcctItemGroup[i].m_iEventTypeID  = qry.field(2).asInteger();
        pSplitAcctItemGroup[i].m_iNextEventOffset = 0;
        pSplitAcctItemGroup[i].m_iNextAcctItemOffset = 0;

        //先处理m_iEventTypeID第一次出现的情况; 加其加入索引
        if(!(m_poSplitAcctItemGroupIndex->get(pSplitAcctItemGroup[i].m_iEventTypeID, &iIdx)))
        {
            m_poSplitAcctItemGroupIndex->add(pSplitAcctItemGroup[i].m_iEventTypeID, i);
            if(i != 1)
            {
                pSplitAcctItemGroup[i].m_iNextEventOffset    = i - 1;
                pSplitAcctItemGroup[i].m_iNextAcctItemOffset = 0;
            }
        }
        else
        {
            //m_iEventTypeID再次出现;就要找到该m_iEventTypeID对应的链表
            pSplitAcctItemGroup[i].m_iNextEventOffset = pSplitAcctItemGroup[iIdx].m_iNextEventOffset;
            pSplitAcctItemGroup[i].m_iNextAcctItemOffset = iIdx;

            m_poSplitAcctItemGroupIndex->add(pSplitAcctItemGroup[i].m_iEventTypeID, i);
        }
    }
    
    qry.close();
    Log::log(0, "B_SPLIT_ACCT_ITEM_GROUP上载正常结束. 记录数: %d", m_poSplitAcctItemGroupData->getCount());
}

void ParamInfoMgr::loadSplitGroupEvent()
{
    int iCount = 0;
    SplitAcctGroupEvent *pSplitAcctGroupEvent = 0;
    char sSqlcode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
        
    strcpy(sSqlcode, " SELECT SPLIT_GROUP_ID,NVL(EVENT_TYPE_ID,0),NVL(OPERATION_ID,0) "
        " FROM B_SPLIT_ACCT_GROUP2EVENT_TYPE ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SPLITACCTEVENT_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+213);
    }
    iCount = atoi(countbuf);

    if(!(m_poSplitAcctGroupEventData->exist()))
        m_poSplitAcctGroupEventData->create(iCount * m_fRatio);
    if(!(m_poSplitAcctGroupEventIndex->exist()))
        m_poSplitAcctGroupEventIndex->create(iCount * m_fRatio);

    pSplitAcctGroupEvent = (SplitAcctGroupEvent *)(*m_poSplitAcctGroupEventData);
	checkMem(m_poSplitAcctGroupEventData->getCount() * sizeof(SplitAcctGroupEvent));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    unsigned int iIdx = 0;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poSplitAcctGroupEventData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+214);

        pSplitAcctGroupEvent[i].m_iSplitGroupID = qry.field(0).asInteger();
        pSplitAcctGroupEvent[i].m_iEventTypeID  = qry.field(1).asInteger();
        pSplitAcctGroupEvent[i].m_iOperationID  = qry.field(2).asInteger();

        m_poSplitAcctGroupEventIndex->add(pSplitAcctGroupEvent[i].m_iSplitGroupID, i);
    }
    
    qry.close();
    Log::log(0, "B_SPLIT_ACCT_GROUP2EVENT_TYPE上载正常结束. 记录数: %d", m_poSplitAcctGroupEventData->getCount());

}

void ParamInfoMgr::loadDistanceType()
{
    int iCount = 0;
    DistanceType *pDistanceType = 0;
    char sSqlcode[1024] = {0};
    TOCIQuery qry(Environment::getDBConn());
        
    strcpy(sSqlcode, " SELECT DISTANCE_TYPE_ID,NVL(LONG_TYPE_ID,0),NVL(ROAM_TYPE_ID,0) "
        " FROM B_DISTANCE_TYPE ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_DISTANCETYPE_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+215);
    }
    iCount = atoi(countbuf);

    if(!(m_poDistanceTypeData->exist()))
        m_poDistanceTypeData->create(iCount * m_fRatio);
    if(!(m_poDistanceTypeIndex->exist()))
        m_poDistanceTypeIndex->create(iCount * m_fRatio);

    pDistanceType = (DistanceType *)(*m_poDistanceTypeData);
	checkMem(m_poDistanceTypeData->getCount() * sizeof(DistanceType));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poDistanceTypeData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+216);

        pDistanceType[i].m_iDistanceTypeID = qry.field(0).asInteger();
        pDistanceType[i].m_iLongTypeID     = qry.field(1).asInteger();
        pDistanceType[i].m_iRoamTypeID     = qry.field(2).asInteger();

        m_poDistanceTypeIndex->add(pDistanceType[i].m_iDistanceTypeID, i);
    }
    
    qry.close();
    Log::log(0, "B_DISTANCE_TYPE上载正常结束. 记录数: %d", m_poDistanceTypeData->getCount());

}

void ParamInfoMgr::loadEdgeRoam()
{
    int iCount = 0;
    EdgeRoamRuleEx *pEdgeRoamRuleEx = 0;
    char sSqlcode[2048] = {0};
    TOCIQuery qry(Environment::getDBConn());
        
    strcpy(sSqlcode, "SELECT '1-'||TRIM(MSC)||DECODE(LAC,NULL,('-'),('-'||TRIM(LAC)))||DECODE(CELL_ID,NULL,'','-'||TRIM(CELL_ID)) SKEY, "
         " REGION_ID,NVL(TYPE,'12A') TYPE,1 ORG_TYPE,TO_CHAR(NVL(EFF_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')),'YYYYMMDDHH24MISS'), "
         "  TO_CHAR(NVL(EXP_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')),'YYYYMMDDHH24MISS'),TRIM(MSC) MSC FROM B_MOBILE_EQUIP_REGION  A ,ORG B "
         " WHERE A.REGION_ID =B.ORG_ID "
         " UNION "
         " ( "
         " SELECT '2-'||TRIM(MSC)||DECODE(LAC,NULL,('-'),('-'||TRIM(LAC)))||DECODE(CELL_ID,NULL,'','-'||TRIM(CELL_ID)) SKEY,"
         " A.REGION_ID,NVL(TYPE,'12A') TYPE,2 ORG_TYPE,TO_CHAR(NVL(EFF_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')), 'YYYYMMDDHH24MISS'),"
         " TO_CHAR(NVL(EXP_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')),'YYYYMMDDHH24MISS'),TRIM(MSC) MSC FROM B_MOBILE_EQUIP_REGION  A ,STAT_REGION B "
         " WHERE A.REGION_ID =B.REGION_ID  ) "
         " UNION "
         " ( "
         "     SELECT REGION_TYPE_ID||'-'||TRIM(MSC)||DECODE(LAC,NULL,('-'),('-'||TRIM(LAC)))||DECODE(CELL_ID,NULL,'','-'||TRIM(CELL_ID)) SKEY,"
         " A.REGION_ID,NVL(TYPE,'12A') TYPE ,B.REGION_TYPE_ID ORG_TYPE,TO_CHAR(NVL(EFF_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')), 'YYYYMMDDHH24MISS'),"
         " TO_CHAR(NVL(EXP_DATE,TO_DATE('19700101010101','YYYYMMDDHH24MISS')),'YYYYMMDDHH24MISS') ,TRIM(MSC) MSC FROM B_MOBILE_EQUIP_REGION  A ,B_SELF_DEFINE_REGION B "
         " WHERE A.REGION_ID =B.REGION_ID "
         " )   ORDER BY TYPE ");

    char countbuf[32] = {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_EDGEROAMRULEEX_NUM, countbuf))
    {
        THROW(MBC_ParamInfoMgr+217);
    }
    iCount = atoi(countbuf);

    if(!(m_poEdgeRoamRuleExData->exist()))
        m_poEdgeRoamRuleExData->create(iCount * m_fRatio);
    if(!(m_poEdgeRoamRuleExTypeAIndex->exist()))
        m_poEdgeRoamRuleExTypeAIndex->create(iCount * m_fRatio, 40);
    if(!(m_poEdgeRoamRuleExTypeBIndex->exist()))
        m_poEdgeRoamRuleExTypeBIndex->create(iCount * m_fRatio, 40);
    if(!(m_poEdgeRoamRuleExMSCIndex->exist()))
        m_poEdgeRoamRuleExMSCIndex->create(iCount * m_fRatio, 40);

    pEdgeRoamRuleEx = (EdgeRoamRuleEx *)(*m_poEdgeRoamRuleExData);
	checkMem(m_poEdgeRoamRuleExData->getCount() * sizeof(EdgeRoamRuleEx));
    // 数据区填值,并建立索引
    unsigned int i = 0;
    unsigned int iIdx = 0;
    int iEdgeType =-1;
    qry.setSQL(sSqlcode);
    qry.open();

    while(qry.next())
    {
        i = m_poEdgeRoamRuleExData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+218);

        char sType[4]={0};
        strcpy(sType, qry.field(2).asString());  //TYPE: 12A 12B
        switch(sType[2]|0x20)
        {
            case 'a':
            {
                iEdgeType =0;
            }
            break;
            case 'b':
            default:
            {
                iEdgeType = 1;
            }
        }
        
        pEdgeRoamRuleEx[i].m_iOrgID = qry.field(1).asInteger();
        pEdgeRoamRuleEx[i].m_nIsEdage = iEdgeType;
        pEdgeRoamRuleEx[i].m_iOrgType= qry.field(3).asInteger();
        strcpy(pEdgeRoamRuleEx[i].m_szEffDate, qry.field(4).asString());
        strcpy(pEdgeRoamRuleEx[i].m_szExpDate, qry.field(5).asString());
        pEdgeRoamRuleEx[i].pNext = 0;
        pEdgeRoamRuleEx[i].m_iNextOffset = 0;

        if(iEdgeType == 0)
        {
            if(m_poEdgeRoamRuleExTypeAIndex->get(qry.field(0).asString(), &iIdx))
            {
                pEdgeRoamRuleEx[i].m_iNextOffset = iIdx;
                m_poEdgeRoamRuleExTypeAIndex->add(qry.field(0).asString(), i);
            }
            else
            {
                m_poEdgeRoamRuleExTypeAIndex->add(qry.field(0).asString(), i);
                m_poEdgeRoamRuleExMSCIndex->add(qry.field(6).asString(), i);
            }
        }
        else
        {
            if(m_poEdgeRoamRuleExTypeBIndex->get(qry.field(0).asString(), &iIdx))
            {
                pEdgeRoamRuleEx[i].m_iNextOffset = iIdx;
                m_poEdgeRoamRuleExTypeBIndex->add(qry.field(0).asString(), i);
            }
            else
            {
                m_poEdgeRoamRuleExTypeBIndex->add(qry.field(0).asString(), i);
                m_poEdgeRoamRuleExMSCIndex->add(qry.field(6).asString(), i);
            }
        }
    }
    
    qry.close();
    Log::log(0, "B_MOBILE_EQUIP_REGION上载正常结束. 记录数: %d", m_poEdgeRoamRuleExData->getCount());

}

void ParamInfoMgr::loadSection()
{
    int i,lasti;
    int strategyID;
    int lastStrategyID = -1;
    lasti = 0;
    int count;

    PricingSection * pTemp;
    
    PricingSection * pSection;
    
    PricingSection * pLastSection = 0;
    
    TOCIQuery qry (Environment::getDBConn ());
    
    char countbuf[32] = {0};
    //memset(countbuf,'\0',sizeof(countbuf));
    int num = 0;
    //b_param_define("LOADPARAMINFO","PARAM_SECTIONINFO_NUM")
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SECTIONINFO_NUM, countbuf))
    {
          THROW(MBC_ParamInfoMgr+5);
          //num = 0;
    }else
    {
        num = atoi(countbuf);
    }
    memset(countbuf,'\0',sizeof(countbuf));
    
    if(!(m_poPricingSectionInfoData->exist()))
    m_poPricingSectionInfoData->create(num*m_fRatio);
    
    //<sectionID,Section>
    if(!(m_poSectionIndex->exist()))
    m_poSectionIndex->create(num*m_fRatio);
    
        //<ID, Section>
    if(!(m_poStrategyIndex->exist()))
    m_poStrategyIndex->create(num*m_fRatio);

    pSection = (PricingSection *)(*m_poPricingSectionInfoData);
	checkMem(m_poPricingSectionInfoData->getCount() * sizeof(PricingSection));
    //
    /*qry.setSQL (" select a.pricing_section_id, "
                " nvl(a.parent_section_id, -1) parent_section_id, "
                " nvl(a.event_pricing_strategy_id, -1) strategy_id, "
                " nvl(a.condition_id,-1), "
                " nvl(b.measure_id, -1) measure_id,"
                " nvl(b.measure_seq, 99999) measure_seq, "
                " nvl(b.usage_type_id, 0) usage_type_id "
                " from pricing_section a, b_strategy_measure b "
                " where (a.event_pricing_strategy_id = b.strategy_id(+) "
                " and a.measure_seq=b.measure_seq(+)) "
                " order by strategy_id, measure_seq, "
                " parent_section_id, pricing_section_seq ");
    */
    
	qry.setSQL(
		"select *  from ( "
		  " select a.pricing_section_id pricing_section_id,"
			" nvl(a.parent_section_id, -1) parent_section_id,  "
			" nvl(a.event_pricing_strategy_id, -1) strategy_id, " 
			" nvl(a.condition_id,-1) condition_id,  "
			" nvl(b.measure_id, -1) measure_id, "
			" nvl(b.measure_seq, 99999) measure_seq,  "
			" nvl(b.usage_type_id, 0) usage_type_id ,"
			" a.pricing_section_seq, "
			" 0 acct_item_type_id "
		 " from pricing_section a, b_strategy_measure b  "
			" where (a.event_pricing_strategy_id = b.strategy_id(+)  "
			" and a.measure_seq=b.measure_seq(+)) "
		" union all "
		  " select at.pricing_section_id pricing_section_id,"
			"	  nvl(at.parent_section_id, -1) parent_section_id,  "
			"	  nvl(at.member_strategy_id, -1) strategy_id,  "
			"	  nvl(at.condition_id,-1),  "
			"	  nvl(bt.measure_id, -1) measure_id, "
			"	  nvl(bt.measure_seq, 99999) measure_seq,  "
			"	  nvl(bt.usage_type_id, 0) usage_type_id,"
			"	  at.pricing_section_seq, "
			"     nvl(bt.acct_item_type_id,0) acct_item_type_id "
		  " from (select distinct aa.*,bb.member_strategy_id "
			"   from pricing_section aa,b_event_type_group_member bb "
			 "  where aa.event_pricing_strategy_id=bb.strategy_id "
			  " ) at, "
			   " ( "
				" select distinct bsm.strategy_id,betgm.member_strategy_id,"
					"   betgm.measure_method_id measure_id,"
					 "  betgm.measure_seq,"
					  " betgm.usage_type_id, "
					  " betgm.acct_item_type_id "
				 " from  "
					  " b_strategy_measure bsm,"
					  " event_pricing_strategy eps,"
					  " b_event_type_group_member betgm "
				" where bsm.strategy_id=eps.event_pricing_strategy_id  "
				 " and nvl(eps.type_flag,0) =1 "
				 " and bsm.strategy_id = betgm.strategy_id "
				 "  and bsm.measure_seq = betgm.measure_seq  "
			  " ) bt  "
			 " where (at.member_strategy_id = bt.member_strategy_id(+)  "
			 " and at.measure_seq=bt.measure_seq(+)) "
		 " ) "
		 " order by strategy_id, measure_seq,  "
		 " parent_section_id, pricing_section_seq ");

    qry.open ();

    int more;

    i = 0;  
	int iAcctItemTypeID = 0;
	int iParentSectionID = 0;
	int iSectionID = 0;
	unsigned int tmp = 0;
    //m_iMAXPricingSectionNum = 0;

    while (qry.next ()) 
    {
		iSectionID = qry.field(0).asInteger ();
		iParentSectionID = qry.field(1).asInteger ();
		if (iParentSectionID != -1 && m_poSectionIndex->get (iSectionID, &tmp)){
			continue;//过滤掉非度量的重复段落
		}

        i = m_poPricingSectionInfoData->malloc ();
        if (i <= 0) THROW(MBC_ParamInfoMgr+6);
                
        memset(&pSection[i],'\0',sizeof(class PricingSection));
        
        pSection[i].m_iSectionID = iSectionID;

        pSection[i].m_iParentSectionID = iParentSectionID;

        pSection[i].m_iConditionID = qry.field(3).asInteger ();

        strategyID = qry.field(2).asInteger ();
		iAcctItemTypeID = qry.field(8).asInteger ();

		if (m_poSectionIndex->get (iSectionID, &tmp)){
			while (pSection[tmp].m_iNextOffset) {//链接在后面
				tmp = pSection[tmp].m_iNextOffset;
			}
			pSection[tmp].m_iNextOffset = i;
		}else{
			m_poSectionIndex->add (pSection[i].m_iSectionID, i);//第一个父段落
		}

        pSection[i].m_iPricingSectionOffset = i;
        
        if (pSection[i].m_iParentSectionID==-1 && strategyID!=-1) 
        {
            pSection[i].m_iConditionID = qry.field(4).asInteger ();
            pSection[i].m_iChildNum = qry.field (5).asInteger ();
            pSection[i].m_iBreakValue = qry.field (6).asInteger ();
			pSection[i].m_iSectionType = iAcctItemTypeID;

            if (strategyID == lastStrategyID) 
            {           
                 while (pSection[lasti].m_iBrotherOffset > 0) 
                 {
                        lasti =  pSection[lasti].m_iBrotherOffset;
                 }
                 pSection[lasti].m_iBrotherOffset = i;
            } 
            else 
            {
                                //strategyID  != lastStrategyID 
                m_poStrategyIndex->add (strategyID, i); 
            }
        }
        lastStrategyID = strategyID;
        lasti = i;
        //m_iMAXPricingSectionNum++;
    }
    count = i;
    qry.close ();
        
	HashList<int> * pSectionTemp = new HashList<int>(count);
	if (pSectionTemp == NULL){
		Log::log (0, "审请临时空间失败，审请条数：%d",count);
		THROW(MBC_PricingInfoMgr+1);
	}
	int iTT = 0;
    for (i=1; i<count+1; i++) 
    {
        if (pSection[i].m_iParentSectionID != -1) {
			if (pSectionTemp->get(pSection[i].m_iSectionID,&iTT)){
				continue;
			}            
            if (m_poSectionIndex->get (pSection[i].m_iParentSectionID, &tmp)) {
				while(tmp){
                                if (pSection[tmp].m_iFirstChildOffset <= 0) {
                                pSection[tmp].m_iFirstChildOffset = i;
					}else 
					{
                            unsigned int tmp2 = 0; 
                        tmp2 = pSection[tmp].m_iFirstChildOffset;
                        if(tmp2>0){                                     
                                                        while (pSection[tmp2].m_iBrotherOffset > 0) {               
                                                                unsigned int tmp3 = 0;
                                tmp3 = pSection[tmp2].m_iBrotherOffset;
                                if(tmp3>0)  tmp2 = tmp3;
                                else  goto _next;
                            }
                                                }
                _next:      
                        pSection[tmp2].m_iBrotherOffset = i;                
                }
					tmp = pSection[tmp].m_iNextOffset;
				}
             }else{
                        THROW(MBC_ParamInfoMgr+7);
            }
        }
		pSectionTemp->add(pSection[i].m_iSectionID,i);
    }
	delete pSectionTemp;
	pSectionTemp = NULL;
}


//
void ParamInfoMgr::loadTariff()
{
    int i;
    int sectionID;
    int lastSectionID = 0;
    int lasti = 0;
    int m_iCount = 0;
    //m_iMAXTariffNum = 0;
        
    PricingSection *pSection;
        
    Tariff * pTariff;
        
    TOCIQuery qry (Environment::getDBConn ());
    char countbuf[32] = {0};
        
    int num = 0;
        
    HashList<int> precisionInfo (10);

    qry.setSQL ("select precision_id, conversion_rate from cfg_precision");

    qry.open ();
        
    while (qry.next ()) 
    {
        precisionInfo.add (qry.field(0).asInteger(), qry.field(1).asInteger ());
    }

    qry.close ();

    memset(countbuf,'\0',sizeof(countbuf));
    num = 0;
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TARIFFINFO_NUM, countbuf))
    {
          THROW(MBC_ParamInfoMgr+8);
        
    }else
    {
        num = atoi(countbuf);
    }
    memset(countbuf,'\0',sizeof(countbuf));
    if(!(m_poTariffInfoData->exist()))
    m_poTariffInfoData->create(num*m_fRatio);
	checkMem(m_poTariffInfoData->getCount() * sizeof(Tariff));
        //<TariffID,Tariff>
    if(!(m_poTariffIndex->exist()))
    m_poTariffIndex->create(num*m_fRatio);
    pTariff = (Tariff *)(*m_poTariffInfoData);  
    pSection = (PricingSection*)(*m_poPricingSectionInfoData);
#ifdef  CAL_BILLING_UNIT            
        qry.setSQL ("select a.tariff_id, a.tariff_type, a.pricing_section_id, "
                "a.resource_id, nvl(a.action_id, 0) action_id, "
                "a.acct_item_type_id, nvl(a.sub_product_id, 0) sub_product_id, "
                "b.standard_conversion_rate, a.calc_method_id, a.rate_unit, "
                "nvl(a.fixed_rate_value_id, 0) fixed_rate_value_id, "
                "a.scaled_rate_value_id, a.calc_priority, a.belong_cycle_duration, "
                "a.rate_value_precision," 
                "a.calculate_precision, "
                "a.istariff, nvl(a.measure_id, 0) measure_id, "
                                "a.unit_ifuseparam ,nvl(a.BILLING_UNIT,0) BILLING_UNIT "
                        "from tariff a, tariff_unit b "
                "where a.tariff_unit_id = b.tariff_unit_id "
                                "order by a.pricing_section_id, a.istariff");
#else   
  //
    qry.setSQL (" select a.tariff_id, a.tariff_type, a.pricing_section_id, "
                " a.resource_id, nvl(a.action_id, 0) action_id, "
                " a.acct_item_type_id, nvl(a.sub_product_id, 0) sub_product_id, "
                " b.standard_conversion_rate, a.calc_method_id, a.rate_unit, "
                " nvl(a.fixed_rate_value_id, 0) fixed_rate_value_id, "
                " a.scaled_rate_value_id, a.calc_priority, a.belong_cycle_duration, "
                " a.rate_value_precision," 
                " a.calculate_precision, "
                " a.istariff, nvl(a.measure_id, 0) measure_id, "
                " a.unit_ifuseparam "
                //" nvl(a.item_ifuseparam,'no') "
                " from tariff a, tariff_unit b "
                " where a.tariff_unit_id = b.tariff_unit_id "
                " order by a.pricing_section_id, a.istariff");
#endif
        
    qry.open ();
        
    i = 0;
        
    while (qry.next ()) {

        i = m_poTariffInfoData->malloc ();

        if (i <= 0) THROW(MBC_ParamInfoMgr+9);
        memset(&pTariff[i],'\0',sizeof(class Tariff));
        //
        pTariff[i].m_iTariffID = qry.field(0).asInteger ();
        
        strcpy(pTariff[i].m_sTariffType, qry.field(1).asString ()); 
                
        pTariff[i].m_iResource = qry.field(3).asInteger ();
                
        //pTariff[i].m_iAction = qry.field(4).asInteger ();
        pTariff[i].m_iAcctItemType = qry.field(5).asInteger ();
                
        //pTariff->m_iSubProduct = qry.field(6).asInteger ();
        pTariff[i].m_iUnitType = qry.field(7).asInteger ();
        //
        pTariff[i].m_iCalMethod = qry.field(8).asInteger ();
        //qry.field(9).asInteger ()*pTariff->m_iUnitType
        pTariff[i].m_iUnit = qry.field(9).asInteger () * pTariff[i].m_iUnitType;
                
        pTariff[i].m_lFixedRate = qry.field(10).asInteger ();
                
        pTariff[i].m_lRate = qry.field(11).asInteger ();
        
        //
        m_poTariffIndex->add(pTariff[i].m_iTariffID,i);
                
        //HashList<int> precisionInfo (10)
        precisionInfo.get (qry.field(14).asInteger (), 
                
                                    &(pTariff[i].m_iRatePrecision));
                
        precisionInfo.get (qry.field(15).asInteger (), 
                
                                    &(pTariff[i].m_iCalPrecision));


     // pricing_section_id
     sectionID = qry.field(2).asInteger ();

   char sTemp[8];

   strcpy (sTemp, qry.field (18).asString ());

   if ((sTemp[2] | 0x20) == 'y') 
   {
            pTariff[i].m_iIsParam = 1;
   } else {
            pTariff[i].m_iIsParam = 0;
   }


#ifdef CAL_BILLING_UNIT
        pTariff[i].m_iBillingUnit = qry.field(19).asInteger();
#endif

        if (sectionID == lastSectionID) 
        {
            pTariff[lasti].m_iNextOffset = i;
        } else {
                        //HashList<PricingSection *> *m_poSectionIndex  ,loadSection()
                        unsigned int m_iTmpOffset = 0;
                        if (m_poSectionIndex->get (sectionID, &m_iTmpOffset))  
                        {
                                pSection[m_iTmpOffset].m_iTariffOffset = i;
                        } else {
                                        THROW(MBC_ParamInfoMgr+7);
                        }
        }
                //record last time
                lasti = i;      
                lastSectionID = sectionID;
                //m_iMAXTariffNum++;
    }
    qry.close ();
}

//
void ParamInfoMgr::loadDiscount()
{   
    int i;
    int sectionID;
    int lastSectionID = 0;
    int lasti = 0;
    //m_iMAXDiscountNum = 0;
    Discount * pDiscount;
    DisctApplyer *pDisctApplyer;
    DisctApplyer::DisctApplyObject* pDisctApplyObject;
    PricingSection *pSection;
    pSection = (PricingSection*)(*m_poPricingSectionInfoData);
    //m_iMAXDisctApplyObjectNum = 0;
    //m_iMAXDisctApplyerNum = 0;
    char countbuf[32]= {0};
    int num = 0;
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_DISCOUNTINFO_NUM, countbuf))
    {
          THROW(MBC_ParamInfoMgr+11);
    }else
    {
        num = atoi(countbuf);
    }
    memset(countbuf,'\0',sizeof(countbuf));
    if(!(m_poDiscountInfoData->exist()))
    m_poDiscountInfoData->create(num*m_fRatio);
    checkMem(m_poDiscountInfoData->getCount() * sizeof(Discount));
    //<ID,>
    if(!(m_poDiscountIndex->exist()))
    m_poDiscountIndex->create(num*m_fRatio);
    pDiscount = (Discount *)(*m_poDiscountInfoData);    
    int discountID;
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_DISCTAPPLYERINFO_NUM, countbuf))
    {
          THROW(MBC_ParamInfoMgr+14);
    }else
    {
        num = atoi(countbuf);
    }
    memset(countbuf,'\0',sizeof(countbuf));
    if(!(m_poDisctApplyerInfoData->exist()))
    m_poDisctApplyerInfoData->create(num*m_fRatio);
    if(!(m_poDisctApplyerIndex->exist()))
    m_poDisctApplyerIndex->create(num*m_fRatio);
    pDisctApplyer = (DisctApplyer*)(*m_poDisctApplyerInfoData);
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_DISCTAPPLYEROBJINFO_NUM, countbuf))
    {
          THROW(MBC_ParamInfoMgr+17);
    }else
    {
        num = atoi(countbuf);
    }
    memset(countbuf,'\0',sizeof(countbuf));
    if(!(m_poDisctApplyObjectInfoData->exist()))
    m_poDisctApplyObjectInfoData->create(num*m_fRatio);
    if(!(m_poDisctApplyObjectIndex->exist()))
    m_poDisctApplyObjectIndex->create(num*m_fRatio);
    pDisctApplyObject = (DisctApplyer::DisctApplyObject*)(*m_poDisctApplyObjectInfoData);
	checkMem(m_poDisctApplyerInfoData->getCount() * sizeof(DisctApplyer));
	checkMem(m_poDisctApplyObjectInfoData->getCount() * sizeof(DisctApplyer::DisctApplyObject));
    TOCIQuery qry (Environment::getDBConn ());
    //
    qry.setSQL ( " select discount_id, cal_precision, priority, "
                 " pricing_section_id, operation_id, repatition_type, is_business "
                 " from discount_desc order by pricing_section_id, priority ");
    qry.open ();
    int t = 0;
    int t2 = 0;
    int tmpK = 0;
    int iTmp4 = 0;


    while (qry.next ()) 
    {   
        t = m_poDiscountInfoData->malloc ();

        if (t <= 0) THROW(MBC_ParamInfoMgr+12);
        memset(&pDiscount[t],'\0',sizeof(class Discount));
        pDiscount[t].m_iDiscountID = qry.field(0).asInteger ();

        pDiscount[t].m_iCalPrecision = qry.field(1).asInteger ();

        pDiscount[t].m_iPriority = qry.field(2).asInteger ();

        pDiscount[t].m_iOperationID = qry.field(4).asInteger ();

        //pDiscount->m_poApplyer = new DisctApplyer(qry.field(5).asString(), pDiscount[t].m_iDiscountID);  
        t2 = m_poDisctApplyerInfoData->malloc ();
        if (t2 <= 0) THROW(MBC_ParamInfoMgr+15);
        memset(&pDisctApplyer[t2],'\0',sizeof(DisctApplyer));

        transDisctApplyer(&pDisctApplyer[t2],qry.field(5).asString());       
        pDisctApplyer[t2].m_iDisctID = pDiscount[t].m_iDiscountID;

        //<DiscountID,DisctApplyer>  ,  DiscountID--n:1-->DisctApplyer
        m_poDisctApplyerIndex->add(pDiscount[t].m_iDiscountID,t2);


        //m_iMAXDisctApplyerNum++;
        //DisctApplyer
        pDiscount[t].m_iApplyer = t2;
                
        //strncpy(pDiscount[t].sApplyMethod,qry.field(5).asString(),sizeof(pDiscount[t].sApplyMethod));
        //strncpy(pDiscountTmp.sApplyMethod,pDiscount[t].sApplyMethod,sizeof(pDiscount[t].sApplyMethod));
        
        //if (!m_poApplyer) THROW (MBC_PricingInfoMgr+20);

        if (qry.field (6).asInteger ()) 
        {
            pDiscount[t].m_iRuleType = 4;
        } else {
            pDiscount[t].m_iRuleType = 5;
        }


        sectionID = qry.field(3).asInteger ();

        if (sectionID == lastSectionID) 
        {
            pDiscount[lasti].m_iNext = t;
        } else {
                        unsigned int iTmp = 0;
                        if (m_poSectionIndex->get (sectionID, &iTmp)) 
                        {
                            pSection[iTmp].m_iDiscountOffset = t;   
                        } else {
                            THROW(MBC_ParamInfoMgr+7);
                        }   
        }
        lasti = t;
        lastSectionID = sectionID;
        //m_poDiscountIndex->add (pDiscount->m_iDiscountID, pDiscount);                                       
        m_poDiscountIndex->add (pDiscount[t].m_iDiscountID, t);
        //m_iMAXDiscountNum++;
    }
    qry.close ();

    qry.setSQL ( "select discount_id, attr_owner_id, attr_id, "
            "nvl(reference_id, 0) reference_id from discount_apply_object");
    qry.open ();
    while (qry.next ()) 
    {
        discountID = qry.field(0).asInteger ();
        unsigned int iTmp3 = 0;
        if (m_poDiscountIndex->get (discountID, &iTmp3)) 
        {
            if(pDiscount[iTmp3].m_iApplyer>0 )
            {
                unsigned int iApplyerTmp = pDiscount[iTmp3].m_iApplyer;
                transApplyObject(iApplyerTmp, qry.field(1).asString (), qry.field (2).asInteger (), qry.field (3).asInteger ());
            }
        }
        else{
                THROW(MBC_ParamInfoMgr+13);
            }
    }

    qry.close ();

}

//  DisctApplyer::addApplyObject(char *sOwnerID, int iObjectID, int iReferenceID)
void ParamInfoMgr::transApplyObject(unsigned int iApplyTmp, char *sOwnerID, int iObjectID, int iReferenceID)
{
    DisctApplyer *pDisctApplyer = (DisctApplyer*)(*m_poDisctApplyerInfoData);
    DisctApplyer::DisctApplyObject *pDisctApplyObject = (DisctApplyer::DisctApplyObject*)(*m_poDisctApplyObjectInfoData);
    unsigned int iApplyObjTmp = pDisctApplyer[iApplyTmp].m_iApplyObject;
    unsigned int iTmp4 = 0;
    iTmp4 = m_poDisctApplyObjectInfoData->malloc ();
    if(iTmp4 <= 0)  THROW(MBC_ParamInfoMgr+18);
    memset(&pDisctApplyObject[iTmp4],'\0',sizeof(DisctApplyer::DisctApplyObject));
    //m_iMAXDisctApplyObjectNum++;
    if(iApplyObjTmp == 0)
    {
        pDisctApplyer[iApplyTmp].m_iApplyObject = iTmp4;
    }else{
            while(iApplyObjTmp>0)
            {
                if(pDisctApplyObject[iApplyObjTmp].m_iNext>0)
                {
                    iApplyObjTmp = pDisctApplyObject[iApplyObjTmp].m_iNext;
                }else{
                    pDisctApplyObject[iApplyObjTmp].m_iNext = iTmp4;
                    break;
                }
            }   
    }

        pDisctApplyObject[iTmp4].m_iObjectID = iObjectID;
        pDisctApplyObject[iTmp4].m_iReferenceID = iReferenceID;
        
    int i = sOwnerID[0] - '0';
    if (i>=0 && i<=9) {
        pDisctApplyObject[iTmp4].m_iExt = 0;
        pDisctApplyObject[iTmp4].m_iObjectOwnerID = atoi (sOwnerID);
    } else {
            if (35 == i){
            pDisctApplyObject[iTmp4].m_iExt = 3;
            }else{
                pDisctApplyObject[iTmp4].m_iExt = 1;
            }
        pDisctApplyObject[iTmp4].m_iObjectOwnerID = atoi (sOwnerID+1);
    }
}

// 
void ParamInfoMgr::transDisctApplyer(DisctApplyer *pDisctApplyer, char *sApplyMethod)
{
    switch (sApplyMethod[2]) 
    {
      case 'B':
        pDisctApplyer->m_iApplyMethod = 0;
        break;

      case 'D':
        pDisctApplyer->m_iApplyMethod = 3;
        break;

      case 'E':
        pDisctApplyer->m_iApplyMethod = 4;
        break;

      default:
        pDisctApplyer->m_iApplyMethod = 1;
        break;
    }
    return;
}

void ParamInfoMgr::loadPercent()
{
        int i; 
        int iSectionID;
        
        PricingSection *pSection;
        pSection = (PricingSection*)(*m_poPricingSectionInfoData);
    Percent *pPercent;
        char sTemp[4];
        
        TOCIQuery qry (Environment::getDBConn ());
        char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_PERCENTINFO_NUM, countbuf))
        {
                  THROW(MBC_ParamInfoMgr+20);
        }else
        {
                num = atoi(countbuf);
        }
                
        if(!(m_poPercentInfoData->exist()))
        m_poPercentInfoData->create(num*m_fRatio);
        
                //<PercentID,Percent>
                if(!(m_poPercentIndex->exist()))
        m_poPercentIndex->create(num*m_fRatio);
            
                pPercent = (Percent *)(*m_poPercentInfoData);   
        checkMem(m_poPercentInfoData->getCount() * sizeof(Percent));
            // qry.setSQL ("select  pricing_section_id, percent_nume, nvl(nume_ifuseparam,0),percent_deno,nvl(deno_ifuseparam,0)"

        qry.setSQL (" select  pricing_section_id, percent_nume, nvl(nume_ifuseparam,0) , percent_deno, nvl(deno_ifuseparam,0) "
                    " , percent_id, is_business "
                    "  from percent_disct ");
                qry.open ();
                i = 0;
            while (qry.next ()) 
            {
                    	i = m_poPercentInfoData->malloc ();
                        if (i <= 0) 
							THROW(MBC_ParamInfoMgr+21);
                        memset(&pPercent[i],'\0',sizeof(class Percent));
            			iSectionID = qry.field (0).asInteger ();

            			pPercent[i].m_iNume = qry.field (1).asInteger ();
						char sTmp[12] = {0};
                        strcpy( sTmp , qry.field (2).asString());
						pPercent[i].m_iNumeIsParam = atoi(sTmp);
						sTmp[0] = '\0';
                        pPercent[i].m_iDeno = qry.field (3).asInteger ();
                        strcpy( sTmp , qry.field (4).asString());
						pPercent[i].m_iDenoIsParam = atoi(sTmp);
						
                        //pPercent[i].m_iDenoIsParam = qry.field (4).asInteger ();
                        pPercent[i].m_iPercentID = qry.field (5).asInteger ();
                        m_poPercentIndex->add(pPercent[i].m_iPercentID, i);
                    if (!pPercent[i].m_iDeno) 
                    {
                                        Log::log (0, "Percent's denominator is 0, SectionID is: %d",iSectionID);
                        pPercent[i].m_iDeno = 1;
                        pPercent[i].m_iNume = 0;
            }
            if (qry.field (6).asInteger ()) 
            {
                pPercent[i].m_iRuleType = 2;        
            } else 
            {
                pPercent[i].m_iRuleType = 3;
            }
                        unsigned int m_iTmp = 0;
            if (m_poSectionIndex->get (iSectionID, &m_iTmp)) 
            {
                                        pSection[m_iTmp].m_iPercentOffset = i;
            } else {                                    
                        Log::log (0, "SECTION %d NOT FOUND", iSectionID);
                        THROW(MBC_ParamInfoMgr+7);
                        }
        }
            //m_iMAXPercentNum = i;
                qry.close ();   
}

void ParamInfoMgr::loadLifeCycle()
{
        //m_iMAXLifeCycleNum = 0;
        int i;
        LifeCycle * pLifeCycle;
        char sTemp[4];
        //m_iMAXLifeCycleNum = 0;
        
        char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_LIFECYCLEINFO_NUM, countbuf))
        {
                  THROW(MBC_ParamInfoMgr+23);
        }else
        {
                num = atoi(countbuf);
        }
        
        memset(countbuf,'\0',sizeof(countbuf));
    if(!(m_poLifeCycleData->exist()))
    m_poLifeCycleData->create(num*m_fRatio);
        //<LifeCycleID,LifeCycle>
        if(!(m_poLifeCycleIndex->exist()))
    m_poLifeCycleIndex->create(num*m_fRatio);
        pLifeCycle = (LifeCycle *)(*m_poLifeCycleData); 
        checkMem(m_poLifeCycleData->getCount() * sizeof(LifeCycle));
        TOCIQuery qry (Environment::getDBConn ());      
        qry.close ();
        
        qry.setSQL ("select life_cycle_id, eff_type, "
                " nvl(to_char(eff_date, 'yyyymmddhh24miss'), '19000101000000') eff_date, "
                " nvl(to_char(exp_date, 'yyyymmddhh24miss'), '19000101000000') exp_date,"
                " nvl(eff_value, -1) eff_value,"
                " nvl(exp_value, -1) exp_value "
                " from life_cycle");
        qry.open ();
        
        i = 0;
        while (qry.next ()) 
        {
        i = m_poLifeCycleData->malloc ();
                if(i <= 0)  THROW(MBC_ParamInfoMgr+24);
                memset(&pLifeCycle[i],'\0',sizeof(class LifeCycle));
                pLifeCycle[i].m_iCycleID = qry.field(0).asInteger ();
                strcpy (pLifeCycle[i].m_sEffDate, qry.field(2).asString ());
                strcpy (pLifeCycle[i].m_sExpDate, qry.field(3).asString ());
                pLifeCycle[i].m_iEffValue = qry.field (4).asInteger ();
                pLifeCycle[i].m_iExpValue = qry.field (5).asInteger ();

                strcpy (sTemp, qry.field(1).asString ());
                switch (sTemp[2] | 0X20) 
                {
                  case 'a':
                        pLifeCycle[i].m_iCycleType = ABSOLUTE_TYPE;
                        break;
                  case 'b':
                        pLifeCycle[i].m_iCycleType = RELATIVE_YEAR_TYPE;
                        break;
                  case 'c':
                        pLifeCycle[i].m_iCycleType = RELATIVE_MONTH_TYPE;
                        break;
                  case 'd':
                        pLifeCycle[i].m_iCycleType = RELATIVE_DAY_TYPE;
                        break;
                  case 'e':
                        pLifeCycle[i].m_iCycleType = MIX_YEAR_TYPE;
                        break;
                  case 'f':
                        pLifeCycle[i].m_iCycleType = MIX_MONTH_TYPE;
                        break;
                  case 'g':
                        pLifeCycle[i].m_iCycleType = MIX_DAY_TYPE;
                        break;
                  case 'h':
                        pLifeCycle[i].m_iCycleType = MOD_MONTH_TYPE;
                        break;
                  case 'i':
                        pLifeCycle[i].m_iCycleType = MOD_CYCLE_TYPE;
                        break;
                  case 'j':
                        pLifeCycle[i].m_iCycleType = ABSOLUTE_NOYEAR_TYPE;
                        break;
          case 'k':
            pLifeCycle[i].m_iCycleType = RELATIVE_CYCLE_TYPE;
            break;
          case 'l':
            pLifeCycle[i].m_iCycleType = MOD_NORMAL_MONTH_TYPE;
            break;
                  default:
                        pLifeCycle[i].m_iCycleType = 0;
                        break;
                }
                m_poLifeCycleIndex->add (pLifeCycle[i].m_iCycleID, i);  
        //m_iMAXLifeCycleNum++;
        }
        qry.close ();
}

void ParamInfoMgr::loadPricingMeasure()
{
        int i = 0;
        int lasti = 0;
        int iStrategyID;
        int iLastStrategyID = -1;
        PricingMeasure * pPricingMeasure;
        char sTemp[4] = {0};
        //m_iMAXPricingMeasureNum = 0;
        
        char countbuf[32]= {0};
        int num = 0;
        
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_MEASUREINFO_NUM, countbuf))
        {
                  THROW(MBC_ParamInfoMgr+26);
        }else
        {
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poPricingMeasureData->exist()))
    m_poPricingMeasureData->create(num*m_fRatio);
        //<PricingMeasureID,PricingMeasure>
        if(!(m_poPricingMeasureIndex->exist()))
    m_poPricingMeasureIndex->create(num*m_fRatio);
        //<StrategyID,PricingMeasure>
        if(!(m_poStrategyIDMeasureIndex->exist()))
    m_poStrategyIDMeasureIndex->create(num*m_fRatio);
        pPricingMeasure = (PricingMeasure *)(*m_poPricingMeasureData);  
		checkMem(m_poPricingMeasureData->getCount() * sizeof(PricingMeasure));
        //TOCIQuery qry (Environment::getDBConn ());
}

void ParamInfoMgr::loadUsageType()
{
        TOCIQuery qry (Environment::getDBConn ());
                int nCnt, i;
                
                UsageTypeMgr::_USAGE_TYPE_ *pUsageType;
                
                char countbuf[32]= {0};
                int num = 0;
                if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_USAGETYPEINFO_NUM, countbuf))
                {
                                THROW(MBC_ParamInfoMgr+70);
                }else{
                                num = atoi(countbuf);
                }
                memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poUsageTypeInfoData->exist()))
            m_poUsageTypeInfoData->create(num*m_fRatio);
        
                //<UsageTypeID,UsageType>
                if(!(m_poUsageTypeIndex->exist()))
                m_poUsageTypeIndex->create(num*m_fRatio);
                
                pUsageType = (UsageTypeMgr::_USAGE_TYPE_ *)(*m_poUsageTypeInfoData);    
    			checkMem(m_poUsageTypeInfoData->getCount() * sizeof(UsageTypeMgr::_USAGE_TYPE_));
        
        qry.setSQL(" SELECT USAGE_TYPE_ID,NVL(CAL_BILLING_UNIT,-1), nvl(USAGE_TYPE_CODE,'-1')  FROM  B_USAGE_TYPE ");
        qry.open();
        i= 0;
        while(qry.next())
        {
          i = m_poUsageTypeInfoData->malloc ();
          if(i <= 0)  THROW(MBC_ParamInfoMgr+71);
          pUsageType[i].iUsageTypeID = qry.field(0).asInteger();
          pUsageType[i].bNeedCal = qry.field(1).asInteger();
          strcpy(pUsageType[i].m_sUsageTypeCode,qry.field(2).asString());
          m_poUsageTypeIndex->add(pUsageType[i].iUsageTypeID,i);
        }
        //m_iMAXUsageTypeNum = i;
        qry.close();
}


void ParamInfoMgr::loadCombine()
{               
            int i;
                int lasti = 0;
                int planID;
                int lastPlanID = 0;
            //m_iMAXPricingCombineNum = 0;

            PricingSection * pSection;
            pSection = (PricingSection*)(*m_poPricingSectionInfoData);
            PricingCombine * pCombine;
                
            char countbuf[32]= {0};
            memset(countbuf,'\0',sizeof(countbuf));
            int num = 0;
                //num = 0;
                
            if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_COMBINEINFO_NUM, countbuf))
            {
                    THROW(MBC_ParamInfoMgr+35);
            }else{
                    num = atoi(countbuf);
            }
            memset(countbuf,'\0',sizeof(countbuf));
        
        if(!(m_poPricingCombineData->exist()))
        m_poPricingCombineData->create(num*m_fRatio);
        
            if(!(m_poPlanIDIndex->exist()))
        m_poPlanIDIndex->create(num*m_fRatio);
        
            if(!(m_poPricingCombineIndex->exist()))
        m_poPricingCombineIndex->create(num*m_fRatio);
                
            if(!(m_poStrategyCombineIndex->exist()))
        m_poStrategyCombineIndex->create(num*m_fRatio);
        
            //m_poVirtualStrategyMeasureIndex->create(num*1.3);
            //if(!(m_poVirtualStrategyCombineIndex->exist()))
            //m_poVirtualStrategyCombineIndex->create(num*1.3);
        
            pCombine = (PricingCombine *)(*m_poPricingCombineData);     
                PricingMeasure *pMeasure = (PricingMeasure*)(*m_poPricingMeasureData);
                checkMem(m_poPricingCombineData->getCount() * sizeof(PricingCombine));
        // char countbuf[32]= {0};
                memset(countbuf,'\0',sizeof(countbuf));
                num = 0;        
                if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FILTERINFO_NUM, countbuf))
                {
                        THROW(MBC_ParamInfoMgr+32);
                }else
                {
                        num = atoi(countbuf);
                }
                memset(countbuf,'\0',sizeof(countbuf));
                
        if(!(m_poFilterConditionData->exist()))
        m_poFilterConditionData->create(num*m_fRatio);
                checkMem(m_poFilterConditionData->getCount() * sizeof(FilterCond));
                if(!(m_poFilterConditionIndex->exist()))
        m_poFilterConditionIndex->create(num*m_fRatio);
                memset(countbuf,'\0',sizeof(countbuf)); 
                if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_CONDITIONPOINTINFO_NUM, countbuf))
                {
            Log::log(0,"CONDITIONPOINT malloc error,num : %d",num*m_fRatio);
                        THROW(MBC_ParamInfoMgr+29);
                }else
                {
                        num = atoi(countbuf);
                }
                memset(countbuf,'\0',sizeof(countbuf));          
        if(!(m_poConditionPointData->exist()))
        m_poConditionPointData->create(num*m_fRatio);
				checkMem(m_poConditionPointData->getCount() * sizeof(ConditionPoint));
                TOCIQuery qry (Environment::getDBConn ());
                char sType[4];
#ifdef USE_TARIFF_DISCOUNT
    qry.setSQL (
        "select * from ( "
        "select a.pricing_combine_id pricing_combine_id, "
        "a.pricing_plan_id pricing_plan_id, "
        "a.life_cycle_id life_cycle_id, "
        "a.calc_priority priority,  "
        "d.event_type_id event_type, "
	"d.member_strategy_id strategy_id, "
        "nvl(a.pricing_objects_id, 0) pricing_object_id, "
        "nvl(e.is_discount, 0) is_discount, "
        "nvl(e.disct_acct_item_type_id, 0) disct_acct_item_type_id, "
        "nvl(a.condition_id, 0) condition_id, "
        "c.event_type_id group_id  "//新增
	"from pricing_combine a,event_pricing_strategy c, "
	" (select distinct type_group_id,strategy_id,member_strategy_id,event_type_id "
	" from  b_event_type_group_member) d ,"
        "b_pricing_combine_ext e "
        "where c.event_pricing_strategy_id=a.event_pricing_strategy_id "
        "and a.pricing_combine_id=e.pricing_combine_id(+) "
        "and nvl(c.type_flag,0)=1 and c.event_type_id=d.type_group_id  "
	" and c.event_pricing_strategy_id=d.strategy_id   "
        "union all "
        "select a.pricing_combine_id pricing_combine_id, "
        "a.pricing_plan_id pricing_plan_id, "
        "a.life_cycle_id life_cycle_id, "
        "a.calc_priority priority,  "
        "c.event_type_id event_type, "
        "a.event_pricing_strategy_id strategy_id, "
        "nvl(a.pricing_objects_id, 0) pricing_object_id, "
        "nvl(e.is_discount, 0) is_discount, "
        "nvl(e.disct_acct_item_type_id, 0) disct_acct_item_type_id, "
        "nvl(a.condition_id, 0) condition_id, "
        "0 group_id  "//新增
        "from pricing_combine a,event_pricing_strategy c, "
        "b_pricing_combine_ext e "
        "where c.event_pricing_strategy_id=a.event_pricing_strategy_id "
        "and a.pricing_combine_id=e.pricing_combine_id(+) "
        "and nvl(c.type_flag,0)=0 ) "
        "order by pricing_plan_id, event_type ");
#else
        qry.setSQL (
        "select * from ( "
        "select a.pricing_combine_id pricing_combine_id,  "
        "a.pricing_plan_id pricing_plan_id, "
        "a.life_cycle_id life_cycle_id, "
        "a.calc_priority priority,  "
        "d.event_type_id event_type,  "
	"d.member_strategy_id strategy_id,  "
        "nvl(a.pricing_objects_id, 0) pricing_object_id,  "
        "nvl(a.condition_id, 0) condition_id, "
        "c.event_type_id group_id "//新增
	"from pricing_combine a,event_pricing_strategy c , "
	" ( select distinct type_group_id,strategy_id,member_strategy_id,event_type_id "
	" from  b_event_type_group_member) d   "
        "where c.event_pricing_strategy_id=a.event_pricing_strategy_id  "
        "and nvl(c.type_flag,0)=1 and c.event_type_id=d.type_group_id "
	" and c.event_pricing_strategy_id=d.strategy_id "
        "union all "
        "select a.pricing_combine_id pricing_combine_id,  "
        "a.pricing_plan_id pricing_plan_id, "
        "a.life_cycle_id life_cycle_id, "
        "a.calc_priority priority,  "
        "c.event_type_id event_type,  "
        "a.event_pricing_strategy_id strategy_id,  "
        "nvl(a.pricing_objects_id, 0) pricing_object_id,  "
        "nvl(a.condition_id, 0) condition_id, "
        "0 group_id "//新增
        "from pricing_combine a,event_pricing_strategy c  "
        "where c.event_pricing_strategy_id=a.event_pricing_strategy_id  "
        "and nvl(c.type_flag,0)=0) "
        "order by pricing_plan_id, event_type,pricing_combine_id ");
#endif
        qry.open ();
        i = 0;
    while (qry.next ()) 
    {
                i = m_poPricingCombineData->malloc ();
        if (i<=0) THROW(MBC_ParamInfoMgr+36);                   
                
        memset(&pCombine[i],'\0',sizeof(class PricingCombine));
        planID = qry.field(1).asInteger ();
                
        pCombine[i].m_iCombineID = qry.field(0).asInteger ();
        pCombine[i].m_iPricingPlanID = planID;
        pCombine[i].m_iObjectID = qry.field(6).asInteger ();
        pCombine[i].m_iEventType = qry.field(4).asInteger ();           
        pCombine[i].m_iPriority = qry.field(3).asInteger ();
                                        
#ifdef USE_TARIFF_DISCOUNT
        pCombine[i].m_bDiscount = qry.field (7).asInteger () ? true : false;
        pCombine[i].m_iDisctAcctItem = qry.field (8).asInteger ();
        pCombine[i].m_iConditionID = qry.field (9).asInteger ();
		pCombine[i].m_iTypeGroupID = qry.field (10).asInteger ();
#else                           
        pCombine[i].m_iConditionID = qry.field (7).asInteger ();
		pCombine[i].m_iTypeGroupID = qry.field (8).asInteger ();
#endif                          

        pCombine[i].m_iRelationOffset = 0;
        pCombine[i].m_iNext = 0;
        pCombine[i].m_iLifeCycle = 0;
        pCombine[i].m_iFirstMeasure = 0;
        
                unsigned int m_iLifeCycleTmp = 0;
                if(m_poLifeCycleIndex->get(qry.field(2).asInteger (),&m_iLifeCycleTmp))
        {   
            pCombine[i].m_iLifeCycle = m_iLifeCycleTmp;
                }else{
                    pCombine[i].m_iLifeCycle = 0;
                }                       
                        
                if (planID == lastPlanID) 
                {
                        pCombine[lasti].m_iNext = i;   
                } else {
                        m_poPlanIDIndex->add (planID, i);
                }
                //if(pCombine[i].m_iCombineID == 19331) 
                //              printf("%s\n","test point ...");
                m_poPricingCombineIndex->add(pCombine[i].m_iCombineID, i);      
                
                lasti = i;
                lastPlanID = planID;
                
                m_iCurEventType = pCombine[i].m_iEventType;
                
                if( m_iCurEventType==Environment::m_iNewCycleTypeId) 
                        m_iCurEventType=CYCLE_ACCT_EVENT_TYPE;          
                
        int iStrategyIDTemp = qry.field(5).asInteger ();          
                
                unsigned int m_iPCTemp = 0;
                if(m_poStrategyCombineIndex->get(iStrategyIDTemp,&(m_iPCTemp))){                                                      
            pCombine[i].m_iCombineType = pCombine[m_iPCTemp].m_iCombineType;                                            
            pCombine[i].m_iCycleType = pCombine[m_iPCTemp].m_iCycleType;
                        pCombine[i].m_iFirstMeasure = pCombine[m_iPCTemp].m_iFirstMeasure;
            continue;                   
        }
                
                //iStrategyIDTemp
                unsigned int m_iPCTemp2 = 0;
                if (!m_poStrategyIndex->get (iStrategyIDTemp, &m_iPCTemp2)) {                   
            //xgs--Log::log (0, "Strategy: %d, with no Section",iStrategyIDTemp);                       
            continue;
        }                                               
        
        makeTree (&pSection[m_iPCTemp2], &pCombine[i]);
        
        /*      */
                switch(m_iCurEventType){                
                case REAL_CYCLE_ACCT_EVENT_TYPE:
                 /*  */
                 makeFilter (&pCombine[i]);                     
                 break;
                                 
                case CYCLE_ACCT_EVENT_TYPE:
                 getCombineType (&pCombine[i]);
                 makeFilter (&pCombine[i]);
                 break;
                                 
                case REAL_ACCT_EVENT_TYPE:
                 /*  */
                 makeFilter (&pCombine[i]);
                 preCompile (&pCombine[i]);
                 break;
                                 
                case 0:                          
                 break;
                                                        
                default:
                        /*  */
                        preCompile (&pCombine[i]);
                        preCompileB (&pCombine[i]);         
                        break;
                }

        m_poStrategyCombineIndex->add (iStrategyIDTemp, i);
                //m_iMAXPricingCombineNum++;            
        }
        qry.close ();
    
}

void ParamInfoMgr::loadRentCfg()
{   
        int iCount = 0;
        long iProductID;
        int iEventTypeID;
        
        char sSqlcode[512] = {0};
        TOCIQuery qry(Environment::getDBConn());
        sprintf ( sSqlcode, "select product_id, event_type_id from b_cycle_fee_event_type where state='10A'" );
                
        char countbuf[32]= {0};
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_RENT_CONFIG_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+1);
        }
        iCount = atoi(countbuf);
        if(!(m_poRentIndex->exist()))
                m_poRentIndex->create(iCount * m_fRatio);
                        
        qry.setSQL(sSqlcode);
        qry.open();
        
        while(qry.next())
        {       
                iProductID = qry.field(0).asLong();
                iEventTypeID = qry.field(1).asInteger();
                        
                m_poRentIndex->add(iProductID, iEventTypeID);
        }
        
    qry.close();
    Log::log(0, "RentInfo参数上载正常结束. 记录数: %d", m_poRentIndex->getCount());
        
}
void ParamInfoMgr::loadProductorIDCfg()
{   
        int iCount = 0;
        long iProductID;
        
        char sSqlcode[512] = {0};
        TOCIQuery qry(Environment::getDBConn());
        sprintf ( sSqlcode, "select product_id  from product where nvl(gen_cyclefee,'Y') != 'N'" );
                
        char countbuf[32]= {0};
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_PRODUCT_CONFIG_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+1);
        }
        iCount = atoi(countbuf);
        if(!(m_poProductIndex->exist()))
                m_poProductIndex->create(iCount * m_fRatio);
                        
        qry.setSQL(sSqlcode);
        qry.open();
        
        while(qry.next())
        {       
                iProductID = qry.field(0).asLong();
                        
                m_poProductIndex->add(iProductID, 1);
        }
        
    qry.close();
    Log::log(0, "Product参数上载正常结束. 记录数: %d", m_poProductIndex->getCount());
        
}

void ParamInfoMgr::loadTrunkToBeNBR()
{
        char sSqlcode[1024] = {0};
        TOCIQuery qry(Environment::getDBConn());
        strcpy(sSqlcode, "Select decode(switch_id,Null,trunk_id,switch_id||' '||trunk_id) skey, "
        " switch_id,trunk_id,acc_nbr From b_trunk_to_nbr");
        qry.setSQL(sSqlcode);

        int iCount = 0;
        char countbuf[32] = {0};
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TRUNKTOBENBR_NUM, countbuf))
        {
            THROW(MBC_ParamInfoMgr+1);
         }
        iCount = atoi(countbuf);

        TTrunkToNBR *pTrunkToBeNBRTemp;
        if(!(m_poTrunkToBeNBRData->exist()))
        m_poTrunkToBeNBRData->create(iCount * m_fRatio);
        if(!(m_poTrunkToBeNBRIndex->exist()))
        m_poTrunkToBeNBRIndex->create(iCount * m_fRatio, 40);
        pTrunkToBeNBRTemp = (TTrunkToNBR *)(*m_poTrunkToBeNBRData);
		checkMem(m_poTrunkToBeNBRData->getCount() * sizeof(TTrunkToNBR));
        unsigned int i = 0;  
        qry.open();
        while (qry.next()) {
        char sKey[22];
        i = m_poTrunkToBeNBRData->malloc();
        if(!i)
            THROW(MBC_ParamInfoMgr+2);        
        strcpy( sKey, qry.field("skey").asString() );
        pTrunkToBeNBRTemp[i].m_iSwitchID = qry.field("switch_id").asInteger();
        strcpy(pTrunkToBeNBRTemp[i].m_sTrunkID, qry.field("trunk_id").asString());
        strcpy(pTrunkToBeNBRTemp[i].m_sAccNBR, qry.field("acc_nbr").asString());
        m_poTrunkToBeNBRIndex->add(sKey, i);                    
        }
        qry.close();
        Log::log(0, "b_trunk_to_nbr上载正常结束. 记录数: %d", m_poTrunkToBeNBRData->getCount());

}

void ParamInfoMgr::makeTree(PricingSection *pSection, PricingCombine *pCombine)
{ 
                
        PricingSection *m_poSection = (PricingSection*)(*m_poPricingSectionInfoData);
                UsageTypeMgr::stUsageType *m_poUsageType = (UsageTypeMgr::stUsageType*)(*m_poUsageTypeInfoData);
        unsigned int iSectTmp = 0;
                //iSectTmp = pSection->m_iPricingSectionOffset;
                m_poSectionIndex->get(pSection->m_iSectionID, &iSectTmp);
        
        PricingMeasure *pMeasure = (PricingMeasure*)(*m_poPricingMeasureData);
        
                unsigned int iMeasureTmp = 0;
        
                int lasti = 0;
                int lastMeasure = 0;
	while (pSection)
	{
		iMeasureTmp = m_poPricingMeasureData->malloc();
		if (iMeasureTmp<=0) THROW(MBC_ParamInfoMgr+27);
                
		memset(&pMeasure[iMeasureTmp],'\0',sizeof(class PricingMeasure));
		pMeasure[iMeasureTmp].m_iFirstSection = pSection->m_iFirstChildOffset;
		pMeasure[iMeasureTmp].m_iUsageType = pSection->m_iBreakValue;
		pMeasure[iMeasureTmp].m_iMeasureSeq = pSection->m_iChildNum;
		pMeasure[iMeasureTmp].m_iAcctItemTypeID = pSection->m_iSectionType;
		
		setMeasureOffset(iMeasureTmp,pSection->m_iConditionID);     
		
		//iMeasureTmp = pMeasure[iMeasureTmp].m_iNext;
        if(lasti == 0) 
        {
			pCombine->m_iFirstMeasure = iMeasureTmp;
		}else{
			pMeasure[lastMeasure].m_iNext = iMeasureTmp;
		}
        //m_iMAXPricingMeasureNum++;
        lasti++;
        lastMeasure = iMeasureTmp;
		
		pSection = pSection->m_iBrotherOffset >0 ? &(m_poSection[pSection->m_iBrotherOffset]) : 0;
            
		#ifdef CAL_BILLING_UNIT
        if(UsageTypeNeedCal(pMeasure[iMeasureTmp].m_iUsageType))
			pMeasure[iMeasureTmp].m_iCalBillingUnit = 1;
        else
			pMeasure[iMeasureTmp].m_iCalBillingUnit = 0;
		#endif
	}
    /*while (iSectTmp>0) 
                {       
                        iMeasureTmp = m_poPricingMeasureData->malloc();
                        
                        if (iMeasureTmp<=0) THROW(MBC_ParamInfoMgr+27);                 
                memset(&pMeasure[iMeasureTmp],'\0',sizeof(class PricingMeasure));
                                        
                pMeasure[iMeasureTmp].m_iFirstSection = m_poSection[iSectTmp].m_iFirstChildOffset;
        
                pMeasure[iMeasureTmp].m_iUsageType = m_poSection[iSectTmp].m_iBreakValue;
        
                pMeasure[iMeasureTmp].m_iMeasureSeq = m_poSection[iSectTmp].m_iChildNum;
				pMeasure[iMeasureTmp].m_iAcctItemTypeID = m_poSection[iSectTmp].m_iSectionType;
                setMeasureOffset(iMeasureTmp,m_poSection[iSectTmp].m_iConditionID);     
                //iMeasureTmp = pMeasure[iMeasureTmp].m_iNext;
                if(lasti == 0) 
                        {
                                pCombine->m_iFirstMeasure = iMeasureTmp;
                }else{  
                                pMeasure[lastMeasure].m_iNext = iMeasureTmp;
                        }
                //m_iMAXPricingMeasureNum++;
                lasti++;
                lastMeasure = iMeasureTmp;
        
                iSectTmp = m_poSection[iSectTmp].m_iBrotherOffset;
                
                        #ifdef CAL_BILLING_UNIT
            if(UsageTypeNeedCal(pMeasure[iMeasureTmp].m_iUsageType))
                        pMeasure[iMeasureTmp].m_iCalBillingUnit = 1;
            else
                        pMeasure[iMeasureTmp].m_iCalBillingUnit = 0;
                        #endif

	}*/
        
}


void ParamInfoMgr::makeFilter(PricingCombine *pCombine)
{
                // 
                PricingMeasure *pMeasure = (PricingMeasure*)(*m_poPricingMeasureData);
                unsigned int m_iFirstMeaTmp = pCombine->m_iFirstMeasure;

        PricingSection *pSection;
                pSection = (PricingSection*)(*m_poPricingSectionInfoData);

        LogicStatement *m_poLogic  = (LogicStatement*)(*m_poLogicStatementData);  
        AssignStatement *m_poAssign = (AssignStatement*)(*m_poAssignStatementData);
    
        Queue<PricingSection *> queue;
                
        Queue<Discount *> qdisct;               
                int m_iFilterNum = 0;
                                        
                FilterCond *pFilterCondition;
                pFilterCondition = (FilterCond*)(*m_poFilterConditionData);
                
        Discount *pDisct;
        Discount *pDiscount = (Discount*)(*m_poDiscountInfoData);   
        DisctApplyer *pDisctApplyer = (DisctApplyer*)(*m_poDisctApplyerInfoData);  
        DisctApplyer::DisctApplyObject *pApplyObject = (DisctApplyer::DisctApplyObject *)(*m_poDisctApplyObjectInfoData);   
        ID *pID;
                
                PricingSection *pSectionTmp;
                
                
                unsigned int m_iFirstChildTmp = 0;
        unsigned int m_iDiscountTmp = 0;
        unsigned int m_iLogicTmp = 0;
                
        bool bRep3 ;
        unsigned int m_iFirstSectTmp = 0;
         
                for( ; m_iFirstMeaTmp>0; m_iFirstMeaTmp = pMeasure[m_iFirstMeaTmp].m_iNext)        
        {               
            bRep3 = false;
            m_iFirstSectTmp  = pMeasure[m_iFirstMeaTmp].m_iFirstSection;
                        
                    for( ; m_iFirstSectTmp>0; m_iFirstSectTmp = pSection[m_iFirstSectTmp].m_iBrotherOffset)                             
            {            
                queue.put (&pSection[m_iFirstSectTmp]);                    
            }
                   
                    while(queue.get (pSectionTmp)) 
            {                             
                    m_iFirstSectTmp = 0;
                m_iFirstChildTmp  = pSectionTmp->m_iFirstChildOffset;
                if(m_iFirstChildTmp > 0)   
                {                   
                    for( ; m_iFirstChildTmp; m_iFirstChildTmp = pSection[m_iFirstChildTmp].m_iBrotherOffset)
                    {   
                            queue.put (&pSection[m_iFirstChildTmp]);                                                                            
                    }           
                } else {                        
                                                m_iDiscountTmp  = pSectionTmp->m_iDiscountOffset;                               

                                        if(m_iDiscountTmp > 0) 
                        { 
                                qdisct.put(&pDiscount[m_iDiscountTmp]);           
                        } 
                } 
                                
                                if(pSectionTmp->m_iConditionID <= 0)       
                continue;
  
                                if(!m_poLogicStatementIndex->get(pSectionTmp->m_iConditionID, &m_iLogicTmp)) continue;
                                                                                                            
            for( ; m_iLogicTmp >0; m_iLogicTmp = m_poLogic[m_iLogicTmp].m_iNextOffset) 
            {
                pID = m_poLogic[m_iLogicTmp].m_poParamID1;              
                if (pID->m_iAttrID == 500) 
                {
                    // pMeasure->insertFilter (pID->m_iExt, pID->m_iOwner, pID->m_lValue);
                    insertFilter3(m_iFirstMeaTmp, pID->m_iExt, pID->m_iOwner, pID->m_lValue);                                           
                }
                
                  
                // Modify 2008.07.09 sichuan 
                //pID = pCur->m_poParamID2;

                pID = m_poLogic[m_iLogicTmp].m_poParamID2;

                if (pID!=NULL && pID->m_iAttrID == 500) 
                {
                    insertFilter3(m_iFirstMeaTmp, pID->m_iExt, pID->m_iOwner, pID->m_lValue);
                    //pMeasure->insertFilter (pID->m_iExt, pID->m_iOwner, pID->m_lValue);                   
                }
            }
        }// while...end
        
                //  Queue<Discount *> qdisct;                            
        while (qdisct.get (pDisct)) 
        {                               
            unsigned int tmpNum = 0;
                        m_poDiscountIndex->get(pDisct->m_iDiscountID, &tmpNum);
                        if(tmpNum<=0) continue;
                                                
                for(; tmpNum>0; tmpNum = pDiscount[tmpNum].m_iNext)
            {
                pDisct = &(pDiscount[tmpNum]);
                                if (!bRep3) 
                {                                                
                          if(pDisct->m_iApplyer > 0)
                          {                             
                                if(pDisctApplyer[pDisct->m_iApplyer].m_iApplyMethod == 3) 
                        {               
                                bRep3 = true;
                        }
                    }
                }
                
                if(pDisct->m_iApplyer > 0)
                {                                                                                                          
                    if(pDisctApplyer[pDisct->m_iApplyer].m_iApplyMethod == 4) 
                        {
                                if(!(pMeasure[m_iFirstMeaTmp].m_iFilter) || pFilterCondition[pMeasure[m_iFirstMeaTmp].m_iFilter].m_iNext) 
                        {
                                Log::log (0, "the reference objection of COMBINE %d is not suit !",  pCombine->m_iCombineID);
                        }
                        //pDisctApplyer[pDisct->m_iApplyer].addRefObj2 (pMeasure[m_iFirstMeaTmp].m_iFilter);
                                                pDisctApplyer[pDisct->m_iApplyer].m_iRef = pMeasure[m_iFirstMeaTmp].m_iFilter;                          
                        }               
                }
                
                unsigned int iAssignTmp = 0;
                m_poAssignStatementIndex->get(pDisct->m_iOperationID, &iAssignTmp);

                while(iAssignTmp > 0)  
                {   
                    pID = m_poAssign[iAssignTmp].m_poParamID1;
                    if (pID->m_iAttrID == 500) 
                    {                                              
                        insertFilter3(m_iFirstMeaTmp, pID->m_iExt, pID->m_iOwner, pID->m_lValue);                                
                    }
                    
                    pID = m_poAssign[iAssignTmp].m_poParamID2;
                    
                    if (pID->m_iAttrID == 500) 
                    {
                        insertFilter3(m_iFirstMeaTmp, pID->m_iExt, pID->m_iOwner, pID->m_lValue);  
                    }
                    iAssignTmp = m_poAssign[iAssignTmp].m_iNextOffset;
                }       
 
                unsigned int iApplyObjTmp = pDisctApplyer[pDisct->m_iApplyer].m_iApplyObject;
                 
                if(iApplyObjTmp > 0)
                {
                        for(; iApplyObjTmp>0; iApplyObjTmp = pApplyObject[iApplyObjTmp].m_iNext)                     
                        {
                                if (pApplyObject[iApplyObjTmp].m_iObjectID != 500)  continue; 
                        // pMeasure->insertFilter  (p->m_iExt, p->m_iObjectOwnerID, p->m_iReferenceID);
                        insertFilter3(m_iFirstMeaTmp, pApplyObject[iApplyObjTmp].m_iExt, pApplyObject[iApplyObjTmp].m_iObjectOwnerID, pApplyObject[iApplyObjTmp].m_iReferenceID);                                                                                       
                        }
                }
            }//for...end
            
        }//while...end
        
        if (bRep3) 
        {
                if(pMeasure[m_iFirstMeaTmp].m_iFilter && pFilterCondition[pMeasure[m_iFirstMeaTmp].m_iFilter].m_iNext)
            {
                Log::log (0, "[WARNING], COMBINE%d is BT Discount,"                                                                                            
                            "maybe it is across the discount,please make sure", pCombine->m_iCombineID); 
            }
        }    
    }//for (; pMeasure; pMeasure=pMeasure->m_poNext) ...end                
}



void  ParamInfoMgr::insertFilter3(unsigned int m_iMeasureTmp, int iExt, int iMember, int iGroup)
{         
                  PricingMeasure *m_poPricingMeasure = (PricingMeasure *)(*m_poPricingMeasureData);
                  if(m_iMeasureTmp <= 0)  return;
                  
          FilterCond *pTemp2 ;       
          pTemp2 = (FilterCond*)(*m_poFilterConditionData);
           
          unsigned int tmp2 = 0;
          tmp2 = m_poPricingMeasure[m_iMeasureTmp].m_iFilter;
                  unsigned int tmp3 = tmp2;
          for (; tmp2 > 0; tmp2 = pTemp2[tmp2].m_iNext) 
          {
                  if (pTemp2[tmp2].m_iExt != iExt) continue;
                  if (pTemp2[tmp2].m_iMemberID != iMember) continue;
                  if (pTemp2[tmp2].m_iGroupID != iGroup) continue;                                                      
                  return;
          }
                
                  unsigned int iFilterTmp = 0;
                  iFilterTmp = m_poFilterConditionData->malloc();                 
          if(iFilterTmp > 0)
          {
                  pTemp2[iFilterTmp].set2 (iExt, iMember, iGroup, tmp3);
                          m_poPricingMeasure[m_iMeasureTmp].m_iFilter = iFilterTmp;     
                  }else{
                                        Log::log (0, "[ERROR], the space for FilterCondition is not enough"                                                                                                                                                                             
                                 ", please make sure."); 
                                        THROW(MBC_ParamInfoMgr+33);                                     
                  }
}


void ParamInfoMgr::preCompile(PricingCombine *pCombine)
{
                PricingCombine *pPricingCombine = (PricingCombine*)(*m_poPricingCombineData);
                unsigned int iCombineTmp = 0;
                
                m_poPricingCombineIndex->get(pCombine->m_iCombineID, &iCombineTmp);
                if(iCombineTmp <= 0) return;
                
                PricingMeasure * m_poPricingMeasure=  (PricingMeasure*)(*m_poPricingMeasureData);
                unsigned int iFirstMeaTmp = 0;
                iFirstMeaTmp = pCombine->m_iFirstMeasure;

        PricingSection *pSectionTemp, *pSectionTemp1;
                PricingSection * m_poPricingSection= (PricingSection*)(*m_poPricingSectionInfoData);
                
        LogicStatement *m_poLogicStatement;
                m_poLogicStatement = (LogicStatement*)(*m_poLogicStatementData);
                ConditionPoint *pConditionPoint = (ConditionPoint*)(*m_poConditionPointData);
                
        Queue<PricingSection *> queue;
        vector<ConditionPoint> vPoint;
        vector< vector<ConditionPoint> > vPointReg;
        vector<int> vOp;         
        int iOp;
                
        unsigned int iMallocTmp = 0;
                unsigned int ipChangeTmp = 0; 
                for(; iFirstMeaTmp > 0; iFirstMeaTmp = m_poPricingMeasure[iFirstMeaTmp].m_iNext)
        {
            unsigned int iTmpFirSection1 = m_poPricingMeasure[iFirstMeaTmp].m_iFirstSection;                    
                        unsigned int iLogicTmp = 0;
            int iLogicTmp2 = 0;
            iLogicTmp2 = m_poPricingSection[iTmpFirSection1].m_iConditionID;
            if((iLogicTmp2>0)&&m_poLogicStatementIndex->get(iLogicTmp2, &iLogicTmp))
            {
                                iMallocTmp = 0;
                        iMallocTmp = m_poPricingSectionInfoData->malloc();    
                memset(&(m_poPricingSection[iMallocTmp]), '\0', sizeof(class PricingSection));                                   
                if(iMallocTmp <= 0)
                {
                    Log::log(0,"SECTION ERROR,THE NUM OF SECTION NEDD TO CHANGE,RESULT: %d",iMallocTmp);
                                        THROW(MBC_ParamInfoMgr + 6);
                                        return;
                }
                                PricingSection * p = &(m_poPricingSection[iMallocTmp]);         
                p->m_iConditionID = -1;                         
                                p->m_iFirstChildOffset = iTmpFirSection1;                                       
                                m_poPricingMeasure[iFirstMeaTmp].m_iFirstSection = iMallocTmp;                                                                                                                  
                                p->m_iPricingSectionOffset = iMallocTmp;                                                    
            }
                                
                        unsigned int iFMeaSection2 = m_poPricingMeasure[iFirstMeaTmp].m_iFirstSection;
                        if(iFMeaSection2>0)
                        {
                                queue.put (&m_poPricingSection[iFMeaSection2]);
            }
                      
            while (queue.get(pSectionTemp)) 
            {   
                            unsigned int iSectionTmp = 0;
                        unsigned int iSectionIndex = 0;
                                if((!pSectionTemp)||(pSectionTemp == &m_poPricingSection[0])) continue;                                 
                                pSectionTemp->m_iBreakValue = 0;                                            
                vPointReg.clear();
                vOp.clear(); 
                                            
                PricingSection *pChange     = 0;
                PricingSection **ppChange   = &pChange;
                PricingSection *pLast = 0;           
                int iType = 0;              
                pSectionTemp->m_iSectionType = -1;                              
                if(pSectionTemp->m_iFirstChildOffset <= 0)
                {
                    pSectionTemp->m_iSectionType = SECTION_LEAF;
                }                               
                unsigned int m_ipSectionTemp1 = pSectionTemp->m_iFirstChildOffset;
                while (m_ipSectionTemp1 > 0)
                {                                                                    
                    iLogicTmp = 0;            
						if((m_poPricingSection[m_ipSectionTemp1].m_iConditionID==-1)||!m_poLogicStatementIndex->get(m_poPricingSection[m_ipSectionTemp1].m_iConditionID, &iLogicTmp))
                        //if(!m_poLogicStatementIndex->get(m_poPricingSection[m_ipSectionTemp1].m_iConditionID, &iLogicTmp))
                        {                      
                        	if (pChange && pChange != &m_poPricingSection[0])
                            {
                            	ipChangeTmp = 0;
								int iTmp = 0;
                                m_poSectionIndex->get(pChange->m_iSectionID, &ipChangeTmp);                          
								iTmp = ipChangeTmp;
								while(m_poPricingSection[iTmp].m_iBrotherOffset>0){
									iTmp = m_poPricingSection[iTmp].m_iBrotherOffset;
								}
								m_poPricingSection[iTmp].m_iBrotherOffset=m_ipSectionTemp1;
                                iMallocTmp = 0;
                                iMallocTmp = m_poPricingSectionInfoData->malloc();                                  
                				memset(&(m_poPricingSection[iMallocTmp]), '\0', sizeof(class PricingSection)); 
                            	if(iMallocTmp <= 0)
                            	{
                                	THROW(MBC_ParamInfoMgr + 6);
                            	}
                                PricingSection * p = &(m_poPricingSection[iMallocTmp]);
                                p->m_iPricingSectionOffset = iMallocTmp;      
                            	p->m_iConditionID = -1;
                                pLast->m_iBrotherOffset = iMallocTmp;
                                p->m_iFirstChildOffset = ipChangeTmp;                                                                                               
                            	queue.put (p);
                                pSectionTemp->m_iElseSection = iMallocTmp;                                                                                                                                                                                                                              
                        	}else{                                                                                                                                          
                        		pSectionTemp->m_iElseSection = m_ipSectionTemp1;
                                queue.put (&(m_poPricingSection[m_ipSectionTemp1]));
                        	}
                    		m_ipSectionTemp1 = 0;                    
                    		continue;
                		}//if...end
                vPoint.clear ();                                                                                                                                
                if(pCombine->m_iEventType == REAL_ACCT_EVENT_TYPE)
                {
                	if(iLogicTmp > 0)
                    {
                    	iType = setAcctBreakPoint (iFirstMeaTmp, &m_poLogicStatement[iLogicTmp], &m_poPricingSection[m_ipSectionTemp1], vPoint, iOp);                           
                    }
                } else {                                        
                	if(iLogicTmp > 0)
                    {
                    	iType = setBreakPoint (iFirstMeaTmp, &m_poLogicStatement[iLogicTmp], &m_poPricingSection[m_ipSectionTemp1], vPoint, iOp);  
                    }
                }                                                               
                if (iType == SECTION_AGGR || iType == SECTION_AGGR2) 
                {
                      pCombine->m_iCombineType = 1;
                }                                                               
                if (pSectionTemp->m_iSectionType != iType)               
                {
                    if (pSectionTemp->m_iSectionType == -1) 
                    {                                                                                           
                        pSectionTemp->m_iSectionType = iType;                                                                                           
                        vPointReg.push_back (vPoint);                                                                                           
                        vOp.push_back (iOp);                                            
                        queue.put (&m_poPricingSection[m_ipSectionTemp1]);
                        pLast = &(m_poPricingSection[m_ipSectionTemp1]);                                                                
                                                memcpy(pSectionTemp->m_poNewBreakID,m_poPricingSection[m_ipSectionTemp1].m_poNewBreakID,sizeof(ID));                    
                        if (iType==SECTION_MSPAN || iType==SECTION_MAGGR) 
                        {                           
                            pSectionTemp->m_iBreakValue = m_poPricingSection[m_ipSectionTemp1].m_iBreakValue;
                        }
                    } else {
                        pLast->m_iBrotherOffset = m_poPricingSection[m_ipSectionTemp1].m_iBrotherOffset;
                        m_poPricingSection[m_ipSectionTemp1].m_iBrotherOffset = 0;
                        if(!pChange){
                        	pChange = &(m_poPricingSection[m_ipSectionTemp1]);
                        	iSectionIndex = m_ipSectionTemp1;
                        }else{
                        	m_poPricingSection[iSectionIndex].m_iBrotherOffset = m_ipSectionTemp1;
                        	iSectionIndex = m_ipSectionTemp1;
                        }
                    }
                } else {
                    vPointReg.push_back (vPoint);
                    vOp.push_back (iOp);
                    queue.put (&m_poPricingSection[m_ipSectionTemp1]);                                                        
                    pLast = &m_poPricingSection[m_ipSectionTemp1];                                      
                }
                m_ipSectionTemp1 = pLast->m_iBrotherOffset;                                                     
                if (m_ipSectionTemp1 <= 0) 
                {               
                    if (pChange && pChange != &m_poPricingSection[0]) 
                    {
                    	iMallocTmp = 0;
                    	iMallocTmp = m_poPricingSectionInfoData->malloc();                                              
               	 		memset(&(m_poPricingSection[iMallocTmp]), '\0', sizeof(class PricingSection)); 
                        if(iMallocTmp <= 0)
                        {
                            THROW(MBC_ParamInfoMgr + 6);
                        }
                        PricingSection * p = &(m_poPricingSection[iMallocTmp]);                                 
                        p->m_iPricingSectionOffset = iMallocTmp;    
                        p->m_iConditionID = -1;
                        pLast->m_iBrotherOffset = iMallocTmp;
                        p->m_iFirstChildOffset = pChange->m_iPricingSectionOffset;
                        queue.put (p);
                        pSectionTemp->m_iElseSection = iMallocTmp;
                    }
                }
            }

            if (pSectionTemp->m_iSectionType != SECTION_NORM) 
            {
            	iSectionTmp = 0;
            	m_poSectionIndex->get(pSectionTemp->m_iSectionID, &iSectionTmp);
                if(iSectionTmp == 0) iSectionTmp = pSectionTemp->m_iPricingSectionOffset;
                PricingSection *m_poPricingSectionTmp9 = &(m_poPricingSection[pSectionTemp->m_iElseSection]);
                unsigned int iConditionPTmp = pSectionTemp->m_iConditionPointOffset;
                ConditionPoint *m_poConditionPointTmp9 = &(pConditionPoint[iConditionPTmp]);
                analysePoint(vPointReg, vOp, pSectionTemp, &(pSectionTemp->m_iChildNum), 
                 &m_poPricingSectionTmp9, pSectionTemp->m_iSectionType, iSectionTmp);
            }
        }
    }
}


int ParamInfoMgr::setAcctBreakPoint (unsigned int iMeasureTmp, LogicStatement *statement, PricingSection *pSection, vector<ConditionPoint> & vPoint, int &iOp)
{               
            int iBreakType = -1;
            Value value;
            int iValue2, iValue3, iDesValue;
        	int iValueType2, iValueType3, iDesValueType;
        	ConditionPoint point;
                
                PricingMeasure *m_poPricingMeasure = (PricingMeasure*)(*m_poPricingMeasureData);
                
                SplitCfg *pSplitCfg = (SplitCfg*)(*m_poSplitData);   //case FUNC_NOTBETWEEN     
                
                if(iMeasureTmp <= 0) return -1;
                PricingMeasure *pPricingMeasure = &(m_poPricingMeasure[iMeasureTmp]);           
                int iID = statement->m_poParamID1->m_iAttrID;

                /*pPricingMeasure->m_poID->m_iAttrID = statement->m_poParamID1->m_iAttrID;
                pPricingMeasure->m_poID->m_iOwner = statement->m_poParamID1->m_iOwner;
                pPricingMeasure->m_poID->m_iExt = statement->m_poParamID1->m_iExt;
                pPricingMeasure->m_poID->m_lValue = statement->m_poParamID1->m_lValue;
                strcpy(pPricingMeasure->m_poID->m_sValue, statement->m_poParamID1->m_sValue);*/
		memcpy(pPricingMeasure->m_poID, statement->m_poParamID1, sizeof(class ID));
                        
		if (iID != 500) return SECTION_NORM;
        if (pPricingMeasure->m_poID->m_iOwner<2) return SECTION_NORM;

        if (pPricingMeasure->m_iMeasureMethod > 0) 
        {
            pPricingMeasure->m_iMeasureMethod = 0 - pPricingMeasure->m_poID->m_lValue;
        }
        pPricingMeasure->m_lOffset = offsetof(StdEvent, m_lDuration);     
        iValue2 = (int)(statement->m_poParamID2->m_lValue);
        iValueType2 = statement->m_poParamID2->m_iAttrID;

        iValue3 = (int)(statement->m_poParamID3->m_lValue);
        iValueType3 = statement->m_poParamID3->m_iAttrID;

        iDesValue = (int)(statement->m_poDesParamID->m_lValue);
        iDesValueType = statement->m_poDesParamID->m_iAttrID;

        switch (statement->m_iFunctionID) 
        {
            case FUNC_BETWEEN:
                    point.setValue2 (iValue2, iValueType2, 0, 0);
                    vPoint.push_back (point);

                    point.setValue2 (iValue3, iValueType3, pSection->m_iPricingSectionOffset, 1);
                    vPoint.push_back (point);
                                        
                    iOp = 2;            
                    break;
            case FUNC_SPLIT_DECIDE:
                        		static SplitCfg pSplitCfg;
                                if(queryBySplitID(statement->m_poParamID2->m_lValue,&pSplitCfg))
                                {
                                        point.setValue2(atol(pSplitCfg.m_sStartVal), 0, 0, 0);
                                        vPoint.push_back (point);

                                        point.setValue2(atol(pSplitCfg.m_sEndVal), 0, pSection->m_iPricingSectionOffset, 1);
                                        vPoint.push_back (point);                       
                                }else{
                                        iValue2 = 0;
                                        iValueType2 = 0;
                                        iValue3 = 0;
                                        iValueType3 = 0;
                                }
                        		iOp = 2;
                        		break;
        case FUNC_NOTBETWEEN:
                point.setValue2 (iValue2, iValueType2, pSection->m_iPricingSectionOffset, 0);
                vPoint.push_back (point);
                                
                point.setValue2 (iValue3, iValueType3, 0, 1);
                vPoint.push_back (point);
                                
                point.setValue2 (MAX_VALUE, 0, pSection->m_iPricingSectionOffset, 0);
                vPoint.push_back (point);
                
                iOp = 3;
                break;
      case FUNC_NONE:
        switch (statement->m_iOperatorID) 
        {                       
                                case OP_GT: // >
                                        point.setValue2 (iDesValue, iDesValueType, 0, 1);
                                        vPoint.push_back (point);
                        
                                        point.setValue2 (MAX_VALUE, 0, pSection->m_iPricingSectionOffset, 0);
                                        vPoint.push_back (point);

                                        iOp = 3;
                                        break;
                                case OP_LT: // <
                                                point.setValue2 (iDesValue, iDesValueType, pSection->m_iPricingSectionOffset, 0);
                                        vPoint.push_back (point);

                                        iOp = 1;
                                        break;
                                case OP_EQ: // =
                                         point.setValue2 (iDesValue, iDesValueType, 0, 0);
                                         vPoint.push_back (point);

                                         point.setValue2 (iDesValue, iDesValueType, pSection->m_iPricingSectionOffset, 1);
                                         vPoint.push_back (point);

                                        iOp = 4;
                                        break;
                                case OP_GE: // >=
                                        point.setValue2 (iDesValue, iDesValueType, 0, 0);
                                        vPoint.push_back (point);
                                        
                                        point.setValue2 (MAX_VALUE, 0, pSection->m_iPricingSectionOffset, 0);
                                        vPoint.push_back (point);

                                        iOp = 3;
                                        break;
                                case OP_LE: // <=
                                                point.setValue2 (iDesValue, iDesValueType, pSection->m_iPricingSectionOffset, 1);
                                        vPoint.push_back (point);

                                        iOp = 1;
                                        break;
                                case OP_NT: // <>
                                        point.setValue2 (iDesValue, iDesValueType, pSection->m_iPricingSectionOffset, 0);
                                        vPoint.push_back (point);
                                        point.setValue2 (iDesValue, iDesValueType, 0, 1);
                                        vPoint.push_back (point);
                                        
                                        point.setValue2 (MAX_VALUE, 0, pSection->m_iPricingSectionOffset, 0);
                                        vPoint.push_back (point);

                                        iOp = 4;
                                        break;
                                case OP_LIKE: //LIKE
                            {
                            char m_sValue[32];
                            strcpy(m_sValue,statement->m_poDesParamID->m_sValue);
                            int nLen = strlen(m_sValue);
                            char sValueMax[9]={0};
                            char sValueMin[9]={0};
                            int iValueLen = 0;
                            if( 74 ==iID)   //
                                iValueLen = 6;
                            else
                            {
                                    if( 75 == iID)
                                iValueLen = 8;     //
                                    else
                                    {
                                    //Like no 
                                    Log::log(0,"Like strings are outside the rule Condition_ID:%d",100);
                                    return SECTION_NORM ;
                                    }
                            }
                        if((iValueLen==nLen)&&('%'!=m_sValue[nLen-1]))
                        {
                                //'%'
                                if(74==iID)
                                {
                                if(-1==Date::checkTime(m_sValue)) //
                                        return SECTION_NORM;
                                }else
                                {
                                if(-1 ==Date::checkDate(m_sValue)) //
                        return SECTION_NORM ;
                                }
                                //no like =
                                point.setValue2 (iDesValue, iDesValueType, 0, 0);
                                vPoint.push_back (point);
                                
                                point.setValue2 (iDesValue, iDesValueType, pSection->m_iPricingSectionOffset, 1);
                                vPoint.push_back (point);
                                iOp = 4;
                                break ;
                        }
                                        int j=0;
                                        if((nLen>iValueLen)||((nLen<iValueLen)&&('%'!=m_sValue[nLen-1])))
                                        {
                                                //
                                                //
                                                Log::log(0,"Like are outside the rule Condition_ID:%d",100);
                                                return SECTION_NORM ;
                                        }else
                                        {
                                                                //  2008% --->20080101
                                for(int i =0;i<iValueLen;i++)
                                {
                                        if('%'==m_sValue[i])
                                        {
                                                j=i;
                                                strncpy(sValueMin,m_sValue,i);
                                                sValueMin[i]=0;
                                        if(75 == iID)
                                        {
                                        if((i>0)&&(m_sValue[i-1]=='0'))
                                                        j=j+iValueLen;
                                        strcat(sValueMin,Date::m_sDateMin[j]);
                                        }else
                                        {
                                        strncat(sValueMin,"000000",iValueLen-i);
                                        }
                                        j = j%iValueLen;
                                        break;
                                }
                            }
                    }
            strcpy(sValueMax,sValueMin);
            Date d1,d2,d3;
            if(  75 ==iID)
            {
                if(-1 == Date::checkDate(sValueMin))
                    return SECTION_NORM;
                d1.parse(sValueMin,"YYYYMMDD");
                d2.parse(sValueMin,"YYYYMMDD");
                if(j<3)
                    d2.parse("20491231","YYYYMMDD");
                else if(j==3)
                {
                    d2.addYearAsOracle(9);
                    d2.addSec(-1);
                }else if(j==4)
                {
                    d2.addYearAsOracle(1);
                    d2.addSec(-1);
                }else if(j ==5)
                {
                    if(sValueMin[4]=='1')
                    {
                        d2.addMonthAsOracle(3);
                        d2.addSec(-1);
                    }
                    else
                    {
                        d2.addMonthAsOracle(9);
                        d2.addSec(-1);
                    }
                }else if(j==6)
                {
                    d2.addMonthAsOracle(1);
                    d2.addSec(-1);
                }else if(j ==7)
                {
                    d3 = d2;
                    d3.setDay(1);
                    d3.addMonthAsOracle(1);
                    d3.addSec(-1);
                    d2.addDay(10);
                    d2.addSec(-1);
                    if(d2>d3)
                        d2 = d3;
                }
                sprintf(sValueMin,"%04d%02d%02d",d1.getYear(),d1.getMonth(),d1.getDay());
                sprintf(sValueMax,"%04d%02d%02d",d2.getYear(),d2.getMonth(),d2.getDay());
            }else
            {
                if(-1 == Date::checkTime(sValueMin))
                    return SECTION_NORM;
                d1.parse(sValueMin,"HHMISS");
                d2.parse(sValueMin,"HHMISS");
                if(j==0)
                {
                    d2.parse("235959","HHMISS");
                }else if(j==1)
                {
                    if(sValueMin[0]=='2')
                        d2.addHour(4);
                    else
                        d2.addHour(10);
                    d2.addSec(-1);
                }else if(j==2)
                {
                    d2.addHour(1);
                    d2.addSec(-1);
                }else if(j==3)
                {
                    d2.addMin(10);
                    d2.addSec(-1);
                }else if(j==4)
                {
                    d2.addMin(1);
                    d2.addSec(-1);
                }else if(j==5)
                {
                    d2.addSec(9);
                }
                sprintf(sValueMin,"%02d%02d%02d",d1.getHour(),d1.getMin(),d1.getSec());
                sprintf(sValueMax,"%02d%02d%02d",d2.getHour(),d2.getMin(),d2.getSec());
            }

            point.setValue2 (atoi(sValueMin), iValueType3, 0, 0);
            vPoint.push_back (point);

            point.setValue2 (atoi(sValueMax), iValueType3, pSection->m_iPricingSectionOffset, 1);
            vPoint.push_back (point);

            iOp = 2;
                }break;
        }
        break;
    default:
        return SECTION_NORM;
        break;
    }
    return SECTION_SPAN;
}

bool ParamInfoMgr::queryBySplitID (int iSplitID,SplitCfg * pCfg)
{
        SplitCfg *m_poSplitCfg = (SplitCfg*)(*m_poSplitData);
        SplitCfg * cfg = 0;
        unsigned int iCfg = 0;
        if(m_poSplitIndex->get(iSplitID,&iCfg)){
                cfg = &m_poSplitCfg[iCfg];
                memcpy(pCfg,cfg,sizeof(SplitCfg));
                return true;
        }
        return false;
}


void ParamInfoMgr::getCombineType(PricingCombine *pCombine)
{
        unsigned int m_iCombineTmp = 0;
        unsigned int m_iMeasureTmp = 0;
    unsigned int m_iSectionTmp = 0;
    
    PricingCombine *m_poCombine = (PricingCombine*)(*m_poPricingCombineData);   
    PricingMeasure *m_poMeasure = (PricingMeasure*)(*m_poPricingMeasureData);   
    PricingSection *m_poSection = (PricingSection*)(*m_poPricingSectionInfoData);
    LogicStatement *m_poLogic = (LogicStatement*)(*m_poLogicStatementData);
                
    AssignStatement *m_poAssign = (AssignStatement*)(*m_poAssignStatementData);  
    Discount *m_poDisct = (Discount*)(*m_poDiscountInfoData);
    FilterCond *m_poFilterCond = (FilterCond*)(*m_poFilterConditionData);
    
        m_iMeasureTmp = pCombine->m_iFirstMeasure;              
    if(!m_poPricingCombineIndex->get( pCombine->m_iCombineID, &m_iCombineTmp))  return;
                        
    PricingSection *pSection;
    LogicStatement *pLogic;
    AssignStatement *pAssign;
    Queue<PricingSection *> queue;
    Queue<Discount *> qdisct;

    Discount *pDisct;
    ID *pID;
    pCombine->m_iCycleType = COMPLEX_CYCLE_ACCT;

    for (; m_iMeasureTmp; m_iMeasureTmp=m_poMeasure[m_iMeasureTmp].m_iNext) 
    {
                unsigned int iMeaFirSecTmp = m_poMeasure[m_iMeasureTmp].m_iFirstSection;
                
        for ( ; iMeaFirSecTmp; iMeaFirSecTmp=m_poSection[iMeaFirSecTmp].m_iBrotherOffset) 
        {
            queue.put (&m_poSection[iMeaFirSecTmp]);
        }
        while (queue.get (pSection)) 
        {
            unsigned int iSecTmp = pSection->m_iPricingSectionOffset; 
            unsigned int iFirstChildTmp = pSection->m_iFirstChildOffset;
            if (iFirstChildTmp > 0) 
            {
                for (; iFirstChildTmp>0; iFirstChildTmp=m_poSection[iFirstChildTmp].m_iBrotherOffset) 
                {
                    queue.put (&m_poSection[iFirstChildTmp]);
                }
                
            } else {
                                if (pSection->m_iDiscountOffset > 0) 
                                {                               
                                qdisct.put (&m_poDisct[m_poSection[iSecTmp].m_iDiscountOffset]);
                                }
            }

            if (pSection->m_iConditionID <= 0) 
                continue;

            unsigned int iCondTmp = 0;
            if (!m_poLogicStatementIndex->get( pSection->m_iConditionID, &iCondTmp)) continue;

            pID = m_poLogic[iCondTmp].m_poParamID1;
            if (pID->m_iAttrID == 500) 
            {
                if (!pID->m_iExt && pID->m_iOwner==2) 
                {
                    pCombine->m_iCycleType = SERV_CYCLE_ACCT;
                    return;
                }
             }
        }

        while (qdisct.get (pDisct)) 
        {
                unsigned int iDisctTmp = 0;
            m_poDiscountIndex->get( pDisct->m_iDiscountID, &iDisctTmp);
            if(iDisctTmp == 0 ) continue;
            
            for (; iDisctTmp; iDisctTmp=m_poDisct[iDisctTmp].m_iNext)
            {
                                unsigned int m_iAssignTmp = 0;
                                
                                if (!m_poAssignStatementIndex->get( pDisct->m_iOperationID, &m_iAssignTmp)) continue;
                                                                
                pID = m_poAssign[m_iAssignTmp].m_poParamID1;
                                                                
                if (pID->m_iAttrID == 500) 
                {
                    if (!pID->m_iExt && pID->m_iOwner==2) 
                    {
                        pCombine->m_iCycleType = SERV_CYCLE_ACCT;
                        return;
                    }
                }

                pID = m_poAssign[m_iAssignTmp].m_poParamID2;
                if (pID->m_iAttrID == 500) 
                {
                    if (!pID->m_iExt && pID->m_iOwner==2) 
                    {
                        pCombine->m_iCycleType = SERV_CYCLE_ACCT;                                  
                        return;
                    }
                }
            }                   
        }
    }
}



void ParamInfoMgr::preCompileB(PricingCombine *pCombine)
{
    unsigned int m_iMeasureTmp = 0;
    unsigned int m_iSectionTmp = 0;
    unsigned int m_iLogicTmp = 0;
    unsigned int m_iAssignTmp = 0;
    unsigned int m_iDisctTmp = 0;
    
    m_iMeasureTmp = pCombine->m_iFirstMeasure;
    
    PricingMeasure *m_poMeasure = (PricingMeasure*)(*m_poPricingMeasureData);
        
    PricingSection *m_poSection = (PricingSection*)(*m_poPricingSectionInfoData);
        
    LogicStatement *m_poLogic = (LogicStatement*)(*m_poLogicStatementData);
                
    AssignStatement *m_poAssign = (AssignStatement*)(*m_poAssignStatementData);
                
        PricingSection *pSection;  
                
    Queue<PricingSection *> queue;
                
    Discount *m_poDisct = (Discount*)(*m_poDiscountInfoData);
                
    ID *pID;
        queue.clear();
        
    for (; m_iMeasureTmp; m_iMeasureTmp = m_poMeasure[m_iMeasureTmp].m_iNext) 
    {           
        m_iSectionTmp = m_poMeasure[m_iMeasureTmp].m_iFirstSection;
        
        for ( ; m_iSectionTmp; m_iSectionTmp = m_poSection[m_iSectionTmp].m_iBrotherOffset) 
        {
            if(m_iSectionTmp<=m_poPricingSectionInfoData->getCount())
            {
                                queue.put (&m_poSection[m_iSectionTmp]);
            }else{
                goto _compileBNext;
            }
        }
        
_compileBNext:
                                
                unsigned int iSectionTmp1 = 0; 
        while (queue.get (pSection)) 
        {
            iSectionTmp1 = pSection->m_iPricingSectionOffset;  
                        unsigned int iSectionTmp2 = pSection->m_iFirstChildOffset;  
                        
            if((iSectionTmp2 > 0)&&(iSectionTmp2 <= m_poPricingSectionInfoData->getCount()))
            {
                for (; iSectionTmp2>0; iSectionTmp2=m_poSection[iSectionTmp2].m_iBrotherOffset) 
                {                  
                    if(iSectionTmp2 <= m_poPricingSectionInfoData->getCount())
                    {
                                                queue.put (&m_poSection[iSectionTmp2]);
                    }
                }
            } else {                            
                                        for (m_iDisctTmp=pSection->m_iDiscountOffset; m_iDisctTmp; m_iDisctTmp=m_poDisct[m_iDisctTmp].m_iNext) 
                                        {                              
                                                                if (!m_poAssignStatementIndex->get(m_poDisct[m_iDisctTmp].m_iOperationID, &m_iAssignTmp)) continue;                                                                                     
                                        if (m_poAssign[m_iAssignTmp].m_iFunctionID == 908) 
                                        {
                                                                        m_poSection[iSectionTmp1].m_iSectionType = SECTION_CONT;                                                                        
                                                (long &)(m_poSection[iSectionTmp1].m_poTariff) = m_poAssign[m_iAssignTmp].m_poParamID1->m_lValue;
                                                                                        
                                        break;
                                } else if (m_poAssign[m_iAssignTmp].m_iFunctionID == 909) 
                                {
                                            if(iSectionTmp1>0)
                                            {
                                                                        m_poSection[iSectionTmp1].m_iSectionType = SECTION_CONT;
                                                                        (long &)(m_poSection[iSectionTmp1].m_poTariff) = m_poAssign[m_iAssignTmp].m_poParamID1->m_lValue;                                                                                                             
                                                                        (long &)(m_poSection[iSectionTmp1].m_poPercent) = (m_poAssign[m_iAssignTmp].m_poParamID3->m_lValue << 32) | m_poAssign[m_iAssignTmp].m_poParamID2->m_lValue;   
                                                                        break;
                                                }
                                } 
                        }                               
            }
        }
    }
}

void ParamInfoMgr::loadTimeCond()
{ 
        //m_iMAXTimeCondNum = 0;   
    char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TIMECONDINFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+47);
        }else
        {
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poTimeCondData->exist()))
    m_poTimeCondData->create(num*m_fRatio);
        
        //<m_iAccuTypeID, TimeCond>
        if(!(m_poTimeCondIndex->exist()))
    m_poTimeCondIndex->create(num*m_fRatio);
    
    TOCIQuery qry(Environment::getDBConn());  
    int count;   
    TimeCond *p;  
    TimeCond *pTemp;   
    char sql[2048];
    p = (TimeCond*)(*m_poTimeCondData);
    checkMem(m_poTimeCondData->getCount() * sizeof(TimeCond));
    strcpy(sql, " select ACCU_TYPE_ID, START_VALUE, END_VALUE "
                " from b_accu_cond_time_period ");

    qry.setSQL(sql);
    qry.open();
    int i(0);
    while (qry.next())
    {
        i = m_poTimeCondData->malloc();
        if(i <= 0)  THROW(MBC_ParamInfoMgr+48);
        memset(&p[i], '\0', sizeof(struct TimeCond));
        p[i].m_iAccuTypeID = qry.field(0).asInteger();
        strcpy(p[i].m_sStartValue, qry.field(1).asString());
        strcpy(p[i].m_sEndValue, qry.field(2).asString());
                p[i].m_iNext = 0;
        unsigned int iTimeCondTmp = 0;
                if (m_poTimeCondIndex->get(p[i].m_iAccuTypeID, &iTimeCondTmp))
        {
                p[i].m_iNext = p[iTimeCondTmp].m_iNext;
                p[iTimeCondTmp].m_iNext = i;
        }else{
                m_poTimeCondIndex->add(p[i].m_iAccuTypeID, i);
        }   
        //m_iMAXTimeCondNum++;
    }
    qry.close();
}


void ParamInfoMgr::loadEventTypeCond()
{   
    char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_EVENTTYPECONDINFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+50);
        }else
        {
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
    if(!(m_poEventTypeCondData->exist()))
    m_poEventTypeCondData->create(num*m_fRatio);
        
        //<AccuTypeID,>
        if(!(m_poEventTypeCondIndex->exist()))
    m_poEventTypeCondIndex->create(num*m_fRatio);
	checkMem(m_poEventTypeCondData->getCount() * sizeof(EventTypeCond));    
    TOCIQuery qry(Environment::getDBConn());
    int count;
    struct EventTypeCond *p;
    struct EventTypeCond *pTemp;
    char sql[2048];
        
    p = (EventTypeCond *)(*m_poEventTypeCondData);
    /*strcpy(sql, "select AGGR_TYPE_ID, EVENT_TYPE_ID "
                "from b_accu_cond_event_type");*/
        strcpy(sql,"select AGGR_TYPE_ID, EVENT_TYPE_ID  from b_accu_cond_event_type where nvl(type_flag,0)= 0 "
                                "union all select distinct AGGR_TYPE_ID, b.event_type_id  from "
                                "b_accu_cond_event_type a,b_event_type_group_member b "
                                "where nvl(a.type_flag,0)= 1 and  a.event_type_id = b.type_group_id ");
    qry.setSQL(sql);
    qry.open();
    int i(0);
    while (qry.next())
    {
        i = m_poEventTypeCondData->malloc();         
        if(i <= 0)  THROW(MBC_ParamInfoMgr+51);       
        p[i].m_iAccuTypeID = qry.field(0).asInteger(); 
        p[i].m_iEventTypeID = qry.field(1).asInteger();  
                p[i].m_iNext = 0;      
        unsigned int iEventTypeCondTmp = 0;
        if (m_poEventTypeCondIndex->get(p[i].m_iAccuTypeID, &iEventTypeCondTmp))
        {
                p[i].m_iNext = p[iEventTypeCondTmp].m_iNext; 
                p[iEventTypeCondTmp].m_iNext = i;              
        } else {
                m_poEventTypeCondIndex->add(p[i].m_iAccuTypeID, i);  
        }    
        //m_iMAXEventTypeCondNum++; 
    }
    qry.close();
}


//
void ParamInfoMgr::loadCycleType()
{
    char countbuf[32]= {0};
        int num = 0;    
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_CYCLETYPEINFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+53);
        }else
        {
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
    if(!(m_poCycleTypeData->exist()))
    m_poCycleTypeData->create(num*m_fRatio);
        
        //<CycleTypeID,>
        if(!(m_poCycleTypeIndex->exist()))
    m_poCycleTypeIndex->create(num*m_fRatio);
    checkMem(m_poCycleTypeData->getCount() * sizeof(CycleType));       
    TOCIQuery qry(Environment::getDBConn());  
    int count;
    char sql[2084];
    char sCycleBeginType[4];
    char sOffsetType[4];
    char sStepType[4];
    CycleType *p;

    p = (CycleType*)(*m_poCycleTypeData);

    strcpy(sql, "select cycle_type_id, cycle_begin_date_type,"
                "cycle_begin_date_offset, offset_unit,"
                "cycle_duration, CYCLE_DURATINON_UNIT "
                "from b_cycle_type");
    qry.setSQL (sql);
    qry.open ();
    int i(0);
    while (qry.next())
    {
        i = m_poCycleTypeData->malloc();       
        if(i <= 0)  THROW(MBC_ParamInfoMgr+54);  
                memset(&p[i],'\0',sizeof(struct CycleType));        
        p[i].m_iCycleTypeID = qry.field(0).asInteger();
        strcpy(sCycleBeginType, qry.field(1).asString());

        switch ( sCycleBeginType[2] )
        {
            case 'A':
                p[i].m_iBeginDateType = 1;
                break;
                
            case 'B':
                p[i].m_iBeginDateType = 2;
                break;

            case 'C':
                p[i].m_iBeginDateType = 3;
                break;

            case 'D':
                p[i].m_iBeginDateType = 4;
                break;

            case 'E':
                p[i].m_iBeginDateType = 5;
                break;

            case 'F':
                p[i].m_iBeginDateType = 6;
                break;

            default:               
                Log::log (0, "Error in LoadCycleType().\n");
                // p[i].m_iBeginDateType = atoi(qry.field(1).asString());
        }
        p[i].m_iDateOffset = qry.field(2).asInteger();
        strcpy(sOffsetType, qry.field(3).asString());
        switch ( sOffsetType[2] )
        {
            case 'H':             
                p[i].m_iOffsetType = 0;
                break;

            case 'D':                
                p[i].m_iOffsetType = 1;
                break;

            case 'M':               
                p[i].m_iOffsetType = 2;
                break;

            default:
                Log::log (0, "Error in LoadCycleType().\n");           
                // p[i].m_iOffsetType = atoi(qry.field(3).asString());
        }    
        p[i].m_iStepLength = qry.field(4).asInteger();
        strcpy(sStepType, qry.field(5).asString());
        switch (sStepType[2])
        {
            case 'H':             
                p[i].m_iStepType = 0;
                break;

            case 'D':               
                p[i].m_iStepType = 1;
                break;
                                
            case 'M':             
                p[i].m_iStepType = 2;
                break;

            default:
                Log::log (0, "Error in LoadCycleType().\n");
                //  p[i].m_iStepType = atoi(qry.field(5).asString());
        }       
        if(!checkCycleType(&p[i]))
        {
            Log::log(0, "[ERROR]ID is [%d] ,it's lifeType is wrong ,the type of begine time id %s ,it's offset can't be %s",p[i].m_iCycleTypeID,sCycleBeginType,sOffsetType);
            THROW(MBC_ParamInfoMgr+55);
        }
        m_poCycleTypeIndex->add (p[i].m_iCycleTypeID, i);
        //m_iMAXCycleTypeNum++;    
    }   
    qry.close ();   
}


bool ParamInfoMgr::checkCycleType(const CycleType * pCycle)
{
    switch(pCycle->m_iBeginDateType)
    {
        case 1:
            if(pCycle->m_iOffsetType==0)
            {
                return true;
            }
            break;
        case 2:
            if(pCycle->m_iOffsetType==1)
            {
                return true;
            }
            break;
        case 3:
        case 4:
            return true;
            break;
        case 5:
        case 6:
            if(pCycle->m_iOffsetType==1||pCycle->m_iOffsetType==2)
            {
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

//
void ParamInfoMgr::loadAccuType()
{
    char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_ACCTTYPEINFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+56);
        }else
        {
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
    if(!(m_poAccuTypeData->exist()))
    m_poAccuTypeData->create(num*m_fRatio);
        
        //<AccuTypeID,>
        if(!(m_poAccuTypeIndex->exist()))
    m_poAccuTypeIndex->create(num*m_fRatio);
    checkMem(m_poAccuTypeData->getCount() * sizeof(AccuType));  
    TOCIQuery qry(Environment::getDBConn());
    AccuType *p;
    char sql[2084];
    int count = 0;
    p = (AccuType*)(*m_poAccuTypeData);
    
    strcpy(sql, "select a.accu_type_id, a.accu_level, " 
                " nvl(a.ACCU_SPLIT_REASON,''), a.ACCU_SPLIT_CYCLE, "
                " a.CYCLE_TYPE_ID, a.CONDITION_ID, "
                " b.cal_target, b.cal_nume, b.cal_deno, b.cal_org_duration "
                " from B_ACCU_TYPE a, b_accu_cal_rule b "
                " where a.cal_rule_id=b.cal_rule_id");   
    qry.setSQL (sql);           
    qry.open ();           
    int i = 0;

    EventTypeCond *EventTypeCondTemp;
    TimeCond *TimeCondTemp;

    while (qry.next())
    {
        i = m_poAccuTypeData->malloc();       
        if(i <= 0)  THROW(MBC_ParamInfoMgr+57); 
                memset(&p[i],'\0',sizeof(struct AccuType));    
        p[i].m_iAccuTypeID = qry.field(0).asInteger();       
        strcpy(p[i].m_sAccuLevel, qry.field(1).asString());
        strcpy(p[i].m_sSplitby, qry.field(2).asString());
        p[i].m_iSplit = qry.field(3).asInteger();
        p[i].m_iCycleTypeID = qry.field(4).asInteger();
        p[i].m_iCondID = qry.field(5).asInteger();
        strcpy (p[i].m_sTarget, qry.field (6).asString ());
        p[i].m_iNume = qry.field (7).asInteger ();
        p[i].m_iDeno = qry.field (8).asInteger ();              
                ID *pID = new(p[i].m_poID) ID(p[i].m_sTarget);
                pID = 0;
        
        p[i].m_iRealDuration = qry.field (9).asInteger ();
        p[i].m_iSplitAggr = 0;      
        m_poAccuTypeIndex->add(p[i].m_iAccuTypeID, i);      

        if (!p[i].m_iDeno) 
        {
            Log::log (0, "the denominator of cumulate rule is 0! "
                 "the type of cumulate : %d", p[i].m_iAccuTypeID);      
            p[i].m_iDeno = 1;
        }       
        //m_iMAXAccuTypeNum++;
    }
    qry.close ();
    strcpy (sql, "select distinct accu_type_id "
                 "from offer_accumulator_relation "
                 "where use_pricing_section_cond=1 ");
    qry.setSQL (sql); qry.open ();  

    while (qry.next ()) 
    {
        unsigned int iTmp = 0;
                if (m_poAccuTypeIndex->get (qry.field(0).asInteger(), &iTmp)) 
        {
            p[iTmp].m_iSplitAggr = 1;
        }
    }
    qry.close ();       
}


void ParamInfoMgr::loadOfferAccuRelation()
{
    char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_OFFERACCYRELATIONINFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+59);
        }else
        {
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
    if(!(m_poOfferAccuRelationData->exist()))
    m_poOfferAccuRelationData->create(num*m_fRatio);
        
        //<OfferID,>
        if(!(m_poOfferAccuRelationIndex->exist()))
    m_poOfferAccuRelationIndex->create(num*m_fRatio);
	checkMem(m_poOfferAccuRelationData->getCount() * sizeof(OfferAccuRelation));  
    TOCIQuery qry(Environment::getDBConn());
    OfferAccuRelation *p;  
    OfferAccuRelation *pTemp;
    int count = 0; 
    char sql[2084];
    p = (OfferAccuRelation*)(*m_poOfferAccuRelationData);

    strcpy(sql, "select offer_id, accu_type_id ,nvl(use_pricing_Section_cond,0) "
                "from offer_accumulator_relation    ");
                //"where use_pricing_Section_cond<>1  ");

    qry.setSQL (sql);           
    qry.open ();
    int i = 0;

    while (qry.next())
    {
        i = m_poOfferAccuRelationData->malloc();      
        if(i <= 0)  THROW(MBC_ParamInfoMgr+60);         
                p[i].m_iOfferID = qry.field(0).asInteger();
        p[i].m_iAccuTypeID = qry.field(1).asInteger();
                p[i].m_iRelationType = qry.field(2).asInteger();//wangs 20100325
                p[i].m_iNext = 0;
        unsigned int iOfferAccuRela = 0;
            if (m_poOfferAccuRelationIndex->get(p[i].m_iOfferID, &iOfferAccuRela))
        {
            p[i].m_iNext = p[iOfferAccuRela].m_iNext;
            p[iOfferAccuRela].m_iNext = i; 

        }else{  
            m_poOfferAccuRelationIndex->add(p[i].m_iOfferID, i);
        }
        
        i++;    
        //m_iMAXOfferAccuRelationNum++;
    }

    qry.close ();            

}


void ParamInfoMgr::loadOfferAccuExist()
{
    char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_OFFERACCTEXISTINFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+62);
        }else
        {
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        
        //<iOfferID+iAccuTypeID,>
        if(!(m_poOfferAccuExistIndex->exist()))
    m_poOfferAccuExistIndex->create(num*m_fRatio,22);
    
        TOCIQuery qry(Environment::getDBConn()); 
    int count = 0; 
        char sVal[22];    
        int iOfferID;   
        int iAccuTypeID;
    
    char sql[256];
    strcpy(sql, "select offer_id, accu_type_id "
                " from offer_accumulator_relation ");
    
    qry.setSQL (sql);           
    qry.open ();   
    while (qry.next())
    {
                iOfferID = qry.field(0).asInteger();
        
                iAccuTypeID = qry.field(1).asInteger();

                sprintf(sVal,"%d_%d",iOfferID,iAccuTypeID);
                
        m_poOfferAccuExistIndex->add(sVal,1);
        
    }
    
    qry.close ();
    
}


void ParamInfoMgr::loadSwitchInfo()
{
    TSwitchInfo *pSwitchInfo;
    char countbuf[32]={0};
        int num=0;
        
        if (!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SWITCHINFO_NUM, countbuf))
    {
       THROW(MBC_ParamInfoMgr+144);
    } 
    else
    {
       num = atoi(countbuf);
    }
        memset(countbuf,'\0',sizeof(countbuf));

        if(!m_poSwitchData->exist())
        {
           m_poSwitchData->create(num*m_fRatio);
        }
        if(!m_poSwitchIndex->exist())
        {
           m_poSwitchIndex->create(num*m_fRatio);
        }
		if(!m_poSwitchOPCIndex->exist())
        {
           m_poSwitchOPCIndex->create(num*m_fRatio,32);
        }
        pSwitchInfo = (TSwitchInfo*)(*m_poSwitchData);
		checkMem(m_poSwitchData->getCount() * sizeof(TSwitchInfo));  
        TOCIQuery qry(Environment::getDBConn());
        char sql[2048];
        strcpy( sql,"select SWITCH_ID,FILE_NAME_RULE,AREA_CODE, "
                           "SOURCE_EVENT_TYPE,SWITCH_TYPE_ID,SWITCH_LONG_TYPE, "
		           		   "nvl(network_id, 0) NETWORK_ID, nvl(switch_opc_code,'#') SWITCH_OPC_CODE "
                           "from B_SWITCH_INFO  order by SWITCH_ID" );
    qry.setSQL(sql);
        qry.open();

        int i=0;
        while (qry.next())
        {
           i=m_poSwitchData->malloc();
           if(i<=0)
                  THROW(MBC_ParamInfoMgr+145);
           memset(&pSwitchInfo[i],'\0',sizeof(TSwitchInfo));
           pSwitchInfo[i].m_iSwitchID = qry.field(0).asInteger();
           strcpy(pSwitchInfo[i].m_sFileNameRule, qry.field(1).asString());
           strcpy(pSwitchInfo[i].m_sAreaCode, qry.field(2).asString());
           pSwitchInfo[i].m_iSourceEventType = qry.field(3).asInteger();
           pSwitchInfo[i].m_iSwitchTypeID = qry.field(4).asInteger();
           pSwitchInfo[i].m_iSwitchLongType = qry.field(5).asInteger();
           pSwitchInfo[i].m_iNetworkID = qry.field(6).asInteger();
           strcpy(pSwitchInfo[i].m_sSwichOpcCode, qry.field(7).asString());
           pSwitchInfo[i].m_iNext = 0;
           if(i>1)
           {
                        if(pSwitchInfo[i-1].m_iSwitchID == pSwitchInfo[i].m_iSwitchID)
                                pSwitchInfo[i-1].m_iNext = i;
                        else
           m_poSwitchIndex->add(pSwitchInfo[i].m_iSwitchID,i);
           } else {
                        m_poSwitchIndex->add(pSwitchInfo[i].m_iSwitchID,i);
        }
		   if (pSwitchInfo[i].m_sSwichOpcCode[0] != '#')
           		m_poSwitchOPCIndex->add(pSwitchInfo[i].m_sSwichOpcCode, i);
        }
        qry.close();
        Log::log(0, "Switch索引上载量/总量 %d/%d ;Switch数据上载量/总量 %d/%d ",m_poSwitchIndex->getCount(),m_poSwitchIndex->getTotal(),m_poSwitchData->getCount(),m_poSwitchData->getTotal());
}


#ifdef EVENT_AGGR_RECYCLE_USE

void ParamInfoMgr::loadSpecialCycleType()
{
   	 	int iMAXCycleTypeIDNum = 0;
    	m_iDays = 0;
		m_iArrayLen = 0;
        char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_CYCLETYPEIDINFO_NUM, countbuf))
        {
                throw(MBC_ParamInfoMgr+64);
        }else
        {
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        
        //<CycleTypeID,1> 
        if(!(m_poCycleTypeIDIndex->exist()))
    		m_poCycleTypeIDIndex->create(num*m_fRatio+2,20);
        if(!(m_poCycleTypeIDData->exist()))
    		m_poCycleTypeIDData->create(num*m_fRatio);
		
		CycleTypeID *pCycleTypeID = (CycleTypeID*)(*m_poCycleTypeIDData);
		checkMem(m_poCycleTypeIDData->getCount() * sizeof(CycleTypeID));  
        TOCIQuery qry(Environment::getDBConn());  
        bool bFlag = false;
        memset(countbuf,'\0',sizeof(countbuf));   
        if(ParamDefineMgr::getParam(EVENT_AGGR_SEGMENT, EVENT_AGGR_CYCLE_TYPE, countbuf))
        {
            bFlag = true;
        }
    
        if(!bFlag) return;  
    int count = 0; 
    char sql[256];
    
    	sprintf(sql, "select cycle_type_id,cycle_begin_date_type from b_cycle_type where cycle_begin_date_type in (%s)",countbuf);        
        qry.setSQL(sql);     
        qry.open();
        unsigned int i = 0;
        while(qry.next())
        {
				char sKey[20] = {0};
				sprintf(sKey,"%d",qry.field(0).asInteger());
                m_poCycleTypeIDIndex->add(sKey,1);
				i = m_poCycleTypeIDData->malloc();
           		if(i<=0)
                  	THROW(MBC_ParamInfoMgr+145);
           		memset(&pCycleTypeID[i],'\0',sizeof(CycleTypeID));
           		pCycleTypeID[i].m_iCycleTypeID = qry.field(0).asInteger();
		   		strcpy(pCycleTypeID[i].m_sCycleBeginDateType,qry.field(1).asString());
                m_iArrayLen ++;
                iMAXCycleTypeIDNum++;
        }
    
        qry.close();
    
        if(iMAXCycleTypeIDNum>0)
        {
                if(ParamDefineMgr::getParam(EVENT_AGGR_SEGMENT, EVENT_AGGR_CYCLE_DAYS, countbuf))
                {
                        m_iDays = atoi(countbuf);

                }else{
                        m_iDays = 3;
                }
        }
		char *pKey = "m_iDays";
		m_poCycleTypeIDIndex->add(pKey,(unsigned int)m_iDays);
		pKey = "m_iArrayLen"; //这个其实可以不记录
		m_poCycleTypeIDIndex->add(pKey,(unsigned int)m_iArrayLen);
		Log::log(0, "Switch数据上载量/总量 %d/%d ",m_poCycleTypeIDData->getCount(),m_poCycleTypeIDData->getTotal());
		Log::log(0, "Switch索引上载量/总量 %d/%d ",m_poCycleTypeIDIndex->getCount(),m_poCycleTypeIDIndex->getTotal());
}
    
#endif


void ParamInfoMgr::loadVirtualCombine()
{
                int i = 0;              
                PricingSection * pSection;
                PricingCombine * pCombine;              
                TOCIQuery qry (Environment::getDBConn ());
                
                char sType[4];          
                char sSqlcode[500];             
                char countbuf[32]= {0};
                int num = 0;    
                if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_COMBINEINFO_NUM, countbuf))
                {
                        THROW(MBC_ParamInfoMgr+76);
                }else{
                                num = atoi(countbuf);
                }
                memset(countbuf,'\0',sizeof(countbuf));
                if(!(m_poVirtualPricingCombineData->exist()))
                m_poVirtualPricingCombineData->create(num*1.3);
            if(!(m_poVirtualStrategyCombineIndex->exist()))
                m_poVirtualStrategyCombineIndex->create(num*1.3);
        		checkMem(m_poVirtualPricingCombineData->getCount() * sizeof(PricingCombine));  
            pCombine = (PricingCombine*)(*m_poVirtualPricingCombineData);
                pSection = (PricingSection*)(*m_poPricingSectionInfoData);
                
                sprintf(sSqlcode," select event_type_id,event_pricing_strategy_id "
                                                 " from event_pricing_strategy  "
                                 " where event_type_id = %d order by event_type_id", SPECIAL_STRATEGY_EVENT_TYPE);
                
                qry.setSQL (sSqlcode);
                
                qry.open ();
                
        while (qry.next ()) 
        {        
                        i = m_poVirtualPricingCombineData->malloc();                    
            if (i <= 0) THROW(MBC_PricingInfoMgr+77); 

            pCombine[i].m_iEventType = qry.field(0).asInteger ();           
                        pCombine[i].m_iCycleType = 0;  // with ni discount                      
                        pCombine[i].m_iCombineType = 0;  // 1: ; 0: simple; 2:base ;
                        m_iCurEventType = pCombine[i].m_iEventType;  // recorde cur eventType
            int iTemp = qry.field(1).asInteger (); 
            unsigned int iSectTmp = 0;
            //<StrategyID,sectionOffset>
            m_poStrategyIndex->get (iTemp, &iSectTmp);  
            if (iSectTmp <= 0) {
                        Log::log (0, "Strategy %d, have no section",iTemp);
                continue;
            }
                                
            /*  section tree  */
            makeTree (&pSection[iSectTmp], &pCombine[i]);
                        
            switch (m_iCurEventType) {  /*      */
                            case REAL_CYCLE_ACCT_EVENT_TYPE:
                                                                                /*      */
                                                                                makeFilter (&pCombine[i]);
                                                                                        break;                                                  
                                case CYCLE_ACCT_EVENT_TYPE:
                                                                                getCombineType (&pCombine[i]);
                                                                                makeFilter (&pCombine[i]);
                                                                                break;                                                  
                                case REAL_ACCT_EVENT_TYPE:                                                      
                                                                                /*      */
                                                                                makeFilter (&pCombine[i]);
                                                                                preCompile (&pCombine[i]);
                                                                                break;                                                  
                                case 0:
                                                                                        break;
                                default:
                                                                                        /*    */
                                                                                preCompile (&pCombine[i]);
                                                                                preCompileB (&pCombine[i]);
                                                                                break;
        }
        m_poVirtualStrategyCombineIndex->add (iTemp, i);
        }
        qry.close ();
}

#ifdef GROUP_CONTROL
void ParamInfoMgr::loadCGCombine()
{
                int i = 0;      
                PricingSection * pSection = (PricingSection*)(*m_poPricingSectionInfoData);
        PricingCombine * pCombine;       
                TOCIQuery qry (Environment::getDBConn ()); 
                char sType[4];
                char sSqlcode[500];
                char countbuf[32] = {0};
                int num = 0;
                if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_CGCOMBINEINFO_NUM, countbuf)){
                        THROW(MBC_ParamInfoMgr+78);
                }else{
                        num = atoi(countbuf);
                }
                if(!(m_poCGPricingCombineData->exist()))
                        m_poCGPricingCombineData->create(num*1.6);
                //<StrategyID,combineOffset>
                if(!(m_poCGStrategyCombineIndex->exist()))
                        m_poCGStrategyCombineIndex->create(num*1.6);
                checkMem(m_poCGPricingCombineData->getCount() * sizeof(PricingCombine));  
                pCombine = (PricingCombine*)(*m_poCGPricingCombineData);
                
                sprintf(sSqlcode," select event_type_id,event_pricing_strategy_id "                             
                                                 " from event_pricing_strategy  "
                                 " where event_type_id = %d order by event_type_id",CG_STRATEGY_EVENT_TYPE);

                qry.setSQL (sSqlcode);
                
                qry.open ();
                
            while (qry.next ()){
            i = m_poCGPricingCombineData->malloc();

            if (i <= 0) THROW(MBC_PricingInfoMgr+79);
                        
                        pCombine[i].m_iEventType = qry.field(0).asInteger ();
                        
                        pCombine[i].m_iCycleType = 0;
                        
                        pCombine[i].m_iCombineType = 0;
                                
                        m_iCurEventType = pCombine[i].m_iEventType;
                                
            int iTemp = qry.field(1).asInteger ();
            
            unsigned int iSecTmp = 0;
            m_poStrategyIndex->get (iTemp, &iSecTmp);
            if (iSecTmp <= 0){
                Log::log (0, "Strategy %d, have no SECTION",iTemp);
                continue;
            }
                    
            /*    */
            makeTree (&pSection[iSecTmp], &pCombine[i]);
                                
            /*      */
            switch (m_iCurEventType){
                        case REAL_CYCLE_ACCT_EVENT_TYPE:   /*      */
                                makeFilter (&pCombine[i]);
                                break;
            
                        case CYCLE_ACCT_EVENT_TYPE:
                                getCombineType (&pCombine[i]);
                                makeFilter (&pCombine[i]);
                                break;

                        case REAL_ACCT_EVENT_TYPE:        /*      */
                                    makeFilter (&pCombine[i]);
                                    preCompile (&pCombine[i]);
                                    break;
                        case 0:
                                                break;
                        default:    /*    */
                                    preCompile (&pCombine[i]);
                                    preCompileB (&pCombine[i]);
                                            break;
            }
                    m_poCGStrategyCombineIndex->add (iTemp, i);
        }
        qry.close ();
}
#endif

//relation
void ParamInfoMgr::loadCombineRelation()
{       
                int i, combineID;
                
                PricingCombine *pCombine;

                CombineRelation * pRelation;
                
                char sRelation[8];

                TOCIQuery qry (Environment::getDBConn ());
                
                char countbuf[32] = {0};
                int num = 0;
                
                if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_COMBINERELATIONINFO_NUM, countbuf))
                {
                        THROW(MBC_ParamInfoMgr+44);
                }else{
                        num = atoi(countbuf);
                }
                if(!(m_poCombineRelationData->exist()))
                        m_poCombineRelationData->create(num*1.6);
            
                //<sectionID,SectionOffset>
                if(!(m_poCombineRelationIndex->exist()))
                        m_poCombineRelationIndex->create(num*1.6);
                checkMem(m_poCombineRelationData->getCount() * sizeof(CombineRelation));  
                pRelation = (CombineRelation*)(*m_poCombineRelationData);
                pCombine = (PricingCombine*)(*m_poPricingCombineData);
                
                //##
                qry.setSQL ("select a_pricing_combine_id, "
                                                  " z_pricing_combine_id, "
                                                  " relation_type "
                                                  " from pricing_combine_relation ");
        
                qry.open ();

                while (qry.next ()){
                        i = m_poCombineRelationData->malloc();
                        if (i <= 0) THROW(MBC_ParamInfoMgr+45);  //
            
                        combineID = qry.field(0).asInteger (); 
                        pRelation[i].m_iCombineIDZ = qry.field(1).asInteger ();                 
                        strcpy (sRelation, qry.field(2).asString ());
                        
                        m_poCombineRelationIndex->add(combineID, i);
                                                
                int iRelation;

                switch (sRelation[2] | 0x20){
                        case 'a':                                               
                        iRelation = COVER_RELATION;
                        break;
                        case 'b':                       
                        iRelation = ASSOC_RELATION;
                        break;
                        case 'c':
                        iRelation = MIN_RELATION;
                        break;
                }
                pRelation[i].m_iRelation = iRelation;
            
                unsigned int iCombineTmp = 0;        
                if(m_poPricingCombineIndex->get (combineID, &iCombineTmp)){
                pRelation[i].m_iNext = pCombine[iCombineTmp].m_iRelationOffset;
                                pCombine[iCombineTmp].m_iRelationOffset = i;
                }else{
                THROW(MBC_ParamInfoMgr+46);
                }
                        
                    //relation another hand             
                    i = 0;
                    i = m_poCombineRelationData->malloc();
                    if (i <= 0) THROW(MBC_ParamInfoMgr+45);
                    pRelation[i].m_iNext = 0;
            pRelation[i].m_iRelation = iRelation;
            
                    combineID = qry.field(1).asInteger ();
                    pRelation[i].m_iCombineIDZ = qry.field(0).asInteger ();
                                
                    m_poCombineRelationIndex->add(combineID, i);
                                
                    iCombineTmp = 0;
            if(m_poPricingCombineIndex->get (combineID, &iCombineTmp)){
                        pRelation[i].m_iNext = pCombine[iCombineTmp].m_iRelationOffset;
                                pCombine[iCombineTmp].m_iRelationOffset = i;
            }else{
                THROW(MBC_ParamInfoMgr+46);
            }
            }
            //m_iMAXCombineRelationNum = i;
            qry.close ();

}


#ifdef WHOLE_DISCOUNT
void ParamInfoMgr::loadWholeDiscount()
{
        int iCount, j;
        int i = 0;
            WholeDiscount *pWholeDiscount;
        DEFINE_QUERY (qry);
    
        char countbuf[32]= {0};
            int num = 0;
            
                if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_WHOLEDISCOUNTINFO_NUM, countbuf)){
                        THROW(MBC_ParamInfoMgr+3);
                }else{
                        num = atoi(countbuf);
                }
                memset(countbuf,'\0',sizeof(countbuf));
                if(!(m_poWholeDiscountData->exist()))
                m_poWholeDiscountData->create(num*1.3);
                checkMem(m_poWholeDiscountData->getCount() * sizeof(WholeDiscount)); 
                pWholeDiscount = (WholeDiscount*)(*m_poWholeDiscountData);
                
        qry.setSQL (" select condition_id, offer_id from b_whole_discount "  
                        " where state like '%A'" );  // 
        qry.open ();

        j = 0;
        while (qry.next ()){
                    i = m_poWholeDiscountData->malloc();
                    if (i <= 0) THROW(MBC_ParamInfoMgr+51); //
                    
            pWholeDiscount[i].m_iConditionID = qry.field (0).asInteger ();
            if (m_poOfferCombineIndex->get (qry.field (1).asInteger (), &(pWholeDiscount[i].m_iCombine))){
                j++;
            }
        }
                //m_iWholeDiscountNum = j;
                //m_iMAXWholeDiscountNum = i;
        qry.close ();
}
#endif


/*
void  ParamInfoMgr::loadGatherTask()
{
        char countbuf[32]= {0};
            int num = 0;
            if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_GATHERDATAINFO_NUM, countbuf))
            {
                    THROW(MBC_ParamInfoMgr+26);
            }else{
                    num = atoi(countbuf);
            }
            memset(countbuf,'\0',sizeof(countbuf));
        
            if(!(m_poGatherTaskData->exist()))
            m_poGatherTaskData->create(num*1.3);
            
            if(!(m_poGatherTaskIndex->exist()))
            m_poGatherTaskIndex->create(num*1.3);
            
            if(!(m_poOrgGatherTaskIndex->exist()))
            m_poOrgGatherTaskIndex->create(num*1.3);
            GatherTaskInfo *m_poGatherTaskInfo = (GatherTaskInfo *)(*m_poGatherTaskData);       
            GatherTaskInfo *pGatherTask = 0;   
               
            TOCIQuery qry (Environment::getDBConn ());
            char sSqlcode[2048] = {0};*/
        /*snprintf
        (
        sSqlcode,
        2048,
        "   select                  a.org_id, a.task_id, "
        "   nvl (a.host_id,-1)                  host_id, "
        "   trim(src_path)                     src_path, "
                "   trim(des_path)                     des_path, "
        "   trim(nvl(path_name_mask,'')) path_name_mask, "
        "   trim(nvl(file_name_mask,'')) file_name_mask, "
        "   nvl(to_number(gather_mode), 0)  gather_mode, "
        "   switch_id, "
        "   nvl( to_number(day_begin_offset), 0) begin,  "
        "   nvl( to_number(day_end_offset), 0)   end,    "
        "   nvl(ip_addr,'')                     ip_addr, "
        "   nvl(user_name,'')                 user_name, "
        "   nvl(password,'')                   password  "
                "   nvl(list_mode,0)                   list_mode "
        "   nvl( to_number(check_interval), 0) interval, "
        "   trim(comments)                     comments, "
        "   trim(task_type)                   task_type, "
        "   nvl( to_number(process_id), 0)   process_id, "
                "   trim(nvl (backup_path,''))      backup_path, "
                "   trim(qc_up_path)                    qc_path, "
                "   nvl(qc_host,-1)                     qc_host, "
                "   nvl(process_id2,0)              process_id2  "
        "   from  b_gather_task a, b_host_info b         "
        "   where a.host_id = b.host_id                  "
        "   order by org_id, task_id, host_id, src_path  "
        );*/
                //query.setSQL (sSqlcode);
                /*qry.setSQL(
                "   select                  a.org_id, a.task_id, "
        "   nvl (a.host_id,-1)                  host_id, "
        "   trim(src_path)                     src_path, "
                "   trim(des_path)                     des_path, "
        "   trim(nvl(path_name_mask,'')) path_name_mask, "
        "   trim(nvl(file_name_mask,'')) file_name_mask, "
        "   nvl(to_number(gather_mode), 0)  gather_mode, "
        "   switch_id, "
        "   nvl( to_number(day_begin_offset), 0) begin,  "
        "   nvl( to_number(day_end_offset), 0)   end,    "
        "   nvl(ip_addr,'')                     ip_addr, "
        "   nvl(user_name,'')                 user_name, "
        "   nvl(password,'')                   password  "
                "   nvl(list_mode,0)                   list_mode "
        "   nvl( to_number(check_interval), 0) interval, "
        "   trim(comments)                     comments, "
        "   trim(task_type)                   task_type, "
        "   nvl( to_number(process_id), 0)   process_id, "
                "   trim(nvl (backup_path,''))      backup_path, "
                "   trim(qc_up_path)                    qc_path, "
                "   nvl(qc_host,-1)                     qc_host, "
                "   nvl(process_id2,0)              process_id2  "
        "   from  b_gather_task a, b_host_info b         "
        "   where a.host_id = b.host_id                  "
        "   order by org_id, task_id, host_id, src_path  "
                );
                qry.open();
                int i=0;
                int iOrgID = 0;
                int iLastOrgID = 0;
                int iLast = 0;
                while (qry.next()) 
                {
                     i = m_poGatherTaskData->malloc();
                     if(i<=0) THROW(100+19);
                     pGatherTask = &m_poGatherTaskInfo[i];
                     iOrgID = qry.field(0).asInteger();
                     pGatherTask->m_iTaskID = qry.field(1).asInteger();
                     pGatherTask->m_iOrgID = iOrgID;
                         strncpy(pGatherTask->m_sSrcPath, qry.field( "src_path" ).asString(), 500 );
                         int j = m_poDirectoryData->malloc();
                         if(j>0){
                        pDirectory = &m_poDirectory[j];
                        newDirectory(pDirectory, qry.field( "src_path" ).asString());
                        pGatherTask->m_iSrcDirectory = j;
             }else{
                    pGatherTask->m_iSrcDirectory = 0;
                    THROW(100); // 
             }
             strncpy(pGatherTask->m_sDescPath, qry.field( "des_path" ).asString(), 50);
             j = 0;
                         j = m_poDirectoryData->malloc();
             if(j>0){
                        pDirectory = &m_poDirectory[j];
                        newDirectory(pDirectory, qry.field( "des_path" ).asString());
                        pGatherTask->m_iDesDirectory = j;
             }else{
                        pGatherTask->m_iDesDirectory = 0;
                        THROW(100); // 
             }
             
             if(pGatherTask->m_sSrcPath[0] != '\0'
                &&pGatherTask->m_sSrcPath[strlen(pGatherTask->m_sSrcPath ) - 1] != '/')
             {
                strcat(pGatherTask->m_sSrcPath, "/" );
             }
             
                         pGatherTask->m_iCheckInterval = qry.field( "interval" ).asInteger();
                         //pGatherTask->m_iTimeCnt = pGatherTask->m_iCheckInterval; //
             pGatherTask->m_iDefaultSwitchID = qry.field( "switch_id" ).asInteger();
                         pGatherTask->m_iSwitchID = qry.field( "switch_id" ).asInteger();
             //sprintf(pGatherTask->m_sTaskName, "%s_%d", m_sSrcPath, m_iSwitchID);
             sprintf (pGatherTask->m_sTaskName, "%s", pGatherTask->m_sSrcPath);
                         
                         strcpy(pGatherTask->m_sFileNameWildcard, qry.field( "file_name_mask" ).asString() );
             
             pGatherTask->m_iHostID = qry.field( "host_id" ).asInteger();
            
             strncpy
             (
                pGatherTask->m_sPathNameWildcard,
                qry.field( "path_name_mask" ).asString(), 50
             );
             
             pGatherTask->m_iBegin = qry.field( "begin" ).asInteger();
             pGatherTask->m_iEnd = qry.field( "end" ).asInteger();
             pGatherTask->m_iGatherMode = qry.field( "gather_mode" ).asInteger();
             
                         //malloc
             pGatherTask->m_sIP[0] = '\0';
             pGatherTask->m_iPasv = 0;
             pGatherTask->m_iPort = 21;
             pGatherTask->m_sUserName[0] = '\0';
             pGatherTask->m_sPassword[0] = '\0';
             if(pGatherTask->m_iHostID){
                strncpy(pGatherTask->m_sIP, qry.field( "ip_addr" ).asString(), 50);
                strncpy(pGatherTask->m_sUserName, qry.field( "user_name" ).asString(), 50);
                strncpy(pGatherTask->m_sPassword, qry.field( "password" ).asString(), 50);
                pGatherTask->m_iListMode = qry.field("list_mode").asInteger();
                         }
             //add           
             strncpy(pGatherTask->m_sComments, qry.field( "comments" ).asString(), 50);
             strncpy(pGatherTask->m_sTaskType, qry.field( "task_type" ).asString(), 10);
             pGatherTask->m_iProcessID = qry.field( "process_id" ).asInteger();
             strncpy(pGatherTask->m_sBackupPath, qry.field( "backup_path" ).asString(), 500 );
                         
                         if(pGatherTask->m_iHostID<0 )
                         pGatherTask->m_iQcSend = 0;
                             
                         pGatherTask->m_iQcHostID = qry.field( "qc_host" ).asInteger();              
             strncpy(pGatherTask->m_sQcOrgPath, qry.field( "qc_path" ).asString(), sizeof(pGatherTask->m_sQcOrgPath) );            
             pGatherTask->m_iProcessID2 = qry.field( "process_id2" ).asInteger();
                         if( pGatherTask->m_iQcHostID <= 0){
                 pGatherTask->m_iQcSend = 0;
                 }
                 if ( pGatherTask->m_sQcOrgPath[0] != '\0' 
                         && pGatherTask->m_sQcOrgPath[strlen( pGatherTask->m_sQcOrgPath ) - 1] != '/' ){
                  strcat( pGatherTask->m_sQcOrgPath, "/" );
                 }
                 if(pGatherTask->m_sQcOrgPath[0]=='\0')
                 strncpy(pGatherTask->m_sQcOrgPath,pGatherTask->m_sOrgSrcPath,sizeof(pGatherTask->m_sQcOrgPath));
                 strncpy(pGatherTask->m_sQcPath, pGatherTask->m_sQcOrgPath, sizeof(pGatherTask->m_sQcPath)); 
                        
                        if (pGatherTask->m_sIP[0] != NULL ){
                char *pTmp = 0;
                if ( ( pTmp = strchr(pGatherTask->m_sIP, '#' ) ) != NULL ){
                    pGatherTask->m_iPasv = 1;
                    *pTmp = '\0';
                }
                if ( ( pTmp = strchr(pGatherTask->m_sIP, ':' ) ) != NULL ){
                    *pTmp = NULL;
                    pGatherTask->m_iPort = atoi( ++pTmp );
                    if (pGatherTask->m_iPort <= 0 ){
                        pGatherTask->m_iPort = 21;
                    }
                }
            }
                        if( strlen( pGatherTask->m_sTaskType ) == 0 ){
                        strcpy( pGatherTask->m_sTaskType, "F" );
                }
            pGatherTask->m_iFileNum = 0;
            pGatherTask->m_sMinCreateDate[0] = '\0' ;
            pGatherTask->m_sMaxCreateDate[0] = '\0' ;  
                        m_poGatherTaskIndex->add(pGatherTask[i].m_iTaskID, i);
                    if(iOrgID == iLastOrgID)
                    {
                             pGatherTask[iLast].m_iNext = i;
                        }else{
                             m_poOrgGatherTaskIndex->add(iOrgID,i);
                        }
                        iLastOrgID = iOrgID;
                        iLast = i;
                 }
}*/

/*
void ParamInfoMgr::newDirectory(Directory *pDirectory, char *srcPath)
{
     int iLen = 0;
     char sPath[500] = {0};
         
         strncpy(sPath, srcPath, 500);
         strcpy (pDirectory->m_sPathName, sPath);
         iLen = strlen (sPath);
                    
         if (iLen > 0) {
                if (sPath[iLen-1] != '/')
                        strcat (pDirectory->m_sPathName, "/");
         }
         pDirectory->m_poFileList = NULL;
         pDirectory->m_pDir = NULL;
         pDirectory->m_iFileNum = 0;
         pDirectory->m_iMaxFileNum = 1000;
         pDirectory->m_tmpCurrGetFileNum = 0;
}*/

//
void  ParamInfoMgr::loadSpecialNbrStrategy()
{
                SpecialNbrStrategy * pTemp;
                SpecialNbrStrategy * pTemp2;
                int i = 0;
        int iAccNbrLen = 0;
        int iMAXSpecNbrStrategyNum = 0;
        int iSpecNbrStratLen = 0;
                
                TOCIQuery qry(Environment::getDBConn());
                
                char countbuf[32]= {0};
                int num = 0;
                
                if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SPECIALNBRSTRATEGYINFO_NUM, countbuf))
                {
                        THROW(MBC_ParamInfoMgr+74);
                }else{
                        num = atoi(countbuf);
                }
                memset(countbuf,'\0',sizeof(countbuf));
                if(!(m_poSpecialNbrStrategyData->exist()))
                m_poSpecialNbrStrategyData->create(num*1.3);
        		checkMem(m_poSpecialNbrStrategyData->getCount() * sizeof(SpecialNbrStrategy)); 
                unsigned int m_iNum1 = num*1.3;
                unsigned int m_iNum2 = MAX_CALLING_NBR_LEN;
                unsigned int m_iNum3 = 3;
                //<acc_nbr, offset>
                if(!(m_poSpecialNbrStrategyIndex->exist()))
                        m_poSpecialNbrStrategyIndex->create(m_iNum1, m_iNum2);
                
                //<offer_id, >
                if(!(m_poSpecialNbrOfferIndex->exist()))
                        m_poSpecialNbrOfferIndex->create(num*1.3);
                pTemp = (SpecialNbrStrategy*)(*m_poSpecialNbrStrategyData);
                
                qry.setSQL("select min(length(acc_nbr)) from b_special_nbr_strategy");
            qry.open();
            if(qry.next())
            {
                    iAccNbrLen = qry.field(0).asInteger();
            }
            
            if(iAccNbrLen<1)  iAccNbrLen =1;
            qry.close();
                
        qry.setSQL(" select nvl(area_code,'*') area_code,acc_nbr,strategy_id,eff_date,"
                                   " nvl(exp_date,to_date('30000501','yyyymmdd')) exp_date , "
                                   " nvl(offer_id,0) offer_id"
                                   " from b_special_nbr_strategy order by acc_nbr,area_code,offer_id ");
        qry.open();
        
                while(qry.next()) {
                        i = m_poSpecialNbrStrategyData->malloc();       
                        if (i <= 0) THROW(MBC_ParamInfoMgr+75); //
                        
                        strcpy(pTemp[i].m_sAreaCode, qry.field(0).asString());
                        strcpy(pTemp[i].m_sAccNbr, qry.field(1).asString());
                        pTemp[i].m_iStrategyID = qry.field(2).asInteger();
                        strcpy(pTemp[i].m_sEffDate, qry.field(3).asString());
                        strcpy(pTemp[i].m_sExpDate, qry.field(4).asString());
                        pTemp[i].m_iOfferID = qry.field(5).asInteger();
                        pTemp[i].m_iNext = 0;
                        unsigned int iTemp2 = 0;
                        
                        if(m_poSpecialNbrStrategyIndex->get(pTemp[i].m_sAccNbr,&iTemp2)){
                                        pTemp[i].m_iNext = iTemp2;
                        }
                        
                        m_poSpecialNbrStrategyIndex->add(pTemp[i].m_sAccNbr, i);
                }
                iMAXSpecNbrStrategyNum = i;
                qry.close();
                qry.commit();
                
                // 
                iSpecNbrStratLen = 0;
                qry.setSQL("select min(length(acc_nbr)) from b_special_nbr_strategy");
                qry.open();
                if(qry.next()){
                         iSpecNbrStratLen = qry.field(0).asInteger();
                }
                
                if(iSpecNbrStratLen<1)  iSpecNbrStratLen =1;
                qry.close();
                qry.commit();
        
                char sSqlcode[500];
                int iCount = 0;
                
                sprintf(sSqlcode," select distinct po.offer_id from pricing_combine a,event_pricing_strategy b ,"
                                                 " product_offer po where a.event_pricing_strategy_id = b.event_pricing_strategy_id "
                                                 " and b.event_type_id = %d and a.pricing_plan_id = po.pricing_plan_id"
                                                   , SPECIAL_STRATEGY_EVENT_TYPE);
                
                qry.setSQL (sSqlcode);
                qry.open();
                while(qry.next()){
                        m_poSpecialNbrOfferIndex->add(qry.field(0).asInteger(), 0);
                        iCount++;
                }
                qry.close();
                qry.commit();
                return ;
}

//
int ParamInfoMgr::setBreakPoint(unsigned int iMeasureTmp, LogicStatement *statement, PricingSection *pSection, vector<ConditionPoint> & vPoint, int &iOp)
{
        SplitCfg *pSplitCfg = (SplitCfg*)(*m_poSplitData);
        Tariff *pTariff = (Tariff*)(*m_poTariffInfoData);
        
        PricingMeasure *m_poPricingMeasure = (PricingMeasure*)(*m_poPricingMeasureData);
        
        PricingSection *m_poPricingSection = (PricingSection*)(*m_poPricingSectionInfoData);
        unsigned int iSectionTmp = 0;
        m_poSectionIndex->get(pSection->m_iSectionID, &iSectionTmp);
        
    if((iMeasureTmp <= 0)||(iSectionTmp <= 0)) return SECTION_WRONG;
    PricingMeasure *pMeasureTemp = &m_poPricingMeasure[iMeasureTmp];
        int iBreakType = -1;
        Value value;
        int iValue2, iValue3, iDesValue;
    int iValueType2, iValueType3, iDesValueType;

    int iRetValue = SECTION_NORM;
    ConditionPoint point;

        int iID = statement->m_poParamID1->m_iAttrID;

        if (!m_poPricingMeasure[iMeasureTmp].m_poID) //m_poPricingMeasure[iMeasureTmp].m_poID = statement->m_poParamID1;
    {
                //pMeasureTemp->m_poID->m_iAttrID = statement->m_poParamID1->m_iAttrID;
        //pMeasureTemp->m_poID->m_iOwner = statement->m_poParamID1->m_iOwner;
        //pMeasureTemp->m_poID->m_iExt = statement->m_poParamID1->m_iExt;
        //pMeasureTemp->m_poID->m_lValue = statement->m_poParamID1->m_lValue;
        //strcpy(pMeasureTemp->m_poID->m_sValue, statement->m_poParamID1->m_sValue);
        memcpy(m_poPricingMeasure[iMeasureTmp].m_poID,statement->m_poParamID1,sizeof(class ID));
    }
    /* ?Yê±2??????à?ó·???μ??é?? */
    //pSection->m_poNewBreakID = statement->m_poParamID1;
        //ID *m_poID = new (m_poPricingSection[iSectionTmp].m_poNewBreakID) ID();
        //m_poID = 0;
    PricingSection *p1 = &m_poPricingSection[iSectionTmp];
    //--xgs--p1->m_poNewBreakID->m_iAttrID = statement->m_poParamID1->m_iAttrID;
    //--xgs--p1->m_poNewBreakID->m_iOwner = statement->m_poParamID1->m_iOwner;
    //--xgs--p1->m_poNewBreakID->m_iExt = statement->m_poParamID1->m_iExt;
    //--xgs--p1->m_poNewBreakID->m_lValue = statement->m_poParamID1->m_lValue;
    //--xgs--strcpy(p1->m_poNewBreakID->m_sValue, statement->m_poParamID1->m_sValue);
    memcpy(p1->m_poNewBreakID,statement->m_poParamID1,sizeof(class ID));
        //m_poPricingSection[iSectionTmp].m_poNewBreakID = statement->m_poParamID1;

        if (!iID) return SECTION_NORM;

                if (iID == 64) 
                {
                        iBreakType = BREAK_SPAN;
            iRetValue  = SECTION_SPAN;
                } else if (iID == 74) {
                                                                                        if (m_poPricingMeasure[iMeasureTmp].m_iMeasureMethod == 1) 
                                                                                        {
                                                                                        iBreakType = BREAK_TIME;
                                                                                        iRetValue  = SECTION_TIME;
                                                                                        } else {
                                                                                        return SECTION_NORM;
                                                                                        }

            } else if (iID == 75) {
                                                                                        if (m_poPricingMeasure[iMeasureTmp].m_iMeasureMethod == 1) 
                                                                                        {
                                                                                            iBreakType = BREAK_DATE;
                                                                                            iRetValue  = SECTION_DATE;
                                                                                        } else {
                                                                                            return SECTION_NORM;
                                                                                        }

                } else if (iID == 76) {
                                                                                        if (m_poPricingMeasure[iMeasureTmp].m_iMeasureMethod == 1) 
                                                                                        {
                                                                                                iBreakType = BREAK_WEEK;
                                                                                                iRetValue  = SECTION_WEEK;
                                                                                        } else {
                                                                                                return SECTION_NORM;
                                                                                        }

                } else if (iID == 500) { /* 本帐期的累积费用 */
                                                    iBreakType = BREAK_AGGR1;
                                                    //m_poID = statement->m_poParamID1;
                                                    //m_poPricingMeasure[iMeasureTmp].m_poID = statement->m_poParamID1;
                                pMeasureTemp->m_poID->m_iAttrID = statement->m_poParamID1->m_iAttrID;
                                pMeasureTemp->m_poID->m_iOwner = statement->m_poParamID1->m_iOwner;
                                pMeasureTemp->m_poID->m_iExt = statement->m_poParamID1->m_iExt;
                                pMeasureTemp->m_poID->m_lValue = statement->m_poParamID1->m_lValue;
                                strcpy(pMeasureTemp->m_poID->m_sValue, statement->m_poParamID1->m_sValue);
                                                        //memcpy(m_poPricingMeasure[iMeasureTmp].m_poID,statement->m_poParamID1,sizeof(ID));
                                                                iRetValue  = SECTION_AGGR1;

                                 /*  判断一下是否有用    */
                                //Tariff *pTariff = pSection->m_poTariff;
                                unsigned int iTariffTmp = pSection->m_iTariffOffset;
                                //pTariff[iTariffTmp]
        

        if (!getBelongGroupB (pTariff[iTariffTmp].m_iAcctItemType, m_poPricingMeasure[iMeasureTmp].m_poID->m_lValue)) 
        {
            iRetValue = SECTION_NORM;
        }

                } else if (iID == 5001) {   /* 本周期的累积量   */
        iRetValue = SECTION_AGGR;
        
        AccuTypeCfg accuCfg;
        
        if (!accuCfg.IsAccuTypeEventTypeRelated (m_iCurEventType, statement->m_poParamID1->m_lValue, m_poEventTypeCondIndex, (EventTypeCond*)(*m_poEventTypeCondData)) ) 
        {
             return SECTION_NORM;
        }
        
        AccuType *p1 = accuCfg.getAccuType (statement->m_poParamID1->m_lValue, m_poAccuTypeIndex, (AccuType*)(*m_poAccuTypeData), (TimeCond*)(*m_poTimeCondData), (EventTypeCond*)(*m_poEventTypeCondData));
        if (!p1 || !(p1->m_iSplitAggr)) 
        {
            return SECTION_NORM;
        }
        
        ID *p = p1->m_poID;
        
        if (!p) 
        {
            return SECTION_NORM;
        }
                                
        if (iRetValue != SECTION_NORM) 
        {
             /*  事件的费用  */
            if (p->m_iAttrID == 80) 
            {
                int iItemGroupID = p->m_lValue;
                iRetValue = SECTION_AGGR2;

            } else {
                                                if (pMeasureTemp->m_iMeasureMethod != p->m_iAttrID) 
                                {
                                return SECTION_NORM;
                                }
            }
        }//........

        }else {
                                return SECTION_NORM;
                }
    
    
    if (iRetValue == SECTION_NORM) return SECTION_NORM;
    
    /*
        这里的值要么是常数，要么是商品实例的属性
        商品的属性用 5002_xx_属性标识
        支持分段参数是属性
    */
    
    iValue2 = (int)(statement->m_poParamID2->m_lValue);
    iValueType2 = statement->m_poParamID2->m_iAttrID;
        
    iValue3 = (int)(statement->m_poParamID3->m_lValue);
    iValueType3 = statement->m_poParamID3->m_iAttrID;

    iDesValue = (int)(statement->m_poDesParamID->m_lValue);
    iDesValueType = statement->m_poDesParamID->m_iAttrID;
        unsigned int iSplitCfgTmp = 0;  
    switch (statement->m_iFunctionID) 
    {
      case FUNC_BETWEEN:
        point.setValue2 (iValue2, iValueType2, 0, 0);
        vPoint.push_back (point);

        point.setValue2 (iValue3, iValueType3, pSection->m_iPricingSectionOffset, 1);
        vPoint.push_back (point);

        iOp = 2;
        break;
        
      
          case FUNC_SPLIT_DECIDE:
                                 
                                 if(m_poSplitIndex->get(statement->m_poParamID2->m_lValue,&iSplitCfgTmp)){
                                        point.setValue(atol(pSplitCfg[iSplitCfgTmp].m_sStartVal), 0, 0, 0);
                                        vPoint.push_back (point);
                    
                                        point.setValue2(atol(pSplitCfg[iSplitCfgTmp].m_sEndVal), 0, pSection->m_iPricingSectionOffset, 1);
                                        vPoint.push_back (point);                       
                                }else{
                                        iValue2 = 0;
                                        iValueType2 = 0;
                                        iValue3 = 0;
                                        iValueType3 = 0;
                                }
                        
                        iOp = 2;
                        break;
          
      
      case FUNC_NOTBETWEEN:
            point.setValue2 (iValue2, iValueType2, pSection->m_iPricingSectionOffset, 0);
            vPoint.push_back (point);
            
            point.setValue2 (iValue3, iValueType3, 0, 1);
            vPoint.push_back (point);
            
            point.setValue2 (MAX_VALUE, 0, pSection->m_iPricingSectionOffset, 0);
            vPoint.push_back (point);

            iOp = 3;
            
            break;

      case FUNC_MOD:
            if (iRetValue == SECTION_SPAN) {
                iRetValue = SECTION_MSPAN;
            }else if (iRetValue == SECTION_AGGR) {
                iRetValue = SECTION_MAGGR;
            }
        
            if (!iValue2  || iValueType2) {
                m_poPricingSection[iSectionTmp].m_iBreakValue = 999999999;
                } else {
                m_poPricingSection[iSectionTmp].m_iBreakValue = iValue2;
            }

            /*  这里没有break， 不是bug!    */

      case FUNC_NONE:
        switch (statement->m_iOperatorID) {
          case OP_GT: // >
            point.setValue2 (iDesValue, iDesValueType, 0, 1);
            vPoint.push_back (point);

            point.setValue2 (MAX_VALUE, 0, pSection->m_iPricingSectionOffset, 0);
            vPoint.push_back (point);
    
            iOp = 3;

            break;

          case OP_LT: // <
            point.setValue2 (iDesValue, iDesValueType, pSection->m_iPricingSectionOffset, 0);
            vPoint.push_back (point);

            iOp = 1;
            break;

          case OP_EQ: // =
            point.setValue2 (iDesValue, iDesValueType, 0, 0);
            vPoint.push_back (point);

            point.setValue2 (iDesValue, iDesValueType, pSection->m_iPricingSectionOffset, 1);
            vPoint.push_back (point);

            iOp = 4;

            break;

          case OP_GE: // >=
            point.setValue2 (iDesValue, iDesValueType, 0, 0);
            vPoint.push_back (point);

            point.setValue2 (MAX_VALUE, 0, pSection->m_iPricingSectionOffset, 0);
            vPoint.push_back (point);

            iOp = 3;
            break;

          case OP_LE: // <=
                        point.setValue2 (iDesValue, iDesValueType, pSection->m_iPricingSectionOffset, 1);
            vPoint.push_back (point);

            iOp = 1;
            break;

          case OP_NT: // <>
            point.setValue2 (iDesValue, iDesValueType, pSection->m_iPricingSectionOffset, 0);
            vPoint.push_back (point);
            point.setValue2 (iDesValue, iDesValueType, 0, 1);
            vPoint.push_back (point);
            
            point.setValue2 (MAX_VALUE, 0, pSection->m_iPricingSectionOffset, 0);
            vPoint.push_back (point);

            iOp = 4;
            break;
            case OP_LIKE: //LIKE
            {
                char m_sValue[32];
                strcpy(m_sValue,statement->m_poDesParamID->m_sValue);
                int nLen = strlen(m_sValue);
                char sValueMax[9]={0};
                char sValueMin[9]={0};
                int iValueLen = 0;
                if( 74 ==iID)   
                    iValueLen = 6;
                else
                {
                    if( 75 == iID)
                        iValueLen = 8;     
                    else
                    {
                        //Like函数字符串不合法
                        Log::log(0,"Like strings are out of rule Condition_ID:%d",100);
                        return SECTION_NORM ;
                    }
                }
                if((iValueLen==nLen)&&('%'!=m_sValue[nLen-1]))
                {
                    //位数满足不应该有'%'
                    if(74==iID)
                    {
                        if(-1==Date::checkTime(m_sValue)) //检查时间是否正确
                            return SECTION_NORM;
                    }else
                    {
                        if(-1 ==Date::checkDate(m_sValue))  //检查日期是否正确
                            return SECTION_NORM ;
                    }
                    //没有使用通配符like 函数等同于=
                    point.setValue2 (iDesValue, iDesValueType, 0, 0);
                    vPoint.push_back (point);
                   
                    point.setValue2 (iDesValue, iDesValueType, pSection->m_iPricingSectionOffset, 1);
                    vPoint.push_back (point);
                    iOp = 4;
                    break;
                }
                int j=0;
                if((nLen>iValueLen)||     
                    ((nLen<iValueLen)&&('%'!=m_sValue[nLen-1])))
                {
                    //字符串大于8位或者字符串小于8位且不为%结尾
                    //Like函数字符串不合法
                    Log::log(0,"Like strings are out of rule Condition_ID:%d",100);
                    return SECTION_NORM;
                }else
                {
                    //计算时间和日期格式的下限值  如2008% --->20080101
                    for(int i =0;i<iValueLen;i++)
                    {
                        if('%'==m_sValue[i])
                        {
                            j=i;
                            strncpy(sValueMin,m_sValue,i);
                            sValueMin[i]=0;
                            if(75 == iID)
                            {
                                if((i>0)&&(m_sValue[i-1]=='0'))
                                    j=j+iValueLen;
                                strcat(sValueMin,Date::m_sDateMin[j]);
                            }else
                            {
                                strncat(sValueMin,"000000",iValueLen-i);
                            }
                            j = j%iValueLen;
                            break;
                        }
                    }
                }
                strcpy(sValueMax,sValueMin);

                Date d1,d2,d3;
                if(  75 ==iID)
                {
                    //求取日期的上限值 
                    if(-1 == Date::checkDate(sValueMin))
                        return SECTION_NORM;
                    d1.parse(sValueMin,"YYYYMMDD");
                    d2.parse(sValueMin,"YYYYMMDD");
                    if(j<3)
                        d2.parse("20491231","YYYYMMDD");
                    else if(j==3)
                    {
                        d2.addYearAsOracle(9);
                        d2.addSec(-1);
                    }else if(j==4)
                    {
                        d2.addYearAsOracle(1);
                        d2.addSec(-1);
                    }else if(j ==5)
                    {
                        if(sValueMin[4]=='1')
                        {
                            d2.addMonthAsOracle(3);
                            d2.addSec(-1);
                        }
                        else
                        {
                            d2.addMonthAsOracle(9);
                            d2.addSec(-1);
                        }
                    }else if(j==6)
                    {
                        d2.addMonthAsOracle(1);
                        d2.addSec(-1);
                    }else if(j ==7)
                    {
                        d3 = d2;
                        d3.setDay(1);
                        d3.addMonthAsOracle(1);
                        d3.addSec(-1);

                        d2.addDay(10);
                        d2.addSec(-1);
                        if(d2>d3)
                            d2 = d3;
                    }
                    sprintf(sValueMin,"%04d%02d%02d",d1.getYear(),d1.getMonth(),d1.getDay());
                    sprintf(sValueMax,"%04d%02d%02d",d2.getYear(),d2.getMonth(),d2.getDay());
                }else
                {
                    //求取时间的上限值
                    if(-1 == Date::checkTime(sValueMin))
                        return SECTION_NORM;
                    d1.parse(sValueMin,"HHMISS");
                    d2.parse(sValueMin,"HHMISS");
                    if(j==0)
                    {
                        d2.parse("235959","HHMISS");
                    }else if(j==1)
                    {
                        if(sValueMin[0]=='2')
                            d2.addHour(4);
                        else
                            d2.addHour(10);
                        d2.addSec(-1);
                    }else if(j==2)
                    {
                        d2.addHour(1);
                        d2.addSec(-1);
                    }else if(j==3)
                    {
                        d2.addMin(10);
                        d2.addSec(-1);
                    }else if(j==4)
                    {
                        d2.addMin(1);
                        d2.addSec(-1);
                    }else if(j==5)
                    {
                        d2.addSec(9);
                    }
                    sprintf(sValueMin,"%02d%02d%02d",d1.getHour(),d1.getMin(),d1.getSec());
                    sprintf(sValueMax,"%02d%02d%02d",d2.getHour(),d2.getMin(),d2.getSec());
                }

                point.setValue2 (atoi(sValueMin), iValueType3, 0, 0);
                vPoint.push_back (point);

                point.setValue2 (atoi(sValueMax), iValueType3, pSection->m_iPricingSectionOffset, 1);
                vPoint.push_back (point);

                iOp = 2;
            }break;
        }
        break;

      default:
        return SECTION_NORM;
        break;
    }

    return iRetValue;
}

//设置度量参数
void ParamInfoMgr::setMeasureOffset(unsigned int iMeasure,int iMethod)
{
        #define GET_OFFSET(X, Y) \
                (size_t)(&(((X *)0)->Y))
        PricingMeasure *m_poPricingMeasure = (PricingMeasure*)(*m_poPricingMeasureData);
        m_poPricingMeasure[iMeasure].m_iMeasureMethod = iMethod;
        switch (iMethod) 
        {
          case 1:
                m_poPricingMeasure[iMeasure].m_lOffset = offsetof(StdEvent, m_lDuration);
                break;
          case 2:
                m_poPricingMeasure[iMeasure].m_lOffset = GET_OFFSET(StdEvent, m_lSumAmount);
                break;
          case 3:
                m_poPricingMeasure[iMeasure].m_lOffset = GET_OFFSET(StdEvent, m_lRecvAmount);
                break;
          case 4:
        m_poPricingMeasure[iMeasure].m_lOffset = GET_OFFSET(StdEvent, m_lSendAmount);
                break;
		  case 5:
				m_poPricingMeasure[iMeasure].m_lOffset = GET_OFFSET(StdEvent, m_lSumAmountV4);
				break;
		  case 6:
				m_poPricingMeasure[iMeasure].m_lOffset = GET_OFFSET(StdEvent, m_lSumAmountV6);
                break;
          case 7: 
                m_poPricingMeasure[iMeasure].m_lOffset = GET_OFFSET(StdEvent, m_lDuration);
                break;
          case 8:
                m_poPricingMeasure[iMeasure].m_lOffset = GET_OFFSET(StdEvent, m_lDuration);
                break;
          case 9: 
                m_poPricingMeasure[iMeasure].m_lOffset = GET_OFFSET(StdEvent, m_lDuration);
                break;
          case 50:
                m_poPricingMeasure[iMeasure].m_lOffset = (unsigned long)(&(((StdEvent *)0)->m_lOrgCharge[0]));
                break;
          case 51:
                m_poPricingMeasure[iMeasure].m_lOffset = (unsigned long)(&(((StdEvent *)0)->m_lOrgCharge[1]));
                break;
      case 63:
                m_poPricingMeasure[iMeasure].m_lOffset = GET_OFFSET(StdEvent, m_lDuration);
        break;
          case 71:
                m_poPricingMeasure[iMeasure].m_lOffset = (unsigned long)(&(((StdEvent *)0)->m_oChargeInfo[0].m_lCharge));
                break;
          case 72:
                m_poPricingMeasure[iMeasure].m_lOffset = (unsigned long)(&(((StdEvent *)0)->m_oChargeInfo[1].m_lCharge));
                break;
          case 73:
                m_poPricingMeasure[iMeasure].m_lOffset = (unsigned long)(&(((StdEvent *)0)->m_oChargeInfo[2].m_lCharge));
                break;
          case 74:
                m_poPricingMeasure[iMeasure].m_lOffset = (unsigned long)(&(((StdEvent *)0)->m_oChargeInfo[3].m_lCharge));
                break;
          case 75:
                m_poPricingMeasure[iMeasure].m_lOffset = (unsigned long)(&(((StdEvent *)0)->m_oChargeInfo[4].m_lCharge));
                break;
          case 76:
                break;
          default:
        Log::log (0, "unknown Measure %d", iMethod);
                break;
        }
}


// 排序，过滤最大值...
void ParamInfoMgr::analysePoint (vector< vector<ConditionPoint> > &vPoint,vector<int> &vOp, 
                                                                           PricingSection *pSectionTemp, unsigned int *pNum, PricingSection **ppElse, int iType, unsigned int iSection)
                                     //ConditionPoint **ppPoint, unsigned int *pNum, PricingSection **ppElse, int iType, unsigned int iSection)
{
    if(iSection == 0) return; 
        ConditionPoint *pPoint = 0;
    //ConditionPoint *ppPoint = &pPoint; 
        vector<ConditionPoint> vTemp;
    vector<ConditionPoint> vTemp1;
    vector<ConditionPoint>::iterator iter;
    vector<ConditionPoint>::iterator iter2;
    vector< vector<ConditionPoint> >::iterator iterW;
    vector<int>::iterator iterI;
    vector< vector<ConditionPoint> > oTemp1, oTemp2;
        
    ConditionPoint *pConditionPoint = (ConditionPoint*)(*m_poConditionPointData);
    PricingSection *m_poPricingSection = (PricingSection*)(*m_poPricingSectionInfoData);
        
    int iConst(0), iVar(0);
        
    /*  
        首先对整体排序,
        因为现在需要支持参数的原因，根本无法获知准确的大小，
        特别在[]和==的干扰下
        对常数，可以再排序，对参数，就不排序了;
    */
    for (iterW=vPoint.begin(), iterI=vOp.begin(); iterW!=vPoint.end (); iterW++, iterI++) 
    {
        switch (*iterI)   // *iterI = int
        {
          case 1: //<
          case 2: //[]
          case 4: //==
                    oTemp1.push_back (*iterW); // == ,压栈的是vector< vector<ConditionPoint> >
                    break;
                    
          case 3: //>                    
                    oTemp2.push_back (*iterW); // > , 压栈的是vector< vector<ConditionPoint> >
                    break;
        }
    }
    
    // ==  
    for (iterW=oTemp1.begin(); iterW!=oTemp1.end(); iterW++) 
    {
          // 这里vTemp是些临时变量, vTemp值受*iterW影响, *iterW是参数vPoint
        insertPoint (vTemp, *iterW); //  == , oTemp1 
    }
        
    if (oTemp2.end () != oTemp2.begin ()) // > 
    {
        for (iterW=oTemp2.end()-1; ; iterW--) 
        {
            insertPoint (vTemp, *iterW); //  > , oTemp2         
            if (iterW == oTemp2.begin ())  break;
        }
    }
    
    
    /*  过滤MAX_VALUE   */
    unsigned int iSectionTmp3 = 0;
    for (iter=vTemp.begin (); iter!=vTemp.end (); iter++)  // vector<ConditionPoint> vTemp;
    {
        if ((*iter).m_lValue == MAX_VALUE) 
        {
            
            iSectionTmp3 = (*iter).m_iSection;
        } else {
                vTemp1.push_back (*iter);     // == , vector<ConditionPoint> vTemp1;
                if (!(*iter).m_iValueType) 
                {
                    iConst++;
                } else {
                    iVar++;
                }
        }
    }
    
    
    //if (*ppElse)
    if(m_poPricingSection[iSection].m_iElseSection > 0)
    {
        for (iter=vTemp1.begin (); iter!=vTemp1.end (); iter++) 
        {
 
            if ((*iter).m_iSection == 0)
            {
                (*iter).m_iSection = (*ppElse)->m_iPricingSectionOffset;
            }
        }
    }
    
    if(iSectionTmp3 > 0)
    {
        //*ppElse = pTemp;
        m_poPricingSection[iSection].m_iElseSection = iSectionTmp3;
    }
        
    
    if (!(iConst+iVar)) 
    {
        *pNum = 0;
        return;
    }
    
    *pNum       = iConst+iVar;
    //*ppPoint    = new ConditionPoint[*pNum]; 
    pPoint    = new ConditionPoint[*pNum]; 
    ConditionPoint **ppPoint = &pPoint;     
        
    int i = 0;
    unsigned int iConditionPTmp = 0;
    unsigned int iTmpConP = 0;  //记录初始化的首个值
    for (iter=vTemp1.begin (); iter!=vTemp1.end (); iter++, i++) 
    {
        (*ppPoint)[i] = *iter;
         
        if (!((*ppPoint)[i].m_iValueType)) 
        {
            (*ppPoint)[i].m_lValue      += (*ppPoint)[i].m_iAddValue;
            (*ppPoint)[i].m_iAddValue   =  0;  
        }
            //上面处理结束后，再由私有内存记录到共享
            iConditionPTmp = m_poConditionPointData->malloc();
            
            if(i == 0) {
                                iTmpConP = iConditionPTmp;  //记录初始化的首个下标值
                                pSectionTemp->m_iConditionPointOffset = iTmpConP;
                        }
            if(iConditionPTmp <= 0)  THROW(MBC_ParamInfoMgr+30);
            memset(&pConditionPoint[iConditionPTmp], '\0', sizeof(ConditionPoint));
            pConditionPoint[iConditionPTmp].m_lValue = (*ppPoint)[i].m_lValue;
            pConditionPoint[iConditionPTmp].m_iAddValue = (*ppPoint)[i].m_iAddValue;
            pConditionPoint[iConditionPTmp].m_iValueType = (*ppPoint)[i].m_iValueType;
            pConditionPoint[iConditionPTmp].m_iSection = (*ppPoint)[i].m_iSection;

    }

    //  常数排序
    if (!iVar) 
    {
        int l, k;
        unsigned int kk;
        ConditionPoint *pTemp = *ppPoint;
        ConditionPoint oTemp;
        
         for (l=0; l<i-1; l++)   
        {
            for (k=l+1; k<i; k++) 
            {
                if (pTemp[l].m_lValue > pTemp[k].m_lValue)  
                {               
                                        oTemp           = pTemp[l];
                    pTemp[l]    = pTemp[k];
                    pTemp[k]    = oTemp;
                    //改变值
                    exChangeValue(iTmpConP+l, iTmpConP+k);
                    
                }
            }
        }
    }
        delete []pPoint;
    pPoint = 0;
    return;
}      


bool ParamInfoMgr::UsageTypeNeedCal(int nUsageTypeID)
{
         UsageTypeMgr::stUsageType *m_poUsageType = (UsageTypeMgr::stUsageType*)(*m_poUsageTypeInfoData);
         unsigned int iUsageType = 0;
     if(!m_poUsageTypeIndex->get(nUsageTypeID,&iUsageType))
         return false;
     return m_poUsageType[iUsageType].bNeedCal>0 ?true:false;
}

void ParamInfoMgr::exChangeValue(unsigned int m_iFirst, unsigned int m_iSecond)
{
        if(m_iFirst>0 && m_iSecond>0)
        {
                 ConditionPoint *pTemp = new ConditionPoint;
                 ConditionPoint *pConditionPoint = (ConditionPoint*)(*m_poConditionPointData);
                 
                 pTemp->m_lValue = pConditionPoint[m_iSecond].m_lValue;
                 pTemp->m_iValueType = pConditionPoint[m_iSecond].m_iValueType;
                 pTemp->m_iSection = pConditionPoint[m_iSecond].m_iSection;
                 pTemp->m_iAddValue = pConditionPoint[m_iSecond].m_iAddValue;
                 //pTemp->m_iConditionPoint = pConditionPoint[m_iSecond].m_iConditionPoint;
                 
                 pConditionPoint[m_iSecond].m_lValue = pConditionPoint[m_iFirst].m_lValue;
                 pConditionPoint[m_iSecond].m_iValueType = pConditionPoint[m_iFirst].m_iValueType;
                 pConditionPoint[m_iSecond].m_iSection = pConditionPoint[m_iFirst].m_iSection;
                 pConditionPoint[m_iSecond].m_iAddValue = pConditionPoint[m_iFirst].m_iAddValue;
                 //pConditionPoint[m_iSecond].m_iConditionPoint = pConditionPoint[m_iFirst].m_iConditionPoint;
                 
                 pConditionPoint[m_iFirst].m_lValue = pTemp->m_lValue;
                 pConditionPoint[m_iFirst].m_iValueType = pTemp->m_iValueType;
                 pConditionPoint[m_iFirst].m_iSection = pTemp->m_iSection;
                 pConditionPoint[m_iFirst].m_iAddValue = pTemp->m_iAddValue;
                 //pConditionPoint[m_iFirst].m_iConditionPoint = pTemp->m_iConditionPoint;
                 
                 delete pTemp;
                 pTemp = 0;
         }
}


void ParamInfoMgr::insertPoint(vector<ConditionPoint> &vDes, vector<ConditionPoint> &vSrc)
{
    vector<ConditionPoint>::iterator isrc, ides;
                
    for (isrc=vSrc.begin(); isrc!=vSrc.end(); isrc++) 
    {
        if ((*isrc).m_lValue != MAX_VALUE) 
        {
            for (ides=vDes.begin(); ides!=vDes.end(); ides++) 
            {
                if ((*ides).equal (*isrc)) 
                {
                    if ( ( (*ides).m_iSection) <= 0) 
                    {
                        (*ides).m_iSection = (*isrc).m_iSection;
                    }
                    break;
                }
            }                    
                                                
            if (ides==vDes.end ()) 
            {
                if (vDes.begin () != ides) 
                {
                    ides--;
                    if ((*ides).m_lValue==MAX_VALUE) 
                    {
                        (*isrc).m_iSection = (*ides).m_iSection;
                    }
                }
                vDes.push_back (*isrc);
            }
        } else {
            vDes.push_back (*isrc);
        }
    }
    
}

void ParamInfoMgr::loadSplit()
{
                SplitCfg * pSplitCfgTemp;
                char countbuf[32] = {0};
                int num = 0;
                
                if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SPLITCFGINFO_NUM, countbuf)){
                        THROW(MBC_ParamInfoMgr+72);
                }else{
                        num = atoi(countbuf);
                }
                if(!(m_poSplitData->exist()))   
                m_poSplitData->create(num*m_fRatio);
                if(!(m_poSplitIndex->exist()))
        m_poSplitIndex->create(num*m_fRatio);
        		checkMem(m_poSplitData->getCount() * sizeof(SplitCfg));
        pSplitCfgTemp = (SplitCfg*)(*m_poSplitData);
        
        TOCIQuery qry (Environment::getDBConn ());
        qry.setSQL(" select split_id,split_type,start_val,end_val from cfg_split_attr ");
        qry.open();
        int i = 0;
                while(qry.next()){
                        i = m_poSplitData->malloc();
                        if (i <= 0) THROW(MBC_ParamInfoMgr+73); 
                        pSplitCfgTemp[i].m_iSplitType = qry.field(1).asInteger();
                        strcpy(pSplitCfgTemp[i].m_sStartVal, qry.field(2).asString());
                        strcpy(pSplitCfgTemp[i].m_sEndVal, qry.field(3).asString());
                        m_poSplitIndex->add(qry.field(0).asInteger(), i);                                
                }
                qry.close();
}


void ParamInfoMgr::loadOfferCombine()
{
    char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_OFFERCOMBINEINFO_NUM, countbuf))
        {
                        THROW(MBC_ParamInfoMgr+66);
        }else{
                        num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poOfferCombineIndex->exist()))   
        m_poOfferCombineIndex->create(num*m_fRatio);
        if(!(m_poOfferDisctIndex->exist()))     
        m_poOfferDisctIndex->create(num*m_fRatio);
   PricingCombine* m_poCombine = (PricingCombine*)(*m_poPricingCombineData); 

    DEFINE_QUERY (qry);
    qry.setSQL ("select offer_id, nvl(pricing_plan_id, 0) from product_offer");
    qry.open ();
        unsigned int iCombineTmp = 0;
    while (qry.next ()) 
    {
        if (!qry.field(1).asInteger ()) continue;
        iCombineTmp = 0;
        if(m_poPlanIDIndex->get (qry.field (1).asInteger (), &iCombineTmp))
                {
                m_poOfferCombineIndex->add (qry.field (0).asInteger (), iCombineTmp);
        }
                for(;iCombineTmp>0;iCombineTmp = m_poCombine[iCombineTmp].m_iNext){
                        if(m_poCombine[iCombineTmp].m_iEventType == REAL_CYCLE_ACCT_EVENT_TYPE){
                                m_poOfferDisctIndex->add(qry.field (0).asInteger(), 1);
                                break;
                        }else continue;
                }
        }
    
    qry.close ();
}



void ParamInfoMgr::loadProductCombine()
{
    char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_PRODUCTCOMBINEINFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+68);
        }else{
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poProductCombineIndex->exist())) 
    m_poProductCombineIndex->create(num*m_fRatio);
    
    DEFINE_QUERY (qry);
    qry.setSQL ("select product_id, nvl(pricing_plan_id, 0) from product");
    qry.open ();
    while (qry.next ()) 
    {
        if (!qry.field(1).asInteger ()) continue;
        unsigned int iCombineTmp = 0;
        if(m_poPlanIDIndex->get (qry.field (1).asInteger (), &iCombineTmp))
        {
            m_poProductCombineIndex->add (qry.field (0).asInteger (), iCombineTmp);
        }
    }
    qry.close ();
}


void ParamInfoMgr::loadHost()
{
        char countbuf[32]= {0};
            int num = 0;
                        Log::log(0, "开始主机参数上载");
            if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_HOSTINFOINFO_NUM, countbuf))
            {
                    THROW(MBC_ParamInfoMgr+93);
            }else{
                    num = atoi(countbuf);
            }
            memset(countbuf,'\0',sizeof(countbuf));
        
            if(!(m_poHostInfoData->exist()))
            m_poHostInfoData->create(num*m_fRatio);
            
            if(!(m_poHostInfoIndex->exist()))
            m_poHostInfoIndex->create(num*m_fRatio);
                HostInfo *pHostInfo = (HostInfo*)(*m_poHostInfoData);
            checkMem(m_poHostInfoData->getCount() * sizeof(HostInfo));  
                TOCIQuery qry (Environment::getDBConn ());
                qry.setSQL ("select host_id, ip_addr, user_name, password,nvl(list_mode,0),nvl(org_id,-1) from b_host_info order by host_id");
                qry.open();
                int i=0;
                int iOrgID = 0;
                int iLastOrgID = 0;
                int iLast = 0;
                while (qry.next()) 
                {
                     i = m_poHostInfoData->malloc();
                     if(i<=0) THROW(MBC_ParamInfoMgr+94);
                     pHostInfo[i].m_iHostID = qry.field(0).asInteger();
                     strncpy(pHostInfo[i].m_sIP, qry.field(1).asString(),50);
                         strncpy(pHostInfo[i].m_sUserName, qry.field(2).asString(),50);
                         strncpy(pHostInfo[i].m_sPassword, qry.field(3).asString(),50);
                         pHostInfo[i].m_iListMode = qry.field(4).asInteger();
                         pHostInfo[i].m_iOrgID = qry.field(5).asInteger();
                         m_poHostInfoIndex->add(pHostInfo[i].m_iHostID, i);
                }
                qry.close();
        Log::log(0, "\t数据区. 记录数/总数: %d/%d", m_poHostInfoData->getCount(),m_poHostInfoData->getTotal());
        Log::log(0, "\t索引区. 记录数/总数: %d/%d", m_poHostInfoIndex->getCount(),m_poHostInfoIndex->getTotal());

}

// 
void  ParamInfoMgr::loadGatherTask()
{
        char countbuf[32]= {0};
            int num = 0;
            if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_GATHERDATAINFO_NUM, countbuf))
            {
                    THROW(MBC_ParamInfoMgr+86);
            }else{
                    num = atoi(countbuf);
            }
            memset(countbuf,'\0',sizeof(countbuf));
        
            if(!(m_poGatherTaskData->exist()))
            m_poGatherTaskData->create(num*m_fRatio);
            
            if(!(m_poGatherTaskIndex->exist()))
            m_poGatherTaskIndex->create(num*m_fRatio);
            
            if(!(m_poOrgGatherTaskIndex->exist()))
            m_poOrgGatherTaskIndex->create(num*m_fRatio);
            GatherTaskInfo *m_poGatherTaskInfo = (GatherTaskInfo *)(*m_poGatherTaskData); // m_poGatherTaskInfoList     
            GatherTaskInfo *pGatherTask = 0;
            checkMem(m_poGatherTaskData->getCount() * sizeof(GatherTaskInfo));  
            TOCIQuery qry (Environment::getDBConn ());
            qry.close ();
            /*char sSqlcode[2048] = {0};
        snprintf
        (
        sSqlcode,
        2048,
        "   select                  a.org_id, a.task_id, "
        "   nvl (a.host_id,-1)                  host_id, "
        "   trim(src_path)                     src_path, "
                "   trim(des_path)                     des_path, "
        "   trim(nvl(path_name_mask,'')) path_name_mask, "
        "   trim(nvl(file_name_mask,'')) file_name_mask, "
        "   nvl(to_number(gather_mode), 0)  gather_mode, "
        "   switch_id, "
        "   nvl( to_number(day_begin_offset), 0) begin,  "
        "   nvl( to_number(day_end_offset), 0)   end,    "
        "   nvl(ip_addr,'')                     ip_addr, "
        "   nvl(user_name,'')                 user_name, "
        "   nvl(password,'')                   password  "
                "   nvl(b.list_mode,0)                   list_mode "
        "   nvl( to_number(a.check_interval), 0) interval, "
        "   trim(a.comments)                     comments, "
        "   trim(a.task_type)                   task_type, "
        "   nvl( to_number(a.process_id), 0)   process_id, "
                "   trim(nvl (a.backup_path,''))      backup_path, "
                //"   trim(qc_up_path)                    qc_path, "
        //      "   nvl(qc_host,-1)                     qc_host, "
        //      "   nvl(process_id2,0)              process_id2  "
        "   from  b_gather_task a, b_host_info b         "
        "   where a.host_id = b.host_id                  "
        "   order by org_id, task_id, host_id, src_path  "
        );*/
                qry.setSQL (
                        "   select  nvl(a.org_id,-1), nvl(a.task_id,-1), "
                "   nvl (a.host_id,-1)                  host_id, "
                "   trim(src_path)                     src_path, "
                        "   trim(des_path)                     des_path, "
                "   trim(nvl(path_name_mask,'')) path_name_mask, "
                "   trim(nvl(file_name_mask,'')) file_name_mask, "
                "   nvl(to_number(gather_mode), 0)  gather_mode, "
                "   switch_id, "
                "   nvl( to_number(day_begin_offset), 0) begin,  "
                "   nvl( to_number(day_end_offset), 0)   end,    "
                "   nvl(ip_addr,'')                     ip_addr, "
                "   nvl(user_name,'')                 user_name, "
                "   nvl(password,'')                   password,  "
                        "   nvl(b.list_mode,0)                   list_mode, "
                "   nvl( to_number(a.check_interval), 0) interval, "
                "   trim(a.comments)                     comments, "
                "   trim(a.task_type)                   task_type, "
                "   nvl( to_number(a.process_id), 0)   process_id, "
                "   trim(nvl (a.backup_path,''))      backup_path, "
                                
                "   trim(nvl (qc_up_path,''))           qc_path, "
                "   nvl(qc_host,-1)                     qc_host, "
                "   nvl(process_id2,0)              process_id2, "
                "   nvl(busi_class,0)                busi_class, "
                "   nvl(data_source,0)               data_source, "
				"	trim(nvl (file_name_modify,'')) file_name_modify "
                "   from  b_gather_task a, b_host_info b         "
                "   where nvl(a.host_id,-1) = b.host_id(+)       "
                "   order by a.org_id, a.task_id, a.host_id, a.src_path  "
                );
                qry.open();
                int i=0;
                int iOrgID = 0;
                int iLastOrgID = 0;
                int iLast = 0;
        while (qry.next()) 
        {
                i = m_poGatherTaskData->malloc();
            if(i<=0) THROW(MBC_ParamInfoMgr+87);
            memset(&(m_poGatherTaskInfo[i]), '\0', sizeof(GatherTaskInfo));
            pGatherTask = &m_poGatherTaskInfo[i];
            pGatherTask->m_iNext = 0;
            iOrgID = qry.field(0).asInteger();
            pGatherTask->m_iTaskID = qry.field(1).asInteger();
            pGatherTask->m_iOrgID = iOrgID;
            strncpy(pGatherTask->m_sSrcPath, qry.field( "src_path" ).asString(), 500 );
            strncpy(pGatherTask->m_sDescPath, qry.field( "des_path" ).asString(), 50);
             
            if(pGatherTask->m_sSrcPath[0] != '\0'
                &&pGatherTask->m_sSrcPath[strlen(pGatherTask->m_sSrcPath ) - 1] != '/')
            {
                strcat(pGatherTask->m_sSrcPath, "/" );
            }
             
             pGatherTask->m_iCheckInterval = qry.field( "interval" ).asInteger();
             //pGatherTask->m_iTimeCnt = pGatherTask->m_iCheckInterval; //
             pGatherTask->m_iDefaultSwitchID = qry.field( "switch_id" ).asInteger();
             pGatherTask->m_iSwitchID = qry.field( "switch_id" ).asInteger();
			 strcpy( pGatherTask->m_sFileNameWcbak, qry.field( "file_name_mask" ).asString() );
			 strcpy( pGatherTask->m_sModifyFileName,qry.field("file_name_modify").asString()) ;
             //sprintf(pGatherTask->m_sTaskName, "%s_%d", m_sSrcPath, m_iSwitchID);
             sprintf (pGatherTask->m_sTaskName, "%s", pGatherTask->m_sSrcPath);  
             strcpy(pGatherTask->m_sFileNameWildcard, qry.field( "file_name_mask" ).asString() );          
             pGatherTask->m_iHostID = qry.field( "host_id" ).asInteger();          
             strncpy
             (
                pGatherTask->m_sPathNameWildcard,
                qry.field( "path_name_mask" ).asString(), 50
             );
             
             pGatherTask->m_iBegin = qry.field( "begin" ).asInteger();
             pGatherTask->m_iEnd = qry.field( "end" ).asInteger();
             pGatherTask->m_iGatherMode = qry.field( "gather_mode" ).asInteger();         
             //malloc
             pGatherTask->m_sIP[0] = '\0';
             pGatherTask->m_iPasv = 0;
             pGatherTask->m_iPort = 21;
             pGatherTask->m_sUserName[0] = '\0';
             pGatherTask->m_sPassword[0] = '\0';
             if(-1 != pGatherTask->m_iHostID){
                        strncpy(pGatherTask->m_sIP, qry.field( "ip_addr" ).asString(), 50);
                        strncpy(pGatherTask->m_sUserName, qry.field( "user_name" ).asString(), 50);
                        strncpy(pGatherTask->m_sPassword, qry.field( "password" ).asString(), 50);
                        pGatherTask->m_iListMode = qry.field("list_mode").asInteger();
             }
                //new add           
             strncpy(pGatherTask->m_sComments, qry.field( "comments" ).asString(), 50);
             strncpy(pGatherTask->m_sTaskType, qry.field( "task_type" ).asString(), 10);
             pGatherTask->m_iProcessID = qry.field( "process_id" ).asInteger();
             strncpy(pGatherTask->m_sBackupPath, qry.field( "backup_path" ).asString(), 500 );
                         
             if(pGatherTask->m_iHostID<0 )
                        pGatherTask->m_iQcSend = 0;
                                 
                 pGatherTask->m_iQcHostID = qry.field( "qc_host" ).asInteger();              
                strncpy(pGatherTask->m_sQcOrgPath, qry.field( "qc_path" ).asString(), sizeof(pGatherTask->m_sQcOrgPath) );            
                pGatherTask->m_iProcessID2 = qry.field( "process_id2" ).asInteger();
                if( pGatherTask->m_iQcHostID <= 0){
                        pGatherTask->m_iQcSend = 0;
                }
                if ( pGatherTask->m_sQcOrgPath[0] != '\0' 
                         && pGatherTask->m_sQcOrgPath[strlen( pGatherTask->m_sQcOrgPath ) - 1] != '/' ){
                  strcat( pGatherTask->m_sQcOrgPath, "/" );
                }
             if(pGatherTask->m_sQcOrgPath[0]=='\0')
                strncpy(pGatherTask->m_sQcOrgPath,pGatherTask->m_sOrgSrcPath,sizeof(pGatherTask->m_sQcOrgPath));
             strncpy(pGatherTask->m_sQcPath, pGatherTask->m_sQcOrgPath, sizeof(pGatherTask->m_sQcPath)); 
                 
             pGatherTask->m_iBusiClass = qry.field( "busi_class" ).asInteger();
                 pGatherTask->m_iDataSource = qry.field( "data_source" ).asInteger();
                                
           if (pGatherTask->m_sIP[0] != NULL ){
             char *pTmp = 0;
             if ( ( pTmp = strchr(pGatherTask->m_sIP, '#' ) ) != NULL ){
                    pGatherTask->m_iPasv = 1;
                    *pTmp = '\0';
             }
             if ( ( pTmp = strchr(pGatherTask->m_sIP, ':' ) ) != NULL ){
                    *pTmp = NULL;
                    pGatherTask->m_iPort = atoi( ++pTmp );
                    if (pGatherTask->m_iPort <= 0 ){
                        pGatherTask->m_iPort = 21;
                    }
             }
                }
            if( strlen( pGatherTask->m_sTaskType ) == 0 ){
                        strcpy( pGatherTask->m_sTaskType, "F" );
            }



            pGatherTask->m_iFileNum = 0;
            pGatherTask->m_sMinCreateDate[0] = '\0' ;
            pGatherTask->m_sMaxCreateDate[0] = '\0' ;  
            if(pGatherTask->m_iTaskID != -1)
                m_poGatherTaskIndex->add(pGatherTask->m_iTaskID, i);
            if(iOrgID == iLastOrgID){
                pGatherTask[iLast].m_iNext = i;
            }else{
                if(iOrgID != -1)
                        m_poOrgGatherTaskIndex->add(iOrgID,i);
            }
            iLastOrgID = iOrgID;
            iLast = i;
        }
        qry.close(); 
}


void ParamInfoMgr::loadProductAcctItem()
{
        char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_PRODUCTACCTITEMINFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+90);
        }else{
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
            
        if(!(m_poProductIDIndex->exist()))
        m_poProductIDIndex->create(num*1.3);
        
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_PROID_PRODUCTACCTITEMINFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+91);
        }else{
                num = atoi(countbuf);
        }
        unsigned int m_iNum2 = 23;
        unsigned int m_iNum3 = 3;   
        if(!(m_poProIDAcctItemStringIndex->exist()))
        m_poProIDAcctItemStringIndex->create(num*m_fRatio, m_iNum2);    
        
        DEFINE_QUERY (qry);
        int iProductID;
        qry.setSQL ("select product_id, acct_item_type_id from b_prod_acct_item_type order by product_id");
        qry.open ();
        while (qry.next ()) {
                unsigned int gcl = 0;
                iProductID = qry.field (0).asInteger ();
                if(!m_poProductIDIndex->get(iProductID, &gcl))
                {
                     m_poProductIDIndex->add(iProductID, 1);
                }
                addProductAcctItem(iProductID, qry.field (1).asInteger ());
        }
        qry.close ();
}


void ParamInfoMgr::addProductAcctItem(int iProductID, int iAcctItemTypeID)
{ 
        DEFINE_QUERY(qry);
        char sSQL[2048];
        sprintf (sSQL,  " select acct_item_type_id from acct_item_type"
                        " connect by summary_acct_item_type_id = prior acct_item_type_id "
                        " start with acct_item_type_id = %d", iAcctItemTypeID);
        qry.setSQL (sSQL); 
        qry.open ();
        int j = 0;
        char m_sTmpBuf[23] = {0};
        while (qry.next ()) {
                        memset(m_sTmpBuf, '\0', 23*sizeof(char));
                        sprintf(m_sTmpBuf,"%d_%d", iProductID, qry.field (0).asInteger ());     
                        unsigned int i = 0;
                        if(!m_poProIDAcctItemStringIndex->get(m_sTmpBuf , &i))
                                m_poProIDAcctItemStringIndex->add(m_sTmpBuf , 1);
        }
        qry.close ();
}

// 
void ParamInfoMgr::loadSpecialTariff()
{
        int iCount = 0,m=0;
        int i = 0;
        unsigned int j = 0;
        char sTemp[40];
        SpecialTariff * pSpecialTariffTemp;
        SpecialTariff * pSpecialTariff;

        TOCIQuery qry(Environment::getDBConn());
        char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SPECIALTRIFFINFO_NUM, countbuf))
        {
                        THROW(MBC_ParamInfoMgr+80);
        }else{
                        num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poSpecialTariffData->exist()))
                m_poSpecialTariffData->create(num*m_fRatio);  //m_poSpecialTariffList;
        if(!(m_poSpecialTariffIndex->exist()))
                m_poSpecialTariffIndex->create(num*m_fRatio, 40);//StringIndex
        pSpecialTariffTemp = (SpecialTariff*)(*m_poSpecialTariffData);
        checkMem(m_poSpecialTariffData->getCount() * sizeof(SpecialTariff)); 
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SPECIALTRIFFCHECKINFO_NUM, countbuf))
        {
                        THROW(MBC_ParamInfoMgr+82);
        }else{
                        num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poSpecialTariffCheckData->exist()))      
                m_poSpecialTariffCheckData->create(num*m_fRatio);  //m_poSpecialTariffCheckList
        if(!(m_poSpecialTariffCheckIndex->exist()))
                m_poSpecialTariffCheckIndex->create(num*m_fRatio, 40);//StringIndex
        pSpecialTariff = (SpecialTariff*)(*m_poSpecialTariffCheckData);
        checkMem(m_poSpecialTariffCheckData->getCount() * sizeof(SpecialTariff)); 
    qry.setSQL("select up_down_flag,sp_nbr,in_nbr,service_nbr,billing_type,"
                       " nvl(eff_date,to_date('20000501','yyyymmdd')) eff_date,"
                       " nvl(exp_date,to_date('30000501','yyyymmdd')) exp_date,"
                       " tariff_rate from b_js_sp_special_process a,"
                       " b_js_sp_tariff_cfg b where a.special_type = 1 and a.tariff_id=b.tariff_id"
                       " ORDER BY up_down_flag,sp_nbr,billing_type,service_nbr,in_nbr asc");
    qry.open();
        while(qry.next())
        {
                i = m_poSpecialTariffData->malloc();
                if(i<=0)  THROW(MBC_ParamInfoMgr+81);
                pSpecialTariffTemp[i].m_iUpDownFlag = qry.field(0).asInteger();
                strcpy(pSpecialTariffTemp[i].m_sSPFLAG,qry.field(1).asString());
                strcpy(pSpecialTariffTemp[i].m_sNBR,qry.field(2).asString());
                strcpy(pSpecialTariffTemp[i].m_sServiceNBR,qry.field(3).asString());
                pSpecialTariffTemp[i].m_iBillingType = qry.field(4).asInteger();
                strcpy(pSpecialTariffTemp[i].m_sEffDate,qry.field(5).asString());
                strcpy(pSpecialTariffTemp[i].m_sExpDate,qry.field(6).asString());
                pSpecialTariffTemp[i].m_iCharge = qry.field(7).asInteger();
                //pSpecialTariffTemp[i].m_poNext = 0;
                pSpecialTariffTemp[i].m_iNext = 0;
                sprintf(sTemp,"%d_%s_%d",pSpecialTariffTemp[i].m_iUpDownFlag,
                        pSpecialTariffTemp[i].m_sSPFLAG,pSpecialTariffTemp[i].m_iBillingType);
                if(m_poSpecialTariffIndex->get(sTemp,&j))
                        {
                        pSpecialTariffTemp[i].m_iNext = j;
                        }
                        m_poSpecialTariffIndex->add(sTemp,i);
        }
        qry.close();
        qry.commit();

        
    qry.setSQL("select up_down_flag,sp_nbr,in_nbr,service_nbr,"
                       " nvl(eff_date,to_date('20000501','yyyymmdd')) eff_date,"
                       " nvl(exp_date,to_date('30000501','yyyymmdd')) exp_date,"
                       " tariff_rate from b_js_sp_special_process a,b_js_sp_tariff_cfg b"
                       " where a.special_type = 2 and a.tariff_id=b.tariff_id"
                       " ORDER BY up_down_flag,sp_nbr,service_nbr,in_nbr ASC");
    qry.open();
    i = 0;
    j = 0;
        while(qry.next())
        {
                i = m_poSpecialTariffCheckData->malloc();
                if(i<=0)  THROW(MBC_ParamInfoMgr+83);
                //pSpecialTariffTemp = m_poSpecialTariffCheckData+m;
                pSpecialTariff[i].m_iUpDownFlag = qry.field(0).asInteger();
                strcpy(pSpecialTariff[i].m_sSPFLAG,qry.field(1).asString());
                strcpy(pSpecialTariff[i].m_sNBR,qry.field(2).asString());
                strcpy(pSpecialTariff[i].m_sServiceNBR,qry.field(3).asString());
                strcpy(pSpecialTariff[i].m_sEffDate,qry.field(4).asString());
                strcpy(pSpecialTariff[i].m_sExpDate,qry.field(5).asString());
                pSpecialTariff[i].m_iCharge = qry.field(6).asInteger();
                //pSpecialTariffTemp[i].m_poNext = 0;
                pSpecialTariff[i].m_iNext = 0;
                memset(sTemp,'\0',40*sizeof(char));
                sprintf(sTemp,"%d_%s",pSpecialTariff[i].m_iUpDownFlag,
                        pSpecialTariff[i].m_sSPFLAG);
                j = 0;
                if(m_poSpecialTariffCheckIndex->get(sTemp,&j))
                        {
                        pSpecialTariff[i].m_iNext = j;
                        }
                m_poSpecialTariffCheckIndex->add(sTemp,i);
        }
        qry.close();
        qry.commit();
        return;
}

void ParamInfoMgr::loadSpInfo()
{
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32]= {0};
        int num = 0;
        Log::log(0, "开始ismp_prod_info上载");
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SPINFOSEEKINFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+84);
        }else{
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poSpInfoData->exist()))
                m_poSpInfoData->create(num*m_fRatio);  //m_poSpInfoDataList
        if(!(m_pHSpInfoIndex->exist()))
                m_pHSpInfoIndex->create(num*m_fRatio, 24);
        checkMem(m_poSpInfoData->getCount() * sizeof(SpInfoData)); 
        SpInfoData *pSpInfoData = (SpInfoData*)(*m_poSpInfoData);
        
        qry.setSQL(
        " select a.prd_offer_id,a.prd_id,a.policy_id,"
        " (select nvl(max(b.feetype),-1) from ismp_bill_mode b "
        " where b.policy_id = a.policy_id) feetype"
        " from ismp_prod_info a ");
    qry.open();
    
    int i=0;
    char sTemp[100];
    int iTemp = 0;
        while(qry.next()){
             i = m_poSpInfoData->malloc();
                 if(i<=0)  THROW(MBC_ParamInfoMgr+85);
             strncpy(pSpInfoData[i].m_sPrdOfferID,qry.field(0).asString(),23);
             strncpy(pSpInfoData[i].m_sPrdID,qry.field(1).asString(),23); 
             strncpy(pSpInfoData[i].m_sPolicyID,qry.field(2).asString(),23); 
         pSpInfoData[i].m_iFeeType = qry.field(3).asInteger();
                 
         m_pHSpInfoIndex->add(pSpInfoData[i].m_sPrdOfferID, i);
        }
        qry.commit();
        qry.close();
        Log::log(0, "\t数据区. 记录数/总数: %d/%d", m_poSpInfoData->getCount(),m_poSpInfoData->getTotal());
        Log::log(0, "\t索引区. 记录数/总数: %d/%d", m_pHSpInfoIndex->getCount(),m_pHSpInfoIndex->getTotal());
        
}

void ParamInfoMgr::loadGatherTaskListInfo()
{
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32]= {0};
        int num = 0;
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_GATERTASKLISTINFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+88);
        }else{
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poGatherTaskListInfoData->exist()))
                m_poGatherTaskListInfoData->create(num*m_fRatio);  //m_poSpInfoDataList
        if(!(m_poGatherTaskListInfoIndex->exist()))
                m_poGatherTaskListInfoIndex->create(num*m_fRatio);
        checkMem(m_poGatherTaskListInfoData->getCount() * sizeof(GatherTaskListInfo)); 
        GatherTaskListInfo *pGatherTaskListInfo = (GatherTaskListInfo*)(*m_poGatherTaskListInfoData);
        
        qry.setSQL ("select min(check_interval) interval from b_gather_task");
        qry.open ();
        qry.next ();
        int iMinInterval = qry.field("interval").asInteger ();
        qry.close();
        qry.setSQL(" select task_id, nvl(org_id,-1),  "
                   " nvl(to_number(check_interval), 0)"
                           " from b_gather_task order by task_id");
    qry.open();
    
    int i = 0;
        while(qry.next()){
             i = m_poGatherTaskListInfoData->malloc();
                 if(i<=0)  THROW(MBC_ParamInfoMgr+89);
                 int iTaskID = qry.field(0).asInteger();
                 pGatherTaskListInfo[i].m_iTaskID = iTaskID;
                 pGatherTaskListInfo[i].m_iMinInterval = iMinInterval;
                 pGatherTaskListInfo[i].m_iOrgStr = qry.field(1).asInteger();
                 pGatherTaskListInfo[i].m_iCheckIntervalStr = qry.field(2).asInteger();
                 pGatherTaskListInfo[i].m_iNext = 0;
                 unsigned int gcl = 0;
                 if(m_poGatherTaskListInfoIndex->get(iTaskID, &gcl)){
                        pGatherTaskListInfo[i].m_iNext = gcl;
                 }
                 m_poGatherTaskListInfoIndex->add(iTaskID, i);
        }
        qry.close();
}

void ParamInfoMgr::loadSegment()
{       
    TOCIQuery qry(Environment::getDBConn());
    char countbuf[32]= {0};
        int num = 0;
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SEGMENTINFO_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+221);
        }else{
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poSegmentInfoData->exist()))
                 m_poSegmentInfoData->create(num*m_fRatio);  //m_poSegmentInfoList
        if(!(m_poSegmentIndex->exist()))
                 m_poSegmentIndex->create(num*m_fRatio);
        if(!(m_poSegmentTypeIndex->exist()))
                 m_poSegmentTypeIndex->create(num*m_fRatio);
        checkMem(m_poSegmentInfoData->getCount() * sizeof(SegmentInfo)); 
        SegmentInfo *pSegmentInfo = (SegmentInfo*)(*m_poSegmentInfoData);
        
        /* Different SegmType belong to different group, 
         * use SegmType as the Level_1 contents.
         * In each group, each SegmValue sorted by eff_date in descding order,
         * use different SegmValue as the Level_2 contents in the group.
         * 
         *       Level_1            Level_2
         *                                         
         *      0|-SegmType1-------0|-SegmValue1->SegmValue1->SegmValue1->...
         *       |                  |
         *       |                 1|-SegmValue2->SegmValue2->...
         *       |                  |
         *       |                 :|...
         *       |                  |
         *       |                 n|-SegmValueN->...
         *       |               
         *  (n+1)|-SegmType2---(n+1)|-SegmValue(n+1)->SegmValue(n+1)->...
         *       |                  |
         *       |             (n+2)|-SegmValue(n+2)->...
         *       |                  |
         *       |                 :|...
         *       |                  |
         *       |             (n+m)|-SegmValue(n+m)->...
         *       :
         *       :  
         */
        qry.setSQL("select segment_id, segment_type, begin_segm, end_segm, "
                           "nvl(eff_date, sysdate) eff_date, "
                           "nvl(exp_date, to_date('20500101', 'yyyymmdd')) exp_date "
                           "from b_general_segment "
                           "order by segment_type, begin_segm, eff_date desc");
        qry.open();
    
    int i=0, k=0;
        int lastType=-1, lastPos=0, currCnt;
        char sTemp[8];
        while(qry.next()){
            i = m_poSegmentInfoData->malloc();
                if (i<=0) THROW (MBC_ParamInfoMgr+222);
            memset(&pSegmentInfo[i], '\0', sizeof(class SegmentInfo));
                pSegmentInfo[i].m_iSegmID = qry.field(0).asInteger();
                strcpy(sTemp, qry.field(1).asString());
                pSegmentInfo[i].m_iSegmType = sTemp[2] | 0x20; 
        
                strcpy(pSegmentInfo[i].m_sBeginSegm, qry.field(2).asString());
                strcpy(pSegmentInfo[i].m_sEndSegm, qry.field(3).asString());
                strcpy(pSegmentInfo[i].m_sEffDate, qry.field(4).asString());
                strcpy(pSegmentInfo[i].m_sExpDate, qry.field(5).asString());
                pSegmentInfo[i].bInited = true; 
                pSegmentInfo[i].m_iNext = 0;
                
                if(i>1) pSegmentInfo[i-1].m_iNextInfo = i;
                m_poSegmentIndex->add(pSegmentInfo[i].m_iSegmID, i);
            if(pSegmentInfo[i].m_iSegmType == lastType){
                    pSegmentInfo[i-1].m_iNext = i;
            }else{
                    m_poSegmentTypeIndex->add(pSegmentInfo[i].m_iSegmType, i);
            }
            lastType = pSegmentInfo[i].m_iSegmType;
        }
        qry.close();
        return;
}

void ParamInfoMgr::loadLongTypeRule()
{
    int num = 0;
        char countbuf[32]= {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_LONGTYPE_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+92);
        }else{
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poLongTypeIndex->exist()))
                 m_poLongTypeIndex->create(num*1.3);  
        
    DEFINE_QUERY (qry);
    qry.setSQL ("select CALLED_LONG_GROUP_TYPE_ID, LONG_TYPE_ID from B_LONG_TYPE_RULE");
    qry.open ();
    
    while (qry.next()) {
        int iGroup = qry.field(0).asInteger();
        int iLongType = qry.field(1).asInteger();
        
        m_poLongTypeIndex->add (iGroup, iLongType);
    }
    qry.close();
}


void ParamInfoMgr::TestSection2(unsigned int iSectionOffset)
{
         if(iSectionOffset <= 0) return;
         PricingSection *m_poSection = (PricingSection*)(*m_poPricingSectionInfoData);
         Tariff *m_poTariff = (Tariff*)(*m_poTariffInfoData);
         Discount *m_poDiscount = (Discount*)(*m_poDiscountInfoData);
         DisctApplyer *m_poDisctApplyer = (DisctApplyer*)(*m_poDisctApplyerInfoData);
         DisctApplyer::DisctApplyObject *m_poDisctApplyerObj = (DisctApplyer::DisctApplyObject*)(*m_poDisctApplyObjectInfoData);
         Percent *m_poPercent = (Percent*)(*m_poPercentInfoData);
         LifeCycle *m_poLifeCycle = (LifeCycle*)(*m_poLifeCycleData);
         FilterCond *m_poFilterCond = (FilterCond*)(*m_poFilterConditionData);
         ConditionPoint *m_poCondPoint = (ConditionPoint*)(*m_poConditionPointData);
         
         PricingSection *pSect = &(m_poSection[iSectionOffset]);
         printf("%s%d\n","段落ID: ",pSect->m_iSectionID);
         printf("%s%d\n","段落类型: ",pSect->m_iSectionType);
         printf("%s%d\n","段落[iChildNum]: ",pSect->m_iChildNum);
         printf("%s%d\n","段落条件ID: ",pSect->m_iConditionID);
         printf("%s%d\n","此段落的父段落ID: ",pSect->m_iParentSectionID);
         printf("%s%d\n","段落事件类型[BreakValue]: ",pSect->m_iBreakValue);
         printf("%s%d\n","段落本身偏移量: ",pSect->m_iPricingSectionOffset);
         printf("%s%d\n","段落子节点偏移量: ",pSect->m_iFirstChildOffset);
         printf("%s%d\n","段落兄弟节点偏移量: ",pSect->m_iBrotherOffset);
         printf("%s%d\n","段落ELSE节点偏移量: ",pSect->m_iElseSection);

         printf("%s%d\n","段落中ID[m_iAttrID]: ",pSect->m_poNewBreakID->m_iAttrID);
         printf("%s%d\n","段落中ID[m_iOwner]: ",pSect->m_poNewBreakID->m_iOwner);
         printf("%s%d\n","段落中ID[m_iExt]: ",pSect->m_poNewBreakID->m_iExt);
         printf("%s%ld\n","段落中ID[m_lValue]: ",pSect->m_poNewBreakID->m_lValue);
         printf("%s%s\n","段落中ID[m_sValue]: ",pSect->m_poNewBreakID->m_sValue);
         
         //资费...
         unsigned int iTmp = pSect->m_iTariffOffset;
         int m = 0;
         while(iTmp)
         {
              Tariff *pTariff = &(m_poTariff[iTmp]);
                  printf("%s%d%s%d\n","资费[",m,"]ID: ",pTariff->m_iTariffID);
                  printf("%s%d%s%s\n","资费[",m,"]类型[TariffType]: ",pTariff->m_sTariffType);
              printf("%s%d%s%d\n","资费[",m,"]影响的累积量[Resource]: ",pTariff->m_iResource);
                  printf("%s%d%s%d\n","资费[",m,"]对应账目类型[AcctItemType]: ",pTariff->m_iAcctItemType);
                  printf("%s%d%s%d\n","资费[",m,"]单位[UnitType]: ",pTariff->m_iUnitType);
              printf("%s%d%s%d\n","资费[",m,"]单元数(几个基本单元组成的计费单元)[Unit]: ",pTariff->m_iUnit);
                  printf("%s%d%s%d\n","资费[",m,"]计算方法: ",pTariff->m_iCalMethod);
                  printf("%s%d%s%d\n","资费[",m,"]费率[Rate]: ",pTariff->m_lRate);
                  printf("%s%d%s%d\n","资费[",m,"]费率精度[RatePrecision]: ",pTariff->m_iRatePrecision);
                  printf("%s%d%s%d\n","资费[",m,"]计算精度[CalPrecision]: ",pTariff->m_iCalPrecision);
                  printf("%s%d%s%d\n","资费[",m,"]封顶费用[FixedRate]: ",pTariff->m_lFixedRate);
                  printf("%s%d%s%d\n","资费[",m,"]IsParam: ",pTariff->m_iIsParam);
                  //printf("%s%d%s%d\n","资费[",m,"]ItemIsParam: ",pTariff->m_iItemIsParam);
                  if(pTariff->m_iNextOffset) 
                  {
                                iTmp = pTariff->m_iNextOffset;
                                m++;
                  }else {
                                break;
                  }
         }
         
         //优惠...
         unsigned int iTmp1 = pSect->m_iDiscountOffset;
         int m1 = 0;
         while(iTmp1)
         {
              Discount * pDisc= &(m_poDiscount[iTmp1]);
                  
                  printf("%s%d%s%d\n","优惠[",m1,"]ID: ", pDisc->m_iDiscountID);
                  printf("%s%d%s%d\n","优惠[",m1,"]计算精度: ", pDisc->m_iCalPrecision);
                  printf("%s%d%s%d\n","优惠[",m1,"]Priority: ", pDisc->m_iPriority);
                  printf("%s%d%s%d\n","优惠[",m1,"]OperationID: ", pDisc->m_iOperationID);
                  printf("%s%d%s%d\n","优惠[",m1,"]RuleType: ", pDisc->m_iRuleType);
                  //Applyer
                  if(pDisc->m_iApplyer)
                  {
                                DisctApplyer *pDisctApp = &(m_poDisctApplyer[pDisc->m_iApplyer]);
                                                        
                                printf("%s%d\n","优惠分摊者对应的优惠ID: ", pDisctApp->m_iDisctID);
                                printf("%s%d\n","优惠分摊者的分摊方法: ", pDisctApp->m_iApplyMethod);
                                printf("%s%d\n","优惠分摊者的iRef: ", pDisctApp->m_iRef);
                                //FilterCond
                                unsigned int iTmp2 = pDisctApp->m_iRef;
                                int m2 = 0;
                                while(iTmp2)
                                {
                                         FilterCond *pFCond = &(m_poFilterCond[iTmp2]);
                                         printf("%s%d%s%d\n","FilterCond[",m2,"]MemberID: ", pFCond->m_iMemberID);
                                         printf("%s%d%s%d\n","FilterCond[",m2,"]GroupID: ", pFCond->m_iGroupID);
                                         printf("%s%d%s%d\n","FilterCond[",m2,"]iExt: ", pFCond->m_iExt);
                                         printf("%s%d%s%d\n","FilterCond[",m2,"]iNext: ", pFCond->m_iNext);
                                         if(pFCond->m_iNext)
                                         {
                                                iTmp2 = pFCond->m_iNext;
                                                m2++;   
                                         }else{
                                                        break;
                                         }
                                }
                                
                                //ApplyObject
                                unsigned int iTmp3 = pDisctApp->m_iApplyObject;
                                int m3 = 0;
                                while(iTmp3)
                                {
                                        DisctApplyer::DisctApplyObject *pDisctAppObj = &(m_poDisctApplyerObj[iTmp3]);
                                        printf("%s%d%s%d\n","优惠对象[",m3,"]ObjectOwnerID: ", pDisctAppObj->m_iObjectOwnerID);
                                        printf("%s%d%s%d\n","优惠对象[",m3,"]ObjectID: ", pDisctAppObj->m_iObjectID);
                                        printf("%s%d%s%d\n","优惠对象[",m3,"]ReferenceID: ", pDisctAppObj->m_iReferenceID);
                                        printf("%s%d%s%d\n","优惠对象[",m3,"]iExt: ", pDisctAppObj->m_iExt);
                                         
                                                if(pDisctAppObj->m_iNext)
                                                {
                                                        iTmp3 = pDisctAppObj->m_iNext;
                                                        m3++;   
                                                }else{
                                                        break;
                                                }        
                                }
                  }
                  
                  //优惠next
                  if(pDisc->m_iNext)
                  {
                                iTmp1 = pDisc->m_iNext;
                                m1++;
                  }else{
                                break;
                  }
         }
         
         //折扣...
         if(pSect->m_iPercentOffset)
         {
              Percent * pPercent = &(m_poPercent[pSect->m_iPercentOffset]); 
                  printf("%s%d\n","折扣ID: ",pPercent->m_iPercentID);
                  printf("%s%d\n","折扣[分子]: ",pPercent->m_iNume);
                  printf("%s%d\n","折扣[NumeIsParam]: ",pPercent->m_iNumeIsParam);
                  printf("%s%d\n","折扣[分母]: ",pPercent->m_iDeno);
                  printf("%s%d\n","折扣[DenoIsParam]: ",pPercent->m_iDenoIsParam);
                  printf("%s%d\n","折扣[RuleType]: ",pPercent->m_iRuleType);
         }
         
         //ConditionPoint...
         if(pSect->m_iConditionPointOffset)
         {
              ConditionPoint *pCondPoint = &(m_poCondPoint[pSect->m_iConditionPointOffset]);
                  printf("%s%ld\n","ConditionPoint[m_lValue]: ",pCondPoint->m_lValue);
                  printf("%s%d\n","ConditionPoint[m_iValueType]: ",pCondPoint->m_iValueType);
                  printf("%s%d\n","ConditionPoint[m_iSection]: ",pCondPoint->m_iSection);
                  printf("%s%d\n","ConditionPoint[m_iAddValue]: ",pCondPoint->m_iAddValue);
         }
         
         
         if(pSect->m_iFirstChildOffset)   
         {
                printf("%s%d%s\n","段落号[", pSect->m_iSectionID,"][*的FirstChild*]:... ");
                TestSection2(pSect->m_iFirstChildOffset);
         }
         if(pSect->m_iBrotherOffset)      
         {
                printf("%s%d%s\n","段落号[", pSect->m_iSectionID,"][*的Brother*]:... ");
                TestSection2(pSect->m_iBrotherOffset);
         }
         if(pSect->m_iElseSection)                
         {
                printf("%s%d%s\n","段落号[", pSect->m_iSectionID,"][*的ElseSection*]:... ");
                TestSection2(pSect->m_iElseSection);
         }
         
}

void ParamInfoMgr::TestCombine2(int iCombineID)
{
                PricingSection *m_poSection = (PricingSection*)(*m_poPricingSectionInfoData);
                PricingCombine* m_poCombine = (PricingCombine*)(*m_poPricingCombineData);
                PricingMeasure* m_poMeasure = (PricingMeasure*)(*m_poPricingMeasureData);
                LifeCycle *m_poLifeCycle = (LifeCycle*)(*m_poLifeCycleData);
                
                unsigned int iTmp = 0;
                PricingCombine* pCombine1 = 0;
                LifeCycle *pLifeCycle1 = 0;
                
                if(m_poPricingCombineIndex->get(iCombineID, &iTmp))
                {
                        pCombine1 = &(m_poCombine[iTmp]);
                }else{
                    printf("%s\n","此组合数据不存在");
                        return;
                }
                printf("%s%d\n","组合ID: ",pCombine1->m_iCombineID);
                printf("%s%d\n","组合生命周期ID: ",pCombine1->m_iLifeCycle);
                printf("%s%d\n","组合所属计划ID: ",pCombine1->m_iPricingPlanID);
                //printf("%s%d\n","组合ID: ",pCombine1->m_iCombineID);
                printf("%s%d\n","组合对象ID: ",pCombine1->m_iObjectID);
                printf("%s%d\n","组合事件类型: ",pCombine1->m_iEventType);
                printf("%s%d\n","组合Priority: ",pCombine1->m_iPriority);
                
                if(pCombine1->m_iLifeCycle>0)
                {
                        pLifeCycle1 = &m_poLifeCycle[pCombine1->m_iLifeCycle];
                        printf("%s%d\n","生命周期ID: ",pLifeCycle1->m_iCycleID);
                        printf("%s%d\n","生命周期类型: ",pLifeCycle1->m_iCycleType);
                        printf("%s%d\n","生命周期生效日期[相对]: ",pLifeCycle1->m_iEffValue);
                        printf("%s%d\n","生命周期失效日期[相对]: ",pLifeCycle1->m_iExpValue);
                        printf("%s%s\n","生命周期生效日期[绝对]: ",pLifeCycle1->m_sEffDate);
                        printf("%s%s\n","生命周期失效日期[绝对]: ",pLifeCycle1->m_sExpDate);
                }

                //PricingMeasure* pMeasure = pCombine1->m_poFirstMeasure;
                unsigned int iMeaTmp =  pCombine1->m_iFirstMeasure;
                printf("%s%d\n","度量偏移量: ",pCombine1->m_iFirstMeasure);
                
                while(iMeaTmp>0)
                {
                        //PricingSection *pSection1 = pMeasure->m_poFirstSection;
                        PricingMeasure* pMeasure = &(m_poMeasure[iMeaTmp]);
                        printf("%s%d\n","度量的UsageType: ",pMeasure->m_iUsageType);
                        printf("%s%d\n","度量的MeasureMethod: ",pMeasure->m_iMeasureMethod);
                        printf("%s%d\n","度量下的FirstSection: ",pMeasure->m_iFirstSection);
                        printf("%s%d\n","此度量的下一个度量偏移量: ",pMeasure->m_iNext);
                        printf("%s%d\n","度量MeasureSeq: ",pMeasure->m_iMeasureSeq);
                        printf("%s%d\n","度量m_poID[m_iAttrID]: ",pMeasure->m_poID->m_iAttrID);
                        printf("%s%d\n","度量m_poID[m_iOwner]: ",pMeasure->m_poID->m_iOwner);
                        printf("%s%d\n","度量m_poID[m_iExt]: ",pMeasure->m_poID->m_iExt);
                        printf("%s%ld\n","度量m_poID[m_lValue]: ",pMeasure->m_poID->m_lValue);
                        printf("%s%s\n","度量m_poID[m_sValue]: ",pMeasure->m_poID->m_sValue);
                        unsigned int iTmp = pMeasure->m_iFirstSection;

                        unsigned int iTmp2 = 0;
                        int iMark = 0;
                        printf("%s%d\n","该度量下的首个子段落偏移量: ",iTmp);
                        if(iTmp>0)
                        {
                                TestSection2(iTmp);
                                iMark++;
                        }
                        
                        printf(" \n");
                        if(pMeasure->m_iNext>0)
                        {
                                iMeaTmp = pMeasure->m_iNext;
                        }else{
                                //iMeaTmp = 0;
                                return;
                        }
                }
                
                return;                 
                
}


// CheckRule
void ParamInfoMgr::loadCheckRule()
{
        int num = 0;
        char countbuf[32]= {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_CHECKRULE_RULE_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+38);
        }else{
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poCheckRuleData->exist()))
                 m_poCheckRuleData->create(num*1.3);  
        if(!(m_poCheckRuleIndex->exist()))
                 m_poCheckRuleIndex->create(num*1.3);  
        CheckRule *p = (CheckRule*)(*m_poCheckRuleData);
        checkMem(m_poCheckRuleData->getCount() * sizeof(CheckRule)); 
        int count, i;
        TOCIQuery qry(Environment::getDBConn());
        qry.setSQL ( "select rule_id, nvl(TAG, -1) TAG, "
                                 "nvl(err_code, -1) err_code, nvl(err_level, 0) err_level "
                                 "from Check_Rule ");
        qry.open ();
        i = 0;
        int iRuleID = 0;
        int iTag    = 0;
        int iErrorCode = 0;
        while (qry.next ()) {
                i = m_poCheckRuleData->malloc();
                if (i<=0) THROW (MBC_ParamInfoMgr+39);
                memset(&(p[i]), '\0', sizeof(class CheckRule));
        iRuleID = qry.field(0).asInteger();
        iTag    = qry.field(1).asInteger();
        iErrorCode = qry.field(2).asInteger();
        if (iTag<0){    
                                Log::log (0, "表Check_Rule中，规则(rule_id=%d), TAG值在无效", 
                                    iRuleID);                   
                                continue;
                }
                if (iErrorCode == -1){  
                                Log::log (0, "表Check_Rule中，规则(rule_id=%d), err_code值非法", iRuleID);                      
                                continue;
                }       
                p[i].m_iRuleID = iRuleID;
                p[i].m_iTAG    = iTag;
                p[i].m_iErrCode = iErrorCode;
                p[i].m_iErrLevel = qry.field(3).asInteger();
                m_poCheckRuleIndex->add (p[i].m_iRuleID, i);
        }
        qry.close ();
}


// CheckSubRule
void ParamInfoMgr::loadCheckSubRule()
{
        int num = 0;
        char countbuf[32]= {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_CHECKRULE_SUBRULE_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+40);
        }else{
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poCheckSubRuleData->exist()))
                 m_poCheckSubRuleData->create(num*1.3);  
        if(!(m_poCheckSubRuleIndex->exist()))
                 m_poCheckSubRuleIndex->create(num*1.3);  
		checkMem(m_poCheckSubRuleData->getCount() * sizeof(CheckSubRule));  
        CheckSubRule *p = (CheckSubRule*)(*m_poCheckSubRuleData);
        CheckRule *pCheckRule = (CheckRule*)(*m_poCheckRuleData);
        int count, i;
        TOCIQuery qry(Environment::getDBConn());
        qry.setSQL(     "select a.sub_rule_id, nvl(b.rule_id, -1) rule_id, "
                "nvl(a.func_id, -1) func_id, nvl(a.check_seq, -1) check_seq, "
                "nvl(a.not_id, 0) not_id "
                "from Check_Subrule a, Check_Rule b "
                "where a.rule_id = b.rule_id(+) "
                "order by rule_id, check_seq asc " );
        qry.open();
        i = 0;
        int ioldRuleID = -2;  // 因为空的iRuleID会用NVL转换为-1，故此处不能用-1
        CheckRule * poRule = NULL;
        CheckSubRule * poldSubRule = NULL;
        while (qry.next()){
                i = m_poCheckSubRuleData->malloc();
                if (i <= 0) THROW(MBC_ParamInfoMgr + 41);
                memset(&(p[i]), '\0', sizeof(class CheckSubRule));
                //检查此SubRule是否存在归属的Rule
                int iSubRuleID = qry.field(0).asInteger();
                int iRuleID    = qry.field(1).asInteger();
                int iFuncID    = qry.field(2).asInteger();
                //合法性检查
                if (iRuleID == -1){     
                        Log::log (0, "表Check_Subrule中，子规则(sub_rule_id=%d), 没有找到其归属的校验规则", iSubRuleID);                        
                        continue;
                }else if (iFuncID == -1){
                    Log::log (0, "表Check_Subrule中，子规则(sub_rule_id=%d), func_id值非法", iSubRuleID);                       
                        continue;
                }
                //若存在,且是Rule的第一个子规则，则使Rule指向本SubRule
                if ( ioldRuleID != iRuleID ){
                        unsigned int iRule = 0;
                        if (!m_poCheckRuleIndex->get(iRuleID, &iRule )){        
                                Log::log (0, "表Check_Subrule中，子规则(sub_rule_id=%d), 没有找到其归属的校验规则", iSubRuleID);                        
                                continue;
                        }
                        poRule = &(pCheckRule[iRule]);  
                        poRule->m_iFirstSubRule = i;
                        ioldRuleID = iRuleID;
                }else{     //若不是第一子规则，则使上一个子规则指向本子规则
                        poldSubRule->m_iNextSubRule = i;
                }
                p[i].m_iSubRuleID  = iSubRuleID;
                p[i].m_iRuleID     = iRuleID;
                p[i].m_iFuncID     = iFuncID;
                p[i].m_iCheckSeq   = qry.field(3).asInteger();
                p[i].m_iNOT        = qry.field(4).asInteger();
                
                m_poCheckSubRuleIndex->add(iSubRuleID, i);
                poldSubRule = &(p[i]);          
        }
        qry.close();
}

// CheckArgument
void ParamInfoMgr::loadArgument()
{
        int num = 0;
        char countbuf[32]= {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_CHECKARGUMENT_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+42);
        }else{
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poCheckArgumentData->exist()))
                 m_poCheckArgumentData->create(num*1.3);  
		checkMem(m_poCheckArgumentData->getCount() * sizeof(CheckArgument));  
        CheckArgument *pCheckArgument = (CheckArgument*)(*m_poCheckArgumentData);
        CheckSubRule *pCheckSubRule = (CheckSubRule*)(*m_poCheckSubRuleData);
                
        int count, i;
        TOCIQuery qry(Environment::getDBConn());
        qry.setSQL(     "select a.arg_id, nvl(b.sub_rule_id, -1) sub_rule_id, "
                "nvl(a.location, -1) location, nvl(trim(a.value), '') value "
                "from Check_Argument a, Check_Subrule b "
                "where a.sub_rule_id = b.sub_rule_id(+) "
                "order by sub_rule_id, location  asc ");
        qry.open();
        i = 0;
        int ioldSubRuleID = -2;
        int ioldLocation  = -2;
        CheckSubRule *poSubRule = NULL;
        unsigned int ioldSibArgument = 0;
        unsigned int iParentSibArgument = 0;
        unsigned int ioldArgument = 0;
        unsigned int iprev = 0;
        unsigned int icurr = 0;
        unsigned int m_iP = 0;
        while (qry.next()){
                i = m_poCheckArgumentData->malloc();
                if (i <= 0)      THROW(MBC_ParamInfoMgr + 43);
                memset(&(pCheckArgument[i]), '\0', sizeof(class CheckArgument));
                //检查此参数是否存在归属的子规则
                int iArgID     = qry.field(0).asInteger();
                int iSubRuleID = qry.field(1).asInteger();
                int iLocation  = qry.field(2).asInteger();
                //合法性检查
                if (iSubRuleID <=0 ){
                Log::log (0, "表Check_Argument中，参数(arg_id=%d), sub_rule_id值非法!", iArgID);                        
                    continue;
                }
                if (iLocation <0 ){
                Log::log (0, "表Check_Argument中，参数(arg_id=%d), location值非法!", iArgID);                   
                    continue;
                }
                //若存在，且此参数是SubRule的第一个参数，则使SubRule指向此参数
                if (ioldSubRuleID != iSubRuleID){
                        poSubRule = NULL;
                        unsigned int iSubRule = 0;
                        if (!m_poCheckSubRuleIndex->get(iSubRuleID, &iSubRule)) {       
                                Log::log (0, "表Check_Argument中，参数(arg_id=%d), 没有找到其归属的子规则", iArgID);                    
                                continue;
                        }
                        //poSubRule = &(pCheckSubRule[iSubRule]);
                        //poSubRule->m_iFirstAgrument = i;
                        pCheckSubRule[iSubRule].m_iFirstAgrument = i;
                        ioldSubRuleID = iSubRuleID;
                        //poldSibArgument = &(pCheckArgument[i]);
                        ioldSibArgument = i;
                }
                //若归属于同一个子规则，但参数的位置发生改变
                else if (ioldLocation != iLocation ){
                    //pParentSibArgument = poldSibArgument;
                        iParentSibArgument = ioldSibArgument;
                    //poldSibArgument = &(pCheckArgument[i]);
                        ioldSibArgument = i;
                    pCheckArgument[iParentSibArgument].m_iNextSibArgument = ioldSibArgument;    
                }
                //若归属于同一个子规则，参数位置未发生改变
                else{
                    //对同一位置的参数，按其字符串长度，从大到小进行排序，
                    //方便参数作为号头时，按顺序进行最大匹配。
                    //prev = poldSibArgument;
                    //curr = poldSibArgument;
                        //p = &(pCheckArgument[i]);
                        iprev = ioldSibArgument;
                    icurr = ioldSibArgument;
                        m_iP = i;
                    strcpy(pCheckArgument[i].m_sValue, qry.field(3).asString());
                    //如果p的长度大于poldSibArgument的长度
                    if (strlen(pCheckArgument[m_iP].m_sValue)>strlen(pCheckArgument[icurr].m_sValue)){
                        pCheckArgument[m_iP].m_iNextArgument = icurr;
                                ioldSibArgument = m_iP;
                                pCheckArgument[iParentSibArgument].m_iNextSibArgument = ioldSibArgument;
                                iprev = 0;
                                icurr = 0;
                    }else{
                                iprev = icurr;
                                icurr = pCheckArgument[icurr].m_iNextArgument;
                    }
                    //从当前参数开始找，直至找到一个比较p长度小的参数
                    while (icurr!=0){
                        if (strlen(pCheckArgument[m_iP].m_sValue) >= strlen(pCheckArgument[icurr].m_sValue)){
                                        pCheckArgument[m_iP].m_iNextArgument = icurr;
                                        pCheckArgument[iprev].m_iNextArgument = m_iP;
                            break;                            
                        }
                        else{
                            iprev = icurr;
                            icurr = pCheckArgument[icurr].m_iNextArgument;
                        }
                    }
                    //p的长度最短，应放在链表最后
                    if (icurr==0 && iprev!=0){
                                pCheckArgument[iprev].m_iNextArgument = m_iP;
                                ioldArgument = m_iP;
                    }
                }
                pCheckArgument[i].m_iArgID     = iArgID;
                pCheckArgument[i].m_iSubRuleID = iSubRuleID;
                pCheckArgument[i].m_iLocation  = iLocation;
                strcpy(pCheckArgument[i].m_sValue, qry.field(3).asString());
                ioldLocation = iLocation;
        }
        qry.close();
}

// CheckRelation
void ParamInfoMgr::loadCheckRelation()
{
        int num = 0;
        char countbuf[32]= {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_CHECKRELATION_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+95);
        }else{
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poCheckRelationData->exist()))
                 m_poCheckRelationData->create(num*1.3);  
		checkMem(m_poCheckRelationData->getCount() * sizeof(CheckRelation));  
        CheckRelation *pCheckRelation = (CheckRelation*)(*m_poCheckRelationData);
        //CheckSubRule *pCheckSubRule = (CheckSubRule*)(*m_poCheckSubRuleData);
        int count, i;
        TOCIQuery qry(Environment::getDBConn());
        qry.setSQL(     "select relation_id, rule_id, "
                "bill_type, record_type, check_seq, allow_null "
                "from ("
                "select a.relation_id, nvl(c.rule_id, -1) rule_id, "
                "nvl(b.trans_file_type_id, -1) bill_type, " 
                "nvl(a.record_type, -1) record_type, "
                "nvl(a.check_seq, -1) check_seq, "
                "nvl(a.allow_null,'N') allow_null "
                "from Check_Relation a, trans_file_type b, check_rule c "
                "where a.rule_id = c.rule_id(+) "
                "and   a.bill_type = b.trans_file_type_id(+) "
                "and   nvl(a.bFlag, 1) <>0 and nvl(a.bFlag,1)<>99) "
                "order by bill_type asc , record_type asc, check_seq asc " );
        qry.open();
        i = 0;
        int ioldBillType   = -2;
        int ioldRecordType = -2;
        //CheckRelation *poldSibBill   = NULL;
        //CheckRelation *poldSibRecord = NULL;
        //CheckRelation *poldRelation  = NULL;
        unsigned int ioldSibBill = 0;
        unsigned int ioldSibRecord = 0;
        unsigned int ioldRelation = 0;
        while (qry.next()){
                i = m_poCheckRelationData->malloc();            
                if ( i <= 0) THROW(MBC_ParamInfoMgr + 96);
                memset(&(pCheckRelation[i]), '\0', sizeof(class CheckRelation));
                //先判断RuleID是否满足外健完整性，若满足用m_poCheckRule指向此校验规则
                int iRelationID = qry.field(0).asInteger();
                int iRuleID     = qry.field(1).asInteger();
                int iBillType   = qry.field(2).asInteger();
                int iRecordType = qry.field(3).asInteger();
                //合法性检查
                if (!m_poCheckRuleIndex->get(iRuleID, &(pCheckRelation[i].m_iCheckRule)))
                {               
                        Log::log (0, "表Check_Relation中，校验关系(relation_id=%d), 没有找到与其对应的校验规则", 
                                         iRelationID);                          
                        continue;
                }else if (iBillType<=0){
                    Log::log (0, "表Check_Relation中，校验关系(relation_id=%d), bill_type值非法", 
                                         iRelationID);                          
                        continue;
                }else if (iRecordType<0){
                    Log::log (0, "表Check_Relation中，校验关系(relation_id=%d), record_type值非法", 
                                         iRelationID);                          
                        continue;
                }
                //第一次循环，执行初始化
                if (i == 0)
                {
                        ioldBillType   = iBillType;
                        ioldRecordType = iRecordType;
                        //poldSibBill    = &(pCheckRelation[i]);
                        //poldSibRecord  = &(pCheckRelation[i]);
                        ioldSibBill    = i;
                        ioldSibRecord  = i;
                }
                //如果此校验关系与前一个校验关系的BillType不相同
                else if (ioldBillType != iBillType){
                        pCheckRelation[ioldSibBill].m_iSibBillRelation = i;
                        ioldSibBill = i;
                        ioldSibRecord = i;
                }
                //如果此校验关系与前一个校验关系的BillType相同，但RecordType不相同
                else if (ioldBillType == iBillType && ioldRecordType != iRecordType){
                        pCheckRelation[ioldSibRecord].m_iSibRecordRelation = i;
                        //ioldSibRecord = pCheckRelation[ioldSibRecord].m_iSibRecordRelation;
                        ioldSibRecord = i;
                }
                //BillType, RecordType都相同
                else{ 
                        pCheckRelation[ioldRelation].m_iNextRelation = i;
                }
                pCheckRelation[i].m_iRelationID = iRelationID;
                pCheckRelation[i].m_iRuleID     = iRuleID;
                pCheckRelation[i].m_iBillType   = iBillType;
                pCheckRelation[i].m_iRecordType = iRecordType;
                pCheckRelation[i].m_iCheckSeq   = qry.field(4).asInteger();
                strcpy(pCheckRelation[i].m_sAllowNull,qry.field(5).asString());
                pCheckRelation[i].m_sAllowNull[1] = 0;          
                ioldBillType = iBillType;
                ioldRecordType = iRecordType; 
                //poldRelation = &(pCheckRelation[i]);
                ioldRelation = i;
        }
        qry.close();    
}

// TransFileMgr 
void ParamInfoMgr::loadTransFile()
{
        int num = 0;
        char countbuf[32]= {0};
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TRANSFILE_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+97);
        }else{
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poTransFileTypeData->exist()))
                 m_poTransFileTypeData->create(num*1.3);  
        if(!(m_poTransFileTypeIndex->exist()))
                 m_poTransFileTypeIndex->create(num*1.3); 
        TransFileType *pTransFileType = (TransFileType*)(*m_poTransFileTypeData);
		checkMem(m_poTransFileTypeData->getCount() * sizeof(TransFileType));  
    int count, i;
        TOCIQuery qry(Environment::getDBConn());
        //qry.setSQL(   "select a.trans_file_type_id type_id, "
    //            "nvl(a.trans_file_type_desc, '') type_name,  "
    //            "nvl(b.file_path, '') path, nvl(b.temp_file_path, '') tmp_path "
    //            "from trans_file_type a, trans_file_store_config b "
    //            "where a.trans_file_type_id = b.trans_file_type_id(+) "
    //            "order by type_id asc " );
       qry.setSQL(" SELECT filetypeid type_id,nvl(filerule,'') type_name, "
                        "        nvl(b.file_path,'') PATH,                      "
                        "        NVL(B.TEMP_FILE_PATH,'') TMP_PATH,             "
                        "        nvl(err_head,0)  err_head,nvl(err_tail,0) err_tail "
                        " FROM                                                  "
                        "      (SELECT filetypeid,filerule,err_head,err_tail FROM b_tap3file      "
                        "       UNION                                           "
                        "       SELECT filetypeid,filerule,err_head,err_tail FROM b_sepfile) a,   "
                        "       TRANS_FILE_STORE_CONFIG B                       "
                        "  WHERE a.filetypeid=b.trans_file_type_id(+)           "
                        "  ORDER BY type_id  ");
        qry.open();
        i = 0;
        //TransFileType *pOldFileType = NULL;
        unsigned int iOldFileType = 0;
        while (qry.next()){
                i = m_poTransFileTypeData->malloc();
                if (i <= 0) THROW(MBC_ParamInfoMgr + 98);
                memset(&(pTransFileType[i]), '\0', sizeof(class TransFileType));
                pTransFileType[i].m_iTypeID  = qry.field(0).asInteger();
                strcpy(pTransFileType[i].m_sTypeName, qry.field(1).asString());
                strcpy(pTransFileType[i].m_sPath,     qry.field(2).asString());
                strcpy(pTransFileType[i].m_sTmpPath,  qry.field(3).asString());
                pTransFileType[i].m_iErrHead = qry.field(4).asInteger();
                pTransFileType[i].m_iErrTail = qry.field(5).asInteger();
                m_poTransFileTypeIndex->add(pTransFileType[i].m_iTypeID, i);    
        }
        qry.close();
}

bool ParamInfoMgr::getBelongGroupB(int acctitem, int bacctitemgroup)
{
    unsigned int iIdx = 0;
    ItemGroupMember *pItemMember = 0;
    ItemGroupMember *pHead = (ItemGroupMember *)(*m_poAcctItemGroupBData);
    if(!(m_poAcctItemGroupBIndex->get(bacctitemgroup, &iIdx)))
    {
        return false;
    }
    else
    {
        pItemMember = pHead + iIdx;
        while(1)
        {
            if(pItemMember->m_iItemTypeID == acctitem)
                return true;
            else if(pItemMember->m_iNextOffset != 0)
                pItemMember = pHead + pItemMember->m_iNextOffset;
            else
                return false;
        }
    }
}

int ParamInfoMgr::CheckFilePath (char * filepath)
{
    ///---apin 200611 检查目录是否存在
    //如果不存在则建立,
    DIR * dp;

    if ((dp=opendir(filepath)) == NULL )
    {
        //不能打开目录,试图创建该目录
        if( mkdir(filepath,0770)!=0 )
        {
            Log::log(0,"查重目录(%s)创建失败!",filepath);
            THROW(MBC_APP_DUPCHECKER+14);    
        }
    }

    if(dp) 
        closedir(dp);
    
    return 1;
}


bool ParamInfoMgr::IsCrossed (const int iDupType)
{
    static KeyList<int> *pKeyList = 0;
    static bool bInited = false;
    static int iRet;
    
    if (!bInited) 
    {
        
        DEFINE_QUERY (qry);
        
        qry.setSQL ("select DISTINCT nvl(DUP_TYPE,0) from B_DUP_TYPE_RULE "
            " where dup_field_id in (:vA,:vB) "
            "   and his_dup_field_id in (:vC,:vD) "
            "   and condition_type <> '==' "
        );
        qry.setParameter ("vA", 2);  //开始时间
        qry.setParameter ("vB", 7);  //结束时间
        qry.setParameter ("vC", 2);
        qry.setParameter ("vD", 7);
        
        qry.open ();
        
        while (qry.next ()) {
            if (!pKeyList) {
                pKeyList = new KeyList<int> (qry.field(0).asLong(), 0);
                continue;
            }
            pKeyList->add (qry.field(0).asLong(), 0);
        }
        
        qry.close();
        
        bInited = true;
    }
    
    if (!pKeyList) return false;
    
    return pKeyList->get (iDupType, &iRet);
}


void ParamInfoMgr::getState()
{
/*        
        #ifdef MONITOR_FLOW_CONTROL
        MonitorIpc mIpc;
        #endif
*/
        char *pUserName;
        char sHostName[200];
        char sSql[512];
        char sSql1[1024];
        
        char sRemark[128];
        long m_iSHMID;
        int  m_iType = 0;
        
        long ltotal =0;
        long lused = 0;
        
        float per = 0;
        
        int m_bDelete = 0;



        char sTemp[256]={0};
            char m_sType[24];
        char sLine[104]={'-'};
        TOCIQuery qry (Environment::getDBConn ());
        TOCIQuery qry1 (Environment::getDBConn ());
        pUserName = getenv("LOGNAME");
        int iHostID = IpcKeyMgr::getHostInfo(sHostName);
        
        if (!strlen(pUserName) || iHostID==-1) {
            Log::log (0, "获取不到user或host_id, HOST_NAME:%s", sHostName);
            THROW (MBC_IpcKey + 1);
        }
/*
        if (ParamDefineMgr::getParam("CHECKIPC","CHECKIPC_IFDELETE",sTemp,sizeof(sTemp)-1) )
           m_bDelete = atoi(sTemp);


        
        if (m_bDelete == 1) {
*/              
        /*        qry.close();
          sprintf(sSql," delete from B_SHM_STAT_LOG where sys_username = '%s' and host_id = %d",pUserName,iHostID);
          qry.setSQL(sSql);
          qry.execute();
          qry.commit();
          qry.close();*/
//      }
	qry.close();
          sprintf(sSql," delete from B_SHM_STAT_LOG where sys_username = '%s' and host_id = %d",pUserName,iHostID);
          qry.setSQL(sSql);
          qry.execute();
          qry.commit();
//      }
	qry.close();
    memset(sSql, 0, sizeof(sSql));
    sprintf(sSql," select a.ipc_key,a.ipc_name,a.ipc_type,c.Ipc_Type_name "
            " from b_ipc_cfg a,b_ipc_type c "
            " where a.sys_username = '%s' "
            " and a.host_id = %d "
            " and a.ipc_type=c.ipc_type_id and a.ipc_type IS NOT NULL"
            " order by a.ipc_name"
            ,pUserName,iHostID);
try{
        qry.close();
        qry.setSQL(sSql);
        qry.open();
printf("%-44s%-24s%-12s%-10s%-10s%-10s%-10s\n","ipc_name","ipc_type","ipc_key","used","total","percent","user");
printf("%s\n",sLine);
        while(qry.next()){
           m_iSHMID = qry.field(0).asLong();
           strcpy(sRemark,qry.field(1).asString());
           m_iType = qry.field(2).asLong();
           strcpy(m_sType,qry.field(3).asString());
           ltotal =-1 ; 
           lused =0 ;
           bool bExist=false;
           switch( m_iType)
           {
              case 1:
                {
                 SHMData<int> p1(m_iSHMID);
                 if( p1.exist() )
                 {
                   ltotal = p1.getTotal();
                   lused  = p1.getCount();
                   bExist=true;
                 }
               }
                break;
                       
              case 2:
                {
                 SHMIntHashIndex p2(m_iSHMID);
                 if( p2.exist() )
                 {
                     ltotal = p2.getTotal();
                     lused  = p2.getCount();                    
                   bExist=true;
                 }
               }
                      break; 
              
              case 3:
                {
                 SHMStringTreeIndex p3(m_iSHMID);
                 if( p3.exist() )
                 {
                     ltotal  = p3.getTotal();
                     lused   = p3.getCount();
                   bExist=true;
                 }
               }
                      break;
              case 4:
                {
                 SHMData_A<int> p4(m_iSHMID);
                 if( p4.exist() )
                 {
                   ltotal = p4.getTotal();
                   lused  = p4.getCount();
                   bExist=true;
                 }
               }
                break;
                       
              case 5:
                {
                 SHMIntHashIndex_A p5(m_iSHMID);
                 if( p5.exist() )
                 {
                     ltotal = p5.getTotal();
                     lused  = p5.getCount();                    
                   bExist=true;
                 }
               }
                      break; 
              
              case 6:
                {
                 SHMStringTreeIndex_A p6(m_iSHMID);
                 if( p6.exist() )
                 {
                     ltotal  = p6.getTotal();
                     lused   = p6.getCount();
                   bExist=true;
                 }
               }
                      break;        
              case 8:
                {
                 SHMIntHashIndex_AX p8(m_iSHMID);
                 if( p8.exist() )
                 {
                     ltotal = p8.getTotal();
                     lused  = p8.getCount();                    
                   bExist=true;
                 }
               }
                      break; 
              
              case 9:
                {
                 SHMStringTreeIndex_AX p9(m_iSHMID);
                 if( p9.exist() )
                 {
                     ltotal  = p9.getTotal();
                     lused   = p9.getCount();
                   bExist=true;
                 }
               }
                      break;            
              case 10:
                {
                 SHMData_B<int> p10(m_iSHMID);
                 if( p10.exist() )
                 {
                   ltotal = p10.getTotal();
                   lused  = p10.getCount();
                   bExist=true;
                 }
               }
                break;
                       
              case 11:
                {
                 SHMIntHashIndex_KR p11(m_iSHMID);
                 if( p11.exist() )
                 {
                     ltotal = p11.getTotal();
                     lused  = p11.getCount();                    
                   bExist=true;
                 }
               }
                      break;                            
              default:
              break;
             }
            

            if ( ltotal !=0 )
               per = (float)lused*100/ltotal;
            else
               per=0;   

            printf("%-44s%-24s%-12ld%-10d%-10d%-10.2f%-10s\n",sRemark,m_sType,m_iSHMID, lused,ltotal,per,pUserName);
        
LogSHMMEMInfo(sRemark,m_sType,m_iSHMID, lused,ltotal);

        }
        printf("%s\n",sLine);

}catch(TOCIException &e){
        printf("%s,%s\n",e.getErrMsg(),e.getErrSrc());

}

        
}


template <class dataT >
void ParamInfoMgr::printRecordInfo( dataT * pClass, char const * sClass,char const * sType)
{
                float j=0;
                int iTotal=-1;
                int iUsed=0;
                if ( pClass->exist() ){
                        j=100*(float)(pClass->getCount())/pClass->getTotal();
        printf("%-40s%-10s%-10d%-10d%-10.2f\n", sClass, sType,
        pClass->getCount(), pClass->getTotal(),j);
    }   
    //LogSHMMEMInfo(sClass,sType,iUsed,iTotal);
}       

void ParamInfoMgr::LogSHMMEMInfo(char* pMemCfgName, char* pMemCfgMode, long lSHMID, int nUsed, int nTotal)
{
        char sSql[1024]={0};
        char sTmp[32] = {0};
        char *pUserName = 0;
        char sHostName[200] = {0};

        pUserName = getenv("LOGNAME");
        int iHostID = IpcKeyMgr::getHostInfo(sHostName);
        if (!strlen(pUserName) || iHostID==-1) 
        {
                Log::log (0, "获取不到user或host_id, HOST_NAME:%s", sHostName);
                THROW (MBC_IpcKey + 1);
        }
    
        double iTmpD = (double)nUsed/(double)nTotal;
        sprintf(sTmp,"%.2lf",iTmpD*100);
        iTmpD = atof(sTmp);
        TOCIQuery qry (Environment::getDBConn ());
        qry.close();

        sprintf(sSql,"DELETE FROM B_SHM_STAT_LOG WHERE MEM_CFG_NAME = '%s' AND MEM_CFG_MODE = '%s' AND HOST_ID = %d AND SYS_USERNAME = '%s'", 
                                                                pMemCfgName, pMemCfgMode, iHostID, pUserName);
        qry.setSQL(sSql);
        qry.execute();
        qry.close();
        memset(sSql,'\0',1024);
        sprintf(sSql," INSERT INTO B_SHM_STAT_LOG (MEM_CFG_NAME, MEM_CFG_MODE, ipc_key, MEM_USED, "
                                                                " MEM_TOTAL, PERCENT, HOST_ID, SYS_USERNAME, create_date) "
                                                                " VALUES(:MEM_CFG_NAME, :MEM_CFG_MODE,:shmid, :MEM_USED, :MEM_TOTAL, :USED_RATE, :HOST_ID, :SYS_USERNAME,SYSDATE)");
        qry.setSQL(sSql);
        qry.setParameter("MEM_CFG_NAME", pMemCfgName);
        qry.setParameter("MEM_CFG_MODE", pMemCfgMode);
        qry.setParameter("shmid", lSHMID);
        qry.setParameter("MEM_USED", nUsed);
        qry.setParameter("MEM_TOTAL", nTotal);
        qry.setParameter("USED_RATE", iTmpD);
        qry.setParameter("HOST_ID", iHostID);
        qry.setParameter("SYS_USERNAME", pUserName);
        qry.execute();
        qry.commit();
        qry.close();

}

//prep
void ParamInfoMgr::loadTemplateMapSwitch()
{
        //对这个结构体信息的赋值,在跟原来记录比较后可能会有改动
        char countbuf[32]= {0};
        int num = 0;
        TemplateMapSwitch *pList = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TEMPLATEMAPSWITCH_NUM, countbuf))
        {
                        THROW(MBC_ParamInfoMgr+300);
        }else{
                        num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poTemplateMapSwitchData->exist()))
                m_poTemplateMapSwitchData->create(CAL_COUNT_E(num));
        //<map_id, offset>
        if(!(m_poTemplateMapSwitchIndex->exist()))
                m_poTemplateMapSwitchIndex->create(CAL_COUNT_E(num));
        
        pList = (TemplateMapSwitch*)(*m_poTemplateMapSwitchData);
        checkMem(m_poTemplateMapSwitchData->getCount() * sizeof(TemplateMapSwitch));  
        //加载数据索引数据
        DEFINE_QUERY (qry) ;

        //获得SQL语句
        char m_sSql[1024] = {0};
        strcpy(m_sSql,"select map_id,switch_type_id,template_id,nvl(org_id,-1) from b_template_map_switch order by template_id");

        qry.setSQL (m_sSql);
        qry.open ();
        //mStdDesc.iUsedPos = 0;
        TemplateMapSwitch *pTmp = 0;
        TemplateMapSwitch *p = 0;
        while(qry.next())
        {
                unsigned int i = m_poTemplateMapSwitchData->malloc();
                if(i == 0)
                        THROW(MBC_ParamInfoMgr+301);            
                p = pList + i;  
                p->m_iMapID = qry.field(0).asInteger();
                p->m_iSwitchTypeID = qry.field(1).asInteger();
                p->m_iTemplateID = qry.field(2).asInteger();
                p->m_iOrgID = qry.field(3).asInteger();
                p->m_iNext = 0;
                if(pTmp)
                {
                        if(p->m_iTemplateID != pTmp->m_iTemplateID)
                        {
                                m_poTemplateMapSwitchIndex->add(p->m_iTemplateID,i);
                        } else {
                                pTmp->m_iNext = i;
                        }
                } else {
                        m_poTemplateMapSwitchIndex->add(p->m_iTemplateID,i);
                }
                pTmp = p;
        }
        qry.close();
        Log::log(0,"加载的TemplateMapSwitch数据区使用量/总数[%d/%d]; 索引区使用量/总数[%d/%d]",m_poTemplateMapSwitchData->getCount(),m_poTemplateMapSwitchData->getTotal(),m_poTemplateMapSwitchIndex->getCount(),m_poTemplateMapSwitchIndex->getTotal());      
}

void ParamInfoMgr::loadObjectDefine()
{
        char countbuf[32]= {0};
        int num = 0;
        ObjectDefine *pList = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_OBJECTDEFINE_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+302);
        }else{
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poObjectDefineData->exist()))
                m_poObjectDefineData->create(CAL_COUNT_E(num));
        //<map_id, offset>
        if(!(m_poObjectDefineIndex->exist()))
                m_poObjectDefineIndex->create(CAL_COUNT_E(num));
        
        pList = (ObjectDefine*)(*m_poObjectDefineData);
        checkMem(m_poObjectDefineData->getCount() * sizeof(ObjectDefine));  
        //加载数据索引数据
        DEFINE_QUERY (qry) ;

        //获得SQL语句
        char m_sSql[1024] = {0};
        strcpy(m_sSql,"select object_id,map_id,object_type,calc_method_id,param1,param2,param3,param4,param5 from b_object_define order by map_id");
        
        qry.setSQL (m_sSql);
        qry.open ();
        ObjectDefine *pTmp = 0;
        ObjectDefine *p = 0;
        while(qry.next())
        {       
                unsigned int i = m_poObjectDefineData->malloc();
                if(i == 0)
                        THROW(MBC_ParamInfoMgr+303);
                p = pList + i;
                p->m_iObjectID = qry.field(0).asInteger();
                p->m_iMapID = qry.field(1).asInteger();
                p->m_iObjectType = (OBJECT_TYPE)qry.field(2).asInteger();
                p->m_iCalcMethodID = (CALC_BLOCK_METHOD)qry.field(3).asInteger();
                strcpy(p->m_sParam1,qry.field(4).asString());
                strcpy(p->m_sParam2,qry.field(5).asString());
                strcpy(p->m_sParam3,qry.field(6).asString());
                strcpy(p->m_sParam4,qry.field(7).asString());
                strcpy(p->m_sParam5,qry.field(8).asString());
                p->m_iNext = 0;
                if(pTmp)
                {
                        if(pTmp->m_iMapID != p->m_iMapID)
                        {
                                m_poObjectDefineIndex->add(p->m_iMapID,i);
                        } else {
                                pTmp->m_iNext = i;
                        }
                } else {
                        m_poObjectDefineIndex->add(p->m_iMapID,i);
                }
                pTmp = p;
        }
        qry.close();
        Log::log(0,"加载的ObjectDefine数据区使用量/总数[%d/%d]; 索引区使用量/总数[%d/%d]",m_poObjectDefineData->getCount(),m_poObjectDefineData->getTotal(),m_poObjectDefineIndex->getCount(),m_poObjectDefineIndex->getTotal());       
}

void ParamInfoMgr::loadEventParserGroup()
{
        char countbuf[32]= {0};
        int num = 0;
        EventParserGroup *pList = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_EVENTPARSERGROUP_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+304);
        }else{
                num = atoi(countbuf);
        }
        memset(countbuf,'\0',sizeof(countbuf));
        if(!(m_poEventParserGroupData->exist()))
                m_poEventParserGroupData->create(CAL_COUNT_E(num));
        //<map_id, offset>
        if(!(m_poEventParserGroupIndex->exist()))
                m_poEventParserGroupIndex->create(CAL_COUNT_E(num));
                
        /*if(!(m_poEPGGroupData->exist()))
                m_poEPGGroupData->create(CAL_COUNT_E(num));
        //<obj_id, offset>
        if(!(m_poEPGGroupIndex->exist()))
                m_poEPGGroupIndex->create(CAL_COUNT_E(num));*/
        pList = (EventParserGroup*)(*m_poEventParserGroupData);
        checkMem(m_poEventParserGroupData->getCount() * sizeof(EventParserGroup)); 
        //加载数据索引数据
        DEFINE_QUERY (qry) ;
        //获得SQL语句
        char m_sSql[1024] = {0};
        strcpy(m_sSql,"select object_type,group_id,map_id from b_map_parser_group order by map_id,group_id");   
        qry.setSQL (m_sSql);
        qry.open ();
        EventParserGroup *pTmp = 0;
        EventParserGroup *p = 0;
        while(qry.next())
        {       
                unsigned int i = m_poEventParserGroupData->malloc();
                if(i == 0)
                        THROW(MBC_ParamInfoMgr+305);
                p = pList + i;
                p->m_iObjectType = qry.field(0).asInteger();
                p->m_iGroupID = qry.field(1).asInteger();
                p->m_iMapID = qry.field(2).asInteger();         
                p->m_iNext = 0;
                //p->m_iGroupNext = 0;
                if(pTmp)
                {
                        if(pTmp->m_iMapID != p->m_iMapID)
                        {
                                m_poEventParserGroupIndex->add(p->m_iMapID,i);
                        } else {
                                pTmp->m_iNext = i;
                        }
                } else {
                        m_poEventParserGroupIndex->add(p->m_iMapID,i);
                }
                pTmp = p;
        }
        qry.close();
        
        
        Log::log(0,"加载的EventParserGroup[MAP_ID为主键]数据区使用量/总数[%d/%d]; 索引区使用量/总数[%d/%d]",m_poEventParserGroupData->getCount(),m_poEventParserGroupData->getTotal(),m_poEventParserGroupIndex->getCount(),m_poEventParserGroupIndex->getTotal());      
}

void ParamInfoMgr::loadField()
{
        char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FIELD_SHM_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+306);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poFieldData->exist())
                m_poFieldData->create(CAL_COUNT_E(num));
        if(!m_poFieldTemplateIndex->exist())
                m_poFieldTemplateIndex->create(CAL_COUNT_E(num));
        if(!m_poFieldUnionStrIndex->exist())
                m_poFieldUnionStrIndex->create(CAL_COUNT_E(num),18);
        checkMem(m_poFieldData->getCount() * sizeof(Field)); 
        countbuf[0]= '\0';
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TEMPLATE_RELATION_SHM_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+307);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poTemplateRelationData->exist())
                m_poTemplateRelationData->create(CAL_COUNT_E(num));
        if(!m_poTemplateRelationIndex->exist())
                m_poTemplateRelationIndex->create(CAL_COUNT_E(num));
        checkMem(m_poTemplateRelationData->getCount() * sizeof(TemplateRelation)); 
        TemplateRelation *pTRelation = (TemplateRelation*)(*m_poTemplateRelationData);
        char m_sSql[1024] = {0};
        DEFINE_QUERY (qry);
        //首先获得模板标识
        strcpy(m_sSql,"select b.template_id,a.group_id from b_map_parser_group a,b_template_map_switch b where a.map_id = b.map_id order by a.group_id");
        qry.setSQL(m_sSql);
        qry.open();
        while(qry.next())
        {
                unsigned int i = m_poTemplateRelationData->malloc();
                if(i == 0)
                        THROW(MBC_ParamInfoMgr+308);
                memset(&pTRelation[i],'\0',sizeof(TemplateRelation));
                pTRelation[i].m_iTemplateID = qry.field(0).asInteger();
                pTRelation[i].m_iGroupID = qry.field(1).asInteger();
                pTRelation[i].m_iNext = 0;
                if(i>1)
                {
                        if(pTRelation[i].m_iGroupID == pTRelation[i-1].m_iGroupID)
                        {
                                pTRelation[i-1].m_iNext = i;
                        } else {
                                m_poTemplateRelationIndex->add(pTRelation[i].m_iGroupID,i);
                        }
                } else {
                        m_poTemplateRelationIndex->add(pTRelation[i].m_iGroupID,i);
                }
        }
        qry.close();
        //1,8
        Field * m_opField = 0;
        Field * m_opLast = 0;
        Field *pFieldList = (Field*)(*m_poFieldData);
        if(!pFieldList)
        {
                Log::log(0,"pFieldList指针为空");
                THROW(MBC_ParamInfoMgr+309);
        }
        //从B_TOKEN_FIELD这张表获得数据
        strcpy(m_sSql,"select event_attr_id,field_seq,remark,nvl(data_format_id,0),reserve,group_id from b_sep_field order by group_id");
        qry.setSQL(m_sSql);
        qry.open();
        int iFieldFlag = 0;
        int iGroupID= 0;
        int iLastGroupID= 0;
        while(qry.next())
        {
                unsigned int i = m_poFieldData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+309);
                m_opField = pFieldList + i;
                memset(m_opField,'\0',sizeof(Field));
                if(iFieldFlag == 0)
                {
                        m_poFieldTemplateIndex->add(1,i);
                        m_poFieldTemplateIndex->add(8,i);
                }
                memset(m_opField,'\0',sizeof(struct Field));
                m_opField->m_iEventAttrID = qry.field(0).asInteger();
                m_opField->m_iFieldSeq = qry.field(1).asInteger();
                strcpy(m_opField->m_sFieldName,qry.field(2).asString());
                m_opField->m_iDataFormatID = qry.field(3).asInteger();
                strcpy(m_opField->m_sBitFlag , qry.field(4).asString());
                iGroupID = qry.field(5).asInteger();
                m_opField->m_iGroupID = iGroupID;
                m_opField->m_iNext = 0;
                char sKey1[18] = {0};
                char sKey2[18] = {0};
                sprintf(sKey1,"%d_%d_%d",1,0,iGroupID);
                sprintf(sKey2,"%d_%d_%d",8,0,iGroupID);
                /*if(m_opLast)
                {
                        if(iLastGroupID == iGroupID)
                                m_opLast->m_iNext = i;
                }*/
                unsigned int iIdxTmp = 0;
                if(m_poFieldUnionStrIndex->get(sKey1,&iIdxTmp))//只要看一个即可
                {
                    m_opField->m_iNext = iIdxTmp;
                    m_poFieldUnionStrIndex->add(sKey1,i);
                        m_poFieldUnionStrIndex->add(sKey2,i);
                } else {
                    m_poFieldUnionStrIndex->add(sKey1,i);
                        m_poFieldUnionStrIndex->add(sKey2,i);
                }
                iLastGroupID = iGroupID;
                m_opLast = m_opField;
                iFieldFlag++;
        }
        qry.close();
        
        //2
        //从B_ORDER_FIX_FIELD这张表获得数据
        iFieldFlag = 0;
        m_opLast = 0;
        iGroupID= 0;
        iLastGroupID= 0;
        m_sSql[0] = '\0';
        strcpy(m_sSql,"select event_attr_id,nvl(data_format_id,0),field_seq,attr_begin_byte,attr_begin_bit,attr_length,code,alignment,nvl(fill_char,-1),reserve,group_id from b_order_fix_field order by group_id");
        qry.setSQL(m_sSql);
        qry.open();     
        while(qry.next())
        {
                unsigned int i = m_poFieldData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+309);
                m_opField = pFieldList + i;
                memset(m_opField,'\0',sizeof(Field));
                if(iFieldFlag == 0)
                        m_poFieldTemplateIndex->add(2,i);
                m_opField->m_iEventAttrID = qry.field(0).asInteger();
                m_opField->m_iDataFormatID = qry.field(1).asInteger();
                m_opField->m_iFieldSeq = qry.field(2).asInteger();
                m_opField->m_iAttrBeginByte = qry.field(3).asInteger();
                m_opField->m_iAttrBeginBit = qry.field(4).asInteger();
                m_opField->m_iAttrLength = qry.field(5).asInteger();
                strcpy(m_opField->m_sCode,qry.field(6).asString());
                strcpy(m_opField->m_sAlignment,qry.field(7).asString());
                m_opField->m_iFillChar = atoi(qry.field(8).asString());
                strcpy(m_opField->m_sBitFlag , qry.field(9).asString());
                iGroupID = qry.field(10).asInteger();
                m_opField->m_iGroupID = iGroupID;
                m_opField->m_iNext = 0;
                char sKey[18] = {0};
                sprintf(sKey,"%d_%d_%d",2,0,iGroupID);
                unsigned int iIdxTmp = 0;
                if(m_poFieldUnionStrIndex->get(sKey,&iIdxTmp))
                {
                    m_opField->m_iNext = iIdxTmp;
                    m_poFieldUnionStrIndex->add(sKey,i);
                } else {
                    m_poFieldUnionStrIndex->add(sKey,i);
                }
                /*if(m_opLast)
                {
                        if(iLastGroupID == iGroupID)
                        {
                                m_opLast->m_iNext = i;
                        }
                }*/
                iLastGroupID = iGroupID;
                m_opLast = m_opField;
                iFieldFlag++;
        }
        qry.close ();
        
        //3
        //从B_ORDER_FIX_FIELD这张表获得数据
        m_opLast = 0;
        iGroupID= 0;
        iLastGroupID= 0;
        iFieldFlag = 0;
        m_sSql[0] = '\0';
        strcpy(m_sSql,"select event_attr_id,nvl(data_format_id,0),Tag,attr_begin_bit,attr_length,code,alignment,nvl(fill_char,-1),reserve,remark,group_id from b_disorder_fix_field order by group_id");
        qry.setSQL (m_sSql);
        qry.open();
        while(qry.next ())
        {
                        unsigned int i = m_poFieldData->malloc();
                        if(i == 0 )
                                THROW(MBC_ParamInfoMgr+309);
                        m_opField = pFieldList + i;
                        memset(m_opField,'\0',sizeof(Field));
                        if(iFieldFlag == 0)
                                m_poFieldTemplateIndex->add(3,i);
                        char m_sTemp[10] = {0};
                        m_opField->m_iEventAttrID = qry.field(0).asInteger();
                        m_opField->m_iDataFormatID = qry.field(1).asInteger();

                        strcpy(m_sTemp,qry.field(2).asString());
                        m_opField->m_iTag = atoi(m_sTemp);
                        //m_opField->m_iTag = qry.field(2).asInteger();
                        m_opField->m_iAttrBeginBit = qry.field(3).asInteger();
                        m_opField->m_iAttrLength = qry.field(4).asInteger();
                        strcpy(m_opField->m_sCode,qry.field(5).asString());
                        strcpy(m_opField->m_sAlignment,qry.field(6).asString());
                        m_opField->m_iFillChar = atoi(qry.field(7).asString());
                        strcpy(m_opField->m_sBitFlag,qry.field(8).asString());
                        strcpy(m_opField->m_sFieldName,qry.field(9).asString());
                        iGroupID = qry.field(10).asInteger();
                        m_opField->m_iGroupID = iGroupID;
                        m_opField->m_iNext = 0;
                        char sKey[18] = {0};
                        sprintf(sKey,"%d_%d_%d",3,0,iGroupID);
                        unsigned int iIdxTmp = 0;
                        if(m_poFieldUnionStrIndex->get(sKey,&iIdxTmp))
                        {
                        m_opField->m_iNext = iIdxTmp;
                        m_poFieldUnionStrIndex->add(sKey,i);
                        } else {
                        m_poFieldUnionStrIndex->add(sKey,i);
                        }
                        /*if(m_opLast)
                        {
                                if(iLastGroupID == iGroupID)
                                {
                                        m_opLast->m_iNext = i;
                                }
                        }*/
                        iLastGroupID = iGroupID;
                        m_opLast = m_opField;
                        iFieldFlag++;
        }
        qry.close ();
        
        //5
        //从B_DISORDER_FIX_FIELD这张表获得数据
        m_opLast = 0;
        iGroupID= 0;
        iLastGroupID= 0;
        iFieldFlag = 0;
        m_sSql[0] = '\0';
        strcpy(m_sSql,"select event_attr_id,nvl(data_format_id,0),Tag,attr_begin_bit,attr_length,code,alignment,fill_char,reserve,remark,group_id from b_disorder_fix_field order by group_id");        
        qry.setSQL(m_sSql);
        qry.open();
        while(qry.next())
        {
                        unsigned int i = m_poFieldData->malloc();
                        if(i == 0 )
                                THROW(MBC_ParamInfoMgr+309);
                        m_opField = pFieldList + i;
                        memset(m_opField,'\0',sizeof(Field));
                        if(iFieldFlag == 0)
                                m_poFieldTemplateIndex->add(5,i);
                        char m_sTemp[10] = {0};
                        m_opField->m_iEventAttrID = qry.field(0).asInteger();
                        m_opField->m_iDataFormatID = qry.field(1).asInteger();
                        strcpy(m_sTemp,qry.field(2).asString());
                        m_opField->m_iTag = atoi(m_sTemp);
                        m_opField->m_iAttrBeginBit = qry.field(3).asInteger();
                        m_opField->m_iAttrLength = qry.field(4).asInteger();
                        strcpy(m_opField->m_sCode,qry.field(5).asString());
                        strcpy(m_opField->m_sAlignment,qry.field(6).asString());
                        m_opField->m_iFillChar = atoi(qry.field(7).asString());
                        strcpy(m_opField->m_sBitFlag,qry.field(8).asString());
                        strcpy(m_opField->m_sFieldName,qry.field(9).asString());
                        iGroupID = qry.field(10).asInteger();
                        m_opField->m_iGroupID = iGroupID;
                        m_opField->m_iNext = 0;
                        char sKey[18] = {0};
                        sprintf(sKey,"%d_%d_%d",5,0,iGroupID);
                        unsigned int iIdxTmp = 0;
                        if(m_poFieldUnionStrIndex->get(sKey,&iIdxTmp))
                        {
                        m_opField->m_iNext = iIdxTmp;
                        m_poFieldUnionStrIndex->add(sKey,i);
                        } else {
                        m_poFieldUnionStrIndex->add(sKey,i);
                        }
                        /*if(m_opLast)
                        {
                                if(iLastGroupID == iGroupID)
                                {
                                        m_opLast->m_iNext = i;
                                }
                        }*/
                        iLastGroupID = iGroupID;
                        m_opLast = m_opField;
                        iFieldFlag++;
                        
        }
        qry.close();
        
        //5[2]
        //从B_DISORDER_ASN_FIELD这张表获得数据
        m_opLast = 0;
        iGroupID= 0;
        iLastGroupID= 0;
        iFieldFlag = 0;
        m_sSql[0] = '\0';
        strcpy(m_sSql,"select event_attr_id,nvl(data_format_id,0),tag,length_field_begin_pos,length_field_begin_bits,code,reserve,remark,length_code,length_unit_type,group_id from b_disorder_asn_field order by group_id");
        qry.setSQL(m_sSql);
        qry.open();
        while(qry.next())
        {               
                unsigned int i = m_poFieldData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+309);
                m_opField = pFieldList + i;
                memset(m_opField,'\0',sizeof(Field));
                char m_sTemp[10] = {0};
                m_opField->m_iEventAttrID = qry.field(0).asInteger();
                m_opField->m_iDataFormatID = qry.field(1).asInteger();
                strcpy(m_sTemp,qry.field(2).asString());
                m_opField->m_iTag = atoi(m_sTemp);
                m_opField->m_iLengthBeginPos = qry.field(3).asInteger();
                m_opField->m_iLengthBeginBits = qry.field(4).asInteger();
                strcpy(m_opField->m_sCode,qry.field(5).asString());
                strcpy(m_opField->m_sBitFlag,qry.field(6).asString());
                strcpy(m_opField->m_sFieldName,qry.field(7).asString());
                strcpy(m_opField->m_sLengthCode,qry.field(8).asString());
                strcpy(m_opField->m_sLengthUnitType,qry.field(9).asString());
                iGroupID = qry.field(10).asInteger();           
                m_opField->m_iGroupID = iGroupID;               
                m_opField->m_iNext = 0;
                char sKey[18] = {0};
                sprintf(sKey,"%d_%d_%d",5,1,iGroupID);
                unsigned int iIdxTmp = 0;
                if(m_poFieldUnionStrIndex->get(sKey,&iIdxTmp))
                {
                    m_opField->m_iNext = iIdxTmp;
                    m_poFieldUnionStrIndex->add(sKey,i);
                } else {
                    m_poFieldUnionStrIndex->add(sKey,i);
                }
                /*if(m_opLast)
                {
                        if(iLastGroupID == iGroupID)
                        {
                                m_opLast->m_iNext = i;
                        }
                }*/
                iLastGroupID = iGroupID;
                m_opLast = m_opField;
                iFieldFlag++;
        }
        qry.close();
        
        //TemplateID==6
        //从B_ASN1_FIELD这张表获得数据
        m_opLast = 0;
        iGroupID= 0;
        iLastGroupID= 0;
        iFieldFlag = 0;
        m_sSql[0] = '\0';
        strcpy(m_sSql,"select event_attr_id,nvl(data_format_id,0),Tag,code,reserve,remark,parent_event_attr_id,nvl(fill_char,-1),group_id from b_asn1_field order by group_id");
        qry.setSQL (m_sSql);
        qry.open ();
        while(qry.next ())
        {
                unsigned int i = m_poFieldData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+309);
                if(iFieldFlag == 0)
                                m_poFieldTemplateIndex->add(6,i);
                m_opField = pFieldList + i;
                memset(m_opField,'\0',sizeof(Field));
                char m_sTemp[10] = {0};
                m_opField->m_iEventAttrID = qry.field(0).asInteger();
                m_opField->m_iDataFormatID = qry.field(1).asInteger();
                strcpy(m_sTemp,qry.field(2).asString());
                m_opField->m_iTag = atoi(m_sTemp);
                strcpy(m_opField->m_sCode,qry.field(3).asString());
                strcpy(m_opField->m_sBitFlag,qry.field(4).asString());
                strcpy(m_opField->m_sFieldName,qry.field(5).asString());
                m_opField->m_iParentEventAttrID = qry.field(6).asInteger();
                m_opField->m_iFillChar = atoi(qry.field(7).asString());
                iGroupID = qry.field(8).asInteger();
                m_opField->m_iGroupID = iGroupID;
                m_opField->m_iNext = 0;
                char sKey[18] = {0};
                sprintf(sKey,"%d_%d_%d",6,0,iGroupID);
                unsigned int iIdxTmp = 0;
                if(m_poFieldUnionStrIndex->get(sKey,&iIdxTmp))
                {
                    m_opField->m_iNext = iIdxTmp;
                    m_poFieldUnionStrIndex->add(sKey,i);
                } else {
                    m_poFieldUnionStrIndex->add(sKey,i);
                }
                /*if(m_opLast)
                {
                        if(iLastGroupID == iGroupID)
                        {
                                m_opLast->m_iNext = i;
                        }
                }*/
                iLastGroupID = iGroupID;
                m_opLast = m_opField;
                iFieldFlag++;
        }
        qry.close ();
        
        //7
        // 从B_ADSL_FIELD表获得数据
        m_opLast = 0;
        iGroupID= 0;
        iLastGroupID= 0;
        iFieldFlag = 0;
        m_sSql[0] = '\0';
        strcpy(m_sSql, "select event_attr_id,nvl(data_format_id,0),identifier,reserve,remark,group_id from b_adsl_field order by group_id");
        qry.setSQL (m_sSql);
        qry.open();
        while(qry.next())
        {
                unsigned int i = m_poFieldData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+309);
                if(iFieldFlag == 0)
                        m_poFieldTemplateIndex->add(7,i);
                m_opField = pFieldList + i;
                memset(m_opField,'\0',sizeof(Field));
                m_opField->m_iEventAttrID = qry.field(0).asInteger();
                m_opField->m_iDataFormatID = qry.field(1).asInteger();
                strcpy(m_opField->m_sIdentifier, qry.field(2).asString());
                strcpy(m_opField->m_sReserve, qry.field(3).asString());
                strcpy(m_opField->m_sFieldName, qry.field(4).asString());
                iGroupID = qry.field(5).asInteger();
                m_opField->m_iGroupID = iGroupID;
                m_opField->m_iNext = 0;
                char sKey[18] = {0};
                sprintf(sKey,"%d_%d_%d",7,0,iGroupID);
                unsigned int iIdxTmp = 0;
                if(m_poFieldUnionStrIndex->get(sKey,&iIdxTmp))
                {
                    m_opField->m_iNext = iIdxTmp;
                    m_poFieldUnionStrIndex->add(sKey,i);
                } else {
                    m_poFieldUnionStrIndex->add(sKey,i);
                }
                /*if(m_opLast)
                {
                        if(iLastGroupID == iGroupID)
                        {
                                m_opLast->m_iNext = i;
                        }
                }*/
                iLastGroupID = iGroupID;
                m_opLast = m_opField;
                iFieldFlag++;
        }
        qry.close();
        Log::log(0,"加载的GroupId_TemplateID关系[加载的GroupId为主键]数据区使用量/总数[%d/%d];索引区使用量/总数[%d/%d]",m_poTemplateRelationData->getCount(),m_poTemplateRelationData->getTotal(),m_poTemplateRelationIndex->getCount(),m_poTemplateRelationIndex->getTotal());  
        Log::log(0,"加载的GroupId_Field关系[GroupId为主键]数据区使用量/总数[%d/%d]; 索引区使用量/总数[%d/%d]",m_poFieldData->getCount(),m_poFieldData->getTotal(),m_poFieldTemplateIndex->getCount(),m_poFieldTemplateIndex->getTotal());
        Log::log(0,"加载的GroupId_Field关系[混合主键]索引区使用量/总数[%d/%d]",m_poFieldUnionStrIndex->getlCount(),m_poFieldUnionStrIndex->getlTotal());
}

void ParamInfoMgr::loadSwitchTypeInfo()
{
        char countbuf[32]= {0};
        int num = 0;
        EventParserGroup *pList = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SWITCH_TYPE_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+310);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poMapSwitchTypeData->exist())
                m_poMapSwitchTypeData->create(CAL_COUNT_E(num));
        if(!m_poMapSwitchTypeIndex->exist())
                m_poMapSwitchTypeIndex->create(CAL_COUNT_E(num));
        checkMem(m_poMapSwitchTypeData->getCount() * sizeof(TemplateMapSwitch));
        //获得SQL语句
        TemplateMapSwitch *pMapSwitchType = (TemplateMapSwitch*)(*m_poMapSwitchTypeData);
        TemplateMapSwitch *pM = 0;
        TemplateMapSwitch *pMTmp = 0;
        char m_sSql[1024] = {0};
        DEFINE_QUERY (qry);
        strcpy(m_sSql,"select map_id,switch_type_id from b_template_map_switch order by switch_type_id");
        qry.setSQL (m_sSql);
        qry.open ();
        while(qry.next())
        {
                unsigned int i = m_poMapSwitchTypeData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+311);
                pM = pMapSwitchType+i;
                memset(pM,'\0',sizeof(TemplateMapSwitch));
                pM->m_iMapID = qry.field(0).asInteger();
                pM->m_iSwitchTypeID = qry.field(1).asInteger();
                if(pMTmp &&(pMTmp->m_iMapID == pM->m_iMapID))
                {
                        pMTmp->m_iNext = i;             
                } else {
                        m_poMapSwitchTypeIndex->add(pM->m_iMapID,i);
                }
                pMTmp = pM;
        }
        qry.close ();    
        Log::log(0,"加载的map_id,switch_type_id关系[MapID为主键]数据区使用量/总数[%d/%d];索引区使用量/总数[%d/%d]",m_poMapSwitchTypeData->getCount(),m_poMapSwitchTypeData->getTotal(),m_poMapSwitchTypeIndex->getCount(),m_poMapSwitchTypeIndex->getTotal());          
}


void ParamInfoMgr::loadEventAttr()
{
        char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_EVENT_ATTR_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+312);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poEventAttrData->exist())
                m_poEventAttrData->create(CAL_COUNT_E(num));
        if(!m_poEventAttrIndex->exist())
                m_poEventAttrIndex->create(CAL_COUNT_E(num));
		checkMem(m_poEventAttrData->getCount() * sizeof(EventAttr));
        EventAttr *pList = (EventAttr*)(*m_poEventAttrData);
        EventAttr *pTmp = 0;
        EventAttr *p = 0;
        DEFINE_QUERY (qry);
        char m_sSql[1024] = {0};
        strcpy(m_sSql,"select event_attr_type,data_type,length,precision,event_attr_id from event_attr order by event_attr_id");
        qry.setSQL (m_sSql);
        qry.open ();
        while(qry.next())
        {
                unsigned int i = m_poEventAttrData->malloc();
                if(i == 0)
                        THROW(MBC_ParamInfoMgr+313);
                p = pList + i;
                memset(p,'\0',sizeof(EventAttr));
                strcpy(p->m_sEventAttrType,qry.field(0).asString());
                strcpy(p->m_sDataType,qry.field(1).asString());
                p->m_iLength = qry.field(2).asInteger();
                p->m_iPrecision = qry.field(3).asInteger();
                p->m_iEventAttrID = qry.field(4).asInteger();
                p->m_iNext = 0;
                if(pTmp)
                {
                        if(pTmp->m_iEventAttrID == p->m_iEventAttrID)
                        {
                                pTmp->m_iNext = i;
                        } else {
                                m_poEventAttrIndex->add(p->m_iEventAttrID,i);
                        }
                } else {
                        m_poEventAttrIndex->add(p->m_iEventAttrID,i);
                }
                pTmp = p;
        }
        qry.close();
        Log::log(0,"加载的EventAttr关系[EventAttrID为主键]数据区使用量/总数[%d/%d];索引区使用量/总数[%d/%d]",m_poEventAttrData->getCount(),m_poEventAttrData->getTotal(),m_poEventAttrIndex->getCount(),m_poEventAttrIndex->getTotal());
}


void ParamInfoMgr::loadEventHeadAndColunmCfg()
{
        char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_EVENTHEAD_COLUMUN_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+314);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poEventHeadAndColumnsCfg->exist())
                m_poEventHeadAndColumnsCfg->create(CAL_COUNT_E(num));
        if(!m_poEventHeadAndColumnsIndex->exist())
                m_poEventHeadAndColumnsIndex->create(CAL_COUNT_E(num));
		checkMem(m_poEventHeadAndColumnsCfg->getCount() * sizeof(EventParserGroup));
        EventParserGroup *pList = (EventParserGroup*)(*m_poEventHeadAndColumnsCfg);
        EventParserGroup *pTmp = 0;
        EventParserGroup *p = 0;
        DEFINE_QUERY (qry);
        char m_sSql[1024] = {0};
        sprintf(m_sSql,"%s", " select map_id,group_id from b_map_parser_group order by group_id");
        qry.setSQL (m_sSql);
        qry.open ();
        while(qry.next())
        {
                unsigned int i = m_poEventHeadAndColumnsCfg->malloc();
                if(i == 0)
                        THROW(MBC_ParamInfoMgr+315);
                p = pList + i;
                memset(p,'\0',sizeof(EventParserGroup));                
                p->m_iMapID = qry.field(0).asInteger();         
                p->m_iGroupID = qry.field(1).asInteger();
                p->m_iNext = 0;         
                if(pTmp)
                {
                        if(pTmp->m_iGroupID == p->m_iGroupID)
                        {
                                pTmp->m_iNext = i;
                        } else {
                                m_poEventHeadAndColumnsIndex->add(p->m_iGroupID,i);
                        }
                } else {
                        m_poEventHeadAndColumnsIndex->add(p->m_iGroupID,i);
                }
                pTmp = p;
        }
        qry.close ();
        Log::log(0,"加载的EventHeadAndColumnsCfg关系[GroupID为主键]数据区使用量/总数[%d/%d];索引区使用量/总数[%d/%d]",m_poEventHeadAndColumnsCfg->getCount(),m_poEventHeadAndColumnsCfg->getTotal(),m_poEventHeadAndColumnsIndex->getCount(),m_poEventHeadAndColumnsIndex->getTotal());
}

//获得交换机对应的信息 ParamMgr::getSwitchInfo
void ParamInfoMgr::loadSwitchIDMapSwitchInfo()
{
        char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SWITICHIDMAPSWITCHINFO_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+316);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poSwitchIDMapSwitchData->exist())
                m_poSwitchIDMapSwitchData->create(CAL_COUNT_E(num));
        if(!m_poSwitchIDMapSwitchIndex->exist())
                m_poSwitchIDMapSwitchIndex->create(CAL_COUNT_E(num));
		checkMem(m_poSwitchIDMapSwitchData->getCount() * sizeof(TSwitchInfo));
        TSwitchInfo *pList = (TSwitchInfo*)(*m_poSwitchIDMapSwitchData);
        TSwitchInfo *p = 0;
        TSwitchInfo *pTmp = 0;
        char m_sSql[1024] = {0};
        DEFINE_QUERY (qry);
        strcpy(m_sSql,"select a.switch_id,a.source_event_type,a.switch_type_id,b.map_id,a.switch_long_type,a.area_code from b_switch_info a ,b_template_map_switch b where a.switch_type_id = b.switch_type_id order by a.switch_type_id");
        qry.setSQL (m_sSql);
        qry.open ();
        while(qry.next())
        {
                unsigned int i = m_poSwitchIDMapSwitchData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+317);
                p = pList+i;
                memset(p,'\0',sizeof(TSwitchInfo));
                p->m_iSwitchID = qry.field(0).asInteger();
                p->m_iSourceEventType = qry.field(1).asInteger();
                p->m_iSwitchTypeID = qry.field(2).asInteger();
                p->m_iMapID = qry.field(3).asInteger();
                p->m_iSwitchLongType = qry.field(4).asInteger();
                strcpy(p->m_sAreaCode,qry.field(5).asString());
                p->m_iNext = 0;
                if(pTmp)
                {
                        if(p->m_iSwitchTypeID == pTmp->m_iSwitchTypeID)
                        {
                                pTmp->m_iNext = i;
                        } else {
                                m_poSwitchIDMapSwitchIndex->add(p->m_iSwitchTypeID,i);
                        }
                } else {
                        m_poSwitchIDMapSwitchIndex->add(p->m_iSwitchTypeID,i);
                }
                pTmp = p;
        }
        qry.close ();
        Log::log(0,"加载的SwitchIDMapSwitch关系[SwitchTypeID为主键]数据区使用量/总数[%d/%d];索引区使用量/总数[%d/%d]",m_poSwitchIDMapSwitchData->getCount(),m_poSwitchIDMapSwitchData->getTotal(),m_poSwitchIDMapSwitchIndex->getCount(),m_poSwitchIDMapSwitchIndex->getTotal());
}

void ParamInfoMgr::loadFieldCheckMethod()
{
        char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FIELD_CHECK_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+318);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poFieldCheckMethodData->exist())
                m_poFieldCheckMethodData->create(CAL_COUNT_E(num));
        if(!m_poFieldCheckMethodIndex->exist())
                m_poFieldCheckMethodIndex->create(CAL_COUNT_E(num));
		checkMem(m_poFieldCheckMethodData->getCount() * sizeof(FieldCheckMethod));
        FieldCheckMethod *pList = (FieldCheckMethod*)(*m_poFieldCheckMethodData);
        FieldCheckMethod *p = 0;
        FieldCheckMethod *pTmp = 0;
        char m_sSql[1024] = {0};
        DEFINE_QUERY (qry);
        strcpy(m_sSql,"select t.event_attr_id,t.check_method_id,t.group_id from B_FIELD_CHECK t order by t.group_id ");
        qry.setSQL (m_sSql);
        qry.open ();
        while(qry.next())
        {
                unsigned int i = m_poFieldCheckMethodData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+319);
                p = pList+i;
                memset(p,'\0',sizeof(FieldCheckMethod));
                p->m_iEventAttrID = qry.field(0).asInteger();
                p->m_iCheckMethod = qry.field(1).asInteger();
                p->m_iGroupID = qry.field(2).asInteger();
                p->m_iNext = 0;
                if(pTmp)
                {
                        if(p->m_iGroupID == pTmp->m_iGroupID)
                        {
                                pTmp->m_iNext = i;
                        } else {
                                m_poFieldCheckMethodIndex->add(p->m_iGroupID,i);
                        }
                } else {
                        m_poFieldCheckMethodIndex->add(p->m_iGroupID,i);
                }
                pTmp = p;
        }
        qry.close ();
        Log::log(0,"加载的FieldCheckMethod关系[GroupID为主键]数据区使用量/总数[%d/%d];索引区使用量/总数[%d/%d]",m_poFieldCheckMethodData->getCount(),m_poFieldCheckMethodData->getTotal(),m_poFieldCheckMethodIndex->getCount(),m_poFieldCheckMethodIndex->getTotal());
}

//
void ParamInfoMgr::loadFieldValueMap()
{
        char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FIELD_MAP_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+320);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poFieldMapData->exist())
                m_poFieldMapData->create(CAL_COUNT_E(num));
        if(!m_poFieldMapIndex->exist())
                m_poFieldMapIndex->create(CAL_COUNT_E(num));
		checkMem(m_poFieldMapData->getCount() * sizeof(ValueMap));
        ValueMap *pList = (ValueMap*)(*m_poFieldMapData);
        ValueMap *p = 0;
        ValueMap *pTmp = 0;
        char m_sSql[1024] = {0};
        DEFINE_QUERY (qry);
        strcpy(m_sSql,"select t.event_attr_id,t.org_value,t.dest_value,t.group_id from B_FIELD_VALUE_MAP t order by t.group_id ");
        qry.setSQL (m_sSql);
        qry.open ();
        while(qry.next())
        {
                unsigned int i = m_poFieldMapData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+321);
                p = pList+i;
                memset(p,'\0',sizeof(ValueMap));
                p->m_iEventAttrID = qry.field(0).asInteger();
                strcpy(p->m_sOldValue , qry.field(1).asString());
                strcpy(p->m_sNewValue , qry.field(2).asString());
                p->m_iGroupID = qry.field(3).asInteger();
                p->m_iNext = 0;
                if(pTmp)
                {
                        if(p->m_iGroupID == pTmp->m_iGroupID)
                        {
                                pTmp->m_iNext = i;
                        } else {
                                m_poFieldMapIndex->add(p->m_iGroupID,i);
                        }
                } else {
                        m_poFieldMapIndex->add(p->m_iGroupID,i);
                }
                pTmp = p;
        }
        qry.close ();
        Log::log(0,"加载的FieldMap关系[GroupID为主键]数据区使用量/总数[%d/%d];索引区使用量/总数[%d/%d]",m_poFieldMapData->getCount(),m_poFieldMapData->getTotal(),m_poFieldMapIndex->getCount(),m_poFieldMapIndex->getTotal());
}

void ParamInfoMgr::loadFieldMerge()
{
        char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_FIELD_MERGE_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+322);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poFieldMergeData->exist())
                m_poFieldMergeData->create(CAL_COUNT_E(num));
        if(!m_poFieldMergeIndex->exist())
                m_poFieldMergeIndex->create(CAL_COUNT_E(num));
		checkMem(m_poFieldMergeData->getCount() * sizeof(Field));
        Field *pList = (Field*)(*m_poFieldMergeData);
        Field *p = 0;
        Field *pTmp = 0;
        char m_sSql[1024] = {0};
        DEFINE_QUERY (qry);
        strcpy(m_sSql,"select t.event_attr_id , t.event_attr_id1,t.event_attr_id2,t.data_format_id,t.group_id  from B_FIELD_MERGE t order by t.group_id ");
        qry.setSQL (m_sSql);
        qry.open ();
        while(qry.next())
        {
                unsigned int i = m_poFieldMergeData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+323);
                p = pList+i;
                memset(p,'\0',sizeof(Field));
                p->m_iEventAttrID = qry.field(0).asInteger();
                p->m_iEventAttrID1 = qry.field(1).asInteger();
                p->m_iEventAttrID2 = qry.field(2).asInteger();
                p->m_iDataFormatID = qry.field(3).asInteger();
                p->m_iGroupID = qry.field(4).asInteger();
                p->m_iNext = 0;
                if(pTmp)
                {
                        if(p->m_iGroupID == pTmp->m_iGroupID)
                        {
                                pTmp->m_iNext = i;
                        } else {
                                m_poFieldMergeIndex->add(p->m_iGroupID,i);
                        }
                } else {
                        m_poFieldMergeIndex->add(p->m_iGroupID,i);
                }
                pTmp = p;
        }
        qry.close ();
        Log::log(0,"加载的FieldMerge关系[GroupID为主键]数据区使用量/总数[%d/%d];索引区使用量/总数[%d/%d]",m_poFieldMergeData->getCount(),m_poFieldMergeData->getTotal(),m_poFieldMergeIndex->getCount(),m_poFieldMergeIndex->getTotal());
}

void ParamInfoMgr::loadTap3FileInfo()
{
        char countbuf[32]= {0};
        int num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TAP3_FILE_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+324);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poTap3FileData->exist())
                m_poTap3FileData->create(CAL_COUNT_E(num));
		checkMem(m_poTap3FileData->getCount() * sizeof(Tap3File));
        Tap3File *pFileList = (Tap3File*)(*m_poTap3FileData);
        countbuf[0]= '\0';
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TAP3_BLOCK_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+325);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poTap3BlockInfoData->exist())
                m_poTap3BlockInfoData->create(CAL_COUNT_E(num));
        Tap3BlockInfo *pBlockList = (Tap3BlockInfo*)(*m_poTap3BlockInfoData);
        checkMem(m_poTap3BlockInfoData->getCount() * sizeof(Tap3BlockInfo));
        Tap3File *p = 0;
        char m_sSql[1024] = {0};
        DEFINE_QUERY (qry);
        strcpy(m_sSql,"select filetypeid,filerule,headtag,tailtag,roottag from b_tap3file");
        qry.setSQL(m_sSql);
        qry.open();
        while(qry.next())
        {
                unsigned int i = m_poTap3FileData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+326);
                p = pFileList+i;
                memset(p,'\0',sizeof(Tap3File));
                p->iFileTypeID  = qry.field(0).asInteger();
                strcpy(p->sFileRule,qry.field(1).asString());
                p->iHeadTag     = qry.field(2).asInteger();
                p->iTailTag     = qry.field(3).asInteger();
                p->iRootTag = qry.field(4).asInteger();
        }
        qry.close ();
        
        Tap3BlockInfo *pBlockInfo = 0;
        memset(m_sSql,'\0',strlen(m_sSql));
        strcpy(m_sSql,"select filetypeid,blocktype,tag,eventattrid,type from b_tap3blockinfo");
        qry.setSQL(m_sSql);
        qry.open();
        while(qry.next())
        {
                unsigned int i = m_poTap3BlockInfoData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+327);
                pBlockInfo = pBlockList+i;
                memset(pBlockInfo,'\0',sizeof(Tap3BlockInfo));
                pBlockInfo->iFileTypeID = qry.field(0).asInteger();
                pBlockInfo->iBlockType  = qry.field(1).asInteger();
                pBlockInfo->iTag                = qry.field(2).asInteger();
                pBlockInfo->iAttrID             = qry.field(3).asInteger();
                pBlockInfo->iType               = qry.field(4).asInteger();
        }
        qry.close ();
        Log::log(0,"加载的b_tap3file数据区使用量/总数[%d/%d]",m_poTap3FileData->getCount(),m_poTap3FileData->getTotal());
        Log::log(0,"加载的b_tap3blockinfo数据区使用量/总数[%d/%d]",m_poTap3BlockInfoData->getCount(),m_poTap3BlockInfoData->getTotal());
}

void ParamInfoMgr::loadSepFile()
{
        char countbuf[1024]= {0};
        int  num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SEP_FILE_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+328);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poSepFileData->exist())
                m_poSepFileData->create(CAL_COUNT_E(num));
        SepFile *pSepList = (SepFile*)(*m_poSepFileData);
        checkMem(m_poSepFileData->getCount() * sizeof(SepFile));
        countbuf[0]= '\0';
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_SEP_BLOCK_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+329);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poSepBlockInfoData->exist())
                m_poSepBlockInfoData->create(CAL_COUNT_E(num));
        SepBlockInfo *pSepBlockList = (SepBlockInfo*)(*m_poSepBlockInfoData);
        checkMem(m_poSepBlockInfoData->getCount() * sizeof(SepBlockInfo));
        SepFile *p = 0;
        char m_sSql[1024] = {0};
        DEFINE_QUERY (qry);
        strcpy(m_sSql,"select filetypeid,filerule,have_head,have_tail,seprate_flag from b_sepfile order by length(filerule) desc");
        qry.setSQL(m_sSql);
        qry.open();
        while(qry.next())
        {
                unsigned int i = m_poSepFileData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+330);
                p = pSepList+i;
                memset(p,'\0',sizeof(SepFile));
                p->iFileTypeID  = qry.field(0).asInteger();
                strcpy(p->sFileRule,qry.field(1).asString());
                p->iHaveHead = qry.field(2).asInteger();
                p->iHaveTail = qry.field(3).asInteger();
                strcpy(p->sSeprateFlag,qry.field(4).asString());
        }
        qry.close ();   
        
        SepBlockInfo *pBlockInfo = 0;
        m_sSql[0] = '\0';
        strcpy(m_sSql,"select filetypeid,blocktype,seq,eventattrid,type from b_sepblockinfo");
        qry.setSQL(m_sSql);
        qry.open();
        while(qry.next())
        {
                unsigned int i = m_poSepBlockInfoData->malloc();
                if(i == 0 )
                        THROW(MBC_ParamInfoMgr+331);
                pBlockInfo = pSepBlockList+i;
                memset(pBlockInfo,'\0',sizeof(SepBlockInfo));
                pBlockInfo->iFileTypeID = qry.field(0).asInteger();
                pBlockInfo->iBlockType  = qry.field(1).asInteger();
                pBlockInfo->iSeq                = qry.field(2).asInteger();
                pBlockInfo->iAttrID     = qry.field(3).asInteger();
                pBlockInfo->iType               = qry.field(4).asInteger();
        }
        qry.close ();   
        Log::log(0,"加载的b_sepfile数据区使用量/总数[%d/%d]",m_poSepFileData->getCount(),m_poSepFileData->getTotal());
        Log::log(0,"加载的b_sepblockinfo数据区使用量/总数[%d/%d]",m_poSepBlockInfoData->getCount(),m_poSepBlockInfoData->getTotal());
}

bool ParamInfoMgr::getBillingCycleOffsetTime(int iOffset,char * strDate,TOCIQuery &qry)
{
    Date d;
	char sDate[16];
	memset(sDate,0,sizeof(sDate));
	strcpy(sDate,d.toString());
	BillingCycleMgr * pBCM = new BillingCycleMgr();
	if (!pBCM){
		Log::log (0, "获取帐期管理类失败，默认使用当前时间[%s]进行偏移过滤",sDate);
	}
	if(pBCM){
		BillingCycle * pCycle = pBCM->getBillingCycle (1, sDate);
		if(!pCycle){
			Log::log (0, "获取当前帐期失败，默认使用当前时间[%s]进行偏移过滤",sDate);
		}
		strcpy(sDate,pCycle->getStartDate());
	}
    char sql[1024];
    memset(sql,0,sizeof(sql));
    iOffset = iOffset -1;
    sprintf(sql, "select to_char(add_months(cycle_begin_date,-%d),'yyyymmddhh24miss') begin_date FROM billing_cycle where "
            " billing_cycle_type_id=1 AND to_date(%s,'yyyymmddhh24miss')>=cycle_begin_date and "
            " to_date(%s,'yyyymmddhh24miss')<cycle_end_date ",iOffset,sDate,sDate);
    qry.setSQL (sql);
    qry.open ();
    qry.next();
    strcpy(strDate,qry.field(0).asString());
    qry.commit();
    qry.close ();
	if(pBCM){
		delete pBCM;
		pBCM = NULL;
	}
	if(strlen(strDate) != 14){
		strDate[0]='\0';
	}
    return true;
}


//  
void ParamInfoMgr::loadMServAcct()
{		
    int num;
    char sql[2048] = {0};
    MServAcctInfo *p = 0;
    unsigned int i, lasti=0, k;
    long lastid = 0;
    char state[4];
    unsigned int  iTemp;
    unsigned int *pTemp = NULL;
    unsigned int iUpState, iNext,iNextTemp, iUp=0;
    int iZero=0;
	DEFINE_QUERY (qry);
	qry.close();
    char countbuf[32] = {0};
    if (!ParamDefineMgr::getParam(USERINFO_LIMIT, USERINFO_NUM_SERVACCT, countbuf,qry)) {
        num = 0;
    } else {
        num = atoi(countbuf);
    }
	
	char strDate[16];
    memset(strDate,0,sizeof(strDate));
    if (num>0){
        getBillingCycleOffsetTime(num,strDate,qry);
    }
	
    memset(countbuf,'\0',sizeof(countbuf));
    if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_MSERVACCT_INFO_NUM, countbuf))
    {
    	THROW(MBC_ParamInfoMgr+507);
    }
	num = atoi(countbuf);
	if(!m_poMServAcctInfoData->exist())
    	m_poMServAcctInfoData->create (CAL_COUNT(num));
    p = (MServAcctInfo *)(*m_poMServAcctInfoData);
	if(!m_poMServAcctIndex->exist())
    	m_poMServAcctIndex->create (CAL_COUNT(num));
	checkMem(m_poMServAcctInfoData->getCount() * sizeof(MServAcctInfo));
    strcpy (sql,
            "select a.serv_acct_id,a.serv_id, a.acct_id, a.item_group_id, a.eff_date, a.exp_date,a.state_date, a.state  "
            " ,a.billing_mode_id,a.share_flag from m_serv_acct a, serv b where a.serv_id = b.serv_id");	
    qry.setSQL(sql);
	qry.open();
    int iState;
    while(qry.next()) 
	{
        strcpy (state, qry.field(7).asString ());
        if ((state[2]|0X20) == 'a') {
            iState = 1;
        } else {
            iState = 0;
        }
		//
        if (!iState && strcmp(qry.field(6).asString(),strDate)<0) {
            continue;
        }
        i = m_poMServAcctInfoData->malloc ();
        if (!i)
			THROW(MBC_ParamInfoMgr+12);
		
        p[i].m_lServAcctID = qry.field(0).asLong();
        p[i].m_lServID = qry.field(1).asLong ();
        p[i].m_lAcctID = qry.field(2).asLong ();
        p[i].m_iAcctItemGroup = qry.field(3).asInteger ();
        p[i].m_iState = iState;
		p[i].m_iNextOffset = 0;
        strcpy (p[i].m_sEffDate, qry.field (4).asString());
		strcpy (p[i].m_sExpDate, qry.field (5).asString());
		if(p[i].m_sExpDate[0] == '\0')
		{
        	if (strcmp(p[i].m_sEffDate,qry.field(6).asString()) == 0) 
			{
            	strcpy (p[i].m_sExpDate, "20500101000000");
        	} else {
            	strcpy(p[i].m_sExpDate,qry.field(6).asString());
        	}
		}
		
		p[i].m_iBillingModeID = qry.field(8).asInteger();
		p[i].m_iShareFlag = qry.field(9).asInteger();
        if(m_poMServAcctIndex->get(p[i].m_lServID, &k)) 
		{
            unsigned int *pInt = &(p[k].m_iNextOffset);
			if(*pInt)
			{
            	while (*pInt) 
				{
                	if (strcmp (p[i].m_sEffDate, p[*pInt].m_sEffDate) >=0) 
					{
                    	p[i].m_iNextOffset = *pInt;
                    	*pInt = i;
                    	break;
                	}
                	pInt = &(p[*pInt].m_iNextOffset);
            	}
			} else {
                p[i].m_iNextOffset = 0;
                *pInt = i;
			}
        } else {
        	m_poMServAcctIndex->add (p[i].m_lServID, i);
		}
    }
    qry.close ();
    Log::log(0,"MServAcct 数据上载数目/总数[%d/%d] MServAcct 索引上载数目/总数[%d/%d] ",m_poMServAcctInfoData->getCount(),m_poMServAcctInfoData->getTotal(),m_poMServAcctIndex->getCount(),m_poMServAcctIndex->getTotal());   
}
void ParamInfoMgr::loadMergeRule()
{
        char countbuf[1024]= {0};
        int  num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_MERGE_RULE_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+339);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poMergeRuleData->exist())
                m_poMergeRuleData->create(CAL_COUNT_E(num));
		if(!m_poMergeRuleAssociateData->exist())
			m_poMergeRuleAssociateData->create(CAL_COUNT_E(num));
        if(!m_poMergeRuleStrIndex->exist())
                m_poMergeRuleStrIndex->create(CAL_COUNT_E(num),30);
		if(!m_poMergeRuleAssociateStrIndex->exist())
			m_poMergeRuleAssociateStrIndex->create(CAL_COUNT_E(num),30);
        checkMem(m_poMergeRuleData->getCount() * sizeof(MergeRuleData));
        MergeRuleData *pList = (MergeRuleData*)(*m_poMergeRuleData);
        MergeRuleData *pRuleData = 0;
        DEFINE_QUERY(qry);
        //MergeRuleData oRuleData;
        qry.close();
        string strSQL;
        // 读取合并规则
        strSQL = "SELECT E.ROWNUM1 -1 ROWNUM1 , A.RULE_ID RULE_ID, A.SERIAL SERIAL, B.OUT_ID OUT_ID, \
                 B.KEY_ID KEY_ID, NVL(B.FIRST_ID,-1) FIRST_ID, \
                 nvl(B.LAST_ID,-1) LAST_ID, NVL(B.JUDGE_ID,-1) JUDGE_ID, \
                 nvl(B.SERIAL_ATTR,0) SERIAL_ATTR, B.INTERVAL INTERVAL, \
                 C.SWITCH_ID SWITCH_ID, D.CALL_TYPE CALL_TYPE_ID \
                 FROM B_MERGE_MERGERULE A, B_MERGE_RULE_ATTR B, B_MERGE_EVENTSOURCE C, B_CALLTYPE D, \
                 (   SELECT  RULE_ID,ROWNUM ROWNUM1 FROM \
                                                (select DISTINCT(RULE_ID) FROM B_MERGE_MERGERULE ORDER BY RULE_ID )  ) E \
                                        WHERE A.ATTR_ID = B.ATTR_ID AND \
                                        A.EVENTSOURCE_ID = C.EVENTSOURCE_ID AND \
                                        C.CALL_TYPE_ID = D.CALL_TYPE_ID \
                                        AND A.RULE_ID = E.RULE_ID ORDER BY RULE_ID, SWITCH_ID,CALL_TYPE_ID  ";
        
        qry.setSQL( strSQL.c_str() );
        qry.open();
        while (qry.next())
        {
                //MergeRuleData*  pRuleData = new MergeRuleData();
                unsigned int i = m_poMergeRuleData->malloc();
                if(i == 0)
                        THROW(MBC_ParamInfoMgr+340);
                pRuleData = pList + i;
                memset(pRuleData,'\0',sizeof(MergeRuleData));
                pRuleData->m_iRuleID     = qry.field( "RULE_ID" ).asInteger();
                pRuleData->m_iSerial     = qry.field( "SERIAL" ).asInteger();
                pRuleData->m_iOutRuleID  = qry.field( "OUT_ID" ).asInteger();
                pRuleData->m_iKeyRuleID  = qry.field( "KEY_ID" ).asInteger();
                pRuleData->m_iRuleJudge  = qry.field( "JUDGE_ID" ).asInteger();
                pRuleData->m_iInterval   = qry.field( "INTERVAL" ).asInteger();
                pRuleData->m_iSwitchID   = qry.field( "SWITCH_ID" ).asInteger();
                pRuleData->m_iCallTypeID = qry.field( "CALL_TYPE_ID" ).asInteger();
                pRuleData->m_iRuleFirst  = qry.field( "FIRST_ID" ).asInteger();
                pRuleData->m_iRuleLast   = qry.field( "LAST_ID" ).asInteger();
                pRuleData->m_iSerialAttr = qry.field( "SERIAL_ATTR" ).asInteger();
                pRuleData->m_iPos        = qry.field( "ROWNUM1").asInteger();   
                pRuleData->m_iNext = 0;
                char sKey[256]={0};
                sprintf(sKey,"%d#%d",pRuleData->m_iSwitchID,pRuleData->m_iCallTypeID);
                unsigned int iIdx = 0;
                if(!m_poMergeRuleStrIndex->get(sKey,&iIdx))
                {
                        m_poMergeRuleStrIndex->add(sKey,i);
                } else {
                        pRuleData->m_iNext = iIdx;

                        m_poMergeRuleStrIndex->add(sKey,i);
                }
                
        }
        qry.close();
        
        // 读取关联规则
		MergeRuleData *pAssociateList = (MergeRuleData*)(*m_poMergeRuleAssociateData);
        string strSQL_link  = "SELECT E.ROWNUM1 -1 ROWNUM1, A.RULE_ID RULE_ID, A.SERIAL SERIAL, B.OUT_ID OUT_ID, \
                                 B.KEY_ID KEY_ID, NVL(B.JUDGE_ID,-1) JUDGE_ID, \
                                 B.INTERVAL INTERVAL, C.SWITCH_ID SWITCH_ID, \
                                 D.CALL_TYPE CALL_TYPE_ID \
                                 FROM B_MERGE_ASSOCIATERULE A, B_MERGE_RULE_ATTR B, B_MERGE_EVENTSOURCE C, B_CALLTYPE D ,\
                         (   SELECT  RULE_ID,ROWNUM ROWNUM1 FROM \
                                        (select DISTINCT(RULE_ID) FROM B_MERGE_ASSOCIATERULE ORDER BY RULE_ID )  ) E \
                                        WHERE A.ATTR_ID = B.ATTR_ID AND \
                                        A.EVENTSOURCE_ID = C.EVENTSOURCE_ID AND \
                                        C.CALL_TYPE_ID = D.CALL_TYPE_ID AND \
                                A.RULE_ID = E.RULE_ID ORDER BY RULE_ID,SWITCH_ID,CALL_TYPE_ID  ";
        qry.setSQL(strSQL_link.c_str());
        qry.open();
        while(qry.next())
        {
                unsigned int i = m_poMergeRuleAssociateData->malloc();
                if(i == 0)
                        THROW(MBC_ParamInfoMgr+340);
                pRuleData = pAssociateList + i;
                memset(pRuleData,'\0',sizeof(MergeRuleData));
                pRuleData->m_iRuleID     = qry.field( "RULE_ID" ).asInteger();
                pRuleData->m_iSerial     = qry.field( "SERIAL" ).asInteger();
                pRuleData->m_iOutRuleID  = qry.field( "OUT_ID" ).asInteger();
                pRuleData->m_iKeyRuleID  = qry.field( "KEY_ID" ).asInteger();
                pRuleData->m_iRuleJudge  = qry.field( "JUDGE_ID" ).asInteger();
                pRuleData->m_iInterval   = qry.field( "INTERVAL" ).asInteger();
                pRuleData->m_iSwitchID   = qry.field( "SWITCH_ID" ).asInteger();
                pRuleData->m_iCallTypeID = qry.field( "CALL_TYPE_ID" ).asInteger();
                pRuleData->m_iPos        = qry.field( "ROWNUM1").asInteger();
                pRuleData->m_iNext = 0;
                char sKey[256]={0};
                sprintf(sKey,"%d#%d",pRuleData->m_iSwitchID,pRuleData->m_iCallTypeID);
                unsigned int iIdx = 0;
                if(!m_poMergeRuleAssociateStrIndex->get(sKey,&iIdx))
                {
                        m_poMergeRuleAssociateStrIndex->add(sKey,i);
                } else {
                        //MergeRuleData*  pRulTmp = pList + iIdx;
                        pRuleData->m_iNext = iIdx;
                        m_poMergeRuleAssociateStrIndex->add(sKey,i);
                }
        }
		qry.close();
        
        countbuf[0]= '\0';
        num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_MERGE_RULE_OUT_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+341);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poMergeRuleOutData->exist())
                m_poMergeRuleOutData->create(CAL_COUNT_E(num));
        if(!m_poMergeRuleOutIndex->exist())
                m_poMergeRuleOutIndex->create(CAL_COUNT_E(num));
		checkMem(m_poMergeRuleOutData->getCount() * sizeof(MergeRuleData::ruleOut));
        MergeRuleData::ruleOut *pRuleOutList = (MergeRuleData::ruleOut *)(*m_poMergeRuleOutData);
        MergeRuleData::ruleOut *pRuleOut = 0;
        MergeRuleData::ruleOut *pOutTmp = 0;
        DEFINE_QUERY(qry_rule);
        //OUT RULE 获取输出规则
        qry_rule.close();
        qry_rule.setSQL( " SELECT B.FUNC_ID FUNC_ID, B.PARAM PARAM, "
                                         " B.EVENT_ATTR EVENT_ATTR, "
                                         " NVL(B.CONDITION_ID,0) CONDITION_ID, "
                                         " NVL(A.OUT_ID,0) OUT_ID " 
                                         " FROM B_MERGE_RULEOUT_SET A, B_MERGE_RULEOUT_ATTR B "
                                         " WHERE A.ATTR_ID = B.ATTR_ID "
                                         " ORDER BY A.OUT_ID,B.EVENT_ATTR " );
        qry_rule.open();
        unsigned int iIdx = 0;          
        while ( qry_rule.next() )
        {
                unsigned int i = m_poMergeRuleOutData->malloc();
                if(i == 0)
                        THROW(MBC_ParamInfoMgr+342);
                pRuleOut = pRuleOutList + i;
                memset(pRuleOut,'\0',sizeof(MergeRuleData::ruleOut));
                pRuleOut->iAttrID       = qry_rule.field( "EVENT_ATTR" ).asInteger();
                pRuleOut->iConditionID  = qry_rule.field( "CONDITION_ID" ).asInteger();
                pRuleOut->iFuncID       = qry_rule.field( "FUNC_ID" ).asInteger();
                char sParam[1024] = {0};
                strcpy(sParam,qry_rule.field( "PARAM" ).asString());
                pRuleOut->strParam      = string(sParam);
                pRuleOut->m_iOutRuleID =  qry_rule.field( "OUT_ID" ).asInteger();
                pRuleOut->m_iNext = 0;
                if(pOutTmp)
                {
                        if(pOutTmp->m_iOutRuleID == pRuleOut->m_iOutRuleID)
                        {
                                pOutTmp->m_iNext = i;
                        } else {
                                m_poMergeRuleOutIndex->add(pRuleOut->m_iOutRuleID,i);
                        }
                } else {
                        m_poMergeRuleOutIndex->add(pRuleOut->m_iOutRuleID,i);
                }
                pOutTmp = pRuleOut;
        }
        qry_rule.close();
        
        countbuf[0]= '\0';
        num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_MERGE_KEY_RULE_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+343);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poMergeKeyRuleData->exist())
                m_poMergeKeyRuleData->create(CAL_COUNT_E(num));
        if(!m_poMergeKeyRuleIndex->exist())
                m_poMergeKeyRuleIndex->create(CAL_COUNT_E(num));
		checkMem(m_poMergeKeyRuleData->getCount() * sizeof(MergeKeyRule));
        MergeKeyRule *pKeyRule = 0;
        MergeKeyRule *pKeyRuleTmp = 0;
        MergeKeyRule *pKeyRuleList = (MergeKeyRule*)(*m_poMergeKeyRuleData);
        // KET RULE 获取关键字规则
        qry_rule.setSQL( "SELECT B.EVENT_ATTR EVENT_ATTR , "
                                         " A.KEY_ID KEY_ID "
                                         " FROM B_MERGE_RULEKEY_SET A, B_MERGE_RULEKEY_ATTR B "
                                         " WHERE A.ATTR_ID = B.ATTR_ID  "
                                         " order by A.KEY_ID,B.EVENT_ATTR " );
        qry_rule.open();

        int iCnt=0;
        while (qry_rule.next())
        {
                unsigned int i = m_poMergeKeyRuleData->malloc();
                if(i == 0)
                        THROW(MBC_ParamInfoMgr+343);
                pKeyRule = pKeyRuleList + i;
                memset(pKeyRule,'\0',sizeof(MergeKeyRule));
                pKeyRule->m_iKeyRuleID = qry_rule.field( "KEY_ID" ).asInteger();;
                pKeyRule->m_iEventAttr = qry_rule.field( "EVENT_ATTR" ).asInteger();
                pKeyRule->m_iNext = 0;
                if(pKeyRuleTmp)
                {
                        if(pKeyRuleTmp->m_iKeyRuleID == pKeyRule->m_iKeyRuleID)
                        {
                                pKeyRuleTmp->m_iNext = i;
                        } else {
                                m_poMergeKeyRuleIndex->add(pKeyRule->m_iKeyRuleID,i);
                        }
                } else {
                        m_poMergeKeyRuleIndex->add(pKeyRule->m_iKeyRuleID,i);
                }
                pKeyRuleTmp = pKeyRule;
                iCnt++;
        }
        Log::log(0,"MergeRuleData 数据上载数目/总数[%d/%d] MergeRuleData 索引上载数目/总数[%d/%d] ",m_poMergeRuleData->getCount(),m_poMergeRuleData->getTotal(),m_poMergeRuleStrIndex->getCount(),m_poMergeRuleStrIndex->getTotal());   
        Log::log(0,"MergeRuleOutData 数据上载数目/总数[%d/%d] MergeRuleOutData 索引上载数目/总数[%d/%d] ",m_poMergeRuleOutData->getCount(),m_poMergeRuleOutData->getTotal(),m_poMergeRuleOutIndex->getCount(),m_poMergeRuleOutIndex->getTotal());       
        Log::log(0,"MergeKeyRuleData 数据上载数目/总数[%d/%d] MergeKeyRuleData 索引上载数目/总数[%d/%d] ",m_poMergeKeyRuleData->getCount(),m_poMergeKeyRuleData->getTotal(),m_poMergeKeyRuleIndex->getCount(),m_poMergeKeyRuleIndex->getTotal());       
		Log::log(0,"MergeRuleAssociate 数据上载数目/总数[%d/%d] MergeRuleAssociate 索引上载数目/总数[%d/%d] ",m_poMergeRuleAssociateData->getCount(),m_poMergeRuleAssociateData->getTotal(),m_poMergeRuleAssociateStrIndex->getCount(),m_poMergeRuleAssociateStrIndex->getTotal());			
}

// 
void ParamInfoMgr::loadTransParam()
{
        char countbuf[1024]= {0};
        int  num = 0;
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TRANS_PEER_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+339);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poTransPeerData->exist())
                m_poTransPeerData->create(CAL_COUNT_E(num));
        //if(!m_poTransPeerStrIndex->exist())
        //      m_poTransPeerStrIndex->create(CAL_COUNT_E(num),16);
        if(!m_poTransPeerIndex->exist())
                m_poTransPeerIndex->create(CAL_COUNT_E(num));
        checkMem(m_poTransPeerData->getCount() * sizeof(TransPeer));
        TransPeer *pPeerList = (TransPeer*)(*m_poTransPeerData);
        TransPeer *pPeer = 0;
        TransPeer *pPeerTmp = 0;
        
        DEFINE_QUERY(qry);
        qry.close();
        string strSQL = "select peer_type_id , peer_id from b_trans_peer order by peer_id";     
        qry.setSQL(strSQL.c_str());
        qry.open();
        while(qry.next())
        {
                unsigned int i = m_poTransPeerData->malloc();
                if(i == 0)
                        THROW(MBC_ParamInfoMgr+343);
                pPeer = pPeerList + i;
                memset(pPeer,'\0',sizeof(TransPeer));
                pPeer->m_iPeerTypeID = qry.field("peer_type_id").asInteger();
        strcpy(pPeer->m_sMapKey,"PEER_TYPE_ID");
                pPeer->m_iPeerID = qry.field("peer_id").asInteger();
                if(pPeerTmp && pPeerTmp->m_iPeerID == pPeer->m_iPeerID)
                {
                        pPeerTmp->m_iNext = i;
                } else {
                        m_poTransPeerIndex->add(pPeer->m_iPeerID,i);
                }
        }
        qry.close();
        
        countbuf[0]= '\0';
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TRANS_TYPE_DEFINE_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+339);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poParamTypeDefineData->exist())
                m_poParamTypeDefineData->create(CAL_COUNT_E(num));
        if(!m_poParamTypeDefineStrIndex->exist())
                m_poParamTypeDefineStrIndex->create(CAL_COUNT_E(num),30);
		checkMem(m_poParamTypeDefineData->getCount() * sizeof(ParamTypeDefine));
        ParamTypeDefine *pPeerTypeList = (ParamTypeDefine*)(*m_poParamTypeDefineData);
        ParamTypeDefine *pPeerType = 0;
        ParamTypeDefine *pPeerTypeTmp = 0;
        string strSQL2 = " select trim(b.peer_param_name), a.param_value , b.check_type , a.peer_id , b.peer_type_id "
                                 " from b_trans_peer_param a,b_trans_peer_type_define b "
                                 " where a.PEER_PARAM_ID = b.PEER_PARAM_ID "
                                 " order by a.peer_id , b.peer_type_id , trim(b.peer_param_name) ";     
        qry.setSQL(strSQL2.c_str());
        qry.open();
        while(qry.next())
        {
                unsigned int i = m_poParamTypeDefineData->malloc();
                if(i == 0)
                        THROW(MBC_ParamInfoMgr+343);
                pPeerType = pPeerTypeList + i;
                memset(pPeerType,'\0',sizeof(ParamTypeDefine));
                
                char sTemp[400] = {0};
                strcpy(pPeerType->m_sPeerParamName,qry.field(0).asString());
                strncpy(sTemp,qry.field(1).asString(),sizeof(sTemp));
                pPeerType->m_iCheckType = qry.field(2).asInteger();
                pPeerType->mTransPeer.m_iPeerID = qry.field(3).asInteger();
                pPeerType->mTransPeer.m_iPeerTypeID = qry.field(4).asInteger();
                pPeerType->m_iNext = 0;
        if(pPeerType->m_iCheckType == 1)
                {
                //检查目录最后字符
                if( sTemp[strlen(sTemp)-1]!='/' )
                        strcat(sTemp, "/");
                        strncpy(pPeerType->m_sParamValue,sTemp,sizeof(pPeerType->m_sParamValue));
        } else {
                        strncpy(pPeerType->m_sParamValue,sTemp,sizeof(pPeerType->m_sParamValue));
        }
        
                char sKey[32] = {0};
                sprintf(sKey,"%d_%d",pPeerType->mTransPeer.m_iPeerID,pPeerType->mTransPeer.m_iPeerTypeID);
                if(pPeerTypeTmp && (pPeerType->mTransPeer.m_iPeerID == pPeerTypeTmp->mTransPeer.m_iPeerID) && (pPeerTypeTmp->mTransPeer.m_iPeerTypeID == pPeerType->mTransPeer.m_iPeerTypeID) )
                {
                        pPeerTypeTmp->m_iNext = i;
                } else {
                        m_poParamTypeDefineStrIndex->add(sKey,i);
                }
                strcpy(pPeerType->mTransPeer.m_sMapKey,sKey);
                pPeerTypeTmp = pPeerType;
        }
        qry.close();
        
        countbuf[0]= '\0';
        if(!ParamDefineMgr::getParam(LOADPARAMINFO, PARAM_TRANS_CONTPOINT_TYPEDEFINE_INFO_NUM, countbuf))
        {
                THROW(MBC_ParamInfoMgr+339);
        }else{
                num = atoi(countbuf);
        }
        if(!m_poContPointTypeDefineData->exist())
                m_poContPointTypeDefineData->create(CAL_COUNT_E(num));
        if(!m_poContPointTypeDefineStrIndex->exist())
                m_poContPointTypeDefineStrIndex->create(CAL_COUNT_E(num),32);
		checkMem(m_poContPointTypeDefineData->getCount() * sizeof(ContPointTypeDefine));
        ContPointTypeDefine *pContList = (ContPointTypeDefine*)(*m_poContPointTypeDefineData);
        ContPointTypeDefine *pCont = 0;
        ContPointTypeDefine *pContTmp = 0;
        string strSQL3 = " select trim(b.peer_param_name), a.param_value , b.check_type "
                                 " from b_trans_peer_cont_point a,b_trans_peer_type_define b "
                                 " where a.PEER_PARAM_ID = b.PEER_PARAM_ID"
                                 " and trim(b.peer_param_name) like 'CONT_POINT%'"
                                 " order by a.peer_id ,b.peer_type_id, trim(b.peer_param_name) ";       
        qry.setSQL(strSQL3.c_str());
        qry.open();
        while(qry.next())
        {
                unsigned int i = m_poContPointTypeDefineData->malloc();
                if(i == 0)
                        THROW(MBC_ParamInfoMgr+343);
                pCont = pContList + i;
                memset(pCont,'\0',sizeof(ContPointTypeDefine));
                
                char sTemp[400] = {0};
                strcpy(pCont->m_sPeerParamName,qry.field(0).asString());
                strncpy(sTemp,qry.field(1).asString(),sizeof(sTemp));
                pCont->m_iCheckType = qry.field(2).asInteger();
                pCont->mTransPeer.m_iPeerID = qry.field(3).asInteger();
                pCont->mTransPeer.m_iPeerTypeID = qry.field(4).asInteger();
                pCont->m_iNext = 0;
        
                char sKey[32] = {0};
                sprintf(sKey,"%d_%d",pCont->mTransPeer.m_iPeerID,pCont->mTransPeer.m_iPeerTypeID);
                if(pContTmp && (pCont->mTransPeer.m_iPeerID == pContTmp->mTransPeer.m_iPeerID) && (pContTmp->mTransPeer.m_iPeerTypeID == pCont->mTransPeer.m_iPeerTypeID) )
                {
                        pContTmp->m_iNext = i;
                } else {
                        m_poContPointTypeDefineStrIndex->add(sKey,i);
                }
                strcpy(pCont->mTransPeer.m_sMapKey,sKey);
                pContTmp = pCont;
                
        }
        qry.close();
        
        Log::log(0,"[Trans]TransPeer 数据上载数目/总数[%d/%d] TransPeer 索引上载数目/总数[%d/%d] ",m_poTransPeerData->getCount(),m_poTransPeerData->getTotal(),m_poTransPeerIndex->getCount(),m_poTransPeerIndex->getTotal());  
        Log::log(0,"[Trans]ParamTypeDefine 数据上载数目/总数[%d/%d] ParamTypeDefine 索引上载数目/总数[%d/%d] ",m_poParamTypeDefineData->getCount(),m_poParamTypeDefineData->getTotal(),m_poParamTypeDefineStrIndex->getCount(),m_poParamTypeDefineStrIndex->getTotal());        
        Log::log(0,"[Trans]ContPointTypeDefine 数据上载数目/总数[%d/%d] ContPointTypeDefine 索引上载数目/总数[%d/%d] ",m_poContPointTypeDefineData->getCount(),m_poContPointTypeDefineData->getTotal(),m_poContPointTypeDefineStrIndex->getCount(),m_poContPointTypeDefineStrIndex->getTotal());        
}


// 
bool ParamInfoMgr::updateSHMInfo(long lIPCKey,char *sMEMCfgTable,unsigned int iUsed,unsigned int iTotal,char *sDate)
{
		//先要知道用户信息
		char *pUserName;
    	char sHostName[200];
    	pUserName = getenv("LOGNAME");
    	int iHostID = IpcKeyMgr::getHostInfo(sHostName);
		//先根据b_ipc_cfg获取lIPCKey
		char sIPCName[256] = {0};
		char sIPCRemark[256] = {0};
		char m_sIPCKeySql[512] = {0};
        DEFINE_QUERY (qryKey);
		sprintf(m_sIPCKeySql," select  ipc_name ,nvl(remark,'') from  b_ipc_cfg where ipc_key=%ld and  billflow_id=%d and host_id=%d and sys_username='%s' ",lIPCKey,-1,iHostID,pUserName);
		qryKey.setSQL(m_sIPCKeySql); 
		qryKey.open(); 
		if(qryKey.next())
		{
			strcpy(sIPCName,qryKey.field(0).asString());
			strcpy(sIPCRemark,qryKey.field(1).asString());
		}
		qryKey.close(); 
		
        //获得SQL语句
        char m_sSql[2048] = {0};
        DEFINE_QUERY (qry);
        /*sprintf(m_sSql, " update b_shm_stat_log set MEM_CFG_TABLE=:MEM_CFG_TABLE , MEM_USED=:MEM_USED , MEM_TOTAL=:MEM_TOTAL , CREATE_DATE=:CREATE_DATE  where MEM_CFG_NAME=:MEM_CFG_NAME");
		qry.setParameter ("MEM_CFG_TABLE", sMEMCfgTable);
		qry.setParameter ("MEM_CFG_NAME", sMEMCfgName);
		qry.setParameter ("MEM_USED", iUsed);
		qry.setParameter ("MEM_TOTAL", iTotal);*/
		//MEM_CFG_NAME是b_ipc_cfg的ipc_name
		char *pMEMCfgName = 0;
		if(sIPCRemark[0] != '\0')
			pMEMCfgName = sIPCRemark;
		else
			pMEMCfgName = sIPCName;
		bool Insert = true;
		sprintf(m_sSql," select IPC_KEY from b_shm_stat_log where HOST_ID= %d and SYS_USERNAME = '%s' and MEM_CFG_NAME= '%s' ",iHostID,pUserName,sIPCName);
		qry.setSQL(m_sSql); 
		qry.open(); 
		if(qry.next())
		{
			Insert = false;
		}
		qry.close();
		
		// 
		m_sSql[0] = '\0';
		if(!Insert)
		{
		sprintf(m_sSql," update b_shm_stat_log set MEM_CFG_TABLE = '%s' "
				" , MEM_USED= %d , MEM_TOTAL= %d , IPC_KEY=%ld , "
				" MEM_POS_FLAG = %d ,MEM_REMARK = '%s' , CREATE_DATE = to_date(:CREATEDATE,'YYYYMMDDHH24MISS') where HOST_ID= %d and SYS_USERNAME = '%s' and MEM_CFG_NAME= '%s' ",sMEMCfgTable,iUsed,iTotal,lIPCKey,m_iNo,pMEMCfgName,iHostID,pUserName,sIPCName);
				qry.setSQL(m_sSql); 
				qry.setParameter("CREATEDATE",sDate);
				
		}		
		else {
				sprintf(m_sSql," insert into  b_shm_stat_log (MEM_CFG_NAME,HOST_ID,SYS_USERNAME,MEM_POS_FLAG,MEM_REMARK,IPC_KEY,MEM_USED,MEM_TOTAL,MEM_CFG_TABLE,CREATE_DATE) "
				" values ('%s',%d,'%s',%d,'%s',%ld,%d,%d,'%s',to_date(%s,'YYYYMMDDHH24MISS')) ",sIPCName,iHostID,pUserName,m_iNo,pMEMCfgName,lIPCKey,iUsed,iTotal,sMEMCfgTable,sDate );
				qry.setSQL(m_sSql); 
		}
		try
		{       
     		if(qry.execute())
	 		{
				qry.commit();
				qry.close();
				return true;
	 		} else {
				qry.rollback();
				qry.close();
				return false;
	 		}	
     	}catch(TOCIException& e) 
     	{
	 		Log::log(0,"commit data err ,table name [b_shm_stat_log]"); 
	 		qry.close(); 
	 		return false;
     	}
}

/*
//创建标准数据区
bool ParamInfoMgr::createStdData()
{
        int iTotalStdStructNum = getStdTotalNum();
        if(iTotalStdStructNum == 0)
        {
                THROW(MBC_ParamInfoMgr+42);
        }
        if(!(m_poStdChar8Data->exist()))//标准数据区
                m_poStdChar8Data->create(iTotalStdStructNum);  
        if(!(m_poStdChar8Index->exist()))//标准数据索引区
                m_poStdChar8Index->create(PARAM_STRUCT_NUM+16);
        if(!(m_poStdChar8DataDesc->exist()))//标准数据描述区
                m_poStdChar8DataDesc->create(PARAM_STRUCT_NUM+16);//这个就一个个比较就可以了,因为加载的时候效率没影响
        
        m_poStdList = (StdChar8*)(*m_poStdChar8Data);
        m_poStdChar8DataDescList = (StdChar8DataDesc*)(*m_poStdChar8DataDesc);
        
        return true;
}



//函数:
//功能:根据传的结构体信息[主要是结构体编号,结构体数据申请的数量]获取标准数据区的起始偏移
//参数说明:iStructTyeID 结构体编号[在src/mmdb/SHMStructType.h],iStructSize结构体大小,
//              sStructName结构体名称,
//              mDescUse结构体申请获得最大标准结构
//              [以及获得原来使用情况, 即如果这次使用的最大值,跟原来的不一样,但又没有删除重载,就按原来的最大计算]
//返回值:返回的偏移量就是标准数据区分配给这个结构体的标准偏移
//举例:

unsigned int ParamInfoMgr::getStdDataOffset(StdChar8DataDesc &mDescUse)
{       
        StdChar8DataDesc *pDescTmp = 0;
        //先判断是否已经有这个结构体的信息了
        bool resFindStruct = false;
        if(m_poStdChar8DataDesc->getCount() == 0)
                resFindStruct = false;
        else
        {
                for(unsigned int i = 1;i<=m_poStdChar8DataDesc->getCount();i++)
                {
                        pDescTmp = m_poStdChar8DataDescList+i;
                        if(pDescTmp->lStructID == mDescUse.lStructID)
                        {
                                resFindStruct = true;
                                break;
                        }
                }
        }
        if(resFindStruct)
        {
                mDescUse.iMaxNum = pDescTmp->iMaxNum;
                mDescUse.iIndexNum = pDescTmp->iIndexNum;
                memcpy(&mDescUse,pDescTmp,sizeof(struct StdChar8DataDesc));
        } else {
                unsigned int iMolloc = m_poStdChar8DataDesc->malloc();
                StdChar8DataDesc *pDesc = m_poStdChar8DataDescList+iMolloc;
        
                //unsigned int iTotalSize = iMaxNeedStdDataNum*STD_CHAR8_LEN;
                if(iMolloc)
                {
                        pDesc->lStructID = mDescUse.lStructID;
                        pDesc->sStructName[0] = '\0';
                        strcpy(pDesc->sStructName,mDescUse.sStructName);
                        pDesc->iStructSize = mDescUse.iStructSize;
                        pDesc->iMaxNum = mDescUse.iMaxNum;//最大相关结构体数目
                        pDesc->iIndexNum = mDescUse.iIndexNum;//对
                        pDesc->iUsedPos = 0;
                        mDescUse.iUsedPos = 0;
                } else {
                        Log::log(0,"数据区描述区空间不足,未能申请到空间");
                        THROW(MBC_ParamInfoMgr+197);
                }
        }
        
        unsigned int iOffset = 0;
        unsigned int iUsed = m_poStdChar8Data->getCount();
        unsigned int iTotal = m_poStdChar8Data->getTotal();
        unsigned int iUsedAfter = m_poStdChar8Data->MallocByNum(mDescUse.iMaxNum); 
        if(iUsedAfter == 0)
        {
                Log::log(0,"标准数据区的使用量/总量[%d/%d] ,空间不够申请数据量[%d],获取空间失败!",iUsed,iTotal,mDescUse.iMaxNum);
                //return 0;
                THROW(MBC_ParamInfoMgr+199);
        }
        unsigned int iStart = iUsed+1;
        unsigned int idx = 0;
        if(m_poStdChar8Index->get(mDescUse.lStructID,&idx))
        {
                m_poStdChar8Index->eraseData(mDescUse.lStructID);
        }
        m_poStdChar8Index->add(mDescUse.lStructID,iStart);
        return iStart;
}

void ParamInfoMgr::updateStdDesc(StdChar8DataDesc mStdDesc)
{
         StdChar8DataDesc *pDesc = 0;
         for(unsigned int i=1;i<=m_poStdChar8DataDesc->getCount();i++)
         {
                pDesc = m_poStdChar8DataDescList+i;
                if(pDesc->lStructID == mStdDesc.lStructID)
                {
                        pDesc->iUsedPos = mStdDesc.iUsedPos;
                }
         }
}

StdChar8DataDesc *ParamInfoMgr::findByType(long lStructType)
{
         StdChar8DataDesc *pDesc = 0;
         for(unsigned int i=1;i<=m_poStdChar8DataDesc->getCount();i++)
         {
                pDesc = m_poStdChar8DataDescList+i;
                if(pDesc->lStructID == lStructType)
                {
                        return pDesc;
                }
         }
         return NULL;
}

int ParamInfoMgr::getUsedNumByType(long lStructType)
{
         StdChar8DataDesc *pDesc = findByType(lStructType);
         if(pDesc)
                return pDesc->iUsedPos;
         else
                return 0;
}


int ParamInfoMgr::getTotalNumByType(long lStructType)
{
         StdChar8DataDesc *pDesc = findByType(lStructType);
         if(pDesc)
                return pDesc->iMaxNum;
         else
                return 0;
}


int ParamInfoMgr::loadTemplateMapSwitch()
{
        int res = -1;
        //对这个结构体信息的赋值,在跟原来记录比较后可能会有改动
        StdChar8DataDesc mStdDesc;
        mStdDesc.lStructID = TEMPLATE_MAP_SWITCH_SHMTYPE;
        mStdDesc.iStructSize = sizeof(struct TemplateMapSwitch);
        mStdDesc.sStructName[0] = '\0';
        strcpy(mStdDesc.sStructName,"TemplateMapSwitch");
        mStdDesc.iMaxNum = (m_iTotalTemplateMapSwitchNum*STD_CHAR8_LEN)/mStdDesc.iStructSize;   
        mStdDesc.iIndexNum = mStdDesc.iMaxNum;
        
        unsigned int iStart = getStdDataOffset(mStdDesc);
        if(iStart == 0)
        {
                Log::log(0,"加载的TemplateMapSwitch数据区失败,标准数据区空间申请失败!");
                return res;//没空间
        }
        TemplateMapSwitch *p = 0;
        p = (TemplateMapSwitch*)(&m_poStdList[iStart]);
        TemplateMapSwitch *pList = p;
        if(!m_poTemplateMapSwitchIndex->exist())
                m_poTemplateMapSwitchIndex->create(mStdDesc.iIndexNum);
        m_poStdChar8Index->add(mStdDesc.lStructID,iStart);
        //加载数据索引数据
        DEFINE_QUERY (qry) ;

        //获得SQL语句
        char m_sSql[1024] = {0};
        strcpy(m_sSql,"select map_id,switch_type_id,template_id,nvl(org_id,-1) from b_template_map_switch order by template_id");

        qry.setSQL (m_sSql);
        qry.open ();
        mStdDesc.iUsedPos = 0;
        TemplateMapSwitch *pTmp = 0;
        m_poStdChar8Index->add(mStdDesc.lStructID,iStart);
        while(qry.next())
        {
                if(mStdDesc.iUsedPos >= mStdDesc.iMaxNum)
                        THROW(MBC_ParamInfoMgr+300);            
                p->m_iMapID = qry.field(0).asInteger();
                p->m_iSwitchTypeID = qry.field(1).asInteger();
                p->m_iTemplateID = qry.field(2).asInteger();
                p->m_iOrgID = qry.field(3).asInteger();
                p->m_iNext = 0;
                if(pTmp)
                {
                        if(p->m_iTemplateID != pTmp->m_iTemplateID)
                        {
                                m_poTemplateMapSwitchIndex->add(p->m_iTemplateID,(unsigned int)mStdDesc.iUsedPos);
                        } else {
                                TemplateMapSwitch *pBefore = 0;
                                unsigned int iBefore = 0;
                                if(m_poTemplateMapSwitchIndex->get(p->m_iTemplateID,&iBefore))
                                {
                                        pBefore = pList+iBefore;
                                        while(pBefore->m_iNext)
                                        {
                                                pBefore = pList + (pBefore->m_iNext);
                                        }
                                        pBefore->m_iNext = mStdDesc.iUsedPos;
                                }
                        }
                } else {
                        m_poTemplateMapSwitchIndex->add(p->m_iTemplateID,(unsigned int)mStdDesc.iUsedPos);
                }
                pTmp = p;
                mStdDesc.iUsedPos++;
                p++;
        }
        qry.close();
        updateStdDesc(mStdDesc);
        Log::log(0,"加载的TemplateMapSwitch数据区使用量/总数[%d/%d]; 索引区使用量/总数[%d/%d]",mStdDesc.iUsedPos,mStdDesc.iMaxNum,m_poTemplateMapSwitchIndex->getCount(),mStdDesc.iMaxNum);
        
    return res;
}


void ParamInfoMgr::loadObjectDefine()
{
        //对这个结构体信息的赋值,在跟原来记录比较后可能会有改动
        StdChar8DataDesc mStdDesc;
        mStdDesc.lStructID = OBJECT_DEFINE_SHMTYPE;
        mStdDesc.iStructSize = sizeof(struct ObjectDefine);
        mStdDesc.sStructName[0] = '\0';
        strcpy(mStdDesc.sStructName,"ObjectDefine");
        mStdDesc.iMaxNum = (m_iTotalObjectDefineNum*STD_CHAR8_LEN)/mStdDesc.iStructSize;        
        mStdDesc.iIndexNum = mStdDesc.iMaxNum;
        
        unsigned int iStart = getStdDataOffset(mStdDesc);
        if(iStart == 0)
        {
                Log::log(0,"加载的TemplateMapSwitch数据区失败,标准数据区空间申请失败!");
                return ;//没空间
        }
        mStdDesc.iUsedPos = 0;
        ObjectDefine *p = 0;
        p = (ObjectDefine*)(&m_poStdList[iStart]);
        ObjectDefine *pList = p;
        int m_iCount = 0;
        ObjectDefine *pTmp = 0;
        if(!m_poObjectDefineIndex->exist())
                m_poObjectDefineIndex->create(mStdDesc.iIndexNum);
        DEFINE_QUERY (qry);
        
        //获得SQL语句
        char m_sSql[1024] = {0};
        strcpy(m_sSql,"select object_id,map_id,object_type,calc_method_id,param1,param2,param3,param4,param5 from b_object_define order by map_id");
        
        qry.setSQL (m_sSql);
        qry.open ();
        
        while(qry.next())
        {
                memset(p,'\0',sizeof(struct ObjectDefine));
                if(mStdDesc.iUsedPos >= mStdDesc.iMaxNum)
                        THROW(MBC_ParamInfoMgr+301);
                p->m_iObjectID = qry.field(0).asInteger();
                p->m_iMapID = qry.field(1).asInteger();
                p->m_iObjectType = (OBJECT_TYPE)qry.field(2).asInteger();
                p->m_iCalcMethodID = (CALC_BLOCK_METHOD)qry.field(3).asInteger();
                strcpy(p->m_sParam1,qry.field(4).asString());
                strcpy(p->m_sParam2,qry.field(5).asString());
                strcpy(p->m_sParam3,qry.field(6).asString());
                strcpy(p->m_sParam4,qry.field(7).asString());
                strcpy(p->m_sParam5,qry.field(8).asString());
                p->m_iNext = 0;
                if(pTmp)
                {
                        if(pTmp->m_iMapID != p->m_iMapID)
                        {
                                m_poObjectDefineIndex->add(p->m_iMapID,mStdDesc.iUsedPos);
                        } else {
                                pTmp->m_iNext = mStdDesc.iUsedPos;
                        }
                } else {
                        m_poObjectDefineIndex->add(p->m_iMapID,mStdDesc.iUsedPos);
                }
                pTmp = p;
                p++;
                mStdDesc.iUsedPos++;
        }

        qry.close ();
        updateStdDesc(mStdDesc);
        Log::log(0,"加载的ObjectDefine数据区使用量/总数[%d/%d]; 索引区使用量/总数[%d/%d]",mStdDesc.iUsedPos,mStdDesc.iMaxNum,m_poObjectDefineIndex->getCount(),mStdDesc.iMaxNum);
}


void ParamInfoMgr::loadEventParserGroup()
{
        //对这个结构体信息的赋值,在跟原来记录比较后可能会有改动
        StdChar8DataDesc mStdDesc;
        mStdDesc.lStructID = MAP_PARSER_GROUP_SHMTYPE;
        mStdDesc.iStructSize = sizeof(struct EventParserGroup);
        mStdDesc.sStructName[0] = '\0';
        strcpy(mStdDesc.sStructName,"EventParserGroup");
        mStdDesc.iMaxNum = (m_iTotalEventParserGroupNum*STD_CHAR8_LEN)/mStdDesc.iStructSize;    
        mStdDesc.iIndexNum = mStdDesc.iMaxNum;
        
        unsigned int iStart = getStdDataOffset(mStdDesc);
        if(iStart == 0)
        {
                Log::log(0,"加载的EventParserGroup数据区失败,标准数据区空间申请失败!");
                return ;//没空间
        }
        mStdDesc.iUsedPos = 0;
        EventParserGroup *p = 0;
        p = (EventParserGroup*)(&m_poStdList[iStart]);
        EventParserGroup *pList = p;
        int m_iCount = 0;
        EventParserGroup *pTmp = 0;
        if(!m_poEventParserGroupIndex->exist())
                m_poEventParserGroupIndex->create(mStdDesc.iIndexNum);
        DEFINE_QUERY (qry);
        
        //获得SQL语句
        char m_sSql[1024] = {0};
        strcpy(m_sSql,"select object_type,group_id,map_id from b_map_parser_group order by map_id");
        
        qry.setSQL (m_sSql);
        qry.open();
        
        while(qry.next())
        {
                memset(p,'\0',sizeof(struct EventParserGroup));
                if(mStdDesc.iUsedPos >= mStdDesc.iMaxNum)
                        THROW(MBC_ParamInfoMgr+301);
                p->m_iObjectType = qry.field(0).asInteger();
                p->m_iGroupID = qry.field(1).asInteger();
                p->m_iMapID = qry.field(2).asInteger();
                p->m_iNext = 0;
                if(pTmp)
                {
                        if(pTmp->m_iMapID != p->m_iMapID)
                        {
                                m_poEventParserGroupIndex->add(p->m_iMapID,mStdDesc.iUsedPos);
                        } else {
                                pTmp->m_iNext = mStdDesc.iUsedPos;
                        }
                } else {
                        m_poEventParserGroupIndex->add(p->m_iMapID,mStdDesc.iUsedPos);
                }
                pTmp = p;
                p++;
                mStdDesc.iUsedPos++;
        }

        qry.close ();
        updateStdDesc(mStdDesc);
        Log::log(0,"加载的ObjectDefine数据区使用量/总数[%d/%d]; 索引区使用量/总数[%d/%d]",mStdDesc.iUsedPos,mStdDesc.iMaxNum,m_poObjectDefineIndex->getCount(),mStdDesc.iMaxNum);
}
*/
/*
#ifdef EVENT_AGGR_RECYCLE_USE
        int ParamInfoMgr::m_iArrayLen = 0;
        int     ParamInfoMgr::m_iDays = 0;
#endif*/
#endif
